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

Battlefield::Battlefield() : OutdoorPvP(), m_battleFieldId(0)
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

    // invite to group or que
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
    BattlefieldPlayerMap::iterator itr = m_activePlayers.find(player->GetObjectGuid());
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

    BattlefieldPlayerMap::iterator itr = m_activePlayers.find(player->GetObjectGuid());
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
Function that kicks the player from the battlefield

@param   player to kick
*/
void Battlefield::KickBattlefieldPlayer(Player* player)
{
    float x, y, z;
    if (!GetPlayerKickLocation(player, x, y, z))
        return;

    player->TeleportTo(player->GetMapId(), x, y, z, player->GetOrientation());
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
    for (BattlefieldPlayerMap::iterator itr = m_activePlayers.begin(); itr != m_activePlayers.end(); ++itr)
        delete itr->second;

    m_activePlayers.clear();

    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first)
            continue;

        Player* player = sObjectMgr.GetPlayer(itr->first);
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
    for (BattlefieldPlayerMap::iterator itr = m_activePlayers.begin(); itr != m_activePlayers.end();)
    {
        if (itr->second->removeTime && itr->second->removeTime + itr->second->removeDelay < time(nullptr))
        {
            delete itr->second;
            if (Player* player = sObjectMgr.GetPlayer(itr->first))
                KickBattlefieldPlayer(player);

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
    m_startTime = time(nullptr);
    m_status = BF_STATUS_IN_PROGRESS;
    m_timer = m_battleDuration;
    m_zoneOwner = defender;

    // refresh all players before battle
    InitPlayersBeforeBattle();

    DEBUG_LOG("Disbanding groups");
    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        while (!m_battlefieldRaids[i].empty())
        {
            Group* group = *m_battlefieldRaids[i].begin();
            group->Disband();
            delete group;
        }
    }

    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
            continue;

        uint8 idx = 0;
        std::set<ObjectGuid>::iterator itr2 = m_queuedPlayers[idx].find(itr->first);
        if (itr2 == m_queuedPlayers[idx].end())
        {
            ++idx;
            itr2 = m_queuedPlayers[idx].find(itr->first);
        }

        if (itr2 != m_queuedPlayers[idx].end())
        {
            m_invitedPlayers[idx][itr->first] = time(nullptr) + BF_TIME_TO_ACCEPT;
            plr->GetSession()->SendBattlefieldWarInvite(m_battleFieldId, m_zoneId, BF_TIME_TO_ACCEPT);
            m_queuedPlayers[idx].erase(itr2);
        }
        else
        {
            plr->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
            SendRemoveWorldStates(plr);
            if (m_activePlayers.find(itr->first) != m_activePlayers.end())
            {
                m_activePlayers[itr->first]->removeTime = time(nullptr);
                m_activePlayers[itr->first]->removeDelay = BF_UNACCEPTED_REMOVE_DELAY;
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
*/
void Battlefield::EndBattle(Team winner, bool byTimer)
{
    m_zoneOwner = winner;
    m_status = BF_STATUS_COOLDOWN;
    m_timer = m_cooldownDuration;
    m_playersInvited = false;

    RewardPlayersOnBattleEnd(winner);
    Reset();
}

void Battlefield::Update(uint32 diff)
{
    if (m_timer < diff)
    {
        if (m_status == BF_STATUS_COOLDOWN)
            StartBattle(m_zoneOwner);
        else if (m_status == BF_STATUS_IN_PROGRESS)
            EndBattle(m_zoneOwner, true);
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

    // invite players to queue
    if (GetBattlefieldStatus() == BF_STATUS_COOLDOWN && m_timer <= m_startInviteDelay && !m_playersInvited)
    {
        m_playersInvited = true;
        for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
        {
            if (!itr->first)
                continue;

            Player* player = sObjectMgr.GetPlayer(itr->first);
            if (!player)
                continue;

            InvitePlayerToQueue(player);
        }
    }

    // kick all the players marked to leave
    if (GetBattlefieldStatus() == BF_STATUS_IN_PROGRESS)
    {
        for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        {
            for (std::map<ObjectGuid, time_t>::iterator itr = m_invitedPlayers[i].begin(); itr != m_invitedPlayers[i].end();)
            {
                if (itr->second < time(nullptr))
                {
                    if (Player* player = sObjectMgr.GetPlayer(itr->first))
                        player->GetSession()->SendBattlefieldLeaveMessage(m_battleFieldId, BATTLEFIELD_LEAVE_REASON_EXITED);

                    m_invitedPlayers[i].erase(itr++);
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

        for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        {
            for (std::set<ObjectGuid>::iterator itr = m_queuedPlayers[i].begin(); itr != m_queuedPlayers[i].end();)
            {
                if (!sObjectMgr.GetPlayer(*itr, true))
                    m_queuedPlayers[i].erase(itr++);
                else
                    ++itr;
            }
        }
    }
    else
        m_queueUpdateTimer -= diff;
}

// ToDo: requires review
void Battlefield::SetupPlayerPositions()
{
    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first || !HasPlayer(itr->first))
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
            continue;

        SetupPlayerPosition(plr);
    }
}

// ToDo: requires review
void Battlefield::QuestCreditTeam(uint32 credit, Team team, WorldObject* source, float radius)
{
    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first || !HasPlayer(itr->first))
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
            continue;

        if ((plr->GetTeam() != team || source) && radius > 0.0f && source->GetDistance2d(plr->GetPositionX(), plr->GetPositionY()) > radius)
            continue;

        plr->KilledMonsterCredit(credit);
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
    for (std::set<Group*>::iterator itr = m_battlefieldRaids[teamIdx].begin(); itr != m_battlefieldRaids[teamIdx].end(); ++itr)
    {
        if (!(*itr)->IsFull())
            return *itr;
    }

    return NULL;
}

/**
Function that returns the group of a specified player

@param   player guid
*/
Group* Battlefield::GetGroupFor(ObjectGuid playerGuid)
{
    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (std::set<Group*>::iterator itr = m_battlefieldRaids[i].begin(); itr != m_battlefieldRaids[i].end(); ++itr)
        {
            if ((*itr)->IsMember(playerGuid))
                return *itr;
        }
    }

    return NULL;
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
        DEBUG_LOG("Battlefield: Player %s already has group %s, uninviting", player->GetGuidStr().c_str(), group->GetObjectGuid().GetString().c_str());
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
            DEBUG_LOG("Battlefield: Successfully created new group %s", group->GetObjectGuid().GetString().c_str());
        else
            DEBUG_LOG("Failed to create group!");

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
            DEBUG_LOG("Battlefield: Group %s found, but battlefield is full! Can't add player %s!", group->GetObjectGuid().GetString().c_str(), player->GetGuidStr().c_str());
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
    return GetGroupFor(playerGuid) != NULL;
}

/**
Function that returns the number of players depending on the team

@param   team index
*/
uint32 Battlefield::GetPlayerCountByTeam(PvpTeamIndex teamIdx)
{
    uint32 count = 0;
    for (std::set<Group*>::iterator itr = m_battlefieldRaids[teamIdx].begin(); itr != m_battlefieldRaids[teamIdx].end(); ++itr)
        count += (*itr)->GetMembersCount();

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
    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (std::set<Group*>::iterator itr = m_battlefieldRaids[i].begin(); itr != m_battlefieldRaids[i].end(); ++itr)
        {
            if (*itr == group)
            {
                m_battlefieldRaids[i].erase(itr);
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

    // ToDo: not needed
    //std::map<ObjectGuid, time_t>::iterator itr = m_invitedPlayers[teamIdx].find(player->GetObjectGuid());
    //if (itr != m_invitedPlayers[teamIdx].end())
    //    m_invitedPlayers[teamIdx].erase(itr);

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
@param   wether the player has accepted or not
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

        // control the rest in battlefield script
        UpdatePlayerOnWarResponse(player);
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
}

/**
Function that sends a zone wide warning to all players

@param   text id
@param   source object. Can be null
*/
void Battlefield::SendZoneWarning(int32 entry, WorldObject* source)
{
    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first)
            continue;

        Player* player = sObjectMgr.GetPlayer(itr->first);
        if (!player)
            continue;

        if (player->GetZoneId() != m_zoneId)
            continue;

        int32 locIdx = player->GetSession()->GetSessionDbLocaleIndex();

        char const* text = sObjectMgr.GetMangosString(entry, locIdx);

        WorldPacket data;
        ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, text, LANG_UNIVERSAL, CHAT_TAG_NONE,
            source ? source->GetObjectGuid() : ObjectGuid(), source ? source->GetName() : "", player->GetObjectGuid(), player->GetName());

        player->GetSession()->SendPacket(data);
    }
}