#ifndef _PLAYERDEATHKNIGHTAI_H
#define _PLAYERDEATHKNIGHTAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_DK_UNHOLY,
    SPELL_DK_FROST,
    SPELL_DK_BLOOD
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotDeathKnightAI : PlayerbotClassAI
{
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
        uint32 BONE_SHIELD, PLAGUE_STRIKE, DEATH_GRIP, DEATH_COIL, DEATH_STRIKE, UNHOLY_BLIGHT, SCOURGE_STRIKE, DEATH_AND_DECAY, UNHOLY_PRESENCE, RAISE_DEAD, ARMY_OF_THE_DEAD, SUMMON_GARGOYLE, ANTI_MAGIC_SHELL, ANTI_MAGIC_ZONE, GHOUL_FRENZY, CORPSE_EXPLOSION;

        // Frost
        uint32 ICY_TOUCH, OBLITERATE, HOWLING_BLAST, FROST_STRIKE, CHAINS_OF_ICE, RUNE_STRIKE, ICY_CLUTCH, HORN_OF_WINTER, KILLING_MACHINE, FROST_PRESENCE, DEATHCHILL, ICEBOUND_FORTITUDE, MIND_FREEZE, EMPOWER_WEAPON, HUNGERING_COLD, UNBREAKABLE_ARMOR;

        // Blood
        uint32 BLOOD_STRIKE, PESTILENCE, STRANGULATE, BLOOD_BOIL, HEART_STRIKE, MARK_OF_BLOOD, BLOOD_PRESENCE, RUNE_TAP, VAMPIRIC_BLOOD, DEATH_PACT, DEATH_RUNE_MASTERY, HYSTERIA, DANCING_WEAPON, DARK_COMMAND;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellUnholyDK, LastSpellFrostDK, LastSpellBloodDK;
};

#endif

