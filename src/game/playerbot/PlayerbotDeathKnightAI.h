#ifndef _PLAYERDEATHKNIGHTAI_H
#define _PLAYERDEATHKNIGHTAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_DK_UNHOLY,
    SPELL_DK_FROST,
    SPELL_DK_BLOOD
};

enum DeathKnightSpells
{
    ANTI_MAGIC_SHELL_1              = 48707,
    ANTI_MAGIC_ZONE_1               = 51052,
    ARMY_OF_THE_DEAD_1              = 42650,
    BLOOD_BOIL_1                    = 48721,
    BLOOD_PRESENCE_1                = 48266,
    BLOOD_STRIKE_1                  = 45902,
    BLOOD_TAP_1                     = 45529,
    BONE_SHIELD_1                   = 49222,
    CHAINS_OF_ICE_1                 = 45524,
    CORPSE_EXPLOSION_1              = 49158,
    DANCING_RUNE_WEAPON_1           = 49028,
    DARK_COMMAND_1                  = 56222,
    DEATH_AND_DECAY_1               = 43265,
    DEATH_COIL_DEATH_KNIGHT_1       = 47541,
    DEATH_GRIP_1                    = 49576,
    DEATH_PACT_1                    = 48743,
    DEATH_STRIKE_1                  = 49998,
    DEATHCHILL_1                    = 49796,
    EMPOWER_RUNE_WEAPON_1           = 47568,
    FROST_PRESENCE_1                = 48263,
    FROST_STRIKE_1                  = 49143,
    GHOUL_FRENZY_1                  = 63560,
    HEART_STRIKE_1                  = 55050,
    HORN_OF_WINTER_1                = 57330,
    HOWLING_BLAST_1                 = 49184,
    HUNGERING_COLD_1                = 49203,
    HYSTERIA_1                      = 49016,
    ICEBOUND_FORTITUDE_1            = 48792,
    ICY_TOUCH_1                     = 45477,
    LICHBORNE_1                     = 49039,
    MARK_OF_BLOOD_1                 = 49005,
    MIND_FREEZE_1                   = 47528,
    OBLITERATE_1                    = 49020,
    PATH_OF_FROST_1                 = 3714,
    PESTILENCE_1                    = 50842,
    PLAGUE_STRIKE_1                 = 45462,
    RAISE_ALLY_1                    = 61999,
    RAISE_DEAD_1                    = 46584,
    RUNE_STRIKE_1                   = 56815,
    RUNE_TAP_1                      = 48982,
    SCOURGE_STRIKE_1                = 55090,
    STRANGULATE_1                   = 47476,
    SUMMON_GARGOYLE_1               = 49206,
    UNBREAKABLE_ARMOR_1             = 51271,
    UNHOLY_PRESENCE_1               = 48265,
    VAMPIRIC_BLOOD_1                = 55233
};
//class Player;

class MANGOS_DLL_SPEC PlayerbotDeathKnightAI : PlayerbotClassAI
{
public:
    PlayerbotDeathKnightAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
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
