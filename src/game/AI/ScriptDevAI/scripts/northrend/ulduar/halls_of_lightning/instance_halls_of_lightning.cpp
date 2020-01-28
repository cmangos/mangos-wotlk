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
SDName: Instance_Halls_of_Lightning
SD%Complete: 90%
SDComment: All ready.
SDCategory: Halls of Lightning
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_lightning.h"

/* Halls of Lightning encounters:
0 - General Bjarngrim
1 - Volkhan
2 - Ionar
3 - Loken
*/

instance_halls_of_lightning::instance_halls_of_lightning(Map* pMap) : ScriptedInstance(pMap),
    m_bLightningStruck(false),
    m_bIsShatterResistant(false)
{
    Initialize();
}

void instance_halls_of_lightning::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_halls_of_lightning::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_BJARNGRIM:
        case NPC_IONAR:
        case NPC_VOLKHAN_ANVIL:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_halls_of_lightning::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_VOLKHAN_DOOR:
            if (m_auiEncounter[TYPE_VOLKHAN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_IONAR_DOOR:
            if (m_auiEncounter[TYPE_IONAR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_LOKEN_THRONE:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_halls_of_lightning::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_BJARNGRIM:
            if (uiData == SPECIAL)
                m_bLightningStruck = true;
            else if (uiData == FAIL)
                m_bLightningStruck = false;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_VOLKHAN:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_VOLKHAN_DOOR);
            else if (uiData == IN_PROGRESS)
                m_bIsShatterResistant = true;
            else if (uiData == SPECIAL)
                m_bIsShatterResistant = false;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_IONAR:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_IONAR_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_LOKEN:
            if (uiData == IN_PROGRESS)
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_LOKEN_ID);
            else if (uiData == DONE)
            {
                if (GameObject* pGlobe = GetSingleGameObjectFromStorage(GO_LOKEN_THRONE))
                    pGlobe->SendGameObjectCustomAnim(pGlobe->GetObjectGuid());
            }
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_halls_of_lightning::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

bool instance_halls_of_lightning::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_LIGHTNING:
            return m_bLightningStruck;
        case ACHIEV_CRIT_RESISTANT:
            return m_bIsShatterResistant;
    }

    return false;
}

void instance_halls_of_lightning::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_halls_of_lightning(Map* pMap)
{
    return new instance_halls_of_lightning(pMap);
}

void AddSC_instance_halls_of_lightning()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_lightning";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_lightning;
    pNewScript->RegisterSelf();
}
