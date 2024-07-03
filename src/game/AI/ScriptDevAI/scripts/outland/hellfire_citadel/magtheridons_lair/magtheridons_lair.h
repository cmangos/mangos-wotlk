/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_MAGTHERIDONS_LAIR_H
#define DEF_MAGTHERIDONS_LAIR_H

enum
{
    MAX_ENCOUNTER               = 2,

    TYPE_MAGTHERIDON_EVENT      = 0,
    TYPE_CHANNELER_EVENT        = 1,

    NPC_MAGTHERIDON             = 17257,
    NPC_CHANNELER               = 17256,
    NPC_BURNING_ABYSSAL         = 17454,

    GO_MANTICRON_CUBE           = 181713,
    GO_DOODAD_HF_MAG_DOOR01     = 183847,
    GO_DOODAD_HF_RAID_FX01      = 184653,
    GO_MAGTHERIDON_COLUMN_003   = 184634,
    GO_MAGTHERIDON_COLUMN_002   = 184635,
    GO_MAGTHERIDON_COLUMN_004   = 184636,
    GO_MAGTHERIDON_COLUMN_005   = 184637,
    GO_MAGTHERIDON_COLUMN_000   = 184638,
    GO_MAGTHERIDON_COLUMN_001   = 184639,
    GO_MAGHERIDON_BLAZE         = 181832,

    SPAWN_GROUP_CHANNELER       = 5440005,
};

static const int32 aRandomTaunt[] = { 17339, 17340, 17341, 17342, 17343, 17344 };

class instance_magtheridons_lair : public ScriptedInstance
{
    public:
        instance_magtheridons_lair(Map* pMap);

        void Initialize() override;

        bool IsEncounterInProgress() const override;

        void FailBoss();

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(const uint32 diff) override;

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];

        std::vector<uint32> m_lChannelerGuidList;
        GuidVector m_abyssalTemporaryGuids;

        GuidList m_lColumnGuidList;
        GuidList m_lCubeGuidList;
        GuidList m_lBlazeGuidList;

        uint32 m_uiRandYellTimer;
        uint32 m_uiCageBreakTimer;
        uint8 m_uiCageBreakStage;
};

#endif
