/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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

/* ScriptData
SDName: Item_Scripts
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: Items
EndScriptData */

/* ContentData
item_arcane_charges                 Prevent use if player is not flying (cannot cast while on ground)
item_flying_machine(i34060,i34061)  Engineering crafted flying machines
item_gor_dreks_ointment(i30175)     Protecting Our Own(q10488)
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Spell.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"


/*#####
# item_orb_of_draconic_energy
#####*/

enum
{
    SPELL_DOMINION_SOUL     = 16053,
    NPC_EMBERSTRIFE         = 10321
};

bool ItemUse_item_orb_of_draconic_energy(Player* pPlayer, Item* pItem, const SpellCastTargets& /*pTargets*/)
{
    Creature* pEmberstrife = GetClosestCreatureWithEntry(pPlayer, NPC_EMBERSTRIFE, 20.0f);
    if (!pEmberstrife)
        return false;

    // If Emberstrife already mind controled or above 10% HP: force spell cast failure
    if (pEmberstrife->HasAura(SPELL_DOMINION_SOUL) || pEmberstrife->GetHealthPercent() > 10.0f)
    {
        pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, nullptr);

        if (SpellEntry const* pSpellInfo = GetSpellStore()->LookupEntry<SpellEntry>(SPELL_DOMINION_SOUL))
            Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_TARGET_AURASTATE);

        return true;
    }

    return false;
}

/*#####
# item_arcane_charges
#####*/

enum
{
    SPELL_ARCANE_CHARGES    = 45072
};

bool ItemUse_item_arcane_charges(Player* pPlayer, Item* pItem, const SpellCastTargets& /*pTargets*/)
{
    if (pPlayer->IsTaxiFlying())
        return false;

    pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, nullptr);

    if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry<SpellEntry>(SPELL_ARCANE_CHARGES))
        Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_NOT_ON_GROUND);

    return true;
}

/*#####
# item_flying_machine
#####*/

bool ItemUse_item_flying_machine(Player* pPlayer, Item* pItem, const SpellCastTargets& /*pTargets*/)
{
    uint32 itemId = pItem->GetEntry();

    if (itemId == 34060)
        if (pPlayer->GetSkillValueBase(SKILL_RIDING) >= 225)
            return false;

    if (itemId == 34061)
        if (pPlayer->GetSkillValueBase(SKILL_RIDING) == 300)
            return false;

    debug_log("SD2: Player attempt to use item %u, but did not meet riding requirement", itemId);
    pPlayer->SendEquipError(EQUIP_ERR_CANT_EQUIP_SKILL, pItem, nullptr);
    return true;
}

/*#####
# item_gor_dreks_ointment
#####*/

enum
{
    NPC_TH_DIRE_WOLF        = 20748,
    SPELL_GORDREKS_OINTMENT = 32578
};

bool ItemUse_item_gor_dreks_ointment(Player* pPlayer, Item* pItem, const SpellCastTargets& pTargets)
{
    if (pTargets.getUnitTarget() && pTargets.getUnitTarget()->GetTypeId() == TYPEID_UNIT && pTargets.getUnitTarget()->HasAura(SPELL_GORDREKS_OINTMENT))
    {
        pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, nullptr);

        if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry<SpellEntry>(SPELL_GORDREKS_OINTMENT))
            Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_TARGET_AURASTATE);

        return true;
    }

    return false;
}

enum
{
    SPELL_ASHBRINGER_EFFECT_001 = 28442,
};

struct AshbringerItemAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            int32 basepoints = ReputationRank(REP_FRIENDLY);
            target->CastCustomSpell(nullptr, SPELL_ASHBRINGER_EFFECT_001, &basepoints, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_ASHBRINGER_EFFECT_001);
    }
};

/*#####
# item_petrov_cluster_bombs
#####*/

enum
{
    SPELL_PETROV_BOMB           = 42406,
    AREA_ID_SHATTERED_STRAITS   = 4064,
    ZONE_ID_HOWLING             = 495
};

bool ItemUse_item_petrov_cluster_bombs(Player* pPlayer, Item* pItem, const SpellCastTargets& /*pTargets*/)
{
    if (pPlayer->GetZoneId() != ZONE_ID_HOWLING)
        return false;

    if (!pPlayer->GetTransport() || pPlayer->GetAreaId() != AREA_ID_SHATTERED_STRAITS)
    {
        pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, nullptr);

        if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry<SpellEntry>(SPELL_PETROV_BOMB))
            Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_NOT_HERE);

        return true;
    }

    return false;
}

enum
{
    SPELL_PARACHUTE = 37897,
};

struct X52RocketHelmetAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        if (!apply)
        {
            Unit* target = aura->GetTarget();
            target->CastSpell(target, SPELL_PARACHUTE, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

enum
{
    SPELL_LIMITLESS_POWER = 45044,
};

struct PowerCircleAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const
    {
        Unit* target = aura->GetTarget();
        if (apply && target->GetObjectGuid() == aura->GetCasterGuid())
            target->CastSpell(nullptr, SPELL_LIMITLESS_POWER, TRIGGERED_OLD_TRIGGERED);
        else
            target->RemoveAurasDueToSpell(SPELL_LIMITLESS_POWER);
    }
};

enum
{
    SPELL_GDR_PERIODIC_DAMAGE = 13493,
    SPELL_GDR_DAMAGE_HIT      = 13279,
};

struct GDRChannel : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
            spell->GetCaster()->CastSpell(nullptr, SPELL_GDR_PERIODIC_DAMAGE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct GDRPeriodicDamage : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& /*data*/, int32 /*value*/) const override
    {
        return urand(100, 500);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            int32 dmg = (int32)aura->GetScriptValue();
            aura->GetTarget()->CastCustomSpell(aura->GetTarget()->GetTarget(), SPELL_GDR_DAMAGE_HIT, &dmg, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct OgrilaFlasks : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0 || apply)
            return;

        SpellEntry const* spellInfo = aura->GetSpellProto();
        for (uint32 i = EFFECT_INDEX_1; i < MAX_EFFECT_INDEX; ++i)
            if (uint32 triggerSpell = spellInfo->EffectTriggerSpell[i])
                aura->GetTarget()->RemoveAurasDueToSpell(triggerSpell);
    }
};

struct ReducedProcChancePast60 : public AuraScript
{
    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) const override
    {
        holder->SetReducedProcChancePast60();
    }
};

struct BanishExile : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        uint32 entry = 0;
        switch (spell->m_spellInfo->Id)
        {
            case 4130: entry = 2760; break; // Burning Exile
            case 4131: entry = 2761; break; // Cresting Exile
            case 4132: entry = 2762; break; // Thundering Exile
        }
        if (ObjectGuid target = spell->m_targets.getUnitTargetGuid()) // can be cast only on this target
            if (target.GetEntry() != entry)
                return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        DoScriptText(-1010004, target, spell->GetCaster());
        target->CastSpell(nullptr, 3617, TRIGGERED_OLD_TRIGGERED);
    }
};

struct OrbOfDeception : public AuraScript
{
    uint32 GetAuraScriptCustomizationValue(Aura* aura) const override
    {
        uint32 orbModel = aura->GetTarget()->GetNativeDisplayId();
        uint32 resultingModel = 0; // keeping this for future research
        uint32 resultingTemplate = 0;
        switch (orbModel)
        {
            // Troll Female
            case 1479: resultingModel = 10134; break;
            // Troll Male
            case 1478: resultingModel = 10135; break;
            // Tauren Male
            case 59:   resultingModel = 10136; break;
            // Human Male
            case 49:   resultingModel = 10137; break;
            // Human Female
            case 50:   resultingModel = 10138; break;
            // Orc Male
            case 51:   resultingModel = 10139; break;
            // Orc Female
            case 52:   resultingModel = 10140; break;
            // Dwarf Male
            case 53:   resultingModel = 10141; break;
            // Dwarf Female
            case 54:   resultingModel = 10142; break;
            // NightElf Male
            case 55:   resultingModel = 10143; break;
            // NightElf Female
            case 56:   resultingModel = 10144; break;
            // Undead Female
            case 58:   resultingModel = 10145; break;
            // Undead Male
            case 57:   resultingModel = 10146; break;
            // Tauren Female
            case 60:   resultingModel = 10147; break;
            // Gnome Male
            case 1563: resultingModel = 10148; break;
            // Gnome Female
            case 1564: resultingModel = 10149;break;
            // BloodElf Female
            case 15475: resultingModel = 17830;break;
            // BloodElf Male
            case 15476: resultingModel = 17829;break;
            // Dranei Female
            case 16126: resultingModel = 17828;break;
            // Dranei Male
            case 16125: resultingModel = 17827;break;
            default: break;
        }
        return resultingModel;
    }
};

// 39228 - Argussian Compass
struct ArgussianCompass : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& /*remainingDamage*/, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& /*preventedDeath*/, bool& /*dropCharge*/, DamageEffectType /*damageType*/) const override
    {
        // Max absorb stored in 1 dummy effect
        int32 max_absorb = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
        if (max_absorb < currentAbsorb)
            currentAbsorb = max_absorb;
    }
}; 

enum
{
    SPELL_MALFUNCTION_EXPLOSION = 13261,
    SPELL_SUMMON_GOBLIN_BOMB    = 13258,
    SPELL_PET_BOMB_PASSIVE      = 13260, // triggers 13259 Explosion on successful melee hit
};

// 13258 - Summon Goblin Bomb
struct SummonGoblinBomb : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (Player* player = dynamic_cast<Player*>(spell->GetCaster()))
            summon->SelectLevel(uint16(player->GetSkillValue(SKILL_ENGINEERING) / 5));
        summon->AI()->SetReactState(REACT_AGGRESSIVE);
        summon->CastSpell(summon, SPELL_PET_BOMB_PASSIVE, TRIGGERED_OLD_TRIGGERED);
    }
};

// 23134 - Goblin Bomb
struct GoblinBomb : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (!target || !caster)
            return;
        uint32 roll = urand(0, 99);
        if (roll < 10)
            target->CastSpell(caster, SPELL_MALFUNCTION_EXPLOSION, TRIGGERED_OLD_TRIGGERED);
        else
            caster->CastSpell(caster, SPELL_SUMMON_GOBLIN_BOMB, TRIGGERED_OLD_TRIGGERED);
    }
};

// 11403 - Dream Vision
struct DreamVision : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        if (summon->GetEntry() != 7863)
            return;

        summon->SetHover(true);
        summon->SetWaterWalk(true);
        summon->SetFeatherFall(true);
    }

    void OnRadiusCalculate(Spell* /*spell*/, SpellEffectIndex effIdx, bool /*targetB*/, float& radius) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        radius = 2.f;
    }
};

void AddSC_item_scripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "item_orb_of_draconic_energy";
    pNewScript->pItemUse = &ItemUse_item_orb_of_draconic_energy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "item_arcane_charges";
    pNewScript->pItemUse = &ItemUse_item_arcane_charges;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "item_flying_machine";
    pNewScript->pItemUse = &ItemUse_item_flying_machine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "item_gor_dreks_ointment";
    pNewScript->pItemUse = &ItemUse_item_gor_dreks_ointment;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "item_petrov_cluster_bombs";
    pNewScript->pItemUse = &ItemUse_item_petrov_cluster_bombs;
    pNewScript->RegisterSelf();

    RegisterSpellScript<AshbringerItemAura>("spell_ashbringer_item");
    RegisterSpellScript<X52RocketHelmetAura>("spell_to_infinity_and_above");
    RegisterSpellScript<PowerCircleAura>("spell_power_circle");

    RegisterSpellScript<GDRChannel>("spell_gdr_channel");
    RegisterSpellScript<GDRPeriodicDamage>("spell_gdr_periodic");
    RegisterSpellScript<OgrilaFlasks>("spell_ogrila_flasks");
    RegisterSpellScript<ReducedProcChancePast60>("spell_reduced_proc_chance_past60");
    RegisterSpellScript<BanishExile>("spell_banish_exile");
    RegisterSpellScript<OrbOfDeception>("spell_orb_of_deception");
    RegisterSpellScript<ArgussianCompass>("spell_argussian_compass");
    RegisterSpellScript<SummonGoblinBomb>("spell_summon_goblin_bomb");
    RegisterSpellScript<GoblinBomb>("spell_goblin_bomb");
    RegisterSpellScript<DreamVision>("spell_dream_vision");
}
