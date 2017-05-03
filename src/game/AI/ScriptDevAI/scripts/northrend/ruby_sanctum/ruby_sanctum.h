/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_RUBY_SANCTUM_H
#define DEF_RUBY_SANCTUM_H

enum
{
    MAX_ENCOUNTER                   = 4,

    TYPE_SAVIANA                    = 0,
    TYPE_BALTHARUS                  = 1,
    TYPE_ZARITHRIAN                 = 2,
    TYPE_HALION                     = 3,

    NPC_HALION_REAL                 = 39863,            // Halion - Physical Realm NPC
    NPC_HALION_TWILIGHT             = 40142,            // Halion - Twilight Realm NPC
    NPC_HALION_CONTROLLER           = 40146,

    NPC_SHADOW_ORB_1                = 40083,            // shadow orbs for Halion encounter
    NPC_SHADOW_ORB_2                = 40100,
    NPC_SHADOW_ORB_3                = 40468,            // heroic only
    NPC_SHADOW_ORB_4                = 40469,            // heroic only

    NPC_SAVIANA                     = 39747,            // minibosses
    NPC_BALTHARUS                   = 39751,
    NPC_ZARITHRIAN                  = 39746,

    NPC_XERESTRASZA                 = 40429,            // friendly npc, used for some cinematic and quest
    NPC_ZARITHRIAN_SPAWN_STALKER    = 39794,

    GO_TWILIGHT_PORTAL_ENTER_1      = 202794,           // Portals used in the Halion encounter
    GO_TWILIGHT_PORTAL_ENTER_2      = 202795,
    GO_TWILIGHT_PORTAL_LEAVE        = 202796,

    GO_FIRE_FIELD                   = 203005,           // Xerestrasza flame door
    GO_FLAME_WALLS                  = 203006,           // Zarithrian flame walls
    GO_FLAME_RING                   = 203007,           // Halion flame ring
    GO_TWILIGHT_FLAME_RING          = 203624,           // Halion flame ring - twilight version

    GO_BURNING_TREE_1               = 203036,           // Trees which burn when Halion appears
    GO_BURNING_TREE_2               = 203037,
    GO_BURNING_TREE_3               = 203035,
    GO_BURNING_TREE_4               = 203034,

    // Spells used to summon Halion
    SPELL_FIRE_PILLAR               = 76006,
    SPELL_FIERY_EXPLOSION           = 76010,
    SPELL_CLEAR_DEBUFFS             = 75396,            // cast by the controller on encounter reset

    SAY_HALION_SPAWN                = -1724024,

    // world state to show corporeality in Halion encounter - phase 3
    WORLD_STATE_CORP_PHYSICAL       = 5049,
    WORLD_STATE_CORP_TWILIGHT       = 5050,
    WORLD_STATE_CORPOREALITY        = 5051,
};

class instance_ruby_sanctum : public ScriptedInstance
{
    public:
        instance_ruby_sanctum(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(uint32 uiDiff) override;

        void GetSpawnStalkersGuidList(GuidList& lList) { lList = m_lSpawnStalkersGuidList; }

        const char* Save() const override { return strInstData.c_str(); }
        void Load(const char* chrIn) override;

        // Difficulty wrappers
        bool IsHeroicDifficulty() { return instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }
        bool Is25ManDifficulty() { return instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }

    protected:
        void DoHandleZarithrianDoor();

        std::string strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];

        uint32 m_uiHalionSummonTimer;
        uint32 m_uiHalionSummonStage;
        uint32 m_uiHalionResetTimer;

        GuidList m_lSpawnStalkersGuidList;
};

#endif
