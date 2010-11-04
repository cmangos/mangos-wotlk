#ifndef _PLAYERBOTSHAMANAI_H
#define _PLAYERBOTSHAMANAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_ENHANCEMENT,
    SPELL_RESTORATION,
    SPELL_ELEMENTAL
};

enum
{
    ANCESTRAL_SPIRIT_1              = 2008,
    ASTRAL_RECALL_1                 = 556,
    BLOODLUST_1                     = 2825,
    CALL_OF_THE_ANCESTORS_1         = 66843,
    CALL_OF_THE_ELEMENTS_1          = 66842,
    CALL_OF_THE_SPIRITS_1           = 66844,
    CHAIN_HEAL_1                    = 1064,
    CHAIN_LIGHTNING_1               = 421,
    CHAINED_HEAL_1                  = 70809,
    CLEANSE_SPIRIT_1                = 51886,
    CLEANSING_TOTEM_1               = 8170,
    CURE_TOXINS_1                   = 526,
    EARTH_ELEMENTAL_TOTEM_1         = 2062,
    EARTH_SHIELD_1                  = 974,
    EARTH_SHOCK_1                   = 8042,
    EARTHBIND_TOTEM_1               = 2484,
    EARTHLIVING_WEAPON_1            = 51730,
    ELEMENTAL_MASTERY_1             = 16166,
    FERAL_SPIRIT_1                  = 51533,
    FIRE_ELEMENTAL_TOTEM_1          = 2894,
    FIRE_NOVA_1                     = 1535,
    FIRE_RESISTANCE_TOTEM_1         = 8184,
    FLAME_SHOCK_1                   = 8050,
    FLAMETONGUE_TOTEM_1             = 8227,
    FLAMETONGUE_WEAPON_1            = 8024,
    FROST_RESISTANCE_TOTEM_1        = 8181,
    FROST_SHOCK_1                   = 8056,
    FROSTBRAND_WEAPON_1             = 8033,
    GHOST_WOLF_1                    = 2645,
    GROUNDING_TOTEM_1               = 8177,
    HEALING_STREAM_TOTEM_1          = 5394,
    HEALING_WAVE_1                  = 331,
    HEROISM_1                       = 32182,
    HEX_1                           = 51514,
    LAVA_BURST_1                    = 51505,
    LAVA_LASH_1                     = 60103,
    LESSER_HEALING_WAVE_1           = 8004,
    LIGHTNING_BOLT_1                = 403,
    LIGHTNING_SHIELD_1              = 324,
    MAGMA_TOTEM_1                   = 8190,
    MANA_SPRING_TOTEM_1             = 5675,
    MANA_TIDE_TOTEM_1               = 16190,
    NATURE_RESISTANCE_TOTEM_1       = 10595,
    NATURES_SWIFTNESS_SHAMAN_1      = 16188,
    PURGE_1                         = 370,
    RIPTIDE_1                       = 61295,
    ROCKBITER_WEAPON_1              = 8017,
    SEARING_TOTEM_1                 = 3599,
    SENTRY_TOTEM_1                  = 6495,
    SHAMANISTIC_RAGE_1              = 30823,
    STONECLAW_TOTEM_1               = 5730,
    STONESKIN_TOTEM_1               = 8071,
    STORMSTRIKE_1                   = 17364,
    STRENGTH_OF_EARTH_TOTEM_1       = 8075,
    THUNDERSTORM_1                  = 51490,
    TIDAL_FORCE_1                   = 55198,
    TOTEM_OF_WRATH_1                = 30706,
    TOTEMIC_RECALL_1                = 36936,
    TREMOR_TOTEM_1                  = 8143,
    WATER_BREATHING_1               = 131,
    WATER_SHIELD_1                  = 52127,
    WATER_WALKING_1                 = 546,
    WIND_SHEAR_1                    = 57994,
    WINDFURY_TOTEM_1                = 8512,
    WINDFURY_WEAPON_1               = 8232,
    WRATH_OF_AIR_TOTEM_1            = 3738
};
//class Player;

class MANGOS_DLL_SPEC PlayerbotShamanAI : PlayerbotClassAI
{
public:
    PlayerbotShamanAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotShamanAI();

    // all combat actions go here
    void DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    void DoNonCombatActions();

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
