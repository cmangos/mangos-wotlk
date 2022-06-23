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

#include "Entities/GameObject.h"
#include "Quests/QuestDef.h"
#include "Globals/ObjectMgr.h"
#include "Pools/PoolManager.h"
#include "Spells/SpellMgr.h"
#include "Spells/Spell.h"
#include "Server/Opcodes.h"
#include "WorldPacket.h"
#include "World/World.h"
#include "Database/DatabaseEnv.h"
#include "Loot/LootMgr.h"
#include "Grids/GridNotifiers.h"
#include "Grids/GridNotifiersImpl.h"
#include "Grids/CellImpl.h"
#include "Maps/InstanceData.h"
#include "Maps/MapManager.h"
#include "Maps/MapPersistentStateMgr.h"
#include "BattleGround/BattleGround.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "Util.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Vmap/GameObjectModel.h"
#include "Server/SQLStorages.h"
#include "World/WorldState.h"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Quat.h>
#include "Entities/Transports.h"

bool QuaternionData::isUnit() const
{
    return fabs(x * x + y * y + z * z + w * w - 1.0f) < 1e-5f;
}

void QuaternionData::toEulerAnglesZYX(float& Z, float& Y, float& X) const
{
    G3D::Matrix3(G3D::Quat(x, y, z, w)).toEulerAnglesZYX(Z, Y, X);
}

QuaternionData QuaternionData::fromEulerAnglesZYX(float Z, float Y, float X)
{
    G3D::Quat quat(G3D::Matrix3::fromEulerAnglesZYX(Z, Y, X));
    return QuaternionData(quat.x, quat.y, quat.z, quat.w);
}

#include <G3D/Quat.h>

GameObject::GameObject() : WorldObject(),
    m_model(nullptr),
    m_captureSlider(0),
    m_captureState(),
    m_goInfo(nullptr),
    m_displayInfo(nullptr),
    m_AI(nullptr),
    m_dbGuid(0)
{
    m_objectType |= TYPEMASK_GAMEOBJECT;
    m_objectTypeId = TYPEID_GAMEOBJECT;

    m_updateFlag = (UPDATEFLAG_HIGHGUID | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_POSITION | UPDATEFLAG_ROTATION);

    m_valuesCount = GAMEOBJECT_END;

    m_respawnTime = 0;
    m_respawnDelay = 25;
    m_respawnOverriden = false;
    m_respawnOverrideOnce = false;
    m_forcedDespawn = false;

    m_lootState = GO_READY;
    m_spawnedByDefault = true;
    m_useTimes = 0;
    m_spellId = 0;
    m_cooldownTime = 0;

    m_captureTimer = 0;

    m_packedRotation = 0;
    m_lootGroupRecipientId = 0;

    m_isInUse = false;
    m_reStockTimer = 0;
    m_rearmTimer = 0;
    m_despawnTimer = 0;

    m_delayedActionTimer = 0;

    m_goGroup = nullptr;
}

GameObject::~GameObject()
{
    delete m_model;
}

GameObject* GameObject::CreateGameObject(uint32 entry)
{
    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(entry);
    if (goinfo && goinfo->type == GAMEOBJECT_TYPE_TRANSPORT)
        return new ElevatorTransport;
    return new GameObject;
}

void GameObject::AddToWorld()
{
    ///- Register the gameobject for guid lookup
    if (!IsInWorld())
    {
        GetMap()->GetObjectsStore().insert<GameObject>(GetObjectGuid(), (GameObject*)this);
        if (GetDbGuid())
            GetMap()->AddDbGuidObject(this);
    }

    if (m_model)
        GetMap()->InsertGameObjectModel(*m_model);

    WorldObject::AddToWorld();

    // After Object::AddToWorld so that for initial state the GO is added to the world (and hence handled correctly)
    UpdateCollisionState();

    if (IsSpawned()) // need to prevent linked trap addition due to Pool system Map::Add abuse
    {
        if (GameObject* linkedGO = SummonLinkedTrapIfAny())
            SetLinkedTrap(linkedGO);
    }

    // Make active if required
    if (GetGOInfo()->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_ACTIVE)
        SetActiveObjectState(true);
}

void GameObject::RemoveFromWorld()
{
    ///- Remove the gameobject from the accessor
    if (IsInWorld())
    {
        // Notify the outdoor pvp script
        if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(GetZoneId()))
            outdoorPvP->HandleGameObjectRemove(this);

        // Remove GO from owner
        if (ObjectGuid owner_guid = GetOwnerGuid())
        {
            if (Unit* owner = ObjectAccessor::GetUnit(*this, owner_guid))
                owner->RemoveGameObject(this, false);
            else
            {
                sLog.outError("Delete %s with SpellId %u LinkedGO %u that lost references to owner %s GO list. Crash possible later.",
                              GetGuidStr().c_str(), m_spellId, GetGOInfo()->GetLinkedGameObjectEntry(), owner_guid.GetString().c_str());
            }
        }

        if (m_model && GetMap()->ContainsGameObjectModel(*m_model))
            GetMap()->RemoveGameObjectModel(*m_model);

        GetMap()->GetObjectsStore().erase<GameObject>(GetObjectGuid(), (GameObject*)nullptr);
        if (GetDbGuid())
            GetMap()->RemoveDbGuidObject(this);

        ClearGameObjectGroup();
    }

    WorldObject::RemoveFromWorld();
}

bool GameObject::Create(uint32 guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang, const QuaternionData& rotation, uint8 animprogress, GOState go_state)
{
    MANGOS_ASSERT(map);
    Relocate(x, y, z, ang);
    SetMap(map);
    SetPhaseMask(phaseMask, false);

    m_stationaryPosition = Position(x, y, z, ang);

    if (!IsPositionValid())
    {
        sLog.outError("Gameobject (GUID: %u Entry: %u ) not created. Suggested coordinates are invalid (X: %f Y: %f)", guidlow, name_id, x, y);
        return false;
    }

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(name_id);
    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (GUID: %u) not created: Entry %u does not exist in `gameobject_template`. Map: %u  (X: %f Y: %f Z: %f) ang: %f", guidlow, name_id, map->GetId(), x, y, z, ang);
        return false;
    }

    Object::_Create(guidlow, goinfo->id, HIGHGUID_GAMEOBJECT);

    m_goInfo = goinfo;

    if (goinfo->type >= MAX_GAMEOBJECT_TYPE)
    {
        sLog.outErrorDb("Gameobject (GUID: %u) not created: Entry %u has invalid type %u in `gameobject_template`. It may crash client if created.", guidlow, name_id, goinfo->type);
        return false;
    }

    SetObjectScale(goinfo->size);

    SetLocalRotation(rotation.x, rotation.y, rotation.z, rotation.w);
    // For most of gameobjects is (0, 0, 0, 1) quaternion, only some transports has not standart rotation
    if (const GameObjectDataAddon* addon = sGameObjectDataAddonStorage.LookupEntry<GameObjectDataAddon>(guidlow))
        SetTransportPathRotation(addon->path_rotation);
    else
        SetTransportPathRotation(QuaternionData(0, 0, 0, 1));

    SetUInt32Value(GAMEOBJECT_FACTION, goinfo->faction);
    SetUInt32Value(GAMEOBJECT_FLAGS, goinfo->flags);

    if (goinfo->type == GAMEOBJECT_TYPE_TRANSPORT)
    {
        SetFlag(GAMEOBJECT_FLAGS, (GO_FLAG_TRANSPORT | GO_FLAG_NODESPAWN));
        m_updateFlag = (m_updateFlag | UPDATEFLAG_TRANSPORT) & ~UPDATEFLAG_POSITION;
    }

    SetEntry(goinfo->id);
    SetDisplayId(goinfo->displayId);

    // GAMEOBJECT_BYTES_1, index at 0, 1, 2 and 3
    SetGoState(go_state);
    SetGoType(GameobjectTypes(goinfo->type));
    SetGoArtKit(0);                                         // unknown what this is
    SetGoAnimProgress(animprogress);

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_TRAP:
            // values from rogue detect traps aura
            if (goinfo->trap.stealthed)
            {
                GetVisibilityData().SetStealthMask(STEALTH_TRAP, true);
                GetVisibilityData().AddStealthStrength(STEALTH_TRAP, 70);
            }
            if (goinfo->trap.invisible)
            {
                GetVisibilityData().SetInvisibilityMask(INVISIBILITY_TRAP, true);
                GetVisibilityData().AddInvisibilityValue(INVISIBILITY_TRAP, 300);
            }
            // [[fallthrough]]
        case GAMEOBJECT_TYPE_FISHINGNODE:
            m_lootState = GO_NOT_READY;                     // Initialize Traps and Fishingnode delayed in ::Update
            break;
        case GAMEOBJECT_TYPE_TRANSPORT:
            SetUInt32Value(GAMEOBJECT_LEVEL, goinfo->transport.pause);
            SetGoState(goinfo->transport.startOpen ? GO_STATE_ACTIVE : GO_STATE_READY);
            SetGoAnimProgress(animprogress);
            break;
        case GAMEOBJECT_TYPE_GENERIC:
        case GAMEOBJECT_TYPE_SPELL_FOCUS:
        case GAMEOBJECT_TYPE_GOOBER:
        case GAMEOBJECT_TYPE_CHEST:
            SetUInt32Value(GAMEOBJECT_DYNAMIC, GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
            break;
        case GAMEOBJECT_TYPE_QUESTGIVER:
            SetUInt32Value(GAMEOBJECT_DYNAMIC, GO_DYNFLAG_LO_ACTIVATE);
            break;
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
            ForceGameObjectHealth(GetMaxHealth(), nullptr);
            break;
        default:
            break;
    }

    // Notify the battleground or outdoor pvp script
    if (map->IsBattleGroundOrArena())
        ((BattleGroundMap*)map)->GetBG()->HandleGameObjectCreate(this);
    else if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(GetZoneId()))
        outdoorPvP->HandleGameObjectCreate(this);

    // Notify the map's instance data.
    // Only works if you create the object in it, not if it is moves to that map.
    // Normally non-players do not teleport to other maps.
    if (InstanceData* iData = map->GetInstanceData())
        iData->OnObjectCreate(this);

    // Check if GameObject is Large
    if (GetGOInfo()->IsLargeGameObject())
        GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Large);

    return true;
}

void GameObject::Update(const uint32 diff)
{
    if (GetObjectGuid().IsMOTransport())
    {
        //((Transport*)this)->Update(p_time);
        return;
    }

    m_events.Update(diff);

    switch (m_lootState)
    {
        case GO_NOT_READY:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_TRAP:                  // Initialized delayed to be able to use GetOwner()
                {
                    // Arming Time for GAMEOBJECT_TYPE_TRAP (6)
                    // Note: wotlk+ specific types of traps have a default charge time
                    if (GetGOInfo()->trap.charges == 2 && GetGOInfo()->trap.diameter == 0)
                        m_cooldownTime = time(nullptr) + 10;
                    else
                    {
                        Unit* owner = GetOwner();
                        if (owner && owner->IsInCombat())
                            m_cooldownTime = time(nullptr) + GetGOInfo()->trap.startDelay;
                    }
                    m_lootState = GO_READY;
                    break;
                }
                case GAMEOBJECT_TYPE_FISHINGNODE:           // Keep not ready for some delay
                {
                    // fishing code (bobber ready)
                    if (time(nullptr) > m_respawnTime - FISHING_BOBBER_READY_TIME)
                    {
                        // splash bobber (bobber ready now)
                        Unit* caster = GetOwner();
                        if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            SetGoState(GO_STATE_ACTIVE);
                            // SetUInt32Value(GAMEOBJECT_FLAGS, GO_FLAG_NODESPAWN);

                            SendForcedObjectUpdate();

                            SendGameObjectCustomAnim(GetObjectGuid());
                        }

                        m_lootState = GO_READY;             // can be successfully open with some chance
                    }
                    break;
                }
                case GAMEOBJECT_TYPE_CHEST:
                {
                    if (m_goInfo->chest.chestRestockTime)
                    {
                        if (m_reStockTimer != 0)
                        {
                            if (m_reStockTimer <= time(nullptr))
                            {
                                m_reStockTimer = 0;
                                m_lootState = GO_READY;
                                delete m_loot;
                                m_loot = nullptr;
                                ForceValuesUpdateAtIndex(GAMEOBJECT_DYNAMIC);
                            }
                        }
                        else
                            m_lootState = GO_READY;

                        return;
                    }
                    m_lootState = GO_READY;
                }
                default:
                    break;
            }
            break;
        }
        case GO_READY:
        {
            if (m_respawnTime > 0)                          // timer on
            {
                if (m_respawnTime <= time(nullptr))            // timer expired
                {
                    m_respawnTime = 0;
                    ClearAllUsesData();

                    switch (GetGoType())
                    {
                        case GAMEOBJECT_TYPE_FISHINGNODE:   // can't fish now
                        {
                            Unit* caster = GetOwner();
                            if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                            {
                                caster->FinishSpell(CURRENT_CHANNELED_SPELL);

                                WorldPacket data(SMSG_FISH_NOT_HOOKED, 0);
                                ((Player*)caster)->GetSession()->SendPacket(data);
                            }
                            // can be deleted
                            m_lootState = GO_JUST_DEACTIVATED;
                            return;
                        }
                        case GAMEOBJECT_TYPE_DOOR:
                        case GAMEOBJECT_TYPE_BUTTON:
                            // we need to open doors if they are closed (add there another condition if this code breaks some usage, but it need to be here for battlegrounds)
                            if (GetGoState() != GO_STATE_READY)
                                ResetDoorOrButton();
                        // flags in AB are type_button and we need to add them here so no break!
                        default:
                            if (!m_spawnedByDefault)        // despawn timer
                            {
                                // can be despawned or destroyed
                                SetLootState(GO_JUST_DEACTIVATED);
                                // Remove Wild-Summoned GO on timer expire
                                if (!HasStaticDBSpawnData())
                                {
                                    if (Unit* owner = GetOwner())
                                        owner->RemoveGameObject(this, false);
                                    Delete();
                                }
                                return;
                            }

                            // respawn timer
                            GetMap()->Add(this);
                            AIM_Initialize();
                            break;
                    }
                }
            }

            if (IsSpawned())
            {
                // traps can have time and can not have
                GameObjectInfo const* goInfo = GetGOInfo();
                if (goInfo->type == GAMEOBJECT_TYPE_TRAP && GetGoState() == GO_STATE_READY)   // traps
                {
                    if (m_cooldownTime < time(nullptr))
                    {
                        // FIXME: this is activation radius (in different casting radius that must be selected from spell data)
                        // TODO: move activated state code (cast itself) to GO_ACTIVATED, in this place only check activating and set state
                        float radius = float(goInfo->trap.diameter) / 2.0f;

                        // behavior verified on classic
                        // TODO: needs more research
                        if (goInfo->GetLockId() == 12) // 21 objects currently (hunter traps), all with 5 or less for diameter -> use diameter as radius instead
                            radius = float(goInfo->trap.diameter);

                        bool valid = true;
                        if (!radius)
                        {
                            if (goInfo->trap.cooldown != 3)     // cast in other case (at some triggering/linked go/etc explicit call)
                                valid = false;
                            else
                            {
                                if (m_respawnTime > 0)
                                    valid = false;
                                else // battlegrounds gameobjects has data2 == 0 && data5 == 3
                                    radius = float(goInfo->trap.cooldown);
                            }
                        }

                        if (valid)
                        {
                            // Should trap trigger?
                            Unit* target = nullptr;                     // pointer to appropriate target if found any

                            if (std::function<bool(Unit*)>* functor = sScriptDevAIMgr.OnTrapSearch(this))
                            {
                                MaNGOS::AnyUnitFulfillingConditionInRangeCheck u_check(this, *functor, radius);
                                MaNGOS::UnitSearcher<MaNGOS::AnyUnitFulfillingConditionInRangeCheck> checker(target, u_check);
                                Cell::VisitAllObjects(this, checker, radius);
                            }
                            else
                            {
                                switch (goInfo->trapCustom.triggerOn)
                                {
                                    case 1: // friendly
                                    {
                                        MaNGOS::AnyFriendlyUnitInObjectRangeCheck u_check(this, nullptr, radius);
                                        MaNGOS::UnitSearcher<MaNGOS::AnyFriendlyUnitInObjectRangeCheck> checker(target, u_check);
                                        Cell::VisitAllObjects(this, checker, radius);
                                        break;
                                    }
                                    case 2: // all
                                    {
                                        MaNGOS::AnyUnitInObjectRangeCheck u_check(this, radius);
                                        MaNGOS::UnitSearcher<MaNGOS::AnyUnitInObjectRangeCheck> checker(target, u_check);
                                        Cell::VisitAllObjects(this, checker, radius);
                                        break;
                                    }
                                    default: // unfriendly
                                    {
                                        MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(this, radius);
                                        MaNGOS::UnitSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> checker(target, u_check);
                                        Cell::VisitAllObjects(this, checker, radius);
                                        break;
                                    }
                                }
                            }

                            if (target && (!goInfo->trapCustom.triggerOn || !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))) // do not trigger on hostile traps if not selectable
                                Use(target);
                        }
                        else
                        {
                            // Note: wotlk+ traps which work as bombs cast the spell automatically
                            if (GetGOInfo()->trap.charges == 2 && GetGOInfo()->trap.diameter == 0)
                            {
                                if (Unit* owner = GetOwner())
                                    Use(owner);

                                SetLootState(GO_JUST_DEACTIVATED);
                                break;
                            }
                        }
                    }
                }

                int32 max_charges = goInfo->GetCharges();   // Only check usable (positive) charges; 0 : no charge; -1 : infinite charges
                if (max_charges > 0 && m_useTimes >= uint32(max_charges))
                {
                    m_useTimes = 0;
                    SetLootState(GO_JUST_DEACTIVATED);  // can be despawned or destroyed
                }
            }
            break;
        }
        case GO_ACTIVATED:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_DOOR:
                case GAMEOBJECT_TYPE_BUTTON:
                    if (GetGOInfo()->GetAutoCloseTime() && (m_cooldownTime < time(nullptr)))
                        ResetDoorOrButton();
                    break;
                case GAMEOBJECT_TYPE_CHEST:
                    if (m_loot)
                    {
                        if (m_loot->IsChanged())
                            m_despawnTimer = time(nullptr) + 5 * MINUTE; // TODO:: need to add a define?
                        else if (m_despawnTimer != 0 && m_despawnTimer <= time(nullptr))
                            m_lootState = GO_JUST_DEACTIVATED;

                        m_loot->Update();
                    }
                    break;
                case GAMEOBJECT_TYPE_TRAP:

                    // Note: wotlk+ traps which work as bombs can be disarmed
                    if (GetGOInfo()->trap.charges == 2 && GetGOInfo()->trap.diameter == 0)
                    {
                        SetLootState(GO_JUST_DEACTIVATED);
                        break;
                    }

                    if (m_rearmTimer == 0)
                    {
                        m_rearmTimer = time(nullptr) + GetRespawnDelay();
                        SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    }

                    if (m_rearmTimer < time(nullptr))
                    {
                        SetGoState(GO_STATE_READY);
                        m_lootState = GO_READY;
                        m_rearmTimer = 0;
                    }
                    break;
                case GAMEOBJECT_TYPE_GOOBER:
                    if (m_cooldownTime < time(nullptr))
                    {
                        RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

                        SetLootState(GO_JUST_DEACTIVATED);
                        m_cooldownTime = 0;
                    }
                    break;
                case GAMEOBJECT_TYPE_CAPTURE_POINT:
                    m_captureTimer += diff;
                    if (m_captureTimer >= 5000)
                    {
                        TickCapturePoint();
                        m_captureTimer -= 5000;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        case GO_JUST_DEACTIVATED:
        {
            sWorldState.HandleGameObjectRevertState(this);

            // If nearby linked trap exists, despawn it
            if (GameObject* linkedTrap = GetLinkedTrap())
            {
                linkedTrap->SetLootState(GO_JUST_DEACTIVATED);
                linkedTrap->Delete();
            }

            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_GOOBER:
                    // if gameobject should cast spell, then this, but some GOs (type = 10) should be destroyed
                    if (uint32 spellId = GetGOInfo()->goober.spellId)
                    {
                        for (auto m_UniqueUser : m_UniqueUsers)
                        {
                            if (Player* owner = GetMap()->GetPlayer(m_UniqueUser))
                                owner->CastSpell(owner, spellId, TRIGGERED_NONE, nullptr, nullptr, GetObjectGuid());
                        }

                        ClearAllUsesData();
                    }

                    SetGoState(GO_STATE_READY);
                    // research - 185861 needs to be able to despawn as well TODO: fixup

                    // any return here in case battleground traps
                    break;
                case GAMEOBJECT_TYPE_CAPTURE_POINT:
                    // there are some capture points which are used as visual GOs; we allow these to be despawned
                    if (!GetGOInfo()->capturePoint.radius)
                        break;

                    // remove capturing players because slider wont be displayed if capture point is being locked
                    for (auto m_UniqueUser : m_UniqueUsers)
                    {
                        if (Player* owner = GetMap()->GetPlayer(m_UniqueUser))
                            owner->SendUpdateWorldState(GetGOInfo()->capturePoint.worldState1, WORLD_STATE_REMOVE);
                    }

                    m_UniqueUsers.clear();
                    SetLootState(GO_READY);
                    return; // SetLootState and return because go is treated as "burning flag" due to GetGoAnimProgress() being 100 and would be removed on the client
                case GAMEOBJECT_TYPE_CHEST:
                    m_despawnTimer = 0;
                    // consumable confirmed to override chest restock
                    if (!m_goInfo->chest.consumable && m_goInfo->chest.chestRestockTime)
                    {
                        m_reStockTimer = time(nullptr) + m_goInfo->chest.chestRestockTime;
                        SetLootState(GO_NOT_READY);
                        ForceValuesUpdateAtIndex(GAMEOBJECT_DYNAMIC);
                        return;
                    }
                    break;
                default:
                    break;
            }

            // Remove wild summoned after use
            if (!HasStaticDBSpawnData() && (!GetSpellId() || GetGOInfo()->GetDespawnPossibility() || GetGOInfo()->IsDespawnAtAction() || m_forcedDespawn))
            {
                if (Unit* owner = GetOwner())
                    owner->RemoveGameObject(this, false);
                Delete();
                return;
            }

            // burning flags in some battlegrounds, if you find better condition, just add it
            if (GetGOInfo()->IsDespawnAtAction() || GetGoAnimProgress() > 0)
            {
                SendObjectDeSpawnAnim(GetObjectGuid());
                // reset flags
                if (GetMap()->Instanceable())
                {
                    // In Instances GO_FLAG_LOCKED, GO_FLAG_INTERACT_COND or GO_FLAG_NO_INTERACT are not changed
                    uint32 currentLockOrInteractFlags = GetUInt32Value(GAMEOBJECT_FLAGS) & (GO_FLAG_LOCKED | GO_FLAG_INTERACT_COND | GO_FLAG_NO_INTERACT);
                    SetUInt32Value(GAMEOBJECT_FLAGS, (GetGOInfo()->flags & ~(GO_FLAG_LOCKED | GO_FLAG_INTERACT_COND | GO_FLAG_NO_INTERACT)) | currentLockOrInteractFlags);
                }
                else
                    SetUInt32Value(GAMEOBJECT_FLAGS, GetGOInfo()->flags);
            }

            delete m_loot;
            m_loot = nullptr;
            SetLootRecipient(nullptr);
            SetLootState(GO_READY);

            // non-consumable chests and goobers should never despawn
            if ((GetGoType() == GAMEOBJECT_TYPE_CHEST || GetGoType() == GAMEOBJECT_TYPE_GOOBER) && !GetGOInfo()->IsDespawnAtAction() && !m_forcedDespawn)
                return;

            if (!m_respawnDelay)
                return;

            m_forcedDespawn = false;

            if (AI())
                AI()->JustDespawned();

            if (!m_respawnOverriden)
            {
                // since pool system can fail to roll unspawned object, this one can remain spawned, so must set respawn nevertheless
                if (IsSpawnedByDefault())
                    if (GameObjectData const* data = sObjectMgr.GetGOData(GetObjectGuid().GetCounter()))
                        m_respawnDelay = data->GetRandomRespawnTime();
            }
            else if (m_respawnOverrideOnce)
                m_respawnOverriden = false;

            m_respawnTime = m_spawnedByDefault ? time(nullptr) + m_respawnDelay : 0;

            // if option not set then object will be saved at grid unload
            if (sWorld.getConfig(CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY))
                SaveRespawnTime();

            if (IsUsingNewSpawningSystem()) // does not support pooling
            {
                m_respawnTime = std::numeric_limits<time_t>::max();
                if (m_respawnDelay && !GetGameObjectGroup())
                    GetMap()->GetSpawnManager().AddGameObject(m_respawnDelay, GetDbGuid());
                AddObjectToRemoveList();
            }
            else
            {
                // if part of pool, let pool system schedule new spawn instead of just scheduling respawn
                if (uint16 poolid = sPoolMgr.IsPartOfAPool<GameObject>(m_dbGuid))
                    sPoolMgr.UpdatePool<GameObject>(*GetMap()->GetPersistentState(), poolid, m_dbGuid);
            }

            // can be not in world at pool despawn
            if (IsInWorld())
                UpdateObjectVisibility();

            break;
        }
    }

    if (m_delayedActionTimer)
    {
        if (m_delayedActionTimer <= diff)
        {
            m_delayedActionTimer = 0;
            TriggerDelayedAction();
        }
        else
            m_delayedActionTimer -= diff;
    }

    if (m_AI)
        m_AI->UpdateAI(diff);

    WorldObject::Update(diff);
}

void GameObject::Heartbeat()
{
    if (AI())
        AI()->OnHeartbeat();
}

void GameObject::Refresh()
{
    // not refresh despawned not casted GO (despawned casted GO destroyed in all cases anyway)
    if (m_respawnTime > 0 && m_spawnedByDefault)
        return;

    if (IsSpawned())
    {
        GetMap()->Add(this);
        AIM_Initialize();
    }
}

void GameObject::AddUniqueUse(Player* player)
{
    AddUse();

    if (!m_firstUser)
        m_firstUser = player->GetObjectGuid();

    m_UniqueUsers.insert(player->GetObjectGuid());
}

void GameObject::Delete()
{
    SendObjectDeSpawnAnim(GetObjectGuid());

    SetGoState(GO_STATE_READY);
    SetUInt32Value(GAMEOBJECT_FLAGS, GetGOInfo()->flags);

    if (AI())
        AI()->JustDespawned();

    if (uint16 poolid = sPoolMgr.IsPartOfAPool<GameObject>(m_dbGuid))
        sPoolMgr.UpdatePool<GameObject>(*GetMap()->GetPersistentState(), poolid, m_dbGuid);
    else
        AddObjectToRemoveList();

    if (GameObject* linkedTrap = GetLinkedTrap())
    {
        linkedTrap->SetLootState(GO_JUST_DEACTIVATED);
        linkedTrap->Delete();
    }
}

void GameObject::SaveToDB() const
{
    // this should only be used when the gameobject has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    GameObjectData const* data = sObjectMgr.GetGOData(m_dbGuid);
    if (!data)
    {
        sLog.outError("GameObject::SaveToDB failed, cannot get gameobject data!");
        return;
    }

    SaveToDB(GetMapId(), data->spawnMask, data->phaseMask);
}

void GameObject::SaveToDB(uint32 mapid, uint8 spawnMask, uint32 phaseMask) const
{
    const GameObjectInfo* goI = GetGOInfo();

    if (!goI)
        return;

    // update in loaded data (changing data only in this place)
    GameObjectData& data = sObjectMgr.NewGOData(GetGUIDLow());

    // data->guid = guid don't must be update at save
    data.id = GetEntry();
    data.mapid = mapid;
    data.phaseMask = phaseMask;
    data.posX = GetPositionX();
    data.posY = GetPositionY();
    data.posZ = GetPositionZ();
    data.orientation = GetOrientation();
    data.rotation.x = m_localRotation.x;
    data.rotation.y = m_localRotation.y;
    data.rotation.z = m_localRotation.z;
    data.rotation.w = m_localRotation.w;
    data.spawntimesecsmin = m_spawnedByDefault ? (int32)m_respawnDelay : -(int32)m_respawnDelay;
    data.spawntimesecsmax = m_spawnedByDefault ? (int32)m_respawnDelay : -(int32)m_respawnDelay;
    data.animprogress = GetGoAnimProgress();
    data.go_state = GetGoState();
    data.spawnMask = spawnMask;

    // updated in DB
    std::ostringstream ss;
    ss << "INSERT INTO gameobject VALUES ( "
       << GetGUIDLow() << ", "
       << GetEntry() << ", "
       << mapid << ", "
       << uint32(spawnMask) << ","                         // cast to prevent save as symbol
       << uint16(GetPhaseMask()) << ","                    // prevent out of range error
       << GetPositionX() << ", "
       << GetPositionY() << ", "
       << GetPositionZ() << ", "
       << GetOrientation() << ", "
       << m_localRotation.x << ", "
       << m_localRotation.y << ", "
       << m_localRotation.z << ", "
       << m_localRotation.w << ", "
       << m_respawnDelay << ", "
       << m_respawnDelay << ", " // TODO: Add variance
       << uint32(GetGoAnimProgress()) << ", "
       << uint32(GetGoState()) << ")";

    WorldDatabase.BeginTransaction();
    WorldDatabase.PExecuteLog("DELETE FROM gameobject WHERE guid = '%u'", GetGUIDLow());
    WorldDatabase.PExecuteLog("%s", ss.str().c_str());
    WorldDatabase.CommitTransaction();
}

bool GameObject::LoadFromDB(uint32 dbGuid, Map* map, uint32 newGuid, uint32 forcedEntry, GenericTransport* transport)
{
    GameObjectData const* data = sObjectMgr.GetGOData(dbGuid);

    if (!data)
    {
        sLog.outErrorDb("Gameobject (GUID: %u) not found in table `gameobject`, can't load. ", dbGuid);
        return false;
    }

    uint32 entry = forcedEntry ? forcedEntry : data->id;
    // uint32 map_id = data->mapid;                         // already used before call
    uint32 phaseMask = data->phaseMask;
    float x = data->posX;
    float y = data->posY;
    float z = data->posZ;
    float ang = data->orientation;

    if (transport)
        transport->CalculatePassengerPosition(x, y, z, &ang);

    uint8 animprogress = data->animprogress;
    GOState go_state = data->go_state;

    SpawnGroupEntry* groupEntry = map->GetMapDataContainer().GetSpawnGroupByGuid(dbGuid, TYPEID_GAMEOBJECT); // use dynguid by default \o/
    GameObjectGroup* group = nullptr;
    if (groupEntry)
    {
        group = static_cast<GameObjectGroup*>(map->GetSpawnManager().GetSpawnGroup(groupEntry->Id));
        if (!entry)
            entry = group->GetGuidEntry(dbGuid);
    }

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(entry);
    if ((goinfo && (goinfo->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_DYNGUID) != 0 || groupEntry) && dbGuid == newGuid)
        newGuid = map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT);

    m_dbGuid = dbGuid;

    if (uint32 randomEntry = sObjectMgr.GetRandomGameObjectEntry(GetDbGuid()))
        entry = randomEntry;

    if (!Create(newGuid, entry, map, phaseMask, x, y, z, ang, data->rotation, animprogress, go_state))
        return false;

    if (group)
        SetGameObjectGroup(group);

    if (!GetGOInfo()->GetDespawnPossibility() && !GetGOInfo()->IsDespawnAtAction() && data->spawntimesecsmin >= 0)
    {
        SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NODESPAWN);
        m_spawnedByDefault = true;
        m_respawnDelay = 0;
        m_respawnTime = 0;
    }
    else
    {
        if (data->spawntimesecsmin >= 0)
        {
            m_spawnedByDefault = true;
            m_respawnDelay = data->GetRandomRespawnTime();

            m_respawnTime  = map->GetPersistentState()->GetGORespawnTime(m_dbGuid);

            // ready to respawn
            if (m_respawnTime && m_respawnTime <= time(nullptr))
            {
                m_respawnTime = 0;
                map->GetPersistentState()->SaveGORespawnTime(m_dbGuid, 0);
            }
        }
        else
        {
            m_spawnedByDefault = false;
            m_respawnDelay = -data->spawntimesecsmin;
            m_respawnTime = 0;
        }
    }

    map->Add(this);
    AIM_Initialize();

    if (transport)
    {
        m_movementInfo.SetTransportPos(Position(data->posX, data->posY, data->posZ, data->orientation));
        transport->AddPassenger(this);
    }

    return true;
}

struct GameObjectRespawnDeleteWorker
{
    explicit GameObjectRespawnDeleteWorker(uint32 guid) : i_guid(guid) {}

    void operator()(MapPersistentState* state)
    {
        state->SaveGORespawnTime(i_guid, 0);
    }

    uint32 i_guid;
};

void GameObject::DeleteFromDB() const
{
    if (!HasStaticDBSpawnData())
    {
        DEBUG_LOG("Trying to delete not saved gameobject!");
        return;
    }

    GameObjectRespawnDeleteWorker worker(m_dbGuid);
    sMapPersistentStateMgr.DoForAllStatesWithMapId(GetMapId(), worker);

    sObjectMgr.DeleteGOData(m_dbGuid);
    WorldDatabase.PExecuteLog("DELETE FROM gameobject WHERE guid = '%u'", m_dbGuid);
    WorldDatabase.PExecuteLog("DELETE FROM game_event_gameobject WHERE guid = '%u'", m_dbGuid);
    WorldDatabase.PExecuteLog("DELETE FROM gameobject_battleground WHERE guid = '%u'", m_dbGuid);
}

void GameObject::SetOwnerGuid(ObjectGuid guid)
{
    m_spawnedByDefault = false;                     // all object with owner is despawned after delay
    SetGuidValue(OBJECT_FIELD_CREATED_BY, guid);
}

Unit* GameObject::GetOwner() const
{
    return ObjectAccessor::GetUnit(*this, GetOwnerGuid());
}

WorldObject* GameObject::GetSpawner() const
{
    if (!IsInWorld())
        return nullptr;

    ObjectGuid spawnerGuid = GetSpawnerGuid();
    if (spawnerGuid.IsGameObject())
        return GetMap()->GetGameObject(spawnerGuid);
    else
        return GetMap()->GetUnit(spawnerGuid);
}

GameObjectInfo const* GameObject::GetGOInfo() const
{
    return m_goInfo;
}

GameObjectTemplateAddon const* GameObject::GetTemplateAddon() const
{
    return sObjectMgr.GetGOTemplateAddon(GetEntry());
}

/*********************************************************/
/***                    QUEST SYSTEM                   ***/
/*********************************************************/
bool GameObject::HasQuest(uint32 quest_id) const
{
    QuestRelationsMapBounds bounds = sObjectMgr.GetGOQuestRelationsMapBounds(GetEntry());
    for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool GameObject::HasInvolvedQuest(uint32 quest_id) const
{
    QuestRelationsMapBounds bounds = sObjectMgr.GetGOQuestInvolvedRelationsMapBounds(GetEntry());
    for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool GameObject::IsTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectInfo const* gInfo = GetGOInfo();
    if (!gInfo) return false;
    return gInfo->type == GAMEOBJECT_TYPE_TRANSPORT || gInfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT;
}

// is Dynamic transport = non-stop Transport
bool GameObject::IsDynTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectInfo const* gInfo = GetGOInfo();
    if (!gInfo) return false;
    return gInfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT || (gInfo->type == GAMEOBJECT_TYPE_TRANSPORT && !gInfo->transport.pause);
}

bool GameObject::IsMoTransport() const
{
    GameObjectInfo const* gInfo = GetGOInfo();
    if (!gInfo) return false;
    return gInfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT;
}

void GameObject::SaveRespawnTime()
{
    if (m_respawnTime > time(nullptr) && m_spawnedByDefault)
        GetMap()->GetPersistentState()->SaveGORespawnTime(m_dbGuid, m_respawnTime);
}

bool GameObject::isVisibleForInState(Player const* u, WorldObject const* viewPoint, bool /*inVisibleList*/) const
{
    // Not in world
    if (!IsInWorld() || !u->IsInWorld())
        return false;

    // invisible at client always
    if (!GetGOInfo()->displayId)
        return false;

    // Transport always visible at this step implementation
    if (IsMoTransport() && IsInMap(u))
        return true;

    // quick check visibility false cases for non-GM-mode
    if (!u->IsGameMaster())
    {
        // despawned and then not visible for non-GM in GM-mode
        if (!IsSpawned())
            return false;

        if (GetGOInfo()->IsServerOnly())
            return false;

        bool isEnemyTrap = false;

        // special invisibility cases
        switch (GetGOInfo()->type)
        {
            case GAMEOBJECT_TYPE_TRAP:
            {
                if (GetGOInfo()->trap.stealthed == 0 && GetGOInfo()->trap.invisible == 0)
                    break;

                // handle summoned traps, usually by players
                if (Unit* owner = GetOwner())
                {
                    if (owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* ownerPlayer = (Player*)owner;
                        if ((GetMap()->IsBattleGroundOrArena() && ownerPlayer->GetBGTeam() != u->GetBGTeam()) ||
                            (ownerPlayer->IsInDuelWith(u)) ||
                            (!ownerPlayer->IsInGroup(u)))
                            isEnemyTrap = true;
                    }
                    else
                    {
                        if (owner->CanCooperate(u))
                            return true;
                    }
                }
                // handle environment traps (spawned by DB)
                else
                {
                    if (this->IsFriend(u))
                        return true;
                    else
                        isEnemyTrap = true;
                }
            }
            //[[fallthrough]]
            default:
            {
                if (GetVisibilityData().GetInvisibilityMask()) // invisible gos
                {
                    if (u->GetVisibilityData().CanDetectInvisibilityOf(this))
                        isEnemyTrap = false; // need to check general distance
                    else if (GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP)
                        return false;
                }

                if (GetVisibilityData().GetStealthMask()) // stealthed gos
                {
                    float visibleDistance = GetVisibilityData().GetStealthVisibilityDistance(u);
                    // recheck new distance
                    if (GetDistance(viewPoint, true, DIST_CALC_NONE) < visibleDistance * visibleDistance && u->HasInArc(this))
                        return true;
                    else if (GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP)
                        return false;
                }

                if (isEnemyTrap)
                    return false;

                break;
            }
        }
    }

    // check distance
    return IsWithinDistInMap(viewPoint, GetVisibilityData().GetVisibilityDistance(), false);
}

void GameObject::Respawn()
{
    if (m_spawnedByDefault && m_respawnTime > 0)
    {
        m_respawnTime = time(nullptr);
        GetMap()->GetPersistentState()->SaveGORespawnTime(m_dbGuid, 0);
    }
}

bool GameObject::ActivateToQuest(Player* pTarget) const
{
    // if GO is ReqCreatureOrGoN for quest
    if (pTarget->HasQuestForGO(GetEntry()))
        return true;

    if (!sObjectMgr.IsGameObjectForQuests(GetEntry()))
        return false;

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_QUESTGIVER:
        {
            // Not fully clear when GO's can activate/deactivate
            // For cases where GO has additional (except quest itself),
            // these conditions are not sufficient/will fail.
            // Never expect flags|4 for these GO's? (NF-note: It doesn't appear it's expected)

            QuestRelationsMapBounds bounds = sObjectMgr.GetGOQuestRelationsMapBounds(GetEntry());

            for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                const Quest* qInfo = sObjectMgr.GetQuestTemplate(itr->second);

                if (pTarget->CanTakeQuest(qInfo, false))
                    return true;
            }

            bounds = sObjectMgr.GetGOQuestInvolvedRelationsMapBounds(GetEntry());

            for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                if ((pTarget->GetQuestStatus(itr->second) == QUEST_STATUS_INCOMPLETE || pTarget->GetQuestStatus(itr->second) == QUEST_STATUS_COMPLETE)
                        && !pTarget->GetQuestRewardStatus(itr->second))
                    return true;
            }

            break;
        }
        // scan GO chest with loot including quest items
        case GAMEOBJECT_TYPE_CHEST:
        {
            if (pTarget->GetQuestStatus(GetGOInfo()->chest.questId) == QUEST_STATUS_INCOMPLETE)
                return true;

            if (LootTemplates_Gameobject.HaveQuestLootForPlayer(GetGOInfo()->GetLootId(), pTarget))
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_GENERIC:
        {
            if (pTarget->GetQuestStatus(GetGOInfo()->_generic.questID) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_SPELL_FOCUS:
        {
            if (pTarget->GetQuestStatus(GetGOInfo()->spellFocus.questID) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_GOOBER:
        {
            // Quest ID can be negative, so prevent error in Player::GetQuestStatus by providing only positive value
            if (GetGOInfo()->goober.questId > 0 && pTarget->GetQuestStatus(GetGOInfo()->goober.questId) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        default:
            break;
    }

    return false;
}

GameObject* GameObject::SummonLinkedTrapIfAny() const
{
    uint32 linkedEntry = GetGOInfo()->GetLinkedGameObjectEntry();
    if (!linkedEntry)
        return nullptr;

    GameObject* linkedGO = new GameObject;
    if (!linkedGO->Create(GetMap()->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, GetMap(),
                          GetPhaseMask(), GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation()))
    {
        delete linkedGO;
        return nullptr;
    }

    linkedGO->m_respawnDelay = 0;
    linkedGO->SetSpellId(GetSpellId());

    if (GetOwnerGuid())
    {
        linkedGO->SetOwnerGuid(GetOwnerGuid());
        linkedGO->SetUInt32Value(GAMEOBJECT_LEVEL, GetUInt32Value(GAMEOBJECT_LEVEL));
    }

    GetMap()->Add(linkedGO);
    linkedGO->AIM_Initialize();

    return linkedGO;
}

void GameObject::TriggerLinkedGameObject(Unit* target) const
{
    uint32 trapEntry = GetGOInfo()->GetLinkedGameObjectEntry();

    if (!trapEntry)
        return;

    GameObjectInfo const* trapInfo = sGOStorage.LookupEntry<GameObjectInfo>(trapEntry);
    if (!trapInfo || trapInfo->type != GAMEOBJECT_TYPE_TRAP)
        return;

    // The range to search for linked trap is weird. We set 0.5 as default. Most (all?)
    // traps are probably expected to be pretty much at the same location as the used GO,
    // so it appears that using range from spell is obsolete.
    float range = 0.5f;

    // search nearest linked GO
    GameObject* trapGO = nullptr;

    {
        // search closest with base of used GO, using max range of trap spell as search radius (why? See above)
        MaNGOS::NearestGameObjectEntryInObjectRangeCheck go_check(*this, trapEntry, range);
        MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> checker(trapGO, go_check);

        Cell::VisitGridObjects(this, checker, range);
    }

    // found correct GO
    if (trapGO)
        trapGO->Use(target);
}

GameObject* GameObject::LookupFishingHoleAround(float range) const
{
    GameObject* ok = nullptr;

    MaNGOS::NearestGameObjectFishingHoleCheck u_check(*this, range);
    MaNGOS::GameObjectSearcher<MaNGOS::NearestGameObjectFishingHoleCheck> checker(ok, u_check);
    Cell::VisitGridObjects(this, checker, range);

    return ok;
}

bool GameObject::IsCollisionEnabled() const
{
    if (!IsSpawned())
        return false;

    if (GetGOInfo()->IsServerOnly()) // serverside GOs do not have LOS
        return false;

    // TODO: Possible that this function must consider multiple checks
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
            return GetGoState() == GO_STATE_READY;
        case GAMEOBJECT_TYPE_TRAP:
            return false;
        default:
            return true;
    }
}

void GameObject::ResetDoorOrButton()
{
    if (m_lootState == GO_READY || m_lootState == GO_JUST_DEACTIVATED)
        return;

    SwitchDoorOrButton(false);
    SetLootState(GO_JUST_DEACTIVATED);
    m_cooldownTime = 0;
}

void GameObject::UseOpenableObject(bool open, uint32 withRestoreTime /*=0*/, bool useAlternativeState /*=false*/)
{
    if (open)
    {
        if (GetGoState() == GO_STATE_READY)
        {
            if (GetLootState() == GO_READY)
                UseDoorOrButton(withRestoreTime, useAlternativeState);
            else
                ResetDoorOrButton();
        }
    }
    else
    {
        if (GetGoState() == GO_STATE_ACTIVE)
        {
            if (GetLootState() == GO_READY)
                UseDoorOrButton(withRestoreTime, useAlternativeState);
            else
                ResetDoorOrButton();
        }
    }
}

void GameObject::UseDoorOrButton(uint32 time_to_restore, bool alternative /* = false */)
{
    if (m_lootState != GO_READY)
        return;

    if (!time_to_restore)
        time_to_restore = GetGOInfo()->GetAutoCloseTime();

    SwitchDoorOrButton(true, alternative);
    SetLootState(GO_ACTIVATED);

    m_cooldownTime = time(nullptr) + time_to_restore;
}

void GameObject::SwitchDoorOrButton(bool activate, bool alternative /* = false */)
{
    if (activate)
        SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    else
        RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

    if (GetGoState() == GO_STATE_READY)                     // if closed -> open
        SetGoState(alternative ? GO_STATE_ACTIVE_ALTERNATIVE : GO_STATE_ACTIVE);
    else                                                    // if open -> close
        SetGoState(GO_STATE_READY);
}

void GameObject::Use(Unit* user, SpellEntry const* spellInfo)
{
    // user must be provided
    MANGOS_ASSERT(user || PrintEntryError("GameObject::Use (without user)"));

    // by default spell caster is user
    Unit* spellCaster = user;
    uint32 spellId = 0;
    uint32 triggeredFlags = 0;
    bool originalCaster = true;

    if (user->IsPlayer() && GetGoType() != GAMEOBJECT_TYPE_TRAP) // workaround for GO casting
        if (!spellInfo && !m_goInfo->IsUsableMounted())
            user->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

    // test only for exist cooldown data (cooldown timer used for door/buttons reset that not have use cooldown)
    uint32 cooldown = GetGOInfo()->GetCooldown();
    if ( cooldown > 0)
    {
        if (m_cooldownTime > sWorld.GetGameTime())
            return;

        m_cooldownTime = sWorld.GetGameTime() + cooldown;
    }

    bool scriptReturnValue = user->GetTypeId() == TYPEID_PLAYER && sScriptDevAIMgr.OnGameObjectUse((Player*)user, this);
    if (!scriptReturnValue)
        GetMap()->ScriptsStart(sGameObjectTemplateScripts, GetEntry(), spellCaster, this);

    if (AI())
        AI()->OnUse(user, spellInfo);

    sWorldState.HandleGameObjectUse(this, user);

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_DOOR:                          // 0
        {
            // doors never really despawn, only reset to default state/flags
            UseDoorOrButton();

            // activate script
            if (!scriptReturnValue)
                GetMap()->ScriptsStart(sGameObjectScripts, m_dbGuid, spellCaster, this);
            return;
        }
        case GAMEOBJECT_TYPE_BUTTON:                        // 1
        {
            // objects used mostly in battlegrounds; there are only a few exceptions to this rule
            // CanUseBattleGroundObject() is already checked in the spell cast; all of these objects use the 1479 lock id
            if (GetGOInfo()->button.noDamageImmune && user->GetTypeId() == TYPEID_PLAYER)
            {
                Player* player = (Player*)user;
                if (BattleGround* bg = player->GetBattleGround())
                    bg->HandlePlayerClickedOnFlag(player, this);

                // note: additional scripts and actions are allowed
            }

            // buttons never really despawn, only reset to default state/flags
            UseDoorOrButton();

            TriggerLinkedGameObject(user);

            // activate script
            if (!scriptReturnValue)
                GetMap()->ScriptsStart(sGameObjectScripts, m_dbGuid, spellCaster, this);

            return;
        }
        case GAMEOBJECT_TYPE_QUESTGIVER:                    // 2
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            if (!sScriptDevAIMgr.OnGossipHello(player, this))
            {
                player->PrepareGossipMenu(this, GetGOInfo()->questgiver.gossipID);
                player->SendPreparedGossip(this);
            }

            return;
        }
        case GAMEOBJECT_TYPE_CHEST:                         // 3
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            TriggerLinkedGameObject(user);

            // TODO: possible must be moved to loot release (in different from linked triggering)
            if (GetGOInfo()->chest.eventId > 0)
            {
                DEBUG_LOG("Chest ScriptStart id %u for %s (opened by %s)", GetGOInfo()->chest.eventId, GetGuidStr().c_str(), user->GetGuidStr().c_str());
                StartEvents_Event(GetMap(), GetGOInfo()->chest.eventId, user, this);
            }

            if (!GetGOInfo()->chest.lockId)
                SetLootState(GO_JUST_DEACTIVATED);

            if (GetFaction())
            {
                UnitList targets;
                MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck check(user, 5.f);
                MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, check);
                Cell::VisitAllObjects(this, searcher, 5.f);
                for (Unit* attacker : targets)
                    if (attacker->IsCreature() && !static_cast<Creature*>(attacker)->IsCritter() && attacker->AI())
                        attacker->AI()->AttackStart(user);
            }

            return;
        }
        case GAMEOBJECT_TYPE_GENERIC:                       // 5
        {
            if (scriptReturnValue)
                return;

            // No known way to exclude some - only different approach is to select despawnable GOs by Entry
            SetLootState(GO_JUST_DEACTIVATED);
            return;
        }
        case GAMEOBJECT_TYPE_TRAP:                          // 6
        {
            if (scriptReturnValue)
                return;

            Unit* owner = GetOwner();
            Unit* caster = owner ? owner : user;

            GameObjectInfo const* goInfo = GetGOInfo();
            float radius = float(goInfo->trap.diameter) / 2.0f;
            bool IsBattleGroundTrap = !radius && goInfo->trap.cooldown == 3 && m_respawnTime == 0;

            if (goInfo->trap.spellId == 6636)
                caster = nullptr;

            if (goInfo->trap.spellId)
                if (CastSpell(caster, user, goInfo->trap.spellId, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetObjectGuid()) != SPELL_CAST_OK)
                    return;
            // use template cooldown if provided
            m_cooldownTime = time(nullptr) + (goInfo->trap.cooldown ? goInfo->trap.cooldown : uint32(4));

            // count charges
            if (goInfo->trap.charges > 0)
                AddUse();

            if (IsBattleGroundTrap && user->GetTypeId() == TYPEID_PLAYER)
            {
                // BattleGround gameobjects case
                if (BattleGround* bg = ((Player*)user)->GetBattleGround())
                    bg->HandleTriggerBuff(GetObjectGuid());
            }

            // TODO: all traps can be activated, also those without spell.
            // Some may have have animation and/or are expected to despawn.

            if (goInfo->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_CUSTOM_ANIM_ON_USE)
                SendGameObjectCustomAnim(GetObjectGuid());

            // TODO: Despawning of traps? (Also related to code in ::Update)
            return;
        }
        case GAMEOBJECT_TYPE_CHAIR:                         // 7 Sitting: Wooden bench, chairs
        {
            GameObjectInfo const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            // a chair may have n slots. we have to calculate their positions and teleport the player to the nearest one

            // check if the db is sane
            if (info->chair.slots > 0)
            {
                float lowestDist = DEFAULT_VISIBILITY_DISTANCE;

                float x_lowest = GetPositionX();
                float y_lowest = GetPositionY();

                // the object orientation + 1/2 pi
                // every slot will be on that straight line
                float orthogonalOrientation = GetOrientation() + M_PI_F * 0.5f;
                // find nearest slot
                for (uint32 i = 0; i < info->chair.slots; ++i)
                {
                    // the distance between this slot and the center of the go - imagine a 1D space
                    float relativeDistance = (info->size * i) - (info->size * (info->chair.slots - 1) / 2.0f);

                    float x_i = GetPositionX() + relativeDistance * cos(orthogonalOrientation);
                    float y_i = GetPositionY() + relativeDistance * sin(orthogonalOrientation);

                    // calculate the distance between the player and this slot
                    float thisDistance = player->GetDistance2d(x_i, y_i);

                    /* debug code. It will spawn a npc on each slot to visualize them.
                    Creature* helper = player->SummonCreature(14496, x_i, y_i, GetPositionZ(), GetOrientation(), TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 10000);
                    std::ostringstream output;
                    output << i << ": thisDist: " << thisDistance;
                    helper->MonsterSay(output.str().c_str(), LANG_UNIVERSAL);
                    */

                    if (thisDistance <= lowestDist)
                    {
                        lowestDist = thisDistance;
                        x_lowest = x_i;
                        y_lowest = y_i;
                    }
                }
                player->TeleportTo(GetMapId(), x_lowest, y_lowest, GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
            }
            else
            {
                // fallback, will always work
                player->TeleportTo(GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
            }
            player->SetStandState(UNIT_STAND_STATE_SIT_LOW_CHAIR + info->chair.height);
            return;
        }
        case GAMEOBJECT_TYPE_SPELL_FOCUS:                   // 8
        {
            TriggerLinkedGameObject(user);

            // some may be activated in addition? Conditions for this? (ex: entry 181616)
            return;
        }
        case GAMEOBJECT_TYPE_GOOBER:                        // 10
        {
            GameObjectInfo const* info = GetGOInfo();
            if (!info)
                return;

            // exception - 180619 - ossirian crystal - supposed to be kept from despawning by a pending spellcast - to be implemented, done in db for now

            TriggerLinkedGameObject(user);

            SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
            SetLootState(GO_ACTIVATED);

            // this appear to be ok, however others exist in addition to this that should have custom (ex: 190510, 188692, 187389)
            if (info->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_CUSTOM_ANIM_ON_USE)
                SendGameObjectCustomAnim(GetObjectGuid(), info->goober.customAnim);
            else
                SetGoState(GO_STATE_ACTIVE);

            if (GetGOInfo()->id != 185871)
                m_cooldownTime = time(nullptr) + info->GetAutoCloseTime();
            else // hypothesis - consumable GOs despawn immediately
                m_cooldownTime = time(nullptr) + 1;

            if (user->GetTypeId() == TYPEID_PLAYER)
            {
                Player* player = (Player*)user;

                if (info->goober.pageId)                    // show page...
                {
                    WorldPacket data(SMSG_GAMEOBJECT_PAGETEXT, 8);
                    data << ObjectGuid(GetObjectGuid());
                    player->GetSession()->SendPacket(data);
                }
                else if (info->goober.gossipID)             // ...or gossip, if page does not exist
                {
                    if (!sScriptDevAIMgr.OnGossipHello(player, this))
                    {
                        player->PrepareGossipMenu(this, info->goober.gossipID);
                        player->SendPreparedGossip(this);
                    }
                }

                if (info->goober.eventId)
                {
                    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "Goober ScriptStart id %u for %s (Used by %s).", info->goober.eventId, GetGuidStr().c_str(), player->GetGuidStr().c_str());

                    // for battleground events we need to allow the event id to be forwarded
                    // Note: this exception is required in order not to change the legacy even handling in DB scripts
                    if (GetMap()->IsBattleGround())
                        StartEvents_Event(GetMap(), info->goober.eventId, this, player, true, player);
                    else
                        StartEvents_Event(GetMap(), info->goober.eventId, player, this);
                }

                // possible quest objective for active quests
                if (info->goober.questId > 0 && sObjectMgr.GetQuestTemplate(info->goober.questId))
                {
                    // Quest require to be active for GO using
                    if (player->GetQuestStatus(info->goober.questId) != QUEST_STATUS_INCOMPLETE)
                        break;
                }

                player->RewardPlayerAndGroupAtCast(this);
            }

            // activate script
            if (!scriptReturnValue)
                GetMap()->ScriptsStart(sGameObjectScripts, m_dbGuid, spellCaster, this);
            else
                return;

            // cast this spell later if provided
            spellId = info->goober.spellId;
            triggeredFlags = TRIGGERED_OLD_TRIGGERED;
            // TODO: GO Casting - make caster of goober spells be GO or owner
            if (Unit* owner = GetOwner())
                spellCaster = owner;

            break;
        }
        case GAMEOBJECT_TYPE_CAMERA:                        // 13
        {
            GameObjectInfo const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            if (info->camera.cinematicId)
                player->SendCinematicStart(info->camera.cinematicId);

            if (info->camera.eventID)
                StartEvents_Event(GetMap(), info->camera.eventID, player, this);

            return;
        }
        case GAMEOBJECT_TYPE_FISHINGNODE:                   // 17 fishing bobber
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            if (player->GetObjectGuid() != GetOwnerGuid())
                return;

            switch (GetLootState())
            {
                case GO_READY:                              // ready for loot
                {
                    // 1) skill must be >= base_zone_skill
                    // 2) if skill == base_zone_skill => 5% chance
                    // 3) chance is linear dependence from (base_zone_skill-skill)

                    uint32 zone, subzone;
                    GetZoneAndAreaId(zone, subzone);

                    int32 zone_skill = sObjectMgr.GetFishingBaseSkillLevel(subzone);
                    if (!zone_skill)
                        zone_skill = sObjectMgr.GetFishingBaseSkillLevel(zone);

                    // provide error, no fishable zone or area should be 0
                    if (!zone_skill)
                        sLog.outErrorDb("Fishable areaId %u are not properly defined in `skill_fishing_base_level`.", subzone);

                    int32 skill = player->GetSkillValue(SKILL_FISHING);
                    int32 chance = skill - zone_skill + 5;
                    int32 roll = irand(1, 100);

                    DEBUG_LOG("Fishing check (skill: %i zone min skill: %i chance %i roll: %i", skill, zone_skill, chance, roll);

                    // normal chance
                    bool success = skill >= zone_skill && chance >= roll;
                    GameObject* fishingHole = nullptr;

                    // overwrite fail in case fishhole if allowed (after 3.3.0)
                    if (!success)
                    {
                        if (!sWorld.getConfig(CONFIG_BOOL_SKILL_FAIL_POSSIBLE_FISHINGPOOL))
                        {
                            // TODO: find reasonable value for fishing hole search
                            fishingHole = LookupFishingHoleAround(20.0f + CONTACT_DISTANCE);
                            if (fishingHole)
                                success = true;
                        }
                    }
                    // just search fishhole for success case
                    else
                        // TODO: find reasonable value for fishing hole search
                        fishingHole = LookupFishingHoleAround(20.0f + CONTACT_DISTANCE);

                    player->UpdateFishingSkill();

                    // fish catch or fail and junk allowed (after 3.1.0)
                    if (success || sWorld.getConfig(CONFIG_BOOL_SKILL_FAIL_LOOT_FISHING))
                    {
                        // prevent removing GO at spell cancel
                        player->RemoveGameObject(this, false);
                        SetOwnerGuid(player->GetObjectGuid());

                        if (fishingHole)                    // will set at success only
                        {
                            fishingHole->Use(player);
                            SetLootState(GO_JUST_DEACTIVATED);
                        }
                        else
                        {
                            delete m_loot;
                            m_loot = new Loot(player, this, success ? LOOT_FISHING : LOOT_FISHING_FAIL);
                            m_loot->ShowContentTo(player);
                        }
                    }
                    else
                    {
                        // fish escaped, can be deleted now
                        SetLootState(GO_JUST_DEACTIVATED);

                        WorldPacket data(SMSG_FISH_ESCAPED, 0);
                        player->GetSession()->SendPacket(data);
                    }
                    break;
                }
                case GO_JUST_DEACTIVATED:                   // nothing to do, will be deleted at next update
                    break;
                default:
                {
                    SetLootState(GO_JUST_DEACTIVATED);

                    WorldPacket data(SMSG_FISH_NOT_HOOKED, 0);
                    player->GetSession()->SendPacket(data);
                    break;
                }
            }

            player->FinishSpell(CURRENT_CHANNELED_SPELL);
            return;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:              // 18
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            if (m_delayedActionTimer)
                return;

            Player* player = (Player*)user;

            Unit* owner = GetOwner();

            GameObjectInfo const* info = GetGOInfo();

            if (owner)
            {
                if (owner->GetTypeId() != TYPEID_PLAYER)
                    return;

                // accept only use by player from same group as owner, excluding owner itself (unique use already added in spell effect)
                if (player == (Player*)owner || (info->summoningRitual.castersGrouped && !player->IsInGroup(owner)))
                    return;

                // expect owner to already be channeling, so if not...
                if (!owner->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    return;
            }
            else
            {
                if (m_firstUser && player->GetObjectGuid() != m_firstUser && info->summoningRitual.castersGrouped)
                {
                    if (Group* group = player->GetGroup())
                    {
                        if (!group->IsMember(m_firstUser))
                            return;
                    }
                    else
                        return;
                }
            }

            AddUniqueUse(player);

            if (info->summoningRitual.animSpell)
                player->CastSpell(player, info->summoningRitual.animSpell, TRIGGERED_NONE);
            else
                player->CastSpell(player, GetSpellId(), TRIGGERED_CHANNEL_ONLY);

            // full amount unique participants including original summoner, need more
            if (GetUniqueUseCount() < info->summoningRitual.reqParticipants)
                return;

            if (info->summoningRitualCustom.delay)
                m_delayedActionTimer = info->summoningRitualCustom.delay;
            else
                TriggerSummoningRitual();

            return;
        }
        case GAMEOBJECT_TYPE_SPELLCASTER:                   // 22
        {
            GameObjectInfo const* info = GetGOInfo();
            if (!info)
                return;

            if (info->spellcaster.partyOnly)
            {
                Unit* caster = GetOwner();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (user->GetTypeId() != TYPEID_PLAYER || !user->IsInGroup(caster))
                    return;
            }

            if (spellCaster->CastSpell(user, info->spellcaster.spellId, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, GetObjectGuid()) != SPELL_CAST_OK)
                return;

            AddUse();

            // Previously we locked all spellcasters on use with no real indication why
            // or timeout of the locking. Now only doing it on it being consumed to prevent further use.
            // spellcaster GOs like city portals should never be locked
            if (info->spellcaster.charges > 0 && !GetUseCount())
                SetUInt32Value(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            return;
        }
        case GAMEOBJECT_TYPE_MEETINGSTONE:                  // 23
        {
            GameObjectInfo const* info = GetGOInfo();

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            Player* targetPlayer = ObjectAccessor::FindPlayer(player->GetSelectionGuid());

            // accept only use by player from same group for caster except caster itself
            if (!targetPlayer || targetPlayer == player || !targetPlayer->IsInGroup(player))
                return;

            // required lvl checks!
            uint8 level = player->GetLevel();
            if (level < info->meetingstone.minLevel || level > info->meetingstone.maxLevel)
                return;

            level = targetPlayer->GetLevel();
            if (level < info->meetingstone.minLevel || level > info->meetingstone.maxLevel)
                return;

            if (info->id == 194097)
                spellId = 61994;                            // Ritual of Summoning
            else
                spellId = 59782;                            // Summoning Stone Effect

            originalCaster = false; // the spell is cast by player even in sniff

            break;
        }
        case GAMEOBJECT_TYPE_FLAGSTAND:                     // 24
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            if (player->CanUseBattleGroundObject())
            {
                // Note: object is used in battlegrounds;
                // it's spawned by default in Warsong Gulch, Arathi Basin and Eye of the Storm
                BattleGround* bg = player->GetBattleGround();
                if (bg)
                    bg->HandlePlayerClickedOnFlag(player, this);

                // handle spell data if available; this usually marks the player as the flag carrier in a battleground
                GameObjectInfo const* info = GetGOInfo();
                if (info && info->flagstand.pickupSpell)
                    spellId = info->flagstand.pickupSpell;

                // when clicked the flag despawns
                SetLootState(GO_JUST_DEACTIVATED);
            }
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGHOLE:                   // 25
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            delete m_loot;
            m_loot = new Loot(player, this, LOOT_FISHINGHOLE);
            m_loot->ShowContentTo(player);

            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT, GetGOInfo()->id);
            return;
        }
        case GAMEOBJECT_TYPE_FLAGDROP:                      // 26
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            if (player->CanUseBattleGroundObject() && player->GetMap()->IsBattleGround())
            {
                // Note: object is used in battlegrounds; object only summoned by spells, doesn't have static spawn data
                // summon is triggered by aura removal or aura death proc when player drop the main flag (type 24) in Warsong Gulch and Eye of the Storm
                // clicking on the flag triggers an event which is handled by battleground script
                GameObjectInfo const* info = GetGOInfo();
                if (info && info->flagdrop.eventID)
                {
                    StartEvents_Event(GetMap(), info->flagdrop.eventID, this, player, true, player);

                    // handle spell data if available; this usually marks the player as the flag carrier in a battleground
                    spellId = info->flagdrop.pickupSpell;

                    // despawn the flag after click
                    SetLootState(GO_JUST_DEACTIVATED);
                }
            }
            break;
        }
        case GAMEOBJECT_TYPE_BARBER_CHAIR:                  // 32
        {
            GameObjectInfo const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = (Player*)user;

            // fallback, will always work
            player->TeleportTo(GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);

            WorldPacket data(SMSG_ENABLE_BARBER_SHOP, 0);
            player->GetSession()->SendPacket(data);

            player->SetStandState(UNIT_STAND_STATE_SIT_LOW_CHAIR + info->barberChair.chairheight);
            return;
        }
        default:
            sLog.outError("GameObject::Use unhandled GameObject type %u (entry %u).", GetGoType(), GetEntry());
            return;
    }

    if (!spellId)
        return;

    SpellEntry const* triggeredSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!triggeredSpellInfo)
    {
        sLog.outError("WORLD: unknown spell id %u at use action for gameobject (Entry: %u GoType: %u )", spellId, GetEntry(), GetGoType());
        return;
    }

    Spell* spell = new Spell(spellCaster, triggeredSpellInfo, triggeredFlags, originalCaster ? GetObjectGuid() : ObjectGuid());

    // spell target is user of GO
    SpellCastTargets targets;
    targets.setUnitTarget(user);

    spell->SpellStart(&targets);
}

// overwrite WorldObject function for proper name localization
const char* GameObject::GetNameForLocaleIdx(int32 loc_idx) const
{
    if (loc_idx >= 0)
    {
        GameObjectLocale const* cl = sObjectMgr.GetGameObjectLocale(GetEntry());
        if (cl)
        {
            if (cl->Name.size() > (size_t)loc_idx && !cl->Name[loc_idx].empty())
                return cl->Name[loc_idx].c_str();
        }
    }

    return GetName();
}

using G3D::Quat;
struct QuaternionCompressed
{
    QuaternionCompressed() : m_raw(0) {}
    QuaternionCompressed(int64 val) : m_raw(val) {}
    QuaternionCompressed(const Quat& quat) { Set(quat); }

    enum
    {
        PACK_COEFF_YZ = 1 << 20,
        PACK_COEFF_X = 1 << 21,
    };

    void Set(const Quat& quat)
    {
        int8 w_sign = (quat.w >= 0 ? 1 : -1);
        int64 X = int32(quat.x * PACK_COEFF_X) * w_sign & ((1 << 22) - 1);
        int64 Y = int32(quat.y * PACK_COEFF_YZ) * w_sign & ((1 << 21) - 1);
        int64 Z = int32(quat.z * PACK_COEFF_YZ) * w_sign & ((1 << 21) - 1);
        m_raw = Z | (Y << 21) | (X << 42);
    }

    Quat Unpack() const
    {
        double x = (double)(m_raw >> 42) / (double)PACK_COEFF_X;
        double y = (double)(m_raw << 22 >> 43) / (double)PACK_COEFF_YZ;
        double z = (double)(m_raw << 43 >> 43) / (double)PACK_COEFF_YZ;
        double w = 1 - (x * x + y * y + z * z);
        MANGOS_ASSERT(w >= 0);
        w = sqrt(w);

        return Quat(x, y, z, w);
    }

    int64 m_raw;
};

void GameObject::SetLocalRotation(float qx, float qy, float qz, float qw)
{
    Quat rotation(qx, qy, qz, qw);
    // Temporary solution for gameobjects that has no rotation data in DB:
    if (qz == 0.f && qw == 0.f)
        rotation = Quat::fromAxisAngleRotation(G3D::Vector3::unitZ(), GetOrientation());

    rotation.unitize();
    m_packedRotation = QuaternionCompressed(rotation).m_raw;
    m_localRotation.x = rotation.x;
    m_localRotation.y = rotation.y;
    m_localRotation.z = rotation.z;
    m_localRotation.w = rotation.w;
}

void GameObject::SetTransportPathRotation(const QuaternionData& rotation)
{
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 0, rotation.x);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 1, rotation.y);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 2, rotation.z);
    SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, rotation.w);
}

void GameObject::SetLocalRotationAngles(float z_rot, float y_rot, float x_rot)
{
    Quat quat(G3D::Matrix3::fromEulerAnglesZYX(z_rot, y_rot, x_rot));
    SetLocalRotation(quat.x, quat.y, quat.z, quat.w);
}

void GameObject::SetLootState(LootState state)
{
    m_lootState = state;
    UpdateCollisionState();

    // Call for GameObjectAI script
    if (m_AI)
        m_AI->OnLootStateChange();
}

void GameObject::SetGoState(GOState state)
{
    SetByteValue(GAMEOBJECT_BYTES_1, 0, state);
    UpdateCollisionState();
    if (AI())
        AI()->OnGoStateChange(state);
}

void GameObject::SetDisplayId(uint32 modelId)
{
    SetUInt32Value(GAMEOBJECT_DISPLAYID, modelId);
    m_displayInfo = sGameObjectDisplayInfoStore.LookupEntry(modelId);
    UpdateModel();
}

void GameObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    WorldObject::SetPhaseMask(newPhaseMask, update);
    UpdateCollisionState();
}

void GameObject::UpdateCollisionState() const
{
    if (!m_model || !IsInWorld())
        return;

    m_model->enable(IsCollisionEnabled() ? GetPhaseMask() : 0);
}

void GameObject::UpdateModel()
{
    if (m_model && IsInWorld() && GetMap()->ContainsGameObjectModel(*m_model))
        GetMap()->RemoveGameObjectModel(*m_model);
    delete m_model;

    m_model = GameObjectModel::construct(this);
    if (m_model)
        GetMap()->InsertGameObjectModel(*m_model);
}

void GameObject::AddModelToMap()
{
    if (!m_model)
        return;

    if (!GetMap()->ContainsGameObjectModel(*m_model))
    {
        m_model->Relocate(*this);
        GetMap()->InsertGameObjectModel(*m_model);
    }
}

void GameObject::RemoveModelFromMap()
{
    if (!m_model)
        return;

    if (GetMap()->ContainsGameObjectModel(*m_model))
        GetMap()->RemoveGameObjectModel(*m_model);
}

void GameObject::UpdateModelPosition()
{
    if (!m_model)
        return;

    if (GetMap()->ContainsGameObjectModel(*m_model))
    {
        GetMap()->RemoveGameObjectModel(*m_model);
        m_model->Relocate(*this);
        GetMap()->InsertGameObjectModel(*m_model);
    }
}

Player* GameObject::GetOriginalLootRecipient() const
{
    return m_lootRecipientGuid ? ObjectAccessor::FindPlayer(m_lootRecipientGuid) : nullptr;
}

Group* GameObject::GetGroupLootRecipient() const
{
    // original recipient group if set and not disbanded
    return m_lootGroupRecipientId ? sObjectMgr.GetGroupById(m_lootGroupRecipientId) : nullptr;
}

Player* GameObject::GetLootRecipient() const
{
    // original recipient group if set and not disbanded
    Group* group = GetGroupLootRecipient();

    // original recipient player if online
    Player* player = GetOriginalLootRecipient();

    // if group not set or disbanded return original recipient player if any
    if (!group)
        return player;

    // group case

    // return player if it still be in original recipient group
    if (player && player->GetGroup() == group)
        return player;

    // find any in group
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        if (Player* newPlayer = itr->getSource())
            return newPlayer;

    return nullptr;
}

void GameObject::SetLootRecipient(Unit* pUnit)
{
    // set the player whose group should receive the right
    // to loot the gameobject after its used
    // should be set to nullptr after the loot disappears

    if (!pUnit)
    {
        m_lootRecipientGuid.Clear();
        m_lootGroupRecipientId = 0;
        return;
    }

    Player* player = pUnit->GetBeneficiaryPlayer();
    if (!player)                                            // normal creature, no player involved
        return;

    // set player for non group case or if group will disbanded
    m_lootRecipientGuid = player->GetObjectGuid();

    // set group for group existed case including if player will leave group at loot time
    if (Group* group = player->GetGroup())
        m_lootGroupRecipientId = group->GetId();
}

GameObject* GameObject::GetLinkedTrap()
{
    return GetMap()->GetGameObject(m_linkedTrap);
}

bool GameObject::IsInSkillupList(Player* player) const
{
    return m_SkillupSet.find(player->GetObjectGuid()) != m_SkillupSet.end();
}

void GameObject::AddToSkillupList(Player* player)
{
    m_SkillupSet.insert(player->GetObjectGuid());
}

struct AddGameObjectToRemoveListInMapsWorker
{
    AddGameObjectToRemoveListInMapsWorker(ObjectGuid guid) : i_guid(guid) {}

    void operator()(Map* map)
    {
        if (GameObject* pGameobject = map->GetGameObject(i_guid))
            pGameobject->AddObjectToRemoveList();
    }

    ObjectGuid i_guid;
};

void GameObject::AddToRemoveListInMaps(uint32 db_guid, GameObjectData const* data)
{
    AddGameObjectToRemoveListInMapsWorker worker(ObjectGuid(HIGHGUID_GAMEOBJECT, data->id, db_guid));
    sMapMgr.DoForAllMapsWithMapId(data->mapid, worker);
}

struct SpawnGameObjectInMapsWorker
{
    SpawnGameObjectInMapsWorker(uint32 guid, GameObjectData const* data)
        : i_guid(guid), i_data(data) {}

    void operator()(Map* map)
    {
        // Spawn if necessary (loaded grids only)
        if (map->IsLoaded(i_data->posX, i_data->posY))
        {
            GameObjectData const* data = sObjectMgr.GetGOData(i_guid);
            MANGOS_ASSERT(data);
            GameObject* pGameobject = GameObject::CreateGameObject(data->id);
            // DEBUG_LOG("Spawning gameobject %u", *itr);
            if (!pGameobject->LoadFromDB(i_guid, map, i_guid, 0))
            {
                delete pGameobject;
            }
        }
    }

    uint32 i_guid;
    GameObjectData const* i_data;
};

void GameObject::SpawnInMaps(uint32 db_guid, GameObjectData const* data)
{
    SpawnGameObjectInMapsWorker worker(db_guid, data);
    sMapMgr.DoForAllMapsWithMapId(data->mapid, worker);
}

bool GameObject::HasStaticDBSpawnData() const
{
    return sObjectMgr.GetGOData(m_dbGuid) != nullptr;
}

void GameObject::SetCapturePointSlider(float value, bool isLocked)
{
    GameObjectInfo const* info = GetGOInfo();

    m_captureSlider = value;

    // only activate non-locked capture point
    if (!isLocked)
        SetLootState(GO_ACTIVATED);

    // set the state of the capture point based on the slider value
    if ((int)m_captureSlider == CAPTURE_SLIDER_ALLIANCE)
        m_captureState = CAPTURE_STATE_WIN_ALLIANCE;
    else if ((int)m_captureSlider == CAPTURE_SLIDER_HORDE)
        m_captureState = CAPTURE_STATE_WIN_HORDE;
    else if (m_captureSlider > CAPTURE_SLIDER_MIDDLE + info->capturePoint.neutralPercent * 0.5f)
        m_captureState = CAPTURE_STATE_PROGRESS_ALLIANCE;
    else if (m_captureSlider < CAPTURE_SLIDER_MIDDLE - info->capturePoint.neutralPercent * 0.5f)
        m_captureState = CAPTURE_STATE_PROGRESS_HORDE;
    else
        m_captureState = CAPTURE_STATE_NEUTRAL;
}

void GameObject::TickCapturePoint()
{
    // TODO: On retail: Ticks every 5.2 seconds. slider value increase when new player enters on tick

    GameObjectInfo const* info = GetGOInfo();
    float radius = info->capturePoint.radius;

    // search for players in radius
    PlayerList capturingPlayers;
    MaNGOS::AnyPlayerInCapturePointRange u_check(this, radius);
    MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInCapturePointRange> checker(capturingPlayers, u_check);
    Cell::VisitWorldObjects(this, checker, radius);

    GuidSet tempUsers(m_UniqueUsers);
    uint32 neutralPercent = info->capturePoint.neutralPercent;
    int oldValue = m_captureSlider;
    int rangePlayers = 0;

    for (auto& capturingPlayer : capturingPlayers)
    {
        if (capturingPlayer->GetTeam() == ALLIANCE)
            ++rangePlayers;
        else
            --rangePlayers;

        ObjectGuid guid = capturingPlayer->GetObjectGuid();
        if (!tempUsers.erase(guid))
        {
            // new player entered capture point zone
            m_UniqueUsers.insert(guid);

            // update pvp info
            capturingPlayer->pvpInfo.inPvPCapturePoint = true;

            // send capture point enter packets
            capturingPlayer->SendUpdateWorldState(info->capturePoint.worldState3, neutralPercent);
            capturingPlayer->SendUpdateWorldState(info->capturePoint.worldState2, oldValue);
            capturingPlayer->SendUpdateWorldState(info->capturePoint.worldState1, WORLD_STATE_ADD);
            capturingPlayer->SendUpdateWorldState(info->capturePoint.worldState2, oldValue); // also redundantly sent on retail to prevent displaying the initial capture direction on client capture slider incorrectly
        }
    }

    for (auto tempUser : tempUsers)
    {
        if (Player* owner = GetMap()->GetPlayer(tempUser))
        {
            // update pvp info
            owner->pvpInfo.inPvPCapturePoint = false;

            // send capture point leave packet
            owner->SendUpdateWorldState(info->capturePoint.worldState1, WORLD_STATE_REMOVE);
        }

        // player left capture point zone
        m_UniqueUsers.erase(tempUser);
    }

    // return if there are not enough players capturing the point (works because minSuperiority is always 1)
    if (rangePlayers == 0)
    {
        // set to inactive if all players left capture point zone
        if (m_UniqueUsers.empty())
            SetActiveObjectState(false);
        return;
    }

    // prevents unloading gameobject before all players left capture point zone (to prevent m_UniqueUsers not being cleared if grid is set to idle)
    SetActiveObjectState(true);

    // cap speed
    int maxSuperiority = info->capturePoint.maxSuperiority;
    if (rangePlayers > maxSuperiority)
        rangePlayers = maxSuperiority;
    else if (rangePlayers < -maxSuperiority)
        rangePlayers = -maxSuperiority;

    // time to capture from 0% to 100% is maxTime for minSuperiority amount of players and minTime for maxSuperiority amount of players (linear function: y = dy/dx*x+d)
    float deltaSlider = info->capturePoint.minTime;

    if (int deltaSuperiority = maxSuperiority - info->capturePoint.minSuperiority)
        deltaSlider += (float)(maxSuperiority - abs(rangePlayers)) / deltaSuperiority * (info->capturePoint.maxTime - info->capturePoint.minTime);

    // calculate changed slider value for a duration of 5 seconds (5 * 100%)
    deltaSlider = 500.0f / deltaSlider;

    Team progressFaction;
    if (rangePlayers > 0)
    {
        progressFaction = ALLIANCE;
        m_captureSlider += deltaSlider;
        if (m_captureSlider > CAPTURE_SLIDER_ALLIANCE)
            m_captureSlider = CAPTURE_SLIDER_ALLIANCE;
    }
    else
    {
        progressFaction = HORDE;
        m_captureSlider -= deltaSlider;
        if (m_captureSlider < CAPTURE_SLIDER_HORDE)
            m_captureSlider = CAPTURE_SLIDER_HORDE;
    }

    // return if slider did not move a whole percent
    if ((int)m_captureSlider == oldValue)
        return;

    // on retail this is also sent to newly added players even though they already received a slider value
    for (auto& capturingPlayer : capturingPlayers)
        capturingPlayer->SendUpdateWorldState(info->capturePoint.worldState2, (uint32)m_captureSlider);

    // send capture point events
    uint32 eventId = 0;

    /* WIN EVENTS */
    // alliance wins tower with max points
    if (m_captureState != CAPTURE_STATE_WIN_ALLIANCE && (int)m_captureSlider == CAPTURE_SLIDER_ALLIANCE)
    {
        eventId = info->capturePoint.winEventID1;
        m_captureState = CAPTURE_STATE_WIN_ALLIANCE;
    }
    // horde wins tower with max points
    else if (m_captureState != CAPTURE_STATE_WIN_HORDE && (int)m_captureSlider == CAPTURE_SLIDER_HORDE)
    {
        eventId = info->capturePoint.winEventID2;
        m_captureState = CAPTURE_STATE_WIN_HORDE;
    }

    /* PROGRESS EVENTS */
    // alliance takes the tower from neutral, contested or horde (if there is no neutral area) to alliance
    else if (m_captureState != CAPTURE_STATE_PROGRESS_ALLIANCE && m_captureSlider > CAPTURE_SLIDER_MIDDLE + neutralPercent * 0.5f && progressFaction == ALLIANCE)
    {
        eventId = info->capturePoint.progressEventID1;

        // handle objective complete
        if (m_captureState == CAPTURE_STATE_NEUTRAL)
            if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript((*capturingPlayers.begin())->GetCachedZoneId()))
                outdoorPvP->HandleObjectiveComplete(eventId, capturingPlayers, progressFaction);

        // set capture state to alliance
        m_captureState = CAPTURE_STATE_PROGRESS_ALLIANCE;
    }
    // horde takes the tower from neutral, contested or alliance (if there is no neutral area) to horde
    else if (m_captureState != CAPTURE_STATE_PROGRESS_HORDE && m_captureSlider < CAPTURE_SLIDER_MIDDLE - neutralPercent * 0.5f && progressFaction == HORDE)
    {
        eventId = info->capturePoint.progressEventID2;

        // handle objective complete
        if (m_captureState == CAPTURE_STATE_NEUTRAL)
            if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript((*capturingPlayers.begin())->GetCachedZoneId()))
                outdoorPvP->HandleObjectiveComplete(eventId, capturingPlayers, progressFaction);

        // set capture state to horde
        m_captureState = CAPTURE_STATE_PROGRESS_HORDE;
    }

    /* NEUTRAL EVENTS */
    // alliance takes the tower from horde to neutral
    else if (m_captureState != CAPTURE_STATE_NEUTRAL && m_captureSlider >= CAPTURE_SLIDER_MIDDLE - neutralPercent * 0.5f && m_captureSlider <= CAPTURE_SLIDER_MIDDLE + neutralPercent * 0.5f && progressFaction == ALLIANCE)
    {
        eventId = info->capturePoint.neutralEventID1;
        m_captureState = CAPTURE_STATE_NEUTRAL;
    }
    // horde takes the tower from alliance to neutral
    else if (m_captureState != CAPTURE_STATE_NEUTRAL && m_captureSlider >= CAPTURE_SLIDER_MIDDLE - neutralPercent * 0.5f && m_captureSlider <= CAPTURE_SLIDER_MIDDLE + neutralPercent * 0.5f && progressFaction == HORDE)
    {
        eventId = info->capturePoint.neutralEventID2;
        m_captureState = CAPTURE_STATE_NEUTRAL;
    }

    /* CONTESTED EVENTS */
    // alliance attacks tower which is in control or progress by horde (except if alliance also gains control in that case)
    else if ((m_captureState == CAPTURE_STATE_WIN_HORDE || m_captureState == CAPTURE_STATE_PROGRESS_HORDE) && progressFaction == ALLIANCE)
    {
        eventId = info->capturePoint.contestedEventID1;
        m_captureState = CAPTURE_STATE_CONTEST_HORDE;
    }
    // horde attacks tower which is in control or progress by alliance (except if horde also gains control in that case)
    else if ((m_captureState == CAPTURE_STATE_WIN_ALLIANCE || m_captureState == CAPTURE_STATE_PROGRESS_ALLIANCE) && progressFaction == HORDE)
    {
        eventId = info->capturePoint.contestedEventID2;
        m_captureState = CAPTURE_STATE_CONTEST_ALLIANCE;
    }

    if (eventId)
        StartEvents_Event(GetMap(), eventId, this, this, true, *capturingPlayers.begin());
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
//                              Destructible GO handling
// ////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::DealGameObjectDamage(uint32 damage, uint32 spell, Unit* caster)
{
    MANGOS_ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);
    MANGOS_ASSERT(spell && sSpellTemplate.LookupEntry<SpellEntry>(spell) && caster);

    if (!damage)
        return;

    ForceGameObjectHealth(-int32(damage), caster);

    WorldPacket data(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, 9 + 9 + 9 + 4 + 4);
    data << GetPackGUID();
    data << caster->GetPackGUID();
    data << caster->GetBeneficiary()->GetPackGUID();
    data << uint32(damage);
    data << uint32(spell);
    SendMessageToSet(data, false);
}

void GameObject::RebuildGameObject(Unit* caster)
{
    MANGOS_ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);
    MANGOS_ASSERT(caster);

    ForceGameObjectHealth(0, caster);
}

void GameObject::ForceGameObjectHealth(int32 diff, Unit* caster)
{
    MANGOS_ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);
    MANGOS_ASSERT(caster || diff >= 0);

    if (diff < 0)                                           // Taken damage
    {
        DEBUG_FILTER_LOG(LOG_FILTER_DAMAGE, "DestructibleGO: %s taken damage %u dealt by %s", GetGuidStr().c_str(), uint32(-diff), caster->GetGuidStr().c_str());

        if (m_useTimes > uint32(-diff))
            m_useTimes += diff;
        else
            m_useTimes = 0;
    }
    else if (diff == 0 && GetMaxHealth())                   // Rebuild - TODO: Rebuilding over time with special display-id?
    {
        DEBUG_FILTER_LOG(LOG_FILTER_DAMAGE, "DestructibleGO: %s start rebuild by %s", GetGuidStr().c_str(), caster->GetGuidStr().c_str());

        m_useTimes = GetMaxHealth();
        // Start Event if exist
        if (caster && m_goInfo->destructibleBuilding.rebuildingEvent)
            StartEvents_Event(GetMap(), m_goInfo->destructibleBuilding.rebuildingEvent, this, caster->GetBeneficiary(), true, caster->GetBeneficiary());
    }
    else                                                    // Set to value
        m_useTimes = uint32(diff);

    uint32 newDisplayId = 0xFFFFFFFF;                       // Set to invalid -1 to track if we switched to a change state
    DestructibleModelDataEntry const* destructibleInfo = sDestructibleModelDataStore.LookupEntry(m_goInfo->destructibleBuilding.destructibleData);

    // Get Current State - Note about order: Important for GetMaxHealth() == 0
    if (m_useTimes == GetMaxHealth())                       // Full Health
    {
        DEBUG_FILTER_LOG(LOG_FILTER_DAMAGE, "DestructibleGO: %s set to full health %u", GetGuidStr().c_str(), m_useTimes);

        RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_DESTROYED);
        newDisplayId = m_goInfo->displayId;

        // Start Event if exist
        if (caster && m_goInfo->destructibleBuilding.intactEvent)
            StartEvents_Event(GetMap(), m_goInfo->destructibleBuilding.intactEvent, this, caster->GetBeneficiary(), true, caster->GetBeneficiary());
    }
    else if (m_useTimes == 0)                               // Destroyed
    {
        if (!HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED))     // Was not destroyed before
        {
            DEBUG_FILTER_LOG(LOG_FILTER_DAMAGE, "DestructibleGO: %s got destroyed", GetGuidStr().c_str());

            RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
            SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);

            // Get destroyed DisplayId
            if ((!m_goInfo->destructibleBuilding.destroyedDisplayId || m_goInfo->destructibleBuilding.destroyedDisplayId == 1) && destructibleInfo)
                newDisplayId = destructibleInfo->destroyedDisplayId;
            else
                newDisplayId = m_goInfo->destructibleBuilding.destroyedDisplayId;

            if (!newDisplayId)                              // No proper destroyed display ID exists, fetch damaged
            {
                if ((!m_goInfo->destructibleBuilding.damagedDisplayId || m_goInfo->destructibleBuilding.damagedDisplayId == 1) && destructibleInfo)
                    newDisplayId = destructibleInfo->damagedDisplayId;
                else
                    newDisplayId = m_goInfo->destructibleBuilding.damagedDisplayId;
            }

            // Start Event if exist
            if (caster && m_goInfo->destructibleBuilding.destroyedEvent)
                StartEvents_Event(GetMap(), m_goInfo->destructibleBuilding.destroyedEvent, this, caster->GetBeneficiary(), true, caster->GetBeneficiary());
        }
    }
    else if (m_useTimes <= m_goInfo->destructibleBuilding.damagedNumHits) // Damaged
    {
        if (!HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED))     // Was not damaged before
        {
            DEBUG_FILTER_LOG(LOG_FILTER_DAMAGE, "DestructibleGO: %s got damaged (health now %u)", GetGuidStr().c_str(), m_useTimes);

            SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);

            // Get damaged DisplayId
            if ((!m_goInfo->destructibleBuilding.damagedDisplayId || m_goInfo->destructibleBuilding.damagedDisplayId == 1) && destructibleInfo)
                newDisplayId = destructibleInfo->damagedDisplayId;
            else
                newDisplayId = m_goInfo->destructibleBuilding.damagedDisplayId;

            // Start Event if exist
            if (caster && m_goInfo->destructibleBuilding.damagedEvent)
                StartEvents_Event(GetMap(), m_goInfo->destructibleBuilding.damagedEvent, this, caster->GetBeneficiary(), true, caster->GetBeneficiary());
        }
    }

    // Set display Id
    if (newDisplayId != 0xFFFFFFFF && newDisplayId != GetDisplayId() && newDisplayId)
        SetDisplayId(newDisplayId);

    // Set health
    SetGoAnimProgress(GetMaxHealth() ? m_useTimes * 255 / GetMaxHealth() : 255);
}

void GameObject::SetDestructibleState(GameObjectDestructibleState state, Unit* attackerOrHealer /*= nullptr*/, bool setHealth /*= false*/)
{
    // the user calling this must know he is already operating on destructible gameobject
    MANGOS_ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);

    switch (state)
    {
        case GO_DESTRUCTIBLE_INTACT:                        // Set to full health
            ForceGameObjectHealth(GetMaxHealth(), attackerOrHealer);
            break;
        case GO_DESTRUCTIBLE_DAMAGED:                       // Set to damaged
            ForceGameObjectHealth(GetGOInfo()->destructibleBuilding.damagedNumHits, attackerOrHealer);
            break;
        case GO_DESTRUCTIBLE_DESTROYED:                     // Set to destroyed
            ForceGameObjectHealth(-int32(GetHealth()), attackerOrHealer);
            break;
        case GO_DESTRUCTIBLE_REBUILDING:                    // Set to rebuilding
            ForceGameObjectHealth(0, attackerOrHealer);
            break;
    }
}

GameObjectDestructibleState GameObject::GetDestructibleState() const
{
    if (HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED))
        return GO_DESTRUCTIBLE_DESTROYED;
    if (HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED))
        return GO_DESTRUCTIBLE_DAMAGED;
    return GO_DESTRUCTIBLE_INTACT;
}

float GameObject::GetInteractionDistance() const
{
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_AREADAMAGE:
            return 0.0f;
        case GAMEOBJECT_TYPE_QUESTGIVER:
        case GAMEOBJECT_TYPE_TEXT:
        case GAMEOBJECT_TYPE_FLAGSTAND:
        case GAMEOBJECT_TYPE_FLAGDROP:
        case GAMEOBJECT_TYPE_MINI_GAME:
            return 5.5555553f;
        case GAMEOBJECT_TYPE_BINDER:
            return 10.0f;
        case GAMEOBJECT_TYPE_CHAIR:
        case GAMEOBJECT_TYPE_BARBER_CHAIR:
            return 3.0f;
        case GAMEOBJECT_TYPE_FISHINGNODE:
            return 100.0f;
        case GAMEOBJECT_TYPE_FISHINGHOLE:
            return 20.0f + CONTACT_DISTANCE; // max spell range
        case GAMEOBJECT_TYPE_CAMERA:
        case GAMEOBJECT_TYPE_MAP_OBJECT:
        case GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY:
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
        case GAMEOBJECT_TYPE_DOOR:
            return 5.0f;
            // Following values are not blizzlike
        case GAMEOBJECT_TYPE_GUILD_BANK:
        case GAMEOBJECT_TYPE_MAILBOX:
            // Successful mailbox interaction is rather critical to the client, failing it will start a minute-long cooldown until the next mail query may be executed.
            // And since movement info update is not sent with mailbox interaction query, server may find the player outside of interaction range. Thus we increase it.
            return 10.0f; // 5.0f is blizzlike
        default:
            return INTERACTION_DISTANCE;
    }
}

void GameObject::SetInUse(bool use)
{
    m_isInUse = use;
    if (use)
        SetGoState(GO_STATE_ACTIVE);
    else
        SetGoState(GO_STATE_READY);
}

void GameObject::TriggerSummoningRitual()
{
    const GameObjectInfo* info = GetGOInfo();

    Unit* owner = GetOwner();
    Unit* caster = owner;

    if (!owner)
    {
        if (Player* firstUser = GetMap()->GetPlayer(m_firstUser))
            caster = firstUser;
    }
    else
        // finish owners spell
        owner->FinishSpell(CURRENT_CHANNELED_SPELL);

    if (caster) // two caster checks to maintain order
    {
        for (auto m_UniqueUser : m_UniqueUsers)
        {
            if (m_UniqueUser == caster->GetObjectGuid())
                continue;

            if (Player* pUnique = GetMap()->GetPlayer(m_UniqueUser))
                pUnique->FinishSpell(CURRENT_CHANNELED_SPELL);
        }
    }

    // can be deleted now, if
    if (!info->summoningRitual.ritualPersistent)
        SetLootState(GO_JUST_DEACTIVATED);
    // reset ritual for this GO
    else
        ClearAllUsesData();

    uint32 spellId = info->summoningRitual.spellId;
    if (spellId == 62330)                           // GO store nonexistent spell, replace by expected
        spellId = 61993;

    if (caster)
        caster->CastSpell(sObjectMgr.GetPlayer(m_actionTarget), spellId, TRIGGERED_OLD_TRIGGERED | TRIGGERED_INSTANT_CAST, nullptr, nullptr, GetObjectGuid());
}

void GameObject::TriggerDelayedAction()
{
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
            TriggerSummoningRitual();
            break;
        default:
            break;
    }
}

uint32 GameObject::GetScriptId() const
{
    return ObjectMgr::GetGameObjectInfo(GetEntry())->ScriptId;
}

void GameObject::AIM_Initialize()
{
    m_AI.reset(sScriptDevAIMgr.GetGameObjectAI(this));

    if (AI())
        AI()->JustSpawned();

    if (InstanceData* iData = GetMap()->GetInstanceData())
        iData->OnObjectSpawn(this);
}

bool GameObject::IsAtInteractDistance(Player const* player, uint32 maxRange) const
{
    SpellEntry const* spellInfo;
    if (maxRange || (spellInfo = GetSpellForLock(player)))
    {
        if (maxRange == 0.f)
        {
            SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
            maxRange = GetSpellMaxRange(srange);
        }

        if (GetGoType() == GAMEOBJECT_TYPE_SPELL_FOCUS)
            return maxRange * maxRange >= GetDistance(player, true, DIST_CALC_NONE);

        if (sGameObjectDisplayInfoStore.LookupEntry(GetGOInfo()->displayId))
            return IsAtInteractDistance(player->GetPosition(), maxRange);
    }

    return IsAtInteractDistance(player->GetPosition(), GetInteractionDistance());
}

bool GameObject::_IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const
{
    // behavior verified on classic
    // TODO: needs more research
    if (GetGoType() == GAMEOBJECT_TYPE_TRAP && GetGOInfo()->GetLockId() == 12)
    {
        float distsq = GetDistance(obj, is3D, DIST_CALC_NONE);
        return distsq < dist2compare * dist2compare;
    }

    return IsAtInteractDistance(obj->GetPosition(), obj->GetCombatReach() + dist2compare, is3D);
}

bool GameObject::IsAtInteractDistance(Position const& pos, float radius, bool is3D) const
{
    if (GameObjectDisplayInfoEntry const* displayInfo = m_displayInfo)
    {
        float scale = GetObjectScale();

        float minX = displayInfo->minX * scale - radius;
        float minY = displayInfo->minY * scale - radius;
        float minZ = displayInfo->minZ * scale - radius;
        float maxX = displayInfo->maxX * scale + radius;
        float maxY = displayInfo->maxY * scale + radius;
        float maxZ = displayInfo->maxZ * scale + radius;

        QuaternionData worldRotation = GetWorldRotation();
        G3D::Quat worldRotationQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);

        return G3D::CoordinateFrame{ { worldRotationQuat }, { GetPositionX(), GetPositionY(), GetPositionZ() } }
            .toWorldSpace(G3D::Box{ { minX, minY, minZ }, { maxX, maxY, maxZ } })
            .contains({ pos.GetPositionX(), pos.GetPositionY(), is3D ? pos.GetPositionZ() : GetPositionZ() });
    }

    return GetDistance(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), DIST_CALC_NONE) <= (radius * radius);
}

SpellEntry const* GameObject::GetSpellForLock(Player const* player) const
{
    if (!player)
        return nullptr;

    uint32 lockId = GetGOInfo()->GetLockId();
    if (!lockId)
        return nullptr;

    LockEntry const* lock = sLockStore.LookupEntry(lockId);
    if (!lock)
        return nullptr;

    for (uint8 i = 0; i < MAX_LOCK_CASE; ++i)
    {
        if (!lock->Type[i])
            continue;

        if (lock->Type[i] == LOCK_KEY_SPELL)
            if (SpellEntry const* spell = sSpellTemplate.LookupEntry<SpellEntry>(lock->Index[i]))
                return spell;

        if (lock->Type[i] != LOCK_KEY_SKILL)
            break;

        for (auto&& playerSpell : player->GetSpellMap())
            if (SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(playerSpell.first))
                for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                    if (spellInfo->Effect[i] == SPELL_EFFECT_OPEN_LOCK && ((uint32)spellInfo->EffectMiscValue[i]) == lock->Index[i])
                        if (player->CalculateSpellEffectValue(nullptr, spellInfo, SpellEffectIndex(i), nullptr) >= int32(lock->Skill[i]))
                            return spellInfo;
    }

    return nullptr;
}

SpellCastResult GameObject::CastSpell(Unit* temporaryCaster, Unit* Victim, uint32 spellId, uint32 triggeredFlags, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    return CastSpell(temporaryCaster, Victim, sSpellTemplate.LookupEntry<SpellEntry>(spellId), triggeredFlags, castItem, triggeredByAura, originalCaster, triggeredBy);
}

SpellCastResult GameObject::CastSpell(Unit* temporaryCaster, Unit* Victim, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell: unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell: unknown spell by caster: %s", GetGuidStr().c_str());
        return SPELL_NOT_FOUND;
    }

    if (IsChanneledSpell(spellInfo)) // GOs cannot cast channeled spells
    {
        sLog.outError("CastSpell: GO entry %u attempted casting spellId %u", GetEntry(), spellInfo->Id);
        return SPELL_FAILED_ERROR;
    }

    if (castItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (triggeredByAura)
    {
        if (!originalCaster)
            originalCaster = triggeredByAura->GetCasterGuid();

        triggeredBy = triggeredByAura->GetSpellProto();
    }

    Spell* spell = new Spell(this, spellInfo, triggeredFlags, GetObjectGuid(), triggeredBy);
    spell->SetFakeCaster(temporaryCaster);

    SpellCastTargets targets;
    targets.setUnitTarget(Victim);

    if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        targets.setDestination(Victim->GetPositionX(), Victim->GetPositionY(), Victim->GetPositionZ());
    if (spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
        if (WorldObject* caster = spell->GetCastingObject())
            targets.setSource(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());

    spell->SetCastItem(castItem);
    return spell->SpellStart(&targets, triggeredByAura);
}

void GameObject::GenerateLootFor(Player* player)
{
    if (!m_loot)
        m_loot = new Loot(player, this, LOOT_SKINNING, true);
}

void GameObject::SetCooldown(uint32 cooldown)
{
    m_cooldownTime = time(nullptr) + cooldown;
}

void GameObject::SetGameObjectGroup(GameObjectGroup* group)
{
    m_goGroup = group;
    group->AddObject(GetDbGuid(), GetEntry());
}

void GameObject::ClearGameObjectGroup()
{
    if (m_goGroup)
        m_goGroup->RemoveObject(this);
    m_goGroup = nullptr;
}

QuaternionData GameObject::GetWorldRotation() const
{
    QuaternionData localRotation = GetLocalRotation();
    if (GenericTransport* transport = GetTransport())
    {
        QuaternionData worldRotation = transport->GetWorldRotation();

        G3D::Quat worldRotationQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);
        G3D::Quat localRotationQuat(localRotation.x, localRotation.y, localRotation.z, localRotation.w);

        G3D::Quat resultRotation = localRotationQuat * worldRotationQuat;

        return QuaternionData(resultRotation.x, resultRotation.y, resultRotation.z, resultRotation.w);
    }
    return localRotation;
}

const QuaternionData GameObject::GetLocalRotation() const
{
    return m_localRotation;
}

void GameObject::ForcedDespawn(uint32 timeMSToDespawn)
{
    if (timeMSToDespawn)
    {
        ForcedDespawnDelayGameObjectEvent* pEvent = new ForcedDespawnDelayGameObjectEvent(*this);

        m_events.AddEvent(pEvent, m_events.CalculateTime(timeMSToDespawn));
        return;
    }

    SetForcedDespawn();
    SetLootState(GO_JUST_DEACTIVATED);

    // some GOs have respawn time not filled to prevent despawn on action - need to override that this time
    if (!m_respawnDelay && GetDbGuid() && !m_respawnOverriden)
    {
        // only static spawns should arrive here
        if (GameObjectData const* data = sObjectMgr.GetGOData(GetDbGuid()))
            SetRespawnDelay(data->GetRandomRespawnTime(), true);
    }
}

bool ForcedDespawnDelayGameObjectEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.ForcedDespawn();
    return true;
}
