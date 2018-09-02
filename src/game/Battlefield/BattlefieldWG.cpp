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
        m_destroyedTowers[i] = 0;
        m_workshopCount[i] = 0;
    }

    SetTimer(m_cooldownDuration);
    m_startTime = 0;

    m_playersInvited = false;
    m_sentPrebattleWarning = false;

    // ToDo: initialize zone owner from DB in case of server restart

    DEBUG_LOG("Battlefield WG: Initializing battlefield buildings.");

    // load the defender buildings
    m_keepBuildings.reserve(countof(wgFortressData));
    for (const auto& i : wgFortressData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetWorldState(i.worldState);
        m_keepBuildings.push_back(building);
    }

    // load the defender towers buildings
    m_defenseTowers.reserve(countof(wgFortressTowersData));
    for (const auto& i : wgFortressTowersData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetWorldState(i.worldState);
        m_defenseTowers.push_back(building);
    }

    // load the attacker buildings
    m_offenseTowers.reserve(countof(wgOffenseData));
    for (const auto& i : wgOffenseData)
    {
        BattlefieldBuilding* building = new BattlefieldBuilding(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetAttacker() && GetDefender() != TEAM_NONE)
            goState = GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

        building->SetGoState(goState);
        building->SetWorldState(i.worldState);
        m_offenseTowers.push_back(building);
    }

    // load the defense workshops - only inside the fortress
    m_defenseWorkshops.reserve(countof(wgFortressWorkshopsData));
    for (const auto& i : wgFortressWorkshopsData)
    {
        WintergraspFactory* factory = new WintergraspFactory(i.goEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        if (GetDefender() && GetDefender() != TEAM_NONE)
        {
            goState = GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;
            sObjectMgr.SetGraveYardLinkTeam(i.graveyardId, ZONE_ID_WINTERGRASP, GetDefender());
        }
        else
            sObjectMgr.SetGraveYardLinkTeam(i.graveyardId, ZONE_ID_WINTERGRASP, TEAM_INVALID);

        factory->SetGoState(goState);
        factory->SetOwner(GetDefender());
        factory->SetWorldState(i.worldState);
        factory->SetGraveyardId(i.graveyardId);
        m_defenseWorkshops.push_back(factory);
    }

    // load the capturable workshops
    m_capturableWorkshops.reserve(countof(wgCapturePointData));
    for (const auto& i : wgCapturePointData)
    {
        WintergraspFactory* factory = new WintergraspFactory(i.workshopEntry);

        BattlefieldGoState goState = BF_GO_STATE_NEUTRAL_INTACT;
        Team comparableTeam = TEAM_NONE;

        if (GetDefender() && GetDefender() != TEAM_NONE)
        {
            Team comparableTeam = i.workshopEntry == GO_WORKSHOP_BROKEN_TEMPLE || i.workshopEntry == GO_WORKSHOP_SUNKEN_RING ? GetDefender() : GetAttacker();
            goState = comparableTeam == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT;

            sObjectMgr.SetGraveYardLinkTeam(i.graveyardId, ZONE_ID_WINTERGRASP, comparableTeam);
        }
        else
            sObjectMgr.SetGraveYardLinkTeam(i.graveyardId, ZONE_ID_WINTERGRASP, TEAM_INVALID);

        factory->SetGoState(goState);
        factory->SetOwner(comparableTeam);
        factory->SetWorldState(i.worldState);
        factory->SetCapturePointEntry(TEAM_INDEX_ALLIANCE, i.goEntryAlliance);
        factory->SetCapturePointEntry(TEAM_INDEX_HORDE, i.goEntryHorde);
        factory->SetGraveyardId(i.graveyardId);
        factory->SetAreaId(i.areaId);
        factory->SetConditionId(i.conditionId);
        m_capturableWorkshops.push_back(factory);
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

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_A, uint32(m_vehicleGuids[TEAM_INDEX_ALLIANCE].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_A, m_workshopCount[TEAM_INDEX_ALLIANCE] * 4);

    FillInitialWorldState(data, count, WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGuids[TEAM_INDEX_HORDE].size()));
    FillInitialWorldState(data, count, WORLD_STATE_WG_MAX_VEHICLE_H, m_workshopCount[TEAM_INDEX_HORDE] * 4);

    // display all the walls, towers and workshops
    for (auto building : m_keepBuildings)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());

    for (auto building : m_defenseTowers)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());

    for (auto building : m_defenseWorkshops)
        FillInitialWorldState(data, count, building->GetWorldState(), building->GetGoState());
    
    for (auto building : m_offenseTowers)
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
    // If battlefield is in progress and the first player joins after the battle has started, then he needs to reset the battlefield first
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && m_zonePlayers.empty())
        GetBattlefieldReady(player);

    Battlefield::HandlePlayerEnterZone(player, isMainZone);

    // phase each player based on the defender (if there is one)
    if (GetDefender() != TEAM_NONE)
        player->CastSpell(player, wgTeamControlAuras[GetTeamIndexByTeamId(GetDefender())], TRIGGERED_OLD_TRIGGERED);

    // defenders are phased and get increase XP
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && GetDefender() == player->GetTeam())
        player->CastSpell(player, SPELL_ESSENCE_WINTERGRASP_ZONE, TRIGGERED_OLD_TRIGGERED);

    // flight restriction - applied based on AREA_FLAG_CANNOT_FLY in Player::UpdateArea

    // set the tower control buff
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
    {
        uint32 maxBuff = player->GetTeam() == GetAttacker() ? MAX_WG_OFFENSE_TOWERS - m_destroyedTowers[GetTeamIndexByTeamId(GetAttacker())] : m_destroyedTowers[GetTeamIndexByTeamId(GetAttacker())];
        for (uint8 i = 0; i < maxBuff; ++i)
            player->CastSpell(player, SPELL_TOWER_CONTROL, TRIGGERED_OLD_TRIGGERED);
    }

    // ToDo: update score aura
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

    // flight restriction - removed based on AREA_FLAG_CANNOT_FLY in Player::UpdateArea
}

void BattlefieldWG::HandlePlayerEnterArea(Player* player, uint32 areaId, bool isMainZone)
{
    switch (areaId)
    {
        case AREA_ID_THE_SUNKEN_RING:
        case AREA_ID_THE_BROKEN_TEMPLE:
        case AREA_ID_WESTPARK_WORKSHOP:
        case AREA_ID_EASTPARK_WORKSHOP:
            // apply corresponding workshop aura
            // ToDo: needs to be enabled when phase stacking is supported
            /*for (auto workshop : m_capturableWorkshops)
            {
                if (areaId == workshop->GetAreaId() && workshop->GetOwner() != TEAM_NONE)
                     player->CastSpell(player, wgTeamFactoryAuras[GetTeamIndexByTeamId(workshop->GetOwner())], TRIGGERED_OLD_TRIGGERED);
            }*/
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
            // ToDo: needs to be enabled when phase stacking is supported
            /*player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE);
            player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE);*/
            return;
    }
}

void BattlefieldWG::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_WINTERGRASP_TOWER_CANNON:
            // handle attacker cannons
            if (creature->GetPositionX() < 5000.0f)
            {
                if (GetAttacker() != TEAM_NONE)
                    creature->setFaction(wgTeamFactions[GetTeamIndexByTeamId(GetAttacker())]);

                m_attackCannonsGuids.push_back(creature->GetObjectGuid());
            }
            // handle defender cannons
            else
            {
                if (GetDefender() != TEAM_NONE)
                    creature->setFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);

                m_defenseCannonsGuids.push_back(creature->GetObjectGuid());
            }
            // cannons are despawned during cooldown
            if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
                creature->ForcedDespawn();
            break;
        case NPC_INVISIBLE_STALKER:
            m_stalkersGuids.push_back(creature->GetObjectGuid());
            break;
        case NPC_WINTERGRASP_DETECTION_UNIT:
            m_detectionUnitsGuids.push_back(creature->GetObjectGuid());
            break;
            // alliance vendors
        case NPC_ANCHORITE_TESSA:
        case NPC_COMMANDER_ZANNETH:
        case NPC_SENIOR_DEMOLITIONIST_LEGOSO:
        case NPC_SIEGE_MASTER_STOUTHANDLE:
        case NPC_SORCERESS_KAYLANA:
        case NPC_TACTICAL_OFFICER_AHBRAMIS:
        case NPC_BOWYER_RANDOLPH:
        case NPC_KNIGHT_DAMERON:
        case NPC_MARSHAL_MAGRUDER:
        case NPC_MORGAN_DAY:
        case NPC_TRAVIS_DAY:
        case NPC_ALLIANCE_BRIGADIER_GENERAL:
            m_vendorGuids[TEAM_INDEX_ALLIANCE].push_back(creature->GetObjectGuid());
            break;
            // horde vendors
        case NPC_COMMANDER_DARDOSH:
        case NPC_HOODOO_MASTER_FUJIN:
        case NPC_LEUTENANT_MURP:
        case NPC_PRIMALIST_MULFORT:
        case NPC_SIEGESMITH_STRONGHOOF:
        case NPC_TACTICAL_OFFICER_KILRATH:
        case NPC_VIERON_BLAZEFEATHER:
        case NPC_STONE_GUARD_MUKAR:
        case NPC_CHAMPION_ROSSLAI:
        case NPC_HORDE_WARBRINGER:
            m_vendorGuids[TEAM_INDEX_HORDE].push_back(creature->GetObjectGuid());
            break;
        case NPC_WINTERGRASP_CATAPULT:
        case NPC_WINTERGRASP_DEMOLISHER:
        case NPC_WINTERGRASP_SIEGE_ENGINE_A:
        case NPC_WINTERGRASP_SIEGE_ENGINE_H:
            m_activeVehiclesGuids.push_back(creature->GetObjectGuid());
            // increase the vehicle count and update world states
            if (creature->IsTemporarySummon())
            {
                if (Player* creator = creature->GetMap()->GetPlayer(creature->GetSpawnerGuid()))
                {
                    Team creatorTeam = creator->GetTeam();
                    m_vehicleGuids[GetTeamIndexByTeamId(creatorTeam)].push_back(creature->GetObjectGuid());
                    SendUpdateWorldState(creatorTeam == ALLIANCE ? WORLD_STATE_WG_VEHICLE_A : WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGuids[GetTeamIndexByTeamId(creatorTeam)].size()));

                    // apply faction flag; ToDo: need to research if faction is also updated
                    creature->CastSpell(creature, creatorTeam == ALLIANCE ? SPELL_ALLIANCE_FLAG : SPELL_HORDE_FLAG, TRIGGERED_OLD_TRIGGERED);
                }
            }
            break;
        case NPC_WANDERING_SHADOW:
        case NPC_LIVING_LASHER:
        case NPC_GLACIAL_SPIRIT:
        case NPC_RAGING_FLAME:
        case NPC_WHISPERING_WIND:
        case NPC_CHILLED_EARTH_ELEMENTAL:
            m_trashMobsGuids.push_back(creature->GetObjectGuid());
            break;
    }
}

void BattlefieldWG::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);

    // *** Load Wintergrasp Destructible buildings *** //
    if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        // load the defender buildings
        for (auto building : m_keepBuildings)
        {
            if (go->GetEntry() == building->GetGoEntry())
            {
                // store the object guid and set the correct faction
                building->SetGoGuid(go->GetObjectGuid());

                if (GetDefender() != TEAM_NONE)
                    go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);

                // no further actions required
                return;
            }
        }

        // load the defender towers
        for (auto building : m_defenseTowers)
        {
            if (go->GetEntry() == building->GetGoEntry())
            {
                // store the object guid and set the correct faction
                building->SetGoGuid(go->GetObjectGuid());

                if (GetDefender() != TEAM_NONE)
                    go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);

                // no further actions required
                return;
            }
        }

        // load the attacker buildings
        for (auto building : m_offenseTowers)
        {
            if (go->GetEntry() == building->GetGoEntry())
            {
                // store the object guid and set the correct faction
                building->SetGoGuid(go->GetObjectGuid());

                if (GetAttacker() != TEAM_NONE)
                    go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetAttacker())]);

                // no further actions required
                return;
            }
        }

        // no further action required
        return;
    }

    // *** Load Wintergrasp Capture points *** //
    if (go->GetGoType() == GAMEOBJECT_TYPE_CAPTURE_POINT)
    {
        // load the capture points
        for (auto workshop : m_capturableWorkshops)
        {
            if (go->GetEntry() == workshop->GetCapturePointEntry(TEAM_INDEX_ALLIANCE) || go->GetEntry() == workshop->GetCapturePointEntry(TEAM_INDEX_HORDE))
            {
                // store the object guid
                if (workshop->GetCapturePointEntry(TEAM_INDEX_ALLIANCE) == go->GetEntry())
                    workshop->SetCapturePointGuid(TEAM_INDEX_ALLIANCE, go->GetObjectGuid());
                else
                    workshop->SetCapturePointGuid(TEAM_INDEX_HORDE, go->GetObjectGuid());

                // set the object artkit
                if (workshop->GetOwner() != TEAM_NONE)
                    go->SetGoArtKit(wgCapturePointArtKits[GetTeamIndexByTeamId(workshop->GetOwner())]);
                else
                    go->SetGoArtKit(CAPTURE_ARTKIT_NEUTRAL);

                // capture points are locked during cooldown
                if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
                    go->SetLootState(GO_JUST_DEACTIVATED);

                // no further actions required
                return;
            }
        }

        // no further action required
        return;
    }

    // *** Load Wintergrasp other gameobjects *** //
    switch (go->GetEntry())
    {
        case GO_TITAN_RELIC_ALLIANCE:
            m_relicGuid[TEAM_INDEX_ALLIANCE] = go->GetObjectGuid();
            return;
        case GO_TITAN_RELIC_HORDE:
            m_relicGuid[TEAM_INDEX_HORDE] = go->GetObjectGuid();
            return;
    }
}

bool BattlefieldWG::HandleEvent(uint32 eventId, GameObject* go)
{
    if (GetDefender() == TEAM_NONE)
    {
        sLog.outError("Battlefield WG: Gameobject events cannot be handled when there is no zone defender.");
        return true;
    }

    // no events handled during cooldown
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
        return true;

    bool returnValue = false;

    // handle capture points
    if (go->GetGoType() == GAMEOBJECT_TYPE_CAPTURE_POINT)
        returnValue = HandleCapturePointEvent(eventId, go);
    // handle destructible buildings
    else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
        returnValue = HandleDestructibleBuildingEvent(eventId, go);

    return returnValue;
}

bool BattlefieldWG::HandleGameObjectUse(Player* player, GameObject* go)
{
    if (GetDefender() == TEAM_NONE)
    {
        sLog.outError("Battlefield WG: Gameobject use cannot be handled when there is no zone defender.");
        return true;
    }

    // no events handled during cooldown
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
        return true;

    // handle titan relic capture - get winner faction depending on the relic faction
    if (go->GetEntry() == GO_TITAN_RELIC_ALLIANCE || go->GetEntry() == GO_TITAN_RELIC_HORDE)
    {
        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        EndBattle(go->GetEntry() == GO_TITAN_RELIC_ALLIANCE ? ALLIANCE : HORDE);
    }

    return false;
}

// Function that handles all capture point events
bool BattlefieldWG::HandleCapturePointEvent(uint32 eventId, GameObject* go)
{
    uint8 index = 0;
    for (auto workshop : m_capturableWorkshops)
    {
        // capture points are always phased based on zone defender
        Team owner = workshop->GetGoEntry() == GO_WORKSHOP_BROKEN_TEMPLE || workshop->GetGoEntry() == GO_WORKSHOP_SUNKEN_RING ? GetDefender() : GetAttacker();
        if (go->GetEntry() == workshop->GetCapturePointEntry(GetTeamIndexByTeamId(owner)))
        {
            // loop through all the 4 possible events for each capture point
            for (uint8 i = 0; i < 4; ++i)
            {
                if (eventId == wgCapturePointEventData[index][i].eventEntry)
                {
                    DEBUG_LOG("Battlefield WG: Handle Wintergrasp Capture point event %u from gameobject id %u", eventId, go->GetEntry());

                    // set owner and new object state (faction based)
                    workshop->SetOwner(wgCapturePointEventData[index][i].team);
                    workshop->SetGoState(wgCapturePointEventData[index][i].objectState);

                    // send zone warning and update world state
                    SendWintergraspWarning(wgCapturePointEventData[index][i].defenseMessage, go);
                    SendUpdateWorldState(workshop->GetWorldState(), workshop->GetGoState());

                    // change the banner visuals (if required)
                    if (wgCapturePointEventData[index][i].bannerArtKit)
                        SetBannerVisual(go, wgCapturePointEventData[index][i].bannerArtKit, wgCapturePointEventData[index][i].bannerAnim);

                    // update graveyard links
                    sObjectMgr.SetGraveYardLinkTeam(workshop->GetGraveyardId(), ZONE_ID_WINTERGRASP, wgCapturePointEventData[index][i].team);

                    // change area phasing
                    // ToDo: needs to be enabled when phase stacking is supported
                    // BuffTeam(wgCapturePointEventData[index][i].team, wgTeamFactoryAuras[GetTeamIndexByTeamId(workshop->GetOwner())], false, workshop->GetAreaId());
                    return true;
                }
            }
        }
        ++index;
    }

    return false;
}

// Function that handles all destructible buildings events
bool BattlefieldWG::HandleDestructibleBuildingEvent(uint32 eventId, GameObject* go)
{
    // special event for the fortress door
    if (go->GetEntry() == GO_WG_FORTRESS_DOOR && eventId == EVENT_KEEP_DOOR_DESTROY)
    {
        // Remove the no interact flag when door is destroyed
        if (GameObject* relic = go->GetMap()->GetGameObject(m_relicGuid[GetTeamIndexByTeamId(GetAttacker())]))
            relic->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    }

    // loop through defense tower
    uint8 index = 0;
    for (auto tower : m_defenseTowers)
    {
        if (go->GetEntry() == tower->GetGoEntry())
        {
            DEBUG_LOG("Battlefield WG: Handle Wintergrasp Defender tower event %u from gameobject id %u", eventId, go->GetEntry());

            // update tower based on event id
            if (eventId == wgFortressTowersData[index].eventDamaged)
            {
                tower->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DAMAGED : BF_GO_STATE_HORDE_DAMAGED);
                SendWintergraspWarning(wgFortressTowersData[index].messageDamaged, go);
            }
            else if (eventId == wgFortressTowersData[index].eventDestroyed)
            {
                tower->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DESTROYED : BF_GO_STATE_HORDE_DESTROYED);
                SendWintergraspWarning(wgFortressTowersData[index].messagedDestroyed, go);

                ++m_destroyedTowers[GetTeamIndexByTeamId(GetDefender())];
                // ToDo: handle possible yell

                // ToDo: despawn banner
            }
            else if (eventId == wgFortressTowersData[index].eventIntact)
            {
                go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);
                tower->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            }

            // send world state updatem
            SendUpdateWorldState(tower->GetWorldState(), tower->GetGoState());
            return true;
        }
        ++index;
    }

    // loop through the offensive towers
    index = 0;
    for (auto tower : m_offenseTowers)
    {
        if (go->GetEntry() == tower->GetGoEntry())
        {
            DEBUG_LOG("Battlefield WG: Handle Wintergrasp Attacker tower event %u from gameobject id %u", eventId, go->GetEntry());

            // update tower based on event id
            if (eventId == wgOffenseData[index].eventDamaged)
            {
                tower->SetGoState(GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DAMAGED : BF_GO_STATE_HORDE_DAMAGED);
                SendWintergraspWarning(wgOffenseData[index].messageDamaged, go);
            }
            else if (eventId == wgOffenseData[index].eventDestroyed)
            {
                tower->SetGoState(GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DESTROYED : BF_GO_STATE_HORDE_DESTROYED);
                SendWintergraspWarning(wgOffenseData[index].messagedDestroyed, go);

                // handle tower buffs
                BuffTeam(GetAttacker(), SPELL_TOWER_CONTROL, true);
                BuffTeam(GetDefender(), SPELL_TOWER_CONTROL);

                ++m_destroyedTowers[GetTeamIndexByTeamId(GetAttacker())];
                // ToDo: handle some yell from the Fortress commander

                // ToDo: despawn banner
            }
            else if (eventId == wgOffenseData[index].eventIntact)
            {
                go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetAttacker())]);
                tower->SetGoState(GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            }

            // send world state update
            SendUpdateWorldState(tower->GetWorldState(), tower->GetGoState());
            return true;
        }
        ++index;
    }

    // loop through the keep walls (and gate / door)
    index = 0;
    for (auto wall : m_keepBuildings)
    {
        if (go->GetEntry() == wall->GetGoEntry())
        {
            DEBUG_LOG("Battlefield WG: Handle Wintergrasp Fortress wall event %u from gameobject id %u", eventId, go->GetEntry());

            // update wall state based on even id
            if (eventId == wgFortressData[index].eventDamaged)
            {
                wall->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DAMAGED : BF_GO_STATE_HORDE_DAMAGED);

                // ToDo: despawn banner if any
            }
            else if (eventId == wgFortressData[index].eventDestroyed)
                wall->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_DESTROYED : BF_GO_STATE_HORDE_DESTROYED);
            else if (eventId == wgFortressData[index].eventIntact)
            {
                go->SetFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);
                wall->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            }

            // send world state update
            SendUpdateWorldState(wall->GetWorldState(), wall->GetGoState());
            return true;
        }
        ++index;
    }

    return false;
}

// Function that handles the lock or unlock of the workshops
void BattlefieldWG::LockWorkshops(bool lock, const WorldObject* objRef, WintergraspFactory* workshop)
{
    if (lock)
    {
        if (GameObject* go = objRef->GetMap()->GetGameObject(workshop->GetCapturePointGuid(GetTeamIndexByTeamId(workshop->GetOwner()))))
            go->SetLootState(GO_JUST_DEACTIVATED);
        else
        {
            // if grid is unloaded, changing the saved slider value is enough
            CapturePointSlider value(workshop->GetOwner() == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE, true);
            sOutdoorPvPMgr.SetCapturePointSlider(workshop->GetCapturePointEntry(GetTeamIndexByTeamId(workshop->GetOwner())), value);
        }
    }
    else
    {
        // set the capture point value and visual
        if (GameObject* go = objRef->GetMap()->GetGameObject(workshop->GetCapturePointGuid(GetTeamIndexByTeamId(workshop->GetOwner()))))
        {
            go->SetCapturePointSlider(go->GetGOInfo()->capturePoint.startingValue, false);
            // visual update needed because banner still has artkit from previous owner
            SetBannerVisual(go, wgCapturePointArtKits[GetTeamIndexByTeamId(workshop->GetOwner())], wgCapturePointAnims[GetTeamIndexByTeamId(workshop->GetOwner())]);
        }
        else
        {
            // if grid is unloaded, changing the saved slider value is enough
            CapturePointSlider value(workshop->GetOwner() == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE, false);
            sOutdoorPvPMgr.SetCapturePointSlider(workshop->GetCapturePointEntry(GetTeamIndexByTeamId(workshop->GetOwner())), value);
        }
    }
}

void BattlefieldWG::StartBattle(Team defender)
{
    // set the defender if there isn't any
    if (GetDefender() == TEAM_NONE)
    {
        defender = urand(0, 1) ? ALLIANCE : HORDE;
        DEBUG_LOG("Battlefield WG: No Defender team found. Choosing a random team.");
    }

    // call main battlefield start function
    Battlefield::StartBattle(defender);

    // flight restriction - applied based on AREA_FLAG_CANNOT_FLY in Player::UpdateArea

    // get a player reference in order to start the battlefield
    if (Player* player = GetPlayerInZone())
        GetBattlefieldReady(player);
}

void BattlefieldWG::EndBattle(Team winner)
{
    // get player reference in order to end the battlefield
    if (Player* player = GetPlayerInZone())
        CleanupBattlefield(player, winner);

    // must be called after cleanup
    Battlefield::EndBattle(winner);

    // reset variables
    m_sentPrebattleWarning = false;
}

// Function that will (re)set the battlefield
void BattlefieldWG::GetBattlefieldReady(const WorldObject* objRef)
{
    // add owner phase to both teams; redundant check
    if (GetDefender() == TEAM_NONE)
    {
        sLog.outError("Battlefield WG: Battlefield reset failed. Defender team could not be found.");
        EndBattle(TEAM_NONE);
        return;
    }

    DEBUG_LOG("Battlefield WG: Making Wintergrasp ready for battle.");

    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        m_destroyedTowers[i] = 0;
        m_workshopCount[i] = 0;
    }

    // send message start warning
    SendWintergraspWarning(LANG_OPVP_WG_BATTLE_BEGIN, objRef);

    // ***** Update Wintergrasp phasing **** //
    // remove essence of WG phase
    // ToDo: needs to be enabled when phase stacking is supported
    // BuffTeam(GetDefender(), SPELL_ESSENCE_WINTERGRASP_ZONE, true);

    // add proper battlefield phasing (can be reduntant)
    BuffTeam(ALLIANCE, wgTeamControlAuras[GetTeamIndexByTeamId(GetDefender())]);
    BuffTeam(HORDE, wgTeamControlAuras[GetTeamIndexByTeamId(GetDefender())]);

    // ***** Rebuild and reset the buildings **** //
    // rebuild attack tower - factions handled by event
    for (auto building : m_offenseTowers)
    {
        if (GameObject* gameObject = objRef->GetMap()->GetGameObject(building->GetGoGuid()))
            gameObject->RebuildGameObject((Unit*)objRef);
        else
        {
            building->SetGoState(GetAttacker() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
        }

        // Buff attacker for each tower
        BuffTeam(GetAttacker(), SPELL_TOWER_CONTROL);
    }
    // rebuild fortress - factions handled by event
    for (auto building : m_keepBuildings)
    {
        if (GameObject* gameObject = objRef->GetMap()->GetGameObject(building->GetGoGuid()))
            gameObject->RebuildGameObject((Unit*)objRef);
        else
        {
            building->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
        }
    }
    // rebuild defense towers - factions handled by event
    for (auto building : m_defenseTowers)
    {
        if (GameObject* gameObject = objRef->GetMap()->GetGameObject(building->GetGoGuid()))
            gameObject->RebuildGameObject((Unit*)objRef);
        else
        {
            building->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
            SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
        }
    }
    // reset defense workshops
    for (auto workshop : m_defenseWorkshops)
    {
        workshop->SetOwner(GetDefender());
        workshop->SetGoState(GetDefender() == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(workshop->GetWorldState(), workshop->GetGoState());
        sObjectMgr.SetGraveYardLinkTeam(workshop->GetGraveyardId(), ZONE_ID_WINTERGRASP, GetDefender());

        ++m_workshopCount[GetTeamIndexByTeamId(GetDefender())];
    }
    // reset the other workshops
    for (auto workshop : m_capturableWorkshops)
    {
        Team owner = workshop->GetGoEntry() == GO_WORKSHOP_BROKEN_TEMPLE || workshop->GetGoEntry() == GO_WORKSHOP_SUNKEN_RING ? GetDefender() : GetAttacker();
        workshop->SetOwner(owner);
        workshop->SetGoState(owner == ALLIANCE ? BF_GO_STATE_ALLIANCE_INTACT : BF_GO_STATE_HORDE_INTACT);
        SendUpdateWorldState(workshop->GetWorldState(), workshop->GetGoState());
        sObjectMgr.SetGraveYardLinkTeam(workshop->GetGraveyardId(), ZONE_ID_WINTERGRASP, owner);

        ++m_workshopCount[GetTeamIndexByTeamId(owner)];

        // unlock the workshop
        LockWorkshops(false, objRef, workshop);
    }

    // reset relic for the attacker
    if (GameObject* relic = objRef->GetMap()->GetGameObject(m_relicGuid[GetTeamIndexByTeamId(GetAttacker())]))
    {
        relic->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        relic->Respawn();           // note: on some new versions the relic is allowed to despawn when clicked
    }

    // ***** Reset Wintergrasp cannons **** //
    // reset and respawn cannons
    for (const auto& guid : m_attackCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
        {
            cannon->setFaction(wgTeamFactions[GetTeamIndexByTeamId(GetAttacker())]);
            cannon->Respawn();
        }
    }
    for (const auto& guid : m_defenseCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
        {
            cannon->setFaction(wgTeamFactions[GetTeamIndexByTeamId(GetDefender())]);
            cannon->Respawn();
        }
    }

    // despawn the trash mobs
    for (const auto& guid : m_trashMobsGuids)
    {
        if (Creature* trash = objRef->GetMap()->GetCreature(guid))
        {
            trash->SetRespawnDelay(m_battleDuration);
            trash->ForcedDespawn();
        }
    }

    // ***** Update general stated **** //
    SendUpdateGeneralWorldStates();
}

// Function that will cleanup the battlefield after battle has ended
void BattlefieldWG::CleanupBattlefield(const WorldObject* objRef, Team winner)
{
    // ***** Send messages and apply buffs **** //
    if (GetDefender() == winner)
        SendWintergraspWarning(winner == ALLIANCE ? LANG_OPVP_WG_ALLIANCE_DEFENDED : LANG_OPVP_WG_HORDE_DEFENDED, objRef, winner == ALLIANCE ? SOUND_ID_WG_ALLIANCE_WINS : SOUND_ID_WG_HORDE_WINS);
    else
    {
        SendWintergraspWarning(winner == ALLIANCE ? LANG_OPVP_WG_ALLIANCE_CAPTURED : LANG_OPVP_WG_HORDE_CAPTURED, objRef, winner == ALLIANCE ? SOUND_ID_WG_ALLIANCE_WINS : SOUND_ID_WG_HORDE_WINS);

        // inverse the zone phasing
        BuffTeam(ALLIANCE, wgTeamControlAuras[GetTeamIndexByTeamId(GetDefender())], true);
        BuffTeam(HORDE, wgTeamControlAuras[GetTeamIndexByTeamId(GetDefender())], true);

        BuffTeam(ALLIANCE, wgTeamControlAuras[GetTeamIndexByTeamId(winner)]);
        BuffTeam(HORDE, wgTeamControlAuras[GetTeamIndexByTeamId(winner)]);
    }

    // remove tower buffs
    for (uint8 i = 0; i < MAX_WG_OFFENSE_TOWERS; ++i)
    {
        BuffTeam(ALLIANCE, SPELL_TOWER_CONTROL, true);
        BuffTeam(HORDE, SPELL_TOWER_CONTROL, true);
    }

    // apply reward buff
    // ToDo: needs to be enabled when phase stacking is supported
    // BuffTeam(GetDefender(), SPELL_ESSENCE_WINTERGRASP_ZONE);

    // ***** Reset creatures **** //
    // despawn cannons
    for (const auto& guid : m_attackCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
            cannon->ForcedDespawn();
    }
    for (const auto& guid : m_defenseCannonsGuids)
    {
        if (Creature* cannon = objRef->GetMap()->GetCreature(guid))
            cannon->ForcedDespawn();
    }
    // despawn vehicles
    for (const auto& guid : m_activeVehiclesGuids)
    {
        if (Creature* vehicle = objRef->GetMap()->GetCreature(guid))
            vehicle->ForcedDespawn();
    }
    m_activeVehiclesGuids.clear();
    // respawn all vendors
    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (const auto& guid : m_vendorGuids[i])
        {
            if (Creature* vendor = objRef->GetMap()->GetCreature(guid))
                vendor->Respawn();
        }
    }
    // respawn trash mobs
    for (const auto& guid : m_trashMobsGuids)
    {
        if (Creature* trash = objRef->GetMap()->GetCreature(guid))
        {
            trash->SetRespawnDelay(10 * MINUTE * IN_MILLISECONDS);
            trash->Respawn();
        }
    }

    // ***** Reset towers and fortress **** //
    // note: at the end of the battle the workshops stay in the possesion of whoever owned them in the battle
    // lock the capture points
    for (auto workshop : m_capturableWorkshops)
        LockWorkshops(true, objRef, workshop);

    // buildings are not repaired; only the owner world state is updated if the owner has changed
    if (GetDefender() != winner)
    {
        // reset fortress owner
        for (auto building : m_keepBuildings)
        {
            building->SetGoState(building->GetOppositeGoState());
            SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
        }
        // reset defense tower owner
        for (auto building : m_defenseTowers)
        {
            building->SetGoState(building->GetOppositeGoState());
            SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
        }
    }
    // set attacker towers to neutral and keep the object state
    for (auto building : m_offenseTowers)
    {
        building->SetGoState(building->GetNeutralGoState());
        SendUpdateWorldState(building->GetWorldState(), building->GetGoState());
    }

    // ***** Update general stated **** //
    SendUpdateGeneralWorldStates();
}

// Function that will update all world states on start and end
void BattlefieldWG::SendUpdateGeneralWorldStates()
{
    // update global states
    SendUpdateWorldState(WORLD_STATE_WG_SHOW_COOLDOWN, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_TIME_TO_NEXT_BATTLE, GetBattlefieldStatus() == BF_STATUS_COOLDOWN ? uint32(time(nullptr) + m_timer / 1000) : 0);

    SendUpdateWorldState(WORLD_STATE_WG_SHOW_BATTLE, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_TIME_TO_END, GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS ? uint32(time(nullptr) + m_timer / 1000) : 0);

    // team specific world states
    SendUpdateWorldState(WORLD_STATE_WG_ALLIANCE_DEFENDER, GetDefender() == ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    SendUpdateWorldState(WORLD_STATE_WG_HORDE_DEFENDER, GetDefender() == HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    SendUpdateWorldState(WORLD_STATE_WG_VEHICLE_A, uint32(m_vehicleGuids[TEAM_INDEX_ALLIANCE].size()));
    SendUpdateWorldState(WORLD_STATE_WG_MAX_VEHICLE_A, m_workshopCount[TEAM_INDEX_ALLIANCE] * 4);

    SendUpdateWorldState(WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGuids[TEAM_INDEX_HORDE].size()));
    SendUpdateWorldState(WORLD_STATE_WG_MAX_VEHICLE_H, m_workshopCount[TEAM_INDEX_HORDE] * 4);
}

void BattlefieldWG::HandlePlayerKillInsideArea(Player* player, Unit* victim)
{
    // nothing happens during cooldown
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN)
        return;

    if (victim->GetTypeId() == TYPEID_UNIT)
    {
        switch (victim->GetEntry())
        {
            case NPC_WINTERGRASP_CATAPULT:
            case NPC_WINTERGRASP_DEMOLISHER:
            case NPC_WINTERGRASP_SIEGE_ENGINE_A:
            case NPC_WINTERGRASP_SIEGE_ENGINE_H:
            {
                // update world state on vehicle death
                Creature* vehicle = (Creature*)victim;
                if (vehicle->IsTemporarySummon())
                {
                    if (Player* creator = vehicle->GetMap()->GetPlayer(vehicle->GetSpawnerGuid()))
                    {
                        Team creatorTeam = creator->GetTeam();
                        m_vehicleGuids[GetTeamIndexByTeamId(creatorTeam)].remove(vehicle->GetObjectGuid());
                        SendUpdateWorldState(creatorTeam == ALLIANCE ? WORLD_STATE_WG_VEHICLE_A : WORLD_STATE_WG_VEHICLE_H, uint32(m_vehicleGuids[GetTeamIndexByTeamId(creatorTeam)].size()));
                    }
                }
                break;
            }
            case NPC_VALLIANCE_EXPEDITION_CHAMPION:
            case NPC_VALLIANCE_EXPEDITION_GUARD:
            case NPC_WARSONG_GUARD:
            case NPC_WARSONG_CHAMPION:
                // ToDo: handle player scores when killing opposing faction npcs
                break;
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

bool BattlefieldWG::GetPlayerKickLocation(Player* player, float& x, float& y, float& z)
{
    return false;
}

void BattlefieldWG::UpdatePlayerOnWarResponse(Player* player)
{

}

// Function to handle the warnings
void BattlefieldWG::SendWintergraspWarning(int32 messageId, const WorldObject* objRef, uint32 soundId)
{
    // Attempt to get one stalker from the list; The battlefield expects at least one active stalker
    for (const auto& guid : m_stalkersGuids)
    {
        if (Creature* stalker = objRef->GetMap()->GetCreature(guid))
        {
            SendZoneWarning(stalker, messageId, soundId);
            break;
        }
    }
}

// Get player in zone
Player* BattlefieldWG::GetPlayerInZone()
{
    for (auto& m_zonePlayer : m_zonePlayers)
    {
        if (!m_zonePlayer.first)
            continue;

        Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
        if (player)
            return player;
    }

    return nullptr;
}

void BattlefieldWG::Update(uint32 diff)
{
    Battlefield::Update(diff);

    // send battle warning
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && m_timer <= 3 * MINUTE * IN_MILLISECONDS && !m_sentPrebattleWarning)
    {
        m_sentPrebattleWarning = true;
        if (Player* player = GetPlayerInZone())
            SendWintergraspWarning(LANG_OPVP_WG_ABOUT_TO_BEGIN, player);
    }

    // end battle on timer
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && m_timer < diff)
        EndBattle(GetDefender());
}

bool BattlefieldWG::IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType)
{
    switch (conditionId)
    {
        case OPVP_COND_WG_MAX_ALLIANCE_VEHICLES:
            return GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && uint32(m_vehicleGuids[TEAM_INDEX_ALLIANCE].size()) < m_workshopCount[TEAM_INDEX_ALLIANCE] * 4;
        case OPVP_COND_WG_MAX_HORDE_VEHICLES:
            return GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && uint32(m_vehicleGuids[TEAM_INDEX_HORDE].size()) < m_workshopCount[TEAM_INDEX_HORDE] * 4;
        case OPVP_COND_WG_BATTLEFIELD_IN_PROGRESS:
            return GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS;
        case OPVP_COND_WG_FORTRESS_ACCESS_ALLOWED:
            return source->GetTeam() == GetDefender();
        case OPVP_COND_WG_ALLOW_TELE_SUNKEN_RING:
        case OPVP_COND_WG_ALLOW_TELE_BROKEN_TEMPLE:
        case OPVP_COND_WG_ALLOW_TELE_EASTPARK:
        case OPVP_COND_WG_ALLOW_TELE_WESTPARK:
            // allow teleport only if the team owns the factory
            for (auto workshop : m_capturableWorkshops)
            {
                if (conditionId == workshop->GetConditionId())
                    return source->GetTeam() == workshop->GetOwner();
            }
            break;
    }

    return false;
}

void BattlefieldWG::HandleConditionStateChange(uint32 conditionId, bool state)
{

}
