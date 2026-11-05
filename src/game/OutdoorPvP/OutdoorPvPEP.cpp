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

#include "OutdoorPvPEP.h"
#include "Server/WorldPacket.h"
#include "World/World.h"
#include "Globals/ObjectMgr.h"
#include "Entities/Object.h"
#include "Entities/Creature.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "World/WorldState.h"

OutdoorPvPEP::OutdoorPvPEP(Map& map, OutdoorPvPMgr& mgr) : OutdoorPvP(map, mgr)
{
    m_towerWorldState[0] = WORLD_STATE_EP_NORTHPASS_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_EP_CROWNGUARD_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_EP_EASTWALL_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_EP_PLAGUEWOOD_NEUTRAL;

    for (auto& i : m_towerOwner)
        i = TEAM_NONE;

    // initially set graveyard owner to neither faction
    SetGraveYardLinkTeam(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, TEAM_INVALID, 0);

    m_map.GetVariableManager().SetVariableData(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, true, ZONE_ID_EASTERN_PLAGUELANDS, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_EP_TOWER_COUNT_HORDE, true, ZONE_ID_EASTERN_PLAGUELANDS, 0);

    m_map.GetVariableManager().SetVariable(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_EP_TOWER_COUNT_HORDE, 0);

    for (auto& data : plaguelandsTowerEvents)
    {
        for (auto& towerData : data)
        {
            m_map.GetVariableManager().SetVariableData(towerData.worldState, true, ZONE_ID_EASTERN_PLAGUELANDS, 0);
        }
    }

    for (unsigned int i : m_towerWorldState)
    {
        m_map.GetVariableManager().SetVariable(i, WORLD_STATE_ADD);
    }
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerEnterZone(player, isMainZone);

    OutdoorPvPEP::BuffPlayer(player, m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE), m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_HORDE));
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    OutdoorPvPEP::UnBuffPlayer(player);

    OutdoorPvP::HandlePlayerLeaveZone(player, isMainZone);
}

void OutdoorPvPEP::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_SPECTRAL_FLIGHT_MASTER:
            m_flightMaster = creature->GetObjectGuid();
            creature->setFaction(m_towerOwner[TOWER_ID_PLAGUEWOOD] == ALLIANCE ? FACTION_FLIGHT_MASTER_ALLIANCE : FACTION_FLIGHT_MASTER_HORDE);
            creature->CastSpell(creature, m_towerOwner[TOWER_ID_PLAGUEWOOD] == ALLIANCE ? SPELL_SPIRIT_PARTICLES_BLUE : SPELL_SPIRIT_PARTICLES_RED, TRIGGERED_OLD_TRIGGERED);
            break;
        case NPC_LORDAERON_COMMANDER:
        case NPC_LORDAERON_SOLDIER:
        case NPC_LORDAERON_VETERAN:
        case NPC_LORDAERON_FIGHTER:
            m_soldiers.push_back(creature->GetObjectGuid());
            break;
    }
}

void OutdoorPvPEP::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);

    switch (go->GetEntry())
    {
        case GO_TOWER_BANNER_NORTHPASS:
            InitBanner(go, TOWER_ID_NORTHPASS);
            break;
        case GO_TOWER_BANNER_CROWNGUARD:
            InitBanner(go, TOWER_ID_CROWNGUARD);
            break;
        case GO_TOWER_BANNER_EASTWALL:
            InitBanner(go, TOWER_ID_EASTWALL);
            break;
        case GO_TOWER_BANNER_PLAGUEWOOD:
            InitBanner(go, TOWER_ID_PLAGUEWOOD);
            break;
        case GO_TOWER_BANNER:
            // sort banners
            if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_NORTHPASS][0], plaguelandsTowerLocations[TOWER_ID_NORTHPASS][1], 50.0f))
                InitBanner(go, TOWER_ID_NORTHPASS);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_CROWNGUARD][0], plaguelandsTowerLocations[TOWER_ID_CROWNGUARD][1], 50.0f))
                InitBanner(go, TOWER_ID_CROWNGUARD);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_EASTWALL][0], plaguelandsTowerLocations[TOWER_ID_EASTWALL][1], 50.0f))
                InitBanner(go, TOWER_ID_EASTWALL);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_PLAGUEWOOD][0], plaguelandsTowerLocations[TOWER_ID_PLAGUEWOOD][1], 50.0f))
                InitBanner(go, TOWER_ID_PLAGUEWOOD);
            break;
        case GO_LORDAERON_SHRINE_ALLIANCE:
            m_lordaeronShrineAlliance = go->GetObjectGuid();
            break;
        case GO_LORDAERON_SHRINE_HORDE:
            m_lordaeronShrineHorde = go->GetObjectGuid();
            break;
    }
}

void OutdoorPvPEP::HandleObjectiveComplete(uint32 eventId, const PlayerList& players, Team team)
{
    uint32 credit;
    switch (eventId)
    {
        case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
        case EVENT_CROWNGUARD_PROGRESS_HORDE:
            credit = NPC_CROWNGUARD_TOWER_QUEST_DOODAD;
            break;
        case EVENT_EASTWALL_PROGRESS_ALLIANCE:
        case EVENT_EASTWALL_PROGRESS_HORDE:
            credit = NPC_EASTWALL_TOWER_QUEST_DOODAD;
            break;
        case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
        case EVENT_NORTHPASS_PROGRESS_HORDE:
            credit = NPC_NORTHPASS_TOWER_QUEST_DOODAD;
            break;
        case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
        case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
            credit = NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD;
            break;
        default:
            return;
    }

    for (auto player : players)
    {
        if (player && player->GetTeam() == team)
        {
            player->KilledMonsterCredit(credit);
            player->RewardHonor(nullptr, 1, HONOR_REWARD_PLAGUELANDS);
        }
    }
}

// process the capture events
bool OutdoorPvPEP::HandleEvent(uint32 eventId, Object* source, Object* /*target*/)
{
    if (!source->IsGameObject())
        return false;

    GameObject* go = static_cast<GameObject*>(source);
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
    {
        if (plaguelandsBanners[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (plaguelandsTowerEvents[i][j].eventEntry == eventId)
                {
                    // prevent processing if the owner did not change (happens if progress event is called after contest event)
                    if (plaguelandsTowerEvents[i][j].team != m_towerOwner[i])
                    {
                        if (plaguelandsTowerEvents[i][j].defenseMessage)
                            sWorld.SendDefenseMessageBroadcastText(ZONE_ID_EASTERN_PLAGUELANDS, plaguelandsTowerEvents[i][j].defenseMessage);

                        return ProcessCaptureEvent(go, i, plaguelandsTowerEvents[i][j].team, plaguelandsTowerEvents[i][j].worldState);
                    }
                    // no need to iterate other events or towers
                    return false;
                }
            }
            // no need to iterate other towers
            return false;
        }
    }

    return false;
}

bool OutdoorPvPEP::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState)
{
    uint32 towersAlliance = m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE);
    uint32 towersHorde = m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_HORDE);
    // Remove existing buff for both teams before applying new one: this prevent auras stacking
    if (towersAlliance != 0)
        BuffTeam(ALLIANCE, plaguelandsTowerBuffs[towersAlliance - 1].spellIdAlliance, true);
    if (towersHorde != 0)
        BuffTeam(HORDE, plaguelandsTowerBuffs[towersHorde - 1].spellIdHorde, true);

    // Update world state and banners for captured tower
    if (team == ALLIANCE)
    {
        // update banner
        for (GuidList::const_iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        // update counter
        SetTowers(towersAlliance + 1, towersHorde);
    }
    else if (team == HORDE)
    {
        // update banner
        for (GuidList::const_iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        // update counter
        SetTowers(towersAlliance, towersHorde + 1);
    }
    else
    {
        // update banner
        for (GuidList::const_iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_towerOwner[towerId] == ALLIANCE)
        {
            // update counter
            SetTowers(towersAlliance - 1, towersHorde);
        }
        else
        {
            // update counter
            SetTowers(towersAlliance, towersHorde - 1);
        }
    }

    towersAlliance = m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE);
    towersHorde = m_map.GetVariableManager().GetVariable(WORLD_STATE_EP_TOWER_COUNT_HORDE);
    // Update Echoes of Lordaeron aura for capturing team and opposite team (if needed)
    // Apply auras with updated capture towers values
    if (towersAlliance != 0)
        BuffTeam(ALLIANCE, plaguelandsTowerBuffs[towersAlliance - 1].spellIdAlliance);
    if (towersHorde != 0)
        BuffTeam(HORDE, plaguelandsTowerBuffs[towersHorde - 1].spellIdHorde);

    // Update reward for captured tower
    bool eventHandled = true;

    if (team != TEAM_NONE)
    {
        // update capture point owner before rewards are applied
        m_towerOwner[towerId] = team;

        // apply rewards of changed tower
        switch (towerId)
        {
            case TOWER_ID_NORTHPASS:
                RespawnGO(go, team == ALLIANCE ? m_lordaeronShrineAlliance : m_lordaeronShrineHorde, true);
                break;
            case TOWER_ID_CROWNGUARD:
                SetGraveYardLinkTeam(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, team, 0);
                break;
            case TOWER_ID_EASTWALL:
                // Return false - allow the DB to handle summons
                if (m_towerOwner[TOWER_ID_NORTHPASS] != team)
                    eventHandled = false;
                break;
            case TOWER_ID_PLAGUEWOOD:
                // Return false - allow the DB to handle summons
                eventHandled = false;
                break;
        }
    }
    else
    {
        // remove rewards of changed tower
        switch (towerId)
        {
            case TOWER_ID_NORTHPASS:
                RespawnGO(go, m_towerOwner[TOWER_ID_NORTHPASS] == ALLIANCE ? m_lordaeronShrineAlliance : m_lordaeronShrineHorde, false);
                break;
            case TOWER_ID_CROWNGUARD:
                SetGraveYardLinkTeam(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, TEAM_INVALID, 0);
                break;
            case TOWER_ID_EASTWALL:
                UnsummonSoldiers(go);
                break;
            case TOWER_ID_PLAGUEWOOD:
                UnsummonFlightMaster(go);
                break;
        }

        // update capture point owner after rewards have been removed
        m_towerOwner[towerId] = team;
    }

    // update tower state
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // there are some events which required further DB script
    return eventHandled;
}

bool OutdoorPvPEP::HandleGameObjectUse(Player* /*player*/, GameObject* go)
{
    // prevent despawning after go use
    if (go->GetEntry() == GO_LORDAERON_SHRINE_ALLIANCE || go->GetEntry() == GO_LORDAERON_SHRINE_HORDE)
        go->SetRespawnTime(0);

    return false;
}

void OutdoorPvPEP::SetTowers(uint8 alliance, uint8 horde)
{
    m_map.GetVariableManager().SetVariable(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, alliance);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_EP_TOWER_COUNT_HORDE, horde);
    sWorldState.SetEplTowerData(horde, alliance);
}

void OutdoorPvPEP::BuffPlayer(Player* player, uint32 towersAlliance, uint32 towersHorde)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    for (auto plaguelandsTowerBuff : plaguelandsTowerBuffs)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? plaguelandsTowerBuff.spellIdAlliance : plaguelandsTowerBuff.spellIdHorde);

    // buff the player
    switch (player->GetTeam())
    {
        case ALLIANCE:
            if (towersAlliance)
                player->CastSpell(player, plaguelandsTowerBuffs[towersAlliance - 1].spellIdAlliance, TRIGGERED_OLD_TRIGGERED);
            break;
        case HORDE:
            if (towersHorde)
                player->CastSpell(player, plaguelandsTowerBuffs[towersHorde - 1].spellIdHorde, TRIGGERED_OLD_TRIGGERED);
            break;
        default:
            break;
    }
}

void OutdoorPvPEP::UnBuffPlayer(Player* player)
{
    // remove the buff from the player
    for (auto plaguelandsTowerBuff : plaguelandsTowerBuffs)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? plaguelandsTowerBuff.spellIdAlliance : plaguelandsTowerBuff.spellIdHorde);
}

void OutdoorPvPEP::InitBanner(GameObject* go, uint32 towerId)
{
    m_towerBanners[towerId].push_back(go->GetObjectGuid());
    go->SetGoArtKit(GetBannerArtKit(m_towerOwner[towerId]));
}

// Handle the unsummon of the spectral flight master when the Plaguewood tower is lost
void OutdoorPvPEP::UnsummonFlightMaster(const WorldObject* objRef)
{
    if (Creature* flightMaster = objRef->GetMap()->GetCreature(m_flightMaster))
        flightMaster->ForcedDespawn();
}

// Handle the unsummon of the soldiers when the Eastwall tower is lost
void OutdoorPvPEP::UnsummonSoldiers(const WorldObject* objRef)
{
    for (GuidList::const_iterator itr = m_soldiers.begin(); itr != m_soldiers.end(); ++itr)
    {
        if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
            soldier->ForcedDespawn();
    }

    m_soldiers.clear();
}
