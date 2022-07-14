/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_HYJAL_H
#define DEF_HYJAL_H

#include "World/WorldStateDefines.h"
#include "Chat/Chat.h"
#include <array>

enum
{
    MAX_ENCOUNTER               = 5,

    TYPE_WINTERCHILL            = 0,
    TYPE_ANETHERON              = 1,
    TYPE_KAZROGAL               = 2,
    TYPE_AZGALOR                = 3,
    TYPE_ARCHIMONDE             = 4,

    TYPE_WIN                    = 5,

    // Alliance base
    NPC_JAINA                   = 17772,
    NPC_GUARDIAN_WATER_ELEMENTAL= 18001,
    NPC_ALLIANCE_PEASANT        = 17931,
    NPC_ALLIANCE_KNIGHT         = 17920,
    NPC_ALLIANCE_FOOTMAN        = 17919,
    NPC_ALLIANCE_RIFLEMAN       = 17921,
    NPC_ALLIANCE_PRIEST         = 17928,
    NPC_ALLIANCE_SORCERESS      = 17922,
    NPC_HYJAL_DESPAWN_TRIGGER   = 18410,
    NPC_BUILDING_TRIGGER        = 18304,

    // Horde base
    NPC_THRALL                  = 17852,
    NPC_DIRE_WOLF               = 17854,
    NPC_HORDE_HEADHUNTER        = 17934,
    NPC_HORDE_SHAMAN            = 17936,
    NPC_HORDE_GRUNT             = 17932,
    NPC_HORDE_HEALING_WARD      = 18036,
    NPC_TAUREN_WARRIOR          = 17933,
    NPC_HORDE_WITCH_DOCTOR      = 17935,
    NPC_HORDE_PEON              = 17937,
    NPC_INFERNAL_RELAY          = 18242,
    NPC_INFERNAL_TARGET         = 21075,

    // Elf base
    NPC_TYRANDE                 = 17948,
    NPC_NELF_WISP               = 18502,
    NPC_DRUID_OF_THE_TALON      = 3794,
    NPC_DRUID_OF_THE_CLAW       = 3795,
    NPC_NELF_ANCIENT_PROT       = 18487,
    NPC_NELF_ANCIENT_OF_LORE    = 18486,
    NPC_NELF_ANCIENT_OF_WAR     = 18485,
    NPC_NELF_ARCHER             = 17943,
    NPC_NELF_HUNTRESS           = 17945,
    NPC_DRYAD                   = 17944,

    // Bosses summoned after every 8 waves
    NPC_WINTERCHILL             = 17767,
    NPC_ANETHERON               = 17808,
    NPC_KAZROGAL                = 17888,
    NPC_AZGALOR                 = 17842,
    NPC_ARCHIMONDE              = 17968,

    // Trash Mobs summoned in waves
    NPC_NECRO                   = 17899,
    NPC_ABOMI                   = 17898,
    NPC_GHOUL                   = 17895,
    NPC_BANSH                   = 17905,
    NPC_CRYPT                   = 17897,
    NPC_GARGO                   = 17906,
    NPC_FROST                   = 17907,
    NPC_INFERNAL                = 17908,
    NPC_STALK                   = 17916,

    // Final Archimonde event
    NPC_ANCIENT_WISP            = 17946,

    // Anetheron infernal
    NPC_TOWERING_INFERNAL       = 17818,
    // Azgalor Doomguard
    NPC_LESSER_DOOMGUARD        = 17864,

    GO_ANCIENT_GEM              = 185557,
    GO_HORDE_ENCAMPMENT_PORTAL  = 182060,
    GO_NIGHT_ELF_VILLAGE_PORTAL = 182061,
    GO_ROARING_FLAME            = 182260,

    MAX_WAVES                   = 36,
    MAX_INVASION_WAVES          = 17,
    MAX_CONCURRENT_SPAWNS       = 30,

    SPELL_SUMMON_INFERNAL       = 32148,

    SPELL_SOUL_CHARGE_YELLOW_CHARGE = 32045,
    SPELL_SOUL_CHARGE_GREEN_CHARGE  = 32051,
    SPELL_SOUL_CHARGE_RED_CHARGE    = 32052,

    SPELL_CHOOSE_TARGET_UNUSED      = 32121, // Currently unused. Unknown what its purpose is, but it's cast by Druid of the Claw c.3795

    // Used for Building creature (Base Invasion/Overrun events)
    SPELL_SUPER_INVIS           = 8149,
    FACTION_SPAR                = 1692,
    FACTION_SPAR_BUDDY          = 1693,
};

enum BaseArea
{
    BASE_ALLY  = 0,
    BASE_HORDE = 1,
    BASE_ELF   = 2,
    MAX_BASE   = 3,
};

enum WavePaths
{
    PATH_NONE           = 0, // For stationary temp-spawned triggers
    ALLY_GROUND         = 1, // Normal ground mobs in Alliance Base
    HORDE_GROUND        = 2, // Normal ground mobs in Horde Base
    HORDE_GARGOYLE_1    = 3, // Gargoyles spawned behind the mountains close to the troll camp
    HORDE_GARGOYLE_2    = 4, // Gargoyles spawned behind the mountains close to the gate
    HORDE_FROST_WYRM_1  = 5, // Frost Wyrm spawned behind the mountains close to the troll camp
    HORDE_FROST_WYRM_2  = 6, // Frost Wyrm spawned behind the mountains close to the gate
    NELF_GROUND         = 7, // Waves spawned during Archimonde fight
};

enum HyjalEvents
{
    JAINA_FIRST_BOSS,
    JAINA_SECOND_BOSS,
    JAINA_WIN,
    THRALL_FIRST_BOSS,
    THRALL_SECOND_BOSS,
    THRALL_WIN,
};

struct HyjalWaveMob
{
    uint32 mobEntry;
    float x, y, z, ori;
    WavePaths path;
};

struct HyjalWave
{
    std::vector<HyjalWaveMob> waveMobs;
    uint32 waveTimer;                                     // The timer before the next wave is summoned
};

struct InvasionWave
{
    std::vector<HyjalWaveMob> waveMobs;
    uint32 invasionTimer;                                 // The timer before the next wave is summoned
};


class instance_mount_hyjal : public ScriptedInstance
{
    friend class ChatHandler;
    public:
        instance_mount_hyjal(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnPlayerDeath(Player* player) override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* creature) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        // void OnCreatureEvade(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        const char* Save() const override { return m_saveData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

        void StartEvent(HyjalEvents eventId);
        void FailEvent();
        GuidVector GetOverrunSpawns(uint32 baseId) { return m_overrunSpawns[baseId]; }

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void SpawnNextWave();
        void SpawnNextInvasionWave();
        void SpawnWave(uint32 index, bool setTimer);
        void SpawnWaveInfernals(uint32 index);
        void SpawnInvasionInfernals(uint32 index);
        void SpawnInfernals(uint32 count);
        void SpawnInfernal();
        void SpawnOres(BaseArea index);
        void SpawnInvasionWave(uint32 index, bool setTimer);
        void SpawnArchimonde();
        void DespawnWaveSpawns();
        void DespawnBase(BaseArea index);
        void SpawnBase(BaseArea index, bool spawnLeader);
        void RetreatBase(BaseArea index);
        void OverrunBase(BaseArea index);
        void DespawnOverrun(BaseArea index);
        void RestartEvent();

        uint32 m_encounter[MAX_ENCOUNTER];
        std::string m_saveData;

        GuidVector m_ancientGems[MAX_BASE - 1];
        GuidVector m_roaringFlames[MAX_BASE - 1];
        GuidVector m_waveSpawns;
        GuidVector m_baseSpawns[MAX_BASE];
        GuidVector m_overrunSpawns[MAX_BASE];

        GuidVector m_additionalSpawns;

        uint32 m_hyjalWaves; // Script value
        uint32 m_invasionWaves;

        uint32 m_nextWaveTimer;
        uint32 m_nextInvasionWaveTimer;
        uint32 m_retreatTimer;
        uint32 m_infernalsTimer;
        uint32 m_infernalsCounter;

        uint32 m_failTimer;

        bool m_startCheck;

        static std::array<HyjalWave, MAX_WAVES> m_hyjalWavesData;
        static std::array<InvasionWave, MAX_INVASION_WAVES> m_invasionWavesData;

        // infernals data holders
        GuidVector m_infernalRelays;
        GuidVector m_infernalTargets;
        std::vector<uint32> m_infernalIndices;
};

#endif
