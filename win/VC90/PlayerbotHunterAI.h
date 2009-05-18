#ifndef _PLAYERHUNTERAI_H
#define _PLAYERHUNTERAI_H

#include "PlayerbotClassAI.h"

enum {
	SPELL_HUNTER
};


//class Player;

class MANGOS_DLL_SPEC PlayerbotHunterAI : PlayerbotClassAI {
    public:
        PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotHunterAI();

		// all combat actions go here
		void DoNextCombatManeuver(Unit*);

		// all non combat actions go here, ex buffs, heals, rezzes
		void DoNonCombatActions();

		// buff a specific player, usually a real PC who is not in group
		//void BuffPlayer(Player *target);

	private:
		
		// Hunter
		uint32 RAPTOR_STRIKE, WING_CLIP, MONGOOSE_BITE, BAD_ATTITUDE, SONIC_BLAST, NETHER_SHOCK, DEMORALIZING_SCREECH, BEAR_TRAP, FREEZING_TRAP, ASPECT_OF_THE_HAWK;

		uint32 SpellSequence, LastSpellHunter;
		
};

#endif