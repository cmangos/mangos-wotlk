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

struct WarriorExecute : public SpellScript
{
    void OnCast(Spell* spell) const override // confirmed main spell can not hit and child still hits
    {
        int32 basePoints0 = spell->GetCaster()->CalculateSpellEffectValue(spell->m_targets.getUnitTarget(), spell->m_spellInfo, SpellEffectIndex(0))
            + int32((spell->GetCaster()->GetPower(POWER_RAGE)) * spell->m_spellInfo->DmgMultiplier[0]);
        SpellCastResult result = spell->GetCaster()->CastCustomSpell(spell->m_targets.getUnitTarget(), 20647, &basePoints0, nullptr, nullptr, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
    }
};

enum
{
    SPELL_SUDDEN_DEATH = 52437,
};

struct WarriorExecuteDamage : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE)
            return;

        uint32 rage = spell->GetCaster()->GetPower(POWER_RAGE);

        // up to max 30 rage cost
        if (rage > 300)
            rage = 300;
        // Sudden Death
        if (spell->GetCaster()->HasAura(52437))
        {
            Unit::AuraList const& auras = spell->GetCaster()->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
            for (auto aura : auras)
            {
                // Only Sudden Death have this SpellIconID with SPELL_AURA_PROC_TRIGGER_SPELL
                if (aura->GetSpellProto()->SpellIconID == 1989)
                {
                    // saved rage top stored in next affect
                    uint32 lastrage = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1) * 10;
                    if (lastrage < rage)
                        rage -= lastrage;
                    break;
                }
            }
        }
        spell->GetCaster()->SetPower(POWER_RAGE, rage);
    }
};

struct VictoryRush : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, false);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->SetDamage(uint32(spell->GetDamage() * spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 100));
    }
};

struct SunderArmor : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        if (spell->GetCaster()->HasAura(58387)) // Glyph of Sunder Armor
            spell->SetMaxAffectedTargets(2);
    }
};

struct WarriorDevastate : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        uint32 sunders = 1;

        // Sunder Armor
        Aura* sunder = target->GetAura(SPELL_AURA_MOD_RESISTANCE_PCT, SPELLFAMILY_WARRIOR, uint64(0x0000000000004000), 0x00000000, caster->GetObjectGuid());

        // Devastate bonus and sunder armor refresh
        if (sunder)
        {
            sunder->GetHolder()->RefreshHolder();
            spell->SetDamage(spell->GetDamage() + sunder->GetStackAmount() * spell->CalculateSpellEffectValue(EFFECT_INDEX_2, target));
        }

        if (Aura* glyphAura = caster->GetAura(63332, EFFECT_INDEX_0))
            sunders = 2;

        // Devastate causing Sunder Armor Effect
        // and no need to cast over max stack amount
        if (!sunder || sunder->GetStackAmount() < sunder->GetSpellProto()->StackAmount)
        {
            for (uint32 i = 0; i < sunders; ++i)
                caster->CastSpell(target, 58567, TRIGGERED_IGNORE_HIT_CALCULATION | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS);
        }
    }
};

void LoadWarriorScripts()
{
    RegisterSpellScript<WarriorExecute>("spell_warrior_execute");
    RegisterSpellScript<WarriorExecuteDamage>("spell_warrior_execute_damage");
    RegisterSpellScript<VictoryRush>("spell_warrior_victory_rush");
    RegisterSpellScript<SunderArmor>("spell_sunder_armor");
    RegisterSpellScript<WarriorDevastate>("spell_warrior_devastate");
}