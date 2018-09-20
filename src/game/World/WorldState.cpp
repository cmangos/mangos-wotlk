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
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include "WorldState.h"
#include <algorithm>
#include <map>

enum
{
    GROMGOLUC_EVENT_1   = 15312,
    GROMGOLUC_EVENT_2   = 15313,
    GROMGOLUC_EVENT_3   = 15314,
    GROMGOLUC_EVENT_4   = 15315,

    OGUC_EVENT_1        = 15318,
    OGUC_EVENT_2        = 15319,
    OGUC_EVENT_3        = 15320,
    OGUC_EVENT_4        = 15321,

    GROMGOLOG_EVENT_1   = 15322,
    GROMGOLOG_EVENT_2   = 15323,
    GROMGOLOG_EVENT_3   = 15324,
    GROMGOLOG_EVENT_4   = 15325,
};

WorldState::WorldState() : m_isMagtheridonHeadSpawnedHorde(false), m_isMagtheridonHeadSpawnedAlliance(false), m_adalSongOfBattleTimer(0)
{
    m_transportStates[GROMGOL_UNDERCITY]    = GROMGOLUC_EVENT_1;
    m_transportStates[GROMGOL_ORGRIMMAR]    = OGUC_EVENT_1;
    m_transportStates[ORGRIMMAR_UNDERCITY]  = GROMGOLOG_EVENT_1;
}


WorldState::~WorldState()
{
}

void WorldState::HandleGameObjectUse(GameObject* go, Unit* user)
{
    switch (go->GetEntry())
    {
        case OBJECT_MAGTHERIDONS_HEAD:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (Player* player = dynamic_cast<Player*>(user))
            {
                if (player->GetTeam() == HORDE)
                {
                    m_isMagtheridonHeadSpawnedHorde = true;
                    m_guidMagtheridonHeadHorde = go->GetObjectGuid();
                    BuffMagtheridonTeam(HORDE);
                }
                else
                {
                    m_isMagtheridonHeadSpawnedAlliance = true;
                    m_guidMagtheridonHeadAlliance = go->GetObjectGuid();
                    BuffMagtheridonTeam(ALLIANCE);
                }
            }
            break;
        }
        default:
            break;
    }
}

void WorldState::HandleGameObjectRevertState(GameObject* go)
{
    switch (go->GetEntry())
    {
        case OBJECT_MAGTHERIDONS_HEAD:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (go->GetObjectGuid() == m_guidMagtheridonHeadHorde)
            {
                m_isMagtheridonHeadSpawnedHorde = false;
                m_guidMagtheridonHeadHorde = ObjectGuid();
                DispelMagtheridonTeam(HORDE);
            }
            else if (go->GetObjectGuid() == m_guidMagtheridonHeadAlliance)
            {
                m_isMagtheridonHeadSpawnedAlliance = false;
                m_guidMagtheridonHeadAlliance = ObjectGuid();
                DispelMagtheridonTeam(ALLIANCE);
            }
            break;
        }
        default:
            break;
    }
}

void WorldState::HandlePlayerEnterZone(Player* player, uint32 zoneId)
{
    switch (zoneId)
    {
        case ZONEID_HELLFIRE_PENINSULA:
        case ZONEID_HELLFIRE_RAMPARTS:
        case ZONEID_HELLFIRE_CITADEL:
        case ZONEID_BLOOD_FURNACE:
        case ZONEID_SHATTERED_HALLS:
        case ZONEID_MAGTHERIDON_LAIR:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (m_isMagtheridonHeadSpawnedAlliance && player->GetTeam() == ALLIANCE)
                player->CastSpell(player, SPELL_TROLLBANES_COMMAND, TRIGGERED_OLD_TRIGGERED);
            if (m_isMagtheridonHeadSpawnedHorde && player->GetTeam() == HORDE)
                player->CastSpell(player, SPELL_NAZGRELS_FAVOR, TRIGGERED_OLD_TRIGGERED);
            m_magtheridonHeadPlayers.push_back(player->GetObjectGuid());
        }
        case ZONEID_SHATTRATH:
        case ZONEID_BOTANICA:
        case ZONEID_MECHANAR:
        case ZONEID_ARCATRAZ:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (m_adalSongOfBattleTimer)
                player->CastSpell(player, SPELL_ADAL_SONG_OF_BATTLE, TRIGGERED_OLD_TRIGGERED);
            m_adalSongOfBattlePlayers.push_back(player->GetObjectGuid());
        }
        default:
            break;
    }
}

void WorldState::HandlePlayerLeaveZone(Player* player, uint32 zoneId)
{
    switch (zoneId)
    {
        case ZONEID_HELLFIRE_PENINSULA:
        case ZONEID_HELLFIRE_RAMPARTS:
        case ZONEID_HELLFIRE_CITADEL:
        case ZONEID_BLOOD_FURNACE:
        case ZONEID_SHATTERED_HALLS:
        case ZONEID_MAGTHERIDON_LAIR:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (player->GetTeam() == ALLIANCE)
                player->RemoveAurasDueToSpell(SPELL_TROLLBANES_COMMAND);
            if (player->GetTeam() == HORDE)
                player->RemoveAurasDueToSpell(SPELL_NAZGRELS_FAVOR);
            auto position = std::find(m_magtheridonHeadPlayers.begin(), m_magtheridonHeadPlayers.end(), player->GetObjectGuid());
            if (position != m_magtheridonHeadPlayers.end()) // == myVector.end() means the element was not found
                m_magtheridonHeadPlayers.erase(position);
        }
        case ZONEID_SHATTRATH:
        case ZONEID_BOTANICA:
        case ZONEID_MECHANAR:
        case ZONEID_ARCATRAZ:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            player->RemoveAurasDueToSpell(SPELL_ADAL_SONG_OF_BATTLE);
            auto position = std::find(m_adalSongOfBattlePlayers.begin(), m_adalSongOfBattlePlayers.end(), player->GetObjectGuid());
            if (position != m_adalSongOfBattlePlayers.end()) // == myVector.end() means the element was not found
                m_adalSongOfBattlePlayers.erase(position);
        }
        default:
            break;
    }
}

void WorldState::HandlePlayerEnterArea(Player* player, uint32 areaId)
{
    switch (areaId)
    {
        case AREAID_SKYGUARD_OUTPOST:
        case AREAID_SHARTUUL_TRANSPORTER:
        case AREAID_DEATHS_DOOR:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_areaPlayers[areaId].push_back(player->GetObjectGuid());
            break;
        }
        default: break;
    }
}

void WorldState::HandlePlayerLeaveArea(Player* player, uint32 areaId)
{
    switch (areaId)
    {
        case AREAID_SKYGUARD_OUTPOST:
        case AREAID_SHARTUUL_TRANSPORTER:
        case AREAID_DEATHS_DOOR:
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            auto position = std::find(m_areaPlayers[areaId].begin(), m_areaPlayers[areaId].end(), player->GetObjectGuid());
            if (position != m_areaPlayers[areaId].end()) // == myVector.end() means the element was not found
                m_areaPlayers[areaId].erase(position);
            break;
        }
        default: break;
    }
}

bool WorldState::IsConditionFulfilled(uint32 conditionId, uint32 state) const
{
    return m_transportStates.at(conditionId) == state;
}

void WorldState::HandleConditionStateChange(uint32 conditionId, uint32 state)
{
    m_transportStates[conditionId] = state;
}

void WorldState::BuffMagtheridonTeam(Team team)
{
    for (ObjectGuid& guid : m_magtheridonHeadPlayers)
    {
        if (Player* player = sObjectMgr.GetPlayer(guid))
        {
            if (team == ALLIANCE && player->GetTeam() == ALLIANCE)
            {
                player->GetMap()->AddMessage([guid](Map* map) -> void
                {
                    if (Player* player = map->GetPlayer(guid))
                        player->CastSpell(player, SPELL_TROLLBANES_COMMAND, TRIGGERED_OLD_TRIGGERED);
                });
            }
            if (team == HORDE && player->GetTeam() == HORDE)
            {
                player->GetMap()->AddMessage([guid](Map* map) -> void
                {
                    if (Player* player = map->GetPlayer(guid))
                        player->CastSpell(player, SPELL_NAZGRELS_FAVOR, TRIGGERED_OLD_TRIGGERED);
                });
            }
        }
    }
}

void WorldState::DispelMagtheridonTeam(Team team)
{
    for (ObjectGuid& guid : m_magtheridonHeadPlayers)
    {
        if (Player* player = sObjectMgr.GetPlayer(guid))
        {
            if (team == ALLIANCE && player->GetTeam() == ALLIANCE)
            {
                player->GetMap()->AddMessage([guid](Map* map) -> void
                {
                    if (Player* player = map->GetPlayer(guid))
                        player->RemoveAurasDueToSpell(SPELL_TROLLBANES_COMMAND);
                });
            }
            if (team == HORDE && player->GetTeam() == HORDE)
            {
                player->GetMap()->AddMessage([guid](Map* map) -> void
                {
                    if (Player* player = map->GetPlayer(guid))
                        player->RemoveAurasDueToSpell(SPELL_NAZGRELS_FAVOR);
                });
            }
        }
    }
}

void WorldState::HandleExternalEvent(uint32 eventId)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    if (eventId == CUSTOM_EVENT_ADALS_SONG_OF_BATTLE)
    {
        m_adalSongOfBattleTimer = 120 * MINUTE * IN_MILLISECONDS; // Two hours duration
        BuffAdalsSongOfBattle();
    }
}

void WorldState::Update(const uint32 diff)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    if (m_adalSongOfBattleTimer)
    {
        if (m_adalSongOfBattleTimer <= diff)
        {
            m_adalSongOfBattleTimer = 0;
            DispelAdalsSongOfBattle();
        }
        else m_adalSongOfBattleTimer -= diff;
    }
}

void WorldState::BuffAdalsSongOfBattle()
{
    for (ObjectGuid& guid : m_adalSongOfBattlePlayers)
    {
        if (Player* player = sObjectMgr.GetPlayer(guid))
        {
            player->GetMap()->AddMessage([guid](Map* map) -> void
            {
                if (Player* player = map->GetPlayer(guid))
                    player->CastSpell(player, SPELL_ADAL_SONG_OF_BATTLE, TRIGGERED_OLD_TRIGGERED);
            });
        }
    }
}

void WorldState::DispelAdalsSongOfBattle()
{
    for (ObjectGuid& guid : m_adalSongOfBattlePlayers)
    {
        if (Player* player = sObjectMgr.GetPlayer(guid))
        {
            player->GetMap()->AddMessage([guid](Map* map) -> void
            {
                if (Player* player = map->GetPlayer(guid))
                    player->RemoveAurasDueToSpell(SPELL_ADAL_SONG_OF_BATTLE);
            });
        }
    }
}

void WorldState::ExecuteOnAreaPlayers(uint32 areaId, std::function<void(Player*)> executor)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    for (ObjectGuid guid : m_areaPlayers[areaId])
        if (Player* player = sObjectMgr.GetPlayer(guid))
            executor(player);
}
