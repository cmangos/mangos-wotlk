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

enum
{
    SPELL_BLESSING_OF_THE_CLAW = 28750,
};

// 8936 - Regrowth
struct Regrowth : public AuraScript
{
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        if (Unit* caster = aura->GetCaster())
            if (caster->HasOverrideScript(4537))
                caster->CastSpell(aura->GetTarget(), SPELL_BLESSING_OF_THE_CLAW, TRIGGERED_OLD_TRIGGERED);
    }
};

// 1178 - Bear Form, 3025 - Cat Form, 9635 - Dire Bear Form, 24905 - Moonkin Form
struct FormScalingAttackPowerAuras : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.spellProto->EffectApplyAuraName[data.effIdx] == SPELL_AURA_MOD_ATTACK_POWER && data.target->IsPlayer())
        {
            Player* player = static_cast<Player*>(data.target);
            // Predatory Strikes
            Aura* predatoryStrikes = player->GetKnownTalentRankAuraById(803, EFFECT_INDEX_0);
            if (predatoryStrikes)
                value += data.target->GetLevel() * predatoryStrikes->GetAmount() / 100;
        }
        return value;
    }
};

// 33831 - Force of Nature
struct ForceOfNatureSummon : public SpellScript, public AuraScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 37846, TRIGGERED_NONE);
    }

    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const override
    {
        holder->SetAuraDuration(2000);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        target->CastSpell(nullptr, 41929, TRIGGERED_OLD_TRIGGERED);
    }
};

// 41929 - Guardian Aggro Spell
struct GuardianAggroSpell : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (target->GetEntry() == 1964) // Force of Nature treant
        {
            if (target->CanAttack(caster))
            {
                if (target->IsVisibleForOrDetect(caster, caster, true))
                    target->AI()->AttackStart(caster);
            }
        }
    }
};

struct WildGrowth : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        // stored in dummy effect, affected by mods
        spell->SetMaxAffectedTargets(spell->CalculateSpellEffectValue(EFFECT_INDEX_2, caster)); 
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_PRIORITIZE_HEALTH);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return spell->GetCaster()->IsInGroup(target);
    }
};

struct Brambles : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }
};

// 5221 - Shred
struct ShredDruid : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo == SPELL_MISS_NONE)
        {
            if (Aura* glyphOfShred = spell->GetCaster()->GetAura(54815, EFFECT_INDEX_0)) // Glyph of Shred
            {
                Unit* target = spell->GetUnitTarget();
                if (Aura* rip = target->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x0, 0x00200000, spell->GetCaster()->GetObjectGuid()))
                {
                    int32 increaseAmount = glyphOfShred->GetAmount();
                    int32 maxIncreaseAmount = spell->GetCaster()->CalculateSpellEffectValue(target, glyphOfShred->GetSpellProto(), EFFECT_INDEX_1);
                    if (rip->GetScriptValue() >= maxIncreaseAmount)
                        return;
                    SpellAuraHolder* holder = rip->GetHolder();
                    holder->SetAuraMaxDuration(holder->GetAuraMaxDuration() + increaseAmount);
                    holder->SetAuraDuration(holder->GetAuraDuration() + increaseAmount);
                    holder->SendAuraUpdate(false);
                    rip->SetScriptValue(rip->GetScriptValue() + increaseAmount);
                }
            }
        }
    }
};

// 40121 - Swift Flight Form (Passive)
struct SwiftFlightFormPassive : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (!data.target->IsPlayer() || data.effIdx != EFFECT_INDEX_1)
            return value;
        Player* player = static_cast<Player*>(data.target);
        for (PlayerSpellMap::const_iterator iter = player->GetSpellMap().begin(); iter != player->GetSpellMap().end(); ++iter)
        {
            if (iter->second.state != PLAYERSPELL_REMOVED)
            {
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(iter->first);
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
                    {
                        int32 mountSpeed = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
                        if (mountSpeed > value)
                            return mountSpeed;
                    }
                }
            }
        }
        return value;
    }
};

// 33851 - Primal Tenacity
struct PrimalTenacity : public AuraScript
{
    void OnAbsorb(Aura * aura, int32 & currentAbsorb, int32 & remainingDamage, uint32 & reflectedSpellId, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        // while affected by Stun and Fear
        if (aura->GetTarget()->GetShapeshiftForm() == FORM_CAT && aura->GetTarget()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED))
            remainingDamage -= remainingDamage * currentAbsorb / 100;
    }
};

// 69366 - Moonkin Form (Passive)
struct MoonkinFormPassive : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        // while affected by Stun and Fear
        if (aura->GetTarget()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED))
            remainingDamage -= remainingDamage * currentAbsorb / 100;
    }
};

// 57849 - Improved Insect Swarm
struct ImprovedInsectSwarm : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* /*spellInfo*/) const override
    {
        if (target->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, uint64(0x000000000000002), 0, aura->GetTarget()->GetObjectGuid())) chance += aura->GetModifier()->m_amount; // Starfire
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (victim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, uint64(0x00200000), 0, aura->GetTarget()->GetObjectGuid())) totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f; // Wrath
    }
};

// 37327 - Starfire Bonus
struct StarfireBonus : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (victim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, uint64(0x0000000000200002)))
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 54743 - Glyph of Regrowth
struct GlyphOfRegrowth : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        // Regrowth
        if (victim->GetAura(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, uint64(0x0000000000000040), 0, aura->GetTarget()->GetObjectGuid()))
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 60137 - Nourish Heal Boost
struct NourishHealBoost : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* attacker, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        int32 stepPercent = aura->GetModifier()->m_amount;

        int ownHotCount = 0;                        // counted HoT types amount, not stacks

        Unit::AuraList const& RejorRegr = victim->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
        for (auto itr : RejorRegr)
            if (itr->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID &&
                itr->GetCasterGuid() == attacker->GetObjectGuid())
                ++ownHotCount;

        if (ownHotCount)
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f; 
    }
};

// 50464 - Nourish
struct Nourish : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        int ownHotCount = 0;                        // counted HoT types amount, not stacks
        Unit::AuraList const& rejorRegr = target->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
        for (auto i : rejorRegr)
            if (i->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID &&
                i->GetCasterGuid() == caster->GetObjectGuid())
                ++ownHotCount;

        if (ownHotCount)
        {
            float doneMod = 1.2f; // base bonus at HoTs

            if (Aura* glyph = caster->GetAura(62971, EFFECT_INDEX_0))// Glyph of Nourish
                doneMod *= (glyph->GetModifier()->m_amount * ownHotCount + 100.0f) / 100.0f;

            spell->SetDamageDoneModifier(doneMod, EFFECT_INDEX_0);
        }
    }
};

// 61391 - Typhoon
struct GlyphOfTyphoon : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        // Glyph of Typhoon
        if (spell->GetCaster()->HasAura(62135)) // does not knock back if glyphed
            spell->SetEffectChance(0, EFFECT_INDEX_0);
    }
};

// 54845 - Glyph of Starfire
struct GlyphOfStarfire : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (Aura* moonfire = procData.victim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x00000002, 0, aura->GetTarget()->GetObjectGuid()))
        {
            int32 increaseAmount = aura->GetAmount();
            int32 maxIncreaseAmount = aura->GetTarget()->CalculateSpellEffectValue(procData.victim, aura->GetSpellProto(), EFFECT_INDEX_1);
            if (moonfire->GetScriptValue() >= maxIncreaseAmount)
                return SPELL_AURA_PROC_OK;
            SpellAuraHolder* holder = moonfire->GetHolder();
            holder->SetAuraMaxDuration(holder->GetAuraMaxDuration() + increaseAmount);
            holder->SetAuraDuration(holder->GetAuraDuration() + increaseAmount);
            holder->SendAuraUpdate(false);
            moonfire->SetScriptValue(moonfire->GetScriptValue() + increaseAmount);
        }
        return SPELL_AURA_PROC_OK;
    }
};

// 467 - Thorns
struct ThornsDruid : public AuraScript
{
    virtual int32 OnDurationCalculate(WorldObject const* caster, Unit const* target, int32 duration) const override
    {
        if (caster == target)
            // Glyph of Thorns
            if (Aura const* aur = target->GetAura(57862, EFFECT_INDEX_0))
                duration += aur->GetModifier()->m_amount * MINUTE * IN_MILLISECONDS;
        return duration;
    }
};

// TODO: Glyph of Entangling Roots

void LoadDruidScripts()
{
    RegisterSpellScript<Regrowth>("spell_regrowth");
    RegisterSpellScript<FormScalingAttackPowerAuras>("spell_druid_form_scaling_ap_auras");
    RegisterSpellScript<ForceOfNatureSummon>("spell_force_of_nature_summon");
    RegisterSpellScript<GuardianAggroSpell>("spell_guardian_aggro_spell");
    RegisterSpellScript<StarfireBonus>("spell_starfire_bonus");
    RegisterSpellScript<WildGrowth>("spell_wild_growth");
    RegisterSpellScript<Brambles>("spell_brambles");
    RegisterSpellScript<ShredDruid>("spell_shred_druid");
    RegisterSpellScript<SwiftFlightFormPassive>("spell_swift_flight_form_passive");
    RegisterSpellScript<PrimalTenacity>("spell_primal_tenacity");
    RegisterSpellScript<MoonkinFormPassive>("spell_moonkin_form_passive");
    RegisterSpellScript<ImprovedInsectSwarm>("spell_improved_insect_swarm");
    RegisterSpellScript<GlyphOfRegrowth>("spell_glyph_of_regrowth");
    RegisterSpellScript<NourishHealBoost>("spell_nourish_heal_boost");
    RegisterSpellScript<Nourish>("spell_nourish");
    RegisterSpellScript<GlyphOfTyphoon>("spell_typhoon");
    RegisterSpellScript<GlyphOfStarfire>("spell_glyph_of_starfire");
    RegisterSpellScript<ThornsDruid>("spell_thorns_druid");
}