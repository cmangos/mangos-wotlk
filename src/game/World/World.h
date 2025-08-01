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

/// \addtogroup world The World
/// @{
/// \file

#ifndef __WORLD_H
#define __WORLD_H

#include "Common.h"
#include "Util/Timer.h"
#include "Globals/Locales.h"
#include "Globals/SharedDefines.h"
#include "Entities/Object.h"
#include "Multithreading/Messager.h"
#include "Globals/GraveyardManager.h"
#include "LFG/LFG.h"
#include "LFG/LFGQueue.h"
#include "BattleGround/BattleGroundQueue.h"

#include <set>
#include <list>
#include <deque>
#include <mutex>
#include <functional>
#include <utility>
#include <vector>
#include <array>
#include <thread>

class Object;
class ObjectGuid;
class WorldPacket;
class WorldSession;
class Player;
class QueryResult;
class WorldSocket;

// ServerMessages.dbc
enum ServerMessageType
{
    SERVER_MSG_SHUTDOWN_TIME          = 1,
    SERVER_MSG_RESTART_TIME           = 2,
    SERVER_MSG_CUSTOM                 = 3,
    SERVER_MSG_SHUTDOWN_CANCELLED     = 4,
    SERVER_MSG_RESTART_CANCELLED      = 5,
    SERVER_MSG_BG_SHUTDOWN_TIME       = 6,
    SERVER_MSG_BG_RESTART_TIME        = 7,
    SERVER_MSG_INSTANCE_SHUTDOWN_TIME = 8,
    SERVER_MSG_INSTANCE_RESTART_TIME  = 9,
};

enum ShutdownMask
{
    SHUTDOWN_MASK_RESTART = 1,
    SHUTDOWN_MASK_IDLE    = 2,
};

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

/// Timers for different object refresh rates
enum WorldTimers
{
    WUPDATE_AUCTIONS    = 0,
    WUPDATE_UPTIME      = 1,
    WUPDATE_CORPSES     = 2,
    WUPDATE_EVENTS      = 3,
    WUPDATE_DELETECHARS = 4,
    WUPDATE_AHBOT       = 5,
    WUPDATE_GROUPS      = 6,
    WUPDATE_RAID_BROWSER= 7,
    WUPDATE_METRICS     = 8, // not used if BUILD_METRICS is not set
    WUPDATE_COUNT       = 9
};

/// Configuration elements
enum eConfigUInt32Values
{
    CONFIG_UINT32_COMPRESSION = 0,
    CONFIG_UINT32_INTERVAL_SAVE,
    CONFIG_UINT32_INTERVAL_GRIDCLEAN,
    CONFIG_UINT32_INTERVAL_MAPUPDATE,
    CONFIG_UINT32_INTERVAL_CHANGEWEATHER,
    CONFIG_UINT32_PORT_WORLD,
    CONFIG_UINT32_GAME_TYPE,
    CONFIG_UINT32_REALM_ZONE,
    CONFIG_UINT32_STRICT_PLAYER_NAMES,
    CONFIG_UINT32_STRICT_CHARTER_NAMES,
    CONFIG_UINT32_STRICT_PET_NAMES,
    CONFIG_UINT32_MIN_PLAYER_NAME,
    CONFIG_UINT32_MIN_CHARTER_NAME,
    CONFIG_UINT32_MIN_PET_NAME,
    CONFIG_UINT32_CHARACTERS_CREATING_DISABLED,
    CONFIG_UINT32_CHARACTERS_PER_ACCOUNT,
    CONFIG_UINT32_CHARACTERS_PER_REALM,
    CONFIG_UINT32_HEROIC_CHARACTERS_PER_REALM,
    CONFIG_UINT32_MIN_LEVEL_FOR_HEROIC_CHARACTER_CREATING,
    CONFIG_UINT32_SKIP_CINEMATICS,
    CONFIG_UINT32_MAX_PLAYER_LEVEL,
    CONFIG_UINT32_START_PLAYER_LEVEL,
    CONFIG_UINT32_START_HEROIC_PLAYER_LEVEL,
    CONFIG_UINT32_START_PLAYER_MONEY,
    CONFIG_UINT32_MAX_HONOR_POINTS,
    CONFIG_UINT32_START_HONOR_POINTS,
    CONFIG_UINT32_MAX_ARENA_POINTS,
    CONFIG_UINT32_START_ARENA_POINTS,
    CONFIG_UINT32_INSTANCE_RESET_TIME_HOUR,
    CONFIG_UINT32_INSTANCE_UNLOAD_DELAY,
    CONFIG_UINT32_MAX_SPELL_CASTS_IN_CHAIN,
    CONFIG_UINT32_BIRTHDAY_TIME,
    CONFIG_UINT32_RABBIT_DAY,
    CONFIG_UINT32_MAX_PRIMARY_TRADE_SKILL,
    CONFIG_UINT32_TRADE_SKILL_GMIGNORE_MAX_PRIMARY_COUNT,
    CONFIG_UINT32_TRADE_SKILL_GMIGNORE_LEVEL,
    CONFIG_UINT32_TRADE_SKILL_GMIGNORE_SKILL,
    CONFIG_UINT32_MIN_PETITION_SIGNS,
    CONFIG_UINT32_GM_LOGIN_STATE,
    CONFIG_UINT32_GM_VISIBLE_STATE,
    CONFIG_UINT32_GM_ACCEPT_TICKETS,
    CONFIG_UINT32_GM_LEVEL_ACCEPT_TICKETS,
    CONFIG_UINT32_GM_CHAT,
    CONFIG_UINT32_GM_LEVEL_CHAT,
    CONFIG_UINT32_GM_LEVEL_CHANNEL_MODERATION,
    CONFIG_UINT32_GM_LEVEL_CHANNEL_SILENT_JOIN,
    CONFIG_UINT32_GM_WISPERING_TO,
    CONFIG_UINT32_GM_LEVEL_IN_GM_LIST,
    CONFIG_UINT32_GM_LEVEL_IN_WHO_LIST,
    CONFIG_UINT32_START_GM_LEVEL,
    CONFIG_UINT32_GM_INVISIBLE_AURA,
    CONFIG_UINT32_MAIL_DELIVERY_DELAY,
    CONFIG_UINT32_MASS_MAILER_SEND_PER_TICK,
    CONFIG_UINT32_UPTIME_UPDATE,
    CONFIG_UINT32_NUM_MAP_THREADS,
    CONFIG_UINT32_AUCTION_DEPOSIT_MIN,
    CONFIG_UINT32_SKILL_CHANCE_ORANGE,
    CONFIG_UINT32_SKILL_CHANCE_YELLOW,
    CONFIG_UINT32_SKILL_CHANCE_GREEN,
    CONFIG_UINT32_SKILL_CHANCE_GREY,
    CONFIG_UINT32_SKILL_CHANCE_MINING_STEPS,
    CONFIG_UINT32_SKILL_CHANCE_SKINNING_STEPS,
    CONFIG_UINT32_SKILL_GAIN_CRAFTING,
    CONFIG_UINT32_SKILL_GAIN_DEFENSE,
    CONFIG_UINT32_SKILL_GAIN_GATHERING,
    CONFIG_UINT32_SKILL_GAIN_WEAPON,
    CONFIG_UINT32_MAX_OVERSPEED_PINGS,
    CONFIG_UINT32_EXPANSION,
    CONFIG_UINT32_CHATFLOOD_MESSAGE_COUNT,
    CONFIG_UINT32_CHATFLOOD_MESSAGE_DELAY,
    CONFIG_UINT32_CHATFLOOD_MUTE_TIME,
    CONFIG_UINT32_CREATURE_FAMILY_ASSISTANCE_DELAY,
    CONFIG_UINT32_CREATURE_FAMILY_FLEE_DELAY,
    CONFIG_UINT32_WORLD_BOSS_LEVEL_DIFF,
    CONFIG_UINT32_QUEST_DAILY_RESET_HOUR,
    CONFIG_UINT32_QUEST_WEEKLY_RESET_WEEK_DAY,
    CONFIG_UINT32_QUEST_WEEKLY_RESET_HOUR,
    CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY,
    CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_KICK,
    CONFIG_UINT32_CHANNEL_RESTRICTED_LANGUAGE_MODE,
    CONFIG_UINT32_CORPSE_DECAY_NORMAL,
    CONFIG_UINT32_CORPSE_DECAY_RARE,
    CONFIG_UINT32_CORPSE_DECAY_ELITE,
    CONFIG_UINT32_CORPSE_DECAY_RAREELITE,
    CONFIG_UINT32_CORPSE_DECAY_WORLDBOSS,
    CONFIG_UINT32_INSTANT_LOGOUT,
    CONFIG_UINT32_BATTLEGROUND_INVITATION_TYPE,
    CONFIG_UINT32_BATTLEGROUND_PREMATURE_FINISH_TIMER,
    CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH,
    CONFIG_UINT32_BATTLEGROUND_QUEUE_ANNOUNCER_JOIN,
    CONFIG_UINT32_BATTLEGROUND_RANDOM_RESET_HOUR,
    CONFIG_UINT32_ARENA_MAX_RATING_DIFFERENCE,
    CONFIG_UINT32_ARENA_RATING_DISCARD_TIMER,
    CONFIG_UINT32_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS,
    CONFIG_UINT32_ARENA_SEASON_ID,
    CONFIG_UINT32_ARENA_FIRST_RESET_DAY,
    CONFIG_UINT32_ARENA_SEASON_PREVIOUS_ID,
    CONFIG_UINT32_GROUP_OFFLINE_LEADER_DELAY,
    CONFIG_UINT32_BATTLEFIELD_COOLDOWN_DURATION,
    CONFIG_UINT32_BATTLEFIELD_BATTLE_DURATION,
    CONFIG_UINT32_BATTLEFIELD_MAX_PLAYERS_PER_TEAM,
    CONFIG_UINT32_CLIENTCACHE_VERSION,
    CONFIG_UINT32_GUILD_EVENT_LOG_COUNT,
    CONFIG_UINT32_GUILD_BANK_EVENT_LOG_COUNT,
    CONFIG_UINT32_MIRRORTIMER_FATIGUE_MAX,
    CONFIG_UINT32_MIRRORTIMER_BREATH_MAX,
    CONFIG_UINT32_MIRRORTIMER_ENVIRONMENTAL_MAX,
    CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MIN,
    CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MAX,
    CONFIG_UINT32_INTERACTION_PAUSE_TIMER,
    CONFIG_UINT32_MIN_LEVEL_STAT_SAVE,
    CONFIG_UINT32_CHARDELETE_KEEP_DAYS,
    CONFIG_UINT32_CHARDELETE_METHOD,
    CONFIG_UINT32_CHARDELETE_MIN_LEVEL,
    CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE,
    CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT,
    CONFIG_UINT32_MIN_LEVEL_FOR_RAID,
    CONFIG_UINT32_CREATURE_RESPAWN_AGGRO_DELAY,
    CONFIG_UINT32_CREATURE_CHECK_FOR_HELP_AGGRO_DELAY,
    CONFIG_UINT32_CREATURE_LINKING_AGGRO_DELAY,
    CONFIG_UINT32_MAX_WHOLIST_RETURNS,
    CONFIG_UINT32_FOGOFWAR_STEALTH,
    CONFIG_UINT32_FOGOFWAR_HEALTH,
    CONFIG_UINT32_FOGOFWAR_STATS,
    CONFIG_UINT32_CREATURE_PICKPOCKET_RESTOCK_DELAY,
    CONFIG_UINT32_CHANNEL_STATIC_AUTO_TRESHOLD,
    CONFIG_UINT32_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL,
    CONFIG_UINT32_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL_DIFFERENCE,
    CONFIG_UINT32_SUNSREACH_COUNTER,
    CONFIG_UINT32_VALUE_COUNT
};

/// Configuration elements
enum eConfigInt32Values
{
    CONFIG_INT32_DEATH_SICKNESS_LEVEL = 0,
    CONFIG_INT32_ARENA_STARTRATING,
    CONFIG_INT32_ARENA_STARTPERSONALRATING,
    CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF,
    CONFIG_INT32_QUEST_HIGH_LEVEL_HIDE_DIFF,
    CONFIG_INT32_VALUE_COUNT
};

/// Server config
enum eConfigFloatValues
{
    CONFIG_FLOAT_RATE_HEALTH = 0,
    CONFIG_FLOAT_RATE_POWER_MANA,
    CONFIG_FLOAT_RATE_POWER_RAGE_INCOME,
    CONFIG_FLOAT_RATE_POWER_RAGE_LOSS,
    CONFIG_FLOAT_RATE_POWER_RUNICPOWER_INCOME,
    CONFIG_FLOAT_RATE_POWER_RUNICPOWER_LOSS,
    CONFIG_FLOAT_RATE_POWER_FOCUS,
    CONFIG_FLOAT_RATE_POWER_ENERGY,
    CONFIG_FLOAT_RATE_SKILL_DISCOVERY,
    CONFIG_FLOAT_RATE_DROP_ITEM_POOR,
    CONFIG_FLOAT_RATE_DROP_ITEM_NORMAL,
    CONFIG_FLOAT_RATE_DROP_ITEM_UNCOMMON,
    CONFIG_FLOAT_RATE_DROP_ITEM_RARE,
    CONFIG_FLOAT_RATE_DROP_ITEM_EPIC,
    CONFIG_FLOAT_RATE_DROP_ITEM_LEGENDARY,
    CONFIG_FLOAT_RATE_DROP_ITEM_ARTIFACT,
    CONFIG_FLOAT_RATE_DROP_ITEM_REFERENCED,
    CONFIG_FLOAT_RATE_DROP_ITEM_QUEST,
    CONFIG_FLOAT_RATE_DROP_MONEY,
    CONFIG_FLOAT_RATE_PET_XP_KILL,
    CONFIG_FLOAT_RATE_PET_XP_KILL_VANILLA,
    CONFIG_FLOAT_RATE_PET_XP_KILL_BC,
    CONFIG_FLOAT_RATE_PET_XP_KILL_WOTLK,
    CONFIG_FLOAT_RATE_XP_KILL,
    CONFIG_FLOAT_RATE_XP_KILL_VANILLA,
    CONFIG_FLOAT_RATE_XP_KILL_BC,
    CONFIG_FLOAT_RATE_XP_KILL_WOTLK,
    CONFIG_FLOAT_RATE_XP_QUEST,
    CONFIG_FLOAT_RATE_XP_QUEST_VANILLA,
    CONFIG_FLOAT_RATE_XP_QUEST_BC,
    CONFIG_FLOAT_RATE_XP_QUEST_WOTLK,
    CONFIG_FLOAT_RATE_XP_EXPLORE,
    CONFIG_FLOAT_RATE_XP_EXPLORE_VANILLA,
    CONFIG_FLOAT_RATE_XP_EXPLORE_BC,
    CONFIG_FLOAT_RATE_XP_EXPLORE_WOTLK,
    CONFIG_FLOAT_RATE_REPUTATION_GAIN,
    CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_KILL,
    CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_QUEST,
    CONFIG_FLOAT_RATE_CREATURE_NORMAL_HP,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_HP,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_HP,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_HP,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_HP,
    CONFIG_FLOAT_RATE_CREATURE_NORMAL_DAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_DAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_DAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_DAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_NORMAL_SPELLDAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,
    CONFIG_FLOAT_RATE_CREATURE_AGGRO,
    CONFIG_FLOAT_RATE_REST_INGAME,
    CONFIG_FLOAT_RATE_REST_OFFLINE_IN_TAVERN_OR_CITY,
    CONFIG_FLOAT_RATE_REST_OFFLINE_IN_WILDERNESS,
    CONFIG_FLOAT_RATE_DAMAGE_FALL,
    CONFIG_FLOAT_RATE_AUCTION_TIME,
    CONFIG_FLOAT_RATE_AUCTION_DEPOSIT,
    CONFIG_FLOAT_RATE_AUCTION_CUT,
    CONFIG_FLOAT_RATE_HONOR,
    CONFIG_FLOAT_RATE_MINING_AMOUNT,
    CONFIG_FLOAT_RATE_MINING_NEXT,
    CONFIG_FLOAT_RATE_TALENT,
    CONFIG_FLOAT_RATE_CORPSE_DECAY_LOOTED,
    CONFIG_FLOAT_RATE_INSTANCE_RESET_TIME,
    CONFIG_FLOAT_RATE_DURABILITY_LOSS_DAMAGE,
    CONFIG_FLOAT_RATE_DURABILITY_LOSS_PARRY,
    CONFIG_FLOAT_RATE_DURABILITY_LOSS_ABSORB,
    CONFIG_FLOAT_RATE_DURABILITY_LOSS_BLOCK,
    CONFIG_FLOAT_SIGHT_GUARDER,
    CONFIG_FLOAT_SIGHT_MONSTER,
    CONFIG_FLOAT_LISTEN_RANGE_SAY,
    CONFIG_FLOAT_LISTEN_RANGE_YELL,
    CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE,
    CONFIG_FLOAT_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS,
    CONFIG_FLOAT_CREATURE_FAMILY_ASSISTANCE_RADIUS,
    CONFIG_FLOAT_CREATURE_CHECK_FOR_HELP_RADIUS,
    CONFIG_FLOAT_GROUP_XP_DISTANCE,
    CONFIG_FLOAT_GHOST_RUN_SPEED_WORLD,
    CONFIG_FLOAT_GHOST_RUN_SPEED_BG,
    CONFIG_FLOAT_LEASH_RADIUS,
    CONFIG_FLOAT_MOD_DISCOUNT_REPUTATION_FRIENDLY, // TODO
    CONFIG_FLOAT_MOD_DISCOUNT_REPUTATION_HONORED,
    CONFIG_FLOAT_MOD_DISCOUNT_REPUTATION_REVERED,
    CONFIG_FLOAT_MOD_DISCOUNT_REPUTATION_EXALTED,
    CONFIG_FLOAT_MOD_INCREASED_XP,
    CONFIG_FLOAT_MOD_INCREASED_GOLD,
    CONFIG_FLOAT_MAX_RECRUIT_A_FRIEND_DISTANCE,
    CONFIG_FLOAT_VALUE_COUNT
};

/// Configuration elements
enum eConfigBoolValues
{
    CONFIG_BOOL_GRID_UNLOAD = 0,
    CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY,
    CONFIG_BOOL_OFFHAND_CHECK_AT_TALENTS_RESET,
    CONFIG_BOOL_ALLOW_TWO_SIDE_ACCOUNTS,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHANNEL,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_AUCTION,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_MAIL,
    CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CALENDAR,
    CONFIG_BOOL_ALLOW_TWO_SIDE_WHO_LIST,
    CONFIG_BOOL_ALLOW_TWO_SIDE_ADD_FRIEND,
    CONFIG_BOOL_INSTANCE_IGNORE_LEVEL,
    CONFIG_BOOL_INSTANCE_IGNORE_RAID,
    CONFIG_BOOL_INSTANCE_STRICT_COMBAT_LOCKDOWN,
    CONFIG_BOOL_CAST_UNSTUCK,
    CONFIG_BOOL_GM_LOG_TRADE,
    CONFIG_BOOL_GM_LOWER_SECURITY,
    CONFIG_BOOL_GM_TICKETS_QUEUE_STATUS,
    CONFIG_BOOL_GM_ALLOW_ACHIEVEMENT_GAINS,
    CONFIG_BOOL_SKILL_PROSPECTING,
    CONFIG_BOOL_ALWAYS_MAX_SKILL_FOR_LEVEL,
    CONFIG_BOOL_WEATHER,
    CONFIG_BOOL_EVENT_ANNOUNCE,
    CONFIG_BOOL_QUEST_IGNORE_RAID,
    CONFIG_BOOL_DETECT_POS_COLLISION,
    CONFIG_BOOL_CHAT_RESTRICTED_RAID_WARNINGS,
    CONFIG_BOOL_CHANNEL_RESTRICTED_LFG,
    CONFIG_BOOL_TALENTS_INSPECTING,
    CONFIG_BOOL_CHAT_FAKE_MESSAGE_PREVENTING,
    CONFIG_BOOL_CHAT_STRICT_LINK_CHECKING_SEVERITY,
    CONFIG_BOOL_CHAT_STRICT_LINK_CHECKING_KICK,
    CONFIG_BOOL_ADDON_CHANNEL,
    CONFIG_BOOL_CORPSE_EMPTY_LOOT_SHOW,
    CONFIG_BOOL_CORPSE_ALLOW_ALL_ITEMS_SHOW_IN_MASTER_LOOT,
    CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVP,
    CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVE,
    CONFIG_BOOL_DEATH_BONES_WORLD,
    CONFIG_BOOL_DEATH_BONES_BG_OR_ARENA,
    CONFIG_BOOL_TAXI_FLIGHT_CHAT_FIX,
    CONFIG_BOOL_LONG_TAXI_PATHS_PERSISTENCE,
    CONFIG_BOOL_ALL_TAXI_PATHS,
    CONFIG_BOOL_DECLINED_NAMES_USED,
    CONFIG_BOOL_SKILL_MILLING,
    CONFIG_BOOL_SKILL_FAIL_LOOT_FISHING,
    CONFIG_BOOL_SKILL_FAIL_GAIN_FISHING,
    CONFIG_BOOL_SKILL_FAIL_POSSIBLE_FISHINGPOOL,
    CONFIG_BOOL_BATTLEGROUND_CAST_DESERTER,
    CONFIG_BOOL_BATTLEGROUND_QUEUE_ANNOUNCER_START,
    CONFIG_BOOL_BATTLEGROUND_SCORE_STATISTICS,
    CONFIG_BOOL_ARENA_AUTO_DISTRIBUTE_POINTS,
    CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_JOIN,
    CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_EXIT,
    CONFIG_BOOL_OUTDOORPVP_SI_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_EP_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_HP_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_ZM_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_TF_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_NA_ENABLED,
    CONFIG_BOOL_OUTDOORPVP_GH_ENABLED,
    CONFIG_BOOL_BATTLEFIELD_WG_ENABLED,
    CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET,
    CONFIG_BOOL_STATS_SAVE_ONLY_ON_LOGOUT,
    CONFIG_BOOL_CLEAN_CHARACTER_DB,
    CONFIG_BOOL_VMAP_INDOOR_CHECK,
    CONFIG_BOOL_PET_UNSUMMON_AT_MOUNT,
    CONFIG_BOOL_KEEP_PET_ON_FLYING_MOUNT,
    CONFIG_BOOL_PET_ATTACK_FROM_BEHIND,
    CONFIG_BOOL_AUTO_DOWNRANK,
    CONFIG_BOOL_MMAP_ENABLED,
    CONFIG_BOOL_PLAYER_COMMANDS,
    CONFIG_BOOL_AUTOLOAD_ACTIVE,
    CONFIG_BOOL_PATH_FIND_OPTIMIZE,
    CONFIG_BOOL_PATH_FIND_NORMALIZE_Z,
    CONFIG_BOOL_ALWAYS_SHOW_QUEST_GREETING,
    CONFIG_BOOL_DISABLE_INSTANCE_RELOCATE,
    CONFIG_BOOL_PRELOAD_MMAP_TILES,
    CONFIG_BOOL_SPECIALS_ACTIVE,
    CONFIG_BOOL_REGEN_ZONE_AREA_ON_STARTUP,
    CONFIG_BOOL_VALUE_COUNT
};

/// Can be used in SMSG_AUTH_RESPONSE packet
enum BillingPlanFlags
{
    SESSION_NONE            = 0x00,
    SESSION_UNUSED          = 0x01,
    SESSION_RECURRING_BILL  = 0x02,
    SESSION_FREE_TRIAL      = 0x04,
    SESSION_IGR             = 0x08,
    SESSION_USAGE           = 0x10,
    SESSION_TIME_MIXTURE    = 0x20,
    SESSION_RESTRICTED      = 0x40,
    SESSION_ENABLE_CAIS     = 0x80,
};

/// Type of server, this is values from second column of Cfg_Configs.dbc (1.12.1 have another numeration)
enum RealmType
{
    REALM_TYPE_NORMAL   = 0,
    REALM_TYPE_PVP      = 1,
    REALM_TYPE_NORMAL2  = 4,
    REALM_TYPE_RP       = 6,
    REALM_TYPE_RPPVP    = 8,
    REALM_TYPE_FFA_PVP  = 16                                // custom, free for all pvp mode like arena PvP in all zones except rest activated places and sanctuaries
                          // replaced by REALM_PVP in realm list
};

/// Storage class for commands issued for delayed execution
struct CliCommandHolder
{
    typedef std::function<void(const char*)> Print;
    typedef std::function<void(bool)> CommandFinished;

    uint32 m_cliAccountId;                                  // 0 for console and real account id for RA/soap
    AccountTypes m_cliAccessLevel;
    std::vector<char> m_command;
    Print m_print;
    CommandFinished m_commandFinished;

    CliCommandHolder(uint32 accountId, AccountTypes cliAccessLevel, const char* command, Print print, CommandFinished commandFinished)
        : m_cliAccountId(accountId), m_cliAccessLevel(cliAccessLevel), m_command(strlen(command) + 1), m_print(std::move(print)), m_commandFinished(std::move(commandFinished))
    {
        memcpy(&m_command[0], command, m_command.size() - 1);
    }
};

/// The World
class World
{
    public:
        static volatile uint32 m_worldLoopCounter;

        World();
        ~World();

        void CleanupsBeforeStop();

        WorldSession* FindSession(uint32 id) const;
        void AddSession(WorldSession* s);
        bool RemoveSession(uint32 id);
        /// Get the number of current active sessions
        void UpdateMaxSessionCounters();
        uint32 GetActiveAndQueuedSessionCount() const { return m_sessions.size(); }
        uint32 GetActiveSessionCount() const { return m_sessions.size() - m_QueuedSessions.size(); }
        uint32 GetQueuedSessionCount() const { return m_QueuedSessions.size(); }
        /// Get the maximum number of parallel sessions on the server since last reboot
        uint32 GetMaxQueuedSessionCount() const { return m_maxQueuedSessionCount; }
        uint32 GetMaxActiveSessionCount() const { return m_maxActiveSessionCount; }
        uint32 GetUniqueSessionCount() const { return m_uniqueSessionCount.size(); }
        // player counts
        void SetOnlinePlayer(Team team, uint8 race, uint8 plClass, bool apply); // threadsafe
        uint32 GetOnlineTeamPlayers(bool alliance) const { return m_onlineTeams[alliance]; }
        uint32 GetOnlineRacePlayers(uint8 race) const { return m_onlineRaces[race]; }
        uint32 GetOnlineClassPlayers(uint8 plClass) const { return m_onlineClasses[plClass]; }

        /// Get the active session server limit (or security level limitations)
        uint32 GetPlayerAmountLimit() const { return m_playerLimit >= 0 ? m_playerLimit : 0; }
        AccountTypes GetPlayerSecurityLimit() const { return m_playerLimit <= 0 ? AccountTypes(-m_playerLimit) : SEC_PLAYER; }

        /// Set the active session server limit (or security level limitation)
        void SetPlayerLimit(int32 limit, bool needUpdate = false);

        // player Queue
        typedef std::list<WorldSession*> Queue;
        void AddQueuedSession(WorldSession*);
        bool RemoveQueuedSession(WorldSession* sess);
        int32 GetQueuedSessionPos(WorldSession const* sess) const;

        /// \todo Actions on m_allowMovement still to be implemented
        /// Is movement allowed?
        bool getAllowMovement() const { return m_allowMovement; }
        /// Allow/Disallow object movements
        void SetAllowMovement(bool allow) { m_allowMovement = allow; }

        /// Set a new Message of the Day
        void SetMotd(const std::string& motd) { m_motd = motd; }
        /// Get the current Message of the Day
        const char* GetMotd() const { return m_motd.c_str(); }

        LocaleConstant GetDefaultDbcLocale() const { return m_defaultDbcLocale; }

        /// Get the path where data (dbc, maps) are stored on disk
        std::string GetDataPath() const { return m_dataPath; }

        /// When server started?
        time_t const& GetStartTime() const { return m_startTime; }
        /// What time is it?
        time_t const& GetGameTime() const { return m_gameTime; }
        /// Uptime (in secs)
        uint32 GetUptime() const { return uint32(m_gameTime - m_startTime); }
        /// Next daily quests and random bg reset time
        time_t GetNextDailyQuestsResetTime() const { return m_NextDailyQuestReset; }
        time_t GetNextWeeklyQuestsResetTime() const { return m_NextWeeklyQuestReset; }
        time_t GetNextRandomBattlegroundResetTime() const { return m_NextRandomBattlegroundReset; }

        /// Get the maximum skill level a player can reach
        uint16 GetConfigMaxSkillValue() const
        {
            uint32 lvl = getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
            return lvl > 60 ? 300 + ((lvl - 60) * 75) / 10 : lvl * 5;
        }

        void SetInitialWorldSettings();
        void LoadConfigSettings(bool reload = false);
        void LoadSpamRecords(bool reload = false);

        void SendWorldText(int32 string_id, ...);
        void SendWorldTextToAboveSecurity(uint32 securityLevel, int32 string_id, ...);
        void SendWorldTextToAcceptingTickets(int32 string_id, ...);
        void SendGlobalMessage(WorldPacket const& packet, uint32 team = 0) const;
        void SendServerMessage(ServerMessageType type, const char* text = "", Player* player = nullptr) const;
        void SendZoneUnderAttackMessage(uint32 zoneId, Team team);
        void SendDefenseMessage(uint32 zoneId, int32 textId);
        void SendDefenseMessageBroadcastText(uint32 zoneId, uint32 textId);

        /// Are we in the middle of a shutdown?
        bool IsShutdowning() const { return m_ShutdownTimer > 0; }
        void ShutdownServ(uint32 time, uint32 options, uint8 exitcode);
        void ShutdownCancel();
        void ShutdownMsg(bool show = false, Player* player = nullptr);
        static uint8 GetExitCode() { return m_ExitCode; }
        static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }
        static bool IsStopped() { return m_stopEvent; }

        void Update(uint32 diff);

        void UpdateSessions(uint32 diff);

        /// Get a server configuration element (see #eConfigFloatValues)
        void setConfig(eConfigFloatValues index, float value) { m_configFloatValues[index] = value; }
        /// Get a server configuration element (see #eConfigFloatValues)
        float getConfig(eConfigFloatValues rate) const { return m_configFloatValues[rate]; }

        /// Set a server configuration element (see #eConfigUInt32Values)
        void setConfig(eConfigUInt32Values index, uint32 value) { m_configUint32Values[index] = value; }
        /// Get a server configuration element (see #eConfigUInt32Values)
        uint32 getConfig(eConfigUInt32Values index) const { return m_configUint32Values[index]; }

        /// Set a server configuration element (see #eConfigInt32Values)
        void setConfig(eConfigInt32Values index, int32 value) { m_configInt32Values[index] = value; }
        /// Get a server configuration element (see #eConfigInt32Values)
        int32 getConfig(eConfigInt32Values index) const { return m_configInt32Values[index]; }

        /// Set a server configuration element (see #eConfigBoolValues)
        void setConfig(eConfigBoolValues index, bool value) { m_configBoolValues[index] = value; }
        /// Get a server configuration element (see #eConfigBoolValues)
        bool getConfig(eConfigBoolValues index) const { return m_configBoolValues[index]; }

        /// Get configuration about force-loaded maps
        bool isForceLoadMap(uint32 id) const { return m_configForceLoadMapIds.find(id) != m_configForceLoadMapIds.end(); }

        /// Are we on a "Player versus Player" server?
        bool IsPvPRealm() const { return (getConfig(CONFIG_UINT32_GAME_TYPE) == REALM_TYPE_PVP || getConfig(CONFIG_UINT32_GAME_TYPE) == REALM_TYPE_RPPVP || getConfig(CONFIG_UINT32_GAME_TYPE) == REALM_TYPE_FFA_PVP); }
        bool IsFFAPvPRealm() const { return getConfig(CONFIG_UINT32_GAME_TYPE) == REALM_TYPE_FFA_PVP; }

        void KickAll(bool save);
        void KickAllLess(AccountTypes sec);
        void WarnAccount(uint32 accountId, std::string from, std::string reason, const char* type = "WARNING");
        BanReturn BanAccount(BanMode mode, std::string nameOrIP, uint32 duration_secs, std::string reason, const std::string& author);
        BanReturn BanAccount(WorldSession *session, uint32 duration_secs, const std::string& reason, const std::string& author);
        bool RemoveBanAccount(BanMode mode, const std::string& source, const std::string& message, std::string nameOrIP);

        // for max speed access
        static float GetMaxVisibleDistanceOnContinents()    { return m_MaxVisibleDistanceOnContinents; }
        static float GetMaxVisibleDistanceInInstances()     { return m_MaxVisibleDistanceInInstances;  }
        static float GetMaxVisibleDistanceInBGArenas()      { return m_MaxVisibleDistanceInBGArenas;   }

        static float GetRelocationLowerLimitSq() { return m_relocation_lower_limit_sq; }
        static uint32 GetRelocationAINotifyDelay() { return m_relocation_ai_notify_delay; }

        void ProcessCliCommands();
        void QueueCliCommand(const CliCommandHolder* commandHolder) { std::lock_guard<std::mutex> guard(m_cliCommandQueueLock); m_cliCommandQueue.push_back(commandHolder); }

        void UpdateResultQueue();
        void InitResultQueue();

        void UpdateRealmCharCount(uint32 accountId);

        LocaleConstant GetAvailableDbcLocale(LocaleConstant locale) const
        {
            if (m_availableDbcLocaleMask & (1 << locale)) return locale;
            return m_defaultDbcLocale;
        }

        // used World DB version
        void LoadDBVersion();
        char const* GetDBVersion() const { return m_DBVersion.c_str(); }
        char const* GetCreatureEventAIVersion() const { return m_CreatureEventAIVersion.c_str(); }

        std::vector<std::string> GetSpamRecords() const { return m_spamRecords; }

        /**
        * \brief: force all client to request player data
        * \param: ObjectGuid guid : guid of the specified player
        * \returns: void
        *
        * Description: InvalidatePlayerDataToAllClient force all connected clients to clear specified player cache
        * FullName: World::InvalidatePlayerDataToAllClient
        * Access: public
        **/
        void InvalidatePlayerDataToAllClient(ObjectGuid guid) const;

        static uint32 GetCurrentMSTime() { return m_currentMSTime; }
        static TimePoint GetCurrentClockTime() { return m_currentTime; }
        static uint32 GetCurrentDiff() { return m_currentDiff; }
#ifdef ENABLE_PLAYERBOTS
        static uint32 GetAverageDiff() { return m_averageDiff; }
        static uint32 GetMaxDiff() { return m_maxDiff; }
#endif

        template<typename T>
        void ExecuteForAllSessions(T executor) const
        {
            for (auto& data : m_sessions)
                executor(*data.second);
        }

        Messager<World>& GetMessager() { return m_messager; }

        void IncrementOpcodeCounter(uint32 opcodeId); // thread safe due to atomics

        void LoadWorldSafeLocs() const;
        void LoadGraveyardZones();
        GraveyardManager& GetGraveyardManager() { return m_graveyardManager; }

        void SendGMTextFlags(uint32 accountFlag, int32 stringId, std::string type, const char* message);

        LfgRaidBrowser& GetRaidBrowser() { return m_raidBrowser; }
        LFGQueue& GetLFGQueue() { return m_lfgQueue; }

        void BroadcastToGroup(ObjectGuid groupGuid, std::vector<WorldPacket> const& packets);
        void BroadcastPersonalized(std::map<ObjectGuid, std::vector<WorldPacket>> const& personalizedPackets);

        BattleGroundQueue& GetBGQueue() { return m_bgQueue; }
        void StartLFGQueueThread();
        void StartBGQueueThread();
    protected:
        void _UpdateGameTime();
        // callback for UpdateRealmCharacters
        void _UpdateRealmCharCount(QueryResult* resultCharCount, uint32 accountId);

        void InitDailyQuestResetTime();
        void InitWeeklyQuestResetTime();
        void InitRandomBattlegroundResetTime();
        void SetMonthlyQuestResetTime(bool initialize = true);

        void GenerateEventGroupEvents(bool daily, bool weekly, bool deleteColumns);
        void LoadEventGroupChosen();

        void ResetDailyQuests();
        void ResetWeeklyQuests();
        void ResetMonthlyQuests();
        void ResetRandomBattleground();

#ifdef BUILD_METRICS
        void GeneratePacketMetrics(); // thread safe due to atomics
        uint32 GetAverageLatency() const;
#endif

    private:
        void setConfig(eConfigUInt32Values index, char const* fieldname, uint32 defvalue);
        void setConfig(eConfigInt32Values index, char const* fieldname, int32 defvalue);
        void setConfig(eConfigFloatValues index, char const* fieldname, float defvalue);
        void setConfig(eConfigBoolValues index, char const* fieldname, bool defvalue);
        void setConfigPos(eConfigFloatValues index, char const* fieldname, float defvalue);
        void setConfigMin(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue);
        void setConfigMin(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue);
        void setConfigMin(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue);
        void setConfigMinMax(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue, uint32 maxvalue);
        void setConfigMinMax(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue, int32 maxvalue);
        void setConfigMinMax(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue, float maxvalue);
        bool configNoReload(bool reload, eConfigUInt32Values index, char const* fieldname, uint32 defvalue) const;
        bool configNoReload(bool reload, eConfigInt32Values index, char const* fieldname, int32 defvalue) const;
        bool configNoReload(bool reload, eConfigFloatValues index, char const* fieldname, float defvalue) const;
        bool configNoReload(bool reload, eConfigBoolValues index, char const* fieldname, bool defvalue) const;

        static volatile bool m_stopEvent;
        static uint8 m_ExitCode;
        uint32 m_ShutdownTimer;
        uint32 m_ShutdownMask;

        time_t m_startTime;
        time_t m_gameTime;
        IntervalTimer m_timers[WUPDATE_COUNT];
        uint32 mail_timer;
        uint32 mail_timer_expires;

        typedef std::unordered_map<uint32, WorldSession*> SessionMap;
        typedef std::unordered_set<uint32> UniqueSessions;
        SessionMap m_sessions;
        UniqueSessions m_uniqueSessionCount;
        uint32 m_maxActiveSessionCount;
        uint32 m_maxQueuedSessionCount;

        uint32 m_configUint32Values[CONFIG_UINT32_VALUE_COUNT];
        int32 m_configInt32Values[CONFIG_INT32_VALUE_COUNT];
        float m_configFloatValues[CONFIG_FLOAT_VALUE_COUNT];
        bool m_configBoolValues[CONFIG_BOOL_VALUE_COUNT];

        int32 m_playerLimit;
        LocaleConstant m_defaultDbcLocale;                  // from config for one from loaded DBC locales
        uint32 m_availableDbcLocaleMask;                    // by loaded DBC
        void DetectDBCLang();
        bool m_allowMovement;
        std::string m_motd;
        std::string m_dataPath;

        // for max speed access
        static float m_MaxVisibleDistanceOnContinents;
        static float m_MaxVisibleDistanceInInstances;
        static float m_MaxVisibleDistanceInBGArenas;

        static float  m_relocation_lower_limit_sq;
        static uint32 m_relocation_ai_notify_delay;

        // CLI command holder to be thread safe
        std::mutex m_cliCommandQueueLock;
        std::deque<const CliCommandHolder*> m_cliCommandQueue;

        // next daily quests and random BG reset time
        time_t m_NextDailyQuestReset;
        time_t m_NextWeeklyQuestReset;
        time_t m_NextMonthlyQuestReset;
        time_t m_NextRandomBattlegroundReset;

        // Player Queue
        Queue m_QueuedSessions;

        // sessions that are added async
        void AddSession_(WorldSession* s);

        std::mutex m_sessionAddQueueLock;
        std::deque<WorldSession*> m_sessionAddQueue;

        // used versions
        std::string m_DBVersion;
        std::string m_CreatureEventAIVersion;

        // List of Maps that should be force-loaded on startup
        std::set<uint32> m_configForceLoadMapIds;

        // Vector of quests that were chosen for given group
        std::vector<uint32> m_eventGroupChosen;

        std::vector<std::string> m_spamRecords;

        static uint32 m_currentMSTime;
        static TimePoint m_currentTime;
        static uint32 m_currentDiff;
#ifdef ENABLE_PLAYERBOTS
        static uint32 m_currentDiffSum;
        static uint32 m_currentDiffSumIndex;
        static uint32 m_averageDiff;
        static uint32 m_maxDiff;
        static std::list<uint32> m_histDiff;
#endif

        Messager<World> m_messager;

        // Opcode logging
        std::vector<std::atomic<uint32>> m_opcodeCounters;
        // online count logging
        std::array<std::atomic<uint32>, 2> m_onlineTeams;
        std::array<std::atomic<uint32>, MAX_RACES> m_onlineRaces;
        std::array<std::atomic<uint32>, MAX_CLASSES> m_onlineClasses;

        GraveyardManager m_graveyardManager;

        // World is owner to differentiate from Dungeon finder where queue is completely disjoint
        LfgRaidBrowser m_raidBrowser;
        // Housing this here but logically it is completely asynchronous
        LFGQueue m_lfgQueue;
        std::thread m_lfgQueueThread;
        BattleGroundQueue m_bgQueue;
        std::thread m_bgQueueThread;
};

extern uint32 realmID;

#define sWorld MaNGOS::Singleton<World>::Instance()
#endif
/// @}
