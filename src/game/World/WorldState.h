/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef WORLD_STATE_H
#define WORLD_STATE_H

#include "Policies/Singleton.h"
#include "Globals/ObjectMgr.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Globals/SharedDefines.h"
#include <atomic>
#include <functional>
#include <string>

enum ZoneIds
{
    ZONEID_DUN_MOROGH           = 1,
    ZONEID_ELWYNN_FOREST        = 12,
    ZONEID_AZUREMYST_ISLE       = 3524,
    ZONEID_DUROTAR              = 14,
    ZONEID_TIRISFAL_GLADES      = 85,
    ZONEID_EVERSONG_WOODS       = 3430,

    ZONEID_WINTERSPRING         = 618,
    ZONEID_AZSHARA              = 16,
    ZONEID_EASTERN_PLAGUELANDS  = 139,
    ZONEID_BLASTED_LANDS        = 4,
    ZONEID_BURNING_STEPPES      = 46,
    ZONEID_TANARIS              = 440,
    ZONEID_UNDERCITY_A          = 1497,

    ZONEID_STORMWIND_CITY       = 1519,
    ZONEID_DARNASSUS            = 1657,
    ZONEID_IRONFORGE            = 1537,
    ZONEID_ORGRIMMAR            = 1637,
    ZONEID_THUNDER_BLUFF        = 1638,
    ZONEID_UNDERCITY            = 1497,

    ZONEID_HELLFIRE_PENINSULA   = 3483,
    ZONEID_HELLFIRE_RAMPARTS    = 3562,
    ZONEID_HELLFIRE_CITADEL     = 3563,
    ZONEID_BLOOD_FURNACE        = 3713,
    ZONEID_SHATTERED_HALLS      = 3714,
    ZONEID_MAGTHERIDON_LAIR     = 3836,

    ZONEID_SHATTRATH    = 3703,
    ZONEID_BOTANICA     = 3847,
    ZONEID_ARCATRAZ     = 3848,
    ZONEID_MECHANAR     = 3849,

    ZONEID_ISLE_OF_QUEL_DANAS   = 4080,
    ZONEID_MAGISTERS_TERRACE    = 4131,
    ZONEID_SUNWELL_PLATEAU      = 4075,

    ZONEID_ZANGARMARSH = 3521,
};

enum AreaIds
{
    AREAID_SKYGUARD_OUTPOST     = 3964,
    AREAID_SHARTUUL_TRANSPORTER = 4008,
    AREAID_DEATHS_DOOR          = 3831,
    AREAID_THERAMORE_ISLE       = 513,

    AREAID_GOLDSHIRE            = 87,
    AREAID_KHARANOS             = 131,
    AREAID_AZURE_WATCH          = 3576,
    AREAID_BRILL                = 159,
    AREAID_RAZOR_HILL           = 362,
    AREAID_FALCONWING_SQUARE    = 3665,
};

enum SpellId
{
    SPELL_TROLLBANES_COMMAND    = 39911,
    SPELL_NAZGRELS_FAVOR        = 39913,

    SPELL_ADAL_SONG_OF_BATTLE   = 39953,

    SPELL_KIRU_SONG_OF_VICTORY  = 46302,

    SPELL_BONFIRES_BLESSING     = 45444,
};

enum GoId
{
    OBJECT_MAGTHERIDONS_HEAD    = 184640,
    OBJECT_EVENT_TRAP_THRALL    = 181088,
    OBJECT_EVENT_TRAP_BOLVAR    = 181089,
    OBJECT_EVENT_TRAP_MAGNI     = 181090,
    OBJECT_EVENT_TRAP_TYRANDE   = 181091,
    OBJECT_EVENT_TRAP_CAIRNE    = 181092,
    OBJECT_EVENT_TRAP_SYLVANAS  = 181093,
};

enum Conditions
{
    ORGRIMMAR_UNDERCITY = 164871,
    GROMGOL_ORGRIMMAR   = 175080,
    GROMGOL_UNDERCITY   = 176495,

    WAR_EFFORT_DAYS_LEFT = 2113,
};

enum Events
{
    // vanilla
    CUSTOM_EVENT_YSONDRE_DIED,
    CUSTOM_EVENT_LETHON_DIED,
    CUSTOM_EVENT_EMERISS_DIED,
    CUSTOM_EVENT_TAERAR_DIED,
    CUSTOM_EVENT_LOVE_IS_IN_THE_AIR_LEADER,
    // TBC
    CUSTOM_EVENT_ADALS_SONG_OF_BATTLE,
    CUSTOM_EVENT_HIGHLORD_KRUUL_DIED,
};

enum SaveIds
{
    SAVE_ID_EMERALD_DRAGONS = 0,
    SAVE_ID_AHN_QIRAJ = 1,
    SAVE_ID_LOVE_IS_IN_THE_AIR = 2,
    SAVE_ID_SCOURGE_INVASION = 3,

    SAVE_ID_QUEL_DANAS = 20,
    SAVE_ID_EXPANSION_RELEASE = 21,
    SAVE_ID_HIGHLORD_KRUUL = 22,
};

enum GameEvents
{
    GAME_EVENT_GURUBASHI_ARENA = 16,

    GAME_EVENT_SPIRIT_OF_COMPETITION = 85,

    GAME_EVENT_BREWFEST_DARK_IRON_ATTACK = 86,
    GAME_EVENT_BREWFEST_KEG_TAPPING      = 87,

    // Prepatch event
    GAME_EVENT_BEFORE_THE_STORM = 100,

    // AQ
    // giving items
    GAME_EVENT_AHN_QIRAJ_EFFORT_PHASE_1 = 120,
    // done with items - 5 days
    GAME_EVENT_AHN_QIRAJ_EFFORT_PHASE_2 = 121,
    // gong enabled
    GAME_EVENT_AHN_QIRAJ_EFFORT_PHASE_3 = 122,
    // 10 hour war
    GAME_EVENT_AHN_QIRAJ_EFFORT_PHASE_4 = 123,
    // base perpetual state
    GAME_EVENT_AHN_QIRAJ_EFFORT_PHASE_5 = 124,

    // Scourge Invasion
    GAME_EVENT_SCOURGE_INVASION                         = 17,
    GAME_EVENT_SCOURGE_INVASION_WINTERSPRING            = 90,
    GAME_EVENT_SCOURGE_INVASION_TANARIS                 = 91,
    GAME_EVENT_SCOURGE_INVASION_AZSHARA                 = 92,
    GAME_EVENT_SCOURGE_INVASION_BLASTED_LANDS           = 93,
    GAME_EVENT_SCOURGE_INVASION_EASTERN_PLAGUELANDS     = 94,
    GAME_EVENT_SCOURGE_INVASION_BURNING_STEPPES         = 95,
    GAME_EVENT_SCOURGE_INVASION_50_INVASIONS            = 96,
    GAME_EVENT_SCOURGE_INVASION_100_INVASIONS           = 97,
    GAME_EVENT_SCOURGE_INVASION_150_INVASIONS           = 98,
    GAME_EVENT_SCOURGE_INVASION_INVASIONS_DONE          = 99,

    // Zombie infestation
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_1               = 110,
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_2               = 111,
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_3               = 112,
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_4               = 113,
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_5               = 114,
    GAME_EVENT_ZOMBIE_INFESTATION_PHASE_6               = 115,

    // Isle phases
    GAME_EVENT_QUEL_DANAS_PHASE_1               = 301,
    GAME_EVENT_QUEL_DANAS_PHASE_2_ONLY          = 302,
    GAME_EVENT_QUEL_DANAS_PHASE_2_PERMANENT     = 303,
    GAME_EVENT_QUEL_DANAS_PHASE_2_NO_PORTAL     = 304,
    GAME_EVENT_QUEL_DANAS_PHASE_2_PORTAL        = 305,
    GAME_EVENT_QUEL_DANAS_PHASE_3_ONLY          = 306,
    GAME_EVENT_QUEL_DANAS_PHASE_3_PERMANENT     = 307,
    GAME_EVENT_QUEL_DANAS_PHASE_3_NO_ANVIL      = 308,
    GAME_EVENT_QUEL_DANAS_PHASE_3_ANVIL         = 309,
    GAME_EVENT_QUEL_DANAS_PHASE_4               = 310,
    GAME_EVENT_QUEL_DANAS_PHASE_4_NO_MONUMENT   = 311,
    GAME_EVENT_QUEL_DANAS_PHASE_4_MONUMENT      = 312,
    GAME_EVENT_QUEL_DANAS_PHASE_4_NO_ALCHEMY_LAB= 313,
    GAME_EVENT_QUEL_DANAS_PHASE_4_ALCHEMY_LAB   = 314,
    GAME_EVENT_QUEL_DANAS_PHASE_4_KIRU          = 315,
    // SWP Phases
    GAME_EVENT_SWP_GATES_PHASE_0 = 316, // All Gates Closed
    GAME_EVENT_SWP_GATES_PHASE_1 = 317, // First Gate Open
    GAME_EVENT_SWP_GATES_PHASE_2 = 318, // Second Gate Open
    GAME_EVENT_SWP_GATES_PHASE_3 = 319, // All Gates Open

    // wotlk range for events
    GAME_EVENT_ECHOES_OF_DOOM = 401,
};

enum AQResources
{
    // Horde
    AQ_PEACEBLOOM,
    AQ_LEAN_WOLF_STEAK,
    AQ_TIN_BAR,
    AQ_WOOL_BANDAGE,
    AQ_FIREBLOOM,
    AQ_HEAVY_LEATHER,
    AQ_MITHRIL_BAR,
    AQ_MAGEWEAVE_BANDAGE,
    AQ_RUGGED_LEATHER,
    AQ_BAKED_SALMON,
    // Alliance
    AQ_LIGHT_LEATHER,
    AQ_LINEN_BANDAGE,
    AQ_MEDIUM_LEATHER,
    AQ_STRANGLEKELP,
    AQ_RAINBOW_FIN_ALBACORE,
    AQ_IRON_BAR,
    AQ_ROAST_RAPTOR,
    AQ_SILK_BANDAGE,
    AQ_THORIUM_BAR,
    AQ_ARTHAS_TEARS,
    // Common
    AQ_COPPER_BAR_ALLY,
    AQ_PURPLE_LOTUS_ALLY,
    AQ_THICK_LEATHER_ALLY,
    AQ_SPOTTED_YELLOWTAIL_ALLY,
    AQ_RUNECLOTH_BANDAGE_ALLY,
    RESOURCE_UNIQUE_MAX = AQ_RUNECLOTH_BANDAGE_ALLY,
    AQ_COPPER_BAR_HORDE,
    AQ_PURPLE_LOTUS_HORDE,
    AQ_THICK_LEATHER_HORDE,
    AQ_SPOTTED_YELLOWTAIL_HORDE,
    AQ_RUNECLOTH_BANDAGE_HORDE,
    RESOURCE_MAX,
};

enum AQResourceGroup
{
    RESOURCE_GROUP_SKINNING,
    RESOURCE_GROUP_BANDAGES,
    RESOURCE_GROUP_BARS,
    RESOURCE_GROUP_COOKING,
    RESOURCE_GROUP_HERBS,
};

enum AQPhase
{
    PHASE_0_DISABLED,
    PHASE_1_GATHERING_RESOURCES,
    PHASE_2_TRANSPORTING_RESOURCES,
    PHASE_3_GONG_TIME,
    PHASE_4_10_HOUR_WAR,
    PHASE_5_DONE,
};

struct AhnQirajData
{
    uint32 m_phase;
    uint64 m_timer;
    uint32 m_WarEffortCounters[RESOURCE_MAX];
    GuidVector m_warEffortWorldstatesPlayers;
    std::mutex m_warEffortMutex;
    std::set<uint32> m_spawnedDbGuids;
    uint32 m_phase2Tier;
    AhnQirajData() : m_phase(PHASE_0_DISABLED), m_timer(0), m_phase2Tier(0)
    {
        memset(m_WarEffortCounters, 0, sizeof(m_WarEffortCounters));
    }
    std::string GetData();
    uint32 GetDaysRemaining() const;
};

enum SIState : uint32
{
    STATE_0_DISABLED,
    STATE_1_ENABLED,
    STATE_2_DEBUG,
    SI_STATE_MAX,
};

enum SIZoneIds
{
    SI_ZONE_AZSHARA,
    SI_ZONE_BLASTED_LANDS,
    SI_ZONE_BURNING_STEPPES,
    SI_ZONE_EASTERN_PLAGUELANDS,
    SI_ZONE_TANARIS,
    SI_ZONE_WINTERSPRING,
    SI_ZONE_STORMWIND,
    SI_ZONE_UNDERCITY
};

enum SITimers
{
    SI_TIMER_AZSHARA,
    SI_TIMER_BLASTED_LANDS,
    SI_TIMER_BURNING_STEPPES,
    SI_TIMER_EASTERN_PLAGUELANDS,
    SI_TIMER_TANARIS,
    SI_TIMER_WINTERSPRING,
    SI_TIMER_STORMWIND,
    SI_TIMER_UNDERCITY,
    SI_TIMER_MAX,
};

enum SIRemaining
{
    SI_REMAINING_AZSHARA,
    SI_REMAINING_BLASTED_LANDS,
    SI_REMAINING_BURNING_STEPPES,
    SI_REMAINING_EASTERN_PLAGUELANDS,
    SI_REMAINING_TANARIS,
    SI_REMAINING_WINTERSPRING,
    SI_REMAINING_MAX,
};

struct ScourgeInvasionData
{
    struct InvasionZone
    {
        uint32 map;
        uint32 zoneId;
        uint32 remainingVar;
        uint32 necroAmount;
        ObjectGuid mouthGuid;
        std::vector<Position> mouth;
    };

    struct CityAttack
    {
        uint32 map;
        uint32 zoneId;
        ObjectGuid pallidGuid;
        std::vector<Position> pallid;
    };

    SIState m_state;

    TimePoint m_timers[SI_TIMER_MAX];
    uint32 m_battlesWon;
    uint32 m_lastAttackZone;
    uint32 m_remaining[SI_REMAINING_MAX];
    uint64 m_broadcastTimer;
    std::mutex m_siMutex;

    std::set<uint32> m_pendingInvasions;
    std::set<uint32> m_pendingPallids;
    std::map<uint32, InvasionZone> m_invasionPoints;
    std::map<uint32, CityAttack> m_attackPoints;

    ScourgeInvasionData();

    void Reset();
    std::string GetData();
};

enum SunsReachPhases
{
    SUNS_REACH_PHASE_1_STAGING_AREA,
    SUNS_REACH_PHASE_2_SANCTUM,
    SUNS_REACH_PHASE_3_ARMORY,
    SUNS_REACH_PHASE_4_HARBOR,
};

enum SunsReachSubPhases
{
    SUBPHASE_PORTAL         = 0x01,
    SUBPHASE_ANVIL          = 0x02,
    SUBPHASE_ALCHEMY_LAB    = 0x04,
    SUBPHASE_MONUMENT       = 0x08,
    SUBPHASE_ALL = SUBPHASE_PORTAL + SUBPHASE_ANVIL + SUBPHASE_ALCHEMY_LAB + SUBPHASE_MONUMENT,
};

enum SunsReachCounters
{
    COUNTER_ERRATIC_BEHAVIOR,
    COUNTER_SANCTUM_WARDS,
    COUNTER_BATTLE_FOR_THE_SUNS_REACH_ARMORY,
    COUNTER_DISTRACTION_AT_THE_DEAD_SCAR,
    COUNTER_INTERCEPTING_THE_MANA_CELLS,
    COUNTER_INTERCEPT_THE_REINFORCEMENTS,
    COUNTER_TAKING_THE_HARBOR,
    COUNTER_MAKING_READY,
    COUNTER_DISCOVERING_YOUR_ROOTS,
    COUNTER_A_CHARITABLE_DONATION,
    COUNTERS_MAX,
};

enum SunwellGates
{
    SUNWELL_ALL_GATES_CLOSED,
    SUNWELL_AGAMATH_GATE1_OPEN,
    SUNWELL_ROHENDOR_GATE2_OPEN,
    SUNWELL_ARCHONISUS_GATE3_OPEN,
};

enum SunwellGateCounters
{
    COUNTER_AGAMATH_THE_FIRST_GATE,
    COUNTER_ROHENDOR_THE_SECOND_GATE,
    COUNTER_ARCHONISUS_THE_FINAL_GATE,
    COUNTERS_MAX_GATES,
};

struct SunsReachReclamationData
{
    uint32 m_phase;
    uint32 m_subphaseMask;
    uint32 m_sunsReachReclamationCounters[COUNTERS_MAX];
    uint32 m_gate;
    uint32 m_gateCounters[COUNTERS_MAX_GATES];
    GuidVector m_sunsReachReclamationPlayers;
    std::mutex m_sunsReachReclamationMutex;
    SunsReachReclamationData() : m_phase(SUNS_REACH_PHASE_1_STAGING_AREA), m_subphaseMask(0), m_gate(SUNWELL_ARCHONISUS_GATE3_OPEN) // optional - all SWP gates open by default in 2.4.3
    {
        memset(m_sunsReachReclamationCounters, 0, sizeof(m_sunsReachReclamationCounters));
        memset(m_gateCounters, 0, sizeof(m_gateCounters));
    }
    std::string GetData();
    uint32 GetPhasePercentage(uint32 phase);
    uint32 GetSubPhasePercentage(uint32 subPhase);
    uint32 GetSunwellGatePercentage(uint32 gate);
};

enum LoveIsInTheAirLeaders
{
    LOVE_LEADER_THRALL,
    LOVE_LEADER_CAIRNE,
    LOVE_LEADER_SYLVANAS,
    LOVE_LEADER_BOLVAR,
    LOVE_LEADER_MAGNI,
    LOVE_LEADER_TYRANDE,
    LOVE_LEADER_HORDE,
    LOVE_LEADER_ALLIANCE,
    LOVE_LEADER_MAX,
};

struct LoveIsInTheAir
{
    uint32 counters[LOVE_LEADER_MAX]; // potential race condition which wont cause anything critical
};

// Intended for implementing server wide scripts, note: all behaviour must be safeguarded towards multithreading
class WorldState
{
    public:
        WorldState();
        virtual ~WorldState();

        void Load();
        void Save(SaveIds saveId);
        void SaveHelper(std::string& stringToSave, SaveIds saveId);

        // Called when a gameobject is created or removed
        void HandleGameObjectUse(GameObject* go, Unit* user);
        void HandleGameObjectRevertState(GameObject* go);

        void HandlePlayerEnterZone(Player* player, uint32 zoneId);
        void HandlePlayerLeaveZone(Player* player, uint32 zoneId);

        void HandlePlayerEnterArea(Player* player, uint32 areaId);
        void HandlePlayerLeaveArea(Player* player, uint32 areaId);

        bool IsConditionFulfilled(uint32 conditionId, uint32 state) const;
        void HandleConditionStateChange(uint32 conditionId, uint32 state);

        void HandleExternalEvent(uint32 eventId, uint32 param);
        void ExecuteOnAreaPlayers(uint32 areaId, std::function<void(Player*)> executor);
        void ExecuteOnZonePlayers(uint32 zoneId, std::function<void(Player*)> executor);

        void Update(const uint32 diff);

        void SendWorldstateUpdate(std::mutex& mutex, GuidVector const& guids, uint32 value, uint32 worldStateId);

        // vanilla section

        uint32 GetLoveIsInTheAirCounter(LoveIsInTheAirLeaders leader) { return m_loveIsInTheAirData.counters[leader]; }

        void AddWarEffortProgress(AQResources resource, uint32 count);
        void HandleWarEffortPhaseTransition(uint32 newPhase);
        void StartWarEffortEvent();
        void StopWarEffortEvent();
        void SpawnWarEffortGos();
        void ChangeWarEffortGoSpawns(AQResources resource, int32 forcedTier = -1);
        void ChangeWarEffortPhase2Tier(uint32 remainingDays);
        void DespawnWarEffortGuids(std::set<std::pair<uint32, Team>>& guids);
        AQPhase GetAqPhase() { return (AQPhase)m_aqData.m_phase; }
        std::pair<AQResourceGroup, Team> GetResourceInfo(AQResources resource);
        std::pair<uint32, uint32> GetResourceCounterAndMax(AQResourceGroup group, Team team);
        std::string GetAQPrintout();

        void SetScourgeInvasionState(SIState state);
        void StartScourgeInvasion();
        void StopScourgeInvasion();
        uint32 GetSIRemaining(SIRemaining remaining) const;
        uint32 GetSIRemainingByZone(uint32 zoneId) const;
        void SetSIRemaining(SIRemaining remaining, uint32 value);
        TimePoint GetSITimer(SITimers timer);
        void SetSITimer(SITimers timer, TimePoint timePoint);
        uint32 GetBattlesWon();
        void AddBattlesWon(int32 count);
        uint32 GetLastAttackZone();
        void SetLastAttackZone(uint32 zoneId);
        void BroadcastSIWorldstates();
        void HandleDefendedZones();

        void StartZoneEvent(SIZoneIds eventId);
        void StartNewInvasionIfTime(uint32 attackTimeVar, uint32 zoneId);
        void StartNewCityAttackIfTime(uint32 attackTimeVar, uint32 zoneId);
        void StartNewInvasion(uint32 zoneId);
        void StartNewCityAttack(uint32 zoneId);
        bool ResumeInvasion(ScourgeInvasionData::InvasionZone& zone);
        bool SummonMouth(Map* map, ScourgeInvasionData::InvasionZone& zone, Position position);
        bool SummonPallid(Map* map, ScourgeInvasionData::CityAttack& zone, Position position, uint32 spawnLoc);
        void HandleActiveZone(uint32 attackTimeVar, uint32 zoneId, uint32 remainingVar, TimePoint now);

        Map* GetMap(uint32 mapId, Position const& invZone);
        bool IsActiveZone(uint32 zoneId);
        uint32 GetActiveZones();
        uint32 GetTimerIdForZone(uint32 zoneId);

        void SetPallidGuid(uint32 zoneId, ObjectGuid guid);
        void SetMouthGuid(uint32 zoneId, ObjectGuid guid);
        void AddPendingInvasion(uint32 zoneId);
        void RemovePendingInvasion(uint32 zoneId);
        void AddPendingPallid(uint32 zoneId);
        void RemovePendingPallid(uint32 zoneId);

        void OnEnable(ScourgeInvasionData::InvasionZone& zone);
        void OnDisable(ScourgeInvasionData::InvasionZone& zone);
        void OnDisable(ScourgeInvasionData::CityAttack& zone);

        // tbc section
        void BuffMagtheridonTeam(Team team);
        void DispelMagtheridonTeam(Team team);

        void BuffAdalsSongOfBattle();
        void DispelAdalsSongOfBattle();

        // Release events
        uint8 GetExpansion() const { return m_expansion; }
        bool SetExpansion(uint8 expansion);

        // Sun's Reach Reclamation
        void AddSunsReachProgress(uint32 questId);
        void HandleSunsReachPhaseTransition(uint32 newPhase);
        void HandleSunsReachSubPhaseTransition(int32 subPhaseMask, bool initial = false);
        void SetSunsReachCounter(SunsReachCounters index, uint32 value);
        void StopSunsReachPhase(bool forward);
        void StartSunsReachPhase(bool initial = false);
        std::string GetSunsReachPrintout();
        void AddSunwellGateProgress(uint32 questId);
        void HandleSunwellGateTransition(uint32 newGate);
        void SetSunwellGateCounter(SunwellGateCounters index, uint32 value);
        void StopSunwellGatePhase();
        void StartSunwellGatePhase();

        // Midsummer
        uint32 IsBonfireInZone(Team team, uint32 zoneId);
        bool IsBonfireActive(uint32 entry);
        void SetBonfireActive(uint32 entry, bool team, bool apply);
        void SetBonfireZone(uint32 entry, uint32 zoneId, bool team);

        void FillInitialWorldStates(ByteBuffer& data, uint32& count, uint32 zoneId, uint32 areaId);

        // helper functions for world state list fill
        inline void FillInitialWorldStateData(ByteBuffer& data, uint32& count, uint32 state, uint32 value)
        {
            data << uint32(state);
            data << uint32(value);
            ++count;
        }

        inline void FillInitialWorldStateData(ByteBuffer& data, uint32& count, uint32 state, int32 value)
        {
            data << uint32(state);
            data << int32(value);
            ++count;
        }
    private:
        std::map<uint32, GuidVector> m_areaPlayers;
        std::map<uint32, GuidVector> m_zonePlayers;
        std::map<uint32, std::atomic<uint32>> m_transportStates; // atomic to avoid having to lock

        std::mutex m_mutex; // all World State operations are thread unsafe
        uint32 m_saveTimer;

        // vanilla section
        bool IsDragonSpawned(uint32 entry);
        void RespawnEmeraldDragons();

        uint8 m_emeraldDragonsState;
        uint32 m_emeraldDragonsTimer;
        std::vector<uint32> m_emeraldDragonsChosenPositions;
        AhnQirajData m_aqData;
        std::map<uint32, AQResources> m_aqWorldstateMapReverse;

        LoveIsInTheAir m_loveIsInTheAirData;
        GuidVector m_loveIsInTheAirCapitalsPlayers;

        std::mutex m_loveIsInTheAirMutex; // capital cities optimization

        ScourgeInvasionData m_siData;

        // tbc section
        bool m_isMagtheridonHeadSpawnedHorde;
        bool m_isMagtheridonHeadSpawnedAlliance;
        ObjectGuid m_guidMagtheridonHeadHorde;
        ObjectGuid m_guidMagtheridonHeadAlliance;
        GuidVector m_magtheridonHeadPlayers;

        GuidVector m_adalSongOfBattlePlayers;
        uint32 m_adalSongOfBattleTimer;

        SunsReachReclamationData m_sunsReachData;

        std::map<uint32, bool> m_midsummerBonfireStates;
        std::map<uint32, uint32> m_midsummerZoneIds[2];
        std::map<uint32, uint32> m_midsummerGoToZone;
        std::map<uint32, GuidVector> m_midsummerZonePlayers;

        std::mutex m_midsummerMutex;

        // Release Events
        void StartExpansionEvent();

        std::atomic<uint8> m_expansion;

        void RespawnHighlordKruul();

        bool m_highlordKruulSpawned;
        uint32 m_highlordKruulTimer;
        uint8 m_highlordKruulChosenPosition;
};

#define sWorldState MaNGOS::Singleton<WorldState>::Instance()

#endif

