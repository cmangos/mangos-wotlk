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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef WORLD_PVP_MGR_H
#define WORLD_PVP_MGR_H

#include "Common.h"
#include "Timer.h"

enum
{
    TIMER_OPVP_MGR_UPDATE           = 1 * IN_MILLISECONDS       // 1 milliseconds are required for Wintergrasp
};

enum OutdoorPvPTypes
{
    OPVP_ID_SI = 0,
    OPVP_ID_EP,
    OPVP_ID_HP,
    OPVP_ID_ZM,
    OPVP_ID_TF,
    OPVP_ID_NA,
    OPVP_ID_GH,
    OPVP_ID_WG,

    MAX_OPVP_ID
};

enum BattlefieldTypes
{
    BATTLEFIELD_WG = 1,
};

enum OutdoorPvPZones
{
    ZONE_ID_SILITHUS                = 1377,
    ZONE_ID_TEMPLE_OF_AQ            = 3428,
    ZONE_ID_RUINS_OF_AQ             = 3429,
    ZONE_ID_GATES_OF_AQ             = 3478,

    ZONE_ID_EASTERN_PLAGUELANDS     = 139,
    ZONE_ID_STRATHOLME              = 2017,
    ZONE_ID_SCHOLOMANCE             = 2057,

    ZONE_ID_HELLFIRE_PENINSULA      = 3483,
    ZONE_ID_HELLFIRE_RAMPARTS       = 3562,
    ZONE_ID_HELLFIRE_CITADEL        = 3563,
    ZONE_ID_BLOOD_FURNACE           = 3713,
    ZONE_ID_SHATTERED_HALLS         = 3714,
    ZONE_ID_MAGTHERIDON_LAIR        = 3836,

    ZONE_ID_ZANGARMARSH             = 3521,
    ZONE_ID_SERPENTSHRINE_CAVERN    = 3607,
    ZONE_ID_STEAMVAULT              = 3715,
    ZONE_ID_UNDERBOG                = 3716,
    ZONE_ID_SLAVE_PENS              = 3717,

    ZONE_ID_TEROKKAR_FOREST         = 3519,
    ZONE_ID_SHADOW_LABYRINTH        = 3789,
    ZONE_ID_AUCHENAI_CRYPTS         = 3790,
    ZONE_ID_SETHEKK_HALLS           = 3791,
    ZONE_ID_MANA_TOMBS              = 3792,

    ZONE_ID_NAGRAND                 = 3518,

    ZONE_ID_GRIZZLY_HILLS           = 394,

    ZONE_ID_WINTERGRASP             = 4197,
};

struct CapturePointSlider
{
    CapturePointSlider() : Value(0.0f), IsLocked(false) {}
    CapturePointSlider(float value, bool isLocked) : Value(value), IsLocked(isLocked) {}

    float Value;
    bool IsLocked;
};

class Player;
class GameObject;
class Creature;
class Object;
class OutdoorPvP;
class Battlefield;

typedef std::map<uint32 /*capture point entry*/, CapturePointSlider /*slider value and lock state*/> CapturePointSliderMap;

class OutdoorPvPMgr
{
    public:
        OutdoorPvPMgr();
        ~OutdoorPvPMgr();

        // load all outdoor pvp scripts
        void InitOutdoorPvP();

        // called when a player enters an outdoor pvp zone
        void HandlePlayerEnterZone(Player* player, uint32 zoneId);

        // called when player leaves an outdoor pvp zone
        void HandlePlayerLeaveZone(Player* player, uint32 zoneId);

        // called when a player enters an area which is part of an outdoor pvp zone
        void HandlePlayerEnterArea(Player* player, uint32 zoneId, uint32 areaId);

        // called when player leaves an area which is part of an outdoor pvp zone
        void HandlePlayerLeaveArea(Player* player, uint32 zoneId, uint32 areaId);

        // return assigned outdoor pvp script
        OutdoorPvP* GetScript(uint32 zoneId);

        void Update(uint32 diff);

        // Save and load capture point slider
        CapturePointSliderMap const* GetCapturePointSliderMap() const { return &m_capturePointSlider; }
        void SetCapturePointSlider(uint32 entry, CapturePointSlider value) { m_capturePointSlider[entry] = value; }

        // return assigned battlefield script by id
        Battlefield * GetBattlefieldById(uint32 id);

    private:
        // return assigned outdoor pvp script
        OutdoorPvP* GetScriptOfAffectedZone(uint32 zoneId);

        // contains all outdoor pvp scripts
        OutdoorPvP* m_scripts[MAX_OPVP_ID];

        CapturePointSliderMap m_capturePointSlider;

        // update interval
        ShortIntervalTimer m_updateTimer;
};

#define sOutdoorPvPMgr MaNGOS::Singleton<OutdoorPvPMgr>::Instance()

#endif
