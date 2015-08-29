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

#ifndef AUTH_SOCKET_H
#define AUTH_SOCKET_H

#include <string>
#include <boost/filesystem/fstream.hpp>
#include "Common.h"
#include "Auth/BigNumber.h"
#include "Auth/Sha1.h"
#include "ByteBuffer.h"
#include "Network/Socket.h"

class NetworkManager;
class NetworkThread;

/// Handles authentication service packets
class AuthSocket: public Socket
{
public:
    const static int s_BYTE_SIZE = 32;

    AuthSocket(NetworkManager& manager, NetworkThread& owner);
    ~AuthSocket();

protected:
    virtual bool Open() override;
    virtual bool ProcessIncomingData() override;

private:
    bool SendPacket(const char* buf, size_t len);
    size_t ReceivedDataLength(void) const;
    bool Read(char* buf, size_t len);
    void ReadSkip(size_t len);

    // Login process handlers
    bool HandleLogonChallenge();
    bool HandleLogonProof();
    bool HandleReconnectChallenge();
    bool HandleReconnectProof();
    bool HandleRealmList();
    void SendProof(Sha1Hash sha);
    void LoadRealmlist(ByteBuffer& pkt, uint32 acctid);

    // Patch transfer handlers
    void InitPatch();
    bool HandleXferAccept();
    bool HandleXferCancel();
    bool HandleXferResume();

    // Make the SRP6 calculation based on the hash in the database
    void SetVSFields(const std::string& rI);

    // SRP6
    BigNumber N, s, g, v;
    BigNumber b, B;
    BigNumber K;
    BigNumber reconnect_proof_;

    bool authed_;

    std::string login_;
    std::string safe_login_;

    uint16 build_;
    AccountTypes account_security_level_;

    // Since GetLocaleByName() is _NOT_ bijective, we have to store the locale as a string. Otherwise we can't differ
    // between enUS and enGB, which is important for the patch system
    std::string localization_name_;

    boost::filesystem::fstream patch_;
};

#endif // AUTH_SOCKET_H
