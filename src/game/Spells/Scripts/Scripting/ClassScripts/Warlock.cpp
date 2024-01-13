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

// 30108 - Unstable Affliction
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

// 980 - Curse of Agony
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

// 1454 - Life Tap
struct LifeTap : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        float cost = spell->m_currentBasePoints[EFFECT_INDEX_0];

        Unit* caster = spell->GetCaster();
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(spell->m_spellInfo->Id, SPELLMOD_COST, cost);

        int32 dmg = caster->SpellDamageBonusDone(caster, SpellSchoolMask(spell->m_spellInfo->SchoolMask), spell->m_spellInfo, EFFECT_INDEX_0, uint32(cost > 0 ? cost : 0), SPELL_DIRECT_DAMAGE);
        dmg = caster->SpellDamageBonusTaken(caster, SpellSchoolMask(spell->m_spellInfo->SchoolMask), spell->m_spellInfo, EFFECT_INDEX_0, dmg, SPELL_DIRECT_DAMAGE);
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

// 5699 - Create Healthstone
struct CreateHealthStoneWarlock : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* caster = spell->GetCaster();
        // check if we already have a healthstone
        uint32 itemType = GetUsableHealthStoneItemType(caster, spell->m_spellInfo);
        if (itemType && caster->IsPlayer() && static_cast<Player*>(caster)->GetItemCount(itemType) > 0)
            return SPELL_FAILED_TOO_MANY_OF_ITEM;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        uint32 itemType = GetUsableHealthStoneItemType(target, spell->m_spellInfo);
        if (itemType)
            spell->DoCreateItem(effIdx, itemType);
    }

    uint32 GetUsableHealthStoneItemType(Unit* target, SpellEntry const* spellInfo) const
    {
        if (!target || !target->IsPlayer())
            return 0;

        uint32 itemtype = 0;
        uint32 rank = 0;
        Unit::AuraList const& mDummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
        for (auto mDummyAura : mDummyAuras)
        {
            if (mDummyAura->GetId() == 18692)
            {
                rank = 1;
                break;
            }
            if (mDummyAura->GetId() == 18693)
            {
                rank = 2;
                break;
            }
        }

        static uint32 const itypes[8][3] =
        {
            { 5512, 19004, 19005},              // Minor Healthstone
            { 5511, 19006, 19007},              // Lesser Healthstone
            { 5509, 19008, 19009},              // Healthstone
            { 5510, 19010, 19011},              // Greater Healthstone
            { 9421, 19012, 19013},              // Major Healthstone
            {22103, 22104, 22105},              // Master Healthstone
            {36889, 36890, 36891},              // Demonic Healthstone
            {36892, 36893, 36894}               // Fel Healthstone
        };

        switch (spellInfo->Id)
        {
            case  6201:
                itemtype = itypes[0][rank]; break; // Minor Healthstone
            case  6202:
                itemtype = itypes[1][rank]; break; // Lesser Healthstone
            case  5699:
                itemtype = itypes[2][rank]; break; // Healthstone
            case 11729:
                itemtype = itypes[3][rank]; break; // Greater Healthstone
            case 11730:
                itemtype = itypes[4][rank]; break; // Major Healthstone
            case 27230:
                itemtype = itypes[5][rank]; break; // Master Healthstone
            case 47871:
                itemtype = itypes[6][rank]; break; // Demonic Healthstone
            case 47878:
                itemtype = itypes[7][rank]; break; // Fel Healthstone
        }
        return itemtype;
    }
};

// 35696 - Demonic Knowledge
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

// 126 - Eye of Kilrogg (Summon)
struct EyeOfKilrogg : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 2585, TRIGGERED_OLD_TRIGGERED);
        summon->DisableThreatPropagationToOwner();
        if (spell->GetCaster()->HasAura(58081)) // Glyph of Kilrogg
        {
            MapEntry const* mapEntry = spell->GetCaster()->GetMap()->GetEntry();
            if (mapEntry->addon < 1 || !mapEntry->IsContinent()) // flyable areas
                return;
            summon->CastSpell(nullptr, 58083, TRIGGERED_OLD_TRIGGERED); // flight
        }
    }
};

// 603 - Curse of Doom
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

// 18662 - Curse of Doom Effect
struct CurseOfDoomEffect : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 42010, TRIGGERED_OLD_TRIGGERED);
    }
};

// 19505 - Devour Magic
struct DevourMagic : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        Unit* caster = spell->GetCaster();
        if (target && caster)
        {
            auto auras = target->GetSpellAuraHolderMap();
            for (auto itr : auras)
            {
                SpellEntry const* spell = itr.second->GetSpellProto();
                if (itr.second->GetTarget()->GetObjectGuid() != caster->GetObjectGuid() && spell->Dispel == DISPEL_MAGIC && (caster->CanAssist(target) ? !IsPositiveSpell(spell) : IsPositiveSpell(spell)) && !IsPassiveSpell(spell))
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

// 27243 - Seed of Corruption
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
                caster->CastSpell(aura->GetTarget(), GetSeedDamageSpell(aura->GetId()), TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_CASTER_AURA_STATE | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
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
                caster->CastSpell(procData.victim, GetSeedDamageSpell(aura->GetId()), TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_CASTER_AURA_STATE | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            return SPELL_AURA_PROC_OK;              // no hidden cooldown
        }

        // Damage counting
        mod->m_amount -= procData.damage;
        return SPELL_AURA_PROC_OK;
    }
};

// 27285 - Seed of Corruption
struct SeedOfCorruptionDamage : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetObjectGuid() == spell->m_targets.getUnitTargetGuid()) // in TBC skip target of initial aura
            return true; // in WotLK Seed of Corruption also damages the initial target
        return true;
    }
};

// 30293 - Soul Leech
struct SoulLeech : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        int32 damage = int32(procData.damage * aura->GetAmount() / 100);
        Unit* target = aura->GetTarget();
        target->CastCustomSpell(nullptr, 30294, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);

        if (target->IsPlayer())
        {
            if (Aura* improvedSoulLeech = static_cast<Player*>(target)->GetKnownTalentRankAuraById(1889, EFFECT_INDEX_1)) // Improved Soul Leech
            {
                uint32 selfId = 0, petId = 0;
                switch (aura->GetId())
                {
                    case 54117: selfId = 54300; petId = 54607; break;
                    case 54118: selfId = 59117; petId = 59118; break;
                }

                target->CastSpell(nullptr, selfId, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
                target->CastSpell(nullptr, petId, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);

                if (roll_chance_i(improvedSoulLeech->GetAmount()))
                    target->CastSpell(nullptr, 57669, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            }
        }
        return SPELL_AURA_PROC_OK;
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

// 54049 - Shadow Bite
struct ShadowBite : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* owner = spell->GetCaster()->GetOwner();
        if (!owner)
            return;

        uint32 counter = 0;
        Unit::AuraList const& dotAuras = owner->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
        for (auto dotAura : dotAuras)
            if (dotAura->GetCasterGuid() == owner->GetObjectGuid())
                ++counter;

        uint32 damage = spell->GetDamage();
        if (counter)
            spell->SetDamage( + (counter * owner->CalculateSpellEffectValue(owner, spell->m_spellInfo, EFFECT_INDEX_2) * damage) / 100.0f);

        if (owner->IsPlayer())
        {
            if (SpellEntry const* talent = static_cast<Player*>(owner)->GetKnownTalentRankById(1873)) // Improved Felhunter
            {
                int32 damage = talent->Id == 54037 ? 4 : 8;
                spell->GetCaster()->CastCustomSpell(nullptr, 54425, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            }
        }
    }
};

enum
{
    DEMONIC_CIRCLE_SUMMON        = 48018,
    DEMONIC_CIRCLE_CLEAR         = 60854,
    DEMONIC_CIRCLE_IN_RANGE_AURA = 62388,
};

// 48020 - Demonic Circle: Teleport
struct DemonicCircleTeleport : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* warlock = spell->GetCaster();
        if (!warlock)
            return;      
        GameObject* circle = warlock->GetGameObject(DEMONIC_CIRCLE_SUMMON);
        if (!circle)
            return;
        Position circlePos = circle->GetPosition();
        warlock->NearTeleportTo(circlePos.GetPositionX(), circlePos.GetPositionY(), circlePos.GetPositionZ(), circlePos.GetPositionO());
    }

    SpellCastResult OnCheckCast(Spell* spell, bool) const override
    {
        Unit* warlock = spell->GetCaster();
        if (!warlock)
            return SPELL_FAILED_ERROR;
        GameObject* circle = warlock->GetGameObject(DEMONIC_CIRCLE_SUMMON);
        if (!circle)
        {
            spell->SetParam1(SPELL_FAILED_CUSTOM_ERROR_75);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
        if (warlock->GetDistance(circle) > 40)
            return SPELL_FAILED_OUT_OF_RANGE;
        return SPELL_CAST_OK;
    }
};

// 48018 - Demonic Circle: Summon
struct DemonicCircleSummon : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        Unit* warlock = aura->GetTarget();
        if (!warlock)
            return;
        GameObject* circle = warlock->GetGameObject(DEMONIC_CIRCLE_SUMMON);
        if (!circle)
            return;
        circle->ForcedDespawn();
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* warlock = aura->GetTarget();
        if (!warlock)
            return;      
        GameObject* circle = warlock->GetGameObject(DEMONIC_CIRCLE_SUMMON);
        if (!circle)
            return;
        if (warlock->GetDistance(circle) <= 40)
        {
            if (!warlock->HasAura(DEMONIC_CIRCLE_IN_RANGE_AURA))
                warlock->CastSpell(warlock, DEMONIC_CIRCLE_IN_RANGE_AURA, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_INSTANT_CAST);
        }
        else
            warlock->RemoveAurasDueToSpell(DEMONIC_CIRCLE_IN_RANGE_AURA);
    }
};

// 56229 - Glyph of Shadowburn
struct GlyphOfShadowburn : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_CRIT_CHANCE, apply);
    }

    void OnCritChanceCalculate(Aura* aura, Unit const* target, float& chance, SpellEntry const* spellInfo) const override
    {
        if (target->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT)) chance += aura->GetModifier()->m_amount;
    }
};

// 17804 - Soul Siphon
struct SoulSiphon : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* attacker, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        // effect 1 m_amount
        int32 maxPercent = aura->GetModifier()->m_amount;
        // effect 0 m_amount
        int32 stepPercent = attacker->CalculateSpellEffectValue(attacker, aura->GetSpellProto(), EFFECT_INDEX_0);
        // count affliction effects and calc additional damage in percentage
        int32 modPercent = 0;
        Unit::SpellAuraHolderMap const& victimAuras = victim->GetSpellAuraHolderMap();
        for (const auto& victimAura : victimAuras)
        {
            SpellEntry const* m_spell = victimAura.second->GetSpellProto();
            if (m_spell->SpellFamilyName != SPELLFAMILY_WARLOCK || !(m_spell->SpellFamilyFlags & uint64(0x0004071B8044C402)))
                continue;
            modPercent += stepPercent * victimAura.second->GetStackAmount();
            if (modPercent >= maxPercent)
            {
                modPercent = maxPercent;
                break;
            }
        }
        totalMod *= (modPercent + 100.0f) / 100.0f;
    }
};

// 47198 - Death's Embrace
struct DeathsEmbrace : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_DAMAGE_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
        {
            if (aura->GetTarget()->GetHealthPercent() <= 20.f)
                totalMod *= (100.0f + aura->GetModifier()->m_amount) / 100.0f;
        }
        else
        {
            if (victim->GetHealthPercent() <= 35.f)
                totalMod *= (100.0f + aura->GetModifier()->m_amount) / 100.0f;
        }
    }
};

// 1120 - Drain Soul
struct DrainSoul : public AuraScript
{
    void OnPeriodicCalculateAmount(Aura* aura, uint32& amount) const override
    {
        if (aura->GetTarget()->GetHealthPercent() <= 25.f)
            amount *= 4; // can be done here because amount contains all done bonuses already
    }
};

// 63320 - Glyph of Life Tap
struct GlyphOfLifeTap : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = 63321;
        return SPELL_AURA_PROC_OK;
    }
};

// 63310 - Glyph of Shadowflame
struct GlyphOfShadowflame : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = 63311;
        return SPELL_AURA_PROC_OK;
    }
};

// 6358 - Seduction
struct SeductionSuccubus : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        bool hasGlyph = false;
        if (Unit* caster = aura->GetCaster())
            if (Unit* owner = caster->GetOwner())
                if (owner->HasAura(56250)) // Glyph of Succubus
                    hasGlyph = true;
        if (apply && hasGlyph)
        {
            aura->GetTarget()->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE);
            aura->GetTarget()->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        }
    }
};

// 48181, 59161, 59163, 59164 - Haunt
struct Haunt : public AuraScript, public SpellScript
{
    enum
    {
        SPELL_HAUNT_HEAL = 48210,
    };
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            aura->SetScriptValue(0);
            return;
        }
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!caster)
            return;
        int32 bp0 = aura->GetScriptValue();
        target->CastCustomSpell(caster, SPELL_HAUNT_HEAL, &bp0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, aura);
    }

    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (Aura* dummy = target->GetDummyAura(spell->m_spellInfo->Id))
            dummy->SetScriptValue(spell->GetTotalTargetDamage());
    }
};

void LoadWarlockScripts()
{
    RegisterSpellScript<UnstableAffliction>("spell_unstable_affliction");
    RegisterSpellScript<CurseOfAgony>("spell_curse_of_agony");
    RegisterSpellScript<LifeTap>("spell_life_tap");
    RegisterSpellScript<CreateHealthStoneWarlock>("spell_create_health_stone_warlock");
    RegisterSpellScript<DemonicKnowledge>("spell_demonic_knowledge");
    RegisterSpellScript<SeedOfCorruption>("spell_seed_of_corruption");
    RegisterSpellScript<SoulLeech>("spell_soul_leech");
    RegisterSpellScript<EyeOfKilrogg>("spell_eye_of_kilrogg");
    RegisterSpellScript<DevourMagic>("spell_devour_magic");
    RegisterSpellScript<SeedOfCorruptionDamage>("spell_seed_of_corruption_damage");
    RegisterSpellScript<CurseOfDoom>("spell_curse_of_doom");
    RegisterSpellScript<CurseOfDoomEffect>("spell_curse_of_doom_effect");
    RegisterSpellScript<SoulSiphon>("spell_soul_siphon");
    RegisterSpellScript<SiphonLifeWotlk>("spell_siphon_life_wotlk");
    RegisterSpellScript<ShadowBite>("spell_shadow_bite");
    RegisterSpellScript<DemonicCircleTeleport>("spell_demonic_circle_teleport");
    RegisterSpellScript<DemonicCircleSummon>("spell_demonic_circle_summon");
    RegisterSpellScript<GlyphOfShadowburn>("spell_glyph_of_shadowburn");
    RegisterSpellScript<DeathsEmbrace>("spell_deaths_embrace");
    RegisterSpellScript<DrainSoul>("spell_drain_soul");
    RegisterSpellScript<GlyphOfLifeTap>("spell_glyph_of_life_tap");
    RegisterSpellScript<GlyphOfShadowflame>("spell_glyph_of_shadowflame");
    RegisterSpellScript<SeductionSuccubus>("spell_seduction_succubus");
    RegisterSpellScript<Haunt>("spell_haunt");
}
