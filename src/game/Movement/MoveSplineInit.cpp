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

#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "packet_builder.h"
#include "Entities/Unit.h"
#include "Log.h"
#include "Maps/TransportSystem.h"
#include "Entities/Transports.h"

namespace Movement
{
    static thread_local uint32 splineCounter = 1;

    int32 MoveSplineInit::Launch()
    {
        // show path in the client if need
        if (unit.HaveDebugFlag(CMDEBUGFLAG_WP_PATH))
        {
            uint32 counter = 0;
            for (auto pt : args.path)
            {
                TempSpawnSettings settings;
                settings.spawner = &unit;
                settings.entry = VISUAL_WAYPOINT;
                settings.x = pt.x; settings.y = pt.y; settings.z = pt.z; settings.ori = 0.0f;
                settings.activeObject = true;
                settings.despawnTime = 30 * IN_MILLISECONDS;
                settings.spawnType = TEMPSPAWN_TIMED_DESPAWN;
                settings.spawnDataEntry = 2;

                settings.tempSpawnMovegen = true;
                settings.waypointId = counter++;

                WorldObject::SummonCreature(settings, unit.GetMap(), unit.GetPhaseMask());
            }
        }
        MoveSpline& move_spline = *unit.movespline;
        TransportInfo* transportInfo = unit.GetTransportInfo();
        // TODO: merge these two together
        GenericTransport* transport = unit.GetTransport();

        Location real_position(unit.GetPositionX(), unit.GetPositionY(), unit.GetPositionZ(), unit.GetOrientation());

        // If boarded use current local position
        if (transportInfo)
            transportInfo->GetLocalPosition(real_position.x, real_position.y, real_position.z, real_position.orientation);
        if (transport)
            transport->CalculatePassengerOffset(real_position.x, real_position.y, real_position.z, &real_position.orientation);

        // there is a big chance that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        if (!move_spline.Finalized() && !transportInfo)
            real_position = move_spline.ComputePosition();

        bool pathEmpty = false;
        if (args.path.empty())
        {
            // should i do the things that user should do?
            MoveTo(real_position);
            pathEmpty = true;
        }

        // corrent first vertex
        args.path[0] = real_position;
        args.initialOrientation = real_position.orientation;

        args.flags.enter_cycle = args.flags.cyclic;
        uint32 moveFlags = unit.m_movementInfo.GetMovementFlags();
        if (args.flags.walkmode)
            moveFlags |= MOVEFLAG_WALK_MODE;
        else
            moveFlags &= ~MOVEFLAG_WALK_MODE;

        if (!pathEmpty) // should not broadcast these flags on orientation change only
            moveFlags |= (MOVEFLAG_SPLINE_ENABLED | MOVEFLAG_FORWARD);

        if (args.velocity == 0.f) // ignore swim speed and flight speed because its not used in generic scripting - always possible to override
        {
            args.velocity = unit.GetSpeed(MovementInfo::GetSpeedType(MovementFlags(moveFlags & ~(MOVEFLAG_FLYING | MOVEFLAG_SWIMMING))));
            if (args.slowed != 0.f) // when set always > 0.5
                args.velocity *= args.slowed;
        }

        if (!args.Validate(&unit))
            return 0;

        bool removedRoot = false;
        if (args.flags.hasFlag(MoveSplineFlag::eFlags::BoardVehicle | MoveSplineFlag::eFlags::ExitVehicle))
        {
            if (moveFlags & MOVEFLAG_ROOT)
                removedRoot = true;
            moveFlags &= ~MOVEFLAG_ROOT;
        }

        if (moveFlags & MOVEFLAG_ROOT && !pathEmpty)
        {
            sLog.outCustomLog("Invalid movement during root. Entry: %u IsImmobilized %s, moveflags %u", unit.GetEntry(), unit.IsImmobilizedState() ? "true" : "false", moveFlags);
            sLog.traceLog();
            return 0;
        }

        args.splineId = splineCounter++;

        unit.m_movementInfo.SetMovementFlags(MovementFlags(moveFlags));

        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data << unit.GetPackGUID();

        if (transportInfo || transport)
        {
            data.SetOpcode(SMSG_MONSTER_MOVE_TRANSPORT);
            if (transportInfo)
            {
                data << transportInfo->GetTransportGuid().WriteAsPacked();
                data << int8(transportInfo->GetTransportSeat());
            }
            else if (transport)
            {
                data << transport->GetPackGUID();
                data << int8(-1);
            }
        }

        PacketBuilder::WriteMonsterMove(move_spline, data);
        unit.SendMessageToAllWhoSeeMe(data, true);

        if (removedRoot)
            unit.m_movementInfo.AddMovementFlag(MOVEFLAG_ROOT);

        return move_spline.Duration();
    }

    void MoveSplineInit::Stop(bool forceSend /*= false*/)
    {
        MoveSpline& move_spline = *unit.movespline;

        // No need to stop if we are not moving
        if (!forceSend && move_spline.Finalized())
            return;

        TransportInfo* transportInfo = unit.GetTransportInfo();
        // TODO: merge these two together
        GenericTransport* transport = unit.GetTransport();

        Location real_position(unit.GetPositionX(), unit.GetPositionY(), unit.GetPositionZ(), unit.GetOrientation());

        // If boarded use current local position
        if (transportInfo)
            transportInfo->GetLocalPosition(real_position.x, real_position.y, real_position.z, real_position.orientation);
        if (transport)
            transport->CalculatePassengerOffset(real_position.x, real_position.y, real_position.z, &real_position.orientation);

        // there is a big chane that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        if (!move_spline.Finalized())
            real_position = move_spline.ComputePosition();

        if (args.path.empty())
        {
            // should i do the things that user should do?
            MoveTo(real_position);
        }

        // corrent first vertex
        args.path[0] = real_position;

        args.splineId = splineCounter++;

        args.flags = MoveSplineFlag::Done;
        unit.m_movementInfo.RemoveMovementFlag(MovementFlags(MOVEFLAG_FORWARD | MOVEFLAG_SPLINE_ENABLED));
        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data << unit.GetPackGUID();

        if (transportInfo || transport)
        {
            data.SetOpcode(SMSG_MONSTER_MOVE_TRANSPORT);
            if (transportInfo)
            {
                data << transportInfo->GetTransportGuid().WriteAsPacked();
                data << int8(transportInfo->GetTransportSeat());
            }
            else if (transport)
            {
                data << transport->GetPackGUID();
                data << int8(-1);
            }
        }

        data << uint8(0);
        data << real_position.x << real_position.y << real_position.z;
        data << move_spline.GetId();
        data << uint8(MonsterMoveStop);
        unit.SendMessageToAllWhoSeeMe(data, true);
    }

    MoveSplineInit::MoveSplineInit(Unit& m) : unit(m)
    {
        // mix existing state into new
        args.flags.walkmode = unit.m_movementInfo.HasMovementFlag((MovementFlags)(MOVEFLAG_WALK_MODE | MOVEFLAG_HOVER));
        args.flags.flying = unit.m_movementInfo.HasMovementFlag((MovementFlags)(MOVEFLAG_CAN_FLY | MOVEFLAG_FLYING | MOVEFLAG_LEVITATING));
    }

    void MoveSplineInit::SetFacing(const WorldObject* target)
    {
        args.flags.EnableFacingTarget();
        args.facing.target = target->GetObjectGuid().GetRawValue();
    }

    void MoveSplineInit::SetFacing(float angle)
    {
        args.facing.angle = G3D::wrap(angle, 0.f, (float)G3D::twoPi());
        args.flags.EnableFacingAngle();
    }
}
