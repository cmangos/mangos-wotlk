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

bool GOUse_go_containment_sphere(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    switch (pGo->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA: pInstance->SetData(TYPE_TELESTRA, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS: pInstance->SetData(TYPE_ANOMALUS, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:  pInstance->SetData(TYPE_ORMOROK, SPECIAL);  break;
    }

    // disable the go and stop the breath casting
    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

    if (Creature* pCaster = GetClosestCreatureWithEntry(pGo, NPC_BREATH_CASTER, 20.0f))
        pCaster->InterruptNonMeleeSpells(false);

    return false;
}

instance_nexus::instance_nexus(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();

    for (bool& i : m_abAchievCriteria)
        i = false;
}

void instance_nexus::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_nexus::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA:
            if (m_auiEncounter[TYPE_TELESTRA] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS:
            if (m_auiEncounter[TYPE_ANOMALUS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:
            if (m_auiEncounter[TYPE_ORMOROK] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_nexus::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ORMOROK:
        case NPC_KERISTRASZA:
        case NPC_ANOMALUS:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_nexus::OnCreatureRespawn(Creature * pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CRAZED_MANA_WRAITH:
            if (GetData(TYPE_ANOMALUS) == IN_PROGRESS)
            {
                if (Creature* pAnomalus = GetSingleCreatureFromStorage(NPC_ANOMALUS))
                {
                    if (pAnomalus->GetVictim() && pCreature->IsWithinDistInMap(pAnomalus, 50.0f))
                        pCreature->AI()->AttackStart(pAnomalus->GetVictim());
                }
            }
            break;
        // Passive behavior
        case NPC_CRYSTAL_SPIKE_INITIAL:
        case NPC_CRYSTAL_SPIKE_TRIGGER:
        case NPC_CRYSTAL_SPIKE:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
    }
}

uint32 instance_nexus::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_nexus::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_TELESTRA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SPLIT_PERSONALITY, true);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_TELESTRA, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ANOMALUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_CHAOS_THEORY, true);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_ANOMALUS, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ORMOROK:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_CONTAINMENT_SPHERE_ORMOROK, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_KERISTRASZA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                m_sIntenseColdFailPlayers.clear();
            break;
        case TYPE_INTENSE_COLD_FAILED:
            // Insert the players who fail the achiev and haven't been already inserted in the set
            if (m_sIntenseColdFailPlayers.find(uiData) == m_sIntenseColdFailPlayers.end())
                m_sIntenseColdFailPlayers.insert(uiData);
            break;
        default:
            script_error_log("Instance Nexus: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (m_auiEncounter[TYPE_TELESTRA] == SPECIAL && m_auiEncounter[TYPE_ANOMALUS] == SPECIAL && m_auiEncounter[TYPE_ORMOROK] == SPECIAL && uiType != TYPE_KERISTRASZA)
    {
        // release Keristrasza from her prison here
        SetData(TYPE_KERISTRASZA, SPECIAL);

        Creature* pCreature = GetSingleCreatureFromStorage(NPC_KERISTRASZA);
        if (pCreature && pCreature->IsAlive())
        {
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            pCreature->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
        }
    }

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_nexus::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_nexus::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_CHAOS_THEORY:
            return m_abAchievCriteria[TYPE_ACHIEV_CHAOS_THEORY];
        case ACHIEV_CRIT_SPLIT_PERSONALITY:
            return m_abAchievCriteria[TYPE_ACHIEV_SPLIT_PERSONALITY];
        case ACHIEV_CRIT_INTENSE_COLD:
            // Return true if not found in the set
            return m_sIntenseColdFailPlayers.find(pSource->GetGUIDLow()) == m_sIntenseColdFailPlayers.end();

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

InstanceData* GetInstanceData_instance_nexus(Map* pMap)
{
    return new instance_nexus(pMap);
}

void AddSC_instance_nexus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_nexus";
    pNewScript->GetInstanceData = &GetInstanceData_instance_nexus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_containment_sphere";
    pNewScript->pGOUse = &GOUse_go_containment_sphere;
    pNewScript->RegisterSelf();
}
