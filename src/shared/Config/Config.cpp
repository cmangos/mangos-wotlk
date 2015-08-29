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

#include <fstream>

#include <boost/lexical_cast.hpp>

INSTANTIATE_SINGLETON_1(Config);

Config::Config()
{
}

Config::~Config()
{
}

bool Config::SetSource(std::string const& filename, std::string const& sectionname)
{
    m_Filename = filename;
    m_SectionName = sectionname;

    return Reload();
}

bool Config::Reload()
{
    std::ifstream file;
    file.open(m_Filename.c_str());
    if (!file.is_open())
        return false;

        // clear all old values
        m_values.clear();

        // we accept any options but check later used cases, so use empty
        boost::program_options::options_description description;

        boost::program_options::basic_parsed_options<char> options =
            boost::program_options::parse_config_file(file, description, true);

        // store all given options
        for (size_t i = 0; i < options.options.size(); ++i)
        {
            std::string option_name = options.options[i].string_key;

            // Skip positional options without name
            if (option_name.empty())
             continue;

            std::string original_token = options.options[i].original_tokens.size() ?
                options.options[i].original_tokens[1] : "";

            // remove ""
            // VC90 std::string missing front/back access functions
            if (original_token.size() > 1 && original_token[0] == '"' && original_token[original_token.size() - 1] == '"')
             original_token = original_token.substr(1, original_token.size() - 2);

            m_values[option_name] = original_token;
        }

    return true;
}

std::string const* Config::GetValue(const char* name) const
{
    std::string key = m_SectionName + "." + name;
    Values::const_iterator itr = m_values.find(key);
    return itr != m_values.end() ? &itr->second : nullptr;
}

std::string Config::GetStringDefault(const char* name, const char* def) const
{
    std::string const* val_ptr = GetValue(name);
    if (!val_ptr)
        return def;

    return *val_ptr;
}

bool Config::GetBoolDefault(const char* name, bool def) const
{
    std::string const* val_ptr = GetValue(name);
    if (!val_ptr)
        return def;

    std::string str = *val_ptr;
    return str == "true" || str == "TRUE" || str == "yes" || str == "YES" || str == "1";
}

int32 Config::GetIntDefault(const char* name, int32 def) const
{
    std::string const* val_ptr = GetValue(name);
    if (!val_ptr)
        return def;

    try
    {
        return boost::lexical_cast<int32>(*val_ptr);
    }
    catch (...)
    {
        return def;
    }

}

float Config::GetFloatDefault(const char* name, float def) const
{
    std::string const* val_ptr = GetValue(name);
    if (!val_ptr)
        return def;

    try
    {
        return boost::lexical_cast<float>(*val_ptr);
    }
    catch (...)
    {
        return def;
    }
}
