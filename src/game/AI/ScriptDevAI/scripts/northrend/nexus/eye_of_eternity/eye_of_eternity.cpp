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
#include "Maps/TransportSystem.h"

instance_eye_of_eternity::instance_eye_of_eternity(Map* pMap) : ScriptedInstance(pMap),
    m_uiMalygosResetTimer(0),
    m_uiMalygosCompleteTimer(0)
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
        case NPC_ALEXSTRASZA_INVIS:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_NEXUS_LORD:
        case NPC_SCION_OF_ETERNITY:
            m_lDiskRidersGuids.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_HOVER_DISK_LORD:
        case NPC_HOVER_DISK_SCION:
        case NPC_ARCANE_OVERLOAD:
            m_lSecondPhaseCreaturesGuids.push_back(pCreature->GetObjectGuid());
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

void instance_eye_of_eternity::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following creatures have a passive behavior
        case NPC_SURGE_OF_POWER:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
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
        {
            if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
            {
                pPlatform->RebuildGameObject(pMalygos);
                pPlatform->SetGoState(GO_STATE_READY);
                instance->ChangeGOPathfinding(194232, 8546, true);
            }

            // despawn and respawn boss
            pMalygos->ForcedDespawn(5000);
            m_uiMalygosResetTimer = 30000;
        }
    }
    else if (uiData == DONE)
    {
        // Note: ending event handled by DB

        // Spawn the Heart of Malygos immediately after death
        DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_HEART_OF_MAGIC : GO_HEART_OF_MAGIC_H, 24 * HOUR);

        // respawn the other loot on timer
        m_uiMalygosCompleteTimer = 18000;
    }

    // Currently no reason to save anything
}

void instance_eye_of_eternity::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_NEXUS_LORD:
        case NPC_SCION_OF_ETERNITY:
            // disk vehicles fall down once passenger is killed
            if (pCreature->GetTransportInfo() && pCreature->GetTransportInfo()->IsOnVehicle())
            {
                if (Unit* pVehicle = static_cast<Unit*>(pCreature->GetTransportInfo()->GetTransport()))
                {
                    pVehicle->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, pCreature->GetObjectGuid());
                    pVehicle->GetMotionMaster()->Clear(false, true);
                    pVehicle->SetLevitate(false);
                    pVehicle->GetMotionMaster()->MoveFall();
                }
            }

            // remove passenger from riders list
            m_lDiskRidersGuids.remove(pCreature->GetObjectGuid());

            // start phase 3 if all adds are dead
            if (m_lDiskRidersGuids.empty())
            {
                if (Creature* pMalygos = GetSingleCreatureFromStorage(NPC_MALYGOS))
                    pMalygos->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pMalygos, pMalygos);

                // despawn all disks and arcane bombs before phase 3
                for (const auto& guid : m_lSecondPhaseCreaturesGuids)
                    if (Creature* pCreature = instance->GetCreature(guid))
                        pCreature->ForcedDespawn();
            }
            break;
    }
}

void instance_eye_of_eternity::Update(uint32 uiDiff)
{
    // Respawn Malygos
    if (m_uiMalygosResetTimer)
    {
        if (m_uiMalygosResetTimer <= uiDiff)
        {
            if (Creature* pMalygos = GetSingleCreatureFromStorage(NPC_MALYGOS))
                pMalygos->Respawn();

            m_uiMalygosResetTimer = 0;
        }
        else
            m_uiMalygosResetTimer -= uiDiff;
    }

    // respawn loot and portal
    if (m_uiMalygosCompleteTimer)
    {
        if (m_uiMalygosCompleteTimer <= uiDiff)
        {
            DoRespawnGameObject(GO_EXIT_PORTAL, 24 * HOUR);
            DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_ALEXSTRASZAS_GIFT : GO_ALEXSTRASZAS_GIFT_H, 24 * HOUR);

            m_uiMalygosCompleteTimer = 0;
        }
        else
            m_uiMalygosCompleteTimer -= uiDiff;
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
