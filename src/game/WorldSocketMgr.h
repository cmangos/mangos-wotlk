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

#ifndef WORLD_SOCKET_MGR_H
#define WORLD_SOCKET_MGR_H

#include <mutex>
#include <string>
#include <cstdint>

#include "Policies/Singleton.h"
#include "Network/NetworkManager.h"

/// Manages all sockets connected to peers and network threads
class WorldSocketMgr : public NetworkManager, public MaNGOS::Singleton<WorldSocketMgr, MaNGOS::ClassLevelLockable<WorldSocketMgr, std::recursive_mutex>>
{
public:
    friend class WorldSocket;
    friend class MaNGOS::OperatorNew<WorldSocketMgr>;

    virtual bool StartNetwork(std::uint16_t port, std::string address) override;

private:
    WorldSocketMgr();
    virtual ~WorldSocketMgr();

    virtual bool OnSocketOpen(const SocketPtr& socket) override;
    virtual SocketPtr CreateSocket(NetworkThread& owner) override;

    int     m_SockOutKBuff;
    int     m_SockOutUBuff;
    bool    m_UseNoDelay;
};

#define sWorldSocketMgr WorldSocketMgr::Instance()

#endif // WORLD_SOCKET_MGR_H
