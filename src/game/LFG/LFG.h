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

#ifndef _LFG_H
#define _LFG_H

#include "Common.h"
#include "LFG/LFGDefines.h"
#include "WorldPacket.h"
#include "Globals/SharedDefines.h"

#include <string>

class World;

class LFGData
{
    public:
        LFGData();
        LfgState GetState() const { return m_state; }
        void SetState(LfgState state) { m_state = state; }
        uint32 GetDungeon() const { return m_dungeon; }
        void SetDungeon(uint32 dungeon) { m_dungeon = dungeon; }
        std::string GetComment() const { return m_comment; }
        void SetComment(std::string comment) { m_comment = comment; }
        LfgDungeonSet& GetListedDungeonSet() { return m_listedDungeonSet; }
        // players store everything in index 0 - groups use all values for counts
        void SetPlayerRoles(uint8 roles) { m_roles[ROLE_INDEX_PLAYER] = roles; }
        uint8 GetPlayerRoles() const { return m_roles[ROLE_INDEX_PLAYER]; }
        void SetCountAtJoin(uint8 count) { m_countAtJoin = count; }
        uint8 GetCountAtJoin() const { return m_countAtJoin; }
    private:
        LfgState m_state;
        uint32 m_dungeon;
        std::string m_comment;
        LfgDungeonSet m_listedDungeonSet;
        std::vector<uint8> m_roles;
        uint8 m_countAtJoin;
};

#endif