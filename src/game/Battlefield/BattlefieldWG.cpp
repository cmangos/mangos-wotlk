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

#include "BattlefieldWG.h"
#include "Entities/Creature.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Entities/Unit.h"
#include "Spells/SpellAuras.h"
#include "Globals/ObjectMgr.h"

BattlefieldWG::BattlefieldWG() : Battlefield()
{
    m_zoneOwner         = TEAM_NONE;
    m_zoneId            = ZONE_ID_WINTERGRASP;
    m_battleFieldId     = BATTLEFIELD_WG;

    Reset();
}

void BattlefieldWG::Reset()
{
    m_status = BF_STATUS_COOLDOWN;

    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        m_damagedTowers[i] = 0;
        m_destroyedTowers[i] = 0;
        m_workshopCount[i] = 0;
    }

    SetTimer(m_cooldownDuration);
    m_startTime = 0;

    m_playersInvited = false;
    m_sentPrebattleWarning = false;

    // load the defender buildings
    for (uint8 i = 0; i < countof(wgFortressData); ++i)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(wgFortressData[i].goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetOwner(GetDefender());
        building->SetWorldState(wgFortressData[i].worldState);
        m_keepBuildings.push_back(building);
    }

    // load the attacker buildings
    for (uint8 i = 0; i < countof(wgOffensiveData); ++i)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(wgOffensiveData[i].goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetAttacker() && GetDefender() != TEAM_NONE)
            goState = GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetOwner(GetAttacker());
        building->SetWorldState(wgOffensiveData[i].worldState);
        m_offensiveBuildings.push_back(building);
    }
}

void BattlefieldWG::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // general world states
    FillInitialWorldState(data, count, WORLD_STATE_WG_SHOW_COOLDOWN, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_TIME_TO_NEXT_BATTLE, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? uint32(time(NULL) + m_timer / 1000) : 0);

    FillInitialWorldState(data, count, WORLD_STATE_WG_SHOW_BATTLE, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_TIME_TO_END, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? uint32(time(NULL) + m_timer / 1000) : 0);

    // team specific world states
    FillInitialWorldState(data, count, WORLD_STATE_WG_ALLIANCE_DEFENDER, GetDefender() == ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_HORDE_DEFENDER, GetDefender() == HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_A, uint32(m_vehicleGUIDs[0].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_A, m_workshopCount[0] * 4);

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGUIDs[1].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_H, m_workshopCount[1] * 4);

    // display all the walls, towers and workshops
    for (std::list<BattlefieldBuilding*>::const_iterator itr = m_keepBuildings.begin(); itr != m_keepBuildings.end(); ++itr)
        FillInitialWorldState(data, count, (*itr)->GetWorldState(), (*itr)->GetGoState());

    for (std::list<BattlefieldBuilding*>::const_iterator itr = m_offensiveBuildings.begin(); itr != m_offensiveBuildings.end(); ++itr)
        FillInitialWorldState(data, count, (*itr)->GetWorldState(), (*itr)->GetGoState());
}

void BattlefieldWG::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(WORLD_STATE_WG_SHOW_BATTLE, WORLD_STATE_REMOVE);
}

void BattlefieldWG::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    Battlefield::HandlePlayerEnterZone(player, isMainZone);

    // phase each player based on the defender
    player->CastSpell(player, GetDefender() == ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE : SPELL_HORDE_CONTROL_PHASE, TRIGGERED_OLD_TRIGGERED);

    // defenders are phased and get increase XP
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && GetDefender() == player->GetTeam())
        player->CastSpell(player, SPELL_ESSENCE_WINTERGRASP_ZONE, TRIGGERED_OLD_TRIGGERED);

    // Note: flight restriction during combat - Implemented in Player::UpdateArea()
    //if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && player->IsFreeFlying() && !player->isGameMaster() && !player->HasAura(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA))
    //    player->CastSpell(player, SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA, TRIGGERED_OLD_TRIGGERED);

    // ToDo:
    // update tower control aura and the score aura
}

void BattlefieldWG::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    Battlefield::HandlePlayerLeaveZone(player, isMainZone);

    // remove phase auras
    player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE);
    player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE);
    player->RemoveAurasDueToSpell(SPELL_ESSENCE_WINTERGRASP_ZONE);

    // remove war auras
    player->RemoveAurasDueToSpell(SPELL_RECRUIT);
    player->RemoveAurasDueToSpell(SPELL_CORPORAL);
    player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
    player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);

    // remove flight restriction (not required - removed by timer)
    // player->RemoveAurasDueToSpell(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA);
}

void BattlefieldWG::HandlePlayerEnterArea(Player* player, uint32 areaId, bool isMainZone)
{
    switch (areaId)
    {
        case AREA_ID_WINTERGRASP_FORTRESS:
        case AREA_ID_THE_SUNKEN_RING:
        case AREA_ID_THE_BROKEN_TEMPLE:
        case AREA_ID_THE_CHILLED_QUAGMIRE:
        case AREA_ID_WESTPARK_WORKSHOP:
        case AREA_ID_EASTPARK_WORKSHOP:
            // ToDo: handle SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE and SPELL_HORDE_CONTROLS_FACTORY_PHASE
            return;
    }
}

void BattlefieldWG::HandlePlayerLeaveArea(Player* player, uint32 areaId, bool isMainZone)
{
    // ToDo: handle SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE and SPELL_HORDE_CONTROLS_FACTORY_PHASE
}

void BattlefieldWG::HandleCreatureCreate(Creature* creature)
{

}

void BattlefieldWG::HandleCreatureDeath(Creature* creature)
{

}

void BattlefieldWG::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);
}

void BattlefieldWG::HandlePlayerKillInsideArea(Player* player, Unit* victim)
{

}

bool BattlefieldWG::HandleEvent(uint32 eventId, GameObject* go)
{
    bool returnValue = false;

    // handle capture points
    if (go->GetGoType() == GAMEOBJECT_TYPE_CAPTURE_POINT)
        returnValue = HandleCapturePointEvent(eventId, go);
    // handle destructible buildings
    else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
        returnValue = HandleDestructibleBuildingEvent(eventId, go);
    // handle titan relic capture
    else if (go->GetGoType() == GAMEOBJECT_TYPE_GOOBER && eventId == EVENT_TITAN_RELIC)
    {
        if (GetBattlefieldStatus() != BF_STATUS_IN_PROGRESS)
            return true;

        // EndBattle(teamId, false);
    }

    return returnValue;
}

void BattlefieldWG::Update(uint32 diff)
{
    Battlefield::Update(diff);

    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && m_timer <= 3 * MINUTE * IN_MILLISECONDS && !m_sentPrebattleWarning)
    {
        m_sentPrebattleWarning = true;
        SendZoneWarning(LANG_OPVP_WG_ABOUT_TO_BEGIN);
    }
}

void BattlefieldWG::StartBattle(Team defender)
{
    Battlefield::StartBattle(defender);

    SendZoneWarning(LANG_OPVP_WG_BATTLE_BEGIN);

    // Enable restricted flight area - handled in Player::UpdateArea()
}

void BattlefieldWG::EndBattle(Team winner, bool byTimer)
{

}

void BattlefieldWG::RewardPlayersOnBattleEnd(Team winner)
{

}

void BattlefieldWG::InitPlayerBattlefieldData(Player* player)
{

}

void BattlefieldWG::SetupPlayerPosition(Player* player)
{

}

void BattlefieldWG::UpdateGraveyardOwner(uint8 id, PvpTeamIndex newOwner)
{

}

bool BattlefieldWG::GetPlayerKickLocation(Player* player, float& x, float& y, float& z)
{
    return false;
}

void BattlefieldWG::UpdatePlayerOnWarResponse(Player* player)
{

}

// Function that handles all capture point events
bool BattlefieldWG::HandleCapturePointEvent(uint32 eventId, GameObject* go)
{
    return false;
}

// Function that handles all destructible buildings events
bool BattlefieldWG::HandleDestructibleBuildingEvent(uint32 eventId, GameObject* go)
{
    return false;
}

bool BattlefieldWG::IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType)
{
    switch (conditionId)
    {
        case OPVP_COND_WG_MAX_ALLIANCE_VEHICLES:
        case OPVP_COND_WG_MAX_HORDE_VEHICLES:
            // Todo: return false if the max allowed vehicles for ally / horde is has been reached
            return false;
        case OPVP_COND_WG_BATTLEFIELD_IN_PROGRESS:
            return GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS;
    }

    return false;
}

void BattlefieldWG::HandleConditionStateChange(uint32 conditionId, bool state)
{

}
