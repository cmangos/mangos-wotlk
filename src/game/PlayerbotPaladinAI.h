#ifndef _PlayerbotPaladinAI_H
#define _PlayerbotPaladinAI_H

#include "PlayerbotClassAI.h"

enum
{
    Combat,
    Healing
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotPaladinAI : PlayerbotClassAI
{
    public:
        PlayerbotPaladinAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotPaladinAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
		// Heals the target based off its hps
        void HealTarget (Unit& target, uint8 hp);

        // Retribution
        uint32 RETRIBUTION_AURA, SEAL_OF_COMMAND, JUDGEMENT_OF_LIGHT, JUDGEMENT_OF_WISDOM, GREATER_BLESSING_OF_WISDOM, GREATER_BLESSING_OF_MIGHT, BLESSING_OF_WISDOM, BLESSING_OF_MIGHT, HAMMER_OF_JUSTICE, RIGHTEOUS_FURY, CRUSADER_AURA, CRUSADER_STRIKE, AVENGING_WRATH, DIVINE_STORM, JUDGEMENT_OF_JUSTICE;

        // Holy
        uint32 FLASH_OF_LIGHT, HOLY_LIGHT, DIVINE_SHIELD, HAMMER_OF_WRATH, CONSECRATION, CONCENTRATION_AURA, DIVINE_FAVOR, SACRED_SHIELD, HOLY_SHOCK, HOLY_WRATH, LAY_ON_HANDS, EXORCISM, DIVINE_PLEA;

        // Protection
        uint32 GREATER_BLESSING_OF_KINGS, BLESSING_OF_KINGS, HAND_OF_PROTECTION, SHADOW_RESISTANCE_AURA, DEVOTION_AURA, FIRE_RESISTANCE_AURA, FROST_RESISTANCE_AURA, DEFENSIVE_STANCE, BERSERKER_STANCE, BATTLE_STANCE, DIVINE_SACRIFICE, DIVINE_PROTECTION, DIVINE_INTERVENTION, HOLY_SHIELD, AVENGERS_SHIELD, RIGHTEOUS_DEFENSE, GREATER_BLESSING_OF_SANCTUARY, HAND_OF_SACRIFICE, SHIELD_OF_RIGHTEOUSNESS;

		// cannot be protected
		uint32 FORBEARANCE;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, CombatCounter, HealCounter;
};

#endif

