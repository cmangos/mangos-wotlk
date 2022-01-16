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

#include <stdarg.h>
#include "Common.h"
#include "Log.h"
#include "WorldPacket.h"
#include "Database/DatabaseEnv.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "ProgressBar.h"

#include "../../AuctionHouse/AuctionHouseMgr.h"
#include "../../Chat/Chat.h"
#include "../../Entities/GossipDef.h"
#include "../../Entities/ItemPrototype.h"
#include "../../Entities/Player.h"
#include "../../Entities/Unit.h"
#include "../../Globals/SharedDefines.h"
#include "../../Globals/ObjectMgr.h"

#include "../../Grids/GridNotifiers.h"
#include "../../Grids/GridNotifiersImpl.h"
#include "../../Grids/CellImpl.h"
#include "../../Guilds/Guild.h"
#include "../../Guilds/GuildMgr.h"
#include "../../Loot/LootMgr.h"
#include "../../Mails/Mail.h"
#include "../../MotionGenerators/MotionMaster.h"
#include "../../Spells/Spell.h"
#include "../../Spells/SpellAuras.h"
#include "../../Spells/SpellMgr.h"
#include "../../Tools/Language.h"
#include "../../World/World.h"

#include "../AI/PlayerbotDeathKnightAI.h"
#include "../AI/PlayerbotDruidAI.h"
#include "../AI/PlayerbotHunterAI.h"
#include "../AI/PlayerbotMageAI.h"
#include "../AI/PlayerbotPaladinAI.h"
#include "../AI/PlayerbotPriestAI.h"
#include "../AI/PlayerbotRogueAI.h"
#include "../AI/PlayerbotShamanAI.h"
#include "../AI/PlayerbotWarlockAI.h"
#include "../AI/PlayerbotWarriorAI.h"

#include <iomanip>
#include <iostream>

// returns a float in range of..
float rand_float(float low, float high)
{
    return (rand() / (static_cast<float>(RAND_MAX) + 1.0)) * (high - low) + low;
}

enum NPCFlags VENDOR_MASK = (enum NPCFlags)(UNIT_NPC_FLAG_VENDOR
                            | UNIT_NPC_FLAG_VENDOR_AMMO
                            | UNIT_NPC_FLAG_VENDOR_FOOD
                            | UNIT_NPC_FLAG_VENDOR_POISON
                            | UNIT_NPC_FLAG_VENDOR_REAGENT);

// ChatHandler already implements some useful commands the master can call on bots
// These commands are protected inside the ChatHandler class so this class provides access to the commands
// we'd like to call on our bots
class PlayerbotChatHandler : protected ChatHandler
{
    public:
        explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
        bool revive(Player& botPlayer) { return HandleReviveCommand((char*) botPlayer.GetName()); }
        bool teleport(Player& botPlayer) { return HandleNamegoCommand((char*) botPlayer.GetName()); }
        void sysmessage(const char* str) { SendSysMessage(str); }
        bool dropQuest(char* str) { return HandleQuestRemoveCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr &mgr, Player* const bot, bool debugWhisper) :
    m_mgr(mgr), m_bot(bot), m_classAI(0), m_ignoreAIUpdatesUntilTime(CurrentTime()),
    m_combatOrder(ORDERS_NONE), m_ScenarioType(SCENARIO_PVE),
    m_CurrentlyCastingSpellId(0), m_CraftSpellId(0), m_spellIdCommand(0),
    m_targetGuidCommand(ObjectGuid()),
    m_taxiMaster(ObjectGuid()),
    m_ignoreNeutralizeEffect(false),
    m_bDebugCommandChat(false),
    m_debugWhisper(debugWhisper)
{
    // set bot state
    m_botState = BOTSTATE_LOADING;

    // reset some pointers
    m_targetChanged = false;
    m_targetType = TARGET_NORMAL;
    m_targetCombat = nullptr;
    m_targetAssist = nullptr;
    m_targetProtect = nullptr;

    // set collection options
    m_collectionFlags = 0;
    m_collectDist = m_mgr.m_confCollectDistance;
    if (m_mgr.m_confCollectCombat)
        SetCollectFlag(COLLECT_FLAG_COMBAT);
    if (m_mgr.m_confCollectQuest)
        SetCollectFlag(COLLECT_FLAG_QUEST);
    if (m_mgr.m_confCollectProfession)
        SetCollectFlag(COLLECT_FLAG_PROFESSION);
    if (m_mgr.m_confCollectLoot)
        SetCollectFlag(COLLECT_FLAG_LOOT);
    if (m_mgr.m_confCollectSkin && m_bot->HasSkill(SKILL_SKINNING))
        SetCollectFlag(COLLECT_FLAG_SKIN);
    if (m_mgr.m_confCollectObjects)
        SetCollectFlag(COLLECT_FLAG_NEAROBJECT);

    // set needed item list
    SetQuestNeedItems();
    SetQuestNeedCreatures();

    // start following master (will also teleport bot to master)
    m_dropWhite = false;
    m_AutoEquipToggle = false;
    m_FollowAutoGo = FOLLOWAUTOGO_OFF; //turn on bot auto follow distance can be turned off by player
    DistOverRide = 0; //set initial adjustable follow settings
    IsUpOrDown = 0;
    gTempDist = 0.5f;
    gTempDist2 = 1.0f;
    SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
    BotDataRestore();
    m_DelayAttackInit = CurrentTime();

    // get class specific ai
    ReloadAI();

    ClearActiveTalentSpec();
}

PlayerbotAI::~PlayerbotAI()
{
    if (m_classAI) delete m_classAI;
}

Player* PlayerbotAI::GetMaster() const
{
    return m_mgr.GetMaster();
}

bool PlayerbotAI::CanReachWithSpellAttack(Unit* target)
{
    bool inrange = false;
    float dist = m_bot->GetDistance(target, true, DIST_CALC_COMBAT_REACH_WITH_MELEE);

    for (SpellRanges::iterator itr = m_spellRangeMap.begin(); itr != m_spellRangeMap.end(); ++itr)
    {
        uint32 spellId = itr->first;

        // ignore positive spells
        if (IsPositiveSpell(spellId))
            continue;

        // ignore active auras
        if (target->HasAura(spellId, EFFECT_INDEX_0))
            continue;

        const SpellEntry* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!spellInfo)
            continue;

        // ignore non-ranged spells
        if (!spellInfo->HasAttribute(SPELL_ATTR_RANGED))
            continue;

        float maxrange = itr->second;

        // DEBUG_LOG("[%s] spell (%s) : dist (%f) < maxrange (%f)",m_bot->GetName(), spellInfo->SpellName[0], dist, maxrange);

        if (dist < maxrange)
        {
            inrange = true;
            break;
        }
    }
    return inrange;
}

bool PlayerbotAI::In_Reach(Unit* Target, uint32 spellId)
{
    if (!Target)
        return false;

    float range = 0;
    float dist = m_bot->GetDistance(Target, true, DIST_CALC_COMBAT_REACH_WITH_MELEE);
    SpellRanges::iterator it;
    it = m_spellRangeMap.find(spellId);
    (it != m_spellRangeMap.end()) ? range = it->second : range = 0;

    // DEBUG_LOG("spell (%u) : range (%f)", spellId, range);

    if (dist > range)
        return false;

    return true;
}

// finds spell ID for matching substring args
// in priority of full text match, spells not taking reagents, and highest rank
uint32 PlayerbotAI::getSpellId(const char* args, bool master) const
{
    if (!*args)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    int loc = 0;
    if (master)
        loc = GetMaster()->GetSession()->GetSessionDbcLocale();
    else
        loc = m_bot->GetSession()->GetSessionDbcLocale();

    uint32 foundSpellId = 0;
    bool foundExactMatch = false;
    bool foundMatchUsesNoReagents = false;

    for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!pSpellInfo)
            continue;

        const std::string name = pSpellInfo->SpellName[loc];
        if (name.empty() || !Utf8FitTo(name, wnamepart))
            continue;

        bool isExactMatch = (name.length() == wnamepart.length()) ? true : false;
        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0) ? true : false;

        // if we already found a spell
        bool useThisSpell = true;
        if (foundSpellId > 0)
        {
            if (isExactMatch && !foundExactMatch) {}
            else if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else
                useThisSpell = false;
        }
        if (useThisSpell)
        {
            foundSpellId = spellId;
            foundExactMatch = isExactMatch;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    return foundSpellId;
}

uint32 PlayerbotAI::getPetSpellId(const char* args) const
{
    if (!*args)
        return 0;

    Pet* pet = m_bot->GetPet();
    if (!pet)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

    uint32 foundSpellId = 0;
    bool foundExactMatch = false;
    bool foundMatchUsesNoReagents = false;

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!pSpellInfo)
            continue;

        const std::string name = pSpellInfo->SpellName[loc];
        if (name.empty() || !Utf8FitTo(name, wnamepart))
            continue;

        bool isExactMatch = (name.length() == wnamepart.length()) ? true : false;
        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0) ? true : false;

        // if we already found a spell
        bool useThisSpell = true;
        if (foundSpellId > 0)
        {
            if (isExactMatch && !foundExactMatch) {}
            else if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else
                useThisSpell = false;
        }
        if (useThisSpell)
        {
            foundSpellId = spellId;
            foundExactMatch = isExactMatch;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    return foundSpellId;
}

uint32 PlayerbotAI::initSpell(uint32 spellId)
{
    // Check if bot knows this spell
    if (!m_bot->HasSpell(spellId))
        return 0;

    uint32 next = 0;
    SpellChainMapNext const& nextMap = sSpellMgr.GetSpellChainNext();
    for (SpellChainMapNext::const_iterator itr = nextMap.lower_bound(spellId); itr != nextMap.upper_bound(spellId); ++itr)
    {
        SpellChainNode const* node = sSpellMgr.GetSpellChainNode(itr->second);
        // If next spell is a requirement for this one then skip it
        if (node->req == spellId)
            continue;
        if (node->prev == spellId)
        {
            next = initSpell(itr->second);
            break;
        }
    }
    if (next == 0)
    {
        const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!pSpellInfo)
            return spellId;

        DEBUG_LOG("[PlayerbotAI]: initSpell - Playerbot spell init: %s is %u", pSpellInfo->SpellName[0], spellId);

        // Add spell to spellrange map
        Spell* spell = new Spell(m_bot, pSpellInfo, false);
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange, IsPositiveSpell(spellId));
        if (Player* modOwner = m_bot->GetSpellModOwner())
            modOwner->ApplySpellMod(pSpellInfo->Id, SPELLMOD_RANGE, range);
        m_spellRangeMap.insert(std::pair<uint32, float>(spellId, range));
        delete spell;
    }
    return (next == 0) ? spellId : next;
}

// Pet spells do not form chains like player spells.
// One of the options to initialize a spell is to use spell icon id
uint32 PlayerbotAI::initPetSpell(uint32 spellIconId)
{
    Pet* pet = m_bot->GetPet();

    if (!pet)
        return 0;

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        const uint32 spellId = itr->first;

        if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->SpellIconID == spellIconId)
            return spellId;
    }

    // Nothing found
    return 0;
}

/*
* Send list of the equipment in bot's inventory not currently equipped.
* This is called when the master is inspecting the bot.
*/
void PlayerbotAI::SendNotEquipList(Player& /*player*/)
{
    // find all unequipped items and put them in
    // a vector of dynamically created lists where the vector index is from 0-18
    // and the list contains Item* that can be equipped to that slot
    // Note: each dynamically created list in the vector must be deleted at end
    // so NO EARLY RETURNS!
    // see enum EquipmentSlots in Player.h to see what equipment slot each index in vector
    // is assigned to. (The first is EQUIPMENT_SLOT_HEAD=0, and last is EQUIPMENT_SLOT_TABARD=18)
    std::list<Item*>* equip[19];
    for (uint8 i = 0; i < 19; ++i)
        equip[i] = nullptr;

    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        uint16 dest;
        InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
        if (msg != EQUIP_ERR_OK)
            continue;

        // the dest looks like it includes the old loc in the 8 higher bits
        // so casting it to a uint8 strips them
        int8 equipSlot = uint8(dest);
        if (!(equipSlot >= 0 && equipSlot < 19))
            continue;

        // create a list if one doesn't already exist
        if (equip[equipSlot] == nullptr)
            equip[equipSlot] = new std::list<Item*>;

        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        itemListForEqSlot->push_back(pItem);
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 dest;
                InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;

                int8 equipSlot = uint8(dest);
                if (!(equipSlot >= 0 && equipSlot < 19))
                    continue;

                // create a list if one doesn't already exist
                if (equip[equipSlot] == nullptr)
                    equip[equipSlot] = new std::list<Item*>;

                std::list<Item*>* itemListForEqSlot = equip[equipSlot];
                itemListForEqSlot->push_back(pItem);
            }
    }

    ChatHandler ch(GetMaster());
    bool bAnyEquippable = false;

    const std::string descr[] = { "head", "neck", "shoulders", "body", "chest",
                                  "waist", "legs", "feet", "wrists", "hands", "finger1", "finger2",
                                  "trinket1", "trinket2", "back", "mainhand", "offhand", "ranged",
                                  "tabard"
                                };

    // now send client all items that can be equipped by slot
    for (uint8 equipSlot = 0; equipSlot < 19; ++equipSlot)
    {
        if (equip[equipSlot] == nullptr)
            continue;

        if (!bAnyEquippable)
        {
            TellMaster("Here's all the items in my inventory that I can equip:");
            bAnyEquippable = true;
        }

        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        std::ostringstream out;
        out << descr[equipSlot] << ": ";
        for (std::list<Item*>::iterator it = itemListForEqSlot->begin(); it != itemListForEqSlot->end(); ++it)
            if ((*it))
                MakeItemLink((*it), out, true);
        ch.SendSysMessage(out.str().c_str());

        delete itemListForEqSlot; // delete list of Item*
    }

    if (!bAnyEquippable)
        TellMaster("There are no items in my inventory that I can equip.");
}

void PlayerbotAI::FollowAutoReset()
{
    if (m_FollowAutoGo != FOLLOWAUTOGO_OFF)
    {
        m_FollowAutoGo = FOLLOWAUTOGO_RESET;
        SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
        WorldObject* distTarget = m_followTarget;
        for (uint8 i = 0; i < 1; ++i)
        {
            if (m_bot->GetDistance(distTarget) < 3.0f)
                i = 1;
            else
                i = 0;
        }
    }
}

void PlayerbotAI::AutoUpgradeEquipment() // test for autoequip
{
    if (!m_AutoEquipToggle)
        return;

    ChatHandler ch(GetMaster());
    std::ostringstream out;
    std::ostringstream msg;

    // Find equippable items in main backpack one at a time
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

        if (!pItem)
            continue;

        uint32 spellId = 0;

        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (pItem->GetProto()->Spells[i].SpellId > 0)
            {
                spellId = pItem->GetProto()->Spells[i].SpellId;
                break;
            }
        }

        if (pItem->GetProto()->Flags & ITEM_FLAG_HAS_LOOT && spellId == 0)
        {
            // TODO: std::string oops = "Opening " + [ITEM] + " to see what's inside.";
            std::string oops = "Oh... Look! Theres something inside!!!";
            m_bot->Say(oops, LANG_UNIVERSAL);
            UseItem(pItem);
            continue;
        }

        if (uint32 questid = pItem->GetProto()->StartQuest)
        {
            // TODO: if quest failed, auto-drop and auto-pickup
            //if (m_bot->GetQuestStatus(questid) == QUEST_STATUS_FAILED)
            if (m_bot->GetQuestStatus(questid) == QUEST_STATUS_COMPLETE
                    || m_bot->GetQuestStatus(questid) == QUEST_STATUS_UNAVAILABLE)
                continue;
            else
            {
                Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
                if (!m_bot->CanTakeQuest(qInfo, false))
                {
                    // TODO: make clear which item should be thrown away
                    std::string oops = "Great, more junk... Can I get rid of this please?";
                    m_bot->Say(oops, LANG_UNIVERSAL);
                    continue;
                }
            }
            UseItem(pItem);
        }

        uint16 equipSlot;
        InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, equipSlot, pItem, !pItem->IsBag());
        if (msg != EQUIP_ERR_OK)
            continue;

        // do we have anything equipped of this type?
        Item* const pItem2 = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, (uint8)equipSlot);
        if (!pItem2) // no item to compare to see if has stats useful for this bots class/style so check for stats and equip if possible
        {
            ItemPrototype const* pProto2 = pItem->GetProto();
            if (pProto2->StatsCount > 0)
            {
                if (!ItemStatComparison(pProto2, pProto2))
                    continue;
            }
            EquipItem(pItem); // no item equipped so equip new one and go to next item.
            continue;
        }

        // we have an equippable item, so lets send it to the comparison function to see if it's better than we have on
        AutoEquipComparison(pItem, pItem2); // pItem is new item, pItem2 is equipped item.
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 dest;
                InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;

                uint8 equipSlot = uint8(dest);
                Item* const pItem2 = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, equipSlot); // do we have anything equipped of this type?
                if (!pItem2)
                {
                    ItemPrototype const* pProto2 = pItem->GetProto();
                    if (pProto2->StatsCount > 0)
                    {
                        if (!ItemStatComparison(pProto2, pProto2))
                            continue;
                    }

                    EquipItem(pItem); //no item equipped so equip new one if useable stats and go to next item.
                    continue;
                }

                // we have an equippable item but something else is equipped so lets send it to the comparison function to see if it's better than we have on
                AutoEquipComparison(pItem, pItem2); //pItem is new item, pItem2 is equipped item.
            }
    }
    InspectUpdate();
}

void PlayerbotAI::AutoEquipComparison(Item* pItem, Item* pItem2)
{
    const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
    };
    ItemPrototype const* pProto = pItem2->GetProto(); // equipped item if any
    ItemPrototype const* pProto2 = pItem->GetProto(); // new item to compare
    // DEBUG_LOG("Item Class (%s)",(pProto->Class == ITEM_CLASS_WEAPON ? "Weapon" : "Not Weapon"));
    switch (pProto->Class)
    {
        case ITEM_CLASS_WEAPON:
        {
            // DEBUG_LOG("Current Item DPS (%f) Equippable Item DPS (%f)",pProto->getDPS(),pProto2->getDPS());
            // m_bot->GetSkillValue(pProto->RequiredSkill) < m_bot->GetSkillValue(pProto2->RequiredSkill)
            if (pProto->getDPS() < pProto2->getDPS())   // if new item has a better DPS
            {
                EquipItem(pItem);
                pProto = pProto2; // ensure that the item with the highest DPS is equipped
            }
            break;
        }
        case ITEM_CLASS_ARMOR:
        {
            // now in case they are same itemlevel, but one is better than the other..
            if (pProto->ItemLevel == pProto2->ItemLevel && pProto->Quality < pProto2->Quality && pProto->Armor <= pProto2->Armor &&
                    m_bot->HasSkill(item_armor_skills[pProto2->SubClass]) && !m_bot->HasSkill(item_armor_skills[pProto2->SubClass + 1])) // itemlevel + armour + armour class
            {
                // First check to see if this item has stats, and if the bot REALLY wants to lose its old item
                if (pProto2->StatsCount > 0)
                {
                    if (!ItemStatComparison(pProto, pProto2))
                        return; // stats on equipped item are better, OR stats are not useful for this bots class/style
                }
                EquipItem(pItem);
                break;
            }
            if (pProto->ItemLevel <= pProto2->ItemLevel && pProto->Quality < pProto2->Quality && pProto->Armor > pProto2->Armor &&
                    m_bot->HasSkill(item_armor_skills[pProto2->SubClass]) && !m_bot->HasSkill(item_armor_skills[pProto2->SubClass + 1])) // itemlevel + armour + armour class
            {
                // First check to see if this item has stats, and if the bot REALLY wants to lose its old item
                if (pProto2->StatsCount > 0)
                {
                    if (!ItemStatComparison(pProto, pProto2))
                        return; // stats on equipped item are better, OR stats are not useful for this bots class/style
                }
                EquipItem(pItem);
                break;
            }
            if (pProto->ItemLevel <= pProto2->ItemLevel && pProto->Armor <= pProto2->Armor && m_bot->HasSkill(item_armor_skills[pProto2->SubClass]) &&
                    !m_bot->HasSkill(item_armor_skills[pProto2->SubClass + 1])) // itemlevel + armour + armour class
            {
                // First check to see if this item has stats, and if the bot REALLY wants to lose its old item
                if (pProto2->StatsCount > 0)
                {
                    if (!ItemStatComparison(pProto, pProto2))
                        return; // stats on equipped item are better, OR stats are not useful for this bots class/style
                }
                EquipItem(pItem);
                break;
            }
        }
    }
    InspectUpdate();
}

bool PlayerbotAI::ItemStatComparison(const ItemPrototype* pProto, const ItemPrototype* pProto2)
{
    uint8 isclass = 0; // 1= caster 2 = hybrid 3 = melee
    uint8 ishybrid = 0;
    uint8 olditemscore = 0;
    uint8 newitemscore = 0;
    // get class and style to make it easier to compare later
    switch (m_bot->getClass())
    {
        case CLASS_PRIEST:
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            isclass = 1;
            break;

        case CLASS_PALADIN:
        case CLASS_DEATH_KNIGHT:
        case CLASS_SHAMAN:
        case CLASS_DRUID:
            isclass = 2;
            ishybrid = 1;
            break;

        case CLASS_WARRIOR:
        case CLASS_ROGUE:
            isclass = 3; // melee
            break;

        case CLASS_HUNTER:
            isclass = 2;
            ishybrid = 2;
            break;
    }

    for (int i = 0; i < MAX_ITEM_PROTO_STATS; ++i) // item can only have 10 stats. We check each stat slot available for stat and type.
    {
        uint32 itemmod = pProto->ItemStat[i].ItemStatType; // equipped item stats if any
        uint32 itemmod2 = pProto2->ItemStat[i].ItemStatType; // newitem stats
        if (!itemmod) // if no stat type in this slot, continue to next slot
        {
            if (!itemmod2)
                continue;
        }
        // caster stats
        if (itemmod == ITEM_MOD_MANA || itemmod == ITEM_MOD_INTELLECT || itemmod == ITEM_MOD_SPIRIT || itemmod == ITEM_MOD_HIT_SPELL_RATING ||
                itemmod == ITEM_MOD_CRIT_SPELL_RATING || itemmod == ITEM_MOD_HASTE_SPELL_RATING || itemmod == ITEM_MOD_SPELL_DAMAGE_DONE ||
                itemmod == ITEM_MOD_MANA_REGENERATION || itemmod == ITEM_MOD_SPELL_POWER || itemmod == ITEM_MOD_SPELL_PENETRATION ||
                itemmod2 == ITEM_MOD_MANA || itemmod2 == ITEM_MOD_INTELLECT || itemmod2 == ITEM_MOD_SPIRIT || itemmod2 == ITEM_MOD_HIT_SPELL_RATING ||
                itemmod2 == ITEM_MOD_CRIT_SPELL_RATING || itemmod2 == ITEM_MOD_HASTE_SPELL_RATING || itemmod2 == ITEM_MOD_SPELL_DAMAGE_DONE ||
                itemmod2 == ITEM_MOD_MANA_REGENERATION || itemmod2 == ITEM_MOD_SPELL_POWER || itemmod2 == ITEM_MOD_SPELL_PENETRATION)
        {
            switch (isclass) // 1 caster, 2 hybrid, 3 melee
            {
                case 1:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                case 2:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (ishybrid != 2) //not a hunter
                    {
                        if (itemmod == itemmod2) //same stat type
                        {
                            if (pProto == pProto2) // same item
                            {
                                if (olditemscore > 0)
                                    olditemscore = (olditemscore - 1);
                                newitemscore = (newitemscore + 1);
                            }
                            if (itemmodval < itemmodval2) // which one has the most
                            {
                                if (olditemscore > 0)
                                    olditemscore = (olditemscore - 1);
                                newitemscore = (newitemscore + 1);
                            }
                            else if (itemmodval > itemmodval2)
                            {
                                if (newitemscore > 0)
                                    newitemscore = (newitemscore - 1);
                                olditemscore = (olditemscore + 1);
                            }
                        }
                        else
                        {
                            if (itemmod > 0)
                                olditemscore = (olditemscore + 1);
                            if (itemmod2 > 0)
                                newitemscore = (newitemscore + 1);
                        }
                    }
                    else //is a hunter
                    {
                        if (itemmod > 0)
                        {
                            if (olditemscore > 0) //we dont want any negative returns
                                olditemscore = (olditemscore - 1);
                        }
                        if (itemmod2 > 0)
                        {
                            if (newitemscore > 0) //we dont want any negative returns
                                newitemscore = (newitemscore - 1);
                        }
                    }
                    break;
                }  // pure melee need nothing from this list.
                case 3:
                {
                    if (itemmod > 0)
                    {
                        if (olditemscore > 0) //we dont want any negative returns
                            olditemscore = (olditemscore - 1);
                    }
                    if (itemmod2 > 0)
                    {
                        if (newitemscore > 0) //we dont want any negative returns
                            newitemscore = (newitemscore - 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        // melee only stats (warrior/rogue) or stats that only apply to melee style combat
        if (itemmod == ITEM_MOD_HEALTH || itemmod == ITEM_MOD_AGILITY || itemmod == ITEM_MOD_STRENGTH ||
                itemmod == ITEM_MOD_DEFENSE_SKILL_RATING || itemmod == ITEM_MOD_DODGE_RATING || itemmod == ITEM_MOD_PARRY_RATING ||
                itemmod == ITEM_MOD_BLOCK_RATING || itemmod == ITEM_MOD_HIT_MELEE_RATING || itemmod == ITEM_MOD_CRIT_MELEE_RATING ||
                itemmod == ITEM_MOD_HIT_TAKEN_MELEE_RATING || itemmod == ITEM_MOD_HIT_TAKEN_RANGED_RATING || itemmod == ITEM_MOD_HIT_TAKEN_SPELL_RATING ||
                itemmod == ITEM_MOD_CRIT_TAKEN_MELEE_RATING || itemmod == ITEM_MOD_CRIT_TAKEN_RANGED_RATING ||
                itemmod == ITEM_MOD_CRIT_TAKEN_SPELL_RATING || itemmod == ITEM_MOD_HASTE_MELEE_RATING ||
                itemmod == ITEM_MOD_HIT_TAKEN_RATING || itemmod == ITEM_MOD_CRIT_TAKEN_RATING || itemmod == ITEM_MOD_ATTACK_POWER ||
                itemmod == ITEM_MOD_BLOCK_VALUE || itemmod2 == ITEM_MOD_HEALTH || itemmod2 == ITEM_MOD_AGILITY || itemmod2 == ITEM_MOD_STRENGTH ||
                itemmod2 == ITEM_MOD_DEFENSE_SKILL_RATING || itemmod2 == ITEM_MOD_DODGE_RATING || itemmod2 == ITEM_MOD_PARRY_RATING ||
                itemmod2 == ITEM_MOD_BLOCK_RATING || itemmod2 == ITEM_MOD_HIT_MELEE_RATING || itemmod2 == ITEM_MOD_CRIT_MELEE_RATING ||
                itemmod2 == ITEM_MOD_HIT_TAKEN_MELEE_RATING || itemmod2 == ITEM_MOD_HIT_TAKEN_RANGED_RATING || itemmod2 == ITEM_MOD_HIT_TAKEN_SPELL_RATING ||
                itemmod2 == ITEM_MOD_CRIT_TAKEN_MELEE_RATING || itemmod2 == ITEM_MOD_CRIT_TAKEN_RANGED_RATING ||
                itemmod2 == ITEM_MOD_CRIT_TAKEN_SPELL_RATING || itemmod2 == ITEM_MOD_HASTE_MELEE_RATING ||
                itemmod2 == ITEM_MOD_HIT_TAKEN_RATING || itemmod2 == ITEM_MOD_CRIT_TAKEN_RATING || itemmod2 == ITEM_MOD_ATTACK_POWER ||
                itemmod2 == ITEM_MOD_BLOCK_VALUE)
        {
            switch (isclass) // 1 caster, 2 hybrid, 3 melee
            {
                case 1:
                {
                    if (itemmod > 0)
                    {
                        if (olditemscore > 0) //we dont want any negative returns
                            olditemscore = (olditemscore - 1);
                    }
                    if (itemmod2 > 0)
                    {
                        if (newitemscore > 0) //we dont want any negative returns
                            newitemscore = (newitemscore - 1);
                    }
                    break;
                }
                case 2:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                case 3:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        // stats which aren't strictly caster or melee (hybrid perhaps or style dependant)
        if (itemmod == ITEM_MOD_HIT_RATING || itemmod == ITEM_MOD_CRIT_RATING ||
                itemmod == ITEM_MOD_RESILIENCE_RATING || itemmod == ITEM_MOD_HASTE_RATING || itemmod == ITEM_MOD_EXPERTISE_RATING ||
                itemmod == ITEM_MOD_ARMOR_PENETRATION_RATING || itemmod == ITEM_MOD_HEALTH_REGEN || itemmod == ITEM_MOD_STAMINA ||
                itemmod2 == ITEM_MOD_HIT_RATING || itemmod2 == ITEM_MOD_CRIT_RATING || itemmod2 == ITEM_MOD_RESILIENCE_RATING ||
                itemmod2 == ITEM_MOD_HASTE_RATING || itemmod2 == ITEM_MOD_EXPERTISE_RATING || itemmod2 == ITEM_MOD_ARMOR_PENETRATION_RATING ||
                itemmod2 == ITEM_MOD_HEALTH_REGEN || itemmod2 == ITEM_MOD_STAMINA)
        {
            switch (isclass) // 1 caster, 2 hybrid, 3 melee
            {
                case 1:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                case 2:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                case 3:
                {
                    uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                    uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                    if (itemmod == itemmod2) //same stat type
                    {
                        if (pProto == pProto2) // same item
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        if (itemmodval < itemmodval2) // which one has the most
                        {
                            if (olditemscore > 0)
                                olditemscore = (olditemscore - 1);
                            newitemscore = (newitemscore + 1);
                        }
                        else if (itemmodval > itemmodval2)
                        {
                            if (newitemscore > 0)
                                newitemscore = (newitemscore - 1);
                            olditemscore = (olditemscore + 1);
                        }
                    }
                    else
                    {
                        if (itemmod > 0)
                            olditemscore = (olditemscore + 1);
                        if (itemmod2 > 0)
                            newitemscore = (newitemscore + 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        // stats relating to ranged only
        if (itemmod == ITEM_MOD_HIT_RANGED_RATING || itemmod == ITEM_MOD_CRIT_RANGED_RATING || itemmod == ITEM_MOD_HASTE_RANGED_RATING ||
                itemmod == ITEM_MOD_RANGED_ATTACK_POWER || itemmod2 == ITEM_MOD_HIT_RANGED_RATING || itemmod2 == ITEM_MOD_CRIT_RANGED_RATING ||
                itemmod2 == ITEM_MOD_HASTE_RANGED_RATING || itemmod2 == ITEM_MOD_RANGED_ATTACK_POWER)
        {
            switch (isclass) // 1 caster, 2 hybrid, 3 melee
            {
                case 1:
                {
                    if (itemmod > 0)
                    {
                        if (olditemscore > 0) //we dont want any negative returns
                            olditemscore = (olditemscore - 1);
                    }
                    if (itemmod2 > 0)
                    {
                        if (newitemscore > 0) //we dont want any negative returns
                            newitemscore = (newitemscore - 1);
                    }
                    break;
                }
                case 2:
                {
                    if (ishybrid != 2) //not a hunter
                    {
                        if (itemmod > 0)
                        {
                            if (olditemscore > 0) //we dont want any negative returns
                                olditemscore = (olditemscore - 1);
                        }
                        if (itemmod2 > 0)
                        {
                            if (newitemscore > 0) //we dont want any negative returns
                                newitemscore = (newitemscore - 1);
                        }
                    }
                    else //is a hunter
                    {
                        uint32 itemmodval = pProto->ItemStat[i].ItemStatValue; // equipped item stats if any
                        uint32 itemmodval2 = pProto2->ItemStat[i].ItemStatValue;  // newitem stats
                        if (itemmod == itemmod2) //same stat type
                        {
                            if (pProto == pProto2) // same item
                            {
                                if (olditemscore > 0)
                                    olditemscore = (olditemscore - 1);
                                newitemscore = (newitemscore + 1);
                            }
                            if (itemmodval < itemmodval2) // which one has the most
                            {
                                if (olditemscore > 0)
                                    olditemscore = (olditemscore - 1);
                                newitemscore = (newitemscore + 1);
                            }
                            else if (itemmodval > itemmodval2)
                            {
                                if (newitemscore > 0)
                                    newitemscore = (newitemscore - 1);
                                olditemscore = (olditemscore + 1);
                            }
                        }
                        else
                        {
                            if (itemmod > 0)
                                olditemscore = (olditemscore + 1);
                            if (itemmod2 > 0)
                                newitemscore = (newitemscore + 1);
                        }
                    }
                    break;
                }
                case 3:
                {
                    if (itemmod > 0)
                    {
                        if (olditemscore > 0) //we dont want any negative returns
                            olditemscore = (olditemscore - 1);
                    }
                    if (itemmod2 > 0)
                    {
                        if (newitemscore > 0) //we dont want any negative returns
                            newitemscore = (newitemscore - 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    if (olditemscore <= newitemscore)
        return true;
    else
        return false;
}

void PlayerbotAI::SendQuestNeedList()
{
    std::ostringstream out;

    for (BotNeedItem::iterator itr = m_needItemList.begin(); itr != m_needItemList.end(); ++itr)
    {
        ItemPrototype const* pItemProto = sObjectMgr.GetItemPrototype(itr->first);
        if (pItemProto)
        {
            std::string itemName = pItemProto->Name1;
            ItemLocalization(itemName, pItemProto->ItemId);

            out << " " << itr->second << "x|cffffffff|Hitem:" << pItemProto->ItemId
                << ":0:0:0:0:0:0:0" << "|h[" << itemName
                << "]|h|r";
        }
    }

    for (BotNeedItem::iterator itr = m_needCreatureOrGOList.begin(); itr != m_needCreatureOrGOList.end(); ++itr)
    {
        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(itr->first);
        if (cInfo)
        {
            std::string creatureName = cInfo->Name;
            CreatureLocalization(creatureName, cInfo->Entry);
            out << " " << itr->second << "x|cFFFFFF00|Hcreature_entry:" << itr->first << "|h[" << creatureName << "]|h|r";
        }

        if (m_bot->HasQuestForGO(itr->first))
        {
            GameObjectInfo const* gInfo = ObjectMgr::GetGameObjectInfo(itr->first);
            if (gInfo)
            {
                std::string gameobjectName = gInfo->name;
                GameObjectLocalization(gameobjectName, gInfo->id);
                out << " " << itr->second << "x|cFFFFFF00|Hgameobject_entry:" << itr->first << "|h[" << gameobjectName << "]|h|r";
            }
        }
    }

    TellMaster("Here's a list of all things needed for quests:");
    if (!out.str().empty())
        TellMaster(out.str().c_str());
}

bool PlayerbotAI::IsItemUseful(uint32 itemid)
{
    const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
        SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
        SKILL_STAVES,   0,              0,                   SKILL_UNARMED,   0,
        SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
        SKILL_FISHING
    };

    const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
    };

    ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);
    if (!pProto || pProto->Quality < ITEM_QUALITY_NORMAL)
        return false;

    // do we already have the max allowed of item if more than zero?
    if (pProto->MaxCount > 0 && m_bot->HasItemCount(itemid, pProto->MaxCount, true))
        return false;

    // quest related items
    if (pProto->StartQuest > 0 && HasCollectFlag(COLLECT_FLAG_QUEST))
        return true;

    switch (pProto->Class)
    {
        case ITEM_CLASS_WEAPON:
            if (pProto->SubClass >= MAX_ITEM_SUBCLASS_WEAPON)
                return false;
            else
                return m_bot->HasSkill(item_weapon_skills[pProto->SubClass]);
            break;
        case ITEM_CLASS_ARMOR:
            if (pProto->SubClass >= MAX_ITEM_SUBCLASS_ARMOR)
                return false;
            else
                return (m_bot->HasSkill(item_armor_skills[pProto->SubClass]) && !m_bot->HasSkill(item_armor_skills[pProto->SubClass + 1]));
            break;
        case ITEM_CLASS_QUEST:
            if (!HasCollectFlag(COLLECT_FLAG_QUEST))
                break;
        case ITEM_CLASS_KEY:
            return true;
        case ITEM_CLASS_GEM:
            if ((m_bot->HasSkill(SKILL_BLACKSMITHING) ||
                    m_bot->HasSkill(SKILL_ENGINEERING) ||
                    m_bot->HasSkill(SKILL_JEWELCRAFTING)))
                return true;
            break;
        case ITEM_CLASS_TRADE_GOODS:
            if (!HasCollectFlag(COLLECT_FLAG_PROFESSION))
                break;

            switch (pProto->SubClass)
            {
                case ITEM_SUBCLASS_PARTS:
                case ITEM_SUBCLASS_EXPLOSIVES:
                case ITEM_SUBCLASS_DEVICES:
                    if (m_bot->HasSkill(SKILL_ENGINEERING))
                        return true;
                    break;
                case ITEM_SUBCLASS_JEWELCRAFTING:
                    if (m_bot->HasSkill(SKILL_JEWELCRAFTING))
                        return true;
                    break;
                case ITEM_SUBCLASS_CLOTH:
                    if (m_bot->HasSkill(SKILL_TAILORING))
                        return true;
                    break;
                case ITEM_SUBCLASS_LEATHER:
                    if (m_bot->HasSkill(SKILL_LEATHERWORKING))
                        return true;
                    break;
                case ITEM_SUBCLASS_METAL_STONE:
                    if ((m_bot->HasSkill(SKILL_BLACKSMITHING) ||
                            m_bot->HasSkill(SKILL_ENGINEERING) ||
                            m_bot->HasSkill(SKILL_MINING)))
                        return true;
                    break;
                case ITEM_SUBCLASS_MEAT:
                    if (m_bot->HasSkill(SKILL_COOKING))
                        return true;
                    break;
                case ITEM_SUBCLASS_HERB:
                    if ((m_bot->HasSkill(SKILL_HERBALISM) ||
                            m_bot->HasSkill(SKILL_ALCHEMY) ||
                            m_bot->HasSkill(SKILL_INSCRIPTION)))
                        return true;
                    break;
                case ITEM_SUBCLASS_ELEMENTAL:
                    return true;    // pretty much every profession uses these a bit
                case ITEM_SUBCLASS_ENCHANTING:
                    if (m_bot->HasSkill(SKILL_ENCHANTING))
                        return true;
                    break;
                default:
                    break;
            }
            break;
        case ITEM_CLASS_RECIPE:
        {
            if (!HasCollectFlag(COLLECT_FLAG_PROFESSION))
                break;

            // skip recipes that we have
            if (m_bot->HasSpell(pProto->Spells[2].SpellId))
                break;

            switch (pProto->SubClass)
            {
                case ITEM_SUBCLASS_LEATHERWORKING_PATTERN:
                    if (m_bot->HasSkill(SKILL_LEATHERWORKING))
                        return true;
                    break;
                case ITEM_SUBCLASS_TAILORING_PATTERN:
                    if (m_bot->HasSkill(SKILL_TAILORING))
                        return true;
                    break;
                case ITEM_SUBCLASS_ENGINEERING_SCHEMATIC:
                    if (m_bot->HasSkill(SKILL_ENGINEERING))
                        return true;
                    break;
                case ITEM_SUBCLASS_BLACKSMITHING:
                    if (m_bot->HasSkill(SKILL_BLACKSMITHING))
                        return true;
                    break;
                case ITEM_SUBCLASS_COOKING_RECIPE:
                    if (m_bot->HasSkill(SKILL_COOKING))
                        return true;
                    break;
                case ITEM_SUBCLASS_ALCHEMY_RECIPE:
                    if (m_bot->HasSkill(SKILL_ALCHEMY))
                        return true;
                    break;
                case ITEM_SUBCLASS_FIRST_AID_MANUAL:
                    if (m_bot->HasSkill(SKILL_FIRST_AID))
                        return true;
                    break;
                case ITEM_SUBCLASS_ENCHANTING_FORMULA:
                    if (m_bot->HasSkill(SKILL_ENCHANTING))
                        return true;
                    break;
                case ITEM_SUBCLASS_FISHING_MANUAL:
                    if (m_bot->HasSkill(SKILL_FISHING))
                        return true;
                    break;
                case ITEM_SUBCLASS_JEWELCRAFTING_RECIPE:
                    if (m_bot->HasSkill(SKILL_JEWELCRAFTING))
                        return true;
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }
    return false;
}

void PlayerbotAI::ReloadAI()
{
    switch (m_bot->getClass())
    {
        case CLASS_PRIEST:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_MAGE:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_WARLOCK:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_WARRIOR:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_SHAMAN:
            if (m_classAI) delete m_classAI;
            if (m_bot->GetSpec() == SHAMAN_SPEC_ENHANCEMENT)
                m_combatStyle = COMBAT_MELEE;
            else
                m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_PALADIN:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_ROGUE:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_DRUID:
            if (m_classAI) delete m_classAI;
            if (m_bot->GetSpec() == DRUID_SPEC_FERAL)
                m_combatStyle = COMBAT_MELEE;
            else
                m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_HUNTER:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(*GetMaster(), *m_bot, *this);
            break;
        case CLASS_DEATH_KNIGHT:
            if (m_classAI) delete m_classAI;
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(*GetMaster(), *m_bot, *this);
            break;
    }

    HERB_GATHERING      = initSpell(HERB_GATHERING_1);
    MINING              = initSpell(MINING_1);
    SKINNING            = initSpell(SKINNING_1);
}

void PlayerbotAI::SendOrders(Player& /*player*/)
{
    std::ostringstream out;

    if (m_combatOrder == ORDERS_NONE)
        out << "Got no combat orders!";
    else if (m_combatOrder & ORDERS_MAIN_TANK)
        out << "I'm a MAIN TANK";
    else if (m_combatOrder & ORDERS_TANK)
        out << "I TANK";
    else if (m_combatOrder & ORDERS_ASSIST)
        out << "I ASSIST " << (m_targetAssist ? m_targetAssist->GetName() : "unknown");
    else if (m_combatOrder & ORDERS_MAIN_HEAL)
        out << "I'm a MAIN HEALER";
    else if (m_combatOrder & ORDERS_HEAL)
        out << "I HEAL";
    else if (m_combatOrder & ORDERS_NOT_MAIN_HEAL)
        out << "I HEAL but will ignore any main tank";
    else if (m_combatOrder & ORDERS_PASSIVE)
        out << "I'm PASSIVE";
    if ((m_combatOrder & ORDERS_PRIMARY) && (m_combatOrder & (ORDERS_PROTECT | ORDERS_RESIST | ORDERS_NODISPEL)))
    {
        out << " and ";
        if (m_combatOrder & ORDERS_PROTECT)
            out << "I PROTECT " << (m_targetProtect ? m_targetProtect->GetName() : "unknown");
        if (m_combatOrder & ORDERS_RESIST)
        {
            if (m_combatOrder & ORDERS_RESIST_FIRE)
                out << "I RESIST FIRE";
            if (m_combatOrder & ORDERS_RESIST_NATURE)
                out << "I RESIST NATURE";
            if (m_combatOrder & ORDERS_RESIST_FROST)
                out << "I RESIST FROST";
            if (m_combatOrder & ORDERS_RESIST_SHADOW)
                out << "I RESIST SHADOW";
        }
        if (m_combatOrder & ORDERS_NODISPEL)
            out << "I WON'T DISPEL";
    }
    out << ".";

    if (m_mgr.m_confDebugWhisper)
    {
        out << " " << (IsInCombat() ? "I'm in COMBAT! " : "Not in combat. ");
        out << "Current state is ";
        if (m_botState == BOTSTATE_NORMAL)
            out << "NORMAL";
        else if (m_botState == BOTSTATE_COMBAT)
            out << "COMBAT";
        else if (m_botState == BOTSTATE_TAME)
            out << "TAMING";
        else if (m_botState == BOTSTATE_DEAD)
            out << "DEAD";
        else if (m_botState == BOTSTATE_DEADRELEASED)
            out << "RELEASED";
        else if (m_botState == BOTSTATE_LOOTING)
            out << "LOOTING";
        else if (m_botState == BOTSTATE_FLYING)
            out << "FLYING";
        out << ". Movement order is ";
        if (m_movementOrder == MOVEMENT_NONE)
            out << "NONE";
        else if (m_movementOrder == MOVEMENT_FOLLOW)
            out << "FOLLOW " << (m_followTarget ? m_followTarget->GetName() : "unknown");
        else if (m_movementOrder == MOVEMENT_STAY)
            out << "STAY";
        out << ". Got " << m_attackerInfo.size() << " attacker(s) in list.";
        out << " Next action in " << m_ignoreAIUpdatesUntilTime - CurrentTime() << "sec.";
    }

    TellMaster(out.str().c_str());
    if (m_DelayAttack)
        TellMaster("My combat delay is '%u'", m_DelayAttack);
}

// handle outgoing packets the server would send to the client
void PlayerbotAI::HandleBotOutgoingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        case SMSG_DUEL_WINNER:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
            return;
        }
        case SMSG_DUEL_COMPLETE:
        {
            SetIgnoreUpdateTime(4);
            m_ScenarioType = SCENARIO_PVE;
            ReloadAI();
            m_bot->GetMotionMaster()->Clear(true);
            return;
        }
        case SMSG_DUEL_OUTOFBOUNDS:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);
            return;
        }
        case SMSG_DUEL_REQUESTED:
        {
            SetIgnoreUpdateTime(0);
            WorldPacket p(packet);
            ObjectGuid flagGuid;
            p >> flagGuid;
            ObjectGuid playerGuid;
            p >> playerGuid;
            Player* const pPlayer = ObjectAccessor::FindPlayer(playerGuid);
            if (canObeyCommandFrom(*pPlayer))
            {
                m_bot->GetMotionMaster()->Clear(true);
                std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_DUEL_ACCEPTED, 8));
                *packet << flagGuid;
                m_bot->GetSession()->QueuePacket(std::move(packet)); // queue the packet to get around race condition

                // follow target in casting range
                float angle = rand_float(0, M_PI_F);
                float dist = rand_float(4, 10);

                m_bot->GetMotionMaster()->Clear(true);
                m_bot->GetMotionMaster()->MoveFollow(pPlayer, dist, angle);

                m_bot->SetSelectionGuid(ObjectGuid(playerGuid));
                SetIgnoreUpdateTime(4);
                m_ScenarioType = SCENARIO_PVP_DUEL;
            }
            return;
        }

        case SMSG_PET_TAME_FAILURE:
        {
            // DEBUG_LOG("SMSG_PET_TAME_FAILURE");
            WorldPacket p(packet);
            uint8 reason;
            p >> reason;

            switch (reason)
            {
                case PETTAME_INVALIDCREATURE:           // = 1,
                    DEBUG_LOG("Invalid Creature");
                    break;
                case PETTAME_TOOMANY:                   // = 2,
                    DEBUG_LOG("Too many Creature");
                    break;
                case PETTAME_CREATUREALREADYOWNED:      // = 3,
                    DEBUG_LOG("Creature already owned");
                    break;
                case PETTAME_NOTTAMEABLE:               // = 4,
                    DEBUG_LOG("Creature not tameable");
                    break;
                case PETTAME_ANOTHERSUMMONACTIVE:       // = 5,
                    DEBUG_LOG("Another summon active");
                    break;
                case PETTAME_UNITSCANTTAME:             // = 6,
                    DEBUG_LOG("Unit cant tame");
                    break;
                case PETTAME_NOPETAVAILABLE:            // = 7,    // not used in taming
                    DEBUG_LOG("No pet available");
                    break;
                case PETTAME_INTERNALERROR:             // = 8,
                    DEBUG_LOG("Internal error");
                    break;
                case PETTAME_TOOHIGHLEVEL:              // = 9,
                    DEBUG_LOG("Creature level too high");
                    break;
                case PETTAME_DEAD:                      // = 10,   // not used in taming
                    DEBUG_LOG("Creature dead");
                    break;
                case PETTAME_NOTDEAD:                   // = 11,   // not used in taming
                    DEBUG_LOG("Creature not dead");
                    break;
                case PETTAME_CANTCONTROLEXOTIC:         // = 12,   // 3.x
                    DEBUG_LOG("Creature exotic");
                    break;
                case PETTAME_UNKNOWNERROR:              // = 13
                    DEBUG_LOG("Unknown error");
                    break;
            }
            return;
        }

        case SMSG_BUY_FAILED:
        {
            WorldPacket p(packet); // 8+4+4+1
            ObjectGuid vendorguid;
            p >> vendorguid;
            uint32 itemid;
            p >> itemid;
            uint8 msg;
            p >> msg; // error msg
            p.resize(13);

            switch (msg)
            {
                case BUY_ERR_CANT_FIND_ITEM:
                    break;
                case BUY_ERR_ITEM_ALREADY_SOLD:
                    break;
                case BUY_ERR_NOT_ENOUGHT_MONEY:
                {
                    Announce(CANT_AFFORD);
                    break;
                }
                case BUY_ERR_SELLER_DONT_LIKE_YOU:
                    break;
                case BUY_ERR_DISTANCE_TOO_FAR:
                    break;
                case BUY_ERR_ITEM_SOLD_OUT:
                    break;
                case BUY_ERR_CANT_CARRY_MORE:
                {
                    Announce(INVENTORY_FULL);
                    break;
                }
                case BUY_ERR_RANK_REQUIRE:
                    break;
                case BUY_ERR_REPUTATION_REQUIRE:
                    break;
            }
            return;
        }

        case SMSG_AUCTION_COMMAND_RESULT:
        {
            uint32 auctionId, Action, ErrorCode;
            std::string action[3] = {"Creating", "Cancelling", "Bidding"};
            std::ostringstream out;

            WorldPacket p(packet);
            p >> auctionId;
            p >> Action;
            p >> ErrorCode;
            p.resize(12);

            switch (ErrorCode)
            {
                case AUCTION_OK:
                {
                    out << "|cff1eff00|h" << action[Action] << " was successful|h|r";
                    break;
                }
                case AUCTION_ERR_DATABASE:
                {
                    out << "|cffff0000|hWhile" << action[Action] << ", an internal error occured|h|r";
                    break;
                }
                case AUCTION_ERR_NOT_ENOUGH_MONEY:
                {
                    out << "|cffff0000|hWhile " << action[Action] << ", I didn't have enough money|h|r";
                    break;
                }
                case AUCTION_ERR_ITEM_NOT_FOUND:
                {
                    out << "|cffff0000|hItem was not found!|h|r";
                    break;
                }
                case AUCTION_ERR_BID_OWN:
                {
                    out << "|cffff0000|hI cannot bid on my own auctions!|h|r";
                    break;
                }
            }
            TellMaster(out.str().c_str());
            return;
        }

        case SMSG_INVENTORY_CHANGE_FAILURE:
        {
            WorldPacket p(packet);
            uint8 err;
            p >> err;

            if (err != EQUIP_ERR_OK)
            {
                switch (err)
                {
                    case EQUIP_ERR_CANT_CARRY_MORE_OF_THIS:
                    {
                        TellMaster("I can't carry anymore of those.");
                        m_lootCurrent = ObjectGuid();
                        return;
                    }
                    case EQUIP_ERR_MISSING_REAGENT:
                        TellMaster("I'm missing some reagents for that.");
                        return;
                    case EQUIP_ERR_ITEM_LOCKED:
                        TellMaster("That item is locked.");
                        return;
                    case EQUIP_ERR_ALREADY_LOOTED:
                        TellMaster("That is already looted.");
                        return;
                    case EQUIP_ERR_INVENTORY_FULL:
                    {
                        if (DropGarbage(false))
                            return;

                        if (m_lootCurrent.IsGameObject())
                            if (GameObject* go = m_bot->GetMap()->GetGameObject(m_lootCurrent))
                                m_collectObjects.remove(go->GetEntry());

                        m_lootCurrent = ObjectGuid();

                        if (m_inventory_full)
                            return;

                        TellMaster("My inventory is full.");
                        m_inventory_full = true;
                        return;
                    }
                    case EQUIP_ERR_NOT_IN_COMBAT:
                        TellMaster("I can't use that in combat.");
                        return;
                    case EQUIP_ERR_LOOT_CANT_LOOT_THAT_NOW:
                        TellMaster("I can't get that now.");
                        return;
                    case EQUIP_ERR_ITEM_UNIQUE_EQUIPABLE:
                        TellMaster("I can only have one of those equipped.");
                        return;
                    case EQUIP_ERR_BANK_FULL:
                        TellMaster("My bank is full.");
                        return;
                    case EQUIP_ERR_ITEM_NOT_FOUND:
                        TellMaster("I can't find the item.");
                        return;
                    case EQUIP_ERR_TOO_FAR_AWAY_FROM_BANK:
                        TellMaster("I'm too far from the bank.");
                        return;
                    case EQUIP_ERR_NONE:
                        TellMaster("I can't use it on that");
                        return;
                    default:
                        TellMaster("I can't use that.");
                        DEBUG_LOG("[PlayerbotAI]: HandleBotOutgoingPacket - SMSG_INVENTORY_CHANGE_FAILURE: %u", err);
                        return;
                }
            }
        }

        case SMSG_CAST_RESULT:
        {
            WorldPacket p(packet);
            uint8 castCount;
            uint32 spellId;
            uint8 result;
            std::ostringstream out;

            p >> castCount >> spellId >> result;

            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
            if (!spellInfo)
                return;

            if (result != SPELL_CAST_OK)
            {
                switch (result)
                {
                    case SPELL_FAILED_INTERRUPTED:  // 40
                    {
                        DEBUG_LOG("spell %s interrupted (%u)", spellInfo->SpellName[0], result);
                        return;
                    }
                    case SPELL_FAILED_UNIT_NOT_INFRONT:  // 134
                        if (m_targetCombat)
                            m_bot->SetInFront(m_targetCombat);
                    case SPELL_FAILED_BAD_TARGETS:  // 12
                    {
                        // DEBUG_LOG("[%s]bad target / not in front(%u) for spellId (%u) & m_CurrentlyCastingSpellId (%u)",m_bot->GetName(),result,spellId,m_CurrentlyCastingSpellId);
                        Spell* const pSpell = GetCurrentSpell();
                        if (pSpell)
                            pSpell->cancel();
                        return;
                    }
                    case SPELL_FAILED_REQUIRES_SPELL_FOCUS: // 102
                    {
                        switch (spellInfo->RequiresSpellFocus) // SpellFocusObject.dbc id
                        {
                            case 1:  // need an anvil
                                out << "|cffff0000I require an anvil.";
                                break;
                            case 2:  // need a loom
                                out << "|cffff0000I require a loom.";
                                break;
                            case 3:  // need forge
                                out << "|cffff0000I require a forge.";
                                break;
                            case 4:  // need cooking fire
                                out << "|cffff0000I require a cooking fire.";
                                break;
                            default:
                                out << "|cffff0000I Require Spell Focus on " << spellInfo->RequiresSpellFocus;
                        }
                        break;
                    }
                    case SPELL_FAILED_CANT_BE_DISENCHANTED:  // 14
                    {
                        out << "|cffff0000Item cannot be disenchanted.";
                        break;
                    }
                    case SPELL_FAILED_CANT_BE_MILLED:  // 16
                    {
                        out << "|cffff0000I cannot mill that.";
                        break;
                    }
                    case SPELL_FAILED_CANT_BE_PROSPECTED:  // 17
                    {
                        out << "|cffff0000There are no gems in this.";
                        break;
                    }
                    case SPELL_FAILED_EQUIPPED_ITEM_CLASS:  // 29
                    {
                        out << "|cffff0000That item is not a valid target.";
                        break;
                    }
                    case SPELL_FAILED_NEED_MORE_ITEMS:  // 55
                    {
                        ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(m_itemTarget);
                        if (!pProto)
                            return;

                        out << "|cffff0000Requires 5 " << pProto->Name1 << ".";
                        m_itemTarget = 0;
                        break;
                    }
                    case SPELL_FAILED_REAGENTS:
                    {
                        out << "|cffff0000I don't have the reagents";
                        break;
                    }
                    default:
                        DEBUG_LOG("[%s] SMSG_CAST_FAIL: %s err (%u)", m_bot->GetName(), spellInfo->SpellName[0], result);
                        return;
                }
            }
            TellMaster(out.str().c_str());
            return;
        }

        case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            uint8 castCount;
            uint32 spellId;
            ObjectGuid casterGuid;

            p >> casterGuid.ReadAsPacked();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            p >> castCount >> spellId;
            if (m_CurrentlyCastingSpellId == spellId)
            {
                SetIgnoreUpdateTime(0);
                m_CurrentlyCastingSpellId = 0;
            }
            return;
        }
        // handle flying acknowledgement
        case SMSG_MOVE_SET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid;

            p >> guid.ReadAsPacked();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            //m_bot->SetSpeed(MOVE_RUN, GetMaster()->GetSpeed(MOVE_FLIGHT) +0.1f, true);
            return;
        }

        // handle dismount flying acknowledgement
        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid;

            p >> guid.ReadAsPacked();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            //m_bot->SetSpeed(MOVE_RUN,GetMaster()->GetSpeedRate(MOVE_RUN),true);
            return;
        }

        // Handle chat messages here
        case SMSG_MESSAGECHAT:
        {
            WorldPacket p(packet);
            uint8 msgtype;
            uint32 language;

            p >> msgtype;           // 1 type
            p >> language;          // 4 language

            if (language == LANG_ADDON)
                return;

            switch (msgtype)
            {
                case CHAT_MSG_RAID:
                case CHAT_MSG_RAID_LEADER:
                case CHAT_MSG_PARTY:
                case CHAT_MSG_PARTY_LEADER:
                case CHAT_MSG_WHISPER:
                {
                    ObjectGuid senderGuid;
                    std::string channelName;
                    uint32 length;
                    std::string text;
                    uint8 chattag;

                    p >> senderGuid;        // 8 player from guid
                    p.read_skip<uint32>();  // 4 chat msg group
                    p.read_skip<uint64>();  // 8 player from guid
                    p >> length;            // 4 length of text
                    p >> text;              // string message
                    p >> chattag;           // 1 AFK/DND/WHISPER_INFORM

                    Player* sender = sObjectMgr.GetPlayer(senderGuid);
                    if (!sender)            // couldn't find player that sent message
                        return;

                    // do not listen to other bots
                    if (sender != m_bot && sender->GetPlayerbotAI())
                        return;
                    HandleCommand(text, *sender);
                    return;
                }
                default:
                    return;
            }

            return;
        }

        // If the leader role was given to the bot automatically give it to the master
        // if the master is in the group, otherwise leave group
        case SMSG_GROUP_SET_LEADER:
        {
            WorldPacket p(packet);
            std::string name;
            p >> name;
            if (m_bot->GetGroup() && name == m_bot->GetName())
            {
                if (m_bot->GetGroup()->IsMember(GetMaster()->GetObjectGuid()))
                {
                    p.resize(8);
                    p << GetMaster()->GetObjectGuid();
                    m_bot->GetSession()->HandleGroupSetLeaderOpcode(p);
                }
                else
                {
                    p.clear(); // not really needed
                    m_bot->GetSession()->HandleGroupDisbandOpcode(p); // packet not used updated code
                }
            }
            return;
        }

        // If the master leaves the group, then the bot leaves too
        case SMSG_PARTY_COMMAND_RESULT:
        {
            WorldPacket p(packet);
            uint32 operation;
            p >> operation;
            std::string member;
            p >> member;
            uint32 result;
            p >> result;
            p.clear();
            if (operation == PARTY_OP_LEAVE)
                if (member == GetMaster()->GetName())
                    m_bot->GetSession()->HandleGroupDisbandOpcode(p);  // packet not used updated code
            return;
        }

        // Handle Group invites (auto accept if master is in group, otherwise decline & send message
        case SMSG_GROUP_INVITE:
        {
            if (m_bot->GetGroupInvite())
            {
                const Group* const grp = m_bot->GetGroupInvite();
                if (!grp)
                    return;

                Player* const inviter = sObjectMgr.GetPlayer(grp->GetLeaderGuid());
                if (!inviter)
                    return;

                WorldPacket p;
                if (!canObeyCommandFrom(*inviter))
                {
                    std::string buf = "I can't accept your invite unless you first invite my master ";
                    buf += GetMaster()->GetName();
                    buf += ".";
                    SendWhisper(buf, *inviter);
                    m_bot->GetSession()->HandleGroupDeclineOpcode(p); // packet not used
                }
                else
                    m_bot->GetSession()->HandleGroupAcceptOpcode(p);  // packet not used
            }
            return;
        }

        case SMSG_GUILD_INVITE:
        {
            Guild* guild = sGuildMgr.GetGuildById(m_bot->GetGuildIdInvited());
            if (!guild || m_bot->GetGuildId())
                return;

            // not let enemies sign guild charter
            if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD) && m_bot->GetTeam() != sObjectMgr.GetPlayerTeamByGUID(guild->GetLeaderGuid()))
                return;

            if (!guild->AddMember(m_bot->GetObjectGuid(), guild->GetLowestRank()))
                return;
            // Put record into guild log
            guild->LogGuildEvent(GUILD_EVENT_LOG_JOIN_GUILD, m_bot->GetObjectGuid());

            guild->BroadcastEvent(GE_JOINED, m_bot->GetObjectGuid(), m_bot->GetName());
        }

        // Handle when another player opens the trade window with the bot
        // also sends list of tradable items bot can trade if bot is allowed to obey commands from
        case SMSG_TRADE_STATUS:
        {
            if (m_bot->GetTrader() == nullptr)
                break;

            WorldPacket p(packet);
            uint32 status;
            p >> status;
            p.resize(4);

            //4 == TRADE_STATUS_TRADE_ACCEPT
            if (status == 4)
            {
                m_bot->GetSession()->HandleAcceptTradeOpcode(p);  // packet not used
                SetQuestNeedItems();
                AutoUpgradeEquipment();
            }

            //1 == TRADE_STATUS_BEGIN_TRADE
            else if (status == 1)
            {
                m_bot->GetSession()->HandleBeginTradeOpcode(p); // packet not used

                if (!canObeyCommandFrom(*(m_bot->GetTrader())))
                {
                    // TODO: Really? What if I give a bot all my junk so it's inventory is full when a nice green/blue/purple comes along?
                    SendWhisper("I'm not allowed to trade you any of my items, but you are free to give me money or items.", *(m_bot->GetTrader()));
                    return;
                }

                // list out items
                std::ostringstream out;
                std::ostringstream outT;
                std::ostringstream outNT;
                uint8 countTotalTradeable = 0;
                uint8 countTradeable = 0;
                uint8 countNonTradeable = 0;

                outT << "Tradeable:";
                outNT << "Non-tradeable:";
                // list out items in main backpack
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem)
                    {
                        if (pItem->CanBeTraded())
                        {
                            countTradeable++;
                            MakeItemLink(pItem, outT, true);
                        }
                        else
                        {
                            countNonTradeable++;
                            MakeItemLink(pItem, outNT, true);
                        }
                    }
                }

                countTotalTradeable = countTradeable;
                out << "Backpack (" << countTradeable + countNonTradeable << "/16) ";
                if (countTradeable > 0)
                    out << outT.str();
                if (countNonTradeable > 0)
                    out << "\r" << outNT.str();
                SendWhisper(out.str().c_str(), *(m_bot->GetTrader()));

                // list out items in other removable backpacks
                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                    {
                        countTradeable = 0;
                        countNonTradeable = 0;
                        std::ostringstream outbagT;
                        std::ostringstream outbagNT;
                        outbagT << "Tradeable:";
                        outbagNT << "Non-tradeable:";

                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem)
                            {
                                if (pItem->CanBeTraded())
                                {
                                    countTradeable++;
                                    MakeItemLink(pItem, outbagT, true);
                                }
                                else
                                {
                                    countNonTradeable++;
                                    MakeItemLink(pItem, outbagNT, true);
                                }
                            }
                        }

                        countTotalTradeable += countTradeable;
                        std::ostringstream outbag;
                        const ItemPrototype* const pBagProto = pBag->GetProto();
                        std::string bagName = pBagProto->Name1;
                        ItemLocalization(bagName, pBagProto->ItemId);
                        outbag << bagName << " (";
                        outbag << countTradeable + countNonTradeable << "/" << pBag->GetBagSize();
                        outbag << ") ";
                        if (countTradeable > 0)
                            outbag << outbagT.str();
                        if (countNonTradeable > 0)
                            outbag << "\r" << outbagNT.str();
                        SendWhisper(outbag.str().c_str(), *(m_bot->GetTrader()));
                    }
                }
                if (countTotalTradeable == 0)
                    SendWhisper("I have no items to give you.", *(m_bot->GetTrader()));

                // calculate how much money bot has
                // send bot the message
                uint32 copper = m_bot->GetMoney();
                out.str("");
                out << "I have |cff00ff00" << Cash(copper) << "|r";
                SendWhisper(out.str().c_str(), *(m_bot->GetTrader()));
            }
            return;
        }

        case SMSG_SPELL_START:
        {
            WorldPacket p(packet);

            ObjectGuid castItemGuid;
            p >> castItemGuid.ReadAsPacked();
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            uint8 castCount;
            p >> castCount;
            uint32 spellId;
            p >> spellId;
            uint32 castFlags;
            p >> castFlags;
            uint32 msTime;
            p >> msTime;

            const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
            if (!pSpellInfo)
                return;

            if (pSpellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_STANDING_CANCELS)
                return;

            SetIgnoreUpdateTime((msTime / 1000) + 1);

            return;
        }

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);

            ObjectGuid castItemGuid;
            p >> castItemGuid.ReadAsPacked();
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            uint8 castCount;
            p >> castCount;
            uint32 spellId;
            p >> spellId;
            uint32 castFlags;
            p >> castFlags;
            uint32 msTime;
            p >> msTime;

            // use this spell, 836 login effect, as a signal from server that we're in world
            if (spellId == 836 && m_botState == BOTSTATE_LOADING)
                SetState(BOTSTATE_NORMAL);

            return;
        }

        // if someone tries to resurrect, then accept
        case SMSG_RESURRECT_REQUEST:
        {
            if (!m_bot->IsAlive())
            {
                WorldPacket p(packet);
                ObjectGuid guid;
                p >> guid;

                std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_RESURRECT_RESPONSE, 8 + 1));
                *packet << guid;
                *packet << uint8(1);                        // accept
                m_bot->GetSession()->QueuePacket(std::move(packet));   // queue the packet to get around race condition

                // set back to normal
                SetState(BOTSTATE_NORMAL);
                SetIgnoreUpdateTime(0);
            }
            return;
        }

        case SMSG_LOOT_RESPONSE:
        {
            WorldPacket p(packet); // (8+1+4+1+1+4+4+4+4+4+1)
            ObjectGuid guid;
            uint8 loot_type;
            uint32 gold;
            uint8 items;

            p >> guid;      // 8 corpse guid
            p >> loot_type; // 1 loot type
            p >> gold;      // 4 gold
            p >> items;     // 1 items count

            if (gold > 0)
            {
                WorldPacket* const packet = new WorldPacket(CMSG_LOOT_MONEY, 0);
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(packet)));
            }

            for (uint8 i = 0; i < items; ++i)
            {
                uint32 itemid;
                uint32 itemcount;
                uint8 lootslot_type;
                uint8 itemindex;

                p >> itemindex;         // 1 counter
                p >> itemid;            // 4 itemid
                p >> itemcount;         // 4 item stack count
                p.read_skip<uint32>();  // 4 item model
                p.read_skip<uint32>();  // 4 randomSuffix
                p.read_skip<uint32>();  // 4 randomPropertyId
                p >> lootslot_type;     // 1 LootSlotType

                ItemPrototype const *pProto = ObjectMgr::GetItemPrototype(itemid);
                if (!pProto)
                    continue;

                if (lootslot_type != LOOT_SLOT_NORMAL && lootslot_type != LOOT_SLOT_OWNER)
                    continue;

                // skinning or collect loot flag = just auto loot everything for getting object
                // corpse = run checks
                if (loot_type == LOOT_SKINNING || HasCollectFlag(COLLECT_FLAG_LOOT) ||
                        (loot_type == LOOT_CORPSE && (IsInQuestItemList(itemid) || IsItemUseful(itemid))))
                {
                    ItemPosCountVec dest;
                    if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemid, itemcount) == EQUIP_ERR_INVENTORY_FULL)
                    {
                        if (m_debugWhisper)
                            TellMaster("I can't take %; my inventory is full.", pProto->Name1);
                        m_inventory_full = true;
                        continue;
                    }

                    if (m_debugWhisper)
                        TellMaster("Store loot item %s", pProto->Name1);

                    WorldPacket* const packet = new WorldPacket(CMSG_AUTOSTORE_LOOT_ITEM, 1);
                    *packet << itemindex;
                    m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(packet)));
                }
                else
                {
                    if (m_debugWhisper)
                        TellMaster("Skipping loot item %s", pProto->Name1);
                }
            }

            if (m_debugWhisper)
                TellMaster("Releasing loot");
            // release loot
            m_lootPrev = m_lootCurrent;
            m_lootCurrent = ObjectGuid();
            WorldPacket* const packet = new WorldPacket(CMSG_LOOT_RELEASE, 8);
            *packet << guid;
            m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(packet)));

            return;
        }

        case SMSG_LOOT_RELEASE_RESPONSE:
        {
            WorldPacket p(packet);
            ObjectGuid guid;

            p >> guid;

            if (guid == m_lootPrev)
            {
                Creature* c = m_bot->GetMap()->GetCreature(m_lootPrev);

                if (c && c->GetCreatureInfo()->SkinningLootId && c->GetLootStatus() != CREATURE_LOOT_STATUS_LOOTED)
                {
                    uint32 reqSkill = c->GetCreatureInfo()->GetRequiredLootSkill();
                    // check if it is a leather skin and if it is to be collected (could be ore or herb)
                    if (m_bot->HasSkill(reqSkill) && ((reqSkill != SKILL_SKINNING) ||
                                                      (HasCollectFlag(COLLECT_FLAG_SKIN) && reqSkill == SKILL_SKINNING)))
                    {
                        // calculate skill requirement
                        uint32 skillValue = m_bot->GetSkillValue(reqSkill);
                        uint32 targetLevel = c->GetLevel();
                        uint32 reqSkillValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;
                        if (skillValue >= reqSkillValue)
                        {
                            m_lootCurrent = m_lootPrev;
                            if (m_debugWhisper)
                                TellMaster("I will try to skin next loot attempt.");

                            SetIgnoreUpdateTime(1);
                            return; // so that the DoLoot function is called again to get skin
                        }
                        else
                            TellMaster("My skill is %u but it requires %u", skillValue, reqSkillValue);
                    }
                }

                // clear movement
                m_bot->GetMotionMaster()->Clear(false);
                m_bot->GetMotionMaster()->MoveIdle();
                SetIgnoreUpdateTime(0);
            }

            return;
        }

        case SMSG_LOOT_ROLL_WON:
        {
            WorldPacket p(packet);   // (8+4+4+4+4+8+1+1)
            ObjectGuid p_guid, co_guid;
            uint32 itemid;

            p >> co_guid;            // creature or gameobject guid what we're looting
            p.read_skip<uint32>();   // item slot in loot
            p >> itemid;             // the itemEntryId for the item that shall be rolled fo
            p.read_skip<uint32>();   // randomSuffix
            p.read_skip<uint32>();   // Item random property
            p >> p_guid;             // guid of the player who won
            p.read_skip<uint8>();    // rollnumber related to SMSG_LOOT_ROLL
            p.read_skip<uint8>();    // Rolltype related to SMSG_LOOT_ROLL

            Loot* loot = sLootMgr.GetLoot(m_bot, co_guid);

            if (!loot)
                return;

            // Clean up: remove target guid from (ignore list)
            for (std::list<ObjectGuid>::iterator itr = m_being_rolled_on.begin(); itr != m_being_rolled_on.end();)
                if (co_guid == *itr)
                {
                    // DEBUG_LOG("Rolled item won, removing (%s)",co_guid.GetString().c_str());
                    itr = m_being_rolled_on.erase(itr);
                }
                else
                    ++itr;

            // allow creature corpses to be skinned after roll
            loot->Release(m_bot);

            if (m_bot->GetObjectGuid() != p_guid)
                return;

            SetState(BOTSTATE_DELAYED);

            /* ItemPrototype const *pProto = ObjectMgr::GetItemPrototype(itemid);
            if(pProto)
            {
                std::ostringstream out;
                out << "|cff009900" << "I won: |r";
                MakeItemLink(pProto,out);
                if (FindItem(itemid,true))
                    out << "and have the item";
                TellMaster(out.str().c_str());
            }*/
            return;
        }

        case SMSG_PARTYKILLLOG:
        {
            // reset AI delay so bots immediately respond to next combat target & or looting/skinning
            SetIgnoreUpdateTime(0);
            return;
        }

        case SMSG_ITEM_PUSH_RESULT:
        {
            WorldPacket p(packet);  // (8+4+4+4+1+4+4+4+4+4+4)
            ObjectGuid guid;

            p >> guid;              // 8 player guid
            if (m_bot->GetObjectGuid() != guid)
                return;

            uint8 bagslot;
            uint32 itemslot, itemid, count, totalcount, received, created;

            p >> received;          // 4 0=looted, 1=from npc
            p >> created;           // 4 0=received, 1=created
            p.read_skip<uint32>();  // 4 IsShowChatMessage
            p >> bagslot;           // 1 bagslot
            p >> itemslot;          // 4 item slot, but when added to stack: 0xFFFFFFFF
            p >> itemid;            // 4 item entry id
            p.read_skip<uint32>();  // 4 SuffixFactor
            p.read_skip<uint32>();  // 4 random item property id
            p >> count;             // 4 count of items
            p >> totalcount;        // 4 count of items in inventory

            ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);
            if (pProto)
            {
                std::ostringstream out;
                if (received == 1)
                {
                    if (created == 1)
                        out << "|cff009900" << "I created: |r";
                    else
                        out << "|cff009900" << "I received: |r";
                    MakeItemLink(pProto, out);
                    TellMaster(out.str().c_str());
                    SetState(BOTSTATE_DELAYED);
                }
            }

            if (IsInQuestItemList(itemid))
            {
                m_needItemList[itemid] = (m_needItemList[itemid] - count);
                if (m_needItemList[itemid] <= 0)
                    m_needItemList.erase(itemid);
            }

            return;
        }

        case MSG_MOVE_TELEPORT_ACK:
        {
            WorldPacket rp(packet);
            ObjectGuid guid;
            rp >> guid.ReadAsPacked();

            if (guid != m_bot->GetObjectGuid())
                return;

            uint32 counter;
            rp >> counter;
            // movement location to teleport to
            MovementInfo mi;
            rp >> mi;

            if (m_debugWhisper)
                TellMaster("Preparing to teleport");

            if (m_bot->IsBeingTeleportedNear())
            {
                // simulate same packets that are sent for client
                WorldPacket* const p = new WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
                p->appendPackGUID(m_bot->GetObjectGuid());
                *p << counter;
                *p << (uint32) time(0); // time - not currently used
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(p)));

                // send movement info using received movement packet, pops in location
                WorldPacket* const p2 = new WorldPacket(MSG_MOVE_HEARTBEAT, 64);
                p2->appendPackGUID(m_bot->GetObjectGuid());
                *p2 << mi;
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(p2)));

                WorldPacket* const p3 = new WorldPacket(MSG_MOVE_FALL_LAND, 64);
                p3->appendPackGUID(m_bot->GetObjectGuid());
                *p3 << mi;
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(p3)));

                // resume normal state if was loading
                if (m_botState == BOTSTATE_LOADING)
                    SetState(BOTSTATE_NORMAL);
            }
            return;
        }
        case SMSG_TRANSFER_PENDING:
        {
            if (m_debugWhisper)
                TellMaster("World transfer is pending");
            SetState(BOTSTATE_LOADING);
            SetIgnoreUpdateTime(1);
            m_bot->GetMotionMaster()->Clear(true);
            return;
        }
        case SMSG_NEW_WORLD:
        {
            if (m_debugWhisper)
                TellMaster("Preparing to teleport far");

            if (m_bot->IsBeingTeleportedFar())
            {
                // simulate client canceling trade before worldport
                WorldPacket* const pt1 = new WorldPacket(CMSG_CANCEL_TRADE);
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(pt1)));

                WorldPacket* const p = new WorldPacket(MSG_MOVE_WORLDPORT_ACK);
                m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(p)));
                SetState(BOTSTATE_NORMAL);
            }
            return;
        }

        /* uncomment this and your bots will tell you all their outgoing packet opcode names
        case SMSG_MONSTER_MOVE:
        case SMSG_UPDATE_WORLD_STATE:
        case SMSG_COMPRESSED_UPDATE_OBJECT:
        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_STOP:
        case MSG_MOVE_HEARTBEAT:
        case MSG_MOVE_STOP_STRAFE:
        case MSG_MOVE_START_STRAFE_LEFT:
        case SMSG_UPDATE_OBJECT:
        case MSG_MOVE_START_FORWARD:
        case MSG_MOVE_START_STRAFE_RIGHT:
        case SMSG_DESTROY_OBJECT:
        case MSG_MOVE_START_BACKWARD:
        case SMSG_AURA_UPDATE_ALL:
        case MSG_MOVE_FALL_LAND:
        case MSG_MOVE_JUMP:
        return;*/

        default:
        {
            /*const char* oc = LookupOpcodeName(packet.GetOpcode());

            std::ostringstream out;
            out << "botout: " << oc;
            sLog.outError(out.str().c_str());

            TellMaster(oc);*/
        }
    }
}

uint8 PlayerbotAI::GetHealthPercent(const Unit& target) const
{
    return (static_cast<float>(target.GetHealth()) / target.GetMaxHealth()) * 100;
}

uint8 PlayerbotAI::GetHealthPercent() const
{
    return GetHealthPercent(*m_bot);
}

uint8 PlayerbotAI::GetManaPercent(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
}

uint8 PlayerbotAI::GetManaPercent() const
{
    return GetManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetRageAmount(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_RAGE)));
}

uint8 PlayerbotAI::GetRageAmount() const
{
    return GetRageAmount(*m_bot);
}

uint8 PlayerbotAI::GetEnergyAmount(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_ENERGY)));
}

uint8 PlayerbotAI::GetEnergyAmount() const
{
    return GetEnergyAmount(*m_bot);
}

uint8 PlayerbotAI::GetRunicPower(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_RUNIC_POWER)));
}

uint8 PlayerbotAI::GetRunicPower() const
{
    return GetRunicPower(*m_bot);
}

bool PlayerbotAI::HasAura(uint32 spellId, const Unit& player) const
{
    if (spellId <= 0)
        return false;

    for (Unit::SpellAuraHolderMap::const_iterator iter = player.GetSpellAuraHolderMap().begin(); iter != player.GetSpellAuraHolderMap().end(); ++iter)
    {
        if (iter->second->GetId() == spellId)
            return true;
    }
    return false;
}

bool PlayerbotAI::HasAura(const char* spellName) const
{
    return HasAura(spellName, *m_bot);
}

bool PlayerbotAI::HasAura(const char* spellName, const Unit& player) const
{
    uint32 spellId = getSpellId(spellName);
    return (spellId) ? HasAura(spellId, player) : false;
}

// looks through all items / spells that bot could have to get a mount
Item* PlayerbotAI::FindMount(uint32 matchingRidingSkill) const
{
    // list out items in main backpack

    Item* partialMatch = nullptr;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                continue;

            // Ignore items that can not be used in the current situation (have requirements)
            uint32 spellId = 0;
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
            {
                if (pItemProto->Spells[i].SpellId > 0)
                {
                    spellId = pItemProto->Spells[i].SpellId;
                    break;
                }
            }
            const SpellEntry* const spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
            if (spellInfo)
            {
                Spell* spell = new Spell(m_bot, spellInfo, false);
                if (spell && spell->CheckCast(false) != SPELL_CAST_OK)
                    continue;
            }

            if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                return pItem;

            else if (!partialMatch || (partialMatch && partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank))
                partialMatch = pItem;
        }
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                        continue;

                    // Ignore items that can not be used in the current situation (have requirements)
                    uint32 spellId = 0;
                    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                    {
                        if (pItemProto->Spells[i].SpellId > 0)
                        {
                            spellId = pItemProto->Spells[i].SpellId;
                            break;
                        }
                    }
                    const SpellEntry* const spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                    if (spellInfo)
                    {
                        Spell* spell = new Spell(m_bot, spellInfo, false);
                        if (spell && spell->CheckCast(false) != SPELL_CAST_OK)
                            continue;
                    }

                    if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                        return pItem;

                    else if (!partialMatch || (partialMatch && partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank))
                        partialMatch = pItem;
                }
            }
    }
    return partialMatch;
}

Item* PlayerbotAI::FindFood() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                // if is FOOD
                // this enum is no longer defined in mangos. Is it no longer valid?
                // according to google it was 11
                if (pItemProto->Spells[0].SpellCategory == 11)
                    return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    // this enum is no longer defined in mangos. Is it no longer valid?
                    // according to google it was 11
                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                        // if is FOOD
                        // this enum is no longer defined in mangos. Is it no longer valid?
                        // according to google it was 11
                        // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_FOOD)
                        if (pItemProto->Spells[0].SpellCategory == 11)
                            return pItem;
                }
            }
    }
    return nullptr;
}

Item* PlayerbotAI::FindDrink() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_DRINK)

                // this enum is no longer defined in mangos. Is it no longer valid?
                // according to google it was 59
                // if (pItemProto->Spells[0].SpellCategory == 59)
                if (pItemProto->Spells[0].SpellCategory == 59)
                    return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                        // if is WATER
                        // SPELL_CATEGORY_DRINK is no longer defined in an enum in mangos
                        // google says the valus is 59. Is this still valid?
                        // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_DRINK)
                        if (pItemProto->Spells[0].SpellCategory == 59)
                            return pItem;
                }
            }
    }
    return nullptr;
}

Item* PlayerbotAI::FindBandage() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                        return pItem;
                }
            }
    }
    return nullptr;
}
//Find Poison ...Natsukawa
Item* PlayerbotAI::FindPoison() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                        return pItem;
                }
            }
    }
    return nullptr;
}

Item* PlayerbotAI::FindConsumable(uint32 displayId) const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->DisplayInfoID == displayId)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->DisplayInfoID == displayId)
                        return pItem;
                }
            }
    }
    return nullptr;
}

static const uint32 uPriorizedManaPotionIds[12] =
{
    FEL_MANA_POTION, CRYSTAL_MANA_POTION, SUPER_MANA_POTION, UNSTABLE_MANA_POTION,
    MAJOR_MANA_POTION, MAJOR_REJUVENATION_POTION, SUPERIOR_MANA_POTION,
    GREATER_MANA_POTION, MANA_POTION, LESSER_MANA_POTION,
    MINOR_MANA_POTION, MINOR_REJUVENATION_POTION
};

/**
 * FindManaRegenItem()
 * return Item* Returns items like runes or potion that can help the bot to instantly resplenish some of its mana
 *
 * return nullptr if no relevant item is found in bot inventory, else return a consumable item providing mana
 *
 */
Item* PlayerbotAI::FindManaRegenItem() const
{
    Item* manaRegen;
    // If bot has enough health, try to use a Demonic or Dark Rune
    // to avoid triggering the health potion cooldown with a mana potion
    if (m_bot->GetHealth() > 1500)
    {
        // First try a Demonic Rune as they are BoP
        manaRegen = FindConsumable(DEMONIC_RUNE);
        if (manaRegen)
            return manaRegen;
        else
        {
            manaRegen = FindConsumable(DARK_RUNE);
            if (manaRegen)
                return manaRegen;
        }
    }
    // Else use mana potion (and knowingly trigger the health potion cooldown)
    for (uint8 i = 0; i < countof(uPriorizedManaPotionIds); ++i)
    {
        manaRegen = FindConsumable(uPriorizedManaPotionIds[i]);
        if (manaRegen)
            return manaRegen;
    }

    return nullptr;
}

bool PlayerbotAI::FindAmmo() const
{
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        Item* pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (pItemProto->Class == ITEM_CLASS_PROJECTILE && m_bot->CheckAmmoCompatibility(pItemProto))
            {
                m_bot->SetAmmo(pItem->GetEntry());
                return true;
            }
        }
    }
    for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag* pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
            {
                Item* pItem = m_bot->GetItemByPos(i, j);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (pItemProto->Class == ITEM_CLASS_PROJECTILE && m_bot->CheckAmmoCompatibility(pItemProto))
                    {
                        m_bot->SetAmmo(pItem->GetEntry());
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void PlayerbotAI::InterruptCurrentCastingSpell()
{
    // TellMaster("I'm interrupting my current spell!");
    std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_CANCEL_CAST, 5));  //changed from thetourist suggestion
    *packet << m_CurrentlyCastingSpellId;
    *packet << m_targetGuidCommand;   //changed from thetourist suggestion
    m_CurrentlyCastingSpellId = 0;
    m_bot->GetSession()->QueuePacket(std::move(packet));
}

// intelligently sets a reasonable combat order for this bot
// based on its class / level / etc
void PlayerbotAI::Attack(Unit* forcedTarget)
{
    // set combat state, and clear looting, etc...
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        // m_lootCurrent = ObjectGuid(); This was clearing loot target, causing bots to leave corpses unlooted if interupted by combat. Needs testing.
        // using this caused bot to remove current loot target, and add this new threat to the loot list.  Now it remembers the loot target and adds a new one.
        // Bot will still clear the target if the master gets too far away from it.
        m_targetCombat = nullptr;
        m_DelayAttackInit = CurrentTime(); // Combat started, new start time to check CombatDelay for.
    }

    GetCombatTarget(forcedTarget);

    if (!m_targetCombat)
        return;

    m_bot->Attack(m_targetCombat, true);

    // add thingToAttack to loot list to loot after combat
    if (HasCollectFlag(COLLECT_FLAG_COMBAT))
        m_lootTargets.push_back(m_targetCombat->GetObjectGuid());
}

// intelligently sets a reasonable combat order for this bot
// based on its class / level / etc
// Function will try to avoid returning crowd controlled (neutralised) unit
// unless told so by using forcedTarget parameter
void PlayerbotAI::GetCombatTarget(Unit* forcedTarget)
{
    // update attacker info now
    UpdateAttackerInfo();

    Unit* candidateTarget;

    // check for attackers on protected unit, and make it a forcedTarget if any
    if (!forcedTarget && (m_combatOrder & ORDERS_PROTECT) && m_targetProtect)
    {
        candidateTarget = FindAttacker((ATTACKERINFOTYPE)(AIT_VICTIMNOTSELF | AIT_HIGHESTTHREAT), m_targetProtect);
        if (candidateTarget && candidateTarget != m_targetCombat && !IsNeutralized(candidateTarget))
        {
            forcedTarget = candidateTarget;
            m_targetType = TARGET_THREATEN;
            if (m_mgr.m_confDebugWhisper)
                TellMaster("Changing target to %s to protect %s", forcedTarget->GetName(), m_targetProtect->GetName());
        }
    }

    // are we forced on a target?
    if (forcedTarget)
    {
        // forced to change target to current target == null operation
        if (forcedTarget && forcedTarget == m_targetCombat)
            return;

        if (m_mgr.m_confDebugWhisper)
            TellMaster("Changing target to %s by force!", forcedTarget->GetName());
        m_targetCombat = forcedTarget;
        m_ignoreNeutralizeEffect = true;    // Bypass IsNeutralized() checks on next updates
        m_targetChanged = true;
        m_targetType = (m_combatOrder & (ORDERS_TANK | ORDERS_MAIN_TANK) ? TARGET_THREATEN : TARGET_NORMAL);
    }

    // we already have a target and we are not forced to change it
    if (m_targetCombat)
    {
        // We have a target but it is neutralised and we are not forced to attack it: clear it for now
        if ((IsNeutralized(m_targetCombat) && !m_ignoreNeutralizeEffect))
        {
            m_targetCombat = nullptr;
            m_targetType = TARGET_NORMAL;
            m_targetChanged = true;
            return;
        }
        else
        {
            if (!IsNeutralized(m_targetCombat) && m_ignoreNeutralizeEffect)
                m_ignoreNeutralizeEffect = false;                           // target is no longer neutralised, clear ignore order
            return;                                                         // keep on attacking target
        }
    }

    // No target for now, try to get one
    // do we have to assist someone?
    if (!m_targetCombat && (m_combatOrder & ORDERS_ASSIST) && m_targetAssist)
    {
        candidateTarget = FindAttacker((ATTACKERINFOTYPE)(AIT_VICTIMNOTSELF | AIT_LOWESTTHREAT), m_targetAssist);
        if (candidateTarget && !IsNeutralized(candidateTarget))
        {
            m_targetCombat = candidateTarget;
            if (m_mgr.m_confDebugWhisper)
                TellMaster("Attacking %s to assist %s", m_targetCombat->GetName(), m_targetAssist->GetName());
            m_targetType = (m_combatOrder & (ORDERS_TANK | ORDERS_MAIN_TANK) ? TARGET_THREATEN : TARGET_NORMAL);
            m_targetChanged = true;
        }
    }
    // are there any other attackers?
    if (!m_targetCombat)
    {
        candidateTarget = FindAttacker();
        if (candidateTarget && !IsNeutralized(candidateTarget))
        {
            m_targetCombat = candidateTarget;
            m_targetType = (m_combatOrder & (ORDERS_TANK | ORDERS_MAIN_TANK) ? TARGET_THREATEN : TARGET_NORMAL);
            m_targetChanged = true;
        }
    }
    // no attacker found anyway
    if (!m_targetCombat)
    {
        m_targetType = TARGET_NORMAL;
        m_targetChanged = false;
        return;
    }

    // if thing to attack is in a duel, then ignore and don't call updateAI for 6 seconds
    // this method never gets called when the bot is in a duel and this code
    // prevents bot from helping
    if (m_targetCombat->GetTypeId() == TYPEID_PLAYER && dynamic_cast<Player*>(m_targetCombat)->duel)
    {
        SetIgnoreUpdateTime(6);
        return;
    }

    m_bot->SetSelectionGuid((m_targetCombat->GetObjectGuid()));
    SetIgnoreUpdateTime(1);

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

void PlayerbotAI::GetDuelTarget(Unit* forcedTarget)
{
    // set combat state, and clear looting, etc...
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        m_targetChanged = true;
        m_targetCombat = forcedTarget;
        m_targetType = TARGET_THREATEN;
        m_combatStyle = COMBAT_MELEE;
    }
    m_bot->Attack(m_targetCombat, true);
}

void PlayerbotAI::DoNextCombatManeuver()
{
    if (!GetClassAI())
        return; // error, error...

    if (m_combatOrder == ORDERS_PASSIVE)
        return;

    // check for new targets
    if (m_ScenarioType == SCENARIO_PVP_DUEL)
        GetDuelTarget(GetMaster()); // TODO: Woah... wait... what? So not right.
    else
        Attack();

    // clear orders if current target for attacks doesn't make sense anymore
    if (!m_targetCombat || m_targetCombat->IsDead() || !m_targetCombat->IsInWorld() || !m_bot->CanAttack(m_targetCombat) || !m_bot->IsInMap(m_targetCombat))
    {
        m_bot->AttackStop();
        m_bot->SetSelectionGuid(ObjectGuid());
        MovementReset();
        m_bot->InterruptNonMeleeSpells(true);
        m_targetCombat = nullptr;
        m_targetChanged = false;
        m_targetType = TARGET_NORMAL;
        SetQuestNeedCreatures();
        if (GetCombatOrder() & ORDERS_TEMP)
        {
            if (GetCombatOrder() & ORDERS_TEMP_WAIT_TANKAGGRO)
                TellMaster("I was still waiting for the tank to gain aggro, but that doesn't make sense anymore...");
            if (GetCombatOrder() & ORDERS_TEMP_WAIT_OOC)
                TellMaster("I was still waiting OOC but that was way off...");
            ClearCombatOrder(ORDERS_TEMP);
        }
        return;
    }

    // new target -> DoFirstCombatManeuver
    if (m_targetChanged)
    {
        switch (GetClassAI()->DoFirstCombatManeuver(m_targetCombat))
        {
            case RETURN_CONTINUE: // true needed for rogue stealth attack
                break;

            case RETURN_NO_ACTION_ERROR:
                TellMaster("FirstCombatManeuver: No action performed due to error. Heading onto NextCombatManeuver.");
            case RETURN_FINISHED_FIRST_MOVES: // false default
            case RETURN_NO_ACTION_UNKNOWN:
            case RETURN_NO_ACTION_OK:
            default: // assume no action -> no return
                m_targetChanged = false;
        }
    }

    // do normal combat movement
    DoCombatMovement();

    if (!m_targetChanged)
    {
        // if m_targetChanged = false
        switch (GetClassAI()->DoNextCombatManeuver(m_targetCombat))
        {
            case RETURN_NO_ACTION_UNKNOWN:
            case RETURN_NO_ACTION_OK:
            case RETURN_CONTINUE:
            case RETURN_NO_ACTION_ERROR:
            default:
                return;
        }
    }
}

void PlayerbotAI::DoCombatMovement()
{
    if (!m_targetCombat) return;

    bool meleeReach = m_bot->CanReachWithMeleeAttack(m_targetCombat);

    if (m_combatStyle == COMBAT_MELEE
            && !m_bot->hasUnitState(UNIT_STAT_CHASE)
            && ((m_movementOrder == MOVEMENT_STAY && meleeReach) || m_movementOrder != MOVEMENT_STAY)
            && GetClassAI()->GetWaitUntil() == 0)  // Not waiting
    {
        // melee combat - chase target if in range or if we are not forced to stay
        m_bot->GetMotionMaster()->Clear(false);
        m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
    }
    else if (m_combatStyle == COMBAT_RANGED
             && m_movementOrder != MOVEMENT_STAY
             && GetClassAI()->GetWaitUntil() == 0)  // Not waiting
    {
        // ranged combat - just move within spell range if bot does not have heal orders
        if (!CanReachWithSpellAttack(m_targetCombat) && !IsHealer())
        {
            m_bot->GetMotionMaster()->Clear(false);
            m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
        }
        else
            MovementClear();
    }
}

/*
 * IsGroupReady()
 *
 * return false if any member of the group is in combat or (error handling only) occupied in some way
 * return true otherwise
 */
bool PlayerbotAI::IsGroupReady()
{
    if (!m_bot) return true;
    if (!m_bot->IsAlive() || m_bot->IsInDuel()) return false; // Let's just say you're otherwise occupied
    if (m_bot->IsInCombat()) return false;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (groupMember && groupMember->IsAlive())
            {
                if (groupMember->IsInDuel() || groupMember->IsInCombat())            // all occupied in some way
                    return false;
            }
        }
    }

    return true;
}

Player* PlayerbotAI::GetGroupTank()
{
    if (!m_bot) return nullptr;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        // First loop: we look for a main tank (only bots with JOB_MAIN_TANK) can announce themselves as main tank
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            if (groupMember->GetPlayerbotAI()->IsMainTank())
                return groupMember;
        }
        // Second loop: we look for any tank (bots with JOB_TANK or human players with the right class/spec combination)
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (groupMember)
            {
                if (!groupMember->GetPlayerbotAI() && m_bot->GetPlayerbotAI()->GetClassAI()->GetTargetJob(groupMember) & JOB_TANK)
                    return groupMember;
                else if (groupMember->GetPlayerbotAI() && groupMember->GetPlayerbotAI()->IsTank())
                    return groupMember;
            }
        }
    }

    return nullptr;
}

Player* PlayerbotAI::GetGroupHealer()
{
    if (!m_bot) return nullptr;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            if (groupMember->GetPlayerbotAI()->IsHealer())
                return groupMember;
        }
    }

    return nullptr;
}

void PlayerbotAI::SetGroupCombatOrder(CombatOrderType co)
{
    if (!m_bot) return;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            groupMember->GetPlayerbotAI()->SetCombatOrder(co);
        }
    }
    else
        SetCombatOrder(co);
}

void PlayerbotAI::ClearGroupCombatOrder(CombatOrderType co)
{
    if (!m_bot) return;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            groupMember->GetPlayerbotAI()->ClearCombatOrder(co);
        }
    }
    else
        ClearCombatOrder(co);
}

void PlayerbotAI::SetGroupIgnoreUpdateTime(uint8 t)
{
    if (!m_bot) return;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            groupMember->GetPlayerbotAI()->SetIgnoreUpdateTime(t);
        }
    }
    else
        SetIgnoreUpdateTime(t);
}

bool PlayerbotAI::GroupHoTOnTank()
{
    if (!m_bot) return false;

    bool bReturn = false;

    if (m_bot->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI())
                continue;
            if (groupMember->GetPlayerbotAI()->GetClassAI()->CastHoTOnTank())
                bReturn = true;
        }

        if (bReturn)
        {
            for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
            {
                Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
                if (!groupMember || !groupMember->GetPlayerbotAI())
                    continue;
                groupMember->GetPlayerbotAI()->SetIgnoreUpdateTime(1);
            }
        }
    }
    else // No group
    {
        if (GetClassAI()->CastHoTOnTank())
        {
            SetIgnoreUpdateTime(1);
            return true;
        }
    }

    return bReturn;
}

bool PlayerbotAI::CanPull(Player& fromPlayer)
{
    if (!m_bot) return false;
    if (!GetClassAI()) return false;

    if (!m_bot->GetGroup() || fromPlayer.GetGroup() != m_bot->GetGroup())
    {
        SendWhisper("I can't pull - we're not in the same group.", fromPlayer);
        return false;
    }

    if (!IsGroupReady()) // TODO: add raid support
    {
        SendWhisper("Unable to pull - the group or one of its member is somehow busy.", fromPlayer);
        return false;
    }

    if (!IsTank())
    {
        SendWhisper("I cannot pull as I do not have combat orders to tank or I am not of the proper class.", fromPlayer);
        return false;
    }

    if (((PlayerbotClassAI*)GetClassAI())->CanPull() == false)
    {
        SendWhisper("I cannot pull, I do not have the proper spell or it's not ready yet.", fromPlayer);
        return false;
    }

    return true;
}

// This function assumes a "CanPull()" call was preceded (not doing so will result in odd behavior)
bool PlayerbotAI::CastPull()
{
    if (!m_bot) return false;
    if (!GetClassAI()) return false;
    if (!GetCurrentTarget()) return false;

    if (!IsTank()) return false;

    switch (m_bot->getClass())
    {
        case CLASS_PALADIN:
            return ((PlayerbotPaladinAI*)GetClassAI())->Pull();

        case CLASS_DEATH_KNIGHT:
            return ((PlayerbotDeathKnightAI*)GetClassAI())->Pull();

        case CLASS_DRUID:
            return ((PlayerbotDruidAI*)GetClassAI())->Pull();

        case CLASS_WARRIOR:
            return ((PlayerbotWarriorAI*)GetClassAI())->Pull();

        default:
            return false;
    }

    return false;
}

bool PlayerbotAI::GroupTankHoldsAggro()
{
    if (!m_bot) return false;

    // update attacker info now
    UpdateAttackerInfo();

    if (m_bot->GetGroup())
    {
        Unit* newTarget = FindAttacker((ATTACKERINFOTYPE)(AIT_VICTIMNOTSELF), GetGroupTank());
        if (newTarget)
        {
            return false;
        }
    }
    else
        return false; // no group -> no group tank to hold aggro

    return true;
}

// Wrapper for the UpdateAI cast subfunction
// Each bot class neutralize function will return a spellId
// depending on the creatureType of the target
bool PlayerbotAI::CastNeutralize()
{
    if (!m_bot) return false;
    if (!GetClassAI()) return false;
    if (!m_targetGuidCommand) return false;

    Unit* target = ObjectAccessor::GetUnit(*m_bot, m_targetGuidCommand);
    if (!target) return false;

    uint8 creatureType = 0;
    if (target->GetTypeId() == TYPEID_UNIT)
    {
        // Define the target's creature type, so the bot AI will now if
        // it can neutralize it
        auto* creature = (Creature*) target;
        creatureType = creature->GetCreatureInfo()->CreatureType;
    }
    else    // Target is a player
    {
        // Shapeshifted druids are considered as beasts
        if (target->getClass() == CLASS_DRUID && target->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
            creatureType = CREATURE_TYPE_BEAST;
        // Else, players are humanoids
        else
            creatureType = CREATURE_TYPE_HUMANOID;
    }

    m_spellIdCommand = ((PlayerbotClassAI*)GetClassAI())->Neutralize(creatureType);

    // A spellId was found
    return m_spellIdCommand != 0;
}

void PlayerbotAI::SetQuestNeedCreatures()
{
    // reset values first
    m_needCreatureOrGOList.clear();

    // run through accepted quests, get quest info and data
    for (int qs = 0; qs < MAX_QUEST_LOG_SIZE; ++qs)
    {
        uint32 questid = m_bot->GetQuestSlotQuestId(qs);
        if (questid == 0)
            continue;

        QuestStatusData& qData = m_bot->getQuestStatusMap()[questid];
        // only check quest if it is incomplete
        if (qData.m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
        if (!qInfo)
            continue;

        // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
        {
            if (!qInfo->ReqCreatureOrGOCount[i] || (qInfo->ReqCreatureOrGOCount[i] - qData.m_creatureOrGOcount[i]) <= 0)
                continue;
            m_needCreatureOrGOList[qInfo->ReqCreatureOrGOId[i]] = (qInfo->ReqCreatureOrGOCount[i] - qData.m_creatureOrGOcount[i]);
        }
    }
}

void PlayerbotAI::SetQuestNeedItems()
{
    // reset values first
    m_needItemList.clear();

    // run through accepted quests, get quest info and data
    for (int qs = 0; qs < MAX_QUEST_LOG_SIZE; ++qs)
    {
        uint32 questid = m_bot->GetQuestSlotQuestId(qs);
        if (questid == 0)
            continue;

        QuestStatusData& qData = m_bot->getQuestStatusMap()[questid];
        // only check quest if it is incomplete
        if (qData.m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
        if (!qInfo)
            continue;

        // check for items we not have enough of
        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; i++)
        {
            if (!qInfo->ReqItemCount[i] || (qInfo->ReqItemCount[i] - qData.m_itemcount[i]) <= 0)
                continue;
            m_needItemList[qInfo->ReqItemId[i]] = (qInfo->ReqItemCount[i] - qData.m_itemcount[i]);

            // collect flags not set to gather quest objects skip remaining section
            if (!HasCollectFlag(COLLECT_FLAG_NEAROBJECT) && !HasCollectFlag(COLLECT_FLAG_QUEST))
                continue;

            // TODO: find faster way to handle this look up instead of using SQL lookup for each item
            QueryResult* result;
            // determine if GOs are needed
            result = WorldDatabase.PQuery("SELECT entry FROM gameobject_template WHERE questitem1='%u' "
                                          "OR questitem2='%u' OR questitem3='%u' OR questitem4='%u' OR questitem5='%u' OR questitem6='%u'",
                                          qInfo->ReqItemId[i], qInfo->ReqItemId[i], qInfo->ReqItemId[i], qInfo->ReqItemId[i],
                                          qInfo->ReqItemId[i], qInfo->ReqItemId[i]);

            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    uint32 entry = fields[0].GetUInt32();

                    GameObjectInfo const* gInfo = ObjectMgr::GetGameObjectInfo(entry);
                    if (!gInfo)
                        continue;

                    // add this GO to our collection list if is chest/ore/herb
                    if (gInfo->type == GAMEOBJECT_TYPE_CHEST)
                    {
                        m_collectObjects.push_back(entry);
                        m_collectObjects.sort();
                        m_collectObjects.unique();
                    }
                }
                while (result->NextRow());

                delete result;
            }
        }
    }
}

void PlayerbotAI::SetState(BotState state)
{
    // DEBUG_LOG ("[PlayerbotAI]: SetState - %s switch state %d to %d", m_bot->GetName(), m_botState, state );
    m_botState = state;
}

uint32 PlayerbotAI::GetFreeBagSpace() const
{
    uint32 totalused = 0;
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
            totalused++;
    }
    uint32 totalfree = 16 - totalused;
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            ItemPrototype const* pBagProto = pBag->GetProto();
            if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                totalfree =  totalfree + pBag->GetFreeSlots();
        }
    }
    return totalfree;
}

void PlayerbotAI::DoFlight()
{
    // DEBUG_LOG("[PlayerbotAI]: DoFlight - %s : %s", m_bot->GetName(), m_taxiMaster.GetString().c_str());

    Creature* npc = m_bot->GetNPCIfCanInteractWith(m_taxiMaster, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!npc)
    {
        DEBUG_LOG("[PlayerbotAI]: DoFlight - %s not found or you can't interact with it.", m_taxiMaster.GetString().c_str());
        return;
    }

    m_bot->ActivateTaxiPathTo(m_taxiNodes, npc);
}

void PlayerbotAI::DoLoot()
{
    // clear BOTSTATE_LOOTING if no more loot targets
    if (m_lootCurrent.IsEmpty() && m_lootTargets.empty())
    {
        // DEBUG_LOG ("[PlayerbotAI]: DoLoot - %s is going back to idle", m_bot->GetName());
        SetState(BOTSTATE_NORMAL);
        m_bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
        m_inventory_full = false;
        AutoUpgradeEquipment();
        return;
    }

    // set first in list to current
    if (m_lootCurrent.IsEmpty())
    {
        m_lootCurrent = m_lootTargets.front();
        m_lootTargets.pop_front();
    }

    WorldObject* wo = m_bot->GetMap()->GetWorldObject(m_lootCurrent);

    // clear invalid object or object that is too far from master
    if (!wo || GetMaster()->GetDistance(wo) > float(m_mgr.m_confCollectDistanceMax))
    {
        m_lootCurrent = ObjectGuid();
        if (m_debugWhisper)
            TellMaster("Object is too far away.");
        return;
    }

    Creature* c = nullptr;
    GameObject* go = nullptr;
    if (m_lootCurrent.IsAnyTypeCreature())
        c = static_cast<Creature*>(wo);
    else if (m_lootCurrent.IsGameObject())
        go = static_cast<GameObject*>(wo);

    // clear creature or object that is not spawned or if not creature or object
    if ((c && c->IsDespawned()) || (go && !go->IsSpawned()) || (!c && !go))
    {
        m_lootCurrent = ObjectGuid();
        if (m_debugWhisper)
            TellMaster("Object is not spawned.");
        return;
    }

    uint32 skillId = 0;

    if (c)
    {
        if (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            skillId = c->GetCreatureInfo()->GetRequiredLootSkill();

        // not a lootable creature, clear it
        if ((!c->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE) &&
                !c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE)) ||
                (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE) && !m_bot->HasSkill(skillId)))
        {
            m_lootCurrent = ObjectGuid();
            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear(false);
            m_bot->GetMotionMaster()->MoveIdle();
            return;
        }
        else if (c->m_loot && !c->m_loot->CanLoot(m_bot) && !c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
        {
            if (m_debugWhisper)
                TellMaster("%s is not lootable by me.", wo->GetName());
            m_lootCurrent = ObjectGuid();
            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear(false);
            m_bot->GetMotionMaster()->MoveIdle();
            return;
        }
    }

    // if target contains item being rolled on, do not loot again.
    for (std::list<ObjectGuid>::iterator itr = m_being_rolled_on.begin(); itr != m_being_rolled_on.end(); ++itr)
        if (m_lootCurrent == *itr)
        {
            // DEBUG_LOG("An item is currently being rolled on, ignoring (%s)!",m_lootCurrent.GetString().c_str());
            m_lootCurrent = ObjectGuid();
            return;
        }

    if (m_bot->GetDistance(wo) > CONTACT_DISTANCE + wo->GetObjectBoundingRadius())
    {
        m_bot->GetMotionMaster()->MovePoint(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(), FORCED_MOVEMENT_RUN);
        // give time to move to point before trying again
        SetIgnoreUpdateTime(1);
        if (m_debugWhisper)
            TellMaster("Moving to loot %s", go ? go->GetName() : wo->GetName());
    }

    if (m_bot->GetDistance(wo) < INTERACTION_DISTANCE)
    {
        uint32 reqSkillValue = 0;
        uint32 SkillValue = 0;
        bool keyFailed = false;
        bool skillFailed = false;
        bool forceFailed = false;

        if (m_debugWhisper)
            TellMaster("Beginning to loot %s", go ? go->GetName() : wo->GetName());

        if (c)  // creature
        {
            if (c->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE) && !c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            {
                // loot the creature
                std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_LOOT, 8));
                *packet << m_lootCurrent;
                m_bot->GetSession()->QueuePacket(std::move(packet));
                return; // no further processing is needed
                // m_lootCurrent is reset in SMSG_LOOT_RESPONSE/SMSG_LOOT_RELEASE_RESPONSE
            }
            else if (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
                // not all creature skins are leather, some are ore or herb
                if (m_bot->HasSkill(skillId) && ((skillId != SKILL_SKINNING) ||
                                                 (HasCollectFlag(COLLECT_FLAG_SKIN) && skillId == SKILL_SKINNING)))
                {
                    // calculate skinning skill requirement
                    uint32 targetLevel = c->GetLevel();
                    reqSkillValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;
                }

            // creatures cannot be unlocked or forced open
            keyFailed = true;
            forceFailed = true;
        }

        if (go) // object
        {
            // add this GO to our collection list if active and is chest/ore/herb
            if (go && HasCollectFlag(COLLECT_FLAG_NEAROBJECT) && go->GetGoType() == GAMEOBJECT_TYPE_CHEST)
            {
                m_collectObjects.push_back(go->GetEntry());
                m_collectObjects.sort();
                m_collectObjects.unique();
            }

            uint32 reqItem = 0;

            // check skill or lock on object
            uint32 lockId = go->GetGOInfo()->GetLockId();
            LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);
            if (lockInfo)
                for (int i = 0; i < 8; ++i)
                {
                    if (lockInfo->Type[i] == LOCK_KEY_ITEM)
                    {
                        if (lockInfo->Index[i] > 0)
                        {
                            reqItem = lockInfo->Index[i];
                            if (m_bot->HasItemCount(reqItem, 1))
                                break;
                            continue;
                        }
                    }
                    else if (lockInfo->Type[i] == LOCK_KEY_SKILL)
                    {
                        switch (LockType(lockInfo->Index[i]))
                        {
                            case LOCKTYPE_OPEN:
                                if (CastSpell(3365) == SPELL_CAST_OK)    // Opening
                                    return;
                                break;
                            case LOCKTYPE_CLOSE:
                                if (CastSpell(6233) == SPELL_CAST_OK)    // Closing
                                    return;
                                break;
                            case LOCKTYPE_QUICK_OPEN:
                                if (CastSpell(6247) == SPELL_CAST_OK)    // Opening
                                    return;
                                break;
                            case LOCKTYPE_QUICK_CLOSE:
                                if (CastSpell(6247) == SPELL_CAST_OK)    // Closing
                                    return;
                                break;
                            case LOCKTYPE_OPEN_TINKERING:
                                if (CastSpell(6477) == SPELL_CAST_OK)    // Opening
                                    return;
                                break;
                            case LOCKTYPE_OPEN_KNEELING:
                                if (CastSpell(6478) == SPELL_CAST_OK)    // Opening; listed with 17667 and 22810
                                    return;
                                break;
                            case LOCKTYPE_OPEN_ATTACKING:
                                if (CastSpell(8386) == SPELL_CAST_OK)    // Attacking
                                    return;
                                break;
                            case LOCKTYPE_SLOW_OPEN:
                                if (CastSpell(21651) == SPELL_CAST_OK)   // Opening; also had 26868
                                    return;
                                break;
                            case LOCKTYPE_SLOW_CLOSE:
                                if (CastSpell(21652) == SPELL_CAST_OK)   // Closing
                                    return;
                                break;
                            case LOCKTYPE_OPEN_FROM_VEHICLE:
                                if (CastSpell(61437) == SPELL_CAST_OK)   // Opening
                                    return;
                                break;
                            default:
                                if (SkillByLockType(LockType(lockInfo->Index[i])) > 0)
                                {
                                    skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                                    reqSkillValue = lockInfo->Skill[i];
                                }
                        }
                    }
                }

            // use key on object if available
            if (reqItem > 0 && m_bot->HasItemCount(reqItem, 1))
            {
                UseItem(m_bot->GetItemByEntry(reqItem), TARGET_FLAG_GAMEOBJECT, m_lootCurrent);
                m_lootCurrent = ObjectGuid();
                return;
            }
            else
                keyFailed = true;
        }

        // determine bot's skill value for object's required skill
        if (skillId != SKILL_NONE)
            SkillValue = uint32(m_bot->GetSkillValue(skillId));

        // bot has the specific skill or object requires no skill at all
        if ((m_bot->HasSkill(skillId) && skillId != SKILL_NONE) || (skillId == SKILL_NONE && go))
        {
            if (SkillValue < reqSkillValue)
            {
                TellMaster("My skill is not high enough. It requires %u, but mine is %u.",
                           reqSkillValue, SkillValue);
                skillFailed = true;
            }
            switch (skillId)
            {
                case SKILL_MINING:
                    if (HasTool(TC_MINING_PICK) && CastSpell(MINING) == SPELL_CAST_OK)
                        return;
                    else
                        skillFailed = true;
                    break;
                case SKILL_HERBALISM:
                    if (CastSpell(HERB_GATHERING) == SPELL_CAST_OK)
                        return;
                    else
                        skillFailed = true;
                    break;
                case SKILL_SKINNING:
                    if (c && HasCollectFlag(COLLECT_FLAG_SKIN) &&
                            HasTool(TC_SKINNING_KNIFE) && CastSpell(SKINNING, *c) == SPELL_CAST_OK)
                        return;
                    else
                        skillFailed = true;
                    break;
                case SKILL_LOCKPICKING:
                    if (CastSpell(PICK_LOCK_1) == SPELL_CAST_OK)
                        return;
                    else
                        skillFailed = true;
                    break;
                case SKILL_NONE:
                    if (CastSpell(3365) == SPELL_CAST_OK) //Spell 3365 = Opening?
                        return;
                    else
                        skillFailed = true;
                    break;
                default:
                    TellMaster("I'm not sure how to get that.");
                    skillFailed = true;
                    DEBUG_LOG("[PlayerbotAI]:DoLoot Skill %u is not implemented", skillId);
                    break;
            }
        }
        else
        {
            TellMaster("I do not have the required skill.");
            skillFailed = true;
        }

        if (go) // only go's can be forced
        {
            // if pickable, check if a forcible item is available for the bot
            if (skillId == SKILL_LOCKPICKING && (m_bot->HasSkill(SKILL_BLACKSMITHING) ||
                                                 m_bot->HasSkill(SKILL_ENGINEERING)))
            {
                // check for skeleton keys appropriate for lock value
                if (m_bot->HasSkill(SKILL_BLACKSMITHING))
                {
                    Item* kItem = FindKeyForLockValue(reqSkillValue);
                    if (kItem)
                    {
                        TellMaster("I have a skeleton key that can open it!");
                        UseItem(kItem, TARGET_FLAG_GAMEOBJECT, m_lootCurrent);
                        return;
                    }
                    else
                    {
                        TellMaster("I have no skeleton keys that can open that lock.");
                        forceFailed = true;
                    }
                }

                // check for a charge that can blast it open
                if (m_bot->HasSkill(SKILL_ENGINEERING))
                {
                    Item* bItem = FindBombForLockValue(reqSkillValue);
                    if (bItem)
                    {
                        TellMaster("I can blast it open!");
                        UseItem(bItem, TARGET_FLAG_GAMEOBJECT, m_lootCurrent);
                        return;
                    }
                    else
                    {
                        TellMaster("I have nothing to blast it open with.");
                        forceFailed = true;
                    }
                }
            }
            else
                forceFailed = true;
        }

        // DEBUG_LOG ("[PlayerbotAI]%s: keyFailed [%s]", m_bot->GetName(), keyFailed ? "true" : "false");
        // DEBUG_LOG ("[PlayerbotAI]%s: skillFailed [%s]", m_bot->GetName(), skillFailed ? "true" : "false");
        // DEBUG_LOG ("[PlayerbotAI]%s: forceFailed [%s]", m_bot->GetName(), forceFailed ? "true" : "false");

        // if all attempts failed in some way then clear because it won't get SMSG_LOOT_RESPONSE
        if (keyFailed && skillFailed && forceFailed)
        {
            DEBUG_LOG("[PlayerbotAI]: DoLoot attempts failed on [%s]",
                      go ? go->GetGOInfo()->name : c->GetCreatureInfo()->Name);
            m_lootCurrent = ObjectGuid();

            // remove this GO from our list using the same settings that it was added with earlier
            if (go && HasCollectFlag(COLLECT_FLAG_NEAROBJECT) && go->GetGoType() == GAMEOBJECT_TYPE_CHEST)
                m_collectObjects.remove(go->GetEntry());
        }
        // clear movement target, take next target on next update
        m_bot->GetMotionMaster()->Clear(false);
        m_bot->GetMotionMaster()->MoveIdle();
    }
}

void PlayerbotAI::AcceptQuest(Quest const* qInfo, Player* pGiver)
{
    if (!qInfo || !pGiver)
        return;

    uint32 quest = qInfo->GetQuestId();

    if (!pGiver->CanShareQuest(qInfo->GetQuestId()))
    {
        // giver can't share quest
        m_bot->ClearDividerGuid();
        return;
    }

    if (!m_bot->CanTakeQuest(qInfo, false))
    {
        // can't take quest
        m_bot->ClearDividerGuid();
        return;
    }

    if (m_bot->GetDividerGuid())
    {
        // send msg to quest giving player
        pGiver->SendPushToPartyResponse(m_bot, QUEST_PARTY_MSG_ACCEPT_QUEST);
        m_bot->ClearDividerGuid();
    }

    if (m_bot->CanAddQuest(qInfo, false))
    {
        m_bot->AddQuest(qInfo, pGiver);

        if (m_bot->CanCompleteQuest(quest))
            m_bot->CompleteQuest(quest);

        // build needed items if quest contains any
        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; i++)
            if (qInfo->ReqItemCount[i] > 0)
            {
                SetQuestNeedItems();
                break;
            }

        // build needed creatures if quest contains any
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
            if (qInfo->ReqCreatureOrGOCount[i] > 0)
            {
                SetQuestNeedCreatures();
                break;
            }

        // Runsttren: did not add typeid switch from WorldSession::HandleQuestgiverAcceptQuestOpcode!
        // I think it's not needed, cause typeid should be TYPEID_PLAYER - and this one is not handled
        // there and there is no default case also.

        if (qInfo->GetSrcSpell() > 0)
            m_bot->CastSpell(m_bot, qInfo->GetSrcSpell(), TRIGGERED_OLD_TRIGGERED);
    }
}

void PlayerbotAI::TurnInQuests(WorldObject* questgiver)
{
    ObjectGuid giverGUID = questgiver->GetObjectGuid();

    if (!m_bot->IsInMap(questgiver))
        TellMaster("hey you are turning in quests without me!");
    else
    {
        m_bot->SetSelectionGuid(giverGUID);

        // auto complete every completed quest this NPC has
        m_bot->PrepareQuestMenu(giverGUID);
        QuestMenu& questMenu = m_bot->GetPlayerMenu()->GetQuestMenu();
        for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
        {
            QuestMenuItem const& qItem = questMenu.GetItem(iI);
            uint32 questID = qItem.m_qId;
            Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

            std::ostringstream out;

            QuestStatus status = m_bot->GetQuestStatus(questID);

            // if quest is complete, turn it in
            if (status == QUEST_STATUS_COMPLETE)
            {
                // if bot hasn't already turned quest in
                if (!m_bot->GetQuestRewardStatus(questID))
                {
                    // auto reward quest if no choice in reward
                    if (pQuest->GetRewChoiceItemsCount() == 0)
                    {
                        if (m_bot->CanRewardQuest(pQuest, false))
                        {
                            m_bot->RewardQuest(pQuest, 0, questgiver, false);
                            out << "Quest complete: ";
                        }
                        else
                            out << "|cffff0000Unable to turn quest in:|r ";
                        MakeQuestLink(pQuest, out);
                    }
                    // auto reward quest if one item as reward
                    else if (pQuest->GetRewChoiceItemsCount() == 1)
                    {
                        int rewardIdx = 0;
                        ItemPrototype const* pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        if (m_bot->CanRewardQuest(pQuest, rewardIdx, false))
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, questgiver, true);
                            out << "Quest complete: ";
                            MakeQuestLink(pQuest, out);
                        }
                        else
                            out << "|cffff0000Unable to turn quest in:|r ";
                        MakeQuestLink(pQuest, out);
                        out << " reward: ";
                        MakeItemLink(pRewardItem, out);
                    }
                    // else multiple rewards - let master pick
                    else
                    {
                        out << "What reward should I take for ";
                        MakeQuestLink(pQuest, out);
                        for (uint8 i = 0; i < pQuest->GetRewChoiceItemsCount(); ++i)
                        {
                            ItemPrototype const* const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[i]);
                            MakeItemLink(pRewardItem, out);
                        }
                    }
                }
            }
            else if (status == QUEST_STATUS_INCOMPLETE)
            {
                out << "|cffff0000Quest incomplete:|r ";
                MakeQuestLink(pQuest, out);
            }
            else if (status == QUEST_STATUS_AVAILABLE)
            {
                out << "|cff00ff00Quest available:|r ";
                MakeQuestLink(pQuest, out);
            }

            if (!out.str().empty())
                TellMaster(out.str());
        }
        AutoUpgradeEquipment();
    }
}

bool PlayerbotAI::IsInCombat()
{
    Pet* pet;
    bool inCombat = false;
    inCombat |= m_bot->IsInCombat();
    pet = m_bot->GetPet();
    if (pet)
        inCombat |= pet->IsInCombat();
    inCombat |= GetMaster()->IsInCombat();
    if (m_bot->GetGroup())
    {
        GroupReference* ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            inCombat |= ref->getSource()->IsInCombat();
            pet = ref->getSource()->GetPet();
            if (pet)
                inCombat |= pet->IsInCombat();
            ref = ref->next();
        }
    }
    return inCombat;
}

/**
 * IsRegenerating()
 * return boolean Returns true if bot is having at least one aura associated with regenerating mana (drinking) or health (food)
 *
 * params: null
 * If false is returned, the bot will change his/her stand state to STAND if not already standing.
 *
 */
bool PlayerbotAI::IsRegenerating()
{
    // We check spell category to know if bot is drinking (spell category 59, at least for conjured water)
    // or eating (spell category 11, at least for conjured food or season/battle ground items triggering both food and drink auras)
    // or we check is spell is castable while sitting and provide health/power regen like food/drink spells do
    Unit::SpellAuraHolderMap& auras = m_bot->GetSpellAuraHolderMap();
    for (auto & aura : auras)
    {
        SpellEntry const* spell = aura.second->GetSpellProto();
        // Exclude permanent regenerating aura (DurationIndex 21 : unlimited)
        if (!spell || spell->DurationIndex == 21)
            continue;
        // Generic drinking / eating
        if (spell->Category == 59 || spell->Category == 11)
            return true;
        // Specific drinking / eating after patch 2.0.1, especially conjured goods
        if(spell->HasAttribute(SPELL_ATTR_CASTABLE_WHILE_SITTING))
        {
            for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
            {
                if (spell->Effect[i] == SPELL_EFFECT_APPLY_AURA && (spell->EffectApplyAuraName[i] == SPELL_AURA_MOD_POWER_REGEN || spell->EffectApplyAuraName[i] == SPELL_AURA_MOD_REGEN))
                    return true;
            }
        }
    }
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
    return false;
}

void PlayerbotAI::UpdateAttackersForTarget(Unit* victim)
{
    HostileReference* ref = victim->getHostileRefManager().getFirst();
    while (ref)
    {
        ThreatManager* target = ref->getSource();
        ObjectGuid guid = target->getOwner()->GetObjectGuid();
        m_attackerInfo[guid].attacker = target->getOwner();
        m_attackerInfo[guid].victim = target->getOwner()->GetVictim();
        m_attackerInfo[guid].threat = target->getThreat(victim);
        m_attackerInfo[guid].count = 1;
        //m_attackerInfo[guid].source = 1; // source is not used so far.
        ref = ref->next();
    }
}

void PlayerbotAI::UpdateAttackerInfo()
{
    // clear old list
    m_attackerInfo.clear();

    // check own attackers
    UpdateAttackersForTarget(m_bot);
    Pet* pet = m_bot->GetPet();
    if (pet)
        UpdateAttackersForTarget(pet);

    // check master's attackers
    UpdateAttackersForTarget(GetMaster());
    pet = GetMaster()->GetPet();
    if (pet)
        UpdateAttackersForTarget(pet);

    // check all group members now
    if (m_bot->GetGroup())
    {
        GroupReference* gref = m_bot->GetGroup()->GetFirstMember();
        while (gref)
        {
            if (gref->getSource() == m_bot || gref->getSource() == GetMaster())
            {
                gref = gref->next();
                continue;
            }

            UpdateAttackersForTarget(gref->getSource());
            pet = gref->getSource()->GetPet();
            if (pet)
                UpdateAttackersForTarget(pet);

            gref = gref->next();
        }
    }

    // get highest threat not caused by bot for every entry in AttackerInfoList...
    for (AttackerInfoList::iterator itr = m_attackerInfo.begin(); itr != m_attackerInfo.end(); ++itr)
    {
        if (!itr->second.attacker)
            continue;
        Unit* a = itr->second.attacker;
        float t = 0.00;
        std::list<HostileReference*>::const_iterator i = a->getThreatManager().getThreatList().begin();
        for (; i != a->getThreatManager().getThreatList().end(); ++i)
        {
            if ((*i)->getThreat() > t && (*i)->getTarget() != m_bot)
                t = (*i)->getThreat();
        }
        m_attackerInfo[itr->first].threat2 = t;
    }

    // DEBUG: output attacker info
    //sLog.outBasic( "[PlayerbotAI]: %s m_attackerInfo = {", m_bot->GetName() );
    //for( AttackerInfoList::iterator i=m_attackerInfo.begin(); i!=m_attackerInfo.end(); ++i )
    //    sLog.outBasic( "[PlayerbotAI]:     [%016I64X] { %08X, %08X, %.2f, %.2f, %d, %d }",
    //        i->first,
    //        (i->second.attacker?i->second.attacker->GetGUIDLow():0),
    //        (i->second.victim?i->second.victim->GetGUIDLow():0),
    //        i->second.threat,
    //        i->second.threat2,
    //        i->second.count,
    //        i->second.source );
    //sLog.outBasic( "[PlayerbotAI]: };" );
}

uint32 PlayerbotAI::EstRepairAll()
{
    uint32 TotalCost = 0;
    // equipped, backpack, bags itself
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        TotalCost += EstRepair(((INVENTORY_SLOT_BAG_0 << 8) | i));

    // bank, buyback and keys not repaired

    // items in inventory bags
    for (int j = INVENTORY_SLOT_BAG_START; j < INVENTORY_SLOT_BAG_END; ++j)
        for (int i = 0; i < MAX_BAG_SIZE; ++i)
            TotalCost += EstRepair(((j << 8) | i));
    return TotalCost;
}

uint32 PlayerbotAI::EstRepair(uint16 pos)
{
    Item* item = m_bot->GetItemByPos(pos);

    uint32 TotalCost = 0;
    if (!item)
        return TotalCost;

    uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
    if (!maxDurability)
        return TotalCost;

    uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

    uint32 LostDurability = maxDurability - curDurability;
    if (LostDurability > 0)
    {
        ItemPrototype const* ditemProto = item->GetProto();

        DurabilityCostsEntry const* dcost = sDurabilityCostsStore.LookupEntry(ditemProto->ItemLevel);
        if (!dcost)
        {
            sLog.outError("RepairDurability: Wrong item lvl %u", ditemProto->ItemLevel);
            return TotalCost;
        }

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const* dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
        {
            sLog.outError("RepairDurability: Wrong dQualityModEntry %u", dQualitymodEntryId);
            return TotalCost;
        }

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];
        uint32 costs = uint32(LostDurability * dmultiplier * double(dQualitymodEntry->quality_mod));

        if (costs == 0)                                 //fix for ITEM_QUALITY_ARTIFACT
            costs = 1;

        TotalCost = costs;
    }
    return TotalCost;
}

Unit* PlayerbotAI::FindAttacker(ATTACKERINFOTYPE ait, Unit* victim)
{
    // list empty? why are we here?
    if (m_attackerInfo.empty())
        return nullptr;

    // not searching something specific - return first in list
    if (!ait)
        return (m_attackerInfo.begin())->second.attacker;

    float t = ((ait & AIT_HIGHESTTHREAT) ? 0.00 : 9999.00);
    Unit* a = nullptr;
    AttackerInfoList::iterator itr = m_attackerInfo.begin();
    for (; itr != m_attackerInfo.end(); ++itr)
    {
        if ((ait & AIT_VICTIMSELF) && !(ait & AIT_VICTIMNOTSELF) && itr->second.victim != m_bot)
            continue;

        if (!(ait & AIT_VICTIMSELF) && (ait & AIT_VICTIMNOTSELF) && itr->second.victim == m_bot)
            continue;

        if ((ait & AIT_VICTIMNOTSELF) && victim && itr->second.victim != victim)
            continue;

        if (!(ait & (AIT_LOWESTTHREAT | AIT_HIGHESTTHREAT)))
            return itr->second.attacker;
        else
        {
            if ((ait & AIT_HIGHESTTHREAT) && /*(itr->second.victim==m_bot) &&*/ itr->second.threat >= t)
            {
                t = itr->second.threat;
                a = itr->second.attacker;
            }
            else if ((ait & AIT_LOWESTTHREAT) && /*(itr->second.victim==m_bot) &&*/ itr->second.threat <= t)
            {
                t = itr->second.threat;
                a = itr->second.attacker;
            }
        }
    }
    return a;
}

/**
* BotDataRestore()
* Restores autoequip - the toggle status for the 'equip auto' command.
* Restores gDelayAttack - the other attributes need a valid target. This function is to be called when the targets
* may or may not be online (such as upon login). See CombatOrderRestore() for full orders restore.
*/
void PlayerbotAI::BotDataRestore()
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT combat_delay,autoequip FROM playerbot_saved_data WHERE guid = '%u'", m_bot->GetGUIDLow());

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> [BotDataRestore()] Loaded `playerbot_saved_data`, found no match for guid %u.", m_bot->GetGUIDLow());
        m_DelayAttack = 0;
        return;
    }
    else
    {
        Field* fields = result->Fetch();
        m_DelayAttack = fields[0].GetUInt8();
        m_AutoEquipToggle = fields[1].GetBool();
        delete result;
    }
}

/**
* CombatOrderRestore()
* Restores all saved attributes. This function is to be called when the targets are assumed to be online.
* Restores m_DelayAttack - the other attributes need a valid target. This function is to be called when the targets
*/
void PlayerbotAI::CombatOrderRestore()
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT combat_order,primary_target,secondary_target,pname,sname,combat_delay,auto_follow FROM playerbot_saved_data WHERE guid = '%u'", m_bot->GetGUIDLow());

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> [CombatOrderRestore()] Loaded `playerbot_saved_data`, found no match for guid %u.", m_bot->GetGUIDLow());
        TellMaster("I have no orders");
        return;
    }

    Field* fields = result->Fetch();
    CombatOrderType combatOrders = (CombatOrderType)fields[0].GetUInt32();
    ObjectGuid PrimtargetGUID = ObjectGuid(fields[1].GetUInt64());
    ObjectGuid SectargetGUID = ObjectGuid(fields[2].GetUInt64());
    std::string pname = fields[3].GetString();
    std::string sname = fields[4].GetString();
    m_DelayAttack = fields[5].GetUInt8();
    m_FollowAutoGo = fields[6].GetUInt8();
    gPrimtarget = ObjectAccessor::GetUnit(*m_bot->GetMap()->GetWorldObject(PrimtargetGUID), PrimtargetGUID);
    gSectarget = ObjectAccessor::GetUnit(*m_bot->GetMap()->GetWorldObject(SectargetGUID), SectargetGUID);
    delete result;

    //Unit* target = nullptr;
    //ObjectGuid NoTargetGUID = m_bot->GetObjectGuid();
    //target = ObjectAccessor::GetUnit(*m_bot, NoTargetGUID);
    if (m_FollowAutoGo == FOLLOWAUTOGO_OFF)
    {
        DistOverRide = 0; //set initial adjustable follow settings
        IsUpOrDown = 0;
        gTempDist = 0.5f;
        gTempDist2 = 1.0f;
        SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
    }

    if (combatOrders & ORDERS_PRIMARY) SetCombatOrder(combatOrders, gPrimtarget);
    if (combatOrders & ORDERS_SECONDARY) SetCombatOrder(combatOrders, gSectarget);
}

void PlayerbotAI::SetCombatOrderByStr(std::string str, Unit* target)
{
    CombatOrderType co;
    if (str == "tank")              co = ORDERS_TANK;
    else if (str == "maintank")     co = ORDERS_MAIN_TANK;
    else if (str == "assist")       co = ORDERS_ASSIST;
    else if (str == "heal")         co = ORDERS_HEAL;
    else if (str == "mainheal")     co = ORDERS_MAIN_HEAL;
    else if (str == "notmainheal")  co = ORDERS_NOT_MAIN_HEAL;
    else if (str == "protect")      co = ORDERS_PROTECT;
    else if (str == "passive")      co = ORDERS_PASSIVE;
    else if (str == "pull")         co = ORDERS_TEMP_WAIT_TANKAGGRO;
    else if (str == "nodispel")     co = ORDERS_NODISPEL;
    else if (str == "resistfrost")  co = ORDERS_RESIST_FROST;
    else if (str == "resistnature") co = ORDERS_RESIST_NATURE;
    else if (str == "resistfire")   co = ORDERS_RESIST_FIRE;
    else if (str == "resistshadow") co = ORDERS_RESIST_SHADOW;
    else                            co = ORDERS_RESET;

    SetCombatOrder(co, target);
    if (m_FollowAutoGo != FOLLOWAUTOGO_OFF)
        m_FollowAutoGo = FOLLOWAUTOGO_INIT;
}

void PlayerbotAI::SetCombatOrder(CombatOrderType co, Unit* target)
{
    uint32 gTempTarget;
    std::string gname;
    if (target)
    {
        gTempTarget = target->GetGUIDLow();
        gname = target->GetName();
    }

    // reset m_combatOrder after ORDERS_PASSIVE
    if (m_combatOrder == ORDERS_PASSIVE)
    {
        m_combatOrder = ORDERS_NONE;
        m_targetAssist = nullptr;
        m_targetProtect = nullptr;
    }

    switch (co)
    {
        case ORDERS_TANK:   // 1(01)
        {
            if (m_combatOrder & ORDERS_MAIN_TANK)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_MAIN_TANK);  // ORDERS_TANK and ORDERS_MAIN_TANK exclude one each other, remove one when the other is set
            break;
        }
        case ORDERS_ASSIST: // 2(10)
        {
            if (!target)
            {
                TellMaster("The assist command requires a target.");
                return;
            }
            else m_targetAssist = target;
            break;
        }
        case ORDERS_HEAL:    // 4(100)
        {
            if (m_combatOrder & ORDERS_MAIN_HEAL)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_MAIN_HEAL);  // ORDERS_HEAL and ORDERS_MAIN_HEAL exclude one each other, remove one when the other is set
            break;
        }
        case ORDERS_PROTECT: // 10(10000)
        {
            if (!target)
            {
                TellMaster("The protect command requires a target.");
                return;
            }
            else m_targetProtect = target;
            break;
        }
        case ORDERS_PASSIVE: // 20(100000)
        {
            m_combatOrder = ORDERS_PASSIVE;
            m_targetAssist = nullptr;
            m_targetProtect = nullptr;
            return;
        }
        case ORDERS_MAIN_TANK:  // 1000(1000000000000)
        {
            if (m_combatOrder & ORDERS_TANK)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_TANK);   // ORDERS_TANK and ORDERS_MAIN_TANK exclude one each other, remove one when the other is set
            break;
        }
        case ORDERS_MAIN_HEAL:  // 2000(10000000000000)
        {
            if (m_combatOrder & ORDERS_HEAL)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_HEAL);   // ORDERS_HEAL and ORDERS_MAIN_HEAL exclude one each other, remove one when the other is set
            if (m_combatOrder & ORDERS_NOT_MAIN_HEAL)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_NOT_MAIN_HEAL);  // ORDERS_NOT_MAIN_HEAL and ORDERS_MAIN_HEAL exclude one each other, remove one when the other is set
            break;
        }
        case ORDERS_NOT_MAIN_HEAL:  // 4000(100000000000000)
        {
            if (m_combatOrder & ORDERS_MAIN_HEAL)
                m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~ORDERS_MAIN_HEAL);  // ORDERS_NOT_MAIN_HEAL and ORDERS_MAIN_HEAL exclude one each other, remove one when the other is set
            break;
        }
        case ORDERS_RESET: // FFFF(1111111111111111)
        {
            m_combatOrder = ORDERS_NONE;
            m_targetAssist = nullptr;
            m_targetProtect = nullptr;
            m_DelayAttackInit = CurrentTime();
            m_DelayAttack = 0;
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_order = 0, primary_target = 0, secondary_target = 0, pname = '',sname = '', combat_delay = 0 WHERE guid = '%u'", m_bot->GetGUIDLow());
            TellMaster("Orders are cleaned!");
            return;
        }
        default:
            break;
    }

    // Do your magic
    if ((co & ORDERS_PRIMARY))
    {
        m_combatOrder = (CombatOrderType)(((uint32) m_combatOrder & (uint32) ORDERS_SECONDARY) | (uint32) co);
        if (target)
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_order = '%u', primary_target = '%u', pname = '%s' WHERE guid = '%u'", (m_combatOrder & ~ORDERS_TEMP), gTempTarget, gname.c_str(), m_bot->GetGUIDLow());
        else
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_order = '%u' WHERE guid = '%u'", (m_combatOrder & ~ORDERS_TEMP), m_bot->GetGUIDLow());
    }
    else
    {
        m_combatOrder = (CombatOrderType)((uint32)m_combatOrder | (uint32)co);
        if (target)
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_order = '%u', secondary_target = '%u', sname = '%s' WHERE guid = '%u'", (m_combatOrder & ~ORDERS_TEMP), gTempTarget, gname.c_str(), m_bot->GetGUIDLow());
        else
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_order = '%u' WHERE guid = '%u'", (m_combatOrder & ~ORDERS_TEMP), m_bot->GetGUIDLow());
    }
}

void PlayerbotAI::ClearCombatOrder(CombatOrderType co)
{
    m_combatOrder = (CombatOrderType)((uint32) m_combatOrder & (uint32) ~co);

    switch (co)
    {
        case ORDERS_NONE:
        case ORDERS_TANK:
        case ORDERS_MAIN_TANK:
        case ORDERS_ASSIST:
        case ORDERS_HEAL:
        case ORDERS_MAIN_HEAL:
        case ORDERS_PASSIVE:
        case ORDERS_PRIMARY:
        case ORDERS_RESET:
        case ORDERS_SECONDARY:
            SetCombatOrder(ORDERS_RESET);
            return;

        default:
            return;
    }
}

void PlayerbotAI::SetMovementOrder(MovementOrderType mo, Unit* followTarget)
{
    m_movementOrder = mo;
    m_followTarget = followTarget;
    if (m_botState != BOTSTATE_LOADING)
        MovementReset();
}

void PlayerbotAI::MovementReset()
{
    // stop moving...
    MovementClear();
    if (m_movementOrder == MOVEMENT_FOLLOW)
    {
        if (!m_followTarget)
            return;

        // don't follow while in combat
        if (m_bot->IsInCombat())
            return;

        Player* pTarget;                            // target is player
        if (m_followTarget->GetTypeId() == TYPEID_PLAYER)
            pTarget = ((Player*) m_followTarget);

        if (pTarget)
        {
            // check player for follow situations
            if (pTarget->IsBeingTeleported() || pTarget->IsTaxiFlying() || pTarget->GetCorpse())
                return;
        }

        // is bot too far from the follow target
        if (!m_bot->IsWithinDistInMap(m_followTarget, 50))
        {
            DoTeleport(*m_followTarget);
            return;
        }

        if (m_bot->IsAlive() && !m_bot->IsBeingTeleported())
        {
            if (DistOverRide != 0)
            {
                if (m_FollowAutoGo == FOLLOWAUTOGO_OFF)
                {
                    if (IsUpOrDown < DistOverRide)
                    {
                        IsUpOrDown = DistOverRide;
                        gTempDist = (gTempDist + 1.0);
                        gTempDist2 = (gTempDist2 + 1.0);
                    }
                    if (IsUpOrDown > DistOverRide)
                    {
                        if (IsUpOrDown >= 2)
                        {
                            IsUpOrDown = DistOverRide;
                            gTempDist = (gTempDist - 1.0);
                            gTempDist2 = (gTempDist2 - 1.0);
                        }
                    }
                }
                else
                {
                    if (IsUpOrDown < DistOverRide)
                    {
                        gTempDist = 1.0;
                        gTempDist2 = 2.0;
                        IsUpOrDown = 0;
                        for (IsUpOrDown = 0; IsUpOrDown < DistOverRide; ++IsUpOrDown)
                        {
                            gTempDist = (gTempDist + 1.0);
                            gTempDist2 = (gTempDist2 + 1.0);
                        }
                        IsUpOrDown = DistOverRide;
                    }
                    else if (IsUpOrDown > DistOverRide)
                    {
                        uint8 getdowndist = (IsUpOrDown - DistOverRide);
                        for (uint8 getdowndistb = 0; getdowndistb < getdowndist; ++getdowndistb)
                        {
                            gTempDist = (gTempDist - 1.0);
                            gTempDist2 = (gTempDist2 - 1.0);
                        }
                        IsUpOrDown = DistOverRide;
                    }
                }
                if (m_FollowAutoGo != FOLLOWAUTOGO_RESET)
                {
                    gDist[0] = gTempDist;
                    gDist[1] = gTempDist2;
                }
                else
                {
                    gDist[0] = 0.5f;
                    gDist[1] = 1.0f;
                    SetIgnoreUpdateTime(3);
                    m_FollowAutoGo = FOLLOWAUTOGO_INIT;
                }
            }
            float dist = rand_float(m_mgr.m_confFollowDistance[0], m_mgr.m_confFollowDistance[1]);
            float bdist = rand_float(gDist[0], gDist[1]);
            float angle = rand_float(0, M_PI_F);
            float bangle = rand_float(2.8f, 3.6f); // angle is based on radians
            float TankAngle = 3.1f;
            float AssistAngle = 2.8f;
            if (DistOverRide != 0)
            {
                if (m_combatOrder & ORDERS_TANK)
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, bdist, TankAngle);
                else if (m_combatOrder & ORDERS_ASSIST)
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, bdist, AssistAngle);
                else
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, bdist, bangle);
            }
            else
                m_bot->GetMotionMaster()->MoveFollow(m_followTarget, dist, angle);
            if (m_FollowAutoGo == FOLLOWAUTOGO_RUN)
                m_FollowAutoGo = FOLLOWAUTOGO_INIT;
        }
    }
}

void PlayerbotAI::MovementClear()
{
    // stop...
    m_bot->clearUnitState(UNIT_STAT_FOLLOW);
    m_bot->GetMotionMaster()->Initialize();

    // stand up...
    if (!m_bot->IsStandState())
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

void PlayerbotAI::PlaySound(uint32 soundid)
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << soundid;
    GetMaster()->GetSession()->SendPacket(data);
}

// PlaySound data from SoundEntries.dbc
void PlayerbotAI::Announce(AnnounceFlags msg)
{
    switch (m_bot->getRace())
    {
        case RACE_HUMAN:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(1908) : PlaySound(2032); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(1875) : PlaySound(1999); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(1924) : PlaySound(2048); break;
                default: break;
            }
            break;
        case RACE_ORC:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(2319) : PlaySound(2374); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(2284) : PlaySound(2341); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(2335) : PlaySound(2390); break;
                default: break;
            }
            break;
        case RACE_DWARF:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(1630) : PlaySound(1686); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(1581) : PlaySound(1654); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(1636) : PlaySound(1702); break;
                default: break;
            }
            break;
        case RACE_NIGHTELF:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(2151) : PlaySound(2262); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(2118) : PlaySound(2229); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(2167) : PlaySound(2278); break;
                default: break;
            }
            break;
        case RACE_UNDEAD:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(2096) : PlaySound(2207); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(2054) : PlaySound(2173); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(2112) : PlaySound(2223); break;
                default: break;
            }
            break;
        case RACE_TAUREN:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(2463) : PlaySound(2462); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(2396) : PlaySound(2397); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(2495) : PlaySound(2494); break;
                default: break;
            }
            break;
        case RACE_GNOME:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(1743) : PlaySound(1798); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(1708) : PlaySound(1709); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(1759) : PlaySound(1814); break;
                default: break;
            }
            break;
        case RACE_TROLL:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(1853) : PlaySound(1963); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(1820) : PlaySound(1930); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(1869) : PlaySound(1993); break;
                default: break;
            }
            break;
        case RACE_BLOODELF:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(9583) : PlaySound(9584); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(9549) : PlaySound(9550); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(9565) : PlaySound(9566); break;
                default: break;
            }
            break;
        case RACE_DRAENEI:
            switch (msg)
            {
                case CANT_AFFORD: m_bot->getGender() == GENDER_MALE ? PlaySound(9498) : PlaySound(9499); break;
                case INVENTORY_FULL: m_bot->getGender() == GENDER_MALE ? PlaySound(9465) : PlaySound(9466); break;
                case CANT_USE_TOO_FAR: m_bot->getGender() == GENDER_MALE ? PlaySound(9481) : PlaySound(9482); break;
                default: break;
            }
            break;
        default:
            break;
    }
}

// some possible things to use in AI
// GetRandomContactPoint
// GetPower, GetMaxPower
// HasSpellCooldown
// IsAffectedBySpellmod
// isMoving
// hasUnitState(FLAG) FLAG like: UNIT_STAT_ROOT, UNIT_STAT_CONFUSED, UNIT_STAT_STUNNED
// hasAuraType

void PlayerbotAI::UpdateAI(const uint32 /*p_time*/)
{
    if (GetClassAI()->GetWaitUntil() <= CurrentTime())
        GetClassAI()->ClearWait();

    if (CurrentTime() < m_ignoreAIUpdatesUntilTime)
        return;

    // default updates occur every two seconds
    SetIgnoreUpdateTime(2);
    
    if (m_botState == BOTSTATE_LOADING)
    {
        if (m_bot->IsBeingTeleported())
            return;
        else
        {
            // is bot too far from the follow target
            if (!m_bot->IsWithinDistInMap(m_followTarget, 50))
            {
                DoTeleport(*m_followTarget);
                return;
            }
            else
                SetState(BOTSTATE_NORMAL);

            return;
        }
    }

    if (m_bot->IsBeingTeleported() || m_bot->GetTrader())
        return;

    if (m_FollowAutoGo == FOLLOWAUTOGO_INIT)
    {
        if (m_combatOrder & ORDERS_TANK)
            DistOverRide = 1;
        else if (m_combatOrder & ORDERS_ASSIST)
            DistOverRide = 3;
        else
            DistOverRide = 4;
        m_FollowAutoGo = FOLLOWAUTOGO_SET;
        SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
    }

    if (!m_bot->IsAlive())
    {
        if (m_botState == BOTSTATE_DEAD)
        {
            // become ghost
            if (m_bot->GetCorpse())
            {
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s already has a corpse...", m_bot->GetName() );
                SetState(BOTSTATE_DEADRELEASED);
                return;
            }
            m_bot->SetBotDeathTimer();
            m_bot->BuildPlayerRepop();
            // relocate ghost
            WorldLocation loc;
            Corpse* corpse = m_bot->GetCorpse();
            corpse->GetPosition(loc);
            m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
            // set state to released
            SetState(BOTSTATE_DEADRELEASED);

            return;
        }

        if (m_botState == BOTSTATE_DEADRELEASED)
        {
            // get bot's corpse
            Corpse* corpse = m_bot->GetCorpse();
            if (!corpse)
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s has no corpse!", m_bot->GetName() );
                return;

            // check if we are allowed to resurrect now
            time_t resurrect_time = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP);
            if (resurrect_time > CurrentTime())
            {
                SetIgnoreUpdateTime(resurrect_time);
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s has to wait for %d seconds to revive...", m_bot->GetName(), m_ignoreAIUpdatesUntilTime-CurrentTime());
                return;
            }
            // resurrect now
            // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - Reviving %s to corpse...", m_bot->GetName() );

            SetIgnoreUpdateTime(6);

            PlayerbotChatHandler ch(GetMaster());
            if (!ch.revive(*m_bot))
            {
                ch.sysmessage(".. could not be revived ..");
                return;
            }
            // set back to normal
            SetState(BOTSTATE_NORMAL);

            return;
        }

        // if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s died and is not in correct state...", m_bot->GetName());
        // clear loot list on death
        m_lootTargets.clear();
        m_lootCurrent = ObjectGuid();
        // clear combat orders
        m_bot->SetSelectionGuid(ObjectGuid());
        m_bot->GetMotionMaster()->Clear(true);
        // set state to dead
        SetState(BOTSTATE_DEAD);
        // wait 30sec
        SetIgnoreUpdateTime(30);

        return;
    }

    // If bot is in water, allow it to swim instead of being stuck above water or at the floor until it drowns itself
    if (m_bot->IsInWater() && m_bot->GetMap()->GetTerrain()->IsSwimmable(m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), m_bot->GetCollisionHeight()))
    {
        if (!m_bot->IsSwimming())
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_SWIMMING);
    }
    else if (m_bot->IsSwimming())   // Clear swimming when going out of water
        m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_SWIMMING);
    
    // bot still alive
    if (!m_findNPC.empty())
        findNearbyCreature();

    // if we are casting a spell then interrupt it
    // unless it is a positive spell then bot is helping a friendly unit and the death of an enemy target should not prevent this
    // make sure any actions that cast a spell set a proper m_ignoreAIUpdatesUntilTime!
    Spell* const pSpell = GetCurrentSpell();
    if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()) && !IsPositiveSpell(pSpell->m_spellInfo->Id))
    {
        // DEBUG_LOG("spell (%s) is being interrupted",pSpell->m_spellInfo->SpellName[0]);
        InterruptCurrentCastingSpell();
        return;
    }

    if (m_botState == BOTSTATE_TAME)
    {
        Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, m_targetGuidCommand);
        if (!pTarget)
            return;

        m_bot->SetSelectionGuid(m_targetGuidCommand);

        if (!In_Range(pTarget, TAME_BEAST_1))
            m_bot->clearUnitState(UNIT_STAT_CHASE);

        if (!m_bot->hasUnitState(UNIT_STAT_CHASE))
        {
            m_bot->GetMotionMaster()->MoveChase(pTarget);
            return;
        }

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(TAME_BEAST_1);
        if (!spellInfo)
            return;

        Spell* spell = new Spell(m_bot, spellInfo, false);
        if (!spell)
            return;

        if (m_bot->GetPetGuid() || spell->CheckCast(true) != SPELL_CAST_OK || !pTarget ||
                pTarget->IsDead() || !m_bot->IsInMap(pTarget) || !(((Creature*) pTarget)->GetCreatureInfo()->CreatureTypeFlags & CREATURE_TYPEFLAGS_TAMEABLE))
        {
            MovementReset();
            m_bot->SetSelectionGuid(ObjectGuid());
            SetState(BOTSTATE_NORMAL);
            SetIgnoreUpdateTime(0);
        }
        else if (!m_bot->HasAura(TAME_BEAST_1, EFFECT_INDEX_1))
        {
            m_bot->SetFacingTo(m_bot->GetAngle(pTarget));
            SpellCastTargets targets;
            targets.setUnitTarget(pTarget);
            spell->SpellStart(&targets);
            SetIgnoreUpdateTime(10);
        }

        return;
    }

    // direct cast command from master
    if (m_spellIdCommand != 0)
    {
        Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, m_targetGuidCommand);
        if (pTarget)
            CastSpell(m_spellIdCommand, *pTarget);
        m_spellIdCommand = 0;
        m_targetGuidCommand = ObjectGuid();
        return;
    }

    if (m_botState == BOTSTATE_DELAYED)
    {
        if (m_CraftSpellId == 0)
        {
            SetState(BOTSTATE_NORMAL);
            AutoUpgradeEquipment();
            return;
        }

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(m_CraftSpellId);
        if (!spellInfo)
            return;

        Spell* spell = new Spell(m_bot, spellInfo, false);
        if (!spell)
            return;

        if (GetSpellCharges(m_CraftSpellId) == 0 || spell->CheckCast(true) != SPELL_CAST_OK)
        {
            SetState(BOTSTATE_NORMAL);
            SetIgnoreUpdateTime(0);
            AutoUpgradeEquipment();
            m_CraftSpellId = 0;
        }
        else
        {
            SpellCastTargets targets;
            spell->SpellStart(&targets);
            SetIgnoreUpdateTime(6);
        }

        return;
    }

    //if master is unmounted, unmount the bot
    if (!GetMaster()->IsMounted() && m_bot->IsMounted())
    {
        WorldPacket emptyPacket;
        m_bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);  //updated code
        return;
    }

    // handle combat (either self/master/group in combat, or combat state and valid target)
    if (IsInCombat() || (m_botState == BOTSTATE_COMBAT && m_targetCombat) ||  m_ScenarioType == SCENARIO_PVP_DUEL)
    {
        //check if the bot is Mounted
        if (!m_bot->IsMounted())
        {
            if (!pSpell || !pSpell->IsChannelActive())
            {
                // DEBUG_LOG("m_DelayAttackInit (%li) + m_DelayAttack (%u) > time(%li)", m_DelayAttackInit, m_DelayAttack, CurrentTime());
                if (m_DelayAttackInit + m_DelayAttack > CurrentTime())
                    return SetIgnoreUpdateTime(1); // short bursts of delay

                return DoNextCombatManeuver();
            }
            else // channelling a spell
                return SetIgnoreUpdateTime(0);  // It's better to update AI more frequently during combat
        }

        return;
    }

    // bot was in combat recently - loot now
    if (m_botState == BOTSTATE_COMBAT)
    {
        if (GetCombatOrder() & ORDERS_TEMP)
        {
            if (GetCombatOrder() & ORDERS_TEMP_WAIT_TANKAGGRO)
                TellMaster("I was still waiting for the tank to gain aggro, but that doesn't make sense anymore...");
            if (GetCombatOrder() & ORDERS_TEMP_WAIT_OOC)
                TellMaster("I was still waiting OOC but I just got out of combat...");
            ClearCombatOrder(ORDERS_TEMP);
        }
        SetState(BOTSTATE_LOOTING);
        m_attackerInfo.clear();
        if (HasCollectFlag(COLLECT_FLAG_COMBAT))
            m_lootTargets.unique();
        else
            m_lootTargets.clear();
    }

    if (m_botState == BOTSTATE_LOOTING)
        return DoLoot();

    if (m_botState == BOTSTATE_FLYING)
    {
        /* std::ostringstream out;
        out << "Taxi: " << m_bot->GetName() << m_ignoreAIUpdatesUntilTime;
        TellMaster(out.str().c_str()); */
        DoFlight();
        SetState(BOTSTATE_NORMAL);
        SetIgnoreUpdateTime(0);
        return;
    }

    // if commanded to follow master and not already following master then follow master
    if (!m_bot->IsInCombat() && m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
        return MovementReset();

    // do class specific non combat actions
    if (GetClassAI() && !m_bot->IsMounted() && !IsRegenerating())
    {
        GetClassAI()->DoNonCombatActions();

        // have we been told to collect loot after combat
        if (HasCollectFlag(COLLECT_FLAG_LOOT))
        {
            findNearbyCorpse();
            // start looting if have targets
            if (!m_lootTargets.empty())
                SetState(BOTSTATE_LOOTING);
        }

        // have we been told to collect GOs
        if (HasCollectFlag(COLLECT_FLAG_NEAROBJECT))
        {
            findNearbyGO();
            // start looting if have targets
            if (!m_lootTargets.empty())
                SetState(BOTSTATE_LOOTING);
        }
        return;
    }
}

Spell* PlayerbotAI::GetCurrentSpell() const
{
    if (m_CurrentlyCastingSpellId == 0)
        return nullptr;

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(m_CurrentlyCastingSpellId);
    return pSpell;
}

void PlayerbotAI::TellMaster(const std::string& text) const
{
    SendWhisper(text, *GetMaster());
}

void PlayerbotAI::TellMaster(const char* fmt, ...) const
{
    char temp_buf[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(temp_buf, 2048, fmt, ap);
    va_end(ap);
    std::string str = temp_buf;
    TellMaster(str);
}

void PlayerbotAI::SendWhisper(const std::string& text, Player& player) const
{
    if (player.GetPlayerbotAI())
        return;

    WorldPacket* const packet = new WorldPacket(CMSG_MESSAGECHAT, 200);
    *packet << uint32(CHAT_MSG_WHISPER);
    *packet << uint32(LANG_UNIVERSAL);
    *packet << player.GetName();
    *packet << text;
    m_bot->GetSession()->QueuePacket(std::move(std::unique_ptr<WorldPacket>(packet))); // queue the packet to get around race condition
}

bool PlayerbotAI::canObeyCommandFrom(const Player& player) const
{
    return player.GetSession()->GetAccountId() == GetMaster()->GetSession()->GetAccountId();
}

bool PlayerbotAI::In_Range(Unit* Target, uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!pSpellInfo)
        return false;

    SpellRangeEntry const* TempRange = GetSpellRangeStore()->LookupEntry(pSpellInfo->rangeIndex);

    //Spell has invalid range store so we can't use it
    if (!TempRange)
        return false;

    if ((TempRange->minRange == 0.0f) && (TempRange->maxRange == 0.0f))
        return true;

    //Unit is out of range for this spell
    if (!m_bot->IsInRange(Target, TempRange->minRange, TempRange->maxRange))
        return false;

    return true;
}

SpellCastResult PlayerbotAI::CheckBotCast(const SpellEntry* sInfo)
{
    if (!sInfo)
        return SPELL_FAILED_ERROR;

    // check DoLoot() spells before casting
    Spell* tmp_spell = new Spell(m_bot, sInfo, false);
    if (tmp_spell)
    {
        if (m_lootCurrent.IsCreature())
        {
            if (Creature* obj = m_bot->GetMap()->GetCreature(m_lootCurrent))
                tmp_spell->m_targets.setUnitTarget(obj);
        }
        else if (m_lootCurrent.IsGameObject())
        {
            if (GameObject* obj = m_bot->GetMap()->GetGameObject(m_lootCurrent))
                tmp_spell->m_targets.setGOTarget(obj);
        }

        // DEBUG_LOG("CheckBotCast SpellCastResult res(%u)", res);
        return tmp_spell->CheckCast(false);
    }
    return SPELL_FAILED_ERROR;
}

SpellCastResult PlayerbotAI::CastSpell(const char* args)
{
    uint32 spellId = getSpellId(args);
    return (spellId) ? CastSpell(spellId) : SPELL_NOT_FOUND;
}

SpellCastResult PlayerbotAI::CastSpell(uint32 spellId, Unit& target)
{
    ObjectGuid oldSel = m_bot->GetSelectionGuid();
    m_bot->SetSelectionGuid(target.GetObjectGuid());
    SpellCastResult rv = CastSpell(spellId);
    m_bot->SetSelectionGuid(oldSel);
    return rv;
}

SpellCastResult PlayerbotAI::CastSpell(uint32 spellId)
{
    // some AIs don't check if the bot doesn't have spell before using it
    // so just return false when this happens
    if (spellId == 0)
        return SPELL_FAILED_NOT_KNOWN;

    // see Creature.cpp 1738 for reference
    // don't allow bot to cast damage spells on friends
    const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!pSpellInfo)
    {
        TellMaster("Missing spell entry in CastSpell for spellid %u.", spellId);
        return SPELL_NOT_FOUND;
    }

    // check spell cooldown
    if (!m_bot->IsSpellReady(*pSpellInfo))
        return SPELL_FAILED_NOT_READY;

    // for AI debug purpose: uncomment the following line and bot will tell Master of every spell they attempt to cast
    // TellMaster("I'm trying to cast %s (spellID %u)", pSpellInfo->SpellName[0], spellId);

    // Power check
    // We use Spell::CheckPower() instead of UnitAI::CanCastSpell() because bots are players and have more requirements than mere units
    Spell* tmp_spell = new Spell(m_bot, pSpellInfo, false);
    SpellCastResult res = tmp_spell->CheckPower(true);
    if (res != SPELL_CAST_OK)
        return res;

    // set target
    ObjectGuid targetGUID = m_bot->GetSelectionGuid();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);

    if (!pTarget)
        pTarget = m_bot;

    if (IsPositiveSpell(spellId))
    {
        if (pTarget && m_bot->CanAttack(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->CanAssist(pTarget))    // Can't cast hostile spell on friendly unit
            return SPELL_FAILED_TARGET_FRIENDLY;

        m_bot->SetInFront(pTarget);
    }

    float CastTime = 0.0f;

    // stop movement to prevent cancel spell casting
    SpellCastTimesEntry const* castTimeEntry = sSpellCastTimesStore.LookupEntry(pSpellInfo->CastingTimeIndex);
    if (castTimeEntry && castTimeEntry->CastTime)
    {
        CastTime = (castTimeEntry->CastTime / 1000);
        DEBUG_LOG("[PlayerbotAI]: CastSpell - Bot movement reset for casting %s (%u)", pSpellInfo->SpellName[0], spellId);
        m_bot->StopMoving();
    }

    uint32 target_type = TARGET_FLAG_UNIT;

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK)
        target_type = TARGET_FLAG_GAMEOBJECT;

    m_CurrentlyCastingSpellId = spellId;

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
            pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        if (m_lootCurrent)
        {
            SpellCastResult castResult = CheckBotCast(pSpellInfo);
            if (castResult != SPELL_CAST_OK)
                return castResult;

            std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_CAST_SPELL, 1 + 4 + 1 + 4 + 8));
            *packet << uint8(0);                            // spells cast count;
            *packet << spellId;
            *packet << uint8(0);                            // unk_flags
            *packet << uint32(target_type);
            *packet << m_lootCurrent.WriteAsPacked();
            m_bot->GetSession()->QueuePacket(std::move(packet));       // queue the packet to get around race condition

            if (target_type == TARGET_FLAG_GAMEOBJECT)
            {
                std::unique_ptr<WorldPacket> packetgouse(new WorldPacket(CMSG_GAMEOBJ_REPORT_USE, 8));
                *packetgouse << m_lootCurrent;
                m_bot->GetSession()->QueuePacket(std::move(packetgouse));  // queue the packet to get around race condition

                GameObject* obj = m_bot->GetMap()->GetGameObject(m_lootCurrent);
                if (!obj)
                    return SPELL_FAILED_BAD_TARGETS;

                // add other go types here, i.e.:
                // GAMEOBJECT_TYPE_CHEST - loot quest items of chest
                if (obj->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
                {
                    TurnInQuests(obj);

                    // auto accept every available quest this NPC has
                    m_bot->PrepareQuestMenu(m_lootCurrent);
                    QuestMenu& questMenu = m_bot->GetPlayerMenu()->GetQuestMenu();
                    for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
                    {
                        QuestMenuItem const& qItem = questMenu.GetItem(iI);
                        uint32 questID = qItem.m_qId;
                        if (!AddQuest(questID, obj))
                            TellMaster("Couldn't take quest");
                    }
                    m_lootCurrent = ObjectGuid();
                    m_bot->GetMotionMaster()->Clear(false);
                    m_bot->GetMotionMaster()->MoveIdle();
                }
            }
            return SPELL_CAST_OK;
        }
        else
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
    }
    else
    {
        // Check spell range
        if (!In_Range(pTarget, spellId))
            return SPELL_FAILED_OUT_OF_RANGE;

        // Check line of sight
        if (!m_bot->IsWithinLOSInMap(pTarget))
            return SPELL_FAILED_LINE_OF_SIGHT;

        // Some casting times are negative so set ignore update time to 1 sec to avoid stucking the bot AI
        SetIgnoreUpdateTime(std::max(CastTime, 0.0f) + 1);

        if (IsAutoRepeatRangedSpell(pSpellInfo))
            return m_bot->CastSpell(pTarget, pSpellInfo, TRIGGERED_OLD_TRIGGERED); // cast triggered spell
        else
            return m_bot->CastSpell(pTarget, pSpellInfo, TRIGGERED_NONE);          // uni-cast spell
    }
}

SpellCastResult PlayerbotAI::CastPetSpell(uint32 spellId, Unit* target)
{
    if (spellId == 0)
        return SPELL_FAILED_NOT_KNOWN;

    Pet* pet = m_bot->GetPet();
    if (!pet)
        return SPELL_FAILED_NO_PET;

    const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!pSpellInfo)
    {
        TellMaster("Missing spell entry in CastPetSpell()");
        return SPELL_NOT_FOUND;
    }

    if (!pet->IsSpellReady(*pSpellInfo))
        return SPELL_FAILED_NOT_READY;

    // set target
    Unit* pTarget;
    if (!target)
    {
        ObjectGuid targetGUID = m_bot->GetSelectionGuid();
        pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);
    }
    else
        pTarget = target;

    if (IsPositiveSpell(spellId))
    {
        if (pTarget && m_bot->CanAttack(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->CanAssist(pTarget))    // Can cast hostile spell on friendly unit
            return SPELL_FAILED_TARGET_FRIENDLY;

        if (!pet->isInFrontInMap(pTarget, 10)) // distance probably should be calculated
            pet->SetFacingTo(pet->GetAngle(pTarget));
    }

    pet->CastSpell(pTarget, pSpellInfo, TRIGGERED_NONE);

    Spell* const pSpell = pet->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return SPELL_FAILED_NOT_KNOWN;

    return SPELL_CAST_OK;
}

// Perform sanity checks and cast spell
SpellCastResult PlayerbotAI::Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player*))
{
    if (spellId == 0)
        return SPELL_FAILED_NOT_KNOWN;

    // Target already has aura from spellId, skip for speed. May need to add exceptions
    if (target->HasAura(spellId))
        return SPELL_FAILED_AURA_BOUNCED;

    SpellEntry const* spellProto = sSpellTemplate.LookupEntry<SpellEntry>(spellId);

    if (!spellProto)
        return SPELL_NOT_FOUND;

    if (!target)
        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

    // Select appropriate spell rank for target's level
    spellProto = sSpellMgr.SelectAuraRankForLevel(spellProto, target->GetLevel());
    if (!spellProto)
        return SPELL_NOT_FOUND;

    // Check if spell will boost one of already existent auras
    bool willBenefitFromSpell = false;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (spellProto->EffectApplyAuraName[i] == SPELL_AURA_NONE)
            break;

        bool sameOrBetterAuraFound = false;
        int32 bonus = m_bot->CalculateSpellEffectValue(target, spellProto, SpellEffectIndex(i));
        Unit::AuraList const& auras = target->GetAurasByType(AuraType(spellProto->EffectApplyAuraName[i]));
        for (Unit::AuraList::const_iterator it = auras.begin(); it != auras.end(); ++it)
            if ((*it)->GetModifier()->m_miscvalue == spellProto->EffectMiscValue[i] && (*it)->GetModifier()->m_amount >= bonus)
            {
                sameOrBetterAuraFound = true;
                break;
            }
        willBenefitFromSpell = willBenefitFromSpell || !sameOrBetterAuraFound;
    }

    if (!willBenefitFromSpell)
        return SPELL_FAILED_AURA_BOUNCED;

    // Druids may need to shapeshift before casting
    if (beforeCast)
        (*beforeCast)(m_bot);

    return CastSpell(spellProto->Id, *target);
}

// Can be used for personal buffs like Mage Armor and Inner Fire
SpellCastResult PlayerbotAI::SelfBuff(uint32 spellId)
{
    if (spellId == 0)
        return SPELL_FAILED_NOT_KNOWN;

    if (m_bot->HasAura(spellId))
        return SPELL_FAILED_AURA_BOUNCED;

    return CastSpell(spellId, *m_bot);
}

// Checks if spell is single per target per caster and will make any effect on target
bool PlayerbotAI::CanReceiveSpecificSpell(uint8 spec, Unit* target) const
{
    if (IsSpellSpecificUniquePerCaster(SpellSpecific(spec)))
    {
        Unit::SpellAuraHolderMap holders = target->GetSpellAuraHolderMap();
        Unit::SpellAuraHolderMap::iterator it;
        for (it = holders.begin(); it != holders.end(); ++it)
            if ((*it).second->GetCasterGuid() == m_bot->GetObjectGuid() && GetSpellSpecific((*it).second->GetId()) == SpellSpecific(spec))
                return false;
    }
    return true;
}

uint8 PlayerbotAI::_findItemSlot(Item* target)
{
    // list out items equipped & in main backpack
    //INVENTORY_SLOT_ITEM_START = 23
    //INVENTORY_SLOT_ITEM_END = 39

    for (uint8 slot = EQUIPMENT_SLOT_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        // DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]backpack slot = %u",m_bot->GetName(),slot); // 23 to 38 = 16
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);  // 255, 23 to 38
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            if (pItemProto->ItemId == target->GetProto()->ItemId)   // have required item
                return slot;
        }
    }
    // list out items in other removable backpacks
    //INVENTORY_SLOT_BAG_START = 19
    //INVENTORY_SLOT_BAG_END = 23

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)  // 20 to 23 = 4
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);    // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                // DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);  // 1 to bagsize = ?
                Item* const pItem = m_bot->GetItemByPos(bag, slot); // 20 to 23, 1 to bagsize
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == target->GetProto()->ItemId)        // have required item
                        return slot;
                }
            }
    }
    return 0;
}

Item* PlayerbotAI::FindItem(uint32 ItemId, bool Equipped_too /* default = false */)
{
    uint8 first_slot;
    Equipped_too ? first_slot = EQUIPMENT_SLOT_START : first_slot = INVENTORY_SLOT_ITEM_START;

    // list out items equipped &/OR in main backpack
    //EQUIPMENT_SLOT_START = 0
    //INVENTORY_SLOT_ITEM_START = 23
    //INVENTORY_SLOT_ITEM_END = 39

    for (uint8 slot = first_slot; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        // DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]backpack slot = %u",m_bot->GetName(),slot); // 23 to 38 = 16
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);  // 255, 23 to 38
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            if (pItemProto->ItemId == ItemId)   // have required item
                return pItem;
        }
    }
    // list out items in other removable backpacks
    //INVENTORY_SLOT_BAG_START = 19
    //INVENTORY_SLOT_BAG_END = 23

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)  // 20 to 23 = 4
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);    // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                // DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);  // 1 to bagsize = ?
                Item* const pItem = m_bot->GetItemByPos(bag, slot); // 20 to 23, 1 to bagsize
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == ItemId)        // have required item
                        return pItem;
                }
            }
    }
    return nullptr;
}

Item* PlayerbotAI::FindItemInBank(uint32 ItemId)
{
    // list out items in bank item slots

    for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_END; slot++)
    {
        // sLog.outDebug("[%s's]backpack slot = %u",m_bot->GetName(),slot);
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            if (pItemProto->ItemId == ItemId)   // have required item
                return pItem;
        }
    }
    // list out items in bank bag slots

    for (uint8 bag = BANK_SLOT_BAG_START; bag < BANK_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                // sLog.outDebug("[%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == ItemId)        // have required item
                        return pItem;
                }
            }
    }
    return nullptr;
}

Item* PlayerbotAI::FindKeyForLockValue(uint32 reqSkillValue)
{
    if (reqSkillValue <= 25 && m_bot->HasItemCount(SILVER_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(SILVER_SKELETON_KEY);
    if (reqSkillValue <= 125 && m_bot->HasItemCount(GOLDEN_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(GOLDEN_SKELETON_KEY);
    if (reqSkillValue <= 200 && m_bot->HasItemCount(TRUESILVER_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(TRUESILVER_SKELETON_KEY);
    if (reqSkillValue <= 300 && m_bot->HasItemCount(ARCANITE_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(ARCANITE_SKELETON_KEY);
    if (reqSkillValue <= 375 && m_bot->HasItemCount(TITANIUM_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(TITANIUM_SKELETON_KEY);
    if (reqSkillValue <= 400 && m_bot->HasItemCount(COBALT_SKELETON_KEY, 1))
        return m_bot->GetItemByEntry(COBALT_SKELETON_KEY);

    return nullptr;
}

Item* PlayerbotAI::FindBombForLockValue(uint32 reqSkillValue)
{
    if (reqSkillValue <= 150 && m_bot->HasItemCount(SMALL_SEAFORIUM_CHARGE, 1))
        return m_bot->GetItemByEntry(SMALL_SEAFORIUM_CHARGE);
    if (reqSkillValue <= 250 && m_bot->HasItemCount(LARGE_SEAFORIUM_CHARGE, 1))
        return m_bot->GetItemByEntry(LARGE_SEAFORIUM_CHARGE);
    if (reqSkillValue <= 300 && m_bot->HasItemCount(POWERFUL_SEAFORIUM_CHARGE, 1))
        return m_bot->GetItemByEntry(POWERFUL_SEAFORIUM_CHARGE);
    if (reqSkillValue <= 350 && m_bot->HasItemCount(ELEMENTAL_SEAFORIUM_CHARGE, 1))
        return m_bot->GetItemByEntry(ELEMENTAL_SEAFORIUM_CHARGE);

    return nullptr;
}

bool PlayerbotAI::HasTool(uint32 TC)
{
    std::ostringstream out;

    switch (TC)
    {
        case TC_MINING_PICK:                //  = 165

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a MINING PICK!";
            break;

        case TC_ARCLIGHT_SPANNER:          //  = 14

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have an ARCLIGHT SPANNER!";
            break;

        case TC_BLACKSMITH_HAMMER:         //  = 162

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a BLACKSMITH's HAMMER!";
            break;

        case TC_SKINNING_KNIFE:            //  = 166

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a SKINNING KNIFE!";
            break;

        case TC_COPPER_ROD:                //  = 6,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED COPPER ROD!";
            break;

        case TC_SILVER_ROD:                //  = 7,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED SILVER ROD!";
            break;

        case TC_GOLDEN_ROD:                //  = 8,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED GOLDEN ROD!";
            break;

        case TC_TRUESILVER_ROD:            //  = 9,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED TRUESILVER ROD!";
            break;

        case TC_ARCANITE_ROD:              //  = 10,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED ARCANITE ROD!";
            break;

        case TC_FEL_IRON_ROD:              //  = 41,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED FEL IRON ROD!";
            break;

        case TC_ADAMANTITE_ROD:            //  = 62,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED ADAMANTITE ROD!";
            break;

        case TC_ETERNIUM_ROD:              //  = 63,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED ETERNIUM ROD!";
            break;

        case TC_RUNED_AZURITE_ROD:         //  = 101,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED AZURITE ROD!";
            break;

        case TC_VIRTUOSO_INKING_SET:       //  = 121,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a VIRTUOSO INKING SET!";
            break;

        case TC_RUNED_COBALT_ROD:          //  = 189,
            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED COBALT ROD!";
            break;

        case TC_RUNED_TITANIUM_ROD:        //  = 190,

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffff0000I do not have a RUNED TITANIUM ROD!";
            break;
        default:
            out << "|cffffffffI do not know what tool that needs! TC (" << TC << ")";
    }
    TellMaster(out.str().c_str());
    return false;
}

bool PlayerbotAI::PickPocket(Unit* pTarget)
{
    if (!pTarget)
        return false;

    ObjectGuid markGuid = pTarget->GetObjectGuid();
    Creature* c = m_bot->GetMap()->GetCreature(markGuid);
    if (c)
    {
        Loot*& loot = c->m_loot;
        if (!loot)
            loot = new Loot(m_bot, c, LOOT_PICKPOCKETING);
        else
        {
            if (loot->GetLootType() != LOOT_PICKPOCKETING)
            {
                delete loot;
                loot = new Loot(m_bot, c, LOOT_PICKPOCKETING);
            }
        }

        if (loot->GetGoldAmount())
        {
            m_bot->ModifyMoney(loot->GetGoldAmount());

            if (m_mgr.m_confDebugWhisper)
            {
                std::ostringstream out;

                // calculate how much money bot loots
                uint32 copper = loot->GetGoldAmount();
                uint32 gold = uint32(copper / 10000);
                copper -= (gold * 10000);
                uint32 silver = uint32(copper / 100);
                copper -= (silver * 100);

                out << "|r|cff009900" << m_bot->GetName() << " loots: " << "|h|cffffffff[|r|cff00ff00" << gold
                    << "|r|cfffffc00g|r|cff00ff00" << silver
                    << "|r|cffcdcdcds|r|cff00ff00" << copper
                    << "|r|cff993300c"
                    << "|h|cffffffff]";

                TellMaster(out.str().c_str());
            }

            // send the money to the bot and remove it from the creature
            loot->SendGold(m_bot);
        }

        if (!loot->AutoStore(m_bot, false, NULL_BAG, NULL_SLOT))
            sLog.outDebug("PLAYERBOT Debug: Failed to get loot from pickpocketed NPC");

        // release the loot whatever happened
        loot->Release(m_bot);
    }
    return false; // ensures that the rogue only pick pockets target once
}

bool PlayerbotAI::HasSpellReagents(uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!pSpellInfo)
        return false;

    if (m_bot->CanNoReagentCast(pSpellInfo))
        return true;

    for (uint32 i = 0; i < MAX_SPELL_REAGENTS; ++i)
    {
        if (pSpellInfo->Reagent[i] <= 0)
            continue;

        uint32 itemid = pSpellInfo->Reagent[i];
        uint32 count = pSpellInfo->ReagentCount[i];

        if (!m_bot->HasItemCount(itemid, count))
            return false;
    }

    return true;
}

uint32 PlayerbotAI::GetSpellCharges(uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!pSpellInfo)
        return 0;

    if (m_bot->CanNoReagentCast(pSpellInfo))
        return 0;

    uint32 charges = 0;
    std::list<uint32> chargeList;
    for (uint32 i = 0; i < MAX_SPELL_REAGENTS; ++i)
    {
        if (pSpellInfo->Reagent[i] <= 0)
            continue;

        uint32 totalcount = 0;
        uint32 itemid = pSpellInfo->Reagent[i];
        uint32 count = pSpellInfo->ReagentCount[i];
        ItemCountInInv(itemid, totalcount);
        chargeList.push_back((totalcount / count));
    }

    for (uint32 i = 0; i < MAX_SPELL_TOTEM_CATEGORIES; ++i)
    {
        if (pSpellInfo->TotemCategory[i] == 0)
            continue;

        if (!m_bot->HasItemTotemCategory(pSpellInfo->TotemCategory[i]))
        {
            m_noToolList.push_back(pSpellInfo->TotemCategory[i]);
            return 0;
        }
    }

    if (!chargeList.empty())
    {
        charges = chargeList.front();
        chargeList.pop_front();
        for (std::list<uint32>::iterator it = chargeList.begin(); it != chargeList.end(); ++it)
            if (*it < charges)
                charges = *it;
    }
    return charges;
}

void PlayerbotAI::ItemCountInInv(uint32 itemid, uint32& count)
{
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        Item* pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pItem && pItem->GetEntry() == itemid && !pItem->IsInTrade())
            count += pItem->GetCount();
    }

    for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag* pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
            {
                Item* pItem = m_bot->GetItemByPos(i, j);
                if (pItem && pItem->GetEntry() == itemid && !pItem->IsInTrade())
                    count += pItem->GetCount();
            }
    }
}

// extracts all item ids in format below
// I decided to roll my own extractor rather then use the one in ChatHandler
// because this one works on a const string, and it handles multiple links
// |color|linkType:key:something1:...:somethingN|h[name]|h|r
void PlayerbotAI::extractItemIds(const std::string& text, std::list<uint32>& itemIds) const
{
    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hitem:", pos);
        if (i == -1)
            break;
        pos = i + 6;
        int endPos = text.find(':', pos);
        if (endPos == -1)
            break;
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            itemIds.push_back(id);
    }
}

void PlayerbotAI::extractMailIds(const std::string& text, std::list<uint32>& mailIds) const
{
    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hmail:", pos);
        if (i == -1)
            break;
        pos = i + 6;
        int endPos = text.find('|', pos);
        if (endPos == -1)
            break;
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            mailIds.push_back(id);
    }
}

/**
* Checks whether the TalentSpec database contains any obvious errors
*
* return 0  -> all ok
* return x  -> return the talentspec_id of the first talentspec that errors out
*/

// TODO: the way this is built is just begging for a memory leak (by adding a return case and forgetting to delete result)
uint32 PlayerbotAI::TalentSpecDBContainsError()
{
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM playerbot_talentspec ORDER BY class ASC");

    if (!result)
    {
        // Do you really need a progress bar? No, but all the other kids jumped off the bridge too...
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded `playerbot_talentspec`, table is empty.");

        return 0;   // Because, well, no specs means none contain errors...
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        /* 0            talentspec_id
        1            name
        2            class
        3            purpose
        4 to 74        talent_10 to 71
        75 to 80        major_glyph_15, 30, 80, minor_glyph_15, 50, 70
        */
        Field* fields = result->Fetch();

        uint32 ts_id = fields[0].GetUInt32();
        if (!ts_id)    // Nice bit of paranoia: ts_id is a non-zero NOT NULL AUTO_INCREMENT value
            continue;  // Of course, if the impossible ever does happen, we can't very well identify a TalentSpec without an ID...

        std::string ts_name = fields[1].GetCppString();
        /*    Commented out? Because it's only required if you assume only players (not the server) pick talentspecs
        if (0 == ts_name.size())
        {
        TellMaster("TalentSpec ID: %u does not have a name.", ts_id);

        delete result;
        return ts_id;
        }
        */

        long ts_class = fields[2].GetInt32();
        if (ts_class != CLASS_DEATH_KNIGHT && ts_class != CLASS_DRUID && ts_class != CLASS_HUNTER && ts_class != CLASS_MAGE && ts_class != CLASS_PALADIN && ts_class != CLASS_PRIEST && ts_class != CLASS_ROGUE && ts_class != CLASS_SHAMAN && ts_class != CLASS_WARLOCK && ts_class != CLASS_WARRIOR &&
                ts_class != CLASS_PET_CUNNING && ts_class != CLASS_PET_FEROCITY && ts_class != CLASS_PET_TENACITY)
        {
            TellMaster("TalentSpec: %u. \"%s\" contains an invalid class: %i.", ts_id, ts_name.c_str(), ts_class);

            delete result;
            return ts_id;    // invalid class
        }

        // Can't really be error checked, can it?
        // uint32 ts_purpose = fields[3].GetUInt32();

        // check all talents
        for (uint8 i = 0; i < 71; i++)
        {
            uint8 fieldLoc = i + 4;
            if (fields[fieldLoc].GetUInt16() == 0)
            {
                for (uint8 j = (i + 1); j < 71; j++)
                {
                    fieldLoc = j + 4;
                    if (fields[fieldLoc].GetUInt16() != 0)
                    {
                        TellMaster("TalentSpec: %u. \"%s\" contains an empty talent for level: %u while a talent for level: %u exists.", ts_id, ts_name.c_str(), (i + 10), (j + 10));

                        delete result;
                        return ts_id;
                    }
                }
                break;
            }
            else if (!ValidateTalent(fields[fieldLoc].GetUInt16(), ts_class))
            {
                TellMaster("TalentSpec: %u. \"%s\" (class: %i) contains an invalid talent for level %u: %u", ts_id, ts_name.c_str(), ts_class, (i + 10), fields[fieldLoc].GetUInt16());

                delete result;
                return ts_id;    // invalid talent
            }
        }

        for (uint8 i = 75; i < 78; i++)  // as in, the 3 major glyphs
        {
            if (fields[i].GetUInt16() != 0 && !ValidateMajorGlyph(fields[i].GetUInt16(), ts_class))
            {
                TellMaster("TalentSpec: %u. \"%s\" contains an invalid Major glyph %u: %u", ts_id, ts_name.c_str(), (i - 74), fields[i].GetUInt16());
                if (!ValidateGlyph(fields[i].GetUInt16(), ts_class))
                    TellMaster("In fact, according to our records, it's no glyph at all");

                delete result;
                return ts_id;
            }
        }
        for (uint8 i = 78; i < 81; i++)  // as in, the 3 minor glyphs
        {
            if (fields[i].GetUInt16() != 0 && !ValidateMinorGlyph(fields[i].GetUInt16(), ts_class))
            {
                TellMaster("TalentSpec: %u. \"%s\" contains an invalid Minor glyph %u: %u", ts_id, ts_name.c_str(), (i - 77), fields[i].GetUInt16());
                if (!ValidateGlyph(fields[i].GetUInt16(), ts_class))
                    TellMaster("In fact, according to our records, it's no glyph at all");

                delete result;
                return ts_id;
            }
        }
    }
    while (result->NextRow());

    delete result;
    return 0;
}

uint32 PlayerbotAI::GetTalentSpecsAmount()
{
    QueryResult* result = CharacterDatabase.Query("SELECT COUNT(*) FROM playerbot_talentspec");

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> Loaded `playerbot_talentspec`, table is empty.");

        return 0;
    }

    Field* fields = result->Fetch();

    uint32 count = fields[0].GetUInt32();

    delete result;
    return count;
}

uint32 PlayerbotAI::GetTalentSpecsAmount(long specClass)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM playerbot_talentspec WHERE class = '%li'", specClass);

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> Loaded `playerbot_talentspec`, found no talentspecs for class %li.", specClass);

        return 0;
    }

    Field* fields = result->Fetch();

    uint32 count = fields[0].GetUInt32();

    delete result;
    return count;
}

/**
* GetTalentSpecs queries DB for the talentspecs (for a class), returning them in a list of TS structures
*
* *** for the most part, GetTalentSpecs assumes ALL SPECS ARE VALID ***
*/
std::list<TalentSpec> PlayerbotAI::GetTalentSpecs(long specClass)
{
    TalentSpec ts;
    std::list<TalentSpec> tsList;

    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM playerbot_talentspec WHERE class = %li ORDER BY talentspec_id ASC", specClass);

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> Loaded `playerbot_talentspec`, found no talentspecs for class %li.", specClass);

        return tsList; // empty
    }

    do
    {
        /* 0            talentspec_id
        1            name
        2            class
        3            purpose
        4 to 74        talent_10 to 71
        75 to 80        major_glyph_15, 30, 80, minor_glyph_15, 50, 70
        */
        Field* fields = result->Fetch();

        /* ts_id = fields[0].GetUInt32(); // not used
        if (!ts_id)    // Nice bit of paranoia: ts_id is an AUTO_INCREMENT value
        continue;  // Of course, if the impossible ever does happen, we can't very well identify a TalentSpec without an ID...
        */

        ts.specName = fields[1].GetCppString();
        ts.specClass = fields[2].GetInt16();
        if (ts.specClass != CLASS_DEATH_KNIGHT && ts.specClass != CLASS_DRUID && ts.specClass != CLASS_HUNTER && ts.specClass != CLASS_MAGE && ts.specClass != CLASS_PALADIN && ts.specClass != CLASS_PRIEST && ts.specClass != CLASS_ROGUE && ts.specClass != CLASS_SHAMAN && ts.specClass != CLASS_WARLOCK && ts.specClass != CLASS_WARRIOR &&
                ts.specClass != CLASS_PET_CUNNING && ts.specClass != CLASS_PET_FEROCITY && ts.specClass != CLASS_PET_TENACITY)
        {
            TellMaster("TalentSpec: %u. \"%s\" contains an invalid class.", fields[0].GetUInt32(), ts.specName.c_str());

            continue;    // this spec is clearly broken, the next may or may not be
        }

        ts.specPurpose = (TalentSpecPurpose) fields[3].GetUInt32();

        // check all talents
        for (uint8 i = 0; i < 71; i++)
        {
            ts.talentId[i] = fields[i + 4].GetUInt16();
        }

        for (uint8 i = 0; i < 3; i++)  // as in, the 3 major glyphs
        {
            ts.glyphIdMajor[i] = fields[i + 75].GetUInt16();
        }
        for (uint8 i = 0; i < 3; i++)  // as in, the 3 minor glyphs
        {
            ts.glyphIdMajor[i] = fields[i + 78].GetUInt16();
        }

        tsList.push_back(ts);
    }
    while (result->NextRow());

    delete result;
    return tsList;
}

/**
* GetTalentSpec queries DB for a talentspec given a class and a choice.
* The choice applies to the results for that class only, and is volatile.
*
* *** for the most part, GetTalentSpec assumes ALL SPECS ARE VALID ***
*/
TalentSpec PlayerbotAI::GetTalentSpec(long specClass, long choice)
{
    TalentSpec ts;
    // Let's zero it out to be safe
    ts.specName = "";
    ts.specClass = 0;
    ts.specPurpose = TSP_NONE;
    for (int i = 0; i < 71; i++) ts.talentId[i] = 0;
    for (int i = 0; i < 3; i++) ts.glyphIdMajor[i] = 0;
    for (int i = 0; i < 3; i++) ts.glyphIdMinor[i] = 0;

    // Weed out invalid choice - ts has been zero'd out anyway
    if (0 >= choice || (long) GetTalentSpecsAmount(specClass) < choice) return ts;

    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM playerbot_talentspec WHERE class = %li ORDER BY talentspec_id ASC", specClass);

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> Loaded `playerbot_talentspec`, found no talentspecs for class %li.", specClass);

        delete result;
        return ts; // empty
    }

    for (int i = 1; i <= (int) GetTalentSpecsAmount(specClass); i++)
    {

        if (i == choice)
        {
            /*
            0            talentspec_id
            1            name
            2            class
            3            purpose
            4 to 74    talent_10 to 71
            75 to 80    major_glyph_15, 30, 80, minor_glyph_15, 50, 70
            */
            Field* fields = result->Fetch();

            /* ts_id = fields[0].GetUInt32(); // not used
            if (!ts_id)    // Nice bit of paranoia: ts_id is an AUTO_INCREMENT value
            continue;  // Of course, if the impossible ever does happen, we can't very well identify a TalentSpec without an ID...
            */

            ts.specName = fields[1].GetCppString();
            ts.specClass = fields[2].GetInt16();
            if (ts.specClass != CLASS_DEATH_KNIGHT && ts.specClass != CLASS_DRUID && ts.specClass != CLASS_HUNTER && ts.specClass != CLASS_MAGE && ts.specClass != CLASS_PALADIN && ts.specClass != CLASS_PRIEST && ts.specClass != CLASS_ROGUE && ts.specClass != CLASS_SHAMAN && ts.specClass != CLASS_WARLOCK && ts.specClass != CLASS_WARRIOR &&
                    ts.specClass != CLASS_PET_CUNNING && ts.specClass != CLASS_PET_FEROCITY && ts.specClass != CLASS_PET_TENACITY)
            {
                TellMaster("TalentSpec: %u. \"%s\" contains an invalid class.", fields[0].GetUInt32(), ts.specName.c_str());

                ts.specName = "";
                ts.specClass = 0;
                delete result;
                return ts;
            }

            ts.specPurpose = (TalentSpecPurpose) fields[3].GetUInt32();

            // check all talents
            for (uint8 i = 0; i < 71; i++)
            {
                ts.talentId[i] = fields[i + 4].GetUInt16();
            }

            for (uint8 i = 0; i < 3; i++)  // as in, the 3 major glyphs
            {
                ts.glyphIdMajor[i] = fields[i + 75].GetUInt16();
            }
            for (uint8 i = 0; i < 3; i++)  // as in, the 3 minor glyphs
            {
                ts.glyphIdMajor[i] = fields[i + 78].GetUInt16();
            }

            delete result;
            return ts;
        }

        // TODO: okay, this won't bog down the system, but it's still a waste. Figure out a better way.
        result->NextRow();
    }

    delete result;
    return ts;
}

/**
* ApplyActiveTalentSpec takes the active talent spec and attempts to apply it
*
* return true  -> ok, talentspec applied as fully as possible
* return false -> talentspec was not or only partially applied
*/
bool PlayerbotAI::ApplyActiveTalentSpec()
{
    // empty talent spec -> nothing to apply -> fully applied
    if (m_activeTalentSpec.specClass == 0 || m_activeTalentSpec.specPurpose == TSP_NONE)
        return true;

    // Some basic error checking just in case
    if (m_activeTalentSpec.specClass != m_bot->getClass())
        return false;

    std::vector<uint16> talentsToLearn;
    talentsToLearn.reserve(71);
    for (int i = 0; i < 71; i++)
    {
        if (m_activeTalentSpec.talentId[i] != 0)
            talentsToLearn.push_back(m_activeTalentSpec.talentId[i]);
    }

    PlayerTalentMap ptm = m_bot->GetTalents(m_bot->GetActiveSpec());
    // First do a check as to whether all known talents are in the talent spec
    for (PlayerTalentMap::iterator iter = ptm.begin(); iter != ptm.end(); iter++)
    {
        PlayerTalent talent = (*iter).second;

        // WARNING: There may be more than 71 'talents' in the PTM - unlearned talents are simply set as disabled - not removed
        if (talent.state == PLAYERSPELL_REMOVED)
            continue;

        // currentRank = 0 to (MAX_RANK-1) not 1 to MAX_RANK
        for (int i = 0; i <= (int) talent.currentRank; i++)
        {
            int j = 0; // why 0 and not -1? Because if talentsToCheck (no TalentSpec) is empty and talents have been learned -> NOK
            for (std::vector<uint16>::iterator it = talentsToLearn.begin(); it != talentsToLearn.end(); it++)
            {
                if (talentsToLearn.at(j) == talent.talentEntry->TalentID)
                {
                    talentsToLearn.erase(it);
                    j = -1; // So j = -1 -> learned talent found in talentspec
                    break;
                }
                j++;
            }

            // j == -1 signifies talent has been found in talent spec
            if (-1 != j)
            {
                TellMaster("I've learned talents that are not in my talent spec. If you want me to learn the talent spec anyway you should have me reset my talents.");
                return false;
            }
        }
    }

    int x = 0;
    for (std::vector<uint16>::iterator iter = talentsToLearn.begin(); iter != talentsToLearn.end(); iter++)
    {
        // find current talent rank
        uint32 learnTalentRank = 0;
        if (PlayerTalent const* talent = m_bot->GetKnownTalentById(talentsToLearn.at(x)))
            learnTalentRank = talent->currentRank + 1;
        // else -> not known -> to learn = 0

        // check if we have enough talent points
        uint32 freeTalentPointsBefore = m_bot->GetFreeTalentPoints();
        if (0 == freeTalentPointsBefore)
            return true;

        m_bot->LearnTalent(talentsToLearn.at(x), learnTalentRank);
        if (freeTalentPointsBefore == m_bot->GetFreeTalentPoints())
        {
            // Do not tell master - error is logged server side, master gets generic failure warning from calling function.
            //TellMaster("Failed to learn talent - Class: %i; TalentId: %i; TalentRank: %i. This error has been logged.", m_bot->getClass(), talentsToLearn.at(x), learnTalentRank);
            DEBUG_LOG("[PlayerbotAI]: ApplyActiveTalentSpec failure - Class: %i; TalentId: %i; TalentRank: %i.", m_bot->getClass(), talentsToLearn.at(x), learnTalentRank);
            return false;
        }

        x++;
    }

    return true;
}

/**
* ValidateTalent tests a talent against class to see if it belongs to that class
*
* uint16 talent:        talent ID
* long charClass:    member of the Classes enum or ClassesCombatPets enum
*
* return true  -> ok
* return false -> not a valid talent for that class
*/
bool PlayerbotAI::ValidateTalent(uint16 talent, long charClass)
{
    if (charClass == CLASS_DEATH_KNIGHT)
    {
        // this looong 'if' is to see if any talent is not a Death Knight talent when the class clearly is
        if (DEATHKNIGHT_BUTCHERY != talent && DEATHKNIGHT_SUBVERSION != talent && DEATHKNIGHT_BLADE_BARRIER != talent && DEATHKNIGHT_BLADED_ARMOR != talent && DEATHKNIGHT_SCENT_OF_BLOOD != talent && DEATHKNIGHT_TWOHANDED_WEAPON_SPECIALIZATION != talent && DEATHKNIGHT_RUNE_TAP != talent && DEATHKNIGHT_DARK_CONVICTION != talent && DEATHKNIGHT_DEATH_RUNE_MASTERY != talent && DEATHKNIGHT_IMPROVED_RUNE_TAP != talent && DEATHKNIGHT_SPELL_DEFLECTION != talent && DEATHKNIGHT_VENDETTA != talent && DEATHKNIGHT_BLOODY_STRIKES != talent && DEATHKNIGHT_VETERAN_OF_THE_THIRD_WAR != talent && DEATHKNIGHT_MARK_OF_BLOOD != talent && DEATHKNIGHT_BLOODY_VENGEANCE != talent && DEATHKNIGHT_ABOMINATIONS_MIGHT != talent && DEATHKNIGHT_BLOOD_WORMS != talent && DEATHKNIGHT_HYSTERIA != talent && DEATHKNIGHT_IMPROVED_BLOOD_PRESENCE != talent && DEATHKNIGHT_IMPROVED_DEATH_STRIKE != talent && DEATHKNIGHT_SUDDEN_DOOM != talent && DEATHKNIGHT_VAMPIRIC_BLOOD != talent && DEATHKNIGHT_WILL_OF_THE_NECROPOLIS != talent && DEATHKNIGHT_HEART_STRIKE != talent && DEATHKNIGHT_MIGHT_OF_MOGRAINE != talent && DEATHKNIGHT_BLOOD_GORGED != talent && DEATHKNIGHT_DANCING_RUNE_WEAPON != talent && DEATHKNIGHT_IMPROVED_ICY_TOUCH != talent && DEATHKNIGHT_RUNIC_POWER_MASTERY != talent && DEATHKNIGHT_TOUGHNESS != talent && DEATHKNIGHT_ICY_REACH != talent && DEATHKNIGHT_BLACK_ICE != talent && DEATHKNIGHT_NERVES_OF_COLD_STEEL != talent && DEATHKNIGHT_ICY_TALONS != talent && DEATHKNIGHT_LICHBORNE != talent && DEATHKNIGHT_ANNIHILATION != talent && DEATHKNIGHT_KILLING_MACHINE != talent && DEATHKNIGHT_CHILL_OF_THE_GRAVE != talent && DEATHKNIGHT_ENDLESS_WINTER != talent && DEATHKNIGHT_FRIGID_DREADPLATE != talent && DEATHKNIGHT_GLACIER_ROT != talent && DEATHKNIGHT_DEATHCHILL != talent && DEATHKNIGHT_IMPROVED_ICY_TALONS != talent && DEATHKNIGHT_MERCILESS_COMBAT != talent && DEATHKNIGHT_RIME != talent && DEATHKNIGHT_CHILLBLAINS != talent && DEATHKNIGHT_HUNGERING_COLD != talent && DEATHKNIGHT_IMPROVED_FROST_PRESENCE != talent && DEATHKNIGHT_THREAT_OF_THASSARIAN != talent && DEATHKNIGHT_BLOOD_OF_THE_NORTH != talent && DEATHKNIGHT_UNBREAKABLE_ARMOR != talent && DEATHKNIGHT_ACCLIMATION != talent && DEATHKNIGHT_FROST_STRIKE != talent && DEATHKNIGHT_GUILE_OF_GOREFIEND != talent && DEATHKNIGHT_TUNDRA_STALKER != talent && DEATHKNIGHT_HOWLING_BLAST != talent && DEATHKNIGHT_VICIOUS_STRIKES != talent && DEATHKNIGHT_VIRULENCE != talent && DEATHKNIGHT_ANTICIPATION != talent && DEATHKNIGHT_EPIDEMIC != talent && DEATHKNIGHT_MORBIDITY != talent && DEATHKNIGHT_UNHOLY_COMMAND != talent && DEATHKNIGHT_RAVENOUS_DEAD != talent && DEATHKNIGHT_OUTBREAK != talent && DEATHKNIGHT_NECROSIS != talent && DEATHKNIGHT_CORPSE_EXPLOSION != talent && DEATHKNIGHT_ON_A_PALE_HORSE != talent && DEATHKNIGHT_BLOODCAKED_BLADE != talent && DEATHKNIGHT_NIGHT_OF_THE_DEAD != talent && DEATHKNIGHT_UNHOLY_BLIGHT != talent && DEATHKNIGHT_IMPURITY != talent && DEATHKNIGHT_DIRGE != talent && DEATHKNIGHT_DESECRATION != talent && DEATHKNIGHT_MAGIC_SUPPRESSION != talent && DEATHKNIGHT_REAPING != talent && DEATHKNIGHT_MASTER_OF_GHOULS != talent && DEATHKNIGHT_DESOLATION != talent && DEATHKNIGHT_ANTIMAGIC_ZONE != talent && DEATHKNIGHT_IMPROVED_UNHOLY_PRESENCE != talent && DEATHKNIGHT_GHOUL_FRENZY != talent && DEATHKNIGHT_CRYPT_FEVER != talent && DEATHKNIGHT_BONE_SHIELD != talent && DEATHKNIGHT_WANDERING_PLAGUE != talent && DEATHKNIGHT_EBON_PLAGUEBRINGER != talent && DEATHKNIGHT_SCOURGE_STRIKE != talent && DEATHKNIGHT_RAGE_OF_RIVENDARE != talent && DEATHKNIGHT_SUMMON_GARGOYLE != talent)
            return false;
    }
    else if (charClass == CLASS_DRUID)
    {
        if (DRUID_FEROCITY != talent && DRUID_FERAL_AGGRESSION != talent && DRUID_FERAL_INSTINCT != talent && DRUID_SAVAGE_FURY != talent && DRUID_THICK_HIDE != talent && DRUID_FERAL_SWIFTNESS != talent && DRUID_SURVIVAL_INSTINCTS != talent && DRUID_SHARPENED_CLAWS != talent && DRUID_SHREDDING_ATTACKS != talent && DRUID_PREDATORY_STRIKES != talent && DRUID_PRIMAL_FURY != talent && DRUID_PRIMAL_PRECISION != talent && DRUID_BRUTAL_IMPACT != talent && DRUID_FERAL_CHARGE != talent && DRUID_NURTURING_INSTINCT != talent && DRUID_NATURAL_REACTION != talent && DRUID_HEART_OF_THE_WILD != talent && DRUID_SURVIVAL_OF_THE_FITTEST != talent && DRUID_LEADER_OF_THE_PACK != talent && DRUID_IMPROVED_LEADER_OF_THE_PACK != talent && DRUID_PRIMAL_TENACITY != talent && DRUID_PROTECTOR_OF_THE_PACK != talent && DRUID_PREDATORY_INSTINCTS != talent && DRUID_INFECTED_WOUNDS != talent && DRUID_KING_OF_THE_JUNGLE != talent && DRUID_MANGLE != talent && DRUID_IMPROVED_MANGLE != talent && DRUID_REND_AND_TEAR != talent && DRUID_PRIMAL_GORE != talent && DRUID_BERSERK != talent && DRUID_IMPROVED_MARK_OF_THE_WILD != talent && DRUID_NATURES_FOCUS != talent && DRUID_FUROR != talent && DRUID_NATURALIST != talent && DRUID_SUBTLETY != talent && DRUID_NATURAL_SHAPESHIFTER != talent && DRUID_INTENSITY != talent && DRUID_OMEN_OF_CLARITY != talent && DRUID_MASTER_SHAPESHIFTER != talent && DRUID_TRANQUIL_SPIRIT != talent && DRUID_IMPROVED_REJUVENATION != talent && DRUID_NATURES_SWIFTNESS != talent && DRUID_GIFT_OF_NATURE != talent && DRUID_IMPROVED_TRANQUILITY != talent && DRUID_EMPOWERED_TOUCH != talent && DRUID_NATURES_BOUNTY != talent && DRUID_LIVING_SPIRIT != talent && DRUID_SWIFTMEND != talent && DRUID_NATURAL_PERFECTION != talent && DRUID_EMPOWERED_REJUVENATION != talent && DRUID_LIVING_SEED != talent && DRUID_REVITALIZE != talent && DRUID_TREE_OF_LIFE != talent && DRUID_IMPROVED_TREE_OF_LIFE != talent && DRUID_IMPROVED_BARKSKIN != talent && DRUID_GIFT_OF_THE_EARTHMOTHER != talent && DRUID_WILD_GROWTH != talent && DRUID_STARLIGHT_WRATH != talent && DRUID_GENESIS != talent && DRUID_MOONGLOW != talent && DRUID_NATURES_MAJESTY != talent && DRUID_IMPROVED_MOONFIRE != talent && DRUID_BRAMBLES != talent && DRUID_NATURES_GRACE != talent && DRUID_NATURES_SPLENDOR != talent && DRUID_NATURES_REACH != talent && DRUID_VENGEANCE != talent && DRUID_CELESTIAL_FOCUS != talent && DRUID_LUNAR_GUIDANCE != talent && DRUID_INSECT_SWARM != talent && DRUID_IMPROVED_INSECT_SWARM != talent && DRUID_DREAMSTATE != talent && DRUID_MOONFURY != talent && DRUID_BALANCE_OF_POWER != talent && DRUID_MOONKIN_FORM != talent && DRUID_IMPROVED_MOONKIN_FORM != talent && DRUID_IMPROVED_FAERIE_FIRE != talent && DRUID_OWLKIN_FRENZY != talent && DRUID_WRATH_OF_CENARIUS != talent && DRUID_ECLIPSE != talent && DRUID_TYPHOON != talent && DRUID_FORCE_OF_NATURE != talent && DRUID_GALE_WINDS != talent && DRUID_EARTH_AND_MOON != talent && DRUID_STARFALL != talent)
            return false;
    }
    else if (charClass == CLASS_HUNTER)
    {
        if (HUNTER_IMPROVED_ASPECT_OF_THE_HAWK != talent && HUNTER_ENDURANCE_TRAINING != talent && HUNTER_FOCUSED_FIRE != talent && HUNTER_IMPROVED_ASPECT_OF_THE_MONKEY != talent && HUNTER_THICK_HIDE != talent && HUNTER_IMPROVED_REVIVE_PET != talent && HUNTER_PATHFINDING != talent && HUNTER_ASPECT_MASTERY != talent && HUNTER_UNLEASHED_FURY != talent && HUNTER_IMPROVED_MEND_PET != talent && HUNTER_FEROCITY != talent && HUNTER_SPIRIT_BOND != talent && HUNTER_INTIMIDATION != talent && HUNTER_BESTIAL_DISCIPLINE != talent && HUNTER_ANIMAL_HANDLER != talent && HUNTER_FRENZY != talent && HUNTER_FEROCIOUS_INSPIRATION != talent && HUNTER_BESTIAL_WRATH != talent && HUNTER_CATLIKE_REFLEXES != talent && HUNTER_INVIGORATION != talent && HUNTER_SERPENTS_SWIFTNESS != talent && HUNTER_LONGEVITY != talent && HUNTER_THE_BEAST_WITHIN != talent && HUNTER_COBRA_STRIKES != talent && HUNTER_KINDRED_SPIRITS != talent && HUNTER_BEAST_MASTERY != talent && HUNTER_IMPROVED_TRACKING != talent && HUNTER_HAWK_EYE != talent && HUNTER_SAVAGE_STRIKES != talent && HUNTER_SUREFOOTED != talent && HUNTER_ENTRAPMENT != talent && HUNTER_TRAP_MASTERY != talent && HUNTER_SURVIVAL_INSTINCTS != talent && HUNTER_SURVIVALIST != talent && HUNTER_SCATTER_SHOT != talent && HUNTER_DEFLECTION != talent && HUNTER_SURVIVAL_TACTICS != talent && HUNTER_TNT != talent && HUNTER_LOCK_AND_LOAD != talent && HUNTER_HUNTER_VS_WILD != talent && HUNTER_KILLER_INSTINCT != talent && HUNTER_COUNTERATTACK != talent && HUNTER_LIGHTNING_REFLEXES != talent && HUNTER_RESOURCEFULNESS != talent && HUNTER_EXPOSE_WEAKNESS != talent && HUNTER_WYVERN_STING != talent && HUNTER_THRILL_OF_THE_HUNT != talent && HUNTER_MASTER_TACTICIAN != talent && HUNTER_NOXIOUS_STINGS != talent && HUNTER_POINT_OF_NO_ESCAPE != talent && HUNTER_BLACK_ARROW != talent && HUNTER_SNIPER_TRAINING != talent && HUNTER_HUNTING_PARTY != talent && HUNTER_EXPLOSIVE_SHOT != talent && HUNTER_IMPROVED_CONCUSSIVE_SHOT != talent && HUNTER_FOCUSED_AIM != talent && HUNTER_LETHAL_SHOTS != talent && HUNTER_CAREFUL_AIM != talent && HUNTER_IMPROVED_HUNTERS_MARK != talent && HUNTER_MORTAL_SHOTS != talent && HUNTER_GO_FOR_THE_THROAT != talent && HUNTER_IMPROVED_ARCANE_SHOT != talent && HUNTER_AIMED_SHOT != talent && HUNTER_RAPID_KILLING != talent && HUNTER_IMPROVED_STINGS != talent && HUNTER_EFFICIENCY != talent && HUNTER_CONCUSSIVE_BARRAGE != talent && HUNTER_READINESS != talent && HUNTER_BARRAGE != talent && HUNTER_COMBAT_EXPERIENCE != talent && HUNTER_RANGED_WEAPON_SPECIALIZATION != talent && HUNTER_PIERCING_SHOTS != talent && HUNTER_TRUESHOT_AURA != talent && HUNTER_IMPROVED_BARRAGE != talent && HUNTER_MASTER_MARKSMAN != talent && HUNTER_RAPID_RECUPERATION != talent && HUNTER_WILD_QUIVER != talent && HUNTER_SILENCING_SHOT != talent && HUNTER_IMPROVED_STEADY_SHOT != talent && HUNTER_MARKED_FOR_DEATH != talent && HUNTER_CHIMERA_SHOT != talent)
            return false;
    }
    else if (charClass == CLASS_MAGE)
    {
        if (MAGE_IMPROVED_FIRE_BLAST != talent && MAGE_INCINERATION != talent && MAGE_IMPROVED_FIREBALL != talent && MAGE_IGNITE != talent && MAGE_BURNING_DETERMINATION != talent && MAGE_WORLD_IN_FLAMES != talent && MAGE_FLAME_THROWING != talent && MAGE_IMPACT != talent && MAGE_PYROBLAST != talent && MAGE_BURNING_SOUL != talent && MAGE_IMPROVED_SCORCH != talent && MAGE_MOLTEN_SHIELDS != talent && MAGE_MASTER_OF_ELEMENTS != talent && MAGE_PLAYING_WITH_FIRE != talent && MAGE_CRITICAL_MASS != talent && MAGE_BLAST_WAVE != talent && MAGE_BLAZING_SPEED != talent && MAGE_FIRE_POWER != talent && MAGE_PYROMANIAC != talent && MAGE_COMBUSTION != talent && MAGE_MOLTEN_FURY != talent && MAGE_FIERY_PAYBACK != talent && MAGE_EMPOWERED_FIRE != talent && MAGE_FIRESTARTER != talent && MAGE_DRAGONS_BREATH != talent && MAGE_HOT_STREAK != talent && MAGE_BURNOUT != talent && MAGE_LIVING_BOMB != talent && MAGE_FROSTBITE != talent && MAGE_IMPROVED_FROSTBOLT != talent && MAGE_ICE_FLOES != talent && MAGE_ICE_SHARDS != talent && MAGE_FROST_WARDING != talent && MAGE_PRECISION != talent && MAGE_PERMAFROST != talent && MAGE_PIERCING_ICE != talent && MAGE_ICY_VEINS != talent && MAGE_IMPROVED_BLIZZARD != talent && MAGE_ARCTIC_REACH != talent && MAGE_FROST_CHANNELING != talent && MAGE_SHATTER != talent && MAGE_COLD_SNAP != talent && MAGE_IMPROVED_CONE_OF_COLD != talent && MAGE_FROZEN_CORE != talent && MAGE_COLD_AS_ICE != talent && MAGE_WINTERS_CHILL != talent && MAGE_SHATTERED_BARRIER != talent && MAGE_ICE_BARRIER != talent && MAGE_ARCTIC_WINDS != talent && MAGE_EMPOWERED_FROSTBOLT != talent && MAGE_FINGERS_OF_FROST != talent && MAGE_BRAIN_FREEZE != talent && MAGE_SUMMON_WATER_ELEMENTAL != talent && MAGE_ENDURING_WINTER != talent && MAGE_CHILLD_TO_THE_BONE != talent && MAGE_DEEP_FREEZE != talent && MAGE_ARCANE_SUBTLETY != talent && MAGE_ARCANE_FOCUS != talent && MAGE_ARCANE_STABILITY != talent && MAGE_ARCANE_FORTITUDE != talent && MAGE_MAGIC_ABSORPTION != talent && MAGE_ARCANE_CONCENTRATION != talent && MAGE_MAGIC_ATTUNEMENT != talent && MAGE_SPELL_IMPACT != talent && MAGE_STUDENT_OF_THE_MIND != talent && MAGE_FOCUS_MAGIC != talent && MAGE_ARCANE_SHIELDING != talent && MAGE_IMPROVED_COUNTERSPELL != talent && MAGE_ARCANE_MEDITATION != talent && MAGE_TORMENT_THE_WEAK != talent && MAGE_IMPROVED_BLINK != talent && MAGE_PRESENCE_OF_MIND != talent && MAGE_ARCANE_MIND != talent && MAGE_PRISMATIC_CLOAK != talent && MAGE_ARCANE_INSTABILITY != talent && MAGE_ARCANE_POTENCY != talent && MAGE_ARCANE_EMPOWERMENT != talent && MAGE_ARCANE_POWER != talent && MAGE_INCANTERS_ABSORPTION != talent && MAGE_ARCANE_FLOWS != talent && MAGE_MIND_MASTERY != talent && MAGE_SLOW != talent && MAGE_MISSILE_BARRAGE != talent && MAGE_NETHERWIND_PRESENCE != talent && MAGE_SPELL_POWER != talent && MAGE_ARCANE_BARRAGE != talent)
            return false;
    }
    else if (charClass == CLASS_PALADIN)
    {
        if (PALADIN_DEFLECTION != talent && PALADIN_BENEDICTION != talent && PALADIN_IMPROVED_JUDGEMENTS != talent && PALADIN_HEART_OF_THE_CRUSADER != talent && PALADIN_IMPROVED_BLESSING_OF_MIGHT != talent && PALADIN_VINDICATION != talent && PALADIN_CONVICTION != talent && PALADIN_SEAL_OF_COMMAND != talent && PALADIN_PURSUIT_OF_JUSTICE != talent && PALADIN_EYE_FOR_AN_EYE != talent && PALADIN_SANCTITY_OF_BATTLE != talent && PALADIN_CRUSADE != talent && PALADIN_TWOHANDED_WEAPON_SPECIALIZATION != talent && PALADIN_SANCTIFIED_RETRIBUTION != talent && PALADIN_VENGEANCE != talent && PALADIN_DIVINE_PURPOSE != talent && PALADIN_THE_ART_OF_WAR != talent && PALADIN_REPENTANCE != talent && PALADIN_JUDGEMENTS_OF_THE_WISE != talent && PALADIN_FANATICISM != talent && PALADIN_SANCTIFIED_WRATH != talent && PALADIN_SWIFT_RETRIBUTION != talent && PALADIN_CRUSADER_STRIKE != talent && PALADIN_SHEATH_OF_LIGHT != talent && PALADIN_RIGHTEOUS_VENGEANCE != talent && PALADIN_DIVINE_STORM != talent && PALADIN_SPIRITUAL_FOCUS != talent && PALADIN_SEALS_OF_THE_PURE != talent && PALADIN_HEALING_LIGHT != talent && PALADIN_DIVINE_INTELLECT != talent && PALADIN_UNYIELDING_FAITH != talent && PALADIN_AURA_MASTERY != talent && PALADIN_ILLUMINATION != talent && PALADIN_IMPROVED_LAY_ON_HANDS != talent && PALADIN_IMPROVED_CONCENTRATION_AURA != talent && PALADIN_IMPROVED_BLESSING_OF_WISDOM != talent && PALADIN_BLESSED_HANDS != talent && PALADIN_PURE_OF_HEART != talent && PALADIN_DIVINE_FAVOR != talent && PALADIN_SANCTIFIED_LIGHT != talent && PALADIN_PURIFYING_POWER != talent && PALADIN_HOLY_POWER != talent && PALADIN_LIGHTS_GRACE != talent && PALADIN_HOLY_SHOCK != talent && PALADIN_BLESSED_LIFE != talent && PALADIN_SACRED_CLEANSING != talent && PALADIN_HOLY_GUIDANCE != talent && PALADIN_DIVINE_ILLUMINATION != talent && PALADIN_JUDGEMENTS_OF_THE_PURE != talent && PALADIN_INFUSION_OF_LIGHT != talent && PALADIN_ENLIGHTENED_JUDGEMENTS != talent && PALADIN_BEACON_OF_LIGHT != talent && PALADIN_DIVINITY != talent && PALADIN_DIVINE_STRENGTH != talent && PALADIN_STOICISM != talent && PALADIN_GUARDIANS_FAVOR != talent && PALADIN_ANTICIPATION != talent && PALADIN_DIVINE_SACRIFICE != talent && PALADIN_IMPROVED_RIGHTEOUS_FURY != talent && PALADIN_TOUGHNESS != talent && PALADIN_DIVINE_GUARDIAN != talent && PALADIN_IMPROVED_HAMMER_OF_JUSTICE != talent && PALADIN_IMPROVED_DEVOTION_AURA != talent && PALADIN_BLESSING_OF_SANCTUARY != talent && PALADIN_RECKONING != talent && PALADIN_SACRED_DUTY != talent && PALADIN_ONEHANDED_WEAPON_SPECIALIZATION != talent && PALADIN_SPIRITUAL_ATTUNEMENT != talent && PALADIN_HOLY_SHIELD != talent && PALADIN_ARDENT_DEFENDER != talent && PALADIN_REDOUBT != talent && PALADIN_COMBAT_EXPERTISE != talent && PALADIN_TOUCHER_BY_THE_LIGHT != talent && PALADIN_AVENGERS_SHIELD != talent && PALADIN_GUARDED_BY_THE_LIGHT != talent && PALADIN_SHIELD_OF_THE_TEMPLAR != talent && PALADIN_JUDGEMENT_OF_THE_JUST != talent && PALADIN_HAMMER_OF_THE_RIGHTEOUS != talent)
            return false;
    }
    else if (charClass == CLASS_PRIEST)
    {
        if (PRIEST_UNBREAKABLE_WILL != talent && PRIEST_TWIN_DISCIPLINES != talent && PRIEST_SILENT_RESOLVE != talent && PRIEST_IMPROVED_INNER_FIRE != talent && PRIEST_IMPROVED_POWER_WORD_FORTITUDE != talent && PRIEST_MARTYRDOM != talent && PRIEST_MEDITATION != talent && PRIEST_INNER_FOCUS != talent && PRIEST_IMPROVED_POWER_WORD_SHIELD != talent && PRIEST_ABSOLUTION != talent && PRIEST_MENTAL_AGILITY != talent && PRIEST_IMPROVED_MANA_BURN != talent && PRIEST_REFLECTIVE_SHIELD != talent && PRIEST_MENTAL_STRENGTH != talent && PRIEST_SOUL_WARDING != talent && PRIEST_FOCUSED_POWER != talent && PRIEST_ENLIGHTENMENT != talent && PRIEST_FOCUSED_WILL != talent && PRIEST_POWER_INFUSION != talent && PRIEST_IMPROVED_FLASH_HEAL != talent && PRIEST_RENEWED_HOPE != talent && PRIEST_RAPTURE != talent && PRIEST_ASPIRATION != talent && PRIEST_DIVINE_AEGIS != talent && PRIEST_PAIN_SUPPRESSION != talent && PRIEST_GRACE != talent && PRIEST_BORROWED_TIME != talent && PRIEST_PENANCE != talent && PRIEST_HEALING_FOCUS != talent && PRIEST_IMPROVED_RENEW != talent && PRIEST_HOLY_SPECIALIZATION != talent && PRIEST_SPELL_WARDING != talent && PRIEST_DIVINE_FURY != talent && PRIEST_DESPERATE_PRAYER != talent && PRIEST_BLESSED_RECOVERY != talent && PRIEST_INSPIRATION != talent && PRIEST_HOLY_REACH != talent && PRIEST_IMPROVED_HEALIN != talent && PRIEST_SEARING_LIGHT != talent && PRIEST_HEALING_PRAYERS != talent && PRIEST_SPIRIT_OF_REDEMPTION != talent && PRIEST_SPIRITUAL_GUIDANCE != talent && PRIEST_SURGE_OF_LIGHT != talent && PRIEST_SPIRITUAL_HEALING != talent && PRIEST_HOLY_CONCENTRATION != talent && PRIEST_LIGHTWELL != talent && PRIEST_BLESSED_RESILIENCE != talent && PRIEST_BODY_AND_SOUL != talent && PRIEST_EMPOWERED_HEALING != talent && PRIEST_SERENDIPITY != talent && PRIEST_EMPOWERED_RENEW != talent && PRIEST_CIRCLE_OF_HEALING != talent && PRIEST_TEST_OF_FAITH != talent && PRIEST_DIVINE_PROVIDENCE != talent && PRIEST_GUARDIAN_SPIRIT != talent && PRIEST_SPIRIT_TAP != talent && PRIEST_IMPROVED_SPIRIT_TAP != talent && PRIEST_DARKNESS != talent && PRIEST_SHADOW_AFFINITY != talent && PRIEST_IMPROVED_SHADOW_WORD_PAIN != talent && PRIEST_SHADOW_FOCUS != talent && PRIEST_IMPROVED_PSYCHIC_SCREAM != talent && PRIEST_IMPROVED_MIND_BLAST != talent && PRIEST_MIND_FLAY != talent && PRIEST_VEILED_SHADOWS != talent && PRIEST_SHADOW_REACH != talent && PRIEST_SHADOW_WEAVING != talent && PRIEST_SILENCE != talent && PRIEST_VAMPIRIC_EMBRACE != talent && PRIEST_IMPROVED_VAMPIRIC_EMBRACE != talent && PRIEST_FOCUSED_MIND != talent && PRIEST_MIND_MELT != talent && PRIEST_IMPROVED_DEVOURING_PLAGUE != talent && PRIEST_SHADOWFORM != talent && PRIEST_SHADOW_POWER != talent && PRIEST_IMPROVED_SHADOWFORM != talent && PRIEST_MISERY != talent && PRIEST_PSYCHIC_HORROR != talent && PRIEST_VAMPIRIC_TOUCH != talent && PRIEST_PAIN_AND_SUFFERING != talent && PRIEST_TWISTED_FAITH != talent && PRIEST_DISPERSION != talent)
            return false;
    }
    else if (charClass == CLASS_ROGUE)
    {
        if (ROGUE_IMPROVED_GOUGE != talent && ROGUE_IMPROVED_SINISTER_STRIKE != talent && ROGUE_DUAL_WIELD_SPECIALIZATION != talent && ROGUE_IMPROVED_SLICE_AND_DICE != talent && ROGUE_DEFLECTION != talent && ROGUE_PRECISION != talent && ROGUE_ENDURANCE != talent && ROGUE_RIPOSTE != talent && ROGUE_CLOSE_QUARTERS_COMBAT != talent && ROGUE_IMPROVED_KICK != talent && ROGUE_IMPROVED_SPRINT != talent && ROGUE_LIGHTNING_REFLEXES != talent && ROGUE_AGGRESSION != talent && ROGUE_MACE_SPECIALIZATION != talent && ROGUE_BLADE_FLURRY != talent && ROGUE_HACK_AND_SLASH != talent && ROGUE_WEAPON_EXPERTISE != talent && ROGUE_BLADE_TWISTING != talent && ROGUE_VITALITY != talent && ROGUE_ADRENALINE_RUSH != talent && ROGUE_NERVES_OF_STEEL != talent && ROGUE_THROWING_SPECIALIZATION != talent && ROGUE_COMBAT_POTENCY != talent && ROGUE_UNFAIR_ADVANTAGE != talent && ROGUE_SURPRISE_ATTACKS != talent && ROGUE_SAVAGE_COMBAT != talent && ROGUE_PREY_ON_THE_WEAK != talent && ROGUE_KILLING_SPREE != talent && ROGUE_IMPROVED_EVISCERATE != talent && ROGUE_REMORSELESS_ATTACKS != talent && ROGUE_MALICE != talent && ROGUE_RUTHLESSNESS != talent && ROGUE_BLOOD_SPATTER != talent && ROGUE_PUNCTURING_WOUNDS != talent && ROGUE_VIGOR != talent && ROGUE_IMPROVED_EXPOSE_ARMOR != talent && ROGUE_LETHALITY != talent && ROGUE_VILE_POISONS != talent && ROGUE_IMPROVED_POISONS != talent && ROGUE_FLEET_FOOTED != talent && ROGUE_COLD_BLOOD != talent && ROGUE_IMPROVED_KIDNEY_SHOT != talent && ROGUE_QUICK_RECOVERY != talent && ROGUE_SEAL_FATE != talent && ROGUE_MURDER != talent && ROGUE_DEADLY_BREW != talent && ROGUE_OVERKILL != talent && ROGUE_DEADENED_NERVES != talent && ROGUE_FOCUSED_ATTACKS != talent && ROGUE_FIND_WEAKNESS != talent && ROGUE_MASTER_POISONER != talent && ROGUE_MUTILATE != talent && ROGUE_TURN_THE_TABLES != talent && ROGUE_CUT_TO_THE_CHASE != talent && ROGUE_HUNGER_FOR_BLOOD != talent && ROGUE_RELENTLESS_STRIKES != talent && ROGUE_MASTER_OF_DECEPTION != talent && ROGUE_OPPORTUNITY != talent && ROGUE_SLEIGHT_OF_HAND != talent && ROGUE_DIRTY_TRICKS != talent && ROGUE_CAMOUFLAGE != talent && ROGUE_ELUSIVENESS != talent && ROGUE_GHOSTLY_STRIKE != talent && ROGUE_SERRATED_BLADES != talent && ROGUE_SETUP != talent && ROGUE_INITIATIVE != talent && ROGUE_IMPROVED_AMBUSH != talent && ROGUE_HEIGHTENED_SENSES != talent && ROGUE_PREPARATION != talent && ROGUE_DIRTY_DEEDS != talent && ROGUE_HEMORRHAGE != talent && ROGUE_MASTER_OF_SUBTLETY != talent && ROGUE_DEADLINESS != talent && ROGUE_ENVELOPING_SHADOWS != talent && ROGUE_PREMEDITATION != talent && ROGUE_CHEAT_DEATH != talent && ROGUE_SINISTER_CALLING != talent && ROGUE_WAYLAY != talent && ROGUE_HONOR_AMONG_THIEVES != talent && ROGUE_SHADOWSTEP != talent && ROGUE_FILTHY_TRICKS != talent && ROGUE_SLAUGHTER_FROM_THE_SHADOWS != talent && ROGUE_SHADOW_DANCE != talent)
            return false;
    }
    else if (charClass == CLASS_SHAMAN)
    {
        if (SHAMAN_CONVECTION != talent && SHAMAN_CONCUSSION != talent && SHAMAN_CALL_OF_FLAME != talent && SHAMAN_ELEMENTAL_WARDING != talent && SHAMAN_ELEMENTAL_DEVASTATION != talent && SHAMAN_REVERBERATION != talent && SHAMAN_ELEMENTAL_FOCUS != talent && SHAMAN_ELEMENTAL_FURY != talent && SHAMAN_IMPROVED_FIRE_NOVA != talent && SHAMAN_EYE_OF_THE_STORM != talent && SHAMAN_ELEMENTAL_REACH != talent && SHAMAN_CALL_OF_THUNDER != talent && SHAMAN_UNRELENTING_STORM != talent && SHAMAN_ELEMENTAL_PRECISION != talent && SHAMAN_LIGHTNING_MASTERY != talent && SHAMAN_ELEMENTAL_MASTERY != talent && SHAMAN_STORM_EARTH_AND_FIRE != talent && SHAMAN_BOOMING_ECHOES != talent && SHAMAN_ELEMENTAL_OATH != talent && SHAMAN_LIGHTNING_OVERLOAD != talent && SHAMAN_ASTRAL_SHIFT != talent && SHAMAN_TOTEM_OF_WRATH != talent && SHAMAN_LAVA_FLOWS != talent && SHAMAN_SHAMANISM != talent && SHAMAN_THUNDERSTORM != talent && SHAMAN_IMPROVED_HEALING_WAVE != talent && SHAMAN_TOTEMIC_FOCUS != talent && SHAMAN_IMPROVED_REINCARNATION != talent && SHAMAN_HEALING_GRACE != talent && SHAMAN_TIDAL_FOCUS != talent && SHAMAN_IMPROVED_WATER_SHIELD != talent && SHAMAN_HEALING_FOCUS != talent && SHAMAN_TIDAL_FORCE != talent && SHAMAN_ANCESTRAL_HEALING != talent && SHAMAN_RESTORATIVE_TOTEMS != talent && SHAMAN_TIDAL_MASTERY != talent && SHAMAN_HEALING_WAY != talent && SHAMAN_NATURES_SWIFTNESS != talent && SHAMAN_FOCUSED_MIND != talent && SHAMAN_PURIFICATION != talent && SHAMAN_NATURES_GUARDIAN != talent && SHAMAN_MANA_TIDE_TOTEM != talent && SHAMAN_CLEANSE_SPIRIT != talent && SHAMAN_BLESSING_OF_THE_ETERNALS != talent && SHAMAN_IMPROVED_CHAIN_HEAL != talent && SHAMAN_NATURES_BLESSING != talent && SHAMAN_ANCESTRAL_AWAKENING != talent && SHAMAN_EARTH_SHIELD != talent && SHAMAN_IMPROVED_EARTH_SHIELD != talent && SHAMAN_TIDAL_WAVES != talent && SHAMAN_RIPTIDE != talent && SHAMAN_ENHANCING_TOTEMS != talent && SHAMAN_EARTHS_GRASP != talent && SHAMAN_ANCESTRAL_KNOWLEDGE != talent && SHAMAN_GUARDIAN_TOTEMS != talent && SHAMAN_THUNDERING_STRIKES != talent && SHAMAN_IMPROVED_GHOST_WOLF != talent && SHAMAN_IMPROVED_SHIELDS != talent && SHAMAN_ELEMENTAL_WEAPONS != talent && SHAMAN_SHAMANISTIC_FOCUS != talent && SHAMAN_ANTICIPATION != talent && SHAMAN_FLURRY != talent && SHAMAN_TOUGHNESS != talent && SHAMAN_IMPROVED_WINDFURY_TOTEM != talent && SHAMAN_SPIRIT_WEAPONS != talent && SHAMAN_MENTAL_DEXTERITY != talent && SHAMAN_UNLEASHED_RAGE != talent && SHAMAN_WEAPON_MASTERY != talent && SHAMAN_FROZEN_POWER != talent && SHAMAN_DUAL_WIELD_SPECIALIZATION != talent && SHAMAN_DUAL_WIELD != talent && SHAMAN_STORMSTRIKE != talent && SHAMAN_STATIC_SHOCK != talent && SHAMAN_LAVA_LASH != talent && SHAMAN_IMPROVED_STORMSTRIKE != talent && SHAMAN_MENTAL_QUICKNESS != talent && SHAMAN_SHAMANISTIC_RAGE != talent && SHAMAN_EARTHEN_POWER != talent && SHAMAN_MAELSTROM_WEAPON != talent && SHAMAN_FERAL_SPIRIT != talent)
            return false;
    }
    else if (charClass == CLASS_WARLOCK)
    {
        if (WARLOCK_IMPROVED_SHADOW_BOLT != talent && WARLOCK_BANE != talent && WARLOCK_AFTERMATH != talent && WARLOCK_MOLTEN_SKIN != talent && WARLOCK_CATACLYSM != talent && WARLOCK_DEMONIC_POWER != talent && WARLOCK_SHADOWBURN != talent && WARLOCK_RUIN != talent && WARLOCK_INTENSITY != talent && WARLOCK_DESTRUCTIVE_REACH != talent && WARLOCK_IMPROVED_SEARING_PAIN != talent && WARLOCK_BACKLASH != talent && WARLOCK_IMPROVED_IMMOLATE != talent && WARLOCK_DEVASTATION != talent && WARLOCK_NETHER_PROTECTION != talent && WARLOCK_EMBERSTORM != talent && WARLOCK_CONFLAGRATE != talent && WARLOCK_SOUL_LEECH != talent && WARLOCK_PYROCLASM != talent && WARLOCK_SHADOW_AND_FLAME != talent && WARLOCK_IMPROVED_SOUL_LEECH != talent && WARLOCK_BACKDRAFT != talent && WARLOCK_SHADOWFURY != talent && WARLOCK_EMPOWERED_IMP != talent && WARLOCK_FIRE_AND_BRIMSTONE != talent && WARLOCK_CHAOS_BOLT != talent && WARLOCK_IMPROVED_CURSE_OF_AGONY != talent && WARLOCK_SUPPRESSION != talent && WARLOCK_IMPROVED_CORRUPTION != talent && WARLOCK_IMPROVED_CURSE_OF_WEAKNESS != talent && WARLOCK_IMPROVED_DRAIN_SOUL != talent && WARLOCK_IMPROVED_LIFE_TAP != talent && WARLOCK_SOUL_SIPHON != talent && WARLOCK_IMPROVED_FEAR != talent && WARLOCK_FEL_CONCENTRATION != talent && WARLOCK_AMPLIFY_CURSE != talent && WARLOCK_GRIM_REACH != talent && WARLOCK_NIGHTFALL != talent && WARLOCK_EMPOWERED_CORRUPTION != talent && WARLOCK_SHADOW_EMBRACE != talent && WARLOCK_SIPHON_LIFE != talent && WARLOCK_CURSE_OF_EXHAUSTION != talent && WARLOCK_IMPROVED_FELHUNTER != talent && WARLOCK_SHADOW_MASTERY != talent && WARLOCK_ERADICATION != talent && WARLOCK_CONTAGION != talent && WARLOCK_DARK_PACT != talent && WARLOCK_IMPROVED_HOWL_OF_TERROR != talent && WARLOCK_MALEDICTION != talent && WARLOCK_DEATHS_EMBRACE != talent && WARLOCK_UNSTABLE_AFFLICTION != talent && WARLOCK_PANDEMIC != talent && WARLOCK_EVERLASTING_AFFLICTION != talent && WARLOCK_HAUNT != talent && WARLOCK_IMPROVED_HEALTHSTONE != talent && WARLOCK_IMPROVED_IMP != talent && WARLOCK_DEMONIC_EMBRACE != talent && WARLOCK_FEL_SYNERGY != talent && WARLOCK_IMPROVED_HEALTH_FUNNEL != talent && WARLOCK_DEMONIC_BRUTALITY != talent && WARLOCK_FEL_VITALITY != talent && WARLOCK_IMPROVED_SUCCUBUS != talent && WARLOCK_SOUL_LINK != talent && WARLOCK_FEL_DOMINATION != talent && WARLOCK_DEMONIC_AEGIS != talent && WARLOCK_UNHOLY_POWER != talent && WARLOCK_MASTER_SUMMONER != talent && WARLOCK_MANA_FEED != talent && WARLOCK_MASTER_CONJURER != talent && WARLOCK_MASTER_DEMONOLOGIST != talent && WARLOCK_MOLTEN_CORE != talent && WARLOCK_DEMONIC_RESILIENCE != talent && WARLOCK_DEMONIC_EMPOWERMENT != talent && WARLOCK_DEMONIC_KNOWLEDGE != talent && WARLOCK_DEMONIC_TACTICS != talent && WARLOCK_DECIMATION != talent && WARLOCK_IMPROVED_DEMONIC_TACTICS != talent && WARLOCK_SUMMON_FELGUARD != talent && WARLOCK_NEMESIS != talent && WARLOCK_DEMONIC_PACT != talent && WARLOCK_METAMORPHOSIS != talent)
            return false;
    }
    else if (charClass == CLASS_WARRIOR)
    {
        if (WARRIOR_IMPROVED_HEROIC_STRIKE != talent && WARRIOR_DEFLECTION != talent && WARRIOR_IMPROVED_REND != talent && WARRIOR_IMPROVED_CHARGE != talent && WARRIOR_IRON_WILL != talent && WARRIOR_TACTICAL_MASTERY != talent && WARRIOR_IMPROVED_OVERPOWER != talent && WARRIOR_ANGER_MANAGEMENT != talent && WARRIOR_IMPALE != talent && WARRIOR_DEEP_WOUNDS != talent && WARRIOR_TWOHANDED_WEAPON_SPECIALIZATION != talent && WARRIOR_TASTE_FOR_BLOOD != talent && WARRIOR_POLEAXE_SPECIALIZATION != talent && WARRIOR_SWEEPING_STRIKES != talent && WARRIOR_MACE_SPECIALIZATION != talent && WARRIOR_SWORD_SPECIALIZATION != talent && WARRIOR_WEAPON_MASTERY != talent && WARRIOR_IMPROVED_HAMSTRING != talent && WARRIOR_TRAUMA != talent && WARRIOR_SECOND_WIND != talent && WARRIOR_MORTAL_STRIKE != talent && WARRIOR_STRENGTH_OF_ARMS != talent && WARRIOR_IMPROVED_SLAM != talent && WARRIOR_JUGGERNAUT != talent && WARRIOR_IMPROVED_MORTAL_STRIKE != talent && WARRIOR_UNRELENTING_ASSAULT != talent && WARRIOR_SUDDEN_DEATH != talent && WARRIOR_ENDLESS_RAGE != talent && WARRIOR_BLOOD_FRENZY != talent && WARRIOR_WRECKING_CREW != talent && WARRIOR_BLADESTORM != talent && WARRIOR_IMPROVED_BLOODRAGE != talent && WARRIOR_SHIELD_SPECIALIZATION != talent && WARRIOR_IMPROVED_THUNDER_CLAP != talent && WARRIOR_INCITE != talent && WARRIOR_ANTICIPATION != talent && WARRIOR_LAST_STAND != talent && WARRIOR_IMPROVED_REVENGE != talent && WARRIOR_SHIELD_MASTERY != talent && WARRIOR_TOUGHNESS != talent && WARRIOR_IMPROVED_SPELL_REFLECTION != talent && WARRIOR_IMPROVED_DISARM != talent && WARRIOR_PUNCTURE != talent && WARRIOR_IMPROVED_DISCIPLINES != talent && WARRIOR_CONCUSSION_BLOW != talent && WARRIOR_GAG_ORDER != talent && WARRIOR_ONEHANDED_WEAPON_SPECIALIZATION != talent && WARRIOR_IMPROVED_DEFENSIVE_STANCE != talent && WARRIOR_VIGILANCE != talent && WARRIOR_FOCUSED_RAGE != talent && WARRIOR_VITALITY != talent && WARRIOR_SAFEGUARD != talent && WARRIOR_WARBRINGER != talent && WARRIOR_DEVASTATE != talent && WARRIOR_CRITICAL_BLOCK != talent && WARRIOR_SWORD_AND_BOARD != talent && WARRIOR_DAMAGE_SHIELD != talent && WARRIOR_SHOCKWAVE != talent && WARRIOR_ARMORED_TO_THE_TEETH != talent && WARRIOR_BOOMING_VOICE != talent && WARRIOR_CRUELTY != talent && WARRIOR_IMPROVED_DEMORALIZING_SHOUT != talent && WARRIOR_UNBRIDLED_WRATH != talent && WARRIOR_IMPROVED_CLEAVE != talent && WARRIOR_PIERCING_HOWL != talent && WARRIOR_BLOOD_CRAZE != talent && WARRIOR_COMMANDING_PRESENCE != talent && WARRIOR_DUAL_WIELD_SPECIALIZATION != talent && WARRIOR_IMPROVED_EXECUTE != talent && WARRIOR_ENRAGE != talent && WARRIOR_PRECISION != talent && WARRIOR_DEATH_WISH != talent && WARRIOR_IMPROVED_INTERCEPT != talent && WARRIOR_IMPROVED_BERSERKER_RAGE != talent && WARRIOR_FLURRY != talent && WARRIOR_INTENSIFY_RAGE != talent && WARRIOR_BLOODTHIRST != talent && WARRIOR_IMPROVED_WHIRLWIND != talent && WARRIOR_FURIOUS_ATTACKS != talent && WARRIOR_IMPROVED_BERSERKER_STANCE != talent && WARRIOR_HEROIC_FURY != talent && WARRIOR_RAMPAGE != talent && WARRIOR_BLOODSURGE != talent && WARRIOR_UNENDING_FURY != talent && WARRIOR_TITANS_GRIP != talent)
            return false;
    }
    else if (charClass == CLASS_PET_CUNNING)
    {
        if (PET_CUNNING_COBRA_REFLEXES != talent && PET_CUNNING_DASHDIVE1 != talent && PET_CUNNING_DASHDIVE2 != talent && PET_CUNNING_GREAT_STAMINA != talent && PET_CUNNING_NATURAL_ARMOR != talent && PET_CUNNING_BOARS_SPEED != talent && PET_CUNNING_MOBILITY1 != talent && PET_CUNNING_MOBILITY2 != talent && PET_CUNNING_OWLS_FOCUS != talent && PET_CUNNING_SPIKED_COLLAR != talent && PET_CUNNING_CULLING_THE_HERD != talent && PET_CUNNING_LIONHEARTED != talent && PET_CUNNING_CARRION_FEEDER != talent && PET_CUNNING_GREAT_RESISTANCE != talent && PET_CUNNING_CORNERED != talent && PET_CUNNING_FEEDING_FRENZY != talent && PET_CUNNING_WOLVERINE_BITE != talent && PET_CUNNING_ROAR_OF_RECOVERY != talent && PET_CUNNING_BULLHEADED != talent && PET_CUNNING_GRACE_OF_THE_MANTIS != talent && PET_CUNNING_WILD_HUNT != talent && PET_CUNNING_ROAR_OF_SACRIFICE != talent)
            return false;
    }
    else if (charClass == CLASS_PET_FEROCITY)
    {
        if (PET_FEROCITY_COBRA_REFLEXES != talent && PET_FEROCITY_DASHDIVE1 != talent && PET_FEROCITY_DASHDIVE2 != talent && PET_FEROCITY_GREAT_STAMINA != talent && PET_FEROCITY_NATURAL_ARMOR != talent && PET_FEROCITY_IMPROVED_COWER != talent && PET_FEROCITY_BLOODTHIRSTY != talent && PET_FEROCITY_SPIKED_COLLAR != talent && PET_FEROCITY_BOARS_SPEED != talent && PET_FEROCITY_CULLING_THE_HERD != talent && PET_FEROCITY_LIONHEARTED != talent && PET_FEROCITY_CHARGESWOOP1 != talent && PET_FEROCITY_CHARGESWOOP2 != talent && PET_FEROCITY_HEART_OF_THE_PHOENIX != talent && PET_FEROCITY_SPIDERS_BITE != talent && PET_FEROCITY_GREAT_RESISTANCE != talent && PET_FEROCITY_RABID != talent && PET_FEROCITY_LICK_YOUR_WOUNDS != talent && PET_FEROCITY_CALL_OF_THE_WILD != talent && PET_FEROCITY_SHARK_ATTACK != talent && PET_FEROCITY_WILD_HUNT != talent)
            return false;
    }
    else if (charClass == CLASS_PET_TENACITY)
    {
        if (PET_TENACITY_COBRA_REFLEXES != talent && PET_TENACITY_CHARGE != talent && PET_TENACITY_GREAT_STAMINA != talent && PET_TENACITY_NATURAL_ARMOR != talent && PET_TENACITY_SPIKED_COLLAR != talent && PET_TENACITY_BOARS_SPEED != talent && PET_TENACITY_BLOOD_OF_THE_RHINO != talent && PET_TENACITY_PET_BARDING != talent && PET_TENACITY_CULLING_THE_HERD != talent && PET_TENACITY_GUARD_DOG != talent && PET_TENACITY_LIONHEARTED != talent && PET_TENACITY_THUNDERSTOMP != talent && PET_TENACITY_GRACE_OF_THE_MANTIS != talent && PET_TENACITY_GREAT_RESISTANCE != talent && PET_TENACITY_LAST_STAND != talent && PET_TENACITY_TAUNT != talent && PET_TENACITY_ROAR_OF_SACRIFICE != talent && PET_TENACITY_INTERVENE != talent && PET_TENACITY_SILVERBACK != talent && PET_TENACITY_WILD_HUNT != talent)
            return false;
    }
    else // charClass unknown
    {
        DEBUG_LOG("[PlayerbotAI]: Someone was naughty and supplied an invalid class to ValidateTalent: %u", (uint32) charClass);
        return false;
    }

    return true;
}

/**
* ValidateGlyph tests a glyph against class to see if it belongs to that class - accepts both Major and Minor glyphs
*
* uint16 glyph:        glyph ID
* long charClass:    member of the Classes enum or ClassesCombatPets enum
*
* return true  -> ok
* return false -> not a valid glyph for that class
*/
bool PlayerbotAI::ValidateGlyph(uint16 glyph, long charClass)
{
    // XOR the two helper functions. Both true (supposedly impossible) or both false -> false
    return ValidateMajorGlyph(glyph, charClass) ^ ValidateMinorGlyph(glyph, charClass);
}

/**
* ValidateMajorGlyph tests a glyph against class to see if it belongs to that class - only accepts Major glyphs
*
* uint16 glyph:        glyph ID
* long charClass:    member of the Classes enum or ClassesCombatPets enum
*
* return true  -> ok
* return false -> not a valid major glyph for that class
*/
bool PlayerbotAI::ValidateMajorGlyph(uint16 glyph, long charClass)
{
    if (charClass == CLASS_DEATH_KNIGHT)
    {
        // this looong 'if' is to see if any glyph is not a Death Knight glyph when the class clearly is
        if (DEATH_KNIGHT_MAJOR_GLYPH_OF_DARK_COMMAND != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_ANTIMAGIC_SHELL != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_HEART_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_BONE_SHIELD != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_CHAINS_OF_ICE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_GRIP != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_AND_DECAY != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_FROST_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_ICEBOUND_FORTITUDE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_ICY_TOUCH != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_OBLITERATE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_PLAGUE_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_THE_GHOUL != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_RUNE_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_SCOURGE_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_STRANGULATE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_UNBREAKABLE_ARMOR != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_VAMPIRIC_BLOOD != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_RUNE_TAP != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_BLOOD_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DEATH_STRIKE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DANCING_RUNE_WEAPON != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_HUNGERING_COLD != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_UNHOLY_BLIGHT != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DARK_DEATH != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_DISEASE != glyph && DEATH_KNIGHT_MAJOR_GLYPH_OF_HOWLING_BLAST != glyph)
            return false;
    }
    else if (charClass == CLASS_DRUID)
    {
        if (DRUID_MAJOR_GLYPH_OF_FRENZIED_REGENERATION != glyph && DRUID_MAJOR_GLYPH_OF_MAUL != glyph && DRUID_MAJOR_GLYPH_OF_MANGLE != glyph && DRUID_MAJOR_GLYPH_OF_SHRED != glyph && DRUID_MAJOR_GLYPH_OF_RIP != glyph && DRUID_MAJOR_GLYPH_OF_RAKE != glyph && DRUID_MAJOR_GLYPH_OF_SWIFTMEND != glyph && DRUID_MAJOR_GLYPH_OF_INNERVATE != glyph && DRUID_MAJOR_GLYPH_OF_REBIRTH != glyph && DRUID_MAJOR_GLYPH_OF_REGROWTH != glyph && DRUID_MAJOR_GLYPH_OF_REJUVENATION != glyph && DRUID_MAJOR_GLYPH_OF_HEALING_TOUCH != glyph && DRUID_MAJOR_GLYPH_OF_LIFEBLOOM != glyph && DRUID_MAJOR_GLYPH_OF_STARFIRE != glyph && DRUID_MAJOR_GLYPH_OF_INSECT_SWARM != glyph && DRUID_MAJOR_GLYPH_OF_HURRICANE != glyph && DRUID_MAJOR_GLYPH_OF_STARFALL != glyph && DRUID_MAJOR_GLYPH_OF_WRATH != glyph && DRUID_MAJOR_GLYPH_OF_MOONFIRE != glyph && DRUID_MAJOR_GLYPH_OF_ENTANGLING_ROOTS != glyph && DRUID_MAJOR_GLYPH_OF_FOCUS != glyph && DRUID_MAJOR_GLYPH_OF_BERSERK != glyph && DRUID_MAJOR_GLYPH_OF_WILD_GROWTH != glyph && DRUID_MAJOR_GLYPH_OF_NOURISH != glyph && DRUID_MAJOR_GLYPH_OF_SAVAGE_ROAR != glyph && DRUID_MAJOR_GLYPH_OF_MONSOON != glyph && DRUID_MAJOR_GLYPH_OF_BARKSKIN != glyph && DRUID_MAJOR_GLYPH_OF_SURVIVAL_INSTINCTS != glyph && DRUID_MAJOR_GLYPH_OF_CLAW != glyph && DRUID_MAJOR_GLYPH_OF_RAPID_REJUVENATION != glyph)
            return false;
    }
    else if (charClass == CLASS_HUNTER)
    {
        if (HUNTER_MAJOR_GLYPH_OF_AIMED_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_ARCANE_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_THE_BEAST != glyph && HUNTER_MAJOR_GLYPH_OF_MENDING != glyph && HUNTER_MAJOR_GLYPH_OF_ASPECT_OF_THE_VIPER != glyph && HUNTER_MAJOR_GLYPH_OF_BESTIAL_WRATH != glyph && HUNTER_MAJOR_GLYPH_OF_DETERRENCE != glyph && HUNTER_MAJOR_GLYPH_OF_DISENGAGE != glyph && HUNTER_MAJOR_GLYPH_OF_FREEZING_TRAP != glyph && HUNTER_MAJOR_GLYPH_OF_FROST_TRAP != glyph && HUNTER_MAJOR_GLYPH_OF_HUNTERS_MARK != glyph && HUNTER_MAJOR_GLYPH_OF_IMMOLATION_TRAP != glyph && HUNTER_MAJOR_GLYPH_OF_MULTISHOT != glyph && HUNTER_MAJOR_GLYPH_OF_RAPID_FIRE != glyph && HUNTER_MAJOR_GLYPH_OF_SERPENT_STING != glyph && HUNTER_MAJOR_GLYPH_OF_SNAKE_TRAP != glyph && HUNTER_MAJOR_GLYPH_OF_STEADY_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_TRUESHOT_AURA != glyph && HUNTER_MAJOR_GLYPH_OF_VOLLEY != glyph && HUNTER_MAJOR_GLYPH_OF_WYVERN_STING != glyph && HUNTER_MAJOR_GLYPH_OF_CHIMERA_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_EXPLOSIVE_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_KILL_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_EXPLOSIVE_TRAP != glyph && HUNTER_MAJOR_GLYPH_OF_SCATTER_SHOT != glyph && HUNTER_MAJOR_GLYPH_OF_RAPTOR_STRIKE != glyph)
            return false;
    }
    else if (charClass == CLASS_MAGE)
    {
        if (MAGE_MAJOR_GLYPH_OF_ARCANE_EXPLOSION != glyph && MAGE_MAJOR_GLYPH_OF_ARCANE_MISSILES != glyph && MAGE_MAJOR_GLYPH_OF_ARCANE_POWER != glyph && MAGE_MAJOR_GLYPH_OF_BLINK != glyph && MAGE_MAJOR_GLYPH_OF_EVOCATION != glyph && MAGE_MAJOR_GLYPH_OF_FIREBALL != glyph && MAGE_MAJOR_GLYPH_OF_FIRE_BLAST != glyph && MAGE_MAJOR_GLYPH_OF_FROST_NOVA != glyph && MAGE_MAJOR_GLYPH_OF_FROSTBOLT != glyph && MAGE_MAJOR_GLYPH_OF_ICE_ARMOR != glyph && MAGE_MAJOR_GLYPH_OF_ICE_BLOCK != glyph && MAGE_MAJOR_GLYPH_OF_ICE_LANCE != glyph && MAGE_MAJOR_GLYPH_OF_ICY_VEINS != glyph && MAGE_MAJOR_GLYPH_OF_SCORCH != glyph && MAGE_MAJOR_GLYPH_OF_INVISIBILITY != glyph && MAGE_MAJOR_GLYPH_OF_MAGE_ARMOR != glyph && MAGE_MAJOR_GLYPH_OF_MANA_GEM != glyph && MAGE_MAJOR_GLYPH_OF_MOLTEN_ARMOR != glyph && MAGE_MAJOR_GLYPH_OF_POLYMORPH != glyph && MAGE_MAJOR_GLYPH_OF_REMOVE_CURSE != glyph && MAGE_MAJOR_GLYPH_OF_WATER_ELEMENTAL != glyph && MAGE_MAJOR_GLYPH_OF_FROSTFIRE != glyph && MAGE_MAJOR_GLYPH_OF_ARCANE_BLAST != glyph && MAGE_MAJOR_GLYPH_OF_DEEP_FREEZE != glyph && MAGE_MAJOR_GLYPH_OF_LIVING_BOMB != glyph && MAGE_MAJOR_GLYPH_OF_ARCANE_BARRAGE != glyph && MAGE_MAJOR_GLYPH_OF_MIRROR_IMAGE != glyph && MAGE_MAJOR_GLYPH_OF_ICE_BARRIER != glyph && MAGE_MAJOR_GLYPH_OF_ETERNAL_WATER != glyph)
            return false;
    }
    else if (charClass == CLASS_PALADIN)
    {
        if (PALADIN_MAJOR_GLYPH_OF_JUDGEMENT != glyph && PALADIN_MAJOR_GLYPH_OF_SEAL_OF_COMMAND != glyph && PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_JUSTICE != glyph && PALADIN_MAJOR_GLYPH_OF_SPIRITUAL_ATTUNEMENT != glyph && PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_WRATH != glyph && PALADIN_MAJOR_GLYPH_OF_CRUSADER_STRIKE != glyph && PALADIN_MAJOR_GLYPH_OF_CONSECRATION != glyph && PALADIN_MAJOR_GLYPH_OF_RIGHTEOUS_DEFENSE != glyph && PALADIN_MAJOR_GLYPH_OF_AVENGERS_SHIELD != glyph && PALADIN_MAJOR_GLYPH_OF_TURN_EVIL != glyph && PALADIN_MAJOR_GLYPH_OF_EXORCISM != glyph && PALADIN_MAJOR_GLYPH_OF_CLEANSING != glyph && PALADIN_MAJOR_GLYPH_OF_FLASH_OF_LIGHT != glyph && PALADIN_MAJOR_GLYPH_OF_HOLY_LIGHT != glyph && PALADIN_MAJOR_GLYPH_OF_AVENGING_WRATH != glyph && PALADIN_MAJOR_GLYPH_OF_DIVINITY != glyph && PALADIN_MAJOR_GLYPH_OF_SEAL_OF_WISDOM != glyph && PALADIN_MAJOR_GLYPH_OF_SEAL_OF_LIGHT != glyph && PALADIN_MAJOR_GLYPH_OF_HOLY_WRATH != glyph && PALADIN_MAJOR_GLYPH_OF_SEAL_OF_RIGHTEOUSNESS != glyph && PALADIN_MAJOR_GLYPH_OF_SEAL_OF_VENGEANCE != glyph && PALADIN_MAJOR_GLYPH_OF_BEACON_OF_LIGHT != glyph && PALADIN_MAJOR_GLYPH_OF_HAMMER_OF_THE_RIGHTEOUS != glyph && PALADIN_MAJOR_GLYPH_OF_DIVINE_STORM != glyph && PALADIN_MAJOR_GLYPH_OF_SHIELD_OF_RIGHTEOUSNESS != glyph && PALADIN_MAJOR_GLYPH_OF_DIVINE_PLEA != glyph && PALADIN_MAJOR_GLYPH_OF_HOLY_SHOCK != glyph && PALADIN_MAJOR_GLYPH_OF_SALVATION != glyph)
            return false;
    }
    else if (charClass == CLASS_PRIEST)
    {
        if (PRIEST_MAJOR_GLYPH_OF_CIRCLE_OF_HEALING != glyph && PRIEST_MAJOR_GLYPH_OF_DISPEL_MAGIC != glyph && PRIEST_MAJOR_GLYPH_OF_FADE != glyph && PRIEST_MAJOR_GLYPH_OF_FEAR_WARD != glyph && PRIEST_MAJOR_GLYPH_OF_FLASH_HEAL != glyph && PRIEST_MAJOR_GLYPH_OF_HOLY_NOVA != glyph && PRIEST_MAJOR_GLYPH_OF_INNER_FIRE != glyph && PRIEST_MAJOR_GLYPH_OF_LIGHTWELL != glyph && PRIEST_MAJOR_GLYPH_OF_MASS_DISPEL != glyph && PRIEST_MAJOR_GLYPH_OF_MIND_CONTROL != glyph && PRIEST_MAJOR_GLYPH_OF_SHADOW_WORD_PAIN != glyph && PRIEST_MAJOR_GLYPH_OF_SHADOW != glyph && PRIEST_MAJOR_GLYPH_OF_POWER_WORD_SHIELD != glyph && PRIEST_MAJOR_GLYPH_OF_PRAYER_OF_HEALING != glyph && PRIEST_MAJOR_GLYPH_OF_PSYCHIC_SCREAM != glyph && PRIEST_MAJOR_GLYPH_OF_RENEW != glyph && PRIEST_MAJOR_GLYPH_OF_SCOURGE_IMPRISONMENT != glyph && PRIEST_MAJOR_GLYPH_OF_SHADOW_WORD_DEATH != glyph && PRIEST_MAJOR_GLYPH_OF_MIND_FLAY != glyph && PRIEST_MAJOR_GLYPH_OF_SMITE != glyph && PRIEST_MAJOR_GLYPH_OF_SPIRIT_OF_REDEMPTION != glyph && PRIEST_MAJOR_GLYPH_OF_DISPERSION != glyph && PRIEST_MAJOR_GLYPH_OF_GUARDIAN_SPIRIT != glyph && PRIEST_MAJOR_GLYPH_OF_PENANCE != glyph && PRIEST_MAJOR_GLYPH_OF_MIND_SEAR != glyph && PRIEST_MAJOR_GLYPH_OF_HYMN_OF_HOPE != glyph && PRIEST_MAJOR_GLYPH_OF_PAIN_SUPPRESSION != glyph)
            return false;
    }
    else if (charClass == CLASS_ROGUE)
    {
        if (ROGUE_MAJOR_GLYPH_OF_ADRENALINE_RUSH != glyph && ROGUE_MAJOR_GLYPH_OF_AMBUSH != glyph && ROGUE_MAJOR_GLYPH_OF_BACKSTAB != glyph && ROGUE_MAJOR_GLYPH_OF_BLADE_FLURRY != glyph && ROGUE_MAJOR_GLYPH_OF_CRIPPLING_POISON != glyph && ROGUE_MAJOR_GLYPH_OF_DEADLY_THROW != glyph && ROGUE_MAJOR_GLYPH_OF_EVASION != glyph && ROGUE_MAJOR_GLYPH_OF_EVISCERATE != glyph && ROGUE_MAJOR_GLYPH_OF_EXPOSE_ARMOR != glyph && ROGUE_MAJOR_GLYPH_OF_FEINT != glyph && ROGUE_MAJOR_GLYPH_OF_GARROTE != glyph && ROGUE_MAJOR_GLYPH_OF_GHOSTLY_STRIKE != glyph && ROGUE_MAJOR_GLYPH_OF_GOUGE != glyph && ROGUE_MAJOR_GLYPH_OF_HEMORRHAGE != glyph && ROGUE_MAJOR_GLYPH_OF_PREPARATION != glyph && ROGUE_MAJOR_GLYPH_OF_RUPTURE != glyph && ROGUE_MAJOR_GLYPH_OF_SAP != glyph && ROGUE_MAJOR_GLYPH_OF_VIGOR != glyph && ROGUE_MAJOR_GLYPH_OF_SINISTER_STRIKE != glyph && ROGUE_MAJOR_GLYPH_OF_SLICE_AND_DICE != glyph && ROGUE_MAJOR_GLYPH_OF_SPRINT != glyph && ROGUE_MAJOR_GLYPH_OF_HUNGER_FOR_BLOOD != glyph && ROGUE_MAJOR_GLYPH_OF_KILLING_SPREE != glyph && ROGUE_MAJOR_GLYPH_OF_SHADOW_DANCE != glyph && ROGUE_MAJOR_GLYPH_OF_FAN_OF_KNIVES != glyph && ROGUE_MAJOR_GLYPH_OF_TRICKS_OF_THE_TRADE != glyph && ROGUE_MAJOR_GLYPH_OF_MUTILATE != glyph && ROGUE_MAJOR_GLYPH_OF_CLOAK_OF_SHADOWS != glyph)
            return false;
    }
    else if (charClass == CLASS_SHAMAN)
    {
        if (SHAMAN_MAJOR_GLYPH_OF_WATER_MASTERY != glyph && SHAMAN_MAJOR_GLYPH_OF_CHAIN_HEAL != glyph && SHAMAN_MAJOR_GLYPH_OF_CHAIN_LIGHTNING != glyph && SHAMAN_MAJOR_GLYPH_OF_LAVA != glyph && SHAMAN_MAJOR_GLYPH_OF_SHOCKING != glyph && SHAMAN_MAJOR_GLYPH_OF_EARTHLIVING_WEAPON != glyph && SHAMAN_MAJOR_GLYPH_OF_FIRE_ELEMENTAL_TOTEM != glyph && SHAMAN_MAJOR_GLYPH_OF_FIRE_NOVA != glyph && SHAMAN_MAJOR_GLYPH_OF_FLAME_SHOCK != glyph && SHAMAN_MAJOR_GLYPH_OF_FLAMETONGUE_WEAPON != glyph && SHAMAN_MAJOR_GLYPH_OF_FROST_SHOCK != glyph && SHAMAN_MAJOR_GLYPH_OF_HEALING_STREAM_TOTEM != glyph && SHAMAN_MAJOR_GLYPH_OF_HEALING_WAVE != glyph && SHAMAN_MAJOR_GLYPH_OF_LESSER_HEALING_WAVE != glyph && SHAMAN_MAJOR_GLYPH_OF_LIGHTNING_SHIELD != glyph && SHAMAN_MAJOR_GLYPH_OF_LIGHTNING_BOLT != glyph && SHAMAN_MAJOR_GLYPH_OF_STORMSTRIKE != glyph && SHAMAN_MAJOR_GLYPH_OF_LAVA_LASH != glyph && SHAMAN_MAJOR_GLYPH_OF_ELEMENTAL_MASTERY != glyph && SHAMAN_MAJOR_GLYPH_OF_WINDFURY_WEAPON != glyph && SHAMAN_MAJOR_GLYPH_OF_THUNDER != glyph && SHAMAN_MAJOR_GLYPH_OF_FERAL_SPIRIT != glyph && SHAMAN_MAJOR_GLYPH_OF_RIPTIDE != glyph && SHAMAN_MAJOR_GLYPH_OF_EARTH_SHIELD != glyph && SHAMAN_MAJOR_GLYPH_OF_TOTEM_OF_WRATH != glyph && SHAMAN_MAJOR_GLYPH_OF_HEX != glyph && SHAMAN_MAJOR_GLYPH_OF_STONECLAW_TOTEM != glyph)
            return false;
    }
    else if (charClass == CLASS_WARLOCK)
    {
        if (WARLOCK_MAJOR_GLYPH_OF_INCINERATE != glyph && WARLOCK_MAJOR_GLYPH_OF_CONFLAGRATE != glyph && WARLOCK_MAJOR_GLYPH_OF_CORRUPTION != glyph && WARLOCK_MAJOR_GLYPH_OF_CURSE_OF_AGONY != glyph && WARLOCK_MAJOR_GLYPH_OF_DEATH_COIL != glyph && WARLOCK_MAJOR_GLYPH_OF_FEAR != glyph && WARLOCK_MAJOR_GLYPH_OF_FELGUARD != glyph && WARLOCK_MAJOR_GLYPH_OF_FELHUNTER != glyph && WARLOCK_MAJOR_GLYPH_OF_HEALTH_FUNNEL != glyph && WARLOCK_MAJOR_GLYPH_OF_HEALTHSTONE != glyph && WARLOCK_MAJOR_GLYPH_OF_HOWL_OF_TERROR != glyph && WARLOCK_MAJOR_GLYPH_OF_IMMOLATE != glyph && WARLOCK_MAJOR_GLYPH_OF_IMP != glyph && WARLOCK_MAJOR_GLYPH_OF_SEARING_PAIN != glyph && WARLOCK_MAJOR_GLYPH_OF_SHADOW_BOLT != glyph && WARLOCK_MAJOR_GLYPH_OF_SHADOWBURN != glyph && WARLOCK_MAJOR_GLYPH_OF_SIPHON_LIFE != glyph && WARLOCK_MAJOR_GLYPH_OF_SOULSTONE != glyph && WARLOCK_MAJOR_GLYPH_OF_SUCCUBUS != glyph && WARLOCK_MAJOR_GLYPH_OF_UNSTABLE_AFFLICTION != glyph && WARLOCK_MAJOR_GLYPH_OF_VOIDWALKER != glyph && WARLOCK_MAJOR_GLYPH_OF_HAUNT != glyph && WARLOCK_MAJOR_GLYPH_OF_METAMORPHOSIS != glyph && WARLOCK_MAJOR_GLYPH_OF_CHAOS_BOLT != glyph && WARLOCK_MAJOR_GLYPH_OF_DEMONIC_CIRCLE != glyph && WARLOCK_MAJOR_GLYPH_OF_SHADOWFLAME != glyph && WARLOCK_MAJOR_GLYPH_OF_LIFE_TAP != glyph && WARLOCK_MAJOR_GLYPH_OF_SOUL_LINK != glyph && WARLOCK_MAJOR_GLYPH_OF_QUICK_DECAY != glyph)
            return false;
    }
    else if (charClass == CLASS_WARRIOR)
    {
        if (WARRIOR_MAJOR_GLYPH_OF_MORTAL_STRIKE != glyph && WARRIOR_MAJOR_GLYPH_OF_BLOODTHIRST != glyph && WARRIOR_MAJOR_GLYPH_OF_RAPID_CHARGE != glyph && WARRIOR_MAJOR_GLYPH_OF_CLEAVING != glyph && WARRIOR_MAJOR_GLYPH_OF_DEVASTATE != glyph && WARRIOR_MAJOR_GLYPH_OF_EXECUTION != glyph && WARRIOR_MAJOR_GLYPH_OF_HAMSTRING != glyph && WARRIOR_MAJOR_GLYPH_OF_HEROIC_STRIKE != glyph && WARRIOR_MAJOR_GLYPH_OF_INTERVENE != glyph && WARRIOR_MAJOR_GLYPH_OF_BARBARIC_INSULTS != glyph && WARRIOR_MAJOR_GLYPH_OF_OVERPOWER != glyph && WARRIOR_MAJOR_GLYPH_OF_RENDING != glyph && WARRIOR_MAJOR_GLYPH_OF_REVENGE != glyph && WARRIOR_MAJOR_GLYPH_OF_BLOCKING != glyph && WARRIOR_MAJOR_GLYPH_OF_LAST_STAND != glyph && WARRIOR_MAJOR_GLYPH_OF_SUNDER_ARMOR != glyph && WARRIOR_MAJOR_GLYPH_OF_SWEEPING_STRIKES != glyph && WARRIOR_MAJOR_GLYPH_OF_TAUNT != glyph && WARRIOR_MAJOR_GLYPH_OF_RESONATING_POWER != glyph && WARRIOR_MAJOR_GLYPH_OF_VICTORY_RUSH != glyph && WARRIOR_MAJOR_GLYPH_OF_WHIRLWIND != glyph && WARRIOR_MAJOR_GLYPH_OF_BLADESTORM != glyph && WARRIOR_MAJOR_GLYPH_OF_SHOCKWAVE != glyph && WARRIOR_MAJOR_GLYPH_OF_VIGILANCE != glyph && WARRIOR_MAJOR_GLYPH_OF_ENRAGED_REGENERATION != glyph && WARRIOR_MAJOR_GLYPH_OF_SPELL_REFLECTION != glyph && WARRIOR_MAJOR_GLYPH_OF_SHIELD_WALL != glyph)
            return false;
    }
    // pets don't have glyphs... yet
    else if (charClass == CLASS_PET_CUNNING || charClass == CLASS_PET_FEROCITY || charClass == CLASS_PET_TENACITY)
    {
        DEBUG_LOG("[PlayerbotAI]: Someone tried to validate a glyph for a pet... ValidateMajorGlyph: %u", (uint32) charClass);
        return false;
    }
    else // charClass unknown
    {
        DEBUG_LOG("[PlayerbotAI]: Someone was naughty and supplied an invalid class to ValidateMajorGlyph: %u", (uint32) charClass);
        return false;
    }

    return true;
}

/**
* ValidateMinorGlyph tests a glyph against class to see if it belongs to that class - only accepts Minor glyphs
*
* uint16 glyph:        glyph ID
* long charClass:    member of the Classes enum or ClassesCombatPets enum
*
* return true  -> ok
* return false -> not a valid minor glyph for that class
*/
bool PlayerbotAI::ValidateMinorGlyph(uint16 glyph, long charClass)
{
    if (charClass == CLASS_DEATH_KNIGHT)
    {
        // this looong 'if' is to see if any glyph is not a Death Knight glyph when the class clearly is
        if (DEATH_KNIGHT_MINOR_GLYPH_OF_BLOOD_TAP != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_DEATHS_EMBRACE != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_HORN_OF_WINTER != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_PESTILENCE != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_CORPSE_EXPLOSION != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_RAISE_DEAD != glyph && DEATH_KNIGHT_MINOR_GLYPH_OF_RAISE_DEAD2 != glyph)
            return false;
    }
    else if (charClass == CLASS_DRUID)
    {
        if (DRUID_MINOR_GLYPH_OF_AQUATIC_FORM != glyph && DRUID_MINOR_GLYPH_OF_CHALLENGING_ROAR != glyph && DRUID_MINOR_GLYPH_OF_THE_WILD != glyph && DRUID_MINOR_GLYPH_OF_UNBURDENED_REBIRTH != glyph && DRUID_MINOR_GLYPH_OF_THORNS != glyph && DRUID_MINOR_GLYPH_OF_DASH != glyph && DRUID_MINOR_GLYPH_OF_TYPHOON != glyph)
            return false;
    }
    else if (charClass == CLASS_HUNTER)
    {
        if (HUNTER_MINOR_GLYPH_OF_REVIVE_PET != glyph && HUNTER_MINOR_GLYPH_OF_MEND_PET != glyph && HUNTER_MINOR_GLYPH_OF_FEIGN_DEATH != glyph && HUNTER_MINOR_GLYPH_OF_SCARE_BEAST != glyph && HUNTER_MINOR_GLYPH_OF_THE_PACK != glyph && HUNTER_MINOR_GLYPH_OF_POSSESSED_STRENGTH != glyph)
            return false;
    }
    else if (charClass == CLASS_MAGE)
    {
        if (MAGE_MINOR_GLYPH_OF_ARCANE_INTELLECT != glyph && MAGE_MINOR_GLYPH_OF_BLAST_WAVE != glyph && MAGE_MINOR_GLYPH_OF_FIRE_WARD != glyph && MAGE_MINOR_GLYPH_OF_FROST_WARD != glyph && MAGE_MINOR_GLYPH_OF_FROST_ARMOR != glyph && MAGE_MINOR_GLYPH_OF_THE_PENGUIN != glyph && MAGE_MINOR_GLYPH_OF_SLOW_FALL != glyph)
            return false;
    }
    else if (charClass == CLASS_PALADIN)
    {
        if (PALADIN_MINOR_GLYPH_OF_BLESSING_OF_KINGS != glyph && PALADIN_MINOR_GLYPH_OF_BLESSING_OF_MIGHT != glyph && PALADIN_MINOR_GLYPH_OF_BLESSING_OF_WISDOM != glyph && PALADIN_MINOR_GLYPH_OF_LAY_ON_HANDS != glyph && PALADIN_MINOR_GLYPH_OF_SENSE_UNDEAD != glyph && PALADIN_MINOR_GLYPH_OF_THE_WISE != glyph)
            return false;
    }
    else if (charClass == CLASS_PRIEST)
    {
        if (PRIEST_MINOR_GLYPH_OF_FADING != glyph && PRIEST_MINOR_GLYPH_OF_LEVITATE != glyph && PRIEST_MINOR_GLYPH_OF_FORTITUDE != glyph && PRIEST_MINOR_GLYPH_OF_SHACKLE_UNDEAD != glyph && PRIEST_MINOR_GLYPH_OF_SHADOW_PROTECTION != glyph && PRIEST_MINOR_GLYPH_OF_SHADOWFIEND != glyph)
            return false;
    }
    else if (charClass == CLASS_ROGUE)
    {
        if (ROGUE_MINOR_GLYPH_OF_DISTRACT != glyph && ROGUE_MINOR_GLYPH_OF_PICK_LOCK != glyph && ROGUE_MINOR_GLYPH_OF_PICK_POCKET != glyph && ROGUE_MINOR_GLYPH_OF_SAFE_FALL != glyph && ROGUE_MINOR_GLYPH_OF_BLURRED_SPEED != glyph && ROGUE_MINOR_GLYPH_OF_VANISH != glyph)
            return false;
    }
    else if (charClass == CLASS_SHAMAN)
    {
        if (SHAMAN_MINOR_GLYPH_OF_ASTRAL_RECALL != glyph && SHAMAN_MINOR_GLYPH_OF_RENEWED_LIFE != glyph && SHAMAN_MINOR_GLYPH_OF_WATER_BREATHING != glyph && SHAMAN_MINOR_GLYPH_OF_WATER_SHIELD != glyph && SHAMAN_MINOR_GLYPH_OF_WATER_WALKING != glyph && SHAMAN_MINOR_GLYPH_OF_GHOST_WOLF != glyph && SHAMAN_MINOR_GLYPH_OF_THUNDERSTORM != glyph)
            return false;
    }
    else if (charClass == CLASS_WARLOCK)
    {
        if (WARLOCK_MINOR_GLYPH_OF_UNENDING_BREATH != glyph && WARLOCK_MINOR_GLYPH_OF_DRAIN_SOUL != glyph && WARLOCK_MINOR_GLYPH_OF_KILROGG != glyph && WARLOCK_MINOR_GLYPH_OF_ENSLAVE_DEMON != glyph && WARLOCK_MINOR_GLYPH_OF_SOULS != glyph)
            return false;
    }
    else if (charClass == CLASS_WARRIOR)
    {
        if (WARRIOR_MINOR_GLYPH_OF_BATTLE != glyph && WARRIOR_MINOR_GLYPH_OF_BLOODRAGE != glyph && WARRIOR_MINOR_GLYPH_OF_CHARGE != glyph && WARRIOR_MINOR_GLYPH_OF_MOCKING_BLOW != glyph && WARRIOR_MINOR_GLYPH_OF_THUNDER_CLAP != glyph && WARRIOR_MINOR_GLYPH_OF_ENDURING_VICTORY != glyph && WARRIOR_MINOR_GLYPH_OF_COMMAND != glyph)
            return false;
    }
    // pets don't have glyphs... yet
    else if (charClass == CLASS_PET_CUNNING || charClass == CLASS_PET_FEROCITY || charClass == CLASS_PET_TENACITY)
    {
        DEBUG_LOG("[PlayerbotAI]: Someone tried to validate a glyph for a pet... ValidateMinorGlyph: %u", (uint32) charClass);
        return false;
    }
    else // charClass unknown
    {
        DEBUG_LOG("[PlayerbotAI]: Someone was naughty and supplied an invalid class to ValidateMinorGlyph: %u", (uint32) charClass);
        return false;
    }

    return true;
}

void PlayerbotAI::extractQuestIds(const std::string& text, std::list<uint32>& questIds) const
{
    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hquest:", pos);
        if (i == -1)
            break;
        pos = i + 7;
        int endPos = text.find(':', pos);
        if (endPos == -1)
            break;
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            questIds.push_back(id);
    }
}

// Build an hlink for Quests in Yellow
void PlayerbotAI::MakeQuestLink(Quest const* quest, std::ostringstream& out)
{
    std::string questTitle = quest->GetTitle();
    QuestLocalization(questTitle, quest->GetQuestId());
    out << "|cFFEFFD00|Hquest:" << quest->GetQuestId() << ':' << quest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
}

// Build an hlink for Weapon skills in Aqua
void PlayerbotAI::MakeWeaponSkillLink(const SpellEntry* sInfo, std::ostringstream& out, uint32 skillid)
{
    int loc = GetMaster()->GetSession()->GetSessionDbcLocale();
    out << "|cff00ffff|Hspell:" << sInfo->Id << "|h[" << sInfo->SpellName[loc] << " : " << m_bot->GetSkillValue(skillid) << " /" << m_bot->GetSkillMax(skillid) << "]|h|r";
}

// Build an hlink for spells in White
void PlayerbotAI::MakeSpellLink(const SpellEntry* sInfo, std::ostringstream& out)
{
    int    loc = GetMaster()->GetSession()->GetSessionDbcLocale();
    const char* name = sInfo->SpellName[loc];

    // fallback if nothing in current locale
    if (sInfo->SpellName[loc][0] == '\0')
        name = sInfo->SpellName[0];

    out << "|cffffffff|Hspell:" << sInfo->Id << "|h[" << name << "]|h|r";
}

// Builds a hlink for an item, but since its
// only a ItemPrototype, we cant fill in everything
void PlayerbotAI::MakeItemLink(const ItemPrototype* item, std::ostringstream& out)
{
    // Color
    out << "|c";
    switch (item->Quality)
    {
        case ITEM_QUALITY_POOR:     out << "ff9d9d9d"; break;  //GREY
        case ITEM_QUALITY_NORMAL:   out << "ffffffff"; break;  //WHITE
        case ITEM_QUALITY_UNCOMMON: out << "ff1eff00"; break;  //GREEN
        case ITEM_QUALITY_RARE:     out << "ff0070dd"; break;  //BLUE
        case ITEM_QUALITY_EPIC:     out << "ffa335ee"; break;  //PURPLE
        case ITEM_QUALITY_LEGENDARY: out << "ffff8000"; break;  //ORANGE
        case ITEM_QUALITY_ARTIFACT: out << "ffe6cc80"; break;  //LIGHT YELLOW
        case ITEM_QUALITY_HEIRLOOM: out << "ffe6cc80"; break;  //LIGHT YELLOW
        default:                    out << "ffff0000"; break;  //Don't know color, so red?
    }
    out << "|Hitem:";

    // Item Id
    out << item->ItemId << ":";

    // Permanent enchantment, gems, 4 unknowns, and reporter_level
    // ->new items wont have enchantments or gems so..
    out << "0:0:0:0:0:0:0:0:0";

    // Name
    std::string name = item->Name1;
    ItemLocalization(name, item->ItemId);
    out << "|h[" << name << "]|h|r";

    // Stacked items
    if (item->BuyCount > 1)
        out << "|cff009900x" << item->BuyCount << ".|r";
    else
        out << "|cff009900.|r";
}

// Builds a hlink for an item, includes everything
// |color|Hitem:item_id:perm_ench_id:gem1:gem2:gem3:0:0:0:0:reporter_level|h[name]|h|r
void PlayerbotAI::MakeItemLink(const Item* item, std::ostringstream& out, bool IncludeQuantity /*= true*/)
{
    const ItemPrototype* proto = item->GetProto();
    // Color
    out << "|c";
    switch (proto->Quality)
    {
        case ITEM_QUALITY_POOR:     out << "ff9d9d9d"; break;  //GREY
        case ITEM_QUALITY_NORMAL:   out << "ffffffff"; break;  //WHITE
        case ITEM_QUALITY_UNCOMMON: out << "ff1eff00"; break;  //GREEN
        case ITEM_QUALITY_RARE:     out << "ff0070dd"; break;  //BLUE
        case ITEM_QUALITY_EPIC:     out << "ffa335ee"; break;  //PURPLE
        case ITEM_QUALITY_LEGENDARY: out << "ffff8000"; break;  //ORANGE
        case ITEM_QUALITY_ARTIFACT: out << "ffe6cc80"; break;  //LIGHT YELLOW
        case ITEM_QUALITY_HEIRLOOM: out << "ffe6cc80"; break;  //LIGHT YELLOW
        default:                    out << "ffff0000"; break;  //Don't know color, so red?
    }
    out << "|Hitem:";

    // Item Id
    out << proto->ItemId << ":";

    // Permanent enchantment
    out << item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << ":";

    // Gems
    uint32 g1 = 0, g2 = 0, g3 = 0;
    for (uint32 slot = SOCK_ENCHANTMENT_SLOT; slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; ++slot)
    {
        uint32 eId = item->GetEnchantmentId(EnchantmentSlot(slot));
        if (!eId) continue;

        SpellItemEnchantmentEntry const* entry = sSpellItemEnchantmentStore.LookupEntry(eId);
        if (!entry) continue;

        switch (slot - SOCK_ENCHANTMENT_SLOT)
        {
            case 1: g1 = entry->GemID; break;
            case 2: g2 = entry->GemID; break;
            case 3: g3 = entry->GemID; break;
        }
    }
    out << g1 << ":" << g2 << ":" << g3 << ":";

    // Temp enchantment, Bonus Enchantment, Prismatic Enchantment?
    // Other stuff, don't know what it is
    out << "0:0:0:0:";

    // Reporter Level
    out << "0";

    // Name
    std::string name = proto->Name1;
    ItemLocalization(name, proto->ItemId);
    out << "|h[" << name << "]|h|r";

    // Stacked items
    if (item->GetCount() > 1 && IncludeQuantity)
        out << "x" << item->GetCount() << ' ';
}

// Builds a string for an item   |color[name]|r
void PlayerbotAI::MakeItemText(const Item* item, std::ostringstream& out, bool IncludeQuantity /*= true*/)
{
    const ItemPrototype* proto = item->GetProto();
    // Color
    out << "|c";
    switch (proto->Quality)
    {
        case ITEM_QUALITY_POOR:     out << "ff9d9d9d"; break;  //GREY
        case ITEM_QUALITY_NORMAL:   out << "ffffffff"; break;  //WHITE
        case ITEM_QUALITY_UNCOMMON: out << "ff1eff00"; break;  //GREEN
        case ITEM_QUALITY_RARE:     out << "ff0070dd"; break;  //BLUE
        case ITEM_QUALITY_EPIC:     out << "ffa335ee"; break;  //PURPLE
        case ITEM_QUALITY_LEGENDARY: out << "ffff8000"; break;  //ORANGE
        case ITEM_QUALITY_ARTIFACT: out << "ffe6cc80"; break;  //LIGHT YELLOW
        case ITEM_QUALITY_HEIRLOOM: out << "ffe6cc80"; break;  //LIGHT YELLOW
        default:                    out << "ffff0000"; break;  //Don't know color, so red?
    }

    // Name
    std::string name = proto->Name1;
    ItemLocalization(name, proto->ItemId);
    out << "[" << name << "]|r";

    // Stacked items
    if (item->GetCount() > 1 && IncludeQuantity)
        out << "x" << item->GetCount() << ' ';
}

void PlayerbotAI::extractAuctionIds(const std::string& text, std::list<uint32>& auctionIds) const
{
    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Htitle:", pos);
        if (i == -1)
            break;
        pos = i + 7;
        int endPos = text.find('|', pos);
        if (endPos == -1)
            break;
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            auctionIds.push_back(id);
    }
}

void PlayerbotAI::extractSpellId(const std::string& text, uint32& spellId) const
{

    //   Link format
    //   |cffffffff|Hspell:" << spellId << ":" << "|h[" << pSpellInfo->SpellName[loc] << "]|h|r";
    //   cast |cff71d5ff|Hspell:686|h[Shadow Bolt]|h|r";
    //   012345678901234567890123456
    //        base = 16 >|  +7 >|

    uint8 pos = 0;

    int i = text.find("Hspell:", pos);
    if (i == -1)
        return;

    // DEBUG_LOG("[PlayerbotAI]: extractSpellId - first pos %u i %u",pos,i);
    pos = i + 7;     // start of window in text 16 + 7 = 23
    int endPos = text.find('|', pos);
    if (endPos == -1)
        return;

    // DEBUG_LOG("[PlayerbotAI]: extractSpellId - second endpos : %u pos : %u",endPos,pos);
    std::string idC = text.substr(pos, endPos - pos);     // 26 - 23
    spellId = atol(idC.c_str());
    pos = endPos;     // end
}

void PlayerbotAI::extractSpellIdList(const std::string& text, BotEntryList& m_spellsToLearn) const
{

    //   Link format
    //   |cffffffff|Hspell:" << spellId << ":" << "|h[" << pSpellInfo->SpellName[loc] << "]|h|r";
    //   cast |cff71d5ff|Hspell:686|h[Shadow Bolt]|h|r";
    //   012345678901234567890123456
    //        base = 16 >|  +7 >|

    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hspell:", pos);
        if (i == -1)
            break;

        // DEBUG_LOG("[PlayerbotAI]: extractSpellIdList - first pos %u i %u",pos,i);
        pos = i + 7;     // start of window in text 16 + 7 = 23
        int endPos = text.find('|', pos);
        if (endPos == -1)
            break;

        // DEBUG_LOG("[PlayerbotAI]: extractSpellIdList - second endpos : %u pos : %u",endPos,pos);
        std::string idC = text.substr(pos, endPos - pos);     // 26 - 23
        uint32 spellId = atol(idC.c_str());
        pos = endPos;     // end

        if (spellId)
            m_spellsToLearn.push_back(spellId);
    }
}

void PlayerbotAI::extractTalentIds(const std::string& text, std::list<talentPair>& talentIds) const
{
    // Link format:
    // |color|Htalent:talent_id:rank|h[name]|h|r
    // |cff4e96f7|Htalent:1396:4|h[Unleashed Fury]|h|r

    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Htalent:", pos);
        if (i == -1)
            break;
        pos = i + 8;
        // DEBUG_LOG("extractTalentIds first pos %u i %u",pos,i);
        // extract talent_id
        int endPos = text.find(':', pos);
        if (endPos == -1)
            break;
        // DEBUG_LOG("extractTalentId second endpos : %u pos : %u",endPos,pos);
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos + 1;
        // extract rank
        endPos = text.find('|', pos);
        if (endPos == -1)
            break;
        // DEBUG_LOG("extractTalentId third endpos : %u pos : %u",endPos,pos);
        std::string rankC = text.substr(pos, endPos - pos);
        uint32 rank = atol(rankC.c_str());
        pos = endPos + 1;

        // DEBUG_LOG("extractTalentId second id : %u  rank : %u",id,rank);

        if (id)
            talentIds.push_back(std::pair<uint32, uint32>(id, rank));
    }
}

void PlayerbotAI::extractGOinfo(const std::string& text, BotObjectList& m_lootTargets) const
{

    //    Link format
    //    |cFFFFFF00|Hfound:" << guid << ':'  << entry << ':'  <<  "|h[" << gInfo->name << "]|h|r";
    //    |cFFFFFF00|Hfound:9582:1731|h[Copper Vein]|h|r

    uint8 pos = 0;
    while (true)
    {
        // extract GO guid
        int i = text.find("Hfound:", pos);     // base H = 11
        if (i == -1)     // break if error
            break;

        pos = i + 7;     //start of window in text 11 + 7 = 18
        int endPos = text.find(':', pos);     // end of window in text 22
        if (endPos == -1)     //break if error
            break;
        std::string guidC = text.substr(pos, endPos - pos);     // get string within window i.e guid 22 - 18 =  4
        uint32 guid = atol(guidC.c_str());     // convert ascii to long int

        // extract GO entry
        pos = endPos + 1;
        endPos = text.find(':', pos);     // end of window in text
        if (endPos == -1)     //break if error
            break;

        std::string entryC = text.substr(pos, endPos - pos);     // get string within window i.e entry
        uint32 entry = atol(entryC.c_str());     // convert ascii to float

        ObjectGuid lootCurrent = ObjectGuid(HIGHGUID_GAMEOBJECT, entry, guid);

        if (GameObject* gob = m_bot->GetMap()->GetGameObject(lootCurrent))
            m_lootTargets.push_back(gob->GetObjectGuid());
    }
}

// extracts currency in #g#s#c format
uint32 PlayerbotAI::extractMoney(const std::string& text) const
{
    // if user specified money in ##g##s##c format
    std::string acum = "";
    uint32 copper = 0;
    for (uint8 i = 0; i < text.length(); i++)
    {
        if (text[i] == 'g')
        {
            copper += (atol(acum.c_str()) * 100 * 100);
            acum = "";
        }
        else if (text[i] == 'c')
        {
            copper += atol(acum.c_str());
            acum = "";
        }
        else if (text[i] == 's')
        {
            copper += (atol(acum.c_str()) * 100);
            acum = "";
        }
        else if (text[i] == ' ')
            break;
        else if (text[i] >= 48 && text[i] <= 57)
            acum += text[i];
        else
        {
            copper = 0;
            break;
        }
    }
    return copper;
}

// finds items in equipment and adds Item* to foundItemList
// also removes found item IDs from itemIdSearchList when found
void PlayerbotAI::findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{
    for (uint8 slot = EQUIPMENT_SLOT_START; itemIdSearchList.size() > 0 && slot < EQUIPMENT_SLOT_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }
}

// finds items in inventory and adds Item* to foundItemList
// also removes found item IDs from itemIdSearchList when found
void PlayerbotAI::findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{

    // look for items in main bag
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; itemIdSearchList.size() > 0 && slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            if (m_bot->GetTrader() && m_bot->GetTradeData()->HasItem(pItem->GetObjectGuid()))
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }

    // for all for items in other bags
    for (uint8 bag = INVENTORY_SLOT_BAG_START; itemIdSearchList.size() > 0 && bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (!pBag)
            continue;

        for (uint8 slot = 0; itemIdSearchList.size() > 0 && slot < pBag->GetBagSize(); ++slot)
        {
            Item* const pItem = m_bot->GetItemByPos(bag, slot);
            if (!pItem)
                continue;

            for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
            {
                if (pItem->GetProto()->ItemId != *it)
                    continue;

                if (m_bot->GetTrader() && m_bot->GetTradeData()->HasItem(pItem->GetObjectGuid()))
                    continue;

                foundItemList.push_back(pItem);
                itemIdSearchList.erase(it);
                break;
            }
        }
    }
}

void PlayerbotAI::findNearbyGO()
{
    if (m_collectObjects.empty())
        return;

    GameObjectList tempTargetGOList;

    for (BotEntryList::iterator itr = m_collectObjects.begin(); itr != m_collectObjects.end(); itr++)
    {
        uint32 entry = *(itr);
        GameObjectInfo const* gInfo = ObjectMgr::GetGameObjectInfo(entry);
        bool questGO = false;
        uint8 needCount = 0;

        for (uint32 i = 0; i < 6; ++i)
        {
            if (gInfo->questItems[i] != 0)  // check whether the gameobject contains quest items
            {
                questGO = true;
                if (IsInQuestItemList(gInfo->questItems[i]))    // quest item needed
                    needCount++;
            }
        }

        if (questGO && needCount == 0)
        {
            m_collectObjects.remove(entry); // remove gameobject from collect list
            return;
        }

        // search for GOs with entry, within range of m_bot
        MaNGOS::GameObjectEntryInPosRangeCheck go_check(*m_bot, entry, m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), float(m_collectDist));
        MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectEntryInPosRangeCheck> checker(tempTargetGOList, go_check);
        Cell::VisitGridObjects(m_bot, checker, float(m_collectDist));

        // no objects found, continue to next entry
        if (tempTargetGOList.empty())
            continue;

        // add any objects found to our lootTargets
        for (GameObjectList::iterator iter = tempTargetGOList.begin(); iter != tempTargetGOList.end(); iter++)
        {
            GameObject* go = (*iter);

            TerrainInfo const* map = go->GetTerrain();

            float ground_z = map->GetHeightStatic(go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
            // DEBUG_LOG("ground_z (%f) > INVALID_HEIGHT (%f)",ground_z,INVALID_HEIGHT);
            if ((ground_z > INVALID_HEIGHT) && go->IsSpawned())
                m_lootTargets.push_back(go->GetObjectGuid());
        }
    }
}

void PlayerbotAI::findNearbyCorpse()
{
    UnitList corpseList;
    float radius = float(m_mgr.m_confCollectDistance);
    MaNGOS::AnyDeadUnitCheck corpse_check(m_bot);
    MaNGOS::UnitListSearcher<MaNGOS::AnyDeadUnitCheck> reaper(corpseList, corpse_check);
    Cell::VisitAllObjects(m_bot, reaper, radius);

    //if (!corpseList.empty())
    //    TellMaster("Found %i Corpse(s)", corpseList.size());

    for (UnitList::const_iterator i = corpseList.begin(); i != corpseList.end(); ++i)
    {
        Creature* corpse = (Creature*)*i;
        if (!corpse)
            continue;

        if (!corpse->IsCorpse() || corpse->IsDespawned() || m_bot->CanAssist(corpse) || !corpse->m_loot)
            continue;

        if (!corpse->m_loot->CanLoot(m_bot) && !corpse->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            continue;

        uint32 skillId = 0;
        if (corpse->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            skillId = corpse->GetCreatureInfo()->GetRequiredLootSkill();

        if (corpse->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE) ||
                (corpse->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE) && m_bot->HasSkill(skillId)))
        {
            m_lootTargets.push_back(corpse->GetObjectGuid());
            m_lootTargets.unique();
        }
    }
}

void PlayerbotAI::findNearbyCreature()
{
    // Do not waste time finding a creature if bot has nothing to do
    // and clear list (sanity check)
    if (m_tasks.empty())
    {
        m_findNPC.clear();
        return;
    }
    CreatureList creatureList;
    float radius = INTERACTION_DISTANCE;

    CellPair pair(MaNGOS::ComputeCellPair(m_bot->GetPositionX(), m_bot->GetPositionY()));
    Cell cell(pair);

    MaNGOS::AnyUnitInObjectRangeCheck go_check(m_bot, radius);
    MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> go_search(creatureList, go_check);
    TypeContainerVisitor<MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck>, GridTypeMapContainer> go_visit(go_search);

    // Get Creatures
    cell.Visit(pair, go_visit, *(m_bot->GetMap()), *(m_bot), radius);

    // if (!creatureList.empty())
    //    TellMaster("Found %i Creatures & size of m_findNPC (%i)", creatureList.size(),m_findNPC.size());

    for (CreatureList::iterator iter = creatureList.begin(); iter != creatureList.end(); iter++)
    {
        Creature* currCreature = *iter;

        for (std::list<enum NPCFlags>::iterator itr = m_findNPC.begin(); itr != m_findNPC.end();)
        {
            uint32 npcflags = currCreature->GetUInt32Value(UNIT_NPC_FLAGS);

            if (!(*itr & npcflags))
            {
                ++itr;
                continue;
            }

            if ((*itr == UNIT_NPC_FLAG_TRAINER_CLASS) && !currCreature->CanTrainAndResetTalentsOf(m_bot))
                break;

            WorldObject* wo = m_bot->GetMap()->GetWorldObject(currCreature->GetObjectGuid());

            if (m_bot->GetDistance(wo) > CONTACT_DISTANCE + wo->GetObjectBoundingRadius())
            {
                float x, y, z;
                wo->GetContactPoint(m_bot, x, y, z, wo->GetObjectBoundingRadius());
                m_bot->GetMotionMaster()->MovePoint(wo->GetMapId(), x, y, z, FORCED_MOVEMENT_RUN, false);
                // give time to move to point before trying again
                SetIgnoreUpdateTime(1);
            }

            if (m_bot->GetDistance(wo) < INTERACTION_DISTANCE)
            {
                // DEBUG_LOG("%s is interacting with (%s)",m_bot->GetName(),currCreature->GetCreatureInfo()->Name);
                // Stop moving as soon as bot is in range
                m_bot->GetMotionMaster()->Clear(false);
                m_bot->GetMotionMaster()->MoveIdle();
                GossipMenuItemsMapBounds pMenuItemBounds = sObjectMgr.GetGossipMenuItemsMapBounds(currCreature->GetCreatureInfo()->GossipMenuId);

                // prepares quest menu when true
                bool canSeeQuests = currCreature->GetCreatureInfo()->GossipMenuId == m_bot->GetDefaultGossipMenuForSource(wo);

                // if canSeeQuests (the default, top level menu) and no menu options exist for this, use options from default options
                if (pMenuItemBounds.first == pMenuItemBounds.second && canSeeQuests)
                    pMenuItemBounds = sObjectMgr.GetGossipMenuItemsMapBounds(0);

                for (GossipMenuItemsMap::const_iterator it = pMenuItemBounds.first; it != pMenuItemBounds.second; it++)
                {
                    if (!(it->second.npc_option_npcflag & npcflags))
                        continue;

                    switch (it->second.option_id)
                    {
                        case GOSSIP_OPTION_BANKER:
                        {
                            // Manage banking actions
                            if (!m_tasks.empty())
                                for (std::list<taskPair>::iterator ait = m_tasks.begin(); ait != m_tasks.end(); ait = m_tasks.erase(ait))
                                {
                                    switch (ait->first)
                                    {
                                        // withdraw items
                                        case BANK_WITHDRAW:
                                        {
                                            // TellMaster("Withdraw items");
                                            if (!Withdraw(ait->second))
                                                DEBUG_LOG("Withdraw: Couldn't withdraw (%u)", ait->second);
                                            break;
                                        }
                                        // deposit items
                                        case BANK_DEPOSIT:
                                        {
                                            // TellMaster("Deposit items");
                                            if (!Deposit(ait->second))
                                                DEBUG_LOG("Deposit: Couldn't deposit (%u)", ait->second);
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                            BankBalance();
                            break;
                        }
                        case GOSSIP_OPTION_TAXIVENDOR:
                        case GOSSIP_OPTION_GOSSIP:
                        case GOSSIP_OPTION_INNKEEPER:
                        case GOSSIP_OPTION_TRAINER:
                        case GOSSIP_OPTION_QUESTGIVER:
                        case GOSSIP_OPTION_VENDOR:
                        case GOSSIP_OPTION_UNLEARNTALENTS:
                        case GOSSIP_OPTION_ARMORER:
                        {
                            // Manage questgiver, trainer, innkeeper & vendor actions
                            if (!m_tasks.empty())
                                for (std::list<taskPair>::iterator ait = m_tasks.begin(); ait != m_tasks.end(); ait = m_tasks.erase(ait))
                                {
                                    switch (ait->first)
                                    {
                                        // reset talents
                                        case RESET_TALENTS:
                                        {
                                            // TellMaster("Reset all talents");
                                            if (Talent(currCreature))
                                                InspectUpdate();
                                            break;
                                        }
                                        // take new quests
                                        case TAKE_QUEST:
                                        {
                                            // TellMaster("Accepting quest");
                                            if (!AddQuest(ait->second, wo))
                                                DEBUG_LOG("AddQuest: Couldn't add quest (%u)", ait->second);
                                            break;
                                        }
                                        // list npc quests
                                        case LIST_QUEST:
                                        {
                                            // TellMaster("Show available npc quests");
                                            ListQuests(wo);
                                            break;
                                        }
                                        // end quests
                                        case END_QUEST:
                                        {
                                            // TellMaster("Turn in available quests");
                                            TurnInQuests(wo);
                                            break;
                                        }
                                        // sell items
                                        case SELL_ITEMS:
                                        {
                                            // TellMaster("Selling items");
                                            Sell(ait->second);
                                            break;
                                        }
                                        // buy items
                                        case BUY_ITEMS:
                                        {
                                            // TellMaster("Buying items");
                                            Buy(currCreature, ait->second);
                                            break;
                                        }
                                        // repair items
                                        case REPAIR_ITEMS:
                                        {
                                            // TellMaster("Repairing items");
                                            Repair(ait->second, currCreature);
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                            break;
                        }
                        case GOSSIP_OPTION_AUCTIONEER:
                        {
                            // Manage auctioneer actions
                            if (!m_tasks.empty())
                                for (std::list<taskPair>::iterator ait = m_tasks.begin(); ait != m_tasks.end(); ait = m_tasks.erase(ait))
                                {
                                    switch (ait->first)
                                    {
                                        // add new auction item
                                        case ADD_AUCTION:
                                        {
                                            // TellMaster("Creating auction");
                                            AddAuction(ait->second, currCreature);
                                            break;
                                        }
                                        // cancel active auction
                                        case REMOVE_AUCTION:
                                        {
                                            // TellMaster("Cancelling auction");
                                            if (!RemoveAuction(ait->second))
                                                DEBUG_LOG("RemoveAuction: Couldn't remove auction (%u)", ait->second);
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                            ListAuctions();
                            break;
                        }
                        default:
                            break;
                    }
                    AutoUpgradeEquipment();
                    m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }
                itr = m_findNPC.erase(itr); // all done lets go home
                continue;
            }
            ++itr;
        }
    }
}

/**
* GiveLevel sets the bot's level to 'level'
* Not the clearest of function names, we're just mirroring Player.cpp's function name
*/
void PlayerbotAI::GiveLevel(uint32 /*level*/)
{
    // Talent function in Player::GetLevel take care of resetting talents in case level < getLevel()
    ApplyActiveTalentSpec();
}

/**
 * IsElite()
 * Playerbot wrapper to know if a target is elite or not. This is used by the AI to switch from one action to another
 * return bool Returns true if bot's target is a creature with elite rank (elite rare, elite, worldboss)
 *
 * params:target Unit* the target to check if it is elite
 * params:isWorldBoss bool if true, the function will return true only if the target is a worldboss. This allow to enable specific code if the target is a worldboss
 * return false if the target is not elite/rare elite/worldboss or if isWorldBoss was provided as true and that the target is not a worldboss
 *
 */
bool PlayerbotAI::IsElite(Unit* target, bool isWorldBoss) const
{
    if (!target || target->GetTypeId() == TYPEID_PLAYER)
        return false;

    if (Creature* creature = (Creature*) target)
    {
        if (isWorldBoss)
            return creature->IsWorldBoss();
        else
            return (creature->IsElite() || creature->IsWorldBoss());
    }

    return false;
}

// Check if bot target has one of the following auras: Sap, Polymorph, Shackle Undead, Banish, Seduction, Freezing Trap, Hibernate
// This is used by the AI to prevent bots from attacking crowd control targets

static const uint32 aurasIds[21] =
{
    118, 12824, 12825, 12826,   // polymorph
    28272, 28271,               // polymorph pig, turtle
    9484, 9485, 10955,          // shackle
    6358,                       // seduction
    710, 18647,                 // banish
    6770, 2070, 11297,          // sap
    3355, 14308, 14309,         // freezing trap (effect auras IDs, not spell IDs)
    2637, 18657, 18658          // hibernate
};

bool PlayerbotAI::IsNeutralized(Unit* target)
{
    if (!target)
        return false;

    for (auto aura : aurasIds)
    {
        if (target->HasAura(aura, EFFECT_INDEX_0))
            return true;
    }

    return false;
}

// Utility function to make the bots face their target
// Useful to ensure bots can cast spells/abilities
// without getting facing target errors
void PlayerbotAI::FaceTarget(Unit* target)
{
    if (!target)
        return;

    // Only update orientation if not already facing target
    if (!m_bot->HasInArc(target))
        m_bot->SetFacingTo(m_bot->GetAngle(target));
}

/**
 * IsImmuneToSchool()
 * Playerbot wrapper to know if a target is immune or not to a specific damage school. This is used by the AI to prevent using an ability the target is immuned to
 * return bool Returns true if bot's target is a creature with immunity to specified damage school
 *
 * params:target Unit* the target to check if it is immune
 * params:schoolMask the school mask to be checked against the creature template or current spell immunity
 * return false if the target is not immune, also return false by default
 *
 */
bool PlayerbotAI::IsImmuneToSchool(Unit* target, SpellSchoolMask schoolMask)
{
    if (!target || target->GetTypeId() == TYPEID_PLAYER)
        return false;

    if (Creature* creature = (Creature*) target)
            return creature->IsImmuneToDamage(schoolMask);

    return false;
}

bool PlayerbotAI::CanStore()
{
    uint32 totalused = 0;
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
            totalused++;
    }
    uint32 totalfree = 16 - totalused;
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            ItemPrototype const* pBagProto = pBag->GetProto();
            if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                totalfree =  totalfree + pBag->GetFreeSlots();
        }
    }
    return totalfree > 0;
}

// use item on self
void PlayerbotAI::UseItem(Item* item)
{
    UseItem(item, m_bot);
}

// use item on equipped item
void PlayerbotAI::UseItem(Item* item, uint8 targetInventorySlot)
{
    if (targetInventorySlot >= EQUIPMENT_SLOT_END)
        return;

    Item* const targetItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetInventorySlot);
    if (!targetItem)
        return;

    UseItem(item, TARGET_FLAG_ITEM, targetItem->GetObjectGuid());
}

// use item on unit
void PlayerbotAI::UseItem(Item* item, Unit* target)
{
    if (!target)
        return;

    UseItem(item, TARGET_FLAG_UNIT, target->GetObjectGuid());
}

// generic item use method
void PlayerbotAI::UseItem(Item* item, uint32 targetFlag, ObjectGuid targetGUID)
{
    if (!item)
        return;

    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 cast_count = 1;
    ObjectGuid item_guid = item->GetObjectGuid();
    uint32 glyphIndex = 0;
    uint8 unk_flags = 0;

    if (uint32 questid = item->GetProto()->StartQuest)
    {
        std::ostringstream report;

        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
        if (qInfo)
        {
            m_bot->GetMotionMaster()->Clear(true);
            std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_QUESTGIVER_ACCEPT_QUEST, 8 + 4 + 4));
            *packet << item_guid;
            *packet << questid;
            *packet << uint32(0);
            m_bot->GetSession()->QueuePacket(std::move(packet)); // queue the packet to get around race condition
            report << "|cffffff00Quest taken |r" << qInfo->GetTitle();
            TellMaster(report.str());
        }
        return;
    }

    uint32 spellId = 0;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (item->GetProto()->Spells[i].SpellId > 0)
        {
            spellId = item->GetProto()->Spells[i].SpellId;
            break;
        }
    }

    if (item->GetProto()->Flags & ITEM_FLAG_HAS_LOOT && spellId == 0)
    {
        // Open quest item in inventory, containing related items (e.g Gnarlpine necklace, containing Tallonkai's Jewel)
        std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_OPEN_ITEM, 2));
        *packet << item->GetBagSlot();
        *packet << item->GetSlot();
        m_bot->GetSession()->QueuePacket(std::move(packet)); // queue the packet to get around race condition
        return;
    }

    std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_USE_ITEM, 28));
    *packet << bagIndex << slot << cast_count << spellId << item_guid
            << glyphIndex << unk_flags << targetFlag;

    if (targetFlag & (TARGET_FLAG_UNIT | TARGET_FLAG_ITEM | TARGET_FLAG_GAMEOBJECT))
        *packet << targetGUID.WriteAsPacked();

    m_bot->GetSession()->QueuePacket(std::move(packet));

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
    {
        TellMaster("Can't find spell entry for spell %u on item %u", spellId, item->GetEntry());
        return;
    }

    SpellCastTimesEntry const* castingTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);
    if (!castingTimeEntry)
    {
        TellMaster("Can't find casting time entry for spell %u with index %u", spellId, spellInfo->CastingTimeIndex);
        return;
    }

    uint8 duration, castTime;
    castTime = (uint8)((float) castingTimeEntry->CastTime / 1000.0f);

    if (item->GetProto()->Class == ITEM_CLASS_CONSUMABLE && item->GetProto()->SubClass == ITEM_SUBCLASS_FOOD)
    {
        duration = (uint8)((float) GetSpellDuration(spellInfo) / 1000.0f);
        SetIgnoreUpdateTime(castTime + duration);
    }
    else
        SetIgnoreUpdateTime(castTime);
}

static const uint32 uPriorizedHealingItemIds[19] =
{
    HEALTHSTONE_DISPLAYID, FEL_REGENERATION_POTION, SUPER_HEALING_POTION, CRYSTAL_HEALING_POTION, MAJOR_DREAMLESS_SLEEP_POTION, VOLATILE_HEALING_POTION,
    MAJOR_HEALING_POTION, WHIPPER_ROOT_TUBER, NIGHT_DRAGON_BREATH, LIMITED_INVULNERABILITY_POTION, GREATER_DREAMLESS_SLEEP_POTION,
    SUPERIOR_HEALING_POTION, CRYSTAL_RESTORE, DREAMLESS_SLEEP_POTION, GREATER_HEALING_POTION, HEALING_POTION, LESSER_HEALING_POTION, DISCOLORED_HEALING_POTION, MINOR_HEALING_POTION,
};

/**
 * TryEmergency()
 * Playerbot function to select an item that the bot will use to heal itself on low health without waiting for a heal from a healer
 *
 * params:pAttacker Unit* the creature that is attacking the bot
 * return nothing
 */
void PlayerbotAI::TryEmergency(Unit* pAttacker)
{
    // Do not use consumable if bot can heal self
    if (IsHealer() && GetManaPercent() > 20)
        return;

    // If bot does not have aggro: use bandage instead of potion/stone/crystal
    if (!pAttacker && !m_bot->HasAura(11196)) // Recently bandaged
    {
        Item* bandage = FindBandage();
        if (bandage)
        {
            SetIgnoreUpdateTime(8);
            UseItem(bandage);
            return;
        }
    }

    // Else loop over the list of health consumable to pick one
    Item* healthItem;
    for (uint8 i = 0; i < countof(uPriorizedHealingItemIds); ++i)
    {
        healthItem = FindConsumable(uPriorizedHealingItemIds[i]);
        if (healthItem)
        {
            UseItem(healthItem);
            return;
        }
    }

    return;
}

// submits packet to use an item
void PlayerbotAI::EquipItem(Item* src_Item)
{
    uint8 src_bagIndex = src_Item->GetBagSlot();
    uint8 src_slot = src_Item->GetSlot();

    // DEBUG_LOG("PlayerbotAI::EquipItem: %s in srcbag = %u, srcslot = %u", src_Item->GetProto()->Name1, src_bagIndex, src_slot);

    uint16 dest;
    InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, dest, src_Item, !src_Item->IsBag());
    if (msg != EQUIP_ERR_OK)
    {
        m_bot->SendEquipError(msg, src_Item, nullptr);
        return;
    }

    uint16 src = src_Item->GetPos();
    if (dest == src)                                        // prevent equip in same slot, only at cheat
        return;

    Item* dest_Item = m_bot->GetItemByPos(dest);
    if (!dest_Item)                                          // empty slot, simple case
    {
        m_bot->RemoveItem(src_bagIndex, src_slot, true);
        m_bot->EquipItem(dest, src_Item, true);
        m_bot->AutoUnequipOffhandIfNeed();
    }
    else                                                    // have currently equipped item, not simple case
    {
        uint8 dest_bagIndex = dest_Item->GetBagSlot();
        uint8 dest_slot = dest_Item->GetSlot();

        msg = m_bot->CanUnequipItem(dest, false);
        if (msg != EQUIP_ERR_OK)
        {
            m_bot->SendEquipError(msg, dest_Item, nullptr);
            return;
        }

        // check dest->src move possibility
        ItemPosCountVec sSrc;
        if (m_bot->IsInventoryPos(src))
        {
            msg = m_bot->CanStoreItem(src_bagIndex, src_slot, sSrc, dest_Item, true);
            if (msg != EQUIP_ERR_OK)
                msg = m_bot->CanStoreItem(src_bagIndex, NULL_SLOT, sSrc, dest_Item, true);
            if (msg != EQUIP_ERR_OK)
                msg = m_bot->CanStoreItem(NULL_BAG, NULL_SLOT, sSrc, dest_Item, true);
        }

        if (msg != EQUIP_ERR_OK)
        {
            m_bot->SendEquipError(msg, dest_Item, src_Item);
            return;
        }

        // now do moves, remove...
        m_bot->RemoveItem(dest_bagIndex, dest_slot, false);
        m_bot->RemoveItem(src_bagIndex, src_slot, false);

        // add to dest
        m_bot->EquipItem(dest, src_Item, true);

        // add to src
        if (m_bot->IsInventoryPos(src))
            m_bot->StoreItem(sSrc, dest_Item, true);

        m_bot->AutoUnequipOffhandIfNeed();
    }
}

// submits packet to trade an item (trade window must already be open)
// default slot is -1 which means trade slots 0 to 5. if slot is set
// to TRADE_SLOT_NONTRADED (which is slot 6) item will be shown in the
// 'Will not be traded' slot.
bool PlayerbotAI::TradeItem(const Item& item, int8 slot)
{
    // DEBUG_LOG ("[PlayerbotAI]: TradeItem - slot=%d, hasTrader=%d, itemInTrade=%d, itemTradeable=%d",
    //    slot,
    //    (m_bot->GetTrader() ? 1 : 0),
    //    (item.IsInTrade() ? 1 : 0),
    //    (item.CanBeTraded() ? 1 : 0)
    //    );

    if (!m_bot->GetTrader() || item.IsInTrade() || (!item.CanBeTraded() && slot != TRADE_SLOT_NONTRADED))
        return false;

    int8 tradeSlot = -1;

    TradeData* pTrade = m_bot->GetTradeData();
    if ((slot >= 0 && slot < TRADE_SLOT_COUNT) && pTrade->GetItem(TradeSlots(slot)) == nullptr)
        tradeSlot = slot;
    else
        for (uint8 i = 0; i < TRADE_SLOT_TRADED_COUNT && tradeSlot == -1; i++)
        {
            if (pTrade->GetItem(TradeSlots(i)) == nullptr)
            {
                tradeSlot = i;
                // reserve trade slot to allow multiple items to be traded
                pTrade->SetItem(TradeSlots(i), const_cast<Item*>(&item));
            }
        }

    if (tradeSlot == -1) return false;

    std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_SET_TRADE_ITEM, 3));
    *packet << (uint8) tradeSlot << (uint8) item.GetBagSlot()
            << (uint8) item.GetSlot();
    m_bot->GetSession()->QueuePacket(std::move(packet));
    return true;
}

// submits packet to trade copper (trade window must be open)
bool PlayerbotAI::TradeCopper(uint32 copper)
{
    if (copper > 0)
    {
        std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_SET_TRADE_GOLD, 4));
        *packet << copper;
        m_bot->GetSession()->QueuePacket(std::move(packet));
        return true;
    }
    return false;
}

bool PlayerbotAI::DoTeleport(WorldObject& /*obj*/)
{
    if (m_targetCombat)
    {
        m_attackerInfo.clear();
        m_targetCombat = nullptr;
    }

    SetIgnoreUpdateTime(6);
    PlayerbotChatHandler ch(GetMaster());
    if (!ch.teleport(*m_bot))
    {
        ch.sysmessage(".. could not be teleported ..");
        // DEBUG_LOG ("[PlayerbotAI]: DoTeleport - %s failed to teleport", m_bot->GetName() );
        return false;
    }
    return true;
}

// Localization support
void PlayerbotAI::ItemLocalization(std::string& itemName, const uint32 itemID) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    ItemLocale const* pItemInfo = sObjectMgr.GetItemLocale(itemID);
    if (pItemInfo)
        if (pItemInfo->Name.size() > loc && !pItemInfo->Name[loc].empty())
        {
            const std::string name = pItemInfo->Name[loc];
            if (Utf8FitTo(name, wnamepart))
                itemName = name.c_str();
        }
}

void PlayerbotAI::QuestLocalization(std::string& questTitle, const uint32 questID) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    QuestLocale const* pQuestInfo = sObjectMgr.GetQuestLocale(questID);
    if (pQuestInfo)
        if (pQuestInfo->Title.size() > loc && !pQuestInfo->Title[loc].empty())
        {
            const std::string title = pQuestInfo->Title[loc];
            if (Utf8FitTo(title, wnamepart))
                questTitle = title.c_str();
        }
}

void PlayerbotAI::CreatureLocalization(std::string& creatureName, const uint32 entry) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    CreatureLocale const* pCreatureInfo = sObjectMgr.GetCreatureLocale(entry);
    if (pCreatureInfo)
        if (pCreatureInfo->Name.size() > loc && !pCreatureInfo->Name[loc].empty())
        {
            const std::string title = pCreatureInfo->Name[loc];
            if (Utf8FitTo(title, wnamepart))
                creatureName = title.c_str();
        }
}

void PlayerbotAI::GameObjectLocalization(std::string& gameobjectName, const uint32 entry) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    GameObjectLocale const* pGameObjectInfo = sObjectMgr.GetGameObjectLocale(entry);
    if (pGameObjectInfo)
        if (pGameObjectInfo->Name.size() > loc && !pGameObjectInfo->Name[loc].empty())
        {
            const std::string title = pGameObjectInfo->Name[loc];
            if (Utf8FitTo(title, wnamepart))
                gameobjectName = title.c_str();
        }
}

// Helper function for automatically selling poor quality items to the vendor
void PlayerbotAI::_doSellItem(Item* const item, std::ostringstream& report, std::ostringstream& canSell, uint32& TotalCost, uint32& TotalSold)
{
    if (!item)
        return;

    uint8 autosell = 0;
    ItemPrototype const* pProto = item->GetProto();

    std::ostringstream mout;
    if (item->CanBeTraded() && pProto->Quality == ITEM_QUALITY_POOR) // trash sells automatically.
        autosell = 1;

    if (SellWhite == 1) // set this with the command 'sell all'
    {
        // here we'll do some checks for other items that are safe to automatically sell such as
        // white items that are a number of levels lower than anything we could possibly use.
        // We'll check to make sure its not a tradeskill tool, quest item etc, things that we don't want to lose.
        if (pProto->SellPrice > 0 && (pProto->Quality == ITEM_QUALITY_NORMAL || pProto->Quality == ITEM_QUALITY_UNCOMMON) && pProto->SubClass != ITEM_SUBCLASS_QUEST)
        {
            if (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff) && pProto->SubClass != ITEM_SUBCLASS_WEAPON_MISC && pProto->FoodType == 0)
            {
                if (pProto->Class == ITEM_CLASS_WEAPON)
                    autosell = 1;
                if (pProto->Class == ITEM_CLASS_ARMOR)
                    autosell = 1;
            }
            if (pProto->SubClass == ITEM_SUBCLASS_FOOD && (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff)))
            {
                autosell = 1;
            }
        }
    }

    if (autosell == 1) // set this switch above and this item gets sold automatically. Only set this for automatic sales e.g junk etc.
    {


        uint32 cost = pProto->SellPrice * item->GetCount();

        // handle spell charge if any
        for (auto i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            auto const& spell = pProto->Spells[i];

            // if spell charges for this item are negative, it means that the item should be destroyed once the charges are consumed.
            // it also means that the value of this item is relative to how many charges are remaining.
            if (spell.SpellId != 0 && spell.SpellCharges < 0)
            {
                auto const multiplier = static_cast<float>(item->GetSpellCharges(i)) / spell.SpellCharges;
                cost *= multiplier;
                break;
            }
        }

        m_bot->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);
        m_bot->AddItemToBuyBackSlot(item, cost);
        m_bot->ModifyMoney(cost);

        ++TotalSold;
        TotalCost += cost;

        report << "Sold ";
        MakeItemLink(item, report, true);
        report << " for " << Cash(cost);
    }
    else if (pProto->SellPrice > 0)
        MakeItemLink(item, canSell, true);
}

bool PlayerbotAI::Withdraw(const uint32 itemid)
{
    Item* pItem = FindItemInBank(itemid);
    if (pItem)
    {
        std::ostringstream report;

        ItemPosCountVec dest;
        InventoryResult msg = m_bot->CanStoreItem(NULL_BAG, NULL_SLOT, dest, pItem, false);
        if (msg != EQUIP_ERR_OK)
        {
            m_bot->SendEquipError(msg, pItem, nullptr);
            return false;
        }

        m_bot->RemoveItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->StoreItem(dest, pItem, true);

        report << "Withdrawn ";
        MakeItemLink(pItem, report, true);

        TellMaster(report.str());
    }
    return true;
}

bool PlayerbotAI::Deposit(const uint32 itemid)
{
    Item* pItem = FindItem(itemid);
    if (pItem)
    {
        std::ostringstream report;

        ItemPosCountVec dest;
        InventoryResult msg = m_bot->CanBankItem(NULL_BAG, NULL_SLOT, dest, pItem, false);
        if (msg != EQUIP_ERR_OK)
        {
            m_bot->SendEquipError(msg, pItem, nullptr);
            return false;
        }

        m_bot->RemoveItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->BankItem(dest, pItem, true);

        report << "Deposited ";
        MakeItemLink(pItem, report, true);

        TellMaster(report.str());
    }
    return true;
}

void PlayerbotAI::BankBalance()
{
    std::ostringstream report;

    report << "In my bank\n ";
    report << "My item slots: ";

    for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_END; ++slot)
    {
        Item* const item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (item)
            MakeItemLink(item, report, true);
    }
    TellMaster(report.str());

    // and each of my bank bags
    for (uint8 bag = BANK_SLOT_BAG_START; bag < BANK_SLOT_BAG_END; ++bag)
    {
        std::ostringstream goods;
        const Bag* const pBag = static_cast<Bag*>(m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
        if (pBag)
        {
            goods << "\nMy ";
            const ItemPrototype* const pBagProto = pBag->GetProto();
            std::string bagName = pBagProto->Name1;
            ItemLocalization(bagName, pBagProto->ItemId);
            goods << bagName << " slot: ";

            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const item = m_bot->GetItemByPos(bag, slot);
                if (item)
                    MakeItemLink(item, goods, true);
            }
            TellMaster(goods.str());
        }
    }
}

bool PlayerbotAI::Talent(Creature* trainer)
{
    if (!(m_bot->resetTalents()))
    {
        std::unique_ptr<WorldPacket> packet(new WorldPacket(MSG_TALENT_WIPE_CONFIRM, 8 + 4));    //you do not have any talent
        *packet << uint64(0);
        *packet << uint32(0);
        m_bot->GetSession()->QueuePacket(std::move(packet));
        return false;
    }

    m_bot->SendTalentsInfoData(false);
    trainer->CastSpell(m_bot, 14867, TRIGGERED_OLD_TRIGGERED);                  //spell: "Untalent Visual Effect"
    return true;
}

void PlayerbotAI::InspectUpdate()
{
    WorldPacket packet(SMSG_INSPECT_RESULTS, 50);
    packet << m_bot->GetPackGUID();
    m_bot->BuildPlayerTalentsInfoData(packet);
    m_bot->BuildEnchantmentsInfoData(packet);
    GetMaster()->GetSession()->SendPacket(packet);
}

void PlayerbotAI::Repair(const uint32 itemid, Creature* rCreature)
{
    uint32 cost = 0;
    uint8 UseGuild = (m_bot->GetGuildId() != 0) ? uint8(1) : uint8(0);
    Item* rItem = FindItem(itemid, true); // if item equipped or in bags
    if (rItem)
        cost = EstRepair(rItem->GetPos());
    else
        cost = EstRepairAll();

    Guild* pGuild = sGuildMgr.GetGuildById(m_bot->GetGuildId());

    if (pGuild)
    {
        // Check whether bot can use the guildbank first
        if (!pGuild->HasRankRight(m_bot->GetRank(), GR_RIGHT_WITHDRAW_REPAIR))
        {
            DEBUG_LOG("You do not have rights to withdraw for repairs");
            UseGuild = 0;
        }

        if (pGuild->GetGuildBankMoney() < cost)
        {
            DEBUG_LOG("There is not enough money in the guild bank");
            UseGuild = 0;
        }
    }

    // If guildbank unavailable, check pockets
    if (UseGuild == 0)
        if (m_bot->GetMoney() < cost)
        {
            TellMaster("I do not have enough money to repair");
            return;
        }

    ObjectGuid itemGuid = (rItem) ? rItem->GetObjectGuid() : ObjectGuid();

    std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_REPAIR_ITEM, 8 + 8 + 1));
    *packet << rCreature->GetObjectGuid();  // repair npc guid
    *packet << itemGuid; // if item specified then repair this, else repair all
    *packet << UseGuild;  // guildbank yes=1 no=0
    m_bot->GetSession()->QueuePacket(std::move(packet));  // queue the packet to get around race condition
}

bool PlayerbotAI::RemoveAuction(const uint32 auctionid)
{
    QueryResult* result = CharacterDatabase.PQuery(
                              "SELECT houseid,itemguid,item_template,itemowner,buyoutprice,time,buyguid,lastbid,startbid,deposit FROM auction WHERE id = '%u'", auctionid);

    AuctionEntry* auction;

    if (result)
    {
        Field* fields = result->Fetch();

        auction = new AuctionEntry;
        auction->Id = auctionid;
        uint32 houseid  = fields[0].GetUInt32();
        auction->itemGuidLow = fields[1].GetUInt32();
        auction->itemTemplate = fields[2].GetUInt32();
        auction->owner = fields[3].GetUInt32();
        auction->buyout = fields[4].GetUInt32();
        auction->expireTime = fields[5].GetUInt32();
        auction->bidder = fields[6].GetUInt32();
        auction->bid = fields[7].GetUInt32();
        auction->startbid = fields[8].GetUInt32();
        auction->deposit = fields[9].GetUInt32();
        auction->auctionHouseEntry = nullptr;                  // init later

        // check if sold item exists for guid
        // and item_template in fact (GetAItem will fail if problematic in result check in AuctionHouseMgr::LoadAuctionItems)
        Item* pItem = sAuctionMgr.GetAItem(auction->itemGuidLow);
        if (!pItem)
        {
            auction->DeleteFromDB();
            sLog.outError("Auction %u has not a existing item : %u, deleted", auction->Id, auction->itemGuidLow);
            delete auction;
            delete result;
            return false;
        }

        auction->auctionHouseEntry = sAuctionHouseStore.LookupEntry(houseid);

        // Attempt send item back to owner
        std::ostringstream msgAuctionCanceledOwner;
        msgAuctionCanceledOwner << auction->itemTemplate << ":0:" << AUCTION_CANCELED << ":0:0";

        // item will deleted or added to received mail list
        MailDraft(msgAuctionCanceledOwner.str(), "")    // TODO: fix body
        .AddItem(pItem)
        .SendMailTo(MailReceiver(ObjectGuid(HIGHGUID_PLAYER, auction->owner)), auction, MAIL_CHECK_MASK_COPIED);

        if (sAuctionMgr.RemoveAItem(auction->itemGuidLow))
            m_bot->GetSession()->SendAuctionCommandResult(auction, AUCTION_REMOVED, AUCTION_OK);

        auction->DeleteFromDB();

        delete auction;
        delete result;
    }
    return true;
}

// Subject - 9360:0:2
// Subject - item:0:MailAuctionAnswer
// Body - 0:2650:2650:120:132
// Body - 0:High Bid:Buyout:Deposit:AuctionHouse Cut

std::string PlayerbotAI::AuctionResult(std::string subject, std::string body)
{
    std::ostringstream out;
    std::string winner;
    int pos;

    subject.append(":");
    if (body.size() > 0)
    {
        pos = body.find_first_not_of(" ");
        subject.append(body, pos, body.size() - pos);
        subject.append(":");
    }

    // DEBUG_LOG("Auctions string (%s)", subject.c_str());
    pos = 0;
    int sublen = subject.size() / 2;
    uint32 a_info[15];
    for (int i = 0; i < sublen; i++)
    {
        int endpos = subject.find(':', pos);
        std::string idc = subject.substr(pos, endpos - pos);
        a_info[i] = atol(idc.c_str());
        // DEBUG_LOG("a_info[%d] = (%u)", i, a_info[i]);
        pos = endpos + 1;
    }

    if (a_info[4] != a_info[5])
        winner =  "High Bidder";
    else
        winner =  "Buyout";

    ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(a_info[0]);
    if (!pProto)
        return out.str();

    switch (a_info[2])
    {
        case AUCTION_OUTBIDDED:           //= 0,
            out << "Subject: Outbid on: " << pProto->Name1;
            break;
        case AUCTION_WON:                 //= 1,
            out << "Subject: Auction won: " << pProto->Name1 << "\n";
            out << "Item Purchased: " << pProto->Name1 << "\n";
            break;
        case AUCTION_SUCCESSFUL:          //= 2,
        {
            out << "Subject: Auction successful: " << pProto->Name1 << "\n";
            out << "Item Sold: " << pProto->Name1 << "\n";
            out << "\n[" << winner << " Sale: " << Cash(a_info[4]) << "]";
            out << "\n( |cff1eff00Deposit:|cffccffff " << Cash(a_info[6]) << " |cffff0000- Tax:|cffccffff " << Cash(a_info[7]) << " ) |cff1eff00+|cffccffff";
            break;
        }
        case AUCTION_EXPIRED:             //= 3,
            out << "Subject: Auction expired: " << pProto->Name1;
            break;
        case AUCTION_CANCELLED_TO_BIDDER: //= 4,
            out << "Subject: Auction cancelled to bidder: " << pProto->Name1;
            break;
        case AUCTION_CANCELED:            //= 5,
            out << "Subject: Auction cancelled: " << pProto->Name1;
            break;
        case AUCTION_SALE_PENDING:        //= 6
            out << "Subject: Auction sale pending: " << pProto->Name1;
            break;
    }
    return out.str();
}

std::string PlayerbotAI::Cash(uint32 copper)
{
    using namespace std;
    std::ostringstream change;

    uint32 gold = uint32(copper / 10000);
    copper -= (gold * 10000);
    uint32 silver = uint32(copper / 100);
    copper -= (silver * 100);

    if (gold > 0)
        change << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
    if (silver > 0)
        change << std::setfill(' ') << std::setw(2) << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
    change << std::setfill(' ') << std::setw(2) << copper << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";

    return change.str();
}

void PlayerbotAI::ListQuests(WorldObject* questgiver)
{
    if (!questgiver)
        return;

    // list all bot quests this NPC has
    m_bot->PrepareQuestMenu(questgiver->GetObjectGuid());
    QuestMenu& questMenu = m_bot->GetPlayerMenu()->GetQuestMenu();
    std::ostringstream out;
    for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
    {
        QuestMenuItem const& qItem = questMenu.GetItem(iI);
        uint32 questID = qItem.m_qId;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

        if (m_bot->SatisfyQuestStatus(pQuest, false))
        {
            if (gQuestFetch != 1)
            {
                MakeQuestLink(pQuest, out);
            }
            else
            {
                if (!AddQuest(questID, questgiver))
                    continue;
            }
        }
    }
    if (!out.str().empty())
        TellMaster(out.str());
}

bool PlayerbotAI::AddQuest(const uint32 entry, WorldObject* questgiver)
{
    std::ostringstream out;

    Quest const* qInfo = sObjectMgr.GetQuestTemplate(entry);
    if (!qInfo)
    {
        ChatHandler(GetMaster()).PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        return false;
    }

    if (m_bot->GetQuestStatus(entry) == QUEST_STATUS_COMPLETE)
    {
        TellMaster("I already completed that quest.");
        return false;
    }
    else if (!m_bot->CanTakeQuest(qInfo, false))
    {
        if (!m_bot->SatisfyQuestStatus(qInfo, false))
            TellMaster("I already have that quest.");
        else
            TellMaster("I can't take that quest.");
        return false;
    }
    else if (!m_bot->SatisfyQuestLog(false))
    {
        TellMaster("My quest log is full.");
        return false;
    }
    else if (m_bot->CanAddQuest(qInfo, false))
    {
        m_bot->AddQuest(qInfo, questgiver);

        out << "|cffffff00Quest taken ";
        MakeQuestLink(qInfo, out);

        if (m_bot->CanCompleteQuest(entry))
            m_bot->CompleteQuest(entry);

        // build needed items if quest contains any
        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; i++)
            if (qInfo->ReqItemCount[i] > 0)
            {
                SetQuestNeedItems();
                break;
            }

        // build needed creatures if quest contains any
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
            if (qInfo->ReqCreatureOrGOCount[i] > 0)
            {
                SetQuestNeedCreatures();
                break;
            }

        TellMaster(out.str());
        return true;
    }
    return false;
}

void PlayerbotAI::ListAuctions()
{
    std::ostringstream report;

    QueryResult* result = CharacterDatabase.PQuery(
                              "SELECT id,itemguid,item_template,time,buyguid,lastbid FROM auction WHERE itemowner = '%u'", m_bot->GetObjectGuid().GetCounter());
    if (result)
    {
        report << "My active auctions are: \n";
        do
        {
            Field* fields = result->Fetch();

            uint32 Id = fields[0].GetUInt32();
            uint32 itemGuidLow = fields[1].GetUInt32();
            uint32 itemTemplate = fields[2].GetUInt32();
            time_t expireTime = fields[3].GetUInt32();
            uint32 bidder = fields[4].GetUInt32();
            uint32 bid = fields[5].GetUInt32();

            time_t remtime = expireTime - CurrentTime();

            tm* aTm = gmtime(&remtime);

            if (expireTime > CurrentTime())
            {
                Item* aItem = sAuctionMgr.GetAItem(itemGuidLow);
                if (aItem)
                {
                    // Name
                    uint32 count = aItem->GetCount();
                    std::string name = aItem->GetProto()->Name1;
                    ItemLocalization(name, itemTemplate);
                    report << "\n|cffffffff|Htitle:" << Id << "|h[" << name;
                    if (count > 1)
                        report << "|cff00ff00x" << count << "|cffffffff" << "]|h|r";
                    else
                        report << "]|h|r";
                }

                if (bidder)
                {
                    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, bidder);
                    std::string bidder_name;
                    if (sObjectMgr.GetPlayerNameByGUID(guid, bidder_name))
                        report << " " << bidder_name << ": ";

                    report << Cash(bid);
                }
                if (aItem)
                    report << " ends: " << aTm->tm_hour << "|cff0070dd|hH|h|r " << aTm->tm_min << "|cff0070dd|hmin|h|r";
            }
        }
        while (result->NextRow());

        delete result;
        TellMaster(report.str().c_str());
    }
}

void PlayerbotAI::AddAuction(const uint32 itemid, Creature* aCreature)
{
    Item* aItem = FindItem(itemid);
    if (aItem)
    {
        std::ostringstream out;
        srand(CurrentTime());
        uint32 duration[3] = { 720, 1440, 2880 };  // 720 = 12hrs, 1440 = 24hrs, 2880 = 48hrs
        uint32 etime = duration[rand() % 3];

        uint32 min = urand(aItem->GetProto()->SellPrice * aItem->GetCount(), aItem->GetProto()->BuyPrice * aItem->GetCount()) * (aItem->GetProto()->Quality + 1);
        uint32 max = urand(aItem->GetProto()->SellPrice * aItem->GetCount(), aItem->GetProto()->BuyPrice * aItem->GetCount()) * (aItem->GetProto()->Quality + 1);

        out << "Auctioning ";
        MakeItemLink(aItem, out, true);
        out << " with " << aCreature->GetCreatureInfo()->Name;
        TellMaster(out.str().c_str());

        std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_AUCTION_SELL_ITEM, 8 + 4 + 8 + 4 + 4 + 4 + 4));
        *packet << aCreature->GetObjectGuid();     // auctioneer guid
        *packet << uint32(1);                      // const 1
        *packet << aItem->GetObjectGuid();         // item guid
        *packet << aItem->GetCount();      // stacksize
        *packet << uint32((min < max) ? min : max);  // starting bid
        *packet << uint32((max > min) ? max : min);  // buyout
        *packet << uint32(etime);  // auction duration

        m_bot->GetSession()->QueuePacket(std::move(packet));  // queue the packet to get around race condition
    }
}

void PlayerbotAI::Buy(Creature* vendor, const uint32 itemid)
{
    if (!vendor)
        return;

    // DEBUG_LOG("vendor (%s) itemid (%u)",vendor->GetName(),itemid);

    VendorItemData const* vItems = vendor->GetVendorItems();
    VendorItemData const* tItems = vendor->GetVendorTemplateItems();

    uint8 customitems = vItems ? vItems->GetItemCount() : 0;
    uint8 numitems = customitems + (tItems ? tItems->GetItemCount() : 0);

    for (uint8 vendorslot = 0; vendorslot < numitems; ++vendorslot)
    {
        VendorItem const* crItem = vendorslot < customitems ? vItems->GetItem(vendorslot) : tItems->GetItem(vendorslot - customitems);

        if (crItem)
        {
            if (itemid != crItem->item)
                continue;

            ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);
            if (pProto)
            {
                // class wrong item skip only for bindable case
                if ((pProto->AllowableClass & m_bot->getClassMask()) == 0 && pProto->Bonding == BIND_WHEN_PICKED_UP)
                    continue;

                // race wrong item skip always
                if ((pProto->Flags2 & ITEM_FLAG2_FACTION_HORDE) && m_bot->GetTeam() != HORDE)
                    continue;

                if ((pProto->Flags2 & ITEM_FLAG2_FACTION_ALLIANCE) && m_bot->GetTeam() != ALLIANCE)
                    continue;

                if ((pProto->AllowableRace & m_bot->getRaceMask()) == 0)
                    continue;

                // possible item coverting for BoA case
                if (pProto->Flags & ITEM_FLAG_IS_BOUND_TO_ACCOUNT)
                    // convert if can use and then buy
                    if (pProto->RequiredReputationFaction && uint32(m_bot->GetReputationRank(pProto->RequiredReputationFaction)) >= pProto->RequiredReputationRank)
                        // checked at convert data loading as existed
                        if (uint32 newItemId = sObjectMgr.GetItemConvert(itemid, m_bot->getRaceMask()))
                            pProto = ObjectMgr::GetItemPrototype(newItemId);
                m_bot->BuyItemFromVendorSlot(vendor->GetObjectGuid(), vendorslot, itemid, 1, NULL_BAG, NULL_SLOT);
                return;
            }
        }
    }
}

void PlayerbotAI::Sell(const uint32 itemid)
{
    Item* pItem = FindItem(itemid);
    if (pItem)
    {
        std::ostringstream report;

        ItemPrototype const* pProto = pItem->GetProto();

        if (!pProto)
            return;

        uint32 cost = pItem->GetProto()->SellPrice * pItem->GetCount();

        // handle spell charge if any
        for (auto i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            auto const& spell = pProto->Spells[i];

            // if spell charges for this item are negative, it means that the item should be destroyed once the charges are consumed.
            // it also means that the value of this item is relative to how many charges are remaining.
            if (spell.SpellId != 0 && spell.SpellCharges < 0)
            {
                auto const multiplier = static_cast<float>(pItem->GetSpellCharges(i)) / spell.SpellCharges;
                cost *= multiplier;
                break;
            }
        }

        m_bot->MoveItemFromInventory(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->AddItemToBuyBackSlot(pItem, cost);
        m_bot->ModifyMoney(cost);

        report << "Sold ";
        MakeItemLink(pItem, report, true);
        report << " for " << Cash(cost);

        TellMaster(report.str());
    }
}

void PlayerbotAI::SellGarbage(Player& /*player*/, bool bListNonTrash, bool bDetailTrashSold, bool bVerbose)
{
    uint32 SoldCost = 0;
    uint32 SoldQuantity = 0;
    bool sellableItems = false;
    std::ostringstream report, goods;

    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (item)
            _doSellItem(item, report, goods, SoldCost, SoldQuantity);
    }
    if (goods.str().size() > 0)
        sellableItems = true;
    if (bVerbose && bListNonTrash && goods.str().size() > 0)    // Tell master of unsold items
        TellMaster("Unsold items in my main backpack: %s", goods.str().c_str());

    // and each of our other packs
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag) // check for extra bags
    {
        std::ostringstream subBagGoods;
        const Bag* const pBag = static_cast<Bag*>(m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const item = m_bot->GetItemByPos(bag, slot);
                if (item)
                    _doSellItem(item, report, subBagGoods, SoldCost, SoldQuantity);
            }
            if (subBagGoods.str().size() > 0)
                sellableItems = true;
            if (bVerbose && bListNonTrash && subBagGoods.str().size() > 0)  // Tell master of unsold items
            {
                const ItemPrototype* const bagProto = pBag->GetProto();     // Get bag name to help master retrieve it
                std::string bagName = bagProto->Name1;
                ItemLocalization(bagName, bagProto->ItemId);
                TellMaster("Unsold items in my %s: %s", bagName.c_str(), subBagGoods.str().c_str());
            }
        }
    }

    if (!bDetailTrashSold) // no trash got sold
        report.str(""); // clear ostringstream

    if (SoldCost > 0)
    {
        if (bDetailTrashSold)
            report << "Sold total " << SoldQuantity << " item(s) for ";
        else
            report << "Sold " << SoldQuantity << " trash item(s) for ";
        report << Cash(SoldCost);

        if (bVerbose)
            TellMaster(report.str());
    }

    // For all bags, non-gray sellable items
    if (bVerbose)
    {
        if (SoldQuantity == 0 && !sellableItems)
            TellMaster("No items to sell, trash or otherwise.");
    }
}

// no time to sell junk, then just drop it...
bool PlayerbotAI::DropGarbage(bool bVerbose)
{
    std::ostringstream report;
    bool autodrop = false, junk = false;

    if (bVerbose)
        report << "Dropping ";

    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        autodrop = false;
        Item* const item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (item)
        {
            ItemPrototype const* pProto = item->GetProto();
            if (item->CanBeTraded() && item->GetProto()->Quality == ITEM_QUALITY_POOR) // trash sells automatically.
            {
                junk = true;
                report << DropItem(pProto->ItemId);
            }
            if (m_dropWhite && item->GetProto()->SellPrice > 0 && (item->GetProto()->Quality == ITEM_QUALITY_NORMAL || item->GetProto()->Quality == ITEM_QUALITY_UNCOMMON) && item->GetProto()->SubClass != ITEM_SUBCLASS_QUEST)
            {
                if (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff) && pProto->SubClass != ITEM_SUBCLASS_WEAPON_MISC && pProto->FoodType == 0)
                {
                    if (pProto->Class == ITEM_CLASS_WEAPON)
                        autodrop = true;
                    if (pProto->Class == ITEM_CLASS_ARMOR)
                        autodrop = true;
                }
                if (pProto->SubClass == ITEM_SUBCLASS_FOOD && (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff)))
                {
                    autodrop = true;
                }
                if (autodrop)
                {
                    junk = true;
                    report << DropItem(pProto->ItemId);
                }
            }
        }
    }

    // and each of our other packs
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag) // check for extra bags
    {
        const Bag* const pBag = static_cast<Bag*>(m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                autodrop = false;
                Item* const item = m_bot->GetItemByPos(bag, slot);
                if (item)
                {
                    ItemPrototype const* pProto = item->GetProto();
                    if (item->CanBeTraded() && item->GetProto()->Quality == ITEM_QUALITY_POOR) // trash sells automatically.
                    {
                        junk = true;
                        report << DropItem(pProto->ItemId);
                    }
                    if (m_dropWhite && item->GetProto()->SellPrice > 0 && (item->GetProto()->Quality == ITEM_QUALITY_NORMAL || item->GetProto()->Quality == ITEM_QUALITY_UNCOMMON) && item->GetProto()->SubClass != ITEM_SUBCLASS_QUEST)
                    {
                        if (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff) && pProto->SubClass != ITEM_SUBCLASS_WEAPON_MISC && pProto->FoodType == 0)
                        {
                            if (pProto->Class == ITEM_CLASS_WEAPON)
                                autodrop = true;
                            if (pProto->Class == ITEM_CLASS_ARMOR)
                                autodrop = true;
                        }
                        if (pProto->SubClass == ITEM_SUBCLASS_FOOD && (pProto->RequiredLevel < (m_bot->GetLevel() - m_mgr.gConfigSellLevelDiff)))
                        {
                            autodrop = true;
                        }
                        if (autodrop)
                        {
                            junk = true;
                            report << DropItem(pProto->ItemId);
                        }
                    }
                }
            }
        }
    }
    if (bVerbose)
        TellMaster(report.str());
    return junk;
}

std::string PlayerbotAI::DropItem(const uint32 itemid)
{
    Item* pItem = FindItem(itemid);
    if (pItem)
    {
        std::ostringstream report;

        // Yea, that's right, get the item info BEFORE you destroy it :)
        MakeItemText(pItem, report, true);

        m_bot->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);

        return report.str();
    }

    return "";
}

void PlayerbotAI::GetTaxi(ObjectGuid guid, BotTaxiNode& nodes)
{
    // DEBUG_LOG("[PlayerbotAI]: GetTaxi - %s node[0] %d node[1] %d", m_bot->GetName(), nodes[0], nodes[1]);

    Creature* unit = m_bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!unit)
    {
        DEBUG_LOG("[PlayerbotAI]: GetTaxi - %s not found or you can't interact with it.", guid.GetString().c_str());
        return;
    }

    if (m_bot->m_taxi.IsTaximaskNodeKnown(nodes[0]) ? 0 : 1)
        return;

    if (m_bot->m_taxi.IsTaximaskNodeKnown(nodes[nodes.size() - 1]) ? 0 : 1)
        return;

    if (m_bot->GetPlayerbotAI()->GetMovementOrder() != MOVEMENT_STAY)
    {
        m_taxiNodes = nodes;
        m_taxiMaster = guid;
        SetState(BOTSTATE_FLYING);
    }
}

// handle commands sent through chat channels
void PlayerbotAI::HandleCommand(const std::string& text, Player& fromPlayer)
{
    // prevent bot task spam
    m_inventory_full = false;
    m_tasks.unique();
    m_findNPC.unique();

    if (m_bDebugCommandChat)
    {
        DEBUG_LOG("chat(%s)", text.c_str());
        TellMaster(text);
    }

    // ignore any messages from Addons
    if (text.empty()                                   ||
            text.find("X-Perl")      != std::wstring::npos ||
            text.find("HealBot")     != std::wstring::npos ||
            text.find("HealComm")    != std::wstring::npos ||   // "HealComm    99990094"
            text.find("LOOT_OPENED") != std::wstring::npos ||
            text.find("CTRA")        != std::wstring::npos ||
            text.find("GathX")       == 0)                      // Gatherer
        return;

    // if message is not from a player in the masters account auto reply and ignore
    if (!canObeyCommandFrom(fromPlayer))
    {
        // only tell the player once instead of endlessly nagging them
        if (m_ignorePlayersChat.find(fromPlayer.GetObjectGuid()) == m_ignorePlayersChat.end())
        {
            std::string msg = "I can't talk to you. Please speak to my master ";
            msg += GetMaster()->GetName();
            SendWhisper(msg, fromPlayer);
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
            m_ignorePlayersChat.insert(fromPlayer.GetObjectGuid());
        }
        return;
    }

    // Passed along to ExtractCommand, if (sub)command is found "input" will only contain the rest of the string (or "")
    std::string input = text.c_str();

    // if in the middle of a trade, and player asks for an item/money
    // WARNING: This makes it so you can't use any other commands during a trade!
    if (m_bot->GetTrader() && m_bot->GetTrader()->GetObjectGuid() == fromPlayer.GetObjectGuid())
    {
        uint32 copper = extractMoney(text);
        if (copper > 0)
            TradeCopper(copper);

        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        if (itemIds.size() == 0)
            SendWhisper("Show me what item you want by shift clicking the item in the chat window.", fromPlayer);
        else if (!strncmp(text.c_str(), "nt ", 3))
        {
            if (itemIds.size() > 1)
                SendWhisper("There is only one 'Will not be traded' slot. Shift-click just one item, please!", fromPlayer);
            else
            {
                std::list<Item*> itemList;
                findItemsInEquip(itemIds, itemList);
                findItemsInInv(itemIds, itemList);
                if (itemList.size() > 0)
                    TradeItem((**itemList.begin()), TRADE_SLOT_NONTRADED);
                else
                    SendWhisper("I do not have this item equipped or in my bags!", fromPlayer);
            }
        }
        else
        {
            std::list<Item*> itemList;
            findItemsInInv(itemIds, itemList);
            for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
                TradeItem(**it);
        }
    }

    else if (ExtractCommand("help", input))
        _HandleCommandHelp(input, fromPlayer);

    // debug can be enabled without needing to change playerbot.conf file
    else if (text == "debug")
    {
        TellMaster("Debugging is on. Type 'no debug' to disable.");
        m_debugWhisper = true;
    }
    else if (text == "no debug")
    {
        TellMaster("Debugging is off.");
        m_debugWhisper = false;
    }

    else if (fromPlayer.GetSession()->GetSecurity() > SEC_PLAYER && ExtractCommand("gm", input))
        _HandleCommandGM(input, fromPlayer);

    else if (ExtractCommand("reset", input))
        _HandleCommandReset(input, fromPlayer);
    else if (ExtractCommand("orders", input))
        _HandleCommandOrders(input, fromPlayer);
    else if (ExtractCommand("follow", input) || ExtractCommand("come", input))
        _HandleCommandFollow(input, fromPlayer);
    else if (ExtractCommand("stay", input) || ExtractCommand("stop", input))
        _HandleCommandStay(input, fromPlayer);
    else if (ExtractCommand("attack", input))
        _HandleCommandAttack(input, fromPlayer);
    else if (ExtractCommand("pull", input))
        _HandleCommandPull(input, fromPlayer);

    else if (ExtractCommand("neutralize", input) || ExtractCommand("neutral", input))
        _HandleCommandNeutralize(input, fromPlayer);

    else if (ExtractCommand("cast", input, true)) // true -> "cast" OR "c"
        _HandleCommandCast(input, fromPlayer);

    else if (ExtractCommand("sell", input))
        _HandleCommandSell(input, fromPlayer);

    else if (ExtractCommand("buy", input))
        _HandleCommandBuy(input, fromPlayer);

    else if (ExtractCommand("drop", input))
        _HandleCommandDrop(input, fromPlayer);

    else if (ExtractCommand("repair", input))
        _HandleCommandRepair(input, fromPlayer);

    else if (ExtractCommand("auction", input))
        _HandleCommandAuction(input, fromPlayer);

    else if (ExtractCommand("mail", input))
        _HandleCommandMail(input, fromPlayer);

    else if (ExtractCommand("bank", input))
        _HandleCommandBank(input, fromPlayer);

    else if (ExtractCommand("talent", input))
        _HandleCommandTalent(input, fromPlayer);

    else if (ExtractCommand("use", input, true)) // true -> "use" OR "u"
        _HandleCommandUse(input, fromPlayer);

    else if (ExtractCommand("equip", input, true)) // true -> "equip" OR "e"
        _HandleCommandEquip(input, fromPlayer);

    // find project: 20:50 02/12/10 rev.4 item in world and wait until ordered to follow
    else if (ExtractCommand("find", input, true)) // true -> "find" OR "f"
        _HandleCommandFind(input, fromPlayer);

    // get project: 20:50 02/12/10 rev.4 compact edition, handles multiple linked gameobject & improves visuals
    else if (ExtractCommand("get", input, true)) // true -> "get" OR "g"
        _HandleCommandGet(input, fromPlayer);

    // Handle all collection related commands here
    else if (ExtractCommand("collect", input))
        _HandleCommandCollect(input, fromPlayer);

    else if (ExtractCommand("quest", input))
        _HandleCommandQuest(input, fromPlayer);

    else if (ExtractCommand("craft", input))
        _HandleCommandCraft(input, fromPlayer);

    else if (ExtractCommand("enchant", input))
        _HandleCommandEnchant(input, fromPlayer);

    else if (ExtractCommand("process", input))
        _HandleCommandProcess(input, fromPlayer);

    else if (ExtractCommand("pet", input))
        _HandleCommandPet(input, fromPlayer);

    else if (ExtractCommand("spells", input))
        _HandleCommandSpells(input, fromPlayer);

    // survey project: 20:50 02/12/10 rev.4 compact edition
    else if (ExtractCommand("survey", input))
        _HandleCommandSurvey(input, fromPlayer);

    // Handle class & professions training:
    else if (ExtractCommand("skill", input))
        _HandleCommandSkill(input, fromPlayer);

    // stats project: 11:30 15/12/10 rev.2 display bot statistics
    else if (ExtractCommand("stats", input))
        _HandleCommandStats(input, fromPlayer);

    else
    {
        // if this looks like an item link, reward item it completed quest and talking to NPC
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        if (!itemIds.empty())
        {
            uint32 itemId = itemIds.front();
            bool wasRewarded = false;
            ObjectGuid questRewarderGUID = m_bot->GetSelectionGuid();
            Object* const pNpc = (WorldObject*) m_bot->GetObjectByTypeMask(questRewarderGUID, TYPEMASK_CREATURE_OR_GAMEOBJECT);
            if (!pNpc)
                return;

            QuestMenu& questMenu = m_bot->GetPlayerMenu()->GetQuestMenu();
            for (uint32 iI = 0; !wasRewarded && iI < questMenu.MenuItemCount(); ++iI)
            {
                QuestMenuItem const& qItem = questMenu.GetItem(iI);

                uint32 questID = qItem.m_qId;
                Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);
                QuestStatus status = m_bot->GetQuestStatus(questID);

                // if quest is complete, turn it in
                if (status == QUEST_STATUS_COMPLETE &&
                        !m_bot->GetQuestRewardStatus(questID) &&
                        pQuest->GetRewChoiceItemsCount() > 1 &&
                        m_bot->CanRewardQuest(pQuest, false))
                    for (uint8 rewardIdx = 0; !wasRewarded && rewardIdx < pQuest->GetRewChoiceItemsCount(); ++rewardIdx)
                    {
                        ItemPrototype const* const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        if (itemId == pRewardItem->ItemId)
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, pNpc, false);

                            std::string itemName = pRewardItem->Name1;
                            m_bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);

                            std::ostringstream out;
                            MakeItemLink(pRewardItem, out);
                            SendWhisper(out.str(), fromPlayer);
                            wasRewarded = true;
                        }
                    }
            }
        }
        else
        {
            // TODO: make this only in response to direct whispers (chatting in party chat can in fact be between humans)
            std::string msg = "What is [";
            std::string textsub;
            if (text.length() > 10)
                textsub = text.substr(0, 10) + "...";
            else
                textsub = text;
            msg += textsub.c_str();
            msg += "]? For a list of commands, ask for 'help'.";
            SendWhisper(msg, fromPlayer);
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }
}

/**
* ExtractCommand looks for a command in a text string
* sLookingFor       - string you're looking for (e.g. "help")
* text              - string which may or may not start with sLookingFor
* bUseShort         - does this command accept the shorthand command? If true, "help" would ALSO look for "h"
*
* returns true if the string has been found
* returns false if the string has not been found
*/
bool PlayerbotAI::ExtractCommand(const std::string sLookingFor, std::string& text, bool bUseShort)
{
    // ("help" + " ") < "help X"  AND  text's start (as big as sLookingFor) == sLookingFor
    // Recommend AGAINST adapting this for non-space situations (thinking MangosZero)
    // - unknown would risk being (short for "use") 'u' + "nknown"
    if (sLookingFor.size() + 1 < text.size() && text.at(sLookingFor.size()) == ' '
            && 0 == text.substr(0, sLookingFor.size()).compare(sLookingFor))
    {
        text = text.substr(sLookingFor.size() + 1);
        return true;
    }

    if (0 == text.compare(sLookingFor))
    {
        text = "";
        return true;
    }

    if (bUseShort)
    {
        if (text.size() > 1 && sLookingFor.at(0) == text.at(0) && text.at(1) == ' ')
        {
            text = text.substr(2);
            return true;
        }
        else if (text.size() == 1 && sLookingFor.at(0) == text.at(0))
        {
            text = "";
            return true;
        }
    }

    return false;
}

void PlayerbotAI::_HandleCommandReset(std::string& text, Player& fromPlayer)
{
    if (!text.empty())
    {
        SendWhisper("reset does not have a subcommand.", fromPlayer);
        return;
    }
    SetState(BOTSTATE_NORMAL);
    MovementReset();
    SetQuestNeedItems();
    SetQuestNeedCreatures();
    UpdateAttackerInfo();
    m_lootTargets.clear();
    m_lootCurrent = ObjectGuid();
    m_targetCombat = nullptr;
    ClearActiveTalentSpec();
}

void PlayerbotAI::_HandleCommandOrders(std::string& text, Player& fromPlayer)
{
    if (ExtractCommand("delay", text))
    {
        uint32 gdelay;
        sscanf(text.c_str(), "%d", &gdelay);
        if (gdelay <= 10)
        {
            m_DelayAttack = gdelay;
            TellMaster("Combat delay is now '%u' ", m_DelayAttack);
            CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET combat_delay = '%u' WHERE guid = '%u'", m_DelayAttack, m_bot->GetGUIDLow());
            return;
        }
        else
            SendWhisper("Invalid delay. choose a number between 0 and 10", fromPlayer);
        return;
    }
    else if (ExtractCommand("resume", text))
        CombatOrderRestore();
    else if (ExtractCommand("combat", text, true))
    {
        Unit* target = nullptr;

        if (text.empty())
        {
            SendWhisper("|cffff0000Syntax error:|cffffffff orders combat <botName> <reset | tank | heal | passive><assist | protect [targetPlayer]>", fromPlayer);
            return;
        }

        QueryResult* resultlvl = CharacterDatabase.PQuery("SELECT guid FROM playerbot_saved_data WHERE guid = '%u'", m_bot->GetObjectGuid().GetCounter());
        if (!resultlvl)
            CharacterDatabase.DirectPExecute("INSERT INTO playerbot_saved_data (guid,combat_order,primary_target,secondary_target,pname,sname,combat_delay,auto_follow,autoequip) VALUES ('%u',0,0,0,'','',0,0,false)", m_bot->GetObjectGuid().GetCounter());
        else
            delete resultlvl;

        size_t protect = text.find("protect");
        size_t assist = text.find("assist");

        if (ExtractCommand("protect", text) || ExtractCommand("assist", text))
        {
            ObjectGuid targetGUID = fromPlayer.GetSelectionGuid();
            if (text == "" && !targetGUID)
                return SendWhisper("|cffff0000Combat orders protect and assist expect a target either by selection or by giving target player in command string!", fromPlayer);

            if (!text.empty())
            {
                ObjectGuid targ_guid = sObjectMgr.GetPlayerGuidByName(text.c_str());
                targetGUID.Set(targ_guid.GetRawValue());
            }
            target = ObjectAccessor::GetUnit(fromPlayer, targetGUID);
            if (!target)
                return SendWhisper("|cffff0000Invalid target for combat order protect or assist!", fromPlayer);

            if (protect != std::string::npos)
                SetCombatOrder(ORDERS_PROTECT, target);
            else if (assist != std::string::npos)
                SetCombatOrder(ORDERS_ASSIST, target);
        }
        else
            SetCombatOrderByStr(text, target);
    }
    else if (!text.empty())
    {
        SendWhisper("See help for details on using 'orders'.", fromPlayer);
        return;
    }
    SendOrders(*GetMaster());
}

void PlayerbotAI::_HandleCommandFollow(std::string& text, Player& fromPlayer)
{
    if (ExtractCommand("auto", text)) // switch to automatic follow distance
    {
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'follow'", fromPlayer);
            return;
        }
        DistOverRide = 0; // this resets follow distance to config default
        IsUpOrDown = 0;
        std::ostringstream msg;
        gTempDist = 1;
        gTempDist2 = 2;

        if (m_FollowAutoGo != FOLLOWAUTOGO_SET)
        {
            m_FollowAutoGo = FOLLOWAUTOGO_INIT;
            msg << "Automatic Follow Distance is now |h|cff1eff00ON|h|r";
            SendWhisper(msg.str(), fromPlayer);
        }
        else
        {
            m_FollowAutoGo = FOLLOWAUTOGO_OFF;
            msg << "Automatic Follow Distance is now |h|cffff0000OFF|h|r";
            SendWhisper(msg.str(), fromPlayer);
        }
        CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET auto_follow = '%u' WHERE guid = '%u'", m_FollowAutoGo, m_bot->GetGUIDLow());
    }
    else if (ExtractCommand("reset", text)) // switch to reset follow distance
    {
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'follow'", fromPlayer);
            return;
        }
        DistOverRide = 0; // this resets follow distance to config default
        IsUpOrDown = 0;
        std::ostringstream msg;
        gTempDist = 1;
        gTempDist2 = 2;
        msg << "Bit crowded isn't it?";
        SendWhisper(msg.str(), fromPlayer);
    }
    else if (ExtractCommand("far", text)) // switch to increment follow distance
    {
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'follow'", fromPlayer);
            return;
        }
        DistOverRide = (DistOverRide + 1); // this increments follow distance
        std::ostringstream msg;
        msg << "Increasing My follow distance";
        SendWhisper(msg.str(), fromPlayer);
    }
    else if (ExtractCommand("near", text)) // switch to increment follow distance
    {
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'follow'", fromPlayer);
            return;
        }
        if (DistOverRide > 0)
            DistOverRide = (DistOverRide - 1); // this increments follow distance,

        std::ostringstream msg;
        if (DistOverRide == 0)
        {
            IsUpOrDown = 0;
            DistOverRide = 0;
            gTempDist = 1;
            gTempDist2 = 2;
            msg << "I'm NOT getting any closer than this";
        }
        if (DistOverRide != 0)
            msg << "Decreasing My follow distance";
        SendWhisper(msg.str(), fromPlayer);
    }
    else if (ExtractCommand("info", text))
    {
        std::ostringstream msg;

        msg << "Automatic Follow Distance is ";

        switch (DistOverRide)
        {
            case 0: msg << "|h|cffff0000" << m_bot->GetDistance(GetMaster()) << "|h|r"; break; //red
            case 1: msg << "|h|cffff8000" << m_bot->GetDistance(GetMaster()) << "|h|r"; break; //yellow
            case 2: msg << "|h|cffe6cc80" << m_bot->GetDistance(GetMaster()) << "|h|r"; break; //orange
            case 3: msg << "|h|cff1eff00" << m_bot->GetDistance(GetMaster()) << "|h|r"; break; //green
            case 4:
            default: msg << "|h|cff0070dd" << m_bot->GetDistance(GetMaster()) << "|h|r"; break; //blue
        }

        m_FollowAutoGo ?  SendWhisper(msg.str(), fromPlayer) : SendWhisper("Automatic Follow Distance is |h|cffff0000OFF|h|r", fromPlayer);
        return;
    }
    else if (!text.empty())
    {
        SendWhisper("see help for details on using follow.", fromPlayer);
        return;
    }
    SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
}

void PlayerbotAI::_HandleCommandStay(std::string& text, Player& fromPlayer)
{
    if (!text.empty())
    {
        SendWhisper("stay cannot have a subcommand.", fromPlayer);
        return;
    }
    SetMovementOrder(MOVEMENT_STAY);
}

void PlayerbotAI::_HandleCommandAttack(std::string& text, Player& fromPlayer)
{
    if (!text.empty())
    {
        SendWhisper("attack cannot have a subcommand.", fromPlayer);
        return;
    }
    ObjectGuid attackOnGuid = fromPlayer.GetSelectionGuid();
    if (attackOnGuid)
    {
        if (Unit* thingToAttack = ObjectAccessor::GetUnit(*m_bot, attackOnGuid))
        {
            if (m_bot->CanAttack(thingToAttack))
            {
                if (!m_bot->IsWithinLOSInMap(thingToAttack))
                    DoTeleport(*m_followTarget);
                if (m_bot->IsWithinLOSInMap(thingToAttack))
                    Attack(thingToAttack);
            }
        }
    }
    else
    {
        SendWhisper("No target is selected.", fromPlayer);
        m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    }
}

void PlayerbotAI::_HandleCommandPull(std::string& text, Player& fromPlayer)
{
    bool bReadyCheck = false;

    if (!m_bot) return;

    if (ExtractCommand("test", text)) // switch to automatic follow distance
    {
        if (CanPull(fromPlayer))
            SendWhisper("Looks like I am capable of pulling. Ask me 'pull ready' with a target for a more precise check.", fromPlayer);
        return;
    }
    if (ExtractCommand("ready", text)) // switch to automatic follow distance
    {
        bReadyCheck = true;
    }
    else if (!text.empty())
    {
        SendWhisper("See 'help pull' for details on using the pull command.", fromPlayer);
        return;
    }

    // This function also takes care of error reporting
    if (!CanPull(fromPlayer))
        return;

    // Check for valid target
    m_bot->SetSelectionGuid(fromPlayer.GetSelectionGuid());
    ObjectGuid attackOnGuid = m_bot->GetSelectionGuid();
    if (!attackOnGuid)
    {
        SendWhisper("No target is selected.", fromPlayer);
        return;
    }

    Unit* thingToAttack = ObjectAccessor::GetUnit(*m_bot, attackOnGuid);
    if (!thingToAttack)
    {
        SendWhisper("No target is selected.", fromPlayer);
        return;
    }

    if (m_bot->CanAssist(thingToAttack))
    {
        SendWhisper("Where I come from we don't attack our friends.", fromPlayer);
        return;
    }
    // TODO: Okay, this one should actually be fixable. InMap should return, but LOS (Line of Sight) should result in moving, well, into LoS.
    if (!m_bot->IsWithinLOSInMap(thingToAttack))
    {
        SendWhisper("I can't see that target!", fromPlayer);
        return;
    }
    GetCombatTarget(thingToAttack);
    if (!GetCurrentTarget())
    {
        SendWhisper("Failed to set target, cause unknown.", fromPlayer);
        return;
    }

    if (bReadyCheck)
    {
        SendWhisper("All checks have been passed and I am ready to pull! ... Are you sure you wouldn't like a smaller target?", fromPlayer);
        return;
    }

    // All healers which have it available will cast any applicable HoT (Heal over Time) spell on the tank
    GroupHoTOnTank();

    /* Technically the tank should wait a bit if/until the HoT has been applied
       but the above function immediately casts it rather than wait for an UpdateAI tick

       There is no need to take into account that GroupHoTOnTank() may fail due to global cooldown. Either you're prepared for a difficult
       pull in which case it won't fail due to global cooldown, or you're chaining easy pulls in which case you don't care.
       */
    /* So have the group wait for the tank to take action (and aggro) - this way it will be easy to see if tank has aggro or not without having to
       worry about tank not being the first to have UpdateAI() called
       */

    // Need to have a group and a tank, both checked in "CanPull()" call above
    //if (!(GetGroupTank()->GetPlayerbotAI()->GetClassAI()->Pull()))
    // I've been told to pull and a check was done above whether I'm actually a tank, so *I* will try to pull:
    if (!CastPull())
    {
        SendWhisper("I did my best but I can't actually pull. How odd.", fromPlayer);
        return;
    }

    // Sets Combat Orders to PULL
    SetGroupCombatOrder(ORDERS_TEMP_WAIT_TANKAGGRO);

    SetGroupIgnoreUpdateTime(2);

    // Set all group members (save this tank) to wait 10 seconds. They will wait until the tank says so, until any non-tank gains aggro or 10 seconds - whichever is shortest
    if (m_bot->GetGroup()) // one last sanity check, should be unnecessary
    {
        Group::MemberSlotList const& groupSlot = m_bot->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!groupMember || !groupMember->GetPlayerbotAI() || groupMember == m_bot)
                continue;
            groupMember->GetPlayerbotAI()->GetClassAI()->SetWait(10);
        }
    }

    //(4a) if tank, deactivate any attack (such as 'shoot (bow/gun)' for warriors), wait until in melee range, attack
    //(4b) if dps, wait until the target is in melee range of the tank +2seconds or until tank no longer holds aggro
    //(4c) if healer, do healing checks
    //(5) when target is in melee range of tank, wait 2 seconds (healers continue to do group heal checks, all do self-heal checks), then return to normal functioning
}

void PlayerbotAI::_HandleCommandNeutralize(std::string& text, Player& fromPlayer)
{
    if (!m_bot) return;

    if (!text.empty())
    {
        SendWhisper("See 'help neutralize' for details on using the neutralize command.", fromPlayer);
        return;
    }

    // Check for valid target
    m_bot->SetSelectionGuid(fromPlayer.GetSelectionGuid());
    ObjectGuid selectOnGuid = m_bot->GetSelectionGuid();
    if (!selectOnGuid)
    {
        SendWhisper("No target is selected.", fromPlayer);
        return;
    }

    Unit* thingToNeutralize = ObjectAccessor::GetUnit(*m_bot, selectOnGuid);
    if (!thingToNeutralize)
    {
        SendWhisper("No valid target is selected.", fromPlayer);
        return;
    }

    if (m_bot->CanAssist(thingToNeutralize))
    {
        SendWhisper("I can't neutralize that target: this is a friend to me.", fromPlayer);
        return;
    }

    if (!m_bot->IsWithinLOSInMap(thingToNeutralize))
    {
        SendWhisper("I can't see that target!", fromPlayer);
        return;
    }

    if (IsNeutralized(thingToNeutralize))
    {
        SendWhisper("Target is already neutralized.", fromPlayer);
        return;
    }

    m_targetGuidCommand = selectOnGuid;

    // All checks passed: call the Neutralize function of each bot class
    // to define what spellid to use if available and if creature type is correct
    // m_spellIdCommand will be defined there and UpdateAI will then handle the cast
    if (!CastNeutralize())
    {
        SendWhisper("Something went wrong: I can't neutralize that target.", fromPlayer);
        return;
    }
}

void PlayerbotAI::_HandleCommandCast(std::string& text, Player& fromPlayer)
{
    if (text.empty())
    {
        SendWhisper("cast must be used with a single spell link (shift + click the spell).", fromPlayer);
        return;
    }

    std::string spellStr = text;
    uint32 spellId = (uint32) atol(spellStr.c_str());

    // try and get spell ID by name
    if (spellId == 0)
    {
        spellId = getSpellId(spellStr.c_str(), true);

        // try link if text NOT (spellid OR spellname)
        if (spellId == 0)
            extractSpellId(text, spellId);
    }

    if (m_bot->HasAura(spellId))
    {
        m_bot->RemoveAurasByCasterSpell(spellId, m_bot->GetObjectGuid());
        return;
    }

    ObjectGuid castOnGuid = fromPlayer.GetSelectionGuid();
    if (spellId != 0 && m_bot->HasSpell(spellId))
    {
        m_spellIdCommand = spellId;
        if (castOnGuid)
            m_targetGuidCommand = castOnGuid;
        else
            m_targetGuidCommand = m_bot->GetObjectGuid();
    }
}

// _HandleCommandSell: Handle selling items
// sell [Item Link][Item Link] .. -- Sells bot(s) items from inventory
void PlayerbotAI::_HandleCommandSell(std::string& text, Player& fromPlayer)
{
    FollowAutoReset();
    if (ExtractCommand("all", text)) // switch to auto sell low level white items
    {
        std::ostringstream msg;
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'sell all'", fromPlayer);
            return;
        }
        SellWhite = !SellWhite;
        msg << "I will " << (SellWhite ? "" : "no longer ") << "sell my low level normal items.";
        SendWhisper(msg.str(), fromPlayer);
        return;
    }
    if (text.empty())
    {
        SendWhisper("sell must be used with one or more item links (shift + click the item).", fromPlayer);
        return;
    }

    std::list<uint32> itemIds;
    extractItemIds(text, itemIds);
    for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(SELL_ITEMS, *it));
    m_findNPC.push_back(VENDOR_MASK);
}

// _HandleCommandBuy: Handle buying items
// buy [Item Link][Item Link] .. -- Buys items from vendor
void PlayerbotAI::_HandleCommandBuy(std::string& text, Player& fromPlayer)
{
    if (text.empty())
    {
        SendWhisper("buy must be used with one or more item links (shift + click the item).", fromPlayer);
        return;
    }

    FollowAutoReset();

    std::list<uint32> itemIds;
    extractItemIds(text, itemIds);
    for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(BUY_ITEMS, *it));
    m_findNPC.push_back(VENDOR_MASK);
}

// _HandleCommandDrop: Handle dropping items
// drop [Item Link][Item Link] .. -- Drops item(s) from bot's inventory
void PlayerbotAI::_HandleCommandDrop(std::string& text, Player& fromPlayer)
{
    if (ExtractCommand("all", text)) // switch to auto drop low level white items
    {
        std::ostringstream msg;
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'drop all'", fromPlayer);
            return;
        }
        m_dropWhite = !m_dropWhite;
        msg << "I will " << (m_dropWhite ? "" : "no longer ") << "drop my low level normal items.";
        SendWhisper(msg.str(), fromPlayer);
        return;
    }
    if (text == "")
    {
        SendWhisper("drop must be used with one or more item links (shift + click the item).", fromPlayer);
        return;
    }

    std::ostringstream report;
    std::list<uint32> itemIds;
    extractItemIds(text, itemIds);
    report << "Dropped ";
    for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
        report << DropItem(*it);

    if (report.str() == "Dropped ")
    {
        SendWhisper("No items were dropped. It would appear something has gone hinky.", fromPlayer);
        return;
    }

    report << ".";
    SendWhisper(report.str(), fromPlayer);
}

// _HandleCommandRepair: Handle repair items
// repair  all                      -- repair all bot(s) items
// repair [Item Link][Item Link] .. -- repair select bot(s) items
void PlayerbotAI::_HandleCommandRepair(std::string& text, Player& fromPlayer)
{
    FollowAutoReset();
    if (ExtractCommand("all", text))
    {
        if (!text.empty())
        {
            SendWhisper("Invalid subcommand for 'repair all'", fromPlayer);
            return;
        }
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(REPAIR_ITEMS, 0));
        m_findNPC.push_back(UNIT_NPC_FLAG_REPAIR);
        return;
    }

    std::list<uint32> itemIds;
    extractItemIds(text, itemIds);

    for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); it++)
    {
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(REPAIR_ITEMS, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_REPAIR);
    }
}

// _HandleCommandAuction: Handle auctions:
// auction                                        -- Lists bot(s) active auctions.
// auction add [Item Link][Item Link] ..          -- Create bot(s) active auction.
// auction remove [Auction Link][Auction Link] .. -- Cancel bot(s) active auction. ([Auction Link] from auction)
void PlayerbotAI::_HandleCommandAuction(std::string& text, Player& fromPlayer)
{
    FollowAutoReset();
    if (text == "")
        m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);  // list all bot auctions
    else if (ExtractCommand("add", text))
    {
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
            m_tasks.push_back(std::pair<enum TaskFlags, uint32>(ADD_AUCTION, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);
    }
    else if (ExtractCommand("remove", text))
    {
        std::list<uint32> auctionIds;
        extractAuctionIds(text, auctionIds);
        for (std::list<uint32>::iterator it = auctionIds.begin(); it != auctionIds.end(); ++it)
            m_tasks.push_back(std::pair<enum TaskFlags, uint32>(REMOVE_AUCTION, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);
    }
    else
        SendWhisper("I don't understand what you're trying to do", fromPlayer);
}

void PlayerbotAI::_HandleCommandMail(std::string& text, Player& fromPlayer)
{
    ChatHandler ch(&fromPlayer);

    if (text.empty())
    {
        ch.SendSysMessage("Syntax: mail <inbox [Mailbox] | getcash [mailid].. | getitem [mailid].. | delete [mailid]..>");
        return;
    }
    else if (ExtractCommand("inbox", text))
    {
        uint32 mail_count = 0;
        extractGOinfo(text, m_lootTargets);

        if (m_lootTargets.empty())
        {
            ch.SendSysMessage("Syntax: mail <inbox [Mailbox]>");
            return;
        }

        ObjectGuid m_mailboxGuid = m_lootTargets.front();
        m_lootTargets.pop_front();
        m_lootTargets.clear();

        if (!m_bot->GetGameObjectIfCanInteractWith(m_mailboxGuid, GAMEOBJECT_TYPE_MAILBOX))
        {
            Announce(CANT_USE_TOO_FAR);
            return;
        }

        TellMaster("Inbox:\n");

        for (PlayerMails::reverse_iterator itr = m_bot->GetMailRBegin(); itr != m_bot->GetMailREnd(); ++itr)
        {
            std::ostringstream msg;
            ++mail_count;

            msg << "|cffffcccc|Hmail:" << (*itr)->messageID << "|h[" << (*itr)->messageID << "]|h|r ";

            switch ((*itr)->messageType)
            {
                case MAIL_NORMAL:
                {
                    msg << "|cffffffff"; // white
                    if ((*itr)->subject != "")
                        msg << "Subject: " << (*itr)->subject << "\n";

                    if ((*itr)->body != "")
                        msg << (*itr)->body << "\n";
                    break;
                }
                case MAIL_CREATURE:
                    msg << "|cffccffccMAIL_CREATURE\n"; // green
                    break;
                case MAIL_GAMEOBJECT:
                    msg << "|cffccffccMAIL_GAMEOBJECT\n"; // green
                    break;
                case MAIL_AUCTION:
                {
                    msg << "|cffccffff"; // blue
                    msg << AuctionResult((*itr)->subject, (*itr)->body) << "\n";
                    break;
                }
                case MAIL_CALENDAR:
                    msg << "|cffccffccMAIL_ITEM\n"; // green
                    break;
            }

            if ((*itr)->money)
                msg << "[To Collect: " << Cash((*itr)->money) << " ]\n";

            uint8 item_count = (*itr)->items.size(); // max count is MAX_MAIL_ITEMS (12)
            if (item_count > 0)
            {
                msg << "Items: ";
                for (uint8 i = 0; i < item_count; ++i)
                {
                    Item* item = m_bot->GetMItem((*itr)->items[i].item_guid);
                    if (item)
                        MakeItemLink(item, msg, true);
                }
            }
            msg << "\n";
            ch.SendSysMessage(msg.str().c_str());
        }

        if (mail_count == 0)
            ch.SendSysMessage("|cff009900My inbox is empty.");
    }
    else if (ExtractCommand("getcash", text))
    {
        std::ostringstream msg;
        std::list<uint32> mailIds;
        extractMailIds(text, mailIds);
        mailIds.unique();
        mailIds.sort();
        uint32 total = 0;

        if (mailIds.empty())
        {
            ch.SendSysMessage("Syntax: mail <getcash [mailId]..>");
            return;
        }

        for (std::list<uint32>::iterator it = mailIds.begin(); it != mailIds.end(); ++it)
        {
            Mail* m = m_bot->GetMail(*it);
            if (!m || m->state == MAIL_STATE_DELETED || m->deliver_time > CurrentTime())
            {
                m_bot->SendMailResult(*it, MAIL_MONEY_TAKEN, MAIL_ERR_INTERNAL_ERROR);
                return;
            }

            m_bot->SendMailResult(*it, MAIL_MONEY_TAKEN, MAIL_OK);
            m_bot->ModifyMoney(m->money);
            total += m->money;
            m->money = 0;
            m->state = MAIL_STATE_CHANGED;
            m_bot->m_mailsUpdated = true;
            m_bot->UpdateMail();
        }
        if (total > 0)
        {
            msg << "|cff009900" << "I received: |r" << Cash(total);
            ch.SendSysMessage(msg.str().c_str());
        }
    }
    else if (ExtractCommand("getitem", text))
    {
        std::list<uint32> mailIds;
        extractMailIds(text, mailIds);
        mailIds.unique();
        mailIds.sort();

        if (mailIds.empty())
        {
            ch.SendSysMessage("Syntax: mail <getitem [mailId]..>");
            return;
        }

        for (std::list<uint32>::iterator it = mailIds.begin(); it != mailIds.end(); it++)
        {
            Mail* m = m_bot->GetMail(*it);
            if (!m || m->state == MAIL_STATE_DELETED || m->deliver_time > CurrentTime())
            {
                m_bot->SendMailResult(*it, MAIL_ITEM_TAKEN, MAIL_ERR_INTERNAL_ERROR);
                return;
            }

            // prevent cheating with skip client money check
            if (m_bot->GetMoney() < m->COD)
            {
                m_bot->SendMailResult(*it, MAIL_ITEM_TAKEN, MAIL_ERR_NOT_ENOUGH_MONEY);
                return;
            }

            if (m->HasItems())
            {
                bool has_items = true;
                std::ostringstream msg;

                msg << "|cff009900" << "I received item: |r";
                for (MailItemInfoVec::const_iterator itr = m->items.begin(); itr != m->items.end();)
                {
                    has_items = true;
                    Item* item = m_bot->GetMItem(itr->item_guid);
                    if (!item)
                    {
                        ch.SendSysMessage("item not found");
                        return;
                    }

                    ItemPosCountVec dest;

                    InventoryResult res = m_bot->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false);
                    if (res == EQUIP_ERR_OK)
                    {
                        m->removedItems.push_back(itr->item_guid);

                        if (m->COD > 0)  // if there is COD, take COD money from player and send them to sender by mail
                        {
                            ObjectGuid sender_guid = ObjectGuid(HIGHGUID_PLAYER, m->sender);
                            Player* sender = sObjectMgr.GetPlayer(sender_guid);

                            uint32 sender_accId = 0;

                            if (GetMaster()->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE))
                            {
                                std::string sender_name;
                                if (sender)
                                {
                                    sender_accId = sender->GetSession()->GetAccountId();
                                    sender_name = sender->GetName();
                                }
                                else if (sender_guid)
                                {
                                    // can be calculated early
                                    sender_accId = sObjectMgr.GetPlayerAccountIdByGUID(sender_guid);

                                    if (!sObjectMgr.GetPlayerNameByGUID(sender_guid, sender_name))
                                        sender_name = sObjectMgr.GetMangosStringForDbcLocale(LANG_UNKNOWN);
                                }
                                sLog.outCommand(GetMaster()->GetSession()->GetAccountId(), "GM %s (Account: %u) receive mail item: %s (Entry: %u Count: %u) and send COD money: %u to player: %s (Account: %u)",
                                                GetMaster()->GetSession()->GetPlayerName(), GetMaster()->GetSession()->GetAccountId(), item->GetProto()->Name1, item->GetEntry(), item->GetCount(), m->COD, sender_name.c_str(), sender_accId);
                            }
                            else if (!sender)
                                sender_accId = sObjectMgr.GetPlayerAccountIdByGUID(sender_guid);

                            // check player existence
                            if (sender || sender_accId)
                                MailDraft(m->subject, "")
                                .SetMoney(m->COD)
                                .SendMailTo(MailReceiver(sender, sender_guid), m_bot, MAIL_CHECK_MASK_COD_PAYMENT);

                            m_bot->ModifyMoney(-int32(m->COD));
                        }
                        m->COD = 0;
                        m->state = MAIL_STATE_CHANGED;
                        m_bot->m_mailsUpdated = true;
                        m_bot->RemoveMItem(item->GetGUIDLow());

                        uint32 count = item->GetCount(); // save counts before store and possible merge with deleting
                        m_bot->MoveItemToInventory(dest, item, true);
                        m_bot->UpdateMail();
                        m_bot->SendMailResult(*it, MAIL_ITEM_TAKEN, MAIL_OK, 0, itr->item_guid, count);
                        if (m->RemoveItem(itr->item_guid))
                        {
                            MakeItemLink(item, msg, true);
                            has_items = false;
                        }
                    }
                    else
                        m_bot->SendMailResult(*it, MAIL_ITEM_TAKEN, MAIL_ERR_EQUIP_ERROR, res);
                }

                if (!has_items)
                {
                    CharacterDatabase.BeginTransaction();
                    CharacterDatabase.PExecute("UPDATE mail SET has_items = 0 WHERE id = %u", *it);
                    CharacterDatabase.CommitTransaction();
                }
                msg << "\n";
                ch.SendSysMessage(msg.str().c_str());
            }
        }
    }
    else if (ExtractCommand("delete", text))
    {
        std::ostringstream msg;
        std::list<uint32> mailIds;
        extractMailIds(text, mailIds);
        mailIds.unique();
        mailIds.sort();

        if (mailIds.empty())
        {
            ch.SendSysMessage("Syntax: mail <delete [mailId]..>");
            return;
        }

        msg << "|cff009900Mail ";
        for (std::list<uint32>::iterator it = mailIds.begin(); it != mailIds.end(); ++it)
        {
            m_bot->m_mailsUpdated = true;

            if (Mail* m = m_bot->GetMail(*it))
            {
                // delete shouldn't show up for COD mails
                if (m->COD)
                {
                    m_bot->SendMailResult(*it, MAIL_DELETED, MAIL_ERR_INTERNAL_ERROR);
                    return;
                }
                m->state = MAIL_STATE_DELETED;

                m_bot->SendMailResult(*it, MAIL_DELETED, MAIL_OK);
                CharacterDatabase.BeginTransaction();
                CharacterDatabase.PExecute("DELETE FROM mail WHERE id = '%u'", *it);
                CharacterDatabase.PExecute("DELETE FROM mail_items WHERE mail_id = '%u'", *it);
                CharacterDatabase.CommitTransaction();
                m_bot->RemoveMail(*it);
                msg << "|cffffcccc|h[" << *it << "]|h|r";
            }
        }
        msg << "|cff009900 has been deleted..";
        ch.SendSysMessage(msg.str().c_str());
    }
}

// _HandleCommandBank: Handle bank:
// bank                                        -- Lists bot(s) bank balance.
// bank deposit [Item Link][Item Link] ..      -- Deposit item(s) in bank.
// bank withdraw [Item Link][Item Link] ..     -- Withdraw item(s) from bank. ([Item Link] from bank)
void PlayerbotAI::_HandleCommandBank(std::string& text, Player& fromPlayer)
{
    FollowAutoReset();
    if (text == "")
        m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);  // list all bot balance
    else if (ExtractCommand("deposit", text))
    {
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
            m_tasks.push_back(std::pair<enum TaskFlags, uint32>(BANK_DEPOSIT, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);
    }
    else if (ExtractCommand("withdraw", text))
    {
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
            m_tasks.push_back(std::pair<enum TaskFlags, uint32>(BANK_WITHDRAW, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);
    }
    else
        SendWhisper("I don't understand what you're trying to do", fromPlayer);
}

// _HandleCommandTalent: Handle talents & glyphs:
// talent                           -- Lists bot(s) active talents [TALENT LINK] & glyphs [GLYPH LINK], unspent points & cost to reset
// talent learn [TALENT LINK] ..    -- Learn selected talent from bot client 'inspect' dialog -> 'talent' tab or from talent command (shift click icon/link)
// talent reset                     -- Resets all talents
// talent spec                      -- Lists various talentspecs for this bot's class
// talent spec #                    -- Sets talent spec # as active talentspec
void PlayerbotAI::_HandleCommandTalent(std::string& text, Player& fromPlayer)
{
    std::ostringstream out;
    if (ExtractCommand("learn", text))
    {
        std::list<talentPair>talents;
        extractTalentIds(text, talents);

        for (std::list<talentPair>::iterator itr = talents.begin(); itr != talents.end(); itr++)
        {
            uint32 talentid;
            uint32 rank;

            talentid = itr->first;
            rank = itr->second;

            m_bot->LearnTalent(talentid, ++rank);
            m_bot->SendTalentsInfoData(false);
            InspectUpdate();
        }

        m_bot->MakeTalentGlyphLink(out);
        SendWhisper(out.str(), fromPlayer);
    }
    else if (ExtractCommand("reset", text))
    {
        FollowAutoReset();
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(RESET_TALENTS, 0));
        m_findNPC.push_back(UNIT_NPC_FLAG_TRAINER_CLASS);
    }
    else if (ExtractCommand("spec", text))
    {
        if (0 == GetTalentSpecsAmount())
        {
            SendWhisper("Database does not contain any Talent Specs (for any classes).", fromPlayer);
            return;
        }
        if (text.size() == 0) // no spec chosen nor other subcommand
        {
            std::list<TalentSpec> classSpecs = GetTalentSpecs((long) m_bot->getClass());
            std::list<TalentSpec>::iterator it;
            int count = 0;

            SendWhisper("Please select a talent spec to activate (reply 'talent spec #'):", fromPlayer);
            for (it = classSpecs.begin(); it != classSpecs.end(); it++)
            {
                count++;

                std::ostringstream oss;
                oss << count << ". " << it->specName;
                SendWhisper(oss.str(), fromPlayer);
            }
            if (count == 0)
            {
                std::ostringstream oss;
                oss << "Error: No TalentSpecs listed. Specs retrieved from DB for this class: %u" << m_bot->getClass();
                SendWhisper(oss.str(), fromPlayer);
            }
        }
        else
        {
            uint32 chosenSpec = strtoul(text.c_str(), nullptr, 0); // non-int returns 0; too big returns UINT MAX (or somesuch)

            // Warning: also catches non-int sub2command's - e.g. 'talent spec foobar'
            if (0 == chosenSpec)
            {
                ClearActiveTalentSpec();
                SendWhisper("The talent spec has been cleared.", fromPlayer);
            }
            else if (chosenSpec > GetTalentSpecsAmount((long) m_bot->getClass()))
                SendWhisper("The talent spec you have chosen is invalid. Please select one from the valid range (reply 'talent spec' for options).", fromPlayer);
            else
            {
                TalentSpec ts = GetTalentSpec((long) m_bot->getClass(), chosenSpec);

                // no use setting it to an invalid (and probably - hopefully - empty) TalentSpec
                if (0 != ts.specClass && TSP_NONE != ts.specPurpose)
                {
                    out << "Activated talent spec: " << chosenSpec << ". " << ts.specName;
                    SendWhisper(out.str(), fromPlayer);
                    SetActiveTalentSpec(ts);
                    if (!ApplyActiveTalentSpec())
                    {
                        SendWhisper("The talent spec has been set active but could not be applied. It appears something has gone awry.", fromPlayer);
                        DEBUG_LOG("[PlayerbotAI]: Could set TalentSpec but could not apply it - 'talent spec #': Class: %u; chosenSpec: %u", m_bot->getClass(), chosenSpec);
                    }
                    InspectUpdate();
                }
                else
                {
                    SendWhisper("An error has occured. Please let a Game Master know. This error has been logged.", fromPlayer);
                    DEBUG_LOG("[PlayerbotAI]: Could not GetTalentSpec to set & apply - 'talent spec #': Class: %u; chosenSpec: %u", m_bot->getClass(), chosenSpec);
                }
            }
        }
    }
    // no valid subcommand found for command 'talent'
    else
    {
        uint32 gold = uint32(m_bot->resetTalentsCost() / 10000);

        if (gold > 0)
            out << "Cost to reset all Talents is " << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";

        m_bot->MakeTalentGlyphLink(out);
        SendWhisper(out.str(), fromPlayer);
    }
}

void PlayerbotAI::_HandleCommandProcess(std::string& text, Player& fromPlayer)
{
    uint32 spellId;

    if (ExtractCommand("disenchant", text, true)) // true -> "process disenchant" OR "process d"
    {
        if (m_bot->HasSkill(SKILL_ENCHANTING))
            spellId = DISENCHANTING_1;
        else
        {
            SendWhisper("|cffff0000I can't disenchant, I don't have the skill.", fromPlayer);
            return;
        }
    }
    else if (ExtractCommand("mill", text, true)) // true -> "process mill" OR "process m"
    {
        if (m_bot->HasSkill(SKILL_INSCRIPTION))
            spellId = MILLING_1;
        else
        {
            SendWhisper("|cffff0000I can't mill, I don't have the skill.", fromPlayer);
            return;
        }
    }
    else if (ExtractCommand("prospect", text, true)) // true -> "process prospect" OR "process p"
    {
        if (m_bot->HasSkill(SKILL_JEWELCRAFTING) && m_bot->GetSkillValuePure(SKILL_JEWELCRAFTING) >= 20)
            spellId = PROSPECTING_1;
        else
        {
            SendWhisper("|cffff0000I can't prospect, I don't have the skill.", fromPlayer);
            return;
        }
    }
    else
        return;

    std::list<uint32> itemIds;
    std::list<Item*> itemList;
    extractItemIds(text, itemIds);
    findItemsInInv(itemIds, itemList);

    if (itemList.empty())
    {
        SendWhisper("|cffff0000I can't process that!", fromPlayer);
        return;
    }

    Item* reagent = itemList.back();
    itemList.pop_back();

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return;

    if (reagent)
    {
        SpellCastTargets targets;
        m_itemTarget = reagent->GetProto()->ItemId;
        targets.setItemTarget(reagent);
        Spell* spell = new Spell(m_bot, spellInfo, false);
        spell->SpellStart(&targets);
    }
}

void PlayerbotAI::_HandleCommandUse(std::string& text, Player& fromPlayer)
{
    std::list<uint32> itemIds;
    std::list<Item*> itemList;
    extractItemIds(text, itemIds);
    findItemsInInv(itemIds, itemList);

    if (itemList.empty())
    {
        SendWhisper("|cffff0000I can't use that!", fromPlayer);
        return;
    }

    Item* tool = itemList.back();
    itemList.pop_back();
    if (tool)
    {
        // set target
        Unit* unit = ObjectAccessor::GetUnit(*m_bot, fromPlayer.GetSelectionGuid());
        findItemsInEquip(itemIds, itemList);
        extractGOinfo(text, m_lootTargets);
        // DEBUG_LOG("tool (%s)",tool->GetProto()->Name1);

        if (!itemList.empty())
        {
            Item* itarget = itemList.back();
            if (itarget)
            {
                // DEBUG_LOG("target (%s)",itarget->GetProto()->Name1);
                UseItem(tool, _findItemSlot(itarget)); // on equipped item
                SetState(BOTSTATE_DELAYED);
                SetIgnoreUpdateTime(1);
            }
        }
        else if (!m_lootTargets.empty())
        {
            ObjectGuid gotarget = m_lootTargets.front();
            m_lootTargets.pop_front();

            GameObject* go = m_bot->GetMap()->GetGameObject(gotarget);
            if (go)
                // DEBUG_LOG("tool (%s) on target gameobject (%s)",tool->GetProto()->Name1,go->GetGOInfo()->name);
                UseItem(tool, TARGET_FLAG_GAMEOBJECT, gotarget);  // on gameobject
        }
        else if (unit)
            // DEBUG_LOG("tool (%s) on selected target unit",tool->GetProto()->Name1);
            UseItem(tool, unit);  // on unit
        else
            // DEBUG_LOG("tool (%s) on self",tool->GetProto()->Name1);
            UseItem(tool);  // on self
    }
    return;
}

void PlayerbotAI::_HandleCommandEquip(std::string& text, Player& fromPlayer)
{
    if (ExtractCommand("auto", text))
    {
        bool bOnce = false;
        bool bWasToggleOn = m_AutoEquipToggle;

        // run autoequip cycle once - right now - turning off after
        if (ExtractCommand("once", text))
        {
            bWasToggleOn = m_AutoEquipToggle;
            m_AutoEquipToggle = true;
            bOnce = true;
        }
        else if (ExtractCommand("on", text))
            m_AutoEquipToggle = true;
        else if (ExtractCommand("off", text))
            m_AutoEquipToggle = false;
        else // subcommand not found, assume toggle
            m_AutoEquipToggle = !m_AutoEquipToggle;

        CharacterDatabase.DirectPExecute("UPDATE playerbot_saved_data SET autoequip = '%u' WHERE guid = '%u'", m_AutoEquipToggle, m_bot->GetGUIDLow());

        if (m_AutoEquipToggle)
            AutoUpgradeEquipment();

        // feedback
        if (bOnce)
        {
            if (bWasToggleOn)
                SendWhisper("Equip Auto has run once, switching it off.", fromPlayer);
            else
                SendWhisper("Running Equip Auto once.", fromPlayer);

            m_AutoEquipToggle = false;
        }
        else if (m_AutoEquipToggle)
        {
            SendWhisper("Equip Auto has run and is |h|cff1eff00ON|h|r", fromPlayer);
        }
        else
            SendWhisper("Equip Auto is |h|cffff0000OFF|h|r", fromPlayer);
        return;
    }
    else if (ExtractCommand("info", text))
    {
        m_AutoEquipToggle ?  SendWhisper("Auto Equip is |h|cff1eff00ON|h|r", fromPlayer) : SendWhisper("Auto Equip is |h|cffff0000OFF|h|r", fromPlayer);
        return;
    }

    // handle 'equip' command, expected in the form of 'equip [ITEM(s)]'
    std::list<uint32> itemIds;
    std::list<Item*> itemList;
    extractItemIds(text, itemIds);
    findItemsInInv(itemIds, itemList);
    for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
        EquipItem(*it);
    InspectUpdate();
    SendNotEquipList(*m_bot);
}

void PlayerbotAI::_HandleCommandFind(std::string& text, Player& fromPlayer)
{
    m_lootTargets.clear();
    extractGOinfo(text, m_lootTargets);

    if (m_lootTargets.empty())
    {
        SendWhisper("|cffff0000I can't find that!", fromPlayer);
        return;
    }

    m_lootCurrent = m_lootTargets.front();
    m_lootTargets.pop_front();

    GameObject* go = m_bot->GetMap()->GetGameObject(m_lootCurrent);
    if (!go)
    {
        m_lootTargets.clear();
        m_lootCurrent = ObjectGuid();
        return;
    }

    SetMovementOrder(MOVEMENT_STAY);
    m_bot->GetMotionMaster()->MovePoint(go->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), FORCED_MOVEMENT_RUN);
    m_lootTargets.clear();
    m_lootCurrent = ObjectGuid();
}

void PlayerbotAI::_HandleCommandGet(std::string& text, Player& fromPlayer)
{
    if (!text.empty())
    {
        extractGOinfo(text, m_lootTargets);
        SetState(BOTSTATE_LOOTING);
        return;
    }

    // get a selected lootable corpse
    ObjectGuid getOnGuid = fromPlayer.GetSelectionGuid();
    if (getOnGuid)
    {
        Creature* c = m_bot->GetMap()->GetCreature(getOnGuid);
        if (!c)
            return;

        uint32 skillId = 0;
        if (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            skillId = c->GetCreatureInfo()->GetRequiredLootSkill();

        if (c->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE) ||
                (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE) && m_bot->HasSkill(skillId)))
        {
            m_lootTargets.push_back(getOnGuid);
            SetState(BOTSTATE_LOOTING);
        }
        else
            SendWhisper("Target is not lootable by me.", fromPlayer);
    }
    else
    {
        SendWhisper("No target is selected.", fromPlayer);
        m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    }
}

void PlayerbotAI::_HandleCommandCollect(std::string& text, Player& fromPlayer)
{
    while (text.size() > 0)
    {
        if (ExtractCommand("combat", text))
            SetCollectFlag(COLLECT_FLAG_COMBAT);
        else if (ExtractCommand("loot", text))
            SetCollectFlag(COLLECT_FLAG_LOOT);
        else if (ExtractCommand("quest", text))
            SetCollectFlag(COLLECT_FLAG_QUEST);
        else if (ExtractCommand("profession", text) || ExtractCommand("skill", text))
            SetCollectFlag(COLLECT_FLAG_PROFESSION);
        else if (ExtractCommand("skin", text) && m_bot->HasSkill(SKILL_SKINNING)) // removes skin even if bot does not have skill
            SetCollectFlag(COLLECT_FLAG_SKIN);
        else if (ExtractCommand("objects", text) || ExtractCommand("nearby", text))
        {
            SetCollectFlag(COLLECT_FLAG_NEAROBJECT);
            if (!HasCollectFlag(COLLECT_FLAG_NEAROBJECT))
                m_collectObjects.clear();
        }
        else if (ExtractCommand("distance:", text))
        {
            uint32 distance;
            sscanf(text.c_str(), "distance:%u", &distance);
            if (distance > 0 && distance <= m_mgr.m_confCollectDistanceMax)
            {
                m_collectDist = distance;
                std::ostringstream oss;
                oss << "I will now collect items within " << m_collectDist << " yards.";
                SendWhisper(oss.str(), fromPlayer);
            }
            else
            {
                m_collectDist = m_mgr.m_confCollectDistanceMax;
                std::stringstream oss;
                oss << "I will now collect items within " << m_mgr.m_confCollectDistanceMax << " yards. " << distance << " yards is just too far away.",
                    SendWhisper(oss.str(), fromPlayer);
            }
        }
        else if (ExtractCommand("none", text) || ExtractCommand("nothing", text))
        {
            m_collectionFlags = 0;
            m_collectObjects.clear();
            break;  // because none is an exclusive choice
        }
        else
        {
            std::ostringstream oss;
            oss << "Collect <collectable(s)>: none | distance:<1-" << m_mgr.m_confCollectDistanceMax << ">, combat, loot, quest, profession, objects";
            if (m_bot->HasSkill(SKILL_SKINNING))
                oss << ", skin";
            // TODO: perhaps change the command syntax, this way may be lacking in ease of use
            SendWhisper(oss.str(), fromPlayer);
            break;
        }
    }

    std::string collset = "";
    if (HasCollectFlag(COLLECT_FLAG_LOOT))
        collset += ", all loot";
    if (HasCollectFlag(COLLECT_FLAG_PROFESSION))
        collset += ", profession";
    if (HasCollectFlag(COLLECT_FLAG_QUEST))
        collset += ", quest";
    if (HasCollectFlag(COLLECT_FLAG_SKIN))
        collset += ", skin";
    if (collset.length() > 1)
    {
        if (HasCollectFlag(COLLECT_FLAG_COMBAT))
            collset += " items after combat";
        else
            collset += " items";
    }

    if (HasCollectFlag(COLLECT_FLAG_NEAROBJECT))
    {
        if (collset.length() > 1)
            collset += " and ";
        else
            collset += " ";    // padding for substr
        collset += "nearby objects (";
        if (!m_collectObjects.empty())
        {
            std::string strobjects = "";
            for (BotEntryList::iterator itr = m_collectObjects.begin(); itr != m_collectObjects.end(); ++itr)
            {
                uint32 objectentry = *(itr);
                GameObjectInfo const* gInfo = ObjectMgr::GetGameObjectInfo(objectentry);
                strobjects += ", ";
                strobjects += gInfo->name;
            }
            collset += strobjects.substr(2);
        }
        else
            collset += "use survey and get to set";
        collset += ")";
    }

    if (collset.length() > 1)
        SendWhisper("I'm collecting " + collset.substr(2), fromPlayer);
    else
        SendWhisper("I'm collecting nothing.", fromPlayer);
}

void PlayerbotAI::_HandleCommandEnchant(std::string& text, Player& fromPlayer)
{
    // DEBUG_LOG("Enchant (%s)",text.c_str());

    if (!m_bot->HasSkill(SKILL_ENCHANTING))
    {
        SendWhisper("|cffff0000I can't enchant, I don't have the skill.", fromPlayer);
        return;
    }

    if (text.size() > 0)
    {
        uint32 spellId;
        extractSpellId(text, spellId);

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!spellInfo)
            return;

        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInEquip(itemIds, itemList);
        findItemsInInv(itemIds, itemList);

        if (itemList.empty())
        {
            SendWhisper("|cffff0000I can't enchant that!", fromPlayer);
            return;
        }

        Item* iTarget = itemList.back();
        itemList.pop_back();

        if (iTarget)
        {
            SpellCastTargets targets;
            targets.setItemTarget(iTarget);
            Spell* spell = new Spell(m_bot, spellInfo, false);
            spell->SpellStart(&targets);
            SetState(BOTSTATE_DELAYED);
            SetIgnoreUpdateTime(1);
        }
        return;
    }
    else
    {
        std::ostringstream msg;
        uint32 charges;
        uint32 linkcount = 0;

        m_spellsToLearn.clear();
        m_bot->skill(m_spellsToLearn);
        SendWhisper("I can enchant:\n", fromPlayer);
        ChatHandler ch(&fromPlayer);
        for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
        {
            SkillLineEntry const* SkillLine = sSkillLineStore.LookupEntry(*it);

            if (SkillLine->categoryId == SKILL_CATEGORY_PROFESSION && *it == SKILL_ENCHANTING)
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const* SkillAbility = sSkillLineAbilityStore.LookupEntry(j);
                    if (!SkillAbility)
                        continue;

                    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SkillAbility->spellId);
                    if (!spellInfo)
                        continue;

                    if (IsPrimaryProfessionSkill(*it) && spellInfo->Effect[EFFECT_INDEX_0] != SPELL_EFFECT_ENCHANT_ITEM)
                        continue;

                    if (SkillAbility->skillId == *it && m_bot->HasSpell(SkillAbility->spellId) && SkillAbility->forward_spellid == 0 && ((SkillAbility->classmask & m_bot->getClassMask()) == 0))
                    {
                        MakeSpellLink(spellInfo, msg);
                        ++linkcount;
                        if ((charges = GetSpellCharges(SkillAbility->spellId)) > 0)
                            msg << "[" << charges << "]";
                        if (linkcount >= 10)
                        {
                            ch.SendSysMessage(msg.str().c_str());
                            linkcount = 0;
                            msg.str("");
                        }
                    }
                }
        }
        m_noToolList.unique();
        for (std::list<uint32>::iterator it = m_noToolList.begin(); it != m_noToolList.end(); it++)
            HasTool(*it);
        ch.SendSysMessage(msg.str().c_str());
        m_noToolList.clear();
        m_spellsToLearn.clear();
    }
}

void PlayerbotAI::_HandleCommandCraft(std::string& text, Player& fromPlayer)
{
    DEBUG_LOG("Craft (%s)", text.c_str());

    std::ostringstream msg;
    uint32 charges;
    uint32 skill;
    int32 category;
    uint32 linkcount = 0;
    m_CraftSpellId = 0;

    if (ExtractCommand("alchemy", text, true)) // true -> "craft alchemy" OR "craft a"
    {
        if (m_bot->HasSkill(SKILL_ALCHEMY))
        {
            skill = SKILL_ALCHEMY;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("blacksmithing", text, true)) // true -> "craft blacksmithing" OR "craft b"
    {
        if (m_bot->HasSkill(SKILL_BLACKSMITHING))
        {
            skill = SKILL_BLACKSMITHING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("cooking", text, true)) // true -> "craft cooking" OR "craft c"
    {
        if (m_bot->HasSkill(SKILL_COOKING))
        {
            skill = SKILL_COOKING;
            category = SKILL_CATEGORY_SECONDARY;
        }
        else
            return;
    }
    else if (ExtractCommand("engineering", text, true)) // true -> "craft engineering" OR "craft e"
    {
        if (m_bot->HasSkill(SKILL_ENGINEERING))
        {
            skill = SKILL_ENGINEERING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("firstaid", text, true)) // true -> "craft firstaid" OR "craft f"
    {
        if (m_bot->HasSkill(SKILL_FIRST_AID))
        {
            skill = SKILL_FIRST_AID;
            category = SKILL_CATEGORY_SECONDARY;
        }
        else
            return;
    }
    else if (ExtractCommand("inscription", text, true)) // true -> "craft inscription" OR "craft i"
    {
        if (m_bot->HasSkill(SKILL_INSCRIPTION))
        {
            skill = SKILL_INSCRIPTION;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("jewelcrafting", text, true)) // true -> "craft jewelcrafting" OR "craft j"
    {
        if (m_bot->HasSkill(SKILL_JEWELCRAFTING))
        {
            skill = SKILL_JEWELCRAFTING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("leatherworking", text, true)) // true -> "craft leatherworking" OR "craft l"
    {
        if (m_bot->HasSkill(SKILL_LEATHERWORKING))
        {
            skill = SKILL_LEATHERWORKING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("magic", text, true)) // true -> "craft magic" OR "craft m"
    {
        if (m_bot->HasSkill(SKILL_ENCHANTING))
        {
            skill = SKILL_ENCHANTING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("smelting", text, true)) // true -> "craft smelting" OR "craft s"
    {
        if (m_bot->HasSkill(SKILL_MINING))
        {
            skill = SKILL_MINING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else if (ExtractCommand("tailoring", text, true)) // true -> "craft tailoring" OR "craft t"
    {
        if (m_bot->HasSkill(SKILL_TAILORING))
        {
            skill = SKILL_TAILORING;
            category = SKILL_CATEGORY_PROFESSION;
        }
        else
            return;
    }
    else
    {
        uint32 spellId;
        extractSpellId(text, spellId);

        if (!m_bot->HasSpell(spellId))
        {
            SendWhisper("|cffff0000I don't have that spell.", fromPlayer);
            return;
        }

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!spellInfo)
            return;

        SpellCastTargets targets;
        Spell* spell = new Spell(m_bot, spellInfo, false);

        if (text.find("all", 0) != std::string::npos)
        {
            SpellCastResult result = spell->CheckCast(true);

            if (result != SPELL_CAST_OK)
            {
                spell->SendCastResult(result);
            }
            else
            {
                spell->SpellStart(&targets);
                m_CraftSpellId = spellId;
                SetState(BOTSTATE_DELAYED);
                SetIgnoreUpdateTime(6);
            }
        }
        else
            spell->SpellStart(&targets);
        return;
    }

    m_spellsToLearn.clear();
    m_bot->skill(m_spellsToLearn);
    SendWhisper("I can create:\n", fromPlayer);
    ChatHandler ch(&fromPlayer);
    for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
    {
        SkillLineEntry const* SkillLine = sSkillLineStore.LookupEntry(*it);

        if (SkillLine->categoryId == category && *it == skill)
            for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
            {
                SkillLineAbilityEntry const* SkillAbility = sSkillLineAbilityStore.LookupEntry(j);
                if (!SkillAbility)
                    continue;

                SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SkillAbility->spellId);
                if (!spellInfo)
                    continue;

                if (IsPrimaryProfessionSkill(*it) && spellInfo->Effect[EFFECT_INDEX_0] != SPELL_EFFECT_CREATE_ITEM)
                    continue;

                if (SkillAbility->skillId == *it && m_bot->HasSpell(SkillAbility->spellId) && SkillAbility->forward_spellid == 0 && ((SkillAbility->classmask & m_bot->getClassMask()) == 0))
                {
                    MakeSpellLink(spellInfo, msg);
                    ++linkcount;
                    if ((charges = GetSpellCharges(SkillAbility->spellId)) > 0)
                        msg << "[" << charges << "]";
                    if (linkcount >= 10)
                    {
                        ch.SendSysMessage(msg.str().c_str());
                        linkcount = 0;
                        msg.str("");
                    }
                }
            }
    }
    m_noToolList.unique();
    for (std::list<uint32>::iterator it = m_noToolList.begin(); it != m_noToolList.end(); it++)
        HasTool(*it);
    ch.SendSysMessage(msg.str().c_str());
    m_noToolList.clear();
    m_spellsToLearn.clear();
}

void PlayerbotAI::_HandleCommandQuest(std::string& text, Player& fromPlayer)
{
    std::ostringstream msg;

    if (ExtractCommand("add", text, true)) // true -> "quest add" OR "quest a"
    {
        FollowAutoReset();
        std::list<uint32> questIds;
        extractQuestIds(text, questIds);
        for (std::list<uint32>::iterator it = questIds.begin(); it != questIds.end(); it++)
            m_tasks.push_back(std::pair<enum TaskFlags, uint32>(TAKE_QUEST, *it));
        m_findNPC.push_back(UNIT_NPC_FLAG_QUESTGIVER);
    }
    else if (ExtractCommand("drop", text, true)) // true -> "quest drop" OR "quest d"
    {
        fromPlayer.SetSelectionGuid(m_bot->GetObjectGuid());
        PlayerbotChatHandler ch(GetMaster());
        int8 linkStart = text.find("|");
        if (text.find("|") != std::string::npos)
        {
            if (!ch.dropQuest((char*) text.substr(linkStart).c_str()))
            {
                ch.sysmessage("ERROR: could not drop quest");
            }
            else
            {
                SetQuestNeedItems();
                SetQuestNeedCreatures();
            }
        }
    }
    else if (ExtractCommand("fetch", text, true)) // true -> "quest fetch"
    {
        FollowAutoReset();
        gQuestFetch = 1;
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(LIST_QUEST, 0));
        m_findNPC.push_back(UNIT_NPC_FLAG_QUESTGIVER);
    }
    else if (ExtractCommand("list", text, true)) // true -> "quest list" OR "quest l"
    {
        FollowAutoReset();
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(LIST_QUEST, 0));
        m_findNPC.push_back(UNIT_NPC_FLAG_QUESTGIVER);
    }
    else if (ExtractCommand("report", text))
        SendQuestNeedList();
    else if (ExtractCommand("end", text, true)) // true -> "quest end" OR "quest e"
    {
        FollowAutoReset();
        m_tasks.push_back(std::pair<enum TaskFlags, uint32>(END_QUEST, 0));
        m_findNPC.push_back(UNIT_NPC_FLAG_QUESTGIVER);
    }
    else if (ExtractCommand("complete", text))
    {
        ChatHandler ch(&fromPlayer);

        std::list<uint32> questIds;
        extractQuestIds(text, questIds);

        for (std::list<uint32>::iterator it = questIds.begin(); it != questIds.end(); it++)
        {
            uint32 entry = *it;

            Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);
            if (!pQuest)
                continue;

            // Compare quest entry from [Quest Link] with quest ids listed DB table
            // if found the quest is autocompleted
            QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM playerbot_quest_data WHERE autocomplete='%u'", entry);
            if (!result)
            {
                MakeQuestLink(pQuest, msg);
                msg << " can't be autocompleted\n";
                continue;
            }
            else
            {
                Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
                for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
                {
                    Player* groupMember = sObjectMgr.GetPlayer(itr->guid);

                    // If bot doesn't have the quest
                    if (groupMember->GetQuestStatus(entry) == QUEST_STATUS_NONE)
                    {
                        msg << "|cFFD569CE[|r" << groupMember->GetName() << "|cFFD569CE]|r doesn't have that quest\n";
                        continue;
                    }

                    // Add quest items for quests that require items
                    for (uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x)
                    {
                        uint32 id = pQuest->ReqItemId[x];
                        uint32 count = pQuest->ReqItemCount[x];
                        if (!id || !count)
                            continue;

                        uint32 curItemCount = groupMember->GetItemCount(id, true);

                        ItemPosCountVec dest;
                        uint8 res = groupMember->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
                        if (res == EQUIP_ERR_OK)
                        {
                            Item* item = groupMember->StoreNewItem(dest, id, true);
                            groupMember->SendNewItem(item, count - curItemCount, true, false);
                        }
                    }

                    // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
                    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
                    {
                        int32 creature = pQuest->ReqCreatureOrGOId[i];
                        uint32 creaturecount = pQuest->ReqCreatureOrGOCount[i];

                        if (uint32 spell_id = pQuest->ReqSpell[i])
                        {
                            for (uint16 z = 0; z < creaturecount; ++z)
                                groupMember->CastedCreatureOrGO(creature, ObjectGuid(), spell_id);
                        }
                        else if (creature > 0)
                        {
                            if (CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(creature))
                                for (uint16 z = 0; z < creaturecount; ++z)
                                    groupMember->KilledMonster(cInfo, nullptr);
                        }
                        else if (creature < 0)
                        {
                            for (uint16 z = 0; z < creaturecount; ++z)
                                groupMember->CastedCreatureOrGO(-creature, ObjectGuid(), 0);
                        }
                    }

                    // If the quest requires reputation to complete
                    if (uint32 repFaction = pQuest->GetRepObjectiveFaction())
                    {
                        uint32 repValue = pQuest->GetRepObjectiveValue();
                        uint32 curRep = groupMember->GetReputationMgr().GetReputation(repFaction);
                        if (curRep < repValue)
                            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
                                groupMember->GetReputationMgr().SetReputation(factionEntry, repValue);
                    }

                    // If the quest requires money
                    int32 ReqOrRewMoney = pQuest->GetRewOrReqMoney();
                    if (ReqOrRewMoney < 0)
                        groupMember->ModifyMoney(-ReqOrRewMoney);

                    groupMember->CompleteQuest(entry);
                    msg << "|cFFD569CE[|r" << groupMember->GetName() << "|cFFD569CE]|r completed quest ";
                    MakeQuestLink(pQuest, msg);
                    msg << "\n";
                }
                delete result;
            }
        }
        ch.SendSysMessage(msg.str().c_str());
    }
    else
    {
        bool hasIncompleteQuests = false;
        std::ostringstream incomout;
        incomout << "my incomplete quests are:";
        bool hasCompleteQuests = false;
        std::ostringstream comout;
        comout << "my complete quests are:";
        for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            if (uint32 questId = m_bot->GetQuestSlotQuestId(slot))
            {
                Quest const* pQuest = sObjectMgr.GetQuestTemplate(questId);

                if (m_bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
                {
                    hasCompleteQuests = true;
                    MakeQuestLink(pQuest, comout);
                }
                else
                {
                    Item* qitem = FindItem(pQuest->GetSrcItemId());
                    if (qitem)
                    {
                        incomout << " use ";
                        MakeItemLink(qitem->GetProto(), incomout);
                        incomout << " on ";
                    }
                    hasIncompleteQuests = true;
                    MakeQuestLink(pQuest, incomout);
                }
            }
        }
        if (hasCompleteQuests)
            SendWhisper(comout.str(), fromPlayer);
        if (hasIncompleteQuests)
            SendWhisper(incomout.str(), fromPlayer);
        if (!hasCompleteQuests && !hasIncompleteQuests)
            SendWhisper("I have no quests.", fromPlayer);
    }
}

void PlayerbotAI::_HandleCommandPet(std::string& text, Player& fromPlayer)
{
    if (ExtractCommand("tame", text))
    {
        if (m_bot->GetPetGuid())
        {
            SendWhisper("I already have a pet!", fromPlayer);
            return;
        }

        ObjectGuid castOnGuid = fromPlayer.GetSelectionGuid();
        if (castOnGuid && m_bot->HasSpell(TAME_BEAST_1))
        {
            if (ASPECT_OF_THE_MONKEY > 0 && !m_bot->HasAura(ASPECT_OF_THE_MONKEY, EFFECT_INDEX_0))
                CastSpell(ASPECT_OF_THE_MONKEY, *m_bot);
            m_targetGuidCommand = castOnGuid;
            SetState(BOTSTATE_TAME);
        }
        else
            SendWhisper("I can't tame that!", fromPlayer);
        return;
    }

    Pet* pet = m_bot->GetPet();
    if (!pet)
    {
        SendWhisper("I have no pet.", fromPlayer);
        return;
    }

    if (ExtractCommand("abandon", text))
    {
        // abandon pet
        std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_PET_ABANDON, 8));
        *packet << pet->GetObjectGuid();
        m_bot->GetSession()->QueuePacket(std::move(packet));

    }
    else if (ExtractCommand("react", text))
    {
        if (ExtractCommand("aggressive", text, true))
            pet->AI()->SetReactState(REACT_AGGRESSIVE);
        else if (ExtractCommand("defensive", text, true))
            pet->AI()->SetReactState(REACT_DEFENSIVE);
        else if (ExtractCommand("passive", text, true))
            pet->AI()->SetReactState(REACT_PASSIVE);
        else
            _HandleCommandHelp("pet react", fromPlayer);
    }
    else if (ExtractCommand("state", text))
    {
        if (!text.empty())
        {
            SendWhisper("'pet state' does not support subcommands.", fromPlayer);
            return;
        }

        if(!pet || !pet->AI())
            return;

        switch (pet->AI()->GetReactState())
        {
            case REACT_AGGRESSIVE:
                SendWhisper("My pet is aggressive.", fromPlayer);
                break;
            case REACT_DEFENSIVE:
                SendWhisper("My pet is defensive.", fromPlayer);
                break;
            case REACT_PASSIVE:
                SendWhisper("My pet is passive.", fromPlayer);
        }
    }
    else if (ExtractCommand("cast", text))
    {
        if (text.empty())
        {
            _HandleCommandHelp("pet cast", fromPlayer);
            return;
        }

        if(!pet)
            return;

        uint32 spellId = (uint32) atol(text.c_str());

        if (spellId == 0)
        {
            spellId = getPetSpellId(text.c_str());
            if (spellId == 0)
                extractSpellId(text, spellId);
        }

        if (spellId != 0 && pet->HasSpell(spellId))
        {
            if (pet->HasAura(spellId))
            {
                pet->RemoveAurasByCasterSpell(spellId, pet->GetObjectGuid());
                return;
            }

            ObjectGuid castOnGuid = fromPlayer.GetSelectionGuid();
            Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, castOnGuid);
            CastPetSpell(spellId, pTarget);
        }
    }
    else if (ExtractCommand("toggle", text))
    {
        if (text.empty())
        {
            _HandleCommandHelp("pet toggle", fromPlayer);
            return;
        }

        if(!pet)
            return;

        uint32 spellId = (uint32) atol(text.c_str());

        if (spellId == 0)
        {
            spellId = getPetSpellId(text.c_str());
            if (spellId == 0)
                extractSpellId(text, spellId);
        }

        if (spellId != 0 && pet->HasSpell(spellId))
        {
            PetSpellMap::iterator itr = pet->m_spells.find(spellId);
            if (itr != pet->m_spells.end())
            {
                if (itr->second.active == ACT_ENABLED)
                {
                    pet->ToggleAutocast(spellId, false);
                    if (pet->HasAura(spellId))
                        pet->RemoveAurasByCasterSpell(spellId, pet->GetObjectGuid());
                }
                else
                    pet->ToggleAutocast(spellId, true);
            }
        }
    }
    else if (ExtractCommand("spells", text))
    {
        if (!text.empty())
        {
            SendWhisper("'pet spells' does not support subcommands.", fromPlayer);
            return;
        }

        if(!pet)
            return;

        int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

        std::ostringstream posOut;
        std::ostringstream negOut;

        for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
        {
            const uint32 spellId = itr->first;

            if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
                continue;

            const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
            if (!pSpellInfo)
                continue;

            std::string color;
            switch (itr->second.active)
            {
                case ACT_ENABLED:
                    color = "cff35d22d"; // Some flavor of green
                    break;
                default:
                    color = "cffffffff";
            }

            if (IsPositiveSpell(spellId))
                posOut << " |" << color << "|Hspell:" << spellId << "|h["
                       << pSpellInfo->SpellName[loc] << "]|h|r";
            else
                negOut << " |" << color << "|Hspell:" << spellId << "|h["
                       << pSpellInfo->SpellName[loc] << "]|h|r";
        }

        ChatHandler ch(&fromPlayer);
        SendWhisper("Here's my pet's non-attack spells:", fromPlayer);
        ch.SendSysMessage(posOut.str().c_str());
        SendWhisper("and here's my pet's attack spells:", fromPlayer);
        ch.SendSysMessage(negOut.str().c_str());
    }
}

void PlayerbotAI::_HandleCommandSpells(std::string& /*text*/, Player& fromPlayer)
{
    int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

    std::ostringstream posOut;
    std::ostringstream negOut;

    typedef std::map<std::string, uint32> spellMap;

    spellMap posSpells, negSpells;
    std::string spellName;

    uint32 ignoredSpells[] = {1843, 5019, 2479, 6603, 3365, 8386, 21651, 21652, 6233, 6246, 6247,
                              61437, 22810, 22027, 45927, 7266, 7267, 6477, 6478, 7355, 68398
                             };
    uint32 ignoredSpellsCount = sizeof(ignoredSpells) / sizeof(uint32);

    for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
    {
        const uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* const pSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
        if (!pSpellInfo)
            continue;

        spellName = pSpellInfo->SpellName[loc];

        SkillLineAbilityMapBounds const bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySpellId(spellId);

        bool isProfessionOrRidingSpell = false;
        for (SkillLineAbilityMap::const_iterator skillIter = bounds.first; skillIter != bounds.second; ++skillIter)
        {
            if (IsProfessionOrRidingSkill(skillIter->second->skillId) && skillIter->first == spellId)
            {
                isProfessionOrRidingSpell = true;
                break;
            }
        }
        if (isProfessionOrRidingSpell)
            continue;

        bool isIgnoredSpell = false;
        for (uint8 i = 0; i < ignoredSpellsCount; ++i)
        {
            if (spellId == ignoredSpells[i])
            {
                isIgnoredSpell = true;
                break;
            }
        }
        if (isIgnoredSpell)
            continue;

        if (IsPositiveSpell(spellId))
        {
            if (posSpells.find(spellName) == posSpells.end())
                posSpells[spellName] = spellId;
            else if (posSpells[spellName] < spellId)
                posSpells[spellName] = spellId;
        }
        else
        {
            if (negSpells.find(spellName) == negSpells.end())
                negSpells[spellName] = spellId;
            else if (negSpells[spellName] < spellId)
                negSpells[spellName] = spellId;
        }
    }

    for (spellMap::const_iterator iter = posSpells.begin(); iter != posSpells.end(); ++iter)
    {
        posOut << " |cffffffff|Hspell:" << iter->second << "|h[" << iter->first << "]|h|r";
    }

    for (spellMap::const_iterator iter = negSpells.begin(); iter != negSpells.end(); ++iter)
    {
        negOut << " |cffffffff|Hspell:" << iter->second << "|h[" << iter->first << "]|h|r";
    }

    ChatHandler ch(&fromPlayer);
    SendWhisper("here's my non-attack spells:", fromPlayer);
    ch.SendSysMessage(posOut.str().c_str());
    SendWhisper("and here's my attack spells:", fromPlayer);
    ch.SendSysMessage(negOut.str().c_str());
}

void PlayerbotAI::_HandleCommandSurvey(std::string& /*text*/, Player& fromPlayer)
{
    uint32 count = 0;
    std::ostringstream detectout;
    QueryResult* result;
    GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr.GetActiveEventList();
    std::ostringstream eventFilter;
    eventFilter << " AND (event IS NULL ";
    bool initString = true;

    for (GameEventMgr::ActiveEvents::const_iterator itr = activeEventsList.begin(); itr != activeEventsList.end(); ++itr)
    {
        if (initString)
        {
            eventFilter <<  "OR event IN (" << *itr;
            initString = false;
        }
        else
            eventFilter << "," << *itr;
    }

    if (!initString)
        eventFilter << "))";
    else
        eventFilter << ")";

    result = WorldDatabase.PQuery("SELECT gameobject.guid, id, position_x, position_y, position_z, map, "
                                  "(POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ FROM gameobject "
                                  "LEFT OUTER JOIN game_event_gameobject on gameobject.guid=game_event_gameobject.guid WHERE map = '%i' %s ORDER BY order_ ASC LIMIT 10",
                                  m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), m_bot->GetMapId(), eventFilter.str().c_str());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();

            GameObject* go = m_bot->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, entry, guid));
            if (!go)
                continue;

            if (!go->IsSpawned())
                continue;

            detectout << "|cFFFFFF00|Hfound:" << guid << ":" << entry  << ":" <<  "|h[" << go->GetGOInfo()->name << "]|h|r";
            ++count;
        }
        while (result->NextRow());

        delete result;
    }
    SendWhisper(detectout.str().c_str(), fromPlayer);
}

// _HandleCommandSkill: Handle class & professions training:
// skill                           -- Lists bot(s) Primary profession skills & weapon skills
// skill learn                     -- List available class or profession (Primary or Secondary) skills, spells & abilities from selected trainer.
// skill learn all                 -- Learn all skills and spells available from selected trainer.
// skill learn [HLINK][HLINK] ..   -- Learn selected skill and spells, from selected trainer ([HLINK] from skill learn).
// skill unlearn [HLINK][HLINK] .. -- Unlearn selected primary profession skill(s) and all associated spells ([HLINK] from skill)
void PlayerbotAI::_HandleCommandSkill(std::string& text, Player& fromPlayer)
{
    uint32 rank[8] = {0, 75, 150, 225, 300, 375, 450, 525};

    std::ostringstream msg;
    if (ExtractCommand("learn", text))
    {
        uint32 totalCost = 0;

        Unit* unit = ObjectAccessor::GetUnit(*m_bot, fromPlayer.GetSelectionGuid());
        if (!unit)
        {
            SendWhisper("Please select the trainer!", fromPlayer);
            return;
        }

        if (!unit->isTrainer())
        {
            SendWhisper("This is not a trainer!", fromPlayer);
            return;
        }

        Creature* creature =  m_bot->GetMap()->GetCreature(fromPlayer.GetSelectionGuid());
        if (!creature)
            return;

        if (!creature->IsTrainerOf(m_bot, false))
        {
            SendWhisper("This trainer can not teach me anything!", fromPlayer);
            return;
        }

        // check present spell in trainer spell list
        TrainerSpellData const* cSpells = creature->GetTrainerSpells();
        TrainerSpellData const* tSpells = creature->GetTrainerTemplateSpells();
        TrainerSpellMap allSpells;

        if (cSpells && tSpells)
        {
            allSpells.insert(cSpells->spellList.begin(), cSpells->spellList.end());
            allSpells.insert(tSpells->spellList.begin(), tSpells->spellList.end());
        }
        else if (cSpells)
            allSpells.insert(cSpells->spellList.begin(), cSpells->spellList.end());
        else if (tSpells)
            allSpells.insert(tSpells->spellList.begin(), tSpells->spellList.end());
        else
        {
            SendWhisper("No spells can be learnt from this trainer", fromPlayer);
            return;
        }

        // reputation discount
        float fDiscountMod =  m_bot->GetReputationPriceDiscount(creature);

        // Handle: Learning class or profession (primary or secondary) skill & spell(s) for selected trainer, skill learn [HLINK][HLINK][HLINK].. ([HLINK] from skill train)
        if (text.size() > 0)
        {
            msg << "I have learned the following spells:\n";
            uint32 totalSpellLearnt = 0;
            bool visuals = true;
            m_spellsToLearn.clear();
            if (ExtractCommand("all", text))
            {
                for (TrainerSpellMap::const_iterator itr =  allSpells.begin(); itr !=  allSpells.end(); ++itr)
                {
                    TrainerSpell const* trainer_spell = &itr->second;

                    uint32 spellId = trainer_spell->spell;

                    if (!spellId)
                        break;

                    // apply reputation discount
                    uint32 cost = uint32(floor(trainer_spell->spellCost * fDiscountMod));
                    if (!_HandleCommandSkillLearnHelper(trainer_spell, spellId, cost))
                        continue;

                    ++totalSpellLearnt;
                    totalCost += cost;
                    const SpellEntry* const pSpellInfo =  sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                    if (!pSpellInfo)
                        continue;

                    if (visuals)
                    {
                        visuals = false;
                        WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);           // visual effect on trainer
                        data << ObjectGuid(fromPlayer.GetSelectionGuid());
                        data << uint32(0xB3);                                   // index from SpellVisualKit.dbc
                        GetMaster()->GetSession()->SendPacket(data);

                        data.Initialize(SMSG_PLAY_SPELL_IMPACT, 12);            // visual effect on player
                        data << m_bot->GetObjectGuid();
                        data << uint32(0x016A);                                 // index from SpellVisualKit.dbc
                        GetMaster()->GetSession()->SendPacket(data);
                    }

                    WorldPacket data(SMSG_TRAINER_BUY_SUCCEEDED, 12);
                    data << ObjectGuid(fromPlayer.GetSelectionGuid());
                    data << uint32(spellId);                                // should be same as in packet from client
                    GetMaster()->GetSession()->SendPacket(data);
                    MakeSpellLink(pSpellInfo, msg);
                    msg << " ";
                    msg << Cash(cost) << " ";
                }
            }
            else
            {
                extractSpellIdList(text, m_spellsToLearn);

                for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); it++)
                {
                    uint32 spellId = *it;

                    if (!spellId)
                        break;

                    // Try find spell in npc_trainer
                    TrainerSpell const* trainer_spell = cSpells ? cSpells->Find(spellId) : nullptr;

                    // Not found, try find in npc_trainer_template
                    if (!trainer_spell && tSpells)
                        trainer_spell = tSpells->Find(spellId);

                    // apply reputation discount
                    uint32 cost = uint32(floor(trainer_spell->spellCost * fDiscountMod));

                    if (!_HandleCommandSkillLearnHelper(trainer_spell, spellId, cost))
                        continue;

                    ++totalSpellLearnt;
                    totalCost += cost;
                    const SpellEntry* const pSpellInfo =  sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                    if (!pSpellInfo)
                        continue;

                    if (visuals)
                    {
                        visuals = false;
                        WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);           // visual effect on trainer
                        data << ObjectGuid(fromPlayer.GetSelectionGuid());
                        data << uint32(0xB3);                                   // index from SpellVisualKit.dbc
                        GetMaster()->GetSession()->SendPacket(data);

                        data.Initialize(SMSG_PLAY_SPELL_IMPACT, 12);            // visual effect on player
                        data << m_bot->GetObjectGuid();
                        data << uint32(0x016A);                                 // index from SpellVisualKit.dbc
                        GetMaster()->GetSession()->SendPacket(data);
                    }

                    WorldPacket data(SMSG_TRAINER_BUY_SUCCEEDED, 12);
                    data << ObjectGuid(fromPlayer.GetSelectionGuid());
                    data << uint32(spellId);                                // should be same as in packet from client
                    GetMaster()->GetSession()->SendPacket(data);
                    MakeSpellLink(pSpellInfo, msg);
                    msg << " ";
                    msg << Cash(cost) << " ";
                }
            }
            ReloadAI();
            if (totalSpellLearnt == 0) msg.clear();
            else msg << "\n";
            msg << "Total of " << totalSpellLearnt << " spell";
            if (totalSpellLearnt != 1) msg << "s";
            msg << " learnt, ";
            msg << Cash(totalCost) << " spent.";
        }
        // Handle: List class or profession skills, spells & abilities for selected trainer
        else
        {
            msg << "The spells I can learn and their cost:\r";

            for (TrainerSpellMap::const_iterator itr =  allSpells.begin(); itr !=  allSpells.end(); ++itr)
            {
                TrainerSpell const* tSpell = &itr->second;

                if (!tSpell)
                    break;

                uint32 reqLevel = 0;
                if (!tSpell->learnedSpell && !m_bot->IsSpellFitByClassAndRace(tSpell->learnedSpell, &reqLevel))
                    continue;

                if (sSpellMgr.IsPrimaryProfessionFirstRankSpell(tSpell->learnedSpell) && m_bot->HasSpell(tSpell->learnedSpell))
                    continue;

                reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);

                TrainerSpellState state =  m_bot->GetTrainerSpellState(tSpell, reqLevel);
                if (state != TRAINER_SPELL_GREEN)
                    continue;

                uint32 spellId = tSpell->spell;
                const SpellEntry* const pSpellInfo =  sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                if (!pSpellInfo)
                    continue;
                uint32 cost = uint32(floor(tSpell->spellCost *  fDiscountMod));
                totalCost += cost;
                MakeSpellLink(pSpellInfo, msg);
                msg << " ";
                msg << Cash(cost) << " ";
            }

            if (totalCost == 0)
            {
                msg.clear();
                msg << "I have learned all I can from this trainer. Perhaps I can learn more once I grow stronger.";
            }
            else
            {
                int32 moneyDiff = m_bot->GetMoney() - totalCost;
                if (moneyDiff >= 0)
                    msg << "\n" << Cash(moneyDiff) << " left after learning all the spells.";
                else
                {
                    Announce(CANT_AFFORD);
                    moneyDiff *= -1;
                    msg << "\nI need " << Cash(moneyDiff) << " more to learn all the spells!";
                }
            }
        }
    }
    // Handle: Unlearning selected primary profession skill(s) and all associated spells, skill unlearn [HLINK][HLINK].. ([HLINK] from skill)
    else if (ExtractCommand("unlearn", text))
    {
        m_spellsToLearn.clear();
        extractSpellIdList(text, m_spellsToLearn);
        for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
        {
            if (sSpellMgr.IsPrimaryProfessionSpell(*it))
            {
                SpellLearnSkillNode const* spellLearnSkill = sSpellMgr.GetSpellLearnSkill(*it);

                uint32 prev_spell = sSpellMgr.GetPrevSpellInChain(*it);
                if (!prev_spell)                                    // first rank, remove skill
                    GetPlayer()->SetSkill(spellLearnSkill->skill, 0, 0);
                else
                {
                    // search prev. skill setting by spell ranks chain
                    SpellLearnSkillNode const* prevSkill = sSpellMgr.GetSpellLearnSkill(prev_spell);
                    while (!prevSkill && prev_spell)
                    {
                        prev_spell = sSpellMgr.GetPrevSpellInChain(prev_spell);
                        prevSkill = sSpellMgr.GetSpellLearnSkill(sSpellMgr.GetFirstSpellInChain(prev_spell));
                    }
                    if (!prevSkill)                                 // not found prev skill setting, remove skill
                        GetPlayer()->SetSkill(spellLearnSkill->skill, 0, 0);
                }
            }
        }
    }
    // Handle: Lists bot(s) primary profession skills & weapon skills.
    else
    {
        m_spellsToLearn.clear();
        m_bot->skill(m_spellsToLearn);
        msg << "My Primary Professions: ";
        for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
        {
            if (IsPrimaryProfessionSkill(*it))
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
                    if (!skillLine)
                        continue;

                    // has skill
                    if (skillLine->skillId == *it && skillLine->learnOnGetSkill == 0)
                    {
                        uint32 SpellId;
                        m_bot->HasSpell(skillLine->forward_spellid) ? SpellId = skillLine->forward_spellid : SpellId = skillLine->spellId;

                        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SpellId);
                        if (!spellInfo)
                            continue;

                        if (m_bot->GetSkillValue(*it) <= rank[sSpellMgr.GetSpellRank(SpellId)] && m_bot->HasSpell(SpellId))
                        {
                            // DEBUG_LOG ("[PlayerbotAI]: HandleCommand - skill (%u)(%u)(%u):",skillLine->spellId, rank[sSpellMgr.GetSpellRank(skillLine->spellId)], m_bot->GetSkillValue(*it));
                            msg << "\n[" << m_bot->GetSkillValue(*it) << " / " << rank[sSpellMgr.GetSpellRank(SpellId)] << "]: ";
                            MakeSpellLink(spellInfo, msg);
                            break;
                        }
                    }
                }
        }

        msg << "\nMy Weapon skills: ";
        for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
        {
            SkillLineEntry const* SkillLine = sSkillLineStore.LookupEntry(*it);
            // has weapon skill
            if (SkillLine->categoryId == SKILL_CATEGORY_WEAPON)
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
                    if (!skillLine)
                        continue;

                    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(skillLine->spellId);
                    if (!spellInfo)
                        continue;

                    if (skillLine->skillId == *it && spellInfo->Effect[0] == SPELL_EFFECT_WEAPON)
                        MakeWeaponSkillLink(spellInfo, msg, *it);
                }
        }
    }
    SendWhisper(msg.str(), fromPlayer);
    m_spellsToLearn.clear();
    m_bot->GetPlayerbotAI()->GetClassAI();
}

bool PlayerbotAI::_HandleCommandSkillLearnHelper(TrainerSpell const* tSpell, uint32 spellId, uint32 cost)
{
    // Not found anywhere, cheating?
    if (!tSpell)
        return false;

    uint32 reqLevel = 0;
    if (!tSpell->learnedSpell && !m_bot->IsSpellFitByClassAndRace(tSpell->learnedSpell, &reqLevel))
        return false;

    if (sSpellMgr.IsPrimaryProfessionFirstRankSpell(tSpell->learnedSpell) && m_bot->HasSpell(tSpell->learnedSpell))
        return false;

    reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);

    TrainerSpellState state =  m_bot->GetTrainerSpellState(tSpell, reqLevel);
    if (state != TRAINER_SPELL_GREEN)
        return false;

    // check money requirement
    if (m_bot->GetMoney() < cost)
    {
        Announce(CANT_AFFORD);
        return false;
    }

    m_bot->ModifyMoney(-int32(cost));
    // learn explicitly
    m_bot->learnSpell(spellId, false);

    return true;
}

void PlayerbotAI::_HandleCommandStats(std::string& text, Player& fromPlayer)
{
    if (!text.empty())
    {
        SendWhisper("'stats' does not have subcommands", fromPlayer);
        return;
    }

    std::ostringstream out;

    uint32 totalfree = GetFreeBagSpace();

    // estimate how much item damage the bot has
    out << "|cffffffff[|h|cff00ffff" << m_bot->GetName() << "|h|cffffffff] has |cff00ff00";
    out << totalfree << " |h|cffffffff bag slots,|h" << " |cff00ff00";
    out << Cash(EstRepairAll());

    // calculate how much money bot has
    uint32 copper = m_bot->GetMoney();
    out << "|h|cffffffff item damage & has " << "|r|cff00ff00";
    out << Cash(copper);
    ChatHandler ch(&fromPlayer);
    ch.SendSysMessage(out.str().c_str());
}

void PlayerbotAI::_HandleCommandGM(std::string& text, Player& fromPlayer)
{
    // Check should happen OUTSIDE this function, but this is account security we're talking about, so let's be doubly sure
    if (fromPlayer.GetSession()->GetSecurity() <= SEC_PLAYER)
        return;  // no excuses, no warning

    if (text.empty())
    {
        SendWhisper("gm must have a subcommand.", fromPlayer);
        return;
    }
    else if (ExtractCommand("quest", text))
    {
        std::ostringstream out;
        ChatHandler ch(&fromPlayer);

        size_t add = text.find("add");
        size_t del = text.find("del");

        std::list<uint32> questIds;
        extractQuestIds(text, questIds);

        for (std::list<uint32>::iterator it = questIds.begin(); it != questIds.end(); it++)
        {

            uint32 entry = *it;

            Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

            if (!pQuest)
            {
                out << "quest [" << entry << "] not found\n";
                continue;
            }

            //check whether entry is already in database
            QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM playerbot_quest_data WHERE autocomplete='%u'", entry);

            if (add != std::string::npos)
            {
                if (!result)
                {
                    // add new entry
                    out << "adding quest ";
                    MakeQuestLink(pQuest, out);
                    out << " to table\n";
                    CharacterDatabase.DirectPExecute("INSERT INTO playerbot_quest_data (autocomplete) VALUES('%u')", entry);
                }
                else
                {
                    // entry found in table
                    MakeQuestLink(pQuest, out);
                    out << " already in table\n";
                    delete result;
                    continue;
                }
            }
            else if (del != std::string::npos)
            {
                if (result)
                {
                    // delete entry
                    out << "deleting ";
                    MakeQuestLink(pQuest, out);
                    out << " from table\n";
                    CharacterDatabase.DirectPExecute("DELETE FROM playerbot_quest_data WHERE autocomplete='%u'", entry);
                    delete result;
                }
                else
                {
                    // entry not found in table
                    MakeQuestLink(pQuest, out);
                    out << " not in table\n";
                    continue;
                }
            }
            else
            {
                out << "'gm quest' does not have that subcommand.";
                break;
            }
        }
        ch.SendSysMessage(out.str().c_str());
    }
    else if (ExtractCommand("check", text))
    {
        if (ExtractCommand("talent", text))
        {
            if (ExtractCommand("spec", text))
            {
                uint32 tsDBError = TalentSpecDBContainsError();
                if (0 != tsDBError)
                {
                    std::ostringstream oss;
                    oss << "Error found in TalentSpec: " << tsDBError;
                    SendWhisper(oss.str(), fromPlayer);
                }
                else
                    SendWhisper("No errors found. High five!", fromPlayer);
            }
        }
        else
            SendWhisper("'gm check' does not have that subcommand.", fromPlayer);
    }
    else if (ExtractCommand("target", text))
    {
        if (ExtractCommand("combat", text))
        {
            for (AttackerInfoList::iterator i = m_attackerInfo.begin(); i != m_attackerInfo.end(); ++i)
                DEBUG_LOG("[Attacker]:{ %s, victim:%s, threat:%.2f, highest-threat:%.2f, count:%d }",
                          i->second.attacker->GetName(),
                          i->second.victim->GetName(),
                          i->second.threat,
                          i->second.threat2,
                          i->second.count);
            DEBUG_LOG("[Attacker]:{ };");
        }
        else if (ExtractCommand("loot", text))
        {
            for (std::list<ObjectGuid>::iterator it = m_lootTargets.begin(); it != m_lootTargets.end(); ++it)
                DEBUG_LOG("[Looter]:{ %s loots [%s] }", m_bot->GetName(), (*it).GetString().c_str());
            DEBUG_LOG("[Looter]:{ }");
        }
        else
            SendWhisper("'gm target' does not have that subcommand.", fromPlayer);
    }
    else if (ExtractCommand("chat", text))
    {
        m_bDebugCommandChat = !m_bDebugCommandChat;
        if (m_bDebugCommandChat)
            SendWhisper("I will now output all commands received.", fromPlayer);
        else
            SendWhisper("I will no longer output commands received.", fromPlayer);
    }
    else
        SendWhisper("'gm' does not have that subcommand.", fromPlayer);
}

void PlayerbotAI::_HandleCommandHelp(std::string& text, Player& fromPlayer)
{
    ChatHandler ch(&fromPlayer);

    // "help help"? Seriously?
    if (ExtractCommand("help", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("help", "Lists all the things you can order me to do... But it's up to me whether to follow your orders... Or not.").c_str());
        return;
    }

    bool bMainHelp = (text.empty()) ? true : false;
    const std::string sInvalidSubcommand = "That's not a valid subcommand.";
    std::string msg = "";
    // All of these must contain the 'bMainHelp' clause -> help lists all major commands
    // Further indented 'ExtractCommand("subcommand")' conditionals make sure these aren't printed for basic "help"
    if (bMainHelp || ExtractCommand("attack", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("attack", "Attack the selected target. Which would, of course, require a valid target.", HL_TARGET).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("pull", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("pull", "Pull the target in a coordinated party/raid manner.", HL_TARGET).c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("pull test", "I'll tell you if I could pull at all. Can be used anywhere.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pull ready", "I'll tell you if I'm ready to pull *right now*. To be used on location with valid target.").c_str());
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("neutralize", text))
    {
        SendWhisper(_HandleCommandHelpHelper("neutralize|neutral", "The bot will try to put its master's target out of combat with crowd control abilities like polymorph, banish, hibernate, shackles and the like.", HL_TARGET), fromPlayer);

        if (!bMainHelp)
        {
            if (!text.empty())
                SendWhisper(sInvalidSubcommand, fromPlayer);
            return;
        }
    }
    if (bMainHelp || ExtractCommand("follow", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("follow", "I will follow you - this also revives me if dead and teleports me if I'm far away.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("follow auto", "Toggles Automatic Follow Distance (ON/OFF).").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("follow info", "I will show my Automatic Follow Distance, toggle status (ON/OFF).").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("follow far", "I will follow at a father distance away from you.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("follow near", "I will follow at a closer distance to you.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("follow reset", "I will reset my follow distance to its original state.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("stay", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("stay", "I will stay put until told otherwise.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("assist", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("assist", "I will assist the character listed, attacking as they attack.", HL_NAME).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("spells", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("spells", "I will list all the spells I know.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("craft", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("craft", "I will create a single specified recipe", HL_RECIPE).c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("craft [RECIPE] all", "I will create all specified recipes").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < alchemy | a >", "List all learnt alchemy recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < blacksmithing | b >", "List all learnt blacksmith recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < cooking | c >", "List all learnt cooking recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < engineering | e >", "List all learnt engineering recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < firstaid | f >", "List all learnt firstaid recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < inscription | i >", "List all learnt inscription recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < jewelcrafting | j >", "List all learnt jewelcrafting recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < leatherworking | l >", "List all learnt leatherworking recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < magic | m >", "List all learnt enchanting recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < smelting | s >", "List all learnt mining recipes").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("craft < tailoring | t >", "List all learnt tailoring recipes").c_str());
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("process", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("process < disenchant | d >", "Disenchants a green coloured [ITEM] or better", HL_ITEM).c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("process < mill | m >", "Grinds 5 herbs [ITEM] to produce pigments", HL_ITEM).c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("process < prospect | p >", "Searches 5 metal ore [ITEM] for precious gems", HL_ITEM).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("enchant", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("enchant", "Lists all enchantments [SPELL] learnt by the bot").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("enchant [SPELL]", "Enchants selected tradable [ITEM] either equipped or in bag", HL_ITEM).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("cast", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("cast", "I will cast the spell or ability listed.", HL_SPELL).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("use", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("use", "I will use the linked item.", HL_ITEM).c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("use [ITEM]", "I will use the first linked item on a selected TARGET.", HL_TARGET).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("use [ITEM]", "I will use the first linked item on an equipped linked item.", HL_ITEM).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("use [ITEM]", "I will use the first linked item on a linked gameobject.", HL_GAMEOBJECT).c_str());

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("equip", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("equip", "I will equip the linked item(s).", HL_ITEM, true).c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("equip auto <on | off | once>", "I will automatically equip items I acquire if they are better than what I'm wearing. Acts as toggle (ON/OFF) if used without subcommand. Fashion sense not included.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("equip info", "I will tell you my equip auto toggle status (ON/OFF).").c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("auto", text, true))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("equip auto on", "Turns auto equip ON, also does an immediate check (like once).").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("equip auto off", "Turns auto equip OFF.").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("equip auto once", "Runs auto equip once, then turns it off.").c_str());

                if (ExtractCommand("on", text, true)) {}
                else if (ExtractCommand("off", text, true)) {}
                else if (ExtractCommand("once", text, true)) {}

                else if (!text.empty()) ch.SendSysMessage(sInvalidSubcommand.c_str());
            }
            else if (ExtractCommand("info", text, true)) {}

            else if (!text.empty()) ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("reset", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("reset", "I will reset all my states, orders, loot list, talent spec, ... Hey, that's kind of like memory loss.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("stats", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("stats", "This will inform you of my wealth, free bag slots and estimated equipment repair costs.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("survey", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("survey", "Lists all available game objects near me.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("find", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("find", "I will find said game object, walk right up to it, and wait.", HL_GAMEOBJECT).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("get", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("get", "I will get said game object and return to your side.", HL_GAMEOBJECT).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("quest", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("quest", "Lists my current quests.").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("quest add", "Adds this quest to my quest log.", HL_QUEST).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("quest drop", "Removes this quest from my quest log.", HL_QUEST).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("quest end", "Turns in my completed quests.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("quest list", "Lists the quests offered to me by this target.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("quest report", "This will give you a full report of all the items, creatures or gameobjects I still need to finish my quests.", HL_QUEST).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("quest complete", "Autocompletes quests, available in database", HL_QUEST, true).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("add", text, true)) {}
            else if (ExtractCommand("drop", text, true)) {}
            else if (ExtractCommand("end", text, true)) {}
            else if (ExtractCommand("list", text, true)) {}
            else if (ExtractCommand("report", text, true)) {}
            else if (ExtractCommand("complete", text, true)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("orders", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("orders", "Shows you my orders. Free will is overrated, right?").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("orders combat <tank | heal | assist | protect | reset> [targetPlayer]", "Sets general orders I should follow. Assist and Protect require a target.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("orders delay <0-10>", "Activates a delay before I start fighting.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("orders resume", "Resume combat orders to what they were before logout.").c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("combat", text, true))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("orders combat tank", "Order me to tank. Best used on paladins, warriors, druids or death knights.").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("orders combat heal", "Order me to heal. Best used on shamans, priests, druids or paladins.").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("orders combat assist", "Assist the linked target focusing our killing power.", HL_TARGET).c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("orders combat protect", "Protect the listed target, attempting to keep aggro away from the target.", HL_TARGET).c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("orders combat reset", "Resets my combat orders as though you'd never given me any at all.").c_str());

                if (ExtractCommand("tank", text, true)) {}
                else if (ExtractCommand("heal", text, true)) {}
                else if (ExtractCommand("assist", text, true)) {}
                else if (ExtractCommand("protect", text, true)) {}
                else if (ExtractCommand("reset", text, true)) {}

                else if (!text.empty())
                    ch.SendSysMessage(sInvalidSubcommand.c_str());
            }
            else if (ExtractCommand("delay", text, true)) {}
            else if (ExtractCommand("resume", text, true)) {}

            else if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("pet", text))
    {
        if (bMainHelp)
            ch.SendSysMessage(_HandleCommandHelpHelper("pet", "Helps command my pet. Must always be used with a subcommand.").c_str());
        else if (text.empty()) // not "help" AND "help pet"
            ch.SendSysMessage(_HandleCommandHelpHelper("pet", "This by itself is not a valid command. Just so you know. To be used with a subcommand, such as...").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("pet abandon", "Abandons active hunter pet.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet cast", "Has my pet cast this spell. May require a treat. Or at least ask nicely.", HL_SPELL).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet react", "Sets my pet's aggro mode.", HL_PETAGGRO).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet spells", "Shows you the spells my pet knows.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet state", "Shows my pet's aggro mode.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet tame", "Allows a hunter to acquire a pet.", HL_TARGET).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("pet toggle", "Toggles autocast for this spell.", HL_SPELL).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("spells", text)) {}
            else if (ExtractCommand("tame", text)) {}
            else if (ExtractCommand("abandon", text)) {}
            else if (ExtractCommand("cast", text)) {}
            else if (ExtractCommand("toggle", text)) {}
            else if (ExtractCommand("state", text)) {}
            else if (ExtractCommand("react", text))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("pet react", "has three modes.").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("aggressive", "sets it so my precious attacks everything in sight.", HL_NONE, false, true).c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("defensive", "sets it so it automatically attacks anything that attacks me, or anything I attack.", HL_NONE, false, true).c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("passive", "makes it so my pet won't attack anything unless directly told to.", HL_NONE, false, true).c_str());

                // Catches all valid subcommands, also placeholders for potential future sub-subcommands
                if (ExtractCommand("aggressive", text, true)) {}
                else if (ExtractCommand("defensive", text, true)) {}
                else if (ExtractCommand("passive", text, true)) {}
                if (!text.empty())
                    ch.SendSysMessage(sInvalidSubcommand.c_str());
            }

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("collect", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("collect", "Tells you what my current collect status is. Also lists possible options.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("collect", "Sets what I collect. Obviously the 'none' option should be used alone, but all the others can be mixed.", HL_OPTION, true).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("sell", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("sell", "Adds this to my 'for sale' list.", HL_ITEM, true).c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("sell all", "The next time you sell, I'll sell all my low level white items.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("sell all", "This command must be called each time before you sell, OR I won't auto sell white items.").c_str());
        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("buy", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("buy", "Adds this to my 'purchase' list.", HL_ITEM, true).c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("drop", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("drop", "Drops the linked item(s). Permanently.", HL_ITEM, true).c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("drop all", "When my inventory becomes full, I'll drop all my low level items.").c_str());
        ch.SendSysMessage(_HandleCommandHelpHelper("drop all", "Toggles,|cff1eff00ON|r |cff9d9d9d[grey]|r & useless |cffffffff[white]|r items,|cffff0000OFF|r only |cff9d9d9d[grey]|r items.").c_str());

        if (!bMainHelp)
        {
            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("auction", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("auction", "Lists all my active auctions. With pretty little links and such. Hi hi hi... I'm gonna be sooo rich!").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("auction add", "Adds the item to my 'auction off later' list. I have a lot of lists, you see...", HL_ITEM).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("auction remove", "Adds the item to my 'Don't auction after all' list. Hope it hasn't sold by then!", HL_AUCTION).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("add", text, true)) {}
            else if (ExtractCommand("remove", text, true)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("repair", text))
    {
        if (!bMainHelp && text.empty())
            ch.SendSysMessage(_HandleCommandHelpHelper("repair", "This by itself is not a valid command. Just so you know. To be used with a subcommand, such as...").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("repair", "Has me find an armorer and repair the items you listed.", HL_ITEM).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("repair all", "Has me find an armorer and repair all my items, be they equipped or just taking up bagspace.").c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("all", text)) {}

            if (!text.empty()) ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("talent", text))
    {
        msg = _HandleCommandHelpHelper("talent", "Lists my talents, glyphs, unspent talent points and the cost to reset all talents.");
        ch.SendSysMessage(msg.c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("talent learn", "Has me learn the linked talent.", HL_TALENT).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("talent reset", "Resets my talents. Assuming I have the appropriate amount of sparkly gold, shiny silver, and... unrusted copper.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("talent spec", "Lists all talent specs I can use.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("talent spec #", "I will follow this talent spec. Well, I will if you picked a talent spec that exists.").c_str());

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
        if (!bMainHelp) return;
    }
    if (bMainHelp || ExtractCommand("bank", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("bank", "Gives you my bank balance. I thought that was private.").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("bank deposit", "Deposits the listed items in my bank.", HL_ITEM, true).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("bank withdraw", "Withdraw the listed items from my bank.", HL_ITEM, true).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("deposit", text)) {}
            else if (ExtractCommand("withdraw", text)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("skill", text))
    {
        msg = _HandleCommandHelpHelper("skill", "Lists my primary professions & weapon skills.");
        ch.SendSysMessage(msg.c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("skill learn", "Lists the things this trainer can teach me. If you've targeted a trainer, that is.", HL_TARGET).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("skill learn", "Have me learn this skill from the selected trainer.", HL_SKILL).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("skill unlearn", "Unlearn the linked (primary) profession and everything that goes with it.", HL_PROFESSION).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("learn", text))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("skill learn all", "Learn everything this trainer can teach me.", HL_TARGET).c_str());
                if (ExtractCommand("all", text)) {}
            }
            else if (ExtractCommand("unlearn", text)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (fromPlayer.GetSession()->GetSecurity() > SEC_PLAYER && (bMainHelp || ExtractCommand("gm", text)))
    {
        msg = _HandleCommandHelpHelper("gm", "Lists actions available to GM account level and up.");
        ch.SendSysMessage(msg.c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("gm check", "Lists the things you can run a check on.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("gm target", "Lists target items that can be monitored.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("gm chat", "Outputs all commands the bot receives - including those it feels it can't obey.").c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("gm quest", "Lists options to add/remove quests from database.").c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("check", text))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("gm check talent", "Lists talent mechanics you can run a check on.").c_str());

                if (ExtractCommand("talent", text))
                {
                    ch.SendSysMessage(_HandleCommandHelpHelper("gm check talent spec", "Checks the talent spec database for various errors. Only the first error (if any) is returned.").c_str());

                    if (ExtractCommand("spec", text)) {}

                    if (!text.empty()) ch.SendSysMessage(sInvalidSubcommand.c_str());
                    return;
                }

                if (!text.empty()) ch.SendSysMessage(sInvalidSubcommand.c_str());
                return;
            }
            else if (ExtractCommand("target", text))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("gm target combat", "Lists current attacking targets.").c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("gm target loot", "Lists current lootable targets.").c_str());
            }
            else if (ExtractCommand("quest", text))
            {
                ch.SendSysMessage(_HandleCommandHelpHelper("gm quest add", "Add quests to database", HL_QUEST, true).c_str());
                ch.SendSysMessage(_HandleCommandHelpHelper("gm quest del", "Remove quests from database.", HL_QUEST, true).c_str());
            }
            else if (ExtractCommand("chat", text)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }
    if (bMainHelp || ExtractCommand("mail", text))
    {
        ch.SendSysMessage(_HandleCommandHelpHelper("mail inbox |cFFFFFF00|h[Mailbox]|h|r", "Lists all bot mail from selected [Mailbox]").c_str());

        if (!bMainHelp)
        {
            ch.SendSysMessage(_HandleCommandHelpHelper("mail getcash", "Gets money from all selected [Mailid]..", HL_MAIL, true).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("mail getitem", "Gets items from all selected [Mailid]..", HL_MAIL, true).c_str());
            ch.SendSysMessage(_HandleCommandHelpHelper("mail delete", "Delete all selected [Mailid]..", HL_MAIL, true).c_str());

            // Catches all valid subcommands, also placeholders for potential future sub-subcommands
            if (ExtractCommand("inbox", text, true)) {}
            else if (ExtractCommand("getcash", text, true)) {}
            else if (ExtractCommand("getitem", text, true)) {}
            else if (ExtractCommand("delete", text, true)) {}

            if (!text.empty())
                ch.SendSysMessage(sInvalidSubcommand.c_str());
            return;
        }
    }

    if (bMainHelp)
        ch.SendSysMessage(_HandleCommandHelpHelper("help", "Gives you this listing of main commands... But then, you know that already don't you.").c_str());

    if (!text.empty())
        ch.SendSysMessage("Either that is not a valid command, or someone forgot to add it to my help journal. I mean seriously, they can't expect me to remember *all* this stuff, can they?");
}

std::string PlayerbotAI::_HandleCommandHelpHelper(std::string sCommand, std::string sExplain, HELPERLINKABLES reqLink, bool bReqLinkMultiples, bool bCommandShort)
{
    if (sCommand.empty())
    {
        DEBUG_LOG("[PlayerbotAI] _HandleCommandHelpHelper called with an empty sCommand. Ignoring call.");
        return "";
    }

    std::ostringstream oss;
    oss << "'|cffffffff";
    if (bCommandShort)
        oss << "(" << sCommand.at(0) << ")" << sCommand.substr(1);
    else
        oss << sCommand;

    if (reqLink != HL_NONE)
    {
        if (reqLink == HL_PROFESSION)
        {
            oss << " [PROFESSION]";
            if (bReqLinkMultiples)
                oss << " [PROFESSION] ..";
        }
        else if (reqLink == HL_ITEM)
        {
            oss << " [ITEM]";
            if (bReqLinkMultiples)
                oss << " [ITEM] ..";
        }
        else if (reqLink == HL_TALENT)
        {
            oss << " [TALENT]";
            if (bReqLinkMultiples)
                oss << " [TALENT] ..";
        }
        else if (reqLink == HL_SKILL)
        {
            oss << " [SKILL]";
            if (bReqLinkMultiples)
                oss << " [SKILL] ..";
        }
        else if (reqLink == HL_OPTION)
        {
            oss << " <OPTION>";
            if (bReqLinkMultiples)
                oss << " <OPTION> ..";
        }
        else if (reqLink == HL_PETAGGRO)
        {
            oss << " <(a)ggressive | (d)efensive | (p)assive>";
            if (bReqLinkMultiples)
                DEBUG_LOG("[PlayerbotAI] _HandleCommandHelpHelper: sCommand \"pet\" with bReqLinkMultiples \"true\". ... Why? Bug, surely.");
        }
        else if (reqLink == HL_QUEST)
        {
            oss << " [QUEST]";
            if (bReqLinkMultiples)
                oss << " [QUEST] ..";
        }
        else if (reqLink == HL_GAMEOBJECT)
        {
            oss << " [GAMEOBJECT]";
            if (bReqLinkMultiples)
                oss << " [GAMEOBJECT] ..";
        }
        else if (reqLink == HL_SPELL)
        {
            oss << " <Id# | (part of) name | [SPELL]>";
            if (bReqLinkMultiples)
                oss << " <Id# | (part of) name | [SPELL]> ..";
        }
        else if (reqLink == HL_TARGET)
        {
            oss << " (TARGET)";
            if (bReqLinkMultiples)
                oss << " (TARGET) ..";
        }
        else if (reqLink == HL_NAME)
        {
            oss << " <NAME>";
            if (bReqLinkMultiples)
                oss << " <NAME> ..";
        }
        else if (reqLink == HL_AUCTION)
        {
            oss << " [AUCTION]";
            if (bReqLinkMultiples)
                oss << " [AUCTION] ..";
        }
        else if (reqLink == HL_RECIPE)
        {
            oss << " [RECIPE]";
            if (bReqLinkMultiples)
                oss << " [RECIPE] ..";
        }
        else if (reqLink == HL_MAIL)
        {
            oss << " [MAILID]";
            if (bReqLinkMultiples)
                oss << " [MAILID] ..";
        }
        else
        {
            oss << " {unknown}";
            if (bReqLinkMultiples)
                oss << " {unknown} ..";
            DEBUG_LOG("[PlayerbotAI]: _HandleCommandHelpHelper - Uncaught case");
        }
    }

    oss << "|r': " << sExplain;

    return oss.str();
}
