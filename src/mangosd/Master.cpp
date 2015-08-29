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

#include <sstream>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include "AuctionHouseBot/AuctionHouseBot.h"
#include "CliRunnable.h"
#include "Common.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "DBCStores.h"
#include "FreezeDetector.h"
#include "Log.h"
#include "MassMailMgr.h"
#include "MaNGOSsoap.h"
#include "MapManager.h"
#include "ProgressBar.h"
#include "RemoteAdministration.h"
#include "revision.h"
#include "revision_nr.h"
#include "revision_sql.h"
#include "SystemConfig.h"
#include "Util.h"
#include "WorldSocketMgr.h"

#define WORLD_SLEEP_CONST 50

boost::asio::io_service IoService;

#ifdef WIN32
#   include "ServiceWin32.h"
char serviceName[] = "mangosd";
char serviceLongName[] = "MaNGOS world service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int serviceStatus = -1;
#else
#   include "PosixDaemon.h"
#endif

DatabaseType WorldDatabase;     // Accessor to the world database
DatabaseType CharacterDatabase; // Accessor to the character database
DatabaseType LoginDatabase;     // Accessor to the realm/login database

uint32 realmID;                 // Id of the realm

bool StartDatabase();
void ClearOnlineAccounts();

void SignalHandler(const boost::system::error_code& error, int signal_number)
{
    if (!error)
    {
        if (signal_number == SIGINT)
            World::StopNow(RESTART_EXIT_CODE);
        else
            World::StopNow(SHUTDOWN_EXIT_CODE);
    }
}

// Clear 'online' status for all accounts with characters in this realm
void ClearOnlineAccounts()
{
    // Cleanup online status for characters hosted at current realm
    // \todo Only accounts with characters logged on *this* realm should have online status reset. Move the online column from 'account' to 'realmcharacters'?
    LoginDatabase.PExecute("UPDATE account SET active_realm_id = 0 WHERE active_realm_id = '%u'", realmID);
    CharacterDatabase.Execute("UPDATE characters SET online = 0 WHERE online<>0");
    // Battleground instance ids reset at server restart
    CharacterDatabase.Execute("UPDATE character_battleground_data SET instance_id = 0");
}

bool StartDatabase()
{
    // Get world database info from configuration file
    std::string dbstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    int nConnections = sConfig.GetIntDefault("WorldDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Database not specified in configuration file");
        return false;
    }
    sLog.outString("World Database total connections: %i", nConnections + 1);
    // Initialise the world database
    if (!WorldDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to world database %s", dbstring.c_str());
        return false;
    }
    if (!WorldDatabase.CheckRequiredField("db_version", REVISION_DB_MANGOS))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }
    dbstring = sConfig.GetStringDefault("CharacterDatabaseInfo", "");
    nConnections = sConfig.GetIntDefault("CharacterDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Character Database not specified in configuration file");
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }
    sLog.outString("Character Database total connections: %i", nConnections + 1);
    // Initialise the Character database
    if (!CharacterDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to Character database %s", dbstring.c_str());
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }
    if (!CharacterDatabase.CheckRequiredField("character_db_version", REVISION_DB_CHARACTERS))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }
    // Get login database info from configuration file
    dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    nConnections = sConfig.GetIntDefault("LoginDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Login database not specified in configuration file");
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }
    // Initialise the login database
    sLog.outString("Login Database total connections: %i", nConnections + 1);
    if (!LoginDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to login database %s", dbstring.c_str());
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }
    if (!LoginDatabase.CheckRequiredField("realmd_db_version", REVISION_DB_REALMD))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        LoginDatabase.HaltDelayThread();
        return false;
    }
    // Get the realm Id from the configuration file
    realmID = sConfig.GetIntDefault("RealmID", 0);
    if (!realmID)
    {
        sLog.outError("Realm ID not defined in configuration file");
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        LoginDatabase.HaltDelayThread();
        return false;
    }
    sLog.outString("Realm running as realm ID %d", realmID);
    // Clean the database before starting
    ClearOnlineAccounts();
    sWorld.LoadDBVersion();
    sLog.outString("Using World DB: %s", sWorld.GetDBVersion());
    sLog.outString("Using creature EventAI: %s", sWorld.GetCreatureEventAIVersion());
    return true;
}

/// Print out the usage string for this program on the console.
void usage(boost::program_options::options_description const& desc, const char* prog)
{
    std::ostringstream ss;
    ss << desc;
    sLog.outString("Usage: \n %s [<options>]\n%s", prog, ss.str().c_str());
}

/// Handle program arguments
int HandleProgramArguments(int argc, char** argv)
{
    std::string cfg_file;
    std::string serviceDaemonMode;

    boost::program_options::options_description description("Allowed options");
    description.add_options()
        ("version,v", "print version and exit")
        ("help,h", "print commandline help and exit")
        ("config,c", boost::program_options::value<std::string>(&cfg_file)->default_value(_MANGOSD_CONFIG), "use as configuration file")
        ("ahbot,a", boost::program_options::value<std::string>(), "use as ahbot configuration file")
#ifdef WIN32
        ("service,s", boost::program_options::value<std::string>(&serviceDaemonMode), "running as service, arg functions: run, install, uninstall")
#else
        ("service,s", boost::program_options::value<std::string>(&serviceDaemonMode), "running as daemon, arg functions: run, stop")
#endif
        ;

    boost::program_options::variables_map vm;

    try
    {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
                                      options(description).run(), vm);
        boost::program_options::notify(vm);

    }
    catch (boost::program_options::unknown_option const& ex)
    {
        sLog.outError("Runtime-Error: unknown option %s", ex.get_option_name().c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }
    catch (boost::program_options::invalid_command_line_syntax const& ex)
    {
        sLog.outError("Runtime-Error: invalid syntax for option %s", ex.get_option_name().c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    if (vm.count("version"))
    {
        printf("%s\n", _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID));
        return 0;
    }

    if (vm.count("help"))
    {
        usage(description, argv[0]);
        return 0;
    }

    if (vm.count("ahbot"))
        sAuctionBotConfig.SetConfigFileName(vm["ahbot"].as<std::string>().c_str());

    if (!serviceDaemonMode.empty())
    {
#ifdef WIN32
        char const* const serviceModes[] = { "run", "install", "uninstall", nullptr };
#else
        char const* const serviceModes[] = { "run", "stop", nullptr };
#endif
        char const* const* mode_ptr = &serviceModes[0];
        for (; *mode_ptr != nullptr; ++mode_ptr)
            if (*mode_ptr == serviceDaemonMode)
                break;

        if (!*mode_ptr)
        {
            sLog.outError("Runtime-Error: -s unsupported argument %s", serviceDaemonMode.c_str());
            usage(description, argv[0]);
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }
    }

    // Windows service command needs execute before config read
#ifdef WIN32
    switch (serviceDaemonMode[0])
    {
        case 'i':
            if (WinServiceInstall())
                sLog.outString("Installing service");
            return 1;
        case 'u':
            if (WinServiceUninstall())
                sLog.outString("Uninstalling service");
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif

    if (!sConfig.SetSource(cfg_file, "MangosdConf"))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file.c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    // Posix daemon command needs apply after config read
#ifndef WIN32
    switch (serviceDaemonMode[0])
    {
        case 'r':
            startDaemon();
            break;
        case 's':
            stopDaemon();
            break;
    }
#endif

    return 2;
}

void ShowHint()
{
    sLog.outString("%s [world-daemon]", _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID));
    sLog.outString("<Ctrl-C> to stop.");
    sLog.outString("\n\n"
                   "       _____     __  __       _   _  _____  ____   _____ \n"
                   "      / ____|   |  \\/  |     | \\ | |/ ____|/ __ \\ / ____|\n"
                   "     | |        | \\  / |     |  \\| | |  __  |  | | (___  \n"
                   "     | |ontinued| |\\/| | __ _| . ` | | |_ | |  | |\\___ \\ \n"
                   "     | |____    | |  | |/ _` | |\\  | |__| | |__| |____) |\n"
                   "      \\_____|   |_|  |_| (_| |_| \\_|\\_____|\\____/ \\____/ \n"
                   "      http://cmangos.net\\__,_|     Doing things right!\n\n");

    sLog.outString("Using configuration file %s.", sConfig.GetFilename().c_str());

    DETAIL_LOG("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL)
    {
        DETAIL_LOG("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
        DETAIL_LOG("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    DETAIL_LOG("Using BOOST: %i.%i.%i", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
}

void SetProcessPriority(uint32 affinity, bool high_priority)
{
#ifdef WIN32
    HANDLE hProcess = GetCurrentProcess();
    if (affinity > 0)
    {
        ULONG_PTR application_affinity;
        ULONG_PTR system_affinity;
        if (GetProcessAffinityMask(hProcess, &application_affinity, &system_affinity))
        {
            // Remove non accessible processors
            ULONG_PTR current_affinity = affinity & application_affinity;
            if (!current_affinity)
                sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for realmd. Accessible processors bitmask (hex): %x", affinity, application_affinity);
            else if (SetProcessAffinityMask(hProcess, current_affinity))
                sLog.outString("Using processors (bitmask, hex): %x", current_affinity);
            else
                sLog.outError("Can't set used processors (hex): %x", current_affinity);
        }
    }
    if (high_priority)
    {
        if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
            sLog.outString("Service process priority class set to HIGH");
        else
            sLog.outError("Can't set service process priority class.");
    }
#endif
}

void WorldUpdateLoop()
{
    uint32 realCurrTime = 0;
    uint32 realPrevTime = WorldTimer::tick();
    uint32 prevSleepTime = 0; // Used for balanced full tick time length near WORLD_SLEEP_CONST
    // While we have not World::m_stopEvent, update the world
    while (!World::IsStopped())
    {
        ++World::m_worldLoopCounter;
        realCurrTime = WorldTimer::getMSTime();
        uint32 diff = WorldTimer::tick();
        sWorld.Update(diff);
        realPrevTime = realCurrTime;
        // diff (D0) include time of previous sleep (d0) + tick time (t0)
        // we want that next d1 + t1 == WORLD_SLEEP_CONST
        // we can't know next t1 and then can use (t0 + d1) == WORLD_SLEEP_CONST requirement
        // d1 = WORLD_SLEEP_CONST - t0 = WORLD_SLEEP_CONST - (D0 - d0) = WORLD_SLEEP_CONST + d0 - D0
        if (diff <= WORLD_SLEEP_CONST + prevSleepTime)
        {
            prevSleepTime = WORLD_SLEEP_CONST + prevSleepTime - diff;
            MaNGOS::Thread::Sleep(prevSleepTime);
        }
        else
            prevSleepTime = 0;

#ifdef WIN32
        if (serviceStatus == 0)
            World::StopNow(SHUTDOWN_EXIT_CODE);
        while (serviceStatus == 2)
            Sleep(1000);
#endif
    }
}

extern int main(int argc, char** argv)
{
    // Check and handle program arguments
    int result = HandleProgramArguments(argc, argv);
    if (result < 2)
        return result;

    // Show logo and some info
    ShowHint();

    /// Set progress bars show mode
    BarGoLink::SetOutputState(sConfig.GetBoolDefault("ShowProgressBars", false));

    // World PID file creation
    std::string pidfile = sConfig.GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog.outError("Cannot create PID file %s.\n", pidfile.c_str());
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }

        sLog.outString("Daemon PID: %u\n", pid);
    }

    // Start the databases
    if (!StartDatabase())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    // Initialize the World
    sWorld.SetInitialWorldSettings();

#ifndef WIN32
    detachDaemon();
    boost::asio::signal_set signals(IoService, SIGINT, SIGTERM);
#else
    boost::asio::signal_set signals(IoService, SIGINT, SIGTERM, SIGBREAK);
#endif
    // Server loaded successfully => enable async DB requests
    // This is done to forbid any async transactions during server startup!
    CharacterDatabase.AllowAsyncTransactions();
    WorldDatabase.AllowAsyncTransactions();
    LoginDatabase.AllowAsyncTransactions();

    // Register a signal handler to catch shutdown event.
    signals.async_wait(SignalHandler);

    // Set realmbuilds depend on mangosd expected builds, and set server online
    {
        std::string builds = AcceptableClientBuildsListStr();
        LoginDatabase.escape_string(builds);
        LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags & ~(%u), population = 0, realmbuilds = '%s'  WHERE id = '%u'", REALM_FLAG_OFFLINE, builds.c_str(), realmID);
    }

    std::auto_ptr<RemoteAdminSocketMgr> RemoteAdminMgr(new RemoteAdminSocketMgr());
    if (sConfig.GetBoolDefault("Ra.Enable", false))
    {
        uint16 raport = sConfig.GetIntDefault("Ra.Port", 3443);
        std::string stringip = sConfig.GetStringDefault("Ra.IP", "0.0.0.0");
        if (!RemoteAdminMgr->StartNetwork(raport, stringip))
        {
            sLog.outError("MaNGOS Remote administration can not bind to %s:%d", stringip.c_str(), raport);
            Log::WaitBeforeContinueIfNeed();
            World::StopNow(ERROR_EXIT_CODE);
            // go down and shutdown the server
        }
        else
        {
            sLog.outString("Starting Remote access listener on port %d on %s", raport, stringip.c_str());
        }
    }

    // Handle affinity for multiple processors and process priority on Windows
    SetProcessPriority(sConfig.GetIntDefault("UseProcessors", 0), sConfig.GetBoolDefault("ProcessPriority", false));

    // Start soap serving thread
    MaNGOS::Thread* soap_thread = nullptr;
    std::auto_ptr<SoapMgr> soapMgr(new SoapMgr());
    if (sConfig.GetBoolDefault("SOAP.Enabled", false))
    {
        soapMgr->StartNetwork(sConfig.GetStringDefault("SOAP.IP", "127.0.0.1"), sConfig.GetIntDefault("SOAP.Port", 7878));
    }

    // Start up freeze catcher thread
    MaNGOS::Thread* freeze_thread = nullptr;
    if (uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
    {
        FreezeDetectorRunnable* fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay * 1000);
        freeze_thread = new MaNGOS::Thread(fdr);
        freeze_thread->setPriority(MaNGOS::Priority_Highest);
    }

    // Launch the world listener socket
    uint16 wsport = sWorld.getConfig(CONFIG_UINT32_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault("BindIP", "0.0.0.0");
    if (!sWorldSocketMgr.StartNetwork(wsport, bind_ip))
    {
        sLog.outError("Failed to start network");
        Log::WaitBeforeContinueIfNeed();
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    // Init new SQL thread for the world database
    WorldDatabase.ThreadStart();                            // let thread do safe mySQL requests (one connection call enough)
    sWorld.InitResultQueue();

    MaNGOS::Thread* cliThread = nullptr;

#ifdef WIN32
    if (sConfig.GetBoolDefault("Console.Enable", true) && (serviceStatus == -1)/* need disable console in service mode*/)
#else
    if (sConfig.GetBoolDefault("Console.Enable", true))
#endif
    {
        // Launch CliRunnable thread
        cliThread = new MaNGOS::Thread(new CliRunnable);
    }

    // Run main world loop
    WorldUpdateLoop();

    IoService.stop();
    sWorld.CleanupsBeforeStop();

    sWorldSocketMgr.StopNetwork();

    RemoteAdminMgr->StopNetwork();

    MapManager::Instance().UnloadAll();                     // unload all grids (including locked in memory)

    // End the database thread
    WorldDatabase.ThreadEnd();                              // free mySQL thread resources

    // Stop freeze protection before shutdown tasks
    if (freeze_thread)
    {
        freeze_thread->destroy();
        delete freeze_thread;
    }

    // Stop soap thread
    if (soapMgr.get())
    {
        soapMgr->StopNetwork();
    }

    // Set server offline in realmlist
    LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags | %u WHERE id = '%u'", REALM_FLAG_OFFLINE, realmID);

    // Clean account database before leaving
    ClearOnlineAccounts();

    // send all still queued mass mails (before DB connections shutdown)
    sMassMailMgr.Update(true);

    // Wait for DB delay threads to end
    CharacterDatabase.HaltDelayThread();
    WorldDatabase.HaltDelayThread();
    LoginDatabase.HaltDelayThread();

    sLog.outString("Halting process...");

    if (cliThread)
    {
#ifdef WIN32

        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        BOOL ret = WriteConsoleInput(hStdIn, b, 4, &numb);

        cliThread->wait();
#else
        cliThread->destroy();
#endif
        delete cliThread;
    }

    // Exit the process with specified return value
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart mangosd
    return World::GetExitCode();
}

