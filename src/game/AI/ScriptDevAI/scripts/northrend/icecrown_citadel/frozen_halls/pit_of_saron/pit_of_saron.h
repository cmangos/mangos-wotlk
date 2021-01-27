/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_PIT_H
#define DEF_ICECROWN_PIT_H

enum
{
    MAX_ENCOUNTER                   = 4,
    MAX_SPECIAL_ACHIEV_CRITS        = 2,

    TYPE_GARFROST                   = 0,
    TYPE_KRICK                      = 1,
    TYPE_TYRANNUS                   = 2,
    TYPE_AMBUSH                     = 3,

    TYPE_ACHIEV_DOESNT_GO_ELEVEN    = 0,
    TYPE_ACHIEV_DONT_LOOK_UP        = 1,

    // Bosses
    NPC_TYRANNUS_INTRO              = 36794,
    NPC_GARFROST                    = 36494,
    NPC_KRICK                       = 36477,
    NPC_ICK                         = 36476,
    NPC_TYRANNUS                    = 36658,
    NPC_RIMEFANG                    = 36661,
    NPC_SINDRAGOSA                  = 37755,

    // Intro part npcs
    NPC_SYLVANAS_PART1              = 36990,
    NPC_SYLVANAS_PART2              = 38189,
    NPC_JAINA_PART1                 = 36993,
    NPC_JAINA_PART2                 = 38188,
    NPC_KILARA                      = 37583,
    NPC_ELANDRA                     = 37774,
    NPC_LORALEN                     = 37779,
    NPC_KORELN                      = 37582,
    NPC_CHAMPION_1_HORDE            = 37584,
    NPC_CHAMPION_2_HORDE            = 37587,
    NPC_CHAMPION_3_HORDE            = 37588,
    NPC_CHAMPION_1_ALLIANCE         = 37496,
    NPC_CHAMPION_2_ALLIANCE         = 37498,
    NPC_CHAMPION_3_ALLIANCE         = 37497,
    NPC_CORRUPTED_CHAMPION          = 36796,

    // Enslaved npcs
    NPC_IRONSKULL_PART1             = 37592,
    NPC_IRONSKULL_PART2             = 37581,
    NPC_VICTUS_PART1                = 37591,
    NPC_VICTUS_PART2                = 37580,
    NPC_FREE_HORDE_SLAVE_1          = 37577,
    NPC_FREE_HORDE_SLAVE_2          = 37578,
    NPC_FREE_HORDE_SLAVE_3          = 37579,
    NPC_FREE_ALLIANCE_SLAVE_1       = 37572,
    NPC_FREE_ALLIANCE_SLAVE_2       = 37575,
    NPC_FREE_ALLIANCE_SLAVE_3       = 37576,
    NPC_HORDE_SLAVE_1               = 36770,
    NPC_HORDE_SLAVE_2               = 36771,
    NPC_HORDE_SLAVE_3               = 36772,
    NPC_HORDE_SLAVE_4               = 36773,
    NPC_ALLIANCE_SLAVE_1            = 36764,
    NPC_ALLIANCE_SLAVE_2            = 36765,
    NPC_ALLIANCE_SLAVE_3            = 36766,
    NPC_ALLIANCE_SLAVE_4            = 36767,

    // Various npcs
    NPC_FROSTBLADE                  = 37670,
    NPC_FORGEMASTER_STALKER         = 36495,                // used to mark Garfrost jump locations
    NPC_EYE_LICH_KING               = 36913,

    // Ambush npcs
    NPC_YMIRJAR_DEATHBRINGER        = 36892,
    NPC_YMIRJAR_WRATHBRINGER        = 36840,
    NPC_YMIRJAR_FLAMEBEARER         = 36893,
    NPC_FALLEN_WARRIOR              = 36841,
    NPC_COLDWRAITH                  = 36842,
    NPC_STALKER                     = 32780,                // used to handle icicles during the tunnel event
    NPC_GENERAL_BUNNY               = 24110,                // used to handle visuals for the last encounter

    // epilog npcs - used during the Tyrannus encounter
    NPC_WRATHBONE_SORCERER          = 37728,
    NPC_WRATHBONE_REAVER            = 37729,
    NPC_FALLEN_WARRIOR_EPILOG       = 38487,

    GO_ICEWALL                      = 201885,               // open after gafrost/krick
    GO_HALLS_OF_REFLECT_PORT        = 201848,               // unlocked by jaina/sylvanas at last outro
    GO_SARONITE_ROCK                = 196485,

    // GO_BALL_AND_CHAIN_ALLIANCE   = 202168,               // ball and chain used in the quests; kept for reference only
    // GO_BALL_AND_CHAIN_HORDE      = 201969,

    AREATRIGGER_ID_TUNNEL_START     = 5578,
    AREATRIGGER_ID_TUNNEL_END       = 5581,

    ACHIEV_CRIT_DOESNT_GO_ELEVEN    = 12993,                // Garfrost, achiev 4524
    ACHIEV_CRIT_DONT_LOOK_UP        = 12994,                // Gauntlet, achiev 4525
};

static const float afTyrannusMovePos[4][3] =
{
    {922.6365f, 145.877f, 643.2216f},                       // Hide position
    {966.3345f, 159.2058f, 665.0453f},                      // Rimefang position
    {578.7934f, 122.4046f, 583.1108f},                      // Tyrannus backup summon
    {1017.5917f, 37.31939f, 708.9627f},                     // Tyrannus tunnel move position
};

static const float afTyrannusSummonPos[2][3] =
{
    {1066.5039f, 62.3141f, 632.0350f},                      // Summon position
    {1059.7586f, 108.285f, 629.1584f},                      // Move position
};

struct EventNpcLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fX, fY, fZ, fO;
    uint8 pathId;
};

const EventNpcLocations aEventBeginLocations[] =
{
    {NPC_SYLVANAS_PART1, NPC_JAINA_PART1,   430.3012f, 212.204f,  530.1146f, 0.042f, 0},
    {NPC_KILARA,         NPC_ELANDRA,       429.7142f, 212.3021f, 530.2822f, 0.14f,  1},
    {NPC_LORALEN,        NPC_KORELN,        429.5675f, 211.7748f, 530.3246f, 5.972f, 2},

    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 425.911f,   213.33363f, 528.7967f,  0.21628f, 9},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 425.95148f, 213.43628f, 528.7967f,  0.91054f, 10},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 425.42874f, 213.30286f, 528.805f,   0.16026f, 11},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 429.37402f, 212.1061f,  528.80206f, 6.16585f, 12},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 429.98132f, 213.41042f, 528.8051f,  0.09478f, 13},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 429.5885f,  210.69595f, 528.8051f,  5.95487f, 14},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 426.13156f, 211.77509f, 528.7967f,  6.14891f, 15},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 426.7432f,  212.22388f, 528.805f,   5.85526f, 16},
    {NPC_CHAMPION_1_HORDE, NPC_CHAMPION_1_ALLIANCE, 424.07047f, 211.1222f,  528.7967f,  5.57246f, 17},

    {NPC_CHAMPION_2_HORDE, NPC_CHAMPION_2_ALLIANCE, 424.7531f,  212.52791f, 528.8003f,  6.17507f, 3},
    {NPC_CHAMPION_2_HORDE, NPC_CHAMPION_2_ALLIANCE, 425.66174f, 214.27966f, 528.80505f, 0.12017f, 4},
    {NPC_CHAMPION_2_HORDE, NPC_CHAMPION_2_ALLIANCE, 425.34244f, 210.50476f, 528.80505f, 5.95579f, 5},

    {NPC_CHAMPION_3_HORDE, NPC_CHAMPION_3_ALLIANCE, 426.8169f,  210.74643f, 528.7967f,  5.63237f, 4},
    {NPC_CHAMPION_3_HORDE, NPC_CHAMPION_3_ALLIANCE, 427.46008f, 209.36372f, 528.80505f, 5.71778f, 5},
    {NPC_CHAMPION_3_HORDE, NPC_CHAMPION_3_ALLIANCE, 427.26782f, 213.06624f, 528.7967f,  0.06322f, 6},
    {NPC_CHAMPION_3_HORDE, NPC_CHAMPION_3_ALLIANCE, 427.55377f, 216.24657f, 528.80505f, 0.78343f, 7},
};

const EventNpcLocations aEventFirstAmbushLocations[2] =
{
    {NPC_YMIRJAR_DEATHBRINGER, 0, 951.6696f, 53.06405f, 567.5153f, 1.51f, 0},
    {NPC_YMIRJAR_DEATHBRINGER, 0, 950.9911f, 60.26712f, 566.7658f, 1.79f, 1},
};

const EventNpcLocations aEventSecondAmbushLocations[] =
{
    {NPC_FALLEN_WARRIOR, 0, 926.8993f, -68.083336f, 592.0798f, 1.85004f, 0},
    {NPC_FALLEN_WARRIOR, 0, 919.7518f, -68.392365f, 592.2916f, 1.85004f, 1},
    {NPC_FALLEN_WARRIOR, 0, 931.0399f, -55.154514f, 591.8819f, 1.85004f, 2},
    {NPC_FALLEN_WARRIOR, 0, 923.8055f, -55.631947f, 591.8663f, 1.85004f, 3},
    {NPC_FALLEN_WARRIOR, 0, 916.658f,  -55.94097f,  591.6827f, 1.85004f, 4},
    {NPC_FALLEN_WARRIOR, 0, 939.1562f, -65.979164f, 592.2204f, 1.85004f, 5},
    {NPC_COLDWRAITH, 0, 921.65546f, -57.30312f, 591.8749f, 2.01133f, 0},
    {NPC_COLDWRAITH, 0, 935.2812f,  -67.22413f, 592.0464f, 1.66512f, 1},
    {NPC_COLDWRAITH, 0, 926.8993f,  -68.08333f, 592.0798f, 1.85004f, 2},
    {NPC_COLDWRAITH, 0, 935.5867f,  -46.55127f, 591.9112f, 1.63308f, 3},
};

const EventNpcLocations aEventTunnelEndLocations[] =
{
    {NPC_IRONSKULL_PART2,    NPC_VICTUS_PART2,          1071.8212f, 45.12153f,  630.2551f,  1.83259f, 0},

    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1070.9827f, 37.543404f, 629.8123f,  1.74532f, 2},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1077.1823f, 31.899305f, 629.99915f, 1.88495f, 3},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1069.2067f, 34.383682f, 629.9972f,  1.65806f, 4},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1071.1875f, 32.758682f, 630.1298f,  1.72787f, 5},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1079.3368f, 32.458332f, 629.9384f,  1.95476f, 6},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1073.4149f, 31.364584f, 630.2189f,  1.78023f, 7},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1075.0416f, 32.175346f, 630.05164f, 1.83259f, 8},
    {NPC_FREE_HORDE_SLAVE_1, NPC_FREE_ALLIANCE_SLAVE_1, 1072.1216f, 28.32639f,  630.7766f,  1.72787f, 9},

    {NPC_FREE_HORDE_SLAVE_2, NPC_FREE_ALLIANCE_SLAVE_2, 1070.5591f, 29.996529f, 630.42804f, 1.69296f, 1},
    {NPC_FREE_HORDE_SLAVE_2, NPC_FREE_ALLIANCE_SLAVE_2, 1072.7274f, 24.48611f,  631.86096f, 1.72787f, 2},
    {NPC_FREE_HORDE_SLAVE_2, NPC_FREE_ALLIANCE_SLAVE_2, 1079.0312f, 27.003473f, 630.8719f,  1.90240f, 3},
    {NPC_FREE_HORDE_SLAVE_2, NPC_FREE_ALLIANCE_SLAVE_2, 1074.849f,  25.770834f, 631.36926f, 1.78023f, 4},

    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1067.3195f, 43.131947f, 630.1227f,  1.60570f, 2},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1070.6545f, 42.70486f,  630.0697f,  1.76278f, 3},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1073.8733f, 40.260418f, 629.9038f,  1.86750f, 4},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1075.3594f, 36.43403f,  629.72156f, 1.88495f, 5},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1073.8507f, 42.680557f, 630.03265f, 1.90240f, 6},
    {NPC_FREE_HORDE_SLAVE_3, NPC_FREE_ALLIANCE_SLAVE_3, 1075.8976f, 40.331596f, 629.86383f, 1.93731f, 7},
};

class instance_pit_of_saron : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_pit_of_saron(Map* pMap);
        ~instance_pit_of_saron() {}

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        uint32 GetPlayerTeam() const { return m_uiTeam; }

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff);

    protected:
        void JustDidDialogueStep(int32 iEntry) override;

        void DoStartIntroEvent();
        void DoStartAmbushEvent();
        void DoStartTyrannusEvent();
        void DoSpawnTyrannusUndead();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];

        uint8 m_uiAmbushAggroCount;
        uint32 m_uiTeam;                                    // Team of first entered player, used to set if Jaina or Silvana to spawn
        uint32 m_uiSummonDelayTimer;
        uint32 m_uiIciclesTimer;
        uint32 m_uiEyeLichKingTimer;
        uint32 m_uiSummonUndeadTimer;

        GuidList m_lTunnelStalkersGuidList;
        GuidList m_lAmbushNpcsGuidList;
        GuidList m_lSaroniteRockGuidList;
        GuidList m_lEndingCreaturesGuidList;
};

#endif
