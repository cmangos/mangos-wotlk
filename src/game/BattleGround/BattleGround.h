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

#ifndef __BATTLEGROUND_H
#define __BATTLEGROUND_H

#include "Common.h"
#include "Globals/SharedDefines.h"
#include "Maps/Map.h"
#include "Util/ByteBuffer.h"
#include "Entities/ObjectGuid.h"
#include "BattleGround/BattleGroundDefines.h"

// magic event-numbers
#define BG_EVENT_NONE 255
// those generic events should get a high event id
#define BG_EVENT_DOOR 254
// only arena event
// cause this buff apears 90sec after start in every bg i implement it here
#define ARENA_BUFF_EVENT 253
#define ARENA_TIMELIMIT_POINTS_LOSS -16

class Creature;
class GameObject;
class Group;
class Player;
class WorldPacket;
class BattleGroundMap;

struct PvPDifficultyEntry;
struct WorldSafeLocsEntry;

struct BattleGroundEventIdx
{
    uint8 event1;
    uint8 event2;
};

enum BattleGroundSounds
{
    SOUND_HORDE_WINS                = 8454,
    SOUND_ALLIANCE_WINS             = 8455,
    SOUND_BG_START                  = 3439
};

enum BattleGroundQuests
{
    SPELL_WS_QUEST_REWARD_1         = 43483,                // quests 11338 and 11342
    SPELL_WS_QUEST_REWARD_2         = 69158,                // quests 14180 and 14183
    SPELL_WS_QUEST_REWARD_3         = 69496,                // quests 24224 and 24225
    SPELL_WS_QUEST_REWARD_4         = 69497,                // quests 24218 and 24216
    SPELL_WS_QUEST_REWARD_5         = 69498,                // quests 24219 and 24217

    SPELL_AB_QUEST_REWARD_1         = 43484,                // quests 11335 and 11339
    SPELL_AB_QUEST_REWARD_2         = 69153,                // quests 14181 and 14178
    SPELL_AB_QUEST_REWARD_3         = 69499,                // quests 24223 and 24226
    SPELL_AB_QUEST_REWARD_4         = 69500,                // quests 24220 and 24221

    SPELL_AV_QUEST_REWARD_1         = 43475,                // quests 11336 and 11340
    SPELL_AV_QUEST_REWARD_2         = 69160,                // quests 13427 and 13428
    SPELL_AV_QUEST_REWARD_3         = 69501,                // quests 24427 and 24426

    SPELL_EY_QUEST_REWARD_1         = 43477,                // quests 11337 and 11341
    SPELL_EY_QUEST_REWARD_2         = 69156,                // quests 14179 and 14182

    SPELL_SA_QUEST_REWARD           = 61213,                // quests 13405 and 13407

    SPELL_IC_QUEST_REWARD           = 67033,                // quests 14163 and 14164
};

enum BattleGroundMarks
{
    SPELL_WS_MARK_LOSER             = 24950,                // not create marks now
    SPELL_WS_MARK_WINNER            = 24951,                // not create marks now
    SPELL_AB_MARK_LOSER             = 24952,                // not create marks now
    SPELL_AB_MARK_WINNER            = 24953,                // not create marks now
    SPELL_AV_MARK_LOSER             = 24954,                // not create marks now
    SPELL_AV_MARK_WINNER            = 24955,                // not create marks now
    SPELL_SA_MARK_LOSER             = 61159,
    SPELL_SA_MARK_WINNER            = 61160,
};

enum BattleGroundMarksCount
{
    ITEM_WINNER_COUNT               = 3,
    ITEM_LOSER_COUNT                = 1
};

enum ArenaSpells : uint32
{
    SPELL_LAST_MAN_STANDING         = 26549,
    SPELL_ARENA_PREPARATION         = 32727,                // use this one, 32728 not correct
    SPELL_ALLIANCE_GOLD_FLAG        = 32724,
    SPELL_ALLIANCE_GREEN_FLAG       = 32725,
    SPELL_HORDE_GOLD_FLAG           = 35774,
    SPELL_HORDE_GREEN_FLAG          = 35775,
    SPELL_ARENA_DAMPENING           = 74410,                // Arena - Dampening
};

enum BattleGroundSpells : uint32
{
    SPELL_GRAVEYARD_TELEPORT        = 24237,
    SPELL_PREPARATION               = 44521,                // Preparation
    SPELL_RECENTLY_DROPPED_FLAG     = 42792,                // Recently Dropped Flag
    SPELL_AURA_PLAYER_INACTIVE      = 43681,                // Inactive
    SPELL_BATTLEGROUND_DAMPENING    = 74411,                // Battleground - Dampening
};

enum BattleGroundTimeIntervals
{
    CHECK_PLAYER_POSITION_INVERVAL  = 1000,                 // ms
    RESURRECTION_INTERVAL           = 30000,                // ms
    INVITATION_REMIND_TIME          = 20000,                // ms
    INVITE_ACCEPT_WAIT_TIME         = 60000,                // ms
    TIME_TO_AUTOREMOVE              = 120000,               // ms
    MAX_OFFLINE_TIME                = 300,                  // secs
    RESPAWN_ONE_DAY                 = 86400,                // secs
    RESPAWN_IMMEDIATELY             = 0,                    // secs
    BUFF_RESPAWN_TIME               = 180,                  // secs
    ARENA_SPAWN_BUFF_OBJECTS        = 90000,                // ms - 90sec after start
    ARENA_FORCED_DRAW               = 2700000,              // ms - 45min after start
};

enum BattleGroundStartTimeIntervals
{
    BG_START_DELAY_2M               = 120000,               // ms (2 minutes)
    BG_START_DELAY_1M               = 60000,                // ms (1 minute)
    BG_START_DELAY_30S              = 30000,                // ms (30 seconds)
    BG_START_DELAY_15S              = 15000,                // ms (15 seconds) Used only in arena
    BG_START_DELAY_NONE             = 0,                    // ms
};

enum BattleGroundBuffObjects
{
    BG_OBJECTID_SPEEDBUFF_ENTRY     = 179871,
    BG_OBJECTID_REGENBUFF_ENTRY     = 179904,
    BG_OBJECTID_BERSERKERBUFF_ENTRY = 179905
};

enum BattlegroundSpiritHealers
{
    BG_NPC_SPIRIT_GUIDE_ALLIANCE    = 13116,
    BG_NPC_SPIRIT_GUIDE_HORDE       = 13117,
};

enum BattlegroundHonorableDefenders
{
    BG_NPC_HON_DEFENDER_TRIGGER_A    = 35379,               // has aura 66157
    BG_NPC_HON_DEFENDER_TRIGGER_H    = 35380,
    BG_NPC_HON_DEFENDER_TRIGGER_25_A = 36349,               // has aura 68652
    BG_NPC_HON_DEFENDER_TRIGGER_25_H = 36350,
};

enum BattleGroundRandomRewards
{
    BG_REWARD_WINNER_HONOR_FIRST     = 30,
    BG_REWARD_WINNER_ARENA_FIRST     = 25,
    BG_REWARD_WINNER_HONOR_LAST      = 15,
    BG_REWARD_WINNER_ARENA_LAST      = 0,
    BG_REWARD_LOOSER_HONOR_FIRST     = 5,
    BG_REWARD_LOOSER_HONOR_LAST      = 5
};

const uint32 Buff_Entries[3] = { BG_OBJECTID_SPEEDBUFF_ENTRY, BG_OBJECTID_REGENBUFF_ENTRY, BG_OBJECTID_BERSERKERBUFF_ENTRY };

enum BattleGroundStatus
{
    STATUS_NONE         = 0,                                // first status, should mean bg is not instance
    STATUS_WAIT_QUEUE   = 1,                                // means bg is empty and waiting for queue
    STATUS_WAIT_JOIN    = 2,                                // this means, that BG has already started and it is waiting for more players
    STATUS_IN_PROGRESS  = 3,                                // means bg is running
    STATUS_WAIT_LEAVE   = 4                                 // means some faction has won BG and it is ending
};

struct BattleGroundPlayer
{
    time_t  offlineRemoveTime;                              // for tracking and removing offline players from queue after 5 minutes
    Team    playerTeam;                                     // Player's team
};

struct BattleGroundObjectInfo
{
    BattleGroundObjectInfo() : object(nullptr), timer(0), spellid(0) {}

    GameObject*  object;
    int32       timer;
    uint32      spellid;
};

// handle the queue types and bg types separately to enable joining queue for different sized arenas at the same time
enum BattleGroundQueueTypeId
{
    BATTLEGROUND_QUEUE_NONE     = 0,
    BATTLEGROUND_QUEUE_AV       = 1,
    BATTLEGROUND_QUEUE_WS       = 2,
    BATTLEGROUND_QUEUE_AB       = 3,
    BATTLEGROUND_QUEUE_EY       = 4,
    BATTLEGROUND_QUEUE_SA       = 5,
    BATTLEGROUND_QUEUE_IC       = 6,
    BATTLEGROUND_QUEUE_RB       = 7,
    BATTLEGROUND_QUEUE_2v2      = 8,
    BATTLEGROUND_QUEUE_3v3      = 9,
    BATTLEGROUND_QUEUE_5v5      = 10,

    MAX_BATTLEGROUND_QUEUE_TYPES = 11
};

enum ScoreType
{
    SCORE_KILLING_BLOWS         = 1,
    SCORE_DEATHS                = 2,
    SCORE_HONORABLE_KILLS       = 3,
    SCORE_BONUS_HONOR           = 4,
    // EY, but in MSG_PVP_LOG_DATA opcode!
    SCORE_DAMAGE_DONE           = 5,
    SCORE_HEALING_DONE          = 6,
    // WS
    SCORE_FLAG_CAPTURES         = 7,
    SCORE_FLAG_RETURNS          = 8,
    // AB and IC
    SCORE_BASES_ASSAULTED       = 9,
    SCORE_BASES_DEFENDED        = 10,
    // AV
    SCORE_GRAVEYARDS_ASSAULTED  = 11,
    SCORE_GRAVEYARDS_DEFENDED   = 12,
    SCORE_TOWERS_ASSAULTED      = 13,
    SCORE_TOWERS_DEFENDED       = 14,
    SCORE_SECONDARY_OBJECTIVES  = 15,
    // SA
    SCORE_GATES_DESTROYED       = 16,
    SCORE_DEMOLISHERS_DESTROYED = 17,
};

enum BattleGroundType
{
    TYPE_BATTLEGROUND     = 3,
    TYPE_ARENA            = 4
};

enum BattleGroundStartingEvents
{
    BG_STARTING_EVENT_NONE  = 0x00,
    BG_STARTING_EVENT_1     = 0x01,
    BG_STARTING_EVENT_2     = 0x02,
    BG_STARTING_EVENT_3     = 0x04,
    BG_STARTING_EVENT_4     = 0x08
};

enum BattleGroundStartingEventsIds
{
    BG_STARTING_EVENT_FIRST     = 0,
    BG_STARTING_EVENT_SECOND    = 1,
    BG_STARTING_EVENT_THIRD     = 2,
    BG_STARTING_EVENT_FOURTH    = 3
};
#define BG_STARTING_EVENT_COUNT 4

enum BattleGroundGroupJoinStatus
{
    // positive values are indexes in BattlemasterList.dbc
    BG_GROUP_JOIN_STATUS_BATTLEGROUND_FAIL          = 0,            // Your group has joined a battleground queue, but you are not eligible (showed for non existing BattlemasterList.dbc indexes)
    BG_GROUP_JOIN_STATUS_NOT_ELIGIBLE               = -1,           // not show anything
    BG_GROUP_JOIN_STATUS_DESERTERS                  = -2,           // You cannot join the battleground yet because you or one of your party members is flagged as a Deserter.
    BG_GROUP_JOIN_STATUS_NOT_IN_TEAM                = -3,           // Incorrect party size for this arena.
    BG_GROUP_JOIN_STATUS_TOO_MANY_QUEUES            = -4,           // You can only be queued for 2 battles at once
    BG_GROUP_JOIN_STATUS_CANNOT_QUEUE_FOR_RATED     = -5,           // You cannot queue for a rated match while queued for other battles
    BG_GROUP_JOIN_STATUS_QUEUED_FOR_RATED           = -6,           // You cannot queue for another battle while queued for a rated arena match
    BG_GROUP_JOIN_STATUS_TEAM_LEFT_QUEUE            = -7,           // Your team has left the arena queue
    BG_GROUP_JOIN_STATUS_NOT_IN_BATTLEGROUND        = -8,           // You can't do that in a battleground.
    BG_GROUP_JOIN_STATUS_XP_GAIN                    = -9,           // wtf, doesn't exist in client...
    BG_GROUP_JOIN_STATUS_JOIN_RANGE_INDEX           = -10,          // Cannot join the queue unless all members of your party are in the same battleground level range.
    BG_GROUP_JOIN_STATUS_JOIN_TIMED_OUT             = -11,          // %s was unavailable to join the queue. (uint64 guid exist in client cache)
    BG_GROUP_JOIN_STATUS_JOIN_FAILED                = -12,          // Join as a group failed (uint64 guid doesn't exist in client cache)
    BG_GROUP_JOIN_STATUS_LFG_CANT_USE_BATTLEGROUND  = -13,          // You cannot queue for a battleground or arena while using the dungeon system.
    BG_GROUP_JOIN_STATUS_IN_RANDOM_BG               = -14,          // Can't do that while in a Random Battleground queue.
    BG_GROUP_JOIN_STATUS_IN_NON_RANDOM_BG           = -15,          // Can't queue for Random Battleground while in another Battleground queue.
};

/*
 This class is used to keep the battleground score for each individual player
*/
class BattleGroundScore
{
    public:
        BattleGroundScore() : killingBlows(0), deaths(0), honorableKills(0),
            bonusHonor(0), damageDone(0), healingDone(0), Team(ALLIANCE)
        {}
        virtual ~BattleGroundScore() {}                     // virtual destructor is used when deleting score from scores map

        uint32 GetKillingBlows() const      { return killingBlows; }
        uint32 GetDeaths() const            { return deaths; }
        uint32 GetHonorableKills() const    { return honorableKills; }
        uint32 GetBonusHonor() const        { return bonusHonor; }
        uint32 GetDamageDone() const        { return damageDone; }
        uint32 GetHealingDone() const       { return healingDone; }

        virtual uint32 GetAttr1() const     { return 0; }
        virtual uint32 GetAttr2() const     { return 0; }
        virtual uint32 GetAttr3() const     { return 0; }
        virtual uint32 GetAttr4() const     { return 0; }
        virtual uint32 GetAttr5() const     { return 0; }

        uint32 killingBlows;
        uint32 deaths;
        uint32 honorableKills;
        uint32 bonusHonor;
        uint32 damageDone;
        uint32 healingDone;

        uint32 Team;
};

/*
This class is used to:
1. Add player to battleground
2. Remove player from battleground
3. some certain cases, same for all battlegrounds
4. It has properties same for all battlegrounds
*/
class BattleGround
{
        friend class BattleGroundMgr;

    public:
        /* Construction */
        BattleGround();
        virtual ~BattleGround();

        // main battleground functions
        virtual void Update(uint32 diff);                   // must be implemented in BG subclass of BG specific update code, but must in begginning call parent version
        virtual void Reset();                               // resets all common properties for battlegrounds, must be implemented and called in BG subclass

        // handle the opening and closing door events
        virtual void StartingEventCloseDoors() {}
        virtual void StartingEventOpenDoors() {}

        /* Battleground */
        // Get methods:
        char const* GetName() const         { return m_name; }
        BattleGroundTypeId GetTypeId(bool getRandom = false) const { return getRandom ? m_randomTypeId : m_typeId; }
        BattleGroundBracketId GetBracketId() const { return m_bracketId; }
        // the instanceId check is also used to determine a bg-template
        // that's why the m_map hack is here..
        uint32 GetInstanceId() const        { return m_bgMap ? GetBgMap()->GetInstanceId() : 0; }
        BattleGroundStatus GetStatus() const { return m_status; }
        uint32 GetClientInstanceId() const  { return m_clientInstanceId; }
        uint32 GetStartTime() const         { return m_startTime; }
        uint32 GetEndTime() const           { return m_endTime; }
        uint32 GetMaxPlayers() const        { return m_maxPlayers; }
        uint32 GetMinPlayers() const        { return m_minPlayers; }

        uint32 GetMinLevel() const          { return m_levelMin; }
        uint32 GetMaxLevel() const          { return m_levelMax; }

        uint32 GetMaxPlayersPerTeam() const { return m_maxPlayersPerTeam; }
        uint32 GetMinPlayersPerTeam() const { return m_minPlayersPerTeam; }

        int32 GetStartDelayTime() const     { return m_startDelayTime; }
        ArenaType GetArenaType() const      { return m_arenaType; }
        BattleGroundWinner GetWinner() const{ return m_winner; }
        bool IsRandom() const               { return m_isRandom; }
        uint32 GetBattlemasterEntry() const;
        uint32 GetBonusHonorFromKill(uint32 kills) const;

        // Set methods:
        void SetName(char const* name)      { m_name = name; }
        void SetTypeId(BattleGroundTypeId typeId) { m_typeId = typeId; }
        void SetRandomTypeId(BattleGroundTypeId typeId) { m_randomTypeId = typeId; }
        // here we can count minlevel and maxlevel for players
        void SetBracket(PvPDifficultyEntry const* bracketEntry);
        void SetStatus(BattleGroundStatus status) { m_status = status; }
        void SetClientInstanceId(uint32 instanceId) { m_clientInstanceId = instanceId; }
        void SetStartTime(uint32 time)      { m_startTime = time; }
        void SetEndTime(uint32 time)        { m_endTime = time; }
        void SetMaxPlayers(uint32 maxPlayers) { m_maxPlayers = maxPlayers; }
        void SetMinPlayers(uint32 minPlayers) { m_minPlayers = minPlayers; }
        void SetLevelRange(uint32 min, uint32 max) { m_levelMin = min; m_levelMax = max; }
        void SetRated(bool state)           { m_isRated = state; }
        void SetArenaType(ArenaType type)   { m_arenaType = type; }
        void SetArenaorBGType(bool isArena) { m_isArena = isArena; }
        void SetWinner(BattleGroundWinner winner) { m_winner = winner; }
        void SetRandom(bool isRandom)       { m_isRandom = isRandom; }

        void ModifyStartDelayTime(int diff) { m_startDelayTime -= diff; }
        void SetStartDelayTime(int time)    { m_startDelayTime = time; }

        void SetMaxPlayersPerTeam(uint32 maxPlayers) { m_maxPlayersPerTeam = maxPlayers; }
        void SetMinPlayersPerTeam(uint32 minPlayers) { m_minPlayersPerTeam = minPlayers; }

        void AddToBgFreeSlotQueue();                        // this queue will be useful when more battlegrounds instances will be available
        void RemoveFromBgFreeSlotQueue();                   // this method could delete whole BG instance, if another free is available

        // Functions to decrease or increase player count
        void DecreaseInvitedCount(Team team)      { (team == ALLIANCE) ? --m_invitedAlliance : --m_invitedHorde; }
        void IncreaseInvitedCount(Team team)      { (team == ALLIANCE) ? ++m_invitedAlliance : ++m_invitedHorde; }
        uint32 GetInvitedCount(Team team) const
        {
            if (team == ALLIANCE)
                return m_invitedAlliance;
            return m_invitedHorde;
        }
        bool HasFreeSlots() const { return GetPlayersSize() < GetMaxPlayers(); }
        uint32 GetFreeSlotsForTeam(Team team) const;

        // Functions that return if battleground is arena or if it's rated
        bool IsArena() const        { return m_isArena; }
        bool IsBattleGround() const { return !m_isArena; }
        bool IsRated() const        { return m_isRated; }

        // Functions that handle battleground players
        typedef std::map<ObjectGuid, BattleGroundPlayer> BattleGroundPlayerMap;
        BattleGroundPlayerMap const& GetPlayers() const { return m_players; }
        uint32 GetPlayersSize() const { return m_players.size(); }

        // Functions that handle battleground scores
        typedef std::map<ObjectGuid, BattleGroundScore*> BattleGroundScoreMap;
        BattleGroundScoreMap::const_iterator GetPlayerScoresBegin() const { return m_playerScores.begin(); }
        BattleGroundScoreMap::const_iterator GetPlayerScoresEnd() const { return m_playerScores.end(); }
        uint32 GetPlayerScoresSize() const { return m_playerScores.size(); }

        // Function which starts the battleground
        void StartBattleGround();

        // Functions that handle gameobject storage
        typedef std::map<uint32, ObjectGuid> EntryGuidMap;
        GameObject* GetSingleGameObjectFromStorage(uint32 entry) const;
        Creature* GetSingleCreatureFromStorage(uint32 entry, bool skipDebugLog = false) const;

#ifdef ENABLE_PLAYERBOTS
        uint32 GetSingleGameObjectGuid(uint8 event1, uint8 event2);
#endif

        // Function that set and get battleground map id
        void SetMapId(uint32 mapId) { m_mapId = mapId; }
        uint32 GetMapId() const { return m_mapId; }

        // Functions that set and get battleground map
        void SetBgMap(BattleGroundMap* map) { m_bgMap = map; }
        BattleGroundMap* GetBgMap() const
        {
            MANGOS_ASSERT(m_bgMap);
            return m_bgMap;
        }

        // Functions that get and set team start location
        void SetTeamStartLoc(Team team, float x, float y, float z, float o);
        void GetTeamStartLoc(Team team, float& x, float& y, float& z, float& o) const
        {
            PvpTeamIndex idx = GetTeamIndexByTeamId(team);
            x = m_teamStartLocX[idx];
            y = m_teamStartLocY[idx];
            z = m_teamStartLocZ[idx];
            o = m_teamStartLocO[idx];
        }

        void SetStartMaxDist(float startMaxDist) { m_startMaxDist = startMaxDist; }
        float GetStartMaxDist() const { return m_startMaxDist; }

        /* Packet Transfer */
        // method that should fill worldpacket with actual world states (not yet implemented for all battlegrounds!)
        virtual void FillInitialWorldStates(WorldPacket& /*data*/, uint32& /*count*/) {}
        void SendPacketToTeam(Team teamId, WorldPacket const& packet, Player* sender = nullptr, bool self = true);
        void SendPacketToAll(WorldPacket const& packet);

        template<class Do>
        void BroadcastWorker(Do& _do);

        // Functions that handle sending sounds
        void PlaySoundToTeam(uint32 /*soundId*/, Team /*teamId*/);
        void PlaySoundToAll(uint32 /*soundId*/);

        // Function that handle casting spells on team
        void CastSpellOnTeam(uint32 /*spellId*/, Team /*teamId*/);

        // Function that rewards honor to the team
        void RewardHonorToTeam(uint32 /*honor*/, Team /*teamId*/);

        // Function that rewards reputation to the team
        void RewardReputationToTeam(uint32 /*factionId*/, uint32 /*reputation*/, Team /*teamId*/);

        // Function that rewards PvP marks
        void RewardMark(Player* /*player*/, uint32 /*count*/);

        // Function that rewards PvP marks by mail
        void SendRewardMarkByMail(Player* /*player*/, uint32 /*mark*/, uint32 /*count*/) const;

        // Function that rewards items
        void RewardItem(Player* /*player*/, uint32 /*itemId*/, uint32 /*count*/);

        // Function that rewards quest completion
        void RewardQuestComplete(Player* /*player*/);

        // Function that rewards spell id
        void RewardSpellCast(Player* /*player*/, uint32 /*spellId*/) const;

        // Function that updates world states for all players
        void UpdateWorldState(uint32 /*field*/, uint32 /*value*/);

        // Function that updates world state for a player
        void UpdateWorldStateForPlayer(uint32 /*field*/, uint32 /*value*/, Player* /*player*/) const;

        // Function that ends battleground
        virtual void EndBattleGround(Team /*winner*/);

        // Function that blocks movement at the end of the battleground
        static void BlockMovement(Player* /*player*/);

        // Functions that handle battleground announcements; Used usually when players capture battleground objectives
        void SendMessageToAll(int32 /*entry*/, ChatMsg /*type*/, Player const* source = nullptr);
        void SendMessageToAll(int32 /*entry*/, ChatMsg /*type*/, uint32 /*language*/, ObjectGuid /*guid*/);
        void PSendMessageToAll(int32 /*entry*/, ChatMsg /*type*/, Player const* /*source*/, ...);
        void SendMessage2ToAll(int32 /*entry*/, ChatMsg /*type*/, Player const* /*source*/, int32 arg1 = 0, int32 arg2 = 0);

        // Functions that handle creature yells in battleground; Used specifically in Alterac Valley
        void SendYellToAll(int32 /*entry*/, uint32 /*language*/, Creature const* /*source*/);
        void SendYell2ToAll(int32 /*entry*/, uint32 /*language*/, Creature const* /*source*/, int32 /*arg1*/, int32 /*arg2*/);

        void SendBcdToAll(int32 bcdEntry, ChatMsg msgtype, Creature const* source);
        void SendBcdToTeam(int32 bcdEntry, ChatMsg msgtype, Creature const* source, Team team);

        // Handle raid groups
        Group* GetBgRaid(Team team) const { return m_bgRaids[GetTeamIndexByTeamId(team)]; }
        void SetBgRaid(Team /*team*/, Group* /*bgRaid*/);

        // Handle player score update
        virtual void UpdatePlayerScore(Player* /*player*/, uint32 /*type*/, uint32 /*value*/);

        uint32 GetPlayersCountByTeam(Team team) const { return m_playersCount[GetTeamIndexByTeamId(team)]; }
        uint32 GetAlivePlayersCountByTeam(Team /*team*/) const;             // used in arenas to correctly handle death in spirit of redemption / last stand etc. (killer = killed) cases
        void UpdatePlayersCountByTeam(Team team, bool remove)
        {
            if (remove)
                --m_playersCount[GetTeamIndexByTeamId(team)];
            else
                ++m_playersCount[GetTeamIndexByTeamId(team)];
        }

        // Arena specific functions
        void SetArenaTeamIdForTeam(Team team, uint32 arenaTeamId) { m_arenaTeamIds[GetTeamIndexByTeamId(team)] = arenaTeamId; }
        uint32 GetArenaTeamIdForTeam(Team team) const             { return m_arenaTeamIds[GetTeamIndexByTeamId(team)]; }
        void SetArenaTeamRatingChangeForTeam(Team team, int32 ratingChange) { m_arenaTeamRatingChanges[GetTeamIndexByTeamId(team)] = ratingChange; }
        int32 GetArenaTeamRatingChangeForTeam(Team team) const    { return m_arenaTeamRatingChanges[GetTeamIndexByTeamId(team)]; }
        void CheckArenaWinConditions();

        /* Battleground Events */

        // handle player in area trigger
        virtual bool HandleAreaTrigger(Player* /*player*/, uint32 /*triggerId*/) { return false;  }

        // handle player killed; also supports generic player kill logic for arenas
        virtual void HandleKillPlayer(Player* /*player*/, Player* /*killer*/);

        // handle creature killed
        virtual void HandleKillUnit(Creature* /*unit*/, Player* /*killer*/) {}

        // handle event sent from gameobjects
        virtual bool HandleEvent(uint32 /*eventId*/, Object* /*source*/, Object* /*target*/) { return false; }

        // Called when a creature is created
        virtual void HandleCreatureCreate(Creature* /*creature*/) {}

        // Called when a gameobject is created
        virtual void HandleGameObjectCreate(GameObject* /*go*/) {}

        // Called when player drops flag
        virtual void HandlePlayerDroppedFlag(Player* /*player*/) {}

        // Called when player clicks on flag
        virtual void HandlePlayerClickedOnFlag(Player* /*player*/, GameObject* /*target_obj*/) {}

        // Called when a gameobject type 33 is damaged
        virtual void HandleGameObjectDamaged(Player* /*player*/, GameObject* /*object*/, uint32 /*spellId*/) {}

        // Called when player logs in / out
        void EventPlayerLoggedIn(Player* /*player*/);
        void EventPlayerLoggedOut(Player* /*player*/);

        // Called when player joins the battleground after the initial load
        virtual void AddPlayer(Player* /*player*/);                     // must be implemented in BG subclass

        void AddOrSetPlayerToCorrectBgGroup(Player* /*player*/, ObjectGuid /*playerGuid*/, Team /*team*/);

        // Called when player leaves battleground
        virtual void RemovePlayerAtLeave(ObjectGuid /*playerGuid*/, bool /*isOnTransport*/, bool /*doSendPacket*/);

        /* Battleground Events */
        // called when a creature gets added to map (NOTE: only triggered if
        // a player activates the cell of the creature)
        void OnObjectDBLoad(Creature* /*creature*/);
        void OnObjectDBLoad(GameObject* /*obj*/);

        // (de-)spawns creatures and gameobjects from an event
        void SpawnEvent(uint8 /*event1*/, uint8 /*event2*/, bool /*spawn*/);
        bool IsActiveEvent(uint8 event1, uint8 event2)
        {
            if (m_activeEvents.find(event1) == m_activeEvents.end())
                return false;

            return m_activeEvents[event1] == event2;
        }

        // Get creature guid from event
        uint32 GetSingleCreatureGuid(uint8 /*event1*/, uint8 /*event2*/);

        // Handle door events
        void OpenDoorEvent(uint8 /*event1*/, uint8 event2 = 0);
        bool IsDoorEvent(uint8 /*event1*/, uint8 /*event2*/) const;

        // Handle battleground buffs (traps) that give players specific benefits
        void HandleTriggerBuff(ObjectGuid /*go_guid*/);

        // Handle the respawn or despawn of creatures or gameobjects
        void ChangeBgObjectSpawnState(uint32 dbGuid, uint32 /*respawntime*/);
        void ChangeBgCreatureSpawnState(uint32 dbGuid, uint32 /*respawntime*/);

        // Handle door states
        void DoorOpen(uint32 dbGuid);
        void DoorClose(uint32 dbGuid);

        // Get match premature winner
        virtual Team GetPrematureWinner();

        // handle players that fall under the map
        virtual bool HandlePlayerUnderMap(Player* /*player*/) { return false; }

        // since arenas can be AvA or Hvh, we have to get the "temporary" team of a player
        Team GetPlayerTeam(ObjectGuid /*playerGuid*/);

        // returns the other team
        static Team GetOtherTeam(Team team) { return (team == ALLIANCE || team == HORDE) ? ((team == ALLIANCE) ? HORDE : ALLIANCE) : TEAM_NONE; }

        // returns the other team index
        static PvpTeamIndex GetOtherTeamIndex(PvpTeamIndex teamIdx) { return teamIdx == TEAM_INDEX_ALLIANCE ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE; }

        // check if player is inside battleground
        bool IsPlayerInBattleGround(ObjectGuid /*playerGuid*/);

        // Handle script condition fulfillment
        virtual bool IsConditionFulfilled(Player const* /*source*/, uint32 /*conditionId*/, WorldObject const* /*conditionSource*/, uint32 /*conditionSourceType*/) { return false; }

        // Handle achievement criteria requirements
        virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/) { return false; }

        // function that start timed achievement
        void StartTimedAchievement(AchievementCriteriaTypes /*type*/, uint32 /*entry*/);

        struct EventObjects
        {
            std::vector<uint32> gameobjects;
            std::vector<uint32> creatures;
        };

        // cause we create it dynamicly i use a map - to avoid resizing when
        // using vector - also it contains 2*events concatenated with PAIR32
        // this is needed to avoid overhead of a 2dimensional std::map
        std::map<uint32, EventObjects> m_eventObjects;
        // this must be filled first in BattleGroundXY::Reset().. else
        // creatures will get added wrong
        // door-events are automaticly added - but _ALL_ other must be in this vector
        std::map<uint8, uint8> m_activeEvents;

        uint32 GetPlayerSkinRefLootId() const { return m_playerSkinReflootId; }
        void SetPlayerSkinRefLootId(uint32 reflootId) { m_playerSkinReflootId = reflootId; }

        virtual void AlterTeleportLocation(Player* player, ObjectGuid& transportGuid, float& x, float& y, float& z, float& ori) {}

        MaNGOS::unique_weak_ptr<BattleGround> GetWeakPtr() const { return m_weakRef; }
        void SetWeakPtr(MaNGOS::unique_weak_ptr<BattleGround> weakRef) { m_weakRef = std::move(weakRef); }

    protected:
        // this method is called, when BG cannot spawn its own spirit guide, or something is wrong, It correctly ends BattleGround
        void EndNow();
        void PlayerAddedToBgCheckIfBgIsRunning(Player* /*player*/);

        /* Scorekeeping */
        BattleGroundScoreMap m_playerScores;                // Player scores

        // can be implemented in BG subclass - player can be nullptr
        virtual void RemovePlayer(Player* /*player*/, ObjectGuid /*guid*/);

        /* Player lists, those need to be accessible by inherited classes */
        BattleGroundPlayerMap  m_players;

        /*
        these are important variables used for starting messages
        */
        uint8 m_events;
        BattleGroundStartTimeIntervals  m_startDelayTimes[BG_STARTING_EVENT_COUNT];
        // this must be filled in constructors!
        uint32 m_startMessageIds[BG_STARTING_EVENT_COUNT];

        bool   m_buffChange;
        bool   m_isRandom;

        /* Storage lists */
        EntryGuidMap m_goEntryGuidStore;                   // Store unique GO-Guids by entry
        EntryGuidMap m_npcEntryGuidStore;                  // Store unique NPC-Guids by entry

    private:
        /* Battleground */
        BattleGroundTypeId m_typeId;
        BattleGroundTypeId m_randomTypeId;
        BattleGroundStatus m_status;
        BattleGroundBracketId m_bracketId;
        ArenaType  m_arenaType;                             // 2=2v2, 3=3v3, 5=5v5
        BattleGroundWinner m_winner;

        uint32 m_clientInstanceId;                          // the instance-id which is sent to the client and without any other internal use
        uint32 m_startTime;
        uint32 m_validStartPositionTimer;
        uint32 m_prematureCountDownTimer;
        int32 m_endTime;                                    // it is set to 120000 when bg is ending and it decreases itself
        int32  m_startDelayTime;

        bool m_arenaBuffSpawned;                            // to cache if arenabuff event is started (cause bool is faster than checking IsActiveEvent)
        bool m_hasBgFreeSlotQueue;                          // used to make sure that BG is only once inserted into the BattleGroundMgr.BGFreeSlotQueue[bgTypeId] deque
        bool m_isRated;                                     // is this battle rated?
        bool m_prematureCountDown;
        bool m_isArena;

        char const* m_name;

        /* Player lists */
        typedef std::deque<ObjectGuid> OfflineQueue;
        OfflineQueue m_offlineQueue;                        // Player GUID

        /* Invited counters are useful for player invitation to BG - do not allow, if BG is started to one faction to have 2 more players than another faction */
        /* Invited counters will be changed only when removing already invited player from queue, removing player from battleground and inviting player to BG */
        /* Invited players counters*/
        uint32 m_invitedAlliance;
        uint32 m_invitedHorde;

        /* Raid Group */
        Group* m_bgRaids[PVP_TEAM_COUNT];                   // 0 - alliance, 1 - horde

        /* Players count by team */
        uint32 m_playersCount[PVP_TEAM_COUNT];

        /* Arena team ids by team */
        uint32 m_arenaTeamIds[PVP_TEAM_COUNT];

        int32 m_arenaTeamRatingChanges[PVP_TEAM_COUNT];

        /* Limits */
        uint32 m_levelMin;
        uint32 m_levelMax;
        uint32 m_maxPlayersPerTeam;
        uint32 m_maxPlayers;
        uint32 m_minPlayersPerTeam;
        uint32 m_minPlayers;

        /* Start location */
        uint32 m_mapId;
        BattleGroundMap* m_bgMap;
        float m_teamStartLocX[PVP_TEAM_COUNT];
        float m_teamStartLocY[PVP_TEAM_COUNT];
        float m_teamStartLocZ[PVP_TEAM_COUNT];
        float m_teamStartLocO[PVP_TEAM_COUNT];
        float m_startMaxDist;

        uint32 m_playerSkinReflootId;

        MaNGOS::unique_weak_ptr<BattleGround> m_weakRef;
};

// helper functions for world state list fill
inline void FillInitialWorldState(ByteBuffer& data, uint32& count, uint32 state, uint32 value)
{
    data << uint32(state);
    data << uint32(value);
    ++count;
}

inline void FillInitialWorldState(ByteBuffer& data, uint32& count, uint32 state, int32 value)
{
    data << uint32(state);
    data << int32(value);
    ++count;
}

inline void FillInitialWorldState(ByteBuffer& data, uint32& count, uint32 state, bool value)
{
    data << uint32(state);
    data << uint32(value ? 1 : 0);
    ++count;
}

struct WorldStatePair
{
    uint32 state;
    uint32 value;
};

inline void FillInitialWorldState(ByteBuffer& data, uint32& count, WorldStatePair const* array)
{
    for (WorldStatePair const* itr = array; itr->state; ++itr)
    {
        data << uint32(itr->state);
        data << uint32(itr->value);
        ++count;
    }
}

#endif
