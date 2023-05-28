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
SDName: Instance_Old_Hillsbrad
SD%Complete: 75
SDComment: Thrall reset on server restart is not supported, because of core limitation.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "old_hillsbrad.h"

instance_old_hillsbrad::instance_old_hillsbrad(Map* pMap) : ScriptedInstance(pMap),
    m_uiBarrelCount(0),
    m_uiThrallEventCount(0),
    m_uiThrallResetTimer(0)
{
    Initialize();
}

void instance_old_hillsbrad::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_old_hillsbrad::OnPlayerEnter(Player* pPlayer)
{
    // ToDo: HandleThrallRelocation();
    // Note: this isn't yet supported because of the grid load / unload

    // Spawn Drake if necessary
    if (GetData(TYPE_DRAKE) == DONE || GetData(TYPE_BARREL_DIVERSION) != DONE)
        return;

    if (GetSingleCreatureFromStorage(NPC_DRAKE, true))
        return;

    pPlayer->SummonCreature(NPC_DRAKE, aDrakeSummonLoc[0], aDrakeSummonLoc[1], aDrakeSummonLoc[2], aDrakeSummonLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);
}

void instance_old_hillsbrad::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_THRALL:
        case NPC_TARETHA:
        case NPC_EROZION:
        case NPC_ARMORER:
        case NPC_BARN_PROTECTOR:
        case NPC_BARN_LOOKOUT:
        case NPC_YOUNG_BLANCHY:
        case NPC_DRAKE:
        case NPC_SKARLOC:
        case NPC_EPOCH:
        case NPC_BARTOLO:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ORC_PRISONER:
            // Sort the orcs which are inside the houses
            if (pCreature->GetPositionZ() > 53.4f)
            {
                if (pCreature->GetPositionY() > 150.0f)
                    m_lLeftPrisonersList.push_back(pCreature->GetObjectGuid());
                else
                    m_lRightPrisonersList.push_back(pCreature->GetObjectGuid());
            }
            break;
    }
}

void instance_old_hillsbrad::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DRAKE:   SetData(TYPE_DRAKE, DONE);   break;
        case NPC_SKARLOC: SetData(TYPE_SKARLOC, DONE); break;
        case NPC_EPOCH:   SetData(TYPE_EPOCH, DONE);   break;
    }
}

void instance_old_hillsbrad::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DRAKE:
            SetData(TYPE_DRAKE, IN_PROGRESS);
            DoUpdateWorldState(WORLD_STATE_OLD_HILLSBRAD_BARREL_COUNT, 0);
            break;
        case NPC_SKARLOC: SetData(TYPE_SKARLOC, IN_PROGRESS); break;
        case NPC_EPOCH:   SetData(TYPE_EPOCH, IN_PROGRESS);   break;
    }
}

void instance_old_hillsbrad::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DRAKE:   SetData(TYPE_DRAKE, FAIL);   break;
        case NPC_SKARLOC: SetData(TYPE_SKARLOC, FAIL); break;
        case NPC_EPOCH:   SetData(TYPE_EPOCH, FAIL);   break;
    }
}

void instance_old_hillsbrad::OnObjectCreate(GameObject* pGo)
{
    if (pGo->GetEntry() == GO_ROARING_FLAME)
        m_lRoaringFlamesList.push_back(pGo->GetObjectGuid());
    else if (pGo->GetEntry() == GO_PRISON_DOOR)
        m_goEntryGuidStore[GO_PRISON_DOOR] = pGo->GetObjectGuid();
}

void instance_old_hillsbrad::HandleThrallRelocation()
{
    // reset instance data
    SetData(TYPE_THRALL_EVENT, IN_PROGRESS);

    if (Creature* pThrall = GetSingleCreatureFromStorage(NPC_THRALL))
    {
        debug_log("SD2: Instance Old Hillsbrad: Thrall relocation");

        if (!pThrall->IsAlive())
            pThrall->Respawn();

        // epoch failed, reloc to inn
        if (GetData(TYPE_ESCORT_INN) == DONE)
            pThrall->GetMap()->CreatureRelocation(pThrall, 2660.57f, 659.173f, 61.9370f, 5.76f);
        // barn to inn failed, reloc to barn
        else if (GetData(TYPE_ESCORT_BARN) == DONE)
            pThrall->GetMap()->CreatureRelocation(pThrall, 2486.91f, 626.356f, 58.0761f, 4.66f);
        // keep to barn failed, reloc to keep
        else if (GetData(TYPE_SKARLOC) == DONE)
            pThrall->GetMap()->CreatureRelocation(pThrall, 2063.40f, 229.509f, 64.4883f, 2.23f);
        // prison to keep failed, reloc to prison
        else
            pThrall->GetMap()->CreatureRelocation(pThrall, 2231.89f, 119.95f, 82.2979f, 4.21f);
    }
}

void instance_old_hillsbrad::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_BARREL_DIVERSION:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
            {
                if (m_uiBarrelCount >= MAX_BARRELS)
                    return;

                // Update barrels used and world state
                ++m_uiBarrelCount;
                DoUpdateWorldState(WORLD_STATE_OLD_HILLSBRAD_BARREL_COUNT, m_uiBarrelCount);

                debug_log("SD2: Instance Old Hillsbrad: go_barrel_old_hillsbrad count %u", m_uiBarrelCount);

                // Set encounter to done, and spawn Liutenant Drake
                if (m_uiBarrelCount == MAX_BARRELS)
                {
                    UpdateLodgeQuestCredit();

                    if (Player* pPlayer = GetPlayerInMap())
                    {
                        pPlayer->SummonCreature(NPC_DRAKE, aDrakeSummonLoc[0], aDrakeSummonLoc[1], aDrakeSummonLoc[2], aDrakeSummonLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0);

                        // set the houses on fire
                        for (GuidList::const_iterator itr = m_lRoaringFlamesList.begin(); itr != m_lRoaringFlamesList.end(); ++itr)
                            DoRespawnGameObject(*itr, 30 * MINUTE);

                        // move the orcs outside the houses
                        float fX, fY, fZ;
                        for (GuidList::const_iterator itr = m_lRightPrisonersList.begin(); itr != m_lRightPrisonersList.end(); ++itr)
                        {
                            if (Creature* pOrc = instance->GetCreature(*itr))
                            {
                                pOrc->GetRandomPoint(afInstanceLoc[0][0], afInstanceLoc[0][1], afInstanceLoc[0][2], 10.0f, fX, fY, fZ);
                                pOrc->SetWalk(false);
                                pOrc->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                                pOrc->SetStandState(UNIT_STAND_STATE_STAND);
                            }
                        }
                        for (GuidList::const_iterator itr = m_lLeftPrisonersList.begin(); itr != m_lLeftPrisonersList.end(); ++itr)
                        {
                            if (Creature* pOrc = instance->GetCreature(*itr))
                            {
                                pOrc->GetRandomPoint(afInstanceLoc[1][0], afInstanceLoc[1][1], afInstanceLoc[1][2], 10.0f, fX, fY, fZ);
                                pOrc->SetWalk(false);
                                pOrc->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                                pOrc->SetStandState(UNIT_STAND_STATE_STAND);
                            }
                        }
                    }
                    else
                        debug_log("SD2: Instance Old Hillsbrad: SetData (Type: %u Data %u) cannot find any pPlayer.", uiType, uiData);

                    SetData(TYPE_BARREL_DIVERSION, DONE);
                }
            }
            break;
        case TYPE_THRALL_EVENT:
            // nothing to do if already done and thrall respawn
            if (GetData(TYPE_THRALL_EVENT) == DONE)
                return;
            m_auiEncounter[uiType] = uiData;
            if (uiData == FAIL)
            {
                // despawn the bosses if necessary
                if (Creature* pSkarloc = GetSingleCreatureFromStorage(NPC_SKARLOC, true))
                    pSkarloc->ForcedDespawn();
                if (Creature* pEpoch = GetSingleCreatureFromStorage(NPC_EPOCH, true))
                    pEpoch->ForcedDespawn();

                if (m_uiThrallEventCount <= MAX_WIPE_COUNTER)
                {
                    ++m_uiThrallEventCount;
                    debug_log("SD2: Instance Old Hillsbrad: Thrall event failed %u times.", m_uiThrallEventCount);

                    // reset Thrall on timer
                    m_uiThrallResetTimer = 30000;
                }
                // If we already respawned Thrall too many times, the event is failed for good
                else if (m_uiThrallEventCount > MAX_WIPE_COUNTER)
                    debug_log("SD2: Instance Old Hillsbrad: Thrall event failed %u times. Reset instance required.", m_uiThrallEventCount);
            }
            break;
        case TYPE_DRAKE:
        case TYPE_SKARLOC:
        case TYPE_ESCORT_BARN:
        case TYPE_ESCORT_INN:
        case TYPE_EPOCH:
            m_auiEncounter[uiType] = uiData;
            debug_log("SD2: Instance Old Hillsbrad: Thrall event type %u adjusted to data %u.", uiType, uiData);
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_old_hillsbrad::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_old_hillsbrad::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    // custom reload - if the escort event or the Epoch event are not done, then reset the escort
    // this is done, because currently we cannot handle Thrall relocation on server reset
    if (m_auiEncounter[5] != DONE)
    {
        m_auiEncounter[2] = NOT_STARTED;
        m_auiEncounter[3] = NOT_STARTED;
        m_auiEncounter[4] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_old_hillsbrad::UpdateLodgeQuestCredit()
{
    Map::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        for (const auto& player : players)
        {
            if (Player* pPlayer = player.getSource())
                pPlayer->KilledMonsterCredit(NPC_LODGE_QUEST_TRIGGER);
        }
    }
}

void instance_old_hillsbrad::Update(uint32 uiDiff)
{
    if (m_uiThrallResetTimer)
    {
        if (m_uiThrallResetTimer <= uiDiff)
        {
            HandleThrallRelocation();
            m_uiThrallResetTimer = 0;
        }
        else
            m_uiThrallResetTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_old_hillsbrad(Map* pMap)
{
    return new instance_old_hillsbrad(pMap);
}

bool ProcessEventId_event_go_barrel_old_hillsbrad(uint32 /*uiEventId*/, Object* pSource, Object* pTarget, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_old_hillsbrad* pInstance = (instance_old_hillsbrad*)((Player*)pSource)->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_BARREL_DIVERSION) == DONE || (GameObject*)pTarget->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT))
                return true;

            pInstance->SetData(TYPE_BARREL_DIVERSION, IN_PROGRESS);

            // Don't allow players to use same object twice
            if (pTarget->GetTypeId() == TYPEID_GAMEOBJECT)
                ((GameObject*)pTarget)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

            return true;
        }
    }
    return false;
}

void AddSC_instance_old_hillsbrad()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_old_hillsbrad";
    pNewScript->GetInstanceData = &GetInstanceData_instance_old_hillsbrad;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_barrel_old_hillsbrad";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_barrel_old_hillsbrad;
    pNewScript->RegisterSelf();
}
