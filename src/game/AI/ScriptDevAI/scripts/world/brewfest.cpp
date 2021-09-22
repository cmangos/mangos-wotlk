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

struct BrewfestMountTransformation : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == ALLIANCE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct BrewfestMountTransformationFactionSwap : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster->IsPlayer())
            return;

        if (!caster->HasAuraType(SPELL_AURA_MOUNTED))
            return;

        caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

        // Ram for Alliance, Kodo for Horde
        if (static_cast<Player*>(caster)->GetTeam() == HORDE)
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Ram
                caster->CastSpell(nullptr, 43900, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Ram
                caster->CastSpell(nullptr, 43899, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            if (caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                // 100% Kodo
                caster->CastSpell(nullptr, 49379, TRIGGERED_OLD_TRIGGERED);
            else
                // 60% Kodo
                caster->CastSpell(nullptr, 49378, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void AddSC_brewfest()
{
    RegisterSpellScript<BrewfestMountTransformation>("spell_brewfest_mount_transformation");
    RegisterSpellScript<BrewfestMountTransformationFactionSwap>("spell_brewfest_mount_transformation_faction_swap");
}