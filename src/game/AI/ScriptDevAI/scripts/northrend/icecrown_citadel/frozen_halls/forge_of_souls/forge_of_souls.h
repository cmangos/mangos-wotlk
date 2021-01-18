/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_FORGE_OF_SOULS_H
#define DEF_FORGE_OF_SOULS_H

enum
{
    MAX_ENCOUNTER               = 2,
    TYPE_BRONJAHM               = 1,
    TYPE_DEVOURER_OF_SOULS      = 2,
    TYPE_ACHIEV_PHANTOM_BLAST   = 3,

    DATA_SOULFRAGMENT_REMOVE    = 4,                        // on Death and on Use

    NPC_BRONJAHM                = 36497,
    NPC_DEVOURER_OF_SOULS       = 36502,
    NPC_CORRUPTED_SOUL_FRAGMENT = 36535,

    // Event NPCs
    NPC_SILVANA_BEGIN           = 37596,
    NPC_SILVANA_END             = 38161,
    NPC_JAINA_BEGIN             = 37597,
    NPC_JAINA_END               = 38160,
    NPC_ARCHMAGE_ELANDRA        = 37774,
    NPC_ARCHMAGE_KORELN         = 37582,
    NPC_DARK_RANGER_KALIRA      = 37583,
    NPC_DARK_RANGER_LORALEN     = 37779,
    NPC_COLISEUM_CHAMPION_A_P   = 37498,                    // Alliance Paladin
    NPC_COLISEUM_CHAMPION_A_F   = 37496,                    // Alliance Footman
    NPC_COLISEUM_CHAMPION_A_M   = 37497,                    // Alliance Mage
    NPC_COLISEUM_CHAMPION_H_F   = 37584,                    // Horde Footman
    NPC_COLISEUM_CHAMPION_H_T   = 37587,                    // Horde Taure
    NPC_COLISEUM_CHAMPION_H_M   = 37588,                    // Horde Mage

    ACHIEV_CRIT_SOUL_POWER      = 12752,
    ACHIEV_CRIT_PHANTOM_BLAST   = 12976,
};

struct sIntoEventNpcSpawnLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fSpawnX, fSpawnY, fSpawnZ, fSpawnO;
};

struct sExtroEventNpcLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fSpawnX, fSpawnY, fSpawnZ, fOrient;
    uint8 pathId;
};

const sIntoEventNpcSpawnLocations aEventBeginLocations[3] =
{
    {NPC_SILVANA_BEGIN,       NPC_JAINA_BEGIN,      4899.9766f, 2208.158f,  638.8166f, 0.3490654f},
    {NPC_DARK_RANGER_KALIRA,  NPC_ARCHMAGE_ELANDRA, 4902.829f,  2212.8315f, 638.8166f, 0.3490658f},
    {NPC_DARK_RANGER_LORALEN, NPC_ARCHMAGE_KORELN,  4899.8535f, 2205.9011f, 638.8166f, 5.4977869f},
};

const sExtroEventNpcLocations aEventEndLocations[19] =
{
    {NPC_SILVANA_END,           NPC_JAINA_END,             5606.34f,  2436.321f, 705.9351f, 0.89011f, 0},
    {NPC_DARK_RANGER_LORALEN,   NPC_ARCHMAGE_KORELN,       5606.351f, 2432.88f,  705.9351f, 0.78539f, 1},
    {NPC_DARK_RANGER_KALIRA,    NPC_ARCHMAGE_ELANDRA,      5602.804f, 2435.668f, 705.9351f, 0.95993f, 0},

    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5594.079f, 2425.111f, 705.9351f, 1.15191f, 0},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5590.505f, 2428.026f, 705.9352f, 0.80285f, 1},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5593.632f, 2428.58f,  705.9351f, 0.97738f, 2},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5600.848f, 2429.545f, 705.9351f, 0.89011f, 3},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5601.465f, 2426.771f, 705.9351f, 0.89011f, 4},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5598.036f, 2429.377f, 705.9351f, 1.04719f, 5},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5597.932f, 2421.781f, 705.9351f, 0.61086f, 6},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5600.836f, 2421.359f, 705.9351f, 0.89011f, 7},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 5601.587f, 2418.604f, 705.9351f, 1.15191f, 8},

    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 5592.267f, 2419.378f, 705.9351f, 0.85521f, 0},
    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 5594.647f, 2417.108f, 705.9351f, 0.90757f, 1},
    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 5589.813f, 2421.271f, 705.9351f, 0.85521f, 2},

    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 5586.8f,   2416.974f, 705.9351f, 0.94247f, 0},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 5585.499f, 2418.229f, 705.9351f, 1.06465f, 1},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 5591.653f, 2412.898f, 705.9351f, 0.92502f, 2},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 5593.937f, 2410.875f, 705.9351f, 0.87266f, 3},
};

class instance_forge_of_souls : public ScriptedInstance
{
    public:
        instance_forge_of_souls(Map* pMap);
        ~instance_forge_of_souls() {}

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;
        void SetData64(uint32 uiType, uint64 uiData) override;

        void OnPlayerEnter(Player* pPlayer) override;
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    protected:
        void ProcessEventNpcs(Player* pPlayer);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_bCriteriaPhantomBlastFailed;

        uint32 m_uiTeam;                                    // Team of first entered player, used to set if Jaina or Silvana to spawn

        GuidList m_luiSoulFragmentAliveGUIDs;
        GuidList m_lEventMobGUIDs;
};

#endif
