/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_HYJAL_H
#define DEF_HYJAL_H

enum
{
    MAX_ENCOUNTER               = 5,

    TYPE_WINTERCHILL            = 0,
    TYPE_ANETHERON              = 1,
    TYPE_KAZROGAL               = 2,
    TYPE_AZGALOR                = 3,
    TYPE_ARCHIMONDE             = 4,

    TYPE_TRASH_COUNT            = 5,
    TYPE_RETREAT                = 6,

    WORLD_STATE_WAVES           = 2842,
    WORLD_STATE_ENEMY           = 2453,
    WORLD_STATE_ENEMYCOUNT      = 2454,

    NPC_JAINA                   = 17772,
    NPC_THRALL                  = 17852,
    NPC_TYRANDE                 = 17948,

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
    NPC_GIANT                   = 17908,
    NPC_STALK                   = 17916,

    NPC_WATER_ELEMENTAL         = 18001,
    NPC_DIRE_WOLF               = 17854,

    GO_ANCIENT_GEM              = 185557,
};

static const float aArchimondeSpawnLoc[4] = {5581.49f, -3445.63f, 1575.1f, 3.905f};

class instance_mount_hyjal : public ScriptedInstance
{
    public:
        instance_mount_hyjal(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strSaveData.c_str(); }
        void Load(const char* chrIn) override;

    private:
        void DoSpawnArchimonde();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strSaveData;

        GuidList lAncientGemGUIDList;

        uint32 m_uiTrashCount;
};

#endif
