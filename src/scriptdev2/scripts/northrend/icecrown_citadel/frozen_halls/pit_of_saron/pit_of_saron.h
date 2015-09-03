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
    NPC_CHAMPION_2_ALLIANCE         = 37497,
    NPC_CHAMPION_3_ALLIANCE         = 37498,
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

    AREATRIGGER_ID_TUNNEL_START     = 5578,
    AREATRIGGER_ID_TUNNEL_END       = 5581,

    ACHIEV_CRIT_DOESNT_GO_ELEVEN    = 12993,                // Garfrost, achiev 4524
    ACHIEV_CRIT_DONT_LOOK_UP        = 12994,                // Gauntlet, achiev 4525
};

static const float afTyrannusMovePos[4][3] =
{
    {922.6365f, 145.877f, 643.2216f},                       // Hide position
    {835.5887f, 139.4345f, 530.9526f},                      // Ick position
    {906.9048f, -49.03813f, 618.8016f},                     // Tunnel position
    {966.3345f, 159.2058f, 665.0453f},                      // Rimefang position
};

struct EventNpcLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fX, fY, fZ, fO;
    float fMoveX, fMoveY, fMoveZ;
};

const EventNpcLocations aEventBeginLocations[3] =
{
    {NPC_SYLVANAS_PART1, NPC_JAINA_PART1,   430.3012f, 212.204f,  530.1146f, 0.042f, 440.7882f, 213.7587f, 528.7103f},
    {NPC_KILARA,         NPC_ELANDRA,       429.7142f, 212.3021f, 530.2822f, 0.14f,  438.9462f, 215.4271f, 528.7087f},
    {NPC_LORALEN,        NPC_KORELN,        429.5675f, 211.7748f, 530.3246f, 5.972f, 438.5052f, 211.5399f, 528.7085f},
    // ToDo: add the soldiers here when proper waypoint movement is supported
};

const EventNpcLocations aEventFirstAmbushLocations[2] =
{
    {NPC_YMIRJAR_DEATHBRINGER, 0, 951.6696f, 53.06405f, 567.5153f, 1.51f, 914.7256f, 76.66406f, 553.8029f},
    {NPC_YMIRJAR_DEATHBRINGER, 0, 950.9911f, 60.26712f, 566.7658f, 1.79f, 883.1805f, 52.69792f, 527.6385f},
};

const EventNpcLocations aEventSecondAmbushLocations[] =
{
    {NPC_FALLEN_WARRIOR, 0, 916.658f, -55.94097f, 591.6827f, 1.85f, 950.5694f, 31.85649f, 572.2693f},
    {NPC_FALLEN_WARRIOR, 0, 923.8055f, -55.63195f, 591.8663f, 1.85f, 941.3954f, 35.83769f, 571.4308f},
    {NPC_FALLEN_WARRIOR, 0, 936.0625f, -53.52778f, 592.0226f, 1.85f, 934.8011f, 8.024931f, 577.3419f},
    {NPC_FALLEN_WARRIOR, 0, 919.7518f, -68.39236f, 592.2916f, 1.85f, 932.5734f, -22.54153f, 587.403f},
    {NPC_FALLEN_WARRIOR, 0, 926.8993f, -68.08334f, 592.0798f, 1.85f, 922.6043f, -22.07627f, 585.6684f},
    {NPC_FALLEN_WARRIOR, 0, 939.1563f, -65.97916f, 592.2205f, 1.85f, 927.0928f, -32.97949f, 589.3028f},
    {NPC_COLDWRAITH, 0, 924.0261f, -62.3316f, 592.0191f, 2.01f, 929.4673f, 9.722589f, 577.4904f},
    {NPC_COLDWRAITH, 0, 936.4531f, -60.45486f, 592.1215f, 1.63f, 936.1395f, -4.003471f, 581.3139f},
    {NPC_COLDWRAITH, 0, 935.8055f, -72.76736f, 592.077f, 1.66f, 933.8441f, -47.83234f, 591.7538f},
    {NPC_COLDWRAITH, 0, 923.3785f, -74.6441f, 592.368f, 2.37f, 920.726f, -42.32272f, 589.9808f}
};

const EventNpcLocations aEventTunnelEndLocations[] =
{
    {NPC_IRONSKULL_PART2, NPC_VICTUS_PART2, 1071.45f, 48.23907f, 630.4871f, 1.68f, 1046.361f, 124.7031f, 628.2811f},
    // ToDo: add the freed slaves here when proper waypoint movement is supported
};
const EventNpcLocations aEventOutroLocations[] =
{
    {NPC_SINDRAGOSA, 0, 842.8611f, 194.5556f, 531.6536f, 6.108f, 900.106f, 181.677f, 659.374f},
    {NPC_SYLVANAS_PART2, NPC_JAINA_PART2, 1062.85f, 100.075f, 631.0021f, 1.77f, 1062.85f, 100.075f, 631.0021f},
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

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        uint32 GetPlayerTeam() { return m_uiTeam; }

        void DoStartAmbushEvent();

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff);

    protected:
        void JustDidDialogueStep(int32 iEntry) override;
        void ProcessIntroEventNpcs(Player* pPlayer);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];

        uint8 m_uiAmbushAggroCount;
        uint32 m_uiTeam;                                    // Team of first entered player, used to set if Jaina or Silvana to spawn
        uint32 m_uiIciclesTimer;

        GuidList m_lTunnelStalkersGuidList;
        GuidList m_lAmbushNpcsGuidList;
        GuidList m_lArcaneShieldBunniesGuidList;
        GuidList m_lFrozenAftermathBunniesGuidList;
};

#endif
