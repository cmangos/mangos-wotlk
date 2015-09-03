/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_AHNKAHET_H
#define DEF_AHNKAHET_H
/* Encounters
 * Elder Nadox         = 1
 * Prince Taldram      = 2
 * Jedoga Shadowseeker = 3
 * Herald Volazj       = 4
 * Amanitar            = 5
*/
enum
{
    MAX_ENCOUNTER               = 5,
    MAX_INITIATES               = 15,

    TYPE_NADOX                  = 0,
    TYPE_TALDARAM               = 1,
    TYPE_JEDOGA                 = 2,
    TYPE_VOLAZJ                 = 3,
    TYPE_AMANITAR               = 4,

    DATA_INSANITY_PLAYER        = 1,

    GO_DOOR_TALDARAM            = 192236,
    GO_ANCIENT_DEVICE_L         = 193093,
    GO_ANCIENT_DEVICE_R         = 193094,
    GO_VORTEX                   = 193564,

    NPC_ELDER_NADOX             = 29309,
    NPC_TALDARAM                = 29308,
    NPC_JEDOGA_SHADOWSEEKER     = 29310,
    NPC_AHNKAHAR_GUARDIAN_EGG   = 30173,
    NPC_AHNKAHAR_SWARM_EGG      = 30172,
    NPC_JEDOGA_CONTROLLER       = 30181,
    NPC_TWILIGHT_INITIATE       = 30114,

    NPC_HERALD_VOLAZJ           = 29311,
    NPC_TWISTED_VISAGE_1        = 30621,
    NPC_TWISTED_VISAGE_2        = 30622,
    NPC_TWISTED_VISAGE_3        = 30623,
    NPC_TWISTED_VISAGE_4        = 30624,
    NPC_TWISTED_VISAGE_5        = 30625,

    SPELL_TWISTED_VISAGE_DEATH  = 57555,
    SPELL_INSANITY_SWITCH       = 57538,
    SPELL_INSANITY_CLEAR        = 57558,

    SPELL_INSANITY_PHASE_16     = 57508,
    SPELL_INSANITY_PHASE_32     = 57509,
    SPELL_INSANITY_PHASE_64     = 57510,
    SPELL_INSANITY_PHASE_128    = 57511,
    SPELL_INSANITY_PHASE_256    = 57512,

    ACHIEV_START_VOLAZJ_ID      = 20382,

    ACHIEV_CRIT_RESPECT_ELDERS  = 7317,             // Nadox, achiev 2038
    ACHIEV_CRIT_VOLUNTEER_WORK  = 7359,             // Jedoga, achiev 2056
};

static const float aTaldaramLandingLoc[4] = {528.734f, -845.998f, 11.54f, 0.68f};
static const float aJedogaLandingLoc[4] = {375.4977f, -707.3635f, -16.094f, 5.42f};

class instance_ahnkahet : public ScriptedInstance
{
    public:
        instance_ahnkahet(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureEvade(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void SetData64(uint32 uiType, uint64 uiGuid) override;

        ObjectGuid SelectRandomGuardianEggGuid();
        ObjectGuid SelectRandomSwarmerEggGuid();
        ObjectGuid SelectJedogaSacrificeControllerGuid() { return m_jedogaSacrificeController; }

        void GetJedogaControllersList(GuidList& lList) { lList = m_lJedogaControllersGuidList; }
        void GetJedogaEventControllersList(GuidList& lList) {lList = m_lJedogaEventControllersGuidList; }

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:
        void HandleInsanityClear();
        void HandleInsanitySwitch(Player* pPlayer);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_bRespectElders;
        bool m_bVolunteerWork;

        uint8 m_uiDevicesActivated;
        uint8 m_uiInitiatesKilled;
        uint8 m_uiTwistedVisageCount;

        ObjectGuid m_jedogaSacrificeController;

        GuidList m_GuardianEggList;
        GuidList m_SwarmerEggList;
        GuidList m_lJedogaControllersGuidList;
        GuidList m_lJedogaEventControllersGuidList;
        GuidList m_lInsanityPlayersGuidList;
};

#endif
