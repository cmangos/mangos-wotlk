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
    m_keepBuildings.reserve(countof(wgFortressData));
    for (const auto& i : wgFortressData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetOwner(GetDefender());
        building->SetWorldState(i.worldState);
        m_keepBuildings.push_back(building);
    }

    // load the attacker buildings
    m_offensiveBuildings.reserve(countof(wgOffensiveData));
    for (const auto& i : wgOffensiveData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetAttacker() && GetDefender() != TEAM_NONE)
            goState = GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetOwner(GetAttacker());
        building->SetWorldState(i.worldState);
        m_offensiveBuildings.push_back(building);
    }

    // load the defense workshops - only inside the fortress
    m_defenseWorkshops.reserve(countof(wgFortressWorkshopsData));
    for (const auto& i : wgFortressWorkshopsData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetOwner(GetDefender());
        building->SetWorldState(i.worldState);
        m_defenseWorkshops.push_back(building);
    }

    // load the capturable workshops
    m_capturableWorkshops.reserve(countof(wgCapturePointData));
    for (const auto& i : wgCapturePointData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntryWorkshop);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
        {
            Team comparableTeam = i.goEntryWorkshop == GO_WORKSHOP_BROKEN_TEMPLE || i.goEntryWorkshop == GO_WORKSHOP_SUNKEN_RING ? GetDefender() : GetAttacker();
            goState = comparableTeam == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;
        }

        building->SetGoState(goState);
        building->SetOwner(GetDefender());
        building->SetWorldState(i.worldState);
        m_capturableWorkshops.push_back(building);
    }
}

void BattlefieldWG::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // general world states
    FillInitialWorldState(data, count, WORLD_STATE_WG_SHOW_COOLDOWN, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_TIME_TO_NEXT_BATTLE, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? uint32(time(nullptr) + m_timer / 1000) : 0);

    FillInitialWorldState(data, count, WORLD_STATE_WG_SHOW_BATTLE, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_TIME_TO_END, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? uint32(time(nullptr) + m_timer / 1000) : 0);

    // team specific world states
    FillInitialWorldState(data, count, WORLD_STATE_WG_ALLIANCE_DEFENDER, GetDefender() == ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WORLD_STATE_WG_HORDE_DEFENDER, GetDefender() == HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_A, uint32(m_vehicleGUIDs[0].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_A, m_workshopCount[0] * 4);

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGUIDs[1].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_H, m_workshopCount[1] * 4);

    // display all the walls, towers and workshops
    for (auto building : m_keepBuildings)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());
    
    for (auto building : m_offensiveBuildings)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());

    for (auto building : m_defenseWorkshops)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());

    for (auto building : m_capturableWorkshops)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());
}

void BattlefieldWG::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(WORLD_STATE_WG_SHOW_BATTLE, WORLD_STATE_REMOVE);
}

void BattlefieldWG::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    Battlefield::HandlePlayerEnterZone(player, isMainZone);

    // phase each player based on the defender (if there is one)
    if (GetDefender() != TEAM_NONE)
        player->CastSpell(player, GetDefender() == ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE : SPELL_HORDE_CONTROL_PHASE, TRIGGERED_OLD_TRIGGERED);

    // defenders are phased and get increase XP
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && GetDefender() == player->GetTeam())
        player->CastSpell(player, SPELL_ESSENCE_WINTERGRASP_ZONE, TRIGGERED_OLD_TRIGGERED);

    // Note: flight restriction during combat - Implemented in Player::UpdateArea()

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
        case AREA_ID_THE_SUNKEN_RING:
        case AREA_ID_THE_BROKEN_TEMPLE:
        case AREA_ID_WESTPARK_WORKSHOP:
        case AREA_ID_EASTPARK_WORKSHOP:
            // ToDo: handle SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE and SPELL_HORDE_CONTROLS_FACTORY_PHASE
            return;
    }
}

void BattlefieldWG::HandlePlayerLeaveArea(Player* player, uint32 areaId, bool isMainZone)
{
    switch (areaId)
    {
        case AREA_ID_THE_SUNKEN_RING:
        case AREA_ID_THE_BROKEN_TEMPLE:
        case AREA_ID_WESTPARK_WORKSHOP:
        case AREA_ID_EASTPARK_WORKSHOP:
            player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE);
            player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE);
            return;
    }
}

void BattlefieldWG::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_WINTERGRASP_TOWER_CANNON:
            if (creature->GetPositionX() < 5000.0f)
            {
                if (GetAttacker() != TEAM_NONE)
                    creature->setFaction(GetAttacker() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);

                m_attackCannonsGuids.push_back(creature->GetObjectGuid());
            }
            else
            {
                if (GetDefender() != TEAM_NONE)
                    creature->setFaction(GetDefender() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);

                m_defenseCannonsGuids.push_back(creature->GetObjectGuid());
            }
            break;
    }
}

void BattlefieldWG::HandleCreatureDeath(Creature* creature)
{

}

void BattlefieldWG::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);

    // load the attacker buildings
    for (auto building : m_offensiveBuildings)
    {
        if (building->GetGoEntry() == go->GetEntry())
        {
            // store the object guid and set the correct faction
            building->SetGoGuid(go->GetObjectGuid());

            if (GetAttacker() != TEAM_NONE)
                go->SetFaction(GetAttacker() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);

            // no other actions required
            return;
        }
    }

    // load the defender buildings
    for (auto building : m_keepBuildings)
    {
        if (building->GetGoEntry() == go->GetEntry())
        {
            // store the object guid and set the correct faction
            building->SetGoGuid(go->GetObjectGuid());

            if (GetDefender() != TEAM_NONE)
                go->SetFaction(GetDefender() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);

            // no other actions required
            return;
        }
    }

    switch (go->GetEntry())
    {
        case GO_CAPTUREPOINT_BROKEN_TEMPLE_A:
        case GO_CAPTUREPOINT_BROKEN_TEMPLE_H:
        case GO_CAPTUREPOINT_SUNKEN_RING_A:
        case GO_CAPTUREPOINT_SUNKEN_RING_H:
        case GO_CAPTUREPOINT_WESTPARK_A:
        case GO_CAPTUREPOINT_WESTPARK_H:
        case GO_CAPTUREPOINT_EASTPARK_A:
        case GO_CAPTUREPOINT_EASTPARK_H:
            // ToDo: fix this. It has to get the real time owner
            if (go->GetCapturePointSliderValue() == 100)
                go->SetGoArtKit(CAPTURE_ARTKIT_ALLIANCE);
            else if (go->GetCapturePointSliderValue() == 0)
                go->SetGoArtKit(CAPTURE_ARTKIT_HORDE);
            break;
    }
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

        // get winner faction depending on the relic faction
        EndBattle(go->GetEntry() == GO_TITAN_RELIC_ALLIANCE ? ALLIANCE : HORDE, false);
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

    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && m_timer < diff)
    {
        EndBattle(GetDefender(), true);
    }
}

void BattlefieldWG::StartBattle(Team defender)
{
    // set the defender if there isn't any
    if (GetDefender() == TEAM_NONE)
        defender = urand(0, 1) ? ALLIANCE : HORDE;

    Battlefield::StartBattle(defender);

    SendZoneWarning(LANG_OPVP_WG_BATTLE_BEGIN);

    // remove essence of WG phase
    BuffTeam(GetDefender() == ALLIANCE ? ALLIANCE : HORDE, SPELL_ESSENCE_WINTERGRASP_ZONE, true);

    // reset battlefield
    for (GuidZoneMap::const_iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        // Find player who is in main zone
        if (!itr->second)
            continue;

        if (Player* player = sObjectMgr.GetPlayer(itr->first))
            ResetBattlefield(player);
    }

    // Enable restricted flight area - handled in Player::UpdateArea()

    // note: at the beginning the entire area is reset to the following status
    // the defender starts by owning the fortress and the sunken ring and broken temple
    // the attacker starts by owning all three towers plus the two eastpart and westpark workshops
}

void BattlefieldWG::EndBattle(Team winner, bool byTimer)
{
    // note: at the end of the battle the workshops stay in the possesion of whoever owned them in the battle
    // the towers however reset to neutral

    Battlefield::EndBattle(winner, byTimer);

    // ToDo: update phasing; respawn vendors; despawn vehicles; repair fortress (maybe)
}

void BattlefieldWG::ResetBattlefield(const WorldObject* objRef)
{
    // add owner phase to both teams; redundant check
    if (GetDefender() == TEAM_NONE)
        return;

    // add proper team buff
    BuffTeam(ALLIANCE, GetDefender() == ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE : SPELL_HORDE_CONTROL_PHASE);
    BuffTeam(HORDE, GetDefender() == ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE : SPELL_HORDE_CONTROL_PHASE);

    // Note: research if the fortress vendors are despawned during the battle

    // update world states
    SendUpdateWorldState(WORLD_STATE_WG_SHOW_COOLDOWN, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_TIME_TO_NEXT_BATTLE, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? uint32(time(nullptr) + m_timer / 1000) : 0);

    SendUpdateWorldState(WORLD_STATE_WG_SHOW_BATTLE, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_TIME_TO_END, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? uint32(time(nullptr) + m_timer / 1000) : 0);

    // team specific world states
    SendUpdateWorldState(WORLD_STATE_WG_ALLIANCE_DEFENDER, GetDefender() == ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_HORDE_DEFENDER, GetDefender() == HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    SendUpdateWorldState(WORLD_STATE_WG_VEHICLE_A, uint32(m_vehicleGUIDs[0].size()));
    SendUpdateWorldState(WORLD_STATE_WG_MAX_VEHICLE_A, m_workshopCount[0] * 4);

    SendUpdateWorldState(WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGUIDs[1].size()));
    SendUpdateWorldState(WORLD_STATE_WG_MAX_VEHICLE_H, m_workshopCount[1] * 4);

    // reset and respawn buildings
    for (auto building : m_offensiveBuildings)
    {
        if (GameObject* gameObject = objRef->GetMap()->GetGameObject(building->GetGoGuid()))
        {
            gameObject->SetFaction(GetAttacker() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);
            gameObject->RebuildGameObject((Unit*)objRef);
        }

        building->SetOwner(GetAttacker() == ALLIANCE ? ALLIANCE : HORDE);
        building->SetGoState(GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
    }
    
    for (auto building : m_keepBuildings)
    {
        if (GameObject* gameObject = objRef->GetMap()->GetGameObject(building->GetGoGuid()))
        {
            gameObject->SetFaction(GetDefender() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);
            gameObject->RebuildGameObject((Unit*)objRef);
        }

        building->SetOwner(GetDefender() == ALLIANCE ? ALLIANCE : HORDE);
        building->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
    }

    // reset workshops
    for (auto building : m_defenseWorkshops)
    {
        building->SetOwner(GetDefender() == ALLIANCE ? ALLIANCE : HORDE);
        building->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
    }
    
    for (auto building : m_capturableWorkshops)
    {
        Team comparableTeam = building->GetGoEntry() == GO_WORKSHOP_BROKEN_TEMPLE || building->GetGoEntry() == GO_WORKSHOP_SUNKEN_RING ? GetDefender() : GetAttacker();
        building->SetOwner(comparableTeam == ALLIANCE ? ALLIANCE : HORDE);
        building->SetGoState(comparableTeam == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
    }

    // reset and respawn cannons
    for (const auto& guid : m_attackCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
        {
            cannon->setFaction(GetAttacker() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);
            cannon->Respawn();
        }
    }
    
    for (const auto& guid : m_defenseCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
        {
            cannon->setFaction(GetDefender() == ALLIANCE ? FACTION_ID_ALLIANCE_GENERIC : FACTION_ID_HORDE_GENERIC);
            cannon->Respawn();
        }
    }
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
    for (const auto& i : wgCapturePointData)
    {
        if (go->GetEntry() == i.goEntryAlliance || go->GetEntry() == i.goEntryHorde)
        {
            if (eventId == i.eventContestedAlliance || eventId == i.eventContestedHorde)
            {
                // ToDo: send some warning
            }
            else if (eventId == i.eventProgressAlliance)
            {
                SetBannerVisual(go, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

                // ToDo: SendUpdateWorldState(); send text; switch owner; change phase aura
            }
            else if (eventId == i.eventProgressHorde)
            {
                SetBannerVisual(go, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);


                 // ToDo: SendUpdateWorldState(); send text; switch owner; change phase aura
            }
        }
    }

    return false;
}

// Function that handles all destructible buildings events
bool BattlefieldWG::HandleDestructibleBuildingEvent(uint32 eventId, GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_WG_FORTRESS_DOOR:
            // ToDo: on destroy remove flag 16 from the titan relic
            break;
    }

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
