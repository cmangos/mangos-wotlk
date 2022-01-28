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

struct ScourgeStrike : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        uint32 count = 0;
        Unit* target = spell->GetUnitTarget();
        Unit::SpellAuraHolderMap const& auras = target->GetSpellAuraHolderMap();
        for (const auto& aura : auras)
        {
            if (aura.second->GetSpellProto()->Dispel == DISPEL_DISEASE &&
                aura.second->GetCasterGuid() == spell->GetCaster()->GetObjectGuid())
                ++count;
        }

        if (count)
        {
            int32 bp = count * spell->CalculateSpellEffectValue(EFFECT_INDEX_2, target) * spell->GetTotalTargetDamage() / 100;
            if (bp)
                spell->GetCaster()->CastCustomSpell(target, 70890, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct RaiseDead : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (eff == EFFECT_INDEX_1)
        {
            Unit* caster = spell->GetCaster();
            if (target->IsTrivialForTarget(caster))
                return false;

            if (target->GetDeathState() != CORPSE)
                return false;

            if (target->GetCreatureType() != CREATURE_TYPE_HUMANOID)
                return false;
        }

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_2) // reagent consuming version
            if (spell->GetTargetList().size() > 1) // more than caster is hit
                return;

        Unit* caster = spell->GetCaster();
        uint32 spellId = caster->HasAura(52143) ? 52150 : 46585; // Master of Ghouls talent
        if (effIdx == EFFECT_INDEX_2) // corpse dust version
        {
            if (caster->CastSpell(nullptr, 48289, TRIGGERED_IGNORE_GCD) == SPELL_CAST_OK)
            {
                Position pos = Pet::GetPetSpawnPosition(caster);
                caster->CastSpell(pos.x, pos.y, pos.z, spellId, TRIGGERED_OLD_TRIGGERED);
            }
            else
                caster->RemoveSpellCooldown(*spell->m_spellInfo, true);
        }
        else if (effIdx == EFFECT_INDEX_1) // corpse version
            caster->CastSpell(spell->GetUnitTarget(), spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

void LoadDeathKnightScripts()
{
    RegisterSpellScript<ScourgeStrike>("spell_scourge_strike");
    RegisterSpellScript<RaiseDead>("spell_dk_raise_dead");
}