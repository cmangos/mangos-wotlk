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
#include "Server/WorldPacket.h"
#include "Server/WorldSession.h"
#include "Server/Opcodes.h"
#include "Log.h"
#include "Entities/Player.h"
#include "Movement/MoveSpline.h"
#include "Maps/MapManager.h"
#include "Entities/Transports.h"
#include "BattleGround/BattleGround.h"
#include "MotionGenerators/WaypointMovementGenerator.h"
#include "Maps/MapPersistentStateMgr.h"
#include "Globals/ObjectMgr.h"
#include "Entities/Vehicle.h"
#include "World/World.h"
#include "Anticheat/Anticheat.hpp"

#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#define MOVEMENT_PACKET_TIME_DELAY 0

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_LOG("WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    // get start teleport coordinates (will used later in fail case)
    WorldLocation old_loc;
    GetPlayer()->GetPosition(old_loc);

    // get the teleport destination
    WorldLocation& loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check (only cheating case possible)
    if (!MapManager::IsValidMapCoord(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation))
    {
        sLog.outError("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far to a not valid location "
                      "(map:%u, x:%f, y:%f, z:%f) We port him to his homebind instead..",
                      GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);
        // stop teleportation else we would try this again and again in LogoutPlayer...
        GetPlayer()->SetSemaphoreTeleportFar(false);
        // and teleport the player to a valid place
        GetPlayer()->TeleportToHomebind();
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.mapid);
    
    auto returnHomeFunc = [this, player = GetPlayer(), old_loc, loc]()
    {
        Map* map = nullptr;
        // must have map in teleport
        if (!map)
            map = sMapMgr.CreateMap(loc.mapid, player);
        if (!map)
            map = sMapMgr.CreateMap(old_loc.mapid, player);

        if (!map)
        {
            KickPlayer();
            return;
        }

        player->SetMap(map);

        player->SetSemaphoreTeleportFar(false);

        // Teleport to previous place, if cannot be ported back TP to homebind place
        if (!player->TeleportTo(old_loc))
        {
            DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s cannot be ported to his previous place, teleporting him to his homebind place...",
                player->GetGuidStr().c_str());
            player->TeleportToHomebind();
        }
    };

    Map* map = nullptr;

    // prevent crash at attempt landing to not existed battleground instance
    if (mEntry->IsBattleGroundOrArena())
    {
        if (GetPlayer()->GetBattleGroundId())
            map = sMapMgr.FindMap(loc.mapid, GetPlayer()->GetBattleGroundId());

        if (!map)
        {
            DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far to nonexisten battleground instance "
                       " (map:%u, x:%f, y:%f, z:%f) Trying to port him to his previous place..",
                       GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);

            returnHomeFunc();
            return;
        }
    }

    Difficulty diff = GetPlayer()->GetGroup() ? GetPlayer()->GetGroup()->GetDifficulty(mEntry->IsRaid()) : GetPlayer()->GetDifficulty(mEntry->IsRaid());
    uint32 miscRequirement = 0;
    if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(loc.mapid))
    {
        if (AREA_LOCKSTATUS_OK != GetPlayer()->GetAreaTriggerLockStatus(at, diff, miscRequirement))
        {
            returnHomeFunc();
            return;
        }
    }

    InstanceTemplate const* mInstance = ObjectMgr::GetInstanceTemplate(loc.mapid);

    // reset instance validity, except if going to an instance inside an instance
    if (!GetPlayer()->m_InstanceValid && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    // relocate the player to the teleport destination
    if (!map)
        map = sMapMgr.CreateMap(loc.mapid, GetPlayer());

    GetPlayer()->SetMap(map);

    // must be in wotlk before AlterTeleportLocation - TODO: Refactor BG entering so that BG script can alter transport movement info on bg enter and
    // change phase mask without interrupt flag removing it
    // SOTA must already send player inside bg on ship and in already correct phase mask else player falls through the ship on SOTA entry when attacker
    _player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_WORLD);

    if (_player->InBattleGround())
        if (BattleGround* bg = _player->GetBattleGround())
            if (_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                bg->AlterTeleportLocation(GetPlayer(), GetPlayer()->m_teleportTransport, loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation);

    bool found = true;
    if (GetPlayer()->m_teleportTransport)
    {
        found = false;
        GetPlayer()->m_movementInfo.t_pos = Position(loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation); // when teleporting onto transport, position is local coords
        if (GenericTransport* transport = map->GetTransport(GetPlayer()->m_teleportTransport))
            if (transport->GetMapId() == loc.mapid)
                found = true;
    }
    else
        GetPlayer()->Relocate(loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation);
    auto lambda = [this, loc, old_loc, mEntry, mInstance](Map* map)
    {
        if (GenericTransport* transport = map->GetTransport(GetPlayer()->m_teleportTransport))
        {
            if (transport->GetMapId() == loc.mapid)
            {
                transport->AddPassenger(GetPlayer(), false);
                transport->UpdatePassengerPosition(GetPlayer());
                if (GetPlayer()->IsVehicle()) // when teleporting onto transport, protocol doesnt support summoning accessories
                    GetPlayer()->GetVehicleInfo()->DisableAccessoryInit();
            }
        }
        GetPlayer()->m_teleportTransport = ObjectGuid();

        GetPlayer()->SendInitialPacketsBeforeAddToMap();
        // the CanEnter checks are done in TeleporTo but conditions may change
        // while the player is in transit, for example the map may get full
        if (!GetPlayer()->GetMap()->Add(GetPlayer()))
        {
            // if player wasn't added to map, reset his map pointer!
            GetPlayer()->ResetMap();

            DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far but couldn't be added to map "
                " (map:%u, x:%f, y:%f, z:%f) Trying to port him to his previous place..",
                GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);

            // Teleport to previous place, if cannot be ported back TP to homebind place
            if (!GetPlayer()->TeleportTo(old_loc))
            {
                DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s cannot be ported to his previous place, teleporting him to his homebind place...",
                    GetPlayer()->GetGuidStr().c_str());
                GetPlayer()->TeleportToHomebind();
            }
            return;
        }

        // battleground state prepare (in case join to BG), at relogin/tele player not invited
        // only add to bg group and object, if the player was invited (else he entered through command)
        if (_player->InBattleGround())
        {
            // cleanup setting if outdated
            if (!mEntry->IsBattleGroundOrArena())
            {
                // We're not in BG
                _player->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);
                // reset destination bg team
                _player->SetBGTeam(TEAM_NONE);
            }
            // join to bg case
            else if (BattleGround* bg = _player->GetBattleGround())
            {
                if (_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                    bg->AddPlayer(_player);
            }
        }

        m_anticheat->Teleport({ loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation });

        GetPlayer()->SendInitialPacketsAfterAddToMap();

        // flight fast teleport case
        if (_player->InBattleGround())
            _player->TaxiFlightInterrupt(false);
        else
            _player->TaxiFlightResume();

        if (mInstance)
        {
            // mount allow check
            if (!mInstance->mountAllowed)
                _player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }

        // honorless target
        if (GetPlayer()->pvpInfo.inPvPEnforcedArea)
            GetPlayer()->CastSpell(GetPlayer(), 2479, TRIGGERED_OLD_TRIGGERED);

        _player->ResummonPetTemporaryUnSummonedIfAny();

        // lets process all delayed operations on successful teleport
        GetPlayer()->ProcessDelayedOperations();

        // notify group after successful teleport
        if (_player->GetGroup())
            _player->SetGroupUpdateFlag(GROUP_UPDATE_FULL);
    };
    if (found)
        lambda(map);
    else
        map->GetMessager().AddMessage(lambda);
}

void WorldSession::HandleMoveTeleportAckOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("MSG_MOVE_TELEPORT_ACK");

    ObjectGuid guid;

    recv_data >> guid.ReadAsPacked();

    uint32 counter, time;
    recv_data >> counter >> time;
    DEBUG_LOG("Guid: %s", guid.GetString().c_str());
    DEBUG_LOG("Counter %u, time %u", counter, time / IN_MILLISECONDS);

    Unit* mover = _player->GetMover();
    Player* plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : nullptr;

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetObjectGuid())
        return;

    m_anticheat->OrderAck(recv_data.GetOpcode(), counter);

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->SetDelayedZoneUpdate(false, 0);

    plMover->SetPosition(dest.coord_x, dest.coord_y, dest.coord_z, dest.orientation, true);

    plMover->SetFallInformation(0, dest.coord_z);

    GenericTransport* currentTransport = nullptr;
    if (plMover->m_teleportTransport)
        currentTransport = plMover->GetMap()->GetTransport(plMover->m_teleportTransport);
    if (currentTransport)
        currentTransport->AddPassenger(plMover);
    plMover->m_teleportTransport = ObjectGuid();

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (plMover->pvpInfo.inPvPEnforcedArea)
            plMover->CastSpell(plMover, 2479, TRIGGERED_OLD_TRIGGERED);
    }

    m_anticheat->Teleport({ dest.coord_x, dest.coord_y, dest.coord_z, dest.orientation });

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    // lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket& recv_data)
{
    Opcodes opcode = recv_data.GetOpcode();
    if (!sLog.HasLogFilter(LOG_FILTER_PLAYER_MOVES))
    {
        DEBUG_LOG("WORLD: Received opcode %s (%u, 0x%X)", LookupOpcodeName(opcode), opcode, opcode);
        recv_data.hexlike();
    }

    Unit* mover = _player->GetMover();
    Player* plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : nullptr;

    /* extract packet */
    ObjectGuid guid;
    MovementInfo movementInfo;

    recv_data >> guid.ReadAsPacked();
    recv_data >> movementInfo;
    /*----------------*/

    if (opcode == CMSG_MOVE_CHNG_TRANSPORT) // TODO: Should mark down pending transport change for anticheat purpose
    {
        if (guid != mover->GetObjectGuid() && _player->GetObjectGuid() == guid)
            mover = _player;
    }

    if (!ProcessMovementInfo(movementInfo, mover, plMover, recv_data))
        return;

    // CMSG opcode has no handler in client, should not be sent to others.
    // It is sent by client when you jump and hit something on the way up,
    // thus stopping upward movement and causing you to descend sooner.
    if (opcode == CMSG_MOVE_FALL_RESET)
        return;

    WorldPacket data(opcode, recv_data.size());
    data << mover->GetPackGUID();             // write guid
    movementInfo.Write(data);                               // write data
    mover->SendMessageToSetExcept(data, _player);
}

void WorldSession::HandleForceSpeedChangeAckOpcodes(WorldPacket& recv_data)
{
    Opcodes opcode = recv_data.GetOpcode();
    DEBUG_LOG("WORLD: Received %s (%u, 0x%X) opcode", recv_data.GetOpcodeName(), opcode, opcode);

    /* extract packet */
    ObjectGuid guid;
    uint32 counter;
    MovementInfo movementInfo;
    float  newspeed;

    recv_data >> guid.ReadAsPacked();
    recv_data >> counter;                                   // counter or moveEvent
    recv_data >> movementInfo;
    recv_data >> newspeed;

    Unit* mover = _player->GetMover();

    if (!ProcessMovementInfo(movementInfo, mover, _player, recv_data))
        return;

    m_anticheat->OrderAck(opcode, counter);

    if (opcode == CMSG_MOVE_SET_COLLISION_HGT_ACK)
    {
        WorldPacket data(MSG_MOVE_SET_COLLISION_HGT, 18);
        data << guid.WriteAsPacked();
        data << movementInfo;
        data << newspeed; // new collision height
        mover->SendMessageToSetExcept(data, _player);
        return;
    }

    /*----------------*/

    // Process anticheat checks, remember client-side speed ...
    if (_player->IsSelfMover() && !m_anticheat->SpeedChangeAck(movementInfo, recv_data, newspeed))
        return;

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack", "PitchRate" };

    switch (opcode)
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_FORCE_PITCH_RATE_CHANGE_ACK:          move_type = MOVE_PITCH_RATE;    force_move_type = MOVE_PITCH_RATE;  break;
        default:
            sLog.outError("WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    const SpeedOpcodePair& speedOpcodes = SetSpeed2Opc_table[move_type];
    WorldPacket data(speedOpcodes[2], 18);
    data << guid.WriteAsPacked();
    data << movementInfo;
    data << newspeed;
    mover->SendMessageToSetExcept(data, _player);

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if (_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(mover->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (mover->GetSpeed(move_type) > newspeed)        // must be greater - just correct
        {
            sLog.outError("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                          move_type_name[move_type], _player->GetName(), mover->GetSpeed(move_type), newspeed);
            mover->SetSpeedRate(move_type, _player->GetSpeedRate(move_type), true);
        }
        else                                                // must be lesser - cheating
        {
            BASIC_LOG("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                      _player->GetName(), _player->GetSession()->GetAccountId(), mover->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_SET_ACTIVE_MOVER");
    recv_data.hexlike();

    ObjectGuid guid;
    recv_data >> guid;

    m_anticheat->Movement(_player->GetMover()->m_movementInfo, recv_data);

    if (_player->GetMover()->GetObjectGuid() != guid)
    {
        sLog.outError("HandleSetActiveMoverOpcode: incorrect mover guid: mover is %s and should be %s",
                      _player->GetMover()->GetGuidStr().c_str(), guid.GetString().c_str());
    }
}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_MOVE_NOT_ACTIVE_MOVER");
    recv_data.hexlike();

    ObjectGuid old_mover_guid;
    MovementInfo mi;

    recv_data >> old_mover_guid.ReadAsPacked();
    recv_data >> mi;

    if (_player->IsClientControlled() && _player->GetMover() && _player->GetMover()->GetObjectGuid() == old_mover_guid)
    {
        sLog.outError("HandleMoveNotActiveMover: incorrect mover guid: mover is %s and should be %s instead of %s",
                      _player->GetMover()->GetGuidStr().c_str(),
                      _player->GetGuidStr().c_str(),
                      old_mover_guid.GetString().c_str());
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    if (!_player->IsTaxiFlying())
        _player->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    // DEBUG_LOG("WORLD: Received opcode CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << GetPlayer()->GetObjectGuid();

    GetPlayer()->SendMessageToSet(data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_MOVE_KNOCK_BACK_ACK");

    Unit* mover = _player->GetMover();

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (mover->IsPlayer() && static_cast<Player*>(mover)->IsBeingTeleported())
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    ObjectGuid guid;
    uint32 counter;
    MovementInfo movementInfo;

    recv_data >> guid.ReadAsPacked();
    recv_data >> counter;                                   // knockback packets counter
    recv_data >> movementInfo;

    m_anticheat->OrderAck(recv_data.GetOpcode(), counter);

    if (!ProcessMovementInfo(movementInfo, mover, _player, recv_data))
        return;

    if (mover->IsPlayer() && static_cast<Player*>(mover)->IsFreeFlying())
        mover->SetCanFly(true);

    WorldPacket data(MSG_MOVE_KNOCK_BACK, recv_data.size() + 15);
    data << mover->GetPackGUID();
    data << movementInfo;
    data << movementInfo.jump.cosAngle;
    data << movementInfo.jump.sinAngle;
    data << movementInfo.jump.xyspeed;
    data << movementInfo.jump.zspeed;
    mover->SendMessageToSetExcept(data, _player);
}

void WorldSession::SendKnockBack(Unit* who, float angle, float horizontalSpeed, float verticalSpeed)
{
    GetPlayer()->SetLaunched(true);
    float vsin = sin(angle);
    float vcos = cos(angle);

    WorldPacket data(SMSG_MOVE_KNOCK_BACK, 9 + 4 + 4 + 4 + 4 + 4);
    data << who->GetPackGUID();
    data << GetOrderCounter();
    data << float(vcos);                                // x direction
    data << float(vsin);                                // y direction
    data << float(horizontalSpeed);                     // Horizontal speed
    data << float(-verticalSpeed);                      // Z Movement speed (vertical)
    SendPacket(data);
    IncrementOrderCounter();

    m_anticheat->KnockBack(horizontalSpeed, -verticalSpeed, vcos, vsin);
}

void WorldSession::HandleMoveFlagChangeOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("%s", recv_data.GetOpcodeName());

    ObjectGuid guid;
    uint32 counter;
    MovementInfo movementInfo;
    uint32 isApplied;

    recv_data >> guid.ReadAsPacked();
    recv_data >> counter;
    recv_data >> movementInfo;
    recv_data >> isApplied;

    m_anticheat->OrderAck(recv_data.GetOpcode(), counter);

    Unit* mover = _player->GetMover();

    if (!ProcessMovementInfo(movementInfo, mover, _player, recv_data))
        return;

    Opcodes response = MSG_NULL_ACTION;

    switch (recv_data.GetOpcode())
    {
        case CMSG_MOVE_HOVER_ACK: response = MSG_MOVE_HOVER; break;
        case CMSG_MOVE_FEATHER_FALL_ACK: response = MSG_MOVE_FEATHER_FALL; break;
        case CMSG_MOVE_WATER_WALK_ACK: response = MSG_MOVE_WATER_WALK; break;
        case CMSG_MOVE_SET_CAN_FLY_ACK: response = MSG_MOVE_UPDATE_CAN_FLY; break;
        default: break;
    }

    WorldPacket data(response, 8);
    data << guid.WriteAsPacked();
    data << movementInfo;
    mover->SendMessageToSetExcept(data, _player);
}

void WorldSession::HandleMoveRootAck(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode %s", recv_data.GetOpcodeName());
    // Pre-Wrath: broadcast root
    ObjectGuid guid;
    uint32 counter;
    MovementInfo movementInfo;
    recv_data >> guid.ReadAsPacked();
    recv_data >> counter;
    recv_data >> movementInfo;

    m_anticheat->OrderAck(recv_data.GetOpcode(), counter);

    Unit* mover = _player->GetMover();

    if (mover->GetObjectGuid() != guid)
        return;

    if (recv_data.GetOpcode() == CMSG_FORCE_MOVE_UNROOT_ACK) // unroot case
    {
        if (!mover->m_movementInfo.HasMovementFlag(MOVEFLAG_ROOT))
            return;
    }
    else // root case
    {
        if (mover->m_movementInfo.HasMovementFlag(MOVEFLAG_ROOT))
            return;
    }

    if (!ProcessMovementInfo(movementInfo, mover, _player, recv_data))
        return;

    WorldPacket data(recv_data.GetOpcode() == CMSG_FORCE_MOVE_UNROOT_ACK ? MSG_MOVE_UNROOT : MSG_MOVE_ROOT);
    data << guid.WriteAsPacked();
    data << movementInfo;
    mover->SendMessageToSetExcept(data, _player);
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!_player->IsAlive() || _player->IsInCombat())
        return;

    ObjectGuid summonerGuid;
    bool agree;
    recv_data >> summonerGuid;
    recv_data >> agree;

    _player->SummonIfPossible(agree, summonerGuid);
}

bool WorldSession::VerifyMovementInfo(MovementInfo const& movementInfo, Unit* mover, bool unroot) const
{
    // ignore wrong guid (player attempt cheating own session for not own guid possible...)
    if (mover->GetObjectGuid() != _player->GetMover()->GetObjectGuid())
        return false;

    if (!MaNGOS::IsValidMapCoord(movementInfo.GetPos().x, movementInfo.GetPos().y, movementInfo.GetPos().z, movementInfo.GetPos().o))
        return false;

    // rooted mover sent packet without root or moving AND root - ignore, due to client crash possibility
    if (!unroot)
        if (mover->IsRooted() && (!movementInfo.HasMovementFlag(MOVEFLAG_ROOT) || movementInfo.HasMovementFlag(MOVEFLAG_MASK_MOVING)))
            return false;

    if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        // transports size limited
        // (also received at zeppelin/lift leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (movementInfo.GetTransportPos().x > 50 || movementInfo.GetTransportPos().y > 50 || movementInfo.GetTransportPos().z > 100)
            return false;

        if (!MaNGOS::IsValidMapCoord(movementInfo.GetPos().x + movementInfo.GetTransportPos().x, movementInfo.GetPos().y + movementInfo.GetTransportPos().y,
                                     movementInfo.GetPos().z + movementInfo.GetTransportPos().z, movementInfo.GetPos().o + movementInfo.GetTransportPos().o))
        {
            return false;
        }
    }

    return true;
}

void WorldSession::HandleMoverRelocation(MovementInfo& movementInfo)
{
    SynchronizeMovement(movementInfo);

    Unit* mover = _player->GetMover();

    if (Player* plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : nullptr)
    {
        plMover->m_movementInfo = movementInfo;
        if (plMover->m_movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
        {
            if (!plMover->m_transport)
                if (GenericTransport* transport = plMover->GetMap()->GetTransport(movementInfo.GetTransportGuid()))
                    transport->AddPassenger(plMover);
        }
        else if (plMover->m_transport)               // if we were on a transport, leave
        {
            plMover->m_transport->RemovePassenger(plMover);
            plMover->m_transport = nullptr;
            plMover->m_movementInfo.ClearTransportData();
        }

        plMover->SetPosition(movementInfo.GetPos().x, movementInfo.GetPos().y, movementInfo.GetPos().z, movementInfo.GetPos().o);

        if (movementInfo.GetPos().z < -500.0f)
        {
            if (plMover->GetBattleGround()
                    && plMover->GetBattleGround()->HandlePlayerUnderMap(_player))
            {
                // do nothing, the handle already did if returned true
            }
            else
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                // TODO: discard movement packets after the player is rooted
                if (plMover->IsAlive())
                {
                    plMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, plMover->GetMaxHealth());
                    // pl can be alive if GM/etc
                    if (!plMover->IsAlive())
                    {
                        // change the death state to CORPSE to prevent the death timer from
                        // starting in the next player update
                        plMover->KillPlayer();
                        plMover->BuildPlayerRepop();
                    }
                }

                // cancel the death timer here if started
                plMover->RepopAtGraveyard();
            }
        }
    }
    else                                                    // creature charmed
    {
        if (mover->IsInWorld())
            mover->GetMap()->CreatureRelocation((Creature*)mover, movementInfo.GetPos().x, movementInfo.GetPos().y, movementInfo.GetPos().z, movementInfo.GetPos().o);
    }
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket& recv_data)
{
    /*  WorldSession::Update( WorldTimer::getMSTime() );*/
    DEBUG_LOG("WORLD: Received opcode CMSG_MOVE_TIME_SKIPPED");

    ObjectGuid guid;
    uint32 timeSkipped;
    recv_data >> guid.ReadAsPacked();
    recv_data >> timeSkipped;

    Unit* mover = _player->GetMover();

    // Ignore updates not for current player
    if (mover == nullptr || guid != mover->GetObjectGuid())
        return;

    mover->m_movementInfo.stime += timeSkipped;
    mover->m_movementInfo.ctime += timeSkipped;
    m_anticheat->TimeSkipped(guid, timeSkipped);

    // Send to other players
    WorldPacket data(MSG_MOVE_TIME_SKIPPED, 16);
    data << mover->GetPackGUID();
    data << timeSkipped;
    mover->SendMessageToSetExcept(data, _player);
}

bool WorldSession::ProcessMovementInfo(MovementInfo& movementInfo, Unit* mover, Player* plMover, WorldPacket& recv_data)
{
    if (plMover && plMover->IsBeingTeleported())
        return false;

    if (!VerifyMovementInfo(movementInfo, mover, recv_data.GetOpcode() == CMSG_FORCE_MOVE_UNROOT_ACK))
        return false;

    // TODO: if root becomes problem during spline again - recheck sniffs
    if (!mover->movespline->Finalized())
    {
        if (!mover->movespline->IsBoarding() || (recv_data.GetOpcode() != CMSG_FORCE_MOVE_UNROOT_ACK && recv_data.GetOpcode() != CMSG_FORCE_MOVE_ROOT_ACK))
			return false;
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (recv_data.GetOpcode() == MSG_MOVE_FALL_LAND && plMover && !plMover->IsTaxiFlying())
        plMover->HandleFall(movementInfo);

    if (!m_anticheat->Movement(movementInfo, recv_data))
        return false;

    if (movementInfo.GetMovementFlags() & MOVEFLAG_MASK_MOVING_OR_TURN)
    {
        if (mover->IsStandUpOnMovementState())
            mover->SetStandState(UNIT_STAND_STATE_STAND);
        mover->HandleEmoteState(0);
    }

    /* process position-change */
    HandleMoverRelocation(movementInfo);

    // just landed from a knockback? update status
    if (plMover && plMover->IsLaunched() && (recv_data.GetOpcode() == MSG_MOVE_FALL_LAND || recv_data.GetOpcode() == MSG_MOVE_START_SWIM))
        plMover->SetLaunched(false);

    if (plMover && recv_data.GetOpcode() != CMSG_MOVE_KNOCK_BACK_ACK)
        plMover->UpdateFallInformationIfNeed(movementInfo, recv_data.GetOpcode());

    return true;
}

void WorldSession::HandleTimeSyncResp(WorldPacket& recvData)
{
    DEBUG_LOG("CMSG_TIME_SYNC_RESP");

    uint32 counter, clientTimestamp;
    recvData >> counter >> clientTimestamp;

    if (m_pendingTimeSyncRequests.count(counter) == 0)
        return;

    uint32 serverTimeAtSent = m_pendingTimeSyncRequests.at(counter);
    m_pendingTimeSyncRequests.erase(counter);

    // time it took for the request to travel to the client, for the client to process it and reply and for response to travel back to the server.
    // we are going to make 2 assumptions:
    // 1) we assume that the request processing time equals 0.
    // 2) we assume that the packet took as much time to travel from server to client than it took to travel from client to server.
    uint32 roundTripDuration = WorldTimer::getMSTimeDiff(serverTimeAtSent, recvData.GetReceivedTime());
    uint32 lagDelay = roundTripDuration / 2;

    /*
    clockDelta = serverTime - clientTime
    where
    serverTime: time that was displayed on the clock of the SERVER at the moment when the client processed the SMSG_TIME_SYNC_REQUEST packet.
    clientTime:  time that was displayed on the clock of the CLIENT at the moment when the client processed the SMSG_TIME_SYNC_REQUEST packet.
    Once clockDelta has been computed, we can compute the time of an event on server clock when we know the time of that same event on the client clock,
    using the following relation:
    serverTime = clockDelta + clientTime
    */
    int64 clockDelta = (int64)(serverTimeAtSent + lagDelay) - (int64)clientTimestamp;
    m_timeSyncClockDeltaQueue.push_back(std::pair<int64, uint32>(clockDelta, roundTripDuration));
    ComputeNewClockDelta();
}

void WorldSession::ComputeNewClockDelta()
{
    // implementation of the technique described here: https://web.archive.org/web/20180430214420/http://www.mine-control.com/zack/timesync/timesync.html
    // to reduce the skew induced by dropped TCP packets that get resent.

    using namespace boost::accumulators;

    accumulator_set<uint32, features<tag::mean, tag::median, tag::variance(lazy)> > latencyAccumulator;

    for (auto pair : m_timeSyncClockDeltaQueue)
        latencyAccumulator(pair.second);

    uint32 latencyMedian = static_cast<uint32>(std::round(median(latencyAccumulator)));
    uint32 latencyStandardDeviation = static_cast<uint32>(std::round(sqrt(variance(latencyAccumulator))));

    accumulator_set<int64, features<tag::mean> > clockDeltasAfterFiltering;
    uint32 sampleSizeAfterFiltering = 0;
    for (auto pair : m_timeSyncClockDeltaQueue)
    {
        if (pair.second < latencyStandardDeviation + latencyMedian)
        {
            clockDeltasAfterFiltering(pair.first);
            sampleSizeAfterFiltering++;
        }
    }

    if (sampleSizeAfterFiltering != 0)
    {
        int64 meanClockDelta = static_cast<int64>(std::round(mean(clockDeltasAfterFiltering)));
        if (std::abs(meanClockDelta - m_timeSyncClockDelta) > 25)
            m_timeSyncClockDelta = meanClockDelta;
    }
    else if (m_timeSyncClockDelta == 0)
    {
        std::pair<int64, uint32> back = m_timeSyncClockDeltaQueue.back();
        m_timeSyncClockDelta = back.first;
    }
}
