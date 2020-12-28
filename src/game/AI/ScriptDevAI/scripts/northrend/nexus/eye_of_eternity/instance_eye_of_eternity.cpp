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
SDName: instance_eye_of_eternity
SD%Complete: 50
SDComment:
SDCategory: Eye of Eternity
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "eye_of_eternity.h"

instance_eye_of_eternity::instance_eye_of_eternity(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_eye_of_eternity::Initialize()
{
    m_uiEncounter = NOT_STARTED;
}

bool instance_eye_of_eternity::IsEncounterInProgress() const
{
    return m_uiEncounter == IN_PROGRESS;
}

void instance_eye_of_eternity::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MALYGOS:
        case NPC_ALEXSTRASZA:
        case NPC_LARGE_TRIGGER:
        case NPC_ALEXSTRASZAS_GIFT:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_NEXUS_LORD:
        case NPC_SCION_OF_ETERNITY:
            m_lDiskRidersGuids.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_eye_of_eternity::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_EXIT_PORTAL:
        case GO_PLATFORM:
        case GO_FOCUSING_IRIS:
        case GO_FOCUSING_IRIS_H:
        case GO_HEART_OF_MAGIC:
        case GO_HEART_OF_MAGIC_H:
        case GO_ALEXSTRASZAS_GIFT:
        case GO_ALEXSTRASZAS_GIFT_H:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_eye_of_eternity::SetData(uint32 uiType, uint32 uiData)
{
    if (uiType != TYPE_MALYGOS)
        return;

    m_uiEncounter = uiData;
    if (uiData == IN_PROGRESS)
    {
        // Portal and iris despawn handled in DB
        DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_MALYGOS_ID);
    }
    else if (uiData == FAIL)
    {
        // respawn iris and portal
        DoRespawnGameObject(GO_EXIT_PORTAL, 7 * DAY);
        DoRespawnGameObject(instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? GO_FOCUSING_IRIS : GO_FOCUSING_IRIS_H, 7 * DAY);

        // rebuild platform
        if (Creature* pMalygos = GetSingleCreatureFromStorage(NPC_MALYGOS))
            if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
                pPlatform->RebuildGameObject(pMalygos);
    }
    else if (uiData == DONE)
    {
        // Note: ending event handled by DB

        // Spawn the Heart of Malygos
        DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_HEART_OF_MAGIC : GO_HEART_OF_MAGIC_H, 30 * MINUTE);
    }

    // Currently no reason to save anything
}

void instance_eye_of_eternity::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_NEXUS_LORD:
        case NPC_SCION_OF_ETERNITY:
            m_lDiskRidersGuids.remove(pCreature->GetObjectGuid());

            // start phase 3 if all adds are dead
            if (m_lDiskRidersGuids.empty())
                if (Creature* pMalygos = GetSingleCreatureFromStorage(NPC_MALYGOS))
                    pMalygos->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pMalygos, pMalygos);
            break;
    }
}

InstanceData* GetInstanceData_instance_eye_of_eternity(Map* pMap)
{
    return new instance_eye_of_eternity(pMap);
}

void AddSC_instance_eye_of_eternity()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_eye_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_eye_of_eternity;
    pNewScript->RegisterSelf();
}
