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

#ifndef __NETWORK_THREAD_HPP_
#define __NETWORK_THREAD_HPP_

#include <thread>
#include <list>

#include <boost/asio.hpp>

#include "Socket.hpp"

namespace MaNGOS
{
    template <typename SocketType>
    class NetworkThread
    {
        private:
            // note that the order of the members here is carefully chosen to set the order of initialization
            boost::asio::io_service m_service;

            std::list<std::unique_ptr<SocketType>> m_sockets;

            boost::asio::io_service::work m_work;

            std::thread m_thread;

        public:
            NetworkThread() : m_work(m_service), m_thread([this] { boost::system::error_code ec; this->m_service.run(ec); }) { m_thread.detach(); }

            size_t Size() const { return m_sockets.size(); }

            SocketType *CreateSocket();

            void RemoveSocket(Socket *socket)
            {
                for (auto i = m_sockets.begin(); i != m_sockets.end(); ++i)
                    if (i->get() == socket)
                    {
                        m_sockets.erase(i);
                        return;
                    }
            }
    };

    template <typename SocketType>
    SocketType *NetworkThread<SocketType>::CreateSocket()
    {
        m_sockets.push_front(std::unique_ptr<SocketType>(new SocketType(m_service, [this](Socket *socket) { this->RemoveSocket(socket); })));

        return m_sockets.begin()->get();
    }
}

#endif /* !__NETWORK_THREAD_HPP_ */