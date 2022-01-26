/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SUNWELLPLATEAU_H
#define DEF_SUNWELLPLATEAU_H

#include "Chat/Chat.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

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
    NPC_SUNBLADE_PROTECTOR      = 25507,            // npc_sunblade_scout
    NPC_MURU                    = 25741,
    NPC_ENTROPIUS               = 25840,
    NPC_BERSERKER               = 25798,            // muru trash mobs - scripted in Acid
    NPC_FURY_MAGE               = 25799,
    NPC_DARK_FIEND              = 25744,
    NPC_VOID_SENTINEL           = 25772,
    NPC_HAND_OF_THE_DECEIVER    = 25588,
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

    NPC_BLUE_ORB_TARGET         = 25640,
    SPELL_RING_BLUE_FLAME       = 45825,

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
    NPC_ANVEENA_MARKER          = 26057,

    // felmyst
    NPC_UNYELDING_DEAD          = 25268,
    NPC_DEMONIC_VAPOR           = 25265,
    NPC_DEMONIC_VAPOR_TRAIL     = 25267,
    SPELL_FOG_CORRUPTION        = 45582,

    // gauntlet
    NPC_GAUNTLET_IMP_TRIGGER    = 25848,
    NPC_SHADOWSWORD_COMMANDER   = 25837,

    SAY_GAUNTLET_IMPS           = -1580116,

    NPC_SHADOWSWORD_DEATHBRINGER= 25485,
    NPC_VOLATILE_FIEND          = 25851,

    SPELL_CRYSTAL_CHANNEL       = 46174, // Shadowsword Soulbinder

    // twins
    GO_BLAZE                    = 187366,
    NPC_SHADOW_IMAGE            = 25214,
    DB_ENCOUNTER_TWINS          = 727,

    // muru
    NPC_MURU_PORTAL_TARGET      = 25770,
    SPELL_TRANSFORM_VISUAL_PERIODIC = 46205,
    NPC_VOID_SPAWN              = 25824,
    NPC_DARKNESS                = 25879,
    NPC_SINGULARITY             = 25855,

    // KJ
    MAX_DECEIVERS               = 3,
    NPC_SINISTER_REFLECTION     = 25708,

    // outro
    SPELL_SIMPLE_TELEPORT       = 12980,

    GUID_PREFIX                 = 5800000,
};

static const Position aMadrigosaLoc[] =
{
    {1463.82f, 661.212f, 19.7971f, 4.712551f},      // reload spawn loc - the place where to spawn Felmyst
    {1463.82f, 661.212f, 39.234f,  0.f},            // fly loc during the cinematig
    {1464.6943f, 652.1426f, 20.81918f, 0.f},        // landing loc after fel fireball
};

static const Position aKalecLoc[] =
{
    {1573.1461f, 755.20245f, 99.524956f, 3.59537816047668457f}, // spawn loc
    {1474.2347f, 624.0703f,  29.32589f,  0.f}, // first move
    {1511.6547f, 550.70245f, 25.510078f, 0.f}, // open door
    {1648.255f,  519.3769f,  165.8482f,  0.f}, // fly away
};

static const float afMuruSpawnLoc[4] = { 1816.401f, 625.8939f, 69.68857f, 5.585053f };

static const float aKalegSpawnLoc[4] = { 1734.465f, 592.5678f, 142.3971f, 4.533074f };

class instance_sunwell_plateau : public ScriptedInstance, private DialogueHelper, public TimerManager
{
    public:
        instance_sunwell_plateau(Map* pMap);
        ~instance_sunwell_plateau() {}

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* creature) override;
        void OnCreatureEnterCombat(Creature* creature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature) override;
        void OnPlayerDeath(Player* player);

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(const uint32 diff) override;

        ObjectGuid SelectFelmystFlightTrigger(bool bLeftSide, uint8 uiIndex);

        void AddToSpectralRealm(ObjectGuid playerGuid) { m_spectralRealmPlayers.insert(playerGuid); }
        void RemoveFromSpectralRealm(ObjectGuid playerGuid) { m_spectralRealmPlayers.erase(playerGuid); }
        void DoEjectSpectralPlayers();

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

        bool IsBrutallusIntroStarted() const { return m_brutallusIntroStarted; }
        void StartBrutallusIntro() { m_brutallusIntroStarted = true; }

        void DropFog();

    protected:
        void JustDidDialogueStep(int32 iEntry) override;
        void DoSortFlightTriggers();
        void SpawnFelmyst();
        void SpawnGauntlet(bool respawn = false);
        void SpawnTwins();
        void SpawnMuru();
        void SpawnKiljaeden();
        void FailKiljaeden();

        void StartImps();
        void StopImps();
        void ResetGauntlet(bool stop = false);
        void RespawnGauntlet();
        void ImpYell();
        void StartMiniEvent();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_brutallusIntroStarted;

        bool m_firstEnter;
        bool m_spawnedGauntlet;
        bool m_impsStarted;
        bool m_miniAttackEvent;
        TimePoint m_gauntletRespawnTime;

        // Misc
        uint8 m_uiDeceiversKilled;
        uint32 m_uiSpectralRealmTimer;
        uint32 m_uiMuruBerserkTimer;
        uint32 m_uiKiljaedenYellTimer;

        GuidSet m_spectralRealmPlayers;
        GuidVector m_vRightFlightTriggersVect;
        GuidVector m_vLeftFlightTriggersVect;
        GuidList m_lAllFlightTriggersList;
        GuidList m_lBackdoorTriggersList;
        std::vector<uint32> m_kiljaedenRespawnDbGuids;
        GuidVector m_muruTrashGuids;
        GuidVector m_felmystSpawns;
        GuidVector m_gauntletSpawns;
        GuidVector m_twinsSpawns;
};
#endif
