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
class LFGQueue;

struct LFGQueuePlayer
{
    uint32 m_level;
    uint32 m_class;
    uint32 m_race;
    uint32 m_roles;
};

typedef std::map<ObjectGuid, LFGQueuePlayer> LfgPlayerInfoMap;

struct LfgProposalPlayer
{
    LfgProposalPlayer() : role(0), answer(LFG_ANSWER_PENDING), group() { }
    LfgProposalPlayer(uint8 role, LfgAnswer answer, ObjectGuid group, uint32 randomDungeonId) : role(role), answer(answer), group(group), randomDungeonId(randomDungeonId) {}
    uint8 role;                                            // Proposed role
    LfgAnswer answer;                                      // Accept status (-1 not answer | 0 Not agree | 1 agree)
    ObjectGuid group;                                      // Original group guid. 0 if no original group
    uint32 randomDungeonId;                                // Filled if player joined dungeonId as random dungeon 
};

typedef std::map<ObjectGuid, LfgProposalPlayer> LfgProposalPlayerContainer;

/// Stores group data related to proposal to join
struct LfgProposal
{
    LfgProposal(uint32 dungeon = 0) : id(0), dungeonId(dungeon), state(LFG_PROPOSAL_INITIATING),
        group(), leader(), encounters(0), isNew(true)
    { }

    uint32 id;                                             /// Proposal Id
    uint32 dungeonId;                                      /// Dungeon to join
    LfgProposalState state;                                /// State of the proposal
    ObjectGuid group;                                      /// Proposal group (0 if new)
    ObjectGuid leader;                                     /// Leader guid.
    TimePoint cancelTime;                                  /// Time when we will cancel this proposal
    uint32 encounters;                                     /// Dungeon Encounters
    bool isNew;                                            /// Determines if it's new group or not
    GuidList queues;                                       /// Queue Ids to remove/readd
    GuidList showorder;                                    /// Show order in update window
    LfgProposalPlayerContainer players;                    /// Players data

    void UpdatePlayerProposal(ObjectGuid guid, bool accept);

    void UpdateProposal(LFGQueue& queue);
    void FailProposal(LFGQueue& queue);
    void AcceptProposal(LFGQueue& queue);
};

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
    void UpdateRoleCheck(ObjectGuid guid, uint8 roles, bool abort, bool timeout);
    void PopQueue(LfgProposal& proposal);
    LfgDungeonSet GetDungeons() const;
    LfgState GetState() const { return m_state; }
    void SetState(LfgState state) { m_state = state; }
    TimePoint GetJoinTime() const { return m_joinTime; }
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
        void UpdateProposal(ObjectGuid playerGuid, uint32 proposalId, bool accept);
        void RemoveProposal(uint32 proposalId);

        void OnPlayerLogout(ObjectGuid guid, ObjectGuid groupGuid);

        LFGQueueData& GetQueueData(ObjectGuid owner) { return m_queueData[owner]; }

        void Update();

        std::string GetDebugPrintout();

        Messager<LFGQueue>& GetMessager() { return m_messager; }

        void ToggleTesting() { m_testing = true; }
        bool IsTestingEnabled() const { return m_testing; }

        void SetPartyMemberCountAtJoin(ObjectGuid guid, uint32 count) { m_numberOfPartyMembersAtJoin[guid] = count; }
        uint32 GetPartyMemberCountAtJoin(ObjectGuid guid) const;

        void UpdateWaitTimeDps(int32 time, uint32 dungeonId);
        void UpdateWaitTimeHealer(int32 time, uint32 dungeonId);
        void UpdateWaitTimeTank(int32 time, uint32 dungeonId);
        void UpdateWaitTimeAvg(int32 time, uint32 dungeonId);
    private:

        std::map<ObjectGuid, LFGQueueData> m_queueData;
        std::vector<LFGQueueData*> m_sortedQueue; // sorted by time

        Messager<LFGQueue> m_messager;

        bool m_testing = false;

        std::map<uint32, LfgProposal> m_proposals;
        std::vector<uint32> m_proposalsForRemoval;

        std::map<ObjectGuid, uint32> m_numberOfPartyMembersAtJoin;
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