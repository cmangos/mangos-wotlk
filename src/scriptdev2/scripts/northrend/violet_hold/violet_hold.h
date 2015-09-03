/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_VIOLET_H
#define DEF_VIOLET_H

enum
{
    MAX_ENCOUNTER               = 10,
    MAX_MINIBOSSES              = 6,

    TYPE_MAIN                   = 0,
    TYPE_SEAL                   = 1,
    TYPE_PORTAL                 = 2,
    TYPE_LAVANTHOR              = 3,
    TYPE_MORAGG                 = 4,
    TYPE_EREKEM                 = 5,
    TYPE_ICHORON                = 6,
    TYPE_XEVOZZ                 = 7,
    TYPE_ZURAMAT                = 8,
    TYPE_CYANIGOSA              = 9,

    WORLD_STATE_ID              = 3816,
    WORLD_STATE_SEAL            = 3815,
    WORLD_STATE_PORTALS         = 3810,

    GO_INTRO_CRYSTAL            = 193615,
    GO_PRISON_CRYSTAL           = 193611,
    GO_PRISON_SEAL_DOOR         = 191723,

    GO_CELL_LAVANTHOR           = 191566,
    GO_CELL_MORAGG              = 191606,
    GO_CELL_ZURAMAT             = 191565,
    GO_CELL_EREKEM              = 191564,
    GO_CELL_EREKEM_GUARD_L      = 191563,
    GO_CELL_EREKEM_GUARD_R      = 191562,
    GO_CELL_XEVOZZ              = 191556,
    GO_CELL_ICHORON             = 191722,

    NPC_EVENT_CONTROLLER        = 30883,
    NPC_PORTAL_INTRO            = 31011,
    NPC_PORTAL                  = 30679,
    NPC_PORTAL_ELITE            = 32174,
    NPC_DOOR_SEAL               = 30896,

    NPC_SINCLARI                = 30658,
    NPC_SINCLARI_ALT            = 32204,                    // yeller for seal weakening and summoner for portals
    NPC_HOLD_GUARD              = 30659,

    NPC_EREKEM                  = 29315,
    NPC_EREKEM_GUARD            = 29395,
    NPC_MORAGG                  = 29316,
    NPC_ICHORON                 = 29313,
    NPC_XEVOZZ                  = 29266,
    NPC_LAVANTHOR               = 29312,
    NPC_ZURAMAT                 = 29314,
    NPC_CYANIGOSA               = 31134,

    NPC_PORTAL_GUARDIAN         = 30660,
    NPC_PORTAL_KEEPER           = 30695,

    NPC_AZURE_INVADER           = 30661,
    NPC_AZURE_SPELLBREAKER      = 30662,
    NPC_AZURE_BINDER            = 30663,
    NPC_AZURE_MAGE_SLAYER       = 30664,
    NPC_MAGE_HUNTER             = 30665,
    NPC_AZURE_CAPTAIN           = 30666,
    NPC_AZURE_SORCEROR          = 30667,
    NPC_AZURE_RAIDER            = 30668,
    NPC_AZURE_STALKER           = 32191,

    NPC_VOID_SENTRY             = 29364,                    // Npc checked for Zuramat achiev
    NPC_ICHORON_SUMMON_TARGET   = 29326,                    // Npc which summons the Ichoron globules

    // used for intro
    NPC_AZURE_BINDER_INTRO      = 31007,
    NPC_AZURE_INVADER_INTRO     = 31008,
    NPC_AZURE_SPELLBREAKER_INTRO = 31009,
    NPC_AZURE_MAGE_SLAYER_INTRO = 31010,

    NPC_AZURE_SABOTEUR          = 31079,

    NPC_DEFENSE_SYSTEM          = 30837,
    NPC_DEFENSE_DUMMY_TARGET    = 30857,

    // 'Ghosts' for Killed mobs after Wipe
    NPC_ARAKKOA                 = 32226,
    NPC_ARAKKOA_GUARD           = 32228,
    NPC_VOID_LORD               = 32230,
    NPC_ETHERAL                 = 32231,
    NPC_SWIRLING                = 32234,
    NPC_WATCHER                 = 32235,
    NPC_LAVA_HOUND              = 32237,

    SPELL_DEFENSE_SYSTEM_VISUAL = 57887,
    SPELL_DEFENSE_SYSTEM_SPAWN  = 57886,
    SPELL_LIGHTNING_INTRO       = 60038,                    // intro kill spells, also related to spell 58152
    SPELL_ARCANE_LIGHTNING      = 57930,                    // damage spells, related to spell 57912

    SPELL_DESTROY_DOOR_SEAL     = 58040,                    // spell periodic cast by misc
    SPELL_TELEPORTATION_PORTAL  = 57687,                    // visual aura, but possibly not used? creature_template model for portals are same

    SPELL_SHIELD_DISRUPTION     = 58291,                    // dummy when opening a cell
    SPELL_SIMPLE_TELEPORT       = 12980,                    // used after a cell has been opened - not sure if the id is correct

    SPELL_PORTAL_PERIODIC       = 58008,                    // most likely the tick for each summon (tick each 15 seconds)
    SPELL_PORTAL_CHANNEL        = 58012,                    // the blue "stream" between portal and guardian/keeper
    SPELL_PORTAL_BEAM           = 56046,                    // large beam, unsure if really used here (or possible for something different)

    SPELL_PORTAL_VISUAL_1       = 57872,                    // no idea, but is possibly related based on it's visual appearence
    SPELL_PORTAL_VISUAL_2       = 57630,

    SAY_SEAL_75                 = -1608002,
    SAY_SEAL_50                 = -1608003,
    SAY_SEAL_5                  = -1608004,

    SAY_RELEASE_EREKEM          = -1608008,
    SAY_RELEASE_ICHORON         = -1608009,
    SAY_RELEASE_XEVOZZ          = -1608010,
    SAY_RELEASE_ZURAMAT         = -1608011,

    EMOTE_GUARDIAN_PORTAL       = -1608005,
    EMOTE_DRAGONFLIGHT_PORTAL   = -1608006,
    EMOTE_KEEPER_PORTAL         = -1608007,

    MAX_NORMAL_PORTAL           = 8,

    ACHIEV_CRIT_DEFENSELES      = 6803,                     // event achiev - 1816
    ACHIEV_CRIT_DEHYDRATATION   = 7320,                     // Ichoron achiev - 2041
    ACHIEV_CRIT_VOID_DANCE      = 7587,                     // Zuramat achiev - 2153
};

static const float fDefenseSystemLoc[4] = {1888.146f, 803.382f, 58.604f, 3.072f};
static const float fGuardExitLoc[3] = {1806.955f, 803.851f, 44.36f};
static const float fSealAttackLoc[3] = {1858.027f, 804.11f, 44.008f};

static const uint32 aRandomPortalNpcs[5] = {NPC_AZURE_INVADER, NPC_MAGE_HUNTER, NPC_AZURE_SPELLBREAKER, NPC_AZURE_BINDER, NPC_AZURE_MAGE_SLAYER};
static const uint32 aRandomIntroNpcs[4] = {NPC_AZURE_BINDER_INTRO, NPC_AZURE_INVADER_INTRO, NPC_AZURE_SPELLBREAKER_INTRO, NPC_AZURE_MAGE_SLAYER_INTRO};

static const int32 aSealWeakYell[3] = {SAY_SEAL_75, SAY_SEAL_50, SAY_SEAL_5};

enum ePortalType
{
    PORTAL_TYPE_NORM = 0,
    PORTAL_TYPE_SQUAD,
    PORTAL_TYPE_BOSS,
};

struct PortalData
{
    ePortalType pPortalType;
    float fX, fY, fZ, fOrient;
};

static const PortalData afPortalLocation[] =
{
    {PORTAL_TYPE_NORM, 1936.07f, 803.198f, 53.3749f, 3.1241f},  // balcony
    {PORTAL_TYPE_NORM, 1877.51f, 850.104f, 44.6599f, 4.7822f},  // erekem
    {PORTAL_TYPE_NORM, 1890.64f, 753.471f, 48.7224f, 1.7104f},  // moragg
    {PORTAL_TYPE_SQUAD, 1911.06f, 802.103f, 38.6465f, 2.8908f}, // below balcony
    {PORTAL_TYPE_SQUAD, 1928.06f, 763.256f, 51.3167f, 2.3905f}, // bridge
    {PORTAL_TYPE_SQUAD, 1924.26f, 847.661f, 47.1591f, 4.0202f}, // zuramat
    {PORTAL_TYPE_NORM, 1914.16f, 832.527f, 38.6441f, 3.5160f},  // xevozz
    {PORTAL_TYPE_NORM, 1857.30f, 764.145f, 38.6543f, 0.8339f},  // lavanthor
    {PORTAL_TYPE_BOSS, 1890.73f, 803.309f, 38.4001f, 2.4139f},  // center
};

struct BossInformation
{
    uint32 uiType, uiEntry, uiGhostEntry, uiWayPointId;
    float fX, fY, fZ;                                       // Waypoint for Saboteur
    int32 iSayEntry;
};

struct BossSpawn
{
    uint32 uiEntry;
    float fX, fY, fZ, fO;
};

static const BossInformation aBossInformation[] =
{
    {TYPE_EREKEM,    NPC_EREKEM,    NPC_ARAKKOA,    1, 1877.03f, 853.84f, 43.33f, SAY_RELEASE_EREKEM},
    {TYPE_ZURAMAT,   NPC_ZURAMAT,   NPC_VOID_LORD,  1, 1922.41f, 847.95f, 47.15f, SAY_RELEASE_ZURAMAT},
    {TYPE_XEVOZZ,    NPC_XEVOZZ,    NPC_ETHERAL,    1, 1903.61f, 838.46f, 38.72f, SAY_RELEASE_XEVOZZ},
    {TYPE_ICHORON,   NPC_ICHORON,   NPC_SWIRLING,   1, 1915.52f, 779.13f, 35.94f, SAY_RELEASE_ICHORON},
    {TYPE_LAVANTHOR, NPC_LAVANTHOR, NPC_LAVA_HOUND, 1, 1855.28f, 760.85f, 38.65f, 0},
    {TYPE_MORAGG,    NPC_MORAGG,    NPC_WATCHER,    1, 1890.51f, 752.85f, 47.66f, 0}
};

class instance_violet_hold : public ScriptedInstance
{
    public:
        instance_violet_hold(Map* pMap);
        ~instance_violet_hold();                            // Destructor used to free m_vRandomBosses

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void UpdateCellForBoss(uint32 uiBossEntry, bool bForceClosing = false);

        void SetIntroPortals(bool bDeactivate);

        void CallGuards(bool bRespawn);

        uint32 GetRandomPortalEliteEntry() { return (urand(0, 1) ? NPC_PORTAL_GUARDIAN : NPC_PORTAL_KEEPER); }
        uint32 GetRandomMobForNormalPortal() { return aRandomPortalNpcs[urand(0, 4)]; }
        uint32 GetRandomMobForIntroPortal() { return aRandomIntroNpcs[urand(0, 3)]; }

        uint32 GetCurrentPortalNumber() { return m_uiWorldStatePortalCount; }

        BossInformation const* GetBossInformation(uint32 uiEntry = 0);

        bool IsCurrentPortalForTrash()
        {
            if (m_uiWorldStatePortalCount % MAX_MINIBOSSES)
                return true;

            return false;
        }

        void ProcessActivationCrystal(Unit* pUser, bool bIsIntro = false);

        void GetErekemGuardList(GuidList& lGuardList) { lGuardList = GetData(TYPE_EREKEM) != DONE ? m_lErekemGuardList : m_lArakkoaGuardList; }
        void GetIchoronTriggerList(GuidList& lList) { lList = m_lIchoronTargetsList; }

        void OnPlayerEnter(Player* pPlayer) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff) override;

        typedef std::multimap<uint32, ObjectGuid> BossToCellMap;

    protected:
        PortalData const* GetPortalData() { return &afPortalLocation[m_uiPortalId]; }

        void UpdateWorldState(bool bEnable = true);

        void SetRandomBosses();

        void SpawnPortal();
        void SetPortalId();

        void ResetAll();
        void ResetVariables();

        bool IsNextPortalForTrash()
        {
            if ((m_uiWorldStatePortalCount + 1) % MAX_MINIBOSSES)
                return true;

            return false;
        }

        BossSpawn* CreateBossSpawnByEntry(uint32 uiEntry);
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiWorldState;
        uint32 m_uiWorldStateSealCount;
        uint32 m_uiWorldStatePortalCount;

        uint8 m_uiPortalId;
        uint32 m_uiPortalTimer;
        uint32 m_uiMaxCountPortalLoc;

        uint32 m_uiSealYellCount;
        uint32 m_uiEventResetTimer;

        bool m_bIsVoidDance;
        bool m_bIsDefenseless;
        bool m_bIsDehydratation;

        BossToCellMap m_mBossToCellMap;

        GuidList m_lIntroPortalList;
        GuidList m_lGuardsList;
        GuidList m_lErekemGuardList;
        GuidList m_lArakkoaGuardList;
        GuidList m_lIchoronTargetsList;
        std::vector<uint32> m_vRandomBossList;

        std::vector<BossSpawn*> m_vRandomBosses;
};

#endif
