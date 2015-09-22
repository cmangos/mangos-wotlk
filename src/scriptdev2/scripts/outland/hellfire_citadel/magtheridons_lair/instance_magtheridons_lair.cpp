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
SDName: Instance_Magtheridons_Lair
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Magtheridon's lair
EndScriptData */

#include "precompiled.h"
#include "magtheridons_lair.h"

instance_magtheridons_lair::instance_magtheridons_lair(Map* pMap) : ScriptedInstance(pMap),
    m_uiRandYellTimer(90000),
    m_uiCageBreakTimer(0),
    m_uiCageBreakStage(0)
{
    Initialize();
}

void instance_magtheridons_lair::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_magtheridons_lair::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_magtheridons_lair::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MAGTHERIDON:
            m_mNpcEntryGuidStore[NPC_MAGTHERIDON] = pCreature->GetObjectGuid();
            break;
        case NPC_CHANNELER:
            m_lChannelerGuidList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_magtheridons_lair::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOODAD_HF_MAG_DOOR01:                       // event door
            m_mGoEntryGuidStore[GO_DOODAD_HF_MAG_DOOR01] = pGo->GetObjectGuid();
            break;
        case GO_DOODAD_HF_RAID_FX01:                        // hall
        case GO_MAGTHERIDON_COLUMN_003:                     // six columns
        case GO_MAGTHERIDON_COLUMN_002:
        case GO_MAGTHERIDON_COLUMN_004:
        case GO_MAGTHERIDON_COLUMN_005:
        case GO_MAGTHERIDON_COLUMN_000:
        case GO_MAGTHERIDON_COLUMN_001:
            m_lColumnGuidList.push_back(pGo->GetObjectGuid());
            break;
        case GO_MANTICRON_CUBE:
            m_lCubeGuidList.push_back(pGo->GetObjectGuid());
            break;
    }
}

void instance_magtheridons_lair::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_MAGTHERIDON_EVENT:
            switch (uiData)
            {
                case FAIL:
                    // Reset channelers
                    for (GuidList::const_iterator itr = m_lChannelerGuidList.begin(); itr != m_lChannelerGuidList.end(); ++itr)
                    {
                        if (Creature* pChanneler = instance->GetCreature(*itr))
                        {
                            if (!pChanneler->isAlive())
                                pChanneler->Respawn();
                        }
                    }

                    // Reset columns
                    for (GuidList::const_iterator itr = m_lColumnGuidList.begin(); itr != m_lColumnGuidList.end(); ++itr)
                    {
                        if (GameObject* pColumn = instance->GetGameObject(*itr))
                            pColumn->ResetDoorOrButton();
                    }

                    // Reset cubes
                    for (GuidList::const_iterator itr = m_lCubeGuidList.begin(); itr != m_lCubeGuidList.end(); ++itr)
                        DoToggleGameObjectFlags(*itr, GO_FLAG_NO_INTERACT, true);

                    // Reset timers and doors
                    SetData(TYPE_CHANNELER_EVENT, NOT_STARTED);
                    m_uiCageBreakTimer = 0;
                    m_uiCageBreakStage = 0;

                    // no break;
                case DONE:
                    // Reset door on Fail or Done
                    if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_DOODAD_HF_MAG_DOOR01))
                        pDoor->ResetDoorOrButton();
                    break;
                case IN_PROGRESS:
                    // Set boss in combat
                    if (Creature* pMagtheridon = GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
                    {
                        if (pMagtheridon->isAlive())
                        {
                            pMagtheridon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pMagtheridon->SetInCombatWithZone();
                        }
                    }
                    // Enable cubes
                    for (GuidList::const_iterator itr = m_lCubeGuidList.begin(); itr != m_lCubeGuidList.end(); ++itr)
                        DoToggleGameObjectFlags(*itr, GO_FLAG_NO_INTERACT, false);
                    break;
                case SPECIAL:
                    // Collapse the hall - don't store this value
                    for (GuidList::const_iterator itr = m_lColumnGuidList.begin(); itr != m_lColumnGuidList.end(); ++itr)
                        DoUseDoorOrButton(*itr);
                    // return, don't set encounter as special
                    return;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_CHANNELER_EVENT:
            // don't set the same data twice
            if (m_auiEncounter[1] == uiData)
                break;
            // stop the event timer on fail
            if (uiData == FAIL)
            {
                m_uiCageBreakTimer = 0;
                m_uiCageBreakStage = 0;

                // Reset door on Fail
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_DOODAD_HF_MAG_DOOR01))
                    pDoor->ResetDoorOrButton();

                // Reset Magtheridon
                if (Creature* pMagtheridon = GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
                {
                    if (pMagtheridon->isAlive())
                        pMagtheridon->AI()->EnterEvadeMode();
                }
            }
            // prepare Magtheridon for release
            if (uiData == IN_PROGRESS)
            {
                if (Creature* pMagtheridon = GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
                {
                    if (pMagtheridon->isAlive())
                    {
                        DoScriptText(EMOTE_EVENT_BEGIN, pMagtheridon);
                        m_uiCageBreakTimer = MINUTE * IN_MILLISECONDS;
                    }
                }

                // combat door
                DoUseDoorOrButton(GO_DOODAD_HF_MAG_DOOR01);
            }
            m_auiEncounter[uiType] = uiData;
            break;
    }

    // Instance save isn't needed for this one
}

uint32 instance_magtheridons_lair::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_magtheridons_lair::Update(uint32 uiDiff)
{
    // Prepare to release Magtheridon
    if (m_uiCageBreakTimer)
    {
        if (m_uiCageBreakTimer <= uiDiff)
        {
            switch (m_uiCageBreakStage)
            {
                case 0:
                    if (Creature* pMagtheridon = GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
                    {
                        if (pMagtheridon->isAlive())
                        {
                            DoScriptText(EMOTE_NEARLY_FREE, pMagtheridon);
                            m_uiCageBreakTimer = MINUTE * IN_MILLISECONDS;
                        }
                    }
                    break;
                case 1:
                    SetData(TYPE_MAGTHERIDON_EVENT, IN_PROGRESS);
                    m_uiCageBreakTimer = 0;
                    break;
            }

            ++m_uiCageBreakStage;
        }
        else
            m_uiCageBreakTimer -= uiDiff;
    }

    // no yell if event is in progress or finished
    if (m_auiEncounter[TYPE_CHANNELER_EVENT] == IN_PROGRESS || m_auiEncounter[TYPE_MAGTHERIDON_EVENT] == DONE)
        return;

    if (m_uiRandYellTimer < uiDiff)
    {
        DoOrSimulateScriptTextForThisInstance(aRandomTaunt[urand(0, 5)], NPC_MAGTHERIDON);
        m_uiRandYellTimer = 90000;
    }
    else
        m_uiRandYellTimer -= uiDiff;
}

InstanceData* GetInstanceData_instance_magtheridons_lair(Map* pMap)
{
    return new instance_magtheridons_lair(pMap);
}

void AddSC_instance_magtheridons_lair()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_magtheridons_lair";
    pNewScript->GetInstanceData = &GetInstanceData_instance_magtheridons_lair;
    pNewScript->RegisterSelf();
}
