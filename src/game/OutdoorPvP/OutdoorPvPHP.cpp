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

#include "OutdoorPvPHP.h"
#include "Server/WorldPacket.h"
#include "World/World.h"
#include "Entities/Object.h"
#include "Entities/Creature.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "World/WorldState.h"

OutdoorPvPHP::OutdoorPvPHP(Map& map, OutdoorPvPMgr& mgr) : OutdoorPvP(map, mgr)
{
    m_towerWorldState[0] = WORLD_STATE_HP_OVERLOOK_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_HP_STADIUM_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_HP_BROKEN_HILL_NEUTRAL;

    for (auto& i : m_towerOwner)
        i = TEAM_NONE;

    m_map.GetVariableManager().SetVariableData(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE, true, ZONE_ID_HELLFIRE_PENINSULA, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_HP_TOWER_COUNT_HORDE, true, ZONE_ID_HELLFIRE_PENINSULA, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_HP_TOWER_DISPLAY_A, true, ZONE_ID_HELLFIRE_PENINSULA, 0);
    m_map.GetVariableManager().SetVariableData(WORLD_STATE_HP_TOWER_DISPLAY_H, true, ZONE_ID_HELLFIRE_PENINSULA, 0);

    for (auto& data : hellfireTowerEvents)
        for (auto& towerData : data)
            m_map.GetVariableManager().SetVariableData(towerData.worldState, true, ZONE_ID_HELLFIRE_PENINSULA, 0);

    m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE, 0);
    m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE, 0);

    for (auto& towerWorldState : m_towerWorldState)
        m_map.GetVariableManager().SetVariable(towerWorldState, WORLD_STATE_ADD);
}

void OutdoorPvPHP::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerEnterZone(player, isMainZone);

    Team zoneOwner = TEAM_NONE;
    if (m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE) == MAX_HP_TOWERS && player->GetTeam() == ALLIANCE)
        zoneOwner = ALLIANCE;
    else if (m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE) == MAX_HP_TOWERS && player->GetTeam() == HORDE)
        zoneOwner = HORDE;

    OutdoorPvPHP::BuffPlayer(player, zoneOwner);
}

void OutdoorPvPHP::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    OutdoorPvPHP::UnBuffPlayer(player);

    OutdoorPvP::HandlePlayerLeaveZone(player, isMainZone);
}

void OutdoorPvPHP::HandleGameObjectCreate(GameObject* go)
{
    OutdoorPvP::HandleGameObjectCreate(go);

    switch (go->GetEntry())
    {
        case GO_TOWER_BANNER_OVERLOOK:
            m_banners[0] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[0], GO_ARTKIT_OVERLOOK_ALLIANCE, GO_ARTKIT_OVERLOOK_HORDE, GO_ARTKIT_OVERLOOK_NEUTRAL));
            break;
        case GO_TOWER_BANNER_STADIUM:
            m_banners[1] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[1], GO_ARTKIT_STADIUM_ALLIANCE, GO_ARTKIT_STADIUM_HORDE, GO_ARTKIT_STADIUM_NEUTRAL));
            break;
        case GO_TOWER_BANNER_BROKEN_HILL:
            m_banners[2] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[2], GO_ARTKIT_BROKEN_HILL_ALLIANCE, GO_ARTKIT_BROKEN_HILL_HORDE, GO_ARTKIT_BROKEN_HILL_NEUTRAL));
            break;
        case GO_HELLFIRE_BANNER_OVERLOOK:
            m_towers[0] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[0]));
            break;
        case GO_HELLFIRE_BANNER_STADIUM:
            m_towers[1] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[1]));
            break;
        case GO_HELLFIRE_BANNER_BROKEN_HILL:
            m_towers[2] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[2]));
            break;
    }
}

void OutdoorPvPHP::HandleObjectiveComplete(uint32 eventId, const PlayerList& players, Team team)
{
    uint32 credit;
    switch (eventId)
    {
        case EVENT_OVERLOOK_PROGRESS_ALLIANCE:
        case EVENT_OVERLOOK_PROGRESS_HORDE:
            credit = NPC_CAPTURE_CREDIT_OVERLOOK;
            break;
        case EVENT_STADIUM_PROGRESS_ALLIANCE:
        case EVENT_STADIUM_PROGRESS_HORDE:
            credit = NPC_CAPTURE_CREDIT_STADIUM;
            break;
        case EVENT_BROKEN_HILL_PROGRESS_ALLIANCE:
        case EVENT_BROKEN_HILL_PROGRESS_HORDE:
            credit = NPC_CAPTURE_CREDIT_BROKEN_HILL;
            break;
        default:
            return;
    }

    for (auto player : players)
    {
        if (player && player->GetTeam() == team)
        {
            player->KilledMonsterCredit(credit);
            player->RewardHonor(nullptr, 1, HONOR_REWARD_HELLFIRE);
        }
    }
}

// Cast player spell on opponent kill
void OutdoorPvPHP::HandlePlayerKillInsideArea(Player* player, Unit* victim)
{
    for (auto m_tower : m_towers)
    {
        if (GameObject* capturePoint = player->GetMap()->GetGameObject(m_tower))
        {
            // check capture point range
            GameObjectInfo const* info = capturePoint->GetGOInfo();
            if (info && player->IsWithinDistInMap(capturePoint, info->capturePoint.radius))
            {
                player->CastSpell(nullptr, player->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_TOWER_TOKEN_ALLIANCE : SPELL_HELLFIRE_TOWER_TOKEN_HORDE, TRIGGERED_OLD_TRIGGERED);
                return;
            }
        }
    }
}

void OutdoorPvPHP::BuffPlayer(Player* player, uint32 zoneOwner)
{
    if (zoneOwner == ALLIANCE && player->GetTeam() == ALLIANCE)
        player->CastSpell(player, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE, TRIGGERED_OLD_TRIGGERED);
    else if (zoneOwner == HORDE && player->GetTeam() == HORDE)
        player->CastSpell(player, SPELL_HELLFIRE_SUPERIORITY_HORDE, TRIGGERED_OLD_TRIGGERED);
}

void OutdoorPvPHP::UnBuffPlayer(Player* player)
{
    player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_SUPERIORITY_ALLIANCE : SPELL_HELLFIRE_SUPERIORITY_HORDE);
}

// process the capture events
bool OutdoorPvPHP::HandleEvent(uint32 eventId, Object* source, Object* /*target*/)
{
    if (!source->IsGameObject())
        return false;

    GameObject* go = static_cast<GameObject*>(source);
    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
    {
        if (hellfireBanners[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (hellfireTowerEvents[i][j].eventEntry == eventId)
                {
                    // prevent processing if the owner did not change (happens if progress event is called after contest event)
                    if (hellfireTowerEvents[i][j].team != m_towerOwner[i])
                    {
                        if (hellfireTowerEvents[i][j].defenseMessage)
                            sWorld.SendDefenseMessage(ZONE_ID_HELLFIRE_PENINSULA, hellfireTowerEvents[i][j].defenseMessage);

                        return ProcessCaptureEvent(go, i, hellfireTowerEvents[i][j].team, hellfireTowerEvents[i][j].worldState, hellfireTowerEvents[i][j].towerArtKit, hellfireTowerEvents[i][j].towerAnim);
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

bool OutdoorPvPHP::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState, uint32 towerArtKit, uint32 towerAnim)
{
    // set artkits and process buffs
    if (team == ALLIANCE)
    {
        SetBannerVisual(go, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        // update counter state
        uint32 controlledTowers = m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE) + 1;
        m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE, controlledTowers);

        if (controlledTowers == MAX_HP_TOWERS)
        {
            BuffTeam(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE);
            sWorldState.SetHellfireZoneOwner(ALLIANCE);
        }
    }
    else if (team == HORDE)
    {
        SetBannerVisual(go, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        // update counter
        uint32 controlledTowers = m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE) + 1;
        m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE, controlledTowers);

        if (controlledTowers == MAX_HP_TOWERS)
        {
            BuffTeam(HORDE, SPELL_HELLFIRE_SUPERIORITY_HORDE);
            sWorldState.SetHellfireZoneOwner(HORDE);
        }
    }
    else
    {
        SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_towerOwner[towerId] == ALLIANCE)
        {
            uint32 controlledTowers = m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE);
            if (controlledTowers == MAX_HP_TOWERS)
            {
                BuffTeam(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE, true);
                sWorldState.SetHellfireZoneOwner(TEAM_NONE);
            }

            // update counter
            m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_ALLIANCE, controlledTowers - 1);
        }
        else
        {
            uint32 controlledTowers = m_map.GetVariableManager().GetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE);
            if (controlledTowers == MAX_HP_TOWERS)
            {
                BuffTeam(HORDE, SPELL_HELLFIRE_SUPERIORITY_HORDE, true);
                sWorldState.SetHellfireZoneOwner(TEAM_NONE);
            }

            // update counter
            m_map.GetVariableManager().SetVariable(WORLD_STATE_HP_TOWER_COUNT_HORDE, controlledTowers - 1);
        }
    }

    SetBannerVisual(go, m_banners[towerId], towerArtKit, towerAnim);

    // update tower state
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    m_map.GetVariableManager().SetVariable(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // update capture point owner
    m_towerOwner[towerId] = team;

    // the are no DB exceptions in this case
    return true;
}
