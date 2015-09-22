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

/* Still TODO
** We have 12 npc-entries to do moving, and often different waypoints for one entry
** Best way to handle the paths of these mobs is still open
*/

struct sExtroEventNpcLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fStartO, fEndO;                                   // Orientation for Spawning
    float fSpawnX, fSpawnY, fSpawnZ;
    float fEndX, fEndY, fEndZ;
};

// TODO: verify Horde - Entries
const sIntoEventNpcSpawnLocations aEventBeginLocations[3] =
{
    {NPC_SILVANA_BEGIN,       NPC_JAINA_BEGIN,      4901.25439f, 2206.861f, 638.8166f, 5.88175964f},
    {NPC_DARK_RANGER_KALIRA,  NPC_ARCHMAGE_ELANDRA, 4899.709961f, 2205.899902f, 638.817017f, 5.864306f},
    {NPC_DARK_RANGER_LORALEN, NPC_ARCHMAGE_KORELN,  4903.160156f, 2213.090088f, 638.817017f, 0.1745329f}
};

const sExtroEventNpcLocations aEventEndLocations[18] =
{
    // Horde Entry              Ally Entry                 O_Spawn     O_End      SpawnPos                             EndPos
    {NPC_SILVANA_END,           NPC_JAINA_END,             0.8901179f, 0.890118f, 5606.34033f, 2436.32129f, 705.9351f, 5638.404f, 2477.154f, 708.6932f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 0.9773844f, 1.780236f, 5593.632f, 2428.57983f, 705.9351f,   5695.879f, 2522.944f, 714.6915f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 1.1519173f, 1.78023f,  5594.079f, 2425.111f, 705.9351f,     5692.123f, 2522.613f, 714.6915f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 0.6108652f, 0.296706f, 5597.932f, 2421.78125f, 705.9351f,   5669.314f, 2540.029f, 714.6915f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 1.0471975f, 5.358161f, 5598.03564f, 2429.37671f, 705.9351f, 5639.267f, 2520.912f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 0.8901179f, 0.112373f, 5600.836f, 2421.35938f, 705.9351f,   5668.145f, 2543.854f, 714.6915f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 0.8901179f, 5.358161f, 5600.848f, 2429.54517f, 705.9351f,   5639.961f, 2522.936f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 0.8901179f, 5.347504f, 5601.46533f, 2426.77075f, 705.9351f, 5643.156f, 2525.342f, 708.6958f},
    {NPC_COLISEUM_CHAMPION_H_F, NPC_COLISEUM_CHAMPION_A_F, 1.1519173f, 0.232039f, 5601.587f, 2418.60425f, 705.9351f,   5670.483f, 2536.204f, 714.6915f},
    {NPC_DARK_RANGER_LORALEN,   NPC_ARCHMAGE_KORELN,       0.7853982f, 3.717551f, 5606.35059f, 2432.88013f, 705.9351f, 5688.9f,   2538.981f, 714.6915f},
    {NPC_DARK_RANGER_KALIRA,    NPC_ARCHMAGE_ELANDRA,      0.9599311f, 4.694936f, 5602.80371f, 2435.66846f, 705.9351f, 5685.069f, 2541.771f, 714.6915f},
    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 0.8552113f, 1.958489f, 5589.8125f, 2421.27075f, 705.9351f,  5669.351f, 2472.626f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 0.8552113f, 2.111848f, 5592.2666f, 2419.37842f, 705.9351f,  5665.927f, 2470.574f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_T, NPC_COLISEUM_CHAMPION_A_P, 0.9075712f, 2.196496f, 5594.64746f, 2417.10767f, 705.9351f, 5662.503f, 2468.522f, 708.6958f},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 1.0646508f, 0.837758f, 5585.49854f, 2418.22925f, 705.9351f, 5624.832f, 2473.713f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 0.9424777f, 0.837758f, 5586.80029f, 2416.97388f, 705.9351f, 5627.443f, 2472.236f, 708.6959f},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 0.9250245f, 0.977384f, 5591.653f, 2412.89771f, 705.9351f,   5637.912f, 2465.69f,  708.6959f},
    {NPC_COLISEUM_CHAMPION_H_M, NPC_COLISEUM_CHAMPION_A_M, 0.8726646f, 0.977384f, 5593.93652f, 2410.875f, 705.9351f,   5642.629f, 2474.331f, 708.6959f}
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
        void ProcessEventNpcs(Player* pPlayer, bool bChanged);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_bCriteriaPhantomBlastFailed;

        uint32 m_uiTeam;                                    // Team of first entered player, used to set if Jaina or Silvana to spawn

        GuidList m_luiSoulFragmentAliveGUIDs;
        GuidList m_lEventMobGUIDs;
};

#endif
