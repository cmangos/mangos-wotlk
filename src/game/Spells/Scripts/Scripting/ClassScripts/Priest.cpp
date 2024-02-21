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

// 27827 - Spirit of Redemption
struct SpiritOfRedemptionHeal : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
            spell->SetDamage(spell->GetCaster()->GetMaxHealth());
    }
};

// 10060 - Power Infusion
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

// 32379 - Shadow Word: Death
struct ShadowWordDeath : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        // ignores absorb - has to respect stuff like mitigation and partial resist
        int32 swdDamage = spell->GetTotalTargetDamage() + spell->GetTotalTargetAbsorb();
        if (Aura* painAndSuffering = spell->GetCaster()->GetAura(47580, EFFECT_INDEX_1))
            swdDamage *= (painAndSuffering->GetModifier()->m_amount + 100.0f) / 100.0f;
        if (Aura* painAndSuffering = spell->GetCaster()->GetAura(47581, EFFECT_INDEX_1))
            swdDamage *= (painAndSuffering->GetModifier()->m_amount + 100.0f) / 100.0f;
        if (Aura* painAndSuffering = spell->GetCaster()->GetAura(47582, EFFECT_INDEX_1))
            swdDamage *= (painAndSuffering->GetModifier()->m_amount + 100.0f) / 100.0f;
        spell->GetCaster()->CastCustomSpell(nullptr, 32409, &swdDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

enum
{
    MANA_LEECH_PASSIVE = 28305,
    SPELL_SHADOWFIEND_DEATH = 57989,
};

// 34433 - Shadowfiend
struct Shadowfiend : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(summon, MANA_LEECH_PASSIVE, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(nullptr, SPELL_SHADOWFIEND_DEATH, TRIGGERED_OLD_TRIGGERED);
        summon->AI()->AttackStart(spell->m_targets.getUnitTarget());
    }
};

// 33076 - Prayer of Mending
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
                value = spell->GetCaster()->SpellHealingBonusDone(target, sSpellTemplate.LookupEntry<SpellEntry>(41635), EFFECT_INDEX_0, value, HEAL);
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
        value = spell->GetCaster()->SpellHealingBonusDone(spell->GetUnitTarget(), sSpellTemplate.LookupEntry<SpellEntry>(41635), effIdx, value, HEAL);
        spell->SetDamage(value);
    }
};

enum
{
    SPELL_PAIN_SUPPRESSION_THREAT_REDUCTION = 44416,
};

// 33206 - Pain Suppression
struct PainSuppression : public SpellScript, public AuraScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetUsableWhileStunned(spell->GetCaster()->HasAura(63248)); // Glyph of Pain Suppression
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(aura->GetTarget(), SPELL_PAIN_SUPPRESSION_THREAT_REDUCTION, TRIGGERED_OLD_TRIGGERED, nullptr, aura, aura->GetCasterGuid());
    }
};

// 17 - Power Word: Shield
struct PowerWordShieldPriest : public AuraScript
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

    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& reflectDamage, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        Unit* caster = aura->GetTarget();
        Unit::AuraList const& vOverRideCS = caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (auto k : vOverRideCS) // 33201 - Reflective Shield
        {
            switch (k->GetModifier()->m_miscvalue)
            {
                case 5065:                      // Rank 1
                case 5064:                      // Rank 2
                {
                    if (remainingDamage >= currentAbsorb)
                        reflectDamage = k->GetModifier()->m_amount * currentAbsorb / 100;
                    else
                        reflectDamage = k->GetModifier()->m_amount * remainingDamage / 100;
                    reflectedSpellId = 33619;
                } break;
                default: break;
            }
        }
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        if (!apply || !caster || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        // Glyph of Power Word: Shield
        if (Aura* glyph = caster->GetAura(55672, EFFECT_INDEX_0))
        {
            int32 heal = (glyph->GetModifier()->m_amount * aura->GetAmount()) / 100;
            caster->CastCustomSpell(aura->GetTarget(), 56160, &heal, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

// 64844 - Divine Hymn
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

// 64904 - Hymn of Hope
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

// 34861 - Circle of Healing
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

// 7001 - Lightwell Renew
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

// 60123 - Lightwell Renew
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

// 55673 - Glyph of Lightwell
struct GlyphOfLightwell : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* /*victim*/, int32& advertisedBenefit, float& totalMod) const override
    {
        advertisedBenefit += aura->GetModifier()->m_amount / 3; // ticks 3 times
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

// 47788 - Guardian Spirit
struct GuardianSpiritPriest : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& /*currentAbsorb*/, int32& /*remainingDamage*/, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& preventedDeath, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        preventedDeath = true;
    }

    void OnAuraDeathPrevention(Aura* aura, int32& remainingDamage) const override
    {
        int32 healAmount = aura->GetTarget()->GetMaxHealth() * aura->GetModifier()->m_amount / 100;
        aura->GetTarget()->CastCustomSpell(nullptr, 48153, &healAmount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        aura->GetTarget()->RemoveAurasDueToSpell(aura->GetSpellProto()->Id);
        remainingDamage = 0;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            if (Unit* caster = aura->GetCaster())
                if (caster->IsPlayer())
                    if (Aura const* glyph = caster->GetAura(63231, EFFECT_INDEX_0))
                        static_cast<Player*>(caster)->ModifyCooldown(47788, glyph->GetAmount() * 1000);
        }
    }
};

// 57470 - Renewed Hope
struct RenewedHope : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* spellInfo) const override
    {
        if (target->HasAura(6788)) chance += aura->GetModifier()->m_amount; // Weakened Soul
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.cooldown = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);
        return SPELL_AURA_PROC_OK;
    }
};

// 63944 - RenewedHope
struct RenewedHopeDamageTaken : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_MELEE_DAMAGE_TAKEN, apply);
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_TAKEN, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* /*victim*/, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        totalMod *= (aura->GetAmount() + 100) / 100;
    }
};

// 63504 - Improved Flash Heal
struct ImprovedFlashHeal : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* spellInfo) const override
    {
        if (target->GetHealthPercent() <= 50.f) chance += aura->GetModifier()->m_amount;
    }
};

// 47573 - TwistedFaith
struct TwistedFaith : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        // Shadow Word: Pain
        if (victim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, uint64(0x0000000000008000), 0, aura->GetTarget()->GetObjectGuid()))
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 55682 - Glyph of Shadow Word: Death
struct GlyphOfShadowWordDeath : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        SpellAuraHolder* holder = aura->GetHolder();
        Aura const* hpPct = holder->GetAuraByEffectIndex(EFFECT_INDEX_0);
        Aura const* dmPct = holder->GetAuraByEffectIndex(EFFECT_INDEX_1);
        if (hpPct && dmPct && victim->GetHealth() * 100 <= victim->GetMaxHealth() * hpPct->GetModifier()->m_amount)
            totalMod *= (dmPct->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 47558 - Test of Faith
struct TestOfFaith : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (victim->GetHealth() < victim->GetMaxHealth() / 2)
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 55692 - Glyph of Smite
struct GlyphOfSmite : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        // Shadow Word: Pain
        if (victim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, uint64(0x00100000)))
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 55680 - Glyph of Prayer of Healing
struct GlyphOfPrayerOfHealing : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.basepoints[0] = int32(procData.damage * aura->GetAmount() / 200);   // 10% each tick
        procData.triggeredSpellId = 56161; // Glyph of Prayer of Healing
        return SPELL_AURA_PROC_OK;
    }
};

// 55689 - Glyph of Shadow
struct GlyphOfShadow : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (data.procFlags & (PROC_FLAG_DEAL_HARMFUL_PERIODIC | PROC_FLAG_TAKE_HARMFUL_PERIODIC)) // do not proc on dots
            return false;
        return true;
    }
};

// 57989 - Shadowfiend Death
struct ShadowfiendDeath : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        return !aura->GetTarget()->IsAlive();
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (Unit* spawner = aura->GetTarget()->GetSpawner())
            if (spawner->HasAura(58228)) // Glyph of Shadowfiend
                spawner->CastSpell(nullptr, 58227, TRIGGERED_OLD_TRIGGERED);
        return SPELL_AURA_PROC_OK;
    }
};

// 71132 - Glyph of Shadow Word: Pain
struct GlyphOfShadowWordPain : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->SetDamage(spell->GetCaster()->GetPower(POWER_MANA) / 100); // 1%
    }
};

// 55677 - Glyph of Dispel Magic
struct GlyphOfDispelMagic : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (!aura->GetTarget()->CanAssist(procData.target))
            return SPELL_AURA_PROC_FAILED;

        procData.basepoints[0] = int32(procData.target->GetMaxHealth() * aura->GetAmount() / 100);
        return SPELL_AURA_PROC_OK;
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
    RegisterSpellScript<PowerWordShieldPriest>("spell_power_word_shield_priest");
    RegisterSpellScript<DivineHymn>("spell_divine_hymn");
    RegisterSpellScript<HymnOfHope>("spell_hymn_of_hope");
    RegisterSpellScript<CircleOfHealing>("spell_circle_of_healing");
    RegisterSpellScript<LightwellRenew>("spell_lightwell_renew");
    RegisterSpellScript<LightwellRelay>("spell_lightwell_relay");
    RegisterSpellScript<GlyphOfLightwell>("spell_glyph_of_lightwell");
    RegisterSpellScript<ShadowAffinityDots>("spell_shadow_affinity_dots");
    RegisterSpellScript<GuardianSpiritPriest>("spell_guardian_spirit_priest");
    RegisterSpellScript<RenewedHope>("spell_renewed_hope");
    RegisterSpellScript<RenewedHopeDamageTaken>("spell_renewed_hope_damage_taken");
    RegisterSpellScript<ImprovedFlashHeal>("spell_improved_flash_heal");
    RegisterSpellScript<TwistedFaith>("spell_twisted_faith");
    RegisterSpellScript<GlyphOfShadowWordDeath>("spell_glyph_of_shadow_word_death");
    RegisterSpellScript<TestOfFaith>("spell_test_of_faith");
    RegisterSpellScript<GlyphOfSmite>("spell_glyph_of_smite");
    RegisterSpellScript<GlyphOfPrayerOfHealing>("spell_glyph_of_prayer_of_healing");
    RegisterSpellScript<GlyphOfShadow>("spell_glyph_of_shadow");
    RegisterSpellScript<ShadowfiendDeath>("spell_shadowfiend_death");
    RegisterSpellScript<GlyphOfShadowWordPain>("spell_glyph_of_shadow_word_pain");
    RegisterSpellScript<GlyphOfDispelMagic>("spell_glyph_of_dispel_magic");
}
