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
SDName: Instance_Shadow_Labyrinth
SD%Complete: 85
SDComment: Some cleanup left along with save
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "precompiled.h"
#include "shadow_labyrinth.h"

/* Shadow Labyrinth encounters:
1 - Ambassador Hellmaw event
2 - Blackheart the Inciter event
3 - Grandmaster Vorpil event
4 - Murmur event
*/

instance_shadow_labyrinth::instance_shadow_labyrinth(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_shadow_labyrinth::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_shadow_labyrinth::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_REFECTORY_DOOR:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SCREAMING_HALL_DOOR:
            if (m_auiEncounter[3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;

        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_shadow_labyrinth::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_VORPIL:
        case NPC_HELLMAW:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_shadow_labyrinth::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_HELLMAW:
            m_auiEncounter[0] = uiData;
            break;

        case TYPE_INCITER:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_REFECTORY_DOOR);
            m_auiEncounter[1] = uiData;
            break;

        case TYPE_VORPIL:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SCREAMING_HALL_DOOR);
            m_auiEncounter[2] = uiData;
            break;

        case TYPE_MURMUR:
            m_auiEncounter[3] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                   << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_shadow_labyrinth::GetData(uint32 uiType) const
{
    switch (uiType)
    {
        case TYPE_HELLMAW:  return m_auiEncounter[0];
        case TYPE_INCITER:  return m_auiEncounter[1];
        case TYPE_VORPIL:   return m_auiEncounter[2];
        case TYPE_MURMUR:   return m_auiEncounter[3];

        default:
            return 0;
    }
}

void instance_shadow_labyrinth::SetData64(uint32 uiData, uint64 uiGuid)
{
    // If Hellmaw already completed, just ignore
    if (GetData(TYPE_HELLMAW) == DONE)
        return;

    // Note: this is handled in Acid. The purpose is check which Cabal Ritualists is alive, in case of server reset
    // The function is triggered by eventAI on generic timer
    if (uiData == DATA_CABAL_RITUALIST)
        m_sRitualistsAliveGUIDSet.insert(ObjectGuid(uiGuid));
}

void instance_shadow_labyrinth::OnCreatureDeath(Creature* pCreature)
{
    // unbanish Hellmaw when all Cabal Ritualists are dead
    if (pCreature->GetEntry() == NPC_CABAL_RITUALIST)
    {
        m_sRitualistsAliveGUIDSet.erase(pCreature->GetObjectGuid());

        if (m_sRitualistsAliveGUIDSet.empty())
        {
            if (Creature* pHellmaw = GetSingleCreatureFromStorage(NPC_HELLMAW))
            {
                // yell intro and remove banish aura
                DoScriptText(SAY_HELLMAW_INTRO, pHellmaw);
                pHellmaw->GetMotionMaster()->MoveWaypoint();
                pHellmaw->RemoveAurasDueToSpell(SPELL_BANISH);
            }
        }
    }
}

void instance_shadow_labyrinth::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_shadow_labyrinth(Map* pMap)
{
    return new instance_shadow_labyrinth(pMap);
}

void AddSC_instance_shadow_labyrinth()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_shadow_labyrinth";
    pNewScript->GetInstanceData = &GetInstanceData_instance_shadow_labyrinth;
    pNewScript->RegisterSelf();
}
