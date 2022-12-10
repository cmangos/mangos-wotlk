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

struct spell_judgement : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
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

        if (caster->HasAura(40470)) // spell_paladin_tier_6_trinket
            if (roll_chance_f(50.f))
                caster->CastSpell(unitTarget, 40472, TRIGGERED_OLD_TRIGGERED);
    }
};

struct spell_paladin_tier_6_trinket : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
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
    void OnApply(Aura* aura, bool apply) const
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*victim*/, int32& advertisedBenefit, float& /*totalMod*/) const override
    {
        advertisedBenefit += aura->GetModifier()->m_amount;
    }
};

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

struct DivineStormCooldown : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        caster->RemoveSpellCooldown(53385, true);
    }
};

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

struct JudgementsOfTheWiseEnergize : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->SetDamage(spell->GetDamage() * spell->GetCaster()->GetCreateMana() / 100);
    }
};

struct ArdentDefender : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& preventedDeath) const override
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
        float defenseFactor = (player->GetDefenseSkillValue() - player->GetLevel() * 5) / 140.f;
        healMod *= defenseFactor;
        healMod = player->GetMaxHealth() * (healMod / 100.f);
        remainingDamage = 0;
        player->CastCustomSpell(nullptr, 66235, &healMod, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        player->CastSpell(nullptr, 66233, TRIGGERED_OLD_TRIGGERED);
    }
};

void LoadPaladinScripts()
{
    RegisterSpellScript<IncreasedHolyLightHealing>("spell_increased_holy_light_healing");
    RegisterSpellScript<spell_judgement>("spell_judgement");
    RegisterSpellScript<RighteousDefense>("spell_righteous_defense");
    RegisterSpellScript<spell_paladin_tier_6_trinket>("spell_paladin_tier_6_trinket");
    RegisterSpellScript<DivineStorm>("spell_divine_storm");
    RegisterSpellScript<DivineStormHeal>("spell_divine_storm_heal");
    RegisterSpellScript<DivineStormCooldown>("spell_divine_storm_cooldown");
    RegisterSpellScript<JudgementsOfTheWise>("spell_judgements_of_the_wise");
    RegisterSpellScript<JudgementsOfTheWiseEnergize>("spell_judgements_of_the_wise_energize");
    RegisterSpellScript<ArdentDefender>("spell_ardent_defender");
}