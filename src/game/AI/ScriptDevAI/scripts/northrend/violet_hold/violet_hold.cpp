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
SDName: Instance_Violet_Hold
SD%Complete: 80
SDComment:
SDCategory: Violet Hold
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "violet_hold.h"

instance_violet_hold::instance_violet_hold(Map* pMap) : ScriptedInstance(pMap),
    m_uiWorldState(0),
    m_uiWorldStateSealCount(100),
    m_uiWorldStatePortalCount(0),

    m_uiPortalId(0),
    m_uiPortalTimer(0),
    m_uiMaxCountPortalLoc(0),

    m_uiSealYellCount(0),
    m_uiIntroSummonTimer(0),
    m_uiEventResetTimer(0),
    m_uiEventStartTimer(0),
    m_uiCyanigosaMoveTimer(0),

    m_bIsVoidDance(false),
    m_bIsDefenseless(false),
    m_bIsDehydratation(false)
{
    Initialize();
}

void instance_violet_hold::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_uiMaxCountPortalLoc = countof(afPortalLocation) - 1;
}

void instance_violet_hold::ResetVariables()
{
    m_uiWorldStateSealCount = 100;
    m_uiWorldStatePortalCount = 0;
    m_uiSealYellCount = 0;
    m_uiIntroSummonTimer = 1000;
}

// Method that will start teh event
void instance_violet_hold::StartEvent()
{
    // Close door; update world state; enable crystals and set variables
    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR);

    UpdateWorldState();
    UpdateCrystals(false);

    m_bIsDefenseless = true;
    m_uiPortalId = urand(0, 2);
    m_uiPortalTimer = 15000;
}

// Method that will reset the event in case of a fail
void instance_violet_hold::ResetEvent()
{
    // reset everything
    ResetVariables();
    UpdateWorldState(false);
    ResetGuards();
    ResetIntroPortals();
    UpdateCrystals(true);

    // respawn sinclari and the controller
    if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI))
    {
        if (!pSinclari->IsAlive())
            pSinclari->Respawn();
    }
    if (Creature* pController = GetSingleCreatureFromStorage(NPC_EVENT_CONTROLLER))
    {
        if (!pController->IsAlive())
            pController->Respawn();
    }

    for (auto spawn : m_vRandomBosses)
    {
        const BossInformation* pData = GetBossInformation(spawn->uiEntry);
        if (!pData)
            return;

        if (m_auiEncounter[pData->uiType] == DONE)
        {
            // Despawn ghost boss
            if (Creature* pGhostBoss = GetSingleCreatureFromStorage(pData->uiGhostEntry, true))
                pGhostBoss->ForcedDespawn();

            // Spawn new boss replacement
            if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
                pSummoner->SummonCreature(pData->uiGhostEntry, spawn->fX, spawn->fY, spawn->fZ, spawn->fO, TEMPSPAWN_DEAD_DESPAWN, 0);

            // Replace Erekem guards
            if (pData->uiType == TYPE_EREKEM)
            {
                // Despawn ghost guards
                for (const auto& guid : m_lArakkoaGuardList)
                    if (Creature* pGhostGuard = instance->GetCreature(guid))
                        pGhostGuard->ForcedDespawn();

                m_lArakkoaGuardList.clear();

                // Spawn new guards replacement
                float fX, fY, fZ, fO;
                for (const auto& guid : m_lErekemGuardList)
                {
                    if (Creature* pGuard = instance->GetCreature(guid))
                    {
                        // Don't allow alive original guards while the boss is dead
                        if (!pGuard->IsDead())
                            pGuard->ForcedDespawn();

                        // Spawn a ghost guard for each original guard
                        pGuard->GetRespawnCoord(fX, fY, fZ, &fO);
                        pGuard->SummonCreature(NPC_ARAKKOA_GUARD, fX, fY, fZ, fO, TEMPSPAWN_DEAD_DESPAWN, 0);
                    }
                }
            }
        }
        // failed encounters make boss to respawn
        else if (m_auiEncounter[pData->uiType] == FAIL)
        {
            if (Creature* pBoss = GetSingleCreatureFromStorage(pData->uiEntry))
            {
                pBoss->Respawn();

                // make sure it has proper flags just in case; normally handled directly in core
                pBoss->SetImmuneToNPC(true);
                pBoss->SetImmuneToPlayer(true);
            }

            if (pData->uiType == TYPE_EREKEM)
            {
                // respawn guards
                for (const auto& guid : m_lErekemGuardList)
                {
                    if (Creature* pGuard = instance->GetCreature(guid))
                    {
                        pGuard->Respawn();

                        // make sure it has proper flags just in case
                        pGuard->SetImmuneToNPC(true);
                        pGuard->SetImmuneToPlayer(true);
                    }
                }
            }
        }

        // Close Door if still open
        if (pData && (m_auiEncounter[pData->uiType] == DONE || m_auiEncounter[pData->uiType] == FAIL))
            UpdateCellForBoss(pData->uiEntry, true);
    }

    // reset data
    SetData(TYPE_MAIN, NOT_STARTED);
}

void instance_violet_hold::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SINCLARI:
            if (pCreature->IsTemporarySummon())
                return;
        case NPC_SINCLARI_ALT:
        case NPC_DOOR_SEAL:
        case NPC_EVENT_CONTROLLER:
            break;

        case NPC_EREKEM:
        case NPC_MORAGG:
        case NPC_ICHORON:
        case NPC_XEVOZZ:
        case NPC_LAVANTHOR:
        case NPC_ZURAMAT:
            m_vRandomBossList.push_back(pCreature->GetEntry());
            break;

        case NPC_PORTAL_INTRO:
            m_lIntroPortalVector.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_HOLD_GUARD:
            m_lGuardsList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_EREKEM_GUARD:
            m_lErekemGuardList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_ARAKKOA_GUARD:
            m_lArakkoaGuardList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_VOID_SENTRY:
            m_lVoidSentriesList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_ICHOR_GLOBULE:
            m_lIchorGlobuleList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_ETHEREAL_SPHERE_1:
        case NPC_ETHEREAL_SPHERE_2:
            m_lEtherealSphereList.push_back(pCreature->GetObjectGuid());
            return;

        case NPC_ARAKKOA:
        case NPC_VOID_LORD:
        case NPC_ETHERAL:
        case NPC_SWIRLING:
        case NPC_WATCHER:
        case NPC_LAVA_HOUND:
        case NPC_CYANIGOSA:
            break;

        case NPC_DEFENSE_SYSTEM:
            if (GetData(TYPE_MAIN) == IN_PROGRESS)
                m_bIsDefenseless = false;
            return;

        default:
            return;
    }
    m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_violet_hold::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // passive behavior
        case NPC_VOID_SENTRY_BALL:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
        case NPC_CYANIGOSA:
            m_uiCyanigosaMoveTimer = 8000;
            break;
    }
}

void instance_violet_hold::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CELL_LAVANTHOR:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_LAVANTHOR, pGo->GetObjectGuid()));
            return;
        case GO_CELL_MORAGG:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_MORAGG, pGo->GetObjectGuid()));
            return;
        case GO_CELL_ZURAMAT:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_ZURAMAT, pGo->GetObjectGuid()));
            return;
        case GO_CELL_XEVOZZ:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_XEVOZZ, pGo->GetObjectGuid()));
            return;
        case GO_CELL_ICHORON:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_ICHORON, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM_GUARD_L:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM_GUARD_R:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;
        case GO_PRISON_CRYSTAL:
            m_lActivationCrystalList.push_back(pGo->GetObjectGuid());
            return;

        case GO_INTRO_CRYSTAL:
        case GO_PRISON_SEAL_DOOR:
            break;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

// Method that will update the cell door for the given boss entry
void instance_violet_hold::UpdateCellForBoss(uint32 uiBossEntry, bool bForceClosing /*= false*/)
{
    BossToCellMap::const_iterator itrCellLower = m_mBossToCellMap.lower_bound(uiBossEntry);
    BossToCellMap::const_iterator itrCellUpper = m_mBossToCellMap.upper_bound(uiBossEntry);

    if (itrCellLower == itrCellUpper)
        return;

    for (BossToCellMap::const_iterator itr = itrCellLower; itr != itrCellUpper; ++itr)
    {
        if (!bForceClosing)
            DoUseDoorOrButton(itr->second);
        else
        {
            GameObject* pGo = instance->GetGameObject(itr->second);
            if (pGo && pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR && pGo->GetGoState() == GO_STATE_ACTIVE)
                pGo->ResetDoorOrButton();
        }
    }
}

void instance_violet_hold::UpdateWorldState(bool bEnable)
{
    m_uiWorldState = bEnable ? 1 : 0;

    DoUpdateWorldState(WORLD_STATE_ID, m_uiWorldState);
    DoUpdateWorldState(WORLD_STATE_SEAL, m_uiWorldStateSealCount);
    DoUpdateWorldState(WORLD_STATE_PORTALS, m_uiWorldStatePortalCount);
}

void instance_violet_hold::OnPlayerEnter(Player* /*pPlayer*/)
{
    UpdateWorldState(m_auiEncounter[TYPE_MAIN] == IN_PROGRESS);

    // randomize bosses
    if (m_vRandomBosses.empty())
    {
        SetRandomBosses();
        ResetEvent();
    }

    // set the initial intro timer
    if (GetData(TYPE_MAIN) == NOT_STARTED && !m_uiIntroSummonTimer)
        m_uiIntroSummonTimer = 1000;
}

void instance_violet_hold::SetData(uint32 uiType, uint32 uiData)
{
    debug_log("SD2: instance_violet_hold: SetData got type %u, data %u.", uiType, uiData);

    switch (uiType)
    {
        case TYPE_MAIN:
        {
            if (uiData == m_auiEncounter[uiType])
                return;
            if (m_auiEncounter[uiType] == DONE)
                return;

            switch (uiData)
            {
                case IN_PROGRESS:
                    StartEvent();
                    break;
                case FAIL:
                    // Despawn sinclari and the controller
                    // creature despawn (creature cleanup is handled in creature_linking)
                    if (Creature* pController = GetSingleCreatureFromStorage(NPC_EVENT_CONTROLLER))
                        pController->ForcedDespawn();

                    // open instance door
                    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR);
                    m_uiEventResetTimer = 20000;            // Timer may not be correct - 20 sec is default reset timer for blizz
                    break;
                case DONE:
                    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR);
                    UpdateWorldState(false);
                    break;
                case SPECIAL:
                    m_uiIntroSummonTimer = 0;
                    m_uiEventStartTimer = 20000;
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        }
        case TYPE_SEAL:
            m_auiEncounter[uiType] = uiData;
            if (uiData == SPECIAL)
            {
                --m_uiWorldStateSealCount;
                DoUpdateWorldState(WORLD_STATE_SEAL, m_uiWorldStateSealCount);

                // Yell at 75%, 50% and 25% shield
                if (m_uiWorldStateSealCount < 100 - 25 * m_uiSealYellCount)
                {
                    if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
                    {
                        // ToDo: I'm not sure if the last yell should be at 25% or at 5%. Needs research
                        ++m_uiSealYellCount;
                        DoScriptText(aSealWeakYell[m_uiSealYellCount - 1], pSinclari);
                    }
                }

                // set achiev to failed
                if (m_bIsDefenseless)
                    m_bIsDefenseless = false;

                if (!m_uiWorldStateSealCount)
                {
                    SetData(TYPE_MAIN, FAIL);
                    SetData(TYPE_SEAL, NOT_STARTED);
                }
            }
            break;
        case TYPE_PORTAL:
        {
            switch (uiData)
            {
                case SPECIAL:                               // timer to next
                    m_uiPortalTimer = 90000;
                    break;
                case DONE:                                  // portal done, set timer to 5 secs
                    m_uiPortalTimer = 3000;
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        }
        case TYPE_LAVANTHOR:
        case TYPE_MORAGG:
        case TYPE_EREKEM:
        case TYPE_ICHORON:
        case TYPE_XEVOZZ:
        case TYPE_ZURAMAT:
            if (uiData == DONE)
                m_uiPortalTimer = 35000;
            if (m_auiEncounter[uiType] != DONE)             // Keep the DONE-information stored
                m_auiEncounter[uiType] = uiData;
            // Handle achievements if necessary
            if (uiData == IN_PROGRESS)
            {
                if (uiType == TYPE_ZURAMAT)
                    m_bIsVoidDance = true;
                else if (uiType == TYPE_ICHORON)
                    m_bIsDehydratation = true;
            }
            if (uiData == SPECIAL && uiType == TYPE_ICHORON)
                m_bIsDehydratation = false;
            if (uiData == FAIL)
                SetData(TYPE_MAIN, FAIL);
            break;
        case TYPE_CYANIGOSA:
            if (uiData == DONE)
                SetData(TYPE_MAIN, DONE);
            if (uiData == FAIL)
                SetData(TYPE_MAIN, FAIL);
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE)
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

uint32 instance_violet_hold::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];
    return 0;
}

void instance_violet_hold::Load(const char* chrIn)
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

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

// Method to reset the intro portals
void instance_violet_hold::ResetIntroPortals()
{
    for (const auto& guid : m_lIntroPortalVector)
        if (Creature* pPortal = instance->GetCreature(guid))
            pPortal->Respawn();
}

// Method to spawn event portal
void instance_violet_hold::SpawnPortal()
{
    if (const PortalData* pData = GetPortalData())
    {
        if (Creature* pController = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
        {
            uint32 uiPortalEntry = pData->pPortalType == PORTAL_TYPE_NORM ? NPC_PORTAL : NPC_PORTAL_ELITE;

            pController->SummonCreature(uiPortalEntry, pData->fX, pData->fY, pData->fZ, pData->fOrient, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 1800 * IN_MILLISECONDS);
        }
    }
}

// Method to set a random portal id
void instance_violet_hold::SetPortalId()
{
    if (IsCurrentPortalForTrash())
    {
        // Find another Trash portal position
        uint8 uiTemp = m_uiPortalId + urand(1, m_uiMaxCountPortalLoc - 1);
        // Decrease m_uiMaxCountPortalLoc so that the center position is skipped
        uiTemp %= m_uiMaxCountPortalLoc - 1;

        debug_log("SD2: instance_violet_hold: SetPortalId %u, old was id %u.", uiTemp, m_uiPortalId);

        m_uiPortalId = uiTemp;
    }
    else if (GetCurrentPortalNumber() == 18)
    {
        debug_log("SD2: instance_violet_hold: SetPortalId %u (Cyanigosa), old was id %u.", 0, m_uiPortalId);
        m_uiPortalId = 0;
    }
    else
    {
        debug_log("SD2: instance_violet_hold: SetPortalId %u (is boss), old was id %u.", m_uiMaxCountPortalLoc, m_uiPortalId);
        m_uiPortalId = m_uiMaxCountPortalLoc;
    }
}

// Function to return the boss spawn location
BossSpawn* instance_violet_hold::CreateBossSpawnByEntry(uint32 uiEntry)
{
    BossSpawn* pBossSpawn = new BossSpawn;
    pBossSpawn->uiEntry = uiEntry;

    if (Creature* pBoss = GetSingleCreatureFromStorage(uiEntry))
        pBoss->GetRespawnCoord(pBossSpawn->fX, pBossSpawn->fY, pBossSpawn->fZ, &(pBossSpawn->fO));

    return pBossSpawn;
}

// Method to randomise encounters; only 2 are actually fighting
void instance_violet_hold::SetRandomBosses()
{
    // Store bosses that are already done
    for (const auto& i : aBossInformation)
    {
        if (m_auiEncounter[i.uiType] == DONE)
            m_vRandomBosses.push_back(CreateBossSpawnByEntry(i.uiEntry));
    }

    if (m_vRandomBosses.size() < 2)                         // Get some new random bosses
    {
        std::shuffle(m_vRandomBossList.begin(), m_vRandomBossList.end(), *GetRandomGenerator());
        // two required, in case the first is already pushed to m_vRandomBosses
        if (m_vRandomBossList.size() < 2)
            script_error_log("instance_violet_hold, Mini Bosses are not properly spawned");
        else
            m_vRandomBossList.resize(2);

        // Fill up some random bosses
        for (std::vector<uint32>::const_iterator itr = m_vRandomBossList.begin(); itr != m_vRandomBossList.end(); ++itr)
        {
            if (m_vRandomBosses.empty() || m_vRandomBosses[0]->uiEntry != *itr)
                m_vRandomBosses.push_back(CreateBossSpawnByEntry(*itr));
        }
    }

    for (uint8 i = 0; i < m_vRandomBosses.size(); ++i)
        debug_log("SD2: instance_violet_hold random boss %u is entry %u", i, m_vRandomBosses[i]->uiEntry);
}

// Method that will release the boss from the cage
void instance_violet_hold::DoReleaseBoss(uint32 entry)
{
    const BossInformation* pData = GetBossInformation(entry);
    if (!pData)
    {
        script_error_log("instance_violet_hold: cannot find boss information");
        return;
    }

    Creature* pBoss = GetSingleCreatureFromStorage(GetData(pData->uiType) != DONE ? pData->uiEntry : pData->uiGhostEntry);
    if (!pBoss)
    {
        script_error_log("instance_violet_hold: cannot find boss creature %u", pBoss->GetEntry());
        return;
    }

    UpdateCellForBoss(pData->uiEntry);

    if (pData->iSayEntry)
        DoScriptText(pData->iSayEntry, pBoss);

    // move boss outside of the cell; flags are removed by DB script
    pBoss->GetMotionMaster()->MoveWaypoint();

    // Handle Erekem guards
    if (pData->uiType == TYPE_EREKEM)
    {
        GuidList& lAddGuids = GetData(TYPE_EREKEM) != DONE ? m_lErekemGuardList : m_lArakkoaGuardList;

        // creature path depending on location
        for (const auto& guid : lAddGuids)
            if (Creature* pAdd = instance->GetCreature(guid))
                pAdd->GetMotionMaster()->MoveWaypoint(pAdd->GetPositionX() > 1880.0f ? 0 : 1);
    }
}

// Method to handle guards
void instance_violet_hold::ResetGuards()
{
    for (const auto& guid : m_lGuardsList)
        if (Creature* pGuard = instance->GetCreature(guid))
            pGuard->Respawn();
}

// Method to reset Crystals
void instance_violet_hold::UpdateCrystals(bool reset)
{
    for (const auto& guid : m_lActivationCrystalList)
        DoToggleGameObjectFlags(guid, GO_FLAG_NO_INTERACT, reset);
}

// Method to clear the adds from an encounter
void instance_violet_hold::DoClearBossMobs(GuidList& list)
{
    for (const auto& guid : list)
        if (Creature* pMob = instance->GetCreature(guid))
            pMob->ForcedDespawn();
}

// Method to spawn an intro mob
void instance_violet_hold::DoSpawnIntroMob()
{
    uint32 mobEntry = aRandomIntroNpcs[urand(0, 3)];

    Creature* pPortal = instance->GetCreature(m_lIntroPortalVector[urand(0, m_lIntroPortalVector.size() - 1)]);
    if (!pPortal)
    {
        script_error_log("instance_violet_hold, cannot find intro portals");
        return;
    }

    // spawn mob and make it move to the entrance; it will attack the guards automatically
    if (Creature* pMob = pPortal->SummonCreature(mobEntry, 0, 0, 0, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
    {
        pMob->SetWalk(false);
        pMob->GetMotionMaster()->MovePoint(1, fSealAttackLoc[0], fSealAttackLoc[1], fSealAttackLoc[2]);
    }
}

bool instance_violet_hold::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_DEFENSELES:
            return m_bIsDefenseless;
        case ACHIEV_CRIT_DEHYDRATATION:
            return m_bIsDehydratation;
        case ACHIEV_CRIT_VOID_DANCE:
            return m_bIsVoidDance;

        default:
            return false;
    }
}

void instance_violet_hold::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, IN_PROGRESS);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, IN_PROGRESS);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, IN_PROGRESS);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, IN_PROGRESS);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, IN_PROGRESS);
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, IN_PROGRESS);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, IN_PROGRESS);
            break;
        case NPC_AZURE_CAPTAIN:
        case NPC_AZURE_RAIDER:
        case NPC_AZURE_SORCEROR:
        case NPC_AZURE_STALKER:
        case NPC_AZURE_INVADER:
        case NPC_MAGE_HUNTER:
        case NPC_AZURE_SPELLBREAKER:
        case NPC_AZURE_BINDER:
        case NPC_AZURE_MAGE_SLAYER:
            // Interrupt door seal casting (if necessary)
            pCreature->InterruptNonMeleeSpells(false);
            break;
    }
}

void instance_violet_hold::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, FAIL);
            DoClearBossMobs(m_lVoidSentriesList);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, FAIL);
            DoClearBossMobs(m_lEtherealSphereList);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, FAIL);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, FAIL);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, FAIL);
            break;
        case NPC_EREKEM_GUARD:
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, FAIL);
            DoClearBossMobs(m_lIchorGlobuleList);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, FAIL);
            break;
        case NPC_AZURE_CAPTAIN:
        case NPC_AZURE_RAIDER:
        case NPC_AZURE_SORCEROR:
        case NPC_AZURE_STALKER:
        case NPC_AZURE_INVADER:
        case NPC_MAGE_HUNTER:
        case NPC_AZURE_SPELLBREAKER:
        case NPC_AZURE_BINDER:
        case NPC_AZURE_MAGE_SLAYER:
            // Allow them to finish off the door seal
            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MovePoint(1, fSealAttackLoc[0], fSealAttackLoc[1], fSealAttackLoc[2]);
            break;
    }
}

void instance_violet_hold::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, DONE);
            DoClearBossMobs(m_lVoidSentriesList);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, DONE);
            DoClearBossMobs(m_lEtherealSphereList);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, DONE);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, DONE);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, DONE);
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, DONE);
            DoClearBossMobs(m_lIchorGlobuleList);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, DONE);
            break;
        case NPC_VOID_SENTRY:
            if (GetData(TYPE_ZURAMAT) == IN_PROGRESS)
                m_bIsVoidDance = false;
            break;
    }
}

void instance_violet_hold::Update(uint32 uiDiff)
{
    // event reset timer
    if (m_uiEventResetTimer)
    {
        if (m_uiEventResetTimer <= uiDiff)
        {
            ResetEvent();
            m_uiEventResetTimer = 0;
        }
        else
            m_uiEventResetTimer -= uiDiff;
    }

    // event start timer
    if (m_uiEventStartTimer)
    {
        if (m_uiEventStartTimer <= uiDiff)
        {
            SetData(TYPE_MAIN, IN_PROGRESS);
            m_uiEventStartTimer = 0;
        }
        else
            m_uiEventStartTimer -= uiDiff;
    }

    // spawn intro mobs
    if (m_uiIntroSummonTimer)
    {
        if (m_uiIntroSummonTimer <= uiDiff)
        {
            // Note: timer and frequency of intro mobs requires more research
            DoSpawnIntroMob();
            m_uiIntroSummonTimer = urand(5000, 10000);
        }
        else
            m_uiIntroSummonTimer -= uiDiff;
    }

    if (m_uiCyanigosaMoveTimer)
    {
        if (m_uiCyanigosaMoveTimer <= uiDiff)
        {
            if (Creature* pCyanigosa = GetSingleCreatureFromStorage(NPC_CYANIGOSA))
                pCyanigosa->GetMotionMaster()->MoveJump(afPortalLocation[8].fX, afPortalLocation[8].fY, afPortalLocation[8].fZ, pCyanigosa->GetSpeed(MOVE_RUN) * 2, 10.0f);

            m_uiCyanigosaMoveTimer = 0;
        }
        else
            m_uiCyanigosaMoveTimer -= uiDiff;
    }

    // no update when event isn't in progress
    if (m_auiEncounter[TYPE_MAIN] != IN_PROGRESS)
        return;

    // portal timer
    if (m_uiPortalTimer)
    {
        if (m_uiPortalTimer <= uiDiff)
        {
            DoUpdateWorldState(WORLD_STATE_PORTALS, ++m_uiWorldStatePortalCount);

            SetPortalId();
            SpawnPortal();

            m_uiPortalTimer = 0;
        }
        else
            m_uiPortalTimer -= uiDiff;
    }
}

// Function that returns boss info by entry
BossInformation const* instance_violet_hold::GetBossInformation(uint32 uiEntry/* = 0*/)
{
    uint32 mEntry = uiEntry;
    if (!mEntry)
    {
        if (GetCurrentPortalNumber() == 6 && !m_vRandomBosses.empty())
            mEntry = m_vRandomBosses[0]->uiEntry;
        else if (GetCurrentPortalNumber() == 12 && m_vRandomBosses.size() >= 2)
            mEntry = m_vRandomBosses[1]->uiEntry;
    }

    if (!mEntry)
        return nullptr;

    for (const auto& i : aBossInformation)
    {
        if (i.uiEntry == mEntry)
            return &i;
    }

    return nullptr;
}

void instance_violet_hold::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n stopintro, erekem, moragg, ichoron, xevozz, lavanthor, zuramat");
}

// Debug commands for Violet Hold bosses
void instance_violet_hold::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;

    if (val == "stopintro")
        m_uiIntroSummonTimer = 0;
    else if (val == "erekem")
        DoReleaseBoss(NPC_EREKEM);
    else if (val == "moragg")
        DoReleaseBoss(NPC_MORAGG);
    else if (val == "ichoron")
        DoReleaseBoss(NPC_ICHORON);
    else if (val == "xevozz")
        DoReleaseBoss(NPC_XEVOZZ);
    else if (val == "lavanthor")
        DoReleaseBoss(NPC_LAVANTHOR);
    else if (val == "zuramat")
        DoReleaseBoss(NPC_ZURAMAT);
}

instance_violet_hold::~instance_violet_hold()
{
    // Need to free std::vector<sBossSpawn*> m_vRandomBosses;
    for (std::vector<BossSpawn*>::const_iterator itr = m_vRandomBosses.begin(); itr != m_vRandomBosses.end(); ++itr)
    {
        if (*itr)
            delete (*itr);
    }
}

InstanceData* GetInstanceData_instance_violet_hold(Map* pMap)
{
    return new instance_violet_hold(pMap);
}

void AddSC_instance_violet_hold()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_violet_hold";
    pNewScript->GetInstanceData = GetInstanceData_instance_violet_hold;
    pNewScript->RegisterSelf();
}
