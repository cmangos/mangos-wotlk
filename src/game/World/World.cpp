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

/** \file
    \ingroup world
*/

#include "World/World.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "Models/M2Stores.h"
#include "Platform/Define.h"
#include "SystemConfig.h"
#include "Log/Log.h"
#include "Server/Opcodes.h"
#include "Server/WorldSession.h"
#include "Server/WorldPacket.h"
#include "Entities/Player.h"
#include "Skills/SkillExtraItems.h"
#include "Skills/SkillDiscovery.h"
#include "Accounts/AccountMgr.h"
#include "Achievements/AchievementMgr.h"
#include "AuctionHouse/AuctionHouseMgr.h"
#include "Globals/ObjectMgr.h"
#include "AI/EventAI/CreatureEventAIMgr.h"
#include "Guilds/GuildMgr.h"
#include "Spells/SpellMgr.h"
#include "Chat/Chat.h"
#include "Server/DBCStores.h"
#include "Mails/MassMailMgr.h"
#include "Loot/LootMgr.h"
#include "Entities/ItemEnchantmentMgr.h"
#include "Maps/MapManager.h"
#include "DBScripts/ScriptMgr.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/CreatureAIRegistry.h"
#include "Policies/Singleton.h"
#include "BattleGround/BattleGroundMgr.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "Vmap/VMapFactory.h"
#include "MotionGenerators/MoveMap.h"
#include "GameEvents/GameEventMgr.h"
#include "Pools/PoolManager.h"
#include "Database/DatabaseImpl.h"
#include "Grids/GridNotifiersImpl.h"
#include "Grids/CellImpl.h"
#include "Maps/MapPersistentStateMgr.h"
#include "MotionGenerators/WaypointManager.h"
#include "GMTickets/GMTicketMgr.h"
#include "Util/Util.h"
#include "Tools/CharacterDatabaseCleaner.h"
#include "Entities/CreatureLinkingMgr.h"
#include "Calendar/Calendar.h"
#include "Weather/Weather.h"
#include "World/WorldState.h"
#include "Cinematics/CinematicMgr.h"
#include "Maps/TransportMgr.h"
#include "Anticheat/Anticheat.hpp"
#include "LFG/LFGMgr.h"
#include "Vmap/GameObjectModel.h"
#include "Spells/SpellStacking.h"

#ifdef BUILD_AHBOT
 #include "AuctionHouseBot/AuctionHouseBot.h"
#endif

#ifdef BUILD_METRICS
 #include "Metric/Metric.h"
#endif

#ifdef ENABLE_PLAYERBOTS
#include "ahbot/AhBot.h"
#include "playerbot/PlayerbotAIConfig.h"
#include "playerbot/RandomPlayerbotMgr.h"
#endif

#include <algorithm>
#include <mutex>

INSTANTIATE_SINGLETON_1(World);

volatile bool World::m_stopEvent = false;
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
volatile uint32 World::m_worldLoopCounter = 0;

float World::m_MaxVisibleDistanceOnContinents = DEFAULT_VISIBILITY_DISTANCE;
float World::m_MaxVisibleDistanceInInstances  = DEFAULT_VISIBILITY_INSTANCE;
float World::m_MaxVisibleDistanceInBGArenas   = DEFAULT_VISIBILITY_BGARENAS;

float  World::m_relocation_lower_limit_sq = 10.f * 10.f;
uint32 World::m_relocation_ai_notify_delay = 1000u;

uint32 World::m_currentMSTime = 0;
TimePoint World::m_currentTime = TimePoint();
uint32 World::m_currentDiff = 0;
#ifdef ENABLE_PLAYERBOTS
uint32 World::m_currentDiffSum = 0;
uint32 World::m_currentDiffSumIndex = 0;
uint32 World::m_averageDiff = 0;
uint32 World::m_maxDiff = 0;
std::list<uint32> World::m_histDiff;
#endif

/// World constructor
World::World() : mail_timer(0), mail_timer_expires(0), m_NextDailyQuestReset(0), m_NextWeeklyQuestReset(0), m_NextMonthlyQuestReset(0), m_opcodeCounters(NUM_MSG_TYPES)
{
    m_playerLimit = 0;
    m_allowMovement = true;
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_gameTime = time(nullptr);
    m_startTime = m_gameTime;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;

    m_defaultDbcLocale = DEFAULT_LOCALE;
    m_availableDbcLocaleMask = 0;

    for (unsigned int& m_configUint32Value : m_configUint32Values)
        m_configUint32Value = 0;

    for (int& m_configInt32Value : m_configInt32Values)
        m_configInt32Value = 0;

    for (float& m_configFloatValue : m_configFloatValues)
        m_configFloatValue = 0.0f;

    for (bool& m_configBoolValue : m_configBoolValues)
        m_configBoolValue = false;
}

/// World destructor
World::~World()
{
    // it is assumed that no other thread is accessing this data when the destructor is called.  therefore, no locks are necessary

    ///- Empty the kicked session set
    for (auto const session : m_sessions)
        delete session.second;

    for (auto const cliCommand : m_cliCommandQueue)
        delete cliCommand;

    for (auto const session : m_sessionAddQueue)
        delete session;

    VMAP::VMapFactory::clear();
    MMAP::MMapFactory::clear();

    if (m_lfgQueueThread.joinable())
        m_lfgQueueThread.join();
    if (m_bgQueueThread.joinable())
        m_bgQueueThread.join();
}

/// Cleanups before world stop
void World::CleanupsBeforeStop()
{
#ifdef ENABLE_PLAYERBOTS
    sRandomPlayerbotMgr.LogoutAllBots();
#endif
    KickAll(true);                                   // save and kick all players
    UpdateSessions(1);                               // real players unload required UpdateSessions call
    sBattleGroundMgr.DeleteAllBattleGrounds();       // unload battleground templates before different singletons destroyed
    sMapMgr.UnloadAll();                             // unload all grids (including locked in memory)
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;                                 // also can return nullptr for kicked session
    return nullptr;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;
        itr->second->KickPlayer();
    }

    return true;
}

void World::AddSession(WorldSession* s)
{
    std::lock_guard<std::mutex> guard(m_sessionAddQueueLock);

    m_sessionAddQueue.push_back(s);
}

void
World::AddSession_(WorldSession* s)
{
    MANGOS_ASSERT(s);

    if (FindSession(s->GetAccountId()))
    {
        sLog.outError("Trying to add an already existing session");
        return;
    }

    // NOTE - Still there is race condition in WorldSession* being used in the Sockets

    ///- kick already loaded player with same account (if any) and remove session
    ///- if player is in loading and want to load again, return
    if (!RemoveSession(s->GetAccountId()))
    {
        s->KickPlayer();
        delete s;                                           // session not added yet in session list, so not listed in queue
        return;
    }

    // decrease session counts only at not reconnection case
    bool decrease_session = true;

    // if session already exist, prepare to it deleting at next world update
    // NOTE - KickPlayer() should be called on "old" in RemoveSession()
    {
        SessionMap::const_iterator old = m_sessions.find(s->GetAccountId());

        if (old != m_sessions.end())
        {
            // prevent decrease sessions count if session queued
            if (RemoveQueuedSession(old->second))
                decrease_session = false;
            // do not remove replaced session from queue if listed
            delete old->second;
        }
    }

    m_sessions[s->GetAccountId()] = s;

    uint32 Sessions = GetActiveAndQueuedSessionCount();
    uint32 pLimit = GetPlayerAmountLimit();
    uint32 QueueSize = GetQueuedSessionCount();             // number of players in the queue

    // so we don't count the user trying to
    // login as a session and queue the socket that we are using
    if (decrease_session)
        --Sessions;

    if (m_uniqueSessionCount.find(s->GetAccountId()) == m_uniqueSessionCount.end())
        m_uniqueSessionCount.insert(s->GetAccountId());

    if (pLimit > 0 && Sessions >= pLimit && s->GetSecurity() == SEC_PLAYER)
    {
        AddQueuedSession(s);
        UpdateMaxSessionCounters();
        DETAIL_LOG("PlayerQueue: Account id %u is in Queue Position (%u).", s->GetAccountId(), ++QueueSize);
        return;
    }

    WorldPacket pkt(SMSG_CLIENTCACHE_VERSION, 4);
    pkt << uint32(getConfig(CONFIG_UINT32_CLIENTCACHE_VERSION));
    s->SendPacket(pkt);

    s->SendTutorialsData();

    UpdateMaxSessionCounters();

    // Updates the population
    if (pLimit > 0)
    {
        float popu = float(GetActiveSessionCount());        // updated number of users on the server
        popu /= pLimit;
        popu *= 2;

        static SqlStatementID id;

        SqlStatement stmt = LoginDatabase.CreateStatement(id, "UPDATE realmlist SET population = ? WHERE id = ?");
        stmt.PExecute(popu, realmID);

        DETAIL_LOG("Server Population (%f).", popu);
    }
}

int32 World::GetQueuedSessionPos(WorldSession const* sess) const
{
    uint32 position = 1;

    for (Queue::const_iterator iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void World::AddQueuedSession(WorldSession* sess)
{
    sess->SetInQueue(true);
    m_QueuedSessions.push_back(sess);

    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4 + 1 + 4 + 1);
    packet << uint8(AUTH_WAIT_QUEUE);
    packet << uint32(0);                                    // BillingTimeRemaining
    packet << uint8(0);                                     // BillingPlanFlags
    packet << uint32(0);                                    // BillingTimeRested
    packet << uint8(sess->GetExpansion());                     // 0 - normal, 1 - TBC, must be set in database manually for each account
    packet << uint32(GetQueuedSessionPos(sess));            // position in queue
    packet << uint8(0);                                     // unk 3.3.0
    sess->SendPacket(packet);
}

bool World::RemoveQueuedSession(WorldSession* sess)
{
    // sessions count including queued to remove (if removed_session set)
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    Queue::iterator iter = m_QueuedSessions.begin();

    // search to remove and count skipped positions
    bool found = false;

    for (; iter != m_QueuedSessions.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            iter = m_QueuedSessions.erase(iter);
            found = true;                                   // removing queued session
            break;
        }
    }

    // iter point to next socked after removed or end()
    // position store position of removed socket and then new position next socket after removed

    // if session not queued then we need decrease sessions count
    if (!found && sessions)
        --sessions;

    // accept first in queue
    if ((!m_playerLimit || (int32)sessions < m_playerLimit) && !m_QueuedSessions.empty())
    {
        WorldSession* pop_sess = m_QueuedSessions.front();
        pop_sess->SetInQueue(false);
        pop_sess->SendAuthWaitQue(0);

        WorldPacket pkt(SMSG_CLIENTCACHE_VERSION, 4);
        pkt << uint32(getConfig(CONFIG_UINT32_CLIENTCACHE_VERSION));
        pop_sess->SendPacket(pkt);

        pop_sess->SendAccountDataTimes(GLOBAL_CACHE_MASK);
        pop_sess->SendTutorialsData();

        m_QueuedSessions.pop_front();

        // update iter to point first queued socket or end() if queue is empty now
        iter = m_QueuedSessions.begin();
        position = 1;
    }

    // update position from iter to end()
    // iter point to first not updated socket, position store new position
    for (; iter != m_QueuedSessions.end(); ++iter, ++position)
        (*iter)->SendAuthWaitQue(position);

    return found;
}

/// Initialize config values
void World::LoadConfigSettings(bool reload)
{
    if (reload)
    {
        if (!sConfig.Reload())
        {
            sLog.outError("World settings reload fail: can't read settings from %s.", sConfig.GetFilename().c_str());
            return;
        }
    }

    ///- Read the version of the configuration file and warn the user in case of emptiness or mismatch
    uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if (!confVersion)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: mangosd.conf does not include a ConfVersion variable.");
        sLog.outError("          Your configuration file may be out of date!");
        sLog.outError("*****************************************************************************");
        Log::WaitBeforeContinueIfNeed();
    }
    else
    {
        if (confVersion < _MANGOSDCONFVERSION)
        {
            sLog.outError("*****************************************************************************");
            sLog.outError(" WARNING: Your mangosd.conf version indicates your conf file is out of date!");
            sLog.outError("          Please check for updates, as your current default values may cause");
            sLog.outError("          unexpected behavior.");
            sLog.outError("*****************************************************************************");
            Log::WaitBeforeContinueIfNeed();
        }
    }

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerLimit(sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT), true);
    SetMotd(sConfig.GetStringDefault("Motd", "Welcome to the Massive Network Game Object Server."));

    ///- Read all rates from the config file
    setConfigPos(CONFIG_FLOAT_RATE_HEALTH,                               "Rate.Health",                               1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_MANA,                           "Rate.Mana",                                 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_RAGE_INCOME,                    "Rate.Rage.Income",                          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_RAGE_LOSS,                      "Rate.Rage.Loss",                            1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_RUNICPOWER_INCOME,              "Rate.RunicPower.Income",                    1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_RUNICPOWER_LOSS,                "Rate.RunicPower.Loss",                      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_FOCUS,                          "Rate.Focus",                                1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_ENERGY,                         "Rate.Energy",                               1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_SKILL_DISCOVERY,                      "Rate.Skill.Discovery",                      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_POOR,                       "Rate.Drop.Item.Poor",                       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_NORMAL,                     "Rate.Drop.Item.Normal",                     1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_UNCOMMON,                   "Rate.Drop.Item.Uncommon",                   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_RARE,                       "Rate.Drop.Item.Rare",                       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_EPIC,                       "Rate.Drop.Item.Epic",                       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_LEGENDARY,                  "Rate.Drop.Item.Legendary",                  1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_ARTIFACT,                   "Rate.Drop.Item.Artifact",                   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_REFERENCED,                 "Rate.Drop.Item.Referenced",                 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_QUEST,                      "Rate.Drop.Item.Quest",                      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_MONEY,                           "Rate.Drop.Money",                           1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_PET_XP_KILL,                          "Rate.Pet.XP.Kill",                          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_PET_XP_KILL_VANILLA,                  "Rate.Pet.XP.Kill.Vanilla",                  1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_PET_XP_KILL_BC,                       "Rate.Pet.XP.Kill.BC",                       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_PET_XP_KILL_WOTLK,                    "Rate.Pet.XP.Kill.Wrath",                    1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_KILL,                              "Rate.XP.Kill",                              1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_KILL_VANILLA,                      "Rate.XP.Kill.Vanilla",                      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_KILL_BC,                           "Rate.XP.Kill.BC",                           1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_KILL_WOTLK,                        "Rate.XP.Kill.Wrath",                        1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_QUEST,                             "Rate.XP.Quest",                             1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_QUEST_VANILLA,                     "Rate.XP.Quest.Vanilla",                     1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_QUEST_BC,                          "Rate.XP.Quest.BC",                          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_QUEST_WOTLK,                       "Rate.XP.Quest.Wrath",                       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_EXPLORE,                           "Rate.XP.Explore",                           1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_EXPLORE_VANILLA,                   "Rate.XP.Explore.Vanilla",                   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_EXPLORE_BC,                        "Rate.XP.Explore.BC",                        1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_XP_EXPLORE_WOTLK,                     "Rate.XP.Explore.Wrath",                     1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_REPUTATION_GAIN,                      "Rate.Reputation.Gain",                      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_KILL,             "Rate.Reputation.LowLevel.Kill",             1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_QUEST,            "Rate.Reputation.LowLevel.Quest",            1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_DAMAGE,               "Rate.Creature.Normal.Damage",               1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_DAMAGE,          "Rate.Creature.Elite.Elite.Damage",          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_DAMAGE,      "Rate.Creature.Elite.RAREELITE.Damage",      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,      "Rate.Creature.Elite.WORLDBOSS.Damage",      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_DAMAGE,           "Rate.Creature.Elite.RARE.Damage",           1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_HP,                   "Rate.Creature.Normal.HP",                   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_HP,              "Rate.Creature.Elite.Elite.HP",              1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_HP,          "Rate.Creature.Elite.RAREELITE.HP",          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_HP,          "Rate.Creature.Elite.WORLDBOSS.HP",          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_HP,               "Rate.Creature.Elite.RARE.HP",               1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_SPELLDAMAGE,          "Rate.Creature.Normal.SpellDamage",          1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,     "Rate.Creature.Elite.Elite.SpellDamage",     1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE, "Rate.Creature.Elite.RAREELITE.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE, "Rate.Creature.Elite.WORLDBOSS.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,      "Rate.Creature.Elite.RARE.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_AGGRO, "Rate.Creature.Aggro", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_INGAME,                    "Rate.Rest.InGame", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_OFFLINE_IN_TAVERN_OR_CITY, "Rate.Rest.Offline.InTavernOrCity", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_OFFLINE_IN_WILDERNESS,     "Rate.Rest.Offline.InWilderness", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DAMAGE_FALL,  "Rate.Damage.Fall", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_AUCTION_TIME, "Rate.Auction.Time", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_AUCTION_DEPOSIT, "Rate.Auction.Deposit", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_AUCTION_CUT,     "Rate.Auction.Cut", 1.0f);
    setConfig(CONFIG_UINT32_AUCTION_DEPOSIT_MIN, "Auction.Deposit.Min", SILVER);
    setConfig(CONFIG_FLOAT_RATE_HONOR, "Rate.Honor", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_MINING_AMOUNT, "Rate.Mining.Amount", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_MINING_NEXT,   "Rate.Mining.Next", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_INSTANCE_RESET_TIME, "Rate.InstanceResetTime", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_TALENT, "Rate.Talent", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CORPSE_DECAY_LOOTED, "Rate.Corpse.Decay.Looted", 0.0f);

    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_DAMAGE, "DurabilityLossChance.Damage", 0.5f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_ABSORB, "DurabilityLossChance.Absorb", 0.5f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_PARRY,  "DurabilityLossChance.Parry",  0.05f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_BLOCK,  "DurabilityLossChance.Block",  0.05f);

    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_SAY,       "ListenRange.Say",       25.0f);
    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_YELL,      "ListenRange.Yell",     300.0f);
    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE, "ListenRange.TextEmote", 25.0f);

    setConfigPos(CONFIG_FLOAT_GROUP_XP_DISTANCE, "MaxGroupXPDistance", 74.0f);
    setConfigPos(CONFIG_FLOAT_SIGHT_GUARDER,     "GuarderSight",       50.0f);
    setConfigPos(CONFIG_FLOAT_SIGHT_MONSTER,     "MonsterSight",       50.0f);

    setConfigPos(CONFIG_FLOAT_CREATURE_FAMILY_ASSISTANCE_RADIUS,      "CreatureFamilyAssistanceRadius",     5.0f);
    setConfigPos(CONFIG_FLOAT_CREATURE_CHECK_FOR_HELP_RADIUS,         "CreatureCheckForHelpRadius",     5.0f);
    setConfig(CONFIG_UINT32_CREATURE_CHECK_FOR_HELP_AGGRO_DELAY,      "CreatureCheckForHelpAggroDelay",     2000);
    setConfig(CONFIG_UINT32_CREATURE_LINKING_AGGRO_DELAY,             "CreatureLinkingAggroDelay",     2000);
    setConfigPos(CONFIG_FLOAT_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS, "CreatureFamilyFleeAssistanceRadius", 30.0f);

    ///- Read other configuration items from the config file
    setConfigMinMax(CONFIG_UINT32_COMPRESSION, "Compression", 1, 1, 9);
    setConfig(CONFIG_BOOL_ADDON_CHANNEL, "AddonChannel", true);
    setConfig(CONFIG_BOOL_CLEAN_CHARACTER_DB, "CleanCharacterDB", true);
    setConfig(CONFIG_BOOL_GRID_UNLOAD, "GridUnload", true);
    setConfig(CONFIG_UINT32_MAX_WHOLIST_RETURNS, "MaxWhoListReturns", 49);

    std::string forceLoadGridOnMaps = sConfig.GetStringDefault("LoadAllGridsOnMaps");
    if (!forceLoadGridOnMaps.empty())
    {
        unsigned int pos = 0;
        unsigned int id;
        VMAP::VMapFactory::chompAndTrim(forceLoadGridOnMaps);
        while (VMAP::VMapFactory::getNextId(forceLoadGridOnMaps, pos, id))
            m_configForceLoadMapIds.insert(id);
    }

    setConfig(CONFIG_BOOL_AUTOLOAD_ACTIVE, "Autoload.Active", true);
    setConfig(CONFIG_BOOL_SPECIALS_ACTIVE, "Specials.Active", false);

    setConfig(CONFIG_UINT32_INTERVAL_SAVE, "PlayerSave.Interval", 15 * MINUTE * IN_MILLISECONDS);
    setConfigMinMax(CONFIG_UINT32_MIN_LEVEL_STAT_SAVE, "PlayerSave.Stats.MinLevel", 0, 0, MAX_LEVEL);
    setConfig(CONFIG_BOOL_STATS_SAVE_ONLY_ON_LOGOUT, "PlayerSave.Stats.SaveOnlyOnLogout", true);

    setConfigMin(CONFIG_UINT32_INTERVAL_GRIDCLEAN, "GridCleanUpDelay", 5 * MINUTE * IN_MILLISECONDS, MIN_GRID_DELAY);
    if (reload)
        sMapMgr.SetGridCleanUpDelay(getConfig(CONFIG_UINT32_INTERVAL_GRIDCLEAN));

    setConfigMin(CONFIG_UINT32_INTERVAL_MAPUPDATE, "MapUpdateInterval", 100, MIN_MAP_UPDATE_DELAY);
    if (reload)
        sMapMgr.SetMapUpdateInterval(getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));

    setConfig(CONFIG_UINT32_INTERVAL_CHANGEWEATHER, "ChangeWeatherInterval", 10 * MINUTE * IN_MILLISECONDS);

    if (configNoReload(reload, CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT))
        setConfig(CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT);

    if (configNoReload(reload, CONFIG_UINT32_GAME_TYPE, "GameType", 0))
        setConfig(CONFIG_UINT32_GAME_TYPE, "GameType", 0);

    if (configNoReload(reload, CONFIG_UINT32_REALM_ZONE, "RealmZone", REALM_ZONE_DEVELOPMENT))
        setConfig(CONFIG_UINT32_REALM_ZONE, "RealmZone", REALM_ZONE_DEVELOPMENT);

    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_ACCOUNTS,            "AllowTwoSide.Accounts", true);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT,    "AllowTwoSide.Interaction.Chat", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHANNEL, "AllowTwoSide.Interaction.Channel", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP,   "AllowTwoSide.Interaction.Group", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD,   "AllowTwoSide.Interaction.Guild", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_AUCTION, "AllowTwoSide.Interaction.Auction", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_MAIL,    "AllowTwoSide.Interaction.Mail", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CALENDAR, "AllowTwoSide.Interaction.Calendar", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_WHO_LIST,            "AllowTwoSide.WhoList", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_ADD_FRIEND,          "AllowTwoSide.AddFriend", false);

    setConfig(CONFIG_UINT32_STRICT_PLAYER_NAMES,  "StrictPlayerNames",  0);
    setConfig(CONFIG_UINT32_STRICT_CHARTER_NAMES, "StrictCharterNames", 0);
    setConfig(CONFIG_UINT32_STRICT_PET_NAMES,     "StrictPetNames",     0);

    setConfigMinMax(CONFIG_UINT32_MIN_PLAYER_NAME,  "MinPlayerName",  2, 1, MAX_PLAYER_NAME);
    setConfigMinMax(CONFIG_UINT32_MIN_CHARTER_NAME, "MinCharterName", 2, 1, MAX_CHARTER_NAME);
    setConfigMinMax(CONFIG_UINT32_MIN_PET_NAME,     "MinPetName",     2, 1, MAX_PET_NAME);

    setConfig(CONFIG_UINT32_CHARACTERS_CREATING_DISABLED, "CharactersCreatingDisabled", 0);

    setConfigMinMax(CONFIG_UINT32_CHARACTERS_PER_REALM, "CharactersPerRealm", 10, 1, 10);

    // must be after CONFIG_UINT32_CHARACTERS_PER_REALM
    setConfigMin(CONFIG_UINT32_CHARACTERS_PER_ACCOUNT, "CharactersPerAccount", 50, getConfig(CONFIG_UINT32_CHARACTERS_PER_REALM));

    setConfigMinMax(CONFIG_UINT32_HEROIC_CHARACTERS_PER_REALM, "HeroicCharactersPerRealm", 1, 1, 10);

    setConfig(CONFIG_UINT32_MIN_LEVEL_FOR_HEROIC_CHARACTER_CREATING, "MinLevelForHeroicCharacterCreating", 55);

    setConfigMinMax(CONFIG_UINT32_SKIP_CINEMATICS, "SkipCinematics", 0, 0, 2);

    if (configNoReload(reload, CONFIG_UINT32_MAX_PLAYER_LEVEL, "MaxPlayerLevel", DEFAULT_MAX_LEVEL))
        setConfigMinMax(CONFIG_UINT32_MAX_PLAYER_LEVEL, "MaxPlayerLevel", DEFAULT_MAX_LEVEL, 1, MAX_LEVEL);

    setConfigMinMax(CONFIG_UINT32_START_PLAYER_LEVEL, "StartPlayerLevel", 1, 1, getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));
    setConfigMinMax(CONFIG_UINT32_START_HEROIC_PLAYER_LEVEL, "StartHeroicPlayerLevel", 55, 1, getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));

    setConfigMinMax(CONFIG_UINT32_START_PLAYER_MONEY, "StartPlayerMoney", 0, 0, MAX_MONEY_AMOUNT);

    setConfig(CONFIG_UINT32_MAX_HONOR_POINTS, "MaxHonorPoints", 75000);

    setConfigMinMax(CONFIG_UINT32_START_HONOR_POINTS, "StartHonorPoints", 0, 0, getConfig(CONFIG_UINT32_MAX_HONOR_POINTS));

    setConfig(CONFIG_UINT32_MAX_ARENA_POINTS, "MaxArenaPoints", 5000);

    setConfigMinMax(CONFIG_UINT32_START_ARENA_POINTS, "StartArenaPoints", 0, 0, getConfig(CONFIG_UINT32_MAX_ARENA_POINTS));

    setConfig(CONFIG_BOOL_ALWAYS_SHOW_QUEST_GREETING, "AlwaysShowQuestGreeting", false);

    setConfig(CONFIG_BOOL_TAXI_FLIGHT_CHAT_FIX, "TaxiFlightChatFix", false);
    setConfig(CONFIG_BOOL_LONG_TAXI_PATHS_PERSISTENCE, "LongFlightPathsPersistence", false);
    setConfig(CONFIG_BOOL_ALL_TAXI_PATHS, "AllFlightPaths", false);

    setConfig(CONFIG_BOOL_INSTANCE_IGNORE_LEVEL, "Instance.IgnoreLevel", false);
    setConfig(CONFIG_BOOL_INSTANCE_IGNORE_RAID,  "Instance.IgnoreRaid", false);
    setConfig(CONFIG_BOOL_INSTANCE_STRICT_COMBAT_LOCKDOWN,  "Instance.StrictCombatLockdown", true);

    setConfig(CONFIG_BOOL_CAST_UNSTUCK, "CastUnstuck", true);
    setConfig(CONFIG_UINT32_MAX_SPELL_CASTS_IN_CHAIN, "MaxSpellCastsInChain", 20);
    setConfig(CONFIG_UINT32_BIRTHDAY_TIME, "BirthdayTime", 1125180000);
    setConfig(CONFIG_UINT32_RABBIT_DAY, "RabbitDay", 0);

    setConfig(CONFIG_UINT32_INSTANCE_RESET_TIME_HOUR, "Instance.ResetTimeHour", 4);
    setConfig(CONFIG_UINT32_INSTANCE_UNLOAD_DELAY,    "Instance.UnloadDelay", 30 * MINUTE * IN_MILLISECONDS);
    setConfig(CONFIG_BOOL_DISABLE_INSTANCE_RELOCATE, "Instance.DisableRelocate", false);

    setConfigMinMax(CONFIG_UINT32_MAX_PRIMARY_TRADE_SKILL, "MaxPrimaryTradeSkill", 2, 0, 10);

    setConfigMinMax(CONFIG_UINT32_TRADE_SKILL_GMIGNORE_MAX_PRIMARY_COUNT, "TradeSkill.GMIgnore.MaxPrimarySkillsCount", SEC_CONSOLE, SEC_PLAYER, SEC_CONSOLE);
    setConfigMinMax(CONFIG_UINT32_TRADE_SKILL_GMIGNORE_LEVEL, "TradeSkill.GMIgnore.Level", SEC_CONSOLE, SEC_PLAYER, SEC_CONSOLE);
    setConfigMinMax(CONFIG_UINT32_TRADE_SKILL_GMIGNORE_SKILL, "TradeSkill.GMIgnore.Skill", SEC_CONSOLE, SEC_PLAYER, SEC_CONSOLE);

    setConfigMinMax(CONFIG_UINT32_MIN_PETITION_SIGNS, "MinPetitionSigns", 9, 0, 9);

    setConfig(CONFIG_UINT32_GM_LOGIN_STATE,                 "GM.LoginState",                2);
    setConfig(CONFIG_UINT32_GM_VISIBLE_STATE,               "GM.Visible",                   2);
    setConfig(CONFIG_UINT32_GM_ACCEPT_TICKETS,              "GM.AcceptTickets",             2);
    setConfig(CONFIG_UINT32_GM_LEVEL_ACCEPT_TICKETS,        "GM.AcceptTicketsLevel",        2);
    setConfig(CONFIG_UINT32_GM_CHAT,                        "GM.Chat",                      2);
    setConfig(CONFIG_UINT32_GM_LEVEL_CHAT,                  "GM.ChatLevel",                 1);
    setConfig(CONFIG_UINT32_GM_LEVEL_CHANNEL_MODERATION,    "GM.ChannelModerationLevel",    1);
    setConfig(CONFIG_UINT32_GM_LEVEL_CHANNEL_SILENT_JOIN,   "GM.ChannelSilentJoinLevel",    0);
    setConfig(CONFIG_UINT32_GM_WISPERING_TO,                "GM.WhisperingTo",              2);

    setConfig(CONFIG_UINT32_GM_LEVEL_IN_GM_LIST,  "GM.InGMList.Level",  SEC_ADMINISTRATOR);
    setConfig(CONFIG_UINT32_GM_LEVEL_IN_WHO_LIST, "GM.InWhoList.Level", SEC_ADMINISTRATOR);
    setConfig(CONFIG_BOOL_GM_LOG_TRADE,           "GM.LogTrade", false);

    setConfigMinMax(CONFIG_UINT32_START_GM_LEVEL, "GM.StartLevel", 1, getConfig(CONFIG_UINT32_START_PLAYER_LEVEL), MAX_LEVEL);
    setConfig(CONFIG_BOOL_GM_LOWER_SECURITY, "GM.LowerSecurity", false);
    setConfig(CONFIG_BOOL_GM_ALLOW_ACHIEVEMENT_GAINS, "GM.AllowAchievementGain", true);
    setConfig(CONFIG_UINT32_GM_INVISIBLE_AURA, "GM.InvisibleAura", 37800);
    setConfig(CONFIG_BOOL_GM_TICKETS_QUEUE_STATUS, "GM.TicketsQueueStatus", true);

    setConfig(CONFIG_UINT32_FOGOFWAR_STEALTH, "Visibility.FogOfWar.Stealth", 0);
    setConfig(CONFIG_UINT32_FOGOFWAR_HEALTH, "Visibility.FogOfWar.Health", 2);
    setConfig(CONFIG_UINT32_FOGOFWAR_STATS, "Visibility.FogOfWar.Stats", 0);

    setConfig(CONFIG_UINT32_MAIL_DELIVERY_DELAY, "MailDeliveryDelay", HOUR);

    setConfigMin(CONFIG_UINT32_MASS_MAILER_SEND_PER_TICK, "MassMailer.SendPerTick", 10, 1);

    setConfig(CONFIG_UINT32_UPTIME_UPDATE, "UpdateUptimeInterval", 10);
    if (reload)
    {
        m_timers[WUPDATE_UPTIME].SetInterval(getConfig(CONFIG_UINT32_UPTIME_UPDATE)*MINUTE * IN_MILLISECONDS);
        m_timers[WUPDATE_UPTIME].Reset();
    }

    setConfig(CONFIG_UINT32_NUM_MAP_THREADS, "MapUpdate.Threads", 3);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_ORANGE, "SkillChance.Orange", 100);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_YELLOW, "SkillChance.Yellow", 75);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_GREEN,  "SkillChance.Green",  25);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_GREY,   "SkillChance.Grey",   0);

    setConfig(CONFIG_UINT32_SKILL_CHANCE_MINING_STEPS,   "SkillChance.MiningSteps",   75);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_SKINNING_STEPS, "SkillChance.SkinningSteps", 75);

    setConfig(CONFIG_BOOL_SKILL_PROSPECTING, "SkillChance.Prospecting", false);
    setConfig(CONFIG_BOOL_SKILL_MILLING,     "SkillChance.Milling",     false);

    setConfig(CONFIG_UINT32_SKILL_GAIN_CRAFTING,  "SkillGain.Crafting",  1);
    setConfig(CONFIG_UINT32_SKILL_GAIN_DEFENSE,   "SkillGain.Defense",   1);
    setConfig(CONFIG_UINT32_SKILL_GAIN_GATHERING, "SkillGain.Gathering", 1);
    setConfig(CONFIG_UINT32_SKILL_GAIN_WEAPON,       "SkillGain.Weapon",    1);

    setConfig(CONFIG_BOOL_SKILL_FAIL_LOOT_FISHING,         "SkillFail.Loot.Fishing", true);
    setConfig(CONFIG_BOOL_SKILL_FAIL_GAIN_FISHING,         "SkillFail.Gain.Fishing", true);
    setConfig(CONFIG_BOOL_SKILL_FAIL_POSSIBLE_FISHINGPOOL, "SkillFail.Possible.FishingPool", false);

    setConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS, "MaxOverspeedPings", 2);
    if (getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS) != 0 && getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS) < 2)
    {
        sLog.outError("MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check). Set to 2.", getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS));
        setConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS, 2);
    }

    setConfig(CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY, "SaveRespawnTimeImmediately", true);
    setConfig(CONFIG_BOOL_WEATHER, "ActivateWeather", true);

    setConfig(CONFIG_BOOL_ALWAYS_MAX_SKILL_FOR_LEVEL, "AlwaysMaxSkillForLevel", false);

    if (configNoReload(reload, CONFIG_UINT32_EXPANSION, "Expansion", MAX_EXPANSION))
        setConfigMinMax(CONFIG_UINT32_EXPANSION, "Expansion", MAX_EXPANSION, 0, MAX_EXPANSION);

    setConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_COUNT, "ChatFlood.MessageCount", 10);
    setConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_DELAY, "ChatFlood.MessageDelay", 1);
    setConfig(CONFIG_UINT32_CHATFLOOD_MUTE_TIME,     "ChatFlood.MuteTime", 10);

    setConfig(CONFIG_BOOL_EVENT_ANNOUNCE, "Event.Announce", false);

    setConfig(CONFIG_UINT32_CREATURE_FAMILY_ASSISTANCE_DELAY, "CreatureFamilyAssistanceDelay", 1500);
    setConfig(CONFIG_UINT32_CREATURE_FAMILY_FLEE_DELAY,       "CreatureFamilyFleeDelay",       10000);

    setConfig(CONFIG_UINT32_WORLD_BOSS_LEVEL_DIFF, "WorldBossLevelDiff", 3);

    setConfigMinMax(CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF, "Quests.LowLevelHideDiff", 5, -1, MAX_LEVEL);
    setConfigMinMax(CONFIG_INT32_QUEST_HIGH_LEVEL_HIDE_DIFF, "Quests.HighLevelHideDiff", 7, -1, MAX_LEVEL);

    setConfigMinMax(CONFIG_UINT32_QUEST_DAILY_RESET_HOUR, "Quests.Daily.ResetHour", 6, 0, 23);
    setConfigMinMax(CONFIG_UINT32_QUEST_WEEKLY_RESET_WEEK_DAY, "Quests.Weekly.ResetWeekDay", 3, 0, 6);
    setConfigMinMax(CONFIG_UINT32_QUEST_WEEKLY_RESET_HOUR, "Quests.Weekly.ResetHour", 6, 0, 23);

    setConfig(CONFIG_BOOL_QUEST_IGNORE_RAID, "Quests.IgnoreRaid", false);

    setConfig(CONFIG_BOOL_DETECT_POS_COLLISION, "DetectPosCollision", true);

    setConfig(CONFIG_BOOL_CHAT_RESTRICTED_RAID_WARNINGS,        "Chat.RestrictedRaidWarnings", 1);
    setConfig(CONFIG_UINT32_CHANNEL_RESTRICTED_LANGUAGE_MODE,   "Channel.RestrictedLanguageMode", 0);
    setConfig(CONFIG_BOOL_CHANNEL_RESTRICTED_LFG,               "Channel.RestrictedLfg", true);
    setConfig(CONFIG_UINT32_CHANNEL_STATIC_AUTO_TRESHOLD,       "Channel.StaticAutoTreshold", 0);

    setConfig(CONFIG_BOOL_TALENTS_INSPECTING,           "TalentsInspecting", true);
    setConfig(CONFIG_BOOL_CHAT_FAKE_MESSAGE_PREVENTING, "ChatFakeMessagePreventing", false);

    setConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY, "ChatStrictLinkChecking.Severity", 0);
    setConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_KICK,     "ChatStrictLinkChecking.Kick", 0);

    setConfig(CONFIG_BOOL_CORPSE_EMPTY_LOOT_SHOW,                     "Corpse.EmptyLootShow",                  true);
    setConfig(CONFIG_BOOL_CORPSE_ALLOW_ALL_ITEMS_SHOW_IN_MASTER_LOOT, "Corpse.AllowAllItemsShowInMasterLoot", false);
    setConfig(CONFIG_UINT32_CORPSE_DECAY_NORMAL,                      "Corpse.Decay.NORMAL",                    300);
    setConfig(CONFIG_UINT32_CORPSE_DECAY_RARE,                        "Corpse.Decay.RARE",                      900);
    setConfig(CONFIG_UINT32_CORPSE_DECAY_ELITE,                       "Corpse.Decay.ELITE",                     600);
    setConfig(CONFIG_UINT32_CORPSE_DECAY_RAREELITE,                   "Corpse.Decay.RAREELITE",                1200);
    setConfig(CONFIG_UINT32_CORPSE_DECAY_WORLDBOSS,                   "Corpse.Decay.WORLDBOSS",                3600);

    setConfig(CONFIG_INT32_DEATH_SICKNESS_LEVEL, "Death.SicknessLevel", 11);

    setConfig(CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVP, "Death.CorpseReclaimDelay.PvP", true);
    setConfig(CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVE, "Death.CorpseReclaimDelay.PvE", true);
    setConfig(CONFIG_BOOL_DEATH_BONES_WORLD,              "Death.Bones.World", true);
    setConfig(CONFIG_BOOL_DEATH_BONES_BG_OR_ARENA,        "Death.Bones.BattlegroundOrArena", true);
    setConfigMinMax(CONFIG_FLOAT_GHOST_RUN_SPEED_WORLD,   "Death.Ghost.RunSpeed.World", 1.0f, 0.1f, 10.0f);
    setConfigMinMax(CONFIG_FLOAT_GHOST_RUN_SPEED_BG,      "Death.Ghost.RunSpeed.Battleground", 1.0f, 0.1f, 10.0f);

    setConfig(CONFIG_FLOAT_LEASH_RADIUS, "LeashRadius", 30.f);
    setConfigMin(CONFIG_UINT32_CREATURE_RESPAWN_AGGRO_DELAY, "CreatureRespawnAggroDelay", 5000, 0);
    setConfig(CONFIG_UINT32_CREATURE_PICKPOCKET_RESTOCK_DELAY, "CreaturePickpocketRestockDelay", 600);

    // always use declined names in the russian client
    if (getConfig(CONFIG_UINT32_REALM_ZONE) == REALM_ZONE_RUSSIAN)
        setConfig(CONFIG_BOOL_DECLINED_NAMES_USED, true);
    else
        setConfig(CONFIG_BOOL_DECLINED_NAMES_USED, "DeclinedNames", false);

    setConfig(CONFIG_BOOL_BATTLEGROUND_CAST_DESERTER,                  "Battleground.CastDeserter", true);
    setConfigMinMax(CONFIG_UINT32_BATTLEGROUND_QUEUE_ANNOUNCER_JOIN,   "Battleground.QueueAnnouncer.Join", 0, 0, 2);
    setConfig(CONFIG_BOOL_BATTLEGROUND_QUEUE_ANNOUNCER_START,          "Battleground.QueueAnnouncer.Start", false);
    setConfig(CONFIG_BOOL_BATTLEGROUND_SCORE_STATISTICS,               "Battleground.ScoreStatistics", false);
    setConfig(CONFIG_UINT32_BATTLEGROUND_INVITATION_TYPE,              "Battleground.InvitationType", 0);
    setConfig(CONFIG_UINT32_BATTLEGROUND_PREMATURE_FINISH_TIMER,       "BattleGround.PrematureFinishTimer", 5 * MINUTE * IN_MILLISECONDS);
    setConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH, "BattleGround.PremadeGroupWaitForMatch", 30 * MINUTE * IN_MILLISECONDS);
    setConfigMinMax(CONFIG_UINT32_BATTLEGROUND_RANDOM_RESET_HOUR,      "BattleGround.Random.ResetHour", 6, 0, 23);
    setConfig(CONFIG_UINT32_ARENA_MAX_RATING_DIFFERENCE,               "Arena.MaxRatingDifference", 150);
    setConfig(CONFIG_UINT32_ARENA_RATING_DISCARD_TIMER,                "Arena.RatingDiscardTimer", 10 * MINUTE * IN_MILLISECONDS);
    setConfig(CONFIG_BOOL_ARENA_AUTO_DISTRIBUTE_POINTS,                "Arena.AutoDistributePoints", false);
    setConfig(CONFIG_UINT32_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS,       "Arena.AutoDistributeInterval", 7);
    setConfig(CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_JOIN,                  "Arena.QueueAnnouncer.Join", false);
    setConfig(CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_EXIT,                  "Arena.QueueAnnouncer.Exit", false);
    setConfig(CONFIG_UINT32_ARENA_SEASON_ID,                           "Arena.ArenaSeason.ID", 1);
    setConfig(CONFIG_UINT32_ARENA_FIRST_RESET_DAY,                     "Arena.FirstResetDay", 3);
    setConfig(CONFIG_UINT32_ARENA_SEASON_PREVIOUS_ID,                  "Arena.ArenaSeasonPrevious.ID", 0);
    setConfigMin(CONFIG_INT32_ARENA_STARTRATING,                       "Arena.StartRating", -1, -1);
    setConfigMin(CONFIG_INT32_ARENA_STARTPERSONALRATING,               "Arena.StartPersonalRating", -1, -1);
    setConfig(CONFIG_BOOL_OUTDOORPVP_SI_ENABLED,                       "OutdoorPvp.SIEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_EP_ENABLED,                       "OutdoorPvp.EPEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_HP_ENABLED,                       "OutdoorPvp.HPEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_ZM_ENABLED,                       "OutdoorPvp.ZMEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_TF_ENABLED,                       "OutdoorPvp.TFEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_NA_ENABLED,                       "OutdoorPvp.NAEnabled", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_GH_ENABLED,                       "OutdoorPvp.GHEnabled", true);
    setConfig(CONFIG_BOOL_BATTLEFIELD_WG_ENABLED,                      "Battlefield.WGEnabled", true);
    setConfig(CONFIG_UINT32_BATTLEFIELD_COOLDOWN_DURATION,             "Battlefield.CooldownDuraton", 150);
    setConfig(CONFIG_UINT32_BATTLEFIELD_BATTLE_DURATION,               "Battlefield.BattleDuration", 30);
    setConfig(CONFIG_UINT32_BATTLEFIELD_MAX_PLAYERS_PER_TEAM,          "Battlefield.MaxPlayersPerTeam", 120);

    setConfig(CONFIG_BOOL_OFFHAND_CHECK_AT_TALENTS_RESET, "OffhandCheckAtTalentsReset", false);

    setConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET, "Network.KickOnBadPacket", false);

    setConfig(CONFIG_BOOL_PLAYER_COMMANDS, "PlayerCommands", true);

    if (int clientCacheId = sConfig.GetIntDefault("ClientCacheVersion", 0))
    {
        // overwrite DB/old value
        if (clientCacheId > 0)
        {
            setConfig(CONFIG_UINT32_CLIENTCACHE_VERSION, clientCacheId);
            sLog.outString("Client cache version set to: %u", clientCacheId);
        }
        else
            sLog.outError("ClientCacheVersion can't be negative %d, ignored.", clientCacheId);
    }

    setConfig(CONFIG_UINT32_INSTANT_LOGOUT, "InstantLogout", SEC_MODERATOR);

    setConfigMin(CONFIG_UINT32_GROUP_OFFLINE_LEADER_DELAY, "Group.OfflineLeaderDelay", 300, 0);

    setConfigMin(CONFIG_UINT32_GUILD_EVENT_LOG_COUNT, "Guild.EventLogRecordsCount", GUILD_EVENTLOG_MAX_RECORDS, GUILD_EVENTLOG_MAX_RECORDS);
    setConfigMin(CONFIG_UINT32_GUILD_BANK_EVENT_LOG_COUNT, "Guild.BankEventLogRecordsCount", GUILD_BANK_MAX_LOGS, GUILD_BANK_MAX_LOGS);

    setConfig(CONFIG_UINT32_MIRRORTIMER_FATIGUE_MAX,       "MirrorTimer.Fatigue.Max", 60);
    setConfig(CONFIG_UINT32_MIRRORTIMER_BREATH_MAX,        "MirrorTimer.Breath.Max", 180);
    setConfig(CONFIG_UINT32_MIRRORTIMER_ENVIRONMENTAL_MAX, "MirrorTimer.Environmental.Max", 1);
    setConfig(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MIN, "EnvironmentalDamage.Min", 605);
    setConfigMin(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MAX, "EnvironmentalDamage.Max", 610, getConfig(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MIN));

    setConfig(CONFIG_UINT32_INTERACTION_PAUSE_TIMER, "InteractionPauseTimer", 180000);

    setConfig(CONFIG_BOOL_PET_UNSUMMON_AT_MOUNT,      "PetUnsummonAtMount", false);
    setConfig(CONFIG_BOOL_KEEP_PET_ON_FLYING_MOUNT,   "KeepPetOnFlyingMount", false);
    setConfig(CONFIG_BOOL_PET_ATTACK_FROM_BEHIND,     "PetAttackFromBehind", true);

    setConfig(CONFIG_BOOL_AUTO_DOWNRANK,              "AutoDownrank", true);

    m_relocation_ai_notify_delay = sConfig.GetIntDefault("Visibility.AIRelocationNotifyDelay", 1000u);
    m_relocation_lower_limit_sq = pow(sConfig.GetFloatDefault("Visibility.RelocationLowerLimit", 10), 2);

    // Visibility on Continents
    m_MaxVisibleDistanceOnContinents      = sConfig.GetFloatDefault("Visibility.Distance.Continents",     DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceOnContinents < 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.Continents can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceOnContinents = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceOnContinents >  MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Continents can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceOnContinents = MAX_VISIBILITY_DISTANCE;
    }

    // Visibility in Instances
    m_MaxVisibleDistanceInInstances        = sConfig.GetFloatDefault("Visibility.Distance.Instances",       DEFAULT_VISIBILITY_INSTANCE);
    if (m_MaxVisibleDistanceInInstances < 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.Instances can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInInstances = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInInstances >  MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Instances can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInInstances = MAX_VISIBILITY_DISTANCE;
    }

    // Visibility in BG/Arenas
    m_MaxVisibleDistanceInBGArenas        = sConfig.GetFloatDefault("Visibility.Distance.BGArenas",       DEFAULT_VISIBILITY_BGARENAS);
    if (m_MaxVisibleDistanceInBGArenas < 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.BGArenas can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInBGArenas = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInBGArenas >  MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.BGArenas can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInBGArenas = MAX_VISIBILITY_DISTANCE;
    }

    ///- Load the CharDelete related config options
    setConfigMinMax(CONFIG_UINT32_CHARDELETE_METHOD, "CharDelete.Method", 0, 0, 1);
    setConfigMinMax(CONFIG_UINT32_CHARDELETE_MIN_LEVEL, "CharDelete.MinLevel", 0, 0, getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));
    setConfig(CONFIG_UINT32_CHARDELETE_KEEP_DAYS, "CharDelete.KeepDays", 30);

    if (configNoReload(reload, CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE, "GuidReserveSize.Creature", 100))
        setConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE,   "GuidReserveSize.Creature",   100);
    if (configNoReload(reload, CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT, "GuidReserveSize.GameObject", 100))
        setConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT, "GuidReserveSize.GameObject", 100);

    setConfig(CONFIG_UINT32_MIN_LEVEL_FOR_RAID, "Raid.MinLevel", 10);

    ///- Read the "Data" directory from the config file
    std::string dataPath = sConfig.GetStringDefault("DataDir", "./");

    // for empty string use current dir as for absent case
    if (dataPath.empty())
        dataPath = "./";
    // normalize dir path to path/ or path\ form
    else if (dataPath.at(dataPath.length() - 1) != '/' && dataPath.at(dataPath.length() - 1) != '\\')
        dataPath.append("/");

    if (reload)
    {
        if (dataPath != m_dataPath)
            sLog.outError("DataDir option can't be changed at mangosd.conf reload, using current value (%s).", m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
        sLog.outString("Using DataDir %s", m_dataPath.c_str());
    }

    setConfig(CONFIG_BOOL_VMAP_INDOOR_CHECK, "vmap.enableIndoorCheck", true);
    bool enableLOS = sConfig.GetBoolDefault("vmap.enableLOS", false);
    bool enableHeight = sConfig.GetBoolDefault("vmap.enableHeight", false);

    if (!enableHeight)
        sLog.outError("VMAP height use disabled! Creatures movements and other things will be in broken state.");

    VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(enableLOS);
    VMAP::VMapFactory::createOrGetVMapManager()->setEnableHeightCalc(enableHeight);
    sLog.outString("WORLD: VMap support included. LineOfSight:%i, getHeight:%i, indoorCheck:%i",
                   enableLOS, enableHeight, getConfig(CONFIG_BOOL_VMAP_INDOOR_CHECK) ? 1 : 0);
    sLog.outString("WORLD: VMap data directory is: %svmaps", m_dataPath.c_str());

    setConfig(CONFIG_BOOL_MMAP_ENABLED, "mmap.enabled", true);
    std::string ignoreMapIds = sConfig.GetStringDefault("mmap.ignoreMapIds");
    setConfig(CONFIG_BOOL_PRELOAD_MMAP_TILES, "mmap.preload", false);
    MMAP::MMapFactory::preventPathfindingOnMaps(ignoreMapIds.c_str());
    bool enabledPathfinding = getConfig(CONFIG_BOOL_MMAP_ENABLED);
    sLog.outString("WORLD: MMap pathfinding %sabled", enabledPathfinding ? "en" : "dis");
    MMAP::MMapFactory::createOrGetMMapManager()->SetEnabled(enabledPathfinding);

    setConfig(CONFIG_BOOL_PATH_FIND_OPTIMIZE, "PathFinder.OptimizePath", true);
    setConfig(CONFIG_BOOL_PATH_FIND_NORMALIZE_Z, "PathFinder.NormalizeZ", false);

    setConfig(CONFIG_UINT32_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL, "Raf.BonusLevel", 60);
    setConfig(CONFIG_UINT32_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL_DIFFERENCE, "Raf.LevelDifference", 4);
    setConfig(CONFIG_FLOAT_MAX_RECRUIT_A_FRIEND_DISTANCE, "Raf.Distance", 100.f);

    setConfig(CONFIG_UINT32_SUNSREACH_COUNTER, "Sunsreach.CounterMax", 10000);

    setConfig(CONFIG_BOOL_REGEN_ZONE_AREA_ON_STARTUP, "Spawns.ZoneArea", false);

    sLog.outString();
}

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Initialize the random number generator
    srand((unsigned int)time(nullptr));

    ///- Time server startup
    uint32 uStartTime = WorldTimer::getMSTime();

    ///- Initialize detour memory management
    dtAllocSetCustom(dtCustomAlloc, dtCustomFree);

    ///- Initialize config settings
    LoadConfigSettings();

    ///- Check the existence of the map files for all races start areas.
    if (!MapManager::ExistMapAndVMap(0, -6240.32f, 331.033f) ||                     // Dwarf/ Gnome
            !MapManager::ExistMapAndVMap(0, -8949.95f, -132.493f) ||                // Human
            !MapManager::ExistMapAndVMap(1, -618.518f, -4251.67f) ||                // Orc
            !MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f) ||                  // Scourge
            !MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f) ||                  // NightElf
            !MapManager::ExistMapAndVMap(1, -2917.58f, -257.98f) ||                 // Tauren
            (m_configUint32Values[CONFIG_UINT32_EXPANSION] >= EXPANSION_TBC &&
             (!MapManager::ExistMapAndVMap(530, 10349.6f, -6357.29f) ||             // BloodElf
              !MapManager::ExistMapAndVMap(530, -3961.64f, -13931.2f))) ||          // Draenei
            (m_configUint32Values[CONFIG_UINT32_EXPANSION] >= EXPANSION_WOTLK &&
             !MapManager::ExistMapAndVMap(609, 2355.84f, -5664.77f)))               // Death Knight
    {
        sLog.outError("Correct *.map files not found in path '%smaps' or *.vmtree/*.vmtile files in '%svmaps'. Please place *.map and vmap files in appropriate directories or correct the DataDir value in the mangosd.conf file.", m_dataPath.c_str(), m_dataPath.c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    ///- Loading strings. Getting no records means core load has to be canceled because no error message can be output.
    sLog.outString("Loading MaNGOS strings...");
    if (!sObjectMgr.LoadMangosStrings())
    {
        Log::WaitBeforeContinueIfNeed();
        exit(1);                                            // Error message displayed in function already
    }

    ///- Update the realm entry in the database with the realm type from the config file
    // No SQL injection as values are treated as integers

    // not send custom type REALM_FFA_PVP to realm list
    uint32 server_type = IsFFAPvPRealm() ? uint32(REALM_TYPE_PVP) : getConfig(CONFIG_UINT32_GAME_TYPE);
    uint32 realm_zone = getConfig(CONFIG_UINT32_REALM_ZONE);
    LoginDatabase.PExecute("UPDATE realmlist SET icon = %u, timezone = %u WHERE id = '%u'", server_type, realm_zone, realmID);

    ///- Remove the bones (they should not exist in DB though) and old corpses after a restart
    CharacterDatabase.PExecute("DELETE FROM corpse WHERE corpse_type = '0' OR time < (" _UNIXTIME_ "-'%u')", 3 * DAY);

    // load SQL dbcs first, other DBCs need them
    sObjectMgr.LoadSQLDBCs();

    m_bgQueue.SetNextRatingDiscardUpdate(std::chrono::milliseconds(sWorld.getConfig(CONFIG_UINT32_ARENA_RATING_DISCARD_TIMER)));

    sLog.outString("Loading spell groups...");
    sSpellStacker.LoadSpellGroups();

    // Load before npc_text, gossip_menu_option, script_texts
    sLog.outString("Loading broadcast_text...");
    sObjectMgr.LoadBroadcastText();

    sLog.outString("Loading world safe locs ...");
    LoadWorldSafeLocs();

    ///- Load the DBC files
    sLog.outString("Initialize DBC data stores...");
    LoadDBCStores(m_dataPath);
    DetectDBCLang();
    sObjectMgr.SetDbc2StorageLocaleIndex(GetDefaultDbcLocale());    // Get once for all the locale index of DBC language (console/broadcasts)

    if (VMAP::IVMapManager* vmmgr2 = VMAP::VMapFactory::createOrGetVMapManager()) // after map store init
    {
        std::vector<uint32> mapIds;
        for (uint32 mapId = 0; mapId < sMapStore.GetNumRows(); mapId++)
            if (sMapStore.LookupEntry(mapId))
                mapIds.push_back(mapId);

        vmmgr2->InitializeThreadUnsafe(mapIds);
    }

    // Loading cameras for characters creation cinematic
    sLog.outString("Loading cinematic...");
    LoadM2Cameras(m_dataPath);

    sLog.outString("Loading Attachment Data...");
    LoadM2Attachments(m_dataPath);

    sLog.outString("Loading Script Names...");
    sScriptDevAIMgr.LoadScriptNames();

    sLog.outString("Loading WorldTemplate...");
    sObjectMgr.LoadWorldTemplate();

    sLog.outString("Loading InstanceTemplate...");
    sObjectMgr.LoadInstanceTemplate();

    sLog.outString("Loading SkillLineAbilityMultiMaps Data...");
    sSpellMgr.LoadSkillLineAbilityMaps();

    sLog.outString("Loading SkillRaceClassInfoMultiMap Data...");
    sSpellMgr.LoadSkillRaceClassInfoMap();

    ///- Clean up and pack instances
    sLog.outString("Packing instances...");
    sMapPersistentStateMgr.PackInstances();

    sLog.outString("Cleaning up instances...");
    sMapPersistentStateMgr.CleanupInstances();              // must be called before `creature_respawn`/`gameobject_respawn` tables and after pack instances

    sLog.outString("Packing groups...");
    sObjectMgr.PackGroupIds();                              // must be after CleanupInstances

    ///- Init highest guids before any guid using table loading to prevent using not initialized guids in some code.
    sObjectMgr.SetHighestGuids();                           // must be after PackInstances() and PackGroupIds()
    sLog.outString();

    sLog.outString("Loading Page Texts...");
    sObjectMgr.LoadPageTexts();

    sLog.outString("Loading String Ids...");
    sScriptMgr.LoadStringIds(); // must be before LoadCreatureSpawnDataTemplates

    sLog.outString("Loading Game Object Templates...");     // must be after LoadPageTexts
    std::vector<uint32> transportDisplayIds = sObjectMgr.LoadGameobjectInfo();
    MMAP::MMapFactory::createOrGetMMapManager()->loadAllGameObjectModels(GetDataPath(), transportDisplayIds);

    sLog.outString("Loading GameObject models...");
    GameObjectModel::LoadGOVmapModels();
    sLog.outString();

    // loads GO data
    sTransportMgr.LoadTransportAnimationAndRotation();

    sLog.outString("Loading Spell Chain Data...");
    sSpellMgr.LoadSpellChains();

    sLog.outString("Checking Spell Cone Data...");
    sObjectMgr.CheckSpellCones();

    sLog.outString("Loading Spell Elixir types...");
    sSpellMgr.LoadSpellElixirs();

    sLog.outString("Loading Spell Learn Skills...");
    sSpellMgr.LoadSpellLearnSkills();                       // must be after LoadSpellChains

    sLog.outString("Loading Spell Learn Spells...");
    sSpellMgr.LoadSpellLearnSpells();

    sLog.outString("Loading Spell Proc Event conditions...");
    sSpellMgr.LoadSpellProcEvents();

    sLog.outString("Loading Spell Proc Item Enchant...");
    sSpellMgr.LoadSpellProcItemEnchant();                   // must be after LoadSpellChains

    sLog.outString("Loading Aggro Spells Definitions...");
    sSpellMgr.LoadSpellThreats();

    sLog.outString("Loading NPC Texts...");
    sObjectMgr.LoadGossipText();

    sLog.outString("Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();

    sLog.outString("Loading Item Templates...");            // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr.LoadItemPrototypes();

    sLog.outString("Loading Item converts...");             // must be after LoadItemPrototypes
    sObjectMgr.LoadItemConverts();

    sLog.outString("Loading Item expire converts...");      // must be after LoadItemPrototypes
    sObjectMgr.LoadItemExpireConverts();

    sLog.outString("Loading Creature Model Based Info Data...");
    sObjectMgr.LoadCreatureModelInfo();

    sLog.outString("Loading Equipment templates...");
    sObjectMgr.LoadEquipmentTemplates();

    sLog.outString("Loading Creature Stats...");
    sObjectMgr.LoadCreatureClassLvlStats();

    sLog.outString("Loading Creature templates...");
    sObjectMgr.LoadCreatureTemplates();

    sLog.outString("Loading Creature immunities...");
    sObjectMgr.LoadCreatureImmunities();

    sLog.outString("Loading Combat Conditions, Unit Conditions and Worldstate Expressions...");
    sObjectMgr.LoadConditionsAndExpressions();

    sLog.outString("Loading Creature spell lists...");
    auto spellLists = sObjectMgr.LoadCreatureSpellLists();

    sLog.outString("Loading Creature cooldowns...");
    sObjectMgr.LoadCreatureCooldowns();

    sLog.outString("Loading Creature template spells...");
    sObjectMgr.LoadCreatureTemplateSpells(spellLists);

    sLog.outString("Loading Creature Model for race...");   // must be after creature templates
    sObjectMgr.LoadCreatureModelRace();

    sLog.outString("Loading Vehicle Accessory...");         // must be after LoadCreatureTemplates
    sObjectMgr.LoadVehicleAccessory();

    sLog.outString("Loading Vehicle Seat Parameters...");         // must be after dbc load
    sObjectMgr.LoadVehicleSeatParameters();

    sLog.outString("Loading ItemRequiredTarget...");
    sObjectMgr.LoadItemRequiredTarget();

    sLog.outString("Loading Reputation Reward Rates...");
    sObjectMgr.LoadReputationRewardRate();

    sLog.outString("Loading Creature Reputation OnKill Data...");
    sObjectMgr.LoadReputationOnKill();

    sLog.outString("Loading Reputation Spillover Data...");
    sObjectMgr.LoadReputationSpilloverTemplate();

    sLog.outString("Loading Points Of Interest Data...");
    sObjectMgr.LoadPointsOfInterest();

    sLog.outString("Loading Creature Conditional Spawn Data...");  // must be after LoadCreatureTemplates and before LoadCreatures
    sObjectMgr.LoadCreatureConditionalSpawn();

    sLog.outString("Loading Creature Spawn Template Data..."); // must be before LoadCreatures
    sObjectMgr.LoadCreatureSpawnDataTemplates();

    sLog.outString("Loading Creature Spawn Entry Data..."); // must be before LoadCreatures
    sObjectMgr.LoadCreatureSpawnEntry();

    sLog.outString("Loading Creature Data...");
    sObjectMgr.LoadCreatures();

    sLog.outString("Loading Gameobject Spawn Entry Data..."); // must be before LoadGameObjects
    sObjectMgr.LoadGameObjectSpawnEntry();

    sLog.outString("Loading Gameobject Data...");
    sObjectMgr.LoadGameObjects();

    if (getConfig(CONFIG_BOOL_REGEN_ZONE_AREA_ON_STARTUP))
    {
        sLog.outString("Generating zone and area ids for creatures and gameobjects...");
        sObjectMgr.GenerateZoneAndAreaIds();
    }

    sLog.outString("Loading SpellsScriptTarget...");
    sSpellMgr.LoadSpellScriptTarget();                      // must be after LoadCreatureTemplates, LoadCreatures and LoadGameobjectInfo

    sLog.outString("Generating SpellTargetMgr data...\n");
    SpellTargetMgr::Initialize(); // must be after LoadSpellScriptTarget

    sLog.outString("Loading pet levelup spells...");
    sSpellMgr.LoadPetLevelupSpellMap();

    sLog.outString("Loading pet default spell additional to levelup spells...");
    sSpellMgr.LoadPetDefaultSpells();

    sLog.outString("Loading Creature Addon Data...");
    sObjectMgr.LoadCreatureAddons();                        // must be after LoadCreatureTemplates() and LoadCreatures()
    sLog.outString(">>> Creature Addon Data loaded");
    sLog.outString();

    sLog.outString("Loading Gameobject Template Addon Data...");
    sObjectMgr.LoadGameObjectTemplateAddons();

    sLog.outString("Loading CreatureLinking Data...");      // must be after Creatures
    sCreatureLinkingMgr.LoadFromDB();

    sLog.outString("Loading Objects Pooling Data...");
    sPoolMgr.LoadFromDB();

    sLog.outString("Loading Weather Data...");
    sWeatherMgr.LoadWeatherZoneChances();

    sLog.outString("Loading Quests...");
    sObjectMgr.LoadQuests();                                // must be loaded after DBCs, creature_template, item_template, gameobject tables

    sLog.outString("Loading Quest POI");
    sObjectMgr.LoadQuestPOI();

    sLog.outString("Loading Quests Relations...");
    sObjectMgr.LoadQuestRelations();                        // must be after quest load
    sLog.outString(">>> Quests Relations loaded");
    sLog.outString();

    sLog.outString("Loading Game Event Data...");           // must be after sPoolMgr.LoadFromDB and quests to properly load pool events and quests for events
    sGameEventMgr.LoadFromDB();
    sLog.outString(">>> Game Event Data loaded");
    sLog.outString();

    sLog.outString("Loading WorldState Names...");          // must be before conditions and dbscripts
    sObjectMgr.LoadWorldStateNames();

    sLog.outString("Loading Conditions...");                // Load Conditions
    sObjectMgr.LoadConditions();

    sLog.outString("Loading Spawn Groups");                 // must be after creature and GO load
    sObjectMgr.LoadSpawnGroups();

    // Not sure if this can be moved up in the sequence (with static data loading) as it uses MapManager
    sLog.outString("Loading Transports...");
    sMapMgr.LoadTransports();

    sLog.outString("Creating map persistent states for non-instanceable maps...");     // must be after PackInstances(), LoadCreatures(), sPoolMgr.LoadFromDB(), sGameEventMgr.LoadFromDB();
    sMapPersistentStateMgr.InitWorldMaps();
    sLog.outString();

    sLog.outString("Loading Creature Respawn Data...");     // must be after LoadCreatures(), and sMapPersistentStateMgr.InitWorldMaps()
    sMapPersistentStateMgr.LoadCreatureRespawnTimes();

    sLog.outString("Loading Gameobject Respawn Data...");   // must be after LoadGameObjects(), and sMapPersistentStateMgr.InitWorldMaps()
    sMapPersistentStateMgr.LoadGameobjectRespawnTimes();

    sLog.outString("Loading UNIT_NPC_FLAG_SPELLCLICK Data...");
    sObjectMgr.LoadNPCSpellClickSpells();

    sLog.outString("Loading SpellArea Data...");            // must be after quest load
    sSpellMgr.LoadSpellAreas();

    sLog.outString("Loading AreaTrigger definitions...");
    sObjectMgr.LoadAreaTriggerTeleports();                  // must be after item template load

    sLog.outString("Loading Quest Area Triggers...");
    sObjectMgr.LoadQuestAreaTriggers();                     // must be after LoadQuests

    sLog.outString("Loading Tavern Area Triggers...");
    sObjectMgr.LoadTavernAreaTriggers();

    sLog.outString("Loading AreaTrigger script names...");
    sScriptDevAIMgr.LoadAreaTriggerScripts();

    sLog.outString("Loading LFG dungeons...");
    sLFGMgr.LoadLFGDungeons();

    sLog.outString("Loading LFG rewards...");
    sLFGMgr.LoadRewards();

    sLog.outString("Loading event id script names...");
    sScriptDevAIMgr.LoadEventIdScripts();

    sLog.outString("Loading Graveyard-zone links...");
    LoadGraveyardZones();

    sLog.outString("Loading taxi flight shortcuts...");
    sObjectMgr.LoadTaxiShortcuts();

    sLog.outString("Loading spell target destination coordinates...");
    sSpellMgr.LoadSpellTargetPositions();

    sLog.outString("Loading spell pet auras...");
    sSpellMgr.LoadSpellPetAuras();

    sLog.outString("Loading Player Create Info & Level Stats...");
    sObjectMgr.LoadPlayerInfo();
    sLog.outString(">>> Player Create Info & Level Stats loaded");
    sLog.outString();

    sLog.outString("Loading Exploration BaseXP Data...");
    sObjectMgr.LoadExplorationBaseXP();

    sLog.outString("Loading Pet Name Parts...");
    sObjectMgr.LoadPetNames();

    CharacterDatabaseCleaner::CleanDatabase();
    sLog.outString();

    sLog.outString("Loading the max pet number...");
    sObjectMgr.LoadPetNumber();

    sLog.outString("Loading pet level stats...");
    sObjectMgr.LoadPetLevelInfo();

    sLog.outString("Loading Player Corpses...");
    sObjectMgr.LoadCorpses();

    sLog.outString("Loading Player level dependent mail rewards...");
    sObjectMgr.LoadMailLevelRewards();

    sLog.outString("Loading Loot Tables...");
    LootIdSet ids_set;
    LoadLootTables(ids_set);
    sLog.outString(">>> Loot Tables loaded");
    sLog.outString();

    sLog.outString("Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();

    sLog.outString("Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();

    sLog.outString("Loading Skill Fishing base level requirements...");
    sObjectMgr.LoadFishingBaseSkillLevel();

    sLog.outString("Loading Achievements...");
    sAchievementMgr.LoadAchievementReferenceList();
    sAchievementMgr.LoadAchievementCriteriaList();
    sAchievementMgr.LoadAchievementCriteriaRequirements();
    sAchievementMgr.LoadRewards();
    sAchievementMgr.LoadRewardLocales();
    sAchievementMgr.LoadCompletedAchievements();
    sLog.outString(">>> Achievements loaded");
    sLog.outString();

    sLog.outString("Loading access requirements...");
    sObjectMgr.LoadAccessRequirements();                    // must be after achievements

    sLog.outString("Loading Instance encounters data...");  // must be after Creature loading
    sObjectMgr.LoadInstanceEncounters();

    sLog.outString("Loading Npc Text Id...");
    sObjectMgr.LoadNpcGossips();                            // must be after load Creature and LoadGossipText

    sLog.outString("Loading Scripts random templates...");  // must be before String calls
    sScriptMgr.LoadDbScriptRandomTemplates();
    ///- Load and initialize DBScripts Engine
    sLog.outString("Loading DB-Scripts Engine...");
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_RELAY);                // must be first in dbscripts loading
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_GOSSIP);               // must be before gossip menu options
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_QUEST_START);          // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_QUEST_END);            // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_SPELL);                // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_GAMEOBJECT);           // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_GAMEOBJECT_TEMPLATE);  // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_EVENT);                // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_CREATURE_DEATH);       // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadScriptMap(SCRIPT_TYPE_CREATURE_MOVEMENT);    // before loading from creature_movement 
    sLog.outString(">>> Scripts loaded");
    sLog.outString();

    sLog.outString("Loading Scripts text locales...");      // must be after Load*Scripts calls
    sScriptMgr.LoadDbScriptStrings();

    sLog.outString("Loading Gossip Menus...");
    sObjectMgr.LoadGossipMenus();

    sLog.outString("Loading Vendors...");
    sObjectMgr.LoadVendorTemplates();                       // must be after load ItemTemplate
    sObjectMgr.LoadVendors();                               // must be after load CreatureTemplate, VendorTemplate, and ItemTemplate

    sLog.outString("Loading Trainers...");
    sObjectMgr.LoadTrainerTemplates();                      // must be after load CreatureTemplate
    sObjectMgr.LoadTrainers();                              // must be after load CreatureTemplate, TrainerTemplate

    sLog.outString("Loading Waypoint scripts...");

    sLog.outString("Loading Waypoints...");
    sWaypointMgr.Load();

    sLog.outString("Loading ReservedNames...");
    sObjectMgr.LoadReservedPlayersNames();

    sLog.outString("Loading GameObjects for quests...");
    sObjectMgr.LoadGameObjectForQuests();

    sLog.outString("Loading BattleMasters...");
    sBattleGroundMgr.LoadBattleMastersEntry(false);

    sLog.outString("Loading BattleGround event indexes...");
    sBattleGroundMgr.LoadBattleEventIndexes(false);

    sLog.outString("Loading GameTeleports...");
    sObjectMgr.LoadGameTele();

    sLog.outString("Loading Questgiver Greetings...");
    sObjectMgr.LoadQuestgiverGreeting();

    sLog.outString("Loading Trainer Greetings...");
    sObjectMgr.LoadTrainerGreetings();

    ///- Loading localization data
    sLog.outString("Loading Localization strings...");
    sObjectMgr.LoadCreatureLocales();                       // must be after CreatureInfo loading
    sObjectMgr.LoadGameObjectLocales();                     // must be after GameobjectInfo loading
    sObjectMgr.LoadItemLocales();                           // must be after ItemPrototypes loading
    sObjectMgr.LoadQuestLocales();                          // must be after QuestTemplates loading
    sObjectMgr.LoadGossipTextLocales();                     // must be after LoadGossipText
    sObjectMgr.LoadPageTextLocales();                       // must be after PageText loading
    sObjectMgr.LoadGossipMenuItemsLocales();                // must be after gossip menu items loading
    sObjectMgr.LoadPointOfInterestLocales();                // must be after POI loading
    sObjectMgr.LoadQuestgiverGreetingLocales();
    sObjectMgr.LoadTrainerGreetingLocales();                // must be after CreatureInfo loading
    sObjectMgr.LoadBroadcastTextLocales();
    sLog.outString(">>> Localization strings loaded");
    sLog.outString();

    ///- Load dynamic data tables from the database
    sLog.outString("Loading Auctions...");
    sAuctionMgr.LoadAuctionItems();
    sAuctionMgr.LoadAuctions();
    sLog.outString(">>> Auctions loaded");
    sLog.outString();

    sLog.outString("Loading Guilds...");
    sGuildMgr.LoadGuilds();

    sLog.outString("Loading ArenaTeams...");
    sObjectMgr.LoadArenaTeams();

    sLog.outString("Loading Groups...");
    sObjectMgr.LoadGroups();

    sCalendarMgr.LoadCalendarsFromDB();

    sLog.outString("Returning old mails...");
    sObjectMgr.ReturnOrDeleteOldMails(false);

    sLog.outString("Loading GM tickets...");
    sTicketMgr.LoadGMTickets();

    ///- Load and initialize EventAI Scripts
    sLog.outString("Loading CreatureEventAI Summons...");
    sEventAIMgr.LoadCreatureEventAI_Summons(false);         // false, will checked in LoadCreatureEventAI_Scripts

    sLog.outString("Loading CreatureEventAI Scripts...");
    sEventAIMgr.LoadCreatureEventAI_Scripts();

    ///- Load and initialize scripting library
    sLog.outString("Initializing Scripting Library...");
    sScriptDevAIMgr.Initialize();
    sLog.outString();

    // after SD2
    sLog.outString("Loading spell scripts...");
    SpellScriptMgr::LoadScripts();

    // after spellscripts
    sScriptDevAIMgr.CheckScriptNames();

    ///- Initialize game time and timers
    sLog.outString("Initialize game time and timers");
    m_gameTime = time(nullptr);
    m_startTime = m_gameTime;

    time_t curr;
    time(&curr);
    tm local = *(localtime(&curr));                            // dereference and assign
    char isoDate[128];
    sprintf(isoDate, "%04d-%02d-%02d %02d:%02d:%02d",
            local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);

    LoginDatabase.PExecute("INSERT INTO uptime (realmid, starttime, startstring, uptime) VALUES('%u', " UI64FMTD ", '%s', 0)",
                           realmID, uint64(m_startTime), isoDate);

    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_UPTIME].SetInterval(getConfig(CONFIG_UINT32_UPTIME_UPDATE)*MINUTE * IN_MILLISECONDS);
    // Update "uptime" table based on configuration entry in minutes.
    m_timers[WUPDATE_CORPSES].SetInterval(20 * MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_DELETECHARS].SetInterval(DAY * IN_MILLISECONDS); // check for chars to delete every day
    m_timers[WUPDATE_RAID_BROWSER].SetInterval(IN_MILLISECONDS);

#ifdef BUILD_AHBOT
    // for AhBot
    m_timers[WUPDATE_AHBOT].SetInterval(20 * IN_MILLISECONDS); // every 20 sec
#endif

    // Update groups with offline leader after delay in seconds
    m_timers[WUPDATE_GROUPS].SetInterval(IN_MILLISECONDS);

    // to set mailtimer to return mails every day between 4 and 5 am
    // mailtimer is increased when updating auctions
    // one second is 1000 -(tested on win system)
    mail_timer = uint32((((localtime(&m_gameTime)->tm_hour + 20) % 24) * HOUR * IN_MILLISECONDS) / m_timers[WUPDATE_AUCTIONS].GetInterval());
    // 1440
    mail_timer_expires = uint32((DAY * IN_MILLISECONDS) / (m_timers[WUPDATE_AUCTIONS].GetInterval()));
    DEBUG_LOG("Mail timer set to: %u, mail return is called every %u minutes", mail_timer, mail_timer_expires);

    ///- Initialize static helper structures
    AIRegistry::Initialize();

    ///- Initialize Outdoor PvP
    sLog.outString("Starting Outdoor PvP System");          // should be before loading maps
    sOutdoorPvPMgr.InitOutdoorPvP();

    ///- Initialize MapManager
    sLog.outString("Starting Map System");
    sMapMgr.Initialize();
    sLog.outString();

    ///- Initialize Battlegrounds
    sLog.outString("Starting BattleGround System");
    sBattleGroundMgr.CreateInitialBattleGrounds();
    m_bgQueue.InitAutomaticArenaPointDistribution();
    CheckLootTemplates_Reference(ids_set);

    sLog.outString("Deleting expired bans...");

    LoginDatabase.Execute("DELETE FROM ip_banned WHERE expires_at<=" _UNIXTIME_ " AND expires_at<>banned_at");
    sLog.outString();

    sLog.outString("Calculate next daily quest reset time...");
    InitDailyQuestResetTime();
    sLog.outString();

    sLog.outString("Calculate next weekly quest reset time...");
    InitWeeklyQuestResetTime();
    sLog.outString();

    sLog.outString("Calculate next monthly quest reset time...");
    SetMonthlyQuestResetTime();
    sLog.outString();

    sLog.outString("Calculate random battleground reset time...");
    InitRandomBattlegroundResetTime();
    sLog.outString();

    sLog.outString("Loading Spam records...");
    LoadSpamRecords();
    sLog.outString();

    sLog.outString("Starting Game Event system...");
    uint32 nextGameEvent = sGameEventMgr.Initialize();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);    // depend on next event
    sLog.outString();

    sLog.outString("Loading Quest Group chosen quests...");
    LoadEventGroupChosen();
    sLog.outString();

    // Delete all characters which have been deleted X days before
    Player::DeleteOldCharacters();

    sLog.outString("Loading anticheat library");
    sAnticheatLib->Initialize();

#ifdef BUILD_AHBOT
    sLog.outString("Initialize AuctionHouseBot...");
    sAuctionHouseBot.Initialize();
    sLog.outString();
#endif

    sLog.outString("Loading WorldState");
    sWorldState.Load();
    sLog.outString();

#ifdef BUILD_METRICS
    // update metrics output every second
    m_timers[WUPDATE_METRICS].SetInterval(1 * IN_MILLISECONDS);
#endif // BUILD_METRICS


#ifdef BUILD_DEPRECATED_PLAYERBOT
    PlayerbotMgr::SetInitialWorldSettings();
#endif

#ifdef ENABLE_PLAYERBOTS
    sPlayerbotAIConfig.Initialize();
#ifndef BUILD_AHBOT
    auctionbot.Init();
#endif
#endif

    sLog.outString("---------------------------------------");
    sLog.outString("      CMANGOS: World initialized       ");
    sLog.outString("---------------------------------------");
    sLog.outString();

    uint32 uStartInterval = WorldTimer::getMSTimeDiff(uStartTime, WorldTimer::getMSTime());
    sLog.outString("SERVER STARTUP TIME: %i minutes %i seconds", uStartInterval / 60000, (uStartInterval % 60000) / 1000);
    sLog.outString();
}

void World::DetectDBCLang()
{
    uint32 m_lang_confid = sConfig.GetIntDefault("DBC.Locale", 255);

    if (m_lang_confid != 255 && m_lang_confid >= MAX_LOCALE)
    {
        sLog.outError("Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)", MAX_LOCALE);
        m_lang_confid = DEFAULT_LOCALE;
    }

    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(RACE_HUMAN);
    MANGOS_ASSERT(race);

    std::string availableLocalsStr;

    uint32 default_locale = MAX_LOCALE;
    for (int i = MAX_LOCALE - 1; i >= 0; --i)
    {
        if (strlen(race->name[i]) > 0)                      // check by race names
        {
            default_locale = i;
            m_availableDbcLocaleMask |= (1 << i);
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if (default_locale != m_lang_confid && m_lang_confid < MAX_LOCALE &&
            (m_availableDbcLocaleMask & (1 << m_lang_confid)))
    {
        default_locale = m_lang_confid;
    }

    if (default_locale >= MAX_LOCALE)
    {
        sLog.outError("Unable to determine your DBC Locale! (corrupt DBC?)");
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    m_defaultDbcLocale = LocaleConstant(default_locale);

    sLog.outString("Using %s DBC Locale as default. All available DBC locales: %s", localeNames[m_defaultDbcLocale], availableLocalsStr.empty() ? "<none>" : availableLocalsStr.c_str());
    sLog.outString();
}

/// Update the World !
void World::Update(uint32 diff)
{
    m_currentMSTime = WorldTimer::getMSTime();
    m_currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
    m_currentDiff = diff;

#ifdef ENABLE_PLAYERBOTS
    m_currentDiffSum += diff;
    m_currentDiffSumIndex++;

    m_histDiff.push_back(diff);
    m_maxDiff = std::max(m_maxDiff, diff);

    while (m_histDiff.size() >= 600)
    {
        m_currentDiffSum -= m_histDiff.front();
        m_histDiff.pop_front();
    }

    m_averageDiff = (uint32)(m_currentDiffSum / m_histDiff.size());

    if (m_currentDiffSumIndex && m_currentDiffSumIndex % 60 == 0)
    {
        sLog.outBasic("Avg Diff: %u. Sessions online: %u.", m_averageDiff, (uint32)GetActiveSessionCount());
        sLog.outBasic("Max Diff: %u.", m_maxDiff);
    }

    if (m_currentDiffSum % 3000 == 0)
    {
        m_maxDiff = *std::max_element(m_histDiff.begin(), m_histDiff.end());
    }
#endif

    ///- Update the different timers
    for (auto& m_timer : m_timers)
    {
        if (m_timer.GetCurrent() >= 0)
            m_timer.Update(diff);
        else
            m_timer.SetCurrent(0);
    }

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    GetMessager().Execute(this);

    ///-Update mass mailer tasks if any
    sMassMailMgr.Update();

    /// Handle daily quests reset time
    if (m_gameTime > m_NextDailyQuestReset)
        ResetDailyQuests();

    /// Handle weekly quests reset time
    if (m_gameTime > m_NextWeeklyQuestReset)
        ResetWeeklyQuests();

    /// Handle monthly quests reset time
    if (m_gameTime > m_NextMonthlyQuestReset)
        ResetMonthlyQuests();

    /// Handle daily random battleground reset time
    if (m_gameTime > m_NextRandomBattlegroundReset)
        ResetRandomBattleground();

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_AUCTIONS].Passed())
    {
        m_timers[WUPDATE_AUCTIONS].Reset();

        ///- Update mails (return old mails with item, or delete them)
        //(tested... works on win)
        if (++mail_timer > mail_timer_expires)
        {
            mail_timer = 0;
            sObjectMgr.ReturnOrDeleteOldMails(true);
        }

        ///- Handle expired auctions
        sAuctionMgr.Update();
    }

#ifdef BUILD_AHBOT
    /// <li> Handle AHBot operations
    if (m_timers[WUPDATE_AHBOT].Passed())
    {
        sAuctionHouseBot.Update();
        m_timers[WUPDATE_AHBOT].Reset();
    }
#endif

#ifdef ENABLE_PLAYERBOTS
#ifndef BUILD_AHBOT
    /// <li> Handle AHBot operations
    if (m_timers[WUPDATE_AHBOT].Passed())
    {
        auctionbot.Update();
        m_timers[WUPDATE_AHBOT].Reset();
    }
#endif
    sRandomPlayerbotMgr.UpdateAI(diff);
    sRandomPlayerbotMgr.UpdateSessions(diff);
#endif

    /// <li> Handle session updates
#ifdef BUILD_METRICS
    auto preSessionTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
#endif
    UpdateSessions(diff);

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff = uint32(m_gameTime - m_startTime);
        uint32 maxClientsNum = GetMaxActiveSessionCount();

        m_timers[WUPDATE_UPTIME].Reset();
        LoginDatabase.PExecute("UPDATE uptime SET uptime = %u, maxplayers = %u WHERE realmid = %u AND starttime = " UI64FMTD, tmpDiff, maxClientsNum, realmID, uint64(m_startTime));
    }

    /// <li> Handle all other objects
    ///- Update objects (maps, transport, creatures,...)
#ifdef BUILD_METRICS
    auto preMapTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
#endif
    sMapMgr.Update(diff);
#ifdef BUILD_METRICS
    auto postMapTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
#endif
    sBattleGroundMgr.Update(diff);
    sOutdoorPvPMgr.Update(diff);
    sWorldState.Update(diff);
#ifdef BUILD_METRICS
    auto postSingletonTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
#endif
    ///- Update groups with offline leaders
    if (m_timers[WUPDATE_GROUPS].Passed())
    {
        m_timers[WUPDATE_GROUPS].Reset();
        if (const uint32 delay = getConfig(CONFIG_UINT32_GROUP_OFFLINE_LEADER_DELAY))
        {
            for (ObjectMgr::GroupMap::const_iterator i = sObjectMgr.GetGroupMapBegin(); i != sObjectMgr.GetGroupMapEnd(); ++i)
                i->second->UpdateOfflineLeader(m_gameTime, delay);
        }
    }

    ///- Delete all characters which have been deleted X days before
    if (m_timers[WUPDATE_DELETECHARS].Passed())
    {
        m_timers[WUPDATE_DELETECHARS].Reset();
        Player::DeleteOldCharacters();
    }

    // execute callbacks from sql queries that were queued recently
    UpdateResultQueue();

    ///- Erase corpses once every 20 minutes
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();

        sObjectAccessor.RemoveOldCorpses();
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
        uint32 nextGameEvent = sGameEventMgr.Update();
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
    }

    //- Process Raid browser
    if (m_timers[WUPDATE_RAID_BROWSER].Passed())
    {
        m_timers[WUPDATE_RAID_BROWSER].Reset();
        GetRaidBrowser().Update(this);
    }

#ifdef BUILD_METRICS
    if (m_timers[WUPDATE_METRICS].Passed())
    {
        m_timers[WUPDATE_METRICS].Reset();
        GeneratePacketMetrics();
    }
#endif

    /// </ul>
    ///- Move all creatures with "delayed move" and remove and delete all objects with "delayed remove"
    sMapMgr.RemoveAllObjectsInRemoveList();

    // update the instance reset times
    sMapPersistentStateMgr.Update();

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();

    // cleanup unused GridMap objects as well as VMaps
    sTerrainMgr.Update(diff);
#ifdef BUILD_METRICS
    auto updateEndTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
    long long total = (updateEndTime - m_currentTime).count();
    long long presession = (preSessionTime - m_currentTime).count();
    long long premap = (preMapTime - preSessionTime).count();
    long long map = (postMapTime - preMapTime).count();
    long long singletons = (postSingletonTime - postMapTime).count();
    long long cleanup = (updateEndTime - postSingletonTime).count();

    metric::measurement meas("world.update");
    meas.add_field("total", std::to_string(total));
    meas.add_field("presession", std::to_string(presession));
    meas.add_field("premap", std::to_string(premap));
    meas.add_field("map", std::to_string(map));
    meas.add_field("singletons", std::to_string(singletons));
    meas.add_field("cleanup", std::to_string(cleanup));
#endif
}

namespace MaNGOS
{
    class WorldWorldTextBuilder
    {
        public:
            typedef std::vector<std::unique_ptr<WorldPacket>> WorldPacketList;
            explicit WorldWorldTextBuilder(int32 textId, va_list* args = nullptr) : i_textId(textId), i_args(args) {}
            void operator()(WorldPacketList& data_list, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId, loc_idx);

                if (i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap, *i_args);

                    char str [2048];
                    vsnprintf(str, 2048, text, ap);
                    va_end(ap);

                    do_helper(data_list, &str[0]);
                }
                else
                    do_helper(data_list, (char*)text);
            }
        private:
            char* lineFromMessage(char*& pos) const { char* start = strtok(pos, "\n"); pos = nullptr; return start; }
            void do_helper(WorldPacketList& data_list, char* text)
            {
                char* pos = text;

                while (char* line = lineFromMessage(pos))
                {
                    auto data = std::make_unique<WorldPacket>();
                    ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, line);
                    data_list.push_back(std::move(data));
                }
            }

            int32 i_textId;
            va_list* i_args;
    };
}                                                           // namespace MaNGOS

/// Sends a system message to all players
void World::SendWorldText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    MaNGOS::LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld())
                wt_do(player);
        }
    }

    va_end(ap);
}

/// Sends a system message to all given security level and above
void World::SendWorldTextToAboveSecurity(uint32 securityLevel, int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    MaNGOS::LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld())
                if (WorldSession* playerSession = player->GetSession())
                    if (uint32(playerSession->GetSecurity()) >= securityLevel)
                        wt_do(player);
        }
    }

    va_end(ap);
}

/// Sends a system message to all players who accept tickets
void World::SendWorldTextToAcceptingTickets(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    MaNGOS::LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->isAcceptTickets() && player->IsInWorld())
                wt_do(player);
        }
    }

    va_end(ap);
}

/// Sends a packet to all players with optional team and instance restrictions
void World::SendGlobalMessage(WorldPacket const& packet, uint32 team) const
{
    for (const auto& m_session : m_sessions)
    {
        if (WorldSession* session = m_session.second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld() && (team == 0 || team == player->GetTeam()))
                session->SendPacket(packet);
        }
    }
}

/// Sends a server message to the specified or all players
void World::SendServerMessage(ServerMessageType type, const char* text /*=""*/, Player* player /*= nullptr*/) const
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50);              // guess size
    data << uint32(type);
    data << text;

    if (player)
        player->GetSession()->SendPacket(data);
    else
        SendGlobalMessage(data);
}

/// Sends a zone under attack message to all players not in an instance
void World::SendZoneUnderAttackMessage(uint32 zoneId, Team team)
{
    WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
    data << uint32(zoneId);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld() && player->GetTeam() == team && !player->GetMap()->Instanceable())
                itr->second->SendPacket(data);
        }
    }
}

/// Sends a world defense message to all players not in an instance
void World::SendDefenseMessage(uint32 zoneId, int32 textId)
{
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld() && !player->GetMap()->Instanceable())
            {
                char const* message = session->GetMangosString(textId);
                uint32 messageLength = strlen(message) + 1;

                WorldPacket data(SMSG_DEFENSE_MESSAGE, 4 + 4 + messageLength);
                data << uint32(zoneId);
                data << uint32(messageLength);
                data << message;
                session->SendPacket(data);
            }
        }
    }
}

void World::SendDefenseMessageBroadcastText(uint32 zoneId, uint32 textId)
{
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (WorldSession* session = itr->second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld() && !player->GetMap()->Instanceable())
            {
                BroadcastText const* bct = sObjectMgr.GetBroadcastText(textId);
                std::string const& message = bct->GetText(session->GetSessionDbLocaleIndex());
                uint32 messageLength = message.size() + 1;

                WorldPacket data(SMSG_DEFENSE_MESSAGE, 4 + 4 + messageLength);
                data << uint32(zoneId);
                data << uint32(messageLength);
                data << message;
                session->SendPacket(data);
            }
        }
    }
}

/// Kick (and save) all players
void World::KickAll(bool save)
{
    m_QueuedSessions.clear();                               // prevent send queue update packet and login queued sessions

    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        itr->second->KickPlayer(save, true);
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllLess(AccountTypes sec)
{
    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (WorldSession* session = itr->second)
            if (session->GetSecurity() < sec)
                session->KickPlayer();
}

void World::WarnAccount(uint32 accountId, std::string from, std::string reason, const char* type)
{
    LoginDatabase.escape_string(from);
    reason = std::string(type) + ": " + reason;
    LoginDatabase.escape_string(reason);

    LoginDatabase.PExecute("INSERT INTO account_banned (account_id, banned_at, expires_at, banned_by, reason, active) VALUES ('%u', " _UNIXTIME_ ", " _UNIXTIME_ "+1, '%s', '%s', '0')",
        accountId, from.c_str(), reason.c_str());
}

BanReturn World::BanAccount(WorldSession *session, uint32 duration_secs, const std::string& reason, const std::string& author)
{
    if (duration_secs)
        LoginDatabase.PExecute("INSERT INTO account_banned(account_id, banned_at, expires_at, banned_by, reason, active) VALUES ('%u', " _UNIXTIME_ ", " _UNIXTIME_ "+%u, '%s', '%s', '1')",
            session->GetAccountId(), duration_secs, author.c_str(), reason.c_str());
    else
        LoginDatabase.PExecute("INSERT INTO account_banned(account_id, banned_at, expires_at, banned_by, reason, active) VALUES ('%u',  " _UNIXTIME_ ", 0, '%s', '%s', '1')",
            session->GetAccountId(), author.c_str(), reason.c_str());

    session->KickPlayer();
    return BAN_SUCCESS;
}

/// Ban an account or ban an IP address, duration_secs if it is positive used, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string nameOrIP, uint32 duration_secs, std::string reason, const std::string& author)
{
    LoginDatabase.escape_string(nameOrIP);
    LoginDatabase.escape_string(reason);
    std::string safe_author = author;
    LoginDatabase.escape_string(safe_author);

    std::unique_ptr<QueryResult> resultAccounts;  // used for kicking

    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            // No SQL injection as strings are escaped
            resultAccounts = LoginDatabase.PQuery("SELECT accountId FROM account_logons WHERE ip = '%s' ORDER BY loginTime DESC LIMIT 1", nameOrIP.c_str());
            LoginDatabase.PExecute("INSERT INTO ip_banned VALUES ('%s'," _UNIXTIME_ "," _UNIXTIME_ "+%u,'%s','%s')", nameOrIP.c_str(), duration_secs, safe_author.c_str(), reason.c_str());
            break;
        case BAN_ACCOUNT:
            // No SQL injection as string is escaped
            resultAccounts = LoginDatabase.PQuery("SELECT id FROM account WHERE username = '%s'", nameOrIP.c_str());
            break;
        case BAN_CHARACTER:
            // No SQL injection as string is escaped
            resultAccounts = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name = '%s'", nameOrIP.c_str());
            break;
        default:
            return BAN_SYNTAX_ERROR;
    }

    if (!resultAccounts)
    {
        if (mode == BAN_IP)
            return BAN_SUCCESS;                             // ip correctly banned but nobody affected (yet)
        return BAN_NOTFOUND;
        // Nobody to ban
    }

    ///- Disconnect all affected players (for IP it can be several)
    do
    {
        Field* fieldsAccount = resultAccounts->Fetch();
        uint32 account = fieldsAccount->GetUInt32();

        if (mode != BAN_IP)
        {
            // No SQL injection as strings are escaped
            LoginDatabase.PExecute("INSERT INTO account_banned(account_id, banned_at, expires_at, banned_by, reason, active) VALUES ('%u', " _UNIXTIME_ ", " _UNIXTIME_ "+%u, '%s', '%s', '1')",
                                   account, duration_secs, safe_author.c_str(), reason.c_str());
        }

        if (WorldSession* sess = FindSession(account))
            if (std::string(sess->GetPlayerName()) != author)
                sess->KickPlayer();
    }
    while (resultAccounts->NextRow());

    return BAN_SUCCESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, const std::string& source, const std::string& message, std::string nameOrIP)
{
    if (mode == BAN_IP)
    {
        LoginDatabase.escape_string(nameOrIP);
        LoginDatabase.PExecute("DELETE FROM ip_banned WHERE ip = '%s'", nameOrIP.c_str());
    }
    else
    {
        uint32 account = 0;
        if (mode == BAN_ACCOUNT)
            account = sAccountMgr.GetId(nameOrIP);
        else if (mode == BAN_CHARACTER)
            account = sObjectMgr.GetPlayerAccountIdByPlayerName(nameOrIP);

        if (!account)
            return false;

        // NO SQL injection as account is uint32
        LoginDatabase.PExecute("UPDATE account_banned SET active = '0', unbanned_at = " _UNIXTIME_ ", unbanned_by = '%s' WHERE account_id = '%u'", source.data(), account);
        WarnAccount(account, source, message, "UNBAN");
    }
    return true;
}

void World::BroadcastToGroup(ObjectGuid groupGuid, std::vector<WorldPacket> const& packets)
{
    if (Group* group = sObjectMgr.GetGroupById(groupGuid.GetCounter()))
        for (auto& packet : packets)
            group->BroadcastPacket(packet, false);
}

void World::BroadcastPersonalized(std::map<ObjectGuid, std::vector<WorldPacket>> const& personalizedPackets)
{
    for (auto& packets : personalizedPackets)
        if (Player* player = ObjectAccessor::FindPlayer(packets.first)) // TODO: pass these along directly to worldsession
            for (WorldPacket const& packet : packets.second)
                player->GetSession()->SendPacket(packet);
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(nullptr);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;

    ///- if there is a shutdown timer
    if (!m_stopEvent && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
                m_stopEvent = true;                         // exist code already set
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode)
{
    // ignore if server shutdown at next tick
    if (m_stopEvent)
        return;

    m_ShutdownMask = options;
    m_ExitCode = exitcode;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if (time == 0)
    {
        if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
            m_stopEvent = true;                             // exist code already set
        else
            m_ShutdownTimer = 1;                            // So that the session count is re-evaluated at next world tick
    }
    ///- Else set the shutdown timer and warn users
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show /*= false*/, Player* player /*= nullptr*/)
{
    // not show messages for idle shutdown mode
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    ///- Display a message every 12 hours, 1 hour, 5 minutes, 1 minute and 15 seconds
    if (show ||
            (m_ShutdownTimer <= 15) || // every sec down from 15 secs
            (m_ShutdownTimer < 5 * MINUTE && (m_ShutdownTimer % 15) == 0) ||            // < 5 min; every 15 sec
            (m_ShutdownTimer < 15 * MINUTE && (m_ShutdownTimer % MINUTE) == 0) ||       // < 15 min; every 1 min
            (m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) || // < 30 min; every 5 min
            (m_ShutdownTimer < 12 * HOUR && (m_ShutdownTimer % HOUR) == 0) ||           // < 12 h; every 1 h
            (m_ShutdownTimer >= 12 * HOUR && (m_ShutdownTimer % (12 * HOUR)) == 0))     // >= 12 h; every 12 h
    {
        std::string str = secsToTimeString(m_ShutdownTimer);

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid, str.c_str(), player);
        DEBUG_LOG("Server is %s in %s", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutting down"), str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    // nothing to cancel or too late
    if (!m_ShutdownTimer || m_stopEvent)
        return;

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_ExitCode = SHUTDOWN_EXIT_CODE;                       // to default value
    SendServerMessage(msgid);

    DEBUG_LOG("Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutdown"));
}

void World::UpdateSessions(uint32 diff)
{
    ///- Add new sessions
    {
        std::deque<WorldSession*> sessionQueueCopy;
        {
            std::lock_guard<std::mutex> guard(m_sessionAddQueueLock);
            std::swap(m_sessionAddQueue, sessionQueueCopy);
        }

        for (auto const& session : sessionQueueCopy)
            AddSession_(session);
    }

    ///- Then send an update signal to remaining ones
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end();)
    {
        ///- and remove not active sessions from the list
        WorldSession* pSession = itr->second;

        if (!pSession->Update(diff))
        {
            RemoveQueuedSession(pSession);
            itr = m_sessions.erase(itr);
            delete pSession;
        }
        else
            ++itr;
    }
}

// This handles the issued and queued CLI/RA commands
void World::ProcessCliCommands()
{
    std::lock_guard<std::mutex> guard(m_cliCommandQueueLock);

    while (!m_cliCommandQueue.empty())
    {
        auto const command = m_cliCommandQueue.front();
        m_cliCommandQueue.pop_front();

        DEBUG_LOG("CLI command under processing...");

        CliHandler handler(command->m_cliAccountId, command->m_cliAccessLevel, command->m_print);
        handler.ParseCommands(&command->m_command[0]);

        if (command->m_commandFinished)
            command->m_commandFinished(!handler.HasSentErrorMessage());

        delete command;
    }
}

void World::InitResultQueue()
{
}

void World::UpdateResultQueue()
{
    // process async result queues
    CharacterDatabase.ProcessResultQueue();
    WorldDatabase.ProcessResultQueue();
    LoginDatabase.ProcessResultQueue();
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    CharacterDatabase.AsyncPQuery(this, &World::_UpdateRealmCharCount, accountId,
                                  "SELECT COUNT(guid) FROM characters WHERE account = '%u'", accountId);
}

void World::_UpdateRealmCharCount(QueryResult* resultCharCount, uint32 accountId)
{
    if (resultCharCount)
    {
        Field* fields = resultCharCount->Fetch();
        uint32 charCount = fields[0].GetUInt32();
        delete resultCharCount;

        LoginDatabase.BeginTransaction();
        LoginDatabase.PExecute("DELETE FROM realmcharacters WHERE acctid= '%u' AND realmid = '%u'", accountId, realmID);
        LoginDatabase.PExecute("INSERT INTO realmcharacters (numchars, acctid, realmid) VALUES (%u, %u, %u)", charCount, accountId, realmID);
        LoginDatabase.CommitTransaction();
    }
}

void World::InitWeeklyQuestResetTime()
{
    auto queryResult = CharacterDatabase.Query("SELECT NextWeeklyQuestResetTime FROM saved_variables");
    if (!queryResult)
        m_NextWeeklyQuestReset = time_t(time(nullptr));        // game time not yet init
    else
        m_NextWeeklyQuestReset = time_t((*queryResult)[0].GetUInt64());

    // generate time by config
    time_t curTime = time(nullptr);
    tm localTm = *localtime(&curTime);

    int week_day_offset = localTm.tm_wday - int(getConfig(CONFIG_UINT32_QUEST_WEEKLY_RESET_WEEK_DAY));

    // current week reset time
    localTm.tm_hour = getConfig(CONFIG_UINT32_QUEST_WEEKLY_RESET_HOUR);
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;
    time_t nextWeekResetTime = mktime(&localTm);
    nextWeekResetTime -= week_day_offset * DAY;             // move time to proper day

    // next reset time before current moment
    if (curTime >= nextWeekResetTime)
        nextWeekResetTime += WEEK;

    // normalize reset time
    m_NextWeeklyQuestReset = m_NextWeeklyQuestReset < curTime ? nextWeekResetTime - WEEK : nextWeekResetTime;

    if (!queryResult)
        CharacterDatabase.PExecute("INSERT INTO saved_variables (NextWeeklyQuestResetTime) VALUES ('" UI64FMTD "')", uint64(m_NextWeeklyQuestReset));
}

void World::InitDailyQuestResetTime()
{
    auto queryResult = CharacterDatabase.Query("SELECT NextDailyQuestResetTime FROM saved_variables");
    if (!queryResult)
        m_NextDailyQuestReset = time_t(time(nullptr));         // game time not yet init
    else
        m_NextDailyQuestReset = time_t((*queryResult)[0].GetUInt64());

    // generate time by config
    time_t curTime = time(nullptr);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = getConfig(CONFIG_UINT32_QUEST_DAILY_RESET_HOUR);
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;

    // current day reset time
    time_t nextDayResetTime = mktime(&localTm);

    // next reset time before current moment
    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    // normalize reset time
    m_NextDailyQuestReset = m_NextDailyQuestReset < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!queryResult)
        CharacterDatabase.PExecute("INSERT INTO saved_variables (NextDailyQuestResetTime) VALUES ('" UI64FMTD "')", uint64(m_NextDailyQuestReset));
}

void World::SetMonthlyQuestResetTime(bool initialize)
{
    if (initialize)
    {
        auto queryResult = CharacterDatabase.Query("SELECT NextMonthlyQuestResetTime FROM saved_variables");

        if (!queryResult)
            m_NextMonthlyQuestReset = time_t(time(nullptr));
        else
            m_NextMonthlyQuestReset = time_t((*queryResult)[0].GetUInt64());
    }

    // generate time
    time_t currentTime = time(nullptr);
    tm localTm = *localtime(&currentTime);

    int month = localTm.tm_mon;
    int year = localTm.tm_year;

    ++month;

    // month 11 is december, next is january (0)
    if (month > 11)
    {
        month = 0;
        year += 1;
    }

    // reset time for next month
    localTm.tm_year = year;
    localTm.tm_mon = month;
    localTm.tm_mday = 1;                                    // don't know if we really need config option for day/hour
    localTm.tm_hour = 0;
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;

    time_t nextMonthResetTime = mktime(&localTm);

    m_NextMonthlyQuestReset = (initialize && m_NextMonthlyQuestReset < nextMonthResetTime) ? m_NextMonthlyQuestReset : nextMonthResetTime;

    // Row must exist for this to work. Currently row is added by InitDailyQuestResetTime(), called before this function
    CharacterDatabase.PExecute("UPDATE saved_variables SET NextMonthlyQuestResetTime = '" UI64FMTD "'", uint64(m_NextMonthlyQuestReset));
}

void World::GenerateEventGroupEvents(bool daily, bool weekly, bool deleteColumns)
{
    auto& eventGroups = sGameEventMgr.GetEventGroups();
    auto& events = sGameEventMgr.GetEventMap();
    for (auto itr = m_eventGroupChosen.begin(); itr != m_eventGroupChosen.end();)
    {
        if (!daily && events[*itr].length == 1440) // dailies
            ++itr;
        else if (!weekly && events[*itr].length == 10080) // weeklies
            ++itr;
        else
        {
            sGameEventMgr.StopEvent(*itr);
            itr = m_eventGroupChosen.erase(itr);
        }
    }

    for (auto& data : eventGroups) // For each quest group pick a set
    {
        if (!daily && events[data.second[0]].length == 1440) // dailies
            continue;
        if (!weekly && events[data.second[0]].length == 10080) // weeklies
            continue;
        if (deleteColumns)
            CharacterDatabase.PExecute("DELETE FROM event_group_chosen WHERE eventGroup = '%u'", data.first);

        uint32 random = urand(0, data.second.size() - 1);
        uint32 chosenId = data.second[random];
        CharacterDatabase.PExecute("INSERT INTO event_group_chosen(eventGroup,entry) VALUES('%u','%u')",
                                   data.first, chosenId);
        m_eventGroupChosen.push_back(chosenId);
        // start events
        sGameEventMgr.StartEvent(chosenId);
    }
}

void World::LoadEventGroupChosen()
{
    auto queryResult = CharacterDatabase.Query("SELECT entry FROM event_group_chosen");
    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();
            m_eventGroupChosen.push_back(fields[0].GetUInt32());
            sGameEventMgr.StartEvent(fields[0].GetUInt32(), false, true);
        }
        while (queryResult->NextRow());
    }
    else // if table not set yet, generate quests
        GenerateEventGroupEvents(true, true, false);
}

void World::LoadSpamRecords(bool reload)
{
    auto queryResult = WorldDatabase.Query("SELECT record FROM spam_records");

    if (queryResult)
    {
        if (reload)
            m_spamRecords.clear();

        do
        {
            Field* fields = queryResult->Fetch();
            std::string record = fields[0].GetCppString();

            m_spamRecords.push_back(record);
        } while (queryResult->NextRow());
    }
}

void World::InitRandomBattlegroundResetTime()
{
    auto queryResult = CharacterDatabase.Query("SELECT NextRandomBattlegroundResetTime FROM saved_variables");
    if (!queryResult)
        m_NextRandomBattlegroundReset = time_t(time(NULL));         // game time not yet init
    else
        m_NextRandomBattlegroundReset = time_t((*queryResult)[0].GetUInt64());

    // generate time by config
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = getConfig(CONFIG_UINT32_BATTLEGROUND_RANDOM_RESET_HOUR);
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;

    // current day reset time
    time_t nextDayResetTime = mktime(&localTm);

    // next reset time before current moment
    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    // normalize reset time
    m_NextRandomBattlegroundReset = m_NextRandomBattlegroundReset < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!queryResult)
        CharacterDatabase.PExecute("INSERT INTO saved_variables (NextRandomBattlegroundResetTime) VALUES ('" UI64FMTD "')", uint64(m_NextRandomBattlegroundReset));
}

void World::ResetDailyQuests()
{
    DETAIL_LOG("Daily quests reset for all characters.");
    CharacterDatabase.Execute("DELETE FROM character_queststatus_daily");
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetDailyQuestStatus();

    m_NextDailyQuestReset = time_t(m_NextDailyQuestReset + DAY);
    CharacterDatabase.PExecute("UPDATE saved_variables SET NextDailyQuestResetTime = '" UI64FMTD "'", uint64(m_NextDailyQuestReset));

    GenerateEventGroupEvents(true, false, true); // generate dailies and save to DB
}

void World::ResetWeeklyQuests()
{
    DETAIL_LOG("Weekly quests reset for all characters.");
    CharacterDatabase.Execute("DELETE FROM character_queststatus_weekly");
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetWeeklyQuestStatus();

    m_NextWeeklyQuestReset = time_t(m_NextWeeklyQuestReset + WEEK);
    CharacterDatabase.PExecute("UPDATE saved_variables SET NextWeeklyQuestResetTime = '" UI64FMTD "'", uint64(m_NextWeeklyQuestReset));

    GenerateEventGroupEvents(false, true, true); // generate weeklies and save to DB
    sGameEventMgr.WeeklyEventTimerRecalculation();
}

void World::ResetMonthlyQuests()
{
    DETAIL_LOG("Monthly quests reset for all characters.");

    CharacterDatabase.Execute(_TRUNCATE_ " character_queststatus_monthly");

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetMonthlyQuestStatus();

    SetMonthlyQuestResetTime(false);
}

void World::ResetRandomBattleground()
{
    DETAIL_LOG("Random battleground reset for all characters.");

    CharacterDatabase.Execute("DELETE FROM character_battleground_random");

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->SetRandomBattlegroundWinner(false);

    m_NextRandomBattlegroundReset = time_t(m_NextRandomBattlegroundReset + DAY);
    CharacterDatabase.PExecute("UPDATE saved_variables SET NextRandomBattlegroundResetTime = '" UI64FMTD "'", uint64(m_NextRandomBattlegroundReset));
}

void World::SetPlayerLimit(int32 limit, bool needUpdate)
{
    if (limit < -int32(SEC_ADMINISTRATOR))
        limit = -int32(SEC_ADMINISTRATOR);

    // lock update need
    bool db_update_need = needUpdate || (limit < 0) != (m_playerLimit < 0) || (limit < 0 && m_playerLimit < 0 && limit != m_playerLimit);

    m_playerLimit = limit;

    if (db_update_need)
        LoginDatabase.PExecute("UPDATE realmlist SET allowedSecurityLevel = '%u' WHERE id = '%u'",
                               uint32(GetPlayerSecurityLimit()), realmID);
}

void World::UpdateMaxSessionCounters()
{
    m_maxActiveSessionCount = std::max(m_maxActiveSessionCount, uint32(m_sessions.size() - m_QueuedSessions.size()));
    m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount, uint32(m_QueuedSessions.size()));
}

void World::SetOnlinePlayer(Team team, uint8 race, uint8 plClass, bool apply)
{
    if (apply)
    {
        ++m_onlineTeams[team == ALLIANCE ? 1 : 0];
        ++m_onlineRaces[race];
        ++m_onlineClasses[plClass];
    }
    else
    {
        --m_onlineTeams[team == ALLIANCE ? 1 : 0];
        --m_onlineRaces[race];
        --m_onlineClasses[plClass];
    }
}

void World::LoadDBVersion()
{
    auto queryResult = WorldDatabase.Query("SELECT version, creature_ai_version, cache_id FROM db_version LIMIT 1");
    if (queryResult)
    {
        Field* fields = queryResult->Fetch();

        m_DBVersion              = fields[0].GetCppString();
        m_CreatureEventAIVersion = fields[1].GetCppString();

        // will be overwrite by config values if different and non-0
        setConfig(CONFIG_UINT32_CLIENTCACHE_VERSION, fields[2].GetUInt32());
    }

    if (m_DBVersion.empty())
        m_DBVersion = "Unknown world database.";

    if (m_CreatureEventAIVersion.empty())
        m_CreatureEventAIVersion = "Unknown creature EventAI.";
}

void World::setConfig(eConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    setConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
    if (int32(getConfig(index)) < 0)
    {
        sLog.outError("%s (%i) can't be negative. Using %u instead.", fieldname, int32(getConfig(index)), defvalue);
        setConfig(index, defvalue);
    }
}

void World::setConfig(eConfigInt32Values index, char const* fieldname, int32 defvalue)
{
    setConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
}

void World::setConfig(eConfigFloatValues index, char const* fieldname, float defvalue)
{
    setConfig(index, sConfig.GetFloatDefault(fieldname, defvalue));
}

void World::setConfig(eConfigBoolValues index, char const* fieldname, bool defvalue)
{
    setConfig(index, sConfig.GetBoolDefault(fieldname, defvalue));
}

void World::setConfigPos(eConfigFloatValues index, char const* fieldname, float defvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < 0.0f)
    {
        sLog.outError("%s (%f) can't be negative. Using %f instead.", fieldname, getConfig(index), defvalue);
        setConfig(index, defvalue);
    }
}

void World::setConfigMin(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be >= %u. Using %u instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMin(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be >= %i. Using %i instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMin(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be >= %f. Using %f instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMinMax(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue, uint32 maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

void World::setConfigMinMax(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue, int32 maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

void World::setConfigMinMax(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue, float maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

bool World::configNoReload(bool reload, eConfigUInt32Values index, char const* fieldname, uint32 defvalue) const
{
    if (!reload)
        return true;

    uint32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%u).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigInt32Values index, char const* fieldname, int32 defvalue) const
{
    if (!reload)
        return true;

    int32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%i).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigFloatValues index, char const* fieldname, float defvalue) const
{
    if (!reload)
        return true;

    float val = sConfig.GetFloatDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%f).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigBoolValues index, char const* fieldname, bool defvalue) const
{
    if (!reload)
        return true;

    bool val = sConfig.GetBoolDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%s).", fieldname, getConfig(index) ? "'true'" : "'false'");

    return false;
}

void World::InvalidatePlayerDataToAllClient(ObjectGuid guid) const
{
    WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
    data << guid;
    SendGlobalMessage(data);
}

void World::SendGMTextFlags(uint32 accountFlag, int32 stringId, std::string type, const char* message)
{
    std::string mangosString = sObjectMgr.GetMangosString(stringId, DEFAULT_LOCALE);
    std::string output = mangosString + " Type: " + type + " Content: " + message;
    WorldPacket inform;
    ChatHandler::BuildChatPacket(inform, CHAT_MSG_WHISPER_INFORM, output.data(), LANG_UNIVERSAL, CHAT_TAG_NONE);
    GetMessager().AddMessage([inform, accountFlag](World* world)
    {
        world->ExecuteForAllSessions([inform, accountFlag](auto& data)
        {
            if (data.HasAccountFlag(accountFlag))
                data.SendPacket(inform);
        });
    });
}

void World::IncrementOpcodeCounter(uint32 opcodeId)
{
    ++m_opcodeCounters[opcodeId];
}

#ifdef BUILD_METRICS
void World::GeneratePacketMetrics()
{
    for (uint32 i = 0; i < NUM_MSG_TYPES; ++i)
    {
        if (m_opcodeCounters[i] == 0)
            continue;

        metric::measurement meas("world.metrics.packets.received", { {"opcode", opcodeTable[i].name} });
        meas.add_field("count", std::to_string(static_cast<uint32>(m_opcodeCounters[i])));

        // Reset counter
        m_opcodeCounters[i] = 0;
    }

    metric::measurement meas_players("world.metrics.players");
    meas_players.add_field("online", std::to_string(GetActiveSessionCount()));
    meas_players.add_field("unique", std::to_string(GetUniqueSessionCount()));
    meas_players.add_field("queued", std::to_string(GetQueuedSessionCount()));
    // team
    meas_players.add_field("alliance", std::to_string(GetOnlineTeamPlayers(true)));
    meas_players.add_field("horde", std::to_string(GetOnlineTeamPlayers(false)));
    // race
    meas_players.add_field("human", std::to_string(GetOnlineRacePlayers(RACE_HUMAN)));
    meas_players.add_field("dwarf", std::to_string(GetOnlineRacePlayers(RACE_DWARF)));
    meas_players.add_field("gnome", std::to_string(GetOnlineRacePlayers(RACE_GNOME)));
    meas_players.add_field("nelf", std::to_string(GetOnlineRacePlayers(RACE_NIGHTELF)));
    meas_players.add_field("draenei", std::to_string(GetOnlineRacePlayers(RACE_DRAENEI)));

    meas_players.add_field("orc", std::to_string(GetOnlineRacePlayers(RACE_ORC)));
    meas_players.add_field("undead", std::to_string(GetOnlineRacePlayers(RACE_UNDEAD)));
    meas_players.add_field("tauren", std::to_string(GetOnlineRacePlayers(RACE_TAUREN)));
    meas_players.add_field("troll", std::to_string(GetOnlineRacePlayers(RACE_TROLL)));
    meas_players.add_field("belf", std::to_string(GetOnlineRacePlayers(RACE_BLOODELF)));
    // class
    meas_players.add_field("warrior", std::to_string(GetOnlineClassPlayers(CLASS_WARRIOR)));
    meas_players.add_field("paladin", std::to_string(GetOnlineClassPlayers(CLASS_PALADIN)));
    meas_players.add_field("hunter", std::to_string(GetOnlineClassPlayers(CLASS_HUNTER)));
    meas_players.add_field("rogue", std::to_string(GetOnlineClassPlayers(CLASS_ROGUE)));
    meas_players.add_field("priest", std::to_string(GetOnlineClassPlayers(CLASS_PRIEST)));
    meas_players.add_field("shaman", std::to_string(GetOnlineClassPlayers(CLASS_SHAMAN)));
    meas_players.add_field("mage", std::to_string(GetOnlineClassPlayers(CLASS_MAGE)));
    meas_players.add_field("warlock", std::to_string(GetOnlineClassPlayers(CLASS_WARLOCK)));
    meas_players.add_field("druid", std::to_string(GetOnlineClassPlayers(CLASS_DRUID)));
    meas_players.add_field("deathknight", std::to_string(GetOnlineClassPlayers(CLASS_DEATH_KNIGHT)));

    metric::measurement meas_latency("world.metrics.latency");
    meas_latency.add_field("online", std::to_string(GetAverageLatency()));
}

uint32 World::GetAverageLatency() const
{
    if (m_sessions.size() == 0)
        return 0;

    uint32 result = 0;
    ExecuteForAllSessions([&](WorldSession const& session)
    {
        result += session.GetLatency();
    });
    result /= m_sessions.size();
    return result;
}
#endif

// Load or reload the graveyard links from the data
void World::LoadGraveyardZones()
{
    // Clear the map (in case we're reloading) and then add all the entries from
    // the database.

    // mGraveYardMap has records by both map id and area id. map ID records have their
    // most significant bit set. If you want to query for a map id you must
    // query map_id | (1 << 31) instead.
    auto& graveyardMap = GetGraveyardManager().GetGraveyardMap();

    auto queryResult = WorldDatabase.Query("SELECT id,ghost_loc,link_kind,faction FROM game_graveyard_zone");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u graveyard-zone links", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        ++count;
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        uint32 locId = fields[1].GetUInt32();
        uint32 linkKind = fields[2].GetUInt32();
        uint32 team = fields[3].GetUInt32();

        if (linkKind != 0 && linkKind != 1)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record with invalid `link_kind`=%d state, skipped.", linkKind);
            continue;
        }

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry<WorldSafeLocsEntry>(safeLocId);
        if (!entry)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for nonexistent graveyard (WorldSafeLocs.dbc id) %u, skipped.", safeLocId);
            continue;
        }

        if (linkKind == GRAVEYARD_AREALINK && GetAreaEntryByAreaID(locId) == nullptr)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for nonexistent area id (%u), skipped.", locId);
            continue;
        }

        if (linkKind == GRAVEYARD_MAPLINK && sMapStore.LookupEntry(locId) == nullptr)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for nonexistent map id (%u), skipped.", locId);
            continue;
        }

        if (team != TEAM_BOTH_ALLOWED && team != HORDE && team != ALLIANCE)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for non player faction (%u), skipped.", team);
            continue;
        }

        uint32 locKey = GraveyardManager::GraveyardLinkKey(locId, linkKind);
        if (GraveyardManager::FindGraveYardData(graveyardMap, safeLocId, locKey))
            sLog.outErrorDb("Table `game_graveyard_zone` has a duplicate record"
                " for Graveyard (ID: %u) and location (ID: %u, kind: %u), "
                "skipped.", safeLocId, locId, linkKind);
        else
        {
            GraveYardData data;
            data.safeLocId = safeLocId;
            data.team = Team(team);
            graveyardMap.insert(GraveYardMap::value_type(locKey, data));
        }
    } while (queryResult->NextRow());

    sLog.outString(">> Loaded %u graveyard-zone links", count);
    sLog.outString();
}

void World::LoadWorldSafeLocs() const
{
    sWorldSafeLocsStore.Load(true);
    sLog.outString(">> Loaded %u world safe locs", sWorldSafeLocsStore.GetRecordCount());
}

void World::StartLFGQueueThread()
{
    m_lfgQueueThread = std::thread([&]()
    {
        m_lfgQueue.Update();
    });
}

void World::StartBGQueueThread()
{
    m_bgQueueThread = std::thread([&]()
    {
        m_bgQueue.Update();
    });
}
