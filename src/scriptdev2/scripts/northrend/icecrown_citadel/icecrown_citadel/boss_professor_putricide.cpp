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
SDName: boss_professor_putricide
SD%Complete: 70%
SDComment: NYI: Abomination and table handling, Malleable Goo,
           possibly Green Ooze and Orange Gas scripts require handling in sd2, but need further research on their spells
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631092,
    SAY_AIRLOCK                 = -1631093,
    SAY_PHASE_CHANGE            = -1631094,
    SAY_TRANSFORM_1             = -1631095,
    SAY_TRANSFORM_2             = -1631096,
    SAY_SLAY_1                  = -1631097,
    SAY_SLAY_2                  = -1631098,
    SAY_BERSERK                 = -1631099,
    SAY_DEATH                   = -1631100,
};

enum
{
    SPELL_BERSERK                   = 47008,

    // controlled abomination
    SPELL_MUTATED_TRANSFORMATION    = 70308,
    SPELL_EAT_OOZE                  = 72527,
    SPELL_REGURGITATED_OOZE         = 70539,
    SPELL_MUTATED_SLASH             = 70542,
    SPELL_MUTATED_AURA              = 70405,
    SPELL_ABOMINATION_POWER_DRAIN   = 70385, // prevents normal regen of abomination's power

    SPELL_UNSTABLE_EXPERIMENT       = 70351, // ooze and gas summoning spells in basepoints of effects of this spell suggest that they should be handled in core

    // Volatile Experiment on heroic difficulties
    SPELL_VOLATILE_EXPERIMENT       = 72840, // single target dummy effect
    SPELL_VOLATILE_EXPERIMENT_2     = 72841, // single target dummy effect
    SPELL_VOLATILE_EXPERIMENT_3     = 72842, // radius target dummy effect
    SPELL_VOLATILE_EXPERIMENT_4     = 72843, // radius target dummy effect

    SPELL_GREEN_OOZE_SUMMON         = 71412,
    SPELL_ORANGE_OOZE_SUMMON        = 71415,

    SPELL_OOZE_ADHESIVE             = 70447,
    SPELL_OOZE_ERUPTION             = 70492,

    SPELL_GASEOUS_BLOAT             = 70672,
    SPELL_EXPUNGED_GAS              = 70701,
    SPELL_GASEOUS_BLOAT_VISUAL      = 70215,

    SPELL_SLIME_PUDDLE              = 70341,
    SPELL_SLIME_PUDDLE_SUMMON       = 70342,
    SPELL_SLIME_PUDDLE_AURA         = 70343,
// SPELL_SLIME_PUDDLE_TRIGGER      = 71424, // trigger summon spell from target?
// SPELL_SLIME_PUDDLE_SUMMON_TRIG  = 71425,
    SPELL_GROW_STACKER              = 70345,
    SPELL_GROW_STACKER_GROW_AURA    = 70347,

    SPELL_MALLEABLE_GOO_MISSILE     = 70852,

    SPELL_CHOKING_GAS_BOMB          = 71255,
    SPELL_CHOKING_GAS_BOMB_AURA     = 71259,
    SPELL_CHOKING_GAS_BOMB_EXPL_AUR = 71280,
    SPELL_CHOKING_GAS_EXPLOSION     = 71279,

    // phase transitions
    SPELL_TEAR_GAS                  = 71617, // stuns players
    SPELL_TEAR_GAS_PERIODIC_AURA    = 73170, // stuns summoned creatures?
    SPELL_TEAR_GAS_CANCEL           = 71620,

    SPELL_CREATE_CONCOCTION         = 71621,
    SPELL_GUZZLE_POTIONS            = 71893,

    SPELL_MUTATED_PLAGUE            = 72451,

    // heroic
    SPELL_UNBOUND_PLAGUE            = 70911,
    SPELL_OOZE_VARIABLE             = 70352, // aura 303 - dont allow taking damage from attacker with linked aura303?
    SPELL_OOZE_VARIABLE_OOZE        = 74118, // anyway, implemented as hardcoded in script
    SPELL_GAS_VARIABLE              = 70353,
    SPELL_GAS_VARIABLE_GAS          = 74119,

    SPELL_OOZE_TANK_PROTECTION      = 71770
};

enum Phase
{
    PHASE_ONE               = 1,
    PHASE_RUNNING_ONE       = 2,
    PHASE_TRANSITION_ONE    = 3,
    PHASE_TWO               = 4,
    PHASE_RUNNING_TWO       = 5,
    PHASE_TRANSITION_TWO    = 6,
    PHASE_THREE             = 7
};

enum Waypoint
{
    POINT_PUTRICIDE_SPAWN = 1
};

static const float fPutricidePosition[1][3] =
{
    {4356.78f, 3263.51f, 389.40f}   // 0 Putricide spawn point
};

struct boss_professor_putricideAI : public ScriptedAI
{
    boss_professor_putricideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;

    uint32 m_uiHealthCheckTimer;
    uint32 m_uiTransitionTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiPuddleTimer;
    uint32 m_uiUnstableExperimentTimer;
    uint32 m_uiUnboundPlagueTimer;
    uint32 m_uiChokingGasBombTimer;

    void Reset() override
    {
        m_uiPhase                   = PHASE_ONE;
        m_uiHealthCheckTimer        = 1000;
        m_uiEnrageTimer             = 10 * MINUTE * IN_MILLISECONDS;
        m_uiPuddleTimer             = 10000;
        m_uiUnstableExperimentTimer = 20000;
        m_uiUnboundPlagueTimer      = 10000;
        m_uiChokingGasBombTimer     = urand(10000, 15000);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PROFESSOR_PUTRICIDE, FAIL);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiData == POINT_PUTRICIDE_SPAWN)
        {
            if (m_uiPhase == PHASE_RUNNING_ONE)
            {
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    DoScriptText(SAY_PHASE_CHANGE, m_creature);
                    m_uiTransitionTimer = 30000;
                }
                else
                {
                    DoCastSpellIfCan(m_creature, SPELL_CREATE_CONCOCTION);
                    DoScriptText(SAY_TRANSFORM_1, m_creature);
                    m_uiTransitionTimer = 15000;
                }

                m_uiPhase = PHASE_TRANSITION_ONE;           // waiting for entering phase 2
            }
            else if (m_uiPhase == PHASE_RUNNING_TWO)
            {
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    DoScriptText(SAY_PHASE_CHANGE, m_creature);
                    m_uiTransitionTimer = 30000;
                }
                else
                {
                    DoCastSpellIfCan(m_creature, SPELL_GUZZLE_POTIONS);
                    DoScriptText(SAY_TRANSFORM_2, m_creature);
                    m_uiTransitionTimer = 15000;
                }

                m_uiPhase = PHASE_TRANSITION_TWO;           // waiting for entering phase 3
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage
        if (m_uiEnrageTimer)
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiEnrageTimer = 0;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_ONE:
            {
                // health check
                if (m_uiHealthCheckTimer <= uiDiff)
                {
                    if (m_creature->GetHealthPercent() <= 80.0f)
                    {
                        uint32 spellId = (m_pInstance && m_pInstance->IsHeroicDifficulty() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS);

                        if (DoCastSpellIfCan(m_creature, spellId) == CAST_OK)
                        {
                            m_creature->GetMotionMaster()->Clear();
                            SetCombatMovement(false);
                            m_creature->GetMotionMaster()->MovePoint(POINT_PUTRICIDE_SPAWN, fPutricidePosition[0][0], fPutricidePosition[0][1], fPutricidePosition[0][2]);
                            m_uiPhase = PHASE_RUNNING_ONE;
                            return;
                        }
                    }
                    m_uiHealthCheckTimer = 1000;
                }
                else
                    m_uiHealthCheckTimer -= uiDiff;

                // Unbound Plague
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiUnboundPlagueTimer <= uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                                m_uiUnboundPlagueTimer = 70000;
                        }
                    }
                    else
                        m_uiUnboundPlagueTimer -= uiDiff;
                }

                // Slime Puddle
                if (m_uiPuddleTimer <= uiDiff)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SLIME_PUDDLE_SUMMON, SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, SPELL_SLIME_PUDDLE, CAST_TRIGGERED);
                    }
                    m_uiPuddleTimer = 30000;
                }
                else
                    m_uiPuddleTimer -= uiDiff;

                // Unstable Experiment
                if (m_uiUnstableExperimentTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
                        m_uiUnstableExperimentTimer = 30000;
                }
                else
                    m_uiUnstableExperimentTimer -= uiDiff;

                break;
            }
            case PHASE_TRANSITION_ONE:
            {
                if (m_uiTransitionTimer <= uiDiff)
                {
                    m_creature->GetMotionMaster()->Clear();
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_uiPhase = PHASE_TWO;

                    if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                    {
                        DoCastSpellIfCan(m_creature, SPELL_CREATE_CONCOCTION);
                        DoScriptText(SAY_TRANSFORM_1, m_creature);
                    }
                    else
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_CANCEL, CAST_INTERRUPT_PREVIOUS);
                }
                else
                    m_uiTransitionTimer -= uiDiff;

                return;
            }
            case PHASE_TWO:
            {
                // health check
                if (m_uiHealthCheckTimer <= uiDiff)
                {
                    if (m_creature->GetHealthPercent() <= 35.0f)
                    {
                        uint32 spellId = (m_pInstance && m_pInstance->IsHeroicDifficulty() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS);

                        if (DoCastSpellIfCan(m_creature, spellId) == CAST_OK)
                        {
                            m_creature->GetMotionMaster()->Clear();
                            SetCombatMovement(false);
                            m_creature->GetMotionMaster()->MovePoint(POINT_PUTRICIDE_SPAWN, fPutricidePosition[0][0], fPutricidePosition[0][1], fPutricidePosition[0][2]);
                            m_uiPhase = PHASE_RUNNING_TWO;

                            // TODO: remove Mutated Abomination

                            return;
                        }
                    }
                    m_uiHealthCheckTimer = 1000;
                }
                else
                    m_uiHealthCheckTimer -= uiDiff;

                // Unbound Plague
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiUnboundPlagueTimer <= uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                                m_uiUnboundPlagueTimer = 70000;
                        }
                    }
                    else
                        m_uiUnboundPlagueTimer -= uiDiff;
                }

                // Slime Puddle
                if (m_uiPuddleTimer <= uiDiff)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SLIME_PUDDLE_SUMMON, SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, SPELL_SLIME_PUDDLE, CAST_TRIGGERED);
                    }

                    m_uiPuddleTimer = 30000;
                }
                else
                    m_uiPuddleTimer -= uiDiff;

                // Unstable Experiment
                if (m_uiUnstableExperimentTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
                        m_uiUnstableExperimentTimer = 30000;
                }
                else
                    m_uiUnstableExperimentTimer -= uiDiff;

                // Choking Gas
                if (m_uiChokingGasBombTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CHOKING_GAS_BOMB) == CAST_OK)
                        m_uiChokingGasBombTimer = urand(25000, 30000);
                }
                else
                    m_uiChokingGasBombTimer -= uiDiff;

                // TODO: Malleable Goo

                break;
            }
            case PHASE_TRANSITION_TWO:
            {
                if (m_uiTransitionTimer <= uiDiff)
                {
                    m_creature->GetMotionMaster()->Clear();
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_uiPhase = PHASE_THREE;

                    if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                    {
                        DoCastSpellIfCan(m_creature, SPELL_GUZZLE_POTIONS);
                        DoScriptText(SAY_TRANSFORM_2, m_creature);
                    }
                    else
                        DoCastSpellIfCan(m_creature, SPELL_TEAR_GAS_CANCEL, CAST_INTERRUPT_PREVIOUS);
                }
                else
                    m_uiTransitionTimer -= uiDiff;

                return;
            }
            case PHASE_THREE:
            {
                // Unbound Plague
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiUnboundPlagueTimer <= uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_UNBOUND_PLAGUE, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_UNBOUND_PLAGUE) == CAST_OK)
                                m_uiUnboundPlagueTimer = 70000;
                        }
                    }
                    else
                        m_uiUnboundPlagueTimer -= uiDiff;
                }

                // Slime Puddle
                if (m_uiPuddleTimer <= uiDiff)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SLIME_PUDDLE_SUMMON, SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, SPELL_SLIME_PUDDLE, CAST_TRIGGERED);
                    }
                    m_uiPuddleTimer = 30000;
                }
                else
                    m_uiPuddleTimer -= uiDiff;

                // Choking Gas
                if (m_uiChokingGasBombTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_CHOKING_GAS_BOMB) == CAST_OK)
                        m_uiChokingGasBombTimer = urand(25000, 30000);
                }
                else
                    m_uiChokingGasBombTimer -= uiDiff;

                // TODO: Malleable Goo

                break;
            }
            case PHASE_RUNNING_ONE:
            case PHASE_RUNNING_TWO:
            {
                // wait for arriving at the table (during phase transition)
                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_professor_putricide(Creature* pCreature)
{
    return new boss_professor_putricideAI(pCreature);
}

void AddSC_boss_professor_putricide()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_professor_putricide";
    pNewScript->GetAI = &GetAI_boss_professor_putricide;
    pNewScript->RegisterSelf();
}
