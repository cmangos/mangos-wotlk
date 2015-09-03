/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SUNWELLPLATEAU_H
#define DEF_SUNWELLPLATEAU_H

enum
{
    MAX_ENCOUNTER               = 6,

    TYPE_KALECGOS               = 0,
    TYPE_BRUTALLUS              = 1,
    TYPE_FELMYST                = 2,
    TYPE_EREDAR_TWINS           = 3,
    TYPE_MURU                   = 4,
    TYPE_KILJAEDEN              = 5,

    NPC_KALECGOS_DRAGON         = 24850,            // kalecgos blue dragon hostile
    NPC_KALECGOS_HUMAN          = 24891,            // kalecgos human form in spectral realm
    NPC_SATHROVARR              = 24892,
    NPC_MADRIGOSA               = 24895,
    NPC_FLIGHT_TRIGGER_LEFT     = 25357,            // Related to Felmyst flight path. Also the anchor to summon Madrigosa
    NPC_FLIGHT_TRIGGER_RIGHT    = 25358,            // related to Felmyst flight path
    NPC_WORLD_TRIGGER           = 22515,
    NPC_WORLD_TRIGGER_LARGE     = 23472,            // ground triggers spawned in Brutallus / Felmyst arena
    NPC_BRUTALLUS               = 24882,
    NPC_FELMYST                 = 25038,
    NPC_KALECGOS_MADRIGOSA      = 24844,            // kalecgos blue dragon; spawns after Felmyst
    NPC_ALYTHESS                = 25166,
    NPC_SACROLASH               = 25165,
    NPC_MURU                    = 25741,
    NPC_ENTROPIUS               = 25840,
    NPC_BERSERKER               = 25798,            // muru trash mobs - scripted in Acid
    NPC_FURY_MAGE               = 25799,
    NPC_DARK_FIEND              = 25744,
    NPC_VOID_SENTINEL           = 25772,
    NPC_DECEIVER                = 25588,
    NPC_KILJAEDEN               = 25315,
    NPC_KILJAEDEN_CONTROLLER    = 25608,            // kiljaeden event controller
    NPC_ANVEENA                 = 26046,            // related to kiljaeden event
    NPC_KALECGOS                = 25319,            // related to kiljaeden event
    NPC_VELEN                   = 26246,
    NPC_LIADRIN                 = 26247,

    GO_FORCEFIELD               = 188421,           // kalecgos door + collisions
    GO_BOSS_COLLISION_1         = 188523,
    GO_BOSS_COLLISION_2         = 188524,
    GO_ICE_BARRIER              = 188119,           // used to block the players path during the Brutallus intro event
    GO_FIRE_BARRIER             = 188075,           // door after felmyst
    // GO_FIRST_GATE            = 187766,           // door between felmyst and eredar twins
    // GO_SECOND_GATE           = 187764,           // door after eredar twins
    GO_MURU_ENTER_GATE          = 187990,           // muru gates
    GO_MURU_EXIT_GATE           = 188118,
    // GO_THIRD_GATE            = 187765,           // door after muru; why another?

    GO_ORB_BLUE_FLIGHT_1        = 188115,           // orbs used in the Kil'jaeden fight
    GO_ORB_BLUE_FLIGHT_2        = 188116,
    GO_ORB_BLUE_FLIGHT_3        = 187869,
    GO_ORB_BLUE_FLIGHT_4        = 188114,

    SAY_KALECGOS_OUTRO          = -1580043,
    SAY_TWINS_INTRO             = -1580044,

    // Kil'jaeden yells
    SAY_ORDER_1                 = -1580064,
    SAY_ORDER_2                 = -1580065,
    SAY_ORDER_3                 = -1580066,
    SAY_ORDER_4                 = -1580067,
    SAY_ORDER_5                 = -1580068,

    AREATRIGGER_TWINS           = 4937,

    // Kalec spectral realm spells
    SPELL_TELEPORT_NORMAL_REALM = 46020,
    SPELL_SPECTRAL_REALM_AURA   = 46021,
    SPELL_SPECTRAL_EXHAUSTION   = 44867,
    // Felmyst ouro spell
    SPELL_OPEN_BACK_DOOR        = 46650,            // Opens the fire barrier - script effect for 46652
    // used by both muru and entropius
    SPELL_MURU_BERSERK          = 26662,
    // visuals for Kiljaeden encounter
    SPELL_ANVEENA_DRAIN         = 46410,

    MAX_DECEIVERS               = 3
};

struct EventLocations
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const EventLocations aMadrigosaLoc[] =
{
    {1463.82f, 661.212f, 19.79f, 4.88f},            // reload spawn loc - the place where to spawn Felmyst
    {1463.82f, 661.212f, 39.234f},                  // fly loc during the cinematig
};

static const EventLocations aKalecLoc[] =
{
    {1573.146f, 755.2025f, 99.524f, 3.59f},         // spawn loc
    {1474.235f, 624.0703f, 29.325f},                // first move
    {1511.655f, 550.7028f, 25.510f},                // open door
    {1648.255f, 519.377f, 165.848f},                // fly away
};

static const float afMuruSpawnLoc[4] = { 1816.25f, 625.484f, 69.603f, 5.624f };

class instance_sunwell_plateau : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_sunwell_plateau(Map* pMap);
        ~instance_sunwell_plateau() {}

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(uint32 uiDiff) override;

        ObjectGuid SelectFelmystFlightTrigger(bool bLeftSide, uint8 uiIndex);

        void AddToSpectralRealm(ObjectGuid playerGuid) { m_spectralRealmPlayers.insert(playerGuid); }
        void RemoveFromSpectralRealm(ObjectGuid playerGuid) { m_spectralRealmPlayers.erase(playerGuid); }
        void DoEjectSpectralPlayers();

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    protected:
        void JustDidDialogueStep(int32 iEntry) override;
        void DoSortFlightTriggers();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        // Misc
        uint8 m_uiDeceiversKilled;
        uint32 m_uiSpectralRealmTimer;
        uint32 m_uiKalecRespawnTimer;
        uint32 m_uiMuruBerserkTimer;
        uint32 m_uiKiljaedenYellTimer;

        GuidSet m_spectralRealmPlayers;
        GuidVector m_vRightFlightTriggersVect;
        GuidVector m_vLeftFlightTriggersVect;
        GuidList m_lAllFlightTriggersList;
        GuidList m_lBackdoorTriggersList;
        GuidList m_lDeceiversGuidList;
        GuidList m_lMuruTrashGuidList;
};
#endif
