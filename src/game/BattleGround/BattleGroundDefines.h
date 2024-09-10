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

#ifndef _BG_DEFINES_H
#define _BG_DEFINES_H

#include "Common.h"

#define BATTLEGROUND_ARENA_POINT_DISTRIBUTION_DAY 86400     // seconds in a day
#define COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME 10

enum BattleGroundQueueGroupTypes
{
    BG_QUEUE_PREMADE_ALLIANCE   = 0,
    BG_QUEUE_PREMADE_HORDE      = 1,
    BG_QUEUE_NORMAL_ALLIANCE    = 2,
    BG_QUEUE_NORMAL_HORDE       = 3
};

#define BG_QUEUE_GROUP_TYPES_COUNT 4

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

// indexes of BattlemasterList.dbc
enum BattleGroundTypeId
{
    BATTLEGROUND_TYPE_NONE     = 0,
    BATTLEGROUND_AV            = 1,
    BATTLEGROUND_WS            = 2,
    BATTLEGROUND_AB            = 3,
    BATTLEGROUND_NA            = 4,
    BATTLEGROUND_BE            = 5,
    BATTLEGROUND_AA            = 6,                         // all arenas
    BATTLEGROUND_EY            = 7,
    BATTLEGROUND_RL            = 8,
    BATTLEGROUND_SA            = 9,
    BATTLEGROUND_DS            = 10,
    BATTLEGROUND_RV            = 11,
    BATTLEGROUND_IC            = 30,
    BATTLEGROUND_RB            = 32                         // random battleground
};
#define MAX_BATTLEGROUND_TYPE_ID 33

enum ArenaType
{
    ARENA_TYPE_NONE = 0,                            // used for mark non-arenas or problematic cases
    ARENA_TYPE_2v2 = 2,
    ARENA_TYPE_3v3 = 3,
    ARENA_TYPE_5v5 = 5
};

inline bool IsArenaTypeValid(ArenaType type) { return type == ARENA_TYPE_2v2 || type == ARENA_TYPE_3v3 || type == ARENA_TYPE_5v5; }

#endif
