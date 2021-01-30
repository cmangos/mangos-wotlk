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
SDName: boss_eck
SD%Complete: 100%
SDComment: Script handled in EAI
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"
#include "Spells/Scripts/SpellScript.h"

/*######
## spell_eck_spit - 55814
######*/

struct spell_eck_spit : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // trigger Eck Residue on target
        target->CastSpell(target, 55817, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_eck()
{
    RegisterSpellScript<spell_eck_spit>("spell_eck_spit");
}
