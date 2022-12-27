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
#include "Server/WorldSession.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "Battlefield/Battlefield.h"
#include "Server/WorldPacket.h"
#include "Log.h"
#include "Server/Opcodes.h"
#include "Entities/Player.h"

/**
Invite a player to join and teleport to a battlefield

@param   battlefield id
@param   zone id for the battlefield
@param   time delay to accept the invitation
*/
void WorldSession::SendBattlefieldWarInvite(uint32 battlefieldId, uint32 zoneId, uint32 acceptDelay)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_ENTRY_INVITE, 12);
    data << uint32(battlefieldId);
    data << uint32(zoneId);
    data << uint32(time(nullptr) + acceptDelay);

    SendPacket(data);
}

/**
Invite a player to queue for a battlefield (when entering zone or by battlemaster)

@param   battlefield id
*/
void WorldSession::SendBattlefieldQueueInvite(uint32 battlefieldId)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_QUEUE_INVITE, 5);

    data << uint32(battlefieldId);
    data << uint8(1);                                       // warmup ? used ?

    SendPacket(data);
}

/**
Send response to player when he joins the battlefield queue

@param   battlefield id
@param   zone id for the battlefield
@param   if player is able to queue
@param   if battlefield is full
*/
void WorldSession::SendBattlefieldQueueResponse(uint32 battlefieldId, uint32 zoneId, bool canJoin, bool queueFull)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_QUEUE_REQUEST_RESPONSE, 11);
    data << uint32(battlefieldId);
    data << uint32(zoneId);
    data << uint8(canJoin ? 1 : 0);     // Accepted     // 0 cannot queue   // 1 already in queue
    data << uint8(queueFull ? 0 : 1);   // Logging In   // 0 queue full     // 1 queue for upcoming battle
    data << uint8(1);                   // Warmup

    SendPacket(data);
}

/**
Send response when player accepts invitation to a battlefield

@param   battlefield id
*/
void WorldSession::SendBattlefieldEnteredResponse(uint32 battlefieldId)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_ENTERING, 7);
    data << uint32(battlefieldId);
    data << uint8(1);                                       // unk
    data << uint8(1);                                       // unk
    data << uint8(_player->isAFK() ? 1 : 0);                // Clear AFK

    SendPacket(data);
}

/**
Send response when player leaves a battlefield

@param   battlefield id
@param   leave reason
*/
void WorldSession::SendBattlefieldLeaveMessage(uint32 battlefieldId, BattlefieldLeaveReason reason)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_EJECTED, 7);
    data << uint32(battlefieldId);
    data << uint8(reason);          // byte Reason
    data << uint8(2);               // byte BattleStatus
    data << uint8(0);               // bool Relocated

    SendPacket(data);
}

// Sent by client when player clicks accept / reject queue invitation
void WorldSession::HandleBattlefieldQueueResponse(WorldPacket& recv_data)
{
    uint32 battlefieldId;
    uint8 accepted;

    recv_data >> battlefieldId >> accepted;
    DEBUG_LOG("HandleBattlefieldQueueResponse: battlefieldId: %u accepted: %u", battlefieldId, accepted);

    if (Battlefield* battlefield = sOutdoorPvPMgr.GetBattlefieldById(battlefieldId))
        battlefield->HandleQueueInviteResponse(GetPlayer(), accepted != 0);
}

// Sent by client when player clicks accept / reject war invitation
void WorldSession::HandleBattlefieldWarResponse(WorldPacket& recv_data)
{
    uint32 battlefieldId;
    uint8 accepted;

    recv_data >> battlefieldId >> accepted;
    DEBUG_LOG("HandleBattlefieldWarResponse: battlefieldId: %u accepted: %u", battlefieldId, accepted);

    if (Battlefield* battlefield = sOutdoorPvPMgr.GetBattlefieldById(battlefieldId))
        battlefield->HandleWarInviteResponse(GetPlayer(), accepted != 0);
}

// Sent by client when player exits the battlefield
void WorldSession::HandleBattlefieldExitRequest(WorldPacket& recv_data)
{
    uint32 battlefieldId;
    recv_data >> battlefieldId;

    DEBUG_LOG("HandleBattlefieldExitRequest: battlefieldId: %u", battlefieldId);

    if (Battlefield* battlefield = sOutdoorPvPMgr.GetBattlefieldById(battlefieldId))
        battlefield->HandleExitRequest(GetPlayer());
}
