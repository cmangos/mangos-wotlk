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
        // CURSES
        uint32 CURSE_OF_WEAKNESS, CURSE_OF_AGONY, CURSE_OF_RECKLESSNESS, CURSE_OF_TONGUES;

        // AFFLICTION
        uint32 CORRUPTION, DRAIN_SOUL, DRAIN_LIFE, SIPHON_LIFE, UNSTABLE_AFFLICTION, HAUNT, ATROCITY, SEED_OF_CORRUPTION;

        // DESTRUCTION
        uint32 SHADOW_BOLT, IMMOLATE, INCINERATE, SEARING_PAIN, CONFLAGRATE, SOUL_FIRE, SHADOWFURY, CHAOS_BOLT;

        // DEMONOLOGY
        uint32 DEMON_SKIN, DEMON_ARMOR, SHADOW_WARD, FEL_ARMOR, SOULSHATTER, SUMMON_IMP, SUMMON_VOIDWALKER, SUMMON_SUCCUBUS, SUMMON_FELHUNTER, SUMMON_FELGUARD;

        uint32 SpellSequence, LastSpellCurse, LastSpellAffliction, LastSpellDestruction;
};

#endif
