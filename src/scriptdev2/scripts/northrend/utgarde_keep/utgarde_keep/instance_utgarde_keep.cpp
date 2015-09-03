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
SDName: Instance_Utgarde_Keep
SD%Complete: 20%
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

#include "precompiled.h"
#include "utgarde_keep.h"

instance_utgarde_keep::instance_utgarde_keep(Map* pMap) : ScriptedInstance(pMap),
    m_bKelesethAchievFailed(false)
{
    Initialize();
}

void instance_utgarde_keep::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_utgarde_keep::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KELESETH:
        case NPC_SKARVALD:
        case NPC_DALRONN:
        case NPC_INGVAR:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_utgarde_keep::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_BELLOW_1:
            if (m_auiEncounter[TYPE_BELLOW_1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_BELLOW_2:
            if (m_auiEncounter[TYPE_BELLOW_2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_BELLOW_3:
            if (m_auiEncounter[TYPE_BELLOW_3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_1:
            if (m_auiEncounter[TYPE_BELLOW_1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_2:
            if (m_auiEncounter[TYPE_BELLOW_2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_3:
            if (m_auiEncounter[TYPE_BELLOW_3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORTCULLIS_EXIT_1:
        case GO_PORTCULLIS_EXIT_2:
            if (m_auiEncounter[TYPE_INGVAR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORTCULLIS_COMBAT:
            break;

        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_utgarde_keep::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_FROST_TOMB)
        m_bKelesethAchievFailed = true;
}

void instance_utgarde_keep::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_KELESETH:
            if (uiData == IN_PROGRESS)
                m_bKelesethAchievFailed = false;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SKARVALD_DALRONN:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_INGVAR:
            if (m_auiEncounter[uiType] == uiData)
                return;
            DoUseDoorOrButton(GO_PORTCULLIS_COMBAT);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_PORTCULLIS_EXIT_1);
                DoUseDoorOrButton(GO_PORTCULLIS_EXIT_2);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_1:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_2:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_3:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_utgarde_keep::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_utgarde_keep::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

bool instance_utgarde_keep::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    if (uiCriteriaId == ACHIEV_CRIT_ON_THE_ROCKS)
        return !m_bKelesethAchievFailed;

    return false;
}

InstanceData* GetInstanceData_instance_utgarde_keep(Map* pMap)
{
    return new instance_utgarde_keep(pMap);
}

void AddSC_instance_utgarde_keep()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_utgarde_keep";
    pNewScript->GetInstanceData = GetInstanceData_instance_utgarde_keep;
    pNewScript->RegisterSelf();
}
