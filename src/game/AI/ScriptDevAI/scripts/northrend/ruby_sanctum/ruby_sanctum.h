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

    // npcs used in the Halion encounter
    NPC_COMBUSTION                  = 40001,
    NPC_CONSUMPTION                 = 40135,

    NPC_SHADOW_ORB_1                = 40083,            // shadow orbs for Halion encounter
    NPC_SHADOW_ORB_2                = 40100,
    NPC_SHADOW_ORB_3                = 40468,            // heroic only
    NPC_SHADOW_ORB_4                = 40469,            // heroic only

    NPC_ORB_CARRIER                 = 40081,            // vehicle for shadow orbs; has 2 or 4 shadow orbs boarded; boarding handled by DB
    NPC_ORB_ROTATION_FOCUS          = 40091,            // npc that moves in a circle, around the center of the boss arena; the orb carrier will alter rotation based on the position of this npc

    NPC_METEOR_STRIKE_MAIN          = 40029,            // summons the other meteor strikes using serverside spells like 74680, 74681, 74682, 74683
    NPC_METEOR_STRIKE_1             = 40041,            // Npc 40029 summons the first 4 secondary meteor strike npcs around the main one; then each of them summons one 40055 towards the back using serverside spells 74687, 74688
    NPC_METEOR_STRIKE_2             = 40042,
    NPC_METEOR_STRIKE_3             = 40043,
    NPC_METEOR_STRIKE_4             = 40044,
    NPC_METEOR_STRIKE_FLAME         = 40055,            // Each npc 40055 summons other 10 40055 npcs towards the back of the creature

    NPC_SAVIANA                     = 39747,            // minibosses
    NPC_BALTHARUS                   = 39751,
    NPC_ZARITHRIAN                  = 39746,

    NPC_XERESTRASZA                 = 40429,            // friendly npc, used for some cinematic and quest
    NPC_ZARITHRIAN_SPAWN_STALKER    = 39794,

    // Portals used in the Halion encounter
    GO_TWILIGHT_PORTAL_ENTER_1      = 202794,           // uses spell 75074; summoned by 74809
    GO_TWILIGHT_PORTAL_ENTER_2      = 202795,           // uses spell 75074; static spawn; used in the 3rd encounter phase
    GO_TWILIGHT_PORTAL_LEAVE        = 202796,           // uses spell 74812; static spawn; used in the 3rd encounter phase

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
        void OnCreatureRespawn(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(const uint32 diff) override;

        void GetSpawnStalkersGuidList(GuidList& lList) const { lList = m_lSpawnStalkersGuidList; }
        void GetPortalsGuidList(GuidList& lPortals) const { lPortals = m_lTwilightPortalsGuidList; }

        const char* Save() const override { return strInstData.c_str(); }
        void Load(const char* chrIn) override;

        // Difficulty wrappers
        bool IsHeroicDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }
        bool Is25ManDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }

    protected:
        void DoHandleZarithrianDoor();

        std::string strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];

        uint32 m_uiHalionSummonTimer;
        uint32 m_uiHalionSummonStage;
        uint32 m_uiHalionResetTimer;

        GuidList m_lSpawnStalkersGuidList;
        GuidList m_lTwilightPortalsGuidList;
};

#endif
