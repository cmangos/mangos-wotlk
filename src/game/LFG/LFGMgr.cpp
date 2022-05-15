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

#include "LFGMgr.h"
#include "Globals/ObjectMgr.h"
#include "World/World.h"
#include "Server/DBCStores.h"
#include "GameEvents/GameEventMgr.h"
#include "LFG/LFGDefines.h"
#include "LFG/LFGQueue.h"

std::string ConcatenateDungeons(LfgDungeonSet const& dungeons)
{
    std::string dungeonstr = "";
    if (!dungeons.empty())
    {
        std::ostringstream o;
        auto itr = dungeons.begin();
        o << (*itr);
        for (++itr; itr != dungeons.end(); ++itr)
            o << ", " << uint32(*itr);
        dungeonstr = o.str();
    }
    return dungeonstr;
}


LFGDungeonData::LFGDungeonData() : id(0), name(), map(0), type(0), expansion(0), group(0), minlevel(0),
    maxlevel(0), difficulty(REGULAR_DIFFICULTY), seasonal(false), x(0.0f), y(0.0f), z(0.0f), o(0.0f)
{
}

LFGDungeonData::LFGDungeonData(LFGDungeonEntry const* dbc) : id(dbc->ID), name(dbc->Name[0]), map(dbc->MapID),
    type(dbc->TypeID), expansion(uint8(dbc->ExpansionLevel)), group(uint8(dbc->GroupID)),
    minlevel(uint8(dbc->MinLevel)), maxlevel(uint8(dbc->MaxLevel)), difficulty(Difficulty(dbc->Difficulty)),
    seasonal((dbc->Flags & LFG_FLAG_HOLIDAY) != 0), x(0.0f), y(0.0f), z(0.0f), o(0.0f)
{
    for (uint32 i = 1; i < sLFGDungeonExpansionStore.GetNumRows(); ++i)
    {
        if (LfgDungeonExpansionEntry const* entry = sLFGDungeonExpansionStore.LookupEntry(i))
        {
            if (entry->LFGID == id)
            {
                expansionData.emplace(entry->ExpansionLevel, LFGDungeonExpansionData(entry->MinLevel, entry->MaxLevel));
            }
        }
    }
}

bool LFGDungeonData::CheckMinLevel(uint8 expansion, uint8 playerLevel) const
{
    auto itr = expansionData.find(expansion);
    if (itr != expansionData.end())
        return itr->second.minLevel <= playerLevel;

    return minlevel <= playerLevel;
}

bool LFGDungeonData::CheckMaxLevel(uint8 expansion, uint8 playerLevel) const
{
    auto itr = expansionData.find(expansion);
    if (itr != expansionData.end())
        return itr->second.maxLevel >= playerLevel;

    return maxlevel >= playerLevel;
}

void LFGMgr::LoadRewards()
{
    m_lfgRewards.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT dungeonId, maxLevel, firstQuestId, otherQuestId FROM lfg_dungeon_rewards ORDER BY dungeonId, maxLevel ASC"));

    uint32 count = 0;
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u lfg_dungeon_rewards", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();
        Field* fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        uint32 maxLevel = fields[1].GetUInt8();
        uint32 firstQuestId = fields[2].GetUInt32();
        uint32 otherQuestId = fields[3].GetUInt32();

        if (!GetLFGDungeonEntry(dungeonId))
        {
            sLog.outErrorDb("Dungeon %u specified in table `lfg_dungeon_rewards` does not exist!", dungeonId);
            continue;
        }

        if (!maxLevel || maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        {
            sLog.outErrorDb("Level %u specified for dungeon %u in table `lfg_dungeon_rewards` can never be reached!", maxLevel, dungeonId);
            maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
        }

        if (!firstQuestId || !sObjectMgr.GetQuestTemplate(firstQuestId))
        {
            sLog.outErrorDb("First quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", firstQuestId, dungeonId);
            continue;
        }

        if (otherQuestId && !sObjectMgr.GetQuestTemplate(otherQuestId))
        {
            sLog.outErrorDb("Other quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", otherQuestId, dungeonId);
            otherQuestId = 0;
        }

        LfgReward reward;
        reward.maxLevel = maxLevel;
        reward.firstQuest = firstQuestId;
        reward.otherQuest = otherQuestId;
        m_lfgRewards.emplace(dungeonId, reward);
        ++count;
    } while (result->NextRow());

    sLog.outString(">> Loaded %u lfg_dungeon_rewards", count);
    sLog.outString();
}

void LFGMgr::LoadLFGDungeons(bool reload)
{
    m_lfgDungeons.clear();

    // Initialize Dungeon map with data from dbcs
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon)
            continue;

        switch (dungeon->TypeID)
        {
            case LFG_TYPE_DUNGEON:
            case LFG_TYPE_HEROIC_DUNGEON:
            case LFG_TYPE_RAID:
            case LFG_TYPE_RANDOM_DUNGEON:
                m_lfgDungeons[dungeon->ID] = LFGDungeonData(dungeon);
                break;
        }
    }

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT dungeonId, position_x, position_y, position_z, orientation FROM lfg_dungeon_template"));

    uint32 count = 0;
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u lfg_dungeon_template", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();
        Field* fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        auto itr = m_lfgDungeons.find(dungeonId);
        if (itr == m_lfgDungeons.end())
        {
            sLog.outErrorDb("table `lfg_dungeon_template` contains coordinates for wrong dungeon %u", dungeonId);
            continue;
        }

        LFGDungeonData& data = itr->second;
        data.x = fields[1].GetFloat();
        data.y = fields[2].GetFloat();
        data.z = fields[3].GetFloat();
        data.o = fields[4].GetFloat();

        ++count;
    } while (result->NextRow());

    // Fill all other teleport coords from areatriggers
    for (auto itr = m_lfgDungeons.begin(); itr != m_lfgDungeons.end(); ++itr)
    {
        LFGDungeonData& dungeon = itr->second;

        // No teleport coords in database, load from areatriggers
        if (dungeon.type != LFG_TYPE_RANDOM_DUNGEON && dungeon.x == 0.0f && dungeon.y == 0.0f && dungeon.z == 0.0f)
        {
            AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(dungeon.map);
            if (!at)
            {
                sLog.outErrorDb("Failed to load dungeon %s, cant find areatrigger for map %u", dungeon.name.c_str(), dungeon.map);
                continue;
            }

            dungeon.map = at->target_mapId;
            dungeon.x = at->target_X;
            dungeon.y = at->target_Y;
            dungeon.z = at->target_Z;
            dungeon.o = at->target_Orientation;
        }

        if (dungeon.type != LFG_TYPE_RANDOM_DUNGEON)
            m_cachedDungeonMapsPerGroup[dungeon.group].insert(dungeon.id);
        m_cachedDungeonMapsPerGroup[0].insert(dungeon.id);
    }
}

bool LFGMgr::IsSeasonActive(uint32 dungeonId) const
{
    switch (dungeonId)
    {
        case SEASONAL_HEADLESS_HORSEMAN: // The Headless Horseman
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_HALLOWS_END);
        case SEASONAL_AHUNE: // The Frost Lord Ahune
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_FIRE_FESTIVAL);
        case SEASONAL_COREN_DIREBREW: // Coren Direbrew
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_BREWFEST);
        case SEASONAL_CROWN_CHEMICAL_CO: // The Crown Chemical Co.
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_LOVE_IS_IN_THE_AIR);
    }
    return false;
}

LFGDungeonData const* LFGMgr::GetLFGDungeon(uint32 id)
{
    auto itr = m_lfgDungeons.find(id);
    if (itr != m_lfgDungeons.end())
        return &(itr->second);

    return nullptr;
}

// intended to be an in-place method that does not alter global state in any way
void LFGMgr::FinishDungeon(Group* group, uint32 dungeonId, Map const* currMap)
{
    LFGData& groupLfgData = group->GetLfgData();
    uint32 gDungeonId = groupLfgData.GetDungeon();
    if (gDungeonId != dungeonId)
    {
        sLog.outDebug("Group %s finished dungeon %u but queued for %u", group->GetObjectGuid().GetString().c_str(), dungeonId, gDungeonId);
        return;
    }

    if (groupLfgData.GetState() == LFG_STATE_FINISHED_DUNGEON) // Shouldn't happen. Do not reward multiple times
    {
        sLog.outDebug("Group: %s already rewarded", group->GetObjectGuid().GetString().c_str());
        return;
    }

    groupLfgData.SetState(LFG_STATE_FINISHED_DUNGEON);

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* player = itr->getSource();
        ObjectGuid guid = player->GetObjectGuid();
        LFGData& playerLfgData = player->GetLfgData();
        if (playerLfgData.GetState() == LFG_STATE_FINISHED_DUNGEON)
        {
            sLog.outDebug("Group: %s, Player: %s already rewarded", group->GetObjectGuid().GetString().c_str(), guid.GetString().c_str());
            continue;
        }

        uint32 rDungeonId = playerLfgData.GetDungeon();

        playerLfgData.SetState(LFG_STATE_FINISHED_DUNGEON);

        // Give rewards only if its a random dungeon
        LFGDungeonData const* dungeon = GetLFGDungeon(rDungeonId);

        if (!dungeon || (dungeon->type != LFG_TYPE_RANDOM_DUNGEON && !dungeon->seasonal))
        {
            sLog.outDebug("Group: %s, Player: %s dungeon %u is not random or seasonal", group->GetObjectGuid().GetString().c_str(), guid.GetString().c_str(), rDungeonId);
            continue;
        }

        if (player->GetMap() != currMap)
        {
            sLog.outDebug("Group: %s, Player: %s is in a different map", group->GetObjectGuid().GetString().c_str(), guid.GetString().c_str());
            continue;
        }

        player->RemoveAurasDueToSpell(LFG_SPELL_DUNGEON_COOLDOWN);

        LFGDungeonData const* dungeonDone = GetLFGDungeon(dungeonId);
        uint32 mapId = dungeonDone ? uint32(dungeonDone->map) : 0;

        if (player->GetMapId() != mapId)
        {
            sLog.outDebug("Group: %s, Player: %s is in map %u and should be in %u to get reward", group->GetObjectGuid().GetString().c_str(), guid.GetString().c_str(), player->GetMapId(), mapId);
            continue;
        }

        // Update achievements
        if (dungeon->difficulty == DUNGEON_DIFFICULTY_HEROIC)
        {
            uint8 lfdRandomPlayers = 0;
            if (uint8 numParty = playerLfgData.GetCountAtJoin())
                lfdRandomPlayers = 5 - numParty;
            else
                lfdRandomPlayers = 4;
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, lfdRandomPlayers);
        }

        LfgReward const* reward = GetRandomDungeonReward(rDungeonId, player->GetLevel());
        if (!reward)
            continue;

        bool done = false;
        Quest const* quest = sObjectMgr.GetQuestTemplate(reward->firstQuest);
        if (!quest)
            continue;

        // if we can take the quest, means that we haven't done this kind of "run", IE: First Heroic Random of Day.
        if (player->CanRewardQuest(quest, false))
            player->RewardQuest(quest, 0, nullptr, false);
        else
        {
            done = true;
            quest = sObjectMgr.GetQuestTemplate(reward->otherQuest);
            if (!quest)
                continue;
            // we give reward without informing client (retail does this)
            player->RewardQuest(quest, 0, nullptr, false);
        }

        // Give rewards
        sLog.outDebug("Group: %s, Player: %s done dungeon %u, %s previously done.", group->GetObjectGuid().GetString().c_str(), guid.GetString().c_str(), gDungeonId, done ? " " : " not");
        LfgPlayerRewardData data = LfgPlayerRewardData(dungeon->Entry(), dungeonDone->Entry(), done, quest);
        player->GetSession()->SendLfgPlayerReward(data);
    }
}

/**
   Given a list of dungeons remove the dungeons players have restrictions.

   @param[in, out] dungeons Dungeons to check restrictions
   @param[in]     players Set of players to check their dungeon restrictions
   @param[out]    lockMap Map of players Lock status info of given dungeons (Empty if dungeons is not empty)
*/
void LFGMgr::GetCompatibleDungeons(LfgDungeonSet& dungeons, GuidSet const& players, LfgLockPartyMap& lockMap, LfgInstanceSaveMap& savedMap, bool isContinue)
{
    lockMap.clear();

    for (GuidSet::const_iterator it = players.begin(); it != players.end() && !dungeons.empty(); ++it)
    {
        ObjectGuid guid = (*it);
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
            continue;
        LfgLockMap const& cachedLockMap = GetLockedDungeons(player);
        for (LfgLockMap::const_iterator it2 = cachedLockMap.begin(); it2 != cachedLockMap.end() && !dungeons.empty(); ++it2)
        {
            uint32 dungeonId = (it2->first & 0x00FFFFFF); // Compare dungeon ids
            LfgDungeonSet::iterator itDungeon = dungeons.find(dungeonId);
            if (itDungeon != dungeons.end())
            {
                bool eraseDungeon = true;

                // Don't remove the dungeon if team members are trying to continue a locked instance
                if (it2->second == LFG_LOCKSTATUS_RAID_LOCKED && isContinue)
                {
                    LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId);
                    MANGOS_ASSERT(dungeon);
                    MANGOS_ASSERT(player);
                    if (InstancePlayerBind* playerBind = player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
                    {
                        if (DungeonPersistentState* state = playerBind->state)
                        {
                            ObjectGuid dungeonInstanceGuid = state->GetInstanceGuid();
                            auto itLockedDungeon = savedMap.find(dungeonId);
                            if (itLockedDungeon == savedMap.end() || itLockedDungeon->second.instanceGuid == dungeonInstanceGuid)
                                eraseDungeon = false;

                            auto& lockedDungeon = savedMap[dungeonId];
                            lockedDungeon.instanceGuid = dungeonInstanceGuid;
                            lockedDungeon.completedEncountersMask = state->GetCompletedEncountersMask();
                        }
                    }
                }

                if (eraseDungeon)
                    dungeons.erase(itDungeon);

                lockMap[guid][dungeonId] = it2->second;
            }
        }
    }
    if (!dungeons.empty())
        lockMap.clear();
}

uint32 LFGMgr::GetLFGDungeonEntry(uint32 id)
{
    if (id)
        if (LFGDungeonData const* dungeon = GetLFGDungeon(id))
            return dungeon->Entry();

    return 0;
}

LfgReward const* LFGMgr::GetRandomDungeonReward(uint32 dungeon, uint8 level)
{
    LfgReward const* rew = nullptr;
    auto bounds = m_lfgRewards.equal_range(dungeon & 0x00FFFFFF);
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        rew = &itr->second;
        // ordered properly at loading
        if (itr->second.maxLevel >= level)
            break;
    }

    return rew;
}

LfgDungeonSet LFGMgr::GetRandomAndSeasonalDungeons(uint8 level, uint8 expansion) const
{
    LfgDungeonSet randomDungeons;
    for (auto itr = m_lfgDungeons.begin(); itr != m_lfgDungeons.end(); ++itr)
    {
        auto& dungeon = itr->second;
        if ((dungeon.type == LFG_TYPE_RANDOM_DUNGEON || (dungeon.seasonal && IsSeasonActive(dungeon.id)))
            && dungeon.expansion <= expansion && dungeon.CheckMinLevel(expansion, level) && dungeon.CheckMaxLevel(expansion, level))
            randomDungeons.insert(dungeon.Entry());
    }
    return randomDungeons;
}

LfgLockMap const LFGMgr::GetLockedDungeons(Player* player)
{
    LfgLockMap lock;

    uint8 level = player->GetLevel();
    uint8 expansion = player->GetSession()->GetExpansion();
    LfgDungeonSet const& dungeons = GetDungeonsByRandom(0);
    bool denyJoin = false; // TODO: Implement disabling LFG in anticheat

    for (LfgDungeonSet::const_iterator it = dungeons.begin(); it != dungeons.end(); ++it)
    {
        LFGDungeonData const* dungeon = GetLFGDungeon(*it);
        if (!dungeon) // should never happen - We provide a list from sLFGDungeonStore
            continue;

        uint32 lockData = 0;
        if (denyJoin)
            lockData = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (dungeon->expansion > expansion)
            lockData = LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        //else if (DisableMgr::IsDisabledFor(DISABLE_TYPE_MAP, dungeon->map, player))
        //    lockData = LFG_LOCKSTATUS_RAID_LOCKED;
        //else if (DisableMgr::IsDisabledFor(DISABLE_TYPE_LFG_MAP, dungeon->map, player))
        //    lockData = LFG_LOCKSTATUS_RAID_LOCKED;
        else if ((dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL || dungeon->group >= 6)
            && player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
            lockData = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (!dungeon->CheckMinLevel(expansion, level))
            lockData = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (!dungeon->CheckMaxLevel(expansion, level))
            lockData = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
        else if (dungeon->seasonal && !IsSeasonActive(dungeon->id))
            lockData = LFG_LOCKSTATUS_NOT_IN_SEASON;
        else if (AccessRequirement const* ar = sObjectMgr.GetAccessRequirement(dungeon->map, Difficulty(dungeon->difficulty)))
        {
            if (ar->item_level && player->GetAverageItemLevel() < ar->item_level)
                lockData = LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE;
            else if (ar->achievement && !player->GetAchievementMgr().HasAchievement(ar->achievement))
                lockData = LFG_LOCKSTATUS_MISSING_ACHIEVEMENT;
            else if (player->GetTeam() == ALLIANCE && ar->quest_A && !player->GetQuestRewardStatus(ar->quest_A))
                lockData = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else if (player->GetTeam() == HORDE && ar->quest_H && !player->GetQuestRewardStatus(ar->quest_H))
                lockData = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else
                if (ar->item)
                {
                    if (!player->HasItemCount(ar->item, 1) && (!ar->item2 || !player->HasItemCount(ar->item2, 1)))
                        lockData = LFG_LOCKSTATUS_MISSING_ITEM;
                }
                else if (ar->item2 && !player->HasItemCount(ar->item2, 1))
                    lockData = LFG_LOCKSTATUS_MISSING_ITEM;
        }

        /* @todo VoA closed if WG is not under team control (LFG_LOCKSTATUS_RAID_LOCKED)
        lockData = LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE;
        lockData = LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL;
        lockData = LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL;
        */

        if (lockData)
            lock[dungeon->Entry()] = lockData;
    }

    return lock;

    return lock;
}

LfgDungeonSet const& LFGMgr::GetDungeonsByRandom(uint32 randomdungeon)
{
    LFGDungeonData const* dungeon = GetLFGDungeon(randomdungeon);
    uint32 group = dungeon ? dungeon->group : 0;
    return m_cachedDungeonMapsPerGroup[group];
}

LfgType LFGMgr::GetDungeonType(uint32 dungeonId)
{
    LFGDungeonData const* dungeon = GetLFGDungeon(dungeonId);
    if (!dungeon)
        return LFG_TYPE_NONE;

    return LfgType(dungeon->type);
}

void LFGMgr::JoinLfg(Player* player, uint8 roles, LfgDungeonSet& dungeons, std::string& comment)
{
    if (!player)
        return;

    // At least 1 role must be selected
    if (!(roles & (PLAYER_ROLE_TANK | PLAYER_ROLE_HEALER | PLAYER_ROLE_DAMAGE)))
        return;

    // Sanitize input roles
    roles = FilterClassRoles(roles, player->getClassMask());

    Group* grp = player->GetGroup();
    ObjectGuid guid = player->GetObjectGuid();
    ObjectGuid gguid = grp ? grp->GetObjectGuid() : guid;
    LfgJoinResultData joinData;
    GuidSet players;
    uint32 rDungeonId = 0;
    bool isContinue = grp && grp->IsLFGGroup() && grp->GetLfgData().GetState() != LFG_STATE_FINISHED_DUNGEON;

    // Do not allow to change dungeon in the middle of a current dungeon
    if (isContinue)
    {
        dungeons.clear();
        dungeons.insert(grp->GetLfgData().GetDungeon());
    }

    // Already in queue?
    LfgState state = grp ? grp->GetLfgData().GetState() : player->GetLfgData().GetState();
    if (state == LFG_STATE_QUEUED)
    {
        ObjectGuid ownerGuid = grp ? grp->GetObjectGuid() : player->GetObjectGuid();
        sWorld.GetLFGQueue().GetMessager().AddMessage([ownerGuid](LFGQueue* queue)
        {
            queue->RemoveFromQueue(ownerGuid);
        });
    }

    // Check player or group member restrictions
    if (player->InBattleGround() || player->InArena() || player->InBattleGroundQueue())
        joinData.result = LFG_JOIN_USING_BG_SYSTEM;
    else if (player->HasAura(LFG_SPELL_DUNGEON_DESERTER))
        joinData.result = LFG_JOIN_DESERTER;
    else if (!isContinue && player->HasAura(LFG_SPELL_DUNGEON_COOLDOWN))
        joinData.result = LFG_JOIN_RANDOM_COOLDOWN;
    else if (dungeons.empty())
        joinData.result = LFG_JOIN_NOT_MEET_REQS;
    else if (player->HasAura(9454)) // check Freeze debuff
        joinData.result = LFG_JOIN_NOT_MEET_REQS;
    else if (grp)
    {
        if (grp->GetMembersCount() > MAX_GROUP_SIZE)
            joinData.result = LFG_JOIN_TOO_MUCH_MEMBERS;
        else
        {
            uint8 memberCount = 0;
            for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr && joinData.result == LFG_JOIN_OK; itr = itr->next())
            {
                if (Player* groupPlayer = itr->getSource())
                {
                    if (groupPlayer->HasAura(LFG_SPELL_DUNGEON_DESERTER))
                        joinData.result = LFG_JOIN_PARTY_DESERTER;
                    else if (!isContinue && groupPlayer->HasAura(LFG_SPELL_DUNGEON_COOLDOWN))
                        joinData.result = LFG_JOIN_PARTY_RANDOM_COOLDOWN;
                    else if (groupPlayer->InBattleGround() || groupPlayer->InArena() || groupPlayer->InBattleGroundQueue())
                        joinData.result = LFG_JOIN_USING_BG_SYSTEM;
                    else if (groupPlayer->HasAura(9454)) // check Freeze debuff
                        joinData.result = LFG_JOIN_PARTY_NOT_MEET_REQS;
                    ++memberCount;
                    players.insert(groupPlayer->GetObjectGuid());
                }
            }

            if (joinData.result == LFG_JOIN_OK && memberCount != grp->GetMembersCount())
                joinData.result = LFG_JOIN_DISCONNECTED;
        }
    }
    else
        players.insert(player->GetObjectGuid());

    LfgInstanceSaveMap savedMap;

    // Check if all dungeons are valid
    bool isRaid = false;
    if (joinData.result == LFG_JOIN_OK)
    {
        bool isDungeon = false;
        for (LfgDungeonSet::const_iterator it = dungeons.begin(); it != dungeons.end() && joinData.result == LFG_JOIN_OK; ++it)
        {
            LfgType type = GetDungeonType(*it);
            switch (type)
            {
                case LFG_TYPE_RANDOM_DUNGEON:
                    if (dungeons.size() > 1)               // Only allow 1 random dungeon
                        joinData.result = LFG_JOIN_DUNGEON_INVALID;
                    else
                        rDungeonId = (*dungeons.begin());
                    [[fallthrough]]; // Random can only be dungeon or heroic dungeon
                case LFG_TYPE_HEROIC_DUNGEON:
                case LFG_TYPE_DUNGEON:
                    if (isRaid)
                        joinData.result = LFG_JOIN_MIXED_RAID_DUNGEON;
                    isDungeon = true;
                    break;
                case LFG_TYPE_RAID:
                    if (isDungeon)
                        joinData.result = LFG_JOIN_MIXED_RAID_DUNGEON;
                    isRaid = true;
                    break;
                default:
                    joinData.result = LFG_JOIN_DUNGEON_INVALID;
                    break;
            }
        }

        // it could be changed
        if (joinData.result == LFG_JOIN_OK)
        {
            // Expand random dungeons and check restrictions
            if (rDungeonId)
                dungeons = GetDungeonsByRandom(rDungeonId);

            // if we have lockmap then there are no compatible dungeons
            GetCompatibleDungeons(dungeons, players, joinData.lockmap, savedMap, isContinue || isRaid);
            if (dungeons.empty())
                joinData.result = grp ? LFG_JOIN_PARTY_NOT_MEET_REQS : LFG_JOIN_NOT_MEET_REQS;
        }
    }

    // Can't join. Send result
    if (joinData.result != LFG_JOIN_OK)
    {
        DEBUG_LOG("%s joining with %u members. Result: %u, Dungeons: %s",
            guid.GetString().c_str(), grp ? grp->GetMembersCount() : 1, joinData.result, ConcatenateDungeons(dungeons).c_str());

        if (!dungeons.empty())                             // Only should show lockmap when have no dungeons available
            joinData.lockmap.clear();
        player->GetSession()->SendPacket(WorldSession::BuildLfgJoinResult(joinData));
        return;
    }

    if (grp)
        grp->GetLfgData().SetComment(comment);
    else
        player->GetLfgData().SetComment(comment);

    LFGQueueData queueData;
    if (isRaid)
    {
        queueData.m_savedMap = savedMap;
        if (grp)
        {
            // Rolecheck for raid browser does not seem to be possible and client receives group roles but does not display them
            grp->GetLfgData().SetState(LFG_STATE_RAIDBROWSER);
            WorldPacket data = WorldSession::BuildLfgJoinResult(joinData);
            grp->BroadcastPacket(data, false);
            data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_JOIN_QUEUE, LFG_STATE_RAIDBROWSER, dungeons, comment), true);
            grp->BroadcastPacket(data, false);
            queueData.m_state = LFG_STATE_RAIDBROWSER;
            queueData.m_ownerGuid = grp->GetObjectGuid();
            queueData.m_leaderGuid = player->GetObjectGuid();
            queueData.m_joinTime = player->GetMap()->GetCurrentClockTime();
            queueData.m_cancelTime = player->GetMap()->GetCurrentClockTime() + std::chrono::seconds(45);
            queueData.m_roleCheckState = LFG_ROLECHECK_DEFAULT;
            queueData.m_dungeons = dungeons;
            queueData.m_randomDungeonId = rDungeonId;
            for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                ObjectGuid groupMember = (*itr).getSource()->GetObjectGuid();
                queueData.m_groupGuids.push_back(groupMember);
                queueData.m_playerInfoPerGuid[groupMember].m_roles = (*itr).getSource()->GetLfgData().GetPlayerRoles();
                queueData.m_playerInfoPerGuid[groupMember].m_level = (*itr).getSource()->GetLevel();
                queueData.m_playerInfoPerGuid[groupMember].m_race = (*itr).getSource()->getRace();
                queueData.m_playerInfoPerGuid[groupMember].m_class = (*itr).getSource()->getClass();
                player->GetLfgData().SetState(LFG_STATE_QUEUED);
            }
            queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_roles = roles;
            queueData.m_raid = true;
            queueData.m_team = player->GetTeam();
        }
        else
        {
            player->GetSession()->SendPacket(WorldSession::BuildLfgJoinResult(joinData));
            WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_JOIN_QUEUE, dungeons, comment), false);
            player->GetSession()->SendPacket(data);
            auto& dungeonSet = player->GetLfgData().GetListedDungeonSet();
            dungeonSet = dungeons;
            player->GetLfgData().SetState(LFG_STATE_RAIDBROWSER);
            player->GetLfgData().SetPlayerRoles(roles);

            queueData.m_state = LFG_STATE_RAIDBROWSER;
            queueData.m_ownerGuid = player->GetObjectGuid();
            queueData.m_joinTime = player->GetMap()->GetCurrentClockTime();
            queueData.m_dungeons = dungeons;
            queueData.m_randomDungeonId = 0;
            queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_roles = roles;
            queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_level = player->GetLevel();
            queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_class = player->getClass();
            queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_race = player->getRace();
            queueData.m_raid = true;
            queueData.m_team = player->GetTeam();
        }

        sWorld.GetMessager().AddMessage([queueData](World* world)
        {
            world->GetRaidBrowser().AddListed(queueData);
        });
        return;
    }

    if (grp)
    {
        grp->GetLfgData().SetState(LFG_STATE_ROLECHECK);
        queueData.m_state = LFG_STATE_ROLECHECK;
        queueData.m_ownerGuid = grp->GetObjectGuid();
        queueData.m_leaderGuid = player->GetObjectGuid();
        queueData.m_joinTime = player->GetMap()->GetCurrentClockTime();
        queueData.m_cancelTime = player->GetMap()->GetCurrentClockTime() + std::chrono::seconds(45);
        queueData.m_roleCheckState = LFG_ROLECHECK_INITIALITING;
        queueData.m_dungeons = dungeons;
        queueData.m_randomDungeonId = rDungeonId;
        for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* player = (*itr).getSource();
            ObjectGuid groupMember = player->GetObjectGuid();
            queueData.m_groupGuids.push_back(groupMember);
            queueData.m_playerInfoPerGuid[groupMember].m_roles = 0;
            queueData.m_playerInfoPerGuid[groupMember].m_level = (*itr).getSource()->GetLevel();
            player->GetLfgData().SetState(LFG_STATE_QUEUED);
        }
        queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_roles = roles;
        queueData.m_raid = false;
        // cross node broadcasts
        WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_JOIN_QUEUE, dungeons, comment), true);
        grp->BroadcastPacket(data, false);
    }
    else
    {
        player->GetSession()->SendPacket(WorldSession::BuildLfgJoinResult(joinData));
        WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_JOIN_QUEUE, dungeons, comment), false);
        player->GetSession()->SendPacket(data);

        queueData.m_state = LFG_STATE_QUEUED;
        queueData.m_ownerGuid = player->GetObjectGuid();
        queueData.m_joinTime = player->GetMap()->GetCurrentClockTime();
        queueData.m_dungeons = dungeons;
        queueData.m_randomDungeonId = rDungeonId;
        queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_roles = roles;
        queueData.m_playerInfoPerGuid[player->GetObjectGuid()].m_level = player->GetLevel();
        queueData.m_raid = false;

        player->GetLfgData().SetState(LFG_STATE_QUEUED);
    }
    sWorld.GetLFGQueue().GetMessager().AddMessage([queueData](LFGQueue* queue)
    {
        queue->AddToQueue(queueData);
    });
}

void LFGMgr::LeaveLfg(Player* player)
{
    if (!player)
        return;

    Group* grp = player->GetGroup();

    LfgState state = grp ? grp->GetLfgData().GetState() : player->GetLfgData().GetState();
    if (state == LFG_STATE_RAIDBROWSER)
    {
        if (grp)
        {
            grp->GetLfgData().GetListedDungeonSet().clear();
            WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, grp->GetLfgData().GetListedDungeonSet(), ""), true);
            grp->BroadcastPacket(data, false);
            sWorld.GetMessager().AddMessage([guid = grp->GetObjectGuid()](World* world)
            {
                world->GetRaidBrowser().RemoveListed(guid);
            });
            grp->GetLfgData().SetState(LFG_STATE_NONE);
        }
        else
        {
            player->GetLfgData().GetListedDungeonSet().clear();
            WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, player->GetLfgData().GetListedDungeonSet(), ""), false);
            player->GetSession()->SendPacket(data);
            sWorld.GetMessager().AddMessage([guid = player->GetObjectGuid()](World* world)
            {
                world->GetRaidBrowser().RemoveListed(guid);
            });
            player->GetLfgData().SetState(LFG_STATE_NONE);
        }
        return;
    }

    if (grp)
    {
        grp->GetLfgData().GetListedDungeonSet().clear();
        WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, grp->GetLfgData().GetListedDungeonSet(), ""), true);
        grp->BroadcastPacket(data, false);
        grp->GetLfgData().SetState(LFG_STATE_NONE);
        sWorld.GetLFGQueue().GetMessager().AddMessage([guid = grp->GetObjectGuid()](LFGQueue* queue)
        {
            queue->RemoveFromQueue(guid);
        });
    }
    else
    {
        player->GetLfgData().GetListedDungeonSet().clear();
        WorldPacket data = WorldSession::BuildLfgUpdate(LfgUpdateData(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, player->GetLfgData().GetListedDungeonSet(), ""), false);
        player->GetSession()->SendPacket(data);
        sWorld.GetLFGQueue().GetMessager().AddMessage([guid = player->GetObjectGuid()](LFGQueue* queue)
        {
            queue->RemoveFromQueue(guid);
        });
        player->GetLfgData().SetState(LFG_STATE_NONE);
    }
}

enum LfgRoleClasses {
    TANK = (1 << (CLASS_WARRIOR - 1)) |
           (1 << (CLASS_PALADIN - 1)) |
           (1 << (CLASS_DEATH_KNIGHT - 1)) |
           (1 << (CLASS_DRUID - 1)),

    HEALER = (1 << (CLASS_PALADIN - 1)) |
             (1 << (CLASS_PRIEST - 1)) |
             (1 << (CLASS_SHAMAN - 1)) |
             (1 << (CLASS_DRUID - 1)),
};

uint32 LFGMgr::FilterClassRoles(uint32 roles, uint32 classMask)
{
    roles &= PLAYER_ROLE_ANY;
    if (!(LfgRoleClasses::TANK & classMask))
        roles &= ~PLAYER_ROLE_TANK;
    if (!(LfgRoleClasses::HEALER & classMask))
        roles &= ~PLAYER_ROLE_HEALER;
    return roles;
}

bool LFGMgr::CheckGroupRoles(LfgPlayerInfoMap& groles)
{
    if (groles.empty())
        return false;

    uint8 damage = 0;
    uint8 tank = 0;
    uint8 healer = 0;

    for (auto itr = groles.begin(); itr != groles.end(); ++itr)
    {
        uint8 role = itr->second.m_roles & ~PLAYER_ROLE_LEADER;
        if (role == PLAYER_ROLE_NONE)
            return false;

        if (role & PLAYER_ROLE_DAMAGE)
        {
            if (role != PLAYER_ROLE_DAMAGE)
            {
                itr->second.m_roles -= PLAYER_ROLE_DAMAGE;
                if (CheckGroupRoles(groles))
                    return true;
                itr->second.m_roles += PLAYER_ROLE_DAMAGE;
            }
            else if (damage == LFG_DPS_NEEDED)
                return false;
            else
                damage++;
        }

        if (role & PLAYER_ROLE_HEALER)
        {
            if (role != PLAYER_ROLE_HEALER)
            {
                itr->second.m_roles -= PLAYER_ROLE_HEALER;
                if (CheckGroupRoles(groles))
                    return true;
                itr->second.m_roles += PLAYER_ROLE_HEALER;
            }
            else if (healer == LFG_HEALERS_NEEDED)
                return false;
            else
                healer++;
        }

        if (role & PLAYER_ROLE_TANK)
        {
            if (role != PLAYER_ROLE_TANK)
            {
                itr->second.m_roles -= PLAYER_ROLE_TANK;
                if (CheckGroupRoles(groles))
                    return true;
                itr->second.m_roles += PLAYER_ROLE_TANK;
            }
            else if (tank == LFG_TANKS_NEEDED)
                return false;
            else
                tank++;
        }
    }
    return (tank + healer + damage) == uint8(groles.size());
}

/**
   Teleports the player in or out the dungeon

   @param[in]     player Player to teleport
   @param[in]     out Teleport out (true) or in (false)
   @param[in]     fromOpcode Function called from opcode handlers? (Default false)
*/
void LFGMgr::TeleportPlayer(Player* player, bool out, bool fromOpcode /*= false*/)
{
    LFGDungeonData const* dungeon = nullptr;
    Group* group = player->GetGroup();

    if (group && group->IsLFGGroup())
        dungeon = GetLFGDungeon(group->GetLfgData().GetDungeon());

    if (!dungeon)
    {
        sLog.outDebug("Player %s not in group/lfggroup or dungeon not found!", player->GetName());
        player->GetSession()->SendLfgTeleportError(uint8(LFG_TELEPORTERROR_INVALID_LOCATION));
        return;
    }

    if (out)
    {
        sLog.outDebug("Player %s is being teleported out. Current Map %u - Expected Map %u", player->GetName(), player->GetMapId(), uint32(dungeon->map));
        if (player->GetMapId() == uint32(dungeon->map))
            player->TeleportToBGEntryPoint();

        return;
    }

    LfgTeleportError error = LFG_TELEPORTERROR_OK;

    if (!player->IsAlive())
        error = LFG_TELEPORTERROR_PLAYER_DEAD;
    else if (player->IsFalling() || player->hasUnitState(UNIT_STAT_PROPELLED))
        error = LFG_TELEPORTERROR_FALLING;
    else if (player->IsMirrorTimerActive(MirrorTimer::FATIGUE))
        error = LFG_TELEPORTERROR_FATIGUE;
    else if (player->GetVehicleInfo())
        error = LFG_TELEPORTERROR_IN_VEHICLE;
    else if (player->GetCharm())
        error = LFG_TELEPORTERROR_CHARMING;
    else if (player->HasAura(9454)) // check Freeze debuff
        error = LFG_TELEPORTERROR_INVALID_LOCATION;
    else if (player->GetMapId() != uint32(dungeon->map))  // Do not teleport players in dungeon to the entrance
    {
        uint32 mapid = dungeon->map;
        float x = dungeon->x;
        float y = dungeon->y;
        float z = dungeon->z;
        float orientation = dungeon->o;

        if (!fromOpcode)
        {
            // Select a player inside to be teleported to
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* plrg = itr->getSource();
                if (plrg && plrg != player && plrg->GetMapId() == uint32(dungeon->map))
                {
                    mapid = plrg->GetMapId();
                    x = plrg->GetPositionX();
                    y = plrg->GetPositionY();
                    z = plrg->GetPositionZ();
                    orientation = plrg->GetOrientation();
                    break;
                }
            }
        }

        if (!player->GetMap()->IsDungeon())
            player->SetBattleGroundEntryPoint();

        player->TaxiFlightInterrupt(true);

        if (!player->TeleportTo(mapid, x, y, z, orientation))
            error = LFG_TELEPORTERROR_INVALID_LOCATION;
    }
    else
        error = LFG_TELEPORTERROR_INVALID_LOCATION;

    if (error != LFG_TELEPORTERROR_OK)
        player->GetSession()->SendLfgTeleportError(uint8(error));

    sLog.outDebug("Player %s is being teleported in to map %u (x: %f, y: %f, z: %f) Result: %u", player->GetName(), dungeon->map, dungeon->x, dungeon->y, dungeon->z, error);
}
