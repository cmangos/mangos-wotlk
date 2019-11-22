/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WORLD_MAP_SCRIPTS_H
#define DEF_WORLD_MAP_SCRIPTS_H

enum
{
    // Quest 6403
    NPC_JONATHAN                = 466,
    NPC_WRYNN                   = 1747,
    NPC_BOLVAR                  = 1748,
    NPC_PRESTOR                 = 1749,
    NPC_WINDSOR                 = 12580,

    // Transports
    NPC_NEZRAZ                  = 3149,
    NPC_HINDENBURG              = 3150,
    NPC_FREZZA                  = 9564,
    NPC_ZAPETTA                 = 9566,
    NPC_SNURK_BUCKSQUICK        = 12136,
    NPC_SQUIBBY_OVERSPECK       = 12137,
    NPC_HARROWMEISER            = 23823,
    NPC_GREEB_RAMROCKET         = 26537,
    NPC_NARGO_SCREWBORE         = 26538,
    NPC_MEEFI_FARTHROTTLE       = 26539,
    NPC_DRENK_SPANNERSPARK      = 26540,
    NPC_ZELLI_HOTNOZZLE         = 34765,
    NPC_KRENDLE_BIGPOCKETS      = 34766,

    // Elemental invasions
    NPC_THE_WINDREAVER          = 14454,
    NPC_PRINCESS_TEMPESTRIA     = 14457,
    NPC_BARON_CHARR             = 14461,
    NPC_AVALANCHION             = 14464,

    GO_EARTH_RIFT               = 179664,
    GO_WATER_RIFT               = 179665,
    GO_FIRE_RIFT                = 179666,
    GO_AIR_RIFT                 = 179667,

    // Quest 1126
    NPC_HIVE_ASHI_DRONES        = 13136,
};

enum ElementalInvasionIndexes
{
    ELEMENTAL_EARTH             = 0,
    ELEMENTAL_AIR               = 1,
    ELEMENTAL_FIRE              = 2,
    ELEMENTAL_WATER             = 3,
    MAX_ELEMENTS
};

#endif
