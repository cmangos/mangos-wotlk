/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_outland.h"
#include "World/WorldState.h"
#include "World/WorldStateDefines.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

/* *********************************************************
 *                     OUTLAND
 */
enum BashirNpcs
{
    // Timer NPC
    NPC_AETHER_TECH_APPRENTICE  = 23473,

    // Surrounding NPCs which despawn on start
    NPC_BASHIR_RAIDER           = 22241,
    NPC_BASHIR_SPELL_THIEF      = 22242,

    // NPCs
    NPC_SKYGUARD_AETHER_TECH    = 23241,
    NPC_SKYGUARD_RANGER         = 23242,
    NPC_SKYGUARD_LIEUTENANT     = 23430,

    // Vendors
    NPC_AETHER_TECH_ASSISTANT   = 23243,
    NPC_AETHER_TECH_ADEPT       = 23244,
    NPC_AETHER_TECH_MASTER      = 23245,

    // Enemies
    NPC_SLAVERING_SLAVE         = 23246,
    NPC_BASHIR_SUBPRIMAL        = 23247,
    NPC_BASHIR_CONTROLLER       = 23368,
    NPC_BASHIR_RECKONER         = 23332,
    NPC_BASHIR_INQUISITOR       = 23414,

    // Bosses
    NPC_BASHIR_FLESH_FIEND      = 23249,
    NPC_DISRUPTOR_TOWER         = 23250,
    NPC_GRAND_COLLECTOR         = 23333,
};

enum BashirData
{
    BASHIR_MAX_EVENTS = 15,

    SAY_LIEUTENANT_1 = -1015039,
    SAY_LIEUTENANT_2 = -1015040,
    SAY_LIEUTENANT_3 = -1015041,
    SAY_LIEUTENANT_4 = -1015042,
    SAY_RANGER_1     = -1015043,
    SAY_LIEUTENANT_5 = -1015044,
    SAY_LIEUTENANT_6 = -1015045,
    SAY_TECH_PHASE_1 = -1015046,
    SAY_TECH_PHASE_2 = -1015048,
    SAY_TECH_PHASE_3 = -1015052,
    SAY_TECH_END     = -1015054,

    PATH_ID_BIG_PATH = 1,

    PHASE_1_KILL_COUNT = 15,
    PHASE_2_KILL_COUNT = 15,
    PHASE_3_KILL_COUNT = 40,

    PHASE_1_INITIAL_SPAWN_COUNT = 5,
    PHASE_2_INITIAL_SPAWN_COUNT = 15,
    PHASE_3_INITIAL_SPAWN_COUNT = 18,

    PHASE_1_ATTACK_INTERVAL = 10000,
    PHASE_2_ATTACK_INTERVAL = 10000,
    PHASE_3_ATTACK_INTERVAL = 10000,

    CRYSTALFORGE_SLAVE_EVENT_RELAY_ID = 10140,
    CRYSTALFORGE_SLAVE_EVENT_CONTROLLER_ENTRY = 19656,

    GO_BASHIR_CRYSTALFORGE = 185921,
};

enum BashirActions
{
    BASHIR_ACTION_INTRO,
    BASHIR_ACTION_SPAWN_ENEMY,
    BASHIR_ACTION_ENEMY_ATTACK,
    BASHIR_ACTION_OUTRO,
};

enum BashirPhases : uint32
{
    BASHIR_PHASE_NOT_ACTIVE,
    BASHIR_PHASE_START,
    BASHIR_PHASE_1,
    BASHIR_PHASE_TRANSITION_1,
    BASHIR_PHASE_2,
    BASHIR_PHASE_TRANSITION_2,
    BASHIR_PHASE_3,
    BASHIR_PHASE_TRANSITION_3,
    BASHIR_PHASE_ALL_VENDORS_SPAWNED,
    BASHIR_DESPAWN,
};

float bashirSpawnPositions[][4] =
{
    // Initial spawns
    {2515.963f, 7331.445f, 375.0656f, 4.08407f}, // Tech
    {2508.059f, 7327.602f, 375.2871f, 5.51524f}, // Ranger 1
    {2509.62f,  7328.884f, 375.582f,  5.61996f}, // Ranger 2
    {2511.741f, 7324.75f,  374.971f,  1.832596f}, // Lieutenant
    {2510.918f, 7330.019f, 375.7001f, 5.5676f}, // Ranger 3
     // Orientations not blizzlike due to spawn method in sniff
    {3941.35f,  5911.558f, 383.8647f, 0.f}, // Assistant
    {3942.124f, 5913.187f, 385.3546f, 0.f}, // Adept
    {3942.078f, 5911.988f, 384.3665f, 0.f}, // Master
    // Bosses
    {3971.717f, 5844.253f, 266.7054f, 3.682645f}, // Bash'ir Flesh Fiend
    {3950.263f, 5872.112f, 266.1565f, 0.06981317f}, // Disruptor Tower 1
    {3959.747f, 5844.606f, 266.2597f, 2.86234f}, // Disruptor Tower 2
    {3985.801f, 5824.674f, 265.8477f, 3.543018f}, // Disruptor Tower 3
    {3982.51f,  5840.563f, 266.6467f, 1.27409f}, // The Grand Collector
    // {,,,},
};

struct BashirCombatSpawn
{
    float x;
    float y;
    float z;
    float ori;
    uint32 phaseId;
};

// Note: Official script has 2 bunnies that randomly run around with seemingly no purpose - entries 23424 and 23245
BashirCombatSpawn bashirCombatSpawnLocations[] =
{
    { 3954.099f, 5896.103f, 265.4047f, 4.263914f,  BASHIR_PHASE_2 }, // 23247
    { 3954.123f, 5895.753f, 265.333f,  4.513785f,  BASHIR_PHASE_1 }, // 23246
    { 3954.14f,  5883.52f,  265.8082f, 3.529384f,  BASHIR_PHASE_3 }, // 23368
    { 3954.405f, 5883.173f, 265.7686f, 3.341078f,  BASHIR_PHASE_1 }, // 23246
    { 3956.501f, 5867.157f, 265.7953f, 2.243402f,  BASHIR_PHASE_2 }, // 23247
    { 3956.555f, 5867.063f, 265.7869f, 0.1047198f, BASHIR_PHASE_1 }, // 23246
    { 3960.026f, 5851.23f,  267.0371f, 3.263047f,  BASHIR_PHASE_1 }, // 23246
    { 3960.457f, 5851.486f, 267.0367f, 2.412842f,  BASHIR_PHASE_3 }, // 23332
    { 3960.875f, 5901.212f, 266.748f,  3.714837f,  BASHIR_PHASE_2 }, // 23247
    { 3961.224f, 5901.752f, 266.8205f, 0.4712389f, BASHIR_PHASE_1 }, // 23246
    { 3962.268f, 5880.346f, 264.35f,   0.6926048f, BASHIR_PHASE_3 }, // 23368
    { 3963.943f, 5891.918f, 265.5939f, 3.599896f,  BASHIR_PHASE_3 }, // 23368
    { 3964.152f, 5891.268f, 265.5515f, 0.9250245f, BASHIR_PHASE_1 }, // 23246
    { 3964.454f, 5865.78f,  265.7123f, 4.060629f,  BASHIR_PHASE_3 }, // 23332
    { 3965.698f, 5852.543f, 267.077f,  3.417504f,  BASHIR_PHASE_3 }, // 23332
    { 3966.984f, 5837.517f, 265.152f,  0.721127f,  BASHIR_PHASE_1 }, // 23246
    { 3967.059f, 5837.698f, 265.1926f, 0.5973462f, BASHIR_PHASE_2 }, // 23247
    { 3970.29f,  5873.223f, 265.2673f, 0.3797616f, BASHIR_PHASE_2 }, // 23247
    { 3972.134f, 5859.938f, 266.3955f, 5.842862f,  BASHIR_PHASE_2 }, // 23247
    { 3975.669f, 5849.6f,   267.0623f, 6.156796f,  BASHIR_PHASE_2 }, // 23247
    { 3980.986f, 5834.263f, 266.3177f, 1.282679f,  BASHIR_PHASE_1 }, // 23246
    { 3981.247f, 5834.633f, 266.3542f, 5.053397f,  BASHIR_PHASE_3 }, // 23332
    { 3991.14f,  5839.3f,   266.884f,  1.796547f,  BASHIR_PHASE_2 }, // 23247
    { 3995.316f, 5832.168f, 266.8185f, 4.12309f,   BASHIR_PHASE_1 }, // 23246
    { 3995.352f, 5832.272f, 266.8245f, 2.670354f,  BASHIR_PHASE_2 }, // 23247
    { 4006.729f, 5821.15f,  266.9615f, 5.000666f,  BASHIR_PHASE_1 }, // 23246
    { 4007.123f, 5821.586f, 267.0208f, 1.544307f,  BASHIR_PHASE_3 }, // 23368
    { 4008.742f, 5831.805f, 267.5115f, 4.939476f,  BASHIR_PHASE_1 }, // 23246
    { 4017.789f, 5832.946f, 267.5005f, 4.570905f,  BASHIR_PHASE_1 }, // 23246
    { 4018.153f, 5833.46f,  267.4955f, 6.131096f,  BASHIR_PHASE_2 }, // 23247
    // {,,,, -1},
};

BashirCombatSpawn bashirCombatDisruptorSpawnLocations[] =
{
    { 3987.066f, 5828.364f, 266.4626f, 1.117011f,  BASHIR_PHASE_2 }, // 23247
    { 3982.202f, 5826.089f, 265.8585f, 0.9948376f, BASHIR_PHASE_2 }, // 23247
    { 3988.97f,  5822.692f, 265.44f,   1.27409f,   BASHIR_PHASE_2 }, // 23247
    { 3953.648f, 5874.088f, 266.0405f, 0.2268928f, BASHIR_PHASE_2 }, // 23247
    { 3949.731f, 5876.353f, 265.7751f, 0.1745329f, BASHIR_PHASE_2 }, // 23247
    { 3952.306f, 5868.657f, 266.1395f, 0.3141593f, BASHIR_PHASE_2 }, // 23247
    { 3962.829f, 5846.46f,  266.5654f, 0.715585f,  BASHIR_PHASE_2 }, // 23247
    { 3956.881f, 5847.137f, 266.8138f, 0.6632251f, BASHIR_PHASE_2 }, // 23247
    { 3961.659f, 5841.467f, 265.6436f, 0.7679449f, BASHIR_PHASE_2 }, // 23247
};

// Custom data for debugging purposes
float bashirCustomSpawnPositions[][4] =
{
    { 4013.02f,  5889.713f, 267.9485f, 3.803299f  }, // 23430
    { 4015.771f, 5889.592f, 267.9485f, 4.725542f  }, // 23432
    { 4010.755f, 5891.151f, 267.9267f, 3.627615f  }, // 23432
    { 4018.191f, 5893.194f, 267.9485f, 0.5996879f }, // 23431
    { 4017.951f, 5890.071f, 267.9485f, 5.132855f  }, // 23432
    { 4017.238f, 5894.595f, 267.9485f, 0.1919862f }, // 23243
    { 4020.681f, 5891.649f, 267.9485f, 1.466077f  }, // 23244
    { 4015.183f, 5891.058f, 267.9485f, 1.32645f   }, // 23245
    // {,,,},
};

struct world_map_outland : public ScriptedMap, public TimerManager
{
    world_map_outland(Map* pMap) : ScriptedMap(pMap) { Initialize(); }

    uint8 m_uiEmissaryOfHate_KilledAddCount;
    uint8 m_uiRazaan_KilledAddCount;

    uint32 m_uiBellTimer;
    uint32 m_uiBellTolls;
    std::vector<std::pair<ObjectGuid, uint32>> m_vBellGuids;

    // Worldstate variables
    uint32 m_deathsDoorEventActive;
    int32 m_deathsDoorNorthHP;
    int32 m_deathsDoorSouthHP;

    uint32 m_shartuulEventActive;
    uint32 m_shartuulShieldPercent;

    std::tm m_bashirTime;
    uint32 m_bashirTimer;
    BashirPhases m_bashirPhase;
    GuidVector m_bashirSpawns;
    GuidVector m_bashirEnemySpawns;
    GuidVector m_bashirEnemyWaveSpawns;
    Creature* m_apprentice;
    uint32 m_bashirIntroStage;
    uint32 m_deadTowerCounter;
    uint32 m_bashirKillCounter;
    std::vector<BashirCombatSpawn*> m_bashirPhaseSpawnData;
    std::vector<BashirCombatSpawn*> m_bashirPhaseSpawnDataUsed;

    uint32 GetIntroTimer(uint32 eventId)
    {
        switch (eventId)
        {
            case 0: return 5000;
            case 1: return 14000;
            case 2: return 2450;
            case 3: return 12000;
            case 4: return 14500;
            case 5: return 6500;
            case 6: return 4300;
            case 7: return 2450;
            case 8: return 6500;
            case 9: return 2400;
            case 10: return 2000;
            case 11: return 1100;
            case 12: return 1300;
            case 13: return 1200;
            case 14: return 1200;
            case 15: return 2600;
            default: return 0;
        }
    }

    void ScriptAction(uint32 eventId)
    {
        switch (eventId)
        {
            case 0: // talk
                DoScriptText(SAY_LIEUTENANT_1, GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT));
                break;
            case 1: // move and talk
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->GetMotionMaster()->MovePoint(0, 2514.943f, 7329.165f, 375.3175f);
                DoScriptText(SAY_LIEUTENANT_2, lieutenant);
                break;
            }
            case 2: // change orientation
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->SetOrientation(2.80998f);
                lieutenant->SetFacingTo(2.80998f);
                break;
            }
            case 3: // talk
                DoScriptText(SAY_LIEUTENANT_3, GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT));
                break;
            case 4: // talk
                DoScriptText(SAY_LIEUTENANT_4, GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT));
                break;
            case 5: // talk to lieutenant
                DoScriptText(SAY_RANGER_1, GetSingleCreatureFromStorage(NPC_SKYGUARD_RANGER), GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT)); // uses last ranger
                break;
            case 6: // move and talk
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->GetMotionMaster()->MovePoint(0, 2511.741f, 7324.75f, 375.0029f);
                DoScriptText(SAY_LIEUTENANT_5, lieutenant);
                break;
            }
            case 7: // change orientation
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->SetOrientation(1.832596f);
                lieutenant->SetFacingTo(1.832596f);
                break;
            }
            case 8: // talk
                DoScriptText(SAY_LIEUTENANT_6, GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT));
                break;
            case 9: // Lieutenant mount
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->Mount(MOUNT_NETHER_RAY_DISPLAY_ID);
                lieutenant->SetHover(true);
                lieutenant->SetWalk(false);
                break;
            }
            case 10: // Lieutenant path and others mount
            {
                Creature* lieutenant = GetSingleCreatureFromStorage(NPC_SKYGUARD_LIEUTENANT);
                lieutenant->GetMotionMaster()->MoveWaypoint(PATH_ID_BIG_PATH);
                lieutenant->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
                tech->Mount(MOUNT_NETHER_RAY_DISPLAY_ID);
                GuidVector rangerVector;
                GetCreatureGuidVectorFromStorage(NPC_SKYGUARD_RANGER, rangerVector);
                for (ObjectGuid guid : rangerVector)
                    instance->GetCreature(guid)->Mount(MOUNT_NETHER_RAY_DISPLAY_ID);
                break;
            }
            case 11: // Flags and hover
            {
                Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
                tech->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                tech->SetHover(true);
                tech->SetWalk(false);
                GuidVector rangerVector;
                GetCreatureGuidVectorFromStorage(NPC_SKYGUARD_RANGER, rangerVector);
                for (ObjectGuid guid : rangerVector)
                {
                    Creature* ranger = instance->GetCreature(guid);
                    ranger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    ranger->SetHover(true);
                    ranger->SetWalk(false);
                }
                break;
            }
            case 12: // first Ranger paths
            case 13:
            case 14:
            {
                GuidVector rangerVector;
                GetCreatureGuidVectorFromStorage(NPC_SKYGUARD_RANGER, rangerVector);
                instance->GetCreature(rangerVector[eventId - 12])->GetMotionMaster()->MoveWaypoint(PATH_ID_BIG_PATH);
                break;
            }
            case 15: // Tech paths
                Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
                tech->GetMotionMaster()->MoveWaypoint(PATH_ID_BIG_PATH);
                break;
        }
        ++m_bashirIntroStage;
        if (eventId != 16)
            ResetTimer(BASHIR_ACTION_INTRO, GetIntroTimer(m_bashirIntroStage));
    }

    void Initialize() override
    {
        m_uiEmissaryOfHate_KilledAddCount = 0;
        m_uiRazaan_KilledAddCount = 0;

        m_deathsDoorEventActive = 1;
        m_deathsDoorNorthHP = 100;
        m_deathsDoorSouthHP = 100;

        m_shartuulEventActive = 1;
        m_shartuulShieldPercent = 100;

        std::time_t now = time(nullptr);
        m_bashirTime = *std::gmtime(&now);
        m_bashirTimer = (60 - m_bashirTime.tm_sec) * IN_MILLISECONDS;
        m_bashirPhase = BASHIR_PHASE_NOT_ACTIVE;
        AddCustomAction(BASHIR_ACTION_INTRO, true, [&] { ScriptAction(m_bashirIntroStage); });
        AddCustomAction(BASHIR_ACTION_SPAWN_ENEMY, true, [&]
        {
            if (m_bashirPhaseSpawnData.size() > 0) // only when max is not spawned
                SpawnRandomBashirMob();
            ResetTimer(BASHIR_ACTION_SPAWN_ENEMY, GetActionTimer(BASHIR_ACTION_SPAWN_ENEMY));
        });
        AddCustomAction(BASHIR_ACTION_ENEMY_ATTACK, true, [&]
        {
            if (m_bashirEnemyWaveSpawns.size() > 0)
            {
                switch (m_bashirPhase)
                {
                    case BASHIR_PHASE_2:
                    {
                        uint32 slaveCount = 0;
                        for (ObjectGuid guid : m_bashirEnemyWaveSpawns)
                            if (guid.GetEntry() == NPC_SLAVERING_SLAVE)
                                ++slaveCount;
                        uint32 randomSlave = urand(1, slaveCount);
                        uint32 randomSubprimal = urand(1, m_bashirEnemyWaveSpawns.size() - slaveCount);
                        bool slaveSpawned = slaveCount > 0 ? false : true;
                        bool subprimalSpawned = m_bashirEnemyWaveSpawns.size() - slaveCount > 0 ? false : true;
                        for (auto itr = m_bashirEnemyWaveSpawns.begin(); itr != m_bashirEnemyWaveSpawns.end();)
                        {
                            ObjectGuid guid = (*itr);
                            if (guid.GetEntry() == NPC_SLAVERING_SLAVE)
                            {
                                if (slaveSpawned)
                                    continue;
                                --randomSlave;
                                if (randomSlave == 0)
                                {
                                    slaveSpawned = true;
                                    if (Creature* waveMob = instance->GetCreature(guid))
                                        waveMob->AI()->AttackStart(GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH));
                                    itr = m_bashirEnemyWaveSpawns.erase(itr);
                                }
                            }
                            else if (guid.GetEntry() == NPC_BASHIR_SUBPRIMAL)
                            {
                                if (subprimalSpawned)
                                    continue;
                                --randomSubprimal;
                                if (randomSubprimal == 0)
                                {
                                    subprimalSpawned = true;
                                    if (Creature* waveMob = instance->GetCreature(guid))
                                        waveMob->AI()->AttackStart(GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH));
                                    itr = m_bashirEnemyWaveSpawns.erase(itr);
                                }
                            }
                        }
                        break;
                    }
                    default:
                    {
                        uint32 index = urand(0, m_bashirEnemyWaveSpawns.size() - 1);
                        if (Creature* waveMob = instance->GetCreature(m_bashirEnemyWaveSpawns[index]))
                            waveMob->AI()->AttackStart(GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH));
                        m_bashirEnemyWaveSpawns.erase(m_bashirEnemyWaveSpawns.begin() + index);
                        break;
                    }
                }
            }
            ResetTimer(BASHIR_ACTION_ENEMY_ATTACK, GetActionTimer(BASHIR_ACTION_ENEMY_ATTACK));
        });
        AddCustomAction(BASHIR_ACTION_OUTRO, true, [&]
        {
            if (Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH))
            {
                DoScriptText(SAY_TECH_END, tech);
                tech->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, tech, tech);
            }
            DespawnBashir(false);
        });
    }

    uint32 GetActionTimer(BashirActions action)
    {
        switch (action)
        {
            case BASHIR_ACTION_INTRO: return 0;
            case BASHIR_ACTION_SPAWN_ENEMY: return 20000;
            case BASHIR_ACTION_ENEMY_ATTACK:
            {
                switch (m_bashirPhase)
                {
                    case BASHIR_PHASE_1: return 15000;
                    case BASHIR_PHASE_2: return 15000;
                    case BASHIR_PHASE_3: return 5000;
                    default: break;
                }
            }
            default: return 0;
        }
    }

    void OnCreatureCreate(Creature* creature) override
    {
        switch (creature->GetEntry())
        {
            case NPC_VIMGOL_VISUAL_BUNNY:
                m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
                break;
            case NPC_WYRM_FROM_BEYOND:
                if (Creature* creature = instance->GetCreature(m_npcEntryGuidStore[NPC_OSCILLATING_FREQUENCY_SCANNER_BUNNY]))
                    creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, creature, creature);
                break;
            case NPC_EMISSARY_OF_HATE:
            case NPC_WHISPER_RAVEN_GOD_TEMPLATE:
            case NPC_OSCILLATING_FREQUENCY_SCANNER_BUNNY:
            case NPC_SOCRETHAR:
            case NPC_DEATHS_DOOR_NORTH_WARP_GATE:
            case NPC_DEATHS_DOOR_SOUTH_WARP_GATE:
                m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
                break;
            case NPC_SKYGUARD_AETHER_TECH:
            case NPC_SKYGUARD_RANGER:
            case NPC_SKYGUARD_LIEUTENANT:
            case NPC_AETHER_TECH_ASSISTANT:
            case NPC_AETHER_TECH_ADEPT:
            case NPC_AETHER_TECH_MASTER:
                m_bashirSpawns.push_back(creature->GetObjectGuid());
                switch (creature->GetEntry())
                {
                    case NPC_SKYGUARD_RANGER: // store all + last ranger
                        m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
                    case NPC_SKYGUARD_AETHER_TECH:
                    case NPC_SKYGUARD_LIEUTENANT:
                        m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid(); // Note: we want last ranger to be stored
                        break;
                }
                break;
            case NPC_SLAVERING_SLAVE:
            case NPC_BASHIR_SUBPRIMAL:
            case NPC_BASHIR_CONTROLLER:
            case NPC_BASHIR_INQUISITOR:
            case NPC_BASHIR_FLESH_FIEND:
                m_bashirEnemyWaveSpawns.push_back(creature->GetObjectGuid());
            case NPC_DISRUPTOR_TOWER:
            case NPC_GRAND_COLLECTOR:
                m_bashirEnemySpawns.push_back(creature->GetObjectGuid());
                break;
            case NPC_AETHER_TECH_APPRENTICE: // always spawned
                m_apprentice = creature;
                creature->SetActiveObjectState(true);
                break;
            case NPC_OBSIDIA:
            case NPC_RIVENDARK:
            case NPC_FURYWING:
            case NPC_INSIDION:
                if (creature->IsTemporarySummon()) // Only dragons summoned by the player (by using the respective egg gameobjects) should have UNIT_FLAG_IMMUNE_TO_PLAYER
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                break;
        }
    }

    void OnCreatureDeath(Creature* creature) override
    {
        switch (creature->GetEntry())
        {
            case NPC_IRESPEAKER:
            case NPC_UNLEASHED_HELLION:
                if (!GetSingleCreatureFromStorage(NPC_EMISSARY_OF_HATE, true))
                {
                    ++m_uiEmissaryOfHate_KilledAddCount;
                    if (m_uiEmissaryOfHate_KilledAddCount == 6)
                    {
                        creature->SummonCreature(NPC_EMISSARY_OF_HATE, aSpawnLocations[POS_IDX_EMISSARY_SPAWN][0], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][1], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][2], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                        m_uiEmissaryOfHate_KilledAddCount = 0;
                    }
                }
                break;
            case NPC_SLAVERING_SLAVE:
            case NPC_BASHIR_SUBPRIMAL:
            case NPC_BASHIR_CONTROLLER:
            case NPC_BASHIR_RECKONER:
                m_bashirEnemyWaveSpawns.erase(std::remove(m_bashirEnemyWaveSpawns.begin(), m_bashirEnemyWaveSpawns.end(), creature->GetObjectGuid()), m_bashirEnemyWaveSpawns.end());
                HandleBashirMobDeath(creature);
                break;
            case NPC_BASHIR_FLESH_FIEND:
                m_bashirEnemyWaveSpawns.erase(std::remove(m_bashirEnemyWaveSpawns.begin(), m_bashirEnemyWaveSpawns.end(), creature->GetObjectGuid()), m_bashirEnemyWaveSpawns.end());
                FinishPhase(BASHIR_PHASE_1);
                break;
            case NPC_DISRUPTOR_TOWER:
                ++m_deadTowerCounter;
                if (m_deadTowerCounter == 3)
                    FinishPhase(BASHIR_PHASE_2);
                break;
            case NPC_GRAND_COLLECTOR:
                FinishPhase(BASHIR_PHASE_3);
                break;
        }
    }

    void HandleBashirMobDeath(Creature* creature)
    {
        for (auto itr = m_bashirPhaseSpawnDataUsed.begin(); itr != m_bashirPhaseSpawnDataUsed.end(); ++itr)
        {
            BashirCombatSpawn* data = (*itr);
            float x, y, z;
            creature->GetRespawnCoord(x,y,z);
            if (data->x == x && data->y == y)
            {
                m_bashirPhaseSpawnDataUsed.erase(itr);
                m_bashirPhaseSpawnData.push_back(data);
                break;
            }
        }
        ++m_bashirKillCounter;
        uint32 bossSpawnCount = 0;
        switch (m_bashirPhase)
        {
            case BASHIR_PHASE_1: bossSpawnCount = PHASE_1_KILL_COUNT; break;
            case BASHIR_PHASE_2: bossSpawnCount = PHASE_2_KILL_COUNT; break;
            case BASHIR_PHASE_3: bossSpawnCount = PHASE_3_KILL_COUNT; break;
            default: return; // should never occur
        }
        if (bossSpawnCount == m_bashirKillCounter)
            SpawnBoss(m_bashirPhase);
        else
            SpawnRandomBashirMob();
    }

    void SpawnRandomBashirMob()
    {
        uint32 randomIdx = urand(0, m_bashirPhaseSpawnData.size() - 1);
        BashirCombatSpawn* data = m_bashirPhaseSpawnData[randomIdx];
        m_bashirPhaseSpawnDataUsed.push_back(data);
        m_bashirPhaseSpawnData.erase(m_bashirPhaseSpawnData.begin() + randomIdx);
        Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH); // Has to be alive during this so we can use him for summoning
        uint32 entry;
        switch (data->phaseId)
        {
            case BASHIR_PHASE_1: entry = NPC_SLAVERING_SLAVE; break;
            case BASHIR_PHASE_2: entry = NPC_BASHIR_SUBPRIMAL; break;
            case BASHIR_PHASE_3: entry = urand(0, 1) ? NPC_BASHIR_CONTROLLER : NPC_BASHIR_RECKONER; break;
        }
        Creature* mob = tech->SummonCreature(entry, data->x, data->y, data->z, data->ori, TEMPSPAWN_DEAD_DESPAWN, 0, true);
        switch (entry)
        {
            case NPC_SLAVERING_SLAVE:
            case NPC_BASHIR_SUBPRIMAL:
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                break;
            default:
                mob->CastSpell(nullptr, SPELL_ETHEREAL_TELEPORT, TRIGGERED_NONE);
                break;
        }
    }

    void SpawnTowerDefenders()
    {
        Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH); // Has to be alive during this so we can use him for summoning
        for (BashirCombatSpawn& data : bashirCombatDisruptorSpawnLocations)
        {
            Creature* mob = tech->SummonCreature(NPC_BASHIR_SUBPRIMAL, data.x, data.y, data.z, data.ori, TEMPSPAWN_DEAD_DESPAWN, 0, true);
            mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
        }
    }

    void OnObjectCreate(GameObject* go) override
    {
        switch (go->GetEntry())
        {
            case GO_ROCKET_FIRE:
            case GO_ROCKET_SMOKE:
                m_goEntryGuidCollection[go->GetEntry()].push_back(go->GetObjectGuid());
                std::sort(m_goEntryGuidCollection[go->GetEntry()].begin(), m_goEntryGuidCollection[go->GetEntry()].end());
                break;
            case GO_BASHIR_CRYSTALFORGE:
                m_goEntryGuidStore.emplace(go->GetEntry(), go->GetObjectGuid());
                break;
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_DEATHS_DOOR_NORTH:
                m_deathsDoorNorthHP = std::max(0, 100 - int32(data * 15));
                sWorldState.ExecuteOnAreaPlayers(AREAID_DEATHS_DOOR, [=](Player* player)->void {player->SendUpdateWorldState(WORLD_STATE_DEATHS_DOOR_NORTH_WARP_GATE_HEALTH, m_deathsDoorNorthHP); });
                break;
            case TYPE_DEATHS_DOOR_SOUTH:
                m_deathsDoorSouthHP = std::max(0, 100 - int32(data * 15));
                sWorldState.ExecuteOnAreaPlayers(AREAID_DEATHS_DOOR, [=](Player* player)->void {player->SendUpdateWorldState(WORLD_STATE_DEATHS_DOOR_SOUTH_WARP_GATE_HEALTH, m_deathsDoorSouthHP); });
                break;
            case TYPE_SHARTUUL:
                // TODO: add calculation
                break;
            case TYPE_BASHIR:
                switch (data)
                {
                    case 0:
                        DespawnCrystalforgeGuardians();
                        break;
                    case 1:
                        StartPhase(BASHIR_PHASE_1);
                        break;
                    case 2:
                        StartPhase(BashirPhases(uint32(m_bashirPhase) + 1));
                        break;
                    case 3:
                        m_bashirPhase = BASHIR_PHASE_NOT_ACTIVE;
                        break;
                }
                break;
        }
    }

    bool CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const override
    {
        switch (instanceConditionId)
        {
            case INSTANCE_CONDITION_ID_SOCRETHAR_GOSSIP:
            {
                Creature const* socrethar = GetSingleCreatureFromStorage(NPC_SOCRETHAR);
                if (!socrethar || !socrethar->IsAlive() || socrethar->IsInCombat())
                    return true;
                return false;
            }
            case INSTANCE_CONDITION_ID_BASHIR_FLYING:
                return m_bashirPhase == BASHIR_PHASE_START;
            case INSTANCE_CONDITION_ID_BASHIR_IN_PROGRESS:
                return m_bashirPhase > BASHIR_PHASE_START;
        }

        script_error_log("world_map_outland::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
            instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
        return false;
    }

    uint32 CalculateBashirTimerValue()
    {
        return 60 - m_bashirTime.tm_min + 60 * (m_bashirTime.tm_hour % 2);
    }

    void FillInitialWorldStates(ByteBuffer& data, uint32& count, uint32 /*zoneId*/, uint32 areaId) override
    {
        switch (areaId)
        {
            case AREAID_DEATHS_DOOR:
            {
                FillInitialWorldStateData(data, count, WORLD_STATE_DEATHS_DOOR_NORTH_WARP_GATE_HEALTH, m_deathsDoorNorthHP);
                FillInitialWorldStateData(data, count, WORLD_STATE_DEATHS_DOOR_SOUTH_WARP_GATE_HEALTH, m_deathsDoorSouthHP);
                FillInitialWorldStateData(data, count, WORLD_STATE_DEATHS_DOOR_EVENT_ACTIVE, m_deathsDoorEventActive);
                break;
            }
            case AREAID_SHARTUUL_TRANSPORTER:
            {
                FillInitialWorldStateData(data, count, WORLD_STATE_SHARTUUL_SHIELD_REMAINING, m_shartuulShieldPercent);
                FillInitialWorldStateData(data, count, WORLD_STATE_SHARTUUL_EVENT_ACTIVE, m_shartuulEventActive);
                break;
            }
            case AREAID_SKYGUARD_OUTPOST:
            {
                FillInitialWorldStateData(data, count, WORLD_STATE_BASHIR_TIMER_WOTLK, CalculateBashirTimerValue());
                break;
            }
            default: break;
        }
    }

    void StartBashirLandingEvent()
    {
        if (!m_apprentice) // his grid wasnt loaded yet and autoload isnt on for performance reasons
            return;
        m_bashirIntroStage = 0;
        m_apprentice->SummonCreature(NPC_SKYGUARD_AETHER_TECH,  bashirSpawnPositions[0][0], bashirSpawnPositions[0][1], bashirSpawnPositions[0][2], bashirSpawnPositions[0][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        Creature* ranger = m_apprentice->SummonCreature(NPC_SKYGUARD_RANGER,      bashirSpawnPositions[1][0], bashirSpawnPositions[1][1], bashirSpawnPositions[1][2], bashirSpawnPositions[1][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 2);
        ranger = m_apprentice->SummonCreature(NPC_SKYGUARD_RANGER,      bashirSpawnPositions[2][0], bashirSpawnPositions[2][1], bashirSpawnPositions[2][2], bashirSpawnPositions[2][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 3);
        m_apprentice->SummonCreature(NPC_SKYGUARD_LIEUTENANT, bashirSpawnPositions[3][0], bashirSpawnPositions[3][1], bashirSpawnPositions[3][2], bashirSpawnPositions[3][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger = m_apprentice->SummonCreature(NPC_SKYGUARD_RANGER,      bashirSpawnPositions[4][0], bashirSpawnPositions[4][1], bashirSpawnPositions[4][2], bashirSpawnPositions[4][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 4);
        m_bashirPhase = BASHIR_PHASE_START;
        m_deadTowerCounter = 0;
        ResetTimer(BASHIR_ACTION_INTRO, GetIntroTimer(m_bashirIntroStage));
    }

    void SpawnAlliesAtCrystalforge(BashirPhases phaseId, Player* player)
    {
        player->SummonCreature(NPC_SKYGUARD_LIEUTENANT, bashirCustomSpawnPositions[0][0], bashirCustomSpawnPositions[0][1], bashirCustomSpawnPositions[0][2], bashirCustomSpawnPositions[0][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        Creature* ranger = player->SummonCreature(NPC_SKYGUARD_RANGER, bashirCustomSpawnPositions[1][0], bashirCustomSpawnPositions[1][1], bashirCustomSpawnPositions[1][2], bashirCustomSpawnPositions[1][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 2);
        ranger = player->SummonCreature(NPC_SKYGUARD_RANGER, bashirCustomSpawnPositions[2][0], bashirCustomSpawnPositions[2][1], bashirCustomSpawnPositions[2][2], bashirCustomSpawnPositions[2][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 3);
        Creature* tech = player->SummonCreature(NPC_SKYGUARD_AETHER_TECH, bashirCustomSpawnPositions[3][0], bashirCustomSpawnPositions[3][1], bashirCustomSpawnPositions[3][2], bashirCustomSpawnPositions[3][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        tech->HandleEmote(EMOTE_STATE_USESTANDING_NOSHEATHE);
        ranger = player->SummonCreature(NPC_SKYGUARD_RANGER, bashirCustomSpawnPositions[4][0], bashirCustomSpawnPositions[4][1], bashirCustomSpawnPositions[4][2], bashirCustomSpawnPositions[4][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
        ranger->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, ranger, ranger, 4);
        Creature* vendor;
        switch (phaseId)
        {
            case BASHIR_PHASE_ALL_VENDORS_SPAWNED:
                vendor = player->SummonCreature(NPC_AETHER_TECH_MASTER, bashirCustomSpawnPositions[7][0], bashirCustomSpawnPositions[7][1], bashirCustomSpawnPositions[7][2], bashirCustomSpawnPositions[7][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
                vendor->HandleEmote(EMOTE_STATE_USESTANDING);
                vendor->SetCanFly(false);
                // no break
            case BASHIR_PHASE_TRANSITION_3:
            case BASHIR_PHASE_3:
                vendor = player->SummonCreature(NPC_AETHER_TECH_ADEPT, bashirCustomSpawnPositions[6][0], bashirCustomSpawnPositions[6][1], bashirCustomSpawnPositions[6][2], bashirCustomSpawnPositions[6][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
                vendor->HandleEmote(EMOTE_STATE_USESTANDING);
                vendor->SetCanFly(false);
                // no break
            case BASHIR_PHASE_TRANSITION_2:
            case BASHIR_PHASE_2:
                vendor = player->SummonCreature(NPC_AETHER_TECH_ASSISTANT, bashirCustomSpawnPositions[5][0], bashirCustomSpawnPositions[5][1], bashirCustomSpawnPositions[5][2], bashirCustomSpawnPositions[5][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true);
                vendor->HandleEmote(EMOTE_STATE_USESTANDING);
                vendor->SetCanFly(false);
                break;
        }
    }

    void StartBashirAtCustomSpot(BashirPhases phaseId, Player* player)
    {
        SpawnAlliesAtCrystalforge(phaseId, player);
        DespawnCrystalforgeGuardians();
        StartPhase(phaseId);
    }

    void DespawnCrystalforgeGuardians()
    {
        if (GameObject* go = GetSingleGameObjectFromStorage(GO_BASHIR_CRYSTALFORGE))
        {
            CreatureList defenders;
            GetCreatureListWithEntryInGrid(defenders, go, NPC_BASHIR_RAIDER, 60.f);
            GetCreatureListWithEntryInGrid(defenders, go, NPC_BASHIR_SPELL_THIEF, 60.f);
            for (Creature* defender : defenders)
                defender->ForcedDespawn();
        }
    }

    void DespawnBashir(bool all)
    {
        for (ObjectGuid guid : m_bashirSpawns)
            if (all || guid.GetEntry() != NPC_SKYGUARD_AETHER_TECH)
                if (Creature* spawn = instance->GetCreature(guid))
                    spawn->ForcedDespawn();

        for (ObjectGuid guid : m_bashirEnemySpawns)
            if (Creature* spawn = instance->GetCreature(guid))
                spawn->ForcedDespawn();

        if (all)
            m_bashirPhase = BASHIR_PHASE_NOT_ACTIVE;
        else
            m_bashirPhase = BASHIR_DESPAWN;
    }

    void StartBattlePhase()
    {
        ResetTimer(BASHIR_ACTION_SPAWN_ENEMY, GetActionTimer(BASHIR_ACTION_SPAWN_ENEMY));
        ResetTimer(BASHIR_ACTION_ENEMY_ATTACK, 10000);
        m_bashirEnemySpawns.clear();
        m_bashirEnemyWaveSpawns.clear();
        m_bashirPhaseSpawnData.clear();
        for (BashirCombatSpawn& data : bashirCombatSpawnLocations)
            if (data.phaseId == m_bashirPhase || data.phaseId == m_bashirPhase - 2)
                m_bashirPhaseSpawnData.push_back(&data);
        uint32 initialSpawnCount;
        switch (m_bashirPhase)
        {
            case BASHIR_PHASE_1:
            {
                initialSpawnCount = PHASE_1_INITIAL_SPAWN_COUNT;
                break;
            }
            case BASHIR_PHASE_2:
            {
                initialSpawnCount = PHASE_2_INITIAL_SPAWN_COUNT;
                break;
            }
            case BASHIR_PHASE_3:
            {
                initialSpawnCount = PHASE_3_INITIAL_SPAWN_COUNT;
                break;
            }
        }
        for (uint32 i = 0; i < initialSpawnCount; ++i)
            SpawnRandomBashirMob();
    }

    void StartTransitionPhase()
    {
        Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
        Creature* vendor;
        switch (m_bashirPhase)
        {
            case BASHIR_PHASE_TRANSITION_1:
                vendor = tech->SummonCreature(NPC_AETHER_TECH_ASSISTANT, bashirSpawnPositions[5][0], bashirSpawnPositions[5][1], bashirSpawnPositions[5][2], bashirSpawnPositions[5][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true, true, 1);
                break;
            case BASHIR_PHASE_TRANSITION_2:
                vendor = tech->SummonCreature(NPC_AETHER_TECH_ADEPT, bashirSpawnPositions[6][0], bashirSpawnPositions[6][1], bashirSpawnPositions[6][2], bashirSpawnPositions[6][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true, true, 1);
                break;
            case BASHIR_PHASE_TRANSITION_3:
                vendor = tech->SummonCreature(NPC_AETHER_TECH_MASTER, bashirSpawnPositions[7][0], bashirSpawnPositions[7][1], bashirSpawnPositions[7][2], bashirSpawnPositions[7][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 1000, true, true, 1);
                break;
        }
        vendor->Mount(MOUNT_NETHER_RAY_DISPLAY_ID);
        tech->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, tech, tech);
    }

    void StartPhase(BashirPhases phase)
    {
        m_bashirPhase = phase;
        switch (phase)
        {
            case BASHIR_PHASE_1:
            case BASHIR_PHASE_2:
            case BASHIR_PHASE_3:
                StartBattlePhase();
                break;
            case BASHIR_PHASE_TRANSITION_1:
            case BASHIR_PHASE_TRANSITION_2:
            case BASHIR_PHASE_TRANSITION_3:
                StartTransitionPhase();
                break;
            case BASHIR_PHASE_ALL_VENDORS_SPAWNED:
                ResetTimer(BASHIR_ACTION_OUTRO, (4 * 60 + 40) * IN_MILLISECONDS); // 4 minutes 40 seconds
                break;
        }
    }

    void SpawnBoss(BashirPhases phase)
    {
        Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH); // Has to be alive during this so we can use him for summoning
        Creature* mob;
        switch (phase)
        {
            case BASHIR_PHASE_1:
                mob = tech->SummonCreature(NPC_BASHIR_FLESH_FIEND, bashirSpawnPositions[8][0], bashirSpawnPositions[8][1], bashirSpawnPositions[8][2], bashirSpawnPositions[8][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 2 * 60 * IN_MILLISECONDS, true);
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                break;
            case BASHIR_PHASE_2:
                SpawnTowerDefenders();
                mob = tech->SummonCreature(NPC_DISRUPTOR_TOWER, bashirSpawnPositions[9][0],  bashirSpawnPositions[9][1],  bashirSpawnPositions[9][2],  bashirSpawnPositions[9][3],  TEMPSPAWN_CORPSE_TIMED_DESPAWN, 2 * 60 * IN_MILLISECONDS, true);
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                mob = tech->SummonCreature(NPC_DISRUPTOR_TOWER, bashirSpawnPositions[10][0], bashirSpawnPositions[10][1], bashirSpawnPositions[10][2], bashirSpawnPositions[10][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 2 * 60 * IN_MILLISECONDS, true);
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                mob = tech->SummonCreature(NPC_DISRUPTOR_TOWER, bashirSpawnPositions[11][0], bashirSpawnPositions[11][1], bashirSpawnPositions[11][2], bashirSpawnPositions[11][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 2 * 60 * IN_MILLISECONDS, true);
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                break;
            case BASHIR_PHASE_3:
                mob = tech->SummonCreature(NPC_GRAND_COLLECTOR, bashirSpawnPositions[12][0], bashirSpawnPositions[12][1], bashirSpawnPositions[12][2], bashirSpawnPositions[12][3], TEMPSPAWN_CORPSE_TIMED_DESPAWN, 2 * 60 * IN_MILLISECONDS, true);
                mob->CastSpell(nullptr, SPELL_SPIRIT_SPAWN_IN, TRIGGERED_NONE);
                break;
        }
    }

    void FinishPhase(BashirPhases phase)
    {
        DisableTimer(BASHIR_ACTION_SPAWN_ENEMY);
        DisableTimer(BASHIR_ACTION_ENEMY_ATTACK);
        Creature* tech = GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
        switch (phase)
        {
            case BASHIR_PHASE_1:
                DoScriptText(SAY_TECH_PHASE_1, tech);
                break;
            case BASHIR_PHASE_2:
                DoScriptText(SAY_TECH_PHASE_2, tech);
                break;
            case BASHIR_PHASE_3:
                DoScriptText(SAY_TECH_PHASE_3, tech);
                break;
        }
        for (ObjectGuid guid : m_bashirEnemySpawns)
            if (Creature* spawn = instance->GetCreature(guid))
                if (!spawn->IsInCombat())
                    spawn->ForcedDespawn();
        StartPhase(BashirPhases(uint32(phase) + 1));
    }

    void StartManaSlaveEvent()
    {
        if (GameObject* go = GetSingleGameObjectFromStorage(GO_BASHIR_CRYSTALFORGE))
            if (Creature* controller = GetClosestCreatureWithEntry(go, CRYSTALFORGE_SLAVE_EVENT_CONTROLLER_ENTRY, 10.f))
                instance->ScriptsStart(sRelayScripts, CRYSTALFORGE_SLAVE_EVENT_RELAY_ID, controller, controller);
    }

    void Update(const uint32 diff) override
    {
        UpdateTimers(diff);

        if (m_bashirTimer <= diff)
        {
            std::time_t now = time(nullptr);
            m_bashirTime = *std::gmtime(&now);
            m_bashirTimer = 60 * IN_MILLISECONDS;
            uint32 timerValue = CalculateBashirTimerValue();
            sWorldState.ExecuteOnAreaPlayers(AREAID_SKYGUARD_OUTPOST, [=](Player* player)->void
            {
                player->SendUpdateWorldState(WORLD_STATE_BASHIR_TIMER_WOTLK, timerValue);
            });
            if (m_bashirPhase == BASHIR_PHASE_NOT_ACTIVE)
            {
                if (timerValue == 0)
                    StartBashirLandingEvent();
                else
                {
                    timerValue %= 60;
                    if (timerValue == 15 || timerValue == 45)
                        StartManaSlaveEvent();
                }
            }
        }
        else m_bashirTimer -= diff;
    }

    void ShowChatCommands(ChatHandler* handler) override
    {
        handler->SendSysMessage("This instance supports the following commands:\n bashir (0,1,2,3,4,5,6,7) starts event at stage respectively - start event, start phase 1, finish phase 1,"
        "start phase 2, finish phase 2, start phase 3, finish phase 3, despawn event");
    }

    void ExecuteChatCommand(ChatHandler* handler, char* args) override
    {
        char* result = handler->ExtractLiteralArg(&args);
        if (!result)
            return;
        std::string val = result;
        if (val == "bashir")
        {
            uint32 startPhase;
            if (!handler->ExtractUInt32(&args, startPhase))
                return;

            switch (startPhase)
            {
                case BASHIR_PHASE_START: // Start event - equivalent to starting on its own
                    StartBashirLandingEvent();
                    break;
                case BASHIR_PHASE_1: // Start event at custom phases
                case BASHIR_PHASE_TRANSITION_1:
                case BASHIR_PHASE_2:
                case BASHIR_PHASE_TRANSITION_2:
                case BASHIR_PHASE_3:
                case BASHIR_PHASE_TRANSITION_3:
                case BASHIR_PHASE_ALL_VENDORS_SPAWNED:
                    StartBashirAtCustomSpot(BashirPhases(startPhase), handler->GetPlayer());
                    break;
                case BASHIR_DESPAWN:
                    DespawnBashir(true);
                    break;
                default: break;
            }
        }
    }
};

InstanceData* GetInstanceData_world_map_outland(Map* map)
{
    return new world_map_outland(map);
}

void AddSC_OutlandWorldScript()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "world_map_outland";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_outland;
    pNewScript->RegisterSelf();
}