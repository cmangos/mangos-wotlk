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

#include <cstdint>

#include "RemoteAdministration.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "AccountMgr.h"
#include "Config/Config.h"

bool RemoteAdminSocketMgr::StartNetwork(std::uint16_t port, std::string address)
{
    if (m_running)
        return false;

    return NetworkManager::StartNetwork(port, address);
}

RemoteAdminSocketMgr::RemoteAdminSocketMgr() : NetworkManager("Remote Administration")
{
}

RemoteAdminSocketMgr::~RemoteAdminSocketMgr()
{

}

SocketPtr RemoteAdminSocketMgr::CreateSocket(NetworkThread& owner)
{
    return SocketPtr(new RASocket(*this, owner));
}

//////////////////////////////////////////////////////////////////////////
RASocket::RASocket(NetworkManager& manager, NetworkThread& owner) :
    Socket(manager, owner),
    m_isLogged(0),
    m_accountID(0),
    m_loginTry(0)
{
    m_minAccountLevel = AccountTypes(sConfig.GetIntDefault("RA.MinLevel", SEC_ADMINISTRATOR));
}

RASocket::~RASocket()
{

}

bool RASocket::Open()
{
    if (!Socket::Open())
        return false;

    sLog.outRALog("Incoming connection from %s.", GetRemoteAddress().c_str());
    sLog.outString("Remote administration connection detected.");
    // Print Motd
    SendString(sWorld.GetMotd());
    SendString("\r\n");
    SendString(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_USER));
    return true;
}

bool RASocket::ProcessIncomingData()
{
    while (1)
    {
        uint8 byte;
        if (!m_readBuffer->ReadNoConsume(&byte, 1))
            return true;

        if (std::iscntrl(byte))
        {
            switch (byte)
            {
                case 0x0A: // \r return
                    break;
                case 0x0D: // \n line feed
                    ProcessCommand(m_commandBuffer);
                    m_commandBuffer.clear();
                    break;

                case 0x08: // \b backspace
                    if (!m_commandBuffer.empty())
                    {
                        m_commandBuffer.pop_back();
                        char rchar[2] { 0x20, 0x08 };
                        SendPacket(rchar, 2);
                    }
                    else
                        SendPacket(">", 1);
                    break;

                default:
                    break;
            }
            m_readBuffer->Consume(1);
            continue;
        }

        if (!m_isLogged && !m_userName.empty())
        {
            // hide password
            char rchar[2] { 0x08, '*' };
            SendPacket(rchar, 2);
        }

        char data;
        Read(&data, 1);
        m_commandBuffer += data;
    }
    return true;
}

bool RASocket::CanTryAnotherTime()
{
    if (m_loginTry > RA_MAX_LOGIN_TRY - 1)
    {
        sLog.outRALog("Ip %s kicked, too much login attempt.", GetRemoteAddress().c_str());
        sLog.outString("Too much logion attempt from %s. Ip Kicked.", GetRemoteAddress().c_str());
        SendString("Too much attempt!\r\n");
        this->CloseSocket();
        return false;
    }
    SendString("\r\n");
    SendString(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_USER));
    m_userName.clear();
    return true;
}

void RASocket::TryToLog(std::string str)
{
    if (m_userName.empty())
    {
        m_accountID = sAccountMgr.GetId(str);
        m_userName = str;
        SendString(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_PASS));
        return;
    }

    ++m_loginTry;
    m_accountID = sAccountMgr.GetId(m_userName);
    if (!m_accountID)
    {
        SendString("Wrong user name or password.\r\n");
        sLog.outRALog("User %s ip'%s' tried to connect with wrong user name.", m_userName.c_str(), GetRemoteAddress().c_str());
        CanTryAnotherTime();
        return;
    }

    AccountTypes accountLevel = sAccountMgr.GetSecurity(m_accountID);
    if (accountLevel < m_minAccountLevel)
    {
        sLog.outRALog("Account %s ip'%s' tried to connect on Remote Administration console with insufficient privilege.", m_userName.c_str(), GetRemoteAddress().c_str());
        sLog.outString("Account %s ip'%s' tried to connect on Remote Administration console with insufficient privilege.", m_userName.c_str(), GetRemoteAddress().c_str());
        SendString("Wrong user name or password.\r\n");
        CanTryAnotherTime();
        return;
    }
    m_accountLevel = accountLevel;

    if (!sAccountMgr.CheckPassword(m_accountID, str))
    {
        sLog.outRALog("Account %s ip'%s' tried to connect on Remote Administration console with wrong password.", m_userName.c_str(), GetRemoteAddress().c_str());
        sLog.outString("Account %s ip'%s' tried to connect on Remote Administration console with wrong password.", m_userName.c_str(), GetRemoteAddress().c_str());
        SendString("Wrong user name or password.\r\n");
        CanTryAnotherTime();
        return;
    }

    m_isLogged = true;

    SendString("\r\n");
    SendString(CMANGOS_PROMPT);
}

void RASocket::ProcessCommand(std::string command)
{
    if (command.empty())
    {
        SendString(CMANGOS_PROMPT);
        return;
    }

    if (!m_isLogged)
    {
        TryToLog(command);
        return;
    }

    sLog.outRALog("User '%s' command.", m_userName.c_str(), command.c_str());
    if ((command == "quit") || (command == "exit"))
    {
        SendString("Bye!");
        this->CloseSocket();
    }
    else
    {
        CliCommandHolder* cmd = new CliCommandHolder(m_accountID, m_accountLevel, this, command.c_str(), &RASocket::zprint, &RASocket::commandFinished);
        sWorld.QueueCliCommand(cmd);
    }
}

bool RASocket::SendString(std::string str)
{
    return SendPacket(str.c_str(), str.length());
}

bool RASocket::SendPacket(const char* buf, size_t len)
{
    GuardType Guard(m_outBufferLock);

    if (m_outBuffer->Write((uint8*)buf, len))
    {
        StartAsyncSend();
        return true;
    }
    else
        sLog.outError("network write buffer is too small to accommodate packet");

    return false;
}

size_t RASocket::ReceivedDataLength(void) const
{
    return m_readBuffer->length();
}

bool RASocket::Read(char* buf, size_t len)
{
    return m_readBuffer->Read((uint8*)buf, len);
}

void RASocket::zprint(void* callbackArg, const char* szText)
{
    if (!szText)
        return;

    ((RASocket*)callbackArg)->SendString(szText);
}

void RASocket::commandFinished(void* callbackArg, bool success)
{
    RASocket* raSocket = (RASocket*)callbackArg;
    raSocket->SendString(CMANGOS_PROMPT);
}
