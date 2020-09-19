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

#ifndef _PLAYERBOTAI_H
#define _PLAYERBOTAI_H

#include "Common.h"
#include "../../Entities/Creature.h"
#include "../../Entities/ObjectGuid.h"
#include "../../Entities/Unit.h"
#include "../../GameEvents/GameEventMgr.h"
#include "../../Quests/QuestDef.h"

class WorldPacket;
class WorldObject;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;
class PlayerbotMgr;

enum RacialTraits
{
    ARCANE_TORRENT_MANA_CLASSES    = 28730,
    ARCANE_TORRENT_DEATH_KNIGHT    = 50613,
    ARCANE_TORRENT_ROGUE           = 25046,
    BERSERKING_ALL                 = 26297,
    BLOOD_FURY_MELEE_CLASSES       = 20572,
    BLOOD_FURY_WARLOCK             = 33702,
    BLOOD_FURY_SHAMAN              = 33697,
    ESCAPE_ARTIST_ALL              = 20589,
    EVERY_MAN_FOR_HIMSELF_ALL      = 59752,
    GIFT_OF_THE_NAARU_DEATH_KNIGHT = 59545,
    GIFT_OF_THE_NAARU_HUNTER       = 59543,
    GIFT_OF_THE_NAARU_MAGE         = 59548,
    GIFT_OF_THE_NAARU_PALADIN      = 59542,
    GIFT_OF_THE_NAARU_PRIEST       = 59544,
    GIFT_OF_THE_NAARU_SHAMAN       = 59547,
    GIFT_OF_THE_NAARU_WARRIOR      = 28880,
    SHADOWMELD_ALL                 = 58984,
    STONEFORM_ALL                  = 20594,
    WAR_STOMP_ALL                  = 20549,
    WILL_OF_THE_FORSAKEN_ALL       = 7744
};

enum ProfessionSpells
{
    ALCHEMY_1                      = 2259,
    BLACKSMITHING_1                = 2018,
    COOKING_1                      = 2550,
    ENCHANTING_1                   = 7411,
    ENGINEERING_1                  = 4036,
    FIRST_AID_1                    = 3273,
    FISHING_1                      = 7620,
    HERB_GATHERING_1               = 2366,
    INSCRIPTION_1                  = 45357,
    JEWELCRAFTING_1                = 25229,
    MINING_1                       = 2575,
    SKINNING_1                     = 8613,
    TAILORING_1                    = 3908,
    MILLING_1                      = 51005,
    DISENCHANTING_1                = 13262,
    PROSPECTING_1                  = 31252
};

enum TalentsDeathKnight  // 0x020
{
    DEATHKNIGHT_BUTCHERY                        = 1939,  // TabId = 398, Tab = 0, Row = 0, Column = 0
    DEATHKNIGHT_SUBVERSION                      = 1945,  // TabId = 398, Tab = 0, Row = 0, Column = 1
    DEATHKNIGHT_BLADE_BARRIER                   = 2017,  // TabId = 398, Tab = 0, Row = 0, Column = 2
    DEATHKNIGHT_BLADED_ARMOR                    = 1938,  // TabId = 398, Tab = 0, Row = 1, Column = 0
    DEATHKNIGHT_SCENT_OF_BLOOD                  = 1948,  // TabId = 398, Tab = 0, Row = 1, Column = 1
    DEATHKNIGHT_TWOHANDED_WEAPON_SPECIALIZATION = 2217,  // TabId = 398, Tab = 0, Row = 1, Column = 2
    DEATHKNIGHT_RUNE_TAP                        = 1941,  // TabId = 398, Tab = 0, Row = 2, Column = 0
    DEATHKNIGHT_DARK_CONVICTION                 = 1943,  // TabId = 398, Tab = 0, Row = 2, Column = 1
    DEATHKNIGHT_DEATH_RUNE_MASTERY              = 2086,  // TabId = 398, Tab = 0, Row = 2, Column = 2
    DEATHKNIGHT_IMPROVED_RUNE_TAP               = 1942,  // TabId = 398, Tab = 0, Row = 3, Column = 0
    DEATHKNIGHT_SPELL_DEFLECTION                = 2018,  // TabId = 398, Tab = 0, Row = 3, Column = 2
    DEATHKNIGHT_VENDETTA                        = 1953,  // TabId = 398, Tab = 0, Row = 3, Column = 3
    DEATHKNIGHT_BLOODY_STRIKES                  = 2015,  // TabId = 398, Tab = 0, Row = 4, Column = 0
    DEATHKNIGHT_VETERAN_OF_THE_THIRD_WAR        = 1950,  // TabId = 398, Tab = 0, Row = 4, Column = 2
    DEATHKNIGHT_MARK_OF_BLOOD                   = 1949,  // TabId = 398, Tab = 0, Row = 4, Column = 3
    DEATHKNIGHT_BLOODY_VENGEANCE                = 1944,  // TabId = 398, Tab = 0, Row = 5, Column = 1
    DEATHKNIGHT_ABOMINATIONS_MIGHT              = 2105,  // TabId = 398, Tab = 0, Row = 5, Column = 2
    DEATHKNIGHT_BLOOD_WORMS                     = 1960,  // TabId = 398, Tab = 0, Row = 6, Column = 0
    DEATHKNIGHT_HYSTERIA                        = 1954,  // TabId = 398, Tab = 0, Row = 6, Column = 1
    DEATHKNIGHT_IMPROVED_BLOOD_PRESENCE         = 1936,  // TabId = 398, Tab = 0, Row = 6, Column = 2
    DEATHKNIGHT_IMPROVED_DEATH_STRIKE           = 2259,  // TabId = 398, Tab = 0, Row = 7, Column = 0
    DEATHKNIGHT_SUDDEN_DOOM                     = 1955,  // TabId = 398, Tab = 0, Row = 7, Column = 1
    DEATHKNIGHT_VAMPIRIC_BLOOD                  = 2019,  // TabId = 398, Tab = 0, Row = 7, Column = 2
    DEATHKNIGHT_WILL_OF_THE_NECROPOLIS          = 1959,  // TabId = 398, Tab = 0, Row = 8, Column = 0
    DEATHKNIGHT_HEART_STRIKE                    = 1957,  // TabId = 398, Tab = 0, Row = 8, Column = 1
    DEATHKNIGHT_MIGHT_OF_MOGRAINE               = 1958,  // TabId = 398, Tab = 0, Row = 8, Column = 2
    DEATHKNIGHT_BLOOD_GORGED                    = 2034,  // TabId = 398, Tab = 0, Row = 9, Column = 1
    DEATHKNIGHT_DANCING_RUNE_WEAPON             = 1961,  // TabId = 398, Tab = 0, Row = 10, Column = 1
    DEATHKNIGHT_IMPROVED_ICY_TOUCH              = 2031,  // TabId = 399, Tab = 1, Row = 0, Column = 0
    DEATHKNIGHT_RUNIC_POWER_MASTERY             = 2020,  // TabId = 399, Tab = 1, Row = 0, Column = 1
    DEATHKNIGHT_TOUGHNESS                       = 1968,  // TabId = 399, Tab = 1, Row = 0, Column = 2
    DEATHKNIGHT_ICY_REACH                       = 2035,  // TabId = 399, Tab = 1, Row = 1, Column = 1
    DEATHKNIGHT_BLACK_ICE                       = 1973,  // TabId = 399, Tab = 1, Row = 1, Column = 2
    DEATHKNIGHT_NERVES_OF_COLD_STEEL            = 2022,  // TabId = 399, Tab = 1, Row = 1, Column = 3
    DEATHKNIGHT_ICY_TALONS                      = 2042,  // TabId = 399, Tab = 1, Row = 2, Column = 0
    DEATHKNIGHT_LICHBORNE                       = 2215,  // TabId = 399, Tab = 1, Row = 2, Column = 1
    DEATHKNIGHT_ANNIHILATION                    = 2048,  // TabId = 399, Tab = 1, Row = 2, Column = 2
    DEATHKNIGHT_KILLING_MACHINE                 = 2044,  // TabId = 399, Tab = 1, Row = 3, Column = 1
    DEATHKNIGHT_CHILL_OF_THE_GRAVE              = 1981,  // TabId = 399, Tab = 1, Row = 3, Column = 2
    DEATHKNIGHT_ENDLESS_WINTER                  = 1971,  // TabId = 399, Tab = 1, Row = 3, Column = 3
    DEATHKNIGHT_FRIGID_DREADPLATE               = 1990,  // TabId = 399, Tab = 1, Row = 4, Column = 1
    DEATHKNIGHT_GLACIER_ROT                     = 2030,  // TabId = 399, Tab = 1, Row = 4, Column = 2
    DEATHKNIGHT_DEATHCHILL                      = 1980,  // TabId = 399, Tab = 1, Row = 4, Column = 3
    DEATHKNIGHT_IMPROVED_ICY_TALONS             = 2223,  // TabId = 399, Tab = 1, Row = 5, Column = 0
    DEATHKNIGHT_MERCILESS_COMBAT                = 1993,  // TabId = 399, Tab = 1, Row = 5, Column = 1
    DEATHKNIGHT_RIME                            = 1992,  // TabId = 399, Tab = 1, Row = 5, Column = 2
    DEATHKNIGHT_CHILLBLAINS                     = 2260,  // TabId = 399, Tab = 1, Row = 6, Column = 0
    DEATHKNIGHT_HUNGERING_COLD                  = 1999,  // TabId = 399, Tab = 1, Row = 6, Column = 1
    DEATHKNIGHT_IMPROVED_FROST_PRESENCE         = 2029,  // TabId = 399, Tab = 1, Row = 6, Column = 2
    DEATHKNIGHT_THREAT_OF_THASSARIAN            = 2284,  // TabId = 399, Tab = 1, Row = 7, Column = 0
    DEATHKNIGHT_BLOOD_OF_THE_NORTH              = 2210,  // TabId = 399, Tab = 1, Row = 7, Column = 1
    DEATHKNIGHT_UNBREAKABLE_ARMOR               = 1979,  // TabId = 399, Tab = 1, Row = 7, Column = 2
    DEATHKNIGHT_ACCLIMATION                     = 1997,  // TabId = 399, Tab = 1, Row = 8, Column = 0
    DEATHKNIGHT_FROST_STRIKE                    = 1975,  // TabId = 399, Tab = 1, Row = 8, Column = 1
    DEATHKNIGHT_GUILE_OF_GOREFIEND              = 2040,  // TabId = 399, Tab = 1, Row = 8, Column = 2
    DEATHKNIGHT_TUNDRA_STALKER                  = 1998,  // TabId = 399, Tab = 1, Row = 9, Column = 1
    DEATHKNIGHT_HOWLING_BLAST                   = 1989,  // TabId = 399, Tab = 1, Row = 10, Column = 1
    DEATHKNIGHT_VICIOUS_STRIKES                 = 2082,  // TabId = 400, Tab = 2, Row = 0, Column = 0
    DEATHKNIGHT_VIRULENCE                       = 1932,  // TabId = 400, Tab = 2, Row = 0, Column = 1
    DEATHKNIGHT_ANTICIPATION                    = 2218,  // TabId = 400, Tab = 2, Row = 0, Column = 2
    DEATHKNIGHT_EPIDEMIC                        = 1963,  // TabId = 400, Tab = 2, Row = 1, Column = 0
    DEATHKNIGHT_MORBIDITY                       = 1933,  // TabId = 400, Tab = 2, Row = 1, Column = 1
    DEATHKNIGHT_UNHOLY_COMMAND                  = 2025,  // TabId = 400, Tab = 2, Row = 1, Column = 2
    DEATHKNIGHT_RAVENOUS_DEAD                   = 1934,  // TabId = 400, Tab = 2, Row = 1, Column = 3
    DEATHKNIGHT_OUTBREAK                        = 2008,  // TabId = 400, Tab = 2, Row = 2, Column = 0
    DEATHKNIGHT_NECROSIS                        = 2047,  // TabId = 400, Tab = 2, Row = 2, Column = 1
    DEATHKNIGHT_CORPSE_EXPLOSION                = 1985,  // TabId = 400, Tab = 2, Row = 2, Column = 2
    DEATHKNIGHT_ON_A_PALE_HORSE                 = 2039,  // TabId = 400, Tab = 2, Row = 3, Column = 1
    DEATHKNIGHT_BLOODCAKED_BLADE                = 2004,  // TabId = 400, Tab = 2, Row = 3, Column = 2
    DEATHKNIGHT_NIGHT_OF_THE_DEAD               = 2225,  // TabId = 400, Tab = 2, Row = 3, Column = 3
    DEATHKNIGHT_UNHOLY_BLIGHT                   = 1996,  // TabId = 400, Tab = 2, Row = 4, Column = 0
    DEATHKNIGHT_IMPURITY                        = 2005,  // TabId = 400, Tab = 2, Row = 4, Column = 1
    DEATHKNIGHT_DIRGE                           = 2011,  // TabId = 400, Tab = 2, Row = 4, Column = 2
    DEATHKNIGHT_DESECRATION                     = 2226,  // TabId = 400, Tab = 2, Row = 5, Column = 0
    DEATHKNIGHT_MAGIC_SUPPRESSION               = 2009,  // TabId = 400, Tab = 2, Row = 5, Column = 1
    DEATHKNIGHT_REAPING                         = 2001,  // TabId = 400, Tab = 2, Row = 5, Column = 2
    DEATHKNIGHT_MASTER_OF_GHOULS                = 1984,  // TabId = 400, Tab = 2, Row = 5, Column = 3
    DEATHKNIGHT_DESOLATION                      = 2285,  // TabId = 400, Tab = 2, Row = 6, Column = 0
    DEATHKNIGHT_ANTIMAGIC_ZONE                  = 2221,  // TabId = 400, Tab = 2, Row = 6, Column = 1
    DEATHKNIGHT_IMPROVED_UNHOLY_PRESENCE        = 2013,  // TabId = 400, Tab = 2, Row = 6, Column = 2
    DEATHKNIGHT_GHOUL_FRENZY                    = 2085,  // TabId = 400, Tab = 2, Row = 6, Column = 3
    DEATHKNIGHT_CRYPT_FEVER                     = 1962,  // TabId = 400, Tab = 2, Row = 7, Column = 1
    DEATHKNIGHT_BONE_SHIELD                     = 2007,  // TabId = 400, Tab = 2, Row = 7, Column = 2
    DEATHKNIGHT_WANDERING_PLAGUE                = 2003,  // TabId = 400, Tab = 2, Row = 8, Column = 0
    DEATHKNIGHT_EBON_PLAGUEBRINGER              = 2043,  // TabId = 400, Tab = 2, Row = 8, Column = 1
    DEATHKNIGHT_SCOURGE_STRIKE                  = 2216,  // TabId = 400, Tab = 2, Row = 8, Column = 2
    DEATHKNIGHT_RAGE_OF_RIVENDARE               = 2036,  // TabId = 400, Tab = 2, Row = 9, Column = 1
    DEATHKNIGHT_SUMMON_GARGOYLE                 = 2000   // TabId = 400, Tab = 2, Row = 10, Column = 1
};

enum TalentsDruid  // 0x400
{
    DRUID_FEROCITY                              = 796,   // TabId = 281, Tab = 1, Row = 0, Column = 1
    DRUID_FERAL_AGGRESSION                      = 795,   // TabId = 281, Tab = 1, Row = 0, Column = 2
    DRUID_FERAL_INSTINCT                        = 799,   // TabId = 281, Tab = 1, Row = 1, Column = 0
    DRUID_SAVAGE_FURY                           = 805,   // TabId = 281, Tab = 1, Row = 1, Column = 1
    DRUID_THICK_HIDE                            = 794,   // TabId = 281, Tab = 1, Row = 1, Column = 2
    DRUID_FERAL_SWIFTNESS                       = 807,   // TabId = 281, Tab = 1, Row = 2, Column = 0
    DRUID_SURVIVAL_INSTINCTS                    = 1162,  // TabId = 281, Tab = 1, Row = 2, Column = 1
    DRUID_SHARPENED_CLAWS                       = 798,   // TabId = 281, Tab = 1, Row = 2, Column = 2
    DRUID_SHREDDING_ATTACKS                     = 802,   // TabId = 281, Tab = 1, Row = 3, Column = 0
    DRUID_PREDATORY_STRIKES                     = 803,   // TabId = 281, Tab = 1, Row = 3, Column = 1
    DRUID_PRIMAL_FURY                           = 801,   // TabId = 281, Tab = 1, Row = 3, Column = 2
    DRUID_PRIMAL_PRECISION                      = 1914,  // TabId = 281, Tab = 1, Row = 3, Column = 3
    DRUID_BRUTAL_IMPACT                         = 797,   // TabId = 281, Tab = 1, Row = 4, Column = 0
    DRUID_FERAL_CHARGE                          = 804,   // TabId = 281, Tab = 1, Row = 4, Column = 2
    DRUID_NURTURING_INSTINCT                    = 1792,  // TabId = 281, Tab = 1, Row = 4, Column = 3
    DRUID_NATURAL_REACTION                      = 2242,  // TabId = 281, Tab = 1, Row = 5, Column = 0
    DRUID_HEART_OF_THE_WILD                     = 808,   // TabId = 281, Tab = 1, Row = 5, Column = 1
    DRUID_SURVIVAL_OF_THE_FITTEST               = 1794,  // TabId = 281, Tab = 1, Row = 5, Column = 2
    DRUID_LEADER_OF_THE_PACK                    = 809,   // TabId = 281, Tab = 1, Row = 6, Column = 1
    DRUID_IMPROVED_LEADER_OF_THE_PACK           = 1798,  // TabId = 281, Tab = 1, Row = 6, Column = 2
    DRUID_PRIMAL_TENACITY                       = 1793,  // TabId = 281, Tab = 1, Row = 6, Column = 3
    DRUID_PROTECTOR_OF_THE_PACK                 = 2241,  // TabId = 281, Tab = 1, Row = 7, Column = 0
    DRUID_PREDATORY_INSTINCTS                   = 1795,  // TabId = 281, Tab = 1, Row = 7, Column = 2
    DRUID_INFECTED_WOUNDS                       = 1919,  // TabId = 281, Tab = 1, Row = 7, Column = 3
    DRUID_KING_OF_THE_JUNGLE                    = 1921,  // TabId = 281, Tab = 1, Row = 8, Column = 0
    DRUID_MANGLE                                = 1796,  // TabId = 281, Tab = 1, Row = 8, Column = 1
    DRUID_IMPROVED_MANGLE                       = 1920,  // TabId = 281, Tab = 1, Row = 8, Column = 2
    DRUID_REND_AND_TEAR                         = 1918,  // TabId = 281, Tab = 1, Row = 9, Column = 1
    DRUID_PRIMAL_GORE                           = 2266,  // TabId = 281, Tab = 1, Row = 9, Column = 2
    DRUID_BERSERK                               = 1927,  // TabId = 281, Tab = 1, Row = 10, Column = 1
    DRUID_IMPROVED_MARK_OF_THE_WILD             = 821,   // TabId = 282, Tab = 2, Row = 0, Column = 0
    DRUID_NATURES_FOCUS                         = 823,   // TabId = 282, Tab = 2, Row = 0, Column = 1
    DRUID_FUROR                                 = 822,   // TabId = 282, Tab = 2, Row = 0, Column = 2
    DRUID_NATURALIST                            = 824,   // TabId = 282, Tab = 2, Row = 1, Column = 0
    DRUID_SUBTLETY                              = 841,   // TabId = 282, Tab = 2, Row = 1, Column = 1
    DRUID_NATURAL_SHAPESHIFTER                  = 826,   // TabId = 282, Tab = 2, Row = 1, Column = 2
    DRUID_INTENSITY                             = 829,   // TabId = 282, Tab = 2, Row = 2, Column = 0
    DRUID_OMEN_OF_CLARITY                       = 827,   // TabId = 282, Tab = 2, Row = 2, Column = 1
    DRUID_MASTER_SHAPESHIFTER                   = 1915,  // TabId = 282, Tab = 2, Row = 2, Column = 2
    DRUID_TRANQUIL_SPIRIT                       = 843,   // TabId = 282, Tab = 2, Row = 3, Column = 1
    DRUID_IMPROVED_REJUVENATION                 = 830,   // TabId = 282, Tab = 2, Row = 3, Column = 2
    DRUID_NATURES_SWIFTNESS                     = 831,   // TabId = 282, Tab = 2, Row = 4, Column = 0
    DRUID_GIFT_OF_NATURE                        = 828,   // TabId = 282, Tab = 2, Row = 4, Column = 1
    DRUID_IMPROVED_TRANQUILITY                  = 842,   // TabId = 282, Tab = 2, Row = 4, Column = 3
    DRUID_EMPOWERED_TOUCH                       = 1788,  // TabId = 282, Tab = 2, Row = 5, Column = 0
    DRUID_NATURES_BOUNTY                        = 825,   // TabId = 282, Tab = 2, Row = 5, Column = 2
    DRUID_LIVING_SPIRIT                         = 1797,  // TabId = 282, Tab = 2, Row = 6, Column = 0
    DRUID_SWIFTMEND                             = 844,   // TabId = 282, Tab = 2, Row = 6, Column = 1
    DRUID_NATURAL_PERFECTION                    = 1790,  // TabId = 282, Tab = 2, Row = 6, Column = 2
    DRUID_EMPOWERED_REJUVENATION                = 1789,  // TabId = 282, Tab = 2, Row = 7, Column = 1
    DRUID_LIVING_SEED                           = 1922,  // TabId = 282, Tab = 2, Row = 7, Column = 2
    DRUID_REVITALIZE                            = 1929,  // TabId = 282, Tab = 2, Row = 8, Column = 0
    DRUID_TREE_OF_LIFE                          = 1791,  // TabId = 282, Tab = 2, Row = 8, Column = 1
    DRUID_IMPROVED_TREE_OF_LIFE                 = 1930,  // TabId = 282, Tab = 2, Row = 8, Column = 2
    DRUID_IMPROVED_BARKSKIN                     = 2264,  // TabId = 282, Tab = 2, Row = 9, Column = 0
    DRUID_GIFT_OF_THE_EARTHMOTHER               = 1916,  // TabId = 282, Tab = 2, Row = 9, Column = 2
    DRUID_WILD_GROWTH                           = 1917,  // TabId = 282, Tab = 2, Row = 10, Column = 1
    DRUID_STARLIGHT_WRATH                       = 762,   // TabId = 283, Tab = 0, Row = 0, Column = 1
    DRUID_GENESIS                               = 2238,  // TabId = 283, Tab = 0, Row = 0, Column = 2
    DRUID_MOONGLOW                              = 783,   // TabId = 283, Tab = 0, Row = 1, Column = 0
    DRUID_NATURES_MAJESTY                       = 1822,  // TabId = 283, Tab = 0, Row = 1, Column = 1
    DRUID_IMPROVED_MOONFIRE                     = 763,   // TabId = 283, Tab = 0, Row = 1, Column = 3
    DRUID_BRAMBLES                              = 782,   // TabId = 283, Tab = 0, Row = 2, Column = 0
    DRUID_NATURES_GRACE                         = 789,   // TabId = 283, Tab = 0, Row = 2, Column = 1
    DRUID_NATURES_SPLENDOR                      = 2240,  // TabId = 283, Tab = 0, Row = 2, Column = 2
    DRUID_NATURES_REACH                         = 764,   // TabId = 283, Tab = 0, Row = 2, Column = 3
    DRUID_VENGEANCE                             = 792,   // TabId = 283, Tab = 0, Row = 3, Column = 1
    DRUID_CELESTIAL_FOCUS                       = 784,   // TabId = 283, Tab = 0, Row = 3, Column = 2
    DRUID_LUNAR_GUIDANCE                        = 1782,  // TabId = 283, Tab = 0, Row = 4, Column = 0
    DRUID_INSECT_SWARM                          = 788,   // TabId = 283, Tab = 0, Row = 4, Column = 1
    DRUID_IMPROVED_INSECT_SWARM                 = 2239,  // TabId = 283, Tab = 0, Row = 4, Column = 2
    DRUID_DREAMSTATE                            = 1784,  // TabId = 283, Tab = 0, Row = 5, Column = 0
    DRUID_MOONFURY                              = 790,   // TabId = 283, Tab = 0, Row = 5, Column = 1
    DRUID_BALANCE_OF_POWER                      = 1783,  // TabId = 283, Tab = 0, Row = 5, Column = 2
    DRUID_MOONKIN_FORM                          = 793,   // TabId = 283, Tab = 0, Row = 6, Column = 1
    DRUID_IMPROVED_MOONKIN_FORM                 = 1912,  // TabId = 283, Tab = 0, Row = 6, Column = 2
    DRUID_IMPROVED_FAERIE_FIRE                  = 1785,  // TabId = 283, Tab = 0, Row = 6, Column = 3
    DRUID_OWLKIN_FRENZY                         = 1913,  // TabId = 283, Tab = 0, Row = 7, Column = 0
    DRUID_WRATH_OF_CENARIUS                     = 1786,  // TabId = 283, Tab = 0, Row = 7, Column = 2
    DRUID_ECLIPSE                               = 1924,  // TabId = 283, Tab = 0, Row = 8, Column = 0
    DRUID_TYPHOON                               = 1923,  // TabId = 283, Tab = 0, Row = 8, Column = 1
    DRUID_FORCE_OF_NATURE                       = 1787,  // TabId = 283, Tab = 0, Row = 8, Column = 2
    DRUID_GALE_WINDS                            = 1925,  // TabId = 283, Tab = 0, Row = 8, Column = 3
    DRUID_EARTH_AND_MOON                        = 1928,  // TabId = 283, Tab = 0, Row = 9, Column = 1
    DRUID_STARFALL                              = 1926   // TabId = 283, Tab = 0, Row = 10, Column = 1
};

enum TalentsHunter  // 0x004
{
    HUNTER_IMPROVED_ASPECT_OF_THE_HAWK          = 1382,  // TabId = 361, Tab = 0, Row = 0, Column = 1
    HUNTER_ENDURANCE_TRAINING                   = 1389,  // TabId = 361, Tab = 0, Row = 0, Column = 2
    HUNTER_FOCUSED_FIRE                         = 1624,  // TabId = 361, Tab = 0, Row = 1, Column = 0
    HUNTER_IMPROVED_ASPECT_OF_THE_MONKEY        = 1381,  // TabId = 361, Tab = 0, Row = 1, Column = 1
    HUNTER_THICK_HIDE                           = 1395,  // TabId = 361, Tab = 0, Row = 1, Column = 2
    HUNTER_IMPROVED_REVIVE_PET                  = 1625,  // TabId = 361, Tab = 0, Row = 1, Column = 3
    HUNTER_PATHFINDING                          = 1384,  // TabId = 361, Tab = 0, Row = 2, Column = 0
    HUNTER_ASPECT_MASTERY                       = 2138,  // TabId = 361, Tab = 0, Row = 2, Column = 1
    HUNTER_UNLEASHED_FURY                       = 1396,  // TabId = 361, Tab = 0, Row = 2, Column = 2
    HUNTER_IMPROVED_MEND_PET                    = 1385,  // TabId = 361, Tab = 0, Row = 3, Column = 1
    HUNTER_FEROCITY                             = 1393,  // TabId = 361, Tab = 0, Row = 3, Column = 2
    HUNTER_SPIRIT_BOND                          = 1388,  // TabId = 361, Tab = 0, Row = 4, Column = 0
    HUNTER_INTIMIDATION                         = 1387,  // TabId = 361, Tab = 0, Row = 4, Column = 1
    HUNTER_BESTIAL_DISCIPLINE                   = 1390,  // TabId = 361, Tab = 0, Row = 4, Column = 3
    HUNTER_ANIMAL_HANDLER                       = 1799,  // TabId = 361, Tab = 0, Row = 5, Column = 0
    HUNTER_FRENZY                               = 1397,  // TabId = 361, Tab = 0, Row = 5, Column = 2
    HUNTER_FEROCIOUS_INSPIRATION                = 1800,  // TabId = 361, Tab = 0, Row = 6, Column = 0
    HUNTER_BESTIAL_WRATH                        = 1386,  // TabId = 361, Tab = 0, Row = 6, Column = 1
    HUNTER_CATLIKE_REFLEXES                     = 1801,  // TabId = 361, Tab = 0, Row = 6, Column = 2
    HUNTER_INVIGORATION                         = 2136,  // TabId = 361, Tab = 0, Row = 7, Column = 0
    HUNTER_SERPENTS_SWIFTNESS                   = 1802,  // TabId = 361, Tab = 0, Row = 7, Column = 2
    HUNTER_LONGEVITY                            = 2140,  // TabId = 361, Tab = 0, Row = 8, Column = 0
    HUNTER_THE_BEAST_WITHIN                     = 1803,  // TabId = 361, Tab = 0, Row = 8, Column = 1
    HUNTER_COBRA_STRIKES                        = 2137,  // TabId = 361, Tab = 0, Row = 8, Column = 2
    HUNTER_KINDRED_SPIRITS                      = 2227,  // TabId = 361, Tab = 0, Row = 9, Column = 1
    HUNTER_BEAST_MASTERY                        = 2139,  // TabId = 361, Tab = 0, Row = 10, Column = 1
    HUNTER_IMPROVED_TRACKING                    = 1623,  // TabId = 362, Tab = 2, Row = 0, Column = 0
    HUNTER_HAWK_EYE                             = 1820,  // TabId = 362, Tab = 2, Row = 0, Column = 1
    HUNTER_SAVAGE_STRIKES                       = 1621,  // TabId = 362, Tab = 2, Row = 0, Column = 2
    HUNTER_SUREFOOTED                           = 1310,  // TabId = 362, Tab = 2, Row = 1, Column = 0
    HUNTER_ENTRAPMENT                           = 1304,  // TabId = 362, Tab = 2, Row = 1, Column = 1
    HUNTER_TRAP_MASTERY                         = 1305,  // TabId = 362, Tab = 2, Row = 1, Column = 2
    HUNTER_SURVIVAL_INSTINCTS                   = 1810,  // TabId = 362, Tab = 2, Row = 1, Column = 3
    HUNTER_SURVIVALIST                          = 1622,  // TabId = 362, Tab = 2, Row = 2, Column = 0
    HUNTER_SCATTER_SHOT                         = 1814,  // TabId = 362, Tab = 2, Row = 2, Column = 1
    HUNTER_DEFLECTION                           = 1311,  // TabId = 362, Tab = 2, Row = 2, Column = 2
    HUNTER_SURVIVAL_TACTICS                     = 1309,  // TabId = 362, Tab = 2, Row = 2, Column = 3
    HUNTER_TNT                                  = 2229,  // TabId = 362, Tab = 2, Row = 3, Column = 1
    HUNTER_LOCK_AND_LOAD                        = 1306,  // TabId = 362, Tab = 2, Row = 3, Column = 3
    HUNTER_HUNTER_VS_WILD                       = 2228,  // TabId = 362, Tab = 2, Row = 4, Column = 0
    HUNTER_KILLER_INSTINCT                      = 1321,  // TabId = 362, Tab = 2, Row = 4, Column = 1
    HUNTER_COUNTERATTACK                        = 1312,  // TabId = 362, Tab = 2, Row = 4, Column = 2
    HUNTER_LIGHTNING_REFLEXES                   = 1303,  // TabId = 362, Tab = 2, Row = 5, Column = 0
    HUNTER_RESOURCEFULNESS                      = 1809,  // TabId = 362, Tab = 2, Row = 5, Column = 2
    HUNTER_EXPOSE_WEAKNESS                      = 1812,  // TabId = 362, Tab = 2, Row = 6, Column = 0
    HUNTER_WYVERN_STING                         = 1325,  // TabId = 362, Tab = 2, Row = 6, Column = 1
    HUNTER_THRILL_OF_THE_HUNT                   = 1811,  // TabId = 362, Tab = 2, Row = 6, Column = 2
    HUNTER_MASTER_TACTICIAN                     = 1813,  // TabId = 362, Tab = 2, Row = 7, Column = 0
    HUNTER_NOXIOUS_STINGS                       = 2141,  // TabId = 362, Tab = 2, Row = 7, Column = 1
    HUNTER_POINT_OF_NO_ESCAPE                   = 2142,  // TabId = 362, Tab = 2, Row = 8, Column = 0
    HUNTER_BLACK_ARROW                          = 1322,  // TabId = 362, Tab = 2, Row = 8, Column = 1
    HUNTER_SNIPER_TRAINING                      = 2143,  // TabId = 362, Tab = 2, Row = 8, Column = 3
    HUNTER_HUNTING_PARTY                        = 2144,  // TabId = 362, Tab = 2, Row = 9, Column = 2
    HUNTER_EXPLOSIVE_SHOT                       = 2145,  // TabId = 362, Tab = 2, Row = 10, Column = 1
    HUNTER_IMPROVED_CONCUSSIVE_SHOT             = 1341,  // TabId = 363, Tab = 1, Row = 0, Column = 0
    HUNTER_FOCUSED_AIM                          = 2197,  // TabId = 363, Tab = 1, Row = 0, Column = 1
    HUNTER_LETHAL_SHOTS                         = 1344,  // TabId = 363, Tab = 1, Row = 0, Column = 2
    HUNTER_CAREFUL_AIM                          = 1806,  // TabId = 363, Tab = 1, Row = 1, Column = 0
    HUNTER_IMPROVED_HUNTERS_MARK                = 1343,  // TabId = 363, Tab = 1, Row = 1, Column = 1
    HUNTER_MORTAL_SHOTS                         = 1349,  // TabId = 363, Tab = 1, Row = 1, Column = 2
    HUNTER_GO_FOR_THE_THROAT                    = 1818,  // TabId = 363, Tab = 1, Row = 2, Column = 0
    HUNTER_IMPROVED_ARCANE_SHOT                 = 1346,  // TabId = 363, Tab = 1, Row = 2, Column = 1
    HUNTER_AIMED_SHOT                           = 1345,  // TabId = 363, Tab = 1, Row = 2, Column = 2
    HUNTER_RAPID_KILLING                        = 1819,  // TabId = 363, Tab = 1, Row = 2, Column = 3
    HUNTER_IMPROVED_STINGS                      = 1348,  // TabId = 363, Tab = 1, Row = 3, Column = 1
    HUNTER_EFFICIENCY                           = 1342,  // TabId = 363, Tab = 1, Row = 3, Column = 2
    HUNTER_CONCUSSIVE_BARRAGE                   = 1351,  // TabId = 363, Tab = 1, Row = 4, Column = 0
    HUNTER_READINESS                            = 1353,  // TabId = 363, Tab = 1, Row = 4, Column = 1
    HUNTER_BARRAGE                              = 1347,  // TabId = 363, Tab = 1, Row = 4, Column = 2
    HUNTER_COMBAT_EXPERIENCE                    = 1804,  // TabId = 363, Tab = 1, Row = 5, Column = 0
    HUNTER_RANGED_WEAPON_SPECIALIZATION         = 1362,  // TabId = 363, Tab = 1, Row = 5, Column = 3
    HUNTER_PIERCING_SHOTS                       = 2130,  // TabId = 363, Tab = 1, Row = 6, Column = 0
    HUNTER_TRUESHOT_AURA                        = 1361,  // TabId = 363, Tab = 1, Row = 6, Column = 1
    HUNTER_IMPROVED_BARRAGE                     = 1821,  // TabId = 363, Tab = 1, Row = 6, Column = 2
    HUNTER_MASTER_MARKSMAN                      = 1807,  // TabId = 363, Tab = 1, Row = 7, Column = 1
    HUNTER_RAPID_RECUPERATION                   = 2131,  // TabId = 363, Tab = 1, Row = 7, Column = 2
    HUNTER_WILD_QUIVER                          = 2132,  // TabId = 363, Tab = 1, Row = 8, Column = 0
    HUNTER_SILENCING_SHOT                       = 1808,  // TabId = 363, Tab = 1, Row = 8, Column = 1
    HUNTER_IMPROVED_STEADY_SHOT                 = 2133,  // TabId = 363, Tab = 1, Row = 8, Column = 2
    HUNTER_MARKED_FOR_DEATH                     = 2134,  // TabId = 363, Tab = 1, Row = 9, Column = 1
    HUNTER_CHIMERA_SHOT                         = 2135  // TabId = 363, Tab = 1, Row = 10, Column = 1
};

enum TalentsMage  // 0x080
{
    MAGE_IMPROVED_FIRE_BLAST                    = 27,    // TabId = 41, Tab = 1, Row = 0, Column = 0
    MAGE_INCINERATION                           = 1141,  // TabId = 41, Tab = 1, Row = 0, Column = 1
    MAGE_IMPROVED_FIREBALL                      = 26,    // TabId = 41, Tab = 1, Row = 0, Column = 2
    MAGE_IGNITE                                 = 34,    // TabId = 41, Tab = 1, Row = 1, Column = 0
    MAGE_BURNING_DETERMINATION                  = 2212,  // TabId = 41, Tab = 1, Row = 1, Column = 1
    MAGE_WORLD_IN_FLAMES                        = 31,    // TabId = 41, Tab = 1, Row = 1, Column = 2
    MAGE_FLAME_THROWING                         = 28,    // TabId = 41, Tab = 1, Row = 2, Column = 0
    MAGE_IMPACT                                 = 30,    // TabId = 41, Tab = 1, Row = 2, Column = 1
    MAGE_PYROBLAST                              = 29,    // TabId = 41, Tab = 1, Row = 2, Column = 2
    MAGE_BURNING_SOUL                           = 23,    // TabId = 41, Tab = 1, Row = 2, Column = 3
    MAGE_IMPROVED_SCORCH                        = 25,    // TabId = 41, Tab = 1, Row = 3, Column = 0
    MAGE_MOLTEN_SHIELDS                         = 24,    // TabId = 41, Tab = 1, Row = 3, Column = 1
    MAGE_MASTER_OF_ELEMENTS                     = 1639,  // TabId = 41, Tab = 1, Row = 3, Column = 3
    MAGE_PLAYING_WITH_FIRE                      = 1730,  // TabId = 41, Tab = 1, Row = 4, Column = 0
    MAGE_CRITICAL_MASS                          = 33,    // TabId = 41, Tab = 1, Row = 4, Column = 1
    MAGE_BLAST_WAVE                             = 32,    // TabId = 41, Tab = 1, Row = 4, Column = 2
    MAGE_BLAZING_SPEED                          = 1731,  // TabId = 41, Tab = 1, Row = 5, Column = 0
    MAGE_FIRE_POWER                             = 35,    // TabId = 41, Tab = 1, Row = 5, Column = 2
    MAGE_PYROMANIAC                             = 1733,  // TabId = 41, Tab = 1, Row = 6, Column = 0
    MAGE_COMBUSTION                             = 36,    // TabId = 41, Tab = 1, Row = 6, Column = 1
    MAGE_MOLTEN_FURY                            = 1732,  // TabId = 41, Tab = 1, Row = 6, Column = 2
    MAGE_FIERY_PAYBACK                          = 1848,  // TabId = 41, Tab = 1, Row = 7, Column = 0
    MAGE_EMPOWERED_FIRE                         = 1734,  // TabId = 41, Tab = 1, Row = 7, Column = 2
    MAGE_FIRESTARTER                            = 1849,  // TabId = 41, Tab = 1, Row = 8, Column = 0
    MAGE_DRAGONS_BREATH                         = 1735,  // TabId = 41, Tab = 1, Row = 8, Column = 1
    MAGE_HOT_STREAK                             = 1850,  // TabId = 41, Tab = 1, Row = 8, Column = 2
    MAGE_BURNOUT                                = 1851,  // TabId = 41, Tab = 1, Row = 9, Column = 1
    MAGE_LIVING_BOMB                            = 1852,  // TabId = 41, Tab = 1, Row = 10, Column = 1
    MAGE_FROSTBITE                              = 38,    // TabId = 61, Tab = 2, Row = 0, Column = 0
    MAGE_IMPROVED_FROSTBOLT                     = 37,    // TabId = 61, Tab = 2, Row = 0, Column = 1
    MAGE_ICE_FLOES                              = 62,    // TabId = 61, Tab = 2, Row = 0, Column = 2
    MAGE_ICE_SHARDS                             = 73,    // TabId = 61, Tab = 2, Row = 1, Column = 0
    MAGE_FROST_WARDING                          = 70,    // TabId = 61, Tab = 2, Row = 1, Column = 1
    MAGE_PRECISION                              = 1649,  // TabId = 61, Tab = 2, Row = 1, Column = 2
    MAGE_PERMAFROST                             = 65,    // TabId = 61, Tab = 2, Row = 1, Column = 3
    MAGE_PIERCING_ICE                           = 61,    // TabId = 61, Tab = 2, Row = 2, Column = 0
    MAGE_ICY_VEINS                              = 69,    // TabId = 61, Tab = 2, Row = 2, Column = 1
    MAGE_IMPROVED_BLIZZARD                      = 63,    // TabId = 61, Tab = 2, Row = 2, Column = 2
    MAGE_ARCTIC_REACH                           = 741,   // TabId = 61, Tab = 2, Row = 3, Column = 0
    MAGE_FROST_CHANNELING                       = 66,    // TabId = 61, Tab = 2, Row = 3, Column = 1
    MAGE_SHATTER                                = 67,    // TabId = 61, Tab = 2, Row = 3, Column = 2
    MAGE_COLD_SNAP                              = 72,    // TabId = 61, Tab = 2, Row = 4, Column = 1
    MAGE_IMPROVED_CONE_OF_COLD                  = 64,    // TabId = 61, Tab = 2, Row = 4, Column = 2
    MAGE_FROZEN_CORE                            = 1736,  // TabId = 61, Tab = 2, Row = 4, Column = 3
    MAGE_COLD_AS_ICE                            = 1737,  // TabId = 61, Tab = 2, Row = 5, Column = 0
    MAGE_WINTERS_CHILL                          = 68,    // TabId = 61, Tab = 2, Row = 5, Column = 2
    MAGE_SHATTERED_BARRIER                      = 2214,  // TabId = 61, Tab = 2, Row = 6, Column = 0
    MAGE_ICE_BARRIER                            = 71,    // TabId = 61, Tab = 2, Row = 6, Column = 1
    MAGE_ARCTIC_WINDS                           = 1738,  // TabId = 61, Tab = 2, Row = 6, Column = 2
    MAGE_EMPOWERED_FROSTBOLT                    = 1740,  // TabId = 61, Tab = 2, Row = 7, Column = 1
    MAGE_FINGERS_OF_FROST                       = 1853,  // TabId = 61, Tab = 2, Row = 7, Column = 2
    MAGE_BRAIN_FREEZE                           = 1854,  // TabId = 61, Tab = 2, Row = 8, Column = 0
    MAGE_SUMMON_WATER_ELEMENTAL                 = 1741,  // TabId = 61, Tab = 2, Row = 8, Column = 1
    MAGE_ENDURING_WINTER                        = 1855,  // TabId = 61, Tab = 2, Row = 8, Column = 2
    MAGE_CHILLD_TO_THE_BONE                     = 1856,  // TabId = 61, Tab = 2, Row = 9, Column = 1
    MAGE_DEEP_FREEZE                            = 1857,  // TabId = 61, Tab = 2, Row = 10, Column = 1
    MAGE_ARCANE_SUBTLETY                        = 74,    // TabId = 81, Tab = 0, Row = 0, Column = 0
    MAGE_ARCANE_FOCUS                           = 76,    // TabId = 81, Tab = 0, Row = 0, Column = 1
    MAGE_ARCANE_STABILITY                       = 80,    // TabId = 81, Tab = 0, Row = 0, Column = 2
    MAGE_ARCANE_FORTITUDE                       = 85,    // TabId = 81, Tab = 0, Row = 1, Column = 0
    MAGE_MAGIC_ABSORPTION                       = 1650,  // TabId = 81, Tab = 0, Row = 1, Column = 1
    MAGE_ARCANE_CONCENTRATION                   = 75,    // TabId = 81, Tab = 0, Row = 1, Column = 2
    MAGE_MAGIC_ATTUNEMENT                       = 82,    // TabId = 81, Tab = 0, Row = 2, Column = 0
    MAGE_SPELL_IMPACT                           = 81,    // TabId = 81, Tab = 0, Row = 2, Column = 1
    MAGE_STUDENT_OF_THE_MIND                    = 1845,  // TabId = 81, Tab = 0, Row = 2, Column = 2
    MAGE_FOCUS_MAGIC                            = 2211,  // TabId = 81, Tab = 0, Row = 2, Column = 3
    MAGE_ARCANE_SHIELDING                       = 83,    // TabId = 81, Tab = 0, Row = 3, Column = 0
    MAGE_IMPROVED_COUNTERSPELL                  = 88,    // TabId = 81, Tab = 0, Row = 3, Column = 1
    MAGE_ARCANE_MEDITATION                      = 1142,  // TabId = 81, Tab = 0, Row = 3, Column = 2
    MAGE_TORMENT_THE_WEAK                       = 2222,  // TabId = 81, Tab = 0, Row = 3, Column = 3
    MAGE_IMPROVED_BLINK                         = 1724,  // TabId = 81, Tab = 0, Row = 4, Column = 0
    MAGE_PRESENCE_OF_MIND                       = 86,    // TabId = 81, Tab = 0, Row = 4, Column = 1
    MAGE_ARCANE_MIND                            = 77,    // TabId = 81, Tab = 0, Row = 4, Column = 3
    MAGE_PRISMATIC_CLOAK                        = 1726,  // TabId = 81, Tab = 0, Row = 5, Column = 0
    MAGE_ARCANE_INSTABILITY                     = 421,   // TabId = 81, Tab = 0, Row = 5, Column = 1
    MAGE_ARCANE_POTENCY                         = 1725,  // TabId = 81, Tab = 0, Row = 5, Column = 2
    MAGE_ARCANE_EMPOWERMENT                     = 1727,  // TabId = 81, Tab = 0, Row = 6, Column = 0
    MAGE_ARCANE_POWER                           = 87,    // TabId = 81, Tab = 0, Row = 6, Column = 1
    MAGE_INCANTERS_ABSORPTION                   = 1844,  // TabId = 81, Tab = 0, Row = 6, Column = 2
    MAGE_ARCANE_FLOWS                           = 1843,  // TabId = 81, Tab = 0, Row = 7, Column = 1
    MAGE_MIND_MASTERY                           = 1728,  // TabId = 81, Tab = 0, Row = 7, Column = 2
    MAGE_SLOW                                   = 1729,  // TabId = 81, Tab = 0, Row = 8, Column = 1
    MAGE_MISSILE_BARRAGE                        = 2209,  // TabId = 81, Tab = 0, Row = 8, Column = 2
    MAGE_NETHERWIND_PRESENCE                    = 1846,  // TabId = 81, Tab = 0, Row = 9, Column = 1
    MAGE_SPELL_POWER                            = 1826,  // TabId = 81, Tab = 0, Row = 9, Column = 2
    MAGE_ARCANE_BARRAGE                         = 1847  // TabId = 81, Tab = 0, Row = 10, Column = 1
};

enum TalentsPaladin  // 0x002
{
    PALADIN_DEFLECTION                          = 1403,  // TabId = 381, Tab = 2, Row = 0, Column = 1
    PALADIN_BENEDICTION                         = 1407,  // TabId = 381, Tab = 2, Row = 0, Column = 2
    PALADIN_IMPROVED_JUDGEMENTS                 = 1631,  // TabId = 381, Tab = 2, Row = 1, Column = 0
    PALADIN_HEART_OF_THE_CRUSADER               = 1464,  // TabId = 381, Tab = 2, Row = 1, Column = 1
    PALADIN_IMPROVED_BLESSING_OF_MIGHT          = 1401,  // TabId = 381, Tab = 2, Row = 1, Column = 2
    PALADIN_VINDICATION                         = 1633,  // TabId = 381, Tab = 2, Row = 2, Column = 0
    PALADIN_CONVICTION                          = 1411,  // TabId = 381, Tab = 2, Row = 2, Column = 1
    PALADIN_SEAL_OF_COMMAND                     = 1481,  // TabId = 381, Tab = 2, Row = 2, Column = 2
    PALADIN_PURSUIT_OF_JUSTICE                  = 1634,  // TabId = 381, Tab = 2, Row = 2, Column = 3
    PALADIN_EYE_FOR_AN_EYE                      = 1632,  // TabId = 381, Tab = 2, Row = 3, Column = 0
    PALADIN_SANCTITY_OF_BATTLE                  = 1761,  // TabId = 381, Tab = 2, Row = 3, Column = 2
    PALADIN_CRUSADE                             = 1755,  // TabId = 381, Tab = 2, Row = 3, Column = 3
    PALADIN_TWOHANDED_WEAPON_SPECIALIZATION     = 1410,  // TabId = 381, Tab = 2, Row = 4, Column = 0
    PALADIN_SANCTIFIED_RETRIBUTION              = 1756,  // TabId = 381, Tab = 2, Row = 4, Column = 2
    PALADIN_VENGEANCE                           = 1402,  // TabId = 381, Tab = 2, Row = 5, Column = 1
    PALADIN_DIVINE_PURPOSE                      = 1757,  // TabId = 381, Tab = 2, Row = 5, Column = 2
    PALADIN_THE_ART_OF_WAR                      = 2176,  // TabId = 381, Tab = 2, Row = 6, Column = 0
    PALADIN_REPENTANCE                          = 1441,  // TabId = 381, Tab = 2, Row = 6, Column = 1
    PALADIN_JUDGEMENTS_OF_THE_WISE              = 1758,  // TabId = 381, Tab = 2, Row = 6, Column = 2
    PALADIN_FANATICISM                          = 1759,  // TabId = 381, Tab = 2, Row = 7, Column = 1
    PALADIN_SANCTIFIED_WRATH                    = 2147,  // TabId = 381, Tab = 2, Row = 7, Column = 2
    PALADIN_SWIFT_RETRIBUTION                   = 2148,  // TabId = 381, Tab = 2, Row = 8, Column = 0
    PALADIN_CRUSADER_STRIKE                     = 1823,  // TabId = 381, Tab = 2, Row = 8, Column = 1
    PALADIN_SHEATH_OF_LIGHT                     = 2179,  // TabId = 381, Tab = 2, Row = 8, Column = 2
    PALADIN_RIGHTEOUS_VENGEANCE                 = 2149,  // TabId = 381, Tab = 2, Row = 9, Column = 1
    PALADIN_DIVINE_STORM                        = 2150,  // TabId = 381, Tab = 2, Row = 10, Column = 1
    PALADIN_SPIRITUAL_FOCUS                     = 1432,  // TabId = 382, Tab = 0, Row = 0, Column = 1
    PALADIN_SEALS_OF_THE_PURE                   = 1463,  // TabId = 382, Tab = 0, Row = 0, Column = 2
    PALADIN_HEALING_LIGHT                       = 1444,  // TabId = 382, Tab = 0, Row = 1, Column = 0
    PALADIN_DIVINE_INTELLECT                    = 1449,  // TabId = 382, Tab = 0, Row = 1, Column = 1
    PALADIN_UNYIELDING_FAITH                    = 1628,  // TabId = 382, Tab = 0, Row = 1, Column = 2
    PALADIN_AURA_MASTERY                        = 1435,  // TabId = 382, Tab = 0, Row = 2, Column = 0
    PALADIN_ILLUMINATION                        = 1461,  // TabId = 382, Tab = 0, Row = 2, Column = 1
    PALADIN_IMPROVED_LAY_ON_HANDS               = 1443,  // TabId = 382, Tab = 0, Row = 2, Column = 2
    PALADIN_IMPROVED_CONCENTRATION_AURA         = 1450,  // TabId = 382, Tab = 0, Row = 3, Column = 0
    PALADIN_IMPROVED_BLESSING_OF_WISDOM         = 1446,  // TabId = 382, Tab = 0, Row = 3, Column = 2
    PALADIN_BLESSED_HANDS                       = 2198,  // TabId = 382, Tab = 0, Row = 3, Column = 3
    PALADIN_PURE_OF_HEART                       = 1742,  // TabId = 382, Tab = 0, Row = 4, Column = 0
    PALADIN_DIVINE_FAVOR                        = 1433,  // TabId = 382, Tab = 0, Row = 4, Column = 1
    PALADIN_SANCTIFIED_LIGHT                    = 1465,  // TabId = 382, Tab = 0, Row = 4, Column = 2
    PALADIN_PURIFYING_POWER                     = 1743,  // TabId = 382, Tab = 0, Row = 5, Column = 0
    PALADIN_HOLY_POWER                          = 1627,  // TabId = 382, Tab = 0, Row = 5, Column = 2
    PALADIN_LIGHTS_GRACE                        = 1745,  // TabId = 382, Tab = 0, Row = 6, Column = 0
    PALADIN_HOLY_SHOCK                          = 1502,  // TabId = 382, Tab = 0, Row = 6, Column = 1
    PALADIN_BLESSED_LIFE                        = 1744,  // TabId = 382, Tab = 0, Row = 6, Column = 2
    PALADIN_SACRED_CLEANSING                    = 2190,  // TabId = 382, Tab = 0, Row = 7, Column = 0
    PALADIN_HOLY_GUIDANCE                       = 1746,  // TabId = 382, Tab = 0, Row = 7, Column = 2
    PALADIN_DIVINE_ILLUMINATION                 = 1747,  // TabId = 382, Tab = 0, Row = 8, Column = 0
    PALADIN_JUDGEMENTS_OF_THE_PURE              = 2199,  // TabId = 382, Tab = 0, Row = 8, Column = 2
    PALADIN_INFUSION_OF_LIGHT                   = 2193,  // TabId = 382, Tab = 0, Row = 9, Column = 1
    PALADIN_ENLIGHTENED_JUDGEMENTS              = 2191,  // TabId = 382, Tab = 0, Row = 9, Column = 2
    PALADIN_BEACON_OF_LIGHT                     = 2192,  // TabId = 382, Tab = 0, Row = 10, Column = 1
    PALADIN_DIVINITY                            = 1442,  // TabId = 383, Tab = 1, Row = 0, Column = 1
    PALADIN_DIVINE_STRENGTH                     = 2185,  // TabId = 383, Tab = 1, Row = 0, Column = 2
    PALADIN_STOICISM                            = 1748,  // TabId = 383, Tab = 1, Row = 1, Column = 0
    PALADIN_GUARDIANS_FAVOR                     = 1425,  // TabId = 383, Tab = 1, Row = 1, Column = 1
    PALADIN_ANTICIPATION                        = 1629,  // TabId = 383, Tab = 1, Row = 1, Column = 2
    PALADIN_DIVINE_SACRIFICE                    = 2280,  // TabId = 383, Tab = 1, Row = 2, Column = 0
    PALADIN_IMPROVED_RIGHTEOUS_FURY             = 1501,  // TabId = 383, Tab = 1, Row = 2, Column = 1
    PALADIN_TOUGHNESS                           = 1423,  // TabId = 383, Tab = 1, Row = 2, Column = 2
    PALADIN_DIVINE_GUARDIAN                     = 2281,  // TabId = 383, Tab = 1, Row = 3, Column = 0
    PALADIN_IMPROVED_HAMMER_OF_JUSTICE          = 1521,  // TabId = 383, Tab = 1, Row = 3, Column = 1
    PALADIN_IMPROVED_DEVOTION_AURA              = 1422,  // TabId = 383, Tab = 1, Row = 3, Column = 2
    PALADIN_BLESSING_OF_SANCTUARY               = 1431,  // TabId = 383, Tab = 1, Row = 4, Column = 1
    PALADIN_RECKONING                           = 1426,  // TabId = 383, Tab = 1, Row = 4, Column = 2
    PALADIN_SACRED_DUTY                         = 1750,  // TabId = 383, Tab = 1, Row = 5, Column = 0
    PALADIN_ONEHANDED_WEAPON_SPECIALIZATION     = 1429,  // TabId = 383, Tab = 1, Row = 5, Column = 2
    PALADIN_SPIRITUAL_ATTUNEMENT                = 2282,  // TabId = 383, Tab = 1, Row = 6, Column = 0
    PALADIN_HOLY_SHIELD                         = 1430,  // TabId = 383, Tab = 1, Row = 6, Column = 1
    PALADIN_ARDENT_DEFENDER                     = 1751,  // TabId = 383, Tab = 1, Row = 6, Column = 2
    PALADIN_REDOUBT                             = 1421,  // TabId = 383, Tab = 1, Row = 7, Column = 0
    PALADIN_COMBAT_EXPERTISE                    = 1753,  // TabId = 383, Tab = 1, Row = 7, Column = 2
    PALADIN_TOUCHER_BY_THE_LIGHT                = 2195,  // TabId = 383, Tab = 1, Row = 8, Column = 0
    PALADIN_AVENGERS_SHIELD                     = 1754,  // TabId = 383, Tab = 1, Row = 8, Column = 1
    PALADIN_GUARDED_BY_THE_LIGHT                = 2194,  // TabId = 383, Tab = 1, Row = 8, Column = 2
    PALADIN_SHIELD_OF_THE_TEMPLAR               = 2204,  // TabId = 383, Tab = 1, Row = 9, Column = 1
    PALADIN_JUDGEMENT_OF_THE_JUST               = 2200,  // TabId = 383, Tab = 1, Row = 9, Column = 2
    PALADIN_HAMMER_OF_THE_RIGHTEOUS             = 2196   // TabId = 383, Tab = 1, Row = 10, Column = 1
};

enum TalentsPriest  // 0x010
{
    PRIEST_UNBREAKABLE_WILL                     = 342,   // TabId = 201, Tab = 0, Row = 0, Column = 1
    PRIEST_TWIN_DISCIPLINES                     = 1898,  // TabId = 201, Tab = 0, Row = 0, Column = 2
    PRIEST_SILENT_RESOLVE                       = 352,   // TabId = 201, Tab = 0, Row = 1, Column = 0
    PRIEST_IMPROVED_INNER_FIRE                  = 346,   // TabId = 201, Tab = 0, Row = 1, Column = 1
    PRIEST_IMPROVED_POWER_WORD_FORTITUDE        = 344,   // TabId = 201, Tab = 0, Row = 1, Column = 2
    PRIEST_MARTYRDOM                            = 321,   // TabId = 201, Tab = 0, Row = 1, Column = 3
    PRIEST_MEDITATION                           = 347,   // TabId = 201, Tab = 0, Row = 2, Column = 0
    PRIEST_INNER_FOCUS                          = 348,   // TabId = 201, Tab = 0, Row = 2, Column = 1
    PRIEST_IMPROVED_POWER_WORD_SHIELD           = 343,   // TabId = 201, Tab = 0, Row = 2, Column = 2
    PRIEST_ABSOLUTION                           = 1769,  // TabId = 201, Tab = 0, Row = 3, Column = 0
    PRIEST_MENTAL_AGILITY                       = 341,   // TabId = 201, Tab = 0, Row = 3, Column = 1
    PRIEST_IMPROVED_MANA_BURN                   = 350,   // TabId = 201, Tab = 0, Row = 3, Column = 3
    PRIEST_REFLECTIVE_SHIELD                    = 2268,  // TabId = 201, Tab = 0, Row = 4, Column = 0
    PRIEST_MENTAL_STRENGTH                      = 1201,  // TabId = 201, Tab = 0, Row = 4, Column = 1
    PRIEST_SOUL_WARDING                         = 351,   // TabId = 201, Tab = 0, Row = 4, Column = 2
    PRIEST_FOCUSED_POWER                        = 1771,  // TabId = 201, Tab = 0, Row = 5, Column = 0
    PRIEST_ENLIGHTENMENT                        = 1772,  // TabId = 201, Tab = 0, Row = 5, Column = 2
    PRIEST_FOCUSED_WILL                         = 1858,  // TabId = 201, Tab = 0, Row = 6, Column = 0
    PRIEST_POWER_INFUSION                       = 322,   // TabId = 201, Tab = 0, Row = 6, Column = 1
    PRIEST_IMPROVED_FLASH_HEAL                  = 1773,  // TabId = 201, Tab = 0, Row = 6, Column = 2
    PRIEST_RENEWED_HOPE                         = 2235,  // TabId = 201, Tab = 0, Row = 7, Column = 0
    PRIEST_RAPTURE                              = 1896,  // TabId = 201, Tab = 0, Row = 7, Column = 1
    PRIEST_ASPIRATION                           = 1894,  // TabId = 201, Tab = 0, Row = 7, Column = 2
    PRIEST_DIVINE_AEGIS                         = 1895,  // TabId = 201, Tab = 0, Row = 8, Column = 0
    PRIEST_PAIN_SUPPRESSION                     = 1774,  // TabId = 201, Tab = 0, Row = 8, Column = 1
    PRIEST_GRACE                                = 1901,  // TabId = 201, Tab = 0, Row = 8, Column = 2
    PRIEST_BORROWED_TIME                        = 1202,  // TabId = 201, Tab = 0, Row = 9, Column = 1
    PRIEST_PENANCE                              = 1897,  // TabId = 201, Tab = 0, Row = 10, Column = 1
    PRIEST_HEALING_FOCUS                        = 410,   // TabId = 202, Tab = 1, Row = 0, Column = 0
    PRIEST_IMPROVED_RENEW                       = 406,   // TabId = 202, Tab = 1, Row = 0, Column = 1
    PRIEST_HOLY_SPECIALIZATION                  = 401,   // TabId = 202, Tab = 1, Row = 0, Column = 2
    PRIEST_SPELL_WARDING                        = 411,   // TabId = 202, Tab = 1, Row = 1, Column = 1
    PRIEST_DIVINE_FURY                          = 1181,  // TabId = 202, Tab = 1, Row = 1, Column = 2
    PRIEST_DESPERATE_PRAYER                     = 442,   // TabId = 202, Tab = 1, Row = 2, Column = 0
    PRIEST_BLESSED_RECOVERY                     = 1636,  // TabId = 202, Tab = 1, Row = 2, Column = 1
    PRIEST_INSPIRATION                          = 361,   // TabId = 202, Tab = 1, Row = 2, Column = 3
    PRIEST_HOLY_REACH                           = 1635,  // TabId = 202, Tab = 1, Row = 3, Column = 0
    PRIEST_IMPROVED_HEALIN                      = 408,   // TabId = 202, Tab = 1, Row = 3, Column = 1
    PRIEST_SEARING_LIGHT                        = 403,   // TabId = 202, Tab = 1, Row = 3, Column = 2
    PRIEST_HEALING_PRAYERS                      = 413,   // TabId = 202, Tab = 1, Row = 4, Column = 0
    PRIEST_SPIRIT_OF_REDEMPTION                 = 1561,  // TabId = 202, Tab = 1, Row = 4, Column = 1
    PRIEST_SPIRITUAL_GUIDANCE                   = 402,   // TabId = 202, Tab = 1, Row = 4, Column = 2
    PRIEST_SURGE_OF_LIGHT                       = 1766,  // TabId = 202, Tab = 1, Row = 5, Column = 0
    PRIEST_SPIRITUAL_HEALING                    = 404,   // TabId = 202, Tab = 1, Row = 5, Column = 2
    PRIEST_HOLY_CONCENTRATION                   = 1768,  // TabId = 202, Tab = 1, Row = 6, Column = 0
    PRIEST_LIGHTWELL                            = 1637,  // TabId = 202, Tab = 1, Row = 6, Column = 1
    PRIEST_BLESSED_RESILIENCE                   = 1765,  // TabId = 202, Tab = 1, Row = 6, Column = 2
    PRIEST_BODY_AND_SOUL                        = 2279,  // TabId = 202, Tab = 1, Row = 7, Column = 0
    PRIEST_EMPOWERED_HEALING                    = 1767,  // TabId = 202, Tab = 1, Row = 7, Column = 1
    PRIEST_SERENDIPITY                          = 1904,  // TabId = 202, Tab = 1, Row = 7, Column = 2
    PRIEST_EMPOWERED_RENEW                      = 1902,  // TabId = 202, Tab = 1, Row = 8, Column = 0
    PRIEST_CIRCLE_OF_HEALING                    = 1815,  // TabId = 202, Tab = 1, Row = 8, Column = 1
    PRIEST_TEST_OF_FAITH                        = 1903,  // TabId = 202, Tab = 1, Row = 8, Column = 2
    PRIEST_DIVINE_PROVIDENCE                    = 1905,  // TabId = 202, Tab = 1, Row = 9, Column = 1
    PRIEST_GUARDIAN_SPIRIT                      = 1911,  // TabId = 202, Tab = 1, Row = 10, Column = 1
    PRIEST_SPIRIT_TAP                           = 465,   // TabId = 203, Tab = 2, Row = 0, Column = 0
    PRIEST_IMPROVED_SPIRIT_TAP                  = 2027,  // TabId = 203, Tab = 2, Row = 0, Column = 1
    PRIEST_DARKNESS                             = 462,   // TabId = 203, Tab = 2, Row = 0, Column = 2
    PRIEST_SHADOW_AFFINITY                      = 466,   // TabId = 203, Tab = 2, Row = 1, Column = 0
    PRIEST_IMPROVED_SHADOW_WORD_PAIN            = 482,   // TabId = 203, Tab = 2, Row = 1, Column = 1
    PRIEST_SHADOW_FOCUS                         = 463,   // TabId = 203, Tab = 2, Row = 1, Column = 2
    PRIEST_IMPROVED_PSYCHIC_SCREAM              = 542,   // TabId = 203, Tab = 2, Row = 2, Column = 0
    PRIEST_IMPROVED_MIND_BLAST                  = 481,   // TabId = 203, Tab = 2, Row = 2, Column = 1
    PRIEST_MIND_FLAY                            = 501,   // TabId = 203, Tab = 2, Row = 2, Column = 2
    PRIEST_VEILED_SHADOWS                       = 483,   // TabId = 203, Tab = 2, Row = 3, Column = 1
    PRIEST_SHADOW_REACH                         = 881,   // TabId = 203, Tab = 2, Row = 3, Column = 2
    PRIEST_SHADOW_WEAVING                       = 461,   // TabId = 203, Tab = 2, Row = 3, Column = 3
    PRIEST_SILENCE                              = 541,   // TabId = 203, Tab = 2, Row = 4, Column = 0
    PRIEST_VAMPIRIC_EMBRACE                     = 484,   // TabId = 203, Tab = 2, Row = 4, Column = 1
    PRIEST_IMPROVED_VAMPIRIC_EMBRACE            = 1638,  // TabId = 203, Tab = 2, Row = 4, Column = 2
    PRIEST_FOCUSED_MIND                         = 1777,  // TabId = 203, Tab = 2, Row = 4, Column = 3
    PRIEST_MIND_MELT                            = 1781,  // TabId = 203, Tab = 2, Row = 5, Column = 0
    PRIEST_IMPROVED_DEVOURING_PLAGUE            = 2267,  // TabId = 203, Tab = 2, Row = 5, Column = 2
    PRIEST_SHADOWFORM                           = 521,   // TabId = 203, Tab = 2, Row = 6, Column = 1
    PRIEST_SHADOW_POWER                         = 1778,  // TabId = 203, Tab = 2, Row = 6, Column = 2
    PRIEST_IMPROVED_SHADOWFORM                  = 1906,  // TabId = 203, Tab = 2, Row = 7, Column = 0
    PRIEST_MISERY                               = 1816,  // TabId = 203, Tab = 2, Row = 7, Column = 2
    PRIEST_PSYCHIC_HORROR                       = 1908,  // TabId = 203, Tab = 2, Row = 8, Column = 0
    PRIEST_VAMPIRIC_TOUCH                       = 1779,  // TabId = 203, Tab = 2, Row = 8, Column = 1
    PRIEST_PAIN_AND_SUFFERING                   = 1909,  // TabId = 203, Tab = 2, Row = 8, Column = 2
    PRIEST_TWISTED_FAITH                        = 1907,  // TabId = 203, Tab = 2, Row = 9, Column = 2
    PRIEST_DISPERSION                           = 1910   // TabId = 203, Tab = 2, Row = 10, Column = 1
};

enum TalentsRogue  // 0x008
{
    ROGUE_IMPROVED_GOUGE                        = 203,   // TabId = 181, Tab = 1, Row = 0, Column = 0
    ROGUE_IMPROVED_SINISTER_STRIKE              = 201,   // TabId = 181, Tab = 1, Row = 0, Column = 1
    ROGUE_DUAL_WIELD_SPECIALIZATION             = 221,   // TabId = 181, Tab = 1, Row = 0, Column = 2
    ROGUE_IMPROVED_SLICE_AND_DICE               = 1827,  // TabId = 181, Tab = 1, Row = 1, Column = 0
    ROGUE_DEFLECTION                            = 187,   // TabId = 181, Tab = 1, Row = 1, Column = 1
    ROGUE_PRECISION                             = 181,   // TabId = 181, Tab = 1, Row = 1, Column = 3
    ROGUE_ENDURANCE                             = 204,   // TabId = 181, Tab = 1, Row = 2, Column = 0
    ROGUE_RIPOSTE                               = 301,   // TabId = 181, Tab = 1, Row = 2, Column = 1
    ROGUE_CLOSE_QUARTERS_COMBAT                 = 182,   // TabId = 181, Tab = 1, Row = 2, Column = 2
    ROGUE_IMPROVED_KICK                         = 206,   // TabId = 181, Tab = 1, Row = 3, Column = 0
    ROGUE_IMPROVED_SPRINT                       = 222,   // TabId = 181, Tab = 1, Row = 3, Column = 1
    ROGUE_LIGHTNING_REFLEXES                    = 186,   // TabId = 181, Tab = 1, Row = 3, Column = 2
    ROGUE_AGGRESSION                            = 1122,  // TabId = 181, Tab = 1, Row = 3, Column = 3
    ROGUE_MACE_SPECIALIZATION                   = 184,   // TabId = 181, Tab = 1, Row = 4, Column = 0
    ROGUE_BLADE_FLURRY                          = 223,   // TabId = 181, Tab = 1, Row = 4, Column = 1
    ROGUE_HACK_AND_SLASH                        = 242,   // TabId = 181, Tab = 1, Row = 4, Column = 2
    ROGUE_WEAPON_EXPERTISE                      = 1703,  // TabId = 181, Tab = 1, Row = 5, Column = 1
    ROGUE_BLADE_TWISTING                        = 1706,  // TabId = 181, Tab = 1, Row = 5, Column = 2
    ROGUE_VITALITY                              = 1705,  // TabId = 181, Tab = 1, Row = 6, Column = 0
    ROGUE_ADRENALINE_RUSH                       = 205,   // TabId = 181, Tab = 1, Row = 6, Column = 1
    ROGUE_NERVES_OF_STEEL                       = 1707,  // TabId = 181, Tab = 1, Row = 6, Column = 2
    ROGUE_THROWING_SPECIALIZATION               = 2072,  // TabId = 181, Tab = 1, Row = 7, Column = 0
    ROGUE_COMBAT_POTENCY                        = 1825,  // TabId = 181, Tab = 1, Row = 7, Column = 2
    ROGUE_UNFAIR_ADVANTAGE                      = 2073,  // TabId = 181, Tab = 1, Row = 8, Column = 0
    ROGUE_SURPRISE_ATTACKS                      = 1709,  // TabId = 181, Tab = 1, Row = 8, Column = 1
    ROGUE_SAVAGE_COMBAT                         = 2074,  // TabId = 181, Tab = 1, Row = 8, Column = 2
    ROGUE_PREY_ON_THE_WEAK                      = 2075,  // TabId = 181, Tab = 1, Row = 9, Column = 1
    ROGUE_KILLING_SPREE                         = 2076,  // TabId = 181, Tab = 1, Row = 10, Column = 1
    ROGUE_IMPROVED_EVISCERATE                   = 276,   // TabId = 182, Tab = 0, Row = 0, Column = 0
    ROGUE_REMORSELESS_ATTACKS                   = 272,   // TabId = 182, Tab = 0, Row = 0, Column = 1
    ROGUE_MALICE                                = 270,   // TabId = 182, Tab = 0, Row = 0, Column = 2
    ROGUE_RUTHLESSNESS                          = 273,   // TabId = 182, Tab = 0, Row = 1, Column = 0
    ROGUE_BLOOD_SPATTER                         = 2068,  // TabId = 182, Tab = 0, Row = 1, Column = 1
    ROGUE_PUNCTURING_WOUNDS                     = 277,   // TabId = 182, Tab = 0, Row = 1, Column = 3
    ROGUE_VIGOR                                 = 382,   // TabId = 182, Tab = 0, Row = 2, Column = 0
    ROGUE_IMPROVED_EXPOSE_ARMOR                 = 278,   // TabId = 182, Tab = 0, Row = 2, Column = 1
    ROGUE_LETHALITY                             = 269,   // TabId = 182, Tab = 0, Row = 2, Column = 2
    ROGUE_VILE_POISONS                          = 682,   // TabId = 182, Tab = 0, Row = 3, Column = 1
    ROGUE_IMPROVED_POISONS                      = 268,   // TabId = 182, Tab = 0, Row = 3, Column = 2
    ROGUE_FLEET_FOOTED                          = 1721,  // TabId = 182, Tab = 0, Row = 4, Column = 0
    ROGUE_COLD_BLOOD                            = 280,   // TabId = 182, Tab = 0, Row = 4, Column = 1
    ROGUE_IMPROVED_KIDNEY_SHOT                  = 279,   // TabId = 182, Tab = 0, Row = 4, Column = 2
    ROGUE_QUICK_RECOVERY                        = 1762,  // TabId = 182, Tab = 0, Row = 4, Column = 3
    ROGUE_SEAL_FATE                             = 283,   // TabId = 182, Tab = 0, Row = 5, Column = 1
    ROGUE_MURDER                                = 274,   // TabId = 182, Tab = 0, Row = 5, Column = 2
    ROGUE_DEADLY_BREW                           = 2065,  // TabId = 182, Tab = 0, Row = 6, Column = 0
    ROGUE_OVERKILL                              = 281,   // TabId = 182, Tab = 0, Row = 6, Column = 1
    ROGUE_DEADENED_NERVES                       = 1723,  // TabId = 182, Tab = 0, Row = 6, Column = 2
    ROGUE_FOCUSED_ATTACKS                       = 2069,  // TabId = 182, Tab = 0, Row = 7, Column = 0
    ROGUE_FIND_WEAKNESS                         = 1718,  // TabId = 182, Tab = 0, Row = 7, Column = 2
    ROGUE_MASTER_POISONER                       = 1715,  // TabId = 182, Tab = 0, Row = 8, Column = 0
    ROGUE_MUTILATE                              = 1719,  // TabId = 182, Tab = 0, Row = 8, Column = 1
    ROGUE_TURN_THE_TABLES                       = 2066,  // TabId = 182, Tab = 0, Row = 8, Column = 2
    ROGUE_CUT_TO_THE_CHASE                      = 2070,  // TabId = 182, Tab = 0, Row = 9, Column = 1
    ROGUE_HUNGER_FOR_BLOOD                      = 2071,  // TabId = 182, Tab = 0, Row = 10, Column = 1
    ROGUE_RELENTLESS_STRIKES                    = 2244,  // TabId = 183, Tab = 2, Row = 0, Column = 0
    ROGUE_MASTER_OF_DECEPTION                   = 241,   // TabId = 183, Tab = 2, Row = 0, Column = 1
    ROGUE_OPPORTUNITY                           = 261,   // TabId = 183, Tab = 2, Row = 0, Column = 2
    ROGUE_SLEIGHT_OF_HAND                       = 1700,  // TabId = 183, Tab = 2, Row = 1, Column = 0
    ROGUE_DIRTY_TRICKS                          = 262,   // TabId = 183, Tab = 2, Row = 1, Column = 1
    ROGUE_CAMOUFLAGE                            = 244,   // TabId = 183, Tab = 2, Row = 1, Column = 2
    ROGUE_ELUSIVENESS                           = 247,   // TabId = 183, Tab = 2, Row = 2, Column = 0
    ROGUE_GHOSTLY_STRIKE                        = 303,   // TabId = 183, Tab = 2, Row = 2, Column = 1
    ROGUE_SERRATED_BLADES                       = 1123,  // TabId = 183, Tab = 2, Row = 2, Column = 2
    ROGUE_SETUP                                 = 246,   // TabId = 183, Tab = 2, Row = 3, Column = 0
    ROGUE_INITIATIVE                            = 245,   // TabId = 183, Tab = 2, Row = 3, Column = 1
    ROGUE_IMPROVED_AMBUSH                       = 263,   // TabId = 183, Tab = 2, Row = 3, Column = 2
    ROGUE_HEIGHTENED_SENSES                     = 1701,  // TabId = 183, Tab = 2, Row = 4, Column = 0
    ROGUE_PREPARATION                           = 284,   // TabId = 183, Tab = 2, Row = 4, Column = 1
    ROGUE_DIRTY_DEEDS                           = 265,   // TabId = 183, Tab = 2, Row = 4, Column = 2
    ROGUE_HEMORRHAGE                            = 681,   // TabId = 183, Tab = 2, Row = 4, Column = 3
    ROGUE_MASTER_OF_SUBTLETY                    = 1713,  // TabId = 183, Tab = 2, Row = 5, Column = 0
    ROGUE_DEADLINESS                            = 1702,  // TabId = 183, Tab = 2, Row = 5, Column = 2
    ROGUE_ENVELOPING_SHADOWS                    = 1711,  // TabId = 183, Tab = 2, Row = 6, Column = 0
    ROGUE_PREMEDITATION                         = 381,   // TabId = 183, Tab = 2, Row = 6, Column = 1
    ROGUE_CHEAT_DEATH                           = 1722,  // TabId = 183, Tab = 2, Row = 6, Column = 2
    ROGUE_SINISTER_CALLING                      = 1712,  // TabId = 183, Tab = 2, Row = 7, Column = 1
    ROGUE_WAYLAY                                = 2077,  // TabId = 183, Tab = 2, Row = 7, Column = 2
    ROGUE_HONOR_AMONG_THIEVES                   = 2078,  // TabId = 183, Tab = 2, Row = 8, Column = 0
    ROGUE_SHADOWSTEP                            = 1714,  // TabId = 183, Tab = 2, Row = 8, Column = 1
    ROGUE_FILTHY_TRICKS                         = 2079,  // TabId = 183, Tab = 2, Row = 8, Column = 2
    ROGUE_SLAUGHTER_FROM_THE_SHADOWS            = 2080,  // TabId = 183, Tab = 2, Row = 9, Column = 1
    ROGUE_SHADOW_DANCE                          = 2081   // TabId = 183, Tab = 2, Row = 10, Column = 1
};

enum TalentsShaman  // 0x040
{
    SHAMAN_CONVECTION                           = 564,   // TabId = 261, Tab = 0, Row = 0, Column = 1
    SHAMAN_CONCUSSION                           = 563,   // TabId = 261, Tab = 0, Row = 0, Column = 2
    SHAMAN_CALL_OF_FLAME                        = 561,   // TabId = 261, Tab = 0, Row = 1, Column = 0
    SHAMAN_ELEMENTAL_WARDING                    = 1640,  // TabId = 261, Tab = 0, Row = 1, Column = 1
    SHAMAN_ELEMENTAL_DEVASTATION                = 1645,  // TabId = 261, Tab = 0, Row = 1, Column = 2
    SHAMAN_REVERBERATION                        = 575,   // TabId = 261, Tab = 0, Row = 2, Column = 0
    SHAMAN_ELEMENTAL_FOCUS                      = 574,   // TabId = 261, Tab = 0, Row = 2, Column = 1
    SHAMAN_ELEMENTAL_FURY                       = 565,   // TabId = 261, Tab = 0, Row = 2, Column = 2
    SHAMAN_IMPROVED_FIRE_NOVA                   = 567,   // TabId = 261, Tab = 0, Row = 3, Column = 0
    SHAMAN_EYE_OF_THE_STORM                     = 1642,  // TabId = 261, Tab = 0, Row = 3, Column = 3
    SHAMAN_ELEMENTAL_REACH                      = 1641,  // TabId = 261, Tab = 0, Row = 4, Column = 0
    SHAMAN_CALL_OF_THUNDER                      = 562,   // TabId = 261, Tab = 0, Row = 4, Column = 1
    SHAMAN_UNRELENTING_STORM                    = 1682,  // TabId = 261, Tab = 0, Row = 4, Column = 3
    SHAMAN_ELEMENTAL_PRECISION                  = 1685,  // TabId = 261, Tab = 0, Row = 5, Column = 0
    SHAMAN_LIGHTNING_MASTERY                    = 721,   // TabId = 261, Tab = 0, Row = 5, Column = 2
    SHAMAN_ELEMENTAL_MASTERY                    = 573,   // TabId = 261, Tab = 0, Row = 6, Column = 1
    SHAMAN_STORM_EARTH_AND_FIRE                 = 2052,  // TabId = 261, Tab = 0, Row = 6, Column = 2
    SHAMAN_BOOMING_ECHOES                       = 2262,  // TabId = 261, Tab = 0, Row = 7, Column = 0
    SHAMAN_ELEMENTAL_OATH                       = 2049,  // TabId = 261, Tab = 0, Row = 7, Column = 1
    SHAMAN_LIGHTNING_OVERLOAD                   = 1686,  // TabId = 261, Tab = 0, Row = 7, Column = 2
    SHAMAN_ASTRAL_SHIFT                         = 2050,  // TabId = 261, Tab = 0, Row = 8, Column = 0
    SHAMAN_TOTEM_OF_WRATH                       = 1687,  // TabId = 261, Tab = 0, Row = 8, Column = 1
    SHAMAN_LAVA_FLOWS                           = 2051,  // TabId = 261, Tab = 0, Row = 8, Column = 2
    SHAMAN_SHAMANISM                            = 2252,  // TabId = 261, Tab = 0, Row = 9, Column = 1
    SHAMAN_THUNDERSTORM                         = 2053,  // TabId = 261, Tab = 0, Row = 10, Column = 1
    SHAMAN_IMPROVED_HEALING_WAVE                = 586,   // TabId = 262, Tab = 2, Row = 0, Column = 1
    SHAMAN_TOTEMIC_FOCUS                        = 595,   // TabId = 262, Tab = 2, Row = 0, Column = 2
    SHAMAN_IMPROVED_REINCARNATION               = 589,   // TabId = 262, Tab = 2, Row = 1, Column = 0
    SHAMAN_HEALING_GRACE                        = 1646,  // TabId = 262, Tab = 2, Row = 1, Column = 1
    SHAMAN_TIDAL_FOCUS                          = 593,   // TabId = 262, Tab = 2, Row = 1, Column = 2
    SHAMAN_IMPROVED_WATER_SHIELD                = 583,   // TabId = 262, Tab = 2, Row = 2, Column = 0
    SHAMAN_HEALING_FOCUS                        = 587,   // TabId = 262, Tab = 2, Row = 2, Column = 1
    SHAMAN_TIDAL_FORCE                          = 582,   // TabId = 262, Tab = 2, Row = 2, Column = 2
    SHAMAN_ANCESTRAL_HEALING                    = 581,   // TabId = 262, Tab = 2, Row = 2, Column = 3
    SHAMAN_RESTORATIVE_TOTEMS                   = 588,   // TabId = 262, Tab = 2, Row = 3, Column = 1
    SHAMAN_TIDAL_MASTERY                        = 594,   // TabId = 262, Tab = 2, Row = 3, Column = 2
    SHAMAN_HEALING_WAY                          = 1648,  // TabId = 262, Tab = 2, Row = 4, Column = 0
    SHAMAN_NATURES_SWIFTNESS                    = 591,   // TabId = 262, Tab = 2, Row = 4, Column = 2
    SHAMAN_FOCUSED_MIND                         = 1695,  // TabId = 262, Tab = 2, Row = 4, Column = 3
    SHAMAN_PURIFICATION                         = 592,   // TabId = 262, Tab = 2, Row = 5, Column = 2
    SHAMAN_NATURES_GUARDIAN                     = 1699,  // TabId = 262, Tab = 2, Row = 6, Column = 0
    SHAMAN_MANA_TIDE_TOTEM                      = 590,   // TabId = 262, Tab = 2, Row = 6, Column = 1
    SHAMAN_CLEANSE_SPIRIT                       = 2084,  // TabId = 262, Tab = 2, Row = 6, Column = 2
    SHAMAN_BLESSING_OF_THE_ETERNALS             = 2060,  // TabId = 262, Tab = 2, Row = 7, Column = 0
    SHAMAN_IMPROVED_CHAIN_HEAL                  = 1697,  // TabId = 262, Tab = 2, Row = 7, Column = 1
    SHAMAN_NATURES_BLESSING                     = 1696,  // TabId = 262, Tab = 2, Row = 7, Column = 2
    SHAMAN_ANCESTRAL_AWAKENING                  = 2061,  // TabId = 262, Tab = 2, Row = 8, Column = 0
    SHAMAN_EARTH_SHIELD                         = 1698,  // TabId = 262, Tab = 2, Row = 8, Column = 1
    SHAMAN_IMPROVED_EARTH_SHIELD                = 2059,  // TabId = 262, Tab = 2, Row = 8, Column = 2
    SHAMAN_TIDAL_WAVES                          = 2063,  // TabId = 262, Tab = 2, Row = 9, Column = 1
    SHAMAN_RIPTIDE                              = 2064,  // TabId = 262, Tab = 2, Row = 10, Column = 1
    SHAMAN_ENHANCING_TOTEMS                     = 610,   // TabId = 263, Tab = 1, Row = 0, Column = 0
    SHAMAN_EARTHS_GRASP                         = 2101,  // TabId = 263, Tab = 1, Row = 0, Column = 1
    SHAMAN_ANCESTRAL_KNOWLEDGE                  = 614,   // TabId = 263, Tab = 1, Row = 0, Column = 2
    SHAMAN_GUARDIAN_TOTEMS                      = 609,   // TabId = 263, Tab = 1, Row = 1, Column = 0
    SHAMAN_THUNDERING_STRIKES                   = 613,   // TabId = 263, Tab = 1, Row = 1, Column = 1
    SHAMAN_IMPROVED_GHOST_WOLF                  = 605,   // TabId = 263, Tab = 1, Row = 1, Column = 2
    SHAMAN_IMPROVED_SHIELDS                     = 607,   // TabId = 263, Tab = 1, Row = 1, Column = 3
    SHAMAN_ELEMENTAL_WEAPONS                    = 611,   // TabId = 263, Tab = 1, Row = 2, Column = 0
    SHAMAN_SHAMANISTIC_FOCUS                    = 617,   // TabId = 263, Tab = 1, Row = 2, Column = 2
    SHAMAN_ANTICIPATION                         = 601,   // TabId = 263, Tab = 1, Row = 2, Column = 3
    SHAMAN_FLURRY                               = 602,   // TabId = 263, Tab = 1, Row = 3, Column = 1
    SHAMAN_TOUGHNESS                            = 615,   // TabId = 263, Tab = 1, Row = 3, Column = 2
    SHAMAN_IMPROVED_WINDFURY_TOTEM              = 1647,  // TabId = 263, Tab = 1, Row = 4, Column = 0
    SHAMAN_SPIRIT_WEAPONS                       = 616,   // TabId = 263, Tab = 1, Row = 4, Column = 1
    SHAMAN_MENTAL_DEXTERITY                     = 2083,  // TabId = 263, Tab = 1, Row = 4, Column = 2
    SHAMAN_UNLEASHED_RAGE                       = 1689,  // TabId = 263, Tab = 1, Row = 5, Column = 0
    SHAMAN_WEAPON_MASTERY                       = 1643,  // TabId = 263, Tab = 1, Row = 5, Column = 2
    SHAMAN_FROZEN_POWER                         = 2263,  // TabId = 263, Tab = 1, Row = 5, Column = 3
    SHAMAN_DUAL_WIELD_SPECIALIZATION            = 1692,  // TabId = 263, Tab = 1, Row = 6, Column = 0
    SHAMAN_DUAL_WIELD                           = 1690,  // TabId = 263, Tab = 1, Row = 6, Column = 1
    SHAMAN_STORMSTRIKE                          = 901,   // TabId = 263, Tab = 1, Row = 6, Column = 2
    SHAMAN_STATIC_SHOCK                         = 2055,  // TabId = 263, Tab = 1, Row = 7, Column = 0
    SHAMAN_LAVA_LASH                            = 2249,  // TabId = 263, Tab = 1, Row = 7, Column = 1
    SHAMAN_IMPROVED_STORMSTRIKE                 = 2054,  // TabId = 263, Tab = 1, Row = 7, Column = 2
    SHAMAN_MENTAL_QUICKNESS                     = 1691,  // TabId = 263, Tab = 1, Row = 8, Column = 0
    SHAMAN_SHAMANISTIC_RAGE                     = 1693,  // TabId = 263, Tab = 1, Row = 8, Column = 1
    SHAMAN_EARTHEN_POWER                        = 2056,  // TabId = 263, Tab = 1, Row = 8, Column = 2
    SHAMAN_MAELSTROM_WEAPON                     = 2057,  // TabId = 263, Tab = 1, Row = 9, Column = 1
    SHAMAN_FERAL_SPIRIT                         = 2058   // TabId = 263, Tab = 1, Row = 10, Column = 1
};

enum TalentsWarlock  // 0x100
{
    WARLOCK_IMPROVED_SHADOW_BOLT                = 944,   // TabId = 301, Tab = 2, Row = 0, Column = 1
    WARLOCK_BANE                                = 943,   // TabId = 301, Tab = 2, Row = 0, Column = 2
    WARLOCK_AFTERMATH                           = 982,   // TabId = 301, Tab = 2, Row = 1, Column = 0
    WARLOCK_MOLTEN_SKIN                         = 1887,  // TabId = 301, Tab = 2, Row = 1, Column = 1
    WARLOCK_CATACLYSM                           = 941,   // TabId = 301, Tab = 2, Row = 1, Column = 2
    WARLOCK_DEMONIC_POWER                       = 983,   // TabId = 301, Tab = 2, Row = 2, Column = 0
    WARLOCK_SHADOWBURN                          = 963,   // TabId = 301, Tab = 2, Row = 2, Column = 1
    WARLOCK_RUIN                                = 967,   // TabId = 301, Tab = 2, Row = 2, Column = 2
    WARLOCK_INTENSITY                           = 985,   // TabId = 301, Tab = 2, Row = 3, Column = 0
    WARLOCK_DESTRUCTIVE_REACH                   = 964,   // TabId = 301, Tab = 2, Row = 3, Column = 1
    WARLOCK_IMPROVED_SEARING_PAIN               = 965,   // TabId = 301, Tab = 2, Row = 3, Column = 3
    WARLOCK_BACKLASH                            = 1817,  // TabId = 301, Tab = 2, Row = 4, Column = 0
    WARLOCK_IMPROVED_IMMOLATE                   = 961,   // TabId = 301, Tab = 2, Row = 4, Column = 1
    WARLOCK_DEVASTATION                         = 981,   // TabId = 301, Tab = 2, Row = 4, Column = 2
    WARLOCK_NETHER_PROTECTION                   = 1679,  // TabId = 301, Tab = 2, Row = 5, Column = 0
    WARLOCK_EMBERSTORM                          = 966,   // TabId = 301, Tab = 2, Row = 5, Column = 2
    WARLOCK_CONFLAGRATE                         = 968,   // TabId = 301, Tab = 2, Row = 6, Column = 1
    WARLOCK_SOUL_LEECH                          = 1678,  // TabId = 301, Tab = 2, Row = 6, Column = 2
    WARLOCK_PYROCLASM                           = 986,   // TabId = 301, Tab = 2, Row = 6, Column = 3
    WARLOCK_SHADOW_AND_FLAME                    = 1677,  // TabId = 301, Tab = 2, Row = 7, Column = 1
    WARLOCK_IMPROVED_SOUL_LEECH                 = 1889,  // TabId = 301, Tab = 2, Row = 7, Column = 2
    WARLOCK_BACKDRAFT                           = 1888,  // TabId = 301, Tab = 2, Row = 8, Column = 0
    WARLOCK_SHADOWFURY                          = 1676,  // TabId = 301, Tab = 2, Row = 8, Column = 1
    WARLOCK_EMPOWERED_IMP                       = 2045,  // TabId = 301, Tab = 2, Row = 8, Column = 2
    WARLOCK_FIRE_AND_BRIMSTONE                  = 1890,  // TabId = 301, Tab = 2, Row = 9, Column = 1
    WARLOCK_CHAOS_BOLT                          = 1891,  // TabId = 301, Tab = 2, Row = 10, Column = 1
    WARLOCK_IMPROVED_CURSE_OF_AGONY             = 1284,  // TabId = 302, Tab = 0, Row = 0, Column = 0
    WARLOCK_SUPPRESSION                         = 1005,  // TabId = 302, Tab = 0, Row = 0, Column = 1
    WARLOCK_IMPROVED_CORRUPTION                 = 1003,  // TabId = 302, Tab = 0, Row = 0, Column = 2
    WARLOCK_IMPROVED_CURSE_OF_WEAKNESS          = 1006,  // TabId = 302, Tab = 0, Row = 1, Column = 0
    WARLOCK_IMPROVED_DRAIN_SOUL                 = 1101,  // TabId = 302, Tab = 0, Row = 1, Column = 1
    WARLOCK_IMPROVED_LIFE_TAP                   = 1007,  // TabId = 302, Tab = 0, Row = 1, Column = 2
    WARLOCK_SOUL_SIPHON                         = 1004,  // TabId = 302, Tab = 0, Row = 1, Column = 3
    WARLOCK_IMPROVED_FEAR                       = 2205,  // TabId = 302, Tab = 0, Row = 2, Column = 0
    WARLOCK_FEL_CONCENTRATION                   = 1001,  // TabId = 302, Tab = 0, Row = 2, Column = 1
    WARLOCK_AMPLIFY_CURSE                       = 1061,  // TabId = 302, Tab = 0, Row = 2, Column = 2
    WARLOCK_GRIM_REACH                          = 1021,  // TabId = 302, Tab = 0, Row = 3, Column = 0
    WARLOCK_NIGHTFALL                           = 1002,  // TabId = 302, Tab = 0, Row = 3, Column = 1
    WARLOCK_EMPOWERED_CORRUPTION                = 1764,  // TabId = 302, Tab = 0, Row = 3, Column = 3
    WARLOCK_SHADOW_EMBRACE                      = 1763,  // TabId = 302, Tab = 0, Row = 4, Column = 0
    WARLOCK_SIPHON_LIFE                         = 1041,  // TabId = 302, Tab = 0, Row = 4, Column = 1
    WARLOCK_CURSE_OF_EXHAUSTION                 = 1081,  // TabId = 302, Tab = 0, Row = 4, Column = 2
    WARLOCK_IMPROVED_FELHUNTER                  = 1873,  // TabId = 302, Tab = 0, Row = 5, Column = 0
    WARLOCK_SHADOW_MASTERY                      = 1042,  // TabId = 302, Tab = 0, Row = 5, Column = 1
    WARLOCK_ERADICATION                         = 1878,  // TabId = 302, Tab = 0, Row = 6, Column = 0
    WARLOCK_CONTAGION                           = 1669,  // TabId = 302, Tab = 0, Row = 6, Column = 1
    WARLOCK_DARK_PACT                           = 1022,  // TabId = 302, Tab = 0, Row = 6, Column = 2
    WARLOCK_IMPROVED_HOWL_OF_TERROR             = 1668,  // TabId = 302, Tab = 0, Row = 7, Column = 0
    WARLOCK_MALEDICTION                         = 1667,  // TabId = 302, Tab = 0, Row = 7, Column = 2
    WARLOCK_DEATHS_EMBRACE                      = 1875,  // TabId = 302, Tab = 0, Row = 8, Column = 0
    WARLOCK_UNSTABLE_AFFLICTION                 = 1670,  // TabId = 302, Tab = 0, Row = 8, Column = 1
    WARLOCK_PANDEMIC                            = 2245,  // TabId = 302, Tab = 0, Row = 8, Column = 2
    WARLOCK_EVERLASTING_AFFLICTION              = 1876,  // TabId = 302, Tab = 0, Row = 9, Column = 1
    WARLOCK_HAUNT                               = 2041,  // TabId = 302, Tab = 0, Row = 10, Column = 1
    WARLOCK_IMPROVED_HEALTHSTONE                = 1221,  // TabId = 303, Tab = 1, Row = 0, Column = 0
    WARLOCK_IMPROVED_IMP                        = 1222,  // TabId = 303, Tab = 1, Row = 0, Column = 1
    WARLOCK_DEMONIC_EMBRACE                     = 1223,  // TabId = 303, Tab = 1, Row = 0, Column = 2
    WARLOCK_FEL_SYNERGY                         = 1883,  // TabId = 303, Tab = 1, Row = 0, Column = 3
    WARLOCK_IMPROVED_HEALTH_FUNNEL              = 1224,  // TabId = 303, Tab = 1, Row = 1, Column = 0
    WARLOCK_DEMONIC_BRUTALITY                   = 1225,  // TabId = 303, Tab = 1, Row = 1, Column = 1
    WARLOCK_FEL_VITALITY                        = 1242,  // TabId = 303, Tab = 1, Row = 1, Column = 2
    WARLOCK_IMPROVED_SUCCUBUS                   = 1243,  // TabId = 303, Tab = 1, Row = 2, Column = 0
    WARLOCK_SOUL_LINK                           = 1282,  // TabId = 303, Tab = 1, Row = 2, Column = 1
    WARLOCK_FEL_DOMINATION                      = 1226,  // TabId = 303, Tab = 1, Row = 2, Column = 2
    WARLOCK_DEMONIC_AEGIS                       = 1671,  // TabId = 303, Tab = 1, Row = 2, Column = 3
    WARLOCK_UNHOLY_POWER                        = 1262,  // TabId = 303, Tab = 1, Row = 3, Column = 1
    WARLOCK_MASTER_SUMMONER                     = 1227,  // TabId = 303, Tab = 1, Row = 3, Column = 2
    WARLOCK_MANA_FEED                           = 1281,  // TabId = 303, Tab = 1, Row = 4, Column = 0
    WARLOCK_MASTER_CONJURER                     = 1261,  // TabId = 303, Tab = 1, Row = 4, Column = 2
    WARLOCK_MASTER_DEMONOLOGIST                 = 1244,  // TabId = 303, Tab = 1, Row = 5, Column = 1
    WARLOCK_MOLTEN_CORE                         = 1283,  // TabId = 303, Tab = 1, Row = 5, Column = 2
    WARLOCK_DEMONIC_RESILIENCE                  = 1680,  // TabId = 303, Tab = 1, Row = 6, Column = 0
    WARLOCK_DEMONIC_EMPOWERMENT                 = 1880,  // TabId = 303, Tab = 1, Row = 6, Column = 1
    WARLOCK_DEMONIC_KNOWLEDGE                   = 1263,  // TabId = 303, Tab = 1, Row = 6, Column = 2
    WARLOCK_DEMONIC_TACTICS                     = 1673,  // TabId = 303, Tab = 1, Row = 7, Column = 1
    WARLOCK_DECIMATION                          = 2261,  // TabId = 303, Tab = 1, Row = 7, Column = 2
    WARLOCK_IMPROVED_DEMONIC_TACTICS            = 1882,  // TabId = 303, Tab = 1, Row = 8, Column = 0
    WARLOCK_SUMMON_FELGUARD                     = 1672,  // TabId = 303, Tab = 1, Row = 8, Column = 1
    WARLOCK_NEMESIS                             = 1884,  // TabId = 303, Tab = 1, Row = 8, Column = 2
    WARLOCK_DEMONIC_PACT                        = 1885,  // TabId = 303, Tab = 1, Row = 9, Column = 1
    WARLOCK_METAMORPHOSIS                       = 1886   // TabId = 303, Tab = 1, Row = 10, Column = 1
};

enum TalentsWarrior  // 0x001
{
    WARRIOR_IMPROVED_HEROIC_STRIKE              = 124,   // TabId = 161, Tab = 0, Row = 0, Column = 0
    WARRIOR_DEFLECTION                          = 130,   // TabId = 161, Tab = 0, Row = 0, Column = 1
    WARRIOR_IMPROVED_REND                       = 127,   // TabId = 161, Tab = 0, Row = 0, Column = 2
    WARRIOR_IMPROVED_CHARGE                     = 126,   // TabId = 161, Tab = 0, Row = 1, Column = 0
    WARRIOR_IRON_WILL                           = 641,   // TabId = 161, Tab = 0, Row = 1, Column = 1
    WARRIOR_TACTICAL_MASTERY                    = 128,   // TabId = 161, Tab = 0, Row = 1, Column = 2
    WARRIOR_IMPROVED_OVERPOWER                  = 131,   // TabId = 161, Tab = 0, Row = 2, Column = 0
    WARRIOR_ANGER_MANAGEMENT                    = 137,   // TabId = 161, Tab = 0, Row = 2, Column = 1
    WARRIOR_IMPALE                              = 662,   // TabId = 161, Tab = 0, Row = 2, Column = 2
    WARRIOR_DEEP_WOUNDS                         = 121,   // TabId = 161, Tab = 0, Row = 2, Column = 3
    WARRIOR_TWOHANDED_WEAPON_SPECIALIZATION     = 136,   // TabId = 161, Tab = 0, Row = 3, Column = 1
    WARRIOR_TASTE_FOR_BLOOD                     = 2232,  // TabId = 161, Tab = 0, Row = 3, Column = 2
    WARRIOR_POLEAXE_SPECIALIZATION              = 132,   // TabId = 161, Tab = 0, Row = 4, Column = 0
    WARRIOR_SWEEPING_STRIKES                    = 133,   // TabId = 161, Tab = 0, Row = 4, Column = 1
    WARRIOR_MACE_SPECIALIZATION                 = 125,   // TabId = 161, Tab = 0, Row = 4, Column = 2
    WARRIOR_SWORD_SPECIALIZATION                = 123,   // TabId = 161, Tab = 0, Row = 4, Column = 3
    WARRIOR_WEAPON_MASTERY                      = 134,   // TabId = 161, Tab = 0, Row = 5, Column = 0
    WARRIOR_IMPROVED_HAMSTRING                  = 129,   // TabId = 161, Tab = 0, Row = 5, Column = 2
    WARRIOR_TRAUMA                              = 1859,  // TabId = 161, Tab = 0, Row = 5, Column = 3
    WARRIOR_SECOND_WIND                         = 1663,  // TabId = 161, Tab = 0, Row = 6, Column = 0
    WARRIOR_MORTAL_STRIKE                       = 135,   // TabId = 161, Tab = 0, Row = 6, Column = 1
    WARRIOR_STRENGTH_OF_ARMS                    = 1862,  // TabId = 161, Tab = 0, Row = 6, Column = 2
    WARRIOR_IMPROVED_SLAM                       = 2233,  // TabId = 161, Tab = 0, Row = 6, Column = 3
    WARRIOR_JUGGERNAUT                          = 2283,  // TabId = 161, Tab = 0, Row = 7, Column = 0
    WARRIOR_IMPROVED_MORTAL_STRIKE              = 1824,  // TabId = 161, Tab = 0, Row = 7, Column = 1
    WARRIOR_UNRELENTING_ASSAULT                 = 1860,  // TabId = 161, Tab = 0, Row = 7, Column = 2
    WARRIOR_SUDDEN_DEATH                        = 1662,  // TabId = 161, Tab = 0, Row = 8, Column = 0
    WARRIOR_ENDLESS_RAGE                        = 1661,  // TabId = 161, Tab = 0, Row = 8, Column = 1
    WARRIOR_BLOOD_FRENZY                        = 1664,  // TabId = 161, Tab = 0, Row = 8, Column = 2
    WARRIOR_WRECKING_CREW                       = 2231,  // TabId = 161, Tab = 0, Row = 9, Column = 1
    WARRIOR_BLADESTORM                          = 1863,  // TabId = 161, Tab = 0, Row = 10, Column = 1
    WARRIOR_IMPROVED_BLOODRAGE                  = 142,   // TabId = 163, Tab = 2, Row = 0, Column = 0
    WARRIOR_SHIELD_SPECIALIZATION               = 1601,  // TabId = 163, Tab = 2, Row = 0, Column = 1
    WARRIOR_IMPROVED_THUNDER_CLAP               = 141,   // TabId = 163, Tab = 2, Row = 0, Column = 2
    WARRIOR_INCITE                              = 144,   // TabId = 163, Tab = 2, Row = 1, Column = 1
    WARRIOR_ANTICIPATION                        = 138,   // TabId = 163, Tab = 2, Row = 1, Column = 2
    WARRIOR_LAST_STAND                          = 153,   // TabId = 163, Tab = 2, Row = 2, Column = 0
    WARRIOR_IMPROVED_REVENGE                    = 147,   // TabId = 163, Tab = 2, Row = 2, Column = 1
    WARRIOR_SHIELD_MASTERY                      = 1654,  // TabId = 163, Tab = 2, Row = 2, Column = 2
    WARRIOR_TOUGHNESS                           = 140,   // TabId = 163, Tab = 2, Row = 2, Column = 3
    WARRIOR_IMPROVED_SPELL_REFLECTION           = 2247,  // TabId = 163, Tab = 2, Row = 3, Column = 0
    WARRIOR_IMPROVED_DISARM                     = 151,   // TabId = 163, Tab = 2, Row = 3, Column = 1
    WARRIOR_PUNCTURE                            = 146,   // TabId = 163, Tab = 2, Row = 3, Column = 2
    WARRIOR_IMPROVED_DISCIPLINES                = 150,   // TabId = 163, Tab = 2, Row = 4, Column = 0
    WARRIOR_CONCUSSION_BLOW                     = 152,   // TabId = 163, Tab = 2, Row = 4, Column = 1
    WARRIOR_GAG_ORDER                           = 149,   // TabId = 163, Tab = 2, Row = 4, Column = 2
    WARRIOR_ONEHANDED_WEAPON_SPECIALIZATION     = 702,   // TabId = 163, Tab = 2, Row = 5, Column = 2
    WARRIOR_IMPROVED_DEFENSIVE_STANCE           = 1652,  // TabId = 163, Tab = 2, Row = 6, Column = 0
    WARRIOR_VIGILANCE                           = 148,   // TabId = 163, Tab = 2, Row = 6, Column = 1
    WARRIOR_FOCUSED_RAGE                        = 1660,  // TabId = 163, Tab = 2, Row = 6, Column = 2
    WARRIOR_VITALITY                            = 1653,  // TabId = 163, Tab = 2, Row = 7, Column = 1
    WARRIOR_SAFEGUARD                           = 1870,  // TabId = 163, Tab = 2, Row = 7, Column = 2
    WARRIOR_WARBRINGER                          = 2236,  // TabId = 163, Tab = 2, Row = 8, Column = 0
    WARRIOR_DEVASTATE                           = 1666,  // TabId = 163, Tab = 2, Row = 8, Column = 1
    WARRIOR_CRITICAL_BLOCK                      = 1893,  // TabId = 163, Tab = 2, Row = 8, Column = 2
    WARRIOR_SWORD_AND_BOARD                     = 1871,  // TabId = 163, Tab = 2, Row = 9, Column = 1
    WARRIOR_DAMAGE_SHIELD                       = 2246,  // TabId = 163, Tab = 2, Row = 9, Column = 2
    WARRIOR_SHOCKWAVE                           = 1872,  // TabId = 163, Tab = 2, Row = 10, Column = 1
    WARRIOR_ARMORED_TO_THE_TEETH                = 2250,  // TabId = 164, Tab = 1, Row = 0, Column = 0
    WARRIOR_BOOMING_VOICE                       = 158,   // TabId = 164, Tab = 1, Row = 0, Column = 1
    WARRIOR_CRUELTY                             = 157,   // TabId = 164, Tab = 1, Row = 0, Column = 2
    WARRIOR_IMPROVED_DEMORALIZING_SHOUT         = 161,   // TabId = 164, Tab = 1, Row = 1, Column = 1
    WARRIOR_UNBRIDLED_WRATH                     = 159,   // TabId = 164, Tab = 1, Row = 1, Column = 2
    WARRIOR_IMPROVED_CLEAVE                     = 166,   // TabId = 164, Tab = 1, Row = 2, Column = 0
    WARRIOR_PIERCING_HOWL                       = 160,   // TabId = 164, Tab = 1, Row = 2, Column = 1
    WARRIOR_BLOOD_CRAZE                         = 661,   // TabId = 164, Tab = 1, Row = 2, Column = 2
    WARRIOR_COMMANDING_PRESENCE                 = 154,   // TabId = 164, Tab = 1, Row = 2, Column = 3
    WARRIOR_DUAL_WIELD_SPECIALIZATION           = 1581,  // TabId = 164, Tab = 1, Row = 3, Column = 0
    WARRIOR_IMPROVED_EXECUTE                    = 1542,  // TabId = 164, Tab = 1, Row = 3, Column = 1
    WARRIOR_ENRAGE                              = 155,   // TabId = 164, Tab = 1, Row = 3, Column = 2
    WARRIOR_PRECISION                           = 1657,  // TabId = 164, Tab = 1, Row = 4, Column = 0
    WARRIOR_DEATH_WISH                          = 165,   // TabId = 164, Tab = 1, Row = 4, Column = 1
    WARRIOR_IMPROVED_INTERCEPT                  = 1543,  // TabId = 164, Tab = 1, Row = 4, Column = 2
    WARRIOR_IMPROVED_BERSERKER_RAGE             = 1541,  // TabId = 164, Tab = 1, Row = 5, Column = 0
    WARRIOR_FLURRY                              = 156,   // TabId = 164, Tab = 1, Row = 5, Column = 2
    WARRIOR_INTENSIFY_RAGE                      = 1864,  // TabId = 164, Tab = 1, Row = 6, Column = 0
    WARRIOR_BLOODTHIRST                         = 167,   // TabId = 164, Tab = 1, Row = 6, Column = 1
    WARRIOR_IMPROVED_WHIRLWIND                  = 1655,  // TabId = 164, Tab = 1, Row = 6, Column = 3
    WARRIOR_FURIOUS_ATTACKS                     = 1865,  // TabId = 164, Tab = 1, Row = 7, Column = 0
    WARRIOR_IMPROVED_BERSERKER_STANCE           = 1658,  // TabId = 164, Tab = 1, Row = 7, Column = 3
    WARRIOR_HEROIC_FURY                         = 1868,  // TabId = 164, Tab = 1, Row = 8, Column = 0
    WARRIOR_RAMPAGE                             = 1659,  // TabId = 164, Tab = 1, Row = 8, Column = 1
    WARRIOR_BLOODSURGE                          = 1866,  // TabId = 164, Tab = 1, Row = 8, Column = 2
    WARRIOR_UNENDING_FURY                       = 2234,  // TabId = 164, Tab = 1, Row = 9, Column = 1
    WARRIOR_TITANS_GRIP                         = 1867   // TabId = 164, Tab = 1, Row = 10, Column = 1
};

enum TalentsPet  // 0x000
{
    PET_TENACITY_COBRA_REFLEXES                 = 2114,  // TabId = 409, Tab = 0, Row = 0, Column = 0
    PET_TENACITY_CHARGE                         = 2237,  // TabId = 409, Tab = 0, Row = 0, Column = 1
    PET_TENACITY_GREAT_STAMINA                  = 2116,  // TabId = 409, Tab = 0, Row = 0, Column = 2
    PET_TENACITY_NATURAL_ARMOR                  = 2117,  // TabId = 409, Tab = 0, Row = 0, Column = 3
    PET_TENACITY_SPIKED_COLLAR                  = 2126,  // TabId = 409, Tab = 0, Row = 1, Column = 0
    PET_TENACITY_BOARS_SPEED                    = 2160,  // TabId = 409, Tab = 0, Row = 1, Column = 1
    PET_TENACITY_BLOOD_OF_THE_RHINO             = 2173,  // TabId = 409, Tab = 0, Row = 1, Column = 2
    PET_TENACITY_PET_BARDING                    = 2122,  // TabId = 409, Tab = 0, Row = 1, Column = 3
    PET_TENACITY_CULLING_THE_HERD               = 2110,  // TabId = 409, Tab = 0, Row = 2, Column = 0
    PET_TENACITY_GUARD_DOG                      = 2123,  // TabId = 409, Tab = 0, Row = 2, Column = 1
    PET_TENACITY_LIONHEARTED                    = 2162,  // TabId = 409, Tab = 0, Row = 2, Column = 2
    PET_TENACITY_THUNDERSTOMP                   = 2277,  // TabId = 409, Tab = 0, Row = 2, Column = 3
    PET_TENACITY_GRACE_OF_THE_MANTIS            = 2163,  // TabId = 409, Tab = 0, Row = 3, Column = 2
    PET_TENACITY_GREAT_RESISTANCE               = 2161,  // TabId = 409, Tab = 0, Row = 3, Column = 3
    PET_TENACITY_LAST_STAND                     = 2171,  // TabId = 409, Tab = 0, Row = 4, Column = 0
    PET_TENACITY_TAUNT                          = 2170,  // TabId = 409, Tab = 0, Row = 4, Column = 1
    PET_TENACITY_ROAR_OF_SACRIFICE              = 2172,  // TabId = 409, Tab = 0, Row = 4, Column = 2
    PET_TENACITY_INTERVENE                      = 2169,  // TabId = 409, Tab = 0, Row = 4, Column = 3
    PET_TENACITY_SILVERBACK                     = 2258,  // TabId = 409, Tab = 0, Row = 5, Column = 1
    PET_TENACITY_WILD_HUNT                      = 2255,  // TabId = 409, Tab = 0, Row = 5, Column = 2
    PET_FEROCITY_COBRA_REFLEXES                 = 2107,  // TabId = 410, Tab = 0, Row = 0, Column = 0
    // TODO: same spell, one for flying pets other for land pets. Figure out which is which if it ever matters.
    PET_FEROCITY_DASHDIVE1                      = 2109,  // TabId = 410, Tab = 0, Row = 0, Column = 1
    PET_FEROCITY_DASHDIVE2                      = 2203,  // TabId = 410, Tab = 0, Row = 0, Column = 1
    PET_FEROCITY_GREAT_STAMINA                  = 2112,  // TabId = 410, Tab = 0, Row = 0, Column = 2
    PET_FEROCITY_NATURAL_ARMOR                  = 2113,  // TabId = 410, Tab = 0, Row = 0, Column = 3
    PET_FEROCITY_IMPROVED_COWER                 = 2124,  // TabId = 410, Tab = 0, Row = 1, Column = 0
    PET_FEROCITY_BLOODTHIRSTY                   = 2128,  // TabId = 410, Tab = 0, Row = 1, Column = 1
    PET_FEROCITY_SPIKED_COLLAR                  = 2125,  // TabId = 410, Tab = 0, Row = 1, Column = 2
    PET_FEROCITY_BOARS_SPEED                    = 2151,  // TabId = 410, Tab = 0, Row = 1, Column = 3
    PET_FEROCITY_CULLING_THE_HERD               = 2106,  // TabId = 410, Tab = 0, Row = 2, Column = 0
    PET_FEROCITY_LIONHEARTED                    = 2152,  // TabId = 410, Tab = 0, Row = 2, Column = 2
    // TODO: same spell, one for flying pets other for land pets. Figure out which is which if it ever matters.
    PET_FEROCITY_CHARGESWOOP1                   = 2111,  // TabId = 410, Tab = 0, Row = 2, Column = 3
    PET_FEROCITY_CHARGESWOOP2                   = 2219,  // TabId = 410, Tab = 0, Row = 2, Column = 3
    PET_FEROCITY_HEART_OF_THE_PHOENIX           = 2156,  // TabId = 410, Tab = 0, Row = 3, Column = 1
    PET_FEROCITY_SPIDERS_BITE                   = 2129,  // TabId = 410, Tab = 0, Row = 3, Column = 2
    PET_FEROCITY_GREAT_RESISTANCE               = 2154,  // TabId = 410, Tab = 0, Row = 3, Column = 3
    PET_FEROCITY_RABID                          = 2155,  // TabId = 410, Tab = 0, Row = 4, Column = 0
    PET_FEROCITY_LICK_YOUR_WOUNDS               = 2153,  // TabId = 410, Tab = 0, Row = 4, Column = 1
    PET_FEROCITY_CALL_OF_THE_WILD               = 2157,  // TabId = 410, Tab = 0, Row = 4, Column = 2
    PET_FEROCITY_SHARK_ATTACK                   = 2254,  // TabId = 410, Tab = 0, Row = 5, Column = 0
    PET_FEROCITY_WILD_HUNT                      = 2253,  // TabId = 410, Tab = 0, Row = 5, Column = 2
    PET_CUNNING_COBRA_REFLEXES                  = 2118,  // TabId = 411, Tab = 0, Row = 0, Column = 0
    // TODO: same spell, one for flying pets other for land pets. Figure out which is which if it ever matters.
    PET_CUNNING_DASHDIVE1                       = 2119,  // TabId = 411, Tab = 0, Row = 0, Column = 1
    PET_CUNNING_DASHDIVE2                       = 2201,  // TabId = 411, Tab = 0, Row = 0, Column = 1
    PET_CUNNING_GREAT_STAMINA                   = 2120,  // TabId = 411, Tab = 0, Row = 0, Column = 2
    PET_CUNNING_NATURAL_ARMOR                   = 2121,  // TabId = 411, Tab = 0, Row = 0, Column = 3
    PET_CUNNING_BOARS_SPEED                     = 2165,  // TabId = 411, Tab = 0, Row = 1, Column = 0
    // TODO: Frankly, not sure what the difference is. Presumably one for land, one for air? Because they require Dash or Dive.
    PET_CUNNING_MOBILITY1                       = 2207,  // TabId = 411, Tab = 0, Row = 1, Column = 1
    PET_CUNNING_MOBILITY2                       = 2208,  // TabId = 411, Tab = 0, Row = 1, Column = 1
    PET_CUNNING_OWLS_FOCUS                      = 2182,  // TabId = 411, Tab = 0, Row = 1, Column = 2
    PET_CUNNING_SPIKED_COLLAR                   = 2127,  // TabId = 411, Tab = 0, Row = 1, Column = 3
    PET_CUNNING_CULLING_THE_HERD                = 2166,  // TabId = 411, Tab = 0, Row = 2, Column = 0
    PET_CUNNING_LIONHEARTED                     = 2167,  // TabId = 411, Tab = 0, Row = 2, Column = 1
    PET_CUNNING_CARRION_FEEDER                  = 2206,  // TabId = 411, Tab = 0, Row = 2, Column = 2
    PET_CUNNING_GREAT_RESISTANCE                = 2168,  // TabId = 411, Tab = 0, Row = 3, Column = 1
    PET_CUNNING_CORNERED                        = 2177,  // TabId = 411, Tab = 0, Row = 3, Column = 2
    PET_CUNNING_FEEDING_FRENZY                  = 2183,  // TabId = 411, Tab = 0, Row = 3, Column = 3
    PET_CUNNING_WOLVERINE_BITE                  = 2181,  // TabId = 411, Tab = 0, Row = 4, Column = 0
    PET_CUNNING_ROAR_OF_RECOVERY                = 2184,  // TabId = 411, Tab = 0, Row = 4, Column = 1
    PET_CUNNING_BULLHEADED                      = 2175,  // TabId = 411, Tab = 0, Row = 4, Column = 2
    PET_CUNNING_GRACE_OF_THE_MANTIS             = 2257,  // TabId = 411, Tab = 0, Row = 4, Column = 3
    PET_CUNNING_WILD_HUNT                       = 2256,  // TabId = 411, Tab = 0, Row = 5, Column = 0
    PET_CUNNING_ROAR_OF_SACRIFICE               = 2278  // TabId = 411, Tab = 0, Row = 5, Column = 3
};

enum Glyphs
{
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DARK_COMMAND         = 511,   // Level 55  Spell ID: 58613
    DEATH_KNIGHT_MAJOR_GLYPH_OF_ANTIMAGIC_SHELL      = 512,   // Level 55  Spell ID: 58623
    DEATH_KNIGHT_MAJOR_GLYPH_OF_HEART_STRIKE         = 513,   // Unknown   Spell ID: 58616
    DEATH_KNIGHT_MAJOR_GLYPH_OF_BONE_SHIELD          = 515,   // Level 55  Spell ID: 58673
    DEATH_KNIGHT_MAJOR_GLYPH_OF_CHAINS_OF_ICE        = 516,   // Level 55  Spell ID: 58620
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_GRIP           = 519,   // Level 55  Spell ID: 62259
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_AND_DECAY      = 520,   // Level 55  Spell ID: 58629
    DEATH_KNIGHT_MAJOR_GLYPH_OF_FROST_STRIKE         = 521,   // Level 55  Spell ID: 58647
    DEATH_KNIGHT_MAJOR_GLYPH_OF_ICEBOUND_FORTITUDE   = 523,   // Level 55  Spell ID: 58625
    DEATH_KNIGHT_MAJOR_GLYPH_OF_ICY_TOUCH            = 524,   // Level 55  Spell ID: 58631
    DEATH_KNIGHT_MAJOR_GLYPH_OF_OBLITERATE           = 525,   // Level 55  Spell ID: 58671
    DEATH_KNIGHT_MAJOR_GLYPH_OF_PLAGUE_STRIKE        = 526,   // Level 55  Spell ID: 58657
    DEATH_KNIGHT_MAJOR_GLYPH_OF_THE_GHOUL            = 527,   // Level 55  Spell ID: 58686
    DEATH_KNIGHT_MAJOR_GLYPH_OF_RUNE_STRIKE          = 528,   // Level 55  Spell ID: 58669
    DEATH_KNIGHT_MAJOR_GLYPH_OF_SCOURGE_STRIKE       = 529,   // Level 55  Spell ID: 58642
    DEATH_KNIGHT_MAJOR_GLYPH_OF_STRANGULATE          = 530,   // Level 55  Spell ID: 58618
    DEATH_KNIGHT_MAJOR_GLYPH_OF_UNBREAKABLE_ARMOR    = 531,   // Level 55  Spell ID: 58635
    DEATH_KNIGHT_MAJOR_GLYPH_OF_VAMPIRIC_BLOOD       = 532,   // Level 55  Spell ID: 58676
    DEATH_KNIGHT_MAJOR_GLYPH_OF_RUNE_TAP             = 556,   // Level 55  Spell ID: 59327
    DEATH_KNIGHT_MAJOR_GLYPH_OF_BLOOD_STRIKE         = 557,   // Level 55  Spell ID: 59332
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_STRIKE         = 558,   // Level 55  Spell ID: 59336
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DANCING_RUNE_WEAPON  = 768,   // Level 60  Spell ID: 63330
    DEATH_KNIGHT_MAJOR_GLYPH_OF_HUNGERING_COLD       = 769,   // Level 60  Spell ID: 63331
    DEATH_KNIGHT_MAJOR_GLYPH_OF_UNHOLY_BLIGHT        = 770,   // Level 60  Spell ID: 63332
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DARK_DEATH           = 771,   // Level 55  Spell ID: 63333
    DEATH_KNIGHT_MAJOR_GLYPH_OF_DISEASE              = 772,   // Level 55  Spell ID: 63334
    DEATH_KNIGHT_MAJOR_GLYPH_OF_HOWLING_BLAST        = 773,   // Level 60  Spell ID: 63335
    DEATH_KNIGHT_MINOR_GLYPH_OF_BLOOD_TAP            = 514,   // Level 55  Spell ID: 58640
    DEATH_KNIGHT_MINOR_GLYPH_OF_DEATHS_EMBRACE       = 518,   // Level 55  Spell ID: 58677
    DEATH_KNIGHT_MINOR_GLYPH_OF_HORN_OF_WINTER       = 522,   // Level 55  Spell ID: 58680
    DEATH_KNIGHT_MINOR_GLYPH_OF_PESTILENCE           = 553,   // Level 55  Spell ID: 59309
    DEATH_KNIGHT_MINOR_GLYPH_OF_CORPSE_EXPLOSION     = 554,   // Level 55  Spell ID: 59307
    // TODO: Hmm, this looks suspiciously alike... yet not. What's up?
    DEATH_KNIGHT_MINOR_GLYPH_OF_RAISE_DEAD           = 555,   // Level 55  Spell ID: 60200
    DEATH_KNIGHT_MINOR_GLYPH_OF_RAISE_DEAD2          = 571,   // Level 55  Spell ID: 60200
    DRUID_MAJOR_GLYPH_OF_FRENZIED_REGENERATION       = 161,   // Level 36  Spell ID: 54810
    DRUID_MAJOR_GLYPH_OF_MAUL                        = 162,   // Level 15  Spell ID: 54811
    DRUID_MAJOR_GLYPH_OF_MANGLE                      = 164,   // Level 50  Spell ID: 54813
    DRUID_MAJOR_GLYPH_OF_SHRED                       = 165,   // Level 22  Spell ID: 54815
    DRUID_MAJOR_GLYPH_OF_RIP                         = 166,   // Level 20  Spell ID: 54818
    DRUID_MAJOR_GLYPH_OF_RAKE                        = 167,   // Level 24  Spell ID: 54821
    DRUID_MAJOR_GLYPH_OF_SWIFTMEND                   = 168,   // Level 40  Spell ID: 54824
    DRUID_MAJOR_GLYPH_OF_INNERVATE                   = 169,   // Level 40  Spell ID: 54832
    DRUID_MAJOR_GLYPH_OF_REBIRTH                     = 170,   // Level 20  Spell ID: 54733
    DRUID_MAJOR_GLYPH_OF_REGROWTH                    = 171,   // Level 15  Spell ID: 54743
    DRUID_MAJOR_GLYPH_OF_REJUVENATION                = 172,   // Level 15  Spell ID: 54754
    DRUID_MAJOR_GLYPH_OF_HEALING_TOUCH               = 173,   // Level 15  Spell ID: 54825
    DRUID_MAJOR_GLYPH_OF_LIFEBLOOM                   = 174,   // Level 64  Spell ID: 54826
    DRUID_MAJOR_GLYPH_OF_STARFIRE                    = 175,   // Level 20  Spell ID: 54845
    DRUID_MAJOR_GLYPH_OF_INSECT_SWARM                = 176,   // Level 20  Spell ID: 54830
    DRUID_MAJOR_GLYPH_OF_HURRICANE                   = 177,   // Level 40  Spell ID: 54831
    DRUID_MAJOR_GLYPH_OF_STARFALL                    = 178,   // Level 60  Spell ID: 54828
    DRUID_MAJOR_GLYPH_OF_WRATH                       = 179,   // Level 15  Spell ID: 54756
    DRUID_MAJOR_GLYPH_OF_MOONFIRE                    = 180,   // Level 15  Spell ID: 54829
    DRUID_MAJOR_GLYPH_OF_ENTANGLING_ROOTS            = 181,   // Level 15  Spell ID: 54760
    DRUID_MAJOR_GLYPH_OF_FOCUS                       = 631,   // Level 70  Spell ID: 62080
    DRUID_MAJOR_GLYPH_OF_BERSERK                     = 671,   // Level 60  Spell ID: 62969
    DRUID_MAJOR_GLYPH_OF_WILD_GROWTH                 = 672,   // Level 60  Spell ID: 62970
    DRUID_MAJOR_GLYPH_OF_NOURISH                     = 673,   // Level 80  Spell ID: 62971
    DRUID_MAJOR_GLYPH_OF_SAVAGE_ROAR                 = 674,   // Level 75  Spell ID: 63055
    DRUID_MAJOR_GLYPH_OF_MONSOON                     = 675,   // Level 50  Spell ID: 63056
    DRUID_MAJOR_GLYPH_OF_BARKSKIN                    = 676,   // Level 44  Spell ID: 63057
    DRUID_MAJOR_GLYPH_OF_SURVIVAL_INSTINCTS          = 811,   // Level 20  Spell ID: 65243
    DRUID_MAJOR_GLYPH_OF_CLAW                        = 831,   // Level 20  Spell ID: 67598
    DRUID_MAJOR_GLYPH_OF_RAPID_REJUVENATION          = 891,   // Level 15  Spell ID: 71013
    DRUID_MINOR_GLYPH_OF_AQUATIC_FORM                = 431,   // Level 16  Spell ID: 57856
    DRUID_MINOR_GLYPH_OF_CHALLENGING_ROAR            = 432,   // Level 28  Spell ID: 57858
    DRUID_MINOR_GLYPH_OF_THE_WILD                    = 433,   // Level 15  Spell ID: 57855
    DRUID_MINOR_GLYPH_OF_UNBURDENED_REBIRTH          = 434,   // Level 20  Spell ID: 57857
    DRUID_MINOR_GLYPH_OF_THORNS                      = 435,   // Level 15  Spell ID: 57862
    DRUID_MINOR_GLYPH_OF_DASH                        = 551,   // Level 16  Spell ID: 59219
    DRUID_MINOR_GLYPH_OF_TYPHOON                     = 613,   // Level 70  Spell ID: 62135
    HUNTER_MAJOR_GLYPH_OF_AIMED_SHOT                 = 351,   // Level 20  Spell ID: 56824
    HUNTER_MAJOR_GLYPH_OF_ARCANE_SHOT                = 352,   // Level 15  Spell ID: 56841
    HUNTER_MAJOR_GLYPH_OF_THE_BEAST                  = 353,   // Level 30  Spell ID: 56857
    HUNTER_MAJOR_GLYPH_OF_MENDING                    = 354,   // Level 15  Spell ID: 56833
    HUNTER_MAJOR_GLYPH_OF_ASPECT_OF_THE_VIPER        = 355,   // Level 20  Spell ID: 56851
    HUNTER_MAJOR_GLYPH_OF_BESTIAL_WRATH              = 356,   // Level 40  Spell ID: 56830
    HUNTER_MAJOR_GLYPH_OF_DETERRENCE                 = 357,   // Level 20  Spell ID: 56850
    HUNTER_MAJOR_GLYPH_OF_DISENGAGE                  = 358,   // Level 20  Spell ID: 56844
    HUNTER_MAJOR_GLYPH_OF_FREEZING_TRAP              = 359,   // Level 20  Spell ID: 56845
    HUNTER_MAJOR_GLYPH_OF_FROST_TRAP                 = 360,   // Level 28  Spell ID: 56847
    HUNTER_MAJOR_GLYPH_OF_HUNTERS_MARK               = 361,   // Level 15  Spell ID: 56829
    HUNTER_MAJOR_GLYPH_OF_IMMOLATION_TRAP            = 362,   // Level 16  Spell ID: 56846
    HUNTER_MAJOR_GLYPH_OF_MULTISHOT                  = 364,   // Level 18  Spell ID: 56836
    HUNTER_MAJOR_GLYPH_OF_RAPID_FIRE                 = 365,   // Level 26  Spell ID: 56828
    HUNTER_MAJOR_GLYPH_OF_SERPENT_STING              = 366,   // Level 15  Spell ID: 56832
    HUNTER_MAJOR_GLYPH_OF_SNAKE_TRAP                 = 367,   // Level 68  Spell ID: 56849
    HUNTER_MAJOR_GLYPH_OF_STEADY_SHOT                = 368,   // Level 62  Spell ID: 56826
    HUNTER_MAJOR_GLYPH_OF_TRUESHOT_AURA              = 369,   // Level 40  Spell ID: 56842
    HUNTER_MAJOR_GLYPH_OF_VOLLEY                     = 370,   // Level 40  Spell ID: 56838
    HUNTER_MAJOR_GLYPH_OF_WYVERN_STING               = 371,   // Level 40  Spell ID: 56848
    HUNTER_MAJOR_GLYPH_OF_CHIMERA_SHOT               = 677,   // Level 60  Spell ID: 63065
    HUNTER_MAJOR_GLYPH_OF_EXPLOSIVE_SHOT             = 691,   // Level 60  Spell ID: 63066
    HUNTER_MAJOR_GLYPH_OF_KILL_SHOT                  = 692,   // Level 71  Spell ID: 63067
    HUNTER_MAJOR_GLYPH_OF_EXPLOSIVE_TRAP             = 693,   // Level 34  Spell ID: 63068
    HUNTER_MAJOR_GLYPH_OF_SCATTER_SHOT               = 694,   // Level 20  Spell ID: 63069
    HUNTER_MAJOR_GLYPH_OF_RAPTOR_STRIKE              = 695,   // Level 15  Spell ID: 63086
    HUNTER_MINOR_GLYPH_OF_REVIVE_PET                 = 439,   // Level 15  Spell ID: 57866
    HUNTER_MINOR_GLYPH_OF_MEND_PET                   = 440,   // Level 15  Spell ID: 57870
    HUNTER_MINOR_GLYPH_OF_FEIGN_DEATH                = 441,   // Level 30  Spell ID: 57903
    HUNTER_MINOR_GLYPH_OF_SCARE_BEAST                = 442,   // Level 15  Spell ID: 57902
    HUNTER_MINOR_GLYPH_OF_THE_PACK                   = 443,   // Level 40  Spell ID: 57904
    HUNTER_MINOR_GLYPH_OF_POSSESSED_STRENGTH         = 444,   // Level 15  Spell ID: 57900
    MAGE_MAJOR_GLYPH_OF_ARCANE_EXPLOSION             = 311,   // Level 15  Spell ID: 56360
    MAGE_MAJOR_GLYPH_OF_ARCANE_MISSILES              = 312,   // Level 15  Spell ID: 56363
    MAGE_MAJOR_GLYPH_OF_ARCANE_POWER                 = 313,   // Level 40  Spell ID: 56381
    MAGE_MAJOR_GLYPH_OF_BLINK                        = 314,   // Level 20  Spell ID: 56365
    MAGE_MAJOR_GLYPH_OF_EVOCATION                    = 315,   // Level 20  Spell ID: 56380
    MAGE_MAJOR_GLYPH_OF_FIREBALL                     = 316,   // Level 15  Spell ID: 56368
    MAGE_MAJOR_GLYPH_OF_FIRE_BLAST                   = 317,   // Level 15  Spell ID: 56369
    MAGE_MAJOR_GLYPH_OF_FROST_NOVA                   = 318,   // Level 15  Spell ID: 56376
    MAGE_MAJOR_GLYPH_OF_FROSTBOLT                    = 319,   // Level 15  Spell ID: 56370
    MAGE_MAJOR_GLYPH_OF_ICE_ARMOR                    = 320,   // Level 15  Spell ID: 56384
    MAGE_MAJOR_GLYPH_OF_ICE_BLOCK                    = 321,   // Level 30  Spell ID: 56372
    MAGE_MAJOR_GLYPH_OF_ICE_LANCE                    = 322,   // Level 66  Spell ID: 56377
    MAGE_MAJOR_GLYPH_OF_ICY_VEINS                    = 323,   // Level 20  Spell ID: 56374
    MAGE_MAJOR_GLYPH_OF_SCORCH                       = 324,   // Level 20  Spell ID: 56371
    MAGE_MAJOR_GLYPH_OF_INVISIBILITY                 = 325,   // Level 68  Spell ID: 56366
    MAGE_MAJOR_GLYPH_OF_MAGE_ARMOR                   = 326,   // Level 34  Spell ID: 56383
    MAGE_MAJOR_GLYPH_OF_MANA_GEM                     = 327,   // Level 30  Spell ID: 56367
    MAGE_MAJOR_GLYPH_OF_MOLTEN_ARMOR                 = 328,   // Level 62  Spell ID: 56382
    MAGE_MAJOR_GLYPH_OF_POLYMORPH                    = 329,   // Level 15  Spell ID: 56375
    MAGE_MAJOR_GLYPH_OF_REMOVE_CURSE                 = 330,   // Level 18  Spell ID: 56364
    MAGE_MAJOR_GLYPH_OF_WATER_ELEMENTAL              = 331,   // Level 50  Spell ID: 56373
    MAGE_MAJOR_GLYPH_OF_FROSTFIRE                    = 591,   // Level 75  Spell ID: 61205
    MAGE_MAJOR_GLYPH_OF_ARCANE_BLAST                 = 651,   // Level 64  Spell ID: 62210
    MAGE_MAJOR_GLYPH_OF_DEEP_FREEZE                  = 696,   // Level 60  Spell ID: 63090
    MAGE_MAJOR_GLYPH_OF_LIVING_BOMB                  = 697,   // Level 60  Spell ID: 63091
    MAGE_MAJOR_GLYPH_OF_ARCANE_BARRAGE               = 698,   // Level 60  Spell ID: 63092
    MAGE_MAJOR_GLYPH_OF_MIRROR_IMAGE                 = 699,   // Level 80  Spell ID: 63093
    MAGE_MAJOR_GLYPH_OF_ICE_BARRIER                  = 700,   // Level 46  Spell ID: 63095
    MAGE_MAJOR_GLYPH_OF_ETERNAL_WATER                = 871,   // Level 50  Spell ID: 70937
    MAGE_MINOR_GLYPH_OF_ARCANE_INTELLECT             = 445,   // Level 15  Spell ID: 57924
    MAGE_MINOR_GLYPH_OF_BLAST_WAVE                   = 611,   // Level 70  Spell ID: 62126
    MAGE_MINOR_GLYPH_OF_FIRE_WARD                    = 446,   // Level 20  Spell ID: 57926
    MAGE_MINOR_GLYPH_OF_FROST_WARD                   = 447,   // Level 22  Spell ID: 57927
    MAGE_MINOR_GLYPH_OF_FROST_ARMOR                  = 448,   // Level 15  Spell ID: 57928
    MAGE_MINOR_GLYPH_OF_THE_PENGUIN                  = 450,   // Level 15  Spell ID: 52648
    MAGE_MINOR_GLYPH_OF_SLOW_FALL                    = 451,   // Level 15  Spell ID: 57925
    PALADIN_MAJOR_GLYPH_OF_JUDGEMENT                 = 183,   // Level 15  Spell ID: 54922
    PALADIN_MAJOR_GLYPH_OF_SEAL_OF_COMMAND           = 184,   // Level 20  Spell ID: 54925
    PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_JUSTICE         = 185,   // Level 15  Spell ID: 54923
    PALADIN_MAJOR_GLYPH_OF_SPIRITUAL_ATTUNEMENT      = 186,   // Level 18  Spell ID: 54924
    PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_WRATH           = 187,   // Level 44  Spell ID: 54926
    PALADIN_MAJOR_GLYPH_OF_CRUSADER_STRIKE           = 188,   // Level 20  Spell ID: 54927
    PALADIN_MAJOR_GLYPH_OF_CONSECRATION              = 189,   // Level 20  Spell ID: 54928
    PALADIN_MAJOR_GLYPH_OF_RIGHTEOUS_DEFENSE         = 190,   // Level 15  Spell ID: 54929
    PALADIN_MAJOR_GLYPH_OF_AVENGERS_SHIELD           = 191,   // Level 50  Spell ID: 54930
    PALADIN_MAJOR_GLYPH_OF_TURN_EVIL                 = 192,   // Level 24  Spell ID: 54931
    PALADIN_MAJOR_GLYPH_OF_EXORCISM                  = 193,   // Level 20  Spell ID: 54934
    PALADIN_MAJOR_GLYPH_OF_CLEANSING                 = 194,   // Level 35  Spell ID: 54935
    PALADIN_MAJOR_GLYPH_OF_FLASH_OF_LIGHT            = 195,   // Level 20  Spell ID: 54936
    PALADIN_MAJOR_GLYPH_OF_HOLY_LIGHT                = 196,   // Level 15  Spell ID: 54937
    PALADIN_MAJOR_GLYPH_OF_AVENGING_WRATH            = 197,   // Level 70  Spell ID: 54938
    PALADIN_MAJOR_GLYPH_OF_DIVINITY                  = 198,   // Level 15  Spell ID: 54939
    PALADIN_MAJOR_GLYPH_OF_SEAL_OF_WISDOM            = 199,   // Level 38  Spell ID: 54940
    PALADIN_MAJOR_GLYPH_OF_SEAL_OF_LIGHT             = 200,   // Level 30  Spell ID: 54943
    PALADIN_MAJOR_GLYPH_OF_HOLY_WRATH                = 559,   // Unknown   Spell ID: 56420
    PALADIN_MAJOR_GLYPH_OF_SEAL_OF_RIGHTEOUSNESS     = 560,   // Level 50  Spell ID: 56414
    PALADIN_MAJOR_GLYPH_OF_SEAL_OF_VENGEANCE         = 561,   // Level 50  Spell ID: 56416
    PALADIN_MAJOR_GLYPH_OF_BEACON_OF_LIGHT           = 701,   // Level 60  Spell ID: 63218
    PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_THE_RIGHTEOUS   = 702,   // Level 60  Spell ID: 63219
    PALADIN_MAJOR_GLYPH_OF_DIVINE_STORM              = 703,   // Level 60  Spell ID: 63220
    PALADIN_MAJOR_GLYPH_OF_SHIELD_OF_RIGHTEOUSNESS   = 704,   // Level 75  Spell ID: 63222
    PALADIN_MAJOR_GLYPH_OF_DIVINE_PLEA               = 705,   // Level 71  Spell ID: 63223
    PALADIN_MAJOR_GLYPH_OF_HOLY_SHOCK                = 706,   // Level 40  Spell ID: 63224
    PALADIN_MAJOR_GLYPH_OF_SALVATION                 = 707,   // Level 26  Spell ID: 63225
    PALADIN_MINOR_GLYPH_OF_BLESSING_OF_KINGS         = 452,   // Level 20  Spell ID: 57937
    PALADIN_MINOR_GLYPH_OF_BLESSING_OF_MIGHT         = 453,   // Level 15  Spell ID: 57958
    PALADIN_MINOR_GLYPH_OF_BLESSING_OF_WISDOM        = 454,   // Level 15  Spell ID: 57979
    PALADIN_MINOR_GLYPH_OF_LAY_ON_HANDS              = 455,   // Level 15  Spell ID: 57955
    PALADIN_MINOR_GLYPH_OF_SENSE_UNDEAD              = 456,   // Level 20  Spell ID: 57947
    PALADIN_MINOR_GLYPH_OF_THE_WISE                  = 457,   // Level 15  Spell ID: 57954
    PRIEST_MAJOR_GLYPH_OF_CIRCLE_OF_HEALING          = 251,   // Level 50  Spell ID: 55675
    PRIEST_MAJOR_GLYPH_OF_DISPEL_MAGIC               = 252,   // Level 18  Spell ID: 55677
    PRIEST_MAJOR_GLYPH_OF_FADE                       = 253,   // Level 15  Spell ID: 55684
    PRIEST_MAJOR_GLYPH_OF_FEAR_WARD                  = 254,   // Level 20  Spell ID: 55678
    PRIEST_MAJOR_GLYPH_OF_FLASH_HEAL                 = 255,   // Level 20  Spell ID: 55679
    PRIEST_MAJOR_GLYPH_OF_HOLY_NOVA                  = 256,   // Level 20  Spell ID: 55683
    PRIEST_MAJOR_GLYPH_OF_INNER_FIRE                 = 257,   // Level 15  Spell ID: 55686
    PRIEST_MAJOR_GLYPH_OF_LIGHTWELL                  = 258,   // Level 40  Spell ID: 55673
    PRIEST_MAJOR_GLYPH_OF_MASS_DISPEL                = 259,   // Level 70  Spell ID: 55691
    PRIEST_MAJOR_GLYPH_OF_MIND_CONTROL               = 260,   // Level 30  Spell ID: 55688
    PRIEST_MAJOR_GLYPH_OF_SHADOW_WORD_PAIN           = 261,   // Level 15  Spell ID: 55681
    PRIEST_MAJOR_GLYPH_OF_SHADOW                     = 262,   // Level 20  Spell ID: 55689
    PRIEST_MAJOR_GLYPH_OF_POWER_WORD_SHIELD          = 263,   // Level 15  Spell ID: 55672
    PRIEST_MAJOR_GLYPH_OF_PRAYER_OF_HEALING          = 264,   // Level 30  Spell ID: 55680
    PRIEST_MAJOR_GLYPH_OF_PSYCHIC_SCREAM             = 265,   // Level 15  Spell ID: 55676
    PRIEST_MAJOR_GLYPH_OF_RENEW                      = 266,   // Level 15  Spell ID: 55674
    PRIEST_MAJOR_GLYPH_OF_SCOURGE_IMPRISONMENT       = 267,   // Level 20  Spell ID: 55690
    PRIEST_MAJOR_GLYPH_OF_SHADOW_WORD_DEATH          = 268,   // Level 62  Spell ID: 55682
    PRIEST_MAJOR_GLYPH_OF_MIND_FLAY                  = 269,   // Level 20  Spell ID: 55687
    PRIEST_MAJOR_GLYPH_OF_SMITE                      = 270,   // Level 15  Spell ID: 55692
    PRIEST_MAJOR_GLYPH_OF_SPIRIT_OF_REDEMPTION       = 271,   // Level 30  Spell ID: 55685
    PRIEST_MAJOR_GLYPH_OF_DISPERSION                 = 708,   // Level 60  Spell ID: 63229
    PRIEST_MAJOR_GLYPH_OF_GUARDIAN_SPIRIT            = 709,   // Level 60  Spell ID: 63231
    PRIEST_MAJOR_GLYPH_OF_PENANCE                    = 710,   // Level 60  Spell ID: 63235
    PRIEST_MAJOR_GLYPH_OF_MIND_SEAR                  = 711,   // Level 75  Spell ID: 63237
    PRIEST_MAJOR_GLYPH_OF_HYMN_OF_HOPE               = 712,   // Level 60  Spell ID: 63246
    PRIEST_MAJOR_GLYPH_OF_PAIN_SUPPRESSION           = 713,   // Level 50  Spell ID: 63248
    PRIEST_MINOR_GLYPH_OF_FADING                     = 458,   // Level 15  Spell ID: 57985
    PRIEST_MINOR_GLYPH_OF_LEVITATE                   = 459,   // Level 34  Spell ID: 57987
    PRIEST_MINOR_GLYPH_OF_FORTITUDE                  = 460,   // Level 15  Spell ID: 58009
    PRIEST_MINOR_GLYPH_OF_SHACKLE_UNDEAD             = 461,   // Level 20  Spell ID: 57986
    PRIEST_MINOR_GLYPH_OF_SHADOW_PROTECTION          = 462,   // Level 30  Spell ID: 58015
    PRIEST_MINOR_GLYPH_OF_SHADOWFIEND                = 463,   // Level 66  Spell ID: 58228
    ROGUE_MAJOR_GLYPH_OF_ADRENALINE_RUSH             = 391,   // Level 40  Spell ID: 56808
    ROGUE_MAJOR_GLYPH_OF_AMBUSH                      = 392,   // Level 18  Spell ID: 56813
    ROGUE_MAJOR_GLYPH_OF_BACKSTAB                    = 393,   // Level 15  Spell ID: 56800
    ROGUE_MAJOR_GLYPH_OF_BLADE_FLURRY                = 394,   // Level 30  Spell ID: 56818
    ROGUE_MAJOR_GLYPH_OF_CRIPPLING_POISON            = 395,   // Level 20  Spell ID: 56820
    ROGUE_MAJOR_GLYPH_OF_DEADLY_THROW                = 396,   // Level 64  Spell ID: 56806
    ROGUE_MAJOR_GLYPH_OF_EVASION                     = 397,   // Level 15  Spell ID: 56799
    ROGUE_MAJOR_GLYPH_OF_EVISCERATE                  = 398,   // Level 15  Spell ID: 56802
    ROGUE_MAJOR_GLYPH_OF_EXPOSE_ARMOR                = 399,   // Level 15  Spell ID: 56803
    ROGUE_MAJOR_GLYPH_OF_FEINT                       = 400,   // Level 16  Spell ID: 56804
    ROGUE_MAJOR_GLYPH_OF_GARROTE                     = 401,   // Level 15  Spell ID: 56812
    ROGUE_MAJOR_GLYPH_OF_GHOSTLY_STRIKE              = 402,   // Level 20  Spell ID: 56814
    ROGUE_MAJOR_GLYPH_OF_GOUGE                       = 403,   // Level 15  Spell ID: 56809
    ROGUE_MAJOR_GLYPH_OF_HEMORRHAGE                  = 404,   // Level 30  Spell ID: 56807
    ROGUE_MAJOR_GLYPH_OF_PREPARATION                 = 405,   // Level 30  Spell ID: 56819
    ROGUE_MAJOR_GLYPH_OF_RUPTURE                     = 406,   // Level 20  Spell ID: 56801
    ROGUE_MAJOR_GLYPH_OF_SAP                         = 407,   // Level 15  Spell ID: 56798
    ROGUE_MAJOR_GLYPH_OF_VIGOR                       = 408,   // Level 70  Spell ID: 56805
    ROGUE_MAJOR_GLYPH_OF_SINISTER_STRIKE             = 409,   // Level 15  Spell ID: 56821
    ROGUE_MAJOR_GLYPH_OF_SLICE_AND_DICE              = 410,   // Level 15  Spell ID: 56810
    ROGUE_MAJOR_GLYPH_OF_SPRINT                      = 411,   // Level 15  Spell ID: 56811
    ROGUE_MAJOR_GLYPH_OF_HUNGER_FOR_BLOOD            = 714,   // Level 60  Spell ID: 63249
    ROGUE_MAJOR_GLYPH_OF_KILLING_SPREE               = 715,   // Level 60  Spell ID: 63252
    ROGUE_MAJOR_GLYPH_OF_SHADOW_DANCE                = 716,   // Level 60  Spell ID: 63253
    ROGUE_MAJOR_GLYPH_OF_FAN_OF_KNIVES               = 731,   // Level 80  Spell ID: 63254
    ROGUE_MAJOR_GLYPH_OF_TRICKS_OF_THE_TRADE         = 732,   // Level 75  Spell ID: 63256
    ROGUE_MAJOR_GLYPH_OF_MUTILATE                    = 733,   // Level 50  Spell ID: 63268
    ROGUE_MAJOR_GLYPH_OF_CLOAK_OF_SHADOWS            = 734,   // Level 66  Spell ID: 63269
    ROGUE_MINOR_GLYPH_OF_DISTRACT                    = 464,   // Level 22  Spell ID: 58032
    ROGUE_MINOR_GLYPH_OF_PICK_LOCK                   = 465,   // Level 16  Spell ID: 58027
    ROGUE_MINOR_GLYPH_OF_PICK_POCKET                 = 466,   // Level 15  Spell ID: 58017
    ROGUE_MINOR_GLYPH_OF_SAFE_FALL                   = 467,   // Level 40  Spell ID: 58033
    ROGUE_MINOR_GLYPH_OF_BLURRED_SPEED               = 468,   // Level 15  Spell ID: 58039
    ROGUE_MINOR_GLYPH_OF_VANISH                      = 469,   // Level 22  Spell ID: 58038
    SHAMAN_MAJOR_GLYPH_OF_WATER_MASTERY              = 211,   // Level 20  Spell ID: 55436
    SHAMAN_MAJOR_GLYPH_OF_CHAIN_HEAL                 = 212,   // Level 40  Spell ID: 55437
    SHAMAN_MAJOR_GLYPH_OF_CHAIN_LIGHTNING            = 213,   // Level 32  Spell ID: 55449
    SHAMAN_MAJOR_GLYPH_OF_LAVA                       = 214,   // Level 66  Spell ID: 55454
    SHAMAN_MAJOR_GLYPH_OF_SHOCKING                   = 215,   // Unknown   Spell ID: 55442
    SHAMAN_MAJOR_GLYPH_OF_EARTHLIVING_WEAPON         = 216,   // Level 30  Spell ID: 55439
    SHAMAN_MAJOR_GLYPH_OF_FIRE_ELEMENTAL_TOTEM       = 217,   // Level 68  Spell ID: 55455
    SHAMAN_MAJOR_GLYPH_OF_FIRE_NOVA                  = 218,   // Level 15  Spell ID: 55450
    SHAMAN_MAJOR_GLYPH_OF_FLAME_SHOCK                = 219,   // Level 15  Spell ID: 55447
    SHAMAN_MAJOR_GLYPH_OF_FLAMETONGUE_WEAPON         = 220,   // Level 15  Spell ID: 55451
    SHAMAN_MAJOR_GLYPH_OF_FROST_SHOCK                = 221,   // Level 20  Spell ID: 55443
    SHAMAN_MAJOR_GLYPH_OF_HEALING_STREAM_TOTEM       = 222,   // Level 20  Spell ID: 55456
    SHAMAN_MAJOR_GLYPH_OF_HEALING_WAVE               = 223,   // Level 15  Spell ID: 55440
    SHAMAN_MAJOR_GLYPH_OF_LESSER_HEALING_WAVE        = 224,   // Level 20  Spell ID: 55438
    SHAMAN_MAJOR_GLYPH_OF_LIGHTNING_SHIELD           = 225,   // Level 15  Spell ID: 55448
    SHAMAN_MAJOR_GLYPH_OF_LIGHTNING_BOLT             = 226,   // Level 15  Spell ID: 55453
    SHAMAN_MAJOR_GLYPH_OF_STORMSTRIKE                = 228,   // Level 40  Spell ID: 55446
    SHAMAN_MAJOR_GLYPH_OF_LAVA_LASH                  = 229,   // Level 15  Spell ID: 55444
    SHAMAN_MAJOR_GLYPH_OF_ELEMENTAL_MASTERY          = 230,   // Level 50  Spell ID: 55452
    SHAMAN_MAJOR_GLYPH_OF_WINDFURY_WEAPON            = 231,   // Level 30  Spell ID: 55445
    SHAMAN_MAJOR_GLYPH_OF_THUNDER                    = 735,   // Level 60  Spell ID: 63270
    SHAMAN_MAJOR_GLYPH_OF_FERAL_SPIRIT               = 736,   // Level 60  Spell ID: 63271
    SHAMAN_MAJOR_GLYPH_OF_RIPTIDE                    = 737,   // Level 60  Spell ID: 63273
    SHAMAN_MAJOR_GLYPH_OF_EARTH_SHIELD               = 751,   // Level 50  Spell ID: 63279
    SHAMAN_MAJOR_GLYPH_OF_TOTEM_OF_WRATH             = 752,   // Level 50  Spell ID: 63280
    SHAMAN_MAJOR_GLYPH_OF_HEX                        = 753,   // Level 80  Spell ID: 63291
    SHAMAN_MAJOR_GLYPH_OF_STONECLAW_TOTEM            = 754,   // Level 15  Spell ID: 63298
    SHAMAN_MINOR_GLYPH_OF_ASTRAL_RECALL              = 470,   // Level 30  Spell ID: 58058
    SHAMAN_MINOR_GLYPH_OF_RENEWED_LIFE               = 473,   // Level 30  Spell ID: 58059
    SHAMAN_MINOR_GLYPH_OF_WATER_BREATHING            = 474,   // Level 22  Spell ID: 58055
    SHAMAN_MINOR_GLYPH_OF_WATER_SHIELD               = 475,   // Level 20  Spell ID: 58063
    SHAMAN_MINOR_GLYPH_OF_WATER_WALKING              = 476,   // Level 28  Spell ID: 58057
    SHAMAN_MINOR_GLYPH_OF_GHOST_WOLF                 = 552,   // Level 16  Spell ID: 59289
    SHAMAN_MINOR_GLYPH_OF_THUNDERSTORM               = 612,   // Level 70  Spell ID: 62132
    WARLOCK_MAJOR_GLYPH_OF_INCINERATE                = 272,   // Unknown   Spell ID: 56242
    WARLOCK_MAJOR_GLYPH_OF_CONFLAGRATE               = 273,   // Level 40  Spell ID: 56235
    WARLOCK_MAJOR_GLYPH_OF_CORRUPTION                = 274,   // Level 15  Spell ID: 56218
    WARLOCK_MAJOR_GLYPH_OF_CURSE_OF_AGONY            = 275,   // Level 15  Spell ID: 56241
    WARLOCK_MAJOR_GLYPH_OF_DEATH_COIL                = 276,   // Level 42  Spell ID: 56232
    WARLOCK_MAJOR_GLYPH_OF_FEAR                      = 277,   // Level 15  Spell ID: 56244
    WARLOCK_MAJOR_GLYPH_OF_FELGUARD                  = 278,   // Level 50  Spell ID: 56246
    WARLOCK_MAJOR_GLYPH_OF_FELHUNTER                 = 279,   // Level 30  Spell ID: 56249
    WARLOCK_MAJOR_GLYPH_OF_HEALTH_FUNNEL             = 280,   // Level 15  Spell ID: 56238
    WARLOCK_MAJOR_GLYPH_OF_HEALTHSTONE               = 281,   // Level 15  Spell ID: 56224
    WARLOCK_MAJOR_GLYPH_OF_HOWL_OF_TERROR            = 282,   // Level 40  Spell ID: 56217
    WARLOCK_MAJOR_GLYPH_OF_IMMOLATE                  = 283,   // Level 15  Spell ID: 56228
    WARLOCK_MAJOR_GLYPH_OF_IMP                       = 284,   // Level 15  Spell ID: 56248
    WARLOCK_MAJOR_GLYPH_OF_SEARING_PAIN              = 285,   // Level 18  Spell ID: 56226
    WARLOCK_MAJOR_GLYPH_OF_SHADOW_BOLT               = 286,   // Level 15  Spell ID: 56240
    WARLOCK_MAJOR_GLYPH_OF_SHADOWBURN                = 287,   // Level 20  Spell ID: 56229
    WARLOCK_MAJOR_GLYPH_OF_SIPHON_LIFE               = 288,   // Level 30  Spell ID: 56216
    WARLOCK_MAJOR_GLYPH_OF_SOULSTONE                 = 289,   // Level 18  Spell ID: 56231
    WARLOCK_MAJOR_GLYPH_OF_SUCCUBUS                  = 290,   // Level 26  Spell ID: 56250
    WARLOCK_MAJOR_GLYPH_OF_UNSTABLE_AFFLICTION       = 291,   // Level 50  Spell ID: 56233
    WARLOCK_MAJOR_GLYPH_OF_VOIDWALKER                = 292,   // Level 15  Spell ID: 56247
    WARLOCK_MAJOR_GLYPH_OF_HAUNT                     = 755,   // Level 60  Spell ID: 63302
    WARLOCK_MAJOR_GLYPH_OF_METAMORPHOSIS             = 756,   // Level 60  Spell ID: 63303
    WARLOCK_MAJOR_GLYPH_OF_CHAOS_BOLT                = 757,   // Level 60  Spell ID: 63304
    WARLOCK_MAJOR_GLYPH_OF_DEMONIC_CIRCLE            = 758,   // Level 80  Spell ID: 63309
    WARLOCK_MAJOR_GLYPH_OF_SHADOWFLAME               = 759,   // Level 75  Spell ID: 63310
    WARLOCK_MAJOR_GLYPH_OF_LIFE_TAP                  = 760,   // Level 15  Spell ID: 63320
    WARLOCK_MAJOR_GLYPH_OF_SOUL_LINK                 = 761,   // Level 20  Spell ID: 63312
    WARLOCK_MAJOR_GLYPH_OF_QUICK_DECAY               = 911,   // Level 15  Spell ID: 70947
    WARLOCK_MINOR_GLYPH_OF_UNENDING_BREATH           = 477,   // Level 15  Spell ID: 58079
    WARLOCK_MINOR_GLYPH_OF_DRAIN_SOUL                = 478,   // Level 15  Spell ID: 58070
    WARLOCK_MINOR_GLYPH_OF_KILROGG                   = 479,   // Level 22  Spell ID: 58081
    WARLOCK_MINOR_GLYPH_OF_ENSLAVE_DEMON             = 481,   // Level 30  Spell ID: 58107
    WARLOCK_MINOR_GLYPH_OF_SOULS                     = 482,   // Level 68  Spell ID: 58094
    WARRIOR_MAJOR_GLYPH_OF_MORTAL_STRIKE             = 489,   // Level 40  Spell ID: 58368
    WARRIOR_MAJOR_GLYPH_OF_BLOODTHIRST               = 490,   // Level 40  Spell ID: 58369
    WARRIOR_MAJOR_GLYPH_OF_RAPID_CHARGE              = 491,   // Level 15  Spell ID: 58355
    WARRIOR_MAJOR_GLYPH_OF_CLEAVING                  = 492,   // Level 20  Spell ID: 58366
    WARRIOR_MAJOR_GLYPH_OF_DEVASTATE                 = 493,   // Level 40  Spell ID: 58388
    WARRIOR_MAJOR_GLYPH_OF_EXECUTION                 = 494,   // Level 24  Spell ID: 58367
    WARRIOR_MAJOR_GLYPH_OF_HAMSTRING                 = 495,   // Level 15  Spell ID: 58372
    WARRIOR_MAJOR_GLYPH_OF_HEROIC_STRIKE             = 496,   // Level 15  Spell ID: 58357
    WARRIOR_MAJOR_GLYPH_OF_INTERVENE                 = 497,   // Level 70  Spell ID: 58377
    WARRIOR_MAJOR_GLYPH_OF_BARBARIC_INSULTS          = 498,   // Level 16  Spell ID: 58365
    WARRIOR_MAJOR_GLYPH_OF_OVERPOWER                 = 499,   // Level 15  Spell ID: 58386
    WARRIOR_MAJOR_GLYPH_OF_RENDING                   = 500,   // Level 15  Spell ID: 58385
    WARRIOR_MAJOR_GLYPH_OF_REVENGE                   = 501,   // Level 15  Spell ID: 58364
    WARRIOR_MAJOR_GLYPH_OF_BLOCKING                  = 502,   // Level 40  Spell ID: 58375
    WARRIOR_MAJOR_GLYPH_OF_LAST_STAND                = 503,   // Level 20  Spell ID: 58376
    WARRIOR_MAJOR_GLYPH_OF_SUNDER_ARMOR              = 504,   // Level 15  Spell ID: 58387
    WARRIOR_MAJOR_GLYPH_OF_SWEEPING_STRIKES          = 505,   // Level 30  Spell ID: 58384
    WARRIOR_MAJOR_GLYPH_OF_TAUNT                     = 506,   // Level 15  Spell ID: 58353
    WARRIOR_MAJOR_GLYPH_OF_RESONATING_POWER          = 507,   // Level 15  Spell ID: 58356
    WARRIOR_MAJOR_GLYPH_OF_VICTORY_RUSH              = 508,   // Level 62  Spell ID: 58382
    WARRIOR_MAJOR_GLYPH_OF_WHIRLWIND                 = 509,   // Level 36  Spell ID: 58370
    WARRIOR_MAJOR_GLYPH_OF_BLADESTORM                = 762,   // Level 60  Spell ID: 63324
    WARRIOR_MAJOR_GLYPH_OF_SHOCKWAVE                 = 763,   // Level 60  Spell ID: 63325
    WARRIOR_MAJOR_GLYPH_OF_VIGILANCE                 = 764,   // Level 40  Spell ID: 63326
    WARRIOR_MAJOR_GLYPH_OF_ENRAGED_REGENERATION      = 765,   // Level 75  Spell ID: 63327
    WARRIOR_MAJOR_GLYPH_OF_SPELL_REFLECTION          = 766,   // Level 64  Spell ID: 63328
    WARRIOR_MAJOR_GLYPH_OF_SHIELD_WALL               = 767,   // Level 28  Spell ID: 63329
    WARRIOR_MINOR_GLYPH_OF_BATTLE                    = 483,   // Level 15  Spell ID: 58095
    WARRIOR_MINOR_GLYPH_OF_BLOODRAGE                 = 484,   // Level 15  Spell ID: 58096
    WARRIOR_MINOR_GLYPH_OF_CHARGE                    = 485,   // Level 15  Spell ID: 58097
    WARRIOR_MINOR_GLYPH_OF_MOCKING_BLOW              = 486,   // Level 16  Spell ID: 58099
    WARRIOR_MINOR_GLYPH_OF_THUNDER_CLAP              = 487,   // Level 15  Spell ID: 58098
    WARRIOR_MINOR_GLYPH_OF_ENDURING_VICTORY          = 488,   // Level 62  Spell ID: 58104
    WARRIOR_MINOR_GLYPH_OF_COMMAND                   = 851,   // Level 68  Spell ID: 68164
};

/**
* So they're kept track of somewhere... but let's comment them out.
enum GlyphsUnknown
{
UNKNOWN_MAJOR_GLYPH_OF_NATURAL_FORCE             = 2,     // Unknown   Spell ID: 52084
UNKNOWN_UNKNOWN_GLYPH_CRUELTY                    = 21,    // Unknown   Spell ID: 12320
UNKNOWN_UNKNOWN_GLYPH_ANTICIPATION               = 22,    // Unknown   Spell ID: 12297
UNKNOWN_UNKNOWN_GLYPH_VENOMOUS_MANA              = 61,    // Unknown   Spell ID: 46831
UNKNOWN_MINOR_GLYPH_OF_NATURAL_FORCE             = 81,    // Unknown   Spell ID: 52084
UNKNOWN_UNKNOWN_GLYPH_GLYPH_OF_MOONFIRE          = 82,    // Unknown   Spell ID: 52085
UNKNOWN_MAJOR_                                   = 101,   // Unknown   Spell ID: NULL
UNKNOWN_UNKNOWN_GLYPH_COPY_OF_HOLY_BOLT          = 121,   // Unknown   Spell ID: 46487
UNKNOWN_MINOR_GLYPH_OF_THE_WHITE_BEAR            = 141,   // Unknown   Spell ID: 54292
UNKNOWN_MAJOR_GLYPH_OF_GROWLING                  = 163,   // Unknown   Spell ID: 54812
UNKNOWN_MINOR_GLYPH_OF_THE_RED_LYNX              = 182,   // Unknown   Spell ID: 54912
UNKNOWN_MAJOR_GLYPH_OF_LAVA                      = 214,   // Unknown   Spell ID: 55454
UNKNOWN_MAJOR_GLYPH_OF_MANA_TIDE                 = 227,   // Unknown   Spell ID: 55441
UNKNOWN_MAJOR_GLYPH_OF_MOLTEN_ARMOR              = 328,   // Unknown   Spell ID: 56382
UNKNOWN_MAJOR_GLYPH_OF_THE_HAWK                  = 363,   // Unknown   Spell ID: 56856
UNKNOWN_MINOR_GLYPH_OF_THE_FOREST_LYNX           = 436,   // Unknown   Spell ID: 58133
UNKNOWN_MINOR_GLYPH_OF_THE_BLACK_BEAR            = 438,   // Unknown   Spell ID: 58132
UNKNOWN_MINOR_GLYPH_OF_THE_BEAR_CUB              = 449,   // Unknown   Spell ID: 58136
UNKNOWN_MINOR_GLYPH_OF_THE_ARCTIC_WOLF           = 471,   // Unknown   Spell ID: 58135
UNKNOWN_MINOR_GLYPH_OF_THE_BLACK_WOLF            = 472,   // Unknown   Spell ID: 58134
UNKNOWN_MINOR_GLYPH_OF_CURSE_OF_EXHAUSION        = 480,   // Unknown   Spell ID: 58080
UNKNOWN_MINOR_GLYPH_OF_BLAST_WAVE                = 611,   // Unknown   Spell ID: 62126
UNKNOWN_MAJOR_GLYPH_OF_ENVENOM                   = 791,   // Unknown   Spell ID: 64199
};*/

// TODO: replace this with mangos data... I mean this has GOT to be out there somewhere already
//       and when you do, don't forget to change everywhere (including the sql file)
// But in the meantime, value + 412 = TabId.
enum ClassesCombatPets
{
    CLASS_PET_CUNNING  = -1,
    CLASS_PET_FEROCITY = -2,
    CLASS_PET_TENACITY = -3
};

enum TalentSpecPurpose
{
    TSP_NONE                        = 0x00000000,  // should probably error out?
    TSP_PVE_TANK                    = 0x00000001,
    TSP_PVE_DPS                     = 0x00000002,
    TSP_PVE_LEVELING                = 0x00000004,
    TSP_PVE_HEALING                 = 0x00000008,
    TSP_PVE_INSTANCING              = 0x00000010,
    TSP_PVE_RAID                    = 0x00000020,
    TSP_PVE_ALL                     = 0x0000FFFF,  // Highly recommend AGAINST using this
    TSP_PVP_TANK                    = 0x00010000,
    TSP_PVP_DPS                     = 0x00020000,
    TSP_PVP_LEVELING                = 0x00040000,
    TSP_PVP_TWINK                   = 0x00080000,
    TSP_PVP_HEALING                 = 0x00100000,
    TSP_PVP_ARENA                   = 0x00200000,
    TSP_PVP_ALL                     = 0xFFFF0000   // Highly recommend AGAINST using this
};

enum ManaPotionsId
{
    MINOR_MANA_POTION                   = 15715,
    LESSER_MANA_POTION                  = 15716,
    MANA_POTION                         = 15717,
    GREATER_MANA_POTION                 = 15718,
    SUPERIOR_MANA_POTION                = 24151,
    MAJOR_MANA_POTION                   = 21672,
    SUPER_MANA_POTION                   = 37808,
    UNSTABLE_MANA_POTION                = 23731,
    FEL_MANA_POTION                     = 44295,
    CRYSTAL_MANA_POTION                 = 47133,
    AUCHENAI_MANA_POTION                = 37808,
    MINOR_REJUVENATION_POTION           = 2345,
    MAJOR_REJUVENATION_POTION           = 18253
};

enum ManaRunesId
{
    DEMONIC_RUNE                        = 22952,
    DARK_RUNE                           = 32905
};

enum HealingItemDisplayId
{
    MAJOR_HEALING_POTION                = 24152,
    WHIPPER_ROOT_TUBER                  = 21974,
    NIGHT_DRAGON_BREATH                 = 21975,
    LIMITED_INVULNERABILITY_POTION      = 24213,
    GREATER_DREAMLESS_SLEEP_POTION      = 17403,
    SUPERIOR_HEALING_POTION             = 15714,
    CRYSTAL_RESTORE                     = 2516,
    DREAMLESS_SLEEP_POTION              = 17403,
    GREATER_HEALING_POTION              = 15713,
    HEALING_POTION                      = 15712,
    LESSER_HEALING_POTION               = 15711,
    DISCOLORED_HEALING_POTION           = 15736,
    MINOR_HEALING_POTION                = 15710,
    VOLATILE_HEALING_POTION             = 24212,
    SUPER_HEALING_POTION                = 37807,
    CRYSTAL_HEALING_POTION              = 47132,
    FEL_REGENERATION_POTION             = 37864,
    MAJOR_DREAMLESS_SLEEP_POTION        = 37845,
};

enum MainSpec
{
    MAGE_SPEC_FIRE              = 41,
    MAGE_SPEC_FROST             = 61,
    MAGE_SPEC_ARCANE            = 81,
    WARRIOR_SPEC_ARMS           = 161,
    WARRIOR_SPEC_PROTECTION     = 163,
    WARRIOR_SPEC_FURY           = 164,
    ROGUE_SPEC_COMBAT           = 181,
    ROGUE_SPEC_ASSASSINATION    = 182,
    ROGUE_SPEC_SUBTELTY         = 183,
    PRIEST_SPEC_DISCIPLINE      = 201,
    PRIEST_SPEC_HOLY            = 202,
    PRIEST_SPEC_SHADOW          = 203,
    SHAMAN_SPEC_ELEMENTAL       = 261,
    SHAMAN_SPEC_RESTORATION     = 262,
    SHAMAN_SPEC_ENHANCEMENT     = 263,
    DRUID_SPEC_FERAL            = 281,
    DRUID_SPEC_RESTORATION      = 282,
    DRUID_SPEC_BALANCE          = 283,
    WARLOCK_SPEC_DESTRUCTION    = 301,
    WARLOCK_SPEC_AFFLICTION     = 302,
    WARLOCK_SPEC_DEMONOLOGY     = 303,
    HUNTER_SPEC_BEASTMASTERY    = 361,
    HUNTER_SPEC_SURVIVAL        = 362,
    HUNTER_SPEC_MARKSMANSHIP    = 363,
    PALADIN_SPEC_RETRIBUTION    = 381,
    PALADIN_SPEC_HOLY           = 382,
    PALADIN_SPEC_PROTECTION     = 383,
    DEATHKNIGHT_SPEC_BLOOD      = 398,
    DEATHKNIGHT_SPEC_FROST      = 399,
    DEATHKNIGHT_SPEC_UNHOLY     = 400
};

struct TalentSpec
{
    std::string specName;
    short specClass;
    TalentSpecPurpose specPurpose;
    uint16 talentId[71];
    uint16 glyphIdMajor[3];
    uint16 glyphIdMinor[3];
};

enum NotableItems
{
    // Skeleton Keys
    SILVER_SKELETON_KEY = 15869,
    GOLDEN_SKELETON_KEY = 15870,
    TRUESILVER_SKELETON_KEY = 15871,
    ARCANITE_SKELETON_KEY = 15872,
    TITANIUM_SKELETON_KEY = 43853,
    COBALT_SKELETON_KEY = 43854,
    // Lock Charges
    SMALL_SEAFORIUM_CHARGE = 4367,
    LARGE_SEAFORIUM_CHARGE = 4398,
    POWERFUL_SEAFORIUM_CHARGE = 18594,
    ELEMENTAL_SEAFORIUM_CHARGE = 23819
};

enum CombatManeuverReturns
{
    // TODO: RETURN_NO_ACTION_UNKNOWN is not part of ANY_OK or ANY_ERROR. It's also bad form and should be eliminated ASAP.
    RETURN_NO_ACTION_OK                 = 0x01, // No action taken during this combat maneuver, as intended (just wait, etc...)
    RETURN_NO_ACTION_UNKNOWN            = 0x02, // No action taken during this combat maneuver, unknown reason
    RETURN_NO_ACTION_ERROR              = 0x04, // No action taken due to error
    RETURN_NO_ACTION_INVALIDTARGET      = 0x08, // No action taken - invalid target
    RETURN_FINISHED_FIRST_MOVES         = 0x10, // Last action of first-combat-maneuver finished, continue onto next-combat-maneuver
    RETURN_CONTINUE                     = 0x20, // Continue first moves; normal return value for next-combat-maneuver
    RETURN_NO_ACTION_INSUFFICIENT_POWER = 0x40, // No action taken due to insufficient power (rage, focus, mana, runes)
    RETURN_ANY_OK                       = 0x31, // All the OK values bitwise OR'ed
    RETURN_ANY_ACTION                   = 0x30, // All returns that result in action (which should also be 'OK')
    RETURN_ANY_ERROR                    = 0x4C  // All the ERROR values bitwise OR'ed
};

enum AutoEquipEnum
{
    AUTOEQUIP_OFF  = 0,
    AUTOEQUIP_ON   = 1,
    AUTOEQUIP_ONCE = 2
};

enum m_FollowAutoGo
{
    FOLLOWAUTOGO_OFF        = 0,
    FOLLOWAUTOGO_INIT       = 1,
    FOLLOWAUTOGO_SET        = 2,
    FOLLOWAUTOGO_RESET      = 3,
    FOLLOWAUTOGO_RUN        = 4
};

class PlayerbotAI
{
    public:
        enum ScenarioType
        {
            SCENARIO_PVE,
            SCENARIO_PVE_ELITE, // group (5 members max) when an elite is near - most likely instance (can happen in open world)
            SCENARIO_PVE_RAID,
            SCENARIO_PVP_DUEL,
            SCENARIO_PVP_BG,    // You'll probably want to expand this to suit goal? (capture the flag, assault, domination, ...)
            SCENARIO_PVP_ARENA,
            SCENARIO_PVP_OPENWORLD
        };

        enum CombatStyle
        {
            COMBAT_MELEE                = 0x01,             // class melee attacker
            COMBAT_RANGED               = 0x02              // class is ranged attacker
        };

        // masters orders that should be obeyed by the AI during the updateAI routine
        // the master will auto set the target of the bot
        enum CombatOrderType
        {
            ORDERS_NONE                 = 0x0000,   // no special orders given
            ORDERS_TANK                 = 0x0001,   // bind attackers by gaining threat
            ORDERS_ASSIST               = 0x0002,   // assist someone (dps type)
            ORDERS_HEAL                 = 0x0004,   // concentrate on healing (no attacks, only self defense)
            ORDERS_NODISPEL             = 0x0008,   // Dont dispel anything
            ORDERS_PROTECT              = 0x0010,   // combinable state: check if protectee is attacked
            ORDERS_PASSIVE              = 0x0020,   // bots do nothing
            ORDERS_TEMP_WAIT_TANKAGGRO  = 0x0040,   // Wait on tank to build aggro - expect healing to continue, disable setting when tank loses focus
            ORDERS_TEMP_WAIT_OOC        = 0x0080,   // Wait but only while OOC - wait only - combat will resume healing, dps, tanking, ...
            ORDERS_RESIST_FIRE          = 0x0100,   // resist fire
            ORDERS_RESIST_NATURE        = 0x0200,   // resist nature
            ORDERS_RESIST_FROST         = 0x0400,   // resist frost
            ORDERS_RESIST_SHADOW        = 0x0800,   // resist shadow
            ORDERS_MAIN_TANK            = 0x1000,   // main attackers binder by gaining threat in raid situation
            ORDERS_MAIN_HEAL            = 0x2000,   // concentrate on healing the main tank (will ignore other targets as long as MT needs healing)
            ORDERS_NOT_MAIN_HEAL        = 0x4000,   // concentrate on healing except the main tank that will be ignored

            // Cumulative orders
            ORDERS_PRIMARY              = 0x7007,
            ORDERS_SECONDARY            = 0x0F78,
            ORDERS_RESIST               = 0x0F00,
            ORDERS_TEMP                 = 0x00C0,   // All orders NOT to be saved, turned off by bots (or logoff, reset, ...)
            ORDERS_RESET                = 0xFFFF
        };

        enum CombatTargetType
        {
            TARGET_NORMAL               = 0x00,
            TARGET_THREATEN             = 0x01
        };

        enum BotState
        {
            BOTSTATE_LOADING,           // loading state during world load
            BOTSTATE_NORMAL,            // normal AI routines are processed
            BOTSTATE_COMBAT,            // bot is in combat
            BOTSTATE_DEAD,              // we are dead and wait for becoming ghost
            BOTSTATE_DEADRELEASED,      // we released as ghost and wait to revive
            BOTSTATE_LOOTING,           // looting mode, used just after combat
            BOTSTATE_FLYING,            // bot is flying
            BOTSTATE_TAME,              // bot hunter taming
            BOTSTATE_DELAYED            // bot delay action
        };

        enum CollectionFlags
        {
            COLLECT_FLAG_NOTHING    = 0x00,     // skip looting of anything
            COLLECT_FLAG_COMBAT     = 0x01,     // loot after combat
            COLLECT_FLAG_QUEST      = 0x02,     // quest and needed items
            COLLECT_FLAG_PROFESSION = 0x04,     // items related to skills
            COLLECT_FLAG_LOOT       = 0x08,     // all loot on corpses
            COLLECT_FLAG_SKIN       = 0x10,     // skin creatures if available
            COLLECT_FLAG_NEAROBJECT = 0x20      // collect specified nearby object
        };

        enum MovementOrderType
        {
            MOVEMENT_NONE               = 0x00,
            MOVEMENT_FOLLOW             = 0x01,
            MOVEMENT_STAY               = 0x02
        };

        enum TaskFlags
        {
            NONE                        = 0x00,  // do nothing
            SELL_ITEMS                  = 0x01,  // sell items
            BUY_ITEMS                   = 0x02,  // buy items
            REPAIR_ITEMS                = 0x03,  // repair items
            ADD_AUCTION                 = 0x04,  // add auction
            REMOVE_AUCTION              = 0x05,  // remove auction
            RESET_TALENTS               = 0x06,  // reset all talents
            BANK_WITHDRAW               = 0x07,  // withdraw item from bank
            BANK_DEPOSIT                = 0x08,  // deposit item in bank
            LIST_QUEST                  = 0x09,  // list quests
            END_QUEST                   = 0x0A,  // turn in quests
            TAKE_QUEST                  = 0x0B   // take quest
        };

        enum AnnounceFlags
        {
            NOTHING                     = 0x00,
            INVENTORY_FULL              = 0x01,
            CANT_AFFORD                 = 0x02,
            CANT_USE_TOO_FAR            = 0x03
        };

        typedef std::pair<enum TaskFlags, uint32> taskPair;
        typedef std::list<taskPair> BotTaskList;
        typedef std::list<enum NPCFlags> BotNPCList;
        typedef std::map<uint32, uint32> BotNeedItem;
        typedef std::pair<uint32, uint32> talentPair;
        typedef std::list<ObjectGuid> BotObjectList;
        typedef std::list<uint32> BotEntryList;
        typedef std::vector<uint32> BotTaxiNode;
        typedef std::set<ObjectGuid> BotObjectSet;

        // attacker query used in PlayerbotAI::FindAttacker()
        enum ATTACKERINFOTYPE
        {
            AIT_NONE                    = 0x00,
            AIT_LOWESTTHREAT            = 0x01,
            AIT_HIGHESTTHREAT           = 0x02,
            AIT_VICTIMSELF              = 0x04,
            AIT_VICTIMNOTSELF           = 0x08      // could/should use victim param in FindAttackers
        };
        struct AttackerInfo
        {
            Unit*    attacker;            // reference to the attacker
            Unit*    victim;              // combatant's current victim
            float threat;                 // own threat on this combatant
            float threat2;                // highest threat not caused by bot
            uint32 count;                 // number of units attacking
            uint32 source;                // 1=bot, 2=master, 3=group
        };
        typedef std::map<ObjectGuid, AttackerInfo> AttackerInfoList;
        typedef std::map<uint32, float> SpellRanges;

        enum HELPERLINKABLES
        {
            HL_NONE,
            HL_PROFESSION,
            HL_ITEM,
            HL_TALENT,
            HL_SKILL,
            HL_OPTION,
            HL_PETAGGRO,
            HL_QUEST,
            HL_GAMEOBJECT,
            HL_SPELL,
            HL_TARGET,
            HL_NAME,
            HL_AUCTION,
            HL_MAIL,
            HL_RECIPE
        };

    public:
        PlayerbotAI(PlayerbotMgr& mgr, Player* const bot, bool debugWhisper);
        virtual ~PlayerbotAI();

        // This is called from Unit.cpp and is called every second (I think)
        void UpdateAI(const uint32 p_time);

        // This is called from ChatHandler.cpp when there is an incoming message to the bot
        // from a whisper or from the party channel
        void HandleCommand(const std::string& text, Player& fromPlayer);

        // This is called by WorldSession.cpp
        // It provides a view of packets normally sent to the client.
        // Since there is no client at the other end, the packets are dropped of course.
        // For a list of opcodes that can be caught see Opcodes.cpp (SMSG_* opcodes only)
        void HandleBotOutgoingPacket(const WorldPacket& packet);

        // Returns what kind of situation we are in so the ai can react accordingly
        ScenarioType GetScenarioType() { return m_ScenarioType; }
        CombatStyle GetCombatStyle() { return m_combatStyle; }
        void SetCombatStyle(CombatStyle cs) { m_combatStyle = cs; }

        PlayerbotClassAI* GetClassAI() { return m_classAI; }
        void ReloadAI();

        // finds spell ID for matching substring args
        // in priority of full text match, spells not taking reagents, and highest rank
        uint32 getSpellId(const char* args, bool master = false) const;
        uint32 getPetSpellId(const char* args) const;
        // Initialize spell using rank 1 spell id
        uint32 initSpell(uint32 spellId);
        uint32 initPetSpell(uint32 spellIconId);

        // extract mail ids from links
        void extractMailIds(const std::string& text, std::list<uint32>& mailIds) const;

        // extract quest ids from links
        void extractQuestIds(const std::string& text, std::list<uint32>& questIds) const;

        // extract auction ids from links
        void extractAuctionIds(const std::string& text, std::list<uint32>& auctionIds) const;

        // extracts talent ids to list
        void extractTalentIds(const std::string& text, std::list<talentPair>& talentIds) const;

        // extracts item ids from links
        void extractItemIds(const std::string& text, std::list<uint32>& itemIds) const;

        // extract spellid from links
        void extractSpellId(const std::string& text, uint32& spellId) const;

        // extract spellids from links to list
        void extractSpellIdList(const std::string& text, BotEntryList& m_spellsToLearn) const;

        // extracts currency from a string as #g#s#c and returns the total in copper
        uint32 extractMoney(const std::string& text) const;

        // extracts gameobject info from link
        void extractGOinfo(const std::string& text, BotObjectList& m_lootTargets) const;

        // finds items in bots equipment and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
        // finds items in bots inventory and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
        // finds nearby game objects that are specified in m_collectObjects then adds them to the m_lootTargets list
        void findNearbyGO();
        // finds nearby creatures, whose UNIT_NPC_FLAGS match the flags specified in item list m_itemIds
        void findNearbyCreature();
        // finds nearby corpse that is lootable
        void findNearbyCorpse();

        void GiveLevel(uint32 level);

        // Error check the TS DB. Should only be used when admins want to verify their new TS input
        uint32 TalentSpecDBContainsError();

        // Get talent specs or counts thereof
        uint32 GetTalentSpecsAmount();
        uint32 GetTalentSpecsAmount(long specClass);
        std::list<TalentSpec> GetTalentSpecs(long specClass);
        TalentSpec GetTalentSpec(long specClass, long choice);
        TalentSpec GetActiveTalentSpec() { return m_activeTalentSpec; }
        void ClearActiveTalentSpec() { m_activeTalentSpec.specName = ""; m_activeTalentSpec.specClass = 0; m_activeTalentSpec.specPurpose = TSP_NONE; for (int i = 0; i < 71; i++) m_activeTalentSpec.talentId[i] = 0; for (int i = 0; i < 3; i++) { m_activeTalentSpec.glyphIdMajor[i] = 0; m_activeTalentSpec.glyphIdMinor[i] = 0; } }
        void SetActiveTalentSpec(TalentSpec ts) { m_activeTalentSpec = ts; }
        bool ApplyActiveTalentSpec();
        bool IsElite(Unit* target, bool isWorldBoss = false) const;
        // Used by bots to check if their target is neutralized (polymorph, shackle or the like). Useful to avoid breaking crowd control
        static bool IsNeutralized(Unit* target);
        // Make the bots face their target
        void FaceTarget(Unit* target);
        // Used by bot to check if target is immune to a specific damage school before using an ability
        static bool IsImmuneToSchool(Unit* pTarget, SpellSchoolMask SchoolMask);

        void MakeSpellLink(const SpellEntry* sInfo, std::ostringstream& out);
        void MakeWeaponSkillLink(const SpellEntry* sInfo, std::ostringstream& out, uint32 skillid);

        // currently bots only obey commands from the master
        bool canObeyCommandFrom(const Player& player) const;

        // get current casting spell (will return NULL if no spell!)
        Spell* GetCurrentSpell() const;
        uint32 GetCurrentSpellId() { return m_CurrentlyCastingSpellId; }

        bool HasAura(uint32 spellId, const Unit& player) const;
        bool HasAura(const char* spellName, const Unit& player) const;
        bool HasAura(const char* spellName) const;

        bool CanReceiveSpecificSpell(uint8 spec, Unit* target) const;

        bool PickPocket(Unit* pTarget);
        bool HasTool(uint32 TC);        // TODO implement this for opening lock
        bool HasSpellReagents(uint32 spellId);
        void ItemCountInInv(uint32 itemid, uint32& count);
        uint32 GetSpellCharges(uint32 spellId);

        uint8 GetHealthPercent(const Unit& target) const;
        uint8 GetHealthPercent() const;
        uint8 GetManaPercent(const Unit& target) const;
        uint8 GetManaPercent() const;
        uint8 GetRageAmount(const Unit& target) const;
        uint8 GetRageAmount() const;
        uint8 GetEnergyAmount(const Unit& target) const;
        uint8 GetEnergyAmount() const;
        uint8 GetRunicPower(const Unit& target) const;
        uint8 GetRunicPower() const;

        Item* FindFood() const;
        Item* FindDrink() const;
        Item* FindBandage() const;
        Item* FindPoison() const;
        Item* FindMount(uint32 matchingRidingSkill) const;
        Item* FindItem(uint32 ItemId, bool Equipped_too = false);
        Item* FindItemInBank(uint32 ItemId);
        Item* FindKeyForLockValue(uint32 reqSkillValue);
        Item* FindBombForLockValue(uint32 reqSkillValue);
        Item* FindConsumable(uint32 displayId) const;
        Item* FindManaRegenItem() const;
        bool  FindAmmo() const;
        uint8 _findItemSlot(Item* target);
        bool CanStore();

        // ******* Actions ****************************************
        // Your handlers can call these actions to make the bot do things.
        void TellMaster(const std::string& text) const;
        void TellMaster(const char* fmt, ...) const;
        void SendWhisper(const std::string& text, Player& player) const;
        SpellCastResult CastSpell(const char* args);
        SpellCastResult CastSpell(uint32 spellId);
        SpellCastResult CastSpell(uint32 spellId, Unit& target);
        SpellCastResult CheckBotCast(const SpellEntry* sInfo);
        SpellCastResult CastPetSpell(uint32 spellId, Unit* target = nullptr);
        SpellCastResult Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player*) = nullptr);
        SpellCastResult SelfBuff(uint32 spellId);
        bool In_Range(Unit* Target, uint32 spellId);
        bool In_Reach(Unit* Target, uint32 spellId);
        bool CanReachWithSpellAttack(Unit* target);

        void UseItem(Item* item, uint32 targetFlag, ObjectGuid targetGUID);
        void UseItem(Item* item, uint8 targetInventorySlot);
        void UseItem(Item* item, Unit* target);
        void UseItem(Item* item);

        void TryEmergency(Unit* pAttacker);

        void PlaySound(uint32 soundid);
        void Announce(AnnounceFlags msg);

        void EquipItem(Item* src_Item);
        //void Stay();
        //bool Follow(Player& player);
        void SendNotEquipList(Player& player);

        uint8  m_DelayAttack;
        time_t m_DelayAttackInit;
        Unit* gPrimtarget;
        Unit* gSectarget;
        uint32 gQuestFetch;

        bool m_AutoEquipToggle;             //switch for autoequip
        uint32 SellWhite;                   //switch for white item auto sell
        uint8 DistOverRide;
        float gDist[2]; //gDist, gTemp vars are used for variable follow distance
        float gTempDist;
        float gTempDist2;
        uint8 m_FollowAutoGo;
        uint8 IsUpOrDown; //tracks variable follow distance
        void BotDataRestore();
        void CombatOrderRestore();
        void AutoUpgradeEquipment();
        void FollowAutoReset();
        void AutoEquipComparison(Item* pItem, Item* pItem2);
        bool ItemStatComparison(const ItemPrototype* pProto, const ItemPrototype* pProto2);
        void InterruptCurrentCastingSpell();
        void Attack(Unit* forcedTarget = nullptr);
        void GetCombatTarget(Unit* forcedTarget = 0);
        void GetDuelTarget(Unit* forcedTarget);
        Unit* GetCurrentTarget() { return m_targetCombat; };
        void DoNextCombatManeuver();
        void DoCombatMovement();
        void SetIgnoreUpdateTime(uint8 t = 0) { m_ignoreAIUpdatesUntilTime = time(nullptr) + t; };
        time_t CurrentTime() { return time(nullptr); };

        Player* GetPlayerBot() const { return m_bot; }
        Player* GetPlayer() const { return m_bot; }
        Player* GetMaster() const;

        BotState GetState() { return m_botState; };
        void SetState(BotState state);
        void SetQuestNeedItems();
        void SetQuestNeedCreatures();
        void SendQuestNeedList();
        bool IsInQuestItemList(uint32 itemid) { return m_needItemList.find(itemid) != m_needItemList.end(); };
        bool IsInQuestCreatureList(uint32 id) { return m_needCreatureOrGOList.find(id) != m_needCreatureOrGOList.end(); };
        bool IsItemUseful(uint32 itemid);
        void SendOrders(Player& player);
        bool DoTeleport(WorldObject& obj);
        void DoLoot();
        void DoFlight();
        void GetTaxi(ObjectGuid guid, BotTaxiNode& nodes);
        void BeingRolledOn(ObjectGuid target) { m_being_rolled_on.push_back(target); };

        bool HasCollectFlag(uint8 flag) { return (m_collectionFlags & flag) ? true : false; }
        void SetCollectFlag(uint8 flag)
        {
            if (HasCollectFlag(flag)) m_collectionFlags &= ~flag;
            else m_collectionFlags |= flag;
        }

        uint32 EstRepairAll();
        uint32 EstRepair(uint16 pos);

        void AcceptQuest(Quest const* qInfo, Player* pGiver);
        void TurnInQuests(WorldObject* questgiver);
        void ListQuests(WorldObject* questgiver);
        bool AddQuest(const uint32 entry, WorldObject* questgiver);
        void MakeQuestLink(Quest const* quest, std::ostringstream& out);

        bool IsInCombat();
        bool IsRegenerating();
        bool IsGroupReady();
        Player* GetGroupTank(); // TODO: didn't want to pollute non-playerbot code but this should really go in group.cpp
        Player* GetGroupHealer(); // TODO: didn't want to pollute non-playerbot code but this should really go in group.cpp
        void SetGroupCombatOrder(CombatOrderType co);
        void ClearGroupCombatOrder(CombatOrderType co);
        void SetGroupIgnoreUpdateTime(uint8 t);
        bool GroupHoTOnTank();
        bool CanPull(Player& fromPlayer);
        bool CastPull();
        bool GroupTankHoldsAggro();
        bool CastNeutralize();
        void UpdateAttackerInfo();
        Unit* FindAttacker(ATTACKERINFOTYPE ait = AIT_NONE, Unit* victim = 0);
        uint32 GetAttackerCount() { return m_attackerInfo.size(); };
        void SetCombatOrderByStr(std::string str, Unit* target = 0);
        void SetCombatOrder(CombatOrderType co, Unit* target = 0);
        void ClearCombatOrder(CombatOrderType co);
        CombatOrderType GetCombatOrder() { return this->m_combatOrder; }
        bool IsMainTank() { return (m_combatOrder & ORDERS_MAIN_TANK) ? true : false; }
        bool IsTank() { return (m_combatOrder & ORDERS_TANK) || IsMainTank() ? true : false; }
        bool IsMainHealer() { return (m_combatOrder & ORDERS_MAIN_HEAL) ? true : false; }
        bool IsHealer() { return (m_combatOrder & (ORDERS_HEAL | ORDERS_NOT_MAIN_HEAL)) || IsMainHealer() ? true : false; }
        bool HasDispelOrder() { return !(m_combatOrder & ORDERS_NODISPEL); }
        bool IsDPS() { return (m_combatOrder & ORDERS_ASSIST) ? true : false; }
        bool Impulse() { srand(time(nullptr)); return (((rand() % 100) > 50) ? true : false); }
        void SetMovementOrder(MovementOrderType mo, Unit* followTarget = 0);
        MovementOrderType GetMovementOrder() { return this->m_movementOrder; }
        void MovementReset();
        void MovementClear();

        void SetInFront(const Unit* obj);

        void ItemLocalization(std::string& itemName, const uint32 itemID) const;
        void QuestLocalization(std::string& questTitle, const uint32 questID) const;
        void CreatureLocalization(std::string& creatureName, const uint32 entry) const;
        void GameObjectLocalization(std::string& gameobjectName, const uint32 entry) const;

        uint32 GetFreeBagSpace() const;
        bool DropGarbage(bool bVerbose);
        void SellGarbage(Player& player, bool listNonTrash = true, bool bDetailTrashSold = false, bool verbose = true);
        void Sell(const uint32 itemid);
        void Buy(Creature* vendor, const uint32 itemid);
        std::string DropItem(const uint32 itemid);
        void AddAuction(const uint32 itemid, Creature* aCreature);
        void ListAuctions();
        bool RemoveAuction(const uint32 auctionid);
        void Repair(const uint32 itemid, Creature* rCreature);
        bool Talent(Creature* tCreature);
        void InspectUpdate();
        bool Withdraw(const uint32 itemid);
        bool Deposit(const uint32 itemid);
        void BankBalance();
        std::string Cash(uint32 copper);
        std::string AuctionResult(std::string subject, std::string body);

    protected:
        bool ValidateTalent(uint16 talent, long charClass);
        bool ValidateGlyph(uint16 glyph, long charClass);
        bool ValidateMajorGlyph(uint16 glyph, long charClass);
        bool ValidateMinorGlyph(uint16 glyph, long charClass);

    private:
        bool ExtractCommand(const std::string sLookingFor, std::string& text, bool bUseShort = false);
        // outsource commands for code clarity
        void _HandleCommandReset(std::string& text, Player& fromPlayer);
        void _HandleCommandOrders(std::string& text, Player& fromPlayer);
        void _HandleCommandFollow(std::string& text, Player& fromPlayer);
        void _HandleCommandStay(std::string& text, Player& fromPlayer);
        void _HandleCommandAttack(std::string& text, Player& fromPlayer);
        void _HandleCommandPull(std::string& text, Player& fromPlayer);
        void _HandleCommandNeutralize(std::string& text, Player& fromPlayer);
        void _HandleCommandCast(std::string& text, Player& fromPlayer);
        void _HandleCommandSell(std::string& text, Player& fromPlayer);
        void _HandleCommandBuy(std::string& text, Player& fromPlayer);
        void _HandleCommandDrop(std::string& text, Player& fromPlayer);
        void _HandleCommandRepair(std::string& text, Player& fromPlayer);
        void _HandleCommandAuction(std::string& text, Player& fromPlayer);
        void _HandleCommandMail(std::string& text, Player& fromPlayer);
        void _HandleCommandBank(std::string& text, Player& fromPlayer);
        void _HandleCommandTalent(std::string& text, Player& fromPlayer);
        void _HandleCommandUse(std::string& text, Player& fromPlayer);
        void _HandleCommandEquip(std::string& text, Player& fromPlayer);
        void _HandleCommandFind(std::string& text, Player& fromPlayer);
        void _HandleCommandGet(std::string& text, Player& fromPlayer);
        void _HandleCommandCollect(std::string& text, Player& fromPlayer);
        void _HandleCommandQuest(std::string& text, Player& fromPlayer);
        void _HandleCommandCraft(std::string& text, Player& fromPlayer);
        void _HandleCommandEnchant(std::string& text, Player& fromPlayer);
        void _HandleCommandProcess(std::string& text, Player& fromPlayer);
        void _HandleCommandPet(std::string& text, Player& fromPlayer);
        void _HandleCommandSpells(std::string& text, Player& fromPlayer);
        void _HandleCommandSurvey(std::string& text, Player& fromPlayer);
        void _HandleCommandSkill(std::string& text, Player& fromPlayer);
        bool _HandleCommandSkillLearnHelper(TrainerSpell const* tSpell, uint32 spellId, uint32 cost);
        void _HandleCommandStats(std::string& text, Player& fromPlayer);
        void _HandleCommandHelp(std::string& text, Player& fromPlayer);
        void _HandleCommandHelp(const char* szText, Player& fromPlayer) { std::string text = szText; _HandleCommandHelp(text, fromPlayer); }
        void _HandleCommandGM(std::string& text, Player& fromPlayer);
        std::string _HandleCommandHelpHelper(std::string sCommand, std::string sExplain, HELPERLINKABLES reqLink = HL_NONE, bool bReqLinkMultiples = false, bool bCommandShort = false);

        // ****** Closed Actions ********************************
        // These actions may only be called at special times.
        // Trade methods are only applicable when the trade window is open
        // and are only called from within HandleCommand.
        bool TradeItem(const Item& item, int8 slot = -1);
        bool TradeCopper(uint32 copper);

        // Helper routines not needed by class AIs.
        void UpdateAttackersForTarget(Unit* victim);

        void _doSellItem(Item* const item, std::ostringstream& report, std::ostringstream& canSell, uint32& TotalCost, uint32& TotalSold);
        void MakeItemLink(const Item* item, std::ostringstream& out, bool IncludeQuantity = true);
        void MakeItemText(const Item* item, std::ostringstream& out, bool IncludeQuantity = true);
        void MakeItemLink(const ItemPrototype* item, std::ostringstream& out);

        // it is safe to keep these back reference pointers because m_bot
        // owns the "this" object and m_master owns m_bot. The owner always cleans up.
        PlayerbotMgr& m_mgr;
        Player* const m_bot;
        PlayerbotClassAI* m_classAI;

        // ignores AI updates until time specified
        // no need to waste CPU cycles during casting etc
        time_t m_ignoreAIUpdatesUntilTime;

        CombatStyle m_combatStyle;
        CombatOrderType m_combatOrder;
        MovementOrderType m_movementOrder;

        TalentSpec m_activeTalentSpec;

        ScenarioType m_ScenarioType;

        // defines the state of behaviour of the bot
        BotState m_botState;

        // list of items, creatures or gameobjects needed to fullfill quests
        BotNeedItem m_needItemList;
        BotNeedItem m_needCreatureOrGOList;

        // list of creatures we recently attacked and want to loot
        BotNPCList m_findNPC;               // list of NPCs
        BotTaskList m_tasks;                // list of tasks
        BotObjectList m_lootTargets;        // list of targets
        BotEntryList m_spellsToLearn;       // list of spells
        ObjectGuid m_lootCurrent;           // current remains of interest
        ObjectGuid m_lootPrev;              // previous loot
        BotEntryList m_collectObjects;      // object entries searched for in findNearbyGO
        BotTaxiNode m_taxiNodes;            // flight node chain
        BotEntryList m_noToolList;          // list of required tools
        BotObjectList m_being_rolled_on;    // list of targets currently involved in item rolls

        uint8 m_collectionFlags;            // what the bot should look for to loot
        uint32 m_collectDist;               // distance to collect objects
        bool m_inventory_full;
        uint32 m_itemTarget;
        bool m_dropWhite;

        uint32 m_CurrentlyCastingSpellId;
        uint32 m_CraftSpellId;
        //bool m_IsFollowingMaster;

        // if master commands bot to do something, store here until updateAI
        // can do it
        uint32 m_spellIdCommand;
        ObjectGuid m_targetGuidCommand;
        ObjectGuid m_taxiMaster;

        BotObjectSet m_ignorePlayersChat;  // list of players that the bot will not respond to

        AttackerInfoList m_attackerInfo;

        // Force bot to pick a neutralised unit as combat target when told so
        bool m_ignoreNeutralizeEffect;
        bool m_targetChanged;
        CombatTargetType m_targetType;

        Unit* m_targetCombat;       // current combat target
        Unit* m_targetAssist;       // get new target by checking attacker list of assisted player
        Unit* m_targetProtect;      // check

        Unit* m_followTarget;       // whom to follow in non combat situation?

        uint32 FISHING,
               HERB_GATHERING,
               MINING,
               SKINNING,
               ASPECT_OF_THE_MONKEY;

        SpellRanges m_spellRangeMap;

        float m_destX, m_destY, m_destZ; // latest coordinates for chase and point movement types

        bool m_bDebugCommandChat;

        bool m_debugWhisper = false;
};

#endif
