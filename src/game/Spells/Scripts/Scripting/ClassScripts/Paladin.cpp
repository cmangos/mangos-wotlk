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

// 20271 - Judgement
struct spell_judgement : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || !unitTarget->IsAlive())
            return;

        Unit* caster = spell->GetCaster();

        uint32 spellId1;
        uint32 spellId2 = 0;

        // Judgement self add switch
        switch (spell->m_spellInfo->Id)
        {
            case 53407: spellId1 = 20184; break;    // Judgement of Justice
            case 20271:                             // Judgement of Light
            case 57774: spellId1 = 20185; break;    // Judgement of Light
            case 53408: spellId1 = 20186; break;    // Judgement of Wisdom
            default:
                sLog.outError("Unsupported Judgement (seal trigger) spell (Id: %u) in Spell::EffectScriptEffect", spell->m_spellInfo->Id);
                return;
        }

        // offensive seals have aura dummy in 2 effect
        Unit::AuraList const& m_dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
        for (auto m_dummyAura : m_dummyAuras)
        {
            // search seal (offensive seals have judgement's aura dummy spell id in 2 effect
            if (m_dummyAura->GetEffIndex() != EFFECT_INDEX_2 || !IsSealSpell(m_dummyAura->GetSpellProto()))
                continue;
            spellId2 = m_dummyAura->GetModifier()->m_amount;
            SpellEntry const* judge = sSpellTemplate.LookupEntry<SpellEntry>(spellId2);
            if (!judge)
                continue;
            break;
        }

        // if there were no offensive seals than there is seal with proc trigger aura
        if (!spellId2)
        {
            Unit::AuraList const& procTriggerAuras = caster->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
            for (auto procTriggerAura : procTriggerAuras)
            {
                if (procTriggerAura->GetEffIndex() != EFFECT_INDEX_0 || !IsSealSpell(procTriggerAura->GetSpellProto()))
                    continue;
                spellId2 = 54158;
                break;
            }
        }

        if (spellId1)
            caster->CastSpell(unitTarget, spellId1, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);

        if (spellId2)
            caster->CastSpell(unitTarget, spellId2, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);

        if (caster->HasAura(37188)) // improved judgement
            caster->CastSpell(nullptr, 43838, TRIGGERED_OLD_TRIGGERED);

        if (caster->HasAura(40470)) // PaladinTier6Trinket
            if (roll_chance_f(50.f))
                caster->CastSpell(unitTarget, 40472, TRIGGERED_OLD_TRIGGERED);
    }
};

// 40470 - Paladin Tier 6 Trinket
struct PaladinTier6Trinket : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        if (!procData.spellInfo)
            return SPELL_AURA_PROC_FAILED;

        float chance = 0.f;

        // Flash of light/Holy light
        if (procData.spellInfo->SpellFamilyFlags & uint64(0x00000000C0000000))
        {
            procData.triggeredSpellId = 40471;
            chance = 15.0f;
            procData.triggerTarget = procData.victim;
        }

        if (!roll_chance_f(chance))
            return SPELL_AURA_PROC_FAILED;

        return SPELL_AURA_PROC_OK;
    }
};

struct IncreasedHolyLightHealing : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* /*victim*/, int32& advertisedBenefit, float& /*totalMod*/) const override
    {
        advertisedBenefit += aura->GetModifier()->m_amount;
    }
};

// 31789 - Righteous Defense
struct RighteousDefense : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            return false;

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;
        Unit* caster = spell->GetCaster();

        // non-standard cast requirement check
        if (unitTarget->getAttackers().empty())
        {
            caster->RemoveSpellCooldown(*spell->m_spellInfo, true);
            spell->SendCastResult(SPELL_FAILED_TARGET_AFFECTING_COMBAT);
            return;
        }

        // not empty (checked), copy
        Unit::AttackerSet attackers = unitTarget->getAttackers();

        // selected from list 3
        size_t size = std::min(size_t(3), attackers.size());
        for (uint32 i = 0; i < size; ++i)
        {
            Unit::AttackerSet::iterator aItr = attackers.begin();
            std::advance(aItr, urand() % attackers.size());
            caster->CastSpell((*aItr), 31790, TRIGGERED_NONE); // step 2
            attackers.erase(aItr);
        }
    }
};

// 53385 - Divine Storm
struct DivineStorm : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(4);
    }

    void OnAfterHit(Spell* spell) const override
    {
        spell->SetScriptValue(spell->GetScriptValue() + spell->GetTotalTargetDamage());
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        int32 totalDamage = int32(spell->GetScriptValue() * spell->CalculateSpellEffectValue(EFFECT_INDEX_1, nullptr) / 100);
        spell->GetCaster()->CastCustomSpell(nullptr, 54171, &totalDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

// 54171 - Divine Storm
struct DivineStormHeal : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(3);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        int32 damage = spell->GetDamage() / spell->GetTargetList().size();
        spell->GetCaster()->CastCustomSpell(target, 54172, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    }
};

// 70769 - Divine Storm!
struct DivineStormCooldown : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        caster->RemoveSpellCooldown(53385, true);
    }
};

// 31876, 31877, 31878
struct JudgementsOfTheWise : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (data.damage == 0)
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        Unit* caster = aura->GetTarget();
        procData.triggeredSpellId = 31930; // Judgements of the Wise
        procData.triggerTarget = nullptr;

        // Replenishment
        caster->CastSpell(nullptr, 57669, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return SPELL_AURA_PROC_OK;
    }
};

// 31930 - Judgements of the Wise
struct JudgementsOfTheWiseEnergize : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->SetDamage(spell->GetDamage() * spell->GetCaster()->GetCreateMana() / 100);
    }
};

// 31850, 31851, 31852 - Ardent Defender
struct ArdentDefender : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& preventedDeath, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        currentAbsorb = 0;
        Player* player = dynamic_cast<Player*>(aura->GetCaster());
        if (!player)
            return;
        if (int32(player->GetHealth()) - remainingDamage > (player->GetMaxHealth() * 0.35))
            return;
        int32 reduction = 0;
        int32 healMod = 0;
        if (Aura* aur = aura->GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_0))
            reduction = aur->GetBasePoints();
        if (Aura* aur = aura->GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_1))
            healMod = aur->GetBasePoints();
        remainingDamage *= (100 - reduction) / 100.f;
        if (int32(player->GetHealth()) - remainingDamage > 0 || player->HasAura(66233))
            return;
        float defenseFactor = std::min(140u, (player->GetDefenseSkillValue() - player->GetLevel() * 5)) / 140.f;
        defenseFactor = std::max(0.f, defenseFactor);
        healMod *= defenseFactor;
        healMod = player->GetMaxHealth() * (healMod / 100.f);
        remainingDamage = 0;
        preventedDeath = true;
        player->CastCustomSpell(nullptr, 66235, &healMod, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        player->CastSpell(nullptr, 66233, TRIGGERED_OLD_TRIGGERED);
    }
};

// 58597 - Sacred Shield
struct SacredShieldCrit : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* /*spellInfo*/) const override
    {
        if (aura->GetCasterGuid() == target->GetObjectGuid()) chance += aura->GetModifier()->m_amount; // Weakened Soul
    }
};

// 879 - Exorcism
struct ExorcismPaladin : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->GetUnitTarget()->GetCreatureTypeMask() & CREATURE_TYPEMASK_DEMON_OR_UNDEAD)
            spell->SetGuaranteedCrit();
    }
};

// 20911 - Blessing of Sanctuary, 25899 - Greater Blessing of Sanctuary
struct BlessingOfSanctuary : public AuraScript
{
    void OnAuraInit(Aura* aura) const override
    {
        aura->SetAffectOverriden();
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_MELEE_DAMAGE_TAKEN, apply);
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_TAKEN, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* /*victim*/, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f; // all damage
    }
};

// 1038 - Hand of Salvation
struct HandOfSalvation : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster && data.effIdx == EFFECT_INDEX_1)
            if (data.caster->GetObjectGuid() == data.target->GetObjectGuid())
                if (Aura* aur = data.target->GetAura(63225, EFFECT_INDEX_0))
                    return value - aur->GetModifier()->m_amount;
        return value;
    }
};

// 54925 - Glyph of Seal of Command
struct GlyphOfSealOfCommand : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        return aura->GetTarget()->HasAura(20375); // must have seal of command active
    }
};

// 68082 - Glyph of Seal of Command
struct GlyphOfSealOfCommandMana : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;
        spell->SetDamage(spell->GetDamage() * spell->GetUnitTarget()->GetCreateMana() / 100);
    }
};

// 54939 - Glyph of Divinity
struct GlyphOfDivinity : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        if (procData.spell)
        {
            procData.basepoints[EFFECT_INDEX_1] = procData.spell->GetDamageForEffect(EFFECT_INDEX_1);
            procData.triggeredSpellId = 54986;
            procData.triggerTarget = nullptr;
        }

        return SPELL_AURA_PROC_OK;
    }
};

// 54937 - Glyph of Holy Light
struct GlyphOfHolyLight : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = 54968;
        procData.basepoints[0] = aura->GetAmount() * procData.damage / 100;

        return SPELL_AURA_PROC_OK;
    }
};

// 19740 - Blessing Of Might
struct BlessingOfMight : public AuraScript
{
    virtual int32 OnDurationCalculate(WorldObject const* caster, Unit const* target, int32 duration) const override
    {
        if (caster == target)
            // Glyph of Blessing of Might
            if (Aura const* aur = target->GetAura(57958, EFFECT_INDEX_0))
                duration += aur->GetModifier()->m_amount * MINUTE * IN_MILLISECONDS;
        return duration;
    }
};

// 19740 - Blessing Of Wisdom
struct BlessingOfWisdom : public AuraScript
{
    virtual int32 OnDurationCalculate(WorldObject const* caster, Unit const* target, int32 duration) const override
    {
        if (caster == target)
            // Glyph of Blessing of Wisdom
            if (Aura const* aur = target->GetAura(57979, EFFECT_INDEX_0))
                duration += aur->GetModifier()->m_amount * MINUTE * IN_MILLISECONDS;
        return duration;
    }
};

// 19752 - Divine Intervention
struct DivineIntervention : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target)
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
        if (target->HasAura(23333) || target->HasAura(23335) || target->HasAura(34976)) // possibly SPELL_ATTR_EX_IMMUNITY_TO_HOSTILE_AND_FRIENDLY_EFFECTS
            return SPELL_FAILED_TARGET_AURASTATE;
        return SPELL_CAST_OK;
    }
};

void LoadPaladinScripts()
{
    RegisterSpellScript<IncreasedHolyLightHealing>("spell_increased_holy_light_healing");
    RegisterSpellScript<DivineIntervention>("spell_divine_intervention");
    RegisterSpellScript<spell_judgement>("spell_judgement");
    RegisterSpellScript<RighteousDefense>("spell_righteous_defense");
    RegisterSpellScript<PaladinTier6Trinket>("spell_paladin_tier_6_trinket");
    RegisterSpellScript<DivineStorm>("spell_divine_storm");
    RegisterSpellScript<DivineStormHeal>("spell_divine_storm_heal");
    RegisterSpellScript<DivineStormCooldown>("spell_divine_storm_cooldown");
    RegisterSpellScript<JudgementsOfTheWise>("spell_judgements_of_the_wise");
    RegisterSpellScript<JudgementsOfTheWiseEnergize>("spell_judgements_of_the_wise_energize");
    RegisterSpellScript<ArdentDefender>("spell_ardent_defender");
    RegisterSpellScript<SacredShieldCrit>("spell_sacred_shield_crit");
    RegisterSpellScript<ExorcismPaladin>("spell_exorcism_paladin");
    RegisterSpellScript<BlessingOfSanctuary>("spell_blessing_of_sanctuary");
    RegisterSpellScript<HandOfSalvation>("spell_hand_of_salvation");
    RegisterSpellScript<GlyphOfSealOfCommand>("spell_glyph_of_seal_of_command");
    RegisterSpellScript<GlyphOfSealOfCommandMana>("spell_glyph_of_seal_of_command_mana");
    RegisterSpellScript<GlyphOfDivinity>("spell_glyph_of_divinity");
    RegisterSpellScript<GlyphOfHolyLight>("spell_glyph_of_holy_light");
    RegisterSpellScript<BlessingOfMight>("spell_blessing_of_might");
    RegisterSpellScript<BlessingOfWisdom>("spell_blessing_of_wisdom");
}