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

#include "Server/WorldSession.h"
#include "Log.h"
#include "Entities/Player.h"
#include "WorldPacket.h"
#include "Globals/ObjectAccessor.h"
#include "LFG/LFGMgr.h"
#include "World/World.h"

void BuildPlayerLockDungeonBlock(WorldPacket& data, LfgLockMap const& locks)
{
    data << uint32(locks.size());                           // Size of lock dungeons
    for (auto& lock : locks)
    {
        data << uint32(lock.first);                         // Dungeon entry (id + type)
        data << uint32(lock.second);                        // Lock status
    }
}

void BuildPartyLockDungeonBlock(WorldPacket& data, LfgLockPartyMap const& lockMap)
{
    data << uint8(lockMap.size());
    for (auto& lock : lockMap)
    {
        data << uint64(lock.first);                         // Player guid
        BuildPlayerLockDungeonBlock(data, lock.second);
    }
}

void WorldSession::HandleLfgJoinOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_JOIN");

    uint32 roles;
    uint8 noPartialClear;
    uint8 achievements;

    std::string comment;
    std::vector<uint32> slots;
    std::vector<uint8> needs;

    recv_data >> roles;
    recv_data >> noPartialClear;
    recv_data >> achievements;

    slots.resize(recv_data.read<uint8>());

    for (auto& slot : slots)
        recv_data >> slot;

    needs.resize(recv_data.read<uint8>());

    for (auto& need : needs)
        recv_data >> need;

    recv_data >> comment;

    LfgDungeonSet newDungeons;
    for (uint32 slot : slots)
    {
        uint32 dungeon = slot & 0x00FFFFFF;                             // remove the type from the dungeon entry
        if (sLFGDungeonStore.LookupEntry(dungeon))
            newDungeons.insert(dungeon);
    }

    sLFGMgr.JoinLfg(GetPlayer(), roles, newDungeons, comment);
}

void WorldSession::HandleLfgLeaveOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_LOG("CMSG_LFG_LEAVE");

    sLFGMgr.LeaveLfg(GetPlayer());
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPacket& recv_data)
{
    uint32 lfgGroupID; // Internal lfgGroupID
    bool accept; // Accept to join?
    recv_data >> lfgGroupID;
    recv_data >> accept;
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPacket& recv_data)
{
    uint8 roles;
    recv_data >> roles; // Player Group Roles
}

void WorldSession::HandleLfgSetCommentOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_SET_LFG_COMMENT");

    std::string comment;
    recv_data >> comment;
    DEBUG_LOG("LFG comment \"%s\"", comment.c_str());

    Player* player = GetPlayer();
    if (!player)
        return;

    if (Group* group = player->GetGroup())
        group->GetLfgData().SetComment(comment);
    else
        player->GetLfgData().SetComment(comment);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket& recv_data)
{
    bool agree; // Agree to kick player
    recv_data >> agree;
}

void WorldSession::HandleLfrJoinOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_SEARCH_JOIN");

    uint32 dungeonId; // Raid id to search
    recv_data >> dungeonId;
    dungeonId = dungeonId & 0x00FFFFFF;

    sWorld.GetMessager().AddMessage([dungeonId, team = GetPlayer()->GetTeam(), guid = GetPlayer()->GetObjectGuid()](World* world)
    {
        world->GetRaidBrowser().AddListener(dungeonId, team, guid);
    });
}

void WorldSession::HandleLfrLeaveOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_SEARCH_LEAVE");

    uint32 dungeonId; // Raid id queue to leave
    recv_data >> dungeonId;
    dungeonId = dungeonId & 0x00FFFFFF;

    sWorld.GetMessager().AddMessage([dungeonId, team = GetPlayer()->GetTeam(), guid = GetPlayer()->GetObjectGuid()](World* world)
    {
        world->GetRaidBrowser().RemoveListener(dungeonId, team, guid);
    });
}

void WorldSession::HandleLfgGetStatus(WorldPacket& recv_data)
{
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket& recv_data)
{
    bool out;
    recv_data >> out;
}

void WorldSession::HandleLfgPlayerLockInfoRequestOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LFG_GET_PLAYER_INFO %s", GetPlayer()->GetName());

    // Get Random dungeons that can be done at a certain level and expansion
    uint8 level = GetPlayer()->GetLevel();
    LfgDungeonSet const& randomDungeons = sLFGMgr.GetRandomAndSeasonalDungeons(level, GetPlayer()->GetSession()->GetExpansion());

    // Get player locked Dungeons
    LfgLockMap const& lock = sLFGMgr.GetLockedDungeons(GetPlayer());
    uint32 rsize = uint32(randomDungeons.size());
    uint32 lsize = uint32(lock.size());

    DEBUG_LOG("SMSG_LFG_PLAYER_INFO %s", GetPlayer()->GetName());
    WorldPacket data(SMSG_LFG_PLAYER_INFO, 1 + rsize * (4 + 1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4) + 4 + lsize * (1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4));

    data << uint8(randomDungeons.size());                  // Random Dungeon count
    for (auto itr = randomDungeons.begin(); itr != randomDungeons.end(); ++itr)
    {
        data << uint32(*itr);                               // Dungeon Entry (id + type)
        LfgReward const* reward = sLFGMgr.GetRandomDungeonReward(*itr, level);
        Quest const* quest = nullptr;
        bool done = false;
        if (reward)
        {
            quest = sObjectMgr.GetQuestTemplate(reward->firstQuest);
            if (quest)
            {
                done = !GetPlayer()->CanRewardQuest(quest, false);
                if (done)
                    quest = sObjectMgr.GetQuestTemplate(reward->otherQuest);
            }
        }

        if (quest)
        {
            data << uint8(done);
            data << uint32(quest->GetRewOrReqMoney());
            data << uint32(quest->XPValue(GetPlayer()));
            data << uint32(0); // money variance per missing member when queueing - not actually used back then
            data << uint32(0); // experience variance per missing member when queueing - not actually used back then
            data << uint8(quest->GetRewItemsCount());
            if (quest->GetRewItemsCount())
            {
                for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
                    if (uint32 itemId = quest->RewItemId[i])
                    {
                        ItemPrototype const* item = sObjectMgr.GetItemPrototype(itemId);
                        data << uint32(itemId);
                        data << uint32(item ? item->DisplayInfoID : 0);
                        data << uint32(quest->RewItemCount[i]);
                    }
            }
        }
        else
        {
            data << uint8(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint8(0);
        }
    }
    BuildPlayerLockDungeonBlock(data, lock);
    SendPacket(data);
}

void WorldSession::HandleLfgPartyLockInfoRequestOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("SMSG_LFG_PARTY_INFO %s", GetPlayer()->GetName());
    Player* player = GetPlayer();
    if (!player)
        return;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    // Get the locked dungeons of the other party members
    LfgLockPartyMap lockMap;
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->getSource();
        if (!member)
            continue;

        if (member == player)
            continue;

        lockMap[player->GetObjectGuid()] = sLFGMgr.GetLockedDungeons(player);
    }

    uint32 size = 0;
    for (auto& data : lockMap)
        size += 8 + 4 + uint32(data.second.size()) * (4 + 4);

    DEBUG_LOG("SMSG_LFG_PARTY_INFO %s", player->GetName());
    WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);
    BuildPartyLockDungeonBlock(data, lockMap);
    SendPacket(data);
}

void WorldSession::SendLfgJoinResult(LfgJoinResultData joinResult) const
{
    uint32 size = 0;
    for (auto itr = joinResult.lockmap.begin(); itr != joinResult.lockmap.end(); ++itr)
        size += 8 + 4 + uint32(itr->second.size()) * (4 + 4);

    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);
    data << uint32(joinResult.result); // Check Result
    data << uint32(joinResult.state);  // Check Value
    if (!joinResult.lockmap.empty())
        BuildPartyLockDungeonBlock(data, joinResult.lockmap);
    SendPacket(data);
}

void WorldSession::SendLfgUpdatePlayer(LfgUpdateData const& updateData) const
{
    bool queued = false;
    uint8 size = uint8(updateData.dungeons.size());

    switch (updateData.updateType)
    {
        case LFG_UPDATETYPE_JOIN_QUEUE:
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            queued = true;
            break;
        case LFG_UPDATETYPE_UPDATE_STATUS:
            queued = updateData.state == LFG_STATE_QUEUED;
            break;
        default:
            break;
    }

    DEBUG_LOG("SMSG_LFG_UPDATE_PLAYER %s updatetype: %u", GetPlayerName(), updateData.updateType);
    WorldPacket data(SMSG_LFG_UPDATE_PLAYER, 1 + 1 + (size > 0 ? 1 : 0) * (1 + 1 + 1 + 1 + size * 4 + updateData.comment.length()));
    data << uint8(updateData.updateType);                  // Lfg Update type
    data << uint8(size > 0);                               // Is joined in LFG
    if (size)
    {
        data << uint8(queued);                             // Join the queue
        data << uint8(0);                                  // NoPartialClear
        data << uint8(0);                                  // Achievements

        data << uint8(size);
        for (auto itr = updateData.dungeons.begin(); itr != updateData.dungeons.end(); ++itr)
            data << uint32(*itr);
        data << updateData.comment;
    }
    SendPacket(data);
}

void WorldSession::SendLfgUpdate(bool isGroup, uint8 updateType, uint32 id) const
{
    WorldPacket data(isGroup ? SMSG_LFG_UPDATE_PARTY : SMSG_LFG_UPDATE_PLAYER, 0);
    data << uint8(updateType);

    data << uint8(true);

    if (true)
    {
        data << uint8(0);
        data << uint8(0);
        data << uint8(0);

        if (isGroup)
        {
            data << uint8(0);
            for (uint32 i = 0; i < 3; ++i)
                data << uint8(0);
        }

        uint8 count = 1;
        data << uint8(count);
        for (uint32 i = 0; i < count; ++i)
            data << uint32(id);
        data << "";
    }
    SendPacket(data);
}

void WorldSession::SendLfgDisabled()
{
    DEBUG_LOG("SMSG_LFG_DISABLED");
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(data);
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    DEBUG_LOG("SMSG_LFG_OFFER_CONTINUE %u", dungeonEntry);
    WorldPacket data(SMSG_LFG_OFFER_CONTINUE, 4);
    data << uint32(dungeonEntry);
    SendPacket(data);
}

void WorldSession::SendLfgTeleportError(uint8 err)
{
    DEBUG_LOG("SMSG_LFG_TELEPORT_DENIED %u", err);
    WorldPacket data(SMSG_LFG_TELEPORT_DENIED, 4);
    data << uint32(err); // Error
    SendPacket(data);
}

