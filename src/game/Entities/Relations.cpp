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

#include "Server/DBCStores.h"
#include "Entities/Unit.h"
#include "Entities/Player.h"
#include "Entities/Corpse.h"
#include "Entities/GameObject.h"
#include "Entities/DynamicObject.h"
#include "Entities/Vehicle.h"
#include "Globals/ObjectMgr.h"
#include "Tools/Formulas.h"

/////////////////////////////////////////////////
/// @file       Relations.cpp
/// @date       September, 2017
/// @brief      Relations API for supported Entities.
///
/// Relations API controls various interactions between entities, such as friendliness or hostility.
///
/// Relations API is split into three tiers:
///
/// <b>Tier 1</b> is a direct reverse engineered gameplay logic from the game.
/// It should never be modified with custom logic to be in sync with the client we aim to support.
/// Each function presented here has a counterpart in client for comparision.
///
/// <b>Tier 2</b> is a server-side extension to plug holes left by original client-side perspective.
/// It builds up on previous tier for entities not represented client-side.
/// All functions presented here are required to mimic the look and feel of the original API.
/// No functions in this tier have a real client-side counterpart.
///
/// <b>Tier 3</b> is a custom server-side convinience API for CMaNGOS.
/// This is a stylistically relaxed set of custom wrappers and helpers for various subsystems.
/// All functions presented in this tier are exclusive to the emulator and have no outside influence.
///
/// Tier 1 is implied to be "set in stone" as it comes from 1st hand source - the game itself.
/// The only reason Tier 1 API should be ever modified is to fix possible mistakes in reverse engineered code.
/// Any user modifications or additions to Tier 1 which are not coming from client's code should be rejected to preserve overall integrity.
///
/// Tiers 2 and 3 are serverside APIs and will be extended in the future as demand arises during actual rollout.
/////////////////////////////////////////////////

/*##########################
########            ########
########   TIER 1   ########
########            ########
##########################*/

/////////////////////////////////////////////////
/// Get faction template to faction tenplate reaction
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>static function (original symbol name unknown)</tt>
/////////////////////////////////////////////////
static inline ReputationRank GetFactionReaction(FactionTemplateEntry const* thisTemplate, FactionTemplateEntry const* otherTemplate)
{
    // Simple sanity check
    if (!thisTemplate || !otherTemplate)
        return REP_NEUTRAL;

    // Original logic begins

    if (otherTemplate->factionGroupMask & thisTemplate->enemyGroupMask)
        return REP_HOSTILE;

    if (thisTemplate->enemyFaction[0] && otherTemplate->faction)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (thisTemplate->enemyFaction[i] == otherTemplate->faction)
                return REP_HOSTILE;
        }
    }

    if (otherTemplate->factionGroupMask & thisTemplate->friendGroupMask)
        return REP_FRIENDLY;

    if (thisTemplate->friendFaction[0] && otherTemplate->faction)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (thisTemplate->friendFaction[i] == otherTemplate->faction)
                return REP_FRIENDLY;
        }
    }

    if (thisTemplate->factionGroupMask & otherTemplate->friendGroupMask)
        return REP_FRIENDLY;

    if (otherTemplate->friendFaction[0] && thisTemplate->faction)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (otherTemplate->friendFaction[i] == thisTemplate->faction)
                return REP_FRIENDLY;
        }
    }
    return (thisTemplate->factionFlags & FACTION_TEMPLATE_FLAG_NEUTRAL_AGGRESSIVE) ? REP_HOSTILE : REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// Get faction template to unit reaction
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>static CGUnit_C::UnitReaction(int factionTemplateID, const CGUnit_C *unit)</tt>
/// Faction template id was replaced with FactionTemplateEntry ptr for performance, now caller is responsible for lookup
/// Used as static function instead of being a static method of Unit class
/////////////////////////////////////////////////
static ReputationRank GetFactionReaction(FactionTemplateEntry const* thisTemplate, Unit const* unit)
{
    // Simple sanity check
    if (!unit)
        return REP_NEUTRAL;

    // Original logic begins

    if (thisTemplate)
    {
        if (const FactionTemplateEntry* unitFactionTemplate = unit->getFactionTemplateEntry())
        {
            if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            {
                if (const Player* unitPlayer = unit->GetControllingPlayer())
                {
                    if (unitPlayer->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP) && thisTemplate->IsContestedGuardFaction())
                        return REP_HOSTILE;

                    if (const ReputationRank* rank = unitPlayer->GetReputationMgr().GetForcedRankIfAny(thisTemplate))
                        return (*rank);

                    if (!unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION))
                    {
                        const FactionEntry* thisFactionEntry = sFactionStore.LookupEntry(thisTemplate->faction);
                        if (thisFactionEntry && thisFactionEntry->HasReputation())
                        {
                            const ReputationMgr& reputationMgr = unitPlayer->GetReputationMgr();
                            return reputationMgr.GetRank(thisFactionEntry);
                        }
                    }
                }

            }
            // Default fallback if player-specific checks didn't catch anything: facton to faction
            return GetFactionReaction(thisTemplate, unitFactionTemplate);
        }
    }
    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// Get unit to unit reaction
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::UnitReaction(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
ReputationRank Unit::GetReactionTo(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return REP_NEUTRAL;

    // Original logic begins

    if (this == unit)
        return REP_FRIENDLY;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
    {
        const Player* thisPlayer = GetControllingPlayer();

        if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        {
            const Player* unitPlayer = unit->GetControllingPlayer();

            // WotLK+ player checks: does not default to neutral when either player controlled unit misses its player
            if (thisPlayer && unitPlayer)
            {
                if (thisPlayer == unitPlayer)
                    return REP_FRIENDLY;

                if (unitPlayer->GetUInt32Value(PLAYER_DUEL_TEAM))
                {
                    // TODO: Dueling misses duel arbiter and temporary truce during countdown, fix me later...
                    if (thisPlayer->IsInDuelWith(unitPlayer))
                        return REP_HOSTILE;
                }

                // WotLK+ group check: faction to unit
                if (thisPlayer->IsInGroup(unitPlayer))
                    return GetFactionReaction(getFactionTemplateEntry(), unit);
            }

            // WotLK+ FFA check
            if (IsPvPFreeForAll() && unit->IsPvPFreeForAll())
                return REP_HOSTILE;
        }

        if (thisPlayer)
        {
            if (const FactionTemplateEntry* unitFactionTemplate = unit->getFactionTemplateEntry())
            {
                if (const ReputationRank* rank = thisPlayer->GetReputationMgr().GetForcedRankIfAny(unitFactionTemplate))
                    return (*rank);

                const FactionEntry* unitFactionEntry = sFactionStore.LookupEntry(unitFactionTemplate->faction);

                // If the faction has reputation ranks available, "at war" and contested PVP flags decide outcome
                if (!this->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION) && unitFactionEntry && unitFactionEntry->HasReputation())
                {
                    if (thisPlayer->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP) && unitFactionTemplate->IsContestedGuardFaction())
                        return REP_HOSTILE;

                    return thisPlayer->GetReputationMgr().IsAtWar(unitFactionEntry) ? REP_HOSTILE : REP_FRIENDLY;
                }
            }
        }
    }
    // Default fallback if player-specific checks didn't catch anything: facton to unit
    return GetFactionReaction(getFactionTemplateEntry(), unit);
}

/////////////////////////////////////////////////
/// Get unit to corpse reaction
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::UnitReaction(const CGUnit_C *this, const CGCorpse_C *corpse)</tt>
/////////////////////////////////////////////////
ReputationRank Unit::GetReactionTo(const Corpse* corpse) const
{
    // Simple sanity check
    if (!corpse)
        return REP_NEUTRAL;

    // Original logic begins

    if (const FactionTemplateEntry* thisTemplate = getFactionTemplateEntry())
    {
        if (const uint32 corpseTemplateId = corpse->getFaction())
        {
            if (const FactionTemplateEntry* corpseTemplate = sFactionTemplateStore.LookupEntry(corpseTemplateId))
                return GetFactionReaction(thisTemplate, corpseTemplate);
        }
    }
    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// Get GO to unit reaction
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGGameObject_C::ObjectReaction(const CGGameObject_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
ReputationRank GameObject::GetReactionTo(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return REP_NEUTRAL;

    // Original logic begins

    if (const Unit* owner = GetOwner())
        return owner->GetReactionTo(unit);
    else if (const uint32 faction = GetUInt32Value(GAMEOBJECT_FACTION))
    {
        if (const FactionTemplateEntry* factionTemplate = sFactionTemplateStore.LookupEntry(faction))
            return GetFactionReaction(factionTemplate, unit);
    }

    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// Reaction preset: Unit sees another unit as an enemy
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::UnitIsEnemy(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
bool Unit::IsEnemy(Unit const* unit) const
{
    return (GetReactionTo(unit) < REP_UNFRIENDLY);
}

/////////////////////////////////////////////////
/// Reaction preset: Unit sees another unit as a friend
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::UnitIsFriend(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
bool Unit::IsFriend(const Unit* unit) const
{
    // WotLK+: Special flag overrides reaction
    if (GetTypeId() == TYPEID_UNIT)
    {
        if (static_cast<const Creature*>(this)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_TREAT_AS_IN_RAID)
            return true;
    }
    return (GetReactionTo(unit) > REP_NEUTRAL);
}

/////////////////////////////////////////////////
/// Opposition: Unit treats another unit as an enemy it can attack (generic)
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanAttack(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/// Backbone of all spells which can target hostile units.
/// Dependent 2.0+ macro API condition: <tt>[harm]</tt>
/////////////////////////////////////////////////
bool Unit::CanAttack(const Unit* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // TBC+: Arena Tournament commenatator
    if (GetTypeId() == TYPEID_PLAYER && static_cast<const Player*>(this)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_COMMENTATOR))
        return false;

    // Creatures cannot attack player ghosts, unless it is a specially flagged ghost creature
    if (GetTypeId() == TYPEID_UNIT && unit->GetTypeId() == TYPEID_PLAYER && static_cast<const Player*>(unit)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
    {
        if (!(static_cast<const Creature*>(this)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_GHOST_VISIBLE))
            return false;
    }

    // We can't attack unit when at least one of these flags is present on it:
    const uint32 mask = (UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_UNK_16 | UNIT_FLAG_TAXI_FLIGHT | UNIT_FLAG_NOT_SELECTABLE);
    if (unit->HasFlag(UNIT_FIELD_FLAGS, mask))
        return false;

    // Cross-check immunity and sanctuary flags: this <-> unit
    const bool thisPlayerControlled = HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
    if (thisPlayerControlled)
    {
        if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return false;
    }
    else if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    const bool unitPlayerControlled = unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
    if (unitPlayerControlled)
    {
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
            return false;
    }
    else if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // WotLK+: Check for same root vehicle
    // TODO: Rename flags appropriately after finding out it's usecases
    if (HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK16) || unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK16))
    {
        if (const VehicleInfo* thisVehicleInfo = GetVehicleInfo())
        {
            const VehicleEntry* thisVehicleEntry = thisVehicleInfo->GetVehicleEntry();
            if (thisVehicleEntry && (thisVehicleEntry->m_flags & VEHICLE_FLAG_UNK21) && unit->FindRootVehicle(this) == this)
                return false;
        }

        if (const VehicleInfo* unitVehicleInfo = unit->GetVehicleInfo())
        {
            const VehicleEntry* unitVehicleEntry = unitVehicleInfo->GetVehicleEntry();
            if (unitVehicleEntry && (unitVehicleEntry->m_flags & VEHICLE_FLAG_UNK21) && FindRootVehicle(unit) == unit)
                return false;
        }
    }

    if (thisPlayerControlled || unitPlayerControlled)
    {
        if (thisPlayerControlled && unitPlayerControlled)
        {
            if (IsFriend(unit))
                return false;

            const Player* thisPlayer = GetControllingPlayer();
            if (!thisPlayer)
                return (!IsPvPSanctuary() && !unit->IsPvPSanctuary());

            const Player* unitPlayer = unit->GetControllingPlayer();
            if (!unitPlayer)
                return (!IsPvPSanctuary() && !unit->IsPvPSanctuary());

            if (thisPlayer->IsInDuelWith(unitPlayer))
                return true;

            if (unit->IsPvP())
                return (!IsPvPSanctuary() && !unit->IsPvPSanctuary());

            if (IsPvPFreeForAll() && unit->IsPvPFreeForAll())
                return true;

            // WotLK+ TODO: Find out the meaning of this flag and rename
            if (HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1) || unit->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1))
                return (!IsPvPSanctuary() && !unit->IsPvPSanctuary());

            return false;
        }
        else
        {
            // WotLK+: generic sanctuary cross-check (moved down here from immunity flag checks in tbc)
            if (thisPlayerControlled && unit->IsPvPSanctuary())
                return false;

            if (unitPlayerControlled && IsPvPSanctuary())
                return false;
        }
        return (!IsFriend(unit));
    }
    return (IsEnemy(unit) || unit->IsEnemy(this));
}

/////////////////////////////////////////////////
/// Opposition: Unit treats another unit as an enemy it can attack (immediate response)
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanAttackNow(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/// Intended usage is to verify direct requests to attack something.
/////////////////////////////////////////////////
bool Unit::CanAttackNow(const Unit* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // We can't initiate attack while dead or ghost
    // NOTE: WotLK: client additionally contains an attack display hackfix for raised ally situation here (if player is dead and posesses a unit), we should handle it diffrerently serverside
    if (!isAlive())
        return false;

    // We can't initiate attack while mounted ...
    if (IsMounted())
    {
        // ... unless we are a creature with a special flag
        if (GetTypeId() != TYPEID_UNIT || !(static_cast<const Creature*>(this)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_MOUNTED_COMBAT))
            return false;
    }

    // We can't initiate attack on dead units
    if (!unit->isAlive())
        return false;

    return CanAttack(unit);
}

/////////////////////////////////////////////////
/// Assistance: Unit treats another unit as an ally it can help
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanAssist(const CGUnit_C *this, const CGUnit_C *unit, bool ignoreFlags)</tt>
/// Backbone of all spells which can target friendly units.
/// Dependent 2.0+ macro API condition: <tt>[help]</tt>
/////////////////////////////////////////////////
bool Unit::CanAssist(const Unit* unit, bool ignoreFlags) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // We can't assist unselectable unit
    if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    // Check immunity flags
    if (!ignoreFlags)
    {
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        {
            if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER))
                return false;
        }
        else if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
            return false;
    }

    // WotLK+: Exclude non-friendlies at this point ...
    if (GetReactionTo(unit) < REP_FRIENDLY)
    {
        // ... unless we are a creature with a special flag
        if (GetTypeId() != TYPEID_UNIT || !(static_cast<const Creature*>(this)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_TREAT_AS_IN_RAID))
            return false;
    }

    // Detect player controlled unit and exit early
    if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
    {
        const Player* thisPlayer = GetControllingPlayer();
        const Player* unitPlayer = unit->GetControllingPlayer();

        if (thisPlayer && unitPlayer)
        {
            if (thisPlayer->IsInDuelWith(unitPlayer))
                return false;
        }

        if (unit->IsPvPFreeForAll() && !IsPvPFreeForAll())
            return false;

        // TBC+: Cannot help a player outside of the sanctuary zone from within
        if (IsPvPSanctuary() && !unit->IsPvPSanctuary())
        {
            if (unit->IsPvP())
                return false;
        }
        return true;
    }

    // If we continue here, unit is an npc. Detect if we are an npc too, so we can exit early
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        return true;

    // WotLK+: We are left with player assisting an npc case here: can assist if we ignore flags during the check, or friendly NPCs with PVP flag or special flags
    if (ignoreFlags)
        return true;

    if (unit->IsPvP())
        return true;

    if (unit->GetTypeId() == TYPEID_UNIT)
    {
        if (const uint32 flags = static_cast<const Creature*>(unit)->GetCreatureInfo()->CreatureTypeFlags)
        {
            if (flags & CREATURE_TYPEFLAGS_CAN_ASSIST)
                return true;

            if (flags & CREATURE_TYPEFLAGS_TREAT_AS_IN_RAID)
                return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////
/// Assistance: Unit treats a corpse as an ally corpse it can help
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanAssist(const CGUnit_C *this, const CGCorpse_C *corpse)</tt>
/// Backbone of all spells which can target friendly corpses.
/// Dependent 2.0+ macro API condition: <tt>[help]</tt> (mouseover only for obvious reasons)
/////////////////////////////////////////////////
bool Unit::CanAssist(Corpse const* corpse) const
{
    return GetReactionTo(corpse) > REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// Cooperation: Unit can cooperate with another unit
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanCooperate(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
bool Unit::CanCooperate(const Unit* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // Can't cooperate with yourself
    if (this == unit)
        return false;

    // We can't cooperate while being charmed or with charmed unit
    if (GetCharmerGuid() || unit->GetCharmerGuid())
        return false;

    if (const FactionTemplateEntry* thisFactionTemplate = getFactionTemplateEntry())
    {
        if (const FactionTemplateEntry* unitFactionTemplate = unit->getFactionTemplateEntry())
        {
            if (thisFactionTemplate->factionGroupMask == unitFactionTemplate->factionGroupMask)
                return (!CanAttack(unit));
        }
    }
    return false;
}

/////////////////////////////////////////////////
/// Interaction: Unit can interact with an object (generic)
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanInteract(const CGUnit_C *this, const CGGameObject_C *object)</tt>
/////////////////////////////////////////////////
bool Unit::CanInteract(const GameObject* object) const
{
    // Simple sanity check
    if (!object)
        return false;

    // Original logic

    // Can't interact with GOs as a ghost
    if (GetTypeId() == TYPEID_PLAYER && static_cast<const Player*>(this)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return false;

    return (object->GetReactionTo(this) > REP_UNFRIENDLY);
}

/////////////////////////////////////////////////
/// Interaction: Unit can interact with another unit (generic)
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanInteract(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
bool Unit::CanInteract(const Unit* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // Unit must be selectable
    if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    // Unit must have NPC flags so we can actually interact in some way
    if (!unit->GetUInt32Value(UNIT_NPC_FLAGS))
        return false;

    // We can't interact with anyone as a ghost except specially flagged NPCs
    if (GetTypeId() == TYPEID_PLAYER && static_cast<const Player*>(this)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
    {
        if (unit->GetTypeId() != TYPEID_UNIT || !(static_cast<const Creature*>(unit)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_GHOST_VISIBLE))
            return false;
    }

    // WotLK+: personal squire support
    if (GetTypeId() == TYPEID_PLAYER && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP) && unit->GetTypeId() == TYPEID_UNIT)
    {
        if ((static_cast<const Creature*>(unit)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_SQUIRE) && unit->GetOwnerGuid() != GetObjectGuid())
            return false;
    }

    // WotLK+: ignore reaction when interacting with specially flagged unit
    if (unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INTERACT_ANY_REACTION))
        return true;

    return (GetReactionTo(unit) > REP_UNFRIENDLY && unit->GetReactionTo(this) > REP_UNFRIENDLY);
}

/////////////////////////////////////////////////
/// Interaction: Unit can interact with another unit (immediate response)
///
/// @note Relations API Tier 1
///
/// Client-side counterpart: <tt>CGUnit_C::CanInteractNow(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/////////////////////////////////////////////////
bool Unit::CanInteractNow(const Unit* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    // Original logic

    // We can't intract while on taxi
    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_TAXI_FLIGHT))
        return false;

    // We can't interact while being charmed
    if (GetCharmerGuid())
        return false;

    // We can't interact with anyone while being dead (this does not apply to player ghosts, which allow very limited interactions)
    if (!isAlive() && (GetTypeId() == TYPEID_UNIT || !(static_cast<const Player*>(this)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))))
        return false;

    // We can't interact with anyone while being shapeshifted, unless form flags allow us to do so
    if (IsShapeShifted())
    {
        if (SpellShapeshiftFormEntry const* formEntry = sSpellShapeshiftFormStore.LookupEntry(GetShapeshiftForm()))
        {
            if (!(formEntry->flags1 & SHAPESHIFT_FORM_FLAG_ALLOW_NPC_INTERACT))
                return false;
        }
    }

    // We can't interact with dead units, unless it's a creature with special flag
    if (!unit->isAlive())
    {
        if (GetTypeId() != TYPEID_UNIT || !(static_cast<const Creature*>(unit)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_INTERACT_DEAD))
            return false;
    }

    // WotLK+: We can't interact with charmed units, unless it is a vehicle
    if (unit->GetCharmerGuid() && !unit->IsVehicle())
        return false;

    // WotLK+: We can't interact with units who are currently fighting, unless specific conditions are met
    if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT) ||  unit->getVictim())
    {
        // We can't interact with units in fight by default
        bool interactable = false;

        // We can interact with siege weapons in fight
        if (!interactable && unit->GetTypeId() == TYPEID_UNIT && (static_cast<const Creature*>(unit)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_SIEGE_WEAPON))
            interactable = true;

        // We can interact with UNIT_FLAG2_INTERACT_ANY_REACTION flagged units even in fight
        if (!interactable && unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INTERACT_ANY_REACTION))
            interactable = true;

        // We can interact with UNIT_NPC_FLAG_SPELLCLICK flagged units in fight (if spell clicking is allowed)
        if (!interactable && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK) && !unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SPELL_CLICK_DISABLED))
        {
            interactable = !(unit->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SPELL_CLICK_IN_GROUP));
            if (!interactable)
            {
                if (const Player* thisPlayer = GetControllingPlayer())
                {
                    if (const Player* unitPlayer = unit->GetControllingPlayer())
                        interactable = thisPlayer->IsInGroup(unitPlayer);
                }
            }
        }

        if (!interactable)
            return false;
    }

    return CanInteract(unit);
}

/////////////////////////////////////////////////
/// Trivial: Unit does not count as a worthy target for another unit
///
/// @note Relations API Tier 1
///
/// Based on client-side counterpart: <tt>static CGPlayer_C::UnitIsTrivial(const CGUnit_C *unit)</tt>
/// Points of view are swapped to fit in with the rest of API, logic is preserved.
/////////////////////////////////////////////////
bool Unit::IsTrivialForTarget(Unit const* pov) const
{
    // Simple sanity check
    if (!pov)
        return false;

    // Original logic adapation for server (original function was operating as a local player PoV only)

    // Players are never seen as trivial
    if (GetTypeId() == TYPEID_PLAYER)
        return false;

    // Perform a level range query on the appropriate global constant NON_TRIVIAL_LEVEL_DIFFS array for the expansion
    return MaNGOS::XP::IsTrivialLevelDifference(pov->GetLevelForTarget(this), GetLevelForTarget(pov));
}

/////////////////////////////////////////////////
/// Civilian: Unit counts as a dishonorable kill for another unit
///
/// @note Relations API Tier 1
///
/// Forwardported from classic client-side counterpart: <tt>static function (original symbol name unknown)</tt>
/// Gameplay concept of dishonorable kill was removed in 2.0.1, all API related to it was removed in 2.0.3.
/// First appeared in 1.5.0, lasted until 2.0.3, forwardported for API unification and potential alternative usecases.
/////////////////////////////////////////////////
bool Unit::IsCivilianForTarget(Unit const* pov) const
{
    // Simple sanity check
    if (!pov)
        return false;

    // Original logic

    // PvP-enabled enemy npcs with civilian flag
    if (IsPvP() && GetTypeId() == TYPEID_UNIT && static_cast<const Creature*>(this)->IsCivilian())
        return (IsTrivialForTarget(pov) && IsEnemy(pov));

    return false;
}

/////////////////////////////////////////////////
/// Group: Unit counts as being placed in the same group (party or raid) with another unit (for gameplay purposes)
///
/// @note Relations API Tier 1
///
/// Based on client-side counterpart: <tt>static CGUnit_C::IsUnitInGroup(const CGUnit_C *this, const CGUnit_C *unit)</tt>
/// Additionally contains optional detection of same group from UI standpoint datamined from other functions.
/// Points of view are swapped to fit in with the rest of API, logic is preserved.
/////////////////////////////////////////////////
bool Unit::IsInGroup(Unit const* other, bool party/* = false*/, bool UI/* = false*/) const
{
    // Simple sanity check
    if (!other)
        return false;

    // Original logic adaptation for server (original function was operating as a local player PoV only)

    // Same unit is always in group with itself
    if (this == other)
        return true;

    // Only player controlled
    if (this->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) && other->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
    {
        // UI mode: only players and their current pets/charms are in the party UI
        if (UI)
        {
            const size_t comparisions = 3;
            const Player* thisPlayer[comparisions] = { nullptr, nullptr, nullptr };
            const Player* otherPlayer[comparisions] = { nullptr, nullptr, nullptr };

            auto getUIPlayerComparisions = [] (const Unit* unit, const Player* (&array)[comparisions])
            {
                // In reverse order
                if (unit->GetTypeId() == TYPEID_PLAYER)
                    array[0] = static_cast<const Player*>(unit);
                 ObjectGuid const& summonerGuid = unit->GetSummonerGuid();
                 ObjectGuid const& charmerGuid = unit->GetCharmerGuid();
                 if (summonerGuid.IsPlayer())
                    array[1] = sObjectMgr.GetPlayer(summonerGuid);
                 if (charmerGuid.IsPlayer())
                    array[2] = sObjectMgr.GetPlayer(charmerGuid);
            };

            getUIPlayerComparisions(this, thisPlayer);
            getUIPlayerComparisions(other, otherPlayer);

            for (size_t i = 0; i < comparisions; ++i)
            {
                if (thisPlayer[i])
                {
                    for (size_t j = 0; j < comparisions; ++j)
                    {
                        if (otherPlayer[j])
                        {
                            const Group* group = thisPlayer[i]->GetGroup();
                            if (thisPlayer[i] == otherPlayer[j] || (group && group == otherPlayer[j]->GetGroup() && (!party || group->SameSubGroup(thisPlayer[i], otherPlayer[j]))))
                                return true;
                        }
                    }
                }
            }
            return false;
        }

        // Check if controlling players are in the same group (same logic as client, but not local)
        if (const Player* thisPlayer = GetControllingPlayer())
        {
            if (const Player* otherPlayer = other->GetControllingPlayer())
            {
                const Group* group = thisPlayer->GetGroup();
                return (thisPlayer == otherPlayer || (group && group == otherPlayer->GetGroup() && (!party || group->SameSubGroup(thisPlayer, otherPlayer))));
            }
        }
    }

    // NOTE: For future reference: server uses additional gameplay grouping logic for mobs (in combat and out of combat) - requires research for Tier 2 implementation

    return false;
}

/*##########################
########            ########
########   TIER 2   ########
########            ########
##########################*/

/////////////////////////////////////////////////
/// [Serverside] Get default WorldObject (hierarchy) reaction to a unit
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Game always defaults reactions to neutral.
/////////////////////////////////////////////////
ReputationRank WorldObject::GetReactionTo(Unit const* /*unit*/) const
{
    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// [Serverside] Get default WorldObject (hierarchy) reaction to a corpse
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Game always defaults reactions to neutral.
/////////////////////////////////////////////////
ReputationRank WorldObject::GetReactionTo(Corpse const* /*corpse*/) const
{
    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: WorldObject (hierarchy) sees a unit as an enemy
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Game always defaults reactions to "neutral", so this is always false (neutral is not an enemy).
/////////////////////////////////////////////////
bool WorldObject::IsEnemy(Unit const* /*unit*/) const
{
    return false;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: WorldObject (hierarchy) sees a unit as a friend
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Game always defaults reactions to "neutral", so this is always false (neutral is not a friend).
/////////////////////////////////////////////////
bool WorldObject::IsFriend(Unit const* /*unit*/) const
{
    return false;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: GO sees a unit as an enemy
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Some gameobjects can be involved in spell casting, so server needs additional API support.
/////////////////////////////////////////////////
bool GameObject::IsEnemy(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    if (const Unit* owner = GetOwner())
        return owner->IsEnemy(unit);
    else if (const uint32 faction = GetUInt32Value(GAMEOBJECT_FACTION))
    {
        if (const FactionTemplateEntry* factionTemplate = sFactionTemplateStore.LookupEntry(faction))
            return (GetFactionReaction(factionTemplate, unit) < REP_UNFRIENDLY);
    }

    return false;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: GO sees a unit as a friend
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Some gameobjects can be involved in spell casting, so server needs additional API support.
/////////////////////////////////////////////////
bool GameObject::IsFriend(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    if (const Unit* owner = GetOwner())
        return owner->IsFriend(unit);
    else if (const uint32 faction = GetUInt32Value(GAMEOBJECT_FACTION))
    {
        if (const FactionTemplateEntry* factionTemplate = sFactionTemplateStore.LookupEntry(faction))
            return (GetFactionReaction(factionTemplate, unit) > REP_NEUTRAL);
    }

    return false;
}

/////////////////////////////////////////////////
/// [Serverside] Get DynamicObject reaction to a unit
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Dynamic objects act as serverside proxy casters for units.
/////////////////////////////////////////////////
ReputationRank DynamicObject::GetReactionTo(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return REP_NEUTRAL;

    if (const Unit* caster = GetCaster())
        return caster->GetReactionTo(unit);

    return REP_NEUTRAL;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: DynamicObject sees a unit as an enemy
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Dynamic objects act as serverside proxy casters for units.
/////////////////////////////////////////////////
bool DynamicObject::IsEnemy(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    if (const Unit* caster = GetCaster())
        return caster->IsEnemy(unit);

    return false;
}

/////////////////////////////////////////////////
/// [Serverside] Reaction preset: DynamicObject sees a unit as a friend
///
/// @note Relations API Tier 2
///
/// This function is not intented to have client-side counterpart by original design.
/// Dynamic objects act as serverside proxy casters for units.
/////////////////////////////////////////////////
bool DynamicObject::IsFriend(Unit const* unit) const
{
    // Simple sanity check
    if (!unit)
        return false;

    if (const Unit* caster = GetCaster())
        return caster->IsFriend(unit);

    return false;
}

/*##########################
########            ########
########   TIER 3   ########
########            ########
##########################*/

/////////////////////////////////////////////////
/// [Serverside] Opposition: DynamicObject can target a target with a harmful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// Dynamic objects act as serverside proxy casters for units.
/// It utilizes owners CanAttackSpell if owner exists
/////////////////////////////////////////////////
bool DynamicObject::CanAttackSpell(const Unit* target, SpellEntry const* spellInfo, bool isAOE) const
{
    if (Unit* owner = GetCaster())
        return owner->CanAttackSpell(target, spellInfo, isAOE);
    else
        return false;
}

/////////////////////////////////////////////////
/// [Serverside] Assistance: DynamicObject can target a target with a helpful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// Dynamic objects act as serverside proxy casters for units.
/// It utilizes owners CanAssistSpell if owner exists
/////////////////////////////////////////////////
bool DynamicObject::CanAssistSpell(const Unit* target, SpellEntry const* spellInfo) const
{
    if (Unit* owner = GetCaster())
        return owner->CanAttackSpell(target, spellInfo);
    else
        return false;
}

/////////////////////////////////////////////////
/// [Serverside] Opposition: GameObject can target a target with a harmful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// Some gameobjects can be involved in spell casting, so server needs additional API support.
/// It utilizes owners CanAttackSpell if owner exists
/////////////////////////////////////////////////
bool GameObject::CanAttackSpell(const Unit* target, SpellEntry const* spellInfo, bool isAOE) const
{
    Unit* owner = GetOwner();
    if (owner)
        return owner->CanAttackSpell(target, spellInfo, isAOE);

    return IsEnemy(target);
}

/////////////////////////////////////////////////
/// [Serverside] Assistance: GameObject can target a target with a helpful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// Some gameobjects can be involved in spell casting, so server needs additional API support.
/// It utilizes owners CanAssistSpell if owner exists
/////////////////////////////////////////////////
bool GameObject::CanAssistSpell(const Unit* target, SpellEntry const* spellInfo) const
{
    Unit* owner = GetOwner();
    if (owner)
        return owner->CanAssistSpell(target, spellInfo);

    return IsFriend(target);
}

/////////////////////////////////////////////////
/// [Serverside] Opposition: Unit can target a target with a harmful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// It utilizes SpellEntry for additional target filtering.
/// Also an additional fine grained check needs to be done for AOE spells, because they
/// need to skip PVP enabled targets in some special cases. (Chain spells, AOE)
/////////////////////////////////////////////////
bool Unit::CanAttackSpell(const Unit* target, SpellEntry const* spellInfo, bool isAOE) const
{
    if (spellInfo)
    {
        // inversealive is needed for some spells which need to be casted at dead targets (aoe)
        if (!target->isAlive() && !spellInfo->HasAttribute(SPELL_ATTR_EX2_CAN_TARGET_DEAD))
            return false;
    }

    if (CanAttack(target))
    {
        if (isAOE)
        {
            if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            {
                if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                {
                    const Player* thisPlayer = GetControllingPlayer();
                    if (!thisPlayer)
                        return (!IsPvPSanctuary() && !target->IsPvPSanctuary());

                    const Player* unitPlayer = target->GetControllingPlayer();
                    if (!unitPlayer)
                        return (!IsPvPSanctuary() && !target->IsPvPSanctuary());

                    if (thisPlayer->IsInDuelWith(unitPlayer))
                        return true;

                    if (target->IsPvP() && (!isAOE || thisPlayer->IsPvP()))
                        return (!IsPvPSanctuary() && !target->IsPvPSanctuary());

                    if (IsPvPFreeForAll() && target->IsPvPFreeForAll())
                        return true;

                    // WotLK+ TODO: Find out the meaning of this flag and rename
                    if (HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1) || target->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1))
                        return (!IsPvPSanctuary() && !target->IsPvPSanctuary());

                    return false;
                }
            }
        }

        return true;
    }
    else return false;
}

/////////////////////////////////////////////////
/// [Serverside] Assistance: Unit can target a target with a helpful spell
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// It utilizes owners CanAssistSpell if owner exists
/////////////////////////////////////////////////
bool Unit::CanAssistSpell(const Unit* target, SpellEntry const* spellInfo) const
{
    return CanAssist(target);
}

/////////////////////////////////////////////////
/// [Serverside] Opposition: Unit can attack a target on sight
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// It utilizes CanAttack with a small exclusion for Feign-Death targets and a hostile-only check.
/// Typically used in AIs in MoveInLineOfSight
/////////////////////////////////////////////////
bool Unit::CanAttackOnSight(const Unit* target)
{
    return CanAttack(target) && !target->IsFeigningDeathSuccessfully() && IsEnemy(target);
}

/////////////////////////////////////////////////
/// [Serverside] Fog of War: Unit can be seen by other unit through invisibility effects
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// A helper function to determine if unit is always visible to another unit.
/////////////////////////////////////////////////
bool Unit::IsFogOfWarVisibleStealth(Unit const* other) const
{
    // Gamemasters can see through invisibility
    if (other->GetTypeId() == TYPEID_PLAYER && static_cast<Player const*>(other)->isGameMaster())
        return true;

    switch (sWorld.getConfig(CONFIG_UINT32_FOGOFWAR_STEALTH))
    {
        default: return IsInGroup(other);
        case 1:  return CanCooperate(other);
    }
}

/////////////////////////////////////////////////
/// [Serverside] Fog of War: Unit's health values can be seen by other unit
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// A helper function to determine if unit's health values are always visible to another unit.
/////////////////////////////////////////////////
bool Unit::IsFogOfWarVisibleHealth(Unit const* other) const
{
    // Gamemasters can see health values
    if (other->GetTypeId() == TYPEID_PLAYER && static_cast<Player const*>(other)->isGameMaster())
        return true;

    switch (sWorld.getConfig(CONFIG_UINT32_FOGOFWAR_HEALTH))
    {
        case 0:  return IsInGroup(other, false, true);
        case 1:  return CanCooperate(other);
        default: return true;
    }
}

/////////////////////////////////////////////////
/// [Serverside] Fog of War: Unit's stat values can be seen by other unit
///
/// @note Relations API Tier 3
///
/// This function is not intented to have client-side counterpart by original design.
/// A helper function to determine if unit's stat values are always visible to another unit.
/////////////////////////////////////////////////
bool Unit::IsFogOfWarVisibleStats(Unit const* other) const
{
    // Gamemasters can see stat values
    if (other->GetTypeId() == TYPEID_PLAYER && static_cast<Player const*>(other)->isGameMaster())
        return true;

    switch (sWorld.getConfig(CONFIG_UINT32_FOGOFWAR_STATS))
    {
        default: return (this == other || GetSummonerGuid() == other->GetObjectGuid());
        case 1:  return CanCooperate(other);
        case 2:  return true;
    }
}
