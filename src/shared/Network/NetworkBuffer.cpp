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

#include "NetworkBuffer.h"

NetworkBuffer::NetworkBuffer() : m_writePosition(0), m_readPosition(0),
m_size(0), m_dataAllocated(false)
{

}

NetworkBuffer::NetworkBuffer(const uint32 size) : m_writePosition(0), m_readPosition(0),
m_size(size), m_dataAllocated(true)
{
    m_data = new uint8[size];
}

NetworkBuffer::NetworkBuffer(uint8* buffer, const uint32 size) : m_writePosition(0), m_readPosition(0),
m_size(size), m_dataAllocated(false)
{
    m_data = buffer;
}

NetworkBuffer::~NetworkBuffer()
{
    Deallocate();
}

void NetworkBuffer::Allocate(const uint32 size)
{
    if (m_data == nullptr)
    {
        m_size = size;
        m_data = new uint8[size];
        m_dataAllocated = true;
    }
}

void NetworkBuffer::Reallocate(const uint32 new_size)
{
    if (m_dataAllocated)
    {
        delete[] m_data;
        m_size = new_size;
        m_data = new uint8[new_size];
        Reset();
    }
}

void NetworkBuffer::Deallocate()
{
    if (m_dataAllocated)
    {
        delete[] m_data;
        m_dataAllocated = false;
    }
}

void NetworkBuffer::AssignBuffer(uint8* buffer, const uint32 size)
{
    Deallocate();
    m_size = size;
    m_data = buffer;
}

void NetworkBuffer::UnassignBuffer()
{
    if (!m_dataAllocated)
    {
        m_size = 0;
        m_data = nullptr;
        Reset();
    }
}

bool NetworkBuffer::Write(const uint8* data, const size_t n)
{
    if (m_data == nullptr || data == nullptr || n > space())
        return false;

    std::memcpy(&m_data[m_writePosition], data, n);
    Commit(n);
    return true;
}

bool NetworkBuffer::Read(uint8* data, const size_t n)
{
    if (m_data == nullptr || data == nullptr || n > length())
        return false;

    std::memcpy(data, &m_data[m_readPosition], n);
    Consume(n);
    return true;
}

bool NetworkBuffer::ReadNoConsume(uint8* data, const size_t n)
{
    if (m_data == nullptr || data == nullptr || n > length())
        return false;

    std::memcpy(data, &m_data[m_readPosition], n);
    return true;
}

void NetworkBuffer::Commit(const size_t n)
{
    uint32 pos = m_writePosition + n;
    if (capacity() >= pos)
        m_writePosition = pos;
}

void NetworkBuffer::Consume(const size_t n)
{
    uint32 pos = m_readPosition + n;
    if (capacity() >= pos)
        m_readPosition = pos;
}

void NetworkBuffer::Prepare()
{
    if (!Crunch())
        Reset();
}

bool NetworkBuffer::Crunch()
{
    if (m_data != nullptr && length() != 0)
    {
        if (m_readPosition > m_writePosition)
            return false;

        size_t len = length();
        std::memmove(m_data, &m_data[m_readPosition], len);
        m_writePosition = len;
        m_readPosition = 0;
        return true;
    }
    return false;
}

void NetworkBuffer::Reset()
{
    m_writePosition = m_readPosition = 0;
}

uint8* NetworkBuffer::read_data() const
{
    if (m_data == nullptr)
        return nullptr;

    return &m_data[m_readPosition];
}

uint8* NetworkBuffer::write_data() const
{
    if (m_data == nullptr)
        return nullptr;

    return &m_data[m_writePosition];
}

const uint32 NetworkBuffer::length() const
{
    return m_writePosition - m_readPosition;
}

const uint32 NetworkBuffer::space() const
{
    return m_size - m_writePosition;
}

const uint32 NetworkBuffer::capacity() const
{
    return m_size;
}