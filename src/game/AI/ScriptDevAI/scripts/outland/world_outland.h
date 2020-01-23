
#ifndef DEF_WORLD_MAP_OUTLAND_SCRIPTS_H
#define DEF_WORLD_MAP_OUTLAND_SCRIPTS_H

enum
{
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
    GO_ROCKET_FIRE  = 183987,
    GO_ROCKET_SMOKE = 183988,

    // Quest 10409
    NPC_SOCRETHAR = 20132,

    // Deaths Door gates
    NPC_DEATHS_DOOR_NORTH_WARP_GATE = 22471,
    NPC_DEATHS_DOOR_SOUTH_WARP_GATE = 22472,

    TYPE_DEATHS_DOOR_NORTH  = 0,
    TYPE_DEATHS_DOOR_SOUTH  = 1,
    TYPE_SHARTUUL           = 2,
    TYPE_BASHIR             = 3,

    // Bashir
    SPELL_SPIRIT_SPAWN_IN       = 17321,
    SPELL_ETHEREAL_TELEPORT     = 34427,
    MOUNT_NETHER_RAY_DISPLAY_ID = 21156,

    // Ogri'la Four Dragons
    NPC_OBSIDIA   = 23282,
    NPC_RIVENDARK = 23061,
    NPC_FURYWING  = 23261,
    NPC_INSIDION  = 23281,

    // Ring of Blood
    NPC_BROKENTOE           = 18398,
    NPC_MURKBLOOD_TWIN      = 18399,
    NPC_ROKDAR              = 18400,
    NPC_SKRAGATH            = 18401,
    NPC_WARMAUL_CHAMPION    = 18402,
    NPC_MOGOR               = 18069,
};

enum SpawnIndexes
{
    POS_IDX_EMISSARY_SPAWN = 0,
    POS_IDX_MAX,
};

static const float aSpawnLocations[POS_IDX_MAX][4] =
{
    {12583.019f, -6916.194f,    4.601f, 6.18f},         // Emissary of Hate, guesswork
};

#endif