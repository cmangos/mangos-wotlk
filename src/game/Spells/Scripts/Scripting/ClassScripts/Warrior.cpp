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

// 5308 - Execute
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

// 20647 - Execute
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

// 34428 - Victory Rush
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

// 58567 - Sunder Armor
struct SunderArmor : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        if (spell->GetCaster()->HasAura(58387)) // Glyph of Sunder Armor
        {
            spell->SetChainTargetsCount(EFFECT_INDEX_0, 2);
            spell->SetChainTargetsCount(EFFECT_INDEX_1, 2);
        }
    }
};

// 20243 - Devastate
struct WarriorDevastate : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
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

        if (Aura* glyphAura = caster->GetAura(58388, EFFECT_INDEX_0))
            sunders = 2;

        // Devastate causing Sunder Armor Effect
        // and no need to cast over max stack amount
        if (!sunder || sunder->GetStackAmount() < sunder->GetSpellProto()->StackAmount)
        {
            for (uint32 i = 0; i < sunders; ++i)
                caster->CastSpell(target, 58567, TRIGGERED_IGNORE_HIT_CALCULATION | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS);
        }
    }
};

// 20230 - Retaliation
struct RetaliationWarrior : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        // check attack comes not from behind
        if (procData.victim->IsFacingTargetsBack(procData.attacker))
            return SPELL_AURA_PROC_FAILED;

        procData.victim->CastSpell(procData.attacker, 20240, TRIGGERED_IGNORE_HIT_CALCULATION | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS);
        return SPELL_AURA_PROC_OK;
    }
};

// 29707 - Heroic Strike
struct HeroicStrike : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit::AuraList const& decSpeedList = target->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
        for (Unit::AuraList::const_iterator iter = decSpeedList.begin(); iter != decSpeedList.end(); ++iter)
        {
            if ((*iter)->GetSpellProto()->SpellIconID == 15 && (*iter)->GetSpellProto()->Dispel == 0)
            {
                // formula based on tooltip
                spell->SetDamage(spell->GetDamage() + spell->m_spellInfo->EffectBasePoints[EFFECT_INDEX_0]);
                break;
            }
        }
    }
};

// 7384 - Overpower
struct Overpower : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            return;

        Player* caster = dynamic_cast<Player*>(aura->GetCaster());
        Unit* target = aura->GetTarget();
        if (!caster || !target->IsNonMeleeSpellCasted(false))
            return;

        if (Aura* aura = caster->GetKnownTalentRankAuraById(1860, EFFECT_INDEX_0)) // Unrelenting Assault talent
        {
            uint32 spellId = 0;
            switch (aura->GetId())
            {
                case 46859: spellId = 64849; break; // Unrelenting Assault, rank 1
                case 46860: spellId = 64850; break; // Unrelenting Assault, rank 2
                default: break;
            }
            if (spellId)
                target->CastSpell(nullptr, spellId, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        }
    }
};

// 23920 - Spell Reflection
struct SpellReflection : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        if (Player* caster = dynamic_cast<Player*>(spell->GetCaster()))
            if (Aura* dummy = caster->GetKnownTalentRankAuraById(2247, EFFECT_INDEX_1)) // Improved Spell Reflections
                caster->CastSpell(nullptr, 59725, TRIGGERED_OLD_TRIGGERED);
    }
};

// 59725 - Spell Reflection
struct SpellReflectionRaid : public SpellScript, public AuraScript
{
    void OnInit(Spell* spell) const override
    {
        if (Player* caster = dynamic_cast<Player*>(spell->GetCaster()))
            if (Aura* dummy = caster->GetKnownTalentRankAuraById(2247, EFFECT_INDEX_1)) // Improved Spell Reflections
                spell->SetMaxAffectedTargets(dummy->GetAmount());

        spell->SetFilteringScheme(EFFECT_INDEX_0, false, SCHEME_CLOSEST);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster() == target)
            return false;
        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
        {
            if (Player* player = dynamic_cast<Player*>(aura->GetTarget()))
            {
                if (Group* group = player->GetGroup())
                {
                    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                    {
                        if (itr->getSource()->IsInMap(player))
                        {
                            // avoid infinite loop
                            itr->getSource()->RemoveAurasByCasterSpell(aura->GetId(), aura->GetCasterGuid(), AURA_REMOVE_BY_CANCEL);
                        }
                    }
                }
            }
        }
    }
};

// 50720 - Vigilance
struct Vigilance : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
        procData.triggerTarget = aura->GetCaster();
        return SPELL_AURA_PROC_OK;
    }
};

// 50725 - Vigilance
struct VigilanceTrigger : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        target->RemoveSpellCategoryCooldown(82, true); // remove cooldown on Taunt
    }
};

// 3411 - Intervene
struct Intervene : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        if (spell->GetCaster()->GetOverrideScript(6953))
            spell->SetIgnoreRoot(true);
    }

    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK, 0, true);
        // temporary hack to investigate root flag changes serverside - necessary to make it work
        spell->GetCaster()->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ROOT);
    }
};

void LoadWarriorScripts()
{
    RegisterSpellScript<WarriorExecute>("spell_warrior_execute");
    RegisterSpellScript<WarriorExecuteDamage>("spell_warrior_execute_damage");
    RegisterSpellScript<VictoryRush>("spell_warrior_victory_rush");
    RegisterSpellScript<SunderArmor>("spell_sunder_armor");
    RegisterSpellScript<WarriorDevastate>("spell_warrior_devastate");
    RegisterSpellScript<RetaliationWarrior>("spell_retaliation_warrior");
    RegisterSpellScript<HeroicStrike>("spell_heroic_strike");
    RegisterSpellScript<Overpower>("spell_overpower");
    RegisterSpellScript<SpellReflection>("spell_spell_reflection");
    RegisterSpellScript<SpellReflectionRaid>("spell_spell_reflection_raid");
    RegisterSpellScript<Vigilance>("spell_vigilance");
    RegisterSpellScript<VigilanceTrigger>("spell_vigilance_trigger");
    RegisterSpellScript<Intervene>("spell_intervene");
}