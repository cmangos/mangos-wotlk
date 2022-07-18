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
SDName: Boss_Supremus
SD%Complete: 100
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    EMOTE_NEW_TARGET                = -1564010,
    EMOTE_PUNCH_GROUND              = -1564011,
    EMOTE_GROUND_CRACK              = -1564012,

    // Spells
    SPELL_BERSERK                   = 45078,
    // Phase 1
    SPELL_HATEFUL_STRIKE_PRIMER     = 41925,
    SPELL_HATEFUL_STRIKE            = 41926,
    SPELL_MOLTEN_PUNCH              = 40126,

    // Molten punch spells
    SPELL_MOLTEN_FLAME              = 40980, // Punch stalker puts it on himself

    // Phase 2
    SPELL_CHARGE                    = 41581, // used when fixate target is too far in second phase
    SPELL_VOLCANIC_ERUPTION         = 40276,
    SPELL_SLOW_SELF                 = 41922,
    SPELL_RANDOM_TARGET             = 41951, // Serverside in tbc/wotlk - sometime later changed to Fixate and visible in client

    // NPC Volcano Spells
    SPELL_VOLCANIC_ERUPTION_SELF    = 40117, // Applied to Volcano upon spawn - Cast on self - casts 40118 & 42055
    SPELL_TARGET_RANDOM_LOCATION    = 40118, // Used to select a random location within 15.00 yards around the volcano - DUMMY - Is the visual
    SPELL_VOLCANO_AOE               = 42052, // AOE Damage used by NPC - 15yard radius - TARGET_ALL_ENEMY_IN_AREA - Applied by 42055
    SPELL_TRIGGER_VOLCANO_AOE       = 42055, // AOE Trigger spell

    NPC_VOLCANO                     = 23085,
    NPC_STALKER                     = 23095,
};

// TODO: Threat reset between phases

/* Non existed spells that were used in 3.2
 * Stalker:  40257 41930
 * Supremus: 33420 41582 41925 41951
 */

const float RANGE_MOLTEN_PUNCH      = 40.0; // Not sure why this is here?

/* These floats are related to the speed-hack near end of script;
 * Statet at wowwiki: "If the gaze target is further away than 40 yards, he dashes at about five times the normal run speed until the range is about 20 yards."
 * TODO But this is currently not confirmed otherwise to be actually happening
 * const float RANGE_MIN_DASHING       = 20.0;
 * const float SPEED_DASHING           = 5.0;
 * const float SPEED_CHASE             = 0.9f;
 */

enum SupremusActions // order based on priority
{
    SUPREMUS_ACTION_PHASE_SWITCH,
    SUPREMUS_ACTION_BERSERK,
    SUPREMUS_ACTION_SWITCH_TARGET,
    SUPREMUS_ACTION_MOLTEN_PUNCH,
    SUPREMUS_ACTION_VOLCANIC_ERUPTION,
    SUPREMUS_ACTION_HATEFUL_STRIKE,
    SUPREMUS_ACTION_MAX,
    SUPREMUS_ACTION_DELAY,
};

struct boss_supremusAI : public CombatAI
{
    boss_supremusAI(Creature* creature) : CombatAI(creature, SUPREMUS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddCombatAction(SUPREMUS_ACTION_PHASE_SWITCH, GetInitialActionTimer(SUPREMUS_ACTION_PHASE_SWITCH));
        AddCombatAction(SUPREMUS_ACTION_BERSERK, GetInitialActionTimer(SUPREMUS_ACTION_BERSERK));
        AddCombatAction(SUPREMUS_ACTION_MOLTEN_PUNCH, GetInitialActionTimer(SUPREMUS_ACTION_MOLTEN_PUNCH));
        AddCombatAction(SUPREMUS_ACTION_VOLCANIC_ERUPTION, true);
        AddCombatAction(SUPREMUS_ACTION_SWITCH_TARGET, true);
        AddCombatAction(SUPREMUS_ACTION_HATEFUL_STRIKE, GetInitialActionTimer(SUPREMUS_ACTION_HATEFUL_STRIKE));
        AddCustomAction(SUPREMUS_ACTION_DELAY, true, [&]
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            if (!m_bTankPhase)
                ResetTimer(SUPREMUS_ACTION_SWITCH_TARGET, 0); // switch target immediately
            DoStartMovement(m_creature->GetVictim());
        });
        Reset();
    }

    ScriptedInstance* m_instance;

    bool m_bTankPhase;
    GuidList m_lSummonedGUIDs;

    void Reset() override
    {
        CombatAI::Reset();

        m_creature->FixateTarget(nullptr);
        m_bTankPhase = true;
    }

    uint32 GetInitialActionTimer(SupremusActions id)
    {
        switch (id)
        {
            case SUPREMUS_ACTION_PHASE_SWITCH: return 60000;
            case SUPREMUS_ACTION_BERSERK: return 900000;
            case SUPREMUS_ACTION_MOLTEN_PUNCH: return urand(11000, 14700);
            case SUPREMUS_ACTION_VOLCANIC_ERUPTION: return 6000;
            case SUPREMUS_ACTION_SWITCH_TARGET: return 0;
            case SUPREMUS_ACTION_HATEFUL_STRIKE: return 5000;
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(SupremusActions id)
    {
        switch (id)
        {
            case SUPREMUS_ACTION_PHASE_SWITCH: return 60000;
            case SUPREMUS_ACTION_BERSERK: return 0;
            case SUPREMUS_ACTION_MOLTEN_PUNCH: return urand(15000, 20000);
            case SUPREMUS_ACTION_VOLCANIC_ERUPTION: return urand(3000, 6000);
            case SUPREMUS_ACTION_SWITCH_TARGET: return 11000;
            case SUPREMUS_ACTION_HATEFUL_STRIKE: return 1200;
            default: return 0;
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SUPREMUS, NOT_STARTED);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SUPREMUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SUPREMUS, DONE);

        for (GuidList::const_iterator itr = m_lSummonedGUIDs.begin(); itr != m_lSummonedGUIDs.end(); ++itr)
        {
            if (Creature* pSummoned = m_creature->GetMap()->GetCreature(*itr))
                pSummoned->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_VOLCANO:
            {
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->AI()->SetCombatMovement(false);
                summoned->SetCanEnterCombat(false);
                summoned->CastSpell(nullptr, SPELL_VOLCANIC_ERUPTION_SELF, TRIGGERED_NONE);
                break;
            }
            case NPC_STALKER:
            {
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
                if (!target)
                    target = m_creature->GetVictim();

                summoned->CastSpell(nullptr, SPELL_MOLTEN_FLAME, TRIGGERED_NONE);
                summoned->SetInCombatWithZone();
                if (target)
                    summoned->AddThreat(target, 1000000.f);
                break;
            }
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && !m_bTankPhase)
            ResetTimer(SUPREMUS_ACTION_SWITCH_TARGET, 1000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SUPREMUS_ACTION_PHASE_SWITCH:
            {
                if (m_bTankPhase)
                {
                    m_creature->CastSpell(nullptr, SPELL_SLOW_SELF, TRIGGERED_OLD_TRIGGERED);
                    DoScriptText(EMOTE_GROUND_CRACK, m_creature);
                    m_bTankPhase = false;
                    DoResetThreat();
                    DisableTimer(SUPREMUS_ACTION_HATEFUL_STRIKE);
                    SetActionReadyStatus(SUPREMUS_ACTION_HATEFUL_STRIKE, false);
                    ResetTimer(SUPREMUS_ACTION_VOLCANIC_ERUPTION, GetInitialActionTimer(SUPREMUS_ACTION_VOLCANIC_ERUPTION));
                    SetCombatScriptStatus(true);
                    SetCombatMovement(false, true);
                    SetMeleeEnabled(false);
                    m_creature->SetTarget(nullptr);
                    ResetTimer(SUPREMUS_ACTION_DELAY, 1500);
                }
                else
                {
                    if (m_creature->HasAura(SPELL_SLOW_SELF))
                        m_creature->RemoveAurasDueToSpell(SPELL_SLOW_SELF);

                    m_creature->FixateTarget(nullptr); // fixate aura runs a tad longer
                    m_bTankPhase = true;
                    DoResetThreat();
                    DoScriptText(EMOTE_PUNCH_GROUND, m_creature);
                    DisableCombatAction(SUPREMUS_ACTION_SWITCH_TARGET);
                    DisableCombatAction(SUPREMUS_ACTION_VOLCANIC_ERUPTION);
                    ResetTimer(SUPREMUS_ACTION_HATEFUL_STRIKE, GetInitialActionTimer(SUPREMUS_ACTION_HATEFUL_STRIKE));
                    SetCombatScriptStatus(true);
                    SetCombatMovement(false, true);
                    SetMeleeEnabled(false);
                    m_creature->SetTarget(nullptr);
                    ResetTimer(SUPREMUS_ACTION_DELAY, 1500);
                }

                ResetCombatAction(action, GetSubsequentActionTimer(SupremusActions(action)));
                return;
            }
            case SUPREMUS_ACTION_BERSERK:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                return;
            case SUPREMUS_ACTION_MOLTEN_PUNCH:
                if (DoCastSpellIfCan(nullptr, SPELL_MOLTEN_PUNCH) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(SupremusActions(action)));
                return;
            case SUPREMUS_ACTION_VOLCANIC_ERUPTION:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_TANK))
                    if (DoCastSpellIfCan(target, SPELL_VOLCANIC_ERUPTION) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(SupremusActions(action)));
                return;
            }
            case SUPREMUS_ACTION_SWITCH_TARGET:
                if (DoCastSpellIfCan(nullptr, SPELL_RANDOM_TARGET) == CAST_OK)
                {
                    DoScriptText(EMOTE_NEW_TARGET, m_creature);
                    DisableCombatAction(action);
                }
                return;
            case SUPREMUS_ACTION_HATEFUL_STRIKE:
                if (DoCastSpellIfCan(nullptr, SPELL_HATEFUL_STRIKE_PRIMER) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(SupremusActions(action)));
                return;
        }
    }
};

struct SupremusRandomTarget : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster || !caster->AI())
            return;
        if (apply)
        {
            caster->FixateTarget(aura->GetTarget());
            caster->AI()->AttackStart(aura->GetTarget()); // visual change for immediate crosshair update - only safe with fixate
            caster->CastSpell(aura->GetTarget(), SPELL_CHARGE, TRIGGERED_OLD_TRIGGERED);
        }
        else
            caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, aura->GetTarget(), caster);
    }
};

struct SupremusHatefulStrikePrimer : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        auto& targetInfo = spell->GetTargetList();
        if (!target || targetInfo.rbegin()->targetGUID != target->GetObjectGuid())
            return;

        uint32 uiHealth = 0;
        target = nullptr;
        Unit* victim = caster->GetVictim();

        for (auto& targetInfo : targetInfo)
        {
            Unit* unit = caster->GetMap()->GetUnit(targetInfo.targetGUID);

            if (unit && unit != victim && caster->CanReachWithMeleeAttack(unit))
            {
                if (unit->GetHealth() > uiHealth)
                {
                    uiHealth = unit->GetHealth();
                    target = unit;
                }
            }
        }
        if (!target && victim && caster->CanReachWithMeleeAttack(victim))
            target = victim;

        caster->CastSpell(target, SPELL_HATEFUL_STRIKE, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_NORMAL_COMBAT_CAST);
    }
};

void AddSC_boss_supremus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_supremus";
    pNewScript->GetAI = &GetNewAIInstance<boss_supremusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SupremusRandomTarget>("spell_supremus_random_target");
    RegisterSpellScript<SupremusHatefulStrikePrimer>("spell_hateful_strike_primer");
}
