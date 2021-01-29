/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_UTG_PINNACLE_H
#define DEF_UTG_PINNACLE_H

enum
{
    MAX_ENCOUNTER                   = 4,
    MAX_SPECIAL_ACHIEV_CRITS        = 3,

    TYPE_SVALA                      = 0,
    TYPE_GORTOK                     = 1,
    TYPE_SKADI                      = 2,
    TYPE_YMIRON                     = 3,

    TYPE_ACHIEV_INCREDIBLE_HULK     = 0,
    TYPE_ACHIEV_LOVE_SKADI          = 1,
    TYPE_ACHIEV_KINGS_BANE          = 2,

    GO_STASIS_GENERATOR             = 188593,
    GO_DOOR_SKADI                   = 192173,
    GO_DOOR_YMIRON                  = 192174,

    NPC_WORLD_TRIGGER               = 22515,

    NPC_ARTHAS_IMAGE                = 29280,
    NPC_SVALA                       = 29281,
    NPC_SVALA_SORROWGRAVE           = 26668,
    NPC_FLAME_BRAZIER               = 27273,

    NPC_GRAUF                       = 26893,
    NPC_SKADI                       = 26693,
    NPC_YMIRJAR_WARRIOR             = 26690,
    NPC_YMIRJAR_WITCH_DOCTOR        = 26691,
    NPC_YMIRJAR_HARPOONER           = 26692,
    NPC_RITUAL_TARGET               = 27327,
    NPC_FLAME_BREATH_TRIGGER        = 28351,            // triggers the freezing cloud spell in script
    NPC_WORLD_TRIGGER_LARGE         = 23472,            // only one spawn in this instance - casts 49308 during the gauntlet event
    NPC_SPIRIT_FOUNT                = 27339,

    NPC_FURBOLG                     = 26684,
    NPC_WORGEN                      = 26683,
    NPC_JORMUNGAR                   = 26685,
    NPC_RHINO                       = 26686,
    NPC_GORTOK                      = 26687,

    // Ymiron spirits
    NPC_BJORN                       = 27303,            // front right
    NPC_HALDOR                      = 27307,            // front left
    NPC_RANULF                      = 27308,            // back left
    NPC_TORGYN                      = 27309,            // back right

    ACHIEV_CRIT_INCREDIBLE_HULK     = 7322,             // Svala, achiev - 2043
    ACHIEV_CRIT_GIRL_LOVES_SKADI    = 7595,             // Skadi, achiev - 2156
    ACHIEV_CRIT_KINGS_BANE          = 7598,             // Ymiron, achiev - 2157

    ACHIEV_START_SKADI_ID           = 17726,            // Starts Skadi timed achiev - 1873

    // Gortok event spells
    SPELL_ORB_VISUAL                = 48044,
    SPELL_AWAKEN_SUBBOSS            = 47669,
    SPELL_AWAKEN_GORTOK             = 47670,

    // Skadi event spells
    SPELL_GAUNTLET_PERIODIC         = 47546,            // cast by each player; triggers 47547 every 5 secs. Potentially used to check if boss has to reset during the fight
    SPELL_GAUNTLET_RESET_CHECK      = 49308,            // cast by 23472 every 7 seconds during the Skadi fight; its purpose is to reset the flame breath triggers
};

static const float aArthasSpawnPos[4] = { 296.2659f, -364.3349f, 92.9248f, 1.588f};
static const float aSvalaSpawnPos[4] = { 296.703f, -346.099f, 97.6311f, 4.694f};

static const float aOrbPositions[2][3] =
{
    {238.6077f, -460.7103f, 112.5671f},                 // Orb lift up
    {279.26f,   -452.1f,    110.0f},                    // Orb center stop
};

static const uint32 aGortokMiniBosses[MAX_ENCOUNTER] = {NPC_WORGEN, NPC_FURBOLG, NPC_JORMUNGAR, NPC_RHINO};

class instance_pinnacle : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_pinnacle(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureEvade(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* creature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        ObjectGuid GetRandomBrazier() const
        {
            if (m_vbrazierVector.empty())
                return ObjectGuid();

            return m_vbrazierVector[urand(0, m_vbrazierVector.size() - 1)];
        }

        void SetGortokEventStarter(ObjectGuid playerGuid) { m_gortokEventStarterGuid = playerGuid; }
        ObjectGuid GetGortokEventStarter() const { return m_gortokEventStarterGuid; }
        ObjectGuid GetSkadiMobsTrigger() const { return m_skadiMobsTriggerGuid; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

    private:
        void JustDidDialogueStep(int32 iEntry) override;

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];
        std::string m_strInstData;

        uint32 m_uiGortokOrbTimer;
        uint8 m_uiGortokOrbPhase;

        uint32 m_uiGauntletCheckTimer;
        uint32 m_uiSkadiResetTimer;

        ObjectGuid m_gortokEventTriggerGuid;
        ObjectGuid m_gortokEventStarterGuid;
        ObjectGuid m_skadiMobsTriggerGuid;

        GuidList m_lskadiGauntletMobsList;
        GuidVector m_vbrazierVector;
};

#endif
