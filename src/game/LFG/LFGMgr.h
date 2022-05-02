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

#ifndef _LFG_MGR_H
#define _LFG_MGR_H

#include "Common.h"
#include "Entities/ObjectGuid.h"
#include "Entities/Player.h"
#include "Globals/ObjectMgr.h"
#include "Policies/Singleton.h"
#include "Server/DBCEnums.h"
#include "LFG/LFGDefines.h"

#include <map>

struct LFGDungeonEntry;

struct LFGQueuePlayer;
typedef std::map<ObjectGuid, LFGQueuePlayer> LfgPlayerInfoMap;

 /// Reward info
struct LfgReward
{
    LfgReward(uint32 maxLevel = 0, uint32 firstQuest = 0, uint32 otherQuest = 0) :
        maxLevel(maxLevel), firstQuest(firstQuest), otherQuest(otherQuest) { }

    uint32 maxLevel;
    uint32 firstQuest;
    uint32 otherQuest;
};

struct LFGDungeonExpansionData
{
    LFGDungeonExpansionData(uint8 minLevel, uint8 maxLevel) : minLevel(minLevel), maxLevel(maxLevel) {}

    uint8 minLevel;
    uint8 maxLevel;
};

struct LFGDungeonData
{
    LFGDungeonData();
    LFGDungeonData(LFGDungeonEntry const* dbc);

    uint32 id;
    std::string name;
    uint16 map;
    uint8 type;
    uint8 expansion;
    uint8 group;
    Difficulty difficulty;
    bool seasonal;
    float x, y, z, o;

    std::map<uint8, LFGDungeonExpansionData> expansionData;

    // Helpers
    uint32 Entry() const { return id + (type << 24); }
    bool CheckMinLevel(uint8 expansion, uint8 playerLevel) const;
    bool CheckMaxLevel(uint8 expansion, uint8 playerLevel) const;

    private:
        uint8 minlevel;
        uint8 maxlevel;
};

class LFGMgr
{
    public:
        void LoadRewards();
        void LoadLFGDungeons(bool reload = false);

        // Checks if Seasonal dungeon is active
        bool IsSeasonActive(uint32 dungeonId) const;

        // Return Lfg dungeon entry for given dungeon id
        uint32 GetLFGDungeonEntry(uint32 id);
        // Gets the random dungeon reward corresponding to given dungeon and player level
        LfgReward const* GetRandomDungeonReward(uint32 dungeon, uint8 level);
        // Returns all random and seasonal dungeons for given level and expansion
        LfgDungeonSet GetRandomAndSeasonalDungeons(uint8 level, uint8 expansion) const;
        /// Get locked dungeons
        LfgLockMap const GetLockedDungeons(Player* player);

        LfgDungeonSet const& GetDungeonsByRandom(uint32 randomdungeon);

        // Helper functions
        LfgType GetDungeonType(uint32 dungeonId);

        // Attempts to join LFD or LFR
        void JoinLfg(Player* player, uint8 roles, LfgDungeonSet& dungeons, std::string& comment);
        void LeaveLfg(Player* player);

        static uint32 FilterClassRoles(uint32 roles, uint32 classMask);
        static bool CheckGroupRoles(LfgPlayerInfoMap& groles);

        void TeleportPlayer(Player* player, bool out, bool fromOpcode);

        LFGDungeonData const* GetLFGDungeon(uint32 id);

        void FinishDungeon(Group* group, uint32 dungeonId, Map const* currMap);
    private:
        void GetCompatibleDungeons(LfgDungeonSet& dungeons, GuidSet const& players, LfgLockPartyMap& lockMap, LfgInstanceSaveMap& savedMap, bool isContinue);

        std::unordered_map<uint32, LFGDungeonData> m_lfgDungeons;
        std::multimap<uint32, LfgReward> m_lfgRewards;
        std::map<uint32, LfgDungeonSet> m_cachedDungeonMapsPerGroup;
};

#define sLFGMgr MaNGOS::Singleton<LFGMgr>::Instance()

#endif