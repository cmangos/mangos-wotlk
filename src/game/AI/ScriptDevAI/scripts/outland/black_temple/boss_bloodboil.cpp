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
SDName: Boss_Bloodboil
SD%Complete: 90
SDComment: Timers may need adjustments.
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    // Speech'n'Sound
    SAY_AGGRO                   = -1564029,
    SAY_SLAY1                   = -1564030,
    SAY_SLAY2                   = -1564031,
    SAY_SPECIAL1                = -1564032,
    SAY_SPECIAL2                = -1564033,
    SAY_ENRAGE1                 = -1564034,
    SAY_ENRAGE2                 = -1564035,
    SAY_DEATH                   = -1564036,

    // Spells
    // Buffs
    SPELL_HIT_CHANCE            = 43689,

    // Phase 1
    SPELL_FEL_ACID_1            = 40508,
    SPELL_ARCING_SMASH_1        = 40457,
    SPELL_EJECT_1               = 40486,
    SPELL_ACIDIC_WOUND          = 40484,
    SPELL_BLOODBOIL             = 42005,
    SPELL_BEWILDERING_STRIKE    = 40491,

    // Phase Transition
    SPELL_FEL_GEYSER_TARGETING  = 40606,
    SPELL_FEL_GEYSER            = 40591,
    SPELL_FEL_GEYSER_SUMMON     = 40569,
    SPELL_FEL_GEYSER_DAMAGE     = 40593,
    SPELL_BIRTH_NO_VISUAL       = 40031,

    // Phase 2
    SPELL_FEL_ACID_2            = 40595,
    SPELL_ARCING_SMASH_2        = 40599,
    SPELL_EJECT_2               = 40597,
    SPELL_INSIGNIFIGANCE        = 40618,
    SPELL_FEL_RAGE              = 40594,
    SPELL_FEL_RAGE_PLAYER_1     = 40604,
    SPELL_FEL_RAGE_PLAYER_2     = 40616,
    SPELL_FEL_RAGE_PLAYER_3     = 41625,
    SPELL_FEL_RAGE_PLAYER_4     = 46787,
    SPELL_TAUNT_GURTOGG         = 40603,

    // Other spells
    SPELL_CHARGE                = 40602,                // spell not confirmed
    SPELL_BERSERK               = 27680,

    MAX_BLOODBOILS              = 5,
};

enum GurtoggActions
{
    GURTOGG_ACTION_CHANGE_PHASE,
    GURTOGG_ACTION_BERSERK,
    GURTOGG_ACTION_BLOODBOIL, // Phase 1 only
    GURTOGG_ACTION_ARCING_SMASH,
    GURTOGG_ACTION_FEL_ACID,
    GURTOGG_ACTION_BEWILDERING_STRIKE, // Phase 1 only
    GURTOGG_ACTION_EJECT,
    GURTOGG_ACTION_MAX,
};

struct boss_gurtogg_bloodboilAI : public ScriptedAI, public CombatActions
{
    boss_gurtogg_bloodboilAI(Creature* creature) : ScriptedAI(creature), CombatActions(GURTOGG_ACTION_MAX)
    {
        m_instance = static_cast<ScriptedInstance*>(creature->GetInstanceData());
        AddCombatAction(GURTOGG_ACTION_CHANGE_PHASE, 0u);
        AddCombatAction(GURTOGG_ACTION_BERSERK, 0u);
        AddCombatAction(GURTOGG_ACTION_BLOODBOIL, 0u);
        AddCombatAction(GURTOGG_ACTION_ARCING_SMASH, 0u);
        AddCombatAction(GURTOGG_ACTION_FEL_ACID, 0u);
        AddCombatAction(GURTOGG_ACTION_BEWILDERING_STRIKE, 0u);
        AddCombatAction(GURTOGG_ACTION_EJECT, 0u);
        Reset();
    }

    ScriptedInstance* m_instance;

    bool m_phase1;
    ObjectGuid m_felRageTarget;

    void Reset() override
    {
        for (uint32 i = 0; i < GURTOGG_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(GURTOGG_ACTION_CHANGE_PHASE,         GetInitialActionTimer(GURTOGG_ACTION_CHANGE_PHASE));
        ResetTimer(GURTOGG_ACTION_BERSERK,              GetInitialActionTimer(GURTOGG_ACTION_BERSERK));
        ResetTimer(GURTOGG_ACTION_BLOODBOIL,            GetInitialActionTimer(GURTOGG_ACTION_BLOODBOIL));
        ResetTimer(GURTOGG_ACTION_ARCING_SMASH,         GetInitialActionTimer(GURTOGG_ACTION_ARCING_SMASH));
        ResetTimer(GURTOGG_ACTION_FEL_ACID,             GetInitialActionTimer(GURTOGG_ACTION_FEL_ACID));
        ResetTimer(GURTOGG_ACTION_BEWILDERING_STRIKE,   GetInitialActionTimer(GURTOGG_ACTION_BEWILDERING_STRIKE));
        ResetTimer(GURTOGG_ACTION_EJECT,                GetInitialActionTimer(GURTOGG_ACTION_EJECT));

        DoCastSpellIfCan(nullptr, SPELL_HIT_CHANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_ACIDIC_WOUND, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_phase1 = true;
    }

    uint32 GetInitialActionTimer(GurtoggActions id)
    {
        switch (id)
        {
            case GURTOGG_ACTION_CHANGE_PHASE: return MINUTE * IN_MILLISECONDS;
            case GURTOGG_ACTION_BERSERK: return 10 * MINUTE * IN_MILLISECONDS;
            case GURTOGG_ACTION_BLOODBOIL: return 11000;
            case GURTOGG_ACTION_ARCING_SMASH: return 19000;
            case GURTOGG_ACTION_FEL_ACID: return urand(8000,23000);
            case GURTOGG_ACTION_BEWILDERING_STRIKE: return 15000;
            case GURTOGG_ACTION_EJECT: return 10000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(GurtoggActions id)
    {
        switch (id)
        {
            case GURTOGG_ACTION_CHANGE_PHASE: if (m_phase1) return 60000; else return 30000;
            case GURTOGG_ACTION_BLOODBOIL: return 11000;
            case GURTOGG_ACTION_ARCING_SMASH: return 10000;
            case GURTOGG_ACTION_FEL_ACID: return urand(7000, 23000);
            case GURTOGG_ACTION_BEWILDERING_STRIKE: return 20000;
            case GURTOGG_ACTION_EJECT: return 15000;
            default: return 0;
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BLOODBOIL, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(nullptr, SPELL_HIT_CHANCE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_ACIDIC_WOUND, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        if (m_instance)
            m_instance->SetData(TYPE_BLOODBOIL, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BLOODBOIL, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }
    
    void JustSummoned(Creature* summoned) override
    {
        summoned->AI()->SetReactState(REACT_PASSIVE);
        summoned->CastSpell(nullptr, SPELL_FEL_GEYSER_DAMAGE, TRIGGERED_NONE);
        summoned->CastSpell(nullptr, SPELL_BIRTH_NO_VISUAL, TRIGGERED_NONE);
    }

    void SwitchPhase()
    {
        if (m_phase1)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 2, nullptr, SELECT_FLAG_PLAYER))
            {
                m_felRageTarget = target->GetObjectGuid();
                // Buff self
                DoCastSpellIfCan(nullptr, SPELL_FEL_RAGE);
                DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, m_creature);

                // Debuff player
                DoCastSpellIfCan(target, SPELL_FEL_RAGE_PLAYER_1, CAST_TRIGGERED);
                DoCastSpellIfCan(target, SPELL_FEL_RAGE_PLAYER_2, CAST_TRIGGERED);
                DoCastSpellIfCan(target, SPELL_FEL_RAGE_PLAYER_3, CAST_TRIGGERED);
                DoCastSpellIfCan(target, SPELL_FEL_RAGE_PLAYER_4, CAST_TRIGGERED);
                // Allow player to taunt Gurtogg
                target->CastSpell(m_creature, SPELL_TAUNT_GURTOGG, TRIGGERED_OLD_TRIGGERED);

                // Don't allow others to generate threat
                DoCastSpellIfCan(m_creature, SPELL_INSIGNIFIGANCE, CAST_TRIGGERED);

                m_creature->RemoveAurasDueToSpell(SPELL_ACIDIC_WOUND);
                m_creature->CastSpell(nullptr, SPELL_FEL_GEYSER, TRIGGERED_OLD_TRIGGERED);
                m_creature->CastSpell(target, SPELL_FEL_GEYSER_SUMMON, TRIGGERED_OLD_TRIGGERED);

                // Reset timers
                m_phase1 = false;
                DisableCombatAction(GURTOGG_ACTION_BLOODBOIL);
                DisableCombatAction(GURTOGG_ACTION_BEWILDERING_STRIKE);
                ResetTimer(GURTOGG_ACTION_ARCING_SMASH, 10000);
            }
        }
        else
        {
            // Reset timers
            // TODO: recheck phase end timers
            m_phase1 = true;
            DoCastSpellIfCan(nullptr, SPELL_ACIDIC_WOUND, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            m_creature->CastSpell(nullptr, SPELL_FEL_GEYSER, TRIGGERED_OLD_TRIGGERED);
            ResetTimer(GURTOGG_ACTION_BLOODBOIL, 12000);
            ResetTimer(GURTOGG_ACTION_ARCING_SMASH, 10000);
            ResetTimer(GURTOGG_ACTION_FEL_ACID, GetInitialActionTimer(GURTOGG_ACTION_FEL_ACID));
            ResetTimer(GURTOGG_ACTION_BEWILDERING_STRIKE, 28000);
            ResetTimer(GURTOGG_ACTION_EJECT, 30000);
            if (Unit* felRageTarget = m_creature->GetMap()->GetCreature(m_felRageTarget))
                m_creature->getThreatManager().modifyThreatPercent(felRageTarget, -100);
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < GURTOGG_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case GURTOGG_ACTION_CHANGE_PHASE:
                    {
                        SwitchPhase();
                        ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case GURTOGG_ACTION_BERSERK:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_ENRAGE1 : SAY_ENRAGE2, m_creature);
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                    case GURTOGG_ACTION_BLOODBOIL:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_BLOODBOIL) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                    case GURTOGG_ACTION_ARCING_SMASH:
                    {
                        if (DoCastSpellIfCan(nullptr, m_phase1 ? SPELL_ARCING_SMASH_1 : SPELL_ARCING_SMASH_2) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                    case GURTOGG_ACTION_FEL_ACID:
                    {
                        uint32 exclusionCount = 0;
                        uint32 flags = SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE;
                        if (m_creature->getThreatManager().getThreatList().size() >= 2) // when at least 2 players skip 2 topaggro and main tank
                        {
                            flags |= SELECT_FLAG_SKIP_TANK;
                            exclusionCount = 2;
                        }
                        Unit* target = m_phase1 ? m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, exclusionCount, SPELL_FEL_ACID_1, flags) : m_creature->GetVictim();
                        if (target)
                        {
                            if (DoCastSpellIfCan(target, m_phase1 ? SPELL_FEL_ACID_1 : SPELL_FEL_ACID_2) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        continue;
                    }
                    case GURTOGG_ACTION_BEWILDERING_STRIKE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BEWILDERING_STRIKE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                    case GURTOGG_ACTION_EJECT:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), m_phase1 ? SPELL_EJECT_1 : SPELL_EJECT_2) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(GurtoggActions(i)));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        EnterEvadeIfOutOfCombatArea(diff);

        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_gurtogg_bloodboil(Creature* pCreature)
{
    return new boss_gurtogg_bloodboilAI(pCreature);
}

void AddSC_boss_gurtogg_bloodboil()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_gurtogg_bloodboil";
    pNewScript->GetAI = &GetAI_boss_gurtogg_bloodboil;
    pNewScript->RegisterSelf();
}
