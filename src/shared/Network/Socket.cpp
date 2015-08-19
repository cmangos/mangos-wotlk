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

#include "Socket.h"
#include <boost/bind.hpp>
#include "Auth/Sha1.h"
#include "Common.h"
#include "NetworkThread.h"
#include "NetworkManager.h"
#include "Log.h"
#include "Util.h"

const std::string Socket::UNKNOWN_NETWORK_ADDRESS = "<unknown>";

Socket::Socket(NetworkManager& manager, NetworkThread& owner) : m_manager(manager), m_owner(owner), m_socket(owner.service()),
    m_outgoingBufferSize(protocol::SEND_BUFFER_SIZE), m_writeOperation(false), m_closed(true), m_address(UNKNOWN_NETWORK_ADDRESS)
{

}

Socket::~Socket(void)
{
    Close();
}

void Socket::CloseSocket(void)
{
    if (m_closed)
        return;

    Close();

    m_manager.OnSocketClose(shared_from_this());
}

bool Socket::Open()
{
    if (m_outBuffer.get())
        return false;

    m_address = ObtainRemoteAddress();
    if (m_address == UNKNOWN_NETWORK_ADDRESS)
        return false;

    if (!m_manager.OnSocketOpen(shared_from_this()))
        return false;

    m_closed = false;

    m_outBuffer.reset(new NetworkBuffer(m_outgoingBufferSize));
    m_readBuffer.reset(new NetworkBuffer(protocol::READ_BUFFER_SIZE));

    StartAsyncRead();

    return true;
}

void Socket::Close()
{
    if (IsClosed())
        return;

    m_closed = true;

    try
    {
        if (m_socket.is_open())
        {
            m_socket.shutdown(boost::asio::socket_base::shutdown_both);
            m_socket.close();
        }
    }
    catch (boost::system::error_code& e)
    {
        sLog.outError("Socket::close: error occurred while closing socket = %s", e.message().c_str());
    }
}

bool Socket::EnableTCPNoDelay(bool enable)
{
    try
    {
        m_socket.set_option(boost::asio::ip::tcp::no_delay(enable));
    }
    catch (boost::system::error_code& error)
    {
        sLog.outError("Socket::SetTCPNoDelay: set_option TCP_NODELAY errno = %s", error.message().c_str());
        return false;
    }

    return true;
}

bool Socket::SetSendBufferSize(int size)
{
    try
    {
        m_socket.set_option(boost::asio::socket_base::send_buffer_size(size));
    }
    catch (boost::system::error_code& error)
    {
        sLog.outError("Socket::SetSendBufferSize set_option SO_SNDBUF with size %i errno = %s", size, error.message().c_str());
        return false;
    }

    return true;
}

void Socket::SetOutgoingBufferSize(size_t size)
{
    m_outgoingBufferSize = size;
}

uint32 Socket::native_handle()
{
    return uint32(m_socket.native_handle());
}

void Socket::StartAsyncSend()
{
    if (m_closed)
        return;

    if (m_writeOperation)
        return;

    if (m_outBuffer->length() == 0)
    {
        m_writeOperation = false;
        return;
    }

    m_writeOperation = true;

    m_socket.async_write_some(boost::asio::buffer(m_outBuffer->read_data(), m_outBuffer->length()),
        boost::bind(&Socket::OnWriteComplete, shared_from_this(), boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Socket::OnWriteComplete(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error)
    {
        OnError(error);
        return;
    }

    GuardType Lock(m_outBufferLock);

    m_writeOperation = false;
    m_outBuffer->Consume(bytes_transferred);
    m_outBuffer->Prepare();

    StartAsyncSend();
}

void Socket::StartAsyncRead()
{
    if (IsClosed())
        return;

    m_readBuffer->Prepare();

    m_socket.async_read_some(boost::asio::buffer(m_readBuffer->write_data(), m_readBuffer->space()),
        boost::bind(&Socket::OnReadComplete, shared_from_this(), boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Socket::OnReadComplete(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error)
    {
        OnError(error);
        return;
    }

    if (bytes_transferred > 0)
    {
        m_readBuffer->Commit(bytes_transferred);

        if (!ProcessIncomingData())
        {
            CloseSocket();
            return;
        }
    }

    StartAsyncRead();
}

void Socket::OnError(const boost::system::error_code& error)
{
    if (!error)
        return;

    CloseSocket();

    // Don't log EOF errors since they notify about remote client connection close
    if( error != boost::asio::error::eof )
        sLog.outError("Network error occurred = %s. Closing connection", error.message().c_str());
}

std::string Socket::ObtainRemoteAddress() const
{
    try
    {
        protocol::Endpoint remote_addr = m_socket.remote_endpoint();
        protocol::IPAddress ip_addr = remote_addr.address();
        return ip_addr.to_string();
    }
    catch( boost::system::error_code& error )
    {
        sLog.outError("Socket::obtain_remote_address: errno = %s",error.message().c_str());
    }

    return UNKNOWN_NETWORK_ADDRESS;
}