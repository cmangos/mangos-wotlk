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
SDName: Instance_The_Eye
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "the_eye.h"

instance_the_eye::instance_the_eye(Map* pMap) : ScriptedInstance(pMap),
    m_uiKaelthasEventPhase(0)
{
    Initialize();
}

void instance_the_eye::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_the_eye::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_the_eye::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ALAR:
        case NPC_THALADRED:
        case NPC_TELONICUS:
        case NPC_CAPERNIAN:
        case NPC_SANGUINAR:
        case NPC_KAELTHAS:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_WORLD_TRIGGER_LARGE:
            m_npcEntryGuidCollection[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_the_eye::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_EMBER_OF_ALAR:
            if (Creature* alar = GetSingleCreatureFromStorage(NPC_ALAR))
                alar->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, creature, alar);
            break;
    }
}

void instance_the_eye::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ARCANE_DOOR_HORIZ_3:
        case GO_ARCANE_DOOR_HORIZ_4:
        case GO_KAEL_STATUE_LEFT:
        case GO_KAEL_STATUE_RIGHT:
        case GO_BRIDGE_WINDOW:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_RAID_DOOR_3:
        case GO_RAID_DOOR_4:
        case GO_ARCANE_DOOR_VERT_3:
        case GO_ARCANE_DOOR_VERT_4:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            if (CheckDoorOpening())
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }
}

void instance_the_eye::OnCreatureEvade(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_NETHERSTRAND_LONGBOW:
        case NPC_DEVASTATION:
        case NPC_COSMIC_INFUSER:
        case NPC_INFINITY_BLADES:
        case NPC_WARP_SLICER:
        case NPC_PHASESHIFT_BULWARK:
        case NPC_STAFF_OF_DISINTEGRATION:
            SetData(TYPE_KAELTHAS, FAIL);
            break;
    }
}

void instance_the_eye::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ALAR:
        case TYPE_SOLARIAN:
        case TYPE_VOIDREAVER:
            m_auiEncounter[uiType] = uiData;
#ifdef PRENERF_2_0_3
            if (CheckDoorOpening())
                OpenDoors();
#endif
            break;
        case TYPE_KAELTHAS:
            // Don't set the same data twice
            if (m_auiEncounter[uiType] == uiData)
                break;
            DoUseDoorOrButton(GO_ARCANE_DOOR_HORIZ_3);
            DoUseDoorOrButton(GO_ARCANE_DOOR_HORIZ_4);
            if (uiData == FAIL)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_KAEL_STATUE_LEFT))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_KAEL_STATUE_RIGHT))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_BRIDGE_WINDOW))
                    pGo->ResetDoorOrButton();

                // despawn the advisors
                for (unsigned int aAdvisor : aAdvisors)
                    if (Creature* add = GetSingleCreatureFromStorage(aAdvisor))
                        add->ForcedDespawn();

                if (Creature* kael = GetSingleCreatureFromStorage(NPC_KAELTHAS))
                {
                    kael->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, kael, kael);
                    kael->SetRespawnDelay(30, true);
                    kael->ForcedDespawn();
                }
            }
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
            << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_the_eye::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[TYPE_ALAR] >> m_auiEncounter[TYPE_SOLARIAN] >> m_auiEncounter[TYPE_VOIDREAVER] >> m_auiEncounter[TYPE_KAELTHAS];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_the_eye::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

bool instance_the_eye::CheckDoorOpening() const
{
    return m_auiEncounter[TYPE_ALAR] == DONE && m_auiEncounter[TYPE_SOLARIAN] == DONE && m_auiEncounter[TYPE_VOIDREAVER] == DONE;
}

void instance_the_eye::OpenDoors()
{
    DoUseOpenableObject(GO_RAID_DOOR_3, true);
    DoUseOpenableObject(GO_RAID_DOOR_4, true);
    DoUseOpenableObject(GO_ARCANE_DOOR_VERT_3, true);
    DoUseOpenableObject(GO_ARCANE_DOOR_VERT_4, true);
}

struct CounterCharge : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        if (Unit* target = caster->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, 35039, SELECT_FLAG_PLAYER | SELECT_FLAG_CASTING))
        {
            caster->CastSpell(target, 35039, TRIGGERED_OLD_TRIGGERED);
            caster->RemoveAurasDueToSpell(35035);
        }
        spell->SetTriggerChance(0, EFFECT_INDEX_1);
    }
};

void AddSC_instance_the_eye()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_the_eye";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_the_eye>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<CounterCharge>("spell_countercharge");
}
