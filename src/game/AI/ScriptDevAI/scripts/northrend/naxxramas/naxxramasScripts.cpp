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
 SDName: icecrown_citadel
 SD%Complete: 50%
 SDComment: Teleporters, Light's Hammer ATs, Putricide's trap.
 SDCategory: Icecrown Citadel
 EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/northrend/naxxramas/naxxramas.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

static const Position gargoyleResetCoords = { 2963.f, -3476.f, 297.6f };

enum
{
    SAY_GARGOYLE_NOISE      = -1533160, // %s emits a strange noise.

    SPELL_STONEFORM         = 29154,
    SPELL_STEALTH_DETECTION = 18950,
    SPELL_STONESKIN         = 28995,
    SPELL_ACID_VOLLEY       = 29325,
    SPELL_ACID_VOLLEY_25    = 54714,

    SPELL_EXPLODE           = 28433, // Used by Living Poison blobs when players come in range
};

/*###################
#   npc_living_poison
###################*/

struct npc_living_poisonAI : public ScriptedAI
{
    npc_living_poisonAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        SetMeleeEnabled(false);
        SetCombatMovement(false);
    }

    void Reset() override
    {
    }

    // Any time a player comes close to the Living Poison, it will explode and kill itself while doing heavy AoE damage to the player
    void MoveInLineOfSight(Unit* who) override
    {
        if (!who->IsPlayer() || m_creature->GetDistance2d(who->GetPositionX(), who->GetPositionY(), DIST_CALC_BOUNDING_RADIUS) > 4.0f)
            return;

        DoCastSpellIfCan(nullptr, SPELL_EXPLODE, CAST_TRIGGERED);
    }
};

void AddSC_naxxramas()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_living_poison";
    pNewScript->GetAI = &GetNewAIInstance<npc_living_poisonAI>;
    pNewScript->RegisterSelf();
}