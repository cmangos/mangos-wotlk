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

#include "Entities/Object.h"
#include "Globals/SharedDefines.h"
#include "Server/WorldPacket.h"
#include "Server/Opcodes.h"
#include "Log/Log.h"
#include "World/World.h"
#include "Entities/Creature.h"
#include "Entities/Player.h"
#include "Entities/Vehicle.h"
#include "Entities/GameObject.h"
#include "Entities/Transports.h"
#include "Globals/ObjectMgr.h"
#include "Entities/ObjectGuid.h"
#include "Entities/UpdateData.h"
#include "Entities/Transports.h"
#include "UpdateMask.h"
#include "Util/Util.h"
#include "Grids/CellImpl.h"
#include "Grids/GridNotifiers.h"
#include "Grids/GridNotifiersImpl.h"
#include "Maps/GridDefines.h"
#include "Maps/MapManager.h"
#include "Maps/ObjectPosSelector.h"
#include "Entities/TemporarySpawn.h"
#include "Movement/packet_builder.h"
#include "Entities/CreatureLinkingMgr.h"
#include "Chat/Chat.h"
#include "Loot/LootMgr.h"
#include "Spells/SpellMgr.h"
#include "MotionGenerators/PathFinder.h"
#include "Movement/MoveSpline.h"

Object::Object(): m_updateFlag(0), m_itsNewObject(false), m_dbGuid(0), m_scriptRef(this, NoopObjectDeleter())
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = nullptr;
    m_valuesCount       = 0;

    m_inWorld           = false;
    m_objectUpdated     = false;
    m_loot              = nullptr;
}

Object::~Object()
{
    if (IsInWorld())
    {
        ///- Do NOT call RemoveFromWorld here, if the object is a player it will crash
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still in world!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    if (m_objectUpdated)
    {
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still have updated status!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    delete[] m_uint32Values;

    delete m_loot;
}

void Object::AddToWorld()
{
    if (m_inWorld)
        return;

    m_inWorld = true;

    // synchronize values mirror with values array (changes will send in updatecreate opcode any way
    ClearUpdateMask(false);                         // false - we can't have update data in update queue before adding to world

    // Set new ref when adding to world (except if we already have one - also set in constructor to allow scripts to work in initialization phase)
    // Changing the ref when adding/removing from world prevents accessing players on different maps (possibly from another thread)
    if (!m_scriptRef)
        m_scriptRef.reset(this, NoopObjectDeleter());
}

void Object::RemoveFromWorld()
{
    // if we remove from world then sending changes not required
    ClearUpdateMask(true);
    m_inWorld = false;
    m_scriptRef = nullptr;
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[ m_valuesCount ];
    memset(m_uint32Values, 0, m_valuesCount * sizeof(uint32));

    m_changedValues.resize(m_valuesCount, false);

    m_objectUpdated = false;
}

void Object::_Create(uint32 dbGuid, uint32 guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values)
        _InitValues();

    ObjectGuid guid = ObjectGuid(guidhigh, entry, guidlow);
    m_dbGuid = dbGuid;

    SetGuidValue(OBJECT_FIELD_GUID, guid);
    SetUInt32Value(OBJECT_FIELD_TYPE, m_objectType);
    m_PackGUID.Set(guid);
}

void Object::SetObjectScale(float newScale)
{
    SetFloatValue(OBJECT_FIELD_SCALE_X, newScale);
}

void Object::SendForcedObjectUpdate()
{
    if (!m_inWorld || !m_objectUpdated)
        return;

    UpdateDataMapType update_players;

    BuildUpdateData(update_players);
    RemoveFromClientUpdateList();

    // here we allocate a std::vector with a size of 0x10000
    for (auto& update_player : update_players)
    {
        update_player.second.SendData(*update_player.first->GetSession());
    }
}

void Object::BuildMovementUpdateBlock(UpdateData& data, uint16 flags) const
{
    ByteBuffer buf(500);

    buf << uint8(UPDATETYPE_MOVEMENT);
    buf << GetPackGUID();

    BuildMovementUpdate(&buf, flags);

    data.AddUpdateBlock(buf);
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData& data, Player* target) const
{
    if (!target)
        return;

    uint8  updatetype   = UPDATETYPE_CREATE_OBJECT;
    uint16 updateFlags  = m_updateFlag;

    /** lower flag1 **/
    if (target == this)                                     // building packet for yourself
        updateFlags |= UPDATEFLAG_SELF;

    if (m_itsNewObject)
    {
        switch (GetObjectGuid().GetHigh())
        {
            case HighGuid::HIGHGUID_DYNAMICOBJECT:
            case HighGuid::HIGHGUID_CORPSE:
            case HighGuid::HIGHGUID_PLAYER:
            case HighGuid::HIGHGUID_UNIT:
            case HighGuid::HIGHGUID_VEHICLE:
            case HighGuid::HIGHGUID_GAMEOBJECT:
            case HighGuid::HIGHGUID_TRANSPORT:
                updatetype = UPDATETYPE_CREATE_OBJECT2;
                break;
            default:
                break;
        }
    }

    if (isType(TYPEMASK_UNIT))
    {
        if (static_cast<const Unit*>(this)->GetVictim())
            updateFlags |= UPDATEFLAG_HAS_ATTACKING_TARGET;
    }

    // DEBUG_LOG("BuildCreateUpdate: update-type: %u, object-type: %u got updateFlags: %X", updatetype, m_objectTypeId, updateFlags);

    ByteBuffer buf(500);
    buf << uint8(updatetype);
    buf << GetPackGUID();
    buf << uint8(m_objectTypeId);

    BuildMovementUpdate(&buf, updateFlags);

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);
    _SetCreateBits(updateMask, target);
    BuildValuesUpdate(updatetype, &buf, &updateMask, target);
    data.AddUpdateBlock(buf);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData& data, Player* target) const
{
    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    _SetUpdateBits(updateMask, target);
    if (updateMask.HasData())
        BuildValuesUpdateBlockForPlayer(data, updateMask, target);
}

void Object::BuildValuesUpdateBlockForPlayerWithFlags(UpdateData& data, Player* target, UpdateFieldFlags flags) const
{
    UpdateMask updateMask;
    updateMask.SetCount(GetValuesCount());
    MarkUpdateFieldsWithFlagForUpdate(updateMask, (uint16)flags);
    if (updateMask.HasData())
        BuildValuesUpdateBlockForPlayer(data, updateMask, target);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData& data, UpdateMask& updateMask, Player* target) const
{
    ByteBuffer buf(500);

    buf << uint8(UPDATETYPE_VALUES);
    buf << GetPackGUID();

    BuildValuesUpdate(UPDATETYPE_VALUES, &buf, &updateMask, target);
    data.AddUpdateBlock(buf);
}

void Object::BuildForcedValuesUpdateBlockForPlayer(UpdateData& data, Player* target) const
{
    ByteBuffer buf(500);

    buf << uint8(UPDATETYPE_VALUES);
    buf << GetPackGUID();

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    _SetCreateBits(updateMask, target);
    BuildValuesUpdate(UPDATETYPE_VALUES, &buf, &updateMask, target);

    data.AddUpdateBlock(buf);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData& data) const
{
    data.AddOutOfRangeGUID(GetObjectGuid());
}

void Object::DestroyForPlayer(Player* target, bool anim) const
{
    MANGOS_ASSERT(target);

    if (isType(TYPEMASK_UNIT) || isType(TYPEMASK_PLAYER))
    {
        if (BattleGround* bg = target->GetBattleGround())
        {
            if (bg->IsArena())
            {
                WorldPacket data(SMSG_DESTROY_ARENA_UNIT, 8);
                data << uint64(GetObjectGuid());
                target->SendDirectMessage(data);
            }
        }
    }

    WorldPacket data(SMSG_DESTROY_OBJECT, 9);
    data << GetObjectGuid();
    data << uint8(anim ? 1 : 0);                            // WotLK (bool), may be despawn animation
    target->GetSession()->SendPacket(data);
}

void Object::BuildMovementUpdate(ByteBuffer* data, uint16 updateFlags) const
{
    // uint16 moveFlags2 = (isType(TYPEMASK_UNIT) ? ((Unit*)this)->m_movementInfo.GetMovementFlags2() : MOVEFLAG2_NONE);

    *data << uint16(updateFlags);                           // update flags

    // 0x20
    if (updateFlags & UPDATEFLAG_LIVING)
    {
        Unit const* unit = static_cast<Unit const*>(this);

        // TODO: Remove this when moveflag is properly used
        if (unit->GetTransport() || unit->IsBoarded())
            const_cast<Unit*>(unit)->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
        else
            const_cast<Unit*>(unit)->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ONTRANSPORT);

        // Write movement info
        // TODO: Move boarding and unboarding and root enable/disable to movement generator
        if ((unit->m_movementInfo.GetMovementFlags() & MOVEFLAG_SPLINE_ENABLED) && unit->movespline->IsBoarding())
        {
            MovementInfo info = unit->m_movementInfo;
            info.RemoveMovementFlag(MOVEFLAG_ROOT);
            *data << info;
        }
        else
            *data << unit->m_movementInfo;

        // Unit speeds
        *data << float(unit->GetSpeed(MOVE_WALK));
        *data << float(unit->GetSpeed(MOVE_RUN));
        *data << float(unit->GetSpeed(MOVE_RUN_BACK));
        *data << float(unit->GetSpeed(MOVE_SWIM));
        *data << float(unit->GetSpeed(MOVE_SWIM_BACK));
        *data << float(unit->GetSpeed(MOVE_FLIGHT));
        *data << float(unit->GetSpeed(MOVE_FLIGHT_BACK));
        *data << float(unit->GetSpeed(MOVE_TURN_RATE));
        *data << float(unit->GetSpeed(MOVE_PITCH_RATE));

        // 0x08000000
        if (unit->m_movementInfo.GetMovementFlags() & MOVEFLAG_SPLINE_ENABLED)
            Movement::PacketBuilder::WriteCreate(*unit->movespline, *data);
    }
    else
    {
        if (updateFlags & UPDATEFLAG_POSITION)
        {
            WorldObject const* wo = static_cast<WorldObject const*>(this);

            GenericTransport* transport = wo->GetTransport();

            if (transport)
                *data << transport->GetPackGUID();
            else
                *data << uint8(0);

            *data << wo->GetPositionX();
            *data << wo->GetPositionY();
            *data << wo->GetPositionZ();

            if (transport)
            {
                *data << wo->GetTransOffsetX();
                *data << wo->GetTransOffsetY();
                *data << wo->GetTransOffsetZ();
            }
            else
            {
                *data << wo->GetPositionX();
                *data << wo->GetPositionY();
                *data << wo->GetPositionZ();
            }

            *data << wo->GetOrientation();

            if (transport)
                *data << float(wo->GetTransOffsetO());
            else
                *data << float(wo->GetOrientation());
        }
        else
        {
            // 0x40
            if (updateFlags & UPDATEFLAG_HAS_POSITION)
            {
                // 0x02
                if (updateFlags & UPDATEFLAG_TRANSPORT)
                {
                    GameObject const* go = static_cast<GameObject const*>(this);
                    *data << float(go->GetStationaryX());
                    *data << float(go->GetStationaryY());
                    *data << float(go->GetStationaryZ());
                    *data << float(go->GetStationaryO());
                }
                else
                {
                    WorldObject const* wo = static_cast<WorldObject const*>(this);
                    *data << float(wo->GetPositionX());
                    *data << float(wo->GetPositionY());
                    *data << float(wo->GetPositionZ());
                    *data << float(wo->GetOrientation());
                }
            }
        }
    }

    // 0x8
    if (updateFlags & UPDATEFLAG_LOWGUID)
    {
        switch (GetTypeId())
        {
            case TYPEID_OBJECT:
            case TYPEID_ITEM:
            case TYPEID_CONTAINER:
            case TYPEID_GAMEOBJECT:
            case TYPEID_DYNAMICOBJECT:
            case TYPEID_CORPSE:
                *data << uint32(GetGUIDLow());              // GetGUIDLow()
                break;
            case TYPEID_UNIT:
                *data << uint32(0x0000000B);                // unk, can be 0xB or 0xC
                break;
            case TYPEID_PLAYER:
                if (updateFlags & UPDATEFLAG_SELF)
                    *data << uint32(0x0000002F);            // unk, can be 0x15 or 0x22
                else
                    *data << uint32(0x00000008);            // unk, can be 0x7 or 0x8
                break;
            default:
                *data << uint32(0x00000000);                // unk
                break;
        }
    }

    // 0x10
    if (updateFlags & UPDATEFLAG_HIGHGUID)
    {
        switch (GetTypeId())
        {
            case TYPEID_OBJECT:
            case TYPEID_ITEM:
            case TYPEID_CONTAINER:
            case TYPEID_GAMEOBJECT:
            case TYPEID_DYNAMICOBJECT:
            case TYPEID_CORPSE:
                *data << uint32(GetObjectGuid().GetHigh()); // GetGUIDHigh()
                break;
            case TYPEID_UNIT:
                *data << uint32(0x0000000B);                // unk, can be 0xB or 0xC
                break;
            case TYPEID_PLAYER:
                if (updateFlags & UPDATEFLAG_SELF)
                    *data << uint32(0x0000002F);            // unk, can be 0x15 or 0x22
                else
                    *data << uint32(0x00000008);            // unk, can be 0x7 or 0x8
                break;
            default:
                *data << uint32(0x00000000);                // unk
                break;
        }
    }

    // 0x4
    if (updateFlags & UPDATEFLAG_HAS_ATTACKING_TARGET)      // packed guid (current target guid)
    {
        if (((Unit*) this)->GetVictim())
            *data << ((Unit*) this)->GetVictim()->GetPackGUID();
        else
            data->appendPackGUID(0);
    }

    // 0x2
    if (updateFlags & UPDATEFLAG_TRANSPORT)
    {
        WorldObject const* wo = static_cast<WorldObject const*>(this);
        *data << uint32(wo->GetMap()->GetCurrentMSTime());
    }

    // 0x80
    if (updateFlags & UPDATEFLAG_VEHICLE)
    {
        *data << uint32(((Unit*)this)->GetVehicleInfo()->GetVehicleEntry()->m_ID); // vehicle id
        *data << float(((WorldObject*)this)->GetOrientation());
    }

    // 0x200
    if (updateFlags & UPDATEFLAG_ROTATION)
    {
        *data << int64(((GameObject*)this)->GetPackedLocalRotation());
    }
}

void Object::BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, UpdateMask* updateMask, Player* target) const
{
    if (!target)
        return;

    bool IsActivateToQuest = false;
    bool IsPerCasterAuraState = false;

    if (updatetype == UPDATETYPE_CREATE_OBJECT || updatetype == UPDATETYPE_CREATE_OBJECT2)
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsDynTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYNAMIC);
        }
        else if (isType(TYPEMASK_UNIT))
        {
            if (((Unit*)this)->HasAuraState(AURA_STATE_CONFLAGRATE))
            {
                IsPerCasterAuraState = true;
                updateMask->SetBit(UNIT_FIELD_AURASTATE);
            }
        }
    }
    else                                                    // case UPDATETYPE_VALUES
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsDynTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYNAMIC);
            updateMask->SetBit(GAMEOBJECT_BYTES_1);         // why do we need this here?
        }
        else if (isType(TYPEMASK_UNIT))
        {
            if (((Unit*)this)->HasAuraState(AURA_STATE_CONFLAGRATE))
            {
                IsPerCasterAuraState = true;
                updateMask->SetBit(UNIT_FIELD_AURASTATE);
            }
        }
    }

    MANGOS_ASSERT(updateMask && updateMask->GetCount() == m_valuesCount);

    *data << (uint8)updateMask->GetBlockCount();
    data->append(updateMask->GetMask(), updateMask->GetLength());

    // 2 specialized loops for speed optimization in non-unit case
    if (isType(TYPEMASK_UNIT))                              // unit (creature/player) case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == UNIT_NPC_FLAGS)
                {
                    uint32 appendValue = m_uint32Values[index];

                    if (GetTypeId() == TYPEID_UNIT)
                    {
                        if (!target->canSeeSpellClickOn((Creature*)this))
                            appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;

                        if (appendValue & UNIT_NPC_FLAG_TRAINER)
                        {
                            if (!((Creature*)this)->IsTrainerOf(target, false))
                                appendValue &= ~(UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_TRAINER_CLASS | UNIT_NPC_FLAG_TRAINER_PROFESSION);
                        }

                        if (appendValue & UNIT_NPC_FLAG_STABLEMASTER)
                        {
                            if (target->getClass() != CLASS_HUNTER)
                                appendValue &= ~UNIT_NPC_FLAG_STABLEMASTER;
                        }

                        if (appendValue & UNIT_NPC_FLAG_FLIGHTMASTER)
                        {
                            QuestRelationsMapBounds bounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(((Creature*)this)->GetEntry());
                            for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
                            {
                                Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);
                                if (target->CanSeeStartQuest(pQuest))
                                {
                                    appendValue &= ~UNIT_NPC_FLAG_FLIGHTMASTER;
                                    break;
                                }
                            }

                            bounds = sObjectMgr.GetCreatureQuestInvolvedRelationsMapBounds(((Creature*)this)->GetEntry());
                            for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
                            {
                                Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);
                                if (target->CanRewardQuest(pQuest, false))
                                {
                                    appendValue &= ~UNIT_NPC_FLAG_FLIGHTMASTER;
                                    break;
                                }
                            }
                        }
                    }

                    *data << uint32(appendValue);
                }
                else if (index == UNIT_FIELD_AURASTATE)
                {
                    if (IsPerCasterAuraState)
                    {
                        // IsPerCasterAuraState set if related pet caster aura state set already
                        if (((Unit*)this)->HasAuraStateForCaster(AURA_STATE_CONFLAGRATE, target->GetObjectGuid()))
                            *data << m_uint32Values[index];
                        else
                            *data << (m_uint32Values[index] & ~(1 << (AURA_STATE_CONFLAGRATE - 1)));
                    }
                    else
                        *data << m_uint32Values[index];
                }
                // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from float to uint32 and send
                    *data << uint32(m_floatValues[index] < 0 ? 0 : m_floatValues[index]);
                }

                // there are some float values which may be negative or can't get negative due to other checks
                else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                         (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                         (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                         (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                {
                    *data << uint32(m_floatValues[index]);
                }
                else if (index == UNIT_FIELD_HEALTH || index == UNIT_FIELD_MAXHEALTH)
                {
                    uint32 value = m_uint32Values[index];

                    // Fog of War: replace absolute health values with percentages for non-allied units according to settings
                    if (!static_cast<const Unit*>(this)->IsFogOfWarVisibleHealth(target) &&
                        !target->CanSeeSpecialInfoOf(static_cast<const Unit*>(this)))
                    {
                        switch (index)
                        {
                            case UNIT_FIELD_HEALTH:     value = uint32(ceil((100.0 * value) / m_uint32Values[UNIT_FIELD_MAXHEALTH]));   break;
                            case UNIT_FIELD_MAXHEALTH:  value = 100;                                                                    break;
                        }
                    }

                    *data << value;
                }
                else if (index == UNIT_FIELD_FLAGS)
                {
                    uint32 value = m_uint32Values[index];

                    // For gamemasters in GM mode:
                    if (target->IsGameMaster())
                    {
                        // Gamemasters should be always able to select units - remove not selectable flag:
                        value &= ~UNIT_FLAG_UNINTERACTIBLE;
                    }

                    // Client bug workaround: Fix for missing chat channels when resuming taxi flight on login
                    // Client does not send any chat joining attempts by itself when taxi flag is on
                    if (target == this && (value & UNIT_FLAG_TAXI_FLIGHT))
                    {
                        if (sWorld.getConfig(CONFIG_BOOL_TAXI_FLIGHT_CHAT_FIX))
                            if (WorldSession* session = static_cast<Player const*>(this)->GetSession())
                                if (!session->IsInitialZoneUpdated())
                                    value &= ~UNIT_FLAG_TAXI_FLIGHT;
                    }

                    // On login/reconnect: delay combat state application at client UI to not interfere with secure frames init
                    if (target == this && (value & UNIT_FLAG_IN_COMBAT))
                    {
                        if (static_cast<Player const*>(this)->GetSession()->PlayerLoading())
                            value &= ~UNIT_FLAG_IN_COMBAT;
                    }

                    *data << value;
                }
                // Hide special-info for non empathy-casters,
                // Hide lootable animation for unallowed players
                // Handle tapped flag
                else if (index == UNIT_DYNAMIC_FLAGS)
                {
                    Creature const* creature = static_cast<Creature const*>(this);
                    uint32 dynflagsValue = m_uint32Values[index];
                    bool setTapFlags = false;

                    if (creature->IsAlive())
                    {
                        // Checking SPELL_AURA_EMPATHY and caster
                        if (dynflagsValue & UNIT_DYNFLAG_SPECIALINFO)
                        {
                            bool bIsEmpathy = false;
                            bool bIsCaster = false;
                            Unit::AuraList const& mAuraEmpathy = creature->GetAurasByType(SPELL_AURA_EMPATHY);
                            for (Unit::AuraList::const_iterator itr = mAuraEmpathy.begin(); !bIsCaster && itr != mAuraEmpathy.end(); ++itr)
                            {
                                bIsEmpathy = true;              // Empathy by aura set
                                if ((*itr)->GetCasterGuid() == target->GetObjectGuid())
                                    bIsCaster = true;           // target is the caster of an empathy aura
                            }
                            if (bIsEmpathy && !bIsCaster)       // Empathy by aura, but target is not the caster
                                dynflagsValue &= ~UNIT_DYNFLAG_SPECIALINFO;
                        }

                        // creature is alive so, not lootable
                        dynflagsValue = dynflagsValue & ~UNIT_DYNFLAG_LOOTABLE;
                        if (creature->IsInCombat())
                        {
                            // as creature is in combat we have to manage tap flags
                            setTapFlags = true;
                        }
                        else
                        {
                            // creature is not in combat so its not tapped
                            dynflagsValue = dynflagsValue & ~(UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER);
                            //sLog.outString(">> %s is not in combat so not tapped by %s", this->GetGuidStr().c_str(), target->GetGuidStr().c_str());
                        }
                    }
                    else
                    {
                        // check loot flag
                        if (creature->m_loot && creature->m_loot->CanLoot(target))
                        {
                            // creature is dead and this player can loot it
                            dynflagsValue = dynflagsValue | UNIT_DYNFLAG_LOOTABLE;
                            //sLog.outString(">> %s is lootable for %s", this->GetGuidStr().c_str(), target->GetGuidStr().c_str());
                        }
                        else
                        {
                            // creature is dead but this player cannot loot it
                            dynflagsValue = dynflagsValue & ~UNIT_DYNFLAG_LOOTABLE;
                            //sLog.outString(">> %s is not lootable for %s", this->GetGuidStr().c_str(), target->GetGuidStr().c_str());
                        }

                        // as creature is died we have to manage tap flags
                        setTapFlags = true;
                    }

                    // check tap flags
                    if (setTapFlags)
                    {
                        dynflagsValue = dynflagsValue | UNIT_DYNFLAG_TAPPED;
                        if (creature->IsTappedBy(target))
                        {
                            // creature is in combat or died and tapped by this player
                            dynflagsValue = dynflagsValue | UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                            //sLog.outString(">> %s is tapped by %s", this->GetGuidStr().c_str(), target->GetGuidStr().c_str());
                        }
                        else
                        {
                            // creature is in combat or died but not tapped by this player
                            dynflagsValue = dynflagsValue & ~UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                            //sLog.outString(">> %s is not tapped by %s", this->GetGuidStr().c_str(), target->GetGuidStr().c_str());
                        }
                    }

                    if (GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER)
                    {
                        Unit const* unit = static_cast<const Unit*>(this); // hunters mark effects should only be visible to owners and not all players
                        if (!unit->HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, target->GetObjectGuid()))
                            dynflagsValue &= ~UNIT_DYNFLAG_TRACK_UNIT;
                    }

                    *data << dynflagsValue;
                }
                else if (index == UNIT_FIELD_FACTIONTEMPLATE)
                {
                    uint32 value = m_uint32Values[index];

                    // [XFACTION]: Alter faction if detected crossfaction group interaction when updating faction field:
                    if (this != target && GetTypeId() == TYPEID_PLAYER)
                    {
                        Player const* thisPlayer = static_cast<Player const*>(this);

                        if (sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP) && target->IsInGroup(thisPlayer))
                        {
                            const uint32 targetTeam = target->GetTeam();

                            if (thisPlayer->GetTeam() != targetTeam && value == Player::getFactionForRace(thisPlayer->getRace()))
                            {
                                switch (targetTeam)
                                {
                                    case ALLIANCE:  value = 1054;   break;  // "Alliance Generic"
                                    case HORDE:     value = 1495;   break;  // "Horde Generic"
                                }
                            }
                        }
                    }

                    *data << value;
                }
                else                                        // Unhandled index, just send
                {
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[index];
                }
            }
        }
    }
    else if (isType(TYPEMASK_CORPSE))                       // corpse case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == CORPSE_FIELD_BYTES_1)
                {
                    uint32 value = m_uint32Values[index];

                    // [XFACTION]: Alter race field if detected crossfaction group interaction:
                    if (sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP))
                    {
                        Corpse const* thisCorpse = static_cast<Corpse const*>(this);
                        ObjectGuid const& ownerGuid = thisCorpse->GetOwnerGuid();
                        Group const* targetGroup = target->GetGroup();

                        if (ownerGuid != target->GetObjectGuid() && targetGroup && targetGroup->IsMember(ownerGuid))
                        {
                            const uint8 targetRace = target->getRace();

                            if (Player::TeamForRace(thisCorpse->getRace()) != Player::TeamForRace(targetRace))
                                value = ((value &~ uint32(0xFF << 8)) | (uint32(targetRace) << 8));
                        }
                    }

                    *data << value;
                }
                else
                    *data << m_uint32Values[index];         // other cases
            }
        }
    }
    else if (isType(TYPEMASK_GAMEOBJECT))                   // gameobject case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                if (index == GAMEOBJECT_DYNAMIC)
                {
                    // GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY can have lo flag = 2
                    //      most likely related to "can enter map" and then should be 0 if can not enter

                    if (IsActivateToQuest)
                    {
                        GameObject const* gameObject = static_cast<GameObject const*>(this);
                        switch (((GameObject*)this)->GetGoType())
                        {
                            case GAMEOBJECT_TYPE_QUESTGIVER:
                                // GO also seen with GO_DYNFLAG_LO_SPARKLE explicit, relation/reason unclear (192861)
                                *data << uint16(GO_DYNFLAG_LO_ACTIVATE);
                                *data << uint16(-1);
                                break;
                            case GAMEOBJECT_TYPE_CHEST:
                                if (gameObject->GetLootState() == GO_READY || gameObject->GetLootState() == GO_ACTIVATED)
                                    *data << uint16(GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
                                else
                                    *data << uint16(0);
                                *data << uint16(-1);
                                break;
                            case GAMEOBJECT_TYPE_GENERIC:
                            case GAMEOBJECT_TYPE_SPELL_FOCUS:
                            case GAMEOBJECT_TYPE_GOOBER:
                                *data << uint16(GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
                                *data << uint16(-1);
                                break;
                            default:
                                // unknown, not happen.
                                *data << uint16(0);
                                *data << uint16(-1);
                                break;
                        }
                    }
                    else
                    {
                        GameObject const* gameObject = static_cast<GameObject const*>(this);
                        switch (((GameObject*)this)->GetGoType())
                        {
                            case GAMEOBJECT_TYPE_TRANSPORT:
                            case GAMEOBJECT_TYPE_MO_TRANSPORT:
                                *data << m_uint32Values[index];
                                break;
                            default:
                                // disable quest object
                                *data << uint16(0);
                                *data << uint16(-1);
                                break;
                        }
                    }
                }
                else
                    *data << m_uint32Values[index];         // other cases
            }
        }
    }
    else                                                    // other objects case (no special index checks)
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                *data << m_uint32Values[index];
            }
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    if (m_uint32Values)
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
            m_changedValues[index] = false;
    }

    if (m_objectUpdated)
    {
        if (remove)
            RemoveFromClientUpdateList();
        m_objectUpdated = false;
    }
}

uint16 Object::GetUpdateFieldFlagsForTarget(Player const* target, uint16 const*& flags) const
{
    flags = UpdateFields::GetUpdateFieldFlagsArray(GetTypeId());
    uint16 visibleFlag = UF_FLAG_PUBLIC | UF_FLAG_DYNAMIC;

    if (target == this)
        visibleFlag |= UF_FLAG_PRIVATE;

    switch (GetTypeId())
    {
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
            if (static_cast<Item const*>(this)->GetOwnerGuid() == target->GetObjectGuid())
                visibleFlag |= UF_FLAG_OWNER_ONLY | UF_FLAG_UNK2;
            break;
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
        {
            if (static_cast<Unit const*>(this)->GetMasterGuid() == target->GetObjectGuid())
                visibleFlag |= UF_FLAG_OWNER_ONLY;

            if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO))
                if (target->CanSeeSpecialInfoOf(static_cast<Unit const*>(this)))
                    visibleFlag |= UF_FLAG_SPECIAL_INFO;

            if (static_cast<Unit const*>(this)->IsInGroup(target, false, false))
                visibleFlag |= UF_FLAG_GROUP_ONLY;
            break;
        }
        case TYPEID_GAMEOBJECT:
            if (static_cast<GameObject const*>(this)->GetOwnerGuid() == target->GetObjectGuid())
                visibleFlag |= UF_FLAG_OWNER_ONLY;
            break;
        case TYPEID_DYNAMICOBJECT:
            if (static_cast<DynamicObject const*>(this)->GetCasterGuid() == target->GetObjectGuid())
                visibleFlag |= UF_FLAG_OWNER_ONLY;
            break;
        case TYPEID_CORPSE:
            if (static_cast<Corpse const*>(this)->GetOwnerGuid() == target->GetObjectGuid())
                visibleFlag |= UF_FLAG_OWNER_ONLY;
            break;
        case TYPEID_OBJECT:
            break;
    }

    return visibleFlag;
}

void Object::_LoadIntoDataField(const char* data, uint32 startOffset, uint32 count)
{
    if (!data)
        return;

    Tokens tokens = StrSplit(data, " ");

    if (tokens.size() != count)
        return;

    Tokens::const_iterator iter;
    uint32 index;
    for (iter = tokens.begin(), index = 0; index < count; ++iter, ++index)
    {
        m_uint32Values[startOffset + index] = atol((*iter).c_str());
    }
}

void Object::MarkUpdateFieldsWithFlagForUpdate(UpdateMask& updateMask, uint16 flag) const
{
    uint16 const* flags = UpdateFields::GetUpdateFieldFlagsArray(GetTypeId());
    MANGOS_ASSERT(flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (GetUInt32Value(index) != 0 && (flags[index] & flag))
            updateMask.SetBit(index);
    }
}

void Object::_SetUpdateBits(UpdateMask& updateMask, Player* target) const
{
    uint16 const* flags = nullptr;
    uint16 visibleFlag = GetUpdateFieldFlagsForTarget(target, flags);
    MANGOS_ASSERT(flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
        if (m_changedValues[index] && (flags[index] & visibleFlag))
            updateMask.SetBit(index);
}

void Object::_SetCreateBits(UpdateMask& updateMask, Player* target) const
{
    uint16 const* flags = nullptr;
    uint16 visibleFlag = GetUpdateFieldFlagsForTarget(target, flags);
    MANGOS_ASSERT(flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
        if (GetUInt32Value(index) != 0 && (flags[index] & visibleFlag))
            updateMask.SetBit(index);
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[index] != value)
    {
        m_int32Values[index] = value;
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[index] != value)
    {
        m_uint32Values[index] = value;
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetUInt64Value(uint16 index, const uint64& value)
{
    MANGOS_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*) & (m_uint32Values[index])) != value)
    {
        m_uint32Values[index] = *((uint32*)&value);
        m_uint32Values[index + 1] = *(((uint32*)&value) + 1);
        m_changedValues[index] = true;
        m_changedValues[index + 1] = true;
        MarkForClientUpdate();
    }
}

void Object::SetFloatValue(uint16 index, float value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[index] != value)
    {
        m_floatValues[index] = value;
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 8));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        sLog.outError("Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[index] >> (offset * 16)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[index] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (offset * 8));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (offset * 8));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::SetShortFlag(uint16 index, bool highpart, uint16 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (!(uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (highpart ? 16 : 0));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

void Object::RemoveShortFlag(uint16 index, bool highpart, uint16 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (highpart ? 16 : 0));
        m_changedValues[index] = true;
        MarkForClientUpdate();
    }
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    sLog.outError("Attempt %s nonexistent value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // ASSERT must fail after function call
    return false;
}

bool Object::PrintEntryError(char const* descr) const
{
    sLog.outError("Object Type %u, Entry %u (lowguid %u) with invalid call for %s", GetTypeId(), GetEntry(), GetObjectGuid().GetCounter(), descr);

    // always false for continue assert fail
    return false;
}

void Object::BuildUpdateDataForPlayer(Player* pl, UpdateDataMapType& update_players) const
{
    UpdateDataMapType::iterator iter = update_players.find(pl);

    if (iter == update_players.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = update_players.insert(UpdateDataMapType::value_type(pl, UpdateData()));
        MANGOS_ASSERT(p.second);
        iter = p.first;
    }

    BuildValuesUpdateBlockForPlayer(iter->second, iter->first);
}

void Object::BuildCreateDataForPlayer(Player* pl, UpdateDataMapType& update_players, bool auras) const
{
    UpdateDataMapType::iterator iter = update_players.find(pl);

    if (iter == update_players.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = update_players.insert(UpdateDataMapType::value_type(pl, UpdateData()));
        MANGOS_ASSERT(p.second);
        iter = p.first;
    }

    BuildCreateUpdateBlockForPlayer(iter->second, iter->first);

    if (auras && IsUnit())
        iter->second.AddAfterCreatePacket(Player::BuildAurasForTarget(static_cast<Unit const*>(this)));
}

void Object::SetUpdateFlag(ObjectUpdateFlags flag, bool add)
{
    if (add)
        m_updateFlag |= flag;
    else
        m_updateFlag &= ~flag;
}

void Object::BuildOutOfRangeDataForPlayer(Player* pl, UpdateDataMapType& update_players, ObjectGuid oorObject)
{
    UpdateDataMapType::iterator iter = update_players.find(pl);

    if (iter == update_players.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = update_players.insert(UpdateDataMapType::value_type(pl, UpdateData()));
        MANGOS_ASSERT(p.second);
        iter = p.first;
    }

    iter->second.AddOutOfRangeGUID(oorObject);
}

void Object::AddToClientUpdateList()
{
    sLog.outError("Unexpected call of Object::AddToClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::RemoveFromClientUpdateList()
{
    sLog.outError("Unexpected call of Object::RemoveFromClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::MarkForClientUpdate()
{
    if (m_inWorld)
    {
        if (!m_objectUpdated)
        {
            AddToClientUpdateList();
            m_objectUpdated = true;
        }
    }
}

void Object::ForceValuesUpdateAtIndex(uint16 index)
{
    m_changedValues[index] = true;
    if (m_inWorld && !m_objectUpdated)
    {
        AddToClientUpdateList();
        m_objectUpdated = true;
    }
}

void Object::ForceValuesUpdateForFlag(uint16 flag)
{
    uint16 const* flags = UpdateFields::GetUpdateFieldFlagsArray(GetTypeId());
    MANGOS_ASSERT(flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (GetUInt32Value(index) != 0 && (flags[index] & flag))
            ForceValuesUpdateAtIndex(index);
    }
}

void WorldObject::SetStringId(uint32 stringId, bool apply)
{
    if (apply)
        m_stringIds.insert(stringId);
    else
        m_stringIds.erase(stringId);

    if (IsInWorld())
    {
        if (apply)
            GetMap()->AddStringIdObject(stringId, this);
        else
            GetMap()->RemoveStringIdObject(stringId, this);
    }
}

void WorldObject::AddStringId(std::string& stringId)
{
    uint32 stringIdId = GetMap()->GetMapDataContainer().GetStringId(stringId);
    if (stringIdId)
        SetStringId(stringIdId, true);
}

void WorldObject::RemoveStringId(std::string& stringId)
{
    uint32 stringIdId = GetMap()->GetMapDataContainer().GetStringId(stringId);
    if (stringIdId)
        SetStringId(stringIdId, false);
}

bool WorldObject::HasStringId(const std::string& stringId) const
{
    return HasStringId(GetMap()->GetMapDataContainer().GetStringId(stringId));
}

bool WorldObject::HasStringId(uint32 stringId) const
{
    return m_stringIds.find(stringId) != m_stringIds.end();
}

WorldObject::WorldObject() :
    m_transport(nullptr), m_transportInfo(nullptr), m_isOnEventNotified(false),
    m_visibilityData(this), m_nextUpdateTime(0), m_accumulatedUpdateDiff(0), m_currMap(nullptr),
    m_mapId(0), m_InstanceId(0), m_phaseMask(PHASEMASK_NORMAL),
    m_isActiveObject(false), m_debugFlags(0), m_destLocCounter(0), m_castCounter(0), m_inRemoveList(false)
{
}

void WorldObject::CleanupsBeforeDelete()
{
    m_events.KillAllEvents(false);                      // non-delatable (currently casted spells) will not deleted now but it will deleted at call in Map::RemoveAllObjectsInRemoveList
    RemoveFromWorld();
}

WorldObject::~WorldObject()
{
    MANGOS_ASSERT(!m_inRemoveList);
}

void WorldObject::Update(const uint32 diff)
{
    m_heartBeatTimer.Update(diff);
    while (m_heartBeatTimer.Passed())
    {
        m_heartBeatTimer.Reset(m_heartBeatTimer.GetExpiry() + GetHeartbeatDuration());
        Heartbeat();
    }
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh, uint32 phaseMask)
{
    Object::_Create(guidlow, guidlow, 0, guidhigh);
    m_phaseMask = phaseMask;
}

void WorldObject::Relocate(float x, float y, float z, float orientation)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_position.o = orientation;

    if (isType(TYPEMASK_UNIT))
        m_movementInfo.ChangePosition(x, y, z, orientation);
}

void WorldObject::Relocate(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;

    if (isType(TYPEMASK_UNIT))
        m_movementInfo.ChangePosition(x, y, z, GetOrientation());
}

void WorldObject::SetOrientation(float orientation)
{
    m_position.o = orientation;

    if (isType(TYPEMASK_UNIT))
        m_movementInfo.ChangeOrientation(orientation);
}

uint32 WorldObject::GetZoneId() const
{
    return GetTerrain()->GetZoneId(m_position.x, m_position.y, m_position.z);
}

uint32 WorldObject::GetAreaId() const
{
    return GetTerrain()->GetAreaId(m_position.x, m_position.y, m_position.z);
}

AreaNameInfo WorldObject::GetAreaName(LocaleConstant locale) const
{
    return GetTerrain()->GetAreaName(m_position.x, m_position.y, m_position.z, locale);
}

void WorldObject::GetZoneAndAreaId(uint32& zoneid, uint32& areaid) const
{
    GetTerrain()->GetZoneAndAreaId(zoneid, areaid, m_position.x, m_position.y, m_position.z);
}

InstanceData* WorldObject::GetInstanceData() const
{
    return GetMap()->GetInstanceData();
}

float WorldObject::GetDistance(const WorldObject* obj, bool is3D, DistanceCalculation distcalc) const
{
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx * dx + dy * dy;

    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz * dz;
    }

    switch (distcalc)
    {
        case DIST_CALC_BOUNDING_RADIUS:
        {
            float sizefactor = GetObjectBoundingRadius() + obj->GetObjectBoundingRadius();
            float dist = sqrt(distsq) - sizefactor;
            return dist > 0.0f ? dist : 0.0f;
        }
        case DIST_CALC_COMBAT_REACH:
        case DIST_CALC_COMBAT_REACH_WITH_MELEE:
        {
            float sizefactor = GetCombinedCombatReach(obj, distcalc == DIST_CALC_COMBAT_REACH_WITH_MELEE);
            float dist = sqrt(distsq) - sizefactor;
            return dist > 0.0f ? dist : 0.0f;
        }
        default: return distsq;
    }
}

float WorldObject::GetDistance(float x, float y, float z, DistanceCalculation distcalc, bool transport) const
{
    Position pos = GetPosition(transport ? GetTransport() : nullptr);
    float dx = pos.x - x;
    float dy = pos.y - y;
    float dz = pos.z - z;
    float dist = dx * dx + dy * dy + dz * dz;

    switch (distcalc)
    {
        case DIST_CALC_BOUNDING_RADIUS:
        {
            float sizefactor = GetObjectBoundingRadius();
            dist = sqrt(dist) - sizefactor;
            return dist > 0.0f ? dist : 0.0f;
        }
        case DIST_CALC_COMBAT_REACH:
        case DIST_CALC_COMBAT_REACH_WITH_MELEE:
        {
            float reach = GetCombinedCombatReach(distcalc == DIST_CALC_COMBAT_REACH_WITH_MELEE);
            dist = sqrt(dist) - reach;
            return dist > 0.0f ? dist : 0.0f;
        }
        default: return dist;
    }
}

float WorldObject::GetDistance2d(float x, float y, DistanceCalculation distcalc, bool transport) const
{
    Position const& pos = GetPosition(transport ? GetTransport() : nullptr);
    float dx = pos.x - x;
    float dy = pos.y - y;
    float dist = dx * dx + dy * dy;

    switch (distcalc)
    {
        case DIST_CALC_BOUNDING_RADIUS:
        {
            float sizefactor = GetObjectBoundingRadius();
            dist = sqrt(dist) - sizefactor;
            return dist > 0.0f ? dist : 0.0f;
        }
        case DIST_CALC_COMBAT_REACH:
        case DIST_CALC_COMBAT_REACH_WITH_MELEE:
        {
            float reach = GetCombinedCombatReach(distcalc == DIST_CALC_COMBAT_REACH_WITH_MELEE);
            dist = sqrt(dist) - reach;
            return dist > 0.0f ? dist : 0.0f;
        }
        default: return dist;
    }
}

float WorldObject::GetDistanceZ(const WorldObject* obj) const
{
    float dz = fabs(GetPositionZ() - obj->GetPositionZ());
    float sizefactor = GetCombatReach() + obj->GetCombatReach();
    float dist = dz - sizefactor;
    return dist > 0 ? dist : 0;
}

bool WorldObject::IsWithinDist3d(float x, float y, float z, float dist2compare) const
{
    float distsq = GetDistance(x, y, z, DIST_CALC_NONE);
    float sizefactor = GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinDist2d(float x, float y, float dist2compare) const
{
    float distsq = GetDistance2d(x, y, DIST_CALC_NONE);
    float sizefactor = GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool WorldObject::_IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const
{
    float distsq = GetDistance(obj, is3D, DIST_CALC_NONE);
    float sizefactor = GetCombatReach() + obj->GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool WorldObject::_IsWithinCombatDist(WorldObject const* obj, float dist2compare, bool is3D) const
{
    float distsq = GetDistance(obj, is3D, DIST_CALC_NONE);
    float sizefactor = GetCombinedCombatReach(obj);
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinLOSInMap(const WorldObject* obj, bool ignoreM2Model) const
{
    if (!IsInMap(obj)) return false;
    float x, y, z;
    obj->GetPosition(x, y, z);
    return IsWithinLOS(x, y, z + obj->GetCollisionHeight(), ignoreM2Model);
}

bool WorldObject::IsWithinLOS(float ox, float oy, float oz, bool ignoreM2Model) const
{
    float x, y, z;
    GetPosition(x, y, z);
    return GetMap()->IsInLineOfSight(x, y, z + GetCollisionHeight(), ox, oy, oz, GetPhaseMask(), ignoreM2Model);
}

bool WorldObject::IsWithinLOSForMe(float ox, float oy, float oz, float collisionHeight, bool ignoreM2Model) const
{
    float x, y, z;
    GetPosition(x, y, z);
    return GetMap()->IsInLineOfSight(x, y, z + collisionHeight, ox, oy, oz + collisionHeight, GetPhaseMask(), ignoreM2Model);
}

bool WorldObject::GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D /* = true */, DistanceCalculation distcalc /* = NONE */) const
{
    float distsq1 = GetDistance(obj1, is3D, distcalc);
    float distsq2 = GetDistance(obj2, is3D, distcalc);
    return distsq1 < distsq2;
}

bool WorldObject::IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D /* = true */, bool combat /*= false*/) const
{
    float distsq = GetDistance(obj, is3D, DIST_CALC_NONE);
    float sizefactor;
    if (combat)
        sizefactor = GetObjectBoundingRadius() + obj->GetObjectBoundingRadius();
    else
        sizefactor = GetCombatReach() + obj->GetCombatReach();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange2d(float x, float y, float minRange, float maxRange, bool combat /*= false*/) const
{
    float distsq = GetDistance2d(x, y, DIST_CALC_NONE);
    float sizefactor;
    if (combat)
        sizefactor = GetObjectBoundingRadius();
    else
        sizefactor = GetCombatReach();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange3d(float x, float y, float z, float minRange, float maxRange, bool combat /*= false*/) const
{
    float distsq = GetDistance(x, y, z, DIST_CALC_NONE);
    float sizefactor;
    if (combat)
        sizefactor = GetObjectBoundingRadius();
    else
        sizefactor = GetCombatReach();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

// Return angle in range 0..2*pi
float WorldObject::GetAngleAt(float x, float y, float ox, float oy)
{
    float dx = (ox - x);
    float dy = (oy - y);

    float ang = std::atan2(dy, dx);                              // returns value between -Pi..Pi
    ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
    return ang;
}

float WorldObject::GetAngle(float x, float y) const
{
    return m_position.GetAngle(x, y);
}

float WorldObject::GetAngleAt(float x, float y, const WorldObject* obj) const
{
    if (!obj)
        return 0.0f;

    // Rework the assert, when more cases where such a call can happen have been fixed
    // MANGOS_ASSERT(obj != this || PrintEntryError("GetAngle (for self)"));
    if (obj == this)
    {
        sLog.outError("INVALID CALL for GetAngle for %s", obj->GetGuidStr().c_str());
        return 0.0f;
    }
    return GetAngleAt(x, y, obj->GetPositionX(), obj->GetPositionY());
}

float WorldObject::GetAngle(const WorldObject* obj) const
{
    if (!obj)
        return 0.0f;

    return GetAngleAt(GetPositionX(), GetPositionY(), obj->GetPositionX(), obj->GetPositionY());
}

bool WorldObject::HasInArcAt(float x, float y, float o, const WorldObject* target, float arc/* = M_PI_F*/) const
{
    // always have self in arc
    if (target == this)
        return true;

    // move arc to range 0.. 2*pi
    arc = MapManager::NormalizeOrientation(arc);

    float angle = GetAngleAt(x, y, target);
    angle -= o;

    // move angle to range -pi ... +pi
    angle = MapManager::NormalizeOrientation(angle);
    if (angle > M_PI_F)
        angle -= 2.0f * M_PI_F;

    float lborder =  -1 * (arc / 2.0f);                     // in range -pi..0
    float rborder = (arc / 2.0f);                           // in range 0..pi
    return ((angle >= lborder) && (angle <= rborder));
}

bool WorldObject::HasInArc(const WorldObject* target, float arc/* = M_PI_F*/) const
{
    return HasInArcAt(GetPositionX(), GetPositionY(), GetOrientation(), target, arc);
}

bool WorldObject::IsFacingTargetsBack(const WorldObject* target, float arc /*= M_PI_F*/) const
{
    if (!target)
        return false;

    //if target is facing the current object then we know its not possible that the current object would be facing the targets back
    if (target->HasInArc(this, arc))
        return false;

    //if current object is not facing the target then we know the current object is not facing the target at all
    if (!this->HasInArc(target, arc))
        return false;

    return true;
}

bool WorldObject::IsFacingTargetsFront(const WorldObject* target, float arc /*= M_PI_F*/) const
{
    if (!target)
        return false;

    //if target is not facing the current object then we know its not possible that the current object would be facing the targets front
    if (!target->HasInArc(this, arc))
        return false;

    //if current object is not facing the target then we know the current object is not facing the target at all
    if (!this->HasInArc(target, arc))
        return false;

    return true;
}

bool WorldObject::isInFrontInMap(WorldObject const* target, float distance,  float arc /*= M_PI_F*/) const
{
    return IsInMap(target) && isInFront(target, distance, arc);
}

bool WorldObject::isInBackInMap(WorldObject const* target, float distance, float arc /*= M_PI_F*/) const
{
    return IsInMap(target) && isInBack(target, distance, arc);
}

bool WorldObject::isInFront(WorldObject const* target, float distance,  float arc /*= M_PI_F*/) const
{
    return target->GetDistance(GetPositionX(), GetPositionY(), GetPositionZ(), DIST_CALC_COMBAT_REACH) <= distance && HasInArc(target, arc);
}

bool WorldObject::isInBack(WorldObject const* target, float distance, float arc /*= M_PI_F*/) const
{
    return target->GetDistance(GetPositionX(), GetPositionY(), GetPositionZ(), DIST_CALC_COMBAT_REACH) <= distance && !HasInArc(target, 2 * M_PI_F - arc);
}

Position WorldObject::GetFirstRandomAngleCollisionPosition(float dist, float angle)
{
    Position pos;
    for (uint32 i = 0; i < 10; ++i)
    {
        GetFirstCollisionPosition(pos, dist, angle);
        if (GetPosition().GetDistance(pos) > dist * 0.8f) // if at least 80% distance, good enough
            break;
        angle += (M_PI_F / 5); // else try slightly different angle
    }
    return pos;
}

void WorldObject::GetRandomPoint(float x, float y, float z, float distance, float& rand_x, float& rand_y, float& rand_z, float minDist /*=0.0f*/, float const* ori /*=nullptr*/) const
{
    if (distance == 0)
    {
        rand_x = x;
        rand_y = y;
        rand_z = z;
        return;
    }

    // angle to face `obj` to `this`
    float angle;
    if (!ori)
        angle = rand_norm_f() * 2 * M_PI_F;
    else
        angle = *ori;

    float new_dist;
    if (minDist == 0.0f)
        new_dist = rand_norm_f() * distance;
    else
        new_dist = minDist + rand_norm_f() * (distance - minDist);

    rand_x = x + new_dist * cos(angle);
    rand_y = y + new_dist * sin(angle);
    rand_z = z;

    MaNGOS::NormalizeMapCoord(rand_x);
    MaNGOS::NormalizeMapCoord(rand_y);
    UpdateAllowedPositionZ(rand_x, rand_y, rand_z);          // update to LOS height if available
}

void WorldObject::UpdateGroundPositionZ(float x, float y, float& z) const
{
    float new_z = GetMap()->GetHeight(GetPhaseMask(), x, y, z);
    if (new_z > INVALID_HEIGHT)
        z = new_z + 0.05f;                                  // just to be sure that we are not a few pixel under the surface
}

void WorldObject::UpdateAllowedPositionZ(float x, float y, float& z, Map* atMap /*=nullptr*/) const
{
    if (!atMap)
        atMap = GetMap();

    float ground_z = atMap->GetHeight(GetPhaseMask(), x, y, z);
    if (ground_z > INVALID_HEIGHT)
        z = ground_z;
}

void WorldObject::MovePositionToFirstCollision(Position& pos, float dist, float angle)
{
    float destX = pos.x + dist * cos(angle);
    float destY = pos.y + dist * sin(angle);
    float destZ = pos.z;

    GenericTransport* transport = GetTransport();

    float halfHeight = GetCollisionHeight();
    if (IsUnit())
    {
        PathFinder path(static_cast<Unit*>(this));
        Vector3 src(pos.x, pos.y, pos.z);
        Vector3 dest(destX, destY, destZ + halfHeight);
        if (transport) // need to use offsets for PF check
        {
            transport->CalculatePassengerOffset(src.x, src.y, src.z);
            transport->CalculatePassengerOffset(dest.x, dest.y, dest.z);
        }
        UpdateAllowedPositionZ(dest.x, dest.y, dest.z);
        path.calculate(src, dest, false, true);
        if ((path.getPathType() & PATHFIND_NOPATH) == 0)
        {
            G3D::Vector3 result = path.getPath().back();
            destX = result.x;
            destY = result.y;
            destZ = result.z;
            // no collision detected - reset height
            if (dest.z == result.z)
                AdjustZForCollision(destX, destY, destZ, halfHeight);
            if (transport) // transport produces offset, but we need global pos
                transport->CalculatePassengerPosition(destX, destY, destZ);
        }
    }

    UpdateAllowedPositionZ(destX, destY, destZ);
    destZ += halfHeight;
    bool colPoint = GetMap()->GetHitPosition(pos.x, pos.y, pos.z + halfHeight, destX, destY, destZ, GetPhaseMask(), -0.5f);
    destZ -= halfHeight;

    if (colPoint)
    {
        destX -= CONTACT_DISTANCE * cos(angle);
        destY -= CONTACT_DISTANCE * sin(angle);
        dist = sqrt((pos.x - destX) * (pos.x - destX) + (pos.y - destY) * (pos.y - destY));
    }

    colPoint = GetMap()->GetHitPosition(destX, destY, destZ + halfHeight, destX, destY, destZ, GetPhaseMask(), -0.5f);
    if (colPoint)
        dist = sqrt((pos.x - destX) * (pos.x - destX) + (pos.y - destY) * (pos.y - destY));

    float step = dist / 10.0f;
    Position tempPos(destX, destY, destZ, 0.f);
    bool distanceZSafe = true;
    float previousZ = destZ;

    for (int i = 0; i < 10; i++)
    {
        if (fabs(pos.z - destZ) > ATTACK_DISTANCE)
        {
            previousZ = destZ;
            destX -= step * cos(angle);
            destY -= step * sin(angle);
            UpdateAllowedPositionZ(destX, destY, destZ);
            if (fabs(previousZ - destZ) > (ATTACK_DISTANCE / 2))
                distanceZSafe = false;
        }
        else
        {
            pos.x = destX;
            pos.y = destY;
            pos.z = destZ;
            break;
        }
    }

    if (distanceZSafe)
        pos = tempPos;

    MaNGOS::NormalizeMapCoord(pos.x);
    MaNGOS::NormalizeMapCoord(pos.y);
    UpdateAllowedPositionZ(pos.x, pos.y, pos.z);
    pos.o = m_position.o;
}

float WorldObject::GetCombinedCombatReach(WorldObject const* pVictim, bool forMeleeRange, float flat_mod) const
{
    return GetCombinedCombatReach(forMeleeRange, flat_mod + pVictim->GetCombatReach());
}

float WorldObject::GetCombinedCombatReach(bool forMeleeRange, float flat_mod) const
{
    // The measured values show BASE_MELEE_OFFSET in (1.3224, 1.342)
    float reach = GetCombatReach() + flat_mod;

    if (forMeleeRange)
    {
        reach += BASE_MELEERANGE_OFFSET;
        if (reach < ATTACK_DISTANCE)
            reach = ATTACK_DISTANCE;
    }

    return reach;
}

bool WorldObject::IsPositionValid() const
{
    return MaNGOS::IsValidMapCoord(m_position.x, m_position.y, m_position.z, m_position.o);
}

void WorldObject::MonsterSay(char const* text, uint32 language, Unit const* target) const
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_SAY, text, Language(language), CHAT_TAG_NONE, GetObjectGuid(), GetName(),
                                 target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(data, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY), true);
}

void WorldObject::MonsterYell(const char* text, uint32 /*language*/, Unit const* target) const
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_YELL, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
                                 target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(data, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL), true);
}

void WorldObject::MonsterTextEmote(const char* text, Unit const* target, bool IsBossEmote) const
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
                                 target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(data, sWorld.getConfig(IsBossEmote ? CONFIG_FLOAT_LISTEN_RANGE_YELL : CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE), true);
}

void WorldObject::MonsterWhisper(const char* text, Unit const* target, bool IsBossWhisper) const
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
                                 target->GetObjectGuid(), target->GetName());
    ((Player*)target)->GetSession()->SendPacket(data);
}

namespace MaNGOS
{
    class MonsterChatBuilder
    {
        public:
            MonsterChatBuilder(WorldObject const& obj, ChatMsg msgtype, std::vector<std::string> content, Language language, Unit const* target)
                : i_object(obj), i_msgtype(msgtype), i_content(content), i_language(language), i_target(target) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = nullptr;
                if ((int32)i_content.size() > loc_idx + 1 && !i_content[loc_idx + 1].empty())
                    text = i_content[loc_idx + 1].c_str();
                else
                    text = i_content[0].c_str();

                ChatHandler::BuildChatPacket(data, i_msgtype, text, i_language, CHAT_TAG_NONE, i_object.GetObjectGuid(), i_object.GetNameForLocaleIdx(loc_idx),
                                             i_target ? i_target->GetObjectGuid() : ObjectGuid(), i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
            }

        private:
            WorldObject const& i_object;
            ChatMsg i_msgtype;
            std::vector<std::string> i_content;
            Language i_language;
            Unit const* i_target;
    };
}                                                           // namespace MaNGOS

/// Helper function to create localized around a source
void _DoLocalizedTextAround(WorldObject const* source, std::vector<std::string> content, ChatMsg msgtype, Language language, Unit const* target, float range)
{
    MaNGOS::MonsterChatBuilder say_build(*source, msgtype, content, language, target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> > say_worker(source, range, say_do);
    Cell::VisitWorldObjects(source, say_worker, range);
}

/// Function that sends a text associated to a MangosString or BroadcastText
void WorldObject::MonsterText(std::vector<std::string> content, uint32 type, Language lang, Unit const* target) const
{
    switch (type)
    {
        case CHAT_TYPE_SAY:
            _DoLocalizedTextAround(this, content, CHAT_MSG_MONSTER_SAY, lang, target, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY));
            break;
        case CHAT_TYPE_YELL:
            _DoLocalizedTextAround(this, content, CHAT_MSG_MONSTER_YELL, lang, target, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL));
            break;
        case CHAT_TYPE_TEXT_EMOTE:
            _DoLocalizedTextAround(this, content, CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, target, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE));
            break;
        case CHAT_TYPE_BOSS_EMOTE:
            _DoLocalizedTextAround(this, content, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, target, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL));
            break;
        case CHAT_TYPE_WHISPER:
        {
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return;
            MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_WHISPER, content, LANG_UNIVERSAL, target);
            MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
            say_do((Player*)target);
            break;
        }
        case CHAT_TYPE_BOSS_WHISPER:
        {
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return;
            MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_RAID_BOSS_WHISPER, content, LANG_UNIVERSAL, target);
            MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
            say_do((Player*)target);
            break;
        }
        case CHAT_TYPE_ZONE_YELL:
        {
            MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, content, lang, target);
            MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
            uint32 zoneId = GetZoneId();
            GetMap()->ExecuteMapWorkerZone(zoneId, std::bind(&MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder>::operator(), &say_do, std::placeholders::_1));
            break;
        }
        case CHAT_TYPE_ZONE_EMOTE:
        {
            MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_EMOTE, content, lang, target);
            MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
            uint32 zoneId = GetZoneId();
            GetMap()->ExecuteMapWorkerZone(zoneId, std::bind(&MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder>::operator(), &say_do, std::placeholders::_1));
            break;
        }
        case CHAT_TYPE_PARTY:
        {
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return;

            if (Group* group = ((Player*)target)->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    Player* groupMember = itr->getSource();

                    if (!groupMember)
                        continue;

                    MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_PARTY, content, LANG_UNIVERSAL, groupMember);
                    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
                    say_do(groupMember);
                }
            }
            break;
        }
    }
}

void WorldObject::SendMessageToSet(WorldPacket const& data, bool /*bToSelf*/) const
{
    // if object is in world, map for it already created!
    if (IsInWorld())
        GetMap()->MessageBroadcast(this, data);
}

void WorldObject::SendMessageToSetInRange(WorldPacket const& data, float dist, bool /*bToSelf*/) const
{
    // if object is in world, map for it already created!
    if (IsInWorld())
        GetMap()->MessageDistBroadcast(this, data, dist);
}

void WorldObject::SendMessageToSetExcept(WorldPacket const& data, Player const* skipped_receiver) const
{
    // if object is in world, map for it already created!
    if (IsInWorld())
    {
        MaNGOS::MessageDelivererExcept notifier(this, data, skipped_receiver);
        Cell::VisitWorldObjects(this, notifier, GetMap()->GetVisibilityDistance());
    }
}

void WorldObject::SendMessageToAllWhoSeeMe(WorldPacket const& data, bool /*self*/) const
{
    if (IsInWorld())
        for (ObjectGuid guid : m_clientGUIDsIAmAt)
            if (Player* player = GetMap()->GetPlayer(guid))
                player->GetSession()->SendPacket(data);
}

void WorldObject::SendObjectDeSpawnAnim(ObjectGuid guid) const
{
    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
    data << ObjectGuid(guid);
    SendMessageToSet(data, true);
}

void WorldObject::SendGameObjectCustomAnim(ObjectGuid guid, uint32 animId /*= 0*/) const
{
    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 8 + 4);
    data << ObjectGuid(guid);
    data << uint32(animId);
    SendMessageToSet(data, true);
}

void WorldObject::SetMap(Map* map)
{
    MANGOS_ASSERT(map);
    m_currMap = map;
    // lets save current map's Id/instanceId
    m_mapId = map->GetId();
    m_InstanceId = map->GetInstanceId();
}

void WorldObject::AddToWorld()
{
    if (m_isOnEventNotified)
        m_currMap->AddToOnEventNotified(this);

    if (!m_stringIds.empty())
        for (uint32 stringId : m_stringIds)
            m_currMap->AddStringIdObject(stringId, this);

    Object::AddToWorld();
}

void WorldObject::RemoveFromWorld()
{
    if (!IsPlayer()) // players have their own logic due to cross map transports
        if (GenericTransport* transport = GetTransport())
            transport->RemovePassenger(this);

    if (IsInWorld())
    {
        if (m_isOnEventNotified)
            m_currMap->RemoveFromOnEventNotified(this);

        if (!m_stringIds.empty())
            for (uint32 stringId : m_stringIds)
                m_currMap->RemoveStringIdObject(stringId, this);
    }

    Object::RemoveFromWorld();
}

TerrainInfo const* WorldObject::GetTerrain() const
{
    MANGOS_ASSERT(m_currMap);
    return m_currMap->GetTerrain();
}

void WorldObject::AddObjectToRemoveList()
{
    GetMap()->AddObjectToRemoveList(this);
}

void WorldObject::GetPosition(float& x, float& y, float& z, GenericTransport* transport) const
{
    if (transport && m_movementInfo.t_guid == transport->GetObjectGuid())
    {
        x = m_movementInfo.t_pos.x;
        y = m_movementInfo.t_pos.y;
        z = m_movementInfo.t_pos.z;
        return;
    }
    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();
    if (transport)
        transport->CalculatePassengerOffset(x, y, z);
}

Creature* WorldObject::SummonCreature(TempSpawnSettings settings, Map* map, uint32 phaseMask)
{
    CreatureInfo const* cinfo = ObjectMgr::GetCreatureTemplate(settings.entry);
    if (!cinfo)
    {
        sLog.outErrorDb("WorldObject::SummonCreature: Creature (Entry: %u) not existed for summoner: %s. ", settings.entry, settings.spawner ? settings.spawner->GetGuidStr().c_str() : ObjectGuid().GetString().data());
        return nullptr;
    }

    TemporarySpawn* creature;
    if (!settings.tempSpawnMovegen)
        creature = new TemporarySpawn(settings.spawner ? settings.spawner->GetObjectGuid() : ObjectGuid());
    else
        creature = new TemporarySpawnWaypoint(settings.spawner ? settings.spawner->GetObjectGuid() : ObjectGuid(), settings.waypointId, settings.spawnPathId, settings.pathOrigin);

    GenericTransport* transport = nullptr;
    if (settings.spawner)
        transport = settings.spawner->GetTransport();
    float x = settings.x, y = settings.y, z = settings.z;
    if (transport)
        transport->CalculatePassengerPosition(x, y, z);

    CreatureCreatePos pos(map, x, y, z, settings.ori, phaseMask);

    if (settings.x == 0.0f && settings.y == 0.0f && settings.z == 0.0f && settings.spawner)
    {
        float dist = settings.forcedOnTop ? 0.0f : CONTACT_DISTANCE;
        pos = CreatureCreatePos(settings.spawner, settings.spawner->GetOrientation(), dist, settings.ori);
    }
    uint32 lowGuid = map->GenerateLocalLowGuid(cinfo->GetHighGuid());
    if (!creature->Create(lowGuid, lowGuid, pos, cinfo))
    {
        delete creature;
        return nullptr;
    }

    creature->SetRespawnCoord(pos);
    if (transport)
    {
        creature->m_movementInfo.t_pos.x = settings.x;
        creature->m_movementInfo.t_pos.y = settings.y;
        creature->m_movementInfo.t_pos.z = settings.z;
        creature->m_movementInfo.t_pos.o = settings.ori;
    }

    // Set run or walk before any other movement starts
    creature->SetWalk(!settings.setRun);

    // Active state set before added to map
    creature->SetActiveObjectState(settings.activeObject);

    if (settings.faction)
        creature->setFaction(settings.faction);

    if (settings.modelId)
        creature->SetDisplayId(settings.modelId);

    if (settings.spawnCounting)
        creature->SetSpawnCounting(true);

    creature->GetMotionMaster()->SetDefaultPathId(settings.pathId);
    if (settings.movegen != -1)
        creature->SetDefaultMovementType(MovementGeneratorType(settings.movegen));

    if (settings.spawner)
    {
        if (GenericTransport* transport = settings.spawner->GetTransport())
        {
            transport->AddPassenger(creature);
            transport->UpdatePassengerPosition(creature);
        }
    }

    uint32 relayId = 0;
    if (settings.spawnDataEntry)
    {
        if (CreatureSpawnTemplate const* templateData = sObjectMgr.GetCreatureSpawnTemplate(settings.spawnDataEntry))
        {
            if (templateData->npcFlags != -1)
                creature->SetUInt32Value(UNIT_NPC_FLAGS, uint32(templateData->npcFlags));
            if (templateData->unitFlags != -1)
                creature->SetUInt32Value(UNIT_FIELD_FLAGS, uint32(templateData->unitFlags));
            if (templateData->faction > 0)
                creature->SetPower(POWER_MANA, templateData->curMana);
            if (templateData->modelId > 0)
                creature->SetDisplayId(templateData->modelId);
            if (templateData->equipmentId != -1)
                creature->LoadEquipment(templateData->equipmentId, true);
            if (templateData->curHealth > 1)
                creature->SetHealth(templateData->curHealth);
            if (templateData->curMana > 0)
                creature->SetPower(POWER_MANA, templateData->curMana);
            if (templateData->IsRunning())
                creature->SetWalk(false);
            if (templateData->IsHovering())
                creature->SetHover(true);
            if (templateData->IsGravityDisabled())
                creature->SetLevitate(true);
            relayId = templateData->relayId;
            if (templateData->stringId)
                creature->SetStringId(templateData->stringId, true);
        }
    }

    if (settings.spellId)
        creature->SetUInt32Value(UNIT_CREATED_BY_SPELL, settings.spellId);

    if (settings.level)
        creature->SelectLevel(settings.level);

    if (settings.ownerGuid)
        creature->SetOwnerGuid(settings.ownerGuid);

    // intended only for visual way point debug feature
    if (settings.waypointId)
        creature->SetLevel(settings.waypointId);

    creature->Summon(settings.spawnType, settings.despawnTime);                  // Also initializes the AI and MMGen

    if (relayId)
        map->ScriptsStart(SCRIPT_TYPE_RELAY, relayId, creature, settings.dbscriptTarget);

    if (settings.corpseDespawnTime)
        creature->SetCorpseDelay(settings.corpseDespawnTime);

    if (settings.spawner && settings.spawner->GetTypeId() == TYPEID_UNIT)
        if (Creature* spawnerCreature = static_cast<Creature*>(settings.spawner))
            if (UnitAI* ai = spawnerCreature->AI())
                ai->JustSummoned(creature);

    // Creature Linking, Initial load is handled like respawn
    if (creature->IsLinkingEventTrigger())
        map->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, creature);

    // return the creature therewith the summoner has access to it
    return creature;
}

Creature* WorldObject::SummonCreature(uint32 id, float x, float y, float z, float ang, TempSpawnType spwtype, uint32 despwtime, bool asActiveObject, bool setRun, uint32 pathId, uint32 faction, uint32 modelId, bool spawnCounting, bool forcedOnTop)
{
    return WorldObject::SummonCreature(TempSpawnSettings(this, id, x, y, z, ang, spwtype, despwtime, asActiveObject, setRun, pathId, faction, modelId, spawnCounting, forcedOnTop), GetMap(), GetPhaseMask());
}

GameObject* WorldObject::SpawnGameObject(uint32 dbGuid, Map* map, uint32 forcedEntry, GenericTransport* transport)
{
    GameObjectData const* data = sObjectMgr.GetGOData(dbGuid);
    if (!data)
        return nullptr;

    if (data->spawnMask && !map->CanSpawn(TYPEID_GAMEOBJECT, dbGuid))
        return nullptr;

    GameObject* gameobject = GameObject::CreateGameObject(forcedEntry ? forcedEntry : data->id);
    if (!gameobject->LoadFromDB(dbGuid, map, 0, forcedEntry, transport))
    {
        delete gameobject;
        return nullptr;
    }
    return gameobject;
}

Creature* WorldObject::SpawnCreature(uint32 dbGuid, Map* map, uint32 forcedEntry, GenericTransport* transport)
{
    CreatureData const* data = sObjectMgr.GetCreatureData(dbGuid);
    if (!data)
    {
        sLog.outErrorDb("Creature (GUID: %u) not found in table `creature`, can't load. ", dbGuid);
        return nullptr;
    }

    uint32 entry = forcedEntry ? forcedEntry : data->id;

    if (data->spawnMask && !map->CanSpawn(TYPEID_UNIT, dbGuid))
        return nullptr;

    Creature* creature = new Creature;
    // DEBUG_LOG("Spawning creature %u",*itr);
    if (!creature->LoadFromDB(dbGuid, map, 0, forcedEntry, transport))
    {
        delete creature;
        return nullptr;
    }
    return creature;
}

// how much space should be left in front of/ behind a mob that already uses a space
#define OCCUPY_POS_DEPTH_FACTOR                          1.8f

namespace MaNGOS
{
    class NearUsedPosDo
    {
        public:
            NearUsedPosDo(WorldObject const& obj, WorldObject const* searcher, float absAngle, ObjectPosSelector& selector)
                : i_object(obj), i_searcher(searcher), i_absAngle(MapManager::NormalizeOrientation(absAngle)), i_selector(selector) {}

            void operator()(Corpse*) const {}
            void operator()(DynamicObject*) const {}

            void operator()(Creature* c) const
            {
                // skip self or target
                if (c == i_searcher || c == &i_object)
                    return;

                float x, y, z;

                if (c->IsStopped() || !c->GetMotionMaster()->GetDestination(x, y, z))
                {
                    x = c->GetPositionX();
                    y = c->GetPositionY();
                }

                add(c, x, y);
            }

            template<class T>
            void operator()(T* u) const
            {
                // skip self or target
                if (u == i_searcher || u == &i_object)
                    return;

                float x = u->GetPositionX();
                float y = u->GetPositionY();

                add(u, x, y);
            }

            // we must add used pos that can fill places around center
            void add(WorldObject* u, float x, float y) const
            {
                float dx = i_object.GetPositionX() - x;
                float dy = i_object.GetPositionY() - y;
                float dist2d = sqrt((dx * dx) + (dy * dy));

                // It is ok for the objects to require a bit more space
                float delta = u->GetObjectBoundingRadius();
                if (i_selector.m_searchPosFor && i_selector.m_searchPosFor != u)
                    delta += i_selector.m_searchPosFor->GetObjectBoundingRadius();

                delta *= OCCUPY_POS_DEPTH_FACTOR;           // Increase by factor

                // u is too near/far away from i_object. Do not consider it to occupy space
                if (fabs(i_selector.m_searcherDist - dist2d) > delta)
                    return;

                float angle = i_object.GetAngle(u) - i_absAngle;

                // move angle to range -pi ... +pi, range before is -2Pi..2Pi
                if (angle > M_PI_F)
                    angle -= 2.0f * M_PI_F;
                else if (angle < -M_PI_F)
                    angle += 2.0f * M_PI_F;

                i_selector.AddUsedArea(u, angle, dist2d);
            }
        private:
            WorldObject const& i_object;
            WorldObject const* i_searcher;
            float              i_absAngle;
            ObjectPosSelector& i_selector;
    };
}                                                           // namespace MaNGOS

//===================================================================================================

void WorldObject::GetNearPoint2dAt(const float posX, const float posY, float& x, float& y, float distance2d, float absAngle)
{
    x = (posX + (distance2d * cosf(absAngle)));
    y = (posY + (distance2d * sinf(absAngle)));

    MaNGOS::NormalizeMapCoord(x);
    MaNGOS::NormalizeMapCoord(y);
}

void WorldObject::GetNearPointAt(const float posX, const float posY, const float posZ, WorldObject const* searcher, float& x, float& y, float& z, float searcher_bounding_radius, float distance2d, float absAngle, bool isInWater) const
{
    GetNearPoint2dAt(posX, posY, x, y, distance2d, absAngle);
    const float init_z = z = posZ;

    // if detection disabled, return first point
    if (!sWorld.getConfig(CONFIG_BOOL_DETECT_POS_COLLISION))
    {
        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z, GetMap()); // update to LOS height if available
        else if (!isInWater)
            UpdateGroundPositionZ(x, y, z);
        return;
    }

    // or remember first point
    float first_x = x;
    float first_y = y;
    bool first_los_conflict = false;                        // first point LOS problems

    const float dist = distance2d + searcher_bounding_radius + GetObjectBoundingRadius();

    // prepare selector for work
    ObjectPosSelector selector(posX, posY, distance2d, searcher_bounding_radius, searcher);

    // adding used positions around object - unused because its not blizzlike
    //{
        //MaNGOS::NearUsedPosDo u_do(*this, searcher, absAngle, selector);
        //MaNGOS::WorldObjectWorker<MaNGOS::NearUsedPosDo> worker(this, u_do);

    //    Cell::VisitAllObjects(this, worker, dist);
    //}

    // maybe can just place in primary position
    float collisionHeight = GetCollisionHeight();
    if (selector.CheckOriginalAngle())
    {
        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z, GetMap()); // update to LOS height if available
        else if (!isInWater)
            UpdateGroundPositionZ(x, y, z);

        if (fabs(init_z - z) < dist && IsWithinLOSForMe(x, y, z, collisionHeight))
            return;

        first_los_conflict = true;                          // first point have LOS problems
    }

    // set first used pos in lists
    selector.InitializeAngle();

    float angle;                                            // candidate of angle for free pos

    // select in positions after current nodes (selection one by one)
    while (selector.NextAngle(angle))                       // angle for free pos
    {
        GetNearPoint2dAt(posX, posY, x, y, distance2d, absAngle + angle);
        z = posZ;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z, GetMap()); // update to LOS height if available
        else if (!isInWater)
            UpdateGroundPositionZ(x, y, z);

        if (fabs(init_z - z) < dist && IsWithinLOSForMe(x, y, z, collisionHeight))
            return;
    }

    // BAD NEWS: not free pos (or used or have LOS problems)
    // Attempt find _used_ pos without LOS problem
    if (!first_los_conflict)
    {
        x = first_x;
        y = first_y;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z, GetMap()); // update to LOS height if available
        else if (!isInWater)
            UpdateGroundPositionZ(x, y, z);
        return;
    }

    // set first used pos in lists
    selector.InitializeAngle();

    // select in positions after current nodes (selection one by one)
    while (selector.NextUsedAngle(angle))                   // angle for used pos but maybe without LOS problem
    {
        GetNearPoint2dAt(posX, posY, x, y, distance2d, absAngle + angle);
        z = posZ;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z, GetMap()); // update to LOS height if available
        else if (!isInWater)
            UpdateGroundPositionZ(x, y, z);

        if (fabs(init_z - z) < dist && IsWithinLOSForMe(x, y, z, collisionHeight))
            return;
    }

    // BAD BAD NEWS: all found pos (free and used) have LOS problem :(
    x = first_x;
    y = first_y;

    if (searcher)
        searcher->UpdateAllowedPositionZ(x, y, z, GetMap());// update to LOS height if available
    else if (!isInWater)
        UpdateGroundPositionZ(x, y, z);
}

void WorldObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    m_phaseMask = newPhaseMask;

    if (update && IsInWorld())
        UpdateVisibilityAndView();
}

void WorldObject::PlayDistanceSound(uint32 sound_id, PlayPacketParameters parameters /*= PlayPacketParameters(PLAY_SET)*/) const
{
    WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 4 + 8);
    data << uint32(sound_id);
    data << GetObjectGuid();
    HandlePlayPacketSettings(data, parameters);
}

void WorldObject::PlayDirectSound(uint32 sound_id, PlayPacketParameters parameters /*= PlayPacketParameters(PLAY_SET)*/) const
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(sound_id);
    HandlePlayPacketSettings(data, parameters);
}

void WorldObject::PlayMusic(uint32 sound_id, PlayPacketParameters parameters /*= PlayPacketParameters(PLAY_SET)*/) const
{
    WorldPacket data(SMSG_PLAY_MUSIC, 4);
    data << uint32(sound_id);
    HandlePlayPacketSettings(data, parameters);
}

void WorldObject::PlaySpellVisual(uint32 artKitId, PlayPacketParameters parameters /*= PlayPacketParameters(PLAY_SET)*/) const
{
    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 4);
    data << GetObjectGuid();
    data << artKitId; // index from SpellVisualKit.dbc
    HandlePlayPacketSettings(data, parameters);
}

void WorldObject::HandlePlayPacketSettings(WorldPacket& msg, PlayPacketParameters& parameters) const
{
    switch (parameters.setting)
    {
        case PlayPacketSettings::SET:
            SendMessageToSet(msg, true);
            break;
        case PlayPacketSettings::TARGET:
            if (Player const* target = parameters.target.target)
                target->SendDirectMessage(msg);
            break;
        case PlayPacketSettings::MAP:
            if (IsInWorld())
                GetMap()->MessageMapBroadcast(this, msg);
            break;
        case PlayPacketSettings::ZONE:
            if (IsInWorld())
                GetMap()->MessageMapBroadcastZone(this, msg, parameters.areaOrZone.id);
            break;
        case PlayPacketSettings::AREA:
            if (IsInWorld())
                GetMap()->MessageMapBroadcastArea(this, msg, parameters.areaOrZone.id);
            break;
    }
}

void WorldObject::UpdateVisibilityAndView()
{
    GetMap()->AddUpdateCreateObject(this);
}

void WorldObject::AddToClientUpdateList()
{
    GetMap()->AddUpdateObject(this);
}

void WorldObject::RemoveFromClientUpdateList()
{
    GetMap()->RemoveUpdateObject(this);
}

struct WorldObjectCreateAccumulator
{
    WorldObject& i_object;
    PlayerSet i_playerSet;
    WorldObjectCreateAccumulator(WorldObject& obj) : i_object(obj)
    {

    }

    void Visit(CameraMapType& m)
    {
        for (auto& iter : m)
        {
            Player* owner = iter.getSource()->GetOwner();
#ifdef ENABLE_PLAYERBOTS
            if (owner->isRealPlayer())
            {
#endif
                if (owner != &i_object)
                {
                    if (!owner->HasAtClient(&i_object))
                    {
                        if (i_object.isVisibleForInState(owner, owner->GetCamera().GetBody(), false))
                        {
                            owner->AddAtClient(&i_object);
                            i_playerSet.insert(owner);
                        }
                    }
                }
#ifdef ENABLE_PLAYERBOTS
            }
#endif
        }
    }

    template<class SKIP> void Visit(GridRefManager<SKIP>&) {}
};

void WorldObject::BuildUpdateData(UpdateDataMapType& update_players)
{
    if (ItsNewObject())
        GetMap()->AddCameraToWorld(this);

    if (IsPlayer())
        BuildUpdateDataForPlayer((Player*)this, update_players);

    for (auto& iter : m_clientGUIDsIAmAt)
    {
        if (Player* player = GetMap()->GetPlayer(iter))
            if (player != this && player->HasAtClient(this))
                BuildUpdateDataForPlayer(player, update_players);
    }

    ClearUpdateMask(false);

    if (ItsNewObject())
        SetItsNewObject(false);
}

void WorldObject::UpdateVisibility(UpdateDataMapType& update_players)
{
    if (ItsNewObject())
        GetMap()->AddCameraToWorld(this);

    GetViewPoint().Call_UpdateVisibilityForOwner(update_players);

    GuidSet oor;
    for (auto itr = m_clientGUIDsIAmAt.begin(); itr != m_clientGUIDsIAmAt.end(); )
    {
        if (Player* client = GetMap()->GetPlayer(*itr))
        {
            if (!this->isVisibleForInState(client, client->GetCamera().GetBody(), false))
            {
                client->RemoveAtClient(this, true);
                oor.insert(*itr);
                itr = m_clientGUIDsIAmAt.erase(itr);
                continue;
            }
        }

        ++itr;
    }

    if (!oor.empty())
        GetMap()->AddUpdateRemoveObject(oor, this->GetObjectGuid());

    WorldObjectCreateAccumulator notifier(*this);
    Cell::VisitWorldObjects(this, notifier, GetVisibilityData().GetVisibilityDistance());
    GetMap()->AddCreateAtClientObjects(notifier.i_playerSet, this);
    ClearUpdateMask(false);
}

bool WorldObject::IsControlledByPlayer() const
{
    switch (GetTypeId())
    {
        case TYPEID_GAMEOBJECT:
            return ((GameObject*)this)->GetOwnerGuid().IsPlayer();
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
            return ((Unit*)this)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        case TYPEID_DYNAMICOBJECT:
            return ((DynamicObject*)this)->GetCasterGuid().IsPlayer();
        case TYPEID_CORPSE:
            return true;
        default:
            return false;
    }
}

bool WorldObject::PrintCoordinatesError(float x, float y, float z, char const* descr) const
{
    sLog.outError("%s with invalid %s coordinates: mapid = %uu, x = %f, y = %f, z = %f", GetGuidStr().c_str(), descr, GetMapId(), x, y, z);
    return false;                                           // always false for continue assert fail
}

void WorldObject::SetActiveObjectState(bool active)
{
    if (m_isActiveObject == active || (isType(TYPEMASK_PLAYER) && !active))  // player shouldn't became inactive, never
        return;

    if (IsInWorld() && !isType(TYPEMASK_PLAYER))
        // player's update implemented in a different from other active worldobject's way
        // it's considired to use generic way in future
    {
        if (isActiveObject() && !active)
            GetMap()->RemoveFromActive(this);
        else if (!isActiveObject() && active)
            GetMap()->AddToActive(this);
    }
    m_isActiveObject = active;
}

void WorldObject::SetNotifyOnEventState(bool state)
{
    if (state == m_isOnEventNotified)
        return;

    m_isOnEventNotified = state;

    if (!IsInWorld())
        return;

    if (state)
        GetMap()->AddToOnEventNotified(this);
    else
        GetMap()->RemoveFromOnEventNotified(this);
}

void WorldObject::AddGCD(SpellEntry const& spellEntry, uint32 forcedDuration /*= 0*/, bool /*updateClient = false*/)
{
    uint32 gcdRecTime = forcedDuration ? forcedDuration : spellEntry.StartRecoveryTime;
    if (!gcdRecTime)
        return;

    m_GCDCatMap.emplace(spellEntry.StartRecoveryCategory, std::chrono::milliseconds(gcdRecTime) + GetMap()->GetCurrentClockTime());
}

bool WorldObject::HasGCD(SpellEntry const* spellEntry) const
{
    if (spellEntry)
    {
        auto gcdItr = m_GCDCatMap.find(spellEntry->StartRecoveryCategory);
        return gcdItr != m_GCDCatMap.end() && (*gcdItr).second > GetMap()->GetCurrentClockTime();
    }
    return !m_GCDCatMap.empty();
}

TimePoint WorldObject::GetGCD(SpellEntry const* spellEntry) const
{
    if (spellEntry)
    {
        auto gcdItr = m_GCDCatMap.find(spellEntry->StartRecoveryCategory);
        if (gcdItr != m_GCDCatMap.end())
        return (*gcdItr).second;
    }

    return GetMap()->GetCurrentClockTime();
}

void WorldObject::AddCooldown(SpellEntry const& spellEntry, ItemPrototype const* /*itemProto = nullptr*/, bool /*permanent = false*/, uint32 forcedDuration /*= 0*/, bool ignoreCat /*= false*/)
{
    uint32 recTimeDuration = forcedDuration ? forcedDuration : spellEntry.RecoveryTime;
    uint32 catTimeDuration = ignoreCat ? 0 : spellEntry.CategoryRecoveryTime;
    if (recTimeDuration || catTimeDuration)
        m_cooldownMap.AddCooldown(GetMap()->GetCurrentClockTime(), spellEntry.Id, recTimeDuration, spellEntry.Category, catTimeDuration);
}

void WorldObject::UpdateCooldowns(TimePoint const& now)
{
    // handle GCD
    auto cdItr = m_GCDCatMap.begin();
    while (cdItr != m_GCDCatMap.end())
    {
        auto& cd = cdItr->second;
        if (cd <= now)
            cdItr = m_GCDCatMap.erase(cdItr);
        else
            ++cdItr;
    }

    // handle spell and category cooldowns
    m_cooldownMap.Update(now);

    // handle spell lockouts
    auto lockoutCDItr = m_lockoutMap.begin();
    while (lockoutCDItr != m_lockoutMap.end())
    {
        if (lockoutCDItr->second <= now)
            lockoutCDItr = m_lockoutMap.erase(lockoutCDItr);
        else
            ++lockoutCDItr;
    }
}

bool WorldObject::CheckLockout(SpellSchoolMask schoolMask, TimePoint const& now) const
{
    for (auto& lockoutItr : m_lockoutMap)
    {
        SpellSchoolMask lockoutSchoolMask = SpellSchoolMask(1 << lockoutItr.first);
        if ((lockoutSchoolMask & schoolMask) && lockoutItr.second > now)
            return true;
    }

    return false;
}

bool WorldObject::GetExpireTime(SpellEntry const& spellEntry, TimePoint& expireTime, bool& isPermanent) const
{
    auto spellItr = m_cooldownMap.FindBySpellId(spellEntry.Id);
    if (spellItr != m_cooldownMap.end())
    {
        auto& cdData = spellItr->second;
        if (cdData->IsPermanent())
        {
            isPermanent = true;
            return true;
        }

        TimePoint spellExpireTime = TimePoint();
        TimePoint catExpireTime = TimePoint();
        bool foundSpellCD = cdData->GetSpellCDExpireTime(spellExpireTime);
        bool foundCatCD = cdData->GetSpellCDExpireTime(catExpireTime);
        if (foundCatCD || foundSpellCD)
        {
            expireTime = spellExpireTime > catExpireTime ? spellExpireTime : catExpireTime;
            return true;
        }
    }
    return false;
}

bool WorldObject::IsSpellReady(SpellEntry const& spellEntry, ItemPrototype const* itemProto /*= nullptr*/) const
{
    uint32 spellCategory = spellEntry.Category;

    TimePoint now;
    if (IsInWorld())
        now = GetMap()->GetCurrentClockTime();
    else
        now = World::GetCurrentClockTime();

    // overwrite category by provided category in item prototype during item cast if need
    if (itemProto)
    {
        for (const auto& Spell : itemProto->Spells)
        {
            if (Spell.SpellId == spellEntry.Id)
            {
                spellCategory = Spell.SpellCategory;
                break;
            }
        }
    }

    {
        auto itr = m_cooldownMap.FindBySpellId(spellEntry.Id);
        if (itr != m_cooldownMap.end() && !(*itr).second->IsSpellCDExpired(now))
            if (!itemProto || itemProto->ItemId == (*itr).second.get()->GetItemId())
                return false;
    }

    if (spellCategory)
    {
        auto itr = m_cooldownMap.FindByCategory(spellCategory);
        if (itr != m_cooldownMap.end() && !(*itr).second->IsCatCDExpired(now))
            return false;
    }

    if (spellEntry.PreventionType == SPELL_PREVENTION_TYPE_SILENCE && CheckLockout(GetSpellSchoolMask(&spellEntry), now))
        return false;

    return true;
}

bool WorldObject::IsSpellReady(uint32 spellId, ItemPrototype const* itemProto /*= nullptr*/) const
{
    SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellEntry)
        return false;

    return IsSpellReady(*spellEntry, itemProto);
}

bool WorldObject::IsSpellOnPermanentCooldown(SpellEntry const& spellEntry) const
{
    TimePoint now;
    if (IsInWorld())
        now = GetMap()->GetCurrentClockTime();
    else
        now = World::GetCurrentClockTime();

    auto itr = m_cooldownMap.FindBySpellId(spellEntry.Id);
    if (itr != m_cooldownMap.end() && !(*itr).second->IsSpellCDExpired(now))
        return itr->second->IsPermanent();

    return false;
}

bool WorldObject::HasGCDOrCooldownWithinMargin(SpellEntry const& spellEntry, ItemPrototype const* itemProto /*= nullptr*/)
{
    uint64 diff = 0;
    auto gcdItr = m_GCDCatMap.find(spellEntry.StartRecoveryCategory);
    if (gcdItr != m_GCDCatMap.end())
        diff = std::max(diff, uint64(((*gcdItr).second - GetMap()->GetCurrentClockTime()).count()));

    uint32 spellCategory = spellEntry.Category;
    if (itemProto)
    {
        for (const auto& Spell : itemProto->Spells)
        {
            if (Spell.SpellId == spellEntry.Id)
            {
                spellCategory = Spell.SpellCategory;
                break;
            }
        }
    }

    auto itr = m_cooldownMap.FindBySpellId(spellEntry.Id);
    if (itr != m_cooldownMap.end() && !(*itr).second->IsSpellCDExpired(GetMap()->GetCurrentClockTime()))
    {
        if (!itemProto || itemProto->ItemId == (*itr).second.get()->GetItemId())
        {
            TimePoint timePoint;
            if ((*itr).second->GetSpellCDExpireTime(timePoint))
                diff = std::max(diff, uint64((timePoint - GetMap()->GetCurrentClockTime()).count()));
        }
    }

    if (spellCategory)
    {
        auto itr = m_cooldownMap.FindByCategory(spellCategory);
        if (itr != m_cooldownMap.end() && !(*itr).second->IsCatCDExpired(GetMap()->GetCurrentClockTime()))
        {
            TimePoint timePoint;
            if ((*itr).second->GetCatCDExpireTime(timePoint))
                diff = std::max(diff, uint64((timePoint - GetMap()->GetCurrentClockTime()).count()));
        }
    }

    if (spellEntry.PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
    {
        SpellSchoolMask spellSchoolMask = GetSpellSchoolMask(&spellEntry);
        for (auto& lockoutItr : m_lockoutMap)
        {
            SpellSchoolMask lockoutSchoolMask = SpellSchoolMask(1 << lockoutItr.first);
            if ((lockoutSchoolMask & spellSchoolMask) && lockoutItr.second > GetMap()->GetCurrentClockTime())
                diff = std::max(diff, uint64((lockoutItr.second - GetMap()->GetCurrentClockTime()).count()));
        }
    }

    if (diff < 50)
        return true;

    return false;
}

void WorldObject::LockOutSpells(SpellSchoolMask schoolMask, uint32 duration)
{
    for (uint32 i = 0; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (schoolMask & (1 << i))
            m_lockoutMap.emplace(SpellSchools(i), std::chrono::milliseconds(duration) + GetMap()->GetCurrentClockTime());
    }
}

void WorldObject::RemoveSpellCooldown(uint32 spellId, bool updateClient /*= true*/)
{
    SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellEntry)
        return;

    RemoveSpellCooldown(*spellEntry, updateClient);
}

void WorldObject::RemoveSpellCooldown(SpellEntry const& spellEntry, bool /*updateClient = true*/)
{
    m_cooldownMap.RemoveBySpellId(spellEntry.Id);
}

void WorldObject::RemoveSpellCategoryCooldown(uint32 category, bool /*updateClient = true*/)
{
    m_cooldownMap.RemoveByCategory(category);
}

void WorldObject::ResetGCD(SpellEntry const* spellEntry /*= nullptr*/)
{
    if (!spellEntry)
    {
        m_GCDCatMap.clear();
        return;
    }

    auto gcdItr = m_GCDCatMap.find(spellEntry->StartRecoveryCategory);
    if (gcdItr != m_GCDCatMap.end())
        m_GCDCatMap.erase(gcdItr);
}

void ConvertMillisecondToStr(std::chrono::milliseconds& duration, std::stringstream& durationStr)
{
    std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
    std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));
    durationStr << mm.count() << "m " << ss.count() << "s " << msec.count() << "ms";
}

void WorldObject::PrintCooldownList(ChatHandler& chat) const
{
    // print gcd
    auto now = GetMap()->GetCurrentClockTime();
    uint32 cdCount = 0;
    uint32 permCDCount = 0;

    for (auto& cdItr : m_GCDCatMap)
    {
        auto& cdData = cdItr.second;
        std::stringstream cdLine;
        std::stringstream durationStr;
        if (cdData > now)
        {
            auto cdDuration = cdData - now;
            ConvertMillisecondToStr(cdDuration, durationStr);
            ++cdCount;
        }
        else
            continue;

        cdLine << "GCD category" << "(" << cdItr.first << ") have " << durationStr.str() << " cd";
        chat.PSendSysMessage("%s", cdLine.str().c_str());
    }

    // print spell and category cd
    for (auto& cdItr : m_cooldownMap)
    {
        auto& cdData = cdItr.second;
        std::stringstream cdLine;
        std::stringstream durationStr("permanent");
        std::stringstream spellStr;
        std::stringstream catStr;
        if (cdData->IsPermanent())
            ++permCDCount;
        else
        {
            TimePoint spellExpireTime;
            TimePoint catExpireTime;
            bool foundSpellCD = cdData->GetSpellCDExpireTime(spellExpireTime);
            bool foundcatCD = cdData->GetCatCDExpireTime(catExpireTime);

            if (foundSpellCD && spellExpireTime > now)
            {
                auto cdDuration = std::chrono::duration_cast<std::chrono::milliseconds>(spellExpireTime - now);
                spellStr << "RecTime(";
                ConvertMillisecondToStr(cdDuration, spellStr);
                spellStr << ")";
            }

            if (foundcatCD && catExpireTime > now)
            {
                auto cdDuration = std::chrono::duration_cast<std::chrono::milliseconds>(catExpireTime - now);
                if (foundSpellCD)
                    catStr << ", ";
                catStr << "CatRecTime(";
                ConvertMillisecondToStr(cdDuration, catStr);
                catStr << ")";
            }

            if (!foundSpellCD && !foundcatCD)
                continue;

            durationStr << spellStr.str() << catStr.str();
            ++cdCount;
        }

        cdLine << "Spell" << "(" << cdItr.first << ") have " << durationStr.str() << " cd";
        chat.PSendSysMessage("%s", cdLine.str().c_str());
    }

    // print spell lockout
    static std::string schoolName[] = { "SPELL_SCHOOL_NORMAL", "SPELL_SCHOOL_HOLY", "SPELL_SCHOOL_FIRE", "SPELL_SCHOOL_NATURE", "SPELL_SCHOOL_FROST", "SPELL_SCHOOL_SHADOW", "SPELL_SCHOOL_ARCANE" };

    for (auto& lockoutItr : m_lockoutMap)
    {
        std::stringstream cdLine;
        std::stringstream durationStr;
        auto& cdData = lockoutItr.second;
        if (cdData > now)
        {
            auto cdDuration = std::chrono::duration_cast<std::chrono::milliseconds>(cdData - now);
            ConvertMillisecondToStr(cdDuration, durationStr);
            ++cdCount;
        }
        else
            continue;
        cdLine << "LOCKOUT for " << schoolName[lockoutItr.first] << " with " << durationStr.str() << " remaining time cd";
        chat.PSendSysMessage("%s", cdLine.str().c_str());
    }

    chat.PSendSysMessage("Found %u cooldown%s.", cdCount, (cdCount > 1) ? "s" : "");
    chat.PSendSysMessage("Found %u permanent cooldown%s.", permCDCount, (permCDCount > 1) ? "s" : "");
}

int32 WorldObject::CalculateSpellEffectValue(Unit const* target, SpellEntry const* spellProto, SpellEffectIndex effect_index, int32 const* effBasePoints, bool maximum, bool finalUse) const
{
    Unit const* unitCaster = dynamic_cast<Unit const*>(this);
    Player const* unitPlayer = (GetTypeId() == TYPEID_PLAYER) ? static_cast<Player const*>(this) : nullptr;

    float basePointsPerLevel = spellProto->EffectRealPointsPerLevel[effect_index];
    float basePoints = effBasePoints ? *effBasePoints - 1 : spellProto->EffectBasePoints[effect_index];

    if (unitCaster && basePointsPerLevel != 0.0f)
    {
        int32 level = int32(unitCaster->GetSpellRank(spellProto) / 5);
        if (level > int32(spellProto->maxLevel) && spellProto->maxLevel > 0)
            level = int32(spellProto->maxLevel);
        else if (level < int32(spellProto->baseLevel))
            level = int32(spellProto->baseLevel);

        // if base level is greater than spell level, reduce by base level
        level -= int32(std::max(spellProto->baseLevel, spellProto->spellLevel));
        basePoints += float(level) * basePointsPerLevel;
    }

    int32 randomPoints = int32(spellProto->EffectDieSides[effect_index]);
    switch (randomPoints)
    {
        case 0:                                             // not used
        case 1:
            basePoints += 1;                                // range 1..1
            break;
        default:
        {
            if (maximum)
                basePoints += randomPoints;
            else
            {
                // range can have positive (1..rand) and negative (rand..1) values, so order its for irand
                int32 randvalue = (randomPoints >= 1)
                    ? irand(1, randomPoints)
                    : irand(randomPoints, 1);

                basePoints += randvalue;
            }
            break;
        }
    }

    float value = basePoints;
    float comboDamage = spellProto->EffectPointsPerComboPoint[effect_index];

    // random damage
    if (comboDamage != 0.0f && unitPlayer && target &&
        (target->GetObjectGuid() == unitPlayer->GetComboTargetGuid() || IsOnlySelfTargeting(spellProto)))
    {
        value += (comboDamage * float(unitPlayer->GetComboPoints()));
    }

    if (unitCaster)
    {
        if (Player * modOwner = unitCaster->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_ALL_EFFECTS, value, finalUse);

            switch (effect_index)
            {
                case EFFECT_INDEX_0:
                    modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT1, value, finalUse);
                    break;
                case EFFECT_INDEX_1:
                    modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT2, value, finalUse);
                    break;
                case EFFECT_INDEX_2:
                    modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT3, value, finalUse);
                    break;
            }
        }
    }

    if (unitCaster && unitCaster->IsCreature() && spellProto->HasAttribute(SPELL_ATTR_SCALES_WITH_CREATURE_LEVEL) && spellProto->spellLevel)
    {
        // TODO: Drastically beter than before, but still needs some additional aura scaling research
        bool damage = false;
        if (uint32 aura = spellProto->EffectApplyAuraName[effect_index])
        {
            switch (aura)
            {
                case SPELL_AURA_PERIODIC_DAMAGE:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_SCHOOL_ABSORB:
                    //   SPELL_AURA_PERIODIC_DAMAGE_PERCENT: excluded, abs values only
                case SPELL_AURA_POWER_BURN_MANA:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                case SPELL_AURA_PERIODIC_MANA_LEECH:
                    damage = true;
            }
        }
        else if (uint32 effect = spellProto->Effect[effect_index])
        {
            switch (effect)
            {
                case SPELL_EFFECT_SCHOOL_DAMAGE:
                case SPELL_EFFECT_POWER_DRAIN:
                case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
                case SPELL_EFFECT_HEALTH_LEECH:
                case SPELL_EFFECT_HEAL:
                case SPELL_EFFECT_SUMMON:
                case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                    //   SPELL_EFFECT_WEAPON_PERCENT_DAMAGE: excluded, abs values only
                case SPELL_EFFECT_WEAPON_DAMAGE:
                case SPELL_EFFECT_POWER_BURN:
                case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                case SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE:
                case SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE:
                    damage = true;
            }
        }

        if (damage)
        {
            CreatureInfo const* cInfo = static_cast<Creature const*>(unitCaster)->GetCreatureInfo();
            CreatureClassLvlStats const* casterCLS = sObjectMgr.GetCreatureClassLvlStats(unitCaster->GetLevel(), cInfo->UnitClass, cInfo->Expansion);
            CreatureClassLvlStats const* spellCLS = sObjectMgr.GetCreatureClassLvlStats(spellProto->spellLevel, cInfo->UnitClass, cInfo->Expansion);
            if (casterCLS && spellCLS)
            {
                float CLSPowerCreature = casterCLS->BaseDamage;
                float CLSPowerSpell = spellCLS->BaseDamage;
                value = value * (CLSPowerCreature / CLSPowerSpell);
            }
        }
    }

    return value;
}

uint32 WorldObject::ShouldPerformObjectUpdate(uint32 const diff)
{
    // For objects that don't have next update time return diff immediately
    if (!m_nextUpdateTime)
        return diff;

    m_accumulatedUpdateDiff += diff;

    // Once accumulated time reaches and goes over update time lets use it
    if (m_accumulatedUpdateDiff >= GetNextUpdateTime())
        return m_accumulatedUpdateDiff;

    return 0;
}

float Position::GetAngle(const float x, const float y) const
{
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();

    float ang = atan2(dy, dx);                              // returns value between -Pi..Pi
    ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
    return ang;
}

float Position::GetDistance(Position const& other) const
{
    float dx = GetPositionX() - other.GetPositionX();
    float dy = GetPositionY() - other.GetPositionY();
    float distsq = dx * dx + dy * dy;

    float dz = GetPositionZ() - other.GetPositionZ();
    distsq += dz * dz;

    return distsq;
}

float Position::GetDistance2d(Position const& other) const
{
    float dx = GetPositionX() - other.GetPositionX();
    float dy = GetPositionY() - other.GetPositionY();
    float distsq = dx * dx + dy * dy;

    return distsq;
}

void Position::RelocateOffset(Position const& offset)
{
    x = GetPositionX() + (offset.GetPositionX() * std::cos(GetPositionO()) + offset.GetPositionY() * std::sin(GetPositionO() + float(M_PI)));
    y = GetPositionY() + (offset.GetPositionY() * std::cos(GetPositionO()) + offset.GetPositionX() * std::sin(GetPositionO()));
    z = GetPositionZ() + offset.GetPositionZ();
    o = GetPositionO() + offset.GetPositionO();
}

std::string Position::to_string() const
{
    return "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z) + " O: " + std::to_string(o);
}

bool operator!=(const Position& left, const Position& right)
{
    return left.x != right.x || left.y != right.y || left.z != right.z || left.o != right.o;
}

bool WorldObject::IsUsingNewSpawningSystem() const
{
    return GetDbGuid() && GetDbGuid() != GetGUIDLow();
}

void WorldObject::AddClientIAmAt(Player const* player)
{
    m_clientGUIDsIAmAt.insert(player->GetObjectGuid());
}

void WorldObject::RemoveClientIAmAt(Player const* player)
{
    m_clientGUIDsIAmAt.erase(player->GetObjectGuid());
}

void WorldObject::DestroyOnClientsIAmAt()
{
    if (IsInWorld())
    {
        for (ObjectGuid guid : m_clientGUIDsIAmAt)
            if (Player* player = GetMap()->GetPlayer(guid))
                player->DestroyAtClient(this, true);
        m_clientGUIDsIAmAt.clear();
    }
}

bool WorldObject::CheckAndIncreaseCastCounter()
{
    uint32 maxCasts = sWorld.getConfig(CONFIG_UINT32_MAX_SPELL_CASTS_IN_CHAIN);

    if (maxCasts && m_castCounter >= maxCasts)
        return false;

    ++m_castCounter;
    return true;
}
