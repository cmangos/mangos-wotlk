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
SDName: Oculus
SD%Complete: 100
SDComment: Spell Scripts
SDCategory: Oculus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "oculus.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

/*######
## spell_call_oculus_drake - 49345, 49461, 49462
######*/

struct spell_call_oculus_drake : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        if (!caster)
            return;

        uint32 triggerSpellId;

        switch (spell->m_spellInfo->Id)
        {
            case 49345: triggerSpellId = 49427; break;          // emerald drake
            case 49461: triggerSpellId = 49459; break;          // amber drake
            case 49462: triggerSpellId = 49463; break;          // ruby drake
            default: return;
        }

        // apply aura that will board the drake
        caster->CastSpell(caster, triggerSpellId, TRIGGERED_NONE);
    }
};

/*######
## spell_ride_oculus_drake_saddle - 49346, 49460, 49464
######*/

struct spell_ride_oculus_drake_saddle : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        // control vehicle applies a visual flag on player
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!caster || !caster->IsPlayer() || !target)
            return;

        if (apply)
        {
            // apply drake flag visual
            caster->CastSpell(caster, 53797, TRIGGERED_OLD_TRIGGERED);
            // apply flight aura
            target->CastSpell(target, 50296, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            // remove drake flag visual
            caster->RemoveAurasDueToSpell(53797);
            // apply parachute aura
            caster->CastSpell(caster, 50550, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## spell_parachute_aura - 50550
######*/

struct spell_parachute_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        // apply parachute aura (50553) if player doesn't have it already
        uint32 uiSpell = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0);

        if (target->IsFalling() && !target->HasAura(uiSpell))
            target->CastSpell(target, uiSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_oculus()
{
    RegisterSpellScript<spell_call_oculus_drake>("spell_call_oculus_drake");
    RegisterSpellScript<spell_ride_oculus_drake_saddle>("spell_ride_oculus_drake_saddle");
    RegisterSpellScript<spell_parachute_aura>("spell_parachute_aura");
}
