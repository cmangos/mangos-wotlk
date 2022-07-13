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

#ifndef WORLD_PVP_WG
#define WORLD_PVP_WG

#include "Common.h"
#include "Battlefield.h"
#include "Tools/Language.h"
#include "World/World.h"

class Group;
class BattlefieldWG;

enum
{
    MAX_WG_WORKSHOPS                            = 4,                // refers to the capturable workshops
    MAX_WG_OFFENSE_TOWERS                       = 3,                // maximum towers available for the attacker team
    MAX_WG_CORPORAL_KILLS                       = 5,                // maximum number of kills required for rank promotion
    MAX_WG_LIEUTENANT_KILLS                     = 15,

    // ***** Spells *****
    // player rank spells
    SPELL_RECRUIT                               = 37795,            // player receives it by default on join
    SPELL_CORPORAL                              = 33280,            // 5 enemy kills - allowed for catapults
    SPELL_LIEUTENANT                            = 55629,            // 10 more enemy kills - allowed for all vehicles

    SPELL_TOWER_CONTROL                         = 62064,            // tower control buff
    // SPELL_SPIRITUAL_IMMUNITY                 = 58729,            // temporary protection after ressurection - handled by spirit healer

    // tenacity - used to balance the groups
    SPELL_TENACITY                              = 58549,            // received by the smaller team; the smaller the team, the more stacks it receives
    SPELL_TENACITY_VEHICLE                      = 59911,

    // tenacity reward spells - the exact tenacity stack count has to be confirmed
    // alliance version of honor spells
    SPELL_HONORABLE                             = 58553,            // +25 honor
    SPELL_GREAT_HONOR                           = 58555,            // +50 honor; received when the player has at least 5 stacks of tenacity
    SPELL_GREATER_HONOR                         = 58556,            // +75 honor; received when the player has more than 10 stacks of tenacity
    SPELL_GREATEST_HONOR                        = 58557,            // +100 honor; received when the player has 20 stacks of tenacity

    // horde versions of honor spells
    SPELL_LOKREGAR                              = 58554,            // +25 honor
    SPELL_LOKNARASH                             = 58558,            // +50 honor
    SPELL_LOKTAR                                = 58559,            // +75 honor
    SPELL_LOKTAR_OGAR                           = 58560,            // +100 honor

    SPELL_ALLIANCE_FLAG                         = 14268,            // used on vehicles
    SPELL_HORDE_FLAG                            = 14267,

    // SPELL_GRAB_PASSENGER                     = 61178,
    // SPELL_ACTIVATE_ROBOTIC_ARMS              = 49899,

    // reward spells
    SPELL_WINTERGRASP_VICTORY                   = 56902,            // add 3000 honor and a WG mark of honor
    SPELL_WINTERGRASP_DEFEAT                    = 58494,            // add 1250 honor and a WG mark of honor

    SPELL_DAMAGED_TOWER                         = 59135,            // honor award spells for specific actions; Require more research
    SPELL_DESTROYED_TOWER                       = 59136,
    SPELL_DAMAGED_BUILDING                      = 59201,
    SPELL_INTACT_BUILDING_DEFEND                = 59203,

    // misc spells
    // SPELL_WINTERGRASP_WATER                  = 36444,            // water damage against vehicles - applied automatically by the map
    SPELL_ESSENCE_WINTERGRASP_MAP               = 57940,            // map wide buff
    // SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA = 58730,            // no fly zone aura - Implemented in Player::UpdateArea()
    // SPELL_VICTORY_AURA_COSMETIC              = 60044,            // use unk
    SPELL_RULERS_OF_WINTERGRASP                 = 52108,            // increase damage buff
    SPELL_ACHIEV_LEAN_TOWER_DEFENSE             = 20723,            // achiev id 1727; server side spells
    SPELL_ACHIEV_LEAN_TOWER_OFFENSE             = 60676,            // achiev id 1727; server side spells

    // Phasing spells
    SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE       = 56617,            // phase 32 - workshop phase aura
    SPELL_HORDE_CONTROLS_FACTORY_PHASE          = 56618,            // phase 16 - workshop phase aura
    SPELL_ALLIANCE_CONTROL_PHASE                = 55774,            // phase 129 - zone phase aura
    SPELL_HORDE_CONTROL_PHASE                   = 55773,            // phase 65 - zone phase aura
    SPELL_ESSENCE_WINTERGRASP_ZONE              = 58045,            // phasing 257 - zone specific buff - allows the player to see the Greater Elemental Spirits

    // Spirit healer teleport spells - handled by dbscript on gossip
    // SPELL_TELEPORT_FORTRESS_GRAVEYARD        = 59760,
    // SPELL_TELEPORT_SUNKEN_RING               = 59762,
    // SPELL_TELEPORT_BROKEN_TEMPLE             = 59763,
    // SPELL_TELEPORT_HORDE_LANDING             = 59765,
    // SPELL_TELEPORT_WESTPARK_GRAVEYARD        = 59766,
    // SPELL_TELEPORT_EASTPARK_GRAVEYARD        = 59767,
    // SPELL_TELEPORT_ALLIANCE_LANDING          = 59769,

    // Teleport spells used during the battle
    // SPELL_TELEPORT_PORTAL                    = 54643,            // triggered from 54640; cast by the teleport object
    // SPELL_TELEPORT_VEHICLE                   = 49759,            // handled in wintergrasp scripts

    // Teleport spells - to and from Wintergrasp; used during the battle
    SPELL_TELEPORT_DALARAN                      = 53360,            // teleport back to Dalaran
    SPELL_TELEPORT_BRIDGE                       = 59096,            // description is that it teleports to the Fortress, but it's actually the bridge
    SPELL_TELEPORT_DALARAN_TO_WG                = 60035,            // teleports to the middle of the fortress

    // Dalaran to Wintergrasp teleport object
    // SPELL_TELEPORT_WINTERGRASP               = 58622,            // source spell; used by go 193772; teleports the player to WG based on the battlefield status
    SPELL_TELEPORT_WINTERGRASP_ALLIANCE         = 58633,            // to alliance landing
    SPELL_TELEPORT_WINTERGRASP_HORDE            = 58632,            // to horde landing
    SPELL_TELEPORT_WINTERGRASP_FORTRESS         = 58681,            // teleport in front of the Fortress' door

    // Vehicle build spells - handled by dbscript on gossip
    // SPELL_BUILD_CATAPULT_FORCE               = 56664,
    // SPELL_BUILD_DEMOLISHER_FORCE             = 56659,
    // SPELL_BUILD_SIEGE_VEHICLE_A_FORCE        = 56662,
    // SPELL_BUILD_SIEGE_VEHICLE_H_FORCE        = 61409,


    // ***** Quests *****
    QUEST_ID_VICTORY_IN_WINTERGRASP_A           = 13181,
    QUEST_ID_VICTORY_IN_WINTERGRASP_H           = 13183,

    // ***** Kill credits *****
    NPC_QUEST_CREDIT_KILL_VEHICLE               = 31093,            // kill enemy vehicle
    NPC_QUEST_CREDIT_DEFEND_VEHICLE             = 31284,
    NPC_QUEST_CREDIT_KILL_ALLIANCE              = 31086,
    // NPC_QUEST_CREDIT_KILL_TOWER              = 31156,
    // NPC_QUEST_CREDIT_KILL_STRUCTURE          = 31244,
    // NPC_QUEST_CREDIT_KILL_BRIDGE             = 31286,
    // NPC_QUEST_CREDIT_KILL_WALL               = 31287,
    // NPC_QUEST_CREDIT_KILL_WORKSHOP           = 31288,
    // NPC_QUEST_CREDIT_KILL_GATE               = 31289,
    NPC_QUEST_CREDIT_KILL_SOUTHERN_TOWER        = 35074,            // destroy any of the 3 southern towers
    NPC_QUEST_CREDIT_KILL_HORDE                 = 39019,


    // ***** Achiev criterias *****
    // ACHIV_CRITERIA_WIN_10_MINUTES            = 7666,


    // ***** Area ids *****
    AREA_ID_THE_SUNKEN_RING                     = 4538,
    AREA_ID_THE_BROKEN_TEMPLE                   = 4539,
    // AREA_ID_WINTERGRASP_FORTRESS             = 4575,
    // AREA_ID_CENTRAL_BRIDGE                   = 4576,
    // AREA_ID_EASTERN_BRIDGE                   = 4577,
    // AREA_ID_WESTERN_BRIDGE                   = 4578,
    // AREA_ID_FLAMEWATCH_TOWER                 = 4581,
    // AREA_ID_WINTERS_EDGE_TOWER               = 4582,
    // AREA_ID_SHADOWSIGHT_TOWER                = 4583,
    // AREA_ID_THE_CAULDRON_OF_FLAMES           = 4584,
    // AREA_ID_GLACIAL_FALLS                    = 4585,
    // AREA_ID_WINDY_BLUFFS                     = 4586,
    // AREA_ID_THE_FOREST_OF_SHADOWS            = 4587,
    // AREA_ID_THE_CHILLED_QUAGMIRE             = 4589,
    // AREA_ID_THE_STEPPE_OF_LIFE               = 4590,
    AREA_ID_WESTPARK_WORKSHOP                   = 4611,
    AREA_ID_EASTPARK_WORKSHOP                   = 4612,

    // ***** Creatures *****
    // ally creatures and vendors
    NPC_ANCHORITE_TESSA                         = 31054,
    NPC_COMMANDER_ZANNETH                       = 31036,
    NPC_SENIOR_DEMOLITIONIST_LEGOSO             = 31109,
    NPC_SIEGE_MASTER_STOUTHANDLE                = 31108,
    NPC_SORCERESS_KAYLANA                       = 31051,
    NPC_TACTICAL_OFFICER_AHBRAMIS               = 31153,
    NPC_BOWYER_RANDOLPH                         = 31052,
    NPC_KNIGHT_DAMERON                          = 32294,                // phased; available for battlefield defender only
    NPC_MARSHAL_MAGRUDER                        = 39172,
    NPC_MORGAN_DAY                              = 30489,
    NPC_TRAVIS_DAY                              = 30488,
    NPC_ALLIANCE_BRIGADIER_GENERAL              = 32626,

    // horde creatures
    NPC_COMMANDER_DARDOSH                       = 31091,
    NPC_HOODOO_MASTER_FUJIN                     = 31101,
    NPC_LEUTENANT_MURP                          = 31107,
    NPC_PRIMALIST_MULFORT                       = 31053,
    NPC_SIEGESMITH_STRONGHOOF                   = 31106,
    NPC_TACTICAL_OFFICER_KILRATH                = 31151,
    NPC_VIERON_BLAZEFEATHER                     = 31102,
    NPC_STONE_GUARD_MUKAR                       = 32296,                // phased; available for battlefield defender only
    NPC_CHAMPION_ROSSLAI                        = 39173,
    NPC_HORDE_WARBRINGER                        = 32615,

    // Alliance and horde guards - phased and with auto respawn of 3 min
    NPC_VALLIANCE_EXPEDITION_CHAMPION           = 30740,
    NPC_VALLIANCE_EXPEDITION_GUARD              = 32308,                // no spawns found
    NPC_WARSONG_CHAMPION                        = 30739,
    NPC_WARSONG_GUARD                           = 32307,                // no spawns found

    // spirit guides
    // NPC_SPIRIT_GUIDE_ALLIANCE                = 31842,                // they have aura 58729
    // NPC_SPIRIT_GUIDE_HORDE                   = 31841,

    // Note: revenants phase 256; this makes them visible only for battlefield winner; handled by DB
    // the gnome / goblin mechanics are also phased based on factory owner; handled by DB

    // PvP related npcs and vehicles
    NPC_WINTERGRASP_CATAPULT                    = 27881,
    NPC_WINTERGRASP_DEMOLISHER                  = 28094,
    NPC_WINTERGRASP_SIEGE_ENGINE_A              = 28312,                // has accessory 28319
    NPC_WINTERGRASP_SIEGE_ENGINE_H              = 32627,                // has accessory 32629
    // NPC_WINTERGRASP_SIEGE_TURRET_A           = 28319,
    // NPC_WINTERGRASP_SIEGE_TURRET_H           = 32629,
    NPC_WINTERGRASP_TOWER_CANNON                = 28366,

    // NPC_RP_GG                                = 30560,            // rocket launcher. picked from the ground
    // NPC_CONTROL_ARMS                         = 27852,            // has some animation during vehicle summmon

    // dummy npcs / helpers
    NPC_INVISIBLE_STALKER                       = 15214,            // sends raid emotes
    NPC_WINTERGRASP_DETECTION_UNIT              = 27869,            // use unk; has unk aura 57577; loaded as active object in order to keep the grid active

    // lesser trash mobs; have to be manually despawned during the battle; The Revenants are phased for the owner, so they don't need to be despawned
    NPC_WANDERING_SHADOW                        = 30842,
    NPC_LIVING_LASHER                           = 30845,
    NPC_GLACIAL_SPIRIT                          = 30846,
    NPC_RAGING_FLAME                            = 30847,
    NPC_WHISPERING_WIND                         = 30848,
    NPC_CHILLED_EARTH_ELEMENTAL                 = 30849,


    // ***** Gameobjects *****
    // titan relics (goobers)
    GO_TITAN_RELIC_ALLIANCE                     = 192834,           // both send event id 22097
    GO_TITAN_RELIC_HORDE                        = 192829,

    // fortress doors (literally door objects)
    GO_WG_FORTRESS_COLLISION_DOOR               = 194162,           // invisible door - needs to be manually open when the real door is destroyed
    GO_WG_FORTRESS_COLLISION_WALL               = 194323,

    // *** Destructible objects ***
    // fortress walls
    GO_WG_FORTRESS_DOOR                         = 191810,
    GO_WG_FORTRESS_GATE                         = 190375,
    GO_WG_FORTRESS_WALL_1                       = 190220,
    GO_WG_FORTRESS_WALL_2                       = 190219,
    GO_WG_FORTRESS_WALL_3                       = 190369,
    GO_WG_FORTRESS_WALL_4                       = 190371,
    GO_WG_FORTRESS_WALL_5                       = 190374,
    GO_WG_FORTRESS_WALL_6                       = 190376,
    GO_WG_FORTRESS_WALL_7                       = 190370,
    GO_WG_FORTRESS_WALL_8                       = 190372,
    GO_WG_FORTRESS_WALL_9                       = 191799,
    GO_WG_FORTRESS_WALL_10                      = 191809,
    GO_WG_FORTRESS_WALL_11                      = 191806,
    GO_WG_FORTRESS_WALL_12                      = 191808,
    GO_WG_FORTRESS_WALL_13                      = 191801,
    GO_WG_FORTRESS_WALL_14                      = 191807,
    GO_WG_FORTRESS_WALL_15                      = 191796,
    GO_WG_FORTRESS_WALL_16                      = 191800,
    GO_WG_FORTRESS_WALL_17                      = 191804,
    GO_WG_FORTRESS_WALL_18                      = 191795,
    GO_WG_FORTRESS_WALL_19                      = 191803,
    GO_WG_FORTRESS_WALL_20                      = 191802,
    GO_WG_WALL_1                                = 191798,
    GO_WG_WALL_2                                = 191805,
    GO_WG_WALL_3                                = 191797,

    // fortress towers
    GO_WG_TOWER_WEST                            = 190221,
    GO_WG_TOWER_SOUTHWEST                       = 190373,
    GO_WG_TOWER_SOUTHEAST                       = 190377,
    GO_WG_TOWER_EAST                            = 190378,

    // attackers towers
    GO_TOWER_SHADOWSIGHT                        = 190356,
    GO_TOWER_WINTERS_EDGE                       = 190357,
    GO_TOWER_FLAMEWATCH                         = 190358,

    // workshops - friendly buildings; cannot be destoryed (as of 3.2)
    GO_WORKSHOP_KEEP_WEST                       = 192028,
    GO_WORKSHOP_KEEP_EAST                       = 192029,
    GO_WORKSHOP_BROKEN_TEMPLE                   = 192030,
    GO_WORKSHOP_SUNKEN_RING                     = 192031,
    GO_WORKSHOP_WESTPARK                        = 192032,
    GO_WORKSHOP_EASTPARK                        = 192033,

    // *** Capture points ***
    // each faction has a different starting value for the capture point; the objects are phased based on the zone defender
    // each object pair share the same event id
    GO_CAPTUREPOINT_BROKEN_TEMPLE_A             = 192627,
    GO_CAPTUREPOINT_BROKEN_TEMPLE_H             = 190487,
    GO_CAPTUREPOINT_WESTPARK_A                  = 194962,
    GO_CAPTUREPOINT_WESTPARK_H                  = 194963,
    GO_CAPTUREPOINT_EASTPARK_A                  = 194959,
    GO_CAPTUREPOINT_EASTPARK_H                  = 194960,
    GO_CAPTUREPOINT_SUNKEN_RING_A               = 192626,
    GO_CAPTUREPOINT_SUNKEN_RING_H               = 190475,
    // note: wintergrasp flag banners are phased based on battlefield or factory owner; handled by DB

    // portal GOs
    // GO_DEFENDERS_PORTAL_HORDE                = 190763,               // generic faction portals around the fortress
    // GO_DEFENDERS_PORTAL_ALLIANCE             = 191575,

    // GO_DEFENDERS_PORTAL_GENERIC              = 192819,               // portal inside the fortress
    // GO_VEHICLE_TELEPORTER                    = 192951,               // generic object; the actual teleport is handled in a different way
    // GO_PORTAL_TO_WINTERGRASP                 = 193772,               // portal from dalaran to WG

    // banners
    GO_WINTERGRASP_BANNER_DEFENSE_ALLIANCE      = 192487,               // banners that have to be despawned when object is damaged or destroyed
    GO_WINTERGRASP_BANNER_DEFENSE_HORDE         = 192488,
    GO_WINTERGRASP_BANNER_OFFENSE_ALLIANCE      = 192501,
    GO_WINTERGRASP_BANNER_OFFENSE_HORDE         = 192502,

    // note: wintergrasp portal objects are phased based on battlefield defender; handled by DB


    // ***** Graveyards *****
    GRAVEYARD_ID_KEEP_EAST                      = 1285,
    GRAVEYARD_ID_KEEP_WEST                      = 1328,
    GRAVEYARD_ID_SUNKEN_RING                    = 1329,
    GRAVEYARD_ID_BROKEN_TEMPLE                  = 1330,
    GRAVEYARD_ID_EASTPARK                       = 1333,
    GRAVEYARD_ID_WESTPARK                       = 1334,
    GRAVEYARD_ID_FORTRESS_INDOOR                = 1474,                 // fortress indoor graveyard id; inactive during the battle


    // ***** Events *****
    // destructible building events are defined below - too many to add here
    // EVENT_TITAN_RELIC                        = 22097,                // event sent by the Titan relic - handled by HandleGameObjectUse
    EVENT_KEEP_DOOR_DESTROY                     = 19607,

    // capture points events (identical between horde and alliance GO versions) - defined in wgCapturePointEventData
    EVENT_BROKEN_TEMPLE_CONTEST_ALLIANCE        = 20787,                // horde -> neutral
    EVENT_BROKEN_TEMPLE_CONTEST_HORDE           = 20788,                // alliance -> neutral
    EVENT_BROKEN_TEMPLE_PROGRESS_ALLIANCE       = 19612,                // neutral -> alliance
    EVENT_BROKEN_TEMPLE_PROGRESS_HORDE          = 19611,                // neutral -> horde

    EVENT_WESTPARK_CONTEST_ALLIANCE             = 21568,
    EVENT_WESTPARK_CONTEST_HORDE                = 21569,
    EVENT_WESTPARK_PROGRESS_ALLIANCE            = 21562,
    EVENT_WESTPARK_PROGRESS_HORDE               = 21560,

    EVENT_EASTPARK_CONTEST_ALLIANCE             = 21566,
    EVENT_EASTPARK_CONTEST_HORDE                = 21567,
    EVENT_EASTPARK_PROGRESS_ALLIANCE            = 21565,
    EVENT_EASTPARK_PROGRESS_HORDE               = 21563,

    EVENT_SUNKEN_RING_CONTEST_ALLIANCE          = 20785,
    EVENT_SUNKEN_RING_CONTEST_HORDE             = 20786,
    EVENT_SUNKEN_RING_PROGRESS_ALLIANCE         = 19610,
    EVENT_SUNKEN_RING_PROGRESS_HORDE            = 19609,

    // ***** world states *****
    // generic world states
    WORLD_STATE_WG_VEHICLE_H                    = 3490,
    WORLD_STATE_WG_MAX_VEHICLE_H                = 3491,
    WORLD_STATE_WG_VEHICLE_A                    = 3680,
    WORLD_STATE_WG_MAX_VEHICLE_A                = 3681,
    WORLD_STATE_WG_SHOW_BATTLE                  = 3710,
    WORLD_STATE_WG_SHOW_COOLDOWN                = 3801,
    WORLD_STATE_WG_HORDE_DEFENDER               = 3802,
    WORLD_STATE_WG_ALLIANCE_DEFENDER            = 3803,

    // time world states
    WORLD_STATE_WG_TIME_TO_END                  = 3781,
    WORLD_STATE_WG_TIME_TO_NEXT_BATTLE          = 4354,

    // keep walls
    WORLD_STATE_WG_FORTRESS_DOOR                = 3773,
    WORLD_STATE_WG_FORTRESS_GATE                = 3763,
    WORLD_STATE_WG_FORTRESS_WALL_1              = 3750,
    WORLD_STATE_WG_FORTRESS_WALL_2              = 3749,
    WORLD_STATE_WG_FORTRESS_WALL_3              = 3753,
    WORLD_STATE_WG_FORTRESS_WALL_4              = 3754,
    WORLD_STATE_WG_FORTRESS_WALL_5              = 3755,
    WORLD_STATE_WG_FORTRESS_WALL_6              = 3756,
    WORLD_STATE_WG_FORTRESS_WALL_7              = 3758,
    WORLD_STATE_WG_FORTRESS_WALL_8              = 3757,
    WORLD_STATE_WG_FORTRESS_WALL_9              = 3762,
    WORLD_STATE_WG_FORTRESS_WALL_10             = 3761,
    WORLD_STATE_WG_FORTRESS_WALL_11             = 3769,
    WORLD_STATE_WG_FORTRESS_WALL_12             = 3760,
    WORLD_STATE_WG_FORTRESS_WALL_13             = 3770,
    WORLD_STATE_WG_FORTRESS_WALL_14             = 3759,
    WORLD_STATE_WG_FORTRESS_WALL_15             = 3772,
    WORLD_STATE_WG_FORTRESS_WALL_16             = 3766,
    WORLD_STATE_WG_FORTRESS_WALL_17             = 3767,
    WORLD_STATE_WG_FORTRESS_WALL_18             = 3764,
    WORLD_STATE_WG_FORTRESS_WALL_19             = 3752,
    WORLD_STATE_WG_FORTRESS_WALL_20             = 3751,
    WORLD_STATE_WG_WALL_1                       = 3771,
    WORLD_STATE_WG_WALL_2                       = 3768,
    WORLD_STATE_WG_WALL_3                       = 3765,

    // keep towers
    WORLD_STATE_WG_KEEP_TOWER_WEST              = 3711,
    WORLD_STATE_WG_KEEP_TOWER_SOUTHWEST         = 3713,
    WORLD_STATE_WG_KEEP_TOWER_SOUTHEAST         = 3714,
    WORLD_STATE_WG_KEEP_TOWER_EAST              = 3712,

    // attacker towers
    WORLD_STATE_WG_TOWER_SHADOWSIGHT            = 3704,
    WORLD_STATE_WG_TOWER_WINTERS_EDGE           = 3705,
    WORLD_STATE_WG_TOWER_FLAMEWATCH             = 3706,

    // workshops
    WORLD_STATE_WG_WORKSHOP_SUNKEN_RING         = 3701,
    WORLD_STATE_WG_WORKSHOP_BROKEN_TEMPLE       = 3700,
    WORLD_STATE_WG_WORKSHOP_EASTPARK            = 3703,
    WORLD_STATE_WG_WORKSHOP_WESTPARK            = 3702,
    WORLD_STATE_WG_WORKSHOP_KEEP_WEST           = 3698,
    WORLD_STATE_WG_WORKSHOP_KEEP_EAST           = 3699,


    // ***** Sound ids *****
    SOUND_ID_WG_HORDE_WINS                      = 8454,
    SOUND_ID_WG_ALLIANCE_WINS                   = 8455,
    // SOUND_ID_WG_HORDE_WARNING                = 8457,
    // SOUND_ID_WG_ALLIANCE_WARNING             = 8456,
    // SOUND_ID_WG_ENTER_QUEUE                  = 8458,             // also 8462
    // SOUND_ID_WG_ENTER_THROUGH                = 8459,             // also 8463
    // SOUND_ID_WG_START                        = 3439,             // to be confirmed

    // ***** NPC Factions *****
    // FACTION_ID_WARSONG                       = 1979,
    // FACTION_ID_VALLIANCE                     = 1891,

    FACTION_ID_ALLIANCE_GENERIC                 = 1732,             // used by GOs and creatures
    // FACTION_ID_ALLIANCE_GENERIC_2            = 1733,             // ToDo: to be confirmed
    // FACTION_ID_HORDE_GENERIC_2               = 1734,             // ToDo: to be confirmed
    FACTION_ID_HORDE_GENERIC                    = 1735,             // used by GOs and creatures

    // ***** Achiev criterias and achiev spells *****
    ACHIEV_CRIT_WG_NO_CHANCE                    = 7703,             // achiev id 1751
    ACHIEV_CRIT_WG_VEHICULER_SLAUGHTER          = 7704,             // achiev id 1723
    // ACHEIV_CRIT_WG_RANGER                    = 7709,             // achiev id 2199
    ACHIEV_CRIT_WG_WITHIN_GRASP                 = 7666,             // achiev id 1755
    ACHIEV_CRIT_VEH_SLAUGHTER_CANNON            = 7704,             // achiev id 1723
    ACHIEV_CRIT_VEH_SLAUGHTER_SIEGE_A           = 7705,
    ACHIEV_CRIT_VEH_SLAUGHTER_SIEGE_H           = 7706,
    ACHIEV_CRIT_VEH_SLAUGHTER_DEMOLISHER        = 7707,
    ACHIEV_CRIT_VEH_SLAUGHTER_CATAPULT          = 7708,

    // ***** Condition entries *****
    // used to check the gossip options in DB for various npcs
    OPVP_COND_WG_MAX_ALLIANCE_VEHICLES          = 0,                // check if player is allowed to build more vehicles
    OPVP_COND_WG_MAX_HORDE_VEHICLES             = 1,
    OPVP_COND_WG_BATTLEFIELD_IN_PROGRESS        = 2,                // check if battlefield is in progress
    OPVP_COND_WG_FORTRESS_ACCESS_ALLOWED        = 3,                // check if player is allowed to enter or teleport inside fortress
    OPVP_COND_WG_ALLOW_TELE_SUNKEN_RING         = 4,
    OPVP_COND_WG_ALLOW_TELE_BROKEN_TEMPLE       = 5,
    OPVP_COND_WG_ALLOW_TELE_EASTPARK            = 6,
    OPVP_COND_WG_ALLOW_TELE_WESTPARK            = 7,
};

// *** Battlefield factions *** //
const uint32 wgTeamFactions[PVP_TEAM_COUNT] = { FACTION_ID_ALLIANCE_GENERIC, FACTION_ID_HORDE_GENERIC };

// *** Battlefield phasing auras *** //
const uint32 wgTeamControlAuras[PVP_TEAM_COUNT] = { SPELL_ALLIANCE_CONTROL_PHASE, SPELL_HORDE_CONTROL_PHASE };
const uint32 wgTeamFactoryAuras[PVP_TEAM_COUNT] = { SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE, SPELL_HORDE_CONTROLS_FACTORY_PHASE };

struct WintergraspGoData
{
    uint32 goEntry, worldState, eventIntact, eventDamaged, eventDestroyed, eventRebuild, messageDamaged, messagedDestroyed, graveyardId;
};

// *** Fortress data *** //
static const WintergraspGoData wgFortressData[] =
{
    {GO_WG_FORTRESS_DOOR,       WORLD_STATE_WG_FORTRESS_DOOR,           19955,  19448,  19607,  19448},
    {GO_WG_FORTRESS_GATE,       WORLD_STATE_WG_FORTRESS_GATE,           19958,  19956,  19957,  0},
    {GO_WG_FORTRESS_WALL_1,     WORLD_STATE_WG_FORTRESS_WALL_1,         19883,  19897,  19911,  19447},
    {GO_WG_FORTRESS_WALL_2,     WORLD_STATE_WG_FORTRESS_WALL_2,         19882,  19896,  19910,  19447},
    {GO_WG_FORTRESS_WALL_3,     WORLD_STATE_WG_FORTRESS_WALL_3,         19886,  19900,  19914,  19447},
    {GO_WG_FORTRESS_WALL_4,     WORLD_STATE_WG_FORTRESS_WALL_4,         19887,  19901,  19915,  19447},
    {GO_WG_FORTRESS_WALL_5,     WORLD_STATE_WG_FORTRESS_WALL_5,         19888,  19902,  19916,  19447},
    {GO_WG_FORTRESS_WALL_6,     WORLD_STATE_WG_FORTRESS_WALL_6,         19889,  19903,  19917,  19447},
    {GO_WG_FORTRESS_WALL_7,     WORLD_STATE_WG_FORTRESS_WALL_7,         19891,  19905,  19919,  19447},
    {GO_WG_FORTRESS_WALL_8,     WORLD_STATE_WG_FORTRESS_WALL_8,         19890,  19904,  19918,  19447},
    {GO_WG_FORTRESS_WALL_9,     WORLD_STATE_WG_FORTRESS_WALL_9,         19895,  19909,  19923,  19447},
    {GO_WG_FORTRESS_WALL_10,    WORLD_STATE_WG_FORTRESS_WALL_10,        19894,  19908,  19922,  19447},
    {GO_WG_FORTRESS_WALL_11,    WORLD_STATE_WG_FORTRESS_WALL_11,        19929,  19938,  19947,  19447},
    {GO_WG_FORTRESS_WALL_12,    WORLD_STATE_WG_FORTRESS_WALL_12,        19893,  19907,  19921,  19447},
    {GO_WG_FORTRESS_WALL_13,    WORLD_STATE_WG_FORTRESS_WALL_13,        19930,  19939,  19948,  19447},
    {GO_WG_FORTRESS_WALL_14,    WORLD_STATE_WG_FORTRESS_WALL_14,        19892,  19906,  19920,  19447},
    {GO_WG_FORTRESS_WALL_15,    WORLD_STATE_WG_FORTRESS_WALL_15,        19932,  19941,  19950,  19447},
    {GO_WG_FORTRESS_WALL_16,    WORLD_STATE_WG_FORTRESS_WALL_16,        19926,  19935,  19944,  19447},
    {GO_WG_FORTRESS_WALL_17,    WORLD_STATE_WG_FORTRESS_WALL_17,        19927,  19936,  19945,  19447},
    {GO_WG_FORTRESS_WALL_18,    WORLD_STATE_WG_FORTRESS_WALL_18,        19924,  19933,  19942,  19447},
    {GO_WG_FORTRESS_WALL_19,    WORLD_STATE_WG_FORTRESS_WALL_19,        19885,  19899,  19913,  19447},
    {GO_WG_FORTRESS_WALL_20,    WORLD_STATE_WG_FORTRESS_WALL_20,        19884,  19898,  19912,  19447},
    {GO_WG_WALL_1,              WORLD_STATE_WG_WALL_1,                  19931,  19940,  19949,  0},
    {GO_WG_WALL_2,              WORLD_STATE_WG_WALL_2,                  19928,  19937,  19946,  0},
    {GO_WG_WALL_3,              WORLD_STATE_WG_WALL_3,                  19925,  19934,  19943,  0},
};

// *** Defense towers data *** //
static const WintergraspGoData wgFortressTowersData[] =
{
    {GO_WG_TOWER_WEST,          WORLD_STATE_WG_KEEP_TOWER_WEST,         19665,  19657,  19661,  19460,  LANG_OPVP_WG_NW_KEEP_TOWER_DAMAGE,  LANG_OPVP_WG_NW_KEEP_TOWER_DESTROY},
    {GO_WG_TOWER_SOUTHWEST,     WORLD_STATE_WG_KEEP_TOWER_SOUTHWEST,    19667,  19659,  19662,  19459,  LANG_OPVP_WG_SW_KEEP_TOWER_DAMAGE,  LANG_OPVP_WG_SW_KEEP_TOWER_DESTROY},
    {GO_WG_TOWER_SOUTHEAST,     WORLD_STATE_WG_KEEP_TOWER_SOUTHEAST,    19668,  19660,  19664,  19457,  LANG_OPVP_WG_SE_KEEP_TOWER_DAMAGE,  LANG_OPVP_WG_SE_KEEP_TOWER_DESTROY},
    {GO_WG_TOWER_EAST,          WORLD_STATE_WG_KEEP_TOWER_EAST,         19666,  19658,  19663,  19458,  LANG_OPVP_WG_NE_KEEP_TOWER_DAMAGE,  LANG_OPVP_WG_NE_KEEP_TOWER_DESTROY},
};

// *** Attack towers data *** //
static const WintergraspGoData wgOffenseData[] =
{
    {GO_TOWER_SHADOWSIGHT,      WORLD_STATE_WG_TOWER_SHADOWSIGHT,       19671,  19674,  19677,  19680,  LANG_OPVP_WG_WEST_TOWER_DAMAGE,     LANG_OPVP_WG_WEST_TOWER_DESTROY},
    {GO_TOWER_WINTERS_EDGE,     WORLD_STATE_WG_TOWER_WINTERS_EDGE,      19670,  19673,  19676,  19679,  LANG_OPVP_WG_SOUTH_TOWER_DAMAGE,    LANG_OPVP_WG_SOUTH_TOWER_DESTROY},
    {GO_TOWER_FLAMEWATCH,       WORLD_STATE_WG_TOWER_FLAMEWATCH,        19669,  19672,  19675,  19678,  LANG_OPVP_WG_EAST_TOWER_DAMAGE,     LANG_OPVP_WG_EAST_TOWER_DESTROY},
};

// *** Fortress Workshop data *** //
static const WintergraspGoData wgFortressWorkshopsData[] =
{
    {GO_WORKSHOP_KEEP_WEST,     WORLD_STATE_WG_WORKSHOP_KEEP_WEST,      19790,  19782,  19786,  19794, 0, 0, GRAVEYARD_ID_KEEP_WEST},
    {GO_WORKSHOP_KEEP_EAST,     WORLD_STATE_WG_WORKSHOP_KEEP_EAST,      19791,  19783,  19787,  19795, 0, 0, GRAVEYARD_ID_KEEP_EAST},
};

// *** Middle-ground Workshop data *** //
static const WintergraspGoData wgDefendeWorkshopsData[] =
{
    {GO_WORKSHOP_BROKEN_TEMPLE, WORLD_STATE_WG_WORKSHOP_BROKEN_TEMPLE,  19775,  19777,  19779,  19781},
    {GO_WORKSHOP_SUNKEN_RING,   WORLD_STATE_WG_WORKSHOP_SUNKEN_RING,    19774,  19776,  19778,  19780},
};

// *** Attacker Workshop data *** //
static const WintergraspGoData wgOffensiveWorkshopsData[] =
{
    {GO_WORKSHOP_WESTPARK,      WORLD_STATE_WG_WORKSHOP_WESTPARK,       19792,  19784,  19788,  19796},
    {GO_WORKSHOP_EASTPARK,      WORLD_STATE_WG_WORKSHOP_EASTPARK,       19793,  19785,  19789,  19797},
};

// *** Battlefield capture point visuals *** //
const CapturePointArtKits wgCapturePointArtKits[PVP_TEAM_COUNT] = { CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE };
const CapturePointAnimations wgCapturePointAnims[PVP_TEAM_COUNT] = { CAPTURE_ANIM_ALLIANCE, CAPTURE_ANIM_HORDE };

// *** Capture point data *** //
static const uint32 wgCapturePoints[PVP_TEAM_COUNT][MAX_WG_WORKSHOPS] =
{
    {GO_CAPTUREPOINT_SUNKEN_RING_A, GO_CAPTUREPOINT_BROKEN_TEMPLE_A, GO_CAPTUREPOINT_EASTPARK_A, GO_CAPTUREPOINT_WESTPARK_A},
    {GO_CAPTUREPOINT_SUNKEN_RING_H, GO_CAPTUREPOINT_BROKEN_TEMPLE_H, GO_CAPTUREPOINT_EASTPARK_H, GO_CAPTUREPOINT_WESTPARK_H},
};

struct WintergraspCapturePointData
{
    uint32 goEntryAlliance, goEntryHorde, workshopEntry, areaId, worldState, graveyardId, conditionId;
};

static const WintergraspCapturePointData wgCapturePointData[MAX_WG_WORKSHOPS] =
{
    {GO_CAPTUREPOINT_SUNKEN_RING_A,     GO_CAPTUREPOINT_SUNKEN_RING_H,      GO_WORKSHOP_SUNKEN_RING,     AREA_ID_THE_SUNKEN_RING,    WORLD_STATE_WG_WORKSHOP_SUNKEN_RING,   GRAVEYARD_ID_SUNKEN_RING,   OPVP_COND_WG_ALLOW_TELE_SUNKEN_RING},
    {GO_CAPTUREPOINT_BROKEN_TEMPLE_A,   GO_CAPTUREPOINT_BROKEN_TEMPLE_H,    GO_WORKSHOP_BROKEN_TEMPLE,   AREA_ID_THE_BROKEN_TEMPLE,  WORLD_STATE_WG_WORKSHOP_BROKEN_TEMPLE, GRAVEYARD_ID_BROKEN_TEMPLE, OPVP_COND_WG_ALLOW_TELE_BROKEN_TEMPLE},
    {GO_CAPTUREPOINT_EASTPARK_A,        GO_CAPTUREPOINT_EASTPARK_H,         GO_WORKSHOP_EASTPARK,        AREA_ID_EASTPARK_WORKSHOP,  WORLD_STATE_WG_WORKSHOP_EASTPARK,      GRAVEYARD_ID_EASTPARK,      OPVP_COND_WG_ALLOW_TELE_EASTPARK},
    {GO_CAPTUREPOINT_WESTPARK_A,        GO_CAPTUREPOINT_WESTPARK_H,         GO_WORKSHOP_WESTPARK,        AREA_ID_WESTPARK_WORKSHOP,  WORLD_STATE_WG_WORKSHOP_WESTPARK,      GRAVEYARD_ID_WESTPARK,      OPVP_COND_WG_ALLOW_TELE_WESTPARK},
};

struct WintergraspCapturePointEventData
{
    uint32  eventEntry;
    Team    team;
    uint32  defenseMessage;
    BattlefieldGoState  objectState;
    uint32  bannerArtKit;
    uint32  bannerAnim;
};

static const WintergraspCapturePointEventData wgCapturePointEventData[MAX_WG_WORKSHOPS][4] =
{
    {
        {EVENT_SUNKEN_RING_CONTEST_ALLIANCE,        HORDE,          LANG_OPVP_WG_SUNKEN_RING_ATTACK_A,      BF_GO_STATE_HORDE_INTACT,       0,  0},
        {EVENT_SUNKEN_RING_CONTEST_HORDE,           ALLIANCE,       LANG_OPVP_WG_SUNKEN_RING_ATTACK_H,      BF_GO_STATE_ALLIANCE_INTACT,    0,  0},
        {EVENT_SUNKEN_RING_PROGRESS_ALLIANCE,       ALLIANCE,       LANG_OPVP_WG_SUNKEN_RING_CAPTURE_A,     BF_GO_STATE_ALLIANCE_INTACT,    CAPTURE_ARTKIT_ALLIANCE,    CAPTURE_ANIM_ALLIANCE},
        {EVENT_SUNKEN_RING_PROGRESS_HORDE,          HORDE,          LANG_OPVP_WG_SUNKEN_RING_CAPTURE_H,     BF_GO_STATE_HORDE_INTACT,       CAPTURE_ARTKIT_HORDE,       CAPTURE_ANIM_HORDE},
    },
    {
        {EVENT_BROKEN_TEMPLE_CONTEST_ALLIANCE,      HORDE,          LANG_OPVP_WG_BR_TEMPLE_ATTACK_A,        BF_GO_STATE_HORDE_INTACT,       0,  0},
        {EVENT_BROKEN_TEMPLE_CONTEST_HORDE,         ALLIANCE,       LANG_OPVP_WG_BR_TEMPLE_ATTACK_H,        BF_GO_STATE_ALLIANCE_INTACT,    0,  0},
        {EVENT_BROKEN_TEMPLE_PROGRESS_ALLIANCE,     ALLIANCE,       LANG_OPVP_WG_BR_TEMPLE_CAPTURE_A,       BF_GO_STATE_ALLIANCE_INTACT,    CAPTURE_ARTKIT_ALLIANCE,    CAPTURE_ANIM_ALLIANCE},
        {EVENT_BROKEN_TEMPLE_PROGRESS_HORDE,        HORDE,          LANG_OPVP_WG_BR_TEMPLE_CAPTURE_H,       BF_GO_STATE_HORDE_INTACT,       CAPTURE_ARTKIT_HORDE,       CAPTURE_ANIM_HORDE},
    },
    {
        {EVENT_EASTPARK_CONTEST_ALLIANCE,           HORDE,          LANG_OPVP_WG_EASTPARK_ATTACK_A,         BF_GO_STATE_HORDE_INTACT,       0,  0},
        {EVENT_EASTPARK_CONTEST_HORDE,              ALLIANCE,       LANG_OPVP_WG_EASTPARK_ATTACK_H,         BF_GO_STATE_ALLIANCE_INTACT,    0,  0},
        {EVENT_EASTPARK_PROGRESS_ALLIANCE,          ALLIANCE,       LANG_OPVP_WG_EASTPARK_CAPTURE_A,        BF_GO_STATE_ALLIANCE_INTACT,    CAPTURE_ARTKIT_ALLIANCE,    CAPTURE_ANIM_ALLIANCE},
        {EVENT_EASTPARK_PROGRESS_HORDE,             HORDE,          LANG_OPVP_WG_EASTPARK_CAPTURE_H,        BF_GO_STATE_HORDE_INTACT,       CAPTURE_ARTKIT_HORDE,       CAPTURE_ANIM_HORDE},
    },
    {
        {EVENT_WESTPARK_CONTEST_ALLIANCE,           HORDE,          LANG_OPVP_WG_WESTPARK_ATTACK_A,         BF_GO_STATE_HORDE_INTACT,       0,  0},
        {EVENT_WESTPARK_CONTEST_HORDE,              ALLIANCE,       LANG_OPVP_WG_WESTPARK_ATTACK_H,         BF_GO_STATE_ALLIANCE_INTACT,    0,  0},
        {EVENT_WESTPARK_PROGRESS_ALLIANCE,          ALLIANCE,       LANG_OPVP_WG_WESTPARK_CAPTURE_A,        BF_GO_STATE_ALLIANCE_INTACT,    CAPTURE_ARTKIT_ALLIANCE,    CAPTURE_ANIM_ALLIANCE},
        {EVENT_WESTPARK_PROGRESS_HORDE,             HORDE,          LANG_OPVP_WG_WESTPARK_CAPTURE_H,        BF_GO_STATE_HORDE_INTACT,       CAPTURE_ARTKIT_HORDE,       CAPTURE_ANIM_HORDE},
    },
};

class WintergraspFactory : public BattlefieldBuilding
{
    public:
        WintergraspFactory(uint32 entry) : BattlefieldBuilding(entry), m_objectOwner(TEAM_NONE), m_graveyardId(0), m_areaId(0), m_conditionId(0)
        {
            for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
            {
                m_capturePointGuid[i] = ObjectGuid();
                m_capturePointEntry[i] = 0;
            }
        };

        // get and set object owner team
        Team GetOwner() const { return m_objectOwner; }
        void SetOwner(Team newOwner) { m_objectOwner = newOwner; }

        // get and set capture point entry
        void SetCapturePointEntry(PvpTeamIndex teamIndex, uint32 entry) { m_capturePointEntry[teamIndex] = entry; }
        uint32 GetCapturePointEntry(PvpTeamIndex teamIndex) { return m_capturePointEntry[teamIndex]; }

        // get and set capture point objectGuid
        void SetCapturePointGuid(PvpTeamIndex teamIndex, ObjectGuid guid) { m_capturePointGuid[teamIndex] = guid; }
        ObjectGuid GetCapturePointGuid(PvpTeamIndex teamIndex) { return m_capturePointGuid[teamIndex]; }

        // get and set graveyard id
        void SetGraveyardId(uint32 id) { m_graveyardId = id; }
        uint32 GetGraveyardId() { return m_graveyardId; }

        // get and set area id
        void SetAreaId(uint32 id) { m_areaId = id; }
        uint32 GetAreaId() { return m_areaId; }

        // get and set condition id
        void SetConditionId(uint32 id) { m_conditionId = id; }
        uint32 GetConditionId() { return m_conditionId; }

    protected:
        Team m_objectOwner;

        ObjectGuid m_capturePointGuid[PVP_TEAM_COUNT];

        uint32 m_capturePointEntry[PVP_TEAM_COUNT];
        uint32 m_graveyardId;
        uint32 m_areaId;
        uint32 m_conditionId;
};

enum WintergraspRank
{
    WG_RANK_NONE        = 0,
    WG_RANK_RECRUIT     = 1,            // player gets this by default when battle starts
    WG_RANK_CORPORAL    = 2,            // gained after 5 enemy (creature / player) kills
    WG_RANK_LIEUTENANT  = 3,            // gained after 10 enemy kills from the previous promotion
};

class WintergraspPlayer : public BattlefieldPlayer
{
    public:
        WintergraspPlayer() : BattlefieldPlayer(), killCount(0), playerRank(WG_RANK_NONE) { }

        void SetPlayerRank(WintergraspRank rank) { playerRank = rank; }
        WintergraspRank GetPlayerRank() const { return playerRank; }

        // Set and get the kill count
        void IncreaseKillCount() { ++killCount; }
        uint32 GetKillCount() { return killCount; }

    protected:
        uint32 killCount;
        WintergraspRank playerRank;
};

class BattlefieldWG : public Battlefield
{
    public:
        BattlefieldWG();

        void Reset();

        // player enter / exit zone
        void HandlePlayerEnterZone(Player* player, bool isMainZone) override;
        void HandlePlayerLeaveZone(Player* player, bool isMainZone) override;

        // player enter / exit area - used for factories
        void HandlePlayerEnterArea(Player* player, uint32 areaId, bool isMainZone) override;
        void HandlePlayerLeaveArea(Player* player, uint32 areaId, bool isMainZone) override;

        // update world states
        void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
        void SendRemoveWorldStates(Player* player) override;

        // handle gameobject events
        bool HandleEvent(uint32 eventId, Object* source, Object* target) override;

        // handle creature create or death
        void HandleCreatureCreate(Creature* creature) override;
        void HandleCreatureDeath(Creature* creature) override;

        // handle gameobject create or death
        void HandleGameObjectCreate(GameObject* go) override;
        bool HandleGameObjectUse(Player* player, GameObject* go) override;

        // handle player kills
        void HandlePlayerKillInsideArea(Player* player, Unit* victim) override;

        // update functions
        void Update(uint32 diff) override;
        void SendBattlefieldTimerUpdate() override;

        // start and end battle
        void StartBattle(Team defender) override;
        void EndBattle(Team winner) override;

        // reward teams
        void RewardPlayersOnBattleEnd(Team winner) override;

        // player and group updates
        void OnBattlefieldPlayersUpdate() override;
        void UpdatePlayerBattleResponse(Player* player) override;
        void UpdatePlayerExitRequest(Player* player) override;
        void UpdatePlayerGroupDisband(Player* player) override;

        // conditions
        bool IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType) override;
        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1) override;

    private:

        // load player data: rank and killcount
        void InitPlayerBattlefieldData(Player* player) override;

        // kick afk players
        void KickBattlefieldPlayer(Player* player) override;

        // set the position for player to enter the battlefield (done by spell)
        void SetupPlayerPosition(Player* player) override;

        // functions to handle the battlefield events
        bool HandleCapturePointEvent(uint32 eventId, GameObject* go);
        bool HandleDestructibleBuildingEvent(uint32 eventId, GameObject* go, Unit* invoker);

        // handle lock / unlock of the workshops
        void LockWorkshops(bool lock, const WorldObject* objRef, WintergraspFactory* workshop);

        // battlefield reset before and after the battle
        void GetBattlefieldReady(const WorldObject* objRef);
        void CleanupBattlefield(const WorldObject* objRef, Team winner);

        // update all world states
        void SendUpdateGeneralWorldStates();

        // send battlefield warning
        void SendWintergraspWarning(int32 messageId, const WorldObject* objRef, uint32 soundId = 0);

        // retrieve any available player in zone
        Player* GetPlayerInZone();

        // update player data
        void UpdatePlayerScore(Player* player);

        // update group tenacities - custom way to balance the teams
        void UpdateTenacities(const WorldObject* objRef);

        // get current player rank
        WintergraspRank GetPlayerRank(Player* player);

        // send promotion to player
        void SendPromotionWhisper(Player* player, int32 textEntry);

        // variables
        bool m_sentPrebattleWarning;

        // counters
        uint32 m_destroyedTowers[PVP_TEAM_COUNT];
        uint32 m_workshopCount[PVP_TEAM_COUNT];

        // object storage
        ObjectGuid m_relicGuid[PVP_TEAM_COUNT];
        ObjectGuid m_zoneLeaderGuid[PVP_TEAM_COUNT];

        ObjectGuid m_fortressDoorGuid;
        ObjectGuid m_fortressDoorWallGuid;

        // buildings
        std::vector<BattlefieldBuilding*> m_keepBuildings;
        std::vector<BattlefieldBuilding*> m_defenseTowers;
        std::vector<BattlefieldBuilding*> m_offenseTowers;

        std::vector<WintergraspFactory*> m_defenseWorkshops;
        std::vector<WintergraspFactory*> m_capturableWorkshops;

        // guid lists
        GuidList m_defenseCannonsGuids;
        GuidList m_attackCannonsGuids;
        GuidList m_stalkersGuids;
        GuidList m_detectionUnitsGuids;
        GuidList m_trashMobsGuids;

        GuidList m_vendorGuids[PVP_TEAM_COUNT];
        GuidList m_activeVehiclesGuids[PVP_TEAM_COUNT];
        GuidList m_towerBannersGuids[PVP_TEAM_COUNT];
};

#endif
