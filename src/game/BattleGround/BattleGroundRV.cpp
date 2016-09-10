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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundRV.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

BattleGroundRV::BattleGroundRV()
{
    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    // we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

void BattleGroundRV::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (m_uiPillarTimer < diff)
    {
        DoSwitchPillars();
        m_uiPillarTimer = 20000;
    }
    else
        m_uiPillarTimer -= diff;
}

void BattleGroundRV::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);

    m_uiPillarTimer = 45000;
}

void BattleGroundRV::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundRVScore* sc = new BattleGroundRVScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
}

void BattleGroundRV::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

void BattleGroundRV::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog.outError("BattleGroundRV: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player, killer);

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

bool BattleGroundRV::HandlePlayerUnderMap(Player* player)
{
    player->TeleportTo(GetMapId(), 763.5f, -284, 28.276f, player->GetOrientation(), false);
    return true;
}

bool BattleGroundRV::HandleAreaTrigger(Player* player, uint32 triggerId)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return false;

    switch (triggerId)
    {
        case 5224:
        case 5226:
        case 5473:
        case 5474:
            break;
        default:
            return false;
    }
    return true;
}

void BattleGroundRV::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_RV_GO_ORG_ARENA_PILAR_1:
        case BG_RV_GO_ORG_ARENA_PILAR_2:
        case BG_RV_GO_ORG_ARENA_PILAR_3:
        case BG_RV_GO_ORG_ARENA_PILAR_4:
            m_lPillarsGuids.push_back(go->GetObjectGuid());
            break;
        case BG_RV_GO_ORG_ARENA_COLLISION_1:
        case BG_RV_GO_ORG_ARENA_COLLISION_2:
        case BG_RV_GO_ORG_ARENA_COLLISION_3:
        case BG_RV_GO_ORG_ARENA_COLLISION_4:
            m_lCollisionsGuids.push_back(go->GetObjectGuid());
            break;
        case BG_RV_GO_ORG_ARENA_GEAR_1:
        case BG_RV_GO_ORG_ARENA_GEAR_2:
        case BG_RV_GO_ORG_ARENA_PULLEY_1:
        case BG_RV_GO_ORG_ARENA_PULLEY_2:
            m_lAnimationsGuids.push_back(go->GetObjectGuid());
            break;
    }
}

void BattleGroundRV::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    FillInitialWorldState(data, count, 0xe10, GetAlivePlayersCountByTeam(HORDE));
    FillInitialWorldState(data, count, 0xe1a, 1);
}

void BattleGroundRV::DoSwitchPillars()
{
    // change collision state
    for (GuidList::const_iterator itr = m_lCollisionsGuids.begin(); itr != m_lCollisionsGuids.end(); ++itr)
    {
        if (GameObject* animGo = GetBgMap()->GetGameObject(*itr))
            animGo->SetGoState(animGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);
    }

    // change animations
    for (GuidList::const_iterator itr = m_lAnimationsGuids.begin(); itr != m_lAnimationsGuids.end(); ++itr)
    {
        if (GameObject* animGo = GetBgMap()->GetGameObject(*itr))
            animGo->SetGoState(animGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);
    }

    // change pillars
    for (GuidList::const_iterator itr = m_lPillarsGuids.begin(); itr != m_lPillarsGuids.end(); ++itr)
    {
        if (GameObject* pillarGo = GetBgMap()->GetGameObject(*itr))
            pillarGo->SetGoState(pillarGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);
    }
}
