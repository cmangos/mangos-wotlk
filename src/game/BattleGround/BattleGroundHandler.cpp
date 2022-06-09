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
#include "Globals/SharedDefines.h"
#include "WorldPacket.h"
#include "Server/Opcodes.h"
#include "Log.h"
#include "Entities/Player.h"
#include "Globals/ObjectMgr.h"
#include "Server/WorldSession.h"
#include "Entities/Object.h"
#include "BattleGroundEY.h"
#include "BattleGroundMgr.h"
#include "BattleGroundWS.h"
#include "BattleGround.h"
#include "Arena/ArenaTeam.h"
#include "Tools/Language.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "World/World.h"

// Sent by client when player talk to the battle master
void WorldSession::HandleBattlemasterHelloOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;

    DEBUG_LOG("WORLD: Received opcode CMSG_BATTLEMASTER_HELLO from %s", guid.GetString().c_str());

    Creature* pCreature = GetPlayer()->GetMap()->GetCreature(guid);

    if (!pCreature)
        return;

    if (!pCreature->isBattleMaster())                       // it's not battlemaster
        return;

    // Stop the npc if moving
    if (uint32 pauseTimer = pCreature->GetInteractionPauseTimer())
        pCreature->GetMotionMaster()->PauseWaypoints(pauseTimer);

    BattleGroundTypeId bgTypeId = sBattleGroundMgr.GetBattleMasterBG(pCreature->GetEntry());

    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
        return;

    if (!_player->GetBGAccessByLevel(bgTypeId))
    {
        // temp, must be gossip message...
        SendNotification(LANG_YOUR_BG_LEVEL_REQ_ERROR);
        return;
    }

    SendBattleGroundList(guid, bgTypeId);
}

/**
Send battleground list

@param   guid
@param   bgTypeId
*/
void WorldSession::SendBattleGroundList(ObjectGuid guid, BattleGroundTypeId bgTypeId) const
{
    WorldPacket data;
    sBattleGroundMgr.BuildBattleGroundListPacket(data, guid, _player, bgTypeId, 0);
    SendPacket(data);
}

// Sent by client when player wants to join a battleground
void WorldSession::HandleBattlemasterJoinOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    uint32 receivedBgTypeId;
    uint32 instanceId;
    uint8 joinAsGroup;
    bool isPremade = false;
    Group* grp = nullptr;

    recv_data >> guid;                                      // battlemaster guid
    recv_data >> receivedBgTypeId;                          // battleground type id (DBC id)
    recv_data >> instanceId;                                // instance id, 0 if First Available selected
    recv_data >> joinAsGroup;                               // join as group

    if (!sBattlemasterListStore.LookupEntry(receivedBgTypeId))
    {
        sLog.outError("Battleground: invalid bgtype (%u) received. possible cheater? player guid %u", receivedBgTypeId, _player->GetGUIDLow());
        return;
    }

    BattleGroundTypeId bgTypeId = BattleGroundTypeId(receivedBgTypeId);

    DEBUG_LOG("WORLD: Received opcode CMSG_BATTLEMASTER_JOIN from %s", guid.GetString().c_str());

    // can do this, since it's battleground, not arena
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BgQueueTypeId(bgTypeId, ARENA_TYPE_NONE);
    BattleGroundQueueTypeId bgQueueTypeIdRandom = BattleGroundMgr::BgQueueTypeId(BATTLEGROUND_RB, ARENA_TYPE_NONE);

    // ignore if player is already in BG
    if (_player->InBattleGround())
        return;

    // get bg instance or bg template if instance not found
    BattleGround* bg = nullptr;
    if (instanceId)
        bg = sBattleGroundMgr.GetBattleGroundThroughClientInstance(instanceId, bgTypeId);

    if (!bg)
        bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);

    if (!bg)
    {
        sLog.outError("Battleground: no available bg / template found");
        return;
    }

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL;

    // check queue conditions
    if (!joinAsGroup)
    {
        // check Deserter debuff
        if (!_player->CanJoinToBattleground())
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(data);
            return;
        }

        // check if already in random queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeIdRandom) < PLAYER_MAX_BATTLEGROUND_QUEUES)
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, ERR_IN_RANDOM_BG);
            _player->GetSession()->SendPacket(data);
            return;
        }

        // check if already in normal queue; cannot start random queue
        if(_player->InBattleGroundQueue() && bgTypeId == BATTLEGROUND_RB)
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, ERR_IN_NON_RANDOM_BG);
            _player->GetSession()->SendPacket(data);
            return;
        }

        // check if already in queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            // player is already in this queue
            return;

        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
            _player->GetSession()->SendPacket(data);
            return;
        }
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        if (grp->GetLeaderGuid() != _player->GetObjectGuid())
            return;
        err = grp->CanJoinBattleGroundQueue(bg, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), false, 0);
        isPremade = sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH) &&
                    (grp->GetMembersCount() >= bg->GetMinPlayersPerTeam());
    }
    // if we're here, then the conditions to join a bg are met. We can proceed in joining.

    // _player->GetGroup() was already checked, grp is already initialized
    BattleGroundQueue& bgQueue = sBattleGroundMgr.m_battleGroundQueues[bgQueueTypeId];
    if (joinAsGroup)
    {
        GroupQueueInfo* queueInfo = nullptr;
        uint32 avgTime = 0;

        if (err > ERR_GROUP_JOIN_BATTLEGROUND_FAIL)
        {
            DEBUG_LOG("Battleground: the following players are joining as group:");
            queueInfo = bgQueue.AddGroup(_player, grp, bgTypeId, bracketEntry, ARENA_TYPE_NONE, false, isPremade, instanceId, 0);
            avgTime = bgQueue.GetAverageQueueWaitTime(queueInfo, bracketEntry->GetBracketId());
        }

        for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member)
                continue;                                   // this should never happen

            WorldPacket data;

            if (err <= ERR_GROUP_JOIN_BATTLEGROUND_FAIL)
            {
                sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, err);
                member->GetSession()->SendPacket(data);
                continue;
            }

            // add to queue
            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);

            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, queueInfo->arenaType, TEAM_NONE);
            member->GetSession()->SendPacket(data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, err);
            member->GetSession()->SendPacket(data);
            DEBUG_LOG("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
        }
        DEBUG_LOG("Battleground: group end");
    }
    else
    {
        GroupQueueInfo* queueInfo = bgQueue.AddGroup(_player, nullptr, bgTypeId, bracketEntry, ARENA_TYPE_NONE, false, isPremade, instanceId, 0);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(queueInfo, bracketEntry->GetBracketId());
        // already checked if queueSlot is valid, now just get it
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);

        WorldPacket data;
        // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, queueInfo->arenaType, TEAM_NONE);
        SendPacket(data);
        DEBUG_LOG("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, _player->GetGUIDLow(), _player->GetName());
    }

    sBattleGroundMgr.ScheduleQueueUpdate(0, ARENA_TYPE_NONE, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

// Sent by client while inside battleground; depends on the battleground type
void WorldSession::HandleBattleGroundPlayerPositionsOpcode(WorldPacket& /*recv_data*/)
{
    // empty opcode
    DEBUG_LOG("WORLD: Received opcode MSG_BATTLEGROUND_PLAYER_POSITIONS");

    BattleGround* bg = _player->GetBattleGround();
    if (!bg)                                                // can't be received if player not in battleground
        return;

    switch (bg->GetTypeId())
    {
        case BATTLEGROUND_WS:
        {
            uint32 flagCarrierCount = 0;

            Player* flagCarrierAlliance = sObjectMgr.GetPlayer(static_cast<BattleGroundWS*>(bg)->GetFlagCarrierGuid(TEAM_INDEX_ALLIANCE));
            if (flagCarrierAlliance)
            {
                if (flagCarrierAlliance->GetTeam() == _player->GetTeam() || static_cast<BattleGroundWS*>(bg)->IsFlagHeldFor45Seconds(flagCarrierAlliance->GetTeam()))
                    ++flagCarrierCount;
            }

            Player* flagCarrierHorde = sObjectMgr.GetPlayer(static_cast<BattleGroundWS*>(bg)->GetFlagCarrierGuid(TEAM_INDEX_HORDE));
            if (flagCarrierHorde)
            {
                if (flagCarrierHorde->GetTeam() == _player->GetTeam() || static_cast<BattleGroundWS*>(bg)->IsFlagHeldFor45Seconds(flagCarrierHorde->GetTeam()))
                    ++flagCarrierCount;
            }

            WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, 4 + 4 + 16 * flagCarrierCount);
            data << uint32(0); // own team position XYZ
            data << uint32(flagCarrierCount);

            if (flagCarrierAlliance)
            {
                data << flagCarrierAlliance->GetObjectGuid();
                data << float(flagCarrierAlliance->GetPositionX());
                data << float(flagCarrierAlliance->GetPositionY());
            }
            if (flagCarrierHorde)
            {
                data << flagCarrierHorde->GetObjectGuid();
                data << float(flagCarrierHorde->GetPositionX());
                data << float(flagCarrierHorde->GetPositionY());
            }

            SendPacket(data);
            break;
        }
        case BATTLEGROUND_EY:
        {
            uint32 flagCarrierCount = 0;

            Player* flagCarrier = sObjectMgr.GetPlayer(((BattleGroundEY*)bg)->GetFlagCarrierGuid());
            if (flagCarrier)
                flagCarrierCount = 1;

            WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, 4 + 4 + 16 * flagCarrierCount);
            data << uint32(0);
            data << uint32(flagCarrierCount);

            if (flagCarrier)
            {
                data << flagCarrier->GetObjectGuid();
                data << float(flagCarrier->GetPositionX());
                data << float(flagCarrier->GetPositionY());
            }

            SendPacket(data);
            break;
        }
        case BATTLEGROUND_AB:
        case BATTLEGROUND_AV:
        {
            // for other BG types - send default
            WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, 4 + 4);
            data << uint32(0);
            data << uint32(0);
            SendPacket(data);
            break;
        }
        default:
            // maybe it is sent also in arena - do nothing
            break;
    }
}

// Sent by client when requesting the PvP match log
void WorldSession::HandlePVPLogDataOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_LOG("WORLD: Received opcode MSG_PVP_LOG_DATA");

    BattleGround* bg = _player->GetBattleGround();
    if (!bg)
        return;

    // arena finish version will send in BattleGround::EndBattleGround directly
    if (bg->IsArena())
        return;

    WorldPacket data;
    sBattleGroundMgr.BuildPvpLogDataPacket(data, bg);
    SendPacket(data);

    DEBUG_LOG("WORLD: Sent MSG_PVP_LOG_DATA Message");
}

// Sent by client when requesting available battleground list
void WorldSession::HandleBattlefieldListOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_BATTLEFIELD_LIST");

    uint32 bgTypeId;
    recv_data >> bgTypeId;                                  // id from DBC

    uint8 fromWhere;
    recv_data >> fromWhere;                                 // 0 - battlemaster (lua: ShowBattlefieldList), 1 - UI (lua: RequestBattlegroundInstanceInfo)

    uint8 xpLocked;
    recv_data >> xpLocked;                                  // players with locked xp have a separate queue; Not implemented yet

    BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);
    if (!bl)
    {
        sLog.outError("Battleground: invalid bgtype received.");
        return;
    }

    WorldPacket data;
    sBattleGroundMgr.BuildBattleGroundListPacket(data, ObjectGuid(), _player, BattleGroundTypeId(bgTypeId), fromWhere);
    SendPacket(data);
}

// Sent by client when requesting teleport to the battleground location
void WorldSession::HandleBattlefieldPortOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_BATTLEFIELD_PORT");

    uint8 type;                                             // arenatype if arena
    uint8 unk2;                                             // unk, can be 0x0 (may be if was invited?) and 0x1
    uint32 receivedBgTypeId;                                // type id from dbc
    uint16 unk;                                             // 0x1F90 constant?
    uint8 action;                                           // enter battle 0x1, leave queue 0x0

    recv_data >> type >> unk2 >> receivedBgTypeId >> unk >> action;

    if (!sBattlemasterListStore.LookupEntry(receivedBgTypeId))
    {
        sLog.outError("BattlegroundHandler: invalid bgtype (%u) received.", receivedBgTypeId);
        return;
    }

    if (type && !IsArenaTypeValid(ArenaType(type)))
    {
        sLog.outError("BattlegroundHandler: Invalid CMSG_BATTLEFIELD_PORT received from player (%u), arena type wrong: %u.", _player->GetGUIDLow(), type);
        return;
    }

    if (!_player->InBattleGroundQueue())
    {
        sLog.outError("BattlegroundHandler: Invalid CMSG_BATTLEFIELD_PORT received from player (%u), he is not in bg_queue.", _player->GetGUIDLow());
        return;
    }

    // get GroupQueueInfo from BattleGroundQueue
    BattleGroundTypeId bgTypeId = BattleGroundTypeId(receivedBgTypeId);
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BgQueueTypeId(bgTypeId, ArenaType(type));
    BattleGroundQueue& bgQueue = sBattleGroundMgr.m_battleGroundQueues[bgQueueTypeId];

    // we must use temporary variable, because GroupQueueInfo pointer can be deleted in BattleGroundQueue::RemovePlayer() function
    GroupQueueInfo queueInfo;
    if (!bgQueue.GetPlayerGroupInfoData(_player->GetObjectGuid(), &queueInfo))
    {
        sLog.outError("BattlegroundHandler: itrplayerstatus not found.");
        return;
    }

    // if action == 1, then instanceId is required
    if (!queueInfo.isInvitedToBgInstanceGuid && action == 1)
    {
        sLog.outError("BattlegroundHandler: instance not found.");
        return;
    }

    BattleGround* bg = sBattleGroundMgr.GetBattleGround(queueInfo.isInvitedToBgInstanceGuid, bgTypeId);

    // bg template might and must be used in case of leaving queue, when instance is not created yet
    if (!bg && action == 0)
        bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
    {
        sLog.outError("BattlegroundHandler: bg_template not found for type id %u.", bgTypeId);
        return;
    }

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    // some checks if player isn't cheating - it is not exactly cheating, but we cannot allow it
    if (action == 1 && queueInfo.arenaType == ARENA_TYPE_NONE)
    {
        // if player is trying to enter battleground (not arena!) and he has deserter debuff, we must just remove him from queue
        if (!_player->CanJoinToBattleground())
        {
            // send bg command result to show nice message
            WorldPacket data2;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data2, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(data2);
            action = 0;

            DEBUG_LOG("Battleground: player %s (%u) has a deserter debuff, do not port him to battleground!", _player->GetName(), _player->GetGUIDLow());
        }
        // if player don't match battleground max level, then do not allow him to enter! (this might happen when player leveled up during his waiting in queue
        if (_player->GetLevel() > bg->GetMaxLevel())
        {
            sLog.outError("Battleground: Player %s (%u) has level (%u) higher than maxlevel (%u) of battleground (%u)! Do not port him to battleground!",
                          _player->GetName(), _player->GetGUIDLow(), _player->GetLevel(), bg->GetMaxLevel(), bg->GetTypeId());
            action = 0;
        }
    }

    uint32 queueSlot = _player->GetBattleGroundQueueIndex(bgQueueTypeId);
    WorldPacket data;

    switch (action)
    {
        case 1:                                         // port to battleground
            if (!_player->IsInvitedForBattleGroundQueueType(bgQueueTypeId))
                return;                                 // cheating?

            if (!_player->InBattleGround())
                _player->SetBattleGroundEntryPoint();

            // resurrect the player
            if (!_player->IsAlive())
            {
                _player->ResurrectPlayer(1.0f);
                _player->SpawnCorpseBones();
            }

            _player->TaxiFlightInterrupt(false);

            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime(), bg->GetArenaType(), _player->GetBGTeam());
            _player->GetSession()->SendPacket(data);

            // remove battleground queue status from BGmgr
            bgQueue.RemovePlayer(_player->GetObjectGuid(), false);

            // this is still needed here if battleground "jumping" shouldn't add deserter debuff
            // also this is required to prevent stuck at old battleground after SetBattleGroundId set to new
            if (BattleGround* currentBg = _player->GetBattleGround())
                currentBg->RemovePlayerAtLeave(_player->GetObjectGuid(), false, true);

            // set the destination instance id
            _player->SetBattleGroundId(bg->GetInstanceId(), bgTypeId);
            // set the destination team
            _player->SetBGTeam(queueInfo.groupTeam);
            // bg->HandleBeforeTeleportToBattleGround(_player);
            sBattleGroundMgr.SendToBattleGround(_player, queueInfo.isInvitedToBgInstanceGuid, bgTypeId);
            // add only in HandleMoveWorldPortAck()
            // bg->AddPlayer(_player,team);

            DEBUG_LOG("Battleground: player %s (%u) joined battle for bg %u, bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetInstanceId(), bg->GetTypeId(), bgQueueTypeId);

            break;
        case 0:                                         // leave queue
            // if player leaves rated arena match before match start, it is counted as he played but he lost
            if (queueInfo.isRated && queueInfo.isInvitedToBgInstanceGuid)
            {
                ArenaTeam* at = sObjectMgr.GetArenaTeamById(queueInfo.arenaTeamId);
                if (at)
                {
                    DEBUG_LOG("UPDATING memberLost's personal arena rating for %s by opponents rating: %u, because he has left queue!", _player->GetGuidStr().c_str(), queueInfo.opponentsTeamRating);
                    at->MemberLost(_player, queueInfo.opponentsTeamRating);
                    at->SaveToDB();
                }
            }

            _player->RemoveBattleGroundQueueId(bgQueueTypeId);  // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_NONE, 0, 0, ARENA_TYPE_NONE, TEAM_NONE);
            bgQueue.RemovePlayer(_player->GetObjectGuid(), true);

            // player left queue, we should update it - do not update Arena Queue
            if (queueInfo.arenaType == ARENA_TYPE_NONE)
                sBattleGroundMgr.ScheduleQueueUpdate(queueInfo.arenaTeamRating, queueInfo.arenaType, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());

            SendPacket(data);

            DEBUG_LOG("Battleground: player %s (%u) left queue for bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetTypeId(), bgQueueTypeId);

            break;
        default:
            sLog.outError("Battleground port: unknown action %u", action);
            break;
    }
}

// Sent by client when leaving the battleground
void WorldSession::HandleLeaveBattlefieldOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_LEAVE_BATTLEFIELD");

    recv_data.read_skip<uint8>();                           // unk1
    recv_data.read_skip<uint8>();                           // unk2
    recv_data.read_skip<uint32>();                          // BattleGroundTypeId
    recv_data.read_skip<uint16>();                          // unk3

    // if(bgTypeId >= MAX_BATTLEGROUND_TYPES)               // cheating? but not important in this case
    //    return;

    // not allow leave battleground in combat
    if (_player->IsInCombat())
        if (BattleGround* bg = _player->GetBattleGround())
            if (bg->GetStatus() != STATUS_WAIT_LEAVE)
                return;

    _player->LeaveBattleground();
}

// Sent by client when requesting battleground status
void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket& /*recv_data*/)
{
    // empty opcode
    DEBUG_LOG("WORLD: Battleground status");

    WorldPacket data;
    // we must update all queues here
    BattleGround* bg;
    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattleGroundQueueTypeId bgQueueTypeId = _player->GetBattleGroundQueueTypeId(i);
        if (!bgQueueTypeId)
            continue;

        BattleGroundTypeId bgTypeId = BattleGroundMgr::BgTemplateId(bgQueueTypeId);
        ArenaType arenaType = BattleGroundMgr::BgArenaType(bgQueueTypeId);

        // bgTypeId is always BATTLEGROUND_AA for arena whereas GetBattleGroundTypeId() is the actual arena map type
        if (bgTypeId == _player->GetBattleGroundTypeId() || (bgTypeId == BATTLEGROUND_AA && sBattleGroundMgr.IsArenaType(_player->GetBattleGroundTypeId())))
        {
            bg = _player->GetBattleGround();
            // i cannot check any variable from player class because player class doesn't know if player is in 2v2 / 3v3 or 5v5 arena
            // so i must use bg pointer to get that information
            if (bg && bg->GetArenaType() == arenaType)
            {
                // this line is checked, i only don't know if GetStartTime is changing itself after bg end!
                // send status in BattleGround
                sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, i, STATUS_IN_PROGRESS, bg->GetEndTime(), bg->GetStartTime(), arenaType, _player->GetBGTeam());
                SendPacket(data);
                continue;
            }
        }
        // we are sending update to player about queue - he can be invited there!
        // get GroupQueueInfo for queue status
        BattleGroundQueue& bgQueue = sBattleGroundMgr.m_battleGroundQueues[bgQueueTypeId];
        GroupQueueInfo queueInfo;
        if (!bgQueue.GetPlayerGroupInfoData(_player->GetObjectGuid(), &queueInfo))
            continue;

        if (queueInfo.isInvitedToBgInstanceGuid)
        {
            bg = sBattleGroundMgr.GetBattleGround(queueInfo.isInvitedToBgInstanceGuid, bgTypeId);
            if (!bg)
                continue;

            uint32 remainingTime = WorldTimer::getMSTimeDiff(WorldTimer::getMSTime(), queueInfo.removeInviteTime);
            // send status invited to BattleGround
            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, i, STATUS_WAIT_JOIN, remainingTime, 0, arenaType, TEAM_NONE);
            SendPacket(data);
        }
        else
        {
            bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            if (!bg)
                continue;

            // expected bracket entry
            PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
            if (!bracketEntry)
                continue;

            uint32 avgTime = bgQueue.GetAverageQueueWaitTime(&queueInfo, bracketEntry->GetBracketId());
            // send status in BattleGround Queue
            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, i, STATUS_WAIT_QUEUE, avgTime, WorldTimer::getMSTimeDiff(queueInfo.joinTime, WorldTimer::getMSTime()), arenaType, TEAM_NONE);
            SendPacket(data);
        }
    }
}

// Sent by client when requesting the spirit healer
void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    BattleGround* bg = _player->GetBattleGround();
    if (!bg)
        return;

    ObjectGuid guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                           // it's not spirit service
        return;

    unit->SendAreaSpiritHealerQueryOpcode(GetPlayer());
}

// Sent by client when requesting spirit healer queue
void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    BattleGround* bg = _player->GetBattleGround();
    if (!bg)
        return;

    ObjectGuid guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                           // it's not spirit service
        return;

    sScriptDevAIMgr.OnGossipHello(GetPlayer(), unit);
}

// Sent by client when requesting arena join
void WorldSession::HandleBattlemasterJoinArena(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_BATTLEMASTER_JOIN_ARENA");
    // recv_data.hexlike();

    ObjectGuid guid;                                        // arena Battlemaster guid
    uint8 arenaslot;                                        // 2v2, 3v3 or 5v5
    uint8 asGroup;                                          // asGroup
    uint8 isRated;                                          // isRated

    recv_data >> guid >> arenaslot >> asGroup >> isRated;

    // ignore if we already in BG or BG queue
    if (_player->InBattleGround())
        return;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                            // it's not battle master
        return;

    ArenaType arenatype;
    uint32 arenaRating = 0;

    switch (arenaslot)
    {
        case 0:
            arenatype = ARENA_TYPE_2v2;
            break;
        case 1:
            arenatype = ARENA_TYPE_3v3;
            break;
        case 2:
            arenatype = ARENA_TYPE_5v5;
            break;
        default:
            sLog.outError("Unknown arena slot %u at HandleBattlemasterJoinArena()", arenaslot);
            return;
    }

    // check existence
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_AA);
    if (!bg)
    {
        sLog.outError("Battleground: template bg (all arenas) not found");
        return;
    }

    BattleGroundTypeId bgTypeId = bg->GetTypeId();
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BgQueueTypeId(bgTypeId, arenatype);
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), _player->GetLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL;

    Group* group = nullptr;

    // check queue conditions
    if (!asGroup)
    {
        // you can't join in this way by client
        if (isRated)
            return;

        // check if already in queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            // player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
            return;
    }
    else
    {
        group = _player->GetGroup();
        // no group found, error
        if (!group)
            return;

        if (group->GetLeaderGuid() != _player->GetObjectGuid())
            return;

        // may be Group::CanJoinBattleGroundQueue should be moved to player class...
        err = group->CanJoinBattleGroundQueue(bg, bgQueueTypeId, arenatype, arenatype, isRated != 0, arenaslot);
    }

    uint32 ateamId = 0;

    if (isRated)
    {
        ateamId = _player->GetArenaTeamId(arenaslot);
        // check real arena team existence only here (if it was moved to group->CanJoin .. () then we would have to get it twice)
        ArenaTeam* at = sObjectMgr.GetArenaTeamById(ateamId);
        if (!at)
        {
            _player->GetSession()->SendNotInArenaTeamPacket(arenatype);
            return;
        }
        // get the team rating for queue
        arenaRating = at->GetRating();
        // the arena team id must match for everyone in the group
        // get the personal ratings for queue
        uint32 avg_pers_rating = 0;

        for (const auto& citr : group->GetMemberSlots())
        {
            ArenaTeamMember const* at_member = at->GetMember(citr.guid);
            if (!at_member)                                 // group member joining to arena must be in leader arena team
                return;

            // calc avg personal rating
            avg_pers_rating += at_member->personal_rating;
        }

        avg_pers_rating /= group->GetMembersCount();

        // if avg personal rating is more than 150 points below the teams rating, the team will be queued against an opponent matching or similar to the average personal rating
        if (avg_pers_rating + 150 < arenaRating)
            arenaRating = avg_pers_rating;
    }

    BattleGroundQueue& bgQueue = sBattleGroundMgr.m_battleGroundQueues[bgQueueTypeId];
    if (asGroup)
    {
        uint32 avgTime = 0;

        if (err > ERR_GROUP_JOIN_BATTLEGROUND_FAIL)
        {
            DEBUG_LOG("Battleground: arena join as group start");
            if (isRated)
                DEBUG_LOG("Battleground: arena team id %u, leader %s queued with rating %u for type %u", _player->GetArenaTeamId(arenaslot), _player->GetName(), arenaRating, arenatype);

            // set arena rated type to show correct minimap arena icon
            bg->SetRated(isRated != 0);

            GroupQueueInfo* ginfo = bgQueue.AddGroup(_player, group, bgTypeId, bracketEntry, arenatype, isRated != 0, false, 0, arenaRating, ateamId);
            avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member)
                continue;

            WorldPacket data;

            if (err <= ERR_GROUP_JOIN_BATTLEGROUND_FAIL)
            {
                sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, err);
                member->GetSession()->SendPacket(data);
                continue;
            }

            // add to queue
            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);

            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype, TEAM_NONE);
            member->GetSession()->SendPacket(data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(data, err);
            member->GetSession()->SendPacket(data);
            DEBUG_LOG("Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
        }
        DEBUG_LOG("Battleground: arena join as group end");
    }
    else
    {
        GroupQueueInfo* ginfo = bgQueue.AddGroup(_player, nullptr, bgTypeId, bracketEntry, arenatype, isRated != 0, false, 0, arenaRating, ateamId);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);

        WorldPacket data;
        // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype, TEAM_NONE);
        SendPacket(data);
        DEBUG_LOG("Battleground: player joined queue for arena, skirmish, bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, _player->GetGUIDLow(), _player->GetName());
    }
    sBattleGroundMgr.ScheduleQueueUpdate(arenaRating, arenatype, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

// Sent by client when reporting AFK
void WorldSession::HandleReportPvPAFK(WorldPacket& recv_data)
{
    ObjectGuid playerGuid;
    recv_data >> playerGuid;
    Player* reportedPlayer = sObjectMgr.GetPlayer(playerGuid);

    if (!reportedPlayer)
    {
        DEBUG_LOG("WorldSession::HandleReportPvPAFK: player not found");
        return;
    }

    DEBUG_LOG("WorldSession::HandleReportPvPAFK: %s reported %s", _player->GetName(), reportedPlayer->GetName());

    reportedPlayer->ReportedAfkBy(_player);
}
