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

BattlefieldWG::BattlefieldWG() : Battlefield(),
    m_zoneOwner(TEAM_NONE)
{
    m_zoneId = ZONE_ID_WINTERGRASP;
    m_battleFieldId = BATTLEFIELD_WG;

    Reset();
}

void BattlefieldWG::Reset()
{
    m_status = BF_STATUS_COOLDOWN;
}

void BattlefieldWG::FillInitialWorldStates(WorldPacket& data, uint32& count)
{

}

void BattlefieldWG::SendRemoveWorldStates(Player* player)
{

}

void BattlefieldWG::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    Battlefield::HandlePlayerEnterZone(player, isMainZone);
}

void BattlefieldWG::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    Battlefield::HandlePlayerLeaveZone(player, isMainZone);
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

bool BattlefieldWG::HandleEvent(uint32 uiEventId, GameObject* pGo)
{
    return false;
}

void BattlefieldWG::Update(uint32 diff)
{
    Battlefield::Update(diff);
}

void BattlefieldWG::StartBattle(Team defender)
{

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
