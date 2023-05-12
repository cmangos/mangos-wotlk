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
SDName: Boss_Vexallus
SD%Complete: 95
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "magisters_terrace.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                       = -1585007,
    SAY_ENERGY                      = -1585008,
    SAY_OVERLOAD                    = -1585009,
    SAY_KILL                        = 25627,
    EMOTE_DISCHARGE_ENERGY          = -1585011,
    EMOTE_OVERLOAD                  = -1585031,

    // Pure energy spell info
    SPELL_ENERGY_BOLT_PERIODIC      = 46156,
    SPELL_ENERGY_FEEDBACK_CHANNELED = 44328, // Channel
    SPELL_ENERGY_FEEDBACK_DEBUFF    = 44335, // The actual debuff
    //SPELL_ENERGY_FEEDBACK_VISUAL    = 44339, // Visual procced by 44328

    SPELL_ENERGY_PASSIVE            = 44326,
    SPELL_ENERGY_BOLT               = 44342,

    // Vexallus spell info
    SPELL_CHAIN_LIGHTNING           = 44318,
    SPELL_CHAIN_LIGHTNING_H         = 46380,                // heroic spell
    SPELL_OVERLOAD                  = 44352,
    SPELL_ARCANE_SHOCK              = 44319,
    SPELL_ARCANE_SHOCK_H            = 46381,                // heroic spell

    SPELL_SUMMON_PURE_ENERGY        = 44322,                // mod scale -10
    SPELL_SUMMON_PURE_ENERGY1_H     = 46154,                // mod scale -5
    SPELL_SUMMON_PURE_ENERGY2_H     = 46159,                // mod scale -5

    SPELL_CLEAR_ENERGY_FEEDBACK     = 47108,

    // Creatures
    NPC_PURE_ENERGY                 = 24745,
};

enum VexallusActions
{
    VEXALLUS_OVERLOAD,
    VEXALLUS_SUMMON_PURE_ENERGY,
    VEXALLUS_ACTION_CHAIN_LIGHTNING,
    VEXALLUS_ACTION_SHOCK,
    VEXALLUS_ACTION_MAX,
};

struct boss_vexallusAI : public CombatAI
{
    boss_vexallusAI(Creature* creature) : CombatAI(creature, VEXALLUS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddCombatAction(VEXALLUS_ACTION_CHAIN_LIGHTNING, 8000, 16000);
        AddCombatAction(VEXALLUS_ACTION_SHOCK, 25000, 30000);
        AddTimerlessCombatAction(VEXALLUS_SUMMON_PURE_ENERGY, true);
        AddTimerlessCombatAction(VEXALLUS_OVERLOAD, true);
        AddOnKillText(SAY_KILL);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float /*z*/)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr;
            });
        }
    }

    ScriptedInstance* m_instance;

    bool m_isRegularMode;

    float m_intervalHealthAmount;

    GuidVector m_sparks;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatMovement(true);
        m_intervalHealthAmount = 85;

        DespawnGuids(m_sparks);
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case VEXALLUS_ACTION_CHAIN_LIGHTNING: return 8000;
            case VEXALLUS_ACTION_SHOCK: return 8000;
            default: return 0; // never occurs but for compiler
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_VEXALLUS, FAIL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->CastSpell(nullptr, SPELL_CLEAR_ENERGY_FEEDBACK, TRIGGERED_OLD_TRIGGERED);

        if (m_instance)
            m_instance->SetData(TYPE_VEXALLUS, DONE);

        DespawnGuids(m_sparks);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_VEXALLUS, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->CastSpell(nullptr, SPELL_ENERGY_BOLT_PERIODIC, TRIGGERED_OLD_TRIGGERED);
        summoned->CastSpell(nullptr, SPELL_ENERGY_PASSIVE, TRIGGERED_OLD_TRIGGERED);

        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
        {
            summoned->AddThreat(target, 1000000.f);
            summoned->AI()->AttackStart(target);
        }
        m_sparks.push_back(summoned->GetObjectGuid());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case VEXALLUS_ACTION_CHAIN_LIGHTNING:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CHAIN_LIGHTNING, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(target, m_isRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case VEXALLUS_ACTION_SHOCK:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ARCANE_SHOCK, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                    DoCastSpellIfCan(target, m_isRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H);

                ResetCombatAction(action, GetSubsequentActionTimer(action));
                return;
            }
            case VEXALLUS_SUMMON_PURE_ENERGY:
            {
                // used for check, when Vexallus cast adds 85%, 70%, 55%, 40%, 25%
                if (m_creature->GetHealthPercent() <= m_intervalHealthAmount)
                {
                    DoScriptText(SAY_ENERGY, m_creature);
                    DoScriptText(EMOTE_DISCHARGE_ENERGY, m_creature);
                    m_intervalHealthAmount -= 15.0f;
                    if (m_intervalHealthAmount == 10.f)
                        SetActionReadyStatus(action, false);

                    if (m_isRegularMode)
                        DoCastSpellIfCan(nullptr, SPELL_SUMMON_PURE_ENERGY);
                    else
                    {
                        DoCastSpellIfCan(nullptr, SPELL_SUMMON_PURE_ENERGY1_H, CAST_TRIGGERED);
                        DoCastSpellIfCan(nullptr, SPELL_SUMMON_PURE_ENERGY2_H, CAST_TRIGGERED);
                    }
                }
                return;
            }
            case VEXALLUS_OVERLOAD:
                if (m_creature->GetHealthPercent() > 20.f)
                    return;
                DoCastSpellIfCan(nullptr, SPELL_OVERLOAD);
                SetActionReadyStatus(action, false);
                DoScriptText(SAY_OVERLOAD, m_creature);
                DoScriptText(EMOTE_OVERLOAD, m_creature);
                break;
        }
    }
};

struct mob_pure_energyAI : public ScriptedAI
{
    mob_pure_energyAI(Creature* creature) : ScriptedAI(creature)
    {
        SetDeathPrevention(true);
        SetMeleeEnabled(false);
        SetCombatMovement(true);
        Reset();
    }

    void Reset() override { }

    void JustPreventedDeath(Unit* attacker) override
    {
        if (attacker->IsUnit() && attacker->IsControlledByPlayer())
            attacker = const_cast<Player*>(attacker->GetControllingPlayer());

        DoFakeDeath();
        m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_BOLT_PERIODIC);
        SetCombatMovement(false);
        m_creature->CastSpell(attacker, SPELL_ENERGY_FEEDBACK_CHANNELED, TRIGGERED_NONE);
        attacker->CastSpell(nullptr, SPELL_ENERGY_FEEDBACK_DEBUFF, TRIGGERED_OLD_TRIGGERED);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_creature->ForcedDespawn(1); // on next update
    }
};

struct spell_clear_energy_feedback : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (spell->GetUnitTarget())
            spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_ENERGY_FEEDBACK_DEBUFF);
    }
};

void AddSC_boss_vexallus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_vexallus";
    pNewScript->GetAI = &GetNewAIInstance<boss_vexallusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_pure_energy";
    pNewScript->GetAI = &GetNewAIInstance<mob_pure_energyAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_clear_energy_feedback>("spell_clear_energy_feedback");
}
