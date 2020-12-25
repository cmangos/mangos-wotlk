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
SDName: ruby_sanctum scripts
SD%Complete: 100%
SDComment: Spell scripts for Ruby Sanctum
SDCategory: Ruby Sanctum
EndScriptData */

/* ContentData
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ruby_sanctum.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*######
## spell_rallying_shout - 75415
######*/

struct spell_rallying_shout : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        uint32 targetCount = spell->GetTargetList().size();

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // if unit already has aura, make sure it's stacked right
        if (target->HasAura(75416))
        {
            if (SpellAuraHolder* rallyAura = target->GetSpellAuraHolder(75416))
                if (rallyAura->GetStackAmount() != targetCount)
                    rallyAura->SetStackAmount(targetCount, target);
        }
        else
        {
            // stack Rally based on the target count
            for (uint8 i = 0; i < targetCount; ++i)
                target->CastSpell(target, 75416, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

void AddSC_ruby_sanctum()
{
    RegisterSpellScript<spell_rallying_shout>("spell_rallying_shout");
}
