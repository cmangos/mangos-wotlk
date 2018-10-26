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

#include "Spells/SpellTargetDefines.h"

SpellTargetInfo::SpellTargetInfo(char const* name, SpellTargetImplicitType type, SpellTargetFilter filter, SpellTargetEnumerator enumerator) :
    name(name), type(type), filter(filter), enumerator(enumerator)
{
}

SpellTargetInfo SpellTargetInfoTable[MAX_SPELL_TARGETS] =
{
    /*[0]*/     { "TARGET_NONE",                                          TARGET_TYPE_UNKNOWN                                                 },
    /*[1]*/     { "TARGET_UNIT_CASTER",                                   TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[2]*/     { "TARGET_UNIT_ENEMY_NEAR_CASTER",                        TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_CHAIN  },
    /*[3]*/     { "TARGET_UNIT_FRIEND_NEAR_CASTER",                       TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_CHAIN  },
    /*[4]*/     { "TARGET_UNIT_NEAR_CASTER",                              TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_CHAIN  },
    /*[5]*/     { "TARGET_UNIT_CASTER_PET",                               TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[6]*/     { "TARGET_UNIT_ENEMY",                                    TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_CHAIN  },
    /*[7]*/     { "TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC",              TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_AOE    },
    /*[8]*/     { "TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DEST_LOC",             TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_AOE    },
    /*[9]*/     { "TARGET_LOCATION_CASTER_HOME_BIND",                     TARGET_TYPE_LOCATION                                                },
    /*[10]*/    { "TARGET_LOCATION_CASTER_DIVINE_BIND_NYI",               TARGET_TYPE_LOCATION                                                },
    /*[11]*/    { "TARGET_PLAYER_NYI",                                    TARGET_TYPE_PLAYER,     TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[12]*/    { "TARGET_PLAYER_NEAR_CASTER_NYI",                        TARGET_TYPE_PLAYER,     TARGET_NEUTRAL,    TARGET_ENUMERATOR_CHAIN  },
    /*[13]*/    { "TARGET_PLAYER_ENEMY_NYI",                              TARGET_TYPE_PLAYER,     TARGET_HARMFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[14]*/    { "TARGET_PLAYER_FRIEND_NYI",                             TARGET_TYPE_PLAYER,     TARGET_HELPFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[15]*/    { "TARGET_ENUM_UNITS_ENEMY_AOE_AT_SRC_LOC",               TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_AOE    },
    /*[16]*/    { "TARGET_ENUM_UNITS_ENEMY_AOE_AT_DEST_LOC",              TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_AOE    },
    /*[17]*/    { "TARGET_LOCATION_DATABASE",                             TARGET_TYPE_LOCATION,   TARGET_SCRIPT                               },
    /*[18]*/    { "TARGET_LOCATION_CASTER_DEST",                          TARGET_TYPE_LOCATION                                                },
    /*[19]*/    { "TARGET_UNK_19",                                        TARGET_TYPE_UNKNOWN                                                 },
    /*[20]*/    { "TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE",          TARGET_TYPE_UNIT,       TARGET_PARTY,      TARGET_ENUMERATOR_AOE    },
    /*[21]*/    { "TARGET_UNIT_FRIEND",                                   TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[22]*/    { "TARGET_LOCATION_CASTER_SRC",                           TARGET_TYPE_LOCATION                                                },
    /*[23]*/    { "TARGET_GAMEOBJECT",                                    TARGET_TYPE_GAMEOBJECT, TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[24]*/    { "TARGET_ENUM_UNITS_ENEMY_IN_CONE_24",                   TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_CONE   },
    /*[25]*/    { "TARGET_UNIT",                                          TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[26]*/    { "TARGET_LOCKED",                                        TARGET_TYPE_LOCK,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[27]*/    { "TARGET_UNIT_CASTER_MASTER",                            TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_SINGLE },
    /*[28]*/    { "TARGET_ENUM_UNITS_ENEMY_AOE_AT_DYNOBJ_LOC",            TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_AOE    },
    /*[29]*/    { "TARGET_ENUM_UNITS_FRIEND_AOE_AT_DYNOBJ_LOC",           TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_AOE    },
    /*[30]*/    { "TARGET_ENUM_UNITS_FRIEND_AOE_AT_SRC_LOC",              TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_AOE    },
    /*[31]*/    { "TARGET_ENUM_UNITS_FRIEND_AOE_AT_DEST_LOC",             TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_AOE    },
    /*[32]*/    { "TARGET_LOCATION_UNIT_MINION_POSITION",                 TARGET_TYPE_LOCATION                                                },
    /*[33]*/    { "TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC",               TARGET_TYPE_UNIT,       TARGET_PARTY,      TARGET_ENUMERATOR_AOE    },
    /*[34]*/    { "TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC",              TARGET_TYPE_UNIT,       TARGET_PARTY,      TARGET_ENUMERATOR_AOE    },
    /*[35]*/    { "TARGET_UNIT_PARTY",                                    TARGET_TYPE_UNIT,       TARGET_PARTY,      TARGET_ENUMERATOR_SINGLE },
    /*[36]*/    { "TARGET_ENUM_UNITS_ENEMY_WITHIN_CASTER_RANGE",          TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_AOE    },
    /*[37]*/    { "TARGET_UNIT_FRIEND_AND_PARTY",                         TARGET_TYPE_UNIT,       TARGET_PARTY,      TARGET_ENUMERATOR_CHAIN  },
    /*[38]*/    { "TARGET_UNIT_SCRIPT_NEAR_CASTER",                       TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_CHAIN  },
    /*[39]*/    { "TARGET_LOCATION_CASTER_FISHING_SPOT",                  TARGET_TYPE_LOCATION                                                },
    /*[40]*/    { "TARGET_GAMEOBJECT_SCRIPT_NEAR_CASTER",                 TARGET_TYPE_GAMEOBJECT, TARGET_SCRIPT,     TARGET_ENUMERATOR_CHAIN  },
    /*[41]*/    { "TARGET_LOCATION_CASTER_FRONT_RIGHT",                   TARGET_TYPE_LOCATION                                                },
    /*[42]*/    { "TARGET_LOCATION_CASTER_BACK_RIGHT",                    TARGET_TYPE_LOCATION                                                },
    /*[43]*/    { "TARGET_LOCATION_CASTER_BACK_LEFT",                     TARGET_TYPE_LOCATION                                                },
    /*[44]*/    { "TARGET_LOCATION_CASTER_FRONT_LEFT",                    TARGET_TYPE_LOCATION                                                },
    /*[45]*/    { "TARGET_UNIT_FRIEND_CHAIN_HEAL",                        TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_CHAIN  },
    /*[46]*/    { "TARGET_LOCATION_SCRIPT_NEAR_CASTER",                   TARGET_TYPE_LOCATION,   TARGET_SCRIPT                               },
    /*[47]*/    { "TARGET_LOCATION_CASTER_FRONT",                         TARGET_TYPE_LOCATION                                                },
    /*[48]*/    { "TARGET_LOCATION_CASTER_BACK",                          TARGET_TYPE_LOCATION                                                },
    /*[49]*/    { "TARGET_LOCATION_CASTER_LEFT",                          TARGET_TYPE_LOCATION                                                },
    /*[50]*/    { "TARGET_LOCATION_CASTER_RIGHT",                         TARGET_TYPE_LOCATION                                                },
    /*[51]*/    { "TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_SRC_LOC",        TARGET_TYPE_GAMEOBJECT, TARGET_SCRIPT,     TARGET_ENUMERATOR_AOE    },
    /*[52]*/    { "TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC",       TARGET_TYPE_GAMEOBJECT, TARGET_SCRIPT,     TARGET_ENUMERATOR_AOE    },
    /*[53]*/    { "TARGET_LOCATION_CASTER_TARGET_POSITION",               TARGET_TYPE_LOCATION                                                },
    /*[54]*/    { "TARGET_ENUM_UNITS_ENEMY_IN_CONE_54",                   TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_CONE   },
    /*[55]*/    { "TARGET_LOCATION_CASTER_FRONT_LEAP",                    TARGET_TYPE_LOCATION                                                },
    /*[56]*/    { "TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE",           TARGET_TYPE_UNIT,       TARGET_GROUP,      TARGET_ENUMERATOR_AOE    },
    /*[57]*/    { "TARGET_UNIT_RAID",                                     TARGET_TYPE_UNIT,       TARGET_GROUP,      TARGET_ENUMERATOR_SINGLE },
    /*[58]*/    { "TARGET_UNIT_RAID_NEAR_CASTER",                         TARGET_TYPE_UNIT,       TARGET_GROUP,      TARGET_ENUMERATOR_CHAIN  },
    /*[59]*/    { "TARGET_ENUM_UNITS_FRIEND_IN_CONE",                     TARGET_TYPE_UNIT,       TARGET_HELPFUL,    TARGET_ENUMERATOR_CONE   },
    /*[60]*/    { "TARGET_ENUM_UNITS_SCRIPT_IN_CONE_60",                  TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_CONE   },
    /*[61]*/    { "TARGET_UNIT_RAID_AND_CLASS",                           TARGET_TYPE_UNIT,       TARGET_GROUP,      TARGET_ENUMERATOR_CHAIN  },
    /*[62]*/    { "TARGET_PLAYER_RAID_NYI",                               TARGET_TYPE_PLAYER,     TARGET_GROUP,      TARGET_ENUMERATOR_SINGLE },
    /*[63]*/    { "TARGET_LOCATION_UNIT_POSITION",                        TARGET_TYPE_LOCATION                                                },
    /*[64]*/    { "TARGET_LOCATION_UNIT_FRONT",                           TARGET_TYPE_LOCATION                                                },
    /*[65]*/    { "TARGET_LOCATION_UNIT_BACK",                            TARGET_TYPE_LOCATION                                                },
    /*[66]*/    { "TARGET_LOCATION_UNIT_RIGHT",                           TARGET_TYPE_LOCATION                                                },
    /*[67]*/    { "TARGET_LOCATION_UNIT_LEFT",                            TARGET_TYPE_LOCATION                                                },
    /*[68]*/    { "TARGET_LOCATION_UNIT_FRONT_RIGHT",                     TARGET_TYPE_LOCATION                                                },
    /*[69]*/    { "TARGET_LOCATION_UNIT_BACK_RIGHT",                      TARGET_TYPE_LOCATION                                                },
    /*[70]*/    { "TARGET_LOCATION_UNIT_BACK_LEFT",                       TARGET_TYPE_LOCATION                                                },
    /*[71]*/    { "TARGET_LOCATION_UNIT_FRONT_LEFT",                      TARGET_TYPE_LOCATION                                                },
    /*[72]*/    { "TARGET_LOCATION_CASTER_RANDOM_SIDE",                   TARGET_TYPE_LOCATION                                                },
    /*[73]*/    { "TARGET_LOCATION_CASTER_RANDOM_CIRCUMFERENCE",          TARGET_TYPE_LOCATION                                                },
    /*[74]*/    { "TARGET_LOCATION_UNIT_RANDOM_SIDE",                     TARGET_TYPE_LOCATION                                                },
    /*[75]*/    { "TARGET_LOCATION_UNIT_RANDOM_CIRCUMFERENCE",            TARGET_TYPE_LOCATION                                                },
    /*[76]*/    { "TARGET_LOCATION_DYNOBJ_POSITION",                      TARGET_TYPE_LOCATION                                                },
    /*[77]*/    { "TARGET_UNIT_CHANNEL_TARGET",                           TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[78]*/    { "TARGET_LOCATION_NORTH",                                TARGET_TYPE_LOCATION                                                },
    /*[79]*/    { "TARGET_LOCATION_SOUTH",                                TARGET_TYPE_LOCATION                                                },
    /*[80]*/    { "TARGET_LOCATION_EAST",                                 TARGET_TYPE_LOCATION                                                },
    /*[81]*/    { "TARGET_LOCATION_WEST",                                 TARGET_TYPE_LOCATION                                                },
    /*[82]*/    { "TARGET_LOCATION_NE",                                   TARGET_TYPE_LOCATION                                                },
    /*[83]*/    { "TARGET_LOCATION_NW",                                   TARGET_TYPE_LOCATION                                                },
    /*[84]*/    { "TARGET_LOCATION_SE",                                   TARGET_TYPE_LOCATION                                                },
    /*[85]*/    { "TARGET_LOCATION_SW",                                   TARGET_TYPE_LOCATION                                                },
    /*[86]*/    { "TARGET_LOCATION_RANDOM_SIDE",                          TARGET_TYPE_LOCATION                                                },
    /*[87]*/    { "TARGET_LOCATION_CURRENT_REFERENCE",                    TARGET_TYPE_LOCATION                                                },
    /*[88]*/    { "TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DYNOBJ_LOC",           TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_AOE    },
    /*[89]*/    { "TARGET_LOCATION_TRAJECTORY",                           TARGET_TYPE_LOCATION                                                },
    /*[90]*/    { "TARGET_UNIT_CASTER_COMPANION",                         TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[91]*/    { "TARGET_LOCATION_RANDOM_CIRCUMFERENCE",                 TARGET_TYPE_LOCATION                                                },
    /*[92]*/    { "TARGET_UNIT_CASTER_SUMMONER",                          TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[93]*/    { "TARGET_CORPSE_ENEMY_NEAR_CASTER_NYI",                  TARGET_TYPE_CORPSE,     TARGET_HARMFUL,    TARGET_ENUMERATOR_CHAIN  },
    /*[94]*/    { "TARGET_UNIT_CASTER_VEHICLE",                           TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[95]*/    { "TARGET_UNIT_CASTER_DRIVER",                            TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[96]*/    { "TARGET_UNIT_CASTER_PASSENGER_0",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[97]*/    { "TARGET_UNIT_CASTER_PASSENGER_1",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[98]*/    { "TARGET_UNIT_CASTER_PASSENGER_2",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[99]*/    { "TARGET_UNIT_CASTER_PASSENGER_3",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[100]*/   { "TARGET_UNIT_CASTER_PASSENGER_4",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[101]*/   { "TARGET_UNIT_CASTER_PASSENGER_5",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[102]*/   { "TARGET_UNIT_CASTER_PASSENGER_6",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[103]*/   { "TARGET_UNIT_CASTER_PASSENGER_7",                       TARGET_TYPE_UNIT,       TARGET_NEUTRAL,    TARGET_ENUMERATOR_SINGLE },
    /*[104]*/   { "TARGET_ENUM_UNITS_ENEMY_IN_CONE_104",                  TARGET_TYPE_UNIT,       TARGET_HARMFUL,    TARGET_ENUMERATOR_CONE   },
    /*[105]*/   { "TARGET_UNIT_105_NYI",                                  TARGET_TYPE_UNIT                                                    },
    /*[106]*/   { "TARGET_LOCATION_106_NYI",                              TARGET_TYPE_LOCATION                                                },
    /*[107]*/   { "TARGET_UNK_107",                                       TARGET_TYPE_UNKNOWN                                                 },
    /*[108]*/   { "TARGET_ENUM_GAMEOBJECTS_IN_CONE",                      TARGET_TYPE_GAMEOBJECT, TARGET_NEUTRAL,    TARGET_ENUMERATOR_CONE   },
    /*[109]*/   { "TARGET_UNK_109",                                       TARGET_TYPE_UNKNOWN                                                 },
    /*[110]*/   { "TARGET_ENUM_UNITS_SCRIPT_IN_CONE_110",                 TARGET_TYPE_UNIT,       TARGET_SCRIPT,     TARGET_ENUMERATOR_CONE   },
};
