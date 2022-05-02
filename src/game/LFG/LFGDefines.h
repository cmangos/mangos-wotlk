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

#ifndef _LFG_DEFINES_H
#define _LFG_DEFINES_H

#include "Common.h"
#include "Entities/ObjectGuid.h"

class Quest;

enum LfgFlags
{
    LFG_FLAG_XREALM                              = 0x1,
    LFG_FLAG_GLOBAL                              = 0x2,
    LFG_FLAG_HOLIDAY                             = 0x4,
    LFG_FLAG_SPECIFIC_REWARD                     = 0x8
};

enum LfgType
{
    LFG_TYPE_NONE                 = 0,
    LFG_TYPE_DUNGEON              = 1,
    LFG_TYPE_RAID                 = 2,
    LFG_TYPE_QUEST                = 3,
    LFG_TYPE_ZONE                 = 4,
    LFG_TYPE_HEROIC_DUNGEON       = 5,
    LFG_TYPE_RANDOM_DUNGEON       = 6
};

enum LfgProposalState
{
    LFG_PROPOSAL_INITIATING  = 0,
    LFG_PROPOSAL_FAILED      = 1,
    LFG_PROPOSAL_SUCCESS     = 2
};

enum LfgAnswer
{
    LFG_ANSWER_PENDING  = -1,
    LFG_ANSWER_DENY     = 0,
    LFG_ANSWER_AGREE    = 1
};

// Teleport errors
enum LfgTeleportError
{
    // 7 = "You can't do that right now" | 5 = No client reaction
    LFG_TELEPORTERROR_OK                         = 0,      // Internal use
    LFG_TELEPORTERROR_PLAYER_DEAD                = 1,
    LFG_TELEPORTERROR_FALLING                    = 2,
    LFG_TELEPORTERROR_IN_VEHICLE                 = 3,
    LFG_TELEPORTERROR_FATIGUE                    = 4,
    LFG_TELEPORTERROR_INVALID_LOCATION           = 6,
    LFG_TELEPORTERROR_CHARMING                   = 8       // FIXME - It can be 7 or 8 (Need proper data)
};

/// Instance lock types
enum LfgLockStatusType
{
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION        = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL                 = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL                = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE            = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE           = 5,
    LFG_LOCKSTATUS_RAID_LOCKED                   = 6,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL      = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL     = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED           = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM                  = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON                 = 1031,
    LFG_LOCKSTATUS_MISSING_ACHIEVEMENT           = 1034
};

enum LfgRoles
{
    PLAYER_ROLE_NONE                             = 0x00,
    PLAYER_ROLE_LEADER                           = 0x01,
    PLAYER_ROLE_TANK                             = 0x02,
    PLAYER_ROLE_HEALER                           = 0x04,
    PLAYER_ROLE_DAMAGE                           = 0x08,
    PLAYER_ROLE_ANY                              = PLAYER_ROLE_LEADER | PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE
};

enum LfgState
{
    LFG_STATE_NONE,                                        // Not using LFG / LFR
    LFG_STATE_ROLECHECK,                                   // Rolecheck active
    LFG_STATE_QUEUED,                                      // Queued
    LFG_STATE_PROPOSAL,                                    // Proposal active
    //LFG_STATE_BOOT,                                      // Vote kick active
    LFG_STATE_DUNGEON = 5,                                 // In LFG Group, in a Dungeon
    LFG_STATE_FINISHED_DUNGEON,                            // In LFG Group, in a finished Dungeon
    LFG_STATE_RAIDBROWSER,                                 // Using Raid finder
    LFG_STATE_RAIDBROWSER_ROLECHECK,                       // Using Raid finder and Rolecheck active
    LFG_STATE_FAILED,                                      // Deletes queue entry on next queue update
};

enum LfgSpells
{
    LFG_SPELL_DUNGEON_COOLDOWN = 71328,
    LFG_SPELL_DUNGEON_DESERTER = 71041,
    LFG_SPELL_LUCK_OF_THE_DRAW = 72221,
};

enum LfgTimers
{
    LFG_TIME_ROLECHECK  = 45,
    LFG_TIME_BOOT       = 120,
    LFG_TIME_PROPOSAL   = 45,
};

// Role check states
enum LfgRoleCheckState
{
    LFG_ROLECHECK_DEFAULT                        = 0,      // Internal use = Not initialized.
    LFG_ROLECHECK_FINISHED                       = 1,      // Role check finished
    LFG_ROLECHECK_INITIALITING                   = 2,      // Role check begins
    LFG_ROLECHECK_MISSING_ROLE                   = 3,      // Someone didn't selected a role after 2 mins
    LFG_ROLECHECK_WRONG_ROLES                    = 4,      // Can't form a group with that role selection
    LFG_ROLECHECK_ABORTED                        = 5,      // Someone leave the group
    LFG_ROLECHECK_NO_ROLE                        = 6       // Someone selected no role
};

// Queue join results
enum LfgJoinResult
{
    // 3 = No client reaction | 18 = "Rolecheck failed"
    LFG_JOIN_OK                                  = 0,      // Joined (no client msg)
    LFG_JOIN_FAILED                              = 1,      // RoleCheck Failed
    LFG_JOIN_GROUPFULL                           = 2,      // Your group is full
    LFG_JOIN_INTERNAL_ERROR                      = 4,      // Internal LFG Error
    LFG_JOIN_NOT_MEET_REQS                       = 5,      // You do not meet the requirements for the chosen dungeons
    LFG_JOIN_PARTY_NOT_MEET_REQS                 = 6,      // One or more party members do not meet the requirements for the chosen dungeons
    LFG_JOIN_MIXED_RAID_DUNGEON                  = 7,      // You cannot mix dungeons, raids, and random when picking dungeons
    LFG_JOIN_MULTI_REALM                         = 8,      // The dungeon you chose does not support players from multiple realms
    LFG_JOIN_DISCONNECTED                        = 9,      // One or more party members are pending invites or disconnected
    LFG_JOIN_PARTY_INFO_FAILED                   = 10,     // Could not retrieve information about some party members
    LFG_JOIN_DUNGEON_INVALID                     = 11,     // One or more dungeons was not valid
    LFG_JOIN_DESERTER                            = 12,     // You can not queue for dungeons until your deserter debuff wears off
    LFG_JOIN_PARTY_DESERTER                      = 13,     // One or more party members has a deserter debuff
    LFG_JOIN_RANDOM_COOLDOWN                     = 14,     // You can not queue for random dungeons while on random dungeon cooldown
    LFG_JOIN_PARTY_RANDOM_COOLDOWN               = 15,     // One or more party members are on random dungeon cooldown
    LFG_JOIN_TOO_MUCH_MEMBERS                    = 16,     // You can not enter dungeons with more that 5 party members
    LFG_JOIN_USING_BG_SYSTEM                     = 17,     // You can not use the dungeon system while in BG or arenas
    LFG_JOIN_ROLE_CHECK_FAILED                   = 18,
};

enum LfgUpdateType
{
    LFG_UPDATETYPE_DEFAULT                       = 0,      // Internal Use
    LFG_UPDATETYPE_LEADER_UNK1                   = 1,      // FIXME: At group leave
    LFG_UPDATETYPE_UNK2                          = 2,
    LFG_UPDATETYPE_UNK3                          = 3,
    LFG_UPDATETYPE_ROLECHECK_ABORTED             = 4,
    LFG_UPDATETYPE_JOIN_QUEUE                    = 5,
    LFG_UPDATETYPE_ROLECHECK_FAILED              = 6,
    LFG_UPDATETYPE_REMOVED_FROM_QUEUE            = 7,
    LFG_UPDATETYPE_PROPOSAL_FAILED               = 8,
    LFG_UPDATETYPE_PROPOSAL_DECLINED             = 9,
    LFG_UPDATETYPE_GROUP_FOUND                   = 10,
    LFG_UPDATETYPE_ADDED_TO_QUEUE                = 12,
    LFG_UPDATETYPE_PROPOSAL_BEGIN                = 13,
    LFG_UPDATETYPE_UPDATE_STATUS                 = 14,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE          = 15,
    LFG_UPDATETYPE_GROUP_DISBAND_UNK16           = 16,     // FIXME: Sometimes at group disband
};

enum RoleIndex
{
    ROLE_INDEX_PLAYER   = 0,
    ROLE_INDEX_TANK     = 0,
    ROLE_INDEX_HEALER   = 1,
    ROLE_INDEX_DPS      = 2,
    ROLE_INDEX_COUNT
};

enum LFGEnum
{
    LFG_TANKS_NEEDED                             = 1,
    LFG_HEALERS_NEEDED                           = 1,
    LFG_DPS_NEEDED                               = 3
};

enum SeasonalDungeons
{
    SEASONAL_HEADLESS_HORSEMAN  = 285,
    SEASONAL_AHUNE              = 286,
    SEASONAL_COREN_DIREBREW     = 287,
    SEASONAL_CROWN_CHEMICAL_CO  = 288,
};

typedef std::map<uint32, uint32> LfgLockMap;
typedef std::map<ObjectGuid, LfgLockMap> LfgLockPartyMap;
typedef std::set<uint32> LfgDungeonSet;

struct LfgInstanceSave
{
    ObjectGuid instanceGuid;
    uint32 completedEncountersMask;
};

typedef std::map<uint32, LfgInstanceSave> LfgInstanceSaveMap;

struct LfgJoinResultData
{
    LfgJoinResultData(LfgJoinResult result = LFG_JOIN_OK, LfgRoleCheckState state = LFG_ROLECHECK_DEFAULT) :
        result(result), state(state) { }
    LfgJoinResult result;
    LfgRoleCheckState state;
    LfgLockPartyMap lockmap;
};

struct LfgUpdateData
{
    LfgUpdateData(LfgUpdateType type = LFG_UPDATETYPE_DEFAULT) : updateType(type), state(LFG_STATE_NONE), comment(""), partialClear(false) { memset(m_roles, 0, sizeof(m_roles)); }
    LfgUpdateData(LfgUpdateType type, LfgDungeonSet const& dungeons, std::string const& comment) :
        updateType(type), state(LFG_STATE_NONE), dungeons(dungeons), comment(comment), partialClear(false) { memset(m_roles, 0, sizeof(m_roles)); }
    LfgUpdateData(LfgUpdateType type, LfgState state, LfgDungeonSet const& dungeons, std::string const& comment = "") :
        updateType(type), state(state), dungeons(dungeons), comment(comment), partialClear(false) { memset(m_roles, 0, sizeof(m_roles)); }

    LfgUpdateType updateType;
    LfgState state;
    LfgDungeonSet dungeons;
    bool partialClear;
    std::string comment;
    uint8 m_roles[ROLE_INDEX_COUNT];
};

struct LfgPlayerRewardData
{
    LfgPlayerRewardData(uint32 random, uint32 current, bool done, Quest const* quest) :
        rdungeonEntry(random), sdungeonEntry(current), done(done), quest(quest) { }
    uint32 rdungeonEntry;
    uint32 sdungeonEntry;
    bool done;
    Quest const* quest;
};

#endif