/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_UTG_KEEP_H
#define DEF_UTG_KEEP_H

enum
{
    MAX_ENCOUNTER               = 6,

    TYPE_KELESETH               = 0,
    TYPE_SKARVALD_DALRONN       = 1,
    TYPE_INGVAR                 = 2,
    TYPE_BELLOW_1               = 3,
    TYPE_BELLOW_2               = 4,
    TYPE_BELLOW_3               = 5,

    NPC_KELESETH                = 23953,
    NPC_SKARVALD                = 24200,
    NPC_DALRONN                 = 24201,
    NPC_INGVAR                  = 23954,

    NPC_FROST_TOMB              = 23965,

    GO_BELLOW_1                 = 186688,
    GO_BELLOW_2                 = 186689,
    GO_BELLOW_3                 = 186690,
    GO_FORGEFIRE_1              = 186692,
    GO_FORGEFIRE_2              = 186693,
    GO_FORGEFIRE_3              = 186691,
    GO_PORTCULLIS_COMBAT        = 186612,
    GO_PORTCULLIS_EXIT_1        = 186694,
    GO_PORTCULLIS_EXIT_2        = 186756,

    ACHIEV_CRIT_ON_THE_ROCKS    = 7231,
};

class instance_utgarde_keep : public ScriptedInstance
{
    public:
        instance_utgarde_keep(Map* pMap);
        ~instance_utgarde_keep() {}

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_bKelesethAchievFailed;
};

#endif
