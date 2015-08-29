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

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include "AuthCodes.h"
#include "Common.h"
#include "Log.h"
#include "PatchHandler.h"

INSTANTIATE_SINGLETON_1(PatchCache);

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct Chunk
{
    uint8 cmd;
    uint16 data_size;
    uint8 data[4096]; // 4096 - page size on most arch
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

PatchCache::PatchCache()
{
    LoadPatchesInfo();
}

PatchCache::~PatchCache()
{
    for (Patches::iterator i = patches_.begin(); i != patches_.end(); ++i)
        delete i->second;
}

void PatchCache::LoadPatchMD5(const boost::filesystem::path& p)
{
    sLog.outDebug("Loading patch info for %s", p.c_str());
    boost::filesystem::fstream fs(p);

    if (!fs)
        return;

    // Calculate the MD5 hash
    MD5_CTX ctx;
    MD5_Init(&ctx);

    const size_t CHUNCK_SIZE = 4 * 1024;
    char buffer[CHUNCK_SIZE];

    while (!fs.eof())
    {
        fs.read(buffer, CHUNCK_SIZE);
        MD5_Update(&ctx, buffer, fs.gcount());
    }

    fs.close();

    // Store the result in the internal patch hash map
    patches_[p.string()] = new PATCH_INFO;
    MD5_Final((uint8*)&patches_[p.string()]->md5, &ctx);
}

bool PatchCache::GetHash(const char* pat, uint8 mymd5[MD5_DIGEST_LENGTH])
{
    for (Patches::iterator i = patches_.begin(); i != patches_.end(); ++i)
    {
        if (!stricmp(pat, i->first.c_str()))
        {
            memcpy(mymd5, i->second->md5, MD5_DIGEST_LENGTH);
            return true;
        }
    }

    return false;
}

void PatchCache::LoadPatchesInfo()
{
    const int MIN_FILENAME_LENGTH = 8;
    boost::filesystem::path p = "./patches/";

    try
    {
        if (boost::filesystem::exists(p) && boost::filesystem::is_directory(p))
        {
            boost::filesystem::directory_iterator end_itr;

            for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
            {
                if (!boost::filesystem::is_regular_file(itr->status()))
                    continue;

                if (itr->path().filename().string().length() >= MIN_FILENAME_LENGTH
                    && itr->path().filename().extension().string() == ".mpq")
                        LoadPatchMD5(itr->path());

            }
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        sLog.outError("PatchCache::LoadPatchInfos: Error occured: %s", ex.what());
    }
}

PatchHandler::PatchHandler(protocol::Socket& socket, boost::filesystem::fstream& fs_patch) : socket_(socket), fs_patch_(fs_patch),
    timer_(socket.get_io_service(), boost::posix_time::seconds(1)), send_buffer_(sizeof(Chunk))
{
    Chunk* data = (Chunk*)send_buffer_.read_data();
    data->cmd = CMD_XFER_DATA;
    data->data_size = 0;
}

PatchHandler::~PatchHandler()
{
    if (fs_patch_.is_open())
        fs_patch_.close();
}

size_t PatchHandler::offset() const
{
    Chunk* chunk = (Chunk*)send_buffer_.read_data();
    return sizeof(Chunk) - sizeof(chunk->data);
}

bool PatchHandler::Open()
{
    if (!fs_patch_.is_open())
        return false;

    timer_.async_wait(boost::bind(&PatchHandler::OnTimeout, shared_from_this(), boost::asio::placeholders::error));
    return true;
}

void PatchHandler::OnTimeout(const boost::system::error_code& error)
{
    if (error)
        return;

    TransmitFile();
}

void PatchHandler::TransmitFile()
{
    send_buffer_.Reset();
    Chunk* data = (Chunk*)send_buffer_.read_data();
    fs_patch_.read((char*)data->data, sizeof(data->data));

    std::streamsize size = fs_patch_.gcount();
    if (size <= 0)
        return;

    data->data_size = (uint16) size;
    send_buffer_.Commit(size_t(size) + offset());

    StartAsyncWrite();
}

void PatchHandler::StartAsyncWrite()
{
    socket_.async_write_some(boost::asio::buffer(send_buffer_.read_data(), send_buffer_.length()),
        boost::bind(&PatchHandler::OnWriteComplete, shared_from_this(),
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void PatchHandler::OnWriteComplete(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error)
        return;

    send_buffer_.Consume(bytes_transferred);

    if (send_buffer_.length() > 0)
    {
        StartAsyncWrite();
        return;
    }

    TransmitFile();
}
