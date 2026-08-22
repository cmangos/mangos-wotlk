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
SDName: boss_rotface
SD%Complete: 90%
SDComment: Some auras require improved stacking handling. Also, Ooze flow triggered spells logic requires core fix.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

enum
{
    // yells
    SAY_AGGRO                   = 37831,
    SAY_OOZE_EXPLODE            = 37063,
    SAY_SLAY_1                  = 37833,
    SAY_SLAY_2                  = 37832,
    SAY_DEATH                   = 37834,
    SAY_PUTRICIDE_AGGRO         = 37848,            // used on heroic when Festergut is dead

    EMOTE_SLIME_SPRAY           = 38363,
    EMOTE_OOZE_GROW_1           = 36950,            // grow at 2 stacks
    EMOTE_OOZE_GROW_2           = 36952,            // grow at 3 stacks
    EMOTE_OOZE_GROW_3           = 36956,            // grow at 4 stacks

    // Mutated Infection
    SPELL_MUTATED_INFECTION_10N = 69674,
    SPELL_MUTATED_INFECTION_25N = 71224,
    SPELL_MUTATED_INFECTION_10H = 73022,
    SPELL_MUTATED_INFECTION_25H = 73023,

    // Slime Spray
    SPELL_SLIME_SPRAY_SUMMON    = 70882,            // spawns 37986
    SPELL_FACE_SPRAY_STALKER    = 70881,            // targets 37986
    SPELL_SLIME_SPRAY           = 69508,            // targets 37986

    // Ooze Flood
    SPELL_OOZE_FLOOD_PERIODIC   = 70069,            // periodically trigger 69795
    SPELL_OOZE_FLOOD_REMOVE     = 70079,            // cancel aura 70069

    // Little Ooze spells
    SPELL_GREEN_ABOMINATION_HIT = 70001,
    SPELL_WEAK_RADIATING_OOZE   = 69750,
    SPELL_LITTLE_OOZE_COMBINE   = 69537,            // periodic check for 69538; combines two small oozes
    SPELL_STICKY_OOZE           = 69774,            // spawns 37006
    //SPELL_MERGE               = 69889,            // summons 36899
    //SPELL_UNSTABLE_OOZE_TRIGG = 69644,            // increate stacks of 69558 on big ooze

    // Sticky Ooze
    SPELL_STICKY_AURA           = 69776,            // damage aura on npc 37006

    // Big Ooze spells
    SPELL_UNSTABLE_OOZE         = 69558,            // stacking buff; triggers 69839 at 5 stacks
    SPELL_RADIATING_OOZE        = 69760,
    SPELL_BIG_OOZE_COMBINE      = 69552,            // periodic check for 69553; combines a small ooze with a large one
    SPELL_BIG_OOZE_BUFF_COMB    = 69611,            // periodic check to combine 69558; possible triggers 69617; combines two large oozes
    SPELL_UNSTABLE_EXPLOSION    = 69839,            // prepare the explosion triggers; summons creature 38107 on each target
    SPELL_OOZE_EXPLOSION        = 69832,

    // heroic spells
    SPELL_VILE_TRIGGER          = 72285,            // triggers 72287 which spawns 38548; Putricide casts 72272 on the spawned npc
    SPELL_VILE_GAS              = 72272,

    // npcs
    NPC_LITTLE_OOZE             = 36897,
    NPC_STICKY_OOZE             = 37006,
    NPC_BIG_OOZE                = 36899,
    NPC_VILE_GAS_STALKER        = 38548,
    NPC_OOZE_SPRAY_STALKER      = 37986,
    NPC_OOZE_EXPLOSION_STALKER  = 38107,

};

enum RotfaceActions
{
    ROTFACE_SLIME_SPRAY,
    ROTFACE_MUTATED_INFECTION,
    ROTFACE_INFECTION_ACCELERATE,
    ROTFACE_VILE_GAS,
    ROTFACE_ACTION_MAX,
};

enum LittleOozeActions
{
    LITTLE_OOZE_STICKY,
    LITTLE_OOZE_ACTION_MAX,
};

enum BigOozeActions
{
    BIG_OOZE_STICKY,
    BIG_OOZE_EXPLOSION,
    BIG_OOZE_ACTION_MAX,
};

static const float afBalconyLocation[3] = {4390.371f, 3164.5f, 389.389f};

/*######
## boss_rotface
######*/

struct boss_rotfaceAI : public CombatAI
{
    boss_rotfaceAI(Creature* creature) : CombatAI(creature, ROTFACE_ACTION_MAX),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        AddCombatAction(ROTFACE_SLIME_SPRAY, 5000u);
        AddCombatAction(ROTFACE_MUTATED_INFECTION, 14000u);
        AddCombatAction(ROTFACE_INFECTION_ACCELERATE, 90000u);
        AddCombatAction(ROTFACE_VILE_GAS, 30000u);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_infectionInterval;

    void Reset() override
    {
        CombatAI::Reset();
        m_infectionInterval = 14000;
    }

    uint32 GetMutatedInfectionSpell() const
    {
        if (!m_instance)
            return SPELL_MUTATED_INFECTION_10N;

        if (m_instance->IsHeroicDifficulty())
            return m_instance->Is25ManDifficulty() ? SPELL_MUTATED_INFECTION_25H : SPELL_MUTATED_INFECTION_10H;

        return m_instance->Is25ManDifficulty() ? SPELL_MUTATED_INFECTION_25N : SPELL_MUTATED_INFECTION_10N;
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_ROTFACE, IN_PROGRESS);

            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
            {
                putricide->CastSpell(putricide, SPELL_OOZE_FLOOD_PERIODIC, TRIGGERED_OLD_TRIGGERED);
                putricide->SetWalk(false);
                putricide->GetMotionMaster()->MovePoint(102, afBalconyLocation[0], afBalconyLocation[1], afBalconyLocation[2]);

                // heroic aggro text
                if (m_instance->IsHeroicDifficulty() && m_instance->GetData(TYPE_FESTERGUT) == DONE)
                    DoBroadcastText(SAY_PUTRICIDE_AGGRO, putricide);
            }
        }

        DoBroadcastText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_REMOVE, CAST_TRIGGERED);

        if (m_instance)
        {
            m_instance->SetData(TYPE_ROTFACE, FAIL);

            // reset putricide
            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->AI()->EnterEvadeMode();
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_REMOVE, CAST_TRIGGERED);

        if (m_instance)
        {
            m_instance->SetData(TYPE_ROTFACE, DONE);

            // reset putricide
            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->AI()->EnterEvadeMode();
        }

        DoBroadcastText(SAY_DEATH, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_OOZE_SPRAY_STALKER)
        {
            DoBroadcastText(EMOTE_SLIME_SPRAY, m_creature);
            DoCastSpellIfCan(summoned, SPELL_SLIME_SPRAY, CAST_TRIGGERED);
            DoCastSpellIfCan(summoned, SPELL_FACE_SPRAY_STALKER, CAST_TRIGGERED);
        }
        else if (summoned->GetEntry() == NPC_VILE_GAS_STALKER)
        {
            if (!m_instance)
                return;

            if (Creature* putricide = m_instance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                putricide->CastSpell(summoned, SPELL_VILE_GAS, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ROTFACE_SLIME_SPRAY:
                if (DoCastSpellIfCan(m_creature, SPELL_SLIME_SPRAY_SUMMON) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 25000));
                break;
            case ROTFACE_MUTATED_INFECTION:
            {
                uint32 infectionSpell = GetMutatedInfectionSpell();
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, infectionSpell, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA);

                if (target && DoCastSpellIfCan(target, infectionSpell) == CAST_OK)
                    ResetCombatAction(action, m_infectionInterval);
                break;
            }
            case ROTFACE_INFECTION_ACCELERATE:
                if (m_infectionInterval > 6000)
                    m_infectionInterval -= 2000;

                ResetCombatAction(action, 90000);
                break;
            case ROTFACE_VILE_GAS:
                if (!m_instance || !m_instance->IsHeroicDifficulty())
                {
                    DisableCombatAction(action);
                    break;
                }
                if (DoCastSpellIfCan(m_creature, SPELL_VILE_TRIGGER) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
        }
    }
};

UnitAI* GetAI_boss_rotface(Creature* creature)
{
    return new boss_rotfaceAI(creature);
}

/*######
## mob_little_ooze
######*/

struct mob_little_oozeAI : public CombatAI
{
    mob_little_oozeAI(Creature* creature) : CombatAI(creature, LITTLE_OOZE_ACTION_MAX)
    {
        AddCombatAction(LITTLE_OOZE_STICKY, 5000u);
        Reset();
    }

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(m_creature, SPELL_GREEN_ABOMINATION_HIT, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_WEAK_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_LITTLE_OOZE_COMBINE, CAST_TRIGGERED);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();

        // Mutated Infection summons the ooze from the affected player. Keep the
        // retail fixation by immediately giving that player overwhelming threat.
        if (Unit* spawner = m_creature->GetSpawner())
        {
            m_creature->AddThreat(spawner, 500000.0f);
            AttackStart(spawner);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STICKY_OOZE)
            summoned->CastSpell(summoned, SPELL_STICKY_AURA, TRIGGERED_OLD_TRIGGERED);
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == LITTLE_OOZE_STICKY &&
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STICKY_OOZE) == CAST_OK)
            ResetCombatAction(action, 15000);
    }
};

UnitAI* GetAI_mob_little_ooze(Creature* creature)
{
    return new mob_little_oozeAI(creature);
}

/*######
## mob_big_ooze
######*/

struct mob_big_oozeAI : public CombatAI
{
    mob_big_oozeAI(Creature* creature) : CombatAI(creature, BIG_OOZE_ACTION_MAX),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        AddCombatAction(BIG_OOZE_STICKY, 5000u);
        AddCustomAction(BIG_OOZE_EXPLOSION, true, [&]() { HandleUnstableExplosion(); });
        Reset();
    }

    instance_icecrown_citadel* m_instance;

    void Reset() override
    {
        CombatAI::Reset();

        DoCastSpellIfCan(m_creature, SPELL_GREEN_ABOMINATION_HIT, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_COMBINE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_BUFF_COMB, CAST_TRIGGERED);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // prepare explosion; Rotface yells and achiev is failed
            if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPLOSION) == CAST_OK)
            {
                if (m_instance)
                {
                    if (Creature* rotface = m_instance->GetSingleCreatureFromStorage(NPC_ROTFACE))
                        DoBroadcastText(SAY_OOZE_EXPLODE, rotface);

                    m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_DANCES_OOZES, false);
                }

                ResetTimer(BIG_OOZE_EXPLOSION, 5000);
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_STICKY_OOZE)
            summoned->CastSpell(summoned, SPELL_STICKY_AURA, TRIGGERED_OLD_TRIGGERED);
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == BIG_OOZE_STICKY &&
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STICKY_OOZE) == CAST_OK)
            ResetCombatAction(action, 15000);
    }

    void HandleUnstableExplosion()
    {
        CreatureList targetsInRange;
        GetCreatureListWithEntryInGrid(targetsInRange, m_creature, NPC_OOZE_EXPLOSION_STALKER, DEFAULT_VISIBILITY_INSTANCE);

        if (targetsInRange.empty())
        {
            script_error_log("Instance Icecrown Citadel: ERROR Failed to find creature %u for Unstable Ooze explosion.", NPC_OOZE_EXPLOSION_STALKER);
            return;
        }

        // explode all the summoned triggers
        for (Creature* target : targetsInRange)
        {
            DoCastSpellIfCan(target, SPELL_OOZE_EXPLOSION, CAST_TRIGGERED);
            target->ForcedDespawn(10000);
        }
    }
};

UnitAI* GetAI_mob_big_ooze(Creature* creature)
{
    return new mob_big_oozeAI(creature);
}

// 69674, 71224, 73022, 73023 - Mutated Infection
struct MutatedInfectionRotface : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        Unit* target = aura->GetTarget();
        if (!target)
            return;

        uint32 summonSpell = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);
        if (summonSpell)
            target->CastSpell(target, summonSpell, TRIGGERED_OLD_TRIGGERED, nullptr, aura, aura->GetCasterGuid());
    }
};

// 69558 - Unstable Ooze
struct UnstableOozeRotface : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (effIdx != EFFECT_INDEX_2 || !target->AI())
            return;

        // send AI event on 5 stacks of unstable Ooze
        SpellAuraHolder* oozeHolder = target->GetSpellAuraHolder(spell->m_spellInfo->Id);
        if (oozeHolder)
        {
            // Note: stacks are increased after the effect is processed, so we need to use (stacks - 1)
            switch (oozeHolder->GetStackAmount())
            {
                case 1: DoBroadcastText(EMOTE_OOZE_GROW_1, target); break;
                case 2: DoBroadcastText(EMOTE_OOZE_GROW_2, target); break;
                case 4: target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spell->GetCaster(), target);
                // no break;
                case 3: DoBroadcastText(EMOTE_OOZE_GROW_3, target); break;
            }
        }
    }
};

void AddSC_boss_rotface()
{
    Script* newScript = new Script;
    newScript->Name = "boss_rotface";
    newScript->GetAI = &GetAI_boss_rotface;
    newScript->RegisterSelf();

    newScript = new Script;
    newScript->Name = "mob_little_ooze";
    newScript->GetAI = &GetAI_mob_little_ooze;
    newScript->RegisterSelf();

    newScript = new Script;
    newScript->Name = "mob_big_ooze";
    newScript->GetAI = &GetAI_mob_big_ooze;
    newScript->RegisterSelf();

    RegisterSpellScript<MutatedInfectionRotface>("spell_rotface_mutated_infection");
    RegisterSpellScript<UnstableOozeRotface>("spell_unstable_ooze_rotface");
}
