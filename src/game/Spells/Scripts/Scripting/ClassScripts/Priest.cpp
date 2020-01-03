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

enum
{
    SPELL_PLAYER_CONSUME_MAGIC = 32676,
};

struct ConsumeMagicSpellScript : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        if (strict)
        {
            auto holderMap = spell->GetCaster()->GetSpellAuraHolderMap();
            for (auto holderPair : holderMap)
            {
                if (holderPair.second->GetSpellProto())
                {
                    if (holderPair.second->GetSpellProto()->SpellFamilyName == SPELLFAMILY_PRIEST)
                    {
                        if (holderPair.second->IsPositive() && !holderPair.second->IsPassive())
                        {
                            spell->SetScriptValue(holderPair.second->GetId());
                            return SPELL_CAST_OK;
                        }
                    }
                }
            }

            return SPELL_FAILED_NOTHING_TO_DISPEL;
        }
        else
            return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        spell->GetCaster()->RemoveAurasDueToSpell(spell->GetScriptValue());
    }
};

struct ShadowWordDeath : public SpellScript
{
    void OnHit(Spell* spell) const override
    {
        int32 swdDamage = spell->GetTotalTargetDamage();
        spell->GetCaster()->CastCustomSpell(nullptr, 32409, &swdDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

void LoadPriestScripts()
{
    RegisterSpellScript<ConsumeMagicSpellScript>("spell_consume_magic");
    RegisterSpellScript<ShadowWordDeath>("spell_shadow_word_death");
}