#ifndef _PLAYERBOTPRIESTAI_H
#define _PLAYERBOTPRIESTAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HOLY,
    SPELL_SHADOWMAGIC,
    SPELL_DISCIPLINE
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotPriestAI : PlayerbotClassAI
{
    public:
        PlayerbotPriestAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotPriestAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // Heals the target based off its hps
        void HealTarget (Unit& target, uint8 hp);

        // holy
        uint32 GREAT_HEAL, RENEW, HEAL, FLASH_HEAL, REZZ, SMITE, CLEARCASTING, HOLY_NOVA, HOLY_FIRE, DESPERATE_PRAYER, PRAYER_OF_HEALING, CIRCLE_OF_HEALING, BINDING_HEAL, PRAYER_OF_MENDING, MANA_BURN;

        // shadowmagic
        uint32 FADE, PAIN, MIND_BLAST, SCREAM, MIND_FLAY, DEVOURING_PLAGUE, SHADOW_PROTECTION, VAMPIRIC_TOUCH, PRAYER_OF_SHADOW_PROTECTION, SHADOWFIEND, MIND_SEAR;

        // discipline
        uint32 PWS, INNER_FIRE, FORTITUDE, TOUCH_OF_WEAKNESS, FEAR_WARD, DSPIRIT, POWER_INFUSION, MASS_DISPEL, PENANCE, DIVINE_SPIRIT, INNER_FOCUS;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellHoly, LastSpellShadowMagic, LastSpellDiscipline;
};

#endif
