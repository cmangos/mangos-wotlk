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
		queueData.UpdateRoleCheck(queueData.m_leaderGuid, queueData.m_playerInfoPerGuid[queueData.m_leaderGuid].m_roles, false);
}

void LFGQueue::RemoveFromQueue(ObjectGuid owner)
{
	m_queueData.erase(owner);
}

void LFGQueue::SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles)
{
    auto itr = m_queueData.find(group);
    if (itr != m_queueData.end())
        itr->second.UpdateRoleCheck(player, roles, false);
}

void LFGQueue::Update()
{
	while (!World::IsStopped())
	{
		GetMessager().Execute(this);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

std::string LFGQueue::GetDebugPrintout()
{
	return std::string();
}

void LFGQueueData::UpdateRoleCheck(ObjectGuid guid, uint8 roles, bool abort)
{
    LfgPlayerInfoMap check_roles;

    bool sendRoleChosen = m_roleCheckState != LFG_ROLECHECK_DEFAULT && abort;

    if (abort)
        m_roleCheckState = LFG_ROLECHECK_ABORTED;
    else if (m_playerInfoPerGuid.empty())                            // Player selected no role.
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
                m_raid ? LFG_STATE_RAIDBROWSER : LFG_STATE_NONE, dungeons, m_comment), true));
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
        queueData.UpdateRoleCheck(queueData.m_leaderGuid, queueData.m_playerInfoPerGuid[queueData.m_leaderGuid].m_roles, false);
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
        itr->second.UpdateRoleCheck(player, roles, false);
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
