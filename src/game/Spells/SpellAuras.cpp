/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Server/WorldPacket.h"
#include "Server/WorldSession.h"
#include "Server/Opcodes.h"
#include "Log.h"
#include "World/World.h"
#include "Globals/ObjectMgr.h"
#include "Spells/SpellMgr.h"
#include "Entities/Player.h"
#include "Entities/Unit.h"
#include "Spells/Spell.h"
#include "Entities/DynamicObject.h"
#include "Groups/Group.h"
#include "Entities/UpdateData.h"
#include "Entities/UpdateMask.h"
#include "Globals/ObjectAccessor.h"
#include "Policies/Singleton.h"
#include "Entities/Totem.h"
#include "Entities/Creature.h"
#include "BattleGround/BattleGround.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "AI/BaseAI/CreatureAI.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Util/Util.h"
#include "Grids/GridNotifiers.h"
#include "Grids/GridNotifiersImpl.h"
#include "Entities/Vehicle.h"
#include "Grids/CellImpl.h"
#include "Tools/Language.h"
#include "Maps/MapManager.h"
#include "Loot/LootMgr.h"
#include "Entities/TemporarySpawn.h"
#include "Maps/InstanceData.h"
#include "AI/ScriptDevAI/include/sc_grid_searchers.h"

#define NULL_AURA_SLOT 0xFF

/**
 * An array with all the different handlers for taking care of
 * the various aura types that are defined in AuraType.
 */
pAuraHandler AuraHandler[TOTAL_AURAS] =
{
    &Aura::HandleNULL,                                      //  0 SPELL_AURA_NONE
    &Aura::HandleBindSight,                                 //  1 SPELL_AURA_BIND_SIGHT
    &Aura::HandleModPossess,                                //  2 SPELL_AURA_MOD_POSSESS
    &Aura::HandlePeriodicDamage,                            //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Aura::HandleAuraDummy,                                 //  4 SPELL_AURA_DUMMY
    &Aura::HandleModConfuse,                                //  5 SPELL_AURA_MOD_CONFUSE
    &Aura::HandleModCharm,                                  //  6 SPELL_AURA_MOD_CHARM
    &Aura::HandleModFear,                                   //  7 SPELL_AURA_MOD_FEAR
    &Aura::HandlePeriodicHeal,                              //  8 SPELL_AURA_PERIODIC_HEAL
    &Aura::HandleModAttackSpeed,                            //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Aura::HandleModThreat,                                 // 10 SPELL_AURA_MOD_THREAT
    &Aura::HandleModTaunt,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Aura::HandleAuraModStun,                               // 12 SPELL_AURA_MOD_STUN
    &Aura::HandleModDamageDone,                             // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Aura::HandleNoImmediateEffect,                         // 14 SPELL_AURA_MOD_DAMAGE_TAKEN   implemented in Unit::MeleeDamageBonusTaken and Unit::SpellBaseDamageBonusTaken
    &Aura::HandleAuraDamageShield,                          // 15 SPELL_AURA_DAMAGE_SHIELD      implemented in Unit::DealMeleeDamage
    &Aura::HandleModStealth,                                // 16 SPELL_AURA_MOD_STEALTH
    &Aura::HandleModStealthDetect,                          // 17 SPELL_AURA_MOD_STEALTH_DETECT
    &Aura::HandleInvisibility,                              // 18 SPELL_AURA_MOD_INVISIBILITY
    &Aura::HandleInvisibilityDetect,                        // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Aura::HandleAuraModTotalHealthPercentRegen,            // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Aura::HandleAuraModTotalManaPercentRegen,              // 21 SPELL_AURA_OBS_MOD_MANA
    &Aura::HandleAuraModResistance,                         // 22 SPELL_AURA_MOD_RESISTANCE
    &Aura::HandlePeriodicTriggerSpell,                      // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Aura::HandlePeriodicEnergize,                          // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Aura::HandleAuraModPacify,                             // 25 SPELL_AURA_MOD_PACIFY
    &Aura::HandleAuraModRoot,                               // 26 SPELL_AURA_MOD_ROOT
    &Aura::HandleAuraModSilence,                            // 27 SPELL_AURA_MOD_SILENCE
    &Aura::HandleNoImmediateEffect,                         // 28 SPELL_AURA_REFLECT_SPELLS        implement in Unit::SpellHitResult
    &Aura::HandleAuraModStat,                               // 29 SPELL_AURA_MOD_STAT
    &Aura::HandleAuraModSkill,                              // 30 SPELL_AURA_MOD_SKILL
    &Aura::HandleAuraModIncreaseSpeed,                      // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Aura::HandleAuraModIncreaseMountedSpeed,               // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Aura::HandleAuraModDecreaseSpeed,                      // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Aura::HandleAuraModIncreaseHealth,                     // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Aura::HandleAuraModIncreaseEnergy,                     // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Aura::HandleAuraModShapeshift,                         // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Aura::HandleAuraModEffectImmunity,                     // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Aura::HandleAuraModStateImmunity,                      // 38 SPELL_AURA_STATE_IMMUNITY
    &Aura::HandleAuraModSchoolImmunity,                     // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Aura::HandleAuraModDmgImmunity,                        // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Aura::HandleAuraModDispelImmunity,                     // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Aura::HandleAuraProcTriggerSpell,                      // 42 SPELL_AURA_PROC_TRIGGER_SPELL  implemented in Unit::ProcDamageAndSpellFor and Unit::HandleProcTriggerSpell
    &Aura::HandleNoImmediateEffect,                         // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE implemented in Unit::ProcDamageAndSpellFor
    &Aura::HandleAuraTrackCreatures,                        // 44 SPELL_AURA_TRACK_CREATURES
    &Aura::HandleAuraTrackResources,                        // 45 SPELL_AURA_TRACK_RESOURCES
    &Aura::HandleUnused,                                    // 46 SPELL_AURA_46 (used in test spells 54054 and 54058, and spell 48050) (3.0.8a-3.2.2a)
    &Aura::HandleAuraModParryPercent,                       // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Aura::HandleNoImmediateEffect,                         // 48 SPELL_AURA_PERIODIC_TRIGGER_BY_CLIENT (Client periodic trigger spell by self (3 spells in 3.3.5a)). Implemented in pet/player cast chains.
    &Aura::HandleAuraModDodgePercent,                       // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Aura::HandleNoImmediateEffect,                         // 50 SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT implemented in Unit::SpellCriticalHealingBonus
    &Aura::HandleAuraModBlockPercent,                       // 51 SPELL_AURA_MOD_BLOCK_PERCENT
    &Aura::HandleAuraModCritPercent,                        // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Aura::HandlePeriodicLeech,                             // 53 SPELL_AURA_PERIODIC_LEECH
    &Aura::HandleModHitChance,                              // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Aura::HandleModSpellHitChance,                         // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Aura::HandleAuraTransform,                             // 56 SPELL_AURA_TRANSFORM
    &Aura::HandleModSpellCritChance,                        // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Aura::HandleAuraModIncreaseSwimSpeed,                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Aura::HandleNoImmediateEffect,                         // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE implemented in Unit::MeleeDamageBonusDone and Unit::SpellDamageBonusDone
    &Aura::HandleAuraModPacifyAndSilence,                   // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Aura::HandleAuraModScale,                              // 61 SPELL_AURA_MOD_SCALE
    &Aura::HandlePeriodicHealthFunnel,                      // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Aura::HandleUnused,                                    // 63 unused (3.0.8a-3.2.2a) old SPELL_AURA_PERIODIC_MANA_FUNNEL
    &Aura::HandlePeriodicManaLeech,                         // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Aura::HandleModCastingSpeed,                           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &Aura::HandleFeignDeath,                                // 66 SPELL_AURA_FEIGN_DEATH
    &Aura::HandleAuraModDisarm,                             // 67 SPELL_AURA_MOD_DISARM
    &Aura::HandleAuraModStalked,                            // 68 SPELL_AURA_MOD_STALKED
    &Aura::HandleSchoolAbsorb,                              // 69 SPELL_AURA_SCHOOL_ABSORB implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleUnused,                                    // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell 41560 that has only visual effect (3.2.2a)
    &Aura::HandleModSpellCritChanceShool,                   // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Aura::HandleModPowerCostPCT,                           // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Aura::HandleModPowerCost,                              // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Aura::HandleNoImmediateEffect,                         // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL  implemented in Unit::SpellHitResult
    &Aura::HandleNoImmediateEffect,                         // 75 SPELL_AURA_MOD_LANGUAGE           implemented in WorldSession::HandleMessagechatOpcode
    &Aura::HandleFarSight,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Aura::HandleModMechanicImmunity,                       // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Aura::HandleAuraMounted,                               // 78 SPELL_AURA_MOUNTED
    &Aura::HandleModDamagePercentDone,                      // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Aura::HandleModPercentStat,                            // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Aura::HandleNoImmediateEffect,                         // 81 SPELL_AURA_SPLIT_DAMAGE_PCT       implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleWaterBreathing,                            // 82 SPELL_AURA_WATER_BREATHING
    &Aura::HandleModBaseResistance,                         // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Aura::HandleModRegen,                                  // 84 SPELL_AURA_MOD_REGEN
    &Aura::HandleModPowerRegen,                             // 85 SPELL_AURA_MOD_POWER_REGEN
    &Aura::HandleChannelDeathItem,                          // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Aura::HandleDamagePercentTaken,                        // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN implemented in Unit::MeleeDamageBonusTaken and Unit::SpellDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT implemented in Player::RegenerateHealth
    &Aura::HandlePeriodicDamagePCT,                         // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Aura::HandleUnused,                                    // 90 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_RESIST_CHANCE
    &Aura::HandleNoImmediateEffect,                         // 91 SPELL_AURA_MOD_DETECT_RANGE implemented in Creature::GetAttackDistance
    &Aura::HandlePreventFleeing,                            // 92 SPELL_AURA_PREVENTS_FLEEING
    &Aura::HandleModUnattackable,                           // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Aura::HandleNoImmediateEffect,                         // 94 SPELL_AURA_INTERRUPT_REGEN implemented in Player::RegenerateAll
    &Aura::HandleAuraGhost,                                 // 95 SPELL_AURA_GHOST
    &Aura::HandleNoImmediateEffect,                         // 96 SPELL_AURA_SPELL_MAGNET implemented in Unit::SelectMagnetTarget
    &Aura::HandleManaShield,                                // 97 SPELL_AURA_MANA_SHIELD implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleAuraModSkill,                              // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Aura::HandleAuraModAttackPower,                        // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Aura::HandleUnused,                                    //100 SPELL_AURA_AURAS_VISIBLE obsolete 3.x? all player can see all auras now, but still have 2 spells including GM-spell (1852,2855)
    &Aura::HandleModResistancePercent,                      //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Aura::HandleNoImmediateEffect,                         //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModTotalThreat,                        //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Aura::HandleAuraWaterWalk,                             //104 SPELL_AURA_WATER_WALK
    &Aura::HandleAuraFeatherFall,                           //105 SPELL_AURA_FEATHER_FALL
    &Aura::HandleAuraHover,                                 //106 SPELL_AURA_HOVER
    &Aura::HandleAddModifier,                               //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Aura::HandleAddModifier,                               //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Aura::HandleNoImmediateEffect,                         //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Aura::HandleModPowerRegenPCT,                          //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Aura::HandleNoImmediateEffect,                         //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER implemented in Unit::SelectMagnetTarget
    &Aura::HandleOverrideClassScript,                       //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS implemented in diff functions.
    &Aura::HandleNoImmediateEffect,                         //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //115 SPELL_AURA_MOD_HEALING                 implemented in Unit::SpellBaseHealingBonusTaken
    &Aura::HandleNoImmediateEffect,                         //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT     imppemented in Player::RegenerateAll and Player::RegenerateHealth
    &Aura::HandleNoImmediateEffect,                         //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE     implemented in Unit::MagicSpellHitResult
    &Aura::HandleModHealingPCT,                             //118 SPELL_AURA_MOD_HEALING_PCT             implemented in Unit::SpellHealingBonusTaken
    &Aura::HandleUnused,                                    //119 unused (3.0.8a-3.2.2a) old SPELL_AURA_SHARE_PET_TRACKING
    &Aura::HandleAuraUntrackable,                           //120 SPELL_AURA_UNTRACKABLE
    &Aura::HandleAuraEmpathy,                               //121 SPELL_AURA_EMPATHY
    &Aura::HandleModOffhandDamagePercent,                   //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Aura::HandleModTargetResistance,                       //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Aura::HandleAuraModRangedAttackPower,                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Aura::HandleNoImmediateEffect,                         //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleModPossessPet,                             //128 SPELL_AURA_MOD_POSSESS_PET
    &Aura::HandleAuraModIncreaseSpeed,                      //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Aura::HandleAuraModIncreaseMountedSpeed,               //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Aura::HandleNoImmediateEffect,                         //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModIncreaseEnergyPercent,              //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Aura::HandleAuraModIncreaseHealthPercent,              //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Aura::HandleAuraModRegenInterrupt,                     //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Aura::HandleModHealingDone,                            //135 SPELL_AURA_MOD_HEALING_DONE
    &Aura::HandleNoImmediateEffect,                         //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT   implemented in Unit::SpellHealingBonusDone
    &Aura::HandleModTotalPercentStat,                       //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Aura::HandleModMeleeSpeedPct,                          //138 SPELL_AURA_MOD_MELEE_HASTE
    &Aura::HandleForceReaction,                             //139 SPELL_AURA_FORCE_REACTION
    &Aura::HandleAuraModRangedHaste,                        //140 SPELL_AURA_MOD_RANGED_HASTE
    &Aura::HandleRangedAmmoHaste,                           //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Aura::HandleAuraModBaseResistancePercent,              //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Aura::HandleAuraModResistanceExclusive,                //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Aura::HandleAuraSafeFall,                              //144 SPELL_AURA_SAFE_FALL                  implemented in WorldSession::HandleMovementOpcodes
    &Aura::HandleAuraModPetTalentsPoints,                   //145 SPELL_AURA_MOD_PET_TALENT_POINTS
    &Aura::HandleNoImmediateEffect,                         //146 SPELL_AURA_ALLOW_TAME_PET_TYPE        implemented in Player::CanTameExoticPets
    &Aura::HandleModMechanicImmunityMask,                   //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK     implemented in Unit::IsImmuneToSpell and Unit::IsImmuneToSpellEffect (check part)
    &Aura::HandleAuraRetainComboPoints,                     //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Aura::HandleNoImmediateEffect,                         //149 SPELL_AURA_REDUCE_PUSHBACK            implemented in Spell::Delayed and Spell::DelayedChannel
    &Aura::HandleShieldBlockValue,                          //150 SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT
    &Aura::HandleAuraTrackStealthed,                        //151 SPELL_AURA_TRACK_STEALTHED
    &Aura::HandleNoImmediateEffect,                         //152 SPELL_AURA_MOD_DETECTED_RANGE         implemented in Creature::GetAttackDistance
    &Aura::HandleNoImmediateEffect,                         //153 SPELL_AURA_SPLIT_DAMAGE_FLAT          implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleModStealthLevel,                           //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &Aura::HandleModWaterBreathing,                         //155 SPELL_AURA_MOD_WATER_BREATHING
    &Aura::HandleNoImmediateEffect,                         //156 SPELL_AURA_MOD_REPUTATION_GAIN        implemented in Player::CalculateReputationGain
    &Aura::HandleUnused,                                    //157 SPELL_AURA_PET_DAMAGE_MULTI (single test like spell 20782, also single for 214 aura)
    &Aura::HandleShieldBlockValue,                          //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Aura::HandleNoImmediateEffect,                         //159 SPELL_AURA_NO_PVP_CREDIT              implemented in Player::RewardHonor
    &Aura::HandleNoImmediateEffect,                         //160 SPELL_AURA_MOD_AOE_AVOIDANCE          implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT implemented in Player::RegenerateAll and Player::RegenerateHealth
    &Aura::HandleAuraPowerBurn,                             //162 SPELL_AURA_POWER_BURN_MANA
    &Aura::HandleNoImmediateEffect,                         //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS      implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleUnused,                                    //164 unused (3.0.8a-3.2.2a), only one test spell 10654
    &Aura::HandleNoImmediateEffect,                         //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModAttackPowerPercent,                 //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Aura::HandleAuraModRangedAttackPowerPercent,           //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Aura::HandleNoImmediateEffect,                         //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS            implemented in Unit::SpellDamageBonusDone, Unit::MeleeDamageBonusDone
    &Aura::HandleNoImmediateEffect,                         //169 SPELL_AURA_MOD_CRIT_PERCENT_VERSUS           implemented in Unit::DealDamageBySchool, Unit::DoAttackDamage, Unit::SpellCriticalBonus
    &Aura::HandleDetectAmore,                               //170 SPELL_AURA_DETECT_AMORE       different spells that ignore transformation effects
    &Aura::HandleAuraModIncreaseSpeed,                      //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Aura::HandleAuraModIncreaseMountedSpeed,               //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Aura::HandleUnused,                                    //173 unused (3.0.8a-3.2.2a) no spells, old SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Aura::HandleModSpellDamagePercentFromStat,             //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT  implemented in Unit::SpellBaseDamageBonusDone
    &Aura::HandleModSpellHealingPercentFromStat,            //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT implemented in Unit::SpellBaseHealingBonusDone
    &Aura::HandleSpiritOfRedemption,                        //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Aura::HandleAoECharm,                                  //177 SPELL_AURA_AOE_CHARM
    &Aura::HandleNoImmediateEffect,                         //178 SPELL_AURA_MOD_DEBUFF_RESISTANCE          implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE implemented in Unit::SpellCriticalBonus
    &Aura::HandleNoImmediateEffect,                         //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS   implemented in Unit::SpellDamageBonusDone
    &Aura::HandleUnused,                                    //181 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS
    &Aura::HandleAuraModResistenceOfStatPercent,            //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //183 SPELL_AURA_MOD_CRITICAL_THREAT only used in 28746, implemented in ThreatCalcHelper::CalcThreat
    &Aura::HandleNoImmediateEffect,                         //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE  implemented in Unit::CalculateEffectiveMissChance
    &Aura::HandleNoImmediateEffect,                         //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE implemented in Unit::CalculateEffectiveMissChance
    &Aura::HandleNoImmediateEffect,                         //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE  implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE  implemented in Unit::CalculateEffectiveCritChance
    &Aura::HandleNoImmediateEffect,                         //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE implemented in Unit::CalculateEffectiveCritChance
    &Aura::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &Aura::HandleNoImmediateEffect,                         //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN     implemented in Player::CalculateReputationGain
    &Aura::HandleAuraModUseNormalSpeed,                     //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Aura::HandleModMeleeRangedSpeedPct,                    //192 SPELL_AURA_MOD_MELEE_RANGED_HASTE
    &Aura::HandleModCombatSpeedPct,                         //193 SPELL_AURA_HASTE_ALL (in fact combat (any type attack) speed pct)
    &Aura::HandleNoImmediateEffect,                         //194 SPELL_AURA_MOD_IGNORE_ABSORB_SCHOOL       implement in Unit::CalcNotIgnoreAbsorbDamage
    &Aura::HandleNoImmediateEffect,                         //195 SPELL_AURA_MOD_IGNORE_ABSORB_FOR_SPELL    implement in Unit::CalcNotIgnoreAbsorbDamage
    &Aura::HandleNULL,                                      //196 SPELL_AURA_MOD_COOLDOWN (single spell 24818 in 3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE implemented in Unit::SpellCriticalBonus Unit::GetUnitCriticalChance
    &Aura::HandleUnused,                                    //198 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &Aura::HandleNoImmediateEffect,                         //199 SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT  implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //200 SPELL_AURA_MOD_KILL_XP_PCT                 implemented in Player::GiveXP
    &Aura::HandleAuraAllowFlight,                           //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &Aura::HandleNoImmediateEffect,                         //202 SPELL_AURA_IGNORE_COMBAT_RESULT            implemented in Unit::MeleeSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE  implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleNoImmediateEffect,                         //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleNoImmediateEffect,                         //205 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_DAMAGE  implemented in Unit::SpellCriticalDamageBonus
    &Aura::HandleAuraModIncreaseFlightSpeed,                //206 SPELL_AURA_MOD_FLIGHT_SPEED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //207 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //208 SPELL_AURA_MOD_FLIGHT_SPEED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //209 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //210 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //211 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING
    &Aura::HandleAuraModRangedAttackPowerOfStatPercent,     //212 SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &Aura::HandleUnused,                                    //214 Tamed Pet Passive (single test like spell 20782, also single for 157 aura)
    &Aura::HandleArenaPreparation,                          //215 SPELL_AURA_ARENA_PREPARATION
    &Aura::HandleModCastingSpeed,                           //216 SPELL_AURA_HASTE_SPELLS
    &Aura::HandleModMeleeSpeedPct,                          //217 SPELL_AURA_MOD_MELEE_HASTE_2
    &Aura::HandleAuraModRangedHaste,                        //218 SPELL_AURA_HASTE_RANGED
    &Aura::HandleModManaRegen,                              //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &Aura::HandleModRatingFromStat,                         //220 SPELL_AURA_MOD_RATING_FROM_STAT
    &Aura::HandleAuraDetaunt,                               //221 SPELL_AURA_DETAUNT
    &Aura::HandleUnused,                                    //222 unused (3.0.8a-3.2.2a) only for spell 44586 that not used in real spell cast
    &Aura::HandleNULL,                                      //223 SPELL_AURA_RAID_PROC_FROM_CHARGE
    &Aura::HandleUnused,                                    //224 unused (3.0.8a-3.2.2a)
    &Aura::HandlePrayerOfMending,                           //225 SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE
    &Aura::HandleAuraPeriodicDummy,                         //226 SPELL_AURA_PERIODIC_DUMMY
    &Aura::HandlePeriodicTriggerSpellWithValue,             //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //228 SPELL_AURA_DETECT_STEALTH
    &Aura::HandleNoImmediateEffect,                         //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE        implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleAuraModIncreaseMaxHealth,                  //230 Commanding Shout
    &Aura::HandleNoImmediateEffect,                         //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //232 SPELL_AURA_MECHANIC_DURATION_MOD           implement in Unit::CalculateAuraDuration
    &Aura::HandleNULL,                                      //233 set model id to the one of the creature with id m_modifier.m_miscvalue
    &Aura::HandleNoImmediateEffect,                         //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK implement in Unit::CalculateAuraDuration
    &Aura::HandleAuraModDispelResist,                       //235 SPELL_AURA_MOD_DISPEL_RESIST               implement in Unit::MagicSpellHitResult
    &Aura::HandleAuraControlVehicle,                        //236 SPELL_AURA_CONTROL_VEHICLE
    &Aura::HandleModSpellDamagePercentFromAttackPower,      //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER  implemented in Unit::SpellBaseDamageBonusDone
    &Aura::HandleModSpellHealingPercentFromAttackPower,     //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER implemented in Unit::SpellBaseHealingBonusDone
    &Aura::HandleAuraModScale,                              //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &Aura::HandleAuraModExpertise,                          //240 SPELL_AURA_MOD_EXPERTISE
    &Aura::HandleForceMoveForward,                          //241 SPELL_AURA_FORCE_MOVE_FORWARD Forces the caster to move forward
    &Aura::HandleUnused,                                    //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING (only 2 test spels in 3.2.2a)
    &Aura::HandleFactionOverride,                           //243 SPELL_AURA_FACTION_OVERRIDE
    &Aura::HandleComprehendLanguage,                        //244 SPELL_AURA_COMPREHEND_LANGUAGE
    &Aura::HandleNoImmediateEffect,                         //245 SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS     implemented in Unit::CalculateAuraDuration
    &Aura::HandleNoImmediateEffect,                         //246 SPELL_AURA_MOD_DURATION_OF_EFFECTS_BY_DISPEL implemented in Unit::CalculateAuraDuration
    &Aura::HandleAuraMirrorImage,                           //247 SPELL_AURA_MIRROR_IMAGE                      target to become a clone of the caster
    &Aura::HandleNoImmediateEffect,                         //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE         implemented in Unit::CalculateEffectiveDodgeChance, Unit::CalculateEffectiveParryChance, Unit::CalculateEffectiveBlockChance
    &Aura::HandleAuraConvertRune,                           //249 SPELL_AURA_CONVERT_RUNE
    &Aura::HandleAuraModIncreaseHealth,                     //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &Aura::HandleNULL,                                      //251 SPELL_AURA_MOD_ENEMY_DODGE
    &Aura::HandleModCombatSpeedPct,                         //252 SPELL_AURA_SLOW_ALL
    &Aura::HandleNoImmediateEffect,                         //253 SPELL_AURA_MOD_BLOCK_CRIT_CHANCE             implemented in Unit::CalculateMeleeDamage
    &Aura::HandleAuraModDisarm,                             //254 SPELL_AURA_MOD_DISARM_OFFHAND     also disarm shield
    &Aura::HandleNoImmediateEffect,                         //255 SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT    implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNoReagentUseAura,                          //256 SPELL_AURA_NO_REAGENT_USE Use SpellClassMask for spell select
    &Aura::HandleNULL,                                      //257 SPELL_AURA_MOD_TARGET_RESIST_BY_SPELL_CLASS Use SpellClassMask for spell select
    &Aura::HandleNULL,                                      //258 SPELL_AURA_MOD_SPELL_VISUAL
    &Aura::HandleNoImmediateEffect,                         //259 SPELL_AURA_MOD_PERIODIC_HEAL                    implemented in Unit::SpellHealingBonus
    &Aura::HandleNoImmediateEffect,                         //260 SPELL_AURA_SCREEN_EFFECT (miscvalue = id in ScreenEffect.dbc) not required any code
    &Aura::HandlePhase,                                     //261 SPELL_AURA_PHASE undetectable invisibility?     implemented in Unit::IsVisibleForOrDetect
    &Aura::HandleNoImmediateEffect,                         //262 SPELL_AURA_IGNORE_UNIT_STATE                    implemented in Unit::isIgnoreUnitState & Spell::CheckCast
    &Aura::HandleAuraAllowOnlyAbility,                      //263 SPELL_AURA_ALLOW_ONLY_ABILITY
    &Aura::HandleUnused,                                    //264 unused (3.0.8a-3.2.2a)
    &Aura::HandleUnused,                                    //265 unused (3.0.8a-3.2.2a)
    &Aura::HandleUnused,                                    //266 unused (3.0.8a-3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //267 SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL         implemented in Unit::IsImmuneToSpellEffect
    &Aura::HandleAuraModAttackPowerOfStatPercent,           //268 SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //269 SPELL_AURA_MOD_IGNORE_DAMAGE_REDUCTION_SCHOOL   implemented in Unit::CalcNotIgnoreDamageReduction
    &Aura::HandleUnused,                                    //270 SPELL_AURA_MOD_IGNORE_TARGET_RESIST (unused in 3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //271 SPELL_AURA_MOD_DAMAGE_FROM_CASTER    implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //272 SPELL_AURA_MAELSTROM_WEAPON (unclear use for aura, it used in (3.2.2a...3.3.0) in single spell 53817 that spellmode stacked and charged spell expected to be drop as stack
    &Aura::HandleNoImmediateEffect,                         //273 SPELL_AURA_X_RAY (client side implementation)
    &Aura::HandleNULL,                                      //274 proc free shot?
    &Aura::HandleNoImmediateEffect,                         //275 SPELL_AURA_MOD_IGNORE_SHAPESHIFT Use SpellClassMask for spell select
    &Aura::HandleNULL,                                      //276 mod damage % mechanic?
    &Aura::HandleNoImmediateEffect,                         //277 SPELL_AURA_MOD_MAX_AFFECTED_TARGETS Use SpellClassMask for spell select
    &Aura::HandleAuraModDisarm,                             //278 SPELL_AURA_MOD_DISARM_RANGED disarm ranged weapon
    &Aura::HandleMirrorName,                                //279 SPELL_AURA_MIRROR_NAME                target receives the casters name
    &Aura::HandleModTargetArmorPct,                         //280 SPELL_AURA_MOD_TARGET_ARMOR_PCT
    &Aura::HandleNoImmediateEffect,                         //281 SPELL_AURA_MOD_HONOR_GAIN             implemented in Player::RewardHonor
    &Aura::HandleAuraIncreaseBaseHealthPercent,             //282 SPELL_AURA_INCREASE_BASE_HEALTH_PERCENT
    &Aura::HandleNoImmediateEffect,                         //283 SPELL_AURA_MOD_HEALING_RECEIVED       implemented in Unit::SpellHealingBonusTaken
    &Aura::HandleTriggerLinkedAura,                         //284 SPELL_AURA_TRIGGER_LINKED_AURA
    &Aura::HandleAuraModAttackPowerOfArmor,                 //285 SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR  implemented in Player::UpdateAttackPowerAndDamage
    &Aura::HandleNoImmediateEffect,                         //286 SPELL_AURA_ABILITY_PERIODIC_CRIT      implemented in Aura::IsCritFromAbilityAura called from Aura::PeriodicTick
    &Aura::HandleNoImmediateEffect,                         //287 SPELL_AURA_DEFLECT_SPELLS             implemented in Unit::MagicSpellHitResult and Unit::MeleeSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //288 SPELL_AURA_MOD_PARRY_FROM_BEHIND_PERCENT percent from normal parry/deflect applied to from behind attack case (single spell used 67801, also look 4.1.0 spell 97574)
    &Aura::HandleNoImmediateEffect,                         //289 SPELL_AURA_PREVENT_DURABILITY_LOSS
    &Aura::HandleAuraModAllCritChance,                      //290 SPELL_AURA_MOD_ALL_CRIT_CHANCE
    &Aura::HandleNoImmediateEffect,                         //291 SPELL_AURA_MOD_QUEST_XP_PCT           implemented in Player::GiveXP
    &Aura::HandleAuraOpenStable,                            //292 SPELL_AURA_OPEN_STABLE
    &Aura::HandleAuraAddMechanicAbilities,                  //293 SPELL_AURA_ADD_MECHANIC_ABILITIES  replaces target's action bars with a predefined spellset
    &Aura::HandleAuraStopNaturalManaRegen,                  //294 SPELL_AURA_STOP_NATURAL_MANA_REGEN implemented in Player:Regenerate
    &Aura::HandleUnused,                                    //295 unused (3.2.2a)
    &Aura::HandleAuraSetVehicleId,                          //296 SPELL_AURA_SET_VEHICLE_ID
    &Aura::HandleNULL,                                      //297 SPELL_AURA_BLOCK_SPELL_FAMILY
    &Aura::HandleUnused,                                    //298 SPELL_AURA_STRANGULATE
    &Aura::HandleUnused,                                    //299 unused (3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //300 SPELL_AURA_SHARE_DAMAGE_PCT 9 spells
    &Aura::HandleNULL,                                      //301 SPELL_AURA_HEAL_ABSORB 5 spells
    &Aura::HandleUnused,                                    //302 unused (3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //303 SPELL_AURA_DAMAGE_DONE_VERSUS_AURA_STATE_PCT - 17 spells implemented in Unit::*DamageBonus
    &Aura::HandleAuraFakeInebriation,                       //304 SPELL_AURA_FAKE_INEBRIATE
    &Aura::HandleAuraModIncreaseSpeed,                      //305 SPELL_AURA_MOD_MINIMUM_SPEED
    &Aura::HandleNULL,                                      //306 1 spell
    &Aura::HandleNULL,                                      //307 absorb healing?
    &Aura::HandleNULL,                                      //308 SPELL_AURA_MOD_CRIT_CHANCE_FOR_CASTER
    &Aura::HandleNULL,                                      //309 absorb healing?
    &Aura::HandleNoImmediateEffect,                         //310 SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNULL,                                      //311 0 spells in 3.3
    &Aura::HandleNULL,                                      //312 0 spells in 3.3
    &Aura::HandleNULL,                                      //313 0 spells in 3.3
    &Aura::HandlePreventResurrection,                       //314 SPELL_AURA_PREVENT_RESURRECTION
    &Aura::HandleNULL,                                      //315 SPELL_AURA_UNDERWATER_WALKING
    &Aura::HandleNULL                                       //316 SPELL_AURA_PERIODIC_HASTE
};

static AuraType const frozenAuraTypes[] = { SPELL_AURA_MOD_ROOT, SPELL_AURA_MOD_STUN, SPELL_AURA_NONE };

Aura::Aura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 const* currentDamage, int32 const* currentBasePoints, SpellAuraHolder* holder, Unit* target, Unit* caster, Item* castItem) :
    m_spellmod(nullptr), m_periodicTimer(0), m_periodicTick(0), m_removeMode(AURA_REMOVE_BY_DEFAULT),
    m_effIndex(eff), m_positive(false), m_isPeriodic(false), m_isAreaAura(false),
    m_isPersistent(false), m_magnetUsed(false), m_spellAuraHolder(holder),
    m_scriptValue(0), m_storage(nullptr)
{
    MANGOS_ASSERT(target);
    MANGOS_ASSERT(spellproto && spellproto == sSpellTemplate.LookupEntry<SpellEntry>(spellproto->Id) && "`info` must be pointer to sSpellTemplate element");

    m_currentBasePoints = currentBasePoints ? *currentBasePoints : spellproto->CalculateSimpleValue(eff);

    m_positive = IsPositiveAuraEffect(spellproto, m_effIndex, caster, target);

    m_applyTime = time(nullptr);

    int32 damage = currentDamage ? *currentDamage : m_currentBasePoints;
    if (caster)
    {
        if (!damage && castItem && castItem->GetItemSuffixFactor())
        {
            ItemRandomSuffixEntry const* item_rand_suffix = sItemRandomSuffixStore.LookupEntry(abs(castItem->GetItemRandomPropertyId()));
            if (item_rand_suffix)
            {
                for (int k = 0; k < 3; ++k)
                {
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(item_rand_suffix->enchant_id[k]);
                    if (pEnchant)
                    {
                        for (unsigned int t : pEnchant->spellid)
                        {
                            if (t != spellproto->Id)
                                continue;

                            damage = uint32((item_rand_suffix->prefix[k] * castItem->GetItemSuffixFactor()) / 10000);
                            break;
                        }
                    }

                    if (damage)
                        break;
                }
            }
        }

        damage = CalculateAuraEffectValue(caster, target, spellproto, eff, damage);

        damage = OnAuraValueCalculate(caster, damage);
    }

    damage *= holder->GetStackAmount();

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Aura: construct Spellid : %u, Aura : %u Target : %d Damage : %d", spellproto->Id, spellproto->EffectApplyAuraName[eff], spellproto->EffectImplicitTargetA[eff], damage);

    SetModifier(AuraType(spellproto->EffectApplyAuraName[eff]), damage, spellproto->EffectAmplitude[eff], spellproto->EffectMiscValue[eff]);

    Player* modOwner = caster ? caster->GetSpellModOwner() : nullptr;

    // Apply periodic time mod
    if (modOwner && m_modifier.periodictime)
        modOwner->ApplySpellMod(spellproto->Id, SPELLMOD_ACTIVATION_TIME, m_modifier.periodictime);

    if (caster && (spellproto->HasAttribute(SPELL_ATTR_EX5_SPELL_HASTE_AFFECTS_PERIODIC) || caster->HasAffectedAura(SPELL_AURA_PERIODIC_HASTE, spellproto)))
        m_modifier.periodictime = int32(m_modifier.periodictime * caster->GetFloatValue(UNIT_MOD_CAST_SPEED));

    // Start periodic on next tick or at aura apply
    if (!spellproto->HasAttribute(SPELL_ATTR_EX5_EXTRA_INITIAL_PERIOD))
        m_periodicTimer = m_modifier.periodictime;
}

Aura::~Aura()
{
    delete m_storage;
    delete m_spellmod;
}

AreaAura::AreaAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 const* currentDamage, int32 const* currentBasePoints, SpellAuraHolder* holder, Unit* target,
                   Unit* caster, Item* castItem, uint32 originalRankSpellId)
    : Aura(spellproto, eff, currentDamage, currentBasePoints, holder, target, caster, castItem), m_originalRankSpellId(originalRankSpellId)
{
    m_isAreaAura = true;

    // caster==nullptr in constructor args if target==caster in fact
    Unit* caster_ptr = caster ? caster : target;

    m_radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellproto->EffectRadiusIndex[m_effIndex]));
    if (Player* modOwner = caster_ptr->GetSpellModOwner())
        modOwner->ApplySpellMod(spellproto->Id, SPELLMOD_RADIUS, m_radius);

    switch (spellproto->Effect[eff])
    {
        case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
            m_areaAuraType = AREA_AURA_PARTY;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_RAID:
            m_areaAuraType = AREA_AURA_RAID;
            // Light's Beacon not applied to caster itself (TODO: more generic check for another similar spell if any?)
            if (target == caster_ptr && spellproto->Id == 53651)
                m_modifier.m_auraname = SPELL_AURA_NONE;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
            m_areaAuraType = AREA_AURA_FRIEND;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
            m_areaAuraType = AREA_AURA_ENEMY;
            if (target == caster_ptr)
                m_modifier.m_auraname = SPELL_AURA_NONE;    // Do not do any effect on self
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_PET:
            m_areaAuraType = AREA_AURA_PET;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
            m_areaAuraType = AREA_AURA_OWNER;
            if (target == caster_ptr)
                m_modifier.m_auraname = SPELL_AURA_NONE;
            break;
        default:
            sLog.outError("Wrong spell effect in AreaAura constructor");
            MANGOS_ASSERT(false);
            break;
    }

    // totems are immune to any kind of area auras
    if (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsTotem())
        m_modifier.m_auraname = SPELL_AURA_NONE;
}

AreaAura::~AreaAura()
{
}

PersistentAreaAura::PersistentAreaAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 const* currentDamage, int32 const* currentBasePoints, SpellAuraHolder* holder, Unit* target,
                                       Unit* caster, Item* castItem) : Aura(spellproto, eff, currentDamage, currentBasePoints, holder, target, caster, castItem)
{
    m_isPersistent = true;
}

PersistentAreaAura::~PersistentAreaAura()
{
}

Aura* CreateAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 const* currentDamage, int32 const* currentBasePoints, SpellAuraHolder* holder, Unit* target, Unit* caster, Item* castItem, uint64 scriptValue)
{
    Aura* aura = nullptr;
    if (IsAreaAuraEffect(spellproto->Effect[eff]))
        aura = new AreaAura(spellproto, eff, currentDamage, currentBasePoints, holder, target, caster, castItem);

    if (!aura)
        aura = new Aura(spellproto, eff, currentDamage, currentBasePoints, holder, target, caster, castItem);

    aura->SetScriptValue(scriptValue); // must be done before OnAuraInit
    aura->OnAuraInit();
    return aura;
}

SpellAuraHolder* CreateSpellAuraHolder(SpellEntry const* spellproto, Unit* target, WorldObject* caster, Item* castItem /*= nullptr*/, SpellEntry const* triggeredBy /*= nullptr*/)
{
    return new SpellAuraHolder(spellproto, target, caster, castItem, triggeredBy);
}

void Aura::SetModifier(AuraType type, int32 amount, uint32 periodicTime, int32 miscValue)
{
    m_modifier.m_auraname = type;
    m_modifier.m_amount = amount * GetStackAmount();
    m_modifier.m_baseAmount = amount;
    m_modifier.m_miscvalue = miscValue;
    m_modifier.periodictime = periodicTime;
}

void Aura::Update(uint32 diff)
{
    if (m_isPeriodic)
    {
        m_periodicTimer -= diff;
        if (m_periodicTimer <= 0) // tick also at m_periodicTimer==0 to prevent lost last tick in case max m_duration == (max m_periodicTimer)*N
        {
            // update before applying (aura can be removed in TriggerSpell or PeriodicTick calls)
            m_periodicTimer += m_modifier.periodictime;
            ++m_periodicTick;                               // for some infinity auras in some cases can overflow and reset
            PeriodicTick();
        }
    }
}

void AreaAura::Update(uint32 diff)
{
    // update for the caster of the aura
    if (GetCasterGuid() == GetTarget()->GetObjectGuid())
    {
        Unit* caster = GetTarget();

        if (!caster->hasUnitState(UNIT_STAT_ISOLATED))
        {
            Unit* owner = caster->GetMaster();
            if (!owner)
                owner = caster;
            UnitList targets;

            switch (m_areaAuraType) // TODO: Unify with SetTargetMap
            {
                case AREA_AURA_PARTY:
                {
                    Group* pGroup = nullptr;

                    // Handle aura party for players
                    if (owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        pGroup = ((Player*)owner)->GetGroup();

                        if (pGroup)
                        {
                            uint8 subgroup = ((Player*)owner)->GetSubGroup();
                            for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
                            {
                                Player* Target = itr->getSource();
                                if (Target && Target->IsAlive() && Target->GetSubGroup() == subgroup && caster->CanAssistSpell(Target, GetSpellProto()))
                                {
                                    if (caster->IsWithinDistInMap(Target, m_radius, true, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT)))
                                        targets.push_back(Target);
                                    Pet* pet = Target->GetPet();
                                    if (pet && pet->IsAlive() && caster->IsWithinDistInMap(pet, m_radius, true, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT)))
                                        targets.push_back(pet);
                                }
                            }
                            break;
                        }
                    }
                    else    // handle aura party for creatures
                    {
                        // Get all creatures in spell radius
                        std::list<Creature*> nearbyTargets;
                        MaNGOS::AnyUnitInObjectRangeCheck u_check(owner, m_radius);
                        MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(nearbyTargets, u_check);
                        Cell::VisitGridObjects(owner, searcher, m_radius);

                        for (auto target : nearbyTargets)
                        {
                            // Due to the lack of support for NPC groups or formations, are considered of the same party NPCs with same faction than caster
                            if (target != owner && target->IsAlive() && target->GetFaction() == ((Creature*)owner)->GetFaction())
                                targets.push_back(target);
                        }
                    }

                    // add owner
                    if (owner != caster && caster->IsWithinDistInMap(owner, m_radius, true, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT)))
                        targets.push_back(owner);
                    // add caster's pet
                    Unit* pet = caster->GetPet();
                    if (pet && caster->IsWithinDistInMap(pet, m_radius, true, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT)))
                        targets.push_back(pet);

                    break;
                }
                case AREA_AURA_RAID:
                {
                    Group* pGroup = nullptr;

                    if (owner->GetTypeId() == TYPEID_PLAYER)
                        pGroup = ((Player*)owner)->GetGroup();

                    if (pGroup)
                    {
                        for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
                        {
                            Player* Target = itr->getSource();
                            if (Target && Target->IsAlive() && caster->CanAssistSpell(Target, GetSpellProto()))
                            {
                                if (caster->IsWithinDistInMap(Target, m_radius))
                                    targets.push_back(Target);
                                Pet* pet = Target->GetPet();
                                if (pet && pet->IsAlive() && caster->IsWithinDistInMap(pet, m_radius))
                                    targets.push_back(pet);
                            }
                        }
                    }
                    else
                    {
                        // add owner
                        if (owner != caster && caster->IsWithinDistInMap(owner, m_radius))
                            targets.push_back(owner);
                        // add caster's pet
                        Unit* pet = caster->GetPet();
                        if (pet && caster->IsWithinDistInMap(pet, m_radius))
                            targets.push_back(pet);
                    }
                    break;
                }
                case AREA_AURA_FRIEND:
                {
                    MaNGOS::AnySpellAssistableUnitInObjectRangeCheck u_check(caster, nullptr, m_radius, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT));
                    MaNGOS::UnitListSearcher<MaNGOS::AnySpellAssistableUnitInObjectRangeCheck> searcher(targets, u_check);
                    Cell::VisitAllObjects(caster, searcher, m_radius);
                    break;
                }
                case AREA_AURA_ENEMY:
                {
                    MaNGOS::AnyAoETargetUnitInObjectRangeCheck u_check(caster, nullptr, m_radius, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT)); // No GetCharmer in searcher
                    MaNGOS::UnitListSearcher<MaNGOS::AnyAoETargetUnitInObjectRangeCheck> searcher(targets, u_check);
                    Cell::VisitAllObjects(caster, searcher, m_radius);
                    break;
                }
                case AREA_AURA_OWNER:
                case AREA_AURA_PET:
                {
                    if (owner != caster && caster->IsWithinDistInMap(owner, m_radius, true, GetSpellProto()->HasAttribute(SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT))
                        && caster->CanAssistSpell(owner, GetSpellProto()))
                        targets.push_back(owner);
                    break;
                }
            }

            for (auto& target : targets)
            {
                // flag for selection is need apply aura to current iteration target
                bool apply = true;

                SpellEntry const* actualSpellInfo;
                if (GetCasterGuid() == target->GetObjectGuid()) // if caster is same as target then no need to change rank of the spell
                    actualSpellInfo = GetSpellProto();
                else
                    actualSpellInfo = sSpellMgr.SelectAuraRankForLevel(GetSpellProto(), target->GetLevel()); // use spell id according level of the target
                if (!actualSpellInfo)
                    continue;

                Unit::SpellAuraHolderBounds spair = target->GetSpellAuraHolderBounds(actualSpellInfo->Id);
                // we need ignore present caster self applied are auras sometime
                // in cases if this only auras applied for spell effect
                for (Unit::SpellAuraHolderMap::const_iterator i = spair.first; i != spair.second; ++i)
                {
                    if (i->second->IsDeleted())
                        continue;

                    Aura* aur = i->second->GetAuraByEffectIndex(m_effIndex);

                    if (!aur)
                        continue;

                    switch (m_areaAuraType)
                    {
                        case AREA_AURA_ENEMY:
                            // non caster self-casted auras (non stacked)
                            if (aur->GetModifier()->m_auraname != SPELL_AURA_NONE)
                                apply = false;
                            break;
                        case AREA_AURA_PARTY:
                        case AREA_AURA_RAID:
                            // non caster self-casted auras (stacked from diff. casters)
                            if (aur->GetModifier()->m_auraname != SPELL_AURA_NONE && i->second->GetCasterGuid() != GetCasterGuid())
                            {
                                apply = IsStackableSpell(actualSpellInfo, i->second->GetSpellProto(), target);
                                break;
                            }
                            if (aur->GetModifier()->m_auraname != SPELL_AURA_NONE || i->second->GetCasterGuid() == GetCasterGuid())
                                apply = false;
                            break;
                        default:
                            // in generic case not allow stacking area auras
                            apply = false;
                            break;
                    }

                    if (!apply)
                        break;
                }

                if (!apply)
                    continue;

                // Skip some targets (TODO: Might require better checks, also unclear how the actual caster must/can be handled)
                if (actualSpellInfo->HasAttribute(SPELL_ATTR_EX3_ONLY_ON_PLAYER) && target->GetTypeId() != TYPEID_PLAYER)
                    continue;

                if (actualSpellInfo->HasAttribute(SPELL_ATTR_EX5_NOT_ON_PLAYER) && target->GetTypeId() == TYPEID_PLAYER)
                    continue;

                if (actualSpellInfo->HasAttribute(SPELL_ATTR_EX5_NOT_ON_PLAYER_CONTROLLED_NPC) && target->IsPlayerControlled() && target->GetTypeId() != TYPEID_PLAYER)
                    continue;

                int32 actualBasePoints = m_currentBasePoints;
                int32 actualDamage = m_modifier.m_baseAmount;
                // recalculate basepoints for lower rank (all AreaAura spell not use custom basepoints?)
                if (actualSpellInfo != GetSpellProto())
                {
                    actualBasePoints = actualSpellInfo->CalculateSimpleValue(m_effIndex);
                    actualDamage = caster->CalculateSpellEffectValue(target, actualSpellInfo, m_effIndex, &actualBasePoints);
                }

                SpellAuraHolder* holder = target->GetSpellAuraHolder(actualSpellInfo->Id, GetCasterGuid());

                bool addedToExisting = true;
                if (!holder)
                {
                    holder = CreateSpellAuraHolder(actualSpellInfo, target, caster);
                    addedToExisting = false;
                }

                holder->SetAuraDuration(GetAuraDuration());

                AreaAura* aur = new AreaAura(actualSpellInfo, m_effIndex, &actualDamage, &actualBasePoints, holder, target, caster, nullptr, GetSpellProto()->Id);
                holder->AddAura(aur, m_effIndex);

                if (addedToExisting)
                {
                    target->AddAuraToModList(aur);
                    aur->ApplyModifier(true, true);
                }
                else
                {
                    if (target->AddSpellAuraHolder(holder))
                        holder->SetState(SPELLAURAHOLDER_STATE_READY);
                    else
                        delete holder;
                }
            }
        }
        Aura::Update(diff);
    }
    else                                                    // aura at non-caster
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        uint32 originalRankSpellId = m_originalRankSpellId ? m_originalRankSpellId : GetId(); // caster may have different spell id if target has lower level

        Aura::Update(diff);

        // remove aura if out-of-range from caster (after teleport for example)
        // or caster is isolated or caster no longer has the aura
        // or caster is (no longer) friendly
        bool needFriendly = (m_areaAuraType != AREA_AURA_ENEMY);
        if (!caster ||
                caster->hasUnitState(UNIT_STAT_ISOLATED)               ||
                !caster->HasAura(originalRankSpellId, GetEffIndex())   ||
                !caster->IsWithinDistInMap(target, m_radius)           ||
                caster->CanAssistSpell(target, GetSpellProto()) != needFriendly
           )
        {
            target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
        }
        else if (m_areaAuraType == AREA_AURA_PARTY)         // check if in same sub group
        {
            // Do not check group if target == owner or target == pet
            // or if caster is a not player (as NPCs do not support group so aura is only removed by moving out of range)
            if (caster->GetMasterGuid() != target->GetObjectGuid()  &&
                caster->GetObjectGuid() != target->GetMasterGuid()  &&
                caster->IsPlayerControlled())
            {
                Player* check = caster->GetBeneficiaryPlayer();

                Group* pGroup = check ? check->GetGroup() : nullptr;
                if (pGroup)
                {
                    Player* checkTarget = target->GetBeneficiaryPlayer();
                    if (!checkTarget || !pGroup->SameSubGroup(check, checkTarget))
                        target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
                }
                else
                    target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
            }
        }
        else if (m_areaAuraType == AREA_AURA_RAID)          // Check if on same raid group
        {
            // not check group if target == owner or target == pet
            if (caster->GetMasterGuid() != target->GetObjectGuid() && caster->GetObjectGuid() != target->GetMasterGuid())
            {
                Player* check = caster->GetBeneficiaryPlayer();

                Group* pGroup = check ? check->GetGroup() : nullptr;
                if (pGroup)
                {
                    Player* checkTarget = target->GetBeneficiaryPlayer();
                    if (!checkTarget || !checkTarget->GetGroup() || checkTarget->GetGroup()->GetId() != pGroup->GetId())
                        target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
                }
                else
                    target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
            }
        }
        else if (m_areaAuraType == AREA_AURA_PET || m_areaAuraType == AREA_AURA_OWNER)
        {
            if (target->GetObjectGuid() != caster->GetMasterGuid())
                target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
        }
    }
}

void PersistentAreaAura::Update(uint32 diff)
{
    bool remove = true;
    AuraRemoveMode removeMode = AURA_REMOVE_BY_EXPIRE;

    // remove the aura if its caster or the dynamic object causing it was removed
    // or if the target moves too far from the dynamic object
    if (Unit* caster = GetCaster())
    {
        if (DynamicObject* dynObj = caster->GetDynObject(GetId(), GetEffIndex(), GetTarget()))
        {
            if (GetTarget()->GetDistance(dynObj, true, DIST_CALC_COMBAT_REACH) > dynObj->GetRadius())
            {
                removeMode = AURA_REMOVE_BY_DEFAULT;
                dynObj->RemoveAffected(GetTarget()); // let later reapply if target return to range
            }
            else
                remove = false;
        }
    }

    if (remove)
    {
        GetTarget()->RemoveSingleAuraFromSpellAuraHolder(GetHolder(), GetEffIndex(), removeMode);
        return;
    }

    Aura::Update(diff);
}

bool Aura::IsSaveToDbAura() const
{
    if (IsAreaAura() && GetHolder()->GetCasterGuid() != GetTarget()->GetObjectGuid())
        return false;

    if (IsPersistent())
        return false;

    return true;
}

void Aura::ApplyModifier(bool apply, bool Real)
{
    AuraType aura = m_modifier.m_auraname;

    if (apply)
        OnApply(apply);
    if (!apply)
        OnAfterApply(apply);
    if (aura < TOTAL_AURAS)
        (*this.*AuraHandler [aura])(apply, Real);
    if (apply)
        OnAfterApply(apply);
    if (!apply)
        OnApply(apply);

    if (GetSpellProto()->HasAttribute(SPELL_ATTR_EX4_OWNER_POWER_SCALING) && m_removeMode != AURA_REMOVE_BY_GAINED_STACK)
        GetTarget()->RegisterScalingAura(this, apply);
}

void Aura::UpdateAuraScaling()
{
    if (Unit* caster = GetCaster())
    {
        int32 amount = 0;
        amount = OnAuraValueCalculate(caster, amount);
        // Reapply if amount change
        if (amount != GetModifier()->m_amount)
        {
            SetRemoveMode(AURA_REMOVE_BY_GAINED_STACK);
            if (IsAuraRemoveOnStacking(this->GetSpellProto(), GetEffIndex()))
                ApplyModifier(false, true);
            GetModifier()->m_recentAmount = amount - GetModifier()->m_amount;
            GetModifier()->m_amount = amount;
            ApplyModifier(true, true);
        }
    }
}

bool Aura::isAffectedOnSpell(SpellEntry const* spell) const
{
    return spell->IsFitToFamily(SpellFamily(GetSpellProto()->SpellFamilyName), GetAuraSpellClassMask());
}

bool Aura::CanProcFrom(SpellEntry const* spell, uint32 /*procFlag*/, uint32 EventProcEx, uint32 procEx, bool active, bool useClassMask) const
{
    // Check EffectClassMask
    ClassFamilyMask const& mask  = GetAuraSpellClassMask();

    // allow proc for modifier auras with charges
    if (IsCastEndProcModifierAura(GetSpellProto(), GetEffIndex(), spell))
    {
        if (GetHolder()->GetAuraCharges() > 0)
        {
            if (procEx != PROC_EX_CAST_END && EventProcEx == PROC_EX_NONE)
                return false;
        }
    }
    else if (EventProcEx == PROC_EX_NONE && procEx == PROC_EX_CAST_END)
        return false;

    // if no class mask defined, or spell_proc_event has SpellFamilyName=0 - allow proc
    if (!useClassMask || !mask)
    {
        if (!(EventProcEx & PROC_EX_EX_TRIGGER_ALWAYS))
        {
            // Check for extra req (if none) and hit/crit
            if (EventProcEx == PROC_EX_NONE)
            {
                // No extra req, so can trigger only for active (damage/healing present) and hit/crit
                return ((procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) && active) || procEx == PROC_EX_CAST_END;
            }
                // Passive spells hits here only if resist/reflect/immune/evade
            // Passive spells can`t trigger if need hit (exclude cases when procExtra include non-active flags)
            if ((EventProcEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT) & procEx) && !active)
                return false;
        }
        return true;
    }
    // SpellFamilyName check is performed in SpellMgr::IsSpellProcEventCanTriggeredBy and it is done once for whole holder
    // note: SpellFamilyName is not checked if no spell_proc_event is defined
    return mask.IsFitToFamilyMask(spell->SpellFamilyFlags);
}

void Aura::ReapplyAffectedPassiveAuras(Unit* target, bool owner_mode)
{
    // we need store cast item guids for self casted spells
    // expected that not exist permanent auras from stackable auras from different items
    std::map<uint32, ObjectGuid> affectedSelf;

    std::set<uint32> affectedAuraCaster;

    for (Unit::SpellAuraHolderMap::const_iterator itr = target->GetSpellAuraHolderMap().begin(); itr != target->GetSpellAuraHolderMap().end(); ++itr)
    {
        // permanent passive or permanent area aura
        // passive spells can be affected only by own or owner spell mods)
        if ((itr->second->IsPermanent() && ((owner_mode && itr->second->IsPassive()) || itr->second->IsAreaAura())) &&
                // non deleted and not same aura (any with same spell id)
                !itr->second->IsDeleted() && itr->second->GetId() != GetId() &&
                // and affected by aura
                isAffectedOnSpell(itr->second->GetSpellProto()))
        {
            // only applied by self or aura caster
            if (itr->second->GetCasterGuid() == target->GetObjectGuid())
                affectedSelf[itr->second->GetId()] = itr->second->GetCastItemGuid();
            else if (itr->second->GetCasterGuid() == GetCasterGuid())
                affectedAuraCaster.insert(itr->second->GetId());
        }
    }

    if (!affectedSelf.empty())
    {
        Player* pTarget = target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : nullptr;

        for (std::map<uint32, ObjectGuid>::const_iterator map_itr = affectedSelf.begin(); map_itr != affectedSelf.end(); ++map_itr)
        {
            Item* item = pTarget && map_itr->second ? pTarget->GetItemByGuid(map_itr->second) : nullptr;
            target->RemoveAurasDueToSpell(map_itr->first);
            target->CastSpell(target, map_itr->first, TRIGGERED_OLD_TRIGGERED, item);
        }
    }

    if (!affectedAuraCaster.empty())
    {
        Unit* caster = GetCaster();
        for (uint32 set_itr : affectedAuraCaster)
        {
            target->RemoveAurasDueToSpell(set_itr);
            if (caster)
                caster->CastSpell(GetTarget(), set_itr, TRIGGERED_OLD_TRIGGERED);
        }
    }
}

struct ReapplyAffectedPassiveAurasHelper
{
    explicit ReapplyAffectedPassiveAurasHelper(Aura* _aura) : aura(_aura) {}
    void operator()(Unit* unit) const { aura->ReapplyAffectedPassiveAuras(unit, true); }
    Aura* aura;
};

void Aura::ReapplyAffectedPassiveAuras()
{
    // not reapply spell mods with charges (use original value because processed and at remove)
    if (GetSpellProto()->procCharges)
        return;

    // not reapply some spell mods ops (mostly speedup case)
    switch (m_modifier.m_miscvalue)
    {
        case SPELLMOD_DURATION:
        case SPELLMOD_CHARGES:
        case SPELLMOD_NOT_LOSE_CASTING_TIME:
        case SPELLMOD_CASTING_TIME:
        case SPELLMOD_COOLDOWN:
        case SPELLMOD_COST:
        case SPELLMOD_ACTIVATION_TIME:
        case SPELLMOD_GLOBAL_COOLDOWN:
            return;
    }

    // reapply talents to own passive persistent auras
    ReapplyAffectedPassiveAuras(GetTarget(), true);

    // re-apply talents/passives/area auras applied to pet/totems (it affected by player spellmods)
    GetTarget()->CallForAllControlledUnits(ReapplyAffectedPassiveAurasHelper(this), CONTROLLED_PET | CONTROLLED_TOTEMS);

    // re-apply talents/passives/area auras applied to group members (it affected by player spellmods)
    if (Group* group = ((Player*)GetTarget())->GetGroup())
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            if (Player* member = itr->getSource())
                if (member != GetTarget() && member->IsInMap(GetTarget()))
                    ReapplyAffectedPassiveAuras(member, false);
}

void Aura::PickTargetsForSpellTrigger(Unit*& triggerCaster, Unit*& triggerTarget, WorldObject*& triggerTargetObject, SpellEntry const* spellInfo)
{
    // automatic caster - target resolution
    switch (spellInfo->EffectImplicitTargetA[0])
    {
        case TARGET_LOCATION_UNIT_RANDOM_SIDE: // fireball barrage
        case TARGET_UNIT_ENEMY:
        case TARGET_UNIT:
            triggerCaster = GetCaster();
            if (!triggerCaster)
                triggerCaster = triggerTarget;
            triggerTarget = triggerCaster->GetTarget(); // This will default to channel target for channels
            break;
        case TARGET_UNIT_FRIEND: // Abolish Disease / Poison confirms this
        case TARGET_UNIT_CASTER:
            triggerCaster = GetTarget();
            triggerTarget = GetTarget();
            break;
        case TARGET_UNIT_CHANNEL_TARGET: // Electrified net
            triggerCaster = GetCaster();
            triggerTarget = dynamic_cast<Unit*>(triggerCaster->GetChannelObject());
            break;
        case TARGET_LOCATION_CHANNEL_TARGET_DEST:
            triggerCaster = GetCaster();
            if (triggerCaster)
                triggerTargetObject = GetCaster()->GetChannelObject();
        case TARGET_LOCATION_CASTER_SRC: // TODO: this needs to be done whenever target isnt important, doing it per case for safety
            //[[fallthrough]]
        case TARGET_LOCATION_CASTER_DEST:
        case TARGET_UNIT_SCRIPT_NEAR_CASTER:
        default:
            triggerTarget = nullptr;
            break;
    }
}

void Aura::CastTriggeredSpell(PeriodicTriggerData& data)
{
    Spell* spell = new Spell(data.caster, data.spellInfo, TRIGGERED_OLD_TRIGGERED, data.caster->GetObjectGuid(), GetSpellProto());
    if (data.spellInfo->HasAttribute(SPELL_ATTR_EX2_RETAIN_ITEM_CAST)) // forward guid to at least spell go
        spell->SetForwardedCastItem(GetCastItemGuid());
    SpellCastTargets targets;
    if (data.spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
    {
        if (data.targetObject)
            targets.setDestination(data.targetObject->GetPositionX(), data.targetObject->GetPositionY(), data.targetObject->GetPositionZ());
        else if (data.target)
            targets.setDestination(data.target->GetPositionX(), data.target->GetPositionY(), data.target->GetPositionZ());
        else
            targets.setDestination(data.caster->GetPositionX(), data.caster->GetPositionY(), data.caster->GetPositionZ());
    }
    if (data.spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
    {
        if (data.targetObject)
            targets.setSource(data.targetObject->GetPositionX(), data.targetObject->GetPositionY(), data.targetObject->GetPositionZ());
        else if (data.target)
            targets.setSource(data.target->GetPositionX(), data.target->GetPositionY(), data.target->GetPositionZ());
        else
            targets.setSource(data.caster->GetPositionX(), data.caster->GetPositionY(), data.caster->GetPositionZ());
    }
    if (data.target)
        targets.setUnitTarget(data.target);
    if (data.basePoints[0])
        spell->m_currentBasePoints[EFFECT_INDEX_0] = data.basePoints[0];
    if (data.basePoints[1])
        spell->m_currentBasePoints[EFFECT_INDEX_1] = data.basePoints[1];
    if (data.basePoints[2])
        spell->m_currentBasePoints[EFFECT_INDEX_2] = data.basePoints[2];
    spell->SpellStart(&targets, this);
}

/*********************************************************/
/***               BASIC AURA FUNCTION                 ***/
/*********************************************************/
void Aura::HandleAddModifier(bool apply, bool Real)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER || !Real)
        return;

    if (m_modifier.m_miscvalue >= MAX_SPELLMOD)
        return;

    if (apply)
    {
        SpellEntry const* spellProto = GetSpellProto();
        uint32 priority = spellProto->spellPriority;

        // Add custom charges for some mod aura
        switch (spellProto->Id)
        {
            case 17941:                                     // Shadow Trance
            case 22008:                                     // Netherwind Focus
            case 31834:                                     // Light's Grace
            case 34754:                                     // Clearcasting
            case 34936:                                     // Backlash
            case 44401:                                     // Missile Barrage
            case 48108:                                     // Hot Streak
            case 51124:                                     // Killing Machine
            case 54741:                                     // Firestarter
            case 57761:                                     // Fireball!
            case 64823:                                     // Elune's Wrath (Balance druid t8 set
                GetHolder()->SetAuraCharges(1);
                break;
            case 16246:                                     // Clearcasting - Shaman - need to be applied before elemental mastery
                priority = 100;
                break;
        }

        m_spellmod = new SpellModifier(
            SpellModOp(m_modifier.m_miscvalue),
            SpellModType(m_modifier.m_auraname),            // SpellModType value == spell aura types
            m_modifier.m_amount,
            this,
            priority,
            // prevent expire spell mods with (charges > 0 && m_stackAmount > 1)
            // all this spell expected expire not at use but at spell proc event check
            spellProto->StackAmount > 1 ? 0 : GetHolder()->GetAuraCharges());
    }

    static_cast<Player*>(GetTarget())->AddSpellMod(m_spellmod, apply);
    if (!apply)
        m_spellmod = nullptr;

    ReapplyAffectedPassiveAuras();
}

void Aura::TriggerSpell()
{
    ObjectGuid casterGUID = GetCasterGuid();
    Unit* triggerTarget = GetTriggerTarget();

    if (!casterGUID || !triggerTarget)
        return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = GetSpellProto()->EffectTriggerSpell[m_effIndex];

    SpellEntry const* triggeredSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id);
    SpellEntry const* auraSpellInfo = GetSpellProto();
    uint32 auraId = auraSpellInfo->Id;
    Unit* target = GetTarget();
    Unit* triggerCaster = triggerTarget;
    WorldObject* triggerTargetObject = nullptr;

    // specific code for cases with no trigger spell provided in field
    if (triggeredSpellInfo == nullptr)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                switch (auraId)
                {
                    case 812:                               // Periodic Mana Burn
                    {
                        trigger_spell_id = 25779;           // Mana Burn

                        if (GetTarget()->GetTypeId() != TYPEID_UNIT)
                            return;

                        triggerTarget = ((Creature*)GetTarget())->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, trigger_spell_id, SELECT_FLAG_POWER_MANA);
                        if (!triggerTarget)
                            return;

                        break;
                    }
//                    // Polymorphic Ray
//                    case 6965: break;
                    case 9712:                              // Thaumaturgy Channel
                        if (Unit* caster = GetCaster())
                            caster->CastSpell(caster, 21029, TRIGGERED_OLD_TRIGGERED);
                        return;
//                    // Egan's Blaster
//                    case 17368: break;
//                    // Haunted
//                    case 18347: break;
//                    // Ranshalla Waiting
//                    case 18953: break;
//                    // Frostwolf Muzzle DND
//                    case 21794: break;
//                    // Alterac Ram Collar DND
//                    case 21866: break;
//                    // Celebras Waiting
//                    case 21916: break;
                    case 23170:                             // Brood Affliction: Bronze
                    {
                        // Only 10% chance of triggering spell, return for the remaining 90%
                        if (urand(0, 9) >= 1)
                            return;
                        target->CastSpell(target, 23171, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    case 23493:                             // Restoration
                    {
                        // sniff confirmed to send no packets
                        uint32 heal = triggerTarget->GetMaxHealth() / 10;
                        uint32 absorb = 0;
                        triggerTarget->CalculateHealAbsorb(heal, &absorb);
                        triggerTarget->ModifyHealth(int32(heal - absorb));
                        if (int32 mana = triggerTarget->GetMaxPower(POWER_MANA))
                        {
                            mana /= 10;
                            triggerTarget->ModifyPower(POWER_MANA, mana);
                        }
                        return;
                    }
//                    // Stoneclaw Totem Passive TEST
//                    case 23792: break;
//                    // Axe Flurry
//                    case 24018: break;
//                    // Restoration
//                    case 24379: break;
//                    // Happy Pet
//                    case 24716: break;
                    case 24743:                             // Cannon Prep
                    case 24832:                             // Cannon Prep
                    case 42825:                             // Cannon Prep
                        trigger_spell_id = auraId == 42825 ? 42868 : 24731;
                        break;
                    case 24780:                             // Dream Fog
                    {
                        // Note: In 1.12 triggered spell 24781 still exists, need to script dummy effect for this spell then
                        // Select an unfriendly enemy in 100y range and attack it
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        ThreatList const& tList = target->getThreatManager().getThreatList();
                        for (auto itr : tList)
                        {
                            Unit* pUnit = target->GetMap()->GetUnit(itr->getUnitGuid());

                            if (pUnit && target->getThreatManager().getThreat(pUnit))
                                target->getThreatManager().modifyThreatPercent(pUnit, -100);
                        }

                        if (Unit* pEnemy = target->SelectRandomUnfriendlyTarget(target->GetVictim(), 100.0f))
                            ((Creature*)target)->AI()->AttackStart(pEnemy);

                        return;
                    }
//                    // Cannon Prep
//                    case 24832: break;
                    case 24834:                             // Shadow Bolt Whirl
                    {
                        uint32 spellForTick[8] = { 24820, 24821, 24822, 24823, 24835, 24836, 24837, 24838 };
                        uint32 tick = (GetAuraTicks() + 7/*-1*/) % 8;

                        // casted in left/right (but triggered spell have wide forward cone)
                        float forward = target->GetOrientation();
                        if (tick <= 3)
                            target->SetOrientation(forward + 0.75f * M_PI_F - tick * M_PI_F / 8);       // Left
                        else
                            target->SetOrientation(forward - 0.75f * M_PI_F + (8 - tick) * M_PI_F / 8); // Right

                        triggerTarget->CastSpell(triggerTarget, spellForTick[tick], TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        target->SetOrientation(forward);
                        return;
                    }
//                    // Stink Trap
//                    case 24918: break;
//                    // Agro Drones
//                    case 25152: break;
                    case 25371:                             // Consume
                    {
                        int32 bpDamage = triggerTarget->GetMaxHealth() * 10 / 100;
                        triggerTarget->CastCustomSpell(triggerTarget, 25373, &bpDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        return;
                    }
//                    // Pain Spike
//                    case 25572: break;
//                    // Consume
//                    case 26196: break;
//                    // Berserk
//                    case 26615: break;
//                    // Defile
//                    case 27177: break;
//                    // Teleport: IF/UC
//                    case 27601: break;
//                    // Five Fat Finger Exploding Heart Technique
//                    case 27673: break;
                    case 27746:                             // Nitrous Boost
                    {
                        if (triggerTarget->GetPower(POWER_MANA) >= 10)
                        {
                            triggerTarget->ModifyPower(POWER_MANA, -10);
                            triggerTarget->SendEnergizeSpellLog(triggerTarget, 27746, -10, POWER_MANA);
                        }
                        else
                        {
                            triggerTarget->RemoveAurasDueToSpell(27746);
                        }
                        return;
                    }
                    case 27747:                             // Steam Tank Passive
                    {
                        if (triggerTarget->GetPower(POWER_MANA) <= 90)
                        {
                            triggerTarget->ModifyPower(POWER_MANA, 10);
                            triggerTarget->SendEnergizeSpellLog(triggerTarget, 27747, 10, POWER_MANA);
                        }
                        return;
                    }
                    case 27808:                             // Frost Blast
                    {
                        int32 bpDamage = triggerTarget->GetMaxHealth() * 26 / 100;
                        triggerTarget->CastCustomSpell(triggerTarget, 29879, &bpDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        return;
                    }
                    // Detonate Mana
                    case 27819:
                    {
                        // 33% Mana Burn on normal mode, 50% on heroic mode
                        int32 bpDamage = (int32)triggerTarget->GetPower(POWER_MANA) / (triggerTarget->GetMap()->GetDifficulty() ? 2 : 3);
                        triggerTarget->ModifyPower(POWER_MANA, -bpDamage);
                        triggerTarget->CastCustomSpell(triggerTarget, 27820, &bpDamage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, triggerTarget->GetObjectGuid());
                        return;
                    }
//                    // Controller Timer
//                    case 28095: break;
                    // Stalagg Chain and Feugen Chain
                    case 28096:
                    case 28111:
                    {
                        // X-Chain is casted by Tesla to X, so: caster == Tesla, target = X
                        Unit* pCaster = GetCaster();
                        if (pCaster && pCaster->GetTypeId() == TYPEID_UNIT && !pCaster->IsWithinDistInMap(target, 60.0f))
                        {
                            pCaster->InterruptNonMeleeSpells(true);
                            ((Creature*)pCaster)->SetInCombatWithZone();
                            // Stalagg Tesla Passive or Feugen Tesla Passive
                            pCaster->CastSpell(pCaster, auraId == 28096 ? 28097 : 28109, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, target->GetObjectGuid());
                        }
                        return;
                    }
                    // Stalagg Tesla Passive and Feugen Tesla Passive
                    case 28097:
                    case 28109:
                    {
                        // X-Tesla-Passive is casted by Tesla on Tesla with original caster X, so: caster = X, target = Tesla
                        Unit* pCaster = GetCaster();
                        if (pCaster && pCaster->GetTypeId() == TYPEID_UNIT)
                        {
                            if (pCaster->GetVictim() && !pCaster->IsWithinDistInMap(target, 60.0f))
                            {
                                if (Unit* pTarget = ((Creature*)pCaster)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    target->CastSpell(pTarget, 28099, TRIGGERED_NONE);// Shock
                            }
                            else
                            {
                                // "Evade"
                                target->RemoveAurasDueToSpell(auraId);
                                target->CombatStop(true);
                                // Recast chain (Stalagg Chain or Feugen Chain
                                target->CastSpell(pCaster, auraId == 28097 ? 28096 : 28111, TRIGGERED_NONE);
                            }
                        }
                        return;
                    }
//                    // Mark of Didier
//                    case 28114: break;
//                    // Communique Timer, camp
//                    case 28346: break;
//                    // Icebolt
//                    case 28522: break;
//                    // Silithyst
//                    case 29519: break;
                    case 29528:                             // Inoculate Nestlewood Owlkin
                        // prevent error reports in case ignored player target
                        if (triggerTarget->GetTypeId() != TYPEID_UNIT)
                            return;
                        break;
//                    // Return Fire
//                    case 29788: break;
//                    // Return Fire
//                    case 29793: break;
//                    // Return Fire
//                    case 29794: break;
//                    // Guardian of Icecrown Passive
//                    case 29897: break;
                    case 29917:                             // Feed Captured Animal
                        trigger_spell_id = 29916;
                        break;
//                    // Mind Exhaustion Passive
//                    case 30025: break;
//                    // Nether Beam - Serenity
//                    case 30401: break;
                    case 30427:                             // Extract Gas
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;
                        // move loot to player inventory and despawn target
                        if (caster->GetTypeId() == TYPEID_PLAYER &&
                                triggerTarget->GetTypeId() == TYPEID_UNIT &&
                                ((Creature*)triggerTarget)->GetCreatureInfo()->CreatureType == CREATURE_TYPE_GAS_CLOUD)
                        {
                            Player* player = (Player*)caster;
                            Creature* creature = (Creature*)triggerTarget;
                            // missing lootid has been reported on startup - just return
                            if (!creature->GetCreatureInfo()->SkinningLootId)
                                return;

                            Loot loot(player, creature->GetCreatureInfo()->SkinningLootId, LOOT_SKINNING);
                            loot.AutoStore(player);

                            creature->ForcedDespawn();
                        }
                        return;
                    }
                    case 30576:                             // Quake
                        trigger_spell_id = 30571;
                        break;
//                    // Burning Maul
//                    case 30598: break;
//                    // Regeneration
//                    case 30799:
//                    case 30800:
//                    case 30801:
//                        break;
//                    // Despawn Self - Smoke cloud
//                    case 31269: break;
//                    // Time Rift Periodic
//                    case 31320: break;
//                    // Corrupt Medivh
//                    case 31326: break;
                    case 31373:                             // Spellcloth
                    {
                        // Summon Elemental after create item
                        triggerTarget->SummonCreature(17870, 0.0f, 0.0f, 0.0f, triggerTarget->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);
                        return;
                    }
                    case 31611:                             // Bloodmyst Tesla
                        // no custom effect required; return to avoid spamming with errors
                        return;
                    case 31944:                             // Doomfire
                    {
                        int32 damage = m_modifier.m_amount * ((GetAuraDuration() + m_modifier.periodictime) / GetAuraMaxDuration());
                        triggerTarget->CastCustomSpell(triggerTarget, 31969, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        return;
                    }
//                    // Teleport Test
//                    case 32236: break;
                    case 32686:                             // Earthquake
                        if (urand(0, 1)) // 50% chance to trigger
                            triggerTarget->CastSpell(nullptr, 13360, TRIGGERED_OLD_TRIGGERED);
                        break;
//                    // Possess
//                    case 33401: break;
//                    // Draw Shadows
//                    case 33563: break;
//                    // Murmur's Touch
//                    case 33711: break;
                    case 34229:                             // Flame Quills
                    {
                        // cast 24 spells 34269-34289, 34314-34316
                        for (uint32 spell_id = 34269; spell_id != 34290; ++spell_id)
                            triggerTarget->CastSpell(triggerTarget, spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        for (uint32 spell_id = 34314; spell_id != 34317; ++spell_id)
                            triggerTarget->CastSpell(triggerTarget, spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        return;
                    }
                    case 35268:                             // Inferno (normal and heroic)
                    case 39346:
                    {
                        int32 damage = auraSpellInfo->EffectBasePoints[0];
                        triggerTarget->CastCustomSpell(triggerTarget, 35283, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                        return;
                    }
//                    // Tornado
//                    case 34683: break;
//                    // Frostbite Rotate
//                    case 34748: break;
                    case 34821:                               // Arcane Flurry (Melee Component)
                    {
                        trigger_spell_id = 34824;       // (Range Component)

                        if (GetTarget()->GetTypeId() != TYPEID_UNIT)
                            return;

                        triggerTarget = ((Creature*)GetTarget())->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, trigger_spell_id, SELECT_FLAG_PLAYER);
                        if (!triggerTarget)
                            return;

                        break;
                    }
//                    // Interrupt Shutdown
//                    case 35016: break;
//                    // Interrupt Shutdown
//                    case 35176: break;
//                    // Salaadin's Tesla
                    case 35515:
                        return;
//                    // Ethereal Channel (Red)
//                    case 35518: break;
//                    // Nether Vapor
//                    case 35879: break;
//                    // Dark Portal Storm
//                    case 36018: break;
//                    // Burning Maul
//                    case 36056: break;
//                    // Living Grove Defender Lifespan
//                    case 36061: break;
//                    // Professor Dabiri Talks
//                    case 36064: break;
//                    // Kael Gaining Power
                    case 36091:
                    {
                        switch (GetAuraTicks())
                        {
                            case 1:
                                target->CastSpell(target, 36364, TRIGGERED_OLD_TRIGGERED);
                                target->PlayDirectSound(27);
                                target->PlayDirectSound(1136);
                                break;
                            case 2:
                                target->RemoveAurasDueToSpell(36364);
                                target->CastSpell(target, 36370, TRIGGERED_OLD_TRIGGERED);
                                target->PlayDirectSound(27);
                                target->PlayDirectSound(1136);
                                break;
                            case 3:
                                target->RemoveAurasDueToSpell(36370);
                                target->CastSpell(target, 36371, TRIGGERED_OLD_TRIGGERED);
                                target->PlayDirectSound(27);
                                target->PlayDirectSound(1136);
                                break;
                            case 4:
                                if (target->GetTypeId() == TYPEID_UNIT && target->AI())
                                    target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, static_cast<Creature*>(target));
                                break;
                        }
                        break;
                    }
//                    // They Must Burn Bomb Aura
//                    case 36344: break;        
                    case 36350:                             // They Must Burn Bomb Aura (self)
                        trigger_spell_id = 36325;           // They Must Burn Bomb Drop (DND)
                        break;
//                    // Stolen Ravenous Ravager Egg
//                    case 36401: break;
//                    // Activated Cannon
//                    case 36410: break;
//                    // Stolen Ravenous Ravager Egg
//                    case 36418: break;
//                    // Enchanted Weapons
//                    case 36510: break;
//                    // Cursed Scarab Periodic
//                    case 36556: break;
//                    // Cursed Scarab Despawn Periodic
//                    case 36561: break;
//                    // Vision Guide
                    case 36573: return; // atm implemented in dbscripts
                    // Cannon Charging (platform)
//                    case 36785: break;
//                    // Cannon Charging (self)
//                    case 36860: break;
                    case 37027:                             // Remote Toy
                        if (urand(0, 4) == 0)               // 20% chance to apply trigger spell
                            trigger_spell_id = 37029;
                        else
                            return;
                        break;
//                    // Mark of Death
//                    case 37125: break;
                    case 37268:                               // Arcane Flurry (Melee Component)
                    {
                        trigger_spell_id = 37271;       // (Range Component, parentspell 37269)

                        if (GetTarget()->GetTypeId() != TYPEID_UNIT)
                            return;

                        triggerTarget = ((Creature*)GetTarget())->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, trigger_spell_id, SELECT_FLAG_PLAYER);
                        if (!triggerTarget)
                            return;

                        break;
                    }
//                    // Karazhan - Chess NPC AI, Snapshot timer
//                    case 37440: break;
//                    // Karazhan - Chess NPC AI, action timer
//                    case 37504: break;
//                    // Banish
//                    case 37546: break;
//                    // Shriveling Gaze
//                    case 37589: break;
//                    // Fake Aggro Radius (2 yd)
//                    case 37815: break;
//                    // Corrupt Medivh
//                    case 37853: break;
                    case 38495:                             // Eye of Grillok
                    {
                        target->CastSpell(target, 38530, TRIGGERED_OLD_TRIGGERED);
                        return;
                    }
                    case 38554:                             // Absorb Eye of Grillok (Zezzak's Shard)
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        if (Unit* caster = GetCaster())
                            caster->CastSpell(caster, 38495, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        else
                            return;

                        Creature* creatureTarget = (Creature*)target;

                        creatureTarget->ForcedDespawn();
                        return;
                    }
//                    // Magic Sucker Device timer
//                    case 38672: break;
//                    // Tomb Guarding Charging
//                    case 38751: break;
                    // Murmur's Touch
                    case 33711:
                        switch (GetAuraTicks())
                        {
                            case 7:
                            case 10:
                            case 12:
                            case 13:
                            case 14:
                                target->CastSpell(target, 33760, TRIGGERED_OLD_TRIGGERED);
                                break;
                        }
                        return;
                    // Murmur's Touch
                    case 38794:
                        switch (GetAuraTicks())
                        {
                            case 3:
                            case 6:
                            case 7:
                                target->CastSpell(target, 33760, TRIGGERED_OLD_TRIGGERED);
                                break;
                        }
                        return;
                    case 39105:                             // Activate Nether-wraith Beacon (31742 Nether-wraith Beacon item)
                    {
                        float fX, fY, fZ;
                        triggerTarget->GetClosePoint(fX, fY, fZ, triggerTarget->GetObjectBoundingRadius(), 20.0f);
                        Creature* wraith = triggerTarget->SummonCreature(22408, fX, fY, fZ, triggerTarget->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0);
                        if (Unit* caster = GetCaster())
                            wraith->AI()->AttackStart(caster);
                        return;
                    }
//                    // Drain World Tree Visual
//                    case 39140: break;
//                    // Quest - Dustin's Undead Dragon Visual aura
//                    case 39259: break;
//                    // Hellfire - The Exorcism, Jules releases darkness, aura
//                    case 39306: break;
//                    // Enchanted Weapons
//                    case 39489: break;
//                    // Shadow Bolt Whirl
//                    case 39630: break;
//                    // Shadow Bolt Whirl
//                    case 39634: break;
//                    // Shadow Inferno
                    case 39645:
                    {
                        int32 damageForTick[8] = { 1000, 1000, 2000, 2000, 3000, 3000, 4000, 5000 };
                        triggerTarget->CastCustomSpell(triggerTarget, 39646, &damageForTick[GetAuraTicks() - 1], nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr);
                        return;
                    }
                    break;
                    case 39857:                             // Tear of Azzinoth Summon Channel - it's not really supposed to do anything,and this only prevents the console spam
                        trigger_spell_id = 39856;
                        break;
//                    // Soulgrinder Ritual Visual (Smashed)
//                    case 39974: break;
//                    // Simon Game Pre-game timer
//                    case 40041: break;
//                    // Knockdown Fel Cannon: The Aggro Check Aura
//                    case 40113: break;
                    case 40157:                             // Spirit Lance - Teron Gorefiend - Remove stack on tick
                        target->RemoveAuraStack(40157);
                        return;
                    case 40398:                             // Demon Transform 2
                        if (UnitAI* ai = target->AI())
                            ai->SendAIEvent(AI_EVENT_CUSTOM_C, target, target, GetAuraTicks());
                        break;
                    case 40511:                             // Demon Transform 1
                        if (UnitAI* ai = target->AI())
                            ai->SendAIEvent(AI_EVENT_CUSTOM_B, target, target, GetAuraTicks());
                        break;
//                    // Ethereal Ring Cannon: Cannon Aura
//                    case 40734: break;
                    case 40760:                             // Cage Trap
                        if (GetAuraTicks() == 1)
                            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_F, target, (Creature*)target);
                        break;
                    case 40867: // Random Periodic
                    {
                        // casts random beam
                        static const uint32 beams[] = { 40827, 40859, 40860, 40861 };
                        trigger_spell_id = beams[urand(0, 3)];
                        if (target->GetTypeId() == TYPEID_UNIT)
                            triggerTarget = static_cast<Creature*>(target)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, trigger_spell_id, SELECT_FLAG_PLAYER);
                        return;
                    }
//                    // Prismatic Shield
//                    case 40879: break;
                    case 41404:                             // Dementia
                        switch (urand(0, 1))
                        {
                            case 0:
                                trigger_spell_id = 41406;
                                break;
                            case 1:
                                trigger_spell_id = 41409;
                                break;
                        }
                        break;
                    case 41350:                             // Aura of Desire
                    {
                        if (Aura* aura = GetHolder()->m_auras[EFFECT_INDEX_1])
                        {
                            if (aura->m_modifier.m_amount > -100)
                            {
                                UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + m_modifier.m_miscvalue);
                                GetTarget()->HandleStatModifier(unitMod, TOTAL_PCT, float(aura->m_modifier.m_amount), false);
                                aura->m_modifier.m_amount -= 5;
                                GetTarget()->HandleStatModifier(unitMod, TOTAL_PCT, float(aura->m_modifier.m_amount), true);
                            }
                        }
                        return;
                    }
//                    // Chaos Form
//                    case 41629: break;
                    case 42177:                             // Alert Drums
                        if (GetAuraTicks() < 3 || GetAuraTicks() > 8)
                        {
                            if (target->GetTypeId() == TYPEID_UNIT && target->AI())
                                target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, static_cast<Creature*>(target));
                        }
                        break;
                    case 42581:                             // Spout (left)
                    case 42582:                             // Spout (right)
                    {
                        float newAngle = target->GetOrientation();

                        if (auraId == 42581)
                            newAngle += 2 * M_PI_F / 100;
                        else
                            newAngle -= 2 * M_PI_F / 100;

                        newAngle = MapManager::NormalizeOrientation(newAngle);

                        target->SetFacingTo(newAngle);

                        target->CastSpell(target, auraSpellInfo->CalculateSimpleValue(m_effIndex), TRIGGERED_OLD_TRIGGERED);
                        return;
                    }
                    case 44035:                             // Return to the Spirit Realm
                        trigger_spell_id = 44036;
                        break;
//                    // Curse of Boundless Agony
//                    case 45050: break;
//                    // Earthquake
//                    case 46240: break;
                    case 46736:                             // Personalized Weather
                        switch (urand(0, 1))
                        {
                            case 0:
                                return;
                            case 1:
                                trigger_spell_id = 46737;
                                break;
                        }
                        break;
//                    // Stay Submerged
//                    case 46981: break;
//                    // Dragonblight Ram
//                    case 47015: break;
//                    // Party G.R.E.N.A.D.E.
//                    case 51510: break;
//                    // Horseman Abilities
//                    case 52347: break;
//                    // GPS (Greater drake Positioning System)
//                    case 53389: break;
//                    // WotLK Prologue Frozen Shade Summon Aura
//                    case 53459: break;
//                    // WotLK Prologue Frozen Shade Speech
//                    case 53460: break;
//                    // WotLK Prologue Dual-plagued Brain Summon Aura
//                    case 54295: break;
//                    // WotLK Prologue Dual-plagued Brain Speech
//                    case 54299: break;
//                    // Rotate 360 (Fast)
//                    case 55861: break;
//                    // Shadow Sickle
//                    case 56702: break;
//                    // Draw Magic
//                    case 58185: break;
                    case 58886:                             // Food
                    {
                        if (GetAuraTicks() != 1)
                            return;

                        uint32 randomBuff[5] = {57288, 57139, 57111, 57286, 57291};

                        trigger_spell_id = urand(0, 1) ? 58891 : randomBuff[urand(0, 4)];

                        break;
                    }
//                    // Shadow Sickle
//                    case 59103: break;
//                    // Time Bomb
//                    case 59376: break;
//                    // Whirlwind Visual
//                    case 59551: break;
//                    // Hearstrike
//                    case 59783: break;
//                    // Z Check
//                    case 61678: break;
//                    // IsDead Check
//                    case 61976: break;
//                    // Start the Engine
//                    case 62432: break;
//                    // Enchanted Broom
//                    case 62571: break;
//                    // Mulgore Hatchling
//                    case 62586: break;
                    case 62679:                             // Durotar Scorpion
                        trigger_spell_id = auraSpellInfo->CalculateSimpleValue(m_effIndex);
                        break;
//                    // Fighting Fish
//                    case 62833: break;
//                    // Shield Level 1
//                    case 63130: break;
//                    // Shield Level 2
//                    case 63131: break;
//                    // Shield Level 3
//                    case 63132: break;
//                    // Food
                    case 64345:                             // Remove Player from Phase
                        target->RemoveSpellsCausingAura(SPELL_AURA_PHASE);
                        return;
//                    case 64445: break;
//                    // Food
//                    case 65418: break;
//                    // Food
//                    case 65419: break;
//                    // Food
//                    case 65420: break;
//                    // Food
//                    case 65421: break;
//                    // Food
//                    case 65422: break;
//                    // Rolling Throw
//                    case 67546: break;
//                    // Ice Tomb
//                    case 70157: break;
//                    // Mana Barrier                       // HANDLED IN SD2!
//                    case 70842: break;
//                    // Summon Timer: Suppresser
//                    case 70912: break;
//                    // Aura of Darkness
//                    case 71110: break;
//                    // Aura of Darkness
//                    case 71111: break;
                    case 71441:                             // Unstable Ooze Explosion Suicide Trigger
                        target->Suicide();
                        return;
//                    // Ball of Flames Visual
//                    case 71706: break;
//                    // Summon Broken Frostmourne
//                    case 74081: break;
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                /*switch (auraId)
                {
                    default:
                        break;
                }*/
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                switch(auraId)
                {
                    case 23410:                             // Wild Magic (Mage class call in Nefarian encounter)
                    {
                        trigger_spell_id = 23603;
                        break;
                    }
//                    // Corrupted Totems
//                    case 23425: break;
                    default:
                        break;
                }
                break;
            }
//            case SPELLFAMILY_PRIEST:
//            {
//                switch(auraId)
//                {
//                    // Blue Beam
//                    case 32930: break;
//                    // Fury of the Dreghood Elders
//                    case 35460: break;
//                    default:
//                        break;
//                }
//                break;
//            }
            case SPELLFAMILY_HUNTER:
            {
                switch (auraId)
                {
                    case 53302:                             // Sniper training
                    case 53303:
                    case 53304:
                        if (triggerTarget->GetTypeId() != TYPEID_PLAYER)
                            return;

                        // Reset reapply counter at move
                        if (triggerTarget->IsMoving())
                        {
                            m_modifier.m_amount = 6;
                            return;
                        }

                        // We are standing at the moment
                        if (m_modifier.m_amount > 0)
                        {
                            --m_modifier.m_amount;
                            return;
                        }

                        // select rank of buff
                        switch (auraId)
                        {
                            case 53302: trigger_spell_id = 64418; break;
                            case 53303: trigger_spell_id = 64419; break;
                            case 53304: trigger_spell_id = 64420; break;
                        }

                        // If aura is active - no need to continue
                        if (triggerTarget->HasAura(trigger_spell_id))
                            return;

                        break;
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                switch (auraId)
                {
                    case 768:                               // Cat Form
                        // trigger_spell_id not set and unknown effect triggered in this case, ignoring for while
                        return;
                    case 22842:                             // Frenzied Regeneration
                    case 22895:
                    case 22896:
                    case 26999:
                    {
                        int32 LifePerRage = GetModifier()->m_amount;

                        int32 lRage = target->GetPower(POWER_RAGE);
                        if (lRage > 100)                    // rage stored as rage*10
                            lRage = 100;
                        target->ModifyPower(POWER_RAGE, -lRage);
                        int32 FRTriggerBasePoints = int32(lRage * LifePerRage / 10);
                        target->CastCustomSpell(target, 22845, &FRTriggerBasePoints, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    default:
                        break;
                }
                break;
            }

//            case SPELLFAMILY_HUNTER:
//            {
//                switch(auraId)
//                {
//                    // Frost Trap Aura
//                    case 13810:
//                        return;
//                    // Rizzle's Frost Trap
//                    case 39900:
//                        return;
//                    // Tame spells
//                    case 19597:         // Tame Ice Claw Bear
//                    case 19676:         // Tame Snow Leopard
//                    case 19677:         // Tame Large Crag Boar
//                    case 19678:         // Tame Adult Plainstrider
//                    case 19679:         // Tame Prairie Stalker
//                    case 19680:         // Tame Swoop
//                    case 19681:         // Tame Dire Mottled Boar
//                    case 19682:         // Tame Surf Crawler
//                    case 19683:         // Tame Armored Scorpid
//                    case 19684:         // Tame Webwood Lurker
//                    case 19685:         // Tame Nightsaber Stalker
//                    case 19686:         // Tame Strigid Screecher
//                    case 30100:         // Tame Crazed Dragonhawk
//                    case 30103:         // Tame Elder Springpaw
//                    case 30104:         // Tame Mistbat
//                    case 30647:         // Tame Barbed Crawler
//                    case 30648:         // Tame Greater Timberstrider
//                    case 30652:         // Tame Nightstalker
//                        return;
//                    default:
//                        break;
//                }
//                break;
//            }
            //case SPELLFAMILY_SHAMAN:
            //{
            //    break;
            //}
            default:
                break;
        }

        // Reget trigger spell proto
        triggeredSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id);
    }
    else 
    {
        // for channeled spell cast applied from aura owner to channel target (persistent aura affects already applied to true target)
        // come periodic casts applied to targets, so need select proper caster (ex. 15790)
        // interesting 2 cases: periodic aura at caster of channeled spell
        if (target->GetObjectGuid() == casterGUID)
            triggerCaster = target;

        PickTargetsForSpellTrigger(triggerCaster, triggerTarget, triggerTargetObject, triggeredSpellInfo);

        // Spell exist but require custom code
        switch (auraId)
        {
            case 9347:                                      // Mortal Strike
            {
                if (target->GetTypeId() != TYPEID_UNIT)
                    return;
                // expected selection current fight target
                triggerTarget = ((Creature*)target)->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, triggeredSpellInfo);
                if (!triggerTarget)
                    return;

                break;
            }
            case 1010:                                      // Curse of Idiocy
            {
                // TODO: spell casted by result in correct way mostly
                // BUT:
                // 1) target show casting at each triggered cast: target don't must show casting animation for any triggered spell
                //      but must show affect apply like item casting
                // 2) maybe aura must be replace by new with accumulative stat mods instead stacking

                // prevent cast by triggered auras
                if (casterGUID == triggerTarget->GetObjectGuid())
                    return;

                // stop triggering after each affected stats lost > 90
                int32 intelectLoss = 0;
                int32 spiritLoss = 0;

                Unit::AuraList const& mModStat = triggerTarget->GetAurasByType(SPELL_AURA_MOD_STAT);
                for (auto i : mModStat)
                {
                    if (i->GetId() == 1010)
                    {
                        switch (i->GetModifier()->m_miscvalue)
                        {
                            case STAT_INTELLECT: intelectLoss += i->GetModifier()->m_amount; break;
                            case STAT_SPIRIT:    spiritLoss   += i->GetModifier()->m_amount; break;
                            default: break;
                        }
                    }
                }

                if (intelectLoss <= -90 && spiritLoss <= -90)
                    return;

                break;
            }
            case 16191:                                     // Mana Tide
            {
                triggerCaster->CastCustomSpell(nullptr, trigger_spell_id, &m_modifier.m_amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 29768:                                     // Overload
            {
                int32 damage = m_modifier.m_amount * (pow(2.0f, GetAuraTicks()));
                if (damage > 3200)
                    damage = 3200;
                triggerCaster->CastCustomSpell(triggerTarget, triggeredSpellInfo, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, casterGUID);
                return;
            }
            case 32930:                                     // Blue beam
                return; // Never seems to go off in sniffs - hides errors
            /*case 30502:                                   // Dark Spin - Only Effect0 s.30505 should be affect, else s.30508 doesnt work anymore
            {
                if (GetCaster()->GetTypeId() != TYPEID_UNIT)
                    return;

                triggerTarget = ((Creature*)GetCaster())->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, trigger_spell_id, SELECT_FLAG_PLAYER);
                if (!triggerTarget)
                    return;

                break;
            }*/
            case 36716:                                     // Energy Discharge
            case 38828:
            {
                if (urand(0, 1) == 0) // 50% chance to proc
                    break;
                return;
            }
            case 36720:                                     // Kael'Thas - Phoenix - Burn
            case 44197:                                     // MgT Kael'Thas - Phoenix - Burn
            {
                uint32 dmg = target->GetMaxHealth() * 0.05f; // 5% dmg every tick
                uint32 absorb = 0;
                Unit::DealDamageMods(target, target, dmg, &absorb, SELF_DAMAGE);
                Unit::DealDamage(target, target, dmg, nullptr, SELF_DAMAGE, SPELL_SCHOOL_MASK_FIRE, nullptr, false);
                break; // continue executing rest
            }
            case 37716:                                     // Demon Link
                triggerTarget = static_cast<TemporarySpawn*>(target)->GetSpawner();
                break;
            case 37850:                                     // Watery Grave
            case 38023:
            case 38024:
            case 38025:
            {
                casterGUID = target->GetObjectGuid();
                break;
            }
            case 38652:                                     // Spore Cloud
            case 40106:                                     // Merge
            {
                triggerCaster = GetCaster();
                break;
            }
            // dummy trigger 18350 family
            case 28820:                                     // Lightning Shield
            case 38443:                                     // Totemic Mastery
            {
                triggerTarget = target;
                break;
            }
            case 38736:                                     // Rod of Purification - for quest 10839 (Veil Skith: Darkstone of Terokk)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(triggerTarget, trigger_spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 28059:                                     // Positive Charge
            case 28084:                                     // Negative Charge
            case 39088:                                     // Positive Charge
            case 39091:                                     // Negative Charge
            {
                uint32 buffAuraId;
                float range;
                switch (auraId)
                {
                    case 28059:
                        buffAuraId = 29659;
                        range = 13.f;
                        break;
                    case 28084:
                        buffAuraId = 29660;
                        range = 13.f;
                        break;
                    case 39088:
                        buffAuraId = 39089;
                        range = 10.f;
                        break;
                    default:
                    case 39091:
                        buffAuraId = 39092;
                        range = 10.f;
                        break;
                }
                uint32 curCount = 0;
                PlayerList playerList;
                GetPlayerListWithEntryInWorld(playerList, target, range); // official range
                for (Player* player : playerList)
                    if (target != player && player->HasAura(auraId))
                        curCount++;

                target->RemoveAurasDueToSpell(buffAuraId);
                if (curCount)
                    for (uint32 i = 0; i < curCount; i++)
                        target->CastSpell(target, buffAuraId, TRIGGERED_OLD_TRIGGERED);

                break;
            }
            case 36657:                                     // Death Count
            case 38818:                                     // Death Count
            {
                Unit* caster = GetCaster(); // should only go off if caster is still alive
                if (!caster || !caster->IsAlive())
                    return;
                break;
            }
            case 37098:                                     // Rain of Bones - Nightbane
            {
                switch (GetAuraTicks()) // on some aura ticks also spawn skeletons
                {
                    case 2:
                    case 7:
                    case 13:
                    case 16:
                    case 20:
                        target->CastSpell(triggerTargetObject->GetPositionX(), triggerTargetObject->GetPositionY(), triggerTargetObject->GetPositionZ(), 30170, TRIGGERED_OLD_TRIGGERED);
                        break;
                }
                break;
            }
            case 39575:                                     // Charge Frenzy
            {
                Unit* caster = GetCaster();
                triggerTarget = ((Creature*)caster)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, triggeredSpellInfo);
                if (!triggerTarget)
                    return;
                caster->FixateTarget(triggerTarget);
                break;
            }
            case 40862: // Sinful Periodic
            case 40863: // Sinister Periodic
            case 40865: // Vile Periodic
            case 40866: // Wicked Periodic
            {
                if (target->GetTypeId() == TYPEID_UNIT)
                    triggerTarget = static_cast<Creature*>(target)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, trigger_spell_id, SELECT_FLAG_PLAYER);
                break;
            }
            case 43149:                                     // Claw Rage
            {
                // Need to provide explicit target for trigger spell target combination
                target->CastSpell(target->GetVictim(), trigger_spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 47702:                                     // Unholy Union
            case 47722:                                     // Frost Draw
            case 50251:                                     // Blood Draw
            {
                // TODO: Determine if we need to scan effect 1 and 2 for default prefill
                triggerCaster = GetCaster();
                triggerTarget = target;
                break;
            }
            case 53563:                                     // Beacon of Light
                // original caster must be target (beacon)
                target->CastSpell(target, trigger_spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this, target->GetObjectGuid());
                return;
        }
    }
    int32 basePoints[] = { 0,0,0 };
    PeriodicTriggerData data(triggerCaster, triggerTarget, triggerTargetObject, triggeredSpellInfo, basePoints);
    OnPeriodicTrigger(data);

    // All ok cast by default case
    if (data.spellInfo)
    {
        CastTriggeredSpell(data);
    }
    else if (!GetAuraScript()) // if scripter scripted spell, it is handled somehow
    {
        if (Unit* caster = GetCaster())
        {
            if (triggerTarget->GetTypeId() != TYPEID_UNIT || !sScriptDevAIMgr.OnEffectDummy(caster, GetId(), GetEffIndex(), (Creature*)triggerTarget, ObjectGuid()))
                sLog.outError("Aura::TriggerSpell: Spell %u have 0 in EffectTriggered[%d], not handled custom case?", GetId(), GetEffIndex());
        }
    }
}

void Aura::TriggerSpellWithValue()
{
    ObjectGuid casterGUID = GetCasterGuid();
    Unit* triggerTarget = GetTriggerTarget();

    if (!casterGUID || !triggerTarget)
        return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = GetSpellProto()->EffectTriggerSpell[m_effIndex];
    int32 calculatedAmount = GetModifier()->m_amount;

    SpellEntry const* triggeredSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id);
    int32 basePoints[3] = { 0,0,0 };
    // damage triggered from spell might not only be processed by first effect (but always EffectDieSides equal 1)
    if (triggeredSpellInfo)
    {
        uint8 j = 0;
        for (uint8 i = 0; i < 3; ++i)
        {
            if (triggeredSpellInfo->EffectDieSides[i] == 1)
                j = i;
        }
        basePoints[j] = calculatedAmount;
    }
    Unit* triggerCaster = triggerTarget;
    WorldObject* triggerTargetObject = nullptr;
    PickTargetsForSpellTrigger(triggerCaster, triggerTarget, triggerTargetObject, triggeredSpellInfo);

    PeriodicTriggerData data(triggerCaster, triggerTarget, triggerTargetObject, triggeredSpellInfo, basePoints);
    OnPeriodicTrigger(data);

    if (data.spellInfo)
        CastTriggeredSpell(data);
}

/*********************************************************/
/***                  AURA EFFECTS                     ***/
/*********************************************************/

void Aura::HandleAuraDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    // AT APPLY
    if (apply)
    {
        switch (GetSpellProto()->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                switch (GetId())
                {
                    case 11403:                             // Dream Vision
                    {
                        if (target->IsPlayer())
                        {
                            Unit* pet = static_cast<Player*>(target)->GetCharm();
                            if (pet && pet->GetEntry() == 7863)
                                pet->SetVisibility(VISIBILITY_OFF);
                        }
                        break;
                    }
                    case 1515:                              // Tame beast
                        if (Unit* caster = GetCaster()) // Wotlk - sniff - adds 1000 threat
                            target->AddThreat(caster, 1000.0f, false, GetSpellSchoolMask(GetSpellProto()), GetSpellProto());
                        return;
                    case 6946:                              // Curse of the Bleakheart
                    case 41170:
                        m_isPeriodic = true;
                        m_modifier.periodictime = 5 * IN_MILLISECONDS;
                        m_periodicTimer = m_modifier.periodictime;
                        return;
                    case 7057:                              // Haunting Spirits
                        // expected to tick with 30 sec period (tick part see in Aura::PeriodicTick)
                        m_isPeriodic = true;
                        m_modifier.periodictime = 30 * IN_MILLISECONDS;
                        m_periodicTimer = m_modifier.periodictime;
                        return;
                    case 7054:                              // Forsaken Skill
                        m_isPeriodic = true;
                        m_modifier.periodictime = 1 * IN_MILLISECONDS;
                        m_periodicTimer = m_modifier.periodictime;
                        break;
                    case 13139:                             // net-o-matic
                        // root to self part of (root_target->charge->root_self sequence
                        if (Unit* caster = GetCaster())
                            caster->CastSpell(caster, 13138, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 21094:                             // Separation Anxiety (Majordomo Executus)
                    case 23487:                             // Separation Anxiety (Garr)
                    {
                        // expected to tick with 5 sec period (tick part see in Aura::PeriodicTick)
                        m_isPeriodic = true;
                        m_modifier.periodictime = 5 * IN_MILLISECONDS;
                        m_periodicTimer = m_modifier.periodictime;
                        return;
                    }
                    case 23183:                             // Mark of Frost
                    {
                        if (target->HasAura(23182))
                            target->CastSpell(target, 23186, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        return;
                    }
                    case 25042:                             // Mark of Nature
                    {
                        if (target->HasAura(25040))
                            target->CastSpell(target, 25043, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        return;
                    }
                    case 25471:                             // Attack Order
                    {
                        target->CastSpell(nullptr, 25473, TRIGGERED_OLD_TRIGGERED);
                        return;
                    }
                    case 37127:                             // Mark of Death
                    {
                        if (target->HasAura(37128))
                            target->CastSpell(target, 37131, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        return;
                    }
                    case 26681:                             // Cologne
                    {
                        if (Unit* target = GetTarget())
                        {
                            if (target->HasAura(26682))
                                target->RemoveAurasDueToSpell(26682);
                        }
                        return;
                    }
                    case 26682:                             // Perfume
                    {
                        if (Unit* target = GetTarget())
                        {
                            if (target->HasAura(26681))
                                target->RemoveAurasDueToSpell(26681);
                        }
                        return;
                    }
                    case 28832:                             // Mark of Korth'azz
                    case 28833:                             // Mark of Blaumeux
                    case 28834:                             // Mark of Rivendare
                    case 28835:                             // Mark of Zeliek
                    {
                        int32 damage;
                        switch (GetStackAmount())
                        {
                            case 1:
                                return;
                            case 2: damage =   500; break;
                            case 3: damage =  1500; break;
                            case 4: damage =  4000; break;
                            case 5: damage = 12500; break;
                            default:
                                damage = 14000 + 1000 * GetStackAmount();
                                break;
                        }

                        if (Unit* caster = GetCaster())
                            caster->CastCustomSpell(target, 28836, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    case 31736:                                     // Ironvine Seeds
                    {
                        Unit* pCaster = GetCaster();

                        Creature* SteamPumpOverseer = target->SummonCreature(18340, pCaster->GetPositionX()-20, pCaster->GetPositionY()+20, pCaster->GetPositionZ(), target->GetOrientation(), TEMPSPAWN_TIMED_OOC_DESPAWN, 10000);

                        if (SteamPumpOverseer && pCaster)
                            SteamPumpOverseer->GetMotionMaster()->MovePoint(0, pCaster->GetPositionX(), pCaster->GetPositionY(), pCaster->GetPositionZ());

                        return;
                    }
                    case 32044:                             // Soul Charge - Archimonde - Battle For Mount Hyjal
                    {
                        if (target->IsAlive())
                            return;


                        Unit* pCaster = GetCaster();
                        uint8 targetClass = target->getClass();
                        switch (targetClass)
                        {
                            case 1:
                            case 2:
                            case 4:
                                target->CastSpell(pCaster, 32045, TRIGGERED_NONE);
                                break;
                            case 3:
                            case 7:
                            case 11:
                                target->CastSpell(pCaster, 32051, TRIGGERED_NONE);
                                break;
                            case 5:
                            case 8:
                            case 9:
                                target->CastSpell(pCaster, 32052, TRIGGERED_NONE);
                                break;
                        }

                        return;
                    }
                    case 32045:                             // Soul Charge
                    case 32051:
                    case 32052:
                    {
                        // TODO: Controls the random timer for the spell to be cast based on aura, they need to be split up.
                        return;
                    }
                    case 32441:                             // Brittle Bones
                    {
                        m_isPeriodic = true;
                        m_modifier.periodictime = 10 * IN_MILLISECONDS; // randomly applies Rattled 32437
                        m_periodicTimer = 0;
                        return;
                    }
                    case 33326:                             // Stolen Soul Dispel
                    {
                        target->RemoveAurasDueToSpell(32346);
                        return;
                    }
                    case 36550:                             // Floating Drowned
                    {
                        // Possibly need some of the below to fix Vengeful Harbinger flying

                        //if (Unit* caster = GetCaster())
                        //{
                        //    caster->SetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                        //    caster->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                        //    caster->SetHover(true);
                        //    caster->SetLevitate(true);
                        //    caster->SetCanFly(true);
                        //}
                        return;
                    }
                    case 36089:
                    case 36090:                             // Netherbeam - Kaelthas
                    {
                        float speed = target->GetBaseRunSpeed(); // fetch current base speed
                        target->ApplyModPositiveFloatValue(OBJECT_FIELD_SCALE_X, float(m_modifier.m_amount) / 100, apply);
                        target->UpdateModelData(); // resets speed
                        target->SetBaseRunSpeed(speed + (1.f / 7.f));
                        target->UpdateSpeed(MOVE_RUN, true); // sends speed packet
                        return;
                    }
                    case 36587:                             // Vision Guide
                    {
                        target->CastSpell(target, 36573, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    // Gender spells
                    case 38224:                             // Illidari Agent Illusion
                    case 37096:                             // Blood Elf Illusion
                    case 46354:                             // Blood Elf Illusion
                    {
                        uint8 gender = target->getGender();
                        uint32 spellId;
                        switch (GetId())
                        {
                            case 38224: spellId = (gender == GENDER_MALE ? 38225 : 38227); break;
                            case 37096: spellId = (gender == GENDER_MALE ? 37093 : 37095); break;
                            case 46354: spellId = (gender == GENDER_MALE ? 46355 : 46356); break;
                            default: return;
                        }
                        target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    case 37408:                             // Oscillating field
                        if (Player* player = dynamic_cast<Player*>(target))
                            if (player->GetAuraCount(37408) >= 5 && player->GetQuestStatus(10594) == QUEST_STATUS_INCOMPLETE)
                                player->AreaExploredOrEventHappens(10594);
                        return;
                    case 37750:                             // Clear Consuming Madness
                        if (target->HasAura(37749))
                            target->RemoveAurasDueToSpell(37749);
                        return;
                    case 39850:                             // Rocket Blast
                        if (roll_chance_i(20))              // backfire stun
                            target->CastSpell(target, 51581, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 40607:                             // Fixate - Shade of Akama
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        target->FixateTarget(GetCaster());
                        return;
                    }
                    case 40856:                                     // Wrangling Rope
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        if (Unit* caster = GetCaster())
                            target->CastSpell(caster, 40917, TRIGGERED_NONE); // Wrangle Aether Rays: Character Force Cast

                        static_cast<Creature*>(target)->ForcedDespawn();

                        return;
                    }
                    case 40926:                                     // Wrangle Aether Rays: Wrangling Rope Channel
                    {
                        if (target->GetTypeId() != TYPEID_PLAYER)
                            return;

                        if (Unit* caster = GetCaster())
                            caster->GetMotionMaster()->MoveFollow(target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE, true);

                        return;
                    }
                    case 40952:                             // Ethereal Ring, Cannon Death Visual, start
                        target->CastSpell(nullptr, 40868, TRIGGERED_OLD_TRIGGERED);
                        return;
                    case 42416:                             // Apexis Mob Faction Check Aura
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        if (target->GetPositionX() > 3000.f)
                            ((Creature*)target)->UpdateEntry(22243);
                        else
                            ((Creature*)target)->UpdateEntry(23386);
                        return;
                    }
                    case 43873:                             // Headless Horseman Laugh
                        target->PlayDistanceSound(11965);
                        return;
                    case 45963:                             // Call Alliance Deserter
                    {
                        // Escorting Alliance Deserter
                        if (target->GetMiniPet())
                            target->CastSpell(target, 45957, TRIGGERED_OLD_TRIGGERED);

                        return;
                    }
                    case 46699:                             // Requires No Ammo
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            // not use ammo and not allow use
                            ((Player*)target)->RemoveAmmo();
                        return;
                    case 47190:                             // Toalu'u's Spiritual Incense
                        target->CastSpell(target, 47189, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        // allow script to process further (text)
                        break;
                    case 47563:                             // Freezing Cloud
                        target->CastSpell(target, 47574, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 47593:                             // Freezing Cloud
                        target->CastSpell(target, 47594, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 48025:                             // Headless Horseman's Mount
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 51621, 48024, 51617, 48023, 0);
                        return;
                    case 48143:                             // Forgotten Aura
                        // See Death's Door
                        target->CastSpell(target, 48814, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 51405:                             // Digging for Treasure
                        target->HandleEmote(EMOTE_STATE_WORK);
                        // Pet will be following owner, this makes him stop
                        target->addUnitState(UNIT_STAT_STUNNED);
                        return;
                    case 54729:                             // Winged Steed of the Ebon Blade
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 54726, 54727, 0);
                        return;
                    case 58600:                             // Restricted Flight Area (Dalaran)
                    case 58730:                             // Restricted Flight Area (Wintergrasp)
                    {
                        if (!target || target->GetTypeId() != TYPEID_PLAYER)
                            return;
                        const char* text = sObjectMgr.GetMangosString(LANG_NO_FLY_ZONE, ((Player*)target)->GetSession()->GetSessionDbLocaleIndex());
                        target->MonsterWhisper(text, target, true);
                        return;
                    }
                    case 62061:                             // Festive Holiday Mount
                        if (target->HasAuraType(SPELL_AURA_MOUNTED))
                            // Reindeer Transformation
                            target->CastSpell(target, 25860, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 62109:                             // Tails Up: Aura
                        target->setFaction(1990);           // Ambient (hostile)
                        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        return;
                    case 63122:                             // Clear Insane
                        target->RemoveAurasDueToSpell(GetSpellProto()->CalculateSimpleValue(m_effIndex));
                        return;
                    case 63624:                             // Learn a Second Talent Specialization
                        // Teach Learn Talent Specialization Switches, required for client triggered casts, allow after 30 sec delay
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            ((Player*)target)->learnSpell(63680, false);
                        return;
                    case 63651:                             // Revert to One Talent Specialization
                        // Teach Learn Talent Specialization Switches, remove
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            ((Player*)target)->removeSpell(63680);
                        return;
                    case 64132:                             // Constrictor Tentacle
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            target->CastSpell(target, 64133, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 65684:                             // Dark Essence
                        target->RemoveAurasDueToSpell(65686);
                        return;
                    case 65686:                             // Light Essence
                        target->RemoveAurasDueToSpell(65684);
                        return;
                    case 68912:                             // Wailing Souls
                        if (Unit* caster = GetCaster())
                        {
                            caster->SetTarget(target);

                            // TODO - this is confusing, it seems the boss should channel this aura, and start casting the next spell
                            caster->CastSpell(caster, 68899, TRIGGERED_NONE);
                        }
                        return;
                    case 71342:                             // Big Love Rocket
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 71344, 71345, 71346, 71347, 0);
                        return;
                    case 71563:                             // Deadly Precision
                        target->CastSpell(target, 71564, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 72286:                             // Invincible
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 72281, 72282, 72283, 72284, 0);
                        return;
                    case 74856:                             // Blazing Hippogryph
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 74854, 74855, 0);
                        return;
                    case 75614:                             // Celestial Steed
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 75619, 75620, 75617, 75618, 76153);
                        return;
                    case 75973:                             // X-53 Touring Rocket
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 75957, 75972, 76154);
                        return;
                }
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                switch (GetId())
                {
                    case 23427:                             // Summon Infernals (Warlock class call in Nefarian encounter)
                    {
                        if (Unit* target = GetTarget())
                            target->CastSpell(target, 23426, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    }
                    case 41099:                             // Battle Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Battle Aura
                        target->CastSpell(target, 41106, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32614);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 41100:                             // Berserker Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Berserker Aura
                        target->CastSpell(target, 41107, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32614);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 41101:                             // Defensive Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Defensive Aura
                        target->CastSpell(target, 41105, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32604);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 31467);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 53790:                             // Defensive Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Defensive Aura
                        target->CastSpell(target, 41105, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 39384);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 53791:                             // Berserker Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Berserker Aura
                        target->CastSpell(target, 41107, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 53792:                             // Battle Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // Battle Aura
                        target->CastSpell(target, 41106, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43623);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                }
                break;
            }
            case SPELLFAMILY_MAGE:
                break;
            case SPELLFAMILY_HUNTER:
            {
                switch (GetId())
                {
                    case 34026:                             // Kill Command
                        target->CastSpell(target, 34027, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                    case 35079:                             // Misdirection, triggered buff
                        target->RemoveAurasDueToSpell(35079, GetHolder(), AURA_REMOVE_BY_STACK);
                        return;
                }
                break;
            }
            //case SPELLFAMILY_PALADIN:
            //{
            //    break;
            //}
            case SPELLFAMILY_SHAMAN:
            {
                switch (GetId())
                {
                    case 55198:                             // Tidal Force
                        target->CastSpell(target, 55166, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        return;
                }
                break;
            }
            case SPELLFAMILY_PRIEST:
            {
                switch (GetId())
                {
                    case 30238:             // Lordaeron's Blessing
                    {
                        target->CastSpell(target, 31906, TRIGGERED_OLD_TRIGGERED);
                        return;
                    }
                }
                break;
            }
        }
    }
    // AT REMOVE
    else
    {
        if (IsQuestTameSpell(GetId()) && target->IsAlive())
        {
            Unit* caster = GetCaster();
            if (!caster || !caster->IsAlive())
                return;

            uint32 finalSpellId = 0;
            switch (GetId())
            {
                case 19548: finalSpellId = 19597; break;
                case 19674: finalSpellId = 19677; break;
                case 19687: finalSpellId = 19676; break;
                case 19688: finalSpellId = 19678; break;
                case 19689: finalSpellId = 19679; break;
                case 19692: finalSpellId = 19680; break;
                case 19693: finalSpellId = 19684; break;
                case 19694: finalSpellId = 19681; break;
                case 19696: finalSpellId = 19682; break;
                case 19697: finalSpellId = 19683; break;
                case 19699: finalSpellId = 19685; break;
                case 19700: finalSpellId = 19686; break;
                case 30646: finalSpellId = 30647; break;
                case 30653: finalSpellId = 30648; break;
                case 30654: finalSpellId = 30652; break;
                case 30099: finalSpellId = 30100; break;
                case 30102: finalSpellId = 30103; break;
                case 30105: finalSpellId = 30104; break;
            }

            if (finalSpellId)
                caster->CastSpell(target, finalSpellId, TRIGGERED_OLD_TRIGGERED, nullptr, this);

            return;
        }

        switch (GetId())
        {
            case 11129:                                     // Combustion
                target->RemoveAurasDueToSpell(28682); // on Combustion removal remove crit % stacks
                return;
            case 11826:                                     // Electromagnetic Gigaflux Reactivator
                if (m_removeMode != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* caster = GetCaster())
                {
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                        caster->CastSpell(target, 11828, TRIGGERED_OLD_TRIGGERED, ((Player*)caster)->GetItemByGuid(this->GetCastItemGuid()), this);
                }
                return;
            case 12774:                                     // (DND) Belnistrasz Idol Shutdown Visual
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    return;

                // Idom Rool Camera Shake <- wtf, don't drink while making spellnames?
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, 12816, TRIGGERED_OLD_TRIGGERED);

                return;
            }
            case 17189:                                     // Frostwhisper's Lifeblood
                // Ras Frostwhisper gets back to full health when turned to his human form
                if (Unit* caster = GetCaster())
                    caster->ModifyHealth(caster->GetMaxHealth() - caster->GetHealth());
                return;
            case 25185:                                     // Itch
            {
                GetCaster()->CastSpell(target, 25187, TRIGGERED_OLD_TRIGGERED);
                return;
            }
            case 26077:                                     // Itch
            {
                GetCaster()->CastSpell(target, 26078, TRIGGERED_OLD_TRIGGERED);
                return;
            }
            case 28169:                                     // Mutating Injection
            {
                // Mutagen Explosion
                target->CastSpell(target, 28206, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                // Poison Cloud
                target->CastSpell(target, 28240, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 28059:                                     // Positive Charge
                target->RemoveAurasDueToSpell(29659);
                return;
            case 28084:                                     // Negative Charge
                target->RemoveAurasDueToSpell(29660);
                return;
            case 30238:                                     // Lordaeron's Blessing
            {
                target->RemoveAurasDueToSpell(31906);
                return;
            }
            case 32045:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32054, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            }
            case 32051:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32057, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            }
            case 32052:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32053, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            }
            case 35016:                                     // Interrupt shutdown
            case 35176:                                     // Interrupt shutdown (ara)
            {
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                {
                    Unit* caster = GetCaster();
                    if (caster && GetAuraDuration() <= 100) // only fail if finished cast (seems to finish with .1 seconds left)
                        if (Creature* summoner = caster->GetMap()->GetCreature(caster->GetSpawnerGuid()))
                            caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, caster, summoner);
                }
                return;
            }
            case 35079:                                     // Misdirection, triggered buff
            case 59628:                                     // Tricks of the Trade, triggered buff
            {
                uint32 spellId = GetId() == 35079 ? 34477 : 57934;
                if (Unit* pCaster = GetCaster())
                    pCaster->getHostileRefManager().ResetThreatRedirection(spellId);
                return;
            }
            case 36301:                                     // On Fire
            {
                if (!target->IsCreature())
                    return;
                float x, y, z;
                static_cast<Creature*>(target)->GetRespawnCoord(x, y, z);
                target->GetMotionMaster()->MovePoint(1, x, y, z);
                return;
            }
            case 36730:                                     // Flame Strike
            {
                target->CastSpell(target, 36731, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 39088:                                     // Positive Charge
                target->RemoveAurasDueToSpell(39089);
                return;
            case 39091:                                     // Negative Charge
                target->RemoveAurasDueToSpell(39092);
                return;
            case 39908:                                     // Eye Blast - Illidan
                if (UnitAI* ai = target->AI())
                    if (Unit* caster = GetCaster())
                        ai->SendAIEvent(AI_EVENT_CUSTOM_E, caster, caster);
                return;
            case 40401:                                     // Shade Soul Channel
                target->RemoveAuraStack(40520);
                return;
            case 40830:                                     // Banish the Demons: Banishment Beam Periodic Aura Effect
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    target->CastSpell(nullptr, 40828, TRIGGERED_OLD_TRIGGERED);
                return;
            }
            case 41099:                                     // Battle Stance
            {
                // Battle Aura
                target->RemoveAurasDueToSpell(41106);
                return;
            }
            case 41100:                                     // Berserker Stance
            {
                // Berserker Aura
                target->RemoveAurasDueToSpell(41107);
                return;
            }
            case 41101:                                     // Defensive Stance
            {
                // Defensive Aura
                target->RemoveAurasDueToSpell(41105);
                return;
            }
            case 42385:                                     // Alcaz Survey Aura
            {
                target->CastSpell(nullptr, 42316, TRIGGERED_OLD_TRIGGERED);
                return;
            }
            case 42517:                                     // Beam to Zelfrax
            {
                // expecting target to be a dummy creature
                target->SummonCreature(23864, -2985.01f, -3853.649f, 44.23183f, 5.51524f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                return;
            }
            case 43969:                                     // Feathered Charm
            {
                // Steelfeather Quest Credit, Are there any requirements for this, like area?
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 43984, TRIGGERED_OLD_TRIGGERED);

                return;
            }
            case 44191:                                     // Flame Strike
            {
                target->CastSpell(nullptr, target->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 44192 ? 44190 : 46163, TRIGGERED_OLD_TRIGGERED);
                return;
            }
            case 45934:                                     // Dark Fiend
            {
                // Kill target if dispelled - TODO: recheck suicide spell existence
                if (m_removeMode == AURA_REMOVE_BY_DISPEL)
                    Unit::DealDamage(target, target, target->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                return;
            }
            case 45963:                                     // Call Alliance Deserter
            {
                // Escorting Alliance Deserter
                target->RemoveAurasDueToSpell(45957);
                return;
            }
            case 46308:                                     // Burning Winds
            {
                // casted only at creatures at spawn
                target->CastSpell(target, 47287, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 46736:                                     // Personalized Weather
            case 46738:
            case 46739:
            case 46740:
            {
                uint32 spellId = 0;
                switch (urand(0, 5))
                {
                    case 0: spellId = 46736; break;
                    case 1: spellId = 46738; break;
                    case 2: spellId = 46739; break;
                    case 3: spellId = 46740; break;
                    case 4: return;
                }
                target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
                break;
            }
            case 50141:                                     // Blood Oath
            {
                // Blood Oath
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 50001, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            }
            case 51405:                                     // Digging for Treasure
            {
                const uint32 spell_list[7] =
                {
                    51441,                                  // hare
                    51397,                                  // crystal
                    51398,                                  // armor
                    51400,                                  // gem
                    51401,                                  // platter
                    51402,                                  // treasure
                    51443                                   // bug
                };

                target->CastSpell(target, spell_list[urand(0, 6)], TRIGGERED_OLD_TRIGGERED);

                target->HandleEmote(EMOTE_STATE_NONE);
                target->clearUnitState(UNIT_STAT_STUNNED);
                return;
            }
            case 51870:                                     // Collect Hair Sample
            {
                if (Unit* pCaster = GetCaster())
                {
                    if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                        pCaster->CastSpell(target, 51872, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }

                return;
            }
            case 52098:                                     // Charge Up
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 52092, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            }
            case 53790:                                     // Defensive Stance
            {
                // Defensive Aura
                target->RemoveAurasDueToSpell(41105);
                return;
            }
            case 53791:                                     // Berserker Stance
            {
                // Berserker Aura
                target->RemoveAurasDueToSpell(41107);
                return;
            }
            case 53792:                                     // Battle Stance
            {
                // Battle Aura
                target->RemoveAurasDueToSpell(41106);
                return;
            }
            case 56511:                                     // Towers of Certain Doom: Tower Bunny Smoke Flare Effect
            {
                // Towers of Certain Doom: Skorn Cannonfire
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                    target->CastSpell(target, 43069, TRIGGERED_OLD_TRIGGERED);

                return;
            }
            case 58600:                                     // Restricted Flight Area (Dalaran)
            case 58730:                                     // Restricted Flight Area (Wintergrasp)
            {
                AreaTableEntry const* area = GetAreaEntryByAreaID(target->GetAreaId());

                // Dalaran restricted flight zone (recheck before apply unmount)
                if (area && target->GetTypeId() == TYPEID_PLAYER && ((GetId() == 58600 && area->flags & AREA_FLAG_CANNOT_FLY) || (GetId() == 58730 && area->flags & AREA_FLAG_OUTDOOR_PVP)) &&
                        ((Player*)target)->IsFreeFlying() && !((Player*)target)->IsGameMaster())
                {
                    target->CastSpell(target, 58601, TRIGGERED_OLD_TRIGGERED); // Remove Flight Auras (also triggered Parachute (45472))
                }
                return;
            }
            case 61900:                                     // Electrical Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0), TRIGGERED_OLD_TRIGGERED);

                return;
            }
            case 70308:                                     // Mutated Transformation
            {
                if (target->GetMap()->IsDungeon())
                {
                    uint32 spellId;

                    Difficulty diff = target->GetMap()->GetDifficulty();
                    if (diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC)
                        spellId = 70311;
                    else
                        spellId = 71503;

                    target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }
                return;
            }
        }

        // Living Bomb
        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_MAGE && (GetSpellProto()->SpellFamilyFlags & uint64(0x2000000000000)))
        {
            if (m_removeMode == AURA_REMOVE_BY_EXPIRE || m_removeMode == AURA_REMOVE_BY_DISPEL)
                target->CastSpell(target, m_modifier.m_amount, TRIGGERED_OLD_TRIGGERED, nullptr, this);

            return;
        }
    }

    // AT APPLY & REMOVE

    switch (GetSpellProto()->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (GetId())
            {
                case 6606:                                  // Self Visual - Sleep Until Cancelled (DND)
                {
                    if (apply)
                    {
                        target->SetStandState(UNIT_STAND_STATE_SLEEP);
                        target->addUnitState(UNIT_STAT_ROOT);
                    }
                    else
                    {
                        target->clearUnitState(UNIT_STAT_ROOT);
                        target->SetStandState(UNIT_STAND_STATE_STAND);
                    }

                    return;
                }
                case 11196:                                 // Recently Bandaged
                    target->ApplySpellImmune(this, IMMUNITY_MECHANIC, GetMiscValue(), apply);
                    return;
                case 16093:                                  // Self Visual - Sleep Until Cancelled (DND)
                {
                    if (apply)
                    {
                        target->SetStandState(UNIT_STAND_STATE_SLEEP);
                        target->addUnitState(UNIT_STAT_ROOT);
                    }
                    else
                    {
                        target->clearUnitState(UNIT_STAT_ROOT);
                        target->SetStandState(UNIT_STAND_STATE_STAND);
                    }

                    return;
                }
                case 24658:                                 // Unstable Power
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        caster->CastSpell(target, 24659, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                    }
                    else
                        target->RemoveAurasDueToSpell(24659);
                    return;
                }
                case 24661:                                 // Restless Strength
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        caster->CastSpell(target, 24662, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                    }
                    else
                        target->RemoveAurasDueToSpell(24662);
                    return;
                }
                case 28819:                                 // Submerge Visual
                {
                    if (apply)
                        target->SetStandState(UNIT_STAND_STATE_CUSTOM);
                    else
                        target->SetStandState(UNIT_STAND_STATE_STAND);
                    return;
                }
                case 29266:                                 // Permanent Feign Death
                case 31261:                                 // Permanent Feign Death (Root)
                case 37493:                                 // Feign Death
                case 52593:                                 // Bloated Abomination Feign Death
                case 55795:                                 // Falling Dragon Feign Death
                case 57626:                                 // Feign Death
                case 57685:                                 // Permanent Feign Death
                case 58768:                                 // Permanent Feign Death (Freeze Jumpend)
                case 58806:                                 // Permanent Feign Death (Drowned Anim)
                case 58951:                                 // Permanent Feign Death
                case 64461:                                 // Permanent Feign Death (No Anim) (Root)
                case 65985:                                 // Permanent Feign Death (Root Silence Pacify)
                case 70592:                                 // Permanent Feign Death
                case 70628:                                 // Permanent Feign Death
                case 70630:                                 // Frozen Aftermath - Feign Death
                case 71598:                                 // Feign Death
                {
                    // Unclear what the difference really is between them.
                    // Some has effect1 that makes the difference, however not all.
                    // Some appear to be used depending on creature location, in water, at solid ground, in air/suspended, etc
                    // For now, just handle all the same way
                    //if (target->GetTypeId() == TYPEID_UNIT)
                    target->SetFeignDeath(apply, GetCasterGuid(), GetId());

                    return;
                }
                case 32096:                                 // Thrallmar's Favor
                case 32098:                                 // Honor Hold's Favor
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (apply) // cast/remove Buffbot Buff Effect
                            target->CastSpell(target, 32172, TRIGGERED_NONE);
                        else
                            target->RemoveAurasDueToSpell(32172);
                    }
                    return;
                case 32567:                                 // Green Banish State
                {
                    target->SetHover(apply);
                    return;
                }
                case 35519:                                 // White Beam
                {
                    target->SetLevitate(apply);
                    target->SetHover(apply);
                    return;
                }
                case 35356:                                 // Spawn Feign Death
                case 35357:                                 // Spawn Feign Death
                case 42557:                                 // Feign Death
                case 51329:                                 // Feign Death
                {
                    // UNIT_DYNFLAG_DEAD does not appear with these spells.
                    // All of the spells appear to be present at spawn and not used to feign in combat or similar.
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->SetFeignDeath(apply, GetCasterGuid(), GetId(), false);

                    return;
                }
                case 37025: // Coilfang Water
                {
                    if (apply)
                    {
                        if (InstanceData* pInst = target->GetInstanceData())
                        {
                            Player* playerTarget = (Player*)target;
                            if (pInst->CheckConditionCriteriaMeet(playerTarget, INSTANCE_CONDITION_ID_LURKER, nullptr, CONDITION_FROM_HARDCODED))
                            {
                                if (pInst->CheckConditionCriteriaMeet(playerTarget, INSTANCE_CONDITION_ID_SCALDING_WATER, nullptr, CONDITION_FROM_HARDCODED))
                                    playerTarget->CastSpell(playerTarget, 37284, TRIGGERED_OLD_TRIGGERED);
                                else
                                {
                                    m_isPeriodic = true;
                                    m_modifier.periodictime = 2 * IN_MILLISECONDS; // Summons Coilfang Frenzy
                                    m_periodicTimer = 0;
                                }
                            }
                        }
                        return;
                    }
                    else
                        target->RemoveAurasDueToSpell(37284);
                }
                case 40133:                                 // Summon Fire Elemental
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit* owner = caster->GetOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (apply)
                            owner->CastSpell(owner, 8985, TRIGGERED_OLD_TRIGGERED);
                        else
                            ((Player*)owner)->RemovePet(PET_SAVE_REAGENTS);
                    }
                    return;
                }
                case 40132:                                 // Summon Earth Elemental
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit* owner = caster->GetOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (apply)
                            owner->CastSpell(owner, 19704, TRIGGERED_OLD_TRIGGERED);
                        else
                            ((Player*)owner)->RemovePet(PET_SAVE_REAGENTS);
                    }
                    return;
                }
                case 42515:                                 // Jarl Beam
                {
                    // aura animate dead (fainted) state for the duration, but we need to animate the death itself (correct way below?)
                    if (Unit* pCaster = GetCaster())
                        pCaster->ApplyModFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH, apply);

                    // Beam to Zelfrax at remove
                    if (!apply)
                        target->CastSpell(target, 42517, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                case 42520:                                 // Spirit Drained - Zul'Jin
                {
                    target->SetFeignDeath(apply, GetCasterGuid(), GetId());
                    return;
                }
                case 34719:                                 // Fixate
                case 42583:                                 // Claw Rage
                case 68987:                                 // Pursuit
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    if (apply)
                        caster->FixateTarget(target);
                    else
                        caster->FixateTarget(nullptr);

                    if (GetId() == 42583)
                    {
                        if (apply)
                            caster->CastSpell(nullptr, 43149, TRIGGERED_OLD_TRIGGERED);
                        else
                            caster->RemoveAurasDueToSpell(43149);
                    }
                    return;
                }
                case 43874:                                 // Scourge Mur'gul Camp: Force Shield Arcane Purple x3
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER, apply);
                    if (apply)
                        target->addUnitState(UNIT_STAT_ROOT);
                    return;
                case 47178:                                 // Plague Effect Self
                    target->SetFleeing(apply, GetCasterGuid(), GetId());
                    return;
                case 50053:                                 // Centrifuge Shield
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER, apply);
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC, apply);
                    return;
                case 50241:                                 // Evasive Charges
                    target->ModifyAuraState(AURA_STATE_UNKNOWN22, apply);
                    return;
                case 56422:                                 // Nerubian Submerge
                case 70733:                                 // Stoneform
                    // not known if there are other things todo, only flag are confirmed valid
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE, apply);
                    return;
                case 58204:                                 // LK Intro VO (1)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        // Play part 1
                        if (apply)
                            target->PlayDirectSound(14970, PlayPacketParameters(PLAY_TARGET, (Player*)target));
                        // continue in 58205
                        else
                            target->CastSpell(target, 58205, TRIGGERED_OLD_TRIGGERED);
                    }
                    return;
                case 58205:                                 // LK Intro VO (2)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        // Play part 2
                        if (apply)
                            target->PlayDirectSound(14971, PlayPacketParameters(PLAY_TARGET, (Player*)target));
                        // Play part 3
                        else
                            target->PlayDirectSound(14972, PlayPacketParameters(PLAY_TARGET, (Player*)target));
                    }
                    return;
                case 27978:
                case 40131:
                    if (apply)
                        target->m_AuraFlags |= UNIT_AURAFLAG_ALIVE_INVISIBLE;
                    else
                        target->m_AuraFlags &= ~UNIT_AURAFLAG_ALIVE_INVISIBLE;
                    return;
                case 66936:                                     // Submerge
                case 66948:                                     // Submerge
                    if (apply)
                        target->CastSpell(target, 66969, TRIGGERED_OLD_TRIGGERED);
                    else
                        target->RemoveAurasDueToSpell(66969);
                    return;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARLOCK:
        {
            // Haunt
            if (GetSpellProto()->SpellIconID == 3172 && (GetSpellProto()->SpellFamilyFlags & uint64(0x0004000000000000)))
            {
                // NOTE: for avoid use additional field damage stored in dummy value (replace unused 100%
                if (apply)
                    m_modifier.m_amount = 0;                // use value as damage counter instead redundant 100% percent
                else
                {
                    int32 bp0 = m_modifier.m_amount;

                    if (Unit* caster = GetCaster())
                        target->CastCustomSpell(caster, 48210, &bp0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (GetId())
            {
                case 52610:                                 // Savage Roar
                {
                    if (apply)
                    {
                        if (target->GetShapeshiftForm() != FORM_CAT)
                            return;

                        target->CastSpell(target, 62071, TRIGGERED_OLD_TRIGGERED);
                    }
                    else
                        target->RemoveAurasDueToSpell(62071);
                    return;
                }
                case 61336:                                 // Survival Instincts
                {
                    if (apply)
                    {
                        if (!target->IsNoWeaponShapeShift())
                            return;

                        int32 bp0 = int32(target->GetMaxHealth() * m_modifier.m_amount / 100);
                        target->CastCustomSpell(target, 50322, &bp0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                    }
                    else
                        target->RemoveAurasDueToSpell(50322);
                    return;
                }
            }
            // Lifebloom
            if (GetSpellProto()->SpellFamilyFlags & uint64(0x1000000000))
            {
                if (apply)
                {
                    if (Unit* caster = GetCaster())
                    {
                        // prevent double apply bonuses
                        if (target->GetTypeId() != TYPEID_PLAYER || !((Player*)target)->GetSession()->PlayerLoading())
                        {
                            m_modifier.m_amount = caster->SpellHealingBonusDone(target, GetSpellProto(), m_modifier.m_amount, SPELL_DIRECT_DAMAGE);
                            m_modifier.m_amount = target->SpellHealingBonusTaken(caster, GetSpellProto(), m_modifier.m_amount, SPELL_DIRECT_DAMAGE);
                        }
                    }
                }
                else
                {
                    // Final heal on duration end
                    if (m_removeMode != AURA_REMOVE_BY_EXPIRE && m_removeMode != AURA_REMOVE_BY_DISPEL)
                        return;

                    // final heal
                    if (target->IsInWorld())
                    {
                        int32 amount = m_modifier.m_amount;
                        target->CastCustomSpell(nullptr, 33778, &amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, GetCasterGuid());

                        if (Unit* caster = GetCaster())
                        {
                            int32 returnmana = (GetSpellProto()->ManaCostPercentage * caster->GetCreateMana() / 100) * GetStackAmount() / 2;
                            caster->CastCustomSpell(caster, 64372, &returnmana, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this, GetCasterGuid());
                        }
                    }
                }
                return;
            }

            // Improved Moonkin Form
            if (GetSpellProto()->SpellIconID == 2855)
            {
                uint32 spell_id;
                switch (GetId())
                {
                    case 48384: spell_id = 50170; break;    // Rank 1
                    case 48395: spell_id = 50171; break;    // Rank 2
                    case 48396: spell_id = 50172; break;    // Rank 3
                    default:
                        sLog.outError("HandleAuraDummy: Not handled rank of IMF (Spell: %u)", GetId());
                        return;
                }

                if (apply)
                {
                    if (target->GetShapeshiftForm() != FORM_MOONKIN)
                        return;

                    target->CastSpell(target, spell_id, TRIGGERED_OLD_TRIGGERED);
                }
                else
                    target->RemoveAurasDueToSpell(spell_id);
                return;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
            switch (GetId())
            {
                case 57934:                                 // Tricks of the Trade, main spell
                {
                    if (apply)
                        GetHolder()->SetAuraCharges(1);     // not have proper charges set in spell data
                    else
                    {
                        // used for direct in code aura removes and spell proc event charges expire
                        if (m_removeMode != AURA_REMOVE_BY_DEFAULT)
                            target->getHostileRefManager().ResetThreatRedirection(57934);
                    }
                    return;
                }
            }
            break;
        case SPELLFAMILY_HUNTER:
        {
            switch (GetId())
            {
                case 34477:                                 // Misdirection, main spell
                {
                    if (apply)
                        GetHolder()->SetAuraCharges(1);     // not have proper charges set in spell data
                    else
                    {
                        // used for direct in code aura removes and spell proc event charges expire
                        if (m_removeMode != AURA_REMOVE_BY_DEFAULT)
                        {
                            if (Unit* misdirectTarget = target->getHostileRefManager().GetThreatRedirectionTarget(34477))
                                misdirectTarget->RemoveAurasDueToSpell(35079);
                            target->getHostileRefManager().ResetThreatRedirection(34477);
                        }
                    }
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
            switch (GetId())
            {
                case 20911:                                 // Blessing of Sanctuary
                case 25899:                                 // Greater Blessing of Sanctuary
                {
                    if (apply)
                        target->CastSpell(target, 67480, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    else
                        target->RemoveAurasDueToSpell(67480);
                    return;
                }
            }
            break;
        case SPELLFAMILY_DEATHKNIGHT:
            switch (GetId())
            {
                // Raise ally
                case 46619:
                {
                    // at this point the ghoul is already spawned
                    Unit* caster = GetCaster();
                    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* player = static_cast<Player*>(caster);

                    if (apply)
                    {
                        player->SetGhouled(true);
                    }
                    else
                    {
                        player->SetGhouled(false);

                        // this will reset death timer in the client
                        WorldPacket data(SMSG_FORCED_DEATH_UPDATE);
                        player->GetSession()->SendPacket(data);
                        player->ResetDeathTimer();
                    }
                }
            }
            break;
        case SPELLFAMILY_PRIEST:
        {
            switch (GetId())
            {
                case 36414: // Focused Bursts
                case 38985:
                {
                    if (apply)
                        target->clearUnitState(UNIT_STAT_MELEE_ATTACKING);
                    else
                        target->addUnitState(UNIT_STAT_MELEE_ATTACKING);
                    return;
                }
            }
            break;
        }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(GetId(), m_effIndex))
    {
        if (apply)
            target->AddPetAura(petSpell);
        else
            target->RemovePetAura(petSpell);
        return;
    }

    if (target->IsBoarded() && target->GetTransportInfo()->IsOnVehicle())
    {
        if (IsSpellHaveAura(GetSpellProto(), SPELL_AURA_CONTROL_VEHICLE))
        {
            // TODO maybe move GetVehicleInfo() to WorldObject class
            auto vehicle = static_cast<Unit*>(target->GetTransportInfo()->GetTransport());
            auto vehicleInfo = vehicle->GetVehicleInfo();

            if (!apply)
            {
                //sLog.outString("Unboarding %s %s from %s %s", target->GetName(), target->GetGuidStr().c_str(), vehicle->GetName(), vehicle->GetGuidStr().c_str());
                vehicleInfo->UnBoard(target, false);
            }
        }
    }

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        SpellAreaForAreaMapBounds saBounds = sSpellMgr.GetSpellAreaForAuraMapBounds(GetId());
        if (saBounds.first != saBounds.second)
        {
            uint32 zone, area;
            target->GetZoneAndAreaId(zone, area);
            std::set<uint32> appliedSpells;
            std::set<uint32> removedSpells;

            Player* player = static_cast<Player*>(target);

            // custom loop for evaluation reasons
            for (SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
            {
                auto data = (*itr).second;
                uint32 spellId = data->spellId;
                // one success trumps multiple failures
                if (appliedSpells.find(spellId) != appliedSpells.end())
                    continue;

                if (data->IsFitToRequirements(player, zone, area))
                {
                    if (data->autocast && !player->HasAura(spellId))
                    {
                        appliedSpells.insert(spellId);
                        removedSpells.erase(spellId);
                    }
                }
                else if (player->HasAura(spellId))
                    removedSpells.insert(spellId);
            }

            for (auto spellId : appliedSpells)
                player->CastSpell(player, spellId, TRIGGERED_OLD_TRIGGERED);

            for (auto spellId : removedSpells)
                player->RemoveAurasDueToSpell(spellId);
        }
    }

    // script has to "handle with care", only use where data are not ok to use in the above code.
    if (target->GetTypeId() == TYPEID_UNIT)
        sScriptDevAIMgr.OnAuraDummy(this, apply);
}

void Aura::HandleAuraMounted(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(m_modifier.m_miscvalue);
        if (!ci)
        {
            sLog.outErrorDb("AuraMounted: `creature_template`='%u' not found in database (only need it modelid)", m_modifier.m_miscvalue);
            return;
        }

        uint32 display_id = Creature::ChooseDisplayId(ci);
        CreatureModelInfo const* minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
        if (minfo)
            display_id = minfo->modelid;

        m_modifier.m_amount = display_id;

        target->Mount(display_id, this);

        if (ci->VehicleTemplateId)
        {
            target->SetVehicleId(ci->VehicleTemplateId, ci->Entry);

            if (target->GetTypeId() == TYPEID_PLAYER)
                target->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
        }
    }
    else
    {
        target->Unmount(this);

        CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(m_modifier.m_miscvalue);
        if (ci && target->IsVehicle() && ci->VehicleTemplateId == target->GetVehicleInfo()->GetVehicleEntry()->m_ID)
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);

            target->SetVehicleId(0, 0);
        }
    }
}

void Aura::HandleAuraWaterWalk(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    GetTarget()->SetWaterWalk(apply);
}

void Aura::HandleAuraFeatherFall(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    GetTarget()->SetFeatherFall(apply);
}

void Aura::HandleAuraHover(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    GetTarget()->SetHover(apply);
}

void Aura::HandleWaterBreathing(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (apply || target->HasAuraType(SPELL_AURA_WATER_BREATHING))
            static_cast<Player*>(target)->SetWaterBreathingIntervalMultiplier(0);
        else
            static_cast<Player*>(target)->SetWaterBreathingIntervalMultiplier(target->GetTotalAuraMultiplier(SPELL_AURA_MOD_WATER_BREATHING));
    }
}

void Aura::HandleModWaterBreathing(bool /*apply*/, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
        static_cast<Player*>(target)->SetWaterBreathingIntervalMultiplier(target->GetTotalAuraMultiplier(SPELL_AURA_MOD_WATER_BREATHING));
}

void Aura::HandleAuraModShapeshift(bool apply, bool Real)
{
    if (!Real)
        return;

    ShapeshiftForm form = ShapeshiftForm(m_modifier.m_miscvalue);

    SpellShapeshiftFormEntry const* ssEntry = sSpellShapeshiftFormStore.LookupEntry(form);
    if (!ssEntry)
    {
        sLog.outError("Unknown shapeshift form %u in spell %u", form, GetId());
        return;
    }

    Unit* target = GetTarget();

    uint32 displayId = 0;
    if (ssEntry->modelID_A)
    {
        // i will asume that creatures will always take the defined model from the dbc
        // since no field in creature_templates describes wether an alliance or
        // horde modelid should be used at shapeshifting
        if (target->GetTypeId() != TYPEID_PLAYER)
            displayId = ssEntry->modelID_A;
        else
        {
            // players are a bit different since the dbc has seldomly an horde modelid
            if (Player::TeamForRace(target->getRace()) == HORDE)
            {
                if (ssEntry->modelID_H)
                    displayId = ssEntry->modelID_H;           // 3.2.3 only the moonkin form has this information
                else                                        // get model for race
                    displayId = sObjectMgr.GetModelForRace(ssEntry->modelID_A, target->getRaceMask());
            }

            // nothing found in above, so use default
            if (!displayId)
                displayId = ssEntry->modelID_A;
        }
    }

    if (uint32 overrideDisplayId = GetAuraScriptCustomizationValue()) // from script
        displayId = overrideDisplayId;

    switch (GetId())
    {
        case 35200: // Roc Form
            displayId = 4877;
            break;
    }

    // remove polymorph before changing display id to keep new display id
    switch (form)
    {
        case FORM_CAT:
        case FORM_TREE:
        case FORM_TRAVEL:
        case FORM_AQUA:
        case FORM_BEAR:
        case FORM_DIREBEAR:
        case FORM_FLIGHT_EPIC:
        case FORM_FLIGHT:
        case FORM_MOONKIN:
        {
            // remove movement affects
            target->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT, GetHolder(), true);
            Unit::AuraList const& slowingAuras = target->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
            for (Unit::AuraList::const_iterator iter = slowingAuras.begin(); iter != slowingAuras.end();)
            {
                SpellEntry const* aurSpellInfo = (*iter)->GetSpellProto();

                uint32 aurMechMask = GetAllSpellMechanicMask(aurSpellInfo);

                if (aurMechMask == 0) // all shapeshift removing spells have some mechanic mask. 33572 and 38132 confirmed not removed on poly
                {
                    ++iter;
                    continue;
                }

                // If spell that caused this aura has Croud Control or Daze effect
                if ((aurMechMask & MECHANIC_NOT_REMOVED_BY_SHAPESHIFT) ||
                        // some Daze spells have these parameters instead of MECHANIC_DAZE (skip snare spells)
                        (aurSpellInfo->SpellIconID == 15 && aurSpellInfo->Dispel == 0 &&
                         (aurMechMask & (1 << (MECHANIC_SNARE - 1))) == 0))
                {
                    ++iter;
                    continue;
                }

                // All OK, remove aura now
                target->RemoveAurasDueToSpellByCancel(aurSpellInfo->Id);
                iter = slowingAuras.begin();
            }

            target->RemoveAurasDueToSpell(16591); // Patch 2.0.1 - Shapeshifting removes Noggenfogger elixir
            //no break here
            break;
        }
        case FORM_DEFENSIVESTANCE:
        {
            // disable threat aura of Defiance talent when not in defensive stance
            SpellAuraHolder* defianceHolder = target->GetSpellAuraHolder(12303);
            if (!defianceHolder)
                defianceHolder = target->GetSpellAuraHolder(12788);
            if (!defianceHolder)
                defianceHolder = target->GetSpellAuraHolder(12789);
            if (defianceHolder)
            {
                if (Aura* threatAura = defianceHolder->m_auras[0])
                {
                    threatAura->GetModifier()->m_amount = apply ? threatAura->GetModifier()->m_baseAmount : 0;
                    for (int8 x = 0; x < MAX_SPELL_SCHOOL; ++x)
                        if (threatAura->GetModifier()->m_miscvalue & int32(1 << x))
                            ApplyPercentModFloatVar(target->m_threatModifier[x], float(threatAura->GetModifier()->m_baseAmount), apply);
                }
            }
            break;
        }
        default:
            break;
    }

    if (apply)
    {
        Powers PowerType = POWER_MANA;

        // remove other shapeshift before applying a new one
        target->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT, GetHolder());

        if (displayId > 0)
            target->SetDisplayId(displayId);

        // now only powertype must be set
        switch (form)
        {
            case FORM_CAT:
                PowerType = POWER_ENERGY;
                break;
            case FORM_BEAR:
            case FORM_DIREBEAR:
            case FORM_BATTLESTANCE:
            case FORM_BERSERKERSTANCE:
            case FORM_DEFENSIVESTANCE:
                PowerType = POWER_RAGE;
                break;
            default:
                break;
        }

        if (PowerType != POWER_MANA)
        {
            // reset power to default values only at power change
            if (target->GetPowerType() != PowerType)
                target->SetPowerType(PowerType);

            switch (form)
            {
                case FORM_BATTLESTANCE:
                case FORM_DEFENSIVESTANCE:
                case FORM_BERSERKERSTANCE:
                {
                    ShapeshiftForm previousForm = target->GetShapeshiftForm();
                    uint32 remainingRage = 0;
                    if (previousForm == FORM_DEFENSIVESTANCE)
                        if (Aura* aura = target->GetOverrideScript(831))
                            remainingRage += aura->GetModifier()->m_amount * 10;
                    // Stance mastery + Tactical mastery (both passive, and last have aura only in defense stance, but need apply at any stance switch)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        PlayerSpellMap const& sp_list = ((Player*)target)->GetSpellMap();
                        for (const auto& itr : sp_list)
                        {
                            if (itr.second.state == PLAYERSPELL_REMOVED)
                                continue;

                            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(itr.first);
                            if (spellInfo && spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR && spellInfo->SpellIconID == 139)
                                remainingRage += target->CalculateSpellEffectValue(target, spellInfo, EFFECT_INDEX_0) * 10;
                        }
                    }

                    if (target->GetPower(POWER_RAGE) > remainingRage)
                        target->SetPower(POWER_RAGE, remainingRage);
                    break;
                }
                default:
                    break;
            }
        }

        target->SetShapeshiftForm(form);

        // a form can give the player a new castbar with some spells.. this is a clientside process..
        // serverside just needs to register the new spells so that player isn't kicked as cheater
        if (target->GetTypeId() == TYPEID_PLAYER)
            for (unsigned int i : ssEntry->spellId)
                if (i)
                    ((Player*)target)->addSpell(i, true, false, false, false);

        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SHAPESHIFTING);
    }
    else
    {
        target->RestoreDisplayId();

        if (target->getClass() == CLASS_DRUID)
            target->SetPowerType(POWER_MANA);

        target->SetShapeshiftForm(FORM_NONE);

        switch (form)
        {
            // Nordrassil Harness - bonus
            case FORM_BEAR:
            case FORM_DIREBEAR:
            case FORM_CAT:
                if (Aura* dummy = target->GetDummyAura(37315))
                    target->CastSpell(target, 37316, TRIGGERED_OLD_TRIGGERED, nullptr, dummy);
                break;
            // Nordrassil Regalia - bonus
            case FORM_MOONKIN:
                if (Aura* dummy = target->GetDummyAura(37324))
                    target->CastSpell(target, 37325, TRIGGERED_OLD_TRIGGERED, nullptr, dummy);
                break;
            default:
                break;
        }

        // look at the comment in apply-part
        if (target->GetTypeId() == TYPEID_PLAYER)
            for (unsigned int i : ssEntry->spellId)
                if (i)
                    ((Player*)target)->removeSpell(i, false, false, false);
    }

    // adding/removing linked auras
    // add/remove the shapeshift aura's boosts
    HandleShapeshiftBoosts(apply);

    if (target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)target)->InitDataForForm();

    if (apply)
    {
        switch (form)
        {
            case FORM_CAT: // need to cast Track Humanoids if no other tracking is on
                if (target->HasSpell(5225) && !target->HasAura(2383) && !target->HasAura(2580))
                    target->CastSpell(nullptr, 5225, TRIGGERED_OLD_TRIGGERED);
                // no break
            case FORM_BEAR:
            case FORM_DIREBEAR:
            {
                // get furor proc chance
                int32 furorChance = 0;
                Unit::AuraList const& mDummy = target->GetAurasByType(SPELL_AURA_DUMMY);
                for (auto i : mDummy)
                {
                    if (i->GetSpellProto()->SpellIconID == 238)
                    {
                        furorChance = i->GetModifier()->m_amount;
                        break;
                    }
                }

                if (m_modifier.m_miscvalue == FORM_CAT)
                {
                    // Furor chance is now amount allowed to save energy for cat form
                    // without talent it reset to 0
                    if ((int32)target->GetPower(POWER_ENERGY) > furorChance)
                    {
                        target->SetPower(POWER_ENERGY, 0);
                        target->CastCustomSpell(target, 17099, &furorChance, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    }
                }
                else if (furorChance)                   // only if talent known
                {
                    target->SetPower(POWER_RAGE, 0);
                    if (irand(1, 100) <= furorChance)
                        target->CastSpell(target, 17057, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }
                break;
            }
            default:
                break;
        }
    }
}

void Aura::HandleAuraTransform(bool apply, bool Real)
{
    Unit* target = GetTarget();
    bool skipDisplayUpdate = false;
    if (apply)
    {
        if (uint32 overrideDisplayId = GetAuraScriptCustomizationValue()) // from script
            m_modifier.m_amount = overrideDisplayId;
        // special case (spell specific functionality)
        else if (m_modifier.m_miscvalue == 0)
        {
            switch (GetId())
            {
                case 42365:                                 // Murloc costume
                    m_modifier.m_amount = 21723;
                    break;
                // case 44186:                          // Gossip NPC Appearance - All, Brewfest
                // break;
                // case 48305:                          // Gossip NPC Appearance - All, Spirit of Competition
                // break;
                case 50517:                                 // Dread Corsair
                case 51926:                                 // Corsair Costume
                {
                    // expected for players
                    uint32 race = target->getRace();

                    switch (race)
                    {
                        case RACE_HUMAN:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25037 : 25048;
                            break;
                        case RACE_ORC:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25039 : 25050;
                            break;
                        case RACE_DWARF:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25034 : 25045;
                            break;
                        case RACE_NIGHTELF:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25038 : 25049;
                            break;
                        case RACE_UNDEAD:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25042 : 25053;
                            break;
                        case RACE_TAUREN:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25040 : 25051;
                            break;
                        case RACE_GNOME:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25035 : 25046;
                            break;
                        case RACE_TROLL:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25041 : 25052;
                            break;
                        case RACE_GOBLIN:                   // not really player race (3.x), but model exist
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25036 : 25047;
                            break;
                        case RACE_BLOODELF:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25032 : 25043;
                            break;
                        case RACE_DRAENEI:
                            m_modifier.m_amount = target->getGender() == GENDER_MALE ? 25033 : 25044;
                            break;
                    }
                    break;
                }
                // case 50531:                              // Gossip NPC Appearance - All, Pirate Day
                // break;
                // case 51010:                              // Dire Brew
                // break;
                // case 53806:                              // Pygmy Oil
                // break;
                // case 62847:                              // NPC Appearance - Valiant 02
                // break;
                // case 62852:                              // NPC Appearance - Champion 01
                // break;
                // case 63965:                              // NPC Appearance - Champion 02
                // break;
                // case 63966:                              // NPC Appearance - Valiant 03
                // break;
                case 65386:                                 // Honor the Dead
                case 65495:
                {
                    switch (target->getGender())
                    {
                        case GENDER_MALE:
                            m_modifier.m_amount = 29203;    // Chapman
                            break;
                        case GENDER_FEMALE:
                        case GENDER_NONE:
                            m_modifier.m_amount = 29204;    // Catrina
                            break;
                    }
                }
                // case 65511:                              // Gossip NPC Appearance - Brewfest
                // break;
                // case 65522:                              // Gossip NPC Appearance - Winter Veil
                // break;
                // case 65523:                              // Gossip NPC Appearance - Default
                // break;
                // case 65524:                              // Gossip NPC Appearance - Lunar Festival
                // break;
                // case 65525:                              // Gossip NPC Appearance - Hallow's End
                // break;
                // case 65526:                              // Gossip NPC Appearance - Midsummer
                // break;
                // case 65527:                              // Gossip NPC Appearance - Spirit of Competition
                // break;
                case 65528:                                 // Gossip NPC Appearance - Pirates' Day
                {
                    // expecting npc's using this spell to have models with race info.
                    // random gender, regardless of current gender
                    switch (target->getRace())
                    {
                        case RACE_HUMAN:
                            m_modifier.m_amount = roll_chance_i(50) ? 25037 : 25048;
                            break;
                        case RACE_ORC:
                            m_modifier.m_amount = roll_chance_i(50) ? 25039 : 25050;
                            break;
                        case RACE_DWARF:
                            m_modifier.m_amount = roll_chance_i(50) ? 25034 : 25045;
                            break;
                        case RACE_NIGHTELF:
                            m_modifier.m_amount = roll_chance_i(50) ? 25038 : 25049;
                            break;
                        case RACE_UNDEAD:
                            m_modifier.m_amount = roll_chance_i(50) ? 25042 : 25053;
                            break;
                        case RACE_TAUREN:
                            m_modifier.m_amount = roll_chance_i(50) ? 25040 : 25051;
                            break;
                        case RACE_GNOME:
                            m_modifier.m_amount = roll_chance_i(50) ? 25035 : 25046;
                            break;
                        case RACE_TROLL:
                            m_modifier.m_amount = roll_chance_i(50) ? 25041 : 25052;
                            break;
                        case RACE_GOBLIN:
                            m_modifier.m_amount = roll_chance_i(50) ? 25036 : 25047;
                            break;
                        case RACE_BLOODELF:
                            m_modifier.m_amount = roll_chance_i(50) ? 25032 : 25043;
                            break;
                        case RACE_DRAENEI:
                            m_modifier.m_amount = roll_chance_i(50) ? 25033 : 25044;
                            break;
                    }

                    break;
                }
                case 65529:                                 // Gossip NPC Appearance - Day of the Dead (DotD)
                    // random, regardless of current gender
                    m_modifier.m_amount = roll_chance_i(50) ? 29203 : 29204;
                    break;
                // case 66236:                          // Incinerate Flesh
                // break;
                // case 69999:                          // [DND] Swap IDs
                // break;
                // case 70764:                          // Citizen Costume (note: many spells w/same name)
                // break;
                // case 71309:                          // [DND] Spawn Portal
                // break;
                case 71450:                                 // Crown Parcel Service Uniform
                    m_modifier.m_amount = target->getGender() == GENDER_MALE ? 31002 : 31003;
                    break;
                // case 75531:                          // Gnomeregan Pride
                // break;
                // case 75532:                          // Darkspear Pride
                // break;
                default:
                    if (!m_modifier.m_amount) // can be set through script
                        sLog.outError("Aura::HandleAuraTransform, spell %u does not have creature entry defined, need custom defined model.", GetId());
                    break;
            }
        }
        else                                                // m_modifier.m_amount != 0
        {
            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(m_modifier.m_miscvalue);
            if (!cInfo)
            {
                m_modifier.m_amount = 16358;                           // pig pink ^_^
                sLog.outError("Auras: unknown creature id = %d (only need its modelid) Form Spell Aura Transform in Spell ID = %d", m_modifier.m_amount, GetId());
            }
            else
                m_modifier.m_amount = Creature::ChooseDisplayId(cInfo);   // Will use the default model here

            // creature case, need to update equipment if additional provided
            if (cInfo && target->GetTypeId() == TYPEID_UNIT)
            {
                skipDisplayUpdate = ((Creature*)target)->IsTotem();
                ((Creature*)target)->LoadEquipment(cInfo->EquipmentTemplateId, false);
            }
        }

        if (!skipDisplayUpdate)
            target->SetDisplayId(m_modifier.m_amount);

        // polymorph case
        if (Real && GetSpellProto()->Mechanic == MECHANIC_POLYMORPH)
        {
            if (target->IsMounted())
            {
                if (target->GetTypeId() == TYPEID_PLAYER)
                    target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED, GetHolder());
                else // dismount polymorphed creature target (after patch 2.4.2)
                    target->Unmount();
            }
        }
    }
    else                                                    // !apply
    {
        // ApplyModifier(true) will reapply it if need
        target->RestoreDisplayId();

        // apply default equipment for creature case
        bool restoreEquip = true;
        switch (GetId())
        {
            case 42594: case 42606: case 42607: case 42608: restoreEquip = false; break;
            default: break;
        }
        if (target->GetTypeId() == TYPEID_UNIT && restoreEquip)
            static_cast<Creature*>(target)->LoadEquipment(static_cast<Creature*>(target)->GetCreatureInfo()->EquipmentTemplateId, true);
    }
}

void Aura::HandleForceReaction(bool apply, bool Real)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!Real)
        return;

    Player* player = (Player*)GetTarget();

    uint32 faction_id = m_modifier.m_miscvalue;
    ReputationRank faction_rank = ReputationRank(m_modifier.m_amount);

    player->GetReputationMgr().ApplyForceReaction(faction_id, faction_rank, apply);
    player->GetReputationMgr().SendForceReactions();

    // stop fighting if at apply forced rank friendly or at remove real rank friendly
    if ((apply && faction_rank >= REP_FRIENDLY) || (!apply && player->GetReputationRank(faction_id) >= REP_FRIENDLY))
        player->StopAttackFaction(faction_id);
}

void Aura::HandleAuraModSkill(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* target = static_cast<Player*>(GetTarget());

    Modifier const* mod = GetModifier();
    const uint16 skillId = uint16(GetSpellProto()->EffectMiscValue[m_effIndex]);
    const int16 amount = int16(mod->m_amount);
    const bool permanent = (mod->m_auraname == SPELL_AURA_MOD_SKILL_TALENT);

    target->ModifySkillBonus(skillId, (apply ? amount : -amount), permanent);
}

void Aura::HandleChannelDeathItem(bool apply, bool Real)
{
    if (Real && !apply)
    {
        if (m_removeMode != AURA_REMOVE_BY_DEATH)
            return;
        // Item amount
        if (m_modifier.m_amount <= 0)
            return;

        SpellEntry const* spellInfo = GetSpellProto();
        if (spellInfo->EffectItemType[m_effIndex] == 0)
            return;

        Unit* victim = GetTarget();
        Unit* caster = GetCaster();
        if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
            return;

        uint32 itemType = spellInfo->EffectItemType[m_effIndex];

        // Soul Shard (target req.)
        if (itemType == 6265)
        {
            // Only from non-grey units
            if (!((Player*)caster)->isHonorOrXPTarget(victim))
                return;
            // Only if the creature is tapped by the player or his group
            if (victim->GetTypeId() == TYPEID_UNIT && !((Creature*)victim)->IsTappedBy((Player*)caster))
                return;
            // Avoid awarding multiple souls on the same target
            // 1.11.0: If you cast Drain Soul while shadowburn is on the victim, you will no longer receive two soul shards upon the victim's death.
            for (auto const& aura : victim->GetAurasByType(SPELL_AURA_CHANNEL_DEATH_ITEM))
                if (aura != this && caster->GetObjectGuid() == aura->GetCasterGuid() && aura->GetSpellProto()->EffectItemType[aura->GetEffIndex()] == itemType)
                    return;
        }

        // Adding items
        uint32 noSpaceForCount = 0;
        uint32 count = m_modifier.m_amount;

        ItemPosCountVec dest;
        InventoryResult msg = ((Player*)caster)->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemType, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)
        {
            count -= noSpaceForCount;
            ((Player*)caster)->SendEquipError(msg, nullptr, nullptr, itemType);
            if (count == 0)
                return;
        }

        Item* newitem = ((Player*)caster)->StoreNewItem(dest, itemType, true);
        ((Player*)caster)->SendNewItem(newitem, count, true, true);

        // Soul Shard (glyph bonus)
        if (spellInfo->EffectItemType[m_effIndex] == 6265)
        {
            // Glyph of Soul Shard
            if (caster->HasAura(58070) && roll_chance_i(40))
                caster->CastSpell(caster, 58068, TRIGGERED_OLD_TRIGGERED, nullptr, this);
        }
    }
}

void Aura::HandleBindSight(bool apply, bool /*Real*/)
{
    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Camera& camera = ((Player*)caster)->GetCamera();
    if (apply)
        camera.SetView(GetTarget());
    else
        camera.ResetView();
}

void Aura::HandleFarSight(bool apply, bool /*Real*/)
{
    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Camera& camera = ((Player*)caster)->GetCamera();
    if (apply)
        camera.SetView(GetTarget());
    else
        camera.ResetView();
}

void Aura::HandleAuraTrackCreatures(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        GetTarget()->SetFlag(PLAYER_TRACK_CREATURES, uint32(1) << (m_modifier.m_miscvalue - 1));
    else
        GetTarget()->RemoveFlag(PLAYER_TRACK_CREATURES, uint32(1) << (m_modifier.m_miscvalue - 1));
}

void Aura::HandleAuraTrackResources(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        GetTarget()->SetFlag(PLAYER_TRACK_RESOURCES, uint32(1) << (m_modifier.m_miscvalue - 1));
    else
        GetTarget()->RemoveFlag(PLAYER_TRACK_RESOURCES, uint32(1) << (m_modifier.m_miscvalue - 1));
}

void Aura::HandleAuraTrackStealthed(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    GetTarget()->ApplyModByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_TRACK_STEALTHED, apply);
}

void Aura::HandleAuraModScale(bool /*apply*/, bool /*Real*/)
{
    int32 value = GetTarget()->GetTotalAuraModifier(SPELL_AURA_MOD_SCALE);
    int32 otherValue = GetTarget()->GetTotalAuraModifier(SPELL_AURA_MOD_SCALE_2);
    float scale = std::max(0.1f, float(100 + value) / 100.f * float(100 + otherValue) / 100.f);
    GetTarget()->SetObjectScale(scale);
    GetTarget()->UpdateModelData();
}

void Aura::HandleModPossess(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // not possess yourself
    if (GetCasterGuid() == target->GetObjectGuid())
        return;

    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER) // TODO:: well i know some bosses can take control of player???
        return;

    if (apply)
    {
        if (const SpellEntry* spellInfo = GetSpellProto())
        {
            switch (spellInfo->Id)
            {
                case 24937: // Using Control Console
                {
                    static const uint32 spells[] = { 25026, 25024, 27759, 25027 };
                    if (target->GetTypeId() == TYPEID_UNIT)
                        static_cast<Creature*>(target)->UpdateSpell(5, spells[urand(0, 3)]);
                }
                break;
            }
        }

        // Possess: advertised type of charm (unique) - remove existing advertised charm
        caster->BreakCharmOutgoing(true);

        caster->TakePossessOf(target);

        if (Player* playerCaster = caster->IsPlayer() ? static_cast<Player*>(caster) : nullptr)
        {
            UpdateData newData;
            target->BuildValuesUpdateBlockForPlayerWithFlags(newData, playerCaster, UF_FLAG_OWNER_ONLY);
            if (newData.HasData())
            {
                WorldPacket newDataPacket = newData.BuildPacket(0);
                playerCaster->SendDirectMessage(newDataPacket);
            }
        }
    }
    else
    {
        caster->Uncharm(target);

        // clean dummy auras from caster : TODO check if its right in all case
        caster->RemoveAurasDueToSpell(GetId());
    }

    if (const SpellEntry* spellInfo = GetSpellProto())
    {
        switch (spellInfo->Id)
        {
            case 24937: // Using Control Console
                if (!apply)
                {
                    caster->CastSpell(caster, 24938, TRIGGERED_OLD_TRIGGERED);      // Close Control Console
                    caster->CastSpell(caster, 27880, TRIGGERED_OLD_TRIGGERED);      // Stun
                    if (target->GetTypeId() == TYPEID_UNIT)
                    {
                        target->CastSpell(target, 3617, TRIGGERED_OLD_TRIGGERED);   // Quiet Suicide
                        static_cast<Creature*>(target)->ForcedDespawn(5000);
                    }
                }
                break;
            // Need to teleport to spawn position on possess end
            case 37868: // Arcano-Scorp Control
            case 37893:
            case 37895:
                if (!apply)
                {
                    float x, y, z, o;
                    Creature* creatureTarget = (Creature*)target;
                    creatureTarget->GetRespawnCoord(x, y, z, &o);
                    creatureTarget->NearTeleportTo(x, y, z, o);
                    caster->InterruptSpell(CURRENT_CHANNELED_SPELL);
                }
                break;
            case 37748: // Teron Gorefiend - remove aura from caster when posses is removed
                if (!apply)
                {
                    static_cast<Creature*>(target)->ForcedDespawn();
                    caster->RemoveAurasDueToSpell(37748);
                }
                break;
            case 40268: // Spiritual Vengeance - Teron Gorefiend - kill player and despawn spirit
                if (!apply)
                {
                    caster->RemoveAurasDueToSpell(40268);
                    caster->RemoveAurasDueToSpell(40282);
                    caster->CastSpell(nullptr, 29878, TRIGGERED_OLD_TRIGGERED);
                    if (target->GetTypeId() == TYPEID_UNIT)
                        static_cast<Creature*>(target)->ForcedDespawn();
                }
                break;
        }
    }
}

void Aura::HandleModPossessPet(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_UNIT || !static_cast<Creature*>(target)->IsPet())
        return;

    if (apply)
    {
        // Possess pet: advertised type of charm (unique) - remove existing advertised charm
        caster->BreakCharmOutgoing(true);

        caster->TakePossessOf(target);
    }
    else
        caster->Uncharm(target);
}

void Aura::HandleAuraModPetTalentsPoints(bool /*Apply*/, bool Real)
{
    if (!Real)
        return;

    // Recalculate pet talent points
    if (Pet* pet = GetTarget()->GetPet())
        pet->InitTalentForLevel();
}

void Aura::HandleModCharm(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // not charm yourself
    if (GetCasterGuid() == target->GetObjectGuid())
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    Player* playerCaster = caster->GetTypeId() == TYPEID_PLAYER ? static_cast<Player*>(caster) : nullptr;

    if (apply)
    {
        // Charm: normally advertised type of charm (unique), but with notable exceptions:
        // * Seems to be non-unique for NPCs - allows overwriting advertised charm by offloading existing one (e.g. Chromatic Mutation)
        // * Seems to be always unique for players - remove player's existing advertised charm (no evidence against this found yet)
        if (playerCaster)
            caster->BreakCharmOutgoing(true);

        caster->TakeCharmOf(target, GetId());

        if (playerCaster)
        {
            UpdateMask updateMask;
            updateMask.SetCount(target->GetValuesCount());
            target->MarkUpdateFieldsWithFlagForUpdate(updateMask, UF_FLAG_OWNER_ONLY);
            if (updateMask.HasData())
            {
                UpdateData newData;
                target->BuildValuesUpdateBlockForPlayer(newData, updateMask, playerCaster);

                if (newData.HasData())
                {
                    WorldPacket newDataPacket = newData.BuildPacket(0);
                    playerCaster->SendDirectMessage(newDataPacket);
                }
            }
        }
    }
    else
        caster->Uncharm(target, GetId());

    if (apply)
    {

    }
    else
    {
        switch (GetId())
        {
            case 30019: // Control Piece
                caster->RemoveAurasDueToSpell(30019);
                break;
            case 33684:
                if (caster->GetTypeId() == TYPEID_UNIT)
                    static_cast<Creature*>(caster)->ForcedDespawn();
                break;
        }
    }
}


void Aura::HandleAoECharm(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // not charm yourself
    if (GetCasterGuid() == target->GetObjectGuid())
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    if (apply)
        // AoE charm: non-advertised type of charm - co-exists with other charms
        caster->TakeCharmOf(target, GetId(), false);
    else
        caster->Uncharm(target, GetId());
}

void Aura::HandleModConfuse(bool apply, bool Real)
{
    if (!Real)
        return;

    // Do not remove it yet if more effects are up, do it for the last effect
    if (!apply && GetTarget()->HasAuraType(SPELL_AURA_MOD_CONFUSE))
        return;

    GetTarget()->SetConfused(apply, GetCasterGuid(), GetId());

    GetTarget()->getHostileRefManager().HandleSuppressed(apply);
}

void Aura::HandleModFear(bool apply, bool Real)
{
    if (!Real)
        return;

    // Do not remove it yet if more effects are up, do it for the last effect
    if (!apply && GetTarget()->HasAuraType(SPELL_AURA_MOD_FEAR))
        return;

    GetTarget()->SetFleeing(apply, GetCasterGuid(), GetId());

    // 2.3.0 - fear no longer applies suppression - in case of uncomment, need to adjust IsSuppressedTarget
#ifdef PRENERF_2_1
    GetTarget()->getHostileRefManager().HandleSuppressed(apply);
#endif
}

void Aura::HandleFeignDeath(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // Do not remove it yet if more effects are up, do it for the last effect
    if (!apply && target->HasAuraType(SPELL_AURA_FEIGN_DEATH))
        return;

    if (apply)
    {
        bool success = true;

        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        {
            // Players and player-controlled units do an additional success roll for this aura on application
            const SpellEntry* entry = GetSpellProto();
            const SpellSchoolMask schoolMask = GetSpellSchoolMask(entry);

            float resist = 0;

            for (auto attacker : target->getAttackers())
            {
                if (attacker && !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                {
                    const float chance = target->CalculateSpellMissChance(attacker, schoolMask, entry);
                    resist = ((chance > resist) ? chance : resist);
                }
            }

            success = !roll_chance_f(resist);
        }

        if (success)
            target->InterruptSpellsCastedOnMe();

        target->SetFeignDeath(apply, GetCasterGuid(), GetId(), true, success);
    }
    else
        target->SetFeignDeath(false);
}

void Aura::HandleAuraModDisarm(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!apply && target->HasAuraType(GetModifier()->m_auraname))
        return;

    uint32 flags;
    uint32 field;
    WeaponAttackType attack_type;

    switch (GetModifier()->m_auraname)
    {
        default:
        case SPELL_AURA_MOD_DISARM:
        {
            field = UNIT_FIELD_FLAGS;
            flags = UNIT_FLAG_DISARMED;
            attack_type = BASE_ATTACK;
            break;
        }
        case SPELL_AURA_MOD_DISARM_OFFHAND:
        {
            field = UNIT_FIELD_FLAGS_2;
            flags = UNIT_FLAG2_DISARM_OFFHAND;
            attack_type = OFF_ATTACK;
            break;
        }
        case SPELL_AURA_MOD_DISARM_RANGED:
        {
            field = UNIT_FIELD_FLAGS_2;
            flags = UNIT_FLAG2_DISARM_RANGED;
            attack_type = RANGED_ATTACK;
            break;
        }
    }

    target->ApplyModFlag(field, flags, apply);

    // main-hand attack speed already set to special value for feral form already and don't must change and reset at remove.
    if (target->IsNoWeaponShapeShift())
        return;

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (apply)
            target->SetAttackTime(attack_type, BASE_ATTACK_TIME);
        else
            ((Player*)target)->SetRegularAttackTime();
    }

    target->UpdateDamagePhysical(attack_type);
}

void Aura::HandleAuraModStun(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        // Frost stun aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
            target->ModifyAuraState(AURA_STATE_FROZEN, apply);

        // Creature specific
        if (target->IsInWorld() && !target->IsPlayer() && !target->IsStunned())
            target->SetFacingTo(target->GetOrientation());

        Unit* caster = GetCaster();
        target->SetStunned(true, (caster ? caster->GetObjectGuid() : ObjectGuid()), GetSpellProto()->Id);

        if (caster)
            if (UnitAI* ai = caster->AI())
                ai->JustStunnedTarget(GetSpellProto(), target);

        if (GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_DAMAGE)
            target->getHostileRefManager().HandleSuppressed(apply);

        switch (GetId())
        {
            case 32890: // Knockout - OHF
                target->SetStandState(UNIT_STAND_STATE_DEAD);
                break;
            case 39837: // Summon the Naj'entus Spine GameObject on target if spell is Impaling Spine
                target->CastSpell(nullptr, 39929, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }
    else
    {
        // Frost stun aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
        {
            bool found_another = false;
            for (AuraType const* itr = &frozenAuraTypes[0]; *itr != SPELL_AURA_NONE; ++itr)
            {
                Unit::AuraList const& auras = target->GetAurasByType(*itr);
                for (auto aura : auras)
                {
                    if (GetSpellSchoolMask(aura->GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
                    {
                        found_another = true;
                        break;
                    }
                }
                if (found_another)
                    break;
            }

            if (!found_another)
                target->ModifyAuraState(AURA_STATE_FROZEN, apply);
        }

        if (GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_DAMAGE)
            target->getHostileRefManager().HandleSuppressed(apply);

        // Real remove called after current aura remove from lists, check if other similar auras active
        if (!target->HasAuraType(SPELL_AURA_MOD_STUN))
            target->SetStunned(false);

        switch (GetId())
        {
            case 19386: // Wyvern Sting - Hunter
            case 24132:
            case 24133:
            case 27068:
            {
                Unit* caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                uint32 spell_id;
                switch (GetId())
                {
                    case 19386: spell_id = 24131; break;
                    case 24132: spell_id = 24134; break;
                    case 24133: spell_id = 24135; break;
                    case 27068: spell_id = 27069; break;
                    case 49011: spell_id = 49009; break;
                    case 49012: spell_id = 49010; break;
                    default: sLog.outError("Spell selection called for unexpected original spell %u, new spell for this spell family?", GetId()); return;
                }

                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spell_id);

                if (!spellInfo)
                    return;

                caster->CastSpell(target, spellInfo, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            case 40510: // Demon Transform 3 - Illidan
                if (UnitAI * ai = target->AI())
                    ai->SendAIEvent(AI_EVENT_CUSTOM_D, target, target);
                break;
            case 41083: // Paralyze
                if (Unit * caster = GetCaster())
                    if (UnitAI * ai = caster->AI())
                        ai->SendAIEvent(AI_EVENT_CUSTOM_A, caster, caster);
                break;
        }
    }
}

void Aura::HandleModStealth(bool apply, bool Real)
{
    Unit* target = GetTarget();
    // TODO: add mask
    target->GetVisibilityData().AddStealthStrength(StealthType(m_modifier.m_miscvalue), apply ? m_modifier.m_amount : -m_modifier.m_amount);

    if (apply)
    {
        // drop flag at stealth in bg
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_STEALTH_INVIS_CANCELS);

        // only at real aura add
        if (Real)
        {
            target->SetVisFlags(UNIT_VIS_FLAG_CREEP);

            if (target->GetTypeId() == TYPEID_PLAYER)
                target->SetByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_STEALTH);

            // apply only if not in GM invisibility (and overwrite invisibility state)
            if (target->GetVisibility() != VISIBILITY_OFF)
            {
                target->SetVisibility(VISIBILITY_GROUP_NO_DETECT);
                target->SetVisibility(VISIBILITY_GROUP_STEALTH);
            }

            // apply full stealth period bonuses only at first stealth aura in stack
            if (target->GetAurasByType(SPELL_AURA_MOD_STEALTH).size() <= 1)
            {
                Unit::AuraList const& mDummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
                for (auto mDummyAura : mDummyAuras)
                {
                    // Master of Subtlety
                    if (mDummyAura->GetSpellProto()->SpellIconID == 2114)
                    {
                        target->RemoveAurasDueToSpell(31666);
                        int32 bp = mDummyAura->GetModifier()->m_amount;
                        target->CastCustomSpell(target, 31665, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                    }
                    // Overkill
                    else if (mDummyAura->GetId() == 58426 && GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000400000))
                    {
                        target->CastSpell(target, 58427, TRIGGERED_OLD_TRIGGERED);
                    }
                }
            }
        }
    }
    else
    {
        // only at real aura remove of _last_ SPELL_AURA_MOD_STEALTH
        if (Real && !target->HasAuraType(SPELL_AURA_MOD_STEALTH))
        {
            // if no GM invisibility
            if (target->GetVisibility() != VISIBILITY_OFF)
            {
                target->RemoveVisFlags(UNIT_VIS_FLAG_CREEP);

                if (target->GetTypeId() == TYPEID_PLAYER)
                    target->RemoveByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_STEALTH);

                // restore invisibility if any
                if (target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                {
                    target->SetVisibility(VISIBILITY_GROUP_NO_DETECT);
                    target->SetVisibility(VISIBILITY_GROUP_INVISIBILITY);
                }
                else
                    target->SetVisibility(VISIBILITY_ON);
            }

            // apply delayed talent bonus remover at last stealth aura remove
            Unit::AuraList const& mDummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
            for (auto mDummyAura : mDummyAuras)
            {
                // Master of Subtlety
                if (mDummyAura->GetSpellProto()->SpellIconID == 2114)
                    target->CastSpell(target, 31666, TRIGGERED_OLD_TRIGGERED);
                // Overkill
                else if (mDummyAura->GetId() == 58426 && GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000400000))
                {
                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(58427))
                    {
                        holder->SetAuraMaxDuration(20 * IN_MILLISECONDS);
                        holder->RefreshHolder();
                    }
                }
            }
        }
    }
}

void Aura::HandleModStealthDetect(bool apply, bool Real)
{
    GetTarget()->GetVisibilityData().AddStealthDetectionStrength(StealthType(m_modifier.m_miscvalue), apply ? m_modifier.m_amount : -m_modifier.m_amount);
}

void Aura::HandleModStealthLevel(bool apply, bool Real)
{
    GetTarget()->GetVisibilityData().AddStealthStrength(StealthType(m_modifier.m_miscvalue), apply ? m_modifier.m_amount : -m_modifier.m_amount);
}

void Aura::HandleInvisibility(bool apply, bool Real)
{
    Unit* target = GetTarget();

    target->GetVisibilityData().AddInvisibilityValue(m_modifier.m_miscvalue, apply ? m_modifier.m_amount : -m_modifier.m_amount);
    int32 value = target->GetVisibilityData().GetInvisibilityValue(m_modifier.m_miscvalue);
    bool trueApply = value > 0;
    target->GetVisibilityData().SetInvisibilityMask(m_modifier.m_miscvalue, trueApply);
    if (trueApply)
    {
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_STEALTH_INVIS_CANCELS);

        if (Real && target->GetTypeId() == TYPEID_PLAYER && GetId() != 32727) // avoid changing this for Arena Preparation spell - it seems this should not cause the glow vision
        {
            if (((Player*)target)->IsSelfMover()) // check if the player doesnt have a mover, when player is hidden during MC of creature
            {
                // apply glow vision
                target->SetByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);
            }
        }

        // apply only if not in GM invisibility and not stealth
        if (target->GetVisibility() == VISIBILITY_ON)
            target->SetVisibilityWithoutUpdate(VISIBILITY_GROUP_INVISIBILITY);
    }
    else
    {
        // only at real aura remove and if not have different invisibility auras.
        if (Real && target->GetVisibilityData().GetInvisibilityMask() == 0)
        {
            // remove glow vision
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->RemoveByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);

            // apply only if not in GM invisibility & not stealthed while invisible
            if (target->GetVisibility() != VISIBILITY_OFF)
            {
                // if have stealth aura then already have stealth visibility
                if (!target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    target->SetVisibilityWithoutUpdate(VISIBILITY_ON);
            }
        }

        if (GetId() == 48809)                               // Binding Life
            target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(m_effIndex), TRIGGERED_OLD_TRIGGERED);
    }

    if (target->IsInWorld())
        target->UpdateVisibilityAndView();
}

void Aura::HandleInvisibilityDetect(bool apply, bool Real)
{
    Unit* target = GetTarget();

    target->GetVisibilityData().SetInvisibilityDetectMask(m_modifier.m_miscvalue, apply);
    target->GetVisibilityData().AddInvisibilityDetectValue(m_modifier.m_miscvalue, apply ? m_modifier.m_amount : -m_modifier.m_amount);
    if (!apply)
    {
        Unit::AuraList const& auras = target->GetAurasByType(SPELL_AURA_MOD_INVISIBILITY_DETECTION);
        for (auto aura : auras)
            target->GetVisibilityData().SetInvisibilityDetectMask(aura->GetModifier()->m_miscvalue, true);
    }
    if (Real && target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)target)->GetCamera().UpdateVisibilityForOwner();
}

void Aura::HandleDetectAmore(bool apply, bool /*real*/)
{
    if (!GetTarget()->IsPlayer())
        return;

    GetTarget()->ApplyModByteFlag(PLAYER_FIELD_BYTES2, 3, 1 << (GetMiscValue() - 1), apply);
}

void Aura::HandleAuraModRoot(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        // Frost root aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
            target->ModifyAuraState(AURA_STATE_FROZEN, apply);

        if (Unit* caster = GetCaster())
            if (UnitAI* ai = caster->AI())
                ai->JustRootedTarget(GetSpellProto(), target);
    }
    else
    {
        // Frost root aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
        {
            bool found_another = false;
            for (AuraType const* itr = &frozenAuraTypes[0]; *itr != SPELL_AURA_NONE; ++itr)
            {
                Unit::AuraList const& auras = target->GetAurasByType(*itr);
                for (auto aura : auras)
                {
                    if (GetSpellSchoolMask(aura->GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
                    {
                        found_another = true;
                        break;
                    }
                }
                if (found_another)
                    break;
            }

            if (!found_another)
                target->ModifyAuraState(AURA_STATE_FROZEN, apply);
        }

        // Real remove called after current aura remove from lists, check if other similar auras active
        if (target->HasAuraType(SPELL_AURA_MOD_ROOT))
            return;
    }

    target->SetImmobilizedState(apply);
}

void Aura::HandleAuraModSilence(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
        // Stop cast only spells vs PreventionType == SPELL_PREVENTION_TYPE_SILENCE
        for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            if (Spell* spell = target->GetCurrentSpell(CurrentSpellTypes(i)))
                if (spell->m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
                    // Stop spells on prepare or casting state
                    target->InterruptSpell(CurrentSpellTypes(i), false);
    }
    else
    {
        // Real remove called after current aura remove from lists, check if other similar auras active
        if (target->HasAuraType(SPELL_AURA_MOD_SILENCE))
            return;

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
    }
}

void Aura::HandleModThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!target->IsAlive())
        return;

    int level_diff = 0;
    int multiplier = 0;
    switch (GetId())
    {
        // Arcane Shroud
        case 26400:
            level_diff = target->GetLevel() - 60;
            multiplier = 2;
            break;
        // The Eye of Diminution
        case 28862:
            level_diff = target->GetLevel() - 60;
            multiplier = 1;
            break;
        // Defiance talents
        case 12303:
        case 12788:
        case 12789:
            if (target->GetShapeshiftForm() != FORM_DEFENSIVESTANCE)
                m_modifier.m_amount = 0;
            break;
    }

    if (level_diff > 0)
        m_modifier.m_amount += multiplier * level_diff;

    for (int8 x = 0; x < MAX_SPELL_SCHOOL; ++x)
        if (m_modifier.m_miscvalue & int32(1 << x))
            ApplyPercentModFloatVar(target->m_threatModifier[x], float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModTotalThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!target->IsAlive() || target->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->IsAlive())
        return;

    target->getHostileRefManager().threatTemporaryFade(caster, m_modifier.m_amount, apply);
}

void Aura::HandleModTaunt(bool /*apply*/, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!target->IsAlive() || !target->CanHaveThreatList())
        return;

    target->TauntUpdate();

    if (target->AI())
        target->AI()->OnTaunt();
}

void Aura::HandleAuraFakeInebriation(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 point = target->GetInt32Value(PLAYER_FAKE_INEBRIATION);
        point += (apply ? 1 : -1) * GetBasePoints();

        target->SetInt32Value(PLAYER_FAKE_INEBRIATION, point);
    }

    target->UpdateObjectVisibility();
}

/*********************************************************/
/***                  MODIFY SPEED                     ***/
/*********************************************************/
void Aura::HandleAuraModIncreaseSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    GetTarget()->UpdateSpeed(MOVE_RUN, true);
}

void Aura::HandleAuraModIncreaseMountedSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);

    // Festive Holiday Mount
    if (apply && GetSpellProto()->SpellIconID != 1794 && target->HasAura(62061))
        // Reindeer Transformation
        target->CastSpell(target, 25860, TRIGGERED_OLD_TRIGGERED, nullptr, this);
}

void Aura::HandleAuraModIncreaseFlightSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    // Enable Fly mode for flying mounts
    if (m_modifier.m_auraname == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
    {
        target->SetCanFly(apply);

        // Players on flying mounts must be immune to polymorph
        if (target->GetTypeId() == TYPEID_PLAYER)
            target->ApplySpellImmune(this, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);

        // Festive Holiday Mount
        if (apply && GetSpellProto()->SpellIconID != 1794 && target->HasAura(62061))
            // Reindeer Transformation
            target->CastSpell(target, 25860, TRIGGERED_OLD_TRIGGERED, nullptr, this);
    }

    target->UpdateSpeed(MOVE_FLIGHT, true);
}

void Aura::HandleAuraModIncreaseSwimSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    GetTarget()->UpdateSpeed(MOVE_SWIM, true);
}

void Aura::HandleAuraModDecreaseSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);
    target->UpdateSpeed(MOVE_RUN_BACK, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT, true);
}

void Aura::HandleAuraModUseNormalSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT, true);
}

/*********************************************************/
/***                     IMMUNITY                      ***/
/*********************************************************/

void Aura::HandleModMechanicImmunity(bool apply, bool /*Real*/)
{
    uint32 misc  = m_modifier.m_miscvalue;
    // Forbearance
    // in DBC wrong mechanic immune since 3.0.x
    if (GetId() == 25771)
        misc = MECHANIC_IMMUNE_SHIELD;

    Unit* target = GetTarget();

    if (apply && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
    {
        uint32 mechanic = 1 << (misc - 1);

        // immune movement impairment and loss of control (spell data have special structure for mark this case)
        if (IsSpellRemoveAllMovementAndControlLossEffects(GetSpellProto()))
            mechanic = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;

        target->RemoveAurasAtMechanicImmunity(mechanic, GetId());
    }

    target->ApplySpellImmune(this, IMMUNITY_MECHANIC, misc, apply);

    // Bestial Wrath
    if (GetSpellProto()->Id == 19574)
    {
        // The Beast Within cast on owner if talent present
        if (Unit* owner = target->GetOwner())
        {
            // Search talent The Beast Within
            Unit::AuraList const& dummyAuras = owner->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            for (auto dummyAura : dummyAuras)
            {
                if (dummyAura->GetSpellProto()->SpellIconID == 2229)
                {
                    if (apply)
                        owner->CastSpell(owner, 34471, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    else
                        owner->RemoveAurasDueToSpell(34471);
                    break;
                }
            }
        }
    }
    // Heroic Fury (Intercept cooldown remove)
    else if (apply && GetSpellProto()->Id == 60970 && target->GetTypeId() == TYPEID_PLAYER)
        target->RemoveSpellCooldown(20252, true);

    switch (GetId())
    {
        case 18461: // Vanish Purge
            if (m_effIndex == EFFECT_INDEX_0)
                target->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);
            break;
        case 42292: // PvP trinket
            target->RemoveRankAurasDueToSpell(20184); // Judgement of justice - remove any rank
            break;
    }
}

void Aura::HandleModMechanicImmunityMask(bool apply, bool /*Real*/)
{
    uint32 mechanic  = m_modifier.m_miscvalue;

    if (apply && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
        GetTarget()->RemoveAurasAtMechanicImmunity(mechanic, GetId());

    // check implemented in Unit::IsImmuneToSpell and Unit::IsImmuneToSpellEffect
}

// this method is called whenever we add / remove aura which gives m_target some imunity to some spell effect
void Aura::HandleAuraModEffectImmunity(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    // when removing flag aura, handle flag drop
    if (target->GetTypeId() == TYPEID_PLAYER && (GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_INVULNERABILITY_BUFF_CANCELS))
    {
        Player* player = static_cast<Player*>(target);

        if (apply)
            player->pvpInfo.isPvPFlagCarrier = true;
        else
        {
            player->pvpInfo.isPvPFlagCarrier = false;

            if (BattleGround* bg = player->GetBattleGround())
                bg->HandlePlayerDroppedFlag(player);
            else if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(player->GetCachedZoneId()))
                outdoorPvP->HandleDropFlag(player, GetSpellProto()->Id);
        }
    }

    target->ApplySpellImmune(this, IMMUNITY_EFFECT, m_modifier.m_miscvalue, apply);

    if (apply && IsPositive())
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_INVULNERABILITY_BUFF_CANCELS, GetHolder());

    switch (GetSpellProto()->Id)
    {
        case 32430:                     // Battle Standard (Alliance - ZM OPVP)
        case 32431:                     // Battle Standard (Horde - ZM OPVP)
        {
            // Handle OPVP script condition change on aura apply; Specific for Zangarmarsh outdoor pvp
            if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(3521))
                outdoorPvP->HandleConditionStateChange(uint32(GetSpellProto()->Id == 32431), apply);
        }
    }
}

void Aura::HandleAuraModStateImmunity(bool apply, bool Real)
{
    if (apply && Real && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
    {
        Unit::AuraList const& auraList = GetTarget()->GetAurasByType(AuraType(m_modifier.m_miscvalue));
        for (Unit::AuraList::const_iterator itr = auraList.begin(); itr != auraList.end();)
        {
            if (auraList.front() != this)                   // skip itself aura (it already added)
            {
                GetTarget()->RemoveAurasDueToSpell(auraList.front()->GetId());
                itr = auraList.begin();
            }
            else
                ++itr;
        }
    }

    GetTarget()->ApplySpellImmune(this, IMMUNITY_STATE, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModSchoolImmunity(bool apply, bool Real)
{
    Unit* target = GetTarget();
    target->ApplySpellImmune(this, IMMUNITY_SCHOOL, m_modifier.m_miscvalue, apply);

    // remove all flag auras (they are positive, but they must be removed when you are immune)
    if (GetSpellProto()->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_INVULNERABILITY_BUFF_CANCELS);

    // TODO: optimalize this cycle - use RemoveAurasWithInterruptFlags call or something else
    if (Real && apply
            && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT)
            && IsPositiveSpell(GetId(), GetCaster(), target))                    // Only positive immunity removes auras
    {
        uint32 school_mask = m_modifier.m_miscvalue;
        Unit::SpellAuraHolderMap& Auras = target->GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
        {
            next = iter;
            ++next;
            SpellEntry const* spell = iter->second->GetSpellProto();
            if ((GetSpellSchoolMask(spell) & school_mask)   // Check for school mask
                    && !spell->HasAttribute(SPELL_ATTR_NO_IMMUNITIES)   // Spells unaffected by invulnerability
                    && !iter->second->IsPositive()          // Don't remove positive spells
                    && spell->Id != GetId())                // Don't remove self
            {
                target->RemoveAurasDueToSpell(spell->Id);
                if (Auras.empty())
                    break;
                next = Auras.begin();
            }
        }
    }

    if (Real && GetSpellProto()->Mechanic == MECHANIC_BANISH)
    {
        if (apply)
            target->addUnitState(UNIT_STAT_ISOLATED);
        else
            target->clearUnitState(UNIT_STAT_ISOLATED);
    }

    GetTarget()->getHostileRefManager().HandleSuppressed(apply, true);

    if (apply)
        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (target->HasAuraType(SPELL_AURA_SCHOOL_IMMUNITY))
            return;
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE);
    }
}

void Aura::HandleAuraModDmgImmunity(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    target->ApplySpellImmune(this, IMMUNITY_DAMAGE, m_modifier.m_miscvalue, apply);

    target->getHostileRefManager().HandleSuppressed(apply, true);

    if (!apply && GetId() == 41376) // Spite - Essence of Anger
        if (Unit* caster = GetCaster())
            caster->CastSpell(target, 41377, TRIGGERED_OLD_TRIGGERED);
}

void Aura::HandleAuraModDispelImmunity(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    GetTarget()->ApplySpellDispelImmunity(this, DispelType(m_modifier.m_miscvalue), apply);
}

void Aura::HandleAuraProcTriggerSpell(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    switch (GetId())
    {
        // some spell have charges by functionality not have its in spell data
        case 28200:                                         // Ascendance (Talisman of Ascendance trinket)
            if (apply)
                GetHolder()->SetAuraCharges(6);
            break;
        case 39057: // Serpentshrine Parasite - always hit with melee
            GetTarget()->SetAlwaysHit(apply);
            break;
        case 40594: // Felrage - Gurtogg Bloodboil - remove Fury on removal of Felrage
            if (!apply)
                GetTarget()->RemoveAurasDueToSpell(40601);
            break;
        case 50720:                                         // Vigilance (threat transfering)
            if (apply)
            {
                if (Unit* caster = GetCaster())
                    target->CastSpell(caster, 59665, TRIGGERED_OLD_TRIGGERED);
            }
            else
                target->getHostileRefManager().ResetThreatRedirection(59665);
            break;
        default:
            break;
    }
}

void Aura::HandleAuraModStalked(bool apply, bool /*Real*/)
{
    // used by spells: Hunter's Mark, Mind Vision, Syndicate Tracker (MURP) DND
    if (apply)
        GetTarget()->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    else
        GetTarget()->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
}

/*********************************************************/
/***                   PERIODIC                        ***/
/*********************************************************/

void Aura::HandlePeriodicTriggerSpell(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    Unit* target = GetTarget();

    if (apply)
    {
        switch (GetId())
        {
            case 29946:
                if (target->HasAura(29947))
                    target->RemoveAurasDueToSpellByCancel(29947);
                break;
            case 43648: // Electrical Storm - Akil'zon - cast teleport self
                target->CastSpell(nullptr, 44006, TRIGGERED_OLD_TRIGGERED); // done here instead of script effect due to auras being applied after effects
                break;
            default:
                break;
        }
    }
    else
    {
        switch (GetId())
        {
            case 66:                                        // Invisibility
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32612, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                return;
            case 18173:                                     // Burning Adrenaline (Main Target version)
            case 23620:                                     // Burning Adrenaline (Caster version)
                // On aura removal, the target deals AoE damage to friendlies and kills himself/herself (prevent durability loss)
                target->CastSpell(target, 23478, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                target->CastSpell(target, 23644, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            case 29946:
                if (m_removeMode != AURA_REMOVE_BY_EXPIRE)
                    // Cast "crossed flames debuff"
                    target->CastSpell(target, 29947, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            case 33401:                                     // Possess
                if (Unit* caster = GetCaster())
                    caster->CastSpell(target, 32830, TRIGGERED_NONE);
                return;
            case 34229:                                     // Flame Quills
                if (UnitAI* ai = target->AI())
                    ai->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
                return;
            case 33711:                                     // Murmur's Touch normal and heroic
            case 38794:
                target->CastSpell(nullptr, 33686, TRIGGERED_OLD_TRIGGERED, nullptr, this); // cast Shockwave
                target->CastSpell(nullptr, 33673, TRIGGERED_OLD_TRIGGERED); // cast Shockwave knockup serverside
                return;
            case 35515:                                     // Salaadin's Tesla
                if ((m_removeMode != AURA_REMOVE_BY_STACK) && (!target->HasAura(35515)))
                    if (UnitAI* ai = target->AI())
                        ai->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
                return;
            case 37640:                                     // Leotheras Whirlwind
                if (UnitAI* ai = target->AI())
                    ai->DoResetThreat();
                return;
            case 39575:                                     // Charge Frenzy
                if (Unit* pCaster = GetCaster())
                    pCaster->FixateTarget(nullptr);
                return;
            case 39828:                                     // Light of the Naaru
                target->CastSpell(nullptr, 39831, TRIGGERED_NONE);
                target->CastSpell(nullptr, 39832, TRIGGERED_NONE);
                break;
            case 41194:                                     // Whirlwind - Bonechewer Blade Fury 23235
            case 41399:                                     // Whirlwind - Image of Dementia 23436
            case 46270:                                     // Whirlwind - Priestess of Torment 25509
                if (Unit* caster = GetCaster())
                {
                    ThreatList const& threatList = caster->getThreatManager().getThreatList();
                    for (auto i : threatList)
                        if (Unit* Temp = caster->GetMap()->GetUnit(i->getUnitGuid()))
                            caster->getThreatManager().modifyThreatPercent(Temp, -100);
                }
                return;
            case 42177:                                     // Alert Drums
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (UnitAI * ai = target->AI())
                        ai->SendAIEvent(AI_EVENT_CUSTOM_B, target, target);
                }
                return;
            case 42783:                                     // Wrath of the Astrom...
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE && GetEffIndex() + 1 < MAX_EFFECT_INDEX)
                    target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(SpellEffectIndex(GetEffIndex() + 1)), TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                return;
            case 43648: // Electrical Storm - Akil'zon - send event
                if (Unit * caster = GetCaster())
                    if (UnitAI * ai = caster->AI())
                        ai->SendAIEvent(AI_EVENT_CUSTOM_A, caster, caster);
                break;
            case 44251:                                     // Gravity Lapse Beam Visual Periodic
                target->CastSpell(nullptr, 44232, TRIGGERED_OLD_TRIGGERED);
                if (UnitAI * ai = target->AI())
                    ai->SendAIEvent(AI_EVENT_CUSTOM_A, target, target, m_removeMode == AURA_REMOVE_BY_EXPIRE);
                return;
            case 46221:                                     // Animal Blood
                if (target->GetTypeId() == TYPEID_PLAYER && m_removeMode == AURA_REMOVE_BY_DEFAULT && target->IsInWater())
                {
                    float position_z = target->GetTerrain()->GetWaterLevel(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                    // Spawn Blood Pool
                    target->CastSpell(target->GetPositionX(), target->GetPositionY(), position_z, 63471, TRIGGERED_OLD_TRIGGERED);
                }

                return;
            case 51912:                                     // Ultra-Advanced Proto-Typical Shortening Blaster
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(target, GetSpellProto()->EffectTriggerSpell[GetEffIndex()], TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }
                return;
            case 70405:                                     // Mutated Transformation (10n)
            case 72508:                                     // Mutated Transformation (25n)
            case 72509:                                     // Mutated Transformation (10h)
            case 72510:                                     // Mutated Transformation (25h)
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                {
                    if (target->IsVehicle() && target->GetTypeId() == TYPEID_UNIT)
                    {
                        target->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                        ((Creature*)target)->ForcedDespawn();
                    }
                }
            default:
                break;
        }
    }
}

void Aura::HandlePeriodicTriggerSpellWithValue(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePeriodicEnergize(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    if (apply && !loading)
    {
        switch (GetId())
        {
            case 54833:                                     // Glyph of Innervate (value%/2 of casters base mana)
            {
                if (Unit* caster = GetCaster())
                    m_modifier.m_amount = int32(caster->GetCreateMana() * GetBasePoints() / (200 * GetAuraMaxTicks()));
                break;
            }
            case 29166:                                     // Innervate (value% of casters base mana)
            {
                if (Unit* caster = GetCaster())
                {
                    // Glyph of Innervate
                    if (caster->HasAura(54832))
                        caster->CastSpell(caster, 54833, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                    m_modifier.m_amount = int32(caster->GetCreateMana() * GetBasePoints() / (100 * GetAuraMaxTicks()));
                }
                break;
            }
            case 48391:                                     // Owlkin Frenzy 2% base mana
                m_modifier.m_amount = target->GetCreateMana() * 2 / 100;
                break;
            case 57669:                                     // Replenishment (0.2% from max)
            case 61782:                                     // Infinite Replenishment
                m_modifier.m_amount = target->GetMaxPower(POWER_MANA) * 2 / 1000;
                break;
            default:
                break;
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandleAuraPowerBurn(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePrayerOfMending(bool /*apply*/, bool /*Real*/)
{

}

void Aura::HandleAuraPeriodicDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    SpellEntry const* spell = GetSpellProto();
    switch (spell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {


            switch (spell->Id)
            {
                case 36207:                                     // Steal Weapon
                {
                    if (target->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (apply)
                    {
                        if (Player* playerCaster = GetCaster()->GetBeneficiaryPlayer())
                        {
                            if (Item* item = playerCaster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                            {
                                ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, item->GetEntry());
                            }
                        }
                    }
                    else
                    {
                        ((Creature*)target)->LoadEquipment(((Creature*)target)->GetCreatureInfo()->EquipmentTemplateId, true);
                    }
                    break;
                }
                case 30019:                                     // Control Piece - Chess
                {
                    if (apply || target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    target->CastSpell(target, 30529, TRIGGERED_OLD_TRIGGERED);
                    target->RemoveAurasDueToSpell(30019);
                    target->RemoveAurasDueToSpell(30532);

                    Unit* chessPiece = target->GetCharm();
                    if (chessPiece)
                        chessPiece->RemoveAurasDueToSpell(30019);

                    return;
                }
                case 39993: // Simon Game START timer, (DND)
                {
                    if (apply)
                        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, GetCaster(), target);
                    break;
                }
                case 44328: // Energy Feedback - Vexallus
                {
                    if (!apply)
                        if (Unit * caster = GetCaster())
                            caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, caster, caster);
                    break;
                }
                case 47214: // Burninate Effect
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        target->CastSpell(caster, 47208, TRIGGERED_NONE);
                        target->CastSpell(nullptr, 42726, TRIGGERED_NONE);
                    }
                    else // kill self on removal
                        target->CastSpell(nullptr, 51744, TRIGGERED_NONE);
                    break;
                }
            }
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (spell->Id)
            {
                // Master of Subtlety
                case 31666:
                {
                    if (apply)
                    {
                        // for make duration visible
                        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(31665))
                        {
                            holder->SetAuraMaxDuration(GetHolder()->GetAuraDuration());
                            holder->RefreshHolder();
                        }
                    }
                    else
                        target->RemoveAurasDueToSpell(31665);
                    break;
                }
            }
            break;
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicHeal(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    Unit* target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if (loading)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        // Gift of the Naaru (have diff spellfamilies)
        if (GetSpellProto()->SpellIconID == 329 && GetSpellProto()->SpellVisual[0] == 7625)
        {
            int32 ap = int32(0.22f * caster->GetTotalAttackPowerValue(BASE_ATTACK));
            int32 holy = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(GetSpellProto()));
            if (holy < 0)
                holy = 0;
            holy = int32(holy * 377 / 1000);
            m_modifier.m_amount += ap > holy ? ap : holy;
        }
        // Lifeblood
        else if (GetSpellProto()->SpellIconID == 3088 && GetSpellProto()->SpellVisual[0] == 8145)
        {
            int32 healthBonus = int32(0.0032f * caster->GetMaxHealth());
            m_modifier.m_amount += healthBonus;
        }

        switch (GetSpellProto()->Id)
        {
            case 12939: m_modifier.m_amount = target->GetMaxHealth() / 10; break; // Polymorph Heal Effect
            default: m_modifier.m_amount = caster->SpellHealingBonusDone(target, GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount()); break;
        }

        // Rejuvenation
        if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_DRUID, uint64(0x0000000000000010)))
            if (caster->HasAura(64760))                     // Item - Druid T8 Restoration 4P Bonus
                caster->CastCustomSpell(target, 64801, &m_modifier.m_amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr);
    }
}

void Aura::HandleDamagePercentTaken(bool apply, bool Real)
{
    m_isPeriodic = apply;

    Unit* target = GetTarget();

    if (!Real)
        return;

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    if (apply)
    {
        if (loading)
            return;

        // Hand of Salvation (only it have this aura and mask)
        if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_PALADIN, uint64(0x0000000000000100)))
        {
            // Glyph of Salvation
            if (target->GetObjectGuid() == GetCasterGuid())
                if (Aura* aur = target->GetAura(63225, EFFECT_INDEX_0))
                    m_modifier.m_amount -= aur->GetModifier()->m_amount;
        }
    }
    else
        if (GetSpellProto()->Id == 43421) // Malacrass - Lifebloom
            target->CastSpell(nullptr, 43422, TRIGGERED_OLD_TRIGGERED);
}

void Aura::HandleAuraDamageShield(bool apply, bool real)
{
    if (!real)
        return;

    Unit* target = GetTarget();
    if (apply)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        m_modifier.m_amount = caster->SpellDamageBonusDone(target, SpellSchoolMask(GetSpellProto()->SchoolMask), GetSpellProto(), m_modifier.m_amount, SPELL_DIRECT_DAMAGE, GetStackAmount());
    }
}

void Aura::HandlePeriodicDamage(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    m_isPeriodic = apply;

    Unit* target = GetTarget();
    SpellEntry const* spellProto = GetSpellProto();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if (loading)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        switch (spellProto->SpellFamilyName)
        {
            case SPELLFAMILY_WARRIOR:
            {
                // Rend
                if (spellProto->SpellFamilyFlags & uint64(0x0000000000000020))
                {
                    // $0.2*(($MWB+$mwb)/2+$AP/14*$MWS) bonus per tick
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 mws = caster->GetAttackTime(BASE_ATTACK);
                    float mwb_min = caster->GetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE);
                    float mwb_max = caster->GetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE);
                    m_modifier.m_amount += int32(((mwb_min + mwb_max) / 2 + ap * mws / 14000) * 0.2f);
                    // If used while target is above 75% health, Rend does 35% more damage
                    if (spellProto->CalculateSimpleValue(EFFECT_INDEX_1) != 0 &&
                            target->GetHealth() > target->GetMaxHealth() * spellProto->CalculateSimpleValue(EFFECT_INDEX_1) / 100)
                        m_modifier.m_amount += m_modifier.m_amount * spellProto->CalculateSimpleValue(EFFECT_INDEX_2) / 100;
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                // Rip
                if (spellProto->SpellFamilyFlags & uint64(0x000000000000800000))
                {
                    if (caster->GetTypeId() != TYPEID_PLAYER)
                        break;

                    // 0.01*$AP*cp
                    uint8 cp = caster->GetComboPoints();

                    // Idol of Feral Shadows. Cant be handled as SpellMod in SpellAura:Dummy due its dependency from CPs
                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (auto dummyAura : dummyAuras)
                    {
                        if (dummyAura->GetId() == 34241)
                        {
                            m_modifier.m_amount += cp * dummyAura->GetModifier()->m_amount;
                            break;
                        }
                    }
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * cp / 100);
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
            {
                // Rupture
                if (spellProto->SpellFamilyFlags & uint64(0x000000000000100000))
                {
                    if (caster->GetTypeId() != TYPEID_PLAYER)
                        break;
                    // 1 point : ${($m1+$b1*1+0.015*$AP)*4} damage over 8 secs
                    // 2 points: ${($m1+$b1*2+0.024*$AP)*5} damage over 10 secs
                    // 3 points: ${($m1+$b1*3+0.03*$AP)*6} damage over 12 secs
                    // 4 points: ${($m1+$b1*4+0.03428571*$AP)*7} damage over 14 secs
                    // 5 points: ${($m1+$b1*5+0.0375*$AP)*8} damage over 16 secs
                    float AP_per_combo[6] = {0.0f, 0.015f, 0.024f, 0.03f, 0.03428571f, 0.0375f};
                    uint8 cp = caster->GetComboPoints();
                    if (cp > 5) cp = 5;
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * AP_per_combo[cp]);
                }
                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                // Holy Vengeance / Blood Corruption
                if (spellProto->SpellFamilyFlags & uint64(0x0000080000000000) && spellProto->SpellVisual[0] == 7902)
                {
                    // AP * 0.025 + SPH * 0.013 bonus per tick
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(spellProto));
                    if (holy < 0)
                        holy = 0;
                    m_modifier.m_amount += int32(GetStackAmount()) * (int32(ap * 0.025f) + int32(holy * 13 / 1000));
                }
                break;
            }
            default:
                break;
        }

        if (m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
        {
            // SpellDamageBonusDone for magic spells
            if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE || spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                m_modifier.m_amount = caster->SpellDamageBonusDone(target, SpellSchoolMask(GetSpellProto()->SchoolMask), GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
            // MeleeDamagebonusDone for weapon based spells
            else
            {
                WeaponAttackType attackType = GetWeaponAttackType(GetSpellProto());
                m_modifier.m_amount = caster->MeleeDamageBonusDone(target, m_modifier.m_amount, attackType, SpellSchoolMask(spellProto->SchoolMask), spellProto, DOT, GetStackAmount());
            }
        }
    }
    // remove time effects
    else
    {
        switch (spellProto->Id)
        {
            case 23155: // Chromaggus - Brood Affliction: Red
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    target->CastSpell(nullptr, 23168, TRIGGERED_OLD_TRIGGERED); // Heal Chromaggus
                break;
            case 43364: // Tranquilizing Poison
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(nullptr, 24004, TRIGGERED_OLD_TRIGGERED); // Sleep
                break;
            case 35201: // Paralytic Poison
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                    target->CastSpell(target, 35202, TRIGGERED_OLD_TRIGGERED); // Paralysis
                break;
            case 39044: // Serpentshrine Parasite
            case 39053:
                if (m_removeMode == AURA_REMOVE_BY_DEATH && target->GetTypeId() == TYPEID_PLAYER)
                    target->CastSpell(target, 39045, TRIGGERED_OLD_TRIGGERED); // Summon Serpentshrine Parasite
                break;
        }
    }
}

void Aura::HandlePeriodicDamagePCT(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePeriodicLeech(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    // For prevent double apply bonuses
    bool loading = (GetTarget()->GetTypeId() == TYPEID_PLAYER && ((Player*)GetTarget())->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if (loading)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        m_modifier.m_amount = caster->SpellDamageBonusDone(GetTarget(), SpellSchoolMask(GetSpellProto()->SchoolMask), GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
    }
}

void Aura::HandlePeriodicManaLeech(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    if (!apply)
    {
        switch (GetId())
        {
            case 24002: // Tranquilizing Poison
            case 24003: // Tranquilizing Poison
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(nullptr, 24004, TRIGGERED_OLD_TRIGGERED); // Sleep
                break;
        }
    }
}

void Aura::HandlePeriodicHealthFunnel(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    // For prevent double apply bonuses
    bool loading = (GetTarget()->GetTypeId() == TYPEID_PLAYER && ((Player*)GetTarget())->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if (loading)
            return;

        Unit* caster = GetCaster();
        if (!caster)
            return;

        m_modifier.m_amount = caster->SpellDamageBonusDone(GetTarget(), SpellSchoolMask(GetSpellProto()->SchoolMask), GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
    }
}

/*********************************************************/
/***                  MODIFY STATS                     ***/
/*********************************************************/

/********************************/
/***        RESISTANCE        ***/
/********************************/

void Aura::HandleAuraModResistanceExclusive(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();

    for (uint32 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & (1 << i))
        {
            // Apply the value exclusively for each school
            const int32 amount = m_modifier.m_amount;
            int32 bonusMax = 0;
            int32 malusMax = 0;

            for (Aura* aura : target->GetAurasByType(m_modifier.m_auraname))
            {
                if (aura == this || !(aura->GetMiscValue() & (1 << i)))
                    continue;

                const int32 mod = aura->GetModifier()->m_amount;

                if (mod > bonusMax)
                    bonusMax = mod;
                else if (mod < malusMax)
                    malusMax = mod;
            }

            float oldResist = target->GetTotalResistanceValue(SpellSchools(i));
            if (amount > bonusMax)
            {
                target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(apply ? bonusMax : amount), false);
                target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(apply ? amount : bonusMax), true);
            }
            else if (amount < malusMax)
            {
                target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(apply ? malusMax : amount), false);
                target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(apply ? amount : malusMax), true);
            }
            float change = target->GetTotalResistanceValue(SpellSchools(i)) - oldResist;
            // UI malus info:
            target->ApplyResistanceBuffModsMod(SpellSchools(i), (m_modifier.m_amount > 0), apply ? change : -change, apply);
        }
    }
}

void Aura::HandleAuraModResistance(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();

    for (uint32 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & (1 << i))
        {
            float oldResist = target->GetTotalResistanceValue(SpellSchools(i));
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(m_modifier.m_amount), apply);
            float change = target->GetTotalResistanceValue(SpellSchools(i)) - oldResist;
            // UI bonus/malus info:
            target->ApplyResistanceBuffModsMod(SpellSchools(i), (m_modifier.m_amount > 0), apply ? change : -change, apply);
        }
    }
}
void Aura::HandleModResistancePercent(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();

    for (uint32 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & (1 << i))
        {
            float oldResist = target->GetTotalResistanceValue(SpellSchools(i));
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_PCT, float(m_modifier.m_amount), apply);
            float change = target->GetTotalResistanceValue(SpellSchools(i)) - oldResist;
            // UI bonus/malus info:
            target->ApplyResistanceBuffModsMod(SpellSchools(i), (m_modifier.m_amount > 0), apply ? change : -change, apply);
        }
    }
}

void Aura::HandleModBaseResistance(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();

    for (uint32 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & (1 << i))
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), BASE_VALUE, float(m_modifier.m_amount), apply);
    }
}

void Aura::HandleAuraModBaseResistancePercent(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();

    for (uint32 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & (1 << i))
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), BASE_PCT, float(m_modifier.m_amount), apply);
    }
}

/********************************/
/***           STAT           ***/
/********************************/

void Aura::HandleAuraModStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -2 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Spell %u effect %u have unsupported misc value (%i) for SPELL_AURA_MOD_STAT ", GetId(), GetEffIndex(), m_modifier.m_miscvalue);
        return;
    }

    Unit* target = GetTarget();

    // Holy Strength amount decrease by 4% each level after 60 From Crusader Enchant
    if (apply && GetId() == 20007)
        if (GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCaster()->GetLevel() > 60)
            m_modifier.m_amount = int32(m_modifier.m_amount * (1 - (((float(GetCaster()->GetLevel()) - 60) * 4) / 100)));

    if (GetSpellProto()->IsFitToFamilyMask(0x0000000000008000)) // improved scorpid sting
    {
        if (apply)
        {
            int32 staminaToRemove = 0;
            Unit::AuraList const& auraClassScripts = target->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
            for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end(); ++itr)
            {
                switch ((*itr)->GetModifier()->m_miscvalue)
                {
                    case 2388: staminaToRemove = m_modifier.m_amount * 10 / 100; break;
                    case 2389: staminaToRemove = m_modifier.m_amount * 20 / 100; break;
                    case 2390: staminaToRemove = m_modifier.m_amount * 30 / 100; break;
                }
            }
            if (staminaToRemove)
                GetCaster()->CastCustomSpell(target, 19486, &staminaToRemove, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
        else
            target->RemoveAurasTriggeredBySpell(GetId(), GetCasterGuid()); // just do it every time, lookup is too time consuming
    }

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        // -1 or -2 is all stats ( misc < -2 checked in function beginning )
        if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue == i)
        {
            float amount = m_removeMode == AURA_REMOVE_BY_GAINED_STACK ? float(m_modifier.m_recentAmount) : float(m_modifier.m_amount);
            // m_target->ApplyStatMod(Stats(i), m_modifier.m_amount,apply);
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, amount, apply);
            if (target->GetTypeId() == TYPEID_PLAYER || ((Creature*)target)->IsPet())
                target->ApplyStatBuffMod(Stats(i), amount, apply);
        }
    }
}

void Aura::HandleModPercentStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -1 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Misc Value for SPELL_AURA_MOD_PERCENT_STAT not valid");
        return;
    }

    // only players have base stats
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (m_modifier.m_miscvalue == i || m_modifier.m_miscvalue == -1)
            GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), BASE_PCT, float(m_modifier.m_amount), apply);
    }
}

void Aura::HandleModSpellDamagePercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageBonus();
}

void Aura::HandleModSpellHealingPercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellHealingBonus();
}

void Aura::HandleAuraModDispelResist(bool apply, bool Real)
{
    if (!Real || !apply)
        return;
}

void Aura::HandleModSpellDamagePercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageBonus();
}

void Aura::HandleModSpellHealingPercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellHealingBonus();
}

void Aura::HandleModHealingDone(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;
    // implemented in Unit::SpellHealingBonusDone
    // this information is for client side only
    ((Player*)GetTarget())->UpdateSpellHealingBonus();
}

void Aura::HandleModTotalPercentStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -1 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Misc Value for SPELL_AURA_MOD_PERCENT_STAT not valid");
        return;
    }

    Unit* target = GetTarget();

    // save current and max HP before applying aura
    uint32 curHPValue = target->GetHealth();
    uint32 maxHPValue = target->GetMaxHealth();

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (m_modifier.m_miscvalue == i || m_modifier.m_miscvalue == -1)
        {
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, float(m_modifier.m_amount), apply);
            if (target->GetTypeId() == TYPEID_PLAYER || ((Creature*)target)->IsPet())
                target->ApplyStatPercentBuffMod(Stats(i), float(m_modifier.m_amount), apply);
        }
    }

    // recalculate current HP/MP after applying aura modifications (only for spells with 0x10 flag)
    if (m_modifier.m_miscvalue == STAT_STAMINA && maxHPValue > 0 && GetSpellProto()->HasAttribute(SPELL_ATTR_IS_ABILITY) && target->GetHealth() > 0)
    {
        // newHP = (curHP / maxHP) * newMaxHP = (newMaxHP * curHP) / maxHP -> which is better because no int -> double -> int conversion is needed
        uint32 newHPValue = std::max(1u, (target->GetMaxHealth() * curHPValue) / maxHPValue);
        target->SetHealth(newHPValue);
    }
}

void Aura::HandleAuraModResistenceOfStatPercent(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_modifier.m_miscvalue != SPELL_SCHOOL_MASK_NORMAL)
    {
        // support required adding replace UpdateArmor by loop by UpdateResistence at intellect update
        // and include in UpdateResistence same code as in UpdateArmor for aura mod apply.
        sLog.outError("Aura SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT(182) need adding support for non-armor resistances!");
        return;
    }

    // Recalculate Armor
    GetTarget()->UpdateArmor();
}

/********************************/
/***      HEAL & ENERGIZE     ***/
/********************************/
void Aura::HandleAuraModTotalHealthPercentRegen(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
    if (GetId() == 40409)
    {
        if (apply)
            GetTarget()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        else
            GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }
}

void Aura::HandleAuraModTotalManaPercentRegen(bool apply, bool /*Real*/)
{
    if (m_modifier.periodictime == 0)
        m_modifier.periodictime = 1000;

    m_periodicTimer = m_modifier.periodictime;
    m_isPeriodic = apply;

    if (GetId() == 30024 && !apply && m_removeMode == AURA_REMOVE_BY_DEFAULT) // Shade of Aran drink on interrupt
    {
        Unit* target = GetTarget();
        UnitAI* ai = GetTarget()->AI();
        if (ai && target->GetTypeId() == TYPEID_UNIT)
            ai->SendAIEvent(AI_EVENT_CUSTOM_A, target, static_cast<Creature*>(target));
    }
}

void Aura::HandleModRegen(bool apply, bool /*Real*/)        // eating
{
    if (m_modifier.periodictime == 0)
        m_modifier.periodictime = 5000;

    m_periodicTimer = 5000;
    m_isPeriodic = apply;
}

void Aura::HandleModPowerRegen(bool apply, bool Real)       // drinking
{
    if (!Real)
        return;

    Powers powerType = GetTarget()->GetPowerType();
    if (m_modifier.periodictime == 0)
    {
        // Anger Management (only spell use this aura for rage)
        if (powerType == POWER_RAGE)
            m_modifier.periodictime = 3000;
        else
            m_modifier.periodictime = 2000;
    }

    m_periodicTimer = 5000;

    if (GetTarget()->GetTypeId() == TYPEID_PLAYER && m_modifier.m_miscvalue == POWER_MANA)
        static_cast<Player*>(GetTarget())->UpdateManaRegen();

    m_isPeriodic = apply;
}

void Aura::HandleModPowerRegenPCT(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Update manaregen value
    if (m_modifier.m_miscvalue == POWER_MANA)
        static_cast<Player*>(GetTarget())->UpdateManaRegen();
    else if (m_modifier.m_miscvalue == POWER_ENERGY)
        static_cast<Player*>(GetTarget())->UpdateEnergyRegen();
}

void Aura::HandleModHealingPCT(bool apply, bool real)
{
    switch (GetId())
    {
        case 41292: // Aura of Suffering - Reliquary of Souls
        {
            if (apply)
                GetTarget()->CastSpell(nullptr, 42017, TRIGGERED_OLD_TRIGGERED);
            else
                GetTarget()->RemoveAurasDueToSpell(42017);
            break;
        }
    }
}

void Aura::HandleModManaRegen(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Note: an increase in regen does NOT cause threat.
    ((Player*)GetTarget())->UpdateManaRegen();
}

void Aura::HandleComprehendLanguage(bool apply, bool /*Real*/)
{
    if (apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
}

void Aura::HandleAuraModIncreaseHealth(bool apply, bool Real)
{
    Unit* target = GetTarget();

    switch (GetId())
    {
        // Special case with temporary increase max/current health
        // Cases where we need to manually calculate the amount for the spell (by percentage)
        // recalculate to full amount at apply for proper remove
        case 54443:                                         // Demonic Empowerment (Voidwalker)
        case 55233:                                         // Vampiric Blood
        case 61254:                                         // Will of Sartharion (Obsidian Sanctum)
            if (Real && apply)
                m_modifier.m_amount = target->GetMaxHealth() * m_modifier.m_amount / 100;
        // no break here

        // Cases where m_amount already has the correct value (spells cast with CastCustomSpell or absolute values)
        case 4148:                                          // Growl of Fortitude
        case 12976:                                         // Warrior Last Stand triggered spell (Cast with percentage-value by CastCustomSpell)
        case 16609:                                         // Warchief's Blessing
        case 28506:                                         // Potion of Heroes (i.22837 - Heroic Potion)
        case 28726:                                         // Nightmare Seed
        case 31616:                                         // Nature's Guardian (Cast with percentage-value by CastCustomSpell)
        case 33053:                                         // Mr Pinchy's Blessing (s.33060 - Make a Wish)
        case 33668:                                         // Tenacity (i.28042 - Regal Protectorate)
        case 39625:                                         // Elixir of Major Fortitude (i.32062 - Elixir of Major Fortitude)
        case 34410:                                         // Hellscream's Warsong (Thrall Nagrand)
        case 34511:                                         // Valor (Bulwark of Kings, Bulwark of the Ancient Kings)
        case 40464:                                         // Protector's Vigor (i.32501 - Shadowmoon Insignia)
        case 40538:                                         // Tenacity (i.32534 - Brooch of the Immortal King)
        case 44055: case 55915: case 55917: case 67596:     // Tremendous Fortitude (Battlemaster's Alacrity)
        case 45049:                                         // Tenacity (i.34428 - Steely Naaru Sliver)
        case 46302:                                         // K'iru's Song of Victory (Isle of Quel'Danas Zone Aura)
        case 50322:                                         // Survival Instincts (Cast with percentage-value by CastCustomSpell)
        case 53479:                                         // Hunter pet - Last Stand (Cast with percentage-value by CastCustomSpell)
        case 59465:                                         // Brood Rage (Ahn'Kahet)
        {
            if (Real)
            {
                if (apply) // code confirmed for last stand
                {
                    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
                    target->ModifyHealth(m_modifier.m_amount);
                }
                else
                {
                    if (m_removeMode != AURA_REMOVE_BY_DEATH)
                    {
                        if (int32(target->GetHealth()) > m_modifier.m_amount)
                            target->ModifyHealth(-m_modifier.m_amount);
                        else
                            target->SetHealth(1);
                    }

                    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
                }
            }
            return;
        }
        // confirmed to keep percentage on unapply
        case 40604:                                         // Fel Rage - Gurtogg Bloodboil
        {
            float percentage = target->GetHealthPercent();
            target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            if (apply)
                target->ModifyHealth(m_modifier.m_amount);
            else
                target->SetHealthPercent(percentage);
            return;
        }
        case 30421:
        {
            if (m_removeMode != AURA_REMOVE_BY_GAINED_STACK)
                target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(30000 + m_modifier.m_amount), apply);
            else
                target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_recentAmount), apply);
            if (apply)
                target->SetHealth(target->GetMaxHealth());
            else if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                target->CastSpell(target, 38637, TRIGGERED_OLD_TRIGGERED);
            return;
        }
        // generic case
        default:
            if (m_removeMode != AURA_REMOVE_BY_GAINED_STACK)
                target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            else
                target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_recentAmount), apply);
            break;
    }
}

void Aura::HandleAuraModIncreaseMaxHealth(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    uint32 oldhealth = target->GetHealth();
    double healthPercentage = (double)oldhealth / (double)target->GetMaxHealth();

    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);

    // refresh percentage
    if (oldhealth > 0)
    {
        uint32 newhealth = std::max(1u, uint32(ceil((double)target->GetMaxHealth() * healthPercentage)));
        target->SetHealth(newhealth);
    }
}

void Aura::HandleAuraModIncreaseEnergy(bool apply, bool Real)
{
    Unit* target = GetTarget();
    Powers powerType = Powers(m_modifier.m_miscvalue);

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    // Special case with temporary increase max/current power (percent)
    if (GetId() == 64904)                                   // Hymn of Hope
    {
        if (Real)
        {
            uint32 val = target->GetPower(powerType);
            target->HandleStatModifier(unitMod, TOTAL_PCT, float(m_modifier.m_amount), apply);
            target->SetPower(powerType, apply ? val * (100 + m_modifier.m_amount) / 100 : val * 100 / (100 + m_modifier.m_amount));
        }
        return;
    }

    // generic flat case
    target->HandleStatModifier(unitMod, TOTAL_VALUE, float(m_removeMode == AURA_REMOVE_BY_GAINED_STACK ? m_modifier.m_recentAmount : m_modifier.m_amount), apply);
}

void Aura::HandleAuraModIncreaseEnergyPercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    Powers powerType = Powers(m_modifier.m_miscvalue);

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    target->HandleStatModifier(unitMod, TOTAL_PCT, float(m_modifier.m_amount), apply);
    target->ModifyPower(powerType, apply ? m_modifier.m_amount : -m_modifier.m_amount);
}

void Aura::HandleAuraModIncreaseHealthPercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    uint32 oldMaxHealth = target->GetMaxHealth();
    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(m_modifier.m_amount), apply);

    // spell special cases when current health set to max value at apply
    switch (GetId())
    {
        case 802:                           // Mutate Bug
        case 40851:                         // Disgruntled
            if (apply)
                target->ModifyHealth(target->GetMaxHealth() - oldMaxHealth);
            break;
        case 60430:                                         // Molten Fury
        case 64193:                                         // Heartbreak
        case 65737:                                         // Heartbreak
            target->SetHealth(target->GetMaxHealth());
            break;
        default:
            break;
    }
}

void Aura::HandleAuraIncreaseBaseHealthPercent(bool apply, bool /*Real*/)
{
    GetTarget()->HandleStatModifier(UNIT_MOD_HEALTH, BASE_PCT, float(m_modifier.m_amount), apply);
}

/********************************/
/***          FIGHT           ***/
/********************************/

void Aura::HandleAuraModParryPercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
    {
        target->m_modParryChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        return;
    }

    ((Player*)target)->UpdateParryPercentage();
}

void Aura::HandleAuraModDodgePercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
    {
        target->m_modDodgeChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        return;
    }

    ((Player*)target)->UpdateDodgePercentage();
    // sLog.outError("BONUS DODGE CHANCE: + %f", float(m_modifier.m_amount));
}

void Aura::HandleAuraModBlockPercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
    {
        target->m_modBlockChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        return;
    }

    ((Player*)target)->UpdateBlockPercentage();
    // sLog.outError("BONUS BLOCK CHANCE: + %f", float(m_modifier.m_amount));
}

void Aura::HandleAuraModRegenInterrupt(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateManaRegen();
}

void Aura::HandleAuraModCritPercent(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
    {
        for (float& i : target->m_modCritChance)
            i += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        return;
    }

    // apply item specific bonuses for already equipped weapon
    if (Real)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i), true, false))
                ((Player*)target)->_ApplyWeaponDependentAuraCritMod(pItem, WeaponAttackType(i), this, apply);
    }

    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if (GetSpellProto()->EquippedItemClass == -1)
    {
        ((Player*)target)->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float(m_modifier.m_amount), apply);
        ((Player*)target)->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float(m_modifier.m_amount), apply);
        ((Player*)target)->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float(m_modifier.m_amount), apply);
    }
    else
    {
        // done in Player::_ApplyWeaponDependentAuraMods
    }
}

void Aura::HandleModHitChance(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)target)->UpdateMeleeHitChances();
        ((Player*)target)->UpdateRangedHitChances();
    }
    else
    {
        target->m_modMeleeHitChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        target->m_modRangedHitChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
    }
}

void Aura::HandleModSpellHitChance(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)GetTarget())->UpdateSpellHitChances();
    }
    else
    {
        GetTarget()->m_modSpellHitChance += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
    }
}

void Aura::HandleModSpellCritChance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_UNIT)
    {
        for (uint8 school = SPELL_SCHOOL_NORMAL; school < MAX_SPELL_SCHOOL; ++school)
            target->m_modSpellCritChance[school] += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
        return;
    }

    ((Player*)target)->UpdateAllSpellCritChances();
}

void Aura::HandleModSpellCritChanceShool(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    for (uint8 school = SPELL_SCHOOL_NORMAL; school < MAX_SPELL_SCHOOL; ++school)
    {
        if (m_modifier.m_miscvalue & (1 << school))
        {
            if (target->GetTypeId() == TYPEID_UNIT)
                target->m_modSpellCritChance[school] += (apply ? m_modifier.m_amount : -m_modifier.m_amount);
            else
                ((Player*)target)->UpdateSpellCritChance(school);
        }
    }
}

/********************************/
/***         ATTACK SPEED     ***/
/********************************/

void Aura::HandleModCastingSpeed(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    std::set<uint32> exclusiveAuras = { 32182, 2825, 49725 };
    if (exclusiveAuras.find(GetId()) != exclusiveAuras.end())
    {
        auto& auras = target->GetAurasByType(m_modifier.m_auraname);
        int32 max = 0;
        for (auto& data : auras)
        {
            if (this != data && exclusiveAuras.find(data->GetId()) != exclusiveAuras.end() && max < data->GetAmount())
                max = data->GetAmount();
        }
        if (m_modifier.m_amount < max)
            return;
        target->ApplyCastTimePercentMod(float(max), !apply);
    }
    target->ApplyCastTimePercentMod(float(m_modifier.m_amount), apply);
}

void Aura::HandleModMeleeRangedSpeedPct(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    std::set<uint32> exclusiveAuras = { 32182, 2825, 45856 };
    if (exclusiveAuras.find(GetId()) != exclusiveAuras.end())
    {
        auto& auras = target->GetAurasByType(m_modifier.m_auraname);
        int32 max = 0;
        for (auto& data : auras)
        {
            if (this != data && exclusiveAuras.find(data->GetId()) != exclusiveAuras.end() && max < data->GetAmount())
                max = data->GetAmount();
        }
        if (m_modifier.m_amount < max)
            return;
        target->ApplyAttackTimePercentMod(BASE_ATTACK, float(max), !apply);
        target->ApplyAttackTimePercentMod(OFF_ATTACK, float(max), !apply);
        target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(max), !apply);
    }
    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleModCombatSpeedPct(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    target->ApplyCastTimePercentMod(float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleModAttackSpeed(bool apply, bool /*Real*/)
{
    GetTarget()->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleModMeleeSpeedPct(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);

    if (GetId() == 42583)
        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, target, target, apply);
}

void Aura::HandleAuraModRangedHaste(bool apply, bool /*Real*/)
{
    GetTarget()->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleRangedAmmoHaste(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;
    GetTarget()->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

/********************************/
/***        ATTACK POWER      ***/
/********************************/

void Aura::HandleAuraModAttackPower(bool apply, bool /*Real*/)
{
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedAttackPower(bool apply, bool /*Real*/)
{
    if ((GetTarget()->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModAttackPowerPercent(bool apply, bool /*Real*/)
{
    // UNIT_FIELD_ATTACK_POWER_MULTIPLIER = multiplier - 1
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedAttackPowerPercent(bool apply, bool /*Real*/)
{
    if ((GetTarget()->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    // UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = multiplier - 1
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedAttackPowerOfStatPercent(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    // Recalculate bonus
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER && !(GetTarget()->getClassMask() & CLASSMASK_WAND_USERS))
        ((Player*)GetTarget())->UpdateAttackPowerAndDamage(true);
}

void Aura::HandleAuraModAttackPowerOfStatPercent(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    // Recalculate bonus
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->UpdateAttackPowerAndDamage(false);
}

void Aura::HandleAuraModAttackPowerOfArmor(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    // Recalculate bonus
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->UpdateAttackPowerAndDamage(false);
}
/********************************/
/***        DAMAGE BONUS      ***/
/********************************/
void Aura::HandleModDamageDone(bool apply, bool Real)
{
    Unit* target = GetTarget();

    // apply item specific bonuses for already equipped weapon
    if (Real && target->GetTypeId() == TYPEID_PLAYER)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i), true, false))
                ((Player*)target)->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 ( 0-bit ) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wands
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (GetSpellProto()->EquippedItemClass == -1 || !target->IsPlayer())
        {
            target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_VALUE, float(m_modifier.m_amount), apply);
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
    }

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    if (target->IsPlayer())
    {
        // This information for client side use only
        // Recalculate bonus
        static_cast<Player*>(GetTarget())->UpdateSpellDamageBonus();
        if (Pet* pet = target->GetPet())
            pet->UpdateScalingAuras();
    }
}

void Aura::HandleModDamagePercentDone(bool apply, bool Real)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "AURA MOD DAMAGE type:%u negative:%u", m_modifier.m_miscvalue, m_positive ? 0 : 1);
    Unit* target = GetTarget();

    // apply item specific bonuses for already equipped weapon
    if (Real && target->GetTypeId() == TYPEID_PLAYER)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i), true, false))
                ((Player*)target)->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 ( 0-bit ) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wand
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (GetSpellProto()->EquippedItemClass == -1 || target->GetTypeId() != TYPEID_PLAYER)
        {
            target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_PCT, float(m_modifier.m_amount), apply);

            // For show in client
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT, m_modifier.m_amount / 100.0f, apply);
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
    }

    // Skip non magic case for speedup
    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if (GetSpellProto()->EquippedItemClass != -1 || GetSpellProto()->EquippedItemInventoryTypeMask != 0)
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    // Magic damage percent modifiers implemented in Unit::SpellDamageBonusDone
    // Send info to client
    if (target->GetTypeId() == TYPEID_PLAYER)
        for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            if (m_modifier.m_miscvalue & (1 << i))
                target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, m_modifier.m_amount / 100.0f, apply);

    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
        if (GetId() == 30423)
            target->CastSpell(target, 38639, TRIGGERED_OLD_TRIGGERED);
}

void Aura::HandleModOffhandDamagePercent(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "AURA MOD OFFHAND DAMAGE");

    GetTarget()->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

/********************************/
/***        POWER COST        ***/
/********************************/

void Aura::HandleModPowerCostPCT(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    float amount = (m_removeMode == AURA_REMOVE_BY_GAINED_STACK ? m_modifier.m_recentAmount : m_modifier.m_amount) / 100.0f;
    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (m_modifier.m_miscvalue & (1 << i))
            target->ApplyModSignedFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + i, amount, apply);

    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
        if (GetId() == 30422)
            target->CastSpell(target, 38638, TRIGGERED_OLD_TRIGGERED);
}

void Aura::HandleModPowerCost(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (m_modifier.m_miscvalue & (1 << i))
            GetTarget()->ApplyModInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + i, m_modifier.m_amount, apply);
}

void Aura::HandleNoReagentUseAura(bool /*Apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;
    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    ClassFamilyMask mask;
    Unit::AuraList const& noReagent = target->GetAurasByType(SPELL_AURA_NO_REAGENT_USE);
    for (auto i : noReagent)
        mask |= i->GetAuraSpellClassMask();

    target->SetUInt64Value(PLAYER_NO_REAGENT_COST_1 + 0, mask.Flags);
    target->SetUInt32Value(PLAYER_NO_REAGENT_COST_1 + 2, mask.Flags2);
}

/*********************************************************/
/***                    OTHERS                         ***/
/*********************************************************/

void Aura::HandleShapeshiftBoosts(bool apply)
{
    uint32 spellId1 = 0;
    uint32 spellId2 = 0;
    uint32 HotWSpellId = 0;
    uint32 MasterShaperSpellId = 0;

    ShapeshiftForm form = ShapeshiftForm(GetModifier()->m_miscvalue);

    Unit* target = GetTarget();

    switch (form)
    {
        case FORM_CAT:
            spellId1 = 3025;
            HotWSpellId = 24900;
            MasterShaperSpellId = 48420;
            break;
        case FORM_TREE:
            spellId1 = 5420;
            spellId2 = 34123;
            MasterShaperSpellId = 48422;
            break;
        case FORM_TRAVEL:
            spellId1 = 5419;
            break;
        case FORM_AQUA:
            spellId1 = 5421;
            break;
        case FORM_BEAR:
            spellId1 = 1178;
            spellId2 = 21178;
            HotWSpellId = 24899;
            MasterShaperSpellId = 48418;
            break;
        case FORM_DIREBEAR:
            spellId1 = 9635;
            spellId2 = 21178;
            HotWSpellId = 24899;
            MasterShaperSpellId = 48418;
            break;
        case FORM_BATTLESTANCE:
            spellId1 = 21156;
            break;
        case FORM_DEFENSIVESTANCE:
            spellId1 = 7376;
            break;
        case FORM_BERSERKERSTANCE:
            spellId1 = 7381;
            break;
        case FORM_MOONKIN:
            spellId1 = 24905;
            spellId2 = 69366;
            MasterShaperSpellId = 48421;
            break;
        case FORM_FLIGHT:
            spellId1 = 33948;
            spellId2 = 34764;
            break;
        case FORM_FLIGHT_EPIC:
            spellId1 = 40122;
            spellId2 = 40121;
            break;
        case FORM_METAMORPHOSIS:
            spellId1 = 54817;
            spellId2 = 54879;
            break;
        case FORM_SPIRITOFREDEMPTION:
            spellId1 = 27792;
            spellId2 = 27795;                               // must be second, this important at aura remove to prevent to early iterator invalidation.
            break;
        case FORM_SHADOW:
            spellId1 = 49868;
            spellId2 = 71167;

            if (target->GetTypeId() == TYPEID_PLAYER)     // Spell 49868 have same category as main form spell and share cooldown
                target->RemoveSpellCooldown(49868);
            break;
        case FORM_GHOSTWOLF:
            spellId1 = 67116;
            break;
        case FORM_AMBIENT:
        case FORM_GHOUL:
        case FORM_STEALTH:
        case FORM_CREATURECAT:
        case FORM_CREATUREBEAR:
        case FORM_STEVES_GHOUL:
        case FORM_THARONJA_SKELETON:
        case FORM_TEST_OF_STRENGTH:
        case FORM_BLB_PLAYER:
        case FORM_SHADOW_DANCE:
        case FORM_TEST:
        case FORM_ZOMBIE:
        case FORM_UNDEAD:
        case FORM_FRENZY:
        case FORM_NONE:
            break;
        default:
            break;
    }

    if (apply)
    {
        if (spellId1)
            target->CastSpell(nullptr, spellId1, TRIGGERED_OLD_TRIGGERED, nullptr, this);
        if (spellId2)
            target->CastSpell(nullptr, spellId2, TRIGGERED_OLD_TRIGGERED, nullptr, this);

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            const PlayerSpellMap& sp_list = ((Player*)target)->GetSpellMap();
            for (const auto& itr : sp_list)
            {
                if (itr.second.state == PLAYERSPELL_REMOVED) continue;
                if (itr.first == spellId1 || itr.first == spellId2) continue;
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(itr.first);
                if (!spellInfo || !IsNeedCastSpellAtFormApply(spellInfo, form))
                    continue;
                target->CastSpell(nullptr, itr.first, TRIGGERED_OLD_TRIGGERED, nullptr, this);
            }
            // remove auras that do not require shapeshift, but are not active in this specific form (like Improved Barkskin)
            Unit::SpellAuraHolderMap& tAuras = target->GetSpellAuraHolderMap();
            for (Unit::SpellAuraHolderMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
            {
                SpellEntry const* spellInfo = itr->second->GetSpellProto();
                if (itr->second->IsPassive() && spellInfo->HasAttribute(SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED)
                        && (spellInfo->StancesNot[0] & (1 << (form - 1))))
                {
                    target->RemoveAurasDueToSpell(itr->second->GetId());
                    itr = tAuras.begin();
                }
                else
                    ++itr;
            }

            // Master Shapeshifter
            if (MasterShaperSpellId)
            {
                Unit::AuraList const& ShapeShifterAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
                for (auto ShapeShifterAura : ShapeShifterAuras)
                {
                    if (ShapeShifterAura->GetSpellProto()->SpellIconID == 2851)
                    {
                        int32 ShiftMod = ShapeShifterAura->GetModifier()->m_amount;
                        target->CastCustomSpell(nullptr, MasterShaperSpellId, &ShiftMod, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                        break;
                    }
                }
            }

            // Leader of the Pack
            if (((Player*)target)->HasSpell(17007))
            {
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(24932);
                if (spellInfo && spellInfo->Stances[0] & (1 << (form - 1)))
                    target->CastSpell(nullptr, 24932, TRIGGERED_OLD_TRIGGERED, nullptr, this);
            }

            // Savage Roar
            if (form == FORM_CAT && ((Player*)target)->HasAura(52610))
                target->CastSpell(nullptr, 62071, TRIGGERED_OLD_TRIGGERED);

            // Survival of the Fittest (Armor part)
            if (form == FORM_BEAR || form == FORM_DIREBEAR)
            {
                Unit::AuraList const& modAuras = target->GetAurasByType(SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
                for (auto modAura : modAuras)
                {
                    if (modAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID &&
                        modAura->GetSpellProto()->SpellIconID == 961)
                    {
                        int32 bp = modAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);
                        if (bp)
                            target->CastCustomSpell(nullptr, 62069, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        break;
                    }
                }
            }

            // Improved Moonkin Form
            if (form == FORM_MOONKIN)
            {
                Unit::AuraList const& dummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
                for (auto dummyAura : dummyAuras)
                {
                    if (dummyAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID &&
                        dummyAura->GetSpellProto()->SpellIconID == 2855)
                    {
                        uint32 spell_id = 0;
                        switch (dummyAura->GetId())
                        {
                            case 48384: spell_id = 50170; break; // Rank 1
                            case 48395: spell_id = 50171; break; // Rank 2
                            case 48396: spell_id = 50172; break; // Rank 3
                            default:
                                sLog.outError("Aura::HandleShapeshiftBoosts: Not handled rank of IMF (Spell: %u)", dummyAura->GetId());
                                break;
                        }

                        if (spell_id)
                            target->CastSpell(nullptr, spell_id, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        break;
                    }
                }
            }

            // Heart of the Wild
            if (HotWSpellId)
            {
                Unit::AuraList const& mModTotalStatPct = target->GetAurasByType(SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
                for (auto i : mModTotalStatPct)
                {
                    if (i->GetSpellProto()->SpellIconID == 240 && i->GetModifier()->m_miscvalue == 3)
                    {
                        int32 HotWMod = i->GetModifier()->m_amount / 2;
                        target->CastCustomSpell(nullptr, HotWSpellId, &HotWMod, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if (spellId1)
            target->RemoveAurasDueToSpell(spellId1);
        if (spellId2)
            target->RemoveAurasDueToSpell(spellId2);
        if (MasterShaperSpellId)
            target->RemoveAurasDueToSpell(MasterShaperSpellId);

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            // re-apply passive spells that don't need shapeshift but were inactive in current form:
            const PlayerSpellMap& sp_list = ((Player*)target)->GetSpellMap();
            for (const auto& itr : sp_list)
            {
                if (itr.second.state == PLAYERSPELL_REMOVED) continue;
                if (itr.first == spellId1 || itr.first == spellId2) continue;
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(itr.first);
                if (!spellInfo || !IsPassiveSpell(spellInfo))
                    continue;
                if (spellInfo->HasAttribute(SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED) && (spellInfo->StancesNot[0] & (1 << (form - 1))))
                    target->CastSpell(nullptr, itr.first, TRIGGERED_OLD_TRIGGERED, nullptr, this);
            }
        }

        Unit::SpellAuraHolderMap& tAuras = target->GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
        {
            if (itr->second->IsRemovedOnShapeLost())
            {
                target->RemoveAurasDueToSpell(itr->second->GetId());
                itr = tAuras.begin();
            }
            else
                ++itr;
        }
    }
}

void Aura::HandleAuraEmpathy(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    // This aura is expected to only work with CREATURE_TYPE_BEAST or players
    CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(target->GetEntry());
    if (target->GetTypeId() == TYPEID_PLAYER || (target->GetTypeId() == TYPEID_UNIT && ci && ci->CreatureType == CREATURE_TYPE_BEAST))
        target->ApplyModUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO, apply);

    if (apply)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* playerCaster = caster->IsPlayer() ? static_cast<Player*>(caster) : nullptr)
            {
                UpdateData updateData;
                target->BuildValuesUpdateBlockForPlayerWithFlags(updateData, playerCaster, UpdateFieldFlags(UF_FLAG_SPECIAL_INFO | UF_FLAG_DYNAMIC));
                updateData.SendData(*playerCaster->GetSession());
            }
        }
    }
}

void Aura::HandleAuraUntrackable(bool apply, bool /*Real*/)
{
    if (apply)
        GetTarget()->SetVisFlags(UNIT_VIS_FLAG_UNTRACKABLE);
    else
        GetTarget()->RemoveVisFlags(UNIT_VIS_FLAG_UNTRACKABLE);
}

void Aura::HandleAuraModPacify(bool apply, bool /*Real*/)
{
    if (apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
}

void Aura::HandleAuraModPacifyAndSilence(bool apply, bool Real)
{
    HandleAuraModPacify(apply, Real);
    HandleAuraModSilence(apply, Real);
    if (!apply && GetId() == 42354) // Anzu - Banish
        if (UnitAI* ai = GetTarget()->AI())
            ai->SendAIEvent(AI_EVENT_CUSTOM_A, GetTarget(), GetTarget());
}

void Aura::HandleAuraGhost(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    Player* player = target->IsPlayer() ? (Player*)target : nullptr;

    if (apply)
    {
        target->SetVisFlags(UNIT_VIS_FLAG_GHOST);
        if (player)
        {
            player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
            if (!player->HasAuraType(SPELL_AURA_WATER_WALK))
                player->SetWaterWalk(true);
        }
    }
    else
    {
        target->RemoveVisFlags(UNIT_VIS_FLAG_GHOST);
        if (player)
        {
            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
            if (!player->HasAuraType(SPELL_AURA_WATER_WALK))
                player->SetWaterWalk(false);
        }
    }

    if (player && player->GetGroup())
        player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_STATUS);
}

void Aura::HandleAuraAllowFlight(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    GetTarget()->SetCanFly(apply);
}

void Aura::HandleModRating(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
        if (m_modifier.m_miscvalue & (1 << rating))
            ((Player*)GetTarget())->ApplyRatingMod(CombatRating(rating), m_modifier.m_amount, apply);
}

void Aura::HandleModRatingFromStat(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;
    // Just recalculate ratings
    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
        if (m_modifier.m_miscvalue & (1 << rating))
            ((Player*)GetTarget())->ApplyRatingMod(CombatRating(rating), 0, apply);
}

void Aura::HandleForceMoveForward(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
}

void Aura::HandleAuraModExpertise(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateExpertise(BASE_ATTACK);
    ((Player*)GetTarget())->UpdateExpertise(OFF_ATTACK);
}

void Aura::HandleModTargetResistance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;
    Unit* target = GetTarget();
    // applied to damage as HandleNoImmediateEffect in Unit::CalculateAbsorbAndResist and Unit::CalcArmorReducedDamage
    // show armor penetration
    if (target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, m_modifier.m_amount, apply);

    // show as spell penetration only full spell penetration bonuses (all resistances except armor and holy
    if (target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_SPELL) == SPELL_SCHOOL_MASK_SPELL)
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, m_modifier.m_amount, apply);
}

void Aura::HandleShieldBlockValue(bool apply, bool /*Real*/)
{
    BaseModType modType = FLAT_MOD;
    if (m_modifier.m_auraname == SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT)
        modType = PCT_MOD;

    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->HandleBaseModValue(SHIELD_BLOCK_VALUE, modType, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraRetainComboPoints(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* target = (Player*)GetTarget();

    // combo points was added in SPELL_EFFECT_ADD_COMBO_POINTS handler
    // remove only if aura expire by time (in case combo points amount change aura removed without combo points lost)
    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE && target->GetComboTargetGuid())
        if (Unit* unit = ObjectAccessor::GetUnit(*GetTarget(), target->GetComboTargetGuid()))
            target->AddComboPoints(unit, -m_modifier.m_amount);
}

void Aura::HandleModUnattackable(bool apply, bool Real)
{
    Unit* target = GetTarget();

    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE, apply);

    if (Real && apply)
    {
        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        {
            InstanceData* instance = target->GetInstanceData();
            if (instance && sWorld.getConfig(CONFIG_BOOL_INSTANCE_STRICT_COMBAT_LOCKDOWN) && instance->IsEncounterInProgress())
                target->AttackStop(true, false, true);
            else
                target->CombatStop();
        }
        else
            target->AttackStop(true, false, true);
    }
}

void Aura::HandleSpiritOfRedemption(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->ApplySpellImmune(this, IMMUNITY_DAMAGE, 255, apply); // wotlk style hack - immunity against all damage

    // prepare spirit state
    if (apply)
    {
        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            // set stand state (expected in this form)
            if (!target->IsStandState())
                target->SetStandState(UNIT_STAND_STATE_STAND);
        }

        // interrupt casting when entering Spirit of Redemption
        if (target->IsNonMeleeSpellCasted(false))
            target->InterruptNonMeleeSpells(false);

        // set health and mana to maximum        
        target->SetPower(POWER_MANA, target->GetMaxPower(POWER_MANA));
    }
    // die at aura end
    else
        target->CastSpell(nullptr, 27965, TRIGGERED_OLD_TRIGGERED); // Suicide
}

void Aura::HandleSchoolAbsorb(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    Unit* target = GetTarget();
    SpellEntry const* spellProto = GetSpellProto();
    if (!apply)
    {
        switch (spellProto->Id)
        {
            case 33810: // Rock Shell
                if (m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK)
                    caster->CastSpell(caster, 33811, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                break;
        }
        if (caster &&
                // Power Word: Shield
                spellProto->SpellFamilyName == SPELLFAMILY_PRIEST && spellProto->Mechanic == MECHANIC_SHIELD &&
                (spellProto->SpellFamilyFlags & uint64(0x0000000000000001)) &&
                // completely absorbed or dispelled
                (m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK || m_removeMode == AURA_REMOVE_BY_DISPEL))
        {
            Unit::AuraList const& vDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
            for (auto vDummyAura : vDummyAuras)
            {
                SpellEntry const* vSpell = vDummyAura->GetSpellProto();

                // Rapture (main spell)
                if (vSpell->SpellFamilyName == SPELLFAMILY_PRIEST && vSpell->SpellIconID == 2894 && vSpell->Effect[EFFECT_INDEX_1])
                {
                    switch (vDummyAura->GetEffIndex())
                    {
                        case EFFECT_INDEX_0:
                        {
                            // energize caster
                            int32 manapct1000 = 5 * (vDummyAura->GetModifier()->m_amount + sSpellMgr.GetSpellRank(vSpell->Id));
                            int32 basepoints0 = caster->GetMaxPower(POWER_MANA) * manapct1000 / 1000;
                            caster->CastCustomSpell(caster, 47755, &basepoints0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                            break;
                        }
                        case EFFECT_INDEX_1:
                        {
                            // energize target
                            if (!roll_chance_i(vDummyAura->GetModifier()->m_amount) || caster->HasAura(63853))
                                break;

                            switch (target->GetPowerType())
                            {
                                case POWER_RUNIC_POWER:
                                    target->CastSpell(target, 63652, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                                    break;
                                case POWER_RAGE:
                                    target->CastSpell(target, 63653, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                                    break;
                                case POWER_MANA:
                                {
                                    int32 basepoints0 = target->GetMaxPower(POWER_MANA) * 2 / 100;
                                    target->CastCustomSpell(target, 63654, &basepoints0, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
                                    break;
                                }
                                case POWER_ENERGY:
                                    target->CastSpell(target, 63655, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                                    break;
                                default:
                                    break;
                            }

                            // cooldown aura
                            caster->CastSpell(caster, 63853, TRIGGERED_OLD_TRIGGERED);
                            break;
                        }
                        default:
                            sLog.outError("Changes in R-dummy spell???: effect 3");
                            break;
                    }
                }
            }
        }
    }
}

void Aura::PeriodicTick()
{
    Unit* target = GetTarget();
    // passive periodic trigger spells should not be updated when dead, only death persistent should
    if (!target->IsAlive() && GetHolder()->IsPassive())
        return;

    SpellEntry const* spellProto = GetSpellProto();

    switch (m_modifier.m_auraname)
    {
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Unit* caster = GetCaster();

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            uint32 pdamage;
            if (m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
                pdamage = amount;
            else
                pdamage = uint32(target->GetMaxHealth() * amount / 100);

            // some auras remove at specific health level or more or have damage interactions
            bool breakSwitch = false;
            bool overrideImmune = false;
            switch (GetId())
            {
                case 43093: case 31956: case 38801:
                case 35321: case 38363: case 39215:
                case 48920:
                {
                    if (target->GetHealth() == target->GetMaxHealth())
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        breakSwitch = true;
                    }
                    break;
                }
                case 38772:
                {
                    uint32 percent =
                        GetEffIndex() < EFFECT_INDEX_2 && spellProto->Effect[GetEffIndex()] == SPELL_EFFECT_DUMMY ?
                        caster->CalculateSpellEffectValue(target, spellProto, SpellEffectIndex(GetEffIndex() + 1)) :
                        100;
                    if (target->GetHealth() * 100 >= target->GetMaxHealth() * percent)
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        breakSwitch = true;
                    }
                    break;
                }
                case 29964: // Dragons Breath
                {
                    target->CastSpell(nullptr, 29965, TRIGGERED_OLD_TRIGGERED);
                    break;
                }
                case 40932: // Agonizing Flames - Illidan
                {
                    if (GetAuraTicks() % 3 == 0) // increased damage after every 3rd tick
                        m_modifier.m_amount += m_modifier.m_baseAmount;
                    break;
                }
                case 41337: // Aura of Anger
                {
                    m_modifier.m_amount += m_modifier.m_baseAmount;
                    if (Aura* aura = GetHolder()->m_auras[EFFECT_INDEX_1])
                    {
                        aura->ApplyModifier(false, true);
                        aura->m_modifier.m_amount += aura->m_modifier.m_baseAmount;
                        aura->ApplyModifier(true, true);
                    }
                    // during normal immunities - ticks, only doesnt tick during spite
                    overrideImmune = (target->HasAura(41376) || target->HasAura(41377));
                    break;
                }
                default:
                    break;
            }
            if (breakSwitch)
                break;

            OnPeriodicCalculateAmount(pdamage);

            if (spellProto->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA) // safe case - caster always will exist
            {
                if (!caster)
                {
                    sLog.outCustomLog("Spell ID: %u Caster guid %lu", spellProto->Id, GetCasterGuid().GetRawValue());
                    MANGOS_ASSERT(caster);
                }
                if (Unit::SpellHitResult(caster, target, spellProto, (1 << GetEffIndex()), false) != SPELL_MISS_NONE)
                    break;
            }

            // Check for immune (not use charges)
            // Aura of anger - video evidence confirms this, but attribute is legit because aura is still applied during 
            if (!spellProto->HasAttribute(SPELL_ATTR_NO_IMMUNITIES) || overrideImmune) // confirmed Impaling spine goes through immunity
            {
                if (target->IsImmuneToDamage(GetSpellSchoolMask(spellProto)))
                {
                    Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            uint32 absorb = 0;
            int32 resist = 0;

            bool isNotBleed = GetEffectMechanic(spellProto, m_effIndex) != MECHANIC_BLEED;

            // SpellDamageBonus for magic spells
            if ((spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE && isNotBleed) || spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                pdamage = target->SpellDamageBonusTaken(caster, SpellSchoolMask(spellProto->SchoolMask), spellProto, pdamage, DOT, GetStackAmount());
            // MeleeDamagebonus for weapon based spells
            else
            {
                WeaponAttackType attackType = GetWeaponAttackType(spellProto);
                pdamage = target->MeleeDamageBonusTaken(caster, pdamage, attackType, SpellSchoolMask(spellProto->SchoolMask), spellProto, DOT, GetStackAmount());
            }

            // This method can modify pdamage
            bool isCrit = false;
            if (caster)
            {
                isCrit = IsCritFromAbilityAura(caster, pdamage);
                // only from players
                // FIXME: need use SpellDamageBonus instead?
                if (caster->GetTypeId() == TYPEID_PLAYER)
                    pdamage -= target->GetResilienceRatingDamageReduction(pdamage, SpellDmgClass(spellProto->DmgClass), true);
            }

            target->CalculateDamageAbsorbAndResist(caster, GetSpellSchoolMask(spellProto), DOT, pdamage, &absorb, &resist, IsReflectableSpell(spellProto), IsResistableSpell(spellProto) && !spellProto->HasAttribute(SPELL_ATTR_EX5_NO_PARTIAL_RESISTS));

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s attacked %s for %u dmg inflicted by %u abs is %u",
                              GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId(), absorb);

            Unit::DealDamageMods(caster, target, pdamage, &absorb, DOT, spellProto);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DEAL_HARMFUL_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_TAKE_HARMFUL_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx = isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT;

            const uint32 bonus = (resist < 0 ? uint32(std::abs(resist)) : 0);
            pdamage += bonus;
            const uint32 malus = (resist > 0 ? (absorb + uint32(resist)) : absorb);
            pdamage = (pdamage <= malus ? 0 : (pdamage - malus));

            uint32 overkill = pdamage > target->GetHealth() ? pdamage - target->GetHealth() : 0;
            SpellPeriodicAuraLogInfo pInfo(this, pdamage, overkill, absorb, resist, 0.0f, isCrit);
            target->SendPeriodicAuraLog(&pInfo);

            if (pdamage)
                procVictim |= PROC_FLAG_TAKE_ANY_DAMAGE;

            CleanDamage cleanDamage(pdamage, BASE_ATTACK, MELEE_HIT_NORMAL, pdamage || absorb);
            // send critical in hit info for threat calculation
            if (isCrit)
                cleanDamage.hitOutCome = MELEE_HIT_CRIT;
            Unit::DealDamage(caster, target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, true);

            Unit::ProcDamageAndSpell(ProcSystemArguments(caster, target, procAttacker, procVictim, PROC_EX_NORMAL_HIT, pdamage, BASE_ATTACK, spellProto));

            // Drain Soul (chance soul shard)
            if (caster && caster->GetTypeId() == TYPEID_PLAYER && spellProto->SpellFamilyName == SPELLFAMILY_WARLOCK && spellProto->SpellFamilyFlags & uint64(0x0000000000004000))
            {
                // Only from non-grey units
                if (roll_chance_i(10) &&                                                                        // 1-2 from drain with final and without glyph, 0-1 from damage
                    ((Player*)caster)->isHonorOrXPTarget(target) &&                                             // Gain XP or Honor requirement
                    (target->GetTypeId() == TYPEID_UNIT && !((Creature*)target)->IsTappedBy((Player*)caster)))  // Tapped by player requirement
                {
                    caster->CastSpell(nullptr, 43836, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                }
            }

            if (GetId() == 13493)
                m_scriptValue += pdamage;
            break;
        }
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Unit* pCaster = GetCaster();
            if (!pCaster)
                break;

            if (!pCaster->IsAlive())
                break;

            uint32 pdamage = (m_modifier.m_amount > 0 ? uint32(m_modifier.m_amount) : 0);
            OnPeriodicCalculateAmount(pdamage);

            if (spellProto->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                    Unit::SpellHitResult(pCaster, target, spellProto, (1 << GetEffIndex()), false) != SPELL_MISS_NONE)
                break;

            // Check for immune
            if (!spellProto->HasAttribute(SPELL_ATTR_NO_IMMUNITIES))
            {
                if (target->IsImmuneToDamage(GetSpellSchoolMask(spellProto)))
                {
                    Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            uint32 absorb = 0;
            int32 resist = 0;

            pdamage = target->SpellDamageBonusTaken(pCaster, SpellSchoolMask(spellProto->SchoolMask), spellProto, pdamage, DOT, GetStackAmount());

            bool isCrit = IsCritFromAbilityAura(pCaster, pdamage);

            // only from players
            // FIXME: need use SpellDamageBonus instead?
            if (GetCasterGuid().IsPlayer())
                pdamage -= target->GetResilienceRatingDamageReduction(pdamage, SpellDmgClass(spellProto->DmgClass), true);

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s health leech of %s for %u dmg inflicted by %u abs is %u",
                              GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId(), absorb);

            Unit::DealDamageMods(pCaster, target, pdamage, &absorb, DOT, spellProto);

            Unit::SendSpellNonMeleeDamageLog(pCaster, target, GetId(), pdamage, GetSpellSchoolMask(spellProto), absorb, resist, true, 0, isCrit);

            float multiplier = spellProto->EffectMultipleValue[GetEffIndex()] > 0 ? spellProto->EffectMultipleValue[GetEffIndex()] : 1;

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DEAL_HARMFUL_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_TAKE_HARMFUL_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx = isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT;

            const uint32 bonus = (resist < 0 ? uint32(std::abs(resist)) : 0);
            pdamage += bonus;
            const uint32 malus = (resist > 0 ? (absorb + uint32(resist)) : absorb);
            pdamage = (pdamage <= malus ? 0 : (pdamage - malus));

            pdamage = std::min(pdamage, target->GetHealth());

            if (pdamage)
                procVictim |= PROC_FLAG_TAKE_ANY_DAMAGE;

            CleanDamage cleanDamage(pdamage, BASE_ATTACK, MELEE_HIT_NORMAL, pdamage || absorb);
            // send critical in hit info for threat calculation
            if (isCrit)
                cleanDamage.hitOutCome = MELEE_HIT_CRIT;
            int32 new_damage = Unit::DealDamage(pCaster, target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, false);
            Unit::ProcDamageAndSpell(ProcSystemArguments(pCaster, target, procAttacker, procVictim, PROC_EX_NORMAL_HIT, pdamage, BASE_ATTACK, spellProto));

            if (!target->IsAlive() && pCaster->IsNonMeleeSpellCasted(false))
                for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                    if (Spell* spell = pCaster->GetCurrentSpell(CurrentSpellTypes(i)))
                        if (spell->m_spellInfo->Id == GetId())
                            spell->cancel();

            if (Player* modOwner = pCaster->GetSpellModOwner())
            {
                modOwner->ApplySpellMod(GetId(), SPELLMOD_ALL_EFFECTS, new_damage);
                modOwner->ApplySpellMod(GetId(), SPELLMOD_MULTIPLE_VALUE, multiplier);
            }

            int32 heal = pCaster->SpellHealingBonusTaken(pCaster, spellProto, int32(new_damage * multiplier), DOT, GetStackAmount());

            uint32 absorbHeal = 0;
            pCaster->CalculateHealAbsorb(heal, &absorbHeal);

            int32 gain = pCaster->DealHeal(pCaster, heal - absorbHeal, spellProto, false, absorbHeal);
            // Health Leech effects do not generate healing aggro
            if (m_modifier.m_auraname == SPELL_AURA_PERIODIC_LEECH)
                break;
            pCaster->getHostileRefManager().threatAssist(pCaster, gain * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto, false, true);
            break;
        }
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                break;

            // don't heal target if max health or if not alive, mostly death persistent effects from items
            if (!target->IsAlive() || (target->GetHealth() == target->GetMaxHealth()))
                break;

            // heal for caster damage (must be alive)
            if (target != pCaster && spellProto->SpellVisual[0] == 163 && !pCaster->IsAlive())
                break;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            uint32 pdamage;

            if (m_modifier.m_auraname == SPELL_AURA_OBS_MOD_HEALTH)
                pdamage = uint32(target->GetMaxHealth() * amount / 100);
            else
            {
                pdamage = amount;
                // Wild Growth (1/7 - 6 + 2*ramainTicks) %
                if (spellProto->SpellFamilyName == SPELLFAMILY_DRUID && spellProto->SpellIconID == 2864)
                {
                    int32 ticks = GetAuraMaxTicks();
                    int32 remainingTicks = ticks - GetAuraTicks();
                    int32 addition = int32(amount) * ticks * (-6 + 2 * remainingTicks) / 100;

                    if (GetAuraTicks() != 1)
                        // Item - Druid T10 Restoration 2P Bonus
                        if (Aura* aura = pCaster->GetAura(70658, EFFECT_INDEX_0))
                            addition += abs(int32((addition * aura->GetModifier()->m_amount) / ((ticks - 1) * 100)));

                    pdamage = int32(pdamage) + addition;
                }
            }

            OnPeriodicCalculateAmount(pdamage);

            // first aid confirmed to work through immune when selfcast
            if (pCaster != target && target->IsImmuneToSchool(spellProto, (1 << GetEffIndex()))) // TODO: Check if we need to check for avoiding immune
            {
                Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                break;
            }

            pdamage = target->SpellHealingBonusTaken(pCaster, spellProto, pdamage, DOT, GetStackAmount());

            // This method can modify pdamage
            bool isCrit = IsCritFromAbilityAura(pCaster, pdamage);

            uint32 absorbHeal = 0;
            pCaster->CalculateHealAbsorb(pdamage, &absorbHeal);
            pdamage -= absorbHeal;

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s heal of %s for %u health  (absorbed %u) inflicted by %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, absorbHeal, GetId());

            int32 gain = target->ModifyHealth(pdamage);
            SpellPeriodicAuraLogInfo pInfo(this, pdamage, (pdamage - uint32(gain)), absorbHeal, 0, 0.0f, isCrit);
            target->SendPeriodicAuraLog(&pInfo);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DEAL_HARMFUL_PERIODIC;
            uint32 procVictim = PROC_FLAG_TAKE_HARMFUL_PERIODIC;
            uint32 procEx = PROC_EX_INTERNAL_HOT | (isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT);

            // add HoTs to amount healed in bgs
            if (pCaster->GetTypeId() == TYPEID_PLAYER)
                if (BattleGround* bg = ((Player*)pCaster)->GetBattleGround())
                    bg->UpdatePlayerScore(((Player*)pCaster), SCORE_HEALING_DONE, gain);

            if (pCaster->IsInCombat() && !pCaster->IsCrowdControlled())
                target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto, false, true);

            Unit::ProcDamageAndSpell(ProcSystemArguments(pCaster, target, procAttacker, procVictim, procEx, gain, BASE_ATTACK, spellProto, nullptr, gain, true));
            break;
        }
        case SPELL_AURA_PERIODIC_MANA_LEECH:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                break;

            Powers power = Powers(m_modifier.m_miscvalue);

            // power type might have changed between aura applying and tick (druid's shapeshift)
            if (target->GetPowerType() != power)
                break;

            Unit* pCaster = GetCaster();
            if (!pCaster)
                break;

            if (!pCaster->IsAlive())
                break;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            // Special case: draining x% of mana (up to a maximum of 2*x% of the caster's maximum mana)
            // It's mana percent cost spells, m_modifier.m_amount is percent drain from target
            if (spellProto->ManaCostPercentage)
            {
                // max value
                uint32 maxmana = pCaster->GetMaxPower(power)  * pdamage * 2 / 100;
                pdamage = target->GetMaxPower(power) * pdamage / 100;
                if (pdamage > maxmana)
                    pdamage = maxmana;
            }

            OnPeriodicCalculateAmount(pdamage);

            if (GetSpellProto()->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                    Unit::SpellHitResult(pCaster, target, spellProto, (1 << GetEffIndex()), false) != SPELL_MISS_NONE)
                break;

            // Check for immune (not use charges)
            if (!spellProto->HasAttribute(SPELL_ATTR_NO_IMMUNITIES)) // confirmed Impaling spine goes through immunity
            {
                if (target->IsImmuneToDamage(GetSpellSchoolMask(spellProto)))
                {
                    Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s power leech of %s for %u dmg inflicted by %u",
                              GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId());

            switch (GetId())
            {
                case 32960:                                 // Mark of Kazzak
                {
                    if (target->GetTypeId() == TYPEID_PLAYER && target->GetPowerType() == POWER_MANA)
                    {
                        // Drain 5% of target's mana
                        pdamage = target->GetMaxPower(POWER_MANA) * 5 / 100;
                    }
                    break;
                }
            }

            int32 drain_amount = target->GetPower(power) > pdamage ? pdamage : target->GetPower(power);

            drain_amount -= target->GetResilienceRatingDamageReduction(uint32(drain_amount), SpellDmgClass(spellProto->DmgClass), false, power);

            target->ModifyPower(power, -drain_amount);

            float gain_multiplier = 0.0f;

            if (pCaster->GetMaxPower(power) > 0)
            {
                gain_multiplier = spellProto->EffectMultipleValue[GetEffIndex()];

                if (Player* modOwner = pCaster->GetSpellModOwner())
                    modOwner->ApplySpellMod(GetId(), SPELLMOD_MULTIPLE_VALUE, gain_multiplier);
            }

            SpellPeriodicAuraLogInfo pInfo(this, drain_amount, 0, 0, 0, gain_multiplier);
            target->SendPeriodicAuraLog(&pInfo);

            int32 gainCalculated = int32(drain_amount * gain_multiplier);
            int32 gain = gainCalculated;
            if (gainCalculated)
            {
                gain = pCaster->ModifyPower(power, gainCalculated);

                if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_WARLOCK, 0x0000000000000010)) // Drain Mana
                    if (Aura* petPart = GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_1))
                        if (int32 pet_gain = gainCalculated * petPart->GetModifier()->m_amount / 100)
                            pCaster->CastCustomSpell(pCaster, 32554, &pet_gain, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);

                target->AddThreat(pCaster, float(gain) * 0.5f, pInfo.critical, GetSpellSchoolMask(spellProto), spellProto);
            }

            // Some special cases
            switch (GetId())
            {
                case 21056:                                 // Mark of Kazzak
                case 32960:                                 // Mark of Kazzak
                {
                    uint32 triggerSpell = 0;
                    switch (GetId())
                    {
                        case 21056: triggerSpell = 21058; break;
                        case 32960: triggerSpell = 32961; break;
                    }
                    if (target->GetTypeId() == TYPEID_PLAYER && target->GetPower(power) == 0)
                    {
                        target->CastSpell(target, triggerSpell, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                        target->RemoveAurasDueToSpell(GetId());
                    }
                    break;
                }
            }
            break;
        }
        case SPELL_AURA_PERIODIC_ENERGIZE:
        {
            // don't energize target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Unit* pCaster = GetCaster();

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;
            OnPeriodicCalculateAmount(pdamage);

            if (pCaster)
            {
                if (target->IsImmuneToSchool(spellProto, (1 << GetEffIndex())))
                {
                    // likely need to ignore all of them but the only case is periodic energize
                    if (!spellProto->HasAttribute(SPELL_ATTR_DO_NOT_LOG_IMMUNE_MISSES))
                        Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s energize %s for %u dmg inflicted by %u",
                              GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId());

            if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                break;

            Powers power = Powers(m_modifier.m_miscvalue);

            if (target->GetMaxPower(power) == 0)
                break;

            SpellPeriodicAuraLogInfo pInfo(this, pdamage, 0, 0, 0, 0.0f);
            target->SendPeriodicAuraLog(&pInfo);

            int32 gain = target->ModifyPower(power, pdamage);

            if (pCaster)
                target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto, false, true);

            if (GetId() == 25685) // Moam - Energize
                if (target->GetPower(POWER_MANA) > 24000)
                    target->RemoveAurasDueToSpell(GetId());
            break;
        }
        case SPELL_AURA_OBS_MOD_MANA:
        {
            // don't energize target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Unit* pCaster = GetCaster();

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            uint32 pdamage = uint32(target->GetMaxPower(POWER_MANA) * amount / 100);
            OnPeriodicCalculateAmount(pdamage);

            if (pCaster)
            {
                if (target->IsImmuneToSchool(spellProto, (1 << GetEffIndex())))
                {
                    Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s energize %s for %u mana inflicted by %u",
                              GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId());

            if (target->GetMaxPower(POWER_MANA) == 0)
                break;

            SpellPeriodicAuraLogInfo pInfo(this, pdamage, 0, 0, 0, 0.0f);
            target->SendPeriodicAuraLog(&pInfo);

            int32 gain = target->ModifyPower(POWER_MANA, pdamage);

            if (pCaster)
                target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto, false, true);
            break;
        }
        case SPELL_AURA_POWER_BURN_MANA:
        {
            // don't mana burn target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Unit* pCaster = GetCaster();
            if (!pCaster)
                break;

            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;
            OnPeriodicCalculateAmount(pdamage);

            // Check for immune (not use charges)
            if (!spellProto->HasAttribute(SPELL_ATTR_NO_IMMUNITIES)) // confirmed Impaling spine goes through immunity
            {
                if (target->IsImmuneToDamage(GetSpellSchoolMask(spellProto)))
                {
                    Unit::SendSpellOrDamageImmune(GetCasterGuid(), target, spellProto->Id);
                    break;
                }
            }

            Powers powerType = Powers(m_modifier.m_miscvalue);

            if (!target->IsAlive() || target->GetPowerType() != powerType)
                break;

            pdamage -= target->GetResilienceRatingDamageReduction(pdamage, SpellDmgClass(spellProto->DmgClass), false, powerType);

            uint32 gain = uint32(-target->ModifyPower(powerType, -int32(pdamage)));

            gain = uint32(gain * spellProto->EffectMultipleValue[GetEffIndex()]);

            // maybe has to be sent different to client, but not by SMSG_PERIODICAURALOG
            SpellNonMeleeDamage spellDamageInfo(pCaster, target, spellProto->Id, SpellSchoolMask(spellProto->SchoolMask));
            spellDamageInfo.periodicLog = true;

            pCaster->CalculateSpellDamage(&spellDamageInfo, gain, spellProto);

            spellDamageInfo.target->CalculateAbsorbResistBlock(pCaster, &spellDamageInfo, spellProto);

            Unit::DealDamageMods(pCaster, spellDamageInfo.target, spellDamageInfo.damage, &spellDamageInfo.absorb, SPELL_DIRECT_DAMAGE, spellProto);

            pCaster->SendSpellNonMeleeDamageLog(&spellDamageInfo);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DEAL_HARMFUL_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_TAKE_HARMFUL_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx       = createProcExtendMask(&spellDamageInfo, SPELL_MISS_NONE);
            if (spellDamageInfo.damage)
                procVictim |= PROC_FLAG_TAKE_ANY_DAMAGE;

            Unit::DealSpellDamage(pCaster, &spellDamageInfo, true, false);

            Unit::ProcDamageAndSpell(ProcSystemArguments(pCaster, spellDamageInfo.target, procAttacker, procVictim, procEx, spellDamageInfo.damage, BASE_ATTACK, spellProto));
            break;
        }
        case SPELL_AURA_MOD_REGEN:
        {
            // don't heal target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            int32 gain = target->ModifyHealth(m_modifier.m_amount);
            if (Unit* caster = GetCaster())
                target->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f  * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto, false, true);
            break;
        }
        case SPELL_AURA_MOD_POWER_REGEN:
        {
            // don't energize target if not alive, possible death persistent effects
            if (!target->IsAlive())
                break;

            Powers powerType = target->GetPowerType();
            if (int32(powerType) != m_modifier.m_miscvalue)
                break;

            if (spellProto->AuraInterruptFlags & AURA_INTERRUPT_FLAG_STANDING_CANCELS)
            {
                // eating anim
                target->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
            }
            else if (GetId() == 20577)
            {
                // cannibalize anim
                target->HandleEmoteCommand(EMOTE_STATE_CANNIBALIZE);
            }

            // Anger Management
            // amount = 1+ 16 = 17 = 3,4*5 = 10,2*5/3
            // so 17 is rounded amount for 5 sec tick grow ~ 1 range grow in 3 sec
            if (powerType == POWER_RAGE && target->IsInCombat())
                target->ModifyPower(powerType, m_modifier.m_amount * 3 / 5);
            // Butchery
            else if (powerType == POWER_RUNIC_POWER && target->IsInCombat())
                target->ModifyPower(powerType, m_modifier.m_amount);
            break;
        }
        // Here tick dummy auras
        case SPELL_AURA_DUMMY:                              // some spells have dummy aura
        case SPELL_AURA_PERIODIC_DUMMY:
        {
            PeriodicDummyTick();
            break;
        }
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        {
            TriggerSpell();
            break;
        }
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
        {
            TriggerSpellWithValue();
            break;
        }
        default:
            break;
    }
    OnPeriodicTickEnd();
}

void Aura::PeriodicDummyTick()
{
    SpellEntry const* spell = GetSpellProto();
    Unit* target = GetTarget();
    switch (spell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (spell->Id)
            {
                case 6946:                                  // Curse of the Bleakheart
                case 41170:
                {
                    if (roll_chance_i(5))
                    {
                        int32 damageValue = target->CalculateSpellEffectValue(target, spell, EFFECT_INDEX_1);
                        target->CastCustomSpell(nullptr, spell->Id == 6946 ? 6945 : 41356, nullptr, &damageValue, nullptr, TRIGGERED_OLD_TRIGGERED);
                    }
                    return;
                }
                case 7057:                                  // Haunting Spirits
                    if (roll_chance_i(33))
                        target->CastSpell(target, m_modifier.m_amount, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
//              // Panda
//              case 19230: break;
                case 21094:                                 // Separation Anxiety (Majordomo Executus)
                case 23487:                                 // Separation Anxiety (Garr)
                    if (Unit* caster = GetCaster())
                    {
                        float m_radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spell->EffectRadiusIndex[m_effIndex]));
                        if (caster->IsAlive() && !caster->IsWithinDistInMap(target, m_radius))
                            target->CastSpell(target, (spell->Id == 21094 ? 21095 : 23492), TRIGGERED_OLD_TRIGGERED, nullptr);      // Spell 21095: Separation Anxiety for Majordomo Executus' adds, 23492: Separation Anxiety for Garr's adds
                    }
                    return;
                case 27769:                                 // Whisper Gulch: Yogg-Saron Whisper
                {
                    if (roll_chance_i(20))
                        target->CastSpell(nullptr, 29072, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
//              // Gossip NPC Periodic - Talk
                case 32441:                                 // Brittle Bones
                    if (roll_chance_i(33))
                        target->CastSpell(target, 32437, true, nullptr, this);  // Rattled
                    return;
//              case 33208: break;
//              // Gossip NPC Periodic - Despawn
//              case 33209: break;
//              // Steal Weapon
//              case 36207: break;
                case 37025: // Coilfang Water
                    if (target->IsInSwimmableWater())
                        target->CastSpell(target, 37026, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                case 39993: // Simon Game START timer, (DND)
                    target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
                    return;
//              case 40084: break;
//              // Knockdown Fel Cannon: break; The Aggro Burst
//              case 40119: break;
//              // Old Mount Spell
//              case 40154: break;
//              // Magnetic Pull
//              case 40581: break;
//              // Ethereal Ring: break; The Bolt Burst
                case 40801:
                {
                    target->CastSpell(GetCaster(), 40758, TRIGGERED_OLD_TRIGGERED);
                    break;
                }
//              // Crystal Prison
//              case 40846: break;
//              // Copy Weapon
//              case 41054: break;
//              // Dementia
//              case 41404: break;
//              // Ethereal Ring Visual, Lightning Aura
//              case 41477: break;
//              // Ethereal Ring Visual, Lightning Aura (Fork)
//              case 41525: break;
//              // Ethereal Ring Visual, Lightning Jumper Aura
//              case 41567: break;
//              // No Man's Land
//              case 41955: break;
//              // Headless Horseman - Fire
//              case 42074: break;
//              // Headless Horseman - Visual - Large Fire
//              case 42075: break;
//              // Headless Horseman - Start Fire, Periodic Aura
//              case 42140: break;
//              // Ram Speed Boost
//              case 42152: break;
//              // Headless Horseman - Fires Out Victory Aura
//              case 42235: break;
//              // Pumpkin Life Cycle
//              case 42280: break;
//              // Brewfest Request Chick Chuck Mug Aura
//              case 42537: break;
//              // Squashling
//              case 42596: break;
//              // Headless Horseman Climax, Head: Periodic
//              case 42603: break;
                case 42621:                                 // Fire Bomb
                {
                    // Cast the summon spells (42622 to 42627) with increasing chance
                    uint32 rand = urand(0, 99);
                    for (uint32 i = 1; i <= 6; ++i)
                    {
                        if (rand < i * (i + 1) / 2 * 5)
                        {
                            target->CastSpell(target, spell->Id + i, TRIGGERED_OLD_TRIGGERED);
                            break;
                        }
                    }
                    if (GetAuraTicks() == 50)
                        target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
                    break;
                }
//              // Headless Horseman - Conflagrate, Periodic Aura
//              case 42637: break;
//              // Headless Horseman - Create Pumpkin Treats Aura
//              case 42774: break;
//              // Headless Horseman Climax - Summoning Rhyme Aura
//              case 42879: break;
//              // Giddyup!
//              case 42924: break;
//              // Ram - Trot
//              case 42992: break;
//              // Ram - Canter
//              case 42993: break;
//              // Ram - Gallop
//              case 42994: break;
//              // Ram Level - Neutral
//              case 43310: break;
//              // Headless Horseman - Maniacal Laugh, Maniacal, Delayed 17
//              case 43884: break;
//              // Wretched!
//              case 43963: break;
//              // Headless Horseman - Maniacal Laugh, Maniacal, other, Delayed 17
//              case 44000: break;
                case 44328:                             // Energy Feedback
                {
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(nullptr, 44339, TRIGGERED_NONE);
                    break;
                }
//              // Romantic Picnic
//              case 45102: break;
//              // Romantic Picnic
//              case 45123: break;
//              // Looking for Love
//              case 45124: break;
//              // Kite - Lightning Strike Kite Aura
//              case 45197: break;
//              // Rocket Chicken
//              case 45202: break;
//              // Copy Offhand Weapon
//              case 45205: break;
//              // Upper Deck - Kite - Lightning Periodic Aura
//              case 45207: break;
//              // Kite -Sky  Lightning Strike Kite Aura
//              case 45251: break;
//              // Ribbon Pole Dancer Check Aura
//              case 45390: break;
//              // Holiday - Midsummer, Ribbon Pole Periodic Visual
//              case 45406: break;
//              // Parachute
//              case 45472: break;
//              // Alliance Flag, Extra Damage Debuff
//              case 45898: break;
//              // Horde Flag, Extra Damage Debuff
//              case 45899: break;
//              // Ahune - Summoning Rhyme Aura
//              case 45926: break;
//              // Ahune - Slippery Floor
//              case 45945: break;
//              // Ahune's Shield
//              case 45954: break;
//              // Nether Vapor Lightning
//              case 45960: break;
//              // Darkness
//              case 45996: break;
//              // Transform Visual Missile Periodic
//              case 46205: break;
//              // Find Opening Beam End
//              case 46333: break;
                case 46371:                                 // Ice Spear Control Aura
                    target->CastSpell(target, 46372, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
//              // Hailstone Chill
//              case 46458: break;
//              // Hailstone Chill, Internal
//              case 46465: break;
//              // Chill, Internal Shifter
//              case 46549: break;
//              // Summon Ice Spear Knockback Delayer
//              case 46878: break;
//              // Burninate Effect
//              case 47214: break;
//              // Fizzcrank Practice Parachute
//              case 47228: break;
//              // Send Mug Control Aura
//              case 47369: break;
//              // Direbrew's Disarm (precast)
//              case 47407: break;
//              // Mole Machine Port Schedule
//              case 47489: break;
//              case 47941: break; // Crystal Spike
//              case 48200: break; // Healer Aura
//              case 49313: break; // Proximity Mine Area Aura
//              // Mole Machine Portal Schedule
//              case 49466: break;
                case 49555:                                 // Corpse Explode (Drak'tharon Keep - Trollgore)
                case 59807:                                 // Corpse Explode (heroic)
                {
                    if (GetAuraTicks() == 3 && target->GetTypeId() == TYPEID_UNIT)
                        ((Creature*)target)->ForcedDespawn();
                    if (GetAuraTicks() != 2)
                        return;

                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(target, spell->Id == 49555 ? 49618 : 59809, TRIGGERED_OLD_TRIGGERED);

                    return;
                }
//              case 49592: break; // Temporal Rift
//              case 49957: break; // Cutting Laser
//              case 50085: break; // Slow Fall
//              // Listening to Music
//              case 50493: break; // TODO: Implement
//              // Love Rocket Barrage
//              case 50530: break;
                case 52441:                                 // Cool Down
                    target->CastSpell(target, 52443, TRIGGERED_OLD_TRIGGERED);
                    return;
                case 53520:                                 // Carrion Beetles
                    target->CastSpell(target, 53521, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    target->CastSpell(target, 53521, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                case 55592:                                 // Clean
                    switch (urand(0, 2))
                    {
                        case 0: target->CastSpell(target, 55731, TRIGGERED_OLD_TRIGGERED); break;
                        case 1: target->CastSpell(target, 55738, TRIGGERED_OLD_TRIGGERED); break;
                        case 2: target->CastSpell(target, 55739, TRIGGERED_OLD_TRIGGERED); break;
                    }
                    return;
                case 61968:                                 // Flash Freeze
                {
                    if (GetAuraTicks() == 1 && !target->HasAura(62464))
                        target->CastSpell(target, 61970, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                case 62018:                                 // Collapse
                {
                    // lose 1% of health every second
                    Unit::DealDamage(target, target, target->GetMaxHealth() * .01, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, nullptr, false);
                    return;
                }
                case 62019:                                 // Rune of Summoning
                {
                    target->CastSpell(target, 62020, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                case 62038:                                 // Biting Cold
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // if player is moving remove one aura stack
                    if (target->IsMoving())
                        target->RemoveAuraHolderFromStack(62039);
                    // otherwise add one aura stack each 3 seconds
                    else if (GetAuraTicks() % 3 && !target->HasAura(62821))
                        target->CastSpell(target, 62039, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                case 62039:                                 // Biting Cold
                {
                    target->CastSpell(target, 62188, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                case 62566:                                 // Healthy Spore Summon Periodic
                {
                    target->CastSpell(target, 62582, TRIGGERED_OLD_TRIGGERED);
                    target->CastSpell(target, 62591, TRIGGERED_OLD_TRIGGERED);
                    target->CastSpell(target, 62592, TRIGGERED_OLD_TRIGGERED);
                    target->CastSpell(target, 62593, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                case 62717:                                 // Slag Pot
                {
                    target->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? 65722 : 65723, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                    // cast Slag Imbued if the target survives up to the last tick
                    if (GetAuraTicks() == 10)
                        target->CastSpell(target, 63536, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                case 63050:                                 // Sanity
                {
                    if (GetHolder()->GetStackAmount() <= 25 && !target->HasAura(63752))
                        target->CastSpell(target, 63752, TRIGGERED_OLD_TRIGGERED);
                    else if (GetHolder()->GetStackAmount() > 25 && target->HasAura(63752))
                        target->RemoveAurasDueToSpell(63752);
                    return;
                }
                case 63382:                                 // Rapid Burst
                {
                    if (GetAuraTicks() % 2)
                        target->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? 64019 : 64532, TRIGGERED_OLD_TRIGGERED);
                    else
                        target->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? 63387 : 64531, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                case 64101:                                 // Defend
                {
                    target->CastSpell(target, 62719, TRIGGERED_OLD_TRIGGERED);
                    target->CastSpell(target, 64192, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                case 64217:                                 // Overcharged
                {
                    if (GetHolder()->GetStackAmount() >= 10)
                    {
                        target->CastSpell(target, 64219, TRIGGERED_OLD_TRIGGERED);
                        target->Suicide();
                    }
                    return;
                }
                case 64412:                                 // Phase Punch
                {
                    if (SpellAuraHolder* phaseAura = target->GetSpellAuraHolder(64412))
                    {
                        uint32 uiAuraId = 0;
                        switch (phaseAura->GetStackAmount())
                        {
                            case 1: uiAuraId = 64435; break;
                            case 2: uiAuraId = 64434; break;
                            case 3: uiAuraId = 64428; break;
                            case 4: uiAuraId = 64421; break;
                            case 5: uiAuraId = 64417; break;
                        }

                        if (uiAuraId && !target->HasAura(uiAuraId))
                        {
                            target->CastSpell(target, uiAuraId, TRIGGERED_OLD_TRIGGERED, nullptr, this);

                            // remove original aura if phased
                            if (uiAuraId == 64417)
                            {
                                target->RemoveAurasDueToSpell(64412);
                                target->CastSpell(target, 62169, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                            }
                        }
                    }
                    return;
                }
                case 65272:                                 // Shatter Chest
                {
                    target->CastSpell(target, 62501, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                case 66798:                                 // Death's Respite
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    caster->CastSpell(target, 66797, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    target->RemoveAurasDueToSpell(GetId());
                    return;
                }
                case 70069:                                 // Ooze Flood Periodic Trigger
                {
                    target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(m_effIndex), TRIGGERED_OLD_TRIGGERED);
                    return;
                }
// Exist more after, need add later
                default:
                    break;
            }

            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (spell->Id)
            {
                case 55342:                                 // Mirror Image
                {
                    if (GetAuraTicks() != 1)
                        return;
                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(pCaster, spell->EffectTriggerSpell[GetEffIndex()], TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (spell->Id)
            {
                // Frenzied Regeneration
                case 22842:
                {
                    // Converts up to 10 rage per second into health for $d.  Each point of rage is converted into ${$m2/10}.1% of max health.
                    // Should be manauser
                    if (target->GetPowerType() != POWER_RAGE)
                        return;
                    uint32 rage = target->GetPower(POWER_RAGE);
                    // Nothing todo
                    if (rage == 0)
                        return;
                    int32 mod = (rage < 100) ? rage : 100;
                    int32 points = target->CalculateSpellEffectValue(target, spell, EFFECT_INDEX_1);
                    int32 regen = target->GetMaxHealth() * (mod * points / 10) / 1000;
                    target->CastCustomSpell(target, 22845, &regen, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    target->SetPower(POWER_RAGE, rage - mod);
                    return;
                }
                // Force of Nature
                case 33831:
                    return;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (spell->Id)
            {
                // Harpooner's Mark
                // case 40084:
                //    return;
                // Feeding Frenzy Rank 1 & 2
                case 53511:
                case 53512:
                {
                    Unit* victim = target->GetVictim();
                    if (victim && victim->GetHealth() * 100 < victim->GetMaxHealth() * 35)
                        target->CastSpell(target, spell->Id == 53511 ? 60096 : 60097, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Astral Shift
            if (spell->Id == 52179)
            {
                // Periodic need for remove visual on stun/fear/silence lost
                if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED))
                    target->RemoveAurasDueToSpell(52179);
                return;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Death and Decay
            if (spell->SpellFamilyFlags & uint64(0x0000000000000020))
            {
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(target, 52212, &m_modifier.m_amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            // Raise Dead
//            if (spell->SpellFamilyFlags & uint64(0x0000000000001000))
//                return;
            // Chains of Ice
            if (spell->SpellFamilyFlags & uint64(0x0000400000000000))
            {
                // Get 0 effect aura
                Aura* slow = target->GetAura(GetId(), EFFECT_INDEX_0);
                if (slow)
                {
                    slow->ApplyModifier(false, true);
                    Modifier* mod = slow->GetModifier();
                    mod->m_amount += m_modifier.m_amount;
                    if (mod->m_amount > 0) mod->m_amount = 0;
                    slow->ApplyModifier(true, true);
                }
                return;
            }
            // Summon Gargoyle
//            if (spell->SpellFamilyFlags & uint64(0x0000008000000000))
//                return;
            // Death Rune Mastery
//            if (spell->SpellFamilyFlags & uint64(0x0000000000004000))
//                return;
            // Bladed Armor
            if (spell->SpellIconID == 2653)
            {
                // Increases your attack power by $s1 for every $s2 armor value you have.
                // Calculate AP bonus (from 1 efect of this spell)
                int32 apBonus = m_modifier.m_amount * target->GetArmor() / target->CalculateSpellEffectValue(target, spell, EFFECT_INDEX_1);
                target->CastCustomSpell(target, 61217, &apBonus, &apBonus, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, this);
                return;
            }
            // Reaping
//            if (spell->SpellIconID == 22)
//                return;
            // Blood of the North
//            if (spell->SpellIconID == 30412)
//                return;
            // Hysteria
            if (spell->SpellFamilyFlags & uint64(0x0000000020000000))
            {
                // damage not expected to be show in logs, not any damage spell related to damage apply
                uint32 deal = m_modifier.m_amount * target->GetMaxHealth() / 100;
                Unit::DealDamage(target, target, deal, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                return;
            }
            break;
        }
        default:
            break;
    }

    OnPeriodicDummy();

    if (Unit* caster = GetCaster())
    {
        if (target && target->GetTypeId() == TYPEID_UNIT)
            sScriptDevAIMgr.OnEffectDummy(caster, GetId(), GetEffIndex(), (Creature*)target, ObjectGuid());
    }
}

void Aura::HandlePreventFleeing(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit::AuraList const& fearAuras = GetTarget()->GetAurasByType(SPELL_AURA_MOD_FEAR);
    if (!fearAuras.empty())
    {
        const Aura* first = fearAuras.front();
        if (apply)
            GetTarget()->SetFleeing(false);
        else
            GetTarget()->SetFleeing(true, first->GetCasterGuid(), first->GetId());
    }
}

void Aura::HandleManaShield(bool apply, bool Real)
{
    if (!Real)
        return;

    // prevent double apply bonuses
    if (apply && (GetTarget()->GetTypeId() != TYPEID_PLAYER || !((Player*)GetTarget())->GetSession()->PlayerLoading()))
    {
        if (Unit* caster = GetCaster())
        {
            float DoneActualBenefit = 0.0f;
            switch (GetSpellProto()->SpellFamilyName)
            {
                case SPELLFAMILY_MAGE:
                    if (GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000008000))
                    {
                        // Mana Shield
                        // +50% from +spd bonus
                        DoneActualBenefit = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(GetSpellProto())) * 0.5f;
                        break;
                    }
                    break;
                default:
                    break;
            }

            DoneActualBenefit *= caster->CalculateLevelPenalty(GetSpellProto());

            m_modifier.m_amount += (int32)DoneActualBenefit;
        }
    }
}

void Aura::HandleArenaPreparation(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION, apply);

    if (apply)
    {
        // max regen powers at start preparation
        target->SetHealth(target->GetMaxHealth());
        target->SetPower(POWER_MANA, target->GetMaxPower(POWER_MANA));
        target->SetPower(POWER_ENERGY, target->GetMaxPower(POWER_ENERGY));
    }
    else
    {
        // reset originally 0 powers at start/leave
        target->SetPower(POWER_RAGE, 0);
        target->SetPower(POWER_RUNIC_POWER, 0);
    }
}

/**
 * Such auras are applied from a caster(=player) to a vehicle.
 * This has been verified using spell #49256
 */
void Aura::HandleAuraControlVehicle(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    if (!target->IsVehicle())
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    if (apply)
    {
        //sLog.outString("Boarding %s %s on %s %s", caster->GetName(), caster->GetGuidStr().c_str(), target->GetName(), target->GetGuidStr().c_str());
        target->GetVehicleInfo()->Board(caster, GetBasePoints() - 1);

        if (Player* playerCaster = dynamic_cast<Player*>(caster))
        {
            UpdateMask updateMask;
            updateMask.SetCount(target->GetValuesCount());
            target->MarkUpdateFieldsWithFlagForUpdate(updateMask, UF_FLAG_OWNER_ONLY);
            if (updateMask.HasData())
            {
                UpdateData newData;
                target->BuildValuesUpdateBlockForPlayer(newData, updateMask, playerCaster);

                if (newData.HasData())
                {
                    WorldPacket newDataPacket = newData.BuildPacket(0);
                    playerCaster->SendDirectMessage(newDataPacket);
                }
            }
        }
    }
    else
    {
        //sLog.outString("Unboarding %s %s from %s %s", caster->GetName(), caster->GetGuidStr().c_str(), target->GetName(), target->GetGuidStr().c_str());
        target->GetVehicleInfo()->UnBoard(caster, m_removeMode == AURA_REMOVE_BY_TRACKING);
    }
}

void Aura::HandleAuraAddMechanicAbilities(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!target || target->GetTypeId() != TYPEID_PLAYER)    // only players should be affected by this aura
        return;

    uint16 i_OverrideSetId = GetMiscValue();

    const OverrideSpellDataEntry* spellSet = sOverrideSpellDataStore.LookupEntry(i_OverrideSetId);
    if (!spellSet)
        return;

    if (apply)
    {
        // spell give the player a new castbar with some spells.. this is a clientside process..
        // serverside just needs to register the new spells so that player isn't kicked as cheater
        for (unsigned int spellId : spellSet->Spells)
            if (spellId)
                static_cast<Player*>(target)->addSpell(spellId, true, false, false, false);

        target->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, i_OverrideSetId);
    }
    else
    {
        target->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, 0);
        for (unsigned int spellId : spellSet->Spells)
            if (spellId)
                static_cast<Player*>(target)->removeSpell(spellId, false, false, false);
    }
}

void Aura::HandleAuraOpenStable(bool apply, bool Real)
{
    if (!Real || GetTarget()->GetTypeId() != TYPEID_PLAYER || !GetTarget()->IsInWorld())
        return;

    Player* player = (Player*)GetTarget();

    if (apply)
        player->GetSession()->SendStablePet(player->GetObjectGuid());

    // client auto close stable dialog at !apply aura
}

void Aura::HandleAuraMirrorImage(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    if (!target->IsCreature())
        return;

    // Target of aura should always be creature (ref Spell::CheckCast)

    if (apply)
    {
        // Caster can be player or creature, the unit who pCreature will become an clone of.
        Unit* caster = GetCaster();

        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE, caster->getRace());
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS, caster->getClass());
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER, caster->getGender());
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE, caster->GetPowerType());

        target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_CLONED);

        target->SetDisplayId(caster->GetNativeDisplayId());
    }
    else
    {
        const CreatureInfo* cinfo = static_cast<Creature*>(target)->GetCreatureInfo();
        const CreatureModelInfo* minfo = sObjectMgr.GetCreatureModelInfo(target->GetNativeDisplayId());

        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE, 0);
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS, cinfo->UnitClass);
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER, minfo->gender);
        target->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE, 0);

        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_CLONED);

        target->SetDisplayId(target->GetNativeDisplayId());
    }
}

void Aura::HandleMirrorName(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();
    Unit* target = GetTarget();

    if (!target || !caster || target->GetTypeId() != TYPEID_UNIT)
        return;

    if (apply)
        target->SetName(caster->GetName());
    else
    {
        CreatureInfo const* cinfo = ((Creature*)target)->GetCreatureInfo();
        if (!cinfo)
            return;

        target->SetName(cinfo->Name);
    }
}

void Aura::HandleAuraConvertRune(bool apply, bool Real)
{
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plr = (Player*)GetTarget();

    if (plr->getClass() != CLASS_DEATH_KNIGHT)
        return;

    RuneType runeFrom = RuneType(GetSpellProto()->EffectMiscValue[m_effIndex]);
    RuneType runeTo   = RuneType(GetSpellProto()->EffectMiscValueB[m_effIndex]);

    if (apply)
    {
        for (uint32 i = 0; i < MAX_RUNES; ++i)
        {
            if (plr->GetCurrentRune(i) == runeFrom && !plr->GetRuneCooldown(i))
            {
                plr->AddRuneByAuraEffect(i, runeTo, this);
                break;
            }
        }
    }
    else
        plr->RemoveRunesByAura(this);
}

void Aura::HandlePhase(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // always non stackable
    if (apply)
    {
        Unit::AuraList const& phases = target->GetAurasByType(SPELL_AURA_PHASE);
        if (!phases.empty())
            target->RemoveAurasDueToSpell(phases.front()->GetId(), GetHolder());
    }

    target->SetPhaseMask(apply ? GetMiscValue() : uint32(PHASEMASK_NORMAL), true);
    // no-phase is also phase state so same code for apply and remove
    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        SpellAreaForAreaMapBounds saBounds = sSpellMgr.GetSpellAreaForAuraMapBounds(GetId());
        if (saBounds.first != saBounds.second)
        {
            uint32 zone, area;
            target->GetZoneAndAreaId(zone, area);

            for (SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
                itr->second->ApplyOrRemoveSpellIfCan((Player*)target, zone, area, false);
        }
    }
}

void Aura::HandleAuraDetaunt(bool /*Apply*/, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->IsAlive() || !caster->CanHaveThreatList())
        return;

    caster->TauntUpdate();
}

void Aura::HandleAuraSafeFall(bool Apply, bool Real)
{
    // implemented in WorldSession::HandleMovementOpcodes

    // only special case
    if (Apply && Real && GetId() == 32474 && GetTarget()->GetTypeId() == TYPEID_PLAYER && GetHolder()->GetState() != SPELLAURAHOLDER_STATE_DB_LOAD)
        ((Player*)GetTarget())->ActivateTaxiPathTo(506, GetId()); // on DB load flight path is initiated on its own after its safe to do so
}

bool Aura::IsCritFromAbilityAura(Unit* caster, uint32& damage) const
{
    if (!GetSpellProto()->IsFitToFamily(SPELLFAMILY_ROGUE, uint64(0x100000)) && // Rupture
            !caster->HasAffectedAura(SPELL_AURA_ABILITY_PERIODIC_CRIT, GetSpellProto()))
        return false;

    if (Unit::RollSpellCritOutcome(caster, GetTarget(), GetSpellSchoolMask(GetSpellProto()), GetSpellProto()))
    {
        damage = caster->CalculateCritAmount(GetTarget(), damage, GetSpellProto());
        return true;
    }

    return false;
}

void Aura::HandleModTargetArmorPct(bool /*apply*/, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateArmorPenetration();
}

void Aura::HandleAuraModAllCritChance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)target)->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float(m_modifier.m_amount), apply);
    ((Player*)target)->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float(m_modifier.m_amount), apply);
    ((Player*)target)->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float(m_modifier.m_amount), apply);

    // included in Player::UpdateSpellCritChance calculation
    ((Player*)target)->UpdateAllSpellCritChances();
}

void Aura::HandleAuraStopNaturalManaRegen(bool apply, bool Real)
{
    if (!Real)
        return;

    GetTarget()->ApplyModFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER, !apply && !GetTarget()->IsUnderLastManaUseEffect());
}

void Aura::HandleAuraSetVehicleId(bool apply, bool Real)
{
    if (!Real)
        return;

    GetTarget()->SetVehicleId(apply ? GetMiscValue() : 0, 0);
}

void Aura::HandlePreventResurrection(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        target->RemoveByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_RELEASE_TIMER);
    else if (!target->GetMap()->Instanceable())
        target->SetByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_RELEASE_TIMER);
}

void Aura::HandleFactionOverride(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    if (!target || !sFactionTemplateStore.LookupEntry(GetMiscValue()))
        return;

    if (apply)
        target->setFaction(GetMiscValue());
    else
        target->RestoreOriginalFaction();

    target->RemoveUnattackableTargets();
}

void Aura::HandleTriggerLinkedAura(bool apply, bool Real)
{
    if (!Real)
        return;

    uint32 linkedSpell = GetSpellProto()->EffectTriggerSpell[m_effIndex];
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(linkedSpell);
    if (!spellInfo)
    {
        sLog.outError("Aura::HandleTriggerLinkedAura for spell %u effect %u triggering unknown spell id %u", GetSpellProto()->Id, m_effIndex, linkedSpell);
        return;
    }

    Unit* target = GetTarget();
    Unit* caster = GetCaster();

    if (apply)
    {
        SpellCastArgs args;
        if (int32 points = GetAmount())
            args.SetBasePoints(&points, nullptr, nullptr);
        args.SetTarget(target);
        caster->CastSpell(args, spellInfo, TRIGGERED_OLD_TRIGGERED, nullptr, this);
    }
    else
        target->RemoveAurasByCasterSpell(linkedSpell, GetCasterGuid());
}

void Aura::HandleOverrideClassScript(bool apply, bool real)
{
    GetTarget()->RegisterOverrideScriptAura(this, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraAllowOnlyAbility(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (apply)
            target->SetFlag(PLAYER_FLAGS, PLAYER_ALLOW_ONLY_ABILITY);
        else
        {
            // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
            if (target->HasAuraType(SPELL_AURA_ALLOW_ONLY_ABILITY))
                return;
            target->RemoveFlag(PLAYER_FLAGS, PLAYER_ALLOW_ONLY_ABILITY);
        }
    }
}

bool Aura::IsLastAuraOnHolder()
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (i != GetEffIndex() && GetHolder()->m_auras[i])
            return false;
    return true;
}

bool Aura::HasMechanic(uint32 mechanic) const
{
    return GetSpellProto()->Mechanic == mechanic ||
           GetSpellProto()->EffectMechanic[m_effIndex] == mechanic;
}

inline bool IsRemovedOnShapeshiftLost(SpellEntry const* spellproto, ObjectGuid const& casterGuid, ObjectGuid const& targetGuid)
{
    if (casterGuid == targetGuid)
    {
        if (spellproto->HasAttribute(SPELL_ATTR_AURA_IS_DEBUFF))
            return false;

        if (spellproto->Stances[0])
        {
            switch (spellproto->Id)
            {
                case 11327: // vanish stealth aura improvements are removed on stealth removal
                case 11329: // but they have attribute SPELL_ATTR_NOT_SHAPESHIFT
                case 26888: // maybe relic from when they had Effect1?
                    return true;
                default:
                    break;
            }

            if (!spellproto->HasAttribute(SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED) && !spellproto->HasAttribute(SPELL_ATTR_NOT_SHAPESHIFT))
                return true;
        }
        else if (spellproto->SpellFamilyName == SPELLFAMILY_DRUID && spellproto->EffectApplyAuraName[0] == SPELL_AURA_MOD_DODGE_PERCENT)
            return true;
    }

    return false;
    /*TODO: investigate spellid 24864  or (SpellFamilyName = 7 and EffectApplyAuraName_1 = 49 and stances = 0)*/
}

SpellAuraHolder::SpellAuraHolder(SpellEntry const* spellproto, Unit* target, WorldObject* caster, Item* castItem, SpellEntry const* triggeredBy) :
    m_spellProto(spellproto), m_target(target),
    m_castItemGuid(castItem ? castItem->GetObjectGuid() : ObjectGuid()), m_triggeredBy(triggeredBy),
    m_spellAuraHolderState(SPELLAURAHOLDER_STATE_CREATED), m_auraSlot(MAX_AURAS), m_auraFlags(AFLAG_NONE),
    m_auraLevel(1), m_procCharges(0),
    m_stackAmount(1), m_timeCla(1000), m_removeMode(AURA_REMOVE_BY_DEFAULT),
    m_AuraDRGroup(DIMINISHING_NONE), m_permanent(false), m_isRemovedOnShapeLost(true),
    m_heartbeatResistChance(0), m_heartbeatResistTimer(0), m_heartbeatResistInterval(0),
    m_deleted(false), m_skipUpdate(false), m_reducedProcChancePast60(false),
    m_auraScript(SpellScriptMgr::GetAuraScript(spellproto->Id))
{
    MANGOS_ASSERT(target);
    MANGOS_ASSERT(spellproto && spellproto == sSpellTemplate.LookupEntry<SpellEntry>(spellproto->Id) && "`info` must be pointer to sSpellTemplate element");

    if (!caster)
        m_casterGuid = target->GetObjectGuid();
    else
        m_casterGuid = caster->GetObjectGuid();

    m_applyTime      = time(nullptr);
    m_applyMSTime    = target->GetMap()->GetCurrentMSTime();
    m_isPassive      = IsPassiveSpell(spellproto);
    m_isDeathPersist = IsDeathPersistentSpell(spellproto);
    m_trackedAuraType = sSpellMgr.IsSingleTargetSpell(spellproto) ? TRACK_AURA_TYPE_SINGLE_TARGET : IsSpellHaveAura(spellproto, SPELL_AURA_CONTROL_VEHICLE) ? TRACK_AURA_TYPE_CONTROL_VEHICLE : TRACK_AURA_TYPE_NOT_TRACKED;
    m_procCharges    = spellproto->procCharges;

    m_isRemovedOnShapeLost = IsRemovedOnShapeshiftLost(m_spellProto, GetCasterGuid(), target->GetObjectGuid());

    Unit* unitCaster = caster && caster->isType(TYPEMASK_UNIT) ? (Unit*)caster : nullptr;

    m_duration = m_maxDuration = CalculateSpellDuration(spellproto, unitCaster, target, m_auraScript);

    if (m_maxDuration == -1 || (m_isPassive && spellproto->DurationIndex == 0))
        m_permanent = true;

    if (unitCaster)
    {
        if (Player* modOwner = unitCaster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_CHARGES, m_procCharges);
    }

    // some custom stack values at aura holder create
    switch (m_spellProto->Id)
    {
        // some auras applied with max stack
        case 24575:                                         // Brittle Armor
        case 24659:                                         // Unstable Power
        case 24662:                                         // Restless Strength
        case 26464:                                         // Mercurial Shield
        case 32065:                                         // Fungal Decay
        case 34027:                                         // Kill Command
        case 36659:                                         // Tail Sting
        case 37589:                                         // Shriveling Gaze
        case 40157:                                         // Spirit Lance
        case 55166:                                         // Tidal Force
        case 58914:                                         // Kill Command (pet part)
        case 62519:                                         // Attuned to Nature
        case 63050:                                         // Sanity
        case 64455:                                         // Feral Essence
        case 65294:                                         // Empowered
        case 70672:                                         // Gaseous Bloat
        case 71564:                                         // Deadly Precision
            m_stackAmount = m_spellProto->StackAmount;
            break;
    }

    for (auto& m_aura : m_auras)
        m_aura = nullptr;

    OnHolderInit(caster);
}

void SpellAuraHolder::AddAura(Aura* aura, SpellEffectIndex index)
{
    m_auras[index] = aura;
    m_auraFlags |= (1 << index);
}

void SpellAuraHolder::RemoveAura(SpellEffectIndex index)
{
    m_auras[index] = nullptr;
    m_auraFlags &= ~(1 << index);
}

void SpellAuraHolder::ApplyAuraModifiers(bool apply, bool real)
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX && !IsDeleted(); ++i)
        if (Aura* aur = GetAuraByEffectIndex(SpellEffectIndex(i)))
            aur->ApplyModifier(apply, real);
}

void SpellAuraHolder::_AddSpellAuraHolder()
{
    if (!GetId())
        return;
    if (!m_target)
        return;

    // Try find slot for aura
    uint8 slot = NULL_AURA_SLOT;

    // Lookup free slot
    if (m_target->GetVisibleAurasCount() < MAX_AURAS)
    {
        Unit::VisibleAuraMap const& visibleAuras = m_target->GetVisibleAuras();
        for (uint8 i = 0; i < MAX_AURAS; ++i)
        {
            Unit::VisibleAuraMap::const_iterator itr = visibleAuras.find(i);
            if (itr == visibleAuras.end())
            {
                slot = i;
                // update for out of range group members (on 1 slot use)
                m_target->UpdateAuraForGroup(slot);
                break;
            }
        }
    }

    Unit* caster = GetCaster();

    uint8 flags = 0;
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (m_auras[i])
            flags |= (1 << i);
    }
    flags |= ((GetCasterGuid() == GetTarget()->GetObjectGuid()) ? AFLAG_NOT_CASTER : AFLAG_NONE) | ((GetSpellMaxDuration(m_spellProto) > 0) ? AFLAG_DURATION : AFLAG_NONE) | (IsPositive() ? AFLAG_POSITIVE : AFLAG_NEGATIVE);
    SetAuraFlags(flags);

    SetAuraLevel(caster ? caster->GetLevel() : sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));

    if (IsNeedVisibleSlot(caster))
    {
        SetAuraSlot(slot);
        if (slot < MAX_AURAS)                       // slot found send data to client
        {
            SetVisibleAura(false);
            SendAuraUpdate(false);
        }

        //*****************************************************
        // Update target aura state flag on holder apply
        // TODO: Make it easer
        //*****************************************************

        // Sitdown on apply aura req seated
        if (m_spellProto->AuraInterruptFlags & AURA_INTERRUPT_FLAG_STANDING_CANCELS && !m_target->IsSitState())
            m_target->SetStandState(UNIT_STAND_STATE_SIT);

        // register aura diminishing on apply
        if (getDiminishGroup() != DIMINISHING_NONE)
            m_target->ApplyDiminishingAura(getDiminishGroup(), true);

        // Update Seals information
        if (IsSealSpell(m_spellProto))
            m_target->ModifyAuraState(AURA_STATE_JUDGEMENT, true);

        if (m_spellProto->HasAttribute(SPELL_ATTR_EX_PREVENTS_ANIM))
            m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREVENT_ANIM);

        // Conflagrate aura state on Immolate and Shadowflame
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_WARLOCK, uint64(0x0000000000000004), 0x00000002))
            m_target->ModifyAuraState(AURA_STATE_CONFLAGRATE, true);

        // Faerie Fire (druid versions)
        if (m_spellProto->HasAttribute(SPELL_ATTR_SS_PREVENT_INVIS))
            m_target->ModifyAuraState(AURA_STATE_FAERIE_FIRE, true);

        // Victorious
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_WARRIOR, uint64(0x0004000000000000)))
            m_target->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, true);

        // Swiftmend state on Regrowth & Rejuvenation
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_DRUID, uint64(0x0000000000000050)))
            m_target->ModifyAuraState(AURA_STATE_SWIFTMEND, true);

        // Deadly poison aura state
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_ROGUE, uint64(0x0000000000010000)))
            m_target->ModifyAuraState(AURA_STATE_DEADLY_POISON, true);

        // Enrage aura state
        if (m_spellProto->Dispel == DISPEL_ENRAGE)
            m_target->ModifyAuraState(AURA_STATE_ENRAGE, true);

        // Bleeding aura state
        if (GetAllSpellMechanicMask(m_spellProto) & (1 << (MECHANIC_BLEED - 1)))
            m_target->ModifyAuraState(AURA_STATE_BLEEDING, true);
    }
}

void SpellAuraHolder::_RemoveSpellAuraHolder()
{
    // Remove all triggered by aura spells vs unlimited duration
    // except same aura replace case
    if (m_removeMode != AURA_REMOVE_BY_STACK)
        CleanupTriggeredSpells();

    Unit* caster = GetCaster();

    if (caster && IsPersistent())
        if (DynamicObject* dynObj = caster->GetDynObject(GetId()))
            dynObj->RemoveAffected(m_target);

    // remove at-store spell cast items (for all remove modes?)
    if (m_target->GetTypeId() == TYPEID_PLAYER && m_removeMode != AURA_REMOVE_BY_DEFAULT && m_removeMode != AURA_REMOVE_BY_DELETE)
        if (ObjectGuid castItemGuid = GetCastItemGuid())
            if (Item* castItem = ((Player*)m_target)->GetItemByGuid(castItemGuid))
                ((Player*)m_target)->DestroyItemWithOnStoreSpell(castItem, GetId());

    // passive auras do not get put in slots - said who? ;)
    // Note: but totem can be not accessible for aura target in time remove (to far for find in grid)
    // if (m_isPassive && !(caster && caster->GetTypeId() == TYPEID_UNIT && ((Creature*)caster)->IsTotem()))
    //    return;

    uint8 slot = GetAuraSlot();

    if (slot >= MAX_AURAS)                                  // slot not set
        return;

    if (m_target->GetVisibleAura(slot) == 0)
        return;

    // unregister aura diminishing (and store last time)
    if (getDiminishGroup() != DIMINISHING_NONE)
        m_target->ApplyDiminishingAura(getDiminishGroup(), false);

    SetAuraFlags(AFLAG_NONE);
    SetAuraLevel(0);
    SetVisibleAura(true);

    if (m_removeMode != AURA_REMOVE_BY_DELETE)
    {
        SendAuraUpdate(true);

        // update for out of range group members
        m_target->UpdateAuraForGroup(slot);

        //*****************************************************
        // Update target aura state flag (at last aura remove)
        //*****************************************************
        // Enrage aura state
        if (m_spellProto->Dispel == DISPEL_ENRAGE)
            m_target->ModifyAuraState(AURA_STATE_ENRAGE, false);

        // Bleeding aura state
        if (GetAllSpellMechanicMask(m_spellProto) & (1 << (MECHANIC_BLEED - 1)))
        {
            bool found = false;

            Unit::SpellAuraHolderMap const& holders = m_target->GetSpellAuraHolderMap();
            for (const auto& holder : holders)
            {
                if (GetAllSpellMechanicMask(holder.second->GetSpellProto()) & (1 << (MECHANIC_BLEED - 1)))
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                m_target->ModifyAuraState(AURA_STATE_BLEEDING, false);
        }

        uint32 removeState = 0;
        ClassFamilyMask removeFamilyFlag = m_spellProto->SpellFamilyFlags;
        switch (m_spellProto->SpellFamilyName)
        {
            case SPELLFAMILY_PALADIN:
                if (IsSealSpell(m_spellProto))
                    removeState = AURA_STATE_JUDGEMENT;     // Update Seals information
                break;
            case SPELLFAMILY_WARLOCK:
                // Conflagrate aura state on Immolate and Shadowflame,
                if (m_spellProto->IsFitToFamilyMask(uint64(0x0000000000000004), 0x00000002))
                {
                    removeFamilyFlag = ClassFamilyMask(uint64(0x0000000000000004), 0x00000002);
                    removeState = AURA_STATE_CONFLAGRATE;
                }
                break;
            case SPELLFAMILY_DRUID:
                if (m_spellProto->IsFitToFamilyMask(uint64(0x0000000000000050)))
                {
                    removeFamilyFlag = ClassFamilyMask(uint64(0x00000000000050));
                    removeState = AURA_STATE_SWIFTMEND;     // Swiftmend aura state
                }
                break;
            case SPELLFAMILY_WARRIOR:
                if (m_spellProto->IsFitToFamilyMask(uint64(0x0004000000000000)))
                    removeState = AURA_STATE_WARRIOR_VICTORY_RUSH; // Victorious
                break;
            case SPELLFAMILY_ROGUE:
                if (m_spellProto->IsFitToFamilyMask(uint64(0x0000000000010000)))
                    removeState = AURA_STATE_DEADLY_POISON; // Deadly poison aura state
                break;
            case SPELLFAMILY_HUNTER:
                if (m_spellProto->IsFitToFamilyMask(uint64(0x1000000000000000)))
                    removeState = AURA_STATE_FAERIE_FIRE;   // Sting (hunter versions)
        }

        if (m_spellProto->HasAttribute(SPELL_ATTR_SS_PREVENT_INVIS))
            removeState = AURA_STATE_FAERIE_FIRE;   // Faerie Fire

        // Remove state (but need check other auras for it)
        if (removeState)
            // this has been last aura
            if (!m_target->HasAuraWithCondition([spellProto = m_spellProto, removeFamilyFlag](SpellAuraHolder* holder)
            {
                SpellEntry const* auraSpellInfo = holder->GetSpellProto();
                if (auraSpellInfo->IsFitToFamily(SpellFamily(spellProto->SpellFamilyName), removeFamilyFlag))
                    return true;
                return false;
            }))
                m_target->ModifyAuraState(AuraState(removeState), false);

        if (m_spellProto->HasAttribute(SPELL_ATTR_EX_PREVENTS_ANIM))
            if (!m_target->HasAuraWithCondition([](SpellAuraHolder* holder)
            {
                if (holder->GetSpellProto()->HasAttribute(SPELL_ATTR_EX_PREVENTS_ANIM))
                    return true;
                return false;
            }))
                m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREVENT_ANIM);

        // reset cooldown state for spells
        if (caster && GetSpellProto()->HasAttribute(SPELL_ATTR_COOLDOWN_ON_EVENT))
        {
            // some spells need to start cooldown at aura fade (like stealth)
            caster->AddCooldown(*GetSpellProto());
        }
    }
}

void SpellAuraHolder::CleanupTriggeredSpells()
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!m_spellProto->EffectApplyAuraName[i])
            continue;

        uint32 tSpellId = m_spellProto->EffectTriggerSpell[i];
        if (!tSpellId)
            continue;

        SpellEntry const* tProto = sSpellTemplate.LookupEntry<SpellEntry>(tSpellId);
        if (!tProto)
            continue;

        if (GetSpellDuration(tProto) != -1)
            continue;

        // needed for spell 43680, maybe others
        // TODO: is there a spell flag, which can solve this in a more sophisticated way?
        if (m_spellProto->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_TRIGGER_SPELL &&
                GetSpellDuration(m_spellProto) == int32(m_spellProto->EffectAmplitude[i]))
            continue;

        m_target->RemoveAurasDueToSpell(tSpellId);
    }
}

bool SpellAuraHolder::ModStackAmount(int32 num, Unit* newCaster)
{
    uint32 protoStackAmount = m_spellProto->StackAmount;

    // Can`t mod
    if (!protoStackAmount)
        return true;

    // Modify stack but limit it
    int32 stackAmount = m_stackAmount + num;
    if (stackAmount > (int32)protoStackAmount)
        stackAmount = protoStackAmount;
    else if (stackAmount <= 0) // Last aura from stack removed
    {
        m_stackAmount = 0;
        return true; // need remove aura
    }

    // Update stack amount
    SetStackAmount(stackAmount, newCaster);
    return false;
}

void SpellAuraHolder::SetStackAmount(uint32 stackAmount, Unit* newCaster)
{
    Unit* target = GetTarget();
    if (!target)
        return;

    bool refresh = false;
    if (stackAmount >= m_stackAmount)
    {
        // Change caster
        Unit* oldCaster = GetCaster();
        if (newCaster && oldCaster != newCaster)
        {
            m_casterGuid = newCaster->GetObjectGuid();
            // New caster duration sent for owner in RefreshHolder
        }

        refresh = true;
    }

    int32 oldStackAmount = m_stackAmount;
    if (m_spellProto->Id == 32264) // temporary hack for Inhibit Magic
    {
        OnHolderInit(newCaster);
        stackAmount = m_stackAmount;
        m_stackAmount = oldStackAmount;
    }
    m_stackAmount = stackAmount;

    for (auto aur : m_auras)
    {
        if (aur)
        {
            int32 baseAmount = aur->GetModifier()->m_baseAmount;
            int32 amount = m_stackAmount * baseAmount;
            amount = aur->OnAuraValueCalculate(newCaster, amount);
            // Reapply if amount change
            if (!baseAmount || amount != aur->GetModifier()->m_amount)
            {
                aur->SetRemoveMode(AURA_REMOVE_BY_GAINED_STACK);
                if (IsAuraRemoveOnStacking(this->GetSpellProto(), aur->GetEffIndex()))
                    aur->ApplyModifier(false, true);
                aur->GetModifier()->m_amount = amount;
                aur->GetModifier()->m_recentAmount = baseAmount * (stackAmount - oldStackAmount);
                aur->ApplyModifier(true, true);
            }
        }
    }

    if (refresh) // Stack increased refresh duration
        RefreshHolder();
    else // Stack decreased only send update
        SendAuraUpdate(false);
}

Unit* SpellAuraHolder::GetCaster() const
{
    if (m_casterGuid == m_target->GetObjectGuid())
        return m_target;

    if (m_casterGuid.IsGameObject())
        return nullptr;

    if (!m_target->IsInWorld())
        return nullptr;

    return m_target->GetMap()->GetUnit(m_casterGuid);
}

bool SpellAuraHolder::IsWeaponBuffCoexistableWith(SpellAuraHolder const* ref) const
{
    // only item casted spells
    if (!GetCastItemGuid())
        return false;

    // Exclude Debuffs
    if (!IsPositive())
        return false;

    // Exclude Non-generic Buffs [ie: Runeforging] and Executioner-Enchant
    if (GetSpellProto()->SpellFamilyName != SPELLFAMILY_GENERIC || GetId() == 42976)
        return false;

    // Exclude Stackable Buffs [ie: Blood Reserve]
    if (GetSpellProto()->StackAmount)
        return false;

    // only self applied player buffs
    if (m_target->GetTypeId() != TYPEID_PLAYER || m_target->GetObjectGuid() != GetCasterGuid())
        return false;

    Item* castItem = ((Player*)m_target)->GetItemByGuid(GetCastItemGuid());
    if (!castItem)
        return false;

    // Limit to Weapon-Slots
    if (!castItem->IsEquipped() ||
            (castItem->GetSlot() != EQUIPMENT_SLOT_MAINHAND && castItem->GetSlot() != EQUIPMENT_SLOT_OFFHAND))
        return false;

    // from different weapons
    return ref->GetCastItemGuid() && ref->GetCastItemGuid() != GetCastItemGuid();
}

bool SpellAuraHolder::IsNeedVisibleSlot(Unit const* caster) const
{
    bool totemAura = caster && caster->GetTypeId() == TYPEID_UNIT && ((Creature*)caster)->IsTotem();

    if (m_spellProto->procFlags)
        return true;
    if (IsSpellTriggerSpellByAura(m_spellProto))
        return true;
    if (IsSpellHaveAura(m_spellProto, SPELL_AURA_MOD_IGNORE_SHAPESHIFT))
        return true;
    if (IsSpellHaveAura(m_spellProto, SPELL_AURA_IGNORE_UNIT_STATE))
        return true;

    // passive auras (except totem auras) do not get placed in the slots
    return !m_isPassive || totemAura || HasAreaAuraEffect(m_spellProto);
}

void SpellAuraHolder::BuildUpdatePacket(WorldPacket& data) const
{
    data << uint8(GetAuraSlot());
    data << uint32(GetId());

    uint8 auraFlags = GetAuraFlags();
    data << uint8(auraFlags);
    data << uint8(GetAuraLevel());

    uint32 stackCount = m_spellProto->StackAmount ?  m_stackAmount : m_procCharges;
    data << uint8(stackCount <= 255 ? stackCount : 255);

    if (!(auraFlags & AFLAG_NOT_CASTER))
    {
        data << GetCasterGuid().WriteAsPacked();
    }

    if (auraFlags & AFLAG_DURATION)
    {
        data << uint32(GetAuraMaxDuration());
        data << uint32(GetAuraDuration());
    }
}

void SpellAuraHolder::SendAuraUpdate(bool remove) const
{
    WorldPacket data(SMSG_AURA_UPDATE);
    data << m_target->GetPackGUID();

    if (remove)
    {
        data << uint8(GetAuraSlot());
        data << uint32(0);
    }
    else
        BuildUpdatePacket(data);

    m_target->SendMessageToSet(data, true);
}

void SpellAuraHolder::HandleSpellSpecificBoosts(bool apply)
{
    bool cast_at_remove = false;                            // if spell must be casted at last aura from stack remove
    std::vector<uint32> boostSpells;

    switch (GetSpellProto()->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (GetId())
            {
                case 29865:                                 // Deathbloom (10 man)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    {
                        cast_at_remove = true;
                        boostSpells.push_back(55594);
                    }
                    else
                        return;
                    break;
                }
                case 33896: // Desperate Defense
                    boostSpells.push_back(33897);
                    break;
                case 36797: // Mind Control - Kaelthas
                    boostSpells.push_back(36798);
                    break;
                case 55053:                                 // Deathbloom (25 man)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    {
                        cast_at_remove = true;
                        boostSpells.push_back(55601);
                    }
                    else
                        return;
                    break;
                }
                case 50720:                                 // Vigilance (warrior spell but not have warrior family)
                    boostSpells.push_back(68066);                       // Damage Reduction
                    break;
                case 57350:                                 // Illusionary Barrier
                {
                    if (!apply && m_target->GetPowerType() == POWER_MANA)
                    {
                        cast_at_remove = true;
                        boostSpells.push_back(60242);                   // Darkmoon Card: Illusion
                    }
                    else
                        return;
                    break;
                }
                case 58914:                                 // Kill Command, pet aura
                {
                    // Removal is needed here because the dummy aura handler is applied / removed at stacks change
                    if (!apply)
                        if (Unit* caster = GetCaster())
                            caster->RemoveAurasDueToSpell(34027);
                    return;
                }
                case 62692:                                 // Aura of Despair
                    boostSpells.push_back(64848);
                    break;
                case 71905:                                 // Soul Fragment
                {
                    if (!apply)
                    {
                        boostSpells.push_back(72521);                   // Shadowmourne Visual Low
                        boostSpells.push_back(72523);                   // Shadowmourne Visual High
                    }
                    else
                        return;
                    break;
                }
                default:
                    return;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Ice Barrier (non stacking from one caster)
            if (m_spellProto->SpellIconID == 32)
            {
                if ((!apply && m_removeMode == AURA_REMOVE_BY_DISPEL) || m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK)
                {
                    Unit::AuraList const& dummyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                    for (auto dummyAura : dummyAuras)
                    {
                        // Shattered Barrier
                        if (dummyAura->GetSpellProto()->SpellIconID == 2945)
                        {
                            cast_at_remove = true;
                            // first rank have 50% chance
                            if (dummyAura->GetId() != 44745 || roll_chance_i(50))
                                boostSpells.push_back(55080);
                            break;
                        }
                    }
                }
                else
                    return;
                break;
            }

            switch (GetId())
            {
                case 11129:                                 // Combustion (remove triggered aura stack)
                {
                    if (!apply)
                        boostSpells.push_back(28682);
                    else
                        return;
                    break;
                }
                case 28682:                                 // Combustion (remove main aura)
                {
                    if (!apply)
                        boostSpells.push_back(11129);
                    else
                        return;
                    break;
                }
                case 44401:                                 // Missile Barrage (triggered)
                case 48108:                                 // Hot Streak (triggered)
                case 57761:                                 // Fireball! (Brain Freeze triggered)
                {
                    // consumed aura (at proc charges 0)
                    if (!apply && m_removeMode == AURA_REMOVE_BY_DEFAULT)
                    {
                        Unit* caster = GetCaster();
                        // Item - Mage T10 2P Bonus
                        if (!caster || !caster->HasAura(70752))
                            return;

                        cast_at_remove = true;
                        boostSpells.push_back(70753);                   // Pushing the Limit
                    }
                    else
                        return;
                    break;
                }
                default:
                    break; // Break here for poly below - 2.4.2+ only player poly regens
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            if (!apply)
            {
                // Remove Blood Frenzy only if target no longer has any Deep Wound or Rend (applying is handled by procs)
                if (GetSpellProto()->Mechanic != MECHANIC_BLEED)
                    return;

                // If target still has one of Warrior's bleeds, do nothing
                Unit::AuraList const& PeriodicDamage = m_target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                for (auto i : PeriodicDamage)
                    if (i->GetCasterGuid() == GetCasterGuid() &&
                        i->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARRIOR &&
                        i->GetSpellProto()->Mechanic == MECHANIC_BLEED)
                        return;

                boostSpells.push_back(30069);                           // Blood Frenzy (Rank 1)
                boostSpells.push_back(30070);                           // Blood Frenzy (Rank 2)
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Fear (non stacking)
            if (m_spellProto->SpellFamilyFlags & uint64(0x0000040000000000))
            {
                if (!apply)
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (auto dummyAura : dummyAuras)
                    {
                        SpellEntry const* dummyEntry = dummyAura->GetSpellProto();
                        // Improved Fear
                        if (dummyEntry->SpellFamilyName == SPELLFAMILY_WARLOCK && dummyEntry->SpellIconID == 98)
                        {
                            cast_at_remove = true;
                            switch (dummyAura->GetModifier()->m_amount)
                            {
                                // Rank 1
                                case 0: boostSpells.push_back(60946); break;
                                // Rank 1
                                case 1: boostSpells.push_back(60947); break;
                            }
                            break;
                        }
                    }
                }
                else
                    return;
            }
            // Shadowflame (DoT)
            else if (m_spellProto->IsFitToFamilyMask(uint64(0x0000000000000000), 0x00000002))
            {
                // Glyph of Shadowflame
                if (!apply)
                    boostSpells.push_back(63311);
                else
                {
                    Unit* caster = GetCaster();
                    if (caster && caster->HasAura(63310))
                        boostSpells.push_back(63311);
                    else
                        return;
                }
            }
            else
                return;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Power Word: Shield
            if (apply && m_spellProto->SpellFamilyFlags & uint64(0x0000000000000001) && m_spellProto->Mechanic == MECHANIC_SHIELD)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                // Glyph of Power Word: Shield
                if (Aura* glyph = caster->GetAura(55672, EFFECT_INDEX_0))
                {
                    Aura* shield = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    int32 heal = (glyph->GetModifier()->m_amount * shield->GetModifier()->m_amount) / 100;
                    caster->CastCustomSpell(m_target, 56160, &heal, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, shield);
                }
                return;
            }
            switch (GetId())
            {
                // Abolish Disease (remove 1 more poison effect with Body and Soul)
                case 552:
                {
                    if (apply)
                    {
                        int chance = 0;
                        Unit::AuraList const& dummyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                        for (auto dummyAura : dummyAuras)
                        {
                            SpellEntry const* dummyEntry = dummyAura->GetSpellProto();
                            // Body and Soul (talent ranks)
                            if (dummyEntry->SpellFamilyName == SPELLFAMILY_PRIEST && dummyEntry->SpellIconID == 2218 &&
                                    dummyEntry->SpellVisual[0] == 0)
                            {
                                chance = dummyAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
                                break;
                            }
                        }

                        if (roll_chance_i(chance))
                            boostSpells.push_back(64134);               // Body and Soul (periodic dispel effect)
                    }
                    else
                                boostSpells.push_back(64134);                   // Body and Soul (periodic dispel effect)
                    break;
                }
                // Dispersion mana reg and immunity
                case 47585:
                    boostSpells.push_back(60069);                       // Dispersion
                    boostSpells.push_back(63230);                       // Dispersion
                    break;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Barkskin
            if (GetId() == 22812 && m_target->HasAura(63057)) // Glyph of Barkskin
                boostSpells.push_back(63058);                           // Glyph - Barkskin 01
            else if (!apply && GetId() == 5229)             // Enrage (Druid Bear)
                boostSpells.push_back(51185);                           // King of the Jungle (Enrage damage aura)
            else
                return;
            break;
        }
        case SPELLFAMILY_ROGUE:
            // Sprint (skip non player casted spells by category)
            if (GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000000040) && GetSpellProto()->Category == 44)
            {
                if (!apply || m_target->HasAura(58039))     // Glyph of Blurred Speed
                    boostSpells.push_back(61922);                       // Sprint (waterwalk)
                else
                    return;
            }
            else
                return;
            break;
        case SPELLFAMILY_HUNTER:
        {
            switch (GetId())
            {
                case 34074:                                 // Aspect of the Viper
                {
                    if (!apply || m_target->HasAura(60144)) // Viper Attack Speed
                        boostSpells.push_back(61609);                   // Vicious Viper
                    else
                        return;
                    break;
                }
                case 19574:                                 // Bestial Wrath - immunity
                case 34471:                                 // The Beast Within - immunity
                {
                    boostSpells.push_back(24395);
                    boostSpells.push_back(24396);
                    boostSpells.push_back(24397);
                    boostSpells.push_back(26592);
                    break;
                }
                case 34027:                                 // Kill Command, owner aura (spellmods)
                {
                    if (apply)
                    {
                        if (m_target->HasAura(35029))       // Focused Fire, rank 1
                            boostSpells.push_back(60110);               // Kill Command, Focused Fire rank 1 bonus
                        else if (m_target->HasAura(35030))  // Focused Fire, rank 2
                            boostSpells.push_back(60113);               // Kill Command, Focused Fire rank 2 bonus
                        else
                            return;
                    }
                    else
                    {
                        boostSpells.push_back(34026);                   // Kill Command, owner casting aura
                        boostSpells.push_back(60110);                   // Kill Command, Focused Fire rank 1 bonus
                        boostSpells.push_back(60113);                   // Kill Command, Focused Fire rank 2 bonus
                        if (Unit* pet = m_target->GetPet())
                            pet->RemoveAurasDueToSpell(58914); // Kill Command, pet aura
                    }
                    break;
                }
                case 35029:                                 // Focused Fire, rank 1
                {
                    if (apply && !m_target->HasAura(34027)) // Kill Command, owner casting aura
                        return;

                    boostSpells.push_back(60110);                       // Kill Command, Focused Fire rank 1 bonus
                    break;
                }
                case 35030:                                 // Focused Fire, rank 2
                {
                    if (apply && !m_target->HasAura(34027)) // Kill Command, owner casting aura
                        return;

                    boostSpells.push_back(60113);                       // Kill Command, Focused Fire rank 2 bonus
                    break;
                }
                default:
                    // Freezing Trap Effect
                    if (m_spellProto->SpellFamilyFlags & uint64(0x0000000000000008))
                    {
                        if (!apply)
                        {
                            Unit* caster = GetCaster();
                            // Glyph of Freezing Trap
                            if (caster && caster->HasAura(56845))
                            {
                                cast_at_remove = true;
                                boostSpells.push_back(61394);
                            }
                            else
                                return;
                        }
                        else
                            return;
                    }
                    // Aspect of the Dragonhawk dodge
                    else if (GetSpellProto()->IsFitToFamilyMask(uint64(0x0000000000000000), 0x00001000))
                    {
                        boostSpells.push_back(61848);

                        // triggered spell have same category as main spell and cooldown
                        if (apply && m_target->GetTypeId() == TYPEID_PLAYER)
                            m_target->RemoveSpellCooldown(61848);
                    }
                    else
                        return;
                    break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (m_spellProto->Id == 31884)                  // Avenging Wrath
            {
                if (!apply)
                    boostSpells.push_back(57318);                       // Sanctified Wrath (triggered)
                else
                {
                    int32 percent = 0;
                    Unit::AuraList const& dummyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                    for (auto dummyAura : dummyAuras)
                    {
                        if (dummyAura->GetSpellProto()->SpellIconID == 3029)
                        {
                            percent = dummyAura->GetModifier()->m_amount;
                            break;
                        }
                    }

                    // apply in special way
                    if (percent)
                    {
                        // Sanctified Wrath (triggered)
                        // prevent aura deletion, specially in multi-boost case
                        m_target->CastCustomSpell(m_target, 57318, &percent, &percent, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr);
                    }
                    return;
                }
                break;
            }

            // Only process on player casting paladin aura
            // all aura bonuses applied also in aura area effect way to caster
            if (GetCasterGuid() != m_target->GetObjectGuid() || !GetCasterGuid().IsPlayer())
                return;

            if (GetSpellSpecific(m_spellProto->Id) != SPELL_AURA)
                return;

            // Sanctified Retribution and Swift Retribution (they share one aura), but not Retribution Aura (already gets modded)
            if ((GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000000008)) == 0)
                boostSpells.push_back(63531);                           // placeholder for talent spell mods
            // Improved Concentration Aura (auras bonus)
            boostSpells.push_back(63510);                               // placeholder for talent spell mods
            // Improved Devotion Aura (auras bonus)
            boostSpells.push_back(63514);                               // placeholder for talent spell mods
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // second part of spell apply
            switch (GetId())
            {
                case 49039: boostSpells.push_back(50397); break;        // Lichborne

                case 48263:                                 // Frost Presence
                case 48265:                                 // Unholy Presence
                case 48266:                                 // Blood Presence
                {
                    // else part one per 3 pair
                    if (GetId() == 48263 || GetId() == 48265) // Frost Presence or Unholy Presence
                    {
                        // Improved Blood Presence
                        int32 heal_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& bloodAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for (auto bloodAura : bloodAuras)
                            {
                                // skip same icon
                                if (bloodAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT &&
                                    bloodAura->GetSpellProto()->SpellIconID == 2636)
                                {
                                    heal_pct = bloodAura->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (heal_pct)
                            m_target->CastCustomSpell(m_target, 63611, &heal_pct, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        else
                            m_target->RemoveAurasDueToSpell(63611);
                    }
                    else
                        boostSpells.push_back(63611);                   // Improved Blood Presence, trigger for heal

                    if (GetId() == 48263 || GetId() == 48266) // Frost Presence or Blood Presence
                    {
                        // Improved Unholy Presence
                        int32 power_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& unholyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for (auto unholyAura : unholyAuras)
                            {
                                // skip same icon
                                if (unholyAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT &&
                                    unholyAura->GetSpellProto()->SpellIconID == 2633)
                                {
                                    power_pct = unholyAura->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }
                        if (power_pct || !apply)
                            boostSpells.push_back(49772);               // Unholy Presence, speed part, spell1 used for Improvement presence fit to own presence
                    }
                    else
                                boostSpells.push_back(49772);                   // Unholy Presence move speed

                    if (GetId() == 48265 || GetId() == 48266)   // Unholy Presence or Blood Presence
                    {
                        // Improved Frost Presence
                        int32 stamina_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& frostAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for (auto frostAura : frostAuras)
                            {
                                // skip same icon
                                if (frostAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT &&
                                    frostAura->GetSpellProto()->SpellIconID == 2632)
                                {
                                    stamina_pct = frostAura->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (stamina_pct)
                            m_target->CastCustomSpell(m_target, 61261, &stamina_pct, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        else
                            m_target->RemoveAurasDueToSpell(61261);
                    }
                    else
                        boostSpells.push_back(61261);                   // Frost Presence, stamina

                    if (GetId() == 48265)                   // Unholy Presence
                    {
                        // Improved Unholy Presence, special case for own presence
                        int32 power_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& unholyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for (auto unholyAura : unholyAuras)
                            {
                                // skip same icon
                                if (unholyAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT &&
                                    unholyAura->GetSpellProto()->SpellIconID == 2633)
                                {
                                    power_pct = unholyAura->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (power_pct)
                        {
                            int32 bp = 5;
                            m_target->CastCustomSpell(m_target, 63622, &bp, &bp, &bp, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                            m_target->CastCustomSpell(m_target, 65095, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                        }
                        else
                        {
                            m_target->RemoveAurasDueToSpell(63622);
                            m_target->RemoveAurasDueToSpell(65095);
                        }
                    }
                    break;
                }
            }

            // Improved Blood Presence
            if (GetSpellProto()->SpellIconID == 2636 && m_isPassive)
            {
                // if presence active: Frost Presence or Unholy Presence
                if (apply && (m_target->HasAura(48263) || m_target->HasAura(48265)))
                {
                    Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (!aura)
                        return;

                    int32 bp = aura->GetModifier()->m_amount;
                    m_target->CastCustomSpell(m_target, 63611, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                }
                else
                    m_target->RemoveAurasDueToSpell(63611);
                return;
            }

            // Improved Frost Presence
            if (GetSpellProto()->SpellIconID == 2632 && m_isPassive)
            {
                // if presence active: Unholy Presence or Blood Presence
                if (apply && (m_target->HasAura(48265) || m_target->HasAura(48266)))
                {
                    Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (!aura)
                        return;

                    int32 bp0 = aura->GetModifier()->m_amount;
                    int32 bp1 = 0;                          // disable threat mod part for not Frost Presence case
                    m_target->CastCustomSpell(m_target, 61261, &bp0, &bp1, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                }
                else
                    m_target->RemoveAurasDueToSpell(61261);
                return;
            }

            // Improved Unholy Presence
            if (GetSpellProto()->SpellIconID == 2633 && m_isPassive)
            {
                // if presence active: Unholy Presence
                if (apply && m_target->HasAura(48265))
                {
                    int32 bp = 5;
                    m_target->CastCustomSpell(m_target, 63622, &bp, &bp, &bp, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                    m_target->CastCustomSpell(m_target, 65095, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetCasterGuid());
                }
                else
                {
                    m_target->RemoveAurasDueToSpell(63622);
                    m_target->RemoveAurasDueToSpell(65095);
                }

                // if presence active: Frost Presence or Blood Presence
                if (!apply || m_target->HasAura(48263) || m_target->HasAura(48266))
                    boostSpells.push_back(49772);
                else
                    return;
                break;
            }
            break;
        }
        default:
            return;
    }

    if (GetSpellProto()->Mechanic == MECHANIC_POLYMORPH)
        boostSpells.push_back(12939); // Just so that this doesnt conflict with others

    if (boostSpells.empty())
        return;

    for (uint32 spellId : boostSpells)
    {
        Unit* boostCaster = m_target;
        Unit* boostTarget = nullptr;
        ObjectGuid casterGuid = m_target->GetObjectGuid(); // caster can be nullptr, but guid is still valid for removal
        SpellEntry const* boostEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        for (uint32 target : boostEntry->EffectImplicitTargetA)
        {
            switch (target)
            {
                case TARGET_UNIT_ENEMY:
                case TARGET_UNIT:
                    if (apply) // optimization
                        boostCaster = GetCaster();
                    else
                        casterGuid = GetCasterGuid();
                    boostTarget = m_target;
                    break;
            }
        }
        if (apply || cast_at_remove)
            boostCaster->CastSpell(boostTarget, boostEntry, TRIGGERED_OLD_TRIGGERED);
        else
            m_target->RemoveAurasByCasterSpell(spellId, casterGuid);
    }
}

SpellAuraHolder::~SpellAuraHolder()
{
    // note: auras in delete list won't be affected since they clear themselves from holder when adding to deletedAuraslist
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        delete m_auras[i];
}

void SpellAuraHolder::Update(uint32 diff)
{
    if (m_skipUpdate)
    {
        m_skipUpdate = false;
        if (m_applyMSTime == GetTarget()->GetMap()->GetCurrentMSTime()) // do not tick in same tick as created
            return;
    }

    for (auto aura : m_auras)
        if (aura)
            aura->UpdateAura(diff);

    if (m_duration > 0)
    {
        m_duration -= diff;
        if (m_duration < 0)
            m_duration = 0;

        m_timeCla -= diff;

        if (m_timeCla <= 0)
        {
            if (!GetSpellProto()->HasAttribute(SPELL_ATTR_EX2_NO_TARGET_PER_SECOND_COSTS) || GetCasterGuid() == GetTarget()->GetObjectGuid())
            {
                Powers powertype = Powers(GetSpellProto()->powerType);
                int32 manaPerSecond = GetSpellProto()->manaPerSecond + GetSpellProto()->manaPerSecondPerLevel * GetTarget()->GetLevel();
                m_timeCla = 1 * IN_MILLISECONDS;

                if (manaPerSecond)
                {
                    if (powertype == POWER_HEALTH)
                    {
                        if (GetTarget()->GetHealth() <= (uint32)manaPerSecond)
                        {
                            // cannot apply damage part so we have to cancel responsible aura
                            GetTarget()->RemoveAurasDueToSpell(GetId());

                            // finish current generic/channeling spells, don't affect autorepeat
                            GetTarget()->FinishSpell(CURRENT_GENERIC_SPELL);
                            GetTarget()->FinishSpell(CURRENT_CHANNELED_SPELL);
                        }
                        else
                            GetTarget()->ModifyHealth(-manaPerSecond);
                    }
                    else
                        GetTarget()->ModifyPower(powertype, -manaPerSecond);
                }
            }
        }

        if (m_duration && !IsDeleted())
            UpdateHeartbeatResist(diff);
    }
}

void SpellAuraHolder::RefreshHolder()
{
    SetAuraDuration(GetAuraMaxDuration());
    SendAuraUpdate(false);
}

void SpellAuraHolder::SetAuraMaxDuration(int32 duration)
{
    m_maxDuration = duration;

    // possible overwrite persistent state
    if (!GetSpellProto()->HasAttribute(SPELL_ATTR_EX5_DO_NOT_DISPLAY_DURATION) && duration > 0)
    {
        if (!(IsPassive() && GetSpellProto()->DurationIndex == 0))
            SetPermanent(false);

        SetAuraFlags(GetAuraFlags() | AFLAG_DURATION);
    }
    else
        SetAuraFlags(GetAuraFlags() & ~AFLAG_DURATION);
}

bool SpellAuraHolder::DropAuraCharge()
{
    if (m_procCharges == 0)
        return false;

    --m_procCharges;

    SendAuraUpdate(false);

    if (GetCasterGuid() != m_target->GetObjectGuid() && IsAreaAura())
        if (Unit* caster = GetCaster())
            caster->RemoveAuraCharge(m_spellProto->Id);

    return m_procCharges == 0;
}

void SpellAuraHolder::ResetSpellModCharges()
{
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; i++)
        if (m_auras[i])
            if (SpellModifier* spellMod = m_auras[i]->GetSpellModifier())
                spellMod->charges = m_procCharges;
}

bool SpellAuraHolder::HasModifier(const uint64& modId) const
{
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; i++)
        if (m_auras[i] && m_auras[i]->GetSpellModifier())
            if (m_auras[i]->GetSpellModifier()->modId == modId)
                return true;
    return false;
}

bool SpellAuraHolder::HasMechanic(uint32 mechanic) const
{
    if (mechanic == m_spellProto->Mechanic)
        return true;

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (m_auras[i] && m_spellProto->EffectMechanic[i] == mechanic)
            return true;
    return false;
}

bool SpellAuraHolder::HasMechanicMask(uint32 mechanicMask) const
{
    if (mechanicMask & (1 << (m_spellProto->Mechanic - 1)))
        return true;

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (m_auras[i] && m_spellProto->EffectMechanic[i] && ((1 << (m_spellProto->EffectMechanic[i] - 1)) & mechanicMask))
            return true;
    return false;
}

bool SpellAuraHolder::IsDispellableByMask(uint32 dispelMask, Unit const* caster, SpellEntry const* spellInfo) const
{
    if ((1 << GetSpellProto()->Dispel) & dispelMask)
    {
        if (GetSpellProto()->Dispel == DISPEL_MAGIC)
        {
            // do not remove positive auras if friendly target
            //               negative auras if non-friendly target
            bool positive = IsPositive();
            if (positive == caster->CanAssistSpell(GetTarget(), spellInfo))
                if (positive || !IsCharm())
                    return false;

            // Unholy Blight prevents dispel of diseases from target
            if (GetSpellProto()->Dispel == DISPEL_DISEASE)
                if (GetTarget()->HasAura(50536))
                    return false;
        }
        return true;
    }
    return false;
}

bool SpellAuraHolder::IsPersistent() const
{
    for (auto aur : m_auras)
        if (aur)
            if (aur->IsPersistent())
                return true;
    return false;
}

bool SpellAuraHolder::IsAreaAura() const
{
    for (auto aur : m_auras)
        if (aur)
            if (aur->IsAreaAura())
                return true;
    return false;
}

bool SpellAuraHolder::IsPositive() const
{
    for (auto aur : m_auras)
        if (aur)
            if (!aur->IsPositive())
                return false;
    return true;
}

bool SpellAuraHolder::IsEmptyHolder() const
{
    for (auto m_aura : m_auras)
        if (m_aura)
            return false;
    return true;
}

bool SpellAuraHolder::IsSaveToDbHolder() const
{
    if (IsPassive() || IsChanneledSpell(GetSpellProto()) || IsItemAura(GetSpellProto()))
        return false;

    if (GetTrackedAuraType() == TRACK_AURA_TYPE_SINGLE_TARGET)
        return false;

    if (m_spellProto->AuraInterruptFlags & AURA_INTERRUPT_FLAG_LEAVE_WORLD)
        return false;

    if (IsPermanent())
    {
        // world buff auras should not be saved
        switch (m_spellProto->Id)
        {
            case 11413: // echoes of lordaeron
            case 11414: // echoes of lordaeron
            case 11415: // echoes of lordaeron
            case 1386: // echoes of lordaeron
            case 30880: // echoes of lordaeron
            case 30683: // echoes of lordaeron
            case 30682: // echoes of lordaeron
            case 29520: // echoes of lordaeron
            case 30754: // cenarion favor
            case 32071: // hellfire superiority
            case 32049: // hellfire superiority
            case 33377: // blessing of auchindoun
            case 33779: // twin spire blessing
            case 33795: // halaani strength
            case 39911: // nazgrels command
            case 39913: // trollbanes command
            case 39953: // adals song of battle
            case 45444: // bonfires blessing
            case 46302: // kirus song of victory
            case 47521: // Mole Machine Player Hide and Root
                return false;
        }
    }
    return true;
}

bool SpellAuraHolder::IsCharm() const
{
    for (auto m_aura : m_auras)
        if (m_aura && IsCharmAura(m_spellProto, m_aura->GetEffIndex()))
            return true;
    return false;
}

void SpellAuraHolder::UnregisterAndCleanupTrackedAuras(uint32 auraFlags)
{
    TrackedAuraType trackedType = GetTrackedAuraType();
    if (trackedType == TRACK_AURA_TYPE_NOT_TRACKED)
        return;

    if (trackedType == TRACK_AURA_TYPE_SINGLE_TARGET)
    {
        if (Unit* caster = GetCaster())
            caster->GetTrackedAuraTargets(trackedType).erase(GetSpellProto());
    }
    else if (trackedType == TRACK_AURA_TYPE_CONTROL_VEHICLE)
    {
        Unit* caster = GetCaster();
        if (caster && IsSpellHaveAura(GetSpellProto(), SPELL_AURA_CONTROL_VEHICLE, auraFlags))
        {
            caster->GetTrackedAuraTargets(trackedType).erase(GetSpellProto());

            if (SpellAuraHolder* holder = caster->GetSpellAuraHolder(GetSpellProto()->Id))
            {
                // remove the tracked aura type(TRACK_AURA_TYPE_CONTROL_VEHICLE) to avoid getting back here
                holder->SetTrackedAuraType(TRACK_AURA_TYPE_NOT_TRACKED);
                caster->RemoveSpellAuraHolder(holder);
            }
        }
        else if (caster)
        {
            Unit::TrackedAuraTargetMap scTarget = caster->GetTrackedAuraTargets(trackedType);
            Unit::TrackedAuraTargetMap::iterator find = scTarget.find(GetSpellProto());
            if (find != scTarget.end())
            {
                ObjectGuid vehicleGuid = find->second;
                scTarget.erase(find);
                if (Unit* vehicle = caster->GetMap()->GetUnit(vehicleGuid))
                {
                    vehicle->RemoveAurasByCasterSpell(GetSpellProto()->Id, caster->GetObjectGuid());
                    ObjectGuid const& casterGuid = caster->GetObjectGuid();
                    Unit::SpellAuraHolderBounds spair = vehicle->GetSpellAuraHolderBounds(GetSpellProto()->Id);
                    for (Unit::SpellAuraHolderMap::iterator iter = spair.first; iter != spair.second;)
                    {
                        if (iter->second->GetCasterGuid() == casterGuid)
                        {
                            // remove the tracked aura type(TRACK_AURA_TYPE_CONTROL_VEHICLE) to avoid getting back here
                            iter->second->SetTrackedAuraType(TRACK_AURA_TYPE_NOT_TRACKED);
                            vehicle->RemoveSpellAuraHolder(iter->second);
                            break;
                        }
                        else
                            ++iter;
                    }
                }
            }
        }
    }

    m_trackedAuraType = TRACK_AURA_TYPE_NOT_TRACKED;
}

void SpellAuraHolder::SetCreationDelayFlag()
{
    m_skipUpdate = true;
}

bool SpellAuraHolder::IsProcReady(TimePoint const& now) const
{
    return m_procCooldown < now;
}

void SpellAuraHolder::SetProcCooldown(std::chrono::milliseconds cooldown, TimePoint const& now)
{
    m_procCooldown = now + cooldown;
}

void SpellAuraHolder::SetHeartbeatResist(uint32 chance, int32 originalDuration, uint32 drLevel)
{
    // NOTE: This is an experimental approximation of heartbeat resist mechanics, more research is required
    // Main points in common cited by independent sources:
    // * Break attempts become more frequent as hit count rises
    // * Break chance becomes higher as hit count rises
    m_heartbeatResistChance = (0.01f * chance * (1 + drLevel));
    m_heartbeatResistInterval = std::max(1000, int32(uint32(originalDuration) / (2 + drLevel)));
    m_heartbeatResistTimer = m_heartbeatResistInterval;
}

void SpellAuraHolder::UpdateHeartbeatResist(uint32 diff)
{
    if (m_heartbeatResistChance == 0.0f || !m_heartbeatResistInterval || m_heartbeatResistTimer <= 0)
        return;

    m_heartbeatResistTimer -= diff;

    while (m_heartbeatResistTimer < 0)
    {
        m_heartbeatResistTimer += m_heartbeatResistInterval;

        DEBUG_FILTER_LOG(LOG_FILTER_COMBAT, "UpdateHeartbeatResist: Update tick for spell %u with %i ms interval", m_spellProto->Id, m_heartbeatResistInterval);

        const bool resist = roll_chance_f(m_heartbeatResistChance);

        DEBUG_FILTER_LOG(LOG_FILTER_COMBAT, "UpdateHeartbeatResist: Result: %s (chance %.2f)", (resist ? "RESIST" : "HIT"), double(m_heartbeatResistChance));

        if (resist)
        {
            if (Unit* target = GetTarget())
            {
                target->RemoveSpellAuraHolder(this, AURA_REMOVE_BY_CANCEL);
                return;
            }
        }
    }
}

GameObjectAura::GameObjectAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 const* currentDamage, int32 const* currentBasePoints, SpellAuraHolder* holder, Unit* target, GameObject* /*caster*/)
    : Aura(spellproto, eff, currentDamage, currentBasePoints, holder, target, nullptr)
{
    m_isAreaAura = true;
}

GameObjectAura::~GameObjectAura()
{    
}

void GameObjectAura::Update(uint32 diff)
{
    Aura::Update(diff);
    Unit* target = GetTarget();
    GameObject* goCaster = target->GetMap()->GetGameObject(GetHolder()->GetCasterGuid());
    if (!goCaster)
        target->RemoveSpellAuraHolder(GetHolder());
}

void SpellAuraHolder::OnHolderInit(WorldObject* caster)
{
    if (AuraScript* script = GetAuraScript())
        script->OnHolderInit(this, caster);
}

void SpellAuraHolder::OnDispel(Unit* dispeller, uint32 dispellingSpellId, uint32 originalStacks)
{
    if (AuraScript* script = GetAuraScript())
        script->OnDispel(this, dispeller, dispellingSpellId, originalStacks);
}

uint32 Aura::CalculateAuraEffectValue(Unit* caster, Unit* /*target*/, SpellEntry const* spellProto, SpellEffectIndex effIdx, uint32 value)
{
    switch (spellProto->EffectApplyAuraName[effIdx])
    {
        case SPELL_AURA_SCHOOL_ABSORB:
        {
            float DoneActualBenefit = 0.0f;
            if (SpellBonusEntry const* bonus = sSpellMgr.GetSpellBonusData(spellProto->Id))
            {
                DoneActualBenefit = caster->SpellBaseHealingBonusDone(GetSpellSchoolMask(spellProto)) * bonus->direct_damage;
                DoneActualBenefit *= caster->CalculateLevelPenalty(spellProto);

                value += (int32)DoneActualBenefit;
            }
            return value;
        }
    }
    return value;
}

void Aura::OnAuraInit()
{
    if (AuraScript* script = GetAuraScript())
        script->OnAuraInit(this);
}

int32 Aura::OnAuraValueCalculate(Unit* caster, int32 currentValue)
{
    if (AuraScript* script = GetAuraScript())
    {
        AuraCalcData data(this, caster, GetTarget(), GetSpellProto(), GetEffIndex());
        return script->OnAuraValueCalculate(data, currentValue);
    }
    return currentValue;
}

void Aura::OnDamageCalculate(Unit* victim, int32& advertisedBenefit, float& totalMod)
{
    if (AuraScript* script = GetAuraScript())
        return script->OnDamageCalculate(this, victim, advertisedBenefit, totalMod);
}

void Aura::OnApply(bool apply)
{
    if (AuraScript* script = GetAuraScript())
        script->OnApply(this, apply);
}

void Aura::OnAfterApply(bool apply)
{
    if (AuraScript* script = GetAuraScript())
        script->OnAfterApply(this, apply);
}

bool Aura::OnCheckProc(ProcExecutionData& data)
{
    if (AuraScript* script = GetAuraScript())
        return script->OnCheckProc(this, data);
    return true;
}

SpellAuraProcResult Aura::OnProc(ProcExecutionData& data)
{
    if (AuraScript* script = GetAuraScript())
        return script->OnProc(this, data);
    return SPELL_AURA_PROC_OK;
}

void Aura::OnAbsorb(int32& currentAbsorb, int32& remainingDamage, uint32& reflectedSpellId, int32& reflectDamage, bool& preventedDeath, bool& dropCharge)
{
    if (AuraScript* script = GetAuraScript())
        script->OnAbsorb(this, currentAbsorb, remainingDamage, reflectedSpellId, reflectDamage, preventedDeath, dropCharge);
}

void Aura::OnManaAbsorb(int32& currentAbsorb)
{
    if (AuraScript* script = GetAuraScript())
        script->OnManaAbsorb(this, currentAbsorb);
}

void Aura::OnAuraDeathPrevention(int32& remainingDamage)
{
    if (AuraScript* script = GetAuraScript())
        script->OnAuraDeathPrevention(this, remainingDamage);
}

void Aura::OnPeriodicCalculateAmount(uint32& amount)
{
    if (AuraScript* script = GetAuraScript())
        script->OnPeriodicCalculateAmount(this, amount);
}

void Aura::OnHeartbeat()
{
    // TODO: move HB resist here
    if (AuraScript* script = GetAuraScript())
        script->OnHeartbeat(this);
}

uint32 Aura::GetAuraScriptCustomizationValue()
{
    if (AuraScript* script = GetAuraScript())
       return script->GetAuraScriptCustomizationValue(this);
    return 0;
}

void Aura::ForcePeriodicity(uint32 periodicTime)
{
    if (periodicTime == 0)
        m_isPeriodic = false;
    else
        m_isPeriodic = true;
    m_modifier.periodictime = periodicTime;
    m_periodicTimer = periodicTime;
}

void Aura::OnPeriodicTrigger(PeriodicTriggerData& data)
{
    if (AuraScript* script = GetAuraScript())
        script->OnPeriodicTrigger(this, data);
}

void Aura::OnPeriodicDummy()
{
    if (AuraScript* script = GetAuraScript())
        script->OnPeriodicDummy(this);
}

void Aura::OnPeriodicTickEnd()
{
    if (AuraScript* script = GetAuraScript())
        script->OnPeriodicTickEnd(this);
}
