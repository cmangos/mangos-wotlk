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

struct SwiftHandOfJustice : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.basepoints[EFFECT_INDEX_0] = aura->GetTarget()->GetMaxHealth() * aura->GetAmount() / 100;
        procData.triggeredSpellId = 59913;
        procData.triggerTarget = nullptr;
        return SPELL_AURA_PROC_OK;
    }
};

// 59915 - Discerning Eye of the Beast
struct DiscerningEyeOfTheBeast : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = 59914;
        procData.triggerTarget = nullptr;
        return SPELL_AURA_PROC_OK;
    }
};

// 75475 -  Item - Chamber of Aspects 25 Tank Trinket
struct ProcOnlyBelow35Percent : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& data) const override
    {
        if (data.victim->GetHealthPercent() < 35.f)
            return true;

        return false;
    }
};

enum
{
    SPELL_STRENGTH_OF_THE_TAUNKA = 71484, // STR
    SPELL_SPEED_OF_THE_VRYKUL = 71492, // HASTE
    SPELL_POWER_OF_THE_TAUNKA = 71486, // AP
    SPELL_AIM_OF_THE_IRON_DWARVES = 71491, // CRIT
    SPELL_AGILITY_OF_THE_VRYKUL = 71485, // AGI

    SPELL_STRENGTH_OF_THE_TAUNKA_H = 71561, // STR
    SPELL_SPEED_OF_THE_VRYKUL_H = 71560, // HASTE
    SPELL_POWER_OF_THE_TAUNKA_H = 71558, // AP
    SPELL_AIM_OF_THE_IRON_DWARVES_H = 71559, // CRIT
    SPELL_AGILITY_OF_THE_VRYKUL_H = 71556, // AGI
};

const std::unordered_map<Classes, const std::vector<uint32>> classMap =
{
    {CLASS_WARRIOR, {SPELL_STRENGTH_OF_THE_TAUNKA, SPELL_SPEED_OF_THE_VRYKUL, SPELL_AIM_OF_THE_IRON_DWARVES}},
    {CLASS_DEATH_KNIGHT, {SPELL_STRENGTH_OF_THE_TAUNKA, SPELL_SPEED_OF_THE_VRYKUL, SPELL_AIM_OF_THE_IRON_DWARVES}},
    {CLASS_PALADIN, {SPELL_STRENGTH_OF_THE_TAUNKA, SPELL_SPEED_OF_THE_VRYKUL, SPELL_AIM_OF_THE_IRON_DWARVES}},
    {CLASS_SHAMAN, {SPELL_SPEED_OF_THE_VRYKUL, SPELL_AGILITY_OF_THE_VRYKUL, SPELL_POWER_OF_THE_TAUNKA}},
    {CLASS_DRUID, {SPELL_SPEED_OF_THE_VRYKUL, SPELL_AGILITY_OF_THE_VRYKUL, SPELL_POWER_OF_THE_TAUNKA, SPELL_AIM_OF_THE_IRON_DWARVES}},
    {CLASS_ROGUE, {SPELL_SPEED_OF_THE_VRYKUL, SPELL_AGILITY_OF_THE_VRYKUL, SPELL_POWER_OF_THE_TAUNKA}},
    {CLASS_HUNTER, {SPELL_POWER_OF_THE_TAUNKA, SPELL_AIM_OF_THE_IRON_DWARVES, SPELL_AGILITY_OF_THE_VRYKUL}}
};

const std::unordered_map<Classes, const std::vector<uint32>> classMapHeroic =
{
    {CLASS_WARRIOR, {SPELL_STRENGTH_OF_THE_TAUNKA_H, SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AIM_OF_THE_IRON_DWARVES_H}},
    {CLASS_DEATH_KNIGHT, {SPELL_STRENGTH_OF_THE_TAUNKA_H, SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AIM_OF_THE_IRON_DWARVES_H}},
    {CLASS_PALADIN, {SPELL_STRENGTH_OF_THE_TAUNKA_H, SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AIM_OF_THE_IRON_DWARVES_H}},
    {CLASS_SHAMAN, {SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AGILITY_OF_THE_VRYKUL_H, SPELL_POWER_OF_THE_TAUNKA_H}},
    {CLASS_DRUID, {SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AGILITY_OF_THE_VRYKUL_H, SPELL_POWER_OF_THE_TAUNKA_H, SPELL_AIM_OF_THE_IRON_DWARVES_H}},
    {CLASS_ROGUE, {SPELL_SPEED_OF_THE_VRYKUL_H, SPELL_AGILITY_OF_THE_VRYKUL_H, SPELL_POWER_OF_THE_TAUNKA_H}},
    {CLASS_HUNTER, {SPELL_POWER_OF_THE_TAUNKA_H, SPELL_AIM_OF_THE_IRON_DWARVES_H, SPELL_AGILITY_OF_THE_VRYKUL_H}}
};

struct Icecrown25MeleeTrinket : public AuraScript
{
    void OnAuraInit(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        aura->SetScriptValue(classMap.find(Classes(target->getClass())) == classMap.end());
        if (aura->GetScriptValue() == 1)
            sLog.outError("Icecrown25MeleeTrinket called for unknown class %u", target->getClass());
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (aura->GetScriptValue() == 1)
            return SPELL_AURA_PROC_FAILED;

        bool heroic = false;
        if (aura->GetSpellProto()->Id == 71562)
            heroic = true;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return SPELL_AURA_PROC_FAILED;
        Classes characterClass = Classes(caster->getClass());        
        std::unordered_map<Classes, const std::vector<uint32>>::const_iterator itr;
        if (heroic)
            itr = classMapHeroic.find(characterClass);
        else
            itr = classMap.find(characterClass);
        const std::vector<uint32>& spellArray = itr->second;
        if (!caster->IsAlive() || spellArray.empty())
            return SPELL_AURA_PROC_FAILED;
        uint32 spellToCast = spellArray[urand(0, spellArray.size() - 1)];
        procData.triggeredSpellId = spellToCast;
        procData.cooldown = std::chrono::seconds(1min + 45s).count();
        return SPELL_AURA_PROC_OK;
    }
};

void AddSC_item_scripts_wotlk()
{
    RegisterSpellScript<SwiftHandOfJustice>("spell_swift_hand_of_justice");
    RegisterSpellScript<DiscerningEyeOfTheBeast>("spell_discerning_eye_of_the_beast");
    RegisterSpellScript<ProcOnlyBelow35Percent>("spell_proc_only_below_35_percent");
    RegisterSpellScript<Icecrown25MeleeTrinket>("spell_icecrown_25_melee_trinket");
}