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

#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include "Common.h"
#include "Entities/ObjectGuid.h"
#include "Globals/SharedDefines.h"
#include "OutdoorPvP/OutdoorPvP.h"

class Battlefield;
class Group;

enum BattlefieldStatus
{
    BF_STATUS_COOLDOWN      = 0,
    BF_STATUS_IN_PROGRESS   = 1
};

enum
{
    BF_INACTIVE_REMOVE_DELAY    = 5 * MINUTE,
    BF_UNACCEPTED_REMOVE_DELAY  = 10,
    BF_TIME_TO_ACCEPT           = 20,
};

enum BattlefieldGoState
{
    BF_GO_STATE_NONE                = 0,
    BF_GO_STATE_NEUTRAL_INTACT      = 1,
    BF_GO_STATE_NEUTRAL_DAMAGED     = 2,
    BF_GO_STATE_NEUTRAL_DESTROYED   = 3,
    BF_GO_STATE_HORDE_INTACT        = 4,
    BF_GO_STATE_HORDE_DAMAGED       = 5,
    BF_GO_STATE_HORDE_DESTROYED     = 6,
    BF_GO_STATE_ALLIANCE_INTACT     = 7,
    BF_GO_STATE_ALLIANCE_DAMAGED    = 8,
    BF_GO_STATE_ALLIANCE_DESTROYED  = 9,
};

class BattlefieldBuilding
{
    public:
        BattlefieldBuilding(uint32 goEntry) : entry(goEntry) {};

        virtual void SetupBuilding(Team owner, bool reset) {};

        bool IsIntact() { return state == BF_GO_STATE_NEUTRAL_INTACT || state == BF_GO_STATE_HORDE_INTACT || state == BF_GO_STATE_ALLIANCE_INTACT; };
        bool IsDamaged() { return state == BF_GO_STATE_NEUTRAL_DAMAGED || state == BF_GO_STATE_HORDE_DAMAGED || state == BF_GO_STATE_ALLIANCE_DAMAGED; };
        bool IsDestroyed() { return state == BF_GO_STATE_NEUTRAL_DESTROYED || state == BF_GO_STATE_HORDE_DESTROYED || state == BF_GO_STATE_ALLIANCE_DESTROYED; };

        Team GetOwner() { return owner; }
        void SetOwner(Team newOwner) { owner = newOwner; }

        uint32 GetWorldState() { return worldState; }
        void SetWorldState(uint32 newWorldState) { worldState = newWorldState; }

        uint32 GetGoState() { return (uint32)state; }
        void SetGoState(BattlefieldGoState newState) { state = newState; };

    private:
        Team owner;

        uint32 entry;
        uint32 worldState;

        BattlefieldGoState state;
        ObjectGuid guid;
};

class BattlefieldPlayer
{
    public:
        BattlefieldPlayer() : removeTime(0), removeDelay(0) {}

        time_t removeTime;
        uint32 removeDelay;
};

typedef std::map<ObjectGuid /*playerGuid*/, BattlefieldPlayer* /*playerData*/> BattlefieldPlayerMap;

class Battlefield : public OutdoorPvP
{
    public:
        Battlefield();

        // Player related stuff
        virtual void HandlePlayerEnterZone(Player* /*player*/, bool /*isMainZone*/) override;
        virtual void HandlePlayerLeaveZone(Player* /*player*/, bool /*isMainZone*/) override;

        // session response / requests
        void HandleQueueInviteResponse(Player* /*player*/, bool /*accepted*/);
        void HandleWarInviteResponse(Player* /*player*/, bool /*accepted*/);
        void HandleExitRequest(Player* /*player*/);

        // handle players which get AFK status
        void HandlePlayerAFK(Player* /*player*/);

        // handle player login (if already active)
        void HandlePlayerLoggedIn(Player* /*player*/);

        // handle battle start and end
        virtual void StartBattle(Team /*defender*/);
        virtual void EndBattle(Team /*winner*/, bool /*byTimer*/);

        virtual void Update(uint32 /*diff*/) override;

        // get defender or attacker team
        Team GetDefender() const { return m_zoneOwner; }
        Team GetAttacker() const { return m_zoneOwner == TEAM_NONE ? TEAM_NONE : m_zoneOwner == ALLIANCE ? HORDE : ALLIANCE; }

        // Group system related
        void RemovePlayerFromRaid(ObjectGuid playerGuid);
        void HandleBattlefieldGroupDisband(Player* player);
        bool HasPlayer(ObjectGuid playerGuid);
        bool CanDeleteBattlefieldGroup(Group* group);

        // get battlefield status
        BattlefieldStatus GetBattlefieldStatus() const { return m_status; }

        // get or set the cooldown/battle timer
        void SetTimer(uint32 value) { m_timer = value; }

        // get battlefield id
        uint32 GetBattlefieldId() const { return m_battleFieldId; }

    protected:
        // reset function for battlefields
        virtual void Reset() {};

        // store the player data for the current battlefield
        virtual void InitPlayerBattlefieldData(Player* /*player*/);

        // get the kick location for players depending on the current battlefield
        virtual bool GetPlayerKickLocation(Player* player, float& x, float& y, float& z) { return false; }

        // called when the battlefield player composition is updated
        virtual void OnBattlefieldPlayersUpdate() {};

        // called when the player accepts the battlefield teleport
        virtual void UpdatePlayerOnWarResponse(Player* /*player*/) {};

        // invite player to join battlefield queue
        void InvitePlayerToQueue(Player* /*player*/);

        // kick player from battlefield
        void KickBattlefieldPlayer(Player* /*player*/);

        // init all players on battle start
        void InitPlayersBeforeBattle();

        // refresh battlefield players
        void UpdateBattlefieldPlayers();

        // sends a raid warning to all players in the zone
        void SendZoneWarning(int32 /*entry*/, WorldObject* source = nullptr);

        // update the graveyard on capture event
        virtual void UpdateGraveyardOwner(uint8 id, PvpTeamIndex newOwner) {};

        // reward players
        virtual void RewardPlayersOnBattleEnd(Team /*winner*/) {};

        // ToDo!!! for review!
        void QuestCreditTeam(uint32 credit, Team team, WorldObject* source = nullptr, float radius = -1.0f);

        // Group system
        bool CanAddPlayerToRaid(Player* /*player*/);
        bool IsTeamFull(PvpTeamIndex /*teamIdx*/);
        Group* GetFreeRaid(PvpTeamIndex /*teamIdx*/);
        Group* GetGroupFor(ObjectGuid /*playerGuid*/);
        uint32 GetPlayerCountByTeam(PvpTeamIndex /*teamIdx*/);

        // load player positions
        void SetupPlayerPositions();
        virtual void SetupPlayerPosition(Player* player) { };

        // variables
        BattlefieldPlayerMap m_activePlayers;

        std::set<Group*> m_battlefieldRaids[PVP_TEAM_COUNT];
        std::set<ObjectGuid> m_queuedPlayers[PVP_TEAM_COUNT];           // players that are in queue
        std::map<ObjectGuid, time_t> m_invitedPlayers[PVP_TEAM_COUNT];  // player to whom teleport invitation is send and who are expected to accept

        uint32 m_battleFieldId;
        uint32 m_zoneId;
        uint32 m_queueUpdateTimer;
        uint32 m_playersUpdateTimer;

        BattlefieldStatus m_status;
        Team m_zoneOwner;
        uint32 m_timer;

        uint32 m_battleDuration;
        uint32 m_startInviteDelay;
        uint32 m_cooldownDuration;
        uint32 m_maxPlayersPerTeam;

        time_t m_startTime;

        bool m_playersInvited;
};

#endif
