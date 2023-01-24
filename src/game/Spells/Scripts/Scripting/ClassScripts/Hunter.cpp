/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
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

#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Spells/SpellMgr.h"

struct Entrapment : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        if (procData.spell)
            procData.triggerOriginalCaster = procData.spell->GetTrueCaster()->GetObjectGuid();
        return SPELL_AURA_PROC_OK;
    }
};

struct KillCommand : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        if (spell->GetCaster()->HasAura(37483)) // Improved Kill Command - Item set bonus
            spell->GetCaster()->CastSpell(nullptr, 37482, TRIGGERED_OLD_TRIGGERED);// Exploited Weakness
    }
};

struct Misdirection : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        // Patch 2.3.0 (2007-11-13):
        // Misdirection: If a Hunter attempts to use this ability on a target which already has an active Misdirection, the spell will fail to apply due to a more powerful spell already being in effect.
        if (Unit* target = spell->m_targets.getUnitTarget())
        {
            if (target->HasAura(35079))
                return SPELL_FAILED_AURA_BOUNCED;
            if (target->IsMounted())
                return SPELL_FAILED_NOT_ON_MOUNTED;
        }

        return SPELL_CAST_OK;
    }
};

struct ExposeWeakness : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster)
            value = (data.caster->GetStat(STAT_AGILITY) * value) / 100;

        return value;
    }
};

struct WyvernSting : public AuraScript
{
    int32 OnDurationCalculate(WorldObject const* caster, Unit const* target, int32 duration) const override
    {
        // PVP DR example
        if (target && caster->IsControlledByPlayer() && target->IsPlayerControlled())
            return 6000;
        return duration;
    }
};

struct Disengage : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->GetCaster()->IsInCombat())
            return SPELL_FAILED_CASTER_AURASTATE;

        return SPELL_CAST_OK;
    }
};

struct RoarOfSacrifice : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        Unit* pet = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!pet)
            return SPELL_AURA_PROC_OK;

        int32 damage = procData.damage;
        target->CastCustomSpell(pet, 67481, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return SPELL_AURA_PROC_OK;
    }
};

struct RapidRecuperationPeriodic : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        Unit* target = aura->GetTarget();
        data.basePoints[0] = target->GetPower(POWER_MANA) * aura->GetAmount() / 100;
    }
};

struct RapidKilling : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            return;

        Unit* target = aura->GetTarget();
        if (Player* player = dynamic_cast<Player*>(target))
        {
            if (Aura* aura = player->GetKnownTalentRankAuraById(2131, EFFECT_INDEX_1)) // Rapid Recuperation talent
            {
                uint32 spellId = 0;
                switch (aura->GetId())
                {
                    case 53228: spellId = 56654; break; // Rank 1
                    case 53232: spellId = 58882; break; // Rank 2
                }
                target->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

struct ExplosiveShot : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster)
            value += int32(data.caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 14 / 100);
        return value;
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
        {
            int32 amount = aura->GetAmount();
            caster->CastCustomSpell(aura->GetTarget(), 53352, &amount, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG, nullptr, aura);
        }
    }
};

struct LockAndLoad : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        if (aura->GetTarget()->HasAura(67544)) // Lock and Load Marker
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (!procData.spellInfo || aura->GetEffIndex() != EFFECT_INDEX_0)
            return SPELL_AURA_PROC_OK;

        if (procData.spellInfo->IsFitToFamilyMask(0x00000018) && (procData.procFlags & PROC_FLAG_ON_TRAP_ACTIVATION) != 0)
            if (!roll_chance_i(aura->GetAmount())) // eff 0 chance
                return SPELL_AURA_PROC_FAILED;

        if (procData.spellInfo->IsFitToFamilyMask(0x0800000000000000, 0x00024000) && (procData.procFlags & PROC_FLAG_DEAL_HARMFUL_PERIODIC) != 0)
            if (Aura* secondEffectAura = aura->GetHolder()->m_auras[EFFECT_INDEX_1]) // should never fail
                if (!roll_chance_i(secondEffectAura->GetAmount())) // eff 0 chance
                    return SPELL_AURA_PROC_FAILED;

        return SPELL_AURA_PROC_OK;
    }
};

struct LockAndLoadTrigger : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->AddPrecastSpell(67544);
    }
};

void LoadHunterScripts()
{
    RegisterSpellScript<Entrapment>("spell_entrapment");
    RegisterSpellScript<KillCommand>("spell_kill_command");
    RegisterSpellScript<Misdirection>("spell_misdirection");
    RegisterSpellScript<ExposeWeakness>("spell_expose_weakness");
    RegisterSpellScript<WyvernSting>("spell_wyvern_sting");
    RegisterSpellScript<Disengage>("spell_disengage");
    RegisterSpellScript<RoarOfSacrifice>("spell_roar_of_sacrifice");
    RegisterSpellScript<RapidRecuperationPeriodic>("spell_rapid_recuperation_periodic");
    RegisterSpellScript<RapidKilling>("spell_rapid_killing");
    RegisterSpellScript<ExplosiveShot>("spell_explosive_shot");
    RegisterSpellScript<LockAndLoad>("spell_lock_and_load");
    RegisterSpellScript<LockAndLoadTrigger>("spell_lock_and_load_trigger");
}
