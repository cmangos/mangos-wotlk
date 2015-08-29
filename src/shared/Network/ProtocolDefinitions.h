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

#ifndef PROTOCOL_DEFINITIONS_H
#define PROTOCOL_DEFINITIONS_H

#include "Platform/Define.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace protocol
{
    typedef boost::asio::ip::tcp::acceptor Acceptor;
    typedef boost::asio::ip::address IPAddress;
    typedef boost::asio::ip::tcp::endpoint Endpoint;
    typedef boost::asio::ip::tcp::socket Socket;
    typedef boost::asio::io_service Service;

    const uint32 READ_BUFFER_SIZE = 4096;
    const uint32 SEND_BUFFER_SIZE = 65536;
}

class Socket;
typedef std::shared_ptr<Socket> SocketPtr;

#endif