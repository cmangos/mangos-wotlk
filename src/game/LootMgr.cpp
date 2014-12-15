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

#include "LootMgr.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "ProgressBar.h"
#include "World.h"
#include "Util.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "DBCStores.h"
#include "SQLStorages.h"
#include "BattleGround/BattleGroundAV.h"

INSTANTIATE_SINGLETON_1(LootMgr);

static eConfigFloatValues const qualityToRate[MAX_ITEM_QUALITY] =
{
    CONFIG_FLOAT_RATE_DROP_ITEM_POOR,                       // ITEM_QUALITY_POOR
    CONFIG_FLOAT_RATE_DROP_ITEM_NORMAL,                     // ITEM_QUALITY_NORMAL
    CONFIG_FLOAT_RATE_DROP_ITEM_UNCOMMON,                   // ITEM_QUALITY_UNCOMMON
    CONFIG_FLOAT_RATE_DROP_ITEM_RARE,                       // ITEM_QUALITY_RARE
    CONFIG_FLOAT_RATE_DROP_ITEM_EPIC,                       // ITEM_QUALITY_EPIC
    CONFIG_FLOAT_RATE_DROP_ITEM_LEGENDARY,                  // ITEM_QUALITY_LEGENDARY
    CONFIG_FLOAT_RATE_DROP_ITEM_ARTIFACT,                   // ITEM_QUALITY_ARTIFACT
};

LootStore LootTemplates_Creature("creature_loot_template",     "creature entry",                 true);
LootStore LootTemplates_Disenchant("disenchant_loot_template",   "item disenchant id",             true);
LootStore LootTemplates_Fishing("fishing_loot_template",      "area id",                        true);
LootStore LootTemplates_Gameobject("gameobject_loot_template",   "gameobject lootid",              true);
LootStore LootTemplates_Item("item_loot_template",         "item entry with ITEM_FLAG_LOOTABLE", true);
LootStore LootTemplates_Mail("mail_loot_template",         "mail template id",               false);
LootStore LootTemplates_Milling("milling_loot_template",      "item entry (herb)",              true);
LootStore LootTemplates_Pickpocketing("pickpocketing_loot_template", "creature pickpocket lootid",     true);
LootStore LootTemplates_Prospecting("prospecting_loot_template",  "item entry (ore)",               true);
LootStore LootTemplates_Reference("reference_loot_template",    "reference id",                   false);
LootStore LootTemplates_Skinning("skinning_loot_template",     "creature skinning id",           true);
LootStore LootTemplates_Spell("spell_loot_template",        "spell id (random item creating)", false);

class LootTemplate::LootGroup                               // A set of loot definitions for items (refs are not allowed)
{
    public:
        void AddEntry(LootStoreItem& item);                 // Adds an entry to the group (at loading stage)
        bool HasQuestDrop() const;                          // True if group includes at least 1 quest drop entry
        bool HasQuestDropForPlayer(Player const* player) const;
        // The same for active quests of the player
        void Process(Loot& loot) const;                     // Rolls an item from the group (if any) and adds the item to the loot
        float RawTotalChance() const;                       // Overall chance for the group (without equal chanced items)
        float TotalChance() const;                          // Overall chance for the group

        void Verify(LootStore const& lootstore, uint32 id, uint32 group_id) const;
        void CollectLootIds(LootIdSet& set) const;
        void CheckLootRefs(LootIdSet* ref_set) const;
    private:
        LootStoreItemList ExplicitlyChanced;                // Entries with chances defined in DB
        LootStoreItemList EqualChanced;                     // Zero chances - every entry takes the same chance

        LootStoreItem const* Roll() const;                  // Rolls an item from the group, returns nullptr if all miss their chances
};

// Remove all data and free all memory
void LootStore::Clear()
{
    for (LootTemplateMap::const_iterator itr = m_LootTemplates.begin(); itr != m_LootTemplates.end(); ++itr)
        delete itr->second;
    m_LootTemplates.clear();
}

// Checks validity of the loot store
// Actual checks are done within LootTemplate::Verify() which is called for every template
void LootStore::Verify() const
{
    for (LootTemplateMap::const_iterator i = m_LootTemplates.begin(); i != m_LootTemplates.end(); ++i)
        i->second->Verify(*this, i->first);
}

// Loads a *_loot_template DB table into loot store
// All checks of the loaded template are called from here, no error reports at loot generation required
void LootStore::LoadLootTable()
{
    LootTemplateMap::const_iterator tab;
    uint32 count = 0;

    // Clearing store (for reloading case)
    Clear();

    //                                                 0      1     2                    3        4              5         6
    QueryResult* result = WorldDatabase.PQuery("SELECT entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, condition_id FROM %s", GetName());

    if (result)
    {
        BarGoLink bar(result->GetRowCount());

        do
        {
            Field* fields = result->Fetch();
            bar.step();

            uint32 entry               = fields[0].GetUInt32();
            uint32 item                = fields[1].GetUInt32();
            float  chanceOrQuestChance = fields[2].GetFloat();
            uint8  group               = fields[3].GetUInt8();
            int32  mincountOrRef       = fields[4].GetInt32();
            uint32 maxcount            = fields[5].GetUInt32();
            uint16 conditionId         = fields[6].GetUInt16();

            if (maxcount > std::numeric_limits<uint8>::max())
            {
                sLog.outErrorDb("Table '%s' entry %d item %d: maxcount value (%u) to large. must be less %u - skipped", GetName(), entry, item, maxcount, std::numeric_limits<uint8>::max());
                continue;                                   // error already printed to log/console.
            }

            if (conditionId)
            {
                const PlayerCondition* condition = sConditionStorage.LookupEntry<PlayerCondition>(conditionId);
                if (!condition)
                {
                    sLog.outErrorDb("Table `%s` for entry %u, item %u has condition_id %u that does not exist in `conditions`, ignoring", GetName(), entry, item, conditionId);
                    continue;
                }

                if (mincountOrRef < 0 && !PlayerCondition::CanBeUsedWithoutPlayer(conditionId))
                {
                    sLog.outErrorDb("Table '%s' entry %u mincountOrRef %i < 0 and has condition %u that requires a player and is not supported, skipped", GetName(), entry, mincountOrRef, conditionId);
                    continue;
                }
            }

            LootStoreItem storeitem = LootStoreItem(item, chanceOrQuestChance, group, conditionId, mincountOrRef, maxcount);

            if (!storeitem.IsValid(*this, entry))           // Validity checks
                continue;

            // Looking for the template of the entry
            // often entries are put together
            if (m_LootTemplates.empty() || tab->first != entry)
            {
                // Searching the template (in case template Id changed)
                tab = m_LootTemplates.find(entry);
                if (tab == m_LootTemplates.end())
                {
                    std::pair< LootTemplateMap::iterator, bool > pr = m_LootTemplates.insert(LootTemplateMap::value_type(entry, new LootTemplate));
                    tab = pr.first;
                }
            }
            // else is empty - template Id and iter are the same
            // finally iter refers to already existing or just created <entry, LootTemplate>

            // Adds current row to the template
            tab->second->AddEntry(storeitem);
            ++count;
        }
        while (result->NextRow());

        delete result;

        Verify();                                           // Checks validity of the loot store

        sLog.outString(">> Loaded %u loot definitions (" SIZEFMTD " templates) from table %s", count, m_LootTemplates.size(), GetName());
        sLog.outString();
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 loot definitions. DB table `%s` is empty.", GetName());
    }
}

bool LootStore::HaveQuestLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator itr = m_LootTemplates.find(loot_id);
    if (itr == m_LootTemplates.end())
        return false;

    // scan loot for quest items
    return itr->second->HasQuestDrop(m_LootTemplates);
}

bool LootStore::HaveQuestLootForPlayer(uint32 loot_id, Player* player) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);
    if (tab != m_LootTemplates.end())
        if (tab->second->HasQuestDropForPlayer(m_LootTemplates, player))
            return true;

    return false;
}

LootTemplate const* LootStore::GetLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);

    if (tab == m_LootTemplates.end())
        return nullptr;

    return tab->second;
}

void LootStore::LoadAndCollectLootIds(LootIdSet& ids_set)
{
    LoadLootTable();

    for (LootTemplateMap::const_iterator tab = m_LootTemplates.begin(); tab != m_LootTemplates.end(); ++tab)
        ids_set.insert(tab->first);
}

void LootStore::CheckLootRefs(LootIdSet* ref_set) const
{
    for (LootTemplateMap::const_iterator ltItr = m_LootTemplates.begin(); ltItr != m_LootTemplates.end(); ++ltItr)
        ltItr->second->CheckLootRefs(ref_set);
}

void LootStore::ReportUnusedIds(LootIdSet const& ids_set) const
{
    // all still listed ids isn't referenced
    if (!ids_set.empty())
    {
        for (LootIdSet::const_iterator itr = ids_set.begin(); itr != ids_set.end(); ++itr)
            sLog.outErrorDb("Table '%s' entry %d isn't %s and not referenced from loot, and then useless.", GetName(), *itr, GetEntryName());
        sLog.outString();
    }
}

void LootStore::ReportNotExistedId(uint32 id) const
{
    sLog.outErrorDb("Table '%s' entry %d (%s) not exist but used as loot id in DB.", GetName(), id, GetEntryName());
}

//
// --------- LootStoreItem ---------
//

// Checks if the entry (quest, non-quest, reference) takes it's chance (at loot generation)
// RATE_DROP_ITEMS is no longer used for all types of entries
bool LootStoreItem::Roll(bool rate) const
{
    if (chance >= 100.0f)
        return true;

    if (mincountOrRef < 0)                                  // reference case
        return roll_chance_f(chance * (rate ? sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_ITEM_REFERENCED) : 1.0f));

    ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);

    float qualityModifier = pProto && rate ? sWorld.getConfig(qualityToRate[pProto->Quality]) : 1.0f;

    return roll_chance_f(chance * qualityModifier);
}

// Checks correctness of values
bool LootStoreItem::IsValid(LootStore const& store, uint32 entry) const
{
    if (group >= 1 << 7)                                    // it stored in 7 bit field
    {
        sLog.outErrorDb("Table '%s' entry %d item %d: group (%u) must be less %u - skipped", store.GetName(), entry, itemid, group, 1 << 7);
        return false;
    }

    if (mincountOrRef == 0)
    {
        sLog.outErrorDb("Table '%s' entry %d item %d: wrong mincountOrRef (%d) - skipped", store.GetName(), entry, itemid, mincountOrRef);
        return false;
    }

    if (mincountOrRef > 0)                                  // item (quest or non-quest) entry, maybe grouped
    {
        ItemPrototype const* proto = ObjectMgr::GetItemPrototype(itemid);
        if (!proto)
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: item entry not listed in `item_template` - skipped", store.GetName(), entry, itemid);
            return false;
        }

        if (chance == 0 && group == 0)                      // Zero chance is allowed for grouped entries only
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: equal-chanced grouped entry, but group not defined - skipped", store.GetName(), entry, itemid);
            return false;
        }

        if (chance != 0 && chance < 0.000001f)              // loot with low chance
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: low chance (%f) - skipped", store.GetName(), entry, itemid, chance);
            return false;
        }

        if (maxcount < mincountOrRef)                       // wrong max count
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: max count (%u) less that min count (%i) - skipped", store.GetName(), entry, itemid, uint32(maxcount), mincountOrRef);
            return false;
        }
    }
    else                                                    // mincountOrRef < 0
    {
        if (needs_quest)
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: negative chance is given for a reference, skipped", store.GetName(), entry, itemid);
            return false;
        }
        else if (chance == 0)                               // no chance for the reference
        {
            sLog.outErrorDb("Table '%s' entry %d item %d: zero chance is given for a reference, reference will never be used, skipped", store.GetName(), entry, itemid);
            return false;
        }
    }
    return true;                                            // Referenced template existence is checked at whole store level
}

//
// --------- LootItem ---------
//

// Constructor, copies most fields from LootStoreItem and generates random count
LootItem::LootItem(LootStoreItem const& li)
{
    itemid      = li.itemid;
    conditionId = li.conditionId;

    ItemPrototype const* proto = ObjectMgr::GetItemPrototype(itemid);
    freeforall  = proto && (proto->Flags & ITEM_FLAG_PARTY_LOOT);

    needs_quest = li.needs_quest;

    count       = urand(li.mincountOrRef, li.maxcount);     // constructor called for mincountOrRef > 0 only
    randomSuffix = GenerateEnchSuffixFactor(itemid);
    randomPropertyId = Item::GenerateItemRandomPropertyId(itemid);
    is_looted = 0;
    is_blocked = 0;
    is_underthreshold = 0;
    is_counted = 0;
}

LootItem::LootItem(uint32 itemid_, uint32 count_, uint32 randomSuffix_, int32 randomPropertyId_)
{
    itemid      = itemid_;
    conditionId = 0;

    ItemPrototype const* proto = ObjectMgr::GetItemPrototype(itemid);
    freeforall  = proto && (proto->Flags & ITEM_FLAG_PARTY_LOOT);

    needs_quest = false;

    count       = count_;
    randomSuffix = randomSuffix_;
    randomPropertyId = randomPropertyId_;
    is_looted = 0;
    is_blocked = 0;
    is_underthreshold = 0;
    is_counted = 0;
}

// Basic checks for player/item compatibility - if false no chance to see the item in the loot
bool LootItem::AllowedForPlayer(Player const* player, WorldObject const* lootTarget) const
{
    // DB conditions check
    if (conditionId && !sObjectMgr.IsPlayerMeetToCondition(conditionId, player, player->GetMap(), lootTarget, CONDITION_FROM_LOOT))
        return false;

    ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);
    if (!pProto)
        return false;

    // not show loot for not own team
    if ((pProto->Flags2 & ITEM_FLAG2_HORDE_ONLY) && player->GetTeam() != HORDE)
        return false;

    if ((pProto->Flags2 & ITEM_FLAG2_ALLIANCE_ONLY) && player->GetTeam() != ALLIANCE)
        return false;

    if (needs_quest)
    {
        // Checking quests for quest-only drop (check only quests requirements in this case)
        if (!player->HasQuestForItem(itemid))
            return false;
    }
    else
    {
        // Not quest only drop (check quest starting items for already accepted non-repeatable quests)
        if (pProto->StartQuest && player->GetQuestStatus(pProto->StartQuest) != QUEST_STATUS_NONE && !player->HasQuestForItem(itemid))
            return false;
    }

    return true;
}

LootSlotType LootItem::GetSlotTypeForSharedLoot(LootView const& lv, bool condition_ok /*= false*/) const
{
    // ignore looted, FFA (each player get own copy) and not allowed items
    if (is_looted || freeforall || (conditionId && !condition_ok) || !AllowedForPlayer(lv.viewer, lv.loot.GetLootTarget()))
        return MAX_LOOT_SLOT_TYPE;

    switch (lv.loot.lootMethod)
    {
        case FREE_FOR_ALL:
            return LOOT_SLOT_NORMAL;
        case GROUP_LOOT:
        case NEED_BEFORE_GREED:
        {
            if (!is_blocked)
            {
                if (lv.loot.isReleased || lv.viewer->GetObjectGuid() == lv.loot.currentLooterGuid)
                    return LOOT_SLOT_NORMAL;
                else
                    return MAX_LOOT_SLOT_TYPE;
            }
            return LOOT_SLOT_VIEW;
        }
        case MASTER_LOOT:
        {
            if (lv.viewer->GetObjectGuid() == lv.loot.masterOwnerGuid)
                return LOOT_SLOT_MASTER;
            if (is_underthreshold)
            {
                if (lv.loot.isReleased || lv.viewer->GetObjectGuid() == lv.loot.currentLooterGuid)
                    return LOOT_SLOT_NORMAL;
                return MAX_LOOT_SLOT_TYPE;
            }
            else
                return LOOT_SLOT_VIEW;
        }
        case ROUND_ROBIN:
        {
            if (lv.loot.isReleased || lv.viewer->GetObjectGuid() == lv.loot.currentLooterGuid)
                return LOOT_SLOT_OWNER;
            return MAX_LOOT_SLOT_TYPE;
        }
        case NOT_GROUP_TYPE_LOOT:
            return LOOT_SLOT_OWNER;
        default:
            return MAX_LOOT_SLOT_TYPE;
    }
}

//
// ------- Loot Roll -------
//

// Send the roll for the whole group
void GroupLootRoll::SendStartRoll()
{
    WorldPacket data(SMSG_LOOT_START_ROLL, (8 + 4 + 4 + 4 + 4 + 4 + 4 + 1));
    data << m_loot->GetLootGuid();                          // creature guid what we're looting
    data << uint32(m_loot->GetLootTarget()->GetMapId());    // 3.3.3 mapid
    data << uint32(m_itemSlot);                             // item slot in loot
    data << uint32(m_lootItem->itemid);                     // the itemEntryId for the item that shall be rolled for
    data << uint32(m_lootItem->randomSuffix);               // randomSuffix
    data << uint32(m_lootItem->randomPropertyId);           // item random property ID
    data << uint32(m_lootItem->count);                      // items in stack
    data << uint32(LOOT_ROLL_TIMEOUT);                      // the countdown time to choose "need" or "greed"

    size_t voteMaskPos = data.wpos();
    data << uint8(0);                                       // roll type mask, allowed choices (placeholder)

    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        if (itr->second.vote == ROLL_NOT_VALID)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr || !plr->GetSession())
            continue;
        // dependent from player
        RollVoteMask mask = m_voteMask;
        // In NEED_BEFORE_GREED need disabled for non-usable item for player
        if (m_loot->lootMethod == NEED_BEFORE_GREED && plr->CanUseItem(m_itemProto) != EQUIP_ERR_OK)
            mask = RollVoteMask(mask & ~ROLL_VOTE_MASK_NEED);
        data.put<uint8>(voteMaskPos, uint8(mask));

        plr->GetSession()->SendPacket(&data);
    }
}

// Send all passed message
void GroupLootRoll::SendAllPassed()
{
    WorldPacket data(SMSG_LOOT_ALL_PASSED, (8 + 4 + 4 + 4 + 4));
    data << m_loot->GetLootGuid();                          // creature guid what we're looting
    data << uint32(m_itemSlot);                             // item slot in loot
    data << uint32(m_lootItem->itemid);                     // the itemEntryId for the item that shall be rolled for
    data << uint32(m_lootItem->randomSuffix);               // randomSuffix
    data << uint32(m_lootItem->randomPropertyId);           // item random property ID

    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        if (itr->second.vote == ROLL_NOT_VALID)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr || !plr->GetSession())
            continue;

        plr->GetSession()->SendPacket(&data);
    }
}

// Send roll 'value' of the whole group and the winner to the whole group
void GroupLootRoll::SendLootRollWon(ObjectGuid const& targetGuid, uint32 rollNumber, RollVote rollType)
{
    WorldPacket data(SMSG_LOOT_ROLL_WON, (8 + 4 + 4 + 4 + 4 + 8 + 1 + 1));
    data << m_loot->GetLootGuid();                          // creature guid what we're looting
    data << uint32(m_itemSlot);                             // item slot in loot
    data << uint32(m_lootItem->itemid);                     // the itemEntryId for the item that shall be rolled for
    data << uint32(m_lootItem->randomSuffix);               // randomSuffix
    data << uint32(m_lootItem->randomPropertyId);           // item random property ID
    data << targetGuid;                                     // guid of the player who won.
    data << uint8(rollNumber);                              // rollnumber related to SMSG_LOOT_ROLL
    data << uint8(rollType);                                // Rolltype related to SMSG_LOOT_ROLL

    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        switch (itr->second.vote)
        {
            case ROLL_PASS:
            case ROLL_NOT_EMITED_YET:
            case ROLL_NOT_VALID:
                SendRoll(itr->first, 128, 128);
                break;
            default:
                SendRoll(itr->first, itr->second.number, itr->second.vote);
                break;
        }
    }

    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        if (itr->second.vote == ROLL_NOT_VALID)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr || !plr->GetSession())
            continue;
        plr->GetSession()->SendPacket(&data);
    }
}

// Send roll of targetGuid to the whole group (included targuetGuid)
void GroupLootRoll::SendRoll(ObjectGuid const& targetGuid, uint32 rollNumber, uint32 rollType)
{
    WorldPacket data(SMSG_LOOT_ROLL, (8 + 4 + 8 + 4 + 4 + 4 + 1 + 1 + 1));
    data << m_loot->GetLootGuid();                          // creature guid what we're looting
    data << uint32(m_itemSlot);                             // item slot in loot
    data << targetGuid;
    data << uint32(m_lootItem->itemid);                     // the itemEntryId for the item that shall be rolled for
    data << uint32(m_lootItem->randomSuffix);               // randomSuffix
    data << uint32(m_lootItem->randomPropertyId);           // item random property ID
    data << uint8(rollNumber);                              // 0: "Need for: [item name]" > 127: "you passed on: [item name]"      Roll number
    data << uint8(rollType);                                // 0: "Need for: [item name]" 0: "You have selected need for [item name] 1: need roll 2: greed roll
    data << uint8(0);                                       // auto pass on loot

    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        if (itr->second.vote == ROLL_NOT_VALID)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr || !plr->GetSession())
            continue;

        plr->GetSession()->SendPacket(&data);
    }
}

GroupLootRoll::~GroupLootRoll()
{
    if (m_isStarted)
        SendAllPassed();
}

// Start the group roll for the specified item
void GroupLootRoll::Start(Loot& loot, uint32 itemSlot)
{
    if (!m_isStarted)
    {
        if (itemSlot >= loot.items.size())
            return;

        // initialize the data needed for the roll
        m_lootItem = &loot.items[itemSlot];
        m_loot = &loot;
        m_itemSlot = itemSlot;
        m_lootItem->is_blocked = true;                      // block the item while rolling
        for (GuidSet::const_iterator itr = m_loot->ownerSet.begin(); itr != m_loot->ownerSet.end(); ++itr)
        {
            if (loot.lootMethod != NEED_BEFORE_GREED)
                m_rollVoteMap[*itr].vote = ROLL_NOT_EMITED_YET; // initialize player vote map
            else
            {

            }
        }

        // initialize item prototype and check enchant possibilities for this group
        m_voteMask = ROLL_VOTE_MASK_ALL;
        m_itemProto = ObjectMgr::GetItemPrototype(m_lootItem->itemid);
        if (m_itemProto->Flags2 & ITEM_FLAG2_NEED_ROLL_DISABLED)
            m_voteMask = RollVoteMask(m_voteMask & ~ROLL_VOTE_MASK_NEED);
        if (!m_itemProto->DisenchantID || uint32(m_itemProto->RequiredDisenchantSkill) > m_loot->maxEnchantSkill)
            m_voteMask = RollVoteMask(m_voteMask & ~ROLL_VOTE_MASK_DISENCHANT);

        SendStartRoll();
        m_endTime = time(NULL) + (LOOT_ROLL_TIMEOUT / 1000);
        m_isStarted = true;
    }
}

// Add vote from playerGuid
bool GroupLootRoll::PlayerVote(Player* player, RollVote vote)
{
    ObjectGuid const& playerGuid = player->GetObjectGuid();
    RollVoteMap::iterator voterItr = m_rollVoteMap.find(playerGuid);
    if (voterItr == m_rollVoteMap.end())
        return false;

    voterItr->second.vote = vote;

    if (vote != ROLL_PASS && vote != ROLL_NOT_VALID)
        voterItr->second.number = urand(1, 100);

    switch (vote)
    {
        case ROLL_PASS:                                     // Player choose pass
        {
            SendRoll(playerGuid, 128, 128);
            break;
        }
        case ROLL_NEED:                                     // player choose Need
        {
            SendRoll(playerGuid, 0, 0);
            player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
            break;
        }
        case ROLL_GREED:                                    // player choose Greed
        {
            SendRoll(playerGuid, 128, 2);
            player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
        }
        case ROLL_DISENCHANT:                               // player choose Disenchant
        {
            SendRoll(playerGuid, 128, 3);
            player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
        }
        default:                                            // Roll removed case
            return false;
            break;
    }
    return true;
}

// check if we can found a winner for this roll or if timer is expired
bool GroupLootRoll::UpdateRoll()
{
    RollVoteMap::const_iterator winnerItr = m_rollVoteMap.end();

    if (AllPlayerVoted(winnerItr) || m_endTime <= time(NULL))
    {
        Finish(winnerItr);
        return true;
    }
    return false;
}


/**
* \brief: Check if all player have voted and return true in that case. Also return current winner.
* \param: RollVoteMap::const_iterator& winnerItr > will be different than m_rollCoteMap.end() if winner exist. (Someone voted greed or need)
* \returns: bool > true if all players voted
**/
bool GroupLootRoll::AllPlayerVoted(RollVoteMap::const_iterator& winnerItr)
{
    uint32 notVoted = 0;
    bool isSomeoneNeed = false;

    winnerItr = m_rollVoteMap.end();
    for (RollVoteMap::const_iterator itr = m_rollVoteMap.begin(); itr != m_rollVoteMap.end(); ++itr)
    {
        switch (itr->second.vote)
        {
            case ROLL_NEED:
                if (!isSomeoneNeed || winnerItr == m_rollVoteMap.end() || itr->second.number > winnerItr->second.number)
                {
                    isSomeoneNeed = true;                                               // first passage will force to set winner because need is prioritized
                    winnerItr = itr;
                }
                break;
            case ROLL_GREED:
            case ROLL_DISENCHANT:
                if (!isSomeoneNeed)                                                      // if at least one need is detected then winner can't be a greed
                {
                    if (winnerItr == m_rollVoteMap.end() || itr->second.number > winnerItr->second.number)
                        winnerItr = itr;
                }
                break;
            // Explicitly passing excludes a player from winning loot, so no action required.
            case ROLL_PASS:
                break;
            case ROLL_NOT_EMITED_YET:
                ++notVoted;
                break;
        }
    }

    return notVoted == 0;
}

// terminate the roll
void GroupLootRoll::Finish(RollVoteMap::const_iterator& winnerItr)
{
    if (winnerItr == m_rollVoteMap.end())
    {
        SendAllPassed();
        m_loot->items[m_itemSlot].is_blocked = false;
        m_loot->isReleased = true;
    }
    else
    {
        SendLootRollWon(winnerItr->first, winnerItr->second.number, winnerItr->second.vote);

        Player* plr = sObjectMgr.GetPlayer(winnerItr->first);
        if (plr && plr->GetSession())
        {
            if (winnerItr->second.vote == ROLL_NEED)
                plr->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT, m_lootItem->itemid, winnerItr->second.number);
            else
                plr->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT, m_lootItem->itemid, winnerItr->second.number);
        }

        m_loot->SendItem(winnerItr->first, m_itemSlot);
    }
    m_isStarted = false;
}

//
// --------- Loot ---------
//

// Inserts the item into the loot (called by LootTemplate processors)
void Loot::AddItem(LootStoreItem const& item)
{
    if (item.needs_quest)                                   // Quest drop
    {
        if (m_questItems.size() < MAX_NR_QUEST_ITEMS)
            m_questItems.push_back(LootItem(item));
    }
    else if (items.size() < MAX_NR_LOOT_ITEMS)              // Non-quest drop
    {
        items.push_back(LootItem(item));

        // non-conditional one-player only items are counted here,
        // free for all items are counted in FillFFALoot(),
        // non-ffa conditionals are counted in FillNonQuestNonFFAConditionalLoot()
        if (!item.conditionId)
        {
            ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item.itemid);
            if (!proto || !(proto->Flags & ITEM_FLAG_PARTY_LOOT))
            {
                if (proto->Quality >= uint32(threshold))    // set flag for later to know that we have an over threshold item
                    haveItemOverThreshold = true;
                ++unlootedCount;
            }
        }
    }
}

// Calls processor of corresponding LootTemplate (which handles everything including references)
bool Loot::FillLoot(uint32 loot_id, LootStore const& store, Player* loot_owner, bool personal, bool noEmptyError)
{
    // Must be provided
    if (!loot_owner)
        return false;

    LootTemplate const* tab = store.GetLootFor(loot_id);

    if (!tab)
    {
        if (!noEmptyError)
            sLog.outErrorDb("Table '%s' loot id #%u used but it doesn't have records.", store.GetName(), loot_id);
        return false;
    }

    items.reserve(MAX_NR_LOOT_ITEMS);
    m_questItems.reserve(MAX_NR_QUEST_ITEMS);

    tab->Process(*this, store, store.IsRatesAllowed());     // Processing is done there, callback via Loot::AddItem()

    // Setting access rights for group loot case
    Group* pGroup = loot_owner->GetGroup();
    if (!personal && pGroup)
    {
        for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
            if (Player* pl = itr->getSource())
                FillNotNormalLootFor(pl);
    }
    // ... for personal loot
    else
        FillNotNormalLootFor(loot_owner);

    return true;
}

void Loot::FillNotNormalLootFor(Player* pl)
{
    uint32 plguid = pl->GetGUIDLow();

    QuestItemMap::const_iterator qmapitr = m_playerQuestItems.find(plguid);
    if (qmapitr == m_playerQuestItems.end())
        FillQuestLoot(pl);

    qmapitr = m_playerFFAItems.find(plguid);
    if (qmapitr == m_playerFFAItems.end())
        FillFFALoot(pl);

    qmapitr = m_playerNonQuestNonFFAConditionalItems.find(plguid);
    if (qmapitr == m_playerNonQuestNonFFAConditionalItems.end())
        FillNonQuestNonFFAConditionalLoot(pl);
}

QuestItemList* Loot::FillFFALoot(Player* player)
{
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem& item = items[i];
        if (!item.is_looted && item.freeforall && item.AllowedForPlayer(player, m_lootTarget))
        {
            ql->push_back(QuestItem(i));
            ++unlootedCount;
        }
    }
    if (ql->empty())
    {
        delete ql;
        return NULL;
    }

    m_playerFFAItems[player->GetGUIDLow()] = ql;
    return ql;
}

QuestItemList* Loot::FillQuestLoot(Player* player)
{
    if (items.size() == MAX_NR_LOOT_ITEMS) return NULL;
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < m_questItems.size(); ++i)
    {
        LootItem& item = m_questItems[i];
        if (!item.is_looted && item.AllowedForPlayer(player, m_lootTarget))
        {
            ql->push_back(QuestItem(i));

            // questitems get blocked when they first apper in a
            // player's quest vector
            //
            // increase once if one looter only, looter-times if free for all
            if (item.freeforall || !item.is_blocked)
                ++unlootedCount;

            item.is_blocked = true;

            if (items.size() + ql->size() == MAX_NR_LOOT_ITEMS)
                break;
        }
    }
    if (ql->empty())
    {
        delete ql;
        return NULL;
    }

    m_playerQuestItems[player->GetGUIDLow()] = ql;
    return ql;
}

bool Loot::IsLootedFor(ObjectGuid const& playerGuid) const
{
    QuestItemMap::const_iterator itr = m_playerQuestItems.find(playerGuid.GetCounter());
    if (itr == m_playerQuestItems.end() && unlootedCount == 0 && gold == 0)
        return true;
    return false;
}

bool Loot::IsLootedForAll() const
{
    if (m_playerQuestItems.empty() && unlootedCount == 0 && gold == 0)
        return true;
    return false;
}

bool Loot::CanLoot(ObjectGuid const& playerGuid, bool onlyRightCheck /*= false*/)
{
    // not in Guid list of possible owner mean cheat or big problem
    GuidSet::const_iterator itr = ownerSet.find(playerGuid);
    if (itr == ownerSet.end())
        return false;

    // only check if the player is on the loot list
    if (onlyRightCheck)
        return true;

    // is already looted?
    if (IsLootedFor(playerGuid))
        return false;

    if (lootMethod == NOT_GROUP_TYPE_LOOT || lootMethod == FREE_FOR_ALL)
        return true;

    // now check group right if any
    if (!m_playerFFAItems.empty())
        return true;

    // if it contain quest item then it will be lootable for that player
    if (m_playerQuestItems.find(playerGuid) != m_playerQuestItems.end())
        return true;

    if (haveItemOverThreshold)
    {
        // master loot have always loot right when the loot contain over threshold item
        if (lootMethod == MASTER_LOOT && playerGuid == masterOwnerGuid)
            return true;

        // player can all loot on 'group loot' or 'need before greed' loot type
        if (lootMethod != MASTER_LOOT && lootMethod != ROUND_ROBIN)
            return true;
    }

    // if the player is the current looter (his turn to loot under threshold item) or the current looter released the loot then the player can loot
    if (isReleased || playerGuid == currentLooterGuid)
        return true;

    return false;
}

QuestItemList* Loot::FillNonQuestNonFFAConditionalLoot(Player* player)
{
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem& item = items[i];
        if (!item.is_looted && !item.freeforall && item.conditionId && item.AllowedForPlayer(player, m_lootTarget))
        {
            ql->push_back(QuestItem(i));
            if (!item.is_counted)
            {
                ++unlootedCount;
                item.is_counted = true;
            }
        }
    }
    if (ql->empty())
    {
        delete ql;
        return NULL;
    }

    m_playerNonQuestNonFFAConditionalItems[player->GetGUIDLow()] = ql;
    return ql;
}

//===================================================

void Loot::NotifyItemRemoved(uint8 lootIndex)
{
    // notify all players that are looting this that the item was removed
    // convert the index to the slot the player sees
    GuidSet::iterator i_next;
    for (GuidSet::iterator i = m_playersLooting.begin(); i != m_playersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        Player* plr = ObjectAccessor::FindPlayer(*i);
        if (plr && plr->GetSession())
        {
            WorldPacket data(SMSG_LOOT_REMOVED, 1);
            data << uint8(lootIndex);
            plr->GetSession()->SendPacket(&data);
        }
        else
            m_playersLooting.erase(i);
    }
}

void Loot::NotifyMoneyRemoved()
{
    // notify all players that are looting this that the money was removed
    GuidSet::iterator i_next;
    for (GuidSet::iterator i = m_playersLooting.begin(); i != m_playersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        Player* plr = ObjectAccessor::FindPlayer(*i);
        if (plr && plr->GetSession())
        {
            WorldPacket data(SMSG_LOOT_CLEAR_MONEY, 0);
            plr->GetSession()->SendPacket(&data);
        }
        else
            m_playersLooting.erase(i);
    }
}

void Loot::NotifyQuestItemRemoved(uint8 questIndex)
{
    // when a free for all questitem is looted
    // all players will get notified of it being removed
    // (other questitems can be looted by each group member)
    // bit inefficient but isnt called often

    GuidSet::iterator i_next;
    for (GuidSet::iterator i = m_playersLooting.begin(); i != m_playersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* pl = ObjectAccessor::FindPlayer(*i))
        {
            QuestItemMap::const_iterator pq = m_playerQuestItems.find(pl->GetGUIDLow());
            if (pq != m_playerQuestItems.end() && pq->second)
            {
                // find where/if the player has the given item in it's vector
                QuestItemList& pql = *pq->second;

                uint8 j;
                for (j = 0; j < pql.size(); ++j)
                    if (pql[j].index == questIndex)
                        break;

                if (j < pql.size())
                    NotifyItemRemoved(items.size() + j);
            }
        }
        else
            m_playersLooting.erase(i);
    }
}

void Loot::generateMoneyLoot(uint32 minAmount, uint32 maxAmount)
{
    if (maxAmount > 0)
    {
        if (maxAmount <= minAmount)
            gold = uint32(maxAmount * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));
        else if ((maxAmount - minAmount) < 32700)
            gold = uint32(urand(minAmount, maxAmount) * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));
        else
            gold = uint32(urand(minAmount >> 8, maxAmount >> 8) * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY)) << 8;
    }
}

LootItem* Loot::LootItemInSlot(uint32 lootSlot, Player* player, QuestItem** qitem, QuestItem** ffaitem, QuestItem** conditem)
{
    LootItem* item = NULL;
    bool is_looted = true;
    if (lootSlot >= items.size())
    {
        uint32 questSlot = lootSlot - items.size();
        QuestItemMap::const_iterator itr = m_playerQuestItems.find(player->GetGUIDLow());
        if (itr != m_playerQuestItems.end() && questSlot < itr->second->size())
        {
            QuestItem* qitem2 = &itr->second->at(questSlot);
            if (qitem)
                *qitem = qitem2;
            item = &m_questItems[qitem2->index];
            is_looted = qitem2->is_looted;
        }
    }
    else
    {
        item = &items[lootSlot];
        is_looted = item->is_looted;
        if (item->freeforall)
        {
            QuestItemMap::const_iterator itr = m_playerFFAItems.find(player->GetGUIDLow());
            if (itr != m_playerFFAItems.end())
            {
                for (QuestItemList::const_iterator iter = itr->second->begin(); iter != itr->second->end(); ++iter)
                    if (iter->index == lootSlot)
                    {
                        QuestItem* ffaitem2 = (QuestItem*) & (*iter);
                        if (ffaitem)
                            *ffaitem = ffaitem2;
                        is_looted = ffaitem2->is_looted;
                        break;
                    }
            }
        }
        else if (item->conditionId)
        {
            QuestItemMap::const_iterator itr = m_playerNonQuestNonFFAConditionalItems.find(player->GetGUIDLow());
            if (itr != m_playerNonQuestNonFFAConditionalItems.end())
            {
                for (QuestItemList::const_iterator iter = itr->second->begin(); iter != itr->second->end(); ++iter)
                {
                    if (iter->index == lootSlot)
                    {
                        QuestItem* conditem2 = (QuestItem*) & (*iter);
                        if (conditem)
                            *conditem = conditem2;
                        is_looted = conditem2->is_looted;
                        break;
                    }
                }
            }
        }
    }

    if (is_looted)
        return NULL;

    return item;
}

void Loot::SendReleaseFor(ObjectGuid const& guid)
{
    Player* plr = sObjectAccessor.FindPlayer(guid);
    if (plr || plr->GetSession())
        SendReleaseFor(plr);
}

// no check for null pointer so it must be valid
void Loot::SendReleaseFor(Player* plr)
{
    WorldPacket data(SMSG_LOOT_RELEASE_RESPONSE, (8 + 1));
    data << m_guidTarget;
    data << uint8(1);
    plr->GetSession()->SendPacket(&data);
    m_playersLooting.erase(plr->GetObjectGuid());
    plr->SetLootGuid(ObjectGuid());
}

void Loot::SendReleaseForAll()
{
    GuidSet::iterator itr = m_playersLooting.begin();
    while (itr != m_playersLooting.end())
    {
        SendReleaseFor(*itr++);
    }
}

void Loot::Release(Player* player)
{
    switch (m_guidTarget.GetHigh())
    {
        case HIGHGUID_GAMEOBJECT:
        {
            GameObject* go = (GameObject*) m_lootTarget;

            if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR)
            {
                // locked doors are opened with spelleffect openlock, prevent remove its as looted
                go->UseDoorOrButton();
            }
            else if (IsLootedFor(player->GetObjectGuid()) || go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
            {
                // GO is mineral vein? so it is not removed after its looted
                if (go->GetGoType() == GAMEOBJECT_TYPE_CHEST)
                {
                    uint32 go_min = go->GetGOInfo()->chest.minSuccessOpens;
                    uint32 go_max = go->GetGOInfo()->chest.maxSuccessOpens;

                    // only vein pass this check
                    if (go_min != 0 && go_max > go_min)
                    {
                        float amount_rate = sWorld.getConfig(CONFIG_FLOAT_RATE_MINING_AMOUNT);
                        float min_amount = go_min * amount_rate;
                        float max_amount = go_max * amount_rate;

                        go->AddUse();
                        float uses = float(go->GetUseCount());

                        if (uses < max_amount)
                        {
                            if (uses >= min_amount)
                            {
                                float chance_rate = sWorld.getConfig(CONFIG_FLOAT_RATE_MINING_NEXT);

                                int32 ReqValue = 175;
                                LockEntry const* lockInfo = sLockStore.LookupEntry(go->GetGOInfo()->chest.lockId);
                                if (lockInfo)
                                    ReqValue = lockInfo->Skill[0];
                                float skill = float(player->GetSkillValue(SKILL_MINING)) / (ReqValue + 25);
                                double chance = pow(0.8 * chance_rate, 4 * (1 / double(max_amount)) * double(uses));
                                if (roll_chance_f(float(100.0f * chance + skill)))
                                {
                                    go->SetLootState(GO_READY);
                                }
                                else                        // not have more uses
                                    go->SetLootState(GO_JUST_DEACTIVATED);
                            }
                            else                            // 100% chance until min uses
                                go->SetLootState(GO_READY);
                        }
                        else                                // max uses already
                            go->SetLootState(GO_JUST_DEACTIVATED);
                    }
                    else                                    // not vein
                        go->SetLootState(GO_JUST_DEACTIVATED);
                }
                else if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGHOLE)
                {
                    // The fishing hole used once more
                    go->AddUse();                           // if the max usage is reached, will be despawned at next tick
                    if (go->GetUseCount() >= urand(go->GetGOInfo()->fishinghole.minSuccessOpens, go->GetGOInfo()->fishinghole.maxSuccessOpens))
                    {
                        go->SetLootState(GO_JUST_DEACTIVATED);
                    }
                    else
                        go->SetLootState(GO_READY);
                }
                else // not chest (or vein/herb/etc)
                    go->SetLootState(GO_JUST_DEACTIVATED);

                clear();
            }
            else
                // not fully looted object
                go->SetLootState(GO_ACTIVATED);
            break;
        }
        case HIGHGUID_CORPSE:                               // ONLY remove insignia at BG
        {
            Corpse* corpse = (Corpse*) m_lootTarget;
            if (!corpse || !corpse->IsWithinDistInMap(player, INTERACTION_DISTANCE))
                return;

            if (IsLootedFor(player->GetObjectGuid()))
            {
                clear();
                corpse->RemoveFlag(CORPSE_FIELD_DYNAMIC_FLAGS, CORPSE_DYNFLAG_LOOTABLE);
            }
            break;
        }
        case HIGHGUID_ITEM:
        {
            switch (lootType)
            {
                // temporary loot in stacking items, clear loot state, no auto loot move
                case LOOT_MILLING:
                case LOOT_PROSPECTING:
                {
                    uint32 count = m_itemTarget->GetCount();

                    // >=5 checked in spell code, but will work for cheating cases also with removing from another stacks.
                    if (count > 5)
                        count = 5;

                    // reset loot for allow repeat looting if stack > 5
                    clear();
                    m_itemTarget->SetLootState(ITEM_LOOT_REMOVED);

                    player->DestroyItemCount(m_itemTarget, count, true);
                    break;
                }
                    // temporary loot, auto loot move
                case LOOT_DISENCHANTING:
                {
                    if (!IsLootedFor(player->GetObjectGuid()))
                        AutoStore(player); // can be lost if no space
                    clear();
                    m_itemTarget->SetLootState(ITEM_LOOT_REMOVED);
                    player->DestroyItem(m_itemTarget->GetBagSlot(), m_itemTarget->GetSlot(), true);
                    break;
                }
                    // normal persistence loot
                default:
                {
                    // must be destroyed only if no loot
                    if (IsLootedFor(player->GetObjectGuid()))
                    {
                        m_itemTarget->SetLootState(ITEM_LOOT_REMOVED);
                        player->DestroyItem(m_itemTarget->GetBagSlot(), m_itemTarget->GetSlot(), true);
                    }
                    break;
                }
            }
            return;                                         // item can be looted only single player
        }
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:
        {
            switch (lootType)
            {
                case LOOT_PICKPOCKETING:
                {
                    //creature->AllLootRemovedFromCorpse();
                    if (IsLootedFor(player->GetObjectGuid()))
                    {
                        Creature* creature = (Creature*)m_lootTarget;
                        creature->SetLootStatus(CREATURE_LOOT_STATUS_PICKPOCKETED);
                    }
                    break;
                }
                case LOOT_SKINNING:
                    //creature->AllLootRemovedFromCorpse();
                    if (IsLootedFor(player->GetObjectGuid()))
                    {
                        Creature* creature = (Creature*)m_lootTarget;
                        creature->SetLootStatus(CREATURE_LOOT_STATUS_SKINNED);
                    }
                    break;
                case LOOT_CORPSE:
                {
                    // the owner of the loot released it
                    if (player->GetObjectGuid() == currentLooterGuid)
                        isReleased = true;

                    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
                    Creature* creature = (Creature*)m_lootTarget;
                    if (IsLootedFor(player->GetObjectGuid()))
                    {
                        // for example skinning after normal loot
                        //pCreature->PrepareBodyLootState(pCreature);
                        //creature->AllLootRemovedFromCorpse();
                        creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        SendReleaseForAll();
                        CreatureInfo const* creatureInfo = creature->GetCreatureInfo();
                        creature->SetLootStatus(CREATURE_LOOT_STATUS_LOOTED);
                        if (creatureInfo->SkinningLootId)
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
                    }
                    else
                    {
                        ForceLootAnimationCLientUpdate(); // set the loot available for other player
                        SendReleaseFor(player->GetObjectGuid());
                    }
                    break;
                }
            }
            break;
        }
    }
}

// Popup windows with loot content
void Loot::ShowContentTo(Player* plr)
{
    if (lootMethod != NOT_GROUP_TYPE_LOOT && !isChecked)
        GroupCheck();

    WorldPacket data(SMSG_LOOT_RESPONSE);
    data << m_guidTarget;
    data << uint8(lootType);
    if (ownerSet.find(plr->GetObjectGuid()) != ownerSet.end())
    {
        // player have some right to see the loot
        data << LootView(*this, plr);
        m_playersLooting.insert(plr->GetObjectGuid());      // add 'this' player as one of the players that are looting 'loot'
        plr->SetLootGuid(m_guidTarget);                     // used to keep track of what loot is opened for that player
        if (m_guidTarget.IsCreatureOrVehicle())
            plr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
    }
    else
    {
        // player cannot see the loot
        data << uint32(0);                                  // gold
        data << uint8(0);                                   // item count
    }
    plr->SendDirectMessage(&data);
}

void Loot::GroupCheck()
{
    isChecked = true;
    switch (lootMethod)
    {
        case MASTER_LOOT:
        {
            uint8 playerCount = 0;
            WorldPacket data(SMSG_LOOT_MASTER_LIST);
            data << uint8(0);
            for (GuidSet::const_iterator itr = ownerSet.begin(); itr != ownerSet.end(); ++itr)
            {
                Player* looter = sObjectAccessor.FindPlayer(*itr);
                if (!looter)
                    continue;
                data << *itr;
                ++playerCount;
            }
            data.put<uint8>(0, playerCount);

            for (GuidSet::const_iterator itr = ownerSet.begin(); itr != ownerSet.end(); ++itr)
            {
                Player* looter = sObjectAccessor.FindPlayer(*itr);
                if (!looter)
                    continue;
                looter->GetSession()->SendPacket(&data);
            }

            for (uint8 itemSlot = 0; itemSlot < items.size(); ++itemSlot)
            {
                LootItem& lootItem = items[itemSlot];
                ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(lootItem.itemid);
                if (!itemProto)
                {
                    DEBUG_LOG("Loot::GroupCheck> missing item prototype for item with id: %d", lootItem.itemid);
                    continue;
                }

                // roll for over-threshold item if it's one-player loot
                if (lootItem.freeforall || itemProto->Quality < uint32(threshold))
                    lootItem.is_underthreshold = 1;
            }
            break;
        }
        case NEED_BEFORE_GREED:
        case GROUP_LOOT:
        {
            for (uint8 itemSlot = 0; itemSlot < items.size(); ++itemSlot)
            {
                LootItem& lootItem = items[itemSlot];
                ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(lootItem.itemid);
                if (!itemProto)
                {
                    DEBUG_LOG("Loot::GroupCheck> missing item prototype for item with id: %d", lootItem.itemid);
                    continue;
                }

                // roll for over-threshold item if it's one-player loot
                if (itemProto->Quality >= uint32(threshold) && !lootItem.freeforall)
                    roll[itemSlot].Start(*this, itemSlot);
                else
                    lootItem.is_underthreshold = 1;
            }
            break;
        }
    }
}

void Loot::SetGroupLootRight(Player* player)
{
    Group* grp = player->GetGroup();
    if (grp)
    {
        // filling the player who have access to the loot
        Group::MemberSlotList const& memberList = grp->GetMemberSlots();
        for (Group::MemberSlotList::const_iterator itr = memberList.begin(); itr != memberList.end(); ++itr)
        {
            Player* looter = ObjectAccessor::FindPlayer(itr->guid);

            if (!looter)
                continue;

            if (looter->IsWithinDist(m_lootTarget, sWorld.getConfig(CONFIG_FLOAT_GROUP_XP_DISTANCE), false))
            {
                ownerSet.insert(itr->guid);

                // get enchant skill of authorized looter
                uint32 enchantSkill = looter->GetSkillValue(SKILL_ENCHANTING);
                if (maxEnchantSkill < enchantSkill)
                    maxEnchantSkill = enchantSkill;
            }
        }

        // if more than one player have right to loot than we have to handle group method, round robin, roll, etc..
        if (ownerSet.size() > 1)
        {
            lootMethod = grp->GetLootMethod();
            threshold = grp->GetLootThreshold();
            switch (lootMethod)
            {
                case MASTER_LOOT:
                    masterOwnerGuid = grp->GetMasterLooterGuid();
                    // check if master is in looter list
                    if (ownerSet.find(masterOwnerGuid) == ownerSet.end())
                        ownerSet.insert(masterOwnerGuid);

                    currentLooterGuid = grp->GetCurrentLooterGuid();
                    grp->UpdateCurrentLooterGuid(m_lootTarget);
                    SendAllowedLooter();
                    break;
                case NEED_BEFORE_GREED:
                    currentLooterGuid = grp->GetCurrentLooterGuid();
                    grp->UpdateCurrentLooterGuid(m_lootTarget);
                    SendAllowedLooter();
                    break;
                case GROUP_LOOT:
                    currentLooterGuid = grp->GetCurrentLooterGuid();
                    grp->UpdateCurrentLooterGuid(m_lootTarget);
                    SendAllowedLooter();
                    break;
                case ROUND_ROBIN:
                    currentLooterGuid = grp->GetCurrentLooterGuid();
                    grp->UpdateCurrentLooterGuid(m_lootTarget);
                    SendAllowedLooter();
                    break;
                case FREE_FOR_ALL:
                    break;
                default:
                    break;
            }
        }
        else
        {
            ownerSet.insert(player->GetObjectGuid());
            lootMethod = NOT_GROUP_TYPE_LOOT;
        }
    }
    else
    {
        ownerSet.insert(player->GetObjectGuid());
        lootMethod = NOT_GROUP_TYPE_LOOT;
    }
}


Loot::Loot(Player* player, Creature* creature, LootType type) :
    lootType(LOOT_NONE), lootMethod(NOT_GROUP_TYPE_LOOT), threshold(ITEM_QUALITY_UNCOMMON),
    gold(0), unlootedCount(0), maxEnchantSkill(0), isReleased(false), haveItemOverThreshold(false),
    isChecked(false), m_lootTarget(NULL)
{
    // the player whose group may loot the corpse
    if (!player)
    {
        sLog.outError("LootMgr::CreateLoot> Error cannot get looter info to create loot!");
        return;
    }

    if (!creature)
    {
        sLog.outError("Loot::CreateLoot> cannot create loot, no creature passed!");
        return;
    }

    m_lootTarget = creature;
    m_guidTarget = creature->GetObjectGuid();
    CreatureInfo const* creatureInfo = creature->GetCreatureInfo();

    clear();
    lootType = type;

    switch (type)
    {
        case LOOT_CORPSE:
        {
            // setting loot right
            SetGroupLootRight(player);

            if ((creatureInfo->LootId && FillLoot(creatureInfo->LootId, LootTemplates_Creature, player, false)) || creatureInfo->MaxLootGold > 0)
            {
                generateMoneyLoot(creatureInfo->MinLootGold, creatureInfo->MaxLootGold);
                creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                break;
            }

            sLog.outDebug("Loot::CreateLoot> cannot create corpse loot, FillLoot failed with loot id(%u)!", creatureInfo->LootId);

            // loot is empty, can we show empty loot?
            if (!creatureInfo->SkinningLootId || sWorld.getConfig(CONFIG_BOOL_CORPSE_EMPTY_LOOT_SHOW))
                return;

            // loot is empty so we can set the corpse as skinnable
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
            break;
        }
        case LOOT_PICKPOCKETING:
        {
            if (!creature->isAlive() || player->getClass() != CLASS_ROGUE) // TODO add a flag to creature to check if already pickpocketed
                return;

            if (!creatureInfo->LootId || !FillLoot(creatureInfo->PickpocketLootId, LootTemplates_Pickpocketing, player, false))
            {
                sLog.outError("Loot::CreateLoot> cannot create pickpocket loot, FillLoot failed with loot id(%u)!", creatureInfo->LootId);
                return;
            }

            // Generate extra money for pick pocket loot
            const uint32 a = urand(0, creature->getLevel() / 2);
            const uint32 b = urand(0, player->getLevel() / 2);
            gold = uint32(10 * (a + b) * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));

            // setting loot right
            ownerSet.insert(player->GetObjectGuid());
            lootMethod = NOT_GROUP_TYPE_LOOT;
            break;
        }
        case LOOT_SKINNING:
        {
            if (!creatureInfo->SkinningLootId || !FillLoot(creatureInfo->SkinningLootId, LootTemplates_Skinning, player, false))
            {
                sLog.outError("Loot::CreateLoot> cannot create skinning loot, FillLoot failed with loot id(%u)!", creatureInfo->SkinningLootId);
                return;
            }

            // setting loot right
            ownerSet.insert(player->GetObjectGuid());
            lootMethod = NOT_GROUP_TYPE_LOOT;
            break;
        }
        default:
            sLog.outError("Loot::CreateLoot> Cannot create loot for %s with invalid LootType(%u)", creature->GetObjectGuid().GetString().c_str(), uint32(type));
            lootType = LOOT_NONE;
            break;
    }

    sLog.outString("CreateLoot: %s", haveItemOverThreshold ? "have over threshold item" : "have not over threshold item");

    return;
}

Loot::Loot(Player* player, GameObject* gameObject, LootType type) :
    lootType(LOOT_NONE), lootMethod(NOT_GROUP_TYPE_LOOT), threshold(ITEM_QUALITY_UNCOMMON),
    gold(0), unlootedCount(0), maxEnchantSkill(0), isReleased(false), haveItemOverThreshold(false),
    isChecked(false), m_lootTarget(NULL)
{
    // the player whose group may loot the corpse
    if (!player)
    {
        sLog.outError("LootMgr::CreateLoot> Error cannot get looter info to create loot!");
        return;
    }

    if (!gameObject)
    {
        sLog.outError("Loot::CreateLoot> cannot create game object loot, no game object passed!");
        return;
    }

    m_lootTarget = gameObject;
    m_guidTarget = gameObject->GetObjectGuid();

    // not check distance for GO in case owned GO (fishing bobber case, for example)
    // And permit out of range GO with no owner in case fishing hole
    if ((type != LOOT_FISHINGHOLE &&
        ((type != LOOT_FISHING && type != LOOT_FISHING_FAIL) || gameObject->GetOwnerGuid() != player->GetObjectGuid()) &&
        !gameObject->IsWithinDistInMap(player, INTERACTION_DISTANCE)))
    {
        sLog.outError("Loot::CreateLoot> cannot create game object loot, basic check failed!");
        return;
    }

    clear();
    lootType = type;

    // generate loot only if ready for open and spawned in world
    if (gameObject->getLootState() == GO_READY && gameObject->isSpawned())
    {
        if ((gameObject->GetEntry() == BG_AV_OBJECTID_MINE_N || gameObject->GetEntry() == BG_AV_OBJECTID_MINE_S))
        {
            if (BattleGround* bg = player->GetBattleGround())
                if (bg->GetTypeID() == BATTLEGROUND_AV)
                    if (!(((BattleGroundAV*)bg)->PlayerCanDoMineQuest(gameObject->GetEntry(), player->GetTeam())))
                    {
                        return;
                    }
        }

        switch (type)
        {
            case LOOT_FISHING_FAIL:
            {
                // Entry 0 in fishing loot template used for store junk fish loot at fishing fail it junk allowed by config option
                // this is overwrite fishinghole loot for example
                FillLoot(0, LootTemplates_Fishing, player, true);

                // setting loot right
                ownerSet.insert(player->GetObjectGuid());
                lootMethod = NOT_GROUP_TYPE_LOOT;
                lootType = LOOT_FISHING;
                break;
            }
            case LOOT_FISHING:
            {
                uint32 zone, subzone;
                gameObject->GetZoneAndAreaId(zone, subzone);
                // if subzone loot exist use it
                if (!FillLoot(subzone, LootTemplates_Fishing, player, true, (subzone != zone)) && subzone != zone)
                    // else use zone loot (if zone diff. from subzone, must exist in like case)
                    FillLoot(zone, LootTemplates_Fishing, player, true);

                // setting loot right
                ownerSet.insert(player->GetObjectGuid());
                lootMethod = NOT_GROUP_TYPE_LOOT;
                break;
            }
            default:
            {
                if (uint32 lootid = gameObject->GetGOInfo()->GetLootId())
                {
                    if (gameObject->GetGOInfo()->type == GAMEOBJECT_TYPE_CHEST && gameObject->GetGOInfo()->chest.groupLootRules)
                        SetGroupLootRight(player);
                    else
                    {
                        ownerSet.insert(player->GetObjectGuid());
                        lootMethod = NOT_GROUP_TYPE_LOOT;
                    }

                    FillLoot(lootid, LootTemplates_Gameobject, player, false);
                    generateMoneyLoot(gameObject->GetGOInfo()->MinMoneyLoot, gameObject->GetGOInfo()->MaxMoneyLoot);

                    if (lootType == LOOT_FISHINGHOLE)
                        lootType = LOOT_FISHING;
                }
                break;
            }
        }

        gameObject->SetLootState(GO_ACTIVATED);
    }
    return;
}

Loot::Loot(Player* player, Corpse* corpse, LootType type) :
    lootType(LOOT_NONE), lootMethod(NOT_GROUP_TYPE_LOOT), threshold(ITEM_QUALITY_UNCOMMON),
    gold(0), unlootedCount(0), maxEnchantSkill(0), isReleased(false), haveItemOverThreshold(false),
    isChecked(false), m_lootTarget(NULL)
{
    // the player whose group may loot the corpse
    if (!player)
    {
        sLog.outError("LootMgr::CreateLoot> Error cannot get looter info to create loot!");
        return;
    }

    if (!corpse)
    {
        sLog.outError("Loot::CreateLoot> cannot create corpse loot, no corpse passed!");
        return;
    }

    m_lootTarget = corpse;
    m_guidTarget = corpse->GetObjectGuid();

    if (type != LOOT_INSIGNIA || corpse->GetType() == CORPSE_BONES)
        return;

    if (!corpse->lootForBody)
    {
        corpse->lootForBody = true;
        uint32 pLevel = 0;
        if (Player* plr = sObjectAccessor.FindPlayer(corpse->GetOwnerGuid()))
            pLevel = plr->getLevel();
        else
            pLevel = player->getLevel(); // TODO:: not correct, need to save real player level in the corpse data in case of logout
        clear();
        if (player->GetBattleGround()->GetTypeID() == BATTLEGROUND_AV)
            FillLoot(0, LootTemplates_Creature, player, false);
        // It may need a better formula
        // Now it works like this: lvl10: ~6copper, lvl70: ~9silver
        gold = (uint32)(urand(50, 150) * 0.016f * pow(((float)pLevel) / 5.76f, 2.5f) * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));
    }
    ownerSet.insert(player->GetObjectGuid());
    lootMethod = NOT_GROUP_TYPE_LOOT;
    return;
}

Loot::Loot(Player* player, Item* item, LootType type) :
    lootType(LOOT_NONE), lootMethod(NOT_GROUP_TYPE_LOOT), threshold(ITEM_QUALITY_UNCOMMON),
    gold(0), unlootedCount(0), maxEnchantSkill(0), isReleased(false), haveItemOverThreshold(false),
    isChecked(false), m_lootTarget(NULL)
{
    // the player whose group may loot the corpse
    if (!player)
    {
        sLog.outError("LootMgr::CreateLoot> Error cannot get looter info to create loot!");
        return;
    }

    if (!item)
    {
        sLog.outError("Loot::CreateLoot> cannot create item loot, no item passed!");
        return;
    }

    m_itemTarget = item;
    m_guidTarget = item->GetObjectGuid();

    clear();
    lootType = type;

    switch (type)
    {
        case LOOT_DISENCHANTING:
            FillLoot(item->GetProto()->DisenchantID, LootTemplates_Disenchant, player, true);
            item->SetLootState(ITEM_LOOT_TEMPORARY);
            break;
        case LOOT_PROSPECTING:
            FillLoot(item->GetEntry(), LootTemplates_Prospecting, player, true);
            item->SetLootState(ITEM_LOOT_TEMPORARY);
            break;
        case LOOT_MILLING:
            FillLoot(item->GetEntry(), LootTemplates_Milling, player, true);
            item->SetLootState(ITEM_LOOT_TEMPORARY);
            break;
        default:
            FillLoot(item->GetEntry(), LootTemplates_Item, player, true, item->GetProto()->MaxMoneyLoot == 0);
            generateMoneyLoot(item->GetProto()->MinMoneyLoot, item->GetProto()->MaxMoneyLoot);
            item->SetLootState(ITEM_LOOT_CHANGED);
            break;
    }
    ownerSet.insert(player->GetObjectGuid());
    lootMethod = NOT_GROUP_TYPE_LOOT;
    return;
}

Loot::Loot(Unit* unit, Item* item) :
    lootType(LOOT_NONE), lootMethod(NOT_GROUP_TYPE_LOOT), threshold(ITEM_QUALITY_UNCOMMON),
    gold(0), unlootedCount(0), maxEnchantSkill(0), isReleased(false), haveItemOverThreshold(false),
    isChecked(false), m_lootTarget(NULL), m_itemTarget(item)
{
    ownerSet.insert(unit->GetObjectGuid());
    m_guidTarget = item->GetObjectGuid();
}

void Loot::SendAllowedLooter()
{
    if (lootMethod == FREE_FOR_ALL || lootMethod == NOT_GROUP_TYPE_LOOT)
        return;

    WorldPacket data(SMSG_LOOT_LIST);
    data << GetLootGuid();

    if (lootMethod == MASTER_LOOT)
        data << masterOwnerGuid.WriteAsPacked();
    else
        data << uint8(0);

    data << currentLooterGuid.WriteAsPacked();

    for (GuidSet::const_iterator itr = ownerSet.begin(); itr != ownerSet.end(); ++itr)
        if (Player* plr = ObjectAccessor::FindPlayer(*itr))
            plr->GetSession()->SendPacket(&data);
}

InventoryResult Loot::SendItem(ObjectGuid const& targetGuid, uint32 itemSlot)
{
    return SendItem(sObjectAccessor.FindPlayer(targetGuid), itemSlot);
}

InventoryResult Loot::SendItem(Player* target, uint32 itemSlot)
{
    LootItem& lootItem = items[itemSlot];
    bool playerGotItem = false;
    InventoryResult msg;
    if (target && target->GetSession())
    {
        ItemPosCountVec dest;
        msg = target->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, lootItem.itemid, lootItem.count);
        if (msg == EQUIP_ERR_OK)
        {
            Item* newItem = target->StoreNewItem(dest, lootItem.itemid, true, lootItem.randomPropertyId);
            target->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, lootItem.itemid, lootItem.count);
            target->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE, lootType, lootItem.count);
            target->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_EPIC_ITEM, lootItem.itemid, lootItem.count);

            //lootItem.is_looted = true;
            //--unlootedCount;
            if (!lootItem.freeforall)
            {
                lootItem.is_looted = true;
                --unlootedCount;
                NotifyItemRemoved(itemSlot);
            }
            target->SendNewItem(newItem, uint32(lootItem.count), false, false, true);
            playerGotItem = true;
        }
        else
            target->SendEquipError(msg, NULL, NULL, lootItem.itemid);
    }

    if (!playerGotItem)
    {
        // an error occurred player didn't received his loot
        lootItem.is_blocked = false;                                // make the item available (was blocked since roll started)
        currentLooterGuid = target->GetObjectGuid();                // change looter guid to let only him right to loot
        isReleased = false;                                         // be sure the loot was not already released by another player
        SendAllowedLooter();                                        // update the looter right for client
        ForceLootAnimationCLientUpdate();
    }
    else
    {
        if (IsLootedForAll())
            SendReleaseForAll();
    }
    return msg;
}

bool Loot::AutoStore(Player* player, bool broadcast /*= false*/, uint32 bag /*= NULL_BAG*/, uint32 slot /*= NULL_SLOT*/)
{
    bool result = true;
    uint32 max_slot = GetMaxSlotInLootFor(player);
    for (uint32 i = 0; i < max_slot; ++i)
    {
        LootItem* lootItem = LootItemInSlot(i, player);

        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreNewItem(bag, slot, dest, lootItem->itemid, lootItem->count);
        if (msg != EQUIP_ERR_OK && slot != NULL_SLOT)
            msg = player->CanStoreNewItem(bag, NULL_SLOT, dest, lootItem->itemid, lootItem->count);
        if (msg != EQUIP_ERR_OK && bag != NULL_BAG)
            msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, lootItem->itemid, lootItem->count);
        if (msg != EQUIP_ERR_OK)
        {
            player->SendEquipError(msg, NULL, NULL, lootItem->itemid);
            result = false;
            continue;
        }

        Item* pItem = player->StoreNewItem(dest, lootItem->itemid, true, lootItem->randomPropertyId);
        player->SendNewItem(pItem, lootItem->count, false, false, broadcast);
    }
    return result;
}

void Loot::Update()
{
    GroupLootRollMap::iterator itr = roll.begin();
    while (itr != roll.end())
    {
        if (itr->second.UpdateRoll())
            roll.erase(itr++);
        else
            ++itr;
    }
}

uint32 Loot::GetMaxSlotInLootFor(Player* player) const
{
    QuestItemMap::const_iterator itr = m_playerQuestItems.find(player->GetGUIDLow());
    return items.size() + (itr != m_playerQuestItems.end() ?  itr->second->size() : 0);
}

void Loot::ForceLootAnimationCLientUpdate()
{
    if (m_lootTarget)
        m_lootTarget->ForceValuesUpdateAtIndex(UNIT_DYNAMIC_FLAGS);
}

ByteBuffer& operator<<(ByteBuffer& b, LootItem const& li)
{
    b << uint32(li.itemid);
    b << uint32(li.count);                                  // nr of items of this type
    b << uint32(ObjectMgr::GetItemPrototype(li.itemid)->DisplayInfoID);
    b << uint32(li.randomSuffix);
    b << uint32(li.randomPropertyId);
    // b << uint8(0);                                       // slot type - will send after this function call
    return b;
}

ByteBuffer& operator<<(ByteBuffer& b, LootView const& lv)
{
    Loot const& l = lv.loot;

    uint8 itemsShown = 0;

    // gold
    b << uint32(l.gold);

    size_t count_pos = b.wpos();                            // pos of item count byte
    b << uint8(0);                                          // item count placeholder

    for (uint8 i = 0; i < l.items.size(); ++i)
    {
        LootSlotType slot_type = l.items[i].GetSlotTypeForSharedLoot(lv);
        if (slot_type >= MAX_LOOT_SLOT_TYPE)
            continue;

        b << uint8(i) << l.items[i];
        b << uint8(slot_type);                              // 0 - get 1 - look only 2 - master selection
        ++itemsShown;
    }

    QuestItemMap const& lootPlayerNonQuestNonFFAConditionalItems = l.GetPlayerNonQuestNonFFAConditionalItems();
    QuestItemMap::const_iterator nn_itr = lootPlayerNonQuestNonFFAConditionalItems.find(lv.viewer->GetGUIDLow());
    if (nn_itr != lootPlayerNonQuestNonFFAConditionalItems.end())
    {
        QuestItemList* conditional_list =  nn_itr->second;
        for (QuestItemList::const_iterator ci = conditional_list->begin() ; ci != conditional_list->end(); ++ci)
        {
            LootItem const& item = l.items[ci->index];

            LootSlotType slot_type = item.GetSlotTypeForSharedLoot(lv, !ci->is_looted);
            if (slot_type >= MAX_LOOT_SLOT_TYPE)
                continue;

            b << uint8(ci->index) << item;
            b << uint8(slot_type);                          // allow loot
            ++itemsShown;
        }
    }

    // in next cases used same slot type for all items
    LootSlotType slot_type = lv.loot.lootMethod == NOT_GROUP_TYPE_LOOT ? LOOT_SLOT_OWNER : LOOT_SLOT_NORMAL;

    QuestItemMap const& lootPlayerQuestItems = l.GetPlayerQuestItems();
    QuestItemMap::const_iterator q_itr = lootPlayerQuestItems.find(lv.viewer->GetGUIDLow());
    if (q_itr != lootPlayerQuestItems.end())
    {
        QuestItemList* q_list = q_itr->second;
        for (QuestItemList::const_iterator qi = q_list->begin() ; qi != q_list->end(); ++qi)
        {
            LootItem const& item = l.m_questItems[qi->index];
            if (!qi->is_looted && !item.is_looted)
            {
                b << uint8(l.items.size() + (qi - q_list->begin()));
                b << item;
                b << uint8(slot_type);                      // allow loot
                ++itemsShown;
            }
        }
    }

    QuestItemMap const& lootPlayerFFAItems = l.GetPlayerFFAItems();
    QuestItemMap::const_iterator ffa_itr = lootPlayerFFAItems.find(lv.viewer->GetGUIDLow());
    if (ffa_itr != lootPlayerFFAItems.end())
    {
        QuestItemList* ffa_list = ffa_itr->second;
        for (QuestItemList::const_iterator fi = ffa_list->begin() ; fi != ffa_list->end(); ++fi)
        {
            LootItem const& item = l.items[fi->index];
            if (!fi->is_looted && !item.is_looted)
            {
                b << uint8(fi->index) << item;
                b << uint8(slot_type);                      // allow loot
                ++itemsShown;
            }
        }
    }

    // update number of items shown
    b.put<uint8>(count_pos, itemsShown);

    return b;
}

//
// --------- LootTemplate::LootGroup ---------
//

// Adds an entry to the group (at loading stage)
void LootTemplate::LootGroup::AddEntry(LootStoreItem& item)
{
    if (item.chance != 0)
        ExplicitlyChanced.push_back(item);
    else
        EqualChanced.push_back(item);
}

// Rolls an item from the group, returns nullptr if all miss their chances
LootStoreItem const* LootTemplate::LootGroup::Roll() const
{
    if (!ExplicitlyChanced.empty())                         // First explicitly chanced entries are checked
    {
        float Roll = rand_chance_f();

        for (uint32 i = 0; i < ExplicitlyChanced.size(); ++i) // check each explicitly chanced entry in the template and modify its chance based on quality.
        {
            if (ExplicitlyChanced[i].chance >= 100.0f)
                return &ExplicitlyChanced[i];

            Roll -= ExplicitlyChanced[i].chance;
            if (Roll < 0)
                return &ExplicitlyChanced[i];
        }
    }
    if (!EqualChanced.empty())                              // If nothing selected yet - an item is taken from equal-chanced part
        return &EqualChanced[irand(0, EqualChanced.size() - 1)];

    return nullptr;                                            // Empty drop from the group
}

// True if group includes at least 1 quest drop entry
bool LootTemplate::LootGroup::HasQuestDrop() const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (i->needs_quest)
            return true;
    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if (i->needs_quest)
            return true;
    return false;
}

// True if group includes at least 1 quest drop entry for active quests of the player
bool LootTemplate::LootGroup::HasQuestDropForPlayer(Player const* player) const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (player->HasQuestForItem(i->itemid))
            return true;
    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if (player->HasQuestForItem(i->itemid))
            return true;
    return false;
}

// Rolls an item from the group (if any takes its chance) and adds the item to the loot
void LootTemplate::LootGroup::Process(Loot& loot) const
{
    LootStoreItem const* item = Roll();
    if (item != nullptr)
        loot.AddItem(*item);
}

// Overall chance for the group without equal chanced items
float LootTemplate::LootGroup::RawTotalChance() const
{
    float result = 0;

    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (!i->needs_quest)
            result += i->chance;

    return result;
}

// Overall chance for the group
float LootTemplate::LootGroup::TotalChance() const
{
    float result = RawTotalChance();

    if (!EqualChanced.empty() && result < 100.0f)
        return 100.0f;

    return result;
}

void LootTemplate::LootGroup::Verify(LootStore const& lootstore, uint32 id, uint32 group_id) const
{
    float chance = RawTotalChance();
    if (chance > 101.0f)                                    // TODO: replace with 100% when DBs will be ready
    {
        sLog.outErrorDb("Table '%s' entry %u group %d has total chance > 100%% (%f)", lootstore.GetName(), id, group_id, chance);
    }

    if (chance >= 100.0f && !EqualChanced.empty())
    {
        sLog.outErrorDb("Table '%s' entry %u group %d has items with chance=0%% but group total chance >= 100%% (%f)", lootstore.GetName(), id, group_id, chance);
    }
}

void LootTemplate::LootGroup::CheckLootRefs(LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = ExplicitlyChanced.begin(); ieItr != ExplicitlyChanced.end(); ++ieItr)
    {
        if (ieItr->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-ieItr->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-ieItr->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-ieItr->mincountOrRef);
        }
    }

    for (LootStoreItemList::const_iterator ieItr = EqualChanced.begin(); ieItr != EqualChanced.end(); ++ieItr)
    {
        if (ieItr->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-ieItr->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-ieItr->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-ieItr->mincountOrRef);
        }
    }
}

//
// --------- LootTemplate ---------
//

// Adds an entry to the group (at loading stage)
void LootTemplate::AddEntry(LootStoreItem& item)
{
    if (item.group > 0 && item.mincountOrRef > 0)           // Group
    {
        if (item.group >= Groups.size())
            Groups.resize(item.group);                      // Adds new group the the loot template if needed
        Groups[item.group - 1].AddEntry(item);              // Adds new entry to the group
    }
    else                                                    // Non-grouped entries and references are stored together
        Entries.push_back(item);
}

// Rolls for every item in the template and adds the rolled items the the loot
void LootTemplate::Process(Loot& loot, LootStore const& store, bool rate, uint8 groupId) const
{
    if (groupId)                                            // Group reference uses own processing of the group
    {
        if (groupId > Groups.size())
            return;                                         // Error message already printed at loading stage

        Groups[groupId - 1].Process(loot);
        return;
    }

    // Rolling non-grouped items
    for (LootStoreItemList::const_iterator i = Entries.begin() ; i != Entries.end() ; ++i)
    {
        if (!i->Roll(rate))
            continue;                                       // Bad luck for the entry

        if (i->mincountOrRef < 0)                           // References processing
        {
            LootTemplate const* Referenced = LootTemplates_Reference.GetLootFor(-i->mincountOrRef);

            if (!Referenced)
                continue;                                   // Error message already printed at loading stage

            // Check condition
            if (i->conditionId && !sObjectMgr.IsPlayerMeetToCondition(i->conditionId, nullptr, nullptr, loot.GetLootTarget(), CONDITION_FROM_REFERING_LOOT))
                continue;

            for (uint32 loop = 0; loop < i->maxcount; ++loop) // Ref multiplicator
                Referenced->Process(loot, store, rate, i->group);
        }
        else                                                // Plain entries (not a reference, not grouped)
            loot.AddItem(*i);                               // Chance is already checked, just add
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin() ; i != Groups.end() ; ++i)
        i->Process(loot);
}

// True if template includes at least 1 quest drop entry
bool LootTemplate::HasQuestDrop(LootTemplateMap const& store, uint8 groupId) const
{
    if (groupId)                                            // Group reference
    {
        if (groupId > Groups.size())
            return false;                                   // Error message [should be] already printed at loading stage
        return Groups[groupId - 1].HasQuestDrop();
    }

    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        if (i->mincountOrRef < 0)                           // References
        {
            LootTemplateMap::const_iterator Referenced = store.find(-i->mincountOrRef);
            if (Referenced == store.end())
                continue;                                   // Error message [should be] already printed at loading stage
            if (Referenced->second->HasQuestDrop(store, i->group))
                return true;
        }
        else if (i->needs_quest)
            return true;                                    // quest drop found
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin() ; i != Groups.end() ; ++i)
        if (i->HasQuestDrop())
            return true;

    return false;
}

// True if template includes at least 1 quest drop for an active quest of the player
bool LootTemplate::HasQuestDropForPlayer(LootTemplateMap const& store, Player const* player, uint8 groupId) const
{
    if (groupId)                                            // Group reference
    {
        if (groupId > Groups.size())
            return false;                                   // Error message already printed at loading stage
        return Groups[groupId - 1].HasQuestDropForPlayer(player);
    }

    // Checking non-grouped entries
    for (LootStoreItemList::const_iterator i = Entries.begin() ; i != Entries.end() ; ++i)
    {
        if (i->mincountOrRef < 0)                           // References processing
        {
            LootTemplateMap::const_iterator Referenced = store.find(-i->mincountOrRef);
            if (Referenced == store.end())
                continue;                                   // Error message already printed at loading stage
            if (Referenced->second->HasQuestDropForPlayer(store, player, i->group))
                return true;
        }
        else if (player->HasQuestForItem(i->itemid))
            return true;                                    // active quest drop found
    }

    // Now checking groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (i->HasQuestDropForPlayer(player))
            return true;

    return false;
}

// Checks integrity of the template
void LootTemplate::Verify(LootStore const& lootstore, uint32 id) const
{
    // Checking group chances
    for (uint32 i = 0; i < Groups.size(); ++i)
        Groups[i].Verify(lootstore, id, i + 1);

    // TODO: References validity checks
}

void LootTemplate::CheckLootRefs(LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = Entries.begin(); ieItr != Entries.end(); ++ieItr)
    {
        if (ieItr->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-ieItr->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-ieItr->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-ieItr->mincountOrRef);
        }
    }

    for (LootGroups::const_iterator grItr = Groups.begin(); grItr != Groups.end(); ++grItr)
        grItr->CheckLootRefs(ref_set);
}

void LoadLootTemplates_Creature()
{
    LootIdSet ids_set, ids_setUsed;
    LootTemplates_Creature.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
        {
            if (uint32 lootid = cInfo->LootId)
            {
                if (ids_set.find(lootid) == ids_set.end())
                    LootTemplates_Creature.ReportNotExistedId(lootid);
                else
                    ids_setUsed.insert(lootid);
            }
        }
    }
    for (LootIdSet::const_iterator itr = ids_setUsed.begin(); itr != ids_setUsed.end(); ++itr)
        ids_set.erase(*itr);

    // for alterac valley we've defined Player-loot inside creature_loot_template id=0
    // this hack is used, so that we won't need to create an extra table player_loot_template for just one case
    ids_set.erase(0);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Creature.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Disenchant()
{
    LootIdSet ids_set, ids_setUsed;
    LootTemplates_Disenchant.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        if (ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i))
        {
            if (uint32 lootid = proto->DisenchantID)
            {
                if (ids_set.find(lootid) == ids_set.end())
                    LootTemplates_Disenchant.ReportNotExistedId(lootid);
                else
                    ids_setUsed.insert(lootid);
            }
        }
    }
    for (LootIdSet::const_iterator itr = ids_setUsed.begin(); itr != ids_setUsed.end(); ++itr)
        ids_set.erase(*itr);
    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Disenchant.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Fishing()
{
    LootIdSet ids_set;
    LootTemplates_Fishing.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sAreaStore.GetNumRows(); ++i)
    {
        if (AreaTableEntry const* areaEntry = sAreaStore.LookupEntry(i))
            if (ids_set.find(areaEntry->ID) != ids_set.end())
                ids_set.erase(areaEntry->ID);
    }

    // by default (look config options) fishing at fail provide junk loot, entry 0 use for store this loot
    ids_set.erase(0);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Fishing.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Gameobject()
{
    LootIdSet ids_set, ids_setUsed;
    LootTemplates_Gameobject.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (SQLStorageBase::SQLSIterator<GameObjectInfo> itr = sGOStorage.getDataBegin<GameObjectInfo>(); itr < sGOStorage.getDataEnd<GameObjectInfo>(); ++itr)
    {
        if (uint32 lootid = itr->GetLootId())
        {
            if (ids_set.find(lootid) == ids_set.end())
                LootTemplates_Gameobject.ReportNotExistedId(lootid);
            else
                ids_setUsed.insert(lootid);
        }
    }
    for (LootIdSet::const_iterator itr = ids_setUsed.begin(); itr != ids_setUsed.end(); ++itr)
        ids_set.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Gameobject.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Item()
{
    LootIdSet ids_set;
    LootTemplates_Item.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        if (ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i))
        {
            if (!(proto->Flags & ITEM_FLAG_LOOTABLE))
                continue;

            if (ids_set.find(proto->ItemId) != ids_set.end() || proto->MaxMoneyLoot > 0)
                ids_set.erase(proto->ItemId);
            // wdb have wrong data cases, so skip by default
            else if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
                LootTemplates_Item.ReportNotExistedId(proto->ItemId);
        }
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Item.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Milling()
{
    LootIdSet ids_set;
    LootTemplates_Milling.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i);
        if (!proto)
            continue;

        if (!(proto->Flags & ITEM_FLAG_MILLABLE))
            continue;

        if (ids_set.find(proto->ItemId) != ids_set.end())
            ids_set.erase(proto->ItemId);
        else
            LootTemplates_Milling.ReportNotExistedId(proto->ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Milling.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Pickpocketing()
{
    LootIdSet ids_set, ids_setUsed;
    LootTemplates_Pickpocketing.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
        {
            if (uint32 lootid = cInfo->PickpocketLootId)
            {
                if (ids_set.find(lootid) == ids_set.end())
                    LootTemplates_Pickpocketing.ReportNotExistedId(lootid);
                else
                    ids_setUsed.insert(lootid);
            }
        }
    }
    for (LootIdSet::const_iterator itr = ids_setUsed.begin(); itr != ids_setUsed.end(); ++itr)
        ids_set.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Pickpocketing.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Prospecting()
{
    LootIdSet ids_set;
    LootTemplates_Prospecting.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i);
        if (!proto)
            continue;

        if (!(proto->Flags & ITEM_FLAG_PROSPECTABLE))
            continue;

        if (ids_set.find(proto->ItemId) != ids_set.end())
            ids_set.erase(proto->ItemId);
        // else -- exist some cases that possible can be prospected but not expected have any result loot
        //    LootTemplates_Prospecting.ReportNotExistedId(proto->ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Prospecting.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Mail()
{
    LootIdSet ids_set;
    LootTemplates_Mail.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sMailTemplateStore.GetNumRows(); ++i)
        if (sMailTemplateStore.LookupEntry(i))
            if (ids_set.find(i) != ids_set.end())
                ids_set.erase(i);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Mail.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Skinning()
{
    LootIdSet ids_set, ids_setUsed;
    LootTemplates_Skinning.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
        {
            if (uint32 lootid = cInfo->SkinningLootId)
            {
                if (ids_set.find(lootid) == ids_set.end())
                    LootTemplates_Skinning.ReportNotExistedId(lootid);
                else
                    ids_setUsed.insert(lootid);
            }
        }
    }
    for (LootIdSet::const_iterator itr = ids_setUsed.begin(); itr != ids_setUsed.end(); ++itr)
        ids_set.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Skinning.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Spell()
{
    LootIdSet ids_set;
    LootTemplates_Spell.LoadAndCollectLootIds(ids_set);

    // remove real entries and check existence loot
    for (uint32 spell_id = 1; spell_id < sSpellStore.GetNumRows(); ++spell_id)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);
        if (!spellInfo)
            continue;

        // possible cases
        if (!IsLootCraftingSpell(spellInfo))
            continue;

        if (ids_set.find(spell_id) == ids_set.end())
        {
            // not report about not trainable spells (optionally supported by DB)
            // ignore 61756 (Northrend Inscription Research (FAST QA VERSION) for example
            if (!spellInfo->HasAttribute(SPELL_ATTR_NOT_SHAPESHIFT) || spellInfo->HasAttribute(SPELL_ATTR_TRADESPELL))
            {
                LootTemplates_Spell.ReportNotExistedId(spell_id);
            }
        }
        else
            ids_set.erase(spell_id);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Spell.ReportUnusedIds(ids_set);
}

void LoadLootTemplates_Reference()
{
    LootIdSet ids_set;
    LootTemplates_Reference.LoadAndCollectLootIds(ids_set);

    // check references and remove used
    LootTemplates_Creature.CheckLootRefs(&ids_set);
    LootTemplates_Fishing.CheckLootRefs(&ids_set);
    LootTemplates_Gameobject.CheckLootRefs(&ids_set);
    LootTemplates_Item.CheckLootRefs(&ids_set);
    LootTemplates_Milling.CheckLootRefs(&ids_set);
    LootTemplates_Pickpocketing.CheckLootRefs(&ids_set);
    LootTemplates_Skinning.CheckLootRefs(&ids_set);
    LootTemplates_Disenchant.CheckLootRefs(&ids_set);
    LootTemplates_Prospecting.CheckLootRefs(&ids_set);
    LootTemplates_Mail.CheckLootRefs(&ids_set);
    LootTemplates_Reference.CheckLootRefs(&ids_set);
    LootTemplates_Spell.CheckLootRefs(&ids_set);

    // output error for any still listed ids (not referenced from any loot table)
    LootTemplates_Reference.ReportUnusedIds(ids_set);
}

// Vote for an ongoing roll
void LootMgr::PlayerVote(Player* player, ObjectGuid const& lootTargetGuid, uint32 itemSlot, RollVote vote)
{
    Loot* loot = GetLoot(player, lootTargetGuid);

    if (!loot)
    {
        sLog.outError("LootMgr::PlayerVote> Error cannot get loot object info!");
        return;
    }

    if (loot->roll.find(itemSlot) == loot->roll.end())
    {
        sLog.outError("LootMgr::PlayerVote> Invalid itemSlot!");
        return;
    }

    if (loot->roll[itemSlot].PlayerVote(player, vote))
    {
        switch (vote)
        {
            case ROLL_NEED:
                player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
                break;
            case ROLL_GREED:
            case ROLL_DISENCHANT:
                player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
                break;
        }
    }
}

// Get loot by object guid
// If target guid is not provided, try to find it by recipient or current player target
Loot* LootMgr::GetLoot(Player* player, ObjectGuid const& targetGuid)
{
    Loot* loot = NULL;
    ObjectGuid lguid;
    if (targetGuid.IsEmpty())
    {
        lguid = player->GetLootGuid();
        
        if (lguid.IsEmpty())
        {
            lguid = player->GetTargetGuid();
            if (lguid.IsEmpty())
                return NULL;
        }
    }
    else
        lguid = targetGuid;

    switch (lguid.GetHigh())
    {
        case HIGHGUID_GAMEOBJECT:
        {
            GameObject* gob = player->GetMap()->GetGameObject(lguid);

            // not check distance for GO in case owned GO (fishing bobber case, for example)
            if (gob)
                loot = gob->loot;

            break;
        }
        case HIGHGUID_CORPSE:                               // remove insignia ONLY in BG
        {
            Corpse* bones = player->GetMap()->GetCorpse(lguid);

            if (bones)
                loot = bones->loot;

            break;
        }
        case HIGHGUID_ITEM:
        {
            Item* item = player->GetItemByGuid(lguid);
            if (item || item->HasGeneratedLoot())
                loot = item->loot;
            break;
        }
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:
        {
            Creature* creature = player->GetMap()->GetCreature(lguid);

            if (creature)
                loot = creature->loot;

            break;
        }
        default:
            return NULL;                                         // unlootable type
    }

    return loot;
}

bool LootMgr::IsAllowedToLoot(Player* player, Creature* creature)
{
    // never tapped by any (mob solo kill)
    if (!creature->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED))
        return false;

    bool canLoot = false;
    if (Loot* loot = creature->loot)
        canLoot = loot->CanLoot(player->GetObjectGuid());

    return canLoot;
}
