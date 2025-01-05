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
SDName: Instance_Obsidian_Sanctum
SD%Complete: 80%
SDComment:
SDCategory: Obsidian Sanctum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "obsidian_sanctum.h"

/* Obsidian Sanctum encounters:
0 - Sartharion
*/

instance_obsidian_sanctum::instance_obsidian_sanctum(Map* map) : ScriptedInstance(map),
    m_aliveDragons(0)
{
    Initialize();
}

void instance_obsidian_sanctum::Initialize()
{
    memset(&m_encounter, 0, sizeof(m_encounter));

    for (bool& i : m_portalActive)
        i = false;
}

void instance_obsidian_sanctum::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        // The three dragons below set to active state once created.
        // We must expect bigger raid to encounter main boss, and then three dragons must be active due to grid differences
        case NPC_TENEBRON:
        case NPC_SHADRON:
        case NPC_VESPERON:
            creature->SetActiveObjectState(true);
        case NPC_SARTHARION:
        case NPC_VESPERON_CONTROLLER:
        case NPC_VESPERON_CONTROLLER_DEBUFF_CLEAR:
        case NPC_TENEBRON_EGG_CONTROLLER:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_FIRE_CYCLONE:
            m_fireCycloneGuidList.push_back(creature->GetObjectGuid());
            break;
    }
}

void instance_obsidian_sanctum::SetData(uint32 type, uint32 data)
{
    if (type == TYPE_SARTHARION_EVENT)
    {
        m_encounter[0] = data;
        if (data == IN_PROGRESS)
            m_volcanoBlowFailPlayers.clear();
    }
    else if (type == TYPE_ALIVE_DRAGONS)
        m_aliveDragons = data;
    else if (type == TYPE_VOLCANO_BLOW_FAILED)
    {
        // Insert the players who fail the achiev and haven't been already inserted in the set
        if (m_volcanoBlowFailPlayers.find(data) == m_volcanoBlowFailPlayers.end())
            m_volcanoBlowFailPlayers.insert(data);
    }

    // No need to save anything here
}

uint32 instance_obsidian_sanctum::GetData(uint32 type) const
{
    if (type == TYPE_SARTHARION_EVENT)
        return m_encounter[0];

    return 0;
}

ObjectGuid instance_obsidian_sanctum::SelectRandomFireCycloneGuid()
{
    if (m_fireCycloneGuidList.empty())
        return ObjectGuid();

    GuidList::iterator iter = m_fireCycloneGuidList.begin();
    advance(iter, urand(0, m_fireCycloneGuidList.size() - 1));

    return *iter;
}

bool instance_obsidian_sanctum::CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* /*target*/, uint32 /*miscValue1 = 0*/) const
{
    switch (criteriaId)
    {
        case ACHIEV_DRAGONS_ALIVE_1_N:
        case ACHIEV_DRAGONS_ALIVE_1_H:
            return m_aliveDragons >= 1;
        case ACHIEV_DRAGONS_ALIVE_2_N:
        case ACHIEV_DRAGONS_ALIVE_2_H:
            return m_aliveDragons >= 2;
        case ACHIEV_DRAGONS_ALIVE_3_N:
        case ACHIEV_DRAGONS_ALIVE_3_H:
            return m_aliveDragons >= 3;
        case ACHIEV_CRIT_VOLCANO_BLOW_N:
        case ACHIEV_CRIT_VOLCANO_BLOW_H:
            // Return true if not found in the set
            return m_volcanoBlowFailPlayers.find(source->GetGUIDLow()) == m_volcanoBlowFailPlayers.end();
        default:
            return false;
    }
}

bool instance_obsidian_sanctum::CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instanceConditionId)
    {
        case INSTANCE_CONDITION_ID_HARD_MODE:               // Exactly one dragon alive on event start
        case INSTANCE_CONDITION_ID_HARD_MODE_2:             // Exactly two dragons alive on event start
        case INSTANCE_CONDITION_ID_HARD_MODE_3:             // All three dragons alive on event start
            return m_aliveDragons == instanceConditionId;
    }

    script_error_log("instance_obsidian_sanctum::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
                     instanceConditionId, player ? player->GetGuidStr().c_str() : "NULL", conditionSource ? conditionSource->GetGuidStr().c_str() : "NULL", conditionSourceType);
    return false;
}

void AddSC_instance_obsidian_sanctum()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_obsidian_sanctum";
    pNewScript->GetInstanceData = GetNewInstanceScript<instance_obsidian_sanctum>;
    pNewScript->RegisterSelf();
}
