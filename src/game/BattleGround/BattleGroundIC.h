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

#ifndef __BATTLEGROUNDIC_H
#define __BATTLEGROUNDIC_H

#include "Common.h"
#include "BattleGround.h"
#include "Tools/Language.h"

class BattleGround;

enum
{
    // generic enums
    BG_IC_MAX_REINFORCEMENTS            = 300,
    BG_IC_WORKSHOP_UPDATE_TIME          = 3 * MINUTE * IN_MILLISECONDS,
    BG_IC_TRANSPORT_PERIOD_TIME         = 2 * MINUTE * IN_MILLISECONDS,
    BG_IC_FLAG_CAPTURING_TIME           = MINUTE * IN_MILLISECONDS,
    BG_IC_RESOURCE_TICK_TIMER           = 45000,
    BG_IC_CLOSE_DOORS_TIME              = 20000,

    // world states
    BG_IC_STATE_ALLY_REINFORCE_SHOW     = 4221,         // reinforcements counters
    BG_IC_STATE_HORDE_REINFORCE_SHOW    = 4222,
    BG_IC_STATE_ALLY_REINFORCE_COUNT    = 4226,
    BG_IC_STATE_HORDE_REINFORCE_COUNT   = 4227,

    BG_IC_STATE_GATE_FRONT_H_CLOSED     = 4317,         // keep gates are closed / intact
    BG_IC_STATE_GATE_WEST_H_CLOSED      = 4318,
    BG_IC_STATE_GATE_EAST_H_CLOSED      = 4319,
    BG_IC_STATE_GATE_FRONT_A_CLOSED     = 4328,
    BG_IC_STATE_GATE_WEST_A_CLOSED      = 4327,
    BG_IC_STATE_GATE_EAST_A_CLOSED      = 4326,

    BG_IC_STATE_GATE_FRONT_H_OPEN       = 4322,         // keep gates are open / destroyed
    BG_IC_STATE_GATE_WEST_H_OPEN        = 4321,
    BG_IC_STATE_GATE_EAST_H_OPEN        = 4320,
    BG_IC_STATE_GATE_FRONT_A_OPEN       = 4323,
    BG_IC_STATE_GATE_WEST_A_OPEN        = 4324,
    BG_IC_STATE_GATE_EAST_A_OPEN        = 4325,

    BG_IC_STATE_DOCKS_UNCONTROLLED      = 4301,         // docks
    BG_IC_STATE_DOCKS_CONFLICT_A        = 4305,
    BG_IC_STATE_DOCKS_CONFLICT_H        = 4302,
    BG_IC_STATE_DOCKS_CONTROLLED_A      = 4304,
    BG_IC_STATE_DOCKS_CONTROLLED_H      = 4303,

    BG_IC_STATE_HANGAR_UNCONTROLLED     = 4296,         // hangar
    BG_IC_STATE_HANGAR_CONFLICT_A       = 4300,
    BG_IC_STATE_HANGAR_CONFLICT_H       = 4297,
    BG_IC_STATE_HANGAR_CONTROLLED_A     = 4299,
    BG_IC_STATE_HANGAR_CONTROLLED_H     = 4298,

    BG_IC_STATE_WORKSHOP_UNCONTROLLED   = 4294,         // workshop
    BG_IC_STATE_WORKSHOP_CONFLICT_A     = 4228,
    BG_IC_STATE_WORKSHOP_CONFLICT_H     = 4293,
    BG_IC_STATE_WORKSHOP_CONTROLLED_A   = 4229,
    BG_IC_STATE_WORKSHOP_CONTROLLED_H   = 4230,

    BG_IC_STATE_QUARRY_UNCONTROLLED     = 4306,         // quarry
    BG_IC_STATE_QUARRY_CONFLICT_A       = 4310,
    BG_IC_STATE_QUARRY_CONFLICT_H       = 4307,
    BG_IC_STATE_QUARRY_CONTROLLED_A     = 4309,
    BG_IC_STATE_QUARRY_CONTROLLED_H     = 4308,

    BG_IC_STATE_REFINERY_UNCONTROLLED   = 4311,         // refinery
    BG_IC_STATE_REFINERY_CONFLICT_A     = 4315,
    BG_IC_STATE_REFINERY_CONFLICT_H     = 4312,
    BG_IC_STATE_REFINERY_CONTROLLED_A   = 4314,
    BG_IC_STATE_REFINERY_CONTROLLED_H   = 4313,

    BG_IC_STATE_ALLY_KEEP_UNCONTROLLED  = 4341,         // alliance keep
    BG_IC_STATE_ALLY_KEEP_CONFLICT_A    = 4342,
    BG_IC_STATE_ALLY_KEEP_CONFLICT_H    = 4343,
    BG_IC_STATE_ALLY_KEEP_CONTROLLED_A  = 4339,
    BG_IC_STATE_ALLY_KEEP_CONTROLLED_H  = 4340,

    BG_IC_STATE_HORDE_KEEP_UNCONTROLLED = 4346,         // horde keep
    BG_IC_STATE_HORDE_KEEP_CONFLICT_A   = 4347,
    BG_IC_STATE_HORDE_KEEP_CONFLICT_H   = 4348,
    BG_IC_STATE_HORDE_KEEP_CONTROLLED_A = 4344,
    BG_IC_STATE_HORDE_KEEP_CONTROLLED_H = 4345,

    // creatures
    BG_IC_NPC_COMMANDER_WYRMBANE        = 34924,        // Alliance Boss
    BG_IC_NPC_OVERLORD_AGMAR            = 34922,        // Horde Boss
    BG_IC_NPC_KORKORN_GUARD             = 34918,
    BG_IC_NPC_LEGION_INFANTRY           = 34919,

    BG_IC_NPC_WORLD_TRIGGER             = 22515,

    BG_IC_VEHICLE_GUNSHIP_CANNON_H      = 34935,
    BG_IC_VEHICLE_GUNSHIP_CANNON_A      = 34929,
    BG_IC_VEHICLE_KEEP_CANNON           = 34944,
    BG_IC_VEHICLE_SIEGE_ENGINE_A        = 34776,
    BG_IC_VEHICLE_SIEGE_ENGINE_H        = 35069,
    BG_IC_VEHICLE_DEMOLISHER            = 34775,
    BG_IC_VEHICLE_GLAIVE_THROWER_A      = 34802,
    BG_IC_VEHICLE_GLAIVE_THROWER_H      = 35273,
    BG_IC_VEHICLE_CATAPULT              = 34793,

    // gameobjects
    BG_IC_GO_ALLIANCE_GATE_WEST         = 195699,
    BG_IC_GO_ALLIANCE_GATE_EAST         = 195698,
    BG_IC_GO_ALLIANCE_GATE_FRONT        = 195700,

    BG_IC_GO_HORDE_GATE_WEST            = 195496,
    BG_IC_GO_HORDE_GATE_EAST            = 195495,
    BG_IC_GO_HORDE_GATE_FRONT           = 195494,

    BG_IC_GO_ALLIANCE_PORTCULLIS        = 195703,
    BG_IC_GO_HORDE_PORTCULLIS           = 195491,

    BG_IC_GO_HORDE_GUNSHIP              = 195276,
    BG_IC_GO_ALLIANCE_GUNSHIP           = 195121,

    // ToDo: add banner entries

    // spells
    BG_IC_SPELL_REFINERY                = 68719,
    BG_IC_SPELL_QUARRY                  = 68720,

    // graveyard links
    BG_IC_GRAVEYARD_ID_DOCKS            = 1480,
    BG_IC_GRAVEYARD_ID_HANGAR           = 1481,
    BG_IC_GRAVEYARD_ID_WORKSHOP         = 1482,
    BG_IC_GRAVEYARD_ID_ALLIANCE         = 1483,         // last option for alliance; not capturable
    BG_IC_GRAVEYARD_ID_HORDE            = 1484,         // last option for horde; not capturable
    BG_IC_GRAVEYARD_ID_KEEP_ALLY        = 1485,
    BG_IC_GRAVEYARD_ID_KEEP_HORDE       = 1486,

    // sounds
    BG_IC_SOUND_NODE_CLAIMED            = 8192,
    BG_IC_SOUND_NODE_CAPTURED_ALLIANCE  = 8173,
    BG_IC_SOUND_NODE_CAPTURED_HORDE     = 8213,
    BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE = 8212,
    BG_IC_SOUND_NODE_ASSAULTED_HORDE    = 8174,

};

class BattleGroundICScore : public BattleGroundScore
{
    public:
        BattleGroundICScore(): BasesAssaulted(0), BasesDefended(0) {};
        virtual ~BattleGroundICScore() {};

        uint32 GetAttr1() const { return BasesAssaulted; }
        uint32 GetAttr2() const { return BasesDefended; }

        uint32 BasesAssaulted;
        uint32 BasesDefended;
};

class BattleGroundIC : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundIC();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;
        virtual void StartingEventOpenDoors() override;
        virtual void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
        virtual void Reset() override;
        void EndBattleGround(Team winner) override;

        /* Scorekeeping */
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

        bool HandleEvent(uint32 eventId, GameObject* go, Unit* invoker) override;
        void HandleKillUnit(Creature* creature, Player* killer) override;

        void EventPlayerClickedOnFlag(Player* player, GameObject* go) override;

        void Update(uint32 diff) override;

    private:

        uint32 m_reinforcements[PVP_TEAM_COUNT];
};
#endif
