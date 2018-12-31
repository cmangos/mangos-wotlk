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
SDName: Black_Temple
SD%Complete: 0
SDComment: Placeholder
SDCategory: Black Temple
EndScriptData */

/* ContentData
npc_greater_shadowfiend
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "black_temple.h"

/*######
## npc_greater_shadowfiend
######*/

#define SPELL_SHADOWFORM    34429

struct npc_greater_shadowfiend : public ScriptedAI
{
    npc_greater_shadowfiend(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SHADOWFORM);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            m_creature->AddThreat(pTarget, 100000.f);
    }
};

UnitAI* GetAI_npc_greater_shadowfiend(Creature* pCreature)
{
    return new npc_greater_shadowfiend(pCreature);
}

void AddSC_black_temple()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_greater_shadowfiend";
    pNewScript->GetAI = &GetAI_npc_greater_shadowfiend;
    pNewScript->RegisterSelf();
}
