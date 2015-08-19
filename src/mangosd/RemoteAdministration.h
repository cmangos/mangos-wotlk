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

#ifndef REMOTE_ADMINISTRATION_H
#define REMOTE_ADMINISTRATION_H

#include <string>
#include <mutex>
#include <cstdint>

#include "Network/NetworkManager.h"
#include "Network/Socket.h"
#include "ByteBuffer.h"

#define RA_BUFF_SIZE 8192

/// Manages all sockets connected to peers and network threads
class RemoteAdminSocketMgr : public NetworkManager
{
public:
    virtual bool StartNetwork(std::uint16_t port, std::string address) override;

    RemoteAdminSocketMgr();
    ~RemoteAdminSocketMgr();

private:
    SocketPtr CreateSocket(NetworkThread& owner);
};

class RASocket : public Socket
{
#define CMANGOS_PROMPT "cmangos>"
#define RA_MAX_LOGIN_TRY 3
public:
    RASocket(NetworkManager& manager, NetworkThread& owner);
    std::mutex PendingCommand;
    ~RASocket();

protected:
    virtual bool Open() override;
    virtual bool ProcessIncomingData() override;

private:
    bool SendPacket(const char* buf, size_t len);
    size_t ReceivedDataLength(void) const;
    bool Read(char* buf, size_t len);
    bool SendString(std::string str);
    void ProcessCommand(std::string str);
    void TryToLog(std::string str);
    bool CanTryAnotherTime();

    static void zprint(void* callbackArg, const char* szText);
    static void commandFinished(void* callbackArg, bool success);


    std::string     m_commandBuffer;
    uint32          m_loginTry;
    bool            m_isLogged;
    std::string     m_userName;
    uint32          m_accountID;
    AccountTypes    m_accountLevel;
    AccountTypes    m_minAccountLevel;
};

#endif // REMOTE_ADMINISTRATION_H
