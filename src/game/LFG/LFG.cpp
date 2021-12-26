#include "LFG.h"
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

#include "LFG/LFG.h"
#include "World/World.h"

#include <algorithm>

LfgData::LfgData() : m_roles(3)
{
}

WorldPacket LfgRaidBrowser::BuildSearchResults(uint32 dungeonId, Team team)
{
    auto& listedPlayers = m_listedPlayers[{team, dungeonId}];

    WorldPacket data(SMSG_LFG_SEARCH_RESULTS);

    data << uint32(LFG_TYPE_RAID);  // type
    data << uint32(dungeonId);      // entry from LFGDungeons.dbc

    data << uint8(!listedPlayers.empty());
    if (!listedPlayers.empty())
    {
        data << uint32(listedPlayers.size());
        for (ObjectGuid guid : listedPlayers)
            data << guid; // player/group guid
    }

    uint32 groups_count = 0;
    data << uint32(groups_count);                           // groups count
    data << uint32(groups_count);                           // groups count (total?)

    for (uint32 i = 0; i < groups_count; ++i)
    {
        data << uint64(1);                                  // group guid

        uint32 flags = 0x92;
        data << uint32(flags);                              // flags

        if (flags & 0x2)
        {
            data << uint8(0);                               // comment string, max len 256
        }

        if (flags & 0x10)
        {
            for (uint32 j = 0; j < 3; ++j)
                data << uint8(0);                           // roles
        }

        if (flags & 0x80)
        {
            data << uint64(0);                              // instance guid
            data << uint32(0);                              // completed encounters
        }
    }

    uint32 playersSize = listedPlayers.size();
    data << uint32(playersSize);                            // players count
    data << uint32(playersSize);                            // players count (total?)

    for (ObjectGuid guid : listedPlayers)
    {
        Player* player = ObjectAccessor::FindPlayer(guid);

        if (!player) // TODO: fix teams
            continue;

        if (!player->IsInWorld())
            continue;

        auto& lfgData = player->GetLfgData();

        data << player->GetObjectGuid();                    // guid

        uint32 flags = 0xFF;
        data << uint32(flags);                              // flags

        if (flags & 0x1)
        {
            data << uint8(player->GetLevel());
            data << uint8(player->getClass());
            data << uint8(player->getRace());

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

        if (flags & 0x2)
            data << lfgData.GetComment();                   // comment

        if (flags & 0x4)
            data << uint8(0);                               // group leader

        if (flags & 0x8)
            data << uint64(1);                              // group guid

        if (flags & 0x10)
            data << uint8(lfgData.GetPlayerRoles());        // roles

        if (flags & 0x20)
            data << uint32(player->GetZoneId());            // areaid

        if (flags & 0x40)
            data << uint8(0);                               // status

        if (flags & 0x80)
        {
            data << uint64(0);                              // instance guid
            data << uint32(0);                              // completed encounters
        }
    }
    return data;
}

void LfgRaidBrowser::AddListener(uint32 dungeonId, Team team, ObjectGuid guid)
{
    m_listeners[{team, dungeonId}].push_back(guid);
    if (m_changed[{team, dungeonId}] == false) // if changed will send on next update
    {
        WorldPacket data = BuildSearchResults(dungeonId, team);
        if (Player* plr = ObjectAccessor::FindPlayer(guid))
            plr->GetSession()->SendPacket(data);
    }
}

void LfgRaidBrowser::RemoveListener(uint32 dungeonId, Team team, ObjectGuid guid)
{
    auto& listenerGuids = m_listedPlayers[{team, dungeonId}];
	listenerGuids.erase(std::remove(listenerGuids.begin(), listenerGuids.end(), guid), listenerGuids.end());
}

void LfgRaidBrowser::AddPlayer(LfgDungeonSet const& dungeons, Team team, ObjectGuid guid)
{
	for (uint32 dungeonId : dungeons)
	{
        m_listedPlayers[{team, dungeonId}].push_back(guid);
        m_changed[{team, dungeonId}] = true;
	}
}

void LfgRaidBrowser::RemovePlayer(LfgDungeonSet const& dungeons, Team team, ObjectGuid guid)
{
	for (uint32 dungeonId : dungeons)
	{
        auto& dungeonGuids = m_listedPlayers[{team, dungeonId}];
		dungeonGuids.erase(std::remove(dungeonGuids.begin(), dungeonGuids.end(), guid), dungeonGuids.end());
        m_changed[{team, dungeonId}] = true;
	}
}

void LfgRaidBrowser::Update(World* world)
{
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
