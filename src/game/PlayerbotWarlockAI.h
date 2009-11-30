#ifndef _PlayerbotWarlockAI_H
#define _PlayerbotWarlockAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_CURSES,
    SPELL_AFFLICTION,
    SPELL_DESTRUCTION,
    SPELL_DEMONOLOGY
};

//class Player;
class MANGOS_DLL_SPEC PlayerbotWarlockAI : PlayerbotClassAI
{
    public:
        PlayerbotWarlockAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotWarlockAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        //void BuffPlayer(Player *target);

    private:

		bool m_demonSummonFailed;

        // CURSES
        uint32 CURSE_OF_WEAKNESS, CURSE_OF_AGONY, CURSE_OF_EXHAUSTION, CURSE_OF_TONGUES, CURSE_OF_THE_ELEMENTS, CURSE_OF_DOOM;

        // AFFLICTION
        uint32 CORRUPTION, DRAIN_SOUL, DRAIN_LIFE, DRAIN_MANA, LIFE_TAP, UNSTABLE_AFFLICTION, HAUNT, ATROCITY, SEED_OF_CORRUPTION, DARK_PACT, HOWL_OF_TERROR, FEAR;

        // DESTRUCTION
        uint32 SHADOW_BOLT, IMMOLATE, INCINERATE, SEARING_PAIN, CONFLAGRATE, SOUL_FIRE, SHADOWFURY, CHAOS_BOLT, SHADOWFLAME, HELLFIRE, RAIN_OF_FIRE, SHADOWBURN;

        // DEMONOLOGY
        uint32 DEMON_SKIN, DEMON_ARMOR, SHADOW_WARD, FEL_ARMOR, SOULSHATTER, SOUL_LINK, HEALTH_FUNNEL, DETECT_INVISIBILITY;

		// DEMON SUMMON
		uint32 SUMMON_IMP, SUMMON_VOIDWALKER, SUMMON_SUCCUBUS, SUMMON_FELHUNTER, SUMMON_FELGUARD;

		// DEMON SKILLS
		uint32 BLOOD_PACT, CONSUME_SHADOWS, FEL_INTELLIGENCE;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellCurse, LastSpellAffliction, LastSpellDestruction;
};

#endif
