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
SDName: Instance_Underbog
SD%Complete:
SDComment:
SDCategory: Coilfang Resevoir, The Underbog
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "underbog.h"

instance_underbog::instance_underbog(Map* map) : ScriptedInstance(map), m_bGhazanSurfaced(false)
{
    Initialize();
}

void instance_underbog::Initialize()
{

}

void instance_underbog::SetData(uint32 type, uint32 data)
{
    if (type == DATA_GHAZAN_SURFACE)
        m_bGhazanSurfaced = data != 0;
}

uint32 instance_underbog::GetData(uint32 type) const
{
    if (type == DATA_GHAZAN_SURFACE)
        return uint32(m_bGhazanSurfaced);

    return 0;
}

void instance_underbog::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_UNDERBOG_FRENZY:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            break;
        case NPC_SWAMPLORD_MUSELEK:
        case NPC_CLAW:
        case NPC_GHAZAN:
        case NPC_BLACK_STALKER:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
    }
}

void instance_underbog::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_GHAZAN:
            for (ObjectGuid guid : m_npcEntryGuidCollection[NPC_UNDERBOG_FRENZY])
            {
                if (Creature* creature = instance->GetCreature(guid))
                {
                    creature->CastSpell(nullptr, SPELL_PERMANENT_FEIGH_DEATH, TRIGGERED_OLD_TRIGGERED);
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                }
            }
            break;
    }
}

/*######
## at_ghazan_surface
######*/

bool AreaTrigger_at_ghazan_surface(Player* player, AreaTriggerEntry const* /*pAt*/)
{
    ScriptedInstance* instance = (ScriptedInstance*)player->GetMap()->GetInstanceData();
    if (instance->GetData(DATA_GHAZAN_SURFACE) == 0)
    {
        if (Unit* ghazan = instance->GetSingleCreatureFromStorage(NPC_GHAZAN))
        {
            ghazan->GetMotionMaster()->Clear(false, true);
            ghazan->GetMotionMaster()->MoveWaypoint(1);
        }
        instance->SetData(DATA_GHAZAN_SURFACE, 1);
    }
    return true;
}

void AddSC_instance_underbog()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_underbog";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_underbog>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_ghazan_surface";
    pNewScript->pAreaTrigger = &AreaTrigger_at_ghazan_surface;
    pNewScript->RegisterSelf();
}

