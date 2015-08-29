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

#include <boost/bind.hpp>

#include "NetworkManager.h"
#include "Common.h"
#include "NetworkThread.h"
#include "Socket.h"
#include "Log.h"

NetworkManager::NetworkManager(std::string const& mname) : m_managerName(mname), m_networkThreadsCount(1), m_running(false)
{

}

NetworkManager::~NetworkManager()
{
    StopNetwork();
    m_acceptor.reset();
    m_networkThreads.reset();
}

bool NetworkManager::StartNetwork(std::uint16_t port, std::string address)
{
    if (m_running)
        return false;

    m_address = address;
    m_port = port;

    if (m_networkThreadsCount <= 0)
    {
        sLog.outError("Number of network threads is incorrect = %i", m_networkThreadsCount);
        return false;
    }

    m_networkThreadsCount += 1;
    m_networkThreads.reset(new NetworkThread[m_networkThreadsCount]);

    try
    {
        protocol::Endpoint listen_address(protocol::IPAddress::from_string(m_address), m_port);
        m_acceptor.reset(new protocol::Acceptor(get_acceptor_thread().service(), listen_address));
    }
    catch (boost::system::error_code&)
    {
        sLog.outError("Failed to open acceptor, check if the port is free");
        return false;
    }

    m_running = true;

    AcceptNewConnection();

    std::string threadName = "\"" + m_managerName + "\" Acceptor";
    m_networkThreads[0].SetName(threadName);
    m_networkThreads[0].Start();
    threadName = "\"" + m_managerName + "\" Service n";
    for (size_t i = 1; i < m_networkThreadsCount; ++i)
    {
        // set thread name
        m_networkThreads[i].SetName(threadName + std::to_string(i));
        m_networkThreads[i].Start();
    }

    return true;
}

void NetworkManager::StopNetwork()
{
    if (m_running)
    {
        m_running = false;

        if (m_acceptor.get())
            m_acceptor->cancel();

        if (m_networkThreads)
            for (size_t i = 0; i < m_networkThreadsCount; ++i)
                m_networkThreads[i].Stop();
    }
}

bool NetworkManager::OnSocketOpen(const SocketPtr& socket)
{
    NetworkThread& thread = socket->owner();
    thread.AddSocket(socket);

    return true;
}

void NetworkManager::OnSocketClose(const SocketPtr& socket)
{
    NetworkThread& thread = socket->owner();
    thread.RemoveSocket(socket);
}

void NetworkManager::AcceptNewConnection()
{
    NetworkThread& worker = get_network_thread_for_new_connection();
    SocketPtr connection = CreateSocket(worker);

    m_acceptor->async_accept(connection->socket(),
        boost::bind(&NetworkManager::OnNewConnection, this, connection, boost::asio::placeholders::error));
}

void NetworkManager::OnNewConnection(SocketPtr connection, const boost::system::error_code& error)
{
    if (error)
    {
        if (m_running) // avoid showing an error that may occur durring shutdown
            sLog.outError("Error accepting new client connection!");
        return;
    }

    if (!connection->Open())
    {
        sLog.outError("Unable to start new client connection!");

        connection->CloseSocket();
        return;
    }

    AcceptNewConnection();
}

NetworkThread& NetworkManager::get_acceptor_thread()
{
    return m_networkThreads[0];
}

NetworkThread& NetworkManager::get_network_thread_for_new_connection()
{
    // Skip acceptor thread
    size_t min = 1;

    MANGOS_ASSERT(m_networkThreadsCount > 1);

    for (size_t i = 1; i < m_networkThreadsCount; ++i)
    {
        if (m_networkThreads[i].Connections() < m_networkThreads[min].Connections())
            min = i;
    }

    return m_networkThreads[min];
}