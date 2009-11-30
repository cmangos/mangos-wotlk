
#ifndef _PlayerbotRogueAI_H
#define _PlayerbotRogueAI_H

#include "PlayerbotClassAI.h"

enum
{
    RogueCombat,
    RogueSpellPreventing,
    RogueThreat,
    RogueStealth
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotRogueAI : PlayerbotClassAI
{
    public:
        PlayerbotRogueAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotRogueAI();

        // all combat actions go here
        bool DoFirstCombatManeuver(Unit*);
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // COMBAT
        uint32 SINISTER_STRIKE, BACKSTAB, GOUGE, EVASION, SPRINT, KICK, FEINT, SHIV, FAN_OF_KNIVES;

        // SUBTLETY
        uint32 SHADOWSTEP, STEALTH, VANISH, HEMORRHAGE, BLIND, SHADOW_DANCE, CLOAK_OF_SHADOWS, TRICK_TRADE, CRIPPLING_POISON, DEADLY_POISON, MIND_NUMBING_POISON, GHOSTLY_STRIKE, DISTRACT, PREPARATION, PREMEDITATION;

        // ASSASSINATION
        uint32 EVISCERATE, SLICE_DICE, GARROTE, EXPOSE_ARMOR, AMBUSH, RUPTURE, DISMANTLE, CHEAP_SHOT, KIDNEY_SHOT, MUTILATE, ENVENOM, DEADLY_THROW;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellCombat, LastSpellSubtlety, LastSpellAssassination, Aura;
};

#endif
