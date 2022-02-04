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

struct KillCommand : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        if (spell->GetCaster()->HasAura(37483)) // Improved Kill Command - Item set bonus
            spell->GetCaster()->CastSpell(nullptr, 37482, TRIGGERED_OLD_TRIGGERED);// Exploited Weakness
    }
};

struct Misdirection : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool/* strict*/) const override
    {
        // Patch 2.3.0 (2007-11-13):
        // Misdirection: If a Hunter attempts to use this ability on a target which already has an active Misdirection, the spell will fail to apply due to a more powerful spell already being in effect.
        if (Unit* target = spell->m_targets.getUnitTarget())
        {
            if (target->HasAura(35079))
                return SPELL_FAILED_AURA_BOUNCED;
            if (target->IsMounted())
                return SPELL_FAILED_NOT_ON_MOUNTED;
        }

        return SPELL_CAST_OK;
    }
};

struct ExposeWeakness : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.caster)
            value = (data.caster->GetStat(STAT_AGILITY) * value) / 100;

        return value;
    }
};

struct Disengage : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->GetCaster()->IsInCombat())
            return SPELL_FAILED_CASTER_AURASTATE;

        return SPELL_CAST_OK;
    }
};

struct RoarOfSacrifice : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        Unit* pet = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!pet)
            return SPELL_AURA_PROC_OK;

        int32 damage = procData.damage;
        target->CastCustomSpell(pet, 67481, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return SPELL_AURA_PROC_OK;
    }
};

void LoadHunterScripts()
{
    RegisterSpellScript<KillCommand>("spell_kill_command");
    RegisterSpellScript<Misdirection>("spell_misdirection");
    RegisterAuraScript<ExposeWeakness>("spell_expose_weakness");
    RegisterSpellScript<Disengage>("spell_disengage");
    RegisterAuraScript<RoarOfSacrifice>("spell_roar_of_sacrifice");
}
