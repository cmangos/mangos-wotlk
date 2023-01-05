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

enum GargoyleActions
{
    GARGOYLE_STONESKIN,
    GARGOYLE_ACID_VOLLEY,
    GARGOYLE_ACTION_MAX,
};

struct npc_stoneskin_gargoyleAI : public CombatAI
{
    npc_stoneskin_gargoyleAI(Creature* creature) : CombatAI(creature, GARGOYLE_ACTION_MAX)
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
        {
            return x > gargoyleResetCoords.x && y > gargoyleResetCoords.y && z > gargoyleResetCoords.z;
        });
        m_creature->SetForceAttackingCapability(true);
        AddTimerlessCombatAction(GARGOYLE_STONESKIN, true);
    }

    bool m_canCastVolley;

    void Reset() override
    {
        CombatAI::Reset();
        TryStoneForm();

        DoCastSpellIfCan(nullptr, SPELL_STEALTH_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void TryStoneForm()
    {
        if (m_creature->GetDefaultMovementType() == IDLE_MOTION_TYPE)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STONEFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT) == CAST_OK)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->SetImmuneToPlayer(true);
            }
        }
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        // All Stoneskin Gargoyles cast Acid Volley but the first one encountered
        float respawnX, respawnY, respawnZ;
        m_creature->GetRespawnCoord(respawnX, respawnY, respawnZ);
        if (m_creature->GetDefaultMovementType() == IDLE_MOTION_TYPE || respawnZ < gargoyleResetCoords.z)
            AddCombatAction(GARGOYLE_ACID_VOLLEY, 4000u);
    }

    void JustReachedHome() override
    {
        TryStoneForm();
    }

    void Aggro(Unit* /*enemy*/) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetImmuneToPlayer(false);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GARGOYLE_ACID_VOLLEY:
                if (DoCastSpellIfCan(m_creature, m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? SPELL_ACID_VOLLEY : SPELL_ACID_VOLLEY_25) == CAST_OK)
                    ResetCombatAction(action,8000);
                break;
            case GARGOYLE_STONESKIN:
                // Stoneskin at 30% HP
                if (m_creature->GetHealthPercent() < 30.0f && !m_creature->HasAura(SPELL_STONESKIN))
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_STONESKIN) == CAST_OK)
                        DoScriptText(SAY_GARGOYLE_NOISE, m_creature);
                }
                break;
        }
    }
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
    pNewScript = new Script;
    pNewScript->Name = "npc_stoneskin_gargoyle";
    pNewScript->GetAI = &GetNewAIInstance<npc_stoneskin_gargoyleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_poison";
    pNewScript->GetAI = &GetNewAIInstance<npc_living_poisonAI>;
    pNewScript->RegisterSelf();
}