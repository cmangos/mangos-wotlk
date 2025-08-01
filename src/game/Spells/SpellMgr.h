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

#ifndef _SPELLMGR_H
#define _SPELLMGR_H

// For static or at-server-startup loaded spell data
// For more high level function for sSpellTemplate data

#include "Common.h"
#include "Globals/SharedDefines.h"
#include "Spells/SpellAuraDefines.h"
#include "Spells/SpellTargets.h"
#include "Server/DBCStructure.h"
#include "Server/DBCStores.h"
#include "Entities/DynamicObject.h"
#include "Entities/GameObject.h"
#include "Entities/Corpse.h"
#include "Entities/Unit.h"
#include "Spells/SpellAuras.h"
#include "Server/SQLStorages.h"
#include "Spells/SpellEffectDefines.h"

#include <map>

class Player;
class Spell;
class Unit;
struct CreatureInfo;

// only used in code
enum SpellCategories
{
    SPELLCATEGORY_HEALTH_MANA_POTIONS = 4,
    SPELLCATEGORY_DEVOUR_MAGIC        = 12,
    SPELLCATEGORY_JUDGEMENT           = 1210,               // Judgement (seal trigger)
};

// Different spell properties
inline float GetSpellRadius(SpellRadiusEntry const* radius) { return (radius ? radius->Radius : 0); }
uint32 GetSpellCastTime(SpellEntry const* spellInfo, WorldObject* caster, Spell* spell = nullptr, bool consume = false);
uint32 GetSpellCastTimeForBonus(SpellEntry const* spellProto, DamageEffectType damagetype);
float CalculateDefaultCoefficient(SpellEntry const* spellProto, DamageEffectType const damagetype);
inline float GetSpellMinRange(SpellRangeEntry const* range, bool friendly = false)
{
    if (!range)
        return 0;
    return (friendly ? range->minRangeFriendly : range->minRange);
}
inline float GetSpellMaxRange(SpellRangeEntry const* range, bool friendly = false)
{
    if (!range)
        return 0;
    return (friendly ? range->maxRangeFriendly : range->maxRange);
}
inline uint32 GetSpellRecoveryTime(SpellEntry const* spellInfo) { return spellInfo->RecoveryTime > spellInfo->CategoryRecoveryTime ? spellInfo->RecoveryTime : spellInfo->CategoryRecoveryTime; }
int32 GetSpellDuration(SpellEntry const* spellInfo);
int32 GetSpellMaxDuration(SpellEntry const* spellInfo);
int32 CalculateSpellDuration(SpellEntry const* spellInfo, Unit const* caster, Unit const* target, AuraScript* auraScript);
uint16 GetSpellAuraMaxTicks(SpellEntry const* spellInfo);
uint16 GetSpellAuraMaxTicks(uint32 spellId);
WeaponAttackType GetWeaponAttackType(SpellEntry const* spellInfo);

inline bool IsSpellHaveEffect(SpellEntry const* spellInfo, SpellEffects effect)
{
    for (unsigned int i : spellInfo->Effect)
        if (SpellEffects(i) == effect)
            return true;
    return false;
}

inline bool IsAuraApplyEffect(SpellEntry const* spellInfo, SpellEffectIndex effecIdx)
{
    switch (spellInfo->Effect[effecIdx])
    {
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_PERSISTENT_AREA_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
        case SPELL_EFFECT_APPLY_AREA_AURA_RAID:
        case SPELL_EFFECT_APPLY_AREA_AURA_PET:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
        case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
            return true;
    }
    return false;
}

inline bool IsAuraApplyEffects(SpellEntry const* entry, SpellEffectIndexMask mask)
{
    if (!entry)
        return false;
    uint32 emptyMask = 0;
    for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
    {
        const uint32 current = (1 << i);
        if (entry->Effect[i])
        {
            if ((mask & current) && !IsAuraApplyEffect(entry, SpellEffectIndex(i)))
                return false;
        }
        else
            emptyMask |= current;
    }
    // Check if all queried effects are actually empty
    const bool empty = !(mask & ~emptyMask);
    return !empty;
}

inline bool IsSpellAppliesAura(SpellEntry const* spellInfo, uint32 effectMask = ((1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)))
{
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (effectMask & (1 << i))
            if (IsAuraApplyEffect(spellInfo, SpellEffectIndex(i)))
                return true;

    return false;
}

inline bool IsSpellWithNonAuraEffect(SpellEntry const* spellInfo)
{
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (!IsAuraApplyEffect(spellInfo, SpellEffectIndex(i)))
            return true;

    return false;
}

inline uint32 GetAuraEffectMask(SpellEntry const* spellInfo)
{
    uint32 mask = 0;
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (IsAuraApplyEffect(spellInfo, SpellEffectIndex(i)))
            mask |= (1 << i);

    return mask;
}

inline bool IsEffectHandledImmediatelySpellLaunch(SpellEntry const* spellInfo, SpellEffectIndex effecIdx)
{
    switch (spellInfo->Effect[effecIdx])
    {
        case SPELL_EFFECT_SCHOOL_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
        case SPELL_EFFECT_HEAL:
        case SPELL_EFFECT_HEAL_MECHANICAL:
        case SPELL_EFFECT_HEAL_PCT:
        case SPELL_EFFECT_CHARGE:
        case SPELL_EFFECT_CHARGE_DEST:
        case SPELL_EFFECT_JUMP:
        case SPELL_EFFECT_JUMP_DEST:
            return true;
        default:
            return false;
    }
}

inline bool IsPeriodicRegenerateEffect(SpellEntry const* spellInfo, SpellEffectIndex effecIdx)
{
    switch (AuraType(spellInfo->EffectApplyAuraName[effecIdx]))
    {
        case SPELL_AURA_PERIODIC_ENERGIZE:
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
            return true;
        default:
            return false;
    }
}

bool IsCastEndProcModifierAura(SpellEntry const* spellInfo, SpellEffectIndex effecIdx, SpellEntry const* procSpell);

inline bool IsSpellHaveAura(SpellEntry const* spellInfo, AuraType aura, uint32 effectMask = (1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2))
{
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (effectMask & (1 << i))
            if (AuraType(spellInfo->EffectApplyAuraName[i]) == aura)
                return true;
    return false;
}

inline bool IsNextMeleeSwingSpell(SpellEntry const* spellInfo)
{
	return spellInfo->HasAttribute(SPELL_ATTR_ON_NEXT_SWING_NO_DAMAGE) || spellInfo->HasAttribute(SPELL_ATTR_ON_NEXT_SWING);
}

inline bool IsSpellLastAuraEffect(SpellEntry const* spellInfo, SpellEffectIndex effecIdx)
{
    for (int i = effecIdx + 1; i < MAX_EFFECT_INDEX; ++i)
        if (spellInfo->EffectApplyAuraName[i])
            return false;
    return true;
}

inline bool IsSealSpell(SpellEntry const* spellInfo)
{
    // Collection of all the seal family flags. No other paladin spell has any of those.
    return spellInfo->IsFitToFamily(SPELLFAMILY_PALADIN, uint64(0x26000C000A000000)) &&
           // avoid counting target triggered effect as seal for avoid remove it or seal by it.
           spellInfo->EffectImplicitTargetA[EFFECT_INDEX_0] == TARGET_UNIT_CASTER;
}

inline bool IsAuraRemoveOnStacking(SpellEntry const* spellInfo, int32 effIdx) // TODO: extend to all effects
{
    switch (spellInfo->EffectApplyAuraName[effIdx])
    {
        case SPELL_AURA_MOD_INCREASE_ENERGY:
        case SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT:
        case SPELL_AURA_MOD_INCREASE_HEALTH:
        case SPELL_AURA_MOD_STAT:
            return false;
        default:
            return true;
    }
}

inline bool IsCharmAura(SpellEntry const* spellInfo, int32 effIdx) // TODO: extend to all effects
{
    switch (spellInfo->EffectApplyAuraName[effIdx])
    {
        case SPELL_AURA_MOD_CHARM:
        case SPELL_AURA_AOE_CHARM:
        case SPELL_AURA_MOD_POSSESS:
            return true;
        default:
            return false;
    }
}

inline bool IsAuraRefreshInsteadOfRecast(SpellEntry const* spellInfo, int32 effIdx) // TODO: extend to all effects
{
    switch (spellInfo->EffectApplyAuraName[effIdx])
    {
        case SPELL_AURA_NONE:
        case SPELL_AURA_MOD_STAT:
        case SPELL_AURA_MOD_RESISTANCE:
        case SPELL_AURA_MOD_BASE_RESISTANCE:
        case SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE:
        case SPELL_AURA_MOD_INCREASE_ENERGY:
        case SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE:
        case SPELL_AURA_MOD_THREAT:
        case SPELL_AURA_MOD_DAMAGE_TAKEN:
        case SPELL_AURA_PROC_TRIGGER_DAMAGE:
            return true;
        case SPELL_AURA_DUMMY:
        {
            switch (spellInfo->Id)
            {
                case 39921:                             // Vim'Gol Pentagram Beam
                    return true;
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}

inline bool IsAuraSpellRefreshInsteadOfRecast(SpellEntry const* spellInfo)
{
    if (IsAuraRefreshInsteadOfRecast(spellInfo, 0) && IsAuraRefreshInsteadOfRecast(spellInfo, 1) && IsAuraRefreshInsteadOfRecast(spellInfo, 2))
        return true;

    return false;
}

inline bool IsAllowingDeadTarget(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_EX2_ALLOW_DEAD_TARGET) || spellInfo->HasAttribute(SPELL_ATTR_PASSIVE) || spellInfo->Targets & (TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_UNIT_DEAD | TARGET_FLAG_CORPSE_ALLY);
}

inline bool IsExplicitDiscoverySpell(SpellEntry const* spellInfo)
{
    return (((spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_CREATE_RANDOM_ITEM
              || spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_CREATE_ITEM_2)
             && spellInfo->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SCRIPT_EFFECT)
            || spellInfo->Id == 64323);                     // Book of Glyph Mastery (Effect0==SPELL_EFFECT_SCRIPT_EFFECT without any other data)
}

inline bool IsLootCraftingSpell(SpellEntry const* spellInfo)
{
    return (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_CREATE_RANDOM_ITEM ||
            // different random cards from Inscription (121==Virtuoso Inking Set category) or without explicit item or explicit spells
            (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_CREATE_ITEM_2 &&
             (spellInfo->TotemCategory[0] != 0 || spellInfo->EffectItemType[0] == 0 || spellInfo->Id == 62941)));
}

inline bool IsSpellEffectTriggerSpell(const SpellEntry* entry, SpellEffectIndex effIndex)
{
    if (!entry)
        return false;

    switch (entry->Effect[effIndex])
    {
        case SPELL_EFFECT_TRIGGER_MISSILE:
        case SPELL_EFFECT_TRIGGER_SPELL:
        case SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE:
        case SPELL_EFFECT_TRIGGER_SPELL_2:
            return true;
    }
    return false;
}

inline bool IsSpellEffectTriggerSpellByAura(const SpellEntry* entry, SpellEffectIndex effIndex)
{
    if (!entry || !IsAuraApplyEffect(entry, effIndex))
        return false;

    switch (entry->EffectApplyAuraName[effIndex])
    {
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        case SPELL_AURA_PROC_TRIGGER_SPELL:
        case SPELL_AURA_PROC_TRIGGER_DAMAGE:
        case SPELL_AURA_PERIODIC_TRIGGER_BY_CLIENT:
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
        case SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE:
        case SPELL_AURA_TRIGGER_LINKED_AURA:
            return true;
    }
    return false;
}

inline bool IsSpellTriggerSpellByAura(const SpellEntry* entry)
{
    if (!entry)
        return false;

    for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (entry->Effect[i] && IsSpellEffectTriggerSpellByAura(entry, SpellEffectIndex(i)))
            return true;
    }
    return false;
}

inline bool IsSpellEffectAbleToCrit(const SpellEntry* entry, SpellEffectIndex index)
{
    if (!entry || entry->HasAttribute(SPELL_ATTR_EX2_CANT_CRIT))
        return false;

    switch (entry->Effect[index])
    {
        case SPELL_EFFECT_SCHOOL_DAMAGE:
        case SPELL_EFFECT_HEAL:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            return true;
        case SPELL_EFFECT_ENERGIZE: // Mana Potion and similar spells, Lay on hands
            return (entry->EffectMiscValue[index] == POWER_MANA && entry->SpellFamilyName && entry->DmgClass);
    }
    return false;
}

inline bool IsSpellAbleToCrit(const SpellEntry* entry)
{
    if (!entry || entry->HasAttribute(SPELL_ATTR_EX2_CANT_CRIT))
        return false;

    for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (entry->Effect[i] && IsSpellEffectAbleToCrit(entry, SpellEffectIndex(i)))
            return true;
    }
    return false;
}

bool IsPassiveSpell(uint32 spellId);
bool IsPassiveSpell(SpellEntry const* spellInfo);

inline bool IsPassiveSpellStackableWithRanks(SpellEntry const* spellProto)
{
    MANGOS_ASSERT(IsPassiveSpell(spellProto));
    return !IsSpellHaveEffect(spellProto, SPELL_EFFECT_APPLY_AURA);
}

inline bool IsSpellAffectedBySpellMods(SpellEntry const* spellInfo)
{
    return !(spellInfo->HasAttribute(SPELL_ATTR_EX3_CAN_PROC_FROM_PROCS) && IsPassiveSpell(spellInfo));
}

inline bool IsAutocastable(SpellEntry const* spellInfo)
{
    return !(spellInfo->HasAttribute(SPELL_ATTR_EX_NO_AUTOCAST_AI) || spellInfo->HasAttribute(SPELL_ATTR_PASSIVE));
}

inline bool IsAutocastable(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;
    return IsAutocastable(spellInfo);
}

// TODO: Unify with creature_template_spells so that we can set both attack and pet bar visibility
// If true, only gives access to spellbar, and not states and commands
// Works in connection with AI-CanHandleCharm
inline bool IsPossessCharmType(uint32 spellId)
{
    switch (spellId)
    {
        case 30019: // Control Piece - Chess event
        case 39219: // Death's Door Fel Cannon
        case 52244: // Charm Geist
            return true;
        default: return false;
    }
}

inline bool IsSpellNeedSendOnObjectUpdate(uint32 spellId)
{
    switch (spellId)
    {
        case 39123:
            return true;
        default: return false;
    }
}

inline bool IsSpellRemoveAllMovementAndControlLossEffects(SpellEntry const* spellProto)
{
    return spellProto->EffectApplyAuraName[EFFECT_INDEX_0] == SPELL_AURA_MECHANIC_IMMUNITY &&
           spellProto->EffectMiscValue[EFFECT_INDEX_0] == 1 &&
           spellProto->EffectApplyAuraName[EFFECT_INDEX_1] == 0 &&
           spellProto->EffectApplyAuraName[EFFECT_INDEX_2] == 0 &&
           spellProto->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT)/* && -- all above selected spells have SPELL_ATTR_EX5_* mask
        ((spellProto->AttributesEx5 &
            (SPELL_ATTR_EX5_USABLE_WHILE_CONFUSED|SPELL_ATTR_EX5_USABLE_WHILE_FEARED|SPELL_ATTR_EX5_USABLE_WHILE_STUNNED)) ==
            (SPELL_ATTR_EX5_USABLE_WHILE_CONFUSED|SPELL_ATTR_EX5_USABLE_WHILE_FEARED|SPELL_ATTR_EX5_USABLE_WHILE_STUNNED))*/;
}

inline uint32 GetAllowedMechanicMask(SpellEntry const* spellProto)
{
    uint32 mask = 0;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!spellProto->Effect[i])
            continue;

        if (spellProto->EffectApplyAuraName[i] == SPELL_AURA_MECHANIC_IMMUNITY)
            mask |= 1 << uint32(spellProto->EffectMiscValue[i] - 1);
        else if (spellProto->EffectApplyAuraName[i] == SPELL_AURA_MECHANIC_IMMUNITY_MASK)
            mask |= uint32(spellProto->EffectMiscValue[i]);
    }
    return mask;
}

// based on client Spell_C::CancelsAuraEffect
inline bool SpellCancelsAuraEffect(SpellEntry const* spellInfo, SpellEntry const* auraSpellInfo, uint8 auraEffIndex)
{
    if (!spellInfo->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
        return false;

    if (auraSpellInfo->HasAttribute(SPELL_ATTR_NO_IMMUNITIES))
        return false;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellInfo->Effect[i] != SPELL_EFFECT_APPLY_AURA)
            continue;

        uint32 const miscValue = static_cast<uint32>(spellInfo->EffectMiscValue[i]);
        switch (spellInfo->EffectApplyAuraName[i])
        {
            case SPELL_AURA_STATE_IMMUNITY:
                if (miscValue != auraSpellInfo->EffectApplyAuraName[auraEffIndex])
                    continue;
                break;
            case SPELL_AURA_SCHOOL_IMMUNITY:
                if (auraSpellInfo->HasAttribute(SPELL_ATTR_EX2_NO_SCHOOL_IMMUNITIES) || !(auraSpellInfo->SchoolMask & miscValue))
                    continue;
                break;
            case SPELL_AURA_DISPEL_IMMUNITY:
                if (miscValue != auraSpellInfo->Dispel)
                    continue;
                break;
            case SPELL_AURA_MECHANIC_IMMUNITY:
                if (miscValue != auraSpellInfo->Mechanic)
                    if (miscValue != auraSpellInfo->EffectMechanic[auraEffIndex])
                        continue;
                break;
            default:
                continue;
        }

        return true;
    }

    return false;
}

inline bool IsDeathOnlySpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_EX3_ONLY_ON_GHOSTS) || spellInfo->Id == 2584;
}

inline bool IsDeathPersistentSpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_EX3_ALLOW_AURA_WHILE_DEAD);
}

inline bool IsNonCombatSpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_NOT_IN_COMBAT_ONLY_PEACEFUL);
}

// some creatures should run immediately after being summoned by spell
inline bool IsSpellSetRun(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        case 30445: // Stillpine Ancestor Yor
        case 39163: // [DND]Rescue Wyvern
        case 43486: // Summon Amani'shi Warriors
        case 43487: // Summon Amani Eagle
        case 46214: // Summon Imp
        case 46245: // Summon Shadowsword Deathbringer
            return true;
        default:
            return false;
    }
}

inline bool IsSpellRemovedOnEvade(SpellEntry const* spellInfo)
{
    if (IsSpellHaveAura(spellInfo, SPELL_AURA_FLY) || IsSpellHaveAura(spellInfo, SPELL_AURA_CONTROL_VEHICLE) ||
            IsSpellHaveAura(spellInfo, SPELL_AURA_MIRROR_IMAGE) || IsSpellHaveAura(spellInfo, SPELL_AURA_MIRROR_NAME))
        return false;

    if (IsSpellHaveAura(spellInfo, SPELL_AURA_MOD_CHARM))
        return false;

    if (IsSpellHaveAura(spellInfo, SPELL_AURA_MOD_POSSESS))
        return false;

    if (spellInfo->HasAttribute(SPELL_ATTR_SS_IGNORE_EVADE))
        return false;

    if (spellInfo->HasAttribute(SPELL_ATTR_EX_AURA_STAYS_AFTER_COMBAT))
        return false;

    switch (spellInfo->Id)
    {
        case 588:           // Inner Fire (Rank 1)
        case 3235:          // Rancid Blood
        case 3284:          // Violent Shield
        case 3417:          // Thrash
        case 3418:          // Improved Blocking
        case 3509:          // Sloth Passive
        case 3512:          // Sludge Passive
        case 3582:          // Torch Burst
        case 3616:          // Poison Proc
        case 3637:          // Improved Blocking III
        case 4148:          // Growl of Fortitude
        case 5111:          // Living Flame Passive
        case 5301:          // Defensive State (DND)
        case 5680:          // Torch Burn
        case 6488:          // Sarilus's Elementals Passive
        case 6498:          // Feed Sarilus Passive
        case 6718:          // Phasing Stealth
        case 6752:          // Weak Poison Proc
        case 6820:          // Corrupted Agility Passive
        case 6821:          // Corrupted Strength Passive
        case 6822:          // Corrupted Stamina Passive
        case 6823:          // Corrupted Intellect Passive
        case 6923:          // Growl of Fortitude Proc
        case 6947:          // Curse of the Bleakheart Proc
        case 6961:          // Knockdown Proc
        case 7056:          // Pacified
        case 7090:          // Bear Form (Shapeshift)
        case 7095:          // Knockdown Proc
        case 7165:          // Battle Stance (Rank 1)
        case 7276:          // Poison Proc
        case 7999:          // Soot Covering
        case 8247:          // Wandering Plague
        case 8279:          // Stealth Detection
        case 8393:          // Barbs
        case 8599:          // Enrage
        case 8601:          // Slowing Poison
        case 8876:          // Thrash
        case 8909:          // Unholy Shield
        case 8990:          // Retribution Aura (Rank 1)
        case 9205:          // Hate to Zero (Hate to Zero)
        case 9460:          // Corrosive Ooze
        case 9464:          // Barbs
        case 9617:          // Ghost Visual
        case 9769:          // Radiation
        case 9941:          // Spell Reflection
        case 10022:         // Deadly Poison
        case 10072:         // Splintered Obsidian
        case 10074:         // Spell Reflection
        case 10095:         // Hate to Zero (Hate to Zero)
        case 11838:         // Hate to Zero (Hate to Zero)
        case 11919:         // Poison Proc
        case 11959:         // Poison Proc
        case 11964:         // Fevered Fatigue
        case 11966:         // Fire Shield
        case 11984:         // Immolate
        case 12099:         // Shield Spike
        case 12187:         // Disease Cloud
        case 12246:         // Infected Spine
        case 12529:         // Chilling Touch
        case 12539:         // Ghoul Rot
        case 12544:         // Frost Armor
        case 12546:         // Spitelash (Spitelash)
        case 12550:         // Lightning Shield
        case 12556:         // Frost Armor
        case 12627:         // Disease Cloud
        case 12787:         // Thrash
        case 12898:         // Smoke Aura Visual
        case 13260:         // Pet Bomb Passive
        case 13299:         // Poison Proc
        case 13616:         // Wracking Pains Proc
        case 13767:         // Hate to Zero (Hate to Zero)
        case 13787:         // Demon Armor
        case 14111:         // Bloodpetal Poison Proc
        case 14133:         // Muculent Fever Proc
        case 14178:         // Sticky Tar
        case 15088:         // Flurry
        case 15097:         // Enrage
        case 15506:         // Immolate
        case 15876:         // Ice Blast
        case 16140:         // Exploding Cadaver (Exploding Cadaver)
        case 16345:         // Disease Cloud
        case 16563:         // Drowning Death
        case 16577:         // Disease Cloud
        case 16592:         // Shadowform
        case 17327:         // Spirit Particles
        case 17467:         // Unholy Aura
        case 18148:         // Static Field
        case 18268:         // Fire Shield
        case 18847:         // Fevered Fatigue
        case 18943:         // Double Attack
        case 18968:         // Fire Shield
        case 19030:         // Bear Form (Shapeshift)
        case 18950:         // Invisibility and Stealth Detection
        case 19194:         // Double Attack
        case 19195:         // Hate to 90% (Hate to 90%)
        case 19396:         // Incinerate (Incinerate)
        case 19483:         // Immolation
        case 19514:         // Lightning Shield
        case 19626:         // Fire Shield (Fire Shield)
        case 19640:         // Pummel (Pummel)
        case 19817:         // Double Attack
        case 19818:         // Double Attack
        case 20514:         // Ruul Snowhoof Shapechange (DND)
        case 21061:         // Putrid Breath
        case 21857:         // Lava Shield
        case 21862:         // Radiation
        case 22128:         // Thorns
        case 22578:         // Glowy (Black)
        case 22650:         // Ghost Visual
        case 22735:         // Spirit of Runn Tum
        case 22781:         // Thornling
        case 22788:         // Grow
        case 22856:         // Ice Lock (Guard Slip'kik ice trap in Dire Maul)
        case 23255:         // Deep Wounds
        case 24313:         // Shade Visual
        case 25039:         // Green Ghost Visual
        case 25592:         // Hate to Zero (Hate to Zero)
        case 26341:         // Saurfang's Rage
        case 27578:         // Battle Shout
        case 27793:         // Disease Cloud
        case 27987:         // Unholy Aura
        case 28002:         // Ghost Visual
        case 28126:         // Spirit Particles (purple)
        case 28156:         // Disease Cloud
        case 28362:         // Disease Cloud
        case 28370:         // Toxic Gas
        case 28902:         // Bloodlust
        case 29406:         // Shadowform
        case 29526:         // Hate to Zero (Hate to Zero)
        case 30205:         // Shadow Cage - Magtheridon
        case 30982:         // Crippling Poison
        case 30987:         // Ghost Visual (Red)
        case 31332:         // Dire Wolf Visual
        case 31387:         // Time Rift Channel
        case 31607:         // Disease Cloud
        case 31690:         // Putrid Mushroom
        case 31722:         // Immolation
        case 31757:         // Pulverize
        case 31792:         // Bear Form (Shapeshift)
        case 32007:         // Mo'arg Engineer Transform Visual
        case 32064:         // Battle Shout
        case 32732:         // Flay
        case 32900:         // Bone Shards Proc
        case 32912:         // Windfury
        case 32939:         // Phase Burst
        case 32942:         // Phasing Invisibility
        case 33460:         // Inhibit Magic
        case 33483:         // Mana Tap
        case 33839:         // Vir'aani Concentration
        case 33900:         // Shroud of Death
        case 33908:         // Burning Spikes
        case 34343:         // Thorns
        case 35184:         // Unstable Affliction Proc
        case 35186:         // Melt Armor Proc
        case 35188:         // Chilling Touch
        case 35194:         // Shadowform
        case 35281:         // Raging Flames
        case 35319:         // Electric Skin
        case 35408:         // Fear Proc
        case 35596:         // Power of the Legion
        case 35747:         // Flame Buffet
        case 35838:         // Ghost Visual
        case 35841:         // Draenei Spirit Visual
        case 35847:         // Ghost Visual Red
        case 35850:         // Draenei Spirit Visual 2
        case 35917:         // Firey Intellect
        case 36006:         // Fel Fire Aura
        case 36118:         // Phasing Invisibility
        case 36635:         // Fiery Weapon Proc
        case 36640:         // Arcane Shield
        case 36784:         // Entropic Aura
        case 36788:         // Diminish Soul
        case 37119:         // Spirit Particles (Spawn)
        case 37256:         // Disease Cloud
        case 37266:         // Disease Cloud
        case 37411:         // Skettis Corrupted Ghosts
        case 37497:         // Shadowmoon Ghost Invisibility (Ghostrider of Karabor in SMV) 
        case 37509:         // Ghostly Facade
        case 37816:         // Shadowform
        case 38732:         // Fire Shield
        case 37863:         // Disease Cloud
        case 38844:         // Unholy Aura
        case 38847:         // Diminish Soul
        case 38857:         // Spell Ground
        case 39102:         // Glowy (Black)
        case 39311:         // Scrapped Fel Reaver transform aura that is never removed even on evade
        case 39918:         // Soulgrinder Ritual Visual ( in progress)
        case 39920:         // Soulgrinder Ritual Visual ( beam)
        case 40453:         // Aura of Fear
        case 40816:         // Saber Lash - Mother Shahraz
        case 40899:         // Felfire Proc
        case 41290:         // Disease Cloud
        case 41634:         // Invisibility and Stealth Detection
        case 42459:         // Dual Wield (Passive)
        case 44118:         // Fists of Arcane Fury
        case 44480:         // Seal of Wrath
        case 44505:         // Drink Fel Infusion
        case 44520:         // Fel Armor (Rank 2)
        case 44537:         // Fel Lightning
        case 44604:         // Enchantment of Spell Haste
        case 44855:         // Out of Phase
        case 44962:         // Archer - BE Male Transform Tier 1
        case 44918:         // Archer - BE Male Transform Tier 2
        case 44919:         // Archer - BE Male Transform Tier 3
        case 44920:         // Archer - BE Male Transform Tier 4
        case 44921:         // Archer - BE Female Transform Tier 1
        case 44922:         // Archer - BE Female Transform Tier 2
        case 44923:         // Archer - BE Female Transform Tier 3
        case 44924:         // Archer - BE Female Transform Tier 4
        case 44925:         // Archer - Draenei Male Transform Tier 1
        case 44926:         // Archer - Draenei Male Transform Tier 2
        case 44927:         // Archer - Draenei Male Transform Tier 3
        case 44928:         // Archer - Draenei Male Transform Tier 4
        case 44929:         // Archer - Draenei Female Transform Tier 1
        case 44930:         // Archer - Draenei Female Transform Tier 2
        case 44931:         // Archer - Draenei Female Transform Tier 3
        case 44932:         // Archer - Draenei Female Transform Tier 4
        case 44977:         // Fel Armor (Rank 2)
        case 45033:         // Abyssal Transformation
        case 45155:         // Warrior - BE Female Transform Tier 1
        case 45156:         // Warrior - BE Female Transform Tier 2
        case 45157:         // Warrior - BE Female Transform Tier 3
        case 45158:         // Warrior - BE Female Transform Tier 4
        case 45159:         // Warrior - BE Male Transform Tier 1
        case 45160:         // Warrior - BE Male Transform Tier 2
        case 45161:         // Warrior - BE Male Transform Tier 3
        case 45162:         // Warrior - BE Male Transform Tier 4
        case 45163:         // Warrior - Draenei Female Transform Tier 1
        case 45164:         // Warrior - Draenei Female Transform Tier 2
        case 45165:         // Warrior - Draenei Female Transform Tier 3
        case 45166:         // Warrior - Draenei Female Transform Tier 4
        case 45167:         // Warrior - Draenei Male Transform Tier 1
        case 45168:         // Warrior - Draenei Male Transform Tier 2
        case 45169:         // Warrior - Draenei Male Transform Tier 3
        case 45170:         // Warrior - Draenei Male Transform Tier 4
        case 45187:         // Dawnblade Attack
        case 45822:         // Iceblood Warmaster
        case 45823:         // Tower Point Warmaster
        case 45824:         // West Frostwolf Warmaster
        case 45826:         // East Frostwolf Warmaster
        case 45828:         // Dun Baldar North Marshal
        case 45829:         // Dun Baldar South Marshal
        case 45830:         // Stonehearth Marshal
        case 45831:         // Icewing Marshal
        case 46030:         // Seal of Wrath
        case 46048:         // Fel Lightning
        case 46277:         // Bring Pain
        case 46308:         // Burning Winds
        case 46565:         // Holyform
        case 46744:         // Chilling Touch
        case 47287:         // Burning Destruction
        case 47399:         // Frenzy
            return false;
        default:
            return true;
    }
}

inline bool IsChanneledDelayedSpell(SpellEntry const* spellInfo)
{
    if (!spellInfo->HasAttribute(SPELL_ATTR_EX_IS_CHANNELED) && !spellInfo->HasAttribute(SPELL_ATTR_EX_IS_SELF_CHANNELED))
        return false;

    switch (spellInfo->Id)
    {
        case 39238: // Fumping
        case 39246:
            return true;
        default: return false;
    }
}

bool IsExplicitPositiveTarget(uint32 targetA);
bool IsExplicitNegativeTarget(uint32 targetA);

inline bool IsResistableSpell(const SpellEntry* entry)
{
    return entry->DmgClass != SPELL_DAMAGE_CLASS_NONE;
}

inline bool IsSpellEffectDamage(SpellEntry const& spellInfo, SpellEffectIndex i)
{
    if (!spellInfo.EffectApplyAuraName[i])
    {
        // If its not an aura effect, check for damage effects
        switch (spellInfo.Effect[i])
        {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
            case SPELL_EFFECT_HEALTH_LEECH:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE:
            //   SPELL_EFFECT_POWER_BURN: deals damage for power burned, but its either full damage or resist?
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                return true;
        }
    }
    else
    {
        // If its an aura effect, check for DoT auras
        switch (spellInfo.EffectApplyAuraName[i])
        {
            case SPELL_AURA_PERIODIC_DAMAGE:
            case SPELL_AURA_PERIODIC_LEECH:
            //   SPELL_AURA_POWER_BURN_MANA: deals damage for power burned, but not really a DoT?
            case SPELL_AURA_PERIODIC_MANA_LEECH: // confirmed via 31447
            case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                return true;
        }
    }
    return false;
}

inline bool IsSpellEffectDummy(SpellEntry const& spellInfo, SpellEffectIndex i)
{
    if (!spellInfo.EffectApplyAuraName[i])
    {
        switch (spellInfo.Effect[i])
        {
            case SPELL_EFFECT_DUMMY:
                return true;
        }
    }
    else
    {
        switch (spellInfo.EffectApplyAuraName[i])
        {
            case SPELL_AURA_DUMMY:
            case SPELL_AURA_PERIODIC_DUMMY:
                return true;
        }
    }
    return false;
}

inline bool IsSpellEffectsDamage(SpellEntry const& spellInfo, uint8 effectMask = EFFECT_MASK_ALL)
{
    for (uint8 i = EFFECT_INDEX_0; (i < MAX_EFFECT_INDEX && effectMask); (++i, (effectMask >>= 1)))
    {
        if (spellInfo.Effect[i] && (effectMask & 1) && !IsSpellEffectDamage(spellInfo, SpellEffectIndex(i)))
            return false;
    }
    return true;
}

inline bool IsBinarySpell(SpellEntry const& spellInfo, uint8 effectMask = EFFECT_MASK_ALL)
{
    // Spell is considered binary if:
    // * (Pre-WotLK): It contains non-damage effects or auras
    // * (WotLK+): It contains no damage effects or auras
    uint8 validmask = 0;    // A bitmask of effects: set bits are valid effects
    uint8 nondmgmask = 0;   // A bitmask of effects: set bits are non-damage effects
    uint8 auramask = 0;     // A bitmask of aura effcts: set bits are auras
    for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
    {
        const uint8 thisMask = uint8(1 << i);

        if (!spellInfo.Effect[i] || !(effectMask & thisMask))
            continue;

        if (IsSpellEffectDummy(spellInfo, SpellEffectIndex(i)) || IsSpellEffectTriggerSpell(&spellInfo, SpellEffectIndex(i)))
            continue;

        validmask |= thisMask;

        if (spellInfo.EffectApplyAuraName[i])
            auramask |= thisMask;
        if (!IsSpellEffectDamage(spellInfo, SpellEffectIndex(i)))
            nondmgmask |= thisMask;
    }
    // No valid effects: treat as non-binary
    if (!validmask)
        return false;
    // All effects are non-damage: treat as binary
    // All effects are auras: treat as binary (even pure DoTs are treated as binary on initial application)
    if (nondmgmask == validmask || auramask == validmask)
        return true;
    const uint8 dmgmask = (validmask & ~nondmgmask);
    const uint8 dotmask = (dmgmask & auramask);
    // Just in case, if all damage effects are DoTs: treat as binary
    if (dmgmask == dotmask)
        return true;
    // If we ended up here, we have at least one non-aura damage effect
    // WotLK+: check if at least one non-damage effect hits the same target as damage effect (e.g. Frostbolt) and treat as binary
    if (nondmgmask)
    {
        uint8 directmask = (dmgmask & ~dotmask);
        for (uint8 i = EFFECT_INDEX_0; directmask; (++i, (directmask >>= 1)))
        {
            if (directmask & 1)
            {
                uint8 jmask = nondmgmask;
                for (uint8 j = EFFECT_INDEX_0; jmask; (++j, (jmask >>= 1)))
                {
                    if (jmask & 1)
                    {
                        if (spellInfo.EffectImplicitTargetA[i] == spellInfo.EffectImplicitTargetA[j] &&
                            spellInfo.EffectImplicitTargetB[i] == spellInfo.EffectImplicitTargetB[j])
                        {
                            return !(j < i && (auramask & (1 << (j - 1))) && spellInfo.EffectMechanic[j]); // WotLK+: exclude Frostbolt-like structure
                        }
                    }
                }
            }
        }
    }
    return false;
}

inline bool IsCasterSourceTarget(uint32 target)
{
    switch (target)
    {
        case TARGET_UNIT_CASTER:
        case TARGET_UNIT_CASTER_PET:
        case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
        case TARGET_ENUM_UNITS_ENEMY_IN_CONE_24:
        case TARGET_UNIT_CASTER_MASTER:
        case TARGET_LOCATION_UNIT_MINION_POSITION:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC:
        case TARGET_LOCATION_CASTER_FISHING_SPOT:
        case TARGET_LOCATION_CASTER_FRONT_RIGHT:
        case TARGET_LOCATION_CASTER_BACK_RIGHT:
        case TARGET_LOCATION_CASTER_BACK_LEFT:
        case TARGET_LOCATION_CASTER_FRONT_LEFT:
        case TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE:
        case TARGET_UNIT_CASTER_COMPANION:
        case TARGET_LOCATION_RANDOM_CIRCUMFERENCE:
        case TARGET_ENUM_UNITS_ENEMY_IN_CONE_104:
            return true;
        default:
            break;
    }
    return false;
}

inline bool IsSpellWithScriptUnitTarget(SpellEntry const* spellInfo)
{
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!spellInfo->Effect[i])
            continue;
        switch (spellInfo->EffectImplicitTargetA[i])
        {
            case TARGET_UNIT_SCRIPT_NEAR_CASTER:
                return true;
        }
    }

    return false;
}

inline bool IsSpellWithCasterSourceTargetsOnly(SpellEntry const* spellInfo)
{
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!spellInfo->Effect[i])                          // Skip junk in DBC
            continue;

        uint32 targetA = spellInfo->EffectImplicitTargetA[i];
        if (targetA && !IsCasterSourceTarget(targetA))
            return false;

        uint32 targetB = spellInfo->EffectImplicitTargetB[i];
        if (targetB && !IsCasterSourceTarget(targetB))
            return false;

        if (!targetA && !targetB)
            return false;
    }
    return true;
}

inline bool IsPointEffectTarget(SpellTarget target)
{
    switch (target)
    {
        case TARGET_LOCATION_CASTER_HOME_BIND:
        case TARGET_LOCATION_DATABASE:
        case TARGET_LOCATION_CASTER_SRC:
        case TARGET_LOCATION_SCRIPT_NEAR_CASTER:
        case TARGET_LOCATION_CASTER_TARGET_POSITION:
        case TARGET_LOCATION_UNIT_POSITION:
        case TARGET_LOCATION_CHANNEL_TARGET_DEST:
        case TARGET_LOCATION_NORTH:
        case TARGET_LOCATION_SOUTH:
        case TARGET_LOCATION_EAST:
        case TARGET_LOCATION_WEST:
        case TARGET_LOCATION_NE:
        case TARGET_LOCATION_NW:
        case TARGET_LOCATION_SE:
        case TARGET_LOCATION_SW:
            return true;
        default:
            break;
    }
    return false;
}

inline bool IsAreaEffectPositiveTarget(SpellTarget target)
{
    switch (target)
    {
        case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
        case TARGET_ENUM_UNITS_FRIEND_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_FRIEND_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC:
        case TARGET_UNIT_FRIEND_AND_PARTY:
        case TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE:
        case TARGET_UNIT_RAID_AND_CLASS:
            return true;
        default:
            break;
    }
    return false;
}

inline bool IsAreaEffectTarget(SpellTarget target)
{
    switch (target)
    {
        case TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_ENEMY_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_ENEMY_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
        case TARGET_ENUM_UNITS_ENEMY_IN_CONE_24:
        case TARGET_ENUM_UNITS_ENEMY_AOE_AT_DYNOBJ_LOC:
        case TARGET_ENUM_UNITS_FRIEND_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_FRIEND_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC:
        case TARGET_UNIT_FRIEND_AND_PARTY:
        case TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC:
        case TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE:
        case TARGET_UNIT_RAID_AND_CLASS:
        case TARGET_ENUM_UNITS_ENEMY_IN_CONE_104:
            return true;
        default:
            break;
    }
    return false;
}

inline bool IsAreaOfEffectSpell(SpellEntry const* spellInfo)
{
    if (IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_0])) || IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_0])))
        return true;
    if (IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_1])) || IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_1])))
        return true;
    if (IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_2])) || IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_2])))
        return true;
    return false;
}

inline bool IsAreaAuraEffect(uint32 effect)
{
    return effect == SPELL_EFFECT_APPLY_AREA_AURA_PARTY || effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID || effect == SPELL_EFFECT_APPLY_AREA_AURA_FRIEND || effect == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY || effect == SPELL_EFFECT_APPLY_AREA_AURA_PET || effect == SPELL_EFFECT_APPLY_AREA_AURA_OWNER;
}

inline bool HasAreaAuraEffect(SpellEntry const* spellInfo)
{
    for (unsigned int i : spellInfo->Effect)
        if (IsAreaAuraEffect(i))
            return true;
    return false;
}

inline bool IsPersistentAuraEffect(uint32 effect)
{
    return effect == SPELL_EFFECT_PERSISTENT_AREA_AURA;
}

inline bool HasPersistentAuraEffect(SpellEntry const* spellInfo)
{
    for (unsigned int i : spellInfo->Effect)
        if (IsPersistentAuraEffect(i))
            return true;
    return false;
}

inline bool IsOnlySelfTargeting(SpellEntry const* spellInfo)
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!spellInfo->Effect[i])
            return true;

        switch (spellInfo->EffectImplicitTargetA[i])
        {
            case TARGET_UNIT_CASTER:
            case TARGET_LOCATION_CURRENT_REFERENCE:
                break;
            default:
                return false;
        }
        switch (spellInfo->EffectImplicitTargetB[i])
        {
            case TARGET_UNIT_CASTER:
            case TARGET_LOCATION_CURRENT_REFERENCE:
            case TARGET_NONE:
                break;
            default:
                return false;
        }
    }
    return true;
}
 
inline bool IsDirectDamageSpell(SpellEntry const* spellInfo)
{
    if (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_SCHOOL_DAMAGE || spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL)
        return true;

    if (spellInfo->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SCHOOL_DAMAGE || spellInfo->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL)
        return true;

    if (spellInfo->Effect[EFFECT_INDEX_2] == SPELL_EFFECT_SCHOOL_DAMAGE || spellInfo->Effect[EFFECT_INDEX_2] == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL)
        return true;

    return false;
}

inline bool IsCheckCastTarget(uint32 target)
{
    // copy of checkcast block for attackability and assistability
    auto& data = SpellTargetInfoTable[target];
    if (data.type == TARGET_TYPE_UNIT && data.filter != TARGET_SCRIPT && (data.enumerator == TARGET_ENUMERATOR_SINGLE || data.enumerator == TARGET_ENUMERATOR_CHAIN))
    {
        switch (target)
        {
            case TARGET_UNIT_ENEMY_NEAR_CASTER:
            case TARGET_UNIT_FRIEND_NEAR_CASTER:
            case TARGET_UNIT_NEAR_CASTER:
            case TARGET_UNIT_CASTER_MASTER:
            case TARGET_UNIT_CASTER: break; // never check anything
            default: return true;
        }
    }

    return false;
}

inline uint32 GetCheckCastEffectMask(SpellEntry const* spellInfo)
{
    uint32 resultingMask = 0;
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (IsCheckCastTarget(spellInfo->EffectImplicitTargetA[i]) || IsCheckCastTarget(spellInfo->EffectImplicitTargetB[i]))
            resultingMask |= (1 << i);
    return resultingMask;
}

inline bool HasSpellTarget(SpellEntry const* spellInfo, uint32 target)
{
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (spellInfo->EffectImplicitTargetA[i] == target)
            return true;

    return false;
}

inline uint32 GetCheckCastSelfEffectMask(SpellEntry const* spellInfo)
{
    uint32 resultingMask = 0;
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_CASTER)
            resultingMask |= (1 << i);
    return resultingMask;
}

inline bool IsUnitTargetTarget(uint32 target)
{
    switch (target)
    {
        case TARGET_UNIT_ENEMY:
        case TARGET_UNIT:
        case TARGET_UNIT_FRIEND:
        case TARGET_UNIT_FRIEND_CHAIN_HEAL:
        case TARGET_UNIT_PARTY:
        case TARGET_UNIT_RAID:
        case TARGET_UNIT_FRIEND_AND_PARTY:
        case TARGET_LOCATION_CASTER_TARGET_POSITION:
        case TARGET_LOCATION_UNIT_FRONT:
        case TARGET_LOCATION_UNIT_BACK:
        case TARGET_LOCATION_UNIT_RIGHT:
        case TARGET_LOCATION_UNIT_LEFT:
        case TARGET_LOCATION_UNIT_FRONT_RIGHT:
        case TARGET_LOCATION_UNIT_BACK_RIGHT:
        case TARGET_LOCATION_UNIT_BACK_LEFT:
        case TARGET_LOCATION_UNIT_FRONT_LEFT:
            return true;
        default: return false;
    }
}

inline bool HasMissingTargetFromClient(SpellEntry const* spellInfo)
{
    // client only checks effect 0 target A
    if (IsUnitTargetTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_0]))
        return false;

    if (IsUnitTargetTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_1]) || IsUnitTargetTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_2]))
        return true;

    if (IsUnitTargetTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_0]) || IsUnitTargetTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_1]) || IsUnitTargetTarget(spellInfo->EffectImplicitTargetB[EFFECT_INDEX_2]))
        return true;

    return false;
}

inline bool IsSpellRequireTarget(SpellEntry const* spellInfo)
{
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (IsUnitTargetTarget(spellInfo->EffectImplicitTargetA[i]) || IsUnitTargetTarget(spellInfo->EffectImplicitTargetB[i]))
            return true;

    return false;
}

inline bool IsScriptTarget(uint32 target)
{
    return (target < MAX_SPELL_TARGETS ? (SpellTargetInfoTable[target].filter == TARGET_SCRIPT) : false);
}

inline bool IsNeutralTarget(uint32 target)
{
    return (target < MAX_SPELL_TARGETS ? (SpellTargetInfoTable[target].filter == TARGET_NEUTRAL) : false);
}

inline bool IsFriendlyTarget(uint32 target)
{
    return (target < MAX_SPELL_TARGETS ? (SpellTargetInfoTable[target].filter == TARGET_HELPFUL) : false);
}

inline bool IsHostileTarget(uint32 target)
{
    return (target < MAX_SPELL_TARGETS ? (SpellTargetInfoTable[target].filter == TARGET_HARMFUL) : false);
}

inline bool IsEffectTargetScript(uint32 targetA, uint32 targetB)
{
    return (IsScriptTarget(targetA) || IsScriptTarget(targetB));
}

inline bool IsEffectTargetNeutral(uint32 targetA, uint32 targetB)
{
    return (IsNeutralTarget(targetA) && IsNeutralTarget(targetB));
}

inline bool IsEffectTargetPositive(uint32 targetA, uint32 targetB)
{
    return (IsFriendlyTarget(targetA) || IsFriendlyTarget(targetB));
}

inline bool IsEffectTargetNegative(uint32 targetA, uint32 targetB)
{
    return (IsHostileTarget(targetA) || IsHostileTarget(targetB));
}

inline bool IsNeutralEffectTargetPositive(uint32 etarget, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (etarget < MAX_SPELL_TARGETS && SpellTargetInfoTable[etarget].type != TARGET_TYPE_UNIT)
        return true; // Some gameobjects or coords, who cares

    if (!target || (target->GetTypeId() != TYPEID_PLAYER && target->GetTypeId() != TYPEID_UNIT))
        return true;

    if (caster == target)
        return true; // Early self-cast detection

    if (!caster)
        return true;

    return !caster->CanAttackSpell(static_cast<const Unit*>(target));
}

inline bool IsPositiveEffectTargetMode(const SpellEntry* entry, SpellEffectIndex effIndex, const WorldObject* caster = nullptr, const WorldObject* target = nullptr, bool recursive = false)
{
    if (!entry)
        return false;

    // Triggered spells case: prefer child spell via IsPositiveSpell()-like scan for triggered spell
    if (IsSpellEffectTriggerSpell(entry, effIndex))
    {
        const uint32 spellid = entry->EffectTriggerSpell[effIndex];
        // Its possible to go infinite cycle with triggered spells. We are interested to peek only at the first layer so far
        if (!recursive && spellid && (spellid != entry->Id))
        {
            if (const SpellEntry* triggered = sSpellTemplate.LookupEntry<SpellEntry>(spellid))
            {
                for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (!IsPositiveEffectTargetMode(triggered, SpellEffectIndex(i), caster, target, true))
                        return false;
                }
            }
        }
        // For trigger effects target modes are inconsistent: we have invalid and coflicting ones
        // Let's try to ignore them completely
        return true;
    }

    const uint32 a = entry->EffectImplicitTargetA[effIndex];
    const uint32 b = entry->EffectImplicitTargetB[effIndex];

    if ((!a && !b) || IsEffectTargetPositive(a, b) || IsEffectTargetScript(a, b))
        return true;
    if (IsEffectTargetNegative(a, b))
    {
        // Workaround: Passive talents with negative target modes are getting removed by ice block and similar effects
        // TODO: Fix removal of passives in appropriate places and remove the check below afterwards
        return entry->HasAttribute(SPELL_ATTR_PASSIVE);
    }
    if (IsEffectTargetNeutral(a, b))
        return (IsPointEffectTarget(SpellTarget(b ? b : a)) || IsNeutralEffectTargetPositive((b ? b : a), caster, target));

    // If we ever get to this point, we have unhandled target. Gotta say something about it.
    if (entry->Effect[effIndex])
        DETAIL_LOG("IsPositiveEffectTargetMode: Spell %u's effect %u has unhandled targets (A:%u B:%u)", entry->Id, effIndex,
                   entry->EffectImplicitTargetA[effIndex], entry->EffectImplicitTargetB[effIndex]);
    return true;
}

inline bool IsPositiveEffect(const SpellEntry* spellproto, SpellEffectIndex effIndex, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!spellproto)
        return false;

    switch (spellproto->Id) // Spells whose effects are always positive
    {
        case 24742: // Magic Wings
        case 29880: // Mana Shield - Arcane Anomaly 16488
        case 42867:
        case 34786: // Temporal Analysis - factions and unitflags of target/caster verified, should not incur combat
        case 39384: // Fury Of Medivh visual - Burning Flames - Fury of medivh is friendly to all, and it hits all chess pieces, basically friendly fire damage
        case 37277: // Summon Infernal - neutral spell with TARGET_UNIT which evaluates as hostile due to neutral factions, with delay and gets removed by !IsPositiveSpell check
        case 42399: // Neutral spell with TARGET_UNIT, caster faction 14, target faction 14, evaluates as negative spell
        case 39995: // Four Dragons: Dummy to Dragon - Dummy effect need to trigger even if target is immune
                    // because of POS/NEG decision, should in fact be NEUTRAL decision TODO: Increase check fidelity
        case 33637: // Infernal spells - Neutral targets - in sniff never put into combat - Maybe neutral spells do not put into combat?
        case 33241:
        case 33045: // Wrath of the Astromancer - stacks like positive
        case 52149: // Rain of Darkness - factions and unitflags of target/caster verified - TARGET_DUELVSPLAYER - neutral target type
        case 35424: // Soul Shadows - used by Shade of Mal'druk on Mal'druk the Soulrender
        case 42628: // Zul'Aman - Fire Bomb - Neutral spell
        case 44406: // Energy Infusion - supposed to be buff despite negative targeting
        case 48598: // Ride Flamebringer Cue - the player (target) is supposed to board the dragon (caster); Positive effect needed to avoid combat
            return true;
        case 43101: // Headless Horseman Climax - Command, Head Requests Body - must be negative so that SPELL_ATTR_NO_IMMUNITIES isn't ignored, Headless Horseman script target is immune
        case 34190: // Arcane Orb - should be negative
                    /*34172 is cast onto friendly target, and fails bcs its delayed and we remove negative delayed on friendlies due to Duel code, if we change target pos code
                    bcs 34190 will be evaled as neg, 34172 will be evaled as neg, and hence be removed cos its negative delayed on a friendly*/
        case 34700: // Allergic Reaction - Neutral target type - needs to be a debuff
        case 36717: // Neutral spells with SPELL_ATTR_EX3_TARGET_ONLY_PLAYER as a filter
        case 38829:
        case 40055: // Introspection - Apexis Relic
        case 40165:
        case 40166:
        case 40167:
        case 36812: // Soaring
        case 37910: // overriding because immunity is preventing the spellcast - maybe neutral spells should be judged like negatives?
        case 37940:
        case 37962:
        case 37968:
        case 35734: // Dropping Phase Disruptor
            return false;
    }

    switch (spellproto->Effect[effIndex])
    {
        case SPELL_EFFECT_SEND_TAXI:                // Some NPCs that send taxis are neutral, so target mode fails
        case SPELL_EFFECT_QUEST_COMPLETE:           // TODO: Spells with these effects should be casted by a proper caster to meet target mode.
        case SPELL_EFFECT_KILL_CREDIT_GROUP:        // Until then switch makes them all positive
        case SPELL_EFFECT_KILL_CREDIT_PERSONAL:
        case SPELL_EFFECT_TELEPORT_UNITS:
        case SPELL_EFFECT_CREATE_ITEM:
        case SPELL_EFFECT_SUMMON_CHANGE_ITEM:
            return true;
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:    // Always hostile effects
            return false;
        case SPELL_EFFECT_DUMMY:
            // some explicitly required dummy effect sets
            switch (spellproto->Id)
            {
                case 18153:                                 // Kodo Kombobulator
                case 62105:                                 // To'kini's Blowgun
                    return true;
                case 28441:                                 // AB Effect 000
                case 42442:                                 // Vengeance Landing Cannonfire
                    return false;
                default:
                    break;
            }
            break;
        case SPELL_EFFECT_SCRIPT_EFFECT:
            // some explicitly required script effect sets
            switch (spellproto->Id)
            {
                case 42436:                                 // Drink!
                    return true;
                default:
                    break;
            }
            break;
        case SPELL_EFFECT_SCHOOL_DAMAGE:
        {
            //switch (spellproto->Id)
            //{
            //    default:
            //        break;
            //}
            break;
        }
        // Aura exceptions:
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        {
            switch (spellproto->EffectApplyAuraName[effIndex])
            {
                case SPELL_AURA_CONTROL_VEHICLE:            // Control vehicle auras have to be positive; boarding a vehicle cannot put the passenger in combat with the controlled unit
                    return true;
                case SPELL_AURA_DUMMY:                      // dummy aura can be positive or negative dependent from casted spell
                {
                    switch (spellproto->Id)
                    {
                        case 13139:                         // net-o-matic special effect
                        case 44877:                         // Living Flare Master
                            return false;
                        case 39834:                         // Vimgol population tester
                        case 39851:                         // Vimgol population tester
                        case 39852:                         // Vimgol population tester
                        case 39853:                         // Vimgol population tester
                        case 39854:                         // Vimgol population tester
                        case 39921:                         // Vimgol Pentagram Beam
                        case 70346:                         // Slime Puddle
                            return true;
                        default:
                            break;
                    }
                    break;
                }
                case SPELL_AURA_MOD_MELEE_HASTE:
                {
                    switch (spellproto->Id)
                    {
                        case 38449:                         // Blessing of the Tides
                            return true;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case SPELL_EFFECT_PERSISTENT_AREA_AURA:
        {
            switch (spellproto->EffectApplyAuraName[effIndex])
            {
                case SPELL_AURA_PERIODIC_DAMAGE: // possible TODO: make all return false
                {
                    switch (spellproto->Id)
                    {
                        case 37465: // chess rain of fire and poison cloud spells
                        case 37469:
                        case 37775:
                            return false;
                    }
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    // Generic effect check: negative on negative targets, positive on positive targets
    return IsPositiveEffectTargetMode(spellproto, effIndex, caster, target);
}

inline bool IsPositiveAuraEffect(const SpellEntry* entry, SpellEffectIndex effIndex, const WorldObject* /*caster*/ = nullptr, const WorldObject* /*target*/ = nullptr)
{
    if (IsAuraApplyEffect(entry, effIndex))
    {
        if (entry->HasAttribute(SPELL_ATTR_EX4_AURA_IS_BUFF))
            return true;

        if (entry->HasAttribute(SPELL_ATTR_AURA_IS_DEBUFF))
            return false;

        return !IsEffectTargetNegative(entry->EffectImplicitTargetA[effIndex], entry->EffectImplicitTargetB[effIndex]) && entry->Effect[effIndex] != SPELL_EFFECT_APPLY_AREA_AURA_ENEMY;
    }

    return false;
}

inline bool IsPositiveSpellTargetModeForSpecificTarget(const SpellEntry* entry, uint8 effectMask, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!entry)
        return false;
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (effectMask & (1 << i))
            if (entry->Effect[i] && !IsPositiveEffectTargetMode(entry, SpellEffectIndex(i), caster, target))
                return false;
    return true;
}

inline bool IsPositiveSpellTargetModeForSpecificTarget(uint32 spellId, uint8 effectMask, const WorldObject* caster, const WorldObject* target)
{
    if (!spellId)
        return false;
    return IsPositiveSpellTargetModeForSpecificTarget(sSpellTemplate.LookupEntry<SpellEntry>(spellId), effectMask, caster, target);
}

inline bool IsPositiveSpellTargetMode(const SpellEntry* entry, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!entry)
        return false;
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (entry->Effect[i] && !IsPositiveEffectTargetMode(entry, SpellEffectIndex(i), caster, target))
            return false;
    return true;
}

inline bool IsPositiveSpellTargetMode(uint32 spellId, const WorldObject* caster, const WorldObject* target)
{
    if (!spellId)
        return false;
    return IsPositiveSpellTargetMode(sSpellTemplate.LookupEntry<SpellEntry>(spellId), caster, target);
}

inline bool IsPositiveSpell(const SpellEntry* entry, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!entry)
        return false;
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (entry->Effect[i] && !IsPositiveEffect(entry, SpellEffectIndex(i), caster, target))
            return false;
    return true;
}

// this is propably the correct check for most positivity/negativity decisions
inline bool IsPositiveEffectMask(const SpellEntry* entry, uint8 effectMask, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!entry)
        return false;
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (entry->Effect[i] && (effectMask & (1 << i)) && !IsPositiveEffect(entry, SpellEffectIndex(i), caster, target))
            return false;
    return true;
}

inline bool IsPositiveSpell(uint32 spellId, const WorldObject* caster = nullptr, const WorldObject* target = nullptr)
{
    if (!spellId)
        return false;
    return IsPositiveSpell(sSpellTemplate.LookupEntry<SpellEntry>(spellId), caster, target);
}

inline bool IsSpellDoNotReportFailure(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        case 32172:     // Thrallmars/Honor holds favor trigger spell
            return true;
        default:
            return false;
    }
}

inline bool CanPierceImmuneAura(SpellEntry const* spellInfo, SpellEntry const* auraSpellInfo, uint8 effectMask = 0, SpellEffectIndex effIdx = EFFECT_INDEX_0)
{
    // aura can't be pierced
    if (auraSpellInfo && auraSpellInfo->HasAttribute(SPELL_ATTR_NO_IMMUNITIES))
        return false;

    // these spells pierce all available spells (Resurrection Sickness for example)
    if (spellInfo->HasAttribute(SPELL_ATTR_NO_IMMUNITIES))
        return true;

    if (!auraSpellInfo)
        return false;

    // these spells (Cyclone for example) can pierce all...
    if (spellInfo->HasAttribute(SPELL_ATTR_EX_IMMUNITY_TO_HOSTILE_AND_FRIENDLY_EFFECTS) || spellInfo->HasAttribute(SPELL_ATTR_EX2_NO_SCHOOL_IMMUNITIES))
    {
        // ...but not these (Divine shield, Ice block, Cyclone and Banish for example)
        if (auraSpellInfo->Mechanic != MECHANIC_IMMUNE_SHIELD &&
            auraSpellInfo->Mechanic != MECHANIC_INVULNERABILITY &&
            auraSpellInfo->Mechanic != MECHANIC_BANISH)
            return true;
    }

    if (spellInfo->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
    {
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            uint32 miscValue = (uint32)spellInfo->EffectMiscValue[i];
            if (spellInfo->Effect[i] && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MECHANIC_IMMUNITY_MASK)
                if (effectMask & (1 << i))
                    if (miscValue & (1 << auraSpellInfo->Mechanic) || miscValue & (1 << auraSpellInfo->EffectMechanic[effIdx]))
                        return true;
        }
    }

    // TODO: Add SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY logic

    return false;
}

inline void GetChainJumpRange(SpellEntry const* spellInfo, SpellEffectIndex effIdx, float& minSearchRangeCaster, float& maxSearchRangeTarget)
{ 
    const SpellRangeEntry* range = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
    if (spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE)
        maxSearchRangeTarget = range->maxRange;
    else
        // FIXME: This very like horrible hack and wrong for most spells
        maxSearchRangeTarget = spellInfo->EffectChainTarget[effIdx] * CHAIN_SPELL_JUMP_RADIUS;

    if (range->ID == 114)   // Hunter Search range
        minSearchRangeCaster = 5;

    switch (spellInfo->Id)
    {
        case 32445: // Holy Wrath - Maiden of Virtue
            maxSearchRangeTarget = 100.f;
            break;
        case 40827: // Beams - Mother Shahraz
        case 40859:
        case 40860:
        case 40861:
            minSearchRangeCaster = 0.f;
            maxSearchRangeTarget = 150.f;
            break;
        case 44537: // Fel Lightning - MgT / SWP
        case 46048:
        case 46480:
            maxSearchRangeTarget = 50.f;
            break;
        default:   // default jump radius
            break;
    }
}

inline bool IsDispelSpell(SpellEntry const* spellInfo)
{
    return IsSpellHaveEffect(spellInfo, SPELL_EFFECT_DISPEL);
}

inline bool isSpellBreakStealth(SpellEntry const* spellInfo)
{
    return !spellInfo->HasAttribute(SPELL_ATTR_EX_ALLOW_WHILE_STEALTHED);
}

inline bool IsAutoRepeatRangedSpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT) && spellInfo->HasAttribute(SPELL_ATTR_EX2_AUTO_REPEAT);
}

inline bool IsSpellRequiresRangedAP(SpellEntry const* spellInfo)
{
    return (spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MELEE);
}

SpellCastResult GetErrorAtShapeshiftedCast(SpellEntry const* spellInfo, uint32 form);

inline bool IsChanneledSpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_EX_IS_CHANNELED) || spellInfo->HasAttribute(SPELL_ATTR_EX_IS_SELF_CHANNELED);
}

inline bool IsNeedCastSpellAtFormApply(SpellEntry const* spellInfo, ShapeshiftForm form)
{
    if ((!spellInfo->HasAttribute(SPELL_ATTR_PASSIVE) && !spellInfo->HasAttribute(SPELL_ATTR_DO_NOT_DISPLAY)) || !form)
        return false;

    // passive spells with SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT are already active without shapeshift, do no recast!
    return (spellInfo->Stances[0] & (1 << (form - 1)) && !spellInfo->HasAttribute(SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED));
}

inline bool IsNeedCastSpellAtOutdoor(SpellEntry const* spellInfo)
{
    return (spellInfo->HasAttribute(SPELL_ATTR_ONLY_OUTDOORS) && spellInfo->HasAttribute(SPELL_ATTR_PASSIVE));
}

inline bool IsReflectableSpell(SpellEntry const* spellInfo)
{
    return spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && !spellInfo->HasAttribute(SPELL_ATTR_IS_ABILITY)
           && !spellInfo->HasAttribute(SPELL_ATTR_EX_NO_REFLECTION) && !spellInfo->HasAttribute(SPELL_ATTR_NO_IMMUNITIES)
           && !spellInfo->HasAttribute(SPELL_ATTR_PASSIVE) && !IsPositiveSpell(spellInfo);
}

// Mostly required by spells that target a creature inside GO
inline bool IsIgnoreLosSpell(SpellEntry const* spellInfo)
{
    if (spellInfo->HasAttribute(SPELL_ATTR_EX5_ALWAYS_LINE_OF_SIGHT))
        return false;

    switch (spellInfo->Id)
    {
        case 36795:                                 // Cannon Channel
        case 31628:                                 // Green Beam
        case 31630:                                 // Green Beam
        case 31631:                                 // Green Beam
        case 24742:                                 // Magic Wings
        case 40639:                                 // Arcane Beam - Channel target inside gameobject
        case 42867:                                 // both need LOS, likely TARGET_UNIT should use LOS ignore from normal radius, not per-effect radius WIP
            return true;
        default:
            break;
    }

    return spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LINE_OF_SIGHT);
}

inline bool IsIgnoreLosSpellCast(SpellEntry const* spellInfo)
{
    return spellInfo->rangeIndex == 13 || IsIgnoreLosSpell(spellInfo);
}

inline bool IsIgnoreLosSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex effIdx, bool targetB)
{
    if (spellInfo->HasAttribute(SPELL_ATTR_EX5_ALWAYS_LINE_OF_SIGHT))
        return false;

    // TODO: Move this to target logic
    switch (spellInfo->EffectImplicitTargetA[effIdx])
    {
        case TARGET_UNIT_FRIEND_CHAIN_HEAL: // checked for LOS but in a custom chain way
        case TARGET_UNIT_FRIEND_AND_PARTY:
        case TARGET_UNIT_RAID_AND_CLASS:
        case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE: return true;
        default:
            if (IsCheckCastTarget(targetB ? spellInfo->EffectImplicitTargetB[effIdx] : spellInfo->EffectImplicitTargetA[effIdx]))
                return IsIgnoreLosSpellCast(spellInfo);
            break;
    }

    return spellInfo->EffectRadiusIndex[effIdx] == 28 || IsIgnoreLosSpell(spellInfo);
}

// applied when item is received/looted/equipped
inline bool IsItemAura(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        case 38132:
            return true;
        default:
            return false;
    }
}

inline bool NeedsComboPoints(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_EX_FINISHING_MOVE_DAMAGE) || spellInfo->HasAttribute(SPELL_ATTR_EX_FINISHING_MOVE_DURATION);
}

inline SpellSchoolMask GetSpellSchoolMask(SpellEntry const* spellInfo)
{
    return SpellSchoolMask(spellInfo->SchoolMask);
}

inline uint32 GetSpellMechanicMask(SpellEntry const* spellInfo, uint32 effectMask)
{
    uint32 mask = 0;
    if (spellInfo->Mechanic)
        mask |= 1 << (spellInfo->Mechanic - 1);

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (!(effectMask & (1 << i)))
            continue;

        if (spellInfo->EffectMechanic[i])
            mask |= 1 << (spellInfo->EffectMechanic[i] - 1);
    }

    return mask;
}

inline uint32 GetAllSpellMechanicMask(SpellEntry const* spellInfo)
{
    uint32 mask = 0;
    if (spellInfo->Mechanic)
        mask |= 1 << (spellInfo->Mechanic - 1);
    for (unsigned int i : spellInfo->EffectMechanic)
        if (i)
            mask |= 1 << (i - 1);
    return mask;
}

inline Mechanics GetEffectMechanic(SpellEntry const* spellInfo, SpellEffectIndex effect)
{
    if (spellInfo->EffectMechanic[effect])
        return Mechanics(spellInfo->EffectMechanic[effect]);
    if (spellInfo->Mechanic)
        return Mechanics(spellInfo->Mechanic);
    return MECHANIC_NONE;
}

inline bool IsIgnoreRootSpell(SpellEntry const* spellInfo)
{
    if (!spellInfo->HasAttribute(SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT))
        return false;

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA && spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_CASTER &&
            spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MECHANIC_IMMUNITY && spellInfo->EffectMiscValue[i] == MECHANIC_ROOT)
            return true;

    return false;
}

inline bool IsSpellUseWeaponSkill(SpellEntry const* spellInfo)
{
    // note: this is not a mirror of client function - that function does not work for Bloodthirst edgecase
    return spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON || spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE || spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED && spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT);
}

inline uint32 GetDispellMask(DispelType dispel)
{
    // If dispell all
    if (dispel == DISPEL_ALL)
        return DISPEL_ALL_MASK;
    return (1 << dispel);
}

inline bool IsPartyOrRaidTarget(uint32 target)
{
    switch (target)
    {
        case TARGET_UNIT_FRIEND_NEAR_CASTER:
        case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC:
        case TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC:
        case TARGET_UNIT_PARTY:
        case TARGET_UNIT_FRIEND_AND_PARTY:
        case TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE:
        case TARGET_UNIT_RAID:
        case TARGET_UNIT_RAID_NEAR_CASTER:
        case TARGET_UNIT_RAID_AND_CLASS:
            return true;
        default:
            return false;
    }
}

inline bool IsGroupRestrictedBuff(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        // Soulstone Ressurection - Patch 2.1.0
        case 20707:
        case 20762:
        case 20763:
        case 20764:
        case 20765:
        case 27239:
            return true;
    }

    return false;
}

inline bool IsSimilarAuraEffect(SpellEntry const* entry, uint32 effect, SpellEntry const* entry2, uint32 effect2)
{
    return (entry2->EffectApplyAuraName[effect2] && entry->EffectApplyAuraName[effect] &&
            entry2->Effect[effect2] == entry->Effect[effect] &&
            entry2->EffectApplyAuraName[effect2] == entry->EffectApplyAuraName[effect] &&
            IsPositiveEffect(entry2, SpellEffectIndex(effect2)) == IsPositiveEffect(entry, SpellEffectIndex(effect)));
}

inline bool IsSimilarExistingAuraStronger(const SpellAuraHolder* holder, const SpellAuraHolder* existing)
{
    if (!holder || !existing)
        return false;
    const SpellEntry* entry = holder->GetSpellProto();
    const SpellEntry* entry2 = existing->GetSpellProto();
    if (!entry || !entry2)
        return false;

    // Already compared effects masks to avoid re-entrance
    uint32 effectmask1 = 0;
    uint32 effectmask2 = 0;

    for (uint32 e = EFFECT_INDEX_0; e < MAX_EFFECT_INDEX; ++e)
    {
        for (uint32 e2 = EFFECT_INDEX_0; e2 < MAX_EFFECT_INDEX; ++e2)
        {
            if (IsSimilarAuraEffect(entry, e, entry2, e2) && !(effectmask1 & (1 << e)) && !(effectmask2 & (1 << e2)))
            {
                effectmask1 |= (1 << e);
                effectmask2 |= (1 << e2);
                Aura* aura1 = holder->GetAuraByEffectIndex(SpellEffectIndex(e));
                Aura* aura2 = existing->GetAuraByEffectIndex(SpellEffectIndex(e2));
                int32 value = aura1 ? (aura1->GetModifier()->m_amount / int32(aura1->GetStackAmount())) : 0;
                int32 value2 = aura2 ? (aura2->GetModifier()->m_amount / int32(aura2->GetStackAmount())) : 0;
                if (value < 0 && value2 < 0)
                {
                    value = abs(value);
                    value2 = abs(value2);
                }
                if (value2 > value)
                    return true;
            }
        }
    }
    return false;
}

inline bool IsSimilarExistingAuraStronger(const Unit* caster, const SpellEntry* entry, const SpellAuraHolder* existing, uint32 affectedMask, int32 amounts[MAX_EFFECT_INDEX])
{
    if (!caster || !existing)
        return false;
    const SpellEntry* entry2 = existing->GetSpellProto();
    if (!entry || !entry2)
        return false;

    // Already compared effects masks to avoid re-entrance
    uint32 effectmask1 = 0;
    uint32 effectmask2 = 0;

    for (uint32 e = EFFECT_INDEX_0; e < MAX_EFFECT_INDEX; ++e)
    {
        if (affectedMask & (1 << e))
        {
            for (uint32 e2 = EFFECT_INDEX_0; e2 < MAX_EFFECT_INDEX; ++e2)
            {
                if (IsSimilarAuraEffect(entry, e, entry2, e2) && !(effectmask1 & (1 << e)) && !(effectmask2 & (1 << e2)))
                {
                    effectmask1 |= (1 << e);
                    effectmask2 |= (1 << e2);
                    Aura* aura = existing->GetAuraByEffectIndex(SpellEffectIndex(e2));
                    int32 value = amounts[e];
                    int32 value2 = aura ? (aura->GetModifier()->m_amount / int32(aura->GetStackAmount())) : 0;
                    if (value < 0 && value2 < 0)
                    {
                        value = abs(value);
                        value2 = abs(value2);
                    }
                    if (value2 > value)
                        return true;
                }
            }
        }
    }
    return false;
}

// Diminishing Returns interaction with spells
DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellEntry const* spellproto, bool triggered);
bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group);
bool IsDiminishingReturnsGroupDurationDiminished(DiminishingGroup group, bool pvp);
DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group);
int32 GetDiminishingReturnsLimitDuration(DiminishingGroup group, SpellEntry const* spellproto);
bool IsSubjectToDiminishingLevels(DiminishingGroup group, bool pvp);
bool IsCreatureDRSpell(SpellEntry const* spellInfo);

SpellEntry const* GetSpellEntryByDifficulty(uint32 id, Difficulty difficulty, bool isRaid);

// Spell proc event related declarations (accessed using SpellMgr functions)
enum ProcFlags : uint32
{
    PROC_FLAG_NONE                          = 0x00000000,

    PROC_FLAG_HEARTBEAT                     = 0x00000001,   // 00 Heartbeat
    PROC_FLAG_KILL                          = 0x00000002,   // 01 Kill target (in most cases need XP/Honor reward, see Unit::IsTriggeredAtSpellProcEvent for additinoal check)

    PROC_FLAG_DEAL_MELEE_SWING              = 0x00000004,   // 02 Successful melee auto attack
    PROC_FLAG_TAKE_MELEE_SWING              = 0x00000008,   // 03 Taken damage from melee auto attack hit

    PROC_FLAG_DEAL_MELEE_ABILITY            = 0x00000010,   // 04 Successful attack by Spell that use melee weapon
    PROC_FLAG_TAKE_MELEE_ABILITY            = 0x00000020,   // 05 Taken damage by Spell that use melee weapon

    PROC_FLAG_DEAL_RANGED_ATTACK            = 0x00000040,   // 06 Successful Ranged auto attack
    PROC_FLAG_TAKE_RANGED_ATTACK            = 0x00000080,   // 07 Taken damage from ranged auto attack

    PROC_FLAG_DEAL_RANGED_ABILITY           = 0x00000100,   // 08 Successful Ranged attack by Spell that use ranged weapon
    PROC_FLAG_TAKE_RANGED_ABILITY           = 0x00000200,   // 09 Taken damage by Spell that use ranged weapon

    PROC_FLAG_DEAL_HELPFUL_ABILITY          = 0x00000400,   // 10 Done positive spell that has dmg class none
    PROC_FLAG_TAKE_HELPFUL_ABILITY          = 0x00000800,   // 11 Taken positive spell that has dmg class none

    PROC_FLAG_DEAL_HARMFUL_ABILITY          = 0x00001000,   // 12 Done negative spell that has dmg class none
    PROC_FLAG_TAKE_HARMFUL_ABILITY          = 0x00002000,   // 13 Taken negative spell that has dmg class none

    PROC_FLAG_DEAL_HELPFUL_SPELL            = 0x00004000,   // 14 Successful cast positive spell (by default only on healing)
    PROC_FLAG_TAKE_HELPFUL_SPELL            = 0x00008000,   // 15 Taken positive spell hit (by default only on healing)

    PROC_FLAG_DEAL_HARMFUL_SPELL            = 0x00010000,   // 16 Successful negative spell cast (by default only on damage)
    PROC_FLAG_TAKE_HARMFUL_SPELL            = 0x00020000,   // 17 Taken negative spell (by default only on damage)

    PROC_FLAG_DEAL_HARMFUL_PERIODIC         = 0x00040000,   // 18 Successful do periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)
    PROC_FLAG_TAKE_HARMFUL_PERIODIC         = 0x00080000,   // 19 Taken spell periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)

    PROC_FLAG_TAKE_ANY_DAMAGE               = 0x00100000,   // 20 Taken any damage
    PROC_FLAG_ON_TRAP_ACTIVATION            = 0x00200000,   // 21 On trap activation - different from enumerated strings - likely reuse

    PROC_FLAG_MAIN_HAND_WEAPON_SWING        = 0x00400000,   // 22 Successful main-hand melee attacks
    PROC_FLAG_OFF_HAND_WEAPON_SWING         = 0x00800000,   // 23 Successful off-hand melee attacks

    PROC_FLAG_DEATH                         = 0x01000000,   // 24 On death by any means
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_DEAL_MELEE_SWING        | \
                                  PROC_FLAG_TAKE_MELEE_SWING             | \
                                  PROC_FLAG_DEAL_MELEE_ABILITY  | \
                                  PROC_FLAG_TAKE_MELEE_ABILITY       | \
                                  PROC_FLAG_DEAL_RANGED_ATTACK       | \
                                  PROC_FLAG_TAKE_RANGED_ABILITY            | \
                                  PROC_FLAG_DEAL_RANGED_ABILITY | \
                                  PROC_FLAG_TAKE_RANGED_ABILITY)

#define NEGATIVE_TRIGGER_MASK (MELEE_BASED_TRIGGER_MASK                | \
                               PROC_FLAG_DEAL_HARMFUL_ABILITY      | \
                               PROC_FLAG_TAKE_HARMFUL_ABILITY           | \
                               PROC_FLAG_DEAL_HARMFUL_SPELL | \
                               PROC_FLAG_TAKE_HARMFUL_SPELL)

#define SPELL_CAST_TRIGGER_MASK (PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT    | \
                                 PROC_FLAG_SUCCESSFUL_RANGED_HIT         | \
                                 PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT   | \
                                 PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | \
                                 PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG | \
                                 PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS| \
                                 PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG)
enum ProcFlagsEx
{
    PROC_EX_NONE                = 0x0000000,                // If none can tigger on Hit/Crit only (passive spells MUST defined by SpellFamily flag)
    PROC_EX_NORMAL_HIT          = 0x0000001,                // If set only from normal hit (only damage spells)
    PROC_EX_CRITICAL_HIT        = 0x0000002,
    PROC_EX_MISS                = 0x0000004,
    PROC_EX_RESIST              = 0x0000008,
    PROC_EX_DODGE               = 0x0000010,
    PROC_EX_PARRY               = 0x0000020,
    PROC_EX_BLOCK               = 0x0000040,
    PROC_EX_EVADE               = 0x0000080,
    PROC_EX_IMMUNE              = 0x0000100,
    PROC_EX_DEFLECT             = 0x0000200,
    PROC_EX_ABSORB              = 0x0000400,
    PROC_EX_REFLECT             = 0x0000800,
    PROC_EX_INTERRUPT           = 0x0001000,                // melee hit result can be Interrupt (not used)
    PROC_EX_FULL_BLOCK          = 0x0002000,                // block al attack damage
    PROC_EX_RESERVED2           = 0x0004000,
    PROC_EX_RESERVED3           = 0x0008000,
    PROC_EX_EX_TRIGGER_ON_NO_DAMAGE = 0x0010000,            // if set, hits trigger even if no damage/healing is dealt
    PROC_EX_EX_ONE_TIME_TRIGGER = 0x0020000,                // if set trigger always but only one time (not used)
    PROC_EX_PERIODIC_POSITIVE   = 0x0040000,                // for periodic heal
    PROC_EX_CAST_END            = 0x0080000,                // procs on end of cast
    PROC_EX_MAGNET              = 0x0100000,                // for grounding totem hit

    // Flags for internal use - do not use these in db!
    PROC_EX_INTERNAL_HOT        = 0x2000000
};

struct SpellProcEventEntry
{
    uint32      schoolMask;                                 // if nonzero - bit mask for matching proc condition based on spell candidate's school: Fire=2, Mask=1<<(2-1)=2
    uint32      spellFamilyName;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyNamer value
    ClassFamilyMask spellFamilyMask[MAX_EFFECT_INDEX];      // if nonzero - for matching proc condition based on candidate spell's SpellFamilyFlags  (like auras 107 and 108 do)
    uint32      procFlags;                                  // bitmask for matching proc event
    uint32      procEx;                                     // proc Extend info (see ProcFlagsEx)
    float       ppmRate;                                    // for melee (ranged?) damage spells - proc rate per minute. if zero, falls back to flat chance from Spell.dbc
    float       customChance;                               // Owerride chance (in most cases for debug only)
    uint32      cooldown;                                   // hidden cooldown used for some spell proc events, applied to _triggered_spell_
};

typedef std::unordered_map<uint32, SpellProcEventEntry> SpellProcEventMap;

#define ELIXIR_BATTLE_MASK    0x01
#define ELIXIR_GUARDIAN_MASK  0x02
#define ELIXIR_FLASK_MASK     (ELIXIR_BATTLE_MASK|ELIXIR_GUARDIAN_MASK)
#define ELIXIR_UNSTABLE_MASK  0x04
#define ELIXIR_SHATTRATH_MASK 0x08
#define ELIXIR_WELL_FED       0x10                          // Some foods have SPELLFAMILY_POTION

struct SpellThreatEntry
{
    uint16 threat;
    float multiplier;
    float ap_bonus;
};

typedef std::map<uint32, uint8> SpellElixirMap;
typedef std::map<uint32, float> SpellProcItemEnchantMap;
typedef std::map<uint32, SpellThreatEntry> SpellThreatMap;

// Spell script target related declarations (accessed using SpellMgr functions)
enum SpellTargetType
{
    SPELL_TARGET_TYPE_GAMEOBJECT    = 0,
    SPELL_TARGET_TYPE_CREATURE      = 1,
    SPELL_TARGET_TYPE_DEAD          = 2,
    SPELL_TARGET_TYPE_CREATURE_GUID = 3, // obsolete - use string id instead
    SPELL_TARGET_TYPE_GAMEOBJECT_GUID = 4, // obsolete - use string id instead
    SPELL_TARGET_TYPE_STRING_ID     = 5,
};

#define MAX_SPELL_TARGET_TYPE 6

// pre-defined targeting for spells
struct SpellTargetEntry
{
    uint32 spellId;
    uint32 type;
    uint32 targetEntry;
    uint32 inverseEffectMask;

    bool CanNotHitWithSpellEffect(SpellEffectIndex effect) const { return (inverseEffectMask & (1 << effect)) != 0; }
};

// coordinates for spells (accessed using SpellMgr functions)
struct SpellTargetPosition
{
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

struct SpellCone
{
    uint32 spellId;
    int32 coneAngle;
};

typedef std::unordered_map<uint32, SpellTargetPosition> SpellTargetPositionMap;

// Spell pet auras
class PetAura
{
    public:
        PetAura() :
            removeOnChangePet(false),
            damage(0)
        {
        }

        PetAura(uint32 petEntry, uint32 aura, bool _removeOnChangePet, int _damage) :
            removeOnChangePet(_removeOnChangePet), damage(_damage)
        {
            auras[petEntry] = aura;
        }

        uint32 GetAura(uint32 petEntry) const
        {
            std::map<uint32, uint32>::const_iterator itr = auras.find(petEntry);
            if (itr != auras.end())
                return itr->second;
            std::map<uint32, uint32>::const_iterator itr2 = auras.find(0);
            if (itr2 != auras.end())
                return itr2->second;
            return 0;
        }

        void AddAura(uint32 petEntry, uint32 aura)
        {
            auras[petEntry] = aura;
        }

        bool IsRemovedOnChangePet() const
        {
            return removeOnChangePet;
        }

        int32 GetDamage() const
        {
            return damage;
        }

    private:
        std::map<uint32, uint32> auras;
        bool removeOnChangePet;
        int32 damage;
};
typedef std::map<uint32, PetAura> SpellPetAuraMap;

struct SpellArea
{
    uint32 spellId;
    uint32 areaId;                                          // zone/subzone/or 0 is not limited to zone
    uint32 questStart;                                      // quest start (quest must be active or rewarded for spell apply)
    uint32 questEnd;                                        // quest end (quest don't must be rewarded for spell apply)
    uint16 conditionId;                                     // conditionId - will replace questStart, questEnd, raceMask, gender and questStartCanActive
    int32  auraSpell;                                       // spell aura must be applied for spell apply )if positive) and it don't must be applied in other case
    uint32 raceMask;                                        // can be applied only to races
    Gender gender;                                          // can be applied only to gender
    bool questStartCanActive;                               // if true then quest start can be active (not only rewarded)
    bool autocast;                                          // if true then auto applied at area enter, in other case just allowed to cast

    // helpers
    bool IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const;
    void ApplyOrRemoveSpellIfCan(Player* player, uint32 newZone, uint32 newArea, bool onlyApply) const;
};

typedef std::multimap<uint32 /*applySpellId*/, SpellArea> SpellAreaMap;
typedef std::multimap<uint32 /*auraSpellId*/, SpellArea const*> SpellAreaForAuraMap;
typedef std::multimap<uint32 /*areaOrZoneId*/, SpellArea const*> SpellAreaForAreaMap;
typedef std::pair<SpellAreaMap::const_iterator, SpellAreaMap::const_iterator> SpellAreaMapBounds;
typedef std::pair<SpellAreaForAuraMap::const_iterator, SpellAreaForAuraMap::const_iterator>  SpellAreaForAuraMapBounds;
typedef std::pair<SpellAreaForAreaMap::const_iterator, SpellAreaForAreaMap::const_iterator>  SpellAreaForAreaMapBounds;

// Spell rank chain  (accessed using SpellMgr functions)
struct SpellChainNode
{
    uint32 prev;
    uint32 first;
    uint32 req;
    uint8  rank;
};

typedef std::unordered_map<uint32, SpellChainNode> SpellChainMap;
typedef std::multimap<uint32, uint32> SpellChainMapNext;

// Spell learning properties (accessed using SpellMgr functions)
struct SpellLearnSkillNode
{
    uint16 skill;
    uint16 step;
    SpellEffects effect;
};

typedef std::map<uint32, SpellLearnSkillNode> SpellLearnSkillMap;

struct SpellLearnSpellNode
{
    uint32 spell;
    bool active;                                            // show in spellbook or not
    bool autoLearned;
};

typedef std::multimap<uint32, SpellLearnSpellNode> SpellLearnSpellMap;
typedef std::pair<SpellLearnSpellMap::const_iterator, SpellLearnSpellMap::const_iterator> SpellLearnSpellMapBounds;

typedef std::multimap<uint32, SkillLineAbilityEntry const*> SkillLineAbilityMap;
typedef std::pair<SkillLineAbilityMap::const_iterator, SkillLineAbilityMap::const_iterator> SkillLineAbilityMapBounds;

typedef std::multimap<uint32, SkillRaceClassInfoEntry const*> SkillRaceClassInfoMap;
typedef std::pair<SkillRaceClassInfoMap::const_iterator, SkillRaceClassInfoMap::const_iterator> SkillRaceClassInfoMapBounds;

typedef std::multimap<uint32, uint32> PetLevelupSpellSet;
typedef std::map<uint32, PetLevelupSpellSet> PetLevelupSpellMap;

struct PetDefaultSpellsEntry
{
    PetDefaultSpellsEntry()
    {

    }

    std::vector<uint32> spellid;
};

// < 0 for petspelldata id, > 0 for creature_id
typedef std::map<int32, PetDefaultSpellsEntry> PetDefaultSpellsMap;

bool IsPrimaryProfessionSkill(uint32 skill);

inline bool IsProfessionSkill(uint32 skill)
{
    return  IsPrimaryProfessionSkill(skill) || skill == SKILL_FISHING || skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
}

inline bool IsProfessionOrRidingSkill(uint32 skill)
{
    return  IsProfessionSkill(skill) || skill == SKILL_RIDING;
}

class SpellMgr
{
        friend struct DoSpellProcEvent;
        friend struct DoSpellProcItemEnchant;

        // Constructors
    public:
        SpellMgr();
        ~SpellMgr();

        // Accessors (const or static functions)
    public:

        SpellElixirMap const& GetSpellElixirMap() const { return mSpellElixirs; }

        uint32 GetSpellElixirMask(uint32 spellid) const
        {
            SpellElixirMap::const_iterator itr = mSpellElixirs.find(spellid);
            if (itr == mSpellElixirs.end())
                return 0x0;

            return itr->second;
        }

        // Reverse engineered from binary: do not alter
        static inline bool IsSpellTargetHarmfulAtClient(uint32 target)
        {
            switch (target)
            {
                case TARGET_UNIT_ENEMY_NEAR_CASTER:
                case TARGET_UNIT_ENEMY:
                case TARGET_ENUM_UNITS_ENEMY_AOE_AT_SRC_LOC:
                case TARGET_ENUM_UNITS_ENEMY_AOE_AT_DEST_LOC:
                case TARGET_ENUM_UNITS_ENEMY_IN_CONE_24:
                case TARGET_ENUM_UNITS_ENEMY_AOE_AT_DYNOBJ_LOC:
                case TARGET_LOCATION_CASTER_TARGET_POSITION:
                case TARGET_ENUM_UNITS_ENEMY_IN_CONE_54:
                case TARGET_CORPSE_ENEMY_NEAR_CASTER:
                    return true;
                default:
                    return false;
            }
        }

        // Reverse engineered from binary: do not alter
        static inline bool IsSpellTargetHelpfulAtClient(uint32 target)
        {
            switch (target)
            {
                case TARGET_UNIT_CASTER:
                case TARGET_UNIT_FRIEND_NEAR_CASTER:
                case TARGET_UNIT_NEAR_CASTER:
                case TARGET_UNIT_CASTER_PET:
                case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
                case TARGET_UNIT_FRIEND:
                case TARGET_UNIT_CASTER_MASTER:
                case TARGET_ENUM_UNITS_FRIEND_AOE_AT_DYNOBJ_LOC:
                case TARGET_ENUM_UNITS_FRIEND_AOE_AT_SRC_LOC:
                case TARGET_ENUM_UNITS_FRIEND_AOE_AT_DEST_LOC:
                case TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC:
                case TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC:
                case TARGET_UNIT_PARTY:
                case TARGET_UNIT_FRIEND_CHAIN_HEAL:
                case TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE:
                case TARGET_UNIT_RAID:
                case TARGET_UNIT_RAID_NEAR_CASTER:
                case TARGET_ENUM_UNITS_FRIEND_IN_CONE:
                case TARGET_UNIT_RAID_AND_CLASS:
                case TARGET_PLAYER_RAID_NYI:
                    return true;
                default:
                    return false;
            }
        }

        // Reverse engineered from binary: do not alter
        static inline bool IsSpellEffectHarmfulAtClient(const SpellEntry &entry, SpellEffectIndex effIndex)
        {
            return (IsSpellTargetHarmfulAtClient(entry.EffectImplicitTargetA[effIndex]) || IsSpellTargetHarmfulAtClient(entry.EffectImplicitTargetB[effIndex]));
        }

        // Reverse engineered from binary: do not alter
        static inline bool IsSpellEffectHelpfulAtClient(const SpellEntry &entry, SpellEffectIndex effIndex)
        {
            if (IsSpellTargetHelpfulAtClient(entry.EffectImplicitTargetA[effIndex]))
            {
                if (entry.EffectImplicitTargetA[effIndex] != TARGET_UNIT_CASTER || entry.EffectApplyAuraName[effIndex] != SPELL_AURA_DUMMY)
                    return true;
            }

            if (IsSpellTargetHelpfulAtClient(entry.EffectImplicitTargetB[effIndex]))
            {
                if (entry.EffectImplicitTargetB[effIndex] != TARGET_UNIT_CASTER || entry.EffectApplyAuraName[effIndex] != SPELL_AURA_DUMMY)
                    return true;
            }

            return false;
        }

        // Reverse engineered from binary: do not alter
        enum SpellFaction
        {
            SPELL_NEUTRAL = 0,
            SPELL_HELPFUL = 1,
            SPELL_HARMFUL = 2,
        };

        // Reverse engineered from binary: do not alter
        static inline SpellFaction GetSpellFactionAtClient(const SpellEntry &entry, SpellEffectIndexMask mask = EFFECT_MASK_ALL)
        {
            if (entry.Targets & TARGET_FLAG_UNIT_ALLY)
                return SPELL_HELPFUL;

            if (entry.Targets & TARGET_FLAG_UNIT_ENEMY)
                return SPELL_HARMFUL;

            for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                // Customization: skip junk, skip by mask
                if (!entry.Effect[i] || !(mask & (1 << i)))
                    continue;

                if (IsSpellEffectHarmfulAtClient(entry, SpellEffectIndex(i)))
                    return SPELL_HARMFUL;

                if (IsSpellEffectHelpfulAtClient(entry, SpellEffectIndex(i)))
                    return SPELL_HELPFUL;
            }

            return SPELL_NEUTRAL;
        }

        SpellThreatEntry const* GetSpellThreatEntry(uint32 spellid) const
        {
            SpellThreatMap::const_iterator itr = mSpellThreatMap.find(spellid);
            if (itr != mSpellThreatMap.end())
                return &itr->second;

            return nullptr;
        }

        float GetSpellThreatMultiplier(SpellEntry const* spellInfo) const
        {
            if (!spellInfo)
                return 1.0f;

            if (SpellThreatEntry const* entry = GetSpellThreatEntry(spellInfo->Id))
                return entry->multiplier;

            return 1.0f;
        }

        // Spell proc events
        SpellProcEventEntry const* GetSpellProcEvent(uint32 spellId) const
        {
            SpellProcEventMap::const_iterator itr = mSpellProcEventMap.find(spellId);
            if (itr != mSpellProcEventMap.end())
                return &itr->second;
            return nullptr;
        }

        // Spell procs from item enchants
        float GetItemEnchantProcChance(uint32 spellid) const
        {
            SpellProcItemEnchantMap::const_iterator itr = mSpellProcItemEnchantMap.find(spellid);
            if (itr == mSpellProcItemEnchantMap.end())
                return 0.0f;

            return itr->second;
        }

        static bool IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellEntry const* spellInfo, uint32 procFlags, uint32 procExtra);

        // Spell target coordinates
        SpellTargetPosition const* GetSpellTargetPosition(uint32 spell_id) const
        {
            SpellTargetPositionMap::const_iterator itr = mSpellTargetPositions.find(spell_id);
            if (itr != mSpellTargetPositions.end())
                return &itr->second;
            return nullptr;
        }

        // Spell ranks chains
        SpellChainNode const* GetSpellChainNode(uint32 spell_id) const
        {
            SpellChainMap::const_iterator itr = mSpellChains.find(spell_id);
            if (itr == mSpellChains.end())
                return nullptr;

            return &itr->second;
        }

        uint32 GetFirstSpellInChain(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->first;

            return spell_id;
        }

        uint32 GetPrevSpellInChain(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->prev;

            return 0;
        }

        uint32 GetNextSpellInChain(uint32 spell_id) const
        {
            SpellChainMapNext const& nextMap = GetSpellChainNext();

            for (SpellChainMapNext::const_iterator itr = nextMap.lower_bound(spell_id); itr != nextMap.upper_bound(spell_id); ++itr)
            {
                SpellChainNode const* node = GetSpellChainNode(itr->second);

                // If next spell is a requirement for this one then skip it
                if (node->req == spell_id)
                    continue;

                if (node->prev == spell_id)
                    return itr->second;
            }

            return 0;
        }

        SpellChainMapNext const& GetSpellChainNext() const { return mSpellChainsNext; }

        template<typename Worker>
        void doForHighRanks(uint32 spellid, Worker& worker)
        {
            SpellChainMapNext const& nextMap = GetSpellChainNext();
            for (SpellChainMapNext::const_iterator itr = nextMap.lower_bound(spellid); itr != nextMap.upper_bound(spellid); ++itr)
            {
                worker(itr->second);
                doForHighRanks(itr->second, worker);
            }
        }

        // Note: not use rank for compare to spell ranks: spell chains isn't linear order
        // Use IsSpellHigherRankOfSpell instead
        uint8 GetSpellRank(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->rank;

            return 0;
        }

        bool IsSpellHigherRankOfSpell(uint32 spellId1, uint32 spellId2) const
        {
            if (spellId1 == spellId2)
                return false;

            SpellChainMap::const_iterator itr = mSpellChains.find(spellId1);

            uint32 rank2 = GetSpellRank(spellId2);

            // not ordered correctly by rank value
            if (itr == mSpellChains.end() || !rank2 || itr->second.rank <= rank2)
                return false;

            // check present in same rank chain
            for (; itr != mSpellChains.end(); itr = mSpellChains.find(itr->second.prev))
                if (itr->second.prev == spellId2)
                    return true;

            return false;
        }

        inline bool IsSpellAnotherRankOfSpell(uint32 spellId1, uint32 spellId2) const
        {
            return (spellId1 != spellId2 && GetFirstSpellInChain(spellId1) == GetFirstSpellInChain(spellId2));
        }

        bool IsSingleTargetSpell(SpellEntry const* entry) const
        {
            if (entry->HasAttribute(SPELL_ATTR_EX5_LIMIT_N))
                return true;

            // single target triggered spell.
            // Not real client side single target spell, but it' not triggered until prev. aura expired.
            // This is allow store it in single target spells list for caster for spell proc checking
            if (entry->Id == 38324)                             // Regeneration (triggered by 38299 (HoTs on Heals))
                return true;

            return false;
        }

        bool IsSingleTargetSpells(SpellEntry const* entry1, SpellEntry const* entry2) const
        {
            if (!IsSingleTargetSpell(entry1) || !IsSingleTargetSpell(entry2))
                return false;

            // Early instance of same spell check
            if (entry1 == entry2 || entry1->Id == entry2->Id)
                return true;

            // One spell is a rank of another spell
            if (IsSpellAnotherRankOfSpell(entry1->Id, entry2->Id))
                return true;

            // Experimental: Try to detect spinoffs of specific family spells (e.g. polymorph flavors)
            if (entry1->SpellFamilyName != SPELLFAMILY_GENERIC && entry2->SpellFamilyName != SPELLFAMILY_GENERIC)
            {
                if (!entry1->SpellFamilyFlags.Empty() && !entry2->SpellFamilyFlags.Empty())
                    return entry1->IsFitToFamily(SpellFamily(entry2->SpellFamilyName), entry2->SpellFamilyFlags);
            }

            return false;
        }

        uint32 GetSpellBookSuccessorSpellId(uint32 spellId)
        {
            SkillLineAbilityMapBounds bounds = GetSkillLineAbilityMapBoundsBySpellId(spellId);
            for (SkillLineAbilityMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                if (SkillLineAbilityEntry const* pAbility = itr->second)
                {
                    if (pAbility->forward_spellid)
                        return pAbility->forward_spellid;
                }
            }
            return 0;
        }

        // return true if spell1 can affect spell2
        bool IsSpellCanAffectSpell(SpellEntry const* spellInfo_1, SpellEntry const* spellInfo_2) const;

        SpellEntry const* SelectAuraRankForLevel(SpellEntry const* spellInfo, uint32 level) const;

        // Spell learning
        SpellLearnSkillNode const* GetSpellLearnSkill(uint32 spell_id) const
        {
            SpellLearnSkillMap::const_iterator itr = mSpellLearnSkills.find(spell_id);
            if (itr != mSpellLearnSkills.end())
                return &itr->second;
            return nullptr;
        }

        bool IsSpellLearnSpell(uint32 spell_id) const
        {
            return mSpellLearnSpells.find(spell_id) != mSpellLearnSpells.end();
        }

        SpellLearnSpellMapBounds GetSpellLearnSpellMapBounds(uint32 spell_id) const
        {
            return mSpellLearnSpells.equal_range(spell_id);
        }

        bool IsSpellLearnToSpell(uint32 parent, uint32 child) const
        {
            SpellLearnSpellMapBounds bounds = GetSpellLearnSpellMapBounds(parent);
            for (SpellLearnSpellMap::const_iterator i = bounds.first; i != bounds.second; ++i)
                if (i->second.spell == child)
                    return true;
            return false;
        }

        static bool IsProfessionOrRidingSpell(uint32 spellId);
        static bool IsProfessionSpell(uint32 spellId);
        static bool IsPrimaryProfessionSpell(uint32 spellId);
        bool IsPrimaryProfessionFirstRankSpell(uint32 spellId) const;
        uint32 GetProfessionSpellMinLevel(uint32 spellId) const;

        bool IsSkillBonusSpell(uint32 spellId) const;

        // Spell correctness for client using
        static bool IsSpellValid(SpellEntry const* spellInfo, Player* pl = nullptr, bool msg = true);

        SkillLineAbilityMapBounds GetSkillLineAbilityMapBoundsBySpellId(uint32 spellId) const
        {
            return mSkillLineAbilityMapBySpellId.equal_range(spellId);
        }

        SkillLineAbilityMapBounds GetSkillLineAbilityMapBoundsBySkillId(uint32 skillId) const
        {
            return mSkillLineAbilityMapBySkillId.equal_range(skillId);
        }

        SkillRaceClassInfoMapBounds GetSkillRaceClassInfoMapBounds(uint32 skill_id) const
        {
            return mSkillRaceClassInfoMap.equal_range(skill_id);
        }

        PetAura const* GetPetAura(uint32 spell_id, SpellEffectIndex eff)
        {
            SpellPetAuraMap::const_iterator itr = mSpellPetAuraMap.find((spell_id << 8) + eff);
            if (itr != mSpellPetAuraMap.end())
                return &itr->second;
            return nullptr;
        }

        PetLevelupSpellSet const* GetPetLevelupSpellList(uint32 petFamily) const
        {
            PetLevelupSpellMap::const_iterator itr = mPetLevelupSpellMap.find(petFamily);
            if (itr != mPetLevelupSpellMap.end())
                return &itr->second;
            return nullptr;
        }

        // < 0 for petspelldata id, > 0 for creature_id
        PetDefaultSpellsEntry const* GetPetDefaultSpellsEntry(int32 id) const
        {
            PetDefaultSpellsMap::const_iterator itr = mPetDefaultSpellsMap.find(id);
            if (itr != mPetDefaultSpellsMap.end())
                return &itr->second;
            return nullptr;
        }

        SpellCastResult GetSpellAllowedInLocationError(SpellEntry const* spellInfo, uint32 map_id, uint32 zone_id, uint32 area_id, Player const* player = nullptr) const;

        SpellAreaMapBounds GetSpellAreaMapBounds(uint32 spell_id) const
        {
            return mSpellAreaMap.equal_range(spell_id);
        }

        SpellAreaForAuraMapBounds GetSpellAreaForAuraMapBounds(uint32 spell_id) const
        {
            return mSpellAreaForAuraMap.equal_range(spell_id);
        }

        SpellAreaForAreaMapBounds GetSpellAreaForAreaMapBounds(uint32 area_id) const
        {
            return mSpellAreaForAreaMap.equal_range(area_id);
        }

        // Modifiers
    public:
        static SpellMgr& Instance();

        void CheckUsedSpells(char const* table) const;

        // Loading data at server startup
        void LoadSpellChains();
        void LoadSpellLearnSkills();
        void LoadSpellLearnSpells();
        void LoadSpellScriptTarget();
        void LoadSpellElixirs();
        void LoadSpellProcEvents();
        void LoadSpellProcItemEnchant();
        void LoadSpellTargetPositions();
        void LoadSpellThreats();
        void LoadSkillLineAbilityMaps();
        void LoadSkillRaceClassInfoMap();
        void LoadSpellPetAuras();
        void LoadPetLevelupSpellMap();
        void LoadPetDefaultSpells();
        void LoadSpellAreas();

    private:
        bool LoadPetDefaultSpells_helper(CreatureInfo const* cInfo, PetDefaultSpellsEntry& petDefSpells);

        SpellChainMap      mSpellChains;
        SpellChainMapNext  mSpellChainsNext;
        SpellLearnSkillMap mSpellLearnSkills;
        SpellLearnSpellMap mSpellLearnSpells;
        SpellTargetPositionMap mSpellTargetPositions;
        SpellElixirMap     mSpellElixirs;
        SpellThreatMap     mSpellThreatMap;
        SpellProcEventMap  mSpellProcEventMap;
        SpellProcItemEnchantMap mSpellProcItemEnchantMap;
        SkillLineAbilityMap mSkillLineAbilityMapBySpellId;
        SkillLineAbilityMap mSkillLineAbilityMapBySkillId;
        SkillRaceClassInfoMap mSkillRaceClassInfoMap;
        SpellPetAuraMap     mSpellPetAuraMap;
        PetLevelupSpellMap  mPetLevelupSpellMap;
        PetDefaultSpellsMap mPetDefaultSpellsMap;           // only spells not listed in related mPetLevelupSpellMap entry
        SpellAreaMap         mSpellAreaMap;
        SpellAreaForAuraMap  mSpellAreaForAuraMap;
        SpellAreaForAreaMap  mSpellAreaForAreaMap;
};

#define sSpellMgr SpellMgr::Instance()
#endif
