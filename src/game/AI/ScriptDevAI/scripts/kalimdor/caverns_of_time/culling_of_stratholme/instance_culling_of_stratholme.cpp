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

/* ScriptData
SDName: Instance_culling_of_stratholme
SD%Complete: 80%
SDComment:
SDCategory: Culling of Stratholme
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "culling_of_stratholme.h"

enum
{
    MAX_ARTHAS_SPAWN_POS        = 5,
    MAX_GRAIN_CRATES            = 5,
    MAX_SCOURGE_SPAWN_POS       = 5,
    MAX_BURNING_SCOURGE_POS     = 15,

    SAY_SCOURGE_FESTIVAL_LANE   = -1595003,
    SAY_SCOURGE_KINGS_SQUARE    = -1595004,
    SAY_SCOURGE_MARKET_ROW      = -1595005,
    SAY_SCOURGE_TOWN_HALL       = -1595006,
    SAY_SCOURGE_ELDERS_SQUARE   = -1595007,

    SAY_MEET_TOWN_HALL          = -1595008,
    SAY_CORRUPTOR_DESPAWN       = -1595041,

    WHISPER_CHROMIE_CRATES      = -1595001,
    WHISPER_CHROMIE_GUARDIAN    = -1595002,
    WHISPER_CHROMIE_HURRY       = -1000000,                         // TODO

    SPELL_CORRUPTION_OF_TIME    = 60422,                    // triggers 60451
};

struct sSpawnLocation
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static sSpawnLocation m_aArthasSpawnLocs[] =                // need tuning
{
    {1957.13f, 1287.43f, 145.65f, 2.96f},                   // bridge
    {2091.99f, 1277.25f, 140.47f, 0.43f},                   // city entrance
    {2366.24f, 1195.25f, 132.04f, 3.15f},                   // town hall
    {2534.98f, 1126.16f, 130.86f, 2.84f},                   // burning stratholme
    {2363.44f, 1404.90f, 128.64f, 2.77f},                   // crusader square gate
};

static sSpawnLocation m_aIntroActorsSpawnLocs[] =
{
    {1876.78f, 1305.72f, 146.24f, 6.07f},                   // Jaina
    {1786.18f, 1268.63f, 140.02f, 0.42f},                   // Uther
    {1780.26f, 1261.87f, 139.55f, 0.57f},                   // Silverhand knights
    {1778.59f, 1265.03f, 139.43f, 0.40f},
    {1777.04f, 1268.16f, 139.35f, 0.59f},
    {2091.47f, 1294.28f, 139.82f, 6.27f},                   // High elf priests
    {2091.26f, 1281.71f, 139.92f, 6.27f},
    {2096.12f, 1290.53f, 138.81f, 6.27f},                   // Footman
    {2096.41f, 1284.22f, 138.79f, 6.27f},
    {2096.93f, 1297.57f, 138.96f, 6.27f},
    {2096.32f, 1278.98f, 139.43f, 6.27f}
};

static sSpawnLocation m_aChromieSpawnLocs[] =
{
    {1813.298f, 1283.578f, 142.3258f, 3.96f},               // near bridge
    {2319.562f, 1506.409f, 152.0499f, 3.78f},               // End
    {1810.875f, 1285.035f, 142.4917f, 4.48f},               // Hourglass, near bridge
};

static sSpawnLocation m_aHeroicEventSpawnLocs[] =
{
    {2331.642f, 1273.273f, 132.954f, 3.71f},                // Infinite corruptor
    {2334.626f, 1280.450f, 133.006f, 1.72f},                // Time rift
    {2321.489f, 1268.383f, 132.850f, 0.41f},                // Guardian of time
};

struct sScourgeSpawnLoc
{
    uint8 m_uiId;
    int32 m_iYellId;
    float m_fX, m_fY, m_fZ, m_fO;
};

static sScourgeSpawnLoc m_aScourgeWavesLocs[] =
{
    {POS_FESTIVAL_LANE, SAY_SCOURGE_FESTIVAL_LANE,  2176.517f, 1244.970f, 136.021f, 1.86f},
    {POS_KINGS_SQUARE,  SAY_SCOURGE_KINGS_SQUARE,   2130.760f, 1353.649f, 131.396f, 6.02f},
    {POS_MARKET_ROW,    SAY_SCOURGE_MARKET_ROW,     2219.825f, 1331.119f, 127.978f, 3.08f},
    {POS_TOWN_HALL,     SAY_SCOURGE_TOWN_HALL,      2351.475f, 1211.893f, 130.361f, 4.50f},
    {POS_ELDERS_SQUARE, SAY_SCOURGE_ELDERS_SQUARE,  2259.153f, 1153.199f, 138.431f, 2.39f},
};

struct sBurningScourgeSpawnLoc
{
    uint8 m_uiType;
    float m_fX, m_fY, m_fZ;
};

static sBurningScourgeSpawnLoc m_aBurningScourgeLocs[MAX_BURNING_SCOURGE_POS] =
{
    {SCOURGE_TYPE_GHOUL, 2571.570f, 1169.945f, 126.191f},
    {SCOURGE_TYPE_GOLEM, 2560.524f, 1208.296f, 125.613f},
    {SCOURGE_TYPE_GHOUL, 2562.075f, 1182.137f, 126.499f},
    {SCOURGE_TYPE_FIEND, 2552.720f, 1227.233f, 125.620f},
    {SCOURGE_TYPE_GHOUL, 2545.070f, 1245.650f, 125.991f},
    {SCOURGE_TYPE_GHOUL, 2534.250f, 1258.379f, 127.030f},
    {SCOURGE_TYPE_ACOLYTES, 2532.075f, 1271.579f, 127.243f},
    {SCOURGE_TYPE_GHOUL, 2529.144f, 1281.680f, 128.429f},
    {SCOURGE_TYPE_GHOUL, 2491.742f, 1365.169f, 130.827f},
    {SCOURGE_TYPE_FIEND, 2490.869f, 1366.189f, 130.678f},
    {SCOURGE_TYPE_GHOUL, 2479.944f, 1393.666f, 129.975f},
    {SCOURGE_TYPE_GOLEM, 2484.858f, 1380.665f, 130.075f},
    {SCOURGE_TYPE_GHOUL, 2474.965f, 1399.063f, 130.317f},
    {SCOURGE_TYPE_ACOLYTES, 2461.411f, 1416.090f, 130.663f},
    {SCOURGE_TYPE_GHOUL, 2448.391f, 1426.428f, 130.853f},
};

instance_culling_of_stratholme::instance_culling_of_stratholme(Map* pMap) : ScriptedInstance(pMap),
    m_bStartedInnEvent(false),
    m_uiGrainCrateCount(0),
    m_uiRemoveCrateStateTimer(0),
    m_uiArthasRespawnTimer(0),
    m_uiScourgeWaveTimer(0),
    m_uiScourgeWaveCount(0),
    m_uiCurrentUndeadPos(POS_FESTIVAL_LANE)             // always the first undead location is Festival Lane
{
    Initialize();
}

void instance_culling_of_stratholme::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_culling_of_stratholme::OnPlayerEnter(Player* pPlayer)
{
    // spawn Chromie
    DoSpawnChromieIfNeeded(pPlayer);

    // spawn Arthas if intro is completed / passed
    if (GetData(TYPE_ARTHAS_INTRO_EVENT) == DONE)
    {
        DoSpawnArthasIfNeeded(pPlayer);

        // resume scourge waves if required - this can only happen in case of server reload
        if (GetData(TYPE_SALRAMM_EVENT) != DONE)
        {
            // this will restart the wave event with a delayed timer
            if (GetData(TYPE_MEATHOOK_EVENT) == DONE)
            {
                if (GetData(TYPE_SALRAMM_EVENT) != IN_PROGRESS)
                {
                    SetData(TYPE_SALRAMM_EVENT, IN_PROGRESS);
                    m_uiScourgeWaveTimer = 30000;
                    m_uiScourgeWaveCount = 5;
                }
            }
            else
            {
                if (GetData(TYPE_MEATHOOK_EVENT) != IN_PROGRESS)
                {
                    SetData(TYPE_MEATHOOK_EVENT, IN_PROGRESS);
                    m_uiScourgeWaveTimer = 30000;
                }
            }
        }
    }

    // Show World States if needed
    // Grain event world states
    if (GetData(TYPE_GRAIN_EVENT) == IN_PROGRESS || GetData(TYPE_GRAIN_EVENT) == SPECIAL)
        pPlayer->SendUpdateWorldState(WORLD_STATE_CRATES, 1);      // Show Crates Counter
    else
        pPlayer->SendUpdateWorldState(WORLD_STATE_CRATES, 0);      // Remove Crates Counter

    // Scourge waves
    if (GetData(TYPE_MEATHOOK_EVENT) == IN_PROGRESS || GetData(TYPE_SALRAMM_EVENT) == IN_PROGRESS)
        pPlayer->SendUpdateWorldState(WORLD_STATE_WAVE, m_uiScourgeWaveCount);      // Add WaveCounter
    else
        pPlayer->SendUpdateWorldState(WORLD_STATE_WAVE, 0);                         // Remove WaveCounter

    // Infinite corruptor
    if (GetData(TYPE_INFINITE_CORRUPTER_TIME))
    {
        DoSpawnCorruptorIfNeeded(pPlayer);

        pPlayer->SendUpdateWorldState(WORLD_STATE_TIME, 1);        // Show Timer
        pPlayer->SendUpdateWorldState(WORLD_STATE_TIME_COUNTER, GetData(TYPE_INFINITE_CORRUPTER_TIME) / (MINUTE * IN_MILLISECONDS));
    }
    else
        pPlayer->SendUpdateWorldState(WORLD_STATE_TIME, 0);        // Remove Timer
}

void instance_culling_of_stratholme::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHROMIE_ENTRANCE:
        case NPC_CHROMIE_END:
        case NPC_ARTHAS:
        case NPC_MICHAEL_BELFAST:
        case NPC_HEARTHSINGER_FORRESTEN:
        case NPC_FRAS_SIABI:
        case NPC_FOOTMAN_JAMES:
        case NPC_MAL_CORRICKS:
        case NPC_GRYAN_STOUTMANTLE:
        case NPC_ROGER_OWENS:
        case NPC_SERGEANT_MORIGAN:
        case NPC_JENA_ANDERSON:
        case NPC_MALCOM_MOORE:
        case NPC_BARTLEBY_BATTSON:
        case NPC_PATRICIA_O_REILLY:
        case NPC_LORDAERON_CRIER:
        case NPC_INFINITE_CORRUPTER:
        case NPC_LORD_EPOCH:
        case NPC_MALGANIS:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;

        case NPC_GRAIN_CRATE_HELPER:
            m_luiCratesBunnyGUIDs.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_LORDAERON_FOOTMAN:
        case NPC_HIGH_ELF_MAGE_PRIEST:
            if (pCreature->GetPositionX() > 2000.0f)
                m_luiGateSoldiersGUIDs.push_back(pCreature->GetObjectGuid());
            break;

        case NPC_STRATHOLME_CITIZEN:
        case NPC_STRATHOLME_RESIDENT:
        case NPC_AGIATED_STRATHOLME_CITIZEN:
        case NPC_AGIATED_STRATHOLME_RESIDENT:
            if (m_auiEncounter[TYPE_ARTHAS_INTRO_EVENT] == DONE)
                pCreature->UpdateEntry(NPC_ZOMBIE);
            else
                m_luiResidentGUIDs.push_back(pCreature->GetObjectGuid());
            break;

        case NPC_ENRAGING_GHOUL:
        case NPC_ACOLYTE:
        case NPC_MASTER_NECROMANCER:
        case NPC_CRYPT_FIEND:
        case NPC_PATCHWORK_CONSTRUCT:
        case NPC_TOMB_STALKER:
        case NPC_DARK_NECROMANCER:
        case NPC_BILE_GOLEM:
        case NPC_DEVOURING_GHOUL:
            if (pCreature->IsTemporarySummon() && GetData(TYPE_SALRAMM_EVENT) != DONE)
                m_luiCurrentScourgeWaveGUIDs.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_culling_of_stratholme::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOOR_BOOKCASE:
            if (m_auiEncounter[TYPE_EPOCH_EVENT] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DARK_RUNED_CHEST:
        case GO_DARK_RUNED_CHEST_H:
        case GO_CITY_ENTRANCE_GATE:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_culling_of_stratholme::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_GRAIN_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == SPECIAL)
                DoUpdateWorldState(WORLD_STATE_CRATES, 1);
            else if (uiData == IN_PROGRESS)
            {
                // safety check
                if (m_uiGrainCrateCount >= MAX_GRAIN_CRATES)
                    return;

                ++m_uiGrainCrateCount;
                DoUpdateWorldState(WORLD_STATE_CRATES_COUNT, m_uiGrainCrateCount);

                if (m_uiGrainCrateCount == MAX_GRAIN_CRATES)
                {
                    m_uiRemoveCrateStateTimer = 20000;
                    SetData(TYPE_GRAIN_EVENT, DONE);
                }
            }
            break;
        case TYPE_ARTHAS_INTRO_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                m_uiScourgeWaveCount = 0;
                m_uiScourgeWaveTimer = 1000;
                DoUpdateZombieResidents();

                SetData(TYPE_MEATHOOK_EVENT, IN_PROGRESS);
            }
            break;
        case TYPE_MEATHOOK_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                m_uiScourgeWaveTimer = 20000;
                SetData(TYPE_SALRAMM_EVENT, IN_PROGRESS);
            }
            break;
        case TYPE_SALRAMM_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                m_uiScourgeWaveTimer = 5000;
            break;
        case TYPE_ARTHAS_TOWNHALL_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                // despawn arthas and spawn him in the next point
                if (Creature* pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS))
                    pArthas->ForcedDespawn();

                if (Player* pPlayer = GetPlayerInMap())
                    DoSpawnArthasIfNeeded(pPlayer);
            }
            break;
        case TYPE_EPOCH_EVENT:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_ARTHAS_ESCORT_EVENT:
            // use fail in order to respawn Arthas
            if (uiData == FAIL)
            {
                m_uiArthasRespawnTimer = 10000;

                // despawn the bosses if Arthas dies in order to avoid exploits
                if (Creature* pEpoch = GetSingleCreatureFromStorage(NPC_LORD_EPOCH, true))
                    pEpoch->ForcedDespawn();
                if (Creature* pMalganis = GetSingleCreatureFromStorage(NPC_MALGANIS, true))
                    pMalganis->ForcedDespawn();
            }
            else
                m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MALGANIS_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_CITY_ENTRANCE_GATE);
                DoToggleGameObjectFlags(instance->IsRegularDifficulty() ? GO_DARK_RUNED_CHEST : GO_DARK_RUNED_CHEST_H, GO_FLAG_NO_INTERACT, false);
                DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_DARK_RUNED_CHEST : GO_DARK_RUNED_CHEST_H, 30 * MINUTE);

                if (Player* pPlayer = GetPlayerInMap())
                    DoSpawnChromieIfNeeded(pPlayer);
            }
            break;
        case TYPE_INFINITE_CORRUPTER_TIME:
            m_auiEncounter[uiType] = uiData;
            if (!uiData)
            {
                DoUpdateWorldState(WORLD_STATE_TIME, 0);    // Remove Timer
                DoUpdateWorldState(WORLD_STATE_TIME_COUNTER, 0);
            }
            else
                DoUpdateWorldState(WORLD_STATE_TIME_COUNTER, uiData / (MINUTE * IN_MILLISECONDS));
            break;
        case TYPE_INFINITE_CORRUPTER:
            m_auiEncounter[uiType] = uiData;
            switch (uiData)
            {
                case IN_PROGRESS:
                    if (!GetData(TYPE_INFINITE_CORRUPTER_TIME))
                    {
                        SetData(TYPE_INFINITE_CORRUPTER_TIME, MINUTE * 25 * IN_MILLISECONDS);
                        DoUpdateWorldState(WORLD_STATE_TIME, 1);
                        DoChromieWhisper(WHISPER_CHROMIE_GUARDIAN);

                        // spawn the corruptor for the first time
                        if (Creature* pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS))
                            DoSpawnCorruptorIfNeeded(pArthas);
                    }
                    break;
                case DONE:
                    // event completed - epilog handled by dbscript
                    SetData(TYPE_INFINITE_CORRUPTER_TIME, 0);
                    break;
                case SPECIAL:
                    DoChromieWhisper(WHISPER_CHROMIE_HURRY);
                    break;
                case FAIL:
                    // event failed - despawn the corruptor
                    SetData(TYPE_INFINITE_CORRUPTER_TIME, 0);
                    if (Creature* pCorrupter = GetSingleCreatureFromStorage(NPC_INFINITE_CORRUPTER))
                    {
                        DoOrSimulateScriptTextForThisInstance(SAY_CORRUPTOR_DESPAWN, NPC_INFINITE_CORRUPTER);

                        if (pCorrupter->IsAlive())
                            pCorrupter->ForcedDespawn();
                    }
                    break;
            }
            break;
    }

    if (uiData == DONE || uiType == TYPE_INFINITE_CORRUPTER_TIME)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_culling_of_stratholme::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_culling_of_stratholme::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MEATHOOK:                  SetData(TYPE_MEATHOOK_EVENT, DONE);     break;
        case NPC_SALRAMM_THE_FLESHCRAFTER:  SetData(TYPE_SALRAMM_EVENT, DONE);      break;
        case NPC_LORD_EPOCH:                SetData(TYPE_EPOCH_EVENT, DONE);        break;
        case NPC_INFINITE_CORRUPTER:        SetData(TYPE_INFINITE_CORRUPTER, DONE); break;

        case NPC_ENRAGING_GHOUL:
        case NPC_ACOLYTE:
        case NPC_MASTER_NECROMANCER:
        case NPC_CRYPT_FIEND:
        case NPC_PATCHWORK_CONSTRUCT:
        case NPC_TOMB_STALKER:
        case NPC_DARK_NECROMANCER:
        case NPC_BILE_GOLEM:
        case NPC_DEVOURING_GHOUL:
            if (pCreature->IsTemporarySummon() && GetData(TYPE_SALRAMM_EVENT) != DONE)
            {
                m_luiCurrentScourgeWaveGUIDs.remove(pCreature->GetObjectGuid());

                // send next scourge wave
                if (m_luiCurrentScourgeWaveGUIDs.empty())
                    m_uiScourgeWaveTimer = 2000;
            }
            break;
    }
}

void instance_culling_of_stratholme::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (i != TYPE_INFINITE_CORRUPTER_TIME)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }
    }

    // If already started counting down time, the event is "in progress"
    if (m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME])
        m_auiEncounter[TYPE_INFINITE_CORRUPTER] = IN_PROGRESS;

    OUT_LOAD_INST_DATA_COMPLETE;
}

// Function that will make Chromie to send a whisper to all players in map
void instance_culling_of_stratholme::DoChromieWhisper(int32 iEntry)
{
    if (Creature* pChromie = GetSingleCreatureFromStorage(NPC_CHROMIE_ENTRANCE))
    {
        Map::PlayerList const& players = instance->GetPlayers();
        if (!players.isEmpty())
        {
            for (const auto& player : players)
            {
                if (Player* pPlayer = player.getSource())
                    DoScriptText(iEntry, pChromie, pPlayer);
            }
        }
    }
}

// Function that returns the current position for Arthas event
uint8 instance_culling_of_stratholme::GetInstancePosition()
{
    if (m_auiEncounter[TYPE_MALGANIS_EVENT] == DONE)
        return POS_INSTANCE_FINISHED;
    if (m_auiEncounter[TYPE_ARTHAS_ESCORT_EVENT] == DONE)
        return POS_ARTHAS_MALGANIS;
    if (m_auiEncounter[TYPE_EPOCH_EVENT] == DONE)
        return POS_ARTHAS_ESCORTING;
    if (m_auiEncounter[TYPE_SALRAMM_EVENT] == DONE)
        return POS_ARTHAS_TOWNHALL;
    if (m_auiEncounter[TYPE_MEATHOOK_EVENT] == DONE)
        return POS_ARTHAS_WAVES;
    if (m_auiEncounter[TYPE_ARTHAS_INTRO_EVENT] == DONE)
        return POS_ARTHAS_WAVES;
    if (m_auiEncounter[TYPE_GRAIN_EVENT] == DONE)
        return POS_ARTHAS_INTRO;
    return 0;
}

// Sorting function
static bool sortFromEastToWest(Creature* pFirst, Creature* pSecond)
{
    return pFirst && pSecond && pFirst->GetPositionY() < pSecond->GetPositionY();
}

// return the ordered list of Grain Crate Helpers
void instance_culling_of_stratholme::GetCratesBunnyOrderedList(CreatureList& lList)
{
    CreatureList lCratesBunnyList;
    for (GuidList::const_iterator itr = m_luiCratesBunnyGUIDs.begin(); itr != m_luiCratesBunnyGUIDs.end(); ++itr)
    {
        if (Creature* pBunny = instance->GetCreature(*itr))
            lCratesBunnyList.push_back(pBunny);
    }
    if (lCratesBunnyList.empty())
        return;

    lCratesBunnyList.sort(sortFromEastToWest);
    lList = lCratesBunnyList;
}

// Function that spawns Arthas on demand
void instance_culling_of_stratholme::DoSpawnArthasIfNeeded(Unit* pSummoner)
{
    if (!pSummoner)
        return;

    Creature* pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS, true);
    if (pArthas && pArthas->IsAlive())
        return;

    uint8 uiPosition = GetInstancePosition();
    if (uiPosition && uiPosition <= MAX_ARTHAS_SPAWN_POS)
        pSummoner->SummonCreature(NPC_ARTHAS, m_aArthasSpawnLocs[uiPosition - 1].m_fX, m_aArthasSpawnLocs[uiPosition - 1].m_fY, m_aArthasSpawnLocs[uiPosition - 1].m_fZ, m_aArthasSpawnLocs[uiPosition - 1].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000, true);

    // no gossip flag in the following positions
    if (uiPosition == POS_ARTHAS_INTRO || uiPosition == POS_ARTHAS_WAVES)
    {
        pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS);
        if (pArthas)
        {
            pArthas->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pArthas->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        }
    }

    // summon the other intro actors
    if (uiPosition == POS_ARTHAS_INTRO)
    {
        // start intro event by dbscripts
        pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS);
        if (pArthas)
        {
            pArthas->SetWalk(false);
            pArthas->GetMotionMaster()->MoveWaypoint();
        }
        // spawn Jaina and Uther
        if (Creature* pJaina = pSummoner->SummonCreature(NPC_JAINA_PROUDMOORE, m_aIntroActorsSpawnLocs[0].m_fX, m_aIntroActorsSpawnLocs[0].m_fY, m_aIntroActorsSpawnLocs[0].m_fZ, m_aIntroActorsSpawnLocs[0].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000))
            pJaina->GetMotionMaster()->MoveWaypoint();
        if (Creature* pUther = pSummoner->SummonCreature(NPC_UTHER_LIGHTBRINGER, m_aIntroActorsSpawnLocs[1].m_fX, m_aIntroActorsSpawnLocs[1].m_fY, m_aIntroActorsSpawnLocs[1].m_fZ, m_aIntroActorsSpawnLocs[1].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000))
        {
            pUther->SetWalk(false);
            pUther->GetMotionMaster()->MoveWaypoint();

            // spawn the knights
            if (Creature* pKnight = pSummoner->SummonCreature(NPC_KNIGHT_SILVERHAND, m_aIntroActorsSpawnLocs[2].m_fX, m_aIntroActorsSpawnLocs[2].m_fY, m_aIntroActorsSpawnLocs[2].m_fZ, m_aIntroActorsSpawnLocs[2].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000))
                pKnight->GetMotionMaster()->MoveFollow(pUther, pKnight->GetDistance(pUther), 2 * M_PI_F - pKnight->GetAngle(pUther));
            if (Creature* pKnight = pSummoner->SummonCreature(NPC_KNIGHT_SILVERHAND, m_aIntroActorsSpawnLocs[3].m_fX, m_aIntroActorsSpawnLocs[3].m_fY, m_aIntroActorsSpawnLocs[3].m_fZ, m_aIntroActorsSpawnLocs[3].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000))
                pKnight->GetMotionMaster()->MoveFollow(pUther, pKnight->GetDistance(pUther), 2 * M_PI_F - pKnight->GetAngle(pUther));
            if (Creature* pKnight = pSummoner->SummonCreature(NPC_KNIGHT_SILVERHAND, m_aIntroActorsSpawnLocs[4].m_fX, m_aIntroActorsSpawnLocs[4].m_fY, m_aIntroActorsSpawnLocs[4].m_fZ, m_aIntroActorsSpawnLocs[4].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000))
                pKnight->GetMotionMaster()->MoveFollow(pUther, pKnight->GetDistance(pUther), 2 * M_PI_F - pKnight->GetAngle(pUther));
        }
    }
    // setup the entrance soldiers in case of reload or intro skip
    else if (uiPosition == POS_ARTHAS_WAVES)
        DoSetupEntranceSoldiers(pSummoner);
}

// Atm here only new Chromies are spawned
void instance_culling_of_stratholme::DoSpawnChromieIfNeeded(Unit* pSummoner)
{
    if (!pSummoner)
        return;

    if (GetInstancePosition() == POS_INSTANCE_FINISHED)
    {
        Creature* pChromie = GetSingleCreatureFromStorage(NPC_CHROMIE_END, true);
        if (!pChromie)
            pSummoner->SummonCreature(NPC_CHROMIE_END, m_aChromieSpawnLocs[1].m_fX, m_aChromieSpawnLocs[1].m_fY, m_aChromieSpawnLocs[1].m_fZ, m_aChromieSpawnLocs[1].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000);
    }
    else if (GetInstancePosition() >= POS_ARTHAS_INTRO)
    {
        Creature* pChromie = GetSingleCreatureFromStorage(NPC_CHROMIE_ENTRANCE, true);
        if (!pChromie)
        {
            pSummoner->SummonCreature(NPC_CHROMIE_ENTRANCE, m_aChromieSpawnLocs[0].m_fX, m_aChromieSpawnLocs[0].m_fY, m_aChromieSpawnLocs[0].m_fZ, m_aChromieSpawnLocs[0].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000);
            pSummoner->SummonCreature(NPC_HOURGLASS, m_aChromieSpawnLocs[2].m_fX, m_aChromieSpawnLocs[2].m_fY, m_aChromieSpawnLocs[2].m_fZ, m_aChromieSpawnLocs[2].m_fO, TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10000);
        }
    }
}

// Function that sets up the city entrance soldiers in case of reload or if the intro is skipped
void instance_culling_of_stratholme::DoSetupEntranceSoldiers(Unit* pSummoner)
{
    if (!pSummoner)
        return;

    // despawn the current set of soldiers
    for (GuidList::const_iterator itr = m_luiGateSoldiersGUIDs.begin(); itr != m_luiGateSoldiersGUIDs.end(); ++itr)
    {
        if (Creature* pSoldier = instance->GetCreature(*itr))
            pSoldier->ForcedDespawn();
    }

    // spawn others in the right spot
    pSummoner->SummonCreature(NPC_HIGH_ELF_MAGE_PRIEST, m_aIntroActorsSpawnLocs[5].m_fX, m_aIntroActorsSpawnLocs[5].m_fY, m_aIntroActorsSpawnLocs[5].m_fZ, m_aIntroActorsSpawnLocs[5].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
    pSummoner->SummonCreature(NPC_HIGH_ELF_MAGE_PRIEST, m_aIntroActorsSpawnLocs[6].m_fX, m_aIntroActorsSpawnLocs[6].m_fY, m_aIntroActorsSpawnLocs[6].m_fZ, m_aIntroActorsSpawnLocs[6].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
    pSummoner->SummonCreature(NPC_LORDAERON_FOOTMAN, m_aIntroActorsSpawnLocs[7].m_fX, m_aIntroActorsSpawnLocs[7].m_fY, m_aIntroActorsSpawnLocs[7].m_fZ, m_aIntroActorsSpawnLocs[7].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
    pSummoner->SummonCreature(NPC_LORDAERON_FOOTMAN, m_aIntroActorsSpawnLocs[8].m_fX, m_aIntroActorsSpawnLocs[8].m_fY, m_aIntroActorsSpawnLocs[8].m_fZ, m_aIntroActorsSpawnLocs[8].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
    pSummoner->SummonCreature(NPC_LORDAERON_FOOTMAN, m_aIntroActorsSpawnLocs[9].m_fX, m_aIntroActorsSpawnLocs[9].m_fY, m_aIntroActorsSpawnLocs[9].m_fZ, m_aIntroActorsSpawnLocs[9].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
    pSummoner->SummonCreature(NPC_LORDAERON_FOOTMAN, m_aIntroActorsSpawnLocs[10].m_fX, m_aIntroActorsSpawnLocs[10].m_fY, m_aIntroActorsSpawnLocs[10].m_fZ, m_aIntroActorsSpawnLocs[10].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 10000);
}

// Function that will spawn the infinite corruptor if requires
void instance_culling_of_stratholme::DoSpawnCorruptorIfNeeded(Unit* pSummoner)
{
    if (!pSummoner)
        return;

    if (GetSingleCreatureFromStorage(NPC_INFINITE_CORRUPTER, true))
        return;

    pSummoner->SummonCreature(NPC_TIME_RIFT, m_aHeroicEventSpawnLocs[1].m_fX, m_aHeroicEventSpawnLocs[1].m_fY, m_aHeroicEventSpawnLocs[1].m_fZ, m_aHeroicEventSpawnLocs[1].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
    pSummoner->SummonCreature(NPC_GUARDIAN_OF_TIME, m_aHeroicEventSpawnLocs[2].m_fX, m_aHeroicEventSpawnLocs[2].m_fY, m_aHeroicEventSpawnLocs[2].m_fZ, m_aHeroicEventSpawnLocs[2].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);

    if (Creature* pCorruptor = pSummoner->SummonCreature(NPC_INFINITE_CORRUPTER, m_aHeroicEventSpawnLocs[0].m_fX, m_aHeroicEventSpawnLocs[0].m_fY, m_aHeroicEventSpawnLocs[0].m_fZ, m_aHeroicEventSpawnLocs[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0))
        pCorruptor->CastSpell(pCorruptor, SPELL_CORRUPTION_OF_TIME, TRIGGERED_NONE);
}

// Function that updates all the stratholme humans to zombies
void instance_culling_of_stratholme::DoUpdateZombieResidents()
{
    // update all residents
    for (GuidList::const_iterator itr = m_luiResidentGUIDs.begin(); itr != m_luiResidentGUIDs.end(); ++itr)
    {
        if (Creature* pResident = instance->GetCreature(*itr))
            pResident->UpdateEntry(NPC_ZOMBIE);
    }
}

// Function to check if the grain event can progress
bool instance_culling_of_stratholme::CanGrainEventProgress(Creature* pCrate)
{
    if (!pCrate)
        return false;

    if (m_sGrainCratesGuidSet.find(pCrate->GetObjectGuid()) != m_sGrainCratesGuidSet.end())
        return false;

    if (GetData(TYPE_GRAIN_EVENT) != DONE)
        SetData(TYPE_GRAIN_EVENT, IN_PROGRESS);

    m_sGrainCratesGuidSet.insert(pCrate->GetObjectGuid());
    return true;
}

// Function that handles instance area trigger scripts
void instance_culling_of_stratholme::DoEventAtTriggerIfCan(uint32 uiTriggerId)
{
    switch (uiTriggerId)
    {
        case AREATRIGGER_INN:
            if (m_bStartedInnEvent)
                return;

            // start dialogue
            if (Creature* pMichael = GetSingleCreatureFromStorage(NPC_MICHAEL_BELFAST))
            {
                pMichael->SetStandState(UNIT_STAND_STATE_STAND);
                pMichael->GetMotionMaster()->MoveWaypoint();
            }
            m_bStartedInnEvent = true;
            break;
    }
}

// Function that spawns next scourge wave
void instance_culling_of_stratholme::DoSpawnNextScourgeWave()
{
    Creature* pSummoner = GetSingleCreatureFromStorage(NPC_ARTHAS);
    if (!pSummoner)
        return;

    DoOrSimulateScriptTextForThisInstance(m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_iYellId, NPC_LORDAERON_CRIER);

    for (uint8 i = 0; i < MAX_SCOURGE_TYPE_PER_WAVE; ++i)
    {
        // get the mob entry
        uint32 uiEntry = GetRandomMobOfType(uiScourgeWaveDef[m_uiScourgeWaveCount - 1][i]);
        if (!uiEntry)
            continue;

        float fX, fY, fZ;
        float fO = m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fO;

        // bosses get exact location
        if (uiScourgeWaveDef[m_uiScourgeWaveCount - 1][i] == SCOURGE_TYPE_BOSS || uiScourgeWaveDef[m_uiScourgeWaveCount - 1][i] == SCOURGE_TYPE_ACOLYTES)
        {
            fX = m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fX;
            fY = m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fY;
            fZ = m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fZ;
        }
        // random position around point
        else
            pSummoner->GetRandomPoint(m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fX, m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fY, m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fZ, 5.0f, fX, fY, fZ);

        // special requirement for acolytes - spawn a pack of 4
        if (uiScourgeWaveDef[m_uiScourgeWaveCount - 1][i] == SCOURGE_TYPE_ACOLYTES)
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                pSummoner->GetRandomPoint(m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fX, m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fY, m_aScourgeWavesLocs[m_uiCurrentUndeadPos].m_fZ, 5.0f, fX, fY, fZ);
                pSummoner->SummonCreature(uiEntry, fX, fY, fZ, fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            }
        }
        // spawn the selected mob
        else
            pSummoner->SummonCreature(uiEntry, fX, fY, fZ, fO, TEMPSPAWN_DEAD_DESPAWN, 0);
    }

    // start infinite curruptor event on the first wave
    if (m_uiScourgeWaveCount == 1 && !instance->IsRegularDifficulty() && GetData(TYPE_INFINITE_CORRUPTER) != DONE)
        SetData(TYPE_INFINITE_CORRUPTER, IN_PROGRESS);

    // get a random position that is different from the previous one for the next round
    uint8 uiCurrentPos = urand(POS_FESTIVAL_LANE, POS_ELDERS_SQUARE);

    while (uiCurrentPos == m_uiCurrentUndeadPos)
        uiCurrentPos = urand(POS_FESTIVAL_LANE, POS_ELDERS_SQUARE);

    m_uiCurrentUndeadPos = uiCurrentPos;
}

// function that spawns all the scourge elites in burning stratholme
void instance_culling_of_stratholme::DoSpawnBurningCityUndead(Unit* pSummoner)
{
    for (auto& m_aBurningScourgeLoc : m_aBurningScourgeLocs)
    {
        uint32 uiEntry = GetRandomMobOfType(m_aBurningScourgeLoc.m_uiType);
        if (!uiEntry)
            continue;

        float fX, fY, fZ;

        // special requirement for acolytes - spawn a pack of 3
        if (m_aBurningScourgeLoc.m_uiType == SCOURGE_TYPE_ACOLYTES)
        {
            for (uint8 j = 0; j < 3; ++j)
            {
                pSummoner->GetRandomPoint(m_aBurningScourgeLoc.m_fX, m_aBurningScourgeLoc.m_fY, m_aBurningScourgeLoc.m_fZ, 5.0f, fX, fY, fZ);

                if (Creature* pUndead = pSummoner->SummonCreature(uiEntry, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                    pUndead->GetMotionMaster()->MoveRandomAroundPoint(pUndead->GetPositionX(), pUndead->GetPositionY(), pUndead->GetPositionZ(), 10.0f);
            }
        }
        // spawn the selected mob
        else
        {
            if (Creature* pUndead = pSummoner->SummonCreature(uiEntry, m_aBurningScourgeLoc.m_fX, m_aBurningScourgeLoc.m_fY, m_aBurningScourgeLoc.m_fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                pUndead->GetMotionMaster()->MoveRandomAroundPoint(pUndead->GetPositionX(), pUndead->GetPositionY(), pUndead->GetPositionZ(), 10.0f);
        }

        // spawn a few random zombies
        for (uint8 j = 0; j < 5; ++j)
        {
            pSummoner->GetRandomPoint(m_aBurningScourgeLoc.m_fX, m_aBurningScourgeLoc.m_fY, m_aBurningScourgeLoc.m_fZ, 20.0f, fX, fY, fZ);

            if (Creature* pUndead = pSummoner->SummonCreature(NPC_ZOMBIE, fX, fY, fZ, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
                pUndead->GetMotionMaster()->MoveRandomAroundPoint(pUndead->GetPositionX(), pUndead->GetPositionY(), pUndead->GetPositionZ(), 10.0f);
        }
    }
}

// function that returns a random scourge mob of a specified type
uint32 instance_culling_of_stratholme::GetRandomMobOfType(uint8 uiType) const
{
    switch (uiType)
    {
        case SCOURGE_TYPE_ACOLYTES:
            return NPC_ACOLYTE;
        case SCOURGE_TYPE_GHOUL:
            return urand(0, 1) ? NPC_DEVOURING_GHOUL : NPC_ENRAGING_GHOUL;
        case SCOURGE_TYPE_NECROMANCER:
            return urand(0, 1) ? NPC_MASTER_NECROMANCER : NPC_DARK_NECROMANCER;
        case SCOURGE_TYPE_FIEND:
            return urand(0, 1) ? NPC_CRYPT_FIEND : NPC_TOMB_STALKER;
        case SCOURGE_TYPE_GOLEM:
            return urand(0, 1) ? NPC_BILE_GOLEM : NPC_PATCHWORK_CONSTRUCT;
        case SCOURGE_TYPE_BOSS:
        {
            if (GetData(TYPE_MEATHOOK_EVENT) == IN_PROGRESS)
                return NPC_MEATHOOK;
            if (GetData(TYPE_SALRAMM_EVENT) == IN_PROGRESS)
                return NPC_SALRAMM_THE_FLESHCRAFTER;
        }
    }

    return 0;
}

void instance_culling_of_stratholme::Update(uint32 uiDiff)
{
    // 25min Run - decrease time, update worldstate every ~20s
    // as the time is always saved by m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME], there is no need for an extra timer
    if (m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME])
    {
        if (m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME] <= uiDiff)
            SetData(TYPE_INFINITE_CORRUPTER, FAIL);
        else
        {
            m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME] -= uiDiff;
            if (m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME] / IN_MILLISECONDS % 20 == 0)
                SetData(TYPE_INFINITE_CORRUPTER_TIME, m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME]);
        }

        // This part is needed for a small "hurry up guys" note, TODO, verify 20min
        if (m_auiEncounter[TYPE_INFINITE_CORRUPTER] == IN_PROGRESS && m_auiEncounter[TYPE_INFINITE_CORRUPTER_TIME] <= 24 * MINUTE * IN_MILLISECONDS)
            SetData(TYPE_INFINITE_CORRUPTER, SPECIAL);
    }

    // Small Timer, to remove Grain-Crate WorldState and Spawn Second Chromie
    if (m_uiRemoveCrateStateTimer)
    {
        if (m_uiRemoveCrateStateTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
                DoSpawnChromieIfNeeded(pPlayer);

            DoUpdateWorldState(WORLD_STATE_CRATES, 0);
            DoChromieWhisper(WHISPER_CHROMIE_CRATES);
            m_uiRemoveCrateStateTimer = 0;
        }
        else
            m_uiRemoveCrateStateTimer -= uiDiff;
    }

    // Respawn Arthas after some time
    if (m_uiArthasRespawnTimer)
    {
        if (m_uiArthasRespawnTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
                DoSpawnArthasIfNeeded(pPlayer);

            m_uiArthasRespawnTimer = 0;
        }
        else
            m_uiArthasRespawnTimer -= uiDiff;
    }

    // Handle undead waves
    if (m_uiScourgeWaveTimer)
    {
        if (m_uiScourgeWaveTimer <= uiDiff)
        {
            if (GetData(TYPE_SALRAMM_EVENT) == DONE)
            {
                DoOrSimulateScriptTextForThisInstance(SAY_MEET_TOWN_HALL, NPC_ARTHAS);
                DoUpdateWorldState(WORLD_STATE_WAVE, 0);    // Remove WaveCounter

                // despawn and respawn Arthas in the new location
                if (Creature* pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS))
                    pArthas->ForcedDespawn();
                if (Player* pPlayer = GetPlayerInMap())
                    DoSpawnArthasIfNeeded(pPlayer);
            }
            else
            {
                ++m_uiScourgeWaveCount;
                DoUpdateWorldState(WORLD_STATE_WAVE, m_uiScourgeWaveCount);
                DoSpawnNextScourgeWave();
            }

            m_uiScourgeWaveTimer = 0;
        }
        else
            m_uiScourgeWaveTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_culling_of_stratholme(Map* pMap)
{
    return new instance_culling_of_stratholme(pMap);
}

bool AreaTrigger_at_culling_of_stratholme(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_INN)
    {
        if (pPlayer->IsGameMaster() || pPlayer->IsDead())
            return false;

        if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pPlayer->GetInstanceData())
            pInstance->DoEventAtTriggerIfCan(pAt->id);
    }

    return false;
}

void AddSC_instance_culling_of_stratholme()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_culling_of_stratholme";
    pNewScript->GetInstanceData = &GetInstanceData_instance_culling_of_stratholme;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_culling_of_stratholme";
    pNewScript->pAreaTrigger = &AreaTrigger_at_culling_of_stratholme;
    pNewScript->RegisterSelf();
}
