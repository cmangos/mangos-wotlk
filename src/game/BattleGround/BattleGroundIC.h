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

#ifndef __BATTLEGROUNDIC_H
#define __BATTLEGROUNDIC_H

#include "Common.h"
#include "BattleGround.h"
#include "Tools/Language.h"

class BattleGround;

enum
{
    // generic enums
    BG_IC_MAX_KEEP_GATES                    = 3,
    BG_IC_MAX_REINFORCEMENTS                = 300,
    BG_IC_WORKSHOP_UPDATE_TIME              = 3 * MINUTE * IN_MILLISECONDS,
    BG_IC_TRANSPORT_PERIOD_TIME             = 2 * MINUTE * IN_MILLISECONDS,
    BG_IC_FLAG_CAPTURING_TIME               = MINUTE * IN_MILLISECONDS,
    BG_IC_RESOURCE_TICK_TIMER               = 45000,
    BG_IC_CLOSE_DOORS_TIME                  = 20000,

    // world states
    BG_IC_STATE_ALLY_REINFORCE_SHOW         = 4221,         // reinforcements counters
    BG_IC_STATE_HORDE_REINFORCE_SHOW        = 4222,
    BG_IC_STATE_ALLY_REINFORCE_COUNT        = 4226,
    BG_IC_STATE_HORDE_REINFORCE_COUNT       = 4227,

    BG_IC_STATE_GATE_FRONT_H_CLOSED         = 4317,         // keep gates are closed / intact
    BG_IC_STATE_GATE_WEST_H_CLOSED          = 4318,
    BG_IC_STATE_GATE_EAST_H_CLOSED          = 4319,
    BG_IC_STATE_GATE_FRONT_A_CLOSED         = 4328,
    BG_IC_STATE_GATE_WEST_A_CLOSED          = 4327,
    BG_IC_STATE_GATE_EAST_A_CLOSED          = 4326,

    BG_IC_STATE_GATE_FRONT_H_OPEN           = 4322,         // keep gates are open / destroyed
    BG_IC_STATE_GATE_WEST_H_OPEN            = 4321,
    BG_IC_STATE_GATE_EAST_H_OPEN            = 4320,
    BG_IC_STATE_GATE_FRONT_A_OPEN           = 4323,
    BG_IC_STATE_GATE_WEST_A_OPEN            = 4324,
    BG_IC_STATE_GATE_EAST_A_OPEN            = 4325,

    BG_IC_STATE_DOCKS_UNCONTROLLED          = 4301,         // docks
    BG_IC_STATE_DOCKS_CONFLICT_A            = 4305,
    BG_IC_STATE_DOCKS_CONFLICT_H            = 4302,
    BG_IC_STATE_DOCKS_CONTROLLED_A          = 4304,
    BG_IC_STATE_DOCKS_CONTROLLED_H          = 4303,

    BG_IC_STATE_HANGAR_UNCONTROLLED         = 4296,         // hangar
    BG_IC_STATE_HANGAR_CONFLICT_A           = 4300,
    BG_IC_STATE_HANGAR_CONFLICT_H           = 4297,
    BG_IC_STATE_HANGAR_CONTROLLED_A         = 4299,
    BG_IC_STATE_HANGAR_CONTROLLED_H         = 4298,

    BG_IC_STATE_WORKSHOP_UNCONTROLLED       = 4294,         // workshop
    BG_IC_STATE_WORKSHOP_CONFLICT_A         = 4228,
    BG_IC_STATE_WORKSHOP_CONFLICT_H         = 4293,
    BG_IC_STATE_WORKSHOP_CONTROLLED_A       = 4229,
    BG_IC_STATE_WORKSHOP_CONTROLLED_H       = 4230,

    BG_IC_STATE_QUARRY_UNCONTROLLED         = 4306,         // quarry
    BG_IC_STATE_QUARRY_CONFLICT_A           = 4310,
    BG_IC_STATE_QUARRY_CONFLICT_H           = 4307,
    BG_IC_STATE_QUARRY_CONTROLLED_A         = 4309,
    BG_IC_STATE_QUARRY_CONTROLLED_H         = 4308,

    BG_IC_STATE_REFINERY_UNCONTROLLED       = 4311,         // refinery
    BG_IC_STATE_REFINERY_CONFLICT_A         = 4315,
    BG_IC_STATE_REFINERY_CONFLICT_H         = 4312,
    BG_IC_STATE_REFINERY_CONTROLLED_A       = 4314,
    BG_IC_STATE_REFINERY_CONTROLLED_H       = 4313,

    BG_IC_STATE_ALLY_KEEP_UNCONTROLLED      = 4341,         // alliance keep
    BG_IC_STATE_ALLY_KEEP_CONFLICT_A        = 4342,
    BG_IC_STATE_ALLY_KEEP_CONFLICT_H        = 4343,
    BG_IC_STATE_ALLY_KEEP_CONTROLLED_A      = 4339,
    BG_IC_STATE_ALLY_KEEP_CONTROLLED_H      = 4340,

    BG_IC_STATE_HORDE_KEEP_UNCONTROLLED     = 4346,         // horde keep
    BG_IC_STATE_HORDE_KEEP_CONFLICT_A       = 4347,
    BG_IC_STATE_HORDE_KEEP_CONFLICT_H       = 4348,
    BG_IC_STATE_HORDE_KEEP_CONTROLLED_A     = 4344,
    BG_IC_STATE_HORDE_KEEP_CONTROLLED_H     = 4345,

    // creatures
    BG_IC_NPC_COMMANDER_WYRMBANE            = 34924,        // Alliance Boss
    BG_IC_NPC_OVERLORD_AGMAR                = 34922,        // Horde Boss
    BG_IC_NPC_KORKORN_GUARD                 = 34918,
    BG_IC_NPC_LEGION_INFANTRY               = 34919,        // has aura 66656 cast by 20213

    BG_IC_NPC_GOBLIN_MECHANIC               = 35346,        // workshop npcs
    BG_IC_NPC_GNOMISH_MECHANIC              = 35345,

    // gunship creatures
    BG_IC_NPC_HORDE_GUNSHIP_CAPTAIN         = 35003,
    BG_IC_NPC_KORKORN_REAVER                = 36164,
    BG_IC_NPC_GOBLIN_ENGINEERING_CREW       = 36162,

    BG_IC_NPC_ALLIANCE_GUNSHIP_CAPTAIN      = 34960,
    BG_IC_NPC_LEGION_MARINE                 = 36166,
    BG_IC_NPC_LEGION_DECKHAND               = 36165,
    BG_IC_NPC_NAVIGATOR_TAYLOR              = 36151,
    BG_IC_NPC_NAVIGATOR_SARACEN             = 36152,

    // triggers
    BG_IC_NPC_WORLD_TRIGGER                 = 22515,
    BG_IC_NPC_WORLD_TRIGGER_NOT_FLOAT       = 34984,        // teleport triggers on gunships
    BG_IC_NPC_WORLD_TRIGGER_A               = 20213,        // parachute trigger on gunship
    BG_IC_NPC_WORLD_TRIGGER_H               = 20212,        // parachute trigger on gunship

    // siege vehicles
    BG_IC_VEHICLE_GUNSHIP_CANNON_H          = 34935,
    BG_IC_VEHICLE_GUNSHIP_CANNON_A          = 34929,
    BG_IC_VEHICLE_KEEP_CANNON               = 34944,
    BG_IC_VEHICLE_SIEGE_ENGINE_A            = 34776,        // has accessories: 36356 and 34777; starts with aura 67323
    BG_IC_VEHICLE_SIEGE_ENGINE_H            = 35069,        // has accessories: 34778 and 36355; starts with aura 67323
    BG_IC_VEHICLE_DEMOLISHER                = 34775,
    BG_IC_VEHICLE_GLAIVE_THROWER_A          = 34802,
    BG_IC_VEHICLE_GLAIVE_THROWER_H          = 35273,
    BG_IC_VEHICLE_CATAPULT                  = 34793,


    // destructible gameobjects
    BG_IC_GO_GATE_WEST_A                    = 195699,
    BG_IC_GO_GATE_EAST_A                    = 195698,
    BG_IC_GO_GATE_FRONT_A                   = 195700,

    BG_IC_GO_GATE_WEST_H                    = 195496,
    BG_IC_GO_GATE_EAST_H                    = 195495,
    BG_IC_GO_GATE_FRONT_H                   = 195494,

    // gunships
    BG_IC_GO_GUNSHIP_A                      = 195121,
    BG_IC_GO_GUNSHIP_H                      = 195276,

    // keep gates - they open and close after battleground start
    BG_IC_GO_PORTCULLIS_GATE_A              = 195703,
    BG_IC_GO_PORTCULLIS_GATE_H              = 195491,

    // tower gates - they open and stay opened
    BG_IC_GO_PORTCULLIS_TOWER_A             = 195436,
    BG_IC_GO_PORTCULLIS_TOWER_H             = 195437,

    // inner keep gates - they open once the outer gates are breached
    BG_IC_GO_PORTCULLIS_KEEP_A1             = 195451,
    BG_IC_GO_PORTCULLIS_KEEP_A2             = 195452,
    BG_IC_GO_PORTCULLIS_KEEP_H              = 195223,

    // portals to the gunship (on the ground)
    BG_IC_GO_GUNSHIP_GROUND_PORTAL_A        = 195320,
    BG_IC_GO_GUNSHIP_GROUND_PORTAL_H        = 195326,

    // gunship portals visual effects
    BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_A       = 195705,
    BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_H       = 195706,

    // gunship_portals on the ship
    BG_IC_GO_GUNSHIP_AIR_PORTAL_1           = 195371,
    BG_IC_GO_GUNSHIP_AIR_PORTAL_2           = 196413,

    // seaforium bombs - faction allows click from the opposite team
    BG_IC_GO_HUGE_SEAFORIUM_BOMB_A          = 195332,
    BG_IC_GO_HUGE_SEAFORIUM_BOMB_H          = 195333,
    BG_IC_GO_HUGE_SEAFORIUM_BOMB_TRAP       = 195331,       // actual bomb; triggers spell 66672

    BG_IC_GO_SEAFORIUM_BOMBS                = 195237,       // spawned at the workshop; takes faction from workshop owner
    BG_IC_GO_SEAFORIUM_BOMB_TRAP            = 195235,       // actual bomb; triggers spell 66676

    // teleporters
    BG_IC_GO_TELEPORTER_OUTSIDE_H           = 195314,       // teleports from outside to inside; has spell 66549
    BG_IC_GO_TELEPORTER_INSIDE_H            = 195313,       // teleports from inside to outside; has spell 66548
    BG_IC_GO_TELEPORTER_EFFECTS_H           = 195702,

    BG_IC_GO_TELEPORTER_OUTSIDE_A           = 195315,       // teleports from outside to inside; has spell 66549
    BG_IC_GO_TELEPORTER_INSIDE_A            = 195316,       // teleports from inside to outside; has spell 66548
    BG_IC_GO_TELEPORTER_EFFECTS_A           = 195701,


    // neutral banners - all spawned at the beginning of the battleground
    BG_IC_GO_BANNER_DOCKS                   = 195157,
    BG_IC_GO_BANNER_HANGAR                  = 195158,
    BG_IC_GO_BANNER_QUARRY                  = 195338,
    BG_IC_GO_BANNER_WORKSHOP                = 195133,
    BG_IC_GO_BANNER_REFINERY                = 195343,

    // keep banners
    BG_IC_GO_BANNER_ALLIANCE_KEEP_A         = 195396,       // spawmed at the beginning of battleground
    BG_IC_GO_BANNER_ALLIANCE_KEEP_A_GREY    = 195397,
    BG_IC_GO_BANNER_ALLIANCE_KEEP_H         = 195398,
    BG_IC_GO_BANNER_ALLIANCE_KEEP_H_GREY    = 195399,

    BG_IC_GO_BANNER_HORDE_KEEP_A            = 195391,
    BG_IC_GO_BANNER_HORDE_KEEP_A_GREY       = 195392,
    BG_IC_GO_BANNER_HORDE_KEEP_H            = 195393,       // spawmed at the beginning of battleground
    BG_IC_GO_BANNER_HORDE_KEEP_H_GREY       = 195394,

    // objective banners
    BG_IC_GO_BANNER_DOCKS_A                 = 195153,
    BG_IC_GO_BANNER_DOCKS_A_GREY            = 195154,
    BG_IC_GO_BANNER_DOCKS_H                 = 195155,
    BG_IC_GO_BANNER_DOCKS_H_GREY            = 195156,

    BG_IC_GO_BANNER_HANGAR_A                = 195132,
    BG_IC_GO_BANNER_HANGAR_A_GREY           = 195144,
    BG_IC_GO_BANNER_HANGAR_H                = 195130,
    BG_IC_GO_BANNER_HANGAR_H_GREY           = 195145,

    BG_IC_GO_BANNER_QUARRY_A                = 195334,
    BG_IC_GO_BANNER_QUARRY_A_GREY           = 195335,
    BG_IC_GO_BANNER_QUARRY_H                = 195336,
    BG_IC_GO_BANNER_QUARRY_H_GREY           = 195337,

    BG_IC_GO_BANNER_REFINERY_A              = 195339,
    BG_IC_GO_BANNER_REFINERY_A_GREY         = 195340,
    BG_IC_GO_BANNER_REFINERY_H              = 195341,
    BG_IC_GO_BANNER_REFINERY_H_GREY         = 195342,

    BG_IC_GO_BANNER_WORKSHOP_A              = 195149,
    BG_IC_GO_BANNER_WORKSHOP_A_GREY         = 195150,
    BG_IC_GO_BANNER_WORKSHOP_H              = 195151,
    BG_IC_GO_BANNER_WORKSHOP_H_GREY         = 195152,


    // event ids - keep gates breached (individual events for each gate)
    BG_IC_EVENT_ID_KEEP_BREACHED_A1         = 22082,        // west gate
    BG_IC_EVENT_ID_KEEP_BREACHED_A2         = 22078,        // east gate
    BG_IC_EVENT_ID_KEEP_BREACHED_A3         = 22080,        // front gate

    BG_IC_EVENT_ID_KEEP_BREACHED_H1         = 22081,        // west gate
    BG_IC_EVENT_ID_KEEP_BREACHED_H2         = 22083,        // east gate
    BG_IC_EVENT_ID_KEEP_BREACHED_H3         = 22079,        // front gate


    // spells
    BG_IC_SPELL_REFINERY                    = 68719,
    BG_IC_SPELL_QUARRY                      = 68720,
    BG_IC_SPELL_PARACHUTE                   = 66656,        // triggers 66657


    // achievements
    BG_IC_CRIT_CUT_BLUE_WIRE                = 12132,        // achiev id 3852; type 29; spell id 1843
    BG_IC_CRIT_RESOURCE_GLUT_A              = 12060,        // achiev id 3846
    BG_IC_CRIT_RESOURCE_GLUT_H              = 12061,        // achiev id 4176

    // achievement spells
    BG_IC_SPELL_ACHIEV_DESTROYED_VEHICLE    = 68357,        // used for achiev id 3845 and 3850
    BG_IC_SPELL_ACHIEV_BACK_DOOR_JOB        = 68502,        // used for achiev id 3854
    BG_IC_SPELL_DRIVING_CREDIT_DEMOLISHER   = 68365,        // used for achiev id 3847
    BG_IC_SPELL_DRIVING_CREDIT_GLAIVE       = 68363,        // used for achiev id 3847
    BG_IC_SPELL_DRIVING_CREDIT_SIEGE        = 68364,        // used for achiev id 3847
    BG_IC_SPELL_DRIVING_CREDIT_CATAPULT     = 68362,        // used for achiev id 3847
    BG_IC_SPELL_BOMB_CREDIT                 = 68366,        // used for achievement 3848 and 3849

    // graveyard links
    BG_IC_GRAVEYARD_ID_DOCKS                = 1480,
    BG_IC_GRAVEYARD_ID_HANGAR               = 1481,
    BG_IC_GRAVEYARD_ID_WORKSHOP             = 1482,
    BG_IC_GRAVEYARD_ID_ALLIANCE             = 1483,         // last option for alliance; not capturable
    BG_IC_GRAVEYARD_ID_HORDE                = 1484,         // last option for horde; not capturable
    BG_IC_GRAVEYARD_ID_KEEP_ALLY            = 1485,
    BG_IC_GRAVEYARD_ID_KEEP_HORDE           = 1486,

    // sounds
    BG_IC_SOUND_NODE_CLAIMED                = 8192,
    BG_IC_SOUND_NODE_CAPTURED_ALLIANCE      = 8173,
    BG_IC_SOUND_NODE_CAPTURED_HORDE         = 8213,
    BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE     = 8212,
    BG_IC_SOUND_NODE_ASSAULTED_HORDE        = 8174,

    // factions
    BG_IC_FACTION_ID_ALLIANCE               = 1732,
    BG_IC_FACTION_ID_HORDE                  = 1735,
};

// *** Battleground factions *** //
const uint32 iocTeamFactions[PVP_TEAM_COUNT] = { BG_IC_FACTION_ID_ALLIANCE, BG_IC_FACTION_ID_HORDE };

struct IsleSummonData
{
    uint32 entry;
    float x, y, z, o;
};

// *** Battleground keep spawn data *** //
static const IsleSummonData iocAllianceKeepSpawns[] =
{
    {BG_IC_NPC_COMMANDER_WYRMBANE, 224.983f, -831.573f, 60.9034f, 0.0f},
    {BG_IC_NPC_LEGION_INFANTRY, 223.969f, -822.958f, 60.8151f, 0.46337f},
    {BG_IC_NPC_LEGION_INFANTRY, 224.211f, -826.952f, 60.8188f, 6.25961f},
    {BG_IC_NPC_LEGION_INFANTRY, 223.119f, -838.386f, 60.8145f, 5.64857f},
    {BG_IC_NPC_LEGION_INFANTRY, 223.889f, -835.102f, 60.8201f, 6.21642f},
};

static const IsleSummonData iocHordeKeepSpawns[] =
{
    {BG_IC_NPC_OVERLORD_AGMAR, 1295.44f, -765.733f, 70.0541f, 0.0f},
    {BG_IC_NPC_KORKORN_GUARD, 1296.01f, -773.256f, 69.958f, 0.292168f},
    {BG_IC_NPC_KORKORN_GUARD, 1295.94f, -757.756f, 69.9587f, 6.02165f},
    {BG_IC_NPC_KORKORN_GUARD, 1295.09f, -760.927f, 69.9587f, 5.94311f},
    {BG_IC_NPC_KORKORN_GUARD, 1295.13f, -769.7f, 69.95f, 0.34f},
};

struct IsleDualSummonData
{
    uint32 entryAlly;
    uint32 entryHorde;
    float x, y, z, o;
};

// *** Battleground objectives spawn data *** //
static const IsleDualSummonData iocWorkshopSpawns[] =
{
    {BG_IC_VEHICLE_SIEGE_ENGINE_A,  BG_IC_VEHICLE_SIEGE_ENGINE_H, 773.681f, -884.092f, 16.809f,  1.58825f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     751.828f, -852.733f, 12.5251f, 1.46608f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     761.809f, -854.227f, 12.5263f, 1.46608f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     783.472f, -853.96f,  12.5478f, 1.71042f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     793.056f, -852.719f, 12.5671f, 1.71042f},
    {BG_IC_NPC_GNOMISH_MECHANIC,    BG_IC_NPC_GOBLIN_MECHANIC,    762.6146f, -883.6736f, 18.61661f, 0.01745329f}
};

static const IsleDualSummonData iocDocksSpawns[] =
{
    {BG_IC_VEHICLE_GLAIVE_THROWER_A, BG_IC_VEHICLE_GLAIVE_THROWER_H, 779.312f, -342.972f, 12.2105f, 4.71239f},
    {BG_IC_VEHICLE_GLAIVE_THROWER_A, BG_IC_VEHICLE_GLAIVE_THROWER_H, 790.03f,  -342.899f, 12.2129f, 4.71238f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         757.283f, -341.78f,  12.2114f, 4.72984f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         766.948f, -342.054f, 12.201f,  4.694f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         800.378f, -342.608f, 12.167f,  4.6774f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         810.726f, -342.083f, 12.1676f, 4.66f},
};

enum IsleObjective
{
    BG_IC_OBJECTIVE_KEEP_ALLY,
    BG_IC_OBJECTIVE_KEEP_HORDE,
    BG_IC_OBJECTIVE_WORKSHOP,
    BG_IC_OBJECTIVE_DOCKS,
    BG_IC_OBJECTIVE_HANGAR,
    BG_IC_OBJECTIVE_REFINERY,
    BG_IC_OBJECTIVE_QUARY,
    BG_IC_MAX_OBJECTIVES
};

struct IsleObjectiveData
{
    uint8 objectiveId;
    uint32 message, stateNeutral, stateAlly, stateAllyGrey, stateHorde, stateHordeGrey;
};

// *** Battleground object data *** //
static const IsleObjectiveData isleObjectData[] =
{
    {BG_IC_OBJECTIVE_KEEP_ALLY,  LANG_BG_IC_NODE_KEEP_ALLIANCE, BG_IC_STATE_ALLY_KEEP_UNCONTROLLED,  BG_IC_STATE_ALLY_KEEP_CONTROLLED_A,  BG_IC_STATE_ALLY_KEEP_CONFLICT_A,  BG_IC_STATE_ALLY_KEEP_CONTROLLED_H,  BG_IC_STATE_ALLY_KEEP_CONFLICT_H},
    {BG_IC_OBJECTIVE_KEEP_HORDE, LANG_BG_IC_NODE_KEEP_HORDE,    BG_IC_STATE_HORDE_KEEP_UNCONTROLLED, BG_IC_STATE_HORDE_KEEP_CONTROLLED_A, BG_IC_STATE_HORDE_KEEP_CONFLICT_A, BG_IC_STATE_HORDE_KEEP_CONTROLLED_H, BG_IC_STATE_HORDE_KEEP_CONFLICT_H},
    {BG_IC_OBJECTIVE_WORKSHOP,   LANG_BG_IC_NODE_WORKSHOP,      BG_IC_STATE_WORKSHOP_UNCONTROLLED,   BG_IC_STATE_WORKSHOP_CONTROLLED_A,   BG_IC_STATE_WORKSHOP_CONFLICT_A,   BG_IC_STATE_WORKSHOP_CONTROLLED_H,   BG_IC_STATE_WORKSHOP_CONFLICT_H},
    {BG_IC_OBJECTIVE_DOCKS,      LANG_BG_IC_NODE_DOCKS,         BG_IC_STATE_DOCKS_UNCONTROLLED,      BG_IC_STATE_DOCKS_CONTROLLED_A,      BG_IC_STATE_DOCKS_CONFLICT_A,      BG_IC_STATE_DOCKS_CONTROLLED_H,      BG_IC_STATE_DOCKS_CONFLICT_H},
    {BG_IC_OBJECTIVE_HANGAR,     LANG_BG_IC_NODE_HANGAR,        BG_IC_STATE_HANGAR_UNCONTROLLED,     BG_IC_STATE_HANGAR_CONTROLLED_A,     BG_IC_STATE_HANGAR_CONFLICT_A,     BG_IC_STATE_HANGAR_CONTROLLED_H,     BG_IC_STATE_HANGAR_CONFLICT_H},
    {BG_IC_OBJECTIVE_REFINERY,   LANG_BG_IC_NODE_REFINERY,      BG_IC_STATE_REFINERY_UNCONTROLLED,   BG_IC_STATE_REFINERY_CONTROLLED_A,   BG_IC_STATE_REFINERY_CONFLICT_A,   BG_IC_STATE_REFINERY_CONTROLLED_H,   BG_IC_STATE_REFINERY_CONFLICT_H},
    {BG_IC_OBJECTIVE_QUARY,      LANG_BG_IC_NODE_QUARRY,        BG_IC_STATE_QUARRY_UNCONTROLLED,     BG_IC_STATE_QUARRY_CONTROLLED_A,     BG_IC_STATE_QUARRY_CONFLICT_A,     BG_IC_STATE_QUARRY_CONTROLLED_H,     BG_IC_STATE_QUARRY_CONFLICT_H},
};

struct IsleWallsData
{
    uint32 eventId, stateClosed, stateOpened, message;
};

// *** Battleground walls data *** //
static const IsleWallsData isleAllianceWallsData[] =
{
    {BG_IC_EVENT_ID_KEEP_BREACHED_A3, BG_IC_STATE_GATE_FRONT_A_CLOSED, BG_IC_STATE_GATE_FRONT_A_OPEN, LANG_BG_IC_FRONT_GATE_ALLIANCE},
    {BG_IC_EVENT_ID_KEEP_BREACHED_A1, BG_IC_STATE_GATE_WEST_A_CLOSED,  BG_IC_STATE_GATE_WEST_A_OPEN,  LANG_BG_IC_WEST_GATE_ALLIANCE},
    {BG_IC_EVENT_ID_KEEP_BREACHED_A2, BG_IC_STATE_GATE_EAST_A_CLOSED,  BG_IC_STATE_GATE_EAST_A_OPEN,  LANG_BG_IC_EAST_GATE_ALLIANCE}
};

static const IsleWallsData isleHordeWallsData[] =
{
    {BG_IC_EVENT_ID_KEEP_BREACHED_H3, BG_IC_STATE_GATE_FRONT_H_CLOSED, BG_IC_STATE_GATE_FRONT_H_OPEN, LANG_BG_IC_FRONT_GATE_HORDE},
    {BG_IC_EVENT_ID_KEEP_BREACHED_H1, BG_IC_STATE_GATE_WEST_H_CLOSED,  BG_IC_STATE_GATE_WEST_H_OPEN,  LANG_BG_IC_WEST_GATE_HORDE},
    {BG_IC_EVENT_ID_KEEP_BREACHED_H2, BG_IC_STATE_GATE_EAST_H_CLOSED,  BG_IC_STATE_GATE_EAST_H_OPEN,  LANG_BG_IC_EAST_GATE_HORDE}
};

class BattleGroundICScore : public BattleGroundScore
{
    public:
        BattleGroundICScore(): BasesAssaulted(0), BasesDefended(0) {};
        virtual ~BattleGroundICScore() {};

        uint32 GetAttr1() const { return BasesAssaulted; }
        uint32 GetAttr2() const { return BasesDefended; }

        uint32 BasesAssaulted;
        uint32 BasesDefended;
};

class BattleGroundIC : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundIC();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;
        virtual void StartingEventOpenDoors() override;
        virtual void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
        virtual void Reset() override;
        void EndBattleGround(Team winner) override;

        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;

        /* Scorekeeping */
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

        bool HandleEvent(uint32 eventId, GameObject* go, Unit* invoker) override;
        void HandleKillUnit(Creature* creature, Player* killer) override;
        void HandleKillPlayer(Player* player, Player* killer) override;

        void EventPlayerClickedOnFlag(Player* player, GameObject* go) override;

        void Update(uint32 diff) override;

    private:

        PvpTeamIndex m_objectiveOwner[BG_IC_MAX_OBJECTIVES];
        uint32 m_objectiveState[BG_IC_MAX_OBJECTIVES];
        uint32 m_gatesAllianceState[BG_IC_MAX_KEEP_GATES];
        uint32 m_gatesHordeState[BG_IC_MAX_KEEP_GATES];

        ObjectGuid m_gunshipGuid[PVP_TEAM_COUNT];

        uint32 m_reinforcements[PVP_TEAM_COUNT];
        uint32 m_closeDoorTimer;

        GuidList m_bombsGuids;
        GuidList m_allianceGatesGuids;
        GuidList m_hordeGatesGuids;
        GuidList m_towerGatesGuids;
        GuidList m_teleporterGuids;
        GuidList m_teleporterAnimGuids;
};
#endif
