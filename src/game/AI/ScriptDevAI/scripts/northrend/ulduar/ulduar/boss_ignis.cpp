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
SDName: boss_ignis
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "ulduar.h"
#include "World/WorldStateDefines.h"

enum
{
    SAY_AGGRO                           = 34306,
    SAY_FLAME_JETS                      = 34308,
    SAY_SLAGPOT_1                       = 34309,
    SAY_SLAGPOT_2                       = 34310,
    SAY_ADDS                            = 34307,
    SAY_SLAY_1                          = 34311,
    SAY_SLAY_2                          = 34312,
    SAY_BERSERK                         = 33871,
    SAY_DEATH                           = 34313,

    EMOTE_FLAME_JETS                    = 33465,
    EMOTE_EXTINGUISH_SCORCH             = 34471,

    // spells
    SPELL_FLAME_JETS                    = 62680,
    SPELL_FLAME_JETS_H                  = 63472,
    SPELL_SLAG_POT                      = 62717,        // damage aura applied when passenger is switched to second seat
    SPELL_SLAG_POT_PERIODIC             = 65722,
    SPELL_SLAG_POT_PERIODIC_H           = 65723,
    SPELL_SLAG_IMBUED                   = 63536,        // buff received if target survives the slag pot
    SPELL_GRAB                          = 62707,        // charge spells for Slag pot - triggers 62708 which will load the player into Ingis' hand (seat 1)
    SPELL_GRAB_POS1                     = 62708,
    SPELL_GRAB_POT                      = 62711,        // aura triggered after 1,5 sec after the first grab; switches the seats from hand to pot (seat 2)
    SPELL_SCORCH                        = 62546,
    SPELL_SCORCH_H                      = 63474,
    SPELL_SCORCH_SUMMON                 = 62551,        // summons npc 33221
    SPELL_ACTIVATE_CONSTRUCT            = 62488,        // activates constructs and set them in combat (handled in core)
    SPELL_KILL_ALL_CONSTRUCTS           = 65109,        // on death
    SPELL_BERSERK                       = 26662,

    // iron construct
    SPELL_CONSTRUCT_HITTING_YA          = 65110,        // procs on melee damage; purpose unk
    SPELL_STONED                        = 62468,        // mechanical stun aura
    SPELL_HEAT                          = 65667,        // stackable aura which heats the construct
    SPELL_MOLTEN                        = 62373,        // aura gained by the construct when heated to 10 stacks in Scorch
    SPELL_CHILL                         = 62381,        // chill a construct when moved in water
    SPELL_BRITTLE                       = 62382,        // stun a construct when chilled in water
    SPELL_BRITTLE_H                     = 67114,
    SPELL_SHATTER                       = 62383,        // sends event 21620 for the achiev check
    SPELL_STRENGTH_BUFF                 = 64473,
    SPELL_STRENGTH                      = 64474,
    SPELL_STRENGTH_REMOVE               = 64475,        // remove 1 stack of the Strength of Creator on construct death
    SPELL_WATER_EFFECT                  = 64503,        // spell effect which cools the heated constructs and scorch npcs
    // SPELL_WATER                      = 64502,        // cast by world triggers, in order to check when the constructs reach the water

    // scorch target
    SPELL_SCORCH_AURA                   = 62548,
    SPELL_SCORCH_AURA_H                 = 63476,

    // NPC ids
    NPC_SCORCH                          = 33221,

    MAX_HEAT_STACKS                     = 10,
};

/*######
## boss_ignis
######*/

struct boss_ignisAI : public BossAI
{
    boss_ignisAI(Creature* creature) : BossAI(creature, 0),
    m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_IGNIS);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddOnDeathText(SAY_DEATH);
        AddCastOnDeath({ObjectGuid(), SPELL_KILL_ALL_CONSTRUCTS, TRIGGERED_OLD_TRIGGERED});
        AddRespawnOnEvade(30s);
    }

    bool m_isRegularMode;

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();
        auto* ignisAdds = m_creature->GetMap()->GetCreatures("ULDUAR_IGNIS_ADDS");
        if (!ignisAdds || (*ignisAdds).empty())
            return;
        std::vector<Creature*> constructs((*ignisAdds).size());
        std::copy((*ignisAdds).begin(), (*ignisAdds).end(), std::back_inserter(constructs));
        for (Creature* construct : constructs)
        {
            if (!construct)
                continue;
            construct->ForcedDespawn();
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_SCORCH)
            return;
        summoned->CastSpell(nullptr, m_isRegularMode ? SPELL_SCORCH_AURA : SPELL_SCORCH_AURA_H, TRIGGERED_OLD_TRIGGERED);
        m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
    }

    void OnPassengerRide(Unit* passenger, bool boarded, uint8 seat) override
    {
        if (!boarded || !passenger || seat != 1)
            return;
        DoCastSpellIfCan(passenger, SPELL_SLAG_POT, CAST_TRIGGERED);
    }
};

/*######
## npc_iron_construct
######*/

struct npc_iron_constructAI : public ScriptedAI
{
    npc_iron_constructAI(Creature* creature) : ScriptedAI(creature)
    {
        AddCustomAction(0, true, [&]()
        {
            m_creature->SetImmuneToPlayer(false);
            instance_ulduar* instance = dynamic_cast<instance_ulduar*>(m_creature->GetInstanceData());
            if (!instance)
                return;
            Creature* ignis = instance->GetSingleCreatureFromStorage(NPC_IGNIS);
            if (!ignis || !ignis->AI())
                return;
            ignis->CastSpell(m_creature, SPELL_STRENGTH, TRIGGERED_OLD_TRIGGERED);
            if (ignis->GetVictim())
                AttackStart(ignis->GetVictim());
        }, TIMER_COMBAT_OOC);
    }

    void JustRespawned() override
    {
        DoCastSpellIfCan(nullptr, SPELL_STONED, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_CONSTRUCT_HITTING_YA, CAST_TRIGGERED);
    }
};

/*######
## npc_scorch
######*/

struct npc_scorchAI : public ScriptedAI
{
    npc_scorchAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
        SetMeleeEnabled(false);
        SetCombatMovement(false);
        m_creature->SetImmobilizedState(true, true);
        SetReactState(REACT_PASSIVE);
        m_creature->ForcedDespawn(50 * IN_MILLISECONDS);
    }
};

// 62488 - Activate Construct
struct ActivateConstruct : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target || !target->HasAura(SPELL_STONED))
            return false;
        return true;
    }

    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || !unitTarget->AI())
            return;
        unitTarget->AI()->ResetTimer(0, 3s);
        unitTarget->RemoveAurasDueToSpell(SPELL_STONED);
        unitTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }
};

// 62707 - Grab
struct IgnisGrab : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;
        unitTarget->CastSpell(nullptr, SPELL_GRAB_POS1, TRIGGERED_OLD_TRIGGERED);
    }
};

// 62717 - Slag Pot
struct SlagPot : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        if (!caster || !target)
            return;
        caster->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? SPELL_SLAG_POT_PERIODIC : SPELL_SLAG_POT_PERIODIC_H, TRIGGERED_OLD_TRIGGERED, nullptr, aura);

        if (aura->GetAuraTicks() == 10)
            target->CastSpell(target, SPELL_SLAG_IMBUED, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
    }
};

// 62548, 63476 - Scorch
struct IgnisScorch : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        if (aura->GetAuraTicks() != 4)
            return;
        Unit* caster = aura->GetCaster();
        caster->CastSpell(nullptr, SPELL_SCORCH_SUMMON, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    }
};

// 65667 - Heat
struct IgnisHeat : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (!target || target->GetEntry() != NPC_IRON_CONSTRUCT)
            return false;
        if (target->HasAura(SPELL_STONED) || target->HasAura(SPELL_BRITTLE) || target->HasAura(SPELL_BRITTLE_H))
            return false;
        if (target->IsImmuneToPlayer() || target->IsImmuneToNPC())
            return false;
        if (target->HasAura(SPELL_MOLTEN))
        {
            target->CastSpell(target, SPELL_MOLTEN, TRIGGERED_OLD_TRIGGERED);
            return false;
        }
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        if (target->GetAuraCount(SPELL_HEAT) >= MAX_HEAT_STACKS - 1)
            target->CastSpell(target, SPELL_MOLTEN, TRIGGERED_OLD_TRIGGERED);
    }
};

// 64503 - Water
struct IgnisWater : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Creature* target = (spell->GetUnitTarget() && spell->GetUnitTarget()->IsCreature()) ? static_cast<Creature*>(spell->GetUnitTarget()) : nullptr;
        if (!target)
            return;
        switch (target->GetEntry())
        {
            case NPC_SCORCH:
            {
                DoBroadcastText(EMOTE_EXTINGUISH_SCORCH, target);
                target->ForcedDespawn();
                break;
            }
            case NPC_IRON_CONSTRUCT:
            {
                if (target->HasAura(SPELL_MOLTEN))
                    target->CastSpell(nullptr, SPELL_CHILL, TRIGGERED_OLD_TRIGGERED);
                break;
            }
            default: break;
        }
    }
};

// 62382, 67114 - Brittle
struct IgnisBrittle : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (procData.damage < 5000)
            return SPELL_AURA_PROC_CANT_TRIGGER;
        Creature* creatureTarget = (aura->GetTarget() && aura->GetTarget()->IsCreature()) ? static_cast<Creature*>(aura->GetTarget()) : nullptr;
        if (!creatureTarget || !creatureTarget->AI())
            return SPELL_AURA_PROC_FAILED;

        creatureTarget->RemoveAllAurasOnDeath();

        creatureTarget->AI()->DoCastSpellIfCan(nullptr, SPELL_SHATTER, CAST_TRIGGERED);
        creatureTarget->AI()->DoCastSpellIfCan(nullptr, SPELL_STRENGTH_REMOVE, CAST_TRIGGERED);

        creatureTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        creatureTarget->AI()->SetMeleeEnabled(false);
        creatureTarget->AI()->SetCombatMovement(false);
        creatureTarget->SetImmuneToPlayer(true);
        creatureTarget->SetImmuneToNPC(true);
        creatureTarget->GetMotionMaster()->MoveIdle();
        creatureTarget->SetImmobilizedState(true, true);
        return SPELL_AURA_PROC_OK;
    }
};

// 64475 - Strength of the Creator
struct RemoveStrengthOfTheCreator : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        target->RemoveAuraHolderFromStack(SPELL_STRENGTH_BUFF);
    }
};

void AddSC_boss_ignis()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ignis";
    pNewScript->GetAI = &GetNewAIInstance<boss_ignisAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_iron_construct";
    pNewScript->GetAI = &GetNewAIInstance<npc_iron_constructAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scorch";
    pNewScript->GetAI = &GetNewAIInstance<npc_scorchAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ActivateConstruct>("spell_activate_construct");
    RegisterSpellScript<IgnisGrab>("spell_ignis_grab");
    RegisterSpellScript<SlagPot>("spell_slag_pot");
    RegisterSpellScript<IgnisScorch>("spell_ignis_scorch");
    RegisterSpellScript<IgnisHeat>("spell_ignis_heat");
    RegisterSpellScript<IgnisWater>("spell_ignis_water");
    RegisterSpellScript<IgnisBrittle>("spell_ignis_brittle");
    RegisterSpellScript<RemoveStrengthOfTheCreator>("spell_ignis_remove_strength");
}
