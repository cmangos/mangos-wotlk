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

#include "Entities/Transports.h"
#include "Maps/MapManager.h"
#include "Globals/ObjectMgr.h"
#include "Entities/ObjectGuid.h"
#include "MotionGenerators/Path.h"

#include "WorldPacket.h"
#include "Server/DBCStores.h"
#include "ProgressBar.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"

#include "Movement/MoveSpline.h"

#include "Entities/Vehicle.h"
#include "Entities/Unit.h"

#include <G3D/Quat.h>

void MapManager::LoadTransports()
{
    sTransportMgr.LoadTransportTemplates();

    QueryResult* result = WorldDatabase.Query("SELECT entry, name, period FROM transports");

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u transports", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        std::string name = fields[1].GetCppString();
        uint32 period = fields[2].GetUInt32();

        const GameObjectInfo* goinfo = ObjectMgr::GetGameObjectInfo(entry);

        if (!goinfo)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template missing", entry, name.c_str());
            continue;
        }

        if (goinfo->type != GAMEOBJECT_TYPE_MO_TRANSPORT)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template type wrong", entry, name.c_str());
            continue;
        }

        TransportTemplate* transportTemplate = sTransportMgr.GetTransportTemplate(entry);
        if (!transportTemplate)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, transport template missing (core generated)", entry, name.c_str());
            continue;
        }

        const MapEntry* pMapInfo = sMapStore.LookupEntry(transportTemplate->keyFrames.begin()->Node->mapid);
        if (!pMapInfo)
            continue;

        transportTemplate->pathTime = period;
        transportTemplate->keyFrames.back().DepartureTime = period;

        m_transportsByMap[pMapInfo->MapID].push_back(transportTemplate);
        m_transportsByEntry[entry].push_back(transportTemplate);

        ++count;
    }
    while (result->NextRow());
    delete result;

    // check transport data DB integrity
    result = WorldDatabase.Query("SELECT gameobject.guid,gameobject.id,transports.name FROM gameobject,transports WHERE gameobject.id = transports.entry");
    if (result)                                             // wrong data found
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 guid  = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();
            std::string name = fields[2].GetCppString();
            sLog.outErrorDb("Transport %u '%s' have record (GUID: %u) in `gameobject`. Transports MUST NOT have any records in `gameobject` or its behavior will be unpredictable/bugged.", entry, name.c_str(), guid);
        }
        while (result->NextRow());

        delete result;
    }

    sLog.outString(">> Loaded %u transports", count);
    sLog.outString();
}

Transport::Transport(TransportTemplate const& transportTemplate) : GenericTransport(), m_transportTemplate(transportTemplate), m_isMoving(true), m_lastStopIndex(-1)
{
    m_updateFlag = (UPDATEFLAG_TRANSPORT | UPDATEFLAG_HIGHGUID | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_ROTATION);
}

bool Transport::IsSpawnedByDefault(uint32 entry, Team team)
{
    switch (entry)
    {
        case 201598:        // Halls of Reflection - alliance
        case 201580:        // ICC raid - Skybreaker - alliance
            if (team == HORDE)
                return false;
            break;
        case 201599:        // Halls of Reflection - horde
        case 201812:        // ICC Raid - Orgrim's Hammer - horde
            if (team == ALLIANCE)
                return false;
            break;
        case 201581:        // ICC raid - Orgrim's Hammer - alliance
        case 201811:        // ICC Raid - Skybreaker - horde
        case 201834:        // ICC zeppelin after gunship fight
            return false;
    }

    return true;
}

void Transport::LoadTransport(TransportTemplate const& transportTemplate, Map* map, bool spawnOnDemand /*= false*/)
{
    Transport* t = new Transport(transportTemplate);

    t->SetPeriod(transportTemplate.pathTime);

    // sLog.outString("Loading transport %d between %s, %s", entry, name.c_str(), goinfo->name);

    TaxiPathNodeEntry const* startNode = transportTemplate.keyFrames.begin()->Node;
    float x = startNode->x;
    float y = startNode->y;
    float z = startNode->z;
    float o = t->GetKeyFrames().begin()->InitialOrientation;

    // If we someday decide to use the grid to track transports, here:
    t->SetMap(map);
    t->Object::AddToWorld();

    // creates the Gameobject
    if (!t->Create(transportTemplate.entry, map->GetId(), x, y, z, o, GO_ANIMPROGRESS_DEFAULT, 0))
    {
        delete t;
        return;
    }

    map->AddTransport(t);

    t->SpawnPassengers();

    if (spawnOnDemand)
        t->UpdateForMap(map, true);
}

bool Transport::Create(uint32 guidlow, uint32 mapid, float x, float y, float z, float ang, uint8 animprogress, uint16 dynamicHighValue)
{
    Relocate(x, y, z, ang);
    // instance id and phaseMask isn't set to values different from std.

    if (!IsPositionValid())
    {
        sLog.outError("Transport (GUID: %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
                      guidlow, x, y);
        return false;
    }

    Object::_Create(guidlow, 0, HIGHGUID_MO_TRANSPORT);

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(guidlow);

    if (!goinfo)
    {
        sLog.outErrorDb("Transport not created: entry in `gameobject_template` not found, guidlow: %u map: %u  (X: %f Y: %f Z: %f) ang: %f", guidlow, mapid, x, y, z, ang);
        return false;
    }

    m_goInfo = goinfo;

    // initialize waypoints
    m_nextFrame = GetKeyFrames().begin();
    m_currentFrame = m_nextFrame++;

    m_pathProgress = 0;

    SetObjectScale(goinfo->size);

    SetUInt32Value(GAMEOBJECT_FACTION, goinfo->faction);
    // SetUInt32Value(GAMEOBJECT_FLAGS, goinfo->flags);
    SetUInt32Value(GAMEOBJECT_FLAGS, (GO_FLAG_TRANSPORT | GO_FLAG_NODESPAWN));
    SetUInt32Value(GAMEOBJECT_LEVEL, m_period);
    SetEntry(goinfo->id);

    SetDisplayId(goinfo->displayId);
    SetGoState(GO_STATE_READY);

    m_movementStarted = 0;
    SetGoType(GameobjectTypes(goinfo->type));
    SetGoArtKit(0);
    SetGoAnimProgress(animprogress);

    // low part always 0, dynamicHighValue is some kind of progression (not implemented)
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 0, 0);
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 1, dynamicHighValue);

    SetName(goinfo->name);

    GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Gigantic);

    return true;
}

void Transport::MoveToNextWayPoint()
{
    m_currentFrame = m_nextFrame++;
    if (m_nextFrame == GetKeyFrames().end())
        m_nextFrame = GetKeyFrames().begin();
}

void Transport::SpawnPassengers()
{
    uint32 mapId = GetGOInfo()->moTransport.mapID;
    if (!mapId)
        return;

    auto& guids = sObjectMgr.GetDbGuidsForTransport(mapId);
    for (auto& data : guids)
    {
        if (data.first == TYPEID_GAMEOBJECT)
            WorldObject::SpawnGameObject(data.second, GetMap(), 0, this);
        else if (data.first == TYPEID_UNIT)
            WorldObject::SpawnCreature(data.second, GetMap(), 0, this);
    }
}

void Transport::TeleportTransport(uint32 newMapid, float x, float y, float z, float o)
{
    Map* oldMap = GetMap();
    Relocate(x, y, z);

    bool mapChange = GetMapId() != newMapid;

    auto& passengers = GetPassengers();
    for (m_passengerTeleportIterator = passengers.begin(); m_passengerTeleportIterator != passengers.end();)
    {
        WorldObject* obj = (*m_passengerTeleportIterator++);

        Position pos = obj->m_movementInfo.GetTransportPos();

        if (obj->IsUnit())
        {
            Unit* unit = static_cast<Unit*>(obj);
            if (unit->GetVehicleInfo())
            {
                unit->GetVehicleInfo()->TeleportPassengers(newMapid);
            }
        }

        switch (obj->GetTypeId())
        {
            case TYPEID_UNIT:
            {
                if (mapChange)
                {
                    RemovePassenger(obj);
                    if (obj->IsCreature() && !static_cast<Creature*>(obj)->IsPet())
                        obj->AddObjectToRemoveList();
                }
                break;
            }
            case TYPEID_PLAYER:
            {
                Player* player = static_cast<Player*>(obj);
                if (player->IsDead() && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                    player->ResurrectPlayer(1.0);
                player->TeleportTo(newMapid, pos.x, pos.y, pos.z, pos.o, TELE_TO_NOT_LEAVE_TRANSPORT, nullptr, this);
                break;
            }
            case TYPEID_GAMEOBJECT:
            case TYPEID_DYNAMICOBJECT:
            {
                if (mapChange)
                {
                    RemovePassenger(obj);
                    obj->AddObjectToRemoveList();
                }
                break;
            }
        }
    }

    // we need to create and save new Map object with 'newMapid' because if not done -> lead to invalid Map object reference...
    // player far teleport would try to create same instance, but we need it NOW for transport...
    // correct me if I'm wrong O.o
    if (mapChange)
    {
        RemoveModelFromMap();
        oldMap->RemoveTransport(this);
        UpdateForMap(oldMap, false);
        Object::RemoveFromWorld();
        ResetMap();

        Map* newMap = sMapMgr.CreateMap(newMapid, this);
        newMap->GetMessager().AddMessage([transport = this](Map* map)
        {
            transport->SetMap(map);
            transport->Object::AddToWorld();
            map->AddTransport(transport);
            transport->AddModelToMap();
            transport->SpawnPassengers();
            transport->UpdateForMap(map, true);
        });
    }
    else
    {
        UpdateModelPosition();
        UpdatePassengerPositions(m_passengers);
    }
}

bool GenericTransport::AddPassenger(WorldObject* passenger, bool adjustCoords)
{
    if (m_passengers.find(passenger) == m_passengers.end())
    {
        DETAIL_LOG("Unit %s boarded transport %s.", passenger->GetName(), GetName());
        m_passengers.insert(passenger);
        passenger->SetTransport(this);
        passenger->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
        bool changedTransports = passenger->m_movementInfo.t_guid != GetObjectGuid();
        passenger->m_movementInfo.t_guid = GetObjectGuid();
        passenger->m_movementInfo.t_time = GetPathProgress();
        if (changedTransports && adjustCoords)
        {
            passenger->m_movementInfo.t_pos.x = passenger->GetPositionX();
            passenger->m_movementInfo.t_pos.y = passenger->GetPositionY();
            passenger->m_movementInfo.t_pos.z = passenger->GetPositionZ();
            passenger->m_movementInfo.t_pos.o = passenger->GetOrientation();
            CalculatePassengerOffset(passenger->m_movementInfo.t_pos.x, passenger->m_movementInfo.t_pos.y, passenger->m_movementInfo.t_pos.z, &passenger->m_movementInfo.t_pos.o);
        }

        if (passenger->IsUnit())
        {
            Unit* unitPassenger = static_cast<Unit*>(passenger);
            if (Pet* pet = unitPassenger->GetPet())
                AddPetToTransport(unitPassenger, pet);

            if (Pet* miniPet = unitPassenger->GetMiniPet())
                AddPetToTransport(unitPassenger, miniPet);
        }
    }
    return true;
}

bool GenericTransport::RemovePassenger(WorldObject* passenger)
{
    bool erased = false;
    if (m_passengerTeleportIterator != m_passengers.end())
    {
        PassengerSet::iterator itr = m_passengers.find(passenger);
        if (itr != m_passengers.end())
        {
            if (itr == m_passengerTeleportIterator)
                ++m_passengerTeleportIterator;

            m_passengers.erase(itr);
            erased = true;
        }
    }
    else
        erased = m_passengers.erase(passenger) > 0;

    if (erased)
    {
        DETAIL_LOG("Unit %s removed from transport %s.", passenger->GetName(), GetName());
        passenger->SetTransport(nullptr);
        passenger->m_movementInfo.SetTransportData(ObjectGuid(), 0, 0, 0, 0, 0, -1);

        if (passenger->IsUnit())
        {
            Unit* unitPassenger = static_cast<Unit*>(passenger);
            if (Pet* pet = unitPassenger->GetPet())
            {
                RemovePassenger(pet);
                pet->NearTeleportTo(passenger->m_movementInfo.pos.x, passenger->m_movementInfo.pos.y, passenger->m_movementInfo.pos.z, passenger->m_movementInfo.pos.o);
            }

            if (Pet* pet = unitPassenger->GetMiniPet())
            {
                RemovePassenger(pet);
                pet->NearTeleportTo(passenger->m_movementInfo.pos.x, passenger->m_movementInfo.pos.y, passenger->m_movementInfo.pos.z, passenger->m_movementInfo.pos.o);
            }
        }
    }
    return true;
}

bool GenericTransport::AddPetToTransport(Unit* passenger, Pet* pet)
{
    if (AddPassenger(pet))
    {
        pet->m_movementInfo.SetTransportData(GetObjectGuid(), passenger->m_movementInfo.t_pos.x, passenger->m_movementInfo.t_pos.y, passenger->m_movementInfo.t_pos.z, passenger->m_movementInfo.t_pos.o, GetPathProgress(), -1);
        pet->NearTeleportTo(passenger->m_movementInfo.pos.x, passenger->m_movementInfo.pos.y, passenger->m_movementInfo.pos.z, passenger->m_movementInfo.pos.o);
        return true;
    }
    return false;
}

void Transport::Update(const uint32 diff)
{
    uint32 const positionUpdateDelay = 50;
    uint32 const dynChangeTimer = 1000;

    if (GetKeyFrames().size() <= 1)
        return;

    if (AI())
        AI()->UpdateAI(diff);

    uint32 transportDiff = diff;
    if (!GetGOInfo()->moTransport.canBeStopped)
    {
        uint32 currentMsTime = GetMap()->GetCurrentMSTime() - m_movementStarted;
        if (m_pathProgress >= currentMsTime) // map transition and update happened in same tick due to MT
            return;
        transportDiff = currentMsTime - m_pathProgress; // override diff for nonstoppable GOs
        m_pathProgress = currentMsTime;
    }
    else
    {
        if (m_stopped)
            m_pathProgress = std::min(m_pathProgress + diff, (m_pathProgress / GetPeriod()) * GetPeriod() + m_currentFrame->DepartureTime);
        else
            m_pathProgress += diff;
    }

    uint32 pathProgress = m_pathProgress % GetPeriod();
    while (true)
    {
        if (m_currentFrame->IsStopFrame() && GetGOInfo()->moTransport.canBeStopped && m_lastStopIndex != m_currentFrame->Index)
        {
            m_lastStopIndex = m_currentFrame->Index;
            transportDiff = 0;
            m_stopped = true;
            if (pathProgress > m_currentFrame->DepartureTime)
                m_pathProgress = (m_pathProgress / GetPeriod()) * GetPeriod() + m_currentFrame->DepartureTime;
            SetUInt16Value(GAMEOBJECT_DYNAMIC, 0, GO_DYNFLAG_LO_STOPPED);
            SetUInt16Value(GAMEOBJECT_DYNAMIC, 1, pathProgress % GetPeriod());
            m_dynamicChangeTimer.Reset(dynChangeTimer);
            GameObject::SetGoState(GO_STATE_READY);
            if (AI())
                AI()->JustReachedStopPoint();
        }

        if (pathProgress >= m_currentFrame->ArriveTime && pathProgress < m_currentFrame->DepartureTime)
        {
            SetMoving(false);
            break;  // its a stop frame and we are waiting
        }

        if (m_stopped)
            return;

        // not waiting anymore
        SetMoving(true);

        if (pathProgress >= m_currentFrame->DepartureTime && pathProgress < m_currentFrame->NextArriveTime)
            break;  // found current waypoint

        DoEventIfAny(*m_currentFrame->Node, true);

        MoveToNextWayPoint();

        DoEventIfAny(*m_currentFrame->Node, false);

        // first check help in case client-server transport coordinates de-synchronization
        if (m_currentFrame->Node->mapid != GetMapId() || m_currentFrame->IsTeleportFrame())
        {
            TeleportTransport(m_nextFrame->Node->mapid, m_nextFrame->Node->x, m_nextFrame->Node->y, m_nextFrame->Node->z, m_nextFrame->InitialOrientation);
            return;
        }

        if (m_currentFrame == GetKeyFrames().begin())
            DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, " ************ BEGIN ************** %s", GetName());

        DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "%s moved to %f %f %f %d", GetName(), m_currentFrame->Node->x, m_currentFrame->Node->y, m_currentFrame->Node->z, m_currentFrame->Node->mapid);
    }

    // Set position
    m_positionChangeTimer.Update(transportDiff);
    if (m_positionChangeTimer.Passed())
    {
        m_positionChangeTimer.Reset(positionUpdateDelay);
        if (IsMoving() && pathProgress)
        {
            float t = CalculateSegmentPos(float(pathProgress) * 0.001f);
            G3D::Vector3 pos, dir;
            m_currentFrame->Spline->evaluate_percent(m_currentFrame->Index, t, pos);
            m_currentFrame->Spline->evaluate_derivative(m_currentFrame->Index, t, dir);
            UpdatePosition(pos.x, pos.y, pos.z, atan2(dir.y, dir.x) + M_PI);
        }
    }

    m_dynamicChangeTimer.Update(transportDiff);
    if ((GetUInt16Value(GAMEOBJECT_DYNAMIC, 0) & GO_DYNFLAG_LO_STOPPED) == 0 && m_dynamicChangeTimer.Passed())
    {
        m_dynamicChangeTimer.Reset(dynChangeTimer);
        SetUInt16Value(GAMEOBJECT_DYNAMIC, 1, pathProgress % GetPeriod());
    }
}

float Transport::CalculateSegmentPos(float now)
{
    KeyFrame const& frame = *m_currentFrame;
    float const speed = float(m_goInfo->moTransport.moveSpeed);
    float const accel = float(m_goInfo->moTransport.accelRate);
    float timeSinceStop = frame.TimeFrom + (now - (1.0f / IN_MILLISECONDS) * frame.DepartureTime);
    float timeUntilStop = frame.TimeTo - (now - (1.0f / IN_MILLISECONDS) * frame.DepartureTime);
    float segmentPos, dist;
    float accelTime = m_transportTemplate.accelTime;
    float accelDist = m_transportTemplate.accelDist;
    // calculate from nearest stop, less confusing calculation...
    if (timeSinceStop < timeUntilStop)
    {
        if (timeSinceStop < accelTime)
            dist = 0.5f * accel * timeSinceStop * timeSinceStop;
        else
            dist = accelDist + (timeSinceStop - accelTime) * speed;
        segmentPos = dist - frame.DistSinceStop;
    }
    else
    {
        if (timeUntilStop < m_transportTemplate.accelTime)
            dist = 0.5f * accel * timeUntilStop * timeUntilStop;
        else
            dist = accelDist + (timeUntilStop - accelTime) * speed;
        segmentPos = frame.DistUntilStop - dist;
    }

    return segmentPos / frame.NextDistFromPrev;
}

bool ElevatorTransport::Create(uint32 guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang, const QuaternionData& rotation, uint8 animprogress, GOState go_state)
{
    if (GenericTransport::Create(guidlow, name_id, map, phaseMask, x, y, z, ang, rotation, animprogress, go_state))
    {
        m_pathProgress = GetGOInfo()->transport.startOpen ? GetGOInfo()->transport.pause : 0; // these start in the middle of their path
        m_stopped = GetGOInfo()->transport.pause > 0;
        m_animationInfo = sTransportMgr.GetTransportAnimInfo(GetGOInfo()->id);
        m_currentSeg = 0;
        m_eventTriggered = false;
        return true;
    }
    return false;
}

void ElevatorTransport::Update(const uint32 diff)
{
    if (!m_animationInfo)
        return;

    if (!m_stopped)
    {
        uint32 timeSinceLastStop = (GetMap()->GetCurrentMSTime() - m_movementStarted) % m_animationInfo->TotalTime;
        if (!GetGOInfo()->transport.pause)
            m_pathProgress = timeSinceLastStop;
        else
        {
            if (timeSinceLastStop >= GetGOInfo()->transport.pause) // stopped and needs to be reenabled by script
            {
                m_stopped = true;
                timeSinceLastStop = GetGOInfo()->transport.pause;

                if (AI())
                    AI()->JustReachedStopPoint();
            }
            m_pathProgress = ((m_pathProgress / GetGOInfo()->transport.pause) * GetGOInfo()->transport.pause + timeSinceLastStop) % m_animationInfo->TotalTime;
        }
        TransportAnimationEntry const* nodeNext = m_animationInfo->GetNextAnimNode(m_pathProgress);
        TransportAnimationEntry const* nodePrev = m_animationInfo->GetPrevAnimNode(m_pathProgress);
        if (nodeNext && nodePrev)
        {
            m_currentSeg = nodePrev->TimeSeg;

            G3D::Vector3 posPrev = G3D::Vector3(nodePrev->X, nodePrev->Y, nodePrev->Z);
            G3D::Vector3 posNext = G3D::Vector3(nodeNext->X, nodeNext->Y, nodeNext->Z);
            G3D::Vector3 currentPos;
            if (posPrev == posNext)
                currentPos = posPrev;
            else
            {
                uint32 timeElapsed = m_pathProgress - nodePrev->TimeSeg;
                uint32 timeDiff = nodeNext->TimeSeg - nodePrev->TimeSeg;
                G3D::Vector3 segmentDiff = posNext - posPrev;
                float velocityX = float(segmentDiff.x) / timeDiff, velocityY = float(segmentDiff.y) / timeDiff, velocityZ = float(segmentDiff.z) / timeDiff;

                currentPos = G3D::Vector3(timeElapsed * velocityX, timeElapsed * velocityY, timeElapsed * velocityZ);
                currentPos += posPrev;
            }

            TransportRotationEntry const* rotPrev = m_animationInfo->GetPrevRotation(m_pathProgress);
            if (rotPrev)
            {
                G3D::Quat rotation;
                TransportRotationEntry const* rotNext = m_animationInfo->GetNextRotation(m_pathProgress);
                if (rotPrev == rotNext)
                    rotation = G3D::Quat(rotPrev->x, rotPrev->y, rotPrev->z, rotPrev->w);
                else
                {
                    uint32 timeElapsed = m_pathProgress - rotPrev->TimeSeg;
                    uint32 timeDiff = rotNext->TimeSeg - rotPrev->TimeSeg;
                    G3D::Quat quaternionDiff((rotNext->x - rotPrev->x) / timeDiff, (rotNext->y - rotPrev->y) / timeDiff, (rotNext->z - rotPrev->z) / timeDiff, (rotNext->w - rotPrev->w) / timeDiff);
                    rotation = G3D::Quat(rotPrev->x + quaternionDiff.x * timeElapsed, rotPrev->y + quaternionDiff.y * timeElapsed, rotPrev->z + quaternionDiff.z * timeElapsed, rotPrev->w + quaternionDiff.w * timeElapsed);
                }

                SetOrientation(std::asin(rotation.z) * 2);

                SetLocalRotation(rotation.x, rotation.y, rotation.z, rotation.w);
            }

            G3D::Quat transportPathRotation(GetFloatValue(GAMEOBJECT_PARENTROTATION + 0), GetFloatValue(GAMEOBJECT_PARENTROTATION + 1), GetFloatValue(GAMEOBJECT_PARENTROTATION + 2), GetFloatValue(GAMEOBJECT_PARENTROTATION + 3));
            currentPos = currentPos * transportPathRotation;

            currentPos += G3D::Vector3(m_stationaryPosition.x, m_stationaryPosition.y, m_stationaryPosition.z);

            GetMap()->GameObjectRelocation(this, currentPos.x, currentPos.y, currentPos.z, GetOrientation());
            // SummonCreature(1, currentPos.x, currentPos.y, currentPos.z, GetOrientation(), TEMPSPAWN_TIMED_DESPAWN, 5000);

            UpdatePassengerPositions(GetPassengers());

            if (!m_eventTriggered && (GetGOInfo()->transport.pause1EventID || GetGOInfo()->transport.pause2EventID))
            {
                uint32 eventId = 0;
                switch (GetGOInfo()->id)
                {
                    case 194675: // Ulduar Tram
                    {
                        if (nodePrev->id == 179512)
                            eventId = GetGOInfo()->transport.pause1EventID;
                        else if (nodePrev->id == 179620)
                            eventId = GetGOInfo()->transport.pause2EventID;
                        break;
                    }
                    default:
                        break;
                }
                if (eventId)
                {
                    m_eventTriggered = true;
                    StartEvents_Event(GetMap(), eventId, this, this, true);
                }
            }
        }
    }

    if (AI())
        AI()->UpdateAI(diff);
}

void GenericTransport::UpdatePosition(float x, float y, float z, float o)
{
    Relocate(x, y, z, o);
    UpdateModelPosition();

    UpdatePassengerPositions(m_passengers);
}

void GenericTransport::SetGoState(GOState state)
{
    GameObject::SetGoState(state);
    if ((GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT && GetGOInfo()->transport.pause) || (GetGOInfo()->type == GAMEOBJECT_TYPE_MO_TRANSPORT && GetGOInfo()->moTransport.canBeStopped))
        m_movementStarted = GetMap()->GetCurrentMSTime();
    m_stopped = false;
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 0, 0);
}

void ElevatorTransport::SetGoState(GOState state)
{
    GenericTransport::SetGoState(state);
    m_eventTriggered = false;
}

void GenericTransport::UpdatePassengerPositions(PassengerSet& passengers)
{
    for (const auto passenger : passengers)
        UpdatePassengerPosition(passenger);
}

void GenericTransport::UpdatePassengerPosition(WorldObject* passenger)
{
    // transport teleported but passenger not yet (can happen for players)
    if (passenger->IsInWorld() && passenger->GetMap() != GetMap())
        return;

    // Do not use Unit::UpdatePosition here, we don't want to remove auras
    // as if regular movement occurred
    float x, y, z, o;
    x = passenger->GetTransOffsetX();
    y = passenger->GetTransOffsetY();
    z = passenger->GetTransOffsetZ();
    o = passenger->GetTransOffsetO();
    CalculatePassengerPosition(x, y, z, &o);
    if (!MaNGOS::IsValidMapCoord(x, y, z))
    {
        sLog.outError("[TRANSPORTS] Object %s [guid %u] has invalid position on transport.", passenger->GetName(), passenger->GetGUIDLow());
        return;
    }
    switch (passenger->GetTypeId())
    {
        case TYPEID_UNIT:
        {
            Creature* creature = dynamic_cast<Creature*>(passenger);
            if (passenger->IsInWorld())
                GetMap()->CreatureRelocation(creature, x, y, z, o);
            else
                passenger->Relocate(x, y, z, o);
            creature->m_movementInfo.t_time = GetPathProgress();
            break;
        }
        case TYPEID_PLAYER:
            //relocate only passengers in world and skip any player that might be still logging in/teleporting
            if (passenger->IsInWorld())
                GetMap()->PlayerRelocation(dynamic_cast<Player*>(passenger), x, y, z, o);
            else
            {
                passenger->Relocate(x, y, z, o);
                dynamic_cast<Player*>(passenger)->m_movementInfo.t_guid = GetObjectGuid();
            }
            dynamic_cast<Player*>(passenger)->m_movementInfo.t_time = GetPathProgress();
            break;
        case TYPEID_GAMEOBJECT:
            GetMap()->GameObjectRelocation(static_cast<GameObject*>(passenger), x, y, z, o, false);
            break;
        case TYPEID_DYNAMICOBJECT:
            GetMap()->DynamicObjectRelocation(static_cast<DynamicObject*>(passenger), x, y, z, o);
            break;
        default:
            break;
    }
}

void GenericTransport::CalculatePassengerOrientation(float& o) const
{
    o = MapManager::NormalizeOrientation(GetOrientation() + o);
}

void GenericTransport::CalculatePassengerPosition(float& x, float& y, float& z, float* o, float transX, float transY, float transZ, float transO)
{
    float inx = x, iny = y, inz = z;
    if (o)
        *o = MapManager::NormalizeOrientation(transO + *o);

    x = transX + inx * std::cos(transO) - iny * std::sin(transO);
    y = transY + iny * std::cos(transO) + inx * std::sin(transO);
    z = transZ + inz;
}

void GenericTransport::CalculatePassengerOffset(float& x, float& y, float& z, float* o, float transX, float transY, float transZ, float transO)
{
    if (o)
        *o = MapManager::NormalizeOrientation(*o - transO);

    z -= transZ;
    y -= transY;    // y = searchedY * std::cos(o) + searchedX * std::sin(o)
    x -= transX;    // x = searchedX * std::cos(o) + searchedY * std::sin(o + pi)
    float inx = x, iny = y;
    y = (iny - inx * std::tan(transO)) / (std::cos(transO) + std::sin(transO) * std::tan(transO));
    x = (inx + iny * std::tan(transO)) / (std::cos(transO) + std::sin(transO) * std::tan(transO));
}

void Transport::UpdateForMap(Map const* targetMap, bool newMap)
{
    Map::PlayerList const& pl = targetMap->GetPlayers();
    if (pl.isEmpty())
        return;

    if (newMap)
    {
        for (const auto& itr : pl)
        {
            Player* player = itr.getSource();
            if (this != player->GetTransport())
            {
                UpdateData updateData;
                BuildCreateUpdateBlockForPlayer(&updateData, player);
                WorldPacket packet = updateData.BuildPacket(0); // always only one packet
                player->SendDirectMessage(packet);
                player->AddAtClient(this);
            }
        }
    }
    else
    {
        UpdateData updateData;
        BuildOutOfRangeUpdateBlock(&updateData);
        WorldPacket packet = updateData.BuildPacket(0); // always only one packet
        for (const auto& itr : pl)
        {
            Player* player = itr.getSource();
            if (this != player->GetTransport())
            {
                player->SendDirectMessage(packet);
                player->RemoveAtClient(this);
            }
        }
    }
}

void Transport::DoEventIfAny(TaxiPathNodeEntry const& node, bool departure)
{
    if (uint32 eventid = departure ? node.departureEventID : node.arrivalEventID)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "Taxi %s event %u of node %u of %s \"%s\") path", departure ? "departure" : "arrival", eventid, node.index, GetGuidStr().c_str(), GetName());

        StartEvents_Event(GetMap(), eventid, this, this, departure);
    }
}

TransportTemplate::~TransportTemplate()
{
    // Collect shared pointers into a set to avoid deleting the same memory more than once
    std::set<TransportSpline*> splines;
    for (const auto& keyFrame : keyFrames)
        splines.insert(keyFrame.Spline);

    for (const auto& spline : splines)
        delete spline;
}
