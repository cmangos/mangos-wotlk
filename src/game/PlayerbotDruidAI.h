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

        // druid cat/bear forms & attacks
        uint32 CLAW, COWER, TIGERS_FURY, RAKE, RIP, FEROCIOUS_BITE, MAIM, MANGLE, CAT_FORM, DIRE_BEAR_FORM;

		// druid attacks & debuffs
		uint32 MOONFIRE, ROOTS, WRATH, STARFALL, STARFIRE, INSECT_SWARM, FAERIE_FIRE;

		// druid heals & buffs
		uint32 MARK_OF_THE_WILD, THORNS, LIFEBLOOM, REJUVENATION, REGROWTH, NOURISH, HEALING_TOUCH, WILD_GROWTH, MANA_REJUVENATION;

        uint32 SpellSequence, DruidSpellCombat;
};

#endif
