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
SDName: Instance_Hellfire_Ramparts
SD%Complete: 50
SDComment:
SDCategory: Hellfire Ramparts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hellfire_ramparts.h"

instance_ramparts::instance_ramparts(Map* pMap) : ScriptedInstance(pMap),
    m_uiSentryCounter(0)
{
    Initialize();
}

void instance_ramparts::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_ramparts::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_VAZRUDEN_HERALD:
        case NPC_VAZRUDEN:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_HELLFIRE_SENTRY:
            m_lSentryGUIDs.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_ramparts::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_FEL_IRON_CHEST:
        case GO_FEL_IRON_CHEST_H:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_ramparts::SetData(uint32 uiType, uint32 uiData)
{
    debug_log("SD2: Instance Ramparts: SetData received for type %u with data %u", uiType, uiData);

    switch (uiType)
    {
        case TYPE_VAZRUDEN:
            if (m_auiEncounter[0] == uiData)
                return;
            if (uiData == DONE && m_auiEncounter[1] == DONE)
                DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_FEL_IRON_CHEST : GO_FEL_IRON_CHEST_H, HOUR);
            if (uiData == FAIL && m_auiEncounter[0] != FAIL)
                DoFailVazruden();
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_NAZAN:
            if (m_auiEncounter[1] == uiData)
                return;
            if (uiData == SPECIAL)                          // SPECIAL set via ACID
            {
                ++m_uiSentryCounter;

                if (m_uiSentryCounter == 2)
                {
                    m_auiEncounter[1] = uiData;
                    if (Creature* pVazruden = GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
                    {
                        if (ScriptedAI* pVazrudenAI = dynamic_cast<ScriptedAI*>(pVazruden->AI()))
                            pVazrudenAI->SendAIEvent(AI_EVENT_CUSTOM_A, pVazruden, pVazruden);
                    }
                }

                return;
            }
            if (uiData == DONE && m_auiEncounter[0] == DONE)
            {
                DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_FEL_IRON_CHEST : GO_FEL_IRON_CHEST_H, HOUR);
                DoToggleGameObjectFlags(instance->IsRegularDifficulty() ? GO_FEL_IRON_CHEST : GO_FEL_IRON_CHEST_H, GO_FLAG_NO_INTERACT, false);
                if (GameObject* chest = GetSingleGameObjectFromStorage(instance->IsRegularDifficulty() ? GO_FEL_IRON_CHEST : GO_FEL_IRON_CHEST_H))
                    if (Player* player = GetPlayerInMap(false, false))
                        chest->GenerateLootFor(player);
            }
            if (uiData == FAIL && m_auiEncounter[1] != FAIL)
                DoFailVazruden();

            m_auiEncounter[1] = uiData;
            break;
    }
}

uint32 instance_ramparts::GetData(uint32 uiType) const
{
    if (uiType == TYPE_VAZRUDEN)
        return m_auiEncounter[0];

    if (uiType == TYPE_NAZAN)
        return m_auiEncounter[1];

    return 0;
}

void instance_ramparts::DoFailVazruden()
{
    // Store FAIL for both types
    m_auiEncounter[0] = FAIL;
    m_auiEncounter[1] = FAIL;

    // Restore Sentries (counter and respawn them)
    m_uiSentryCounter = 0;
    for (GuidList::const_iterator itr = m_lSentryGUIDs.begin(); itr != m_lSentryGUIDs.end(); ++itr)
    {
        if (Creature* pSentry = instance->GetCreature(*itr))
            pSentry->Respawn();
    }

    // Respawn or Reset Vazruden the herald
    if (Creature* pVazruden = GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
    {
        if (!pVazruden->IsAlive())
            pVazruden->Respawn();
        else
        {
            if (ScriptedAI* pVazrudenAI = dynamic_cast<ScriptedAI*>(pVazruden->AI()))
                pVazrudenAI->EnterEvadeMode();
        }
    }

    // Despawn Vazruden
    if (Creature* pVazruden = GetSingleCreatureFromStorage(NPC_VAZRUDEN))
        pVazruden->ForcedDespawn();
}

InstanceData* GetInstanceData_instance_ramparts(Map* pMap)
{
    return new instance_ramparts(pMap);
}

void AddSC_instance_ramparts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_ramparts";
    pNewScript->GetInstanceData = &GetInstanceData_instance_ramparts;
    pNewScript->RegisterSelf();
}
