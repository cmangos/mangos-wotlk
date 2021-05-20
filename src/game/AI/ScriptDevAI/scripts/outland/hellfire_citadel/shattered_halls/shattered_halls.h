/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SHATTERED_H
#define DEF_SHATTERED_H

enum
{
    MAX_ENCOUNTER               = 5,

    TYPE_NETHEKURSE             = 0,
    TYPE_OMROGG                 = 1,
    TYPE_BLADEFIST              = 2,                        // Note: if players skip Omrogg and go straight to Karagth then Omrogg comes to aid Karagth
    TYPE_EXECUTION              = 3,
    TYPE_GAUNTLET				= 4,

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

    // Gauntlet
    NPC_SHATTERED_HAND_ZEALOT   = 17462,
    NPC_SHATTERED_HAND_ARCHER   = 17427,
    NPC_SCOUT                   = 17693,
    NPC_BLOOD_GUARD             = 17461,
    NPC_PORUNG                  = 20923,

    SCOUT_AGGRO_YELL            = -1540051,
    PORUNG_FORM_RANKS_YELL      = -1540052,
    PORUNG_READY_YELL           = -1540053,
    PORUNG_AIM_YELL             = -1540054,
    PORUNG_FIRE_YELL            = -1540055,

    DELAY_350_MILLI             = 350,
    PORUNG_YELL_DELAY_1         = 4000,
    PORUNG_YELL_DELAY_2         = 1200,
    WAVE_TIMER                  = 20000,
    ARCHER_SHOOT_DELAY          = 15000,

    SHOOT_FLAME_ARROW           = 30952,

    GO_BLAZE                    = 181915,

    SPELL_FLAMES                = 30979,

    SPELL_KARGATH_EXECUTIONER_1 = 39288,                    // 55 min - first prisoner - officer
    SPELL_KARGATH_EXECUTIONER_2 = 39289,                    // 10 min - second prisoner
    SPELL_KARGATH_EXECUTIONER_3 = 39290,                    // 15 min - last prisoner

    // I'm not sure if these texts are used at the execution but this is most likely they are used to
    SAY_KARGATH_EXECUTE_ALLY    = -1540049,
    SAY_KARGATH_EXECUTE_HORDE   = -1540050,

    // AT_NETHEKURSE               = 4524,                  // Area trigger used for the execution event

    NPC_FLAME_ARROW             = 17687,
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
        void OnCreatureCreate(Creature* creature) override;
        void OnCreatureRespawn(Creature* creature) override;

        void OnCreatureDeath(Creature* creature) override;
        void OnCreatureEvade(Creature* creature) override;
        void OnCreatureEnterCombat(Creature* creature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        void GauntletReset();

        void DoInitialGets();

        void DoSummonInitialWave();

        void DoSummonSHZealot();

        void DoBeginArcherAttack(bool leftOrRight);

        void Update(const uint32 diff) override;

    private:
        void DoCastGroupDebuff(uint32 uiSpellId);
        void FailGauntlet();
        void StopGauntlet();
        void EndGauntlet();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiExecutionTimer;
        uint32 m_uiTeam;
        uint8 m_uiExecutionStage;
        uint8 m_uiPrisonersLeft;

        std::vector<ObjectGuid> m_vGauntletPermanentGuids;
        std::vector<ObjectGuid> m_vGauntletTemporaryGuids;
        std::vector<ObjectGuid> m_vGauntletBossGuids;

        std::vector<std::pair<ObjectGuid, uint32>> m_vBlazeTimers;

        bool m_bInitialWavesSpawned; // done spawning waves?
        bool m_bPorungDoneYelling;	 // done yelling?
        bool m_bZealotOneOrTwo;		 // delay is different whether spawning first or second zealot in wave
        bool m_gauntletStopped;

        Creature* m_porung;				   // normal or heroic this is him
        GuidList m_lSHArchers; // the two archers

        uint8 m_uiNumInitialWaves;			 // counter for initial waves spawning
        uint8 m_uiPorungYellNumber;			 // keeps track of porung as he yells
        uint32 m_uiInitialWaves_Delay;		 // time between initial waves spawn
        uint32 m_uiWaveTimer;				 // timer for periodic wave spawns
        uint32 m_uiPorungYellDelay;			 // delay between READY, AIM, FIRE
        uint32 m_uiShootFlamingArrowTimer_1; // timer for fire arrow ability (left archer)
        uint32 m_uiShootFlamingArrowTimer_2; // (right archer)
};

#endif
