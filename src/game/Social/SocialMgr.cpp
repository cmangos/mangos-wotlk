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

#include "Social/SocialMgr.h"
#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"
#include "Server/Opcodes.h"
#include "Server/WorldPacket.h"
#include "Entities/Player.h"
#include "Globals/ObjectMgr.h"
#include "World/World.h"
#include "Util/Util.h"

INSTANTIATE_SINGLETON_1(SocialMgr);

PlayerSocial::PlayerSocial(): m_playerLowGuid(0)
{
}

PlayerSocial::~PlayerSocial()
{
    m_playerSocialMap.clear();
}

uint32 PlayerSocial::GetNumberOfSocialsWithFlag(SocialFlag flag)
{
    uint32 counter = 0;
    for (PlayerSocialMap::const_iterator itr = m_playerSocialMap.begin(); itr != m_playerSocialMap.end(); ++itr)
    {
        if (itr->second.Flags & flag)
            ++counter;
    }
    return counter;
}

bool PlayerSocial::AddToSocialList(ObjectGuid friend_guid, bool ignore)
{
    // check client limits
    if (ignore)
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_IGNORED) >= SOCIALMGR_IGNORE_LIMIT)
            return false;
    }
    else
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_FRIEND) >= SOCIALMGR_FRIEND_LIMIT)
            return false;
    }

    uint32 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friend_guid.GetCounter());
    if (itr != m_playerSocialMap.end())
    {
        CharacterDatabase.PExecute("UPDATE character_social SET flags = (flags | %u) WHERE guid = '%u' AND friend = '%u'", flag, m_playerLowGuid, friend_guid.GetCounter());
        m_playerSocialMap[friend_guid.GetCounter()].Flags |= flag;
    }
    else
    {
        CharacterDatabase.PExecute("INSERT INTO character_social (guid, friend, flags) VALUES ('%u', '%u', '%u')", m_playerLowGuid, friend_guid.GetCounter(), flag);
        FriendInfo fi;
        fi.Flags |= flag;
        m_playerSocialMap[friend_guid.GetCounter()] = fi;
    }
    return true;
}

void PlayerSocial::RemoveFromSocialList(ObjectGuid friend_guid, bool ignore)
{
    PlayerSocialMap::iterator itr = m_playerSocialMap.find(friend_guid.GetCounter());
    if (itr == m_playerSocialMap.end())                     // not exist
        return;

    uint32 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    itr->second.Flags &= ~flag;
    if (itr->second.Flags == 0)
    {
        CharacterDatabase.PExecute("DELETE FROM character_social WHERE guid = '%u' AND friend = '%u'", m_playerLowGuid, friend_guid.GetCounter());
        m_playerSocialMap.erase(itr);
    }
    else
    {
        CharacterDatabase.PExecute("UPDATE character_social SET flags = (flags & ~%u) WHERE guid = '%u' AND friend = '%u'", flag, m_playerLowGuid, friend_guid.GetCounter());
    }
}

void PlayerSocial::SetFriendNote(ObjectGuid friend_guid, std::string note)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friend_guid.GetCounter());
    if (itr == m_playerSocialMap.end())                     // not exist
        return;

    utf8truncate(note, 48);                                 // DB and client size limitation

    std::string safe_note = note;
    CharacterDatabase.escape_string(safe_note);
    CharacterDatabase.PExecute("UPDATE character_social SET note = '%s' WHERE guid = '%u' AND friend = '%u'", safe_note.c_str(), m_playerLowGuid, friend_guid.GetCounter());
    m_playerSocialMap[friend_guid.GetCounter()].Note = note;
}

void PlayerSocial::SendSocialList()
{
    Player* plr = sObjectMgr.GetPlayer(ObjectGuid(HIGHGUID_PLAYER, m_playerLowGuid));
    if (!plr)
        return;

    uint32 size = m_playerSocialMap.size();

    SocialListFlags flags = (SocialListFlags)0;

    WorldPacket data(SMSG_CONTACT_LIST, (4 + 4 + size * 25)); // just can guess size

    size_t flagPos = data.wpos();
    data << uint32(0);
    data << uint32(size);                                   // friends count

    for (auto& itr : m_playerSocialMap)
    {
        FriendInfo& friendInfo = itr.second;
        sSocialMgr.GetFriendInfo(plr, itr.first, friendInfo);

        data << ObjectGuid(HIGHGUID_PLAYER, itr.first);    // player guid
        data << uint32(friendInfo.Flags);                  // player flag (0x1-friend?, 0x2-ignored?, 0x4-muted?)
        data << friendInfo.Note;                           // string note
        if (friendInfo.Flags & SOCIAL_FLAG_FRIEND)         // if IsFriend()
        {
            data << uint8(friendInfo.Status);              // online/offline/etc?
            if (friendInfo.Status)                         // if online
            {
                data << uint32(friendInfo.Area);           // player area
                data << uint32(friendInfo.Level);          // player level
                data << uint32(friendInfo.Class);          // player class
            }
        }

        if (friendInfo.Flags & SOCIAL_FLAG_FRIEND)
            flags |= SocialListFlags::FRIEND_LIST;
        if (friendInfo.Flags & SOCIAL_FLAG_IGNORED)
            flags |= SocialListFlags::IGNORE_LIST;
        if (friendInfo.Flags & SOCIAL_FLAG_MUTED)
            flags |= SocialListFlags::MUTE_LIST;
    }

    data.put<uint32>(flagPos, uint32(flags));

    plr->GetSession()->SendPacket(data);
    DEBUG_LOG("WORLD: Sent SMSG_CONTACT_LIST");
}

bool PlayerSocial::HasFriend(ObjectGuid friend_guid)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friend_guid.GetCounter());
    if (itr != m_playerSocialMap.end())
        return itr->second.Flags & SOCIAL_FLAG_FRIEND;
    return false;
}

bool PlayerSocial::HasIgnore(ObjectGuid ignore_guid)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(ignore_guid.GetCounter());
    return itr == m_playerSocialMap.end() ? false : (itr->second.Flags & SOCIAL_FLAG_IGNORED) != 0;
}

SocialMgr::SocialMgr()
{
}

SocialMgr::~SocialMgr()
{
}

void SocialMgr::GetFriendInfo(Player* player, uint32 friend_lowguid, FriendInfo& friendInfo) const
{
    if (!player)
        return;

    Player* pFriend = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, friend_lowguid));

    Team team = player->GetTeam();
    AccountTypes security = player->GetSession()->GetSecurity();
    bool allowTwoSideWhoList = sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_WHO_LIST);
    AccountTypes gmLevelInWhoList = AccountTypes(sWorld.getConfig(CONFIG_UINT32_GM_LEVEL_IN_WHO_LIST));

    PlayerSocialMap::iterator itr = player->GetSocial()->m_playerSocialMap.find(friend_lowguid);
    if (itr != player->GetSocial()->m_playerSocialMap.end())
        friendInfo.Note = itr->second.Note;

    // PLAYER see his team only and PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
    // MODERATOR, GAME MASTER, ADMINISTRATOR can see all
    if (pFriend && pFriend->GetName() &&
            (security > SEC_PLAYER ||
             ((pFriend->GetTeam() == team || allowTwoSideWhoList) && (pFriend->GetSession()->GetSecurity() <= gmLevelInWhoList))) &&
            pFriend->IsVisibleGloballyFor(player))
    {
        friendInfo.Status = FRIEND_STATUS_ONLINE;
        if (pFriend->isAFK())
            friendInfo.Status = FRIEND_STATUS_AFK;
        if (pFriend->isDND())
            friendInfo.Status = FRIEND_STATUS_DND;
        friendInfo.Area = pFriend->GetZoneId();
        friendInfo.Level = pFriend->GetLevel();
        friendInfo.Class = pFriend->getClass();
    }
    else
    {
        friendInfo.Status = FRIEND_STATUS_OFFLINE;
        friendInfo.Area = 0;
        friendInfo.Level = 0;
        friendInfo.Class = 0;
    }
}

void SocialMgr::MakeFriendStatusPacket(FriendsResult result, uint32 guid, WorldPacket& data)
{
    data.Initialize(SMSG_FRIEND_STATUS, 5);
    data << uint8(result);
    data << ObjectGuid(HIGHGUID_PLAYER, guid);
}

void SocialMgr::SendFriendStatus(Player* player, FriendsResult result, ObjectGuid friend_guid, bool broadcast)
{
    uint32 friend_lowguid = friend_guid.GetCounter();

    FriendInfo fi;

    WorldPacket data;
    MakeFriendStatusPacket(result, friend_lowguid, data);
    GetFriendInfo(player, friend_lowguid, fi);
    switch (result)
    {
        case FRIEND_ADDED_OFFLINE:
        case FRIEND_ADDED_ONLINE:
            data << fi.Note;
            break;
        default:
            break;
    }

    switch (result)
    {
        case FRIEND_ADDED_ONLINE:
        case FRIEND_ONLINE:
            data << uint8(fi.Status);
            data << uint32(fi.Area);
            data << uint32(fi.Level);
            data << uint32(fi.Class);
            break;
        default:
            break;
    }

    if (broadcast)
        BroadcastToFriendListers(player, data);
    else
        player->GetSession()->SendPacket(data);
}

void SocialMgr::BroadcastToFriendListers(Player* player, WorldPacket const& packet)
{
    if (!player)
        return;

    Team team = player->GetTeam();
    AccountTypes security = player->GetSession()->GetSecurity();
    uint32 guid     = player->GetGUIDLow();
    AccountTypes gmLevelInWhoList = AccountTypes(sWorld.getConfig(CONFIG_UINT32_GM_LEVEL_IN_WHO_LIST));
    bool allowTwoSideWhoList = sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_WHO_LIST);

    for (SocialMap::const_iterator itr = m_socialMap.begin(); itr != m_socialMap.end(); ++itr)
    {
        PlayerSocialMap::const_iterator itr2 = itr->second.m_playerSocialMap.find(guid);
        if (itr2 != itr->second.m_playerSocialMap.end() && (itr2->second.Flags & SOCIAL_FLAG_FRIEND))
        {
            Player* pFriend = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));

            // PLAYER see his team only and PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
            // MODERATOR, GAME MASTER, ADMINISTRATOR can see all
            if (pFriend && pFriend->IsInWorld() &&
                    (pFriend->GetSession()->GetSecurity() > SEC_PLAYER ||
                     ((pFriend->GetTeam() == team || allowTwoSideWhoList) && security <= gmLevelInWhoList)) &&
                    player->IsVisibleGloballyFor(pFriend))
            {
                pFriend->GetSession()->SendPacket(packet);
            }
        }
    }
}

PlayerSocial* SocialMgr::LoadFromDB(std::unique_ptr<QueryResult> queryResult, ObjectGuid guid)
{
    PlayerSocial* social = &m_socialMap[guid.GetCounter()];
    social->SetPlayerGuid(guid);

    if (!queryResult)
        return social;

    // used to speed up check below. Using GetNumberOfSocialsWithFlag will cause unneeded iteration
    uint32 friendCounter = 0, ignoreCounter = 0;

    do
    {
        Field* fields  = queryResult->Fetch();

        uint32 friend_guid = fields[0].GetUInt32();
        uint32 flags = fields[1].GetUInt32();
        std::string note = fields[2].GetCppString();

        if ((flags & SOCIAL_FLAG_IGNORED) && ignoreCounter >= SOCIALMGR_IGNORE_LIMIT)
            continue;
        if ((flags & SOCIAL_FLAG_FRIEND) && friendCounter >= SOCIALMGR_FRIEND_LIMIT)
            continue;

        social->m_playerSocialMap[friend_guid] = FriendInfo(flags, note);

        if (flags & SOCIAL_FLAG_IGNORED)
            ++ignoreCounter;
        else
            ++friendCounter;
    }
    while (queryResult->NextRow());
    return social;
}
