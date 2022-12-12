/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ULDUAR_H
#define DEF_ULDUAR_H

enum
{
    MAX_ENCOUNTER               = 16,
    HARD_MODE_ENCOUNTER         = 8,
    KEEPER_ENCOUNTER            = 4,

    // Main boss types
    TYPE_LEVIATHAN              = 0,
    TYPE_IGNIS                  = 1,
    TYPE_RAZORSCALE             = 2,
    TYPE_XT002                  = 3,
    TYPE_ASSEMBLY               = 4,
    TYPE_KOLOGARN               = 5,
    TYPE_AURIAYA                = 6,
    TYPE_MIMIRON                = 7,
    TYPE_HODIR                  = 8,
    TYPE_THORIM                 = 9,
    TYPE_FREYA                  = 10,
    TYPE_VEZAX                  = 11,
    TYPE_YOGGSARON              = 12,
    TYPE_ALGALON                = 13,
    TYPE_ALGALON_TIMER          = 14,
    TYPE_CHAMPION_FAILED        = 15,                       // Achievements Champion / Conquerer of Ulduar, needs to be saved to database

    // Hard mode boss types
    // Used for hard mode bosses only
    TYPE_LEVIATHAN_HARD         = 16,
    TYPE_XT002_HARD             = 17,
    TYPE_MIMIRON_HARD           = 18,
    TYPE_HODIR_HARD             = 19,
    TYPE_THORIM_HARD            = 20,
    TYPE_FREYA_HARD             = 21,
    TYPE_VEZAX_HARD             = 22,
    TYPE_YOGGSARON_HARD         = 23,

    // Keeper types
    // Used to store the keepers which will be used at yogg
    TYPE_KEEPER_HODIR           = 24,
    TYPE_KEEPER_FREYA           = 25,
    TYPE_KEEPER_THORIM          = 26,
    TYPE_KEEPER_MIMIRON         = 27,

    // Tower types
    // Used to store the towers which will be used at Leviathan encounter
    TYPE_TOWER_HODIR            = 28,
    TYPE_TOWER_FREYA            = 29,
    TYPE_TOWER_THORIM           = 30,
    TYPE_TOWER_MIMIRON          = 31,

    // Other types - not saved
    TYPE_LEVIATHAN_GAUNTLET     = 32,

    // Other types - saved
    TYPE_FREYA_CONSPEEDATORY    = 33, // SetData ID

    ACHIEV_FREYA_CONSPEEDATORY  = 0,  // Saving array ID
    MAX_ACHIEV_ENCOUNTER        = 1,

    // The siege of ulduar
    NPC_LEVIATHAN               = 33113,
    // NPC_IGNIS                = 33118,
    NPC_RAZORSCALE              = 33186,
    NPC_XT002                   = 33293,
    NPC_HEART_DECONSTRUCTOR     = 33329,
    NPC_XT_TOY_PILE             = 33337,                    // robot spawner npc for XT002

    // Leviathan other npcs
    NPC_ULDUAR_COLOSSUS         = 33237,
    NPC_BRONZEBEARD_RADIO       = 34054,
    NPC_EXPLORER_DELLORAH       = 33701,
    NPC_BRANN_BRONZEBEARD       = 33579,
    NPC_ORBITAL_SUPPORT         = 34286,
    // NPC_GENERATOR            = 33571,                    // spawns iron dwarfs from Storm Beacons
    NPC_GENERATOR_SMALL         = 34159,                    // spawns iron dwarfs from hard mode towers

    // Leviathan reinforcements
    // NPC_HIRED_ENGINEER       = 33626,
    // NPC_HIRED_DEMOLITIONIST  = 33627,
    // NPC_BATTLE_MAGE          = 33662,
    NPC_SALVAGED_SIEGE_ENGINE   = 33060,
    NPC_SALVAGED_CHOPPER        = 33062,
    NPC_SALVAGED_DEMOLISHER     = 33109,
    // NPC_LIQUID_PYRITE        = 33189,

    // Razorscale helper npcs
    NPC_EXPEDITION_COMMANDER    = 33210,
    NPC_EXPEDITION_ENGINEER     = 33287,                    // npc used to repair the Harpoons
    NPC_EXPEDITION_TRAPPER      = 33259,                    // npc used in Razorscale grounding phase
    NPC_EXPEDITION_DEFENDER     = 33816,                    // used to fight Razorscale
    NPC_RAZORSCALE_CONTROLLER   = 33233,                    // harpoon shot trigger npc for phase

    // The antechamber of ulduar
    NPC_STEELBREAKER            = 32867,
    NPC_MOLGEIM                 = 32927,
    NPC_BRUNDIR                 = 32857,
    NPC_KOLOGARN                = 32930,
    NPC_RIGHT_ARM               = 32934,
    NPC_LEFT_ARM                = 32933,
    NPC_RUBBLE_STALKER          = 33809,                    // npc which spawns Rubble on Kologarn arms death
    NPC_AURIAYA                 = 33515,
    NPC_SANCTUM_SENTRY          = 34014,
    NPC_FERAL_DEFENDER          = 34035,

    // Archivum
    NPC_BRANN_ARCHIVUM          = 33235,                    // npcs are spawned upon the Iron Council defeat; they handle specific quests
    NPC_PROSPECTOR_DOREN        = 33956,
    NPC_PROSPECTOR_DOREN_H      = 33957,
    NPC_BRANN_ALGALON           = 34064,                    // Brann entry summoned for the Algalon intro and epilogue event

    // The keepers of ulduar
    NPC_MIMIRON                 = 33350,
    NPC_HODIR                   = 32845,
    NPC_THORIM                  = 32865,
    NPC_FREYA                   = 32906,

    // Freya elders
    NPC_ELDER_BRIGHTLEAF        = 32915,
    NPC_ELDER_IRONBRACH         = 32913,
    NPC_ELDER_STONEBARK         = 32914,
    NPC_FREYA_ACHIEV_TRIGGER    = 33406,                    // Cast spell 65015 for achievs 2985 and 2984

    // Hodir helpers
    NPC_DRUID_HORDE_N           = 32941,                    // Tor Greycloud
    NPC_DRUID_HORDE_H           = 33333,                    // Kar Greycloud
    NPC_SHAMAN_HORDE_N          = 32950,                    // Spiritwalker Yona
    NPC_SHAMAN_HORDE_H          = 33332,                    // Spiritwalker Tara
    NPC_MAGE_HORDE_N            = 32946,                    // Veesha Blazeweaver
    NPC_MAGE_HORDE_H            = 33331,                    // Amira Blazeweaver
    NPC_PRIEST_HORDE_N          = 32948,                    // Battle-Priest Eliza
    NPC_PRIEST_HORDE_H          = 33330,                    // Battle-Priest Gina
    NPC_DRUID_ALLIANCE_N        = 32901,                    // Ellie Nightfeather
    NPC_DRUID_ALLIANCE_H        = 33325,                    // Eivi Nightfeather
    NPC_SHAMAN_ALLIANCE_N       = 32900,                    // Elementalist Avuun
    NPC_SHAMAN_ALLIANCE_H       = 33328,                    // Elementalist Mahfuun
    NPC_MAGE_ALLIANCE_N         = 32893,                    // Missy Flamecuffs
    NPC_MAGE_ALLIANCE_H         = 33327,                    // Sissy Flamecuffs
    NPC_PRIEST_ALLIANCE_N       = 32897,                    // Field Medic Penny
    NPC_PRIEST_ALLIANCE_H       = 33326,                    // Field Medic Jessi

    // Thorim event npcs
    NPC_RUNIC_COLOSSUS          = 32872,
    NPC_RUNE_GIANT              = 32873,
    NPC_SIF                     = 33196,
    NPC_JORMUNGAR_BEHEMOTH      = 32882,
    NPC_DARK_RUNE_ACOLYTE       = 32886,
    NPC_SOLDIER_ALLIANCE        = 32885,
    NPC_CAPTAIN_ALLIANCE        = 32908,
    NPC_SOLDIER_HORDE           = 32883,
    NPC_CAPTAIN_HORDE           = 32907,
    NPC_THORIM_EVENT_BUNNY      = 32892,
    NPC_THUNDER_ORB             = 33378,
    NPC_THORIM_CONTROLLER       = 32879,
    NPC_THORIM_COMBAT_TRIGGER   = 34055,
    NPC_RIGHT_HAND_BUNNY        = 33140,
    NPC_LEFT_HAND_BUNNY         = 33141,
    NPC_HONOR_GUARD_STAIRS      = 33125,                    // summoned mobs before the Rune Giant
    NPC_RUNE_ACOLYTE_STAIRS     = 32957,

    // Mimiron event npcs
    NPC_LEVIATHAN_MK            = 33432,
    NPC_LEVIATHAN_MK_TURRET     = 34071,
    NPC_VX001                   = 33651,
    NPC_AERIAL_UNIT             = 33670,
    NPC_COMPUTER                = 34143,
    NPC_BOT_SUMMON_TRIGGER      = 33856,
    NPC_WORLD_TRIGGER_FLAMES    = 21252,

    // The descent into madness
    NPC_VEZAX                   = 33271,
    NPC_SARONITE_ANIMUS         = 33524,
    NPC_VEZAX_BUNNY             = 33500,
    NPC_YOGGSARON               = 33288,
    NPC_SARA                    = 33134,
    NPC_YOGG_BRAIN              = 33890,
    NPC_VOICE_OF_YOGG           = 33280,
    NPC_OMINOUS_CLOUD           = 33292,
    NPC_GUARDIAN_OF_YOGG        = 33136,

    // Yogg Saron illusions actors
    NPC_YSERA                   = 33495,
    NPC_NELTHARION              = 33523,
    NPC_MALYGOS                 = 33535,
    NPC_ALEXSTRASZA             = 33536,
    NPC_GARONA                  = 33436,                    // cast spell 64063 on 33437
    NPC_KING_LLANE              = 33437,
    NPC_LICH_KING               = 33441,                    // cast spell 63037 on 33442
    NPC_IMMOLATED_CHAMPION      = 33442,
    NPC_YOGGSARON_ILLUSION      = 33552,

    // Celestial planetarium
    NPC_ALGALON                 = 32871,

    // Keepers helpers spawned during Yogg Saron encounter
    NPC_THORIM_HELPER           = 33413,
    NPC_MIMIRON_HELPER          = 33412,
    NPC_HODIR_HELPER            = 33411,
    NPC_FREYA_HELPER            = 33410,

    // Keepers spawned in the central hall after releaseed from Yogg's enslavement
    NPC_KEEPER_FREYA            = 33241,
    NPC_KEEPER_HODIR            = 33213,
    NPC_KEEPER_MIMIRON          = 33244,
    NPC_KEEPER_THORIM           = 33242,

    // Freya vigilance npc in the Freya hall for the Freya timed achievs 2980, 2981.
    NPC_CORRUPTED_SERVITOR      = 33354,
    NPC_MISGUIDED_NYMPH         = 33355,
    NPC_GUARDIAN_LASHER         = 33430,
    NPC_FOREST_SWARMER          = 33431,
    NPC_MANGROVE_ENT            = 33525,
    NPC_IRONROOT_LASHER         = 33526,
    NPC_NATURES_BLADE           = 33527,
    NPC_GUARDIAN_OF_LIFE        = 33528,

    // Generic
    NPC_INVISIBLE_STALKER       = 32780,                    // used for teleporting, and probably other things as well

    MAX_SPECIAL_ACHIEV_CRITS    = 20,

    TYPE_ACHIEV_CAT_LADY        = 0,
    TYPE_ACHIEV_NINE_LIVES      = 1,
    TYPE_ACHIEV_STEELBREAKER    = 2,
    TYPE_ACHIEV_BRUNDIR         = 3,
    TYPE_ACHIEV_MOLGEIM         = 4,
    TYPE_ACHIEV_STUNNED         = 5,
    TYPE_ACHIEV_SHATTERED       = 6,
    TYPE_ACHIEV_QUICK_SHAVE     = 7,
    TYPE_ACHIEV_SHUTOUT         = 8,
    TYPE_ACHIEV_NERF_ENG        = 9,
    TYPE_ACHIEV_RUBBLE          = 10,
    TYPE_ACHIEV_LOOKS_KILL      = 11,
    TYPE_ACHIEV_OPEN_ARMS       = 12,
    TYPE_ACHIEV_DISARMED        = 13,
    TYPE_ACHIEV_CHEESE_FREEZE   = 14,
    TYPE_ACHIEV_COOL_FRIENDS    = 15,
    TYPE_ACHIEV_LIGHTNING       = 16,
    TYPE_ACHIEV_BACK_NATURE     = 17,
    TYPE_ACHIEV_DRIVE_CRAZY     = 18,
    TYPE_ACHIEV_SHADOWDODGER    = 19,

    // Loot chests
    // Kologarn
    GO_CACHE_OF_LIVING_STONE_10 = 195046,
    GO_CACHE_OF_LIVING_STONE_25 = 195047,

    // Hodir
    GO_CACHE_OF_WINTER_10       = 194307,
    GO_CACHE_OF_WINTER_25       = 194308,
    GO_CACHE_OF_RARE_WINTER_10  = 194200,
    GO_CACHE_OF_RARE_WINTER_25  = 194201,

    // Mimiron
    GO_CACHE_OF_INOV_10         = 194789,
    GO_CACHE_OF_INOV_25         = 194956,
    GO_CACHE_OF_INOV_10_H       = 194957,
    GO_CACHE_OF_INOV_25_H       = 194958,

    // Thorim
    GO_CACHE_OF_STORMS_10       = 194312,
    GO_CACHE_OF_STORMS_25       = 194315,
    GO_CACHE_OF_STORMS_10_H     = 194313,
    GO_CACHE_OF_STORMS_25_H     = 194314,

    // Alagon
    GO_GIFT_OF_OBSERVER_10      = 194821,
    GO_GIFT_OF_OBSERVER_25      = 194822,

    // Doors and other Objects
    // The siege
    GO_SHIELD_WALL              = 194416,                   // Gate before Leviathan
    GO_LIGHTNING_DOOR           = 194905,                   // Lightning gate after the Leviathan. It closes after the boss enters the arena
    GO_LEVIATHAN_GATE           = 194630,                   // Gate after Leviathan -> this will be broken when the boss enters the arena
    GO_XT002_GATE               = 194631,                   // Gate before Xt002
    GO_BROKEN_HARPOON           = 194565,                   // Broken harpoon from Razorscale
    GO_HARPOON_GUN_1            = 194542,                   // usable harpoons - respawn when the broken one is repaired
    GO_HARPOON_GUN_2            = 194541,
    GO_HARPOON_GUN_3            = 194543,
    GO_HARPOON_GUN_4            = 194519,
    GO_FREYA_CRYSTAL            = 194704,                   // crystals which can be active during Leviathan encounter if hard mode towers are active
    GO_MIMIRON_CRYSTAL          = 194705,
    GO_THORIM_CRYSTAL           = 194706,
    GO_HODIR_CRYSTAL            = 194707,

    // Antechamber
    GO_KOLOGARN_BRIDGE          = 194232,
    // GO_SHATTERED_DOOR        = 194553,                   // Door before kologarn
    GO_IRON_ENTRANCE_DOOR       = 194554,                   // Door before iron council
    GO_ARCHIVUM_DOOR            = 194556,                   // Entrance door to the archivum
    // GO_ARCHIVUM_CONSOLE      = 194555,                   // Used at some sort of cinematic
    // GO_FLOOR_ARCHIVUM        = 194715,                   // Used for animation

    // Planetarium
    GO_CELESTIAL_ACCES          = 194628,                   // Acces console for 10 man mode
    GO_CELESTIAL_ACCES_H        = 194752,                   // Acces console for 25 man mode
    GO_CELESTIAL_DOOR_1         = 194767,                   // Entrance doors to the planetarium
    GO_CELESTIAL_DOOR_2         = 194911,
    GO_CELESTIAL_DOOR_COMBAT    = 194910,
    GO_UNIVERSE_FLOOR           = 194716,                   // For animation
    GO_UNIVERSE_FLOOR_COMBAT    = 194715,
    GO_AZEROTH_GLOBE            = 194148,                   // For animation

    // The keepers
    // Hodir
    GO_HODIR_EXIT               = 194634,
    GO_HODIR_ICE_WALL           = 194441,
    GO_HODIR_ENTER              = 194442,
    // Mimiron
    GO_MIMIRON_BUTTON           = 194739,                   // Used to start hard mode
    GO_MIMIRON_DOOR_1           = 194774,
    GO_MIMIRON_DOOR_2           = 194775,
    GO_MIMIRON_DOOR_3           = 194776,
    GO_MIMIRON_TEL1             = 194741,                   // Used to summon mobs in phase 3
    GO_MIMIRON_TEL2             = 194742,
    GO_MIMIRON_TEL3             = 194743,
    GO_MIMIRON_TEL4             = 194744,
    GO_MIMIRON_TEL5             = 194740,
    GO_MIMIRON_TEL6             = 194746,
    GO_MIMIRON_TEL7             = 194747,
    GO_MIMIRON_TEL8             = 194748,
    GO_MIMIRON_TEL9             = 194745,
    GO_MIMIRON_ELEVATOR         = 194749,                   // Central elevator
    // Thorim
    GO_DARK_IRON_PORTCULIS      = 194560,                   // Door from the arena to the hallway
    GO_RUNED_STONE_DOOR         = 194557,                   // Door after the runic colossus
    GO_THORIM_STONE_DOOR        = 194558,                   // Door after the ancient rune giant
    GO_LIGHTNING_FIELD          = 194559,                   // Arena exit door
    GO_DOOR_LEVER               = 194264,                   // In front of the door

    // Descent to madness
    GO_ANCIENT_GATE             = 194255,                   // Door upstairs before vezax, opens when all keepers are freed
    GO_VEZAX_GATE               = 194750,                   // Door after vezax
    GO_YOGG_GATE                = 194773,                   // Yogg-Saron chamber door
    GO_BRAIN_DOOR_CHAMBER       = 194635,                   // Brain chamber doors
    GO_BRAIN_DOOR_ICECROWN      = 194636,
    GO_BRAIN_DOOR_STORMWIND     = 194637,
    GO_FLEE_TO_SURFACE          = 194625,                   // Brain chamber portals

    // Mimiron tram
    GO_TRAM                     = 194675,                   // Transport GO to Mimiron
    GO_ACTIVATE_TRAM            = 194437,                   // button that starts the tram; the button exists inside the tram
    GO_CALL_TRAM_CENTER         = 194914,                   // button - central Ulduar
    GO_CALL_TRAM_MIMIRON        = 194912,                   // button - mimiron
    GO_TRAM_TURNAROUND_CENTER   = 194915,                   // triggered by event 21393, when tram does 1st stop
    GO_TRAM_TURNAROUND_MIMIRON  = 194913,                   // triggered by event 21394, when tram does 2nd stop

    // generic
    GO_ULDUAR_TELEPORTER        = 194569,

    // World state used for algalon timer
    WORLD_STATE_TIMER           = 4132,
    WORLD_STATE_TIMER_COUNT     = 4131,

    // common spells
    SPELL_TELEPORT              = 62940,

    // events
    EVENT_ID_SHUTDOWN           = 21605,
    EVENT_ID_SCRAP_REPAIR       = 21606,
    EVENT_ID_SPELL_SHATTER      = 21620,
    EVENT_ID_TOWER_LIFE         = 21030,                    // events checked when a tower is destroyed
    EVENT_ID_TOWER_FLAME        = 21033,
    EVENT_ID_TOWER_FROST        = 21032,
    EVENT_ID_TOWER_STORMS       = 21031,

    // EVENT_ID_TRAM_MIMIRON    = 21393,                    // tram reached Mimiron
    // EVENT_ID_TRAM_CENTER     = 21394,                    // tram reached Center

    // area triggers
    AREATRIGGER_ID_INTRO        = 5388,                     // starts the intro dialogue
    AREATRIGGER_ID_REPAIR_1     = 5369,                     // related to vehicle repair
    AREATRIGGER_ID_REPAIR_2     = 5423,

    // Achievement related
    ACHIEV_START_IGNIS_ID       = 20951,                    // Ignis timed achievs 2930, 2929
    ACHIEV_START_XT002_ID       = 21027,                    // XT-002 timed achievs 2937, 2938
    ACHIEV_START_FREYA_ID       = 21597,                    // Freya timed achievs 2980, 2981
    ACHIEV_START_YOGG_ID        = 21001,                    // Yogg timed achievs 3012, 3013
    ACHIEV_START_COMING_WALLS   = 33136,                    // Guardian of Yogg timed achievs 3014, 3017
    ACHIEV_START_DWARFAGEDDON   = 65387,                    // Ulduar gauntlet timed achievs 3097, 3098; triggered by missing spell 65387
    ACHIEV_START_LUMBERJACKED   = 21686,                    // Ulduar elder kill timed achievs 2979, 3118; triggered by missing spell 65296
    ACHIEV_START_NERF_BOTS      = 65037,                    // XT-002 gauntlet timed achievs 2933, 2935; triggered by missing spell 65037
    ACHIEV_START_SUPERMASSIVE   = 21697,                    // Black hole timed achievs 3003, 3002

    ACHIEV_CRIT_SARONITE_N      = 10451,                    // General Vezax, achievs 3181, 3188
    ACHIEV_CRIT_SARONITE_H      = 10462,
    ACHIEV_CRIT_SHADOWDODGER_N  = 10173,                    // General Vezax, achievs 2996, 2997
    ACHIEV_CRIT_SHADOWDODGER_H  = 10306,
    ACHIEV_CRIT_CAT_LADY_N      = 10400,                    // Auriaya, achievs 3006, 3007
    ACHIEV_CRIT_CAT_LADY_H      = 10184,
    ACHIEV_CRIT_NINE_LIVES_N    = 10399,                    // Auriaya, achievs 3076, 3077
    ACHIEV_CRIT_NINE_LIVES_H    = 10243,
    ACHIEV_CRIT_STEELBREAKER_N  = 10084,                    // Iron council, achievs 2941, 2944
    ACHIEV_CRIT_STEELBREAKER_H  = 10087,
    ACHIEV_CRIT_MOLGEIM_N       = 10082,                    // Iron council, achievs 2939, 2942
    ACHIEV_CRIT_MOLGEIM_H       = 10085,
    ACHIEV_CRIT_BRUNDIR_N       = 10083,                    // Iron council, achievs 2940, 2943
    ACHIEV_CRIT_BRUNDIR_H       = 10086,
    ACHIEV_CRIT_STUNNED_BRUND_N = 10090,                    // Iron council, achiev 2947
    ACHIEV_CRIT_STUNNED_STEEL_N = 10422,
    ACHIEV_CRIT_STUNNED_MOLG_N  = 10423,
    ACHIEV_CRIT_STUNNED_BRUND_H = 10091,                    // Iron council, achiev 2948
    ACHIEV_CRIT_STUNNED_STEEL_H = 10424,
    ACHIEV_CRIT_STUNNED_MOLG_H  = 10425,
    ACHIEV_CRIT_SHATTERED_N     = 10068,                    // Ignis, achievs 2925, 2926
    ACHIEV_CRIT_SHATTERED_H     = 10069,
    ACHIEV_CRIT_HEARTBREAKER_N  = 10221,                    // XT-002, achievs 3058, 3059
    ACHIEV_CRIT_HEARTBREAKER_H  = 10220,
    ACHIEV_CRIT_NERF_ENG_N      = 10074,                    // XT-002, achievs 2931, 2932
    ACHIEV_CRIT_NERF_ENG_H      = 10075,
    ACHIEV_CRIT_NERF_GRAVITY_N  = 10077,                    // XT-002, achievs 2934, 2936
    ACHIEV_CRIT_NERF_GRAVITY_H  = 10079,
    ACHIEV_CRIT_QUICK_SHAVE_N   = 10062,                    // Razorscale, achievs 2919, 2921
    ACHIEV_CRIT_QUICK_SHAVE_H   = 10063,
    ACHIEV_CRIT_ORB_BOMB_N      = 10056,                    // Flame Leviathan, achievs 2913, 2918 (one tower)
    ACHIEV_CRIT_ORB_BOMB_H      = 10061,
    ACHIEV_CRIT_ORB_DEV_N       = 10057,                    // Flame Leviathan, achievs 2914, 2916 (two towers)
    ACHIEV_CRIT_ORB_DEV_H       = 10059,
    ACHIEV_CRIT_ORB_NUKED_N     = 10058,                    // Flame Leviathan, achievs 2915, 2917 (three towers)
    ACHIEV_CRIT_ORB_NUKED_H     = 10060,
    ACHIEV_CRIT_ORBITUARY_N     = 10218,                    // Flame Leviathan, achievs 3056, 3057 (four towers)
    ACHIEV_CRIT_ORBITUARY_H     = 10219,
    ACHIEV_CRIT_SHUTOUT_N       = 10054,                    // Flame Leviathan, achievs 2911, 2913
    ACHIEV_CRIT_SHUTOUT_H       = 10055,
    ACHIEV_CRIT_UNBROKEN_N      = 10044,                    // Flame Leviathan, achievs 2905, 2906
    ACHIEV_CRIT_UNBROKEN_H      = 10045,
    ACHIEV_CRIT_DISARMED_N      = 10284,                    // Kologarn, achievs 2953, 2954
    ACHIEV_CRIT_DISARMED_H      = 10722,
    ACHIEV_CRIT_LOOKS_KILL_N    = 10286,                    // Kologarn, achievs 2955, 2956
    ACHIEV_CRIT_LOOKS_KILL_H    = 10099,
    ACHIEV_CRIT_RUBBLE_ROLL_N   = 10290,                    // Kologarn, achievs 2959, 2960
    ACHIEV_CRIT_RUBBLE_ROLL_H   = 10133,
    ACHIEV_CRIT_OPEN_ARMS_N     = 10285,                    // Kologarn, achievs 2951, 2952
    ACHIEV_CRIT_OPEN_ARMS_H     = 10095,
    ACHIEV_CRIT_FEEDS_TEARS_N   = 10568,                    // Algalon, achievs 3004, 3005
    ACHIEV_CRIT_FEEDS_TEARS_H   = 10570,
    ACHIEV_CRIT_CHEESE_N        = 10259,                    // Hodir, achievs 2961, 2962
    ACHIEV_CRIT_CHEESE_H        = 10261,
    ACHIEV_CRIT_GETTING_COLD_N  = 10247,                    // Hodir, achievs 2967, 2968
    ACHIEV_CRIT_GETTING_COLD_H  = 10248,
    ACHIEV_CRIT_COOL_FRIENDS_N  = 10258,                    // Hodir, achievs 2963, 2965
    ACHIEV_CRIT_COOL_FRIENDS_H  = 10260,
    ACHIEV_CRIT_RARE_CACHE_N    = 10452,                    // Hodir, achievs 3182, 3184
    ACHIEV_CRIT_RARE_CACHE_H    = 10458,
    ACHIEV_CRIT_LIGHTNING_N     = 10305,                    // Thorim, achievs 2971, 2972
    ACHIEV_CRIT_LIGHTNING_H     = 10309,
    ACHIEV_CRIT_LOSE_ILLUSION_N = 10440,                    // Thorim, achievs 3176, 3183
    ACHIEV_CRIT_LOSE_ILLUSION_H = 10457,
    ACHIEV_CRIT_BACK_NATURE_N   = 10445,                    // Freya, achievs 2982, 2983
    ACHIEV_CRIT_BACK_NATURE_H   = 10758,
    ACHIEV_CRIT_KNOCK_1_N       = 10447,                    // Freya, achievs 3177, 3185
    ACHIEV_CRIT_KNOCK_1_H       = 10459,
    ACHIEV_CRIT_KNOCK_2_N       = 10448,                    // Freya, achievs 3178, 3186
    ACHIEV_CRIT_KNOCK_2_H       = 10460,
    ACHIEV_CRIT_KNOCK_3_N       = 10449,                    // Freya, achievs 3179, 3187
    ACHIEV_CRIT_KNOCK_3_H       = 10461,
    ACHIEV_CRIT_FIREFIGHTER_N   = 10450,                    // Mimiron, achievs 3180, 3189
    ACHIEV_CRIT_FIREFIGHTER_H   = 10463,
    ACHIEV_CRIT_THREE_LIGHTS_N  = 10410,                    // Yogg-Saron, achievs 3157, 3161,
    ACHIEV_CRIT_THREE_LIGHTS_H  = 10414,
    ACHIEV_CRIT_TWO_LIGHTS_N    = 10338,                    // Yogg-Saron, achievs 3141, 3162
    ACHIEV_CRIT_TWO_LIGHTS_H    = 10415,
    ACHIEV_CRIT_ONE_LIGHT_N     = 10409,                    // Yogg-Saron, achievs 3158, 3163
    ACHIEV_CRIT_ONE_LIGHT_H     = 10416,
    ACHIEV_CRIT_ALONE_DARK_N    = 10412,                    // Yogg-Saron, achievs 3159, 3164
    ACHIEV_CRIT_ALONE_DARK_H    = 10417,
    ACHIEV_CRIT_DRIVE_CRAZY_N   = 10185,                    // Yogg-Saron, achievs 3008, 3010
    ACHIEV_CRIT_DRIVE_CRAZY_H   = 10296,

    // Champion / Conquerer of Ulduar, achievs 2903, 2904
    ACHIEV_CRIT_CHAMP_LEVI      = 10042,
    ACHIEV_CRIT_CHAMP_RAZOR     = 10340,
    ACHIEV_CRIT_CHAMP_XT        = 10341,
    ACHIEV_CRIT_CHAMP_IGNIS     = 10342,
    ACHIEV_CRIT_CHAMP_MIMIRON   = 10347,
    ACHIEV_CRIT_CHAMP_KOLO      = 10348,
    ACHIEV_CRIT_CHAMP_VEZAX     = 10349,
    ACHIEV_CRIT_CHAMP_YOGG      = 10350,
    ACHIEV_CRIT_CHAMP_AURIAYA   = 10351,
    ACHIEV_CRIT_CHAMP_THORIM    = 10403,
    ACHIEV_CRIT_CHAMP_HODIR     = 10439,
    ACHIEV_CRIT_CHAMP_FREYA     = 10582,
    ACHIEV_CRIT_CHAMP_COUNCIL   = 10598,

    ACHIEV_CRIT_CONQ_LEVI       = 10352,
    ACHIEV_CRIT_CONQ_RAZOR      = 10353,
    ACHIEV_CRIT_CONQ_XT         = 10354,
    ACHIEV_CRIT_CONQ_IGNIS      = 10355,
    ACHIEV_CRIT_CONQ_KOLO       = 10357,
    ACHIEV_CRIT_CONQ_MIMIRON    = 10361,
    ACHIEV_CRIT_CONQ_VEZAX      = 10362,
    ACHIEV_CRIT_CONQ_AURIAYA    = 10363,
    ACHIEV_CRIT_CONQ_YOGG       = 10364,
    ACHIEV_CRIT_CONQ_THORIM     = 10404,
    ACHIEV_CRIT_CONQ_FREYA      = 10583,
    ACHIEV_CRIT_CONQ_COUNCIL    = 10599,
    ACHIEV_CRIT_CONQ_HODIR      = 10719,

    TIMER_HODIRS_PROTECTIVE_GAZE = 0,
};

struct UlduarSpawn
{
    float fX, fY, fZ, fO;
    uint32 uiEntry;
};

struct UlduarSpawnTwoSide
{
    float fX, fY, fZ, fO;
    uint32 uiAllyEntry, uiHordeEntry;
};

// Note: coordinates are guessed, but pretty close to what they should be
// ToDo: spawn additional Engineers, Demolitionists, Mages and Liquid Pyrite near the columns
static const UlduarSpawn afReinforcementsNormal[] =
{
    {118.797f, -26.9963f, 409.80f, 3.14f, NPC_SALVAGED_SIEGE_ENGINE},
    {118.847f, -43.758f, 409.80f, 3.15f, NPC_SALVAGED_SIEGE_ENGINE},
    {116.602f, 8.464f, 409.80f, 3.10f, NPC_SALVAGED_CHOPPER},
    {116.859f, -4.199f, 409.80f, 3.12f, NPC_SALVAGED_CHOPPER},
    {122.479f, 25.093f, 410.60f, 3.10f, NPC_SALVAGED_DEMOLISHER},
    {123.022f, 39.671f, 409.80f, 3.10f, NPC_SALVAGED_DEMOLISHER},
};

static const UlduarSpawn afReinforcementsHeroic[] =
{
    {106.359f, -35.269f, 409.80f, 3.12f, NPC_SALVAGED_SIEGE_ENGINE},
    {135.351f, -20.767f, 409.80f, 3.15f, NPC_SALVAGED_SIEGE_ENGINE},
    {135.408f, -50.178f, 409.80f, 3.12f, NPC_SALVAGED_SIEGE_ENGINE},
    {116.429f, 4.036f, 409.79f, 3.10f, NPC_SALVAGED_CHOPPER},
    {116.272f, -0.013f, 409.79f, 3.10f, NPC_SALVAGED_CHOPPER},
    {116.948f, -8.351f, 409.79f, 3.10f, NPC_SALVAGED_CHOPPER},
    {137.523f, 32.346f, 409.80f, 3.12f, NPC_SALVAGED_DEMOLISHER},
    {112.818f, 18.981f, 409.83f, 3.10f, NPC_SALVAGED_DEMOLISHER},
    {112.700f, 47.884f, 409.79f, 3.10f, NPC_SALVAGED_DEMOLISHER},
};

static const UlduarSpawnTwoSide afHodirHelpersNormal[] =
{
    {1999.903f, -230.4966f, 432.7581f, 1.53589f,  NPC_DRUID_ALLIANCE_N,  NPC_DRUID_HORDE_N},
    {2010.058f, -243.4553f, 432.7672f, 1.361357f, NPC_SHAMAN_ALLIANCE_N, NPC_SHAMAN_HORDE_N},
    {2021.118f, -236.6482f, 432.7672f, 1.937315f, NPC_MAGE_ALLIANCE_N,   NPC_MAGE_HORDE_N},
    {1983.751f, -243.3579f, 432.7672f, 1.570796f, NPC_PRIEST_ALLIANCE_N, NPC_PRIEST_HORDE_N},
};

static const UlduarSpawnTwoSide afHodirHelpersHeroic[] =
{
    {2013.37f, -240.331f, 432.687f, 1.80463f, NPC_DRUID_ALLIANCE_H,  NPC_DRUID_HORDE_H},
    {1983.89f, -240.369f, 432.687f, 1.37658f, NPC_SHAMAN_ALLIANCE_H, NPC_SHAMAN_HORDE_H},
    {2000.9f, -231.232f, 432.687f, 1.59846f,  NPC_MAGE_ALLIANCE_H,   NPC_MAGE_HORDE_H},
    {1997.88f, -239.394f, 432.687f, 1.4237f,  NPC_PRIEST_ALLIANCE_H, NPC_PRIEST_HORDE_H},
};

static const UlduarSpawnTwoSide afThorimSpawns[] =
{
    {2127.24f, -251.309f, 419.7935f, 5.899213f,   NPC_SOLDIER_HORDE, NPC_SOLDIER_ALLIANCE},
    {2123.316f, -254.7708f, 419.7886f, 6.178465f, NPC_SOLDIER_HORDE, NPC_SOLDIER_ALLIANCE},
    {2120.431f, -259.0431f, 419.6813f, 6.122538f, NPC_SOLDIER_HORDE, NPC_SOLDIER_ALLIANCE},
    {2145.503f, -256.3357f, 419.7306f, 3.520873f, NPC_CAPTAIN_HORDE, NPC_CAPTAIN_ALLIANCE},
};

// note: original spawn loc is 607.9199f, -12.90516f, 409.887f but we won't use it because it's too far and grid won't be loaded that far
static const float afLeviathanSpawnPos[4] = { 422.8898f, -13.32677f, 409.8839f, 3.12f };
static const float afLeviathanMovePos[4] = { 296.5809f, -11.55668f, 409.8278f, 3.12f };

// spawn locations for Brann and Doren at the archivum
static const float afBrannArchivumSpawnPos[4] = { 1554.274f, 142.1644f, 427.273f, 3.61f };
static const float afProspectorSpawnPos[4] = { 1556.469f, 143.5023f, 427.2918f, 4.04f };

// spawn location for Algalon in reload case
static const float afAlgalonMovePos[4] = {1632.668f, -302.7656f, 417.3211f, 1.53f};

class instance_ulduar : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_ulduar(Map* pMap);
        ~instance_ulduar() {}

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnPlayerDeath(Player* pPlayer) override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* creature) override;
        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnObjectSpawn(GameObject* go) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;
        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        void DoSpawnThorimNpcs(Player* pSummoner);
        void DoProcessShatteredEvent();

        ObjectGuid GetKoloRubbleStalker(bool bRightSide) { return bRightSide ? m_rightKoloStalkerGuid : m_leftKoloStalkerGuid; }
        ObjectGuid GetVezaxBunnyGuid(bool bAnimus) { return bAnimus ? m_animusVezaxBunnyGuid : m_vaporVezaxBunnyGuid; }

        void GetDefenderGuids(GuidList& lDefenders) const { lDefenders = m_lDefendersGuids; }
        void GetEngineersGuids(GuidList& lEngineers) const { lEngineers = m_lEngineersGuids; }
        void GetTrappersGuids(GuidList& lTrappers) const { lTrappers = m_lTrappersGuids; }
        void GetHarpoonsGuids(GuidVector& vHarpoons) const { vHarpoons = m_vBrokenHarpoonsGuids; }
        void GetToyPileGuids(GuidVector& vToyPiles) const { vToyPiles = m_vToyPileGuidVector; }
        void GetThorimBunniesGuids(GuidList& lBunnies, bool bUpper) { lBunnies = bUpper ? m_lUpperBunniesGuids : m_lThorimBunniesGuids; }
        void GetThunderOrbsGuids(GuidList& lOrbs) const { lOrbs = m_lUpperThunderOrbsGuids; }
        void GetSmashTargetsGuids(GuidList& lTargets, bool bLeft) { lTargets = bLeft ? m_lLeftHandBunniesGuids : m_lRightHandBunniesGuids; }
        void GetOminousCloudGuids(GuidList& lClouds) const { lClouds = m_lOminousCloudsGuids; }

        // Function that will trigger the tram turnaround object, based on the tram location
        void SetTramRotateTimer() { m_uiTramRotateTimer = m_bTramAtCenter ? 33000 : 30000; }

        void Update(uint32 uiDiff);

    protected:
        void JustDidDialogueStep(int32 iEntry) override;
        void SpawnFriendlyKeeper(uint32 uiWho);
        void SpawnKeeperHelper(uint32 uiWho);
        void DoSpawnHodirNpcs(Player* pSummoner);
        void DoOpenMadnessDoorIfCan();
        void DoCallLeviathanHelp();

        std::string m_strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        uint32 m_auiHardBoss[HARD_MODE_ENCOUNTER];
        uint32 m_auiUlduarKeepers[KEEPER_ENCOUNTER];
        uint32 m_auiUlduarTowers[KEEPER_ENCOUNTER];
        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];
        uint32 m_auiAchievEncounter[MAX_ACHIEV_ENCOUNTER]; // for achievements which need saving

        bool m_bHelpersLoaded;
        bool m_bTramAtCenter;

        uint32 m_uiAlgalonTimer;
        uint32 m_uiYoggResetTimer;
        uint32 m_uiShatterAchievTimer;
        uint32 m_uiGauntletStatus;
        uint32 m_uiStairsSpawnTimer;
        uint32 m_uiTramRotateTimer;
        uint8 m_uiSlayedArenaMobs;

        ObjectGuid m_leftKoloStalkerGuid;
        ObjectGuid m_rightKoloStalkerGuid;
        ObjectGuid m_animusVezaxBunnyGuid;
        ObjectGuid m_vaporVezaxBunnyGuid;

        GuidVector m_vToyPileGuidVector;
        GuidVector m_vBrokenHarpoonsGuids;
        GuidList m_lEngineersGuids;
        GuidList m_lTrappersGuids;
        GuidList m_lDefendersGuids;
        GuidList m_lHarpoonDummyGuids;
        GuidList m_lRepairedHarpoonsGuids;
        GuidList m_lThorimBunniesGuids;
        GuidList m_lUpperBunniesGuids;
        GuidList m_lUpperThunderOrbsGuids;
        GuidList m_lLeftHandBunniesGuids;
        GuidList m_lRightHandBunniesGuids;
        GuidList m_lOminousCloudsGuids;
        GuidSet m_sColossusGuidSet;
};

#endif
