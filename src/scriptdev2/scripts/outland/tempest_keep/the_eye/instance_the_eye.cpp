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
SDName: Instance_The_Eye
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

instance_the_eye::instance_the_eye(Map* pMap) : ScriptedInstance(pMap),
    m_uiKaelthasEventPhase(0)
{
    Initialize();
}

void instance_the_eye::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_the_eye::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_the_eye::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_THALADRED:
        case NPC_TELONICUS:
        case NPC_CAPERNIAN:
        case NPC_SANGUINAR:
        case NPC_KAELTHAS:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_the_eye::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ARCANE_DOOR_HORIZ_3:
        case GO_ARCANE_DOOR_HORIZ_4:
        case GO_KAEL_STATUE_LEFT:
        case GO_KAEL_STATUE_RIGHT:
        case GO_BRIDGE_WINDOW:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_the_eye::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ALAR:
        case TYPE_SOLARIAN:
        case TYPE_VOIDREAVER:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_KAELTHAS:
            // Don't set the same data twice
            if (m_auiEncounter[uiType] == uiData)
                break;
            DoUseDoorOrButton(GO_ARCANE_DOOR_HORIZ_3);
            DoUseDoorOrButton(GO_ARCANE_DOOR_HORIZ_4);
            if (uiData == FAIL)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_KAEL_STATUE_LEFT))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_KAEL_STATUE_RIGHT))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_BRIDGE_WINDOW))
                    pGo->ResetDoorOrButton();

                // Respawn or reset the advisors
                for (uint8 i = 0; i < MAX_ADVISORS; ++i)
                {
                    if (Creature* pTemp = GetSingleCreatureFromStorage(aAdvisors[i]))
                    {
                        if (!pTemp->isAlive())
                            pTemp->Respawn();
                        else
                            pTemp->AI()->EnterEvadeMode();
                    }
                }
            }
            m_auiEncounter[uiType] = uiData;
            break;
    }
}

uint32 instance_the_eye::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

InstanceData* GetInstanceData_instance_the_eye(Map* pMap)
{
    return new instance_the_eye(pMap);
}

void AddSC_instance_the_eye()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_the_eye";
    pNewScript->GetInstanceData = &GetInstanceData_instance_the_eye;
    pNewScript->RegisterSelf();
}
