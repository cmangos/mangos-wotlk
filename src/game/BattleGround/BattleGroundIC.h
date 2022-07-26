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

enum ICGenericVariables
{
    // generic enums
    BG_IC_MAX_REINFORCEMENTS                = 300,

    BG_IC_WORKSHOP_UPDATE_TIME              = 3 * MINUTE * IN_MILLISECONDS,
    BG_IC_TRANSPORT_PERIOD_TIME             = 2 * MINUTE * IN_MILLISECONDS,
    BG_IC_FLAG_CAPTURING_TIME               = MINUTE * IN_MILLISECONDS,
    BG_IC_RESOURCE_TICK_TIMER               = 45000,
    BG_IC_CLOSE_DOORS_TIME                  = 20000,

    BG_IC_BONUS_HONOR_BOSS_KILL             = 4,

    BG_IC_ZONE_ID_ISLE                      = 4710,
};

enum ICWorldStates
{
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
};

enum ICCreatures
{
    // creatures
    BG_IC_NPC_COMMANDER_WYRMBANE            = 34924,        // Alliance Boss
    BG_IC_NPC_OVERLORD_AGMAR                = 34922,        // Horde Boss
    BG_IC_NPC_KORKORN_GUARD                 = 34918,
    BG_IC_NPC_LEGION_INFANTRY               = 34919,        // has aura 66656 cast by 20213
    BG_IC_NPC_DOOR_FIRE                     = 35377,        // summoned when a gate is destroyed (implemented in DB script by event id)

    BG_IC_NPC_GOBLIN_MECHANIC               = 35346,        // workshop npcs
    BG_IC_NPC_GNOMISH_MECHANIC              = 35345,
    BG_IC_NPC_GNOME_ENGINEER                = 13000,        // has emote state 69
    BG_IC_NPC_GOBLIN_ENGINEER               = 36162,        // this one is guesswork but most likely correct

    // gunship creatures - spawned by DB directly on the transport
    // ToDo: implement intro event for each faction. When Hangar is captured the captain of the ship spawns and yells near the ground teleporters
    BG_IC_NPC_HORDE_GUNSHIP_CAPTAIN         = 35003,
    // BG_IC_NPC_KORKORN_REAVER             = 36164,
    // BG_IC_NPC_GOBLIN_ENGINEERING_CREW    = 36162,
    // BG_IC_NPC_OGRIMS_HAMMER_ENGINEER     = 30753,

    BG_IC_NPC_ALLIANCE_GUNSHIP_CAPTAIN      = 34960,
    // BG_IC_NPC_LEGION_MARINE              = 36166,
    // BG_IC_NPC_LEGION_DECKHAND            = 36165,
    // BG_IC_NPC_NAVIGATOR_TAYLOR           = 36151,
    // BG_IC_NPC_NAVIGATOR_SARACEN          = 36152,

    // creatures that act like boats; used during the docks event
    BG_IC_NPC_ALLIANCE_BOAT                 = 35335,        // vehicles with passengers: creature 35339
    BG_IC_NPC_HORDE_BOAT                    = 35336,
    BG_IC_NPC_BOAT_FIRE                     = 35339,

    // triggers
    BG_IC_NPC_WORLD_TRIGGER                 = 22515,
    BG_IC_NPC_WORLD_TRIGGER_NOT_FLOAT       = 34984,        // teleport triggers on gunships
    BG_IC_NPC_WORLD_TRIGGER_A               = 20213,        // parachute trigger on gunship
    BG_IC_NPC_WORLD_TRIGGER_H               = 20212,        // parachute trigger on gunship
};

enum ICVehicles
{
    // siege vehicles
    BG_IC_VEHICLE_GUNSHIP_CANNON_H          = 34935,
    BG_IC_VEHICLE_GUNSHIP_CANNON_A          = 34929,
    BG_IC_VEHICLE_KEEP_CANNON               = 34944,        // applies aura 50630 and 29266 when creature is about to be killed. Update entry to 35819; Handled in EAI
    // BG_IC_VEHICLE_BROKEN_KEEP_CANNON     = 35819,        // allows player click; player casts 68077 to repair; this triggers 68078, which triggers 43978 and resets entry to 34944
    BG_IC_VEHICLE_SIEGE_ENGINE_A            = 34776,        // has accessories: 36356 and 34777; starts with aura 67323
    BG_IC_VEHICLE_SIEGE_ENGINE_H            = 35069,        // has accessories: 34778 and 36355; starts with aura 67323
    BG_IC_VEHICLE_DEMOLISHER                = 34775,
    BG_IC_VEHICLE_GLAIVE_THROWER_A          = 34802,
    BG_IC_VEHICLE_GLAIVE_THROWER_H          = 35273,
    BG_IC_VEHICLE_CATAPULT                  = 34793,
};

enum ICObjects
{
    // destructible gameobjects
    BG_IC_GO_GATE_WEST_A                    = 195699,
    BG_IC_GO_GATE_EAST_A                    = 195698,
    BG_IC_GO_GATE_FRONT_A                   = 195700,

    BG_IC_GO_GATE_WEST_H                    = 195496,
    BG_IC_GO_GATE_EAST_H                    = 195495,
    BG_IC_GO_GATE_FRONT_H                   = 195494,

    // gunships
    BG_IC_GO_GUNSHIP_A                      = 195121,       // acts as map 641
    BG_IC_GO_GUNSHIP_H                      = 195276,       // acts as map 642

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
    BG_IC_GO_GUNSHIP_GROUND_PORTAL_A        = 195320,       // casts spell 66629 to teleport player to alliance gunship; uses unit target 34984
    BG_IC_GO_GUNSHIP_GROUND_PORTAL_H        = 195326,       // casts spell 66638 to teleport player to horde ship; uses unit target 34984

    // gunship portals visual effects
    BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_A       = 195705,
    BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_H       = 195706,

    // gunship_portals on the ship
    // BG_IC_GO_GUNSHIP_AIR_PORTAL_A        = 195371,       // casts spell 66899 in order to teleport players to the ground
    // BG_IC_GO_GUNSHIP_AIR_PORTAL_H        = 196413,

    // seaforium bombs - faction allows click from the opposite team
    BG_IC_GO_HUGE_SEAFORIUM_BOMB_A          = 195332,
    BG_IC_GO_HUGE_SEAFORIUM_BOMB_H          = 195333,
    // BG_IC_GO_HUGE_SEAFORIUM_BOMB_TRAP    = 195331,       // actual bomb; triggers spell 66672

    // workshop seaforium bombs - faction allows click from the opposite team
    BG_IC_GO_SEAFORIUM_BOMBS_A              = 195237,
    BG_IC_GO_SEAFORIUM_BOMBS_H              = 195232,
    // BG_IC_GO_SEAFORIUM_BOMB_TRAP         = 195235,       // actual bomb; triggers spell 66676

    // teleporters
    BG_IC_GO_TELEPORTER_OUTSIDE_H           = 195314,       // teleports from outside to inside; has spell 66549
    BG_IC_GO_TELEPORTER_INSIDE_H            = 195313,       // teleports from inside to outside; has spell 66548
    BG_IC_GO_TELEPORTER_EFFECTS_H           = 195702,

    BG_IC_GO_TELEPORTER_OUTSIDE_A           = 195315,       // teleports from outside to inside; has spell 66549
    BG_IC_GO_TELEPORTER_INSIDE_A            = 195316,       // teleports from inside to outside; has spell 66548
    BG_IC_GO_TELEPORTER_EFFECTS_A           = 195701,
};

enum ICBanners
{
    // neutral banners - all spawned at the beginning of the battleground
    BG_IC_GO_BANNER_DOCKS                   = 195157,       // triggers spell 65826
    BG_IC_GO_BANNER_HANGAR                  = 195158,       // triggers spell 65825
    BG_IC_GO_BANNER_QUARRY                  = 195338,       // triggers spell 66686
    BG_IC_GO_BANNER_WORKSHOP                = 195133,       // triggers spell 35092
    BG_IC_GO_BANNER_REFINERY                = 195343,       // triggers spell 66687

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
};

enum ICEvents
{
    // event ids - keep gates breached (individual events for each gate)
    BG_IC_EVENT_ID_KEEP_BREACHED_A1         = 22082,        // west gate
    BG_IC_EVENT_ID_KEEP_BREACHED_A2         = 22078,        // east gate
    BG_IC_EVENT_ID_KEEP_BREACHED_A3         = 22080,        // front gate

    BG_IC_EVENT_ID_KEEP_BREACHED_H1         = 22081,        // east gate
    BG_IC_EVENT_ID_KEEP_BREACHED_H2         = 22083,        // west gate
    BG_IC_EVENT_ID_KEEP_BREACHED_H3         = 22079,        // front gate
};

enum ICSpells
{
    // spells
    BG_IC_SPELL_REFINERY                    = 68719,        // triggers 68722 on vehicles
    BG_IC_SPELL_QUARRY                      = 68720,        // triggers 68723 on vehicles
    BG_IC_SPELL_PARACHUTE                   = 66656,        // triggers 66657
    BG_IC_SPELL_SEAFORIUM_BLAST             = 66676,
    BG_IC_SPELL_HUGE_SEAFORIUM_BLAST        = 66672,
    BG_IC_SPELL_BOAT_FIRE                   = 67302,        // boat fire visual

    // achievement spells
    BG_IC_SPELL_ACHIEV_DESTROYED_VEHICLE    = 68357,        // used for achiev id 3845 and 3850
    BG_IC_SPELL_ACHIEV_BACK_DOOR_JOB        = 68502,        // used for achiev id 3854
    // BG_IC_SPELL_DRIVING_CREDIT_DEMOLISHER= 68365,        // used for achiev id 3847
    // BG_IC_SPELL_DRIVING_CREDIT_GLAIVE    = 68363,        // used for achiev id 3847
    // BG_IC_SPELL_DRIVING_CREDIT_SIEGE     = 68364,        // used for achiev id 3847
    // BG_IC_SPELL_DRIVING_CREDIT_CATAPULT  = 68362,        // used for achiev id 3847
    BG_IC_SPELL_BOMB_CREDIT                 = 68366,        // used for achiev id 3848; triggered from spell 66676
    BG_IC_SPELL_HUGE_BOMB_CREDIT            = 68367,        // used for achiev id 3849; triggered from spell 66672
};

enum ICAchievCriteria
{
    // achievements
    // BG_IC_CRIT_CUT_BLUE_WIRE             = 12132,        // achiev id 3852; type 29; spell id 1843
    BG_IC_CRIT_RESOURCE_GLUT_A              = 12060,        // achiev id 3846
    BG_IC_CRIT_RESOURCE_GLUT_H              = 12061,        // achiev id 4176
    BG_IC_CRIT_MINE_A_1                     = 12062,        // achiev id 3851
    BG_IC_CRIT_MINE_A_2                     = 12063,
    BG_IC_CRIT_MINE_H_1                     = 12064,        // achiev id 4177
    BG_IC_CRIT_MINE_H_2                     = 12065,
    BG_IC_CRIT_MOVED_DOWN_VEHICLE           = 12114,        // achiev id 3850
    BG_IC_CRIT_MOVED_DOWN_PLAYER            = 12068,
    BG_IC_CRIT_GLAIVE_GRAVE                 = 12183,        // achiev id 3855
};

enum ICGraveyards
{
    // graveyard links
    BG_IC_GRAVEYARD_ID_DOCKS                = 1480,
    BG_IC_GRAVEYARD_ID_HANGAR               = 1481,
    BG_IC_GRAVEYARD_ID_WORKSHOP             = 1482,
    BG_IC_GRAVEYARD_ID_ALLIANCE             = 1483,         // last option for alliance; not capturable
    BG_IC_GRAVEYARD_ID_HORDE                = 1484,         // last option for horde; not capturable
    BG_IC_GRAVEYARD_ID_KEEP_ALLY            = 1485,
    BG_IC_GRAVEYARD_ID_KEEP_HORDE           = 1486,
};

enum ICSounds
{
    // sounds
    BG_IC_SOUND_NODE_CLAIMED                = 8192,
    BG_IC_SOUND_NODE_CAPTURED_ALLIANCE      = 8173,
    BG_IC_SOUND_NODE_CAPTURED_HORDE         = 8213,
    BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE     = 8212,
    BG_IC_SOUND_NODE_ASSAULTED_HORDE        = 8174,
};

enum ICFactions
{
    // factions
    BG_IC_FACTION_ID_ALLIANCE               = 1732,
    BG_IC_FACTION_ID_HORDE                  = 1735,
};

enum ICAreaTriggers
{
    // area triggers - probably checked for achiev 3854
    BG_IC_AREATRIGGER_KEEP_ALLIANCE         = 5555,
    BG_IC_AREATRIGGER_KEEP_HORDE            = 5535,
};

// *** Battleground Gunships *** //
const uint32 iocGunships[PVP_TEAM_COUNT] = { BG_IC_GO_GUNSHIP_A, BG_IC_GO_GUNSHIP_H };

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

// *** Workshop spawn data *** //
static const IsleDualSummonData iocWorkshopSpawns[] =
{
    {BG_IC_VEHICLE_SIEGE_ENGINE_A,  BG_IC_VEHICLE_SIEGE_ENGINE_H, 773.681f, -884.092f, 16.809f,  1.58825f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     751.828f, -852.733f, 12.5251f, 1.46608f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     761.809f, -854.227f, 12.5263f, 1.46608f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     783.472f, -853.96f,  12.5478f, 1.71042f},
    {BG_IC_VEHICLE_DEMOLISHER,      BG_IC_VEHICLE_DEMOLISHER,     793.056f, -852.719f, 12.5671f, 1.71042f},
    {BG_IC_NPC_GNOMISH_MECHANIC,    BG_IC_NPC_GOBLIN_MECHANIC,    762.6146f, -883.6736f, 18.61661f, 0.01745329f},
    {BG_IC_NPC_GNOME_ENGINEER,      BG_IC_NPC_GOBLIN_ENGINEER,    777.7847f, -886.1962f, 16.59797f, 2.901592f},
    {BG_IC_NPC_GNOME_ENGINEER,      BG_IC_NPC_GOBLIN_ENGINEER,    769.6754f, -874.2414f, 16.49906f, 5.443692f},
    {BG_IC_NPC_GNOME_ENGINEER,      BG_IC_NPC_GOBLIN_ENGINEER,    769.7656f, -886.9271f, 16.51083f, 0.7569275f},
};

// *** Docks spawn data *** //
static const IsleDualSummonData iocDocksSpawns[] =
{
    {BG_IC_VEHICLE_GLAIVE_THROWER_A, BG_IC_VEHICLE_GLAIVE_THROWER_H, 779.312f, -342.972f, 12.2105f, 4.71239f},
    {BG_IC_VEHICLE_GLAIVE_THROWER_A, BG_IC_VEHICLE_GLAIVE_THROWER_H, 790.03f,  -342.899f, 12.2129f, 4.71238f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         757.283f, -341.78f,  12.2114f, 4.72984f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         766.948f, -342.054f, 12.201f,  4.694f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         800.378f, -342.608f, 12.167f,  4.6774f},
    {BG_IC_VEHICLE_CATAPULT,         BG_IC_VEHICLE_CATAPULT,         810.726f, -342.083f, 12.1676f, 4.66f},
};

// *** Boats spawn data *** //
static const IsleDualSummonData iocBoatsSpawns[] =
{
    {BG_IC_NPC_ALLIANCE_BOAT, BG_IC_NPC_HORDE_BOAT, 806.8698f,  32.90625f, -0.5591627f, 0.0f},
};

// *** Hangar spawn data *** //
static const IsleDualSummonData iocHangarSpawns[] =
{
    {BG_IC_NPC_ALLIANCE_GUNSHIP_CAPTAIN, BG_IC_NPC_HORDE_GUNSHIP_CAPTAIN, 825.6667f, -994.0052f, 134.35689f, 3.403392f},
};

// *** Alliance Keep extra Honor triggers spawn data *** //
static const IsleDualSummonData iocHonorTriggerAllySpawns[] =
{
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 298.8455f, -784.8785f, 48.9995f, 0},
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 261.5868f, -784.7656f, 48.9996f, 0},
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 298.0756f, -879.5717f, 48.9169f, 0},         // guesswork
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 260.1043f, -879.8110f, 48.9163f, 0},         // guesswork
};

// *** Horde Keep extra Honor triggers spawn data *** //
static const IsleDualSummonData iocHonorTriggerHordeSpawns[] =
{
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 1284.76f, -705.668f, 48.9163f, 0},             // all entries for horde are guesswork
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 1284.548f, -816.063f, 48.916f, 0},
    {BG_NPC_HON_DEFENDER_TRIGGER_25_A, BG_NPC_HON_DEFENDER_TRIGGER_25_H, 1319.526f, -816.779f, 48.929f, 0},
};

// *** Refinery extra spawn data *** //
static const IsleDualSummonData iocRefinerySpawns[] =
{
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1287.289f, -416.776f, 26.4918f, 2.059488f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1308.335f, -377.229f, 42.5379f, 0.349065f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1273.545f, -383.569f, 24.3170f, 0.767944f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1328.034f, -387.795f, 26.4918f, 1.780235f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1212.652f, -449.828f, 23.5789f, 3.752457f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1228.046f, -443.116f,  0.9766f, 5.288345f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1258.456f, -379.866f, 24.2211f, 1.814243f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1217.276f, -458.545f, 26.5706f, 2.321287f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1225.166f, -464.982f,  2.8630f, 2.705260f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1285.914f, -426.515f, 26.4918f, 0.872664f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1307.829f, -379.757f, 42.2753f, 4.923260f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1331.327f, -399.490f, 26.4085f, 4.973788f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1306.779f, -357.947f,  0.3112f, 5.654866f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1284.616f, -417.835f, 26.4085f, 3.231140f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1192.187f, -425.0f,    0.9353f, 1.931931f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1299.057f, -420.019f, 26.4085f, 4.919094f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1277.057f, -394.619f, 24.2337f, 2.700056f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1306.032f, -381.064f, 42.2464f, 2.320241f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1312.183f, -371.847f,  0.6077f, 2.428996f},
    {BG_IC_NPC_GNOME_ENGINEER, BG_IC_NPC_GOBLIN_ENGINEER, 1329.765f, -393.662f, 26.4085f, 4.974071f},
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

    BG_IC_MAX_OBJECTIVES        // max node ids: 7
};

enum IsleResourceNode
{
    BG_IC_RESOURCE_REFINERY,
    BG_IC_RESOURCE_QUARY,

    BG_IC_MAX_RESOURCE_NODES    // max resources: 2
};

// *** Default world states *** //
const uint32 iocDefaultStates[BG_IC_MAX_OBJECTIVES] = { BG_IC_STATE_ALLY_KEEP_CONTROLLED_A, BG_IC_STATE_HORDE_KEEP_CONTROLLED_H, BG_IC_STATE_WORKSHOP_UNCONTROLLED, BG_IC_STATE_DOCKS_UNCONTROLLED, BG_IC_STATE_HANGAR_UNCONTROLLED, BG_IC_STATE_REFINERY_UNCONTROLLED, BG_IC_STATE_QUARRY_UNCONTROLLED};

struct IsleGameObjectNeutralTeamData
{
    uint32 objectEntry;
    // define the next world state and next gameobject entry when clicked on the key GO entry
    uint32 nextWorldStateAlly, nextWorldStateHorde, nextObjectAlly, nextObjectHorde;
    uint8 objectiveId;
};

// Defines the events when objective is neutral and player clicks on the flag
static const IsleGameObjectNeutralTeamData isleGameObjectNeutralData[] =
{
    {BG_IC_GO_BANNER_WORKSHOP, BG_IC_STATE_WORKSHOP_CONFLICT_A, BG_IC_STATE_WORKSHOP_CONFLICT_H, BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_OBJECTIVE_WORKSHOP},
    {BG_IC_GO_BANNER_DOCKS,    BG_IC_STATE_DOCKS_CONFLICT_A,    BG_IC_STATE_DOCKS_CONFLICT_H,    BG_IC_GO_BANNER_DOCKS_A_GREY,    BG_IC_GO_BANNER_DOCKS_H_GREY,    BG_IC_OBJECTIVE_DOCKS},
    {BG_IC_GO_BANNER_HANGAR,   BG_IC_STATE_HANGAR_CONFLICT_A,   BG_IC_STATE_HANGAR_CONFLICT_H,   BG_IC_GO_BANNER_HANGAR_A_GREY,   BG_IC_GO_BANNER_HANGAR_H_GREY,   BG_IC_OBJECTIVE_HANGAR},
    {BG_IC_GO_BANNER_REFINERY, BG_IC_STATE_REFINERY_CONFLICT_A, BG_IC_STATE_REFINERY_CONFLICT_H, BG_IC_GO_BANNER_REFINERY_A_GREY, BG_IC_GO_BANNER_REFINERY_H_GREY, BG_IC_OBJECTIVE_REFINERY},
    {BG_IC_GO_BANNER_QUARRY,   BG_IC_STATE_QUARRY_CONFLICT_A,   BG_IC_STATE_QUARRY_CONFLICT_H,   BG_IC_GO_BANNER_QUARRY_A_GREY,   BG_IC_GO_BANNER_QUARRY_H_GREY,   BG_IC_OBJECTIVE_QUARY},
};

struct IsleGameObjectDualTeamData
{
    uint32 objectEntry;
    // define the next world state and next gameobject entry when clicked on the key GO entry
    uint32 nextStateAssault, nextStateDefend, nextObjectAssault, nextObjectDefend;
    uint8 objectiveId;
};

// Defines the data triggered when objective is contested and player clicks on the flag
static const IsleGameObjectDualTeamData isleGameObjectContestedData[] =
{
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_A_GREY, BG_IC_STATE_ALLY_KEEP_CONFLICT_H,  BG_IC_STATE_ALLY_KEEP_CONTROLLED_H,  BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_OBJECTIVE_KEEP_ALLY},
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_H_GREY, BG_IC_STATE_ALLY_KEEP_CONFLICT_A,  BG_IC_STATE_ALLY_KEEP_CONTROLLED_A,  BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_OBJECTIVE_KEEP_ALLY},

    {BG_IC_GO_BANNER_HORDE_KEEP_A_GREY,    BG_IC_STATE_HORDE_KEEP_CONFLICT_H, BG_IC_STATE_HORDE_KEEP_CONTROLLED_H, BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_OBJECTIVE_KEEP_HORDE},
    {BG_IC_GO_BANNER_HORDE_KEEP_H_GREY,    BG_IC_STATE_HORDE_KEEP_CONFLICT_A, BG_IC_STATE_HORDE_KEEP_CONTROLLED_A, BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_OBJECTIVE_KEEP_HORDE},

    {BG_IC_GO_BANNER_WORKSHOP_A_GREY,      BG_IC_STATE_WORKSHOP_CONFLICT_H,   BG_IC_STATE_WORKSHOP_CONTROLLED_H,   BG_IC_GO_BANNER_WORKSHOP_H_GREY, BG_IC_GO_BANNER_WORKSHOP_H,      BG_IC_OBJECTIVE_WORKSHOP},
    {BG_IC_GO_BANNER_WORKSHOP_H_GREY,      BG_IC_STATE_WORKSHOP_CONFLICT_A,   BG_IC_STATE_WORKSHOP_CONTROLLED_A,   BG_IC_GO_BANNER_WORKSHOP_A_GREY, BG_IC_GO_BANNER_WORKSHOP_A,      BG_IC_OBJECTIVE_WORKSHOP},

    {BG_IC_GO_BANNER_DOCKS_A_GREY,         BG_IC_STATE_DOCKS_CONFLICT_H,      BG_IC_STATE_DOCKS_CONTROLLED_H,      BG_IC_GO_BANNER_DOCKS_H_GREY,    BG_IC_GO_BANNER_DOCKS_H,         BG_IC_OBJECTIVE_DOCKS},
    {BG_IC_GO_BANNER_DOCKS_H_GREY,         BG_IC_STATE_DOCKS_CONFLICT_A,      BG_IC_STATE_DOCKS_CONTROLLED_A,      BG_IC_GO_BANNER_DOCKS_A_GREY,    BG_IC_GO_BANNER_DOCKS_A,         BG_IC_OBJECTIVE_DOCKS},

    {BG_IC_GO_BANNER_HANGAR_A_GREY,        BG_IC_STATE_HANGAR_CONFLICT_H,     BG_IC_STATE_HANGAR_CONTROLLED_H,     BG_IC_GO_BANNER_HANGAR_H_GREY,   BG_IC_GO_BANNER_HANGAR_H,        BG_IC_OBJECTIVE_HANGAR},
    {BG_IC_GO_BANNER_HANGAR_H_GREY,        BG_IC_STATE_HANGAR_CONFLICT_A,     BG_IC_STATE_HANGAR_CONTROLLED_A,     BG_IC_GO_BANNER_HANGAR_A_GREY,   BG_IC_GO_BANNER_HANGAR_A,        BG_IC_OBJECTIVE_HANGAR},

    {BG_IC_GO_BANNER_REFINERY_A_GREY,      BG_IC_STATE_REFINERY_CONFLICT_H,   BG_IC_STATE_REFINERY_CONTROLLED_H,   BG_IC_GO_BANNER_REFINERY_H_GREY, BG_IC_GO_BANNER_REFINERY_H,      BG_IC_OBJECTIVE_REFINERY},
    {BG_IC_GO_BANNER_REFINERY_H_GREY,      BG_IC_STATE_REFINERY_CONFLICT_A,   BG_IC_STATE_REFINERY_CONTROLLED_A,   BG_IC_GO_BANNER_REFINERY_A_GREY, BG_IC_GO_BANNER_REFINERY_A,      BG_IC_OBJECTIVE_REFINERY},

    {BG_IC_GO_BANNER_QUARRY_A_GREY,        BG_IC_STATE_QUARRY_CONFLICT_H,     BG_IC_STATE_QUARRY_CONTROLLED_H,     BG_IC_GO_BANNER_QUARRY_H_GREY,   BG_IC_GO_BANNER_QUARRY_H,        BG_IC_OBJECTIVE_QUARY},
    {BG_IC_GO_BANNER_QUARRY_H_GREY,        BG_IC_STATE_QUARRY_CONFLICT_A,     BG_IC_STATE_QUARRY_CONTROLLED_A,     BG_IC_GO_BANNER_QUARRY_A_GREY,   BG_IC_GO_BANNER_QUARRY_A,        BG_IC_OBJECTIVE_QUARY},
};

struct IsleGameObjectTeamData
{
    uint32 objectEntry;
    // define the next world state and next gameobject entry when clicked on the key GO entry
    uint32 nextState, nextObject;
    uint8 objectiveId;
};

// Defines the data triggered on timer if the objective capture is successful 
static const IsleGameObjectTeamData isleGameObjectConquerData[] =
{
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_A_GREY, BG_IC_STATE_ALLY_KEEP_CONTROLLED_A,  BG_IC_GO_BANNER_ALLIANCE_KEEP_A, BG_IC_OBJECTIVE_KEEP_ALLY},
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_H_GREY, BG_IC_STATE_ALLY_KEEP_CONTROLLED_H,  BG_IC_GO_BANNER_ALLIANCE_KEEP_H, BG_IC_OBJECTIVE_KEEP_ALLY},

    {BG_IC_GO_BANNER_HORDE_KEEP_A_GREY,    BG_IC_STATE_HORDE_KEEP_CONTROLLED_A, BG_IC_GO_BANNER_HORDE_KEEP_A,    BG_IC_OBJECTIVE_KEEP_HORDE},
    {BG_IC_GO_BANNER_HORDE_KEEP_H_GREY,    BG_IC_STATE_HORDE_KEEP_CONTROLLED_H, BG_IC_GO_BANNER_HORDE_KEEP_H,    BG_IC_OBJECTIVE_KEEP_HORDE},

    {BG_IC_GO_BANNER_WORKSHOP_A_GREY,      BG_IC_STATE_WORKSHOP_CONTROLLED_A,   BG_IC_GO_BANNER_WORKSHOP_A,      BG_IC_OBJECTIVE_WORKSHOP},
    {BG_IC_GO_BANNER_WORKSHOP_H_GREY,      BG_IC_STATE_WORKSHOP_CONTROLLED_H,   BG_IC_GO_BANNER_WORKSHOP_H,      BG_IC_OBJECTIVE_WORKSHOP},

    {BG_IC_GO_BANNER_DOCKS_A_GREY,         BG_IC_STATE_DOCKS_CONTROLLED_A,      BG_IC_GO_BANNER_DOCKS_A,         BG_IC_OBJECTIVE_DOCKS},
    {BG_IC_GO_BANNER_DOCKS_H_GREY,         BG_IC_STATE_DOCKS_CONTROLLED_H,      BG_IC_GO_BANNER_DOCKS_H,         BG_IC_OBJECTIVE_DOCKS},

    {BG_IC_GO_BANNER_HANGAR_A_GREY,        BG_IC_STATE_HANGAR_CONTROLLED_A,     BG_IC_GO_BANNER_HANGAR_A,        BG_IC_OBJECTIVE_HANGAR},
    {BG_IC_GO_BANNER_HANGAR_H_GREY,        BG_IC_STATE_HANGAR_CONTROLLED_H,     BG_IC_GO_BANNER_HANGAR_H,        BG_IC_OBJECTIVE_HANGAR},

    {BG_IC_GO_BANNER_REFINERY_A_GREY,      BG_IC_STATE_REFINERY_CONTROLLED_A,   BG_IC_GO_BANNER_REFINERY_A,      BG_IC_OBJECTIVE_REFINERY},
    {BG_IC_GO_BANNER_REFINERY_H_GREY,      BG_IC_STATE_REFINERY_CONTROLLED_H,   BG_IC_GO_BANNER_REFINERY_H,      BG_IC_OBJECTIVE_REFINERY},

    {BG_IC_GO_BANNER_QUARRY_A_GREY,        BG_IC_STATE_QUARRY_CONTROLLED_A,     BG_IC_GO_BANNER_QUARRY_A,        BG_IC_OBJECTIVE_QUARY},
    {BG_IC_GO_BANNER_QUARRY_H_GREY,        BG_IC_STATE_QUARRY_CONTROLLED_H,     BG_IC_GO_BANNER_QUARRY_H,        BG_IC_OBJECTIVE_QUARY},
};

// Defines the data when objective is owned and enemy attacks it 
static const IsleGameObjectTeamData isleGameObjectOwnedData[] =
{
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_A, BG_IC_STATE_ALLY_KEEP_CONFLICT_H,  BG_IC_GO_BANNER_ALLIANCE_KEEP_H_GREY, BG_IC_OBJECTIVE_KEEP_ALLY},
    {BG_IC_GO_BANNER_ALLIANCE_KEEP_H, BG_IC_STATE_ALLY_KEEP_CONFLICT_A,  BG_IC_GO_BANNER_ALLIANCE_KEEP_A_GREY, BG_IC_OBJECTIVE_KEEP_ALLY},

    {BG_IC_GO_BANNER_HORDE_KEEP_A,    BG_IC_STATE_HORDE_KEEP_CONFLICT_H, BG_IC_GO_BANNER_HORDE_KEEP_H_GREY,    BG_IC_OBJECTIVE_KEEP_HORDE},
    {BG_IC_GO_BANNER_HORDE_KEEP_H,    BG_IC_STATE_HORDE_KEEP_CONFLICT_A, BG_IC_GO_BANNER_HORDE_KEEP_A_GREY,    BG_IC_OBJECTIVE_KEEP_HORDE},

    {BG_IC_GO_BANNER_WORKSHOP_A,      BG_IC_STATE_WORKSHOP_CONFLICT_H,   BG_IC_GO_BANNER_WORKSHOP_H_GREY,      BG_IC_OBJECTIVE_WORKSHOP},
    {BG_IC_GO_BANNER_WORKSHOP_H,      BG_IC_STATE_WORKSHOP_CONFLICT_A,   BG_IC_GO_BANNER_WORKSHOP_A_GREY,      BG_IC_OBJECTIVE_WORKSHOP},

    {BG_IC_GO_BANNER_DOCKS_A,         BG_IC_STATE_DOCKS_CONFLICT_H,      BG_IC_GO_BANNER_DOCKS_H_GREY,         BG_IC_OBJECTIVE_DOCKS},
    {BG_IC_GO_BANNER_DOCKS_H,         BG_IC_STATE_DOCKS_CONFLICT_A,      BG_IC_GO_BANNER_DOCKS_A_GREY,         BG_IC_OBJECTIVE_DOCKS},

    {BG_IC_GO_BANNER_HANGAR_A,        BG_IC_STATE_HANGAR_CONFLICT_H,     BG_IC_GO_BANNER_HANGAR_H_GREY,        BG_IC_OBJECTIVE_HANGAR},
    {BG_IC_GO_BANNER_HANGAR_H,        BG_IC_STATE_HANGAR_CONFLICT_A,     BG_IC_GO_BANNER_HANGAR_A_GREY,        BG_IC_OBJECTIVE_HANGAR},

    {BG_IC_GO_BANNER_REFINERY_A,      BG_IC_STATE_REFINERY_CONFLICT_H,   BG_IC_GO_BANNER_REFINERY_H_GREY,      BG_IC_OBJECTIVE_REFINERY},
    {BG_IC_GO_BANNER_REFINERY_H,      BG_IC_STATE_REFINERY_CONFLICT_A,   BG_IC_GO_BANNER_REFINERY_A_GREY,      BG_IC_OBJECTIVE_REFINERY},

    {BG_IC_GO_BANNER_QUARRY_A,        BG_IC_STATE_QUARRY_CONFLICT_H,     BG_IC_GO_BANNER_QUARRY_H_GREY,        BG_IC_OBJECTIVE_QUARY},
    {BG_IC_GO_BANNER_QUARRY_H,        BG_IC_STATE_QUARRY_CONFLICT_A,     BG_IC_GO_BANNER_QUARRY_A_GREY,        BG_IC_OBJECTIVE_QUARY},
};

struct IsleObjectiveBasicData
{
    uint8 objectiveId;
    uint32 message, graveyardId, spellEntry;
    // coords for Honorable Defender triggers
    float x, y, z;
};

// *** Battleground objective data *** //
static const IsleObjectiveBasicData isleObjectiveData[] =
{
    {BG_IC_OBJECTIVE_KEEP_ALLY,  LANG_BG_IC_NODE_KEEP_ALLIANCE, BG_IC_GRAVEYARD_ID_KEEP_ALLY,  0,                    349.3542f, -834.5278f, 48.999f},
    {BG_IC_OBJECTIVE_KEEP_HORDE, LANG_BG_IC_NODE_KEEP_HORDE,    BG_IC_GRAVEYARD_ID_KEEP_HORDE, 0,                    1218.795f, -765.212f,  48.916f},           // coords are guesswork for this line
    {BG_IC_OBJECTIVE_WORKSHOP,   LANG_BG_IC_NODE_WORKSHOP,      BG_IC_GRAVEYARD_ID_WORKSHOP,   0,                    776.229f,  -804.283f,  6.4505f},
    {BG_IC_OBJECTIVE_DOCKS,      LANG_BG_IC_NODE_DOCKS,         BG_IC_GRAVEYARD_ID_DOCKS,      0,                    726.5035f, -360.1875f, 17.8987f},
    {BG_IC_OBJECTIVE_HANGAR,     LANG_BG_IC_NODE_HANGAR,        BG_IC_GRAVEYARD_ID_HANGAR,     0,                    807.78f,   -1000.07f,  132.381f},
    {BG_IC_OBJECTIVE_REFINERY,   LANG_BG_IC_NODE_REFINERY,      0,                             BG_IC_SPELL_REFINERY, 1269.552f, -400.6875f, 37.7085f},
    {BG_IC_OBJECTIVE_QUARY,      LANG_BG_IC_NODE_QUARRY,        0,                             BG_IC_SPELL_QUARRY,   251.016f,  -1159.32f,  17.2376f}
};

struct IsleGraveyardSpawnData
{
    uint8 objectiveId;
    float x, y, z, o;
};

static const IsleGraveyardSpawnData isleGraveyardData[] =
{
    {BG_IC_OBJECTIVE_KEEP_ALLY,  278.42f,  -883.2f,   49.89f,  1.53f},
    {BG_IC_OBJECTIVE_KEEP_HORDE, 1300.91f, -834.04f,  48.91f,  1.69f},
    {BG_IC_OBJECTIVE_WORKSHOP,   775.74f,  -652.77f,  9.31f,   4.27f},
    {BG_IC_OBJECTIVE_DOCKS,      629.57f,  -279.83f,  11.33f,  0},
    {BG_IC_OBJECTIVE_HANGAR,     780.729f, -1103.08f, 135.51f, 2.27f},
};

enum IsleGates
{
    BG_IC_GATE_FRONT,
    BG_IC_GATE_WEST,
    BG_IC_GATE_EAST,

    BG_IC_MAX_KEEP_GATES
};

struct IsleWallsData
{
    uint8 gateId;
    uint32 eventId, stateClosed, stateOpened, message;
};

// *** Battleground walls data *** //
static const IsleWallsData isleAllianceWallsData[] =
{
    {BG_IC_GATE_FRONT, BG_IC_EVENT_ID_KEEP_BREACHED_A3, BG_IC_STATE_GATE_FRONT_A_CLOSED, BG_IC_STATE_GATE_FRONT_A_OPEN, LANG_BG_IC_FRONT_GATE_ALLIANCE},
    {BG_IC_GATE_WEST, BG_IC_EVENT_ID_KEEP_BREACHED_A1, BG_IC_STATE_GATE_WEST_A_CLOSED,  BG_IC_STATE_GATE_WEST_A_OPEN,  LANG_BG_IC_WEST_GATE_ALLIANCE},
    {BG_IC_GATE_EAST, BG_IC_EVENT_ID_KEEP_BREACHED_A2, BG_IC_STATE_GATE_EAST_A_CLOSED,  BG_IC_STATE_GATE_EAST_A_OPEN,  LANG_BG_IC_EAST_GATE_ALLIANCE}
};

static const IsleWallsData isleHordeWallsData[] =
{
    {BG_IC_GATE_FRONT, BG_IC_EVENT_ID_KEEP_BREACHED_H3, BG_IC_STATE_GATE_FRONT_H_CLOSED, BG_IC_STATE_GATE_FRONT_H_OPEN, LANG_BG_IC_FRONT_GATE_HORDE},
    {BG_IC_GATE_WEST, BG_IC_EVENT_ID_KEEP_BREACHED_H2, BG_IC_STATE_GATE_WEST_H_CLOSED,  BG_IC_STATE_GATE_WEST_H_OPEN,  LANG_BG_IC_WEST_GATE_HORDE},
    {BG_IC_GATE_EAST, BG_IC_EVENT_ID_KEEP_BREACHED_H1, BG_IC_STATE_GATE_EAST_H_CLOSED,  BG_IC_STATE_GATE_EAST_H_OPEN,  LANG_BG_IC_EAST_GATE_HORDE}
};

struct IsleNode
{
    // banner entries and timers
    uint32 oldBannerEntry;
    uint32 currentBannerEntry;
    uint32 bannerChangeTimer;

    // node state and change timer
    uint32 nodeChangeTimer;
    uint32 nodeWorldState;

    // owner and spirit healer
    PvpTeamIndex nodeOwner;                     // current node owner
    PvpTeamIndex nodeConquerer;                 // prev node owner; required if node is defended

    ObjectGuid spiritHealerGuid;
    ObjectGuid honorableDefenderGuid;
    ObjectGuid specialCreatureGuid;             // holder for special creatures

    GuidList creatureGuids;                     // stores the current creatures guids
};

class BattleGroundICScore : public BattleGroundScore
{
    public:
        BattleGroundICScore(): basesAssaulted(0), basesDefended(0) {};
        virtual ~BattleGroundICScore() {};

        uint32 GetAttr1() const { return basesAssaulted; }
        uint32 GetAttr2() const { return basesDefended; }

        uint32 basesAssaulted;
        uint32 basesDefended;
};

class BattleGroundIC : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundIC();
        void Reset() override;
        void Update(uint32 diff) override;

        // Main battleground functions
        void AddPlayer(Player* plr) override;
        void StartingEventOpenDoors() override;
        void EndBattleGround(Team winner) override;

        // General functions
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

        // Battleground event handlers
        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;
        bool HandleEvent(uint32 eventId, Object* source, Object* target) override;
        void HandleKillUnit(Creature* creature, Player* killer) override;
        void HandleKillPlayer(Player* player, Player* killer) override;
        void HandlePlayerClickedOnFlag(Player* player, GameObject* go) override;
        void HandleGameObjectDamaged(Player* player, GameObject* object, uint32 spellId) override;

        // Achievements
        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscvalue1) override;

    private:
        // Battleground objectives helper functions
        void DoApplyTeamBuff(PvpTeamIndex teamIdx, uint32 spellEntry, bool apply);
        void DoCaptureObjective(IsleObjective nodeId);
        void DoApplyObjectiveBenefits(IsleObjective nodeId, GameObject* objRef);
        void DoResetObjective(IsleObjective nodeId);
        void DoChangeBannerState(IsleObjective nodeId);
        void DoUpdateReinforcements(PvpTeamIndex teamIdx, int32 value);

        int32 GetReinforcements(PvpTeamIndex teamIdx);
        void SetReinforcements(PvpTeamIndex teamIdx, int32 reinforcements);

        IsleNode m_isleNode[BG_IC_MAX_OBJECTIVES];

        ObjectGuid m_keepGatesGuid[PVP_TEAM_COUNT][BG_IC_MAX_KEEP_GATES];
        ObjectGuid m_workshopMechanicGuids[PVP_TEAM_COUNT];
        ObjectGuid m_hordeInnerGateGuid;
        ObjectGuid m_allianceInnerGate1Guid;
        ObjectGuid m_allianceInnerGate2Guid;

        uint32 m_gatesAllianceState[BG_IC_MAX_KEEP_GATES];
        uint32 m_gatesHordeState[BG_IC_MAX_KEEP_GATES];
        uint32 m_resourceTickTimer[BG_IC_MAX_RESOURCE_NODES];
        uint32 m_closeDoorTimer;

        bool m_isKeepInvaded[PVP_TEAM_COUNT];

        std::set<uint32> m_hangarPortalsGuids[PVP_TEAM_COUNT];
        std::set<uint32> m_hangarAnimGuids[PVP_TEAM_COUNT];
        std::set<uint32> m_seaforiumBombsGuids[PVP_TEAM_COUNT];
        GuidList m_towerGatesGuids;
        GuidList m_teleporterGuids;
        GuidList m_teleporterAnimGuids;
        GuidList m_boatFiresGuids;
};
#endif
