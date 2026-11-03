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

#include "OutdoorPvPMgr.h"
#include "Policies/Singleton.h"
#include "OutdoorPvP.h"
#include "World/World.h"
#include "Log/Log.h"
#include "OutdoorPvPEP.h"
#include "OutdoorPvPGH.h"
#include "OutdoorPvPHP.h"
#include "OutdoorPvPNA.h"
#include "OutdoorPvPSI.h"
#include "OutdoorPvPTF.h"
#include "OutdoorPvPZM.h"
#include "Battlefield/Battlefield.h"
#include "Battlefield/BattlefieldWG.h"
#include "Maps/Map.h"

OutdoorPvPMgr::OutdoorPvPMgr(Map& map) : m_map(map)
{
    m_updateTimer.SetInterval(TIMER_OPVP_MGR_UPDATE);
    memset(&m_scripts, 0, sizeof(m_scripts));
}

OutdoorPvPMgr::~OutdoorPvPMgr()
{
    for (auto& m_script : m_scripts)
        delete m_script;
}

/**
   Function which loads all outdoor pvp scripts
 */
void OutdoorPvPMgr::InitOutdoorPvP()
{
    uint8 counter = 0;

    switch ((WorldMaps)m_map.GetId())
    {
        case WorldMaps::EASTERN_KINGDOMS:
        {
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_EP_ENABLED))
                m_scripts[OPVP_ID_EP] = new OutdoorPvPEP(m_map, *this);
            break;
        }
        case WorldMaps::KALIMDOR:
        {
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_SI_ENABLED))
                m_scripts[OPVP_ID_SI] = new OutdoorPvPSI(m_map, *this);
            break;
        }
        case WorldMaps::OUTLAND:
        {
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_HP_ENABLED))
                m_scripts[OPVP_ID_HP] = new OutdoorPvPHP(m_map, *this);
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_ZM_ENABLED))
                m_scripts[OPVP_ID_ZM] = new OutdoorPvPZM(m_map, *this);
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_TF_ENABLED))
                m_scripts[OPVP_ID_TF] = new OutdoorPvPTF(m_map, *this);
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_NA_ENABLED))
                m_scripts[OPVP_ID_NA] = new OutdoorPvPNA(m_map, *this);
            break;
        }
        case WorldMaps::NORTHREND:
        {
            if (sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_GH_ENABLED))
                m_scripts[OPVP_ID_GH] = new OutdoorPvPGH(m_map, *this);
            if (sWorld.getConfig(CONFIG_BOOL_BATTLEFIELD_WG_ENABLED))
                m_scripts[OPVP_ID_WG] = new BattlefieldWG(m_map, *this);
            break;
        }
    }

    sLog.outString(">> Loaded Outdoor PvP zones for map %u", m_map.GetId());
}

OutdoorPvP* OutdoorPvPMgr::GetScript(uint32 zoneId) const
{
    switch (zoneId)
    {
        case ZONE_ID_SILITHUS:
            return m_scripts[OPVP_ID_SI];
        case ZONE_ID_EASTERN_PLAGUELANDS:
            return m_scripts[OPVP_ID_EP];
        case ZONE_ID_HELLFIRE_PENINSULA:
            return m_scripts[OPVP_ID_HP];
        case ZONE_ID_ZANGARMARSH:
            return m_scripts[OPVP_ID_ZM];
        case ZONE_ID_TEROKKAR_FOREST:
            return m_scripts[OPVP_ID_TF];
        case ZONE_ID_NAGRAND:
            return m_scripts[OPVP_ID_NA];
        case ZONE_ID_GRIZZLY_HILLS:
            return m_scripts[OPVP_ID_GH];
        case ZONE_ID_WINTERGRASP:
            return m_scripts[OPVP_ID_WG];
        default:
            return nullptr;
    }
}

/**
   Function that handles the players which enters a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
 */
void OutdoorPvPMgr::HandlePlayerEnterZone(Player* player, uint32 zoneId)
{
    if (OutdoorPvP* script = GetScript(zoneId))
        script->HandlePlayerEnterZone(player, true);
}

/**
   Function that handles the player who leaves a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
 */
void OutdoorPvPMgr::HandlePlayerLeaveZone(Player* player, uint32 zoneId)
{
    // teleport: called once from Player::CleanupsBeforeDelete, once from Player::UpdateZone
    if (OutdoorPvP* script = GetScript(zoneId))
        script->HandlePlayerLeaveZone(player, true);
}

/**
   Function that handles the player who enters a specific area in a specific zone id

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
   @param   area id refered by the function
 */
void OutdoorPvPMgr::HandlePlayerEnterArea(Player* player, uint32 zoneId, uint32 areaId)
{
    if (OutdoorPvP* script = GetScript(zoneId))
        script->HandlePlayerEnterArea(player, areaId, true);
}

/**
   Function that handles the player who leaves a specific area in a specific zone id

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
   @param   area id refered by the function
 */
void OutdoorPvPMgr::HandlePlayerLeaveArea(Player* player, uint32 zoneId, uint32 areaId)
{
    if (OutdoorPvP* script = GetScript(zoneId))
        script->HandlePlayerLeaveArea(player, areaId, true);
}

void OutdoorPvPMgr::Update(uint32 diff)
{
    m_updateTimer.Update(diff);
    if (!m_updateTimer.Passed())
        return;

    for (auto& m_script : m_scripts)
        if (m_script)
            m_script->Update(m_updateTimer.GetCurrent());

    m_updateTimer.Reset();
}

/**
   Function that returns the battlefield script by id

   @param   battlefield id
 */
Battlefield* OutdoorPvPMgr::GetBattlefieldById(uint32 id)
{
    for (auto script : m_scripts)
        if (script)
            if (script->IsBattlefield() && ((Battlefield*)script)->GetBattlefieldId() == id)
                return (Battlefield*)script;

    return nullptr;
}
