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
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"

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