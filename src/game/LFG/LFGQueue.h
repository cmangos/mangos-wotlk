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

#ifndef _LFG_QUEUE_H
#define _LFG_QUEUE_H

#include "Common.h"
#include "LFG/LFGDefines.h"
#include "Multithreading/Messager.h"
#include "WorldPacket.h"

#include <string>

class World;

struct LFGQueuePlayer
{
	uint32 m_level;
	uint32 m_class;
	uint32 m_race;
	uint32 m_roles;
};

typedef std::map<ObjectGuid, LFGQueuePlayer> LfgPlayerInfoMap;

struct LFGQueueData
{
	LfgState m_state;
	TimePoint m_joinTime;
	TimePoint m_queueTime;
	uint8 m_roles[ROLE_INDEX_COUNT];
	uint32 m_randomDungeonId;
	LfgDungeonSet m_dungeons;
	LfgPlayerInfoMap m_playerInfoPerGuid;
	ObjectGuid m_ownerGuid; // either player or group
	ObjectGuid m_leaderGuid; // for group only
	TimePoint m_cancelTime; // cancel time on rolecheck
	LfgRoleCheckState m_roleCheckState;
	GuidVector m_groupGuids;
	LfgInstanceSaveMap m_savedMap;
	bool m_raid;
	uint32 m_team;
	std::string m_comment;

	LFGQueueData() { memset(m_roles, 0, sizeof(m_roles)); }

	void RecalculateRoles();
	void UpdateRoleCheck(ObjectGuid guid, uint8 roles, bool abort);
};

/*
 * intended to live in its own thread - must not access anything from the outside that is mutable
 * prototyping for being able to separate certain processes from world thread context entirely
 */
class LFGQueue
{
	public:
		void AddToQueue(LFGQueueData const& data);
		void RemoveFromQueue(ObjectGuid owner);
		void SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles);

		void Update();

		std::string GetDebugPrintout();

		Messager<LFGQueue>& GetMessager() { return m_messager; }
	private:

		std::map<ObjectGuid, LFGQueueData> m_queueData;
		std::vector<LFGQueueData*> m_sortedQueue; // sorted by time

		Messager<LFGQueue> m_messager;
};

struct ListedContainer
{
	std::vector<ObjectGuid> m_players;
	std::vector<ObjectGuid> m_groups;
	std::map<ObjectGuid, ObjectGuid> m_playerToGroup;
};

class LfgRaidBrowser
{
    public:
        WorldPacket BuildSearchResults(uint32 dungeonId, uint32 team);

        void AddListener(uint32 dungeonId, uint32 team, ObjectGuid guid);
        void RemoveListener(uint32 dungeonId, uint32 team, ObjectGuid guid);

        void AddListed(LFGQueueData const& data);
        void RemoveListed(ObjectGuid guid);
		void SetPlayerRoles(ObjectGuid group, ObjectGuid player, uint8 roles);
		void UpdateComment(ObjectGuid guid, std::string comment);

        void Update(World* world);
    private:
		void ProcessDungeons(LfgDungeonSet const& dungeons, uint32 team, ObjectGuid guid);

		std::map<ObjectGuid, LFGQueueData> m_listed;
        std::map<std::pair<uint32, uint32>, ListedContainer> m_listedPerDungeon;
		std::map<std::pair<uint32, uint32>, bool> m_changed;
		std::map<std::pair<uint32, uint32>, std::vector<ObjectGuid>> m_listeners;
};

#endif