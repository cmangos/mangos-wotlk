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
SDName: Instance - Sethekk Halls
SD%Complete: 60
SDComment: Summoning event for Anzu NYI
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "mana_tombs.h"
#include "World/WorldStateDefines.h"

instance_mana_tombs::instance_mana_tombs(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_mana_tombs::Initialize()
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_18, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_20, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_27, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_32, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_34, urand(0, 1));
    instance->GetVariableManager().SetVariable(WORLD_STATE_MANA_TOMBS_GROUP_35, urand(0, 1));
}


void AddSC_instance_mana_tombs()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_mana_tombs";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_mana_tombs>;
    pNewScript->RegisterSelf();
}
