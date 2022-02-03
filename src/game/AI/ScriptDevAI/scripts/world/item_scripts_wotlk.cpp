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
 SDName: Item_Scripts
 SD%Complete: 100
 SDComment: Items for a range of different items. See content below (in script)
 SDCategory: Items
 EndScriptData */

 /* ContentData
 item_arcane_charges                 Prevent use if player is not flying (cannot cast while on ground)
 item_flying_machine(i34060,i34061)  Engineering crafted flying machines
 item_gor_dreks_ointment(i30175)     Protecting Our Own(q10488)
 EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Spell.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

struct SwiftHandOfJustice : public AuraScript
{
	SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
	{
		procData.basepoints[EFFECT_INDEX_0] = aura->GetTarget()->GetMaxHealth() * aura->GetAmount() / 100;
		procData.triggeredSpellId = 59913;
		procData.triggerTarget = nullptr;
		return SPELL_AURA_PROC_OK;
	}
};

struct DiscerningEyeOfTheBeast : public AuraScript
{
	SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
	{
		procData.triggeredSpellId = 59913;
		procData.triggerTarget = nullptr;
		return SPELL_AURA_PROC_OK;
	}
};

void AddSC_item_scripts_wotlk()
{
	RegisterAuraScript<SwiftHandOfJustice>("spell_swift_hand_of_justice");
	RegisterAuraScript<DiscerningEyeOfTheBeast>("spell_discerning_eye_of_the_beast");
}