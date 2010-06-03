#ifndef _PlayerbotMageAI_H
#define _PlayerbotMageAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_FROST,
    SPELL_FIRE,
    SPELL_ARCANE
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotMageAI : PlayerbotClassAI
{
    public:
        PlayerbotMageAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotMageAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // ARCANE
        uint32 ARCANE_MISSILES, ARCANE_EXPLOSION, COUNTERSPELL, SLOW, ARCANE_BARRAGE, ARCANE_BLAST, MIRROR_IMAGE, ARCANE_POWER;

        // FIRE
        uint32 FIREBALL, FIRE_BLAST, FLAMESTRIKE, SCORCH, PYROBLAST, BLAST_WAVE, COMBUSTION, DRAGONS_BREATH, LIVING_BOMB, FROSTFIRE_BOLT, FIRE_WARD;

        // FROST
        uint32 DEEP_FREEZE, FROSTBOLT, FROST_NOVA, BLIZZARD, ICY_VEINS, CONE_OF_COLD, ICE_BARRIER, SUMMON_WATER_ELEMENTAL, ICE_LANCE, FROST_WARD, ICE_BLOCK, COLD_SNAP;

        // buffs
        uint32 FROST_ARMOR, ICE_ARMOR, MAGE_ARMOR, MOLTEN_ARMOR, ARCANE_INTELLECT, ARCANE_BRILLIANCE, DALARAN_INTELLECT, DALARAN_BRILLIANCE, MANA_SHIELD, DAMPEN_MAGIC, AMPLIFY_MAGIC;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellArcane, LastSpellFire, LastSpellFrost, CONJURE_WATER, CONJURE_FOOD;
};

#endif
