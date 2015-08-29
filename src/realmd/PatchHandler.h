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

#ifndef PATCH_HANDLER_H
#define PATCH_HANDLER_H

#include <map>
#include <memory>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include "Network/NetworkBuffer.h"
#include "Network/ProtocolDefinitions.h"
#include "Policies/Singleton.h"

/// Caches MD5 hash of client patches present on the server
class PatchCache
{
public:
    PatchCache();
    ~PatchCache();

    struct PATCH_INFO
    {
        uint8 md5[MD5_DIGEST_LENGTH];
    };

    typedef std::map<std::string, PATCH_INFO*> Patches;

    Patches::const_iterator begin() const
    {
        return patches_.begin();
    }

    Patches::const_iterator end() const
    {
        return patches_.end();
    }

    void LoadPatchMD5(const boost::filesystem::path& p);
    bool GetHash(const char* pat, uint8 mymd5[MD5_DIGEST_LENGTH]);

private:
    void LoadPatchesInfo();

    Patches patches_;
};

#define sPatchCache MaNGOS::Singleton<PatchCache>::Instance()

class PatchHandler : public std::enable_shared_from_this<PatchHandler>
{
public:
    PatchHandler(protocol::Socket& socket, boost::filesystem::fstream& fs_patch);
    virtual ~PatchHandler();

    bool Open();

protected:
    void TransmitFile();
    void StartAsyncWrite();

    void OnWriteComplete(const boost::system::error_code& error, size_t bytes_transferred);
    void OnTimeout(const boost::system::error_code& error);

private:
    size_t offset() const;

    protocol::Socket& socket_;
    boost::asio::deadline_timer timer_;

    boost::filesystem::fstream& fs_patch_;

    NetworkBuffer send_buffer_;
};

typedef std::shared_ptr<PatchHandler> PatchHandlerPtr;

#endif // PATCH_HANDLER_H
