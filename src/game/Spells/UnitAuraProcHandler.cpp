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
#include "Log.h"
#include "Globals/ObjectMgr.h"
#include "Spells/SpellMgr.h"
#include "Entities/Player.h"
#include "Entities/Unit.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuras.h"
#include "Entities/Totem.h"
#include "Entities/Creature.h"
#include "Util.h"

pAuraProcHandler AuraProcHandler[TOTAL_AURAS] =
{
    &Unit::HandleNULLProc,                                  //  0 SPELL_AURA_NONE
    &Unit::HandleNULLProc,                                  //  1 SPELL_AURA_BIND_SIGHT
    &Unit::HandleNULLProc,                                  //  2 SPELL_AURA_MOD_POSSESS
    &Unit::HandlePeriodicAuraProc,                          //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Unit::HandleDummyAuraProc,                             //  4 SPELL_AURA_DUMMY
    &Unit::HandleNULLProc,                                  //  5 SPELL_AURA_MOD_CONFUSE
    &Unit::HandleNULLProc,                                  //  6 SPELL_AURA_MOD_CHARM
    &Unit::HandleRemoveByDamageChanceProc,                  //  7 SPELL_AURA_MOD_FEAR
    &Unit::HandleNULLProc,                                  //  8 SPELL_AURA_PERIODIC_HEAL
    &Unit::HandleNULLProc,                                  //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Unit::HandleNULLProc,                                  // 10 SPELL_AURA_MOD_THREAT
    &Unit::HandleNULLProc,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Unit::HandleNULLProc,                                  // 12 SPELL_AURA_MOD_STUN
    &Unit::HandleNULLProc,                                  // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Unit::HandleNULLProc,                                  // 14 SPELL_AURA_MOD_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  // 15 SPELL_AURA_DAMAGE_SHIELD
    &Unit::HandleNULLProc,                                  // 16 SPELL_AURA_MOD_STEALTH
    &Unit::HandleNULLProc,                                  // 17 SPELL_AURA_MOD_STEALTH_DETECT
    &Unit::HandleInvisibilityAuraProc,                      // 18 SPELL_AURA_MOD_INVISIBILITY
    &Unit::HandleNULLProc,                                  // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Unit::HandleNULLProc,                                  // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Unit::HandleNULLProc,                                  // 21 SPELL_AURA_OBS_MOD_MANA
    &Unit::HandleModResistanceAuraProc,                     // 22 SPELL_AURA_MOD_RESISTANCE
    &Unit::HandleNULLProc,                                  // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Unit::HandleNULLProc,                                  // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Unit::HandleNULLProc,                                  // 25 SPELL_AURA_MOD_PACIFY
    &Unit::HandleRemoveByDamageChanceProc,                  // 26 SPELL_AURA_MOD_ROOT
    &Unit::HandleNULLProc,                                  // 27 SPELL_AURA_MOD_SILENCE
    &Unit::HandleNULLProc,                                  // 28 SPELL_AURA_REFLECT_SPELLS
    &Unit::HandleNULLProc,                                  // 29 SPELL_AURA_MOD_STAT
    &Unit::HandleNULLProc,                                  // 30 SPELL_AURA_MOD_SKILL
    &Unit::HandleNULLProc,                                  // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Unit::HandleNULLProc,                                  // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Unit::HandleNULLProc,                                  // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Unit::HandleNULLProc,                                  // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Unit::HandleNULLProc,                                  // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Unit::HandleNULLProc,                                  // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Unit::HandleNULLProc,                                  // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Unit::HandleNULLProc,                                  // 38 SPELL_AURA_STATE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Unit::HandleNULLProc,                                  // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Unit::HandleProcTriggerSpellAuraProc,                  // 42 SPELL_AURA_PROC_TRIGGER_SPELL
    &Unit::HandleProcTriggerDamageAuraProc,                 // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE
    &Unit::HandleNULLProc,                                  // 44 SPELL_AURA_TRACK_CREATURES
    &Unit::HandleNULLProc,                                  // 45 SPELL_AURA_TRACK_RESOURCES
    &Unit::HandleNULLProc,                                  // 46 SPELL_AURA_46 (used in test spells 54054 and 54058, and spell 48050) (3.0.8a-3.2.2a)
    &Unit::HandleNULLProc,                                  // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Unit::HandleNULLProc,                                  // 48 SPELL_AURA_PERIODIC_TRIGGER_BY_CLIENT
    &Unit::HandleNULLProc,                                  // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Unit::HandleNULLProc,                                  // 50 SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT
    &Unit::HandleNULLProc,                                  // 51 SPELL_AURA_MOD_BLOCK_PERCENT
    &Unit::HandleNULLProc,                                  // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Unit::HandleNULLProc,                                  // 53 SPELL_AURA_PERIODIC_LEECH
    &Unit::HandleNULLProc,                                  // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Unit::HandleNULLProc,                                  // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Unit::HandleNULLProc,                                  // 56 SPELL_AURA_TRANSFORM
    &Unit::HandleSpellCritChanceAuraProc,                   // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Unit::HandleNULLProc,                                  // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE
    &Unit::HandleRemoveByDamageChanceProc,                  // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Unit::HandleNULLProc,                                  // 61 SPELL_AURA_MOD_SCALE
    &Unit::HandleNULLProc,                                  // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Unit::HandleNULLProc,                                  // 63 unused (3.0.8a-3.2.2a) old SPELL_AURA_PERIODIC_MANA_FUNNEL
    &Unit::HandleNULLProc,                                  // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Unit::HandleModCastingSpeedNotStackAuraProc,           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  // 66 SPELL_AURA_FEIGN_DEATH
    &Unit::HandleNULLProc,                                  // 67 SPELL_AURA_MOD_DISARM
    &Unit::HandleNULLProc,                                  // 68 SPELL_AURA_MOD_STALKED
    &Unit::HandleNULLProc,                                  // 69 SPELL_AURA_SCHOOL_ABSORB
    &Unit::HandleNULLProc,                                  // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell 41560 that has only visual effect (3.2.2a)
    &Unit::HandleNULLProc,                                  // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Unit::HandleModPowerCostSchoolAuraProc,                // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Unit::HandleModPowerCostSchoolAuraProc,                // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Unit::HandleReflectSpellsSchoolAuraProc,               // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL
    &Unit::HandleNULLProc,                                  // 75 SPELL_AURA_MOD_LANGUAGE
    &Unit::HandleNULLProc,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Unit::HandleNULLProc,                                  // 78 SPELL_AURA_MOUNTED
    &Unit::HandleModDamagePercentDoneAuraProc,              // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Unit::HandleNULLProc,                                  // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Unit::HandleNULLProc,                                  // 81 SPELL_AURA_SPLIT_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  // 82 SPELL_AURA_WATER_BREATHING
    &Unit::HandleNULLProc,                                  // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Unit::HandleNULLProc,                                  // 84 SPELL_AURA_MOD_REGEN
    &Unit::HandleCantTrigger,                               // 85 SPELL_AURA_MOD_POWER_REGEN
    &Unit::HandleNULLProc,                                  // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Unit::HandleNULLProc,                                  // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN
    &Unit::HandleNULLProc,                                  // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT
    &Unit::HandleNULLProc,                                  // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Unit::HandleNULLProc,                                  // 90 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_RESIST_CHANCE
    &Unit::HandleNULLProc,                                  // 91 SPELL_AURA_MOD_DETECT_RANGE
    &Unit::HandleNULLProc,                                  // 92 SPELL_AURA_PREVENTS_FLEEING
    &Unit::HandleNULLProc,                                  // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Unit::HandleNULLProc,                                  // 94 SPELL_AURA_INTERRUPT_REGEN
    &Unit::HandleNULLProc,                                  // 95 SPELL_AURA_GHOST
    &Unit::HandleMagnetAuraProc,                            // 96 SPELL_AURA_SPELL_MAGNET
    &Unit::HandleManaShieldAuraProc,                        // 97 SPELL_AURA_MANA_SHIELD
    &Unit::HandleNULLProc,                                  // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Unit::HandleNULLProc,                                  // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //100 SPELL_AURA_AURAS_VISIBLE obsolete 3.x? all player can see all auras now, but still have 2 spells including GM-spell (1852,2855)
    &Unit::HandleNULLProc,                                  //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Unit::HandleNULLProc,                                  //104 SPELL_AURA_WATER_WALK
    &Unit::HandleNULLProc,                                  //105 SPELL_AURA_FEATHER_FALL
    &Unit::HandleNULLProc,                                  //106 SPELL_AURA_HOVER
    &Unit::HandleAddFlatModifierAuraProc,                   //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Unit::HandleAddPctModifierAuraProc,                    //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Unit::HandleNULLProc,                                  //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Unit::HandleNULLProc,                                  //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Unit::HandleMagnetAuraProc,                            //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER
    &Unit::HandleOverrideClassScriptAuraProc,               //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS
    &Unit::HandleNULLProc,                                  //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //115 SPELL_AURA_MOD_HEALING
    &Unit::HandleNULLProc,                                  //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE
    &Unit::HandleNULLProc,                                  //118 SPELL_AURA_MOD_HEALING_PCT
    &Unit::HandleNULLProc,                                  //119 unused (3.0.8a-3.2.2a) old SPELL_AURA_SHARE_PET_TRACKING
    &Unit::HandleNULLProc,                                  //120 SPELL_AURA_UNTRACKABLE
    &Unit::HandleNULLProc,                                  //121 SPELL_AURA_EMPATHY
    &Unit::HandleNULLProc,                                  //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Unit::HandleNULLProc,                                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //128 SPELL_AURA_MOD_POSSESS_PET
    &Unit::HandleNULLProc,                                  //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Unit::HandleNULLProc,                                  //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Unit::HandleNULLProc,                                  //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Unit::HandleNULLProc,                                  //135 SPELL_AURA_MOD_HEALING_DONE
    &Unit::HandleNULLProc,                                  //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT
    &Unit::HandleNULLProc,                                  //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Unit::HandleHasteAuraProc,                             //138 SPELL_AURA_MOD_MELEE_HASTE
    &Unit::HandleNULLProc,                                  //139 SPELL_AURA_FORCE_REACTION
    &Unit::HandleNULLProc,                                  //140 SPELL_AURA_MOD_RANGED_HASTE
    &Unit::HandleNULLProc,                                  //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Unit::HandleNULLProc,                                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Unit::HandleNULLProc,                                  //144 SPELL_AURA_SAFE_FALL
    &Unit::HandleNULLProc,                                  //145 SPELL_AURA_MOD_PET_TALENT_POINTS
    &Unit::HandleNULLProc,                                  //146 SPELL_AURA_ALLOW_TAME_PET_TYPE
    &Unit::HandleNULLProc,                                  //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK
    &Unit::HandleNULLProc,                                  //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Unit::HandleCantTrigger,                               //149 SPELL_AURA_REDUCE_PUSHBACK
    &Unit::HandleNULLProc,                                  //150 SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT
    &Unit::HandleNULLProc,                                  //151 SPELL_AURA_TRACK_STEALTHED
    &Unit::HandleNULLProc,                                  //152 SPELL_AURA_MOD_DETECTED_RANGE
    &Unit::HandleNULLProc,                                  //153 SPELL_AURA_SPLIT_DAMAGE_FLAT
    &Unit::HandleNULLProc,                                  //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &Unit::HandleNULLProc,                                  //155 SPELL_AURA_MOD_WATER_BREATHING
    &Unit::HandleNULLProc,                                  //156 SPELL_AURA_MOD_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //157 SPELL_AURA_PET_DAMAGE_MULTI (single test like spell 20782, also single for 214 aura)
    &Unit::HandleNULLProc,                                  //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Unit::HandleNULLProc,                                  //159 SPELL_AURA_NO_PVP_CREDIT
    &Unit::HandleNULLProc,                                  //160 SPELL_AURA_MOD_AOE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT
    &Unit::HandleNULLProc,                                  //162 SPELL_AURA_POWER_BURN_MANA
    &Unit::HandleNULLProc,                                  //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
    &Unit::HandleNULLProc,                                  //164 unused (3.0.8a-3.2.2a), only one test spell 10654
    &Unit::HandleNULLProc,                                  //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS
    &Unit::HandleNULLProc,                                  //169 SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
    &Unit::HandleNULLProc,                                  //170 SPELL_AURA_DETECT_AMORE       different spells that ignore transformation effects
    &Unit::HandleNULLProc,                                  //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //173 unused (3.0.8a-3.2.2a) no spells, old SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Unit::HandleNULLProc,                                  //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Unit::HandleNULLProc,                                  //177 SPELL_AURA_AOE_CHARM (22 spells)
    &Unit::HandleNULLProc,                                  //178 SPELL_AURA_MOD_DEBUFF_RESISTANCE
    &Unit::HandleNULLProc,                                  //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS
    &Unit::HandleNULLProc,                                  //181 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS
    &Unit::HandleNULLProc,                                  //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //183 SPELL_AURA_MOD_CRITICAL_THREAT only used in 28746
    &Unit::HandleNULLProc,                                  //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE
    &Unit::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &Unit::HandleNULLProc,                                  //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Unit::HandleNULLProc,                                  //192 SPELL_AURA_HASTE_MELEE
    &Unit::HandleNULLProc,                                  //193 SPELL_AURA_HASTE_ALL (in fact combat (any type attack) speed pct)
    &Unit::HandleNULLProc,                                  //194 SPELL_AURA_MOD_IGNORE_ABSORB_SCHOOL
    &Unit::HandleNULLProc,                                  //195 SPELL_AURA_MOD_IGNORE_ABSORB_FOR_SPELL
    &Unit::HandleNULLProc,                                  //196 SPELL_AURA_MOD_COOLDOWN (single spell 24818 in 3.2.2a)
    &Unit::HandleNULLProc,                                  //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCEe
    &Unit::HandleNULLProc,                                  //198 unused (3.0.8a-3.2.2a) old SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &Unit::HandleNULLProc,                                  //199 SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT
    &Unit::HandleNULLProc,                                  //200 SPELL_AURA_MOD_KILL_XP_PCT
    &Unit::HandleNULLProc,                                  //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &Unit::HandleNULLProc,                                  //202 SPELL_AURA_CANNOT_BE_DODGED
    &Unit::HandleNULLProc,                                  //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //205 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //206 SPELL_AURA_MOD_FLIGHT_SPEED
    &Unit::HandleNULLProc,                                  //207 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED
    &Unit::HandleNULLProc,                                  //208 SPELL_AURA_MOD_FLIGHT_SPEED_STACKING
    &Unit::HandleNULLProc,                                  //209 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING
    &Unit::HandleNULLProc,                                  //210 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING
    &Unit::HandleNULLProc,                                  //211 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING
    &Unit::HandleNULLProc,                                  //212 SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &Unit::HandleNULLProc,                                  //214 Tamed Pet Passive (single test like spell 20782, also single for 157 aura)
    &Unit::HandleNULLProc,                                  //215 SPELL_AURA_ARENA_PREPARATION
    &Unit::HandleNULLProc,                                  //216 SPELL_AURA_HASTE_SPELLS
    &Unit::HandleNULLProc,                                  //217 unused (3.0.8a-3.2.2a)
    &Unit::HandleNULLProc,                                  //218 SPELL_AURA_HASTE_RANGED
    &Unit::HandleNULLProc,                                  //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &Unit::HandleNULLProc,                                  //220 SPELL_AURA_MOD_RATING_FROM_STAT
    &Unit::HandleNULLProc,                                  //221 ignored
    &Unit::HandleNULLProc,                                  //222 unused (3.0.8a-3.2.2a) only for spell 44586 that not used in real spell cast
    &Unit::HandleRaidProcFromChargeAuraProc,                //223 SPELL_AURA_RAID_PROC_FROM_CHARGE
    &Unit::HandleNULLProc,                                  //224 unused (3.0.8a-3.2.2a)
    &Unit::HandleRaidProcFromChargeWithValueAuraProc,       //225 SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE
    &Unit::HandleNULLProc,                                  //226 SPELL_AURA_PERIODIC_DUMMY
    &Unit::HandleNULLProc,                                  //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE
    &Unit::HandleNULLProc,                                  //228 SPELL_AURA_DETECT_STEALTH
    &Unit::HandleNULLProc,                                  //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //230 Commanding Shout
    &Unit::HandleProcTriggerSpellAuraProc,                  //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &Unit::HandleNULLProc,                                  //232 SPELL_AURA_MECHANIC_DURATION_MOD
    &Unit::HandleNULLProc,                                  //233 set model id to the one of the creature with id m_modifier.m_miscvalue
    &Unit::HandleNULLProc,                                  //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK
    &Unit::HandleNULLProc,                                  //235 SPELL_AURA_MOD_DISPEL_RESIST
    &Unit::HandleNULLProc,                                  //236 SPELL_AURA_CONTROL_VEHICLE
    &Unit::HandleNULLProc,                                  //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &Unit::HandleNULLProc,                                  //240 SPELL_AURA_MOD_EXPERTISE
    &Unit::HandleNULLProc,                                  //241 Forces the player to move forward
    &Unit::HandleNULLProc,                                  //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING (only 2 test spels in 3.2.2a)
    &Unit::HandleNULLProc,                                  //243 faction reaction override spells
    &Unit::HandleNULLProc,                                  //244 SPELL_AURA_COMPREHEND_LANGUAGE
    &Unit::HandleNULLProc,                                  //245 SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS
    &Unit::HandleNULLProc,                                  //246 SPELL_AURA_MOD_DURATION_OF_EFFECTS_BY_DISPEL
    &Unit::HandleNULLProc,                                  //247 target to become a clone of the caster
    &Unit::HandleNULLProc,                                  //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
    &Unit::HandleNULLProc,                                  //249 SPELL_AURA_CONVERT_RUNE
    &Unit::HandleNULLProc,                                  //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &Unit::HandleNULLProc,                                  //251 SPELL_AURA_MOD_ENEMY_DODGE
    &Unit::HandleNULLProc,                                  //252 SPELL_AURA_SLOW_ALL
    &Unit::HandleNULLProc,                                  //253 SPELL_AURA_MOD_BLOCK_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //254 SPELL_AURA_MOD_DISARM_SHIELD disarm Shield
    &Unit::HandleNULLProc,                                  //255 SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT
    &Unit::HandleNULLProc,                                  //256 SPELL_AURA_NO_REAGENT_USE Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //257 SPELL_AURA_MOD_TARGET_RESIST_BY_SPELL_CLASS Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //258 SPELL_AURA_MOD_SPELL_VISUAL
    &Unit::HandleNULLProc,                                  //259 corrupt healing over time spell
    &Unit::HandleNULLProc,                                  //260 SPELL_AURA_SCREEN_EFFECT (miscvalue = id in ScreenEffect.dbc) not required any code
    &Unit::HandleNULLProc,                                  //261 SPELL_AURA_PHASE undetectable invisibility?
    &Unit::HandleNULLProc,                                  //262 SPELL_AURA_IGNORE_UNIT_STATE
    &Unit::HandleNULLProc,                                  //263 SPELL_AURA_ALLOW_ONLY_ABILITY player can use only abilities set in SpellClassMask
    &Unit::HandleNULLProc,                                  //264 unused (3.0.8a-3.2.2a)
    &Unit::HandleNULLProc,                                  //265 unused (3.0.8a-3.2.2a)
    &Unit::HandleNULLProc,                                  //266 unused (3.0.8a-3.2.2a)
    &Unit::HandleNULLProc,                                  //267 SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL
    &Unit::HandleNULLProc,                                  //268 SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //269 SPELL_AURA_MOD_IGNORE_DAMAGE_REDUCTION_SCHOOL
    &Unit::HandleNULLProc,                                  //270 SPELL_AURA_MOD_IGNORE_TARGET_RESIST (unused in 3.2.2a)
    &Unit::HandleModDamageFromCasterAuraProc,               //271 SPELL_AURA_MOD_DAMAGE_FROM_CASTER
    &Unit::HandleNULLProc,                                  //272 SPELL_AURA_MAELSTROM_WEAPON (unclear use for aura, it used in (3.2.2a...3.3.0) in single spell 53817 that spellmode stacked and charged spell expected to be drop as stack
    &Unit::HandleNULLProc,                                  //273 SPELL_AURA_X_RAY (client side implementation)
    &Unit::HandleNULLProc,                                  //274 proc free shot?
    &Unit::HandleNULLProc,                                  //275 SPELL_AURA_MOD_IGNORE_SHAPESHIFT Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //276 mod damage % mechanic?
    &Unit::HandleNULLProc,                                  //277 SPELL_AURA_MOD_MAX_AFFECTED_TARGETS Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //278 SPELL_AURA_MOD_DISARM_RANGED disarm ranged weapon
    &Unit::HandleNULLProc,                                  //279 visual effects? 58836 and 57507
    &Unit::HandleNULLProc,                                  //280 SPELL_AURA_MOD_TARGET_ARMOR_PCT
    &Unit::HandleNULLProc,                                  //281 SPELL_AURA_MOD_HONOR_GAIN
    &Unit::HandleNULLProc,                                  //282 SPELL_AURA_INCREASE_BASE_HEALTH_PERCENT
    &Unit::HandleNULLProc,                                  //283 SPELL_AURA_MOD_HEALING_RECEIVED
    &Unit::HandleNULLProc,                                  //284 51 spells
    &Unit::HandleNULLProc,                                  //285 SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    &Unit::HandleNULLProc,                                  //286 SPELL_AURA_ABILITY_PERIODIC_CRIT
    &Unit::HandleNULLProc,                                  //287 SPELL_AURA_DEFLECT_SPELLS
    &Unit::HandleNULLProc,                                  //288 increase parry/deflect, prevent attack (single spell used 67801)
    &Unit::HandleNULLProc,                                  //289 unused (3.2.2a)
    &Unit::HandleNULLProc,                                  //290 SPELL_AURA_MOD_ALL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //291 SPELL_AURA_MOD_QUEST_XP_PCT
    &Unit::HandleNULLProc,                                  //292 call stabled pet
    &Unit::HandleNULLProc,                                  //293 3 spells
    &Unit::HandleNULLProc,                                  //294 2 spells, possible prevent mana regen
    &Unit::HandleNULLProc,                                  //295 unused (3.2.2a)
    &Unit::HandleNULLProc,                                  //296 2 spells
    &Unit::HandleNULLProc,                                  //297 1 spell (counter spell school?)
    &Unit::HandleNULLProc,                                  //298 unused (3.2.2a)
    &Unit::HandleNULLProc,                                  //299 unused (3.2.2a)
    &Unit::HandleNULLProc,                                  //300 3 spells (share damage?)
    &Unit::HandleNULLProc,                                  //301 5 spells
    &Unit::HandleNULLProc,                                  //302 unused (3.2.2a)
    &Unit::HandleNULLProc,                                  //303 17 spells
    &Unit::HandleNULLProc,                                  //304 2 spells (alcohol effect?)
    &Unit::HandleNULLProc,                                  //305 SPELL_AURA_MOD_MINIMUM_SPEED
    &Unit::HandleNULLProc,                                  //306 1 spell
    &Unit::HandleNULLProc,                                  //307 absorb healing?
    &Unit::HandleNULLProc,                                  //308 new aura for hunter traps
    &Unit::HandleNULLProc,                                  //309 absorb healing?
    &Unit::HandleNULLProc,                                  //310 pet avoidance passive?
    &Unit::HandleNULLProc,                                  //311 0 spells in 3.3
    &Unit::HandleNULLProc,                                  //312 0 spells in 3.3
    &Unit::HandleNULLProc,                                  //313 0 spells in 3.3
    &Unit::HandleNULLProc,                                  //314 1 test spell (reduce duration of silince/magic)
    &Unit::HandleNULLProc,                                  //315 underwater walking
    &Unit::HandleNULLProc                                   //316 makes haste affect HOT/DOT ticks
};

struct ProcTriggeredData
{
    ProcTriggeredData(SpellProcEventEntry const* _spellProcEvent, SpellAuraHolder* _triggeredByHolder)
        : spellProcEvent(_spellProcEvent), triggeredByHolder(_triggeredByHolder)
    {}
    SpellProcEventEntry const* spellProcEvent;
    SpellAuraHolder* triggeredByHolder;
};

typedef std::list< ProcTriggeredData > ProcTriggeredList;

uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition)
{
    uint32 procEx = PROC_EX_NONE;
    // Check victim state
    if (missCondition != SPELL_MISS_NONE)
        switch (missCondition)
        {
            case SPELL_MISS_MISS:    procEx |= PROC_EX_MISS;   break;
            case SPELL_MISS_RESIST:  procEx |= PROC_EX_RESIST; break;
            case SPELL_MISS_DODGE:   procEx |= PROC_EX_DODGE;  break;
            case SPELL_MISS_PARRY:   procEx |= PROC_EX_PARRY;  break;
            case SPELL_MISS_BLOCK:   procEx |= PROC_EX_BLOCK;  break;
            case SPELL_MISS_EVADE:   procEx |= PROC_EX_EVADE;  break;
            case SPELL_MISS_IMMUNE:  procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_IMMUNE2: procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_DEFLECT: procEx |= PROC_EX_DEFLECT; break;
            case SPELL_MISS_ABSORB:  procEx |= PROC_EX_ABSORB; break;
            case SPELL_MISS_REFLECT: procEx |= PROC_EX_REFLECT; break;
            default:
                break;
        }
    else
    {
        // On block
        if (damageInfo->blocked)
            procEx |= PROC_EX_BLOCK;
        // On absorb
        if (damageInfo->absorb)
            procEx |= PROC_EX_ABSORB;
        // On crit
        if (damageInfo->HitInfo & SPELL_HIT_TYPE_CRIT)
            procEx |= PROC_EX_CRITICAL_HIT;
        else
            procEx |= PROC_EX_NORMAL_HIT;
    }
    return procEx;
}

void Unit::ProcSkillsAndReactives(bool isVictim, Unit* target, uint32 procFlags, uint32 procEx, WeaponAttackType attType)
{
    // For melee/ranged based attack need update skills and set some Aura states
    if (procFlags & MELEE_BASED_TRIGGER_MASK)
    {
        // Update skills here for players
        if (GetTypeId() == TYPEID_PLAYER)
        {
            // On melee based hit/miss/resist need update skill (for victim and attacker)
            if (procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT | PROC_EX_MISS | PROC_EX_DODGE | PROC_EX_PARRY | PROC_EX_BLOCK))
            {
                if (target->GetTypeId() != TYPEID_PLAYER && target->GetCreatureType() != CREATURE_TYPE_CRITTER)
                    ((Player*)this)->UpdateCombatSkills(target, attType, isVictim);
            }
            // Update defence if player is victim and parry/dodge/block
            if (isVictim && procEx & (PROC_EX_DODGE | PROC_EX_PARRY | PROC_EX_BLOCK))
                ((Player*)this)->UpdateDefense();
        }
        // If exist crit/parry/dodge/block need update aura state (for victim and attacker)
        if (procEx & (PROC_EX_CRITICAL_HIT | PROC_EX_PARRY | PROC_EX_DODGE | PROC_EX_BLOCK))
        {
            // for victim
            if (isVictim)
            {
                // if victim and dodge attack
                if (procEx & PROC_EX_DODGE)
                {
                    // Update AURA_STATE on dodge
                    if (getClass() != CLASS_ROGUE) // skip Rogue Riposte
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if victim and parry attack
                if (procEx & PROC_EX_PARRY)
                {
                    // For Hunters only Counterattack (skip Mongoose bite)
                    if (getClass() == CLASS_HUNTER)
                    {
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, true);
                        StartReactiveTimer(REACTIVE_HUNTER_PARRY);
                    }
                    else
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if and victim block attack
                if (procEx & PROC_EX_BLOCK)
                {
                    ModifyAuraState(AURA_STATE_DEFENSE, true);
                    StartReactiveTimer(REACTIVE_DEFENSE);
                }
            }
            else // For attacker
            {
                // Overpower on victim dodge
                if (procEx & PROC_EX_DODGE && getClass() == CLASS_WARRIOR)
                {
                    AddComboPoints(target, 1);
                    StartReactiveTimer(REACTIVE_OVERPOWER);
                }
            }
        }
    }
}

void Unit::ProcDamageAndSpell(ProcSystemArguments&& data)
{
    // First lets get skills and reactives out of the way
    bool currentLevel = true;
    if (data.attacker)
    {
        if (data.attacker->m_spellProcsHappening)
            currentLevel = false; // triggered spell in proc system should not make holders ready
        data.attacker->m_spellProcsHappening = true;
        if (data.procFlagsAttacker)
            data.attacker->ProcSkillsAndReactives(false, data.victim, data.procFlagsAttacker, data.procExtra, data.attType);
    }
    bool canProcVictim = data.victim && data.victim->IsAlive() && data.procFlagsVictim;
    if (canProcVictim)
        data.victim->ProcSkillsAndReactives(true, data.attacker, data.procFlagsVictim, data.procExtra, data.attType);

    // Not much to do if no flags are set.
    if (data.attacker && data.procFlagsAttacker)
        data.attacker->ProcDamageAndSpellFor(data, false);

    // Now go on with a victim's events'n'auras
    // Not much to do if no flags are set or there is no victim
    if (canProcVictim)
        data.victim->ProcDamageAndSpellFor(data, true);

    if (data.attacker)
    {
        // trigger weapon enchants for weapon based spells; exclude spells that stop attack, because may break CC
		if (data.attacker->GetTypeId() == TYPEID_PLAYER && (data.procExtra & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) != 0)
            if ((data.procFlagsAttacker & PROC_FLAG_DEAL_HARMFUL_PERIODIC) == 0) // do not proc this on DOTs
			    if (!data.spellInfo || (data.spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && !data.spellInfo->HasAttribute(SPELL_ATTR_STOP_ATTACK_TARGET)))
				    static_cast<Player*>(data.attacker)->CastItemCombatSpell(data.victim, data.attType, data.spellInfo ? !IsNextMeleeSwingSpell(data.spellInfo) : false);

        if (currentLevel)
        {
            data.attacker->m_spellProcsHappening = false;

            // Mark auras created during proccing as ready
            for (SpellAuraHolder* holder : data.attacker->m_delayedSpellAuraHolders)
                if (holder->GetState() == SPELLAURAHOLDER_STATE_CREATED) // if deleted by some unknown circumstance
                    holder->SetState(SPELLAURAHOLDER_STATE_READY);

            data.attacker->m_delayedSpellAuraHolders.clear();
        }
    }
}

ProcExecutionData::ProcExecutionData(ProcSystemArguments& data, bool isVictim) : attacker(data.attacker), victim(data.victim),
    isVictim(isVictim), procExtra(data.procExtra), attType(data.attType), damage(data.damage), spellInfo(data.spellInfo), spell(data.spell), healthGain(data.healthGain), isHeal(data.isHeal),
    triggeredByAura(nullptr), cooldown(0), triggeredSpellId(0), procOnce(false), triggerTarget(nullptr)
{
    if (isVictim)
    {
        source = data.victim;
        target = data.attacker;
        procFlags = data.procFlagsVictim;
    }
    else
    {
        source = data.attacker;
        target = data.victim;
        procFlags = data.procFlagsAttacker;
    }
}

void Unit::ProcDamageAndSpellFor(ProcSystemArguments& argData, bool isVictim)
{
    ProcExecutionData execData(argData, isVictim);

    ProcTriggeredList procTriggered;
    std::vector<SpellAuraHolder*> removedHolders;
    // Fill procTriggered list
    for (SpellAuraHolderMap::const_iterator itr = GetSpellAuraHolderMap().begin(); itr != GetSpellAuraHolderMap().end(); ++itr)
    {
        // skip deleted auras (possible at recursive triggered call
        if (itr->second->GetState() != SPELLAURAHOLDER_STATE_READY || itr->second->IsDeleted())
            continue;

        SpellProcEventEntry const* spellProcEvent = nullptr;
        if (!IsTriggeredAtSpellProcEvent(execData, itr->second, spellProcEvent))
        {
            // spell seem not managed by proc system, although some case need to be handled

            // only process damage case on victim
            if (!isVictim || !(execData.procFlags & PROC_FLAG_TAKE_ANY_DAMAGE) || (execData.spellInfo && execData.spellInfo->HasAttribute(SPELL_ATTR_EX4_DAMAGE_DOESNT_BREAK_AURAS)))
                continue;

            const SpellEntry* se = itr->second->GetSpellProto();

            // check if the aura is interruptible by damage and if its not just added by this spell (spell who is responsible for this damage is spellInfo)
            if (se->AuraInterruptFlags & AURA_INTERRUPT_FLAG_DAMAGE && (!execData.spellInfo || execData.spellInfo->Id != se->Id))
            {
                DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "ProcDamageAndSpell: Added Spell %u to 'remove aura due to spell' list! Reason: Damage received.", se->Id);
                removedHolders.push_back(itr->second);
            }
            continue;
        }

        procTriggered.push_back(ProcTriggeredData(spellProcEvent, itr->second));
    }

    for (auto holder : removedHolders)
        if (!holder->IsDeleted())
            RemoveSpellAuraHolder(holder);

    // Nothing found
    if (procTriggered.empty())
        return;

    // Handle effects proceed this time
    for (ProcTriggeredList::const_iterator itr = procTriggered.begin(); itr != procTriggered.end(); ++itr)
    {
        // Some auras can be deleted in function called in this loop (except first, ofc)
        SpellAuraHolder* triggeredByHolder = itr->triggeredByHolder;
        if (triggeredByHolder->IsDeleted())
            continue;

        SpellProcEventEntry const* spellProcEvent = itr->spellProcEvent;
        bool useCharges = triggeredByHolder->GetAuraCharges() > 0;
        bool procSuccess = true;
        bool anyAuraProc = false;

        execData.cooldown = 0;
        if (spellProcEvent && spellProcEvent->cooldown)
            execData.cooldown = spellProcEvent->cooldown;

        for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            execData.triggeredByAura = triggeredByHolder->GetAuraByEffectIndex(SpellEffectIndex(i));
            if (!execData.triggeredByAura)
                continue;

            Modifier* auraModifier = execData.triggeredByAura->GetModifier();

            if (execData.spellInfo)
            {
                if (spellProcEvent)
                {
                    if (spellProcEvent->spellFamilyMask[i])
                    {
                        if (!execData.spellInfo->IsFitToFamilyMask(spellProcEvent->spellFamilyMask[i]))
                            continue;

                        // don't allow proc from cast end for non modifier spells
                        // unless they have proc ex defined for that
                        if (IsCastEndProcModifierAura(triggeredByHolder->GetSpellProto(), SpellEffectIndex(i), execData.spellInfo))
                        {
                            if (useCharges && execData.procExtra != PROC_EX_CAST_END && spellProcEvent->procEx == PROC_EX_NONE)
                                continue;
                        }
                        else if (spellProcEvent->procEx == PROC_EX_NONE && execData.procExtra == PROC_EX_CAST_END)
                            continue;
                    }
                    // don't check dbc FamilyFlags if schoolMask exists
                    else if (!execData.triggeredByAura->CanProcFrom(execData.spellInfo, execData.procFlags, spellProcEvent->procEx, execData.procExtra, execData.damage != 0, !spellProcEvent->schoolMask))
                        continue;
                }
                else if (!execData.triggeredByAura->CanProcFrom(execData.spellInfo, execData.procFlags, PROC_EX_NONE, execData.procExtra, execData.damage != 0, true))
                    continue;
            }

            execData.triggeredSpellId = 0;
            execData.basepoints = { 0, 0, 0 };
            SpellAuraProcResult procResult = execData.triggeredByAura->OnProc(execData);
            if (procResult == SPELL_AURA_PROC_OK)
                procResult = (*this.*AuraProcHandler[auraModifier->m_auraname])(execData);
            switch (procResult)
            {
                case SPELL_AURA_PROC_CANT_TRIGGER:
                    continue;
                case SPELL_AURA_PROC_FAILED:
                    procSuccess = false;
                    break;
                case SPELL_AURA_PROC_OK:
                    if (execData.procOnce && execData.spell)
                        execData.spell->RegisterAuraProc(execData.triggeredByAura);
                    break;
            }

            anyAuraProc = true;
        }

        // Remove charge (aura can be removed by triggers)
        if (useCharges && procSuccess && anyAuraProc && !triggeredByHolder->IsDeleted())
        {
            // If last charge dropped add spell to remove list
            if (triggeredByHolder->DropAuraCharge())
                RemoveSpellAuraHolder(triggeredByHolder);
        }
    }
}

bool Unit::IsTriggeredAtSpellProcEvent(ProcExecutionData& data, SpellAuraHolder* holder, SpellProcEventEntry const*& spellProcEvent)
{
    SpellEntry const* spellProto = holder->GetSpellProto();

    // Get proc Event Entry
    spellProcEvent = sSpellMgr.GetSpellProcEvent(spellProto->Id);

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags) // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->procFlags;       // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return false;

    // Check spellProcEvent data requirements
    if (!SpellMgr::IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, data.spellInfo, data.procFlags, data.procExtra))
        return false;

    // In most cases req get honor or XP from kill
    if (EventProcFlag & PROC_FLAG_KILL && GetTypeId() == TYPEID_PLAYER)
    {
        bool allow = ((Player*)this)->isHonorOrXPTarget(data.target);
        // Shadow Word: Death - can trigger from every kill
        if (holder->GetId() == 32409)
            allow = true;
        if (!allow)
            return false;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic triggers (can triggered from self)
    if (data.spellInfo && data.spellInfo->Id == spellProto->Id && !(EventProcFlag & PROC_FLAG_TAKE_HARMFUL_PERIODIC))
        return false;

    // Check if current equipment allows aura to proc
    if (!data.isVictim && GetTypeId() == TYPEID_PLAYER)
    {
        if (spellProto->EquippedItemClass == ITEM_CLASS_WEAPON)
        {
            Item* item = nullptr;
            switch (data.attType)
            {
                default:
                case BASE_ATTACK:
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                    break;
                case OFF_ATTACK:
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                    break;
                case RANGED_ATTACK:
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
                    break;
            }

            if (!CanUseEquippedWeapon(data.attType))
                return false;

            if (!item || item->IsBroken() || item->GetProto()->Class != ITEM_CLASS_WEAPON || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
        else if (spellProto->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item* item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || !CanUseEquippedWeapon(OFF_ATTACK) || item->GetProto()->Class != ITEM_CLASS_ARMOR || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
    }

    // Get chance from spell
    float chance = (float)spellProto->procChance;
    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if (spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;
    // If PPM exist calculate chance from PPM
    if (!data.isVictim && spellProcEvent && spellProcEvent->ppmRate != 0)
    {
        uint32 WeaponSpeed = GetAttackTime(data.attType);
        chance = GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate);
    }
    // Apply chance modifier aura
    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_FREQUENCY_OF_SUCCESS, chance);
    }

    // proc chance is reduced by an additional 3.333% per level past 60
    if (holder->IsReducedProcChancePast60() && GetLevel() > 60)
        chance = std::max(0.f, (1.f - ((GetLevel() - 60) * 1.f / 30.f)) * chance);

    if (data.spell)
    {
        if (data.spell->m_IsTriggeredSpell && !spellProto->HasAttribute(SPELL_ATTR_EX3_CAN_PROC_FROM_TRIGGERED))
        {
            if (!data.spell->m_spellInfo->HasAttribute(SPELL_ATTR_EX2_TRIGGERED_CAN_TRIGGER_PROC) && !data.spell->m_spellInfo->HasAttribute(SPELL_ATTR_EX3_TRIGGERED_CAN_TRIGGER_SPECIAL))
                return false;
        }

        for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (Aura* aura = holder->m_auras[i])
                if (data.spell->IsAuraProcced(aura))
                    return false;
    }

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura* aura = holder->m_auras[i])
            if (!aura->OnCheckProc(data))
                return false;

    return roll_chance_f(chance);
}

SpellAuraProcResult Unit::TriggerProccedSpell(Unit* target, std::array<int32, MAX_EFFECT_INDEX>& basepoints, uint32 triggeredSpellId, Item* castItem, Aura* triggeredByAura, uint32 cooldown)
{
    SpellEntry const* triggerEntry = sSpellTemplate.LookupEntry<SpellEntry>(triggeredSpellId);

    if (!triggerEntry)
    {
        sLog.outError("Unit::TriggerProccedSpell: Script has nonexistent triggered spell %u", triggeredSpellId);
        return SPELL_AURA_PROC_FAILED;
    }

    return TriggerProccedSpell(target, basepoints, triggerEntry, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::TriggerProccedSpell(Unit* target, std::array<int32, MAX_EFFECT_INDEX>& basepoints, SpellEntry const* spellInfo, Item* castItem, Aura* triggeredByAura, uint32 cooldown)
{
    // default case
    if (target && (target != this && !target->IsAlive()))
        return SPELL_AURA_PROC_FAILED;

    if (!IsSpellReady(*spellInfo))
        return SPELL_AURA_PROC_FAILED;

    if (basepoints[EFFECT_INDEX_0] || basepoints[EFFECT_INDEX_1] || basepoints[EFFECT_INDEX_2])
        CastCustomSpell(target, spellInfo,
            basepoints[EFFECT_INDEX_0] ? &basepoints[EFFECT_INDEX_0] : nullptr,
            basepoints[EFFECT_INDEX_1] ? &basepoints[EFFECT_INDEX_1] : nullptr,
            basepoints[EFFECT_INDEX_2] ? &basepoints[EFFECT_INDEX_2] : nullptr,
            TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST | TRIGGERED_DO_NOT_RESET_LEASH, castItem, triggeredByAura);
    else
        CastSpell(target, spellInfo, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST | TRIGGERED_DO_NOT_RESET_LEASH, castItem, triggeredByAura);

    if (cooldown)
        AddCooldown(*spellInfo, nullptr, false, cooldown * IN_MILLISECONDS);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleHasteAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura; uint32 cooldown = data.cooldown;
    SpellEntry const* hasteSpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    uint32 triggered_spell_id = data.triggeredSpellId;
    Unit* target = triggered_spell_id ? data.triggerTarget : pVictim;
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;

    switch (hasteSpell->Id)
    {
        // Blade Flurry
        case 13877:
        case 33735:
        {
            target = SelectRandomUnfriendlyTarget(pVictim);
            if (!target)
                return SPELL_AURA_PROC_FAILED;
            basepoints[EFFECT_INDEX_0] = damage;
            triggered_spell_id = 22482;
            break;
        }
        // Flurry - Warrior/Shaman
        case 12966:
        case 12967:
        case 12968:
        case 12969:
        case 12970:
        case 16257:
        case 16277:
        case 16278:
        case 16279:
        case 16280:
            if (pVictim != GetTarget() || m_extraAttacksExecuting) // can only proc on main target
                return SPELL_AURA_PROC_FAILED;
            break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    return TriggerProccedSpell(target, basepoints, triggered_spell_id, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleSpellCritChanceAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.victim; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 cooldown = data.cooldown;
    if (!spellInfo)
        return SPELL_AURA_PROC_FAILED;

    SpellEntry const* triggeredByAuraSpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;

    switch (triggeredByAuraSpell->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            switch (triggeredByAuraSpell->Id)
            {
                // Focus Magic
                case 54646:
                {
                    Unit* caster = triggeredByAura->GetCaster();
                    if (!caster)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 54648;
                    target = caster;
                    break;
                }
            }
        }
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    return TriggerProccedSpell(target, basepoints, triggered_spell_id, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleDummyAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 procFlags = data.procFlags; uint32 procEx = data.procExtra; uint32 cooldown = data.cooldown;
    SpellEntry const* dummySpell = triggeredByAura->GetSpellProto();
    SpellEffectIndex effIndex = triggeredByAura->GetEffIndex();
    int32  triggerAmount = triggeredByAura->GetModifier()->m_amount;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    uint32 triggered_spell_id = data.triggeredSpellId;
    Unit* target = triggered_spell_id ? data.triggerTarget : pVictim;
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;
    if (!triggered_spell_id && dummySpell->EffectApplyAuraName[effIndex] == SPELL_AURA_DUMMY)
        triggered_spell_id = dummySpell->EffectTriggerSpell[triggeredByAura->GetEffIndex()];

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                // Eye for an Eye
                case 9799:
                case 25988:
                {
                    // return damage % to attacker but < 50% own total health
                    basepoints[0] = triggerAmount * int32(damage) / 100;
                    if (basepoints[0] > (int32)GetMaxHealth() / 2)
                        basepoints[0] = (int32)GetMaxHealth() / 2;

                    triggered_spell_id = 25997;
                    break;
                }
                // Sweeping Strikes (NPC spells may be)
                case 18765:
                case 35429:
                {
                    // prevent chain of triggered spell from same triggered spell
                    if (spellInfo && (spellInfo->Id == 26654 || spellInfo->Id == 12723))
                        return SPELL_AURA_PROC_FAILED;

                    target = SelectRandomUnfriendlyTarget(pVictim);
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    if (spellInfo)
                    {
                        if (spellInfo->Id == 1680) // whirlwind procs normalized damage
                            triggered_spell_id = 26654;
                        else if (spellInfo->TargetAuraState == AURA_STATE_HEALTHLESS_20_PERCENT)
                        {
                            if (target->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
                                triggered_spell_id = 12723;
                            else // execute procs normalized damage if target is above 20 percent
                                triggered_spell_id = 26654;
                        }
                        else
                            triggered_spell_id = 12723;
                    }
                    else
                        triggered_spell_id = 12723;

                    if (triggered_spell_id == 12723)
                        basepoints[0] = damage;
                    break;
                }
                // Twisted Reflection (boss spell)
                case 21063:
                    triggered_spell_id = 21064;
                    break;
                // Unstable Power
                case 24658:
                {
                    if (!spellInfo || spellInfo->Id == 24659)
                        return SPELL_AURA_PROC_FAILED;
                    // Need remove one 24659 aura
                    RemoveAuraHolderFromStack(24659);
                    return SPELL_AURA_PROC_OK;
                }
                // Restless Strength
                case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveAuraHolderFromStack(24662);
                    return SPELL_AURA_PROC_OK;
                }
                // Adaptive Warding (Frostfire Regalia set)
                case 28764:
                {
                    if (!spellInfo)
                        return SPELL_AURA_PROC_FAILED;

                    // find Mage Armor
                    bool found = false;
                    AuraList const& mRegenInterrupt = GetAurasByType(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
                    for (auto iter : mRegenInterrupt)
                    {
                        if (SpellEntry const* iterSpellProto = iter->GetSpellProto())
                        {
                            if (iterSpellProto->SpellFamilyName == SPELLFAMILY_MAGE && (iterSpellProto->SpellFamilyFlags & uint64(0x10000000)))
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                        return SPELL_AURA_PROC_FAILED;

                    switch (GetFirstSchoolInMask(GetSpellSchoolMask(spellInfo)))
                    {
                        case SPELL_SCHOOL_NORMAL:
                        case SPELL_SCHOOL_HOLY:
                            return SPELL_AURA_PROC_FAILED;  // ignored
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 28765; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 28768; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 28766; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 28769; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 28770; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
                // Obsidian Armor (Justice Bearer`s Pauldrons shoulder)
                case 27539:
                {
                    if (!spellInfo)
                        return SPELL_AURA_PROC_FAILED;

                    switch (GetFirstSchoolInMask(GetSpellSchoolMask(spellInfo)))
                    {
                        case SPELL_SCHOOL_NORMAL:
                            return SPELL_AURA_PROC_FAILED;  // ignore
                        case SPELL_SCHOOL_HOLY:   triggered_spell_id = 27536; break;
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 27533; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 27538; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 27534; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 27535; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 27540; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
                // Mana Leech (Passive) (Priest Pet Aura)
                case 28305:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 34650;
                    break;
                }
                // Divine purpose
                case 31871:
                case 31872:
                {
                    // Roll chance
                    if (!roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    // Remove any stun effect on target
                    SpellAuraHolderMap& Auras = pVictim->GetSpellAuraHolderMap();
                    for (SpellAuraHolderMap::const_iterator iter = Auras.begin(); iter != Auras.end();)
                    {
                        if (iter->second->HasMechanic(MECHANIC_STUN))
                        {
                            pVictim->RemoveAurasDueToSpell(iter->second->GetId());
                            iter = Auras.begin();
                        }
                        else
                            ++iter;
                    }
                    return SPELL_AURA_PROC_OK;
                }
                // Soul Charge - Archimonde
                case 32045: // Yellow
                case 32051: // Green
                case 32052: // Red
                {
                    switch (dummySpell->Id)
                    {
                        case 32045: triggered_spell_id = 32054; break;
                        case 32051: triggered_spell_id = 32057; break;
                        case 32052: triggered_spell_id = 32053; break;
                    }

                    AI()->SendAIEvent(AI_EVENT_CUSTOM_A, this, this, triggered_spell_id);
                    return SPELL_AURA_PROC_CANT_TRIGGER; // Strangely, stacks and charges are removed on cast finish
                }
                // Mark of Malice
                case 33493:
                {
                    // Cast finish spell at last charge
                    if (triggeredByAura->GetHolder()->GetAuraCharges() > 1)
                        return SPELL_AURA_PROC_OK;

                    target = this;
                    triggered_spell_id = 33494;
                    break;
                }
                // Elemental Sieve
                case 36035:
                {
                    Pet* pCaster = dynamic_cast<Pet*>(triggeredByAura->GetCaster());

                    // aura only affect the spirit totem, since this is the one that need to be in range.
                    // It is possible though, that player is the one who should actually have the aura
                    // and check for presense of spirit totem, but then we can't script the dummy.
                    if (!pCaster)
                        return SPELL_AURA_PROC_FAILED;

                    // Summon the soul of the spirit and cast the visual
                    uint32 uiSoulEntry = 0;
                    switch (GetEntry())
                    {
                        case 21050: uiSoulEntry = 21073; break; // Earthen Soul
                        case 21061: uiSoulEntry = 21097; break; // Fiery Soul
                        case 21059: uiSoulEntry = 21109; break; // Watery Soul
                        case 21060: uiSoulEntry = 21116; break; // Airy Soul
                    }

                    CastSpell(this, 36206, TRIGGERED_OLD_TRIGGERED);
                    pCaster->SummonCreature(uiSoulEntry, GetPositionX(), GetPositionY(), GetPositionZ(), 0, TEMPSPAWN_TIMED_OOC_OR_CORPSE_DESPAWN, 10000);
                    break;
                }
                // Vampiric Aura (boss spell)
                case 38196:
                {
                    basepoints[0] = 3 * damage;             // 300%
                    if (basepoints[0] < 0)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 31285;
                    target = this;
                    break;
                }
                // Aura of Madness (Darkmoon Card: Madness trinket)
                //=====================================================
                // 39511 Sociopath: +35 strength (Paladin, Rogue, Druid, Warrior)
                // 40997 Delusional: +70 attack power (Rogue, Hunter, Paladin, Warrior, Druid)
                // 40998 Kleptomania: +35 agility (Warrior, Rogue, Paladin, Hunter, Druid)
                // 40999 Megalomania: +41 damage/healing (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41002 Paranoia: +35 spell/melee/ranged crit strike rating (All classes)
                // 41005 Manic: +35 haste (spell, melee and ranged) (All classes)
                // 41009 Narcissism: +35 intellect (Druid, Shaman, Priest, Warlock, Mage, Paladin, Hunter)
                // 41011 Martyr Complex: +35 stamina (All classes)
                // 41406 Dementia: Every 5 seconds either gives you +5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41409 Dementia: Every 5 seconds either gives you -5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                case 39446:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Select class defined buff
                    switch (getClass())
                    {
                        case CLASS_PALADIN:                 // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                        case CLASS_DRUID:                   // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                        {
                            uint32 RandomSpell[] = {39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell) - 1)];
                            break;
                        }
                        case CLASS_ROGUE:                   // 39511,40997,40998,41002,41005,41011
                        case CLASS_WARRIOR:                 // 39511,40997,40998,41002,41005,41011
                        {
                            uint32 RandomSpell[] = {39511, 40997, 40998, 41002, 41005, 41011};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell) - 1)];
                            break;
                        }
                        case CLASS_PRIEST:                  // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_SHAMAN:                  // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_MAGE:                    // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_WARLOCK:                 // 40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[] = {40999, 41002, 41005, 41009, 41011, 41406, 41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell) - 1)];
                            break;
                        }
                        case CLASS_HUNTER:                  // 40997,40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[] = {40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell) - 1)];
                            break;
                        }
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    if (roll_chance_i(10))
                        ((Player*)this)->Say("This is Madness!", LANG_UNIVERSAL);
                    break;
                }
                case 42454: // Captured Totem - procs on death quest credit
                {
                    Unit* caster = triggeredByAura->GetCaster();
                    if (!caster)
                        return SPELL_AURA_PROC_FAILED;
                    Unit* owner = caster->GetOwner();
                    if (!owner)
                        return SPELL_AURA_PROC_FAILED;
                    CastSpell(owner, 42455, TRIGGERED_NONE);
                    break;
                }
                // Sunwell Exalted Caster Neck (Shattered Sun Pendant of Acumen neck)
                // cast 45479 Light's Wrath if Exalted by Aldor
                // cast 45429 Arcane Bolt if Exalted by Scryers
                case 45481:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player*)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45479;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player*)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        // triggered at positive/self casts also, current attack target used then
                        if (!CanAttack(target))
                        {
                            target = GetVictim();
                            if (!target)
                            {
                                target = ObjectAccessor::GetUnit(*this, ((Player*)this)->GetSelectionGuid());
                                if (!target)
                                    return SPELL_AURA_PROC_FAILED;
                            }
                            if (!CanAttack(target))
                                return SPELL_AURA_PROC_FAILED;
                        }

                        triggered_spell_id = 45429;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Melee Neck (Shattered Sun Pendant of Might neck)
                // cast 45480 Light's Strength if Exalted by Aldor
                // cast 45428 Arcane Strike if Exalted by Scryers
                case 45482:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player*)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45480;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player*)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45428;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Tank Neck (Shattered Sun Pendant of Resolve neck)
                // cast 45431 Arcane Insight if Exalted by Aldor
                // cast 45432 Light's Ward if Exalted by Scryers
                case 45483:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player*)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45432;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player*)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45431;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Healer Neck (Shattered Sun Pendant of Restoration neck)
                // cast 45478 Light's Salvation if Exalted by Aldor
                // cast 45430 Arcane Surge if Exalted by Scryers
                case 45484:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player*)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45478;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player*)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45430;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Crates Carried
                case 46340:
                {
                    triggered_spell_id = 46342;
                    target = this;
                    break;
                }
                /*
                // Sunwell Exalted Caster Neck (??? neck)
                // cast ??? Light's Wrath if Exalted by Aldor
                // cast ??? Arcane Bolt if Exalted by Scryers*/
                case 46569:
                    return SPELL_AURA_PROC_FAILED;          // old unused version
                // Living Seed
                case 48504:
                {
                    triggered_spell_id = 48503;
                    basepoints[0] = triggerAmount;
                    target = this;
                    break;
                }
                // Vampiric Touch (generic, used by some boss)
                case 52723:
                case 60501:
                {
                    triggered_spell_id = 52724;
                    basepoints[0] = damage / 2;
                    target = this;
                    break;
                }
                // Shadowfiend Death (Gain mana if pet dies with Glyph of Shadowfiend)
                case 57989:
                {
                    Unit* owner = GetOwner();
                    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Glyph of Shadowfiend (need cast as self cast for owner, no hidden cooldown)
                    owner->CastSpell(owner, 58227, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                // Kill Command, pet aura
                case 58914:
                {
                    // also decrease owner buff stack
                    if (Unit* owner = GetOwner())
                        owner->RemoveAuraHolderFromStack(34027);

                    // Remove only single aura from stack
                    if (triggeredByAura->GetStackAmount() > 1 && !triggeredByAura->GetHolder()->ModStackAmount(-1, nullptr))
                        return SPELL_AURA_PROC_CANT_TRIGGER;
                }
                // Grim Reprisal
                case 63305:
                {
                    // also update caster entry if required
                    if (Unit* caster = triggeredByAura->GetCaster())
                    {
                        if (caster->GetEntry() != 33943 && caster->GetTypeId() == TYPEID_UNIT)
                        {
                            ((Creature*)caster)->UpdateEntry(33943);
                            caster->CastSpell(caster, 64017, TRIGGERED_OLD_TRIGGERED);
                        }
                    }

                    triggered_spell_id = 64039;
                    basepoints[EFFECT_INDEX_0] = damage;
                    break;
                }
                // Glyph of Life Tap
                case 63320:
                    triggered_spell_id = 63321;
                    break;
                // Retaliation
                case 65932:
                    triggered_spell_id = 65934;
                    break;
                // Earth Shield
                case 66063:
                    triggered_spell_id = 66064;
                    break;
                // Meteor Fists
                case 66725:
                case 68161:
                    triggered_spell_id = 66765;
                    break;
                // Meteor Fists
                case 66808:
                case 68160:
                    triggered_spell_id = 66809;
                    break;
                // Shiny Shard of the Scale - Equip Effect
                case 69739:
                    // Cauterizing Heal or Searing Flame
                    triggered_spell_id = (procFlags & PROC_FLAG_DEAL_HELPFUL_SPELL) ? 69734 : 69730;
                    break;
                // Purified Shard of the Scale - Equip Effect
                case 69755:
                    // Cauterizing Heal or Searing Flame
                    triggered_spell_id = (procFlags & PROC_FLAG_DEAL_HELPFUL_SPELL) ? 69733 : 69729;
                    break;
                // Item - Shadowmourne Legendary
                case 71903:
                {
                    if (!roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 71905;             // Soul Fragment

                    SpellAuraHolder* aurHolder = GetSpellAuraHolder(triggered_spell_id);

                    // will added first to stack
                    if (!aurHolder)
                        CastSpell(this, 72521, TRIGGERED_OLD_TRIGGERED);       // Shadowmourne Visual Low
                    // half stack
                    else if (aurHolder->GetStackAmount() + 1 == 6)
                        CastSpell(this, 72523, TRIGGERED_OLD_TRIGGERED);       // Shadowmourne Visual High
                    // full stack
                    else if (aurHolder->GetStackAmount() + 1 >= aurHolder->GetSpellProto()->StackAmount)
                    {
                        RemoveAurasDueToSpell(triggered_spell_id);
                        CastSpell(this, 71904, TRIGGERED_OLD_TRIGGERED);       // Chaos Bane
                        return SPELL_AURA_PROC_OK;
                    }
                    break;
                }
                // Blood Link
                case 72178:
                    triggered_spell_id = 72195;
                    break;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Magic Absorption
            if (dummySpell->SpellIconID == 459)             // only this spell have SpellIconID == 459 and dummy aura
            {
                if (GetPowerType() != POWER_MANA)
                    return SPELL_AURA_PROC_FAILED;

                // mana reward
                basepoints[0] = (triggerAmount * GetMaxPower(POWER_MANA) / 100);
                target = this;
                triggered_spell_id = 29442;
                break;
            }
            // Master of Elements
            if (dummySpell->SpellIconID == 1920)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                // mana cost save
                int32 cost = spellInfo->manaCost + spellInfo->ManaCostPercentage * GetCreateMana() / 100;
                basepoints[0] = cost * triggerAmount / 100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 29077;
                data.procOnce = true;
                break;
            }

            // Arcane Potency
            if (dummySpell->SpellIconID == 2120)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                switch (dummySpell->Id)
                {
                    case 31571: triggered_spell_id = 57529; break;
                    case 31572: triggered_spell_id = 57531; break;
                    default:
                        sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u", dummySpell->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }

            // Hot Streak
            if (dummySpell->SpellIconID == 2999)
            {
                if (effIndex != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_OK;
                Aura* counter = GetAura(triggeredByAura->GetId(), EFFECT_INDEX_1);
                if (!counter)
                    return SPELL_AURA_PROC_OK;

                // Count spell criticals in a row in second aura
                Modifier* mod = counter->GetModifier();
                if (procEx & PROC_EX_CRITICAL_HIT)
                {
                    mod->m_amount *= 2;
                    if (mod->m_amount < 100) // not enough
                        return SPELL_AURA_PROC_OK;
                    // Critical counted -> roll chance
                    if (roll_chance_i(triggerAmount))
                        CastSpell(this, 48108, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                }
                mod->m_amount = 25;
                return SPELL_AURA_PROC_OK;
            }
            // Burnout
            if (dummySpell->SpellIconID == 2998)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                int32 cost = spellInfo->manaCost + spellInfo->ManaCostPercentage * GetCreateMana() / 100;
                basepoints[0] = cost * triggerAmount / 100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = 44450;
                target = this;
                break;
            }
            switch (dummySpell->Id)
            {
                // Ignite
                case 11119:
                case 11120:
                case 12846:
                case 12847:
                case 12848:
                {
                    switch (dummySpell->Id)
                    {
                        case 11119: basepoints[0] = int32(0.04f * damage); break;
                        case 11120: basepoints[0] = int32(0.08f * damage); break;
                        case 12846: basepoints[0] = int32(0.12f * damage); break;
                        case 12847: basepoints[0] = int32(0.16f * damage); break;
                        case 12848: basepoints[0] = int32(0.20f * damage); break;
                        default:
                            sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (IG)", dummySpell->Id);
                            return SPELL_AURA_PROC_FAILED;
                    }

                    triggered_spell_id = 12654;
                    break;
                }
                // Glyph of Ice Block
                case 56372:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // not 100% safe with client version switches but for 3.1.3 no spells with cooldown that can have mage player except Frost Nova.
                    RemoveSpellCategoryCooldown(35, true);
                    return SPELL_AURA_PROC_OK;
                }
                // Glyph of Polymorph
                case 56375:
                {
                    if (!pVictim || !pVictim->IsAlive())
                        return SPELL_AURA_PROC_FAILED;

                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE);
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                    return SPELL_AURA_PROC_OK;
                }
                // Blessing of Ancient Kings
                case 64411:
                {
                    // for DOT procs
                    if (!IsPositiveSpell(spellInfo->Id))
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 64413;
                    basepoints[0] = damage * 15 / 100;
                    break;
                }
                // Fingers of Frost
                case 74396:
                {
                    // Remove only single aura from stack and remove holder if its last stack
                    RemoveAuraHolderFromStack(74396);
                    return SPELL_AURA_PROC_OK;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Retaliation
            if (dummySpell->IsFitToFamilyMask(uint64(0x0000000800000000)))
            {
                // check attack comes not from behind
                if (pVictim->IsFacingTargetsBack(this))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 22858;
                break;
            }
            // Second Wind
            if (dummySpell->SpellIconID == 1697)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells (5530 Mace Stun Effect for example)
                if (!spellInfo || !(procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) || this == pVictim)
                    return SPELL_AURA_PROC_FAILED;
                // Need stun or root mechanic
                if (!(GetAllSpellMechanicMask(spellInfo) & IMMUNE_TO_ROOT_AND_STUN_MASK))
                    return SPELL_AURA_PROC_FAILED;

                switch (dummySpell->Id)
                {
                    case 29838: triggered_spell_id = 29842; break;
                    case 29834: triggered_spell_id = 29841; break;
                    case 42770: triggered_spell_id = 42771; break;
                    default:
                        sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (SW)", dummySpell->Id);
                        return SPELL_AURA_PROC_FAILED;
                }

                target = this;
                break;
            }
            // Damage Shield
            if (dummySpell->SpellIconID == 3214)
            {
                triggered_spell_id = 59653;
                basepoints[0] = GetShieldBlockValue() * triggerAmount / 100;
                break;
            }

            // Sweeping Strikes
            if (dummySpell->Id == 12328)
            {
                // prevent chain of triggered spell from same triggered spell
                if (spellInfo && spellInfo->Id == 26654)
                    return SPELL_AURA_PROC_FAILED;

                target = SelectRandomUnfriendlyTarget(pVictim);
                if (!target)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 26654;
                break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Fel Synergy
            if (dummySpell->SpellIconID == 3222)
            {
                target = GetPet();
                if (!target)
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = damage * triggerAmount / 100;
                triggered_spell_id = 54181;
                break;
            }
            switch (dummySpell->Id)
            {
                // Nightfall & Glyph of Corruption
                case 18094:
                case 18095:
                case 56218:
                {
                    target = this;
                    triggered_spell_id = 17941;
                    break;
                }
                // Soul Leech
                case 30293:
                case 30295:
                case 30296:
                {
                    // health
                    basepoints[0] = int32(damage * triggerAmount / 100);
                    target = this;
                    triggered_spell_id = 30294;
                    break;
                }
                // Shadowflame (Voidheart Raiment set bonus)
                case 37377:
                {
                    triggered_spell_id = 37379;
                    break;
                }
                // Pet Healing (Corruptor Raiment or Rift Stalker Armor)
                case 37381:
                {
                    target = GetPet();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    // heal amount
                    basepoints[0] = damage * triggerAmount / 100;
                    triggered_spell_id = 37382;
                    break;
                }
                // Shadowflame Hellfire (Voidheart Raiment set bonus)
                case 39437:
                {
                    triggered_spell_id = 37378;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Vampiric Touch
            if (dummySpell->SpellFamilyFlags & uint64(0x0000040000000000))
            {
                if (!pVictim || !pVictim->IsAlive())
                    return SPELL_AURA_PROC_FAILED;

                // pVictim is caster of aura
                if (triggeredByAura->GetCasterGuid() != pVictim->GetObjectGuid())
                    return SPELL_AURA_PROC_FAILED;

                // Energize 0.25% of max. mana
                pVictim->CastSpell(nullptr, 57669, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG, castItem, triggeredByAura);
                return SPELL_AURA_PROC_OK;                  // no hidden cooldown
            }

            switch (dummySpell->SpellIconID)
            {
                // Improved Shadowform
                case 217:
                {
                    if (!roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
                    RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
                    break;
                }
                // Divine Aegis
                case 2820:
                {
                    basepoints[0] = damage * triggerAmount / 100;
                    triggered_spell_id = 47753;
                    break;
                }
                // Empowered Renew
                case 3021:
                {
                    if (!spellInfo)
                        return SPELL_AURA_PROC_FAILED;

                    // Renew
                    Aura* healingAura = pVictim->GetAura(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_PRIEST, uint64(0x40), 0, GetObjectGuid());
                    if (!healingAura)
                        return SPELL_AURA_PROC_FAILED;

                    int32 healingfromticks = healingAura->GetModifier()->m_amount * GetSpellAuraMaxTicks(spellInfo);

                    basepoints[0] = healingfromticks * triggerAmount / 100;
                    triggered_spell_id = 63544;
                    break;
                }
                // Improved Devouring Plague
                case 3790:
                {
                    if (!spellInfo)
                        return SPELL_AURA_PROC_FAILED;

                    Aura* leachAura = pVictim->GetAura(SPELL_AURA_PERIODIC_LEECH, SPELLFAMILY_PRIEST, uint64(0x02000000), 0, GetObjectGuid());
                    if (!leachAura)
                        return SPELL_AURA_PROC_FAILED;

                    int32 damagefromticks = leachAura->GetModifier()->m_amount * GetSpellAuraMaxTicks(spellInfo);
                    basepoints[0] = damagefromticks * triggerAmount / 100;
                    triggered_spell_id = 63675;
                    break;
                }
            }

            switch (dummySpell->Id)
            {
                // Vampiric Embrace
                case 15286:
                {
                    // Return if self damage
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // Heal amount - Self/Team
                    int32 team = triggerAmount * damage / 500;
                    int32 self = triggerAmount * damage / 100 - team;
                    CastCustomSpell(nullptr, 15290, &team, &self, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;              // no hidden cooldown
                }
                // Priest Tier 6 Trinket (Ashtongue Talisman of Acumen)
                case 40438:
                {
                    // Shadow Word: Pain
                    if (spellInfo->SpellFamilyFlags & uint64(0x0000000000008000))
                        triggered_spell_id = 40441;
                    // Renew
                    else if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000040))
                        triggered_spell_id = 40440;
                    else
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Oracle Healing Bonus ("Garments of the Oracle" set)
                case 26169:
                {
                    // heal amount
                    basepoints[0] = int32(damage * 10 / 100);
                    target = this;
                    triggered_spell_id = 26170;
                    break;
                }
                // Frozen Shadoweave (Shadow's Embrace set) warning! its not only priest set
                case 39372:
                {
                    if (!spellInfo || (GetSpellSchoolMask(spellInfo) & (SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW)) == 0)
                        return SPELL_AURA_PROC_FAILED;

                    // heal amount
                    basepoints[0] = damage * triggerAmount / 100;
                    target = this;
                    triggered_spell_id = 39373;
                    break;
                }
                // Greater Heal (Vestments of Faith (Priest Tier 3) - 4 pieces bonus)
                case 28809:
                {
                    triggered_spell_id = 28810;
                    break;
                }
                // Glyph of Dispel Magic
                case 55677:
                {
                    if (!target->CanAssist(this))
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = int32(target->GetMaxHealth() * triggerAmount / 100);
                    // triggered_spell_id in spell data
                    break;
                }
                // Glyph of Prayer of Healing
                case 55680:
                {
                    basepoints[0] = int32(damage * triggerAmount  / 200);   // 10% each tick
                    triggered_spell_id = 56161;             // Glyph of Prayer of Healing
                    break;
                }
                // Priest T10 Healer 2P Bonus
                case 70770:
                {
                    triggered_spell_id = 70772;             // Blessed Healing
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (dummySpell->Id)
            {
                // Leader of the Pack
                case 24932:
                {
                    // dummy m_amount store health percent (!=0 if Improved Leader of the Pack applied)
                    int32 heal_percent = triggeredByAura->GetModifier()->m_amount;
                    if (!heal_percent)
                        return SPELL_AURA_PROC_FAILED;

                    // check explicitly only to prevent mana cast when halth cast cooldown
                    if (cooldown && !IsSpellReady(34299))
                        return SPELL_AURA_PROC_FAILED;

                    // health
                    triggered_spell_id = 34299;
                    basepoints[0] = GetMaxHealth() * heal_percent / 100;
                    target = this;

                    // mana to caster
                    if (triggeredByAura->GetCasterGuid() == GetObjectGuid())
                    {
                        if (SpellEntry const* manaCastEntry = sSpellTemplate.LookupEntry<SpellEntry>(60889))
                        {
                            int32 mana_percent = manaCastEntry->CalculateSimpleValue(EFFECT_INDEX_0) * heal_percent;
                            CastCustomSpell(this, manaCastEntry, &mana_percent, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                        }
                    }
                    break;
                }
                // Healing Touch (Dreamwalker Raiment set)
                case 28719:
                {
                    // mana back
                    basepoints[0] = int32(spellInfo->manaCost * 30 / 100);
                    target = this;
                    triggered_spell_id = 28742;
                    break;
                }
                // Healing Touch Refund (Idol of Longevity trinket)
                case 28847:
                {
                    target = this;
                    triggered_spell_id = 28848;
                    break;
                }
                // Mana Restore (Malorne Raiment set / Malorne Regalia set)
                case 37288:
                case 37295:
                {
                    target = this;
                    triggered_spell_id = 37238;
                    break;
                }
                // Druid Tier 6 Trinket
                case 40442:
                {
                    float  chance;

                    // Starfire
                    if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000004))
                    {
                        triggered_spell_id = 40445;
                        chance = 25.0f;
                    }
                    // Rejuvenation
                    else if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000010))
                    {
                        triggered_spell_id = 40446;
                        chance = 25.0f;
                    }
                    // Mangle (Bear) and Mangle (Cat)
                    else if (spellInfo->SpellFamilyFlags & uint64(0x0000044000000000))
                    {
                        triggered_spell_id = 40452;
                        chance = 40.0f;
                    }
                    else
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_f(chance))
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Maim Interrupt
                case 44835:
                {
                    // Deadly Interrupt Effect
                    triggered_spell_id = 32747;
                    break;
                }
                // Glyph of Rejuvenation
                case 54754:
                {
                    // less 50% health
                    if (pVictim->GetMaxHealth() < 2 * pVictim->GetHealth())
                        return SPELL_AURA_PROC_FAILED;
                    basepoints[0] = triggerAmount * damage / 100;
                    triggered_spell_id = 54755;
                    break;
                }
                // Glyph of Rake
                case 54821:
                {
                    triggered_spell_id = 54820;
                    break;
                }
                // Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
                case 70664:
                {
                    if (!spellInfo || GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    float radius;
                    if (spellInfo->EffectRadiusIndex[EFFECT_INDEX_0])
                        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellInfo->EffectRadiusIndex[EFFECT_INDEX_0]));
                    else
                        radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellInfo->rangeIndex));

                    ((Player*)this)->ApplySpellMod(spellInfo->Id, SPELLMOD_RADIUS, radius);

                    Unit* second = pVictim->SelectRandomFriendlyTarget(pVictim, radius);

                    if (!second)
                        return SPELL_AURA_PROC_FAILED;

                    pVictim->CastSpell(second, spellInfo, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura, GetObjectGuid());
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Druid T10 Balance 4P Bonus
                case 70723:
                {
                    triggered_spell_id = 71023;             // Languish
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
            }
            // King of the Jungle
            if (dummySpell->SpellIconID == 2850)
            {
                switch (effIndex)
                {
                    case EFFECT_INDEX_0:    // Enrage (bear)
                    {
                        // note : aura removal is done in SpellAuraHolder::HandleSpellSpecificBoosts
                        basepoints[0] = triggerAmount;
                        triggered_spell_id = 51185;
                        break;
                    }
                    case EFFECT_INDEX_1:    // Tiger's Fury (cat)
                    {
                        basepoints[0] = triggerAmount;
                        triggered_spell_id = 51178;
                        break;
                    }
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Eclipse
            if (dummySpell->SpellIconID == 2856)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                // Wrath crit
                if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000001))
                {
                    if (HasAura(48517))
                        return SPELL_AURA_PROC_FAILED;
                    if (!roll_chance_i(60))
                        return SPELL_AURA_PROC_FAILED;
                    triggered_spell_id = 48518;
                    target = this;
                    break;
                }
                // Starfire crit
                if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000004))
                {
                    if (HasAura(48518))
                        return SPELL_AURA_PROC_FAILED;
                    triggered_spell_id = 48517;
                    target = this;
                    break;
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Living Seed
            if (dummySpell->SpellIconID == 2860)
            {
                triggered_spell_id = 48504;
                basepoints[0] = triggerAmount * damage / 100;
                break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (dummySpell->Id)
            {
                // Clean Escape
                case 23582:
                    // triggered spell have same masks and etc with main Vanish spell
                    if (!spellInfo || spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_NONE)
                        return SPELL_AURA_PROC_FAILED;
                    triggered_spell_id = 23583;
                    break;
                // Deadly Throw Interrupt
                case 32748:
                {
                    // Prevent cast Deadly Throw Interrupt on self from last effect (apply dummy) of Deadly Throw
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 32747;
                    break;
                }
                // Tricks of the trade
                case 57934:
                {
                    triggered_spell_id = 57933;             // Tricks of the Trade, increased damage buff
                    target = getHostileRefManager().GetThreatRedirectionTarget();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    CastSpell(this, 59628, TRIGGERED_OLD_TRIGGERED);           // Tricks of the Trade (caster timer)
                    break;
                }
            }
            // Cut to the Chase
            if (dummySpell->SpellIconID == 2909)
            {
                // "refresh your Slice and Dice duration to its 5 combo point maximum"
                // lookup Slice and Dice
                AuraList const& sd = GetAurasByType(SPELL_AURA_MOD_MELEE_HASTE);
                for (auto itr : sd)
                {
                    SpellEntry const* spellProto = itr->GetSpellProto();
                    if (spellProto->SpellFamilyName == SPELLFAMILY_ROGUE &&
                            (spellProto->SpellFamilyFlags & uint64(0x0000000000040000)))
                    {
                        itr->GetHolder()->RefreshHolder();
                        return SPELL_AURA_PROC_OK;
                    }
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Deadly Brew
            if (dummySpell->SpellIconID == 2963)
            {
                triggered_spell_id = 44289;
                break;
            }
            // Quick Recovery
            if (dummySpell->SpellIconID == 2116)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                // energy cost save
                basepoints[0] = spellInfo->manaCost * triggerAmount / 100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 31663;
                break;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Thrill of the Hunt
            if (dummySpell->SpellIconID == 2236)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                // mana cost save
                int32 mana = spellInfo->manaCost + spellInfo->ManaCostPercentage * GetCreateMana() / 100;
                basepoints[0] = mana * 40 / 100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 34720;
                break;
            }
            // Hunting Party
            if (dummySpell->SpellIconID == 3406)
            {
                triggered_spell_id = 57669;
                target = this;
                break;
            }
            // Lock and Load
            if (dummySpell->SpellIconID == 3579)
            {
                // Proc only from periodic (from trap activation proc another aura of this spell)
                if (!(procFlags & PROC_FLAG_DEAL_HARMFUL_PERIODIC) || !roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = 56453;
                target = this;
                break;
            }
            // Rapid Recuperation
            if (dummySpell->SpellIconID == 3560)
            {
                // This effect only from Rapid Killing (mana regen)
                if (!(spellInfo->SpellFamilyFlags & uint64(0x0100000000000000)))
                    return SPELL_AURA_PROC_FAILED;

                target = this;

                switch (dummySpell->Id)
                {
                    case 53228:                             // Rank 1
                        triggered_spell_id = 56654;
                        break;
                    case 53232:                             // Rank 2
                        triggered_spell_id = 58882;
                        break;
                }
                break;
            }
            // Glyph of Mend Pet
            if (dummySpell->Id == 57870)
            {
                pVictim->CastSpell(pVictim, 57894, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetObjectGuid());
                return SPELL_AURA_PROC_OK;
            }
            // Misdirection
            if (dummySpell->Id == 34477)
            {
                triggered_spell_id = 35079;                 // 4 sec buff on self
                target = this;
                break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Seal of Righteousness - melee proc dummy (addition ${$MWS*(0.022*$AP+0.044*$SPH)} damage)
            if ((dummySpell->SpellFamilyFlags & uint64(0x000000008000000)) && effIndex == EFFECT_INDEX_0)
            {
                triggered_spell_id = 25742;
                float ap = GetTotalAttackPowerValue(BASE_ATTACK);
                int32 holy = SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
                if (holy < 0)
                    holy = 0;
                basepoints[0] = GetAttackTime(BASE_ATTACK) * int32(ap * 0.022f + 0.044f * holy) / 1000;
                break;
            }
            // Righteous Vengeance
            if (dummySpell->SpellIconID == 3025)
            {
                triggered_spell_id = 61840;
                basepoints[0] = triggerAmount * damage / 100 / GetSpellAuraMaxTicks(triggered_spell_id);
                break;
            }
            // Sheath of Light
            if (dummySpell->SpellIconID == 3030)
            {
                triggered_spell_id = 54203;
                basepoints[0] = triggerAmount * damage / 100 / GetSpellAuraMaxTicks(triggered_spell_id);
                break;
            }
            switch (dummySpell->Id)
            {
                // Judgement of Light
                case 20185:
                {
                    // only at real damage
                    if (!damage)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = int32(pVictim->GetMaxHealth() * triggeredByAura->GetModifier()->m_amount / 100);
                    pVictim->CastCustomSpell(pVictim, 20267, &basepoints[0], nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG, nullptr, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                // Judgement of Wisdom
                case 20186:
                {
                    // only at real damage
                    if (!damage)
                        return SPELL_AURA_PROC_FAILED;

                    if (pVictim->GetPowerType() == POWER_MANA)
                    {
                        // 2% of maximum base mana
                        basepoints[0] = int32(pVictim->GetCreateMana() * 2 / 100);
                        pVictim->CastCustomSpell(pVictim, 20268, &basepoints[0], nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG, nullptr, triggeredByAura);
                    }
                    return SPELL_AURA_PROC_OK;
                }
                // Heart of the Crusader (Rank 1)
                case 20335:
                    triggered_spell_id = 21183;
                    break;
                // Heart of the Crusader (Rank 2)
                case 20336:
                    triggered_spell_id = 54498;
                    break;
                // Heart of the Crusader (Rank 3)
                case 20337:
                    triggered_spell_id = 54499;
                    break;
                case 20911:                                 // Blessing of Sanctuary
                case 25899:                                 // Greater Blessing of Sanctuary
                {
                    target = this;
                    switch (target->GetPowerType())
                    {
                        case POWER_MANA:
                            triggered_spell_id = 57319;
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                    default:
                        break;
                    }
                // Holy Power (Redemption Armor set)
                case 28789:
                {
                    if (!pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // Set class defined buff
                    switch (pVictim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28795;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28793;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28791;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28790;     // Increases the friendly target's armor
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Spiritual Attunement
                case 31785:
                case 33776:
                {
                    // Only if healed by another unit, not spells like First Aid and only when actual heal occured
                    if (this == pVictim || spellInfo->HasAttribute(SPELL_ATTR_ABILITY) || data.healthGain == 0)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = triggerAmount * data.healthGain / 100;
                    target = this;
                    triggered_spell_id = 31786;
                    break;
                }
                // Seal of Vengeance (damage calc on apply aura)
                case 31801:
                {
                    if (effIndex != EFFECT_INDEX_0)         // effect 1,2 used by seal unleashing code
                        return SPELL_AURA_PROC_FAILED;

                    // At melee attack or Hammer of the Righteous spell damage considered as melee attack
                    if ((procFlags & PROC_FLAG_DEAL_MELEE_ABILITY) || (spellInfo && spellInfo->Id == 53595))
                        triggered_spell_id = 31803;         // Holy Vengeance

                    // Add 5-stack effect from Holy Vengeance
                    const SpellAuraHolder* existing = target->GetSpellAuraHolder(31803, GetObjectGuid());
                    if (existing && existing->GetStackAmount() >= 5)
                        CastSpell(target, 42463, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);
                    break;
                }
                // Judgements of the Wise
                case 31876:
                case 31877:
                case 31878:
                    // triggered only at casted Judgement spells, not at additional Judgement effects
                    if (!spellInfo || spellInfo->Category != 1210)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    triggered_spell_id = 31930;

                    // Replenishment
                    CastSpell(this, 57669, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);
                    break;
                // Light's Beacon (heal target area aura)
                case 53651:
                {
                    // not do bonus heal for explicit beacon focus healing
                    if (GetObjectGuid() == triggeredByAura->GetCasterGuid())
                        return SPELL_AURA_PROC_FAILED;

                    // beacon
                    Unit* beacon = triggeredByAura->GetCaster();
                    if (!beacon)
                        return SPELL_AURA_PROC_FAILED;

                    // find caster main aura at beacon
                    Aura* dummy = nullptr;
                    Unit::AuraList const& baa = beacon->GetAurasByType(SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                    for (auto i : baa)
                    {
                        if (i->GetId() == 53563 && i->GetCasterGuid() == pVictim->GetObjectGuid())
                        {
                            dummy = i;
                            break;
                        }
                    }

                    // original heal must be form beacon caster
                    if (!dummy)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 53652;             // Beacon of Light
                    basepoints[0] = triggeredByAura->GetModifier()->m_amount * damage / 100;

                    // cast with original caster set but beacon to beacon for apply caster mods and avoid LoS check
                    beacon->CastCustomSpell(beacon, triggered_spell_id, &basepoints[0], nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura, pVictim->GetObjectGuid());
                    return SPELL_AURA_PROC_OK;
                }
                // Seal of Corruption (damage calc on apply aura)
                case 53736:
                {
                    if (effIndex != EFFECT_INDEX_0)         // effect 1,2 used by seal unleashing code
                        return SPELL_AURA_PROC_FAILED;

                    // At melee attack or Hammer of the Righteous spell damage considered as melee attack
                    if ((procFlags & PROC_FLAG_DEAL_MELEE_ABILITY) || (spellInfo && spellInfo->Id == 53595))
                        triggered_spell_id = 53742;         // Blood Corruption

                    // Add 5-stack effect from Blood Corruption
                    uint32 stacks = 0;
                    AuraList const& auras = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (auto aura : auras)
                    {
                        if ((aura->GetId() == 53742) && aura->GetCasterGuid() == GetObjectGuid())
                        {
                            stacks = aura->GetStackAmount();
                            break;
                        }
                    }
                    if (stacks >= 5)
                        CastSpell(target, 53739, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);
                    break;
                }
                // Glyph of Holy Light
                case 54937:
                {
                    triggered_spell_id = 54968;
                    basepoints[0] = triggerAmount * damage / 100;
                    break;
                }
                // Sacred Shield (buff)
                case 58597:
                {
                    triggered_spell_id = 66922;
                    basepoints[0] = int32(damage / GetSpellAuraMaxTicks(triggered_spell_id));
                    target = this;
                    break;
                }
                // Sacred Shield (talent rank)
                case 53601:
                {
                    // triggered_spell_id in spell data
                    target = this;
                    break;
                }
                // Item - Paladin T8 Holy 2P Bonus
                case 64890:
                {
                    triggered_spell_id = 64891;             // Holy Mending
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Anger Capacitor
                case 71406:                                 // normal
                case 71545:                                 // heroic
                {
                    if (!pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    SpellEntry const* mote = sSpellTemplate.LookupEntry<SpellEntry>(71432);
                    if (!mote)
                        return SPELL_AURA_PROC_FAILED;
                    uint32 maxStack = mote->StackAmount - (dummySpell->Id == 71545 ? 1 : 0);

                    SpellAuraHolder* aurHolder = GetSpellAuraHolder(71432);
                    if (aurHolder && uint32(aurHolder->GetStackAmount() + 1) >= maxStack)
                    {
                        RemoveAurasDueToSpell(71432);       // Mote of Anger

                        // Manifest Anger (main hand/off hand)
                        CastSpell(pVictim, !hasOffhandWeaponForAttack() || roll_chance_i(50) ? 71433 : 71434, TRIGGERED_OLD_TRIGGERED);
                        return SPELL_AURA_PROC_OK;
                    }
                    triggered_spell_id = 71432;

                    break;
                }
                // Heartpierce, Item - Icecrown 25 Normal Dagger Proc
                case 71880:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    switch (this->GetPowerType())
                    {
                        case POWER_ENERGY: triggered_spell_id = 71882; break;
                        case POWER_RAGE:   triggered_spell_id = 71883; break;
                        case POWER_MANA:   triggered_spell_id = 71881; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Heartpierce, Item - Icecrown 25 Heroic Dagger Proc
                case 71892:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    switch (this->GetPowerType())
                    {
                        case POWER_ENERGY: triggered_spell_id = 71887; break;
                        case POWER_RAGE:   triggered_spell_id = 71886; break;
                        case POWER_MANA:   triggered_spell_id = 71888; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (dummySpell->Id)
            {
                // Totemic Power (The Earthshatterer set)
                case 28823:
                {
                    if (!pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // Set class defined buff
                    switch (pVictim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28824;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28825;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28826;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28827;     // Increases the friendly target's armor
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Lesser Healing Wave (Totem of Flowing Water Relic)
                case 28849:
                {
                    target = this;
                    triggered_spell_id = 28850;
                    break;
                }
                // Windfury Weapon (Passive) 1-5 Ranks
                case 33757:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    if (!castItem || !castItem->IsEquipped())
                        return SPELL_AURA_PROC_FAILED;

                    if (castItem->GetSlot() == EQUIPMENT_SLOT_MAINHAND && procFlags & PROC_FLAG_OFF_HAND_WEAPON_SWING)
                        return SPELL_AURA_PROC_FAILED;

                    if (castItem->GetSlot() == EQUIPMENT_SLOT_OFFHAND && procFlags & PROC_FLAG_MAIN_HAND_WEAPON_SWING)
                        return SPELL_AURA_PROC_FAILED;

                    // custom cooldown processing case
                    if (cooldown && !IsSpellReady(*dummySpell))
                        return SPELL_AURA_PROC_FAILED;

                    // Now amount of extra power stored in 1 effect of Enchant spell
                    // Get it by item enchant id
                    uint32 spellId;
                    switch (castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)))
                    {
                        case 283: spellId =  8232; break;   // 1 Rank
                        case 284: spellId =  8235; break;   // 2 Rank
                        case 525: spellId = 10486; break;   // 3 Rank
                        case 1669: spellId = 16362; break;  // 4 Rank
                        case 2636: spellId = 25505; break;  // 5 Rank
                        case 3785: spellId = 58801; break;  // 6 Rank
                        case 3786: spellId = 58803; break;  // 7 Rank
                        case 3787: spellId = 58804; break;  // 8 Rank
                        default:
                        {
                            sLog.outError("Unit::HandleDummyAuraProc: non handled item enchantment (rank?) %u for spell id: %u (Windfury)",
                                          castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)), dummySpell->Id);
                            return SPELL_AURA_PROC_FAILED;
                        }
                    }

                    SpellEntry const* windfurySpellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                    if (!windfurySpellEntry)
                    {
                        sLog.outError("Unit::HandleDummyAuraProc: nonexistent spell id: %u (Windfury)", spellId);
                        return SPELL_AURA_PROC_FAILED;
                    }

                    int32 extra_attack_power = CalculateSpellEffectValue(pVictim, windfurySpellEntry, EFFECT_INDEX_1);

                    // Totem of Splintering
                    if (Aura* aura = GetAura(60764, EFFECT_INDEX_0))
                        extra_attack_power += aura->GetModifier()->m_amount;

                    if (Aura* aura = GetAura(34244, EFFECT_INDEX_0)) // Increased Windfury Weapon
                    {
                        extra_attack_power += aura->GetModifier()->m_amount;
                    }

                    // Off-Hand case
                    if (castItem->GetSlot() == EQUIPMENT_SLOT_OFFHAND)
                    {
                        // Value gained from additional AP
                        basepoints[0] = int32(extra_attack_power / 14.0f * GetAttackTime(OFF_ATTACK) / 1000 / 2);
                        triggered_spell_id = 33750;
                    }
                    // Main-Hand case
                    else
                    {
                        // Value gained from additional AP
                        basepoints[0] = int32(extra_attack_power / 14.0f * GetAttackTime(BASE_ATTACK) / 1000);
                        triggered_spell_id = 25504;
                    }

                    if (cooldown && !IsSpellReady(triggered_spell_id))
                        return SPELL_AURA_PROC_FAILED;

                    // apply cooldown before cast to prevent processing itself
                    if (cooldown)
                        AddCooldown(*dummySpell, nullptr, false, cooldown * IN_MILLISECONDS);

                    // Attack Twice
                    for (uint32 i = 0; i < 2; ++i)
                        CastCustomSpell(pVictim, triggered_spell_id, &basepoints[0], nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);

                    return SPELL_AURA_PROC_OK;
                }
                // Shaman Tier 6 Trinket
                case 40463:
                {
                    if (!spellInfo)
                        return SPELL_AURA_PROC_FAILED;

                    float  chance;
                    if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000001))
                    {
                        triggered_spell_id = 40465;         // Lightning Bolt
                        chance = 15.0f;
                    }
                    else if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000080))
                    {
                        triggered_spell_id = 40465;         // Lesser Healing Wave
                        chance = 10.0f;
                    }
                    else if (spellInfo->SpellFamilyFlags & uint64(0x0000001000000000))
                    {
                        triggered_spell_id = 40466;         // Stormstrike
                        chance = 50.0f;
                    }
                    else
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_f(chance))
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Glyph of Healing Wave
                case 55440:
                {
                    // Not proc from self heals
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;
                    basepoints[0] = triggerAmount * damage / 100;
                    target = this;
                    triggered_spell_id = 55533;
                    break;
                }
                // Spirit Hunt
                case 58877:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;
                    basepoints[0] = triggerAmount * damage / 100;
                    triggered_spell_id = 58879;
                    break;
                }
                // Glyph of Totem of Wrath
                case 63280:
                {
                    Totem* totem = GetTotem(TOTEM_SLOT_FIRE);
                    if (!totem)
                        return SPELL_AURA_PROC_FAILED;

                    // find totem aura bonus
                    AuraList const& spellPower = totem->GetAurasByType(SPELL_AURA_NONE);
                    for (auto i : spellPower)
                    {
                        // select proper aura for format aura type in spell proto
                        if (i->GetTarget() == totem && i->GetSpellProto()->EffectApplyAuraName[i->GetEffIndex()] == SPELL_AURA_MOD_HEALING_DONE &&
                            i->GetSpellProto()->SpellFamilyName == SPELLFAMILY_SHAMAN && i->GetSpellProto()->SpellFamilyFlags & uint64(0x0000000004000000))
                        {
                            basepoints[0] = triggerAmount * i->GetModifier()->m_amount / 100;
                            break;
                        }
                    }

                    if (!basepoints[0])
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[1] = basepoints[0];
                    triggered_spell_id = 63283;             // Totem of Wrath, caster bonus
                    target = this;
                    break;
                }
                // Item - Shaman T8 Elemental 4P Bonus
                case 64928:
                {
                    triggered_spell_id = 64930;            // Electrified
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Shaman T9 Elemental 4P Bonus (Lava Burst)
                case 67228:
                {
                    triggered_spell_id = 71824;             // Lava Burst
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Shaman T10 Restoration 4P Bonus
                case 70808:
                {
                    triggered_spell_id = 70809;             // Chained Heal
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
            }
            // Storm, Earth and Fire
            if (dummySpell->SpellIconID == 3063)
            {
                // Earthbind Totem summon only
                if (spellInfo->Id != 2484)
                    return SPELL_AURA_PROC_FAILED;

                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 64695;
                break;
            }
            // Ancestral Awakening
            if (dummySpell->SpellIconID == 3065)
            {
                triggered_spell_id = 52759;
                basepoints[0] = triggerAmount * damage / 100;
                target = this;
                break;
            }
            // Flametongue Weapon (Passive), Ranks
            if (dummySpell->SpellFamilyFlags & uint64(0x0000000000200000))
            {
                if (GetTypeId() != TYPEID_PLAYER || !castItem)
                    return SPELL_AURA_PROC_FAILED;

                // Only proc for enchanted weapon
                Item* usedWeapon = ((Player*)this)->GetWeaponForAttack(procFlags & PROC_FLAG_OFF_HAND_WEAPON_SWING ? OFF_ATTACK : BASE_ATTACK, true, true);
                if (usedWeapon != castItem)
                    return SPELL_AURA_PROC_FAILED;

                switch (dummySpell->Id)
                {
                    case 10400: triggered_spell_id =  8026; break; // Rank 1
                    case 15567: triggered_spell_id =  8028; break; // Rank 2
                    case 15568: triggered_spell_id =  8029; break; // Rank 3
                    case 15569: triggered_spell_id = 10445; break; // Rank 4
                    case 16311: triggered_spell_id = 16343; break; // Rank 5
                    case 16312: triggered_spell_id = 16344; break; // Rank 6
                    case 16313: triggered_spell_id = 25488; break; // Rank 7
                    case 58784: triggered_spell_id = 58786; break; // Rank 8
                    case 58791: triggered_spell_id = 58787; break; // Rank 9
                    case 58792: triggered_spell_id = 58788; break; // Rank 10
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Earth Shield
            if (dummySpell->SpellFamilyFlags & uint64(0x0000040000000000))
            {
                target = this;
                basepoints[0] = triggerAmount;

                // Glyph of Earth Shield
                if (Unit* caster = triggeredByAura->GetCaster())
                {
                    if (Aura* aur = caster->GetDummyAura(63279))
                    {
                        int32 aur_mod = aur->GetModifier()->m_amount;
                        basepoints[0] = int32(basepoints[0] * (aur_mod + 100.0f) / 100.0f);
                    }
                }

                triggered_spell_id = 379;
                break;
            }
            // Improved Water Shield
            if (dummySpell->SpellIconID == 2287)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                // Lesser Healing Wave need aditional 60% roll
                if ((spellInfo->SpellFamilyFlags & uint64(0x0000000000000080)) && !roll_chance_i(60))
                    return SPELL_AURA_PROC_FAILED;
                // Chain Heal needs additional 30% roll
                if ((spellInfo->SpellFamilyFlags & uint64(0x0000000000000100)) && !roll_chance_i(30))
                    return SPELL_AURA_PROC_FAILED;
                // lookup water shield
                AuraList const& vs = GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                for (auto v : vs)
                {
                    if (v->GetSpellProto()->SpellFamilyName == SPELLFAMILY_SHAMAN &&
                            (v->GetSpellProto()->SpellFamilyFlags & uint64(0x0000002000000000)))
                    {
                        uint32 spell = v->GetSpellProto()->EffectTriggerSpell[v->GetEffIndex()];
                        CastSpell(this, spell, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                        return SPELL_AURA_PROC_OK;
                    }
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Lightning Overload
            if (dummySpell->SpellIconID == 2018)            // only this spell have SpellFamily Shaman SpellIconID == 2018 and dummy aura
            {
                if (!spellInfo || GetTypeId() != TYPEID_PLAYER || !pVictim)
                    return SPELL_AURA_PROC_FAILED;

                // custom cooldown processing case
                if (cooldown && !IsSpellReady(*dummySpell))
                    return SPELL_AURA_PROC_FAILED;

                uint32 spellId;
                // Every Lightning Bolt and Chain Lightning spell have duplicate vs half damage and zero cost
                switch (spellInfo->Id)
                {
                    // Lightning Bolt
                    case   403: spellId = 45284; break;     // Rank  1
                    case   529: spellId = 45286; break;     // Rank  2
                    case   548: spellId = 45287; break;     // Rank  3
                    case   915: spellId = 45288; break;     // Rank  4
                    case   943: spellId = 45289; break;     // Rank  5
                    case  6041: spellId = 45290; break;     // Rank  6
                    case 10391: spellId = 45291; break;     // Rank  7
                    case 10392: spellId = 45292; break;     // Rank  8
                    case 15207: spellId = 45293; break;     // Rank  9
                    case 15208: spellId = 45294; break;     // Rank 10
                    case 25448: spellId = 45295; break;     // Rank 11
                    case 25449: spellId = 45296; break;     // Rank 12
                    case 49237: spellId = 49239; break;     // Rank 13
                    case 49238: spellId = 49240; break;     // Rank 14
                    // Chain Lightning
                    case   421: spellId = 45297; break;     // Rank  1
                    case   930: spellId = 45298; break;     // Rank  2
                    case  2860: spellId = 45299; break;     // Rank  3
                    case 10605: spellId = 45300; break;     // Rank  4
                    case 25439: spellId = 45301; break;     // Rank  5
                    case 25442: spellId = 45302; break;     // Rank  6
                    case 49270: spellId = 49268; break;     // Rank  7
                    case 49271: spellId = 49269; break;     // Rank  8
                    default:
                        sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (LO)", spellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }

                // Remove cooldown (Chain Lightning - have Category Recovery time)
                if (spellInfo->SpellFamilyFlags & uint64(0x0000000000000002))
                    RemoveSpellCooldown(spellId);

                CastSpell(pVictim, spellId, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);

                if (cooldown)
                    AddCooldown(*dummySpell, nullptr, false, cooldown * IN_MILLISECONDS);

                return SPELL_AURA_PROC_OK;
            }
            // Static Shock
            if (dummySpell->SpellIconID == 3059)
            {
                // lookup Lightning Shield
                AuraList const& vs = GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                for (auto v : vs)
                {
                    if (v->GetSpellProto()->SpellFamilyName == SPELLFAMILY_SHAMAN &&
                            (v->GetSpellProto()->SpellFamilyFlags & uint64(0x0000000000000400)))
                    {
                        uint32 spell;
                        switch (v->GetId())
                        {
                            case   324: spell = 26364; break;
                            case   325: spell = 26365; break;
                            case   905: spell = 26366; break;
                            case   945: spell = 26367; break;
                            case  8134: spell = 26369; break;
                            case 10431: spell = 26370; break;
                            case 10432: spell = 26363; break;
                            case 25469: spell = 26371; break;
                            case 25472: spell = 26372; break;
                            case 49280: spell = 49278; break;
                            case 49281: spell = 49279; break;
                            default:
                                return SPELL_AURA_PROC_FAILED;
                        }
                        CastSpell(target, spell, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                        if (v->GetHolder()->DropAuraCharge())
                            RemoveAuraHolderFromStack(v->GetId());
                        return SPELL_AURA_PROC_OK;
                    }
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Frozen Power
            if (dummySpell->SpellIconID == 3780)
            {
                Unit* caster = triggeredByAura->GetCaster();

                if (!spellInfo || !caster)
                    return SPELL_AURA_PROC_FAILED;

                float distance = caster->GetDistance(pVictim);
                int32 chance = triggerAmount;

                if (distance < 15.0f || !roll_chance_i(chance))
                    return SPELL_AURA_PROC_FAILED;

                // make triggered cast apply after current damage spell processing for prevent remove by it
                if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    spell->AddTriggeredSpell(63685);
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Butchery
            if (dummySpell->SpellIconID == 2664)
            {
                basepoints[0] = triggerAmount;
                triggered_spell_id = 50163;
                target = this;
                break;
            }
            // Dancing Rune Weapon
            if (dummySpell->Id == 49028)
            {
                // 1 dummy aura for dismiss rune blade
                if (effIndex != EFFECT_INDEX_2)
                    return SPELL_AURA_PROC_FAILED;
                // TODO: wite script for this "fights on its own, doing the same attacks"
                // NOTE: Trigger here on every attack and spell cast
                return SPELL_AURA_PROC_FAILED;
            }
            // Mark of Blood
            if (dummySpell->Id == 49005)
            {
                // TODO: need more info (cooldowns/PPM)
                triggered_spell_id = 61607;
                break;
            }
            // Vendetta
            if (dummySpell->SpellFamilyFlags & uint64(0x0000000000010000))
            {
                basepoints[0] = triggerAmount * GetMaxHealth() / 100;
                triggered_spell_id = 50181;
                target = this;
                break;
            }
            // Necrosis
            if (dummySpell->SpellIconID == 2709)
            {
                // only melee auto attack affected and Rune Strike
                if (spellInfo && spellInfo->Id != 56815)
                    return SPELL_AURA_PROC_FAILED;

                basepoints[0] = triggerAmount * damage / 100;
                triggered_spell_id = 51460;
                break;
            }
            // Threat of Thassarian
            if (dummySpell->SpellIconID == 2023)
            {
                // Must Dual Wield
                if (!spellInfo || !hasOffhandWeaponForAttack())
                    return SPELL_AURA_PROC_FAILED;
                // Chance as basepoints for dummy aura
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                switch (spellInfo->Id)
                {
                    // Obliterate
                    case 49020:                             // Rank 1
                        triggered_spell_id = 66198; break;
                    case 51423:                             // Rank 2
                        triggered_spell_id = 66972; break;
                    case 51424:                             // Rank 3
                        triggered_spell_id = 66973; break;
                    case 51425:                             // Rank 4
                        triggered_spell_id = 66974; break;
                    // Frost Strike
                    case 49143:                             // Rank 1
                        triggered_spell_id = 66196; break;
                    case 51416:                             // Rank 2
                        triggered_spell_id = 66958; break;
                    case 51417:                             // Rank 3
                        triggered_spell_id = 66959; break;
                    case 51418:                             // Rank 4
                        triggered_spell_id = 66960; break;
                    case 51419:                             // Rank 5
                        triggered_spell_id = 66961; break;
                    case 55268:                             // Rank 6
                        triggered_spell_id = 66962; break;
                    // Plague Strike
                    case 45462:                             // Rank 1
                        triggered_spell_id = 66216; break;
                    case 49917:                             // Rank 2
                        triggered_spell_id = 66988; break;
                    case 49918:                             // Rank 3
                        triggered_spell_id = 66989; break;
                    case 49919:                             // Rank 4
                        triggered_spell_id = 66990; break;
                    case 49920:                             // Rank 5
                        triggered_spell_id = 66991; break;
                    case 49921:                             // Rank 6
                        triggered_spell_id = 66992; break;
                    // Death Strike
                    case 49998:                             // Rank 1
                        triggered_spell_id = 66188; break;
                    case 49999:                             // Rank 2
                        triggered_spell_id = 66950; break;
                    case 45463:                             // Rank 3
                        triggered_spell_id = 66951; break;
                    case 49923:                             // Rank 4
                        triggered_spell_id = 66952; break;
                    case 49924:                             // Rank 5
                        triggered_spell_id = 66953; break;
                    // Rune Strike
                    case 56815:
                        triggered_spell_id = 66217; break;
                    // Blood Strike
                    case 45902:                             // Rank 1
                        triggered_spell_id = 66215; break;
                    case 49926:                             // Rank 2
                        triggered_spell_id = 66975; break;
                    case 49927:                             // Rank 3
                        triggered_spell_id = 66976; break;
                    case 49928:                             // Rank 4
                        triggered_spell_id = 66977; break;
                    case 49929:                             // Rank 5
                        triggered_spell_id = 66978; break;
                    case 49930:                             // Rank 6
                        triggered_spell_id = 66979; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Runic Power Back on Snare/Root
            if (dummySpell->Id == 61257)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells
                if (!spellInfo || !(procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) || this == pVictim)
                    return SPELL_AURA_PROC_FAILED;
                // Need snare or root mechanic
                if (!(GetAllSpellMechanicMask(spellInfo) & IMMUNE_TO_ROOT_AND_SNARE_MASK))
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = 61258;
                target = this;
                break;
            }
            // Wandering Plague
            if (dummySpell->SpellIconID == 1614)
            {
                if (!roll_chance_f(CalculateEffectiveCritChance(pVictim, BASE_ATTACK)))
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = triggerAmount * damage / 100;
                triggered_spell_id = 50526;
                break;
            }
            // Blood-Caked Blade
            if (dummySpell->SpellIconID == 138)
            {
                // only main hand melee auto attack affected and Rune Strike
                if ((procFlags & PROC_FLAG_OFF_HAND_WEAPON_SWING) || (spellInfo && spellInfo->Id != 56815))
                    return SPELL_AURA_PROC_FAILED;

                // triggered_spell_id in spell data
                break;
            }
            break;
        }
        default:
            break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    return TriggerProccedSpell(target, basepoints, triggered_spell_id, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleProcTriggerSpellAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 procFlags = data.procFlags; uint32 procEx = data.procExtra; uint32 cooldown = data.cooldown;
    // Get triggered aura spell info
    SpellEntry const* auraSpellInfo = triggeredByAura->GetSpellProto();

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = data.triggeredSpellId;
    Unit* target = trigger_spell_id ? data.triggerTarget : nullptr;
    if (!trigger_spell_id)
        trigger_spell_id = auraSpellInfo->EffectTriggerSpell[triggeredByAura->GetEffIndex()];
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;

    if (triggeredByAura->GetModifier()->m_auraname == SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE)
        basepoints[0] = triggerAmount;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    // Try handle unknown trigger spells
    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (auraSpellInfo->Id)
            {
                // case 191:                            // Elemental Response
                //    switch (spellInfo->School)
                //    {
                //        case SPELL_SCHOOL_FIRE:  trigger_spell_id = 34192; break;
                //        case SPELL_SCHOOL_FROST: trigger_spell_id = 34193; break;
                //        case SPELL_SCHOOL_ARCANE:trigger_spell_id = 34194; break;
                //        case SPELL_SCHOOL_NATURE:trigger_spell_id = 34195; break;
                //        case SPELL_SCHOOL_SHADOW:trigger_spell_id = 34196; break;
                //        case SPELL_SCHOOL_HOLY:  trigger_spell_id = 34197; break;
                //        case SPELL_SCHOOL_NORMAL:trigger_spell_id = 34198; break;
                //    }
                //    break;
                // case 5301:  break;                   // Defensive State (DND)
                // case 7137:  break:                   // Shadow Charge (Rank 1)
                // case 7377:  break:                   // Take Immune Periodic Damage <Not Working>
                // case 13358: break;                   // Defensive State (DND)
                // case 16092: break;                   // Defensive State (DND)
                // case 18943: break;                   // Double Attack
                // case 19194: break;                   // Double Attack
                // case 19817: break;                   // Double Attack
                // case 19818: break;                   // Double Attack
                // case 22835: break;                   // Drunken Rage
                //    trigger_spell_id = 14822; break;
                case 23780:                                 // Aegis of Preservation (Aegis of Preservation trinket)
                    trigger_spell_id = 23781;
                    break;
                // case 24949: break;                   // Defensive State 2 (DND)
                case 31073:                                // Conflagrate Self Proc
                {
                    if (spellInfo->SchoolMask != SPELL_SCHOOL_MASK_FIRE)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 27522:                                 // Mana Drain Trigger
                case 40336:                                 // Mana Drain Trigger
                case 46939:                                 // Black Bow of the Betrayer
                    // On successful melee or ranged attack gain 8 mana and if possible drain 8 mana from the target.
                    if (IsAlive())
                        CastSpell(this, 29471, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                    if (pVictim && pVictim->IsAlive())
                        CastSpell(pVictim, 27526, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                case 31255:                                 // Deadly Swiftness (Rank 1)
                {
                    // whenever you deal damage to a target who is below 20% health.
                    if (pVictim->GetHealth() > pVictim->GetMaxHealth() / 5)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    trigger_spell_id = 22588;
                    break;
                }
                // case 33207: break;                       // Gossip NPC Periodic - Fidget
                case 33896:                                 // Desperate Defense (Stonescythe Whelp, Stonescythe Alpha, Stonescythe Ambusher)
                    trigger_spell_id = 33898;
                    break;
                // case 34082: break;                   // Advantaged State (DND)
                // case 34783: break:                   // Spell Reflection
                // case 35205: break:                   // Vanish
                // case 35321: break;                   // Gushing Wound
                case 36096:                             // Spell Reflection
                    return SPELL_AURA_PROC_OK;          // Missing Trigger spell with no evidence to tell what to trigger, need to return to trigger consumption
                // case 36207: break:                   // Steal Weapon
                // case 36576: break:                   // Shaleskin (Shaleskin Flayer, Shaleskin Ripper) 30023 trigger
                // case 37030: break;                   // Chaotic Temperament
                case 38164:                                 // Unyielding Knights - this can only proc in hellfire peninsula with a maximum of 2 guardians against fel orc faction only
                    if (GetZoneId() != 3483 || pVictim->GetFaction() != 943 || CountGuardiansWithEntry(20117) == 2)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                // case 38363: break;                   // Gushing Wound
                // case 39215: break;                   // Gushing Wound
                case 39832:                             // Light of the Naaru - proc only hits demons near Black Temple entrance
                    switch (pVictim->GetEntry())
                    {
                        case 21166:
                        case 21768:
                        case 22857:
                        case 22858:
                        case 22859:
                        case 22860:
                        case 22904:
                        case 22988:
                        case 23044:
                        case 23152:
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                // case 40329: break;                   // Demo Shout Sensor
                // case 40364: break;                   // Entangling Roots Sensor
                // case 41054: break;                   // Copy Weapon
                //    trigger_spell_id = 41055; break;
                // case 41248: break;                   // Consuming Strikes
                //    trigger_spell_id = 41249; break;
                // case 42730: break:                   // Woe Strike
                // case 43453: break:                   // Rune Ward
                // case 43504: break;                   // Alterac Valley OnKill Proc Aura
                case 43820:                                 // Charm of the Witch Doctor (Amani Charm of the Witch Doctor trinket)
                    // Pct value stored in dummy
                    basepoints[0] = pVictim->GetCreateHealth() * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100;
                    target = pVictim;
                    break;
                // case 44326: break:                   // Pure Energy Passive
                // case 44526: break;                   // Hate Monster (Spar) (30 sec)
                // case 44527: break;                   // Hate Monster (Spar Buddy) (30 sec)
                // case 44819: break;                   // Hate Monster (Spar Buddy) (>30% Health)
                // case 44820: break;                   // Hate Monster (Spar) (<30%)
                case 45057:                                 // Evasive Maneuvers (Commendation of Kael`thas trinket)
                    // reduce you below $s1% health (in fact in this specific case can proc from any attack while health in result less $s1%)
                    if (int32(GetHealth()) - int32(damage) >= int32(GetMaxHealth() * triggerAmount / 100))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                // case 45903: break:                   // Offensive State
                // case 46146: break:                   // [PH] Ahune  Spanky Hands
                // case 45205: break;                   // Copy Offhand Weapon
                // case 45343: break;                   // Dark Flame Aura
                case 45396:                         // Blessed Weapon Coating
                case 45398:                         // Righteous Weapon Coating
                {
                    uint32 zoneId = GetZoneId();
                    if (zoneId != 4075 && zoneId != 4080 && zoneId != 4131)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 48473:                                 // Capture Soul - Doom Lord Kazzak
                    if (pVictim->GetTypeId() != TYPEID_PLAYER) // only player death procs
                        return SPELL_AURA_PROC_FAILED;
                    if (Player* lootRecipient = ((Creature*)this)->GetLootRecipient()) // only same team as the one that tagged procs
                        if (lootRecipient->GetTeam() != ((Player*)pVictim)->GetTeam()) // prevents horde/alliance griefing
                            return SPELL_AURA_PROC_FAILED;
                    break;
                // case 48876: break;                   // Beast's Mark
                //    trigger_spell_id = 48877; break;
                // case 49059: break;                   // Horde, Hate Monster (Spar Buddy) (>30% Health)
                // case 50051: break;                   // Ethereal Pet Aura
                // case 50689: break;                   // Blood Presence (Rank 1)
                // case 50844: break;                   // Blood Mirror
                // case 52856: break;                   // Charge
                // case 54072: break;                   // Knockback Ball Passive
                // case 54476: break;                   // Blood Presence
                // case 54775: break;                   // Abandon Vehicle on Poly
                case 57345:                                 // Darkmoon Card: Greatness
                {
                    float stat = 0.0f;
                    // strength
                    if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 60229; stat = GetStat(STAT_STRENGTH); }
                    // agility
                    if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 60233; stat = GetStat(STAT_AGILITY);  }
                    // intellect
                    if (GetStat(STAT_INTELLECT) > stat) { trigger_spell_id = 60234; stat = GetStat(STAT_INTELLECT);}
                    // spirit
                    if (GetStat(STAT_SPIRIT)   > stat) { trigger_spell_id = 60235;                               }
                    break;
                }
                // case 55580: break:                       // Mana Link
                // case 57587: break:                       // Steal Ranged ()
                // case 57594: break;                       // Copy Ranged Weapon
                // case 59237: break;                       // Beast's Mark
                //    trigger_spell_id = 59233; break;
                // case 59288: break;                       // Infra-Green Shield
                // case 59532: break;                       // Abandon Passengers on Poly
                // case 59735: break:                       // Woe Strike
                case 64148:                                 // Diminsh Power
                {
                    if (Unit* caster = triggeredByAura->GetCaster())
                        caster->InterruptNonMeleeSpells(false);
                    return SPELL_AURA_PROC_OK;
                }
                case 64415:                                 // // Val'anyr Hammer of Ancient Kings - Equip Effect
                {
                    // for DOT procs
                    if (!IsPositiveSpell(spellInfo->Id))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 64440:                                 // Blade Warding
                {
                    trigger_spell_id = 64442;

                    // need scale damage base at stack size
                    if (SpellEntry const* trigEntry = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id))
                        basepoints[EFFECT_INDEX_0] = trigEntry->CalculateSimpleValue(EFFECT_INDEX_0) * triggeredByAura->GetStackAmount();

                    break;
                }
                case 64568:                                 // Blood Reserve
                {
                    // When your health drops below 35% ....
                    int32 health35 = int32(GetMaxHealth() * 35 / 100);
                    if (int32(GetHealth()) - int32(damage) >= health35 || int32(GetHealth()) < health35)
                        return SPELL_AURA_PROC_FAILED;

                    trigger_spell_id = 64569;

                    // need scale damage base at stack size
                    if (SpellEntry const* trigEntry = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id))
                        basepoints[EFFECT_INDEX_0] = trigEntry->CalculateSimpleValue(EFFECT_INDEX_0) * triggeredByAura->GetStackAmount();

                    break;
                }
                case 67702:                                 // Death's Choice, Item - Coliseum 25 Normal Melee Trinket
                {
                    float stat = 0.0f;
                    // strength
                    if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 67708; stat = GetStat(STAT_STRENGTH); }
                    // agility
                    if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 67703;                               }
                    break;
                }
                case 67771:                                 // Death's Choice (heroic), Item - Coliseum 25 Heroic Melee Trinket
                {
                    float stat = 0.0f;
                    // strength
                    if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 67773; stat = GetStat(STAT_STRENGTH); }
                    // agility
                    if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 67772;                               }
                    break;
                }
                case 69023:                                 // Mirrored Soul
                {
                    int32 basepoints2 = (int32)(damage * 0.45f);
                    if (Unit* caster = triggeredByAura->GetCaster())
                        // Actually this spell should be sent with SMSG_SPELL_START
                        CastCustomSpell(caster, 69034, &basepoints2, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura, GetObjectGuid());

                    return SPELL_AURA_PROC_OK;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            if (auraSpellInfo->SpellIconID == 2127)         // Blazing Speed
            {
                switch (auraSpellInfo->Id)
                {
                    case 31641:  // Rank 1
                    case 31642:  // Rank 2
                        trigger_spell_id = 31643;
                        break;
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u miss possibly Blazing Speed", auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            else if (auraSpellInfo->Id == 26467)            // Persistent Shield (Scarab Brooch trinket)
            {
                // This spell originally trigger 13567 - Dummy Trigger (vs dummy effect)
                basepoints[0] = damage * 15 / 100;
                target = pVictim;
                trigger_spell_id = 26470;
            }
            else if (auraSpellInfo->Id == 71761)            // Deep Freeze Immunity State
            {
                // spell applied only to permanent immunes to stun targets (bosses)
                if (pVictim->GetTypeId() != TYPEID_UNIT ||
                        (((Creature*)pVictim)->GetCreatureInfo()->MechanicImmuneMask & (1 << (MECHANIC_STUN - 1))) == 0)
                    return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->SpellIconID == 2947)     // Fingers of Frost
            {
                // proc chance for spells in basepoints
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Deep Wounds (replace triggered spells to directly apply DoT), dot spell have familyflags
            if (auraSpellInfo->SpellFamilyFlags.Empty() && auraSpellInfo->SpellIconID == 243)
            {
                float weaponDamage;
                // DW should benefit of attack power, damage percent mods etc.
                if (hasOffhandWeaponForAttack() && getAttackTimer(BASE_ATTACK) > getAttackTimer(OFF_ATTACK))
                    weaponDamage = (GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE)) / 2;
                else
                    weaponDamage = (GetFloatValue(UNIT_FIELD_MINDAMAGE) + GetFloatValue(UNIT_FIELD_MAXDAMAGE)) / 2;

                switch (auraSpellInfo->Id)
                {
                    case 12834: basepoints[EFFECT_INDEX_0] = int32(weaponDamage * 16 / 100); break;
                    case 12849: basepoints[EFFECT_INDEX_0] = int32(weaponDamage * 32 / 100); break;
                    case 12867: basepoints[EFFECT_INDEX_0] = int32(weaponDamage * 48 / 100); break;
                    // Impossible case
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: DW unknown spell rank %u", auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }

                // 1 tick/sec * 6 sec = 6 ticks
                basepoints[EFFECT_INDEX_0] /= 6;

                trigger_spell_id = 12721;
                break;
            }
            if (auraSpellInfo->SpellIconID == 2961)    // Taste for Blood
            {
                // only at real damage
                if (!damage)
                    return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->Id == 50421)            // Scent of Blood
            {
                RemoveAuraHolderFromStack(50421);
                trigger_spell_id = 50422;
                target = this;
                break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Drain Soul
            if (auraSpellInfo->IsFitToFamilyMask(uint64(0x0000000000004000)))
            {
                // search for "Improved Drain Soul" dummy aura
                Unit::AuraList const& mDummyAura = GetAurasByType(SPELL_AURA_DUMMY);
                for (auto i : mDummyAura)
                {
                    if (i->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && i->GetSpellProto()->SpellIconID == 113)
                    {
                        // basepoints of trigger spell stored in dummyeffect of spellProto
                        basepoints[EFFECT_INDEX_0] = GetMaxPower(POWER_MANA) * i->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2) / 100;
                        trigger_spell_id = 18371;
                        break;
                    }
                }
            }
            // Consume Shadows
            else if (auraSpellInfo->IsFitToFamilyMask(uint64(0x0000000002000000)))
            {
                Aura* heal = triggeredByAura->GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_0);
                if (!heal || heal->GetAuraTicks() > 1)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Nether Protection
            else if (auraSpellInfo->SpellIconID == 1985)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;

                switch (GetFirstSchoolInMask(GetSpellSchoolMask(spellInfo)))
                {
                    case SPELL_SCHOOL_HOLY:   trigger_spell_id = 54370; break;
                    case SPELL_SCHOOL_FIRE:   trigger_spell_id = 54371; break;
                    case SPELL_SCHOOL_NATURE: trigger_spell_id = 54375; break;
                    case SPELL_SCHOOL_FROST:  trigger_spell_id = 54372; break;
                    case SPELL_SCHOOL_SHADOW: trigger_spell_id = 54374; break;
                    case SPELL_SCHOOL_ARCANE: trigger_spell_id = 54373; break;
                    case SPELL_SCHOOL_NORMAL:               // ignore
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Cheat Death
            else if (auraSpellInfo->Id == 28845)
            {
                // When your health drops below 20% ....
                int32 health20 = int32(GetMaxHealth()) / 5;
                if (int32(GetHealth()) - int32(damage) >= health20 || int32(GetHealth()) < health20)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Decimation
            else if (auraSpellInfo->Id == 63156 || auraSpellInfo->Id == 63158)
            {
                // Looking for dummy effect
                Aura* aur = GetAura(auraSpellInfo->Id, EFFECT_INDEX_1);
                if (!aur)
                    return SPELL_AURA_PROC_FAILED;

                // If target's health is not below equal certain value (35%) not proc
                if (int32(pVictim->GetHealth() * 100 / pVictim->GetMaxHealth()) > aur->GetModifier()->m_amount)
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            if (auraSpellInfo->SpellIconID == 1875)
            {
                switch (auraSpellInfo->Id)
                {
                    case 27811: trigger_spell_id = 27813; break;
                    case 27815: trigger_spell_id = 27817; break;
                    case 27816: trigger_spell_id = 27818; break;
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u not handled in BR", auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
                basepoints[0] = damage * triggerAmount / 100 / 3;
                target = this;
                // increase healing factor with each critical strike. Patch 3.0.2
                if (Aura* old_aura = GetAura(trigger_spell_id, EFFECT_INDEX_0))
                    basepoints[0] += old_aura->GetModifier()->m_amount;
            }
            // Glyph of Shadow Word: Pain
            else if (auraSpellInfo->Id == 55681)
                basepoints[0] = triggerAmount * GetCreateMana() / 100;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Druid Forms Trinket
            if (auraSpellInfo->Id == 37336)
            {
                switch (GetShapeshiftForm())
                {
                    case FORM_NONE:     trigger_spell_id = 37344; break;
                    case FORM_CAT:      trigger_spell_id = 37341; break;
                    case FORM_BEAR:
                    case FORM_DIREBEAR: trigger_spell_id = 37340; break;
                    case FORM_TREE:     trigger_spell_id = 37342; break;
                    case FORM_MOONKIN:  trigger_spell_id = 37343; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
            else if (auraSpellInfo->Id == 67353)
            {
                switch (GetShapeshiftForm())
                {
                    case FORM_CAT:      trigger_spell_id = 67355; break;
                    case FORM_BEAR:
                    case FORM_DIREBEAR: trigger_spell_id = 67354; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            if (auraSpellInfo->SpellIconID == 2260)         // Combat Potency
            {
                if (!(procFlags & PROC_FLAG_OFF_HAND_WEAPON_SWING))
                    return SPELL_AURA_PROC_FAILED;
            }

            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Piercing Shots
            if (auraSpellInfo->SpellIconID == 3247 && auraSpellInfo->SpellVisual[0] == 0)
            {
                basepoints[0] = damage * triggerAmount / 100 / 8;
                trigger_spell_id = 63468;
                target = pVictim;
            }
            // Rapid Recuperation
            else if (auraSpellInfo->Id == 53228 || auraSpellInfo->Id == 53232)
            {
                // This effect only from Rapid Fire (ability cast)
                if (!(spellInfo->SpellFamilyFlags & uint64(0x0000000000000020)))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Lock and Load
            else if (auraSpellInfo->SpellIconID == 3579)
            {
                // Check for Lock and Load Marker
                if (HasAura(67544))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Item - Hunter T9 4P Bonus
            else if (auraSpellInfo->Id == 67151)
            {
                trigger_spell_id = 68130;
                break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            /*
            // Blessed Life
            if (auraSpellInfo->SpellIconID == 2137)
            {
                switch (auraSpellInfo->Id)
                {
                    case 31828:                         // Rank 1
                    case 31829:                         // Rank 2
                    case 31830:                         // Rank 3
                        break;
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u miss posibly Blessed Life", auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            */
            // Soul Preserver
            if (auraSpellInfo->Id == 60510)
            {
                trigger_spell_id = 60515;
                target = this;
            }
            // Illumination
            else if (auraSpellInfo->SpellIconID == 241)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;
                // procspell is triggered spell but we need mana cost of original casted spell
                uint32 originalSpellId = spellInfo->Id;
                // Holy Shock heal
                if (spellInfo->SpellFamilyFlags & uint64(0x0001000000000000))
                {
                    switch (spellInfo->Id)
                    {
                        case 25914: originalSpellId = 20473; break;
                        case 25913: originalSpellId = 20929; break;
                        case 25903: originalSpellId = 20930; break;
                        case 27175: originalSpellId = 27174; break;
                        case 33074: originalSpellId = 33072; break;
                        case 48820: originalSpellId = 48824; break;
                        case 48821: originalSpellId = 48825; break;
                        default:
                            sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u not handled in HShock", spellInfo->Id);
                            return SPELL_AURA_PROC_FAILED;
                    }
                }
                SpellEntry const* originalSpell = sSpellTemplate.LookupEntry<SpellEntry>(originalSpellId);
                if (!originalSpell)
                {
                    sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u unknown but selected as original in Illu", originalSpellId);
                    return SPELL_AURA_PROC_FAILED;
                }
                // percent stored in effect 1 (class scripts) base points
                int32 cost = originalSpell->manaCost + originalSpell->ManaCostPercentage * GetCreateMana() / 100;
                basepoints[0] = cost * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100;
                trigger_spell_id = 20272;
                target = this;
            }
            // Lightning Capacitor
            else if (auraSpellInfo->Id == 37657)
            {
                if (!pVictim || !pVictim->IsAlive())
                    return SPELL_AURA_PROC_FAILED;
                // stacking
                CastSpell(this, 37658, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);

                Aura* dummy = GetDummyAura(37658);
                // release at 3 aura in stack (cont contain in basepoint of trigger aura)
                if (!dummy || dummy->GetStackAmount() < uint32(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                RemoveAurasDueToSpell(37658);
                trigger_spell_id = 37661;
                target = pVictim;
            }
            // Bonus Healing (Crystal Spire of Karabor mace)
            else if (auraSpellInfo->Id == 40971)
            {
                // If your target is below $s1% health
                if (pVictim->GetHealth() - damage > pVictim->GetMaxHealth() * triggerAmount / 100)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Thunder Capacitor
            else if (auraSpellInfo->Id == 54841)
            {
                if (!pVictim || !pVictim->IsAlive())
                    return SPELL_AURA_PROC_FAILED;
                // stacking
                CastSpell(this, 54842, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);

                // counting
                Aura* dummy = GetDummyAura(54842);
                // release at 3 aura in stack (cont contain in basepoint of trigger aura)
                if (!dummy || dummy->GetStackAmount() < uint32(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                RemoveAurasDueToSpell(54842);
                trigger_spell_id = 54843;
                target = pVictim;
            }
            // Item - Coliseum 25 Normal and Heroic Caster Trinket
            else if (auraSpellInfo->Id == 67712 || auraSpellInfo->Id == 67758)
            {
                if (!pVictim || !pVictim->IsAlive())
                    return SPELL_AURA_PROC_FAILED;

                uint32 castSpell = auraSpellInfo->Id == 67758 ? 67759 : 67713;

                // stacking
                CastSpell(this, castSpell, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);

                // counting
                Aura* dummy = GetDummyAura(castSpell);

                // release at 3 aura in stack (count contained in basepoint of trigger aura)
                if (!dummy || dummy->GetStackAmount() < uint32(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                RemoveAurasDueToSpell(castSpell);
                trigger_spell_id = castSpell + 1;
                target = pVictim;
                break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Lightning Shield (overwrite non existing triggered spell call in spell.dbc
            if (auraSpellInfo->SpellFamilyFlags & uint64(0x0000000000000400))
            {
                switch (auraSpellInfo->Id)
                {
                    case 324:                           // Rank 1
                        trigger_spell_id = 26364; break;
                    case 325:                           // Rank 2
                        trigger_spell_id = 26365; break;
                    case 905:                           // Rank 3
                        trigger_spell_id = 26366; break;
                    case 945:                           // Rank 4
                        trigger_spell_id = 26367; break;
                    case 8134:                          // Rank 5
                        trigger_spell_id = 26369; break;
                    case 10431:                         // Rank 6
                        trigger_spell_id = 26370; break;
                    case 10432:                         // Rank 7
                        trigger_spell_id = 26363; break;
                    case 25469:                         // Rank 8
                        trigger_spell_id = 26371; break;
                    case 25472:                         // Rank 9
                        trigger_spell_id = 26372; break;
                    case 49280:                         // Rank 10
                        trigger_spell_id = 49278; break;
                    case 49281:                         // Rank 11
                        trigger_spell_id = 49279; break;
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u not handled in LShield", auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Lightning Shield (The Ten Storms set)
            else if (auraSpellInfo->Id == 23551)
            {
                trigger_spell_id = 23552;
                target = pVictim;
            }
            // Damage from Lightning Shield (The Ten Storms set)
            else if (auraSpellInfo->Id == 23552)
                trigger_spell_id = 27635;
            // Mana Surge (The Earthfury set)
            else if (auraSpellInfo->Id == 23572)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = spellInfo->manaCost * 35 / 100;
                trigger_spell_id = 23571;
                target = this;
            }
            // Nature's Guardian
            else if (auraSpellInfo->SpellIconID == 2013)
            {
                // Check health condition - should drop to less 30% (trigger at any attack with result health less 30%, independent original health state)
                int32 health30 = int32(GetMaxHealth()) * 3 / 10;
                if (int32(GetHealth()) - int32(damage) >= health30)
                    return SPELL_AURA_PROC_FAILED;

                if (pVictim && pVictim->IsAlive())
                    pVictim->getThreatManager().modifyThreatPercent(this, -10);

                basepoints[0] = triggerAmount * GetMaxHealth() / 100;
                trigger_spell_id = 31616;
                target = this;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Acclimation
            if (auraSpellInfo->SpellIconID == 1930)
            {
                if (!spellInfo)
                    return SPELL_AURA_PROC_FAILED;
                switch (GetFirstSchoolInMask(GetSpellSchoolMask(spellInfo)))
                {
                    case SPELL_SCHOOL_NORMAL:
                        return SPELL_AURA_PROC_FAILED;      // ignore
                    case SPELL_SCHOOL_HOLY:   trigger_spell_id = 50490; break;
                    case SPELL_SCHOOL_FIRE:   trigger_spell_id = 50362; break;
                    case SPELL_SCHOOL_NATURE: trigger_spell_id = 50488; break;
                    case SPELL_SCHOOL_FROST:  trigger_spell_id = 50485; break;
                    case SPELL_SCHOOL_SHADOW: trigger_spell_id = 50489; break;
                    case SPELL_SCHOOL_ARCANE: trigger_spell_id = 50486; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Blade Barrier
            else if (auraSpellInfo->SpellIconID == 85)
            {
                if (GetTypeId() != TYPEID_PLAYER || getClass() != CLASS_DEATH_KNIGHT ||
                        !((Player*)this)->IsBaseRuneSlotsOnCooldown(RUNE_BLOOD))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Improved Blood Presence
            else if (auraSpellInfo->Id == 63611)
            {
                if (GetTypeId() != TYPEID_PLAYER || !((Player*)this)->isHonorOrXPTarget(pVictim) || !damage)
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = triggerAmount * damage / 100;
                trigger_spell_id = 50475;
            }
            break;
        }
        default:
            break;
    }

    // Custom basepoints/target for exist spell
    // dummy basepoints or other customs
    switch (trigger_spell_id)
    {
        // Cast positive spell on enemy target
        case 7099:  // Curse of Mending
        case 39647: // Curse of Mending
        case 29494: // Temptation
        case 20233: // Improved Lay on Hands (cast on target)
        {
            target = pVictim;
            break;
        }
        // Combo points add triggers (need add combopoint only for main target, and after possible combopoints reset)
        case 15250: // Rogue Setup
        {
            if (!pVictim || pVictim != GetVictim())  // applied only for main target
                return SPELL_AURA_PROC_FAILED;
            break;                                   // continue normal case
        }
        // Finishing moves that add combo points
        case 14189: // Seal Fate (Netherblade set)
        case 14157: // Ruthlessness
        case 70802: // Mayhem (Shadowblade sets)
        {
            // Need add combopoint AFTER finishing move (or they get dropped in finish phase)
            if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                spell->AddTriggeredSpell(trigger_spell_id);
                return SPELL_AURA_PROC_OK;
            }
            return SPELL_AURA_PROC_FAILED;
        }
        // Bloodthirst (($m/100)% of max health)
        case 23880:
        {
            basepoints[0] = int32(GetMaxHealth() * triggerAmount / 100);
            break;
        }
        // Shamanistic Rage triggered spell
        case 30824:
        {
            basepoints[0] = int32(GetTotalAttackPowerValue(BASE_ATTACK) * triggerAmount / 100);
            break;
        }
        // Enlightenment (trigger only from mana cost spells)
        case 35095:
        {
            if (!spellInfo || spellInfo->powerType != POWER_MANA || (spellInfo->manaCost == 0 && spellInfo->ManaCostPercentage == 0 && spellInfo->manaCostPerlevel == 0))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Demonic Pact
        case 48090:
        {
            // As the spell is proced from pet's attack - find owner
            Unit* owner = GetOwner();
            if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                return SPELL_AURA_PROC_FAILED;

            // This spell doesn't stack, but refreshes duration. So we receive current bonuses to minus them later.
            int32 curBonus = 0;
            if (Aura* aur = owner->GetAura(48090, EFFECT_INDEX_0))
                curBonus = aur->GetModifier()->m_amount;
            int32 spellDamage  = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC) - curBonus;
            if (spellDamage <= 0)
                return SPELL_AURA_PROC_FAILED;

            // percent stored in owner talent dummy
            AuraList const& dummyAuras = owner->GetAurasByType(SPELL_AURA_DUMMY);
            for (auto dummyAura : dummyAuras)
            {
                if (dummyAura->GetSpellProto()->SpellIconID == 3220)
                {
                    basepoints[0] = basepoints[1] = int32(spellDamage * dummyAura->GetModifier()->m_amount / 100);
                    break;
                }
            }
            break;
        }
        // Sword and Board
        case 50227:
        {
            // Remove cooldown on Shield Slam
            if (GetTypeId() == TYPEID_PLAYER)
                RemoveSpellCategoryCooldown(1209, true);
            break;
        }
        // Maelstrom Weapon
        case 53817:
        {
            // have rank dependent proc chance, ignore too often cases
            // PPM = 2.5 * (rank of talent),
            uint32 rank = sSpellMgr.GetSpellRank(auraSpellInfo->Id);
            // 5 rank -> 100% 4 rank -> 80% and etc from full rate
            if (!roll_chance_i(20 * rank))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Brain Freeze
        case 57761:
        {
            if (!spellInfo)
                return SPELL_AURA_PROC_FAILED;
            // For trigger from Blizzard need exist Improved Blizzard
            if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && (spellInfo->SpellFamilyFlags & uint64(0x0000000000000080)))
            {
                bool found = false;
                AuraList const& mOverrideClassScript = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (auto i : mOverrideClassScript)
                {
                    int32 script = i->GetModifier()->m_miscvalue;
                    if (script == 836 || script == 988 || script == 989)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        // Astral Shift
        case 52179:
        {
            if (!spellInfo || !(procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) || this == pVictim)
                return SPELL_AURA_PROC_FAILED;

            // Need stun, fear or silence mechanic
            if (!(GetAllSpellMechanicMask(spellInfo) & IMMUNE_TO_SILENCE_AND_STUN_AND_FEAR_MASK))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Burning Determination
        case 54748:
        {
            if (!spellInfo)
                return SPELL_AURA_PROC_FAILED;
            // Need Interrupt or Silenced mechanic
            if (!(GetAllSpellMechanicMask(spellInfo) & IMMUNE_TO_INTERRUPT_AND_SILENCE_MASK))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Lock and Load
        case 56453:
        {
            // Proc only from trap activation (from periodic proc another aura of this spell)
            if (!(procFlags & PROC_FLAG_ON_TRAP_ACTIVATION) || !roll_chance_i(triggerAmount))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Freezing Fog (Rime triggered)
        case 59052:
        {
            // Howling Blast cooldown reset
            if (GetTypeId() == TYPEID_PLAYER)
                RemoveSpellCategoryCooldown(1248, true);
            break;
        }
        // Druid - Savage Defense
        case 62606:
        {
            basepoints[0] = int32(GetTotalAttackPowerValue(BASE_ATTACK) * triggerAmount / 100);
            break;
        }
    }

    // All ok. Check current trigger spell
    SpellEntry const* triggerEntry = sSpellTemplate.LookupEntry<SpellEntry>(trigger_spell_id);
    if (!triggerEntry)
    {
        // Not cast unknown spell
        // sLog.outError("Unit::HandleProcTriggerSpellAuraProc: Spell %u have 0 in EffectTriggered[%d], not handled custom case?",auraSpellInfo->Id,triggeredByAura->GetEffIndex());
        return SPELL_AURA_PROC_FAILED;
    }

    // try detect target manually if not set
    if (target == nullptr)
        target = !(procFlags & (PROC_FLAG_DEAL_HELPFUL_SPELL | PROC_FLAG_DEAL_HELPFUL_ABILITY)) && IsPositiveSpellTargetMode(triggerEntry, this, pVictim) ? this : pVictim;

    // Quick check for target modes for procs: do not cast offensive procs on friendly targets and in reverse
    if (target != nullptr)
    {
        if (!(procEx & PROC_EX_REFLECT))
        {
            // TODO: add neutral target handling, neutral targets should still be able to go through
            if (!(this == target && IsOnlySelfTargeting(triggerEntry)))
            {
                if (IsPositiveSpellTargetMode(triggerEntry, this, target) != CanAssistSpell(target, triggerEntry))
                    return SPELL_AURA_PROC_FAILED;
            }
        }
    }

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacksExecuting && IsSpellHaveEffect(triggerEntry, SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return SPELL_AURA_PROC_FAILED;

    return TriggerProccedSpell(target, basepoints, triggerEntry, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleProcTriggerDamageAuraProc(ProcExecutionData& data)
{
    Unit* victim = data.target; Aura* triggeredByAura = data.triggeredByAura;
    SpellEntry const* spellInfo = triggeredByAura->GetSpellProto();
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "ProcDamageAndSpell: doing %u damage from spell id %u (triggered by auratype %u of spell %u)",
                     triggeredByAura->GetModifier()->m_amount, spellInfo->Id, triggeredByAura->GetModifier()->m_auraname, triggeredByAura->GetId());
    // Trigger damage can be resisted...
    if (SpellMissInfo missInfo = SpellHitResult(this, victim, spellInfo, uint8(1 << triggeredByAura->GetEffIndex()), false))
    {
        SendSpellDamageResist(victim, spellInfo->Id);
        return SPELL_AURA_PROC_OK;
    }
    SpellNonMeleeDamage spellDamageInfo(this, victim, spellInfo->Id, SpellSchoolMask(spellInfo->SchoolMask));
    CalculateSpellDamage(&spellDamageInfo, triggeredByAura->GetModifier()->m_amount, spellInfo);
    spellDamageInfo.target->CalculateAbsorbResistBlock(this, &spellDamageInfo, spellInfo);
    Unit::DealDamageMods(this, spellDamageInfo.target, spellDamageInfo.damage, &spellDamageInfo.absorb, SPELL_DIRECT_DAMAGE);
    SendSpellNonMeleeDamageLog(&spellDamageInfo);
    DealSpellDamage(this, &spellDamageInfo, true, false);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleOverrideClassScriptAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 cooldown = data.cooldown;
    int32 scriptId = triggeredByAura->GetModifier()->m_miscvalue;

    if (!pVictim || !pVictim->IsAlive())
        return SPELL_AURA_PROC_FAILED;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    uint32 triggered_spell_id = data.triggeredSpellId;
    Unit* target = triggered_spell_id ? data.triggerTarget : pVictim;
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;

    switch (scriptId)
    {
        case 836:                                           // Improved Blizzard (Rank 1)
        {
            if (!spellInfo || spellInfo->SpellVisual[0] != 9487)
                return SPELL_AURA_PROC_FAILED;
            triggered_spell_id = 12484;
            break;
        }
        case 988:                                           // Improved Blizzard (Rank 2)
        {
            if (!spellInfo || spellInfo->SpellVisual[0] != 9487)
                return SPELL_AURA_PROC_FAILED;
            triggered_spell_id = 12485;
            break;
        }
        case 989:                                           // Improved Blizzard (Rank 3)
        {
            if (!spellInfo || spellInfo->SpellVisual[0] != 9487)
                return SPELL_AURA_PROC_FAILED;
            triggered_spell_id = 12486;
            break;
        }
        case 3656:                                          // Corrupted Healing (Priest class call in Nefarian encounter)
        {
            // Procced spell can only be triggered by direct heals
            // Heal over time like Renew does not trigger it
            // Check that only priest class can proc it is done in Spell::CheckTargetScript() for aura 23401
            if (IsSpellHaveEffect(spellInfo, SPELL_EFFECT_HEAL))
                triggered_spell_id = 23402;
            break;
        }
        case 4086:                                          // Improved Mend Pet (Rank 1)
        case 4087:                                          // Improved Mend Pet (Rank 2)
        {
            if (!roll_chance_i(triggerAmount))
                return SPELL_AURA_PROC_FAILED;

            triggered_spell_id = 24406;
            break;
        }
        case 4533:                                          // Dreamwalker Raiment 2 pieces bonus
        {
            // Chance 50%
            if (!roll_chance_i(50))
                return SPELL_AURA_PROC_FAILED;

            switch (pVictim->GetPowerType())
            {
                case POWER_MANA:   triggered_spell_id = 28722; break;
                case POWER_RAGE:   triggered_spell_id = 28723; break;
                case POWER_ENERGY: triggered_spell_id = 28724; break;
                default:
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        case 4537:                                          // Dreamwalker Raiment 6 pieces bonus - Regrowth
            triggered_spell_id = 28750;                     // Blessing of the Claw
            break;
        case 5497:                                          // Improved Mana Gems (Serpent-Coil Braid)
            CastSpell(pVictim, 37445, TRIGGERED_NONE); // Mana Surge                   
            return SPELL_AURA_PROC_OK;
        case 5510:                                          // Flexibility - T4 Holy Priest bonus
            RemoveAurasDueToSpell(37565);
            return SPELL_AURA_PROC_OK;
        case 6953:                                          // Warbringer
            RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK, 0, true);
            return SPELL_AURA_PROC_OK;
        case 7010:                                          // Revitalize (rank 1)
        case 7011:                                          // Revitalize (rank 2)
        case 7012:                                          // Revitalize (rank 3)
        {
            if (!roll_chance_i(triggerAmount))
                return SPELL_AURA_PROC_FAILED;

            switch (pVictim->GetPowerType())
            {
                case POWER_MANA:        triggered_spell_id = 48542; break;
                case POWER_RAGE:        triggered_spell_id = 48541; break;
                case POWER_ENERGY:      triggered_spell_id = 48540; break;
                case POWER_RUNIC_POWER: triggered_spell_id = 48543; break;
                default: return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
    }

    // not processed
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_FAILED;

    return TriggerProccedSpell(target, basepoints, triggered_spell_id, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleRaidProcFromChargeAuraProc(ProcExecutionData& data)
{
    uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura;
    //if no damage then mending proc has failed, we dont need to do unecessary steps
    if (!damage)
        return SPELL_AURA_PROC_FAILED;

    if (data.spell)
    {
        if (data.spell->IsAuraProcced(triggeredByAura))
            return SPELL_AURA_PROC_FAILED;
    }

    // aura can be deleted at casts
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    SpellEffectIndex effIdx = triggeredByAura->GetEffIndex();
    ObjectGuid caster_guid = triggeredByAura->GetCasterGuid();

    uint32 triggeredSpellId = 0;
    uint32 animationSpellId = 0;

    switch (spellProto->Id)
    {
        case 43593:
            triggeredSpellId = 43594;
            animationSpellId = 43613;
            break;
        case 57949:
            triggeredSpellId = 57952;
            animationSpellId = 57951;
            break;
        case 59978:
            triggeredSpellId = 59979;
            animationSpellId = 57951;
            break;
    }

    if (!triggeredSpellId || !animationSpellId)
        return SPELL_AURA_PROC_FAILED;

    if (data.spellInfo && data.spellInfo->Id == triggeredSpellId)
        return SPELL_AURA_PROC_FAILED;

    int32 jumps = triggeredByAura->GetHolder()->GetAuraCharges() - 1; // jumps
    triggeredByAura->GetHolder()->SetAuraCharges(1); // current aura expire - will be removed at next charges decrease

    // next target selection
    if (jumps > 0 && GetTypeId() == TYPEID_PLAYER && caster_guid.IsPlayer())
    {
        float radius;
        if (spellProto->EffectRadiusIndex[effIdx])
            radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellProto->EffectRadiusIndex[effIdx]));
        else
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellProto->rangeIndex));

        if (Unit* caster = triggeredByAura->GetCaster())
        {
            if (Player* target = GetNextRandomRaidMember(radius, SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE))
            {
                // This custom nonsense was kept throughout wotlk
                SpellAuraHolder* holder = GetSpellAuraHolder(spellProto->Id, caster->GetObjectGuid());
                SpellAuraHolder* new_holder = CreateSpellAuraHolder(spellProto, target, caster);

                for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    Aura* aur = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
                    if (!aur)
                        continue;

                    int32 basePoints = aur->GetBasePoints();
                    int32 damage = aur->GetModifier()->m_baseAmount;
                    Aura* new_aur = CreateAura(spellProto, aur->GetEffIndex(), &damage, &basePoints, new_holder, target, caster);
                    new_holder->AddAura(new_aur, new_aur->GetEffIndex());
                }
                new_holder->SetAuraCharges(jumps, false);

                // lock aura holder (currently SPELL_AURA_RAID_PROC_FROM_CHARGE is single target spell, so will attempt removing from old target
                // when applied to new one)
                if (!target->AddSpellAuraHolder(new_holder))
                    delete new_holder;
                else
                    new_holder->SetState(SPELLAURAHOLDER_STATE_READY);

                if (data.spell)
                    data.spell->RegisterAuraProc(new_holder->m_auras[triggeredByAura->GetEffIndex()]);

                CastSpell(target, animationSpellId, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            }
        }
    }

    CastSpell(nullptr, triggeredSpellId, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleRaidProcFromChargeWithValueAuraProc(ProcExecutionData& data)
{
    uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura;
    //if no damage then mending proc has failed, we dont need to do unecessary steps
    if (!damage || data.isHeal)
        return SPELL_AURA_PROC_FAILED;

    if (data.spell)
    {
        if (data.spell->IsAuraProcced(triggeredByAura))
            return SPELL_AURA_PROC_FAILED;
    }

    // aura can be deleted at casts
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    SpellEffectIndex effIdx = triggeredByAura->GetEffIndex();
    int32 heal = triggeredByAura->GetModifier()->m_amount;
    ObjectGuid caster_guid = triggeredByAura->GetCasterGuid();

    // jumps
    int32 jumps = triggeredByAura->GetHolder()->GetAuraCharges() - 1;

    // next target selection
    if (jumps > 0 && GetTypeId() == TYPEID_PLAYER && caster_guid.IsPlayer())
    {
        float radius;
        if (spellProto->EffectRadiusIndex[effIdx])
            radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellProto->EffectRadiusIndex[effIdx]));
        else
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellProto->rangeIndex));

        if (Player* caster = static_cast<Player*>(triggeredByAura->GetCaster()))
        {
            caster->ApplySpellMod(spellProto->Id, SPELLMOD_RADIUS, radius);

            if (Player* target = static_cast<Player*>(this)->GetNextRaidMemberWithLowestLifePercentage(radius, SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE))
            {
                SpellAuraHolder* holder = GetSpellAuraHolder(spellProto->Id, caster->GetObjectGuid());
                SpellAuraHolder* new_holder = CreateSpellAuraHolder(spellProto, target, caster);

                for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    Aura* aur = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
                    if (!aur)
                        continue;

                    int32 basePoints = aur->GetBasePoints();
                    int32 damage = aur->GetModifier()->m_baseAmount;
                    Aura* new_aur = CreateAura(spellProto, aur->GetEffIndex(), &damage, &basePoints, new_holder, target, caster);
                    new_holder->AddAura(new_aur, new_aur->GetEffIndex());
                }
                new_holder->SetAuraCharges(jumps, false);

                // lock aura holder (currently SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE is single target spell, so will attempt removing from old target
                // when applied to new one)
                if (!target->AddSpellAuraHolder(new_holder))
                    delete new_holder;
                else
                    new_holder->SetState(SPELLAURAHOLDER_STATE_READY);

                if (data.spell)
                    data.spell->RegisterAuraProc(new_holder->m_auras[triggeredByAura->GetEffIndex()]);

                CastSpell(target, 41637, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
            }
        }
    }

    // heal
    CastCustomSpell(nullptr, 33110, &heal, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModCastingSpeedNotStackAuraProc(ProcExecutionData& data)
{
    SpellEntry const* spellInfo = data.spellInfo;
    // Skip melee hits or instant cast spells
    return !(spellInfo == nullptr || GetSpellCastTime(spellInfo, this) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleReflectSpellsSchoolAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo;
    // Skip Melee hits and spells ws wrong school
    return !(spellInfo == nullptr || (triggeredByAura->GetModifier()->m_miscvalue & spellInfo->SchoolMask) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleModPowerCostSchoolAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo;
    // Skip melee hits and spells ws wrong school or zero cost
    return !(spellInfo == nullptr ||
             (spellInfo->manaCost == 0 && spellInfo->ManaCostPercentage == 0) ||           // Cost check
             (triggeredByAura->GetModifier()->m_miscvalue & spellInfo->SchoolMask) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;  // School check
}

SpellAuraProcResult Unit::HandleMechanicImmuneResistanceAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo;
    // Compare mechanic
    return !(spellInfo == nullptr || int32(spellInfo->Mechanic) != triggeredByAura->GetModifier()->m_miscvalue)
           ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleModDamageFromCasterAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.victim; Aura* triggeredByAura = data.triggeredByAura;
    // Compare casters
    return triggeredByAura->GetCasterGuid() == pVictim->GetObjectGuid() ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleAddFlatModifierAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.victim; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo;
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();

    if (spellProto->Id == 55166)                             // Tidal Force
    {
        // Remove only single aura from stack
        if (triggeredByAura->GetStackAmount() > 1 && !triggeredByAura->GetHolder()->ModStackAmount(-1, nullptr))
            return SPELL_AURA_PROC_CANT_TRIGGER;
    }
    else if (spellProto->Id == 53695 || spellProto->Id == 53696)   // Judgements of the Just
    {
        if (!spellInfo)
            return SPELL_AURA_PROC_FAILED;

        if (GetSpellSpecific(spellInfo->Id) != SPELL_JUDGEMENT)
            return SPELL_AURA_PROC_FAILED;

        int bp = triggeredByAura->GetModifier()->m_amount;
        CastCustomSpell(pVictim, 68055, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, nullptr, triggeredByAura);
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleAddPctModifierAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 procEx = data.procExtra;
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    switch (spellProto->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // Combustion
            if (spellProto->Id == 11129)
            {
                // last charge and crit
                if (triggeredByAura->GetHolder()->GetAuraCharges() <= 1 && (procEx & PROC_EX_CRITICAL_HIT))
                    return SPELL_AURA_PROC_OK;              // charge counting (will removed)

                CastSpell(this, 28682, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                return (procEx & PROC_EX_CRITICAL_HIT) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED; // charge update only at crit hits, no hidden cooldowns
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Glyph of Divinity
            if (spellProto->Id == 54939)
            {
                // Lookup base amount mana restore
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (spellProto->Effect[i] == SPELL_EFFECT_ENERGIZE)
                    {
                        int32 mana = spellProto->CalculateSimpleValue(SpellEffectIndex(i));
                        CastCustomSpell(this, 54986, nullptr, &mana, nullptr, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
                        break;
                    }
                }
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
    }
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModDamagePercentDoneAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 cooldown = data.cooldown;
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    // Aspect of the Viper
    if (spellProto->SpellFamilyName == SPELLFAMILY_HUNTER && spellProto->SpellFamilyFlags & uint64(0x4000000000000))
    {
        uint32 maxmana = GetMaxPower(POWER_MANA);
        int32 bp = int32(maxmana * GetAttackTime(RANGED_ATTACK) / 1000.0f / 100.0f);

        if (GetPowerPercent() == 100.f)
            return SPELL_AURA_PROC_FAILED;
        if (cooldown && !IsSpellReady(34075))
            return SPELL_AURA_PROC_FAILED;

        CastCustomSpell(nullptr, 34075, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED, castItem, triggeredByAura);
    }
    // Arcane Blast
    else if (spellProto->Id == 36032 && spellProto->SpellFamilyName == SPELLFAMILY_MAGE && spellProto->SpellIconID == 2294)
        // prevent proc from self(spell that triggered this aura)
        return SPELL_AURA_PROC_FAILED;

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModRating(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura;
    SpellEntry const* spellInfo = triggeredByAura->GetSpellProto();

    if (spellInfo->Id == 71564)                             // Deadly Precision
    {
        // Remove only single aura from stack
        if (triggeredByAura->GetStackAmount() > 1 && !triggeredByAura->GetHolder()->ModStackAmount(-1, nullptr))
            return SPELL_AURA_PROC_CANT_TRIGGER;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleMagnetAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura; uint32 procEx = data.procExtra;
    return procEx & PROC_EX_MAGNET && triggeredByAura->IsMagnetUsed() ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleManaShieldAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; Aura* triggeredByAura = data.triggeredByAura; uint32 cooldown = data.cooldown;
    SpellEntry const* dummySpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;
    std::array<int32, MAX_EFFECT_INDEX>& basepoints = data.basepoints;

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // Incanter's Regalia set (add trigger chance to Mana Shield)
            if (dummySpell->SpellFamilyFlags & uint64(0x0000000000008000) && HasAura(37424)) // Improved Mana Shield
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 37436;
                break;
            }
            break;
        }
        default:
            break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    return TriggerProccedSpell(target, basepoints, triggered_spell_id, castItem, triggeredByAura, cooldown);
}

SpellAuraProcResult Unit::HandleModResistanceAuraProc(ProcExecutionData& data)
{
    uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura;
    SpellEntry const* spellInfo = triggeredByAura->GetSpellProto();

    // Inner Fire
    if (spellInfo->IsFitToFamily(SPELLFAMILY_PRIEST, uint64(0x0000000000002)))
    {
        // only at real damage
        if (!damage)
            return SPELL_AURA_PROC_FAILED;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleRemoveByDamageChanceProc(ProcExecutionData& data)
{
    uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura;
    if (triggeredByAura->GetSpellProto()->Id == 46102) // does not follow this logic
        return SPELL_AURA_PROC_OK;
    // The chance to dispel an aura depends on the damage taken with respect to the casters level.
    uint32 max_dmg = GetLevel() > 8 ? 25 * GetLevel() - 150 : 50;
    float chance = float(damage) / max_dmg * 100.0f;
    if (roll_chance_f(chance))
    {
        RemoveAurasByCasterSpell(triggeredByAura->GetId(), triggeredByAura->GetCasterGuid());
        return SPELL_AURA_PROC_OK;
    }

    return SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleInvisibilityAuraProc(ProcExecutionData& data)
{
    Aura* triggeredByAura = data.triggeredByAura;
    if (triggeredByAura->GetSpellProto()->HasAttribute(SPELL_ATTR_PASSIVE) || triggeredByAura->GetSpellProto()->HasAttribute(SPELL_ATTR_AURA_IS_DEBUFF))
        return SPELL_AURA_PROC_FAILED;

    RemoveAurasDueToSpell(triggeredByAura->GetId());
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandlePeriodicAuraProc(ProcExecutionData& data)
{
    Unit* pVictim = data.target; uint32 damage = data.damage; Aura* triggeredByAura = data.triggeredByAura; SpellEntry const* spellInfo = data.spellInfo; uint32 procFlags = data.procFlags; uint32 procEx = data.procExtra; uint32 cooldown = data.cooldown;

    SpellEntry const* auraInfo = triggeredByAura->GetSpellProto();
    switch (auraInfo->Id)
    {
        case 32065: // Fungal Decay - all three consume one stack on proc
        case 36659: // Tail Sting
            if (triggeredByAura->GetHolder()->ModStackAmount(-1, nullptr)) // Remove aura on return true
                RemoveSpellAuraHolder(triggeredByAura->GetHolder(), AURA_REMOVE_BY_DEFAULT);
        case 35244: // Choking Vines
            if (triggeredByAura->GetHolder()->GetStackAmount() == triggeredByAura->GetHolder()->GetSpellProto()->StackAmount)
            {
                RemoveSpellAuraHolder(triggeredByAura->GetHolder(), AURA_REMOVE_BY_DEFAULT);
                CastSpell(nullptr, 35247, TRIGGERED_OLD_TRIGGERED); // constricting wound
            }
            break;
    }

    return SPELL_AURA_PROC_OK;
}
