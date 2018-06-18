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

/**
 * \addtogroup game
 * @{
 * \file
 */


#ifndef __UNIT_H
#define __UNIT_H

#include "Common.h"
#include "Entities/Object.h"
#include "Server/Opcodes.h"
#include "Spells/SpellAuraDefines.h"
#include "Entities/UpdateFields.h"
#include "Globals/SharedDefines.h"
#include "Combat/ThreatManager.h"
#include "Combat/HostileRefManager.h"
#include "MotionGenerators/FollowerReference.h"
#include "MotionGenerators/FollowerRefManager.h"
#include "Utilities/EventProcessor.h"
#include "MotionGenerators/MotionMaster.h"
#include "Server/DBCStructure.h"
#include "WorldPacket.h"
#include "Timer.h"
#include "AI/BaseAI/CreatureAI.h"

#include <list>

enum SpellInterruptFlags
{
    SPELL_INTERRUPT_FLAG_MOVEMENT     = 0x01,
    SPELL_INTERRUPT_FLAG_DAMAGE       = 0x02,
    SPELL_INTERRUPT_FLAG_INTERRUPT    = 0x04,
    SPELL_INTERRUPT_FLAG_AUTOATTACK   = 0x08,
    SPELL_INTERRUPT_FLAG_ABORT_ON_DMG = 0x10,               // _complete_ interrupt on direct damage
    // SPELL_INTERRUPT_UNK             = 0x20               // unk, 564 of 727 spells having this spell start with "Glyph"
};

enum SpellChannelInterruptFlags
{
    CHANNEL_FLAG_DAMAGE      = 0x0002,
    CHANNEL_FLAG_MOVEMENT    = 0x0008,
    CHANNEL_FLAG_TURNING     = 0x0010,
    CHANNEL_FLAG_DAMAGE2     = 0x0080,
    CHANNEL_FLAG_DELAY       = 0x4000
};

enum SpellAuraInterruptFlags
{
    AURA_INTERRUPT_FLAG_HITBYSPELL                  = 0x00000001,   // 0    removed when getting hit by a negative spell
    AURA_INTERRUPT_FLAG_DAMAGE                      = 0x00000002,   // 1    removed by any damage
    AURA_INTERRUPT_FLAG_UNK2                        = 0x00000004,   // 2
    AURA_INTERRUPT_FLAG_MOVE                        = 0x00000008,   // 3    removed by any movement
    AURA_INTERRUPT_FLAG_TURNING                     = 0x00000010,   // 4    removed by any turning
    AURA_INTERRUPT_FLAG_ENTER_COMBAT                = 0x00000020,   // 5    removed by entering combat
    AURA_INTERRUPT_FLAG_NOT_MOUNTED                 = 0x00000040,   // 6    removed by unmounting
    AURA_INTERRUPT_FLAG_NOT_ABOVEWATER              = 0x00000080,   // 7    removed by entering water
    AURA_INTERRUPT_FLAG_NOT_UNDERWATER              = 0x00000100,   // 8    removed by leaving water
    AURA_INTERRUPT_FLAG_NOT_SHEATHED                = 0x00000200,   // 9    removed by unsheathing
    AURA_INTERRUPT_FLAG_TALK                        = 0x00000400,   // 10   talk to npc / loot? action on creature
    AURA_INTERRUPT_FLAG_USE                         = 0x00000800,   // 11   mine/use/open action on gameobject
    AURA_INTERRUPT_FLAG_MELEE_ATTACK                = 0x00001000,   // 12   removed by attack
    AURA_INTERRUPT_FLAG_UNK13                       = 0x00002000,   // 13
    AURA_INTERRUPT_FLAG_UNK14                       = 0x00004000,   // 14
    AURA_INTERRUPT_FLAG_UNK15                       = 0x00008000,   // 15   removed by casting a spell?
    AURA_INTERRUPT_FLAG_UNK16                       = 0x00010000,   // 16
    AURA_INTERRUPT_FLAG_MOUNTING                    = 0x00020000,   // 17   removed by mounting
    AURA_INTERRUPT_FLAG_NOT_SEATED                  = 0x00040000,   // 18   removed by standing up (used by food and drink mostly and sleep/Fake Death like)
    AURA_INTERRUPT_FLAG_CHANGE_MAP                  = 0x00080000,   // 19   leaving map/getting teleported
    AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION    = 0x00100000,   // 20   removed by auras that make you invulnerable, or make other to loose selection on you
    AURA_INTERRUPT_FLAG_UNK21                       = 0x00200000,   // 21
    AURA_INTERRUPT_FLAG_TELEPORTED                  = 0x00400000,   // 22
    AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT            = 0x00800000,   // 23   removed by entering pvp combat
    AURA_INTERRUPT_FLAG_DIRECT_DAMAGE               = 0x01000000,   // 24   removed by any direct damage
    AURA_INTERRUPT_FLAG_LANDING                     = 0x02000000,   // 25   removed by hitting the ground or water
    AURA_INTERRUPT_FLAG_LEAVE_COMBAT                = 0x80000000,   // 31   removed by leaving combat
};

enum SpellPartialResist
{
    SPELL_PARTIAL_RESIST_NONE = 0,  // 0%, full hit
    SPELL_PARTIAL_RESIST_PCT_10,    // 10%
    SPELL_PARTIAL_RESIST_PCT_20,    // 20%
    SPELL_PARTIAL_RESIST_PCT_30,    // 30%
    SPELL_PARTIAL_RESIST_PCT_40,    // 40%
    SPELL_PARTIAL_RESIST_PCT_50,    // 50%
    SPELL_PARTIAL_RESIST_PCT_60,    // 60%
    SPELL_PARTIAL_RESIST_PCT_70,    // 70%
    SPELL_PARTIAL_RESIST_PCT_80,    // 80%
    SPELL_PARTIAL_RESIST_PCT_90,    // 90%
    SPELL_PARTIAL_RESIST_PCT_100,   // 100%, full resist
};

#define NUM_SPELL_PARTIAL_RESISTS 11

enum SpellModOp
{
    SPELLMOD_DAMAGE                 = 0,
    SPELLMOD_DURATION               = 1,
    SPELLMOD_THREAT                 = 2,
    SPELLMOD_EFFECT1                = 3,
    SPELLMOD_CHARGES                = 4,
    SPELLMOD_RANGE                  = 5,
    SPELLMOD_RADIUS                 = 6,
    SPELLMOD_CRITICAL_CHANCE        = 7,
    SPELLMOD_ALL_EFFECTS            = 8,
    SPELLMOD_NOT_LOSE_CASTING_TIME  = 9,
    SPELLMOD_CASTING_TIME           = 10,
    SPELLMOD_COOLDOWN               = 11,
    SPELLMOD_EFFECT2                = 12,
    // spellmod 13 unused
    SPELLMOD_COST                   = 14,
    SPELLMOD_CRIT_DAMAGE_BONUS      = 15,
    SPELLMOD_RESIST_MISS_CHANCE     = 16,
    SPELLMOD_JUMP_TARGETS           = 17,
    SPELLMOD_CHANCE_OF_SUCCESS      = 18,                   // Only used with SPELL_AURA_ADD_FLAT_MODIFIER and affects proc spells
    SPELLMOD_ACTIVATION_TIME        = 19,
    SPELLMOD_EFFECT_PAST_FIRST      = 20,
    SPELLMOD_GLOBAL_COOLDOWN        = 21,
    SPELLMOD_DOT                    = 22,
    SPELLMOD_EFFECT3                = 23,
    SPELLMOD_SPELL_BONUS_DAMAGE     = 24,
    // spellmod 25 unused
    SPELLMOD_FREQUENCY_OF_SUCCESS   = 26,                   // Only used with SPELL_AURA_ADD_PCT_MODIFIER and affects used on proc spells
    SPELLMOD_MULTIPLE_VALUE         = 27,
    SPELLMOD_RESIST_DISPEL_CHANCE   = 28
};

#define MAX_SPELLMOD 32

enum SpellFacingFlags
{
    SPELL_FACING_FLAG_INFRONT = 0x0001
};

#define BASE_MELEERANGE_OFFSET 1.33f
#define BASE_MINDAMAGE 1.0f
#define BASE_MAXDAMAGE 2.0f
#define BASE_ATTACK_TIME 2000

#define SCALE_SPELLPOWER_HEALING        1.88f

// byte value (UNIT_FIELD_BYTES_1,0)
enum UnitStandStateType
{
    UNIT_STAND_STATE_STAND             = 0,
    UNIT_STAND_STATE_SIT               = 1,
    UNIT_STAND_STATE_SIT_CHAIR         = 2,
    UNIT_STAND_STATE_SLEEP             = 3,
    UNIT_STAND_STATE_SIT_LOW_CHAIR     = 4,
    UNIT_STAND_STATE_SIT_MEDIUM_CHAIR  = 5,
    UNIT_STAND_STATE_SIT_HIGH_CHAIR    = 6,
    UNIT_STAND_STATE_DEAD              = 7,
    UNIT_STAND_STATE_KNEEL             = 8,
    UNIT_STAND_STATE_CUSTOM            = 9                  // Depends on model animation. Submerge, freeze, hide, hibernate, rest
};

#define MAX_UNIT_STAND_STATE             10

// byte flags value (UNIT_FIELD_BYTES_1,1)
// This corresponds to free talent points (pet case)

// byte flags value (UNIT_FIELD_BYTES_1,2)
enum UnitStandFlags
{
    UNIT_STAND_FLAGS_UNK1         = 0x01,
    UNIT_STAND_FLAGS_CREEP        = 0x02,
    UNIT_STAND_FLAGS_UNK3         = 0x04,
    UNIT_STAND_FLAGS_UNK4         = 0x08,
    UNIT_STAND_FLAGS_UNK5         = 0x10,
    UNIT_STAND_FLAGS_ALL          = 0xFF
};

// byte flags value (UNIT_FIELD_BYTES_1,3)
enum UnitBytes1_Flags
{
    UNIT_BYTE1_FLAG_ALWAYS_STAND = 0x01,
    UNIT_BYTE1_FLAG_FLY_ANIM     = 0x02,                    // Creature that can fly and are not on the ground appear to have this flag. If they are on the ground, flag is not present.
    UNIT_BYTE1_FLAG_UNTRACKABLE  = 0x04,
    UNIT_BYTE1_FLAG_ALL          = 0xFF
};

// byte value (UNIT_FIELD_BYTES_2,0)                        // TODO - solve conflicting with SharedDefines.h enum SheathTypes
enum SheathState
{
    SHEATH_STATE_UNARMED  = 0,                              // non prepared weapon
    SHEATH_STATE_MELEE    = 1,                              // prepared melee weapon
    SHEATH_STATE_RANGED   = 2                               // prepared ranged weapon
};

#define MAX_SHEATH_STATE    3

// byte flags value (UNIT_FIELD_BYTES_2,1)
enum UnitPVPStateFlags
{
    UNIT_BYTE2_FLAG_PVP         = 0x01,
    UNIT_BYTE2_FLAG_UNK1        = 0x02,
    UNIT_BYTE2_FLAG_FFA_PVP     = 0x04,
    UNIT_BYTE2_FLAG_SANCTUARY   = 0x08,
    UNIT_BYTE2_FLAG_AURAS       = 0x10,                     // show possitive auras as positive, and allow its dispel
    UNIT_BYTE2_FLAG_UNK5        = 0x20,                     // show negative auras as positive, *not* allowing dispel (at least for pets)
    UNIT_BYTE2_FLAG_UNK6        = 0x40,
    UNIT_BYTE2_FLAG_UNK7        = 0x80
};

// byte flags value (UNIT_FIELD_BYTES_2,2)
enum UnitRename
{
    UNIT_CAN_BE_RENAMED     = 0x01,
    UNIT_CAN_BE_ABANDONED   = 0x02,
};

// byte flags value (UNIT_FIELD_BYTES_2,3)                  See enum ShapeshiftForm in SharedDefines.h

#define CREATURE_MAX_SPELLS     8

enum Swing
{
    NOSWING                    = 0,
    SINGLEHANDEDSWING          = 1,
    TWOHANDEDSWING             = 2
};

enum VictimState
{
    VICTIMSTATE_UNAFFECTED     = 0,                         // seen in relation with HITINFO_MISS
    VICTIMSTATE_NORMAL         = 1,
    VICTIMSTATE_DODGE          = 2,
    VICTIMSTATE_PARRY          = 3,
    VICTIMSTATE_INTERRUPT      = 4,
    VICTIMSTATE_BLOCKS         = 5,
    VICTIMSTATE_EVADES         = 6,
    VICTIMSTATE_IS_IMMUNE      = 7,
    VICTIMSTATE_DEFLECTS       = 8
};

enum HitInfo
{
    HITINFO_NORMALSWING         = 0x00000000,
    HITINFO_UNK0                = 0x00000001,               // req correct packet structure
    HITINFO_NORMALSWING2        = 0x00000002,
    HITINFO_LEFTSWING           = 0x00000004,
    HITINFO_UNK3                = 0x00000008,
    HITINFO_MISS                = 0x00000010,
    HITINFO_ABSORB              = 0x00000020,               // absorbed damage
    HITINFO_ABSORB2             = 0x00000040,               // absorbed damage
    HITINFO_RESIST              = 0x00000080,               // resisted atleast some damage
    HITINFO_RESIST2             = 0x00000100,               // resisted atleast some damage
    HITINFO_CRITICALHIT         = 0x00000200,               // critical hit
    // 0x00000400
    // 0x00000800
    // 0x00001000
    HITINFO_BLOCK               = 0x00002000,               // blocked damage
    // 0x00004000
    // 0x00008000
    HITINFO_GLANCING            = 0x00010000,
    HITINFO_CRUSHING            = 0x00020000,
    HITINFO_NOACTION            = 0x00040000,               // guessed
    // 0x00080000
    // 0x00100000
    HITINFO_SWINGNOHITSOUND     = 0x00200000,               // guessed
    // 0x00400000
    HITINFO_UNK22               = 0x00800000
};

struct FactionTemplateEntry;
struct Modifier;
struct SpellEntry;
struct SpellEntryExt;

class Aura;
class SpellAuraHolder;
class Creature;
class Spell;
class DynamicObject;
class GameObject;
class Item;
class Pet;
class PetAura;
class Totem;
class VehicleInfo;

struct SpellImmune
{
    uint32 type;
    Aura const* aura;
};

typedef std::list<SpellImmune> SpellImmuneList;

enum UnitModifierType
{
    BASE_VALUE = 0,
    BASE_EXCLUSIVE = 1,
    BASE_PCT = 2,
    TOTAL_VALUE = 3,
    TOTAL_PCT = 4,
    MODIFIER_TYPE_END = 5
};

enum WeaponDamageRange
{
    MINDAMAGE,
    MAXDAMAGE
};

enum DamageTypeToSchool
{
    RESISTANCE,
    DAMAGE_DEALT,
    DAMAGE_TAKEN
};

enum AuraRemoveMode
{
    AURA_REMOVE_BY_DEFAULT,
    AURA_REMOVE_BY_STACK,                                   // at replace by similar aura
    AURA_REMOVE_BY_CANCEL,
    AURA_REMOVE_BY_DISPEL,
    AURA_REMOVE_BY_DEATH,
    AURA_REMOVE_BY_DELETE,                                  // use for speedup and prevent unexpected effects at player logout/pet unsummon (must be used _only_ after save), delete.
    AURA_REMOVE_BY_SHIELD_BREAK,                            // when absorb shield is removed by damage, heal absorb debuf
    AURA_REMOVE_BY_EXPIRE,                                  // at duration end
    AURA_REMOVE_BY_TRACKING,                                // aura is removed because of a conflicting tracked aura
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
    TRIGGERED_FULL_MASK                         = 0xFFFFFFFF
};

enum UnitMods
{
    UNIT_MOD_STAT_STRENGTH,                                 // UNIT_MOD_STAT_STRENGTH..UNIT_MOD_STAT_SPIRIT must be in existing order, it's accessed by index values of Stats enum.
    UNIT_MOD_STAT_AGILITY,
    UNIT_MOD_STAT_STAMINA,
    UNIT_MOD_STAT_INTELLECT,
    UNIT_MOD_STAT_SPIRIT,
    UNIT_MOD_HEALTH,
    UNIT_MOD_MANA,                                          // UNIT_MOD_MANA..UNIT_MOD_RUNIC_POWER must be in existing order, it's accessed by index values of Powers enum.
    UNIT_MOD_RAGE,
    UNIT_MOD_FOCUS,
    UNIT_MOD_ENERGY,
    UNIT_MOD_HAPPINESS,
    UNIT_MOD_RUNE,
    UNIT_MOD_RUNIC_POWER,
    UNIT_MOD_ARMOR,                                         // UNIT_MOD_ARMOR..UNIT_MOD_RESISTANCE_ARCANE must be in existing order, it's accessed by index values of SpellSchools enum.
    UNIT_MOD_RESISTANCE_HOLY,
    UNIT_MOD_RESISTANCE_FIRE,
    UNIT_MOD_RESISTANCE_NATURE,
    UNIT_MOD_RESISTANCE_FROST,
    UNIT_MOD_RESISTANCE_SHADOW,
    UNIT_MOD_RESISTANCE_ARCANE,
    UNIT_MOD_ATTACK_POWER,
    UNIT_MOD_ATTACK_POWER_RANGED,
    UNIT_MOD_DAMAGE_MAINHAND,
    UNIT_MOD_DAMAGE_OFFHAND,
    UNIT_MOD_DAMAGE_RANGED,
    UNIT_MOD_END,
    // synonyms
    UNIT_MOD_STAT_START = UNIT_MOD_STAT_STRENGTH,
    UNIT_MOD_STAT_END = UNIT_MOD_STAT_SPIRIT + 1,
    UNIT_MOD_RESISTANCE_START = UNIT_MOD_ARMOR,
    UNIT_MOD_RESISTANCE_END = UNIT_MOD_RESISTANCE_ARCANE + 1,
    UNIT_MOD_POWER_START = UNIT_MOD_MANA,
    UNIT_MOD_POWER_END = UNIT_MOD_RUNIC_POWER + 1
};

enum BaseModGroup
{
    CRIT_PERCENTAGE,
    RANGED_CRIT_PERCENTAGE,
    OFFHAND_CRIT_PERCENTAGE,
    SHIELD_BLOCK_VALUE,
    BASEMOD_END
};

enum BaseModType
{
    FLAT_MOD,
    PCT_MOD,
    MOD_END
};

enum DeathState
{
    ALIVE          = 0,                                     // show as alive
    JUST_DIED      = 1,                                     // temporary state at die, for creature auto converted to CORPSE, for player at next update call
    CORPSE         = 2,                                     // corpse state, for player this also meaning that player not leave corpse
    DEAD           = 3,                                     // for creature despawned state (corpse despawned), for player CORPSE/DEAD not clear way switches (FIXME), and use m_deathtimer > 0 check for real corpse state
    JUST_ALIVED    = 4,                                     // temporary state at resurrection, for creature auto converted to ALIVE, for player at next update call
};

// internal state flags for some auras and movement generators, other.
enum UnitState
{
    // persistent state (applied by aura/etc until expire)
    UNIT_STAT_MELEE_ATTACKING = 0x00000001,                 // unit is melee attacking someone Unit::Attack
    //UNIT_STAT_ATTACK_PLAYER = 0x00000002,                 // (Deprecated) unit attack player or player's controlled unit and have contested pvpv timer setup, until timer expire, combat end and etc
    UNIT_STAT_FEIGN_DEATH     = 0x00000004,                 // Unit::SetFeignDeath - a successful feign death is currently active
    UNIT_STAT_STUNNED         = 0x00000008,                 // Aura::HandleAuraModStun
    UNIT_STAT_ROOT            = 0x00000010,                 // Aura::HandleAuraModRoot
    UNIT_STAT_ISOLATED        = 0x00000020,                 // area auras do not affect other players, Aura::HandleAuraModSchoolImmunity
    UNIT_STAT_POSSESSED       = 0x00000040,                 // Aura::HandleAuraModPossess (duplicates UNIT_FLAG_POSSESSED)

    // persistent movement generator state (all time while movement generator applied to unit (independent from top state of movegen)
    UNIT_STAT_TAXI_FLIGHT     = 0x00000080,                 // player is in flight mode (in fact interrupted at far teleport until next map telport landing)
    UNIT_STAT_DISTRACTED      = 0x00000100,                 // DistractedMovementGenerator active

    // persistent movement generator state with non-persistent mirror states for stop support
    // (can be removed temporary by stop command or another movement generator apply)
    // not use _MOVE versions for generic movegen state, it can be removed temporary for unit stop and etc
    UNIT_STAT_CONFUSED        = 0x00000200,                 // ConfusedMovementGenerator active/onstack
    UNIT_STAT_CONFUSED_MOVE   = 0x00000400,
    UNIT_STAT_ROAMING         = 0x00000800,                 // RandomMovementGenerator/PointMovementGenerator/WaypointMovementGenerator active (now always set)
    UNIT_STAT_ROAMING_MOVE    = 0x00001000,
    UNIT_STAT_CHASE           = 0x00002000,                 // ChaseMovementGenerator active
    UNIT_STAT_CHASE_MOVE      = 0x00004000,
    UNIT_STAT_FOLLOW          = 0x00008000,                 // FollowMovementGenerator active
    UNIT_STAT_FOLLOW_MOVE     = 0x00010000,
    UNIT_STAT_FLEEING         = 0x00020000,                 // FleeMovementGenerator/TimedFleeingMovementGenerator active/onstack
    UNIT_STAT_FLEEING_MOVE    = 0x00040000,
    UNIT_STAT_SEEKING_ASSISTANCE = 0x00080000,
    UNIT_STAT_DONT_TURN       = 0x00100000,                 // Creature will not turn and acquire new target
    UNIT_STAT_CHANNELING      = 0x00200000,
    // More room for other MMGens

    // High-Level states (usually only with Creatures)
    UNIT_STAT_NO_COMBAT_MOVEMENT    = 0x01000000,           // Combat Movement for MoveChase stopped
    UNIT_STAT_RUNNING               = 0x02000000,           // SetRun for waypoints and such
    UNIT_STAT_WAYPOINT_PAUSED       = 0x04000000,           // Waypoint-Movement paused genericly (ie by script)

    UNIT_STAT_IGNORE_PATHFINDING    = 0x10000000,           // do not use pathfinding in any MovementGenerator

    // masks (only for check)

    // can't move currently
    UNIT_STAT_CAN_NOT_MOVE    = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_FEIGN_DEATH,

    // stay by different reasons
    UNIT_STAT_NOT_MOVE        = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_DISTRACTED,

    // stay or scripted movement for effect( = in player case you can't move by client command)
    UNIT_STAT_NO_FREE_MOVE    = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_TAXI_FLIGHT |
                                UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING,

    // not react at move in sight or other
    UNIT_STAT_CAN_NOT_REACT   = UNIT_STAT_STUNNED | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING,

    // AI disabled by some reason
    UNIT_STAT_LOST_CONTROL    = UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING | UNIT_STAT_POSSESSED,

    // above 2 state cases
    UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL  = UNIT_STAT_CAN_NOT_REACT | UNIT_STAT_LOST_CONTROL,

    // masks (for check or reset)

    // for real move using movegen check and stop (except unstoppable flight)
    UNIT_STAT_MOVING          = UNIT_STAT_ROAMING_MOVE | UNIT_STAT_CHASE_MOVE | UNIT_STAT_FOLLOW_MOVE | UNIT_STAT_FLEEING_MOVE,

    UNIT_STAT_RUNNING_STATE   = UNIT_STAT_CHASE_MOVE | UNIT_STAT_FLEEING_MOVE | UNIT_STAT_RUNNING,

    UNIT_STAT_ALL_STATE       = 0xFFFFFFFF,
    UNIT_STAT_ALL_DYN_STATES  = UNIT_STAT_ALL_STATE & ~(UNIT_STAT_NO_COMBAT_MOVEMENT | UNIT_STAT_RUNNING | UNIT_STAT_WAYPOINT_PAUSED | UNIT_STAT_IGNORE_PATHFINDING),
};

enum UnitMoveType
{
    MOVE_WALK           = 0,
    MOVE_RUN            = 1,
    MOVE_RUN_BACK       = 2,
    MOVE_SWIM           = 3,
    MOVE_SWIM_BACK      = 4,
    MOVE_TURN_RATE      = 5,
    MOVE_FLIGHT         = 6,
    MOVE_FLIGHT_BACK    = 7,
    MOVE_PITCH_RATE     = 8
};

#define MAX_MOVE_TYPE     9

#define BASE_CHARGE_SPEED 27.0f

enum CombatRating
{
    CR_WEAPON_SKILL             = 0,
    CR_DEFENSE_SKILL            = 1,
    CR_DODGE                    = 2,
    CR_PARRY                    = 3,
    CR_BLOCK                    = 4,
    CR_HIT_MELEE                = 5,
    CR_HIT_RANGED               = 6,
    CR_HIT_SPELL                = 7,
    CR_CRIT_MELEE               = 8,
    CR_CRIT_RANGED              = 9,
    CR_CRIT_SPELL               = 10,
    CR_HIT_TAKEN_MELEE          = 11,
    CR_HIT_TAKEN_RANGED         = 12,
    CR_HIT_TAKEN_SPELL          = 13,
    CR_CRIT_TAKEN_MELEE         = 14,
    CR_CRIT_TAKEN_RANGED        = 15,
    CR_CRIT_TAKEN_SPELL         = 16,
    CR_HASTE_MELEE              = 17,
    CR_HASTE_RANGED             = 18,
    CR_HASTE_SPELL              = 19,
    CR_WEAPON_SKILL_MAINHAND    = 20,
    CR_WEAPON_SKILL_OFFHAND     = 21,
    CR_WEAPON_SKILL_RANGED      = 22,
    CR_EXPERTISE                = 23,
    CR_ARMOR_PENETRATION        = 24
};

#define MAX_COMBAT_RATING         25

/// internal used flags for marking special auras - for example some dummy-auras
enum UnitAuraFlags
{
    UNIT_AURAFLAG_ALIVE_INVISIBLE   = 0x1,                  // aura which makes unit invisible for alive
};

enum UnitVisibility
{
    VISIBILITY_OFF                = 0,                      // absolute, not detectable, GM-like, can see all other
    VISIBILITY_ON                 = 1,
    VISIBILITY_GROUP_STEALTH      = 2,                      // detect chance, seen and can see group members
    VISIBILITY_GROUP_INVISIBILITY = 3,                      // invisibility, can see and can be seen only another invisible unit or invisible detection unit, set only if not stealthed, and in checks not used (mask used instead)
    VISIBILITY_GROUP_NO_DETECT    = 4,                      // state just at stealth apply for update Grid state. Don't remove, otherwise stealth spells will break
    VISIBILITY_REMOVE_CORPSE      = 5                       // special totally not detectable visibility for force delete object while removing a corpse
};

// Value masks for UNIT_FIELD_FLAGS
enum UnitFlags
{
    UNIT_FLAG_UNK_0                 = 0x00000001,           // Movement checks disabled, likely paired with loss of client control packet.
    UNIT_FLAG_NON_ATTACKABLE        = 0x00000002,           // not attackable
    UNIT_FLAG_CLIENT_CONTROL_LOST   = 0x00000004,           // Generic unspecified loss of control initiated by server script, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_PLAYER_CONTROLLED     = 0x00000008,           // players, pets, totems, guardians, companions, charms, any units associated with players
    UNIT_FLAG_RENAME                = 0x00000010,
    UNIT_FLAG_PREPARATION           = 0x00000020,           // don't take reagents for spells with SPELL_ATTR_EX5_NO_REAGENT_WHILE_PREP
    UNIT_FLAG_UNK_6                 = 0x00000040,
    UNIT_FLAG_NOT_ATTACKABLE_1      = 0x00000080,           // ?? (UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1) is NON_PVP_ATTACKABLE
    UNIT_FLAG_IMMUNE_TO_PLAYER      = 0x00000100,           // Target is immune to players
    UNIT_FLAG_IMMUNE_TO_NPC         = 0x00000200,           // makes you unable to attack everything. Almost identical to our "civilian"-term. Will ignore it's surroundings and not engage in combat unless "called upon" or engaged by another unit.
    UNIT_FLAG_LOOTING               = 0x00000400,           // loot animation
    UNIT_FLAG_PET_IN_COMBAT         = 0x00000800,           // in combat?, 2.0.8
    UNIT_FLAG_PVP_DEPRECATED        = 0x00001000,           // changed in 3.0.3
    UNIT_FLAG_SILENCED              = 0x00002000,           // silenced, 2.1.1
    UNIT_FLAG_UNK_14                = 0x00004000,           // 2.0.8
    UNIT_FLAG_SWIMMING              = 0x00008000,           // related to jerky movement in water?
    UNIT_FLAG_UNK_16                = 0x00010000,           // removes attackable icon
    UNIT_FLAG_PACIFIED              = 0x00020000,           // 3.0.3 ok
    UNIT_FLAG_STUNNED               = 0x00040000,           // Unit is a subject to stun, turn and strafe movement disabled
    UNIT_FLAG_IN_COMBAT             = 0x00080000,
    UNIT_FLAG_TAXI_FLIGHT           = 0x00100000,           // Unit is on taxi, paired with a duplicate loss of client control packet (likely a legacy serverside hack). Disables any spellcasts not allowed in taxi flight client-side.
    UNIT_FLAG_DISARMED              = 0x00200000,           // 3.0.3, disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
    UNIT_FLAG_CONFUSED              = 0x00400000,           // Unit is a subject to confused movement, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_FLEEING               = 0x00800000,           // Unit is a subject to fleeing movement, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_POSSESSED             = 0x01000000,           // Unit is under remote control by another unit, movement checks disabled, paired with loss of client control packet. New master is allowed to use melee attack and can't select this unit via mouse in the world (as if it was own character).
    UNIT_FLAG_NOT_SELECTABLE        = 0x02000000,
    UNIT_FLAG_SKINNABLE             = 0x04000000,
    UNIT_FLAG_MOUNT                 = 0x08000000,
    UNIT_FLAG_UNK_28                = 0x10000000,
    UNIT_FLAG_UNK_29                = 0x20000000,           // used in Feing Death spell
    UNIT_FLAG_SHEATHE               = 0x40000000,
    UNIT_FLAG_UNK_31                = 0x80000000            // set skinnable icon and also changes color of portrait
};

// Value masks for UNIT_FIELD_FLAGS_2
enum UnitFlags2
{
    UNIT_FLAG2_FEIGN_DEATH          = 0x00000001,
    UNIT_FLAG2_UNK1                 = 0x00000002,           // Hides body and body armor. Weapons and shoulder and head armor still visible
    UNIT_FLAG2_IGNORE_REPUTATION    = 0x00000004,
    UNIT_FLAG2_COMPREHEND_LANG      = 0x00000008,
    UNIT_FLAG2_CLONED               = 0x00000010,           // Used in SPELL_AURA_MIRROR_IMAGE
    UNIT_FLAG2_UNK5                 = 0x00000020,
    UNIT_FLAG2_FORCE_MOVE           = 0x00000040,
    UNIT_FLAG2_DISARM_OFFHAND       = 0x00000080,           // also shield case
    UNIT_FLAG2_UNK8                 = 0x00000100,
    UNIT_FLAG2_UNK9                 = 0x00000200,
    UNIT_FLAG2_DISARM_RANGED        = 0x00000400,
    UNIT_FLAG2_REGENERATE_POWER     = 0x00000800,
    UNIT_FLAG2_SPELL_CLICK_IN_GROUP = 0x00001000,
    UNIT_FLAG2_SPELL_CLICK_DISABLED = 0x00002000,
    UNIT_FLAG2_INTERACT_ANY_REACTION = 0x00004000,
    UNIT_FLAG2_UNK15                = 0x00008000,
    UNIT_FLAG2_UNK16                = 0x00010000,
};

/// Non Player Character flags
enum NPCFlags
{
    UNIT_NPC_FLAG_NONE                  = 0x00000000,
    UNIT_NPC_FLAG_GOSSIP                = 0x00000001,       // 100%
    UNIT_NPC_FLAG_QUESTGIVER            = 0x00000002,       // guessed, probably ok
    UNIT_NPC_FLAG_UNK1                  = 0x00000004,
    UNIT_NPC_FLAG_UNK2                  = 0x00000008,
    UNIT_NPC_FLAG_TRAINER               = 0x00000010,       // 100%
    UNIT_NPC_FLAG_TRAINER_CLASS         = 0x00000020,       // 100%
    UNIT_NPC_FLAG_TRAINER_PROFESSION    = 0x00000040,       // 100%
    UNIT_NPC_FLAG_VENDOR                = 0x00000080,       // 100%
    UNIT_NPC_FLAG_VENDOR_AMMO           = 0x00000100,       // 100%, general goods vendor
    UNIT_NPC_FLAG_VENDOR_FOOD           = 0x00000200,       // 100%
    UNIT_NPC_FLAG_VENDOR_POISON         = 0x00000400,       // guessed
    UNIT_NPC_FLAG_VENDOR_REAGENT        = 0x00000800,       // 100%
    UNIT_NPC_FLAG_REPAIR                = 0x00001000,       // 100%
    UNIT_NPC_FLAG_FLIGHTMASTER          = 0x00002000,       // 100%
    UNIT_NPC_FLAG_SPIRITHEALER          = 0x00004000,       // guessed
    UNIT_NPC_FLAG_SPIRITGUIDE           = 0x00008000,       // guessed
    UNIT_NPC_FLAG_INNKEEPER             = 0x00010000,       // 100%
    UNIT_NPC_FLAG_BANKER                = 0x00020000,       // 100%
    UNIT_NPC_FLAG_PETITIONER            = 0x00040000,       // 100% 0xC0000 = guild petitions, 0x40000 = arena team petitions
    UNIT_NPC_FLAG_TABARDDESIGNER        = 0x00080000,       // 100%
    UNIT_NPC_FLAG_BATTLEMASTER          = 0x00100000,       // 100%
    UNIT_NPC_FLAG_AUCTIONEER            = 0x00200000,       // 100%
    UNIT_NPC_FLAG_STABLEMASTER          = 0x00400000,       // 100%
    UNIT_NPC_FLAG_GUILD_BANKER          = 0x00800000,       // cause client to send 997 opcode
    UNIT_NPC_FLAG_SPELLCLICK            = 0x01000000,       // cause client to send 1015 opcode (spell click), dynamic, set at loading and don't must be set in DB
    UNIT_NPC_FLAG_PLAYER_VEHICLE        = 0x02000000,       // players with mounts that have vehicle data should have it set
};

// used in most movement packets (send and received)
enum MovementFlags
{
    MOVEFLAG_NONE               = 0x00000000,
    MOVEFLAG_FORWARD            = 0x00000001,
    MOVEFLAG_BACKWARD           = 0x00000002,
    MOVEFLAG_STRAFE_LEFT        = 0x00000004,
    MOVEFLAG_STRAFE_RIGHT       = 0x00000008,
    MOVEFLAG_TURN_LEFT          = 0x00000010,
    MOVEFLAG_TURN_RIGHT         = 0x00000020,
    MOVEFLAG_PITCH_UP           = 0x00000040,
    MOVEFLAG_PITCH_DOWN         = 0x00000080,
    MOVEFLAG_WALK_MODE          = 0x00000100,               // Walking
    MOVEFLAG_ONTRANSPORT        = 0x00000200,
    MOVEFLAG_LEVITATING         = 0x00000400,
    MOVEFLAG_ROOT               = 0x00000800,
    MOVEFLAG_FALLING            = 0x00001000,
    MOVEFLAG_FALLINGFAR         = 0x00002000,
    MOVEFLAG_PENDINGSTOP        = 0x00004000,
    MOVEFLAG_PENDINGSTRAFESTOP  = 0x00008000,
    MOVEFLAG_PENDINGFORWARD     = 0x00010000,
    MOVEFLAG_PENDINGBACKWARD    = 0x00020000,
    MOVEFLAG_PENDINGSTRAFELEFT  = 0x00040000,
    MOVEFLAG_PENDINGSTRAFERIGHT = 0x00080000,
    MOVEFLAG_PENDINGROOT        = 0x00100000,
    MOVEFLAG_SWIMMING           = 0x00200000,               // appears with fly flag also
    MOVEFLAG_ASCENDING          = 0x00400000,               // swim up also
    MOVEFLAG_DESCENDING         = 0x00800000,               // swim down also
    MOVEFLAG_CAN_FLY            = 0x01000000,               // can fly in 3.3?
    MOVEFLAG_FLYING             = 0x02000000,               // Actual flying mode
    MOVEFLAG_SPLINE_ELEVATION   = 0x04000000,               // used for flight paths
    MOVEFLAG_SPLINE_ENABLED     = 0x08000000,               // used for flight paths
    MOVEFLAG_WATERWALKING       = 0x10000000,               // prevent unit from falling through water
    MOVEFLAG_SAFE_FALL          = 0x20000000,               // active rogue safe fall spell (passive)
    MOVEFLAG_HOVER              = 0x40000000
};

// flags that use in movement check for example at spell casting
MovementFlags const movementFlagsMask = MovementFlags(
        MOVEFLAG_FORWARD | MOVEFLAG_BACKWARD  | MOVEFLAG_STRAFE_LEFT | MOVEFLAG_STRAFE_RIGHT |
        MOVEFLAG_PITCH_UP | MOVEFLAG_PITCH_DOWN |
        MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR | MOVEFLAG_ASCENDING   |
        MOVEFLAG_FLYING  | MOVEFLAG_SPLINE_ELEVATION
                                        );

MovementFlags const movementOrTurningFlagsMask = MovementFlags(
            movementFlagsMask | MOVEFLAG_TURN_LEFT | MOVEFLAG_TURN_RIGHT
        );

enum MovementFlags2
{
    MOVEFLAG2_NONE              = 0x0000,
    MOVEFLAG2_NO_STRAFE         = 0x0001,
    MOVEFLAG2_NO_JUMPING        = 0x0002,
    MOVEFLAG2_UNK3              = 0x0004,
    MOVEFLAG2_FULLSPEEDTURNING  = 0x0008,
    MOVEFLAG2_FULLSPEEDPITCHING = 0x0010,
    MOVEFLAG2_ALLOW_PITCHING    = 0x0020,
    MOVEFLAG2_UNK4              = 0x0040,
    MOVEFLAG2_UNK5              = 0x0080,
    MOVEFLAG2_UNK6              = 0x0100,                   // transport related
    MOVEFLAG2_UNK7              = 0x0200,
    MOVEFLAG2_INTERP_MOVEMENT   = 0x0400,
    MOVEFLAG2_INTERP_TURNING    = 0x0800,
    MOVEFLAG2_INTERP_PITCHING   = 0x1000,
    MOVEFLAG2_UNK8              = 0x2000,
    MOVEFLAG2_UNK9              = 0x4000,
    MOVEFLAG2_UNK10             = 0x8000,
    MOVEFLAG2_INTERP_MASK       = MOVEFLAG2_INTERP_MOVEMENT | MOVEFLAG2_INTERP_TURNING | MOVEFLAG2_INTERP_PITCHING
};

class MovementInfo
{
    public:
        MovementInfo() : moveFlags(MOVEFLAG_NONE), moveFlags2(MOVEFLAG2_NONE), time(0),
            t_time(0), t_seat(-1), t_time2(0), s_pitch(0.0f), fallTime(0), u_unk1(0.0f) {}

        // Read/Write methods
        void Read(ByteBuffer& data);
        void Write(ByteBuffer& data) const;

        // Movement flags manipulations
        void AddMovementFlag(MovementFlags f) { moveFlags |= f; }
        void RemoveMovementFlag(MovementFlags f) { moveFlags &= ~f; }
        bool HasMovementFlag(MovementFlags f) const { return !!(moveFlags & f); }
        MovementFlags GetMovementFlags() const { return MovementFlags(moveFlags); }
        void SetMovementFlags(MovementFlags f) { moveFlags = f; }
        MovementFlags2 GetMovementFlags2() const { return MovementFlags2(moveFlags2); }
        void AddMovementFlags2(MovementFlags2 f) { moveFlags2 |= f; }

        // Position manipulations
        Position const* GetPos() const { return &pos; }
        void SetTransportData(ObjectGuid guid, float x, float y, float z, float o, uint32 time, int8 seat)
        {
            t_guid = guid;
            t_pos.x = x;
            t_pos.y = y;
            t_pos.z = z;
            t_pos.o = o;
            t_time = time;
            t_seat = seat;
        }
        void ClearTransportData()
        {
            t_guid = ObjectGuid();
            t_pos.x = 0.0f;
            t_pos.y = 0.0f;
            t_pos.z = 0.0f;
            t_pos.o = 0.0f;
            t_time = 0;
            t_seat = -1;
        }
        ObjectGuid const& GetTransportGuid() const { return t_guid; }
        Position const* GetTransportPos() const { return &t_pos; }
        int8 GetTransportSeat() const { return t_seat; }
        uint32 GetTransportTime() const { return t_time; }
        uint32 GetFallTime() const { return fallTime; }
        void ChangeOrientation(float o) { pos.o = o; }
        void ChangePosition(float x, float y, float z, float o) { pos.x = x; pos.y = y; pos.z = z; pos.o = o; }
        void UpdateTime(uint32 _time) { time = _time; }
        uint32 GetTime() const { return time; }

        struct JumpInfo
        {
            JumpInfo() : velocity(0.f), sinAngle(0.f), cosAngle(0.f), xyspeed(0.f) {}
            float   velocity, sinAngle, cosAngle, xyspeed;
        };

        JumpInfo const& GetJumpInfo() const { return jump; }
    private:
        // common
        uint32   moveFlags;                                 // see enum MovementFlags
        uint16   moveFlags2;                                // see enum MovementFlags2
        uint32   time;
        Position pos;
        // transport
        ObjectGuid t_guid;
        Position t_pos;
        uint32   t_time;
        int8     t_seat;
        uint32   t_time2;
        // swimming and flying
        float    s_pitch;
        // last fall time
        uint32   fallTime;
        // jumping
        JumpInfo jump;
        // spline
        float    u_unk1;
};

inline ByteBuffer& operator<< (ByteBuffer& buf, MovementInfo const& mi)
{
    mi.Write(buf);
    return buf;
}

inline ByteBuffer& operator>> (ByteBuffer& buf, MovementInfo& mi)
{
    mi.Read(buf);
    return buf;
}

namespace Movement
{
    class MoveSpline;
}

/**
 * The different available diminishing return levels.
 * \see DiminishingReturn
 */
enum DiminishingLevels
{
    DIMINISHING_LEVEL_1             = 0,         //< Won't make a difference to stun duration
    DIMINISHING_LEVEL_2             = 1,         //< Reduces stun time by 50%
    DIMINISHING_LEVEL_3             = 2,         //< Reduces stun time by 75%
    DIMINISHING_LEVEL_IMMUNE        = 3          //< The target is immune to the DiminishingGrouop
};

/**
 * Structure to keep track of diminishing returns, for more information
 * about the idea behind diminishing returns, see: http://www.wowwiki.com/Diminishing_returns
 * \see Unit::GetDiminishing
 * \see Unit::IncrDiminishing
 * \see Unit::ApplyDiminishingToDuration
 * \see Unit::ApplyDiminishingAura
 */
struct DiminishingReturn
{
    DiminishingReturn(DiminishingGroup group, uint32 t, uint32 count)
        : DRGroup(group), stack(0), hitTime(t), hitCount(count)
    {}

    /**
     * Group that this diminishing return will affect
     */
    DiminishingGroup        DRGroup: 16;
    /**
     * Seems to be how many times this has been stacked, modified in
     * Unit::ApplyDiminishingAura
     */
    uint16                  stack: 16;
    /**
     * Records at what time the last hit with this DiminishingGroup was done, if it's
     * higher than 15 seconds (ie: 15 000 ms) the DiminishingReturn::hitCount will be reset
     * to DiminishingLevels::DIMINISHING_LEVEL_1, which will do no difference to the duration
     * of the stun etc.
     */
    uint32                  hitTime;
    /**
     * Records how many times a spell of this DiminishingGroup has hit, this in turn
     * decides how how long the duration of the stun etc is.
     */
    uint32                  hitCount;
};

// At least some values expected fixed and used in auras field, other custom
enum MeleeHitOutcome
{
    MELEE_HIT_EVADE     = 0,
    MELEE_HIT_MISS      = 1,
    MELEE_HIT_DODGE     = 2,                                // used as misc in SPELL_AURA_IGNORE_COMBAT_RESULT
    MELEE_HIT_BLOCK     = 3,                                // used as misc in SPELL_AURA_IGNORE_COMBAT_RESULT
    MELEE_HIT_PARRY     = 4,                                // used as misc in SPELL_AURA_IGNORE_COMBAT_RESULT
    MELEE_HIT_GLANCING  = 5,
    MELEE_HIT_CRIT      = 6,
    MELEE_HIT_CRUSHING  = 7,
    MELEE_HIT_NORMAL    = 8,
};

enum UnitCombatDieSide
{
    UNIT_COMBAT_DIE_MISS,
    UNIT_COMBAT_DIE_RESIST,
    UNIT_COMBAT_DIE_DODGE,
    UNIT_COMBAT_DIE_PARRY,
    UNIT_COMBAT_DIE_DEFLECT,
    UNIT_COMBAT_DIE_BLOCK,
    UNIT_COMBAT_DIE_GLANCE,
    UNIT_COMBAT_DIE_CRIT,
    UNIT_COMBAT_DIE_CRUSH,
    UNIT_COMBAT_DIE_HIT,
};

#define NUM_UNIT_COMBAT_DIE_SIDES (UNIT_COMBAT_DIE_HIT + 1)

// A little helper func for a nice attack table debug output
inline const char* UnitCombatDieSideText(UnitCombatDieSide side)
{
    switch (side)
    {
        case UNIT_COMBAT_DIE_MISS:    return "MISS";
        case UNIT_COMBAT_DIE_RESIST:  return "RESIST";
        case UNIT_COMBAT_DIE_DODGE:   return "DODGE";
        case UNIT_COMBAT_DIE_PARRY:   return "PARRY";
        case UNIT_COMBAT_DIE_DEFLECT: return "DEFLECT";
        case UNIT_COMBAT_DIE_BLOCK:   return "BLOCK";
        case UNIT_COMBAT_DIE_GLANCE:  return "GLANCE";
        case UNIT_COMBAT_DIE_CRIT:    return "CRIT";
        case UNIT_COMBAT_DIE_CRUSH:   return "CRUSH";
        case UNIT_COMBAT_DIE_HIT:     return "HIT";
    }
    return "INVALID";
}

struct CleanDamage
{
    CleanDamage(uint32 _damage, WeaponAttackType _attackType, MeleeHitOutcome _hitOutCome) :
        damage(_damage), attackType(_attackType), hitOutCome(_hitOutCome) {}

    uint32 damage;
    WeaponAttackType attackType;
    MeleeHitOutcome hitOutCome;
};

// Struct for use in Unit::CalculateMeleeDamage
// Need create structure like in SMSG_ATTACKERSTATEUPDATE opcode
struct CalcDamageInfo
{
    Unit*  attacker;             // Attacker
    Unit*  target;               // Target for damage
    SpellSchoolMask damageSchoolMask;
    uint32 damage;
    uint32 absorb;
    uint32 resist;
    uint32 blocked_amount;
    uint32 HitInfo;
    uint32 TargetState;
    // Helper
    WeaponAttackType attackType; //
    uint32 procAttacker;
    uint32 procVictim;
    uint32 procEx;
    uint32 cleanDamage;          // Used only for rage calculation
    MeleeHitOutcome hitOutCome;  // TODO: remove this field (need use TargetState)
};

// Spell damage info structure based on structure sending in SMSG_SPELLNONMELEEDAMAGELOG opcode
struct SpellNonMeleeDamage
{
    SpellNonMeleeDamage(Unit* _attacker, Unit* _target, uint32 _SpellID, SpellSchoolMask _schoolMask)
        : target(_target), attacker(_attacker), SpellID(_SpellID), damage(0), schoolMask(_schoolMask),
          absorb(0), resist(0), physicalLog(false), unused(false), blocked(0), HitInfo(0)
    {}

    Unit*   target;
    Unit*   attacker;
    uint32 SpellID;
    uint32 damage;
    SpellSchoolMask schoolMask;
    uint32 absorb;
    uint32 resist;
    bool   physicalLog;
    bool   unused;
    uint32 blocked;
    uint32 HitInfo;
};

struct SpellPeriodicAuraLogInfo
{
    SpellPeriodicAuraLogInfo(Aura* _aura, uint32 _damage, uint32 _overDamage, uint32 _absorb, uint32 _resist, float _multiplier, bool _critical = false)
        : aura(_aura), damage(_damage), overDamage(_overDamage), absorb(_absorb), resist(_resist), multiplier(_multiplier), critical(_critical) {}

    Aura*   aura;
    uint32 damage;
    uint32 overDamage;                                      // overkill/overheal
    uint32 absorb;
    uint32 resist;
    float  multiplier;
    bool   critical;
};

uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition);

struct CombatData
{
    public:
        CombatData(Unit* owner) : threatManager(ThreatManager(owner)), hostileRefManager(HostileRefManager(owner)) {};

        // Manage all Units threatening us
        ThreatManager threatManager;
        // Manage all Units that are threatened by us
        HostileRefManager hostileRefManager;
};

enum SpellAuraProcResult
{
    SPELL_AURA_PROC_OK              = 0,                    // proc was processed, will remove charges
    SPELL_AURA_PROC_FAILED          = 1,                    // proc failed - if at least one aura failed the proc, charges won't be taken
    SPELL_AURA_PROC_CANT_TRIGGER    = 2                     // aura can't trigger - skip charges taking, move to next aura if exists
};

typedef SpellAuraProcResult(Unit::*pAuraProcHandler)(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
extern pAuraProcHandler AuraProcHandler[TOTAL_AURAS];

#define MAX_DECLINED_NAME_CASES 5

struct DeclinedName
{
    std::string name[MAX_DECLINED_NAME_CASES];
};

enum CurrentSpellTypes
{
    CURRENT_MELEE_SPELL             = 0,
    CURRENT_GENERIC_SPELL           = 1,
    CURRENT_AUTOREPEAT_SPELL        = 2,
    CURRENT_CHANNELED_SPELL         = 3
};

#define CURRENT_FIRST_NON_MELEE_SPELL 1
#define CURRENT_MAX_SPELL             4

enum ActiveStates
{
    ACT_PASSIVE  = 0x01,                                    // 0x01 - passive
    ACT_DISABLED = 0x81,                                    // 0x80 - castable
    ACT_ENABLED  = 0xC1,                                    // 0x40 | 0x80 - auto cast + castable
    ACT_COMMAND  = 0x07,                                    // 0x01 | 0x02 | 0x04
    ACT_REACTION = 0x06,                                    // 0x02 | 0x04
    ACT_DECIDE   = 0x00                                     // custom
};

enum CommandStates
{
    COMMAND_STAY    = 0,
    COMMAND_FOLLOW  = 1,
    COMMAND_ATTACK  = 2,
    COMMAND_DISMISS = 3
};

#define UNIT_ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define UNIT_ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAX_UNIT_ACTION_BUTTON_ACTION_VALUE (0x00FFFFFF+1)
#define MAKE_UNIT_ACTION_BUTTON(A,T) (uint32(A) | (uint32(T) << 24))

struct UnitActionBarEntry
{
    UnitActionBarEntry() : packedData(uint32(ACT_DISABLED) << 24) {}

    uint32 packedData;

    // helper
    ActiveStates GetType() const { return ActiveStates(UNIT_ACTION_BUTTON_TYPE(packedData)); }
    uint32 GetAction() const { return UNIT_ACTION_BUTTON_ACTION(packedData); }
    bool IsActionBarForSpell() const
    {
        ActiveStates Type = GetType();
        return Type == ACT_DISABLED || Type == ACT_ENABLED || Type == ACT_PASSIVE;
    }

    void SetActionAndType(uint32 action, ActiveStates type)
    {
        packedData = MAKE_UNIT_ACTION_BUTTON(action, type);
    }

    void SetType(ActiveStates type)
    {
        packedData = MAKE_UNIT_ACTION_BUTTON(UNIT_ACTION_BUTTON_ACTION(packedData), type);
    }

    void SetAction(uint32 action)
    {
        packedData = (packedData & 0xFF000000) | UNIT_ACTION_BUTTON_ACTION(action);
    }
};

typedef UnitActionBarEntry CharmSpellEntry;

enum ActionBarIndex
{
    ACTION_BAR_INDEX_START = 0,
    ACTION_BAR_INDEX_PET_SPELL_START = 3,
    ACTION_BAR_INDEX_PET_SPELL_END = 7,
    ACTION_BAR_INDEX_END = 10,
};

#define MAX_UNIT_ACTION_BAR_INDEX (ACTION_BAR_INDEX_END-ACTION_BAR_INDEX_START)

struct CharmInfo
{
    public:
        explicit CharmInfo(Unit* unit);
        ~CharmInfo();

        void SetCharmState(std::string const& ainame = "PetAI", bool withNewThreatList = true);
        void ResetCharmState();
        uint32 GetPetNumber() const { return m_petnumber; }
        void SetPetNumber(uint32 petnumber, bool statwindow);

        void SetCommandState(CommandStates st);
        CommandStates GetCommandState() const { return m_CommandState; }
        bool HasCommandState(CommandStates state) const { return (m_CommandState == state); }

        void InitVehicleCreateSpells();
        void InitPossessCreateSpells();
        void InitCharmCreateSpells();
        void InitPetActionBar();
        void InitEmptyActionBar();

        // return true if successful
        bool AddSpellToActionBar(uint32 spellid, ActiveStates newstate = ACT_DECIDE, uint8 forceSlot = 255);
        bool RemoveSpellFromActionBar(uint32 spell_id);
        void LoadPetActionBar(const std::string& data);
        void BuildActionBar(WorldPacket& data) const;
        void SetSpellAutocast(uint32 spell_id, bool state);
        void SetActionBar(uint8 index, uint32 spellOrAction, ActiveStates type)
        {
            PetActionBar[index].SetActionAndType(spellOrAction, type);
        }
        UnitActionBarEntry const* GetActionBarEntry(uint8 index) const { return &(PetActionBar[index]); }

        void ToggleCreatureAutocast(uint32 spellid, bool apply);

        CharmSpellEntry* GetCharmSpell(uint8 index) { return &(m_charmspells[index]); }

        void SetIsRetreating(bool retreating = false) { m_retreating = retreating; }
        bool GetIsRetreating() { return m_retreating; }

        void SetStayPosition(bool stay = false);
        bool IsStayPosSet() { return m_stayPosSet; }

        float GetStayPosX() { return m_stayPosX; }
        float GetStayPosY() { return m_stayPosY; }
        float GetStayPosZ() { return m_stayPosZ; }
        float GetStayPosO() { return m_stayPosO; }

        uint32 GetSpellOpener() { return m_opener; }
        uint32 GetSpellOpenerMinRange() { return m_openerMinRange; }
        uint32 GetSpellOpenerMaxRange() { return m_openerMaxRange; }

        void SetSpellOpener(uint32 spellId = 0, uint32 minRange = 0, uint32 maxRange = 0)
        {
            m_opener = spellId;
            m_openerMinRange = minRange;
            m_openerMaxRange = maxRange;
        }

        CreatureAI* GetAI() { return m_ai; }
        CombatData* GetCombatData() { return m_combatData; };

    private:
        Unit*               m_unit;
        CreatureAI*         m_ai;
        CombatData*         m_combatData;
        UnitActionBarEntry  PetActionBar[MAX_UNIT_ACTION_BAR_INDEX];
        CharmSpellEntry     m_charmspells[CREATURE_MAX_SPELLS];
        CommandStates       m_CommandState;
        uint32              m_petnumber;
        uint32              m_opener;
        uint32              m_openerMinRange;
        uint32              m_openerMaxRange;
        uint32              m_unitFieldFlags;
        uint8               m_unitFieldBytes2_1;
        bool                m_retreating;
        bool                m_stayPosSet;
        float               m_stayPosX;
        float               m_stayPosY;
        float               m_stayPosZ;
        float               m_stayPosO;
};

// used in CallForAllControlledUnits/CheckAllControlledUnits
enum ControlledUnitMask
{
    CONTROLLED_PET       = 0x01,
    CONTROLLED_MINIPET   = 0x02,
    CONTROLLED_GUARDIANS = 0x04,                            // including PROTECTOR_PET
    CONTROLLED_CHARM     = 0x08,
    CONTROLLED_TOTEMS    = 0x10,
};

// for clearing special attacks
#define REACTIVE_TIMER_START 4000

enum ReactiveType
{
    REACTIVE_DEFENSE      = 0,
    REACTIVE_HUNTER_PARRY = 1,
    REACTIVE_OVERPOWER    = 2
};

#define MAX_REACTIVE 3

// Used as MiscValue for SPELL_AURA_IGNORE_UNIT_STATE
enum IgnoreUnitState
{
    IGNORE_UNIT_TARGET_STATE      = 0,                      // target health, aura states, or combopoints
    IGNORE_UNIT_COMBAT_STATE      = 1,                      // ignore caster in combat state
    IGNORE_UNIT_TARGET_NON_FROZEN = 126,                    // ignore absent of frozen state
};

// delay time next attack to prevent client attack animation problems
#define ATTACK_DISPLAY_DELAY 200
#define MAX_PLAYER_STEALTH_DETECT_RANGE 45.0f               // max distance for detection targets by player
#define MAX_CREATURE_ATTACK_RADIUS 45.0f                    // max distance for creature aggro (use with CONFIG_FLOAT_RATE_CREATURE_AGGRO)

// Regeneration defines
#define REGEN_TIME_FULL     2000                            // For this time difference is computed regen value
#define REGEN_TIME_PRECISE  500                             // Used in Spell::CheckPower for precise regeneration in spell cast time

// Power type values defines
enum PowerDefaults
{
    POWER_RAGE_DEFAULT              = 1000,
    POWER_FOCUS_DEFAULT             = 100,
    POWER_ENERGY_DEFAULT            = 100,
    POWER_HAPPINESS_DEFAULT         = 1000000,
    POWER_RUNE_DEFAULT              = 8,
    POWER_RUNIC_POWER_DEFAULT       = 1000,
};

struct SpellProcEventEntry;                                 // used only privately

class Unit : public WorldObject
{
    public:
        typedef std::set<Unit*> AttackerSet;
        typedef std::multimap<uint32 /*spellId*/, SpellAuraHolder*> SpellAuraHolderMap;
        typedef std::pair<SpellAuraHolderMap::iterator, SpellAuraHolderMap::iterator> SpellAuraHolderBounds;
        typedef std::pair<SpellAuraHolderMap::const_iterator, SpellAuraHolderMap::const_iterator> SpellAuraHolderConstBounds;
        typedef std::list<SpellAuraHolder*> SpellAuraHolderList;
        typedef std::list<Aura*> AuraList;
        typedef std::list<DiminishingReturn> Diminishing;
        typedef std::set<uint32 /*playerGuidLow*/> ComboPointHolderSet;
        typedef std::map<uint8 /*slot*/, uint32 /*spellId*/> VisibleAuraMap;
        typedef std::map<SpellEntry const*, ObjectGuid /*targetGuid*/> TrackedAuraTargetMap;

        virtual ~Unit();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void CleanupsBeforeDelete() override;               // used in ~Creature/~Player (or before mass creature delete to remove cross-references to already deleted units)

        float GetObjectBoundingRadius() const override { return m_floatValues[UNIT_FIELD_BOUNDINGRADIUS]; } // overwrite WorldObject version
        float GetCombatReach() const override { return m_floatValues[UNIT_FIELD_COMBATREACH]; } // overwrite WorldObject version

        /**
         * Gets the current DiminishingLevels for the given group
         * @param group The group that you would like to know the current diminishing return level for
         * @return The current diminishing level, up to DIMINISHING_LEVEL_IMMUNE
         */
        DiminishingLevels GetDiminishing(DiminishingGroup  group);
        /**
         * Increases the level of the DiminishingGroup by one level up until
         * DIMINISHING_LEVEL_IMMUNE where the target becomes immune to spells of
         * that DiminishingGroup
         * @param group The group to increase the level for by one
         */
        void IncrDiminishing(DiminishingGroup group);
        /**
         * Calculates how long the duration of a spell should be considering
         * diminishing returns, ie, if the Level passed in is DIMINISHING_LEVEL_IMMUNE
         * then the duration will be zeroed out. If it is DIMINISHING_LEVEL_1 then a full
         * duration will be used
         * @param group The group to affect
         * @param duration The duration to be changed, will be updated with the new duration
         * @param caster Who's casting the spell, used to decide whether anything should be calculated
         * @param Level The current level of diminishing returns for the group, decides the new duration
         * @param limitduration
         * @param isReflected Whether the spell was reflected or not, used to determine if we should do any calculations at all.
         */
        void ApplyDiminishingToDuration(DiminishingGroup  group, int32& duration, Unit* caster, DiminishingLevels Level, int32 limitduration, bool isReflected);
        /**
         * Applies a diminishing return to the given group if apply is true,
         * otherwise lowers the level by one (?)
         * @param group The group to affect
         * @param apply whether this aura is being added/removed
         */
        void ApplyDiminishingAura(DiminishingGroup  group, bool apply);
        /**
         * Clears all the current diminishing returns for this Unit.
         */
        void ClearDiminishings() { m_Diminishing.clear(); }

        void Update(uint32 update_diff, uint32 time) override;

        /**
         * Updates the attack time for the given WeaponAttackType
         * @param type The type of weapon that we want to update the time for
         * @param time the remaining time until we can attack with the WeaponAttackType again
         */
        void setAttackTimer(WeaponAttackType type, uint32 time) { m_attackTimer[type] = time; }
        /**
         * Resets the attack timer to the base value decided by Unit::m_modAttackSpeedPct and
         * Unit::GetAttackTime
         * @param type The weapon attack type to reset the attack timer for.
         */
        void resetAttackTimer(WeaponAttackType type = BASE_ATTACK);
        /**
         * Get's the remaining time until we can do an attack
         * @param type The weapon type to check the remaining time for
         * @return The remaining time until we can attack with this weapon type.
         */
        uint32 getAttackTimer(WeaponAttackType type) const { return m_attackTimer[type]; }
        /**
         * Checks whether the unit can do an attack. Does this by checking the attacktimer for the
         * WeaponAttackType, can probably be thought of as a cooldown for each swing/shot
         * @param type What weapon should we check for
         * @return true if the Unit::m_attackTimer is zero for the given WeaponAttackType
         */
        bool isAttackReady(WeaponAttackType type = BASE_ATTACK) const { return m_attackTimer[type] == 0; }
        /**
         * Checks if the current Unit has an offhand weapon
         * @return True if there is a offhand weapon.
         */
        bool haveOffhandWeapon() const;
        /**
         * Does an attack if any of the timers allow it and resets them, if the user
         * isn't in range or behind the target an error is sent to the client.
         * Also makes sure to not make and offhand and mainhand attack at the same
         * time. Only handles non-spells ie melee attacks.
         * @return True if an attack was made and no error happened, false otherwise
         */
        bool UpdateMeleeAttackingState();
        /**
         * Check is a given equipped weapon can be used, ie the mainhand, offhand etc.
         * @param attackType The attack type to check, ie: main/offhand/ranged
         * @return True if the weapon can be used, true except for shapeshifts and if disarmed.
         */
        bool CanUseEquippedWeapon(WeaponAttackType attackType) const
        {
            if (IsInFeralForm())
                return false;

            switch (attackType)
            {
                default:
                case BASE_ATTACK:
                    return !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
                case OFF_ATTACK:
                    return !HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND);
                case RANGED_ATTACK:
                    return !HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED);
            }
        }
        /** Returns if the Unit can reach a victim with Melee Attack
         *
         * @param pVictim Who we want to reach with a melee attack
         * @param flat_mod The same as sent to Unit::GetCombatReach
         * @return true if we can reach pVictim with a melee attack
         */
        bool CanReachWithMeleeAttack(Unit const* pVictim, float flat_mod = 0.0f) const;
        uint32 m_extraAttacks;
        void DoExtraAttacks(Unit* pVictim);

        bool _addAttacker(Unit* pAttacker)                  //< (Internal Use) must be called only from Unit::Attack(Unit*)
        {
            AttackerSet::const_iterator itr = m_attackers.find(pAttacker);
            if (itr == m_attackers.end())
            {
                m_attackers.insert(pAttacker);
                return true;
            }
            else
                return false;
        }
        void _removeAttacker(Unit* pAttacker)               //< (Internal Use) must be called only from Unit::AttackStop()
        {
            m_attackers.erase(pAttacker);
        }
        Unit* getAttackerForHelper()                        //< Return a possible enemy from this unit to help in combat
        {
            if (getVictim() != nullptr)
                return getVictim();

            if (!m_attackers.empty())
                return *(m_attackers.begin());

            return nullptr;
        }
        /**
         * Tries to attack a Unit/Player, also makes sure to stop attacking the current target
         * if we're already attacking someone.
         * @param victim The Unit to attack
         * @param meleeAttack Whether we should attack with melee or ranged/magic
         * @return True if an attack was initiated, false otherwise
         */
        bool Attack(Unit* victim, bool meleeAttack);
        /**
         * Called when we are attacked by someone in someway, might be when a fear runs out and
         * we want to notify AI to attack again or when a spell hits.
         * @param attacker Who's attacking us
         */
        void AttackedBy(Unit* attacker);
        /**
         * Stop all spells from casting except the one give by except_spellid
         * @param except_spellid This spell id will not be stopped from casting, defaults to 0
         * \see Unit::InterruptSpell
         */
        void CastStop(uint32 except_spellid = 0);
        /**
         * Stops attacking whatever we are attacking at the moment and tells the Unit we are attacking
         * that we are not doing that anymore.
         * @param targetSwitch if we are switching targets or not, defaults to false
         * @return false if we weren't attacking already, true otherwise
         * \see Unit::m_attacking
         */
        virtual bool AttackStop(bool targetSwitch = false, bool includingCast = false, bool includingCombo = false);
        /**
         * Removes all attackers from the Unit::m_attackers set and logs it if someone that
         * wasn't attacking it was in the list. Does this check by checking if Unit::AttackStop()
         * returned false.
         * \see Unit::AttackStop
         */
        void RemoveAllAttackers();

        void MeleeAttackStart(Unit* victim);
        void MeleeAttackStop(Unit* victim);

        /// Returns the Unit::m_attackers, that stores the units that are attacking you
        AttackerSet const& getAttackers() const { return m_attackers; }

        Unit* getVictim() const { return m_attacking; }     //< Returns the victim that this unit is currently attacking
        void CombatStop(bool includingCast = false, bool includingCombo = true);        //< Stop this unit from combat, if includingCast==true, also interrupt casting
        void CombatStopWithPets(bool includingCast = false);
        void StopAttackFaction(uint32 faction_id);
        Unit* SelectRandomUnfriendlyTarget(Unit* except = nullptr, float radius = ATTACK_DISTANCE) const;
        Unit* SelectRandomFriendlyTarget(Unit* except = nullptr, float radius = ATTACK_DISTANCE) const;
        bool hasNegativeAuraWithInterruptFlag(uint32 flag) const;
        void SendMeleeAttackStop(Unit* victim) const;
        void SendMeleeAttackStart(Unit* pVictim) const;

        void addUnitState(uint32 f) { m_state |= f; }
        bool hasUnitState(uint32 f) const { return !!(m_state & f); }
        void clearUnitState(uint32 f) { m_state &= ~f; }
        bool CanFreeMove() const { return !hasUnitState(UNIT_STAT_NO_FREE_MOVE) && !GetOwnerGuid(); }

        virtual uint32 GetLevelForTarget(Unit const* /*target*/) const { return getLevel(); }
        bool IsTrivialForTarget(Unit const* pov) const;

        void SetLevel(uint32 lvl);

        uint32 getLevel() const { return GetUInt32Value(UNIT_FIELD_LEVEL); }
        virtual uint8 getRace() const { return GetByteValue(UNIT_FIELD_BYTES_0, 0); }
        uint32 getRaceMask() const { return getRace() ? 1 << (getRace() - 1) : 0; }
        uint8 getClass() const { return GetByteValue(UNIT_FIELD_BYTES_0, 1); }
        uint32 getClassMask() const { return 1 << (getClass() - 1); }
        uint8 getGender() const { return GetByteValue(UNIT_FIELD_BYTES_0, 2); }

        float GetStat(Stats stat) const { return float(GetUInt32Value(UNIT_FIELD_STAT0 + stat)); }
        void SetStat(Stats stat, int32 val) { SetStatInt32Value(UNIT_FIELD_STAT0 + stat, val); }
        uint32 GetArmor() const { return GetResistance(SPELL_SCHOOL_NORMAL) ; }
        void SetArmor(int32 val) { SetResistance(SPELL_SCHOOL_NORMAL, val); }

        uint32 GetResistance(SpellSchools school) const { return GetUInt32Value(UNIT_FIELD_RESISTANCES + school); }
        void SetResistance(SpellSchools school, int32 val) { SetStatInt32Value(UNIT_FIELD_RESISTANCES + school, val); }

        uint32 GetHealth()    const { return GetUInt32Value(UNIT_FIELD_HEALTH); }
        uint32 GetMaxHealth() const { return GetUInt32Value(UNIT_FIELD_MAXHEALTH); }
        float GetHealthPercent() const { return (GetHealth() * 100.0f) / GetMaxHealth(); }
        void SetHealth(uint32 val);
        void SetMaxHealth(uint32 val);
        void SetHealthPercent(float percent);
        int32 ModifyHealth(int32 val);

        Powers GetPowerType() const { return Powers(GetByteValue(UNIT_FIELD_BYTES_0, 3)); }
        void SetPowerType(Powers power);
        uint32 GetPower(Powers power) const { return GetUInt32Value(UNIT_FIELD_POWER1 + power); }
        uint32 GetMaxPower(Powers power) const { return GetUInt32Value(UNIT_FIELD_MAXPOWER1 + power); }
        void SetPower(Powers power, uint32 val);
        void SetMaxPower(Powers power, uint32 val);
        int32 ModifyPower(Powers power, int32 val);
        void ApplyPowerMod(Powers power, uint32 val, bool apply);
        void ApplyMaxPowerMod(Powers power, uint32 val, bool apply);

        uint32 GetAttackTime(WeaponAttackType att) const { return (uint32)(GetFloatValue(UNIT_FIELD_BASEATTACKTIME + att) / m_modAttackSpeedPct[att]); }
        void SetAttackTime(WeaponAttackType att, uint32 val) { SetFloatValue(UNIT_FIELD_BASEATTACKTIME + att, val * m_modAttackSpeedPct[att]); }
        void ApplyAttackTimePercentMod(WeaponAttackType att, float val, bool apply);
        void ApplyCastTimePercentMod(float val, bool apply);

        SheathState GetSheath() const { return SheathState(GetByteValue(UNIT_FIELD_BYTES_2, 0)); }
        virtual void SetSheath(SheathState sheathed) { SetByteValue(UNIT_FIELD_BYTES_2, 0, sheathed); }

        // faction template id
        uint32 getFaction() const { return GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE); }
        void setFaction(uint32 faction) { SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, faction); }
        FactionTemplateEntry const* getFactionTemplateEntry() const;
        void RestoreOriginalFaction();
        bool IsHostileTo(Unit const* unit) const override;
        bool IsHostileToPlayers() const;
        bool IsFriendlyTo(Unit const* unit) const override;
        bool IsNeutralToAll() const;
        bool IsContestedGuard() const
        {
            if (FactionTemplateEntry const* entry = getFactionTemplateEntry())
                return entry->IsContestedGuardFaction();

            return false;
        }

        ReputationRank GetReactionTo(Unit const* unit) const override;
        ReputationRank GetReactionTo(Corpse const* corpse) const override;

        bool IsEnemy(Unit const* unit) const override;
        bool IsFriend(Unit const* unit) const override;

        bool CanAssist(Unit const* unit, bool ignoreFlags = false) const;
        bool CanAssist(Corpse const* corpse) const;

        bool CanAttack(Unit const* unit) const;
        bool CanAttackNow(Unit const* unit) const;

        bool CanCooperate(Unit const* unit) const;

        bool CanInteract(GameObject const* object) const;
        bool CanInteract(Unit const* unit) const;
        bool CanInteractNow(Unit const* unit) const;

        bool IsCivilianForTarget(Unit const* pov) const;

        bool IsInGroup(Unit const* other, bool party = false, bool UI = false) const;
        bool IsInParty(Unit const* other, bool UI = false) const { return IsInGroup(other, true, UI); }

        // extensions of CanAttack and CanAssist API needed serverside
        virtual bool CanAttackSpell(Unit const* target, SpellEntry const* spellInfo = nullptr, bool isAOE = false) const override;
        virtual bool CanAssistSpell(Unit const* target, SpellEntry const* spellInfo = nullptr) const override;

        virtual bool CanAttackOnSight(Unit const* target); // Used in MoveInLineOfSight checks

        // Serverside fog of war settings
        bool IsFogOfWarVisibleStealth(Unit const* other) const;
        bool IsFogOfWarVisibleHealth(Unit const* other) const;
        bool IsFogOfWarVisibleStats(Unit const* other) const;

        bool IsImmuneToNPC() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC); }
        void SetImmuneToNPC(bool state);
        bool IsImmuneToPlayer() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER); }
        void SetImmuneToPlayer(bool state);

        bool IsPvP() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP); }
        void SetPvP(bool state);
        bool IsPvPFreeForAll() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP); }
        void SetPvPFreeForAll(bool state);
        bool IsPvPContested() const;
        void SetPvPContested(bool state);
        bool IsPvPSanctuary() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_SANCTUARY); }
        void SetPvPSanctuary(bool state);
        uint32 GetCreatureType() const;
        uint32 GetCreatureTypeMask() const
        {
            uint32 creatureType = GetCreatureType();
            return (creatureType >= 1) ? (1 << (creatureType - 1)) : 0;
        }

        uint8 getStandState() const { return GetByteValue(UNIT_FIELD_BYTES_1, 0); }
        bool IsSitState() const;
        bool IsStandState() const;
        bool IsSeatedState() const;
        void SetStandState(uint8 state);

        void  SetStandFlags(uint8 flags) { SetByteFlag(UNIT_FIELD_BYTES_1, 2, flags); }
        void  RemoveStandFlags(uint8 flags) { RemoveByteFlag(UNIT_FIELD_BYTES_1, 2, flags); }

        bool IsMounted() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT); }
        uint32 GetMountID() const { return GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID); }
        void Mount(uint32 mount, uint32 spellId = 0);
        void Unmount(bool from_aura = false);

        VehicleInfo* GetVehicleInfo() const { return m_vehicleInfo; }
        bool IsVehicle() const { return m_vehicleInfo != nullptr; }
        void SetVehicleId(uint32 entry, uint32 overwriteNpcEntry);
        Unit const* FindRootVehicle(const Unit* whichVehicle = nullptr) const;

        uint16 GetMaxSkillValueForLevel(Unit const* target = nullptr) const { return (target ? GetLevelForTarget(target) : getLevel()) * 5; }
        void DealDamageMods(Unit* pVictim, uint32& damage, uint32* absorb, DamageEffectType damagetype, SpellEntry const* spellProto = nullptr);
        uint32 DealDamage(Unit* pVictim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellEntry const* spellProto, bool durabilityLoss);
        int32 DealHeal(Unit* pVictim, uint32 addhealth, SpellEntry const* spellProto, bool critical = false, uint32 absorb = 0);
        void InterruptOrDelaySpell(Unit* pVictim, DamageEffectType damagetype);

        void PetOwnerKilledUnit(Unit* pVictim);

        void ProcDamageAndSpell(Unit* pVictim, uint32 procAttacker, uint32 procVictim, uint32 procEx, uint32 amount, WeaponAttackType attType = BASE_ATTACK, SpellEntry const* procSpell = nullptr, bool dontTriggerSpecial = true);
        void ProcDamageAndSpellFor(bool isVictim, Unit* pTarget, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellEntry const* procSpell, uint32 damage, bool dontTriggerSpecial);

        void HandleEmote(uint32 emote_id);                  // auto-select command/state
        void HandleEmoteCommand(uint32 emote_id);
        void HandleEmoteState(uint32 emote_id);
        void AttackerStateUpdate(Unit* pVictim, WeaponAttackType attType = BASE_ATTACK, bool extra = false);

        void CalculateMeleeDamage(Unit* pVictim, CalcDamageInfo* damageInfo, WeaponAttackType attackType = BASE_ATTACK);
        void DealMeleeDamage(CalcDamageInfo* damageInfo, bool durabilityLoss);

        bool IsAllowedDamageInArea(Unit* pVictim) const;

        void CalculateSpellDamage(SpellNonMeleeDamage* damageInfo, int32 damage, SpellEntry const* spellInfo, WeaponAttackType attackType = BASE_ATTACK);
        void DealSpellDamage(SpellNonMeleeDamage* damageInfo, bool durabilityLoss);

        // player or player's pet resilience (-1%), cap 100%
        uint32 GetMeleeDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_MELEE, 2.0f, 100.0f, damage); }
        uint32 GetRangedDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_MELEE, 2.0f, 100.0f, damage); }
        uint32 GetSpellDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_MELEE, 2.0f, 100.0f, damage); }

        uint32 GetManaDrainReduction(uint32 amount) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_SPELL, 2.2f, 100.0f, amount); /*NOTE: Verify if cases with melee/ranged ratings exist*/ }

        SpellMissInfo MeleeSpellHitResult(Unit* pVictim, SpellEntry const* spell);
        SpellMissInfo MagicSpellHitResult(Unit* pVictim, SpellEntry const* spell, SpellSchoolMask schoolMask);
        SpellMissInfo SpellHitResult(Unit* pVictim, SpellEntry const* spell, bool reflectable = false);

        // Unit Combat reactions API: Dodge/Parry/Block
        bool CanDodge() const { return m_canDodge; }
        bool CanParry() const { return m_canParry; }
        bool CanBlock() const { return m_canBlock; }
        // Unit Melee events API: Crush/Glance/Daze
        bool CanCrush() const;
        bool CanGlance() const;
        bool CanDaze() const;

        void SetCanDodge(const bool flag);
        void SetCanParry(const bool flag);
        void SetCanBlock(const bool flag);

        bool CanReactInCombat() const { return (isAlive() && !IsIncapacitated()); }
        bool CanDodgeInCombat() const;
        bool CanDodgeInCombat(const Unit* attacker) const;
        bool CanParryInCombat() const;
        bool CanParryInCombat(const Unit* attacker) const;
        bool CanBlockInCombat(SpellSchoolMask weaponSchoolMask = SPELL_SCHOOL_MASK_NORMAL) const;
        bool CanBlockInCombat(const Unit* attacker, SpellSchoolMask weaponSchoolMask = SPELL_SCHOOL_MASK_NORMAL) const;
        bool CanCrushInCombat() const;
        bool CanCrushInCombat(const Unit* victim) const;
        bool CanGlanceInCombat() const { return CanGlance(); }
        bool CanGlanceInCombat(const Unit* victim) const;
        bool CanDazeInCombat() const { return CanDaze(); }
        bool CanDazeInCombat(const Unit* victim) const;

        // Unit Combat Reactions API for abilities (melee and ranged spells): Dodge/Parry/Block/Deflect
        bool CanReactOnAbility(const SpellEntry* ability) const;
        bool CanDodgeAbility(const Unit* attacker, const SpellEntry* ability) const;
        bool CanParryAbility(const Unit* attacker, const SpellEntry* ability) const;
        bool CanBlockAbility(const Unit* attacker, const SpellEntry* ability, bool miss = false) const;
        bool CanDeflectAbility(const Unit* attacker, const SpellEntry* ability) const;

        float CalculateEffectiveDodgeChance(const Unit* attacker, WeaponAttackType attType, const SpellEntry* ability = nullptr) const;
        float CalculateEffectiveParryChance(const Unit* attacker, WeaponAttackType attType, const SpellEntry* ability = nullptr) const;
        float CalculateEffectiveBlockChance(const Unit* attacker, WeaponAttackType attType, const SpellEntry* ability = nullptr) const;
        float CalculateEffectiveCrushChance(const Unit* victim, WeaponAttackType attType) const;
        float CalculateEffectiveGlanceChance(const Unit* victim, WeaponAttackType attType) const;
        float CalculateEffectiveDazeChance(const Unit* victim, WeaponAttackType attType) const;
        uint32 CalculateGlanceAmount(CalcDamageInfo* meleeInfo) const;

        float CalculateAbilityDeflectChance(const Unit* attacker, const SpellEntry* ability) const;

        bool RollAbilityPartialBlockOutcome(const Unit* attacker, WeaponAttackType attType, const SpellEntry* ability) const;

        float GetDodgeChance() const;
        float GetParryChance() const;
        float GetBlockChance() const;

        float GetReflectChance(SpellSchoolMask schoolMask) const;

        virtual bool CanCrit(WeaponAttackType attType) const { return (GetCritChance(attType) >= 0.005f); }
        virtual bool CanCrit(SpellSchoolMask schoolMask) const { return (GetCritChance(schoolMask) >= 0.005f); }
        virtual bool CanCrit(const SpellEntry* entry, SpellSchoolMask schoolMask, WeaponAttackType attType) const;

        virtual float GetCritChance(WeaponAttackType attackType) const;
        virtual float GetCritChance(SpellSchoolMask schoolMask) const;
        virtual float GetCritChance(const SpellEntry* entry, SpellSchoolMask schoolMask) const;

        virtual float GetCritTakenChance(SpellSchoolMask dmgSchoolMask, SpellDmgClass dmgClass, bool heal = false) const;

        virtual float GetCritMultiplier(SpellSchoolMask dmgSchoolMask, uint32 creatureTypeMask, const SpellEntry* spell = nullptr, bool heal = false) const;
        virtual float GetCritTakenMultiplier(SpellSchoolMask dmgSchoolMask, SpellDmgClass dmgClass, float ignorePct = 0.0f, bool heal = false) const;
        uint32 CalculateCritAmount(const Unit* victim, uint32 amount, const SpellEntry* entry, bool heal = false) const;
        uint32 CalculateCritAmount(CalcDamageInfo* meleeInfo) const;

        virtual float GetHitChance(WeaponAttackType attackType) const;
        virtual float GetHitChance(SpellSchoolMask schoolMask) const;
        virtual float GetHitChance(const SpellEntry* entry, SpellSchoolMask schoolMask) const;

        virtual float GetMissChance(WeaponAttackType attackType) const;
        virtual float GetMissChance(SpellSchoolMask schoolMask) const;
        virtual float GetMissChance(const SpellEntry* entry, SpellSchoolMask schoolMask) const;

        float CalculateEffectiveCritChance(const Unit* victim, WeaponAttackType attType, const SpellEntry* ability = nullptr) const;
        float CalculateEffectiveMissChance(const Unit* victim, WeaponAttackType attType, const SpellEntry* ability = nullptr) const;

        float CalculateSpellCritChance(const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell) const;
        float CalculateSpellMissChance(const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell) const;

        bool RollSpellCritOutcome(const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell) const;

        float GetExpertisePercent(WeaponAttackType attType) const;

        virtual int32 GetResistancePenetration(SpellSchools school) const;

        float CalculateEffectiveMagicResistancePercent(const Unit* attacker, SpellSchoolMask schoolMask, bool binary = false) const;
        float RollMagicPartialResistRatioAgainst(const Unit* victim, SpellSchoolMask schoolMask, DamageEffectType dmgType, bool binary = false) const;

        float CalculateSpellResistChance(const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell) const;

        virtual uint32 GetShieldBlockValue() const = 0;
        uint32 GetUnitMeleeSkill(Unit const* target = nullptr) const { return (target ? GetLevelForTarget(target) : getLevel()) * 5; }
        uint32 GetDefenseSkillValue(Unit const* target = nullptr) const;
        uint32 GetWeaponSkillValue(WeaponAttackType attType, Unit const* target = nullptr) const;
        float GetWeaponProcChance() const;
        float GetPPMProcChance(uint32 WeaponSpeed, float PPM) const;

        MeleeHitOutcome RollMeleeOutcomeAgainst(const Unit* pVictim, WeaponAttackType attType, SpellSchoolMask schoolMask) const;

        bool isVendor()       const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR); }
        bool isTrainer()      const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER); }
        bool isQuestGiver()   const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER); }
        bool isGossip()       const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); }
        bool isTaxi()         const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_FLIGHTMASTER); }
        bool isGuildMaster()  const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PETITIONER); }
        bool isBattleMaster() const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BATTLEMASTER); }
        bool isBanker()       const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BANKER); }
        bool isInnkeeper()    const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_INNKEEPER); }
        bool isSpiritHealer() const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER); }
        bool isSpiritGuide()  const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITGUIDE); }
        bool isTabardDesigner()const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TABARDDESIGNER); }
        bool isAuctioner()    const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_AUCTIONEER); }
        bool isArmorer()      const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_REPAIR); }
        bool isServiceProvider() const
        {
            return HasFlag(UNIT_NPC_FLAGS,
                           UNIT_NPC_FLAG_VENDOR | UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_FLIGHTMASTER |
                           UNIT_NPC_FLAG_PETITIONER | UNIT_NPC_FLAG_BATTLEMASTER | UNIT_NPC_FLAG_BANKER |
                           UNIT_NPC_FLAG_INNKEEPER | UNIT_NPC_FLAG_SPIRITHEALER |
                           UNIT_NPC_FLAG_SPIRITGUIDE | UNIT_NPC_FLAG_TABARDDESIGNER | UNIT_NPC_FLAG_AUCTIONEER);
        }
        bool isSpiritService() const { return HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER | UNIT_NPC_FLAG_SPIRITGUIDE); }

        bool IsTaxiFlying()  const { return hasUnitState(UNIT_STAT_TAXI_FLIGHT); }

        bool isInCombat()  const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT); }
        void SetInCombatState(bool PvP, Unit* enemy = nullptr);
        void SetInCombatWith(Unit* enemy);
        void SetInCombatWithAggressor(Unit* aggressor);
        void SetInCombatWithAssisted(Unit* assisted);
        void SetInCombatWithVictim(Unit* victim);
        void ClearInCombat();
        uint32 GetCombatTimer() const { return m_CombatTimer; }

        SpellAuraHolderBounds GetSpellAuraHolderBounds(uint32 spell_id)
        {
            return m_spellAuraHolders.equal_range(spell_id);
        }
        SpellAuraHolderConstBounds GetSpellAuraHolderBounds(uint32 spell_id) const
        {
            return m_spellAuraHolders.equal_range(spell_id);
        }

        uint32 GetAuraCount(uint32 spellId) const;

        bool HasAuraType(AuraType auraType) const;
        bool HasAffectedAura(AuraType auraType, SpellEntry const* spellProto) const;
        bool HasAura(uint32 spellId, SpellEffectIndex effIndex) const;
        bool HasAura(uint32 spellId) const
        {
            return m_spellAuraHolders.find(spellId) != m_spellAuraHolders.end();
        }
        bool HasAuraOfDifficulty(uint32 spellId) const;

        virtual bool HasSpell(uint32 /*spellID*/) const { return false; }

        bool HasStealthAura()      const { return HasAuraType(SPELL_AURA_MOD_STEALTH); }
        bool HasInvisibilityAura() const { return HasAuraType(SPELL_AURA_MOD_INVISIBILITY); }
        bool isFeared()  const { return HasAuraType(SPELL_AURA_MOD_FEAR); }
        bool isInRoots() const { return HasAuraType(SPELL_AURA_MOD_ROOT); }
        bool IsPolymorphed() const;

        bool isFrozen() const;
        bool IsIgnoreUnitState(SpellEntry const* spell, IgnoreUnitState ignoreState) const;

        virtual bool IsInWater() const;
        virtual bool IsUnderWater() const;
        bool isInAccessablePlaceFor(Unit const* unit) const;

        void SendHealSpellLog(Unit* pVictim, uint32 SpellID, uint32 Damage, uint32 OverHeal, bool critical = false, uint32 absorb = 0) const;
        void SendEnergizeSpellLog(Unit* pVictim, uint32 SpellID, uint32 Damage, Powers powertype) const;
        void EnergizeBySpell(Unit* pVictim, uint32 SpellID, uint32 Damage, Powers powertype);
        uint32 SpellNonMeleeDamageLog(Unit* pVictim, uint32 spellID, uint32 damage);

        SpellCastResult CastSpell(Unit* Victim, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(Unit* Victim, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastCustomSpell(Unit* Victim, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastCustomSpell(Unit* Victim, SpellEntry const* spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(float x, float y, float z, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(float x, float y, float z, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);

        // Single flag overload uint32
        SpellCastResult CastSpell(Unit* Victim, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(Victim, spellId, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastCustomSpell(Unit* Victim, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastCustomSpell(Victim, spellId, bp0, bp1, bp2, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(float x, float y, float z, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(x, y, z, spellId, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }

        // Single flag overload SpellEntry
        SpellCastResult CastSpell(Unit* Victim, SpellEntry const* spellInfo, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(Victim, spellInfo, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastCustomSpell(Unit* Victim, SpellEntry const* spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastCustomSpell(Victim, spellInfo, bp0, bp1, bp2, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(float x, float y, float z, SpellEntry const* spellInfo, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(x, y, z, spellInfo, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }

        void DeMorph();

        void SendAttackStateUpdate(CalcDamageInfo* damageInfo) const;
        void SendAttackStateUpdate(uint32 HitInfo, Unit* target, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, uint32 Resist, VictimState TargetState, uint32 BlockedAmount);
        void SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log) const;
        void SendSpellNonMeleeDamageLog(Unit* target, uint32 SpellID, uint32 Damage, SpellSchoolMask damageSchoolMask, uint32 AbsorbedDamage, uint32 Resist, bool PhysicalDamage, uint32 Blocked, bool CriticalHit = false);
        void SendPeriodicAuraLog(SpellPeriodicAuraLogInfo* pInfo) const;
        void SendSpellMiss(Unit* target, uint32 spellID, SpellMissInfo missInfo) const;
        void CasterHitTargetWithSpell(Unit* realCaster, Unit* target, SpellEntry const* spellInfo);

        void NearTeleportTo(float x, float y, float z, float orientation, bool casting = false);
        void MonsterMoveWithSpeed(float x, float y, float z, float speed, bool generatePath = false, bool forceDestination = false);
        // recommend use MonsterMove/MonsterMoveWithSpeed for most case that correctly work with movegens
        // if used additional args in ... part then floats must explicitly casted to double
        void SendHeartBeat();

        bool IsMoving() const { return m_movementInfo.HasMovementFlag(movementFlagsMask); }
        bool IsLevitating() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_LEVITATING); }
        bool IsWalking() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_WALK_MODE); }
        bool IsRooted() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_ROOT); }

        virtual void SetLevitate(bool /*enabled*/) {}
        virtual void SetSwim(bool /*enabled*/) {}
        virtual void SetCanFly(bool /*enabled*/) {}
        virtual void SetFeatherFall(bool /*enabled*/) {}
        virtual void SetHover(bool /*enabled*/) {}
        virtual void SetRoot(bool /*enabled*/) {}
        virtual void SetWaterWalk(bool /*enabled*/) {}

        void SetInFront(Unit const* target);
        void SetFacingTo(float ori);
        void SetFacingToObject(WorldObject* pObject);

        void SendHighestThreatUpdate(HostileReference* pHostileReference);
        void SendThreatClear() const;
        void SendThreatRemove(HostileReference* pHostileReference) const;
        void SendThreatUpdate();

        bool isAlive() const { return (m_deathState == ALIVE); };
        bool isDead() const { return (m_deathState == DEAD || m_deathState == CORPSE); };
        DeathState getDeathState() const { return m_deathState; };
        virtual void SetDeathState(DeathState s);           // overwritten in Creature/Player/Pet

        bool IsTargetUnderControl(Unit const& target) const;

        // Convenience checkers/getters/setters counterparts for some of the protected Unit guid fields
        // See the comments next to protected methods for meanings
        bool HasCharm(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? GetCharmGuid().IsUnit() : (GetCharmGuid() == exactGuid)); }
        bool HasCharmer(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? GetCharmerGuid().IsUnit() : (GetCharmerGuid() == exactGuid)); }
        bool HasTarget(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? GetTargetGuid().IsUnit() : (GetTargetGuid() == exactGuid)); }
        bool HasChannelObject(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? !(GetChannelObjectGuid().IsEmpty()) : (GetChannelObjectGuid() == exactGuid)); }

        Unit* GetCharm(WorldObject const* pov = nullptr) const;
        Unit* GetCharmer(WorldObject const* pov = nullptr) const;
        Unit* GetCreator(WorldObject const* pov = nullptr) const;
        Unit* GetTarget(WorldObject const* pov = nullptr) const;
        Unit* GetChannelObject(WorldObject const* pov = nullptr) const;

        void SetCharm(Unit* charmed) { SetCharmGuid(charmed ? charmed->GetObjectGuid() : ObjectGuid()); }
        void SetCharmer(Unit* charmer) { SetCharmerGuid(charmer ? charmer->GetObjectGuid() : ObjectGuid()); }
        void SetTarget(WorldObject* target) { SetTargetGuid(target ? target->GetObjectGuid() : ObjectGuid()); }
        void SetChannelObject(WorldObject* object) { SetChannelObjectGuid(object ? object->GetObjectGuid() : ObjectGuid()); }

        // Purely logical guid constructs getters/setters
        // Owner: automatically resolves to creator guid and additionally to summoner guid
        ObjectGuid const& GetOwnerGuid() const override { return GetCreatorGuid(); }
        void SetOwnerGuid(ObjectGuid owner) override { SetCreatorGuid(owner); }
        // Pet: automatically resolves to summon guid (permanent pet)
        ObjectGuid const& GetPetGuid() const { return GetSummonGuid(); }
        void SetPetGuid(ObjectGuid pet) { SetSummonGuid(pet); }
        // Selection: by default resolves to target, overriden in Player
        virtual ObjectGuid const& GetSelectionGuid() const { return GetTargetGuid(); }
        virtual void SetSelectionGuid(ObjectGuid guid) { SetTargetGuid(guid); }
        // Master: automatically resolves to charmer or owner guid
        ObjectGuid const& GetMasterGuid() const { ObjectGuid const& guid = GetCharmerGuid(); return (guid ? guid : GetOwnerGuid()); }
        // Spawner: guid of a unit, who is reponsible for starting this unit's parent script (only for script-spawned units within Unit hierarchy)
        virtual ObjectGuid const GetSpawnerGuid() const { return ObjectGuid(); }

        // Convenience unit getters for some of the logical guid constructs above
        Unit* GetOwner(WorldObject const* pov = nullptr, bool recursive = false) const;
        Unit* GetMaster(WorldObject const* pov = nullptr) const;
        Unit* GetSpawner(WorldObject const* pov = nullptr) const;

        // Additional related server-side and client-side ownership-related methods
        // Spell mod owner: static player whose spell mods apply to this unit (server-side)
        virtual Player* GetSpellModOwner() const { return nullptr; }
        // Beneficiary: owner of the xp/loot/etc credit, master or self (server-side)
        Unit* GetBeneficiary() const;
        Player* GetBeneficiaryPlayer() const;
        // Controlling player: official client PoV and term on which player is the "master" of the unit at the moment, limited recursive master/beneficiary logic (client-side)
        Player const* GetControllingPlayer() const;

        // Client controlled: check if unit currently is under client control (has active "mover"), optionally check for specific client (server-side)
        bool IsClientControlled(Player const* exactClient = nullptr) const;
        // Controlling client: server PoV on which client (player) controls movement of the unit at the moment, obtain "mover" (server-side)
        Player const* GetClientControlling() const;

        Pet* GetPet() const;
        void SetPet(Unit* pet) { SetPetGuid(pet ? pet->GetObjectGuid() : ObjectGuid()); }

        Pet* GetMiniPet() const;
        void SetMiniPet(Unit* pet) { SetCritterGuid(pet ? pet->GetObjectGuid() : ObjectGuid()); }
        void RemoveMiniPet();

        void AddGuardian(Pet* pet);
        void RemoveGuardian(Pet* pet);
        void RemoveGuardians();
        Pet* FindGuardianWithEntry(uint32 entry);
        uint32 CountGuardiansWithEntry(uint32 entry);
        Pet* GetProtectorPet();                             // expected single case in guardian list

        CharmInfo* GetCharmInfo() { return m_charmInfo; }
        virtual CharmInfo* InitCharmInfo(Unit* charm);
        virtual void DeleteCharmInfo() { delete m_charmInfo; m_charmInfo = nullptr; }

        ObjectGuid const& GetTotemGuid(TotemSlot slot) const { return m_TotemSlot[slot]; }
        Totem* GetTotem(TotemSlot slot) const;
        bool IsAllTotemSlotsUsed() const;

        void _AddTotem(TotemSlot slot, Totem* totem);       // only for call from Totem summon code
        void _RemoveTotem(Totem* totem);                    // only for call from Totem class

        template<typename Func>
        void CallForAllControlledUnits(Func const& func, uint32 controlledMask);
        template<typename Func>
        bool CheckAllControlledUnits(Func const& func, uint32 controlledMask) const;

        bool AddSpellAuraHolder(SpellAuraHolder* holder);
        void AddAuraToModList(Aura* aura);

        // removing specific aura stack
        void RemoveAura(Aura* aura, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAura(uint32 spellId, SpellEffectIndex effindex, Aura* except = nullptr);
        void RemoveSpellAuraHolder(SpellAuraHolder* holder, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveSingleAuraFromSpellAuraHolder(SpellAuraHolder* holder, SpellEffectIndex index, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveSingleAuraFromSpellAuraHolder(uint32 id, SpellEffectIndex index, ObjectGuid casterGuid, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);

        // removing specific aura stacks by diff reasons and selections
        void RemoveAurasDueToSpell(uint32 spellId, SpellAuraHolder* except = nullptr, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId);
        void RemoveAurasByCasterSpell(uint32 spellId, ObjectGuid casterGuid);
        void RemoveAurasDueToSpellBySteal(uint32 spellId, ObjectGuid casterGuid, Unit* stealer);
        void RemoveAurasDueToSpellByCancel(uint32 spellId);
        void RemoveAurasTriggeredBySpell(uint32 spellId, ObjectGuid casterGuid = ObjectGuid());
        void RemoveAuraStack(uint32 spellId);

        // removing unknown aura stacks by diff reasons and selections
        void RemoveNotOwnTrackedTargetAuras(uint32 newPhase = 0x0);
        void RemoveAurasAtMechanicImmunity(uint32 mechMask, uint32 exceptSpellId, bool non_positive = false);
        void RemoveSpellsCausingAura(AuraType auraType);
        void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except);
        void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except, bool onlyNegative);
        void RemoveSpellsCausingAura(AuraType auraType, ObjectGuid casterGuid);
        void RemoveRankAurasDueToSpell(uint32 spellId);
        bool RemoveNoStackAurasDueToAuraHolder(SpellAuraHolder* holder);
        void RemoveAurasWithInterruptFlags(uint32 flags);
        void RemoveAurasWithDispelType(DispelType type, ObjectGuid casterGuid = ObjectGuid());
        void RemoveAllAuras(AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveArenaAuras(bool onleave = false);
        void RemoveAllAurasOnDeath();
        void RemoveAllAurasOnEvade();
        template <class T>
        void RemoveAurasWithAttribute(T flags);

        // remove specific aura on cast
        void RemoveAurasOnCast(SpellEntry const* castedSpellEntry);

        // removing specific aura FROM stack by diff reasons and selections
        void RemoveAuraHolderFromStack(uint32 spellId, uint32 stackAmount = 1, ObjectGuid casterGuid = ObjectGuid(), AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAuraHolderDueToSpellByDispel(uint32 spellId, uint32 stackAmount, ObjectGuid casterGuid, Unit* dispeller);

        void DelaySpellAuraHolder(uint32 spellId, int32 delaytime, ObjectGuid casterGuid);

        float GetResistanceBuffMods(SpellSchools school, bool positive) const { return GetFloatValue(positive ? UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + school); }
        void SetResistanceBuffMods(SpellSchools school, bool positive, float val) { SetFloatValue(positive ? UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val); }
        void ApplyResistanceBuffModsMod(SpellSchools school, bool positive, float val, bool apply) { ApplyModSignedFloatValue(positive ? UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val, apply); }
        void ApplyResistanceBuffModsPercentMod(SpellSchools school, bool positive, float val, bool apply) { ApplyPercentModFloatValue(positive ? UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val, apply); }
        void InitStatBuffMods()
        {
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(UNIT_FIELD_POSSTAT0 + i, 0);
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(UNIT_FIELD_NEGSTAT0 + i, 0);
        }
        void ApplyStatBuffMod(Stats stat, float val, bool apply) { ApplyModSignedFloatValue((val > 0 ? UNIT_FIELD_POSSTAT0 + stat : UNIT_FIELD_NEGSTAT0 + stat), val, apply); }
        void ApplyStatPercentBuffMod(Stats stat, float val, bool apply)
        {
            ApplyPercentModFloatValue(UNIT_FIELD_POSSTAT0 + stat, val, apply);
            ApplyPercentModFloatValue(UNIT_FIELD_NEGSTAT0 + stat, val, apply);
        }
        void SetCreateStat(Stats stat, float val) { m_createStats[stat] = val; }
        void SetCreateHealth(uint32 val) { SetUInt32Value(UNIT_FIELD_BASE_HEALTH, val); }
        uint32 GetCreateHealth() const { return GetUInt32Value(UNIT_FIELD_BASE_HEALTH); }
        void SetCreateMana(uint32 val) { SetUInt32Value(UNIT_FIELD_BASE_MANA, val); }
        uint32 GetCreateMana() const { return GetUInt32Value(UNIT_FIELD_BASE_MANA); }
        uint32 GetCreatePowers(Powers power) const;
        float GetPosStat(Stats stat) const { return GetFloatValue(UNIT_FIELD_POSSTAT0 + stat); }
        float GetNegStat(Stats stat) const { return GetFloatValue(UNIT_FIELD_NEGSTAT0 + stat); }
        float GetCreateStat(Stats stat) const { return m_createStats[stat]; }

        void SetCurrentCastedSpell(Spell* pSpell);
        virtual void ProhibitSpellSchool(SpellSchoolMask /*idSchoolMask*/, uint32 /*unTimeMs*/) { }
        void InterruptSpell(CurrentSpellTypes spellType, bool withDelayed = true, bool sendAutoRepeatCancelToClient = true);
        void FinishSpell(CurrentSpellTypes spellType, bool ok = true);

        // set withDelayed to true to account delayed spells as casted
        // delayed+channeled spells are always accounted as casted
        // we can skip channeled or delayed checks using flags
        bool IsNonMeleeSpellCasted(bool withDelayed, bool skipChanneled = false, bool skipAutorepeat = false) const;

        // set withDelayed to true to interrupt delayed spells too
        // delayed+channeled spells are always interrupted
        void InterruptNonMeleeSpells(bool withDelayed, uint32 spellid = 0);

        Spell* GetCurrentSpell(CurrentSpellTypes spellType) const { return m_currentSpells[spellType]; }
        Spell* FindCurrentSpellBySpellId(uint32 spell_id) const;

        bool CheckAndIncreaseCastCounter();
        void DecreaseCastCounter() { if (m_castCounter) --m_castCounter; }

        ObjectGuid m_ObjectSlotGuid[4];
        uint32 m_detectInvisibilityMask;
        uint32 m_invisibilityMask;

        ShapeshiftForm GetShapeshiftForm() const { return ShapeshiftForm(GetByteValue(UNIT_FIELD_BYTES_2, 3)); }
        void  SetShapeshiftForm(ShapeshiftForm form) { SetByteValue(UNIT_FIELD_BYTES_2, 3, form); }

        bool IsShapeShifted() const;
        bool IsInFeralForm() const
        {
            ShapeshiftForm form = GetShapeshiftForm();
            return form == FORM_CAT || form == FORM_BEAR || form == FORM_DIREBEAR;
        }

        bool IsInDisallowedMountForm() const
        {
            ShapeshiftForm form = GetShapeshiftForm();
            return form != FORM_NONE && form != FORM_BATTLESTANCE && form != FORM_BERSERKERSTANCE && form != FORM_DEFENSIVESTANCE &&
                   form != FORM_SHADOW && form != FORM_STEALTH;
        }

        float m_modMeleeHitChance;
        float m_modRangedHitChance;
        float m_modSpellHitChance;
        float m_modSpellCritChance[MAX_SPELL_SCHOOL];

        float m_modCritChance[MAX_ATTACK];
        float m_modDodgeChance;
        float m_modDodgeChanceDiminishing;
        float m_modParryChance;
        float m_modParryChanceDiminishing;
        float m_modBlockChance;

        float m_threatModifier[MAX_SPELL_SCHOOL];
        float m_modAttackSpeedPct[3];

        // Event handler
        EventProcessor m_Events;

        // stat system
        bool HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply);
        void SetModifierValue(UnitMods unitMod, UnitModifierType modifierType, float value) { m_auraModifiersGroup[unitMod][modifierType] = value; }
        float GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const;
        float GetTotalStatValue(Stats stat) const;
        float GetTotalAuraModValue(UnitMods unitMod) const;
        SpellSchools GetSpellSchoolByAuraGroup(UnitMods unitMod) const;
        Stats GetStatByAuraGroup(UnitMods unitMod) const;
        Powers GetPowerTypeByAuraGroup(UnitMods unitMod) const;
        bool CanModifyStats() const { return m_canModifyStats; }
        void SetCanModifyStats(bool modifyStats) { m_canModifyStats = modifyStats; }
        virtual bool UpdateStats(Stats stat) = 0;
        virtual bool UpdateAllStats() = 0;
        virtual void UpdateResistances(uint32 school) = 0;
        virtual void UpdateArmor() = 0;
        virtual void UpdateMaxHealth() = 0;
        virtual void UpdateMaxPower(Powers power) = 0;
        virtual void UpdateAttackPowerAndDamage(bool ranged = false) = 0;
        virtual void UpdateDamagePhysical(WeaponAttackType attType) = 0;
        float GetTotalAttackPowerValue(WeaponAttackType attType) const;
        float GetWeaponDamageRange(WeaponAttackType attType, WeaponDamageRange type) const;
        void SetBaseWeaponDamage(WeaponAttackType attType, WeaponDamageRange damageRange, float value) { m_weaponDamage[attType][damageRange] = value; }

        // Visibility system
        UnitVisibility GetVisibility() const { return m_Visibility; }
        void SetVisibility(UnitVisibility x);
        void UpdateVisibilityAndView() override;            // overwrite WorldObject::UpdateVisibilityAndView()

        // common function for visibility checks for player/creatures with detection code
        bool isVisibleForOrDetect(Unit const* u, WorldObject const* viewPoint, bool detect, bool inVisibleList = false, bool is3dDistance = true) const;
        bool canDetectInvisibilityOf(Unit const* u) const;
        void SetPhaseMask(uint32 newPhaseMask, bool update) override;// overwrite WorldObject::SetPhaseMask
        float GetVisibleDist(Unit const* u);

        // virtual functions for all world objects types
        bool isVisibleForInState(Player const* u, WorldObject const* viewPoint, bool inVisibleList) const override;
        // function for low level grid visibility checks in player/creature cases
        virtual bool IsVisibleInGridForPlayer(Player* pl) const = 0;
        bool isInvisibleForAlive() const;

        TrackedAuraTargetMap&       GetTrackedAuraTargets(TrackedAuraType type)       { return m_trackedAuraTargets[type]; }
        TrackedAuraTargetMap const& GetTrackedAuraTargets(TrackedAuraType type) const { return m_trackedAuraTargets[type]; }
        SpellImmuneList m_spellImmune[MAX_SPELL_IMMUNITY];

        // Threat related methods
        bool CanHaveThreatList(bool ignoreAliveState = false) const;
        void AddThreat(Unit* pVictim, float threat = 0.0f, bool crit = false, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NONE, SpellEntry const* threatSpell = nullptr);
        float ApplyTotalThreatModifier(float threat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL);
        void DeleteThreatList();
        bool IsSecondChoiceTarget(Unit* pTarget, bool taunt, bool checkThreatArea) const;
        bool SelectHostileTarget();
        void TauntApply(Unit* pVictim);
        void TauntFadeOut(Unit* taunter);
        void FixateTarget(Unit* pVictim);
        ObjectGuid GetFixateTargetGuid() const { return m_fixateTargetGuid; }
        ThreatManager& getThreatManager() { return GetCombatData()->threatManager; }
        ThreatManager const& getThreatManager() const { return const_cast<Unit*>(this)->GetCombatData()->threatManager; }
        void addHatedBy(HostileReference* pHostileReference) { GetCombatData()->hostileRefManager.insertFirst(pHostileReference); };
        void removeHatedBy(HostileReference* /*pHostileReference*/) { /* nothing to do yet */ }
        HostileRefManager& getHostileRefManager() { return GetCombatData()->hostileRefManager; }

        uint32 GetVisibleAura(uint8 slot) const
        {
            VisibleAuraMap::const_iterator itr = m_visibleAuras.find(slot);
            if (itr != m_visibleAuras.end())
                return itr->second;
            return 0;
        }
        void SetVisibleAura(uint8 slot, uint32 spellid)
        {
            if (spellid == 0)
                m_visibleAuras.erase(slot);
            else
                m_visibleAuras[slot] = spellid;
        }
        VisibleAuraMap const& GetVisibleAuras() const { return m_visibleAuras; }
        uint8 GetVisibleAurasCount() const { return m_visibleAuras.size(); }

        Aura* GetAura(uint32 spellId, SpellEffectIndex effindex);
        Aura* GetAura(AuraType type, SpellFamily family, uint64 familyFlag, uint32 familyFlag2 = 0, ObjectGuid casterGuid = ObjectGuid()) const;
        Aura* GetTriggeredByClientAura(uint32 spellId) const;
        SpellAuraHolder* GetSpellAuraHolder(uint32 spellid) const;
        SpellAuraHolder* GetSpellAuraHolder(uint32 spellid, ObjectGuid casterGUID) const;

        SpellAuraHolderMap&       GetSpellAuraHolderMap()       { return m_spellAuraHolders; }
        SpellAuraHolderMap const& GetSpellAuraHolderMap() const { return m_spellAuraHolders; }
        AuraList const& GetAurasByType(AuraType type) const { return m_modAuras[type]; }
        void ApplyAuraProcTriggerDamage(Aura* aura, bool apply);

        int32 GetTotalAuraModifier(AuraType auratype) const;
        float GetTotalAuraMultiplier(AuraType auratype) const;
        int32 GetMaxPositiveAuraModifier(AuraType auratype) const;
        int32 GetMaxNegativeAuraModifier(AuraType auratype) const;

        int32 GetTotalAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const;
        float GetTotalAuraMultiplierByMiscMask(AuraType auratype, uint32 misc_mask) const;
        int32 GetMaxPositiveAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const;
        int32 GetMaxNegativeAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const;

        int32 GetTotalAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;
        float GetTotalAuraMultiplierByMiscValue(AuraType auratype, int32 misc_value) const;
        int32 GetMaxPositiveAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;
        int32 GetMaxNegativeAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;

        // misc have plain value but we check it fit to provided values mask (mask & (1 << (misc-1)))
        float GetTotalAuraMultiplierByMiscValueForMask(AuraType auratype, uint32 mask) const;

        Aura* GetDummyAura(uint32 spell_id) const;

        uint32 m_AuraFlags;

        uint32 GetDisplayId() const { return GetUInt32Value(UNIT_FIELD_DISPLAYID); }
        void SetDisplayId(uint32 modelId);
        uint32 GetNativeDisplayId() const { return GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID); }
        void SetNativeDisplayId(uint32 modelId) { SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, modelId); }
        void setTransForm(uint32 spellid) { m_transform = spellid;}
        uint32 getTransForm() const { return m_transform;}

        // at any changes to scale and/or displayId
        void UpdateModelData();
        void SendCollisionHeightUpdate(float height);

        DynamicObject* GetDynObject(uint32 spellId, SpellEffectIndex effIndex);
        DynamicObject* GetDynObject(uint32 spellId);
        void AddDynObject(DynamicObject* dynObj);
        void RemoveDynObject(uint32 spellid);
        void RemoveDynObjectWithGUID(ObjectGuid guid) { m_dynObjGUIDs.remove(guid); }
        void RemoveAllDynObjects();

        GameObject* GetGameObject(uint32 spellId) const;
        void AddGameObject(GameObject* gameObj);
        void AddWildGameObject(GameObject* gameObj);
        void RemoveGameObject(GameObject* gameObj, bool del);
        void RemoveGameObject(uint32 spellid, bool del);
        void RemoveAllGameObjects();

        uint32 CalculateDamage(WeaponAttackType attType, bool normalized);
        float GetAPMultiplier(WeaponAttackType attType, bool normalized);
        void ModifyAuraState(AuraState flag, bool apply);
        bool HasAuraState(AuraState flag) const { return HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1)); }
        bool HasAuraStateForCaster(AuraState flag, ObjectGuid casterGuid) const;
        void UnsummonAllTotems() const;
        Unit* SelectMagnetTarget(Unit* victim, Spell* spell = nullptr, SpellEffectIndex eff = EFFECT_INDEX_0);

        int32 SpellBonusWithCoeffs(SpellEntry const* spellProto, int32 total, int32 benefit, int32 ap_benefit, DamageEffectType damagetype, bool donePart, float defCoeffMod = 1.0f);
        int32 SpellBaseDamageBonusDone(SpellSchoolMask schoolMask);
        int32 SpellBaseDamageBonusTaken(SpellSchoolMask schoolMask) const;
        uint32 SpellDamageBonusDone(Unit* pVictim, SpellEntry const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint32 stack = 1);
        uint32 SpellDamageBonusTaken(Unit* pCaster, SpellEntry const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint32 stack = 1);
        int32 SpellBaseHealingBonusDone(SpellSchoolMask schoolMask);
        int32 SpellBaseHealingBonusTaken(SpellSchoolMask schoolMask) const;
        uint32 SpellHealingBonusDone(Unit* pVictim, SpellEntry const* spellProto, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);
        uint32 SpellHealingBonusTaken(Unit* pCaster, SpellEntry const* spellProto, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);
        uint32 MeleeDamageBonusDone(Unit* pVictim, uint32 damage, WeaponAttackType attType, SpellEntry const* spellProto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);
        uint32 MeleeDamageBonusTaken(Unit* pCaster, uint32 pdamage, WeaponAttackType attType, SpellEntry const* spellProto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);

        bool IsTriggeredAtSpellProcEvent(Unit* pVictim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, SpellProcEventEntry const*& spellProcEvent, bool dontTriggerSpecial);
        // Aura proc handlers
        SpellAuraProcResult HandleDummyAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleHasteAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleSpellCritChanceAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleProcTriggerSpellAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleProcTriggerDamageAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleOverrideClassScriptAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleMendingAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModCastingSpeedNotStackAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleReflectSpellsSchoolAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModPowerCostSchoolAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleMechanicImmuneResistanceAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModDamageFromCasterAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleAddFlatModifierAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleAddPctModifierAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModDamagePercentDoneAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModRating(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleSpellMagnetAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleManaShieldAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleModResistanceAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleRemoveByDamageChanceProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleInvisibilityAuraProc(Unit* pVictim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown);
        SpellAuraProcResult HandleNULLProc(Unit* /*pVictim*/, uint32 /*damage*/, Aura* /*triggeredByAura*/, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
        {
            // no proc handler for this aura type
            return SPELL_AURA_PROC_OK;
        }
        SpellAuraProcResult HandleCantTrigger(Unit* /*pVictim*/, uint32 /*damage*/, Aura* /*triggeredByAura*/, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
        {
            // this aura type can't proc
            return SPELL_AURA_PROC_CANT_TRIGGER;
        }

        void SetLastManaUse()
        {
            if (GetTypeId() == TYPEID_PLAYER && !IsUnderLastManaUseEffect())
                RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);

            m_lastManaUseTimer = 5000;
        }
        bool IsUnderLastManaUseEffect() const { return !!m_lastManaUseTimer; }

        uint32 GetRegenTimer() const { return m_regenTimer; }

        void SetContestedPvP(Player* attackedPlayer = nullptr);

        void ApplySpellImmune(Aura const* aura, uint32 op, uint32 type, bool apply);
        void ApplySpellDispelImmunity(Aura const* aura, DispelType type, bool apply);
        virtual bool IsImmuneToSpell(SpellEntry const* spellInfo, bool castOnSelf);
        virtual bool IsImmuneToDamage(SpellSchoolMask meleeSchoolMask);
        virtual bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex index, bool castOnSelf) const;

        uint32 CalcArmorReducedDamage(Unit* pVictim, const uint32 damage);
        void CalculateDamageAbsorbAndResist(Unit* pCaster, SpellSchoolMask schoolMask, DamageEffectType damagetype, const uint32 damage, uint32* absorb, uint32* resist, bool canReflect = false, bool canResist = true, bool binary = false);
        void CalculateAbsorbResistBlock(Unit* pCaster, SpellNonMeleeDamage* damageInfo, SpellEntry const* spellProto, WeaponAttackType attType = BASE_ATTACK);
        void CalculateHealAbsorb(uint32 heal, uint32* absorb);

        void  UpdateSpeed(UnitMoveType mtype, bool forced, float ratio = 1.0f);
        float GetSpeed(UnitMoveType mtype) const;
        float GetSpeedRate(UnitMoveType mtype) const { return m_speed_rate[mtype]; }
        void SetSpeedRate(UnitMoveType mtype, float rate, bool forced = false);

        void KnockBackFrom(Unit* target, float horizontalSpeed, float verticalSpeed);
        void KnockBackWithAngle(float angle, float horizontalSpeed, float verticalSpeed);

        void _RemoveAllAuraMods();
        void _ApplyAllAuraMods();

        int32 CalculateSpellDamage(Unit const* target, SpellEntry const* spellProto, SpellEffectIndex effect_index, int32 const* basePoints = nullptr);

        uint32 CalcNotIgnoreAbsorbDamage(uint32 damage, SpellSchoolMask damageSchoolMask, SpellEntry const* spellInfo = nullptr) const;
        uint32 CalcNotIgnoreDamageReduction(uint32 damage, SpellSchoolMask damageSchoolMask) const;
        int32 CalculateAuraDuration(SpellEntry const* spellProto, uint32 effectMask, int32 duration, Unit const* caster);

        float CalculateLevelPenalty(SpellEntry const* spellProto) const;

        void addFollower(FollowerReference* pRef) { m_FollowingRefManager.insertFirst(pRef); }
        void removeFollower(FollowerReference* /*pRef*/) { /* nothing to do yet */ }

        MotionMaster* GetMotionMaster() { return &i_motionMaster; }

        bool IsStopped() const { return !(hasUnitState(UNIT_STAT_MOVING)); }
        void StopMoving(bool forceSendStop = false);
        void InterruptMoving(bool forceSendStop = false);

        ///----------Various crowd control methods-----------------
        bool IsImmobilized() const { return hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED); }
        void SetImmobilizedState(bool apply, bool stun = false);

        // These getters operate on unit flags set by IncapacitatedState and are meant for formal usage in conjunction with spell effects only
        // For actual internal movement states use UnitState flags
        // TODO: The UnitState thing needs to be rewriten at some point, this kind of duality is bad
        bool IsFleeing() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING); }
        bool IsConfused() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED); }
        bool IsStunned() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED); }
        bool IsIncapacitated() const { return (IsFleeing() || IsConfused() || IsStunned()); }

        void SetFeared(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, uint32 time = 0);
        void SetConfused(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0);
        void SetStunned(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0);
        void SetIncapacitatedState(bool apply, uint32 state = 0, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, uint32 time = 0);
        ///----------End of crowd control methods----------

        bool IsFeigningDeath() const { return HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH); }
        bool IsFeigningDeathSuccessfully() const { return hasUnitState(UNIT_STAT_FEIGN_DEATH); }
        void SetFeignDeath(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, bool dynamic = true, bool success = true);

        void InterruptSpellsCastedOnMe(bool killDelayed = false);

        void AddComboPointHolder(uint32 lowguid) { m_ComboPointHolders.insert(lowguid); }
        void RemoveComboPointHolder(uint32 lowguid) { m_ComboPointHolders.erase(lowguid); }
        void ClearComboPointHolders();

        ///----------Pet responses methods-----------------
        void SendPetActionFeedback(uint8 msg) const;
        void SendPetTalk(uint32 pettalk) const;
        void SendPetAIReaction() const;
        ///----------End of Pet responses methods----------

        void propagateSpeedChange() { GetMotionMaster()->propagateSpeedChange(); }

        // reactive attacks
        void ClearAllReactives();
        void StartReactiveTimer(ReactiveType reactive) { m_reactiveTimer[reactive] = REACTIVE_TIMER_START;}
        void UpdateReactives(uint32 p_time);

        // group updates
        void UpdateAuraForGroup(uint8 slot);

        // pet auras
        typedef std::set<PetAura const*> PetAuraSet;
        PetAuraSet m_petAuras;
        void AddPetAura(PetAura const* petSpell);
        void RemovePetAura(PetAura const* petSpell);

        // Movement info
        MovementInfo m_movementInfo;
        Movement::MoveSpline* movespline;

        void ScheduleAINotify(uint32 delay);
        bool IsAINotifyScheduled() const { return m_AINotifyScheduled;}
        void _SetAINotifyScheduled(bool on) { m_AINotifyScheduled = on;}       // only for call from RelocationNotifyEvent code
        void OnRelocated();

        bool IsLinkingEventTrigger() const { return m_isCreatureLinkingTrigger; }

        virtual bool CanSwim() const = 0;
        virtual bool CanFly() const = 0;
        virtual bool CanWalk() const = 0;

        void TriggerEvadeEvents();

        // Take possession of an unit (pet, creature, ...)
        bool TakePossessOf(Unit* possessed);

        // Take possession of a new spawned unit
        Unit* TakePossessOf(SpellEntry const* spellEntry, SummonPropertiesEntry const* summonProp, uint32 effIdx, float x, float y, float z, float ang);

        // Take charm of an unit
        bool TakeCharmOf(Unit* charmed, bool advertised = true);

        // Break own charm effects on a specific charmed unit
        void BreakCharmOutgoing(Unit* charmed);

        // Break own charm effects on all charmed units or advertised charm field only
        void BreakCharmOutgoing(bool advertisedOnly = false);

        // Break charm effects from current charmer
        void BreakCharmIncoming();

        // Uncharm (physically revert the charm effect) the unit and reset player control if required
        void Uncharm(Unit* charmed);

        void SetTurningOff(bool apply);
        virtual bool IsIgnoringRangedTargets() { return false; }

        float GetAttackDistance(Unit const* pl) const;
        virtual uint32 GetDetectionRange() const { return 20.f; }

        virtual CreatureAI* AI() { return nullptr; }
        virtual CombatData* GetCombatData() { return m_combatData; }

        virtual void AddCooldown(SpellEntry const& spellEntry, ItemPrototype const* itemProto = nullptr, bool permanent = false, uint32 forcedDuration = 0) override;

    protected:
        explicit Unit();

        void _UpdateSpells(uint32 time);
        void _UpdateAutoRepeatSpell();

        uint32 m_attackTimer[MAX_ATTACK];

        float m_createStats[MAX_STATS];

        Unit* m_attacking;

        DeathState m_deathState;

        SpellAuraHolderMap m_spellAuraHolders;
        SpellAuraHolderMap::iterator m_spellAuraHoldersUpdateIterator; // != end() in Unit::m_spellAuraHolders update and point to next element
        AuraList m_deletedAuras;                            // auras removed while in ApplyModifier and waiting deleted
        SpellAuraHolderList m_deletedHolders;

        // Store Auras for which the target must be tracked
        TrackedAuraTargetMap m_trackedAuraTargets[MAX_TRACKED_AURA_TYPES];

        GuidList m_dynObjGUIDs;

        typedef std::list<GameObject*> GameObjectList;
        GameObjectList m_gameObj;
        typedef std::map<uint32, ObjectGuid> WildGameObjectMap;
        WildGameObjectMap m_wildGameObjs;
        bool m_isSorted;
        uint32 m_transform;

        AuraList m_modAuras[TOTAL_AURAS];
        float m_auraModifiersGroup[UNIT_MOD_END][MODIFIER_TYPE_END];
        float m_weaponDamage[MAX_ATTACK][2];
        bool m_canModifyStats;
        // std::list< spellEffectPair > AuraSpells[TOTAL_AURAS];  // TODO: use this if ok for mem
        VisibleAuraMap m_visibleAuras;

        float m_speed_rate[MAX_MOVE_TYPE];

        CharmInfo* m_charmInfo;

        virtual SpellSchoolMask GetMeleeDamageSchoolMask() const;

        MotionMaster i_motionMaster;

        uint32 m_reactiveTimer[MAX_REACTIVE];
        uint32 m_regenTimer;
        uint32 m_lastManaUseTimer;

        bool m_canDodge;
        bool m_canParry;
        bool m_canBlock;

        VehicleInfo* m_vehicleInfo;
        void DisableSpline();
        bool m_isCreatureLinkingTrigger;
        bool m_isSpawningLinked;

        CombatData* m_combatData;

        virtual void SetBaseWalkSpeed(float speed) { m_baseSpeedWalk = speed; }
        virtual void SetBaseRunSpeed(float speed) { m_baseSpeedRun = speed; }

        // base speeds set by model/template
        float m_baseSpeedWalk;
        float m_baseSpeedRun;

        // Protected unit guid fields getters/setters
        // Charm: temporary pet unit guid
        ObjectGuid const& GetCharmGuid() const { return GetGuidValue(UNIT_FIELD_CHARM); }
        void SetCharmGuid(ObjectGuid const& charm) { SetGuidValue(UNIT_FIELD_CHARM, charm); }
        // Summon: permanent pet unit guid (do not use: managed by SetPetGuid/GetPetGuid)
        ObjectGuid const& GetSummonGuid() const { return GetGuidValue(UNIT_FIELD_SUMMON); }
        void SetSummonGuid(ObjectGuid const& summon) { SetGuidValue(UNIT_FIELD_SUMMON, summon); }
        // Charmer: temporary owner unit guid [nameplate]
        ObjectGuid const& GetCharmerGuid() const { return GetGuidValue(UNIT_FIELD_CHARMEDBY); }
        void SetCharmerGuid(ObjectGuid const& owner) { SetGuidValue(UNIT_FIELD_CHARMEDBY, owner); }
        // Summoner: permanent owner unit guid for player pets [nameplate] (do not use: managed by SetOwnerGuid/GetOwnerGuid)
        ObjectGuid const& GetSummonerGuid() const { return GetGuidValue(UNIT_FIELD_SUMMONEDBY); }
        void SetSummonerGuid(ObjectGuid const& owner) { SetGuidValue(UNIT_FIELD_SUMMONEDBY, owner); }
        // Creator: permanent owner unit guid for npc pets or non-pet units [nameplate] (do not use: managed by SetOwnerGuid/GetOwnerGuid)
        ObjectGuid const& GetCreatorGuid() const { return GetGuidValue(UNIT_FIELD_CREATEDBY); }
        void SetCreatorGuid(ObjectGuid const& creator) { SetGuidValue(UNIT_FIELD_CREATEDBY, creator); }
        // Target: current target guid as advertised on unit frames (also known as selection)
        ObjectGuid const& GetTargetGuid() const { return GetGuidValue(UNIT_FIELD_TARGET); }
        void SetTargetGuid(ObjectGuid const& targetGuid) { SetGuidValue(UNIT_FIELD_TARGET, targetGuid); }
        // Channel target: current channeling spell's target worldobject guid
        ObjectGuid const& GetChannelObjectGuid() const { return GetGuidValue(UNIT_FIELD_CHANNEL_OBJECT); }
        void SetChannelObjectGuid(ObjectGuid const& targetGuid) { SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, targetGuid); }
        // Critter: permanent mini-pet unit guid
        ObjectGuid const& GetCritterGuid() const { return GetGuidValue(UNIT_FIELD_CRITTER); }
        void SetCritterGuid(ObjectGuid critterGuid) { SetGuidValue(UNIT_FIELD_CRITTER, critterGuid); }

    private:
        void CleanupDeletedAuras();
        void UpdateSplineMovement(uint32 t_diff);

        // player or player's pet
        float GetCombatRatingReduction(CombatRating cr) const;
        uint32 GetCombatRatingDamageReduction(CombatRating cr, float rate, float cap, uint32 damage) const;

        Unit* _GetTotem(TotemSlot slot) const;              // for templated function without include need
        Pet* _GetPet(ObjectGuid guid) const;                // for templated function without include need
        Unit* _GetUnit(ObjectGuid guid) const;              // for templated function without include need

        // Wrapper called by DealDamage when a creature is killed
        void JustKilledCreature(Creature* victim, Player* responsiblePlayer);

        uint32 m_state;                                     // Even derived shouldn't modify
        uint32 m_CombatTimer;

        AttackerSet m_attackers;                            // Used to help know who is currently attacking this unit
        Spell* m_currentSpells[CURRENT_MAX_SPELL];
        uint32 m_castCounter;                               // count casts chain of triggered spells for prevent infinity cast crashes

        UnitVisibility m_Visibility;
        Position m_last_notified_position;
        bool m_AINotifyScheduled;
        ShortTimeTracker m_movesplineTimer;

        Diminishing m_Diminishing;

        FollowerRefManager m_FollowingRefManager;

        ComboPointHolderSet m_ComboPointHolders;

        GuidSet m_guardianPets;

        GuidSet m_charmedUnitsPrivate;                      // stores non-advertised active charmed unit guids (e.g. aoe charms)

        ObjectGuid m_TotemSlot[MAX_TOTEM_SLOT];

        ObjectGuid m_fixateTargetGuid;                      //< Stores the Guid of a fixated target

        // Need to safeguard aura application in Unit::Update
        bool m_spellUpdateHappening;

        // guard to prevent chaining extra attacks
        bool m_extraAttacksExecuting;
    private:                                                // Error traps for some wrong args using
        // this will catch and prevent build for any cases when all optional args skipped and instead triggered used non boolean type
        // no bodies expected for this declarations
        template <typename TR>
        void CastSpell(Unit* Victim, uint32 spell, TR triggered);
        template <typename TR>
        void CastSpell(Unit* Victim, SpellEntry const* spell, TR triggered);
        template <typename TR>
        void CastCustomSpell(Unit* Victim, uint32 spell, int32 const* bp0, int32 const* bp1, int32 const* bp2, TR triggeredFlags);
        template <typename SP, typename TR>
        void CastCustomSpell(Unit* Victim, SpellEntry const* spell, int32 const* bp0, int32 const* bp1, int32 const* bp2, TR triggeredFlags);
        template <typename TR>
        void CastSpell(float x, float y, float z, uint32 spell, TR triggered);
        template <typename TR>
        void CastSpell(float x, float y, float z, SpellEntry const* spell, TR triggered);
};

template<typename Func>
void Unit::CallForAllControlledUnits(Func const& func, uint32 controlledMask)
{
    if (controlledMask & CONTROLLED_PET)
        if (Pet* pet = GetPet())
            func(pet);

    if (controlledMask & CONTROLLED_MINIPET)
        if (Pet* mini = GetMiniPet())
            func(mini);

    if (controlledMask & CONTROLLED_GUARDIANS)
    {
        for (GuidSet::const_iterator itr = m_guardianPets.begin(); itr != m_guardianPets.end();)
            if (Pet* guardian = _GetPet(*(itr++)))
                func(guardian);
    }

    if (controlledMask & CONTROLLED_TOTEMS)
    {
        for (int i = 0; i < MAX_TOTEM_SLOT; ++i)
            if (Unit* totem = _GetTotem(TotemSlot(i)))
                func(totem);
    }

    if (controlledMask & CONTROLLED_CHARM)
    {
        if (Unit* charm = GetCharm())
            func(charm);

        for (GuidSet::const_iterator itr = m_charmedUnitsPrivate.begin(); itr != m_charmedUnitsPrivate.end();)
            if (Unit* charmed = _GetUnit(*(itr++)))
                func(charmed);
    }
}

template<typename Func>
bool Unit::CheckAllControlledUnits(Func const& func, uint32 controlledMask) const
{
    if (controlledMask & CONTROLLED_PET)
        if (Pet const* pet = GetPet())
            if (func(pet))
                return true;

    if (controlledMask & CONTROLLED_MINIPET)
        if (Pet const* mini = GetMiniPet())
            if (func(mini))
                return true;

    if (controlledMask & CONTROLLED_GUARDIANS)
    {
        for (GuidSet::const_iterator itr = m_guardianPets.begin(); itr != m_guardianPets.end();)
            if (Pet const* guardian = _GetPet(*(itr++)))
                if (func(guardian))
                    return true;
    }

    if (controlledMask & CONTROLLED_TOTEMS)
    {
        for (int i = 0; i < MAX_TOTEM_SLOT; ++i)
            if (Unit const* totem = _GetTotem(TotemSlot(i)))
                if (func(totem))
                    return true;
    }

    if (controlledMask & CONTROLLED_CHARM)
    {
        if (Unit const* charm = GetCharm())
            if (func(charm))
                return true;

        for (GuidSet::const_iterator itr = m_charmedUnitsPrivate.begin(); itr != m_charmedUnitsPrivate.end();)
            if (Unit const* charmed = _GetUnit(*(itr++)))
                if (func(charmed))
                    return true;
    }

    return false;
}

// Helper for targets nearest to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrderNear : public std::binary_function<Unit const, Unit const, bool>
{
    Unit const* m_mainTarget;
    DistanceCalculation m_distcalc;

    TargetDistanceOrderNear(Unit const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator ">"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        return m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
    }
};

// Helper for targets furthest away to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrderFarAway : public std::binary_function<Unit const, Unit const, bool>
{
    Unit const* m_mainTarget;
    DistanceCalculation m_distcalc;
    TargetDistanceOrderFarAway(Unit const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator "<"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        return !m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
    }
};

struct LowestHPNearestOrder : public std::binary_function<Unit const, Unit const, bool>
{
    Unit const* m_mainTarget;
    DistanceCalculation m_distcalc;

    LowestHPNearestOrder(Unit const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator ">"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        if (_Left->GetHealthPercent() == _Right->GetHealthPercent())
            return m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
        else
            return _Left->GetHealthPercent() < _Right->GetHealthPercent();
    }
};

/** @} */

#endif
