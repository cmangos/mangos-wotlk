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

#ifndef CONFIG_H
#define CONFIG_H

#include "Common.h"
#include <Policies/Singleton.h>
#include "Platform/Define.h"

#include "Utilities/UnorderedMapSet.h"

#include <boost/program_options.hpp>

class MANGOS_DLL_SPEC Config
{
    public:

        Config();
        ~Config();

        bool SetSource(std::string const& filename, std::string const& sectionname);
        bool Reload();

        std::string GetStringDefault(const char* name, const char* def) const;
        bool GetBoolDefault(const char* name, bool def = false) const;
        int32 GetIntDefault(const char* name, int32 def = 0) const;
        float GetFloatDefault(const char* name, float def = 0.0f) const;

        std::string GetFilename() const { return m_Filename; }

    private:

        std::string const* GetValue(const char* name) const;

        std::string m_Filename;
        std::string m_SectionName;
        typedef UNORDERED_MAP<std::string, std::string> Values;
        Values m_values;
};

#define sConfig MaNGOS::Singleton<Config>::Instance()

#endif
