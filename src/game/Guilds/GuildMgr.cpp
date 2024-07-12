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

#include "Guilds/GuildMgr.h"
#include "Guilds/Guild.h"
#include "Log/Log.h"
#include "Entities/ObjectGuid.h"
#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"
#include "Util/ProgressBar.h"
#include "World/World.h"

INSTANTIATE_SINGLETON_1(GuildMgr);

GuildMgr::GuildMgr()
{
}

GuildMgr::~GuildMgr()
{
}

void GuildMgr::AddGuild(Guild* guild)
{
    MaNGOS::unique_trackable_ptr<Guild>& ptr = m_GuildMap[guild->GetId()];
    ptr.reset(guild);
    guild->SetWeakPtr(ptr);
}

void GuildMgr::RemoveGuild(uint32 guildId)
{
    m_GuildMap.erase(guildId);
}

Guild* GuildMgr::GetGuildById(uint32 guildId) const
{
    GuildMap::const_iterator itr = m_GuildMap.find(guildId);
    if (itr != m_GuildMap.end())
        return itr->second.get();

    return nullptr;
}

Guild* GuildMgr::GetGuildByName(std::string const& name) const
{
    for (const auto& itr : m_GuildMap)
        if (itr.second->GetName() == name)
            return itr.second.get();

    return nullptr;
}

Guild* GuildMgr::GetGuildByLeader(ObjectGuid const& guid) const
{
    for (const auto& itr : m_GuildMap)
        if (itr.second->GetLeaderGuid() == guid)
            return itr.second.get();

    return nullptr;
}

std::string GuildMgr::GetGuildNameById(uint32 guildId) const
{
    GuildMap::const_iterator itr = m_GuildMap.find(guildId);
    if (itr != m_GuildMap.end())
        return itr->second->GetName();

    return "";
}

void GuildMgr::LoadGuilds()
{
    uint32 count = 0;

    //                                                 0             1          2          3           4           5           6
    auto queryResult = CharacterDatabase.Query("SELECT guild.guildid,guild.name,leaderguid,EmblemStyle,EmblemColor,BorderStyle,BorderColor,"
                          //   7               8    9    10         11        12
                          "BackgroundColor,info,motd,createdate,BankMoney,(SELECT COUNT(guild_bank_tab.guildid) FROM guild_bank_tab WHERE guild_bank_tab.guildid = guild.guildid) "
                          "FROM guild ORDER BY guildid ASC");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u guild definitions", count);
        sLog.outString();
        return;
    }

    // load guild ranks
    //                                                      0       1   2     3      4
    auto guildRanksResult = CharacterDatabase.Query("SELECT guildid,rid,rname,rights,BankMoneyPerDay FROM guild_rank ORDER BY guildid ASC, rid ASC");

    // load guild members
    //                                                        0       1                  2     3     4       5                  6
    auto guildMembersResult = CharacterDatabase.Query("SELECT guildid,guild_member.guid,`rank`,pnote,offnote,BankResetTimeMoney,BankRemMoney,"
                                      //   7                 8                9                 10               11                12
                                      "BankResetTimeTab0,BankRemSlotsTab0,BankResetTimeTab1,BankRemSlotsTab1,BankResetTimeTab2,BankRemSlotsTab2,"
                                      //   13                14               15                16               17                18
                                      "BankResetTimeTab3,BankRemSlotsTab3,BankResetTimeTab4,BankRemSlotsTab4,BankResetTimeTab5,BankRemSlotsTab5,"
                                      //   19               20                21                22                23               24                      25
                                      "characters.name, characters.level, characters.class, characters.gender, characters.zone, characters.logout_time, characters.account "
                                      "FROM guild_member LEFT JOIN characters ON characters.guid = guild_member.guid ORDER BY guildid ASC");

    // load guild bank tab rights
    //                                                              0       1     2   3       4
    auto guildBankTabRightsResult = CharacterDatabase.Query("SELECT guildid,TabId,rid,gbright,SlotPerDay FROM guild_bank_right ORDER BY guildid ASC, TabId ASC");

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        // Field *fields = queryResult->Fetch();

        bar.step();
        ++count;

        Guild* newGuild = new Guild;
        if (!newGuild->LoadGuildFromDB(queryResult.get()) ||
            !newGuild->LoadRanksFromDB(guildRanksResult.get()) ||
            !newGuild->LoadMembersFromDB(guildMembersResult.get()) ||
            !newGuild->LoadBankRightsFromDB(guildBankTabRightsResult.get()) ||
            !newGuild->CheckGuildStructure()
           )
        {
            newGuild->Disband();
            delete newGuild;
            continue;
        }

        newGuild->LoadGuildEventLogFromDB();
        newGuild->LoadGuildBankEventLogFromDB();
        newGuild->LoadGuildBankFromDB();
        AddGuild(newGuild);
    }
    while (queryResult->NextRow());

    // delete unused LogGuid records in guild_eventlog and guild_bank_eventlog table
    // you can comment these lines if you don't plan to change CONFIG_UINT32_GUILD_EVENT_LOG_COUNT and CONFIG_UINT32_GUILD_BANK_EVENT_LOG_COUNT
    CharacterDatabase.PExecute("DELETE FROM guild_eventlog WHERE LogGuid > '%u'", sWorld.getConfig(CONFIG_UINT32_GUILD_EVENT_LOG_COUNT));
    CharacterDatabase.PExecute("DELETE FROM guild_bank_eventlog WHERE LogGuid > '%u'", sWorld.getConfig(CONFIG_UINT32_GUILD_BANK_EVENT_LOG_COUNT));

    sLog.outString(">> Loaded %u guild definitions", count);
    sLog.outString();
}
