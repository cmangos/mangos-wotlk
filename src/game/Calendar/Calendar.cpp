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

#include <utility>
#include "Calendar/Calendar.h"
#include "Mails/Mail.h"
#include "Globals/ObjectMgr.h"
#include "Util/ProgressBar.h"

INSTANTIATE_SINGLETON_1(CalendarMgr);

//////////////////////////////////////////////////////////////////////////
// CalendarEvent Class to store single event informations
//////////////////////////////////////////////////////////////////////////

CalendarEvent::~CalendarEvent()
{
    RemoveAllInvite();
}

// Add an invite to internal invite map return true if success
bool CalendarEvent::AddInvite(CalendarInvite* invite)
{
    if (!invite)
        return false;

    return m_Invitee.insert(CalendarInviteMap::value_type(invite->InviteId, invite)).second;
}

CalendarInvite* CalendarEvent::GetInviteById(uint64 inviteId)
{
    CalendarInviteMap::iterator itr = m_Invitee.find(inviteId);
    if (itr != m_Invitee.end())
        return itr->second;
    return nullptr;
}

CalendarInvite* CalendarEvent::GetInviteByGuid(ObjectGuid const& guid)
{
    CalendarInviteMap::const_iterator inviteItr = m_Invitee.begin();
    while (inviteItr != m_Invitee.end())
    {
        if (inviteItr->second->InviteeGuid == guid)
            return inviteItr->second;
        ++inviteItr;
    }

    return nullptr;
}

// remove invite by its iterator
void CalendarEvent::RemoveInviteByItr(CalendarInviteMap::iterator inviteItr)
{
    MANGOS_ASSERT(inviteItr != m_Invitee.end());    // iterator must be valid

    if (!IsGuildEvent())
        sCalendarMgr.SendCalendarEventInviteRemoveAlert(sObjectMgr.GetPlayer(inviteItr->second->InviteeGuid), this, CALENDAR_STATUS_REMOVED);

    sCalendarMgr.SendCalendarEventInviteRemove(inviteItr->second, Flags);

    CharacterDatabase.PExecute("DELETE FROM calendar_invites WHERE inviteId=" UI64FMTD, inviteItr->second->InviteId);

    delete inviteItr->second;
    m_Invitee.erase(inviteItr);
}

// remove invite by ObjectGuid of the player (may not be found so nothing removed)
void CalendarEvent::RemoveInviteByGuid(ObjectGuid const& playerGuid)
{
    CalendarInviteMap::iterator itr = m_Invitee.begin();
    while (itr != m_Invitee.end())
    {
        if (itr->second->InviteeGuid == playerGuid)
            RemoveInviteByItr(itr++);
        else
            ++itr;
    }
}

// remove invite by invite ID (some check done before and if requirement not complete
// operation is aborted and raison is sent to client
bool CalendarEvent::RemoveInviteById(uint64 inviteId, Player* remover)
{
    CalendarInviteMap::iterator inviteItr = m_Invitee.find(inviteId);
    if (inviteItr == m_Invitee.end())
    {
        // invite not found
        sCalendarMgr.SendCalendarCommandResult(remover, CALENDAR_ERROR_NO_INVITE);
        return false;
    }

    // assign a pointer to CalendarInvite class to make read more easy
    CalendarInvite* invite = inviteItr->second;

    if (invite->InviteeGuid != remover->GetObjectGuid())
    {
        // check if remover is an invitee
        CalendarInvite* removerInvite = GetInviteByGuid(remover->GetObjectGuid());
        if (removerInvite == nullptr)
        {
            // remover is not invitee cheat???
            sCalendarMgr.SendCalendarCommandResult(remover, CALENDAR_ERROR_NOT_INVITED);
            return false;
        }

        if (removerInvite->Rank != CALENDAR_RANK_MODERATOR && removerInvite->Rank != CALENDAR_RANK_OWNER)
        {
            // remover have not enough right to remove invite
            sCalendarMgr.SendCalendarCommandResult(remover, CALENDAR_ERROR_PERMISSIONS);
            return false;
        }
    }

    if (CreatorGuid == invite->InviteeGuid)
    {
        sCalendarMgr.SendCalendarCommandResult(remover, CALENDAR_ERROR_DELETE_CREATOR_FAILED);
        return false;
    }

    // TODO: Send mail to invitee if needed

    RemoveInviteByItr(inviteItr);
    return true;
}

// remove all invite without sending ingame mail
void CalendarEvent::RemoveAllInvite()
{
    CalendarInviteMap::iterator itr = m_Invitee.begin();
    while (itr != m_Invitee.end())
        RemoveInviteByItr(itr++);
}

// remove all invite sending ingame mail
void CalendarEvent::RemoveAllInvite(ObjectGuid const& removerGuid)
{
    // build mail title
    std::ostringstream title;
    title << removerGuid << ':' << Title;

    // build mail body
    std::ostringstream body;
    body << secsToTimeBitFields(time(nullptr));

    // creating mail draft
    MailDraft draft(title.str(), body.str());

    CalendarInviteMap::iterator itr = m_Invitee.begin();
    while (itr != m_Invitee.end())
    {
        if (removerGuid != itr->second->InviteeGuid)
            draft.SendMailTo(MailReceiver(itr->second->InviteeGuid), this, MAIL_CHECK_MASK_COPIED);
        RemoveInviteByItr(itr++);
    }
}

//////////////////////////////////////////////////////////////////////////
// CalendarInvite Classes store single invite information
//////////////////////////////////////////////////////////////////////////

CalendarInvite::CalendarInvite(CalendarEvent* event, uint64 inviteId, ObjectGuid senderGuid, ObjectGuid inviteeGuid, time_t statusTime, CalendarInviteStatus status, CalendarModerationRank rank, std::string text) :
    InviteId(inviteId),
    InviteeGuid(inviteeGuid),
    SenderGuid(senderGuid),
    LastUpdateTime(statusTime),
    Status(status),
    Rank(rank),
    Text(std::move(text)),
    m_calendarEvent(event)
{
    // only for pre invite case
    if (!event)
        InviteId = 0;
}

//////////////////////////////////////////////////////////////////////////
// CalendarMgr Classes handle all events and invites.
//////////////////////////////////////////////////////////////////////////

// fill all player events in provided CalendarEventsList
void CalendarMgr::GetPlayerEventsList(ObjectGuid const& guid, CalendarEventsList& calEventList)
{
    uint32 guildId;
    Player* player = sObjectMgr.GetPlayer(guid);
    if (player)
        guildId = player->GetGuildId();
    else
        guildId = Player::GetGuildIdFromDB(guid);

    for (auto& itr : m_EventStore)
    {
        CalendarEvent* event = &itr.second;

        // add own event and same guild event or announcement
        if ((event->CreatorGuid == guid) || ((event->IsGuildAnnouncement() || event->IsGuildEvent()) && event->GuildId == guildId))
        {
            calEventList.push_back(event);
            continue;
        }

        // add all event where player is invited
        if (event->GetInviteByGuid(guid))
            calEventList.push_back(event);
    }
}

// fill all player invites in provided CalendarInvitesList
void CalendarMgr::GetPlayerInvitesList(ObjectGuid const& guid, CalendarInvitesList& calInvList)
{
    for (auto& itr : m_EventStore)
    {
        CalendarEvent* event = &itr.second;

        if (event->IsGuildAnnouncement())
            continue;

        CalendarInviteMap const* cInvMap = event->GetInviteMap();
        CalendarInviteMap::const_iterator ci_itr = cInvMap->begin();
        while (ci_itr != cInvMap->end())
        {
            if (ci_itr->second->InviteeGuid == guid)
            {
                calInvList.push_back(ci_itr->second);
                break;
            }
            ++ci_itr;
        }
    }
}

// add single event to main events store
// some check done before so it may fail and raison is sent to client
// return value is the CalendarEvent pointer on success
CalendarEvent* CalendarMgr::AddEvent(ObjectGuid const& guid, std::string title, std::string description, uint32 type, uint32 repeatable,
                                     uint32 maxInvites, int32 dungeonId, time_t eventTime, time_t /*unkTime*/, uint32 flags)
{
    Player* player = sObjectMgr.GetPlayer(guid);
    if (!player)
        return nullptr;

    if (title.empty())
    {
        SendCalendarCommandResult(player, CALENDAR_ERROR_NEEDS_TITLE);
        return nullptr;
    }

    if (eventTime < time(nullptr))
    {
        SendCalendarCommandResult(player, CALENDAR_ERROR_INVALID_DATE);
        return nullptr;
    }

    uint32 guildId = 0;

    if ((flags & CALENDAR_FLAG_GUILD_EVENT) || (flags & CALENDAR_FLAG_GUILD_ANNOUNCEMENT))
    {
        guildId = player->GetGuildId();
        if (!guildId)
        {
            SendCalendarCommandResult(player, CALENDAR_ERROR_GUILD_PLAYER_NOT_IN_GUILD);
            return nullptr;
        }

        if (!CanAddGuildEvent(guildId))
        {
            SendCalendarCommandResult(player, CALENDAR_ERROR_GUILD_EVENTS_EXCEEDED);
            return nullptr;
        }
    }
    else
    {
        if (!CanAddEvent(guid))
        {
            SendCalendarCommandResult(player, CALENDAR_ERROR_EVENTS_EXCEEDED);
            return nullptr;
        }
    }

    uint64 nId = GetNewEventId();

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "CalendarMgr::AddEvent> ID(" UI64FMTD "), '%s', Desc > '%s', type=%u, repeat=%u, maxInvites=%u, dungeonId=%d, flags=%u",
                     nId, title.c_str(), description.c_str(), type, repeatable, maxInvites, dungeonId, flags);

    CalendarEvent& newEvent = m_EventStore[nId];

    newEvent.EventId = nId;
    newEvent.CreatorGuid = guid;
    newEvent.Title = title;
    newEvent.Description = description;
    newEvent.Type = (CalendarEventType) type;
    newEvent.Repeatable = (CalendarRepeatType) repeatable;
    newEvent.DungeonId = dungeonId;
    newEvent.EventTime = eventTime;
    newEvent.Flags = flags;
    newEvent.GuildId = guildId;

    CharacterDatabase.escape_string(title);
    CharacterDatabase.escape_string(description);
    CharacterDatabase.PExecute("INSERT INTO calendar_events VALUES (" UI64FMTD ", %u, %u, %u, %u, %d, %u, '%s', '%s')",
                               nId,
                               guid.GetCounter(),
                               guildId,
                               type,
                               flags,
                               dungeonId,
                               uint32(eventTime),
                               title.c_str(),
                               description.c_str());
    return &newEvent;
}

// remove event by its id
// some check done before so it may fail and raison is sent to client
void CalendarMgr::RemoveEvent(uint64 eventId, Player* remover)
{
    CalendarEventStore::iterator citr = m_EventStore.find(eventId);
    if (citr == m_EventStore.end())
    {
        SendCalendarCommandResult(remover, CALENDAR_ERROR_EVENT_INVALID);
        return;
    }

    if (remover->GetObjectGuid() != citr->second.CreatorGuid)
    {
        // only creator can remove his event
        SendCalendarCommandResult(remover, CALENDAR_ERROR_PERMISSIONS);
        return;
    }

    SendCalendarEventRemovedAlert(&citr->second);

    CharacterDatabase.PExecute("DELETE FROM calendar_events WHERE eventId=" UI64FMTD, eventId);

    // explicitly remove all invite and send mail to all invitee
    citr->second.RemoveAllInvite(remover->GetObjectGuid());
    m_EventStore.erase(citr);
}

// Add invit to an event and inform client
// some check done before so it may fail and raison is sent to client
// return value is the CalendarInvite pointer on success
CalendarInvite* CalendarMgr::AddInvite(CalendarEvent* event, ObjectGuid const& senderGuid, ObjectGuid const& inviteeGuid, CalendarInviteStatus status, CalendarModerationRank rank, const std::string& text, time_t statusTime)
{
    Player* sender = sObjectMgr.GetPlayer(senderGuid);
    if (!event || !sender)
        return nullptr;

    std::string name;
    sObjectMgr.GetPlayerNameByGUID(inviteeGuid, name);

    // check if invitee is not already invited
    if (event->GetInviteByGuid(inviteeGuid))
    {
        SendCalendarCommandResult(sender, CALENDAR_ERROR_ALREADY_INVITED_TO_EVENT_S, name.c_str());
        return nullptr;
    }

    // check if player can still have new invite (except for event creator)
    if (!event->IsGuildAnnouncement() && event->CreatorGuid != inviteeGuid)
    {
        if (!CanAddInviteTo(inviteeGuid))
        {
            SendCalendarCommandResult(sender, CALENDAR_ERROR_OTHER_INVITES_EXCEEDED_S, name.c_str());
            return nullptr;
        }
    }

    CalendarInvite* invite = new CalendarInvite(event, GetNewInviteId(), senderGuid, inviteeGuid, statusTime, status, rank, text);

    if (!event->IsGuildAnnouncement())
        SendCalendarEventInvite(invite);

    if (!event->IsGuildEvent() || invite->InviteeGuid == event->CreatorGuid)
        SendCalendarEventInviteAlert(invite);

    if (event->IsGuildAnnouncement())
    {
        // no need to realy add invite for announcements
        delete invite;
        return nullptr;
    }

    DEBUG_FILTER_LOG(LOG_FILTER_CALENDAR, "Add Invite> eventId[" UI64FMTD "], senderGuid[%s], inviteGuid[%s], Status[%u], rank[%u], text[%s], time[%u]",
                     event->EventId, senderGuid.GetString().c_str(), inviteeGuid.GetString().c_str(), uint32(status), uint32(rank), text.c_str(), uint32(statusTime));

    if (!event->AddInvite(invite))
    {
        sLog.outError("CalendarEvent::AddInvite > Fail adding invite!");
        delete invite;
        return nullptr;
    }

    CharacterDatabase.PExecute("INSERT INTO calendar_invites VALUES (" UI64FMTD ", " UI64FMTD ", %u, %u, %u, %u, %u)",
                               invite->InviteId,
                               event->EventId,
                               inviteeGuid.GetCounter(),
                               senderGuid.GetCounter(),
                               uint32(status),
                               uint32(statusTime),
                               uint32(rank));

    return invite;
}

// remove invit from an event and inform client
// some check done before so it may fail and raison is sent to client
// require valid eventId/inviteId and correct right for the remover.
bool CalendarMgr::RemoveInvite(uint64 eventId, uint64 inviteId, ObjectGuid const& removerGuid)
{
    Player* remover = sObjectMgr.GetPlayer(removerGuid);
    CalendarEventStore::iterator citr = m_EventStore.find(eventId);
    if (citr == m_EventStore.end())
    {
        SendCalendarCommandResult(remover, CALENDAR_ERROR_EVENT_INVALID);
        return false;
    }

    CalendarEvent& event = citr->second;

    return event.RemoveInviteById(inviteId, remover);
}

// return how many events still require some pending action
uint32 CalendarMgr::GetPlayerNumPending(ObjectGuid const& guid)
{
    CalendarInvitesList inviteList;
    GetPlayerInvitesList(guid, inviteList);

    uint32 pendingNum = 0;
    time_t currTime = time(nullptr);
    for (CalendarInvitesList::const_iterator itr = inviteList.begin(); itr != inviteList.end(); ++itr)
    {
        if (CalendarEvent const* event = (*itr)->GetCalendarEvent())
        {
            // pass all passed events
            if (event->EventTime < currTime)
                continue;

            // pass all locked events
            if (event->Flags & CALENDAR_FLAG_INVITES_LOCKED)
                continue;
        }

        // add only invite that require some action
        if ((*itr)->Status == CALENDAR_STATUS_INVITED || (*itr)->Status == CALENDAR_STATUS_TENTATIVE || (*itr)->Status == CALENDAR_STATUS_NOT_SIGNED_UP)
            ++pendingNum;
    }

    return pendingNum;
}

// copy event to another date (all invitee is copied too but their status are reseted)
void CalendarMgr::CopyEvent(uint64 eventId, time_t newTime, ObjectGuid const& guid)
{
    Player* player = sObjectMgr.GetPlayer(guid);
    CalendarEvent* event = GetEventById(eventId);
    if (!event)
    {
        SendCalendarCommandResult(player, CALENDAR_ERROR_EVENT_INVALID);
        return;
    }

    CalendarEvent* newEvent = AddEvent(guid, event->Title, event->Description, event->Type, event->Repeatable,
                                       CALENDAR_MAX_INVITES, event->DungeonId, newTime, event->UnknownTime, event->Flags);

    if (!newEvent)
        return;

    if (newEvent->IsGuildAnnouncement())
        AddInvite(newEvent, guid, guid,  CALENDAR_STATUS_CONFIRMED, CALENDAR_RANK_OWNER, "", time(nullptr));
    else
    {
        // copy all invitees, set new owner as the one who make the copy, set invitees status to invited
        CalendarInviteMap const* cInvMap = event->GetInviteMap();
        CalendarInviteMap::const_iterator ci_itr = cInvMap->begin();

        while (ci_itr != cInvMap->end())
        {
            const CalendarInvite* invite = ci_itr->second;
            if (invite->InviteeGuid == guid)
            {
                AddInvite(newEvent, guid, invite->InviteeGuid, CALENDAR_STATUS_CONFIRMED, CALENDAR_RANK_OWNER, "", time(nullptr));
            }
            else
            {
                CalendarModerationRank rank = CALENDAR_RANK_PLAYER;
                // copy moderator rank
                if (invite->Rank == CALENDAR_RANK_MODERATOR)
                    rank = CALENDAR_RANK_MODERATOR;

                AddInvite(newEvent, guid, invite->InviteeGuid, CALENDAR_STATUS_INVITED, rank, "", time(nullptr));
            }
            ++ci_itr;
        }
    }

    SendCalendarEvent(player, newEvent, CALENDAR_SENDTYPE_COPY);
}

// remove all events and invite of player
// used when player is deleted
void CalendarMgr::RemovePlayerCalendar(ObjectGuid const& playerGuid)
{
    CalendarEventStore::iterator itr = m_EventStore.begin();

    while (itr != m_EventStore.end())
    {
        if (itr->second.CreatorGuid == playerGuid)
        {
            // all invite will be automaticaly deleted
            m_EventStore.erase(itr++);
            // itr already incremented so go recheck event owner
            continue;
        }
        // event not owned by playerGuid but an invite can still be found
        CalendarEvent* event = &itr->second;
        event->RemoveInviteByGuid(playerGuid);
        ++itr;
    }
}

// remove all events and invite of player related to a specific guild
// used when player quit a guild
void CalendarMgr::RemoveGuildCalendar(ObjectGuid const& playerGuid, uint32 GuildId)
{
    CalendarEventStore::iterator itr = m_EventStore.begin();

    while (itr != m_EventStore.end())
    {
        CalendarEvent* event = &itr->second;
        if (event->CreatorGuid == playerGuid && (event->IsGuildEvent() || event->IsGuildAnnouncement()))
        {
            // all invite will be automaticaly deleted
            m_EventStore.erase(itr++);
            // itr already incremented so go recheck event owner
            continue;
        }

        // event not owned by playerGuid but an guild invite can still be found
        if (event->GuildId != GuildId || !(event->IsGuildEvent() || event->IsGuildAnnouncement()))
        {
            ++itr;
            continue;
        }

        event->RemoveInviteByGuid(playerGuid);
        ++itr;
    }
}

// load all events and their related invites from invite
void CalendarMgr::LoadCalendarsFromDB()
{
    // in case of reload (not yet implemented)
    m_MaxInviteId = 0;
    m_MaxEventId = 0;
    m_EventStore.clear();

    sLog.outString("Loading Calendar Events...");

    //                                                          0        1            2        3     4      5          6          7      8
    QueryResult* eventsQuery = CharacterDatabase.Query("SELECT eventId, creatorGuid, guildId, type, flags, dungeonId, eventTime, title, description FROM calendar_events ORDER BY eventId");
    if (!eventsQuery)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> calendar_events table is empty!");
        sLog.outString();
    }
    else
    {
        BarGoLink bar(eventsQuery->GetRowCount());
        do
        {
            Field* field = eventsQuery->Fetch();
            bar.step();

            uint64 eventId         = field[0].GetUInt64();

            CalendarEvent& newEvent = m_EventStore[eventId];

            newEvent.EventId       = eventId;
            newEvent.CreatorGuid   = ObjectGuid(HIGHGUID_PLAYER, field[1].GetUInt32());
            newEvent.GuildId       = field[2].GetUInt32();
            newEvent.Type          = CalendarEventType(field[3].GetUInt8());
            newEvent.Flags         = field[4].GetUInt32();
            newEvent.DungeonId     = field[5].GetInt32();
            newEvent.EventTime     = time_t(field[6].GetUInt32());
            newEvent.Title         = field[7].GetCppString();
            newEvent.Description   = field[8].GetCppString();

            m_MaxEventId = std::max(eventId, m_MaxEventId);
        }
        while (eventsQuery->NextRow());

        sLog.outString(">> Loaded %u events!", uint32(eventsQuery->GetRowCount()));
        sLog.outString();
        delete eventsQuery;
    }

    sLog.outString("Loading Calendar invites...");
    //                                                           0         1        2            3           4       5               6
    QueryResult* invitesQuery = CharacterDatabase.Query("SELECT inviteId, eventId, inviteeGuid, senderGuid, status, lastUpdateTime, `rank` FROM calendar_invites ORDER BY inviteId");
    if (!invitesQuery)
    {
        BarGoLink bar(1);
        bar.step();

        if (m_MaxEventId)                                   // An Event was loaded before
        {
            // delete all events (no event exist without at least one invite)
            m_EventStore.clear();
            m_MaxEventId = 0;
            CharacterDatabase.DirectExecute("TRUNCATE TABLE calendar_events");
            sLog.outString(">> calendar_invites table is empty, cleared calendar_events table!");
        }
        else
            sLog.outString(">> calendar_invite table is empty!");
        sLog.outString();
    }
    else
    {
        if (m_MaxEventId)
        {
            uint64 totalInvites = 0;
            uint32 deletedInvites = 0;
            BarGoLink bar(invitesQuery->GetRowCount());
            do
            {
                Field* field = invitesQuery->Fetch();
                bar.step();

                uint64 inviteId             = field[0].GetUInt64();
                uint64 eventId              = field[1].GetUInt64();
                ObjectGuid inviteeGuid      = ObjectGuid(HIGHGUID_PLAYER, field[2].GetUInt32());
                ObjectGuid senderGuid       = ObjectGuid(HIGHGUID_PLAYER, field[3].GetUInt32());
                CalendarInviteStatus status = CalendarInviteStatus(field[4].GetUInt8());
                time_t lastUpdateTime       = time_t(field[5].GetUInt32());
                CalendarModerationRank rank = CalendarModerationRank(field[6].GetUInt8());

                CalendarEvent* event = GetEventById(eventId);
                if (!event)
                {
                    // delete invite
                    CharacterDatabase.PExecute("DELETE FROM calendar_invites WHERE inviteId =" UI64FMTD, field[0].GetUInt64());
                    ++deletedInvites;
                    continue;
                }

                CalendarInvite* invite = new CalendarInvite(event, inviteId, senderGuid, inviteeGuid, lastUpdateTime, status, rank, "");
                event->AddInvite(invite);
                ++totalInvites;
                m_MaxInviteId = std::max(inviteId, m_MaxInviteId);
            }
            while (invitesQuery->NextRow());

            sLog.outString(">> Loaded " UI64FMTD " invites! %s", totalInvites, (deletedInvites != 0) ? "(deleted some invites without corresponding event!)" : "");
        }
        else
        {
            // delete all invites (no invites exist without events)
            CharacterDatabase.DirectExecute("TRUNCATE TABLE calendar_invites");
            sLog.outString(">> calendar_invites table is cleared! (invites without events found)");
        }
        delete invitesQuery;
    }
    sLog.outString();
}

// check if player have not reached event limit
bool CalendarMgr::CanAddEvent(ObjectGuid const& guid)
{
    uint32 totalEvents = 0;
    // count all event created by guid
    for (auto& itr : m_EventStore)
        if ((itr.second.CreatorGuid == guid) && (++totalEvents >= CALENDAR_MAX_EVENTS))
            return false;
    return true;
}

// check if guild have not reached event limit
bool CalendarMgr::CanAddGuildEvent(uint32 guildId)
{
    if (!guildId)
        return false;

    uint32 totalEvents = 0;
    // count all guild events in a guild
    for (auto& itr : m_EventStore)
        if ((itr.second.GuildId == guildId) && (++totalEvents >= CALENDAR_MAX_GUILD_EVENTS))
            return false;
    return true;
}

// check if an invitee have not reached invite limit
bool CalendarMgr::CanAddInviteTo(ObjectGuid const& guid)
{
    uint32 totalInvites = 0;

    for (auto& itr : m_EventStore)
    {
        CalendarEvent* event = &itr.second;

        if (event->IsGuildAnnouncement())
            continue;

        CalendarInviteMap const* cInvMap = event->GetInviteMap();
        CalendarInviteMap::const_iterator ci_itr = cInvMap->begin();
        while (ci_itr != cInvMap->end())
        {
            if ((ci_itr->second->InviteeGuid == guid) && (++totalInvites >= CALENDAR_MAX_INVITES))
                return false;
            ++ci_itr;
        }
    }

    return true;
}
