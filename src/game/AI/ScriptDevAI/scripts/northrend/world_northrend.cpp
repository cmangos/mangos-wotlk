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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_northrend.h"
#include "World/WorldStateDefines.h"
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"

world_map_northrend::world_map_northrend(Map* map) : ScriptedMap(map)
{
    Initialize();
}

void world_map_northrend::Initialize()
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_01, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_02, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_03, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_04, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_05, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_06, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_07, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_08, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_09, urand(0, 2));
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_10, urand(0, 2));
}

void world_map_northrend::OnCreatureGroupDespawn(CreatureGroup* creatureGroup, Creature* creature)
{
    switch (creatureGroup->GetGroupEntry().WorldStateCondition)
    {
    case 25000: case 25001: // TODO: Propagate respawn info so they do not instant respawn
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_01, urand(0, 1));
        break;
    case 25002: case 25003:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_02, urand(0, 1));
        break;
    case 25004: case 25005:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_03, urand(0, 1));
        break;
    case 25006: case 25007:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_04, urand(0, 1));
        break;
    case 25008: case 25009:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_05, urand(0, 1));
        break;
    case 25010: case 25011:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_06, urand(0, 1));
        break;
    case 25012: case 25013:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_07, urand(0, 1));
        break;
    case 25014: case 25015:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_08, urand(0, 1));
        break;
    case 25016: case 25017: case 25018:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_09, urand(0, 2));
        break;
    case 25019: case 25020: case 25021:
        instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_ICC_GRP_10, urand(0, 2));
        break;
    }
}

void world_map_northrend::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_NARGO_SCREWBORE:
        case NPC_HARROWMEISER:
        case NPC_DRENK_SPANNERSPARK:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
    }
}

void world_map_northrend::SetData(uint32 /*uiType*/, uint32 /*uiData*/) {}

void world_map_northrend::SetData64(uint32 data, uint64 value)
{
    switch (data)
    {
        case 11961: // Spirits Watch Over Us
            if (value) // 1 turn on weather
                instance->SetZoneWeather(0, 4040, 8, 1.f);
            else // 0 turn off weather
                instance->SetZoneWeather(0, 4040, 0, 0.f);
            break;
        case 12537: // Lightning Definitely Strikes Twice
            if (value) // 1 turn on weather
                instance->SetZoneWeather(0, 4306, 5, 1.f);
            else // 0 turn off weather
                instance->SetZoneWeather(0, 4306, 0, 0.f);
            break;
    }
}

void world_map_northrend::SetDalaranCooldownTime(ObjectGuid playerGuid)
{
    m_dalaranAreatriggerCooldown.emplace(playerGuid, instance->GetCurrentClockTime());
}

bool world_map_northrend::IsDalaranCooldownForPlayer(ObjectGuid playerGuid) const
{
    auto itr = m_dalaranAreatriggerCooldown.find(playerGuid);
    if (itr == m_dalaranAreatriggerCooldown.end())
        return false;
    return itr->second + std::chrono::minutes(5) > instance->GetCurrentClockTime();
}

void AddSC_world_northrend()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "world_map_northrend";
    pNewScript->GetInstanceData = &GetNewInstanceScript<world_map_northrend>;
    pNewScript->RegisterSelf();
}