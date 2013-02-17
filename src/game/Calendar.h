/*
 * This file is part of the Continued-MaNGOS Project
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

#ifndef MANGOS_CALENDAR_H
#define MANGOS_CALENDAR_H

#include "Policies/SingletonImp.h"
#include "Common.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"

enum CalendarEventType
{
    CALENDAR_TYPE_RAID              = 0,
    CALENDAR_TYPE_DUNGEON           = 1,
    CALENDAR_TYPE_PVP               = 2,
    CALENDAR_TYPE_MEETING           = 3,
    CALENDAR_TYPE_OTHER             = 4
};

enum CalendarInviteStatus
{
    CALENDAR_STATUS_INVITED         = 0,
    CALENDAR_STATUS_ACCEPTED        = 1,
    CALENDAR_STATUS_DECLINED        = 2,
    CALENDAR_STATUS_CONFIRMED       = 3,
    CALENDAR_STATUS_OUT             = 4,
    CALENDAR_STATUS_STANDBY         = 5,
    CALENDAR_STATUS_SIGNED_UP       = 6,
    CALENDAR_STATUS_NOT_SIGNED_UP   = 7,
    CALENDAR_STATUS_TENTATIVE       = 8,
    CALENDAR_STATUS_REMOVED         = 9     // correct name?
};

// commented flag never showed in sniff but handled by client
enum CalendarFlags
{
    CALENDAR_FLAG_ALL_ALLOWED       = 0x001,
    //CALENDAR_FLAG_SYSTEM            = 0x004,
    //CALENDAR_FLAG_HOLLIDAY          = 0x008,
    CALENDAR_FLAG_INVITES_LOCKED    = 0x010,
    CALENDAR_FLAG_GUILD_ANNOUNCEMENT = 0x040,
    //CALENDAR_FLAG_RAID_LOCKOUT      = 0x080,
    //CALENDAR_FLAG_UNK_PLAYER        = 0x102,
    //CALENDAR_FLAG_RAID_RESET        = 0x200,
    CALENDAR_FLAG_GUILD_EVENT       = 0x400
};

enum CalendarRepeatType
{
    CALENDAR_REPEAT_NEVER           = 0,
    CALENDAR_REPEAT_WEEKLY          = 1,
    CALENDAR_REPEAT_BIWEEKLY        = 2,
    CALENDAR_REPEAT_MONTHLY         = 3
};

enum CalendarSendEventType
{
    CALENDAR_SENDTYPE_GET           = 0,
    CALENDAR_SENDTYPE_ADD           = 1,
    CALENDAR_SENDTYPE_COPY          = 2
};

enum CalendarModerationRank
{
    CALENDAR_RANK_PLAYER            = 0,
    CALENDAR_RANK_MODERATOR         = 1,
    CALENDAR_RANK_OWNER             = 2
};

enum CalendarError
{
    CALENDAR_OK                                 = 0,
    CALENDAR_ERROR_GUILD_EVENTS_EXCEEDED        = 1,
    CALENDAR_ERROR_EVENTS_EXCEEDED              = 2,
    CALENDAR_ERROR_SELF_INVITES_EXCEEDED        = 3,
    CALENDAR_ERROR_OTHER_INVITES_EXCEEDED_S     = 4,
    CALENDAR_ERROR_PERMISSIONS                  = 5,
    CALENDAR_ERROR_EVENT_INVALID                = 6,
    CALENDAR_ERROR_NOT_INVITED                  = 7,
    CALENDAR_ERROR_INTERNAL                     = 8,
    CALENDAR_ERROR_GUILD_PLAYER_NOT_IN_GUILD    = 9,
    CALENDAR_ERROR_ALREADY_INVITED_TO_EVENT_S   = 10,
    CALENDAR_ERROR_PLAYER_NOT_FOUND             = 11,
    CALENDAR_ERROR_NOT_ALLIED                   = 12,
    CALENDAR_ERROR_IGNORING_YOU_S               = 13,
    CALENDAR_ERROR_INVITES_EXCEEDED             = 14,
    CALENDAR_ERROR_INVALID_DATE                 = 16,
    CALENDAR_ERROR_INVALID_TIME                 = 17,

    CALENDAR_ERROR_NEEDS_TITLE                  = 19,
    CALENDAR_ERROR_EVENT_PASSED                 = 20,
    CALENDAR_ERROR_EVENT_LOCKED                 = 21,
    CALENDAR_ERROR_DELETE_CREATOR_FAILED        = 22,
    CALENDAR_ERROR_SYSTEM_DISABLED              = 24,
    CALENDAR_ERROR_RESTRICTED_ACCOUNT           = 25,
    CALENDAR_ERROR_ARENA_EVENTS_EXCEEDED        = 26,
    CALENDAR_ERROR_RESTRICTED_LEVEL             = 27,
    CALENDAR_ERROR_USER_SQUELCHED               = 28,
    CALENDAR_ERROR_NO_INVITE                    = 29,

    CALENDAR_ERROR_EVENT_WRONG_SERVER           = 36,
    CALENDAR_ERROR_INVITE_WRONG_SERVER          = 37,
    CALENDAR_ERROR_NO_GUILD_INVITES             = 38,
    CALENDAR_ERROR_INVALID_SIGNUP               = 39,
    CALENDAR_ERROR_NO_MODERATOR                 = 40
};

#define CALENDAR_MAX_EVENTS         30
#define CALENDAR_MAX_GUILD_EVENTS   100
#define CALENDAR_MAX_INVITES        100

// forward declaration
class CalendarEvent;
class CalendarInvite;
class CalendarMgr;
class Player;
class DungeonPersistentState;
class WorldPacket;

typedef std::map<uint64, CalendarInvite*> CalendarInviteMap;
typedef std::list<CalendarInvite*> CalendarInvitesList;
typedef std::list<CalendarEvent*> CalendarEventsList;

// CalendarEvent class used to store one event
class CalendarEvent
{
    public:
        CalendarEvent(uint64 eventId, uint64 creatorGUID, uint32 guildId, CalendarEventType type, int32 dungeonId,
                      time_t eventTime, uint32 flags, time_t unknownTime, std::string title, std::string description) :
            EventId(eventId), CreatorGuid(creatorGUID), GuildId(guildId), Type(type), DungeonId(dungeonId),
            EventTime(eventTime), Flags(flags), UnknownTime(unknownTime), Title(title),
            Description(description) { }

        CalendarEvent() : EventId(0), CreatorGuid(uint64(0)), GuildId(0), Type(CALENDAR_TYPE_OTHER), DungeonId(-1), EventTime(0),
            Flags(0), UnknownTime(0) { }

        ~CalendarEvent();

        // helper to test event flag
        bool IsGuildEvent() const { return Flags & CALENDAR_FLAG_GUILD_EVENT; }
        bool IsGuildAnnouncement() const { return Flags & CALENDAR_FLAG_GUILD_ANNOUNCEMENT; }

        bool AddInvite(CalendarInvite* invite);

        CalendarInviteMap const* GetInviteMap() const { return &m_Invitee; }

        CalendarInvite* GetInviteById(uint64 inviteId);
        CalendarInvite* GetInviteByGuid(ObjectGuid const& guid);

        bool RemoveInviteById(uint64 inviteId, Player* remover);
        void RemoveInviteByGuid(ObjectGuid const& playerGuid);
        void RemoveAllInvite(ObjectGuid const& remover);

        uint64 EventId;                         // unique ID for that event
        ObjectGuid CreatorGuid;                 // ObjectGuid of the event creator
        uint32 GuildId;                         // guild Id set only if its an guild event or guild announce
        CalendarEventType Type;                 // event type (look CalendarEventType enum)
        CalendarRepeatType Repeatable;          // not actualy used, maybe no need to store it because it seem not handled by client
        int32 DungeonId;                        // required for dungeon event type
        time_t EventTime;                       // event date
        uint32 Flags;                           // (look at CalendarFlags enum)
        time_t UnknownTime;                     // maybe related to timezone
        std::string Title;                      // title of the event
        std::string Description;                // description of the event

    private:
        void RemoveInviteByItr(CalendarInviteMap::iterator inviteItr);
        void RemoveAllInvite();

        CalendarInviteMap m_Invitee;
};

// CalendarInvite class store single invite information
class CalendarInvite
{
    public:

        CalendarInvite() : m_calendarEvent(NULL), InviteId(0), LastUpdateTime(time(NULL)), Status(CALENDAR_STATUS_INVITED), Rank(CALENDAR_RANK_PLAYER) {}

        CalendarInvite(CalendarEvent* event, uint64 inviteId, ObjectGuid senderGuid, ObjectGuid inviteeGuid, time_t statusTime,
                       CalendarInviteStatus status, CalendarModerationRank rank, std::string text);

        ~CalendarInvite() {}

        CalendarEvent const* GetCalendarEvent() const { return m_calendarEvent; }

        uint64 InviteId;                        // unique ID
        ObjectGuid InviteeGuid;                 // invitee ObjectGuid
        ObjectGuid SenderGuid;                  // the invite sender (can be equal to InviteeGuid)
        time_t LastUpdateTime;                  // last time updated
        CalendarInviteStatus Status;            // status of this invite (look CalendarInviteStatus enum)
        CalendarModerationRank Rank;            // rank of the invitee (look CalendarModerationRank enum)
        std::string Text;                       // seem not implemented in client

    private:
        CalendarEvent* m_calendarEvent;         // link to the parent events
};

// storage for all events
typedef std::map<uint64, CalendarEvent> CalendarEventStore;

// CalendarMgr class is the main class to handle events and their invites
class CalendarMgr
{
    public:
        CalendarMgr() : m_MaxEventId(0), m_MaxInviteId(0) {};
        ~CalendarMgr() {};

        void GetPlayerEventsList(ObjectGuid const& guid, CalendarEventsList& calEventList);
        void GetPlayerInvitesList(ObjectGuid const& guid, CalendarInvitesList& calInvList);

        CalendarEvent* AddEvent(ObjectGuid const& guid, std::string title, std::string description, uint32 type, uint32 repeatable, uint32 maxInvites,
                                int32 dungeonId, time_t eventTime, time_t unkTime, uint32 flags);

        CalendarInvite* AddInvite(CalendarEvent* event, ObjectGuid const& senderGuid, ObjectGuid const& inviteeGuid, CalendarInviteStatus status, CalendarModerationRank rank, std::string text, time_t statusTime);

        void RemoveEvent(uint64 eventId, Player* remover);
        bool RemoveInvite(uint64 eventId, uint64 inviteId, ObjectGuid const& removerGuid);
        void RemovePlayerCalendar(ObjectGuid const& playerGuid);
        void RemoveGuildCalendar(ObjectGuid const& playerGuid, uint32 GuildId);

        void CopyEvent(uint64 eventId, time_t newTime, ObjectGuid const& guid);
        uint32 GetPlayerNumPending(ObjectGuid const& guid);

        CalendarEvent* GetEventById(uint64 eventId)
        {
            CalendarEventStore::iterator itr = m_EventStore.find(eventId);
            return (itr != m_EventStore.end()) ? &itr->second : NULL;
        }

        // sql related
        void LoadCalendarsFromDB();

        // send data to client function
        void SendCalendarEventInvite(CalendarInvite const* invite);
        void SendCalendarEventInviteAlert(CalendarInvite const* invite);
        void SendCalendarCommandResult(Player* player, CalendarError err, char const* param = NULL);
        void SendCalendarEventRemovedAlert(CalendarEvent const* event);
        void SendCalendarEvent(Player* player, CalendarEvent const* event, uint32 sendType);
        void SendCalendarEventInviteRemoveAlert(Player* player, CalendarEvent const* event, CalendarInviteStatus status);
        void SendCalendarEventInviteRemove(CalendarInvite const* invite, uint32 flags);
        void SendCalendarEventStatus(CalendarInvite const* invite);
        void SendCalendarClearPendingAction(Player* player);
        void SendCalendarEventModeratorStatusAlert(CalendarInvite const* invite);
        void SendCalendarEventUpdateAlert(CalendarEvent const* event, time_t oldEventTime);
        void SendCalendarRaidLockoutRemove(Player* player, DungeonPersistentState const* save);
        void SendCalendarRaidLockoutAdd(Player* player, DungeonPersistentState const* save);

        void SendPacketToAllEventRelatives(WorldPacket packet, CalendarEvent const* event);

    private:
        uint64 GetNewEventId() { return ++m_MaxEventId; }
        uint64 GetNewInviteId() { return ++m_MaxInviteId; }

        bool CanAddInviteTo(ObjectGuid const& guid);    // check if player can recieve a new invite
        bool CanAddGuildEvent(uint32 guildId);          // check if guild not reached the event number limit
        bool CanAddEvent(ObjectGuid const& guid);       // check if player not reached the event number limit

        CalendarEventStore m_EventStore;        // main events storage
        uint64 m_MaxEventId;                    // current max event ID
        uint64 m_MaxInviteId;                   // current max invite ID
};

#define sCalendarMgr MaNGOS::Singleton<CalendarMgr>::Instance()

#endif
