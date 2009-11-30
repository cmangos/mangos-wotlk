#ifndef _PLAYERBOTDRUIDAI_H
#define _PLAYERBOTDRUIDAI_H

#include "PlayerbotClassAI.h"

enum
{
    DruidCombat,
    DruidTank,
    DruidHeal,
    DruidSpell
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotDruidAI : PlayerbotClassAI
{
    public:
        PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotDruidAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // Heals the target based off its hps
        void HealTarget (Unit& target, uint8 hp);

		// druid cat/bear/dire bear/moonkin/tree of life forms
		uint32 CAT_FORM, BEAR_FORM, DIRE_BEAR_FORM, MOONKIN_FORM, TREE_OF_LIFE;

        // druid cat attacks
        uint32 CLAW, COWER, TIGERS_FURY, RAKE, RIP, FEROCIOUS_BITE, MAIM, MANGLE;

		// druid bear/dire bear attacks & buffs
		uint32 BASH, MAUL, SWIPE, DEMORALIZING_ROAR, CHALLENGING_ROAR, GROWL, ENRAGE;

		// druid attacks & debuffs
		uint32 MOONFIRE, ROOTS, WRATH, STARFALL, STARFIRE, INSECT_SWARM, FAERIE_FIRE, FORCE_OF_NATURE, HURRICANE;

		// druid buffs
		uint32 MARK_OF_THE_WILD, THORNS, MANA_REJUVENATION, INNERVATE, BARKSKIN;

		// druid heals
		uint32 LIFEBLOOM, REJUVENATION, REGROWTH, NOURISH, HEALING_TOUCH, WILD_GROWTH, SWIFTMEND, TRANQUILITY;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, DruidSpellCombat;
};

#endif
