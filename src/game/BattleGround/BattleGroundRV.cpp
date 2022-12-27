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

#include "Entities/Object.h"
#include "Entities/Player.h"
#include "BattleGround.h"
#include "BattleGroundRV.h"
#include "Tools/Language.h"
#include "Globals/ObjectMgr.h"
#include "Server/WorldPacket.h"

BattleGroundRV::BattleGroundRV(): m_uiPillarTimer(0)
{
    // set start delay timers
    m_startDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_startDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_startDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_startDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;

    // set arena start message id
    m_startMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_startMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_startMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
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

void BattleGroundRV::DoSwitchPillars()
{
    // change collision state
    for (const auto& guid : m_lCollisionsGuids)
        if (GameObject* animGo = GetBgMap()->GetGameObject(guid))
            animGo->SetGoState(animGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);

    // change animations
    for (const auto& guid : m_lAnimationsGuids)
        if (GameObject* animGo = GetBgMap()->GetGameObject(guid))
            animGo->SetGoState(animGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);

    // change pillars
    for (const auto& guid : m_lPillarsGuids)
        if (GameObject* pillarGo = GetBgMap()->GetGameObject(guid))
            pillarGo->SetGoState(pillarGo->GetGoState() == GO_STATE_READY ? GO_STATE_ACTIVE : GO_STATE_READY);
}
