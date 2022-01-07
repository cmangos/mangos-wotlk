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

#ifndef __SPELL_DEFINES_H
#define __SPELL_DEFINES_H

#include "Common.h"

// ***********************************
// Spell Attributes definitions
// ***********************************

enum SpellAttributes
{
    SPELL_ATTR_UNK0                            = 0x00000001,// 0
    SPELL_ATTR_RANGED                          = 0x00000002,// 1 All ranged abilites have this flag
    SPELL_ATTR_ON_NEXT_SWING_NO_DAMAGE         = 0x00000004,// 2 on next swing
    SPELL_ATTR_UNK3                            = 0x00000008,// 3 not set in 3.0.3
    SPELL_ATTR_ABILITY                         = 0x00000010,// 4 Displays ability instead of spell clientside
    SPELL_ATTR_TRADESPELL                      = 0x00000020,// 5 trade spells, will be added by client to a sublist of profession spell
    SPELL_ATTR_PASSIVE                         = 0x00000040,// 6 Passive spell
    SPELL_ATTR_DO_NOT_DISPLAY                  = 0x00000080,// 7 Hidden in Spellbook, Aura Icon, Combat Log
    SPELL_ATTR_HIDE_IN_COMBAT_LOG              = 0x00000100,// 8 Do Not Log - our name is better understandable
    SPELL_ATTR_TARGET_MAINHAND_ITEM            = 0x00000200,// 9 Client automatically selects item from mainhand slot as a cast target TODO: Implement
    SPELL_ATTR_ON_NEXT_SWING                   = 0x00000400,// 10 on next swing 2
    SPELL_ATTR_WEARER_CASTS_PROC_TRIGGER       = 0x00000800,// 11
    SPELL_ATTR_DAYTIME_ONLY                    = 0x00001000,// 12 only useable at daytime, not set in 2.4.2
    SPELL_ATTR_NIGHT_ONLY                      = 0x00002000,// 13 only useable at night, not set in 2.4.2
    SPELL_ATTR_INDOORS_ONLY                    = 0x00004000,// 14 only useable indoors, not set in 2.4.2
    SPELL_ATTR_OUTDOORS_ONLY                   = 0x00008000,// 15 Only useable outdoors.
    SPELL_ATTR_NOT_SHAPESHIFT                  = 0x00010000,// 16 Not while shapeshifted
    SPELL_ATTR_ONLY_STEALTHED                  = 0x00020000,// 17 Must be in stealth
    SPELL_ATTR_DONT_AFFECT_SHEATH_STATE        = 0x00040000,// 18 client won't hide unit weapons in sheath on cast/channel TODO: Implement
    SPELL_ATTR_LEVEL_DAMAGE_CALCULATION        = 0x00080000,// 19 spelldamage depends on caster level
    SPELL_ATTR_STOP_ATTACK_TARGET              = 0x00100000,// 20 Stop attack after use this spell (and not begin attack if use)
    SPELL_ATTR_IMPOSSIBLE_DODGE_PARRY_BLOCK    = 0x00200000,// 21 Cannot be dodged/parried/blocked
    SPELL_ATTR_SET_TRACKING_TARGET             = 0x00400000,// 22 SetTrackingTarget
    SPELL_ATTR_CASTABLE_WHILE_DEAD             = 0x00800000,// 23 castable while dead
    SPELL_ATTR_CASTABLE_WHILE_MOUNTED          = 0x01000000,// 24 castable while mounted
    SPELL_ATTR_DISABLED_WHILE_ACTIVE           = 0x02000000,// 25 Activate and start cooldown after aura fade or remove summoned creature or go
    SPELL_ATTR_AURA_IS_DEBUFF                  = 0x04000000,// 26
    SPELL_ATTR_CASTABLE_WHILE_SITTING          = 0x08000000,// 27 castable while sitting
    SPELL_ATTR_CANT_USED_IN_COMBAT             = 0x10000000,// 28 Cannot be used in combat
    SPELL_ATTR_NO_IMMUNITIES                   = 0x20000000,// 29 unaffected by invulnerability
    SPELL_ATTR_HEARTBEAT_RESIST_CHECK          = 0x40000000,// 30 Chance for spell effects to break early (heartbeat resist)
    SPELL_ATTR_CANT_CANCEL                     = 0x80000000,// 31 positive aura can't be canceled
};

enum SpellAttributesEx
{
    SPELL_ATTR_EX_DISMISS_PET                  = 0x00000001,// 0
    SPELL_ATTR_EX_DRAIN_ALL_POWER              = 0x00000002,// 1 use all power (Only paladin Lay of Hands and Bunyanize)
    SPELL_ATTR_EX_CHANNELED_1                  = 0x00000004,// 2 channeled 1
    SPELL_ATTR_EX_CANT_BE_REDIRECTED           = 0x00000008,// 3
    SPELL_ATTR_EX_UNK4                         = 0x00000010,// 4
    SPELL_ATTR_EX_NOT_BREAK_STEALTH            = 0x00000020,// 5 Not break stealth
    SPELL_ATTR_EX_CHANNELED_2                  = 0x00000040,// 6 channeled 2
    SPELL_ATTR_EX_CANT_BE_REFLECTED            = 0x00000080,// 7
    SPELL_ATTR_EX_NOT_IN_COMBAT_TARGET         = 0x00000100,// 8 Spell req target not to be in combat state
    SPELL_ATTR_EX_FACING_TARGET                = 0x00000200,// 9 TODO: CONFIRM!
    SPELL_ATTR_EX_NO_THREAT                    = 0x00000400,// 10 no generates threat on cast 100%
    SPELL_ATTR_EX_DONT_REFRESH_DURATION_ON_RECAST = 0x00000800,// 11 Aura will not refresh its duration when recast
    SPELL_ATTR_EX_FAILURE_BREAKS_STEALTH       = 0x00001000,// 12
    SPELL_ATTR_EX_TOGGLE_FARSIGHT              = 0x00002000,// 13
    SPELL_ATTR_EX_CHANNEL_TRACK_TARGET         = 0x00004000,// 14
    SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY     = 0x00008000,// 15 remove auras on immunity
    SPELL_ATTR_EX_UNAFFECTED_BY_SCHOOL_IMMUNE  = 0x00010000,// 16 unaffected by school immunity
    SPELL_ATTR_EX_UNAUTOCASTABLE_BY_CHARMED    = 0x00020000,// 17 TODO: Investigate more Chero version: SPELL_ATTR_EX_PLAYER_CANT_CAST_CHARMED, likely related to MC
    SPELL_ATTR_EX_PREVENTS_ANIM                = 0x00040000,// 18
    SPELL_ATTR_EX_CANT_TARGET_SELF             = 0x00080000,// 19 spells with area effect or friendly targets that exclude the caster
    SPELL_ATTR_EX_REQ_TARGET_COMBO_POINTS      = 0x00100000,// 20 Req combo points on target
    SPELL_ATTR_EX_THREAT_ONLY_ON_MISS          = 0x00200000,// 21
    SPELL_ATTR_EX_REQ_COMBO_POINTS             = 0x00400000,// 22 Use combo points (in 4.x not required combo point target selected)
    SPELL_ATTR_EX_UNK23                        = 0x00800000,// 23
    SPELL_ATTR_EX_UNK24                        = 0x01000000,// 24 Req fishing pole?? SPELL_ATTR_EX_FISHING
    SPELL_ATTR_EX_UNK25                        = 0x02000000,// 25 not set in 2.4.2
    SPELL_ATTR_EX_REQUIRE_ALL_TARGETS          = 0x04000000,// 26
    SPELL_ATTR_EX_REFUND_POWER                 = 0x08000000,// 27 All these spells refund power on parry or deflect
    SPELL_ATTR_EX_DONT_DISPLAY_IN_AURA_BAR     = 0x10000000,// 28
    SPELL_ATTR_EX_CHANNEL_DISPLAY_SPELL_NAME   = 0x20000000,// 29
    SPELL_ATTR_EX_ENABLE_AT_DODGE              = 0x40000000,// 30 overpower
    SPELL_ATTR_EX_UNK31                        = 0x80000000,// 31
};

enum SpellAttributesEx2
{
    SPELL_ATTR_EX2_CAN_TARGET_DEAD             = 0x00000001,// 0 can target dead unit or corpse
    SPELL_ATTR_EX2_UNK1                        = 0x00000002,// 1 
    SPELL_ATTR_EX2_IGNORE_LOS                  = 0x00000004,// 2 do not need LOS (e.g. 18220 since 3.3.3)
    SPELL_ATTR_EX2_UNK3                        = 0x00000008,// 3 auto targeting? (e.g. fishing skill enhancement items since 3.3.3)
    SPELL_ATTR_EX2_DISPLAY_IN_STANCE_BAR       = 0x00000010,// 4 client displays icon in stance bar when learned, even if not shapeshift
    SPELL_ATTR_EX2_AUTOREPEAT_FLAG             = 0x00000020,// 5
    SPELL_ATTR_EX2_CANT_TARGET_TAPPED          = 0x00000040,// 6 only usable on tabbed by yourself
    SPELL_ATTR_EX2_UNK7                        = 0x00000080,// 7
    SPELL_ATTR_EX2_UNK8                        = 0x00000100,// 8 not set in 2.4.2
    SPELL_ATTR_EX2_UNK9                        = 0x00000200,// 9
    SPELL_ATTR_EX2_UNK10                       = 0x00000400,// 10 SPELL_ATTR_EX2_TAME_SPELLS
    SPELL_ATTR_EX2_HEALTH_FUNNEL               = 0x00000800,// 11
    SPELL_ATTR_EX2_UNK12                       = 0x00001000,// 12 SPELL_ATTR_EX2_CLASS_CLEAVE
    SPELL_ATTR_EX2_UNK13                       = 0x00002000,// 13 TODO: Implement from TC SPELL_ATTR_EX2_CASTABLE_ON_ITEMS
    SPELL_ATTR_EX2_UNK14                       = 0x00004000,// 14
    SPELL_ATTR_EX2_UNK15                       = 0x00008000,// 15 not set in 2.4.2
    SPELL_ATTR_EX2_TAME_BEAST                  = 0x00010000,// 16
    SPELL_ATTR_EX2_NOT_RESET_AUTO_ACTIONS      = 0x00020000,// 17 suspend weapon timer instead of resetting it, (?Hunters Shot and Stings only have this flag?)
    SPELL_ATTR_EX2_REQ_DEAD_PET                = 0x00040000,// 18 Only Revive pet - possible req dead pet
    SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT         = 0x00080000,// 19 does not necessary need shapeshift (pre-3.x not have passive spells with this attribute)
    SPELL_ATTR_EX2_FACING_TARGETS_BACK         = 0x00100000,// 20 TODO: CONFIRM!
    SPELL_ATTR_EX2_DAMAGE_REDUCED_SHIELD       = 0x00200000,// 21 for ice blocks, pala immunity buffs, priest absorb shields, but used also for other spells -> not sure!
    SPELL_ATTR_EX2_NO_INITIAL_THREAT           = 0x00400000,// 22
    SPELL_ATTR_EX2_IS_ARCANE_CONCENTRATION     = 0x00800000,// 23 Only mage Arcane Concentration have this flag
    SPELL_ATTR_EX2_UNK24                       = 0x01000000,// 24
    SPELL_ATTR_EX2_UNK25                       = 0x02000000,// 25
    SPELL_ATTR_EX2_UNAFFECTED_BY_AURA_SCHOOL_IMMUNE = 0x04000000,// 26
    SPELL_ATTR_EX2_UNK27                       = 0x08000000,// 27
    SPELL_ATTR_EX2_UNK28                       = 0x10000000,// 28 no breaks stealth if it fails??
    SPELL_ATTR_EX2_CANT_CRIT                   = 0x20000000,// 29 Spell can't crit
    SPELL_ATTR_EX2_TRIGGERED_CAN_TRIGGER_PROC  = 0x40000000,// 30 Chero hint: SPELL_ATTR_EX2_CAN_TRIGGER_VICTIM
    SPELL_ATTR_EX2_FOOD_BUFF                   = 0x80000000,// 31 Food or Drink Buff (like Well Fed)
};

enum SpellAttributesEx3
{
    SPELL_ATTR_EX3_OUT_OF_COMBAT_ATTACK        = 0x00000001,// 0 Spell landed counts as hostile action against enemy even if it doesn't trigger combat state, propagates PvP flags
    SPELL_ATTR_EX3_UNK1                        = 0x00000002,// 1
    SPELL_ATTR_EX3_UNK2                        = 0x00000004,// 2
    SPELL_ATTR_EX3_BLOCKABLE_SPELL             = 0x00000008,// 3 TODO: Investigate more
    SPELL_ATTR_EX3_IGNORE_RESURRECTION_TIMER   = 0x00000010,// 4 Druid Rebirth only this spell have this flag
    SPELL_ATTR_EX3_UNK5                        = 0x00000020,// 5
    SPELL_ATTR_EX3_UNK6                        = 0x00000040,// 6
    SPELL_ATTR_EX3_STACK_FOR_DIFF_CASTERS      = 0x00000080,// 7 create a separate (de)buff stack for each caster
    SPELL_ATTR_EX3_TARGET_ONLY_PLAYER          = 0x00000100,// 8 Can target only player
    SPELL_ATTR_EX3_TRIGGERED_CAN_TRIGGER_SPECIAL = 0x00000200,// 9 Can only proc auras
    SPELL_ATTR_EX3_MAIN_HAND                   = 0x00000400,// 10 Main hand weapon required
    SPELL_ATTR_EX3_BATTLEGROUND                = 0x00000800,// 11 Can casted only on battleground
    SPELL_ATTR_EX3_CAST_ON_DEAD                = 0x00001000,// 12 target is a dead player (not every spell has this flag)
    SPELL_ATTR_EX3_DONT_DISPLAY_CHANNEL_BAR    = 0x00002000,// 13
    SPELL_ATTR_EX3_IS_HONORLESS_TARGET         = 0x00004000,// 14 "Honorless Target" only this spells have this flag
    SPELL_ATTR_EX3_RANGED_ATTACK               = 0x00008000,// 15 Spells with this attribute are processed as ranged attacks in client
    SPELL_ATTR_EX3_SUPPRESS_CASTER_PROCS       = 0x00010000,// 16
    SPELL_ATTR_EX3_SUPPRESS_TARGET_PROCS       = 0x00020000,// 17
    SPELL_ATTR_EX3_ALWAYS_HIT                  = 0x00040000,// 18 Spell should always hit its target 
    SPELL_ATTR_EX3_UNK19                       = 0x00080000,// 19 TODO: Implement from TC
    SPELL_ATTR_EX3_DEATH_PERSISTENT            = 0x00100000,// 20 Death persistent spells
    SPELL_ATTR_EX3_UNK21                       = 0x00200000,// 21
    SPELL_ATTR_EX3_REQ_WAND                    = 0x00400000,// 22 Req wand
    SPELL_ATTR_EX3_UNK23                       = 0x00800000,// 23
    SPELL_ATTR_EX3_REQ_OFFHAND                 = 0x01000000,// 24 Req offhand weapon
    SPELL_ATTR_EX3_TREAT_AS_PERIODIC           = 0x02000000,// 25 Treated as periodic spell
    SPELL_ATTR_EX3_CAN_PROC_FROM_TRIGGERED     = 0x04000000,// 26 Auras with this attribute can proc off SPELL_ATTR_EX3_TRIGGERED_CAN_TRIGGER_SPECIAL
    SPELL_ATTR_EX3_UNK27                       = 0x08000000,// 27
    SPELL_ATTR_EX3_UNK28                       = 0x10000000,// 28 always cast ok ? (requires more research)
    SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS     = 0x20000000,// 29 Resistances should still affect damage
    SPELL_ATTR_EX3_DONT_DISPLAY_RANGE          = 0x40000000,// 30
    SPELL_ATTR_EX3_UNK31                       = 0x80000000,// 31
};

enum SpellAttributesEx4
{
    SPELL_ATTR_EX4_IGNORE_RESISTANCES          = 0x00000001,// 0
    SPELL_ATTR_EX4_PROC_ONLY_ON_CASTER         = 0x00000002,// 1 Only proc on self-cast
    SPELL_ATTR_EX4_AURA_EXPIRES_OFFLINE        = 0x00000004,// 2
    SPELL_ATTR_EX4_UNK3                        = 0x00000008,// 3
    SPELL_ATTR_EX4_UNK4                        = 0x00000010,// 4 This will no longer cause guards to attack on use??
    SPELL_ATTR_EX4_UNK5                        = 0x00000020,// 5
    SPELL_ATTR_EX4_NOT_STEALABLE               = 0x00000040,// 6 although such auras might be dispellable, they cannot be stolen
    SPELL_ATTR_EX4_CAN_CAST_WHILE_CASTING      = 0x00000080,// 7 In theory, can use this spell while another is channeled/cast/autocast
    SPELL_ATTR_EX4_IGNORE_DAMAGE_TAKEN_MODIFIERS = 0x00000100,// 8
    SPELL_ATTR_EX4_TRIGGER_ACTIVATE            = 0x00000200,// 9 initially disabled / trigger activate from event (Execute, Riposte, Deep Freeze end other)
    SPELL_ATTR_EX4_SPELL_VS_EXTEND_COST        = 0x00000400,// 10 Rogue Shiv have this flag
    SPELL_ATTR_EX4_UNK11                       = 0x00000800,// 11
    SPELL_ATTR_EX4_UNK12                       = 0x00001000,// 12
    SPELL_ATTR_EX4_UNK13                       = 0x00002000,// 13
    SPELL_ATTR_EX4_DAMAGE_DOESNT_BREAK_AURAS   = 0x00004000,// 14
    SPELL_ATTR_EX4_UNK15                       = 0x00008000,// 15 Dont add to spellbook
    SPELL_ATTR_EX4_NOT_USABLE_IN_ARENA         = 0x00010000,// 16 not usable in arena
    SPELL_ATTR_EX4_USABLE_IN_ARENA             = 0x00020000,// 17 usable in arena
    SPELL_ATTR_EX4_UNK18                       = 0x00040000,// 18 TODO: Investigate from TC
    SPELL_ATTR_EX4_UNK19                       = 0x00080000,// 19
    SPELL_ATTR_EX4_NOT_CHECK_SELFCAST_POWER    = 0x00100000,// 20 do not give "more powerful spell" error message
    SPELL_ATTR_EX4_UNK21                       = 0x00200000,// 21
    SPELL_ATTR_EX4_UNK22                       = 0x00400000,// 22
    SPELL_ATTR_EX4_UNK23                       = 0x00800000,// 23
    SPELL_ATTR_EX4_AUTO_RANGED_COMBAT_SPELL    = 0x01000000,// 24
    SPELL_ATTR_EX4_IS_PET_SCALING              = 0x02000000,// 25 pet scaling auras
    SPELL_ATTR_EX4_CAST_ONLY_IN_OUTLAND        = 0x04000000,// 26 Can only be used in Outland.
    SPELL_ATTR_EX4_UNK27                       = 0x08000000,// 27 Always shown in combat log
    SPELL_ATTR_EX4_UNK28                       = 0x10000000,// 28
    SPELL_ATTR_EX4_UNK29                       = 0x20000000,// 29 Related to client check about dispel, CC seems to have them - dispel effect 0
    SPELL_ATTR_EX4_UNK30                       = 0x40000000,// 30 - dispel effect 1
    SPELL_ATTR_EX4_UNK31                       = 0x80000000,// 31 - dispel effect 2
};

enum SpellAttributesEx5
{
    SPELL_ATTR_EX5_CAN_CHANNEL_WHEN_MOVING     = 0x00000001,// 0 don't interrupt channeling spells when moving
    SPELL_ATTR_EX5_NO_REAGENT_WHILE_PREP       = 0x00000002,// 1 not need reagents if UNIT_FLAG_PREPARATION
    SPELL_ATTR_EX5_REMOVE_ON_ARENA_ENTER       = 0x00000004,// 2 removed at enter arena (e.g. 31850 since 3.3.3)
    SPELL_ATTR_EX5_USABLE_WHILE_STUNNED        = 0x00000008,// 3 usable while stunned
    SPELL_ATTR_EX5_UNK4                        = 0x00000010,// 4
    SPELL_ATTR_EX5_SINGLE_TARGET_SPELL         = 0x00000020,// 5 Only one target can be apply at a time
    SPELL_ATTR_EX5_UNK6                        = 0x00000040,// 6
    SPELL_ATTR_EX5_UNK7                        = 0x00000080,// 7
    SPELL_ATTR_EX5_UNK8                        = 0x00000100,// 8
    SPELL_ATTR_EX5_START_PERIODIC_AT_APPLY     = 0x00000200,// 9  begin periodic tick at aura apply
    SPELL_ATTR_EX5_HIDE_DURATION               = 0x00000400,// 10
    SPELL_ATTR_EX5_ALLOW_TARGET_OF_TARGET_AS_TARGET = 0x00000800,// 11
    SPELL_ATTR_EX5_CLEAVE_FRONT_TARGET         = 0x00001000,// 12 Cleave Generic chain damage front targets
    SPELL_ATTR_EX5_HASTE_AFFECT_DURATION       = 0x00002000,// 13 haste affects duration
    SPELL_ATTR_EX5_UNK14                       = 0x00004000,// 14
    SPELL_ATTR_EX5_UNK15                       = 0x00008000,// 15
    SPELL_ATTR_EX5_MOD_ALL_DAMAGE              = 0x00010000,// 16 (only affects protpally One-Handed Weapon Spec)
    SPELL_ATTR_EX5_USABLE_WHILE_FEARED         = 0x00020000,// 17 usable while feared
    SPELL_ATTR_EX5_USABLE_WHILE_CONFUSED       = 0x00040000,// 18 usable while confused
    SPELL_ATTR_EX5_AI_DOESNT_FACE_TARGET       = 0x00080000,// 19
    SPELL_ATTR_EX5_UNK20                       = 0x00100000,// 20
    SPELL_ATTR_EX5_UNK21                       = 0x00200000,// 21
    SPELL_ATTR_EX5_NOT_ON_TRIVIAL              = 0x00400000,// 22
    SPELL_ATTR_EX5_UNK23                       = 0x00800000,// 23
    SPELL_ATTR_EX5_UNK24                       = 0x01000000,// 24
    SPELL_ATTR_EX5_UNK25                       = 0x02000000,// 25
    SPELL_ATTR_EX5_UNK26                       = 0x04000000,// 26
    SPELL_ATTR_EX5_DONT_SHOW_AURA_IF_SELF_CAST = 0x08000000,// 27 Auras with this attribute are not visible on units that are the caster
    SPELL_ATTR_EX5_DONT_SHOW_AURA_IF_NOT_SELF_CAST = 0x10000000,// 28 Auras with this attribute are not visible on units that are not the caster
    SPELL_ATTR_EX5_UNK29                       = 0x20000000,// 29
    SPELL_ATTR_EX5_UNK30                       = 0x40000000,// 30
    SPELL_ATTR_EX5_USE_PHYSICAL_HIT_CHANCE     = 0x80000000,// 31 Introduced in patch 2.3: Taunt, Growl, etc spells use ability miss calculation (see implementation for details)
};

enum SpellAttributesEx6
{
    SPELL_ATTR_EX6_DONT_DISPLAY_COOLDOWN       = 0x00000001,// 0 Only Move spell have this flag
    SPELL_ATTR_EX6_ONLY_IN_ARENA               = 0x00000002,// 1 only usable in arena, not used in 3.2.0a and early
    SPELL_ATTR_EX6_IGNORE_CASTER_AURAS         = 0x00000004,// 2
    SPELL_ATTR_EX6_ASSIST_IGNORE_IMMUNE_FLAG   = 0x00000008,// 3
    SPELL_ATTR_EX6_UNK4                        = 0x00000010,// 4
    SPELL_ATTR_EX6_UNK5                        = 0x00000020,// 5
    SPELL_ATTR_EX6_USE_SPELL_CAST_EVENT        = 0x00000040,// 6 Auras with this attribute trigger SPELL_CAST combat log event instead of SPELL_AURA_START (clientside attribute)
    SPELL_ATTR_EX6_UNK7                        = 0x00000080,// 7
    SPELL_ATTR_EX6_IGNORE_CC_TARGETS           = 0x00000100,// 8 ignores target with cc effects
    SPELL_ATTR_EX6_UNK9                        = 0x00000200,// 9
    SPELL_ATTR_EX6_UNK10                       = 0x00000400,// 10
    SPELL_ATTR_EX6_NOT_IN_RAID_INSTANCE        = 0x00000800,// 11 not usable in raid instance
    SPELL_ATTR_EX6_CASTABLE_WHILE_ON_VEHICLE   = 0x00001000,// 12
    SPELL_ATTR_EX6_IGNORE_PHASE_SHIFT          = 0x00002000,// 13 Can target in different phase
    SPELL_ATTR_EX6_UNK14                       = 0x00004000,// 14
    SPELL_ATTR_EX6_UNK15                       = 0x00008000,// 15 not set in 3.0.3
    SPELL_ATTR_EX6_UNK16                       = 0x00010000,// 16
    SPELL_ATTR_EX6_UNK17                       = 0x00020000,// 17
    SPELL_ATTR_EX6_UNK18                       = 0x00040000,// 18
    SPELL_ATTR_EX6_UNK19                       = 0x00080000,// 19
    SPELL_ATTR_EX6_UNK20                       = 0x00100000,// 20
    SPELL_ATTR_EX6_UNK21                       = 0x00200000,// 21
    SPELL_ATTR_EX6_UNK22                       = 0x00400000,// 22
    SPELL_ATTR_EX6_UNK23                       = 0x00800000,// 23 not set in 3.0.3
    SPELL_ATTR_EX6_UNK24                       = 0x01000000,// 24 not set in 3.0.3
    SPELL_ATTR_EX6_UNK25                       = 0x02000000,// 25 not set in 3.0.3
    SPELL_ATTR_EX6_UNK26                       = 0x04000000,// 26 not set in 3.0.3
    SPELL_ATTR_EX6_UNK27                       = 0x08000000,// 27 not set in 3.0.3
    SPELL_ATTR_EX6_UNK28                       = 0x10000000,// 28 not set in 3.0.3
    SPELL_ATTR_EX6_NO_DMG_PERCENT_MODS         = 0x20000000,// 29 do not apply damage mods (usually in cases where it has already been applied)
    SPELL_ATTR_EX6_UNK30                       = 0x40000000,// 30 not set in 3.0.3
    SPELL_ATTR_EX6_UNK31                       = 0x80000000,// 31 not set in 3.0.3
};

enum SpellAttributesEx7
{
    SPELL_ATTR_EX7_UNK0                        = 0x00000001,// 0
    SPELL_ATTR_EX7_UNK1                        = 0x00000002,// 1
    SPELL_ATTR_EX7_PALADIN_AURA                = 0x00000004,// 2
    SPELL_ATTR_EX7_UNK3                        = 0x00000008,// 3
    SPELL_ATTR_EX7_UNK4                        = 0x00000010,// 4
    SPELL_ATTR_EX7_TOTEM_SPELL                 = 0x00000020,// 5  shaman summon totem spells
    SPELL_ATTR_EX7_UNK6                        = 0x00000040,// 6
    SPELL_ATTR_EX7_UNK7                        = 0x00000080,// 7
    SPELL_ATTR_EX7_UNK8                        = 0x00000100,// 8
    SPELL_ATTR_EX7_UNK9                        = 0x00000200,// 9
    SPELL_ATTR_EX7_UNK10                       = 0x00000400,// 10
    SPELL_ATTR_EX7_UNK11                       = 0x00000800,// 11
    SPELL_ATTR_EX7_UNK12                       = 0x00001000,// 12
    SPELL_ATTR_EX7_UNK13                       = 0x00002000,// 13
    SPELL_ATTR_EX7_UNK14                       = 0x00004000,// 14
    SPELL_ATTR_EX7_UNK15                       = 0x00008000,// 15
    SPELL_ATTR_EX7_UNK16                       = 0x00010000,// 16
    SPELL_ATTR_EX7_UNK17                       = 0x00020000,// 17
    SPELL_ATTR_EX7_UNK18                       = 0x00040000,// 18
    SPELL_ATTR_EX7_UNK19                       = 0x00080000,// 19
    SPELL_ATTR_EX7_UNK20                       = 0x00100000,// 20
    SPELL_ATTR_EX7_UNK21                       = 0x00200000,// 21
    SPELL_ATTR_EX7_UNK22                       = 0x00400000,// 22
    SPELL_ATTR_EX7_UNK23                       = 0x00800000,// 23
    SPELL_ATTR_EX7_UNK24                       = 0x01000000,// 24
    SPELL_ATTR_EX7_UNK25                       = 0x02000000,// 25
    SPELL_ATTR_EX7_UNK26                       = 0x04000000,// 26
    SPELL_ATTR_EX7_UNK27                       = 0x08000000,// 27
    SPELL_ATTR_EX7_UNK28                       = 0x10000000,// 28
    SPELL_ATTR_EX7_UNK29                       = 0x20000000,// 29
    SPELL_ATTR_EX7_UNK30                       = 0x40000000,// 30
    SPELL_ATTR_EX7_UNK31                       = 0x80000000,// 31
};

enum SpellAttributesServerside
{
    SPELL_ATTR_SS_PREVENT_INVIS                = 0x00000001,
    SPELL_ATTR_SS_AOE_CAP                      = 0x00000002,
    SPELL_ATTR_SS_IGNORE_EVADE                 = 0x00000004,
};

enum SpellCastResult
{
    SPELL_FAILED_SUCCESS                        = 0,
    SPELL_FAILED_AFFECTING_COMBAT               = 1,
    SPELL_FAILED_ALREADY_AT_FULL_HEALTH         = 2,
    SPELL_FAILED_ALREADY_AT_FULL_MANA           = 3,
    SPELL_FAILED_ALREADY_AT_FULL_POWER          = 4,
    SPELL_FAILED_ALREADY_BEING_TAMED            = 5,
    SPELL_FAILED_ALREADY_HAVE_CHARM             = 6,
    SPELL_FAILED_ALREADY_HAVE_SUMMON            = 7,
    SPELL_FAILED_ALREADY_OPEN                   = 8,
    SPELL_FAILED_AURA_BOUNCED                   = 9,
    SPELL_FAILED_AUTOTRACK_INTERRUPTED          = 10,
    SPELL_FAILED_BAD_IMPLICIT_TARGETS           = 11,
    SPELL_FAILED_BAD_TARGETS                    = 12,
    SPELL_FAILED_CANT_BE_CHARMED                = 13,
    SPELL_FAILED_CANT_BE_DISENCHANTED           = 14,
    SPELL_FAILED_CANT_BE_DISENCHANTED_SKILL     = 15,
    SPELL_FAILED_CANT_BE_MILLED                 = 16,
    SPELL_FAILED_CANT_BE_PROSPECTED             = 17,
    SPELL_FAILED_CANT_CAST_ON_TAPPED            = 18,
    SPELL_FAILED_CANT_DUEL_WHILE_INVISIBLE      = 19,
    SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED      = 20,
    SPELL_FAILED_CANT_STEALTH                   = 21,
    SPELL_FAILED_CASTER_AURASTATE               = 22,
    SPELL_FAILED_CASTER_DEAD                    = 23,
    SPELL_FAILED_CHARMED                        = 24,
    SPELL_FAILED_CHEST_IN_USE                   = 25,
    SPELL_FAILED_CONFUSED                       = 26,
    SPELL_FAILED_DONT_REPORT                    = 27,
    SPELL_FAILED_EQUIPPED_ITEM                  = 28,
    SPELL_FAILED_EQUIPPED_ITEM_CLASS            = 29,
    SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND   = 30,
    SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND    = 31,
    SPELL_FAILED_ERROR                          = 32,
    SPELL_FAILED_FIZZLE                         = 33,
    SPELL_FAILED_FLEEING                        = 34,
    SPELL_FAILED_FOOD_LOWLEVEL                  = 35,
    SPELL_FAILED_HIGHLEVEL                      = 36,
    SPELL_FAILED_HUNGER_SATIATED                = 37,
    SPELL_FAILED_IMMUNE                         = 38,
    SPELL_FAILED_INCORRECT_AREA                 = 39,
    SPELL_FAILED_INTERRUPTED                    = 40,
    SPELL_FAILED_INTERRUPTED_COMBAT             = 41,
    SPELL_FAILED_ITEM_ALREADY_ENCHANTED         = 42,
    SPELL_FAILED_ITEM_GONE                      = 43,
    SPELL_FAILED_ITEM_NOT_FOUND                 = 44,
    SPELL_FAILED_ITEM_NOT_READY                 = 45,
    SPELL_FAILED_LEVEL_REQUIREMENT              = 46,
    SPELL_FAILED_LINE_OF_SIGHT                  = 47,
    SPELL_FAILED_LOWLEVEL                       = 48,
    SPELL_FAILED_LOW_CASTLEVEL                  = 49,
    SPELL_FAILED_MAINHAND_EMPTY                 = 50,
    SPELL_FAILED_MOVING                         = 51,
    SPELL_FAILED_NEED_AMMO                      = 52,
    SPELL_FAILED_NEED_AMMO_POUCH                = 53,
    SPELL_FAILED_NEED_EXOTIC_AMMO               = 54,
    SPELL_FAILED_NEED_MORE_ITEMS                = 55,
    SPELL_FAILED_NOPATH                         = 56,
    SPELL_FAILED_NOT_BEHIND                     = 57,
    SPELL_FAILED_NOT_FISHABLE                   = 58,
    SPELL_FAILED_NOT_FLYING                     = 59,
    SPELL_FAILED_NOT_HERE                       = 60,
    SPELL_FAILED_NOT_INFRONT                    = 61,
    SPELL_FAILED_NOT_IN_CONTROL                 = 62,
    SPELL_FAILED_NOT_KNOWN                      = 63,
    SPELL_FAILED_NOT_MOUNTED                    = 64,
    SPELL_FAILED_NOT_ON_TAXI                    = 65,
    SPELL_FAILED_NOT_ON_TRANSPORT               = 66,
    SPELL_FAILED_NOT_READY                      = 67,
    SPELL_FAILED_NOT_SHAPESHIFT                 = 68,
    SPELL_FAILED_NOT_STANDING                   = 69,
    SPELL_FAILED_NOT_TRADEABLE                  = 70,
    SPELL_FAILED_NOT_TRADING                    = 71,
    SPELL_FAILED_NOT_UNSHEATHED                 = 72,
    SPELL_FAILED_NOT_WHILE_GHOST                = 73,
    SPELL_FAILED_NOT_WHILE_LOOTING              = 74,
    SPELL_FAILED_NO_AMMO                        = 75,
    SPELL_FAILED_NO_CHARGES_REMAIN              = 76,
    SPELL_FAILED_NO_CHAMPION                    = 77,
    SPELL_FAILED_NO_COMBO_POINTS                = 78,
    SPELL_FAILED_NO_DUELING                     = 79,
    SPELL_FAILED_NO_ENDURANCE                   = 80,
    SPELL_FAILED_NO_FISH                        = 81,
    SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED    = 82,
    SPELL_FAILED_NO_MOUNTS_ALLOWED              = 83,
    SPELL_FAILED_NO_PET                         = 84,
    SPELL_FAILED_NO_POWER                       = 85,
    SPELL_FAILED_NOTHING_TO_DISPEL              = 86,
    SPELL_FAILED_NOTHING_TO_STEAL               = 87,
    SPELL_FAILED_ONLY_ABOVEWATER                = 88,
    SPELL_FAILED_ONLY_DAYTIME                   = 89,
    SPELL_FAILED_ONLY_INDOORS                   = 90,
    SPELL_FAILED_ONLY_MOUNTED                   = 91,
    SPELL_FAILED_ONLY_NIGHTTIME                 = 92,
    SPELL_FAILED_ONLY_OUTDOORS                  = 93,
    SPELL_FAILED_ONLY_SHAPESHIFT                = 94,
    SPELL_FAILED_ONLY_STEALTHED                 = 95,
    SPELL_FAILED_ONLY_UNDERWATER                = 96,
    SPELL_FAILED_OUT_OF_RANGE                   = 97,
    SPELL_FAILED_PACIFIED                       = 98,
    SPELL_FAILED_POSSESSED                      = 99,
    SPELL_FAILED_REAGENTS                       = 100,
    SPELL_FAILED_REQUIRES_AREA                  = 101,
    SPELL_FAILED_REQUIRES_SPELL_FOCUS           = 102,
    SPELL_FAILED_ROOTED                         = 103,
    SPELL_FAILED_SILENCED                       = 104,
    SPELL_FAILED_SPELL_IN_PROGRESS              = 105,
    SPELL_FAILED_SPELL_LEARNED                  = 106,
    SPELL_FAILED_SPELL_UNAVAILABLE              = 107,
    SPELL_FAILED_STUNNED                        = 108,
    SPELL_FAILED_TARGETS_DEAD                   = 109,
    SPELL_FAILED_TARGET_AFFECTING_COMBAT        = 110,
    SPELL_FAILED_TARGET_AURASTATE               = 111,
    SPELL_FAILED_TARGET_DUELING                 = 112,
    SPELL_FAILED_TARGET_ENEMY                   = 113,
    SPELL_FAILED_TARGET_ENRAGED                 = 114,
    SPELL_FAILED_TARGET_FRIENDLY                = 115,
    SPELL_FAILED_TARGET_IN_COMBAT               = 116,
    SPELL_FAILED_TARGET_IS_PLAYER               = 117,
    SPELL_FAILED_TARGET_IS_PLAYER_CONTROLLED    = 118,
    SPELL_FAILED_TARGET_NOT_DEAD                = 119,
    SPELL_FAILED_TARGET_NOT_IN_PARTY            = 120,
    SPELL_FAILED_TARGET_NOT_LOOTED              = 121,
    SPELL_FAILED_TARGET_NOT_PLAYER              = 122,
    SPELL_FAILED_TARGET_NO_POCKETS              = 123,
    SPELL_FAILED_TARGET_NO_WEAPONS              = 124,
    SPELL_FAILED_TARGET_NO_RANGED_WEAPONS       = 125,
    SPELL_FAILED_TARGET_UNSKINNABLE             = 126,
    SPELL_FAILED_THIRST_SATIATED                = 127,
    SPELL_FAILED_TOO_CLOSE                      = 128,
    SPELL_FAILED_TOO_MANY_OF_ITEM               = 129,
    SPELL_FAILED_TOTEM_CATEGORY                 = 130,
    SPELL_FAILED_TOTEMS                         = 131,
    SPELL_FAILED_TRY_AGAIN                      = 132,
    SPELL_FAILED_UNIT_NOT_BEHIND                = 133,
    SPELL_FAILED_UNIT_NOT_INFRONT               = 134,
    SPELL_FAILED_WRONG_PET_FOOD                 = 135,
    SPELL_FAILED_NOT_WHILE_FATIGUED             = 136,
    SPELL_FAILED_TARGET_NOT_IN_INSTANCE         = 137,
    SPELL_FAILED_NOT_WHILE_TRADING              = 138,
    SPELL_FAILED_TARGET_NOT_IN_RAID             = 139,
    SPELL_FAILED_TARGET_FREEFORALL              = 140,
    SPELL_FAILED_NO_EDIBLE_CORPSES              = 141,
    SPELL_FAILED_ONLY_BATTLEGROUNDS             = 142,
    SPELL_FAILED_TARGET_NOT_GHOST               = 143,
    SPELL_FAILED_TRANSFORM_UNUSABLE             = 144,
    SPELL_FAILED_WRONG_WEATHER                  = 145,
    SPELL_FAILED_DAMAGE_IMMUNE                  = 146,
    SPELL_FAILED_PREVENTED_BY_MECHANIC          = 147,
    SPELL_FAILED_PLAY_TIME                      = 148,
    SPELL_FAILED_REPUTATION                     = 149,
    SPELL_FAILED_MIN_SKILL                      = 150,
    SPELL_FAILED_NOT_IN_ARENA                   = 151,
    SPELL_FAILED_NOT_ON_SHAPESHIFT              = 152,
    SPELL_FAILED_NOT_ON_STEALTHED               = 153,
    SPELL_FAILED_NOT_ON_DAMAGE_IMMUNE           = 154,
    SPELL_FAILED_NOT_ON_MOUNTED                 = 155,
    SPELL_FAILED_TOO_SHALLOW                    = 156,
    SPELL_FAILED_TARGET_NOT_IN_SANCTUARY        = 157,
    SPELL_FAILED_TARGET_IS_TRIVIAL              = 158,
    SPELL_FAILED_BM_OR_INVISGOD                 = 159,
    SPELL_FAILED_EXPERT_RIDING_REQUIREMENT      = 160,
    SPELL_FAILED_ARTISAN_RIDING_REQUIREMENT     = 161,
    SPELL_FAILED_NOT_IDLE                       = 162,
    SPELL_FAILED_NOT_INACTIVE                   = 163,
    SPELL_FAILED_PARTIAL_PLAYTIME               = 164,
    SPELL_FAILED_NO_PLAYTIME                    = 165,
    SPELL_FAILED_NOT_IN_BATTLEGROUND            = 166,
    SPELL_FAILED_NOT_IN_RAID_INSTANCE           = 167,
    SPELL_FAILED_ONLY_IN_ARENA                  = 168,
    SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE = 169,
    SPELL_FAILED_ON_USE_ENCHANT                 = 170,
    SPELL_FAILED_NOT_ON_GROUND                  = 171,
    SPELL_FAILED_CUSTOM_ERROR                   = 172,
    SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW         = 173,
    SPELL_FAILED_TOO_MANY_SOCKETS               = 174,
    SPELL_FAILED_INVALID_GLYPH                  = 175,
    SPELL_FAILED_UNIQUE_GLYPH                   = 176,
    SPELL_FAILED_GLYPH_SOCKET_LOCKED            = 177,
    SPELL_FAILED_NO_VALID_TARGETS               = 178,
    SPELL_FAILED_ITEM_AT_MAX_CHARGES            = 179,
    SPELL_FAILED_NOT_IN_BARBERSHOP              = 180,
    SPELL_FAILED_FISHING_TOO_LOW                = 181,
    SPELL_FAILED_ITEM_ENCHANT_TRADE_WINDOW      = 182,
    SPELL_FAILED_SUMMON_PENDING                 = 183,
    SPELL_FAILED_MAX_SOCKETS                    = 184,
    SPELL_FAILED_PET_CAN_RENAME                 = 185,
    SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED   = 186,
    SPELL_FAILED_UNKNOWN                        = 187,                             // actually doesn't exist in client

    SPELL_NOT_FOUND = 254,                                  // custom value, don't must be send to client
    SPELL_CAST_OK = 255
};

// Used in addition to SPELL_FAILED_CUSTOM_ERROR
enum SpellCastResultCustom
{
    // Postfix _NONE will not display the text in client
    SPELL_FAILED_CUSTOM_ERROR_1         = 1,                // "Something bad happened, and we want to display a custom message!"
    SPELL_FAILED_CUSTOM_ERROR_2         = 2,                // "Alex broke your quest! Thank him later!"
    SPELL_FAILED_CUSTOM_ERROR_3         = 3,                // "This spell may only be used on Helpless Wintergarde Villagers that have not been rescued."
    SPELL_FAILED_CUSTOM_ERROR_4         = 4,                // "Requires that you be wearing the Warsong Disguise."
    SPELL_FAILED_CUSTOM_ERROR_5         = 5,                // "You must be closer to a plague wagon in order to drop off yor 7th Legion Siege Engineer."
    SPELL_FAILED_CUSTOM_ERROR_6         = 6,                // "You cannot target friendly targets outside your party."
    SPELL_FAILED_CUSTOM_ERROR_7         = 7,                // "You must target a weakened chill nymph."
    SPELL_FAILED_CUSTOM_ERROR_8         = 8,                // "The Imbued Scourge Shroud will only work when equipped in the Temple City of En'Kilah"
    SPELL_FAILED_CUSTOM_ERROR_9         = 9,                // "Requires Corpse Dust"
    SPELL_FAILED_CUSTOM_ERROR_10        = 10,               // "You cannot summon another gargoyle yet."
    SPELL_FAILED_CUSTOM_ERROR_11        = 11,               // "Requires Corpse Dust if the target is not dead and humanoid."
    SPELL_FAILED_CUSTOM_ERROR_12        = 12,               // "Can only be placed near Shatterhorn."
    SPELL_FAILED_CUSTOM_ERROR_13        = 13,               // "You must first select a Proto-Drake Egg"
    SPELL_FAILED_CUSTOM_ERROR_14_NONE   = 14,               // "You must be close to a marked tree."
    SPELL_FAILED_CUSTOM_ERROR_15        = 15,               // "You must target a Fjord Turkey."
    SPELL_FAILED_CUSTOM_ERROR_16        = 16,               // "You must target a Fjord Hawk."
    SPELL_FAILED_CUSTOM_ERROR_17        = 17,               // "You are too far away from the bouy."
    SPELL_FAILED_CUSTOM_ERROR_18        = 18,               // "Must be used near an oil slick."
    SPELL_FAILED_CUSTOM_ERROR_19        = 19,               // "You must be closer to the bouy!"
    SPELL_FAILED_CUSTOM_ERROR_20        = 20,               // "You may only call for the aid of a Wyrmrest Vanquisher in Wyrmrest Temple, The Dragon Wastes, Galakrond's Rest or The Wicked Coil."
    SPELL_FAILED_CUSTOM_ERROR_21        = 21,               // "Can only be used on a Ice Heart Jormungar Spawn."
    SPELL_FAILED_CUSTOM_ERROR_22        = 22,               // "You must be closer to a sinkhole to use your map."
    SPELL_FAILED_CUSTOM_ERROR_23        = 23,               // "You may only call down a stampede on Harold Lane."
    SPELL_FAILED_CUSTOM_ERROR_24        = 24,               // "You may only use the Pouch of Crushed Bloodspore on Gammothra or other magnataur in the Bloodspore Plains and Gammoth."
    SPELL_FAILED_CUSTOM_ERROR_25        = 25,               // "Requires the magmawyrm ressurection chamber in the back of the Maw of Neltharion."
    SPELL_FAILED_CUSTOM_ERROR_26        = 26,               // "You may only call down a Wintergarde Gryphon in Wintergarde Keep or the Carrion Fields."
    SPELL_FAILED_CUSTOM_ERROR_27        = 27,               // "What are you doing? Only aim that thing at Wilhelm!"
    SPELL_FAILED_CUSTOM_ERROR_28        = 28,               // "Not enough health!"
    SPELL_FAILED_CUSTOM_ERROR_29        = 29,               // "There are no nearby corpses to use"
    SPELL_FAILED_CUSTOM_ERROR_30        = 30,               // "You've created enough ghouls. Return to Gothik the Harvester at Death's Breach."
    SPELL_FAILED_CUSTOM_ERROR_31        = 31,               // "Your companion does not want to come here. Go further from the Sundered Shard."
    SPELL_FAILED_CUSTOM_ERROR_32        = 32,               // "Must be in Cat Form"
    SPELL_FAILED_CUSTOM_ERROR_33        = 33,               // "Only Death Knights may enter Ebon Hold."
    SPELL_FAILED_CUSTOM_ERROR_34        = 34,               // "Must be in Cat Form, Bear Form, or Dire Bear Form."
    SPELL_FAILED_CUSTOM_ERROR_35        = 35,               // "You must be within range of a Helpless Wintergarde Villager"
    SPELL_FAILED_CUSTOM_ERROR_36        = 36,               // "You cannot target an elemental or mechanical corpse."
    SPELL_FAILED_CUSTOM_ERROR_37        = 37,               // "This teleport crystal cannot be used until the teleport crystal in Dalaran has been used at least once."
    SPELL_FAILED_CUSTOM_ERROR_38        = 38,               // "You are already holding something in your hand. You must throw the creature in your hand before picking up another."
    SPELL_FAILED_CUSTOM_ERROR_39        = 39,               // "You don't have anything to throw! Find a Vargul and use Gymer Grab to pick one up!"
    SPELL_FAILED_CUSTOM_ERROR_40        = 40,               // "Bouldercrag's War Horn can only be used within 10 yards of Valduran the Stormborn."
    SPELL_FAILED_CUSTOM_ERROR_41        = 41,               // "You are not carrying a passenger. There is nobody to drop off."
    SPELL_FAILED_CUSTOM_ERROR_42        = 42,               // "You cannot build any more siege vehicles."
    SPELL_FAILED_CUSTOM_ERROR_43        = 43,               // "You are already carrying a captured Argent Crusader. You must return to the Argen Vanguard Infirmary and drop off your passenger before you may pick up another."
    SPELL_FAILED_CUSTOM_ERROR_44        = 44,               // "You can't do that while rooted."
    SPELL_FAILED_CUSTOM_ERROR_45        = 45,               // "Requires a nearby target."
    SPELL_FAILED_CUSTOM_ERROR_46        = 46,               // "Nothing left to discover."
    SPELL_FAILED_CUSTOM_ERROR_47        = 47,               // "No targets close enough to bluff."
    SPELL_FAILED_CUSTOM_ERROR_48        = 48,               // "Your Iron Rune Construct is out of range."
    SPELL_FAILED_CUSTOM_ERROR_49        = 49,               // "Requires Grand Master Engineer."
    SPELL_FAILED_CUSTOM_ERROR_50        = 50,               // "You can't use that mount."
    SPELL_FAILED_CUSTOM_ERROR_51        = 51,               // "There is nobody to eject!"
    SPELL_FAILED_CUSTOM_ERROR_52        = 52,               // "The target must be bound to you."
    SPELL_FAILED_CUSTOM_ERROR_53        = 53,               // "Target must be undead."
    SPELL_FAILED_CUSTOM_ERROR_54        = 54,               // "You have no target or your target is too far away."
    SPELL_FAILED_CUSTOM_ERROR_55        = 55,               // "Missing Reagents: Dark Matter"
    SPELL_FAILED_CUSTOM_ERROR_56        = 56,               // "You can't use that item."
    SPELL_FAILED_CUSTOM_ERROR_57        = 57,               // "You can't do that when Cycloned."
    SPELL_FAILED_CUSTOM_ERROR_58        = 58,               // "Target is already affected by a scroll."
    SPELL_FAILED_CUSTOM_ERROR_59        = 59,               // "That anti-venom is not strong enough to dispel that poison."
    SPELL_FAILED_CUSTOM_ERROR_60        = 60,               // "You must have a lance equipped."
    SPELL_FAILED_CUSTOM_ERROR_61        = 61,               // "You must be near the Maiden of Winter's Breath Lake"
    SPELL_FAILED_CUSTOM_ERROR_62        = 62,               // "You have learned everything from that book."
    SPELL_FAILED_CUSTOM_ERROR_63_NONE   = 63,               // "Your pet is dead"
    SPELL_FAILED_CUSTOM_ERROR_64_NONE   = 64,               // "There are no valid targets within range."
    SPELL_FAILED_CUSTOM_ERROR_65        = 65,               // "Only GMs may use that. Your account has been reported for investigation."
    SPELL_FAILED_CUSTOM_ERROR_66        = 66,               // "You must reach level 58 to use this portal."
    SPELL_FAILED_CUSTOM_ERROR_67        = 67,               // "You already have the maximum amount of honor."
    SPELL_FAILED_CUSTOM_ERROR_68        = 68,               // ""
    SPELL_FAILED_CUSTOM_ERROR_69        = 69,               // ""
    SPELL_FAILED_CUSTOM_ERROR_70        = 70,               // ""
    SPELL_FAILED_CUSTOM_ERROR_71        = 71,               // ""
    SPELL_FAILED_CUSTOM_ERROR_72        = 72,               // ""
    SPELL_FAILED_CUSTOM_ERROR_73        = 73,               // ""
    SPELL_FAILED_CUSTOM_ERROR_74        = 74,               // ""
    SPELL_FAILED_CUSTOM_ERROR_75        = 75,               // "You must have a demonic circle active."
    SPELL_FAILED_CUSTOM_ERROR_76        = 76,               // "You already have maximum rage"
    SPELL_FAILED_CUSTOM_ERROR_77        = 77,               // "Requires Engineering (350)"
    SPELL_FAILED_CUSTOM_ERROR_78        = 78,               // "Your soul belongs to the Lich King"
    SPELL_FAILED_CUSTOM_ERROR_79        = 79,               // "Your attendant already has an Argent Pony"
    SPELL_FAILED_CUSTOM_ERROR_80        = 80,               // ""
    SPELL_FAILED_CUSTOM_ERROR_81        = 81,               // ""
    SPELL_FAILED_CUSTOM_ERROR_82        = 82,               // ""
    SPELL_FAILED_CUSTOM_ERROR_83        = 83,               // "You must have a Fire Totem active."
    SPELL_FAILED_CUSTOM_ERROR_84        = 84,               // "You may not bite other vampires."
    SPELL_FAILED_CUSTOM_ERROR_85        = 85,               // "Your pet is already at your level."
    SPELL_FAILED_CUSTOM_ERROR_86        = 86,               // "You do not meet the level requirements for this item."
    SPELL_FAILED_CUSTOM_ERROR_87        = 87,               // "There are too many Mutated Abominations."
    SPELL_FAILED_CUSTOM_ERROR_88        = 88,               // "The potions have all been depleted by Professor Putricide."
    SPELL_FAILED_CUSTOM_ERROR_89        = 89,               // ""
    SPELL_FAILED_CUSTOM_ERROR_90        = 90,               // "Requires level 65"
    SPELL_FAILED_CUSTOM_ERROR_91        = 91,               // ""
    SPELL_FAILED_CUSTOM_ERROR_92        = 92,               // ""
    SPELL_FAILED_CUSTOM_ERROR_93        = 93,               // ""
    SPELL_FAILED_CUSTOM_ERROR_94        = 94,               // ""
    SPELL_FAILED_CUSTOM_ERROR_95        = 95,               // ""
    SPELL_FAILED_CUSTOM_ERROR_96        = 96,               // "You already have the max number of recruits."
    SPELL_FAILED_CUSTOM_ERROR_97        = 97,               // "You already have the max number of volunteers."
    SPELL_FAILED_CUSTOM_ERROR_98        = 98,               // "Frostmourne has rendered you unable to ressurect."
    SPELL_FAILED_CUSTOM_ERROR_99        = 99,               // "You can't mount while affected by that shapeshift."
};


enum SpellInterruptFlags
{
    SPELL_INTERRUPT_FLAG_MOVEMENT           = 0x01,
    SPELL_INTERRUPT_FLAG_DAMAGE_PUSHBACK    = 0x02, // Player only
    SPELL_INTERRUPT_FLAG_STUN               = 0x04, // not implemented until more research - many creature spells miss it
    SPELL_INTERRUPT_FLAG_COMBAT             = 0x08,
    SPELL_INTERRUPT_FLAG_DAMAGE_CANCELS     = 0x10, // Player only
    SPELL_INTERRUPT_FLAG_MELEE_COMBAT       = 0x20, // NYI
};

enum SpellAuraInterruptFlags // also used for ChannelInterruptFlags
{
    AURA_INTERRUPT_FLAG_HOSTILE_ACTION              = 0x00000001,   // 0    removed when getting hit by a negative spell
    AURA_INTERRUPT_FLAG_DAMAGE                      = 0x00000002,   // 1    removed by any damage
    AURA_INTERRUPT_FLAG_ACTION                      = 0x00000004,   // 2    removed by any cast
    AURA_INTERRUPT_FLAG_MOVING                      = 0x00000008,   // 3    removed by any movement
    AURA_INTERRUPT_FLAG_TURNING                     = 0x00000010,   // 4    removed by any turning
    AURA_INTERRUPT_FLAG_ANIM_CANCELS                = 0x00000020,   // 5    removed by anim
    AURA_INTERRUPT_FLAG_DISMOUNT                    = 0x00000040,   // 6    removed by unmounting
    AURA_INTERRUPT_FLAG_UNDERWATER_CANCELS          = 0x00000080,   // 7    removed by entering water
    AURA_INTERRUPT_FLAG_ABOVEWATER_CANCELS          = 0x00000100,   // 8    removed by leaving water
    AURA_INTERRUPT_FLAG_SHEATHING_CANCELS           = 0x00000200,   // 9    removed by unsheathing
    AURA_INTERRUPT_FLAG_INTERACTING                 = 0x00000400,   // 10   talk to npc / loot? action on creature
    AURA_INTERRUPT_FLAG_LOOTING                     = 0x00000800,   // 11   mine/use/open action on gameobject
    AURA_INTERRUPT_FLAG_ATTACKING                   = 0x00001000,   // 12   removed by attack
    AURA_INTERRUPT_FLAG_ITEM_USE                    = 0x00002000,   // 13
    AURA_INTERRUPT_FLAG_DAMAGE_CHANNEL_DURATION     = 0x00004000,   // 14
    AURA_INTERRUPT_FLAG_SHAPESHIFTING               = 0x00008000,   // 15
    AURA_INTERRUPT_FLAG_ACTION_LATE                 = 0x00010000,   // 16
    AURA_INTERRUPT_FLAG_MOUNTING                    = 0x00020000,   // 17   removed by mounting
    AURA_INTERRUPT_FLAG_STANDING_CANCELS            = 0x00040000,   // 18   removed by standing up (used by food and drink mostly and sleep/Fake Death like)
    AURA_INTERRUPT_FLAG_LEAVE_WORLD                 = 0x00080000,   // 19   leaving map/getting teleported
    AURA_INTERRUPT_FLAG_STEALTH_INVIS_CANCELS       = 0x00100000,   // 20
    AURA_INTERRUPT_FLAG_INVULNERABILITY_BUFF_CANCELS= 0x00200000,   // 21
    AURA_INTERRUPT_FLAG_ENTER_WORLD                 = 0x00400000,   // 22
    AURA_INTERRUPT_FLAG_PVP_ACTIVE_CANCELS          = 0x00800000,   // 23   removed by entering pvp combat
    AURA_INTERRUPT_FLAG_NON_PERIODIC_DAMAGE         = 0x01000000,   // 24   removed by any direct damage
    AURA_INTERRUPT_FLAG_LANDING_CANCELS             = 0x02000000,   // 25
    AURA_INTERRUPT_FLAG_RELEASE_CANCELS             = 0x04000000,   // 26
    AURA_INTERRUPT_FLAG_DAMAGE_CANCELS_SCRIPT       = 0x08000000,   // 27   unused
    AURA_INTERRUPT_FLAG_ENTERING_COMBAT_CANCELS     = 0x10000000,   // 28
    AURA_INTERRUPT_FLAG_LOGIN_CANCELS               = 0x20000000,   // 29
    AURA_INTERRUPT_FLAG_SUMMON_CANCELS              = 0x40000000,   // 30
    AURA_INTERRUPT_FLAG_LEAVING_COMBAT_CANCELS      = 0x80000000,   // 31
};

enum SpellFacingFlags
{
    SPELL_FACING_FLAG_INFRONT = 0x0001
};

enum SpellModOp
{
    SPELLMOD_DAMAGE                     = 0,
    SPELLMOD_DURATION                   = 1,
    SPELLMOD_THREAT                     = 2,
    SPELLMOD_EFFECT1                    = 3,
    SPELLMOD_CHARGES                    = 4,
    SPELLMOD_RANGE                      = 5,
    SPELLMOD_RADIUS                     = 6,
    SPELLMOD_CRITICAL_CHANCE            = 7,
    SPELLMOD_ALL_EFFECTS                = 8,
    SPELLMOD_NOT_LOSE_CASTING_TIME      = 9,
    SPELLMOD_CASTING_TIME               = 10,
    SPELLMOD_COOLDOWN                   = 11,
    SPELLMOD_EFFECT2                    = 12,
    SPELLMOD_UNK1                       = 13, // unused
    SPELLMOD_COST                       = 14,
    SPELLMOD_CRIT_DAMAGE_BONUS          = 15,
    SPELLMOD_RESIST_MISS_CHANCE         = 16,
    SPELLMOD_JUMP_TARGETS               = 17,
    SPELLMOD_CHANCE_OF_SUCCESS          = 18, // Only used with SPELL_AURA_ADD_FLAT_MODIFIER and affects proc spells
    SPELLMOD_ACTIVATION_TIME            = 19,
    SPELLMOD_EFFECT_PAST_FIRST          = 20,
    SPELLMOD_GLOBAL_COOLDOWN            = 21,
    SPELLMOD_DOT                        = 22,
    SPELLMOD_EFFECT3                    = 23,
    SPELLMOD_SPELL_BONUS_DAMAGE         = 24,
    SPELLMOD_UNK2                       = 25, // unused
    SPELLMOD_FREQUENCY_OF_SUCCESS       = 26, // Only used with SPELL_AURA_ADD_PCT_MODIFIER and affects used on proc spells
    SPELLMOD_MULTIPLE_VALUE             = 27,
    SPELLMOD_RESIST_DISPEL_CHANCE       = 28,
    MAX_SPELLMOD                        = 32,
};

// Spell triggering settings for CastSpell that enable us to skip some checks so that we can investigate spell specific settings
enum TriggerCastFlags : uint32
{
    TRIGGERED_NONE                              = 0x00000000,   // Not Triggered
    TRIGGERED_OLD_TRIGGERED                     = 0x00000001,   // Legacy bool support TODO: Restrict usage as much as possible.
    TRIGGERED_IGNORE_HIT_CALCULATION            = 0x00000002,   // Will ignore calculating hit in SpellHitResult
    TRIGGERED_IGNORE_UNSELECTABLE_FLAG          = 0x00000004,   // Ignores UNIT_FLAG_NOT_SELECTABLE in CheckTarget
    TRIGGERED_INSTANT_CAST                      = 0x00000008,   // Will ignore any cast time set in spell entry
    TRIGGERED_AUTOREPEAT                        = 0x00000010,   // Will signal spell system that this is internal autorepeat call
    TRIGGERED_IGNORE_UNATTACKABLE_FLAG          = 0x00000020,   // Ignores UNIT_FLAG_NOT_ATTACKABLE in CheckTarget
    TRIGGERED_DO_NOT_PROC                       = 0x00000040,   // Spells from scripts should not proc - DBScripts for example
    TRIGGERED_PET_CAST                          = 0x00000080,   // Spell that should report error through pet opcode
    TRIGGERED_NORMAL_COMBAT_CAST                = 0x00000100,   // AI needs to be notified about change of target TODO: change into TRIGGERED_NONE
    TRIGGERED_IGNORE_GCD                        = 0x00000200,   // Ignores GCD - to be used in spell scripts
    TRIGGERED_IGNORE_COSTS                      = 0x00000400,   // Ignores spell costs
    TRIGGERED_IGNORE_COOLDOWNS                  = 0x00000800,   // Ignores cooldowns
    TRIGGERED_IGNORE_CURRENT_CASTED_SPELL       = 0x00001000,   // Ignores concurrent casts and is not set as currently executed
    TRIGGERED_HIDE_CAST_IN_COMBAT_LOG           = 0x00002000,   // Sends cast flag for ignoring combat log display - used for many procs - default behaviour for triggered by aura
    TRIGGERED_DO_NOT_RESET_LEASH                = 0x00004000,   // Does not reset leash on cast
    TRIGGERED_CHANNEL_ONLY                      = 0x00008000,   // Only starts channel and no effects - used for summoning portal GO anims
    TRIGGERED_FULL_MASK                         = 0xFFFFFFFF
};

#endif
