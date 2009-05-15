#ifndef _PlayerbotWarriorAI_H
#define _PlayerbotWarriorAI_H

#include "PlayerbotClassAI.h"

enum {
	SPELL_ARMS,
	Tanking,
	Berserker
};


//class Player;

class MANGOS_DLL_SPEC PlayerbotWarriorAI : PlayerbotClassAI {
    public:
        PlayerbotWarriorAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotWarriorAI();

		// all combat actions go here
		void DoNextCombatManeuver(Unit*);

		// all non combat actions go here, ex buffs, heals, rezzes
		void DoNonCombatActions();

		// buff a specific player, usually a real PC who is not in group
		void BuffPlayer(Player *target);

	private:

		

		// ARMS
		uint32 HEROIC_STRIKE, REND, THUNDER_CLAP, HAMSTRING, MOCKING_BLOW, RETALIATION, SWEEPING_STRIKES, MORTAL_STRIKE, BLADESTORM, HEROIC_THROW;
		
		// PROTECTION
		uint32 BLOODRAGE, DEFENSIVE_STANCE, SUNDER_ARMOR, TAUNT, SHIELD_BASH, REVENGE, SHIELD_BLOCK, DISARM, SHIELD_WALL, SHIELD_SLAM, VIGILANCE, DEVASTATE,
            SHOCKWAVE, SPELL_REFLECTION;
		
		// FURY
		uint32 BATTLE_SHOUT, DEMORALIZING_SHOUT, CLEAVE, INTIMIDATING_SHOUT, EXECUTE, CHALLENGING_SHOUT, SLAM, BERSERKER_STANCE, INTERCEPT, DEATH_WISH,
            BERSERKER_RAGE, WHIRLWIND, PUMMEL, BLOODTHIRST, RECKLESSNESS, RAMPAGE, HEROIC_FURY, COMMANDING_SHOUT, ENRAGED_REGENERATION;


		uint32 SpellSequence, TankCounter, BerserkerCounter, LastSpellArms, LastSpellProtection, LastSpellFury;
		
		
};

#endif
