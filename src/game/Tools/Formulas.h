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

#ifndef MANGOS_FORMULAS_H
#define MANGOS_FORMULAS_H

#include "World/World.h"

namespace MaNGOS
{
    namespace Honor
    {
        inline float hk_honor_at_level(uint32 level, uint32 count = 1)
        {
            return (float)ceil(count * (-0.53177f + 0.59357f * exp((level + 23.54042f) / 26.07859f)));
        }
    }
    namespace XP
    {
        inline bool IsTrivialLevelDifference(uint32 unitLvl, uint32 targetLvl)
        {
            if (unitLvl > targetLvl)
            {
                const uint32 diff = (unitLvl - targetLvl);
                switch (unitLvl / 5)
                {
                    case 0:     // 0-4
                    case 1:     // 5-9
                        return (diff > 4);
                    case 2:     // 10-14
                    case 3:     // 15-19
                        return (diff > 5);
                    case 4:     // 20-24
                    case 5:     // 25-29
                        return (diff > 6);
                    case 6:     // 30-34
                    case 7:     // 35-39
                        return (diff > 7);
                    default:    // 40+
                        return (diff > 8);
                }
            }
            return false;
        }

        enum XPColorChar { RED, ORANGE, YELLOW, GREEN, GRAY };

        inline uint32 GetGrayLevel(uint32 pl_level)
        {
            if (pl_level <= 5)
                return 0;
            if (pl_level <= 39)
                return pl_level - 5 - pl_level / 10;
            if (pl_level <= 59)
                return pl_level - 1 - pl_level / 5;
            return pl_level - 9;
        }

        inline XPColorChar GetColorCode(uint32 pl_level, uint32 mob_level)
        {
            if (mob_level >= pl_level + 5)
                return RED;
            if (mob_level >= pl_level + 3)
                return ORANGE;
            if (mob_level >= pl_level - 2)
                return YELLOW;
            if (mob_level > GetGrayLevel(pl_level))
                return GREEN;
            return GRAY;
        }

        inline uint32 GetZeroDifference(uint32 unit_level)
        {
            if (unit_level < 8)  return 5;
            if (unit_level < 10) return 6;
            if (unit_level < 12) return 7;
            if (unit_level < 16) return 8;
            if (unit_level < 20) return 9;
            if (unit_level < 30) return 11;
            if (unit_level < 40) return 12;
            if (unit_level < 45) return 13;
            if (unit_level < 50) return 14;
            if (unit_level < 55) return 15;
            if (unit_level < 60) return 16;
            return 17;
        }

        inline uint32 BaseGain(uint32 unit_level, uint32 mob_level, ContentLevels content)
        {
            uint32 nBaseExp = unit_level * 5;
            switch (content)
            {
                case CONTENT_1_60:  nBaseExp += 45;  break;
                case CONTENT_61_70: nBaseExp += 235; break;
                case CONTENT_71_80: nBaseExp += 580; break;
                default:
                    sLog.outError("BaseGain: Unsupported content level %u", content);
                    nBaseExp += 45;  break;
            }

            if (mob_level >= unit_level)
            {
                uint32 nLevelDiff = mob_level - unit_level;
                if (nLevelDiff > 4)
                    nLevelDiff = 4;
                return nBaseExp * (1.0f + (0.05f * nLevelDiff));
            }

            if (!IsTrivialLevelDifference(unit_level, mob_level))
            {
                uint32 ZD = GetZeroDifference(unit_level);
                uint32 nLevelDiff = unit_level - mob_level;
                return nBaseExp * (1.0f - (float(nLevelDiff) / ZD));
            }
            return 0;
        }

        inline uint32 Gain(Unit const* unit, Creature* target)
        {
            if (target->IsTotem() || target->IsPet() || target->IsNoXp() || target->IsCritter())
                return 0;

            uint32 xp_gain = BaseGain(unit->GetLevel(), target->GetLevel(), GetContentLevelsForMapAndZone(unit->GetMapId(), unit->GetZoneId()));
            if (xp_gain == 0.0f)
                return 0;

            if (target->IsElite())
            {
                if (target->GetMap()->IsNonRaidDungeon())
                    xp_gain *= 2.5f;
                else
                    xp_gain *= 2.0f;
            }

            xp_gain *= target->GetCreatureInfo()->ExperienceMultiplier;

            xp_gain = target->GetModifierXpBasedOnDamageReceived(xp_gain);

            return (uint32)(std::nearbyint(xp_gain * sWorld.getConfig(CONFIG_FLOAT_RATE_XP_KILL)));
        }

        inline float xp_in_group_rate(uint32 count, bool /*isRaid*/)
        {
            // TODO: this formula is completely guesswork only based on a logical assumption
            switch (count)
            {
                case 0:
                case 1:
                case 2:
                    return 1.0f;
                case 3:
                    return 1.166f;
                case 4:
                    return 1.3f;
                case 5:
                    return 1.4f;
                default:
                    return std::max(1.f - count * 0.05f, 0.01f);
            }
        }
    }
}
#endif
