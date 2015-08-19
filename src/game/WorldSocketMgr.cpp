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

#include <mutex>
#include <cstdint>

#include <boost/system/error_code.hpp>

#include "WorldSocketMgr.h"
#include "Log.h"
#include "Common.h"
#include "Config/Config.h"
#include "WorldSocket.h"

#define CLASS_LOCK MaNGOS::ClassLevelLockable<WorldSocketMgr, std::recursive_mutex>
INSTANTIATE_SINGLETON_2(WorldSocketMgr, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(WorldSocketMgr, std::recursive_mutex);

WorldSocketMgr::WorldSocketMgr() : NetworkManager("World"),
    m_SockOutKBuff(-1),
    m_SockOutUBuff(protocol::SEND_BUFFER_SIZE),
    m_UseNoDelay(true)
{
}

WorldSocketMgr::~WorldSocketMgr()
{
}

bool WorldSocketMgr::StartNetwork(std::uint16_t port, std::string address)
{
    if (m_running)
        return false;

    m_UseNoDelay = sConfig.GetBoolDefault("Network.TcpNodelay", true);

    // -1 means use default
    m_SockOutKBuff = sConfig.GetIntDefault("Network.OutKBuff", -1);

    m_SockOutUBuff = sConfig.GetIntDefault("Network.OutUBuff", protocol::SEND_BUFFER_SIZE);

    if (m_SockOutUBuff <= 0)
    {
        sLog.outError("Network.OutUBuff is wrong in your config file");

        return false;
    }

    m_networkThreadsCount = static_cast<size_t>(sConfig.GetIntDefault("Network.Threads", 1));

    if (!NetworkManager::StartNetwork(port, address))
        return false;

    BASIC_LOG("Max allowed socket connections %d", boost::asio::socket_base::max_connections);
    return true;
}

bool WorldSocketMgr::OnSocketOpen(const SocketPtr& socket)
{
    // set some options here
    if (m_SockOutKBuff >= 0 && !socket->SetSendBufferSize(m_SockOutKBuff))
    {
        sLog.outError("WorldSocketMgr::OnSocketOpen set_option SO_SNDBUF");
        return false;
    }

    // Set TCP_NODELAY.
    if (m_UseNoDelay && !socket->EnableTCPNoDelay(m_UseNoDelay))
    {
        sLog.outError("WorldSocketMgr::OnSocketOpen: peer().set_option TCP_NODELAY errno = %s",
            boost::system::error_code(errno, boost::system::get_system_category()).message().c_str());
        return false;
    }

    socket->SetOutgoingBufferSize( static_cast<size_t>(m_SockOutUBuff));

    return NetworkManager::OnSocketOpen(socket);
}

SocketPtr WorldSocketMgr::CreateSocket(NetworkThread& owner)
{
    return SocketPtr(new WorldSocket(*this, owner));
}
