/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_EBON_HOLD_H
#define DEF_EBON_HOLD_H

enum
{
    TYPE_BATTLE                         = 0,

    // Light of Dawn battle states
    BATTLE_STATE_NOT_STARTED            = 0,                // battle state idle; nothing is started
    BATTLE_STATE_PREPARE                = 1,                // battle in preparation; player triggered the dialogue text
    BATTLE_STATE_IN_PROGRESS            = 2,                // battle is in progress
    BATTLE_STATE_LOST                   = 3,                // battle is lost; Tirion spawns
    BATTLE_STATE_DIALOGUE               = 4,                // Tirion reaches the Chappel; end dialogue starts
    BATTLE_STATE_WAIT_QUEST             = 5,                // event is complete; Darion waits for players to complete quest
    BATTLE_STATE_RESET                  = 6,                // quest is complete; the entire scene will reset

    TIMER_BATTLE_PREPARE                = 5 * MINUTE * IN_MILLISECONDS,
    TIMER_BATTLE_PROGRESS               = 5 * MINUTE * IN_MILLISECONDS,
    TIMER_BATTLE_QUEST_WAIT             = 5 * MINUTE * IN_MILLISECONDS,
    TIMER_BATTLE_RESET_DELAY            = 30 * IN_MILLISECONDS,

    // npcs
    // death knights
    NPC_HIGHLORD_DARION_MOGRAINE        = 29173,
    NPC_KOLTIRA_DEATHWEAVER             = 29199,
    NPC_ORBAZ_BLOODBANE                 = 29204,
    NPC_THASSARIAN                      = 29200,

    // scourge warriors - summond during the event
    NPC_FLESH_BEHEMOTH                  = 29190,
    NPC_RAMPAGING_ABOMINATION           = 29186,
    NPC_VOLATILE_GHOUL                  = 29219,
    NPC_WARRIOR_OF_THE_FROZEN_WASTES    = 29206,

    // argent dawn commanders
    NPC_HIGHLORD_TIRION_FORDRING        = 29175,
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT    = 29176,
    NPC_LORD_MAXWELL_TYROSUS            = 29178,
    NPC_COMMANDER_ELIGOR_DAWNBRINGER    = 29177,
    NPC_LEONID_BARTHALOMEW_THE_REVERED  = 29179,
    NPC_DUKE_NICHOLAS_ZVERENHOFF        = 29180,
    NPC_RIMBLAT_EARTHSHATTER            = 29182,
    NPC_RAYNE                           = 29181,

    // argent warriors
    NPC_DEFENDER_OF_THE_LIGHT           = 29174,

    // cinematic
    NPC_THE_LICH_KING                   = 29183,
    NPC_HIGHLORD_ALEXANDROS_MOGRAINE    = 29227,
    NPC_DARION_MOGRAINE                 = 29228,

    // object
    GO_LIGHT_OF_DAWN                    = 191330,
    GO_HOLY_LIGHTNING_1                 = 191301,
    GO_HOLY_LIGHTNING_2                 = 191302,

    // spells
    SPELL_CHAPTER_IV                    = 53405,            // phase aura

    // variables
    MAX_SCOURGE_CHAMPIONS               = 4,
    MAX_ABOMINATIONS                    = 3,
    MAX_LIGHT_CHAMPIONS                 = 7,                // the number of the light champions
    MAX_UNDEAD_WARRIORS                 = 20,               // summoned warriors per turn
    MAX_LIGHT_GUARDS                    = 4,                // guards summond for the outro

    // event variables
    MAX_BATTLE_INTRO_TIMER              = 5,
    MAX_LIGHT_DEFENDERS                 = 14,
    MAX_FORCES_LIGHT                    = 300,
    MAX_FORCES_SCOURGE                  = 10000,

    // world states
    // basically world states should be shown to all players with phase mask = 128 as stated in DBC
    // because we don't have the possibility to do that we'll just iterate through the players and set the phase mask manually based on the battle status
    WORLD_STATE_FORCES_SHOW             = 3592,             // show the remaining units
    WORLD_STATE_FORCES_SCOURGE          = 3591,
    WORLD_STATE_FORCES_LIGHT            = 3590,
    WORLD_STATE_BATTLE_TIMER_SHOW       = 3603,             // countdown timer
    WORLD_STATE_BATTLE_TIMER_TIME       = 3604,
    WORLD_STATE_BATTLE_BEGIN            = 3605,             // battle has begun

    // light and weathter id
    LIGHT_ID_DEFAULT                    = 42,
    LIGHT_ID_BATTLE                     = 1674,
    WEATHER_ID_FOG                      = 1,
    WEATHER_ID_BLOOD_RAIN               = 108,
};

static const uint32 aLightChampions[MAX_LIGHT_CHAMPIONS] = { NPC_KORFAX_CHAMPION_OF_THE_LIGHT, NPC_LORD_MAXWELL_TYROSUS, NPC_COMMANDER_ELIGOR_DAWNBRINGER, NPC_LEONID_BARTHALOMEW_THE_REVERED, NPC_DUKE_NICHOLAS_ZVERENHOFF, NPC_RIMBLAT_EARTHSHATTER, NPC_RAYNE };
static const uint32 aScourgeChampions[MAX_SCOURGE_CHAMPIONS] = { NPC_HIGHLORD_DARION_MOGRAINE, NPC_KOLTIRA_DEATHWEAVER, NPC_ORBAZ_BLOODBANE, NPC_THASSARIAN };

struct sSpawnLocation
{
    float m_fX, m_fY, m_fZ, m_fO;
    uint32 m_uiEntry;
};

// light champions
static sSpawnLocation aLightChampionsSpawnLoc[MAX_LIGHT_CHAMPIONS] =
{
    {2283.6975f, -5287.196f,  83.04569f, 0.27925f, NPC_KORFAX_CHAMPION_OF_THE_LIGHT},
    {2283.383f,  -5285.399f,  82.8475f,  0.38397f, NPC_LORD_MAXWELL_TYROSUS},
    {2281.1956f, -5316.9194f, 88.52019f, 1.65108f, NPC_LEONID_BARTHALOMEW_THE_REVERED},
    {2282.071f,  -5261.439f,  81.08088f, 0.08726f, NPC_DUKE_NICHOLAS_ZVERENHOFF},
    {2281.9797f, -5284.03f,   82.6842f,  0.36651f, NPC_COMMANDER_ELIGOR_DAWNBRINGER},
    {2248.4214f, -5294.5264f, 82.25065f, 2.47836f, NPC_RAYNE},
    {2306.4114f, -5292.506f,  82.0796f,  0.48869f, NPC_RIMBLAT_EARTHSHATTER}
};

// defend guards
static sSpawnLocation aGuardsDefendSpawnLoc[] =
{
    {2305.06811f, -5280.10107f, 81.86087f, 0.94247f, NPC_DEFENDER_OF_THE_LIGHT},
    {2271.94335f, -5266.66064f, 80.90213f, 6.23082f, NPC_DEFENDER_OF_THE_LIGHT},
    {2278.59448f, -5264.20263f, 81.12021f, 6.19591f, NPC_DEFENDER_OF_THE_LIGHT},
    {2247.02416f, -5291.07177f, 82.66799f, 0.50614f, NPC_DEFENDER_OF_THE_LIGHT},
    {2253.13574f, -5283.86816f, 82.56885f, 0.36651f, NPC_DEFENDER_OF_THE_LIGHT},
    {2290.81909f, -5267.06494f, 81.94192f, 0.36651f, NPC_DEFENDER_OF_THE_LIGHT},
    {2257.47924f, -5279.24121f, 82.29592f, 0.27925f, NPC_DEFENDER_OF_THE_LIGHT},
    {2296.03637f, -5271.62158f, 81.92523f, 0.55850f, NPC_DEFENDER_OF_THE_LIGHT},
    {2285.56591f, -5264.00244f, 81.67404f, 0.26179f, NPC_DEFENDER_OF_THE_LIGHT},
    {2249.95019f, -5287.71875f, 82.65625f, 0.40142f, NPC_DEFENDER_OF_THE_LIGHT},
    {2261.15722f, -5275.12695f, 81.77896f, 0.17453f, NPC_DEFENDER_OF_THE_LIGHT},
    {2266.33911f, -5270.70947f, 81.13216f, 0.06981f, NPC_DEFENDER_OF_THE_LIGHT},
    {2307.00488f, -5285.17773f, 82.06481f, 1.08210f, NPC_DEFENDER_OF_THE_LIGHT},
    {2300.41137f, -5275.80078f, 81.85671f, 0.75049f, NPC_DEFENDER_OF_THE_LIGHT},
};

// four guards spawned for the outro
static sSpawnLocation aGuardsOutroSpawnLoc[MAX_LIGHT_GUARDS] =
{
    {2275.1013f, -5281.815f,  82.34644f, 0.05235f, NPC_DEFENDER_OF_THE_LIGHT},
    {2285.049f,  -5277.8755f, 82.22345f, 4.01425f, NPC_DEFENDER_OF_THE_LIGHT},
    {2277.112f,  -5278.0474f, 82.21397f, 5.18362f, NPC_DEFENDER_OF_THE_LIGHT},
    {2286.6072f, -5281.87f,   82.40318f, 3.01941f, NPC_DEFENDER_OF_THE_LIGHT}
};

// Abomination spawn or respawn location
static sSpawnLocation aAbominationLocations[MAX_ABOMINATIONS] =
{
    {2320.052f,  -5255.586f,  84.115204f, 0.55850f, NPC_RAMPAGING_ABOMINATION},
    {2244.3254f, -5326.5083f, 82.26559f,  0.78539f, NPC_RAMPAGING_ABOMINATION},
    {2246.7869f, -5283.7407f, 82.46858f,  5.96412f, NPC_RAMPAGING_ABOMINATION},
};

// undead army wave spawn locations
static sSpawnLocation aArmyLocations1[] =
{
    {2455.4446f, -5126.7905f, 77.53983f,  3.54301f, NPC_VOLATILE_GHOUL},
    {2437.1948f, -5145.094f,  82.63543f,  2.18166f, NPC_VOLATILE_GHOUL},
    {2447.2542f, -5140.2837f, 80.2425f,   2.93215f, NPC_VOLATILE_GHOUL},
    {2413.8894f, -5182.391f,  77.400764f, 1.20427f, NPC_VOLATILE_GHOUL},
    {2443.595f,  -5147.8516f, 80.676025f, 2.40855f, NPC_VOLATILE_GHOUL},
    {2423.2708f, -5114.931f,  80.005226f, 5.07890f, NPC_VOLATILE_GHOUL},
    {2438.6711f, -5205.4443f, 74.78815f,  1.67551f, NPC_VOLATILE_GHOUL},
    {2443.6914f, -5170.2603f, 78.94165f,  1.919f,   NPC_WARRIOR_OF_THE_FROZEN_WASTES},
    {2411.5952f, -5157.665f,  82.30953f,  0.82030f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
    {2437.148f,  -5152.8696f, 82.14168f,  1.90240f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
};

static sSpawnLocation aArmyLocations2[] =
{
    {2462.9114f, -5161.0825f, 79.08804f, 2.4783f, NPC_VOLATILE_GHOUL},
    {2439.257f,  -5125.071f,  80.52294f, 4.1189f, NPC_VOLATILE_GHOUL},
    {2418.3708f, -5144.2637f, 84.63735f, 0.5235f, NPC_VOLATILE_GHOUL},
    {2437.273f,  -5194.4873f, 77.52633f, 1.6580f, NPC_VOLATILE_GHOUL},
    {2444.9631f, -5117.479f,  78.04392f, 4.1015f, NPC_VOLATILE_GHOUL},
    {2418.127f,  -5172.1704f, 80.01519f, 1.2217f, NPC_VOLATILE_GHOUL},
    {2428.2678f, -5121.0376f, 80.97027f, 4.9212f, NPC_VOLATILE_GHOUL},
    {2446.852f,  -5127.7793f, 79.22644f, 3.6651f, NPC_VOLATILE_GHOUL},
    {2417.6985f, -5124.905f,  81.8728f,  5.5676f, NPC_VOLATILE_GHOUL},
    {2412.408f,  -5170.1875f, 80.9735f,  1.0646f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
};

static sSpawnLocation aArmyLocations3[] =
{
    {2409.2395f, -5174.799f,  80.36803f, 1.0471f, NPC_VOLATILE_GHOUL},
    {2423.7856f, -5102.352f,  82.122f,   4.9392f, NPC_VOLATILE_GHOUL},
    {2388.6594f, -5157.9097f, 76.2673f,  0.4712f, NPC_VOLATILE_GHOUL},
    {2400.6282f, -5149.1543f, 82.56476f, 0.3839f, NPC_VOLATILE_GHOUL},
    {2445.4973f, -5134.4907f, 80.42659f, 3.3161f, NPC_VOLATILE_GHOUL},
    {2446.5808f, -5192.9536f, 77.00167f, 1.8325f, NPC_VOLATILE_GHOUL},
    {2425.5073f, -5176.929f,  78.39548f, 1.4311f, NPC_VOLATILE_GHOUL},
    {2433.6184f, -5108.799f,  80.21449f, 4.6251f, NPC_VOLATILE_GHOUL},
    {2427.595f,  -5125.255f,  82.18376f, 5.0440f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
    {2411.5144f, -5138.612f,  83.91638f, 0.0872f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
};

static sSpawnLocation aArmyLocations4[] =
{
    {2414.638f,  -5150.64f,   84.60375f,  0.69813f, NPC_VOLATILE_GHOUL},
    {2407.8823f, -5145.991f,  84.36466f,  0.36651f, NPC_VOLATILE_GHOUL},
    {2424.4822f, -5129.9795f, 83.14424f,  5.49778f, NPC_VOLATILE_GHOUL},
    {2449.2188f, -5174.1733f, 78.32296f,  2.00712f, NPC_VOLATILE_GHOUL},
    {2434.9346f, -5176.3936f, 78.272385f, 1.65806f, NPC_VOLATILE_GHOUL},
    {2425.7522f, -5142.311f,  84.284584f, 0.76794f, NPC_VOLATILE_GHOUL},
    {2429.452f,  -5188.8564f, 77.1038f,   1.53588f, NPC_VOLATILE_GHOUL},
    {2442.0679f, -5156.215f,  80.460464f, 2.05948f, NPC_VOLATILE_GHOUL},
    {2438.0554f, -5119.0645f, 79.43321f,  4.34587f, NPC_VOLATILE_GHOUL},
    {2427.8704f, -5167.918f,  80.67618f,  1.46607f, NPC_WARRIOR_OF_THE_FROZEN_WASTES},
};

static sSpawnLocation aArmyLocations5[] =
{
    {2409.1438f, -5165.1826f, 81.067505f, 0.9075f, NPC_VOLATILE_GHOUL},
    {2431.9268f, -5158.1797f, 83.071915f, 1.5882f, NPC_VOLATILE_GHOUL},
    {2404.884f,  -5158.29f,   81.228455f, 0.6806f, NPC_VOLATILE_GHOUL},
    {2436.882f,  -5167.232f,  79.88364f,  1.7453f, NPC_VOLATILE_GHOUL},
    {2424.053f,  -5136.897f,  83.67917f,  0,       NPC_WARRIOR_OF_THE_FROZEN_WASTES},
};

// spawn locations for various creatures
static sSpawnLocation aEventLocations[] =
{
    {2165.711f, -5266.124f, 95.50f, 0.13f},         // 0 Tirion spawn location
    {2280.159f, -5263.561f, 81.15f, 4.70f},         // 1 Alexandros spawn location
    {2280.538f, -5280.103f, 82.41f, 1.60f},         // 2 Young Darion spawn location
    {2280.304f, -5257.205f, 80.09f, 4.62f},         // 3 Lich King spawn location
};

class world_map_ebon_hold : public ScriptedInstance
{
    public:
        world_map_ebon_hold(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void Update(const uint32 diff) override;

        bool CanAndToggleGothikYell();

        void DoUpdateBattleWorldState(uint32 uiStateId, uint32 uiStateData);
        void DoUpdateBattleWeatherData(uint32 weatherId, uint32 weatherGrade);
        void DoUpdateBattleZoneLightData(uint32 lightId, uint32 fadeInTime);

    protected:
        void DoSpawnUndeadWave();
        void DoEnableHolyTraps();
        void DoDefeatUndeadArmy();
        void DoResetBattleScene();
        void DoClearBattleScene();

        uint32 m_uiGothikYellTimer;                         // Timer to check if Gothik can yell (related q 12698)
        uint32 m_uiBattleEncounter;                         // Store state of the battle around  "The Light of Dawn"
        uint32 m_uiBattleProgressTimer;                     // Light of Dawn battle timer
        uint32 m_uiTrapUsageTimer;                          // Light of Dawn trap usage

        uint32 m_uiDefendersDead;
        uint32 m_uiAttackersDead;
        uint32 m_uiAbominationsDead;

        GuidList m_lFleshBehemothGuids;
        GuidList m_lUndeadArmyGuids;
        GuidList m_lLightDefendersGuids;
        GuidList m_lLightTrapsGuids;
};

#endif
