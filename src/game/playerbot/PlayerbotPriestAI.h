#ifndef _PLAYERBOTPRIESTAI_H
#define _PLAYERBOTPRIESTAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HOLY,
    SPELL_SHADOWMAGIC,
    SPELL_DISCIPLINE
};

enum PriestSpells
{
    ABOLISH_DISEASE_1               = 552,
    BINDING_HEAL_1                  = 32546,
    BLESSED_HEALING_1               = 70772,
    CIRCLE_OF_HEALING_1             = 34861,
    CURE_DISEASE_1                  = 528,
    DESPERATE_PRAYER_1              = 19236,
    DEVOURING_PLAGUE_1              = 2944,
    DISPEL_MAGIC_1                  = 527,
    DISPERSION_1                    = 47585,
    DIVINE_HYMN_1                   = 64843,
    DIVINE_SPIRIT_1                 = 14752,
    FADE_1                          = 586,
    FEAR_WARD_1                     = 6346,
    FLASH_HEAL_1                    = 2061,
    GREATER_HEAL_1                  = 2060,
    GUARDIAN_SPIRIT_1               = 47788,
    HEAL_1                          = 2054,
    HOLY_FIRE_1                     = 14914,
    HOLY_NOVA_1                     = 15237,
    HYMN_OF_HOPE_1                  = 64901,
    INNER_FIRE_1                    = 588,
    INNER_FOCUS_1                   = 14751,
    LESSER_HEAL_1                   = 2050,
    LEVITATE_1                      = 1706,
    LIGHTWELL_1                     = 724,
    MANA_BURN_1                     = 8129,
    MASS_DISPEL_1                   = 32375,
    MIND_BLAST_1                    = 8092,
    MIND_CONTROL_1                  = 605,
    MIND_FLAY_1                     = 15407,
    MIND_SEAR_1                     = 48045,
    MIND_SOOTHE_1                   = 453,
    MIND_VISION_1                   = 2096,
    PAIN_SUPPRESSION_1              = 33206,
    PENANCE_1                       = 47540,
    POWER_INFUSION_1                = 10060,
    POWER_WORD_FORTITUDE_1          = 1243,
    POWER_WORD_SHIELD_1             = 17,
    PRAYER_OF_FORTITUDE_1           = 21562,
    PRAYER_OF_HEALING_1             = 596,
    PRAYER_OF_MENDING_1             = 33076,
    PRAYER_OF_SHADOW_PROTECTION_1   = 27683,
    PRAYER_OF_SPIRIT_1              = 27681,
    PSYCHIC_HORROR_1                = 64044,
    PSYCHIC_SCREAM_1                = 8122,
    RENEW_1                         = 139,
    RESURRECTION_1                  = 2006,
    SHACKLE_UNDEAD_1                = 9484,
    SHADOW_PROTECTION_1             = 976,
    SHADOW_WORD_DEATH_1             = 32379,
    SHADOW_WORD_PAIN_1              = 589,
    SHADOWFIEND_1                   = 34433,
    SHADOWFORM_1                    = 15473,
    SILENCE_1                       = 15487,
    SMITE_1                         = 585,
    VAMPIRIC_EMBRACE_1              = 15286,
    VAMPIRIC_TOUCH_1                = 34914
};
//class Player;

class MANGOS_DLL_SPEC PlayerbotPriestAI : PlayerbotClassAI
{
public:
    PlayerbotPriestAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotPriestAI();

    // all combat actions go here
    void DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    bool BuffPlayer(Player *target);

private:
    // Heals the target based off its hps
    bool HealTarget (Unit* target);

    // holy
    uint32 BINDING_HEAL,
           CIRCLE_OF_HEALING,
           CLEARCASTING,
           DESPERATE_PRAYER,
           FLASH_HEAL,
           GREATER_HEAL,
           HEAL,
           HOLY_FIRE,
           HOLY_NOVA,
           LESSER_HEAL,
           MANA_BURN,
           PRAYER_OF_HEALING,
           PRAYER_OF_MENDING,
           RENEW,
           RESURRECTION,
           SMITE;

    // shadowmagic
    uint32 FADE,
           SHADOW_WORD_PAIN,
           MIND_BLAST,
           SCREAM,
           MIND_FLAY,
           DEVOURING_PLAGUE,
           SHADOW_PROTECTION,
           VAMPIRIC_TOUCH,
           PRAYER_OF_SHADOW_PROTECTION,
           SHADOWFIEND,
           MIND_SEAR;

    // discipline
    uint32 POWER_WORD_SHIELD,
           INNER_FIRE,
           POWER_WORD_FORTITUDE,
           PRAYER_OF_FORTITUDE,
           FEAR_WARD,
           POWER_INFUSION,
           MASS_DISPEL,
           PENANCE,
           DIVINE_SPIRIT,
           PRAYER_OF_SPIRIT,
           INNER_FOCUS;

    // first aid
    uint32 RECENTLY_BANDAGED;

    // racial
    uint32 ARCANE_TORRENT,
           GIFT_OF_THE_NAARU,
           STONEFORM,
           ESCAPE_ARTIST,
           EVERY_MAN_FOR_HIMSELF,
           SHADOWMELD,
           WAR_STOMP,
           BERSERKING,
           WILL_OF_THE_FORSAKEN;

    uint32 SpellSequence, LastSpellHoly, LastSpellShadowMagic, LastSpellDiscipline;
};

#endif
