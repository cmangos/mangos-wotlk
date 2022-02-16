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

struct SentryTotem : public SpellScript, public AuraScript
{
    void OnRadiusCalculate(Spell* spell, SpellEffectIndex effIdx, bool targetB, float& radius) const override
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

struct EarthShield : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        Unit* target = data.target;
        if (Unit* caster = data.caster)
        {
            value = caster->SpellHealingBonusDone(target, data.spellProto, value, SPELL_DIRECT_DAMAGE);
            value = target->SpellHealingBonusTaken(caster, data.spellProto, value, SPELL_DIRECT_DAMAGE);
        }
        return value;
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

void LoadShamanScripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_sentry_totem";
    pNewScript->GetAI = &GetNewAIInstance<SentryTotemAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SentryTotem>("spell_sentry_totem");
    RegisterSpellScript<EarthShield>("spell_earth_shield");
    RegisterSpellScript<ItemShamanT10Elemental2PBonus>("spell_item_shaman_t10_elemental_2p_bonus");
    RegisterSpellScript<EartbindTotem>("spell_earthbind_totem");
    RegisterSpellScript<LavaLash>("spell_lava_lash");
    RegisterSpellScript<AncestralAwakening>("spell_ancestral_awakening");
    RegisterSpellScript<AncestralAwakeningSearch>("spell_ancestral_awakening_search");
}