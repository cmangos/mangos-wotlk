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

#include "Battlefield.h"
#include "Tools/Language.h"
#include "World/World.h"
#include "Globals/ObjectMgr.h"
#include "Entities/Object.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Maps/MapManager.h"
#include "Spells/SpellAuras.h"
#include "Entities/Unit.h"
#include "Chat/Chat.h"

Battlefield::Battlefield() : OutdoorPvP(), m_battleFieldId(0), m_zoneId(0), m_queueUpdateTimer(0), m_playersUpdateTimer(0), m_status(BF_STATUS_COOLDOWN), m_zoneOwner(TEAM_NONE), m_timer(0), m_startTime(0), m_playersInvited(false)
{
    m_isBattlefield = true;

    // init generic timers for all battlefields
    m_battleDuration = sWorld.getConfig(CONFIG_UINT32_BATTLEFIELD_BATTLE_DURATION) * MINUTE * IN_MILLISECONDS;
    m_cooldownDuration = sWorld.getConfig(CONFIG_UINT32_BATTLEFIELD_COOLDOWN_DURATION) * MINUTE * IN_MILLISECONDS;
    m_maxPlayersPerTeam = sWorld.getConfig(CONFIG_UINT32_BATTLEFIELD_MAX_PLAYERS_PER_TEAM);

    // invite delay is calculated as 10 percent of cooldown duration
    m_startInviteDelay = m_cooldownDuration * .1;
}

/**
  Function that adds a player to the players of the affected battlefield zones

  @param   player to add
  @param   whether zone is main outdoor pvp zone or a affected zone
*/
void Battlefield::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerEnterZone(player, isMainZone);

    // invite to group or queue
    if (!HasPlayer(player->GetObjectGuid()))
    {
        if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
        {
            m_invitedPlayers[GetTeamIndexByTeamId(player->GetTeam())][player->GetObjectGuid()] = time(nullptr) + BF_TIME_TO_ACCEPT;
            player->GetSession()->SendBattlefieldWarInvite(m_battleFieldId, m_zoneId, BF_TIME_TO_ACCEPT);
        }
        else if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && m_timer < m_startInviteDelay)
            InvitePlayerToQueue(player);
    }

    // check if player is already marked as active
    BattlefieldPlayerDataMap::iterator itr = m_activePlayers.find(player->GetObjectGuid());
    if (itr == m_activePlayers.end())
        InitPlayerBattlefieldData(player);
    else
    {
        if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && itr->second->removeTime && m_startTime > itr->second->removeTime)
            SetupPlayerPosition(player);

        itr->second->removeTime = 0;
    }
}

/**
  Function that removes a player from the players of the affected battlefield zones

  @param   player to remove
  @param   whether zone is main outdoor pvp zone or a affected zone
*/
void Battlefield::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerLeaveZone(player, isMainZone);

    BattlefieldPlayerDataMap::iterator itr = m_activePlayers.find(player->GetObjectGuid());
    if (itr != m_activePlayers.end())
    {
        // remove from battlefield if active
        if (HasPlayer(itr->first))
        {
            player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
            itr->second->removeTime = time(nullptr);
            itr->second->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
        }
        else
            m_activePlayers.erase(itr);
    }
}

/**
  Function that initializes the player data

  @param   player to load
*/
void Battlefield::InitPlayerBattlefieldData(Player* player)
{
    m_activePlayers[player->GetObjectGuid()] = new BattlefieldPlayer();
}

/**
  Function that handles the AFK status

  @param   player to be punished
*/
void Battlefield::HandlePlayerAFK(Player* player)
{
    if (GetBattlefieldStatus() != BF_STATUS_IN_PROGRESS || !HasPlayer(player->GetObjectGuid()))
        return;

    RemovePlayerFromRaid(player->GetObjectGuid());
    player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
    KickBattlefieldPlayer(player);
}

/**
  Function that handles the player which just logged in

  @param   player
*/
void Battlefield::HandlePlayerLoggedIn(Player* player)
{
    if (Group* group = GetGroupFor(player->GetObjectGuid()))
    {
        uint8 subgroup = group->GetMemberGroup(player->GetObjectGuid());
        player->SetBattleRaid(group, subgroup);
    }
}

/**
  Function that initializes all players before the battle starts
*/
void Battlefield::InitPlayersBeforeBattle()
{
    for (auto& m_activePlayer : m_activePlayers)
        delete m_activePlayer.second;

    m_activePlayers.clear();

    for (auto& m_zonePlayer : m_zonePlayers)
    {
        if (!m_zonePlayer.first)
            continue;

        Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
        if (!player)
            continue;

        InitPlayerBattlefieldData(player);
    }
}

/**
  Function that refresh all battlefield players
*/
void Battlefield::UpdateBattlefieldPlayers()
{
    bool bRemoved = false;
    for (BattlefieldPlayerDataMap::iterator itr = m_activePlayers.begin(); itr != m_activePlayers.end();)
    {
        if (itr->second->removeTime && itr->second->removeTime + itr->second->removeDelay < time(nullptr))
        {
            // kick player from zone
            delete itr->second;
            if (Player* player = sObjectMgr.GetPlayer(itr->first))
                KickBattlefieldPlayer(player);

            // remove player from group
            if (HasPlayer(itr->first))
            {
                RemovePlayerFromRaid(itr->first);
                bRemoved = true;
            }

            m_activePlayers.erase(itr++);

        }
        else
            ++itr;
    }

    // call the update function for specific script action
    if (bRemoved)
        OnBattlefieldPlayersUpdate();
}

/**
  Function that starts battlefield battle

  @param   defender team
*/
void Battlefield::StartBattle(Team defender)
{
    DEBUG_LOG("Battlefield: Battle has started. Defender Team: %s.", defender == ALLIANCE ? "Alliance" : "Horde");

    m_startTime     = time(nullptr);
    m_status        = BF_STATUS_IN_PROGRESS;
    m_timer         = m_battleDuration;
    m_zoneOwner     = defender;

    // refresh all players before battle
    InitPlayersBeforeBattle();

    // disband existing raid groups
    for (auto& m_battlefieldRaid : m_battlefieldRaids)
    {
        for (auto group : m_battlefieldRaid)
        {
            group->Disband();
            delete group;
        }
        m_battlefieldRaid.clear();
    }

    for (auto& m_zonePlayer : m_zonePlayers)
    {
        if (!m_zonePlayer.first)
            continue;

        Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
        if (!player)
            continue;

        uint8 idx = 0;
        std::set<ObjectGuid>::iterator itr2 = m_queuedPlayers[idx].find(m_zonePlayer.first);
        if (itr2 == m_queuedPlayers[idx].end())
        {
            ++idx;
            itr2 = m_queuedPlayers[idx].find(m_zonePlayer.first);
        }

        // send battle invite to players added to queue
        if (itr2 != m_queuedPlayers[idx].end())
        {
            m_invitedPlayers[idx][m_zonePlayer.first] = time(nullptr) + BF_TIME_TO_ACCEPT;
            player->GetSession()->SendBattlefieldWarInvite(m_battleFieldId, m_zoneId, BF_TIME_TO_ACCEPT);
            m_queuedPlayers[idx].erase(itr2);
        }
        // kick players not in queue
        else
        {
            player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
            SendRemoveWorldStates(player);
            if (m_activePlayers.find(m_zonePlayer.first) != m_activePlayers.end())
            {
                m_activePlayers[m_zonePlayer.first]->removeTime = time(nullptr);
                m_activePlayers[m_zonePlayer.first]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
            }
        }
    }

    // non-zone queued players
    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (std::set<ObjectGuid>::iterator itr = m_queuedPlayers[i].begin(); itr != m_queuedPlayers[i].end();)
        {
            if (Player* plr = sObjectMgr.GetPlayer(*itr, true))
            {
                m_invitedPlayers[i][plr->GetObjectGuid()] = time(nullptr) + BF_TIME_TO_ACCEPT;
                plr->GetSession()->SendBattlefieldWarInvite(m_battleFieldId, m_zoneId, BF_TIME_TO_ACCEPT);
            }

            m_queuedPlayers[i].erase(itr++);
        }
    }
}

/**
  Function that ends battlefield battle

  @param   winner team
  @param   reference object
*/
void Battlefield::EndBattle(Team winner)
{
    // set the new zone owner; change status and timer
    m_zoneOwner      = winner;
    m_status         = BF_STATUS_COOLDOWN;
    m_timer          = m_cooldownDuration;
    m_playersInvited = false;

    // no reward when there is no winner - battlefield initialization failed
    if (winner == TEAM_NONE)
    {
        sLog.outError("Battlefield: Battle ended with winner %u.", winner);
        return;
    }

    // reward players and reset
    RewardPlayersOnBattleEnd(winner);

    DEBUG_LOG("Battlefield: Battle has ended. Winner Team: %s.", winner == ALLIANCE ? "Alliance" : "Horde");

    // disband battlefield raid groups
    for (auto& m_battlefieldRaid : m_battlefieldRaids)
    {
        for (auto group : m_battlefieldRaid)
        {
            group->Disband();
            delete group;
        }
        m_battlefieldRaid.clear();
    }
}

/**
  Battlefield update function
*/
void Battlefield::Update(uint32 diff)
{
    // update battlefield timer to everyone
    SendBattlefieldTimerUpdate();

    // global battlefield timer
    if (m_timer < diff)
    {
        if (m_status == BF_STATUS_COOLDOWN)
            StartBattle(m_zoneOwner);
        else if (m_status == BF_STATUS_IN_PROGRESS)
            EndBattle(m_zoneOwner);
    }
    else
        m_timer -= diff;

    // refresh battlefield players
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
    {
        if (m_playersUpdateTimer < diff)
        {
            UpdateBattlefieldPlayers();
            m_playersUpdateTimer = 5000;
        }
        else
            m_playersUpdateTimer -= diff;
    }

    // invite all players in zone to queue
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && m_timer <= m_startInviteDelay && !m_playersInvited)
    {
        m_playersInvited = true;
        for (auto& m_zonePlayer : m_zonePlayers)
        {
            if (!m_zonePlayer.first)
                continue;

            Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
            if (!player)
                continue;

            InvitePlayerToQueue(player);
        }
    }

    // kick all the players marked to leave
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
    {
        for (auto& m_invitedPlayer : m_invitedPlayers)
        {
            for (std::map<ObjectGuid, time_t>::iterator itr = m_invitedPlayer.begin(); itr != m_invitedPlayer.end();)
            {
                if (itr->second < time(nullptr))
                {
                    if (Player* player = sObjectMgr.GetPlayer(itr->first))
                        player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);

                    m_invitedPlayer.erase(itr++);
                }
                else
                    ++itr;
            }
        }
    }

    // check the queued players
    if (m_queueUpdateTimer < diff)
    {
        m_queueUpdateTimer = 30000;

        for (auto& m_queuedPlayer : m_queuedPlayers)
        {
            for (std::set<ObjectGuid>::iterator itr = m_queuedPlayer.begin(); itr != m_queuedPlayer.end();)
            {
                if (!sObjectMgr.GetPlayer(*itr, true))
                    m_queuedPlayer.erase(itr++);
                else
                    ++itr;
            }
        }
    }
    else
        m_queueUpdateTimer -= diff;
}

/**
  Function that rewards the given team players

  @param   kill credit
  @param   team
  @param   source object
*/
void Battlefield::QuestCreditTeam(uint32 credit, Team team, WorldObject* source)
{
    for (auto& m_zonePlayer : m_zonePlayers)
    {
        if (!m_zonePlayer.first || !HasPlayer(m_zonePlayer.first))
            continue;

        Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
        if (!player)
            continue;

        // check if player is eligible for kill credit
        if (!source || !credit || player->GetTeam() != team || !player->IsAtGroupRewardDistance(source))
            continue;

        player->KilledMonsterCredit(credit);
    }
}

/*********************************************************/
/***                 BATTLEFIELD GROUPS                ***/
/*********************************************************/

/**
  Function that returns a non full group for a specified team index

  @param   team index
*/
Group* Battlefield::GetFreeRaid(PvpTeamIndex teamIdx)
{
    for (auto itr : m_battlefieldRaids[teamIdx])
    {
        if (!itr->IsFull())
            return itr;
    }

    return nullptr;
}

/**
  Function that returns the group of a specified player

  @param   player guid
*/
Group* Battlefield::GetGroupFor(ObjectGuid playerGuid)
{
    for (auto& m_battlefieldRaid : m_battlefieldRaids)
    {
        for (auto group : m_battlefieldRaid)
        {
            if (group->IsMember(playerGuid))
                return group;
        }
    }

    return nullptr;
}

/**
  Function that checks if the player can be added to a raid group

  @param   player to be added to raid
*/
bool Battlefield::CanAddPlayerToRaid(Player* player)
{
    if (!player->IsInWorld())
        return false;

    DEBUG_LOG("Battlefield: Adding player %s to raid", player->GetGuidStr().c_str());

    if (Group* group = player->GetGroup())
    {
        DEBUG_LOG("Battlefield: Player %s already has group %s, uninviting", player->GetGuidStr().c_str(), group->GetGuidStr().c_str());
        group->RemoveMember(player->GetObjectGuid(), 0);
    }

    PvpTeamIndex teamIdx = GetTeamIndexByTeamId(player->GetTeam());
    Group* group = GetFreeRaid(teamIdx);
    if (!group)
    {
        DEBUG_LOG("Battlefield: No free raid for %s!", player->GetGuidStr().c_str());
        if (IsTeamFull(teamIdx))
        {
            DEBUG_LOG("Battlefield: Battlefield is full! Can't add player %s!", player->GetGuidStr().c_str());
            return false;
        }

        DEBUG_LOG("Battlefield: Trying to create new group for %s!", player->GetGuidStr().c_str());
        group = new Group;
        group->SetBattlefieldGroup(this);
        if (group->Create(player->GetObjectGuid(), player->GetName()))
            DEBUG_LOG("Battlefield: Successfully created new group %s", group->GetGuidStr().c_str());
        else
            sLog.outError("Battlefield: Failed to create group for player %s.", player->GetGuidStr().c_str());

        m_battlefieldRaids[teamIdx].insert(group);
    }
    else if (group->IsMember(player->GetObjectGuid()))
    {
        DEBUG_LOG("Battlefield: Raid already has players %s, making some shit", player->GetGuidStr().c_str());
        uint8 subgroup = group->GetMemberGroup(player->GetObjectGuid());
        player->SetBattleRaid(group, subgroup);
    }
    else
    {
        if (IsTeamFull(teamIdx))
        {
            DEBUG_LOG("Battlefield: Group %s found, but battlefield is full! Can't add player %s!", group->GetGuidStr().c_str(), player->GetGuidStr().c_str());
            return false;
        }

        return group->AddMember(player->GetObjectGuid(), player->GetName());
    }

    return true;
}

/**
  Function that removes a player from a raid group

  @param   player guid to be removed
*/
void Battlefield::RemovePlayerFromRaid(ObjectGuid playerGuid)
{
    if (Group* group = GetGroupFor(playerGuid))
    {
        if (group->RemoveMember(playerGuid, 0) == 0)
            delete group;
    }
}

/**
  Function that checks if the battlefield has the player

  @param   player guid to be removed
*/
bool Battlefield::HasPlayer(ObjectGuid playerGuid)
{
    return GetGroupFor(playerGuid) != nullptr;
}

/**
  Function that returns the number of players depending on the team

  @param   team index
*/
uint32 Battlefield::GetPlayerCountByTeam(PvpTeamIndex teamIdx)
{
    uint32 count = 0;
    for (auto itr : m_battlefieldRaids[teamIdx])
        count += itr->GetMembersCount();

    return count;
}

/**
  Function that checks if a team is already full

  @param   team index
*/
bool Battlefield::IsTeamFull(PvpTeamIndex teamIdx)
{
    return m_queuedPlayers[teamIdx].size() + m_invitedPlayers[teamIdx].size() + GetPlayerCountByTeam(teamIdx) >= m_maxPlayersPerTeam;
}

/**
  Function that checks if a battlefield group can be deleted

  @param   group to be deleted
*/
bool Battlefield::CanDeleteBattlefieldGroup(Group* group)
{
    for (auto& m_battlefieldRaid : m_battlefieldRaids)
    {
        for (std::set<Group*>::iterator itr = m_battlefieldRaid.begin(); itr != m_battlefieldRaid.end(); ++itr)
        {
            if (*itr == group)
            {
                m_battlefieldRaid.erase(itr);
                return true;
            }
        }
    }

    return false;
}

/**
  Function that handles the disband of the group

  @param   player that is affected by the group disband
*/
void Battlefield::HandleBattlefieldGroupDisband(Player* player)
{
    player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);

    if (m_activePlayers.find(player->GetObjectGuid()) != m_activePlayers.end())
    {
        m_activePlayers[player->GetObjectGuid()]->removeTime = time(nullptr);
        m_activePlayers[player->GetObjectGuid()]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
    }

    // notify battlefield
    UpdatePlayerGroupDisband(player);
}

/*********************************************************/
/***           BATTLEFIELD SESSION FUNCTIONS           ***/
/*********************************************************/

/**
  Function that handles player invite to queue

  @param   player to be invited
*/
void Battlefield::InvitePlayerToQueue(Player* player)
{
    if (HasPlayer(player->GetObjectGuid()))
        return;

    PvpTeamIndex teamIdx = GetTeamIndexByTeamId(player->GetTeam());

    // he is in queue or waiting to accept teleport button
    if (m_queuedPlayers[teamIdx].find(player->GetObjectGuid()) != m_queuedPlayers[teamIdx].end())
        return;
    if (m_invitedPlayers[teamIdx].find(player->GetObjectGuid()) != m_invitedPlayers[teamIdx].end())
        return;

    if (!IsTeamFull(teamIdx))
        player->GetSession()->SendBattlefieldQueueInvite(m_battleFieldId);
    else
        player->GetSession()->SendBattlefieldQueueResponse(m_battleFieldId, m_zoneId, true, true);
}

/**
  Function that handles player invite response

  @param   player that responded
  @param   whether the player has accepted or not
*/
void Battlefield::HandleQueueInviteResponse(Player* player, bool accepted)
{
    if (!accepted || HasPlayer(player->GetObjectGuid()))
        return;

    PvpTeamIndex teamIdx = GetTeamIndexByTeamId(player->GetTeam());

    // reject if queue battlefield is full
    if (IsTeamFull(teamIdx))
    {
        player->GetSession()->SendBattlefieldQueueResponse(m_battleFieldId, m_zoneId, true, true);
        return;
    }

    if (m_queuedPlayers[teamIdx].find(player->GetObjectGuid()) == m_queuedPlayers[teamIdx].end())
    {
        if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
        {
            m_invitedPlayers[teamIdx][player->GetObjectGuid()] = time(nullptr) + BF_TIME_TO_ACCEPT;
            player->GetSession()->SendBattlefieldWarInvite(m_battleFieldId, m_zoneId, BF_TIME_TO_ACCEPT);
        }
        else
        {
            m_queuedPlayers[teamIdx].insert(player->GetObjectGuid());
            player->GetSession()->SendBattlefieldQueueResponse(m_battleFieldId, m_zoneId, true, false);
        }
    }
}

/**
  Function that handles player teleport response

  @param   player that responded
  @param   whether the player has accepted or not
*/
void Battlefield::HandleWarInviteResponse(Player* player, bool accepted)
{
    if (HasPlayer(player->GetObjectGuid()))
        return;

    if (accepted)
    {
        PvpTeamIndex teamIdx = GetTeamIndexByTeamId(player->GetTeam());
        std::map<ObjectGuid, time_t>::iterator itr = m_invitedPlayers[teamIdx].find(player->GetObjectGuid());

        // he was not invited by core
        if (itr == m_invitedPlayers[teamIdx].end())
            return;

        m_invitedPlayers[teamIdx].erase(itr);

        if (CanAddPlayerToRaid(player))
        {
            DEBUG_LOG("Battlefield: CanAddPlayerToRaid for %s returned: TRUE", player->GetGuidStr().c_str());
            player->GetSession()->SendBattlefieldEnteredResponse(m_battleFieldId);
        }
        else
            DEBUG_LOG("Battlefield: CanAddPlayerToRaid for %s returned: FALSE", player->GetGuidStr().c_str());

        // ToDo: recheck this!
        if (player->GetZoneId() == m_zoneId)
        {
            if (m_activePlayers.find(player->GetObjectGuid()) == m_activePlayers.end())  // must never happen
                InitPlayerBattlefieldData(player);
        }
    }
    else
    {
        // kick players which refuse the war invite if battlefield is in progress
        if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS && player->GetZoneId() == m_zoneId)
        {
            player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
            if (m_activePlayers.find(player->GetObjectGuid()) != m_activePlayers.end())
            {
                m_activePlayers[player->GetObjectGuid()]->removeTime = time(nullptr);
                m_activePlayers[player->GetObjectGuid()]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
            }
        }
    }

    // notify battlefield
    UpdatePlayerBattleResponse(player);
}

/**
  Function that handles player queue exit request

  @param   player to exit queue
*/
void Battlefield::HandleExitRequest(Player* player)
{
    // if already active in battlefield
    if (HasPlayer(player->GetObjectGuid()))
    {
        player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);

        RemovePlayerFromRaid(player->GetObjectGuid());

        if (m_activePlayers.find(player->GetObjectGuid()) != m_activePlayers.end())
        {
            m_activePlayers[player->GetObjectGuid()]->removeTime = time(nullptr);
            m_activePlayers[player->GetObjectGuid()]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
        }
    }
    // else remove only from queue
    else
    {
        std::set<ObjectGuid>::iterator itr = m_queuedPlayers[GetTeamIndexByTeamId(player->GetTeam())].find(player->GetObjectGuid());
        if (itr != m_queuedPlayers[GetTeamIndexByTeamId(player->GetTeam())].end())
        {
            m_queuedPlayers[GetTeamIndexByTeamId(player->GetTeam())].erase(itr);

            player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);

            if (m_activePlayers.find(player->GetObjectGuid()) != m_activePlayers.end())
            {
                m_activePlayers[player->GetObjectGuid()]->removeTime = time(nullptr);
                m_activePlayers[player->GetObjectGuid()]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
            }
        }
    }

    // notify battlefield
    UpdatePlayerExitRequest(player);
}

/**
  Function that sends a zone wide warning to all players

  @param   source object
  @param   text entry
  @param   sound id
*/
void Battlefield::SendZoneWarning(WorldObject* source, int32 textEntry, uint32 soundId)
{
    for (auto& m_zonePlayer : m_zonePlayers)
    {
        if (!m_zonePlayer.first)
            continue;

        Player* player = sObjectMgr.GetPlayer(m_zonePlayer.first);
        if (!player)
            continue;

        if (player->GetZoneId() != m_zoneId)
            continue;

        int32 locIdx = player->GetSession()->GetSessionDbLocaleIndex();

        char const* text = sObjectMgr.GetMangosString(textEntry, locIdx);

        source->MonsterTextEmote(text, player, true);

        // play sound id, if provided
        if (soundId)
            source->PlayDirectSound(soundId, PlayPacketParameters(PLAY_TARGET, (Player const*)player));
    }
}
