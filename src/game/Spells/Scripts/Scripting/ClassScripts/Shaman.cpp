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
#include "Entities/Totem.h"
#include "AI/BaseAI/TotemAI.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"

// 6495 - Sentry Totem
struct SentryTotem : public SpellScript, public AuraScript
{
    void OnRadiusCalculate(Spell* /*spell*/, SpellEffectIndex effIdx, bool targetB, float& radius) const override
    {
        if (!targetB && effIdx == EFFECT_INDEX_0)
            radius = 2.f;
    }

    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (Player* player = dynamic_cast<Player*>(spell->GetCaster()))
            player->GetCamera().SetView(summon);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsPlayer())
            return;

        Totem* totem = target->GetTotem(TOTEM_SLOT_AIR);

        if (totem && apply)
            static_cast<Player*>(target)->GetCamera().SetView(totem);
        else
            static_cast<Player*>(target)->GetCamera().ResetView();
    }
};

struct SentryTotemAI : public TotemAI
{
    using TotemAI::TotemAI;

    void AttackStart(Unit* who) override
    {
        TotemAI::AttackStart(who);
        // Sentry totem sends ping on attack
        if (Player* owner = dynamic_cast<Player*>(m_creature->GetSpawner()))
        {
            WorldPacket data(MSG_MINIMAP_PING, (8 + 4 + 4));
            data << m_creature->GetObjectGuid();
            data << m_creature->GetPositionX();
            data << m_creature->GetPositionY();
            owner->SendDirectMessage(data);
        }
    }

    void RemoveAura()
    {
        if (Unit* spawner = m_creature->GetSpawner())
            spawner->RemoveAurasDueToSpell(m_creature->GetUInt32Value(UNIT_CREATED_BY_SPELL));
    }

    void JustDied(Unit* killer) override
    {
        TotemAI::JustDied(killer);
        RemoveAura();
    }

    void OnUnsummon() override
    {
        TotemAI::OnUnsummon();
        RemoveAura();
    }
};

// 974 - Earth Shield
struct EarthShield : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        Unit* target = data.target;
        if (Unit* caster = data.caster)
        {
            value = caster->SpellHealingBonusDone(target, data.spellProto, EFFECT_INDEX_0, value, SPELL_DIRECT_DAMAGE);
            value = target->SpellHealingBonusTaken(caster, data.spellProto, EFFECT_INDEX_0, value, SPELL_DIRECT_DAMAGE);
        }
        return value;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.basepoints[0] = aura->GetAmount();
        procData.triggerTarget = aura->GetTarget();
        procData.triggeredSpellId = 379;
        // Glyph of Earth Shield
        if (Unit* caster = aura->GetCaster())
        {
            if (Aura* aur = caster->GetDummyAura(63279))
            {
                int32 aur_mod = aur->GetModifier()->m_amount;
                procData.basepoints[0] = int32(procData.basepoints[0] * (aur_mod + 100.0f) / 100.0f);
            }
        }

        return SPELL_AURA_PROC_OK;
    }
};

// 39610 - Mana Tide Totem
struct ManaTideTotemEffect : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (!unitTarget || unitTarget->GetPowerType() != POWER_MANA)
            return;

        int32 damage = spell->GetDamage();

        // Glyph of Mana Tide
        if (Unit* owner = caster->GetOwner())
            if (Aura* dummy = owner->GetDummyAura(55441))
                damage += dummy->GetModifier()->m_amount;
        // Regenerate 6% of Total Mana Every 3 secs
        int32 EffectBasePoints0 = unitTarget->GetMaxPower(POWER_MANA) * damage / 100;
        caster->CastCustomSpell(unitTarget, 39609, &EffectBasePoints0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

enum
{
    SPELL_SHAMAN_ELEMENTAL_MASTERY = 16166,
};

struct ItemShamanT10Elemental2PBonus : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (Unit* target = aura->GetTarget())
            if (target->IsPlayer())
                static_cast<Player*>(target)->ModifyCooldown(SPELL_SHAMAN_ELEMENTAL_MASTERY, -aura->GetAmount());
        return SPELL_AURA_PROC_OK;
    }
};

struct EartbindTotem : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (Player* caster = dynamic_cast<Player*>(spell->GetCaster()))
            if (Aura* aura = caster->GetKnownTalentRankAuraById(2052, EFFECT_INDEX_1)) // Storm, Earth and Fire talent
                if (roll_chance_i(aura->GetAmount()))
                    summon->CastSpell(nullptr, 64695, TRIGGERED_OLD_TRIGGERED);
    }
};

struct LavaLash : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Player* caster = dynamic_cast<Player*>(spell->GetCaster());
        if (!caster)
            return;

        if (Item* offhand = caster->GetWeaponForAttack(OFF_ATTACK))
            if (uint32 enchantId = offhand->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
                    if (SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(enchantEntry->spellid[0]))
                        if (spellInfo->IsFitToFamilyMask(0x00200000))
                            // adds damage to targets total damage at end of effect
                            spell->SetDamage(spell->GetDamage() + caster->CalculateSpellEffectValue(spell->GetUnitTarget(), spell->m_spellInfo, EFFECT_INDEX_1));
    }
};

struct AncestralAwakening : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        int32 damage = procData.damage * aura->GetAmount() / 100;
        aura->GetTarget()->CastCustomSpell(nullptr, 52759, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return SPELL_AURA_PROC_OK;
    }
};

struct AncestralAwakeningSearch : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
        spell->SetFilteringScheme(EFFECT_INDEX_0, false, SCHEME_PRIORITIZE_HEALTH);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        int32 damage = spell->GetDamage();
        if (Unit* target = spell->GetUnitTarget())
            spell->GetCaster()->CastCustomSpell(target, 52752, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    }
};

enum
{
    SPELL_STONECLAW_TOTEM_ABSORB = 55277,
    SPELL_GLYPH_OF_STONECLAW_TOTEM = 63298,
};

// 5730 - Stoneclaw Totem
struct StoneclawTotem : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        uint32 spellId = 0;
        switch (spell->m_spellInfo->Id)
        {
            case 5730: spellId = 55328; break;
            case 6390: spellId = 55329; break;
            case 6391: spellId = 55330; break;
            case 6392: spellId = 55332; break;
            case 10427: spellId = 55333; break;
            case 10428: spellId = 55335; break;
            case 25525: spellId = 55278; break;
            case 58580: spellId = 58589; break;
            case 58581: spellId = 58590; break;
            case 58582: spellId = 58591; break;
        }
        if (spellId)
            summon->CastSpell(nullptr, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

// 55278, 55328, 55329, 55330, 55332, 55333, 55335, 58589, 58590, 58591 - Stoneclaw Totem
struct StoneclawTotemAbsorb : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        int32 shieldStrength = spell->GetDamage();

        for (uint8 i = TOTEM_SLOT_FIRE; i <= TOTEM_SLOT_AIR; ++i)
            if (Totem* totem = spell->GetUnitTarget()->GetTotem(TotemSlot(i)))
                spell->GetCaster()->CastCustomSpell(totem, SPELL_STONECLAW_TOTEM_ABSORB, &shieldStrength, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);

        if (Aura* glyphAura = spell->GetUnitTarget()->GetAura(SPELL_GLYPH_OF_STONECLAW_TOTEM, EFFECT_INDEX_0)) // Glyph of Stoneclaw Totem
        {
            int32 playerShieldStrength = shieldStrength * glyphAura->GetAmount();
            spell->GetCaster()->CastCustomSpell(spell->GetUnitTarget(), SPELL_STONECLAW_TOTEM_ABSORB, &playerShieldStrength, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct HeroismBloodlust : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const
    {
        if (!target || !target->IsAlive() || target->HasAura(57724) || target->HasAura(57723))
            return false;
        return true;
    }
};

// 1535 - Fire Nova
struct FireNovaShaman : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        // fire totems slot
        if (!spell->GetCaster()->GetTotemGuid(TOTEM_SLOT_FIRE))
            return SPELL_FAILED_TOTEMS;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        // fire totems slot
        Totem* totem = spell->GetCaster()->GetTotem(TOTEM_SLOT_FIRE);
        if (!totem)
            return;

        uint32 triggered_spell_id;
        switch (spell->m_spellInfo->Id)
        {
            case 1535:  triggered_spell_id = 8349;  break;
            case 8498:  triggered_spell_id = 8502;  break;
            case 8499:  triggered_spell_id = 8503;  break;
            case 11314: triggered_spell_id = 11306; break;
            case 11315: triggered_spell_id = 11307; break;
            case 25546: triggered_spell_id = 25535; break;
            case 25547: triggered_spell_id = 25537; break;
            case 61649: triggered_spell_id = 61650; break;
            case 61657: triggered_spell_id = 61654; break;
            default: return;
        }

        totem->CastSpell(totem, triggered_spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid());

        // Fire Nova Visual
        totem->CastSpell(nullptr, 19823, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, spell->GetCaster()->GetObjectGuid());
    }
};

// 51474 - Astral Shift
struct AstralShiftShaman : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        // while affected by Stun and Fear
        if (aura->GetTarget()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED))
            remainingDamage -= remainingDamage * currentAbsorb / 100;
    }
};

// 51505 - Lava Burst
struct LavaBurst : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->GetUnitTarget()->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, uint64(0x0000000010000000), 0, spell->GetCaster()->GetObjectGuid()))
            spell->SetGuaranteedCrit();
    }
};

// 55438 - Glyph of Lesser Healing Wave
struct GlyphOfLesserHealingWave : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            aura->GetTarget()->RegisterScriptedLocationAura(aura, SCRIPT_LOCATION_SPELL_HEALING_DONE, apply);
    }

    void OnDamageCalculate(Aura* aura, Unit* /*attacker*/, Unit* victim, int32& /*advertisedBenefit*/, float& totalMod) const override
    {
        // Earth Shield
        if (victim->GetAura(SPELL_AURA_DUMMY, SPELLFAMILY_SHAMAN, uint64(0x0000040000000000), 0, aura->GetTarget()->GetObjectGuid()))
            totalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

// 55440 - Glyph of Healing Wave
struct GlyphOfHealingWave : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        // Not proc from self heals
        if (procData.source == procData.target)
            return SPELL_AURA_PROC_FAILED;
        procData.basepoints[0] = aura->GetAmount() * procData.damage / 100;
        procData.triggerTarget = procData.source;
        procData.triggeredSpellId = 55533;
        return SPELL_AURA_PROC_OK;
    }
};

// 63280 - Glyph of Totem of Wrath
struct GlyphOfTotemOfWrath : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        Unit* source = procData.source;
        Totem* totem = source->GetTotem(TOTEM_SLOT_FIRE);
        if (!totem)
            return SPELL_AURA_PROC_FAILED;

        // find totem aura bonus
        Unit::AuraList const& spellPower = totem->GetAurasByType(SPELL_AURA_NONE);
        for (auto i : spellPower)
        {
            // select proper aura for format aura type in spell proto
            if (i->GetTarget() == totem && i->GetSpellProto()->EffectApplyAuraName[i->GetEffIndex()] == SPELL_AURA_MOD_HEALING_DONE &&
                i->GetSpellProto()->SpellFamilyName == SPELLFAMILY_SHAMAN && i->GetSpellProto()->SpellFamilyFlags & uint64(0x0000000004000000))
            {
                procData.basepoints[0] = aura->GetAmount() * i->GetModifier()->m_amount / 100;
                break;
            }
        }

        if (!procData.basepoints[0])
            return SPELL_AURA_PROC_FAILED;

        procData.basepoints[1] = procData.basepoints[0];
        procData.triggeredSpellId = 63283; // Totem of Wrath, caster bonus
        procData.triggerTarget = source;
        return SPELL_AURA_PROC_OK;
    }
};

// 52041 - Healing Stream Totem
struct HealingStreamTotemEffect : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (Unit* unitTarget = spell->GetUnitTarget())
        {
            int32 damage = spell->GetDamage();
            Unit* caster = spell->GetCaster();
            if (Unit* owner = caster->GetOwner())
            {
                // spell have SPELL_DAMAGE_CLASS_NONE and not get bonuses from owner, use main spell for bonuses
                if (spell->m_triggeredBySpellInfo)
                {
                    damage = int32(owner->SpellHealingBonusDone(unitTarget, spell->m_triggeredBySpellInfo, effIdx, damage, HEAL));
                    damage = int32(unitTarget->SpellHealingBonusTaken(owner, spell->m_triggeredBySpellInfo, effIdx, damage, HEAL));
                }

                // Restorative Totems
                Aura* restorativeTotems = owner->GetAura(16206, EFFECT_INDEX_1);
                if (!restorativeTotems)
                    restorativeTotems = owner->GetAura(16205, EFFECT_INDEX_1);
                if (!restorativeTotems)
                    restorativeTotems = owner->GetAura(16187, EFFECT_INDEX_1);

                if (restorativeTotems)
                    damage += restorativeTotems->GetModifier()->m_amount * damage / 100;

                // Glyph of Healing Stream Totem
                if (Aura* dummy = owner->GetDummyAura(55456))
                    damage += dummy->GetModifier()->m_amount * damage / 100;
            }
            caster->CastCustomSpell(unitTarget, 52042, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void LoadShamanScripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_sentry_totem";
    pNewScript->GetAI = &GetNewAIInstance<SentryTotemAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SentryTotem>("spell_sentry_totem");
    RegisterSpellScript<EarthShield>("spell_earth_shield");
    RegisterSpellScript<ManaTideTotemEffect>("spell_mana_tide_totem_effect");
    RegisterSpellScript<ItemShamanT10Elemental2PBonus>("spell_item_shaman_t10_elemental_2p_bonus");
    RegisterSpellScript<EartbindTotem>("spell_earthbind_totem");
    RegisterSpellScript<LavaLash>("spell_lava_lash");
    RegisterSpellScript<AncestralAwakening>("spell_ancestral_awakening");
    RegisterSpellScript<AncestralAwakeningSearch>("spell_ancestral_awakening_search");
    RegisterSpellScript<StoneclawTotem>("spell_stoneclaw_totem");
    RegisterSpellScript<StoneclawTotemAbsorb>("spell_stoneclaw_totem_absorb");
    RegisterSpellScript<HeroismBloodlust>("spell_heroism_bloodlust");
    RegisterSpellScript<FireNovaShaman>("spell_fire_nova_shaman");
    RegisterSpellScript<AstralShiftShaman>("spell_astral_shift_shaman");
    RegisterSpellScript<LavaBurst>("spell_lava_burst");
    RegisterSpellScript<GlyphOfLesserHealingWave>("spell_glyph_of_lesser_healing_wave");
    RegisterSpellScript<GlyphOfHealingWave>("spell_glyph_of_healing_wave");
    RegisterSpellScript<GlyphOfTotemOfWrath>("spell_glyph_of_totem_of_wrath");
    RegisterSpellScript<HealingStreamTotemEffect>("spell_healing_stream_totem_effect");
}