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
#include "Spells/SpellMgr.h"

// 11213 - Arcane Concentration
struct ArcaneConcentration : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (Spell* spell = procData.spell)
        {
            if (IsChanneledSpell(spell->m_spellInfo))
                return false; // these never proc
            if (SpellEntry const* spellInfo = spell->GetTriggeredByAuraSpellInfo())
            {
                if (IsChanneledSpell(spellInfo))
                {
                    if (Spell* channeledSpell = spell->GetCaster()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    {
                        if (channeledSpell->IsAuraProcced(aura))
                            return false;

                        channeledSpell->RegisterAuraProc(aura);
                    }
                }
            }
            spell->RegisterAuraProc(aura);
        }
        return true;
    }
};

// 11170 - Shatter
struct ShatterMage : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* spellInfo) const override
    {
        switch (aura->GetModifier()->m_miscvalue)
        {
            // Shatter
            case 849: if (target->isFrozen() || aura->GetTarget()->IsIgnoreUnitState(spellInfo, IGNORE_UNIT_TARGET_NON_FROZEN)) chance += 17.0f; break;
            case 910: if (target->isFrozen() || aura->GetTarget()->IsIgnoreUnitState(spellInfo, IGNORE_UNIT_TARGET_NON_FROZEN)) chance += 34.0f; break;
            case 911: if (target->isFrozen() || aura->GetTarget()->IsIgnoreUnitState(spellInfo, IGNORE_UNIT_TARGET_NON_FROZEN)) chance += 50.0f; break;
            default:
                break;
        }
    }
};

// 42208 - Blizzard
struct Blizzard : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (spell->GetCaster()->HasOverrideScript(836)) // Improved Blizzard (Rank 1)
            spell->AddPrecastSpell(12484);
        if (spell->GetCaster()->HasOverrideScript(988)) // Improved Blizzard (Rank 2)
            spell->AddPrecastSpell(12485);
        if (spell->GetCaster()->HasOverrideScript(989)) // Improved Blizzard (Rank 3)
            spell->AddPrecastSpell(12486);
    }
};

// 31679 - Molten Fury
struct MoltenFury : public AuraScript
{
    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (victim->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
            totalMod *= (100.0f + aura->GetModifier()->m_amount) / 100.0f;
    }
};

// 30455 - Ice Lance
struct IceLance : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (target->isFrozen() || spell->GetCaster()->IsIgnoreUnitState(spell->m_spellInfo, IGNORE_UNIT_TARGET_NON_FROZEN))
        {
            float multiplier = 3.0f;

            // if target have higher level
            if (target->GetLevel() > spell->GetCaster()->GetLevel())
                // Glyph of Ice Lance
                if (Aura* glyph = spell->GetCaster()->GetDummyAura(56377))
                    multiplier = glyph->GetModifier()->m_amount;

            spell->SetDamageDoneModifier(multiplier, EFFECT_INDEX_0);
        }
    }
};

struct MageIgnite : public AuraScript
{
    // implemented this way because we do not support proccing in spellscript on empty aura slot
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        SpellAuraHolder* holder = caster->GetSpellAuraHolder(31658);
        if (!holder)
            holder = caster->GetSpellAuraHolder(31657);
        if (!holder)
            holder = caster->GetSpellAuraHolder(31656);
        if (!holder)
            return;

        uint32 chance = holder->GetSpellProto()->procChance;
        if (roll_chance_f(chance))
        {
            uint32 baseMana = caster->GetCreateMana();
            int32 gainedMana = baseMana * 2 / 100;
            caster->CastCustomSpell(nullptr, 67545, &gainedMana, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct FingersOfFrostProc : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        return roll_chance_i(aura->GetAmount());
    }
};

struct FingersOfFrostIgnore : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->AddPrecastSpell(74396);
    }
};

struct FingersOfFrostDummy : public AuraScript
{
    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const override
    {
        holder->PresetAuraStacks(holder->GetSpellProto()->StackAmount);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() != AURA_REMOVE_BY_GAINED_STACK)
            aura->GetTarget()->RemoveAurasDueToSpell(44544);
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& /*procData*/) const override
    {
        aura->GetTarget()->RemoveAuraStack(aura->GetId());
        return SPELL_AURA_PROC_OK;
    }
};

struct DeepFreezeImmunityState : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (data.victim->IsCreature())
            if (static_cast<Creature*>(data.victim)->GetCreatureInfo()->MechanicImmuneMask & (1 << (MECHANIC_STUN - 1)))
                return true;
        return false;
    }
};

// 118 - Polymorph
struct Polymorph : public AuraScript
{
    uint32 GetAuraScriptCustomizationValue(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            if (caster->HasAura(52648)) // Glyph of the Penguin
                return 26452;
        return 0;
    }
};

// 6143 - Frost Ward, 543 - Fire Ward
struct FrostWarding : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        int chance = 0;
        if (Aura* frostWarding = aura->GetTarget()->GetAura(11189, EFFECT_INDEX_0))
            chance = frostWarding->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
        if (Aura* frostWarding = aura->GetTarget()->GetAura(28332, EFFECT_INDEX_0))
            chance = frostWarding->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
        if (roll_chance_i(chance))
        {
            int32 amount = remainingDamage;
            remainingDamage = 0;
            currentAbsorb = 0;

            // Frost Warding (mana regen)
            aura->GetTarget()->CastCustomSpell(nullptr, 57776, &amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
        }
    }
};

// 56369 - Glyph of Fire Blast
struct GlyphOfFireBlast : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* spellInfo) const override
    {
        if (target->IsStunned()) chance += aura->GetModifier()->m_amount;
    }
};


void LoadMageScripts()
{
    RegisterSpellScript<ArcaneConcentration>("spell_arcane_concentration");
    RegisterSpellScript<ShatterMage>("spell_shatter_mage");
    RegisterSpellScript<Blizzard>("spell_blizzard");
    RegisterSpellScript<MoltenFury>("spell_molten_fury");
    RegisterSpellScript<IceLance>("spell_ice_lance");
    RegisterSpellScript<MageIgnite>("spell_mage_ignite");
    RegisterSpellScript<FingersOfFrostProc>("spell_fingers_of_frost_proc");
    RegisterSpellScript<FingersOfFrostIgnore>("spell_fingers_of_frost_ignore");
    RegisterSpellScript<FingersOfFrostDummy>("spell_fingers_of_frost_dummy");
    RegisterSpellScript<DeepFreezeImmunityState>("spell_deep_freeze_immunity_state");
    RegisterSpellScript<Polymorph>("spell_polymorph");
    RegisterSpellScript<FrostWarding>("spell_frost_warding");
    RegisterSpellScript<GlyphOfFireBlast>("spell_glyph_of_fire_blast");
}