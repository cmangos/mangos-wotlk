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

#ifndef REALM_LIST_H
#define REALM_LIST_H

#include "Common.h"

struct RealmBuildInfo
{
    int build;
    int major_version;
    int minor_version;
    int bugfix_version;
    int hotfix_version;
};

RealmBuildInfo const* FindBuildInfo(uint16 _build);

typedef std::set<uint32> RealmBuilds;

/// Storage object for a realm
struct Realm
{
    std::string address;
    uint8 icon;
    RealmFlags realmflags;
    uint8 timezone;
    uint32 m_ID;
    AccountTypes allowedSecurityLevel; // Current allowed join security level (show as locked for not fit accounts)
    float populationLevel;
    RealmBuilds realmbuilds;           // List of supported builds (updated in DB by mangosd)
    RealmBuildInfo realmBuildInfo;     // Build info for show version in list
};

/// Storage object for the list of realms on the server
class RealmList
{
public:
    typedef std::map<std::string, Realm> RealmMap;

    static RealmList& Instance();

    RealmList();
    ~RealmList() {}

    void Initialize(uint32 update_interval);
    void UpdateIfNeed();

    RealmMap::const_iterator begin() const { return realms_.begin(); }
    RealmMap::const_iterator end() const { return realms_.end(); }
    uint32 size() const { return realms_.size(); }

private:
    void UpdateRealms(bool init);
    void UpdateRealm(uint32 ID, const std::string& name, const std::string& address, uint32 port, uint8 icon, RealmFlags realm_flags,
        uint8 timezone, AccountTypes allowed_security_level, float population, const std::string& builds);

    // Internal map of realms
    RealmMap realms_;

    uint32 update_interval_;
    time_t next_update_time_;
};

#define sRealmList RealmList::Instance()

#endif // REALM_LIST_H