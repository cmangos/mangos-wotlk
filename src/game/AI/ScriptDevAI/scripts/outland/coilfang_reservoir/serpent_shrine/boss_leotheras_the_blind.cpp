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
SDName: Boss_Leotheras_The_Blind
SD%Complete: 100
SDComment:
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO               = -1548009,
    SAY_SWITCH_TO_DEMON     = -1548010,
    SAY_INNER_DEMONS        = -1548011,
    SAY_DEMON_SLAY1         = -1548012,
    SAY_DEMON_SLAY2         = -1548013,
    SAY_DEMON_SLAY3         = -1548014,
    SAY_NIGHTELF_SLAY1      = -1548015,
    SAY_NIGHTELF_SLAY2      = -1548016,
    SAY_NIGHTELF_SLAY3      = -1548017,
    SAY_FINAL_FORM          = -1548018,
    SAY_FREE                = -1548019,
    SAY_DEATH               = -1548020,

    SPELL_BERSERK           = 27680,
    SPELL_WHIRLWIND         = 37640,
    SPELL_WHIRLWIND_PROC    = 37641,                    // Procced from 37640
    SPELL_CHAOS_BLAST       = 37674,                    // triggers 37675
    SPELL_INSIDIOUS_WHISPER = 37676,
    SPELL_WHISPER_CLEAR     = 37922,                    // purpose unk - probably clear the demons on evade
    SPELL_CONS_MADNESS      = 37749,                    // charm spell for the players which didn't kill the inner demons during the demon phase
    SPELL_CLR_CONS_MADNESS  = 37750,
    SPELL_METAMORPHOSIS     = 37673,                    // demon transform spell
    SPELL_SUMMON_PHANTOM    = 37545,
    SPELL_SUMMON_INNER_DEMON = 37735,
    SPELL_SUMMON_SHADOW     = 37781,

    // SPELL_DEMON_ALIGNMENT = 37713,
    SPELL_SHADOW_BOLT       = 39309,
    SPELL_DEMON_LINK        = 37716,

    FACTION_DEMON_1         = 1829,
    FACTION_DEMON_2         = 1830,
    FACTION_DEMON_3         = 1831,
    FACTION_DEMON_4         = 1832,
    FACTION_DEMON_5         = 1833,

    NPC_INNER_DEMON         = 21857,
    NPC_SHADOW_LEO          = 21875

    // Note: Pre 2.1 metamorphosis and whirlwind use completely different timers
};

enum LeotherasActions
{
    LEOTHERAS_PHASE_2,
    LEOTHERAS_METAMORPHOSIS,
    LEOTHERAS_ENRAGE,
    LEOTHERAS_INSIDIOUS_WHISPER,
    LEOTHERAS_CHAOS_BLAST,
    LEOTHERAS_WHIRLWIND,
    LEOTHERAS_ACTION_MAX,
    LEOTHERAS_BANISH,
    LEOTHERAS_PHASE_TRANSITION,
};

struct boss_leotheras_the_blindAI : public CombatAI
{
    boss_leotheras_the_blindAI(Creature* creature) : CombatAI(creature, LEOTHERAS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(LEOTHERAS_PHASE_2, true);
#ifdef PRENERF_2_0_3
        AddCombatAction(LEOTHERAS_METAMORPHOSIS, 65000u);
        AddCombatAction(LEOTHERAS_WHIRLWIND, 15000, 18000);
#else
        AddCombatAction(LEOTHERAS_METAMORPHOSIS, 60000u);
        AddCombatAction(LEOTHERAS_WHIRLWIND, 18000u);
#endif
        AddCombatAction(LEOTHERAS_ENRAGE, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(LEOTHERAS_CHAOS_BLAST, true);
        AddCombatAction(LEOTHERAS_INSIDIOUS_WHISPER, true);
        AddCustomAction(LEOTHERAS_BANISH, 1000u, [&]() { HandleBanish(); });
        AddCustomAction(LEOTHERAS_PHASE_TRANSITION, true, [&]() { HandlePhaseTransition(); });
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float /*z*/)
        {
            return !(x > 290.0f && x < 411.0f && y > -526.0f &&  y < -299.0f);
        });
    }

    ScriptedInstance* m_instance;

    uint32 m_finalFormPhase;

    GuidSet m_charmTargets;

    bool m_bDemonForm;

    void Reset() override
    {
        CombatAI::Reset();
        m_finalFormPhase    = 0;

        m_bDemonForm        = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->SetStunned(false);

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        m_creature->SetCanEnterCombat(false);
        m_attackDistance = 0.f;

        SetReactState(REACT_PASSIVE);
        m_charmTargets.clear();
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_CONS_MADNESS)
            m_charmTargets.insert(target->GetObjectGuid());
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_LEOTHERAS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY1 : SAY_NIGHTELF_SLAY1, m_creature); break;
            case 1: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY2 : SAY_NIGHTELF_SLAY2, m_creature); break;
            case 2: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY3 : SAY_NIGHTELF_SLAY3, m_creature); break;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SHADOW_LEO)
        {
            summoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            summoned->AI()->SetMoveChaseParams(35.f, 0.f, false);
            summoned->AI()->AttackStart(m_creature->GetVictim());
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->CastSpell(nullptr, SPELL_CLR_CONS_MADNESS, TRIGGERED_NONE);

        if (m_instance)
            m_instance->SetData(TYPE_LEOTHERAS_EVENT, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->CastSpell(nullptr, SPELL_CLR_CONS_MADNESS, TRIGGERED_NONE);
        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_LEOTHERAS_EVENT, FAIL);

        ResetTimer(LEOTHERAS_BANISH, 1000);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            DoScriptText(SAY_AGGRO, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_LEOTHERAS_BANISH);
            m_creature->SetCanEnterCombat(true);
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
            DoResetThreat();
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            if (m_charmTargets.find(invoker->GetObjectGuid()) != m_charmTargets.end())
                m_creature->CastSpell(invoker, SPELL_CONS_MADNESS, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void HandleBanish()
    {
        if (!m_creature->IsInCombat())
            DoCastSpellIfCan(nullptr, SPELL_LEOTHERAS_BANISH);
    }

    void HandlePhaseTransition()
    {
        uint32 timer = 0;
        switch (m_finalFormPhase)
        {
            case 0:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_finalFormPhase++;
                timer = 11000;
                break;
            case 1:
            {
                SetCombatScriptStatus(false);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->CastSpell(m_creature, SPELL_SUMMON_SHADOW, TRIGGERED_OLD_TRIGGERED);

                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->SetStunned(false);

                SetCombatMovement(true);
                SetMeleeEnabled(true);

                ResetCombatAction(LEOTHERAS_WHIRLWIND, urand(1000, 10000));
                m_attackDistance = 0.f;
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance);
                break;
            }
        }
        if (timer)
            ResetTimer(LEOTHERAS_PHASE_TRANSITION, timer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LEOTHERAS_PHASE_2:
                // Prepare to summon the Shadow of Leotheras
                if (m_creature->GetHealthPercent() < 15.0f)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND); // whirlwind is removed on entering demon form
                    DoScriptText(SAY_FINAL_FORM, m_creature);
                    ResetTimer(LEOTHERAS_PHASE_TRANSITION, 1000);

                    // reset him to human form if necessary
                    if (m_bDemonForm)
                    {
                        if (m_creature->IsNonMeleeSpellCasted(false))
                            m_creature->InterruptNonMeleeSpells(false);

                        // switch to nightelf form
                        m_creature->RemoveAurasDueToSpell(SPELL_METAMORPHOSIS);

                        DoResetThreat();
                        m_bDemonForm = false;
                        DisableCombatAction(LEOTHERAS_INSIDIOUS_WHISPER);
                        DisableCombatAction(LEOTHERAS_CHAOS_BLAST);
                    }

                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    m_creature->SetTarget(nullptr);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                    m_creature->SetStunned(true);
                    // BUT DO NOT DO THIS ANYWHERE ELSE, its a purely dynamic flag

                    SetCombatMovement(false);
                    SetActionReadyStatus(action, false);
                    DisableCombatAction(LEOTHERAS_METAMORPHOSIS);
                }
                break;
            case LEOTHERAS_METAMORPHOSIS:
                if (m_bDemonForm)
                {
                    if (m_creature->IsNonMeleeSpellCasted(false))
                        m_creature->InterruptNonMeleeSpells(false);

                    // switch to nightelf form
                    m_creature->RemoveAurasDueToSpell(SPELL_METAMORPHOSIS);
                    SetEquipmentSlots(true);

                    m_attackDistance = 0.f;
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

                    DoResetThreat();
                    m_bDemonForm = false;

                    ResetCombatAction(LEOTHERAS_WHIRLWIND, urand(1000, 10000));
                    ResetCombatAction(action, 60000);
                    DisableCombatAction(LEOTHERAS_INSIDIOUS_WHISPER);
                    DisableCombatAction(LEOTHERAS_CHAOS_BLAST);
                }
                else
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_METAMORPHOSIS) == CAST_OK) // TODO: make this stuff happen on cast end
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND); // whirlwind is removed on entering demon form
                        DoScriptText(SAY_SWITCH_TO_DEMON, m_creature);
                        SetEquipmentSlots(false, 0, 0, 0); // remove weapons
                        m_attackDistance = 35.f;
                        m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim(), m_attackDistance, m_attackAngle, m_moveFurther);

                        DoResetThreat();
                        m_bDemonForm = true;

                        ResetCombatAction(action, 60000);
                        DisableCombatAction(LEOTHERAS_WHIRLWIND);
                        ResetCombatAction(LEOTHERAS_INSIDIOUS_WHISPER, 20000);
                        ResetCombatAction(LEOTHERAS_CHAOS_BLAST, 1);
                    }
                }
                break;
            case LEOTHERAS_ENRAGE:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case LEOTHERAS_WHIRLWIND:
                if (DoCastSpellIfCan(nullptr, SPELL_WHIRLWIND) == CAST_OK)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        m_creature->AddThreat(target, 100000.f);

#ifdef PRENERF_2_0_3
                    ResetCombatAction(action, urand(25000, 35000));
#else
                    ResetCombatAction(action, 32000);
#endif
                }
                break;
            case LEOTHERAS_INSIDIOUS_WHISPER:
                m_creature->GetInstanceData()->SetData(TYPE_LEOTHERAS_EVENT_DEMONS, 0); // start counter from zero
                if (DoCastSpellIfCan(nullptr, SPELL_INSIDIOUS_WHISPER, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoScriptText(SAY_INNER_DEMONS, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case LEOTHERAS_CHAOS_BLAST:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAOS_BLAST) == CAST_OK)
                    ResetCombatAction(action, urand(2000, 3000));
                break;
        }
    }
};

struct npc_inner_demonAI : public ScriptedAI
{
    npc_inner_demonAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
        m_creature->CastSpell(nullptr, SPELL_DEMON_LINK, TRIGGERED_NONE);
        m_creature->setFaction(m_creature->GetInstanceData()->GetData(6) + FACTION_DEMON_1);
    }

    uint32 m_uiShadowBoltTimer;

    void Reset() override
    {
        m_uiShadowBoltTimer = urand(6100, 8400);
    }

    void JustRespawned() override
    {
        if (Unit* spawner = m_creature->GetSpawner())
        {
            AttackStart(spawner);
            m_creature->AddThreat(spawner, 50000.f);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget())
            return;

        if (m_uiShadowBoltTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                m_uiShadowBoltTimer = urand(7900, 12500);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer) override
    {
        if (m_creature == killer || killer->GetObjectGuid() == m_creature->GetSpawnerGuid())
            if (Unit* summoner = m_creature->GetMap()->GetUnit(m_creature->GetSpawnerGuid()))
                summoner->RemoveAurasDueToSpell(SPELL_INSIDIOUS_WHISPER);

        m_creature->RemoveAurasDueToSpell(SPELL_DEMON_LINK);
    }
};

struct InsidiousWhisper : public SpellScript, public AuraScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(4);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->GetVictim() == target) // skips tank
            return false;

        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)) // not MCed
            return false;

        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (target->GetTypeId() != TYPEID_PLAYER)
            return;

        if (apply)
        {
            target->CastSpell(target, 37735, TRIGGERED_OLD_TRIGGERED); // Summon Inner Demon

            InstanceData* data = target->GetInstanceData();
            if (data)
            {
                aura->GetModifier()->m_amount = target->GetInstanceData()->GetData(6);
                target->GetInstanceData()->SetData(6, aura->GetModifier()->m_amount + 1);
                aura->GetModifier()->m_amount += 1018;
            }
            else
                aura->GetModifier()->m_amount = 1018;
        }

        ReputationRank faction_rank = ReputationRank(1); // value taken from sniff

        Player* player = (Player*)target;

        player->GetReputationMgr().ApplyForceReaction(aura->GetModifier()->m_amount, faction_rank, apply);
        player->GetReputationMgr().SendForceReactions();

        // stop fighting if at apply forced rank friendly or at remove real rank friendly
        if ((apply && faction_rank >= REP_FRIENDLY) || (!apply && player->GetReputationRank(aura->GetModifier()->m_amount) >= REP_FRIENDLY))
            player->StopAttackFaction(aura->GetModifier()->m_amount);

        if (!apply)
        {
            if (aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE) // MC player if inner demon was not killed
            {
                if (Unit* caster = aura->GetCaster())
                {
                    caster->CastSpell(target, 37749, TRIGGERED_OLD_TRIGGERED); // Consuming Madness
                    caster->getThreatManager().modifyThreatPercent(target, -100);
                }
            }
        }
    }
};

struct ClearConsumingMadness : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
        {
            spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_INSIDIOUS_WHISPER);
            if (spell->GetUnitTarget()->HasAura(SPELL_CONS_MADNESS))
            {
                spell->GetUnitTarget()->RemoveAurasDueToSpell(SPELL_CONS_MADNESS);
                spell->GetUnitTarget()->Suicide();
            }
            if (spell->GetUnitTarget()->HasAura(37716) && spell->GetUnitTarget()->GetTypeId() == TYPEID_UNIT)
                static_cast<Creature*>(spell->GetUnitTarget())->ForcedDespawn();
        }
    }
};

struct LeoWhirlwindAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            if (UnitAI* ai = aura->GetTarget()->AI())
                ai->DoResetThreat();
    }
};

struct LeoWhirlwindProc : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        UnitAI* ai = spell->GetCaster()->AI();
        if (!ai || spell->GetCaster()->GetTypeId() != TYPEID_UNIT)
            return;

        if (urand(0, 2) == 0 && spell->GetCaster()->HasAura(37640))
        {
            ai->DoResetThreat();
            if (Unit* target = static_cast<Creature*>(spell->GetCaster())->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                spell->GetCaster()->AddThreat(target, 100000.f);
        }
        return;
    }
};

void AddSC_boss_leotheras_the_blind()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_leotheras_the_blind";
    pNewScript->GetAI = &GetNewAIInstance<boss_leotheras_the_blindAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_inner_demon";
    pNewScript->GetAI = &GetNewAIInstance<npc_inner_demonAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<InsidiousWhisper>("spell_insidious_whisper");
    RegisterSpellScript<ClearConsumingMadness>("spell_clear_consuming_madness");
    RegisterSpellScript<LeoWhirlwindAura>("spell_leotheras_whirlwind_aura");
    RegisterSpellScript<LeoWhirlwindProc>("spell_leotheras_whirlwind_proc");
}
