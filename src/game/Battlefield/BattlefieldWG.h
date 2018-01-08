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

#ifndef WORLD_PVP_WG
#define WORLD_PVP_WG

#include "Common.h"
#include "Battlefield.h"
#include "Tools/Language.h"
#include "World/World.h"

class Group;
class BattlefieldWG;

enum
{
    // ToDo: add the script defines
};

class BattlefieldWG : public Battlefield
{
    public:
        BattlefieldWG();

        void Reset();

        void HandlePlayerEnterZone(Player* player, bool isMainZone) override;
        void HandlePlayerLeaveZone(Player* player, bool isMainZone) override;
        void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
        void SendRemoveWorldStates(Player* player) override;

        bool HandleEvent(uint32 eventId, GameObject* go) override;

        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;
        void HandleCreatureDeath(Creature* creature) override;

        void HandlePlayerKillInsideArea(Player* player, Unit* victim) override;
        void Update(uint32 diff) override;

        void StartBattle(Team defender) override;
        void EndBattle(Team winner, bool byTimer) override;
        void RewardPlayersOnBattleEnd(Team winner) override;

        void UpdatePlayerOnWarResponse(Player* player) override;

        void UpdateGraveyardOwner(uint8 id, PvpTeamIndex newOwner) override;

    private:
        void InitPlayerBattlefieldData(Player* player) override;
        bool GetPlayerKickLocation(Player* player, float& x, float& y, float& z) override;

        void SetupPlayerPosition(Player* player) override;

        Team m_zoneOwner;
};

#endif
