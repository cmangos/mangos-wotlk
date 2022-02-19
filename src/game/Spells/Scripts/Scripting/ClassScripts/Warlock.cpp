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
    SPELL_UNSTABLE_AFFLICTION_SILENCE = 31117,
};

struct UnstableAffliction : public AuraScript
{
    void OnDispel(SpellAuraHolder* holder, Unit* dispeller, uint32 dispellingSpellId, uint32 originalStacks) const override
    {
        // use clean value for initial damage
        int32 damage = holder->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0);
        damage *= 9;

        // backfire damage and silence - confirmed to have original caster
        dispeller->CastCustomSpell(dispeller, SPELL_UNSTABLE_AFFLICTION_SILENCE, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, holder->GetCasterGuid());
    }
};

struct CurseOfAgony : public AuraScript
{
    void OnPeriodicCalculateAmount(Aura* aura, uint32& amount) const override
    {
        // 1..4 ticks, 1/2 from normal tick damage
        if (aura->GetAuraTicks() <= 4)
            amount = amount / 2;
        // 9..12 ticks, 3/2 from normal tick damage
        else if (aura->GetAuraTicks() >= 9)
            amount += (amount + 1) / 2; // +1 prevent 0.5 damage possible lost at 1..4 ticks
        // 5..8 ticks have normal tick damage
    }
};

struct LifeTap : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        float cost = spell->m_currentBasePoints[EFFECT_INDEX_0];

        Unit* caster = spell->GetCaster();
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(spell->m_spellInfo->Id, SPELLMOD_COST, cost);

        int32 dmg = caster->SpellDamageBonusDone(caster, spell->m_spellInfo, uint32(cost > 0 ? cost : 0), SPELL_DIRECT_DAMAGE);
        dmg = caster->SpellDamageBonusTaken(caster, spell->m_spellInfo, dmg, SPELL_DIRECT_DAMAGE);
        spell->SetScriptValue(dmg);
    }

    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        if (spell->GetScriptValue() > int32(spell->GetCaster()->GetHealth()))
            return SPELL_FAILED_FIZZLE;

        if (!strict)
        {
            int32 dmg = spell->GetScriptValue();
            if (Aura* aura = spell->GetCaster()->GetAura(28830, EFFECT_INDEX_0))
                dmg += dmg * aura->GetModifier()->m_amount / 100;
            spell->SetPowerCost(dmg);
        }
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        int32 mana = spell->GetScriptValue();

        Unit::AuraList const& auraDummy = caster->GetAurasByType(SPELL_AURA_DUMMY);
        for (Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
        {
            if ((*itr)->isAffectedOnSpell(spell->m_spellInfo))
            {
                switch ((*itr)->GetSpellProto()->Id)
                {
                    case 28830: // Plagueheart Rainment - reduce hp cost
                        break;
                        // Improved Life Tap
                    default: mana = ((*itr)->GetModifier()->m_amount + 100) * mana / 100; break;
                }
            }
        }

        caster->CastCustomSpell(nullptr, 31818, &mana, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);

        // Mana Feed
        int32 manaFeedVal = 0;
        Unit::AuraList const& mod = caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
        for (auto itr : mod)
        {
            if (itr->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && itr->GetSpellProto()->SpellIconID == 1982)
                manaFeedVal += itr->GetModifier()->m_amount;
        }
        if (manaFeedVal > 0 && caster->GetPet())
        {
            manaFeedVal = manaFeedVal * mana / 100;
            caster->CastCustomSpell(nullptr, 32553, &manaFeedVal, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr);
        }
    }
};

struct DemonicKnowledge : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (!data.aura)
            return value;
        if (Unit* caster = data.caster)
        {
            Unit* target = data.target;
            if (!data.aura->GetScriptValue())
                data.aura->SetScriptValue((target->HasAura(35693) ? 12 : target->HasAura(35692) ? 8 : target->HasAura(35691) ? 4 : 0));
            value = data.aura->GetScriptValue() * (caster->GetStat(STAT_STAMINA) + caster->GetStat(STAT_INTELLECT)) / 100;
        }
        return value;
    }
};

struct EyeOfKilrogg : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 2585, TRIGGERED_OLD_TRIGGERED);
        summon->DisableThreatPropagationToOwner();
        if (spell->GetCaster()->GetMapId() == 571) // Northrend - Flight
            summon->CastSpell(nullptr, 58083, TRIGGERED_OLD_TRIGGERED);
    }
};

struct CurseOfDoom : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        // not allow cast at player
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target || target->GetTypeId() == TYPEID_PLAYER)
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
            if (Unit* caster = aura->GetCaster())
                if (!aura->GetTarget()->IsTrivialForTarget(caster))
                    caster->CastSpell(aura->GetTarget(), 18662, TRIGGERED_OLD_TRIGGERED);
    }
};

struct CurseOfDoomEffect : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 42010, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DevourMagic : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        Unit* caster = spell->GetCaster();
        if (target && caster)
        {
            auto auras = target->GetSpellAuraHolderMap();
            for (auto itr : auras)
            {
                SpellEntry const* spell = itr.second->GetSpellProto();
                if (itr.second->GetTarget()->GetObjectGuid() != caster->GetObjectGuid() && spell->Dispel == DISPEL_MAGIC && IsPositiveSpell(spell) && !IsPassiveSpell(spell))
                    return SPELL_CAST_OK;
            }
        }
        return SPELL_FAILED_NOTHING_TO_DISPEL;
    }
};

enum
{
    SPELL_SEED = 27243,
    SPELL_SEED_2 = 47835,
    SPELL_SEED_3 = 47836,

    SPELL_SEED_DAMAGE = 27285,
    SPELL_SEED_DAMAGE_2 = 47833,
    SPELL_SEED_DAMAGE_3 = 47834,
};

struct SeedOfCorruption : public AuraScript
{
    uint32 GetSeedDamageSpell(uint32 id) const
    {
        switch (id)
        {
            default:
            case SPELL_SEED: return SPELL_SEED_DAMAGE;
            case SPELL_SEED_2: return SPELL_SEED_DAMAGE_2;
            case SPELL_SEED_3: return SPELL_SEED_DAMAGE_3;
        }
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        if (aura->GetEffIndex() != EFFECT_INDEX_1)
            return;
        if (aura->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
            if (Unit* caster = aura->GetCaster())
                caster->CastSpell(aura->GetTarget(), GetSeedDamageSpell(aura->GetId()), TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_1)
            return SPELL_AURA_PROC_OK;
        Modifier* mod = procData.triggeredByAura->GetModifier();
        // if damage is more than need
        if (mod->m_amount <= (int32)procData.damage)
        {
            // remember guid before aura delete
            ObjectGuid casterGuid = procData.triggeredByAura->GetCasterGuid();

            // Remove aura (before cast for prevent infinite loop handlers)
            procData.victim->RemoveAurasByCasterSpell(procData.triggeredByAura->GetId(), procData.triggeredByAura->GetCasterGuid());

            // Cast finish spell (triggeredByAura already not exist!)
            if (Unit* caster = procData.triggeredByAura->GetCaster())
                caster->CastSpell(procData.victim, GetSeedDamageSpell(aura->GetId()), TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            return SPELL_AURA_PROC_OK;              // no hidden cooldown
        }

        // Damage counting
        mod->m_amount -= procData.damage;
        return SPELL_AURA_PROC_OK;
    }
};

struct SeedOfCorruptionDamage : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetObjectGuid() == spell->m_targets.getUnitTargetGuid()) // in TBC skip target of initial aura
            return false;
        return true;
    }
};

struct SiphonLifeWotlk : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (data.damage == 0)
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        int32  triggerAmount = aura->GetModifier()->m_amount;
        // Glyph of Siphon Life
        if (Aura* aur = aura->GetTarget()->GetAura(56216, EFFECT_INDEX_0))
            triggerAmount += triggerAmount * aur->GetModifier()->m_amount / 100;

        procData.basepoints[0] = int32(procData.damage * triggerAmount / 100);
        procData.triggeredSpellId = 63106;
        procData.triggerTarget = nullptr;

        return SPELL_AURA_PROC_OK;
    }
};

void LoadWarlockScripts()
{
    RegisterSpellScript<UnstableAffliction>("spell_unstable_affliction");
    RegisterSpellScript<CurseOfAgony>("spell_curse_of_agony");
    RegisterSpellScript<LifeTap>("spell_life_tap");
    RegisterSpellScript<DemonicKnowledge>("spell_demonic_knowledge");
    RegisterSpellScript<SeedOfCorruption>("spell_seed_of_corruption");
    RegisterSpellScript<EyeOfKilrogg>("spell_eye_of_kilrogg");
    RegisterSpellScript<DevourMagic>("spell_devour_magic");
    RegisterSpellScript<SeedOfCorruptionDamage>("spell_seed_of_corruption_damage");
    RegisterSpellScript<CurseOfDoom>("spell_curse_of_doom");
    RegisterSpellScript<CurseOfDoomEffect>("spell_curse_of_doom_effect");
    RegisterSpellScript<SiphonLifeWotlk>("spell_siphon_life_wotlk");
}
