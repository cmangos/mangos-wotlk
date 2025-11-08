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
SDName: instance_nexus
SD%Complete: 75%
SDComment:
SDCategory: The Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"

bool GOUse_go_containment_sphere(Player* /*player*/, GameObject* go)
{
    ScriptedInstance* instance = static_cast<ScriptedInstance*>(go->GetInstanceData());

    if (!instance)
        return false;

    switch (go->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA: instance->SetData(TYPE_TELESTRA, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS: instance->SetData(TYPE_ANOMALUS, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:  instance->SetData(TYPE_ORMOROK, SPECIAL);  break;
    }

    // disable the go and stop the breath casting
    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

    if (Creature* caster = GetClosestCreatureWithEntry(go, NPC_BREATH_CASTER, 20.0f))
        caster->InterruptNonMeleeSpells(false);

    return false;
}

instance_nexus::instance_nexus(Map* map) : ScriptedInstance(map)
{
    Initialize();

    for (bool& i : m_abAchievCriteria)
        i = false;
}

void instance_nexus::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_nexus::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA:
            if (m_auiEncounter[TYPE_TELESTRA] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS:
            if (m_auiEncounter[TYPE_ANOMALUS] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:
            if (m_auiEncounter[TYPE_ORMOROK] == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;

        default:
            return;
    }
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_nexus::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_ORMOROK:
        case NPC_KERISTRASZA:
        case NPC_ANOMALUS:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
    }
}

void instance_nexus::OnCreatureRespawn(Creature * creature)
{
    switch (creature->GetEntry())
    {
        case NPC_CRAZED_MANA_WRAITH:
            if (GetData(TYPE_ANOMALUS) == IN_PROGRESS)
            {
                if (Creature* anomalus = GetSingleCreatureFromStorage(NPC_ANOMALUS))
                {
                    if (anomalus->GetVictim() && creature->IsWithinDistInMap(anomalus, 50.0f))
                        creature->AI()->AttackStart(anomalus->GetVictim());
                }
            }
            break;
        // Passive behavior
        case NPC_CRYSTAL_SPIKE_INITIAL:
        case NPC_CRYSTAL_SPIKE_TRIGGER:
        case NPC_CRYSTAL_SPIKE:
            creature->AI()->SetReactState(REACT_PASSIVE);
            creature->SetCanEnterCombat(false);
            break;
    }
}

uint32 instance_nexus::GetData(uint32 type) const
{
    if (type < MAX_ENCOUNTER)
        return m_auiEncounter[type];

    return 0;
}

void instance_nexus::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_TELESTRA:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SPLIT_PERSONALITY, true);
            if (data == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_TELESTRA, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ANOMALUS:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_CHAOS_THEORY, true);
            if (data == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_ANOMALUS, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ORMOROK:
            m_auiEncounter[type] = data;
            if (data == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_ORMOROK, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_KERISTRASZA:
            m_auiEncounter[type] = data;
            if (data == IN_PROGRESS)
                m_sIntenseColdFailPlayers.clear();
            break;
        case TYPE_INTENSE_COLD_FAILED:
            // Insert the players who fail the achiev and haven't been already inserted in the set
            if (m_sIntenseColdFailPlayers.find(data) == m_sIntenseColdFailPlayers.end())
                m_sIntenseColdFailPlayers.insert(data);
            break;
        default:
            script_error_log("Instance Nexus: ERROR SetData = %u for type %u does not exist/not implemented.", type, data);
            return;
    }

    if (m_auiEncounter[TYPE_TELESTRA] == SPECIAL && m_auiEncounter[TYPE_ANOMALUS] == SPECIAL && m_auiEncounter[TYPE_ORMOROK] == SPECIAL && type != TYPE_KERISTRASZA)
    {
        // release Keristrasza from her prison here
        SetData(TYPE_KERISTRASZA, SPECIAL);

        Creature* creature = GetSingleCreatureFromStorage(NPC_KERISTRASZA);
        if (creature && creature->IsAlive())
        {
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            creature->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
        }
    }

    if (data == DONE || data == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_nexus::SetSpecialAchievementCriteria(uint32 type, bool isMet)
{
    if (type < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[type] = isMet;
}

bool instance_nexus::CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* /*target*/, uint32 /*miscValue1 = 0*/) const
{
    switch (criteriaId)
    {
        case ACHIEV_CRIT_CHAOS_THEORY:
            return m_abAchievCriteria[TYPE_ACHIEV_CHAOS_THEORY];
        case ACHIEV_CRIT_SPLIT_PERSONALITY:
            return m_abAchievCriteria[TYPE_ACHIEV_SPLIT_PERSONALITY];
        case ACHIEV_CRIT_INTENSE_COLD:
            // Return true if not found in the set
            return m_sIntenseColdFailPlayers.find(source->GetGUIDLow()) == m_sIntenseColdFailPlayers.end();

        default:
            return false;
    }
}

void instance_nexus::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void AddSC_instance_nexus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_nexus";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_nexus>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_containment_sphere";
    pNewScript->pGOUse = &GOUse_go_containment_sphere;
    pNewScript->RegisterSelf();
}
