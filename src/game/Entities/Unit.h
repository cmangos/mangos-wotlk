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
#include "Combat/CombatManager.h"
#include "Maps/MapManager.h"
#include "MotionGenerators/FollowerReference.h"
#include "MotionGenerators/FollowerRefManager.h"
#include "Utilities/EventProcessor.h"
#include "MotionGenerators/MotionMaster.h"
#include "Server/DBCStructure.h"
#include "Server/WorldPacket.h"
#include "Util/Timer.h"
#include "AI/BaseAI/UnitAI.h"
#include "Spells/SpellDefines.h"
#include "Maps/SpawnGroupDefines.h"

#include <list>
#include <array>

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

#define BASE_MELEERANGE_OFFSET 1.33f
#define BASE_MINDAMAGE 1.0f
#define BASE_MAXDAMAGE 2.0f
#define BASE_ATTACK_TIME 2000

#define SCALE_SPELLPOWER_HEALING        1.88f

enum UnitBytes0Offsets : uint8
{
    UNIT_BYTES_0_OFFSET_RACE       = 0,
    UNIT_BYTES_0_OFFSET_CLASS      = 1,
    UNIT_BYTES_0_OFFSET_GENDER     = 2,
    UNIT_BYTES_0_OFFSET_POWER_TYPE = 3,
};

enum UnitBytes1Offsets : uint8
{
    UNIT_BYTES_1_OFFSET_STAND_STATE     = 0,
    UNIT_BYTES_1_OFFSET_PET_LOYALTY     = 1,
    UNIT_BYTES_1_OFFSET_VIS_FLAGS       = 2,
    UNIT_BYTES_1_OFFSET_MISC_FLAGS      = 3,
};

enum UnitBytes2Offsets : uint8
{
    UNIT_BYTES_2_OFFSET_SHEATH_STATE = 0,
    UNIT_BYTES_2_OFFSET_PVP_FLAG     = 1,
    UNIT_BYTES_2_OFFSET_PET_FLAGS    = 2,
    UNIT_BYTES_2_OFFSET_SHAPESHIFT   = 3,
};

// byte value (UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_STAND_STATE)
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

// byte flags value (UNIT_FIELD_BYTES_1,UNIT_BYTES_1_OFFSET_PET_LOYALTY)
// This corresponds to free talent points (pet case)

// byte flags value (UNIT_FIELD_BYTES_1,2)
enum UnitVisFlags
{
    UNIT_VIS_FLAG_GHOST         = 0x01,
    UNIT_VIS_FLAG_CREEP         = 0x02,
    UNIT_VIS_FLAG_UNTRACKABLE   = 0x04,
    UNIT_VIS_FLAG_UNK4          = 0x08,
    UNIT_VIS_FLAG_UNK5          = 0x10,
    UNIT_VIS_FLAGS_ALL          = 0xFF
};

// byte flags value (UNIT_FIELD_BYTES_1,3)
// These flags seem to be related to miscellaneous animations
enum UnitMiscFlags
{
    UNIT_BYTE1_FLAG_GROUND       = 0x00,
    UNIT_BYTE1_FLAG_ALWAYS_STAND = 0x01,
    UNIT_BYTE1_FLAG_FLY_ANIM     = 0x02,                    // Creature that can fly and are not on the ground appear to have this flag. If they are on the ground, flag is not present.
    UNIT_BYTE1_FLAG_REAL_FLY_ANIM= 0x03,
    UNIT_BYTE1_FLAG_SUBMERGED    = 0x04,
    UNIT_BYTE1_FLAG_ALL          = 0xFF
};

// byte value (UNIT_FIELD_BYTES_2,UNIT_BYTES_2_OFFSET_SHEATH_STATE) // TODO - solve conflicting with SharedDefines.h enum SheathTypes
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
    UNIT_BYTE2_FLAG_AURAS       = 0x10,                     // show positive auras as positive, and allow its dispel
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

#define CREATURE_MAX_SPELLS     10

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
class SpellCastTargets;
class VehicleInfo;
class SpellCastArgs;

struct SpellImmune
{
    uint32 type;
    Aura const* aura;
};

typedef std::list<SpellImmune> SpellImmuneList;

enum UnitModifierType
{
    BASE_VALUE = 0,
    BASE_PCT = 1,
    TOTAL_VALUE = 2,
    TOTAL_PCT = 3,
    MODIFIER_TYPE_END = 4
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
    AURA_REMOVE_BY_GAINED_STACK                             // gained stack
};

enum AuraScriptLocation : uint32
{
    SCRIPT_LOCATION_MELEE_DAMAGE_DONE,
    SCRIPT_LOCATION_MELEE_DAMAGE_TAKEN,
    SCRIPT_LOCATION_SPELL_DAMAGE_DONE,
    SCRIPT_LOCATION_SPELL_DAMAGE_TAKEN,
    SCRIPT_LOCATION_SPELL_HEALING_DONE,
    SCRIPT_LOCATION_SPELL_HEALING_TAKEN,
    SCRIPT_LOCATION_CRIT_CHANCE,
    SCRIPT_LOCATION_MAX,
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
    UNIT_STAT_LOGOUT_TIMER    = 0x00000002,                 // Unit is logging out
    UNIT_STAT_FEIGN_DEATH     = 0x00000004,                 // Unit::SetFeignDeath - a successful feign death is currently active
    UNIT_STAT_STUNNED         = 0x00000008,                 // Aura::HandleAuraModStun
    UNIT_STAT_ROOT            = 0x00000010,                 // Aura::HandleAuraModRoot
    UNIT_STAT_ISOLATED        = 0x00000020,                 // area auras do not affect other players, Aura::HandleAuraModSchoolImmunity
    UNIT_STAT_POSSESSED       = 0x00000040,                 // Aura::HandleAuraModPossess (duplicates UNIT_FLAG_POSSESSED)

    // movement generators begin:
    UNIT_STAT_TAXI_FLIGHT     = 0x00000080,                 // TaxiMovementGenerator on stack
    UNIT_STAT_PROPELLED       = 0x00000100,                 // EffectMovementGenerator on stack
    UNIT_STAT_PANIC           = 0x00000200,                 // PanicMovementGenerator on stack
    UNIT_STAT_RETREATING      = 0x00000400,                 // RetreatMovementGenerator on stack
    UNIT_STAT_DISTRACTED      = 0x00000800,                 // DistractedMovementGenerator on stack
    UNIT_STAT_STAY            = 0x00001000,                 // StayMovementGenerator on stack
    UNIT_STAT_CONFUSED        = 0x00002000,                 // ConfusedMovementGenerator on stack
    UNIT_STAT_CONFUSED_MOVE   = 0x00004000,                 // ^ - spline dispatched
    UNIT_STAT_ROAMING         = 0x00008000,                 // Point/Retreat/Stay/Wander/Waypoint/Effect MovementGenerator on stack
    UNIT_STAT_ROAMING_MOVE    = 0x00010000,                 // ^ - spline dispatched
    UNIT_STAT_CHASE           = 0x00020000,                 // ChaseMovementGenerator on stack
    UNIT_STAT_CHASE_MOVE      = 0x00040000,                 // ^ - spline dispatched
    UNIT_STAT_FOLLOW          = 0x00080000,                 // FollowMovementGenerator on stack
    UNIT_STAT_FOLLOW_MOVE     = 0x00100000,                 // ^ - spline dispatched
    UNIT_STAT_FLEEING         = 0x00200000,                 // FleeingMovementGenerator/PanicMovementGenerator on stack
    UNIT_STAT_FLEEING_MOVE    = 0x00400000,                 // ^ - spline dispatched
    // movemement generators end

    UNIT_STAT_CHANNELING      = 0x00800000,

    // High-Level states (usually only with Creatures)
    UNIT_STAT_NO_COMBAT_MOVEMENT    = 0x01000000,           // Combat Movement for MoveChase stopped
    UNIT_STAT_NO_FOLLOW_MOVEMENT    = 0x02000000,
    UNIT_STAT_RUNNING               = 0x04000000,           // SetRun for waypoints and such
    UNIT_STAT_WAYPOINT_PAUSED       = 0x08000000,           // Waypoint-Movement paused genericly (ie by script)

    UNIT_STAT_IGNORE_PATHFINDING    = 0x10000000,           // do not use pathfinding in any MovementGenerator
    UNIT_STAT_AI_ROOT               = 0x20000000,           // root but from AI script

    // masks (only for check)

    // can't move currently
    UNIT_STAT_CAN_NOT_MOVE    = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_LOGOUT_TIMER | UNIT_STAT_FEIGN_DEATH | UNIT_STAT_AI_ROOT,

    // stay by different reasons
    UNIT_STAT_NOT_MOVE        = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_LOGOUT_TIMER | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_DISTRACTED | UNIT_STAT_AI_ROOT,

    // stay or scripted movement for effect( = in player case you can't move by client command)
    UNIT_STAT_NO_FREE_MOVE    = UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_LOGOUT_TIMER | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_TAXI_FLIGHT |
                                UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING | UNIT_STAT_PROPELLED | UNIT_STAT_AI_ROOT,

    // not react at move in sight or other
    UNIT_STAT_CAN_NOT_REACT   = UNIT_STAT_STUNNED | UNIT_STAT_LOGOUT_TIMER | UNIT_STAT_FEIGN_DEATH |
                                UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING | UNIT_STAT_RETREATING,

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

#define BASE_CHARGE_SPEED 27.0f

typedef const Opcodes SpeedOpcodePair[3];
SpeedOpcodePair SetSpeed2Opc_table[MAX_MOVE_TYPE] =
{
    {SMSG_FORCE_WALK_SPEED_CHANGE,        SMSG_SPLINE_SET_WALK_SPEED,           MSG_MOVE_SET_WALK_SPEED},
    {SMSG_FORCE_RUN_SPEED_CHANGE,         SMSG_SPLINE_SET_RUN_SPEED,            MSG_MOVE_SET_RUN_SPEED},
    {SMSG_FORCE_RUN_BACK_SPEED_CHANGE,    SMSG_SPLINE_SET_RUN_BACK_SPEED,       MSG_MOVE_SET_RUN_BACK_SPEED},
    {SMSG_FORCE_SWIM_SPEED_CHANGE,        SMSG_SPLINE_SET_SWIM_SPEED,           MSG_MOVE_SET_SWIM_SPEED},
    {SMSG_FORCE_SWIM_BACK_SPEED_CHANGE,   SMSG_SPLINE_SET_SWIM_BACK_SPEED,      MSG_MOVE_SET_SWIM_BACK_SPEED},
    {SMSG_FORCE_TURN_RATE_CHANGE,         SMSG_SPLINE_SET_TURN_RATE,            MSG_MOVE_SET_TURN_RATE},
    {SMSG_FORCE_FLIGHT_SPEED_CHANGE,      SMSG_SPLINE_SET_FLIGHT_SPEED,         MSG_MOVE_SET_FLIGHT_SPEED},
    {SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, SMSG_SPLINE_SET_FLIGHT_BACK_SPEED,    MSG_MOVE_SET_FLIGHT_BACK_SPEED},
    {SMSG_FORCE_PITCH_RATE_CHANGE,        SMSG_SPLINE_SET_PITCH_RATE,           MSG_MOVE_SET_PITCH_RATE},
};

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
    UNIT_FLAG_SERVER_CONTROLLED     = 0x00000001,           // Movement checks disabled, likely paired with loss of client control packet. We use it to add custom cliffwalking to GM mode until actual usecases will be known.
    UNIT_FLAG_SPAWNING              = 0x00000002,           // not attackable
    UNIT_FLAG_CLIENT_CONTROL_LOST   = 0x00000004,           // Generic unspecified loss of control initiated by server script, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_PLAYER_CONTROLLED     = 0x00000008,           // players, pets, totems, guardians, companions, charms, any units associated with players
    UNIT_FLAG_RENAME                = 0x00000010,
    UNIT_FLAG_PREPARATION           = 0x00000020,           // don't take reagents for spells with SPELL_ATTR_EX5_NO_REAGENT_WHILE_PREP
    UNIT_FLAG_UNK_6                 = 0x00000040,
    UNIT_FLAG_NOT_ATTACKABLE_1      = 0x00000080,           // ?? (UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1) is NON_PVP_ATTACKABLE
    UNIT_FLAG_IMMUNE_TO_PLAYER      = 0x00000100,           // Target is immune to players
    UNIT_FLAG_IMMUNE_TO_NPC         = 0x00000200,           // Target is immune to Non-Player Characters
    UNIT_FLAG_LOOTING               = 0x00000400,           // loot animation
    UNIT_FLAG_PET_IN_COMBAT         = 0x00000800,           // in combat?, 2.0.8
    UNIT_FLAG_PVP_DEPRECATED        = 0x00001000,           // changed in 3.0.3
    UNIT_FLAG_SILENCED              = 0x00002000,           // silenced, 2.1.1
    UNIT_FLAG_CANNOT_SWIM           = 0x00004000,           // 2.0.8
    UNIT_FLAG_SWIMMING              = 0x00008000,           // related to jerky movement in water?
    UNIT_FLAG_UNTARGETABLE          = 0x00010000,           // is not targetable by attack or spell
    UNIT_FLAG_PACIFIED              = 0x00020000,           // 3.0.3 ok
    UNIT_FLAG_STUNNED               = 0x00040000,           // Unit is a subject to stun, turn and strafe movement disabled
    UNIT_FLAG_IN_COMBAT             = 0x00080000,
    UNIT_FLAG_TAXI_FLIGHT           = 0x00100000,           // Unit is on taxi, paired with a duplicate loss of client control packet (likely a legacy serverside hack). Disables any spellcasts not allowed in taxi flight client-side.
    UNIT_FLAG_DISARMED              = 0x00200000,           // 3.0.3, disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
    UNIT_FLAG_CONFUSED              = 0x00400000,           // Unit is a subject to confused movement, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_FLEEING               = 0x00800000,           // Unit is a subject to fleeing movement, movement checks disabled, paired with loss of client control packet.
    UNIT_FLAG_POSSESSED             = 0x01000000,           // Unit is under remote control by another unit, movement checks disabled, paired with loss of client control packet. New master is allowed to use melee attack and can't select this unit via mouse in the world (as if it was own character).
    UNIT_FLAG_UNINTERACTIBLE        = 0x02000000,
    UNIT_FLAG_SKINNABLE             = 0x04000000,
    UNIT_FLAG_MOUNT                 = 0x08000000,
    UNIT_FLAG_UNK_28                = 0x10000000,
    UNIT_FLAG_PREVENT_ANIM          = 0x20000000,           // used in Feing Death spell
    UNIT_FLAG_SHEATHE               = 0x40000000,
    UNIT_FLAG_IMMUNE                = 0x80000000
};

// Value masks for UNIT_FIELD_FLAGS_2
enum UnitFlags2
{
    UNIT_FLAG2_FEIGN_DEATH            = 0x00000001,
    UNIT_FLAG2_HIDE_BODY              = 0x00000002,           // Hides body and body armor. Weapons and shoulder and head armor still visible
    UNIT_FLAG2_IGNORE_REPUTATION      = 0x00000004,
    UNIT_FLAG2_COMPREHEND_LANG        = 0x00000008,
    UNIT_FLAG2_CLONED                 = 0x00000010,           // Used in SPELL_AURA_MIRROR_IMAGE
    UNIT_FLAG2_DO_NOT_FADE_IN         = 0x00000020,
    UNIT_FLAG2_FORCE_MOVE             = 0x00000040,
    UNIT_FLAG2_DISARM_OFFHAND         = 0x00000080,           // also shield case
    UNIT_FLAG2_DISABLE_PRED_STATS     = 0x00000100,           // NYI
    UNIT_FLAG2_ALLOW_CHANGING_TALENTS = 0x00000200,           // NYI
    UNIT_FLAG2_DISARM_RANGED          = 0x00000400,
    UNIT_FLAG2_REGENERATE_POWER       = 0x00000800,
    UNIT_FLAG2_SPELL_CLICK_IN_GROUP   = 0x00001000,
    UNIT_FLAG2_SPELL_CLICK_DISABLED   = 0x00002000,
    UNIT_FLAG2_INTERACT_ANY_REACTION  = 0x00004000,
    UNIT_FLAG2_CANNOT_TURN            = 0x00008000,
    UNIT_FLAG2_UNK16                  = 0x00010000,
    UNIT_FLAG2_PLAY_DEATH_ANIM        = 0x00020000,           // NYI
    UNIT_FLAG2_ALLOW_CHEAT_SPELLS     = 0x00040000,
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
    CleanDamage(uint32 _damage, WeaponAttackType _attackType, MeleeHitOutcome _hitOutCome, bool _takenOrAbsorbedDamage) :
        damage(_damage), attackType(_attackType), hitOutCome(_hitOutCome), takenOrAbsorbedDamage(_takenOrAbsorbedDamage) {}

    uint32 damage; // only used for rage generation
    WeaponAttackType attackType;
    MeleeHitOutcome hitOutCome;
    bool takenOrAbsorbedDamage;
};

struct SubDamageInfo
{
    SpellSchoolMask damageSchoolMask = SPELL_SCHOOL_MASK_NORMAL;
    uint32 damage = 0;
    uint32 absorb = 0;
    int32 resist = 0;
};

// Struct for use in Unit::CalculateMeleeDamage
// Need create structure like in SMSG_ATTACKERSTATEUPDATE opcode
struct CalcDamageInfo
{
    Unit*  attacker;             // Attacker
    Unit*  target;               // Target for damage
    uint32 totalDamage;
    SubDamageInfo subDamage[MAX_ITEM_PROTO_DAMAGES];
    uint32 blocked_amount;
    uint32 HitInfo;
    uint8  TargetState;
    // Helper
    WeaponAttackType attackType; //
    uint32 procAttacker;
    uint32 procVictim;
    uint32 procEx;
    uint32 cleanDamage;          // Used only for rage calculation
    uint32 absorb;
    MeleeHitOutcome hitOutCome;  // TODO: remove this field (need use TargetState)
};

// Spell damage info structure based on structure sending in SMSG_SPELLNONMELEEDAMAGELOG opcode
struct SpellNonMeleeDamage
{
    SpellNonMeleeDamage(WorldObject* _attacker, Unit* _target, uint32 _SpellID, SpellSchoolMask _schoolMask, Spell* spell = nullptr)
        : target(_target), attacker(_attacker), SpellID(_SpellID), damage(0), schoolMask(_schoolMask),
          absorb(0), resist(0), periodicLog(false), unused(false), blocked(0), HitInfo(0), spell(spell)
    {}

    Unit*   target;
    WorldObject* attacker;
    uint32 SpellID;
    uint32 damage;
    SpellSchoolMask schoolMask;
    uint32 absorb;
    int32  resist;
    bool   periodicLog;
    bool   unused;
    uint32 blocked;
    uint32 HitInfo;
    Spell* spell;
};

struct SpellPeriodicAuraLogInfo
{
    SpellPeriodicAuraLogInfo(Aura* _aura, uint32 _damage, uint32 _overDamage, uint32 _absorb, int32 _resist, float _multiplier, bool _critical = false)
        : aura(_aura), damage(_damage), overDamage(_overDamage), absorb(_absorb), resist(_resist), multiplier(_multiplier), critical(_critical) {}

    Aura*   aura;
    uint32 damage;
    uint32 overDamage;                                      // overkill/overheal
    uint32 absorb;
    int32 resist;
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
    SPELL_AURA_PROC_CANT_TRIGGER    = 2,                    // aura can't trigger - skip charges taking, move to next aura if exists
};

// Unit* victim, uint32 procAttacker, uint32 procVictim, uint32 procExtra, uint32 amount, WeaponAttackType attType, SpellEntry const* spellInfo, bool dontTriggerSpecial

// External struct for passing on data
struct ProcSystemArguments
{
    Unit* attacker;
    Unit* victim;

    uint32 procFlagsAttacker;
    uint32 procFlagsVictim;
    uint32 procExtra;

    uint32 damage; // contains full heal or full damage
    uint32 absorb; // absorbed damage
    SpellEntry const* spellInfo;
    WeaponAttackType attType;

    Spell* spell;

    // Healing specific information
    uint32 healthGain;
    bool isHeal;

    explicit ProcSystemArguments(Unit* attacker, Unit* victim, uint32 procFlagsAttacker, uint32 procFlagsVictim, uint32 procExtra, uint32 amount, uint32 absorb, WeaponAttackType attType = BASE_ATTACK,
        SpellEntry const* spellInfo = nullptr, Spell* spell = nullptr, uint32 healthGain = 0, bool isHeal = false) : attacker(attacker), victim(victim), procFlagsAttacker(procFlagsAttacker), procFlagsVictim(procFlagsVictim), procExtra(procExtra), damage(amount),
        absorb(absorb), spellInfo(spellInfo), attType(attType), spell(spell), healthGain(healthGain), isHeal(isHeal)
    {
    }
};

// Internal struct for passing data to execution
struct ProcExecutionData
{
    // these are same for proc on attackers auras and victims auras
    Unit* attacker;
    Unit* victim;

    // these change based on isVictim
    bool isVictim;
    Unit* source;
    Unit* target;

    uint32 procFlags;
    uint32 procExtra;

    WeaponAttackType attType;
    uint32 damage; // contains full heal or full damage
    uint32 absorb; // absorbed damage
    SpellEntry const* spellInfo; // filled always even on aura tick

    Spell* spell; // only filled on direct spell execution

    // Healing specific information
    uint32 healthGain;
    bool isHeal;

    Aura* triggeredByAura;
    uint32 cooldown;

    // Scripting data
    uint32 triggeredSpellId; // used to designate if a proc was overriden if > 0
    std::array<int32, MAX_EFFECT_INDEX> basepoints = { 0, 0, 0 };
    bool procOnce;
    Unit* triggerTarget;
    ObjectGuid triggerOriginalCaster; // not filled by default

    ProcExecutionData(ProcSystemArguments& data, bool isVictim);
};
typedef SpellAuraProcResult(Unit::*pAuraProcHandler)(ProcExecutionData& data);
extern pAuraProcHandler AuraProcHandler[TOTAL_AURAS];

enum CurrentSpellTypes
{
    CURRENT_MELEE_SPELL             = 0,
    CURRENT_GENERIC_SPELL           = 1,
    CURRENT_AUTOREPEAT_SPELL        = 2,
    CURRENT_CHANNELED_SPELL         = 3,
    CURRENT_MAX_SPELL
};

#define CURRENT_FIRST_NON_MELEE_SPELL 1

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

        void SetCharmState(std::string const& ainame, bool withNewThreatList = true);
        void SetCharmState(UnitAI* ai, bool withNewThreatList = true);
        void ResetCharmState();
        uint32 GetPetNumber() const { return m_petnumber; }
        void SetPetNumber(uint32 petnumber, bool statwindow);
        uint32 GetPetLastAttackCommandTime() const { return m_petLastAttackCommandTime; }
        void SetPetLastAttackCommandTime(uint32 time) { m_petLastAttackCommandTime = time; }

        void SetCommandState(CommandStates st);
        CommandStates GetCommandState() const { return m_CommandState; }
        bool HasCommandState(CommandStates state) const { return (m_CommandState == state); }

        void InitVehicleCreateSpells();
        void InitPossessCreateSpells();
        void InitCharmCreateSpells();
        void InitPetActionBar();
        void InitEmptyActionBar();
        void ProcessUnattackableTargets(CombatData* combatData);

        // return true if successful
        bool AddSpellToActionBar(uint32 spellId, ActiveStates newstate = ACT_DECIDE, uint8 forceSlot = 255);
        bool RemoveSpellFromActionBar(uint32 spellId);
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
        bool GetIsRetreating() const { return m_retreating; }

        void ResetStayPosition();
        void SetStayPosition();
        bool UpdateStayPosition();

        float GetStayPosX() const { return m_stayPosX; }
        float GetStayPosY() const { return m_stayPosY; }
        float GetStayPosZ() const { return m_stayPosZ; }
        float GetStayPosO() const { return m_stayPosO; }

        uint32 GetSpellOpener() const { return m_opener; }
        uint32 GetSpellOpenerMinRange() const { return m_openerMinRange; }
        uint32 GetSpellOpenerMaxRange() const { return m_openerMaxRange; }

        void SetSpellOpener(uint32 spellId = 0, uint32 minRange = 0, uint32 maxRange = 0)
        {
            m_opener = spellId;
            m_openerMinRange = minRange;
            m_openerMaxRange = maxRange;
        }

        UnitAI* GetAI() const { return m_ai; }
        CombatData* GetCombatData() const { return m_combatData; };

        void SetCharmStartPosition(Position const& position) { m_charmStartPosition = position; }
        Position const& GetCharmStartPosition() { return m_charmStartPosition; }

        void SetWalk(bool walk) { m_walk = walk; }
        bool GetWalk() const { return m_walk; }

        Unit* GetUnit() { return m_unit; }

    private:
        Unit*               m_unit;
        UnitAI*             m_ai;
        CombatData*         m_combatData;
        UnitActionBarEntry  PetActionBar[MAX_UNIT_ACTION_BAR_INDEX];
        CharmSpellEntry     m_charmspells[CREATURE_MAX_SPELLS];
        CommandStates       m_CommandState;
        uint32              m_petnumber;
        uint32              m_petLastAttackCommandTime;
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

        Position            m_charmStartPosition;
        bool                m_walk;

        bool                m_deleted;
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
#define REGEN_TIME_FULL_UNIT 5000                           // For npcs
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

// Selection method used by SelectAttackingTarget
enum AttackingTarget
{
    ATTACKING_TARGET_RANDOM = 0,                            // Just selects a random target
    ATTACKING_TARGET_TOPAGGRO,                              // Selects targes from top aggro to bottom
    ATTACKING_TARGET_BOTTOMAGGRO,                           // Selects targets from bottom aggro to top
    ATTACKING_TARGET_NEAREST_BY,                            // Selects the nearest by target
    ATTACKING_TARGET_FARTHEST_AWAY,                         // Selects the farthest away target
    ATTACKING_TARGET_ALL_SUITABLE,
};

enum SelectFlags
{
    SELECT_FLAG_IN_LOS              = 0x0001,               // Default Selection Requirement for Spell-targets
    SELECT_FLAG_PLAYER              = 0x0002,
    SELECT_FLAG_POWER_MANA          = 0x0004,               // For Energy based spells, like manaburn
    SELECT_FLAG_POWER_RAGE          = 0x0008,
    SELECT_FLAG_POWER_ENERGY        = 0x0010,
    SELECT_FLAG_IN_MELEE_RANGE      = 0x0040,
    SELECT_FLAG_NOT_IN_MELEE_RANGE  = 0x0080,
    SELECT_FLAG_HAS_AURA            = 0x0100,
    SELECT_FLAG_NOT_AURA            = 0x0200,
    SELECT_FLAG_RANGE_RANGE         = 0x0400,               // For direct targeted abilities like charge or frostbolt but need custom data
    SELECT_FLAG_RANGE_AOE_RANGE     = 0x0800,               // For AOE targeted abilities like frost nova but need custom data
    SELECT_FLAG_POWER_NOT_MANA      = 0x1000,               // Used in some dungeon encounters
    SELECT_FLAG_USE_EFFECT_RADIUS   = 0x2000,               // For AOE targeted abilities which have correct data in effect index 0
    SELECT_FLAG_SKIP_TANK           = 0x4000,               // Not GetVictim - tank is not always top threat
    SELECT_FLAG_CASTING             = 0x8000,               // Selects only targets that are casting
    SELECT_FLAG_SKIP_CUSTOM         =0x10000,               // skips custom target
    SELECT_FLAG_NOT_IMMUNE          =0x20000,
    SELECT_FLAG_USE_EFFECT_RADIUS_OF_TRIGGERED_SPELL = 0x40000 // For AOE targeted abilities which have correct data in triggered spell effect index 0
};

struct SelectAttackingTargetParams
{
    union
    {
        struct
        {
            uint32 minRange;
            uint32 maxRange;
        } range;
        struct
        {
            uint64 guid;
        } skip;
        struct
        {
            uint32 params[2];
        } raw;
    };
};

struct SpellProcEventEntry;                                 // used only privately

extern float baseMoveSpeed[MAX_MOVE_TYPE];

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

        float CalculateCollisionHeight(uint32 mountId) const;
        float GetCollisionHeight() const override;
        float GetCollisionWidth() const override;
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
        void IncrDiminishing(DiminishingGroup group, bool pvp);
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
        void ApplyDiminishingToDuration(DiminishingGroup group, int32& duration, Unit* caster, DiminishingLevels Level, bool isReflected, SpellEntry const* spellInfo, bool hasAuraScript);
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

        void Update(const uint32 diff) override;
        void Heartbeat() override;

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
         * Checks if the current Unit has a weapon equipped at the moment
         * @return True if there is a weapon.
         */
        virtual bool hasWeapon(WeaponAttackType type) const = 0;
        inline bool hasMainhandWeapon() const { return hasWeapon(BASE_ATTACK); }
        inline bool hasOffhandWeapon() const { return hasWeapon(OFF_ATTACK); }
        inline bool hasRangedWeapon() const { return hasWeapon(RANGED_ATTACK); }
        /**
         * Checks if the current Unit has a usable weapon at the moment
         * @return True if there is a weapon.
         */
        virtual bool hasWeaponForAttack(WeaponAttackType type) const { return CanUseEquippedWeapon(type); }
        inline bool hasMainhandWeaponForAttack() const { return hasWeaponForAttack(BASE_ATTACK); }
        inline bool hasOffhandWeaponForAttack() const { return hasWeaponForAttack(OFF_ATTACK); }
        inline bool hasRangedWeaponForAttack() const { return hasWeaponForAttack(RANGED_ATTACK); }
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
            if (IsAttackSpeedOverridenShapeShift())
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
        ObjectGuid m_extraAttackGuid;
        void DoExtraAttacks(Unit* victim);

        bool IsAttackedBy(Unit* attacker) const
        {
            if (m_attackers.find(attacker) != m_attackers.end())
                return true;

            return false;
        }

        bool _addAttacker(Unit* pAttacker)                  //< (Internal Use) must be called only from Unit::Attack(Unit*)
        {
            AttackerSet::const_iterator itr = m_attackers.find(pAttacker);
            if (itr == m_attackers.end())
            {
                m_attackers.insert(pAttacker);
                return true;
            }
            return false;
        }
        void _removeAttacker(Unit* pAttacker)               //< (Internal Use) must be called only from Unit::AttackStop()
        {
            m_attackers.erase(pAttacker);
        }
        Unit* getAttackerForHelper()                        //< Return a possible enemy from this unit to help in combat
        {
            if (GetVictim() != nullptr)
                return GetVictim();

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
        bool AttackStop(bool targetSwitch = false, bool includingCast = false, bool includingCombo = false, bool clientInitiated = false);
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

        Unit* GetVictim() const { return m_attacking; }     //< Returns the victim that this unit is currently attacking
        void CombatStop(bool includingCast = false, bool includingCombo = true);        //< Stop this unit from combat, if includingCast==true, also interrupt casting
        void CombatStopWithPets(bool includingCast = false, bool includingCombo = true);
        void StopAttackFaction(uint32 faction_id);
        Unit* SelectRandomUnfriendlyTarget(Unit* except = nullptr, float radius = ATTACK_DISTANCE) const;
        Unit* SelectRandomFriendlyTarget(Unit* except = nullptr, float radius = ATTACK_DISTANCE) const;
        bool HasDamageInterruptibleStunAura() const;
        bool HasBreakableByDamageCrowdControlAura(Unit* excludeCasterChannel = nullptr, uint32 excludeAuraApplyMSTimeCutoff = 0) const;
        bool HasBreakableByDamageAuraType(AuraType type, uint32 excludeAura, uint32 excludeAuraApplyMSTimeCutoff) const;
        bool HasAuraPetShouldAvoidBreaking(Unit* excludeCasterChannel = nullptr, uint32 excludeAuraApplyMSTimeCutoff = 0) const;
        void SendMeleeAttackStop(const Unit& victim) const;
        void SendMeleeAttackStart(const Unit& victim) const;

        void addUnitState(uint32 f) { m_state |= f; }
        bool hasUnitState(uint32 f) const { return (m_state & f) != 0; }
        void clearUnitState(uint32 f) { m_state &= ~f; }
        bool CanFreeMove() const { return !hasUnitState(UNIT_STAT_NO_FREE_MOVE) && !GetOwnerGuid(); }

        virtual uint32 GetLevelForTarget(Unit const* /*target*/) const { return GetLevel(); }
        bool IsTrivialForTarget(Unit const* pov) const;

        void SetLevel(uint32 lvl);

        uint32 GetLevel() const override { return GetUInt32Value(UNIT_FIELD_LEVEL); }
        virtual uint8 getRace() const { return GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE); }
        uint32 getRaceMask() const { return getRace() ? 1 << (getRace() - 1) : 0; }
        uint8 getClass() const { return GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS); }
        uint32 getClassMask() const { return 1 << (getClass() - 1); }
        uint8 getGender() const { return GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER); }

        float GetStat(Stats stat) const { return float(GetUInt32Value(UNIT_FIELD_STAT0 + stat)); }
        void SetStat(Stats stat, int32 val) { SetStatInt32Value(UNIT_FIELD_STAT0 + stat, val); }

        inline int32 GetArmor() const { return GetResistance(SPELL_SCHOOL_NORMAL) ; }
        inline void SetArmor(int32 val) { SetStatInt32Value(UNIT_FIELD_RESISTANCES, val); }

        inline int32 GetResistance(SpellSchools school) const { return GetInt32Value(UNIT_FIELD_RESISTANCES + school); }
        inline void SetResistance(SpellSchools school, int32 val) { SetInt32Value(UNIT_FIELD_RESISTANCES + school, val); }

        uint32 GetHealth()    const { return GetUInt32Value(UNIT_FIELD_HEALTH); }
        float GetRealHealth() const { return m_unitHealth; }
        uint32 GetMaxHealth() const { return GetUInt32Value(UNIT_FIELD_MAXHEALTH); }
        float GetHealthPercent() const { return (GetHealth() * 100.0f) / GetMaxHealth(); }
        void SetHealth(float val);
        void SetMaxHealth(uint32 val);
        void SetHealthPercent(float percent);
        float ModifyHealth(float dVal);
        float OCTRegenHPPerSpirit() const;
        float OCTRegenMPPerSpirit() const;

        Powers GetPowerType() const { return Powers(GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE)); }
        void SetPowerType(Powers new_powertype, bool sendUpdate = true);
        uint32 GetPower(Powers power) const { return GetUInt32Value(UNIT_FIELD_POWER1 + power); }
        float GetRealPower(Powers power) const { return m_unitPower[power]; }
        uint32 GetMaxPower(Powers power) const { return GetUInt32Value(UNIT_FIELD_MAXPOWER1 + power); }
        float GetPowerPercent() const { return (GetMaxPower(GetPowerType()) == 0) ? 0.0f : (GetPower(GetPowerType()) * 100.0f) / GetMaxPower(GetPowerType()); }
        float GetPowerPercent(Powers power) const { return (GetMaxPower(power) == 0) ? 0.0f : (GetPower(power) * 100.0f) / GetMaxPower(power); }
        void SetPower(Powers power, float val, bool withPowerUpdate = true);
        void SetMaxPower(Powers power, uint32 val);
        int32 ModifyPower(Powers power, int32 dVal);
        [[deprecated("Use ModifyPower()")]]
        void ApplyPowerMod(Powers power, uint32 val, bool apply);
        void ApplyMaxPowerMod(Powers power, uint32 val, bool apply);
        bool HasMana() { return GetPowerType() == POWER_MANA; }

        uint32 GetAttackTime(WeaponAttackType att) const { return (uint32)(GetFloatValue(UNIT_FIELD_BASEATTACKTIME + att) / m_modAttackSpeedPct[att]); }
        void SetAttackTime(WeaponAttackType att, uint32 val) { SetFloatValue(UNIT_FIELD_BASEATTACKTIME + att, val * m_modAttackSpeedPct[att]); }
        void ApplyAttackTimePercentMod(WeaponAttackType att, float val, bool apply);
        void ApplyCastTimePercentMod(float val, bool apply);

        SheathState GetSheath() const { return SheathState(GetByteValue(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_SHEATH_STATE)); }
        virtual void SetSheath(SheathState sheathed) { SetByteValue(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_SHEATH_STATE, sheathed); }

        // faction template id
        uint32 GetFaction() const override { return GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE); }
        void setFaction(uint32 faction) { SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, faction); }
        FactionTemplateEntry const* GetFactionTemplateEntry() const;
        void RestoreOriginalFaction();
        bool IsNeutralToAll() const;
        bool IsContestedGuard() const
        {
            if (FactionTemplateEntry const* entry = GetFactionTemplateEntry())
                return entry->IsContestedGuardFaction();

            return false;
        }

        Player const* GetControllingPlayer(bool ignoreCharms = false) const;

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

        // Serverside fog of war settings
        bool IsFogOfWarVisibleStealth(Unit const* other) const;
        bool IsFogOfWarVisibleHealth(Unit const* other) const;
        bool IsFogOfWarVisibleStats(Unit const* other) const;

        virtual bool IsInGroup(Unit const* other, bool party = false, bool ignoreCharms = false) const;
        inline bool IsInParty(Unit const* other, bool ignoreCharms = false) const { return IsInGroup(other, true, ignoreCharms); }
        bool IsInGuild(Unit const* other, bool ignoreCharms = false) const;
        bool IsInTeam(Unit const* other, bool ignoreCharms = true) const;

        // extensions of CanAttack and CanAssist API needed serverside
        virtual bool CanAttackSpell(Unit const* target, SpellEntry const* spellInfo = nullptr, bool isAOE = false) const override;
        virtual bool CanAssistSpell(Unit const* target, SpellEntry const* spellInfo = nullptr) const override;

        bool CanAttackOnSight(Unit const* target) const; // Used in MoveInLineOfSight checks
        bool CanAttackInCombat(Unit const* target, bool ignoreFlagsSource, bool ignoreFlagsTarget, bool ignoreUntargetable) const;
        bool CanAttackServerside(Unit const* unit, bool ignoreFlagsSource, bool ignoreFlagsTarget, bool ignoreUntargetable) const;
        bool CanAssistInCombatAgainst(Unit const* who, Unit const* enemy) const;
        bool CanJoinInAttacking(Unit const* enemy) const;

        bool IsImmuneToNPC() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC); }
        void SetImmuneToNPC(bool state);
        bool IsImmuneToPlayer() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER); }
        void SetImmuneToPlayer(bool state);
        bool IsPlayerControlled() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); }
        uint32 GetCreatedBySpellId() const { return GetUInt32Value(UNIT_CREATED_BY_SPELL); }

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

        uint8 getStandState() const { return GetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_STAND_STATE); }
        bool IsSitState() const;
        bool IsStandState() const;
        bool IsSeatedState() const;
        bool IsStandUpOnMovementState() const;
        void SetStandState(uint8 state, bool acknowledge = false);

        void SetVisFlags(uint8 flags) { SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_VIS_FLAGS, flags); }
        void RemoveVisFlags(uint8 flags) { RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_VIS_FLAGS, flags); }
        bool HasVisFlags(uint8 flags) { return GetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_VIS_FLAGS) & flags; }

        bool IsMounted() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT); } // not used with creature non-aura mounts
        uint32 GetMountID() const { return GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID); }
        bool MountEntry(uint32 templateEntry, const Aura* aura = nullptr);
        bool UnmountEntry(const Aura* aura = nullptr);
        virtual bool Mount(uint32 displayid, bool auraExists = false, int32 auraAmount = 0, bool isFlyingAura = false, bool pendingTaxi = false);
        virtual bool Unmount(bool auraExists = false, int32 auraAmount = 0, bool isFlyingAura = false);

        VehicleInfo* GetVehicleInfo() const { return m_vehicleInfo.get(); }
        MaNGOS::unique_weak_ptr<VehicleInfo> GetVehicleInfoWeakPtr() const { return m_vehicleInfo; }
        bool IsVehicle() const { return m_vehicleInfo != nullptr; }
        void SetVehicleId(uint32 entry, uint32 overwriteNpcEntry);
        Unit const* FindRootVehicle(const Unit* whichVehicle = nullptr) const;

        uint16 GetSkillMaxForLevel(Unit const* target = nullptr) const { return (target ? GetLevelForTarget(target) : GetLevel()) * 5; }

        void Suicide();
        void FallSuicide();
        static void DealDamageMods(Unit* dealer, Unit* victim, uint32& damage, uint32* absorb, DamageEffectType damagetype, SpellEntry const* spellProto = nullptr);
        static uint32 DealDamage(Unit* dealer, Unit* victim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellEntry const* spellProto, bool durabilityLoss, Spell* spell = nullptr);
        int32 DealHeal(Unit* pVictim, uint32 addhealth, SpellEntry const* spellProto, bool critical = false, uint32 absorb = 0);
        static void Kill(Unit* killer, Unit* victim, DamageEffectType damagetype, SpellEntry const* spellProto, bool durabilityLoss, bool duel_hasEnded);
        static void HandleDamageDealt(Unit* dealer, Unit* victim, uint32& damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellEntry const* spellProto, bool duel_hasEnded);
        void InterruptOrDelaySpell(Unit* pVictim, DamageEffectType damagetype, SpellEntry const* spellProto);

        void PetOwnerKilledUnit(Unit* pVictim);

        static void ProcDamageAndSpell(ProcSystemArguments&& data);
        void ProcDamageAndSpellFor(ProcSystemArguments& data, bool isVictim);
        void ProcSkillsAndReactives(bool isVictim, Unit* target, uint32 procFlags, uint32 procEx, WeaponAttackType attType);

        void HandleEmote(uint32 emote_id);                  // auto-select command/state
        void HandleEmoteCommand(uint32 emote_id);
        void HandleEmoteState(uint32 emote_id);
        void AttackerStateUpdate(Unit* pVictim, WeaponAttackType attType = BASE_ATTACK, bool extra = false);

        void CalculateMeleeDamage(Unit* pVictim, CalcDamageInfo* calcDamageInfo, WeaponAttackType attackType = BASE_ATTACK);
        void DealMeleeDamage(CalcDamageInfo* calcDamageInfo, bool durabilityLoss);

        static bool IsAllowedDamageInArea(Unit* attacker, Unit* victim);

        void CalculateSpellDamage(SpellNonMeleeDamage* spellDamageInfo, int32 damage, SpellEntry const* spellInfo, SpellEffectIndex effectIndex, WeaponAttackType attackType = BASE_ATTACK);
        static void DealSpellDamage(Unit* affectiveCaster, SpellNonMeleeDamage* spellDamageInfo, bool durabilityLoss, bool resetLeash);

        uint32 GetResilienceRatingDamageReduction(uint32 damage, SpellDmgClass dmgClass, bool periodic = false, Powers pwrType = POWER_HEALTH) const;

        SpellMissInfo MeleeSpellHitResult(Unit* pVictim, SpellEntry const* spell, uint32* heartbeatResistChance = nullptr);
        SpellMissInfo MagicSpellHitResult(Unit* pVictim, SpellEntry const* spell, SpellSchoolMask schoolMask, uint32* heartbeatResistChance = nullptr);
        static SpellMissInfo SpellHitResult(WorldObject* caster, Unit* victim, SpellEntry const* spell, uint8 effectMask, bool reflectable = false, bool reflected = false, uint32* heartbeatResistChance = nullptr);

        bool CanDualWield() const { return m_canDualWield; }
        virtual void SetCanDualWield(bool value) { m_canDualWield = value; }

        // Unit Combat reactions API: Dodge/Parry/Block
        bool CanDodge() const { return m_canDodge; }
        bool CanParry() const { return m_canParry; }
        bool CanBlock() const { return m_canBlock; }
        // Unit Melee events API: Crush/Glance/Daze
        bool CanCrush() const;
        bool CanGlance() const;
        virtual bool CanDaze() const { return false; };

        void SetCanDodge(const bool flag);
        void SetCanParry(const bool flag);
        void SetCanBlock(const bool flag);

        bool CanReactInCombat() const { return (IsAlive() && !IsCrowdControlled() && !GetCombatManager().IsEvadingHome()); }
        bool CanCastSpellInCombat() const { return (IsAlive() && !GetCombatManager().IsEvadingHome()); } // spells are stopped by spell system
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

        virtual float GetCritChance(WeaponAttackType attType) const;
        virtual float GetCritChance(SpellSchoolMask schoolMask) const;
        virtual float GetCritChance(const SpellEntry* entry, SpellSchoolMask schoolMask) const;

        virtual float GetCritTakenChance(Unit const* attacker, SpellSchoolMask dmgSchoolMask, SpellDmgClass dmgClass, SpellEntry const* spellInfo = nullptr, bool heal = false) const;

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

        /*Hack to support always hitting creatures. TODO: investigate Serpentshrine Parasite*/
        void SetAlwaysHit(bool value) { m_alwaysHit = value; }

        static bool RollSpellCritOutcome(Unit* caster, const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell);

        float GetExpertisePercent(WeaponAttackType attType) const;

        virtual int32 GetResistancePenetration(SpellSchools school) const;

        float CalculateEffectiveMagicResistancePercent(const Unit* attacker, SpellSchoolMask schoolMask, bool binary = false) const;
        static float RollMagicResistanceMultiplierOutcomeAgainst(const Unit* attacker, const Unit* victim, SpellSchoolMask schoolMask, DamageEffectType dmgType, bool binary = false);

        float CalculateSpellResistChance(const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell) const;

        virtual uint32 GetShieldBlockValue() const = 0;
        bool IsBlockCritical() const;
        uint32 GetUnitMeleeSkill(Unit const* target = nullptr) const { return (target ? GetLevelForTarget(target) : GetLevel()) * 5; }
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

        bool IsInCombat() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT); }
        // unidirectional API - only use when absolutely necessary
        void SetInCombatState(bool PvP, Unit* enemy = nullptr);
        void SetInCombatWith(Unit* enemy);
        void SetInCombatWithAggressor(Unit* aggressor, bool touchOnly = false);
        inline void SetOutOfCombatWithAggressor(Unit* aggressor) { SetInCombatWithAggressor(aggressor, true); }
        void SetInCombatWithAssisted(Unit* assisted, bool touchOnly = false);
        inline void SetOutOfCombatWithAssisted(Unit* assisted) { SetInCombatWithAssisted(assisted, true); }
        void SetInCombatWithVictim(Unit* victim, bool touchOnly = false);
        inline void SetOutOfCombatWithVictim(Unit* victim) { SetInCombatWithVictim(victim, true); }
        // bidirectional api - we need info about who is the attacker - handles leashing
        void EngageInCombatWith(Unit* enemy);
        void EngageInCombatWithAggressor(Unit* aggressor);
        void ClearInCombat();
        void HandleExitCombat(bool pvpCombat = false);
        virtual uint32 GetPursuit() const { return 15000; }

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
        bool HasAuraTypeWithCaster(AuraType auratype, ObjectGuid caster) const;
        bool HasPeriodicAura() const;
        bool HasMechanicMaskOrDispelMaskAura(uint32 dispelMask, uint32 mechanicMask, Unit const* caster) const;
        bool HasNegativeAuraWithInterruptFlag(SpellAuraInterruptFlags flag) const;
        template<typename Func>
        bool HasAuraHolder(Func func) const
        {
            auto const& Auras = GetSpellAuraHolderMap();
            for (auto itr = Auras.begin(); itr != Auras.end(); ++itr)
            {
                SpellAuraHolder* holder = itr->second;
                if (func(holder))
                    return true;
            }
            return false;
        }

        template<typename Func>
        bool HasAura(Func func, AuraType type) const
        {
            auto const& Auras = GetAurasByType(type);
            for (auto itr = Auras.begin(); itr != Auras.end(); ++itr)
            {
                if (func(*itr))
                    return true;
            }
            return false;
        }
        bool HasAuraTypeWithMiscvalue(AuraType auraType, int32 miscvalue) const;

        virtual bool HasSpell(uint32 /*spellID*/) const { return false; }

        bool HasStealthAura()      const { return HasAuraType(SPELL_AURA_MOD_STEALTH); }
        bool HasInvisibilityAura() const { return HasAuraType(SPELL_AURA_MOD_INVISIBILITY); }
        bool isFeared()  const { return HasAuraType(SPELL_AURA_MOD_FEAR); }
        bool isInRoots() const { return HasAuraType(SPELL_AURA_MOD_ROOT); }
        bool IsPolymorphed() const;

        bool isFrozen() const;
        bool IsIgnoreUnitState(SpellEntry const* spell, IgnoreUnitState ignoreState) const;

        virtual bool IsInWater() const;
        bool IsInSwimmableWater() const;
        virtual bool IsUnderwater() const;
        bool IsAboveGround(float diff = 0.5f) const;
        bool isInAccessablePlaceFor(Unit const* unit) const;

        void EnergizeBySpell(Unit* victim, SpellEntry const* spellInfo, uint32 damage, Powers powerType, bool sendLog = true);
        uint32 SpellNonMeleeDamageLog(Unit* pVictim, uint32 spellID, uint32 damage);
        SpellCastResult CastSpell(Unit* Victim, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(Unit* Victim, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastCustomSpell(Unit* Victim, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastCustomSpell(Unit* Victim, SpellEntry const* spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(float x, float y, float z, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(float x, float y, float z, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(SpellCastTargets& targets, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastCustomSpell(SpellCastTargets& targets, SpellEntry const* spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(SpellCastArgs& args, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        SpellCastResult CastSpell(SpellCastArgs& args, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);
        
        // Single flag overload uint32
        SpellCastResult CastSpell(Unit* Victim, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        { return CastSpell(Victim, spellId, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy); }
        SpellCastResult CastCustomSpell(Unit* Victim, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastCustomSpell(Victim, spellId, bp0, bp1, bp2, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(float x, float y, float z, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(x, y, z, spellId, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(SpellCastArgs& args, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);

        // Single flag overload SpellEntry
        SpellCastResult CastSpell(Unit* Victim, SpellEntry const* spellInfo, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        { return CastSpell(Victim, spellInfo, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy); }
        SpellCastResult CastCustomSpell(Unit* Victim, SpellEntry const* spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastCustomSpell(Victim, spellInfo, bp0, bp1, bp2, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(float x, float y, float z, SpellEntry const* spellInfo, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(x, y, z, spellInfo, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }
        SpellCastResult CastSpell(SpellCastArgs& args, SpellEntry const* spellInfo, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);

        void DeMorph();

        void SendAIReaction(AiReaction reactionType);

        void SendAttackStateUpdate(CalcDamageInfo* calcDamageInfo) const;
        void SendAttackStateUpdate(uint32 HitInfo, Unit* target, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, int32 Resist, VictimState TargetState, uint32 BlockedAmount);
        void SendEnergizeSpellLog(Unit* pVictim, uint32 SpellID, uint32 Damage, Powers powertype) const;
        void SendEnvironmentalDamageLog(uint8 type, uint32 damage, uint32 absorb, int32 resist) const;
        void SendHealSpellLog(Unit* pVictim, uint32 SpellID, uint32 Damage, uint32 OverHeal, bool critical = false, uint32 absorb = 0) const;
        static void SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log);
        static void SendSpellNonMeleeDamageLog(WorldObject* attacker, Unit* target, uint32 spellID, uint32 damage, SpellSchoolMask damageSchoolMask, uint32 absorbedDamage, int32 resist, bool isPeriodic, uint32 blocked, bool criticalHit = false, bool split = false);
        void SendPeriodicAuraLog(SpellPeriodicAuraLogInfo* pInfo) const;
        static void SendSpellMiss(WorldObject* caster, Unit* target, uint32 spellID, SpellMissInfo missInfo);
        void SendSpellDamageResist(Unit* target, uint32 spellId) const;
        static void SendSpellOrDamageImmune(ObjectGuid casterGuid, Unit* target, uint32 spellId);

        void SendEnchantmentLog(ObjectGuid targetGuid, uint32 itemEntry, uint32 enchantId) const;

        static void CasterHitTargetWithSpell(Unit* realCaster, Unit* target, SpellEntry const* spellInfo, bool triggered, bool success = true);
        bool CanInitiateAttack() const;

        Unit* SelectAttackingTarget(AttackingTarget target, uint32 position, uint32 spellId, uint32 selectFlags = 0, SelectAttackingTargetParams params = SelectAttackingTargetParams(), int32 unitConditionId = 0) const;
        Unit* SelectAttackingTarget(AttackingTarget target, uint32 position, SpellEntry const* spellInfo = nullptr, uint32 selectFlags = 0, SelectAttackingTargetParams params = SelectAttackingTargetParams(), int32 unitConditionId = 0) const;
        void SelectAttackingTargets(std::vector<Unit*>& selectedTargets, AttackingTarget target, uint32 position, uint32 spellId, uint32 selectFlags = 0, SelectAttackingTargetParams params = SelectAttackingTargetParams(), int32 unitConditionId = 0) const;
        void SelectAttackingTargets(std::vector<Unit*>& selectedTargets, AttackingTarget target, uint32 position, SpellEntry const* spellInfo = nullptr, uint32 selectFlags = 0, SelectAttackingTargetParams params = SelectAttackingTargetParams(), int32 unitConditionId = 0) const;

        // grid searcher include hiding
        bool IsUnitConditionSatisfied(int32 unitConditionId, Unit const* otherUnit) const;

        void NearTeleportTo(float x, float y, float z, float orientation, bool casting = false, bool transportLeave = false);
        // do not use - kept only for cinematics
        void MonsterMoveWithSpeed(float x, float y, float z, float speed, bool generatePath = false, bool forceDestination = false);

        // recommend use MonsterMove/MonsterMoveWithSpeed for most case that correctly work with movegens
        // if used additional args in ... part then floats must explicitly casted to double
        void SendTeleportPacket(float x, float y, float z, float ori, GenericTransport* transport);
        void SendHeartBeat();

        void SendMoveRoot(bool state, bool broadcastOnly = false);

        bool IsMoving() const { return m_movementInfo.HasMovementFlag(movementFlagsMask); }
        bool IsMovingIgnoreFlying() const { return m_movementInfo.HasMovementFlag(movementFlagsIgnoreFlyingMask); }
        bool IsMovingForward() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_MASK_MOVING_FORWARD); }
        bool IsLevitating() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_LEVITATING); }
        bool IsHovering() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_HOVER); }
        bool IsWalking() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_WALK_MODE); }
        bool IsRooted() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_ROOT); }
        bool IsFalling() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING); }

        enum class MmapForcingStatus
        {
            FORCED,
            DEFAULT,
            IGNORED
        };

        virtual MmapForcingStatus IsIgnoringMMAP() const;

        bool IsDebuggingMovement() const { return m_debuggingMovement; }
        void SetDebuggingMovement(bool state) { m_debuggingMovement = state; }

        void SetLevitate(bool enabled);
        void SetSwim(bool enabled);
        void SetCanFly(bool enabled);
        void SetFeatherFall(bool enabled);
        void SetHover(bool enabled);
        void SetWaterWalk(bool enabled);

        void SetInFront(Unit const* target);
        void SetFacingTo(float ori);
        void SetFacingToObject(WorldObject* pObject);

        void SendHighestThreatUpdate(HostileReference* pHostilReference);
        void SendThreatClear() const;
        void SendThreatRemove(HostileReference* pHostileReference) const;
        void SendThreatUpdate();

        bool IsAlive() const { return (m_deathState == ALIVE); };
        bool IsDead() const { return (m_deathState == DEAD || m_deathState == CORPSE); };
        DeathState GetDeathState() const { return m_deathState; };
        virtual void SetDeathState(DeathState s);           // overwritten in Creature/Player/Pet

        bool IsTargetUnderControl(Unit const& target) const;

        // Convenience checkers/getters/setters counterparts for some of the protected Unit guid fields
        // See the comments next to protected methods for meanings
        bool HasCharm(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? GetCharmGuid().IsUnit() : (GetCharmGuid() == exactGuid)); }
        bool HasCharmer(ObjectGuid const& exactGuid) const { return exactGuid.IsUnit() && GetCharmerGuid().IsUnit() && GetCharmerGuid() == exactGuid; }
        bool HasCharmer() const { return GetCharmerGuid().IsUnit(); }
        bool HasMaster() const;
        bool HasTarget(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? GetTargetGuid().IsUnit() : (GetTargetGuid() == exactGuid)); }
        bool HasChannelObject(ObjectGuid const& exactGuid = ObjectGuid()) const { return (exactGuid.IsEmpty() ? !(GetChannelObjectGuid().IsEmpty()) : (GetChannelObjectGuid() == exactGuid)); }

        Unit* GetCharm(WorldObject const* pov = nullptr) const;
        Unit* GetCharmer(WorldObject const* pov = nullptr) const;
        Unit* GetCreator(WorldObject const* pov = nullptr) const;
        Unit* GetTarget(WorldObject const* pov = nullptr) const;
        WorldObject* GetChannelObject(WorldObject const* pov = nullptr) const;

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

        // Convenience unit getters for some of the logical guid constructs above
        Unit* GetOwner(WorldObject const* pov = nullptr, bool recursive = false) const;
        Unit* GetMaster(WorldObject const* pov = nullptr) const;
        Unit* GetSpawner(WorldObject const* pov = nullptr) const;

        // Additional related server-side and client-side ownership-related methods
        // Beneficiary: owner of the xp/loot/etc credit, master or self (server-side)
        Unit* GetBeneficiary() const;
        Player* GetBeneficiaryPlayer() const;

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
        void RemoveGuardians(bool force = true); // do not remove guardians in combat unless forced
        Pet* FindGuardianWithEntry(uint32 entry);
        uint32 CountGuardiansWithEntry(uint32 entry);
        Pet* GetProtectorPet();                             // expected single case in guardian list

        bool HasAnyPet() const;

        CharmInfo* GetCharmInfo() const { return m_charmInfo; }
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
        void RemoveAura(Aura* Aur, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveSpellAuraHolder(SpellAuraHolder* holder, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveSingleAuraFromSpellAuraHolder(SpellAuraHolder* holder, SpellEffectIndex index, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveSingleAuraFromSpellAuraHolder(uint32 spellId, SpellEffectIndex effindex, ObjectGuid casterGuid, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);

        // removing specific aura stacks by diff reasons and selections
        void RemoveAurasDueToSpell(uint32 spellId, SpellAuraHolder* except = nullptr, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId);
        void RemoveAurasByCasterSpell(uint32 spellId, ObjectGuid casterGuid, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAurasDueToSpellBySteal(SpellAuraHolder* holder, Unit* stealer);
        void RemoveAurasDueToSpellByCancel(uint32 spellId);
        void RemoveAurasTriggeredBySpell(uint32 spellId, ObjectGuid casterGuid = ObjectGuid());
        void RemoveAuraStack(uint32 spellId, int32 modifier = -1);
        void RemoveAuraCharge(uint32 spellId);

        // removing unknown aura stacks by diff reasons and selections
        void RemoveNotOwnTrackedTargetAuras(uint32 newPhase = 0x0);
        void RemoveAurasAtMechanicImmunity(uint32 mechMask, uint32 exceptSpellId, bool non_positive = false);
        void RemoveSpellsCausingAura(AuraType auraType);
        void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except);
        void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except, bool onlyMechanic);
        void RemoveSpellsCausingAura(AuraType auraType, ObjectGuid casterGuid);
        void RemoveRankAurasDueToSpell(uint32 spellId);
        bool RemoveNoStackAurasDueToAuraHolder(SpellAuraHolder* holder);
        void RemoveAurasWithInterruptFlags(uint32 flags);
        void RemoveAurasWithInterruptFlags(uint32 flags, SpellAuraHolder* except);
        void RemoveAurasWithDispelType(DispelType type, ObjectGuid casterGuid = ObjectGuid());
        void RemoveAllAuras(AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveArenaAuras(bool onleave = false);
        void RemoveAllAurasOnDeath();
        void RemoveAllAurasOnEvade();
        void RemoveAllGroupBuffsFromCaster(ObjectGuid casterGuid);
        // wotlk only
        template <class T>
        void RemoveAurasWithAttribute(T flags);

        // remove specific aura on cast
        void RemoveAurasOnCast(uint32 flag, SpellEntry const* castedSpellEntry);

        // removing specific aura FROM stack by diff reasons and selections
        void RemoveAuraHolderFromStack(uint32 spellId, uint32 stackAmount = 1, ObjectGuid casterGuid = ObjectGuid(), AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
        void RemoveAuraHolderDueToSpellByDispel(uint32 spellId, uint32 dispellingSpellId, uint32 stackAmount, ObjectGuid casterGuid, Unit* dispeller);

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
        void SetCreateResistance(SpellSchools school, int32 val) { m_createResistances[school] = val; }
        int32 GetCreateResistance(SpellSchools school) const { return m_createResistances[school]; }

        void SetCurrentCastedSpell(Spell* pSpell);
        virtual void ProhibitSpellSchool(SpellSchoolMask /*idSchoolMask*/, uint32 /*unTimeMs*/) { }
        void InterruptSpell(CurrentSpellTypes spellType, bool withDelayed = true, bool sendAutoRepeatCancelToClient = true);
        void FinishSpell(CurrentSpellTypes spellType, bool ok = true);

        // set withDelayed to true to account delayed spells as casted
        // delayed+channeled spells are always accounted as casted
        // we can skip channeled or delayed checks using flags
        bool IsNonMeleeSpellCasted(bool withDelayed, bool skipChanneled = false, bool skipAutorepeat = false, bool forMovement = false, bool forAutoIgnore = false, bool forEquip = false) const;
        bool IsDelayCombatTimerSpellCasted() const;

        // set withDelayed to true to interrupt delayed spells too
        // delayed+channeled spells are always interrupted
        void InterruptNonMeleeSpells(bool withDelayed, uint32 spell_id = 0);
        void InterruptSpellsWithChannelFlags(uint32 flags);
        void InterruptSpellsAndAurasWithInterruptFlags(uint32 flags);
        bool IsInterruptible() const;

        Spell* GetCurrentSpell(CurrentSpellTypes spellType) const { return m_currentSpells[spellType]; }
        Spell* FindCurrentSpellBySpellId(uint32 spell_id) const;

        ObjectGuid m_ObjectSlotGuid[4];

        ShapeshiftForm GetShapeshiftForm() const { return ShapeshiftForm(GetByteValue(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_SHAPESHIFT)); }
        void  SetShapeshiftForm(ShapeshiftForm form) { SetByteValue(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_SHAPESHIFT, form); }

        bool IsShapeShifted() const;
        bool IsNoWeaponShapeShift() const;
        bool IsAttackSpeedOverridenShapeShift() const;

        bool IsInDisallowedMountForm() const
        {
            ShapeshiftForm form = GetShapeshiftForm();
            return form != FORM_NONE && form != FORM_BATTLESTANCE && form != FORM_BERSERKERSTANCE && form != FORM_DEFENSIVESTANCE &&
                   form != FORM_SHADOW && form != FORM_STEALTH;
        }

        float m_unitHealth;
        float m_unitPower[POWER_RUNIC_POWER + 1];

        float m_modWeaponHitChance[MAX_ATTACK];
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
        float m_modAttackBaseDPSPct[MAX_ATTACK] = { 1.0f, 1.0f, 1.0f };

        // stat system
        bool HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply);
        void SetModifierValue(UnitMods unitMod, UnitModifierType modifierType, float value) { m_auraModifiersGroup[unitMod][modifierType] = value; }
        float GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const;
        float GetTotalStatValue(Stats stat) const;
        float GetTotalResistanceValue(SpellSchools school) const;
        float GetTotalAuraModValue(UnitMods unitMod) const;
        SpellSchools GetSpellSchoolByAuraGroup(UnitMods unitMod) const;
        Stats GetStatByAuraGroup(UnitMods unitMod) const;
        Powers GetPowerTypeByAuraGroup(UnitMods unitMod) const;
        bool CanModifyStats() const { return m_canModifyStats; }
        void SetCanModifyStats(bool modifyStats) { m_canModifyStats = modifyStats; }

        static float GetHealthBonusFromStamina(float stamina);
        virtual float GetHealthBonusFromStamina() const;
        static float GetManaBonusFromIntellect(float intellect);
        float GetManaBonusFromIntellect() const;

        virtual bool UpdateStats(Stats stat) = 0;
        virtual bool UpdateAllStats() = 0;
        virtual void UpdateResistances(uint32 school) = 0;
        virtual void UpdateArmor() = 0;
        virtual void UpdateMaxHealth();
        virtual void UpdateMaxPower(Powers power);
        virtual void UpdateAttackPowerAndDamage(bool ranged = false) = 0;
        virtual void UpdateDamagePhysical(WeaponAttackType attType) = 0;
        float GetTotalAttackPowerValue(WeaponAttackType attType) const;

        float GetBaseWeaponDamage(WeaponAttackType attType, WeaponDamageRange damageRange, uint8 index = 0) const;
        void SetBaseWeaponDamage(WeaponAttackType attType, WeaponDamageRange damageRange, float value, uint8 index = 0) { m_weaponDamageInfo.weapon[attType].damage[index].value[damageRange] = value; }

        SpellSchools GetWeaponDamageSchool(WeaponAttackType attType, uint8 index = 0) const { return m_weaponDamageInfo.weapon[attType].damage[index].school; }
        void SetWeaponDamageSchool(WeaponAttackType attType, SpellSchools school, uint8 index = 0) { m_weaponDamageInfo.weapon[attType].damage[index].school = school; }

        SpellSchoolMask GetAttackDamageSchoolMask(WeaponAttackType attType, bool first = false) const;
        void SetAttackDamageSchool(WeaponAttackType attType, SpellSchools school);

        inline SpellSchoolMask GetRangedDamageSchoolMask(bool first = false) const { return GetAttackDamageSchoolMask(RANGED_ATTACK, first); }
        inline void SetRangedDamageSchool(SpellSchools school) { SetAttackDamageSchool(RANGED_ATTACK, school); }

        inline SpellSchoolMask GetMeleeDamageSchoolMask(bool main, bool first = false) const { return GetAttackDamageSchoolMask((main ? BASE_ATTACK : OFF_ATTACK), first); }
        inline SpellSchoolMask GetMeleeDamageSchoolMask() const { return SpellSchoolMask(GetMeleeDamageSchoolMask(true, true) | GetMeleeDamageSchoolMask(false, true)); }
        inline void SetMeleeDamageSchool(bool main, SpellSchools school) { SetAttackDamageSchool((main ? BASE_ATTACK : OFF_ATTACK), school); }
        inline void SetMeleeDamageSchool(SpellSchools school) { SetMeleeDamageSchool(true, school); SetMeleeDamageSchool(false, school); }

        SpellSchoolMask GetMainAttackSchoolMask();

        // Visibility system
        UnitVisibility GetVisibility() const { return m_Visibility; }
        void SetVisibility(UnitVisibility x);
        void SetVisibilityWithoutUpdate(UnitVisibility x) { m_Visibility = x; }
        void UpdateVisibilityAndView() override;            // overwrite WorldObject::UpdateVisibilityAndView()

        // common function for visibility checks for player/creatures with detection code
        bool IsVisibleForOrDetect(Unit const* u, WorldObject const* viewPoint, bool detect, bool inVisibleList = false, bool is3dDistance = true, bool spell = false, bool ignorePhase = false) const;
        void SetPhaseMask(uint32 newPhaseMask, bool update) override; // overwrite WorldObject::SetPhaseMask

        // virtual functions for all world objects types
        bool isVisibleForInState(Player const* u, WorldObject const* viewPoint, bool inVisibleList) const override;
        // function for low level grid visibility checks in player/creature cases
        virtual bool IsVisibleInGridForPlayer(Player* pl) const = 0;
        virtual bool isInvisibleForAlive() const;

        TrackedAuraTargetMap&       GetTrackedAuraTargets(TrackedAuraType type)       { return m_trackedAuraTargets[type]; }
        TrackedAuraTargetMap const& GetTrackedAuraTargets(TrackedAuraType type) const { return m_trackedAuraTargets[type]; }
        SpellImmuneList m_spellImmune[MAX_SPELL_IMMUNITY];
        bool IsAOEImmune() const { return m_aoeImmune; }
        void SetAOEImmune(bool state) { m_aoeImmune = state; }
        bool IsChainImmune() const { return m_chainImmune; }
        void SetChainImmune(bool state) { m_chainImmune = state; }

        // Threat related methods
        bool CanHaveThreatList(bool ignoreAliveState = false) const;
        void AddThreat(Unit* pVictim, float threat = 0.0f, bool crit = false, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NONE, SpellEntry const* threatSpell = nullptr);
        float ApplyTotalThreatModifier(float threat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL);
        void DeleteThreatList();
        bool SelectHostileTarget();
        bool IsSuppressedTarget(Unit* target) const;
        bool IsOfflineTarget(Unit* victim) const;
        void TauntUpdate();
        void FixateTarget(Unit* taunter);
        ThreatManager& getThreatManager() { return GetCombatData()->threatManager; }
        ThreatManager const& getThreatManager() const { return GetCombatData()->threatManager; }
        void addHatedBy(HostileReference* pHostileReference) { GetCombatData()->hostileRefManager.insertFirst(pHostileReference); };
        void removeHatedBy(HostileReference* /*pHostileReference*/) { /* nothing to do yet */ }
        HostileRefManager& getHostileRefManager() { return GetCombatData()->hostileRefManager; }
        HostileRefManager const& getHostileRefManager() const { return GetCombatData()->hostileRefManager; }
        void SetNoThreatState(bool state) { m_noThreat = state; }
        bool GetNoThreatState() { return m_noThreat; }

        CombatManager& GetCombatManager() { return m_combatManager; }
        CombatManager const& GetCombatManager() const { return m_combatManager; }
        void TriggerEvadeEvents();
        void TriggerHomeEvents();
        void EvadeTimerExpired();

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
        Aura const* GetAura(uint32 spellId, SpellEffectIndex effindex) const;
        Aura* GetAura(AuraType type, SpellFamily family, uint64 familyFlag, uint32 familyFlag2 = 0, ObjectGuid casterGuid = ObjectGuid()) const;
        Aura* GetTriggeredByClientAura(uint32 spellId) const;
        SpellAuraHolder* GetSpellAuraHolder(uint32 spellid) const;
        SpellAuraHolder* GetSpellAuraHolder(uint32 spellid, ObjectGuid casterGuid) const;

        SpellAuraHolderMap&       GetSpellAuraHolderMap()       { return m_spellAuraHolders; }
        SpellAuraHolderMap const& GetSpellAuraHolderMap() const { return m_spellAuraHolders; }
        AuraList const& GetAurasByType(AuraType type) const { return m_modAuras[type]; }
        void ApplyAuraProcTriggerDamage(Aura* aura, bool apply);

        int32 GetTotalAuraModifier(AuraType auratype) const;
        int32 GetTotalAuraModifier(AuraType auratype, std::function<bool(Aura const*)> predicate) const;
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
        int32 GetMaxPositiveAuraModifierByItemClass(AuraType auratype, Item* weapon) const;

        Aura* GetDummyAura(uint32 spell_id) const;

        uint32 m_AuraFlags;

        uint32 GetDisplayId() const { return GetUInt32Value(UNIT_FIELD_DISPLAYID); }
        void SetDisplayId(uint32 modelId);
        uint32 GetNativeDisplayId() const { return GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID); }
        void SetNativeDisplayId(uint32 modelId) { SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, modelId); }
        void RestoreDisplayId();

        // at any changes to scale and/or displayId
        void UpdateModelData();
        void SendCollisionHeightUpdate(float height);

        DynamicObject* GetDynObject(uint32 spellId, SpellEffectIndex effIndex, Unit* target = nullptr);
        DynamicObject* GetDynObject(uint32 spellId);
        void AddDynObject(DynamicObject* dynObj);
        void RemoveDynObject(uint32 spellid);
        void RemoveDynObjectWithGUID(ObjectGuid guid) { m_dynObjGUIDs.remove(guid); }
        void RemoveAllDynObjects();

        GameObject* GetGameObject(uint32 spellId) const;
        void AddGameObject(GameObject* gameObj);
        void AddWildGameObject(GameObject* gameObj);
        void RemoveGameObject(GameObject* gameObj, bool del, bool removeAura = true);
        void RemoveGameObject(uint32 spellid, bool del);
        void RemoveAllGameObjects();

        void AddCreature(uint32 spellId, Creature* creature);
        void RemoveCreature(uint32 spellId, bool del);

        uint32 CalculateDamage(WeaponAttackType attType, bool normalized, uint8 index = 0);
        float GetAPMultiplier(WeaponAttackType attType, bool normalized);
        void ModifyAuraState(AuraState flag, bool apply);
        bool HasAuraState(AuraState flag) const { return HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1)); }
        bool HasAuraStateForCaster(AuraState flag, ObjectGuid casterGuid) const;
        void UnsummonAllTotems() const;
        Unit* SelectMagnetTarget(Unit* victim, Spell* spell = nullptr);

        int32 SpellBonusWithCoeffs(SpellEntry const* spellProto, SpellEffectIndex effectIndex, int32 total, int32 benefit, int32 ap_benefit, bool donePart, float defCoeffMod = 1.0f);
        int32 SpellBaseDamageBonusDone(SpellSchoolMask schoolMask);
        int32 SpellBaseDamageBonusTaken(SpellSchoolMask schoolMask) const;
        uint32 SpellDamageBonusDone(Unit* victim, SpellSchoolMask schoolMask, SpellEntry const* spellProto, SpellEffectIndex effectIndex, uint32 pdamage, DamageEffectType damagetype, uint32 stack = 1);
        uint32 SpellDamageBonusTaken(Unit* caster, SpellSchoolMask schoolMask, SpellEntry const* spellProto, SpellEffectIndex effectIndex, uint32 pdamage, DamageEffectType damagetype, uint32 stack = 1);
        int32 SpellBaseHealingBonusDone(SpellSchoolMask schoolMask);
        int32 SpellBaseHealingBonusTaken(SpellSchoolMask schoolMask) const;
        uint32 SpellHealingBonusDone(Unit* victim, SpellEntry const* spellProto, SpellEffectIndex effectIndex, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);
        uint32 SpellHealingBonusTaken(Unit* caster, SpellEntry const* spellProto, SpellEffectIndex effectIndex, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);
        uint32 MeleeDamageBonusDone(Unit* victim, uint32 damage, WeaponAttackType attType, SpellSchoolMask schoolMask, SpellEntry const* spellProto = nullptr, SpellEffectIndex effectIndex = EFFECT_INDEX_0, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1, bool flat = true);
        uint32 MeleeDamageBonusTaken(Unit* caster, uint32 pdamage, WeaponAttackType attType, SpellSchoolMask schoolMask, SpellEntry const* spellProto = nullptr, SpellEffectIndex effectIndex = EFFECT_INDEX_0, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1, bool flat = true);

        enum class SpellProcEventTriggerCheck
        {
            SPELL_PROC_TRIGGER_FAILED = 0,
            SPELL_PROC_TRIGGER_ROLL_FAILED = 1,
            SPELL_PROC_TRIGGER_OK = 2,
        };

        SpellProcEventTriggerCheck IsTriggeredAtSpellProcEvent(ProcExecutionData& data, SpellAuraHolder* holder, SpellProcEventEntry const*& spellProcEvent, bool (&canProc)[MAX_EFFECT_INDEX]);
        // only to be used in proc handlers - basepoints is expected to be a MAX_EFFECT_INDEX sized array
        SpellAuraProcResult TriggerProccedSpell(Unit* target, std::array<int32, MAX_EFFECT_INDEX>& basepoints, uint32 triggeredSpellId, Item* castItem, Aura* triggeredByAura, uint32 cooldown, ObjectGuid originalCaster);
        SpellAuraProcResult TriggerProccedSpell(Unit* target, std::array<int32, MAX_EFFECT_INDEX>& basepoints, SpellEntry const* spellInfo, Item* castItem, Aura* triggeredByAura, uint32 cooldown, ObjectGuid originalCaster);
        // Aura proc handlers
        SpellAuraProcResult HandleDummyAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleHasteAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleSpellCritChanceAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleProcTriggerSpellAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleProcTriggerDamageAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleOverrideClassScriptAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleRaidProcFromChargeAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleRaidProcFromChargeWithValueAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModCastingSpeedNotStackAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleReflectSpellsSchoolAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModPowerCostSchoolAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleMagnetAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleMechanicImmuneResistanceAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModDamageFromCasterAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleAddFlatModifierAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleAddPctModifierAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModDamagePercentDoneAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModRating(ProcExecutionData& data);
        SpellAuraProcResult HandleSpellMagnetAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleManaShieldAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleModResistanceAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleRemoveByDamageChanceProc(ProcExecutionData& data);
        SpellAuraProcResult HandleInvisibilityAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandlePeriodicAuraProc(ProcExecutionData& data);
        SpellAuraProcResult HandleSpellModProc(ProcExecutionData& data);
        SpellAuraProcResult HandleNULLProc(ProcExecutionData& /*data*/)
        {
            // no proc handler for this aura type
            return SPELL_AURA_PROC_OK;
        }
        SpellAuraProcResult HandleCantTrigger(ProcExecutionData& /*data*/)
        {
            // this aura type can't proc
            return SPELL_AURA_PROC_CANT_TRIGGER;
        }

        void SetLastManaUse() { m_lastManaUseTimer = 5000; }
        bool IsUnderLastManaUseEffect() const { return m_lastManaUseTimer != 0 && m_lastManaUseTimer != 5000; }

        uint32 GetRegenTimer() const { return m_regenTimer; }

        void SetContestedPvP(Player* attackedPlayer = nullptr);

        void ApplySpellImmune(Aura const* aura, uint32 op, uint32 type, bool apply);
        void ApplySpellDispelImmunity(Aura const* aura, DispelType type, bool apply);
        virtual bool IsImmuneToSpell(SpellEntry const* spellInfo, bool castOnSelf, uint8 effectMask, WorldObject const* caster);
        virtual bool IsImmuneToDamage(SpellSchoolMask meleeSchoolMask);
        virtual bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex index, bool castOnSelf) const;
        bool IsImmuneToSchool(SpellEntry const* spellInfo, uint8 effectMask) const;
        uint32 GetSchoolImmunityMask() const;
        uint32 GetDamageImmunityMask() const;
        uint32 GetMechanicImmunityMask() const;

        static float CalcArmorReducedDamage(WorldObject* attacker, Unit* victim, const float damage);
        void CalculateDamageAbsorbAndResist(Unit* caster, SpellSchoolMask schoolMask, DamageEffectType damagetype, const uint32 damage, uint32* absorb, int32* resist, bool canReflect = false, bool canResist = true, bool binary = false);
        void CalculateAbsorbResistBlock(Unit* caster, SpellNonMeleeDamage* spellDamageInfo, SpellEntry const* spellProto, WeaponAttackType attType = BASE_ATTACK);
        void CalculateHealAbsorb(uint32 heal, uint32* absorb);

        void  UpdateSpeed(UnitMoveType mtype, bool forced, float ratio = 1.0f);
        float GetSpeedInMotion() const;
        float GetSpeed(UnitMoveType mtype) const;
        float GetXYFlagBasedSpeed() const;
        float GetXYFlagBasedSpeed(uint32 moveFlags) const;
        float GetSpeedRateInMotion() const;
        float GetSpeedRate(UnitMoveType mtype) const { return m_speed_rate[mtype]; }
        void SetSpeedRate(UnitMoveType mtype, float rate, bool forced = false);

        void KnockBackFrom(Unit* target, float horizontalSpeed, float verticalSpeed);
        void KnockBackWithAngle(float angle, float horizontalSpeed, float verticalSpeed);

        bool HasHoverAura() const { return HasAuraType(SPELL_AURA_HOVER); }
        template<typename Func>
        bool HasAuraWithCondition(Func const& func) const
        {
            Unit::SpellAuraHolderMap const& holders = GetSpellAuraHolderMap();
            for (const auto& holder : holders)
            {
                if (func(holder.second))
                {
                    return true;
                }
            }

            return false;
        }

        void _RemoveAllAuraMods();
        void _ApplyAllAuraMods();

        uint32 CalcNotIgnoreAbsorbDamage(uint32 damage, SpellSchoolMask damageSchoolMask, SpellEntry const* spellInfo = nullptr) const;
        uint32 CalcNotIgnoreDamageReduction(uint32 damage, SpellSchoolMask damageSchoolMask) const;

        int32 CalculateAuraDuration(SpellEntry const* spellProto, uint32 effectMask, int32 duration, Unit const* caster);

        float CalculateLevelPenalty(SpellEntry const* spellProto) const;

        void addFollower(FollowerReference* pRef) { m_FollowingRefManager.insertFirst(pRef); }
        void removeFollower(FollowerReference* /*pRef*/) { /* nothing to do yet */ }

        MotionMaster* GetMotionMaster() { return &i_motionMaster; }
        MotionMaster const* GetMotionMaster() const { return &i_motionMaster; }

        bool IsStopped() const { return !(hasUnitState(UNIT_STAT_MOVING)); }
        void StopMoving(bool forceSendStop = false);
        void InterruptMoving(bool forceSendStop = false);

        ///----------Various crowd control methods-----------------
        inline bool IsCrowdControlled() const { return HasFlag(UNIT_FIELD_FLAGS, (UNIT_FLAG_CONFUSED | UNIT_FLAG_FLEEING | UNIT_FLAG_STUNNED)); }

        inline bool IsConfused() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED); }
        bool SetConfused(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0);

        inline bool IsFleeing() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING); }
        bool SetFleeing(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, uint32 duration = 0);

        inline bool IsStunned() const { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED); }
        bool SetStunned(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, bool logout = false);

        inline bool IsStunnedByLogout() const { return hasUnitState(UNIT_STAT_LOGOUT_TIMER); }
        bool SetStunnedByLogout(bool apply);

        // Panic: AI reaction script, NPC flees (e.g. at low health)
        inline bool IsInPanic() const { return hasUnitState(UNIT_STAT_PANIC); }
        inline bool SetInPanic(uint32 duration) { return SetFleeing(true, GetObjectGuid(), 0, duration); }

        inline bool IsImmobilizedState() const { return hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_LOGOUT_TIMER | UNIT_STAT_AI_ROOT); }
        void SetImmobilizedState(bool apply, bool stun = false, bool logout = false);
        ///----------End of crowd control methods----------

        bool IsFeigningDeath() const { return HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH); }
        bool IsFeigningDeathSuccessfully() const { return hasUnitState(UNIT_STAT_FEIGN_DEATH); }
        void SetFeignDeath(bool apply, ObjectGuid casterGuid = ObjectGuid(), uint32 spellID = 0, bool dynamic = true, bool success = true);
        virtual bool IsIgnoringFeignDeath() const { return false; }
        virtual bool IsIgnoringSanctuary() const { return false; }

        virtual bool IsIgnoringMisdirect() const { return false; }

        virtual bool IsSlowedInCombat() const { return false; }

        void InterruptSpellsCastedOnMe(bool killDelayed = false);

        void AddComboPointHolder(uint32 lowguid) { m_ComboPointHolders.insert(lowguid); }
        void RemoveComboPointHolder(uint32 lowguid) { m_ComboPointHolders.erase(lowguid); }
        void ClearComboPointHolders();

        ///----------Pet responses methods-----------------
        void SendPetActionFeedback(uint8 msg) const;
        void SendPetTalk(uint32 pettalk) const;
        void SendPetAIReaction() const;
        void SendPetDismiss(uint32 soundId) const;
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
        Movement::MoveSpline* movespline;

        void ScheduleAINotify(uint32 delay, bool forced = false);
        bool IsAINotifyScheduled() const { return m_AINotifyEvent != nullptr;}
        void FinalizeAINotifyEvent() { m_AINotifyEvent = nullptr; }
        void AbortAINotifyEvent();
        void OnRelocated();


        bool IsLinkingEventTrigger() const { return m_isCreatureLinkingTrigger; }
        void TriggerAggroLinkingEvent(Unit* enemy);

        virtual bool CanSwim() const = 0;
        virtual bool CanFly() const = 0;
        virtual bool CanWalk() const = 0;
        virtual bool IsFlying() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING); }

        float GetHoverOffset() const
        {
            return m_movementInfo.HasMovementFlag(MOVEFLAG_HOVER) ? GetFloatValue(UNIT_FIELD_HOVERHEIGHT) : 0.0f;
        }

        float GetHoverHeight() const
        {
            return GetFloatValue(UNIT_FIELD_HOVERHEIGHT);
        }

        // Take possession of an unit (pet, creature, ...)
        bool TakePossessOf(Unit* possessed);

        // Take possession of a new spawned unit
        Unit* TakePossessOf(SpellEntry const* spellEntry, SummonPropertiesEntry const* summonProp, uint32 effIdx, float x, float y, float z, float ang);

        // Take charm of an unit
        bool TakeCharmOf(Unit* charmed, uint32 spellId = 0, bool advertised = true);

        // Break own charm effects on a specific charmed unit
        void BreakCharmOutgoing(Unit* charmed);

        // Break own charm effects on all charmed units or advertised charm field only
        void BreakCharmOutgoing(bool advertisedOnly = false);

        // Break charm effects from current charmer
        void BreakCharmIncoming();

        // Uncharm (physically revert the charm effect) the unit and reset player control if required
        void Uncharm(Unit* charmed, uint32 spellId = 0);

        void RemoveUnattackableTargets(Unit* charmer = nullptr);

        // Combat prevention
        bool CanEnterCombat() const { return m_canEnterCombat && !GetCombatManager().IsEvadingHome(); }
        void SetCanEnterCombat(bool can) { m_canEnterCombat = can; }

        void SetIgnoreRangedTargets(bool state) { m_ignoreRangedTargets = state; }
        bool IsIgnoringRangedTargets() { return m_ignoreRangedTargets; }

        void SetSupportThreatOnly(bool state) { m_supportThreatOnly = state; }
        bool IsSupportThreatOnly() { return m_supportThreatOnly; }

        void DisableThreatPropagationToOwner() { m_ownerThreatPropagation = false; }
        bool IsPropagatingThreatToOwner() { return m_ownerThreatPropagation; } // TBC+ - Eye of Kilrogg

        float GetAttackDistance(Unit const* target) const;
        virtual uint32 GetDetectionRange() const { return 18.f; }

        virtual UnitAI* AI() { return nullptr; }
        virtual CombatData* GetCombatData() { return m_combatData; }
        virtual CombatData const* GetCombatData() const { return m_combatData; }

        virtual void SetBaseWalkSpeed(float speed) { m_baseSpeedWalk = speed; }
        virtual void SetBaseRunSpeed(float speed, bool /*force*/ = true) { m_baseSpeedRun = speed; }
        float GetBaseRunSpeed() { return m_baseSpeedRun; }

        void SetHoverHeight(float hoverHeight)
        {
            SetFloatValue(UNIT_FIELD_HOVERHEIGHT, hoverHeight);
        }

        bool IsSpellProccingHappening() const { return m_spellProcsHappening; }
        void AddDelayedHolderDueToProc(SpellAuraHolder* holder) { m_delayedSpellAuraHolders.push_back(holder); }

        void ResetAutoRepeatSpells() { m_AutoRepeatFirstCast = true; }

        const uint64& GetAuraUpdateMask() const { return m_auraUpdateMask; }
        void SetAuraUpdateMask(uint8 slot) { m_auraUpdateMask |= (uint64(1) << slot); }
        void ResetAuraUpdateMask() { m_auraUpdateMask = 0; }

        // WorldObject overrides
        void UpdateAllowedPositionZ(float x, float y, float& z, Map* atMap = nullptr) const override;
        void AdjustZForCollision(float x, float y, float& z, float halfHeight) const override;

        virtual uint32 GetSpellRank(SpellEntry const* spellInfo) const;

        Player* GetNextRandomRaidMember(float radius, AuraType noAuraType);

        bool HasOverrideScript(uint32 id) const;
        Aura* GetOverrideScript(uint32 id) const;
        void RegisterOverrideScriptAura(Aura* aura, uint32 id, bool apply);
        void RegisterScriptedLocationAura(Aura* aura, AuraScriptLocation location, bool apply); // Spell scripting - requires correctly set spell_affect
        std::vector<Aura*> const& GetScriptedLocationAuras(AuraScriptLocation location) const;

        uint8 GetComboPoints() const { return m_comboPoints; }
        ObjectGuid const& GetComboTargetGuid() const { return m_comboTargetGuid; }

        void AddComboPoints(Unit* target, int8 count);
        void ClearComboPoints();

        void RegisterScalingAura(Aura* aura, bool apply);
        void UpdateScalingAuras();

        uint32 GetDamageDoneByOthers() { return m_damageByOthers; }
        uint32 GetModifierXpBasedOnDamageReceived(uint32 xp);

        void UpdateNextUpdateTime() override;
        uint32 ShouldPerformObjectUpdate(uint32 const diff) override;
        
        void OverrideMountDisplayId(uint32 newDisplayId);

        void UpdateSplinePosition(bool relocateOnly = false);
        void SendFlightSplineSyncIfNeeded();

        virtual bool CanCallForAssistance() const { return true; }
        virtual bool CanCheckForHelp() const { return true; }

        virtual std::vector<uint32> GetCharmSpells() const { return {}; }
        void CharmCooldownInitialize(WorldPacket& data) const;

        FormationSlotDataSPtr GetFormationSlot() { return m_formationSlot; }
        void SetFormationSlot(FormationSlotDataSPtr fSlot) { m_formationSlot = fSlot; }

        void AddSummonForOnDeathDespawn(ObjectGuid guid);
        void DespawnSummonsOnDeath();

        // false if only visible to set and not equal
        virtual bool IsOnlyVisibleTo(ObjectGuid guid) const { return true; }

        virtual bool IsNoMountedFollow() const { return false; }

        virtual bool IsNoWeaponSkillGain() const { return false; }
        virtual bool IsPreventingDeath() const { return false; }
        virtual bool IsIgnoringMisdirection() const { return false; }

        void SetRootVehicle(const ObjectGuid& guid) { m_rootVehicle = guid; }
        const ObjectGuid& GetRootVehicle() const { return m_rootVehicle; }

        virtual bool CannotTurn() const { return false; }

        virtual CreatureInfo const* GetMountInfo() const { return nullptr; } // TODO: Meant to be used by players during taxi
        virtual void SetMountInfo(CreatureInfo const* /*info*/) {} // does nothing for base unit
        virtual void SetModelRunSpeed(float /*runSpeed*/) {} // does nothing for base unit

    protected:
        bool MeetsSelectAttackingRequirement(Unit* target, SpellEntry const* spellInfo, uint32 selectFlags, SelectAttackingTargetParams params, int32 unitConditionId) const;

        struct WeaponDamageInfo
        {
            struct Weapon
            {
                struct Damage
                {
                    SpellSchools school = SPELL_SCHOOL_NORMAL;
                    float value[2] = { BASE_MINDAMAGE, BASE_MAXDAMAGE };
                };

                uint32 lines = 1;
                Damage damage[MAX_ITEM_PROTO_DAMAGES];
            };

            Weapon weapon[MAX_ATTACK];
        };

        explicit Unit();

        void _UpdateSpells(uint32 time);
        void _UpdateAutoRepeatSpell();
        bool m_AutoRepeatFirstCast;

        uint32 m_attackTimer[MAX_ATTACK];

        float m_createStats[MAX_STATS];
        int32 m_createResistances[MAX_SPELL_SCHOOL];

        Unit* m_attacking;

        DeathState m_deathState;

        SpellAuraHolderMap m_spellAuraHolders;
        SpellAuraHolderMap::iterator m_spellAuraHoldersUpdateIterator; // != end() in Unit::m_spellAuraHolders update and point to next element
        AuraList m_deletedAuras;                            // auras removed while in ApplyModifier and waiting deleted
        SpellAuraHolderList m_deletedHolders;
        std::map<uint32, Aura*> m_classScripts;
        std::vector<Aura*> m_scriptedLocations[SCRIPT_LOCATION_MAX];
        std::vector<Aura*> m_scalingAuras;

        // Store Auras for which the target must be tracked
        TrackedAuraTargetMap m_trackedAuraTargets[MAX_TRACKED_AURA_TYPES];

        GuidList m_dynObjGUIDs;

        GameObjectList m_gameObj;
        typedef std::map<uint32, ObjectGuid> WildGameObjectMap;
        WildGameObjectMap m_wildGameObjs;
        bool m_isSorted;
        uint32 m_transform;

        std::map<uint32, Creature*> m_creatures;

        AuraList m_modAuras[TOTAL_AURAS];
        float m_auraModifiersGroup[UNIT_MOD_END][MODIFIER_TYPE_END];

        enum class AttackPowerMod
        {
            MELEE_ATTACK_POWER  = 0,
            RANGED_ATTACK_POWER = 1,
            ATTACK_POWER_MOD_MAX
        };

        enum class AttackPowerModSign
        {
            MOD_SIGN_POS,
            MOD_SIGN_NEG,
            MOD_SIGN_MAX
        };
        float m_attackPowerMod[size_t(AttackPowerMod::ATTACK_POWER_MOD_MAX)][size_t(AttackPowerModSign::MOD_SIGN_MAX)];

        WeaponDamageInfo m_weaponDamageInfo;

        bool m_canModifyStats;
        // std::list< spellEffectPair > AuraSpells[TOTAL_AURAS];  // TODO: use this if ok for mem
        VisibleAuraMap m_visibleAuras;

        float m_speed_rate[MAX_MOVE_TYPE];

        CharmInfo* m_charmInfo;

        MotionMaster i_motionMaster;

        uint32 m_reactiveTimer[MAX_REACTIVE];
        uint32 m_regenTimer;
        uint32 m_healthRegenTimer;
        uint32 m_lastManaUseTimer;

        bool m_canDodge;
        bool m_canParry;
        bool m_canBlock;

        bool m_canDualWield = false;

        MaNGOS::unique_trackable_ptr<VehicleInfo> m_vehicleInfo;
        void DisableSpline();
        void EndSpline();
        bool m_isCreatureLinkingTrigger;
        bool m_isSpawningLinked;
        ObjectGuid m_rootVehicle;

        CombatData* m_combatData;
        CombatManager m_combatManager;

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

        FormationSlotDataSPtr m_formationSlot;

        uint32 GetOverridenMountId() const { return m_overridenMountId; }

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
        static void JustKilledCreature(Unit* killer, Creature* victim, Player* responsiblePlayer);

        uint32 m_state;                                     // Even derived shouldn't modify

        AttackerSet m_attackers;                            // Used to help know who is currently attacking this unit
        Spell* m_currentSpells[CURRENT_MAX_SPELL];

        UnitVisibility m_Visibility;
        Position m_last_notified_position;
        BasicEvent* m_AINotifyEvent;
        ShortTimeTracker m_movesplineTimer;
        bool m_hasPeriodicAura;

        Diminishing m_Diminishing;

        FollowerRefManager m_FollowingRefManager;

        ComboPointHolderSet m_ComboPointHolders;

        GuidSet m_guardianPets;
        GuidSet::iterator m_guardianPetsIterator;

        GuidSet m_charmedUnitsPrivate;                      // stores non-advertised active charmed unit guids (e.g. aoe charms)

        GuidSet m_summonsForOnDeathDespawn;                 // SUMMON_PROP_FLAG_DESPAWN_ON_SUMMONER_DEATH

        ObjectGuid m_TotemSlot[MAX_TOTEM_SLOT];

        bool m_canEnterCombat;

        // Need to safeguard aura application in Unit::Update
        bool m_spellUpdateHappening;
        // Need to safeguard aura proccing in Unit::ProcDamageAndSpell
        bool m_spellProcsHappening;
        std::vector<SpellAuraHolder*> m_delayedSpellAuraHolders;
        uint32 m_hasHeartbeatProcCounter;

        bool m_alwaysHit;
        bool m_noThreat;
        bool m_supportThreatOnly;
        bool m_ownerThreatPropagation;
        bool m_ignoreRangedTargets;                         // Ignores ranged targets when picking someone to attack
        bool m_debuggingMovement;

        // guard to prevent chaining extra attacks
        bool m_extraAttacksExecuting;

        uint64 m_auraUpdateMask;

        ObjectGuid m_comboTargetGuid;
        int8 m_comboPoints;

        uint32 m_damageByOthers;

        bool m_isMountOverriden;
        uint32 m_overridenMountId;

        bool m_aoeImmune;
        bool m_chainImmune;

        TimePoint m_lastMoveTime; // used for resetting combat timer on melee

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
        for (m_guardianPetsIterator = m_guardianPets.begin(); m_guardianPetsIterator != m_guardianPets.end();)
            if (Pet* guardian = GetMap()->GetPet(*(m_guardianPetsIterator++)))
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
        for (auto m_guardianPet : m_guardianPets)
            if (Pet* guardian = GetMap()->GetPet(m_guardianPet))
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
struct TargetDistanceOrderNear
{
    WorldObject const* m_mainTarget;
    DistanceCalculation m_distcalc;

    TargetDistanceOrderNear(WorldObject const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator ">"
    bool operator()(WorldObject const* _Left, WorldObject const* _Right) const
    {
        return m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
    }
};

// Helper for targets furthest away to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrderFarAway
{
    WorldObject const* m_mainTarget;
    DistanceCalculation m_distcalc;
    TargetDistanceOrderFarAway(WorldObject const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator "<"
    bool operator()(WorldObject const* _Left, WorldObject const* _Right) const
    {
        return !m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
    }
};

struct LowestHPNearestOrder
{
    WorldObject const* m_mainTarget;
    DistanceCalculation m_distcalc;

    LowestHPNearestOrder(WorldObject const* target, DistanceCalculation distcalc = DIST_CALC_NONE) : m_mainTarget(target), m_distcalc(distcalc) {}
    // functor for operator ">"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        if (_Left->GetHealthPercent() == _Right->GetHealthPercent())
            return m_mainTarget->GetDistanceOrder(_Left, _Right, m_distcalc);
        return _Left->GetHealthPercent() < _Right->GetHealthPercent();
    }
};

class UnitLambdaEvent : public BasicEvent
{
    public:
    UnitLambdaEvent(Unit& owner, std::function<void(Unit&)> const& func) : m_owner(owner), m_func(func) {}

    virtual bool Execute(uint64 /*e_time*/, uint32 /*p_time*/)
    {
        m_func(m_owner);
        return true;
    }

    Unit& m_owner;
    std::function<void(Unit&)> m_func;
};

/** @} */

#endif
