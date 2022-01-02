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
	auto& result = m_queueData.emplace(data.m_ownerGuid, data);
    auto& queueData = result.first->second;
	if (data.m_roleCheckState == LFG_ROLECHECK_INITIALITING)
		UpdateRoleCheck(queueData, queueData.m_leaderGuid, queueData.m_playerInfoPerGuid[queueData.m_leaderGuid].m_roles, false);
}

void LFGQueue::RemoveFromQueue(ObjectGuid owner)
{
	m_queueData.erase(owner);
}

void LFGQueue::SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles)
{
    auto itr = m_queueData.find(group);
    if (itr != m_queueData.end())
        UpdateRoleCheck(itr->second, player, roles, false);
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

void LFGQueue::UpdateRoleCheck(LFGQueueData& data, ObjectGuid guid, uint8 roles, bool abort)
{
    LfgPlayerInfoMap check_roles;

    bool sendRoleChosen = data.m_roleCheckState != LFG_ROLECHECK_DEFAULT && abort;

    if (abort)
        data.m_roleCheckState = LFG_ROLECHECK_ABORTED;
    else if (data.m_playerInfoPerGuid.empty())                            // Player selected no role.
        data.m_roleCheckState = LFG_ROLECHECK_NO_ROLE;
    else
    {
        data.m_playerInfoPerGuid[guid].m_roles = roles;

        // Check if all players have selected a role
        auto itrRoles = data.m_playerInfoPerGuid.begin();
        while (itrRoles != data.m_playerInfoPerGuid.end() && itrRoles->second.m_roles != PLAYER_ROLE_NONE)
            ++itrRoles;

        if (itrRoles == data.m_playerInfoPerGuid.end())
        {
            // use temporal var to check roles, CheckGroupRoles modifies the roles
            check_roles = data.m_playerInfoPerGuid;
            data.m_roleCheckState = LFGMgr::CheckGroupRoles(check_roles) ? LFG_ROLECHECK_FINISHED : LFG_ROLECHECK_WRONG_ROLES;
        }
    }

    LfgDungeonSet dungeons = data.m_dungeons;

    LfgJoinResultData joinData = LfgJoinResultData(LFG_JOIN_FAILED, data.m_roleCheckState);

    std::vector<WorldPacket> packets;
    if (sendRoleChosen)
        packets.emplace_back(WorldSession::BuildLfgRoleChosen(guid, roles));

    packets.emplace_back(WorldSession::BuildLfgRoleCheckUpdate(data));
	switch (data.m_roleCheckState)
	{
		case LFG_ROLECHECK_INITIALITING:
			break;
		case LFG_ROLECHECK_FINISHED:
            packets.emplace_back(WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_ADDED_TO_QUEUE, dungeons, ""), true));
			break;
		default:
			packets.emplace_back(WorldSession::BuildLfgJoinResult(joinData));
            packets.emplace_back(WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_ROLECHECK_FAILED), true));
			break;
	}

    sWorld.GetMessager().AddMessage([groupGuid = data.m_ownerGuid, packets](World* world)
    {
        world->BroadcastToGroup(groupGuid, packets);
    });

    if (data.m_roleCheckState == LFG_ROLECHECK_FINISHED)
    {
        data.m_state = LFG_STATE_QUEUED;
        data.m_queueTime = sWorld.GetCurrentClockTime();
        data.m_roleCheckState = LFG_ROLECHECK_FINISHED;
    }
    else if (data.m_roleCheckState != LFG_ROLECHECK_INITIALITING)
        data.m_state = LFG_STATE_FAILED;
}
