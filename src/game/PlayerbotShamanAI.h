#ifndef _PLAYERBOTSHAMANAI_H
#define _PLAYERBOTSHAMANAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_ENHANCEMENT,
    SPELL_RESTORATION,
    SPELL_ELEMENTAL
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotShamanAI : PlayerbotClassAI
{
    public:
        PlayerbotShamanAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotShamanAI();

        // all combat actions go here
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // Heals the target based off its hps
        void HealTarget (Unit& target, uint8 hp);

        // ENHANCEMENT
        uint32 ROCKBITER_WEAPON, STONESKIN_TOTEM, LIGHTNING_SHIELD, FLAMETONGUE_WEAPON, STRENGTH_OF_EARTH_TOTEM, FOCUSED, FROSTBRAND_WEAPON, FROST_RESISTANCE_TOTEM, FLAMETONGUE_TOTEM, FIRE_RESISTANCE_TOTEM, WINDFURY_WEAPON, GROUNDING_TOTEM, NATURE_RESISTANCE_TOTEM, WIND_FURY_TOTEM, STORMSTRIKE, LAVA_LASH, SHAMANISTIC_RAGE, WRATH_OF_AIR_TOTEM, EARTH_ELEMENTAL_TOTEM, BLOODLUST, HEROISM, FERAL_SPIRIT;

        // RESTORATION
        uint32 HEALING_WAVE, LESSER_HEALING_WAVE, ANCESTRAL_SPIRIT, TREMOR_TOTEM, HEALING_STREAM_TOTEM, MANA_SPRING_TOTEM, CHAIN_HEAL, MANA_TIDE_TOTEM, EARTH_SHIELD, WATER_SHIELD, EARTHLIVING_WEAPON, RIPTIDE;

        // ELEMENTAL
        uint32 LIGHTNING_BOLT, EARTH_SHOCK, STONECLAW_TOTEM, FLAME_SHOCK, SEARING_TOTEM, PURGE, FIRE_NOVA_TOTEM, WIND_SHOCK, FROST_SHOCK, MAGMA_TOTEM, CHAIN_LIGHTNING, TOTEM_OF_WRATH, FIRE_ELEMENTAL_TOTEM, LAVA_BURST, EARTHBIND_TOTEM, HEX;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellEnhancement, LastSpellRestoration, LastSpellElemental;
};

#endif
