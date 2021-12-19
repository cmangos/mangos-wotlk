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
#include "World/World.h"
#include "Server/DBCStores.h"
#include "GameEvents/GameEventMgr.h"
#include "LFG/LFGDefines.h"


LFGDungeonData::LFGDungeonData() : id(0), name(), map(0), type(0), expansion(0), group(0), minlevel(0),
    maxlevel(0), difficulty(REGULAR_DIFFICULTY), seasonal(false), x(0.0f), y(0.0f), z(0.0f), o(0.0f)
{
}

LFGDungeonData::LFGDungeonData(LFGDungeonEntry const* dbc) : id(dbc->ID), name(dbc->Name[0]), map(dbc->MapID),
    type(dbc->TypeID), expansion(uint8(dbc->ExpansionLevel)), group(uint8(dbc->GroupID)),
    minlevel(uint8(dbc->MinLevel)), maxlevel(uint8(dbc->MaxLevel)), difficulty(Difficulty(dbc->Difficulty)),
    seasonal((dbc->Flags & LFG_FLAG_HOLIDAY) != 0), x(0.0f), y(0.0f), z(0.0f), o(0.0f)
{
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
        case 285: // The Headless Horseman
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_HALLOWS_END);
        case 286: // The Frost Lord Ahune
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_FIRE_FESTIVAL);
        case 287: // Coren Direbrew
            return sGameEventMgr.IsActiveHoliday(HOLIDAY_BREWFEST);
        case 288: // The Crown Chemical Co.
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
            && dungeon.expansion <= expansion && dungeon.minlevel <= level && level <= dungeon.maxlevel)
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
        else if (dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL && player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
            lockData = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (dungeon->minlevel > level)
            lockData = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (dungeon->maxlevel < level)
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
