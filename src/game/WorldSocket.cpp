/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "WorldSocket.h"
#include "Common.h"

#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "ByteBuffer.h"
#include "Opcodes.h"
#include "Database/DatabaseEnv.h"
#include "Auth/Sha1.h"
#include "WorldSession.h"
#include "WorldSocketMgr.h"
#include "Log.h"
#include "DBCStores.h"

#include <chrono>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

using namespace std::chrono;

struct ServerPktHeader
{
    /**
    * size is the length of the payload _plus_ the length of the opcode
    */
    ServerPktHeader(uint32 size, uint16 cmd) : size(size)
    {
        uint8 headerIndex = 0;
        if (isLargePacket())
        {
            DEBUG_LOG("initializing large server to client packet. Size: %u, cmd: %u", size, cmd);
            header[headerIndex++] = 0x80 | (0xFF & (size >> 16));
        }
        header[headerIndex++] = 0xFF & (size >> 8);
        header[headerIndex++] = 0xFF & size;

        header[headerIndex++] = 0xFF & cmd;
        header[headerIndex++] = 0xFF & (cmd >> 8);
    }

    uint8 getHeaderLength()
    {
        // cmd = 2 bytes, size= 2||3bytes
        return 2 + (isLargePacket() ? 3 : 2);
    }

    bool isLargePacket()
    {
        return size > 0x7FFF;
    }

    const uint32 size;
    uint8 header[5];
};

struct ClientPktHeader
{
    bool IsValid() const
    {
        return (size >= 4) && (size <= 10240) && (cmd <= 10240);
    }

    void Convert()
    {
        EndianConvertReverse(size);
        EndianConvert(cmd);
    }

    uint16 size;
    uint32 cmd;
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

WorldSocket::WorldSocket(NetworkManager& socketMrg, NetworkThread& owner) : Socket(socketMrg, owner),
m_OverSpeedPings(0),
m_Session(0),
m_RecvWPct(0),
m_RecvPct(),
m_Header(sizeof(ClientPktHeader)),
m_Seed(static_cast<uint32>(rand32()))
{
}

WorldSocket::~WorldSocket(void)
{
    delete m_RecvWPct;
}

void WorldSocket::CloseSocket(void)
{
    {
        GuardType Guard(m_SessionLock);

        m_Session = nullptr;
    }
    Socket::CloseSocket();
}

bool WorldSocket::SendPacket(const WorldPacket& pct)
{
    if (IsClosed())
        return false;

    // Dump outgoing packet.
    sLog.outWorldPacketDump(native_handle(), pct.GetOpcode(), pct.GetOpcodeName(), &pct, false);

    GuardType Guard(m_outBufferLock);

    if (!AppendPacket(pct))
    {
        sLog.outError("network write buffer is too small to accommodate packet. Disconnecting client");
        return false;
    }
    StartAsyncSend();
    return true;
}

bool WorldSocket::Open()
{
    if (!Socket::Open())
        return false;

    // Send startup packet.
    WorldPacket packet(SMSG_AUTH_CHALLENGE, 40);
    packet << uint32(1);                                    // 1...31
    packet << m_Seed;

    BigNumber seed1;
    seed1.SetRand(16 * 8);
    packet.append(seed1.AsByteArray(16), 16);               // new encryption seeds

    BigNumber seed2;
    seed2.SetRand(16 * 8);
    packet.append(seed2.AsByteArray(16), 16);               // new encryption seeds

    return SendPacket(packet);
}

bool WorldSocket::ProcessIncomingData()
{
    while (m_readBuffer->length() > 0)
    {
        if (m_Header.space() > 0)
        {
            // need to receive the header
            const size_t to_header = (m_readBuffer->length() > m_Header.space() ? m_Header.space() : m_readBuffer->length());
            m_Header.Write(m_readBuffer->read_data(), to_header);
            m_readBuffer->Consume(to_header);

            if (m_Header.space() > 0)
            {
                // Couldn't receive the whole header this time.
                return true;
            }

            // We just received nice new header
            if (handle_input_header() == -1)
            {
                return false;
            }
        }
        // Its possible on some error situations that this happens
        // for example on closing when epoll receives more chunked data and stuff
        // hope this is not hack ,as proper m_RecvWPct is asserted around
        if (!m_RecvWPct)
        {
            sLog.outError("Forcing close on input m_RecvWPct = nullptr");
            return false;
        }


        // We have full read header, now check the data payload
        if (m_RecvPct.space() > 0)
        {
            // need more data in the payload
            const size_t to_data = (m_readBuffer->length() > m_RecvPct.space() ? m_RecvPct.space() : m_readBuffer->length());
            m_RecvPct.Write(m_readBuffer->read_data(), to_data);
            m_readBuffer->Consume(to_data);

            if (m_RecvPct.space() > 0)
            {
                return true;
            }
        }

        // just received fresh new payload
        if (handle_input_payload() == -1)
        {
            return false;
        }
    }

    return true;
}

int WorldSocket::handle_input_header(void)
{
    MANGOS_ASSERT(m_RecvWPct == nullptr);

    MANGOS_ASSERT(m_Header.length() == sizeof(ClientPktHeader));

    m_Crypt.DecryptRecv(m_Header.read_data(), sizeof(ClientPktHeader));

    ClientPktHeader& header = *((ClientPktHeader*)m_Header.read_data());

    header.Convert();

    if (!header.IsValid())
    {
        sLog.outError("WorldSocket::handle_input_header: client sent malformed packet size = %d , cmd = %d",
            header.size, header.cmd);

        errno = EINVAL;
        return -1;
    }

    header.size -= 4;

    m_RecvWPct = new WorldPacket((Opcodes)header.cmd, header.size);

    if (header.size > 0)
    {
        m_RecvWPct->resize(header.size);
        m_RecvPct.AssignBuffer((uint8*)m_RecvWPct->contents(), m_RecvWPct->size());
    }
    else
    {
        MANGOS_ASSERT(m_RecvPct.space() == 0);
    }

    return 0;
}

int WorldSocket::handle_input_payload(void)
{
    // set errno properly here on error !!!
    // now have a header and payload

    MANGOS_ASSERT(m_RecvPct.space() == 0);
    MANGOS_ASSERT(m_Header.space() == 0);
    MANGOS_ASSERT(m_RecvWPct != nullptr);

    const int ret = ProcessIncoming(m_RecvWPct);

    m_RecvPct.UnassignBuffer();
    m_RecvWPct = nullptr;

    m_Header.Reset();

    if (ret == -1)
        errno = EINVAL;

    return ret;
}

int WorldSocket::ProcessIncoming(WorldPacket* new_pct)
{
    // manage memory ;)
    std::auto_ptr<WorldPacket> aptr(new_pct);

    const uint16 opcode = new_pct->GetOpcode();

    if (opcode >= NUM_MSG_TYPES)
    {
        sLog.outError("SESSION: received nonexistent opcode 0x%.4X", opcode);
        return -1;
    }

    if (IsClosed())
        return -1;

    // Dump received packet.
    sLog.outWorldPacketDump(native_handle(), new_pct->GetOpcode(), new_pct->GetOpcodeName(), new_pct, true);

    try
    {
        switch (opcode)
        {
        case CMSG_PING:
            return HandlePing(*new_pct);
        case CMSG_AUTH_SESSION:
            if (m_Session)
            {
                sLog.outError("WorldSocket::ProcessIncoming: Player send CMSG_AUTH_SESSION again");
                return -1;
            }

            return HandleAuthSession(*new_pct);
        case CMSG_KEEP_ALIVE:
            DEBUG_LOG("CMSG_KEEP_ALIVE ,size: " SIZEFMTD " ", new_pct->size());

            return 0;
        default:
        {
            GuardType Guard(m_SessionLock);

            if (m_Session != nullptr)
            {
                // OK ,give the packet to WorldSession
                aptr.release();
                // WARNING here we call it with locks held.
                // Its possible to cause deadlock if QueuePacket calls back
                m_Session->QueuePacket(new_pct);
                return 0;
            }
            else
            {
                sLog.outError("WorldSocket::ProcessIncoming: Client not authed opcode = %u", uint32(opcode));
                return -1;
            }
        }
        }
    }
    catch (ByteBufferException&)
    {
        sLog.outError("WorldSocket::ProcessIncoming ByteBufferException occured while parsing an instant handled packet (opcode: %u) from client %s, accountid=%i.",
            opcode, GetRemoteAddress().c_str(), m_Session ? m_Session->GetAccountId() : -1);
        if (sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))
        {
            DEBUG_LOG("Dumping error-causing packet:");
            new_pct->hexlike();
        }

        if (sWorld.getConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET))
        {
            DETAIL_LOG("Disconnecting session [account id %i / address %s] for badly formatted packet.",
                m_Session ? m_Session->GetAccountId() : -1, GetRemoteAddress().c_str());

            return -1;
        }
        else
            return 0;
    }

    return 0;
}

int WorldSocket::HandleAuthSession(WorldPacket& recvPacket)
{
    // NOTE: ATM the socket is singlethread, have this in mind ...
    uint8 digest[20];
    uint32 clientSeed, id, security;
    uint32 ClientBuild;
    uint8 expansion = 0;
    LocaleConstant locale;
    std::string account;
    Sha1Hash sha1;
    BigNumber v, s, g, N, K;
    WorldPacket packet;

    // Read the content of the packet
    recvPacket >> ClientBuild;
    recvPacket.read_skip<uint32>();
    recvPacket >> account;
    recvPacket.read_skip<uint32>();
    recvPacket >> clientSeed;
    recvPacket.read_skip<uint32>();
    recvPacket.read_skip<uint32>();
    recvPacket.read_skip<uint32>();
    recvPacket.read_skip<uint64>();
    recvPacket.read(digest, 20);

    DEBUG_LOG("WorldSocket::HandleAuthSession: client build %u, account %s, clientseed %X",
        ClientBuild,
        account.c_str(),
        clientSeed);

    // Check the version of client trying to connect
    if (!IsAcceptableClientBuild(ClientBuild))
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_VERSION_MISMATCH);

        SendPacket(packet);

        sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (version mismatch).");
        return -1;
    }

    // Get the account information from the realmd database
    std::string safe_account = account; // Duplicate, else will screw the SHA hash verification below
    LoginDatabase.escape_string(safe_account);
    // No SQL injection, username escaped.

    QueryResult* result =
        LoginDatabase.PQuery("SELECT "
        "id, "                      //0
        "gmlevel, "                 //1
        "sessionkey, "              //2
        "last_ip, "                 //3
        "locked, "                  //4
        "v, "                       //5
        "s, "                       //6
        "expansion, "               //7
        "mutetime, "                //8
        "locale "                   //9
        "FROM account "
        "WHERE username = '%s'",
        safe_account.c_str());

    // Stop if the account is not found
    if (!result)
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_UNKNOWN_ACCOUNT);

        SendPacket(packet);

        sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (unknown account).");
        return -1;
    }

    Field* fields = result->Fetch();

    expansion = ((sWorld.getConfig(CONFIG_UINT32_EXPANSION) > fields[7].GetUInt8()) ? fields[7].GetUInt8() : sWorld.getConfig(CONFIG_UINT32_EXPANSION));

    N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
    g.SetDword(7);

    v.SetHexStr(fields[5].GetString());
    s.SetHexStr(fields[6].GetString());
    m_s = s;

    const char* sStr = s.AsHexStr();                        // Must be freed by OPENSSL_free()
    const char* vStr = v.AsHexStr();                        // Must be freed by OPENSSL_free()

    DEBUG_LOG("WorldSocket::HandleAuthSession: (s,v) check s: %s v: %s",
        sStr,
        vStr);

    OPENSSL_free((void*)sStr);
    OPENSSL_free((void*)vStr);

    ///- Re-check ip locking (same check as in realmd).
    if (fields[4].GetUInt8() == 1)  // if ip is locked
    {
        if (strcmp(fields[3].GetString(), GetRemoteAddress().c_str()) != 0)
        {
            packet.Initialize(SMSG_AUTH_RESPONSE, 1);
            packet << uint8(AUTH_FAILED);
            SendPacket(packet);

            delete result;
            BASIC_LOG("WorldSocket::HandleAuthSession: Sent Auth Response (Account IP differs).");
            return -1;
        }
    }

    id = fields[0].GetUInt32();
    security = fields[1].GetUInt16();
    if (security > SEC_ADMINISTRATOR)                       // prevent invalid security settings in DB
        security = SEC_ADMINISTRATOR;

    K.SetHexStr(fields[2].GetString());

    time_t mutetime = time_t(fields[8].GetUInt64());

    locale = LocaleConstant(fields[9].GetUInt8());
    if (locale >= MAX_LOCALE)
        locale = LOCALE_enUS;

    delete result;

    // Re-check account ban (same check as in realmd)
    QueryResult* banresult =
        LoginDatabase.PQuery("SELECT 1 FROM account_banned WHERE id = %u AND active = 1 AND (unbandate > UNIX_TIMESTAMP() OR unbandate = bandate)"
        "UNION "
        "SELECT 1 FROM ip_banned WHERE (unbandate = bandate OR unbandate > UNIX_TIMESTAMP()) AND ip = '%s'",
        id, GetRemoteAddress().c_str());

    if (banresult) // if account banned
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_BANNED);
        SendPacket(packet);

        delete banresult;

        sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (Account banned).");
        return -1;
    }

    // Check locked state for server
    AccountTypes allowedAccountType = sWorld.GetPlayerSecurityLimit();

    if (allowedAccountType > SEC_PLAYER && AccountTypes(security) < allowedAccountType)
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_UNAVAILABLE);

        SendPacket(packet);

        BASIC_LOG("WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
        return -1;
    }

    // Check that Key and account name are the same on client and server
    Sha1Hash sha;

    uint32 t = 0;
    uint32 seed = m_Seed;

    sha.UpdateData(account);
    sha.UpdateData((uint8*)& t, 4);
    sha.UpdateData((uint8*)& clientSeed, 4);
    sha.UpdateData((uint8*)& seed, 4);
    sha.UpdateBigNumbers(&K, nullptr);
    sha.Finalize();

    if (memcmp(sha.GetDigest(), digest, 20) != 0)
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_FAILED);

        SendPacket(packet);

        sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (authentification failed).");
        return -1;
    }

    std::string address = GetRemoteAddress();

    DEBUG_LOG("WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.",
        account.c_str(),
        address.c_str());

    // Update the last_ip in the database
    // No SQL injection, username escaped.
    static SqlStatementID updAccount;

    SqlStatement stmt = LoginDatabase.CreateStatement(updAccount, "UPDATE account SET last_ip = ? WHERE username = ?");
    stmt.PExecute(address.c_str(), account.c_str());

    WorldSocketPtr this_session = std::static_pointer_cast<WorldSocket>(shared_from_this());
    // NOTE ATM the socket is single-threaded, have this in mind ...
    m_Session = new WorldSession(id, this_session, AccountTypes(security), expansion, mutetime, locale);

    m_Crypt.Init(&K);

    m_Session->LoadGlobalAccountData();
    m_Session->LoadTutorialsData();
    m_Session->ReadAddonsInfo(recvPacket);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    sWorld.AddSession(m_Session);

    return 0;
}

int WorldSocket::HandlePing(WorldPacket& recvPacket)
{
    uint32 ping;
    uint32 latency;

    // Get the ping packet content
    recvPacket >> ping;
    recvPacket >> latency;

    if (m_LastPingTime.time_since_epoch().count() == 0)
        m_LastPingTime = system_clock::now();            // for 1st ping
    else
    {
        system_clock::time_point cur_time = system_clock::now();
        system_clock::duration diff_time = cur_time - m_LastPingTime;
        m_LastPingTime = cur_time;

        if (diff_time < seconds(27))
        {
            ++m_OverSpeedPings;

            uint32 max_count = sWorld.getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS);

            if (max_count && m_OverSpeedPings > max_count)
            {
                GuardType Guard(m_SessionLock);

                if (m_Session && m_Session->GetSecurity() == SEC_PLAYER)
                {
                    sLog.outError("WorldSocket::HandlePing: Player kicked for "
                        "overspeeded pings address = %s",
                        GetRemoteAddress().c_str());

                    return -1;
                }
            }
        }
        else
            m_OverSpeedPings = 0;
    }

    // critical section
    {
        GuardType Guard(m_SessionLock);

        if (m_Session)
        {
            m_Session->SetLatency(latency);
            m_Session->ResetClientTimeDelay();
        }
        else
        {
            sLog.outError("WorldSocket::HandlePing: peer sent CMSG_PING, "
                "but is not authenticated or got recently kicked,"
                " address = %s",
                GetRemoteAddress().c_str());
            return -1;
        }
    }

    WorldPacket packet(SMSG_PONG, 4);
    packet << ping;
    if (!SendPacket(packet))
        return -1;

    return 0;
}

bool WorldSocket::AppendPacket(const WorldPacket &pct)
{
    ServerPktHeader header(pct.size() + 2, pct.GetOpcode());
    m_Crypt.EncryptSend((uint8*)header.header, header.getHeaderLength());

    if (m_outBuffer->space() >= pct.size() + header.getHeaderLength())
    {
        // Put the packet on the buffer.
        if (!m_outBuffer->Write(header.header, header.getHeaderLength()))
        {
            MANGOS_ASSERT(false);
            return false;
        }

        if (!pct.empty() && !m_outBuffer->Write(pct.contents(), pct.size()))
        {
            MANGOS_ASSERT(false);
            return false;
        }
    }
    else
    {
        // Enqueue the packet.
        MANGOS_ASSERT(false);
        return false;
    }

    return true;
}
