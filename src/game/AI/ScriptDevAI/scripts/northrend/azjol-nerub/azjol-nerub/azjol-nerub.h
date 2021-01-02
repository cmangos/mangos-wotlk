/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_AZJOL_NERUB_H
#define DEF_AZJOL_NERUB_H

enum
{
    MAX_ENCOUNTER               = 3,
    MAX_GAUNTLET_SPELLS         = 3,
    MAX_GAUNTLET_SPAWNS         = 6,

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

    // initial gauntlet spawn
    NPC_ANUBAR_CRUSHER          = 28922,
    NPC_ANUBAR_CHAMPION         = 29117,
    NPC_ANUBAR_CRYPT_FIEND      = 29118,
    NPC_ANUBAR_NECROMANCER      = 29119,

    NPC_WORLD_TRIGGER           = 22515,        // trigger used in the Anubarak event
    NPC_WORLD_TRIGGER_LARGE     = 23472,        // trigger used in the Hadronox gauntlet event

    // Gauntlet mobs; position described as standing with the back at the main entrance
    NPC_ANUBAR_CHAMPION_1       = 29096,        // summoned by spell 53090 - lower stairs
    NPC_ANUBAR_CHAMPION_2       = 29062,        // summoned by spell 53064 - upper stairs
    NPC_ANUBAR_CHAMPION_3       = 28924,        // summoned by spell 53014 - attacks players
    NPC_ANUBAR_CRYPT_FIEND_1    = 29097,        // summoned by spell 53091 - lower stairs
    NPC_ANUBAR_CRYPT_FIEND_2    = 29063,        // summoned by spell 53065 - upper stairs
    NPC_ANUBAR_CRYPT_FIEND_3    = 29051,        // summoned by spell 53016 - attacks players
    NPC_ANUBAR_NECROMANCER_1    = 29098,        // summoned by spell 53092 - lower stairs
    NPC_ANUBAR_NECROMANCER_2    = 29064,        // summoned by spell 53066 - upper stairs
    NPC_ANUBAR_NECROMANCER_3    = 28925,        // summoned by spell 28925 - attacks players

    EMOTE_MOVE_TUNNEL           = -1601013,
    SAY_CRUSHER_AGGRO           = -1601025,
    SAY_CRUSHER_SPECIAL         = -1601026,

    // Gauntlet summon auras for the spider - summon a lot of spiders that will go down and fight Hadronox
    SPELL_SUMMON_CHAMPION_S     = 53035,        // triggers 53090 on lower stairs; 53064 on higher stairs
    SPELL_SUMMON_NECROMANCER_S  = 53036,        // triggers 53092 on lower stairs; 53066 on higher stairs
    SPELL_SUMMON_CRYPT_FIEND_S  = 53037,        // triggers 53091 on lower stairs; 53065 on higher stairs

    // Gauntlet summon spells for the players - cast by creature 23472 (tempspawn in front of the main entrance to the room) on a random trigger
    // These spells are engaged when players start attacking the main Anub'ar Crusher - this starts the gauntlet event and forces the spider to move up the tunnel
    SPELL_SUMMON_CHAMPION_P     = 53826,        // triggers 53014
    SPELL_SUMMON_NECROMANCER_P  = 53827,        // triggers 53015
    SPELL_SUMMON_CRYPT_FIEND_P  = 53828,        // triggers 53016

    SPELL_CHECK_RESET           = 53255,        // used by the spiders that attack players

    SPELL_WEB_FRONT_DOORS       = 53177,        // sends event 19101 - stops spiders from side upper doors (next to the chamber entrance)
    SPELL_WEB_SIDE_DOORS        = 53185,        // sends event 19102 - stops spiders from lower door (next to the tunnel)

    // Anubarak mobs
    NPC_IMPALE_TARGET           = 29184,
    NPC_ANUBAR_DARTER           = 29213,
    NPC_ANUBAR_ASSASSIN         = 29214,
    NPC_ANUBAR_GUARDIAN         = 29216,
    NPC_ANUBAR_VENOMANCER       = 29217,

    GO_DOOR_KRIKTHIR            = 192395,
    GO_DOOR_ANUBARAK_1          = 192396,
    GO_DOOR_ANUBARAK_2          = 192397,
    GO_DOOR_ANUBARAK_3          = 192398,

    ACHIEV_START_ANUB_ID        = 20381,

    ACHIEV_CRITERIA_WATCH_DIE   = 4240,         // Krikthir, achiev 1296
    ACHIEV_CRITERIA_DENIED      = 4244,         // Hadronox, achiev 1297
};

static const uint32 aWatchers[] = {NPC_GASHRA, NPC_NARJIL, NPC_SILTHIK};

static const uint32 aSpiderSummonSpells[MAX_GAUNTLET_SPELLS] = { SPELL_SUMMON_CHAMPION_S, SPELL_SUMMON_NECROMANCER_S, SPELL_SUMMON_CRYPT_FIEND_S };
static const uint32 aPlayerSummonSpells[MAX_GAUNTLET_SPELLS] = { SPELL_SUMMON_CHAMPION_P, SPELL_SUMMON_NECROMANCER_P, SPELL_SUMMON_CRYPT_FIEND_P };

struct AzjolNerubSummonData
{
    uint32 entry;
    float x, y, z, o;
    uint8 pathId;
};

static const AzjolNerubSummonData triggerSummonData[] =
{
    {NPC_WORLD_TRIGGER_LARGE, 530.495f, 597.0955f, 777.26337f, 5.9864f},
    {NPC_WORLD_TRIGGER_LARGE, 530.602f, 596.997f, 777.2633f, 5.4279f},
    {NPC_WORLD_TRIGGER_LARGE, 530.746f, 596.745f, 777.2631f,2.3038f},
};

static const AzjolNerubSummonData gauntletSummonData[] =
{
    {NPC_ANUBAR_CRUSHER,     583.9209f, 618.177f, 771.5359f, 3.5953f, 0},
    {NPC_ANUBAR_CRUSHER,     476.3354f, 619.261f, 771.5541f, 4.7472f, 1},
    {NPC_ANUBAR_CHAMPION,    477.2731f, 617.280f, 771.6137f, 4.8345f, 0},
    {NPC_ANUBAR_NECROMANCER, 476.2209f, 618.475f, 771.5714f, 4.5378f, 0},
    {NPC_ANUBAR_CRYPT_FIEND, 583.2123f, 617.354f, 771.5668f, 3.7524f, 0},
    {NPC_ANUBAR_NECROMANCER, 584.1617f, 618.482f, 771.5218f, 3.3161f, 1},
};

class instance_azjol_nerub : public ScriptedInstance
{
    public:
        instance_azjol_nerub(Map* pMap);

        void Initialize() override;

        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        ObjectGuid GetVenomancerTrigger() const { return m_venomancerSummonTarget; }
        ObjectGuid GetDarterTrigger() const { return m_darterSummonTarget; }

        void SetHadronoxDeniedAchievCriteria(bool bIsMet) { m_bHadronoxDenied = bIsMet; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

    private:
        void DoSendWatcherOrKrikthir();
        void ResetHadronoxTriggers();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        ObjectGuid m_playerGuid;

        // Hadronox triggers
        ObjectGuid m_playerTriggerGuid[MAX_GAUNTLET_SPELLS];
        ObjectGuid m_leftTargetTriggerGuid;
        ObjectGuid m_rightTargetTriggerGuid;
        GuidList m_lSpiderTriggersGuids;
        GuidList m_lCrusherGuids;

        // Anub triggers
        ObjectGuid m_darterSummonTarget;
        ObjectGuid m_guardianSummonTarget;
        ObjectGuid m_venomancerSummonTarget;
        ObjectGuid m_anubSummonTarget;

        uint32 m_uiWatcherTimer;
        uint32 m_uiPeriodicAuraTimer;
        uint32 m_uiPeriodicAuraStage;
        uint32 m_uiSpiderSummonTimer;

        bool m_bWatchHimDie;
        bool m_bHadronoxDenied;
};
#endif
