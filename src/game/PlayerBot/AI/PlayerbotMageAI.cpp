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

#include "PlayerbotMageAI.h"

class PlayerbotAI;

PlayerbotMageAI::PlayerbotMageAI(Player& master, Player& bot, PlayerbotAI& ai) : PlayerbotClassAI(master, bot, ai)
{
    ARCANE_MISSILES         = m_ai.initSpell(ARCANE_MISSILES_1);
    ARCANE_EXPLOSION        = m_ai.initSpell(ARCANE_EXPLOSION_1);
    COUNTERSPELL            = m_ai.initSpell(COUNTERSPELL_1);
    SLOW                    = m_ai.initSpell(SLOW_1);
    ARCANE_BARRAGE          = m_ai.initSpell(ARCANE_BARRAGE_1);
    ARCANE_BLAST            = m_ai.initSpell(ARCANE_BLAST_1);
    ARCANE_POWER            = m_ai.initSpell(ARCANE_POWER_1);
    DAMPEN_MAGIC            = m_ai.initSpell(DAMPEN_MAGIC_1);
    AMPLIFY_MAGIC           = m_ai.initSpell(AMPLIFY_MAGIC_1);
    MAGE_ARMOR              = m_ai.initSpell(MAGE_ARMOR_1);
    MIRROR_IMAGE            = m_ai.initSpell(MIRROR_IMAGE_1);
    ARCANE_INTELLECT        = m_ai.initSpell(ARCANE_INTELLECT_1);
    ARCANE_BRILLIANCE       = m_ai.initSpell(ARCANE_BRILLIANCE_1);
    DALARAN_INTELLECT       = m_ai.initSpell(DALARAN_INTELLECT_1);
    DALARAN_BRILLIANCE      = m_ai.initSpell(DALARAN_BRILLIANCE_1);
    MANA_SHIELD             = m_ai.initSpell(MANA_SHIELD_1);
    CONJURE_WATER           = m_ai.initSpell(CONJURE_WATER_1);
    CONJURE_FOOD            = m_ai.initSpell(CONJURE_FOOD_1);
    FIREBALL                = m_ai.initSpell(FIREBALL_1);
    FIRE_BLAST              = m_ai.initSpell(FIRE_BLAST_1);
    FLAMESTRIKE             = m_ai.initSpell(FLAMESTRIKE_1);
    SCORCH                  = m_ai.initSpell(SCORCH_1);
    PYROBLAST               = m_ai.initSpell(PYROBLAST_1);
    BLAST_WAVE              = m_ai.initSpell(BLAST_WAVE_1);
    COMBUSTION              = m_ai.initSpell(COMBUSTION_1);
    DRAGONS_BREATH          = m_ai.initSpell(DRAGONS_BREATH_1);
    LIVING_BOMB             = m_ai.initSpell(LIVING_BOMB_1);
    FROSTFIRE_BOLT          = m_ai.initSpell(FROSTFIRE_BOLT_1);
    FIRE_WARD               = m_ai.initSpell(FIRE_WARD_1);
    MOLTEN_ARMOR            = m_ai.initSpell(MOLTEN_ARMOR_1);
    ICY_VEINS               = m_ai.initSpell(ICY_VEINS_1);
    DEEP_FREEZE             = m_ai.initSpell(DEEP_FREEZE_1);
    FROSTBOLT               = m_ai.initSpell(FROSTBOLT_1);
    FROST_NOVA              = m_ai.initSpell(FROST_NOVA_1);
    BLIZZARD                = m_ai.initSpell(BLIZZARD_1);
    CONE_OF_COLD            = m_ai.initSpell(CONE_OF_COLD_1);
    ICE_BARRIER             = m_ai.initSpell(ICE_BARRIER_1);
    SUMMON_WATER_ELEMENTAL  = m_ai.initSpell(SUMMON_WATER_ELEMENTAL_1);
    FROST_WARD              = m_ai.initSpell(FROST_WARD_1);
    ICE_LANCE               = m_ai.initSpell(ICE_LANCE_1);
    FROST_ARMOR             = m_ai.initSpell(FROST_ARMOR_1);
    ICE_ARMOR               = m_ai.initSpell(ICE_ARMOR_1);
    ICE_BLOCK               = m_ai.initSpell(ICE_BLOCK_1);
    COLD_SNAP               = m_ai.initSpell(COLD_SNAP_1);
    MAGE_REMOVE_CURSE       = m_ai.initSpell(REMOVE_CURSE_MAGE_1);

    // RANGED COMBAT
    SHOOT                   = m_ai.initSpell(SHOOT_2);

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = m_ai.initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    GIFT_OF_THE_NAARU       = m_ai.initSpell(GIFT_OF_THE_NAARU_MAGE); // draenei
    ESCAPE_ARTIST           = m_ai.initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = m_ai.initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BERSERKING              = m_ai.initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = m_ai.initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotMageAI::~PlayerbotMageAI() {}

CombatManeuverReturns PlayerbotMageAI::DoFirstCombatManeuver(Unit* pTarget)
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
    }
}

CombatManeuverReturns PlayerbotMageAI::DoFirstCombatManeuverPVE(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotMageAI::DoFirstCombatManeuverPVP(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotMageAI::DoNextCombatManeuver(Unit* pTarget)
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
    }
}

CombatManeuverReturns PlayerbotMageAI::DoNextCombatManeuverPVE(Unit* pTarget)
{
    Unit* pVictim = pTarget->GetVictim();
    bool meleeReach = m_bot.CanReachWithMeleeAttack(pTarget);

    uint32 spec = m_bot.GetSpec();

    if (m_ai.GetCombatStyle() != PlayerbotAI::COMBAT_RANGED && !meleeReach)
        m_ai.SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    // if can't shoot OR have no ranged (wand) equipped
    else if (m_ai.GetCombatStyle() != PlayerbotAI::COMBAT_MELEE && (SHOOT == 0 || !m_bot.GetWeaponForAttack(RANGED_ATTACK, true, true)))
        m_ai.SetCombatStyle(PlayerbotAI::COMBAT_MELEE);

    //Used to determine if this bot is highest on threat
    Unit* newTarget = m_ai.FindAttacker((PlayerbotAI::ATTACKERINFOTYPE)(PlayerbotAI::AIT_VICTIMSELF | PlayerbotAI::AIT_HIGHESTTHREAT), &m_bot);

    // Remove curse on group members
    if (m_ai.HasDispelOrder() && DispelPlayer(GetDispelTarget(DISPEL_CURSE)) & RETURN_CONTINUE)
        return RETURN_CONTINUE;

    if (newTarget) // TODO: && party has a tank
    {
        // Insert instant threat reducing spell (if a mage has one)

        // Have threat, can't quickly lower it. 3 options remain: Stop attacking, lowlevel damage (wand), keep on keeping on.
        if (newTarget->GetHealthPercent() > 25)
        {
            // If elite, do nothing and pray tank gets aggro off you
            // TODO: Is there an IsElite function? If so, find it and insert.
            //if (newTarget->IsElite())
            //    return;

            // Not an elite. You could insert FEAR here but in any PvE situation that's 90-95% likely
            // to worsen the situation for the group. ... So please don't.
            CastSpell(SHOOT, pTarget);
            return RETURN_CONTINUE;
        }
    }

    switch (spec)
    {
        case MAGE_SPEC_FROST:
            if (ICY_VEINS > 0 && m_ai.In_Reach(&m_bot, ICY_VEINS) && !m_bot.HasAura(ICY_VEINS, EFFECT_INDEX_0) && CastSpell(ICY_VEINS, &m_bot))
                return RETURN_CONTINUE;
            if (ICE_BLOCK > 0 && m_ai.In_Reach(&m_bot, ICE_BLOCK) && pVictim == &m_bot && !m_bot.HasAura(ICE_BLOCK, EFFECT_INDEX_0) && CastSpell(ICE_BLOCK, &m_bot))
                return RETURN_CONTINUE;
            if (ICE_BARRIER > 0 && m_ai.In_Reach(&m_bot, ICE_BARRIER) && pVictim == &m_bot && !m_bot.HasAura(ICE_BARRIER, EFFECT_INDEX_0) && m_ai.GetHealthPercent() < 50 && CastSpell(ICE_BARRIER, &m_bot))
                return RETURN_CONTINUE;
            if (DEEP_FREEZE > 0 && m_ai.In_Reach(pTarget, DEEP_FREEZE) && pTarget->HasAura(AURA_STATE_FROZEN, EFFECT_INDEX_0) && !pTarget->HasAura(DEEP_FREEZE, EFFECT_INDEX_0) && CastSpell(DEEP_FREEZE, pTarget))
                return RETURN_CONTINUE;
            if (BLIZZARD > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && m_ai.In_Reach(pTarget, BLIZZARD) && m_ai.GetAttackerCount() >= 5 && CastSpell(BLIZZARD, pTarget))
            {
                m_ai.SetIgnoreUpdateTime(8);
                return RETURN_CONTINUE;
            }
            if (CONE_OF_COLD > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && meleeReach && !pTarget->HasAura(CONE_OF_COLD, EFFECT_INDEX_0) && CastSpell(CONE_OF_COLD, pTarget))
                return RETURN_CONTINUE;
            if (FROSTBOLT > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && m_ai.In_Reach(pTarget, FROSTBOLT) && !pTarget->HasAura(FROSTBOLT, EFFECT_INDEX_0) && CastSpell(FROSTBOLT, pTarget))
                return RETURN_CONTINUE;
            if (FROST_WARD > 0 && m_ai.In_Reach(&m_bot, FROST_WARD) && !m_bot.HasAura(FROST_WARD, EFFECT_INDEX_0) && CastSpell(FROST_WARD, &m_bot))
                return RETURN_CONTINUE;
            if (FROST_NOVA > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && meleeReach && !pTarget->HasAura(FROST_NOVA, EFFECT_INDEX_0) && CastSpell(FROST_NOVA, pTarget))
                return RETURN_CONTINUE;
            if (ICE_LANCE > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && m_ai.In_Reach(pTarget, ICE_LANCE) && CastSpell(ICE_LANCE, pTarget))
                return RETURN_CONTINUE;
            if (SUMMON_WATER_ELEMENTAL > 0 && CastSpell(SUMMON_WATER_ELEMENTAL))
                return RETURN_CONTINUE;
            if (COLD_SNAP > 0 && m_ai.In_Reach(&m_bot, COLD_SNAP) && CastSpell(COLD_SNAP, &m_bot))
                return RETURN_CONTINUE;

            if (FROSTBOLT > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && m_ai.In_Reach(pTarget, FROSTBOLT))
                return CastSpell(FROSTBOLT, pTarget);
            break;

        case MAGE_SPEC_FIRE:
            if (FIRE_WARD > 0 && m_ai.In_Reach(&m_bot, FIRE_WARD) && !m_bot.HasAura(FIRE_WARD, EFFECT_INDEX_0) && CastSpell(FIRE_WARD, &m_bot))
                return RETURN_CONTINUE;
            if (COMBUSTION > 0 && m_ai.In_Reach(&m_bot, COMBUSTION) && !m_bot.HasAura(COMBUSTION, EFFECT_INDEX_0) && CastSpell(COMBUSTION, &m_bot))
                return RETURN_CONTINUE;
            if (FIREBALL > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FIREBALL) && CastSpell(FIREBALL, pTarget))
                return RETURN_CONTINUE;
            if (FIRE_BLAST > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FIRE_BLAST) && CastSpell(FIRE_BLAST, pTarget))
                return RETURN_CONTINUE;
            if (FLAMESTRIKE > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FLAMESTRIKE) && CastSpell(FLAMESTRIKE, pTarget))
                return RETURN_CONTINUE;
            if (SCORCH > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, SCORCH) && CastSpell(SCORCH, pTarget))
                return RETURN_CONTINUE;
            if (PYROBLAST > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, PYROBLAST) && !pTarget->HasAura(PYROBLAST, EFFECT_INDEX_0) && CastSpell(PYROBLAST, pTarget))
                return RETURN_CONTINUE;
            if (BLAST_WAVE > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.GetAttackerCount() >= 3 && meleeReach && CastSpell(BLAST_WAVE, pTarget))
                return RETURN_CONTINUE;
            if (DRAGONS_BREATH > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && meleeReach && CastSpell(DRAGONS_BREATH, pTarget))
                return RETURN_CONTINUE;
            if (LIVING_BOMB > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, LIVING_BOMB) && !pTarget->HasAura(LIVING_BOMB, EFFECT_INDEX_0) && CastSpell(LIVING_BOMB, pTarget))
                return RETURN_CONTINUE;
            if (FROSTFIRE_BOLT > 0 && m_ai.In_Reach(pTarget, FROSTFIRE_BOLT) && !pTarget->HasAura(FROSTFIRE_BOLT, EFFECT_INDEX_0) && CastSpell(FROSTFIRE_BOLT, pTarget))
                return RETURN_CONTINUE;

            if (FIREBALL > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FIREBALL))
                return CastSpell(FIREBALL, pTarget);
            break;

        case MAGE_SPEC_ARCANE:
            if (ARCANE_POWER > 0 && m_ai.In_Reach(pTarget, ARCANE_POWER) && CastSpell(ARCANE_POWER, pTarget))
                return RETURN_CONTINUE;
            if (ARCANE_MISSILES > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && m_ai.In_Reach(pTarget, ARCANE_MISSILES) && CastSpell(ARCANE_MISSILES, pTarget))
            {
                m_ai.SetIgnoreUpdateTime(3);
                return RETURN_CONTINUE;
            }
            if (ARCANE_EXPLOSION > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && m_ai.GetAttackerCount() >= 3 && meleeReach && CastSpell(ARCANE_EXPLOSION, pTarget))
                return RETURN_CONTINUE;
            if (COUNTERSPELL > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && pTarget->IsNonMeleeSpellCasted(true) && CastSpell(COUNTERSPELL, pTarget))
                return RETURN_CONTINUE;
            if (SLOW > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && m_ai.In_Reach(pTarget, SLOW) && !pTarget->HasAura(SLOW, EFFECT_INDEX_0) && CastSpell(SLOW, pTarget))
                return RETURN_CONTINUE;
            if (ARCANE_BARRAGE > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && m_ai.In_Reach(pTarget, ARCANE_BARRAGE) && CastSpell(ARCANE_BARRAGE, pTarget))
                return RETURN_CONTINUE;
            if (ARCANE_BLAST > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_ARCANE) && m_ai.In_Reach(pTarget, ARCANE_BLAST) && CastSpell(ARCANE_BLAST, pTarget))
                return RETURN_CONTINUE;
            if (MIRROR_IMAGE > 0 && m_ai.In_Reach(pTarget, MIRROR_IMAGE) && CastSpell(MIRROR_IMAGE))
                return RETURN_CONTINUE;
            if (MANA_SHIELD > 0 && m_ai.GetHealthPercent() < 70 && pVictim == &m_bot && !m_bot.HasAura(MANA_SHIELD, EFFECT_INDEX_0) && CastSpell(MANA_SHIELD, &m_bot))
                return RETURN_CONTINUE;

            if (FIREBALL > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FIREBALL))
                return CastSpell(FIREBALL, pTarget);
            break;
    }

    // No spec due to low level OR no spell found yet
    if (FROSTBOLT > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FROST) && m_ai.In_Reach(pTarget, FROSTBOLT) && !pTarget->HasAura(FROSTBOLT, EFFECT_INDEX_0))
        return CastSpell(FROSTBOLT, pTarget);
    if (FIREBALL > 0 && !PlayerbotAI::IsImmuneToSchool(pTarget, SPELL_SCHOOL_MASK_FIRE) && m_ai.In_Reach(pTarget, FIREBALL)) // Very low levels
        return CastSpell(FIREBALL, pTarget);

    return RETURN_NO_ACTION_ERROR; // What? Not even Fireball is available?
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotMageAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    if (FIREBALL && m_ai.In_Reach(pTarget, FIREBALL) && m_ai.CastSpell(FIREBALL) == SPELL_CAST_OK)
        return RETURN_CONTINUE;

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

// Function to keep track of active frost cooldowns to clear with Cold Snap
uint8 PlayerbotMageAI::CheckFrostCooldowns()
{
    uint8 uiFrostActiveCooldown = 0;
    if (FROST_NOVA && !m_bot.IsSpellReady(FROST_NOVA))
        uiFrostActiveCooldown++;
    if (ICE_BARRIER && !m_bot.IsSpellReady(ICE_BARRIER))
        uiFrostActiveCooldown++;
    if (CONE_OF_COLD && !m_bot.IsSpellReady(CONE_OF_COLD))
        uiFrostActiveCooldown++;
    if (ICE_BLOCK && !m_bot.IsSpellReady(ICE_BLOCK))
        uiFrostActiveCooldown++;
    if (FROST_WARD && !m_bot.IsSpellReady(FROST_WARD))
        uiFrostActiveCooldown++;

    return uiFrostActiveCooldown;
}

CombatManeuverReturns PlayerbotMageAI::DispelPlayer(Player* cursedTarget)
{
    CombatManeuverReturns r = PlayerbotClassAI::DispelPlayer(cursedTarget);
    if (r != RETURN_NO_ACTION_OK)
        return r;

    if (MAGE_REMOVE_CURSE > 0 && CastSpell(MAGE_REMOVE_CURSE, cursedTarget))
        return RETURN_CONTINUE;

    return RETURN_NO_ACTION_OK;
}

void PlayerbotMageAI::DoNonCombatActions()
{
    // Remove curse on group members
    if (m_ai.HasDispelOrder() && DispelPlayer(GetDispelTarget(DISPEL_CURSE)) & RETURN_CONTINUE)
        return;

    // Buff armor
    if (MOLTEN_ARMOR)
    {
        if (m_ai.SelfBuff(MOLTEN_ARMOR) == SPELL_CAST_OK)
            return;
    }
    else if (MAGE_ARMOR)
    {
        if (m_ai.SelfBuff(MAGE_ARMOR) == SPELL_CAST_OK)
            return;
    }
    else if (ICE_ARMOR)
    {
        if (m_ai.SelfBuff(ICE_ARMOR) == SPELL_CAST_OK)
            return;
    }
    else if (FROST_ARMOR)
        if (m_ai.SelfBuff(FROST_ARMOR) == SPELL_CAST_OK)
            return;

    // buff group
    // the check for group targets is performed by NeedGroupBuff (if group is found for bots by the function)
    if (NeedGroupBuff(ARCANE_BRILLIANCE, ARCANE_INTELLECT) && m_ai.HasSpellReagents(ARCANE_BRILLIANCE))
    {
        if (Buff(&PlayerbotMageAI::BuffHelper, ARCANE_BRILLIANCE) & RETURN_CONTINUE)
            return;
    }
    else if (Buff(&PlayerbotMageAI::BuffHelper, ARCANE_INTELLECT, JOB_MANAONLY) & RETURN_CONTINUE)
        return;

    // if there is space on bag try to conjure some consumables
    if (m_ai.CanStore())
    {
        // TODO: The beauty of a mage is not only its ability to supply itself with water, but to share its water
        // So, conjure at *least* 1.25 stacks, ready to trade a stack and still have some left for self
        if (m_ai.FindDrink() == nullptr && CONJURE_WATER && m_ai.CastSpell(CONJURE_WATER, m_bot) == SPELL_CAST_OK)
        {
            m_ai.TellMaster("I'm conjuring some water.");
            m_ai.SetIgnoreUpdateTime(3);
            return;
        }
        if (m_ai.FindFood() == nullptr && CONJURE_FOOD && m_ai.CastSpell(CONJURE_FOOD, m_bot) == SPELL_CAST_OK)
        {
            m_ai.TellMaster("I'm conjuring some food.");
            m_ai.SetIgnoreUpdateTime(3);
            return;
        }
    }

    if (EatDrinkBandage())
        return;
} // end DoNonCombatActions

// TODO: this and priest's BuffHelper are identical and thus could probably go in PlayerbotClassAI.cpp somewhere
bool PlayerbotMageAI::BuffHelper(PlayerbotAI* ai, uint32 spellId, Unit* target)
{
    //DEBUG_LOG("..Mage_BuffHelper to the rescue!");
    if (!ai)          return false;
    if (spellId == 0) return false;
    if (!target)      return false;
    //DEBUG_LOG("..Sanity checks passed");

    return ai->Buff(spellId, target) == SPELL_CAST_OK;

    //DEBUG_LOG("..Not buffing anyone!");
}
