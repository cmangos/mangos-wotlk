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
SDName: Boss_Broggok
SD%Complete: 70
SDComment: pre-event not made
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "blood_furnace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = 14259,

    SAY_FEL_ORC_AGGRO               = 11999,

    SPELL_SUMMON_INCOMBAT_TRIGGER   = 26837,    // TODO: probably cast on spawn not sure what c.16006 does
    SPELL_SLIME_SPRAY               = 30913,
    SPELL_SLIME_SPRAY_H             = 38458,
    SPELL_POISON_CLOUD              = 30916,
    SPELL_POISON_BOLT               = 30917,
    SPELL_POISON_BOLT_H             = 38459,

    SPELL_POISON                    = 30914,
    SPELL_POISON_H                  = 38462,

    SPELL_STUN_SELF                 = 25900,
    SPELL_PACIFY_SELF               = 19951,

    POINT_EVENT_YELL                = 4,
    POINT_EVENT_COMBAT              = 7,
};

enum BroggokActions
{
    BROGGOK_ACTION_MAX,
};

struct boss_broggokAI : public CombatAI
{
    boss_broggokAI(Creature* creature) : CombatAI(creature, BROGGOK_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()){}

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BROGGOK_EVENT, FAIL);

        CombatAI::EnterEvadeMode();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_IN_COMBAT_TRIGGER)
        {
            summoned->GetCombatManager().SetLeashingDisable(true);
            summoned->AI()->SetReactState(REACT_DEFENSIVE);
            summoned->AI()->SetCombatMovement(false);
            summoned->AI()->SetMeleeEnabled(false);
            summoned->SetCanCallForAssistance(false);
            summoned->SetInCombatWithZone();
        }
        else
        {
            summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_STUN_SELF, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            summoned->AI()->DoCastSpellIfCan(nullptr, SPELL_PACIFY_SELF, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            summoned->AI()->DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_POISON : SPELL_POISON_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        }
    }

    void JustDied(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BROGGOK_EVENT, DONE);
    }

    // Reset Orientation
    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != WAYPOINT_MOTION_TYPE)
            return;

        if (pointId == POINT_EVENT_YELL) // verified its not actual aggro
            DoBroadcastText(SAY_AGGRO, m_creature);
        else if (pointId == POINT_EVENT_COMBAT)
        {
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
        }
    }
};

enum FelOrcAction
{
    FEL_ORC_ACTION_MAX,
};

struct npc_fel_orc : public CombatAI
{
    npc_fel_orc(Creature* creature) : CombatAI(creature, FEL_ORC_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())) {}

    ScriptedInstance* m_instance;

    void Aggro(Unit* /*who*/) override
    {
        if (urand(0, 4) > 2)
            DoBroadcastText(SAY_FEL_ORC_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        CombatAI::JustReachedHome();
         // Only Event Envolved NPCs should trigger fail condition
        if (m_creature->HasStringId(FIRST_BROGGOK_CELL_STRING) || m_creature->HasStringId(SECOND_BROGGOK_CELL_STRING) || m_creature->HasStringId(THIRD_BROGGOK_CELL_STRING) || m_creature->HasStringId(FOURTH_BROGGOK_CELL_STRING))
            if (m_instance->GetData(TYPE_BROGGOK_EVENT == IN_PROGRESS))
                m_instance->SetData(TYPE_BROGGOK_EVENT, FAIL);
    }
};

void AddSC_boss_broggok()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_broggok";
    pNewScript->GetAI = &GetNewAIInstance<boss_broggokAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fel_orc";
    pNewScript->GetAI = &GetNewAIInstance<npc_fel_orc>;
    pNewScript->RegisterSelf();
}
