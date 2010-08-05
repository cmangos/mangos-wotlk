#ifndef _PLAYERBOTAI_H
#define _PLAYERBOTAI_H

#include "Common.h"
#include "../QuestDef.h"
#include "../GameEventMgr.h"

class WorldPacket;
class WorldObject;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;
class PlayerbotMgr;

#define BOTLOOT_DISTANCE 25.0f

enum RankOneSpellId
{
    // Mage spells and talents
    AMPLIFY_MAGIC_1                 = 1008,
    ARCANE_BARRAGE_1                = 44425,
    ARCANE_BLAST_1                  = 30451,
    ARCANE_BRILLIANCE_1             = 23028,
    ARCANE_EXPLOSION_1              = 1449,
    ARCANE_INTELLECT_1              = 1459,
    ARCANE_MISSILES_1               = 5143,
    ARCANE_POWER_1                  = 12042,
    BLAST_WAVE_1                    = 11113,
    BLINK_1                         = 1953,
    BLIZZARD_1                      = 10,
    COLD_SNAP_1                     = 11958,
    COMBUSTION_1                    = 11129,
    CONE_OF_COLD_1                  = 120,
    CONJURE_FOOD_1                  = 587,
    CONJURE_MANA_GEM_1              = 759,
    CONJURE_REFRESHMENT_1           = 42955,
    CONJURE_WATER_1                 = 5504,
    COUNTERSPELL_1                  = 2139,
    DALARAN_BRILLIANCE_1            = 61316,
    DALARAN_INTELLECT_1             = 61024,
    DAMPEN_MAGIC_1                  = 604,
    DEEP_FREEZE_1                   = 44572,
    DRAGONS_BREATH_1                = 31661,
    EVOCATION_1                     = 12051,
    FIRE_BLAST_1                    = 2136,
    FIRE_WARD_1                     = 543,
    FIREBALL_1                      = 133,
    FLAMESTRIKE_1                   = 2120,
    FOCUS_MAGIC_1                   = 54646,
    FROST_ARMOR_1                   = 168,
    FROST_NOVA_1                    = 122,
    FROST_WARD_1                    = 6143,
    FROSTBOLT_1                     = 116,
    FROSTFIRE_BOLT_1                = 44614,
    ICE_ARMOR_1                     = 7302,
    ICE_BARRIER_1                   = 11426,
    ICE_BLOCK_1                     = 45438,
    ICE_LANCE_1                     = 30455,
    ICY_VEINS_1                     = 12472,
    INVISIBILITY_1                  = 66,
    LIVING_BOMB_1                   = 44457,
    MAGE_ARMOR_1                    = 6117,
    MANA_SHIELD_1                   = 1463,
    MIRROR_IMAGE_1                  = 55342,
    MOLTEN_ARMOR_1                  = 30482,
    PRESENCE_OF_MIND_1              = 12043,
    PYROBLAST_1                     = 11366,
    REMOVE_CURSE_1                  = 475,
    RITUAL_OF_REFRESHMENT_1         = 43987,
    SCORCH_1                        = 2948,
    SLOW_1                          = 31589,
    SLOW_FALL_1                     = 130,
    SPELLSTEAL_1                    = 30449,
    SUMMON_WATER_ELEMENTAL_1        = 31687,

    // Warlock
    BANISH_1                        = 710,
    CHALLENGING_HOWL_1              = 59671,
    CHAOS_BOLT_1                    = 50796,
    CONFLAGRATE_1                   = 17962,
    CORRUPTION_1                    = 172,
    CREATE_FIRESTONE_1              = 6366,
    CREATE_HEALTHSTONE_1            = 6201,
    CREATE_SOULSTONE_1              = 693,
    CREATE_SPELLSTONE_1             = 2362,
    CURSE_OF_AGONY_1                = 980,
    CURSE_OF_DOOM_1                 = 603,
    CURSE_OF_EXHAUSTION_1           = 18223,
    CURSE_OF_THE_ELEMENTS_1         = 1490,
    CURSE_OF_TONGUES_1              = 1714,
    CURSE_OF_WEAKNESS_1             = 702,
    DARK_PACT_1                     = 18220,
    DEATH_COIL_1                    = 6789,
    DEMON_ARMOR_1                   = 706,
    DEMON_CHARGE_1                  = 54785,
    DEMON_SKIN_1                    = 687,
    DEMONIC_CIRCLE_SUMMON_1         = 48018,
    DEMONIC_CIRCLE_TELEPORT_1       = 48020,
    DEMONIC_EMPOWERMENT_1           = 47193,
    DEMONIC_IMMOLATE_1              = 75445,
    DETECT_INVISIBILITY_1           = 132,
    DRAIN_LIFE_1                    = 689,
    DRAIN_MANA_1                    = 5138,
    DRAIN_SOUL_1                    = 1120,
    ENSLAVE_DEMON_1                 = 1098,
    EYE_OF_KILROGG_1                = 126,
    FEAR_1                          = 5782,
    FEL_ARMOR_1                     = 28176,
    FEL_DOMINATION_1                = 18708,
    HAUNT_1                         = 48181,
    HEALTH_FUNNEL_1                 = 755,
    HELLFIRE_1                      = 1949,
    HOWL_OF_TERROR_1                = 5484,
    IMMOLATE_1                      = 348,
    IMMOLATION_AURA_1               = 50589,
    INCINERATE_1                    = 29722,
    INFERNO_1                       = 1122,
    LIFE_TAP_1                      = 1454,
    METAMORPHOSIS_1                 = 59672,
    RAIN_OF_FIRE_1                  = 5740,
    RITUAL_OF_DOOM_1                = 18540,
    RITUAL_OF_SOULS_1               = 29893,
    RITUAL_OF_SUMMONING_1           = 698,
    SEARING_PAIN_1                  = 5676,
    SEED_OF_CORRUPTION_1            = 27243,
    SENSE_DEMONS_1                  = 5500,
    SHADOW_BOLT_1                   = 686,
    SHADOW_CLEAVE_1                 = 50581,
    SHADOW_WARD_1                   = 6229,
    SHADOWBURN_1                    = 17877,
    SHADOWFLAME_1                   = 47897,
    SHADOWFURY_1                    = 30283,
    SOUL_FIRE_1                     = 6353,
    SOUL_LINK_1                     = 19028,
    SOULSHATTER_1                   = 29858,
    SUMMON_FELGUARD_1               = 30146,
    SUMMON_FELHUNTER_1              = 691,
    SUMMON_IMP_1                    = 688,
    SUMMON_SUCCUBUS_1               = 712,
    SUMMON_VOIDWALKER_1             = 697,
    UNENDING_BREATH_1               = 5697,
    UNSTABLE_AFFLICTION_1           = 30108,

    //Paladin
    AURA_MASTERY_1                  = 31821,
    AVENGERS_SHIELD_1               = 31935,
    AVENGING_WRATH_1                = 31884,
    BEACON_OF_LIGHT_1               = 53563,
    BLESSING_OF_KINGS_1             = 20217,
    BLESSING_OF_MIGHT_1             = 19740,
    BLESSING_OF_SANCTUARY_1         = 20911,
    BLESSING_OF_WISDOM_1            = 19742,
    CLEANSE_1                       = 4987,
    CONCENTRATION_AURA_1            = 19746,
    CONSECRATION_1                  = 26573,
    CRUSADER_AURA_1                 = 32223,
    CRUSADER_STRIKE_1               = 35395,
    DEVOTION_AURA_1                 = 465,
    DIVINE_FAVOR_1                  = 20216,
    DIVINE_ILLUMINATION_1           = 31842,
    DIVINE_INTERVENTION_1           = 19752,
    DIVINE_PLEA_1                   = 54428,
    DIVINE_PROTECTION_1             = 498,
    DIVINE_SACRIFICE_1              = 64205,
    DIVINE_SHIELD_1                 = 642,
    DIVINE_STORM_1                  = 53385,
    EXORCISM_1                      = 879,
    FIRE_RESISTANCE_AURA_1          = 19891,
    FLASH_OF_LIGHT_1                = 19750,
    FROST_RESISTANCE_AURA_1         = 19888,
    GREATER_BLESSING_OF_KINGS_1     = 25898,
    GREATER_BLESSING_OF_MIGHT_1     = 25782,
    GREATER_BLESSING_OF_SANCTUARY_1 = 25899,
    GREATER_BLESSING_OF_WISDOM_1    = 25894,
    HAMMER_OF_JUSTICE_1             = 853,
    HAMMER_OF_THE_RIGHTEOUS_1       = 53595,
    HAMMER_OF_WRATH_1               = 24275,
    HAND_OF_FREEDOM_1               = 1044,
    HAND_OF_PROTECTION_1            = 1022,
    HAND_OF_RECKONING_1             = 62124,
    HAND_OF_SACRIFICE_1             = 6940,
    HAND_OF_SALVATION_1             = 1038,
    HOLY_LIGHT_1                    = 635,
    HOLY_SHIELD_1                   = 20925,
    HOLY_SHOCK_1                    = 20473,
    HOLY_WRATH_1                    = 2812,
    JUDGEMENT_OF_JUSTICE_1          = 53407,
    JUDGEMENT_OF_LIGHT_1            = 20271,
    JUDGEMENT_OF_WISDOM_1           = 53408,
    LAY_ON_HANDS_1                  = 633,
    PURIFY_1                        = 1152,
    REDEMPTION_1                    = 7328,
    REPENTANCE_1                    = 20066,
    RETRIBUTION_AURA_1              = 7294,
    RIGHTEOUS_DEFENSE_1             = 31789,
    RIGHTEOUS_FURY_1                = 25780,
    SACRED_SHIELD_1                 = 53601,
    SEAL_OF_COMMAND_1               = 20375,
    SEAL_OF_CORRUPTION              = 53736,
    SEAL_OF_JUSTICE_1               = 20164,
    SEAL_OF_LIGHT_1                 = 20165,
    SEAL_OF_RIGHTEOUSNESS_1         = 20154,
    SEAL_OF_VENGEANCE               = 31801,
    SEAL_OF_WISDOM_1                = 20166,
    SENSE_UNDEAD_1                  = 5502,
    SHADOW_RESISTANCE_AURA_1        = 19876,
    SHIELD_OF_RIGHTEOUSNESS_1       = 53600,
    TURN_EVIL_1                     = 10326,

    //Priest
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
    VAMPIRIC_TOUCH_1                = 34914,

    //Druid
    ABOLISH_POISON_1                = 2893,
    AQUATIC_FORM_1                  = 1066,
    BARKSKIN_1                      = 22812,
    BASH_1                          = 5211,
    BEAR_FORM_1                     = 5487,
    BERSERK_1                       = 50334,
    CAT_FORM_1                      = 768,
    CHALLENGING_ROAR_1              = 5209,
    CLAW_1                          = 1082,
    COWER_1                         = 8998,
    CURE_POISON_1                   = 8946,
    CYCLONE_1                       = 33786,
    DASH_1                          = 1850,
    DEMORALIZING_ROAR_1             = 99,
    DIRE_BEAR_FORM_1                = 9634,
    ENRAGE_1                        = 5229,
    ENTANGLING_ROOTS_1              = 339,
    FAERIE_FIRE_1                   = 770,
    FAERIE_FIRE_FERAL_1             = 16857,
    FERAL_CHARGE_1                  = 49377,
    FERAL_CHARGE_BEAR_1             = 16979,
    FERAL_CHARGE_CAT_1              = 49376,
    FEROCIOUS_BITE_1                = 22568,
    FLIGHT_FORM_1                   = 33943,
    FORCE_OF_NATURE_1               = 33831,
    FRENZIED_REGENERATION_1         = 22842,
    GIFT_OF_THE_WILD_1              = 21849,
    GROWL_1                         = 6795,
    HEALING_TOUCH_1                 = 5185,
    HIBERNATE_1                     = 2637,
    HURRICANE_1                     = 16914,
    INNERVATE_1                     = 29166,
    INSECT_SWARM_1                  = 5570,
    LACERATE_1                      = 33745,
    LIFEBLOOM_1                     = 33763,
    MAIM_1                          = 22570,
    MANGLE_1                        = 33917,
    MANGLE_BEAR_1                   = 33878,
    MANGLE_CAT_1                    = 33876,
    MARK_OF_THE_WILD_1              = 1126,
    MAUL_1                          = 6807,
    MOONFIRE_1                      = 8921,
    MOONKIN_FORM_1                  = 24858,
    NATURES_GRASP_1                 = 16689,
    NATURES_SWIFTNESS_1             = 17116,
    NOURISH_1                       = 50464,
    POUNCE_1                        = 9005,
    PROWL_1                         = 5215,
    RAKE_1                          = 1822,
    RAVAGE_1                        = 6785,
    REBIRTH_1                       = 20484,
    REGROWTH_1                      = 8936,
    REJUVENATION_1                  = 774,
    REMOVE_CURSE_1                  = 2782,
    REVIVE_1                        = 50769,
    RIP_1                           = 1079,
    SAVAGE_ROAR_1                   = 52610,
    SHRED_1                         = 5221,
    SOOTHE_ANIMAL_1                 = 2908,
    STARFALL_1                      = 48505,
    STARFIRE_1                      = 2912,
    SURVIVAL_INSTINCTS_1            = 61336,
    SWIFTMEND_1                     = 18562,
    SWIFT_FLIGHT_FORM_1             = 40120,
    SWIPE_BEAR_1                    = 779,
    SWIPE_CAT_1                     = 62078,
    THORNS_1                        = 467,
    TIGERS_FURY_1                   = 5217,
    TRANQUILITY_1                   = 740,
    TRAVEL_FORM_1                   = 783,
    TREE_OF_LIFE_1                  = 33891,
    TYPHOON_1                       = 50516,
    WILD_GROWTH_1                   = 48438,
    WRATH_1                         = 5176,

    //Hunter
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
    WYVERN_STING_1                  = 19386,

    //Death Knight
    ANTIMAGIC_SHELL_1               = 48707,
    ANTIMAGIC_ZONE_1                = 51052,
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
    DEATH_COIL_1                    = 47541,
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
    VAMPIRIC_BLOOD_1                = 55233,

    //Rogue
    ADRENALINE_RUSH_1               = 13750,
    AMBUSH_1                        = 8676,
    BACKSTAB_1                      = 53,
    BLADE_FLURRY_1                  = 13877,
    BLIND_1                         = 2094,
    CHEAP_SHOT_1                    = 1833,
    CLOAK_OF_SHADOWS_1              = 31224,
    COLD_BLOOD_1                    = 14177,
    DEADLY_THROW_1                  = 26679,
    DISARM_TRAP_1                   = 1842,
    DISMANTLE_1                     = 51722,
    DISTRACT_1                      = 1725,
    ENVENOM_1                       = 32645,
    EVASION_1                       = 5277,
    EVISCERATE_1                    = 2098,
    EXPOSE_ARMOR_1                  = 8647,
    FAN_OF_KNIVES_1                 = 51723,
    FEINT_1                         = 1966,
    GARROTE_1                       = 703,
    GHOSTLY_STRIKE_1                = 14278,
    GOUGE_1                         = 1776,
    HEMORRHAGE_1                    = 16511,
    HUNGER_FOR_BLOOD_1              = 51662,
    KICK_1                          = 1766,
    KIDNEY_SHOT_1                   = 408,
    KILLING_SPREE_1                 = 51690,
    MUTILATE_1                      = 1329,
    PICK_LOCK_1                     = 1804,
    PICK_POCKET_1                   = 921,
    PREMEDITATION_1                 = 14183,
    PREPARATION_1                   = 14185,
    RIPOSTE_1                       = 14251,
    RUPTURE_1                       = 1943,
    SAP_1                           = 6770,
    SHADOW_DANCE_1                  = 51713,
    SHADOWSTEP_1                    = 36554,
    SHIV_1                          = 5938,
    SINISTER_STRIKE_1               = 1752,
    SLICE_AND_DICE_1                = 5171,
    SPRINT_1                        = 2983,
    STEALTH_1                       = 1784,
    TRICKS_OF_THE_TRADE_1           = 57934,
    VANISH_1                        = 1856,

    //Shaman
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
    NATURES_SWIFTNESS_1             = 16188,
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
    WRATH_OF_AIR_TOTEM_1            = 3738,

    //Warrior
    BATTLE_SHOUT_1                  = 6673,
    BATTLE_STANCE_1                 = 2457,
    BERSERKER_RAGE_1                = 18499,
    BERSERKER_STANCE_1              = 2458,
    BLADESTORM_1                    = 46924,
    BLOODRAGE_1                     = 2687,
    BLOODTHIRST_1                   = 23881,
    CHALLENGING_SHOUT_1             = 1161,
    CHARGE_1                        = 100,
    CLEAVE_1                        = 845,
    COMMANDING_SHOUT_1              = 469,
    CONCUSSION_BLOW_1               = 12809,
    DEATH_WISH_1                    = 12292,
    DEFENSIVE_STANCE_1              = 71,
    DEMORALIZING_SHOUT_1            = 1160,
    DEVASTATE_1                     = 20243,
    DISARM_1                        = 676,
    ENRAGED_REGENERATION_1          = 55694,
    EXECUTE_1                       = 5308,
    HAMSTRING_1                     = 1715,
    HEROIC_FURY_1                   = 60970,
    HEROIC_STRIKE_1                 = 78,
    HEROIC_THROW_1                  = 57755,
    INTERCEPT_1                     = 20252,
    INTERVENE_1                     = 3411,
    INTIMIDATING_SHOUT_1            = 5246,
    LAST_STAND_1                    = 12975,
    MOCKING_BLOW_1                  = 694,
    MORTAL_STRIKE_1                 = 12294,
    OVERPOWER_1                     = 7384,
    PIERCING_HOWL_1                 = 12323,
    PUMMEL_1                        = 6552,
    RECKLESSNESS_1                  = 1719,
    REND_1                          = 772,
    RETALIATION_1                   = 20230,
    REVENGE_1                       = 6572,
    SHATTERING_THROW_1              = 64382,
    SHIELD_BASH_1                   = 72,
    SHIELD_BLOCK_1                  = 2565,
    SHIELD_SLAM_1                   = 23922,
    SHIELD_WALL_1                   = 871,
    SHOCKWAVE_1                     = 46968,
    SLAM_1                          = 1464,
    SPELL_REFLECTION_1              = 23920,
    SUNDER_ARMOR_1                  = 7386,
    SWEEPING_STRIKES_1              = 12328,
    TAUNT_1                         = 355,
    THUNDER_CLAP_1                  = 6343,
    VICTORY_RUSH_1                  = 34428,
    VIGILANCE_1                     = 50720,
    WHIRLWIND_1                     = 1680
};

class MANGOS_DLL_SPEC PlayerbotAI
{
    public:
        enum ScenarioType
        {
            SCENARIO_PVEEASY,
            SCENARIO_PVEHARD,
            SCENARIO_DUEL,
            SCENARIO_PVPEASY,
            SCENARIO_PVPHARD
        };

        enum CombatStyle {
            COMBAT_MELEE                = 0x01,         // class melee attacker
            COMBAT_RANGED               = 0x02          // class is ranged attacker
        };

        // masters orders that should be obeyed by the AI during the updteAI routine
        // the master will auto set the target of the bot
        enum CombatOrderType
        {
            ORDERS_NONE                 = 0x00,         // no special orders given
            ORDERS_TANK                 = 0x01,         // bind attackers by gaining threat
            ORDERS_ASSIST               = 0x02,         // assist someone (dps type)
            ORDERS_HEAL                 = 0x04,         // concentrate on healing (no attacks, only self defense)
            ORDERS_PROTECT              = 0x10,         // combinable state: check if protectee is attacked
            ORDERS_PRIMARY              = 0x0F,
            ORDERS_SECONDARY            = 0xF0,
            ORDERS_RESET                = 0xFF
        };

        enum CombatTargetType
        {
            TARGET_NORMAL               = 0x00,
            TARGET_THREATEN             = 0x01
        };

        enum BotState
        {
            BOTSTATE_NORMAL,        // normal AI routines are processed
            BOTSTATE_COMBAT,        // bot is in combat
            BOTSTATE_DEAD,          // we are dead and wait for becoming ghost
            BOTSTATE_DEADRELEASED,  // we released as ghost and wait to revive
            BOTSTATE_LOOTING        // looting mode, used just after combat
        };

        enum MovementOrderType
        {
            MOVEMENT_NONE               = 0x00,
            MOVEMENT_FOLLOW             = 0x01,
            MOVEMENT_STAY               = 0x02
        };

        typedef std::map<uint32, uint32> BotNeedItem;
        typedef std::list<uint64> BotLootCreature;

        // attacker query used in PlayerbotAI::FindAttacker()
        enum ATTACKERINFOTYPE
        {
            AIT_NONE                    = 0x00,
            AIT_LOWESTTHREAT            = 0x01,
            AIT_HIGHESTTHREAT           = 0x02,
            AIT_VICTIMSELF              = 0x04,
            AIT_VICTIMNOTSELF           = 0x08	// !!! must use victim param in FindAttackers
        };
        struct AttackerInfo
        {
            Unit*    attacker;        // reference to the attacker
            Unit*    victim;          // combatant's current victim
            float    threat;          // own threat on this combatant
            float    threat2;         // highest threat not caused by bot
            uint32   count;           // number of units attacking
            uint32   source;          // 1=bot, 2=master, 3=group
        };
        typedef std::map<uint64,AttackerInfo> AttackerInfoList;

    public:
        PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot);
        virtual ~PlayerbotAI();

        // This is called from Unit.cpp and is called every second (I think)
        void UpdateAI(const uint32 p_time);

        // This is called from ChatHandler.cpp when there is an incoming message to the bot
        // from a whisper or from the party channel
        void HandleCommand(const std::string& text, Player& fromPlayer);

        // This is called by WorldSession.cpp
        // It provides a view of packets normally sent to the client.
        // Since there is no client at the other end, the packets are dropped of course.
        // For a list of opcodes that can be caught see Opcodes.cpp (SMSG_* opcodes only)
        void HandleBotOutgoingPacket(const WorldPacket& packet);

        // This is called by WorldSession.cpp
        // when it detects that a bot is being teleported. It acknowledges to the server to complete the
        // teleportation
        void HandleTeleportAck();

        // Returns what kind of situation we are in so the ai can react accordingly
        ScenarioType GetScenarioType() {return m_ScenarioType;}

        PlayerbotClassAI* GetClassAI() {return m_classAI;}
        PlayerbotMgr* const GetManager() {return m_mgr;}

        // finds spell ID for matching substring args
        // in priority of full text match, spells not taking reagents, and highest rank
        uint32 getSpellId(const char* args, bool master = false) const;
        uint32 getPetSpellId(const char* args) const;
        uint32 getMaxKnownRankSpellId(uint32 spellId);

        // extracts item ids from links
        void extractItemIds(const std::string& text, std::list<uint32>& itemIds) const;

        // extract spellid from links
        bool extractSpellId(const std::string& text, uint32 &spellId) const;

        // extracts currency from a string as #g#s#c and returns the total in copper
        uint32 extractMoney(const std::string& text) const;

        // extracts gameobject info from link
        bool extractGOinfo(const std::string& text, uint32 &guid,  uint32 &entry, int &mapid, float &x, float &y, float &z) const;

        // finds items in bots equipment and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
        // finds items in bots inventory and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;

        // currently bots only obey commands from the master
        bool canObeyCommandFrom(const Player& player) const;

        // get current casting spell (will return NULL if no spell!)
        Spell* GetCurrentSpell() const;

        bool HasAura(uint32 spellId, const Unit& player) const;
        bool HasAura(const char* spellName, const Unit& player) const;
        bool HasAura(const char* spellName) const;

        bool HasPick();

        uint8 GetHealthPercent(const Unit& target) const;
        uint8 GetHealthPercent() const;
        uint8 GetBaseManaPercent(const Unit& target) const;
        uint8 GetBaseManaPercent() const;
        uint8 GetManaPercent(const Unit& target) const;
        uint8 GetManaPercent() const;
        uint8 GetRageAmount(const Unit& target) const;
        uint8 GetRageAmount() const;
        uint8 GetEnergyAmount(const Unit& target) const;
        uint8 GetEnergyAmount() const;
        uint8 GetRunicPower(const Unit& target) const;
        uint8 GetRunicPower() const;

        Item* FindFood() const;
        Item* FindDrink() const;
        Item* FindBandage() const;
        Item* FindPoison() const;
        Item* FindMount(uint32 matchingRidingSkill) const;
        Item* FindItem(uint32 ItemId);

        // ******* Actions ****************************************
        // Your handlers can call these actions to make the bot do things.
        void TellMaster(const std::string& text) const;
        void TellMaster( const char *fmt, ... ) const;
        void SendWhisper(const std::string& text, Player& player) const;
        bool CastSpell(const char* args);
        bool CastSpell(uint32 spellId);
        bool CastSpell(uint32 spellId, Unit& target);
        bool CastPetSpell(uint32 spellId, Unit* target = NULL);
        void UseItem(Item& item);
        void EquipItem(Item& item);
        //void Stay();
        //bool Follow(Player& player);
        void SendNotEquipList(Player& player);
        void Feast();
        void InterruptCurrentCastingSpell();
        void GetCombatTarget( Unit* forcedTarged = 0 );
        Unit *GetCurrentTarget() { return m_targetCombat; };
        void DoNextCombatManeuver();
        void DoCombatMovement();
        void SetIgnoreUpdateTime(uint8 t) {m_ignoreAIUpdatesUntilTime=time(0) + t; };

        Player *GetPlayerBot() const {return m_bot;}
        Player *GetPlayer() const {return m_bot;}
        Player *GetMaster() const;

        BotState GetState() { return m_botState; };
        void SetState( BotState state );
        void SetQuestNeedItems();
        void SendQuestItemList( Player& player );
        void SendOrders( Player& player );
        bool FollowCheckTeleport( WorldObject &obj );
        void DoLoot();

        uint32 EstRepairAll();
        uint32 EstRepair(uint16 pos);

        void AcceptQuest( Quest const *qInfo, Player *pGiver );
        void TurnInQuests( WorldObject *questgiver );

        bool IsInCombat();
        void UpdateAttackerInfo();
        Unit* FindAttacker( ATTACKERINFOTYPE ait=AIT_NONE, Unit *victim=0 );
        uint32 GetAttackerCount() { return m_attackerInfo.size(); };
        void SetCombatOrderByStr( std::string str, Unit *target=0 );
        void SetCombatOrder( CombatOrderType co, Unit *target=0 );
        CombatOrderType GetCombatOrder() { return this->m_combatOrder; }
        void SetMovementOrder( MovementOrderType mo, Unit *followTarget=0 );
        MovementOrderType GetMovementOrder() { return this->m_movementOrder; }
        void MovementReset();
        void MovementUpdate();
        void MovementClear();
        bool IsMoving();

        void SetInFront( const Unit* obj );

        void ItemLocalization(std::string& itemName, const uint32 itemID) const;
        void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    private:
        // ****** Closed Actions ********************************
        // These actions may only be called at special times.
        // Trade methods are only applicable when the trade window is open
        // and are only called from within HandleCommand.
        bool TradeItem(const Item& item, int8 slot=-1);
        bool TradeCopper(uint32 copper);

        // it is safe to keep these back reference pointers because m_bot
        // owns the "this" object and m_master owns m_bot. The owner always cleans up.
        PlayerbotMgr* const m_mgr;
        Player* const m_bot;
        PlayerbotClassAI* m_classAI;

        // ignores AI updates until time specified
        // no need to waste CPU cycles during casting etc
        time_t m_ignoreAIUpdatesUntilTime;

        CombatStyle m_combatStyle;
        CombatOrderType m_combatOrder;
        MovementOrderType m_movementOrder;

        ScenarioType m_ScenarioType;

        // defines the state of behaviour of the bot
        BotState m_botState;

        // list of items needed to fullfill quests
        BotNeedItem m_needItemList;

        // list of creatures we recently attacked and want to loot
        BotLootCreature m_lootCreature;  // list of creatures
        uint64 m_lootCurrent;            // current remains of interest

        time_t m_TimeDoneEating;
        time_t m_TimeDoneDrinking;
        uint32 m_CurrentlyCastingSpellId;
        //bool m_IsFollowingMaster;

        // if master commands bot to do something, store here until updateAI
        // can do it
        uint32 m_spellIdCommand;
        uint64 m_targetGuidCommand;

        AttackerInfoList m_attackerInfo;

        bool m_targetChanged;
        CombatTargetType m_targetType;

        Unit *m_targetCombat;	// current combat target
        Unit *m_targetAssist;	// get new target by checking attacker list of assisted player
        Unit *m_targetProtect;	// check

        Unit *m_followTarget;	// whom to follow in non combat situation?
};

#endif
