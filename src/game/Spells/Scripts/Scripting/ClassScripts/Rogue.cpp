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

// 14185 - Preparation
struct Preparation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->GetCaster()->IsPlayer())
        {
            // immediately finishes the cooldown on certain Rogue abilities
            auto cdCheck = [](SpellEntry const & spellEntry) -> bool { return (spellEntry.SpellFamilyName == SPELLFAMILY_ROGUE && (spellEntry.SpellFamilyFlags & uint64(0x0000024000000860))); };
            static_cast<Player*>(spell->GetCaster())->RemoveSomeCooldown(cdCheck);
        }
    }
};

enum
{
    SPELL_DISTRACT      = 1725,
    SPELL_EARTHBIND     = 3600,
    SPELL_MASS_DISPEL   = 39897,
    SPELL_MASS_DISPEL_2 = 32592,
};

// Warning: Also currently used by Prowl
// 1784 - Stealth
struct Stealth : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override // per 1.12.0 patch notes - no other indication of how it works
    {
        if (data.spell)
        {
            switch (data.spell->m_spellInfo->Id)
            {
                case SPELL_DISTRACT:
                case SPELL_EARTHBIND:
                case SPELL_MASS_DISPEL:
                case SPELL_MASS_DISPEL_2:
                    return false;
            }
        }
        return true;
    }
};

void CastHighestStealthRank(Unit* caster)
{
    if (!caster->IsPlayer())
        return;

    // get highest rank of the Stealth spell
    uint32 spellId = static_cast<Player*>(caster)->LookupHighestLearnedRank(1784);
    if (!spellId)
        return;
    SpellEntry const* stealthSpellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    // no Stealth spell found
    if (!stealthSpellEntry)
        return;

    // reset cooldown on it if needed
    if (!caster->IsSpellReady(*stealthSpellEntry))
        caster->RemoveSpellCooldown(*stealthSpellEntry);

    caster->CastSpell(nullptr, stealthSpellEntry, TRIGGERED_OLD_TRIGGERED);
}

// 1856 - Vanish
struct VanishRogue : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        CastHighestStealthRank(spell->GetCaster());
    }
};

// 13983 - Setup
struct SetupRogue : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        return data.victim->GetTarget() == data.attacker;
    }
};

// 14082 - Dirty Deeds
struct DirtyDeeds : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_MELEE_DAMAGE_DONE, apply);
        else if (aura->GetEffIndex() == EFFECT_INDEX_2)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_MELEE_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            return;

        if (victim->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
        {
            Aura* eff0 = aura->GetHolder()->m_auras[EFFECT_INDEX_0];
            if (!eff0)
            {
                sLog.outError("Spell structure of DD (%u) changed.", aura->GetId());
                return;
            }

            // effect 0 have expected value but in negative state
            totalMod *= (-eff0->GetModifier()->m_amount + 100.0f) / 100.0f;
        }
    }
};

struct KillingSpreeStorage : public ScriptStorage
{
    GuidSet targets;
};

// 51690 - Killing Spree
struct KillingSpree : public SpellScript, public AuraScript
{
    void OnAuraInit(Aura* aura) const override
    {
        // this is likely emulation due to us killing off spell after it ends even though auras still exist
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->SetScriptStorage(new KillingSpreeStorage());
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        Unit* target = aura->GetTarget();
        target->CastSpell(nullptr, 61851, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1 || !spell->GetUnitTarget())
            return;
        // aura is self effect so this must be executed after in all cases
        if (Aura* aura = spell->GetCaster()->GetAura(spell->m_spellInfo->Id, EFFECT_INDEX_0))
            if (KillingSpreeStorage* storage = dynamic_cast<KillingSpreeStorage*>(aura->GetScriptStorage()))
                storage->targets.insert(spell->GetUnitTarget()->GetObjectGuid());
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        if (KillingSpreeStorage* storage = dynamic_cast<KillingSpreeStorage*>(aura->GetScriptStorage()))
        {
            Unit* target = aura->GetTarget();
            Unit* victim = nullptr;
            std::vector<Unit*> eligibleUnits;
            for (ObjectGuid guid : storage->targets)
                if (Unit* unit = target->GetMap()->GetUnit(guid))
                    if (unit->IsAlive() && target->CanAttackSpell(unit) && target->IsWithinCombatDistInMap(unit, 10.f))
                        eligibleUnits.push_back(unit);

            if (eligibleUnits.size() > 0)
                victim = eligibleUnits[urand(0, eligibleUnits.size() - 1)];

            if (victim)
            {
                target->CastSpell(victim, 57840, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
                target->CastSpell(victim, 57841, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            }
        }
    }
};

struct PreyOnTheWeak : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        Unit* victim = target->GetTarget();
        if (victim && target->GetHealthPercent() > victim->GetHealthPercent())
        {
            Aura* buff = target->GetAura(58670, EFFECT_INDEX_0);
            if (!buff || buff->GetAmount() < aura->GetAmount())
            {
                int32 basepoints = aura->GetAmount();
                target->CastCustomSpell(nullptr, 58670, &basepoints, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
            }
        }
        else
            target->RemoveAurasDueToSpell(58670);
    }
};

void LoadRogueScripts()
{
    RegisterSpellScript<Preparation>("spell_preparation");
    RegisterSpellScript<Stealth>("spell_stealth");
    RegisterSpellScript<VanishRogue>("spell_vanish");
    RegisterSpellScript<SetupRogue>("spell_setup_rogue");
    RegisterSpellScript<DirtyDeeds>("spell_dirty_deeds");
    RegisterSpellScript<KillingSpree>("spell_killing_spree");
    RegisterSpellScript<PreyOnTheWeak>("spell_prey_on_the_weak");
}