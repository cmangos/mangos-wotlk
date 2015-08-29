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

#ifndef WORLD_PACKET_H
#define WORLD_PACKET_H

#include "Common.h"
#include "ByteBuffer.h"
#include "Opcodes.h"

// Note: opcode_ and size stored in platfom dependent format
// Ignore endianess until send, and converted at receive
class WorldPacket : public ByteBuffer
{
public:
    WorldPacket() : ByteBuffer(0), opcode_(MSG_nullptr_ACTION) { }
    explicit WorldPacket(Opcodes opcode, size_t res = 200) : ByteBuffer(res), opcode_(opcode) { }
    WorldPacket(const WorldPacket& packet) : ByteBuffer(packet), opcode_(packet.opcode_) { }

    void Initialize(Opcodes opcode, size_t newres = 200)
    {
        clear();
        _storage.reserve(newres);
        opcode_ = opcode;
    }

    Opcodes GetOpcode() const { return opcode_; }
    void SetOpcode(Opcodes opcode) { opcode_ = opcode; }
    inline const char* GetOpcodeName() const { return LookupOpcodeName(opcode_); }

protected:
    Opcodes opcode_;
};

#endif // WORLD_PACKET_H