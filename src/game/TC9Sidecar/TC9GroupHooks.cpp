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

#include "TC9GroupHooks.h"
#include "Log.h"
#include "Groups/Group.h"
#include "Globals/ObjectMgr.h"

void ToCloud9GroupHooks::OnGroupCreated(EventObjectGroup *group)
{
    DEBUG_LOG("Group created. ID: %d; Leader: %d.", group->guid, group->leader);

    Group* g = new Group();
    g->m_Id = group->guid;
    g->m_leaderGuid = ObjectGuid(group->leader);
    g->m_dungeonDifficulty = Difficulty(group->difficulty);
    g->m_raidDifficulty = Difficulty(group->raidDifficulty);
    g->m_lootMethod = LootMethod(group->lootMethod);
    g->m_lootThreshold = ItemQualities(group->lootThreshold);
    g->m_masterLooterGuid = ObjectGuid(group->masterLooterGuid);
    g->m_groupFlags = GroupType(group->groupType);

    for (int i = 0; i < group->membersSize; i++)
        g->_addMember(ObjectGuid(group->members[i]), "");

    sObjectMgr.AddGroup(g);
}

void ToCloud9GroupHooks::OnGroupDisbanded(uint32 group)
{
    DEBUG_LOG("Group disbanded. ID: %d.", group);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->_disband(true);
}

void ToCloud9GroupHooks::OnGroupMemberAdded(uint32 group, uint64 member)
{
    DEBUG_LOG("Group member added. ID: %d; Member: %d.", group, member);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->_addMember(ObjectGuid(member), "");
}

void ToCloud9GroupHooks::OnGroupMemberRemoved(uint32 group, uint64 member, uint64 newLeader)
{
    DEBUG_LOG("Group member removed. ID: %d; Member: %d; NewLeader: %d.", group, member, newLeader);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->RemoveMember(ObjectGuid(member), 0);
}

void ToCloud9GroupHooks::OnGroupLootTypeChanged(uint32 group, uint8 lootType, uint64 looter, uint8 lootThreshold)
{
    DEBUG_LOG("Group loot type changed. ID: %d; LootType: %d; Looter: %d; LootThreshold: %d.",
             group, lootType, looter, lootThreshold);

    if (Group* g = sObjectMgr.GetGroupById(group))
    {
        g->SetLootMethod((LootMethod)lootType);
        g->SetMasterLooterGuid(ObjectGuid(looter));
        g->SetLootThreshold((ItemQualities)lootThreshold);
    }
}


void ToCloud9GroupHooks::OnGroupConvertedToRaid(uint32 group)
{
    DEBUG_LOG("Group converted to raid. ID: %d.", group);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->ConvertToRaid();
}

void ToCloud9GroupHooks::OnGroupRaidDifficultyChanged(uint32 group, uint8 difficulty)
{
    DEBUG_LOG("Raid difficulty changed. ID: %d; Difficulty: %d.", group, difficulty);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->SetRaidDifficulty((Difficulty)difficulty);
}

void ToCloud9GroupHooks::OnGroupDungeonDifficultyChanged(uint32 group, uint8 difficulty)
{
    DEBUG_LOG("Dungeon difficulty changed. ID: %d; Difficulty: %d.", group, difficulty);

    if (Group* g = sObjectMgr.GetGroupById(group))
        g->SetDungeonDifficulty((Difficulty)difficulty);
}
