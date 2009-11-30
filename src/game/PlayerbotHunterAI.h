#ifndef _PLAYERHUNTERAI_H
#define _PLAYERHUNTERAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HUNTER
};


//class Player;

class MANGOS_DLL_SPEC PlayerbotHunterAI : PlayerbotClassAI
{
    public:
        PlayerbotHunterAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotHunterAI();
        bool HasPet(Player* bot);

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        //void BuffPlayer(Player *target);

    private:
        // Hunter
        bool m_petSummonFailed;
        bool m_rangedCombat;

        uint32 PET_SUMMON, PET_DISMISS, PET_REVIVE, PET_MEND, PET_FEED, BAD_ATTITUDE, SONIC_BLAST, NETHER_SHOCK, DEMORALIZING_SCREECH, INTIMIDATION;
        uint32 AUTO_SHOT, HUNTERS_MARK, ARCANE_SHOT, CONCUSSIVE_SHOT, DISTRACTING_SHOT, MULTI_SHOT, EXPLOSIVE_SHOT, SERPENT_STING, SCORPID_STING, VIPER_STING, WYVERN_STING, AIMED_SHOT, STEADY_SHOT, CHIMERA_SHOT, VOLLEY, BLACK_ARROW, KILL_SHOT;
        uint32 RAPTOR_STRIKE, WING_CLIP, MONGOOSE_BITE, DISENGAGE, DETERRENCE;
        uint32 BEAR_TRAP, FREEZING_TRAP, IMMOLATION_TRAP, FROST_TRAP, EXPLOSIVE_TRAP, ARCANE_TRAP, SNAKE_TRAP;
        uint32 ASPECT_OF_THE_HAWK, ASPECT_OF_THE_MONKEY, RAPID_FIRE, TRUESHOT_AURA, MISDIRECTION;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;
};

#endif

