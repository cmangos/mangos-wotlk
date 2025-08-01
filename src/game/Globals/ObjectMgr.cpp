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

#include "Globals/ObjectMgr.h"
#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"

#include "Server/SQLStorages.h"
#include "Log/Log.h"
#include "Maps/MapManager.h"
#include "Entities/ObjectGuid.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Spells/SpellMgr.h"
#include "World/World.h"
#include "Groups/Group.h"
#include "Arena/ArenaTeam.h"
#include "Util/ProgressBar.h"
#include "Tools/Language.h"
#include "Pools/PoolManager.h"
#include "GameEvents/GameEventMgr.h"
#include "Chat/Chat.h"
#include "Maps/MapPersistentStateMgr.h"
#include "Spells/SpellAuras.h"
#include "Util/Util.h"
#include "Entities/GossipDef.h"
#include "Mails/Mail.h"
#include "Maps/InstanceData.h"
#include "Grids/GridNotifiers.h"
#include "Grids/GridNotifiersImpl.h"
#include "Grids/CellImpl.h"
#include "Server/DBCStores.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "World/WorldState.h"
#include "MotionGenerators/MoveMap.h"

#include "Entities/ItemEnchantmentMgr.h"
#include "Loot/LootMgr.h"

#include "Globals/UnitCondition.h"
#include "Globals/CombatCondition.h"
#include "World/WorldStateExpression.h"

#include <limits>
#include <cstdarg>
#include <cstring>

INSTANTIATE_SINGLETON_1(ObjectMgr);

bool normalizePlayerName(std::string& name, size_t max_len)
{
    if (name.empty())
        return false;

    std::wstring wstr_buf;
    if (!Utf8toWStr(name, wstr_buf))
        return false;

    size_t len = wstr_buf.size();
    if (len > max_len)
        return false;

    wstr_buf[0] = wcharToUpper(wstr_buf[0]);
    for (size_t i = 1; i < len; ++i)
        wstr_buf[i] = wcharToLower(wstr_buf[i]);

    return WStrToUtf8(wstr_buf, name);
}

LanguageDesc lang_description[LANGUAGES_COUNT] =
{
    { LANG_ADDON,           0, 0                       },
    { LANG_UNIVERSAL,       0, 0                       },
    { LANG_ORCISH,        669, SKILL_LANG_ORCISH       },
    { LANG_DARNASSIAN,    671, SKILL_LANG_DARNASSIAN   },
    { LANG_TAURAHE,       670, SKILL_LANG_TAURAHE      },
    { LANG_DWARVISH,      672, SKILL_LANG_DWARVEN      },
    { LANG_COMMON,        668, SKILL_LANG_COMMON       },
    { LANG_DEMONIC,       815, SKILL_LANG_DEMON_TONGUE },
    { LANG_TITAN,         816, SKILL_LANG_TITAN        },
    { LANG_THALASSIAN,    813, SKILL_LANG_THALASSIAN   },
    { LANG_DRACONIC,      814, SKILL_LANG_DRACONIC     },
    { LANG_KALIMAG,       817, SKILL_LANG_OLD_TONGUE   },
    { LANG_GNOMISH,      7340, SKILL_LANG_GNOMISH      },
    { LANG_TROLL,        7341, SKILL_LANG_TROLL        },
    { LANG_GUTTERSPEAK, 17737, SKILL_LANG_GUTTERSPEAK  },
    { LANG_DRAENEI,     29932, SKILL_LANG_DRAENEI      },
    { LANG_ZOMBIE,          0, 0                       },
    { LANG_GNOMISH_BINARY,  0, 0                       },
    { LANG_GOBLIN_BINARY,   0, 0                       }
};

LanguageDesc const* GetLanguageDescByID(uint32 lang)
{
    for (auto& i : lang_description)
    {
        if (uint32(i.lang_id) == lang)
            return &i;
    }

    return nullptr;
}

bool SpellClickInfo::IsFitToRequirements(Player const* player, Creature const* clickedCreature) const
{
    if (conditionId)
        return sObjectMgr.IsConditionSatisfied(conditionId, player, player->GetMap(), clickedCreature, CONDITION_FROM_SPELLCLICK);

    if (questStart)
    {
        // not in expected required quest state
        if (!player || ((!questStartCanActive || !player->IsActiveQuest(questStart)) && !player->GetQuestRewardStatus(questStart)))
            return false;
    }

    if (questEnd)
    {
        // not in expected forbidden quest state
        if (!player || player->GetQuestRewardStatus(questEnd))
            return false;
    }

    return true;
}

bool SpellClickInfo::HasConditionalSpellClick() const
{
    return conditionId || questStart || questEnd;
}

template<typename T>
T IdGenerator<T>::Generate()
{
    if (m_nextGuid >= std::numeric_limits<T>::max() - 1)
    {
        sLog.outError("%s guid overflow!! Can't continue, shutting down server. ", m_name);
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_nextGuid++;
}

template uint32 IdGenerator<uint32>::Generate();
template uint64 IdGenerator<uint64>::Generate();

ObjectMgr::ObjectMgr() :
    m_Dbc2StorageLocaleIndex(DEFAULT_LOCALE),
    m_ArenaTeamIds("Arena team ids"),
    m_AuctionIds("Auction ids"),
    m_EquipmentSetIds("Equipment set ids"),
    m_GuildIds("Guild ids"),
    m_MailIds("Mail ids"),
    m_PetNumbers("Pet numbers"),
    m_FirstTemporaryCreatureGuid(1),
    m_FirstTemporaryGameObjectGuid(1),
    m_unitConditionMgr(std::make_unique<UnitConditionMgr>()),
    m_worldStateExpressionMgr(std::make_unique<WorldStateExpressionMgr>()),
    m_combatConditionMgr(std::make_unique<CombatConditionMgr>(*m_unitConditionMgr, *m_worldStateExpressionMgr)),
    m_maxGoDbGuid(0),
    m_maxCreatureDbGuid(0)
{
}

ObjectMgr::~ObjectMgr()
{
    for (auto& i : petInfo)
        delete[] i.second;

    // free only if loaded
    for (auto& class_ : playerClassInfo)
        delete[] class_.levelInfo;

    for (auto& race : playerInfo)
        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            delete[] race[class_].levelInfo;

    // free objects
    for (auto& itr : mGroupMap)
        delete itr.second;

    for (auto& itr : mArenaTeamMap)
        delete itr.second;

    for (auto& itr : m_mCacheVendorTemplateItemMap)
        itr.second.Clear();

    for (auto& itr : m_mCacheVendorItemMap)
        itr.second.Clear();

    for (auto& itr : m_mCacheTrainerSpellMap)
        itr.second.Clear();
}

Group* ObjectMgr::GetGroupById(uint32 id) const
{
    GroupMap::const_iterator itr = mGroupMap.find(id);
    if (itr != mGroupMap.end())
        return itr->second;

    return nullptr;
}

ArenaTeam* ObjectMgr::GetArenaTeamById(uint32 arenateamid) const
{
    ArenaTeamMap::const_iterator itr = mArenaTeamMap.find(arenateamid);
    if (itr != mArenaTeamMap.end())
        return itr->second;

    return nullptr;
}

bool ichar_equals(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
}

ArenaTeam* ObjectMgr::GetArenaTeamByName(const std::string& arenateamname) const
{
    for (const auto& itr : mArenaTeamMap)
    {
        std::string const& teamName = itr.second->GetName();
        if (std::equal(teamName.begin(), teamName.end(), arenateamname.begin(), arenateamname.end(), ichar_equals))
            return itr.second;
    }            

    return nullptr;
}

ArenaTeam* ObjectMgr::GetArenaTeamByCaptain(ObjectGuid guid) const
{
    for (const auto& itr : mArenaTeamMap)
        if (itr.second->GetCaptainGuid() == guid)
            return itr.second;

    return nullptr;
}

void ObjectMgr::LoadCreatureLocales()
{
    mCreatureLocaleMap.clear();                             // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,name_loc1,subname_loc1,name_loc2,subname_loc2,name_loc3,subname_loc3,name_loc4,subname_loc4,name_loc5,subname_loc5,name_loc6,subname_loc6,name_loc7,subname_loc7,name_loc8,subname_loc8 FROM locales_creature");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 creature locale strings. DB table `locales_creature` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetCreatureTemplate(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_creature` has data for not existed creature entry %u, skipped.", entry);
            continue;
        }

        CreatureLocale& data = mCreatureLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.SubName.size() <= idx)
                        data.SubName.resize(idx + 1);

                    data.SubName[idx] = str;
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " creature locale strings", mCreatureLocaleMap.size());
    sLog.outString();
}

void ObjectMgr::LoadGossipMenuItemsLocales()
{
    mGossipMenuItemsLocaleMap.clear();                      // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT menu_id,id,"
                          "option_text_loc1,box_text_loc1,option_text_loc2,box_text_loc2,"
                          "option_text_loc3,box_text_loc3,option_text_loc4,box_text_loc4,"
                          "option_text_loc5,box_text_loc5,option_text_loc6,box_text_loc6,"
                          "option_text_loc7,box_text_loc7,option_text_loc8,box_text_loc8 "
                          "FROM locales_gossip_menu_option");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 gossip_menu_option locale strings. DB table `locales_gossip_menu_option` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint16 menuId   = fields[0].GetUInt16();
        uint16 id       = fields[1].GetUInt16();

        GossipMenuItemsMapBounds bounds = GetGossipMenuItemsMapBounds(menuId);

        bool found = false;
        if (bounds.first != bounds.second)
        {
            for (GossipMenuItemsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                if (itr->second.id == id)
                {
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            ERROR_DB_STRICT_LOG("Table `locales_gossip_menu_option` has data for nonexistent gossip menu %u item %u, skipped.", menuId, id);
            continue;
        }

        GossipMenuItemsLocale& data = mGossipMenuItemsLocaleMap[MAKE_PAIR32(menuId, id)];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[2 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.OptionText.size() <= idx)
                        data.OptionText.resize(idx + 1);

                    data.OptionText[idx] = str;
                }
            }
            str = fields[2 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.BoxText.size() <= idx)
                        data.BoxText.resize(idx + 1);

                    data.BoxText[idx] = str;
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " gossip_menu_option locale strings", mGossipMenuItemsLocaleMap.size());
    sLog.outString();
}

void ObjectMgr::LoadPointOfInterestLocales()
{
    mPointOfInterestLocaleMap.clear();                      // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,icon_name_loc1,icon_name_loc2,icon_name_loc3,icon_name_loc4,icon_name_loc5,icon_name_loc6,icon_name_loc7,icon_name_loc8 FROM locales_points_of_interest");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 points_of_interest locale strings. DB table `locales_points_of_interest` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetPointOfInterest(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_points_of_interest` has data for nonexistent POI entry %u, skipped.", entry);
            continue;
        }

        PointOfInterestLocale& data = mPointOfInterestLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (str.empty())
                continue;

            int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
            if (idx >= 0)
            {
                if ((int32)data.IconName.size() <= idx)
                    data.IconName.resize(idx + 1);

                data.IconName[idx] = str;
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " points_of_interest locale strings", mPointOfInterestLocaleMap.size());
    sLog.outString();
}

struct SQLCreatureLoader : public SQLStorageLoaderBase<SQLCreatureLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptDevAIMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadCreatureTemplates()
{
    SQLCreatureLoader loader;
    loader.Load(sCreatureStorage);

    std::set<uint32> difficultyEntries[MAX_DIFFICULTY - 1]; // already loaded difficulty 1 value in creatures
    std::set<uint32> hasDifficultyEntries[MAX_DIFFICULTY - 1]; // already loaded creatures with difficulty 1  values

    // check data correctness
    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i);
        if (!cInfo)
            continue;

        bool ok = true;                                     // bool to allow continue outside this loop
        for (uint32 diff = 0; diff < MAX_DIFFICULTY - 1 && ok; ++diff)
        {
            if (!cInfo->DifficultyEntry[diff])
                continue;
            ok = false;                                     // will be set to true at the end of this loop again

            CreatureInfo const* difficultyInfo = GetCreatureTemplate(cInfo->DifficultyEntry[diff]);
            if (!difficultyInfo)
            {
                sLog.outErrorDb("Creature (Entry: %u) have `DifficultyEntry%u`=%u but creature entry %u not exist.",
                                i, diff + 1, cInfo->DifficultyEntry[diff], cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (difficultyEntries[diff].find(i) != difficultyEntries[diff].end())
            {
                sLog.outErrorDb("Creature (Entry: %u) listed as difficulty %u but have value in `DifficultyEntry%u`.", i, diff + 1, diff + 1);
                continue;
            }

            bool ok2 = true;
            for (uint32 diff2 = 0; diff2 < MAX_DIFFICULTY - 1 && ok2; ++diff2)
            {
                ok2 = false;
                if (difficultyEntries[diff2].find(cInfo->DifficultyEntry[diff]) != difficultyEntries[diff2].end())
                {
                    sLog.outErrorDb("Creature (Entry: %u) already listed as difficulty %u for another entry.", cInfo->DifficultyEntry[diff], diff2 + 1);
                    continue;
                }

                if (hasDifficultyEntries[diff2].find(cInfo->DifficultyEntry[diff]) != hasDifficultyEntries[diff2].end())
                {
                    sLog.outErrorDb("Creature (Entry: %u) have `DifficultyEntry%u`=%u but creature entry %u have difficulty %u entry also.",
                                    i, diff + 1, cInfo->DifficultyEntry[diff], cInfo->DifficultyEntry[diff], diff2 + 1);
                    continue;
                }
                ok2 = true;
            }
            if (!ok2)
                continue;

            if (cInfo->UnitClass != difficultyInfo->UnitClass)
            {
                sLog.outErrorDb("Creature (Entry: %u, class %u) has different `UnitClass` in difficulty %u mode (Entry: %u, class %u).",
                                i, cInfo->UnitClass, diff + 1, cInfo->DifficultyEntry[diff], difficultyInfo->UnitClass);
                continue;
            }

            if (cInfo->NpcFlags != difficultyInfo->NpcFlags)
            {
                sLog.outErrorDb("Creature (Entry: %u) has different `NpcFlags` in difficulty %u mode (Entry: %u).", i, diff + 1, cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (cInfo->TrainerClass != difficultyInfo->TrainerClass)
            {
                sLog.outErrorDb("Creature (Entry: %u) has different `TrainerClass` in difficulty %u mode (Entry: %u).", i, diff + 1, cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (cInfo->TrainerRace != difficultyInfo->TrainerRace)
            {
                sLog.outErrorDb("Creature (Entry: %u) has different `TrainerRace` in difficulty %u mode (Entry: %u).", i, diff + 1, cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (cInfo->TrainerType != difficultyInfo->TrainerType)
            {
                sLog.outErrorDb("Creature (Entry: %u) has different `TrainerType` in difficulty %u mode (Entry: %u).", i, diff + 1, cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (cInfo->TrainerSpell != difficultyInfo->TrainerSpell)
            {
                sLog.outErrorDb("Creature (Entry: %u) has different `TrainerSpell` in difficulty %u mode (Entry: %u).", i, diff + 1, cInfo->DifficultyEntry[diff]);
                continue;
            }

            if (difficultyInfo->AIName && *difficultyInfo->AIName)
            {
                sLog.outErrorDb("Difficulty %u mode creature (Entry: %u) has `AIName`, but in any case will used difficulty 0 mode creature (Entry: %u) AIName.",
                                diff + 1, cInfo->DifficultyEntry[diff], i);
                continue;
            }

            if (difficultyInfo->ScriptID)
            {
                sLog.outErrorDb("Difficulty %u mode creature (Entry: %u) has `ScriptName`, but in any case will used difficulty 0 mode creature (Entry: %u) ScriptName.",
                                diff + 1, cInfo->DifficultyEntry[diff], i);
                continue;
            }

            hasDifficultyEntries[diff].insert(i);
            difficultyEntries[diff].insert(cInfo->DifficultyEntry[diff]);
            ok = true;
        }
        if (!ok)
            continue;

        FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->Faction);
        if (!factionTemplate)
            sLog.outErrorDb("Creature (Entry: %u) has nonexistent Faction template (%u)", cInfo->Entry, cInfo->Faction);

        for (int k = 0; k < MAX_KILL_CREDIT; ++k)
        {
            if (cInfo->KillCredit[k])
            {
                if (!GetCreatureTemplate(cInfo->KillCredit[k]))
                {
                    sLog.outErrorDb("Creature (Entry: %u) has nonexistent creature entry in `KillCredit%d` (%u)", cInfo->Entry, k + 1, cInfo->KillCredit[k]);
                    const_cast<CreatureInfo*>(cInfo)->KillCredit[k] = 0;
                }
            }
        }

        // used later for scale
        CreatureDisplayInfoEntry const* displayScaleEntry = nullptr;

        for (int j = 0; j < MAX_CREATURE_MODEL; ++j)
        {
            if (cInfo->DisplayId[j])
            {
                CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->DisplayId[j]);
                if (!displayEntry)
                {
                    sLog.outErrorDb("Creature (Entry: %u) has nonexistent modelid_%d (%u), can crash client", cInfo->Entry, j + 1, cInfo->DisplayId[j]);
                    const_cast<CreatureInfo*>(cInfo)->DisplayId[j] = 0;
                }
                else if (!displayScaleEntry)
                    displayScaleEntry = displayEntry;

                CreatureModelInfo const* minfo = sCreatureModelStorage.LookupEntry<CreatureModelInfo>(cInfo->DisplayId[j]);
                if (!minfo)
                    sLog.outErrorDb("Creature (Entry: %u) are using modelid_%d (%u), but creature_model_info are missing for this model.", cInfo->Entry, j + 1, cInfo->DisplayId[j]);
            }
        }

        if (!displayScaleEntry)
            sLog.outErrorDb("Creature (Entry: %u) has nonexistent modelid in `ModelId1`/`ModelId2`/ModelId3`/`ModelId4`", cInfo->Entry);

        if (!cInfo->MinLevel)
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid `MinLevel`, set to 1", cInfo->Entry);
            const_cast<CreatureInfo*>(cInfo)->MinLevel = 1;
        }

        if (cInfo->MinLevel > cInfo->MaxLevel)
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid `MaxLevel`, using `MinLevel` value instead", cInfo->Entry);
            const_cast<CreatureInfo*>(cInfo)->MaxLevel = cInfo->MinLevel;
        }

        if (cInfo->MinLevel > DEFAULT_MAX_CREATURE_LEVEL)
        {
            sLog.outErrorDb("Creature (Entry: %u) `MinLevel` exceeds maximum allowed value of '%u'", cInfo->Entry, uint32(DEFAULT_MAX_CREATURE_LEVEL));
            const_cast<CreatureInfo*>(cInfo)->MinLevel = uint32(DEFAULT_MAX_CREATURE_LEVEL);
        }

        if (cInfo->MaxLevel > DEFAULT_MAX_CREATURE_LEVEL)
        {
            sLog.outErrorDb("Creature (Entry: %u) `MaxLevel` exceeds maximum allowed value of '%u'", cInfo->Entry, uint32(DEFAULT_MAX_CREATURE_LEVEL));
            const_cast<CreatureInfo*>(cInfo)->MaxLevel = uint32(DEFAULT_MAX_CREATURE_LEVEL);
        }

        if (cInfo->Expansion > MAX_EXPANSION)
        {
            sLog.outErrorDb("Creature (Entry: %u) `Expansion(%u)` is not correct", cInfo->Entry, uint32(MAX_EXPANSION));
            const_cast<CreatureInfo*>(cInfo)->Expansion = -1;
        }

        if (!cInfo->UnitClass || (((1 << (cInfo->UnitClass - 1)) & CLASSMASK_ALL_CREATURES) == 0))
        {
            ERROR_DB_STRICT_LOG("Creature (Entry: %u) does not have proper `UnitClass` (%u) in creature_template", cInfo->Entry, cInfo->UnitClass);
            // Mark NPC as having improper data by his expansion
            const_cast<CreatureInfo*>(cInfo)->Expansion = -1;
        }

        if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK) && cInfo->Expansion >= 0)  // TODO - Remove the DB_STRICTED_CHECK after a while
        {
            // check if ClassLevel data are available for all possible level of that creature
            for (uint32 level = cInfo->MinLevel; level <= cInfo->MaxLevel; ++level)
            {
                if (!GetCreatureClassLvlStats(level, cInfo->UnitClass, cInfo->Expansion))
                {
                    sLog.outErrorDb("Creature (Entry: %u), level(%u) has no data in `creature_template_classlevelstats`", cInfo->Entry, level);
                    // Deactivate using ClassLevelStats for this NPC
                    const_cast<CreatureInfo*>(cInfo)->Expansion = -1;
                }
            }
        }

        if (cInfo->DamageSchool >= MAX_SPELL_SCHOOL)
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid spell school value (%u) in `DamageSchool`", cInfo->Entry, cInfo->DamageSchool);
            const_cast<CreatureInfo*>(cInfo)->DamageSchool = SPELL_SCHOOL_NORMAL;
        }

        if (cInfo->MeleeBaseAttackTime == 0)
            const_cast<CreatureInfo*>(cInfo)->MeleeBaseAttackTime  = BASE_ATTACK_TIME;

        if (cInfo->RangedBaseAttackTime == 0)
            const_cast<CreatureInfo*>(cInfo)->RangedBaseAttackTime = BASE_ATTACK_TIME;

        if (cInfo->NpcFlags & UNIT_NPC_FLAG_SPELLCLICK)
        {
            sLog.outErrorDb("Creature (Entry: %u) has dynamic flag UNIT_NPC_FLAG_SPELLCLICK (%u) set, it expect to be set by code base at `npc_spellclick_spells` content.", cInfo->Entry, UNIT_NPC_FLAG_SPELLCLICK);
            const_cast<CreatureInfo*>(cInfo)->NpcFlags &= ~UNIT_NPC_FLAG_SPELLCLICK;
        }

        if ((cInfo->NpcFlags & UNIT_NPC_FLAG_TRAINER) && cInfo->TrainerType >= MAX_TRAINER_TYPE)
            sLog.outErrorDb("Creature (Entry: %u) has wrong `TrainerType(%u)`", cInfo->Entry, cInfo->TrainerType);

        if (cInfo->CreatureType && !sCreatureTypeStore.LookupEntry(cInfo->CreatureType))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid `CreatureType(%u)`", cInfo->Entry, cInfo->CreatureType);
            const_cast<CreatureInfo*>(cInfo)->CreatureType = CREATURE_TYPE_HUMANOID;
        }

        // must exist or used hidden but used in data horse case
        if (cInfo->Family && !sCreatureFamilyStore.LookupEntry(cInfo->Family) && cInfo->Family != CREATURE_FAMILY_HORSE_CUSTOM)
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid creature family (%u) in `Family`", cInfo->Entry, cInfo->Family);
            const_cast<CreatureInfo*>(cInfo)->Family = 0;
        }

        if (cInfo->InhabitType <= 0 || cInfo->InhabitType > INHABIT_ANYWHERE)
        {
            sLog.outErrorDb("Creature (Entry: %u) has wrong value (%u) in `InhabitType`, creature will not correctly walk/swim/fly", cInfo->Entry, cInfo->InhabitType);
            const_cast<CreatureInfo*>(cInfo)->InhabitType = INHABIT_ANYWHERE;
        }

        if (cInfo->PetSpellDataId)
        {
            CreatureSpellDataEntry const* spellDataId = sCreatureSpellDataStore.LookupEntry(cInfo->PetSpellDataId);
            if (!spellDataId)
                sLog.outErrorDb("Creature (Entry: %u) has non-existing `PetSpellDataId(%u)`", cInfo->Entry, cInfo->PetSpellDataId);
        }

        if (cInfo->MovementType >= MAX_DB_MOTION_TYPE)
        {
            sLog.outErrorDb("Creature (Entry: %u) has wrong movement generator type (%u) set in `MovementType`, ignore and set to IDLE.", cInfo->Entry, cInfo->MovementType);
            const_cast<CreatureInfo*>(cInfo)->MovementType = IDLE_MOTION_TYPE;
        }

        if (cInfo->VehicleTemplateId && !sVehicleStore.LookupEntry(cInfo->VehicleTemplateId))
        {
            sLog.outErrorDb("Creature (Entry: %u) has non-existing `VehicleTemplateId(%u)`, set to 0.", cInfo->Entry, cInfo->VehicleTemplateId);
            const_cast<CreatureInfo*>(cInfo)->VehicleTemplateId = 0;
        }

        if (cInfo->EquipmentTemplateId > 0)                         // 0 no equipment
        {
            if (!GetEquipmentInfo(cInfo->EquipmentTemplateId))
            {
                sLog.outErrorDb("Table `creature_template` have creature (Entry: %u) with `EquipmentTemplateId(%u)` not found in table `creature_equip_template`, set to no equipment.", cInfo->Entry, cInfo->EquipmentTemplateId);
                const_cast<CreatureInfo*>(cInfo)->EquipmentTemplateId = 0;
            }
        }

        if (cInfo->VendorTemplateId > 0)
        {
            if (!(cInfo->NpcFlags & UNIT_NPC_FLAG_VENDOR))
                sLog.outErrorDb("Table `creature_template` have creature (Entry: %u) with `VendorTemplateId(%u)` but not have flag UNIT_NPC_FLAG_VENDOR (%u), vendor items will ignored.", cInfo->Entry, cInfo->VendorTemplateId, UNIT_NPC_FLAG_VENDOR);
        }

        /// if not set custom creature scale then load scale from CreatureDisplayInfo.dbc
        if (cInfo->Scale <= 0.0f)
        {
            if (displayScaleEntry)
                const_cast<CreatureInfo*>(cInfo)->Scale = displayScaleEntry->scale;
            else
                const_cast<CreatureInfo*>(cInfo)->Scale = DEFAULT_OBJECT_SCALE;
        }

        if (cInfo->StringID1 && !sScriptMgr.ExistsStringId(cInfo->StringID1))
        {
            sLog.outErrorDb("Table creature_template entry %u StringId1 %u does not exist. Setting to 0.", cInfo->Entry, cInfo->StringID1);
            const_cast<CreatureInfo*>(cInfo)->StringID1 = 0;
        }

        if (cInfo->StringID2 && !sScriptMgr.ExistsStringId(cInfo->StringID2))
        {
            sLog.outErrorDb("Table creature_template entry %u StringID2 %u does not exist. Setting to 0.", cInfo->Entry, cInfo->StringID2);
            const_cast<CreatureInfo*>(cInfo)->StringID2 = 0;
        }
        if (cInfo->StaticFlags || cInfo->StaticFlags2 || cInfo->StaticFlags3 || cInfo->StaticFlags4)
            const_cast<CreatureInfo*>(cInfo)->TypeFlags = GetTypeFlagsFromStaticFlags(CreatureTypeFlags(cInfo->TypeFlags), cInfo->StaticFlags, cInfo->StaticFlags2, cInfo->StaticFlags3, cInfo->StaticFlags4);
    }

    sLog.outString(">> Loaded %u creature definitions", sCreatureStorage.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::ConvertCreatureAddonAuras(CreatureDataAddon* addon, char const* table, char const* guidEntryStr)
{
    // Now add the auras, format "spell1 spell2 ..."
    char* p;
    std::vector<int> val;
    char* s = p = (char*)reinterpret_cast<char const*>(addon->auras);
    if (p)
    {
        while (p[0] != 0)
        {
            ++p;
            if (p[0] == ' ')
            {
                val.push_back(atoi(s));
                s = ++p;
            }
        }
        if (p != s)
            val.push_back(atoi(s));

        // free char* loaded memory
        delete[](char*)reinterpret_cast<char const*>(addon->auras);
    }

    // empty list
    if (val.empty())
    {
        addon->auras = nullptr;
        return;
    }

    // replace by new structures array
    const_cast<uint32*&>(addon->auras) = new uint32[val.size() + 1];

    uint32 i = 0;
    for (int j : val)
    {
        uint32& cAura = const_cast<uint32&>(addon->auras[i]);
        cAura = uint32(j);

        SpellEntry const* AdditionalSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(cAura);
        if (!AdditionalSpellInfo)
        {
            sLog.outErrorDb("Creature (%s: %u) has wrong spell %u defined in `auras` field in `%s`.", guidEntryStr, addon->guidOrEntry, cAura, table);
            continue;
        }

        // Must be Aura, but also allow dummy/script effect spells, as they are used sometimes to select a random aura or similar
        if (!IsSpellAppliesAura(AdditionalSpellInfo) && !IsSpellHaveEffect(AdditionalSpellInfo, SPELL_EFFECT_DUMMY) && !IsSpellHaveEffect(AdditionalSpellInfo, SPELL_EFFECT_SCRIPT_EFFECT) && !IsSpellHaveEffect(AdditionalSpellInfo, SPELL_EFFECT_TRIGGER_SPELL))
        {
            sLog.outErrorDb("Creature (%s: %u) has spell %u defined in `auras` field in `%s, but spell doesn't apply an aura`.", guidEntryStr, addon->guidOrEntry, cAura, table);
            continue;
        }

        if (SpellCastTimesEntry const* spellCastTimeEntry = sSpellCastTimesStore.LookupEntry(AdditionalSpellInfo->CastingTimeIndex))
        {
            if (spellCastTimeEntry->CastTime > 0)
            {
                sLog.outErrorDb("Creature (%s: %u) has spell %u defined in `auras` field in `%s, but spell has cast time. Use it in AI instead.", guidEntryStr, addon->guidOrEntry, cAura, table);
                continue;
            }
        }

        // TODO: Remove LogFilter check after more research
        if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK) && !IsOnlySelfTargeting(AdditionalSpellInfo))
        {
            sLog.outErrorDb("Creature (%s: %u) has spell %u defined in `auras` field in `%s, but spell is no self-only spell`.", guidEntryStr, addon->guidOrEntry, cAura, table);
            continue;
        }

        if (IsSpellHaveAura(AdditionalSpellInfo, SPELL_AURA_CONTROL_VEHICLE))
        {
            sLog.outErrorDb("Creature (%s: %u) has spell %u defined in `auras` field in `%s, but vehicle control spells are not suitable for _addon.auras handling`.", guidEntryStr, addon->guidOrEntry, cAura, table);
            continue;
        }

        if (std::find(&addon->auras[0], &addon->auras[i], cAura) != &addon->auras[i])
        {
            sLog.outErrorDb("Creature (%s: %u) has duplicate spell %u defined in `auras` field in `%s`.", guidEntryStr, addon->guidOrEntry, cAura, table);
            continue;
        }

        ++i;
    }

    // fill terminator element (after last added)
    const_cast<uint32&>(addon->auras[i]) = 0;
}

void ObjectMgr::LoadCreatureAddons(SQLStorage& creatureaddons, char const* entryName, char const* comment)
{
    creatureaddons.Load();

    // check data correctness and convert 'auras'
    for (uint32 i = 1; i < creatureaddons.GetMaxEntry(); ++i)
    {
        CreatureDataAddon const* addon = creatureaddons.LookupEntry<CreatureDataAddon>(i);
        if (!addon)
            continue;

        if (addon->mount)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(addon->mount))
            {
                sLog.outErrorDb("Creature (%s %u) have invalid displayInfoId for mount (%u) defined in `%s`.", entryName, addon->guidOrEntry, addon->mount, creatureaddons.GetTableName());
                const_cast<CreatureDataAddon*>(addon)->mount = 0;
            }
        }

        if (addon->sheath_state > SHEATH_STATE_RANGED)
            sLog.outErrorDb("Creature (%s %u) has unknown sheath state (%u) defined in `%s`.", entryName, addon->guidOrEntry, addon->sheath_state, creatureaddons.GetTableName());

        if (!sEmotesStore.LookupEntry(addon->emote))
        {
            sLog.outErrorDb("Creature (%s %u) have invalid emote (%u) defined in `%s`.", entryName, addon->guidOrEntry, addon->emote, creatureaddons.GetTableName());
            const_cast<CreatureDataAddon*>(addon)->emote = 0;
        }

        ConvertCreatureAddonAuras(const_cast<CreatureDataAddon*>(addon), creatureaddons.GetTableName(), entryName);
    }

    sLog.outString(">> Loaded %u %s", creatureaddons.GetRecordCount(), comment);
}

void ObjectMgr::LoadCreatureAddons()
{
    LoadCreatureAddons(sCreatureInfoAddonStorage, "Entry", "creature template addons");

    // check entry ids
    for (uint32 i = 1; i < sCreatureInfoAddonStorage.GetMaxEntry(); ++i)
        if (CreatureDataAddon const* addon = sCreatureInfoAddonStorage.LookupEntry<CreatureDataAddon>(i))
            if (!sCreatureStorage.LookupEntry<CreatureInfo>(addon->guidOrEntry))
                sLog.outErrorDb("Creature (Entry: %u) does not exist but has a record in `%s`", addon->guidOrEntry, sCreatureInfoAddonStorage.GetTableName());

    LoadCreatureAddons(sCreatureDataAddonStorage, "GUID", "creature addons");

    // check entry ids
    for (uint32 i = 1; i < sCreatureDataAddonStorage.GetMaxEntry(); ++i)
        if (CreatureDataAddon const* addon = sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(i))
            if (mCreatureDataMap.find(addon->guidOrEntry) == mCreatureDataMap.end())
                sLog.outErrorDb("Creature (GUID: %u) does not exist but has a record in `creature_addon`", addon->guidOrEntry);
}

void ObjectMgr::LoadCreatureClassLvlStats()
{
    // initialize data array
    memset(&m_creatureClassLvlStats, 0, sizeof(m_creatureClassLvlStats));

    std::string queryStr = "SELECT Class, Level, BaseMana, BaseMeleeAttackPower, BaseRangedAttackPower, BaseArmor, Strength, Agility, Stamina, Intellect, Spirit";

    std::string expData;
    for (int i = 0; i <= MAX_EXPANSION; ++i)
    {
        std::ostringstream str;
        str << ", BaseHealthExp" << i << ", BaseDamageExp" << i;
        expData += str.str();
    }

    queryStr += expData + " FROM creature_template_classlevelstats ORDER BY Class, Level";

    auto queryResult = WorldDatabase.Query(queryStr.c_str());

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb("DB table `creature_template_classlevelstats` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());
    uint32 storedRow = 0;

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 creatureClass               = fields[0].GetUInt32();
        uint32 creatureLevel               = fields[1].GetUInt32();

        if (creatureLevel == 0 || creatureLevel > DEFAULT_MAX_CREATURE_LEVEL)
        {
            sLog.outErrorDb("Found stats for creature level [%u], incorrect level for this core. Skip!", creatureLevel);
            continue;
        }

        if (((1 << (creatureClass - 1)) & CLASSMASK_ALL_CREATURES) == 0)
        {
            sLog.outErrorDb("Found stats for creature class [%u], incorrect class for this core. Skip!", creatureClass);
            continue;
        }

        uint32  baseMana                   = fields[2].GetUInt32();
        float   baseMeleeAttackPower       = fields[3].GetFloat();
        float   baseRangedAttackPower      = fields[4].GetFloat();
        uint32  baseArmor                  = fields[5].GetUInt32();
        uint32  strength                   = fields[6].GetUInt32();
        uint32  agility                    = fields[7].GetUInt32();
        uint32  stamina                    = fields[8].GetUInt32();
        uint32  intellect                  = fields[9].GetUInt32();
        uint32  spirit                     = fields[10].GetUInt32();

        for (int i = 0; i <= MAX_EXPANSION; ++i)
        {
            CreatureClassLvlStats& cCLS = m_creatureClassLvlStats[creatureLevel][classToIndex[creatureClass]][i];

            cCLS.BaseMana                   = baseMana;
            cCLS.BaseMeleeAttackPower       = baseMeleeAttackPower;
            cCLS.BaseRangedAttackPower      = baseRangedAttackPower;
            cCLS.BaseArmor                  = baseArmor;
            cCLS.Strength                   = strength;
            cCLS.Agility                    = agility;
            cCLS.Stamina                    = stamina;
            cCLS.Intellect                  = intellect;
            cCLS.Spirit                     = spirit;

            cCLS.BaseHealth = fields[11 + (i * 2)].GetUInt32();
            cCLS.BaseDamage = fields[12 + (i * 2)].GetFloat();

            uint32 apCoeffStr = 2;
            uint32 apCoeffAgi = 0;
            switch (creatureClass)
            {
                case CLASS_ROGUE:
                    apCoeffStr = 1;
                    apCoeffAgi = 1;
                    break;
                case CLASS_MAGE:
                    apCoeffStr = 1;
                    break;
            }

            // should ensure old data does not need change (not wanting to recalculate to avoid losing data)
            // if any mistake is made, it will be in these formulae that make asumptions about the new calculations
            // AP, RAP, HP, Mana and armor should stay the same pre-change and post-change when using multipliers == 1
            // stamina seems to have scaling formula for npcs - so for now does not impact base health
            // cCLS.BaseHealth -= std::min(cCLS.BaseHealth, std::max(0u, (uint32)Unit::GetHealthBonusFromStamina(cCLS.Stamina)));
            cCLS.BaseMana -= std::min(cCLS.BaseMana, std::max(0u, (uint32)Unit::GetManaBonusFromIntellect(cCLS.Intellect)));
            cCLS.BaseMeleeAttackPower -= std::min(cCLS.BaseMeleeAttackPower, std::max(0.f, float(((std::max(cCLS.Strength, 10u) - 10) * apCoeffStr + (std::max(cCLS.Agility, 10u) - 10) * apCoeffAgi))));
            cCLS.BaseRangedAttackPower -= std::min(cCLS.BaseRangedAttackPower, std::max(0.f, float(cCLS.Agility >= 10 ? (cCLS.Agility - 10) : 0)));
            cCLS.BaseArmor -= std::min(cCLS.BaseArmor, std::max(0u, cCLS.Agility * 2));
        }
        ++storedRow;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Found %u creature stats definitions.", storedRow);
    sLog.outString();
}

CreatureClassLvlStats const* ObjectMgr::GetCreatureClassLvlStats(uint32 level, uint32 unitClass, int32 expansion) const
{
    if (expansion < 0)
        return nullptr;

    CreatureClassLvlStats const* cCLS = &m_creatureClassLvlStats[level][classToIndex[unitClass]][expansion];

    if ((cCLS->BaseHealth != 0 || cCLS->Stamina > 0) && cCLS->BaseDamage > 0.1f)
        return cCLS;

    return nullptr;
}

std::vector<std::pair<TypeID, uint32>> const& ObjectMgr::GetDbGuidsForTransport(uint32 mapId) const
{
    return (*m_guidsForMap.find(mapId)).second;
}

VehicleSeatParameters const* ObjectMgr::GetVehicleSeatParameters(uint32 seatEntry) const
{
    auto itr = m_seatParameters.find(seatEntry);
    if (itr == m_seatParameters.end())
        return nullptr;

    return &itr->second;
}

uint32 ObjectMgr::GetTypeFlagsFromStaticFlags(CreatureTypeFlags typeFlags, uint32 staticFlags1, uint32 staticFlags2, uint32 staticFlags3, uint32 staticFlags4) const
{
    if (staticFlags1 & uint32(CreatureStaticFlags::TAMEABLE))
        typeFlags |= CreatureTypeFlags::TAMEABLE;
    if (staticFlags1 & uint32(CreatureStaticFlags::BOSS_MOB))
        typeFlags |= CreatureTypeFlags::BOSS_MOB;
    if (staticFlags1 & uint32(CreatureStaticFlags::VISIBLE_TO_GHOSTS))
        typeFlags |= CreatureTypeFlags::VISIBLE_TO_GHOSTS;
    if (staticFlags1 & uint32(CreatureStaticFlags::NO_FACTION_TOOLTIP))
        typeFlags |= CreatureTypeFlags::NO_FACTION_TOOLTIP;
    if (staticFlags1 & uint32(CreatureStaticFlags::DO_NOT_PLAY_WOUND_ANIM))
        typeFlags |= CreatureTypeFlags::DO_NOT_PLAY_WOUND_ANIM;
    if (staticFlags1 & uint32(CreatureStaticFlags::MORE_AUDIBLE))
        typeFlags |= CreatureTypeFlags::MORE_AUDIBLE;
    if (staticFlags2 & uint32(CreatureStaticFlags2::SPELL_ATTACKABLE))
        typeFlags |= CreatureTypeFlags::SPELL_ATTACKABLE;
    if (staticFlags2 & uint32(CreatureStaticFlags2::INTERACT_WHILE_DEAD))
        typeFlags |= CreatureTypeFlags::INTERACT_WHILE_DEAD;
    if (staticFlags2 & uint32(CreatureStaticFlags2::SKIN_WITH_HERBALISM))
        typeFlags |= CreatureTypeFlags::SKIN_WITH_HERBALISM;
    if (staticFlags2 & uint32(CreatureStaticFlags2::SKIN_WITH_MINING))
        typeFlags |= CreatureTypeFlags::SKIN_WITH_MINING;
    if (staticFlags2 & uint32(CreatureStaticFlags2::ALLOW_MOUNTED_COMBAT))
        typeFlags |= CreatureTypeFlags::ALLOW_MOUNTED_COMBAT;
    if (staticFlags2 & uint32(CreatureStaticFlags2::NO_DEATH_MESSAGE))
        typeFlags |= CreatureTypeFlags::NO_DEATH_MESSAGE;
    if (staticFlags2 & uint32(CreatureStaticFlags2::CAN_ASSIST))
        typeFlags |= CreatureTypeFlags::CAN_ASSIST;
    if (staticFlags2 & uint32(CreatureStaticFlags2::NO_PET_BAR))
        typeFlags |= CreatureTypeFlags::NO_PET_BAR;
    if (staticFlags3 & uint32(CreatureStaticFlags3::MASK_UID))
        typeFlags |= CreatureTypeFlags::MASK_UID;
    if (staticFlags3 & uint32(CreatureStaticFlags3::SKIN_WITH_ENGINEERING))
        typeFlags |= CreatureTypeFlags::SKIN_WITH_ENGINEERING;
    if (staticFlags3 & uint32(CreatureStaticFlags3::TAMEABLE_EXOTIC))
        typeFlags |= CreatureTypeFlags::TAMEABLE_EXOTIC;
    if (staticFlags3 & uint32(CreatureStaticFlags3::NO_NAME_PLATE))
        typeFlags |= CreatureTypeFlags::NO_NAME_PLATE;
    if (staticFlags3 & uint32(CreatureStaticFlags3::USE_MODEL_COLLISION_SIZE))
        typeFlags |= CreatureTypeFlags::USE_MODEL_COLLISION_SIZE;
    if (staticFlags3 & uint32(CreatureStaticFlags3::ALLOW_INTERACTION_WHILE_IN_COMBAT))
        typeFlags |= CreatureTypeFlags::ALLOW_INTERACTION_WHILE_IN_COMBAT;
    if (staticFlags3 & uint32(CreatureStaticFlags3::COLLIDE_WITH_MISSILES))
        typeFlags |= CreatureTypeFlags::COLLIDE_WITH_MISSILES;
    if (staticFlags3 & uint32(CreatureStaticFlags3::DO_NOT_PLAY_MOUNTED_ANIMATIONS))
        typeFlags |= CreatureTypeFlags::DO_NOT_PLAY_MOUNTED_ANIMATIONS;
    if (staticFlags3 & uint32(CreatureStaticFlags3::LINK_ALL))
        typeFlags |= CreatureTypeFlags::LINK_ALL;
    if (staticFlags4 & uint32(CreatureStaticFlags4::INTERACT_ONLY_WITH_CREATOR))
        typeFlags |= CreatureTypeFlags::INTERACT_ONLY_WITH_CREATOR;
    if (staticFlags4 & uint32(CreatureStaticFlags4::DO_NOT_PLAY_UNIT_EVENT_SOUNDS))
        typeFlags |= CreatureTypeFlags::DO_NOT_PLAY_UNIT_EVENT_SOUNDS;
    if (staticFlags4 & uint32(CreatureStaticFlags4::HAS_NO_SHADOW_BLOB))
        typeFlags |= CreatureTypeFlags::HAS_NO_SHADOW_BLOB;
    if (staticFlags4 & uint32(CreatureStaticFlags4::TREAT_AS_RAID_UNIT_FOR_HELPFUL_SPELLS))
        typeFlags |= CreatureTypeFlags::TREAT_AS_RAID_UNIT_FOR_HELPFUL_SPELLS;
    if (staticFlags4 & uint32(CreatureStaticFlags4::FORCE_GOSSIP))
        typeFlags |= CreatureTypeFlags::FORCE_GOSSIP;
    if (staticFlags4 & uint32(CreatureStaticFlags4::DO_NOT_SHEATHE))
        typeFlags |= CreatureTypeFlags::DO_NOT_SHEATHE;
    if (staticFlags4 & uint32(CreatureStaticFlags4::DO_NOT_TARGET_ON_INTERACTION))
        typeFlags |= CreatureTypeFlags::DO_NOT_TARGET_ON_INTERACTION;
    if (staticFlags4 & uint32(CreatureStaticFlags4::DO_NOT_RENDER_OBJECT_NAME))
        typeFlags |= CreatureTypeFlags::DO_NOT_RENDER_OBJECT_NAME;
    if (staticFlags4 & uint32(CreatureStaticFlags4::QUEST_BOSS))
        typeFlags |= CreatureTypeFlags::QUEST_BOSS;
    return uint32(typeFlags);
}

bool ObjectMgr::IsSpellUsedInCondition(uint32 spellId) const
{
    return m_spellsUsedInSpellClickConditions.find(spellId) != m_spellsUsedInSpellClickConditions.end();
}

CreatureImmunityVector const* ObjectMgr::GetCreatureImmunitySet(uint32 entry, uint32 setId) const
{
    auto itr = m_creatureImmunities.find(entry);
    if (itr == m_creatureImmunities.end())
        return nullptr;

    auto& setIds = (*itr).second;
    auto setItr = setIds.find(setId);
    if (setItr == setIds.end())
        return nullptr;

    return &(*setItr).second;
}

CreatureSpellList* ObjectMgr::GetCreatureSpellList(uint32 Id) const
{
    auto itr = m_spellListContainer->spellLists.find(Id);
    if (itr == m_spellListContainer->spellLists.end())
        return nullptr;

    return &(*itr).second;
}

bool ObjectMgr::HasWorldStateName(int32 Id) const
{
    return m_worldStateNames.find(Id) != m_worldStateNames.end();
}

WorldStateName* ObjectMgr::GetWorldStateName(int32 Id)
{
    auto itr = m_worldStateNames.find(Id);
    if (itr == m_worldStateNames.end())
        return nullptr;

    return &(itr->second);
}

std::vector<uint32>* ObjectMgr::GetCreatureDynGuidForMap(uint32 mapId)
{
    auto itr = m_dynguidCreatureDbGuids.find(mapId);
    if (itr == m_dynguidCreatureDbGuids.end())
        return nullptr;

    return &(*itr).second;
}

std::vector<uint32>* ObjectMgr::GetGameObjectDynGuidForMap(uint32 mapId)
{
    auto itr = m_dynguidGameobjectDbGuids.find(mapId);
    if (itr == m_dynguidGameobjectDbGuids.end())
        return nullptr;

    return &(*itr).second;
}

void ObjectMgr::AddDynGuidForMap(uint32 mapId, std::pair<std::vector<uint32>, std::vector<uint32>> const& dbGuids)
{
    auto& data = m_dynguidCreatureDbGuids[mapId];
    for (uint32 creature : dbGuids.first)
        data.push_back(creature);
    auto& goData = m_dynguidGameobjectDbGuids[mapId];
    for (uint32 go : dbGuids.second)
        goData.push_back(go);
}

void ObjectMgr::RemoveDynGuidForMap(uint32 mapId, std::pair<std::vector<uint32>, std::vector<uint32>> const& dbGuids)
{
    auto& data = m_dynguidCreatureDbGuids[mapId];
    for (uint32 creature : dbGuids.first)
        data.erase(std::remove(data.begin(), data.end(), creature), data.end());
    auto& goData = m_dynguidGameobjectDbGuids[mapId];
    for (uint32 go : dbGuids.second)
        goData.erase(std::remove(goData.begin(), goData.end(), go), goData.end());
}

void ObjectMgr::LoadCreatureImmunities()
{
    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT Entry, SetId, Type, Value FROM creature_immunities");

    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();

            uint32 entry = fields[0].GetUInt32();
            if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
            {
                sLog.outErrorDb("LoadCreatureImmunities: Entry %u does not exist.", entry);
                continue;
            }
            uint32 setId = fields[1].GetUInt32();
            uint32 type = fields[2].GetUInt32();
            if (type >= MAX_SPELL_IMMUNITY)
            {
                sLog.outErrorDb("LoadCreatureImmunities: Invalid type %u.", type);
                continue;
            }
            uint32 value = fields[3].GetUInt32();
            m_creatureImmunities[entry][setId].push_back({ type, value });
            ++count;
        } while (queryResult->NextRow());
    }

    sLog.outString(">> Loaded %u creature_immunities definitions", count);
    sLog.outString();
}

std::shared_ptr<CreatureSpellListContainer> ObjectMgr::LoadCreatureSpellLists()
{
    std::shared_ptr<CreatureSpellListContainer> newContainer = std::make_shared<CreatureSpellListContainer>();
    uint32 count = 0;

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Id, Type, Param1, Param2, Param3, UnitCondition, Comments FROM creature_spell_targeting"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            CreatureSpellListTargeting target;
            target.Id = fields[0].GetUInt32();
            target.Type = fields[1].GetUInt32();

            if (target.Type > SPELL_LIST_TARGETING_MAX)
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_targeting type %u. Skipping.", target.Type);
                continue;
            }

            target.Param1 = fields[2].GetUInt32();
            target.Param2 = fields[3].GetUInt32();
            target.Param3 = fields[4].GetUInt32();
            target.UnitCondition = fields[5].GetInt32();
            target.Comment = fields[6].GetCppString();
            newContainer->targeting[target.Id] = target;
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, Name, ChanceSupportAction, ChanceRangedAttack FROM creature_spell_list_entry");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            CreatureSpellList list;
            list.Id = fields[0].GetUInt32();
            list.Name = fields[1].GetCppString();
            if (list.Name.empty())
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list_entry empty name.");
                continue;
            }

            list.ChanceSupportAction = fields[2].GetUInt32();
            list.ChanceRangedAttack = fields[3].GetUInt32();
            list.Disabled = false;
            newContainer->spellLists[list.Id] = list;
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, Position, SpellId, Flags, CombatCondition, TargetId, ScriptId, Availability, Probability, InitialMin, InitialMax, RepeatMin, RepeatMax FROM creature_spell_list");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            CreatureSpellListSpell spell;
            spell.Id = fields[0].GetUInt32();
            spell.Position = fields[1].GetUInt32();
            spell.SpellId = fields[2].GetUInt32();
            spell.Flags = fields[3].GetUInt32();
            spell.CombatCondition = fields[4].GetUInt32();

            auto listItr = newContainer->spellLists.find(spell.Id);
            if (listItr == newContainer->spellLists.end())
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u list does not exist. Skipping.", spell.Id);
                continue;
            }

            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spell.SpellId);
            if (!spellInfo && spell.SpellId != 2) // 2 is attack which is hardcoded in client
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u spell %u does not exist. Skipping.", spell.Id, spell.SpellId);
                continue;
            }

            if (newContainer->spellLists[spell.Id].Spells.find(spell.SpellId) != newContainer->spellLists[spell.Id].Spells.end())
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u contains duplicate position %u. Skipping.", spell.Id, spell.Position);
                continue;
            }

            uint32 targetId = fields[5].GetUInt32();
            auto itr = newContainer->targeting.find(targetId);
            if (itr == newContainer->targeting.end())
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u target %u. Skipping.", spell.Id, targetId);
                continue;
            }

            if (spell.Flags & SPELL_LIST_FLAG_RANGED_ACTION && listItr->second.ChanceRangedAttack == 0)
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u list, spell %u has ranged spell flag but ChanceRangedAttack is 0. Skipping.", spell.Id, spell.SpellId);
                continue;
            }

            if (spell.Flags & SPELL_LIST_FLAG_SUPPORT_ACTION && listItr->second.ChanceSupportAction == 0)
            {
                sLog.outErrorDb("LoadCreatureSpellLists: Invalid creature_spell_list %u list, spell %u has support spell flag but ChanceSupportAction is 0. Skipping.", spell.Id, spell.SpellId);
                continue;
            }

            spell.Target = &(*itr).second;

            spell.ScriptId = fields[6].GetUInt32();
            spell.Availability = fields[7].GetUInt32();
            spell.Probability = fields[8].GetUInt32();
            spell.InitialMin = fields[9].GetUInt32();
            spell.InitialMax = fields[10].GetUInt32();
            spell.RepeatMin = fields[11].GetUInt32();
            spell.RepeatMax = fields[12].GetUInt32();
            spell.DisabledForAI = !spellInfo || spellInfo->HasAttribute(SPELL_ATTR_EX_NO_AUTOCAST_AI);
            newContainer->spellLists[spell.Id].Spells.emplace(spell.Position, spell);
        } while (result->NextRow());
    }

    m_spellListContainer = newContainer;
    sLog.outString(">> Loaded %u creature_spell_list definitions", count);
    sLog.outString();

    return newContainer;
}

void ObjectMgr::LoadSpawnGroups()
{
    std::shared_ptr<SpawnGroupEntryContainer> newContainer = std::make_shared<SpawnGroupEntryContainer>();
    uint32 count = 0;

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Id, Name, Type, MaxCount, WorldState, WorldStateExpression, Flags, StringId, RespawnOverrideMin, RespawnOverrideMax FROM spawn_group"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            SpawnGroupEntry entry;
            entry.Id = fields[0].GetUInt32();
            entry.Name = fields[1].GetCppString();
            if (entry.Name.empty())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group empty name. Skipping.");
                continue;
            }

            entry.Type = (SpawnGroupType)fields[2].GetUInt32();

            if (entry.Type > SPAWN_GROUP_GAMEOBJECT)
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group unknown type %u. Skipping.", entry.Type);
                continue;
            }

            entry.MaxCount = fields[3].GetUInt32();
            entry.WorldStateCondition = fields[4].GetInt32();

            if (entry.WorldStateCondition)
            {
                const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(entry.WorldStateCondition);
                if (!condition) // condition does not exist for some reason
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group (%u) condition entry %u. Skipping.", entry.Id, entry.WorldStateCondition);
                    continue;
                }
            }

            entry.WorldStateExpression = fields[5].GetInt32();
            if (entry.WorldStateExpression)
            {
                if (!m_worldStateExpressionMgr->Exists(entry.WorldStateExpression)) // invalid id
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group (%u) worldstate expression id %u. Skipping.", entry.Id, entry.WorldStateExpression);
                    continue;
                }
            }

            entry.Flags = fields[6].GetUInt32();
            entry.StringId = fields[7].GetUInt32();

            if (entry.StringId && !sScriptMgr.ExistsStringId(entry.StringId))
            {
                sLog.outErrorDb("Table spawn_group entry %u stringId %u does not exist. Setting to 0.", entry.Id, entry.StringId);
                entry.StringId = 0;
            }

            entry.Active = false;
            entry.Large = false;
            entry.EnabledByDefault = true;
            entry.Formation = nullptr;
            entry.HasChancedSpawns = false;
            if (!fields[8].IsNULL())
                entry.RespawnOverrideMin = fields[8].GetUInt32();
            if (!fields[9].IsNULL())
                entry.RespawnOverrideMax = fields[9].GetUInt32();
            newContainer->spawnGroupMap.emplace(entry.Id, std::move(entry));
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, FormationType, FormationSpread, FormationOptions, PathId, MovementType, Comment FROM spawn_group_formation");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            FormationEntry fEntry;
            fEntry.GroupId = fields[0].GetUInt32();
            uint32 fType = fields[1].GetUInt32();
            fEntry.Spread = fields[2].GetFloat();
            fEntry.Options = fields[3].GetUInt32();
            fEntry.MovementIdOrWander = fields[4].GetUInt32();
            fEntry.MovementType = fields[5].GetUInt32();
            fEntry.Comment = fields[6].GetCppString();

            auto itr = newContainer->spawnGroupMap.find(fEntry.GroupId);
            if (itr == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid group Id:%u found in `spawn_group_formation`. Skipping.", fEntry.GroupId);
                continue;
            }

            if (fType >= static_cast<uint32>(SPAWN_GROUP_FORMATION_TYPE_COUNT))
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid formation type in `spawn_group_formation` ID:%u. Skipping.", fEntry.GroupId);
                continue;
            }

            if (fEntry.MovementType >= static_cast<uint32>(MAX_DB_MOTION_TYPE))
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid movement type in `spawn_group_formation` ID:%u. Skipping.", fEntry.GroupId);
                continue;
            }

            //todo this check have to be done after loading movement_table (load movement before spawn_group)
//             if (fEntry->MovementID)
//             {
//                 auto path = sWaypointMgr.GetPathFromOrigin(fEntry->MovementID, 0, 0, WaypointPathOrigin::PATH_FROM_MOVEMENT_TEMPLATE);
//                 if (!path)
//                 {
//                     sLog.outErrorDb("LoadSpawnGroups: No path ID(%u) found for formation ID(%u) in `movement_template` ID:%u. Ignoring movement.", fEntry->MovementID, fEntry->GroupId);
//                     fEntry->MovementID = 0;
//                 }
//             }

            fEntry.Type = static_cast<SpawnGroupFormationType>(fType);

            if (fEntry.Spread > 15.0f || fEntry.Spread < -15)
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spread value (%5.2f) should be between (-15..15) in formation ID:%u . Skipping.", fEntry.Spread, fEntry.GroupId);
                continue;
            }

            itr->second.Formation = std::make_unique<FormationEntry>(std::move(fEntry));
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, Guid, SlotId, Chance FROM spawn_group_spawn");
    if (result)
    {
        std::set<uint32> foundCreatureGuids;
        std::set<uint32> foundGoGuids;
        do
        {
            Field* fields = result->Fetch();

            SpawnGroupDbGuids guid;
            guid.Id = fields[0].GetUInt32();
            guid.DbGuid = fields[1].GetUInt32();
            guid.SlotId = fields[2].GetInt32();
            guid.Chance = fields[3].GetUInt32();
            guid.OwnEntry = 0;
            guid.RandomEntry = false;

            if (newContainer->spawnGroupMap.find(guid.Id) == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_spawn Id %u. Skipping.", guid.Id);
                continue;
            }

            auto& group = newContainer->spawnGroupMap[guid.Id];

            if (group.Type == SPAWN_GROUP_CREATURE)
            {
                if (foundCreatureGuids.find(guid.DbGuid) != foundCreatureGuids.end())
                {
                    sLog.outErrorDb("LoadSpawnGroups: spawn_group_spawn creature dbGuid %u belongs to more than one spawn_group. Skipping.", guid.DbGuid);
                    continue;
                }

                CreatureData const* data = GetCreatureData(guid.DbGuid);
                if (!data)
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_spawn guid %u. Skipping.", guid.DbGuid);
                    continue;
                }
                if (!data->IsNotPartOfPoolOrEvent())
                {
                    sLog.outErrorDb("LoadSpawnGroups: spawn_group_spawn guid %u is part of pool or game event (incompatible). Skipping.", guid.DbGuid);
                    continue;
                }
            }
            else
            {
                if (foundGoGuids.find(guid.DbGuid) != foundGoGuids.end())
                {
                    sLog.outErrorDb("LoadSpawnGroups: spawn_group_spawn gameobject dbGuid %u belongs to more than one spawn_group. Skipping.", guid.DbGuid);
                    continue;
                }

                GameObjectData const* data = GetGOData(guid.DbGuid);
                if (!data)
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_spawn guid %u. Skipping.", guid.DbGuid);
                    continue;
                }
                if (!data->IsNotPartOfPoolOrEvent())
                {
                    sLog.outErrorDb("LoadSpawnGroups: spawn_group_spawn guid %u is part of pool or game event (incompatible). Skipping.", guid.DbGuid);
                    continue;
                }
            }

            group.DbGuids.push_back(guid);
            if (guid.Chance)
                group.HasChancedSpawns = true;

            if (group.Type == SPAWN_GROUP_CREATURE)
                foundCreatureGuids.insert(guid.DbGuid);
            else
                foundGoGuids.insert(guid.DbGuid);
        } while (result->NextRow());

        // check and fix correctness of slot id indexation
        for (auto& sg : newContainer->spawnGroupMap)
        {
            if (sg.second.Formation == nullptr)
                continue;

            auto& guidMap = sg.second.DbGuids;

            uint32 slotIndex = 0;
            bool dbError = false;
            std::sort(guidMap.begin(), guidMap.end(), [](SpawnGroupDbGuids const& a, SpawnGroupDbGuids const& b) { return a.SlotId < b.SlotId; });
            for (auto& gInfo : guidMap)
            {
                if (gInfo.SlotId < 0)
                    continue;

                if (gInfo.SlotId != slotIndex)
                {
                    gInfo.SlotId = slotIndex;
                    dbError = true;
                }
                ++slotIndex;
            }

            if (dbError)
                sLog.outErrorDb("LoadSpawnGroups: Invalid index for slot id in `spawn_group_spawn` for group ID:%u. Using default.", sg.second.Id);
        }
    }

    result = WorldDatabase.Query("SELECT Id, Entry, MinCount, MaxCount, Chance FROM spawn_group_entry");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            SpawnGroupRandomEntry randomEntry;
            randomEntry.Id = fields[0].GetUInt32();
            randomEntry.Entry = fields[1].GetUInt32();
            randomEntry.MinCount = fields[2].GetUInt32();
            randomEntry.MaxCount = fields[3].GetUInt32();
            randomEntry.Chance = fields[4].GetUInt32();

            auto itr = newContainer->spawnGroupMap.find(randomEntry.Id);
            if (itr == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_entry Id %u. Skipping.", randomEntry.Id);
                continue;
            }

            if ((*itr).second.Type == SPAWN_GROUP_CREATURE)
            {
                if (!GetCreatureTemplate(randomEntry.Entry))
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_entry Creature entry %u. Skipping.", randomEntry.Entry);
                    continue;
                }
            }
            else if ((*itr).second.Type == SPAWN_GROUP_GAMEOBJECT)
            {
                if (!GetGameObjectInfo(randomEntry.Entry))
                {
                    sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_entry GameObject %u. Skipping.", randomEntry.Id);
                    continue;
                }
            }
            else // safety for future extension
                continue;

            newContainer->spawnGroupMap[randomEntry.Id].RandomEntries.push_back(randomEntry);
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, LinkedId FROM spawn_group_linked_group");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 Id = fields[0].GetUInt32();
            uint32 linkedId = fields[1].GetUInt32();

            if (newContainer->spawnGroupMap.find(Id) == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_linked_group Id %u. Skipping.", Id);
                continue;
            }

            if (newContainer->spawnGroupMap.find(linkedId) == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_linked_group LinkedId %u. Skipping.", linkedId);
                continue;
            }

            auto& group = newContainer->spawnGroupMap[Id];

            group.LinkedGroups.push_back(linkedId);
        } while (result->NextRow());
    }

    result = WorldDatabase.Query("SELECT Id, SquadId, Guid, Entry FROM spawn_group_squad");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 Id = fields[0].GetUInt32();

            uint32 squadId = fields[1].GetUInt32();
            uint32 dbGuid = fields[2].GetUInt32();
            uint32 entry = fields[3].GetUInt32();

            auto itr = newContainer->spawnGroupMap.find(Id);
            if (itr == newContainer->spawnGroupMap.end())
            {
                sLog.outErrorDb("LoadSpawnGroups: Invalid spawn_group_squad Id %u. Skipping.", Id);
                continue;
            }

            auto& spawnGroup = itr->second;
            if (!spawnGroup.RandomEntries.empty())
                sLog.outErrorDb("LoadSpawnGroups: spawn_group_squad Id %u has spawn_group_entry. Will be overriden by squad", Id);

            auto squadItr = std::find_if(spawnGroup.Squads.begin(), spawnGroup.Squads.end(), [squadId](const SpawnGroupSquad& obj) -> bool { return obj.SquadId == squadId; });

            if (squadItr == spawnGroup.Squads.end())
            {
                SpawnGroupSquad squad;
                squad.SquadId = squadId;
                squad.GuidToEntry.emplace(dbGuid, entry);
                spawnGroup.Squads.push_back(std::move(squad));
            }
            else
            {
                squadItr->GuidToEntry.emplace(dbGuid, entry);
            }
        }
        while (result->NextRow());
    }

    for (auto& data : newContainer->spawnGroupMap)
    {
        SpawnGroupEntry& entry = data.second;
        if (entry.MaxCount == 0) // if no explicit max count, the max count is min of count of all random entries or spawns
        {
            uint32 maxRandom = 0;
            bool maxCount = false;
            for (auto& randomEntry : entry.RandomEntries)
            {
                maxRandom += randomEntry.MaxCount;
                if (randomEntry.Chance == 0)
                    maxCount = true;
            }                
            if (maxCount)
                entry.MaxCount = entry.DbGuids.size();
            else
                entry.MaxCount = std::min(maxRandom, uint32(entry.DbGuids.size()));
            if (!entry.MaxCount && entry.RandomEntries.empty())
                entry.MaxCount = entry.DbGuids.size();
        }

        for (auto& randomEntry : entry.RandomEntries)
        {
            if (randomEntry.Chance == 0)
                entry.EquallyChanced.push_back(&randomEntry);
            else
                entry.ExplicitlyChanced.push_back(&randomEntry);
        }

        for (auto& guidData : entry.DbGuids)
        {
            if (entry.Type == SPAWN_GROUP_CREATURE)
            {
                CreatureData const* data = GetCreatureData(guidData.DbGuid);
                RemoveCreatureFromGrid(guidData.DbGuid, data);
                auto& creatureDynguidsForMap = m_dynguidCreatureDbGuids[data->mapid];
                creatureDynguidsForMap.erase(std::remove(creatureDynguidsForMap.begin(), creatureDynguidsForMap.end(), guidData.DbGuid), creatureDynguidsForMap.end());
                newContainer->spawnGroupByGuidMap.emplace(std::make_pair(guidData.DbGuid, uint32(TYPEID_UNIT)), &entry);
                bool actives = sWorld.getConfig(CONFIG_BOOL_AUTOLOAD_ACTIVE);
                bool specials = sWorld.getConfig(CONFIG_BOOL_SPECIALS_ACTIVE);
                if (actives || specials)
                {
                    for (auto& data : entry.RandomEntries)
                    {
                        if (CreatureInfo const* cinfo = GetCreatureTemplate(data.Entry))
                        {
                            if (actives && (cinfo->ExtraFlags & CREATURE_EXTRA_FLAG_ACTIVE) != 0)
                            {
                                entry.Active = true;
                                break;
                            }
                            else if (specials && cinfo->IsLargeOrBiggerCreature())
                            {
                                entry.Large = true;
                                break;
                            }
                        }
                    }
                }
                if (data->spawnMask == 0)
                    entry.EnabledByDefault = false;
                if (data->id)
                    guidData.OwnEntry = data->id;
                if (GetAllRandomCreatureEntries(guidData.DbGuid))
                    guidData.RandomEntry = true;
            }
            else
            {
                GameObjectData const* data = GetGOData(guidData.DbGuid);
                RemoveGameobjectFromGrid(guidData.DbGuid, data);
                auto& goDynguidsForMap = m_dynguidGameobjectDbGuids[data->mapid];
                goDynguidsForMap.erase(std::remove(goDynguidsForMap.begin(), goDynguidsForMap.end(), guidData.DbGuid), goDynguidsForMap.end());
                newContainer->spawnGroupByGuidMap.emplace(std::make_pair(guidData.DbGuid, uint32(TYPEID_GAMEOBJECT)), &entry);
                bool actives = sWorld.getConfig(CONFIG_BOOL_AUTOLOAD_ACTIVE);
                bool specials = sWorld.getConfig(CONFIG_BOOL_SPECIALS_ACTIVE);
                if (actives || specials)
                {
                    for (auto& data : entry.RandomEntries)
                    {
                        if (GameObjectInfo const* goInfo = GetGameObjectInfo(data.Entry))
                        {
                            if (actives && (goInfo->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_ACTIVE) != 0)
                            {
                                entry.Active = true;
                                break;
                            }
                            else if (specials && goInfo->IsLargeOrBiggerGameObject())
                            {
                                entry.Large = true;
                                break;
                            }
                        }
                    }
                }
                if (data->spawnMask == 0)
                    entry.EnabledByDefault = false;
                if (data->id)
                    guidData.OwnEntry = data->id;
                if (GetAllRandomGameObjectEntries(guidData.DbGuid))
                    guidData.RandomEntry = true;
            }
        }
    }

    m_spawnGroupContainer = newContainer;
    sLog.outString(">> Loaded %u spawn_group definitions", uint32(m_spawnGroupContainer->spawnGroupMap.size()));
    sLog.outString();
}

void ObjectMgr::LoadEquipmentTemplates()
{
    sEquipmentStorage.Load();

    for (uint32 i = 0; i < sEquipmentStorage.GetMaxEntry(); ++i)
    {
        EquipmentInfo const* eqInfo = sEquipmentStorage.LookupEntry<EquipmentInfo>(i);

        if (!eqInfo)
            continue;

        for (uint8 j = 0; j < 3; ++j)
        {
            if (!eqInfo->equipentry[j])
                continue;

            ItemEntry const* dbcitem = sItemStore.LookupEntry(eqInfo->equipentry[j]);
            if (!dbcitem)
            {
                sLog.outErrorDb("Unknown item (entry=%u) in creature_equip_template.equipentry%u for entry = %u, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
                continue;
            }

            if (dbcitem->InventoryType != INVTYPE_WEAPON &&
                    dbcitem->InventoryType != INVTYPE_SHIELD &&
                    dbcitem->InventoryType != INVTYPE_RANGED &&
                    dbcitem->InventoryType != INVTYPE_2HWEAPON &&
                    dbcitem->InventoryType != INVTYPE_WEAPONMAINHAND &&
                    dbcitem->InventoryType != INVTYPE_WEAPONOFFHAND &&
                    dbcitem->InventoryType != INVTYPE_HOLDABLE &&
                    dbcitem->InventoryType != INVTYPE_THROWN &&
                    dbcitem->InventoryType != INVTYPE_RANGEDRIGHT &&
                    dbcitem->InventoryType != INVTYPE_RELIC)
            {
                sLog.outErrorDb("Item (entry=%u) in creature_equip_template.equipentry%u for entry = %u is not equipable in a hand, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
            }
        }
    }

    sLog.outString(">> Loaded %u equipment template", sEquipmentStorage.GetRecordCount());
    sLog.outString();
}

// generally models that does not have a gender(2), or has alternative model for same gender
uint32 ObjectMgr::GetCreatureModelAlternativeModel(uint32 modelId) const
{
    if (const CreatureModelInfo* modelInfo = GetCreatureModelInfo(modelId))
        return modelInfo->modelid_alternative;

    return 0;
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelRandomGender(uint32 display_id) const
{
    CreatureModelInfo const* minfo = GetCreatureModelInfo(display_id);
    if (!minfo)
        return nullptr;

    // If a model for another gender exists, 50% chance to use it
    if (minfo->modelid_other_gender != 0 && urand(0, 1) == 0)
    {
        CreatureModelInfo const* minfo_tmp = GetCreatureModelInfo(minfo->modelid_other_gender);
        if (!minfo_tmp)
        {
            sLog.outErrorDb("Model (Entry: %u) has modelid_other_gender %u not found in table `creature_model_info`. ", minfo->modelid, minfo->modelid_other_gender);
            return minfo;                                   // not fatal, just use the previous one
        }
        return minfo_tmp;
    }
    return minfo;
}

uint32 ObjectMgr::GetModelForRace(uint32 sourceModelId, uint32 racemask)
{
    uint32 modelId = sourceModelId;

    CreatureModelRaceMapBounds bounds = m_mCreatureModelRaceMap.equal_range(sourceModelId);

    for (CreatureModelRaceMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (!(itr->second.racemask & racemask))
            continue;

        if (itr->second.creature_entry)
        {
            const CreatureInfo* cInfo = GetCreatureTemplate(itr->second.creature_entry);
            modelId = Creature::ChooseDisplayId(cInfo);
        }
        else
        {
            modelId = itr->second.modelid_racial;
        }
    }

    return modelId;
}

void ObjectMgr::LoadCreatureModelInfo()
{
    sCreatureModelStorage.Load();

    // post processing
    for (uint32 i = 1; i < sCreatureModelStorage.GetMaxEntry(); ++i)
    {
        CreatureModelInfo const* minfo = sCreatureModelStorage.LookupEntry<CreatureModelInfo>(i);
        if (!minfo)
            continue;

        if (!sCreatureDisplayInfoStore.LookupEntry(minfo->modelid))
            sLog.outErrorDb("Table `creature_model_info` has model for nonexistent model id (%u).", minfo->modelid);

        if (minfo->gender >= MAX_GENDER)
        {
            sLog.outErrorDb("Table `creature_model_info` has invalid gender (%u) for model id (%u).", uint32(minfo->gender), minfo->modelid);
            const_cast<CreatureModelInfo*>(minfo)->gender = GENDER_MALE;
        }

        if (minfo->modelid_other_gender)
        {
            if (minfo->modelid_other_gender == minfo->modelid)
            {
                sLog.outErrorDb("Table `creature_model_info` has redundant modelid_other_gender model (%u) defined for model id %u.", minfo->modelid_other_gender, minfo->modelid);
                const_cast<CreatureModelInfo*>(minfo)->modelid_other_gender = 0;
            }
            else if (!sCreatureDisplayInfoStore.LookupEntry(minfo->modelid_other_gender))
            {
                sLog.outErrorDb("Table `creature_model_info` has nonexistent modelid_other_gender model (%u) defined for model id %u.", minfo->modelid_other_gender, minfo->modelid);
                const_cast<CreatureModelInfo*>(minfo)->modelid_other_gender = 0;
            }
        }

        if (minfo->modelid_alternative)
        {
            if (minfo->modelid_alternative == minfo->modelid)
            {
                sLog.outErrorDb("Table `creature_model_info` has redundant modelid_alternative model (%u) defined for model id %u.", minfo->modelid_alternative, minfo->modelid);
                const_cast<CreatureModelInfo*>(minfo)->modelid_alternative = 0;
            }
            else if (!sCreatureDisplayInfoStore.LookupEntry(minfo->modelid_alternative))
            {
                sLog.outErrorDb("Table `creature_model_info` has nonexistent modelid_alternative model (%u) defined for model id %u.", minfo->modelid_alternative, minfo->modelid);
                const_cast<CreatureModelInfo*>(minfo)->modelid_alternative = 0;
            }
        }
    }

    // character races expected have model info data in table
    for (uint32 race = 1; race < sChrRacesStore.GetNumRows(); ++race)
    {
        ChrRacesEntry const* raceEntry = sChrRacesStore.LookupEntry(race);
        if (!raceEntry)
            continue;

        if (!((1 << (race - 1)) & RACEMASK_ALL_PLAYABLE))
            continue;

        if (CreatureModelInfo const* minfo = GetCreatureModelInfo(raceEntry->model_f))
        {
            if (minfo->gender != GENDER_FEMALE)
                sLog.outErrorDb("Table `creature_model_info` have wrong gender %u for character race %u female model id %u", minfo->gender, race, raceEntry->model_f);

            if (minfo->modelid_other_gender != raceEntry->model_m)
                sLog.outErrorDb("Table `creature_model_info` have wrong other gender model id %u for character race %u female model id %u", minfo->modelid_other_gender, race, raceEntry->model_f);

            if (minfo->bounding_radius <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_model_info` have wrong bounding_radius %f for character race %u female model id %u, use %f instead", minfo->bounding_radius, race, raceEntry->model_f, DEFAULT_WORLD_OBJECT_SIZE);
                const_cast<CreatureModelInfo*>(minfo)->bounding_radius = DEFAULT_WORLD_OBJECT_SIZE;
            }

            if (minfo->combat_reach != 1.5f)
            {
                sLog.outErrorDb("Table `creature_model_info` have wrong combat_reach %f for character race %u female model id %u, expected always 1.5f", minfo->combat_reach, race, raceEntry->model_f);
                const_cast<CreatureModelInfo*>(minfo)->combat_reach = 1.5f;
            }
        }
        else
            sLog.outErrorDb("Table `creature_model_info` expect have data for character race %u female model id %u", race, raceEntry->model_f);

        if (CreatureModelInfo const* minfo = GetCreatureModelInfo(raceEntry->model_m))
        {
            if (minfo->gender != GENDER_MALE)
                sLog.outErrorDb("Table `creature_model_info` have wrong gender %u for character race %u male model id %u", minfo->gender, race, raceEntry->model_m);

            if (minfo->modelid_other_gender != raceEntry->model_f)
                sLog.outErrorDb("Table `creature_model_info` have wrong other gender model id %u for character race %u male model id %u", minfo->modelid_other_gender, race, raceEntry->model_m);

            if (minfo->bounding_radius <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_model_info` have wrong bounding_radius %f for character race %u male model id %u, use %f instead", minfo->bounding_radius, race, raceEntry->model_f, DEFAULT_WORLD_OBJECT_SIZE);
                const_cast<CreatureModelInfo*>(minfo)->bounding_radius = DEFAULT_WORLD_OBJECT_SIZE;
            }

            if (minfo->combat_reach != 1.5f)
            {
                sLog.outErrorDb("Table `creature_model_info` have wrong combat_reach %f for character race %u male model id %u, expected always 1.5f", minfo->combat_reach, race, raceEntry->model_m);
                const_cast<CreatureModelInfo*>(minfo)->combat_reach = 1.5f;
            }
        }
        else
            sLog.outErrorDb("Table `creature_model_info` expect have data for character race %u male model id %u", race, raceEntry->model_m);
    }

    sLog.outString(">> Loaded %u creature model based info", sCreatureModelStorage.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::LoadCreatureModelRace()
{
    m_mCreatureModelRaceMap.clear();                        // can be used for reload

    auto queryResult = WorldDatabase.Query("SELECT modelid, racemask, creature_entry, modelid_racial FROM creature_model_race");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded creature_model_race, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;

    // model, racemask
    std::map<uint32, uint32> model2raceMask;

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        CreatureModelRace raceData;

        raceData.modelid            = fields[0].GetUInt32();
        raceData.racemask           = fields[1].GetUInt32();
        raceData.creature_entry     = fields[2].GetUInt32();
        raceData.modelid_racial     = fields[3].GetUInt32();

        if (!sCreatureDisplayInfoStore.LookupEntry(raceData.modelid))
        {
            sLog.outErrorDb("Table `creature_model_race` has model for nonexistent model id (%u), skipping", raceData.modelid);
            continue;
        }

        if (!sCreatureModelStorage.LookupEntry<CreatureModelInfo>(raceData.modelid))
        {
            sLog.outErrorDb("Table `creature_model_race` modelid %u does not exist in creature_model_info, skipping", raceData.modelid);
            continue;
        }

        if (!raceData.racemask)
        {
            sLog.outErrorDb("Table `creature_model_race` modelid %u has no racemask defined, skipping", raceData.modelid);
            continue;
        }

        if (!(raceData.racemask & RACEMASK_ALL_PLAYABLE))
        {
            sLog.outErrorDb("Table `creature_model_race` modelid %u include invalid racemask, skipping", raceData.modelid);
            continue;
        }

        std::map<uint32, uint32>::const_iterator model2Race = model2raceMask.find(raceData.modelid);

        // can't have same mask for same model several times
        if (model2Race != model2raceMask.end())
        {
            if (model2Race->second & raceData.racemask)
            {
                sLog.outErrorDb("Table `creature_model_race` modelid %u with racemask %u has mask already included for same modelid, skipping", raceData.modelid, raceData.racemask);
                continue;
            }
        }

        model2raceMask[raceData.modelid] |= raceData.racemask;

        // creature_entry is the prefered way
        if (raceData.creature_entry)
        {
            if (raceData.modelid_racial)
                sLog.outErrorDb("Table `creature_model_race` modelid %u has modelid_racial for modelid %u but a creature_entry are already defined, modelid_racial will never be used.", raceData.modelid, raceData.modelid_racial);

            if (!sCreatureStorage.LookupEntry<CreatureInfo>(raceData.creature_entry))
            {
                sLog.outErrorDb("Table `creature_model_race` modelid %u has creature_entry for nonexistent creature_template (%u), skipping", raceData.modelid, raceData.creature_entry);
                continue;
            }
        }
        else if (raceData.modelid_racial)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(raceData.modelid_racial))
            {
                sLog.outErrorDb("Table `creature_model_race` modelid %u has modelid_racial for nonexistent model id (%u), skipping", raceData.modelid, raceData.modelid_racial);
                continue;
            }

            if (!sCreatureModelStorage.LookupEntry<CreatureModelInfo>(raceData.modelid_racial))
            {
                sLog.outErrorDb("Table `creature_model_race` modelid %u has modelid_racial %u, but are not defined in creature_model_info, skipping", raceData.modelid, raceData.modelid_racial);
                continue;
            }
        }
        else
        {
            sLog.outErrorDb("Table `creature_model_race` modelid %u does not have either creature_entry or modelid_racial defined, skipping", raceData.modelid);
            continue;
        }

        m_mCreatureModelRaceMap.insert(CreatureModelRaceMap::value_type(raceData.modelid, raceData));

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u creature_model_race entries", count);
    sLog.outString();
}

void ObjectMgr::LoadCreatureConditionalSpawn()
{
    sCreatureConditionalSpawnStore.Load();

    // post processing
    for (uint32 i = 1; i < sCreatureConditionalSpawnStore.GetMaxEntry(); ++i)
    {
        CreatureConditionalSpawn const* spawn = sCreatureConditionalSpawnStore.LookupEntry<CreatureConditionalSpawn>(i);
        if (!spawn)
            continue;

        CreatureInfo const* cInfoAlliance = GetCreatureTemplate(spawn->EntryAlliance);
        CreatureInfo const* cInfoHorde = GetCreatureTemplate(spawn->EntryHorde);

        // check if both alliance and horde entries are missing; one faction spawn is allowed
        if (!cInfoAlliance && !cInfoHorde)
        {
            sLog.outErrorDb("Table `creature_conditional_spawn` has creature (GUID: %u) with non existing alliance creature entry %u and horde creature entry %u, skipped.", spawn->Guid, spawn->EntryAlliance, spawn->EntryHorde);
        }
    }

    sLog.outString(">> Loaded %u creature_conditional_spawn entries", sCreatureConditionalSpawnStore.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::LoadCreatureSpawnDataTemplates()
{
    m_creatureSpawnTemplateMap.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Entry, NpcFlags, UnitFlags, Faction, ModelId, EquipmentId, CurHealth, CurMana, SpawnFlags, RelayId, StringId, Name FROM creature_spawn_data_template"));
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded creature_spawn_data_template, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    uint32 count = 0;

    do
    {
        bar.step();

        Field* fields = result->Fetch();

        uint32 entry        = fields[0].GetUInt32();
        int32 npcFlags      = int32(fields[1].GetUInt32());
        int64 unitFlags     = int64(fields[2].GetUInt64());
        uint32 faction      = fields[3].GetUInt32();
        uint32 modelId      = fields[4].GetUInt32();
        int32 equipmentId   = fields[5].GetInt32();
        uint32 curHealth    = fields[6].GetUInt32();
        uint32 curMana      = fields[7].GetUInt32();
        uint32 spawnFlags   = fields[8].GetUInt32();
        uint32 relayId      = fields[9].GetUInt32();
        uint32 stringId     = fields[10].GetUInt32();
        std::string name    = fields[11].GetCppString();

        if (name.empty())
            sLog.outErrorDb("Table creature_spawn_data_template for entry %u has empty name", entry);

        if (stringId && !sScriptMgr.ExistsStringId(stringId))
        {
            stringId = 0;
            sLog.outErrorDb("Table creature_spawn_data_template entry %u stringId %u does not exist. Setting to 0.", entry, stringId);
        }

        // leave room for invalidation in future

        auto& data = m_creatureSpawnTemplateMap[entry];
        data.npcFlags = npcFlags;
        data.unitFlags = unitFlags;
        data.faction = faction;
        data.modelId = modelId;
        data.equipmentId = equipmentId;
        data.curHealth = curHealth;
        data.curMana = curMana;
        data.spawnFlags = spawnFlags;
        data.relayId = relayId;
        data.stringId = stringId;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u creature_spawn_data_template entries", count);
    sLog.outString();
}

void ObjectMgr::LoadCreatureSpawnEntry()
{
    m_creatureSpawnEntryMap.clear();

    auto queryResult = WorldDatabase.Query("SELECT guid, entry FROM creature_spawn_entry");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded creature_spawn_entry, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 guid = fields[0].GetUInt32();
        uint32 entry = fields[1].GetUInt32();

        CreatureInfo const* cInfo = GetCreatureTemplate(entry);
        if (!cInfo)
        {
            sLog.outErrorDb("Table `creature_spawn_entry` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, entry);
            continue;
        }

        auto& entries = m_creatureSpawnEntryMap[guid];
        if (cInfo->ExtraFlags & CREATURE_EXTRA_FLAG_ACTIVE)
            entries.first = true; // if at least one entry is dynguided, promote dbGuid to dynguided
        entries.second.push_back(entry);

        ++count;
    } while (queryResult->NextRow());

    sLog.outString(">> Loaded %u creature_spawn_entry entries", count);
    sLog.outString();
}

void ObjectMgr::LoadCreatures()
{
    uint32 count = 0;
    //                                             0                       1   2
    auto queryResult = WorldDatabase.Query("SELECT creature.guid, creature.id, map,"
                          //        3           4           5            6                 7                 8          9
                          "position_x, position_y, position_z, orientation, spawntimesecsmin, spawntimesecsmax, spawndist,"
                          //   10         11        12         13
                          "MovementType, spawnMask, phaseMask, event,"
                          //   14                        15
                          "pool_creature.pool_entry, pool_creature_template.pool_entry,"
                          //   16
                          "creature_spawn_data.id "
                          "FROM creature "
                          "LEFT OUTER JOIN game_event_creature ON creature.guid = game_event_creature.guid "
                          "LEFT OUTER JOIN pool_creature ON creature.guid = pool_creature.guid "
                          "LEFT OUTER JOIN pool_creature_template ON creature.id = pool_creature_template.id "
                          "LEFT OUTER JOIN creature_spawn_data ON creature.guid = creature_spawn_data.guid ");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 creature. DB table `creature` is empty.");
        sLog.outString();
        return;
    }

    // build single time for check creature data
    std::set<uint32> difficultyCreatures[MAX_DIFFICULTY - 1];
    for (uint32 i = 0; i < sCreatureStorage.GetMaxEntry(); ++i)
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
            for (uint32 diff = 0; diff < MAX_DIFFICULTY - 1; ++diff)
                if (cInfo->DifficultyEntry[diff])
                    difficultyCreatures[diff].insert(cInfo->DifficultyEntry[diff]);

    // build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (uint32 i = 0; i < sMapStore.GetNumRows(); ++i)
        if (sMapStore.LookupEntry(i))
            for (int k = 0; k < MAX_DIFFICULTY; ++k)
                if (GetMapDifficultyData(i, Difficulty(k)))
                    spawnMasks[i] |= (1 << k);

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 guid         = fields[ 0].GetUInt32();
        uint32 entry        = fields[ 1].GetUInt32();

        // validate creature dual spawn template
        bool isConditional  = false;
        bool dynGuid = false;
        if (entry == 0)
        {
            CreatureConditionalSpawn const* cSpawn = GetCreatureConditionalSpawn(guid);
            if (!cSpawn)
            {
                if (uint32 randomEntry = GetRandomCreatureEntry(guid))
                {
                    entry = randomEntry;
                    dynGuid = IsCreatureDbGuidDynGuided(guid);
                }
            }
            else
            {
                isConditional = true;
                // set a default entry to validate the record; will be reset back to 0 afterwards
                entry = cSpawn->EntryAlliance != 0 ? cSpawn->EntryAlliance : cSpawn->EntryHorde;
            }
        }

        CreatureInfo const* cInfo = nullptr;
        if (entry)
        {
            cInfo = GetCreatureTemplate(entry);
            if (!cInfo)
            {
                sLog.outErrorDb("Table `creature` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, entry);
                continue;
            }

            if (!strcmp(cInfo->AIName, "TotemAI"))
            {
                sLog.outErrorDb("Table `creature` has a creature (GUID: %u, entry: %u) using TotemAI via AIName, skipped.", guid, entry);
                continue;
            }

            if (cInfo->ExtraFlags & CREATURE_EXTRA_FLAG_DYNGUID)
                dynGuid = true;
        }

        CreatureData& data = mCreatureDataMap[guid];

        data.id                 = entry;
        data.mapid              = fields[ 2].GetUInt32();
        data.posX               = fields[ 3].GetFloat();
        data.posY               = fields[ 4].GetFloat();
        data.posZ               = fields[ 5].GetFloat();
        data.orientation        = fields[ 6].GetFloat();
        data.spawntimesecsmin   = fields[ 7].GetUInt32();
        data.spawntimesecsmax   = fields[ 8].GetUInt32();
        data.spawndist          = fields[ 9].GetFloat();
        data.movementType       = fields[10].GetUInt8();
        data.spawnMask          = fields[11].GetUInt8();
        data.phaseMask          = fields[12].GetUInt16();
        data.gameEvent          = fields[13].GetInt16();
        data.GuidPoolId         = fields[14].GetInt16();
        data.EntryPoolId        = fields[15].GetInt16();
        data.spawnTemplate      = GetCreatureSpawnTemplate(0);
        uint32 spawnDataEntry   = fields[16].GetUInt32();


        if (m_maxCreatureDbGuid < guid)
            m_maxCreatureDbGuid = guid;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapid);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u) that spawned at nonexistent map (Id: %u), skipped.", guid, data.mapid);
            continue;
        }

        if (!MaNGOS::IsValidMapCoord(data.posX, data.posY, data.posZ))
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u) that spawned at not valid coordinate (x:%5.2f, y:%5.2f, z:%5.2f) skipped.", guid, data.posX, data.posY, data.posZ);
            continue;
        }

        if (data.spawntimesecsmax < data.spawntimesecsmin)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `spawntimesecsmax` (%u) value lower than `spawntimesecsmin` (%u), it will be adjusted to %u.",
                            guid, data.id, uint32(data.spawntimesecsmax), uint32(data.spawntimesecsmin), uint32(data.spawntimesecsmin));
            data.spawntimesecsmax = data.spawntimesecsmin;
        }

        if (m_transportMaps.find(data.mapid) == m_transportMaps.end() && data.spawnMask & ~spawnMasks[data.mapid])
            sLog.outErrorDb("Table `creature` have creature (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u).", guid, data.spawnMask, data.mapid);

        bool ok = true;
        for (uint32 diff = 0; diff < MAX_DIFFICULTY - 1 && ok; ++diff)
        {
            if (difficultyCreatures[diff].find(data.id) != difficultyCreatures[diff].end())
            {
                sLog.outErrorDb("Table `creature` have creature (GUID: %u) that listed as difficulty %u template (entry: %u) in `creature_template`, skipped.",
                                guid, diff + 1, data.id);
                ok = false;
            }
        }
        if (!ok)
            continue;

        if (data.spawndist < 0.0f)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `spawndist`< 0, set to 0.", guid, data.id);
            data.spawndist = 0.0f;
        }
        else if (data.movementType == RANDOM_MOTION_TYPE)
        {
            if (data.spawndist == 0.0f)
            {
                sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=1 (random movement) but with `spawndist`=0, replace by idle movement type (0).", guid, data.id);
                data.movementType = IDLE_MOTION_TYPE;
            }
        }
        else if (data.movementType == IDLE_MOTION_TYPE)
        {
            if (data.spawndist != 0.0f)
            {
                sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=0 (idle) have `spawndist`<>0, set to 0.", guid, data.id);
                data.spawndist = 0.0f;
            }
        }

        if (data.phaseMask == 0)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        if (spawnDataEntry > 0)
        {
            if (CreatureSpawnTemplate const* templateData = GetCreatureSpawnTemplate(spawnDataEntry))
                data.spawnTemplate = templateData;
            else
                sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with spawn template %u that doesnt exist.", guid, data.id, spawnDataEntry);
        }

        if (mapEntry->IsContinent())
        {
            auto terrainInfo = sTerrainMgr.LoadTerrain(data.mapid);
            data.OriginalZoneId = terrainInfo->GetZoneId(data.posX, data.posY, data.posZ);
        }
        else
            data.OriginalZoneId = 0;

        if (m_transportMaps.find(data.mapid) != m_transportMaps.end())
            m_guidsForMap[data.mapid].emplace_back(TYPEID_UNIT, guid);
        else if (dynGuid && !data.gameEvent)
        {
            m_dynguidCreatureDbGuids[data.mapid].push_back(guid);
        }
        else if (data.IsNotPartOfPoolOrEvent()) // if not this is to be managed by GameEvent System or Pool system
        {
            AddCreatureToGrid(guid, &data);

            if (sWorld.getConfig(CONFIG_BOOL_AUTOLOAD_ACTIVE) && cInfo && cInfo->ExtraFlags & CREATURE_EXTRA_FLAG_ACTIVE)
                m_activeCreatures.emplace(data.mapid, guid);
            else if (sWorld.getConfig(CONFIG_BOOL_SPECIALS_ACTIVE) && cInfo && cInfo->IsLargeOrBiggerCreature())
                m_largeCreatures.emplace(data.mapid, guid);
        }

        // reset the entry to 0; this will be processed by Creature::GetCreatureConditionalSpawnEntry
        if (isConditional)
            data.id = 0;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " creatures", mCreatureDataMap.size());
    sLog.outString();
}

void ObjectMgr::AddCreatureToGrid(uint32 guid, CreatureData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = MaNGOS::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.creatures.insert(guid);
        }
    }
}

void ObjectMgr::RemoveCreatureFromGrid(uint32 guid, CreatureData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = MaNGOS::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.creatures.erase(guid);
        }
    }
}

void ObjectMgr::LoadGameObjects()
{
    uint32 count = 0;

    //                                             0                           1   2    3           4           5           6
    auto queryResult = WorldDatabase.Query("SELECT gameobject.guid, gameobject.id, map, position_x, position_y, position_z, orientation,"
                          // 7        8          9          10         11                12                13         14         15
                          "rotation0, rotation1, rotation2, rotation3, spawntimesecsmin, spawntimesecsmax, spawnMask, phaseMask, event,"
                          //   16                          17
                          "pool_gameobject.pool_entry, pool_gameobject_template.pool_entry "
                          "FROM gameobject "
                          "LEFT OUTER JOIN game_event_gameobject ON gameobject.guid = game_event_gameobject.guid "
                          "LEFT OUTER JOIN pool_gameobject ON gameobject.guid = pool_gameobject.guid "
                          "LEFT OUTER JOIN pool_gameobject_template ON gameobject.id = pool_gameobject_template.id");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 gameobjects. DB table `gameobject` is empty.");
        sLog.outString();
        return;
    }

    // build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (uint32 i = 0; i < sMapStore.GetNumRows(); ++i)
        if (sMapStore.LookupEntry(i))
            for (int k = 0; k < MAX_DIFFICULTY; ++k)
                if (GetMapDifficultyData(i, Difficulty(k)))
                    spawnMasks[i] |= (1 << k);

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 guid         = fields[ 0].GetUInt32();
        uint32 entry        = fields[ 1].GetUInt32();

        bool dynGuid = false;
        if (entry == 0)
        {
            if (uint32 randomEntry = GetRandomGameObjectEntry(guid))
            {
                entry = randomEntry;
                dynGuid = IsGameObjectDbGuidDynGuided(guid);
            }
        }

        GameObjectInfo const* gInfo = nullptr;
        if (entry)
        {
            gInfo = GetGameObjectInfo(entry);
            if (!gInfo)
            {
                sLog.outErrorDb("Table `gameobject` has gameobject (GUID: %u) with non existing gameobject entry %u, skipped.", guid, entry);
                continue;
            }

            if (!gInfo->displayId)
            {
                switch (gInfo->type)
                {
                    // can be invisible always and then not req. display id in like case
                    case GAMEOBJECT_TYPE_TRAP:
                    case GAMEOBJECT_TYPE_SPELL_FOCUS:
                        break;
                    default:
                        sLog.outErrorDb("Gameobject (GUID: %u Entry %u GoType: %u) have displayId == 0 and then will always invisible in game.", guid, entry, gInfo->type);
                        break;
                }
            }
            else if (!sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
            {
                sLog.outErrorDb("Gameobject (GUID: %u Entry %u GoType: %u) have invalid displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
                continue;
            }

            if (gInfo->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_DYNGUID)
                dynGuid = true;
        }


        GameObjectData& data = mGameObjectDataMap[guid];

        data.id               = entry;
        data.mapid            = fields[ 2].GetUInt32();
        data.posX             = fields[ 3].GetFloat();
        data.posY             = fields[ 4].GetFloat();
        data.posZ             = fields[ 5].GetFloat();
        data.orientation      = fields[ 6].GetFloat();
        data.rotation.x       = fields[ 7].GetFloat();
        data.rotation.y       = fields[ 8].GetFloat();
        data.rotation.z       = fields[ 9].GetFloat();
        data.rotation.w       = fields[10].GetFloat();
        data.spawntimesecsmin = fields[11].GetInt32();
        data.spawntimesecsmax = fields[12].GetInt32();
        data.spawnMask        = fields[13].GetUInt8();
        data.phaseMask        = fields[14].GetUInt16();
        data.gameEvent        = fields[15].GetInt16();
        data.GuidPoolId       = fields[16].GetInt16();
        data.EntryPoolId      = fields[17].GetInt16();

        data.animprogress     = GO_ANIMPROGRESS_DEFAULT;
        data.goState          = -1;

        if (m_maxGoDbGuid < guid)
            m_maxGoDbGuid = guid;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapid);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) that spawned at nonexistent map (Id: %u), skip", guid, data.id, data.mapid);
            continue;
        }

        if (!MaNGOS::IsValidMapCoord(data.posX, data.posY, data.posZ))
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u) that spawned at not valid coordinate (x:%5.2f, y:%5.2f, z:%5.2f) skipped.", guid, data.posX, data.posY, data.posZ);
            continue;
        }

        if (m_transportMaps.find(data.mapid) == m_transportMaps.end() && data.spawnMask & ~spawnMasks[data.mapid])
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u), skip", guid, data.id, data.spawnMask, data.mapid);

        if (data.spawntimesecsmin == 0 && gInfo && gInfo->IsDespawnAtAction())
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with `spawntimesecs` (0) value, but gameobejct marked as despawnable at action.", guid, data.id);
        }

        if (data.spawntimesecsmax < data.spawntimesecsmin)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with `spawntimesecsmax` (%u) value lower than `spawntimesecsmin` (%u), it will be adjusted to %u.",
                            guid, data.id, uint32(data.spawntimesecsmax), uint32(data.spawntimesecsmin), uint32(data.spawntimesecsmin));
            data.spawntimesecsmax = data.spawntimesecsmin;
        }

        if (data.rotation.x < -1.0f || data.rotation.x > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation.x (%f) value, skip", guid, data.id, data.rotation.x);
            continue;
        }

        if (data.rotation.y < -1.0f || data.rotation.y > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation.y (%f) value, skip", guid, data.id, data.rotation.y);
            continue;
        }

        if (data.rotation.z < -1.0f || data.rotation.z > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation.z (%f) value, skip", guid, data.id, data.rotation.z);
            continue;
        }

        if (data.rotation.w < -1.0f || data.rotation.w > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation.w (%f) value, skip", guid, data.id, data.rotation.w);
            continue;
        }

        if (!MapManager::IsValidMapCoord(data.mapid, data.posX, data.posY, data.posZ, data.orientation))
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid coordinates, skip", guid, data.id);
            continue;
        }

        if (data.phaseMask == 0)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        if (mapEntry->IsContinent())
        {
            auto terrainInfo = sTerrainMgr.LoadTerrain(data.mapid);
            data.OriginalZoneId = terrainInfo->GetZoneId(data.posX, data.posY, data.posZ);
        }
        else
            data.OriginalZoneId = 0;

        if (m_transportMaps.find(data.mapid) != m_transportMaps.end())
            m_guidsForMap[data.mapid].emplace_back(TYPEID_GAMEOBJECT, guid);
        else if (dynGuid && !data.gameEvent)
        {
            m_dynguidGameobjectDbGuids[data.mapid].push_back(guid);
        }
        else if (data.IsNotPartOfPoolOrEvent()) // if not this is to be managed by GameEvent System or Pool system
        {
            AddGameobjectToGrid(guid, &data);

            if (sWorld.getConfig(CONFIG_BOOL_AUTOLOAD_ACTIVE) && gInfo && gInfo->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_ACTIVE)
                m_activeGameObjects.emplace(data.mapid, guid);
            else if (sWorld.getConfig(CONFIG_BOOL_SPECIALS_ACTIVE) && gInfo && gInfo->IsLargeOrBiggerGameObject())
                m_largeGameObjects.emplace(data.mapid, guid);
        }

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " gameobjects", mGameObjectDataMap.size());
    sLog.outString();

    queryResult = WorldDatabase.PQuery("SELECT guid, animprogress, state, stringId, path_rotation0, path_rotation1, path_rotation2, path_rotation3 FROM gameobject_addon");
    do
    {
        Field* fields = queryResult->Fetch();
        uint32 guid = fields[0].GetUInt32();
        auto itr = mGameObjectDataMap.find(guid);
        if (itr == mGameObjectDataMap.end())
            continue;

        GameObjectData& data = itr->second;

        data.animprogress = fields[1].GetUInt32();
        int32 state = fields[2].GetInt32();
        uint32 stringId = fields[3].GetUInt32();
        float rotation0 = fields[4].GetFloat();
        float rotation1 = fields[5].GetFloat();
        float rotation2 = fields[6].GetFloat();
        float rotation3 = fields[7].GetFloat();
        QuaternionData pathRotation(rotation0, rotation1, rotation2, rotation3);

        if (data.goState != -1 && data.goState >= MAX_GO_STATE)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid `state` (%u) value, skip", guid, data.id, data.goState);
            continue;
        }

        if (!pathRotation.isUnit())
        {
            sLog.outErrorDb("Gameobject (GUID: %u) has invalid path rotation in gameobject_addon - resetting.", guid);
            data.path_rotation = QuaternionData(0.f, 0.f, 0.f, 1.f);
        }
        else
            data.path_rotation = pathRotation;

        if (stringId && !sScriptMgr.ExistsStringId(stringId))
        {
            stringId = 0;
            sLog.outErrorDb("Table gameobject_addon guid %u stringId %u does not exist. Setting to 0.", guid, stringId);
        }

        data.StringId = stringId;
        data.goState = state;
    }
    while (queryResult->NextRow());
}

void ObjectMgr::LoadGameObjectSpawnEntry()
{
    m_gameobjectSpawnEntryMap.clear();

    auto queryResult = WorldDatabase.Query("SELECT guid, entry FROM gameobject_spawn_entry");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded gameobject_spawn_entry, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;

    std::set<uint32> dynGuided;

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 guid = fields[0].GetUInt32();
        uint32 entry = fields[1].GetUInt32();

        GameObjectInfo const* info = GetGameObjectInfo(entry);
        if (!info)
        {
            sLog.outErrorDb("Table `gameobject_spawn_entry` has gameobject (GUID: %u) with non existing gameobject entry %u, skipped.", guid, entry);
            continue;
        }

        auto& entries = m_gameobjectSpawnEntryMap[guid];
        if (info->ExtraFlags & GAMEOBJECT_EXTRA_FLAG_DYNGUID)
            entries.first = true; // if at least one entry is dynguided, promote dbGuid to dynguided
        entries.second.push_back(entry);

        ++count;
    } while (queryResult->NextRow());

    sLog.outString(">> Loaded %u gameobject_spawn_entry entries", count);
    sLog.outString();
}

void ObjectMgr::LoadGameObjectTemplateAddons()
{
    m_gameobjectAddonTemplates.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT entry, artkit0, artkit1, artkit2, artkit3 FROM gameobject_template_addon"));

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded gameobject_template_addon, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    uint32 count = 0;

	do
	{
		bar.step();

		Field* fields = result->Fetch();

		uint32 entry = fields[0].GetUInt32();

		GameObjectTemplateAddon& gameObjectAddon = m_gameobjectAddonTemplates[entry];

		for (uint32 i = 1; i < gameObjectAddon.artKits.size(); ++i)
		{
			uint32 artKitID = fields[i].GetUInt32();
			if (!artKitID)
				continue;

			if (!sGameObjectArtKitStore.LookupEntry(artKitID))
			{
				sLog.outErrorDb("GameObject (Entry: %u) has invalid `artkit%d` (%d) defined, set to zero instead.", entry, i, artKitID);
				continue;
			}

			gameObjectAddon.artKits[i] = artKitID;
		}
	} while (result->NextRow());

    sLog.outString(">> Loaded %u gameobject_template_addon entries", count);
    sLog.outString();
}

void ObjectMgr::AddGameobjectToGrid(uint32 guid, GameObjectData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = MaNGOS::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.gameobjects.insert(guid);
        }
    }
}

void ObjectMgr::RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = MaNGOS::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.gameobjects.erase(guid);
        }
    }
}

// Get player map id of offline player. Return -1 if not found!
int32 ObjectMgr::GetPlayerMapIdByGUID(ObjectGuid const& guid) const
{
    // prevent DB access for online player
    if (Player* player = GetPlayer(guid))
        return int32(player->GetMapId());

    auto queryResult = CharacterDatabase.PQuery("SELECT map FROM characters WHERE guid = '%u'", guid.GetCounter());

    if (queryResult)
    {
        uint32 mapId = (*queryResult)[0].GetUInt32();
        return int32(mapId);
    }

    return -1;
}

// name must be checked to correctness (if received) before call this function
ObjectGuid ObjectMgr::GetPlayerGuidByName(std::string name) const
{
    ObjectGuid guid;

    CharacterDatabase.escape_string(name);

    // Player name safe to sending to DB (checked at login) and this function using
    auto queryResult = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE name = '%s'", name.c_str());
    if (queryResult)
    {
        guid = ObjectGuid(HIGHGUID_PLAYER, (*queryResult)[0].GetUInt32());
    }

    return guid;
}

bool ObjectMgr::GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const
{
    // prevent DB access for online player
    if (Player* player = GetPlayer(guid))
    {
        name = player->GetName();
        return true;
    }

    uint32 lowguid = guid.GetCounter();

    auto queryResult = CharacterDatabase.PQuery("SELECT name FROM characters WHERE guid = '%u'", lowguid);

    if (queryResult)
    {
        name = (*queryResult)[0].GetCppString();
        return true;
    }

    return false;
}

Team ObjectMgr::GetPlayerTeamByGUID(ObjectGuid guid) const
{
    // prevent DB access for online player
    if (Player* player = GetPlayer(guid))
        return Player::TeamForRace(player->getRace());

    uint32 lowguid = guid.GetCounter();

    auto queryResult = CharacterDatabase.PQuery("SELECT race FROM characters WHERE guid = '%u'", lowguid);

    if (queryResult)
    {
        uint8 race = (*queryResult)[0].GetUInt8();
        return Player::TeamForRace(race);
    }

    return TEAM_NONE;
}

uint32 ObjectMgr::GetPlayerAccountIdByGUID(ObjectGuid guid) const
{
    if (!guid.IsPlayer())
        return 0;

    // prevent DB access for online player
    if (Player* player = GetPlayer(guid))
        return player->GetSession()->GetAccountId();

    uint32 lowguid = guid.GetCounter();

    auto queryResult = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = '%u'", lowguid);
    if (queryResult)
    {
        uint32 acc = (*queryResult)[0].GetUInt32();
        return acc;
    }

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByPlayerName(const std::string& name) const
{
    auto queryResult = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name = '%s'", name.c_str());
    if (queryResult)
    {
        uint32 acc = (*queryResult)[0].GetUInt32();
        return acc;
    }

    return 0;
}

void ObjectMgr::LoadItemLocales()
{
    mItemLocaleMap.clear();                                 // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,name_loc1,description_loc1,name_loc2,description_loc2,name_loc3,description_loc3,name_loc4,description_loc4,name_loc5,description_loc5,name_loc6,description_loc6,name_loc7,description_loc7,name_loc8,description_loc8 FROM locales_item");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 Item locale strings. DB table `locales_item` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetItemPrototype(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_item` has data for nonexistent item entry %u, skipped.", entry);
            continue;
        }

        ItemLocale& data = mItemLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }

            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Description.size() <= idx)
                        data.Description.resize(idx + 1);

                    data.Description[idx] = str;
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " Item locale strings", mItemLocaleMap.size());
    sLog.outString();
}

struct SQLItemLoader : public SQLStorageLoaderBase<SQLItemLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptDevAIMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadItemPrototypes()
{
    SQLItemLoader loader;
    loader.Load(sItemStorage);

    // check data correctness
    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype >(i);
        ItemEntry const* dbcitem = sItemStore.LookupEntry(i);
        if (!proto)
        {
            /* to many errors, and possible not all items really used in game
            if (dbcitem)
                sLog.outErrorDb("Item (Entry: %u) doesn't exists in DB, but must exist.",i);
            */
            continue;
        }

        for (const auto& Spell : proto->Spells)
            if (Spell.SpellCategory && Spell.SpellId)
            {
                if (sSpellTemplate.LookupEntry<SpellEntry>(Spell.SpellId))
                    sItemSpellCategoryStore[Spell.SpellCategory].insert(ItemCategorySpellPair(Spell.SpellId, i));
                else
                    sLog.outErrorDb("Item (Entry: %u) not correct %u spell id, must exist in spell table.", i, Spell.SpellId);
            }

        if (dbcitem)
        {
            if (proto->Class != dbcitem->Class)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct class %u, must be %u (still using DB value).", i, proto->Class, dbcitem->Class);
                // It safe let use Class from DB
            }
            /* disabled: have some strange wrong cases for Subclass values.
               for enable also uncomment Subclass field in ItemEntry structure and in Itemfmt[]
            if(proto->SubClass != dbcitem->SubClass)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct (Class: %u, Sub: %u) pair, must be (Class: %u, Sub: %u) (still using DB value).",i,proto->Class,proto->SubClass,dbcitem->Class,dbcitem->SubClass);
                // It safe let use Subclass from DB
            }
            */

            if (proto->Unk0 != dbcitem->Unk0)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %i Unk0, must be %i (still using DB value).", i, proto->Unk0, dbcitem->Unk0);
                // It safe let use Unk0 from DB
            }

            if (proto->Material != dbcitem->Material)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %i material, must be %i (still using DB value).", i, proto->Material, dbcitem->Material);
                // It safe let use Material from DB
            }

            if (proto->InventoryType != dbcitem->InventoryType)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u inventory type, must be %u (still using DB value).", i, proto->InventoryType, dbcitem->InventoryType);
                // It safe let use InventoryType from DB
            }

            if (proto->DisplayInfoID != dbcitem->DisplayId)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u display id, must be %u (using it).", i, proto->DisplayInfoID, dbcitem->DisplayId);
                const_cast<ItemPrototype*>(proto)->DisplayInfoID = dbcitem->DisplayId;
            }
            if (proto->Sheath != dbcitem->Sheath)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u sheath, must be %u  (using it).", i, proto->Sheath, dbcitem->Sheath);
                const_cast<ItemPrototype*>(proto)->Sheath = dbcitem->Sheath;
            }
        }
        else
        {
            sLog.outErrorDb("Item (Entry: %u) not correct (not listed in list of existing items).", i);
        }

        if (proto->Class >= MAX_ITEM_CLASS)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Class value (%u)", i, proto->Class);
            const_cast<ItemPrototype*>(proto)->Class = ITEM_CLASS_MISC;
        }

        if (proto->SubClass >= MaxItemSubclassValues[proto->Class])
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Subclass value (%u) for class %u", i, proto->SubClass, proto->Class);
            const_cast<ItemPrototype*>(proto)->SubClass = 0;// exist for all item classes
        }

        if (proto->Quality >= MAX_ITEM_QUALITY)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Quality value (%u)", i, proto->Quality);
            const_cast<ItemPrototype*>(proto)->Quality = ITEM_QUALITY_NORMAL;
        }

        if (proto->Flags2 & ITEM_FLAG2_FACTION_HORDE)
        {
            if (FactionEntry const* faction = sFactionStore.LookupEntry(HORDE))
                if ((proto->AllowableRace & faction->BaseRepRaceMask[0]) == 0)
                    sLog.outErrorDb("Item (Entry: %u) have in `AllowableRace` races (%u) only not compatible with ITEM_FLAG2_HORDE_ONLY (%u) in Flags field, item any way will can't be equipped or use by this races.",
                                    i, proto->AllowableRace, ITEM_FLAG2_FACTION_HORDE);

            if (proto->Flags2 & ITEM_FLAG2_FACTION_ALLIANCE)
                sLog.outErrorDb("Item (Entry: %u) have in `Flags2` flags ITEM_FLAG2_ALLIANCE_ONLY (%u) and ITEM_FLAG2_HORDE_ONLY (%u) in Flags field, this is wrong combination.",
                                i, ITEM_FLAG2_FACTION_ALLIANCE, ITEM_FLAG2_FACTION_HORDE);
        }
        else if (proto->Flags2 & ITEM_FLAG2_FACTION_ALLIANCE)
        {
            if (FactionEntry const* faction = sFactionStore.LookupEntry(ALLIANCE))
                if ((proto->AllowableRace & faction->BaseRepRaceMask[0]) == 0)
                    sLog.outErrorDb("Item (Entry: %u) have in `AllowableRace` races (%u) only not compatible with ITEM_FLAG2_ALLIANCE_ONLY (%u) in Flags field, item any way will can't be equipped or use by this races.",
                                    i, proto->AllowableRace, ITEM_FLAG2_FACTION_ALLIANCE);
        }

        if (proto->BuyCount <= 0)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong BuyCount value (%u), set to default(1).", i, proto->BuyCount);
            const_cast<ItemPrototype*>(proto)->BuyCount = 1;
        }

        if (proto->InventoryType >= MAX_INVTYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong InventoryType value (%u)", i, proto->InventoryType);
            const_cast<ItemPrototype*>(proto)->InventoryType = INVTYPE_NON_EQUIP;
        }

        if (proto->InventoryType != INVTYPE_NON_EQUIP)
        {
            if (proto->Flags & ITEM_FLAG_HAS_LOOT)
            {
                sLog.outErrorDb("Item container (Entry: %u) has not allowed for containers flag ITEM_FLAG_LOOTABLE (%u), flag removed.", i, ITEM_FLAG_HAS_LOOT);
                const_cast<ItemPrototype*>(proto)->Flags &= ~ITEM_FLAG_HAS_LOOT;
            }

            if (proto->Flags & ITEM_FLAG_IS_MILLABLE)
            {
                sLog.outErrorDb("Item container (Entry: %u) has not allowed for containers flag ITEM_FLAG_MILLABLE (%u), flag removed.", i, ITEM_FLAG_IS_MILLABLE);
                const_cast<ItemPrototype*>(proto)->Flags &= ~ITEM_FLAG_IS_MILLABLE;
            }

            if (proto->Flags & ITEM_FLAG_IS_PROSPECTABLE)
            {
                sLog.outErrorDb("Item container (Entry: %u) has not allowed for containers flag ITEM_FLAG_PROSPECTABLE (%u), flag removed.", i, ITEM_FLAG_IS_PROSPECTABLE);
                const_cast<ItemPrototype*>(proto)->Flags &= ~ITEM_FLAG_IS_PROSPECTABLE;
            }
        }
        else if (proto->InventoryType != INVTYPE_BAG)
        {
            if (proto->ContainerSlots > 0)
            {
                sLog.outErrorDb("Non-container item (Entry: %u) has ContainerSlots (%u), set to 0.", i, proto->ContainerSlots);
                const_cast<ItemPrototype*>(proto)->ContainerSlots = 0;
            }
        }

        if (proto->RequiredSkill >= MAX_SKILL_TYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RequiredSkill value (%u)", i, proto->RequiredSkill);
            const_cast<ItemPrototype*>(proto)->RequiredSkill = 0;
        }

        {
            // can be used in equip slot, as page read use in inventory, or spell casting at use
            bool req = proto->InventoryType != INVTYPE_NON_EQUIP || proto->PageText;
            if (!req)
            {
                for (const auto& Spell : proto->Spells)
                {
                    if (Spell.SpellId)
                    {
                        req = true;
                        break;
                    }
                }
            }

            if (req)
            {
                if (!(proto->AllowableClass & CLASSMASK_ALL_PLAYABLE))
                    sLog.outErrorDb("Item (Entry: %u) not have in `AllowableClass` any playable classes (%u) and can't be equipped or use.", i, proto->AllowableClass);

                if (!(proto->AllowableRace & RACEMASK_ALL_PLAYABLE))
                    sLog.outErrorDb("Item (Entry: %u) not have in `AllowableRace` any playable races (%u) and can't be equipped or use.", i, proto->AllowableRace);
            }
        }

        if (proto->RequiredSpell && !sSpellTemplate.LookupEntry<SpellEntry>(proto->RequiredSpell))
        {
            sLog.outErrorDb("Item (Entry: %u) have wrong (nonexistent) spell in RequiredSpell (%u)", i, proto->RequiredSpell);
            const_cast<ItemPrototype*>(proto)->RequiredSpell = 0;
        }

        if (proto->RequiredReputationRank >= MAX_REPUTATION_RANK)
            sLog.outErrorDb("Item (Entry: %u) has wrong reputation rank in RequiredReputationRank (%u), item can't be used.", i, proto->RequiredReputationRank);

        if (proto->RequiredReputationFaction)
        {
            if (!sFactionStore.LookupEntry(proto->RequiredReputationFaction))
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong (not existing) faction in RequiredReputationFaction (%u)", i, proto->RequiredReputationFaction);
                const_cast<ItemPrototype*>(proto)->RequiredReputationFaction = 0;
            }

            if (proto->RequiredReputationRank == MIN_REPUTATION_RANK)
                sLog.outErrorDb("Item (Entry: %u) has min. reputation rank in RequiredReputationRank (0) but RequiredReputationFaction > 0, faction setting is useless.", i);
        }
        else if (proto->RequiredReputationRank > MIN_REPUTATION_RANK)
            sLog.outErrorDb("Item (Entry: %u) has RequiredReputationFaction ==0 but RequiredReputationRank > 0, rank setting is useless.", i);

        if (proto->MaxCount < -1)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large negative in maxcount (%i), replace by value (-1) no storing limits.", i, proto->MaxCount);
            const_cast<ItemPrototype*>(proto)->MaxCount = -1;
        }

        if (proto->Stackable == 0)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong value in stackable (%i), replace by default 1.", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 1;
        }
        else if (proto->Stackable < -1)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large negative in stackable (%i), replace by value (-1) no stacking limits.", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = -1;
        }
        else if (proto->Stackable > 1000)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large value in stackable (%u), replace by hardcoded upper limit (1000).", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 1000;
        }

        if (proto->ContainerSlots)
        {
            if (proto->ContainerSlots > MAX_BAG_SIZE)
            {
                sLog.outErrorDb("Item (Entry: %u) has too large value in ContainerSlots (%u), replace by hardcoded limit (%u).", i, proto->ContainerSlots, MAX_BAG_SIZE);
                const_cast<ItemPrototype*>(proto)->ContainerSlots = MAX_BAG_SIZE;
            }
        }

        if (proto->StatsCount > MAX_ITEM_PROTO_STATS)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large value in statscount (%u), replace by hardcoded limit (%u).", i, proto->StatsCount, MAX_ITEM_PROTO_STATS);
            const_cast<ItemPrototype*>(proto)->StatsCount = MAX_ITEM_PROTO_STATS;
        }

        for (int j = 0; j < MAX_ITEM_PROTO_STATS; ++j)
        {
            // for ItemStatValue != 0
            if (proto->ItemStat[j].ItemStatValue && proto->ItemStat[j].ItemStatType >= MAX_ITEM_MOD)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong stat_type%d (%u)", i, j + 1, proto->ItemStat[j].ItemStatType);
                const_cast<ItemPrototype*>(proto)->ItemStat[j].ItemStatType = 0;
            }

            switch (proto->ItemStat[j].ItemStatType)
            {
                case ITEM_MOD_SPELL_HEALING_DONE:
                case ITEM_MOD_SPELL_DAMAGE_DONE:
                    sLog.outErrorDb("Item (Entry: %u) has deprecated stat_type%d (%u)", i, j + 1, proto->ItemStat[j].ItemStatType);
                    break;
                default:
                    break;
            }
        }

        for (int j = 0; j < MAX_ITEM_PROTO_DAMAGES; ++j)
        {
            if (proto->Damage[j].DamageType >= MAX_SPELL_SCHOOL)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong dmg_type%d (%u)", i, j + 1, proto->Damage[j].DamageType);
                const_cast<ItemPrototype*>(proto)->Damage[j].DamageType = 0;
            }
        }

        // special format
        if ((proto->Spells[0].SpellId == SPELL_ID_GENERIC_LEARN) || (proto->Spells[0].SpellId == SPELL_ID_GENERIC_LEARN_PET))
        {
            // spell_1
            if (proto->Spells[0].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u) for special learning format", i, 0 + 1, proto->Spells[0].SpellTrigger);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }

            // spell_2 have learning spell
            if (proto->Spells[1].SpellTrigger != ITEM_SPELLTRIGGER_LEARN_SPELL_ID)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u) for special learning format.", i, 1 + 1, proto->Spells[1].SpellTrigger);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }
            else if (!proto->Spells[1].SpellId)
            {
                sLog.outErrorDb("Item (Entry: %u) not has expected spell in spellid_%d in special learning format.", i, 1 + 1);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }
            else
            {
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(proto->Spells[1].SpellId);
                if (!spellInfo)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong (not existing) spell in spellid_%d (%u)", i, 1 + 1, proto->Spells[1].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                // allowed only in special format
                else if ((proto->Spells[1].SpellId == SPELL_ID_GENERIC_LEARN) || (proto->Spells[1].SpellId == SPELL_ID_GENERIC_LEARN_PET))
                {
                    sLog.outErrorDb("Item (Entry: %u) has broken spell in spellid_%d (%u)", i, 1 + 1, proto->Spells[1].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
            }

            // spell_3*,spell_4*,spell_5* is empty
            for (int j = 2; j < MAX_ITEM_PROTO_SPELLS; ++j)
            {
                if (proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u)", i, j + 1, proto->Spells[j].SpellTrigger);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                else if (proto->Spells[j].SpellId != 0)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong spell in spellid_%d (%u) for learning special format", i, j + 1, proto->Spells[j].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                }
            }
        }
        // normal spell list
        else
        {
            for (int j = 0; j < MAX_ITEM_PROTO_SPELLS; ++j)
            {
                if (proto->Spells[j].SpellTrigger >= MAX_ITEM_SPELLTRIGGER || proto->Spells[j].SpellTrigger == ITEM_SPELLTRIGGER_LEARN_SPELL_ID)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u)", i, j + 1, proto->Spells[j].SpellTrigger);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                // on hit can be sued only at weapon
                else if (proto->Spells[j].SpellTrigger == ITEM_SPELLTRIGGER_CHANCE_ON_HIT)
                {
                    if (proto->Class != ITEM_CLASS_WEAPON)
                        sLog.outErrorDb("Item (Entry: %u) isn't weapon (Class: %u) but has on hit spelltrigger_%d (%u), it will not triggered.", i, proto->Class, j + 1, proto->Spells[j].SpellTrigger);
                }

                if (proto->Spells[j].SpellId)
                {
                    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(proto->Spells[j].SpellId);
                    if (!spellInfo)
                    {
                        sLog.outErrorDb("Item (Entry: %u) has wrong (not existing) spell in spellid_%d (%u)", i, j + 1, proto->Spells[j].SpellId);
                        const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    }
                    // allowed only in special format
                    else if ((proto->Spells[j].SpellId == SPELL_ID_GENERIC_LEARN) || (proto->Spells[j].SpellId == SPELL_ID_GENERIC_LEARN_PET))
                    {
                        sLog.outErrorDb("Item (Entry: %u) has broken spell in spellid_%d (%u)", i, j + 1, proto->Spells[j].SpellId);
                        const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    }

                    else if (spellInfo->speed > 0 && proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE && proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_CHANCE_ON_HIT)
                        sLog.outErrorDb("Item (Entry: %u) spell %u %s has travel speed.", i, spellInfo->Id, spellInfo->SpellName[0]);
                }
            }
        }

        if (proto->Bonding >= MAX_BIND_TYPE)
            sLog.outErrorDb("Item (Entry: %u) has wrong Bonding value (%u)", i, proto->Bonding);

        if (proto->PageText)
        {
            if (!sPageTextStore.LookupEntry<PageText>(proto->PageText))
                sLog.outErrorDb("Item (Entry: %u) has non existing first page (Id:%u)", i, proto->PageText);
        }

        if (proto->LockID && !sLockStore.LookupEntry(proto->LockID))
            sLog.outErrorDb("Item (Entry: %u) has wrong LockID (%u)", i, proto->LockID);

        if (proto->Sheath >= MAX_SHEATHETYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Sheath (%u)", i, proto->Sheath);
            const_cast<ItemPrototype*>(proto)->Sheath = SHEATHETYPE_NONE;
        }

        if (proto->RandomProperty && !sItemRandomPropertiesStore.LookupEntry(GetItemEnchantMod(proto->RandomProperty)))
        {
            sLog.outErrorDb("Item (Entry: %u) has unknown (wrong or not listed in `item_enchantment_template`) RandomProperty (%u)", i, proto->RandomProperty);
            const_cast<ItemPrototype*>(proto)->RandomProperty = 0;
        }

        if (proto->RandomSuffix && !sItemRandomSuffixStore.LookupEntry(GetItemEnchantMod(proto->RandomSuffix)))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RandomSuffix (%u)", i, proto->RandomSuffix);
            const_cast<ItemPrototype*>(proto)->RandomSuffix = 0;
        }

        // item can have not null only one from field values
        if (proto->RandomProperty && proto->RandomSuffix)
        {
            sLog.outErrorDb("Item (Entry: %u) have RandomProperty==%u and RandomSuffix==%u, but must have one from field = 0",
                            proto->ItemId, proto->RandomProperty, proto->RandomSuffix);
            const_cast<ItemPrototype*>(proto)->RandomSuffix = 0;
        }

        if (proto->ItemSet && !sItemSetStore.LookupEntry(proto->ItemSet))
        {
            sLog.outErrorDb("Item (Entry: %u) have wrong ItemSet (%u)", i, proto->ItemSet);
            const_cast<ItemPrototype*>(proto)->ItemSet = 0;
        }

        if (proto->Area && !GetAreaEntryByAreaID(proto->Area))
            sLog.outErrorDb("Item (Entry: %u) has wrong Area (%u)", i, proto->Area);

        if (proto->Map && !sMapStore.LookupEntry(proto->Map))
            sLog.outErrorDb("Item (Entry: %u) has wrong Map (%u)", i, proto->Map);

        if (proto->BagFamily)
        {
            // check bits
            for (uint32 j = 0; j < sizeof(proto->BagFamily) * 8; ++j)
            {
                uint32 mask = 1 << j;
                if (!(proto->BagFamily & mask))
                    continue;

                ItemBagFamilyEntry const* bf = sItemBagFamilyStore.LookupEntry(j + 1);
                if (!bf)
                {
                    sLog.outErrorDb("Item (Entry: %u) has bag family bit set not listed in ItemBagFamily.dbc, remove bit", i);
                    const_cast<ItemPrototype*>(proto)->BagFamily &= ~mask;
                    continue;
                }

                if (BAG_FAMILY_MASK_CURRENCY_TOKENS & mask)
                {
                    CurrencyTypesEntry const* ctEntry = sCurrencyTypesStore.LookupEntry(proto->ItemId);
                    if (!ctEntry)
                    {
                        sLog.outErrorDb("Item (Entry: %u) has currency bag family bit set in BagFamily but not listed in CurrencyTypes.dbc, remove bit", i);
                        const_cast<ItemPrototype*>(proto)->BagFamily &= ~mask;
                    }
                }
            }
        }

        if (proto->TotemCategory && !sTotemCategoryStore.LookupEntry(proto->TotemCategory))
            sLog.outErrorDb("Item (Entry: %u) has wrong TotemCategory (%u)", i, proto->TotemCategory);

        for (int j = 0; j < MAX_ITEM_PROTO_SOCKETS; ++j)
        {
            if (proto->Socket[j].Color && (proto->Socket[j].Color & SOCKET_COLOR_ALL) != proto->Socket[j].Color)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong socketColor_%d (%u)", i, j + 1, proto->Socket[j].Color);
                const_cast<ItemPrototype*>(proto)->Socket[j].Color = 0;
            }
        }

        if (proto->GemProperties && !sGemPropertiesStore.LookupEntry(proto->GemProperties))
            sLog.outErrorDb("Item (Entry: %u) has wrong GemProperties (%u)", i, proto->GemProperties);

        if (proto->RequiredDisenchantSkill < -1)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RequiredDisenchantSkill (%i), set to (-1).", i, proto->RequiredDisenchantSkill);
            const_cast<ItemPrototype*>(proto)->RequiredDisenchantSkill = -1;
        }
        else if (proto->RequiredDisenchantSkill != -1)
        {
            if (proto->Quality > ITEM_QUALITY_EPIC || proto->Quality < ITEM_QUALITY_UNCOMMON)
            {
                ERROR_DB_STRICT_LOG("Item (Entry: %u) has unexpected RequiredDisenchantSkill (%u) for non-disenchantable quality (%u), reset it.",
                                    i, proto->RequiredDisenchantSkill, proto->Quality);
                const_cast<ItemPrototype*>(proto)->RequiredDisenchantSkill = -1;
            }
            else if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR)
            {
                // some wrong data in wdb for unused items
                ERROR_DB_STRICT_LOG("Item (Entry: %u) has unexpected RequiredDisenchantSkill (%u) for non-disenchantable item class (%u), reset it.",
                                    i, proto->RequiredDisenchantSkill, proto->Class);
                const_cast<ItemPrototype*>(proto)->RequiredDisenchantSkill = -1;
            }
        }

        if (proto->DisenchantID)
        {
            if (proto->Quality > ITEM_QUALITY_EPIC || proto->Quality < ITEM_QUALITY_UNCOMMON)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong quality (%u) for disenchanting, remove disenchanting loot id.", i, proto->Quality);
                const_cast<ItemPrototype*>(proto)->DisenchantID = 0;
            }
            else if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item class (%u) for disenchanting, remove disenchanting loot id.", i, proto->Class);
                const_cast<ItemPrototype*>(proto)->DisenchantID = 0;
            }
            else if (proto->RequiredDisenchantSkill < 0)
            {
                sLog.outErrorDb("Item (Entry: %u) marked as non-disenchantable by RequiredDisenchantSkill == -1, remove disenchanting loot id.", i);
                const_cast<ItemPrototype*>(proto)->DisenchantID = 0;
            }
        }
        else
        {
            // lot DB cases
            if (proto->RequiredDisenchantSkill >= 0)
                ERROR_DB_STRICT_LOG("Item (Entry: %u) marked as disenchantable by RequiredDisenchantSkill, but not have disenchanting loot id.", i);
        }

        if (proto->FoodType >= MAX_PET_DIET)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong FoodType value (%u)", i, proto->FoodType);
            const_cast<ItemPrototype*>(proto)->FoodType = 0;
        }

        if (proto->ItemLimitCategory && !sItemLimitCategoryStore.LookupEntry(proto->ItemLimitCategory))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong LimitCategory value (%u)", i, proto->ItemLimitCategory);
            const_cast<ItemPrototype*>(proto)->ItemLimitCategory = 0;
        }

        if (proto->HolidayId && !sHolidaysStore.LookupEntry(proto->HolidayId))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong HolidayId value (%u)", i, proto->HolidayId);
            const_cast<ItemPrototype*>(proto)->HolidayId = 0;
        }

        if (proto->ExtraFlags)
        {
            if (proto->ExtraFlags & ~ITEM_EXTRA_ALL)
                sLog.outErrorDb("Item (Entry: %u) has wrong ExtraFlags (%u) with unused bits set", i, proto->ExtraFlags);

            if (proto->ExtraFlags & ITEM_EXTRA_REAL_TIME_DURATION)
            {
                if (proto->Duration == 0)
                {
                    sLog.outErrorDb("Item (Entry: %u) has redundant real-time duration flag in ExtraFlags, item not have duration", i);
                    const_cast<ItemPrototype*>(proto)->ExtraFlags &= ~ITEM_EXTRA_REAL_TIME_DURATION;
                }
            }
        }
    }

    // check some dbc referenced items (avoid duplicate reports)
    std::set<uint32> notFoundOutfit;
    for (uint32 i = 1; i < sCharStartOutfitStore.GetNumRows(); ++i)
    {
        CharStartOutfitEntry const* entry = sCharStartOutfitStore.LookupEntry(i);
        if (!entry)
            continue;

        for (int j : entry->ItemId)
        {
            if (j <= 0)
                continue;

            uint32 item_id = j;

            if (!GetItemPrototype(item_id))
                if (item_id != 40582)                       // nonexistent item by default but referenced in DBC, skip it from errors
                    notFoundOutfit.insert(item_id);
        }
    }

    for (uint32 itr : notFoundOutfit)
    sLog.outErrorDb("Item (Entry: %u) not exist in `item_template` but referenced in `CharStartOutfit.dbc`", itr);

    sLog.outString(">> Loaded %u item prototypes", sItemStorage.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::LoadItemConverts()
{
    m_ItemConvert.clear();                                  // needed for reload case

    uint32 count = 0;

    auto queryResult = WorldDatabase.Query("SELECT entry,item FROM item_convert");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 Item converts . DB table `item_convert` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 itemEntry    = fields[0].GetUInt32();
        uint32 itemTargetId = fields[1].GetUInt32();

        ItemPrototype const* pItemEntryProto = sItemStorage.LookupEntry<ItemPrototype>(itemEntry);
        if (!pItemEntryProto)
        {
            sLog.outErrorDb("Table `item_convert`: Item %u not exist in `item_template`.", itemEntry);
            continue;
        }

        ItemPrototype const* pItemTargetProto = sItemStorage.LookupEntry<ItemPrototype>(itemTargetId);
        if (!pItemTargetProto)
        {
            sLog.outErrorDb("Table `item_convert`: Item target %u for original item %u not exist in `item_template`.", itemTargetId, itemEntry);
            continue;
        }

        // 2 cases when item convert used
        // Boa item with reputation requirement
        if ((!(pItemEntryProto->Flags & ITEM_FLAG_IS_BOUND_TO_ACCOUNT) || !pItemEntryProto->RequiredReputationFaction) &&
                // convertion to another team/race
                (pItemTargetProto->AllowableRace & pItemEntryProto->AllowableRace))
        {
            sLog.outErrorDb("Table `item_convert` not appropriate item %u conversion to %u. Table can be used for BoA items requirement drop or for conversion to another race/team use.", itemEntry, itemTargetId);
            continue;
        }

        m_ItemConvert[itemEntry] = itemTargetId;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u Item converts", count);
    sLog.outString();
}

void ObjectMgr::LoadItemExpireConverts()
{
    m_ItemExpireConvert.clear();                            // needed for reload case

    uint32 count = 0;

    auto queryResult = WorldDatabase.Query("SELECT entry,item FROM item_expire_convert");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 Item expire converts . DB table `item_expire_convert` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 itemEntry    = fields[0].GetUInt32();
        uint32 itemTargetId = fields[1].GetUInt32();

        ItemPrototype const* pItemEntryProto = sItemStorage.LookupEntry<ItemPrototype>(itemEntry);
        if (!pItemEntryProto)
        {
            sLog.outErrorDb("Table `item_expire_convert`: Item %u not exist in `item_template`.", itemEntry);
            continue;
        }

        ItemPrototype const* pItemTargetProto = sItemStorage.LookupEntry<ItemPrototype>(itemTargetId);
        if (!pItemTargetProto)
        {
            sLog.outErrorDb("Table `item_expire_convert`: Item target %u for original item %u not exist in `item_template`.", itemTargetId, itemEntry);
            continue;
        }

        // Expire convert possible only for items with duration
        if (pItemEntryProto->Duration == 0)
        {
            sLog.outErrorDb("Table `item_expire_convert` not appropriate item %u conversion to %u. Table can be used for items with duration.", itemEntry, itemTargetId);
            continue;
        }

        m_ItemExpireConvert[itemEntry] = itemTargetId;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u Item expire converts", count);
    sLog.outString();
}

void ObjectMgr::LoadItemRequiredTarget()
{
    m_ItemRequiredTarget.clear();                           // needed for reload case

    uint32 count = 0;

    auto queryResult = WorldDatabase.Query("SELECT entry,type,targetEntry FROM item_required_target");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 ItemRequiredTarget. DB table `item_required_target` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 uiItemId      = fields[0].GetUInt32();
        uint32 uiType        = fields[1].GetUInt32();
        uint32 uiTargetEntry = fields[2].GetUInt32();

        ItemPrototype const* pItemProto = sItemStorage.LookupEntry<ItemPrototype>(uiItemId);

        if (!pItemProto)
        {
            sLog.outErrorDb("Table `item_required_target`: Entry %u listed for TargetEntry %u does not exist in `item_template`.", uiItemId, uiTargetEntry);
            continue;
        }

        bool bIsItemSpellValid = false;

        for (const auto& Spell : pItemProto->Spells)
        {
            if (SpellEntry const* pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(Spell.SpellId))
            {
                if (Spell.SpellTrigger == ITEM_SPELLTRIGGER_ON_USE)
                {
                    SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(pSpellInfo->Id);
                    if (bounds.first != bounds.second)
                        break;

                    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
                    {
                        if (pSpellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_ENEMY ||
                                pSpellInfo->EffectImplicitTargetB[j] == TARGET_UNIT_ENEMY ||
                                pSpellInfo->EffectImplicitTargetA[j] == TARGET_UNIT ||
                                pSpellInfo->EffectImplicitTargetB[j] == TARGET_UNIT)
                        {
                            bIsItemSpellValid = true;
                            break;
                        }
                    }
                    if (bIsItemSpellValid)
                        break;
                }
            }
        }

        if (!bIsItemSpellValid)
        {
            sLog.outErrorDb("Table `item_required_target`: Spell used by item %u does not have implicit target TARGET_UNIT_ENEMY(6), TARGET_UNIT(25), already listed in `spell_script_target` or doesn't have item spelltrigger.", uiItemId);
            continue;
        }

        if (!uiType || uiType > MAX_ITEM_REQ_TARGET_TYPE)
        {
            sLog.outErrorDb("Table `item_required_target`: Type %u for TargetEntry %u is incorrect.", uiType, uiTargetEntry);
            continue;
        }

        if (!uiTargetEntry)
        {
            sLog.outErrorDb("Table `item_required_target`: TargetEntry == 0 for Type (%u).", uiType);
            continue;
        }

        if (!sCreatureStorage.LookupEntry<CreatureInfo>(uiTargetEntry))
        {
            sLog.outErrorDb("Table `item_required_target`: creature template entry %u does not exist.", uiTargetEntry);
            continue;
        }

        m_ItemRequiredTarget.insert(ItemRequiredTargetMap::value_type(uiItemId, ItemRequiredTarget(ItemRequiredTargetType(uiType), uiTargetEntry)));

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u Item required targets", count);
    sLog.outString();
}

void ObjectMgr::LoadPetLevelInfo()
{
    // Loading levels data
    //                                              0               1      2   3     4    5    6    7     8    9
    auto queryResult  = WorldDatabase.Query("SELECT creature_entry, level, hp, mana, str, agi, sta, inte, spi, armor FROM pet_levelstats");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u level pet stats definitions", count);
        sLog.outErrorDb("Error loading `pet_levelstats` table or empty table.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        uint32 creature_id = fields[0].GetUInt32();
        if (!sCreatureStorage.LookupEntry<CreatureInfo>(creature_id))
        {
            sLog.outErrorDb("Wrong creature id %u in `pet_levelstats` table, ignoring.", creature_id);
            continue;
        }

        uint32 current_level = fields[1].GetUInt32();
        if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        {
            if (current_level > STRONG_MAX_LEVEL)       // hardcoded level maximum
                sLog.outErrorDb("Wrong (> %u) level %u in `pet_levelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
            else
            {
                DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `pet_levelstats` table, ignoring.", current_level);
                ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
            }
            continue;
        }
        if (current_level < 1)
        {
            sLog.outErrorDb("Wrong (<1) level %u in `pet_levelstats` table, ignoring.", current_level);
            continue;
        }

        PetLevelInfo*& pInfoMapEntry = petInfo[creature_id];

        if (pInfoMapEntry == nullptr)
            pInfoMapEntry =  new PetLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

        // data for level 1 stored in [0] array element, ...
        PetLevelInfo* pLevelInfo = &pInfoMapEntry[current_level - 1];

        pLevelInfo->health = fields[2].GetUInt16();
        pLevelInfo->mana   = fields[3].GetUInt16();
        pLevelInfo->armor  = fields[9].GetUInt16();

        for (int i = 0; i < MAX_STATS; ++i)
        {
            pLevelInfo->stats[i] = fields[i + 4].GetUInt16();
        }

        bar.step();
        ++count;
    }
    while (queryResult->NextRow());

    // Fill gaps and check integrity
    for (auto& itr : petInfo)
    {
        PetLevelInfo* pInfo = itr.second;

        // fatal error if no level 1 data
        if (!pInfo || pInfo[0].health == 0)
        {
            sLog.outErrorDb("Creature %u does not have pet stats data for Level 1!", itr.first);
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
        {
            if (pInfo[level].health == 0)
            {
                sLog.outErrorDb("Creature %u has no data for Level %i pet stats data, using data of Level %i.", itr.first, level + 1, level);
                pInfo[level] = pInfo[level - 1];
            }
        }
    }

    sLog.outString(">> Loaded %u level pet stats definitions", count);
    sLog.outString();
}

PetLevelInfo const* ObjectMgr::GetPetLevelInfo(uint32 creature_id, uint32 level) const
{
    if (level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    PetLevelInfoMap::const_iterator itr = petInfo.find(creature_id);
    if (itr == petInfo.end())
        return nullptr;

    return &itr->second[level - 1];                         // data for level 1 stored in [0] array element, ...
}

void ObjectMgr::LoadPlayerInfo()
{
    // Load playercreate
    {
        //                                             0     1      2    3     4           5           6
        auto queryResult = WorldDatabase.Query("SELECT race, class, map, zone, position_x, position_y, position_z, orientation FROM playercreateinfo");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u player create definitions", count);
            sLog.outErrorDb("Error loading `playercreateinfo` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        BarGoLink bar(queryResult->GetRowCount());

        do
        {
            Field* fields = queryResult->Fetch();

            uint32 current_race  = fields[0].GetUInt32();
            uint32 current_class = fields[1].GetUInt32();
            uint32 mapId         = fields[2].GetUInt32();
            uint32 areaId        = fields[3].GetUInt32();
            float  positionX     = fields[4].GetFloat();
            float  positionY     = fields[5].GetFloat();
            float  positionZ     = fields[6].GetFloat();
            float  orientation   = fields[7].GetFloat();

            ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
            if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                continue;
            }

            ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
            if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                continue;
            }

            // accept DB data only for valid position (and non instanceable)
            if (!MapManager::IsValidMapCoord(mapId, positionX, positionY, positionZ, orientation))
            {
                sLog.outErrorDb("Wrong home position for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                continue;
            }

            if (sMapStore.LookupEntry(mapId)->Instanceable())
            {
                sLog.outErrorDb("Home position in instanceable map for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                continue;
            }

            PlayerInfo* pInfo = &playerInfo[current_race][current_class];

            pInfo->mapId       = mapId;
            pInfo->areaId      = areaId;
            pInfo->positionX   = positionX;
            pInfo->positionY   = positionY;
            pInfo->positionZ   = positionZ;
            pInfo->orientation = orientation;

            pInfo->displayId_m = rEntry->model_m;
            pInfo->displayId_f = rEntry->model_f;

            bar.step();
            ++count;
        }
        while (queryResult->NextRow());

        sLog.outString();
        sLog.outString(">> Loaded %u player create definitions", count);
    }

    // Load playercreate items
    {
        //                                             0     1      2       3
        auto queryResult = WorldDatabase.Query("SELECT race, class, itemid, amount FROM playercreateinfo_item");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            bar.step();

            sLog.outString();
            sLog.outString(">> Loaded %u custom player create items", count);
        }
        else
        {
            BarGoLink bar(queryResult->GetRowCount());

            do
            {
                Field* fields = queryResult->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_item` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_item` table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];

                uint32 item_id = fields[2].GetUInt32();

                if (!GetItemPrototype(item_id))
                {
                    sLog.outErrorDb("Item id %u (race %u class %u) in `playercreateinfo_item` table but not listed in `item_template`, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                uint32 amount  = fields[3].GetUInt32();

                if (!amount)
                {
                    sLog.outErrorDb("Item id %u (class %u race %u) have amount==0 in `playercreateinfo_item` table, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                pInfo->item.push_back(PlayerCreateInfoItem(item_id, amount));

                bar.step();
                ++count;
            }
            while (queryResult->NextRow());

            sLog.outString();
            sLog.outString(">> Loaded %u custom player create items", count);
        }
    }

    // Load playercreate skills
    {
        //
        auto queryResult = WorldDatabase.Query("SELECT raceMask, classMask, skill, step FROM playercreateinfo_skills");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u player create skills", count);
            sLog.outErrorDb("Error loading `playercreateinfo_skills` table or empty table.");
        }
        else
        {
            BarGoLink bar(queryResult->GetRowCount());

            do
            {
                Field* fields = queryResult->Fetch();
                uint32 raceMask = fields[0].GetUInt32();
                uint32 classMask = fields[1].GetUInt32();
                PlayerCreateInfoSkill skill;
                skill.SkillId = fields[2].GetUInt16();
                skill.Step = fields[3].GetUInt16();

                if (skill.Step > MAX_SKILL_STEP)
                {
                    sLog.outErrorDb("Skill step %u out of bounds in `playercreateinfo_skills` table, ignoring.", skill.Step);
                    continue;
                }

                if (raceMask && !(raceMask & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race mask %u in `playercreateinfo_skills` table, ignoring.", raceMask);
                    continue;
                }

                if (classMask && !(classMask & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class mask %u in `playercreateinfo_skills` table, ignoring.", classMask);
                    continue;
                }

                if (!sSkillLineStore.LookupEntry(skill.SkillId))
                {
                    sLog.outErrorDb("Non existing skill %u in `playercreateinfo_skills` table, ignoring.", skill.SkillId);
                    continue;
                }

                auto bounds = sSpellMgr.GetSkillRaceClassInfoMapBounds(skill.SkillId);

                for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
                {
                    const uint32 raceIndexMask = (1 << (raceIndex - 1));
                    if (!raceMask || (raceMask & raceIndexMask))
                    {
                        for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                        {
                            const uint32 classIndexMask = (1 << (classIndex - 1));
                            if (!classMask || (classMask & classIndexMask))
                            {
                                bool obtainable = false;

                                for (auto itr = bounds.first; (itr != bounds.second && !obtainable); ++itr)
                                {
                                    SkillRaceClassInfoEntry const* entry = itr->second;

                                    if (!(entry->raceMask & raceIndexMask))
                                        continue;

                                    if (!(entry->classMask & classIndexMask))
                                        continue;

                                    if (skill.Step)
                                    {
                                        const uint32 stepIndex = (skill.Step - 1);
                                        SkillTiersEntry const* steps = sSkillTiersStore.LookupEntry(entry->skillTierId);

                                        if (!steps || !steps->maxSkillValue[stepIndex])
                                            continue;
                                    }

                                    obtainable = true;
                                }

                                if (!obtainable)
                                    continue;

                                if (PlayerInfo* info = &playerInfo[raceIndex][classIndex])
                                {
                                    info->skill.push_back(skill);
                                    ++count;
                                }
                            }
                        }
                    }
                }
            }
            while (queryResult->NextRow());

            sLog.outString();
            sLog.outString(">> Loaded %u player create skills", count);
        }
    }

    // Load playercreate spells
    {
        //                                             0     1      2
        auto queryResult = WorldDatabase.Query("SELECT race, class, Spell FROM playercreateinfo_spell");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u player create spells", count);
            sLog.outErrorDb("Error loading `playercreateinfo_spell` table or empty table.");
        }
        else
        {
            BarGoLink bar(queryResult->GetRowCount());

            do
            {
                Field* fields = queryResult->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_spell` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_spell` table, ignoring.", current_class);
                    continue;
                }

                uint32 spell_id = fields[2].GetUInt32();
                if (!sSpellTemplate.LookupEntry<SpellEntry>(spell_id))
                {
                    sLog.outErrorDb("Non existing spell %u in `playercreateinfo_spell` table, ignoring.", spell_id);
                    continue;
                }

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];
                pInfo->spell.push_back(spell_id);

                bar.step();
                ++count;
            }
            while (queryResult->NextRow());

            sLog.outString();
            sLog.outString(">> Loaded %u player create spells", count);
        }
    }

    // Load playercreate actions
    {
        //                                             0     1      2       3       4
        auto queryResult = WorldDatabase.Query("SELECT race, class, button, action, type FROM playercreateinfo_action");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u player create actions", count);
            sLog.outErrorDb("Error loading `playercreateinfo_action` table or empty table.");
        }
        else
        {
            BarGoLink bar(queryResult->GetRowCount());

            do
            {
                Field* fields = queryResult->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_action` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_action` table, ignoring.", current_class);
                    continue;
                }

                uint8 action_button  = fields[2].GetUInt8();
                uint32 action = fields[3].GetUInt32();
                uint8 action_type = fields[4].GetUInt8();

                if (!Player::IsActionButtonDataValid(action_button, action, action_type, nullptr))
                    continue;

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];
                pInfo->action.push_back(PlayerCreateInfoAction(action_button, action, action_type));

                bar.step();
                ++count;
            }
            while (queryResult->NextRow());

            sLog.outString();
            sLog.outString(">> Loaded %u player create actions", count);
        }
    }

    // Loading levels data (class only dependent)
    {
        //                                              0      1      2       3
        auto queryResult  = WorldDatabase.Query("SELECT class, level, basehp, basemana FROM player_classlevelstats");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u level health/mana definitions", count);
            sLog.outErrorDb("Error loading `player_classlevelstats` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        BarGoLink bar(queryResult->GetRowCount());

        do
        {
            Field* fields = queryResult->Fetch();

            uint32 current_class = fields[0].GetUInt32();
            if (current_class >= MAX_CLASSES)
            {
                sLog.outErrorDb("Wrong class %u in `player_classlevelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[1].GetUInt32();
            if (current_level == 0)
            {
                sLog.outErrorDb("Wrong level %u in `player_classlevelstats` table, ignoring.", current_level);
                continue;
            }
            if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)       // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_classlevelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_classlevelstats` table, ignoring.", current_level);
                    ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }

            PlayerClassInfo* pClassInfo = &playerClassInfo[current_class];

            if (!pClassInfo->levelInfo)
                pClassInfo->levelInfo = new PlayerClassLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

            PlayerClassLevelInfo* pClassLevelInfo = &pClassInfo->levelInfo[current_level - 1];

            pClassLevelInfo->basehealth = fields[2].GetUInt16();
            pClassLevelInfo->basemana   = fields[3].GetUInt16();

            bar.step();
            ++count;
        }
        while (queryResult->NextRow());

        sLog.outString();
        sLog.outString(">> Loaded %u level health/mana definitions", count);
    }

    // Fill gaps and check integrity
    for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
    {
        // skip nonexistent classes
        if (!sChrClassesStore.LookupEntry(class_))
            continue;

        PlayerClassInfo* pClassInfo = &playerClassInfo[class_];

        // fatal error if no level 1 data
        if (!pClassInfo->levelInfo || pClassInfo->levelInfo[0].basehealth == 0)
        {
            sLog.outErrorDb("Class %i Level 1 does not have health/mana data!", class_);
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
        {
            if (pClassInfo->levelInfo[level].basehealth == 0)
            {
                sLog.outErrorDb("Class %i Level %i does not have health/mana data. Using stats data of level %i.", class_, level + 1, level);
                pClassInfo->levelInfo[level] = pClassInfo->levelInfo[level - 1];
            }
        }
    }

    // Loading levels data (class/race dependent)
    {
        //                                              0     1      2      3    4    5    6     7
        auto queryResult  = WorldDatabase.Query("SELECT race, class, level, str, agi, sta, inte, spi FROM player_levelstats");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u level stats definitions", count);
            sLog.outErrorDb("Error loading `player_levelstats` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        BarGoLink bar(queryResult->GetRowCount());

        do
        {
            Field* fields = queryResult->Fetch();

            uint32 current_race = fields[0].GetUInt32();
            uint32 current_class = fields[1].GetUInt32();

            ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
            if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong race %u in `player_levelstats` table, ignoring.", current_race);
                continue;
            }

            ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
            if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong class %u in `player_levelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[2].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)       // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_levelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_levelstats` table, ignoring.", current_level);
                    ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }

            PlayerInfo* pInfo = &playerInfo[current_race][current_class];

            if (!pInfo->levelInfo)
                pInfo->levelInfo = new PlayerLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

            PlayerLevelInfo* pLevelInfo = &pInfo->levelInfo[current_level - 1];

            for (int i = 0; i < MAX_STATS; ++i)
                pLevelInfo->stats[i] = fields[i + 3].GetUInt8();

            bar.step();
            ++count;
        }
        while (queryResult->NextRow());

        sLog.outString();
        sLog.outString(">> Loaded %u level stats definitions", count);
    }

    // Fill gaps and check integrity
    for (int race = 1; race < MAX_RACES; ++race)
    {
        // skip nonexistent races
        if (!((1 << (race - 1)) & RACEMASK_ALL_PLAYABLE) || !sChrRacesStore.LookupEntry(race))
            continue;

        for (int class_ = 1; class_ < MAX_CLASSES; ++class_)
        {
            // skip nonexistent classes
            if (!((1 << (class_ - 1)) & CLASSMASK_ALL_PLAYABLE) || !sChrClassesStore.LookupEntry(class_))
                continue;

            PlayerInfo* pInfo = &playerInfo[race][class_];

            // skip non loaded combinations
            if (!pInfo->displayId_m || !pInfo->displayId_f)
                continue;

            // skip expansion races if not playing with expansion
            if (sWorld.getConfig(CONFIG_UINT32_EXPANSION) < EXPANSION_TBC && (race == RACE_BLOODELF || race == RACE_DRAENEI))
                continue;

            // skip expansion classes if not playing with expansion
            if (sWorld.getConfig(CONFIG_UINT32_EXPANSION) < EXPANSION_WOTLK && class_ == CLASS_DEATH_KNIGHT)
                continue;

            // fatal error if no level 1 data
            if (!pInfo->levelInfo || pInfo->levelInfo[0].stats[0] == 0)
            {
                sLog.outErrorDb("Race %i Class %i Level 1 does not have stats data!", race, class_);
                Log::WaitBeforeContinueIfNeed();
                exit(1);
            }

            // fill level gaps
            for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
            {
                if (pInfo->levelInfo[level].stats[0] == 0)
                {
                    sLog.outErrorDb("Race %i Class %i Level %i does not have stats data. Using stats data of level %i.", race, class_, level + 1, level);
                    pInfo->levelInfo[level] = pInfo->levelInfo[level - 1];
                }
            }
        }
    }

    // Loading xp per level data
    {
        mPlayerXPperLevel.resize(sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));
        for (uint32 level = 0; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
            mPlayerXPperLevel[level] = 0;

        //                                              0    1
        auto queryResult  = WorldDatabase.Query("SELECT lvl, xp_for_next_level FROM player_xp_for_level");

        uint32 count = 0;

        if (!queryResult)
        {
            BarGoLink bar(1);

            sLog.outString();
            sLog.outString(">> Loaded %u xp for level definitions", count);
            sLog.outErrorDb("Error loading `player_xp_for_level` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        BarGoLink bar(queryResult->GetRowCount());

        do
        {
            Field* fields = queryResult->Fetch();

            uint32 current_level = fields[0].GetUInt32();
            uint32 current_xp    = fields[1].GetUInt32();

            if (current_level >= sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)       // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_xp_for_level` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_xp_for_levels` table, ignoring.", current_level);
                    ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }
            // PlayerXPperLevel
            mPlayerXPperLevel[current_level] = current_xp;
            bar.step();
            ++count;
        }
        while (queryResult->NextRow());

        sLog.outString();
        sLog.outString(">> Loaded %u xp for level definitions", count);
    }

    // fill level gaps
    for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
    {
        if (mPlayerXPperLevel[level] == 0)
        {
            sLog.outErrorDb("Level %i does not have XP for level data. Using data of level [%i] + 100.", level + 1, level);
            mPlayerXPperLevel[level] = mPlayerXPperLevel[level - 1] + 100;
        }
    }
}

void ObjectMgr::GetPlayerClassLevelInfo(uint32 class_, uint32 level, PlayerClassLevelInfo* info) const
{
    if (level < 1 || class_ >= MAX_CLASSES)
        return;

    PlayerClassInfo const* pInfo = &playerClassInfo[class_];

    if (level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    *info = pInfo->levelInfo[level - 1];
}

void ObjectMgr::GetPlayerLevelInfo(uint32 race, uint32 class_, uint32 level, PlayerLevelInfo* info) const
{
    if (level < 1 || race   >= MAX_RACES || class_ >= MAX_CLASSES)
        return;

    PlayerInfo const* pInfo = &playerInfo[race][class_];
    if (pInfo->displayId_m == 0 || pInfo->displayId_f == 0)
        return;

    if (level <= sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        *info = pInfo->levelInfo[level - 1];
    else
        BuildPlayerLevelInfo(race, class_, level, info);
}

void ObjectMgr::BuildPlayerLevelInfo(uint8 race, uint8 _class, uint8 level, PlayerLevelInfo* info) const
{
    // base data (last known level)
    *info = playerInfo[race][_class].levelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL) - 1];

    for (int lvl = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL) - 1; lvl < level; ++lvl)
    {
        switch (_class)
        {
            case CLASS_WARRIOR:
                info->stats[STAT_STRENGTH]  += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 36 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                break;
            case CLASS_PALADIN:
                info->stats[STAT_STRENGTH]  += (lvl > 3  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 7 && !(lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 6 && (lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 7 ? 1 : 0);
                break;
            case CLASS_HUNTER:
                info->stats[STAT_STRENGTH]  += (lvl > 4  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && (lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
                break;
            case CLASS_ROGUE:
                info->stats[STAT_STRENGTH]  += (lvl > 5  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 16 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
                break;
            case CLASS_PRIEST:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 22 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 3  ? 1 : 0);
                break;
            case CLASS_SHAMAN:
                info->stats[STAT_STRENGTH]  += (lvl > 34 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 4 ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 7 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 5 ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 4 ? 1 : 0);
                break;
            case CLASS_MAGE:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 24 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
                break;
            case CLASS_WARLOCK:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
                break;
            case CLASS_DRUID:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 3 : (lvl > 5 ? 1 : 0));
        }
    }
}

/* ********************************************************************************************* */
/* *                                Static Wrappers                                              */
/* ********************************************************************************************* */
GameObjectInfo const* ObjectMgr::GetGameObjectInfo(uint32 id) { return sGOStorage.LookupEntry<GameObjectInfo>(id); }
Player* ObjectMgr::GetPlayer(const char* name) { return ObjectAccessor::FindPlayerByName(name); }
Player* ObjectMgr::GetPlayer(ObjectGuid guid, bool inWorld /*=true*/) { return ObjectAccessor::FindPlayer(guid, inWorld); }
CreatureInfo const* ObjectMgr::GetCreatureTemplate(uint32 id) { return sCreatureStorage.LookupEntry<CreatureInfo>(id); }
CreatureModelInfo const* ObjectMgr::GetCreatureModelInfo(uint32 modelid) { return sCreatureModelStorage.LookupEntry<CreatureModelInfo>(modelid); }
EquipmentInfo const* ObjectMgr::GetEquipmentInfo(uint32 entry) { return sEquipmentStorage.LookupEntry<EquipmentInfo>(entry); }
CreatureDataAddon const* ObjectMgr::GetCreatureAddon(uint32 lowguid) { return sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(lowguid); }
CreatureDataAddon const* ObjectMgr::GetCreatureTemplateAddon(uint32 entry) { return sCreatureInfoAddonStorage.LookupEntry<CreatureDataAddon>(entry); }
ItemPrototype const* ObjectMgr::GetItemPrototype(uint32 id) { return sItemStorage.LookupEntry<ItemPrototype>(id); }
InstanceTemplate const* ObjectMgr::GetInstanceTemplate(uint32 map) { return sInstanceTemplate.LookupEntry<InstanceTemplate>(map); }
WorldTemplate const* ObjectMgr::GetWorldTemplate(uint32 map) { return sWorldTemplate.LookupEntry<WorldTemplate>(map); }
CreatureConditionalSpawn const* ObjectMgr::GetCreatureConditionalSpawn(uint32 lowguid) { return sCreatureConditionalSpawnStore.LookupEntry<CreatureConditionalSpawn>(lowguid); }

/* ********************************************************************************************* */
/* *                                Loading Functions                                            */
/* ********************************************************************************************* */
void ObjectMgr::LoadArenaTeams()
{
    uint32 count = 0;

    //                                                 0                      1    2           3    4               5
    auto queryResult = CharacterDatabase.Query("SELECT arena_team.arenateamid,name,captainguid,type,BackgroundColor,EmblemStyle,"
                          //   6           7           8            9      10         11        12           13          14
                          "EmblemColor,BorderStyle,BorderColor, rating,games_week,wins_week,games_season,wins_season,`rank` "
                          "FROM arena_team LEFT JOIN arena_team_stats ON arena_team.arenateamid = arena_team_stats.arenateamid ORDER BY arena_team.arenateamid ASC");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u arenateam definitions", count);
        sLog.outString();
        return;
    }

    // load arena_team members
    auto arenaTeamMembersResult = CharacterDatabase.Query(
            //      0           1           2           3         4             5           6               7    8
            "SELECT arenateamid,member.guid,played_week,wons_week,played_season,wons_season,personal_rating,name,class "
            "FROM arena_team_member member LEFT JOIN characters chars on member.guid = chars.guid ORDER BY member.arenateamid ASC");

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();
        ++count;

        ArenaTeam* newArenaTeam = new ArenaTeam;
        if (!newArenaTeam->LoadArenaTeamFromDB(queryResult.get()) ||
            !newArenaTeam->LoadMembersFromDB(arenaTeamMembersResult.get()))
        {
            newArenaTeam->Disband(nullptr);
            delete newArenaTeam;
            continue;
        }
        AddArenaTeam(newArenaTeam);
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u arenateam definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadGroups()
{
    // -- loading groups --
    uint32 count = 0;
    //                                                 0         1              2           3           4              5      6      7      8      9      10     11     12     13         14          15              16          17
    auto queryResult = CharacterDatabase.Query("SELECT mainTank, mainAssistant, lootMethod, looterGuid, lootThreshold, icon1, icon2, icon3, icon4, icon5, icon6, icon7, icon8, groupType, difficulty, raiddifficulty, leaderGuid, groupId FROM `groups`");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u group definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();
        ++count;
        Group* group = new Group;
        if (!group->LoadGroupFromDB(fields))
        {
            group->Disband();
            delete group;
            continue;
        }
        AddGroup(group);
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u group definitions", count);
    sLog.outString();

    // -- loading members --
    count = 0;
    //                                            0           1          2         3
    queryResult = CharacterDatabase.Query("SELECT memberGuid, assistant, subgroup, groupId FROM group_member ORDER BY groupId");
    if (!queryResult)
    {
        BarGoLink bar2(1);
        bar2.step();
    }
    else
    {
        Group* group = nullptr;                                // used as cached pointer for avoid relookup group for each member

        BarGoLink bar2(queryResult->GetRowCount());
        do
        {
            bar2.step();
            Field* fields = queryResult->Fetch();
            ++count;

            uint32 memberGuidlow = fields[0].GetUInt32();
            ObjectGuid memberGuid = ObjectGuid(HIGHGUID_PLAYER, memberGuidlow);
            bool   assistent     = fields[1].GetBool();
            uint8  subgroup      = fields[2].GetUInt8();
            uint32 groupId       = fields[3].GetUInt32();
            if (!group || group->GetId() != groupId)
            {
                group = GetGroupById(groupId);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_member table : no group with Id %d for member %s!",
                                    groupId, memberGuid.GetString().c_str());
                    CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%u'", memberGuidlow);
                    continue;
                }
            }

            if (!group->LoadMemberFromDB(memberGuidlow, subgroup, assistent))
            {
                sLog.outErrorDb("Incorrect entry in group_member table : member %s cannot be added to group (Id: %u)!",
                                memberGuid.GetString().c_str(), groupId);
                CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%u'", memberGuidlow);
            }
        }
        while (queryResult->NextRow());
    }

    // clean groups
    // TODO: maybe delete from the DB before loading in this case
    for (GroupMap::iterator itr = mGroupMap.begin(); itr != mGroupMap.end();)
    {
        if (itr->second->GetMembersCount() < 2)
        {
            itr->second->Disband();
            delete itr->second;
            mGroupMap.erase(itr++);
        }
        else
            ++itr;
    }

    // -- loading instances --
    count = 0;
    queryResult = CharacterDatabase.Query(
                 //      0                          1    2         3          4                    5
                 "SELECT group_instance.leaderGuid, map, instance, permanent, instance.difficulty, resettime, "
                 // 6
                 "(SELECT COUNT(*) FROM character_instance WHERE guid = group_instance.leaderGuid AND instance = group_instance.instance AND permanent = 1 LIMIT 1), "
                 // 7              8
                 " `groups`.groupId, instance.encountersMask "
                 "FROM group_instance LEFT JOIN instance ON instance = id LEFT JOIN `groups` ON `groups`.leaderGUID = group_instance.leaderGUID ORDER BY leaderGuid"
             );

    if (!queryResult)
    {
        BarGoLink bar2(1);
        bar2.step();
    }
    else
    {
        Group* group = nullptr;                                // used as cached pointer for avoid relookup group for each member

        BarGoLink bar2(queryResult->GetRowCount());
        do
        {
            bar2.step();
            Field* fields = queryResult->Fetch();
            ++count;

            uint32 leaderGuidLow = fields[0].GetUInt32();
            uint32 mapId = fields[1].GetUInt32();
            uint8 tempDiff = fields[4].GetUInt8();
            uint32 groupId = fields[7].GetUInt32();
            Difficulty diff = REGULAR_DIFFICULTY;

            if (!group || group->GetId() != groupId)
            {
                // find group id in map by leader low guid
                group = GetGroupById(groupId);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_instance table : no group with leader %d", leaderGuidLow);
                    continue;
                }
            }

            MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
            if (!mapEntry || !mapEntry->IsDungeon())
            {
                sLog.outErrorDb("Incorrect entry in group_instance table : no dungeon map %d", mapId);
                continue;
            }

            if (tempDiff >= (mapEntry->IsRaid() ? MAX_RAID_DIFFICULTY : MAX_DUNGEON_DIFFICULTY))
                sLog.outErrorDb("Wrong dungeon difficulty use in group_instance table: %u", uint32(tempDiff));
            else
                diff = Difficulty(tempDiff);

            DungeonPersistentState* state = (DungeonPersistentState*)sMapPersistentStateMgr.AddPersistentState(mapEntry, fields[2].GetUInt32(), Difficulty(diff), (time_t)fields[5].GetUInt64(), (fields[6].GetUInt32() == 0), true, fields[8].GetUInt32());
            group->BindToInstance(state, fields[3].GetBool(), true);
        }
        while (queryResult->NextRow());
    }

    sLog.outString(">> Loaded %u group-instance binds total", count);
    sLog.outString();

    sLog.outString(">> Loaded %u group members total", count);
    sLog.outString();
}

void ObjectMgr::LoadQuests()
{
    // For reload case
    mQuestTemplates.clear();

    m_ExclusiveQuestGroups.clear();

    //                                             0      1       2           3         4           5     6                7              8              9
    auto queryResult = WorldDatabase.Query("SELECT entry, Method, ZoneOrSort, MinLevel, QuestLevel, Type, RequiredClasses, RequiredRaces, RequiredSkill, RequiredSkillValue,"
                          //   10                   11                 12                     13                   14                     15                   16                17
                          "RepObjectiveFaction, RepObjectiveValue, RequiredMinRepFaction, RequiredMinRepValue, RequiredMaxRepFaction, RequiredMaxRepValue, SuggestedPlayers, LimitTime,"
                          //   18          19            20           21            22            23           24           25              26
                          "QuestFlags, SpecialFlags, CharTitleId, PlayersSlain, BonusTalents, PrevQuestId, NextQuestId, ExclusiveGroup, NextQuestInChain,"
                          //   27        28         29           30
                          "RewXPId, SrcItemId, SrcItemCount, SrcSpell,"
                          //   31     32       33          34               35                36       37             38              39              40              41
                          "Title, Details, Objectives, OfferRewardText, RequestItemsText, EndText, CompletedText, ObjectiveText1, ObjectiveText2, ObjectiveText3, ObjectiveText4,"
                          //   42          43          44          45          46          47          48             49             50             51             52             53
                          "ReqItemId1, ReqItemId2, ReqItemId3, ReqItemId4, ReqItemId5, ReqItemId6, ReqItemCount1, ReqItemCount2, ReqItemCount3, ReqItemCount4, ReqItemCount5, ReqItemCount6,"
                          //   54            55            56            57            58               59               60               61
                          "ReqSourceId1, ReqSourceId2, ReqSourceId3, ReqSourceId4, ReqSourceCount1, ReqSourceCount2, ReqSourceCount3, ReqSourceCount4,"
                          //   62                  63                  64                  65                  66                     67                     68                     69
                          "ReqCreatureOrGOId1, ReqCreatureOrGOId2, ReqCreatureOrGOId3, ReqCreatureOrGOId4, ReqCreatureOrGOCount1, ReqCreatureOrGOCount2, ReqCreatureOrGOCount3, ReqCreatureOrGOCount4,"
                          //   70             71             72             73
                          "ReqSpellCast1, ReqSpellCast2, ReqSpellCast3, ReqSpellCast4,"
                          //   74                75                76                77                78                79
                          "RewChoiceItemId1, RewChoiceItemId2, RewChoiceItemId3, RewChoiceItemId4, RewChoiceItemId5, RewChoiceItemId6,"
                          //   80                   81                   82                   83                   84                   85
                          "RewChoiceItemCount1, RewChoiceItemCount2, RewChoiceItemCount3, RewChoiceItemCount4, RewChoiceItemCount5, RewChoiceItemCount6,"
                          //   86          87          88          89          90             91             92             93
                          "RewItemId1, RewItemId2, RewItemId3, RewItemId4, RewItemCount1, RewItemCount2, RewItemCount3, RewItemCount4,"
                          //   94              95              96              97              98
                          "RewRepFaction1, RewRepFaction2, RewRepFaction3, RewRepFaction4, RewRepFaction5,"
                          //   99              100             101             102             103
                          "RewRepValueId1, RewRepValueId2, RewRepValueId3, RewRepValueId4, RewRepValueId5,"
                          //   104           105           106           107           108
                          "RewRepValue1, RewRepValue2, RewRepValue3, RewRepValue4, RewRepValue5,"
                          //   109               110                 111            112               113       114
                          "RewHonorAddition, RewHonorMultiplier, RewOrReqMoney, RewMoneyMaxLevel, RewSpell, RewSpellCast,"
                          //   115                116               117         118     119     120
                          "RewMailTemplateId, RewMailDelaySecs, PointMapId, PointX, PointY, PointOpt,"
                          //   121            122            123            124            125                 126                 127                 128
                          "DetailsEmote1, DetailsEmote2, DetailsEmote3, DetailsEmote4, DetailsEmoteDelay1, DetailsEmoteDelay2, DetailsEmoteDelay3, DetailsEmoteDelay4,"
                          //   129              130            131                132                133                134                135                136
                          "IncompleteEmote, IncompleteEmoteDelay, CompleteEmote, CompleteEmoteDelay, OfferRewardEmote1, OfferRewardEmote2, OfferRewardEmote3, OfferRewardEmote4,"
                          //   137                     138                     139                     140
                          "OfferRewardEmoteDelay1, OfferRewardEmoteDelay2, OfferRewardEmoteDelay3, OfferRewardEmoteDelay4,"
                          //   141          142          143             144              145              146              147              148                149                   150       151
                          "StartScript, CompleteScript, RewMaxRepValue1, RewMaxRepValue2, RewMaxRepValue3, RewMaxRepValue4, RewMaxRepValue5, RequiredCondition, BreadcrumbForQuestId, MaxLevel, ReputationSpilloverMask "

                          " FROM quest_template");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded 0 quests definitions");
        sLog.outErrorDb("`quest_template` table is empty!");
        sLog.outString();
        return;
    }

    // create multimap previous quest for each existing quest
    // some quests can have many previous maps set by NextQuestId in previous quest
    // for example set of race quests can lead to single not race specific quest
    BarGoLink bar(queryResult->GetRowCount());
    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();

        Quest* newQuest = new Quest(fields);
        auto itr = mQuestTemplates.try_emplace(newQuest->GetQuestId(), newQuest).first;
        newQuest->m_weakRef = itr->second;
    }
    while (queryResult->NextRow());

    // Post processing

    std::map<uint32, uint32> usedMailTemplates;

    for (auto& mQuestTemplate : mQuestTemplates)
    {
        Quest* qinfo = mQuestTemplate.second.get();

        // additional quest integrity checks (GO, creature_template and item_template must be loaded already)

        if (qinfo->GetQuestMethod() >= 3)
        {
            sLog.outErrorDb("Quest %u has `Method` = %u, expected values are 0, 1 or 2.", qinfo->GetQuestId(), qinfo->GetQuestMethod());
        }

        if (qinfo->m_SpecialFlags > QUEST_SPECIAL_FLAG_DB_ALLOWED)
        {
            sLog.outErrorDb("Quest %u has `SpecialFlags` = %u, above max flags not allowed for database.", qinfo->GetQuestId(), qinfo->m_SpecialFlags);
        }

        if (qinfo->HasQuestFlag(QUEST_FLAGS_DAILY) && qinfo->HasQuestFlag(QUEST_FLAGS_WEEKLY))
        {
            sLog.outErrorDb("Weekly Quest %u is marked as daily quest in `QuestFlags`, removed daily flag.", qinfo->GetQuestId());
            qinfo->m_QuestFlags &= ~QUEST_FLAGS_DAILY;
        }

        if (qinfo->HasQuestFlag(QUEST_FLAGS_DAILY))
        {
            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
            {
                sLog.outErrorDb("Daily Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE);
            }
        }

        if (qinfo->HasQuestFlag(QUEST_FLAGS_WEEKLY))
        {
            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
            {
                sLog.outErrorDb("Weekly Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE);
            }
        }

        if (qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_MONTHLY))
        {
            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
            {
                sLog.outErrorDb("Monthly quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE);
            }
        }

        if (qinfo->HasQuestFlag(QUEST_FLAGS_AUTO_REWARDED))
        {
            // at auto-reward can be rewarded only RewChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j)
            {
                if (uint32 id = qinfo->RewChoiceItemId[j])
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but item from `RewChoiceItemId%d` can't be rewarded with quest flag QUEST_FLAGS_AUTO_REWARDED.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest ignore this data
                }
            }
        }

        // client quest log visual (area case)
        if (qinfo->ZoneOrSort > 0)
        {
            if (!GetAreaEntryByAreaID(qinfo->ZoneOrSort))
            {
                sLog.outErrorDb("Quest %u has `ZoneOrSort` = %u (zone case) but zone with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client
            }
        }
        // client quest log visual (sort case)
        if (qinfo->ZoneOrSort < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->ZoneOrSort));
            if (!qSort)
            {
                sLog.outErrorDb("Quest %u has `ZoneOrSort` = %i (sort case) but quest sort with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }
        }

        // RequiredClasses, can be 0/CLASSMASK_ALL_PLAYABLE to allow any class
        if (qinfo->RequiredClasses)
        {
            if (!(qinfo->RequiredClasses & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable classes in `RequiredClasses` (%u), value set to 0 (all classes).", qinfo->GetQuestId(), qinfo->RequiredClasses);
                qinfo->RequiredClasses = 0;
            }
        }

        // RequiredRaces, can be 0/RACEMASK_ALL_PLAYABLE to allow any race
        if (qinfo->RequiredRaces)
        {
            if (!(qinfo->RequiredRaces & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable races in `RequiredRaces` (%u), value set to 0 (all races).", qinfo->GetQuestId(), qinfo->RequiredRaces);
                qinfo->RequiredRaces = 0;
            }
        }

        // RequiredSkill, can be 0
        if (qinfo->RequiredSkill)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->RequiredSkill))
            {
                sLog.outErrorDb("Quest %u has `RequiredSkill` = %u but this skill does not exist",
                                qinfo->GetQuestId(), qinfo->RequiredSkill);
            }
        }

        if (qinfo->RequiredSkillValue)
        {
            if (qinfo->RequiredSkillValue > sWorld.GetConfigMaxSkillValue())
            {
                sLog.outErrorDb("Quest %u has `RequiredSkillValue` = %u but max possible skill is %u, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->RequiredSkillValue, sWorld.GetConfigMaxSkillValue());
                // no changes, quest can't be done for this requirement
            }
        }
        // else Skill quests can have 0 skill level, this is ok

        if (qinfo->RepObjectiveFaction && !sFactionStore.LookupEntry(qinfo->RepObjectiveFaction))
        {
            sLog.outErrorDb("Quest %u has `RepObjectiveFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RepObjectiveFaction, qinfo->RepObjectiveFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMinRepFaction))
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepFaction, qinfo->RequiredMinRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMaxRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMaxRepFaction))
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepFaction, qinfo->RequiredMaxRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMinRepValue > ReputationMgr::Reputation_Cap)
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepValue` = %d but max reputation is %u, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue, ReputationMgr::Reputation_Cap);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMaxRepValue && qinfo->RequiredMaxRepValue <= qinfo->RequiredMinRepValue)
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepValue` = %d and `RequiredMinRepValue` = %d, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue, qinfo->RequiredMinRepValue);
            // no changes, quest can't be done for this requirement
        }

        if (!qinfo->RepObjectiveFaction && qinfo->RepObjectiveValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RepObjectiveValue` = %d but `RepObjectiveFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RepObjectiveValue);
            // warning
        }

        if (!qinfo->RequiredMinRepFaction && qinfo->RequiredMinRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepValue` = %d but `RequiredMinRepFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue);
            // warning
        }

        if (!qinfo->RequiredMaxRepFaction && qinfo->RequiredMaxRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepValue` = %d but `RequiredMaxRepFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue);
            // warning
        }

        if (qinfo->CharTitleId && !sCharTitlesStore.LookupEntry(qinfo->CharTitleId))
        {
            sLog.outErrorDb("Quest %u has `CharTitleId` = %u but CharTitle Id %u does not exist, quest can't be rewarded with title.",
                            qinfo->GetQuestId(), qinfo->GetCharTitleId(), qinfo->GetCharTitleId());
            qinfo->CharTitleId = 0;
            // quest can't reward this title
        }

        if (qinfo->SrcItemId)
        {
            if (!sItemStorage.LookupEntry<ItemPrototype>(qinfo->SrcItemId))
            {
                sLog.outErrorDb("Quest %u has `SrcItemId` = %u but item with entry %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcItemId, qinfo->SrcItemId);
                qinfo->SrcItemId = 0;                       // quest can't be done for this requirement
            }
            else if (qinfo->SrcItemCount == 0)
            {
                sLog.outErrorDb("Quest %u has `SrcItemId` = %u but `SrcItemCount` = 0, set to 1 but need fix in DB.",
                                qinfo->GetQuestId(), qinfo->SrcItemId);
                qinfo->SrcItemCount = 1;                    // update to 1 for allow quest work for backward compatibility with DB
            }
        }
        else if (qinfo->SrcItemCount > 0)
        {
            sLog.outErrorDb("Quest %u has `SrcItemId` = 0 but `SrcItemCount` = %u, useless value.",
                            qinfo->GetQuestId(), qinfo->SrcItemCount);
            qinfo->SrcItemCount = 0;                        // no quest work changes in fact
        }

        if (qinfo->SrcSpell)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(qinfo->SrcSpell);
            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `SrcSpell` = %u but spell %u doesn't exist, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `SrcSpell` = %u but spell %u is broken, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
        }

        for (int j = 0; j < QUEST_ITEM_OBJECTIVES_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqItemId[j])
            {
                if (qinfo->ReqItemCount[j] == 0)
                {
                    sLog.outErrorDb("Quest %u has `ReqItemId%d` = %u but `ReqItemCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done for this requirement
                }

                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_DELIVER);

                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has `ReqItemId%d` = %u but item with entry %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->ReqItemCount[j] = 0;             // prevent incorrect work of quest
                }
            }
            else if (qinfo->ReqItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `ReqItemId%d` = 0 but `ReqItemCount%d` = %u, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqItemCount[j]);
                qinfo->ReqItemCount[j] = 0;                 // prevent incorrect work of quest
            }
        }

        for (int j = 0; j < QUEST_SOURCE_ITEM_IDS_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqSourceId[j])
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has `ReqSourceId%d` = %u but item with entry %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    // no changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->ReqSourceCount[j] > 0)
                {
                    sLog.outErrorDb("Quest %u has `ReqSourceId%d` = 0 but `ReqSourceCount%d` = %u.",
                                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqSourceCount[j]);
                    // no changes, quest ignore this data
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqSpell[j])
            {
                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(id);
                if (!spellInfo)
                {
                    sLog.outErrorDb("Quest %u has `ReqSpellCast%d` = %u but spell %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    continue;
                }

                if (!qinfo->ReqCreatureOrGOId[j])
                {
                    bool found = false;
                    for (int k = 0; k < MAX_EFFECT_INDEX; ++k)
                    {
                        if ((spellInfo->Effect[k] == SPELL_EFFECT_QUEST_COMPLETE && uint32(spellInfo->EffectMiscValue[k]) == qinfo->QuestId) ||
                                spellInfo->Effect[k] == SPELL_EFFECT_SEND_EVENT)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
                        {
                            sLog.outErrorDb("Spell (id: %u) have SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT for quest %u and ReqCreatureOrGOId%d = 0, but quest not have flag QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT. Quest flags or ReqCreatureOrGOId%d must be fixed, quest modified to enable objective.", spellInfo->Id, qinfo->QuestId, j + 1, j + 1);

                            // this will prevent quest completing without objective
                            const_cast<Quest*>(qinfo)->SetSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT);
                        }
                    }
                    else
                    {
                        sLog.outErrorDb("Quest %u has `ReqSpellCast%d` = %u and ReqCreatureOrGOId%d = 0 but spell %u does not have SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT effect for this quest, quest can't be done.",
                                        qinfo->GetQuestId(), j + 1, id, j + 1, id);
                        // no changes, quest can't be done for this requirement
                    }
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            int32 id = qinfo->ReqCreatureOrGOId[j];
            if (id < 0 && !sGOStorage.LookupEntry<GameObjectInfo>(-id))
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %i but gameobject %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, id, uint32(-id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id > 0 && !sCreatureStorage.LookupEntry<CreatureInfo>(id))
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %i but creature with entry %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, id, uint32(id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id)
            {
                // In fact SpeakTo and Kill are quite same: either you can speak to mob:SpeakTo or you can't:Kill/Cast

                qinfo->SetSpecialFlag(QuestSpecialFlags(QUEST_SPECIAL_FLAG_KILL_OR_CAST | QUEST_SPECIAL_FLAG_SPEAKTO));

                if (!qinfo->ReqCreatureOrGOCount[j])
                {
                    sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %u but `ReqCreatureOrGOCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can be incorrectly done, but we already report this
                }
            }
            else if (qinfo->ReqCreatureOrGOCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = 0 but `ReqCreatureOrGOCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqCreatureOrGOCount[j]);
                // no changes, quest ignore this data
            }
        }

        bool choice_found = false;
        for (int j = QUEST_REWARD_CHOICES_COUNT - 1; j >= 0; --j)
        {
            if (uint32 id = qinfo->RewChoiceItemId[j])
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewChoiceItemId[j] = 0;          // no changes, quest will not reward this
                }
                else
                    choice_found = true;

                if (!qinfo->RewChoiceItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but `RewChoiceItemCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done
                }
            }
            else if (choice_found)                          // client crash if have gap in item reward choices
            {
                sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = 0 but `RewChoiceItemId%d` = %u, client can crash at like data.",
                                qinfo->GetQuestId(), j + 1, j + 2, qinfo->RewChoiceItemId[j + 1]);
                // fill gap by clone later filled choice
                qinfo->RewChoiceItemId[j] = qinfo->RewChoiceItemId[j + 1];
                qinfo->RewChoiceItemCount[j] = qinfo->RewChoiceItemCount[j + 1];
            }
            else if (qinfo->RewChoiceItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = 0 but `RewChoiceItemCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewChoiceItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REWARDS_COUNT; ++j)
        {
            if (uint32 id = qinfo->RewItemId[j])
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has `RewItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewItemId[j] = 0;                // no changes, quest will not reward this item
                }

                if (!qinfo->RewItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has `RewItemId%d` = %u but `RewItemCount%d` = 0, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes
                }
            }
            else if (qinfo->RewItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `RewItemId%d` = 0 but `RewItemCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewRepFaction[j])
            {
                if (abs(qinfo->RewRepValueId[j]) > 9)
                    sLog.outErrorDb("Quest %u has RewRepValueId%d = %i but value is not valid.", qinfo->GetQuestId(), j + 1, qinfo->RewRepValueId[j]);

                if (!sFactionStore.LookupEntry(qinfo->RewRepFaction[j]))
                {
                    sLog.outErrorDb("Quest %u has `RewRepFaction%d` = %u but raw faction (faction.dbc) %u does not exist, quest will not reward reputation for this faction.",
                                    qinfo->GetQuestId(), j + 1, qinfo->RewRepFaction[j], qinfo->RewRepFaction[j]);
                    qinfo->RewRepFaction[j] = 0;            // quest will not reward this
                }
            }
            else if (qinfo->RewRepValue[j] != 0)
            {
                sLog.outErrorDb("Quest %u has `RewRepFaction%d` = 0 but `RewRepValue%d` = %i.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewRepValue[j]);
                // no changes, quest ignore this data
            }
        }

        if (qinfo->RewSpell)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(qinfo->RewSpell);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u does not exist, spell removed as display reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is broken, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
            else if (GetTalentSpellCost(qinfo->RewSpell))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
        }

        if (qinfo->RewSpellCast)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(qinfo->RewSpellCast);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `RewSpellCast` = %u but spell %u does not exist, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `RewSpellCast` = %u but spell %u is broken, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
            else if (GetTalentSpellCost(qinfo->RewSpellCast))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
        }

        if (qinfo->RewMailTemplateId)
        {
            if (!sMailTemplateStore.LookupEntry(qinfo->RewMailTemplateId))
            {
                sLog.outErrorDb("Quest %u has `RewMailTemplateId` = %u but mail template  %u does not exist, quest will not have a mail reward.",
                                qinfo->GetQuestId(), qinfo->RewMailTemplateId, qinfo->RewMailTemplateId);
                qinfo->RewMailTemplateId = 0;               // no mail will send to player
                qinfo->RewMailDelaySecs = 0;                // no mail will send to player
            }
            else if (usedMailTemplates.find(qinfo->RewMailTemplateId) != usedMailTemplates.end())
            {
                std::map<uint32, uint32>::const_iterator used_mt_itr = usedMailTemplates.find(qinfo->RewMailTemplateId);
                sLog.outErrorDb("Quest %u has `RewMailTemplateId` = %u but mail template  %u already used for quest %u, quest will not have a mail reward.",
                                qinfo->GetQuestId(), qinfo->RewMailTemplateId, qinfo->RewMailTemplateId, used_mt_itr->second);
                qinfo->RewMailTemplateId = 0;               // no mail will send to player
                qinfo->RewMailDelaySecs = 0;                // no mail will send to player
            }
            else
                usedMailTemplates[qinfo->RewMailTemplateId] = qinfo->GetQuestId();
        }

        if (qinfo->NextQuestInChain)
        {
            QuestMap::iterator qNextItr = mQuestTemplates.find(qinfo->NextQuestInChain);
            if (qNextItr == mQuestTemplates.end())
            {
                sLog.outErrorDb("Quest %u has `NextQuestInChain` = %u but quest %u does not exist, quest chain will not work.",
                                qinfo->GetQuestId(), qinfo->NextQuestInChain, qinfo->NextQuestInChain);
                qinfo->NextQuestInChain = 0;
            }
            else
                qNextItr->second->prevChainQuests.push_back(qinfo->GetQuestId());
        }

        // fill additional data stores
        if (qinfo->PrevQuestId)
        {
            QuestMap::iterator qPrevItr = mQuestTemplates.find(abs(qinfo->GetPrevQuestId()));
            if (qPrevItr == mQuestTemplates.end())
                sLog.outErrorDb("Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else if (qPrevItr->second->BreadcrumbForQuestId)
                sLog.outErrorDb("Quest %d should not be unlocked by breadcrumb quest %u", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else
                qinfo->prevQuests.push_back(qinfo->PrevQuestId);
        }

        if (qinfo->NextQuestId)
        {
            QuestMap::iterator qNextItr = mQuestTemplates.find(abs(qinfo->GetNextQuestId()));
            if (qNextItr == mQuestTemplates.end())
            {
                sLog.outErrorDb("Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            }
            else
            {
                int32 signedQuestId = qinfo->NextQuestId < 0 ? -int32(qinfo->GetQuestId()) : int32(qinfo->GetQuestId());
                qNextItr->second->prevQuests.push_back(signedQuestId);
            }
        }

        if (qinfo->ExclusiveGroup)
            m_ExclusiveQuestGroups.insert(ExclusiveQuestGroupsMap::value_type(qinfo->ExclusiveGroup, qinfo->GetQuestId()));

        if (qinfo->LimitTime)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_TIMED);
        if (qinfo->PlayersSlain)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_PLAYER_KILL);

        if (uint32 breadcrumbQuestId = qinfo->BreadcrumbForQuestId)
        {
            QuestMap::iterator qBreadcrumbQuestItr = mQuestTemplates.find(breadcrumbQuestId);
            if (qBreadcrumbQuestItr == mQuestTemplates.end())
            {
                sLog.outErrorDb("Quest %u has BreadcrumbForQuestId %u, but there is no such quest", qinfo->GetQuestId(), breadcrumbQuestId);
                qinfo->BreadcrumbForQuestId = 0;
            }
            else
            {
                if (qinfo->NextQuestId)
                    sLog.outErrorDb("Quest %u is a breadcrumb, it should not unlock quest %d", qinfo->GetQuestId(), qinfo->NextQuestId);
            }
        }
    }

    // Prevent any breadcrumb loops, and inform target quests of their breadcrumbs
    for (auto& mQuestTemplate : mQuestTemplates)
    {
        Quest* qinfo = mQuestTemplate.second.get();
        uint32   qid = qinfo->GetQuestId();
        uint32 breadcrumbForQuestId = qinfo->BreadcrumbForQuestId;
        std::set<uint32> questSet;

        while (breadcrumbForQuestId)
        {
            // If the insertion fails, then we already processed this breadcrumb quest in this iteration. This means that two breadcrumb quests have each other set as targets
            if (!questSet.insert(qinfo->QuestId).second)
            {
                sLog.outErrorDb("Breadcrumb quests %u and %u are in a loop!", qid, breadcrumbForQuestId);
                qinfo->BreadcrumbForQuestId = 0;
                break;
            }

            qinfo = const_cast<Quest*>(sObjectMgr.GetQuestTemplate(breadcrumbForQuestId));

            // Every quest has a list of breadcrumb quests that point toward it
            qinfo->DependentBreadcrumbQuests.push_back(qid);

            breadcrumbForQuestId = qinfo->GetBreadcrumbForQuestId();
        }
    }

    sLog.outString(">> Loaded " SIZEFMTD " quests definitions", mQuestTemplates.size());
    sLog.outString();
}

void ObjectMgr::LoadQuestLocales()
{
    mQuestLocaleMap.clear();                                // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,"
                          "Title_loc1,Details_loc1,Objectives_loc1,OfferRewardText_loc1,RequestItemsText_loc1,EndText_loc1,CompletedText_loc1,ObjectiveText1_loc1,ObjectiveText2_loc1,ObjectiveText3_loc1,ObjectiveText4_loc1,"
                          "Title_loc2,Details_loc2,Objectives_loc2,OfferRewardText_loc2,RequestItemsText_loc2,EndText_loc2,CompletedText_loc2,ObjectiveText1_loc2,ObjectiveText2_loc2,ObjectiveText3_loc2,ObjectiveText4_loc2,"
                          "Title_loc3,Details_loc3,Objectives_loc3,OfferRewardText_loc3,RequestItemsText_loc3,EndText_loc3,CompletedText_loc3,ObjectiveText1_loc3,ObjectiveText2_loc3,ObjectiveText3_loc3,ObjectiveText4_loc3,"
                          "Title_loc4,Details_loc4,Objectives_loc4,OfferRewardText_loc4,RequestItemsText_loc4,EndText_loc4,CompletedText_loc4,ObjectiveText1_loc4,ObjectiveText2_loc4,ObjectiveText3_loc4,ObjectiveText4_loc4,"
                          "Title_loc5,Details_loc5,Objectives_loc5,OfferRewardText_loc5,RequestItemsText_loc5,EndText_loc5,CompletedText_loc5,ObjectiveText1_loc5,ObjectiveText2_loc5,ObjectiveText3_loc5,ObjectiveText4_loc5,"
                          "Title_loc6,Details_loc6,Objectives_loc6,OfferRewardText_loc6,RequestItemsText_loc6,EndText_loc6,CompletedText_loc6,ObjectiveText1_loc6,ObjectiveText2_loc6,ObjectiveText3_loc6,ObjectiveText4_loc6,"
                          "Title_loc7,Details_loc7,Objectives_loc7,OfferRewardText_loc7,RequestItemsText_loc7,EndText_loc7,CompletedText_loc7,ObjectiveText1_loc7,ObjectiveText2_loc7,ObjectiveText3_loc7,ObjectiveText4_loc7,"
                          "Title_loc8,Details_loc8,Objectives_loc8,OfferRewardText_loc8,RequestItemsText_loc8,EndText_loc8,CompletedText_loc8,ObjectiveText1_loc8,ObjectiveText2_loc8,ObjectiveText3_loc8,ObjectiveText4_loc8"
                          " FROM locales_quest"
                                             );

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 Quest locale strings. DB table `locales_quest` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetQuestTemplate(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_quest` has data for nonexistent quest entry %u, skipped.", entry);
            continue;
        }

        QuestLocale& data = mQuestLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 11 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Title.size() <= idx)
                        data.Title.resize(idx + 1);

                    data.Title[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Details.size() <= idx)
                        data.Details.resize(idx + 1);

                    data.Details[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 2].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Objectives.size() <= idx)
                        data.Objectives.resize(idx + 1);

                    data.Objectives[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 3].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.OfferRewardText.size() <= idx)
                        data.OfferRewardText.resize(idx + 1);

                    data.OfferRewardText[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 4].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.RequestItemsText.size() <= idx)
                        data.RequestItemsText.resize(idx + 1);

                    data.RequestItemsText[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 5].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.EndText.size() <= idx)
                        data.EndText.resize(idx + 1);

                    data.EndText[idx] = str;
                }
            }
            str = fields[1 + 11 * (i - 1) + 6].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.CompletedText.size() <= idx)
                        data.CompletedText.resize(idx + 1);

                    data.CompletedText[idx] = str;
                }
            }
            for (int k = 0; k < 4; ++k)
            {
                str = fields[1 + 11 * (i - 1) + 7 + k].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if ((int32)data.ObjectiveText[k].size() <= idx)
                            data.ObjectiveText[k].resize(idx + 1);

                        data.ObjectiveText[k][idx] = str;
                    }
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u Quest locale strings", (uint32)mQuestLocaleMap.size());
    sLog.outString();
}

void ObjectMgr::LoadPageTexts()
{
    sPageTextStore.Load();
    sLog.outString(">> Loaded %u page texts", sPageTextStore.GetRecordCount());
    sLog.outString();

    for (uint32 i = 1; i < sPageTextStore.GetMaxEntry(); ++i)
    {
        // check data correctness
        PageText const* page = sPageTextStore.LookupEntry<PageText>(i);
        if (!page)
            continue;

        if (page->Next_Page && !sPageTextStore.LookupEntry<PageText>(page->Next_Page))
        {
            sLog.outErrorDb("Page text (Id: %u) has not existing next page (Id:%u)", i, page->Next_Page);
            continue;
        }

        // detect circular reference
        std::set<uint32> checkedPages;
        for (PageText const* pageItr = page; pageItr; pageItr = sPageTextStore.LookupEntry<PageText>(pageItr->Next_Page))
        {
            if (!pageItr->Next_Page)
                break;
            checkedPages.insert(pageItr->Page_ID);
            if (checkedPages.find(pageItr->Next_Page) != checkedPages.end())
            {
                std::ostringstream ss;
                ss << "The text page(s) ";
                for (uint32 checkedPage : checkedPages)
                    ss << checkedPage << " ";
                ss << "create(s) a circular reference, which can cause the server to freeze. Changing Next_Page of page "
                   << pageItr->Page_ID << " to 0";
                sLog.outErrorDb("%s", ss.str().c_str());
                const_cast<PageText*>(pageItr)->Next_Page = 0;
                break;
            }
        }
    }
}

void ObjectMgr::LoadPageTextLocales()
{
    mPageTextLocaleMap.clear();                             // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,text_loc1,text_loc2,text_loc3,text_loc4,text_loc5,text_loc6,text_loc7,text_loc8 FROM locales_page_text");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 PageText locale strings. DB table `locales_page_text` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!sPageTextStore.LookupEntry<PageText>(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_page_text` has data for nonexistent page text entry %u, skipped.", entry);
            continue;
        }

        PageTextLocale& data = mPageTextLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (str.empty())
                continue;

            int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
            if (idx >= 0)
            {
                if ((int32)data.Text.size() <= idx)
                    data.Text.resize(idx + 1);

                data.Text[idx] = str;
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " PageText locale strings", mPageTextLocaleMap.size());
    sLog.outString();
}

void ObjectMgr::LoadInstanceEncounters()
{
    m_DungeonEncounters.clear();         // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry, creditType, creditEntry, lastEncounterDungeon FROM instance_encounters");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 Instance Encounters. DB table `instance_encounters` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();
        DungeonEncounterEntry const* dungeonEncounter = sDungeonEncounterStore.LookupEntry<DungeonEncounterEntry>(entry);

        if (!dungeonEncounter)
        {
            sLog.outErrorDb("Table `instance_encounters` has an invalid encounter id %u, skipped!", entry);
            continue;
        }

        uint8 creditType = fields[1].GetUInt8();
        uint32 creditEntry = fields[2].GetUInt32();
        switch (creditType)
        {
            case ENCOUNTER_CREDIT_KILL_CREATURE:
            {
                CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(creditEntry);
                if (!cInfo)
                {
                    sLog.outErrorDb("Table `instance_encounters` has an invalid creature (entry %u) linked to the encounter %u (%s), skipped!", creditEntry, entry, dungeonEncounter->encounterName[0]);
                    continue;
                }
                break;
            }
            case ENCOUNTER_CREDIT_CAST_SPELL:
            {
                if (!sSpellTemplate.LookupEntry<SpellEntry>(creditEntry))
                {
                    // skip spells that aren't in dbc for now
                    // sLog.outErrorDb("Table `instance_encounters` has an invalid spell (entry %u) linked to the encounter %u (%s), skipped!", creditEntry, entry, dungeonEncounter->encounterName[0]);
                    continue;
                }
                break;
            }
            case ENCOUNTER_CREDIT_SCRIPT:
                break;
            default:
                sLog.outErrorDb("Table `instance_encounters` has an invalid credit type (%u) for encounter %u (%s), skipped!", creditType, entry, dungeonEncounter->encounterName[0]);
                continue;
        }
        uint32 lastEncounterDungeon = fields[3].GetUInt32();

        m_DungeonEncounters.emplace(creditEntry, DungeonEncounter(dungeonEncounter, EncounterCreditType(creditType), creditEntry, lastEncounterDungeon));
        m_DungeonEncountersByMap.emplace(dungeonEncounter->mapId, DungeonEncounter(dungeonEncounter, EncounterCreditType(creditType), creditEntry, lastEncounterDungeon));
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " Instance Encounters", m_DungeonEncounters.size());
    sLog.outString();
}

struct SQLInstanceLoader : public SQLStorageLoaderBase<SQLInstanceLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptDevAIMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadInstanceTemplate()
{
    SQLInstanceLoader loader;
    loader.Load(sInstanceTemplate);

    for (uint32 i = 0; i < sInstanceTemplate.GetMaxEntry(); ++i)
    {
        InstanceTemplate const* temp = GetInstanceTemplate(i);
        if (!temp)
            continue;

        MapEntry const* mapEntry = sMapStore.LookupEntry(temp->map);
        if (!mapEntry)
        {
            sLog.outErrorDb("ObjectMgr::LoadInstanceTemplate: bad mapid %d for template!", temp->map);
            sInstanceTemplate.EraseEntry(i);
            continue;
        }

        if (!mapEntry->Instanceable())
        {
            sLog.outErrorDb("ObjectMgr::LoadInstanceTemplate: non-instanceable mapid %d for template!", temp->map);
            sInstanceTemplate.EraseEntry(i);
            continue;
        }

        if (temp->parent > 0)
        {
            // check existence
            MapEntry const* parentEntry = sMapStore.LookupEntry(temp->parent);
            if (!parentEntry)
            {
                sLog.outErrorDb("ObjectMgr::LoadInstanceTemplate: bad parent map id %u for instance template %d template!",
                                temp->parent, temp->map);
                const_cast<InstanceTemplate*>(temp)->parent = 0;
                continue;
            }

            if (parentEntry->IsContinent())
            {
                sLog.outErrorDb("ObjectMgr::LoadInstanceTemplate: parent point to continent map id %u for instance template %d template, ignored, need be set only for non-continent parents!",
                                parentEntry->MapID, temp->map);
                const_cast<InstanceTemplate*>(temp)->parent = 0;
            }
        }
    }

    sLog.outString(">> Loaded %u Instance Template definitions", sInstanceTemplate.GetRecordCount());
    sLog.outString();
}

struct SQLWorldLoader : public SQLStorageLoaderBase<SQLWorldLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptDevAIMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadWorldTemplate()
{
    SQLWorldLoader loader;
    loader.Load(sWorldTemplate, false);

    for (uint32 i = 0; i < sWorldTemplate.GetMaxEntry(); ++i)
    {
        WorldTemplate const* temp = GetWorldTemplate(i);
        if (!temp)
            continue;

        MapEntry const* mapEntry = sMapStore.LookupEntry(temp->map);
        if (!mapEntry)
        {
            sLog.outErrorDb("ObjectMgr::LoadWorldTemplate: bad mapid %d for template!", temp->map);
            sWorldTemplate.EraseEntry(i);
            continue;
        }

        if (mapEntry->Instanceable())
        {
            sLog.outErrorDb("ObjectMgr::LoadWorldTemplate: instanceable mapid %d for template!", temp->map);
            sWorldTemplate.EraseEntry(i);
        }
    }

    sLog.outString(">> Loaded %u World Template definitions", sWorldTemplate.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::LoadWorldStateNames()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Id, Name FROM worldstate_name"));

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded %u worldstate names", count);
        return;
    }

    BarGoLink bar(result->GetRowCount());

    Field* fields;
    do
    {
        bar.step();

        fields = result->Fetch();

        WorldStateName name;
        name.Id = fields[0].GetInt32();
        name.Name = fields[1].GetCppString();

        m_worldStateNames.emplace(name.Id, name);

        ++count;
    } while (result->NextRow());

    sLog.outString(">> Loaded %u worldstate names", count);
    sLog.outString();
}

void ObjectMgr::LoadConditions()
{
    SQLWorldLoader loader;
    loader.Load(sConditionStorage);

    for (uint32 i = 0; i < sConditionStorage.GetMaxEntry(); ++i)
    {
        ConditionEntry const* condition = sConditionStorage.LookupEntry<ConditionEntry>(i);
        if (!condition)
            continue;

        if (!condition->IsValid())
        {
            sLog.outErrorDb("ObjectMgr::LoadConditions: invalid condition_entry %u, skip", i);
            sConditionStorage.EraseEntry(i);
            continue;
        }
    }

    for (auto& mQuestTemplate : mQuestTemplates) // needs to be checked after loading conditions
    {
        Quest* qinfo = mQuestTemplate.second.get();

        if (qinfo->RequiredCondition)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(qinfo->RequiredCondition);
            if (!condition) // condition does not exist for some reason
                sLog.outErrorDb("Quest %u has `RequiredCondition` = %u but does not exist.", qinfo->GetQuestId(), qinfo->RequiredCondition);
        }
    }

    sLog.outString(">> Loaded %u Condition definitions", sConditionStorage.GetRecordCount());
    sLog.outString();
}

GossipText const* ObjectMgr::GetGossipText(uint32 Text_ID) const
{
    GossipTextMap::const_iterator itr = mGossipText.find(Text_ID);
    if (itr != mGossipText.end())
        return &itr->second;
    return nullptr;
}

void ObjectMgr::LoadGossipText()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT * FROM npc_text"));

    int count = 0;
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded %u npc texts", count);
        sLog.outString();
        return;
    }

    BarGoLink bar1(result->GetRowCount());

    do
    {
        ++count;
        int cic = 0;

        Field* fields = result->Fetch();

        bar1.step();

        uint32 Text_ID    = fields[cic++].GetUInt32();
        if (!Text_ID)
        {
            sLog.outErrorDb("Table `npc_text` has record wit reserved id 0, ignore.");
            continue;
        }

        GossipText& gText = mGossipText[Text_ID];

        for (auto& Option : gText.Options)
        {
            Option.Text_0           = fields[cic++].GetCppString();
            Option.Text_1           = fields[cic++].GetCppString();

            Option.Language         = fields[cic++].GetUInt32();
            Option.Probability      = fields[cic++].GetFloat();

            for (int j = 0; j < 3; ++j)
            {
                Option.Emotes[j]._Delay  = fields[cic++].GetUInt32();
                Option.Emotes[j]._Emote  = fields[cic++].GetUInt32();
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u npc texts", count);
    sLog.outString();

    result = WorldDatabase.Query("SELECT Id,Prob0,Prob1,Prob2,Prob3,Prob4,Prob5,Prob6,Prob7,BroadcastTextId0,BroadcastTextId1,BroadcastTextId2,BroadcastTextId3,BroadcastTextId4,BroadcastTextId5,BroadcastTextId6,BroadcastTextId7 FROM npc_text_broadcast_text");

    count = 0;
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded %u npc_text_broadcast_text", count);
        sLog.outString();
        return;
    }

    BarGoLink bar2(result->GetRowCount());

    do
    {
        ++count;

        Field* fields = result->Fetch();

        bar2.step();

        uint32 id = fields[0].GetUInt32();
        if (!id)
        {
            sLog.outErrorDb("Table `npc_text_broadcast_text` has record wit reserved id 0, ignore.");
            continue;
        }
        GossipText& gText = mGossipText[id];
        if (!gText.Options[0].Text_0.empty() || !gText.Options[0].Text_1.empty())
            sLog.outErrorDb("Table `npc_text_broadcast_text` has record in `npc_text` (ID %u) as well. Overwriting.", id);

        for (uint32 i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; ++i)
        {
            auto& option = gText.Options[i];
            option.Probability = fields[1 + i].GetFloat();
            uint32 broadcastTextId = fields[1 + MAX_GOSSIP_TEXT_OPTIONS + i].GetUInt32();
            if (BroadcastText const* bct = sObjectMgr.GetBroadcastText(broadcastTextId))
            {
                option.Text_0 = "";
                option.Text_1 = "";
                option.Language = bct->languageId;
                for (int j = 0; j < 3; ++j)
                {
                    option.Emotes[j]._Delay = bct->emoteDelays[j];
                    option.Emotes[j]._Emote = bct->emoteIds[j];
                }
                option.broadcastTextId = broadcastTextId;
            }
            else
            {
                option.Text_0 = "";
                option.Text_1 = "";
                option.Language = 0;
                for (int j = 0; j < 3; ++j)
                {
                    option.Emotes[j]._Delay = 0;
                    option.Emotes[j]._Emote = 0;
                }
                option.broadcastTextId = 0;
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadGossipTextLocales()
{
    mNpcTextLocaleMap.clear();                              // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,"
                          "Text0_0_loc1,Text0_1_loc1,Text1_0_loc1,Text1_1_loc1,Text2_0_loc1,Text2_1_loc1,Text3_0_loc1,Text3_1_loc1,Text4_0_loc1,Text4_1_loc1,Text5_0_loc1,Text5_1_loc1,Text6_0_loc1,Text6_1_loc1,Text7_0_loc1,Text7_1_loc1,"
                          "Text0_0_loc2,Text0_1_loc2,Text1_0_loc2,Text1_1_loc2,Text2_0_loc2,Text2_1_loc2,Text3_0_loc2,Text3_1_loc2,Text4_0_loc2,Text4_1_loc2,Text5_0_loc2,Text5_1_loc2,Text6_0_loc2,Text6_1_loc2,Text7_0_loc2,Text7_1_loc2,"
                          "Text0_0_loc3,Text0_1_loc3,Text1_0_loc3,Text1_1_loc3,Text2_0_loc3,Text2_1_loc3,Text3_0_loc3,Text3_1_loc3,Text4_0_loc3,Text4_1_loc3,Text5_0_loc3,Text5_1_loc3,Text6_0_loc3,Text6_1_loc3,Text7_0_loc3,Text7_1_loc3,"
                          "Text0_0_loc4,Text0_1_loc4,Text1_0_loc4,Text1_1_loc4,Text2_0_loc4,Text2_1_loc4,Text3_0_loc4,Text3_1_loc4,Text4_0_loc4,Text4_1_loc4,Text5_0_loc4,Text5_1_loc4,Text6_0_loc4,Text6_1_loc4,Text7_0_loc4,Text7_1_loc4,"
                          "Text0_0_loc5,Text0_1_loc5,Text1_0_loc5,Text1_1_loc5,Text2_0_loc5,Text2_1_loc5,Text3_0_loc5,Text3_1_loc5,Text4_0_loc5,Text4_1_loc5,Text5_0_loc5,Text5_1_loc5,Text6_0_loc5,Text6_1_loc5,Text7_0_loc5,Text7_1_loc5,"
                          "Text0_0_loc6,Text0_1_loc6,Text1_0_loc6,Text1_1_loc6,Text2_0_loc6,Text2_1_loc6,Text3_0_loc6,Text3_1_loc6,Text4_0_loc6,Text4_1_loc6,Text5_0_loc6,Text5_1_loc6,Text6_0_loc6,Text6_1_loc6,Text7_0_loc6,Text7_1_loc6,"
                          "Text0_0_loc7,Text0_1_loc7,Text1_0_loc7,Text1_1_loc7,Text2_0_loc7,Text2_1_loc7,Text3_0_loc7,Text3_1_loc7,Text4_0_loc7,Text4_1_loc7,Text5_0_loc7,Text5_1_loc7,Text6_0_loc7,Text6_1_loc7,Text7_0_loc7,Text7_1_loc7, "
                          "Text0_0_loc8,Text0_1_loc8,Text1_0_loc8,Text1_1_loc8,Text2_0_loc8,Text2_1_loc8,Text3_0_loc8,Text3_1_loc8,Text4_0_loc8,Text4_1_loc8,Text5_0_loc8,Text5_1_loc8,Text6_0_loc8,Text6_1_loc8,Text7_0_loc8,Text7_1_loc8 "
                          " FROM locales_npc_text");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 NpcText locale strings. DB table `locales_npc_text` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetGossipText(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_npc_text` has data for nonexistent gossip text entry %u, skipped.", entry);
            continue;
        }

        NpcTextLocale& data = mNpcTextLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                std::string str0 = fields[1 + 8 * 2 * (i - 1) + 2 * j].GetCppString();
                if (!str0.empty())
                {
                    int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if ((int32)data.Text_0[j].size() <= idx)
                            data.Text_0[j].resize(idx + 1);

                        data.Text_0[j][idx] = str0;
                    }
                }
                std::string str1 = fields[1 + 8 * 2 * (i - 1) + 2 * j + 1].GetCppString();
                if (!str1.empty())
                {
                    int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if ((int32)data.Text_1[j].size() <= idx)
                            data.Text_1[j].resize(idx + 1);

                        data.Text_1[j][idx] = str1;
                    }
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " NpcText locale strings", mNpcTextLocaleMap.size());
    sLog.outString();
}

QuestgiverGreeting const* ObjectMgr::GetQuestgiverGreetingData(uint32 entry, uint32 type) const
{
    auto itr = m_questgiverGreetingMap[type].find(entry);
    if (itr == m_questgiverGreetingMap[type].end()) return nullptr;
    return &itr->second;
}

void ObjectMgr::LoadQuestgiverGreeting()
{
    for (auto& i : m_questgiverGreetingMap) // Reload Case
        i.clear();

    auto queryResult = WorldDatabase.Query("SELECT Entry, Type, Text, EmoteId, EmoteDelay FROM questgiver_greeting");
    int count = 0;
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded %u questgiver greetings", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();
        uint32 type = fields[1].GetUInt32();
        uint32 emoteId = fields[3].GetUInt32();

        switch (type)
        {
            case QUESTGIVER_CREATURE:
                if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
                {
                    sLog.outErrorEventAI("Table questgiver_greeting uses nonexistent creature entry %u. Skipping.", entry);
                    continue;
                }
                break;
            case QUESTGIVER_GAMEOBJECT:
                if (!sGOStorage.LookupEntry<GameObjectInfo>(entry))
                {
                    sLog.outErrorEventAI("Table questgiver_greeting uses nonexistent gameobject entry %u. Skipping.", entry);
                    continue;
                }
                break;
            default:
                sLog.outErrorEventAI("Table questgiver_greeting uses questgiver type %u. Skipping.", type);
                continue;
        }

        if (!sEmotesStore.LookupEntry(emoteId))
        {
            sLog.outErrorEventAI("Table questgiver_greeting entry %u type %u uses invalid emote %u. Skipping.", entry, type, emoteId);
            continue;
        }

        QuestgiverGreeting& var = m_questgiverGreetingMap[type][entry];
        var.text = fields[2].GetString();
        var.emoteId = emoteId;
        var.emoteDelay = fields[4].GetUInt32();

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u questgiver greetings.", count);
    sLog.outString();
}

void ObjectMgr::LoadQuestgiverGreetingLocales()
{
    for (auto& i : m_questgiverGreetingLocaleMap)        // need for reload case
        i.clear();

    auto queryResult = WorldDatabase.Query("SELECT Entry, Type, Text_loc1, Text_loc2, Text_loc3, Text_loc4, Text_loc5, Text_loc6, Text_loc7, Text_loc8 FROM locales_questgiver_greeting");
    int count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 locales questgiver greetings");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();
        uint32 type = fields[1].GetUInt32();

        switch (type)
        {
            case QUESTGIVER_CREATURE:
                if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
                {
                    sLog.outErrorEventAI("Table questgiver_greeting uses nonexistent creature entry %u. Skipping.", entry);
                    continue;
                }
                break;
            case QUESTGIVER_GAMEOBJECT:
                if (!sGOStorage.LookupEntry<GameObjectInfo>(entry))
                {
                    sLog.outErrorEventAI("Table questgiver_greeting uses nonexistent gameobject entry %u. Skipping.", entry);
                    continue;
                }
                break;
            default:
                sLog.outErrorEventAI("Table questgiver_greeting uses questgiver type %u. Skipping.", type);
                continue;
        }

        QuestgiverGreetingLocale& var = m_questgiverGreetingLocaleMap[type][entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (var.localeText.size() <= static_cast<size_t>(idx))
                        var.localeText.resize(idx + 1);

                    var.localeText[idx] = str;
                }
            }
        }

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u locales questgiver greetings.", count);
    sLog.outString();
}

TrainerGreeting const* ObjectMgr::GetTrainerGreetingData(uint32 entry) const
{
    auto itr = m_trainerGreetingMap.find(entry);
    if (itr == m_trainerGreetingMap.end()) return nullptr;
    return &itr->second;
}

AccessRequirement const* ObjectMgr::GetAccessRequirement(uint32 mapid, Difficulty difficulty) const
{
    auto itr = m_accessRequirements.find(MAKE_PAIR32(mapid, difficulty));
    if (itr != m_accessRequirements.end())
        return &itr->second;
    return nullptr;
}

void ObjectMgr::LoadTrainerGreetings()
{
    m_trainerGreetingMap.clear();                           // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT Entry, Text FROM trainer_greeting");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded 0 trainer greetings. DB table `trainer_greeting` is empty!");
        sLog.outString();

        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
        {
            sLog.outErrorDb("Table trainer_greeting uses nonexistent creature entry %u. Skipping.", entry);
            continue;
        }

        TrainerGreeting& var = m_trainerGreetingMap[entry];
        var.text = fields[1].GetString();
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u trainer greetings.", uint32(m_trainerGreetingMap.size()));
    sLog.outString();
}

void ObjectMgr::LoadTrainerGreetingLocales()
{
    m_trainerGreetingLocaleMap.clear();                     // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT Entry, Text_loc1, Text_loc2, Text_loc3, Text_loc4, Text_loc5, Text_loc6, Text_loc7, Text_loc8 FROM locales_trainer_greeting");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded 0 locales trainer greetings.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
        {
            sLog.outErrorDb("Table locales_trainer_greeting uses nonexistent creature entry %u. Skipping.", entry);
            continue;
        }

        TrainerGreetingLocale& var = m_trainerGreetingLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (var.localeText.size() <= static_cast<size_t>(idx))
                        var.localeText.resize(idx + 1);

                    var.localeText[idx] = str;
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u locales trainer greetings.", uint32(m_trainerGreetingLocaleMap.size()));
    sLog.outString();
}

void ObjectMgr::GenerateZoneAndAreaIds()
{
    WorldDatabase.DirectExecute("TRUNCATE creature_zone");
    WorldDatabase.DirectExecute("TRUNCATE gameobject_zone");

    std::string baseCreature = "INSERT INTO creature_zone(Guid, ZoneId, AreaId) VALUES";
    int i = 0;
    int total = 0;
    std::string query = "";
    for (auto& data : mCreatureDataMap)
    {
        CreatureData const& creature = data.second;
        uint32 zoneId, areaId;
        TerrainInfo* info = sTerrainMgr.LoadTerrain(creature.mapid);
        MMAP::MMapFactory::createOrGetMMapManager()->loadMapInstance(sWorld.GetDataPath(), creature.mapid, 0);
        CellPair p = MaNGOS::ComputeCellPair(creature.posX, creature.posY);
        Cell cell(p);
        GridPair gp(cell.GridX(), cell.GridY());
        int gx = (MAX_NUMBER_OF_GRIDS - 1) - gp.x_coord;
        int gy = (MAX_NUMBER_OF_GRIDS - 1) - gp.y_coord;
        info->LoadMapAndVMap(gx, gy);
        info->GetZoneAndAreaId(zoneId, areaId, creature.posX, creature.posY, creature.posZ);

        query += "(" + std::to_string(data.first) + "," + std::to_string(zoneId) + "," + std::to_string(areaId) + "),";
        ++i; ++total;
        if (i >= 100)
        {
            std::string finalQuery = baseCreature + query;
            finalQuery[finalQuery.length() - 1] = ';';
            WorldDatabase.DirectExecute(finalQuery.c_str());
            query = "";
            i = 0;
        }
    }

    std::string baseGo = "INSERT INTO gameobject_zone(Guid, ZoneId, AreaId) VALUES";
    for (auto& data : mGameObjectDataMap)
    {
        GameObjectData const& go = data.second;
        uint32 zoneId, areaId;
        TerrainInfo* info = sTerrainMgr.LoadTerrain(go.mapid);
        MMAP::MMapFactory::createOrGetMMapManager()->loadMapInstance(sWorld.GetDataPath(), go.mapid, 0);
        CellPair p = MaNGOS::ComputeCellPair(go.posX, go.posY);
        Cell cell(p);
        GridPair gp(cell.GridX(), cell.GridY());
        int gx = (MAX_NUMBER_OF_GRIDS - 1) - gp.x_coord;
        int gy = (MAX_NUMBER_OF_GRIDS - 1) - gp.y_coord;
        info->LoadMapAndVMap(gx, gy);
        info->GetZoneAndAreaId(zoneId, areaId, go.posX, go.posY, go.posZ + 1);

        query += "(" + std::to_string(data.first) + "," + std::to_string(zoneId) + "," + std::to_string(areaId) + "),";
        ++i; ++total;
        if (i >= 100)
        {
            std::string finalQuery = baseGo + query;
            finalQuery[finalQuery.length() - 1] = ';';
            WorldDatabase.DirectExecute(finalQuery.c_str());
            query = "";
            i = 0;
        }
    }
}

// not very fast function but it is called only once a day, or on starting-up
/// @param serverUp true if the server is already running, false when the server is started
void ObjectMgr::ReturnOrDeleteOldMails(bool serverUp)
{
    time_t basetime = time(nullptr);
    DEBUG_LOG("Returning mails current time: hour: %d, minute: %d, second: %d ", localtime(&basetime)->tm_hour, localtime(&basetime)->tm_min, localtime(&basetime)->tm_sec);
    // delete all old mails without item and without body immediately, if starting server
    if (!serverUp)
        CharacterDatabase.PExecute("DELETE FROM mail WHERE expire_time < '" UI64FMTD "' AND has_items = '0' AND body = ''", (uint64)basetime);
    //                                                  0  1           2      3        4         5           6   7       8
    auto queryResult = CharacterDatabase.PQuery("SELECT id,messageType,sender,receiver,has_items,expire_time,cod,checked,mailTemplateId FROM mail WHERE expire_time < '" UI64FMTD "'", (uint64)basetime);
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Only expired mails (need to be return or delete) or DB table `mail` is empty.");
        sLog.outString();
        return;                                             // any mails need to be returned or deleted
    }

    // std::ostringstream delitems, delmails; // will be here for optimization
    // bool deletemail = false, deleteitem = false;
    // delitems << "DELETE FROM item_instance WHERE guid IN ( ";
    // delmails << "DELETE FROM mail WHERE id IN ( "

    BarGoLink bar(queryResult->GetRowCount());
    uint32 count = 0;

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();
        Mail* m = new Mail;
        m->messageID = fields[0].GetUInt32();
        m->messageType = fields[1].GetUInt8();
        m->sender = fields[2].GetUInt32();
        m->receiverGuid = ObjectGuid(HIGHGUID_PLAYER, fields[3].GetUInt32());
        bool has_items = fields[4].GetBool();
        m->expire_time = (time_t)fields[5].GetUInt64();
        m->deliver_time = 0;
        m->COD = fields[6].GetUInt32();
        m->checked = fields[7].GetUInt32();
        m->mailTemplateId = fields[8].GetInt16();

        Player* pl = nullptr;
        if (serverUp)
            pl = GetPlayer(m->receiverGuid);
        if (pl)
        {
            // this code will run very improbably (the time is between 4 and 5 am, in game is online a player, who has old mail
            // his in mailbox and he has already listed his mails )
            delete m;
            continue;
        }
        // delete or return mail:
        if (has_items)
        {
            auto resultItems = CharacterDatabase.PQuery("SELECT item_guid,item_template FROM mail_items WHERE mail_id='%u'", m->messageID);
            if (resultItems)
            {
                do
                {
                    Field* fields2 = resultItems->Fetch();

                    uint32 item_guid_low = fields2[0].GetUInt32();
                    uint32 item_template = fields2[1].GetUInt32();

                    m->AddItem(item_guid_low, item_template);
                }
                while (resultItems->NextRow());
            }
            // if it is mail from non-player, or if it's already return mail, it shouldn't be returned, but deleted
            if (m->messageType != MAIL_NORMAL || (m->checked & (MAIL_CHECK_MASK_COD_PAYMENT | MAIL_CHECK_MASK_RETURNED)))
            {
                // mail open and then not returned
                for (auto& item : m->items)
                    CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = '%u'", item.item_guid);
            }
            else
            {
                // mail will be returned:
                CharacterDatabase.PExecute("UPDATE mail SET sender = '%u', receiver = '%u', expire_time = '" UI64FMTD "', deliver_time = '" UI64FMTD "',cod = '0', checked = '%u' WHERE id = '%u'",
                                           m->receiverGuid.GetCounter(), m->sender, (uint64)basetime + 30 * DAY, (uint64)basetime, MAIL_CHECK_MASK_RETURNED, m->messageID);
                for (MailItemInfoVec::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                {
                    // update receiver in mail items for its proper delivery, and in instance_item for avoid lost item at sender delete
                    CharacterDatabase.PExecute("UPDATE mail_items SET receiver = %u WHERE item_guid = '%u'", m->sender, itr2->item_guid);
                    CharacterDatabase.PExecute("UPDATE item_instance SET owner_guid = %u WHERE guid = '%u'", m->sender, itr2->item_guid);
                }
                delete m;
                continue;
            }
        }

        // deletemail = true;
        // delmails << m->messageID << ", ";
        CharacterDatabase.PExecute("DELETE FROM mail WHERE id = '%u'", m->messageID);
        delete m;
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u mails", count);
    sLog.outString();
}

void ObjectMgr::LoadQuestAreaTriggers()
{
    mQuestAreaTriggerMap.clear();                           // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT id,quest FROM areatrigger_involvedrelation");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u quest trigger points", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        ++count;
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID   = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Table `areatrigger_involvedrelation` has area trigger (ID: %u) not listed in `AreaTrigger.dbc`.", trigger_ID);
            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);
        if (!quest)
        {
            sLog.outErrorDb("Table `areatrigger_involvedrelation` has record (id: %u) for not existing quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
        {
            sLog.outErrorDb("Table `areatrigger_involvedrelation` has record (id: %u) for not quest %u, but quest not have flag QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT. Trigger or quest flags must be fixed, quest modified to require objective.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT);

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        mQuestAreaTriggerMap[trigger_ID] = quest_ID;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u quest trigger points", count);
    sLog.outString();
}

void ObjectMgr::LoadTavernAreaTriggers()
{
    mTavernAreaTriggerSet.clear();                          // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT id FROM areatrigger_tavern");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u tavern triggers", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        ++count;
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Table `areatrigger_tavern` has area trigger (ID:%u) not listed in `AreaTrigger.dbc`.", Trigger_ID);
            continue;
        }

        mTavernAreaTriggerSet.insert(Trigger_ID);
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u tavern triggers", count);
    sLog.outString();
}

bool ObjectMgr::AddTaxiShortcut(TaxiPathEntry const* path, uint32 lengthTakeoff, uint32 lengthLanding)
{
    if (!path)
        return false;

    auto shortcut = m_TaxiShortcutMap.find(path->ID);
    if (shortcut == m_TaxiShortcutMap.end())
    {
        TaxiShortcutData data;
        data.lengthTakeoff = lengthTakeoff;
        data.lengthLanding = lengthLanding;
        m_TaxiShortcutMap.insert(TaxiShortcutMap::value_type(path->ID, data));
        return true;
    }
    // Already exists
    return false;
}

bool ObjectMgr::GetTaxiShortcut(uint32 pathid, TaxiShortcutData& data)
{
    auto shortcut = m_TaxiShortcutMap.find(pathid);

    // No record for this path
    if (shortcut == m_TaxiShortcutMap.end())
        return false;

    data = (*shortcut).second;
    return true;
}

void ObjectMgr::LoadTaxiShortcuts()
{
    m_TaxiShortcutMap.clear();                              // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT pathid,takeoff,landing FROM taxi_shortcuts");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u taxi shortcuts", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(int(queryResult->GetRowCount()));

    do
    {
        ++count;
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 pathid = fields[0].GetUInt32();
        uint32 takeoff = fields[1].GetUInt32();
        uint32 landing = fields[2].GetUInt32();

        TaxiPathEntry const* path = sTaxiPathStore.LookupEntry(pathid);
        if (!path)
        {
            sLog.outErrorDb("Table `taxi_shortcuts` has a record for non-existent taxi path id %u, skipped.", pathid);
            continue;
        }

        if (!takeoff && !landing)
        {
            sLog.outErrorDb("Table `taxi_shortcuts` has a useless record for taxi path id %u: takeoff and landing lengths are missing, skipped.", pathid);
            continue;
        }

        TaxiPathNodeList const& waypoints = sTaxiPathNodesByPath[pathid];
        const size_t bounds = waypoints.size();

        if (takeoff >= bounds || landing >= bounds)
        {
            sLog.outErrorDb("Table `taxi_shortcuts` has a malformed record for taxi path id %u: lengths are out of bounds, skipped.", pathid);
            continue;
        }

        if (!AddTaxiShortcut(path, takeoff, landing))
            sLog.outErrorDb("Table `taxi_shortcuts` has a duplicate record for taxi path id %u, skipped.", pathid);
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u taxi shortcuts", count);
    sLog.outString();
}

uint32 ObjectMgr::GetNearestTaxiNode(float x, float y, float z, uint32 mapid, Team team) const
{
    bool found = false;
    float dist = std::numeric_limits<float>::max();
    uint32 id = 0;

    for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
    {
        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);
        if (!node || node->map_id != mapid || !node->MountCreatureID[team == ALLIANCE ? 1 : 0])
            continue;

        uint8  field   = (uint8)((i - 1) / 32);
        uint32 submask = 1 << ((i - 1) % 32);

        // skip not taxi network nodes
        if ((sTaxiNodesMask[field] & submask) == 0)
            continue;

        float dist2 = (node->x - x) * (node->x - x) + (node->y - y) * (node->y - y) + (node->z - z) * (node->z - z);
        if (found)
        {
            if (dist2 < dist)
            {
                dist = dist2;
                id = i;
            }
        }
        else
        {
            found = true;
            dist = dist2;
            id = i;
        }
    }

    return id;
}

void ObjectMgr::GetTaxiPath(uint32 source, uint32 destination, uint32& path, uint32& cost) const
{
    TaxiPathSetBySource::iterator src_i = sTaxiPathSetBySource.find(source);
    if (src_i == sTaxiPathSetBySource.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    TaxiPathSetForSource& pathSet = src_i->second;

    TaxiPathSetForSource::iterator dest_i = pathSet.find(destination);
    if (dest_i == pathSet.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    cost = dest_i->second.price;
    path = dest_i->second.ID;
}

uint32 ObjectMgr::GetTaxiMountDisplayId(uint32 id, Team team) const
{
    uint16 mount_entry = 0;

    // select mount creature id
    TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(id);
    if (node)
    {
        if (team == ALLIANCE)
        {
            mount_entry = node->MountCreatureID[1];
            if (!mount_entry)
                mount_entry = node->MountCreatureID[0];
        }
        else if (team == HORDE)
        {
            mount_entry = node->MountCreatureID[0];
            if (!mount_entry)
                mount_entry = node->MountCreatureID[1];
        }
    }

    CreatureInfo const* mount_info = GetCreatureTemplate(mount_entry);
    if (!mount_info)
        return 0;

    uint16 mount_id = Creature::ChooseDisplayId(mount_info);
    if (!mount_id)
        return 0;

    CreatureModelInfo const* minfo = GetCreatureModelRandomGender(mount_id);
    if (minfo)
        mount_id = minfo->modelid;

    return mount_id;
}

void ObjectMgr::LoadAreaTriggerTeleports()
{
    mAreaTriggers.clear();                                  // need for reload case

    uint32 count = 0;

    //                                             0   1               2              3               4           5            6                    7                           8           9                  10                 11                 12                  13
    auto queryResult = WorldDatabase.Query("SELECT id, required_level, required_item, required_item2, heroic_key, heroic_key2, required_quest_done, required_quest_done_heroic, target_map, target_position_x, target_position_y, target_position_z, target_orientation, condition_id FROM areatrigger_teleport");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u area trigger teleport definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        ++count;

        uint32 Trigger_ID = fields[0].GetUInt32();

        AreaTrigger at;

        at.requiredLevel        = fields[1].GetUInt8();
        at.requiredItem         = fields[2].GetUInt32();
        at.requiredItem2        = fields[3].GetUInt32();
        at.heroicKey            = fields[4].GetUInt32();
        at.heroicKey2           = fields[5].GetUInt32();
        at.requiredQuest        = fields[6].GetUInt32();
        at.requiredQuestHeroic  = fields[7].GetUInt32();
        at.target_mapId         = fields[8].GetUInt32();
        at.target_X             = fields[9].GetFloat();
        at.target_Y             = fields[10].GetFloat();
        at.target_Z             = fields[11].GetFloat();
        at.target_Orientation   = fields[12].GetFloat();
        at.conditionId          = fields[13].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has area trigger (ID:%u) not listed in `AreaTrigger.dbc`.", Trigger_ID);
            continue;
        }

        if (at.requiredItem)
        {
            ItemPrototype const* pProto = GetItemPrototype(at.requiredItem);
            if (!pProto)
            {
                sLog.outError("Table `areatrigger_teleport` has nonexistent key item %u for trigger %u, removing key requirement.", at.requiredItem, Trigger_ID);
                at.requiredItem = 0;
            }
        }

        if (at.requiredItem2)
        {
            ItemPrototype const* pProto = GetItemPrototype(at.requiredItem2);
            if (!pProto)
            {
                sLog.outError("Table `areatrigger_teleport` has nonexistent second key item %u for trigger %u, remove key requirement.", at.requiredItem2, Trigger_ID);
                at.requiredItem2 = 0;
            }
        }

        if (at.heroicKey)
        {
            ItemPrototype const* pProto = GetItemPrototype(at.heroicKey);
            if (!pProto)
            {
                sLog.outError("Table `areatrigger_teleport` has nonexistent heroic key item %u for trigger %u, remove key requirement.", at.heroicKey, Trigger_ID);
                at.heroicKey = 0;
            }
        }

        if (at.heroicKey2)
        {
            ItemPrototype const* pProto = GetItemPrototype(at.heroicKey2);
            if (!pProto)
            {
                sLog.outError("Table `areatrigger_teleport` has nonexistent heroic second key item %u for trigger %u, remove key requirement.", at.heroicKey2, Trigger_ID);
                at.heroicKey2 = 0;
            }
        }

        if (at.requiredQuest)
        {
            QuestMap::iterator qReqItr = mQuestTemplates.find(at.requiredQuest);
            if (qReqItr == mQuestTemplates.end())
            {
                sLog.outErrorDb("Table `areatrigger_teleport` has nonexistent required quest %u for trigger %u, remove quest done requirement.", at.requiredQuest, Trigger_ID);
                at.requiredQuest = 0;
            }
        }

        if (at.requiredQuestHeroic)
        {
            QuestMap::iterator qReqItr = mQuestTemplates.find(at.requiredQuestHeroic);
            if (qReqItr == mQuestTemplates.end())
            {
                sLog.outErrorDb("Table `areatrigger_teleport` has nonexistent required heroic quest %u for trigger %u, remove quest done requirement.", at.requiredQuestHeroic, Trigger_ID);
                at.requiredQuestHeroic = 0;
            }
        }

        if (at.conditionId)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(at.conditionId);
            if (!condition) // condition does not exist for some reason
                sLog.outErrorDb("Table `areatrigger_teleport` entry %u has `condition_id` = %u but does not exist.", Trigger_ID, at.conditionId);
        }

        MapEntry const* mapEntry = sMapStore.LookupEntry(at.target_mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has nonexistent target map (ID: %u) for Area trigger (ID:%u).", at.target_mapId, Trigger_ID);
            continue;
        }

        if (at.target_X == 0 && at.target_Y == 0 && at.target_Z == 0)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has area trigger (ID:%u) without target coordinates.", Trigger_ID);
            continue;
        }

        mAreaTriggers[Trigger_ID] = at;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u area trigger teleport definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadAccessRequirements()
{
    m_accessRequirements.clear();

    //                                                              0      1           2          3          4           5      6             7             8                      9     10
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT mapid, difficulty, level_min, level_max, item_level, item, item2, quest_done_A, quest_done_H, completed_achievement, quest_failed_text FROM access_requirement"));

    uint32 count = 0;
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u access_requirement definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();

        bar.step();

        uint32 mapid = fields[0].GetUInt32();
        uint8 difficulty = fields[1].GetUInt8();
        uint32 requirement_ID = MAKE_PAIR32(mapid, difficulty);

        AccessRequirement ar;

        ar.levelMin = fields[2].GetUInt8();
        ar.levelMax = fields[3].GetUInt8();
        ar.item_level = fields[4].GetUInt16();
        ar.item = fields[5].GetUInt32();
        ar.item2 = fields[6].GetUInt32();
        ar.quest_A = fields[7].GetUInt32();
        ar.quest_H = fields[8].GetUInt32();
        ar.achievement = fields[9].GetUInt32();
        ar.questFailedText = fields[10].GetString();

        if (ar.item)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.item);
            if (!pProto)
            {
                sLog.outString("Key item %u does not exist for map %u difficulty %u, removing key requirement.", ar.item, mapid, difficulty);
                ar.item = 0;
            }
        }

        if (ar.item2)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.item2);
            if (!pProto)
            {
                sLog.outString("Second key item %u does not exist for map %u difficulty %u, removing key requirement.", ar.item2, mapid, difficulty);
                ar.item2 = 0;
            }
        }

        if (ar.quest_A)
        {
            if (!GetQuestTemplate(ar.quest_A))
            {
                sLog.outString("Required Alliance Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar.quest_A, mapid, difficulty);
                ar.quest_A = 0;
            }
        }

        if (ar.quest_H)
        {
            if (!GetQuestTemplate(ar.quest_H))
            {
                sLog.outString("Required Horde Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar.quest_H, mapid, difficulty);
                ar.quest_H = 0;
            }
        }

        if (ar.achievement)
        {
            if (!sAchievementMgr.GetAchievementCriteriaByAchievement(ar.achievement))
            {
                sLog.outString("Required Achievement %u not exist for map %u difficulty %u, remove achievement requirement.", ar.achievement, mapid, difficulty);
                ar.achievement = 0;
            }
        }

        m_accessRequirements.emplace(requirement_ID, ar);
    } while (result->NextRow());
}

/*
 * Searches for the areatrigger which teleports players out of the given map (only direct to continent)
 */
AreaTrigger const* ObjectMgr::GetGoBackTrigger(uint32 map_id) const
{
    const MapEntry* mapEntry = sMapStore.LookupEntry(map_id);
    if (!mapEntry || mapEntry->ghost_entrance_map < 0)
        return nullptr;

    // Try to find one that teleports to the map we want to enter
    std::list<AreaTrigger const*> ghostTrigger;
    AreaTrigger const* compareTrigger = nullptr;
    for (const auto& mAreaTrigger : mAreaTriggers)
    {
        if (mAreaTrigger.second.target_mapId == uint32(mapEntry->ghost_entrance_map))
        {
            ghostTrigger.push_back(&mAreaTrigger.second);
            // First run, only consider AreaTrigger that teleport in the proper map
            if ((!compareTrigger || mAreaTrigger.second.IsLessOrEqualThan(compareTrigger)) && sAreaTriggerStore.LookupEntry(mAreaTrigger.first)->mapid == map_id)
            {
                if (mAreaTrigger.second.IsMinimal())
                    return &mAreaTrigger.second;

                compareTrigger = &mAreaTrigger.second;
            }
        }
    }
    if (compareTrigger)
        return compareTrigger;

    // Second attempt: take one fitting
    for (std::list<AreaTrigger const*>::const_iterator itr = ghostTrigger.begin(); itr != ghostTrigger.end(); ++itr)
    {
        if (!compareTrigger || (*itr)->IsLessOrEqualThan(compareTrigger))
        {
            if ((*itr)->IsMinimal())
                return *itr;

            compareTrigger = *itr;
        }
    }
    return compareTrigger;
}

/**
 * Searches for the areatrigger which teleports players to the given map
 * TODO: Requirements should be propably Map bound not Areatrigger bound
 */
AreaTrigger const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    AreaTrigger const* compareTrigger = nullptr;
    MapEntry const* mEntry = sMapStore.LookupEntry(Map);

    for (const auto& mAreaTrigger : mAreaTriggers)
    {
        if (mAreaTrigger.second.target_mapId == Map)
        {
            if (mEntry->Instanceable())
            {
                // Remark that IsLessOrEqualThan is no total order, and a->IsLeQ(b) != !b->IsLeQ(a)
                if (!compareTrigger || compareTrigger->IsLessOrEqualThan(&mAreaTrigger.second))
                    compareTrigger = &mAreaTrigger.second;
            }
            else
            {
                if (!compareTrigger || mAreaTrigger.second.IsLessOrEqualThan(compareTrigger))
                {
                    if (mAreaTrigger.second.IsMinimal())
                        return &mAreaTrigger.second;

                    compareTrigger = &mAreaTrigger.second;
                }
            }
        }
    }
    return compareTrigger;
}

void ObjectMgr::PackGroupIds()
{
    // this routine renumbers groups in such a way so they start from 1 and go up

    // obtain set of all groups
    std::set<uint32> groupIds;

    // all valid ids are in the instance table
    // any associations to ids not in this table are assumed to be
    // cleaned already in CleanupInstances
    auto queryResult = CharacterDatabase.Query("SELECT groupId FROM `groups`");
    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();

            uint32 id = fields[0].GetUInt32();

            if (id == 0)
            {
                CharacterDatabase.BeginTransaction();
                CharacterDatabase.PExecute("DELETE FROM `groups` WHERE groupId = '%u'", id);
                CharacterDatabase.PExecute("DELETE FROM group_member WHERE groupId = '%u'", id);
                CharacterDatabase.CommitTransaction();
                continue;
            }

            groupIds.insert(id);
        }
        while (queryResult->NextRow());
    }

    BarGoLink bar(groupIds.size() + 1);
    bar.step();

    uint32 groupId = 1;
    // we do assume std::set is sorted properly on integer value
    for (uint32 i : groupIds)
    {
        if (i != groupId)
        {
            // remap group id
            CharacterDatabase.BeginTransaction();
            CharacterDatabase.PExecute("UPDATE `groups` SET groupId = '%u' WHERE groupId = '%u'", groupId, i);
            CharacterDatabase.PExecute("UPDATE group_member SET groupId = '%u' WHERE groupId = '%u'", groupId, i);
            CharacterDatabase.CommitTransaction();
        }

        ++groupId;
        bar.step();
    }

    sLog.outString(">> Group Ids remapped, next group id is %u", groupId);
    sLog.outString();
}

void ObjectMgr::SetHighestGuids()
{
    auto result = CharacterDatabase.Query("SELECT MAX(guid) FROM characters");
    if (result)
    {
        m_CharGuids.Set((*result)[0].GetUInt32() + 1);
    }

    result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");
    if (result)
    {
        m_FirstTemporaryCreatureGuid = (*result)[0].GetUInt32() + 1;
    }

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM item_instance");
    if (result)
    {
        m_ItemGuids.Set((*result)[0].GetUInt32() + 1);
    }

    uint32 newInstanceId = 0;
    result = CharacterDatabase.Query("SELECT MAX(id) FROM instance");
    if (result)
    {
        newInstanceId = (*result)[0].GetUInt32() + 1;
    }

    if (newInstanceId < 2) //Instance id 0 and 1 are taken by ebon hold.
        newInstanceId = 2;

    m_InstanceGuids.Set(newInstanceId);

    // Cleanup other tables from nonexistent guids (>=m_hiItemGuid)
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE item >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.PExecute("DELETE FROM mail_items WHERE item_guid >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.PExecute("DELETE FROM auction WHERE itemguid >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.PExecute("DELETE FROM guild_bank_item WHERE item_guid >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.CommitTransaction();

    result = WorldDatabase.Query("SELECT MAX(guid) FROM gameobject");
    if (result)
    {
        m_FirstTemporaryGameObjectGuid = (*result)[0].GetUInt32() + 1;
    }

    result = CharacterDatabase.Query("SELECT MAX(id) FROM auction");
    if (result)
    {
        m_AuctionIds.Set((*result)[0].GetUInt32() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(id) FROM mail");
    if (result)
    {
        m_MailIds.Set((*result)[0].GetUInt32() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM corpse");
    if (result)
    {
        m_CorpseGuids.Set((*result)[0].GetUInt32() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(arenateamid) FROM arena_team");
    if (result)
    {
        m_ArenaTeamIds.Set((*result)[0].GetUInt32() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(setguid) FROM character_equipmentsets");
    if (result)
    {
        m_EquipmentSetIds.Set((*result)[0].GetUInt64() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(guildid) FROM guild");
    if (result)
    {
        m_GuildIds.Set((*result)[0].GetUInt32() + 1);
    }

    result = CharacterDatabase.Query("SELECT MAX(groupId) FROM `groups`");
    if (result)
    {
        m_GroupGuids.Set((*result)[0].GetUInt32() + 1);
    }

    // setup reserved ranges for static guids spawn
    m_StaticCreatureGuids.Set(m_FirstTemporaryCreatureGuid);
    m_FirstTemporaryCreatureGuid += sWorld.getConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE);

    m_StaticGameObjectGuids.Set(m_FirstTemporaryGameObjectGuid);
    m_FirstTemporaryGameObjectGuid += sWorld.getConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT);
}

void ObjectMgr::LoadGameObjectLocales()
{
    mGameObjectLocaleMap.clear();                           // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT entry,"
                          "name_loc1,name_loc2,name_loc3,name_loc4,name_loc5,name_loc6,name_loc7,name_loc8,"
                          "castbarcaption_loc1,castbarcaption_loc2,castbarcaption_loc3,castbarcaption_loc4,"
                          "castbarcaption_loc5,castbarcaption_loc6,castbarcaption_loc7,castbarcaption_loc8 FROM locales_gameobject");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 gameobject locale strings. DB table `locales_gameobject` is empty.");
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 entry = fields[0].GetUInt32();

        if (!GetGameObjectInfo(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_gameobject` has data for nonexistent gameobject entry %u, skipped.", entry);
            continue;
        }

        GameObjectLocale& data = mGameObjectLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
        }

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i + (MAX_LOCALE - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.CastBarCaption.size() <= idx)
                        data.CastBarCaption.resize(idx + 1);

                    data.CastBarCaption[idx] = str;
                }
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded " SIZEFMTD " gameobject locale strings", mGameObjectLocaleMap.size());
    sLog.outString();
}

struct SQLGameObjectLoader : public SQLStorageLoaderBase<SQLGameObjectLoader, SQLHashStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptDevAIMgr.GetScriptId(src));
    }
};

inline void CheckGOLockId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (sLockStore.LookupEntry(dataN))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but lock (Id: %u) not found.",
                    goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckGOLinkedTrapId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (GameObjectInfo const* trapInfo = sGOStorage.LookupEntry<GameObjectInfo>(dataN))
    {
        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
            sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but GO (Entry %u) have not GAMEOBJECT_TYPE_TRAP (%u) type.",
                            goInfo->id, goInfo->type, N, dataN, dataN, GAMEOBJECT_TYPE_TRAP);
    }
    else
        // too many error reports about nonexistent trap templates
        ERROR_DB_STRICT_LOG("Gameobject (Entry: %u GoType: %u) have data%d=%u but trap GO (Entry %u) not exist in `gameobject_template`.",
                            goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckGOSpellId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (sSpellTemplate.LookupEntry<SpellEntry>(dataN))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but Spell (Entry %u) not exist.",
                    goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckAndFixGOChairHeightId(GameObjectInfo const* goInfo, uint32 const& dataN, uint32 N)
{
    if (dataN <= (UNIT_STAND_STATE_SIT_HIGH_CHAIR - UNIT_STAND_STATE_SIT_LOW_CHAIR))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but correct chair height in range 0..%i.",
                    goInfo->id, goInfo->type, N, dataN, UNIT_STAND_STATE_SIT_HIGH_CHAIR - UNIT_STAND_STATE_SIT_LOW_CHAIR);

    // prevent client and server unexpected work
    const_cast<uint32&>(dataN) = 0;
}

inline void CheckGONoDamageImmuneId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) noDamageImmune field value.",
                    goInfo->id, goInfo->type, N, dataN);
}

inline void CheckGOConsumable(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) consumable field value.",
                    goInfo->id, goInfo->type, N, dataN);
}

inline void CheckAndFixGOCaptureMinTime(GameObjectInfo const* goInfo, uint32 const& dataN, uint32 N)
{
    if (dataN > 0)
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data%d=%u but minTime field value must be > 0.",
                    goInfo->id, goInfo->type, N, dataN);

    // prevent division through 0 exception
    const_cast<uint32&>(dataN) = 1;
}

std::vector<uint32> ObjectMgr::LoadGameobjectInfo()
{
    SQLGameObjectLoader loader;
    loader.Load(sGOStorage);

    std::vector<uint32> transportDisplayIds;

    // some checks
    for (SQLStorageBase::SQLSIterator<GameObjectInfo> itr = sGOStorage.getDataBegin<GameObjectInfo>(); itr < sGOStorage.getDataEnd<GameObjectInfo>(); ++itr)
    {
        GameObjectInfo const* goInfo = itr.getValue();

        if (goInfo->size <= 0.0f)                           // prevent use too small scales
        {
            ERROR_DB_STRICT_LOG("Gameobject (Entry: %u GoType: %u) have too small size=%f",
                                goInfo->id, goInfo->type, goInfo->size);
            const_cast<GameObjectInfo*>(goInfo)->size =  DEFAULT_OBJECT_SCALE;
        }

        if (goInfo->StringId && !sScriptMgr.ExistsStringId(goInfo->StringId))
        {
            sLog.outErrorDb("Table creature_template entry %u StringID2 %u does not exist. Setting to 0.", goInfo->id, goInfo->StringId);
            const_cast<GameObjectInfo*>(goInfo)->StringId = 0;
        }

        // some GO types have unused go template, check goInfo->displayId at GO spawn data loading or ignore

        switch (goInfo->type)
        {
            case GAMEOBJECT_TYPE_DOOR:                      // 0
            {
                if (goInfo->door.lockId)
                    CheckGOLockId(goInfo, goInfo->door.lockId, 1);
                CheckGONoDamageImmuneId(goInfo, goInfo->door.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BUTTON:                    // 1
            {
                if (goInfo->button.lockId)
                    CheckGOLockId(goInfo, goInfo->button.lockId, 1);
                if (goInfo->button.linkedTrapId)            // linked trap
                    CheckGOLinkedTrapId(goInfo, goInfo->button.linkedTrapId, 3);
                CheckGONoDamageImmuneId(goInfo, goInfo->button.noDamageImmune, 4);
                break;
            }
            case GAMEOBJECT_TYPE_QUESTGIVER:                // 2
            {
                if (goInfo->questgiver.lockId)
                    CheckGOLockId(goInfo, goInfo->questgiver.lockId, 0);
                CheckGONoDamageImmuneId(goInfo, goInfo->questgiver.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_CHEST:                     // 3
            {
                if (goInfo->chest.lockId)
                    CheckGOLockId(goInfo, goInfo->chest.lockId, 0);

                CheckGOConsumable(goInfo, goInfo->chest.consumable, 3);

                if (goInfo->chest.linkedTrapId)             // linked trap
                    CheckGOLinkedTrapId(goInfo, goInfo->chest.linkedTrapId, 7);
                break;
            }
            case GAMEOBJECT_TYPE_TRAP:                      // 6
            {
                if (goInfo->trap.lockId)
                    CheckGOLockId(goInfo, goInfo->trap.lockId, 0);
                /* disable check for while, too many nonexistent spells
                if (goInfo->trap.spellId)                   // spell
                    CheckGOSpellId(goInfo,goInfo->trap.spellId,3);
                */
                break;
            }
            case GAMEOBJECT_TYPE_CHAIR:                     // 7
                CheckAndFixGOChairHeightId(goInfo, goInfo->chair.height, 1);
                break;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:               // 8
            {
                if (goInfo->spellFocus.focusId)
                {
                    if (!sSpellFocusObjectStore.LookupEntry(goInfo->spellFocus.focusId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data0=%u but SpellFocus (Id: %u) not exist.",
                                        goInfo->id, goInfo->type, goInfo->spellFocus.focusId, goInfo->spellFocus.focusId);
                }

                if (goInfo->spellFocus.linkedTrapId)        // linked trap
                    CheckGOLinkedTrapId(goInfo, goInfo->spellFocus.linkedTrapId, 2);
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:                    // 10
            {
                if (goInfo->goober.lockId)
                    CheckGOLockId(goInfo, goInfo->goober.lockId, 0);

                CheckGOConsumable(goInfo, goInfo->goober.consumable, 3);

                if (goInfo->goober.pageId)                  // pageId
                {
                    if (!sPageTextStore.LookupEntry<PageText>(goInfo->goober.pageId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data7=%u but PageText (Entry %u) not exist.",
                                        goInfo->id, goInfo->type, goInfo->goober.pageId, goInfo->goober.pageId);
                }
                /* disable check for while, too many nonexistent spells
                if (goInfo->goober.spellId)                 // spell
                    CheckGOSpellId(goInfo,goInfo->goober.spellId,10);
                */
                CheckGONoDamageImmuneId(goInfo, goInfo->goober.noDamageImmune, 11);
                if (goInfo->goober.linkedTrapId)            // linked trap
                    CheckGOLinkedTrapId(goInfo, goInfo->goober.linkedTrapId, 12);
                break;
            }
            case GAMEOBJECT_TYPE_AREADAMAGE:                // 12
            {
                if (goInfo->areadamage.lockId)
                    CheckGOLockId(goInfo, goInfo->areadamage.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CAMERA:                    // 13
            {
                if (goInfo->camera.lockId)
                    CheckGOLockId(goInfo, goInfo->camera.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_TRANSPORT:
                transportDisplayIds.push_back(goInfo->displayId);
                break;
            case GAMEOBJECT_TYPE_MO_TRANSPORT:              // 15
            {
                if (goInfo->moTransport.taxiPathId)
                {
                    if (goInfo->moTransport.taxiPathId >= sTaxiPathNodesByPath.size() || sTaxiPathNodesByPath[goInfo->moTransport.taxiPathId].empty())
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data0=%u but TaxiPath (Id: %u) not exist.",
                                        goInfo->id, goInfo->type, goInfo->moTransport.taxiPathId, goInfo->moTransport.taxiPathId);
                }
                if (goInfo->displayId != 462 && goInfo->displayId != 562)
                {
                    transportDisplayIds.push_back(goInfo->displayId);
                    if (goInfo->moTransport.mapID)
                    {
                        m_guidsForMap[goInfo->moTransport.mapID];
                        m_transportMaps.emplace(goInfo->moTransport.mapID, goInfo->id);
                    }
                }
                break;
            }
            case GAMEOBJECT_TYPE_SUMMONING_RITUAL:          // 18
            {
                /* disable check for while, too many nonexistent spells
                // always must have spell
                CheckGOSpellId(goInfo,goInfo->summoningRitual.spellId,1);
                */
                break;
            }
            case GAMEOBJECT_TYPE_SPELLCASTER:               // 22
            {
                // always must have spell
                CheckGOSpellId(goInfo, goInfo->spellcaster.spellId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGSTAND:                 // 24
            {
                if (goInfo->flagstand.lockId)
                    CheckGOLockId(goInfo, goInfo->flagstand.lockId, 0);
                CheckGONoDamageImmuneId(goInfo, goInfo->flagstand.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_FISHINGHOLE:               // 25
            {
                if (goInfo->fishinghole.lockId)
                    CheckGOLockId(goInfo, goInfo->fishinghole.lockId, 4);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGDROP:                  // 26
            {
                if (goInfo->flagdrop.lockId)
                    CheckGOLockId(goInfo, goInfo->flagdrop.lockId, 0);
                CheckGONoDamageImmuneId(goInfo, goInfo->flagdrop.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_CAPTURE_POINT:             // 29
            {
                CheckAndFixGOCaptureMinTime(goInfo, goInfo->capturePoint.minTime, 16);
                break;
            }
            case GAMEOBJECT_TYPE_BARBER_CHAIR:              // 32
                CheckAndFixGOChairHeightId(goInfo, goInfo->barberChair.chairheight, 0);
                break;
        }
    }

    sLog.outString(">> Loaded %u game object templates", sGOStorage.GetRecordCount());
    sLog.outString();

    return transportDisplayIds;
}

void ObjectMgr::LoadExplorationBaseXP()
{
    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT level,basexp FROM exploration_basexp");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u BaseXP definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();
        uint32 level  = fields[0].GetUInt32();
        uint32 basexp = fields[1].GetUInt32();
        mBaseXPTable[level] = basexp;
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u BaseXP definitions", count);
    sLog.outString();
}

uint32 ObjectMgr::GetBaseXP(uint32 level) const
{
    BaseXPMap::const_iterator itr = mBaseXPTable.find(level);
    return itr != mBaseXPTable.end() ? itr->second : 0;
}

uint32 ObjectMgr::GetXPForLevel(uint32 level) const
{
    if (level < mPlayerXPperLevel.size())
        return mPlayerXPperLevel[level];
    return 0;
}

uint32 ObjectMgr::GetMaxLevelForExpansion(uint32 expansion) const
{
    uint32 maxLevel = 60;
    switch (expansion)
    {
        case EXPANSION_TBC: maxLevel = 70; break;
        case EXPANSION_WOTLK: maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); break; // limit latest expansion by config
    }
    return maxLevel;
}

void ObjectMgr::LoadPetNames()
{
    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT word,entry,half FROM pet_name_generation");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u pet name parts", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();
        std::string word = fields[0].GetString();
        uint32 entry     = fields[1].GetUInt32();
        bool   half      = fields[2].GetBool();
        if (half)
            PetHalfName1[entry].push_back(word);
        else
            PetHalfName0[entry].push_back(word);
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u pet name parts", count);
    sLog.outString();
}

void ObjectMgr::LoadPetNumber()
{
    auto queryResult = CharacterDatabase.Query("SELECT MAX(id) FROM character_pet");
    if (queryResult)
    {
        Field* fields = queryResult->Fetch();
        m_PetNumbers.Set(fields[0].GetUInt32() + 1);
    }

    BarGoLink bar(1);
    bar.step();

    sLog.outString(">> Loaded the max pet number: %d", m_PetNumbers.GetNextAfterMaxUsed() - 1);
    sLog.outString();
}

std::string ObjectMgr::GeneratePetName(uint32 entry)
{
    std::vector<std::string>& list0 = PetHalfName0[entry];
    std::vector<std::string>& list1 = PetHalfName1[entry];

    if (list0.empty() || list1.empty())
    {
        CreatureInfo const* cinfo = GetCreatureTemplate(entry);
        char const* petname = GetPetName(cinfo->Family, sWorld.GetDefaultDbcLocale());
        if (!petname)
            petname = cinfo->Name;
        return std::string(petname);
    }

    return *(list0.begin() + urand(0, list0.size() - 1)) + *(list1.begin() + urand(0, list1.size() - 1));
}

void ObjectMgr::LoadCorpses()
{
    uint32 count = 0;
    //                                                 0            1       2                  3                  4                  5                   6
    auto queryResult = CharacterDatabase.Query("SELECT corpse.guid, player, corpse.position_x, corpse.position_y, corpse.position_z, corpse.orientation, corpse.map, "
                          //   7     8            9         10         11      12    13     14           15            16              17       18
                          "time, corpse_type, instance, phaseMask, gender, race, class, playerBytes, playerBytes2, equipmentCache, guildId, playerFlags FROM corpse "
                          "JOIN characters ON player = characters.guid "
                          "LEFT JOIN guild_member ON player=guild_member.guid WHERE corpse_type <> 0");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u corpses", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 guid = fields[0].GetUInt32();

        Corpse* corpse = new Corpse;
        if (!corpse->LoadFromDB(guid, fields))
        {
            delete corpse;
            continue;
        }

        sObjectAccessor.AddCorpse(corpse);

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u corpses", count);
    sLog.outString();
}

void ObjectMgr::LoadReputationRewardRate()
{
    m_RepRewardRateMap.clear();                             // for reload case

    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT faction, quest_rate, creature_rate, spell_rate FROM reputation_reward_rate");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded `reputation_reward_rate`, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 factionId        = fields[0].GetUInt32();

        RepRewardRate repRate;

        repRate.quest_rate      = fields[1].GetFloat();
        repRate.creature_rate   = fields[2].GetFloat();
        repRate.spell_rate      = fields[3].GetFloat();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);
        if (!factionEntry)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_reward_rate`", factionId);
            continue;
        }

        if (repRate.quest_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has quest_rate with invalid rate %f, skipping data for faction %u", repRate.quest_rate, factionId);
            continue;
        }

        if (repRate.creature_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has creature_rate with invalid rate %f, skipping data for faction %u", repRate.creature_rate, factionId);
            continue;
        }

        if (repRate.spell_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has spell_rate with invalid rate %f, skipping data for faction %u", repRate.spell_rate, factionId);
            continue;
        }

        m_RepRewardRateMap[factionId] = repRate;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u reputation_reward_rate", count);
    sLog.outString();
}

void ObjectMgr::LoadReputationOnKill()
{
    uint32 count = 0;

    //                                             0            1                     2
    auto queryResult = WorldDatabase.Query("SELECT creature_id, RewOnKillRepFaction1, RewOnKillRepFaction2,"
                          //   3             4             5                   6             7             8                   9
                          "IsTeamAward1, MaxStanding1, RewOnKillRepValue1, IsTeamAward2, MaxStanding2, RewOnKillRepValue2, TeamDependent "
                          "FROM creature_onkill_reputation");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 creature award reputation definitions. DB table `creature_onkill_reputation` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 creature_id = fields[0].GetUInt32();

        ReputationOnKillEntry repOnKill;
        repOnKill.repfaction1          = fields[1].GetUInt32();
        repOnKill.repfaction2          = fields[2].GetUInt32();
        repOnKill.is_teamaward1        = fields[3].GetBool();
        repOnKill.reputation_max_cap1  = fields[4].GetUInt32();
        repOnKill.repvalue1            = fields[5].GetInt32();
        repOnKill.is_teamaward2        = fields[6].GetBool();
        repOnKill.reputation_max_cap2  = fields[7].GetUInt32();
        repOnKill.repvalue2            = fields[8].GetInt32();
        repOnKill.team_dependent       = fields[9].GetBool();

        if (!GetCreatureTemplate(creature_id))
        {
            sLog.outErrorDb("Table `creature_onkill_reputation` have data for nonexistent creature entry (%u), skipped", creature_id);
            continue;
        }

        if (repOnKill.repfaction1)
        {
            FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repOnKill.repfaction1);
            if (!factionEntry1)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.repfaction1);
                continue;
            }
        }

        if (repOnKill.repfaction2)
        {
            FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repOnKill.repfaction2);
            if (!factionEntry2)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.repfaction2);
                continue;
            }
        }

        mRepOnKill[creature_id] = repOnKill;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u creature award reputation definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadReputationSpilloverTemplate()
{
    m_RepSpilloverTemplateMap.clear();                      // for reload case

    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT faction, faction1, rate_1, rank_1, faction2, rate_2, rank_2, faction3, rate_3, rank_3, faction4, rate_4, rank_4 FROM reputation_spillover_template");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded `reputation_spillover_template`, table is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 factionId                = fields[0].GetUInt32();

        RepSpilloverTemplate repTemplate;

        repTemplate.faction[0]          = fields[1].GetUInt32();
        repTemplate.faction_rate[0]     = fields[2].GetFloat();
        repTemplate.faction_rank[0]     = fields[3].GetUInt32();
        repTemplate.faction[1]          = fields[4].GetUInt32();
        repTemplate.faction_rate[1]     = fields[5].GetFloat();
        repTemplate.faction_rank[1]     = fields[6].GetUInt32();
        repTemplate.faction[2]          = fields[7].GetUInt32();
        repTemplate.faction_rate[2]     = fields[8].GetFloat();
        repTemplate.faction_rank[2]     = fields[9].GetUInt32();
        repTemplate.faction[3]          = fields[10].GetUInt32();
        repTemplate.faction_rate[3]     = fields[11].GetFloat();
        repTemplate.faction_rank[3]     = fields[12].GetUInt32();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (!factionEntry)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", factionId);
            continue;
        }

        if (factionEntry->team == 0)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u in `reputation_spillover_template` does not belong to any team, skipping", factionId);
            continue;
        }

        for (uint32 i = 0; i < MAX_SPILLOVER_FACTIONS; ++i)
        {
            if (repTemplate.faction[i])
            {
                FactionEntry const* factionSpillover = sFactionStore.LookupEntry(repTemplate.faction[i]);

                if (!factionSpillover)
                {
                    sLog.outErrorDb("Spillover faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template` for faction %u, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (factionSpillover->reputationListID < 0)
                {
                    sLog.outErrorDb("Spillover faction (faction.dbc) %u for faction %u in `reputation_spillover_template` can not be listed for client, and then useless, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (repTemplate.faction_rank[i] >= MAX_REPUTATION_RANK)
                {
                    sLog.outErrorDb("Rank %u used in `reputation_spillover_template` for spillover faction %u is not valid, skipping", repTemplate.faction_rank[i], repTemplate.faction[i]);
                }
            }
        }

        FactionEntry const* factionEntry0 = sFactionStore.LookupEntry(repTemplate.faction[0]);
        if (repTemplate.faction[0] && !factionEntry0)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[0]);
            continue;
        }
        FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repTemplate.faction[1]);
        if (repTemplate.faction[1] && !factionEntry1)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[1]);
            continue;
        }
        FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repTemplate.faction[2]);
        if (repTemplate.faction[2] && !factionEntry2)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[2]);
            continue;
        }
        FactionEntry const* factionEntry3 = sFactionStore.LookupEntry(repTemplate.faction[3]);
        if (repTemplate.faction[3] && !factionEntry3)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[3]);
            continue;
        }

        m_RepSpilloverTemplateMap[factionId] = repTemplate;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u reputation_spillover_template", count);
    sLog.outString();
}

void ObjectMgr::LoadPointsOfInterest()
{
    mPointsOfInterest.clear();                              // need for reload case

    uint32 count = 0;

    //                                             0      1  2  3      4     5
    auto queryResult = WorldDatabase.Query("SELECT entry, x, y, icon, flags, data, icon_name FROM points_of_interest");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 Points of Interest definitions. DB table `points_of_interest` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 point_id = fields[0].GetUInt32();

        PointOfInterest POI;
        POI.x                    = fields[1].GetFloat();
        POI.y                    = fields[2].GetFloat();
        POI.icon                 = fields[3].GetUInt32();
        POI.flags                = fields[4].GetUInt32();
        POI.data                 = fields[5].GetUInt32();
        POI.icon_name            = fields[6].GetCppString();

        if (!MaNGOS::IsValidMapCoord(POI.x, POI.y))
        {
            sLog.outErrorDb("Table `points_of_interest` (Entry: %u) have invalid coordinates (X: %f Y: %f), ignored.", point_id, POI.x, POI.y);
            continue;
        }

        mPointsOfInterest[point_id] = POI;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u Points of Interest definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadQuestPOI()
{
    mQuestPOIMap.clear();                              // need for reload case

    uint32 count = 0;

    //                                             0        1      2         3      4          5        6     7
    auto queryResult = WorldDatabase.Query("SELECT questId, poiId, objIndex, mapId, mapAreaId, floorId, unk3, unk4 FROM quest_poi");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 quest POI definitions. DB table `quest_poi` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 questId          = fields[0].GetUInt32();
        uint32 poiId            = fields[1].GetUInt32();
        int32  objIndex         = fields[2].GetInt32();
        uint32 mapId            = fields[3].GetUInt32();
        uint32 mapAreaId        = fields[4].GetUInt32();
        uint32 floorId          = fields[5].GetUInt32();
        uint32 unk3             = fields[6].GetUInt32();
        uint32 unk4             = fields[7].GetUInt32();

        QuestPOI POI(poiId, objIndex, mapId, mapAreaId, floorId, unk3, unk4);

        mQuestPOIMap[questId].push_back(POI);

        ++count;
    }
    while (queryResult->NextRow());

    auto points = WorldDatabase.Query("SELECT questId, poiId, x, y FROM quest_poi_points");

    if (points)
    {
        do
        {
            Field* pointFields  = points->Fetch();

            uint32 questId      = pointFields[0].GetUInt32();
            uint32 poiId        = pointFields[1].GetUInt32();
            int32  x            = pointFields[2].GetInt32();
            int32  y            = pointFields[3].GetInt32();

            QuestPOIVector& vect = mQuestPOIMap[questId];

            for (auto& itr : vect)
            {
                if (itr.PoiId != poiId)
                    continue;

                QuestPOIPoint point(x, y);
                itr.points.push_back(point);
                break;
            }
        }
        while (points->NextRow());
    }

    sLog.outString(">> Loaded %u quest POI definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadNPCSpellClickSpells()
{
    uint32 count = 0;

    mSpellClickInfoMap.clear();
    m_spellsUsedInSpellClickConditions.clear();
    //                                             0          1         2            3                   4          5           6
    auto queryResult = WorldDatabase.Query("SELECT npc_entry, spell_id, quest_start, quest_start_active, quest_end, cast_flags, condition_id FROM npc_spellclick_spells");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 spellclick spells. DB table `npc_spellclick_spells` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        SpellClickInfo info;
        uint32 npc_entry         = fields[0].GetUInt32();
        info.spellId             = fields[1].GetUInt32();
        info.questStart          = fields[2].GetUInt32();
        info.questStartCanActive = fields[3].GetBool();
        info.questEnd            = fields[4].GetUInt32();
        info.castFlags           = fields[5].GetUInt8();
        info.conditionId         = fields[6].GetUInt16();

        CreatureInfo const* cInfo = GetCreatureTemplate(npc_entry);
        if (!cInfo)
        {
            sLog.outErrorDb("Table npc_spellclick_spells references unknown creature_template %u. Skipping entry.", npc_entry);
            continue;
        }

        // spell can be 0 for special or custom cases
        if (info.spellId)
        {
            SpellEntry const* spellinfo = sSpellTemplate.LookupEntry<SpellEntry>(info.spellId);
            if (!spellinfo)
            {
                sLog.outErrorDb("Table npc_spellclick_spells references unknown spellid %u. Skipping entry.", info.spellId);
                continue;
            }
        }

        if (info.conditionId)
        {
            ConditionEntry const* condition = sConditionStorage.LookupEntry<ConditionEntry>(info.conditionId);
            if (!condition)
            {
                sLog.outErrorDb("Table npc_spellclick_spells references unknown condition %u. Skipping entry.", info.conditionId);
                continue;
            }

            if (uint32 spellId = condition->UsesSpell())
                m_spellsUsedInSpellClickConditions.insert(spellId);
        }
        if (!info.conditionId)                         // TODO Drop block after finished converting
        {
            // quest might be 0 to enable spellclick independent of any quest
            if (info.questStart && mQuestTemplates.find(info.questStart) == mQuestTemplates.end())
            {
                sLog.outErrorDb("Table npc_spellclick_spells references unknown start quest %u. Skipping entry.", info.questStart);
                continue;
            }

            // quest might be 0 to enable spellclick active infinity after start quest
            if (info.questEnd && mQuestTemplates.find(info.questEnd) == mQuestTemplates.end())
            {
                sLog.outErrorDb("Table npc_spellclick_spells references unknown end quest %u. Skipping entry.", info.questEnd);
                continue;
            }
        }

        mSpellClickInfoMap.insert(SpellClickInfoMap::value_type(npc_entry, info));

        // mark creature template as spell clickable
        const_cast<CreatureInfo*>(cInfo)->NpcFlags |= UNIT_NPC_FLAG_SPELLCLICK;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u spellclick definitions", count);
    sLog.outString();
}

static char SERVER_SIDE_SPELL[]    = "CMaNGOS server-side spell";

struct SQLSpellLoader : public SQLStorageLoaderBase<SQLSpellLoader, SQLHashStorage>
{
    template<class S, class D>
    void default_fill(uint32 field_pos, S src, D& dst)
    {
        if (field_pos == LOADED_SPELLDBC_FIELD_POS_EQUIPPED_ITEM_CLASS)
            dst = D(-1);
        else
            dst = D(src);
    }

    void default_fill_to_str(uint32 field_pos, char const* /*src*/, char*& dst)
    {
        if (field_pos == LOADED_SPELLDBC_FIELD_POS_SPELLNAME_0)
        {
            dst = SERVER_SIDE_SPELL;
        }
        else
        {
            dst = new char[1];
            *dst = 0;
        }
    }
};

void ObjectMgr::LoadSpellTemplate()
{
    sLog.outString("Loading spell_template...");
    sSpellTemplate.Load();

    /* TODO add validation for spell_dbc */
    for (SQLStorageBase::SQLSIterator<SpellEntry> itr = sSpellTemplate.getDataBegin<SpellEntry>(); itr < sSpellTemplate.getDataEnd<SpellEntry>(); ++itr)
    {
        if (!sSpellTemplate.LookupEntry<SpellEntry>(itr->Id))
        {
            sLog.outErrorDb("LoadSpellDbc: implement validation to erase spell if it does not confirm to requirements for spells");
            sSpellTemplate.EraseEntry(itr->Id);
        }
    }

    for (uint32 i = 1; i < sSpellTemplate.GetMaxEntry(); ++i)
    {
        SpellEntry const* spell = sSpellTemplate.LookupEntry<SpellEntry>(i);
        if (!spell)
            continue;

        // DBC not support uint64 fields but SpellEntry have SpellFamilyFlags mapped at 2 uint32 fields
        // uint32 field already converted to bigendian if need, but must be swapped for correct uint64 bigendian view
#if MANGOS_ENDIAN == MANGOS_BIG_ENDIAN
        std::swap(*((uint32*)(&spell->SpellFamilyFlags)), *(((uint32*)(&spell->SpellFamilyFlags)) + 1));
#endif
    }

    sLog.outString(">> Loaded %u spell_dbc records", sSpellTemplate.GetRecordCount());
    sLog.outString();

    sSpellCones.Load();
    sLog.outString(">> Loaded %u spell_cone records", sSpellCones.GetRecordCount());
    sLog.outString();
}

void ObjectMgr::CheckSpellCones()
{
    for (uint32 i = 1; i < sSpellTemplate.GetMaxEntry(); ++i)
    {
        SpellEntry const* spell = sSpellTemplate.LookupEntry<SpellEntry>(i);
        SpellCone const* spellCone = sSpellCones.LookupEntry<SpellCone>(i);
        if (spell)
        {
            if (uint32 firstRankId = sSpellMgr.GetFirstSpellInChain(i))
            {
                SpellCone const* spellConeFirst = sSpellCones.LookupEntry<SpellCone>(firstRankId);
                if (!spellConeFirst && !spellCone) // no cones for either - is fine
                    continue;

                if (!spellCone && spellConeFirst) // cone for first - is fine
                    continue;

                if (!spellConeFirst && spellCone)
                    sLog.outErrorDb("Table spell_cone is missing entry for spell %u - angle %d for its first rank %u. But has cone for this one.", i, spellCone->coneAngle, firstRankId);
                else if (spellCone->coneAngle != spellConeFirst->coneAngle)
                    sLog.outErrorDb("Table spell_cone has different cone angle for spell Id %u - angle %d and first rank %u - angle %d", i, spellCone->coneAngle, firstRankId, spellConeFirst->coneAngle);
            }
        }
    }
}

void ObjectMgr::LoadDungeonEncounters()
{
    sLog.outString("Loading Dungeon Encounters...");
    sDungeonEncounterStore.Load();
}

void ObjectMgr::LoadAreaGroups()
{
    sLog.outString("Loading Area Groups");
    sAreaGroupStore.Load();
}

void ObjectMgr::LoadSQLDBCs()
{
    LoadSpellTemplate();
    LoadDungeonEncounters();
    LoadAreaGroups();
}

void ObjectMgr::LoadBroadcastText()
{
    uint32 count = 0;

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Id, Text, Text1, ChatTypeID, LanguageID, SoundEntriesID1, EmoteID1, EmoteID2, EmoteID3, EmoteDelay1, EmoteDelay2, EmoteDelay3 FROM broadcast_text"));

    if (!result)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 entries from %s. DB table `%s` is empty.", "broadcast_text", "broadcast_text");
        return;
    }


    BarGoLink bar(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();
        bar.step();

        uint32 id = fields[0].GetUInt32();

        BroadcastText& bct = m_broadcastTextMap[id];

        bct.maleText[DEFAULT_LOCALE] = fields[1].GetCppString();
        bct.femaleText[DEFAULT_LOCALE] = fields[2].GetCppString();
        bct.chatTypeId = ChatType(fields[3].GetUInt32());
        bct.languageId = Language(fields[4].GetUInt32());
        bct.soundId1 = fields[5].GetUInt32();
        bct.emoteIds[0] = fields[6].GetUInt32();
        bct.emoteIds[1] = fields[7].GetUInt32();
        bct.emoteIds[2] = fields[8].GetUInt32();
        bct.emoteDelays[0] = fields[9].GetUInt32();
        bct.emoteDelays[1] = fields[10].GetUInt32();
        bct.emoteDelays[2] = fields[11].GetUInt32();

        ++count;
    } while (result->NextRow());

    sLog.outString(">> Loaded %u texts from %s", count, "broadcast_text");
    sLog.outString();
}

void ObjectMgr::LoadBroadcastTextLocales()
{
    uint32 count = 0;

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT Id, Locale, Text_lang, Text1_lang FROM broadcast_text_locale"));

    if (!result)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString(">> Loaded 0 entries from broadcast_text_locales.");
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();
        bar.step();

        uint32 id = fields[0].GetUInt32();

        BroadcastText& bct = m_broadcastTextMap[id];

        std::string localeString = fields[1].GetCppString();
        std::string maleText = fields[2].GetCppString();
        std::string femaleText = fields[3].GetCppString();
        LocaleConstant locale = GetLocaleByName(localeString);

        int idx = GetOrNewStorageLocaleIndexFor(locale);
        if (idx >= 0)
        {
            ++idx;
            if (bct.maleText.size() <= size_t(idx))
            {
                bct.maleText.resize(idx + 1);
                bct.femaleText.resize(idx + 1);
            }

            bct.maleText[idx] = maleText;
            bct.femaleText[idx] = femaleText;
        }

        ++count;
    } while (result->NextRow());

    sLog.outString(">> Loaded %u texts from %s", count, "broadcast_text_locale");
    sLog.outString();
}

std::tuple<std::shared_ptr<std::map<int32, UnitConditionEntry>>, std::shared_ptr<std::map<int32, WorldStateExpressionEntry>>, std::shared_ptr<std::map<int32, CombatConditionEntry>>> ObjectMgr::LoadConditionsAndExpressions()
{
    auto unitConditions = m_unitConditionMgr->Load();
    auto worldstateExpressions = m_worldStateExpressionMgr->Load();
    auto combatConditions = m_combatConditionMgr->Load();
    return { unitConditions , worldstateExpressions, combatConditions };
}

std::shared_ptr<std::map<int32, UnitConditionEntry>> ObjectMgr::GetUnitConditions()
{
    return m_unitConditionMgr->Get();
}

std::shared_ptr<std::map<int32, WorldStateExpressionEntry>> ObjectMgr::GetWorldStateExpressions()
{
    return m_worldStateExpressionMgr->Get();
}

std::shared_ptr<std::map<int32, CombatConditionEntry>> ObjectMgr::GetCombatConditions()
{
    return m_combatConditionMgr->Get();
}

void ObjectMgr::DeleteCreatureData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    CreatureData const* data = GetCreatureData(guid);
    if (data)
        RemoveCreatureFromGrid(guid, data);

    mCreatureDataMap.erase(guid);
}

GameObjectTemplateAddon const* ObjectMgr::GetGOTemplateAddon(uint32 entry) const
{
    auto itr = m_gameobjectAddonTemplates.find(entry);
    if (itr == m_gameobjectAddonTemplates.end())
        return nullptr;

    return &itr->second;
}

void ObjectMgr::DeleteGOData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    GameObjectData const* data = GetGOData(guid);
    if (data)
        RemoveGameobjectFromGrid(guid, data);

    mGameObjectDataMap.erase(guid);
}

void ObjectMgr::AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses[player_guid] = instance;
}

void ObjectMgr::DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses.erase(player_guid);
}

void ObjectMgr::LoadQuestRelationsHelper(QuestRelationsMap& map, char const* table)
{
    map.clear();                                            // need for reload case

    uint32 count = 0;

    auto queryResult = WorldDatabase.PQuery("SELECT id,quest FROM %s", table);

    if (!queryResult)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 quest relations from %s. DB table `%s` is empty.", table, table);
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 id    = fields[0].GetUInt32();
        uint32 quest = fields[1].GetUInt32();

        if (mQuestTemplates.find(quest) == mQuestTemplates.end())
        {
            sLog.outErrorDb("Table `%s: Quest %u listed for entry %u does not exist.", table, quest, id);
            continue;
        }

        map.insert(QuestRelationsMap::value_type(id, quest));

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u quest relations from %s", count, table);
}

void ObjectMgr::LoadGameobjectQuestRelations()
{
    LoadQuestRelationsHelper(m_GOQuestRelations, "gameobject_questrelation");

    for (auto& m_GOQuestRelation : m_GOQuestRelations)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(m_GOQuestRelation.first);
        if (!goInfo)
            sLog.outErrorDb("Table `gameobject_questrelation` have data for nonexistent gameobject entry (%u) and existing quest %u", m_GOQuestRelation.first, m_GOQuestRelation.second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table `gameobject_questrelation` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", m_GOQuestRelation.first, m_GOQuestRelation.second);
    }
}

void ObjectMgr::LoadGameobjectInvolvedRelations()
{
    LoadQuestRelationsHelper(m_GOQuestInvolvedRelations, "gameobject_involvedrelation");

    for (auto& m_GOQuestInvolvedRelation : m_GOQuestInvolvedRelations)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(m_GOQuestInvolvedRelation.first);
        if (!goInfo)
            sLog.outErrorDb("Table `gameobject_involvedrelation` have data for nonexistent gameobject entry (%u) and existing quest %u", m_GOQuestInvolvedRelation.first, m_GOQuestInvolvedRelation.second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table `gameobject_involvedrelation` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", m_GOQuestInvolvedRelation.first, m_GOQuestInvolvedRelation.second);
    }
}

void ObjectMgr::LoadCreatureQuestRelations()
{
    LoadQuestRelationsHelper(m_CreatureQuestRelations, "creature_questrelation");

    for (auto& m_CreatureQuestRelation : m_CreatureQuestRelations)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(m_CreatureQuestRelation.first);
        if (!cInfo)
            sLog.outErrorDb("Table `creature_questrelation` have data for nonexistent creature entry (%u) and existing quest %u", m_CreatureQuestRelation.first, m_CreatureQuestRelation.second);
    }
}

void ObjectMgr::LoadCreatureInvolvedRelations()
{
    LoadQuestRelationsHelper(m_CreatureQuestInvolvedRelations, "creature_involvedrelation");

    for (auto& m_CreatureQuestInvolvedRelation : m_CreatureQuestInvolvedRelations)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(m_CreatureQuestInvolvedRelation.first);
        if (!cInfo)
            sLog.outErrorDb("Table `creature_involvedrelation` have data for nonexistent creature entry (%u) and existing quest %u", m_CreatureQuestInvolvedRelation.first, m_CreatureQuestInvolvedRelation.second);
    }
}

void ObjectMgr::LoadReservedPlayersNames()
{
    m_ReservedNames.clear();                                // need for reload case

    auto queryResult = WorldDatabase.Query("SELECT name FROM reserved_name");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u reserved player names", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();
        std::string name = fields[0].GetCppString();

        std::wstring wstr;
        if (!Utf8toWStr(name, wstr))
        {
            sLog.outError("Table `reserved_name` have invalid name: %s", name.c_str());
            continue;
        }

        wstrToLower(wstr);

        m_ReservedNames.insert(wstr);
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u reserved player names", count);
    sLog.outString();
}

bool ObjectMgr::IsReservedName(const std::string& name) const
{
    std::wstring wstr;
    if (!Utf8toWStr(name, wstr))
        return false;

    wstrToLower(wstr);

    return m_ReservedNames.find(wstr) != m_ReservedNames.end();
}

enum LanguageType
{
    LT_BASIC_LATIN    = 0x0000,
    LT_EXTENDEN_LATIN = 0x0001,
    LT_CYRILLIC       = 0x0002,
    LT_EAST_ASIA      = 0x0004,
    LT_ANY            = 0xFFFF
};

static LanguageType GetRealmLanguageType(bool create)
{
    switch (sWorld.getConfig(CONFIG_UINT32_REALM_ZONE))
    {
        case REALM_ZONE_UNKNOWN:                            // any language
        case REALM_ZONE_DEVELOPMENT:
        case REALM_ZONE_TEST_SERVER:
        case REALM_ZONE_QA_SERVER:
            return LT_ANY;
        case REALM_ZONE_UNITED_STATES:                      // extended-Latin
        case REALM_ZONE_OCEANIC:
        case REALM_ZONE_LATIN_AMERICA:
        case REALM_ZONE_ENGLISH:
        case REALM_ZONE_GERMAN:
        case REALM_ZONE_FRENCH:
        case REALM_ZONE_SPANISH:
            return LT_EXTENDEN_LATIN;
        case REALM_ZONE_KOREA:                              // East-Asian
        case REALM_ZONE_TAIWAN:
        case REALM_ZONE_CHINA:
            return LT_EAST_ASIA;
        case REALM_ZONE_RUSSIAN:                            // Cyrillic
            return LT_CYRILLIC;
        default:
            return create ? LT_BASIC_LATIN : LT_ANY;        // basic-Latin at create, any at login
    }
}

bool isValidString(const std::wstring& wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
    if (strictMask == 0)                                    // any language, ignore realm
    {
        if (isExtendedLatinString(wstr, numericOrSpace))
            return true;
        if (isCyrillicString(wstr, numericOrSpace))
            return true;
        if (isEastAsianString(wstr, numericOrSpace))
            return true;
        return false;
    }

    if (strictMask & 0x2)                                   // realm zone specific
    {
        LanguageType lt = GetRealmLanguageType(create);
        if (lt & LT_EXTENDEN_LATIN)
            if (isExtendedLatinString(wstr, numericOrSpace))
                return true;
        if (lt & LT_CYRILLIC)
            if (isCyrillicString(wstr, numericOrSpace))
                return true;
        if (lt & LT_EAST_ASIA)
            if (isEastAsianString(wstr, numericOrSpace))
                return true;
    }

    if (strictMask & 0x1)                                   // basic Latin
    {
        if (isBasicLatinString(wstr, numericOrSpace))
            return true;
    }

    return false;
}

bool ObjectMgr::CheckPublicMessageLanguage(const std::string& message)
{
    LanguageType lt = GetRealmLanguageType(false);

    if (lt == LT_ANY)
        return true;

    std::wstring wstr;

    if (!Utf8toWStr(message, wstr))
        return false;

    for (wchar_t c : wstr)
    {
        if (c <= 127)                               // Whitelisted in all locales (basic ASCII)
            continue;
        else
        {
            if (lt & LT_EXTENDEN_LATIN)             // Extended latin locales
            {
                if (!isExtendedLatinCharacter(c))
                    return false;
            }

            if (lt & LT_CYRILLIC)                   // Cyrillic locales
            {
                if (!isCyrillicCharacter(c))
                    return false;
            }

            if (lt & LT_EAST_ASIA)                  // East asian locales
            {
                if (!isEastAsianCharacter(c))
                    return false;
            }
        }
    }
    return true;
}

uint8 ObjectMgr::CheckPlayerName(const std::string& name, bool create)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return CHAR_NAME_INVALID_CHARACTER;

    if (wname.size() > MAX_PLAYER_NAME)
        return CHAR_NAME_TOO_LONG;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_PLAYER_NAME);
    if (wname.size() < minName)
        return CHAR_NAME_TOO_SHORT;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_PLAYER_NAMES);
    if (!isValidString(wname, strictMask, false, create))
        return CHAR_NAME_MIXED_LANGUAGES;

    return CHAR_NAME_SUCCESS;
}

bool ObjectMgr::IsValidCharterName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() > MAX_CHARTER_NAME)
        return false;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_CHARTER_NAME);
    if (wname.size() < minName)
        return false;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_CHARTER_NAMES);

    return isValidString(wname, strictMask, true);
}

PetNameInvalidReason ObjectMgr::CheckPetName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return PET_NAME_INVALID;

    if (wname.size() > MAX_PET_NAME)
        return PET_NAME_TOO_LONG;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_PET_NAME);
    if (wname.size() < minName)
        return PET_NAME_TOO_SHORT;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_PET_NAMES);
    if (!isValidString(wname, strictMask, false))
        return PET_NAME_MIXED_LANGUAGES;

    return PET_NAME_SUCCESS;
}

int ObjectMgr::GetStorageLocaleIndexFor(LocaleConstant loc)
{
    if (loc == DEFAULT_LOCALE)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    return -1;
}

int ObjectMgr::GetOrNewStorageLocaleIndexFor(LocaleConstant loc)
{
    if (loc == DEFAULT_LOCALE)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    m_LocalForIndex.push_back(loc);
    return m_LocalForIndex.size() - 1;
}

bool ObjectMgr::IsEncounter(uint32 creditEntry, uint32 mapId) const
{
    DungeonEncounterMapBounds bounds = GetDungeonEncounterBounds(creditEntry);

    for (auto entryItr = bounds.first; entryItr != bounds.second; ++entryItr)
    {
        auto dbcEntry = entryItr->second.dbcEntry;

        if (entryItr->second.creditType == ENCOUNTER_CREDIT_KILL_CREATURE && dbcEntry->mapId == mapId)
            return true;
    }
    return false;
}

void ObjectMgr::LoadGameObjectForQuests()
{
    mGameObjectForQuestSet.clear();                         // need for reload case

    if (!sGOStorage.GetMaxEntry())
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 GameObjects for quests");
        sLog.outString();
        return;
    }

    BarGoLink bar(sGOStorage.GetRecordCount());
    uint32 count = 0;

    // collect GO entries for GO that must activated
    for (SQLStorageBase::SQLSIterator<GameObjectInfo> itr = sGOStorage.getDataBegin<GameObjectInfo>(); itr < sGOStorage.getDataEnd<GameObjectInfo>(); ++itr)
    {
        bar.step();
        switch (itr->type)
        {
            case GAMEOBJECT_TYPE_QUESTGIVER:
            {
                if (m_GOQuestRelations.find(itr->id) != m_GOQuestRelations.end() ||
                        m_GOQuestInvolvedRelations.find(itr->id) != m_GOQuestInvolvedRelations.end())
                {
                    mGameObjectForQuestSet.insert(itr->id);
                    ++count;
                }

                break;
            }
            case GAMEOBJECT_TYPE_CHEST:
            {
                // scan GO chest with loot including quest items
                uint32 loot_id = itr->GetLootId();

                // always activate to quest, GO may not have loot, OR find if GO has loot for quest.
                if (itr->chest.questId || LootTemplates_Gameobject.HaveQuestLootFor(loot_id))
                {
                    mGameObjectForQuestSet.insert(itr->id);
                    ++count;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GENERIC:
            {
                if (itr->_generic.questID)                  // quest related objects, has visual effects
                {
                    mGameObjectForQuestSet.insert(itr->id);
                    ++count;
                }
                break;
            }
            case GAMEOBJECT_TYPE_SPELL_FOCUS:
            {
                if (itr->spellFocus.questID)                // quest related objects, has visual effect
                {
                    mGameObjectForQuestSet.insert(itr->id);
                    ++count;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:
            {
                if (itr->goober.questId != 0)               // quests objects
                {
                    mGameObjectForQuestSet.insert(itr->id);
                    ++count;
                }
                break;
            }
            default:
                break;
        }
    }

    sLog.outString(">> Loaded %u GameObjects for quests", count);
    sLog.outString();
}

inline void _DoStringError(char const* text, ...)
{
    MANGOS_ASSERT(text);

    char buf[256];
    va_list ap;
    va_start(ap, text);
    vsnprintf(buf, 256, text, ap);
    va_end(ap);

    sLog.outError("%s", buf);
}

bool ObjectMgr::LoadMangosStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value, bool extra_content)
{
    int32 start_value = min_value;
    int32 end_value   = max_value;
    // some string can have negative indexes range
    if (start_value < 0)
    {
        if (end_value >= start_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }

        // real range (max+1,min+1) exaple: (-10,-1000) -> -999...-10+1
        std::swap(start_value, end_value);
        ++start_value;
        ++end_value;
    }
    else
    {
        if (start_value >= end_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }
    }

    // cleanup affected map part for reloading case
    for (MangosStringLocaleMap::iterator itr = mMangosStringLocaleMap.begin(); itr != mMangosStringLocaleMap.end();)
    {
        if (itr->first >= start_value && itr->first < end_value)
            mMangosStringLocaleMap.erase(itr++);
        else
            ++itr;
    }

    sLog.outString("Loading texts from %s%s", table, extra_content ? ", with additional data" : "");

    auto queryResult = db.PQuery("SELECT entry,content_default,content_loc1,content_loc2,content_loc3,content_loc4,content_loc5,content_loc6,content_loc7,content_loc8 %s FROM %s",
                                    extra_content ? ",sound,type,language,emote,broadcast_text_id" : "", table);

    if (!queryResult)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        if (min_value == MIN_MANGOS_STRING_ID)              // error only in case internal strings
            sLog.outErrorDb(">> Loaded 0 mangos strings. DB table `%s` is empty. Cannot continue.", table);
        else
            sLog.outString(">> Loaded 0 string templates. DB table `%s` is empty.", table);
        return false;
    }

    uint32 count = 0;

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        int32 entry = fields[0].GetInt32();

        if (entry == 0)
        {
            _DoStringError("Table `%s` contain reserved entry 0, ignored.", table);
            continue;
        }
        if (entry < start_value || entry >= end_value)
        {
            _DoStringError("Table `%s` contain entry %i out of allowed range (%d - %d), ignored.", table, entry, min_value, max_value);
            continue;
        }

        MangosStringLocale& data = mMangosStringLocaleMap[entry];

        if (!data.Content.empty())
        {
            _DoStringError("Table `%s` contain data for already loaded entry  %i (from another table?), ignored.", table, entry);
            continue;
        }

        data.Content.resize(1);
        ++count;

        // 0 -> default, idx in to idx+1
        data.Content[0] = fields[1].GetCppString();

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewStorageLocaleIndexFor(LocaleConstant(i));
                if (idx >= 0)
                {
                    // 0 -> default, idx in to idx+1
                    if ((int32)data.Content.size() <= idx + 1)
                        data.Content.resize(idx + 2);

                    data.Content[idx + 1] = str;
                }
            }
        }

        // Load additional string content if necessary
        if (extra_content)
        {
            data.SoundId     = fields[10].GetUInt32();
            data.Type        = fields[11].GetUInt32();
            data.LanguageId  = Language(fields[12].GetUInt32());
            data.Emote       = fields[13].GetUInt32();
            uint32 broadcastTextId = fields[14].GetUInt32();

            if (data.SoundId && !sSoundEntriesStore.LookupEntry(data.SoundId))
            {
                _DoStringError("Entry %i in table `%s` has soundId %u but sound does not exist.", entry, table, data.SoundId);
                data.SoundId = 0;
            }

            if (!GetLanguageDescByID(data.LanguageId))
            {
                _DoStringError("Entry %i in table `%s` using Language %u but Language does not exist.", entry, table, uint32(data.LanguageId));
                data.LanguageId = LANG_UNIVERSAL;
            }

            if (data.Type >= CHAT_TYPE_MAX)
            {
                _DoStringError("Entry %i in table `%s` has Type %u but this Chat Type does not exist.", entry, table, data.Type);
                data.Type = CHAT_TYPE_SAY;
            }

            if (data.Emote && !sEmotesStore.LookupEntry(data.Emote))
            {
                _DoStringError("Entry %i in table `%s` has Emote %u but emote does not exist.", entry, table, data.Emote);
                data.Emote = EMOTE_ONESHOT_NONE;
            }

            if (broadcastTextId)
            {
                if (BroadcastText const* bct = GetBroadcastText(broadcastTextId))
                    data.broadcastText = bct;
                else
                    _DoStringError("Entry %i in table `%s` has BroadcastTextID %u but broadcast_text does not exist.", entry, table, broadcastTextId);
            }
        }
    }
    while (queryResult->NextRow());

    if (min_value == MIN_MANGOS_STRING_ID)
        sLog.outString(">> Loaded %u MaNGOS strings from table %s", count, table);
    else
        sLog.outString(">> Loaded %u %s templates from %s", count, extra_content ? "text" : "string", table);
    sLog.outString();

    m_loadedStringCount[min_value] = count;

    return true;
}

const char* ObjectMgr::GetMangosString(int32 entry, int locale_idx) const
{
    // locale_idx==-1 -> default, locale_idx >= 0 in to idx+1
    // Content[0] always exist if exist MangosStringLocale
    if (MangosStringLocale const* msl = GetMangosStringLocale(entry))
    {
        if ((int32)msl->Content.size() > locale_idx + 1 && !msl->Content[locale_idx + 1].empty())
            return msl->Content[locale_idx + 1].c_str();
        return msl->Content[0].c_str();
    }

    _DoStringError("Entry %i not found but requested", entry);

    return "<error>";
}

void ObjectMgr::LoadFishingBaseSkillLevel()
{
    mFishingBaseForArea.clear();                            // for reload case

    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT entry,skill FROM skill_fishing_base_level");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded `skill_fishing_base_level`, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();
        uint32 entry  = fields[0].GetUInt32();
        int32 skill   = fields[1].GetInt32();

        AreaTableEntry const* fArea = GetAreaEntryByAreaID(entry);
        if (!fArea)
        {
            sLog.outErrorDb("AreaId %u defined in `skill_fishing_base_level` does not exist", entry);
            continue;
        }

        mFishingBaseForArea[entry] = skill;
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u areas for fishing base skill level", count);
    sLog.outString();
}

// Check if a target meets condition conditionId
bool ObjectMgr::IsConditionSatisfied(uint32 conditionId, WorldObject const* target, Map const* map, WorldObject const* source, ConditionSource conditionSourceType) const
{
    if (ConditionEntry const* condition = sConditionStorage.LookupEntry<ConditionEntry>(conditionId))
        return condition->Meets(target, map, source, conditionSourceType);

    return false;
}

bool ObjectMgr::IsWorldStateExpressionSatisfied(int32 expressionId, Map const* map)
{
    return m_worldStateExpressionMgr->Meets(map, expressionId);
}

bool ObjectMgr::IsUnitConditionSatisfied(int32 conditionId, Unit const* source, Unit const* target)
{
    return m_unitConditionMgr->Meets(source, target, conditionId);
}

bool ObjectMgr::IsCombatConditionSatisfied(int32 conditionId, Unit const* source, float range)
{
    return m_combatConditionMgr->Meets(source, conditionId, range);
}

bool ObjectMgr::CheckDeclinedNames(const std::wstring& mainpart, DeclinedName const& names)
{
    for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        std::wstring wname;
        if (!Utf8toWStr(names.name[i], wname))
            return false;

        if (mainpart != GetMainPartOfName(wname, i + 1))
            return false;
    }
    return true;
}

SkillRangeType GetSkillRangeType(SkillLineEntry const* pSkill, bool racial)
{
    switch (pSkill->categoryId)
    {
        case SKILL_CATEGORY_LANGUAGES: return SKILL_RANGE_LANGUAGE;
        case SKILL_CATEGORY_WEAPON:
        {
            if (pSkill->id != SKILL_FIST_WEAPONS)
                return SKILL_RANGE_LEVEL;
            return SKILL_RANGE_MONO;
        }
        case SKILL_CATEGORY_ARMOR:
        case SKILL_CATEGORY_CLASS:
        {
            if (pSkill->id != SKILL_LOCKPICKING)
                return SKILL_RANGE_MONO;
            return SKILL_RANGE_LEVEL;
        }
        case SKILL_CATEGORY_SECONDARY:
        case SKILL_CATEGORY_PROFESSION:
            // not set skills for professions and racial abilities
        {
            // not set skills for professions and racial abilities
            if (IsProfessionOrRidingSkill(pSkill->id))
                return SKILL_RANGE_RANK;
            if (racial)
                return SKILL_RANGE_NONE;
            return SKILL_RANGE_MONO;
        }
        default:
        case SKILL_CATEGORY_ATTRIBUTES:                     // not found in dbc
        case SKILL_CATEGORY_GENERIC:                        // only GENERIC(DND)
            return SKILL_RANGE_NONE;
    }
}

void ObjectMgr::LoadGameTele()
{
    m_GameTeleMap.clear();                                  // for reload case

    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT id, position_x, position_y, position_z, orientation, map, name FROM game_tele");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded `game_tele`, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 id         = fields[0].GetUInt32();

        GameTele gt;

        gt.position_x     = fields[1].GetFloat();
        gt.position_y     = fields[2].GetFloat();
        gt.position_z     = fields[3].GetFloat();
        gt.orientation    = fields[4].GetFloat();
        gt.mapId          = fields[5].GetUInt32();
        gt.name           = fields[6].GetCppString();

        if (!MapManager::IsValidMapCoord(gt.mapId, gt.position_x, gt.position_y, gt.position_z, gt.orientation))
        {
            sLog.outErrorDb("Wrong position for id %u (name: %s) in `game_tele` table, ignoring.", id, gt.name.c_str());
            continue;
        }

        if (!Utf8toWStr(gt.name, gt.wnameLow))
        {
            sLog.outErrorDb("Wrong UTF8 name for id %u in `game_tele` table, ignoring.", id);
            continue;
        }

        wstrToLower(gt.wnameLow);

        m_GameTeleMap[id] = gt;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u GameTeleports", count);
    sLog.outString();
}

GameTele const* ObjectMgr::GetGameTele(const std::string& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    // Alternative first GameTele what contains wnameLow as substring in case no GameTele location found
    const GameTele* alt = nullptr;
    for (const auto& itr : m_GameTeleMap)
    {
        if (itr.second.wnameLow == wname)
            return &itr.second;
        if (alt == nullptr && itr.second.wnameLow.find(wname) != std::wstring::npos)
            alt = &itr.second;
    }

    return alt;
}

bool ObjectMgr::AddGameTele(GameTele& tele)
{
    // find max id
    uint32 new_id = 0;
    for (GameTeleMap::const_iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
        if (itr->first > new_id)
            new_id = itr->first;

    // use next
    ++new_id;

    if (!Utf8toWStr(tele.name, tele.wnameLow))
        return false;

    wstrToLower(tele.wnameLow);

    m_GameTeleMap[new_id] = tele;
    std::string safeName(tele.name);
    WorldDatabase.escape_string(safeName);

    return WorldDatabase.PExecuteLog("INSERT INTO game_tele "
                                     "(id,position_x,position_y,position_z,orientation,map,name) "
                                     "VALUES (%u,%f,%f,%f,%f,%u,'%s')",
                                     new_id, tele.position_x, tele.position_y, tele.position_z,
                                     tele.orientation, tele.mapId, safeName.c_str());
}

bool ObjectMgr::DeleteGameTele(const std::string& name)
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleMap::iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
        {
            WorldDatabase.PExecuteLog("DELETE FROM game_tele WHERE name = '%s'", itr->second.name.c_str());
            m_GameTeleMap.erase(itr);
            return true;
        }
    }

    return false;
}

void ObjectMgr::LoadMailLevelRewards()
{
    m_mailLevelRewardMap.clear();                           // for reload case

    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT level, raceMask, mailTemplateId, senderEntry FROM mail_level_reward");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded `mail_level_reward`, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint8 level           = fields[0].GetUInt8();
        uint32 raceMask       = fields[1].GetUInt32();
        uint32 mailTemplateId = fields[2].GetUInt32();
        uint32 senderEntry    = fields[3].GetUInt32();

        if (level > MAX_LEVEL)
        {
            sLog.outErrorDb("Table `mail_level_reward` have data for level %u that more supported by client (%u), ignoring.", level, MAX_LEVEL);
            continue;
        }

        if (!(raceMask & RACEMASK_ALL_PLAYABLE))
        {
            sLog.outErrorDb("Table `mail_level_reward` have raceMask (%u) for level %u that not include any player races, ignoring.", raceMask, level);
            continue;
        }

        if (!sMailTemplateStore.LookupEntry(mailTemplateId))
        {
            sLog.outErrorDb("Table `mail_level_reward` have invalid mailTemplateId (%u) for level %u that invalid not include any player races, ignoring.", mailTemplateId, level);
            continue;
        }

        if (!GetCreatureTemplate(senderEntry))
        {
            sLog.outErrorDb("Table `mail_level_reward` have nonexistent sender creature entry (%u) for level %u that invalid not include any player races, ignoring.", senderEntry, level);
            continue;
        }

        m_mailLevelRewardMap[level].push_back(MailLevelReward(raceMask, mailTemplateId, senderEntry));

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u level dependent mail rewards,", count);
    sLog.outString();
}

void ObjectMgr::LoadTrainers(char const* tableName, bool isTemplates)
{
    CacheTrainerSpellMap& trainerList = isTemplates ? m_mCacheTrainerTemplateSpellMap : m_mCacheTrainerSpellMap;

    // For reload case
    for (auto& itr : trainerList)
        itr.second.Clear();
    trainerList.clear();

    std::set<uint32> skip_trainers;

    auto queryResult = WorldDatabase.PQuery("SELECT entry, spell,spellcost,reqskill,reqskillvalue,reqlevel,ReqAbility1,ReqAbility2,ReqAbility3,condition_id FROM %s", tableName);

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded `%s`, table is empty!", tableName);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    std::set<uint32> talentIds;

    uint32 count = 0;
    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 entry  = fields[0].GetUInt32();
        uint32 spell  = fields[1].GetUInt32();

        SpellEntry const* spellinfo = sSpellTemplate.LookupEntry<SpellEntry>(spell);
        if (!spellinfo)
        {
            sLog.outErrorDb("Table `%s` (Entry: %u ) has non existing spell %u, ignore", tableName, entry, spell);
            continue;
        }

        if (!SpellMgr::IsSpellValid(spellinfo))
        {
            sLog.outErrorDb("Table `%s` (Entry: %u) has broken learning spell %u, ignore", tableName, entry, spell);
            continue;
        }

        if (GetTalentSpellCost(spell))
        {
            if (talentIds.find(spell) == talentIds.end())
            {
                sLog.outErrorDb("Table `%s` has talent as learning spell %u, ignore", tableName, spell);
                talentIds.insert(spell);
            }
            continue;
        }

        if (!isTemplates)
        {
            CreatureInfo const* cInfo = GetCreatureTemplate(entry);

            if (!cInfo)
            {
                sLog.outErrorDb("Table `%s` have entry for nonexistent creature template (Entry: %u), ignore", tableName, entry);
                continue;
            }

            if (!(cInfo->NpcFlags & UNIT_NPC_FLAG_TRAINER))
            {
                if (skip_trainers.find(entry) == skip_trainers.end())
                {
                    sLog.outErrorDb("Table `%s` have data for creature (Entry: %u) without trainer flag, ignore", tableName, entry);
                    skip_trainers.insert(entry);
                }
                continue;
            }

            if (TrainerSpellData const* tSpells = cInfo->TrainerTemplateId ? GetNpcTrainerTemplateSpells(cInfo->TrainerTemplateId) : nullptr)
            {
                if (tSpells->spellList.find(spell) != tSpells->spellList.end())
                {
                    sLog.outErrorDb("Table `%s` (Entry: %u) has spell %u listed in trainer template %u, ignore", tableName, entry, spell, cInfo->TrainerTemplateId);
                    continue;
                }
            }
        }

        TrainerSpellData& data = trainerList[entry];

        TrainerSpell& trainerSpell = data.spellList[spell];
        trainerSpell.spell         = spell;
        trainerSpell.spellCost     = fields[2].GetUInt32();
        trainerSpell.reqSkill      = fields[3].GetUInt32();
        trainerSpell.reqSkillValue = fields[4].GetUInt32();
        trainerSpell.reqLevel      = fields[5].GetUInt32();
        if (!fields[6].IsNULL())
            trainerSpell.reqAbility[0] = fields[6].GetUInt32();
        if (!fields[7].IsNULL())
            trainerSpell.reqAbility[1] = fields[7].GetUInt32();
        if (!fields[8].IsNULL())
            trainerSpell.reqAbility[2] = fields[8].GetUInt32();
        trainerSpell.conditionId   = fields[9].GetUInt16();

        trainerSpell.isProvidedReqLevel = trainerSpell.reqLevel > 0;

        // By default, lets assume the specified spell is the one we want to teach the player...
        trainerSpell.learnedSpell = spell;
        // ...but first, lets inspect this spell...
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (spellinfo->Effect[i] == SPELL_EFFECT_LEARN_SPELL && spellinfo->EffectTriggerSpell[i])
            {
                switch (spellinfo->EffectImplicitTargetA[i])
                {
                    case TARGET_NONE:
                    case TARGET_UNIT_CASTER:
                        // ...looks like the specified spell is actually a trainer's spell casted on a player to teach another spell
                        // Trainer's spells can teach more than one spell (up to number of effects), but we will stick to the first one
                        // Self-casts listed in trainer's lists usually come from recipes which were made trainable in a later patch
                        trainerSpell.learnedSpell = spellinfo->EffectTriggerSpell[i];
                        break;
                }
            }
        }

        // already checked as valid spell so exist.
        SpellEntry const* learnSpellinfo = sSpellTemplate.LookupEntry<SpellEntry>(trainerSpell.learnedSpell);
        if (SpellMgr::IsProfessionSpell(trainerSpell.learnedSpell))
        {
            data.trainerType = 2;

            uint32 minLevel = sSpellMgr.GetProfessionSpellMinLevel(trainerSpell.learnedSpell);
            if (trainerSpell.reqLevel)
            {
                if (minLevel == trainerSpell.reqLevel)
                    ERROR_DB_STRICT_LOG("Table `%s` (Entry: %u) has redundant reqlevel %u (=prof reqlevel) for spell %u", tableName, entry, trainerSpell.reqLevel, spell);
                else
                    sLog.outErrorDb("Table `%s` (Entry: %u) has wrong redundant reqlevel %u (<>prof reqlevel %u) for spell %u", tableName, entry, trainerSpell.reqLevel, minLevel, spell);
            }
            else
                trainerSpell.reqLevel = minLevel;
        }
        // for non-prof. spell use spellLevel if not provided any
        else
        {
            if (trainerSpell.reqLevel)
            {
                if (trainerSpell.reqLevel == learnSpellinfo->spellLevel)
                    ERROR_DB_STRICT_LOG("Table `%s` (Entry: %u) has redundant reqlevel %u (=spell level) for spell %u", tableName, entry, trainerSpell.reqLevel, spell);
            }
            else
                trainerSpell.reqLevel = learnSpellinfo->spellLevel;
        }

        if (trainerSpell.conditionId)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(trainerSpell.conditionId);
            if (!condition) // condition does not exist for some reason
                sLog.outErrorDb("Table `%s` (Entry: %u) has `condition_id` = %u but does not exist.", tableName, entry, trainerSpell.conditionId);
        }

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %d trainer %sspells", count, isTemplates ? "template " : "");
    sLog.outString();
}

void ObjectMgr::LoadTrainerTemplates()
{
    LoadTrainers("npc_trainer_template", true);

    // post loading check
    std::set<uint32> trainer_ids;
    bool hasErrored = false;

    for (CacheTrainerSpellMap::const_iterator tItr = m_mCacheTrainerTemplateSpellMap.begin(); tItr != m_mCacheTrainerTemplateSpellMap.end(); ++tItr)
        trainer_ids.insert(tItr->first);

    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
        {
            if (cInfo->TrainerTemplateId)
            {
                if (m_mCacheTrainerTemplateSpellMap.find(cInfo->TrainerTemplateId) != m_mCacheTrainerTemplateSpellMap.end())
                    trainer_ids.erase(cInfo->TrainerTemplateId);
                else
                {
                    sLog.outErrorDb("Creature (Entry: %u) has TrainerTemplateId = %u for nonexistent trainer template", cInfo->Entry, cInfo->TrainerTemplateId);
                    hasErrored = true;
                }
            }
        }
    }

    for (uint32 trainer_id : trainer_ids)
    sLog.outErrorDb("Table `npc_trainer_template` has trainer template %u not used by any trainers ", trainer_id);

    if (hasErrored || !trainer_ids.empty())                 // Append extra line in case of reported errors
        sLog.outString();
}

void ObjectMgr::LoadVendors(char const* tableName, bool isTemplates)
{
    CacheVendorItemMap& vendorList = isTemplates ? m_mCacheVendorTemplateItemMap : m_mCacheVendorItemMap;

    // For reload case
    for (auto& itr : vendorList)
        itr.second.Clear();
    vendorList.clear();

    auto queryResult = WorldDatabase.PQuery("SELECT entry, item, maxcount, incrtime, ExtendedCost, condition_id FROM %s ORDER BY slot", tableName);
    if (!queryResult)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString(">> Loaded `%s`, table is empty!", tableName);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;
    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();

        uint32 entry        = fields[0].GetUInt32();
        uint32 item_id      = fields[1].GetUInt32();
        uint32 maxcount     = fields[2].GetUInt32();
        uint32 incrtime     = fields[3].GetUInt32();
        uint32 ExtendedCost = fields[4].GetUInt32();
        uint16 conditionId  = fields[5].GetUInt16();

        if (!IsVendorItemValid(isTemplates, tableName, entry, item_id, maxcount, incrtime, ExtendedCost, conditionId, nullptr))
            continue;

        VendorItemData& vList = vendorList[entry];

        vList.AddItem(item_id, maxcount, incrtime, ExtendedCost, conditionId);
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u vendor %sitems", count, isTemplates ? "template " : "");
    sLog.outString();
}

void ObjectMgr::LoadVendorTemplates()
{
    LoadVendors("npc_vendor_template", true);

    // post loading check
    std::set<uint32> vendor_ids;

    for (CacheVendorItemMap::const_iterator vItr = m_mCacheVendorTemplateItemMap.begin(); vItr != m_mCacheVendorTemplateItemMap.end(); ++vItr)
        vendor_ids.insert(vItr->first);

    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
        {
            if (cInfo->VendorTemplateId)
            {
                if (m_mCacheVendorTemplateItemMap.find(cInfo->VendorTemplateId) !=  m_mCacheVendorTemplateItemMap.end())
                    vendor_ids.erase(cInfo->VendorTemplateId);
                else
                    sLog.outErrorDb("Creature (Entry: %u) has VendorTemplateId = %u for nonexistent vendor template", cInfo->Entry, cInfo->VendorTemplateId);
            }
        }
    }

    for (uint32 vendor_id : vendor_ids)
    sLog.outErrorDb("Table `npc_vendor_template` has vendor template %u not used by any vendors ", vendor_id);
}

/* This function is supposed to take care of three things:
 *  1) Load Transports on Map or on Continents
 *  2) Load Active Npcs on Map or Continents
 *  3) Load Everything dependend on config setting LoadAllGridsOnMaps
 *
 *  This function is currently WIP, hence parts exist only as draft.
 */
void ObjectMgr::LoadActiveEntities(Map* _map)
{
    // Load active objects for _map
    if (sWorld.isForceLoadMap(_map->GetId()))
    {
        for (CreatureDataMap::const_iterator itr = mCreatureDataMap.begin(); itr != mCreatureDataMap.end(); ++itr)
        {
            if (itr->second.mapid == _map->GetId())
                _map->ForceLoadGrid(itr->second.posX, itr->second.posY);
        }
    }
    else                                                    // Normal case - Load all npcs that are active
    {
        auto bounds = m_activeCreatures.equal_range(_map->GetId());
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            CreatureData const& data = mCreatureDataMap[itr->second];
            _map->ForceLoadGrid(data.posX, data.posY);
        }

        bounds = m_activeGameObjects.equal_range(_map->GetId());
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            GameObjectData const& data = mGameObjectDataMap[itr->second];
            _map->ForceLoadGrid(data.posX, data.posY);
        }
    }

    // Load Transports on Map _map
}

void ObjectMgr::LoadLargeEntities(Map* _map)
{
    if (sWorld.isForceLoadMap(_map->GetId())) // handled by active
        return;

    auto bounds = m_largeCreatures.equal_range(_map->GetId());
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        CreatureData const& data = mCreatureDataMap[itr->second];
        _map->ForceLoadGrid(data.posX, data.posY);
    }

    bounds = m_largeGameObjects.equal_range(_map->GetId());
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        GameObjectData const& data = mGameObjectDataMap[itr->second];
        _map->ForceLoadGrid(data.posX, data.posY);
    }
}

void ObjectMgr::LoadNpcGossips()
{
    m_mCacheNpcTextIdMap.clear();

    auto queryResult = WorldDatabase.Query("SELECT npc_guid, textid FROM npc_gossip");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded `npc_gossip`, table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;
    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 guid = fields[0].GetUInt32();
        uint32 textid = fields[1].GetUInt32();

        if (!GetCreatureData(guid))
        {
            sLog.outErrorDb("Table `npc_gossip` have nonexistent creature (GUID: %u) entry, ignore. ", guid);
            continue;
        }
        if (!GetGossipText(textid))
        {
            sLog.outErrorDb("Table `npc_gossip` for creature (GUID: %u) have wrong Textid (%u), ignore. ", guid, textid);
            continue;
        }

        m_mCacheNpcTextIdMap[guid] = textid ;
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %d NpcTextId", count);
    sLog.outString();
}

void ObjectMgr::LoadGossipMenu(std::set<uint32>& gossipScriptSet)
{
    m_mGossipMenusMap.clear();
    //                                                 0        1          2             3
    auto queryResult = WorldDatabase.Query("SELECT entry, text_id, script_id, condition_id FROM gossip_menu");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded gossip_menu, table is empty!");
        sLog.outString();
        return;
    }

    auto gossipScripts = sScriptMgr.GetScriptMap(SCRIPT_TYPE_GOSSIP);

    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        GossipMenus gMenu;

        gMenu.entry             = fields[0].GetUInt32();
        gMenu.text_id           = fields[1].GetUInt32();
        gMenu.script_id         = fields[2].GetUInt32();

        gMenu.conditionId       = fields[3].GetUInt16();

        if (!GetGossipText(gMenu.text_id))
        {
            sLog.outErrorDb("Table gossip_menu entry %u are using non-existing text_id %u", gMenu.entry, gMenu.text_id);
            continue;
        }

        // Check script-id
        if (gMenu.script_id)
        {
            if (gossipScripts->second.find(gMenu.script_id) == gossipScripts->second.end())
            {
                sLog.outErrorDb("Table gossip_menu for menu %u, text-id %u have script_id %u that does not exist in `dbscripts_on_gossip`, ignoring", gMenu.entry, gMenu.text_id, gMenu.script_id);
                continue;
            }

            // Remove used script id
            gossipScriptSet.erase(gMenu.script_id);
        }

        if (gMenu.conditionId)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(gMenu.conditionId);
            if (!condition)
            {
                sLog.outErrorDb("Table gossip_menu for menu %u, text-id %u has condition_id %u that does not exist in `conditions`, ignoring", gMenu.entry, gMenu.text_id, gMenu.conditionId);
                gMenu.conditionId = 0;
            }
        }

        m_mGossipMenusMap.insert(GossipMenusMap::value_type(gMenu.entry, gMenu));

        ++count;
    }
    while (queryResult->NextRow());

    // post loading tests
    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
            if (cInfo->GossipMenuId)
                if (m_mGossipMenusMap.find(cInfo->GossipMenuId) == m_mGossipMenusMap.end())
                    sLog.outErrorDb("Creature (Entry: %u) has GossipMenuId = %u for nonexistent menu", cInfo->Entry, cInfo->GossipMenuId);
    }

    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
    {
        for (SQLStorageBase::SQLSIterator<GameObjectInfo> itr = sGOStorage.getDataBegin<GameObjectInfo>(); itr < sGOStorage.getDataEnd<GameObjectInfo>(); ++itr)
            if (uint32 menuid = itr->GetGossipMenuId())
                if (m_mGossipMenusMap.find(menuid) == m_mGossipMenusMap.end())
                    sLog.outErrorDb("Gameobject (Entry: %u) has gossip_menu_id = %u for nonexistent menu", itr->id, menuid);
    }

    sLog.outString(">> Loaded %u gossip_menu entries", count);
    sLog.outString();
}

void ObjectMgr::LoadGossipMenuItems(std::set<uint32>& gossipScriptSet)
{
    m_mGossipMenuItemsMap.clear();

    auto queryResult = WorldDatabase.Query(
                              "SELECT menu_id, id, option_icon, option_text, option_broadcast_text, option_id, npc_option_npcflag, "
                              "action_menu_id, action_poi_id, action_script_id, box_coded, box_money, box_text, box_broadcast_text, "
                              "condition_id "
                              "FROM gossip_menu_option ORDER BY menu_id, id");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded gossip_menu_option, table is empty!");
        sLog.outString();
        return;
    }

    // prepare data for unused menu ids
    std::set<uint32> menu_ids;                              // for later integrity check
    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))   // check unused menu ids only in strict mode
    {
        for (GossipMenusMap::const_iterator itr = m_mGossipMenusMap.begin(); itr != m_mGossipMenusMap.end(); ++itr)
            if (itr->first)
                menu_ids.insert(itr->first);

        for (SQLStorageBase::SQLSIterator<GameObjectInfo> itr = sGOStorage.getDataBegin<GameObjectInfo>(); itr < sGOStorage.getDataEnd<GameObjectInfo>(); ++itr)
            if (uint32 menuid = itr->GetGossipMenuId())
                menu_ids.erase(menuid);
    }

    // loading
    BarGoLink bar(queryResult->GetRowCount());

    uint32 count = 0;

    // prepare menuid -> CreatureInfo map for fast access
    typedef  std::multimap<uint32, const CreatureInfo*> Menu2CInfoMap;
    Menu2CInfoMap menu2CInfoMap;
    for (uint32 i = 1;  i < sCreatureStorage.GetMaxEntry(); ++i)
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
            if (cInfo->GossipMenuId)
            {
                menu2CInfoMap.insert(Menu2CInfoMap::value_type(cInfo->GossipMenuId, cInfo));

                // unused check data preparing part
                if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
                    menu_ids.erase(cInfo->GossipMenuId);
            }

    auto gossipScripts = sScriptMgr.GetScriptMap(SCRIPT_TYPE_GOSSIP);

    do
    {
        bar.step();

        Field* fields = queryResult->Fetch();

        GossipMenuItems gMenuItem;

        gMenuItem.menu_id               = fields[0].GetUInt32();
        gMenuItem.id                    = fields[1].GetUInt32();
        gMenuItem.option_icon           = fields[2].GetUInt8();
        gMenuItem.option_text           = fields[3].GetCppString();
        gMenuItem.option_broadcast_text = fields[4].GetUInt32();
        gMenuItem.option_id             = fields[5].GetUInt32();
        gMenuItem.npc_option_npcflag    = fields[6].GetUInt32();
        gMenuItem.action_menu_id        = fields[7].GetInt32();
        gMenuItem.action_poi_id         = fields[8].GetUInt32();
        gMenuItem.action_script_id      = fields[9].GetUInt32();
        gMenuItem.box_coded             = fields[10].GetUInt8() != 0;
        gMenuItem.box_money             = fields[11].GetUInt32();
        gMenuItem.box_text              = fields[12].GetCppString();
        gMenuItem.box_broadcast_text    = fields[13].GetUInt32();
        gMenuItem.conditionId           = fields[14].GetUInt16();

        if (gMenuItem.menu_id)                              // == 0 id is special and not have menu_id data
        {
            if (m_mGossipMenusMap.find(gMenuItem.menu_id) == m_mGossipMenusMap.end())
            {
                sLog.outErrorDb("Gossip menu option (MenuId: %u) for nonexistent menu", gMenuItem.menu_id);
                continue;
            }
        }

        if (gMenuItem.action_menu_id > 0)
        {
            if (m_mGossipMenusMap.find(gMenuItem.action_menu_id) == m_mGossipMenusMap.end())
                sLog.outErrorDb("Gossip menu option (MenuId: %u Id: %u) have action_menu_id = %u for nonexistent menu", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_menu_id);
            else if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
                menu_ids.erase(gMenuItem.action_menu_id);
        }

        if (gMenuItem.option_icon >= GOSSIP_ICON_MAX)
        {
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown icon id %u. Replacing with GOSSIP_ICON_CHAT", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_icon);
            gMenuItem.option_icon = GOSSIP_ICON_CHAT;
        }

        if (gMenuItem.option_id == GOSSIP_OPTION_NONE)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u use option id GOSSIP_OPTION_NONE. Option will never be used", gMenuItem.menu_id, gMenuItem.id);

        if (gMenuItem.option_id >= GOSSIP_OPTION_MAX)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown option id %u. Option will not be used", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_id);

        if (gMenuItem.menu_id && gMenuItem.npc_option_npcflag)
        {
            bool found_menu_uses = false;
            bool found_flags_uses = false;

            std::pair<Menu2CInfoMap::const_iterator, Menu2CInfoMap::const_iterator> tm_bounds = menu2CInfoMap.equal_range(gMenuItem.menu_id);
            for (Menu2CInfoMap::const_iterator it2 = tm_bounds.first; !found_flags_uses && it2 != tm_bounds.second; ++it2)
            {
                CreatureInfo const* cInfo = it2->second;

                found_menu_uses = true;

                // some from creatures with gossip menu can use gossip option base at npc_flags
                if (gMenuItem.npc_option_npcflag & cInfo->NpcFlags)
                    found_flags_uses = true;
            }

            if (found_menu_uses && !found_flags_uses)
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has `npc_option_npcflag` = %u but creatures using this menu does not have corresponding `NpcFlags`. Option will not accessible in game.", gMenuItem.menu_id, gMenuItem.id, gMenuItem.npc_option_npcflag);
        }

        if (gMenuItem.action_poi_id && !GetPointOfInterest(gMenuItem.action_poi_id))
        {
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u use non-existing action_poi_id %u, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_poi_id);
            gMenuItem.action_poi_id = 0;
        }

        if (gMenuItem.action_script_id)
        {
            if (gossipScripts->second.find(gMenuItem.action_script_id) == gossipScripts->second.end())
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u have action_script_id %u that does not exist in `dbscripts_on_gossip`, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_script_id);
                continue;
            }

            // Remove used script id
            gossipScriptSet.erase(gMenuItem.action_script_id);
        }

        if (gMenuItem.conditionId)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(gMenuItem.conditionId);
            if (!condition)
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has condition_id %u that does not exist in `conditions`, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.conditionId);
                gMenuItem.conditionId = 0;
            }
        }

        m_mGossipMenuItemsMap.insert(GossipMenuItemsMap::value_type(gMenuItem.menu_id, gMenuItem));

        ++count;
    }
    while (queryResult->NextRow());

    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
    {
        for (uint32 menu_id : menu_ids)
        sLog.outErrorDb("Table `gossip_menu` contain unused (in creature or GO or menu options) menu id %u.", menu_id);
    }

    sLog.outString(">> Loaded %u gossip_menu_option entries", count);
    sLog.outString();
}

void ObjectMgr::LoadGossipMenus()
{
    auto gossipScripts = sScriptMgr.GetScriptMap(SCRIPT_TYPE_GOSSIP);
    // Check which script-ids in dbscripts_on_gossip are not used
    std::set<uint32> gossipScriptSet;
    for (auto itr = gossipScripts->second.begin(); itr != gossipScripts->second.end(); ++itr)
        gossipScriptSet.insert(itr->first);

    // Load gossip_menu and gossip_menu_option data
    sLog.outString("(Re)Loading Gossip menus...");
    LoadGossipMenu(gossipScriptSet);
    sLog.outString("(Re)Loading Gossip menu options...");
    LoadGossipMenuItems(gossipScriptSet);

    for (uint32 itr : gossipScriptSet)
        sLog.outErrorDb("Table `dbscripts_on_gossip` contains unused script, id %u.", itr);
}

void ObjectMgr::AddVendorItem(uint32 entry, uint32 item, uint32 maxcount, uint32 incrtime, uint32 extendedcost)
{
    VendorItemData& vList = m_mCacheVendorItemMap[entry];
    vList.AddItem(item, maxcount, incrtime, extendedcost, 0);

    WorldDatabase.PExecuteLog("INSERT INTO npc_vendor (entry,item,maxcount,incrtime,extendedcost) VALUES('%u','%u','%u','%u','%u')", entry, item, maxcount, incrtime, extendedcost);
}

bool ObjectMgr::RemoveVendorItem(uint32 entry, uint32 item)
{
    CacheVendorItemMap::iterator  iter = m_mCacheVendorItemMap.find(entry);
    if (iter == m_mCacheVendorItemMap.end())
        return false;

    if (!iter->second.RemoveItem(item))
        return false;

    WorldDatabase.PExecuteLog("DELETE FROM npc_vendor WHERE entry='%u' AND item='%u'", entry, item);
    return true;
}

bool ObjectMgr::IsVendorItemValid(bool isTemplate, char const* tableName, uint32 vendor_entry, uint32 item_id, uint32 maxcount, uint32 incrtime, uint32 ExtendedCost, uint16 conditionId, Player* pl) const
{
    char const* idStr = isTemplate ? "vendor template" : "vendor";
    CreatureInfo const* cInfo = nullptr;

    if (!isTemplate)
    {
        cInfo = GetCreatureTemplate(vendor_entry);
        if (!cInfo)
        {
            if (pl)
                ChatHandler(pl).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            else
                sLog.outErrorDb("Table `%s` has data for nonexistent creature (Entry: %u), ignoring", tableName, vendor_entry);
            return false;
        }
    }

    if (!GetItemPrototype(item_id))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_NOT_FOUND, item_id);
        else
            sLog.outErrorDb("Table `%s` for %s %u contain nonexistent item (%u), ignoring",
                            tableName, idStr, vendor_entry, item_id);
        return false;
    }

    if (ExtendedCost && !sItemExtendedCostStore.LookupEntry(ExtendedCost))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_EXTENDED_COST_NOT_EXIST, ExtendedCost);
        else
            sLog.outErrorDb("Table `%s` contain item (Entry: %u) with wrong ExtendedCost (%u) for %s %u, ignoring",
                            tableName, item_id, ExtendedCost, idStr, vendor_entry);
        return false;
    }

    if (maxcount > 0 && incrtime == 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount!=0 (%u) but IncrTime==0", maxcount);
        else
            sLog.outErrorDb("Table `%s` has `maxcount` (%u) for item %u of %s %u but `incrtime`=0, ignoring",
                            tableName, maxcount, item_id, idStr, vendor_entry);
        return false;
    }
    if (maxcount == 0 && incrtime > 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount==0 but IncrTime<>=0");
        else
            sLog.outErrorDb("Table `%s` has `maxcount`=0 for item %u of %s %u but `incrtime`<>0, ignoring",
                    tableName, item_id, idStr, vendor_entry);
        return false;
    }

    if (conditionId && !sConditionStorage.LookupEntry<ConditionEntry>(conditionId))
    {
        sLog.outErrorDb("Table `%s` has `condition_id`=%u for item %u of %s %u but this condition is not valid, ignoring", tableName, conditionId, item_id, idStr, vendor_entry);
        return false;
    }

    VendorItemData const* vItems = isTemplate ? GetNpcVendorTemplateItemList(vendor_entry) : GetNpcVendorItemList(vendor_entry);
    VendorItemData const* tItems = isTemplate ? nullptr : GetNpcVendorTemplateItemList(vendor_entry);

    if (!vItems && !tItems)
        return true;                                        // later checks for non-empty lists

    if (vItems && vItems->FindItemCostPair(item_id, ExtendedCost))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, item_id, ExtendedCost);
        else
            sLog.outErrorDb("Table `%s` has duplicate items %u (with extended cost %u) for %s %u, ignoring",
                            tableName, item_id, ExtendedCost, idStr, vendor_entry);
        return false;
    }

    if (!isTemplate)
    {
        if (tItems && tItems->FindItemCostPair(item_id, ExtendedCost))
        {
            if (pl)
                ChatHandler(pl).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, item_id, ExtendedCost);
            else
            {
                if (!cInfo->VendorTemplateId)
                    sLog.outErrorDb("Table `%s` has duplicate items %u (with extended cost %u) for %s %u, ignoring",
                                    tableName, item_id, ExtendedCost, idStr, vendor_entry);
                else
                    sLog.outErrorDb("Table `%s` has duplicate items %u (with extended cost %u) for %s %u (or possible in vendor template %u), ignoring",
                                    tableName, item_id, ExtendedCost, idStr, vendor_entry, cInfo->VendorTemplateId);
            }
            return false;
        }
    }

    uint32 countItems = vItems ? vItems->GetItemCount() : 0;
    countItems += tItems ? tItems->GetItemCount() : 0;

    if (countItems > std::numeric_limits<uint8>::max())
    {
        if (pl)
            ChatHandler(pl).SendSysMessage(LANG_COMMAND_ADDVENDORITEMITEMS);
        else
            sLog.outErrorDb("Table `%s` has too many items (%u > %i) for %s %u, ignoring",
                            tableName, countItems, std::numeric_limits<uint8>::max(), idStr, vendor_entry);
        return false;
    }

    return true;
}

void ObjectMgr::AddGroup(Group* group)
{
    mGroupMap[group->GetId()] = group ;
}

void ObjectMgr::RemoveGroup(Group* group)
{
    mGroupMap.erase(group->GetId());
}

void ObjectMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    mArenaTeamMap[arenaTeam->GetId()] = arenaTeam;
}

void ObjectMgr::RemoveArenaTeam(uint32 Id)
{
    mArenaTeamMap.erase(Id);
}

void ObjectMgr::GetCreatureLocaleStrings(uint32 entry, int32 loc_idx, char const** namePtr, char const** subnamePtr) const
{
    if (loc_idx >= 0)
    {
        if (CreatureLocale const* il = GetCreatureLocale(entry))
        {
            if (namePtr && il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                *namePtr = il->Name[loc_idx].c_str();

            if (subnamePtr && il->SubName.size() > size_t(loc_idx) && !il->SubName[loc_idx].empty())
                *subnamePtr = il->SubName[loc_idx].c_str();
        }
    }
}

void ObjectMgr::GetItemLocaleStrings(uint32 entry, int32 loc_idx, std::string* namePtr, std::string* descriptionPtr) const
{
    if (loc_idx >= 0)
    {
        if (ItemLocale const* il = GetItemLocale(entry))
        {
            if (namePtr && il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                *namePtr = il->Name[loc_idx];

            if (descriptionPtr && il->Description.size() > size_t(loc_idx) && !il->Description[loc_idx].empty())
                *descriptionPtr = il->Description[loc_idx];
        }
    }
}

void ObjectMgr::GetQuestLocaleStrings(uint32 entry, int32 loc_idx, std::string* titlePtr) const
{
    if (loc_idx >= 0)
    {
        if (QuestLocale const* il = GetQuestLocale(entry))
        {
            if (titlePtr && il->Title.size() > size_t(loc_idx) && !il->Title[loc_idx].empty())
                *titlePtr = il->Title[loc_idx];
        }
    }
}

void ObjectMgr::GetNpcTextLocaleStringsAll(uint32 entry, int32 loc_idx, ObjectMgr::NpcTextArray* text0_Ptr, ObjectMgr::NpcTextArray* text1_Ptr) const
{
    if (loc_idx >= 0)
    {
        if (NpcTextLocale const* nl = GetNpcTextLocale(entry))
        {
            if (text0_Ptr)
                for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; ++i)
                    if (nl->Text_0[i].size() > (size_t)loc_idx && !nl->Text_0[i][loc_idx].empty())
                        (*text0_Ptr)[i] = nl->Text_0[i][loc_idx];

            if (text1_Ptr)
                for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; ++i)
                    if (nl->Text_1[i].size() > (size_t)loc_idx && !nl->Text_1[i][loc_idx].empty())
                        (*text1_Ptr)[i] = nl->Text_1[i][loc_idx];
        }
    }
}

void ObjectMgr::GetNpcTextLocaleStrings0(uint32 entry, int32 loc_idx, std::string* text0_0_Ptr, std::string* text1_0_Ptr) const
{
    if (loc_idx >= 0)
    {
        if (NpcTextLocale const* nl = GetNpcTextLocale(entry))
        {
            if (text0_0_Ptr)
                if (nl->Text_0[0].size() > (size_t)loc_idx && !nl->Text_0[0][loc_idx].empty())
                    *text0_0_Ptr = nl->Text_0[0][loc_idx];

            if (text1_0_Ptr)
                if (nl->Text_1[0].size() > (size_t)loc_idx && !nl->Text_1[0][loc_idx].empty())
                    *text1_0_Ptr = nl->Text_1[0][loc_idx];
        }
    }
}

void ObjectMgr::GetQuestgiverGreetingLocales(uint32 entry, uint32 type, int32 loc_idx, std::string* titlePtr) const
{
    if (loc_idx >= 0)
    {
        if (QuestgiverGreetingLocale const* ql = GetQuestgiverGreetingLocale(entry, type))
        {
            if (titlePtr)
                if (ql->localeText.size() > (size_t)loc_idx && !ql->localeText[loc_idx].empty())
                    *titlePtr = ql->localeText[loc_idx];
        }
    }
}

void ObjectMgr::GetTrainerGreetingLocales(uint32 entry, int32 loc_idx, std::string* titlePtr) const
{
    if (loc_idx >= 0)
    {
        if (TrainerGreetingLocale const* tL = GetTrainerGreetingLocale(entry))
        {
            if (titlePtr)
                if (tL->localeText.size() > (size_t)loc_idx && !tL->localeText[loc_idx].empty())
                    *titlePtr = tL->localeText[loc_idx];
        }
    }
}

// Functions for scripting access
bool LoadMangosStrings(DatabaseType& db, char const* table, int32 start_value, int32 end_value, bool extra_content)
{
    return sObjectMgr.LoadMangosStrings(db, table, start_value, end_value, extra_content);
}

void ObjectMgr::LoadCreatureTemplateSpells(std::shared_ptr<CreatureSpellListContainer> container)
{
    uint32 count = 0;
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT entry, setId, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, spell9, spell10 FROM creature_template_spells"));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 entry = fields[0].GetUInt32();
            uint32 setId = fields[1].GetUInt32();

            if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
            {
                sLog.outErrorDb("LoadCreatureTemplateSpells: Spells found for creature entry %u, but creature does not exist, skipping", entry);
                continue;
            }

            if (container->spellLists.find(entry * 100 + setId) != container->spellLists.end())
            {
                sLog.outErrorDb("LoadCreatureTemplateSpells: Spells found for creature entry %u, but is already occupied by spell list of same id, skipping", entry);
                continue;
            }

            auto& spellList = container->spellLists[entry * 100 + setId];
            spellList.Disabled = true;
            auto& spells = spellList.Spells;

            for (uint32 i = 0; i < CREATURE_MAX_SPELLS; ++i)
            {
                uint32 spellId = fields[2 + i].GetUInt32();
                if (!spellId)
                    continue;

                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                if (!spellInfo && spellId != 2) // 2 is attack which is hardcoded in client
                {
                    sLog.outErrorDb("LoadCreatureTemplateSpells: Spells found for creature entry %u, assigned spell %u does not exist, set to 0", entry, spellId);
                    continue;
                }

                CreatureSpellListSpell spell;
                spell.Id = entry * 100 + setId;
                spell.Position = i;
                spell.SpellId = spellId;
                spell.Flags = 0;
                spell.Target = &container->targeting[1];
                spell.InitialMin = 0;
                spell.InitialMax = 0;

                auto cooldown = GetCreatureCooldownRange(entry, spellId);

                spell.RepeatMin = cooldown.first;
                spell.RepeatMax = cooldown.second;

                spell.Availability = 100;
                spell.Probability = 0;
                spell.ScriptId = 0;
                spell.DisabledForAI = !spellInfo || spellInfo->HasAttribute(SPELL_ATTR_EX_NO_AUTOCAST_AI);
                spells.emplace(i, spell);
            }            
        } while (result->NextRow());
    }

    sLog.outString(">> Loaded %u creature_cooldowns definitions", count);
    sLog.outString();
}

void ObjectMgr::LoadCreatureCooldowns()
{
    // not deleting on reload because some cooldowns are SD2 based - instead we overwrite only
    uint32 count = 0;
    auto queryResult = WorldDatabase.Query("SELECT Entry, SpellId, CooldownMin, CooldownMax FROM creature_cooldowns");

    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();

            uint32 entry = fields[0].GetUInt32();
            if (!sCreatureStorage.LookupEntry<CreatureInfo>(entry))
            {
                sLog.outErrorDb("LoadCreatureCooldowns: Entry %u does not exist.", entry);
                continue;
            }
            uint32 spellId = fields[1].GetUInt32();
            if (!sSpellTemplate.LookupEntry<SpellEntry>(spellId))
            {
                sLog.outErrorDb("LoadCreatureCooldowns: SpellId %u does not exist.", spellId);
                continue;
            }
            uint32 cooldownMin = fields[2].GetUInt32();
            uint32 cooldownMax = fields[3].GetUInt32();
            if (cooldownMin == 0 && cooldownMax == 0)
            {
                sLog.outErrorDb("LoadCreatureCooldowns: Cooldowns are both 0 for entry %u spellId %u - redundant entry.", entry, spellId);
                continue;
            }
            m_creatureCooldownMap[entry][spellId] = std::make_pair(cooldownMin, cooldownMax);
        } while (queryResult->NextRow());
    }

    sLog.outString(">> Loaded %u creature_cooldowns definitions", count);
    sLog.outString();
}

void ObjectMgr::AddCreatureCooldown(uint32 entry, uint32 spellId, uint32 min, uint32 max)
{
    m_creatureCooldownMap[entry].emplace(spellId, std::make_pair(min, max));
}

CreatureInfo const* GetCreatureTemplateStore(uint32 entry)
{
    return sCreatureStorage.LookupEntry<CreatureInfo>(entry);
}

Quest const* GetQuestTemplateStore(uint32 entry)
{
    return sObjectMgr.GetQuestTemplate(entry);
}

MangosStringLocale const* GetMangosStringData(int32 entry)
{
    return sObjectMgr.GetMangosStringLocale(entry);
}

bool FindCreatureData::operator()(CreatureDataPair const& dataPair)
{
    // skip wrong entry ids
    if (i_id && dataPair.second.id != i_id)
        return false;

    if (!i_anyData)
        i_anyData = &dataPair;

    // without player we can't find more stricted cases, so use fouded
    if (!i_player)
        return true;

    // skip diff. map cases
    if (dataPair.second.mapid != i_player->GetMapId())
        return false;

    float new_dist = i_player->GetDistance2d(dataPair.second.posX, dataPair.second.posY);

    if (!i_mapData || new_dist < i_mapDist)
    {
        i_mapData = &dataPair;
        i_mapDist = new_dist;
    }

    // skip not spawned (in any state),
    uint16 pool_id = sPoolMgr.IsPartOfAPool<Creature>(dataPair.first);
    if (pool_id && !i_player->GetMap()->GetPersistentState()->IsSpawnedPoolObject<Creature>(dataPair.first))
        return false;

    if (!i_spawnedData || new_dist < i_spawnedDist)
    {
        i_spawnedData = &dataPair;
        i_spawnedDist = new_dist;
    }

    return false;
}

CreatureDataPair const* FindCreatureData::GetResult() const
{
    if (i_spawnedData)
        return i_spawnedData;

    if (i_mapData)
        return i_mapData;

    return i_anyData;
}

bool FindGOData::operator()(GameObjectDataPair const& dataPair)
{
    // skip wrong entry ids
    if (i_id && dataPair.second.id != i_id)
        return false;

    if (!i_anyData)
        i_anyData = &dataPair;

    // without player we can't find more stricted cases, so use fouded
    if (!i_player)
        return true;

    // skip diff. map cases
    if (dataPair.second.mapid != i_player->GetMapId())
        return false;

    float new_dist = i_player->GetDistance2d(dataPair.second.posX, dataPair.second.posY);

    if (!i_mapData || new_dist < i_mapDist)
    {
        i_mapData = &dataPair;
        i_mapDist = new_dist;
    }

    // skip not spawned (in any state)
    uint16 pool_id = sPoolMgr.IsPartOfAPool<GameObject>(dataPair.first);
    if (pool_id && !i_player->GetMap()->GetPersistentState()->IsSpawnedPoolObject<GameObject>(dataPair.first))
        return false;

    if (!i_spawnedData || new_dist < i_spawnedDist)
    {
        i_spawnedData = &dataPair;
        i_spawnedDist = new_dist;
    }

    return false;
}

GameObjectDataPair const* FindGOData::GetResult() const
{
    if (i_mapData)
        return i_mapData;

    if (i_spawnedData)
        return i_spawnedData;

    return i_anyData;
}

bool DoDisplayText(WorldObject* source, int32 entry, Unit const* target, uint32 chatTypeOverride)
{
    uint32 sound, emote, type = 0;
    Language lang = LANG_UNIVERSAL;
    std::vector<std::string> content;
    Gender sourceGender = source->IsUnit() ? (Gender)((Unit*)source)->getGender() : GENDER_NONE;

    if (BroadcastText const* bct = sObjectMgr.GetBroadcastText(entry))
    {
        lang = bct->languageId;
        type = bct->chatTypeId;
        sound = bct->soundId1;
        emote = bct->emoteIds[0];
        content = bct->maleText;

        if ((sourceGender == GENDER_FEMALE || sourceGender == GENDER_NONE) && !bct->femaleText[DEFAULT_LOCALE].empty() && bct->femaleText.size() > 0)
            content = bct->femaleText;

        if (bct->maleText.size() > 0 && !bct->maleText[DEFAULT_LOCALE].empty())
            content = bct->maleText;
    }
    else if (MangosStringLocale const* data = sObjectMgr.GetMangosStringLocale(entry))
    {
        lang = data->LanguageId;
        type = data->Type;
        sound = data->SoundId;
        emote = data->Emote;
        if (BroadcastText const* bct = data->broadcastText)
        {
            if ((sourceGender == GENDER_FEMALE || sourceGender == GENDER_NONE) && !bct->femaleText[DEFAULT_LOCALE].empty() && bct->femaleText.size() > 0)
                content = bct->femaleText;

            if (bct->maleText.size() > 0 && !bct->maleText[DEFAULT_LOCALE].empty())
                content = bct->maleText;
        }
        else
            content = data->Content;
    }

    if (chatTypeOverride > 0)
        type = chatTypeOverride;

    if (content.empty())
    {
        _DoStringError("DoScriptText with source %s could not find text entry %i.", source->GetGuidStr().c_str(), entry);
        return false;
    }

    if (sound)
    {
        switch (type)
        {
            case CHAT_TYPE_ZONE_YELL:
            case CHAT_TYPE_ZONE_EMOTE:
                source->PlayDirectSound(sound, PlayPacketParameters(PlayPacketSettings::ZONE, source->GetZoneId()));
                break;
            case CHAT_TYPE_WHISPER:
            case CHAT_TYPE_BOSS_WHISPER:
                // An error will be displayed for the text
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                    source->PlayDirectSound(sound, PlayPacketParameters(PlayPacketSettings::TARGET, (Player const*)target));
                break;
            default:
                source->PlayDirectSound(sound);
                break;
        }
    }

    if (emote)
    {
        if (source->GetTypeId() == TYPEID_UNIT || source->GetTypeId() == TYPEID_PLAYER)
        {
            ((Unit*)source)->HandleEmote(emote);
        }
        else
        {
            _DoStringError("DoDisplayText entry %i tried to process emote for invalid source %s", entry, source->GetGuidStr().c_str());
            return false;
        }
    }

    if ((type == CHAT_TYPE_WHISPER || type == CHAT_TYPE_BOSS_WHISPER || type == CHAT_TYPE_PARTY) && (!target || target->GetTypeId() != TYPEID_PLAYER))
    {
        _DoStringError("DoDisplayText entry %i cannot whisper/party chat without target unit (TYPEID_PLAYER).", entry);
        return false;
    }

    source->MonsterText(content, type, lang, target);
    return true;
}
