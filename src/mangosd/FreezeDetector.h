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

#ifndef FREEZE_DETECTOR_H
#define FREEZE_DETECTOR_H

#include "Common.h"
#include "Log.h"
#include "Threading.h"
#include "Timer.h"
#include "World.h"

/// Thread which is intended to detect a possible server freeze
class FreezeDetectorRunnable : public MaNGOS::Runnable
{
public:
    FreezeDetectorRunnable() : delaytime_(0), loops_(0), lastchange_(0) { }

    void run(void) override
    {
        if (!delaytime_)
            return;

        sLog.outString("Starting up anti-freeze thread (%u seconds max stuck time)...", delaytime_ / 1000);

        while (!World::IsStopped())
        {
            MaNGOS::Thread::Sleep(1000);

            uint32 curtime = WorldTimer::getMSTime();

            // Normal work
            if (loops_ != World::m_worldLoopCounter)
            {
                lastchange_ = curtime;
                loops_ = World::m_worldLoopCounter;
            }
            // Possible freeze
            else if (WorldTimer::getMSTimeDiff(lastchange_, curtime) > delaytime_)
            {
                sLog.outError("World Thread hangs, kicking out server!");
                MANGOS_ASSERT(false);
            }
        }
        sLog.outString("Anti-freeze thread exiting without problems.");
    }

    void SetDelayTime(uint32 t) { delaytime_ = t; }

private:
    uint32 loops_;
    uint32 lastchange_;
    uint32 delaytime_;
};

#endif // FREEZE_DETECTOR_H
