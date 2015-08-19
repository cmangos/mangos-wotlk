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
#include <thread>
#include "NetworkThread.h"
#include "Database/DatabaseEnv.h"
#include "Socket.h"

NetworkThread::NetworkThread() : m_connections(0), m_threadName("Unknown")
{

}

NetworkThread::~NetworkThread()
{
    Stop();
}

void NetworkThread::Start()
{
    m_serviceWork.reset(new protocol::Service::work(m_service));
    m_thread.reset(new std::thread(boost::bind(&NetworkThread::Work, this)));
}

void NetworkThread::Stop()
{
    m_serviceWork.reset();
    m_service.stop();

    if (m_thread.get())
    {
        m_thread->join();
        m_thread.reset();
    }
}

void NetworkThread::AddSocket(const SocketPtr& socket)
{
    ++m_connections;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sockets.insert(socket);
}

void NetworkThread::RemoveSocket(const SocketPtr& socket)
{
    --m_connections;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sockets.erase(socket);
}

void NetworkThread::Work()
{
    DEBUG_LOG("Starting %s network thread.", m_threadName.c_str());
    LoginDatabase.ThreadStart();
    m_service.run();
    LoginDatabase.ThreadEnd();
    DEBUG_LOG("%s network thread exitting.", m_threadName.c_str());
}
