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

#include "AI/ScriptDevAI/include/sc_common.h"

struct Replenishment : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(10);
        spell->SetFilteringScheme(EFFECT_INDEX_0, false, SCHEME_PRIORITIZE_MANA);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (caster->GetMap()->IsBattleArena()) // in arenas only hits caster
            if (target != caster)
                return false;

        return true;
    }
};

struct RetaliationDummyCreature : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        // check attack comes not from behind
        if (procData.victim->IsFacingTargetsBack(procData.attacker))
            return SPELL_AURA_PROC_FAILED;

        procData.victim->CastSpell(procData.attacker, 65934, TRIGGERED_IGNORE_HIT_CALCULATION | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS);
        return SPELL_AURA_PROC_OK;
    }
};

struct Shadowmeld : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        caster->CastSpell(nullptr, 62196, TRIGGERED_OLD_TRIGGERED);
        std::vector<Unit*> removal;
        for (auto& data : caster->getHostileRefManager())
        {
            if (data.getSource()->getThreatList().size() == 1)
                removal.push_back(data.getSource()->getOwner());
        }
        for (Unit* enemy : removal)
            enemy->getThreatManager().modifyThreatPercent(caster, -101);
    }
};

struct StoicismAbsorb : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster)
            value = data.caster->GetMaxHealth() * 0.20f;
        return value;
    }
};

// 64568 - Blood Reserve
struct BloodReserveEnchant : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        if (aura->GetTarget()->GetHealthPercent() >= 35.f)
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        procData.triggeredSpellId = 64569;
        procData.triggerTarget = nullptr;

        // need scale damage base at stack size
        if (SpellEntry const* trigEntry = sSpellTemplate.LookupEntry<SpellEntry>(procData.triggeredSpellId))
            procData.basepoints[EFFECT_INDEX_0] = trigEntry->CalculateSimpleValue(EFFECT_INDEX_0) * aura->GetStackAmount();

        aura->GetTarget()->RemoveAurasDueToSpell(64568); // uses stacks so wont autoremove itself
        return SPELL_AURA_PROC_OK;
    }
};

void AddSC_spell_scripts_wotlk()
{
    RegisterSpellScript<Replenishment>("spell_replenishment");
    RegisterSpellScript<RetaliationDummyCreature>("spell_retaliation_dummy_creature");
    RegisterSpellScript<Shadowmeld>("spell_shadowmeld");
    RegisterSpellScript<StoicismAbsorb>("spell_stoicism");
    RegisterSpellScript<BloodReserveEnchant>("spell_blood_reserve_enchant");
}