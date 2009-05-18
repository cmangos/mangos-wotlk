#ifndef _PLAYERDEATHKNIGHTAI_H
#define _PLAYERDEATHKNIGHTAI_H

#include "PlayerbotClassAI.h"

enum {
	SPELL_DK_UNHOLY,
	SPELL_DK_FROST,
	SPELL_DK_BLOOD
};


//class Player;

class MANGOS_DLL_SPEC PlayerbotDeathKnightAI : PlayerbotClassAI {
    public:
        PlayerbotDeathKnightAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotDeathKnightAI();

		// all combat actions go here
		void DoNextCombatManeuver(Unit*);

		// all non combat actions go here, ex buffs, heals, rezzes
		void DoNonCombatActions();

		// buff a specific player, usually a real PC who is not in group
		//void BuffPlayer(Player *target);

	private:
		
		// Unholy
		uint32 PLAGUE_STRIKE, DEATH_GRIP, DEATH_COIL, DEATH_STRIKE, UNHOLY_BLIGHT, SCOURGE_STRIKE, DEATH_RUNE_MASTERY, UNHOLY_PRESENCE;

		// Frost
		uint32 ICY_TOUCH, OBLITERATE, HOWLING_BLAST, FROST_STRIKE, FROSTFEVER, RUNE_STRIKE, ICY_CLUTCH, HORN_OF_WINTER, KILLING_MACHINE, FROST_PRESENCE;

		// Blood
		uint32 BLOOD_STRIKE, PESTILENCE, STRANGULATE, BLOOD_BOIL, HEART_STRIKE, MARK_OF_BLOOD, BLOOD_PRESENCE;

		uint32 SpellSequence, LastSpellUnholyDK, LastSpellFrostDK, LastSpellBloodDK;
		
};

#endif