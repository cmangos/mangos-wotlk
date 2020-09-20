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

#include "PlayerbotDeathKnightAI.h"
#include "../Base/PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotDeathKnightAI::PlayerbotDeathKnightAI(Player& master, Player& bot, PlayerbotAI& ai) : PlayerbotClassAI(master, bot, ai)
{

    PLAGUE_STRIKE     = m_ai.initSpell(PLAGUE_STRIKE_1); // Unholy
    DEATH_GRIP        = m_ai.initSpell(DEATH_GRIP_1);
    DEATH_COIL        = m_ai.initSpell(DEATH_COIL_DEATH_KNIGHT_1);
    DEATH_STRIKE      = m_ai.initSpell(DEATH_STRIKE_1);
    UNHOLY_BLIGHT     = 0; // Passive
    SCOURGE_STRIKE    = m_ai.initSpell(SCOURGE_STRIKE_1);
    DEATH_AND_DECAY   = m_ai.initSpell(DEATH_AND_DECAY_1);
    CORPSE_EXPLOSION  = m_ai.initSpell(CORPSE_EXPLOSION_1);
    BONE_SHIELD       = m_ai.initSpell(BONE_SHIELD_1); // buffs
    ANTI_MAGIC_SHELL  = m_ai.initSpell(ANTI_MAGIC_SHELL_1);
    ANTI_MAGIC_ZONE   = m_ai.initSpell(ANTI_MAGIC_ZONE_1);
    GHOUL_FRENZY      = m_ai.initSpell(GHOUL_FRENZY_1);
    RAISE_DEAD        = m_ai.initSpell(RAISE_DEAD_1); // pets
    SUMMON_GARGOYLE   = m_ai.initSpell(SUMMON_GARGOYLE_1);
    ARMY_OF_THE_DEAD  = m_ai.initSpell(ARMY_OF_THE_DEAD_1);
    ICY_TOUCH         = m_ai.initSpell(ICY_TOUCH_1); // Frost
    OBLITERATE        = m_ai.initSpell(OBLITERATE_1);
    HOWLING_BLAST     = m_ai.initSpell(HOWLING_BLAST_1);
    FROST_STRIKE      = m_ai.initSpell(FROST_STRIKE_1);
    CHAINS_OF_ICE     = m_ai.initSpell(CHAINS_OF_ICE_1);
    RUNE_STRIKE       = m_ai.initSpell(RUNE_STRIKE_1);
    ICY_CLUTCH        = 0; // No such spell
    MIND_FREEZE       = m_ai.initSpell(MIND_FREEZE_1);
    HUNGERING_COLD    = m_ai.initSpell(HUNGERING_COLD_1);
    KILLING_MACHINE   = 0; // Passive
    DEATHCHILL        = m_ai.initSpell(DEATHCHILL_1);
    HORN_OF_WINTER    = m_ai.initSpell(HORN_OF_WINTER_1);
    ICEBOUND_FORTITUDE = m_ai.initSpell(ICEBOUND_FORTITUDE_1);
    EMPOWER_WEAPON    = m_ai.initSpell(EMPOWER_RUNE_WEAPON_1);
    UNBREAKABLE_ARMOR = m_ai.initSpell(UNBREAKABLE_ARMOR_1);
    BLOOD_STRIKE      = m_ai.initSpell(BLOOD_STRIKE_1); // Blood
    PESTILENCE        = m_ai.initSpell(PESTILENCE_1);
    STRANGULATE       = m_ai.initSpell(STRANGULATE_1);
    BLOOD_BOIL        = m_ai.initSpell(BLOOD_BOIL_1);
    HEART_STRIKE      = m_ai.initSpell(HEART_STRIKE_1);
    DANCING_WEAPON    = m_ai.initSpell(DANCING_RUNE_WEAPON_1);
    DARK_COMMAND      = m_ai.initSpell(DARK_COMMAND_1);
    MARK_OF_BLOOD     = m_ai.initSpell(MARK_OF_BLOOD_1); // buffs
    RUNE_TAP          = m_ai.initSpell(RUNE_TAP_1);
    VAMPIRIC_BLOOD    = m_ai.initSpell(VAMPIRIC_BLOOD_1);
    DEATH_PACT        = m_ai.initSpell(DEATH_PACT_1);
    HYSTERIA          = m_ai.initSpell(HYSTERIA_1);
    UNHOLY_PRESENCE   = m_ai.initSpell(UNHOLY_PRESENCE_1); // presence (TODO: better spell == presence)
    FROST_PRESENCE    = m_ai.initSpell(FROST_PRESENCE_1);
    BLOOD_PRESENCE    = m_ai.initSpell(BLOOD_PRESENCE_1);

    RECENTLY_BANDAGED = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = m_ai.initSpell(ARCANE_TORRENT_DEATH_KNIGHT); // blood elf
    GIFT_OF_THE_NAARU       = m_ai.initSpell(GIFT_OF_THE_NAARU_DEATH_KNIGHT); // draenei
    STONEFORM               = m_ai.initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = m_ai.initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = m_ai.initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY              = m_ai.initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = m_ai.initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = m_ai.initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = m_ai.initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotDeathKnightAI::~PlayerbotDeathKnightAI() {}

CombatManeuverReturns PlayerbotDeathKnightAI::DoFirstCombatManeuver(Unit* pTarget)
{
    // There are NPCs in BGs and Open World PvP, so don't filter this on PvP scenarios (of course if PvP targets anyone but tank, all bets are off anyway)
    // Wait until the tank says so, until any non-tank gains aggro or X seconds - whichever is shortest
    if (m_ai.GetCombatOrder() & PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO)
    {
        if (m_WaitUntil > m_ai.CurrentTime() && m_ai.GroupTankHoldsAggro())
        {
            if (PlayerbotAI::ORDERS_TANK & m_ai.GetCombatOrder())
            {
                if (m_bot.GetDistance(pTarget, true, DIST_CALC_COMBAT_REACH_WITH_MELEE) <= ATTACK_DISTANCE)
                {
                    // Set everyone's UpdateAI() waiting to 2 seconds
                    m_ai.SetGroupIgnoreUpdateTime(2);
                    // Clear their TEMP_WAIT_TANKAGGRO flag
                    m_ai.ClearGroupCombatOrder(PlayerbotAI::ORDERS_TEMP_WAIT_TANKAGGRO);
                    // Start attacking, force target on current target
                    m_ai.Attack(m_ai.GetCurrentTarget());

                    // While everyone else is waiting 2 second, we need to build up aggro, so don't return
                }
                else
                {
                    // TODO: add check if target is ranged
                    return RETURN_NO_ACTION_OK; // wait for target to get nearer
                }
            }
            else
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

CombatManeuverReturns PlayerbotDeathKnightAI::DoFirstCombatManeuverPVE(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotDeathKnightAI::DoFirstCombatManeuverPVP(Unit* /*pTarget*/)
{
    return RETURN_NO_ACTION_OK;
}

CombatManeuverReturns PlayerbotDeathKnightAI::DoNextCombatManeuver(Unit* pTarget)
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

CombatManeuverReturns PlayerbotDeathKnightAI::DoNextCombatManeuverPVE(Unit* pTarget)
{
    // DK Attacks: Unholy, Frost & Blood

    // damage spells
    Unit* pVictim = pTarget->GetVictim();
    Pet* pet = m_bot.GetPet();
    float dist = m_bot.GetDistance(pTarget, true, DIST_CALC_COMBAT_REACH_WITH_MELEE);
    std::ostringstream out;

    switch (SpellSequence)
    {
        case SPELL_DK_UNHOLY:
            if (UNHOLY_PRESENCE > 0 && !m_bot.HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && m_ai.CastSpell(UNHOLY_PRESENCE, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;

            // check for BONE_SHIELD in combat
            if (BONE_SHIELD > 0 && !m_bot.HasAura(BONE_SHIELD, EFFECT_INDEX_0) && !m_bot.HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && m_ai.CastSpell(BONE_SHIELD, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;

            if (ARMY_OF_THE_DEAD > 0 && m_ai.GetAttackerCount() >= 5 && LastSpellUnholyDK < 1 && m_ai.CastSpell(ARMY_OF_THE_DEAD) == SPELL_CAST_OK && m_bot.HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0))
            {
                out << " summoning Army of the Dead!";
                m_ai.SetIgnoreUpdateTime(7);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (PLAGUE_STRIKE > 0 && !pTarget->HasAura(PLAGUE_STRIKE, EFFECT_INDEX_0) && LastSpellUnholyDK < 2 && m_ai.CastSpell(PLAGUE_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Plague Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (DEATH_GRIP > 0 && !pTarget->HasAura(DEATH_GRIP, EFFECT_INDEX_0) && LastSpellUnholyDK < 3 && m_ai.CastSpell(DEATH_GRIP, *pTarget) == SPELL_CAST_OK)
            {
                out << " Death Grip";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (DEATH_COIL > 0 && LastSpellUnholyDK < 4 && m_ai.CastSpell(DEATH_COIL, *pTarget) == SPELL_CAST_OK)
            {
                out << " Death Coil";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (DEATH_STRIKE > 0 && !pTarget->HasAura(DEATH_STRIKE, EFFECT_INDEX_0) && LastSpellUnholyDK < 5 && m_ai.CastSpell(DEATH_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Death Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (UNHOLY_BLIGHT > 0 && !pTarget->HasAura(UNHOLY_BLIGHT, EFFECT_INDEX_0) && LastSpellUnholyDK < 6 && m_ai.CastSpell(UNHOLY_BLIGHT) == SPELL_CAST_OK)
            {
                out << " Unholy Blight";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (SCOURGE_STRIKE > 0 && LastSpellUnholyDK < 7 && m_ai.CastSpell(SCOURGE_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Scourge Strike";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (DEATH_AND_DECAY > 0 && m_ai.GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && !pTarget->HasAura(DEATH_AND_DECAY, EFFECT_INDEX_0) && LastSpellUnholyDK < 8 && m_ai.CastSpell(DEATH_AND_DECAY) == SPELL_CAST_OK)
            {
                out << " Death and Decay";
                m_ai.SetIgnoreUpdateTime(1);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (SUMMON_GARGOYLE > 0 && !m_bot.HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && !pTarget->HasAura(SUMMON_GARGOYLE, EFFECT_INDEX_0) && LastSpellUnholyDK < 9 && m_ai.CastSpell(SUMMON_GARGOYLE, *pTarget) == SPELL_CAST_OK)
            {
                out << " summoning Gargoyle";
                m_ai.SetIgnoreUpdateTime(2);
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (CORPSE_EXPLOSION > 0 && dist <= ATTACK_DISTANCE && LastSpellUnholyDK < 10 && m_ai.CastSpell(CORPSE_EXPLOSION, *pTarget) == SPELL_CAST_OK)
            {
                out << " Corpse Explosion";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (ANTI_MAGIC_SHELL > 0 && pTarget->IsNonMeleeSpellCasted(true) && !m_bot.HasAura(ANTI_MAGIC_SHELL, EFFECT_INDEX_0) && LastSpellUnholyDK < 11 && m_ai.CastSpell(ANTI_MAGIC_SHELL, m_bot) == SPELL_CAST_OK)
            {
                out << " Anti-Magic Shell";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (ANTI_MAGIC_ZONE > 0 && pTarget->IsNonMeleeSpellCasted(true) && !m_bot.HasAura(ANTI_MAGIC_SHELL, EFFECT_INDEX_0) && LastSpellUnholyDK < 12 && m_ai.CastSpell(ANTI_MAGIC_ZONE, m_bot) == SPELL_CAST_OK)
            {
                out << " Anti-Magic Zone";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (!pet && RAISE_DEAD > 0 && !m_bot.HasAura(ARMY_OF_THE_DEAD, EFFECT_INDEX_0) && LastSpellUnholyDK < 13 && m_ai.CastSpell(RAISE_DEAD) == SPELL_CAST_OK)
            {
                out << " summoning Ghoul";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (pet && GHOUL_FRENZY > 0 && pVictim == pet && !pet->HasAura(GHOUL_FRENZY, EFFECT_INDEX_0) && LastSpellUnholyDK < 14 && m_ai.CastSpell(GHOUL_FRENZY, *pet) == SPELL_CAST_OK)
            {
                out << " casting Ghoul Frenzy on pet";
                SpellSequence = SPELL_DK_FROST;
                LastSpellUnholyDK = LastSpellUnholyDK + 1;
                return RETURN_CONTINUE;
            }
            if (LastSpellUnholyDK > 15)
            {
                LastSpellUnholyDK = 0;
                SpellSequence = SPELL_DK_FROST;
                return RETURN_NO_ACTION_OK; // Not really OK but that's just how the DK rotation works right now
            }

            LastSpellUnholyDK = 0;

        case SPELL_DK_FROST:
            if (FROST_PRESENCE > 0 && !m_bot.HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && m_ai.CastSpell(FROST_PRESENCE, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;

            if (DEATHCHILL > 0)
            {
                if (!m_bot.HasAura(DEATHCHILL, EFFECT_INDEX_0) && !m_bot.HasAura(KILLING_MACHINE, EFFECT_INDEX_0) && m_ai.CastSpell(DEATHCHILL, m_bot) == SPELL_CAST_OK)
                    return RETURN_CONTINUE;
            }
            else if (KILLING_MACHINE > 0)
            {
                if (!m_bot.HasAura(KILLING_MACHINE, EFFECT_INDEX_0) && !m_bot.HasAura(DEATHCHILL, EFFECT_INDEX_0) && m_ai.CastSpell(KILLING_MACHINE, m_bot) == SPELL_CAST_OK)
                    return RETURN_CONTINUE;
            }

            if (ICY_TOUCH > 0 && !pTarget->HasAura(ICY_TOUCH, EFFECT_INDEX_0) && LastSpellFrostDK < 1 && m_ai.CastSpell(ICY_TOUCH, *pTarget) == SPELL_CAST_OK)
            {
                out << " Icy Touch";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (OBLITERATE > 0 && LastSpellFrostDK < 2 && m_ai.CastSpell(OBLITERATE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Obliterate";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (FROST_STRIKE > 0 && LastSpellFrostDK < 3 && m_ai.CastSpell(FROST_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Frost Strike";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (HOWLING_BLAST > 0 && m_ai.GetAttackerCount() >= 3 && LastSpellFrostDK < 4 && m_ai.CastSpell(HOWLING_BLAST, *pTarget) == SPELL_CAST_OK)
            {
                out << " Howling Blast";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (CHAINS_OF_ICE > 0 && !pTarget->HasAura(CHAINS_OF_ICE, EFFECT_INDEX_0) && LastSpellFrostDK < 5 && m_ai.CastSpell(CHAINS_OF_ICE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Chains of Ice";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (RUNE_STRIKE > 0 && LastSpellFrostDK < 6 && m_ai.CastSpell(RUNE_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Rune Strike";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (ICY_CLUTCH > 0 && !pTarget->HasAura(ICY_CLUTCH, EFFECT_INDEX_0) && LastSpellFrostDK < 7 && m_ai.CastSpell(ICY_CLUTCH, *pTarget) == SPELL_CAST_OK)
            {
                out << " Icy Clutch";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (ICEBOUND_FORTITUDE > 0 && m_ai.GetHealthPercent() < 50 && pVictim == &m_bot && !m_bot.HasAura(ICEBOUND_FORTITUDE, EFFECT_INDEX_0) && LastSpellFrostDK < 8 && m_ai.CastSpell(ICEBOUND_FORTITUDE, m_bot) == SPELL_CAST_OK)
            {
                out << " Icebound Fortitude";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (MIND_FREEZE > 0 && pTarget->IsNonMeleeSpellCasted(true) && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 9 && m_ai.CastSpell(MIND_FREEZE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Mind Freeze";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (HUNGERING_COLD > 0 && m_ai.GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 10 && m_ai.CastSpell(HUNGERING_COLD, *pTarget) == SPELL_CAST_OK)
            {
                out << " Hungering Cold";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (EMPOWER_WEAPON > 0 && LastSpellFrostDK < 11 && m_ai.CastSpell(EMPOWER_WEAPON, m_bot) == SPELL_CAST_OK)
            {
                out << " Empower Rune Weapon";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (UNBREAKABLE_ARMOR > 0 && !m_bot.HasAura(UNBREAKABLE_ARMOR, EFFECT_INDEX_0) && m_ai.GetHealthPercent() < 70 && pVictim == &m_bot && LastSpellFrostDK < 12 && m_ai.CastSpell(UNBREAKABLE_ARMOR, m_bot) == SPELL_CAST_OK)
            {
                out << " Unbreakable Armor";
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = LastSpellFrostDK + 1;
                return RETURN_CONTINUE;
            }
            if (LastSpellFrostDK > 13)
            {
                LastSpellFrostDK = 0;
                SpellSequence = SPELL_DK_BLOOD;
                return RETURN_NO_ACTION_OK; // Not really OK, but that's just how the DK rotation works right now
            }

            LastSpellFrostDK = 0;

        case SPELL_DK_BLOOD:
            if (BLOOD_PRESENCE > 0 && !m_bot.HasAura(BLOOD_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(UNHOLY_PRESENCE, EFFECT_INDEX_0) && !m_bot.HasAura(FROST_PRESENCE, EFFECT_INDEX_0) && m_ai.CastSpell(BLOOD_PRESENCE, m_bot) == SPELL_CAST_OK)
                return RETURN_CONTINUE;

            if (MARK_OF_BLOOD > 0 && !pTarget->HasAura(MARK_OF_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 1 && m_ai.CastSpell(MARK_OF_BLOOD, *pTarget) == SPELL_CAST_OK)
            {
                out << " Mark of Blood";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (BLOOD_STRIKE > 0 && LastSpellBloodDK < 2 && m_ai.CastSpell(BLOOD_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Blood Strike";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK =  LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (PESTILENCE > 0 && m_ai.GetAttackerCount() >= 3 && LastSpellBloodDK < 3 && m_ai.CastSpell(PESTILENCE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Pestilence";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (STRANGULATE > 0 && !pTarget->HasAura(STRANGULATE, EFFECT_INDEX_0) && LastSpellBloodDK < 4 && m_ai.CastSpell(STRANGULATE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Strangulate";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (BLOOD_BOIL > 0 && m_ai.GetAttackerCount() >= 5 && dist <= ATTACK_DISTANCE && LastSpellBloodDK < 5 && m_ai.CastSpell(BLOOD_BOIL, *pTarget) == SPELL_CAST_OK)
            {
                out << " Blood Boil";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (HEART_STRIKE > 0 && LastSpellBloodDK < 6 && m_ai.CastSpell(HEART_STRIKE, *pTarget) == SPELL_CAST_OK)
            {
                out << " Heart Strike";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (VAMPIRIC_BLOOD > 0 && m_ai.GetHealthPercent() < 70 && !m_bot.HasAura(VAMPIRIC_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 7 && m_ai.CastSpell(VAMPIRIC_BLOOD, m_bot) == SPELL_CAST_OK)
            {
                out << " Vampiric Blood";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (RUNE_TAP > 0 && m_ai.GetHealthPercent() < 70 && !m_bot.HasAura(VAMPIRIC_BLOOD, EFFECT_INDEX_0) && LastSpellBloodDK < 8 && m_ai.CastSpell(RUNE_TAP, m_bot) == SPELL_CAST_OK)
            {
                out << " Rune Tap";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (HYSTERIA > 0 && m_ai.GetHealthPercent() > 25 && !m_bot.HasAura(HYSTERIA, EFFECT_INDEX_0) && LastSpellBloodDK < 9 && m_ai.CastSpell(HYSTERIA, m_bot) == SPELL_CAST_OK)
            {
                out << " Hysteria";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (DANCING_WEAPON > 0 && !m_bot.HasAura(DANCING_WEAPON, EFFECT_INDEX_0) && LastSpellBloodDK < 10 && m_ai.CastSpell(DANCING_WEAPON, *pTarget) == SPELL_CAST_OK)
            {
                out << " summoning Dancing Rune Weapon";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (DARK_COMMAND > 0 && m_ai.GetHealthPercent() > 50 && pVictim != &m_bot && !pTarget->HasAura(DARK_COMMAND, EFFECT_INDEX_0) && LastSpellBloodDK < 11 && m_ai.CastSpell(DARK_COMMAND, *pTarget) == SPELL_CAST_OK)
            {
                out << " Dark Command";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (pet && DEATH_PACT > 0 && m_ai.GetHealthPercent() < 50 && LastSpellBloodDK < 12 && m_ai.CastSpell(DEATH_PACT, *pet) == SPELL_CAST_OK)
            {
                out << " Death Pact (sacrifice pet)";
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK = LastSpellBloodDK + 1;
                return RETURN_CONTINUE;
            }
            if (LastSpellBloodDK > 13)
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
                return RETURN_NO_ACTION_OK; // Not really OK but that's just how DK rotation works right now
            }
    }
    //if (m_ai.GetManager()->m_confDebugWhisper)
    //    m_ai.TellMaster(out.str().c_str());

    return RETURN_NO_ACTION_UNKNOWN;
} // end DoNextCombatManeuver

CombatManeuverReturns PlayerbotDeathKnightAI::DoNextCombatManeuverPVP(Unit* pTarget)
{
    if (m_ai.CastSpell(PLAGUE_STRIKE) == SPELL_CAST_OK)
        return RETURN_CONTINUE;

    return DoNextCombatManeuverPVE(pTarget); // TODO: bad idea perhaps, but better than the alternative
}

void PlayerbotDeathKnightAI::DoNonCombatActions()
{
    SpellSequence = SPELL_DK_UNHOLY;

    // buff master with HORN_OF_WINTER
    if (HORN_OF_WINTER > 0 && !m_master.HasAura(HORN_OF_WINTER, EFFECT_INDEX_0) && m_ai.CastSpell(HORN_OF_WINTER, m_master) == SPELL_CAST_OK)
        return;

    // hp check
    if (m_bot.getStandState() != UNIT_STAND_STATE_STAND)
        m_bot.SetStandState(UNIT_STAND_STATE_STAND);

    if (EatDrinkBandage(false))
        return;
} // end DoNonCombatActions

// Match up with "Pull()" below
bool PlayerbotDeathKnightAI::CanPull()
{
    if (DEATH_GRIP && m_bot.IsSpellReady(DEATH_GRIP))
        return true;

    return false;
}

// Match up with "CanPull()" above
bool PlayerbotDeathKnightAI::Pull()
{
    if (DEATH_GRIP && m_ai.CastSpell(DEATH_GRIP))
        return true;

    return false;
}
