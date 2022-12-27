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

#include "Maps/InstanceData.h"
#include "Database/DatabaseEnv.h"
#include "Maps/Map.h"
#include "Log.h"
#include "Server/WorldPacket.h"

void InstanceData::SaveToDB() const
{
    // no reason to save BGs/Arenas
    if (instance->IsBattleGroundOrArena())
        return;

    if (!Save())
        return;

    std::string data = Save();
    CharacterDatabase.escape_string(data);

    if (instance->Instanceable())
        CharacterDatabase.PExecute("UPDATE instance SET data = '%s' WHERE id = '%u'", data.c_str(), instance->GetInstanceId());
    else
        CharacterDatabase.PExecute("UPDATE world SET data = '%s' WHERE map = '%u'", data.c_str(), instance->GetId());
}

bool InstanceData::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= nullptr*/, uint32 /*miscvalue1*/ /*= 0*/) const
{
    sLog.outError("Achievement system call InstanceData::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
                  instance->GetId(), criteria_id);
    return false;
}

bool InstanceData::CheckConditionCriteriaMeet(Player const* /*source*/, uint32 instance_condition_id, WorldObject const* /*conditionSource*/, uint32 conditionSourceType) const
{
    sLog.outError("Condition system call InstanceData::CheckConditionCriteriaMeet but instance script for map %u not have implementation for player condition criteria with internal id %u (called from %u)",
                  instance->GetId(), instance_condition_id, uint32(conditionSourceType));
    return false;
}

void InstanceData::SendEncounterFrame(uint32 type, ObjectGuid sourceGuid /*= nullptr*/, uint8 param1 /*= 0*/, uint8 param2 /*= 0*/) const
{
    // size of this packet is at most 15 (usually less)
    WorldPacket data(SMSG_INSTANCE_ENCOUNTER, 15);
    data << uint32(type);

    switch (type)
    {
        case ENCOUNTER_FRAME_ENGAGE:
        case ENCOUNTER_FRAME_DISENGAGE:
        case ENCOUNTER_FRAME_UPDATE_PRIORITY:
            MANGOS_ASSERT(sourceGuid);

            data << sourceGuid.WriteAsPacked();
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_ADD_TIMER:
        case ENCOUNTER_FRAME_ENABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_DISABLE_OBJECTIVE:
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UPDATE_OBJECTIVE:
            data << uint8(param1);
            data << uint8(param2);
            break;
        case ENCOUNTER_FRAME_UNK7:
        default:
            break;
    }

    instance->SendToPlayers(data);
}
