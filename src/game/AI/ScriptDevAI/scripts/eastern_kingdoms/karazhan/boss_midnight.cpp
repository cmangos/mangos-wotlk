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
SDName: Boss_Midnight
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_MIDNIGHT_KILL            = 15334,
    SAY_APPEAR_1                 = 13459,
    SAY_APPEAR_2                 = 15378,
    SAY_APPEAR_3                 = 15379,
    SAY_MOUNT                    = 13456,
    SAY_KILL_1                   = 13460,
    SAY_KILL_2                   = 15333,
    SAY_DISARMED                 = 13490,
    SAY_DEATH                    = 13462,
    SAY_MIDNIGHT_CALL            = 13439,

    // Midnight					 
    SPELL_MOUNT                  = 29770,
    SPELL_KNOCKDOWN              = 29711,
    SPELL_SUMMON_ATTUMEN         = 29714,
    SPELL_SUMMON_ATTUMEN_MOUNTED = 29799,

    SPELL_SET_PHASE_1            = 1615101,
    SPELL_SET_PHASE_2            = 1615102,

    // Attumen
    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_UPPERCUT              = 29850,
    SPELL_CHARGE                = 29847,                    // Only when mounted

    // Extra
    SPELL_SPAWN_SMOKE_1         = 10389,
    SPELL_SPAWN_SMOKE_2         = 29802,
    
    NPC_ATTUMEN_MOUNTED         = 16152,
};

enum MidnightActions
{
    MIDNIGHT_ACTION_MAX,
};

struct boss_midnightAI : public CombatAI
{
    boss_midnightAI(Creature* creature) : CombatAI(creature, MIDNIGHT_ACTION_MAX), m_instance(static_cast<instance_karazhan*>(creature->GetInstanceData()))
    {        
        SetDeathPrevention(true);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
        {
            return (y < -1945.f && x > -11096.f) || z > 73.5f;
        });
    }

    instance_karazhan* m_instance;
    bool m_Phase2 = false;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        m_Phase2 = false;
        m_creature->SetSpellList(m_creature->GetCreatureInfo()->SpellList);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        if (!m_instance)
            return;

        if (Creature* pAttumen = m_instance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
        {
            if (pAttumen->IsInCombat())
            {
                pAttumen->AI()->EnterEvadeMode();
            }
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ATTUMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        if (m_Phase2 && m_instance)
        {
            if (Creature* pAttumen = m_instance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
                DoBroadcastText(SAY_MIDNIGHT_KILL, pAttumen);
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ATTUMEN, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (m_creature->GetVictim())
            summoned->AI()->AttackStart(m_creature->GetVictim());

        if (summoned->GetEntry() == NPC_ATTUMEN)
        {
            m_creature->SetSpellList(SPELL_SET_PHASE_2);
            m_Phase2 = true;
            DoBroadcastText(SAY_MIDNIGHT_CALL, m_creature);
            // Smoke effect
            summoned->CastSpell(summoned, SPELL_SPAWN_SMOKE_2, TRIGGERED_NONE);
            // Attumen yells when spawned
            switch (urand(0, 2))
            {
                case 0: DoBroadcastText(SAY_APPEAR_1, summoned); break;
                case 1: DoBroadcastText(SAY_APPEAR_2, summoned); break;
                case 2: DoBroadcastText(SAY_APPEAR_3, summoned); break;
            }
        }  
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || !pointId || !m_instance)
            return;

        // Spawn the mounted Attumen and despawn
        if (Creature* pAttumen = m_instance->GetSingleCreatureFromStorage(NPC_ATTUMEN))
        {
            if (pAttumen->AI()->DoCastSpellIfCan(nullptr, SPELL_SUMMON_ATTUMEN_MOUNTED, CAST_TRIGGERED) == CAST_OK)
            {
                m_creature->ForcedDespawn();
                pAttumen->ForcedDespawn();
            }
        }
    }

    // Wrapper to prepare phase 3
    void DoPrepareMount(Creature* target)
    {
        if (target)
        {
            SetCombatScriptStatus(true);
            SetCombatMovement(false);
            m_Phase2 = false;
            m_creature->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

            DoBroadcastText(SAY_MOUNT, target, m_creature);
        }
    }
};

enum AttumenActions
{
    ATTUMEN_MOUNT,
    ATTUMEN_ACTION_MAX,
    ATTUMEN_ATTACK_DELAY
};

struct boss_attumenAI : public CombatAI
{
    boss_attumenAI(Creature* creature) : CombatAI(creature, ATTUMEN_ACTION_MAX), m_instance(static_cast<instance_karazhan*>(creature->GetInstanceData()))
    {
        if (m_creature->GetEntry() != NPC_ATTUMEN_MOUNTED)
        {
            SetDeathPrevention(true);
            AddTimerlessCombatAction(ATTUMEN_MOUNT, true);
        }
        if (m_creature->GetEntry() == NPC_ATTUMEN_MOUNTED)
        {
            SetReactState(REACT_PASSIVE);
            AddCustomAction(ATTUMEN_ATTACK_DELAY, 2000u, [&]() { HandleAttackDelay(); });
        }            

        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
        {
            return (y < -1945.f && x > -11096.f) || z > 73.5f;
        });

        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
    }

    instance_karazhan* m_instance;

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ATTUMEN_MOUNT:
            {
                if (m_creature->GetHealthPercent() > 25.0f)
                    return;

                if (Creature* midnight = m_instance->GetSingleCreatureFromStorage(NPC_MIDNIGHT))
                {
                    midnight->CastSpell(m_creature, SPELL_MOUNT, TRIGGERED_OLD_TRIGGERED);
                    SetActionReadyStatus(action, false);
                }
                break;
            }
        }
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        if (Creature* midnight = m_instance->GetSingleCreatureFromStorage(NPC_MIDNIGHT))
        {
            if (midnight->IsInCombat())
            {
                midnight->AI()->EnterEvadeMode();
            }
        }
    }

    void SpellHit(Unit* /*source*/, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Mechanic == MECHANIC_DISARM)
            DoBroadcastText(SAY_DISARMED, m_creature);
    }

    void JustDied(Unit* /*victim*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ATTUMEN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ATTUMEN, FAIL);

        // Despawn Attumen on fail
        m_creature->ForcedDespawn();
    }
    
    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ATTUMEN_MOUNTED)
        {
            if (!m_instance)
                return;

            // Smoke effect
            summoned->CastSpell(nullptr, SPELL_SPAWN_SMOKE_1, TRIGGERED_NONE);

            // The summoned has the health equal to the one which has the higher HP percentage of both
            if (Creature* midnight = m_instance->GetSingleCreatureFromStorage(NPC_MIDNIGHT))
                summoned->SetHealth(midnight->GetHealth() > m_creature->GetHealth() ? midnight->GetHealth() : m_creature->GetHealth());
        }
    }

    void HandleAttackDelay()
    {
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
    }
};

struct MidnightMount : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            Unit* target = spell->GetUnitTarget();
            Unit* caster = spell->GetCaster();
            if (!target || !target->IsCreature() || !caster->IsCreature())
                return;

            // Avoid possible DB errors
            if (caster->GetEntry() == NPC_MIDNIGHT && target->GetEntry() == NPC_ATTUMEN)
            {
                // Prepare for mount
                if (boss_midnightAI* midnightAI = dynamic_cast<boss_midnightAI*>(caster->AI()))
                    midnightAI->DoPrepareMount(static_cast<Creature*>(target));

                if (boss_attumenAI* attumenAI = dynamic_cast<boss_attumenAI*>(target->AI()))
                    attumenAI->SetActionReadyStatus(ATTUMEN_MOUNT, false);
            }
        }
    }
};

void AddSC_boss_attumen()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_attumen";
    pNewScript->GetAI = &GetNewAIInstance<boss_attumenAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_midnight";
    pNewScript->GetAI = &GetNewAIInstance<boss_midnightAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<MidnightMount>("spell_midnight_mount");
}
