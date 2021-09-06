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

struct SpiritOfRedemptionHeal : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
            spell->SetDamage(spell->GetCaster()->GetMaxHealth());
    }
};

struct PowerInfusion : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        // Patch 1.10.2 (2006-05-02):
        // Power Infusion: This aura will no longer stack with Arcane Power. If you attempt to cast it on someone with Arcane Power, the spell will fail.
        if (Unit* target = spell->m_targets.getUnitTarget())
            if (target->GetAuraCount(12042))
                return SPELL_FAILED_AURA_BOUNCED;

        return SPELL_CAST_OK;
    }
};

struct ShadowWordDeath : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        int32 swdDamage = spell->GetTotalTargetDamage();
        spell->GetCaster()->CastCustomSpell(nullptr, 32409, &swdDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

enum
{
    MANA_LEECH_PASSIVE = 28305,
};

struct Shadowfiend : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(summon, MANA_LEECH_PASSIVE, TRIGGERED_OLD_TRIGGERED);
        summon->AI()->AttackStart(spell->m_targets.getUnitTarget());
    }
};

struct PrayerOfMending : public SpellScript
{
    // not needed in wotlk
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;
        if (strict)
        {
            if (Aura* aura = target->GetAura(41635, EFFECT_INDEX_0))
            {
                uint32 value = 0;
                value = spell->CalculateSpellEffectValue(EFFECT_INDEX_0, target, true, false);
                value = spell->GetCaster()->SpellHealingBonusDone(target, sSpellTemplate.LookupEntry<SpellEntry>(41635), value, HEAL);
                if (aura->GetModifier()->m_amount > value)
                    return SPELL_FAILED_AURA_BOUNCED;
            }
        }
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        uint32 value = spell->GetDamage();
        value = spell->GetCaster()->SpellHealingBonusDone(spell->GetUnitTarget(), sSpellTemplate.LookupEntry<SpellEntry>(41635), value, HEAL);
        spell->SetDamage(value);
    }
};

enum
{
    SPELL_PAIN_SUPPRESSION_THREAT_REDUCTION = 44416,
};

struct PainSuppression : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(aura->GetTarget(), SPELL_PAIN_SUPPRESSION_THREAT_REDUCTION, TRIGGERED_OLD_TRIGGERED, nullptr, aura, aura->GetCasterGuid());
    }
};

struct DivineHymn : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        spell->SetMaxAffectedTargets(3);
        if (SpellEntry const* spellInfo = spell->GetTriggeredByAuraSpellInfo())
            if (Aura* aura = caster->GetAura(spellInfo->Id, EFFECT_INDEX_1))
                spell->SetMaxAffectedTargets(aura->GetAmount());
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_PRIORITIZE_HEALTH);
        spell->SetFilteringScheme(EFFECT_INDEX_1, true, SCHEME_PRIORITIZE_HEALTH);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return spell->GetCaster()->IsInGroup(target);
    }
};

struct HymnOfHope : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        spell->SetMaxAffectedTargets(3);
        if (SpellEntry const* spellInfo = spell->GetTriggeredByAuraSpellInfo())
            if (Aura* aura = caster->GetAura(spellInfo->Id, EFFECT_INDEX_1))
                spell->SetMaxAffectedTargets(aura->GetAmount());
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_PRIORITIZE_MANA);
        spell->SetFilteringScheme(EFFECT_INDEX_1, true, SCHEME_PRIORITIZE_MANA);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return spell->GetCaster()->IsInGroup(target);
    }
};

struct CircleOfHealing : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        uint32 count = 5;
        // Glyph of Circle of Healing
        if (Aura const* glyph = caster->GetDummyAura(55675))
            count += glyph->GetModifier()->m_amount;
        spell->SetMaxAffectedTargets(count);
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_PRIORITIZE_HEALTH);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return spell->GetCaster()->IsInGroup(target);
    }
};

void LoadPriestScripts()
{
    RegisterSpellScript<PowerInfusion>("spell_power_infusion");
    RegisterSpellScript<ShadowWordDeath>("spell_shadow_word_death");
    RegisterSpellScript<SpiritOfRedemptionHeal>("spell_spirit_of_redemption_heal");
    RegisterSpellScript<PrayerOfMending>("spell_prayer_of_mending");
    RegisterAuraScript<PainSuppression>("spell_pain_suppression");
    RegisterSpellScript<Shadowfiend>("spell_shadowfiend");
    RegisterSpellScript<DivineHymn>("spell_divine_hymn");
    RegisterSpellScript<HymnOfHope>("spell_hymn_of_hope");
    RegisterSpellScript<CircleOfHealing>("spell_circle_of_healing");
}
