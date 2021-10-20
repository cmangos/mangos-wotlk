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
SDName: instance_oculus
SD%Complete: 50
SDComment: Spawn instance bosses and handle Varos pre event; Dialogue handled by DBScripts
SDCategory: Oculus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "oculus.h"

instance_oculus::instance_oculus(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_oculus::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_oculus::OnPlayerEnter(Player* pPlayer)
{
    if (GetData(TYPE_EREGOS) == DONE)
        return;

    DoSpawnNextBossIfCan();

    if (GetData(TYPE_DRAKOS) == DONE && GetData(TYPE_VAROS) == NOT_STARTED)
    {
        pPlayer->SendUpdateWorldState(WORLD_STATE_CONSTRUCTS, 1);
        pPlayer->SendUpdateWorldState(WORLD_STATE_CONSTRUCTS_COUNT, m_sConstructsAliveGUIDSet.size());
    }
}

void instance_oculus::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_VAROS:
        case NPC_UROM:
        case NPC_EREGOS:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_EMERALD_DRAKE:
        case NPC_AMBER_DRAKE:
        case NPC_RUBY_DRAKE:
            // after the 3rd encounter the vehicles receive additional spells
            if (GetData(TYPE_UROM) == DONE)
                pCreature->SetSpellList(pCreature->GetEntry() * 100 + 1);
            break;
    }
}

void instance_oculus::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CACHE_EREGOS:
        case GO_CACHE_EREGOS_H:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_DRAGON_CAGE_DOOR:
            m_lCageDoorGUIDs.push_back(pGo->GetObjectGuid());
            if (m_auiEncounter[TYPE_DRAKOS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }
}

void instance_oculus::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following creatures have a passive behavior
        case NPC_IMAGE_OF_BELGARISTRASZ:
        case NPC_CENTRIFUGE_CORE:
        case NPC_AZURE_RING_CAPTAIN:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
        break;
    }
}

void instance_oculus::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_DRAKOS:
            m_auiEncounter[TYPE_DRAKOS] = uiData;
            if (uiData == DONE)
            {
                // Open all cages
                for (GuidList::const_iterator itr = m_lCageDoorGUIDs.begin(); itr != m_lCageDoorGUIDs.end(); ++itr)
                    DoUseDoorOrButton(*itr);

                // Notes: The dialogue is handled by DB script
                // Also the Centrifuge Constructs and the related npcs should be summoned - requires additional research

                // Activate the world state - the Centrifuge contructs should be loaded by now
                DoUpdateWorldState(WORLD_STATE_CONSTRUCTS, 1);
                DoUpdateWorldState(WORLD_STATE_CONSTRUCTS_COUNT, m_sConstructsAliveGUIDSet.size());

                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_EREGOS_ID);
            }
            break;
        case TYPE_VAROS:
            m_auiEncounter[TYPE_VAROS] = uiData;
            if (uiData == DONE)
            {
                // Note: Image of Belgaristrasz dialogue is handled by DB script
                DoSpawnNextBossIfCan();
                DoUpdateWorldState(WORLD_STATE_CONSTRUCTS, 0);
            }
            break;
        case TYPE_UROM:
            m_auiEncounter[TYPE_UROM] = uiData;
            // Note: Image of Belgaristrasz dialogue is handled by DB script
            if (uiData == DONE)
                DoSpawnNextBossIfCan();
            break;
        case TYPE_EREGOS:
            m_auiEncounter[TYPE_EREGOS] = uiData;
            // Note: Image of Belgaristrasz teleports to the Cache location and does more dialogue - requires additional research
            if (uiData == DONE)
            {
                // The data about the cache isn't consistent, so it's better to handle both cases
                DoToggleGameObjectFlags(instance->IsRegularDifficulty() ? GO_CACHE_EREGOS : GO_CACHE_EREGOS_H, GO_FLAG_NO_INTERACT, false);
                DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_CACHE_EREGOS : GO_CACHE_EREGOS_H, 30 * MINUTE);
            }
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[TYPE_DRAKOS] << " " << m_auiEncounter[TYPE_VAROS] << " " << m_auiEncounter[TYPE_UROM] << " " << m_auiEncounter[TYPE_EREGOS];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_oculus::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_oculus::SetData64(uint32 uiData, uint64 uiGuid)
{
    // If Varos already completed, just ignore
    if (GetData(TYPE_VAROS) == DONE)
        return;

    // Note: this is handled in Acid. The purpose is check which Centrifuge Construct is alive, in case of server reset
    // The function is triggered by eventAI on generic timer
    if (uiData == DATA_CONSTRUCTS_EVENT)
    {
        m_sConstructsAliveGUIDSet.insert(ObjectGuid(uiGuid));

        // Update world state in case of server reset
        if (GetData(TYPE_DRAKOS) == DONE)
            DoUpdateWorldState(WORLD_STATE_CONSTRUCTS_COUNT, m_sConstructsAliveGUIDSet.size());
    }
}

void instance_oculus::OnCreatureEnterCombat(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_DRAKOS)
        SetData(TYPE_DRAKOS, IN_PROGRESS);
}

void instance_oculus::OnCreatureEvade(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_DRAKOS)
        SetData(TYPE_DRAKOS, FAIL);
}

void instance_oculus::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DRAKOS: SetData(TYPE_DRAKOS, DONE); break;
        case NPC_CENTRIFUGE_CONSTRUCT:
            m_sConstructsAliveGUIDSet.erase(pCreature->GetObjectGuid());
            DoUpdateWorldState(WORLD_STATE_CONSTRUCTS_COUNT, m_sConstructsAliveGUIDSet.size());

            if (m_sConstructsAliveGUIDSet.empty())
            {
                // force Varos to interrupt the channel
                if (Creature* pVaros = GetSingleCreatureFromStorage(NPC_VAROS))
                    pVaros->InterruptNonMeleeSpells(false);
            }
            break;
    }
}

void instance_oculus::DoSpawnNextBossIfCan()
{
    Player* pPlayer = GetPlayerInMap();
    if (!pPlayer)
        return;

    if (GetData(TYPE_UROM) == DONE)
    {
        // return if already summoned
        if (GetSingleCreatureFromStorage(NPC_EREGOS, true))
            return;

        pPlayer->SummonCreature(NPC_EREGOS, aOculusBossSpawnLocs[1][0], aOculusBossSpawnLocs[1][1], aOculusBossSpawnLocs[1][2], aOculusBossSpawnLocs[1][3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }
    else if (GetData(TYPE_VAROS) == DONE)
    {
        // return if already summoned
        if (GetSingleCreatureFromStorage(NPC_UROM, true))
            return;

        pPlayer->SummonCreature(NPC_UROM, aOculusBossSpawnLocs[0][0], aOculusBossSpawnLocs[0][1], aOculusBossSpawnLocs[0][2], aOculusBossSpawnLocs[0][3], TEMPSPAWN_DEAD_DESPAWN, 0);
    }
}

void instance_oculus::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[TYPE_DRAKOS] >> m_auiEncounter[TYPE_VAROS] >> m_auiEncounter[TYPE_UROM] >> m_auiEncounter[TYPE_EREGOS];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_oculus(Map* pMap)
{
    return new instance_oculus(pMap);
}

void AddSC_instance_oculus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_oculus";
    pNewScript->GetInstanceData = &GetInstanceData_instance_oculus;
    pNewScript->RegisterSelf();
}
