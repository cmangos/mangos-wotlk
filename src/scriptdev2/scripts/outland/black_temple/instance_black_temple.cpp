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
SDName: Instance_Black_Temple
SD%Complete: 100
SDComment: Instance Data Scripts and functions to acquire mobs and set encounter status for use in various Black Temple Scripts
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

/* Black Temple encounters:
0 - High Warlord Naj'entus event
1 - Supremus Event
2 - Shade of Akama Event
3 - Teron Gorefiend Event
4 - Gurtogg Bloodboil Event
5 - Reliquary Of Souls Event
6 - Mother Shahraz Event
7 - Illidari Council Event
8 - Illidan Stormrage Event
*/

instance_black_temple::instance_black_temple(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_black_temple::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_black_temple::OnPlayerEnter(Player* /*pPlayer*/)
{
    DoSpawnAkamaIfCan();
}

bool instance_black_temple::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_black_temple::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SPIRIT_OF_OLUM:
        case NPC_SPIRIT_OF_UDALO:
            // Use only the summoned versions
            if (!pCreature->IsTemporarySummon())
                break;
        case NPC_AKAMA:
        case NPC_ILLIDAN_STORMRAGE:
        case NPC_MAIEV_SHADOWSONG:
        case NPC_AKAMA_SHADE:
        case NPC_SHADE_OF_AKAMA:
        case NPC_RELIQUARY_OF_SOULS:
        case NPC_GATHIOS:
        case NPC_ZEREVOR:
        case NPC_LADY_MALANDE:
        case NPC_VERAS:
        case NPC_ILLIDARI_COUNCIL:
        case NPC_COUNCIL_VOICE:
        case NPC_ILLIDAN_DOOR_TRIGGER:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ASH_CHANNELER:
            m_lChannelersGuidList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_CREATURE_GENERATOR:
            m_vCreatureGeneratorGuidVector.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_GLAIVE_TARGET:
            m_vGlaiveTargetGuidVector.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_black_temple::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_NAJENTUS_GATE:                              // Gate past Naj'entus (at the entrance to Supermoose's courtyards)
            if (m_auiEncounter[TYPE_NAJENTUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SUPREMUS_DOORS:                             // Main Temple Doors - right past Supermoose (Supremus)
            if (m_auiEncounter[TYPE_SUPREMUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SHADE_OF_AKAMA:                             // Door close during encounter
        case GO_GOREFIEND_DOOR:                             // Door close during encounter
            break;
        case GO_GURTOGG_DOOR:                               // Door opens after encounter
            if (m_auiEncounter[TYPE_BLOODBOIL] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PRE_SHAHRAZ_DOOR:                           // Door leading to Mother Shahraz
            if (m_auiEncounter[TYPE_SHADE] == DONE && m_auiEncounter[TYPE_GOREFIEND] == DONE && m_auiEncounter[TYPE_BLOODBOIL] == DONE && m_auiEncounter[TYPE_RELIQUIARY] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_POST_SHAHRAZ_DOOR:                          // Door after shahraz
            if (m_auiEncounter[TYPE_SHAHRAZ] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PRE_COUNCIL_DOOR:                           // Door leading to the Council (grand promenade)
        case GO_COUNCIL_DOOR:                               // Door leading to the Council (inside)
        case GO_ILLIDAN_GATE:                               // Gate leading to Temple Summit
        case GO_ILLIDAN_DOOR_R:                             // Right door at Temple Summit
        case GO_ILLIDAN_DOOR_L:                             // Left door at Temple Summit
            break;

        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_black_temple::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_NAJENTUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_NAJENTUS_GATE);
            break;
        case TYPE_SUPREMUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SUPREMUS_DOORS);
            break;
        case TYPE_SHADE:
            m_auiEncounter[uiType] = uiData;
            // combat door
            DoUseDoorOrButton(GO_SHADE_OF_AKAMA);
            if (uiData == FAIL)
            {
                // Reset channelers on fail
                for (GuidList::const_iterator itr = m_lChannelersGuidList.begin(); itr != m_lChannelersGuidList.end(); ++itr)
                {
                    if (Creature* pChanneler = instance->GetCreature(*itr))
                    {
                        if (!pChanneler->isAlive())
                            pChanneler->Respawn();
                        else
                            pChanneler->AI()->EnterEvadeMode();
                    }
                }
            }
            if (uiData == DONE)
                DoOpenPreMotherDoor();
            break;
        case TYPE_GOREFIEND:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_GOREFIEND_DOOR);
            if (uiData == DONE)
                DoOpenPreMotherDoor();
            break;
        case TYPE_BLOODBOIL:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoOpenPreMotherDoor();
                DoUseDoorOrButton(GO_GURTOGG_DOOR);
            }
            break;
        case TYPE_RELIQUIARY:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoOpenPreMotherDoor();
            break;
        case TYPE_SHAHRAZ:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_POST_SHAHRAZ_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_COUNCIL:
            // Don't set the same data twice
            if (m_auiEncounter[uiType] == uiData)
                return;
            DoUseDoorOrButton(GO_COUNCIL_DOOR);
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoSpawnAkamaIfCan();
            break;
        case TYPE_ILLIDAN:
            DoUseDoorOrButton(GO_ILLIDAN_DOOR_R);
            DoUseDoorOrButton(GO_ILLIDAN_DOOR_L);
            if (uiData == FAIL)
            {
                // Cleanup encounter
                DoSpawnAkamaIfCan();
                DoUseDoorOrButton(GO_ILLIDAN_GATE);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            script_error_log("Instance Black Temple: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_black_temple::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_black_temple::DoOpenPreMotherDoor()
{
    if (GetData(TYPE_SHADE) == DONE && GetData(TYPE_GOREFIEND) == DONE && GetData(TYPE_BLOODBOIL) == DONE && GetData(TYPE_RELIQUIARY) == DONE)
        DoUseDoorOrButton(GO_PRE_SHAHRAZ_DOOR);
}

void instance_black_temple::DoSpawnAkamaIfCan()
{
    if (GetData(TYPE_ILLIDAN) == DONE || GetData(TYPE_COUNCIL) != DONE)
        return;

    // If already spawned return
    if (GetSingleCreatureFromStorage(NPC_AKAMA, true))
        return;

    // Summon Akama after the council has been defeated
    if (Player* pPlayer = GetPlayerInMap())
        pPlayer->SummonCreature(NPC_AKAMA, 617.754f, 307.768f, 271.735f, 6.197f, TEMPSUMMON_DEAD_DESPAWN, 0);
}

void instance_black_temple::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)            // Do not load an encounter as "In Progress" - reset it instead.
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_black_temple(Map* pMap)
{
    return new instance_black_temple(pMap);
}

void AddSC_instance_black_temple()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_black_temple";
    pNewScript->GetInstanceData = &GetInstanceData_instance_black_temple;
    pNewScript->RegisterSelf();
}
