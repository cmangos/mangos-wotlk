/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WORLD_MAP_SCRIPTS_H
#define DEF_WORLD_MAP_SCRIPTS_H

enum
{
    // Quest 4740
    NPC_GREYMIST_COASTRUNNNER   = 2202,
    NPC_GREYMIST_WARRIOR        = 2205,
    NPC_GREYMIST_HUNTER         = 2206,
    NPC_MURKDEEP                = 10323,
    QUEST_WANTED_MURKDEEP       = 4740,

    // Quest 6134
    GO_GHOST_MAGNET             = 177746,
    NPC_MAGRAMI_SPECTRE         = 11560,
    SPELL_SPIRIT_SPAWN_OUT      = 17680,

    // Quest 6403
    NPC_JONATHAN                = 466,
    NPC_WRYNN                   = 1747,
    NPC_BOLVAR                  = 1748,
    NPC_PRESTOR                 = 1749,
    NPC_WINDSOR                 = 12580,

    // Quest 11538
    NPC_EMISSARY_OF_HATE        = 25003,
    NPC_IRESPEAKER              = 24999,
    NPC_UNLEASHED_HELLION       = 25002,

    // Quest 10594 - Gauging the Resonant Frequency
    NPC_WYRM_FROM_BEYOND = 21796,
    NPC_OSCILLATING_FREQUENCY_SCANNER_BUNNY = 21760,

    // Quest 10998
    NPC_VIMGOL_VISUAL_BUNNY     = 23040,

    // Quest 10607
    NPC_WHISPER_RAVEN_GOD_TEMPLATE = 21851,

    // Quest 10310
    GO_ROCKET_FIRE              = 183987,
    GO_ROCKET_SMOKE             = 183988,

    // Quest 10409
    NPC_SOCRETHAR = 20132,

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

    // Quest 8868
    NPC_MINION_OMEN             = 15466,
    NPC_OMEN                    = 15467,
    GO_ROCKET_CLUSTER           = 180875,
    SPELL_OMEN_MOONLIGHT        = 26392,
    MAX_ROCKETS                 = 30,

    // Elemental invasions
    NPC_THE_WINDREAVER          = 14454,
    NPC_PRINCESS_TEMPESTRIA     = 14457,
    NPC_BARON_CHARR             = 14461,
    NPC_AVALANCHION             = 14464,

    GO_EARTH_RIFT               = 179664,
    GO_WATER_RIFT               = 179665,
    GO_FIRE_RIFT                = 179666,
    GO_AIR_RIFT                 = 179667,

    // Deaths Door gates
    NPC_DEATHS_DOOR_NORTH_WARP_GATE = 22471,
    NPC_DEATHS_DOOR_SOUTH_WARP_GATE = 22472,

    TYPE_DEATHS_DOOR_NORTH      = 0,
    TYPE_DEATHS_DOOR_SOUTH      = 1,
    TYPE_SHARTUUL               = 2,

    // Quest 1126
    NPC_HIVE_ASHI_DRONES        = 13136,

    // Bashir
    SPELL_SPIRIT_SPAWN_IN       = 17321,
    SPELL_ETHEREAL_TELEPORT     = 34427,
    MOUNT_NETHER_RAY_DISPLAY_ID = 21156,

    // Ogri'la Four Dragons
    NPC_OBSIDIA = 23282,
    NPC_RIVENDARK = 23061,
    NPC_FURYWING = 23261,
    NPC_INSIDION = 23281,
};

enum Encounters
{
    TYPE_OMEN   = 0, // Quest 8868
    TYPE_HIVE   = 1,
    TYPE_TETHYR = 2, // Quest 11198
    TYPE_BASHIR = 3,
    MAX_ENCOUNTER
};

enum ElementalInvasionIndexes
{
    ELEMENTAL_EARTH             = 0,
    ELEMENTAL_AIR               = 1,
    ELEMENTAL_FIRE              = 2,
    ELEMENTAL_WATER             = 3,
    MAX_ELEMENTS
};

enum SpawnIndexes
{
    POS_IDX_EMISSARY_SPAWN      = 0,
    POS_IDX_MURKDEEP_SPAWN      = 1,
    POS_IDX_MURKDEEP_MOVE       = 2,
    POS_IDX_OMEN_SPAWN          = 3,
    POS_IDX_OMEN_MOVE           = 4,
    POS_IDX_MINION_OMEN_START   = 5,
    POS_IDX_MINION_OMEN_STOP    = 11,
    POS_IDX_HIVE_DRONES_START   = 12,
    POS_IDX_HIVE_DRONES_STOP    = 14,
    POS_IDX_MAX                 = 15
};

static const float aSpawnLocations[POS_IDX_MAX][4] =
{
    {12583.019f, -6916.194f,    4.601f, 6.18f},         // Emissary of Hate, guesswork
    {4981.031f,    597.955f,   -1.361f, 4.82f},         // Murkdeep spawn, guesswork
    {4988.970f,    547.002f,    5.379f,  0.0f},         // Murkdeep move, guesswork
    {7564.098f,  -2835.687f,  447.374f, 3.98f},         // Omen spawn, guesswork
    {7528.430f,  -2889.838f,  459.365f,  0.0f},         // Omen move, guesswork
    {7669.430f,  -2869.980f,  464.904f, 2.60f},         // Minions of Omen
    {7621.790f,  -2861.740f,  459.043f, 4.15f},
    {7597.130f,  -2885.880f,  464.898f, 2.88f},
    {7594.170f,  -2856.490f,  459.403f, 3.17f},
    {7477.750f,  -2849.020f,  464.466f, 0.14f},
    {7507.560f,  -2818.230f,  459.486f, 4.63f},
    {7558.880f,  -2856.770f,  457.684f, 4.31f},
    {-7185.94f,     443.29f,  26.59f, 4.8458f},         // Hive'Ashi Drones spawn 1, guesswork
    {-7180.59f,     441.33f,  26.68f, 4.1798f},         // Hive'Ashi Drones spawn 2, guesswork
    {-7176.63f,     437.42f,  26.84f, 3.9348f}          // Hive'Ashi Drones spawn 3, guesswork
};

#endif
