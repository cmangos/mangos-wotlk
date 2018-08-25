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
SDName: Instance_Mechanar
SD%Complete: 70
SDComment: Elevator needs core support
SDCategory: Mechanar
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "mechanar.h"

instance_mechanar::instance_mechanar(Map* pMap) : ScriptedInstance(pMap),
    m_uiBridgeEventTimer(0),
    m_uiBridgeEventPhase(0)
{
    Initialize();
}

void instance_mechanar::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_mechanar::OnPlayerEnter(Player* pPlayer)
{
    // Check encounter states
    if (GetData(TYPE_SEPETHREA) != DONE || GetData(TYPE_PATHALEON) == DONE)
        return;

    // Check if already summoned
    if (GetSingleCreatureFromStorage(NPC_PATHALEON, true))
        return;

    pPlayer->SummonCreature(aBridgeEventLocs[6][0].m_uiSpawnEntry, aBridgeEventLocs[6][0].m_fX, aBridgeEventLocs[6][0].m_fY, aBridgeEventLocs[6][0].m_fZ, aBridgeEventLocs[6][0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
}

void instance_mechanar::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_PATHALEON:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ASTROMAGE:
        case NPC_PHYSICIAN:
        case NPC_CENTURION:
        case NPC_ENGINEER:
        case NPC_NETHERBINDER:
        case NPC_FORGE_DESTROYER:
            if (pCreature->IsTemporarySummon())
                m_sBridgeTrashGuidSet.insert(pCreature->GetObjectGuid());
            break;
    }
}

void instance_mechanar::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_MOARG_DOOR_1:
            if (m_auiEncounter[TYPE_GYRO_KILL] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_MOARG_DOOR_2:
            if (m_auiEncounter[TYPE_IRON_HAND] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_NETHERMANCER_DOOR:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_mechanar::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_GYRO_KILL:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_MOARG_DOOR_1);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_IRON_HAND:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_MOARG_DOOR_2);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_CAPACITUS:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SEPETHREA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                m_uiBridgeEventTimer = 10000;
            DoUseDoorOrButton(GO_NETHERMANCER_DOOR);
            break;
        case TYPE_PATHALEON:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3]  << " " << m_auiEncounter[4];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_mechanar::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_mechanar::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4];

    for (unsigned int& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_mechanar::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_GYRO_KILL:      SetData(TYPE_GYRO_KILL, DONE); break;
        case NPC_IRON_HAND:      SetData(TYPE_IRON_HAND, DONE); break;
        case NPC_LORD_CAPACITUS: SetData(TYPE_CAPACITUS, DONE); break;

        case NPC_ASTROMAGE:
        case NPC_PHYSICIAN:
        case NPC_CENTURION:
        case NPC_ENGINEER:
        case NPC_NETHERBINDER:
        case NPC_FORGE_DESTROYER:
            if (m_sBridgeTrashGuidSet.find(pCreature->GetObjectGuid()) != m_sBridgeTrashGuidSet.end())
            {
                m_sBridgeTrashGuidSet.erase(pCreature->GetObjectGuid());

                if (m_sBridgeTrashGuidSet.empty())
                {
                    // After the 3rd wave wait 10 seconds
                    if (m_uiBridgeEventPhase == 3)
                        m_uiBridgeEventTimer = 10000;
                    else
                        DoSpawnBridgeWave();
                }
            }
            break;
    }
}

void instance_mechanar::DoSpawnBridgeWave()
{
    if (Player* pPlayer = GetPlayerInMap(true, false))
    {
        for (uint8 i = 0; i < MAX_BRIDGE_TRASH; ++i)
        {
            // Skip the blank entries
            if (aBridgeEventLocs[m_uiBridgeEventPhase][i].m_uiSpawnEntry == 0)
                break;

            if (Creature* pTemp = pPlayer->SummonCreature(aBridgeEventLocs[m_uiBridgeEventPhase][i].m_uiSpawnEntry, aBridgeEventLocs[m_uiBridgeEventPhase][i].m_fX, aBridgeEventLocs[m_uiBridgeEventPhase][i].m_fY, aBridgeEventLocs[m_uiBridgeEventPhase][i].m_fZ, aBridgeEventLocs[m_uiBridgeEventPhase][i].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0))
            {
                pTemp->CastSpell(pTemp, SPELL_ETHEREAL_TELEPORT, TRIGGERED_NONE);

                switch (m_uiBridgeEventPhase)
                {
                    case 1:                                 // These waves should attack the player directly
                    case 2:
                    case 4:
                    case 5:
                        pTemp->AI()->AttackStart(pPlayer);
                        break;
                    case 6:                                 // Pathaleon
                        DoScriptText(SAY_PATHALEON_INTRO, pTemp);
                        break;
                }
            }
        }
    }
    ++m_uiBridgeEventPhase;
}

void instance_mechanar::Update(uint32 uiDiff)
{
    if (m_uiBridgeEventTimer)
    {
        if (m_uiBridgeEventTimer <= uiDiff)
        {
            DoSpawnBridgeWave();
            m_uiBridgeEventTimer = 0;
        }
        else
            m_uiBridgeEventTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_mechanar(Map* pMap)
{
    return new instance_mechanar(pMap);
}

void AddSC_instance_mechanar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_mechanar";
    pNewScript->GetInstanceData = &GetInstanceData_instance_mechanar;
    pNewScript->RegisterSelf();
}
