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

#include "Common.h"
#include "Log.h"
#include "Entities/Player.h"
#include "Server/WorldPacket.h"
#include "Server/WorldSession.h"
#include "Server/Opcodes.h"
#include "Maps/MapPersistentStateMgr.h"
#include "Calendar/Calendar.h"
#include "Globals/ObjectMgr.h"
#include "Social/SocialMgr.h"
#include "World/World.h"
#include "Guilds/Guild.h"
#include "Guilds/GuildMgr.h"
#include "Arena/ArenaTeam.h"

void WorldSession::HandleCalendarGetCalendar(WorldPacket& /*recv_data*/)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_GET_CALENDAR [%s]", guid.GetString().c_str());

    time_t currTime = time(nullptr);

    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR);

    CalendarInvitesList invites;
    sCalendarMgr.GetPlayerInvitesList(guid, invites);

    data << uint32(invites.size());
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Sending > %u invites", uint32(invites.size()));

    for (CalendarInvitesList::const_iterator itr = invites.begin(); itr != invites.end(); ++itr)
    {
        CalendarEvent const* event = (*itr)->GetCalendarEvent();
        MANGOS_ASSERT(event);                           // TODO: be sure no way to have a null event

        data << uint64(event->EventId);
        data << uint64((*itr)->InviteId);
        data << uint8((*itr)->Status);
        data << uint8((*itr)->Rank);

        data << uint8(event->IsGuildEvent());
        data << event->CreatorGuid.WriteAsPacked();
        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "invite> EventId[" UI64FMTD "], InviteId[" UI64FMTD "], status[%u], rank[%u]",
                         event->EventId, (*itr)->InviteId, uint32((*itr)->Status), uint32((*itr)->Rank));
    }

    CalendarEventsList events;
    sCalendarMgr.GetPlayerEventsList(guid, events);

    data << uint32(events.size());
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Sending > %u events", uint32(events.size()));

    for (CalendarEventsList::const_iterator itr = events.begin(); itr != events.end(); ++itr)
    {
        CalendarEvent const* event = *itr;

        data << uint64(event->EventId);
        data << event->Title;
        data << uint32(event->Type);
        data << secsToTimeBitFields(event->EventTime);
        data << uint32(event->Flags);
        data << int32(event->DungeonId);
        data << event->CreatorGuid.WriteAsPacked();

        std::string timeStr = TimeToTimestampStr(event->EventTime);
        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Events> EventId[" UI64FMTD "], Title[%s], Time[%s], Type[%u],  Flag[%u], DungeonId[%d], CreatorGuid[%s]",
                         event->EventId, event->Title.c_str(), timeStr.c_str(), uint32(event->Type),
                         uint32(event->Flags), event->DungeonId, event->CreatorGuid.GetString().c_str());
    }

    data << uint32(currTime);                               // server time
    data << secsToTimeBitFields(currTime);                  // zone time ??

    ByteBuffer dataBuffer;
    uint32 boundCounter = 0;
    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
    {
        Player::BoundInstancesMap boundInstances = _player->GetBoundInstances(Difficulty(i));
        for (Player::BoundInstancesMap::const_iterator itr = boundInstances.begin(); itr != boundInstances.end(); ++itr)
        {
            if (itr->second.perm)
            {
                DungeonPersistentState const* state = itr->second.state;
                dataBuffer << uint32(state->GetMapId());
                dataBuffer << uint32(state->GetDifficulty());
                dataBuffer << uint32(state->GetResetTime() - currTime);
                dataBuffer << uint64(state->GetInstanceId());   // instance save id as unique instance copy id
                ++boundCounter;
            }
        }
    }

    data << uint32(boundCounter);
    data.append(dataBuffer);

    data << uint32(1135753200);                             // Constant date, unk (28.12.2005 07:00)

    // Reuse variables
    boundCounter = 0;
    std::set<uint32> sentMaps;
    dataBuffer.clear();

    for (MapDifficultyMap::const_iterator itr = sMapDifficultyMap.begin(); itr != sMapDifficultyMap.end(); ++itr)
    {
        uint32 map_diff_pair = itr->first;
        uint32 mapId = PAIR32_LOPART(map_diff_pair);
        MapDifficultyEntry const* mapDiff = itr->second;

        // skip mapDiff without global reset time
        if (!mapDiff->resetTime)
            continue;

        // skip non raid map
        MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
        if (!mapEntry || !mapEntry->IsRaid())
            continue;

        // skip already sent map (not same difficulty?)
        if (sentMaps.find(mapId) != sentMaps.end())
            continue;

        uint32 resetTime = sMapPersistentStateMgr.GetScheduler().GetMaxResetTimeFor(mapDiff);

        sentMaps.insert(mapId);
        dataBuffer << mapId;
        dataBuffer << resetTime;

        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "MapId [%u] -> Reset Time: %u", mapId, resetTime);
        dataBuffer << int32(0); // showed 68400 on map 509 must investigate more
        ++boundCounter;
    }
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Map sent [%u]", boundCounter);

    data << uint32(boundCounter);
    data.append(dataBuffer);

    uint32 holidayCount = 0;
    for (uint32 i = 0; i < sHolidaysStore.GetNumRows(); ++i)
        if (HolidaysEntry const* holiday = sHolidaysStore.LookupEntry(i))
            if (!holiday->Looping)
                ++holidayCount;

    data << uint32(holidayCount);
    for (uint32 i = 0; i < sHolidaysStore.GetNumRows(); ++i)
    {
        HolidaysEntry const* holiday = sHolidaysStore.LookupEntry(i);

        if (!holiday || holiday->Looping)
            continue;

        data << uint32(holiday->Id);                        // m_ID
        data << uint32(holiday->Region);                    // m_region, might be looping
        data << uint32(holiday->Looping);                   // m_looping, might be region
        data << uint32(holiday->Priority);                  // m_priority
        data << uint32(holiday->CalendarFilterType);        // m_calendarFilterType

        for (uint8 j = 0; j < MAX_HOLIDAY_DATES; ++j)
            data << uint32(holiday->Date[j]);               // 26 * m_date -- WritePackedTime ?

        for (uint8 j = 0; j < MAX_HOLIDAY_DURATIONS; ++j)
            data << uint32(holiday->Duration[j]);           // 10 * m_duration

        for (uint8 j = 0; j < MAX_HOLIDAY_FLAGS; ++j)
            data << uint32(holiday->CalendarFlags[j]);      // 10 * m_calendarFlags

        data << holiday->TextureFilename;                   // m_textureFilename (holiday name)
    }

    SendPacket(data);
}

void WorldSession::HandleCalendarGetEvent(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_GET_EVENT [%s]", guid.GetString().c_str());

    uint64 eventId;
    recv_data >> eventId;

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
        sCalendarMgr.SendCalendarEvent(_player, event, CALENDAR_SENDTYPE_GET);
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarGuildFilter(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_GUILD_FILTER [%s]", _player->GetGuidStr().c_str());

    uint32 minLevel;
    uint32 maxLevel;
    uint32 minRank;

    recv_data >> minLevel >> maxLevel >> minRank;

    if (Guild* guild = sGuildMgr.GetGuildById(_player->GetGuildId()))
        guild->MassInviteToEvent(this, minLevel, maxLevel, minRank);

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Min level [%u], Max level [%u], Min rank [%u]", minLevel, maxLevel, minRank);
}

void WorldSession::HandleCalendarEventSignup(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_SIGNUP [%s]", guid.GetString().c_str());

    uint64 eventId;
    bool tentative;

    recv_data >> eventId;
    recv_data >> tentative; // uint8 == bool size in all compilator???
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "] Tentative %u", eventId, uint32(tentative));

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
    {
        if (event->IsGuildEvent() && event->GuildId != _player->GetGuildId())
        {
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_GUILD_PLAYER_NOT_IN_GUILD);
            return;
        }

        CalendarInviteStatus status = tentative ? CALENDAR_STATUS_TENTATIVE : CALENDAR_STATUS_SIGNED_UP;
        sCalendarMgr.AddInvite(event, guid, guid, CalendarInviteStatus(status), CALENDAR_RANK_PLAYER, "", time(nullptr));
        sCalendarMgr.SendCalendarClearPendingAction(_player);
    }
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarArenaTeam(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_ARENA_TEAM [%s]", _player->GetGuidStr().c_str());
    uint32 areanTeamId;
    recv_data >> areanTeamId;

    if (ArenaTeam* team = sObjectMgr.GetArenaTeamById(areanTeamId))
        team->MassInviteToEvent(this);
}

void WorldSession::HandleCalendarAddEvent(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_ADD_EVENT [%s]", guid.GetString().c_str());

    std::string title;
    std::string description;
    uint8 type;
    uint8 repeatable;
    uint32 maxInvites;
    int32 dungeonId;
    uint32 eventPackedTime;
    uint32 unkPackedTime;
    uint32 flags;

    recv_data >> title;
    recv_data >> description;
    recv_data >> type;
    recv_data >> repeatable;
    recv_data >> maxInvites;
    recv_data >> dungeonId;
    recv_data >> eventPackedTime;
    recv_data >> unkPackedTime;
    recv_data >> flags;

    // 946684800 is 01/01/2000 00:00:00 - default response time
    CalendarEvent* cal =  sCalendarMgr.AddEvent(_player->GetObjectGuid(), title, description, type, repeatable, maxInvites, dungeonId, timeBitFieldsToSecs(eventPackedTime), timeBitFieldsToSecs(unkPackedTime), flags);

    if (cal)
    {
        if (cal->IsGuildAnnouncement())
        {
            sCalendarMgr.AddInvite(cal, guid, ObjectGuid(uint64(0)),  CALENDAR_STATUS_NOT_SIGNED_UP, CALENDAR_RANK_PLAYER, "", time(nullptr));
        }
        else
        {
            uint32 inviteCount;
            recv_data >> inviteCount;

            for (uint32 i = 0; i < inviteCount; ++i)
            {
                ObjectGuid invitee;
                uint8 status = 0;
                uint8 rank = 0;
                recv_data >> invitee.ReadAsPacked();
                recv_data >> status;
                recv_data >> rank;

                sCalendarMgr.AddInvite(cal, guid, invitee, CalendarInviteStatus(status), CalendarModerationRank(rank), "", time(nullptr));
            }
        }
        sCalendarMgr.SendCalendarEvent(_player, cal, CALENDAR_SENDTYPE_ADD);
    }
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_UPDATE_EVENT [%s]", guid.GetString().c_str());

    uint64 eventId;
    uint64 inviteId;
    std::string title;
    std::string description;
    uint8 type;
    uint8 repetitionType;
    uint32 maxInvites;
    int32 dungeonId;
    uint32 eventPackedTime;
    uint32 UnknownPackedTime;
    uint32 flags;

    recv_data >> eventId >> inviteId >> title >> description >> type >> repetitionType >> maxInvites >> dungeonId;
    recv_data >> eventPackedTime;
    recv_data >> UnknownPackedTime;
    recv_data >> flags;

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "], InviteId [" UI64FMTD "] Title %s, Description %s, type %u "
                     "Repeatable %u, MaxInvites %u, Dungeon ID %d, Flags %u", eventId, inviteId, title.c_str(),
                     description.c_str(), uint32(type), uint32(repetitionType), maxInvites, dungeonId, flags);

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
    {
        if (guid != event->CreatorGuid)
        {
            CalendarInvite* updaterInvite = event->GetInviteByGuid(guid);
            if (updaterInvite == nullptr)
            {
                sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NOT_INVITED);
                return ;
            }

            if (updaterInvite->Rank != CALENDAR_RANK_MODERATOR)
            {
                // remover have not enough right to change invite status
                sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PERMISSIONS);
                return;
            }
        }

        time_t oldEventTime = event->EventTime;

        event->Type = CalendarEventType(type);
        event->Flags = flags;
        event->EventTime = timeBitFieldsToSecs(eventPackedTime);
        event->UnknownTime = timeBitFieldsToSecs(UnknownPackedTime);
        event->DungeonId = dungeonId;
        event->Title = title;
        event->Description = description;

        sCalendarMgr.SendCalendarEventUpdateAlert(event, oldEventTime);

        // query construction
        CharacterDatabase.escape_string(title);
        CharacterDatabase.escape_string(description);
        CharacterDatabase.PExecute("UPDATE calendar_events SET "
                                   "type=%u, flags=%u, dungeonId=%d, eventTime=%u, title='%s', description='%s'"
                                   "WHERE eventid=" UI64FMTD,
                                   uint32(type), flags, dungeonId, uint32(event->EventTime), title.c_str(), description.c_str(), eventId);
    }
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_REMOVE_EVENT [%s]", _player->GetGuidStr().c_str());

    uint64 eventId;
    uint64 inviteId;
    uint32 Flags;

    recv_data >> eventId;
    recv_data >> inviteId;
    recv_data >> Flags;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Remove event (eventId=" UI64FMTD ", remover inviteId =" UI64FMTD ")", eventId, inviteId);
    sCalendarMgr.RemoveEvent(eventId, _player);
}

void WorldSession::HandleCalendarCopyEvent(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_COPY_EVENT [%s]", guid.GetString().c_str());

    uint64 eventId;
    uint64 inviteId;
    uint32 packedTime;

    recv_data >> eventId >> inviteId;
    recv_data >> packedTime;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "] inviteId [" UI64FMTD "]",
                     eventId, inviteId);

    sCalendarMgr.CopyEvent(eventId, timeBitFieldsToSecs(packedTime), guid);
}

void WorldSession::HandleCalendarEventInvite(WorldPacket& recv_data)
{
    ObjectGuid playerGuid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_INVITE [%s]", playerGuid.GetString().c_str());

    uint64 eventId;

    // TODO it seem its not inviteID but event->CreatorGuid
    uint64 inviteId;
    std::string name;
    bool isPreInvite;
    bool isGuildEvent;

    ObjectGuid inviteeGuid;
    Team inviteeTeam = TEAM_NONE;
    uint32 inviteeGuildId = 0;
    bool isIgnored = false;

    recv_data >> eventId >> inviteId >> name >> isPreInvite >> isGuildEvent;

    if (Player* player = sObjectAccessor.FindPlayerByName(name.c_str()))
    {
        // Invitee is online
        inviteeGuid = player->GetObjectGuid();
        inviteeTeam = player->GetTeam();
        inviteeGuildId = player->GetGuildId();
        if (player->GetSocial()->HasIgnore(playerGuid))
            isIgnored = true;
    }
    else
    {
        // Invitee offline, get data from database
        CharacterDatabase.escape_string(name);
        QueryResult* result = CharacterDatabase.PQuery("SELECT guid,race FROM characters WHERE name ='%s'", name.c_str());
        if (result)
        {
            Field* fields = result->Fetch();
            inviteeGuid = ObjectGuid(HIGHGUID_PLAYER, fields[0].GetUInt32());
            inviteeTeam = Player::TeamForRace(fields[1].GetUInt8());
            inviteeGuildId = Player::GetGuildIdFromDB(inviteeGuid);
            delete result;

            result = CharacterDatabase.PQuery("SELECT flags FROM character_social WHERE guid = %u AND friend = %u", inviteeGuid.GetCounter(), playerGuid.GetCounter());
            if (result)
            {
                fields = result->Fetch();
                if (fields[0].GetUInt8() & SOCIAL_FLAG_IGNORED)
                    isIgnored = true;
                delete result;
            }
        }
    }

    if (inviteeGuid.IsEmpty())
    {
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PLAYER_NOT_FOUND);
        return;
    }

    if (isIgnored)
    {
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_IGNORING_YOU_S, name.c_str());
        return;
    }

    if (_player->GetTeam() != inviteeTeam && !sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CALENDAR))
    {
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NOT_ALLIED);
        return;
    }

    if (!isPreInvite)
    {
        if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
        {
            if (event->IsGuildEvent() && event->GuildId == inviteeGuildId)
            {
                // we can't invite guild members to guild events
                sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NO_GUILD_INVITES);
                return;
            }

            sCalendarMgr.AddInvite(event, playerGuid, inviteeGuid, CALENDAR_STATUS_INVITED, CALENDAR_RANK_PLAYER, "", time(nullptr));
        }
        else
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
    }
    else
    {
        if (isGuildEvent && inviteeGuildId == _player->GetGuildId())
        {
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NO_GUILD_INVITES);
            return;
        }

        // create a temp invite to send it back to client
        CalendarInvite invite;
        invite.SenderGuid = playerGuid;
        invite.InviteeGuid = inviteeGuid;
        invite.Status = CALENDAR_STATUS_INVITED;
        invite.Rank = CALENDAR_RANK_PLAYER;
        invite.LastUpdateTime = time(nullptr);

        sCalendarMgr.SendCalendarEventInvite(&invite);
        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "PREINVITE> sender[%s], Invitee[%s]", playerGuid.GetString().c_str(), inviteeGuid.GetString().c_str());
    }
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_RSVP [%s]", guid.GetString().c_str());

    uint64 eventId;
    uint64 inviteId;
    uint32 status;

    recv_data >> eventId >> inviteId >> status;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "], InviteId [" UI64FMTD "], status %u",
                     eventId, inviteId, status);

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
    {
        // i think we still should be able to remove self from locked events
        if (status != CALENDAR_STATUS_REMOVED && event->Flags & CALENDAR_FLAG_INVITES_LOCKED)
        {
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_LOCKED);
            return;
        }

        if (CalendarInvite* invite = event->GetInviteById(inviteId))
        {
            if (invite->InviteeGuid != guid)
            {
                CalendarInvite* updaterInvite = event->GetInviteByGuid(guid);
                if (updaterInvite == nullptr)
                {
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NOT_INVITED);
                    return ;
                }

                if (updaterInvite->Rank != CALENDAR_RANK_MODERATOR && updaterInvite->Rank != CALENDAR_RANK_OWNER)
                {
                    // remover have not enough right to change invite status
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PERMISSIONS);
                    return;
                }
            }
            invite->Status = CalendarInviteStatus(status);
            invite->LastUpdateTime = time(nullptr);

            CharacterDatabase.PExecute("UPDATE calendar_invites SET status=%u, lastUpdateTime=%u WHERE inviteId = " UI64FMTD, status, uint32(invite->LastUpdateTime), invite->InviteId);
            sCalendarMgr.SendCalendarEventStatus(invite);
            sCalendarMgr.SendCalendarClearPendingAction(_player);
        }
        else
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NO_INVITE); // correct?
    }
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventRemoveInvite(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_REMOVE_INVITE [%s]", guid.GetString().c_str());

    ObjectGuid invitee;
    uint64 eventId;
    uint64 ownerInviteId; // isn't it sender's inviteId? TODO: need more test to see what we can do with that value
    uint64 inviteId;

    recv_data >> invitee.ReadAsPacked();
    recv_data >> inviteId >> ownerInviteId >> eventId;

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "], ownerInviteId [" UI64FMTD "], Invitee ([%s] id: [" UI64FMTD "])",
                     eventId, ownerInviteId, invitee.GetString().c_str(), inviteId);

    if (sCalendarMgr.GetEventById(eventId))
        sCalendarMgr.RemoveInvite(eventId, inviteId, guid);
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventStatus(WorldPacket& recv_data)
{
    ObjectGuid updaterGuid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_STATUS [%s]", updaterGuid.GetString().c_str());
    recv_data.hexlike();

    ObjectGuid invitee;
    uint64 eventId;
    uint64 inviteId;
    uint64 ownerInviteId; // isn't it sender's inviteId?
    uint32 status;

    recv_data >> invitee.ReadAsPacked();
    recv_data >> eventId >> inviteId >> ownerInviteId >> status;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "] ownerInviteId [" UI64FMTD "], Invitee ([%s] id: [" UI64FMTD "], status %u",
                     eventId, ownerInviteId, invitee.GetString().c_str(), inviteId, status);

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
    {
        if (CalendarInvite* invite = event->GetInviteById(inviteId))
        {
            if (invite->InviteeGuid != updaterGuid)
            {
                CalendarInvite* updaterInvite = event->GetInviteByGuid(updaterGuid);
                if (updaterInvite == nullptr)
                {
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NOT_INVITED);
                    return ;
                }

                if (updaterInvite->Rank != CALENDAR_RANK_MODERATOR && updaterInvite->Rank != CALENDAR_RANK_OWNER)
                {
                    // remover have not enough right to change invite status
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PERMISSIONS);
                    return;
                }
            }
            invite->Status = (CalendarInviteStatus)status;
            invite->LastUpdateTime = time(nullptr);            // not sure if we should set response time when moderator changes invite status

            CharacterDatabase.PExecute("UPDATE calendar_invites SET status=%u, lastUpdateTime=%u WHERE inviteId=" UI64FMTD, status, uint32(invite->LastUpdateTime), invite->InviteId);
            sCalendarMgr.SendCalendarEventStatus(invite);
            sCalendarMgr.SendCalendarClearPendingAction(sObjectMgr.GetPlayer(invitee));
        }
        else
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NO_INVITE);
    }
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventModeratorStatus(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_EVENT_MODERATOR_STATUS [%s]", guid.GetString().c_str());

    ObjectGuid invitee;
    uint64 eventId;
    uint64 inviteId;
    uint64 ownerInviteId; // isn't it sender's inviteId?
    uint32 rank;

    recv_data >> invitee.ReadAsPacked();
    recv_data >> eventId >>  inviteId >> ownerInviteId >> rank;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "] ownerInviteId [" UI64FMTD "], Invitee ([%s] id: [" UI64FMTD "], rank %u",
                     eventId, ownerInviteId, invitee.GetString().c_str(), inviteId, rank);

    if (CalendarEvent* event = sCalendarMgr.GetEventById(eventId))
    {
        if (CalendarInvite* invite = event->GetInviteById(inviteId))
        {
            if (invite->InviteeGuid != guid)
            {
                CalendarInvite* updaterInvite = event->GetInviteByGuid(guid);
                if (updaterInvite == nullptr)
                {
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NOT_INVITED);
                    return ;
                }
                if (updaterInvite->Rank != CALENDAR_RANK_MODERATOR && updaterInvite->Rank != CALENDAR_RANK_OWNER)
                {
                    // remover have not enough right to change invite status
                    sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PERMISSIONS);
                    return;
                }
            }

            if (CalendarModerationRank(rank) == CALENDAR_RANK_OWNER)
            {
                // cannot set owner
                sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_PERMISSIONS);
                return;
            }

            CharacterDatabase.PExecute("UPDATE calendar_invites SET `rank` = %u WHERE inviteId=" UI64FMTD, rank, invite->InviteId);
            invite->Rank = CalendarModerationRank(rank);
            sCalendarMgr.SendCalendarEventModeratorStatusAlert(invite);
        }
        else
            sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_NO_INVITE);
    }
    else
        sCalendarMgr.SendCalendarCommandResult(_player, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarComplain(WorldPacket& recv_data)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_COMPLAIN [%s]", guid.GetString().c_str());

    ObjectGuid badGuyGuid;
    uint64 eventId;
    uint64 inviteId;

    recv_data >> badGuyGuid;
    recv_data >> eventId >>  inviteId;
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "EventId [" UI64FMTD "], BadGuyGuid ([%s] inviteId: [" UI64FMTD "])",
                     eventId, badGuyGuid.GetString().c_str(), inviteId);

    // Remove the invite
    if (sCalendarMgr.RemoveInvite(eventId, inviteId, guid))
    {
        WorldPacket data(SMSG_COMPLAIN_RESULT, 1 + 1);
        data << uint8(0);
        data << uint8(0); // show complain saved. We can send 0x0C to show windows with ok button
        SendPacket(data);
    }
}

void WorldSession::HandleCalendarGetNumPending(WorldPacket& /*recv_data*/)
{
    ObjectGuid guid = _player->GetObjectGuid();
    DEBUG_LOG("WORLD: Received opcode CMSG_CALENDAR_GET_NUM_PENDING [%s]", guid.GetString().c_str());

    uint32 pending = sCalendarMgr.GetPlayerNumPending(guid);

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Pending: %u", pending);

    WorldPacket data(SMSG_CALENDAR_SEND_NUM_PENDING, 4);
    data << uint32(pending);
    SendPacket(data);
}

//////////////////////////////////////////////////////////////////////////
// Send function
//////////////////////////////////////////////////////////////////////////

void CalendarMgr::SendCalendarEventInviteAlert(CalendarInvite const* invite) const
{
    DEBUG_LOG("WORLD: SMSG_CALENDAR_EVENT_INVITE_ALERT");

    CalendarEvent const* event = invite->GetCalendarEvent();
    if (!event)
        return;

    WorldPacket data(SMSG_CALENDAR_EVENT_INVITE_ALERT);
    data << uint64(event->EventId);
    data << event->Title;
    data << secsToTimeBitFields(event->EventTime);
    data << uint32(event->Flags);
    data << uint32(event->Type);
    data << int32(event->DungeonId);
    data << uint64(invite->InviteId);
    data << uint8(invite->Status);
    data << uint8(invite->Rank);
    data << event->CreatorGuid.WriteAsPacked();
    data << invite->SenderGuid.WriteAsPacked();
    //data.hexlike();

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SendCalendarInviteAlert> senderGuid[%s], inviteeGuid[%s], EventId[" UI64FMTD "], Status[%u], InviteId[" UI64FMTD "]",
                     invite->SenderGuid.GetString().c_str(), invite->InviteeGuid.GetString().c_str(), event->EventId, uint32(invite->Status), invite->InviteId);

    if (event->IsGuildEvent() || event->IsGuildAnnouncement())
    {
        if (Guild* guild = sGuildMgr.GetGuildById(event->GuildId))
            guild->BroadcastPacket(data);
    }
    else if (Player* player = sObjectMgr.GetPlayer(invite->InviteeGuid))
        player->SendDirectMessage(data);
}

void CalendarMgr::SendCalendarEventInvite(CalendarInvite const* invite) const
{
    CalendarEvent const* event = invite->GetCalendarEvent();

    time_t statusTime = invite->LastUpdateTime;
    bool preInvite = true;
    uint64 eventId = 0;
    if (event != nullptr)
    {
        preInvite = false;
        eventId = event->EventId;
    }

    Player* player = sObjectMgr.GetPlayer(invite->InviteeGuid);

    uint8 level = player ? player->GetLevel() : Player::GetLevelFromDB(invite->InviteeGuid);
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_INVITE");
    WorldPacket data(SMSG_CALENDAR_EVENT_INVITE, 8 + 8 + 8 + 1 + 1 + 1 + (preInvite ? 0 : 4) + 1);
    data << invite->InviteeGuid.WriteAsPacked();
    data << uint64(eventId);
    data << uint64(invite->InviteId);
    data << uint8(level);
    data << uint8(invite->Status);
    data << uint8(!preInvite);
    if (!preInvite)
        data << secsToTimeBitFields(statusTime);
    data << uint8(invite->SenderGuid != invite->InviteeGuid); // false only if the invite is sign-up (invitee create himself his invite)

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SendCalendarInvit> %s senderGuid[%s], inviteeGuid[%s], EventId[" UI64FMTD "], Status[%u], InviteId[" UI64FMTD "]",
                     preInvite ? "is PreInvite," : "", invite->SenderGuid.GetString().c_str(), invite->InviteeGuid.GetString().c_str(), eventId, uint32(invite->Status), invite->InviteId);

    //data.hexlike();
    if (preInvite)
    {
        if (Player* sender = sObjectMgr.GetPlayer(invite->SenderGuid))
            sender->SendDirectMessage(data);
    }
    else
        SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendCalendarCommandResult(Player* player, CalendarError err, char const* param /*= nullptr*/) const
{
    if (!player)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_COMMAND_RESULT (%u)", err);
    WorldPacket data(SMSG_CALENDAR_COMMAND_RESULT, 0);
    data << uint32(0);
    data << uint8(0);
    switch (err)
    {
        case CALENDAR_ERROR_OTHER_INVITES_EXCEEDED_S:
        case CALENDAR_ERROR_ALREADY_INVITED_TO_EVENT_S:
        case CALENDAR_ERROR_IGNORING_YOU_S:
            data << param;
            break;
        default:
            data << uint8(0);
            break;
    }

    data << uint32(err);
    //data.hexlike();
    player->SendDirectMessage(data);
}

void CalendarMgr::SendCalendarEventRemovedAlert(CalendarEvent const* event) const
{
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_REMOVED_ALERT");
    WorldPacket data(SMSG_CALENDAR_EVENT_REMOVED_ALERT, 1 + 8 + 1);
    data << uint8(1);       // show pending alert?
    data << uint64(event->EventId);
    data << secsToTimeBitFields(event->EventTime);
    //data.hexlike();
    SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendCalendarEvent(Player* player, CalendarEvent const* event, uint32 sendType) const
{
    if (!player || !event)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SendCalendarEvent> sendType[%u], CreatorGuid[%s], EventId[" UI64FMTD "], Type[%u], Flags[%u], Title[%s]",
                     sendType, event->CreatorGuid.GetString().c_str(), event->EventId, uint32(event->Type), event->Flags, event->Title.c_str());

    WorldPacket data(SMSG_CALENDAR_SEND_EVENT);
    data << uint8(sendType);
    data << event->CreatorGuid.WriteAsPacked();
    data << uint64(event->EventId);
    data << event->Title;
    data << event->Description;
    data << uint8(event->Type);
    data << uint8(event->Repeatable);
    data << uint32(CALENDAR_MAX_INVITES);
    data << event->DungeonId;
    data << event->Flags;
    data << secsToTimeBitFields(event->EventTime);
    data << secsToTimeBitFields(event->UnknownTime);
    data << event->GuildId;

    CalendarInviteMap const* cInvMap = event->GetInviteMap();
    data << (uint32)cInvMap->size();
    for (const auto& itr : *cInvMap)
    {
        CalendarInvite const* invite = itr.second;
        ObjectGuid inviteeGuid = invite->InviteeGuid;
        Player* invitee = sObjectMgr.GetPlayer(inviteeGuid);

        uint8 inviteeLevel = invitee ? invitee->GetLevel() : Player::GetLevelFromDB(inviteeGuid);
        uint32 inviteeGuildId = invitee ? invitee->GetGuildId() : Player::GetGuildIdFromDB(inviteeGuid);

        data << inviteeGuid.WriteAsPacked();
        data << uint8(inviteeLevel);
        data << uint8(invite->Status);
        data << uint8(invite->Rank);
        data << uint8(event->IsGuildEvent() && event->GuildId == inviteeGuildId);
        data << uint64(itr.first);
        data << secsToTimeBitFields(invite->LastUpdateTime);
        data << invite->Text;

        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Invite> InviteId[" UI64FMTD "], InviteLvl[%u], Status[%u], Rank[%u],  GuildEvent[%s], Text[%s]",
                         invite->InviteId, uint32(inviteeLevel), uint32(invite->Status), uint32(invite->Rank),
                         (event->IsGuildEvent() && event->GuildId == inviteeGuildId) ? "true" : "false", invite->Text.c_str());
    }
    //data.hexlike();
    player->SendDirectMessage(data);
}

void CalendarMgr::SendCalendarEventInviteRemove(CalendarInvite const* invite, uint32 flags) const
{
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_INVITE_REMOVED");

    CalendarEvent const* event = invite->GetCalendarEvent();

    WorldPacket data(SMSG_CALENDAR_EVENT_INVITE_REMOVED, 8 + 4 + 4 + 1);
    data.appendPackGUID(invite->InviteeGuid);
    data << uint64(event->EventId);
    data << uint32(flags);
    data << uint8(1);       // show pending alert?
    //data.hexlike();
    SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendCalendarEventInviteRemoveAlert(Player* player, CalendarEvent const* event, CalendarInviteStatus status) const
{
    if (player)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT");
        WorldPacket data(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT, 8 + 4 + 4 + 1);
        data << uint64(event->EventId);
        data << secsToTimeBitFields(event->EventTime);
        data << uint32(event->Flags);
        data << uint8(status);
        //data.hexlike();
        player->SendDirectMessage(data);
    }
}

void CalendarMgr::SendCalendarEventStatus(CalendarInvite const* invite) const
{
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_STATUS");
    WorldPacket data(SMSG_CALENDAR_EVENT_STATUS, 8 + 8 + 4 + 4 + 1 + 1 + 4);
    CalendarEvent const* event = invite->GetCalendarEvent();

    data << invite->InviteeGuid.WriteAsPacked();
    data << uint64(event->EventId);
    data << secsToTimeBitFields(event->EventTime);
    data << uint32(event->Flags);
    data << uint8(invite->Status);
    data << uint8(invite->Rank);
    data << secsToTimeBitFields(invite->LastUpdateTime);
    //data.hexlike();
    SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendCalendarClearPendingAction(Player* player) const
{
    if (player)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_CLEAR_PENDING_ACTION TO [%s]", player->GetGuidStr().c_str());
        WorldPacket data(SMSG_CALENDAR_CLEAR_PENDING_ACTION, 0);
        player->SendDirectMessage(data);
    }
}

void CalendarMgr::SendCalendarEventModeratorStatusAlert(CalendarInvite const* invite) const
{
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT");
    CalendarEvent const* event = invite->GetCalendarEvent();
    WorldPacket data(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT, 8 + 8 + 1 + 1);
    data << invite->InviteeGuid.WriteAsPacked();
    data << uint64(event->EventId);
    data << uint8(invite->Rank);
    data << uint8(1); // Display pending action to client?
    //data.hexlike();
    SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendCalendarEventUpdateAlert(CalendarEvent const* event, time_t oldEventTime) const
{
    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "SMSG_CALENDAR_EVENT_UPDATED_ALERT");
    WorldPacket data(SMSG_CALENDAR_EVENT_UPDATED_ALERT, 1 + 8 + 4 + 4 + 4 + 1 + 4 +
                     event->Title.size() + event->Description.size() + 1 + 4 + 4);
    data << uint8(1);       // show pending alert?
    data << uint64(event->EventId);
    data << secsToTimeBitFields(oldEventTime);
    data << uint32(event->Flags);
    data << secsToTimeBitFields(event->EventTime);
    data << uint8(event->Type);
    data << int32(event->DungeonId);
    data << event->Title;
    data << event->Description;
    data << uint8(event->Repeatable);
    data << uint32(CALENDAR_MAX_INVITES);
    data << secsToTimeBitFields(event->UnknownTime);
    //data.hexlike();

    SendPacketToAllEventRelatives(data, event);
}

void CalendarMgr::SendPacketToAllEventRelatives(const WorldPacket& packet, CalendarEvent const* event) const
{
    // Send packet to all guild members
    if (event->IsGuildEvent() || event->IsGuildAnnouncement())
        if (Guild* guild = sGuildMgr.GetGuildById(event->GuildId))
            guild->BroadcastPacket(packet);

    // Send packet to all invitees if event is non-guild, in other case only to non-guild invitees (packet was broadcasted for them)
    CalendarInviteMap const* cInvMap = event->GetInviteMap();
    for (const auto& itr : *cInvMap)
        if (Player* player = sObjectMgr.GetPlayer(itr.second->InviteeGuid))
            if (!event->IsGuildEvent() || (event->IsGuildEvent() && player->GetGuildId() != event->GuildId))
                player->SendDirectMessage(packet);
}

void CalendarMgr::SendCalendarRaidLockoutRemove(Player* player, DungeonPersistentState const* save) const
{
    if (!save || !player)
        return;

    DEBUG_LOG("SMSG_CALENDAR_RAID_LOCKOUT_REMOVED [%s]", player->GetGuidStr().c_str());
    time_t currTime = time(nullptr);

    WorldPacket data(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED, 4 + 4 + 4 + 8);
    data << uint32(save->GetMapId());
    data << uint32(save->GetDifficulty());
    data << uint32(save->GetResetTime() - currTime);
    data << uint64(save->GetInstanceId());
    //data.hexlike();
    player->SendDirectMessage(data);
}

void CalendarMgr::SendCalendarRaidLockoutAdd(Player* player, DungeonPersistentState const* save) const
{
    if (!save || !player)
        return;

    DEBUG_LOG("SMSG_CALENDAR_RAID_LOCKOUT_ADDED [%s]", player->GetGuidStr().c_str());
    time_t currTime = time(nullptr);

    WorldPacket data(SMSG_CALENDAR_RAID_LOCKOUT_ADDED, 4 + 4 + 4 + 4 + 8);
    data << secsToTimeBitFields(currTime);
    data << uint32(save->GetMapId());
    data << uint32(save->GetDifficulty());
    data << uint32(save->GetResetTime() - currTime);
    data << uint64(save->GetInstanceId());
    //data.hexlike();
    player->SendDirectMessage(data);
}
