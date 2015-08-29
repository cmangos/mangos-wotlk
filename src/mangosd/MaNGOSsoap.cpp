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
#include "MaNGOSsoap.h"

void SoapMgr::StartNetwork(std::string host, uint16 port)
{
    if (m_running)
        return;

    m_running = true;
    m_host = host;
    m_port = port;
    m_networkThread.reset(new std::thread(&SoapMgr::NetworkThread, this));
}

void SoapMgr::StopNetwork()
{
    if (!m_running)
        return;

    m_running = false;

    if (m_networkThread)
        m_networkThread->join();

    m_networkThread.reset();
}

void SoapMgr::NetworkThread()
{
    struct soap soap;
    int m;
    soap_init(&soap);
    soap_set_imode(&soap, SOAP_C_UTFSTRING);
    soap_set_omode(&soap, SOAP_C_UTFSTRING);
    m = soap_bind(&soap, m_host.c_str(), m_port, 100);

    // Check every 3 seconds if world ended
    soap.accept_timeout = 3;

    soap.recv_timeout = 5;
    soap.send_timeout = 5;
    if (m < 0)
    {
        sLog.outError("Soap: couldn't bind to %s:%d", m_host.c_str(), m_port);
        exit(-1);
    }

    sLog.outString("Soap: bound to http://%s:%d", m_host.c_str(), m_port);

    while (!World::IsStopped() && m_running)
    {
        int s = soap_accept(&soap);

        if (s < 0)
        {
            // Ran into an accept timeout
            continue;
        }

        DEBUG_LOG("Soap: accepted connection from IP = '%d.%d.%d.%d'", (int)(soap.ip >> 24) & 0xFF, (int)(soap.ip >> 16) & 0xFF, (int)(soap.ip >> 8) & 0xFF, (int)soap.ip & 0xFF);
        // Make a safe copy
        struct soap* thread_soap = soap_copy(&soap);

        // Process the message.
        soap_serve(thread_soap);
        soap_destroy(thread_soap); // Dealloc C++ data
        soap_end(thread_soap);     // Dealloc data and clean up
        soap_done(thread_soap);    // Detach soap struct
        free(thread_soap);
    }
    soap_done(&soap);
}

/*
Code used for generating stubs:

int ns1__executeCommand(char* command, char** result);
*/
int ns1__executeCommand(soap* soap, char* command, char** result)
{
    // security check
    if (!soap->userid || !soap->passwd)
    {
        DEBUG_LOG("Soap: Client didn't provide login information");
        return 401;
    }

    uint32 accountId = sAccountMgr.GetId(soap->userid);
    if (!accountId)
    {
        DEBUG_LOG("Soap: Client used invalid username '%s'", soap->userid);
        return 401;
    }

    if (!sAccountMgr.CheckPassword(accountId, soap->passwd))
    {
        DEBUG_LOG("Soap: invalid password for account '%s'", soap->userid);
        return 401;
    }

    if (sAccountMgr.GetSecurity(accountId) < SEC_ADMINISTRATOR)
    {
        DEBUG_LOG("Soap: %s's gmlevel is too low", soap->userid);
        return 403;
    }

    if (!command || !*command)
        return soap_sender_fault(soap, "Command mustn't be empty", "The supplied command was an empty string");

    DEBUG_LOG("Soap: got command '%s'", command);
    SOAPCommand connection;

    // Commands are executed in the world thread. We have to wait for them to be completed
    {
        // CliCommandHolder will be deleted from world, accessing after queuing is NOT save
        CliCommandHolder* cmd = new CliCommandHolder(accountId, SEC_CONSOLE, &connection, command, &SOAPCommand::print, &SOAPCommand::commandFinished);
        sWorld.QueueCliCommand(cmd);
    }

    // Wait for callback to complete command
    std::unique_lock<std::mutex> lock(connection.localMutex);
    while (!connection.finished)
        connection.conditionVariable.wait(lock);

    // Alright, command finished

    char* printBuffer = soap_strdup(soap, connection.m_printBuffer.c_str());
    if (connection.hasCommandSucceeded())
    {
        *result = printBuffer;
        return SOAP_OK;
    }
    else
        return soap_sender_fault(soap, printBuffer, printBuffer);
}

void SOAPCommand::commandFinished(void* soapconnection, bool success)
{
    SOAPCommand* con = (SOAPCommand*)soapconnection;
    con->setCommandSuccess(success);
    con->finished = true;
    con->conditionVariable.notify_one();
}

////////////////////////////////////////////////////////////////////////////////
//
//  Namespace Definition Table
//
////////////////////////////////////////////////////////////////////////////////

struct Namespace namespaces[] =
{
    { "SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/" }, // must be first
    { "SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/" }, // must be second
    { "xsi", "http://www.w3.org/1999/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance" },
    { "xsd", "http://www.w3.org/1999/XMLSchema",          "http://www.w3.org/*/XMLSchema" },
    { "ns1", "urn:MaNGOS" },     // "ns1" namespace prefix
    { nullptr, nullptr }
};
