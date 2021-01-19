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

#ifndef _PlayerbotWarriorAI_H
#define _PlayerbotWarriorAI_H

#include "../Base/PlayerbotClassAI.h"

enum
{
    WarriorSpellPreventing,
    WarriorBattle,
    WarriorDefensive,
    WarriorBerserker
};

enum WarriorSpells
{
    AUTO_SHOT_2                     = 3018,
    BATTLE_SHOUT_1                  = 6673,
    BATTLE_STANCE_1                 = 2457,
    BERSERKER_RAGE_1                = 18499,
    BERSERKER_STANCE_1              = 2458,
    BLADESTORM_1                    = 46924,
    BLOODRAGE_1                     = 2687,
    BLOODTHIRST_1                   = 23881,
    CHALLENGING_SHOUT_1             = 1161,
    CHARGE_1                        = 100,
    CLEAVE_1                        = 845,
    COMMANDING_SHOUT_1              = 469,
    CONCUSSION_BLOW_1               = 12809,
    DEATH_WISH_1                    = 12292,
    DEFENSIVE_STANCE_1              = 71,
    DEMORALIZING_SHOUT_1            = 1160,
    DEVASTATE_1                     = 20243,
    DISARM_1                        = 676,
    ENRAGED_REGENERATION_1          = 55694,
    EXECUTE_1                       = 5308,
    HAMSTRING_1                     = 1715,
    HEROIC_FURY_1                   = 60970,
    HEROIC_STRIKE_1                 = 78,
    HEROIC_THROW_1                  = 57755,
    INTERCEPT_1                     = 20252,
    INTERVENE_1                     = 3411,
    INTIMIDATING_SHOUT_1            = 5246,
    LAST_STAND_1                    = 12975,
    MOCKING_BLOW_1                  = 694,
    MORTAL_STRIKE_1                 = 12294,
    OVERPOWER_1                     = 7384,
    PIERCING_HOWL_1                 = 12323,
    PUMMEL_1                        = 6552,
    RECKLESSNESS_1                  = 1719,
    REND_1                          = 772,
    RETALIATION_1                   = 20230,
    REVENGE_1                       = 6572,
    SHATTERING_THROW_1              = 64382,
    SHIELD_BASH_1                   = 72,
    SHIELD_BLOCK_1                  = 2565,
    SHIELD_SLAM_1                   = 23922,
    SHIELD_WALL_1                   = 871,
    SHOCKWAVE_1                     = 46968,
    SLAM_1                          = 1464,
    SPELL_REFLECTION_1              = 23920,
    SUNDER_ARMOR_1                  = 7386,
    SWEEPING_STRIKES_1              = 12328,
    TAUNT_1                         = 355,
    THUNDER_CLAP_1                  = 6343,
    VICTORY_RUSH_1                  = 34428,
    VIGILANCE_1                     = 50720,
    WHIRLWIND_1                     = 1680,

    //Procs
    SLAM_PROC_1                     = 46916,
    BLOODSURGE_1                    = 46915,
    TASTE_FOR_BLOOD_1               = 56638,
    SUDDEN_DEATH_1                  = 52437
};

//class Player;

class PlayerbotWarriorAI : PlayerbotClassAI
{
    public:
        PlayerbotWarriorAI(Player& master, Player& bot, PlayerbotAI& ai);
        virtual ~PlayerbotWarriorAI();

        // all combat actions go here
        CombatManeuverReturns DoFirstCombatManeuver(Unit* pTarget) override;
        CombatManeuverReturns DoNextCombatManeuver(Unit* pTarget) override;
        bool CanPull() override;
        bool Pull() override;

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions() override;

        //Buff/rebuff shouts
        void CheckShouts();

    private:
        CombatManeuverReturns DoFirstCombatManeuverPVE(Unit* pTarget) override;
        CombatManeuverReturns DoNextCombatManeuverPVE(Unit* pTarget) override;
        CombatManeuverReturns DoFirstCombatManeuverPVP(Unit* pTarget) override;
        CombatManeuverReturns DoNextCombatManeuverPVP(Unit* pTarget) override;

        // ARMS
        uint32 BATTLE_STANCE,
               CHARGE,
               HEROIC_STRIKE,
               REND,
               THUNDER_CLAP,
               HAMSTRING,
               MOCKING_BLOW,
               RETALIATION,
               SWEEPING_STRIKES,
               MORTAL_STRIKE,
               BLADESTORM,
               HEROIC_THROW,
               SHATTERING_THROW,
               TASTE_FOR_BLOOD,
               SUDDEN_DEATH;

        // PROTECTION
        uint32 DEFENSIVE_STANCE,
               BLOODRAGE,
               SUNDER_ARMOR,
               TAUNT,
               SHIELD_BASH,
               REVENGE,
               SHIELD_BLOCK,
               DISARM,
               SHIELD_WALL,
               SHIELD_SLAM,
               VIGILANCE,
               DEVASTATE,
               SHOCKWAVE,
               CONCUSSION_BLOW,
               SPELL_REFLECTION,
               LAST_STAND;

        // FURY
        uint32 BERSERKER_STANCE,
               BATTLE_SHOUT,
               DEMORALIZING_SHOUT,
               OVERPOWER,
               CLEAVE,
               INTIMIDATING_SHOUT,
               EXECUTE,
               CHALLENGING_SHOUT,
               SLAM,
               INTERCEPT,
               DEATH_WISH,
               BERSERKER_RAGE,
               WHIRLWIND,
               PUMMEL,
               BLOODTHIRST,
               RECKLESSNESS,
               RAMPAGE,
               HEROIC_FURY,
               COMMANDING_SHOUT,
               ENRAGED_REGENERATION,
               PIERCING_HOWL,
               SLAM_PROC,
               BLOODSURGE;

        // racial
        uint32 ARCANE_TORRENT,
               GIFT_OF_THE_NAARU,
               STONEFORM,
               ESCAPE_ARTIST,
               EVERY_MAN_FOR_HIMSELF,
               SHADOWMELD,
               BLOOD_FURY,
               WAR_STOMP,
               BERSERKING,
               WILL_OF_THE_FORSAKEN;

        // general
        uint32 AUTO_SHOT;

        uint32 SpellSequence;
};

#endif
