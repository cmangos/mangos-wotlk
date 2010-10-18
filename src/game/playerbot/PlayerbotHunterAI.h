#ifndef _PLAYERHUNTERAI_H
#define _PLAYERHUNTERAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HUNTER
};

enum HunterSpells
{
    ARCANE_SHOT_1                   = 3044,
    ASPECT_OF_THE_BEAST_1           = 13161,
    ASPECT_OF_THE_CHEETAH_1         = 5118,
    ASPECT_OF_THE_DRAGONHAWK_1      = 61846,
    ASPECT_OF_THE_HAWK_1            = 13165,
    ASPECT_OF_THE_MONKEY_1          = 13163,
    ASPECT_OF_THE_PACK_1            = 13159,
    ASPECT_OF_THE_VIPER_1           = 34074,
    ASPECT_OF_THE_WILD_1            = 20043,
    AUTO_SHOT_1                     = 75,
    BEAST_LORE_1                    = 1462,
    CALL_PET_1                      = 883,
    CALL_STABLED_PET_1              = 62757,
    CONCUSSIVE_SHOT_1               = 5116,
    DETERRENCE_1                    = 19263,
    DISENGAGE_1                     = 781,
    DISMISS_PET_1                   = 2641,
    DISTRACTING_SHOT_1              = 20736,
    EAGLE_EYE_1                     = 6197,
    EXPLOSIVE_TRAP_1                = 13813,
    EYES_OF_THE_BEAST_1             = 1002,
    FEED_PET_1                      = 6991,
    FEIGN_DEATH_1                   = 5384,
    FLARE_1                         = 1543,
    FREEZING_ARROW_1                = 60192,
    FREEZING_TRAP_1                 = 1499,
    FROST_TRAP_1                    = 13809,
    HUNTERS_MARK_1                  = 1130,
    IMMOLATION_TRAP_1               = 13795,
    KILL_COMMAND_1                  = 34026,
    KILL_SHOT_1                     = 53351,
    MASTERS_CALL_1                  = 53271,
    MEND_PET_1                      = 136,
    MISDIRECTION_1                  = 34477,
    MONGOOSE_BITE_1                 = 1495,
    MULTISHOT_1                     = 2643,
    RAPID_FIRE_1                    = 3045,
    RAPTOR_STRIKE_1                 = 2973,
    REVIVE_PET_1                    = 982,
    SCARE_BEAST_1                   = 1513,
    SCORPID_STING_1                 = 3043,
    SERPENT_STING_1                 = 1978,
    SNAKE_TRAP_1                    = 34600,
    STEADY_SHOT_1                   = 56641,
    TAME_BEAST_1                    = 1515,
    TRACK_BEASTS_1                  = 1494,
    TRACK_DEMONS_1                  = 19878,
    TRACK_DRAGONKIN_1               = 19879,
    TRACK_ELEMENTALS_1              = 19880,
    TRACK_GIANTS_1                  = 19882,
    TRACK_HIDDEN_1                  = 19885,
    TRACK_HUMANOIDS_1               = 19883,
    TRACK_UNDEAD_1                  = 19884,
    TRANQUILIZING_SHOT_1            = 19801,
    VIPER_STING_1                   = 3034,
    VOLLEY_1                        = 1510,
    WING_CLIP_1                     = 2974,
    AIMED_SHOT_1                    = 19434,
    BESTIAL_WRATH_1                 = 19574,
    BLACK_ARROW_1                   = 3674,
    CHIMERA_SHOT_1                  = 53209,
    COUNTERATTACK_1                 = 19306,
    EXPLOSIVE_SHOT_1                = 53301,
    INTIMIDATION_1                  = 19577,
    READINESS_1                     = 23989,
    SCATTER_SHOT_1                  = 19503,
    SILENCING_SHOT_1                = 34490,
    TRUESHOT_AURA_1                 = 19506,
    WYVERN_STING_1                  = 19386
};

//class Player;

class MANGOS_DLL_SPEC PlayerbotHunterAI : PlayerbotClassAI
{
public:
    PlayerbotHunterAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotHunterAI();
    bool HasPet(Player* bot);

    // all combat actions go here
    void DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    //void BuffPlayer(Player *target);

private:
    // Hunter
    bool m_petSummonFailed;
    bool m_rangedCombat;

    uint32 PET_SUMMON, PET_DISMISS, PET_REVIVE, PET_MEND, PET_FEED, BAD_ATTITUDE, SONIC_BLAST, NETHER_SHOCK, DEMORALIZING_SCREECH, INTIMIDATION;
    uint32 AUTO_SHOT, HUNTERS_MARK, ARCANE_SHOT, CONCUSSIVE_SHOT, DISTRACTING_SHOT, MULTI_SHOT, EXPLOSIVE_SHOT, SERPENT_STING, SCORPID_STING, VIPER_STING, WYVERN_STING, AIMED_SHOT, STEADY_SHOT, CHIMERA_SHOT, VOLLEY, BLACK_ARROW, KILL_SHOT;
    uint32 RAPTOR_STRIKE, WING_CLIP, MONGOOSE_BITE, DISENGAGE, DETERRENCE;
    uint32 BEAR_TRAP, FREEZING_TRAP, IMMOLATION_TRAP, FROST_TRAP, EXPLOSIVE_TRAP, ARCANE_TRAP, SNAKE_TRAP;
    uint32 ASPECT_OF_THE_HAWK, ASPECT_OF_THE_MONKEY, RAPID_FIRE, TRUESHOT_AURA, MISDIRECTION;

    // first aid
    uint32 RECENTLY_BANDAGED;

    // racial
    uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;
};

#endif
