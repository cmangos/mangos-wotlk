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
    // ***** Spells *****
    // battle spells
    SPELL_RECRUIT                               = 37795,
    SPELL_CORPORAL                              = 33280,
    SPELL_LIEUTENANT                            = 55629,
    SPELL_TENACITY                              = 58549,
    SPELL_TENACITY_VEHICLE                      = 59911,
    SPELL_TOWER_CONTROL                         = 62064,
    SPELL_SPIRITUAL_IMMUNITY                    = 58729,
    SPELL_GREAT_HONOR                           = 58555,
    SPELL_GREATER_HONOR                         = 58556,
    SPELL_GREATEST_HONOR                        = 58557,
    SPELL_ALLIANCE_FLAG                         = 14268,
    SPELL_HORDE_FLAG                            = 14267,
    SPELL_GRAB_PASSENGER                        = 61178,
    SPELL_ACTIVATE_ROBOTIC_ARMS                 = 49899,

    // reward spells
    SPELL_WINTERGRASP_VICTORY                   = 56902,
    SPELL_WINTERGRASP_DEFEAT                    = 58494,
    SPELL_DAMAGED_TOWER                         = 59135,
    SPELL_DESTROYED_TOWER                       = 59136,
    SPELL_DAMAGED_BUILDING                      = 59201,
    SPELL_INTACT_BUILDING                       = 59203,

    // misc spells
    SPELL_WINTERGRASP_WATER                     = 36444,            // water damage against vehicles
    SPELL_ESSENCE_WINTERGRASP_MAP               = 57940,            // map wide buff
    SPELL_ESSENCE_WINTERGRASP_ZONE              = 58045,            // zone specidif buff (includes phasing 257) - allows the player to see the Greater Elemental Spirits
    // SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA = 58730,            // no fly zone aura - Implemented in Player::UpdateArea()
    SPELL_VICTORY_AURA_COSMETIC                 = 60044,
    SPELL_RULERS_OF_WINTERGRASP                 = 52108,            // increase damage buff
    // SPELL_ACH_DESTROYED_TOWER                = 60676,

    // Phasing spells
    SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE       = 56617,            // phase 32
    SPELL_HORDE_CONTROLS_FACTORY_PHASE          = 56618,            // phase 16
    SPELL_ALLIANCE_CONTROL_PHASE                = 60027,            // phase 65
    SPELL_HORDE_CONTROL_PHASE                   = 60028,            // phase 129

    // Teleport spells
    SPELL_TELEPORT_DALARAN                      = 53360,
    SPELL_TELEPORT_BRIDGE                       = 59096,
    SPELL_TELEPORT_DALARAN_TO_WG                = 60035,
    SPELL_TELEPORT_FORTRESS_GRAVEYARD           = 59760,
    SPELL_TELEPORT_SUNKEN_RING                  = 59762,
    SPELL_TELEPORT_BROKEN_TEMPLE                = 59763,
    SPELL_TELEPORT_HORDE_LANDING                = 59765,
    SPELL_TELEPORT_WESTPARK_GRAVEYARD           = 59766,
    SPELL_TELEPORT_EASTPARK_GRAVEYARD           = 59767,
    SPELL_TELEPORT_ALLIANCE_LANDING             = 59769,
    SPELL_TELEPORT_PORTAL                       = 54643,
    SPELL_TELEPORT_VEHICLE                      = 49759,
    SPELL_TELEPORT_WINTERGRASP                  = 58622, // go cast
    SPELL_TELEPORT_WINTERGRASP_ALLIANCE         = 58633, // to alliance landing
    SPELL_TELEPORT_WINTERGRASP_HORDE            = 58632, // to horde landing

    // Vehicle build spells
    SPELL_BUILD_CATAPULT_FORCE                  = 56664,
    SPELL_BUILD_DEMOLISHER_FORCE                = 56659,
    SPELL_BUILD_SIEGE_VEHICLE_A_FORCE           = 56662,
    SPELL_BUILD_SIEGE_VEHICLE_H_FORCE           = 61409,


    // ***** Quests *****
    QUEST_ID_VICTORY_IN_WINTERGRASP_A           = 13181,
    QUEST_ID_VICTORY_IN_WINTERGRASP_H           = 13183,


    // ***** Kill credits *****
    NPC_QUEST_CREDIT_KILL_VEHICLE               = 31093,
    NPC_QUEST_CREDIT_DEFEND_VEHICLE             = 31284,
    NPC_QUEST_CREDIT_KILL_ALLIANCE              = 31086,
    NPC_QUEST_CREDIT_KILL_TOWER                 = 31156,
    NPC_QUEST_CREDIT_KILL_STRUCTURE             = 31244,
    NPC_QUEST_CREDIT_KILL_BRIDGE                = 31286,
    NPC_QUEST_CREDIT_KILL_WALL                  = 31287,
    NPC_QUEST_CREDIT_KILL_WORKSHOP              = 31288,
    NPC_QUEST_CREDIT_KILL_GATE                  = 31289,
    NPC_QUEST_CREDIT_KILL_SOUTHERN_TOWER        = 35074,
    NPC_QUEST_CREDIT_KILL_HORDE                 = 39019,


    // ***** Achiev criterias *****
    // ACHIV_CRITERIA_WIN_10_MINUTES            = 7666,


    // ***** Area ids *****
    AREA_ID_THE_SUNKEN_RING                     = 4538,
    AREA_ID_THE_BROKEN_TEMPLE                   = 4539,
    AREA_ID_WINTERGRASP_FORTRESS                = 4575,
    AREA_ID_CENTRAL_BRIDGE                      = 4576,
    AREA_ID_EASTERN_BRIDGE                      = 4577,
    AREA_ID_WESTERN_BRIDGE                      = 4578,
    AREA_ID_FLAMEWATCH_TOWER                    = 4581,
    AREA_ID_WINTERS_EDGE_TOWER                  = 4582,
    AREA_ID_SHADOWSIGHT_TOWER                   = 4583,
    AREA_ID_THE_CAULDRON_OF_FLAMES              = 4584,
    AREA_ID_GLACIAL_FALLS                       = 4585,
    AREA_ID_WINDY_BLUFFS                        = 4586,
    AREA_ID_THE_FOREST_OF_SHADOWS               = 4587,
    AREA_ID_THE_CHILLED_QUAGMIRE                = 4589,
    AREA_ID_THE_STEPPE_OF_LIFE                  = 4590,
    AREA_ID_WESTPARK_WORKSHOP                   = 4611,
    AREA_ID_EASTPARK_WORKSHOP                   = 4612,

    // ***** Creatures *****
    // ally creatures
    NPC_ANCHORITE_TESSA                         = 31054,
    NPC_COMMANDER_ZANNETH                       = 31036,
    NPC_SENIOR_DEMOLITIONIST_LEGOSO             = 31109,
    NPC_SIEGE_MASTER_STOUTHANDLE                = 31108,
    NPC_SORCERESS_KAYLANA                       = 31051,
    NPC_TACTICAL_OFFICER_AHBRAMIS               = 31153,
    NPC_BOWYER_RANDOLPH                         = 31052,

    NPC_KNIGHT_DAMERON                          = 32294,
    NPC_MARSHAL_MAGRUDER                        = 39172,

    NPC_MORGAN_DAY                              = 30489,
    NPC_TRAVIS_DAY                              = 30488,

    // horde creatures
    NPC_COMMANDER_DARDOSH                       = 31091,
    NPC_HOODOO_MASTER_FUJIN                     = 31101,
    NPC_LEUTENANT_MURP                          = 31107,
    NPC_PRIMALIST_MULFORT                       = 31053,
    NPC_SIEGESMITH_STRONGHOOF                   = 31106,
    NPC_TACTICAL_OFFICER_KILRATH                = 31151,
    NPC_VIERON_BLAZEFEATHER                     = 31102,

    NPC_STONE_GUARD_MUKAR                       = 32296,
    NPC_CHAMPION_ROSSLAI                        = 39173,

    // trash
    NPC_CHILLED_EARH_ELEMENTAL                  = 30849,
    NPC_EARTHBOUND_REVENANT                     = 30876,
    NPC_RAGING_FLAME                            = 30847,
    NPC_FLAME_REVENANT                          = 30873,
    NPC_GLACIAL_SPIRIT                          = 30846,
    NPC_WATER_REVENANT                          = 30877,
    NPC_WANDERING_SHADOW                        = 30842,
    NPC_SHADOW_REVENANT                         = 30872,
    NPC_WHISPERING_WIND                         = 30848,
    NPC_TEMPEST_REVENANT                        = 30875,
    NPC_LIVING_LASHER                           = 30845,
    NPC_MATURE_LASHER                           = 34300,

    NPC_GNOMISH_MECHANIC                        = 30499,
    NPC_GOBLIN_ENGINEER                         = 30400,

    NPC_ALLIANCE_SPIRIT_GUIDE                   = 31842,
    NPC_HORDE_SPIRIT_GUIDE                      = 31841,

    NPC_VALLIANCE_EXPEDITION_CHAMPION           = 30740,
    NPC_VALLIANCE_EXPEDITION_GUARD              = 32308,

    NPC_WARSONG_CHAMPION                        = 30739,
    NPC_WARSONG_GUARD                           = 32307,

    NPC_ALLIANCE_BRIGADIER_GENERAL              = 32626,
    NPC_HORDE_WARBRINGER                        = 32615,

    NPC_WINTERGRASP_CATAPULT                    = 27881,
    NPC_WINTERGRASP_DEMOLISHER                  = 28094,
    NPC_WINTERGRASP_SIEGE_ENGINE_A              = 28312,
    NPC_WINTERGRASP_SIEGE_ENGINE_H              = 32627,
    NPC_WINTERGRASP_SIEGE_TURRET_A              = 28319,
    NPC_WINTERGRASP_SIEGE_TURRET_H              = 32629,
    NPC_WINTERGRASP_TOWER_CANNON                = 28366,

    NPC_RP_GG                                   = 30560,
    NPC_CONTROL_ARMS                            = 27852,

    //NPC_WORLD_TRIGGER                         = 23472,
    NPC_INVISIBLE_STALKER                       = 15214,            // sends raid emotes

    //NPC_ARATHI_BASIN_PORTAL_A                 = 32618,
    //NPC_ARATHI_BASIN_PORTAL_H                 = 32619,

    //NPC_WARSONG_GULCH_PORTAL_A                = 32624,
    //NPC_WARSONG_GULCH_PORTAL_H                = 32625,

    //NPC_STRAND_OF_THE_ANCIENTS_PORTAL_A       = 32622,
    //NPC_STRAND_OF_THE_ANCIENTS_PORTAL_H       = 32623,

    //NPC_EYE_OF_THE_STORM_PORTAL_A             = 32621,
    //NPC_EYE_OF_THE_STORM_PORTAL_H             = 32620,

    //NPC_ALTERAC_VALLEY_PORTAL_A               = 32616,
    //NPC_ALTERAC_VALLEY_PORTAL_H               = 32617,

    //NPC_ISLE_OF_CONQUEST_PORTAL_A             = 34952,
    //NPC_ISLE_OF_CONQUEST_PORTAL_H             = 34953,

    // ***** Gameobjects *****
    // titan relics
    GO_TITAN_RELIC_ALLIANCE                     = 192834,           // both send event id 22097
    GO_TITAN_RELIC_HORDE                        = 192829,

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

    // workshops
    GO_WORKSHOP_KEEP_WEST                       = 192028,
    GO_WORKSHOP_KEEP_EAST                       = 192029,
    GO_WORKSHOP_BROKEN_TEMPLE                   = 192030,
    GO_WORKSHOP_SUNKEN_RING                     = 192031,
    GO_WORKSHOP_WESTPARK                        = 192032,
    GO_WORKSHOP_EASTPARK                        = 192033,

    // capture points
    GO_CAPTUREPOINT_BROKEN_TEMPLE_A             = 192627,
    GO_CAPTUREPOINT_BROKEN_TEMPLE_H             = 190487,
    GO_CAPTUREPOINT_WESTPARK_A                  = 194962,
    GO_CAPTUREPOINT_WESTPARK_H                  = 194963,
    GO_CAPTUREPOINT_EASTPARK_A                  = 194959,
    GO_CAPTUREPOINT_EASTPARK_H                  = 194960,
    GO_CAPTUREPOINT_SUNKEN_RING_A               = 192626,
    GO_CAPTUREPOINT_SUNKEN_RING_H               = 190475,

    // other GOs
    //GO_ALLIANCE_BANNER_1                      = 192287,
    //GO_ALLIANCE_BANNER_2                      = 192320,
    //GO_ALLIANCE_BANNER_3                      = 192314,
    //GO_ALLIANCE_BANNER_4                      = 192487,
    //GO_ALLIANCE_BANNER_5                      = 192313,
    //GO_ALLIANCE_BANNER_6                      = 192312,
    //GO_ALLIANCE_BANNER_7                      = 192299,
    //GO_ALLIANCE_BANNER_8                      = 192292,
    //GO_ALLIANCE_BANNER_9                      = 192308,
    //GO_ALLIANCE_BANNER_10                     = 192327,
    //GO_ALLIANCE_BANNER_11                     = 192329,
    //GO_ALLIANCE_BANNER_12                     = 192309,
    //GO_ALLIANCE_BANNER_13                     = 192324,
    //GO_ALLIANCE_BANNER_14                     = 192310,
    //GO_ALLIANCE_BANNER_15                     = 192325,
    //GO_ALLIANCE_BANNER_16                     = 192326,
    //GO_ALLIANCE_BANNER_17                     = 192322,
    //GO_ALLIANCE_BANNER_18                     = 192323,
    //GO_ALLIANCE_BANNER_19                     = 192318,
    //GO_ALLIANCE_BANNER_20                     = 192319,
    //GO_ALLIANCE_BANNER_21                     = 192304,
    //GO_ALLIANCE_BANNER_22                     = 192305,
    //GO_ALLIANCE_BANNER_23                     = 192286,
    //GO_ALLIANCE_BANNER_24                     = 192317,
    //GO_ALLIANCE_BANNER_25                     = 192316,
    //GO_ALLIANCE_BANNER_26                     = 192321,
    //GO_ALLIANCE_BANNER_27                     = 192335,
    //GO_ALLIANCE_BANNER_28                     = 192334,
    //GO_ALLIANCE_BANNER_29                     = 192331,
    //GO_ALLIANCE_BANNER_30                     = 192332,
    //GO_ALLIANCE_BANNER_31                     = 192333,
    //GO_ALLIANCE_BANNER_32                     = 192314,
    //GO_ALLIANCE_BANNER_33                     = 192306,
    //GO_ALLIANCE_BANNER_34                     = 192307,
    //GO_ALLIANCE_BANNER_35                     = 192330,
    //GO_ALLIANCE_BANNER_36                     = 192328,
    //GO_ALLIANCE_BANNER_37                     = 192252,
    //GO_ALLIANCE_BANNER_38                     = 192253,
    //// center tower
    //GO_ALLIANCE_BANNER_39                     = 192269, // named horde banner in db
    //GO_ALLIANCE_BANNER_40                     = 192501, // all towers have this
    //GO_ALLIANCE_BANNER_41                     = 192266,
    //GO_ALLIANCE_BANNER_42                     = 192415,
    //GO_ALLIANCE_BANNER_43                     = 192429,
    //GO_ALLIANCE_BANNER_44                     = 192430,
    //GO_ALLIANCE_BANNER_45                     = 192270,
    //GO_ALLIANCE_BANNER_46                     = 192431,
    //GO_ALLIANCE_BANNER_47                     = 192414,
    //// wespark
    //GO_ALLIANCE_BANNER_48                     = 192274,
    //GO_ALLIANCE_BANNER_49                     = 192409,
    //GO_ALLIANCE_BANNER_50                     = 192433,
    //GO_ALLIANCE_BANNER_51                     = 192407,
    //GO_ALLIANCE_BANNER_52                     = 192406,
    //// west tower
    //GO_ALLIANCE_BANNER_53                     = 192278,
    //GO_ALLIANCE_BANNER_54                     = 192277,
    //// broken temple
    //GO_ALLIANCE_BANNER_55                     = 192401,
    //GO_ALLIANCE_BANNER_56                     = 192281,
    //GO_ALLIANCE_BANNER_57                     = 192400,
    //GO_ALLIANCE_BANNER_58                     = 192282,
    //// sunken ring
    //GO_ALLIANCE_BANNER_59                     = 192291,
    //GO_ALLIANCE_BANNER_60                     = 192427,
    //GO_ALLIANCE_BANNER_61                     = 192428,
    //GO_ALLIANCE_BANNER_62                     = 192426,
    //GO_ALLIANCE_BANNER_63                     = 192425,
    //GO_ALLIANCE_BANNER_64                     = 192288,
    //// eastpark
    //GO_ALLIANCE_BANNER_65                     = 192418,
    //GO_ALLIANCE_BANNER_66                     = 192273,
    //GO_ALLIANCE_BANNER_67                     = 192417,
    //GO_ALLIANCE_BANNER_68                     = 192416,
    //// east tower
    //GO_ALLIANCE_BANNER_69                     = 192423,

    //GO_HORDE_BANNER_1                         = 192284,
    //GO_HORDE_BANNER_2                         = 192355,
    //GO_HORDE_BANNER_3                         = 192364,
    //GO_HORDE_BANNER_4                         = 192488,
    //GO_HORDE_BANNER_5                         = 192367,
    //GO_HORDE_BANNER_6                         = 192366,
    //GO_HORDE_BANNER_7                         = 192349,
    //GO_HORDE_BANNER_8                         = 192336,
    //GO_HORDE_BANNER_9                         = 192375,
    //GO_HORDE_BANNER_10                        = 192353,
    //GO_HORDE_BANNER_11                        = 192357,
    //GO_HORDE_BANNER_12                        = 192374,
    //GO_HORDE_BANNER_13                        = 192373,
    ////GO_HORDE_BANNER_14                      = ?,
    //GO_HORDE_BANNER_15                        = 192360,
    //GO_HORDE_BANNER_16                        = 192361,
    //GO_HORDE_BANNER_17                        = 192372,
    //GO_HORDE_BANNER_18                        = 192371,
    //GO_HORDE_BANNER_19                        = 192358,
    //GO_HORDE_BANNER_20                        = 192359,
    //GO_HORDE_BANNER_21                        = 192338,
    //GO_HORDE_BANNER_22                        = 192339,
    //GO_HORDE_BANNER_23                        = 192285,
    //GO_HORDE_BANNER_24                        = 192379,
    //GO_HORDE_BANNER_25                        = 192378,
    //GO_HORDE_BANNER_26                        = 192354,
    //GO_HORDE_BANNER_27                        = 192363,
    //GO_HORDE_BANNER_28                        = 192362,
    //GO_HORDE_BANNER_29                        = 192368,
    //GO_HORDE_BANNER_30                        = 192369,
    //GO_HORDE_BANNER_31                        = 192370,
    //GO_HORDE_BANNER_32                        = 192364,
    //GO_HORDE_BANNER_33                        = 192350,
    //GO_HORDE_BANNER_34                        = 192351,
    //GO_HORDE_BANNER_35                        = 192356,
    //GO_HORDE_BANNER_36                        = 192352,
    //GO_HORDE_BANNER_37                        = 192255,
    //GO_HORDE_BANNER_38                        = 192254,
    //// center tower
    //GO_HORDE_BANNER_39                        = 192268,
    //GO_HORDE_BANNER_40                        = 192502, // all towers have this
    //GO_HORDE_BANNER_41                        = 192267,
    //GO_HORDE_BANNER_42                        = 192449,
    //GO_HORDE_BANNER_43                        = 192444,
    //GO_HORDE_BANNER_44                        = 192443,
    //GO_HORDE_BANNER_45                        = 192271,
    //GO_HORDE_BANNER_46                        = 192442,
    //GO_HORDE_BANNER_47                        = 192450,
    //// westpark
    //GO_HORDE_BANNER_48                        = 192275,
    //GO_HORDE_BANNER_49                        = 192408,
    //GO_HORDE_BANNER_50                        = 192432,
    //GO_HORDE_BANNER_51                        = 192441,
    //GO_HORDE_BANNER_52                        = 192440,
    //// west tower
    //GO_HORDE_BANNER_53                        = 192279,
    //GO_HORDE_BANNER_54                        = 192276,
    //// broken temple
    //GO_HORDE_BANNER_55                        = 192435,
    //GO_HORDE_BANNER_56                        = 192280,
    //GO_HORDE_BANNER_57                        = 192434,
    //GO_HORDE_BANNER_58                        = 192283,
    //// sunken ring
    //GO_HORDE_BANNER_59                        = 192290,
    //GO_HORDE_BANNER_60                        = 192460,
    //GO_HORDE_BANNER_61                        = 192461,
    //GO_HORDE_BANNER_62                        = 192459,
    //GO_HORDE_BANNER_63                        = 192458,
    //GO_HORDE_BANNER_64                        = 192289,
    //// eastpark
    //GO_HORDE_BANNER_65                        = 192453,
    //GO_HORDE_BANNER_66                        = 192272,
    //GO_HORDE_BANNER_67                        = 192452,
    //GO_HORDE_BANNER_68                        = 192451,
    //// east tower
    //GO_HORDE_BANNER_69                        = 192424,

    //// Keep misc GOs
    //GO_ALLIANCE_MISC_01                       = 193138,
    //GO_ALLIANCE_MISC_02                       = 193131,
    //GO_ALLIANCE_MISC_03                       = 193137,
    //GO_ALLIANCE_MISC_04                       = 193135,
    //GO_ALLIANCE_MISC_05                       = 193134,
    //GO_ALLIANCE_MISC_06                       = 193132,
    //GO_ALLIANCE_MISC_07                       = 193128,
    //GO_ALLIANCE_MISC_08                       = 193165,
    //GO_ALLIANCE_MISC_09                       = 193124,

    //GO_HORDE_MISC_01                          = 193145,
    //GO_HORDE_MISC_02                          = 193139,
    //GO_HORDE_MISC_03                          = 193142,
    //GO_HORDE_MISC_04                          = 193146,
    //GO_HORDE_MISC_05                          = 193141,
    //GO_HORDE_MISC_06                          = 193140,
    //GO_HORDE_MISC_07                          = 193144,
    //GO_HORDE_MISC_08                          = 193143,
    //GO_HORDE_MISC_09                          = 193161,
    //GO_HORDE_MISC_10                          = 193127,

    // portal GOs
    GO_DEFENDERS_PORTAL_HORDE                   = 190763,
    GO_DEFENDERS_PORTAL_ALLIANCE                = 191575,
    GO_DEFENDERS_PORTAL_GENERIC                 = 192819,
    GO_VEHICLE_TELEPORTER                       = 192951,
    GO_PORTAL_TO_WINTERGRASP                    = 193772,

    // factions
    FACTION_GO_ALLIANCE                         = 1732,
    FACTION_GO_HORDE                            = 1735,

    // ***** Graveyards *****
    GRAVEYARD_ID_KEEP_1                         = 1285,
    GRAVEYARD_ID_KEEP_2                         = 1328,
    GRAVEYARD_ID_SUNKEN_RING                    = 1329,
    GRAVEYARD_ID_BROKEN_TEMPLE                  = 1330,
    GRAVEYARD_ID_EASTPARK                       = 1333,
    GRAVEYARD_ID_WESTPARK                       = 1334,


    // ***** Events *****
    // capture points events (identical between horde and alliance GO versions)
    EVENT_BROKEN_TEMPLE_CONTEST_ALLIANCE        = 20787,
    EVENT_BROKEN_TEMPLE_CONTEST_HORDE           = 20788,
    EVENT_BROKEN_TEMPLE_PROGRESS_ALLIANCE       = 19612,
    EVENT_BROKEN_TEMPLE_PROGRESS_HORDE          = 19611,

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

    // other events
    // destructible building events are defined below - too many to add here
    EVENT_TITAN_RELIC                           = 22097,                // event sent by the Titan relic


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
    SOUND_ID_WG_START                           = 3439,


    // ***** Factions *****
    FACTION_ID_WARSONG                          = 1979,
    FACTION_ID_VALLIANCE                        = 1891,

    FACTION_ID_ALLIANCE_GENERIC_1               = 1732,             // used by GOs and creatures
    FACTION_ID_ALLIANCE_GENERIC_2               = 1733,             // ToDo: to be confirmed
    FACTION_ID_HORDE_GENERIC_1                  = 1734,             // ToDo: to be confirmed
    FACTION_ID_HORDE_GENERIC_2                  = 1735,             // used by GOs and creatures


    // ***** Condition entries *****
    // used to check the gossip options in DB for various npcs
    OPVP_COND_WG_MAX_ALLIANCE_VEHICLES          = 0,
    OPVP_COND_WG_MAX_HORDE_VEHICLES             = 1,
    OPVP_COND_WG_BATTLEFIELD_IN_PROGRESS        = 2,
};

struct WintergraspGoData
{
    uint32 goEntry, worldState, eventIntact, eventDataged, eventDestroyed, eventRebuild;
};

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
    {GO_WG_TOWER_WEST,          WORLD_STATE_WG_KEEP_TOWER_WEST,         19665,  19657,  19661,  19460},
    {GO_WG_TOWER_SOUTHWEST,     WORLD_STATE_WG_KEEP_TOWER_SOUTHWEST,    19667,  19659,  19662,  19459},
    {GO_WG_TOWER_SOUTHEAST,     WORLD_STATE_WG_KEEP_TOWER_SOUTHEAST,    19668,  19660,  19664,  19457},
    {GO_WG_TOWER_EAST,          WORLD_STATE_WG_KEEP_TOWER_EAST,         19666,  19658,  19663,  19458},
    {GO_WORKSHOP_KEEP_WEST,     WORLD_STATE_WG_WORKSHOP_KEEP_WEST,      19790,  19782,  19786,  19794},
    {GO_WORKSHOP_KEEP_EAST,     WORLD_STATE_WG_WORKSHOP_KEEP_EAST,      19791,  19783,  19787,  19795},
};

static const WintergraspGoData wgOffensiveData[] =
{
    {GO_TOWER_SHADOWSIGHT,      WORLD_STATE_WG_TOWER_SHADOWSIGHT,       19671,  19674,  19677,  19680},
    {GO_TOWER_WINTERS_EDGE,     WORLD_STATE_WG_TOWER_WINTERS_EDGE,      19670,  19673,  19676,  19679},
    {GO_TOWER_FLAMEWATCH,       WORLD_STATE_WG_TOWER_FLAMEWATCH,        19669,  19672,  19675,  19678},
    {GO_WORKSHOP_BROKEN_TEMPLE, WORLD_STATE_WG_WORKSHOP_BROKEN_TEMPLE,  19775,  19777,  19779,  19781},
    {GO_WORKSHOP_SUNKEN_RING,   WORLD_STATE_WG_WORKSHOP_SUNKEN_RING,    19774,  19776,  19778,  19780},
    {GO_WORKSHOP_WESTPARK,      WORLD_STATE_WG_WORKSHOP_WESTPARK,       19792,  19784,  19788,  19796},
    {GO_WORKSHOP_EASTPARK,      WORLD_STATE_WG_WORKSHOP_EASTPARK,       19793,  19785,  19789,  19797},
};

enum WGRank
{
    WG_RANK_NONE        = 0,
    WG_RANK_RECRUIT     = 1,
    WG_RANK_CORPORAL    = 2,
    WG_RANK_LIEUTENANT  = 3,
};

class WGWorkShop : public BattlefieldBuilding
{
    public:
        WGWorkShop(uint8 assignedId) : BattlefieldBuilding(assignedId) {};

        void SetupBuilding(Team owner, bool reset);
        void SendUpdateWorldState();

        ObjectGuid capturePoint;
};

class WGTower : public BattlefieldBuilding
{
    public:
        WGTower(uint8 assignedId) : BattlefieldBuilding(assignedId) {};

        void SetupBuilding(Team owner, bool reset);
        void SpawnCannons(bool despawn = false);

        std::list<ObjectGuid> cannons;
};

class WGPlayerScore : public BattlefieldPlayer
{
    public:
        WGPlayerScore() : BattlefieldPlayer(), rank(0) { }

        WGRank GetRank() const;
        uint32 rank;
};

class BattlefieldWG : public Battlefield
{
    public:
        BattlefieldWG();

        void Reset();

        void HandlePlayerEnterZone(Player* player, bool isMainZone) override;
        void HandlePlayerLeaveZone(Player* player, bool isMainZone) override;
        void HandlePlayerEnterArea(Player* player, uint32 areaId, bool isMainZone) override;
        void HandlePlayerLeaveArea(Player* player, uint32 areaId, bool isMainZone) override;
        void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
        void SendRemoveWorldStates(Player* player) override;

        bool HandleEvent(uint32 eventId, GameObject* go) override;

        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;
        void HandleCreatureDeath(Creature* creature) override;

        void HandlePlayerKillInsideArea(Player* player, Unit* victim) override;
        void Update(uint32 diff) override;

        void StartBattle(Team defender) override;
        void EndBattle(Team winner, bool byTimer) override;
        void RewardPlayersOnBattleEnd(Team winner) override;

        void UpdatePlayerOnWarResponse(Player* player) override;

        void UpdateGraveyardOwner(uint8 id, PvpTeamIndex newOwner) override;

        bool IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType) override;
        void HandleConditionStateChange(uint32 conditionId, bool state) override;

    private:
        void InitPlayerBattlefieldData(Player* player) override;
        bool GetPlayerKickLocation(Player* player, float& x, float& y, float& z) override;

        void SetupPlayerPosition(Player* player) override;

        bool HandleCapturePointEvent(uint32 eventId, GameObject* go);
        bool HandleDestructibleBuildingEvent(uint32 eventId, GameObject* go);

        bool m_sentPrebattleWarning;

        // counters
        uint32 m_damagedTowers[PVP_TEAM_COUNT];
        uint32 m_destroyedTowers[PVP_TEAM_COUNT];
        uint32 m_workshopCount[PVP_TEAM_COUNT];

        std::list<ObjectGuid> m_zoneTrashGUIDs;
        std::list<ObjectGuid> m_keepCannonGUIDs;
        std::list<ObjectGuid> m_portalGUIDs;
        std::list<ObjectGuid> m_vehicleGUIDs[PVP_TEAM_COUNT];

        ObjectGuid m_keepDoorGUID;
        ObjectGuid m_keepGateGUID;
        ObjectGuid m_titanRelicGUID;
        ObjectGuid m_zannethGuid;
        ObjectGuid m_dardoshGuid;

        std::map<uint8, WGTower*> m_attackTowers;
        std::map<uint8, WGTower*> m_defenseTowers;
        std::list<BattlefieldBuilding*> m_keepWalls;
        std::map<uint8, WGWorkShop*> m_workshops;

        std::list<BattlefieldBuilding*> m_keepBuildings;
        std::list<BattlefieldBuilding*> m_offensiveBuildings;
};

#endif
