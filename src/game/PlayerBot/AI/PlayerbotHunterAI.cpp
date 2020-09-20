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

#include "PlayerbotHunterAI.h"
#include "../Base/PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotHunterAI::PlayerbotHunterAI(Player& master, Player& bot, PlayerbotAI& ai) : PlayerbotClassAI(master, bot, ai)
{
    // PET CTRL
    PET_SUMMON                    = m_ai.initSpell(CALL_PET_1);
    PET_DISMISS                   = m_ai.initSpell(DISMISS_PET_1);
    PET_REVIVE                    = m_ai.initSpell(REVIVE_PET_1);
    PET_MEND                      = m_ai.initSpell(MEND_PET_1);
    PET_FEED                      = 1539;

    INTIMIDATION                  = m_ai.initSpell(INTIMIDATION_1); // (generic)

    // PET SKILLS must be initialized by pets
    SONIC_BLAST                   = 0; // bat
    DEMORALIZING_SCREECH          = 0;
    BAD_ATTITUDE                  = 0; // crocolisk
    NETHER_SHOCK                  = 0;

    // RANGED COMBAT
    AUTO_SHOT                     = m_ai.initSpell(AUTO_SHOT_1);
    HUNTERS_MARK                  = m_ai.initSpell(HUNTERS_MARK_1);
    ARCANE_SHOT                   = m_ai.initSpell(ARCANE_SHOT_1);
    CONCUSSIVE_SHOT               = m_ai.initSpell(CONCUSSIVE_SHOT_1);
    DISTRACTING_SHOT              = m_ai.initSpell(DISTRACTING_SHOT_1);
    MULTI_SHOT                    = m_ai.initSpell(MULTISHOT_1);
    EXPLOSIVE_SHOT                = m_ai.initSpell(EXPLOSIVE_SHOT_1);
    SERPENT_STING                 = m_ai.initSpell(SERPENT_STING_1);
    SCORPID_STING                 = m_ai.initSpell(SCORPID_STING_1);
    WYVERN_STING                  = m_ai.initSpell(WYVERN_STING_1);
    VIPER_STING                   = m_ai.initSpell(VIPER_STING_1);
    AIMED_SHOT                    = m_ai.initSpell(AIMED_SHOT_1);
    STEADY_SHOT                   = m_ai.initSpell(STEADY_SHOT_1);
    CHIMERA_SHOT                  = m_ai.initSpell(CHIMERA_SHOT_1);
    VOLLEY                        = m_ai.initSpell(VOLLEY_1);
    BLACK_ARROW                   = m_ai.initSpell(BLACK_ARROW_1);
    KILL_SHOT                     = m_ai.initSpell(KILL_SHOT_1);

    // MELEE
    RAPTOR_STRIKE                 = m_ai.initSpell(RAPTOR_STRIKE_1);
    WING_CLIP                     = m_ai.initSpell(WING_CLIP_1);
    MONGOOSE_BITE                 = m_ai.initSpell(MONGOOSE_BITE_1);
    DISENGAGE                     = m_ai.initSpell(DISENGAGE_1);
    MISDIRECTION                  = m_ai.initSpell(MISDIRECTION_1);
    DETERRENCE                    = m_ai.initSpell(DETERRENCE_1);

    // TRAPS
    BEAR_TRAP                     = 0; // non-player spell
    FREEZING_TRAP                 = m_ai.initSpell(FREEZING_TRAP_1);
    IMMOLATION_TRAP               = m_ai.initSpell(IMMOLATION_TRAP_1);
    FROST_TRAP                    = m_ai.initSpell(FROST_TRAP_1);
    EXPLOSIVE_TRAP                = m_ai.initSpell(EXPLOSIVE_TRAP_1);
    ARCANE_TRAP                   = 0; // non-player spell
    SNAKE_TRAP                    = m_ai.initSpell(SNAKE_TRAP_1);

    // BUFFS
    ASPECT_OF_THE_HAWK            = m_ai.initSpell(ASPECT_OF_THE_HAWK_1);
    ASPECT_OF_THE_MONKEY          = m_ai.initSpell(ASPECT_OF_THE_MONKEY_1);
    RAPID_FIRE                    = m_ai.initSpell(RAPID_FIRE_1);
    TRUESHOT_AURA                 = m_ai.initSpell(TRUESHOT_AURA_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = m_ai.initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = m_ai.initSpell(GIFT_OF_THE_NAARU_HUNTER); // draenei
    STONEFORM                     = m_ai.initSpell(STONEFORM_ALL); // dwarf
    SHADOWMELD                    = m_ai.initSpell(SHADOWMELD_ALL);
    BLOOD_FURY                    = m_ai.initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP                     = m_ai.initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING                    = m_ai.initSpell(BERSERKING_ALL); // troll

    m_petSummonFailed = false;
    m_rangedCombat = true;
}

PlayerbotHunterAI::~PlayerbotHunterAI() {}

CombatManeuverReturns PlayerbotHunterAI::DoFirstCombatManeuver(Unit* pTarget)
{
    m_has_ammo = m_bot.HasItemCount(m_bot.GetUInt32Value(PLAYER_AMMO_ID), 1);
    //DEBUG_LOG("current ammo (%u)",m_bot.GetUInt32Value(PLAYER_AMMO_ID));
    m_bot.setAttackTimer(RANGED_ATTACK, 0);
    if (!m_has_ammo)
    {
        m_ai.FindAmmo();
        //DEBUG_LOG("new ammo (%u)",m_bot.GetUInt32Value(PLAYER_AMMO_ID));
        m_has_ammo = m_bot.HasItemCount(m_bot.GetUInt32Value(PLAYER_AMMO_ID), 1);
    }
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

CombatManeuverReturns PlayerbotHunterAI::DoFirstCombatManeuverPVE(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotHunterAI::DoFirstCombatManeuverPVP(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotHunterAI::DoNextCombatManeuver(Unit* pTarget)
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

CombatManeuverReturns PlayerbotHunterAI::DoNextCombatManeuverPVE(Unit* pTarget)
{
    if (!pTarget) return RETURN_NO_ACTION_ERROR;

    Unit* pVictim = pTarget->GetVictim();

    // check for pet and heal if neccessary
    Pet* pet = m_bot.GetPet();
    // TODO: clarify/simplify: !pet->GetDeathState() != ALIVE
    if (pet && PET_MEND > 0 && pet->IsAlive() && pet->GetHealthPercent() < 50 && pVictim != &m_bot && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && m_ai.CastSpell(PET_MEND, m_bot) == SPELL_CAST_OK)
    {
        m_ai.TellMaster("healing pet.");
        return RETURN_CONTINUE;
    }
    else if (pet && INTIMIDATION > 0 && pVictim == pet && !pet->HasAura(INTIMIDATION, EFFECT_INDEX_0) && m_ai.CastSpell(INTIMIDATION, m_bot) == SPELL_CAST_OK)
        return RETURN_CONTINUE;

    // racial traits
    if (m_bot.getRace() == RACE_ORC && !m_bot.HasAura(BLOOD_FURY, EFFECT_INDEX_0))
        m_ai.CastSpell(BLOOD_FURY, m_bot);
    else if (m_bot.getRace() == RACE_TROLL && !m_bot.HasAura(BERSERKING, EFFECT_INDEX_0))
        m_ai.CastSpell(BERSERKING, m_bot);

    // check if ranged combat is possible
    bool meleeReach = m_bot.CanReachWithMeleeAttack(pTarget);

    if (meleeReach || !m_has_ammo)
    {
        // switch to melee combat (target in melee range, out of ammo)
        m_rangedCombat = false;
        m_ai.SetCombatStyle(PlayerbotAI::COMBAT_MELEE);
        if (!m_bot.GetUInt32Value(PLAYER_AMMO_ID))
            m_ai.TellMaster("Out of ammo!");

        // become monkey (increases dodge chance)...
        if (ASPECT_OF_THE_MONKEY > 0 && !m_bot.HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0))
            m_ai.CastSpell(ASPECT_OF_THE_MONKEY, m_bot);

    }
    else if (!meleeReach)
    {
        // switch to ranged combat
        m_rangedCombat = true;
        m_ai.SetCombatStyle(PlayerbotAI::COMBAT_RANGED);

        // increase ranged attack power...
        if (ASPECT_OF_THE_HAWK > 0 && !m_bot.HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
            m_ai.CastSpell(ASPECT_OF_THE_HAWK, m_bot);

        // m_ai.TellMaster("target dist %f",m_bot.GetCombatDistance(pTarget,true));
        if (AUTO_SHOT > 0)
        {
            if (m_bot.isAttackReady(RANGED_ATTACK))
                m_bot.CastSpell(pTarget, AUTO_SHOT, TRIGGERED_OLD_TRIGGERED);

            m_bot.setAttackTimer(RANGED_ATTACK, 500);

            const SpellEntry* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(AUTO_SHOT);
            if (!spellInfo)
                return RETURN_CONTINUE;

            if (!(m_ai.CheckBotCast(spellInfo) == SPELL_CAST_OK))
                m_bot.InterruptNonMeleeSpells(true, AUTO_SHOT);
        }
    }

    // damage spells
    if (m_ai.GetCombatStyle() == PlayerbotAI::COMBAT_RANGED)
    {
        if (HUNTERS_MARK > 0 && m_ai.In_Reach(pTarget, HUNTERS_MARK) && !pTarget->HasAura(HUNTERS_MARK, EFFECT_INDEX_0) && m_ai.CastSpell(HUNTERS_MARK, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (RAPID_FIRE > 0 && m_ai.In_Reach(pTarget, RAPID_FIRE) && !m_bot.HasAura(RAPID_FIRE, EFFECT_INDEX_0) && m_ai.CastSpell(RAPID_FIRE, m_bot) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (MULTI_SHOT > 0 && m_ai.In_Reach(pTarget, MULTI_SHOT) && m_ai.GetAttackerCount() >= 3 && m_ai.CastSpell(MULTI_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (ARCANE_SHOT > 0 && m_ai.In_Reach(pTarget, ARCANE_SHOT) && m_ai.CastSpell(ARCANE_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (CONCUSSIVE_SHOT > 0 && m_ai.In_Reach(pTarget, CONCUSSIVE_SHOT) && !pTarget->HasAura(CONCUSSIVE_SHOT, EFFECT_INDEX_0) && m_ai.CastSpell(CONCUSSIVE_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (EXPLOSIVE_SHOT > 0 && m_ai.In_Reach(pTarget, EXPLOSIVE_SHOT) && !pTarget->HasAura(EXPLOSIVE_SHOT, EFFECT_INDEX_0) && m_ai.CastSpell(EXPLOSIVE_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (VIPER_STING > 0 && m_ai.In_Reach(pTarget, VIPER_STING) && pTarget->GetPower(POWER_MANA) > 0 && m_ai.GetManaPercent() < 70 && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai.CastSpell(VIPER_STING, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (SERPENT_STING > 0 && m_ai.In_Reach(pTarget, SERPENT_STING) && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) &&  !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai.CastSpell(SERPENT_STING, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (SCORPID_STING > 0 && m_ai.In_Reach(pTarget, SCORPID_STING) && !pTarget->HasAura(WYVERN_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SCORPID_STING, EFFECT_INDEX_0) && !pTarget->HasAura(SERPENT_STING, EFFECT_INDEX_0) && !pTarget->HasAura(VIPER_STING, EFFECT_INDEX_0) && m_ai.CastSpell(SCORPID_STING, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (CHIMERA_SHOT > 0 && m_ai.In_Reach(pTarget, CHIMERA_SHOT) && m_ai.CastSpell(CHIMERA_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (VOLLEY > 0 && m_ai.In_Reach(pTarget, VOLLEY) && m_ai.GetAttackerCount() >= 3 && m_ai.CastSpell(VOLLEY, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (BLACK_ARROW > 0 && m_ai.In_Reach(pTarget, BLACK_ARROW) && !pTarget->HasAura(BLACK_ARROW, EFFECT_INDEX_0) && m_ai.CastSpell(BLACK_ARROW, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (AIMED_SHOT > 0 && m_ai.In_Reach(pTarget, AIMED_SHOT) && m_ai.CastSpell(AIMED_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (STEADY_SHOT > 0 && m_ai.In_Reach(pTarget, STEADY_SHOT) && m_ai.CastSpell(STEADY_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (KILL_SHOT > 0 && m_ai.In_Reach(pTarget, KILL_SHOT) && pTarget->GetHealthPercent() < 20 && m_ai.CastSpell(KILL_SHOT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else
            return RETURN_NO_ACTION_OK;
    }
    else
    {
        if (RAPTOR_STRIKE > 0 && m_ai.In_Reach(pTarget, RAPTOR_STRIKE) && m_ai.CastSpell(RAPTOR_STRIKE, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (EXPLOSIVE_TRAP > 0 && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(EXPLOSIVE_TRAP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (WING_CLIP > 0 && m_ai.In_Reach(pTarget, WING_CLIP) && !pTarget->HasAura(WING_CLIP, EFFECT_INDEX_0) && m_ai.CastSpell(WING_CLIP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (IMMOLATION_TRAP > 0 && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(IMMOLATION_TRAP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (MONGOOSE_BITE > 0 && m_ai.Impulse() && m_ai.CastSpell(MONGOOSE_BITE, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (FROST_TRAP > 0 && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(FROST_TRAP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (ARCANE_TRAP > 0 && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(ARCANE_TRAP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (DETERRENCE > 0 && pVictim == &m_bot && m_bot.GetHealthPercent() < 50 && !m_bot.HasAura(DETERRENCE, EFFECT_INDEX_0) && m_ai.CastSpell(DETERRENCE, m_bot) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (m_bot.getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && m_ai.CastSpell(WAR_STOMP, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (m_bot.getRace() == RACE_BLOODELF && !pTarget->HasAura(ARCANE_TORRENT, EFFECT_INDEX_0) && m_ai.CastSpell(ARCANE_TORRENT, *pTarget) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (m_bot.getRace() == RACE_DWARF && m_bot.HasAuraState(AURA_STATE_DEADLY_POISON) && m_ai.CastSpell(STONEFORM, m_bot) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (m_bot.getRace() == RACE_NIGHTELF && pVictim == &m_bot && m_ai.GetHealthPercent() < 25 && !m_bot.HasAura(SHADOWMELD, EFFECT_INDEX_0) && m_ai.CastSpell(SHADOWMELD, m_bot) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (m_bot.getRace() == RACE_DRAENEI && m_ai.GetHealthPercent() < 25 && !m_bot.HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai.CastSpell(GIFT_OF_THE_NAARU, m_bot) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        else if (pet && pet->IsAlive() && MISDIRECTION > 0 && m_ai.In_Reach(pet, MISDIRECTION) && pVictim == &m_bot && !m_bot.HasAura(MISDIRECTION, EFFECT_INDEX_0) && m_ai.CastSpell(MISDIRECTION, *pet) == SPELL_CAST_OK)
            return RETURN_CONTINUE;
        /*else if(FREEZING_TRAP > 0 && !pTarget->HasAura(FREEZING_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(FREEZING_TRAP,*pTarget) == SPELL_CAST_OK)
            out << " > Freezing Trap"; // this can trap your bots too
           else if(BEAR_TRAP > 0 && !pTarget->HasAura(BEAR_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(ARCANE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(EXPLOSIVE_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(IMMOLATION_TRAP, EFFECT_INDEX_0) && !pTarget->HasAura(FROST_TRAP, EFFECT_INDEX_0) && m_ai.CastSpell(BEAR_TRAP,*pTarget) == SPELL_CAST_OK)
            out << " > Bear Trap"; // this was just too annoying :)
           else if(DISENGAGE > 0 && pVictim && m_ai.CastSpell(DISENGAGE,*pTarget) == SPELL_CAST_OK)
            out << " > Disengage!"; // attempt to return to ranged combat*/
    }

    return RETURN_NO_ACTION_OK;
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotHunterAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    if (m_ai.CastSpell(RAPTOR_STRIKE) == SPELL_CAST_OK)
        return RETURN_CONTINUE;

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

bool PlayerbotHunterAI::IsTargetEnraged(Unit* pTarget)
{
    if (!pTarget) return false;

    Unit::SpellAuraHolderMap const& auras = pTarget->GetSpellAuraHolderMap();
    for (auto itr = auras.begin(); itr != auras.end(); ++itr)
    {
        SpellAuraHolder* holder = itr->second;
        // Return true is target unit has aura with DISPEL_ENRAGE dispel type
        if ((1 << holder->GetSpellProto()->Dispel) & GetDispellMask(DISPEL_ENRAGE))
            return true;
    }

    return false;
}

void PlayerbotHunterAI::DoNonCombatActions()
{
    if (!m_rangedCombat || m_ai.GetCombatStyle() == PlayerbotAI::COMBAT_MELEE)
    {
        m_rangedCombat = true;
        m_ai.SetCombatStyle(PlayerbotAI::COMBAT_RANGED);
    }

    // buff group
    if (TRUESHOT_AURA > 0 && !m_bot.HasAura(TRUESHOT_AURA, EFFECT_INDEX_0))
        m_ai.CastSpell(TRUESHOT_AURA, m_bot);

    // buff myself
    if (ASPECT_OF_THE_HAWK > 0 && !m_bot.HasAura(ASPECT_OF_THE_HAWK, EFFECT_INDEX_0))
        m_ai.CastSpell(ASPECT_OF_THE_HAWK, m_bot);

    // hp/mana check
    if (EatDrinkBandage())
        return;

    if (m_bot.getRace() == RACE_DRAENEI && !m_bot.HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && m_ai.GetHealthPercent() < 70)
    {
        m_ai.TellMaster("I'm casting gift of the naaru.");
        m_ai.CastSpell(GIFT_OF_THE_NAARU, m_bot);
        return;
    }

    // check for pet
    if (PET_SUMMON > 0 && !m_petSummonFailed && m_bot.GetPetGuid())
    {
        // we can summon pet, and no critical summon errors before
        Pet* pet = m_bot.GetPet();
        if (!pet)    // Hunter has pet but it is not found: dismissed or dead and corpse was removed
        {
            // summon pet
            SpellCastResult res = m_ai.CastSpell(PET_SUMMON, m_bot);
            switch (res)
            {
                case SPELL_CAST_OK:
                    m_ai.TellMaster("summoning pet.");
                    break;
                case SPELL_FAILED_DONT_REPORT:
                    if (PET_REVIVE > 0)
                    {
                        switch (m_ai.CastSpell(PET_REVIVE, m_bot))
                        {
                            case SPELL_CAST_OK:
                                m_ai.TellMaster("Pet is dead, reviving it.");
                                break;
                            case SPELL_FAILED_NO_POWER: // Not enough mana, just wait for it
                                break;
                            default:
                                m_petSummonFailed = true;
                                m_ai.TellMaster("summon pet failed!");
                        }                   
                    }
                    break;
                case SPELL_FAILED_NO_PET:   // This should not happen as if we went this far, there is a pet entry in the DB
                    m_petSummonFailed = true;
                    m_ai.TellMaster("I don't appear to have a pet. Weird...");
                    break;
                default:                    // Also, pure sanity check: should never happen unless there is an error elsewhere
                    m_petSummonFailed = true;
                    m_ai.TellMaster("summon pet failed!");
                    break;
            }
        }
        else if (!(pet->IsAlive()))
        {
            if (PET_REVIVE > 0 && m_ai.CastSpell(PET_REVIVE, m_bot) == SPELL_CAST_OK)
                m_ai.TellMaster("reviving pet.");
        }
        else if (pet->GetHealthPercent() < 50)
        {
            if (PET_MEND > 0 && pet->IsAlive() && !pet->HasAura(PET_MEND, EFFECT_INDEX_0) && m_ai.CastSpell(PET_MEND, m_bot) == SPELL_CAST_OK)
                m_ai.TellMaster("healing pet.");
        }
        else if (pet->GetHappinessState() != HAPPY) // if pet is hungry
        {
            Unit& caster = m_bot;
            // list out items in main backpack
            for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
            {
                Item* const pItem = m_bot.GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                    {
                        // DEBUG_LOG ("[PlayerbotHunterAI]: DoNonCombatActions - Food for pet: %s",pItemProto->Name1);
                        caster.CastSpell(&caster, 51284, TRIGGERED_OLD_TRIGGERED); // pet feed visual
                        uint32 count = 1; // number of items used
                        int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                        m_bot.DestroyItemCount(pItem, count, true); // remove item from inventory
                        m_bot.CastCustomSpell(&m_bot, PET_FEED, &benefit, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED); // feed pet
                        m_ai.TellMaster("feeding pet.");
                        m_ai.SetIgnoreUpdateTime(10);
                        return;
                    }
                }
            }
            // list out items in other removable backpacks
            for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
            {
                const Bag* const pBag = (Bag*) m_bot.GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                if (pBag)
                    for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                    {
                        Item* const pItem = m_bot.GetItemByPos(bag, slot);
                        if (pItem)
                        {
                            const ItemPrototype* const pItemProto = pItem->GetProto();
                            if (!pItemProto)
                                continue;

                            if (pet->HaveInDiet(pItemProto)) // is pItem in pets diet
                            {
                                // DEBUG_LOG ("[PlayerbotHunterAI]: DoNonCombatActions - Food for pet: %s",pItemProto->Name1);
                                caster.CastSpell(&caster, 51284, TRIGGERED_OLD_TRIGGERED); // pet feed visual
                                uint32 count = 1; // number of items used
                                int32 benefit = pet->GetCurrentFoodBenefitLevel(pItemProto->ItemLevel); // nutritional value of food
                                m_bot.DestroyItemCount(pItem, count, true); // remove item from inventory
                                m_bot.CastCustomSpell(&m_bot, PET_FEED, &benefit, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED); // feed pet
                                m_ai.TellMaster("feeding pet.");
                                m_ai.SetIgnoreUpdateTime(10);
                                return;
                            }
                        }
                    }
            }
            if (pet->HasAura(PET_MEND, EFFECT_INDEX_0) && !pet->HasAura(PET_FEED, EFFECT_INDEX_0))
                m_ai.TellMaster("..no pet food!");
            m_ai.SetIgnoreUpdateTime(7);
        }
    }
} // end DoNonCombatActions
