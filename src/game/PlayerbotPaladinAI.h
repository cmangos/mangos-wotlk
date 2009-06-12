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
        // RET
        uint32 RETRIBUTION_AURA, SEAL_OF_COMMAND, JUDGEMENT_OF_LIGHT, JUDGEMENT_OF_WISDOM, GREATER_BLESSING_OF_WISDOM, GREATER_BLESSING_OF_MIGHT, BLESSING_OF_WISDOM, BLESSING_OF_MIGHT, HAMMER_OF_JUSTICE, RIGHTEOUS_FURY, CRUSADER_AURA, CRUSADER_STRIKE, AVENGING_WRATH;

        // HOLY
        uint32 FLASH_OF_LIGHT, HOLY_LIGHT, DIVINE_SHIELD, HAMMER_OF_WRATH, CONSECRATION, CONCENTRATION_AURA;

        //Protection
        uint32 GREATER_BLESSING_OF_KINGS, BLESSING_OF_KINGS, HAND_OF_PROTECTION, SHADOW_RESISTANCE_AURA, DEVOTION_AURA, FIRE_RESISTANCE_AURA, FROST_RESISTANCE_AURA, DEFENSIVE_STANCE, BERSERKER_STANCE, BATTLE_STANCE;

        uint32 SpellSequence, CombatCounter, HealCounter;
};

#endif

