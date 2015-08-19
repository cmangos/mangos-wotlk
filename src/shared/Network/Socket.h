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

#ifndef SOCKET_H
#define SOCKET_H

#include <mutex>
#include <memory>
#include "NetworkBuffer.h"
#include "ProtocolDefinitions.h"
#include "Common.h"
#include "Auth/AuthCrypt.h"
#include "Auth/BigNumber.h"

class NetworkThread;
class NetworkManager;

class Socket : public std::enable_shared_from_this<Socket>
{
public:
    friend class NetworkManager;

    Socket(NetworkManager& manager, NetworkThread& owner);
    virtual ~Socket(void);

    virtual void CloseSocket(void);

    bool IsClosed(void) const { return m_closed; }
    inline const std::string& GetRemoteAddress(void) const { return m_address; }

    bool EnableTCPNoDelay(bool enable);
    bool SetSendBufferSize(int size);
    void SetOutgoingBufferSize(size_t size);

    inline protocol::Socket& socket() { return m_socket; }
    inline NetworkThread& owner() { return m_owner; }

protected:
    virtual bool Open();
    void StartAsyncSend();
    virtual bool ProcessIncomingData() = 0;
    uint32 native_handle();

    typedef std::mutex                LockType;
    typedef std::lock_guard<LockType> GuardType;

    LockType                     m_outBufferLock;
    std::auto_ptr<NetworkBuffer> m_outBuffer;
    std::auto_ptr<NetworkBuffer> m_readBuffer;

    NetworkManager& m_manager;
    NetworkThread& m_owner;

private:
    void StartAsyncRead();
    void Close();

    void OnWriteComplete(const boost::system::error_code& error, size_t bytes_transferred);
    void OnReadComplete(const boost::system::error_code& error, size_t bytes_transferred);
    void OnError(const boost::system::error_code& error);
    std::string ObtainRemoteAddress() const;

    protocol::Socket  m_socket;
    size_t            m_outgoingBufferSize;
    std::string       m_address;
    bool              m_writeOperation;
    bool              m_closed;

    static const std::string UNKNOWN_NETWORK_ADDRESS;;
};

#endif // SOCKET_H