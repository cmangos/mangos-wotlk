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

#ifndef MANGOS_INSTANCE_DATA_H
#define MANGOS_INSTANCE_DATA_H

#include "Common.h"
#include "Entities/ObjectGuid.h"
#include "ByteBuffer.h"

class Map;
class Unit;
class Player;
class GameObject;
class Creature;
class WorldObject;
class ChatHandler;
class CreatureGroup;

enum InstanceConditionIDs                                   // Suggested values used with CONDITION_INSTANCE_SCRIPT for some generic uses
{
    // for hard-mode loot (0 normal; 1,2... hard,harder... mode)
    INSTANCE_CONDITION_ID_NORMAL_MODE       = 0,
    INSTANCE_CONDITION_ID_HARD_MODE         = 1,
    INSTANCE_CONDITION_ID_HARD_MODE_2       = 2,
    INSTANCE_CONDITION_ID_HARD_MODE_3       = 3,
    INSTANCE_CONDITION_ID_HARD_MODE_4       = 4,
    INSTANCE_CONDITION_ID_HARD_MODE_5       = 5,
    INSTANCE_CONDITION_ID_HARD_MODE_6       = 6,

    // to check for which team the instance is doing scripts
    INSTANCE_CONDITION_ID_TEAM_HORDE        = 67,
    INSTANCE_CONDITION_ID_TEAM_ALLIANCE     = 469,

    // to check water event in SSC
    INSTANCE_CONDITION_ID_LURKER            = 21217,
    INSTANCE_CONDITION_ID_SCALDING_WATER    = 37284,

    INSTANCE_CONDITION_ID_SOCRETHAR_GOSSIP  = 8215,

    // Gossip conditions for Hyjal Summit
    INSTANCE_CONDITION_ID_WINTERCHILL_STARTABLE = 17767,
    INSTANCE_CONDITION_ID_ANETHERON_STARTABLE = 17808,
    INSTANCE_CONDITION_ID_ANETHERON_DONE = 17772,
    INSTANCE_CONDITION_ID_KAZROGAL_STARTABLE = 17888,
    INSTANCE_CONDITION_ID_AZGALOR_STARTABLE = 17842,
    INSTANCE_CONDITION_ID_AZGALOR_DONE = 17852,
    INSTANCE_CONDITION_ID_ARCHIMONDE_STARTABLE = 17968,
    INSTANCE_CONDITION_ID_ARCHIMONDE_DONE = 17948,
    INSTANCE_CONDITION_ID_WAVES_INPROGRESS = 17895,

    INSTANCE_CONDITION_ID_SAAT_BEACON               = 20201,

    INSTANCE_CONDITION_ID_BASHIR_FLYING             = 87361,
    INSTANCE_CONDITION_ID_BASHIR_IN_PROGRESS        = 87362,
    
    // Shade of the Horseman village attack event
    INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_GOLDSHIRE   = 18000, // must be first and all subsequent numbers matching ShadeOfTheHorsemanVillages enum
    INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_KHARANOS    = 18001,
    INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_AZURE_WATCH = 18002,
    INSTANCE_CONDITION_ID_FIRE_TRAINING_BRILL               = 18003,
    INSTANCE_CONDITION_ID_FIRE_TRAINING_RAZOR_HILL          = 18004,
    INSTANCE_CONDITION_ID_FIRE_TRAINING_FALCONWING          = 18005,
    INSTANCE_CONDITION_ID_STOP_THE_FIRES_ALLIANCE           = 18006,
    INSTANCE_CONDITION_ID_STOP_THE_FIRES_HORDE              = 18007,
    INSTANCE_CONDITION_ID_THE_HEADLESS_HORSEMAN_ALLIANCE    = 18008,
    INSTANCE_CONDITION_ID_THE_HEADLESS_HORSEMAN_HORDE       = 18009,
    INSTANCE_CONDITION_ID_LET_THE_FIRES_COME_ALLIANCE       = 18010,
    INSTANCE_CONDITION_ID_LET_THE_FIRES_COME_HORDE          = 18011,

    // to check vehicles in Ulduar
    INSTANCE_CONDITION_ID_ULDUAR            = 33113,

    // Icecrown citadel
    INSTANCE_CONDITION_ID_INNER_SPIRE_TELEPORT      = 63101,
};

enum EncounterFrameTypes                                    // only raid UI specific
{
    ENCOUNTER_FRAME_ENGAGE                  = 0,
    ENCOUNTER_FRAME_DISENGAGE               = 1,
    ENCOUNTER_FRAME_UPDATE_PRIORITY         = 2,
    ENCOUNTER_FRAME_ADD_TIMER               = 3,
    ENCOUNTER_FRAME_ENABLE_OBJECTIVE        = 4,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE        = 5,
    ENCOUNTER_FRAME_DISABLE_OBJECTIVE       = 6,
    ENCOUNTER_FRAME_UNK7                    = 7             // sorting encounter units
};

class InstanceData
{
    public:

        explicit InstanceData(Map* map) : instance(map) {}
        virtual ~InstanceData() {}

        Map* instance;

        // On creation, NOT load.
        virtual void Initialize() {}

        // On load
        virtual void Load(const char* /*data*/) {}

        // When save is needed, this function generates the data
        virtual const char* Save() const { return ""; }

        void SaveToDB() const;

        // Called every map update
        virtual void Update(const uint32 /*diff*/) {}

        // This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress() const { return false; };

        // Called when a player successfully enters the instance (after really added to map)
        virtual void OnPlayerEnter(Player*) {}

        // Called when a player dies inside instance
        virtual void OnPlayerDeath(Player*) {}

        // Called when a player rezzurects inside instance
        virtual void OnPlayerResurrect(Player*) {}

        // Called when a player leaves the instance (before really removed from map (or possibly world))
        virtual void OnPlayerLeave(Player*) {}

        // Called when a gameobject is created
        virtual void OnObjectCreate(GameObject*) {}

        // Called when a gameobject is spawned into map
        virtual void OnObjectSpawn(GameObject*) {}

        // called on creature creation
        virtual void OnCreatureCreate(Creature* /*creature*/) {}

        // called on creature spawn/respawn
        virtual void OnCreatureRespawn(Creature* /*creature*/) {}

        // called on creature enter combat
        virtual void OnCreatureEnterCombat(Creature* /*creature*/) {}

        // called on creature evade
        virtual void OnCreatureEvade(Creature* /*creature*/) {}

        // called on creature death
        virtual void OnCreatureDeath(Creature* /*creature*/) {}

        // called on creature despawn
        virtual void OnCreatureDespawn(Creature* /*creature*/) {}

        // called on creature group last member despawn or death - whichever occurs first and supplies last one to die
        virtual void OnCreatureGroupDespawn(CreatureGroup* /*creatureGroup*/, Creature* creature) {}

        // called on game event
        virtual void OnEventHappened(uint16 /*event_id*/, bool /*activate*/, bool /*resume*/) {}

        // All-purpose data storage 64 bit
        virtual uint64 GetData64(uint32 /*Data*/) const { return 0; }
        virtual void SetData64(uint32 /*Data*/, uint64 /*Value*/) { }

        // Guid data storage (wrapper for set/get from uint64 storage
        ObjectGuid GetGuid(uint32 dataIdx) const { return ObjectGuid(GetData64(dataIdx)); }
        void SetGuid(uint32 dataIdx, ObjectGuid value) { SetData64(dataIdx, value.GetRawValue()); }

        // All-purpose data storage 32 bit
        virtual uint32 GetData(uint32 /*Type*/) const { return 0; }
        virtual void SetData(uint32 /*Type*/, uint32 /*Data*/) {}

        // Achievement criteria additional requirements check
        // NOTE: not use this if same can be checked existing requirement types from AchievementCriteriaRequirementType
        virtual bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target = nullptr, uint32 miscvalue1 = 0) const;

        // Condition criteria additional requirements check
        // This is used for such things are heroic loot
        // See ObjectMgr.h enum ConditionSource for possible values of conditionSourceType
        virtual bool CheckConditionCriteriaMeet(Player const* source, uint32 instance_condition_id, WorldObject const* conditionSource, uint32 conditionSourceType) const;

        virtual void FillInitialWorldStates(ByteBuffer& /*data*/, uint32& /*count*/, uint32 /*zoneId*/, uint32 /*areaId*/) {}

        // helper functions for world state list fill
        inline void FillInitialWorldStateData(ByteBuffer& data, uint32& count, uint32 state, uint32 value)
        {
            data << uint32(state);
            data << uint32(value);
            ++count;
        }

        inline void FillInitialWorldStateData(ByteBuffer& data, uint32& count, uint32 state, int32 value)
        {
            data << uint32(state);
            data << int32(value);
            ++count;
        }

        virtual void ShowChatCommands(ChatHandler* /*handler*/) {}
        virtual void ExecuteChatCommand(ChatHandler* /*handler*/, char* /*args*/) {}

        // Wotlk only
        // Special UI unit frame - sent mostly for raid bosses
        void SendEncounterFrame(uint32 type, ObjectGuid sourceGuid = ObjectGuid(), uint8 param1 = 0, uint8 param2 = 0) const;
};

#endif
