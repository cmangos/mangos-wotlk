/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ZULAMAN_H
#define DEF_ZULAMAN_H

enum InstanceZA
{
    MAX_ENCOUNTER           = 8,
    MAX_VENDOR              = 2,
    MAX_CHESTS              = 4,
    MAX_BEAR_WAVES          = 4,

    SAY_INST_RELEASE        = -1568067,                     // TODO Event NYI
    SAY_INST_BEGIN          = -1568068,
    SAY_INST_PROGRESS_1     = -1568069,
    SAY_INST_PROGRESS_2     = -1568070,
    SAY_INST_PROGRESS_3     = -1568071,
    SAY_INST_WARN_1         = -1568072,
    SAY_INST_WARN_2         = -1568073,
    SAY_INST_WARN_3         = -1568074,
    SAY_INST_WARN_4         = -1568075,
    SAY_INST_SACRIF1        = -1568076,
    SAY_INST_SACRIF2        = -1568077,
    SAY_INST_COMPLETE       = -1568078,

    // Bear event yells
    SAY_WAVE1_AGGRO         = -1568010,
    SAY_WAVE2_STAIR1        = -1568011,
    SAY_WAVE3_STAIR2        = -1568012,
    SAY_WAVE4_PLATFORM      = -1568013,

    WORLD_STATE_ID          = 3104,
    WORLD_STATE_COUNTER     = 3106,

    TYPE_EVENT_RUN          = 0,
    TYPE_AKILZON            = 1,
    TYPE_NALORAKK           = 2,
    TYPE_JANALAI            = 3,
    TYPE_HALAZZI            = 4,
    TYPE_MALACRASS          = 5,
    TYPE_ZULJIN             = 6,
    TYPE_RUN_EVENT_TIME     = 7,                            // Must be MAX_ENCOUNTER -1

    TYPE_RAND_VENDOR_1      = 8,
    TYPE_RAND_VENDOR_2      = 9,

    NPC_AKILZON             = 23574,
    NPC_NALORAKK            = 23576,
    NPC_JANALAI             = 23578,
    NPC_HALAZZI             = 23577,
    NPC_MALACRASS           = 24239,
    NPC_ZULJIN              = 23863,

    // Narolakk event npcs
    NPC_MEDICINE_MAN        = 23581,
    NPC_TRIBES_MAN          = 23582,
    NPC_AXETHROWER          = 23542,
    NPC_WARBRINGER          = 23580,

    // Malacrass companions
    NPC_ALYSON              = 24240,
    NPC_THURG               = 24241,
    NPC_SLITHER             = 24242,
    NPC_RADAAN              = 24243,
    NPC_GAZAKROTH           = 24244,
    NPC_FENSTALKER          = 24245,
    NPC_DARKHEART           = 24246,
    NPC_KORAGG              = 24247,

    NPC_HARRISON            = 24358,
    // Time Run Event NPCs
    NPC_TANZAR              = 23790,                        // at bear
    NPC_KRAZ                = 24024,                        // at phoenix
    NPC_ASHLI               = 24001,                        // at lynx
    NPC_HARKOR              = 23999,                        // at eagle
    // unused (TODO or TODO with DB-tools)
    NPC_TANZAR_CORPSE       = 24442,
    NPC_KRAZ_CORPSE         = 24444,
    NPC_ASHIL_CORPSE        = 24441,
    NPC_HARKOR_CORPSE       = 24443,

    // Zul'jin event spirits
    NPC_BEAR_SPIRIT         = 23878,                        // They should all have aura 42466
    NPC_EAGLE_SPIRIT        = 23880,
    NPC_LYNX_SPIRIT         = 23877,
    NPC_DRAGONHAWK_SPIRIT   = 23879,

    GO_STRANGE_GONG         = 187359,
    GO_MASSIVE_GATE         = 186728,
    GO_WIND_DOOR            = 186858,
    GO_LYNX_TEMPLE_ENTRANCE = 186304,
    GO_LYNX_TEMPLE_EXIT     = 186303,
    GO_HEXLORD_ENTRANCE     = 186305,
    GO_WOODEN_DOOR          = 186306,
    GO_FIRE_DOOR            = 186859,

    GO_TANZARS_TRUNK        = 186648,
    GO_KRAZS_PACKAGE        = 186667,
    GO_ASHLIS_BAG           = 186672,
    GO_HARKORS_SATCHEL      = 187021,
};

enum BossToChestIndex
{
    INDEX_NALORAKK          = 0,
    INDEX_JANALAI           = 1,
    INDEX_HALAZZI           = 2,
    INDEX_AKILZON           = 3
};

enum RunEventSteps
{
    RUN_START               = 1,
    RUN_FAIL                = 2,
    RUN_DONE                = 3,
    RUN_PROGRESS            = 4,
    RUN_FAIL_SOON           = 5
};

struct TimeEventNpcInfo
{
    TimeEventNpcInfo() : uiSavePosition(0) {}

    uint8 uiSavePosition;                                   // stores in what order this npc was saved (0 means unsaved)
    ObjectGuid npGuid;
};

struct NalorakkBearEventInfo
{
    int iYellId;
    float fX, fY, fZ, fO, fAggroDist;
};

static const NalorakkBearEventInfo aBearEventInfo[MAX_BEAR_WAVES] =
{
    {SAY_WAVE1_AGGRO,    0, 0, 0, 0, 45.0f},
    {SAY_WAVE2_STAIR1,   -54.948f, 1419.772f, 27.303f, 0.03f, 37.0f},
    {SAY_WAVE3_STAIR2,   -80.303f, 1372.622f, 40.764f, 1.67f, 35.0f},
    {SAY_WAVE4_PLATFORM, -77.495f, 1294.760f, 48.487f, 1.66f, 60.0f}
};

struct NalorakkTrashInfo
{
    GuidSet sBearTrashGuidSet;
    uint8 uiTrashKilled;
};

class instance_zulaman : public ScriptedInstance
{
    public:
        instance_zulaman(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool IsBearPhaseInProgress() { return m_bIsBearPhaseInProgress; }
        void SetBearEventProgress(bool bIsInProgress) { m_bIsBearPhaseInProgress = bIsInProgress; }
        void SendNextBearWave(Unit* pTarget);

        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        void Update(uint32 uiDiff) override;

    private:
        uint8 GetKilledPreBosses();
        void DoTimeRunSay(RunEventSteps uiData);
        void DoChestEvent(BossToChestIndex uiIndex);

        std::string m_strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        uint32 m_auiRandVendor[MAX_VENDOR];
        TimeEventNpcInfo m_aEventNpcInfo[MAX_CHESTS];

        uint32 m_uiEventTimer;
        uint32 m_uiGongCount;

        NalorakkTrashInfo m_aNalorakkEvent[MAX_BEAR_WAVES];
        uint8 m_uiBearEventPhase;
        bool m_bIsBearPhaseInProgress;
};

#endif
