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
        // ignores absorb - has to respect stuff like mitigation and partial resist
        int32 swdDamage = spell->GetTotalTargetDamage() + spell->GetTotalTargetAbsorb();
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
                if (aura->GetModifier()->m_amount > (int32)value)
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

struct PowerWordShield : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster && data.caster->IsPlayer())
        {
            if (Aura* borrowedTime = static_cast<Player*>(data.caster)->GetKnownTalentRankAuraById(1202, EFFECT_INDEX_1))
                value += (borrowedTime->GetAmount() * data.caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(data.spellProto)) / 100);
        }
        return value;
    }
};

enum LightwellData
{
    NPC_PRIEST_LIGHTWELL_1 = 31897,
    NPC_PRIEST_LIGHTWELL_2 = 31896,
    NPC_PRIEST_LIGHTWELL_3 = 31895,
    NPC_PRIEST_LIGHTWELL_4 = 31894,
    NPC_PRIEST_LIGHTWELL_5 = 31893,
    NPC_PRIEST_LIGHTWELL_6 = 31883,

    SPELL_PRIEST_LIGHTWELL_RENEW_R1 = 7001,
    SPELL_PRIEST_LIGHTWELL_RENEW_R2 = 27873,
    SPELL_PRIEST_LIGHTWELL_RENEW_R3 = 27874,
    SPELL_PRIEST_LIGHTWELL_RENEW_R4 = 28276,
    SPELL_PRIEST_LIGHTWELL_RENEW_R5 = 48084,
    SPELL_PRIEST_LIGHTWELL_RENEW_R6 = 48085,

    SPELL_PRIEST_LIGHTWELL_CHARGES = 59907,
};

struct LightwellRenew : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->SetScriptValue(aura->GetTarget()->GetMaxHealth() * 30 / 100); // set at 30% hp of target
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        // proc until 30% of hp as damage
        uint64 remainingDamage = aura->GetScriptValue();
        if (remainingDamage > procData.damage)
            remainingDamage -= procData.damage;
        else
            remainingDamage = 0;
        aura->SetScriptValue(remainingDamage);
        return remainingDamage ? SPELL_AURA_PROC_FAILED : SPELL_AURA_PROC_OK;
    }
};

struct LightwellRelay : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Creature* caster = dynamic_cast<Creature*>(spell->GetCaster());
        if (!caster || !caster->IsTemporarySummon())
            return;

        uint32 lightwellRenew = 0;
        switch (caster->GetEntry())
        {
            case NPC_PRIEST_LIGHTWELL_1:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R1;
                break;
            case NPC_PRIEST_LIGHTWELL_2:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R2;
                break;
            case NPC_PRIEST_LIGHTWELL_3:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R3;
                break;
            case NPC_PRIEST_LIGHTWELL_4:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R4;
                break;
            case NPC_PRIEST_LIGHTWELL_5:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R5;
                break;
            case NPC_PRIEST_LIGHTWELL_6:
                lightwellRenew = SPELL_PRIEST_LIGHTWELL_RENEW_R6;
                break;
            default:
                return;
        }

        // proc a spellcast
        if (SpellAuraHolder* chargesHolder = caster->GetSpellAuraHolder(SPELL_PRIEST_LIGHTWELL_CHARGES))
        {
            caster->CastSpell(spell->GetUnitTarget(), lightwellRenew, TRIGGERED_NONE, nullptr, nullptr, caster->GetSpawnerGuid());
            if (chargesHolder->DropAuraCharge())
                caster->ForcedDespawn();
        }
    }
};

struct GlyphOfLightwell : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*victim*/, int32& advertisedBenefit, float& totalMod) const override
    {
        advertisedBenefit += aura->GetModifier()->m_amount / 3; // ticks 3 times
    }
};

struct GlyphOfShadowWordDeath : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* victim, int32& advertisedBenefit, float& totalMod) const override
    {
        if (victim->GetHealthPercent() <= 35.f)
            totalMod *= (float(100 + aura->GetModifier()->m_amount) / 100);
    }
};

struct ShadowAffinityDots : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DISPEL)
        {
            if (Player* caster = dynamic_cast<Player*>(aura->GetCaster()))
            {
                if (Aura* aura = caster->GetKnownTalentRankAuraById(466, EFFECT_INDEX_1))
                {
                    int32 basepoints0 = aura->GetModifier()->m_amount * caster->GetCreateMana() / 100;
                    caster->CastCustomSpell(nullptr, 64103, &basepoints0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                }
            }
        }
    }
};

void LoadPriestScripts()
{
    RegisterSpellScript<PowerInfusion>("spell_power_infusion");
    RegisterSpellScript<ShadowWordDeath>("spell_shadow_word_death");
    RegisterSpellScript<SpiritOfRedemptionHeal>("spell_spirit_of_redemption_heal");
    RegisterSpellScript<PrayerOfMending>("spell_prayer_of_mending");
    RegisterSpellScript<PainSuppression>("spell_pain_suppression");
    RegisterSpellScript<Shadowfiend>("spell_shadowfiend");
    RegisterSpellScript<DivineHymn>("spell_divine_hymn");
    RegisterSpellScript<HymnOfHope>("spell_hymn_of_hope");
    RegisterSpellScript<CircleOfHealing>("spell_circle_of_healing");
    RegisterSpellScript<PowerWordShield>("spell_power_word_shield");
    RegisterSpellScript<LightwellRenew>("spell_lightwell_renew");
    RegisterSpellScript<LightwellRelay>("spell_lightwell_relay");
    RegisterSpellScript<GlyphOfLightwell>("spell_glyph_of_lightwell");
    RegisterSpellScript<GlyphOfShadowWordDeath>("spell_glyph_of_shadow_word_death");
    RegisterSpellScript<ShadowAffinityDots>("spell_shadow_affinity_dots");
}
