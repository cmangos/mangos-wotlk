/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SHATTERED_H
#define DEF_SHATTERED_H

enum
{
    MAX_ENCOUNTER               = 4,

    TYPE_NETHEKURSE             = 0,
    TYPE_OMROGG                 = 1,
    TYPE_BLADEFIST              = 2,                        // Note: if players skip Omrogg and go straight to Karagth then Omrogg comes to aid Karagth
    TYPE_EXECUTION              = 3,

    NPC_NETHEKURSE              = 16807,
    NPC_KARGATH_BLADEFIST       = 16808,
    NPC_EXECUTIONER             = 17301,                    // must be killed for the executioner event

    NPC_SOLDIER_ALLIANCE_1      = 17288,                    // quest giver for 9524
    NPC_SOLDIER_ALLIANCE_2      = 17289,
    NPC_SOLDIER_ALLIANCE_3      = 17292,
    NPC_OFFICER_ALLIANCE        = 17290,                    // quest objective

    NPC_SOLDIER_HORDE_1         = 17294,                    // quest giver for 9525
    NPC_SOLDIER_HORDE_2         = 17295,
    NPC_SOLDIER_HORDE_3         = 17297,
    NPC_OFFICER_HORDE           = 17296,                    // quest objective

    GO_NETHEKURSE_DOOR          = 182540,
    GO_NETHEKURSE_ENTER_DOOR    = 182539,

    SPELL_KARGATH_EXECUTIONER_1 = 39288,                    // 55 min - first prisoner - officer
    SPELL_KARGATH_EXECUTIONER_2 = 39289,                    // 10 min - second prisoner
    SPELL_KARGATH_EXECUTIONER_3 = 39290,                    // 15 min - last prisoner

    // I'm not sure if these texts are used at the execution but this is most likely they are used to
    SAY_KARGATH_EXECUTE_ALLY    = -1540049,
    SAY_KARGATH_EXECUTE_HORDE   = -1540050,

    // AT_NETHEKURSE               = 4524,                  // Area trigger used for the execution event
};

struct SpawnLocation
{
    uint32 m_uiAllianceEntry, m_uiHordeEntry;
    float m_fX, m_fY, m_fZ, m_fO;
};

const float afExecutionerLoc[4] = {151.443f, -84.439f, 1.938f, 6.283f};

static SpawnLocation aSoldiersLocs[] =
{
    {0,                      NPC_SOLDIER_HORDE_1, 119.609f, 256.127f, -45.254f, 5.133f},
    {NPC_SOLDIER_ALLIANCE_1, 0,                   131.106f, 254.520f, -45.236f, 3.951f},
    {NPC_SOLDIER_ALLIANCE_3, NPC_SOLDIER_HORDE_3, 151.040f, -91.558f, 1.936f, 1.559f},
    {NPC_SOLDIER_ALLIANCE_2, NPC_SOLDIER_HORDE_2, 150.669f, -77.015f, 1.933f, 4.705f},
    {NPC_OFFICER_ALLIANCE,   NPC_OFFICER_HORDE,   138.241f, -84.198f, 1.907f, 0.055f}
};

class instance_shattered_halls : public ScriptedInstance
{
    public:
        instance_shattered_halls(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;

        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureEnterCombat(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        void Update(uint32 uiDiff) override;

    private:
        void DoCastGroupDebuff(uint32 uiSpellId);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiExecutionTimer;
        uint32 m_uiTeam;
        uint8 m_uiExecutionStage;
};

#endif
