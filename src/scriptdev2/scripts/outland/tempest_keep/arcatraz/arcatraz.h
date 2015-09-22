/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ARCATRAZ_H
#define DEF_ARCATRAZ_H

enum
{
    MAX_ENCOUNTER                   = 10,
    MAX_WARDENS                     = 7,

    TYPE_ENTRANCE                   = 0,
    TYPE_ZEREKETH                   = 1,
    TYPE_DALLIAH                    = 2,
    TYPE_SOCCOTHRATES               = 3,
    TYPE_HARBINGERSKYRISS           = 4,                    // Handled with ACID (FAIL of 20905, 20906, 20908, 20909, 20910, 20911)
    TYPE_WARDEN_1                   = 5,                    // Handled with ACID (20905 - Blazing Trickster, 20906 - Phase-Hunter)
    TYPE_WARDEN_2                   = 6,
    TYPE_WARDEN_3                   = 7,                    // Handled with ACID (20908 - Akkiris Lightning-Waker, 20909 - Sulfuron Magma-Thrower)
    TYPE_WARDEN_4                   = 8,                    // Handled with ACID (20910 - Twilight Drakonaar, 20911 - Blackwing Drakonaar)
    TYPE_WARDEN_5                   = 9,

    NPC_DALLIAH                     = 20885,
    NPC_SOCCOTHRATES                = 20886,
    NPC_MELLICHAR                   = 20904,                // Skyriss will kill this unit
    NPC_PRISON_APHPA_POD            = 21436,
    NPC_PRISON_BETA_POD             = 21437,
    NPC_PRISON_DELTA_POD            = 21438,
    NPC_PRISON_GAMMA_POD            = 21439,
    NPC_PRISON_BOSS_POD             = 21440,

    // intro event related
    NPC_PROTEAN_NIGHTMARE           = 20864,
    NPC_PROTEAN_HORROR              = 20865,
    NPC_ARCATRAZ_WARDEN             = 20859,
    NPC_ARCATRAZ_DEFENDER           = 20857,

    // Harbinger Skyriss event related (trash mobs are scripted in ACID)
    NPC_BLAZING_TRICKSTER           = 20905,                // phase 1
    NPC_PHASE_HUNTER                = 20906,
    NPC_MILLHOUSE                   = 20977,                // phase 2
    NPC_AKKIRIS                     = 20908,                // phase 3
    NPC_SULFURON                    = 20909,
    NPC_TW_DRAKONAAR                = 20910,                // phase 4
    NPC_BL_DRAKONAAR                = 20911,
    NPC_SKYRISS                     = 20912,                // phase 5

    GO_CORE_SECURITY_FIELD_ALPHA    = 184318,               // Door opened when Wrath-Scryer Soccothrates dies
    GO_CORE_SECURITY_FIELD_BETA     = 184319,               // Door opened when Dalliah the Doomsayer dies
    GO_SEAL_SPHERE                  = 184802,               // Shield 'protecting' mellichar
    GO_POD_ALPHA                    = 183961,               // Pod first boss wave
    GO_POD_BETA                     = 183963,               // Pod second boss wave
    GO_POD_DELTA                    = 183964,               // Pod third boss wave
    GO_POD_GAMMA                    = 183962,               // Pod fourth boss wave
    GO_POD_OMEGA                    = 183965,               // Pod fifth boss wave

    SPELL_TARGET_OMEGA              = 36852,                // Visual spell used by Mellichar
};

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const SpawnLocation aSummonPosition[5] =
{
    {478.326f, -148.505f, 42.56f, 3.19f},                   // Trickster or Phase Hunter
    {413.292f, -148.378f, 42.56f, 6.27f},                   // Millhouse
    {420.179f, -174.396f, 42.58f, 0.02f},                   // Akkiris or Sulfuron
    {471.795f, -174.58f,  42.58f, 3.06f},                   // Twilight or Blackwing Drakonaar
    {445.763f, -191.639f, 44.64f, 1.60f}                    // Skyriss
};

static const float aDalliahStartPos[4] = {118.6038f, 96.84682f, 22.44115f, 1.012f};
static const float aSoccotharesStartPos[4] = {122.1035f, 192.7203f, 22.44115f, 5.235f};

static const float aEntranceMoveLoc[3] = {82.020f, 0.306f, -11.026f};
static const float aEntranceSpawnLoc[4] = {173.471f, -0.138f, -10.101f, 3.123f};

class instance_arcatraz : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_arcatraz(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff) override;

    private:
        void JustDidDialogueStep(int32 iEntry) override;

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiResetDelayTimer;
        uint32 m_uiEntranceEventTimer;
        uint8 m_uiKilledWardens;

        GuidList m_lSkyrissEventMobsGuidList;
};

#endif
