/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_DRAKTHARON_KEEP_H
#define DEF_DRAKTHARON_KEEP_H

enum
{
    MAX_ENCOUNTER                   = 4,

    TYPE_TROLLGORE                  = 0,
    TYPE_NOVOS                      = 1,
    TYPE_KING_DRED                  = 2,
    TYPE_THARONJA                   = 3,

    NPC_NOVOS                       = 26631,
    NPC_KING_DRED                   = 27483,

    // Adds of King Dred Encounter - deaths counted for achievement
    NPC_DRAKKARI_GUTRIPPER          = 26641,
    NPC_DRAKKARI_SCYTHECLAW         = 26628,
    NPC_WORLD_TRIGGER               = 22515,

    // Novos Encounter
    SPELL_BEAM_CHANNEL              = 52106,
    SPELL_CRYSTAL_HANDLER_DEATH_1   = 47336,
    SPELL_CRYSTAL_HANDLER_DEATH_2   = 55801,
    SPELL_CRYSTAL_HANDLER_DEATH_3   = 55803,
    SPELL_CRYSTAL_HANDLER_DEATH_4   = 55805,

    MAX_CRYSTALS                    = 4,
    NPC_CRYSTAL_CHANNEL_TARGET      = 26712,
    GO_CRYSTAL_SW                   = 189299,
    GO_CRYSTAL_NE                   = 189300,
    GO_CRYSTAL_NW                   = 189301,
    GO_CRYSTAL_SE                   = 189302,

    // Achievement Criterias to be handled with SD2
    ACHIEV_CRIT_BETTER_OFF_DREAD    = 7318,
    ACHIEV_CRIT_CONSUME_JUNCTION    = 7579,
    ACHIEV_CRIT_OH_NOVOS            = 7361,
};

static const uint32 aCrystalHandlerDeathSpells[MAX_CRYSTALS] =
{SPELL_CRYSTAL_HANDLER_DEATH_1, SPELL_CRYSTAL_HANDLER_DEATH_2, SPELL_CRYSTAL_HANDLER_DEATH_3, SPELL_CRYSTAL_HANDLER_DEATH_4};

struct NovosCrystalInfo
{
    ObjectGuid m_crystalGuid;
    ObjectGuid m_channelGuid;
    bool m_bWasUsed;
};

class instance_draktharon_keep : public ScriptedInstance
{
    public:
        instance_draktharon_keep(Map* pMap);
        ~instance_draktharon_keep() {}

        void Initialize() override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void GetTrollgoreOutsideTriggers(GuidVector& vTriggers) { vTriggers = m_vTriggerGuids; }
        ObjectGuid GetTrollgoreCornerTrigger() { return m_trollgoreCornerTriggerGuid; }

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        Creature* GetNextCrystalTarget(Creature* pCrystalHandler, uint8& uiIndex);
        void DoHandleCrystal(uint8 uiIndex);
        Creature* GetSummonDummy();

    protected:
        void DoSortNovosDummies();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiDreadAddsKilled;
        bool m_bNovosAddGrounded;
        bool m_bTrollgoreConsume;

        ObjectGuid m_novosChannelGuid;
        ObjectGuid m_trollgoreCornerTriggerGuid;

        NovosCrystalInfo m_aNovosCrystalInfo[MAX_CRYSTALS];

        GuidVector m_vSummonDummyGuids;
        GuidList m_lNovosDummyGuids;
        GuidVector m_vTriggerGuids;
};

#endif
