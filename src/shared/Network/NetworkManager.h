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

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <cstdint>

#include <boost/scoped_array.hpp>

#include "ProtocolDefinitions.h"

class NetworkThread;

class NetworkManager
{
public:
    friend class Socket;

    virtual bool StartNetwork(std::uint16_t port, std::string address);
    void StopNetwork();

    const std::string& GetBindAddress() { return m_address; }
    std::uint16_t GetBindPort() { return m_port; }

protected:
    NetworkManager(std::string const& mname);
    virtual ~NetworkManager();

    virtual SocketPtr CreateSocket(NetworkThread& owner) = 0;

    virtual bool OnSocketOpen(const SocketPtr& socket);
    virtual void OnSocketClose(const SocketPtr& socket);

    size_t  m_networkThreadsCount;
    bool    m_running;

private:
    void AcceptNewConnection();
    void OnNewConnection(SocketPtr connection, const boost::system::error_code& error);
    NetworkThread& get_acceptor_thread();
    NetworkThread& get_network_thread_for_new_connection();

    std::string                        m_address;
    std::uint16_t                      m_port;
    std::string                        m_managerName;
    std::auto_ptr<protocol::Acceptor>  m_acceptor;
    boost::scoped_array<NetworkThread> m_networkThreads;
};

#endif // NETWORK_MANAGER_H