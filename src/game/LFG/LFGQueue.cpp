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

#include "LFG/LFGQueue.h"
#include "LFG/LFGMgr.h"
#include "World/World.h"

void LFGQueue::AddToQueue(LFGQueueData const& data)
{
    auto result = m_queueData.emplace(data.m_ownerGuid, data);
    LFGQueueData& queueData = result.first->second;
    if (data.m_roleCheckState == LFG_ROLECHECK_INITIALITING)
        queueData.UpdateRoleCheck(queueData.m_leaderGuid, queueData.m_playerInfoPerGuid[queueData.m_leaderGuid].m_roles, false, false);
}

void LFGQueue::RemoveFromQueue(ObjectGuid owner)
{
    m_queueData.erase(owner);
}

void LFGQueue::SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles)
{
    auto itr = m_queueData.find(group);
    if (itr != m_queueData.end())
    {
        itr->second.UpdateRoleCheck(player, roles, false, false);
        if (itr->second.GetState() == LFG_STATE_FAILED)
            m_queueData.erase(itr);
    }
}

void LFGQueue::UpdateProposal(ObjectGuid playerGuid, uint32 proposalId, bool accept)
{
    auto itr = m_proposals.find(proposalId); // protection against packet spamming
    if (itr != m_proposals.end())
    {
        LfgProposal& proposal = itr->second;
        proposal.UpdatePlayerProposal(playerGuid, accept);

        // broadcast players proposal result to everyone
        std::map<ObjectGuid, std::vector<WorldPacket>> personalizedPackets;
        for (ObjectGuid queuedGroup : proposal.queues)
        {
            LFGQueueData& queueData = m_queueData[queuedGroup];
            for (auto& playerData : queueData.m_playerInfoPerGuid)
                personalizedPackets[playerData.first].emplace_back(WorldSession::BuildLfgUpdateProposal(proposal, queueData.m_randomDungeonId, playerData.first));
        }

        sWorld.GetMessager().AddMessage([=](World* world)
        {
            world->BroadcastPersonalized(personalizedPackets);
        });
    }
}

void LFGQueue::RemoveProposal(uint32 proposalId)
{
    m_proposalsForRemoval.push_back(proposalId);
}

void LFGQueue::OnPlayerLogout(ObjectGuid guid, ObjectGuid groupGuid)
{
    ObjectGuid searchGuid = groupGuid ? groupGuid : guid;
    auto itr = m_queueData.find(searchGuid);
    if (itr != m_queueData.end())
    {
        LFGQueueData& data = itr->second;
        if (data.m_roleCheckState == LFG_ROLECHECK_INITIALITING)
            data.UpdateRoleCheck(guid, 0, true, false);
        else if (data.GetState() == LFG_STATE_QUEUED)
        {
            LfgUpdateData updateData = LfgUpdateData(LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE);
            std::vector<WorldPacket> packets;
            packets.emplace_back(WorldSession::BuildLfgUpdate(updateData, searchGuid.IsGroup()));
            std::map<ObjectGuid, std::vector<WorldPacket>> personalizedPackets;
            for (auto& playerInfo : data.m_playerInfoPerGuid)
                personalizedPackets.emplace(playerInfo.first, packets);
            sWorld.GetMessager().AddMessage([personalizedPackets](World* world)
            {
                world->BroadcastPersonalized(personalizedPackets);
            });
        }
        m_queueData.erase(itr);
    }
}

void LFGQueue::Update()
{
    uint32 counter = 1;
    while (!World::IsStopped())
    {
        GetMessager().Execute(this);

        TimePoint now = sWorld.GetCurrentClockTime();
        for (auto itr = m_queueData.begin(); itr != m_queueData.end();)
        {
            LFGQueueData& queueData = itr->second;
            if (queueData.m_roleCheckState == LFG_ROLECHECK_INITIALITING && queueData.m_cancelTime < now)
            {
                queueData.UpdateRoleCheck(ObjectGuid(), 0, true, true);
                itr = m_queueData.erase(itr);
            }
            else
                ++itr;
        }

        if (IsTestingEnabled()) // in debug pop any queue regardless of eligibility
        {
            for (auto& queuedGroupData : m_queueData)
            {
                LFGQueueData& queueData = queuedGroupData.second;
                if (queueData.GetState() == LFG_STATE_QUEUED)
                {
                    LfgProposal proposal;
                    proposal.id = counter++;
                    queueData.PopQueue(proposal);
                    m_proposals[proposal.id] = proposal;
                }
            }
        }
        else
        {
            // no actual matchmaking for now - enable entering for full groups - TODO: matchmaking
            for (auto& queuedGroupData : m_queueData)
            {
                LFGQueueData& queueData = queuedGroupData.second;
                // rolecheck makes sure integrity of correct group is held
                if (queueData.GetState() == LFG_STATE_QUEUED && queueData.m_playerInfoPerGuid.size() == 5)
                {
                    LfgProposal proposal;
                    proposal.id = counter++;
                    queueData.PopQueue(proposal);
                    m_proposals[proposal.id] = proposal;
                }
            }
        }

        for (auto& proposalData : m_proposals)
            proposalData.second.UpdateProposal(*this);

        for (auto itr = m_queueData.begin(); itr != m_queueData.end();)
        {
            if (itr->second.GetState() == LFG_STATE_FAILED)
                itr = m_queueData.erase(itr);
            else
                ++itr;
        }

        for (uint32 proposalId : m_proposalsForRemoval)
            m_proposals.erase(proposalId);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

std::string LFGQueue::GetDebugPrintout()
{
    return std::string();
}

uint32 LFGQueue::GetPartyMemberCountAtJoin(ObjectGuid guid) const
{
    auto itr = m_numberOfPartyMembersAtJoin.find(guid);
    if (itr == m_numberOfPartyMembersAtJoin.end())
        return 0;

    return itr->second;
}

void LFGQueue::UpdateWaitTimeDps(int32 time, uint32 dungeonId)
{
    // TODO
}

void LFGQueue::UpdateWaitTimeHealer(int32 time, uint32 dungeonId)
{
    // TODO
}

void LFGQueue::UpdateWaitTimeTank(int32 time, uint32 dungeonId)
{
    // TODO
}

void LFGQueue::UpdateWaitTimeAvg(int32 time, uint32 dungeonId)
{
    // TODO
}

void LFGQueueData::UpdateRoleCheck(ObjectGuid guid, uint8 roles, bool abort, bool timeout)
{
    LfgPlayerInfoMap check_roles;

    bool sendRoleChosen = m_roleCheckState != LFG_ROLECHECK_DEFAULT && abort;

    if (abort)
        m_roleCheckState = timeout ? LFG_ROLECHECK_MISSING_ROLE : LFG_ROLECHECK_ABORTED;
    else if (m_playerInfoPerGuid.empty() || !roles) // Player selected no role.
        m_roleCheckState = LFG_ROLECHECK_NO_ROLE;
    else
    {
        m_playerInfoPerGuid[guid].m_roles = roles;

        // Check if all players have selected a role
        auto itrRoles = m_playerInfoPerGuid.begin();
        while (itrRoles != m_playerInfoPerGuid.end() && itrRoles->second.m_roles != PLAYER_ROLE_NONE)
            ++itrRoles;

        if (itrRoles == m_playerInfoPerGuid.end())
        {
            // use temporal var to check roles, CheckGroupRoles modifies the roles
            check_roles = m_playerInfoPerGuid;
            m_roleCheckState = (m_raid || LFGMgr::CheckGroupRoles(check_roles)) ? LFG_ROLECHECK_FINISHED : LFG_ROLECHECK_WRONG_ROLES;
        }
    }

    LfgDungeonSet dungeons = m_dungeons;

    LfgJoinResultData joinData = LfgJoinResultData(LFG_JOIN_FAILED, m_roleCheckState);

    std::vector<WorldPacket> packets;
    if (sendRoleChosen)
        packets.emplace_back(WorldSession::BuildLfgRoleChosen(guid, roles));

    packets.emplace_back(WorldSession::BuildLfgRoleCheckUpdate(*this));
    switch (m_roleCheckState)
    {
        case LFG_ROLECHECK_INITIALITING:
            break;
        case LFG_ROLECHECK_FINISHED:
            packets.emplace_back(WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_ADDED_TO_QUEUE,
                m_raid ? LFG_STATE_RAIDBROWSER : LFG_STATE_NONE, GetDungeons(), m_comment), true));
            break;
        default:
            packets.emplace_back(WorldSession::BuildLfgJoinResult(joinData));
            packets.emplace_back(WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_ROLECHECK_FAILED), true));
            break;
    }

    sWorld.GetMessager().AddMessage([groupGuid = m_ownerGuid, packets](World* world)
    {
        world->BroadcastToGroup(groupGuid, packets);
    });

    if (m_roleCheckState == LFG_ROLECHECK_FINISHED)
    {
        m_state = m_raid ? LFG_STATE_RAIDBROWSER : LFG_STATE_QUEUED;
        m_queueTime = sWorld.GetCurrentClockTime();
        m_roleCheckState = LFG_ROLECHECK_FINISHED;
        RecalculateRoles();
    }
    else if (m_roleCheckState != LFG_ROLECHECK_INITIALITING)
        m_state = LFG_STATE_FAILED;
}

void LFGQueueData::PopQueue(LfgProposal& proposal)
{
    auto itr = m_dungeons.begin();
    std::advance(itr, urand(0, m_dungeons.size() - 1));
    uint32 selectedDungeonId = *itr;
    m_state = LFG_STATE_PROPOSAL;
    std::vector<WorldPacket> packets;
    packets.emplace_back(WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_PROPOSAL_BEGIN, GetDungeons(), ""), true));

    proposal.dungeonId = selectedDungeonId;
    proposal.state = LFG_PROPOSAL_INITIATING;
    proposal.group = ObjectGuid(); // only filled when already lfg group
    proposal.leader = m_leaderGuid;
    proposal.cancelTime = sWorld.GetCurrentClockTime() + std::chrono::seconds(LFG_TIME_ROLECHECK);
    proposal.encounters = 0;
    proposal.isNew = true;
    proposal.queues.push_back(m_ownerGuid);
    
    for (auto& playerData : m_playerInfoPerGuid)
    {
        uint8 roles = playerData.second.m_roles; // give one random role out of given ones
        if (roles & PLAYER_ROLE_TANK)
            roles = PLAYER_ROLE_TANK;
        if (roles & PLAYER_ROLE_HEALER)
            roles = PLAYER_ROLE_HEALER;
        if (roles & PLAYER_ROLE_DAMAGE)
            roles = PLAYER_ROLE_DAMAGE;
        proposal.players[playerData.first] = LfgProposalPlayer(roles, LFG_ANSWER_PENDING, m_ownerGuid.IsGroup() ? m_ownerGuid : ObjectGuid(), m_randomDungeonId);
    }

    std::map<ObjectGuid, std::vector<WorldPacket>> personalizedPackets;
    for (auto& playerData : m_playerInfoPerGuid)
    {
        personalizedPackets[playerData.first] = packets;
        personalizedPackets[playerData.first].emplace_back(WorldSession::BuildLfgUpdateProposal(proposal, m_randomDungeonId, playerData.first));
    }

    sWorld.GetMessager().AddMessage([=](World* world)
    {
        world->BroadcastPersonalized(personalizedPackets);
    });
}

LfgDungeonSet LFGQueueData::GetDungeons() const
{
    if (m_randomDungeonId)
    {
        LfgDungeonSet set;
        set.insert(m_randomDungeonId);
        return set;
    }
    return m_dungeons;
}

void LfgProposal::UpdatePlayerProposal(ObjectGuid guid, bool accept)
{
    players[guid].answer = LfgAnswer(accept);
}

void LfgProposal::UpdateProposal(LFGQueue& queue)
{
    bool end = true;
    bool fail = false;

    for (auto& playerData : players)
    {
        if (playerData.second.answer == LFG_ANSWER_PENDING)
        {
            end = false;
            break;
        }

        if (playerData.second.answer == LFG_ANSWER_DENY)
            fail = true;
    }

    if (end)
    {
        if (fail)
            FailProposal(queue);
        else
            AcceptProposal(queue);
        return;
    }

    if (cancelTime < sWorld.GetCurrentClockTime())
        FailProposal(queue);
}

void LfgProposal::FailProposal(LFGQueue& queue)
{
    state = LFG_PROPOSAL_FAILED;
    std::set<ObjectGuid> declinedSelectors;
    std::set<ObjectGuid> failedSelectors;
    for (auto& proposalPlayer : players)
    {
        LfgProposalPlayer& player = proposalPlayer.second;
        if (player.answer == 0) // did not select or declined
            declinedSelectors.insert(player.group ? player.group : proposalPlayer.first);
        else if (player.answer == -1)
            failedSelectors.insert(player.group ? player.group : proposalPlayer.first);
    }

    LfgUpdateData updateData(declinedSelectors.size() > 0 ? LFG_UPDATETYPE_PROPOSAL_DECLINED : LFG_UPDATETYPE_PROPOSAL_FAILED);
    std::map<ObjectGuid, std::vector<WorldPacket>> personalizedPackets;
    WorldPacket proposalFailed = WorldSession::BuildLfgUpdate(updateData, true);
    for (auto& proposalPlayer : players)
        personalizedPackets[proposalPlayer.first].push_back(proposalFailed);

    updateData.updateType = LFG_UPDATETYPE_REMOVED_FROM_QUEUE;
    std::set<ObjectGuid>& whoToKick = declinedSelectors.size() > 0 ? declinedSelectors : failedSelectors;
    for (auto& queued : queues)
    {
        LFGQueueData& queueData = queue.GetQueueData(queued);
        if (whoToKick.find(queueData.m_ownerGuid) != whoToKick.end())
        {
            // kick from queue
            for (auto& playerInfo : queueData.m_playerInfoPerGuid)
                personalizedPackets[playerInfo.first].push_back(WorldSession::BuildLfgUpdate(updateData, queueData.m_ownerGuid.IsGroup()));
            queueData.SetState(LFG_STATE_FAILED);
        }
        else
        {
            // continue being queued - did nothing wrong
            queueData.SetState(LFG_STATE_QUEUED);
        }
    }

    sWorld.GetMessager().AddMessage([personalizedPackets](World* world)
    {
        world->BroadcastPersonalized(personalizedPackets);
    });

    queue.RemoveProposal(id);
}

void LfgProposal::AcceptProposal(LFGQueue& queue)
{
    bool sendUpdate = state != LFG_PROPOSAL_SUCCESS;
    state = LFG_PROPOSAL_SUCCESS;
    TimePoint joinTime = sWorld.GetCurrentClockTime();

    std::map<ObjectGuid, std::vector<WorldPacket>> personalizedPackets;

    std::map<ObjectGuid, ObjectGuid> m_groupPerPlayer;
    std::map<ObjectGuid, uint8> m_rolesPerPlayer;

    LfgUpdateData updateData = LfgUpdateData(LFG_UPDATETYPE_GROUP_FOUND);
    for (LfgProposalPlayerContainer::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        ObjectGuid pguid = itr->first;
        ObjectGuid gguid = itr->second.group;
        uint32 randomDungeonId = itr->second.group;
        int32 waitTime = -1;

        std::vector<WorldPacket>& packets = personalizedPackets[pguid];

        packets.emplace_back(WorldSession::BuildLfgUpdateProposal(*this, randomDungeonId, pguid));

        LFGQueueData& queueData = queue.GetQueueData(gguid ? gguid : pguid);
        waitTime = int32((joinTime - queueData.GetJoinTime()).count());
        packets.emplace_back(WorldSession::BuildLfgUpdate(updateData, true));
        updateData.updateType = LFG_UPDATETYPE_REMOVED_FROM_QUEUE;
        packets.emplace_back(WorldSession::BuildLfgUpdate(updateData, true));
        packets.emplace_back(WorldSession::BuildLfgUpdate(updateData, false));

        LFGQueuePlayer& playerInfo = queueData.m_playerInfoPerGuid[pguid];
        // Update timers
        uint8 role = playerInfo.m_roles;
        role &= ~PLAYER_ROLE_LEADER;
        switch (role)
        {
            case PLAYER_ROLE_DAMAGE:
                queue.UpdateWaitTimeDps(waitTime, dungeonId);
                break;
            case PLAYER_ROLE_HEALER:
                queue.UpdateWaitTimeHealer(waitTime, dungeonId);
                break;
            case PLAYER_ROLE_TANK:
                queue.UpdateWaitTimeTank(waitTime, dungeonId);
                break;
            default:
                queue.UpdateWaitTimeAvg(waitTime, dungeonId);
                break;
        }

        // Store the number of players that were present in group when joining RFD, used for achievement purposes
        queue.SetPartyMemberCountAtJoin(pguid, queueData.m_playerInfoPerGuid.size());

        m_groupPerPlayer[pguid] = gguid;
        m_rolesPerPlayer[pguid] = playerInfo.m_roles;
        queueData.SetState(LFG_STATE_DUNGEON);
    }

    // Remove players/groups from Queue
    for (GuidList::const_iterator it = queues.begin(); it != queues.end(); ++it)
        queue.RemoveFromQueue(*it);

    queue.RemoveProposal(id);

    sWorld.GetMessager().AddMessage([=](World* world)
    {
        world->BroadcastPersonalized(personalizedPackets);
    });

    GuidList allPlayers, tankPlayers, healPlayers, dpsPlayers;
    GuidList playersToTeleport;
    std::map<ObjectGuid, uint32> randomDungeonPerPlayer;
    std::map<ObjectGuid, uint32> partyCountPerPlayer;

    for (LfgProposalPlayerContainer::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        ObjectGuid guid = itr->first;
        if (guid == leader)
            allPlayers.push_back(guid);
        else
            switch (itr->second.role & ~PLAYER_ROLE_LEADER)
            {
                case PLAYER_ROLE_TANK:
                    tankPlayers.push_back(guid);
                    break;
                case PLAYER_ROLE_HEALER:
                    healPlayers.push_back(guid);
                    break;
                case PLAYER_ROLE_DAMAGE:
                    dpsPlayers.push_back(guid);
                    break;
                default:
                    MANGOS_ASSERT(false); // "Invalid LFG role %u"
                    break;
            }

        if (isNew || m_groupPerPlayer[guid] != group)
            playersToTeleport.push_back(guid);

        randomDungeonPerPlayer[guid] = itr->second.randomDungeonId;
        partyCountPerPlayer[guid] = queue.GetPartyMemberCountAtJoin(guid);
    }

    allPlayers.splice(allPlayers.end(), tankPlayers);
    allPlayers.splice(allPlayers.end(), healPlayers);
    allPlayers.splice(allPlayers.end(), dpsPlayers);

    // Set the dungeon difficulty
    LFGDungeonData const* dungeon = sLFGMgr.GetLFGDungeon(dungeonId);
    MANGOS_ASSERT(dungeon);

    sWorld.GetMessager().AddMessage([allPlayers, group = group, dungeon, playersToTeleport, randomDungeonPerPlayer, partyCountPerPlayer](World* world)
    {
        Group* grp = group ? sObjectMgr.GetGroupById(group.GetCounter()) : nullptr;
        for (GuidList::const_iterator it = allPlayers.begin(); it != allPlayers.end(); ++it)
        {
            ObjectGuid pguid = (*it);
            Player* player = ObjectAccessor::FindPlayer(pguid);
            if (!player)
                continue;

            Group* group = player->GetGroup();
            if (group && group != grp)
                group->RemoveMember(player->GetObjectGuid(), 0);

            if (!grp)
            {
                grp = new Group();
                grp->Create(player->GetObjectGuid(), player->GetName());
                grp->ConvertToLFG();
                ObjectGuid gguid = grp->GetObjectGuid();
                sObjectMgr.AddGroup(grp);
            }
            else if (group != grp)
                grp->AddMember(player->GetObjectGuid(), player->GetName());

            player->GetLfgData().SetCountAtJoin(partyCountPerPlayer.find(pguid)->second);
            player->GetLfgData().SetDungeon(dungeon->id);

            // Add the cooldown spell if queued for a random dungeon
            auto randomItr = randomDungeonPerPlayer.find(player->GetObjectGuid());
            if (uint32 randomDungeonId = randomItr->second)
            {
                LFGDungeonEntry const* dungeonEntry = sLFGDungeonStore.LookupEntry(randomDungeonId);
                if (dungeonEntry && dungeonEntry->TypeID == LFG_TYPE_RANDOM_DUNGEON)
                {
                    player->CastSpell(player, LFG_SPELL_DUNGEON_COOLDOWN, TRIGGERED_OLD_TRIGGERED);
                    player->GetLfgData().SetDungeon(randomDungeonId);
                }
            }
        }

        MANGOS_ASSERT(grp);
        grp->SetDungeonDifficulty(Difficulty(dungeon->difficulty));
        ObjectGuid gguid = grp->GetObjectGuid();
        grp->GetLfgData().SetDungeon(dungeon->id);
        grp->GetLfgData().SetState(LFG_STATE_DUNGEON);

        // _SaveToDB(gguid, grp->GetDbStoreId()); - not saving anything for now

        // Teleport Player
        for (GuidList::const_iterator it = playersToTeleport.begin(); it != playersToTeleport.end(); ++it)
            if (Player* player = ObjectAccessor::FindPlayer(*it))
                sLFGMgr.TeleportPlayer(player, false, false);

        // Update group info
        grp->SendUpdate();
    });
}

void LFGQueueData::RecalculateRoles()
{
    memset(m_roles, 0, sizeof(m_roles));
    for (auto& playerInfo : m_playerInfoPerGuid)
    {
        uint32 i = PLAYER_ROLE_TANK;
        for (uint32 k = 0; i <= PLAYER_ROLE_DAMAGE; i = i << 1, ++k)
            if ((i & playerInfo.second.m_roles) != 0)
                ++m_roles[k];
    }
}

enum RaidBrowserFlags
{
    RAID_BROWSER_FLAG_PLAYER_STATS  = 0x01,
    RAID_BROWSER_FLAG_COMMENT       = 0x02,
    RAID_BROWSER_FLAG_GROUP_LEADER  = 0x04,
    RAID_BROWSER_FLAG_GROUP_GUID    = 0x08,
    RAID_BROWSER_FLAG_ROLES         = 0x10,
    RAID_BROWSER_FLAG_AREAID        = 0x20, // TODO
    RAID_BROWSER_FLAG_STATUS        = 0x40, // TODO
    RAID_BROWSER_FLAG_SAVE          = 0x80, // TODO
};

WorldPacket LfgRaidBrowser::BuildSearchResults(uint32 dungeonId, uint32 team)
{
    ListedContainer& listed = m_listedPerDungeon[{team, dungeonId}];

    // supposed to work akin to object update - at the moment broadcasting everything all the time
    WorldPacket data(SMSG_LFG_SEARCH_RESULTS);

    data << uint32(LFG_TYPE_RAID);  // type
    data << uint32(dungeonId);      // entry from LFGDungeons.dbc

    bool deletedPlayersOrGroups = false;
    data << uint8(deletedPlayersOrGroups);
    if (deletedPlayersOrGroups)
    {
        data << uint32(listed.m_players.size() + listed.m_groups.size());
        for (ObjectGuid guid : listed.m_players)
            data << guid; // player guid
        for (ObjectGuid guid : listed.m_groups)
            data << guid; // group guid
    }

    uint32 groups_count = listed.m_groups.size();
    data << uint32(groups_count);                           // groups count current in packet
    data << uint32(groups_count);                           // groups count total in LFR

    for (ObjectGuid guid : listed.m_groups)
    {
        data << guid;                                       // group guid

        LFGQueueData& queueData = m_listed[guid];

        uint32 flags = RAID_BROWSER_FLAG_ROLES | RAID_BROWSER_FLAG_COMMENT | RAID_BROWSER_FLAG_SAVE;

        data << uint32(flags);                              // flags

        if (flags & RAID_BROWSER_FLAG_COMMENT)
            data << queueData.m_comment;                    // comment string, max len 256

        if (flags & RAID_BROWSER_FLAG_ROLES)
        {
            for (uint32 j = 0; j < 3; ++j)
                data << uint8(queueData.m_roles[j]);        // roles
        }

        if (flags & RAID_BROWSER_FLAG_SAVE)
        {
            if (queueData.m_savedMap.find(dungeonId) != queueData.m_savedMap.end())
            {
                data << uint64(queueData.m_savedMap[dungeonId].instanceGuid);// instance guid
                data << uint32(queueData.m_savedMap[dungeonId].completedEncountersMask); // completed encounters
            }
            else
            {
                data << uint64(0);// instance guid
                data << uint32(0); // completed encounters
            }
        }
    }

    uint32 playersSize = listed.m_players.size();
    data << uint32(playersSize);                            // players count current in packet
    data << uint32(playersSize);                            // players count total in LFR

    for (ObjectGuid guid : listed.m_players)
    {
        ObjectGuid ownerGuid = listed.m_playerToGroup.find(guid) != listed.m_playerToGroup.end() ? listed.m_playerToGroup[guid] : guid;
        LFGQueueData& queueData = m_listed[ownerGuid];
        LFGQueuePlayer const& playerInfo = queueData.m_playerInfoPerGuid[guid];

        data << guid;                                       // guid

        uint32 flags = RAID_BROWSER_FLAG_PLAYER_STATS | RAID_BROWSER_FLAG_ROLES | RAID_BROWSER_FLAG_COMMENT | RAID_BROWSER_FLAG_SAVE | RAID_BROWSER_FLAG_AREAID | RAID_BROWSER_FLAG_SAVE;

        if (queueData.m_ownerGuid.IsGroup())
            flags |= (RAID_BROWSER_FLAG_GROUP_LEADER | RAID_BROWSER_FLAG_GROUP_GUID);

        data << uint32(flags);                              // flags

        if (flags & RAID_BROWSER_FLAG_PLAYER_STATS)
        {
            data << uint8(playerInfo.m_level);
            data << uint8(playerInfo.m_class);
            data << uint8(playerInfo.m_race);

            for (uint32 i = 0; i < 3; ++i)
                data << uint8(0);                           // talent spec x/x/x

            data << uint32(0);                              // armor
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // HasteMelee
            data << uint32(0);                              // HasteRanged
            data << uint32(0);                              // HasteSpell
            data << float(0);                               // MP5
            data << float(0);                               // MP5 Combat
            data << uint32(0);                              // AttackPower
            data << uint32(0);                              // Agility
            data << uint32(0);                              // Health
            data << uint32(0);                              // Mana
            data << uint32(0);                              // Unk1
            data << float(0);                               // Unk2
            data << uint32(0);                              // Defence
            data << uint32(0);                              // Dodge
            data << uint32(0);                              // Block
            data << uint32(0);                              // Parry
            data << uint32(0);                              // Crit
            data << uint32(0);                              // Expertise
        }

        if (flags & RAID_BROWSER_FLAG_COMMENT)
            data << queueData.m_comment;                    // comment

        if (flags & RAID_BROWSER_FLAG_GROUP_LEADER)
            data << uint8(queueData.m_leaderGuid == guid);  // group leader

        if (flags & RAID_BROWSER_FLAG_GROUP_GUID)
            data << uint64(queueData.m_ownerGuid);          // group guid

        if (flags & RAID_BROWSER_FLAG_ROLES)
            data << uint8(playerInfo.m_roles);              // roles

        if (flags & RAID_BROWSER_FLAG_AREAID)
            data << uint32(0);                              // areaid

        if (flags & RAID_BROWSER_FLAG_STATUS)
            data << uint8(0);                               // status

        if (flags & RAID_BROWSER_FLAG_SAVE)
        {
            if (queueData.m_savedMap.find(dungeonId) != queueData.m_savedMap.end())
            {
                data << uint64(queueData.m_savedMap[dungeonId].instanceGuid);// instance guid
                data << uint32(queueData.m_savedMap[dungeonId].completedEncountersMask); // completed encounters
            }
            else
            {
                data << uint64(0);// instance guid
                data << uint32(0); // completed encounters
            }
        }
    }
    return data;
}

void LfgRaidBrowser::AddListener(uint32 dungeonId, uint32 team, ObjectGuid guid)
{
    m_listeners[{team, dungeonId}].push_back(guid);
    if (m_changed[{team, dungeonId}] == false) // if changed will send on next update
    {
        WorldPacket data = BuildSearchResults(dungeonId, team);
        if (Player* plr = ObjectAccessor::FindPlayer(guid))
            plr->GetSession()->SendPacket(data);
    }
}

void LfgRaidBrowser::RemoveListener(uint32 dungeonId, uint32 team, ObjectGuid guid)
{
    auto& listenerGuids = m_listeners[{team, dungeonId}];
    listenerGuids.erase(std::remove(listenerGuids.begin(), listenerGuids.end(), guid), listenerGuids.end());
}

void LfgRaidBrowser::AddListed(LFGQueueData const& data)
{
    auto result = m_listed.emplace(data.m_ownerGuid, data);
    LFGQueueData& queueData = result.first->second;
    if (queueData.m_state == LFG_STATE_RAIDBROWSER)
        ProcessDungeons(queueData.m_dungeons, queueData.m_team, queueData.m_ownerGuid);
    else
        queueData.UpdateRoleCheck(queueData.m_leaderGuid, queueData.m_playerInfoPerGuid[queueData.m_leaderGuid].m_roles, false, false);
}

void LfgRaidBrowser::RemoveListed(ObjectGuid guid)
{
    auto itr = m_listed.find(guid);
    if (itr == m_listed.end())
        return;

    LFGQueueData& data = itr->second;

    for (uint32 dungeonId : data.m_dungeons)
    {
        ListedContainer& dungeonGuids = m_listedPerDungeon[{data.m_team, dungeonId}];
        if (guid.IsPlayer())
            dungeonGuids.m_players.erase(std::remove(dungeonGuids.m_players.begin(), dungeonGuids.m_players.end(), guid), dungeonGuids.m_players.end());
        else
        {
            dungeonGuids.m_groups.erase(std::remove(dungeonGuids.m_groups.begin(), dungeonGuids.m_groups.end(), guid), dungeonGuids.m_groups.end());
            LFGQueueData const& queueData = m_listed[guid];
            for (auto& playerInfo : queueData.m_playerInfoPerGuid)
            {
                dungeonGuids.m_players.push_back(playerInfo.first);
                dungeonGuids.m_playerToGroup.erase(playerInfo.first);
            }
        }            
        m_changed[{data.m_team, dungeonId}] = true;
    }

    m_listed.erase(itr);
}

void LfgRaidBrowser::SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles)
{
    auto itr = m_listed.find(group);
    if (itr != m_listed.end())
        itr->second.UpdateRoleCheck(player, roles, false, false);
}

void LfgRaidBrowser::UpdateComment(ObjectGuid guid, std::string comment)
{
    auto itr = m_listed.find(guid);
    if (itr != m_listed.end())
    {
        itr->second.m_comment = comment;
        for (auto& dungeon : itr->second.m_dungeons)
            m_changed[{itr->second.m_team, dungeon}] = true;
    }
}

void LfgRaidBrowser::Update(World* world)
{
    for (auto& listed : m_listed)
    {
        LFGQueueData const& queueData = listed.second;
        if (queueData.m_state == LFG_STATE_RAIDBROWSER && queueData.m_roleCheckState == LFG_ROLECHECK_FINISHED)
        {
            listed.second.m_roleCheckState = LFG_ROLECHECK_DEFAULT;
            ProcessDungeons(queueData.m_dungeons, queueData.m_team, queueData.m_ownerGuid);
        }
    }

    // runs in world thread - always safe to work with sessions
    for (auto& data : m_changed)
    {
        if (data.second == false) // not changed
            continue;

        uint32 dungeonId = data.first.second;
        uint32 team = data.first.first;
        auto& listeners = m_listeners[{team, dungeonId}];
        if (listeners.empty())
            continue;

        data.second = false; // set to unchanged and process all listeners

        WorldPacket result = BuildSearchResults(dungeonId, Team(team));

        for (ObjectGuid guid : listeners)
        {
            Player* plr = ObjectAccessor::FindPlayer(guid);
            plr->GetSession()->SendPacket(result);
        }
    }
}

void LfgRaidBrowser::ProcessDungeons(LfgDungeonSet const& dungeons, uint32 team, ObjectGuid guid)
{
    for (uint32 dungeonId : dungeons)
    {
        ListedContainer& listed = m_listedPerDungeon[{team, dungeonId}];
        if (guid.IsPlayer())
            listed.m_players.push_back(guid);
        else
        {
            listed.m_groups.push_back(guid);
            LFGQueueData const& queueData = m_listed[guid];
            for (auto& playerInfo : queueData.m_playerInfoPerGuid)
            {
                listed.m_players.push_back(playerInfo.first);
                listed.m_playerToGroup[playerInfo.first] = queueData.m_ownerGuid;
            }
        }
        m_changed[{team, dungeonId}] = true;
    }
}
