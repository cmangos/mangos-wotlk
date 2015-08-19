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

#ifndef MANGOSSOAP_H
#define MANGOSSOAP_H

#include <mutex>
#include <condition_variable>
#include "AccountMgr.h"
#include "Common.h"
#include "Log.h"
#include "soapH.h"
#include "soapStub.h"
#include "World.h"

class SoapMgr
{
public:
    SoapMgr() : m_running(false) { }

    void StartNetwork(std::string host, uint16 port);
    void StopNetwork();

private:
    void NetworkThread();

    std::string                      m_host;
    uint16                           m_port;
    bool                             m_running;
    std::shared_ptr<std::thread> m_networkThread;
};

class SOAPCommand
{
public:
    SOAPCommand() : finished(false), m_success(false) {}
    std::mutex localMutex;
    std::condition_variable conditionVariable;
    void appendToPrintBuffer(const char* msg)
    {
        m_printBuffer += msg;
    }
    void setCommandSuccess(bool val)
    {
        m_success = val;
    }
    bool hasCommandSucceeded()
    {
        return m_success;
    }
    static void print(void* callbackArg, const char* msg)
    {
        ((SOAPCommand*)callbackArg)->appendToPrintBuffer(msg);
    }
    static void commandFinished(void* callbackArg, bool success);
    bool m_success;
    bool finished;
    std::string m_printBuffer;
};

#endif // MANGOSSOAP_H
