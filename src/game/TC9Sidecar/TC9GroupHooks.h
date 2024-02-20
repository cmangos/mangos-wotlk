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

#ifndef _TC9_GROUP_HOOKS_H
#define _TC9_GROUP_HOOKS_H

#include "libsidecar.h"

class ToCloud9GroupHooks
{
public:
    ToCloud9GroupHooks() {};
    ~ToCloud9GroupHooks() {};

    static void OnGroupCreated(EventObjectGroup *group);
    static void OnGroupDisbanded(uint32 group);
    static void OnGroupMemberAdded(uint32 group, uint64 member);
    static void OnGroupMemberRemoved(uint32 group, uint64 member, uint64 newLeader);
    static void OnGroupLootTypeChanged(uint32 group, uint8 lootType, uint64 looter, uint8 lootThreshold);
    static void OnGroupConvertedToRaid(uint32 group);
    static void OnGroupRaidDifficultyChanged(uint32 group, uint8 difficulty);
    static void OnGroupDungeonDifficultyChanged(uint32 group, uint8 difficulty);
};


#endif /* TC9GroupHooks_h */
