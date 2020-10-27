/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WORLD_MAP_SCRIPTS_H
#define DEF_WORLD_MAP_SCRIPTS_H

enum
{
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

    NPC_HIGHLORD_KRUUL          = 18338,

    // Shade of the Horseman
    NPC_COSTUMED_ORPHAN_MATRON  = 24519,
    NPC_TOURING_ORPHAN          = 23712,
    NPC_MASKED_ORPHAN_MATRON    = 23973,
    NPC_TRAVELING_ORPHAN        = 23971,
};

enum ElementalInvasionIndexes
{
    ELEMENTAL_EARTH             = 0,
    ELEMENTAL_AIR               = 1,
    ELEMENTAL_FIRE              = 2,
    ELEMENTAL_WATER             = 3,
    MAX_ELEMENTS
};

// Shade of the Horseman village attack event
enum ShadeOfTheHorsemanPhase
{
    SHADE_PHASE_ALL_CLEAR = 0,
    SHADE_PHASE_SPAWNED   = 1,
    SHADE_PHASE_LANDED    = 2,
    SHADE_PHASE_VICTORY   = 3,
};

enum ShadeOfTheHorsemanVillages
{
    SHADE_VILLAGE_GOLDSHIRE,
    SHADE_VILLAGE_KHARANOS,
    SHADE_VILLAGE_AZURE_WATCH,
    SHADE_VILLAGE_BRILL,
    SHADE_VILLAGE_RAZOR_HILL,
    SHADE_VILLAGE_FALCONWING_SQUARE,
    MAX_VILLAGES
};

enum
{
    TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE = 1000, // High number against collisions
    TYPE_SHADE_OF_THE_HORSEMAN_MAX = TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE + MAX_VILLAGES,
};

struct ShadeOfTheHorsemanData
{
    ShadeOfTheHorsemanData(std::vector<uint32> areaIds) : m_areaIds(areaIds) {}
    uint32 m_shadeOfTheHorsemanAttackPhases[MAX_VILLAGES];

    void Reset();

    uint32 GetDataAttackPhase(uint32 id) const;
    void SetDataAttackPhase(uint32 id, uint32 value);

    bool IsConditionFulfilled(uint32 conditionId, uint32 areaId) const;

    uint32 HandleGetData(uint32 id) const;
    void HandleSetData(uint32 id, uint32 value);

    static uint32 GetTypeFromZoneId(uint32 zoneId);

    std::vector<uint32> m_areaIds;
};

#endif
