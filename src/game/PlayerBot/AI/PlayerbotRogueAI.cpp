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

#include "PlayerbotRogueAI.h"
#include "../Base/PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotRogueAI::PlayerbotRogueAI(Player& master, Player& bot, PlayerbotAI& ai) : PlayerbotClassAI(master, bot, ai)
{
    SINISTER_STRIKE          = m_ai.initSpell(SINISTER_STRIKE_1);
    BACKSTAB                 = m_ai.initSpell(BACKSTAB_1);
    KICK                     = m_ai.initSpell(KICK_1);
    FEINT                    = m_ai.initSpell(FEINT_1);
    FAN_OF_KNIVES            = m_ai.initSpell(FAN_OF_KNIVES_1);
    GOUGE                    = m_ai.initSpell(GOUGE_1);
    SPRINT                   = m_ai.initSpell(SPRINT_1);

    SHADOWSTEP               = m_ai.initSpell(SHADOWSTEP_1);
    STEALTH                  = m_ai.initSpell(STEALTH_1);
    VANISH                   = m_ai.initSpell(VANISH_1);
    EVASION                  = m_ai.initSpell(EVASION_1);
    CLOAK_OF_SHADOWS         = m_ai.initSpell(CLOAK_OF_SHADOWS_1);
    HEMORRHAGE               = m_ai.initSpell(HEMORRHAGE_1);
    GHOSTLY_STRIKE           = m_ai.initSpell(GHOSTLY_STRIKE_1);
    SHADOW_DANCE             = m_ai.initSpell(SHADOW_DANCE_1);
    BLIND                    = m_ai.initSpell(BLIND_1);
    DISTRACT                 = m_ai.initSpell(DISTRACT_1);
    PREPARATION              = m_ai.initSpell(PREPARATION_1);
    PREMEDITATION            = m_ai.initSpell(PREMEDITATION_1);
    PICK_POCKET              = m_ai.initSpell(PICK_POCKET_1);

    EVISCERATE               = m_ai.initSpell(EVISCERATE_1);
    KIDNEY_SHOT              = m_ai.initSpell(KIDNEY_SHOT_1);
    SLICE_DICE               = m_ai.initSpell(SLICE_AND_DICE_1);
    GARROTE                  = m_ai.initSpell(GARROTE_1);
    EXPOSE_ARMOR             = m_ai.initSpell(EXPOSE_ARMOR_1);
    RUPTURE                  = m_ai.initSpell(RUPTURE_1);
    DISMANTLE                = m_ai.initSpell(DISMANTLE_1);
    CHEAP_SHOT               = m_ai.initSpell(CHEAP_SHOT_1);
    AMBUSH                   = m_ai.initSpell(AMBUSH_1);
    MUTILATE                 = m_ai.initSpell(MUTILATE_1);

    RECENTLY_BANDAGED   = 11196; // first aid check
    // racial
    ARCANE_TORRENT           = m_ai.initSpell(ARCANE_TORRENT_ROGUE);
    STONEFORM                = m_ai.initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST            = m_ai.initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF    = m_ai.initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD               = m_ai.initSpell(SHADOWMELD_ALL);
    BLOOD_FURY               = m_ai.initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    BERSERKING               = m_ai.initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN     = m_ai.initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

CombatManeuverReturns PlayerbotRogueAI::DoFirstCombatManeuver(Unit* pTarget)
{
    // There are NPCs in BGs and Open World PvP, so don't filter this on PvP scenarios (of course if PvP targets anyone but tank, all bets are off anyway)
    // Wait until the tank says so, until any non-tank gains aggro or X seconds - whichever is shortest
    if (m_ai.GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO)
    {
        if (m_WaitUntil > m_ai.CurrentTime() && m_ai.GroupTankHoldsAggro())
        {
            return RETURN_NO_ACTION_OK; // wait it out
        }
        else
        {
            m_ai.ClearGroupCombatOrder(PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO);
        }
    }

    if (m_ai.GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_OOC)
    {
        if (m_WaitUntil > m_ai.CurrentTime() && m_ai.IsGroupReady())
            return RETURN_NO_ACTION_OK; // wait it out
        else
            m_ai.ClearGroupCombatOrder(PlayerbotAI::ORDERS_TEMP_WAIT_OOC);
    }

    switch (m_ai.GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_PVP_DUEL:
        case PlayerbotAI::SCENARIO_PVP_BG:
        case PlayerbotAI::SCENARIO_PVP_ARENA:
        case PlayerbotAI::SCENARIO_PVP_OPENWORLD:
            return DoFirstCombatManeuverPVP(pTarget);
        case PlayerbotAI::SCENARIO_PVE:
        case PlayerbotAI::SCENARIO_PVE_ELITE:
        case PlayerbotAI::SCENARIO_PVE_RAID:
        default:
            return DoFirstCombatManeuverPVE(pTarget);
            break;
    }

    return RETURN_NO_ACTION_ERROR;
}

CombatManeuverReturns PlayerbotRogueAI::DoFirstCombatManeuverPVE(Unit* pTarget)
{
    if (STEALTH > 0 && !m_bot.HasAura(STEALTH, EFFECT_INDEX_0) && m_ai.CastSpell(STEALTH, m_bot) == SPELL_CAST_OK)
    {
        m_bot.addUnitState(UNIT_STAT_CHASE); // ensure that the bot does not use MoveChase(), as this doesn't seem to work with STEALTH
        return RETURN_FINISHED_FIRST_MOVES; // DoNextCombatManeuver handles active stealth
    }
    else if (m_bot.HasAura(STEALTH, EFFECT_INDEX_0))
    {
        m_bot.GetMotionMaster()->MoveFollow(pTarget, 4.5f, m_bot.GetOrientation()); // TODO: this isn't the place for movement code, is it?
        return RETURN_FINISHED_FIRST_MOVES; // DoNextCombatManeuver handles active stealth
    }

    // Not in stealth, can't cast stealth; Off to DoNextCombatManeuver
    return RETURN_NO_ACTION_OK;
}

// TODO: blatant copy of PVE for now, please PVP-port it
CombatManeuverReturns PlayerbotRogueAI::DoFirstCombatManeuverPVP(Unit* pTarget)
{
    if (STEALTH > 0 && !m_bot.HasAura(STEALTH, EFFECT_INDEX_0) && m_ai.CastSpell(STEALTH, m_bot) == SPELL_CAST_OK)
    {
        m_bot.addUnitState(UNIT_STAT_CHASE); // ensure that the bot does not use MoveChase(), as this doesn't seem to work with STEALTH
        return RETURN_FINISHED_FIRST_MOVES; // DoNextCombatManeuver handles active stealth
    }
    else if (m_bot.HasAura(STEALTH, EFFECT_INDEX_0))
    {
        m_bot.GetMotionMaster()->MoveFollow(pTarget, 4.5f, m_bot.GetOrientation()); // TODO: this isn't the place for movement code, is it?
        return RETURN_FINISHED_FIRST_MOVES; // DoNextCombatManeuver handles active stealth
    }

    // Not in stealth, can't cast stealth; Off to DoNextCombatManeuver
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotRogueAI::DoNextCombatManeuverPVE(Unit* pTarget)
{
    switch (m_ai.GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_PVP_DUEL:
        case PlayerbotAI::SCENARIO_PVP_BG:
        case PlayerbotAI::SCENARIO_PVP_ARENA:
        case PlayerbotAI::SCENARIO_PVP_OPENWORLD:
            return DoNextCombatManeuverPVP(pTarget);
        case PlayerbotAI::SCENARIO_PVE:
        case PlayerbotAI::SCENARIO_PVE_ELITE:
        case PlayerbotAI::SCENARIO_PVE_RAID:
        default:
            return DoNextCombatManeuverPVE(pTarget);
            break;
    }

    return RETURN_NO_ACTION_ERROR;
}

CombatManeuverReturns PlayerbotRogueAI::DoNextCombatManeuver(Unit* pTarget)
{
    if (!pTarget) return RETURN_NO_ACTION_ERROR;

    Unit* pVictim = pTarget->GetVictim();
    bool meleeReach = m_bot.CanReachWithMeleeAttack(pTarget);

    // TODO: make this work better...
    /*if (pVictim)
       {
        if( pVictim!=m_bot && !m_bot.hasUnitState(UNIT_STAT_FOLLOW) && !pTarget->isInBackInMap(m_bot,10) ) {
            m_ai.TellMaster( "getting behind target" );
            m_bot.GetMotionMaster()->Clear( true );
            m_bot.GetMotionMaster()->MoveFollow( pTarget, 1, 2*M_PI );
        }
        else if( pVictim==m_bot && m_bot.hasUnitState(UNIT_STAT_FOLLOW) )
        {
            m_ai.TellMaster( "chasing attacking target" );
            m_bot.GetMotionMaster()->Clear( true );
            m_bot.GetMotionMaster()->MoveChase( pTarget );
        }
       }*/

    // Rogue like behaviour ^^
    /*if (VANISH > 0 && GetMaster()->IsDead()) { //Causes the server to crash :( removed for now.
        m_bot.AttackStop();
        m_bot.RemoveAllAttackers();
        m_ai.CastSpell(VANISH);
        //m_bot.RemoveAllSpellCooldown();
        m_ai.TellMaster("AttackStop, CombatStop, Vanish");
    }*/

    // decide what to do:
    if (pVictim == &m_bot && CLOAK_OF_SHADOWS > 0 && m_bot.HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !m_bot.HasAura(CLOAK_OF_SHADOWS, EFFECT_INDEX_0) && m_ai.CastSpell(CLOAK_OF_SHADOWS) == SPELL_CAST_OK)
    {
        m_ai.TellMaster("CoS!");
        return RETURN_CONTINUE;
    }
    else if (m_bot.HasAura(STEALTH, EFFECT_INDEX_0))
        SpellSequence = RogueStealth;
    else if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = RogueSpellPreventing;
    else if (pVictim == &m_bot && m_ai.GetHealthPercent() < 40)
        SpellSequence = RogueThreat;
    else
        SpellSequence = RogueCombat;

    // we fight in melee, target is not in range, skip the next part!
    if (!meleeReach)
        return RETURN_CONTINUE;

    std::ostringstream out;
    switch (SpellSequence)
    {
        case RogueStealth:
            if (PICK_POCKET > 0 && (pTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0 && m_ai.PickPocket(pTarget))
                return RETURN_CONTINUE;
            if (PREMEDITATION > 0 && m_ai.CastSpell(PREMEDITATION, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (AMBUSH > 0 && m_ai.CastSpell(AMBUSH, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (CHEAP_SHOT > 0 && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && m_ai.CastSpell(CHEAP_SHOT, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (GARROTE > 0 && m_ai.CastSpell(GARROTE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;

            // No appropriate action found, remove stealth
            m_bot.RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            return RETURN_CONTINUE;

        case RogueThreat:
            if (GOUGE > 0 && !pTarget->HasAura(GOUGE, EFFECT_INDEX_0) && m_ai.CastSpell(GOUGE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (EVASION > 0 && m_ai.GetHealthPercent() <= 35 && !m_bot.HasAura(EVASION, EFFECT_INDEX_0) && m_ai.CastSpell(EVASION) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (BLIND > 0 && m_ai.GetHealthPercent() <= 30 && !pTarget->HasAura(BLIND, EFFECT_INDEX_0) && m_ai.CastSpell(BLIND, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (FEINT > 0 && m_ai.GetHealthPercent() <= 25 && m_ai.CastSpell(FEINT) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (VANISH > 0 && m_ai.GetHealthPercent() <= 20 && !m_bot.HasAura(FEINT, EFFECT_INDEX_0) && m_ai.CastSpell(VANISH) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (PREPARATION > 0 && m_ai.CastSpell(PREPARATION) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_NIGHTELF && m_ai.GetHealthPercent() <= 15 && !m_bot.HasAura(SHADOWMELD, EFFECT_INDEX_0) && m_ai.CastSpell(SHADOWMELD, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            break;

        case RogueSpellPreventing:
            if ((KIDNEY_SHOT > 0 && m_bot.GetComboPoints() >= 2 && m_ai.CastSpell(KIDNEY_SHOT, *pTarget) == SPELL_CAST_OK) ||
                (KICK > 0 && m_ai.CastSpell(KICK, *pTarget) == SPELL_CAST_OK))
                return RETURN_CONTINUE;
        // break; // No action? Go combat!

        case RogueCombat:
        default:
            if (m_bot.GetComboPoints() >= 5)
            {
            	Creature* pCreature = (Creature*) pTarget;
                // wait for energy
                if (m_ai.GetEnergyAmount() < 25 && (KIDNEY_SHOT || SLICE_DICE || EXPOSE_ARMOR))
                    return RETURN_NO_ACTION_OK;

                switch (pTarget->getClass())
                {
                    case CLASS_SHAMAN:
                        if (KIDNEY_SHOT > 0 && m_ai.CastSpell(KIDNEY_SHOT, *pTarget) == SPELL_CAST_OK) // 25 energy (checked above)
                            return RETURN_CONTINUE;
                        break;

                    case CLASS_WARLOCK:
                    case CLASS_HUNTER:
                        if (SLICE_DICE > 0 && m_ai.CastSpell(SLICE_DICE, *pTarget) == SPELL_CAST_OK) // 25 energy (checked above)
                            return RETURN_CONTINUE;
                        break;

                    case CLASS_WARRIOR:
                    case CLASS_PALADIN:
                    case CLASS_DEATH_KNIGHT:
						// If target is a warrior or paladin type (high armor): expose its armor unless already tanked by a warrior (Sunder Armor > Expose Armor)
						if (m_ai.IsElite(pTarget) && pCreature && pCreature->GetCreatureInfo()->UnitClass != 8)
						{
							if  (!m_ai.GetGroupTank() || (m_ai.GetGroupTank() && m_ai.GetGroupTank()->GetVictim() != pTarget))
							{
								if (EXPOSE_ARMOR > 0 && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && m_ai.CastSpell(EXPOSE_ARMOR, *pTarget) == SPELL_CAST_OK) // 25 energy (checked above)
									return RETURN_CONTINUE;
							}
						}
                        break;


                    case CLASS_MAGE:
                    case CLASS_PRIEST:
                        if (RUPTURE > 0 && m_ai.CastSpell(RUPTURE, *pTarget) == SPELL_CAST_OK) // 25 energy (checked above)
                            return RETURN_CONTINUE;
                        break;

                    case CLASS_ROGUE:
                    case CLASS_DRUID:
                    default:
                        break; // fall through to below
                }

                // default combo action for rogue/druid or if other combo action is unavailable/failed
                // wait for energy
                if (m_ai.GetEnergyAmount() < 35 && EVISCERATE)
                    return RETURN_NO_ACTION_OK;
                if (EVISCERATE > 0 && m_ai.CastSpell(EVISCERATE, *pTarget) == SPELL_CAST_OK)
                    return RETURN_CONTINUE;

                // failed for some (non-energy related) reason, fall through to normal attacks to maximize DPS
            }

            if (SHADOW_DANCE > 0 && !m_bot.HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && m_ai.CastSpell(SHADOW_DANCE, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (CHEAP_SHOT > 0 && m_bot.HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && !pTarget->HasAura(CHEAP_SHOT, EFFECT_INDEX_0) && m_ai.CastSpell(CHEAP_SHOT, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (AMBUSH > 0 && m_bot.HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && m_ai.CastSpell(AMBUSH, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (GARROTE > 0 && m_bot.HasAura(SHADOW_DANCE, EFFECT_INDEX_0) && m_ai.CastSpell(GARROTE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (BACKSTAB > 0 && pTarget->isInBackInMap(&m_bot, 1) && m_ai.CastSpell(BACKSTAB, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (MUTILATE > 0 && m_ai.CastSpell(MUTILATE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (SINISTER_STRIKE > 0 && m_ai.CastSpell(SINISTER_STRIKE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (GHOSTLY_STRIKE > 0 && m_ai.CastSpell(GHOSTLY_STRIKE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (HEMORRHAGE > 0 && m_ai.CastSpell(HEMORRHAGE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (DISMANTLE > 0 && !pTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && m_ai.CastSpell(DISMANTLE, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (SHADOWSTEP > 0 && m_ai.CastSpell(SHADOWSTEP, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && m_ai.CastSpell(ARCANE_TORRENT, *pTarget) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_HUMAN && (m_bot.hasUnitState(UNIT_STAT_STUNNED) || m_bot.HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot.HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot.HasAuraType(SPELL_AURA_MOD_CHARM)) && m_ai.CastSpell(EVERY_MAN_FOR_HIMSELF, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_UNDEAD && (m_bot.HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot.HasAuraType(SPELL_AURA_MOD_CHARM)) && m_ai.CastSpell(WILL_OF_THE_FORSAKEN, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_DWARF && m_bot.HasAuraState(AURA_STATE_DEADLY_POISON) && m_ai.CastSpell(STONEFORM, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            if (m_bot.getRace() == RACE_GNOME && (m_bot.hasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT) || m_bot.HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED)) && m_ai.CastSpell(ESCAPE_ARTIST, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            else if (m_bot.getRace() == RACE_ORC && !m_bot.HasAura(BLOOD_FURY, EFFECT_INDEX_0) && m_ai.CastSpell(BLOOD_FURY, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            else if (m_bot.getRace() == RACE_TROLL && !m_bot.HasAura(BERSERKING, EFFECT_INDEX_0) && m_ai.CastSpell(BERSERKING, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;
            break;
    }

    return RETURN_NO_ACTION_OK;
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotRogueAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    //if (m_ai.CastSpell(SINISTER_STRIKE))
    //    return RETURN_CONTINUE;

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

void PlayerbotRogueAI::DoNonCombatActions()
{
    // remove stealth
    if (m_bot.HasAura(STEALTH))
        m_bot.RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

    // hp check
    if (EatDrinkBandage(false))
        return;

    // Search and apply poisons to weapons
    // Mainhand ...
    Item* poison, * weapon;
    weapon = m_bot.GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = m_ai.FindConsumable(INSTANT_POISON_DISPLAYID);
        if (!poison)
            poison = m_ai.FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = m_ai.FindConsumable(DEADLY_POISON_DISPLAYID);
        if (poison)
        {
            m_ai.UseItem(poison, EQUIPMENT_SLOT_MAINHAND);
            m_ai.SetIgnoreUpdateTime(5);
        }
    }
    //... and offhand
    weapon = m_bot.GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    if (weapon && weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0)
    {
        poison = m_ai.FindConsumable(DEADLY_POISON_DISPLAYID);
        if (!poison)
            poison = m_ai.FindConsumable(WOUND_POISON_DISPLAYID);
        if (!poison)
            poison = m_ai.FindConsumable(INSTANT_POISON_DISPLAYID);
        if (poison)
        {
            m_ai.UseItem(poison, EQUIPMENT_SLOT_OFFHAND);
            m_ai.SetIgnoreUpdateTime(5);
        }
    }
} // end DoNonCombatActions
