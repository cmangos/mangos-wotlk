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

#include "precompiled.h"
#include "eye_of_eternity.h"

static const DialogueEntry aEpilogueDialogue[] =
{
    {NPC_ALEXSTRASZA,               0,                  10000},
    {SPELL_ALEXSTRASZAS_GIFT_BEAM,  0,                  3000},
    {NPC_ALEXSTRASZAS_GIFT,         0,                  2000},
    {SAY_OUTRO_1,                   NPC_ALEXSTRASZA,    6000},
    {SAY_OUTRO_2,                   NPC_ALEXSTRASZA,    4000},
    {SAY_OUTRO_3,                   NPC_ALEXSTRASZA,    23000},
    {SAY_OUTRO_4,                   NPC_ALEXSTRASZA,    20000},
    {GO_PLATFORM,                   0,                  0},
    {0, 0, 0},
};

instance_eye_of_eternity::instance_eye_of_eternity(Map* pMap) : ScriptedInstance(pMap),
    DialogueHelper(aEpilogueDialogue)
{
    Initialize();
}

void instance_eye_of_eternity::Initialize()
{
    m_uiEncounter = NOT_STARTED;
    InitializeDialogueHelper(this);
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
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
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
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
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
        // ToDo: Despawn the exit portal

        DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_MALYGOS_ID);
    }
    else if (uiData == FAIL)
    {
        // ToDo: respawn the focus iris and the portal

        if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
            pPlatform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK_11);
    }
    else if (uiData == DONE)
        StartNextDialogueText(NPC_ALEXSTRASZA);

    // Currently no reason to save anything
}

void instance_eye_of_eternity::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case SPELL_ALEXSTRASZAS_GIFT_BEAM:
            if (Creature* pAlextrasza = GetSingleCreatureFromStorage(NPC_ALEXSTRASZA))
                pAlextrasza->CastSpell(pAlextrasza, SPELL_ALEXSTRASZAS_GIFT_BEAM, false);
            break;
        case NPC_ALEXSTRASZAS_GIFT:
            if (Creature* pGift = GetSingleCreatureFromStorage(NPC_ALEXSTRASZAS_GIFT))
                pGift->CastSpell(pGift, SPELL_ALEXSTRASZAS_GIFT_VISUAL, false);
            DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_ALEXSTRASZAS_GIFT : GO_ALEXSTRASZAS_GIFT_H, 30 * MINUTE);
            break;
        case GO_PLATFORM:
            // ToDo: respawn the portal
            if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
                pPlatform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK_11);
            // Spawn the Heart of Malygos
            DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_HEART_OF_MAGIC : GO_HEART_OF_MAGIC_H, 30 * MINUTE);
            break;
    }
}

InstanceData* GetInstanceData_instance_eye_of_eternity(Map* pMap)
{
    return new instance_eye_of_eternity(pMap);
}

void AddSC_instance_eye_of_eternity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_eye_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_eye_of_eternity;
    pNewScript->RegisterSelf();
}
