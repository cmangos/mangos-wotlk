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

#include "Config.h"
#include "Policies/Singleton.h"
#include "Util/Util.h"
#include <mutex>

#include <boost/algorithm/string.hpp>

#include <unordered_map>
#include <string>
#include <fstream>
#include <cstdlib>

INSTANTIATE_SINGLETON_1(Config);

std::optional<std::string> EnvVarForIniKey(std::string const& key);

bool Config::SetSource(const std::string& file)
{
    m_filename = file;

    return Reload(true);
}

bool Config::Reload(bool isNewSource)
{
    std::ifstream in(m_filename, std::ifstream::in);

    if (in.fail())
        return false;

    std::unordered_map<std::string, std::string> newEntries;
    std::unordered_map<std::string, std::string> normalizedToOriginalKeysMap;
    std::lock_guard<std::mutex> guard(m_configLock);

    do
    {
        std::string line;
        std::getline(in, line);

        boost::algorithm::trim_left(line);

        if (!line.length())
            continue;

        if (line[0] == '#' || line[0] == '[')
            continue;

        auto const equals = line.find('=');
        if (equals == std::string::npos)
            return false;

        auto const entryRaw = line.substr(0, equals);
        auto const entry = boost::algorithm::trim_copy(boost::algorithm::to_lower_copy(entryRaw));
        auto const value = boost::algorithm::trim_copy_if(boost::algorithm::trim_copy(line.substr(equals + 1)), boost::algorithm::is_any_of("\""));

        normalizedToOriginalKeysMap[entry] = boost::algorithm::trim_copy(entryRaw);
        newEntries[entry] = value;
    }
    while (in.good());

    m_entries = std::move(newEntries);
    m_normalizedToOriginalKeysMap = std::move(normalizedToOriginalKeysMap);

    if (!isNewSource)
        OverrideWithEnvVariablesIfAny();
    
    return true;
}

std::vector<std::string> Config::OverrideWithEnvVariablesIfAny()
{
    std::vector<std::string> overriddenKeys;

    for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
    {
        if (itr->first.empty())
            continue;

        std::string originalKey = m_normalizedToOriginalKeysMap[itr->first];
        std::optional<std::string> envVar = EnvVarForIniKey(originalKey);
        if (!envVar)
            continue;

        itr->second = *envVar;

        overriddenKeys.push_back(originalKey);
    }

    return overriddenKeys;
}

bool Config::IsSet(const std::string& name) const
{
    auto const nameLower = boost::algorithm::to_lower_copy(name);
    return m_entries.find(nameLower) != m_entries.cend();
}

const std::string Config::GetStringDefault(const std::string& name, const std::string& def) const
{
    auto const nameLower = boost::algorithm::to_lower_copy(name);

    auto const entry = m_entries.find(nameLower);
    
    if (entry == m_entries.cend())
    {
        std::optional<std::string> envVar = EnvVarForIniKey(name);
        if (envVar)
        {
            sLog.outString("Missing key '%s' in config file '%s', recovered with environment '%s' value.", name.c_str(), m_filename.c_str(), envVar->c_str());

            return *envVar;
        }

        return def;
    }

    return entry->second;
}

bool Config::GetBoolDefault(const std::string& name, bool def) const
{
    auto const value = GetStringDefault(name, def ? "true" : "false");

    std::string valueLower;
    std::transform(value.cbegin(), value.cend(), std::back_inserter(valueLower), ::tolower);

    return valueLower == "true" || valueLower == "1" || valueLower == "yes";
}

int32 Config::GetIntDefault(const std::string& name, int32 def) const
{
    auto const value = GetStringDefault(name, std::to_string(def));

    return std::stoi(value);
}

float Config::GetFloatDefault(const std::string& name, float def) const
{
    auto const value = GetStringDefault(name, std::to_string(def));

    return std::stof(value);
}

// Converts ini keys to the environment variable key (upper snake case).
// Example of conversions:
//   SomeConfig => SOME_CONFIG
//   myNestedConfig.opt1 => MY_NESTED_CONFIG_OPT_1
//   LogDB.Opt.ClearTime => LOG_DB_OPT_CLEAR_TIME
std::string IniKeyToEnvVarKey(std::string const& key)
{
    std::string result;

    const char* str = key.c_str();
    size_t n = key.length();

    char curr;
    bool isEnd;
    bool nextIsUpper;
    bool currIsNumeric;
    bool nextIsNumeric;

    for (size_t i = 0; i < n; ++i)
    {
        curr = str[i];
        if (curr == ' ' || curr == '.' || curr == '-')
        {
            result += '_';
            continue;
        }

        isEnd = i == n - 1;
        if (!isEnd)
        {
            nextIsUpper = isupper(str[i + 1]);

            // handle "aB" to "A_B"
            if (!isupper(curr) && nextIsUpper)
            {
                result += static_cast<char>(std::toupper(curr));
                result += '_';
                continue;
            }

            currIsNumeric = isNumeric(curr);
            nextIsNumeric = isNumeric(str[i + 1]);

            // handle "a1" to "a_1"
            if (!currIsNumeric && nextIsNumeric)
            {
                result += static_cast<char>(std::toupper(curr));
                result += '_';
                continue;
            }

            // handle "1a" to "1_a"
            if (currIsNumeric && !nextIsNumeric)
            {
                result += static_cast<char>(std::toupper(curr));
                result += '_';
                continue;
            }
        }

        result += static_cast<char>(std::toupper(curr));
    }
    return result;
}

std::optional<std::string> EnvVarForIniKey(std::string const& key)
{
    std::string envKey = "CM_" + IniKeyToEnvVarKey(key);
    char* val = std::getenv(envKey.c_str());
    if (!val)
        return std::nullopt;

    return std::string(val);
}
