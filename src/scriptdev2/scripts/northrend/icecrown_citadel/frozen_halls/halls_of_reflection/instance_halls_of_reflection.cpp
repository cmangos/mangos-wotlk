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
SDName: instance_halls_of_reflection
SD%Complete: 10
SDComment: Basic support
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"

instance_halls_of_reflection::instance_halls_of_reflection(Map* pMap) : ScriptedInstance(pMap),
    m_uiTeam(TEAM_NONE)
{
    Initialize();
}

void instance_halls_of_reflection::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_halls_of_reflection::OnPlayerEnter(Player* pPlayer)
{
    if (!m_uiTeam)                                          // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();

        // Spawn intro npcs
        for (uint8 i = 0; i < countof(aEventBeginLocations); ++i)
        {
            pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocations[i].uiEntryHorde : aEventBeginLocations[i].uiEntryAlliance,
                aEventBeginLocations[i].fX, aEventBeginLocations[i].fY, aEventBeginLocations[i].fZ, aEventBeginLocations[i].fO, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }
}

void instance_halls_of_reflection::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_JAINA_PART1:
        case NPC_JAINA_PART2:
        case NPC_SYLVANAS_PART1:
        case NPC_SYLVANAS_PART2:
        case NPC_FALRIC:
        case NPC_MARWYN:
        case NPC_LICH_KING:
            break;
        default:
            return;
    }
    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_halls_of_reflection::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_IMPENETRABLE_DOOR:
            if (m_auiEncounter[TYPE_MARWYN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FROSTMOURNE:
        case GO_FROSTMOURNE_ALTAR:
        case GO_ICECROWN_DOOR_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_EXIT:
        case GO_CAVE_IN:

        case GO_CAPTAIN_CHEST_HORDE:
        case GO_CAPTAIN_CHEST_HORDE_H:
        case GO_CAPTAIN_CHEST_ALLIANCE:
        case GO_CAPTAIN_CHEST_ALLIANCE_H:
            break;
        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_halls_of_reflection::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_FALRIC:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MARWYN:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_IMPENETRABLE_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_LICH_KING:
            if (uiData == DONE)
            {
                uint32 uiChestEntry = m_uiTeam == ALLIANCE ? (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_ALLIANCE : GO_CAPTAIN_CHEST_ALLIANCE_H) :
                        (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_HORDE : GO_CAPTAIN_CHEST_HORDE_H);
                DoToggleGameObjectFlags(uiChestEntry, GO_FLAG_NO_INTERACT, false);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_halls_of_reflection::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_halls_of_reflection::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

InstanceData* GetInstanceData_instance_halls_of_reflection(Map* pMap)
{
    return new instance_halls_of_reflection(pMap);
}

void AddSC_instance_halls_of_reflection()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_reflection";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_reflection;
    pNewScript->RegisterSelf();
}
