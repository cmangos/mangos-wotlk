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

#include "OutdoorPvPTF.h"
#include "Server/WorldPacket.h"
#include "World/World.h"
#include "Globals/ObjectMgr.h"
#include "Entities/Object.h"
#include "Entities/Creature.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "World/WorldState.h"

OutdoorPvPTF::OutdoorPvPTF(Map& map, OutdoorPvPMgr& mgr) : OutdoorPvP(map, mgr),
    m_zoneWorldState(WORLD_STATE_TF_TOWERS_CONTROLLED),
    m_zoneOwner(TEAM_NONE),
    //m_zoneUpdateTimer(TIMER_TF_UPDATE_TIME),
    m_zoneLockTimer(0)
{
    m_towerWorldState[0] = WORLD_STATE_TF_WEST_TOWER_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_TF_NORTH_TOWER_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_TF_EAST_TOWER_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_TF_SOUTH_EAST_TOWER_NEUTRAL;
    m_towerWorldState[4] = WORLD_STATE_TF_SOUTH_TOWER_NEUTRAL;

    for (auto& i : m_towerOwner)
        i = TEAM_NONE;

    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT, true, ZONE_ID_TEROKKAR_FOREST, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT, true, ZONE_ID_TEROKKAR_FOREST, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TIME_HOURS, true, ZONE_ID_TEROKKAR_FOREST, 0);

    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TOWERS_CONTROLLED, true, ZONE_ID_TEROKKAR_FOREST, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_LOCKED_ALLIANCE, true, ZONE_ID_TEROKKAR_FOREST, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_LOCKED_HORDE, true, ZONE_ID_TEROKKAR_FOREST, 0);

    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TOWER_COUNT_H, true, ZONE_ID_TEROKKAR_FOREST, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_TF_TOWER_COUNT_A, true, ZONE_ID_TEROKKAR_FOREST, 0);

    for (auto& data : terokkarTowerEvents)
    {
        for (auto& towerData : data)
        {
            m_map.GetVariableManager().SetVariableData(towerData.worldState, true, ZONE_ID_TEROKKAR_FOREST, 0);
        }
    }

    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_H, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_A, 0);

    for (unsigned int i : m_towerWorldState)
        m_map.GetVariableManager().SetVariable(i, WORLD_STATE_ADD);
}

void OutdoorPvPTF::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerEnterZone(player, isMainZone);

    OutdoorPvPTF::UnBuffPlayer(player);
    OutdoorPvPTF::BuffPlayer(player, m_zoneOwner);
}

void OutdoorPvPTF::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    OutdoorPvPTF::UnBuffPlayer(player);

    OutdoorPvP::HandlePlayerLeaveZone(player, isMainZone);
}

void OutdoorPvPTF::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);

    switch (go->GetEntry())
    {
        case GO_TOWER_BANNER_WEST:
            m_towerBanners[0] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[0]));
            break;
        case GO_TOWER_BANNER_NORTH:
            m_towerBanners[1] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[1]));
            break;
        case GO_TOWER_BANNER_EAST:
            m_towerBanners[2] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[2]));
            break;
        case GO_TOWER_BANNER_SOUTH_EAST:
            m_towerBanners[3] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[3]));
            break;
        case GO_TOWER_BANNER_SOUTH:
            m_towerBanners[4] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[4]));
            break;
    }
}

void OutdoorPvPTF::HandleObjectiveComplete(uint32 eventId, const PlayerList& players, Team team)
{
    for (const auto& terokkarTowerEvent : terokkarTowerEvents)
    {
        for (uint8 j = 0; j < 4; ++j)
        {
            if (terokkarTowerEvent[j].eventEntry == eventId)
            {
                for (auto player : players)
                {
                    if (player && player->GetTeam() == team)
                        player->AreaExploredOrEventHappens(team == ALLIANCE ? QUEST_SPIRITS_OF_AUCHINDOUM_ALLIANCE : QUEST_SPIRITS_OF_AUCHINDOUM_HORDE);
                }
                return;
            }
        }
    }
}

// process the capture events
bool OutdoorPvPTF::HandleEvent(uint32 eventId, Object* source, Object* /*target*/)
{
    if (!source->IsGameObject())
        return false;

    GameObject* go = static_cast<GameObject*>(source);
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (terokkarTowers[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (terokkarTowerEvents[i][j].eventEntry == eventId)
                {
                    // prevent processing if the owner did not change (happens if progress event is called after contest event)
                    if (terokkarTowerEvents[i][j].team != m_towerOwner[i])
                    {
                        sWorld.SendDefenseMessage(ZONE_ID_TEROKKAR_FOREST, terokkarTowerEvents[i][j].defenseMessage);

                        return ProcessCaptureEvent(go, i, terokkarTowerEvents[i][j].team, terokkarTowerEvents[i][j].worldState);
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

bool OutdoorPvPTF::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState)
{
    if (team == ALLIANCE)
    {
        // update banner
        SetBannerVisual(go, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        // update tower count
        uint32 towersAlliance = m_map.GetVariableManager().GetVariable(WORLD_STATE_TF_TOWER_COUNT_A);

        // if all towers are captured then process event
        if (towersAlliance + 1 == MAX_TF_TOWERS)
        {
            LockZone(go, towerId, team, newWorldState);
            return true;
        }

        // update tower count world state
        m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_A, towersAlliance + 1);
    }
    else if (team == HORDE)
    {
        // update banner
        SetBannerVisual(go, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        // update tower count
        uint32 towersAlliance = m_map.GetVariableManager().GetVariable(WORLD_STATE_TF_TOWER_COUNT_H);

        // if all towers are captured then process event
        if (towersAlliance + 1 == MAX_TF_TOWERS)
        {
            LockZone(go, towerId, team, newWorldState);
            return true;
        }

        // update tower count world state
        m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_H, towersAlliance + 1);
    }
    else
    {
        // update banner
        SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        // update tower count
        if (m_towerOwner[towerId] == ALLIANCE)
        {
            m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_A, m_map.GetVariableManager().GetVariable(WORLD_STATE_TF_TOWER_COUNT_A) - 1);
        }
        else
        {
            m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_H, m_map.GetVariableManager().GetVariable(WORLD_STATE_TF_TOWER_COUNT_H) - 1);
        }
    }

    // update tower state
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // update capture point owner
    m_towerOwner[towerId] = team;

    // the are no DB exceptions in this case
    return true;
}

// Handle the zone lock when the timer is activated
void OutdoorPvPTF::LockZone(GameObject* go, uint32 towerId, Team team, uint32 newWorldState)
{
    m_map.GetVariableManager().SetVariable(m_zoneWorldState, WORLD_STATE_REMOVE);
    m_zoneWorldState = team == ALLIANCE ? WORLD_STATE_TF_LOCKED_ALLIANCE : WORLD_STATE_TF_LOCKED_HORDE;
    m_map.GetVariableManager().SetVariable(m_zoneWorldState, WORLD_STATE_ADD);

    m_zoneLockTimer = TIMER_TF_LOCK_TIME;
    UpdateTimerWorldState();

    m_zoneOwner = team;
    sWorldState.SetTerokkarZoneOwner(team);
    BuffTeam(team, SPELL_AUCHINDOUN_BLESSING);

    // lock the towers
    LockTowers(go);

    sWorld.SendDefenseMessage(ZONE_ID_TEROKKAR_FOREST, team == ALLIANCE ? LANG_OPVP_TF_CAPTURE_ALL_TOWERS_A : LANG_OPVP_TF_CAPTURE_ALL_TOWERS_H);

    // remove tower states when zone has been captured and locked
    for (unsigned int i : m_towerWorldState)
        m_map.GetVariableManager().SetVariable(i, WORLD_STATE_REMOVE);

    m_towerWorldState[towerId] = newWorldState;
}

// Handle the zone reset when the timer expires
void OutdoorPvPTF::UnlockZone()
{
    // remove buffs
    BuffTeam(m_zoneOwner, SPELL_AUCHINDOUN_BLESSING, true);

    m_zoneOwner = TEAM_NONE;
    sWorldState.SetTerokkarZoneOwner(TEAM_NONE);

    // reset world states and towers
    m_map.GetVariableManager().SetVariable(m_zoneWorldState, WORLD_STATE_REMOVE);
    m_zoneWorldState = WORLD_STATE_TF_TOWERS_CONTROLLED;
    m_map.GetVariableManager().SetVariable(m_zoneWorldState, WORLD_STATE_ADD);

    // reset tower states
    m_towerWorldState[0] = WORLD_STATE_TF_WEST_TOWER_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_TF_NORTH_TOWER_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_TF_EAST_TOWER_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_TF_SOUTH_EAST_TOWER_NEUTRAL;
    m_towerWorldState[4] = WORLD_STATE_TF_SOUTH_TOWER_NEUTRAL;
    for (unsigned int i : m_towerWorldState)
        m_map.GetVariableManager().SetVariable(i, WORLD_STATE_ADD);

    // update tower count
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_A, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TOWER_COUNT_H, 0);

    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_HOURS, 0);

    for (GuidZoneMap::const_iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        // Find player who is in main zone (Terokkar Forest) to get correct map reference
        if (!itr->second)
            continue;

        if (Player* player = sObjectMgr.GetPlayer(itr->first))
        {
            ResetTowers(player);
            break;
        }
    }
}

void OutdoorPvPTF::Update(uint32 diff)
{
    if (m_zoneLockTimer)
    {
        if (m_zoneLockTimer < diff)
        {
            UnlockZone();
            m_zoneLockTimer = 0;
        }
        else
        {
            // update timer - if OutdoorPvPMgr update timer interval needs to be lowered replace this line with the commented-out ones below
            UpdateTimerWorldState();

            /*if (m_zoneUpdateTimer < diff)
            {
                // update timer
                UpdateTimerWorldState();
                m_zoneUpdateTimer = TIMER_TF_UPDATE_TIME;
            }
            else
                m_zoneUpdateTimer -= diff;*/

            m_zoneLockTimer -= diff;
        }
    }
}

void OutdoorPvPTF::BuffPlayer(Player* player, uint32 zoneOwner)
{
    if (player->GetTeam() == zoneOwner)
        player->CastSpell(player, SPELL_AUCHINDOUN_BLESSING, TRIGGERED_OLD_TRIGGERED);
}

void OutdoorPvPTF::UnBuffPlayer(Player* player)
{
    player->RemoveAurasDueToSpell(SPELL_AUCHINDOUN_BLESSING);
}

void OutdoorPvPTF::UpdateTimerWorldState()
{
    // Calculate time
    uint32 firstDigit, secondDigit, hoursLeft;
    CalculateTimerWorldStateValues(firstDigit, secondDigit, hoursLeft);

    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT, firstDigit);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT, secondDigit);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_TF_TIME_HOURS, hoursLeft);
}

void OutdoorPvPTF::CalculateTimerWorldStateValues(uint32& firstDigit, uint32& secondDigit, uint32& hoursLeft)
{
    uint32 minutesLeft = m_zoneLockTimer / 60000;
    hoursLeft = minutesLeft / 60;
    minutesLeft -= hoursLeft * 60;
    firstDigit = minutesLeft / 10;
    secondDigit = minutesLeft - firstDigit * 10;
}

// Handle the Terokkar towers lock during the update timer
void OutdoorPvPTF::LockTowers(const WorldObject* objRef)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (GameObject* go = objRef->GetMap()->GetGameObject(m_towerBanners[i]))
            go->SetLootState(GO_JUST_DEACTIVATED);
        else
        {
            // if grid is unloaded, changing the saved slider value is enough
            CapturePointSlider value(m_zoneOwner == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE, true);
            m_mgr.SetCapturePointSlider(terokkarTowers[i], value);
        }
    }
}

// Handle towers reset when the timer expires
void OutdoorPvPTF::ResetTowers(const WorldObject* objRef)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (GameObject* go = objRef->GetMap()->GetGameObject(m_towerBanners[i]))
        {
            go->SetCapturePointSlider(CAPTURE_SLIDER_MIDDLE, false);
            // visual update needed because banner still has artkit from previous owner
            SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
        }
        else
        {
            // if grid is unloaded, changing the saved slider value is enough
            CapturePointSlider value(CAPTURE_SLIDER_MIDDLE, false);
            m_mgr.SetCapturePointSlider(terokkarTowers[i], value);
        }
    }
}
