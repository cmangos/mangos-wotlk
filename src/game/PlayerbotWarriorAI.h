#ifndef _PlayerbotWarriorAI_H
#define _PlayerbotWarriorAI_H

#include "PlayerbotClassAI.h"

enum
{
    WarriorSpellPreventing,
    WarriorBattle,
    WarriorDefensive,
    WarriorBerserker
};


//class Player;

class MANGOS_DLL_SPEC PlayerbotWarriorAI : PlayerbotClassAI
{
    public:
        PlayerbotWarriorAI(Player* const master, Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotWarriorAI();

        // all combat actions go here
        bool DoFirstCombatManeuver(Unit*);
        void DoNextCombatManeuver(Unit*);

        // all non combat actions go here, ex buffs, heals, rezzes
        void DoNonCombatActions();

        // buff a specific player, usually a real PC who is not in group
        void BuffPlayer(Player *target);

    private:
        // ARMS
        uint32 BATTLE_STANCE, CHARGE, HEROIC_STRIKE, REND, THUNDER_CLAP, HAMSTRING, MOCKING_BLOW, RETALIATION, SWEEPING_STRIKES, MORTAL_STRIKE, BLADESTORM, HEROIC_THROW, SHATTERING_THROW;
        
        // PROTECTION
        uint32 DEFENSIVE_STANCE, BLOODRAGE, SUNDER_ARMOR, TAUNT, SHIELD_BASH, REVENGE, SHIELD_BLOCK, DISARM, SHIELD_WALL, SHIELD_SLAM, VIGILANCE, DEVASTATE, SHOCKWAVE, CONCUSSION_BLOW, SPELL_REFLECTION, LAST_STAND;

        // FURY
        uint32 BERSERKER_STANCE, BATTLE_SHOUT, DEMORALIZING_SHOUT, OVERPOWER, CLEAVE, INTIMIDATING_SHOUT, EXECUTE, CHALLENGING_SHOUT, SLAM, INTERCEPT, DEATH_WISH, BERSERKER_RAGE, WHIRLWIND, PUMMEL, BLOODTHIRST, RECKLESSNESS, RAMPAGE, HEROIC_FURY, COMMANDING_SHOUT, ENRAGED_REGENERATION, PIERCING_HOWL;

		// first aid
		uint32 RECENTLY_BANDAGED;

		// racial
		uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence;
};

#endif
