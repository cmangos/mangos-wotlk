/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_AZJOL_NERUB_H
#define DEF_AZJOL_NERUB_H

enum
{
    MAX_ENCOUNTER               = 3,

    TYPE_KRIKTHIR               = 0,
    TYPE_HADRONOX               = 1,
    TYPE_ANUBARAK               = 2,

    NPC_KRIKTHIR                = 28684,
    NPC_HADRONOX                = 28921,
    NPC_ANUBARAK                = 29120,

    SAY_SEND_GROUP_1            = -1601004,
    SAY_SEND_GROUP_2            = -1601005,
    SAY_SEND_GROUP_3            = -1601006,

    NPC_GASHRA                  = 28730,
    NPC_NARJIL                  = 28729,
    NPC_SILTHIK                 = 28731,
    NPC_ANUBAR_CRUSHER          = 28922,

    NPC_WORLD_TRIGGER           = 22515,
    NPC_WORLD_TRIGGER_LARGE     = 23472,

    GO_DOOR_KRIKTHIR            = 192395,
    GO_DOOR_ANUBARAK_1          = 192396,
    GO_DOOR_ANUBARAK_2          = 192397,
    GO_DOOR_ANUBARAK_3          = 192398,

    SAY_CRUSHER_AGGRO           = -1601025,
    SAY_CRUSHER_SPECIAL         = -1601026,

    ACHIEV_START_ANUB_ID        = 20381,

    ACHIEV_CRITERIA_WATCH_DIE   = 4240,         // Krikthir, achiev 1296
    ACHIEV_CRITERIA_DENIED      = 4244,         // Hadronox, achiev 1297
};

static const uint32 aWatchers[] = {NPC_GASHRA, NPC_NARJIL, NPC_SILTHIK};

// Used to sort the summont triggers
static const int aSortDistance[4] = { -90, 10, 20, 30};

class instance_azjol_nerub : public ScriptedInstance
{
    public:
        instance_azjol_nerub(Map* pMap);

        void Initialize() override;

        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        ObjectGuid GetRandomAssassinTrigger();
        ObjectGuid GetGuardianTrigger() { return m_guardianSummonTarget; }
        ObjectGuid GetDarterTrigger() { return m_darterSummonTarget; }
        ObjectGuid GetAnubTrigger() { return m_anubSummonTarget; }

        void GetHadronoxTriggerList(GuidList& lList) { lList = m_lSpiderTriggersGuids; }
        void ResetHadronoxTriggers();

        void SetHadronoxDeniedAchievCriteria(bool bIsMet) { m_bHadronoxDenied = bIsMet; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff) override;

    private:
        void DoSendWatcherOrKrikthir();
        void DoSortWorldTriggers();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        ObjectGuid m_playerGuid;

        // Hadronox triggers
        GuidList m_lSpiderTriggersGuids;

        // Anub triggers
        ObjectGuid m_darterSummonTarget;
        ObjectGuid m_guardianSummonTarget;
        ObjectGuid m_anubSummonTarget;
        GuidVector m_vAssassinSummonTargetsVect;
        GuidList m_lTriggerGuids;

        uint32 m_uiWatcherTimer;
        uint32 m_uiGauntletEndTimer;

        bool m_bWatchHimDie;
        bool m_bHadronoxDenied;
        bool m_bGauntletStarted;
};
#endif
