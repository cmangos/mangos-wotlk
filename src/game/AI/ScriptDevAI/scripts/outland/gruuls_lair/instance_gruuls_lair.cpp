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
SDName: Instance_Gruuls_Lair
SD%Complete: 100
SDComment:
SDCategory: Gruul's Lair
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gruuls_lair.h"

/* Gruuls Lair encounters:
1 - High King Maulgar event
2 - Gruul event
*/

instance_gruuls_lair::instance_gruuls_lair(Map* pMap) : ScriptedInstance(pMap),
    m_uiCouncilMembersDied(0)
{
    Initialize();
}

void instance_gruuls_lair::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_gruuls_lair::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
        if (i == IN_PROGRESS)
            return true;

    return false;
}

void instance_gruuls_lair::OnCreatureCreate(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_MAULGAR)
        m_npcEntryGuidStore[NPC_MAULGAR] = pCreature->GetObjectGuid();
}

void instance_gruuls_lair::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_PORT_GRONN_1:
            if (m_auiEncounter[TYPE_MAULGAR_EVENT] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORT_GRONN_2:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_gruuls_lair::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_MAULGAR_EVENT:
            if (uiData == SPECIAL)
            {
                ++m_uiCouncilMembersDied;

                if (m_uiCouncilMembersDied == MAX_COUNCIL)
                    SetData(TYPE_MAULGAR_EVENT, DONE);
                // Don't store special data
                break;
            }
            if (uiData == FAIL)
                m_uiCouncilMembersDied = 0;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_PORT_GRONN_1);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GRUUL_EVENT:
            DoUseDoorOrButton(GO_PORT_GRONN_2);
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];

        m_strSaveData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_gruuls_lair::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_gruuls_lair::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);

    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];

    for (uint32& i : m_auiEncounter)
        if (i == IN_PROGRESS)
            i = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_gruuls_lair(Map* pMap)
{
    return new instance_gruuls_lair(pMap);
}

void AddSC_instance_gruuls_lair()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_gruuls_lair";
    pNewScript->GetInstanceData = &GetInstanceData_instance_gruuls_lair;
    pNewScript->RegisterSelf();
}
