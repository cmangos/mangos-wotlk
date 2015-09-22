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
SDName: Instance - Sethekk Halls
SD%Complete: 60
SDComment: Summoning event for Anzu NYI
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "precompiled.h"
#include "sethekk_halls.h"

instance_sethekk_halls::instance_sethekk_halls(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}
void instance_sethekk_halls::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_sethekk_halls::OnCreatureCreate(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_ANZU)
        m_mNpcEntryGuidStore[NPC_ANZU] = pCreature->GetObjectGuid();
}

void instance_sethekk_halls::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_IKISS_DOOR:
            if (m_auiEncounter[TYPE_IKISS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_IKISS_CHEST:
            if (m_auiEncounter[TYPE_IKISS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT | GO_FLAG_INTERACT_COND);
            break;
        case GO_RAVENS_CLAW:
            break;

        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_sethekk_halls::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SYTH:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_ANZU:
            m_auiEncounter[uiType] = uiData;
            // Respawn the Raven's Claw if event fails
            if (uiData == FAIL)
            {
                if (GameObject* pClaw = GetSingleGameObjectFromStorage(GO_RAVENS_CLAW))
                    pClaw->Respawn();
            }
            break;
        case TYPE_IKISS:
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_IKISS_DOOR, DAY);
                DoToggleGameObjectFlags(GO_IKISS_CHEST, GO_FLAG_NO_INTERACT | GO_FLAG_INTERACT_COND, false);
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

uint32 instance_sethekk_halls::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_sethekk_halls::Load(const char* chrIn)
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

bool instance_sethekk_halls::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    if (uiCriteriaId != ACHIEV_CRITA_TURKEY_TIME)
        return false;

    if (!pSource)
        return false;

    return pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_HAT, 1) && (pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_DRESS, 1)
            || pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_ROBE, 1) || pSource->HasItemOrGemWithIdEquipped(ITEM_PILGRIMS_ATTIRE, 1));
}

InstanceData* GetInstanceData_instance_sethekk_halls(Map* pMap)
{
    return new instance_sethekk_halls(pMap);
}

bool ProcessEventId_event_spell_summon_raven_god(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_sethekk_halls* pInstance = (instance_sethekk_halls*)((Player*)pSource)->GetInstanceData())
        {
            // This should be checked by despawning the Raven Claw Go; However it's better to double check the condition
            if (pInstance->GetData(TYPE_ANZU) == DONE || pInstance->GetData(TYPE_ANZU) == IN_PROGRESS)
                return true;

            // Don't summon him twice
            if (pInstance->GetSingleCreatureFromStorage(NPC_ANZU, true))
                return true;

            // ToDo: add more code here to handle the summoning event. For the moment it's handled in DB because of the missing info
        }
    }
    return false;
}

void AddSC_instance_sethekk_halls()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_sethekk_halls";
    pNewScript->GetInstanceData = &GetInstanceData_instance_sethekk_halls;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_raven_god";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_raven_god;
    pNewScript->RegisterSelf();
}
