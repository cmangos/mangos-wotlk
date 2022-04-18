/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_TRIAL_OF_THE_CRUSADER_H
#define DEF_TRIAL_OF_THE_CRUSADER_H

#include "Chat/Chat.h"

enum
{
    MAX_ENCOUNTER               = 7,
    MAX_WIPES_ALLOWED           = 50,
    MAX_CRUSADERS_10MAN         = 6,
    MAX_CRUSADERS_25MAN         = 10,
    MAX_CRUSADERS_HEALERS       = 4,
    MAX_CRUSADERS_OTHER         = 6,

    MIN_ACHIEV_MISTRESSES       = 2,
    MIN_ACHIEV_SNOBOLDS_10      = 2,
    MIN_ACHIEV_SNOBOLDS_25      = 4,

    TYPE_WIPE_COUNT             = 0,
    TYPE_NORTHREND_BEASTS       = 1,
    TYPE_JARAXXUS               = 2,
    TYPE_FACTION_CHAMPIONS      = 3,
    TYPE_TWIN_VALKYR            = 4,
    TYPE_ANUBARAK               = 5,
    TYPE_IMMORTALITY_FAILED     = 6,                       // Achievements A Tribute to Immortality, needs to be saved to database

    EVENT_OPEN_PORTAL           = 6,
    EVENT_KILL_FIZZLEBANG       = 7,
    EVENT_JARAXXUS_START_ATTACK = 8,
    EVENT_SUMMON_TWINS          = 9,
    EVENT_TWINS_KILLED          = 10,
    EVENT_ARTHAS_PORTAL         = 11,
    EVENT_SUMMON_THE_LICHKING   = 12,
    EVENT_DESTROY_FLOOR         = 13,
    EVENT_JARAXXUS_RESET_DELAY  = 14,
    EVENT_CHAMPIONS_ATTACK      = 15,
    EVENT_TWINS_ATTACK          = 16,

    NPC_BEASTS_COMBAT_STALKER   = 36549,
    NPC_BEASTS_CONTROLLER       = 35014,
    NPC_CHAMPIONS_CONTROLLER    = 34781,
    NPC_VALKYR_TWINS_CONTROLLER = 34743,
    NPC_VALKYR_STALKER_DARK     = 34704,                    // summons 34628 using 66107
    NPC_VALKYR_STALKER_LIGHT    = 34720,                    // summons 34630 using 66078

    NPC_NERUBIAN_BURROW         = 34862,                    // visual target for Nerubian Borrower; has aura 66324 in c_t_a
    NPC_NERUBIAN_BURROWER       = 34607,
    NPC_SCARAB                  = 34605,

    NPC_GORMOK                  = 34796,
    NPC_ACIDMAW                 = 35144,
    NPC_DREADSCALE              = 34799,
    NPC_ICEHOWL                 = 34797,
    NPC_JARAXXUS                = 34780,
    NPC_FJOLA                   = 34497,
    NPC_EYDIS                   = 34496,
    NPC_ANUBARAK                = 34564,

    NPC_SNOBOLD_VASSAL          = 34800,                    // used in Gormok encounter
    NPC_MISTRESS_OF_PAIN        = 34826,                    // used in Jaraxxus encounter
    // NPC_JUMP_TARGET          = 35376,                    // used to mark the jump spot for the crusaders; currently not used
    // NPC_DARK_ESSENCE         = 34567,                    // npc spell click for spell 65684
    // NPC_LIGHT_ESSENCE        = 34568,                    // npc spell click for spell 65686

    // NPC_BEASTS_TAPLIST       = 35820,
    // NPC_CHAMPION_TAPLIST     = 35821,
    // NPC_ANUBARAK_TAPLIST     = 36099,

    NPC_ALLY_DEATH_KNIGHT       = 34461,                    // Tyrius Duskblade
    NPC_ALLY_DRUID_BALANCE      = 34460,                    // Kavina Grovesong
    NPC_ALLY_DRUID_RESTO        = 34469,                    // Melador Valestrider
    NPC_ALLY_HUNTER             = 34467,                    // Alyssia Moonstalker
    NPC_ALLY_MAGE               = 34468,                    // Noozle Whizzlestick
    NPC_ALLY_PALADIN_HOLY       = 34465,                    // Velanaa
    NPC_ALLY_PALADIN_RETRI      = 34471,                    // Baelnor Lightbearer
    NPC_ALLY_PRIEST_DISC        = 34466,                    // Anthar Forgemender
    NPC_ALLY_PRIEST_SHADOW      = 34473,                    // Brienna Nightfell
    NPC_ALLY_ROGUE              = 34472,                    // Irieth Shadowstep
    NPC_ALLY_SHAMAN_ENHA        = 34463,                    // Shaabad
    NPC_ALLY_SHAMAN_RESTO       = 34470,                    // Saamul
    NPC_ALLY_WARLOCK            = 34474,                    // Serissa Grimdabbler
    NPC_ALLY_WARRIOR            = 34475,                    // Shocuul

    NPC_HORDE_DEATH_KNIGHT      = 34458,                    // Gorgrim Shadowcleave
    NPC_HORDE_DRUID_BALANCE     = 34451,                    // Birana Stormhoof
    NPC_HORDE_DRUID_RESTO       = 34459,                    // Erin Misthoof
    NPC_HORDE_HUNTER            = 34448,                    // Ruj'kah
    NPC_HORDE_MAGE              = 34449,                    // Ginselle Blightslinger
    NPC_HORDE_PALADIN_HOLY      = 34445,                    // Liandra Suncaller
    NPC_HORDE_PALADIN_RETRI     = 34456,                    // Malithas Brightblade
    NPC_HORDE_PRIEST_DISC       = 34447,                    // Caiphus the Stern
    NPC_HORDE_PRIEST_SHADOW     = 34441,                    // Vivienne Blackwhisper
    NPC_HORDE_ROGUE             = 34454,                    // Maz'dinah
    NPC_HORDE_SHAMAN_ENHA       = 34455,                    // Broln Stouthorn
    NPC_HORDE_SHAMAN_RESTO      = 34444,                    // Thrakgar
    NPC_HORDE_WARLOCK           = 34450,                    // Harkzog
    NPC_HORDE_WARRIOR           = 34453,                    // Narrhok Steelbreaker

    NPC_ZHAAGRYM                = 35465,
    NPC_CAT                     = 35610,

    NPC_TIRION_A                = 34996,
    NPC_TIRION_B                = 36095,                    // Summoned after his text (Champions, you're alive! Not only have you defeated every challenge of the Trial of the Crusader, but also thwarted Arthas' plans! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of the Crusade's mages to transport you to the surface!) is said..
    NPC_ARGENT_MAGE             = 36097,                    // Summoned along with Tirion B
    NPC_VARIAN                  = 34990,
    NPC_GARROSH                 = 34995,
    NPC_FIZZLEBANG              = 35458,
    NPC_OPEN_PORTAL_TARGET      = 17965,
    NPC_WORLD_TRIGGER_LARGE     = 22517,                    // Used for Lich King summon event
    NPC_THE_LICHKING            = 16980,
    NPC_THE_LICHKING_VISUAL     = 35877,
    NPC_RAMSEY_1                = 34816,
    NPC_RAMSEY_2                = 35035,
    NPC_RAMSEY_3                = 35766,
    NPC_RAMSEY_4                = 35770,
    NPC_RAMSEY_5                = 35771,
    NPC_RAMSEY_6                = 35895,                    // Unknown what these three NPCs are used for, maybe horde events?
    NPC_RAMSEY_7                = 35909,
    NPC_RAMSEY_8                = 35910,

    NPC_PURPLE_RUNE             = 35651,

    GO_MAIN_GATE                = 195647,
    GO_WEST_GATE                = 195648,                   // entrance gate
    // GO_SOUTH_GATE            = 195649,                   // not used
    GO_NORTH_GATE               = 195650,                   // dummy entrance; used in Trial of the Champion
    GO_COLISEUM_FLOOR           = 195527,
    GO_WEB_DOOR                 = 195485,
    GO_PORTAL_DALARAN           = 195682,

    GO_CRUSADERS_CACHE          = 195631,
    GO_CRUSADERS_CACHE_25       = 195632,
    GO_CRUSADERS_CACHE_10_H     = 195633,
    GO_CRUSADERS_CACHE_25_H     = 195635,

    GO_TRIBUTE_CHEST_10H_01     = 195665,
    GO_TRIBUTE_CHEST_10H_25     = 195666,
    GO_TRIBUTE_CHEST_10H_45     = 195667,
    GO_TRIBUTE_CHEST_10H_50     = 195668,

    GO_TRIBUTE_CHEST_25H_01     = 195669,
    GO_TRIBUTE_CHEST_25H_25     = 195670,
    GO_TRIBUTE_CHEST_25H_45     = 195671,
    GO_TRIBUTE_CHEST_25H_50     = 195672,

    SPELL_OPEN_PORTAL           = 67864,
    SPELL_FEL_LIGHTNING_KILL    = 67888,
    SPELL_WILFRED_PORTAL        = 68424,
    SPELL_ENSLAVE_JARAXXUS      = 67924,                    // dummy aura that will hold the boss after evade
    // SPELL_LEAP               = 67382,                    // crusader jump inside the arena to the provided coords
    SPELL_ANCHOR_HERE           = 45313,                    // change respawn coords to the current position
    SPELL_ENCOUNTER_KILL_CREDIT = 68184,                    // kill credit for faction champions
    SPELL_RESILIENCE_FIX_CREDIT = 68620,                    // server side spell for achievs 3798, 3814
    SPELL_TWIN_EMPATHY_LIGHT    = 66132,                    // damage share aura; targets dark twin (Eydis)
    SPELL_TWIN_EMPATHY_DARK     = 66133,                    // damage share aura; targets light twin (Fjola)
    SPELL_ARTHAS_PORTAL         = 51807,
    SPELL_FROSTNOVA             = 68198,
    SPELL_CORPSE_TELEPORT       = 69016, // NYI
    SPELL_DESTROY_FLOOR_KNOCKUP = 68193,

    DISPLAYID_DESTROYED_FLOOR   = 9060,
    POINT_COMBAT_POSITION       = 10,

    WORLD_STATE_WIPES           = 4390,
    WORLD_STATE_WIPES_COUNT     = 4389,

    ACHIEV_START_VALKYRS_ID             = 21853,                // Twin Valkyers achievs 3799, 3815
    ACHIEV_START_ANUBARAK_10_ID         = 68186,                // Anub timed achievs 3800, 3816
    ACHIEV_START_ANUBARAK_25_ID         = 68515,

    ACHIEV_CRIT_UPPER_BACK_PAIN_10_N    = 11779,                // Icehowl achievs 3797, 3813
    ACHIEV_CRIT_UPPER_BACK_PAIN_10_H    = 11802,
    ACHIEV_CRIT_UPPER_BACK_PAIN_25_N    = 11780,
    ACHIEV_CRIT_UPPER_BACK_PAIN_25_H    = 11801,
    ACHIEV_CRIT_PAIN_SPIKE_10_N         = 11838,                // Jaraxxus achievs 3996, 3997
    ACHIEV_CRIT_PAIN_SPIKE_10_H         = 11861,
    ACHIEV_CRIT_PAIN_SPIKE_25_N         = 11839,
    ACHIEV_CRIT_PAIN_SPIKE_25_H         = 11862,
    // ACHIEV_CRIT_RESILIENCE_FIX_10_N  = 11803,                // Faction Champions achievs 3798, 3814
    // ACHIEV_CRIT_RESILIENCE_FIX_10_H  = 11804,
    // ACHIEV_CRIT_RESILIENCE_FIX_25_N  = 11799,
    // ACHIEV_CRIT_RESILIENCE_FIX_25_H  = 11800,
    // ACHIEV_CRIT_TWO_JORMUNGARS_10_N  = 12280,                // Twin Jormungars achievs 3936, 3937
    // ACHIEV_CRIT_TWO_JORMUNGARS_10_H  = 12281,
    // ACHIEV_CRIT_TWO_JORMUNGARS_25_N  = 12278,
    // ACHIEV_CRIT_TWO_JORMUNGARS_25_H  = 12279,

    ACHIEV_CRIT_TRIBUTE_SKILL_10_1      = 12344,                // Tribute chest achievs 3808, 3817
    ACHIEV_CRIT_TRIBUTE_SKILL_10_2      = 12345,
    ACHIEV_CRIT_TRIBUTE_SKILL_10_3      = 12346,
    ACHIEV_CRIT_TRIBUTE_SKILL_25_1      = 12338,
    ACHIEV_CRIT_TRIBUTE_SKILL_25_2      = 12339,
    ACHIEV_CRIT_TRIBUTE_SKILL_25_3      = 12340,
    ACHIEV_CRIT_TRIBUTE_MAD_SKILL_10_1  = 12347,                // Tribute chest achievs 3809, 3818
    ACHIEV_CRIT_TRIBUTE_MAD_SKILL_10_2  = 12348,
    ACHIEV_CRIT_TRIBUTE_MAD_SKILL_25_1  = 12341,
    ACHIEV_CRIT_TRIBUTE_MAD_SKILL_25_2  = 12342,
    ACHIEV_CRIT_TRIBUTE_INSANITY_10     = 12349,                // Tribute chest achievs 3810, 3819
    ACHIEV_CRIT_TRIBUTE_INSANITY_25     = 12343,
    ACHIEV_CRIT_TRUBUTE_INSANITY_D      = 12360,                // Tribute chest achiev 4080
    ACHIEV_CRIT_TRIBUTE_IMMORTALITY_H   = 12358,                // Overall 25 heroic achievs 4079, 4156
    ACHIEV_CRIT_TRIBUTE_IMMORTALITY_A   = 12359,
    // ToDo: missing achiev criterias for the rest of the achievs?

};

static const uint32 aAllyHealerCrusaders[MAX_CRUSADERS_HEALERS] = { NPC_ALLY_DRUID_RESTO, NPC_ALLY_PALADIN_HOLY, NPC_ALLY_PRIEST_DISC, NPC_ALLY_SHAMAN_RESTO };
static const uint32 aAllyReplacementCrusaders[MAX_CRUSADERS_HEALERS] = { NPC_ALLY_DRUID_BALANCE, NPC_ALLY_PALADIN_RETRI, NPC_ALLY_PRIEST_SHADOW, NPC_ALLY_SHAMAN_ENHA };
static const uint32 aAllyOtherCrusaders[MAX_CRUSADERS_OTHER] = { NPC_ALLY_DEATH_KNIGHT, NPC_ALLY_HUNTER, NPC_ALLY_MAGE, NPC_ALLY_ROGUE, NPC_ALLY_WARLOCK, NPC_ALLY_WARRIOR };

static const uint32 aHordeHealerCrusaders[MAX_CRUSADERS_HEALERS] = { NPC_HORDE_DRUID_RESTO, NPC_HORDE_PALADIN_HOLY, NPC_HORDE_PRIEST_DISC, NPC_HORDE_SHAMAN_RESTO };
static const uint32 aHordeReplacementCrusaders[MAX_CRUSADERS_HEALERS] = { NPC_HORDE_DRUID_BALANCE, NPC_HORDE_PALADIN_RETRI, NPC_HORDE_PRIEST_SHADOW, NPC_HORDE_SHAMAN_ENHA };
static const uint32 aHordeOtherCrusaders[MAX_CRUSADERS_OTHER] = { NPC_HORDE_DEATH_KNIGHT, NPC_HORDE_HUNTER, NPC_HORDE_MAGE, NPC_HORDE_ROGUE, NPC_HORDE_WARLOCK, NPC_HORDE_WARRIOR };

struct CrusadersLocation
{
    float fSourceX, fSourceY, fSourceZ, fSourceO, fTargetX, fTargetY, fTargetZ;
};

static const CrusadersLocation aAllyCrusadersLoc[MAX_CRUSADERS_25MAN] =
{
    {615.649f, 108.371f, 418.317f, 2.617f, 597.998f, 130.116f, 394.729f},
    {622.361f, 111.691f, 419.785f, 2.705f, 596.189f, 123.862f, 394.710f},
    {619.104f, 101.331f, 421.621f, 2.548f, 594.369f, 118.033f, 394.677f},
    {618.138f, 105.381f, 419.786f, 2.600f, 605.423f, 128.229f, 395.288f},
    {622.956f, 107.000f, 421.619f, 2.652f, 603.840f, 122.100f, 394.832f},
    {621.258f, 117.725f, 418.317f, 2.972f, 601.717f, 115.576f, 395.287f},
    {628.161f, 117.369f, 421.607f, 2.809f, 599.921f, 135.934f, 394.742f},
    {629.005f, 124.168f, 421.627f, 2.914f, 592.413f, 112.477f, 394.684f},
    {622.175f, 123.810f, 418.315f, 2.879f, 607.020f, 134.541f, 394.836f},
    {615.322f,  95.750f, 421.623f, 2.460f, 599.461f, 109.588f, 395.288f},
};

static const CrusadersLocation aHordeCrusadersLoc[MAX_CRUSADERS_25MAN] =
{
    {510.069f, 111.784f, 418.317f, 0.488f, 528.958f, 131.470f, 394.729f},
    {510.208f, 103.791f, 419.787f, 0.593f, 531.399f, 125.630f, 394.708f},
    {505.691f, 124.593f, 418.315f, 0.279f, 533.647f, 119.147f, 394.646f},
    {501.770f, 121.961f, 419.778f, 0.296f, 521.901f, 128.487f, 394.832f},
    {508.244f,  98.039f, 421.546f, 0.645f, 524.237f, 122.411f, 394.819f},
    {497.338f, 124.774f, 421.595f, 0.244f, 526.309f, 116.666f, 394.833f},
    {500.499f, 113.415f, 421.552f, 0.304f, 529.479f, 112.130f, 394.742f},               // the last 4 are guesswork, but they are pretty close
    {504.869f, 112.231f, 419.710f, 0.426f, 536.588f, 114.176f, 394.533f},
    {515.173f, 102.482f, 418.234f, 0.670f, 520.921f, 134.698f, 394.747f},
    {513.624f,  98.620f, 419.703f, 0.642f, 527.289f, 136.818f, 394.654f},
};

static const float aRamsayPositions[2][4] =
{
    {559.1528f, 90.55729f, 395.2734f, 5.078908f},           // Summon Position
    {563.556f,  78.72571f, 395.2125f, 0.0f}                 // Movement Position
};

static const float aSpawnPositions[][4] =
{
    {563.8941f, 137.3333f, 405.8467f, 0.0f},                // Beast combat stalker (Summoned when SAY_VARIAN_BEAST_1)
    {563.9358f, 229.8299f, 394.8061f, 4.694936f},           // Gormok (vehicle) (Summoned when SAY_VARIAN_BEAST_1)
    {564.3301f, 232.1549f, 394.8188f, 1.621917f},           // Dreadscale (Summoned when Tirion says SAY_TIRION_BEAST_2)
    {549.5139f, 170.1389f, 394.7965f, 5.009095f},           // Acidmaw (Summoned(?) 14s after Dreadscale)
    {563.6081f, 228.1491f, 394.7057f, 4.664022f},           // Icehowl (Summoned when SAY_TIRION_BEAST_3)
    {563.6007f, 208.5278f, 395.2696f, 4.729842f},           // Fizzlebang
    {563.8264f, 140.6563f, 393.9861f, 4.694936f},           // Jaraxxus
    {571.684f,  204.9028f, 399.263f,  4.590216f},           // Fjola
    {555.4514f, 205.8889f, 399.2634f, 4.886922f},           // Eydis
    {563.6996f, 175.9826f, 394.5042f, 4.694936f},           // World Trigger Large
    {563.5712f, 174.8351f, 394.4954f, 4.712389f},           // Lich King
    {563.6858f, 139.4323f, 393.9862f, 4.694936f},           // Purple Rune / Center Position
    {648.9169f, 131.0209f, 141.6159f, 0.0f},                // Tirion B
    {649.1610f, 142.0399f, 141.3060f, 0.0f},                // Argent mage
};

static const float aMovePositions[][3] =
{
    {563.748f,  179.766f,  394.4862f},                      // Gormok
    {576.5347f, 168.9514f, 394.7064f},                      // Dreadscale
    {563.8577f, 176.5885f, 394.4417f},                      // Icehowl
    {563.7223f, 131.2344f, 393.9901f},                      // Jaraxxus
};

class instance_trial_of_the_crusader : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_trial_of_the_crusader(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature) override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnPlayerDeath(Player* pPlayer) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        // Difficulty wrappers
        bool IsHeroicDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }
        bool Is25ManDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }

        uint32 GetPlayerTeam() const { return m_uiTeam; }
        void GetStalkersGUIDVector(GuidVector& vVector) const { vVector = m_vStalkersGuidsVector; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget = nullptr, uint32 uiMiscvalue1 = 0) const override;

        void DoSetCrusadersInCombat(Unit* pTarget);
        void DoOpenMainGate(uint32 uiResetTimer);

        void Update(const uint32 diff) override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void DoSummonRamsey(uint32 uiEntry);
        void JustDidDialogueStep(int32 iEntry) override;
        void DoHandleEventEpilogue();

        void DoSelectCrusaders();
        void DoCleanupCrusaders();

        void HandleDestroyFloor();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        std::vector<uint32> m_vCrusadersEntries;

        GuidVector m_vCrusadersGuidsVector;
        GuidVector m_vStalkersGuidsVector;
        GuidList m_lSummonedGuidsList;

        Team m_uiTeam;

        uint32 m_uiGateResetTimer;
        uint32 m_uiKilledCrusaders;
        uint32 m_uiCrusadersAchievTimer;

        bool m_bCrusadersSummoned;
        bool m_bCrusadersAchievCheck;
};

#endif
