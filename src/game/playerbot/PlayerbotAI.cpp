#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "../ItemPrototype.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "../GridNotifiers.h"
#include "../GridNotifiersImpl.h"
#include "../CellImpl.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "PlayerbotDeathKnightAI.h"
#include "PlayerbotDruidAI.h"
#include "PlayerbotHunterAI.h"
#include "PlayerbotMageAI.h"
#include "PlayerbotPaladinAI.h"
#include "PlayerbotPriestAI.h"
#include "PlayerbotRogueAI.h"
#include "PlayerbotShamanAI.h"
#include "PlayerbotWarlockAI.h"
#include "PlayerbotWarriorAI.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "../Chat.h"
#include "WorldPacket.h"
#include "../Spell.h"
#include "../Unit.h"
#include "../SpellAuras.h"
#include "../SharedDefines.h"
#include "Log.h"
#include "../GossipDef.h"
#include "../MotionMaster.h"
#include "../AuctionHouseMgr.h"
#include "../Mail.h"

// returns a float in range of..
float rand_float(float low, float high)
{
    return (rand() / (static_cast<float> (RAND_MAX) + 1.0)) * (high - low) + low;
}

// ChatHandler already implements some useful commands the master can call on bots
// These commands are protected inside the ChatHandler class so this class provides access to the commands
// we'd like to call on our bots
class PlayerbotChatHandler : protected ChatHandler
{
public:
    explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
    bool revive(Player& botPlayer) { return HandleReviveCommand((char *) botPlayer.GetName()); }
    bool teleport(Player& botPlayer) { return HandleNamegoCommand((char *) botPlayer.GetName()); }
    void sysmessage(const char *str) { SendSysMessage(str); }
    bool dropQuest(char *str) { return HandleQuestRemoveCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot) :
    m_mgr(mgr), m_bot(bot), m_classAI(0), m_ignoreAIUpdatesUntilTime(0),
    m_combatOrder(ORDERS_NONE), m_ScenarioType(SCENARIO_PVEEASY),
    m_TimeDoneEating(0), m_TimeDoneDrinking(0),
    m_CurrentlyCastingSpellId(0), m_spellIdCommand(0),
    m_targetGuidCommand(ObjectGuid()),
    m_taxiMaster(ObjectGuid())
{

    // set bot state and needed item list
    m_botState = BOTSTATE_NORMAL;
    SetQuestNeedItems();

    // reset some pointers
    m_targetChanged = false;
    m_targetType = TARGET_NORMAL;
    m_targetCombat = 0;
    m_targetAssist = 0;
    m_targetProtect = 0;

    // set collection options
    m_collectionFlags = 0;
    if (m_mgr->m_confCollectCombat)
        SetCollectFlag(COLLECT_FLAG_COMBAT);
    if (m_mgr->m_confCollectQuest)
        SetCollectFlag(COLLECT_FLAG_QUEST);
    if (m_mgr->m_confCollectProfession)
        SetCollectFlag(COLLECT_FLAG_PROFESSION);
    if (m_mgr->m_confCollectLoot)
        SetCollectFlag(COLLECT_FLAG_LOOT);
    if (m_mgr->m_confCollectSkin && m_bot->HasSkill(SKILL_SKINNING))
        SetCollectFlag(COLLECT_FLAG_SKIN);
    if (m_mgr->m_confCollectObjects)
        SetCollectFlag(COLLECT_FLAG_NEAROBJECT);

    // start following master (will also teleport bot to master)
    SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());

    // get class specific ai
    switch (m_bot->getClass())
    {
        case CLASS_PRIEST:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI *) new PlayerbotPriestAI(GetMaster(), m_bot, this);
            break;
        case CLASS_MAGE:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI *) new PlayerbotMageAI(GetMaster(), m_bot, this);
            break;
        case CLASS_WARLOCK:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI *) new PlayerbotWarlockAI(GetMaster(), m_bot, this);
            break;
        case CLASS_WARRIOR:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotWarriorAI(GetMaster(), m_bot, this);
            break;
        case CLASS_SHAMAN:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotShamanAI(GetMaster(), m_bot, this);
            break;
        case CLASS_PALADIN:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotPaladinAI(GetMaster(), m_bot, this);
            break;
        case CLASS_ROGUE:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotRogueAI(GetMaster(), m_bot, this);
            break;
        case CLASS_DRUID:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotDruidAI(GetMaster(), m_bot, this);
            break;
        case CLASS_HUNTER:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI *) new PlayerbotHunterAI(GetMaster(), m_bot, this);
            break;
        case CLASS_DEATH_KNIGHT:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI *) new PlayerbotDeathKnightAI(GetMaster(), m_bot, this);
            break;
    }

    HERB_GATHERING      = initSpell(HERB_GATHERING_1);
    MINING              = initSpell(MINING_1);
    SKINNING            = initSpell(SKINNING_1);
}

PlayerbotAI::~PlayerbotAI()
{
    if (m_classAI) delete m_classAI;
}

Player* PlayerbotAI::GetMaster() const
{
    return m_mgr->GetMaster();
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

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
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

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
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
        // Work around buggy chains
        if (sSpellStore.LookupEntry(spellId)->SpellIconID != sSpellStore.LookupEntry(itr->second)->SpellIconID)
            continue;

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
        const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
        DEBUG_LOG ("[PlayerbotAI]: initSpell - Playerbot spell init: %s is %u", pSpellInfo->SpellName[0], spellId);

        // Add spell to spellrange map
        Spell *spell = new Spell(m_bot, pSpellInfo, false);
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange, IsPositiveSpell(spellId));
        m_bot->ApplySpellMod(spellId, SPELLMOD_RANGE, range, spell);
        m_spellRangeMap.insert(std::pair<uint32, float>(spellId, range));
        delete spell;
    }
    return (next == 0) ? spellId : next;
}


// Pet spells do not form chains like player spells.
// One of the options to initialize a spell is to use spell icon id
uint32 PlayerbotAI::initPetSpell(uint32 spellIconId)
{
    Pet * pet = m_bot->GetPet();

    if (!pet)
        return 0;

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        const uint32 spellId = itr->first;

        if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->SpellIconID == spellIconId)
            return spellId;
    }

    // Nothing found
    return 0;
}

/*
 * Send a list of equipment that is in bot's inventor that is currently unequipped.
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
        equip[i] = NULL;

    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        uint16 dest;
        uint8 msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
        if (msg != EQUIP_ERR_OK)
            continue;

        // the dest looks like it includes the old loc in the 8 higher bits
        // so casting it to a uint8 strips them
        int8 equipSlot = uint8(dest);
        if (!(equipSlot >= 0 && equipSlot < 19))
            continue;

        // create a list if one doesn't already exist
        if (equip[equipSlot] == NULL)
            equip[equipSlot] = new std::list<Item*>;

        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        itemListForEqSlot->push_back(pItem);
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 dest;
                uint8 msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;

                int8 equipSlot = uint8(dest);
                if (!(equipSlot >= 0 && equipSlot < 19))
                    continue;

                // create a list if one doesn't already exist
                if (equip[equipSlot] == NULL)
                    equip[equipSlot] = new std::list<Item*>;

                std::list<Item*>* itemListForEqSlot = equip[equipSlot];
                itemListForEqSlot->push_back(pItem);
            }
    }

    TellMaster("Here's all the items in my inventory that I can equip.");
    ChatHandler ch(GetMaster());

    const std::string descr[] = { "head", "neck", "shoulders", "body", "chest",
                                  "waist", "legs", "feet", "wrists", "hands", "finger1", "finger2",
                                  "trinket1", "trinket2", "back", "mainhand", "offhand", "ranged",
                                  "tabard" };

    // now send client all items that can be equipped by slot
    for (uint8 equipSlot = 0; equipSlot < 19; ++equipSlot)
    {
        if (equip[equipSlot] == NULL)
            continue;
        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        std::ostringstream out;
        out << descr[equipSlot] << ": ";
        for (std::list<Item*>::iterator it = itemListForEqSlot->begin(); it != itemListForEqSlot->end(); ++it)
        {
            const ItemPrototype* const pItemProto = (*it)->GetProto();

            std::string itemName = pItemProto->Name1;
            ItemLocalization(itemName, pItemProto->ItemId);

            out << " |cffffffff|Hitem:" << pItemProto->ItemId
                << ":0:0:0:0:0:0:0" << "|h[" << itemName
                << "]|h|r";
        }
        ch.SendSysMessage(out.str().c_str());

        delete itemListForEqSlot; // delete list of Item*
    }
}

void PlayerbotAI::SendQuestItemList(Player& /*player*/)
{
    std::ostringstream out;

    for (BotNeedItem::iterator itr = m_needItemList.begin(); itr != m_needItemList.end(); ++itr)
    {
        const ItemPrototype * pItemProto = sObjectMgr.GetItemPrototype(itr->first);

        std::string itemName = pItemProto->Name1;
        ItemLocalization(itemName, pItemProto->ItemId);

        out << " " << itr->second << "x|cffffffff|Hitem:" << pItemProto->ItemId
            << ":0:0:0:0:0:0:0" << "|h[" << itemName
            << "]|h|r";
    }

    TellMaster("Here's a list of all items I need for quests:");
    TellMaster(out.str().c_str());
}

bool PlayerbotAI::IsItemUseful(uint32 itemid)
{
    ItemPrototype const *pProto = ObjectMgr::GetItemPrototype(itemid);
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
        case ITEM_CLASS_QUEST:
            if (!HasCollectFlag(COLLECT_FLAG_QUEST))
                break;
        case ITEM_CLASS_KEY:
            return true;
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

void PlayerbotAI::SendOrders(Player& /*player*/)
{
    std::ostringstream out;

    if (!m_combatOrder)
        out << "Got no combat orders!";
    else if (m_combatOrder & ORDERS_TANK)
        out << "I TANK";
    else if (m_combatOrder & ORDERS_ASSIST)
        out << "I ASSIST " << (m_targetAssist ? m_targetAssist->GetName() : "unknown");
    else if (m_combatOrder & ORDERS_HEAL)
        out << "I HEAL";
    if ((m_combatOrder & ORDERS_PRIMARY) && (m_combatOrder & ORDERS_SECONDARY))
        out << " and ";
    if (m_combatOrder & ORDERS_PROTECT)
        out << "I PROTECT " << (m_targetProtect ? m_targetProtect->GetName() : "unknown");
    out << ".";

    if (m_mgr->m_confDebugWhisper)
    {
        out << " " << (IsInCombat() ? "I'm in COMBAT! " : "Not in combat. ");
        out << "Current state is ";
        if (m_botState == BOTSTATE_NORMAL)
            out << "NORMAL";
        else if (m_botState == BOTSTATE_COMBAT)
            out << "COMBAT";
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
        out << " Next action in " << (m_ignoreAIUpdatesUntilTime - time(0)) << "sec.";
    }

    TellMaster(out.str().c_str());
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
            m_ignoreAIUpdatesUntilTime = time(0) + 4;
            m_ScenarioType = SCENARIO_PVEEASY;
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
            m_ignoreAIUpdatesUntilTime = 0;
            WorldPacket p(packet);
            ObjectGuid flagGuid;
            p >> flagGuid;
            ObjectGuid playerGuid;
            p >> playerGuid;
            Player* const pPlayer = ObjectAccessor::FindPlayer(playerGuid);
            if (canObeyCommandFrom(*pPlayer))
            {
                m_bot->GetMotionMaster()->Clear(true);
                WorldPacket* const packet = new WorldPacket(CMSG_DUEL_ACCEPTED, 8);
                *packet << flagGuid;
                m_bot->GetSession()->QueuePacket(packet); // queue the packet to get around race condition

                // follow target in casting range
                float angle = rand_float(0, M_PI_F);
                float dist = rand_float(4, 10);

                m_bot->GetMotionMaster()->Clear(true);
                m_bot->GetMotionMaster()->MoveFollow(pPlayer, dist, angle);

                m_bot->SetSelectionGuid(ObjectGuid(playerGuid));
                m_ignoreAIUpdatesUntilTime = time(0) + 4;
                m_ScenarioType = SCENARIO_DUEL;
            }
            return;
        }

        case SMSG_AUCTION_COMMAND_RESULT:
        {
            uint32 auctionId, Action, ErrorCode;
            std::string action[3] = {"Creating","Cancelling","Bidding"};
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
                        TellMaster("I can't carry anymore of those.");
                        return;
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
                        TellMaster("My inventory is full.");
                        return;
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
                    default:
                        TellMaster("I can't use that.");
                        DEBUG_LOG ("[PlayerbotAI]: HandleBotOutgoingPacket - SMSG_INVENTORY_CHANGE_FAILURE: %u", err);
                        return;
                }
            }
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
                m_ignoreAIUpdatesUntilTime = time(0);
                m_CurrentlyCastingSpellId = 0;
            }
            return;
        }

        // if a change in speed was detected for the master
        // make sure we have the same mount status
        case SMSG_FORCE_RUN_SPEED_CHANGE:
        {
            WorldPacket p(packet);
            ObjectGuid guid;

            p >> guid.ReadAsPacked();
            if (guid != GetMaster()->GetObjectGuid())
                return;
            if (GetMaster()->IsMounted() && !m_bot->IsMounted())
            {
                //Player Part
                if (!GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).empty())
                {
                    int32 master_speed1 = 0;
                    int32 master_speed2 = 0;
                    master_speed1 = GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[1];
                    master_speed2 = GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[2];

                    //Bot Part
                    uint32 spellMount = 0;
                    for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
                    {
                        uint32 spellId = itr->first;
                        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
                            continue;
                        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
                        if (!pSpellInfo)
                            continue;

                        if (pSpellInfo->EffectApplyAuraName[0] == SPELL_AURA_MOUNTED)
                        {
                            if (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                            {
                                if (pSpellInfo->EffectBasePoints[1] == master_speed1)
                                {
                                    spellMount = spellId;
                                    break;
                                }
                            }
                            else if ((pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                                     && (pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED))
                            {
                                if ((pSpellInfo->EffectBasePoints[1] == master_speed1)
                                    && (pSpellInfo->EffectBasePoints[2] == master_speed2))
                                {
                                    spellMount = spellId;
                                    break;
                                }
                            }
                            else if ((pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                                     && (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED))
                                if ((pSpellInfo->EffectBasePoints[2] == master_speed2)
                                    && (pSpellInfo->EffectBasePoints[1] == master_speed1))
                                {
                                    spellMount = spellId;
                                    break;
                                }
                        }
                    }
                    if (spellMount > 0) m_bot->CastSpell(m_bot, spellMount, false);
                }
            }
            else if (!GetMaster()->IsMounted() && m_bot->IsMounted())
            {
                WorldPacket emptyPacket;
                m_bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);  //updated code
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

        // Handle when another player opens the trade window with the bot
        // also sends list of tradable items bot can trade if bot is allowed to obey commands from
        case SMSG_TRADE_STATUS:
        {
            if (m_bot->GetTrader() == NULL)
                break;

            WorldPacket p(packet);
            uint32 status;
            p >> status;
            p.resize(4);

            //4 == TRADE_STATUS_TRADE_ACCEPT
            if (status == 4)
                m_bot->GetSession()->HandleAcceptTradeOpcode(p);  // packet not used

            //1 == TRADE_STATUS_BEGIN_TRADE
            else if (status == 1)
            {
                m_bot->GetSession()->HandleBeginTradeOpcode(p); // packet not used

                if (!canObeyCommandFrom(*(m_bot->GetTrader())))
                {
                    SendWhisper("I'm not allowed to trade you any of my items, but you are free to give me money or items.", *(m_bot->GetTrader()));
                    return;
                }

                // list out items available for trade
                std::ostringstream out;

                out << "In my main backpack:";
                // list out items in main backpack
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem)
                        MakeItemLink(pItem, out, true);
                }
                ChatHandler ch(m_bot->GetTrader());
                ch.SendSysMessage(out.str().c_str());

                // list out items in other removable backpacks
                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                    {
                        std::ostringstream outbag;
                        outbag << "In my ";
                        const ItemPrototype* const pBagProto = pBag->GetProto();
                        std::string bagName = pBagProto->Name1;
                        ItemLocalization(bagName, pBagProto->ItemId);
                        outbag << bagName << ":";

                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem)
                                MakeItemLink(pItem, outbag, true);
                        }
                        ch.SendSysMessage(outbag.str().c_str());
                    }
                }

                // calculate how much money bot has
                uint32 copper = m_bot->GetMoney();
                uint32 gold = uint32(copper / 10000);
                copper -= (gold * 10000);
                uint32 silver = uint32(copper / 100);
                copper -= (silver * 100);

                // send bot the message
                std::ostringstream whisper;
                whisper << "I have |cff00ff00" << gold
                        << "|r|cfffffc00g|r|cff00ff00" << silver
                        << "|r|cffcdcdcds|r|cff00ff00" << copper
                        << "|r|cffffd333c|r";
                SendWhisper(whisper.str().c_str(), *(m_bot->GetTrader()));
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

            const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
            if (!pSpellInfo)
                return;

            if (pSpellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)
                return;

            m_ignoreAIUpdatesUntilTime = time(0) + (msTime / 1000) + 1;

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

            if (m_CurrentlyCastingSpellId == spellId)
            {
                Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
                if (!pSpell)
                    return;

                uint32 CastingTime = !IsChanneledSpell(pSpell->m_spellInfo) ? GetSpellCastTime(pSpell->m_spellInfo) : GetSpellDuration(pSpell->m_spellInfo);
                if (pSpell->IsChannelActive() || pSpell->IsAutoRepeat())
                    m_ignoreAIUpdatesUntilTime = time(0) + (CastingTime / 1000) + 1;
                else if (pSpell->IsAutoRepeat())
                    m_ignoreAIUpdatesUntilTime = time(0) + 6;
                else
                {
                    m_ignoreAIUpdatesUntilTime = time(0) + 1;
                    m_CurrentlyCastingSpellId = 0;
                }
            }
            return;
        }

        // if someone tries to resurrect, then accept
        case SMSG_RESURRECT_REQUEST:
        {
            if (!m_bot->isAlive())
            {
                WorldPacket p(packet);
                ObjectGuid guid;
                p >> guid;

                WorldPacket* const packet = new WorldPacket(CMSG_RESURRECT_RESPONSE, 8 + 1);
                *packet << guid;
                *packet << uint8(1);                        // accept
                m_bot->GetSession()->QueuePacket(packet);   // queue the packet to get around race condition

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
            p >> gold;      // 4 money on corpse
            p >> items;     // 1 number of items on corpse

            if (gold > 0)
            {
                WorldPacket* const packet = new WorldPacket(CMSG_LOOT_MONEY, 0);
                m_bot->GetSession()->QueuePacket(packet);
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

                if (lootslot_type != LOOT_SLOT_NORMAL && lootslot_type != LOOT_SLOT_OWNER)
                    continue;

                // skinning or collect loot flag = just auto loot everything for getting object
                // corpse = run checks
                if (loot_type == LOOT_SKINNING || HasCollectFlag(COLLECT_FLAG_LOOT) ||
                    (loot_type == LOOT_CORPSE && (IsInQuestItemList(itemid) || IsItemUseful(itemid))))
                {
                    WorldPacket* const packet = new WorldPacket(CMSG_AUTOSTORE_LOOT_ITEM, 1);
                    *packet << itemindex;
                    m_bot->GetSession()->QueuePacket(packet);
                }
            }

            // release loot
            WorldPacket* const packet = new WorldPacket(CMSG_LOOT_RELEASE, 8);
            *packet << guid;
            m_bot->GetSession()->QueuePacket(packet);

            return;
        }

        case SMSG_LOOT_RELEASE_RESPONSE:
        {
            WorldPacket p(packet);
            ObjectGuid guid;

            p >> guid;

            if (guid == m_lootCurrent)
            {
                Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);

                if (c && c->GetCreatureInfo()->SkinLootId && !c->lootForSkin)
                {
                    uint32 reqSkill = c->GetCreatureInfo()->GetRequiredLootSkill();
                    // check if it is a leather skin and if it is to be collected (could be ore or herb)
                    if (m_bot->HasSkill(reqSkill) && ((reqSkill != SKILL_SKINNING) ||
                                                      (HasCollectFlag(COLLECT_FLAG_SKIN) && reqSkill == SKILL_SKINNING)))
                    {
                        // calculate skill requirement
                        uint32 skillValue = m_bot->GetPureSkillValue(reqSkill);
                        uint32 targetLevel = c->getLevel();
                        uint32 reqSkillValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;
                        if (skillValue >= reqSkillValue)
                        {
                            if (m_lootCurrent != m_lootPrev)    // if this wasn't previous loot try again
                            {
                                m_lootPrev = m_lootCurrent;
                                SetIgnoreUpdateTime();
                                return; // so that the DoLoot function is called again to get skin
                            }
                        }
                        else
                            TellMaster("My skill is %u but it requires %u", skillValue, reqSkillValue);
                    }
                }

                // if previous is current, clear
                if (m_lootPrev == m_lootCurrent)
                    m_lootPrev = ObjectGuid();
                // clear current target
                m_lootCurrent = ObjectGuid();
                // clear movement
                m_bot->GetMotionMaster()->Clear();
                m_bot->GetMotionMaster()->MoveIdle();
                SetIgnoreUpdateTime();
            }

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
            uint32 itemslot, itemid, count, totalcount;

            p.read_skip<uint32>();  // 4 0=looted, 1=from npc
            p.read_skip<uint32>();  // 4 0=received, 1=created
            p.read_skip<uint32>();  // 4 IsShowChatMessage
            p >> bagslot;           // 1 bagslot
            p >> itemslot;          // 4 item slot, but when added to stack: 0xFFFFFFFF
            p >> itemid;            // 4 item entry id
            p.read_skip<uint32>();  // 4 SuffixFactor
            p.read_skip<uint32>();  // 4 random item property id
            p >> count;             // 4 count of items
            p >> totalcount;        // 4 count of items in inventory

            if (IsInQuestItemList(itemid))
            {
                m_needItemList[itemid] = (m_needItemList[itemid] - count);
                if (m_needItemList[itemid] <= 0)
                    m_needItemList.erase(itemid);
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
                return;

               default:
               {
                const char* oc = LookupOpcodeName(packet.GetOpcode());

                std::ostringstream out;
                out << "botout: " << oc;
                sLog.outError(out.str().c_str());

                //TellMaster(oc);
               }
             */
    }
}

uint8 PlayerbotAI::GetHealthPercent(const Unit& target) const
{
    return (static_cast<float> (target.GetHealth()) / target.GetMaxHealth()) * 100;
}

uint8 PlayerbotAI::GetHealthPercent() const
{
    return GetHealthPercent(*m_bot);
}

uint8 PlayerbotAI::GetManaPercent(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
}

uint8 PlayerbotAI::GetManaPercent() const
{
    return GetManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetBaseManaPercent(const Unit& target) const
{
    if (target.GetPower(POWER_MANA) >= target.GetCreateMana())
        return (100);
    else
        return (static_cast<float> (target.GetPower(POWER_MANA)) / target.GetCreateMana()) * 100;
}

uint8 PlayerbotAI::GetBaseManaPercent() const
{
    return GetBaseManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetRageAmount(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_RAGE)));
}

uint8 PlayerbotAI::GetRageAmount() const
{
    return GetRageAmount(*m_bot);
}

uint8 PlayerbotAI::GetEnergyAmount(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_ENERGY)));
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

    Item* partialMatch = NULL;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                continue;

            if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                return pItem;

            else if (!partialMatch || (partialMatch && partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank))
                partialMatch = pItem;
        }
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                        continue;

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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
}

void PlayerbotAI::InterruptCurrentCastingSpell()
{
    //TellMaster("I'm interrupting my current spell!");
    WorldPacket* const packet = new WorldPacket(CMSG_CANCEL_CAST, 5);  //changed from thetourist suggestion
    *packet << m_CurrentlyCastingSpellId;
    *packet << m_targetGuidCommand;   //changed from thetourist suggestion
    m_CurrentlyCastingSpellId = 0;
    m_bot->GetSession()->QueuePacket(packet);
}

void PlayerbotAI::Feast()
{
    // stand up if we are done feasting
    if (!(m_bot->GetHealth() < m_bot->GetMaxHealth() || (m_bot->getPowerType() == POWER_MANA && m_bot->GetPower(POWER_MANA) < m_bot->GetMaxPower(POWER_MANA))))
    {
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
        return;
    }

    // wait 3 seconds before checking if we need to drink more or eat more
    time_t currentTime = time(0);
    m_ignoreAIUpdatesUntilTime = currentTime + 3;

    // should we drink another
    if (m_bot->getPowerType() == POWER_MANA && currentTime > m_TimeDoneDrinking
        && ((static_cast<float> (m_bot->GetPower(POWER_MANA)) / m_bot->GetMaxPower(POWER_MANA)) < 0.8))
    {
        Item* pItem = FindDrink();
        if (pItem != NULL)
        {
            UseItem(pItem);
            m_TimeDoneDrinking = currentTime + 30;
            return;
        }
        TellMaster("I need water.");
    }

    // should we eat another
    if (currentTime > m_TimeDoneEating && ((static_cast<float> (m_bot->GetHealth()) / m_bot->GetMaxHealth()) < 0.8))
    {
        Item* pItem = FindFood();
        if (pItem != NULL)
        {
            //TellMaster("eating now...");
            UseItem(pItem);
            m_TimeDoneEating = currentTime + 30;
            return;
        }
        TellMaster("I need food.");
    }

    // if we are no longer eating or drinking
    // because we are out of items or we are above 80% in both stats
    if (currentTime > m_TimeDoneEating && currentTime > m_TimeDoneDrinking)
    {
        TellMaster("done feasting!");
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
    }
}

// intelligently sets a reasonable combat order for this bot
// based on its class / level / etc
void PlayerbotAI::GetCombatTarget(Unit* forcedTarget)
{
    // set combat state, and clear looting, etc...
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        m_lootCurrent = ObjectGuid();
        m_targetCombat = 0;
    }

    // update attacker info now
    UpdateAttackerInfo();

    // check for attackers on protected unit, and make it a forcedTarget if any
    if (!forcedTarget && (m_combatOrder & ORDERS_PROTECT) && m_targetProtect != 0)
    {
        Unit *newTarget = FindAttacker((ATTACKERINFOTYPE) (AIT_VICTIMNOTSELF | AIT_HIGHESTTHREAT), m_targetProtect);
        if (newTarget && newTarget != m_targetCombat)
        {
            forcedTarget = newTarget;
            m_targetType = TARGET_THREATEN;
            if (m_mgr->m_confDebugWhisper)
                TellMaster("Changing target to %s to protect %s", forcedTarget->GetName(), m_targetProtect->GetName());
        }
    }
    else if (forcedTarget)
    {
        if (m_mgr->m_confDebugWhisper)
            TellMaster("Changing target to %s by force!", forcedTarget->GetName());
        m_targetType = (m_combatOrder == ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
    }

    // we already have a target and we are not forced to change it
    if (m_targetCombat && !forcedTarget)
        return;

    // are we forced on a target?
    if (forcedTarget)
    {
        m_targetCombat = forcedTarget;
        m_targetChanged = true;
    }
    // do we have to assist someone?
    if (!m_targetCombat && (m_combatOrder & ORDERS_ASSIST) && m_targetAssist != 0)
    {
        m_targetCombat = FindAttacker((ATTACKERINFOTYPE) (AIT_VICTIMNOTSELF | AIT_LOWESTTHREAT), m_targetAssist);
        if (m_mgr->m_confDebugWhisper && m_targetCombat)
            TellMaster("Attacking %s to assist %s", m_targetCombat->GetName(), m_targetAssist->GetName());
        m_targetType = (m_combatOrder == ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
        m_targetChanged = true;
    }
    // are there any other attackers?
    if (!m_targetCombat)
    {
        m_targetCombat = FindAttacker();
        m_targetType = (m_combatOrder == ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
        m_targetChanged = true;
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
    if (m_targetCombat->GetTypeId() == TYPEID_PLAYER && dynamic_cast<Player*> (m_targetCombat)->duel)
    {
        m_ignoreAIUpdatesUntilTime = time(0) + 6;
        return;
    }

    m_bot->SetSelectionGuid((m_targetCombat->GetObjectGuid()));
    m_ignoreAIUpdatesUntilTime = time(0) + 1;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    m_bot->Attack(m_targetCombat, true);

    // add thingToAttack to loot list
    m_lootTargets.push_back(m_targetCombat->GetObjectGuid());

    return;
}

void PlayerbotAI::DoNextCombatManeuver()
{
    // check for new targets
    GetCombatTarget();
    // check if we have a target - fixes crash reported by rrtn (kill hunter's pet bug)
    // if current target for attacks doesn't make sense anymore
    // clear our orders so we can get orders in next update
    if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld() || !m_bot->IsHostileTo(m_targetCombat) || !m_bot->IsInMap(m_targetCombat))
    {
        m_bot->AttackStop();
        m_bot->SetSelectionGuid(ObjectGuid());
        MovementReset();
        m_bot->InterruptNonMeleeSpells(true);
        m_targetCombat = 0;
        m_targetChanged = false;
        m_targetType = TARGET_NORMAL;
        return;
    }

    // do opening moves, if we changed target
    if (m_targetChanged)
    {
        if (GetClassAI())
            m_targetChanged = GetClassAI()->DoFirstCombatManeuver(m_targetCombat);
        else
            m_targetChanged = false;
    }

    // do normal combat movement
    DoCombatMovement();

    if (GetClassAI() && !m_targetChanged)
        (GetClassAI())->DoNextCombatManeuver(m_targetCombat);
}

void PlayerbotAI::DoCombatMovement()
{
    if (!m_targetCombat) return;

    float targetDist = m_bot->GetDistance(m_targetCombat);

    m_bot->SetFacingTo(m_bot->GetAngle(m_targetCombat));

    if (m_combatStyle == COMBAT_MELEE && !m_bot->hasUnitState(UNIT_STAT_CHASE) && ((m_movementOrder == MOVEMENT_STAY && targetDist <= ATTACK_DISTANCE) || (m_movementOrder != MOVEMENT_STAY)))
        // melee combat - chase target if in range or if we are not forced to stay
        m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
    else if (m_combatStyle == COMBAT_RANGED && m_movementOrder != MOVEMENT_STAY)
    {
        // ranged combat - just move within spell range
        // TODO: just follow in spell range! how to determine bots spell range?
        if (targetDist > 25.0f)
            m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
        else
            MovementClear();
    }
}

void PlayerbotAI::SetQuestNeedItems()
{
    // reset values first
    m_needItemList.clear();

    // run through accepted quests, get quest info and data
    for(int qs = 0; qs < MAX_QUEST_LOG_SIZE; ++qs)
    {
        uint32 questid = m_bot->GetQuestSlotQuestId(qs);
        if (questid == 0)
            continue;

        QuestStatusData &qData = m_bot->getQuestStatusMap()[questid];
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
        }
    }
}

void PlayerbotAI::SetState(BotState state)
{
    // DEBUG_LOG ("[PlayerbotAI]: SetState - %s switch state %d to %d", m_bot->GetName(), m_botState, state );
    m_botState = state;
}

uint8 PlayerbotAI::GetFreeBagSpace() const
{
    uint8 space = 0;
    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        Item *pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!pItem)
            ++space;
    }
    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        Bag* pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pBag && pBag->GetProto()->BagFamily == BAG_FAMILY_MASK_NONE)
            space += pBag->GetFreeSlots();
    }
    return space;
}

void PlayerbotAI::DoFlight()
{
    DEBUG_LOG("[PlayerbotAI]: DoFlight - %s : %s", m_bot->GetName(), m_taxiMaster.GetString().c_str());

    Creature *npc = m_bot->GetNPCIfCanInteractWith(m_taxiMaster, UNIT_NPC_FLAG_FLIGHTMASTER);
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
        // DEBUG_LOG ("[PlayerbotAI]: DoLoot - %s is going back to idle", m_bot->GetName() );
        SetState(BOTSTATE_NORMAL);
        m_bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
        return;
    }

    // set first in list to current
    if (m_lootCurrent.IsEmpty())
    {
        m_lootCurrent = m_lootTargets.front();
        m_lootTargets.pop_front();
    }

    WorldObject *wo = m_bot->GetMap()->GetWorldObject(m_lootCurrent);

    // clear invalid object or object that is too far from master
    if (!wo || GetMaster()->GetDistance(wo) > BOTLOOT_DISTANCE)
    {
        m_lootCurrent = ObjectGuid();
        return;
    }

    Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
    GameObject *go = m_bot->GetMap()->GetGameObject(m_lootCurrent);

    // clear creature or object that is not spawned or if not creature or object
    if ((c && c->IsDespawned()) || (go && !go->isSpawned()) || (!c && !go))
    {
        m_lootCurrent = ObjectGuid();
        return;
    }

    uint32 skillId = 0;

    if (c)
    {
        if (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            skillId = c->GetCreatureInfo()->GetRequiredLootSkill();

        // not a lootable creature, clear it
        if (!c->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE) &&
            (!c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE) ||
            (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE) && !m_bot->HasSkill(skillId))))
        {
            m_lootCurrent = ObjectGuid();
            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear();
            m_bot->GetMotionMaster()->MoveIdle();
            return;
        }
    }

    if (m_bot->GetDistance(wo) > CONTACT_DISTANCE + wo->GetObjectBoundingRadius())
    {
        float x, y, z;
        wo->GetContactPoint(m_bot, x, y, z, 0.1f);
        m_bot->GetMotionMaster()->MovePoint(wo->GetMapId(), x, y, z);
        // give time to move to point before trying again
        SetIgnoreUpdateTime(1);
    }

    if (m_bot->GetDistance(wo) < INTERACTION_DISTANCE)
    {
        uint32 reqSkillValue = 0;
        uint32 SkillValue = 0;
        bool keyFailed = false;
        bool skillFailed = false;
        bool forceFailed = false;

        if (c)  // creature
        {
            if (c->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
            {
                // loot the creature
                WorldPacket* const packet = new WorldPacket(CMSG_LOOT, 8);
                *packet << m_lootCurrent;
                m_bot->GetSession()->QueuePacket(packet);
                return; // no further processing is needed
                // m_lootCurrent is reset in SMSG_LOOT_RELEASE_RESPONSE after checking for skinloot
            }
            else if (c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
            {
                // not all creature skins are leather, some are ore or herb
                if (m_bot->HasSkill(skillId) && ((skillId != SKILL_SKINNING) ||
                    (HasCollectFlag(COLLECT_FLAG_SKIN) && skillId == SKILL_SKINNING)))
                {
                    // calculate skinning skill requirement
                    uint32 targetLevel = c->getLevel();
                    reqSkillValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel-10)*10 : targetLevel*5;
                }
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
            LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
            if (lockInfo)
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (lockInfo->Type[i] == LOCK_KEY_ITEM)
                    {
                        if (lockInfo->Index[i] > 0)
                        {
                            reqItem = lockInfo->Index[i];
                            if (m_bot->HasItemCount(reqItem,1))
                                break;
                            continue;
                        }
                    }
                    else if (lockInfo->Type[i] == LOCK_KEY_SKILL)
                    {
                        switch(LockType(lockInfo->Index[i]))
                        {
                            case LOCKTYPE_OPEN:
                                if (CastSpell(3365))    // Opening
                                    return;
                                break;
                            case LOCKTYPE_CLOSE:
                                if (CastSpell(6233))    // Closing
                                    return;
                                break;
                            case LOCKTYPE_QUICK_OPEN:
                                if (CastSpell(6247))    // Opening
                                    return;
                                break;
                            case LOCKTYPE_QUICK_CLOSE:
                                if (CastSpell(6247))    // Closing
                                    return;
                                break;
                            case LOCKTYPE_OPEN_TINKERING:
                                if (CastSpell(6477))    // Opening
                                    return;
                                break;
                            case LOCKTYPE_OPEN_KNEELING:
                                if (CastSpell(6478))    // Opening; listed with 17667 and 22810
                                    return;
                                break;
                            case LOCKTYPE_OPEN_ATTACKING:
                                if (CastSpell(8386))    // Attacking
                                    return;
                                break;
                            case LOCKTYPE_SLOW_OPEN:
                                if (CastSpell(21651))   // Opening; also had 26868
                                    return;
                                break;
                            case LOCKTYPE_SLOW_CLOSE:
                                if (CastSpell(21652))   // Closing
                                    return;
                                break;
                            case LOCKTYPE_OPEN_FROM_VEHICLE:
                                if (CastSpell(61437))   // Opening
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
            }

            // use key on object if available
            if (reqItem > 0 && m_bot->HasItemCount(reqItem,1))
            {
                UseItem(m_bot->GetItemByEntry(reqItem), TARGET_FLAG_OBJECT, m_lootCurrent);
                m_lootCurrent = ObjectGuid();
                return;
            }
            else
                keyFailed = true;
        }

        // determine bot's skill value for object's required skill
        if (skillId != SKILL_NONE)
            SkillValue = uint32(m_bot->GetPureSkillValue(skillId));

        // bot has the specific skill or object requires no skill at all
        if ((m_bot->HasSkill(skillId) && skillId != SKILL_NONE) || (skillId == SKILL_NONE && go))
        {
            if (SkillValue >= reqSkillValue)
            {
                switch(skillId)
                {
                    case SKILL_MINING:
                        if (HasTool(TC_MINING_PICK) && CastSpell(MINING))
                            return;
                        else
                            skillFailed = true;
                        break;
                    case SKILL_HERBALISM:
                        if (CastSpell(HERB_GATHERING))
                            return;
                        else
                            skillFailed = true;
                        break;
                    case SKILL_SKINNING:
                        if (c && HasCollectFlag(COLLECT_FLAG_SKIN) &&
                            HasTool(TC_SKINNING_KNIFE) && CastSpell(SKINNING, *c))
                            return;
                        else
                            skillFailed = true;
                        break;
                    case SKILL_LOCKPICKING:
                        if (CastSpell(PICK_LOCK_1))
                            return;
                        else
                            skillFailed = true;
                        break;
                    case SKILL_NONE:
                        if (CastSpell(3365)) //Spell 3365 = Opening?
                            return;
                        else
                            skillFailed = true;
                        break;
                    default:
                        TellMaster("I'm not sure how to get that.");
                        skillFailed = true;
                        DEBUG_LOG ( "[PlayerbotAI]:DoLoot Skill %u is not implemented", skillId);
                        break;
                }
            }
            else
            {
                TellMaster("My skill is not high enough. It requires %u, but mine is %u.",
                    reqSkillValue, SkillValue);
                skillFailed = true;
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
                    Item *kItem = FindKeyForLockValue(reqSkillValue);
                    if (kItem)
                    {
                        TellMaster("I have a skeleton key that can open it!");
                        UseItem(kItem, TARGET_FLAG_OBJECT, m_lootCurrent);
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
                    Item *bItem = FindBombForLockValue(reqSkillValue);
                    if (bItem)
                    {
                        TellMaster("I can blast it open!");
                        UseItem(bItem, TARGET_FLAG_OBJECT, m_lootCurrent);
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

        // if all attempts failed in some way then clear because it won't get SMSG_LOOT_RESPONSE
        if (keyFailed && skillFailed && forceFailed)
        {
            DEBUG_LOG ( "[PlayerbotAI]: DoLoot attempts failed on [%s]",
                go ? go->GetGOInfo()->name : c->GetCreatureInfo()->Name);
            m_lootCurrent = ObjectGuid();
            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear();
            m_bot->GetMotionMaster()->MoveIdle();
        }
    }
}

void PlayerbotAI::AcceptQuest(Quest const *qInfo, Player *pGiver)
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
            if (qInfo->ReqItemCount[i]>0)
            {
                m_bot->GetPlayerbotAI()->SetQuestNeedItems();
                break;
            }

        // Runsttren: did not add typeid switch from WorldSession::HandleQuestgiverAcceptQuestOpcode!
        // I think it's not needed, cause typeid should be TYPEID_PLAYER - and this one is not handled
        // there and there is no default case also.

        if (qInfo->GetSrcSpell() > 0)
            m_bot->CastSpell(m_bot, qInfo->GetSrcSpell(), true);
    }
}

void PlayerbotAI::TurnInQuests(WorldObject *questgiver)
{
    ObjectGuid giverGUID = questgiver->GetObjectGuid();

    if (!m_bot->IsInMap(questgiver))
        TellMaster("hey you are turning in quests without me!");
    else
    {
        m_bot->SetSelectionGuid(giverGUID);

        // auto complete every completed quest this NPC has
        m_bot->PrepareQuestMenu(giverGUID);
        QuestMenu& questMenu = m_bot->PlayerTalkClass->GetQuestMenu();
        for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
        {
            QuestMenuItem const& qItem = questMenu.GetItem(iI);
            uint32 questID = qItem.m_qId;
            Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

            std::ostringstream out;
            std::string questTitle  = pQuest->GetTitle();
            QuestLocalization(questTitle, questID);

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
                            out << "Quest complete: |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                        }
                        else
                            out << "|cffff0000Unable to turn quest in:|r |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                    }

                    // auto reward quest if one item as reward
                    else if (pQuest->GetRewChoiceItemsCount() == 1)
                    {
                        int rewardIdx = 0;
                        ItemPrototype const *pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        std::string itemName = pRewardItem->Name1;
                        ItemLocalization(itemName, pRewardItem->ItemId);
                        if (m_bot->CanRewardQuest(pQuest, rewardIdx, false))
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, questgiver, true);

                            std::string itemName = pRewardItem->Name1;
                            ItemLocalization(itemName, pRewardItem->ItemId);

                            out << "Quest complete: "
                                << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel()
                                << "|h[" << questTitle << "]|h|r reward: |cffffffff|Hitem:"
                                << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        }
                        else
                            out << "|cffff0000Unable to turn quest in:|r "
                                << "|cff808080|Hquest:" << questID << ':'
                                << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r"
                                << " reward: |cffffffff|Hitem:"
                                << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                    }

                    // else multiple rewards - let master pick
                    else
                    {
                        out << "What reward should I take for |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel()
                            << "|h[" << questTitle << "]|h|r? ";
                        for (uint8 i = 0; i < pQuest->GetRewChoiceItemsCount(); ++i)
                        {
                            ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[i]);
                            std::string itemName = pRewardItem->Name1;
                            ItemLocalization(itemName, pRewardItem->ItemId);
                            out << "|cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        }
                    }
                }
            }

            else if (status == QUEST_STATUS_INCOMPLETE)
                out << "|cffff0000Quest incomplete:|r "
                    << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";

            else if (status == QUEST_STATUS_AVAILABLE)
                out << "|cff00ff00Quest available:|r "
                    << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";

            if (!out.str().empty())
                TellMaster(out.str());
        }
    }
}

bool PlayerbotAI::IsInCombat()
{
    Pet *pet;
    bool inCombat = false;
    inCombat |= m_bot->isInCombat();
    pet = m_bot->GetPet();
    if (pet)
        inCombat |= pet->isInCombat();
    inCombat |= GetMaster()->isInCombat();
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            inCombat |= ref->getSource()->isInCombat();
            pet = ref->getSource()->GetPet();
            if (pet)
                inCombat |= pet->isInCombat();
            ref = ref->next();
        }
    }
    return inCombat;
}

void PlayerbotAI::UpdateAttackersForTarget(Unit *victim)
{
    HostileReference *ref = victim->getHostileRefManager().getFirst();
    while (ref)
    {
        ThreatManager *target = ref->getSource();
        ObjectGuid guid = target->getOwner()->GetObjectGuid();
        m_attackerInfo[guid].attacker = target->getOwner();
        m_attackerInfo[guid].victim = target->getOwner()->getVictim();
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
    Pet *pet = m_bot->GetPet();
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
        GroupReference *gref = m_bot->GetGroup()->GetFirstMember();
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
        Unit *a = itr->second.attacker;
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
        ItemPrototype const *ditemProto = item->GetProto();

        DurabilityCostsEntry const *dcost = sDurabilityCostsStore.LookupEntry(ditemProto->ItemLevel);
        if (!dcost)
        {
            sLog.outError("RepairDurability: Wrong item lvl %u", ditemProto->ItemLevel);
            return TotalCost;
        }

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const *dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
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

Unit* PlayerbotAI::FindAttacker(ATTACKERINFOTYPE ait, Unit *victim)
{
    // list empty? why are we here?
    if (m_attackerInfo.empty())
        return 0;

    // not searching something specific - return first in list
    if (!ait)
        return (m_attackerInfo.begin())->second.attacker;

    float t = ((ait & AIT_HIGHESTTHREAT) ? 0.00 : 9999.00);
    Unit *a = 0;
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
        {
            a = itr->second.attacker;
            itr = m_attackerInfo.end();
        }
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

void PlayerbotAI::SetCombatOrderByStr(std::string str, Unit *target)
{
    CombatOrderType co;
    if (str == "tank") co = ORDERS_TANK;
    else if (str == "assist") co = ORDERS_ASSIST;
    else if (str == "heal") co = ORDERS_HEAL;
    else if (str == "protect") co = ORDERS_PROTECT;
    else
        co = ORDERS_RESET;
    SetCombatOrder(co, target);
}

void PlayerbotAI::SetCombatOrder(CombatOrderType co, Unit *target)
{
    if ((co == ORDERS_ASSIST || co == ORDERS_PROTECT) && !target) {
        TellMaster("Erf, you forget to target assist/protect characters!");
        return;
    }
    if (co == ORDERS_RESET) {
        m_combatOrder = ORDERS_NONE;
        m_targetAssist = 0;
        m_targetProtect = 0;
        TellMaster("Orders are cleaned!");
        return;
    }
    if (co == ORDERS_PROTECT)
        m_targetProtect = target;
    else if (co == ORDERS_ASSIST)
        m_targetAssist = target;
    if ((co & ORDERS_PRIMARY))
        m_combatOrder = (CombatOrderType) (((uint32) m_combatOrder & (uint32) ORDERS_SECONDARY) | (uint32) co);
    else
        m_combatOrder = (CombatOrderType) (((uint32) m_combatOrder & (uint32) ORDERS_PRIMARY) | (uint32) co);
    SendOrders(*GetMaster());
}

void PlayerbotAI::SetMovementOrder(MovementOrderType mo, Unit *followTarget)
{
    m_movementOrder = mo;
    m_followTarget = followTarget;
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

        WorldObject* distTarget = m_followTarget;   // target to distance check

        // don't follow while in combat
        if (m_bot->isInCombat())
            return;

        Player* pTarget;                            // target is player
        if (m_followTarget->GetTypeId() == TYPEID_PLAYER)
            pTarget = ((Player*) m_followTarget);

        if (pTarget)
        {
            // check player for follow situations
            if (pTarget->IsBeingTeleported() || pTarget->IsTaxiFlying())
                return;

            // use player's corpse as distance check target
            if (pTarget->GetCorpse())
                distTarget = pTarget->GetCorpse();
        }

        // is bot too far from the follow target
        if (!m_bot->IsWithinDistInMap(distTarget, 50))
        {
            DoTeleport(*m_followTarget);
            return;
        }

        if (m_bot->isAlive() && !m_bot->IsBeingTeleported())
        {
            float angle = rand_float(0, M_PI_F);
            float dist = rand_float(m_mgr->m_confFollowDistance[0], m_mgr->m_confFollowDistance[1]);
            m_bot->GetMotionMaster()->MoveFollow(m_followTarget, dist, angle);
        }
    }
}

void PlayerbotAI::MovementClear()
{
    // stop...
    m_bot->GetMotionMaster()->Clear(true);
    m_bot->clearUnitState(UNIT_STAT_CHASE);
    m_bot->clearUnitState(UNIT_STAT_FOLLOW);

    // stand up...
    if (!m_bot->IsStandState())
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

bool PlayerbotAI::IsMoving()
{
    return (m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE ? false : true);
}

void PlayerbotAI::SetInFront(const Unit* obj)
{
    if (IsMoving())
        return;

    m_bot->SetInFront(obj);

    // TODO: Schmoozerd wrote a patch which adds MovementInfo::ChangeOrientation()
    // and added a call to it inside WorldObject::SetOrientation. Check if it is
    // merged to the core.
    // http://getmangos.com/community/viewtopic.php?pid=128003
    float ori = m_bot->GetAngle(obj);
    float x, y, z;
    x = m_bot->m_movementInfo.GetPos()->x;
    y = m_bot->m_movementInfo.GetPos()->y;
    z = m_bot->m_movementInfo.GetPos()->z;
    m_bot->m_movementInfo.ChangePosition(x, y, z, ori);

    m_bot->SendHeartBeat();
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
    if (m_bot->IsBeingTeleported() || m_bot->GetTrader())
        return;

    time_t currentTime = time(0);
    if (currentTime < m_ignoreAIUpdatesUntilTime)
        return;

    // default updates occur every two seconds
    m_ignoreAIUpdatesUntilTime = time(0) + 2;

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s died and is not in correct state...", m_bot->GetName() );
            // clear loot list on death
            m_lootTargets.clear();
            m_lootCurrent = ObjectGuid();
            // clear combat orders
            m_bot->SetSelectionGuid(ObjectGuid());
            m_bot->GetMotionMaster()->Clear(true);
            // set state to dead
            SetState(BOTSTATE_DEAD);
            // wait 30sec
            m_ignoreAIUpdatesUntilTime = time(0) + 30;
        }
        else if (m_botState == BOTSTATE_DEAD)
        {
            // become ghost
            if (m_bot->GetCorpse()) {
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s already has a corpse...", m_bot->GetName() );
                SetState(BOTSTATE_DEADRELEASED);
                return;
            }
            m_bot->SetBotDeathTimer();
            m_bot->BuildPlayerRepop();
            // relocate ghost
            WorldLocation loc;
            Corpse *corpse = m_bot->GetCorpse();
            corpse->GetPosition(loc);
            m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
            // set state to released
            SetState(BOTSTATE_DEADRELEASED);
        }
        else if (m_botState == BOTSTATE_DEADRELEASED)
        {
            // get bot's corpse
            Corpse *corpse = m_bot->GetCorpse();
            if (!corpse)
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s has no corpse!", m_bot->GetName() );
                return;
            // teleport ghost from graveyard to corpse
            // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - Teleport %s to corpse...", m_bot->GetName() );
            DoTeleport(*corpse);
            // check if we are allowed to resurrect now
            if (corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(0))
            {
                m_ignoreAIUpdatesUntilTime = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP);
                // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - %s has to wait for %d seconds to revive...", m_bot->GetName(), m_ignoreAIUpdatesUntilTime-time(0) );
                return;
            }
            // resurrect now
            // DEBUG_LOG ("[PlayerbotAI]: UpdateAI - Reviving %s to corpse...", m_bot->GetName() );
            m_ignoreAIUpdatesUntilTime = time(0) + 6;
            PlayerbotChatHandler ch(GetMaster());
            if (!ch.revive(*m_bot))
            {
                ch.sysmessage(".. could not be revived ..");
                return;
            }
            // set back to normal
            SetState(BOTSTATE_NORMAL);
        }
    }
    else
    {
        if(!m_findNPC.empty())
            findNearbyCreature();

        // if we are casting a spell then interrupt it
        // make sure any actions that cast a spell set a proper m_ignoreAIUpdatesUntilTime!
        Spell* const pSpell = GetCurrentSpell();
        if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()))
            InterruptCurrentCastingSpell();

        // direct cast command from master
        else if (m_spellIdCommand != 0)
        {
            Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, m_targetGuidCommand);
            if (pTarget)
                CastSpell(m_spellIdCommand, *pTarget);
            m_spellIdCommand = 0;
            m_targetGuidCommand = ObjectGuid();
        }

        //if master is unmounted, unmount the bot
        else if (!GetMaster()->IsMounted() && m_bot->IsMounted())
        {
            WorldPacket emptyPacket;
            m_bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);  //updated code
        }

        // handle combat (either self/master/group in combat, or combat state and valid target)
        else if (IsInCombat() || (m_botState == BOTSTATE_COMBAT && m_targetCombat))
        {
           //check if the bot is Mounted
           if (!m_bot->IsMounted())
           {
                if (!pSpell || !pSpell->IsChannelActive())
                    DoNextCombatManeuver();
                else
                    SetIgnoreUpdateTime(1);  // It's better to update AI more frequently during combat
            }
        }
        // bot was in combat recently - loot now
        else if (m_botState == BOTSTATE_COMBAT)
        {
            SetState(BOTSTATE_LOOTING);
            m_attackerInfo.clear();
            if (HasCollectFlag(COLLECT_FLAG_COMBAT))
                m_lootTargets.unique();
            else
                m_lootTargets.clear();
            SetIgnoreUpdateTime();
        }
        else if (m_botState == BOTSTATE_LOOTING)
        {
            DoLoot();
        }
        else if (m_botState == BOTSTATE_FLYING)
        {
            /* std::ostringstream out;
               out << "Taxi: " << m_bot->GetName() << m_ignoreAIUpdatesUntilTime;
               TellMaster(out.str().c_str()); */
            DoFlight();
            SetState(BOTSTATE_NORMAL);
            SetIgnoreUpdateTime();
        }
        // if commanded to follow master and not already following master then follow master
        else if (!m_bot->isInCombat() && !IsMoving())
            MovementReset();

        // do class specific non combat actions
        else if (GetClassAI() && !m_bot->IsMounted())
        {
            (GetClassAI())->DoNonCombatActions();

            // have we been told to collect GOs
            if (HasCollectFlag(COLLECT_FLAG_NEAROBJECT))
            {
                findNearbyGO();
                // start looting if have targets
                if (!m_lootTargets.empty())
                    SetState(BOTSTATE_LOOTING);
            }
        }
    }
}

Spell* PlayerbotAI::GetCurrentSpell() const
{
    if (m_CurrentlyCastingSpellId == 0)
        return NULL;

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(m_CurrentlyCastingSpellId);
    return pSpell;
}

void PlayerbotAI::TellMaster(const std::string& text) const
{
    SendWhisper(text, *GetMaster());
}

void PlayerbotAI::TellMaster(const char *fmt, ...) const
{
    char temp_buf[1024];
    va_list ap;
    va_start(ap, fmt);
    (void) vsnprintf(temp_buf, 1024, fmt, ap);
    va_end(ap);
    std::string str = temp_buf;
    TellMaster(str);
}

void PlayerbotAI::SendWhisper(const std::string& text, Player& player) const
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    m_bot->BuildPlayerChat(&data, CHAT_MSG_WHISPER, text, LANG_UNIVERSAL);
    player.GetSession()->SendPacket(&data);
}

bool PlayerbotAI::canObeyCommandFrom(const Player& player) const
{
    return player.GetSession()->GetAccountId() == GetMaster()->GetSession()->GetAccountId();
}

bool PlayerbotAI::CastSpell(const char* args)
{
    uint32 spellId = getSpellId(args);
    return (spellId) ? CastSpell(spellId) : false;
}

bool PlayerbotAI::CastSpell(uint32 spellId, Unit& target)
{
    ObjectGuid oldSel = m_bot->GetSelectionGuid();
    m_bot->SetSelectionGuid(target.GetObjectGuid());
    bool rv = CastSpell(spellId);
    m_bot->SetSelectionGuid(oldSel);
    return rv;
}

bool PlayerbotAI::CastSpell(uint32 spellId)
{
    // some AIs don't check if the bot doesn't have spell before using it
    // so just return false when this happens
    if (spellId == 0)
        return false;

    // check spell cooldown
    if (m_bot->HasSpellCooldown(spellId))
        return false;

    // see Creature.cpp 1738 for reference
    // don't allow bot to cast damage spells on friends
    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
    {
        TellMaster("Missing spell entry in CastSpell for spellid %u.", spellId);
        return false;
    }

    // set target
    ObjectGuid targetGUID = m_bot->GetSelectionGuid();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);

    if (!pTarget)
        pTarget = m_bot;

    // Check spell range
    SpellRanges::iterator it = m_spellRangeMap.find(spellId);
    if (it != m_spellRangeMap.end() && (int) it->second != 0)
    {
        float dist = m_bot->GetCombatDistance(pTarget);
        if (dist > it->second + 1.25) // See Spell::CheckRange for modifier value
            return false;
    }

    // Check line of sight
    if (!m_bot->IsWithinLOSInMap(pTarget))
        return false;

    if (IsPositiveSpell(spellId))
    {
        if (pTarget && !m_bot->IsFriendlyTo(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->IsFriendlyTo(pTarget))
            return false;

        SetInFront(pTarget);
    }

    // stop movement to prevent cancel spell casting
    SpellCastTimesEntry const * castTimeEntry = sSpellCastTimesStore.LookupEntry(pSpellInfo->CastingTimeIndex);
    if (castTimeEntry && castTimeEntry->CastTime)
    {
        DEBUG_LOG ("[PlayerbotAI]: CastSpell - Bot movement reset for casting %s (%u)", pSpellInfo->SpellName[0], spellId);
        MovementClear();
    }

    uint32 target_type = TARGET_FLAG_UNIT;

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK)
        target_type = TARGET_FLAG_OBJECT;

    uint32 CastingTime = !IsChanneledSpell(pSpellInfo) ? GetSpellCastTime(pSpellInfo) : GetSpellDuration(pSpellInfo);

    m_CurrentlyCastingSpellId = spellId;
    m_ignoreAIUpdatesUntilTime = time(0) + ( CastingTime / 1000) + 1;

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        if (m_lootCurrent)
        {
            WorldPacket* const packet = new WorldPacket(CMSG_CAST_SPELL, 1 + 4 + 1 + 4 + 8);
            *packet << uint8(0);                            // spells cast count;
            *packet << spellId;
            *packet << uint8(0);                            // unk_flags
            *packet << uint32(target_type);
            *packet << m_lootCurrent.WriteAsPacked();
            m_bot->GetSession()->QueuePacket(packet);       // queue the packet to get around race condition

            if (target_type == TARGET_FLAG_OBJECT)
            {
                WorldPacket* const packetgouse = new WorldPacket(CMSG_GAMEOBJ_REPORT_USE, 8);
                *packetgouse << m_lootCurrent;
                m_bot->GetSession()->QueuePacket(packetgouse);  // queue the packet to get around race condition
            }
        }
        else
            return false;
    }
    else
        m_bot->CastSpell(pTarget, pSpellInfo, false);       // actually cast spell

    // if this caused the caster to move (blink) update the position
    // I think this is normally done on the client
    // this should be done on spell success
    /*
       if (name == "Blink") {
       float x,y,z;
       m_bot->GetPosition(x,y,z);
       m_bot->GetNearPoint(m_bot, x, y, z, 1, 5, 0);
       m_bot->Relocate(x,y,z);
       m_bot->SendHeartBeat();

       }
     */

    return true;
}

bool PlayerbotAI::CastPetSpell(uint32 spellId, Unit* target)
{
    if (spellId == 0)
        return false;

    Pet* pet = m_bot->GetPet();
    if (!pet)
        return false;

    if (pet->HasSpellCooldown(spellId))
        return false;

    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
    {
        TellMaster("Missing spell entry in CastPetSpell()");
        return false;
    }

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
        if (pTarget && !m_bot->IsFriendlyTo(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->IsFriendlyTo(pTarget))
            return false;

        if (!pet->isInFrontInMap(pTarget, 10)) // distance probably should be calculated
            pet->SetInFront(pTarget);
    }

    pet->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = pet->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    return true;
}

// Perform sanity checks and cast spell
bool PlayerbotAI::Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player *))
{
    if (spellId == 0)
        return false;

    SpellEntry const * spellProto = sSpellStore.LookupEntry(spellId);

    if (!spellProto)
        return false;

    if (!target)
        return false;

    // Select appropriate spell rank for target's level
    spellProto = sSpellMgr.SelectAuraRankForLevel(spellProto, target->getLevel());
    if (!spellProto)
        return false;

    // Check if spell will boost one of already existent auras
    bool willBenefitFromSpell = false;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (spellProto->EffectApplyAuraName[i] == SPELL_AURA_NONE)
            break;

        bool sameOrBetterAuraFound = false;
        int32 bonus = m_bot->CalculateSpellDamage(target, spellProto, SpellEffectIndex(i));
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
        return false;

    // Druids may need to shapeshift before casting
    if (beforeCast)
        (*beforeCast)(m_bot);

    return CastSpell(spellProto->Id, *target);
}

// Can be used for personal buffs like Mage Armor and Inner Fire
bool PlayerbotAI::SelfBuff(uint32 spellId)
{
    if (spellId == 0)
        return false;

    if (m_bot->HasAura(spellId))
        return false;

    return CastSpell(spellId, *m_bot);
}

// Checks if spell is single per target per caster and will make any effect on target
bool PlayerbotAI::CanReceiveSpecificSpell(uint8 spec, Unit* target) const
{
    if (IsSingleFromSpellSpecificPerTargetPerCaster(SpellSpecific(spec), SpellSpecific(spec)))
    {
        Unit::SpellAuraHolderMap holders = target->GetSpellAuraHolderMap();
        Unit::SpellAuraHolderMap::iterator it;
        for (it = holders.begin(); it != holders.end(); ++it)
            if ((*it).second->GetCasterGuid() == m_bot->GetObjectGuid() && GetSpellSpecific((*it).second->GetId()) == SpellSpecific(spec))
                return false;
    }
    return true;
}

Item* PlayerbotAI::FindItem(uint32 ItemId)
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

            if (pItemProto->ItemId == ItemId)   // have required item
                return pItem;
        }
    }
    // list out items in other removable backpacks
    //INVENTORY_SLOT_BAG_START = 19
    //INVENTORY_SLOT_BAG_END = 23

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)  // 20 to 23 = 4
    {
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);   // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);  // 1 to bagsize = ?
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
    return NULL;
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
        const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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
    return NULL;
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

    return NULL;
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

    return NULL;
}

bool PlayerbotAI::HasTool(uint32 TC)
{
    std::ostringstream out;

    switch (TC)
    {
        case TC_MINING_PICK:

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffffffffI do not have a pick!";
            break;

        case TC_SKINNING_KNIFE:

            if (m_bot->HasItemTotemCategory(TC))
                return true;
            else
                out << "|cffffffffI do not have a skinning knife!";
            break;
        default:
            out << "|cffffffffI do not know what tool that needs!";
    }
    TellMaster(out.str().c_str());
    return false;
}

bool PlayerbotAI::HasSpellReagents(uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
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

// Build an hlink for spells
void PlayerbotAI::MakeSpellLink(const SpellEntry *sInfo, std::ostringstream &out, Player *player)
{
    LocaleConstant loc = LOCALE_enUS;
    if (player)
        loc = player->GetSession()->GetSessionDbcLocale();
    out << "|cffffffff|Hspell:" << sInfo->Id << "|h[" << sInfo->SpellName[loc] << "]|h|r";
}

// Builds a hlink for an item, but since its
// only a ItemPrototype, we cant fill in everything
void PlayerbotAI::MakeItemLink(const ItemPrototype *item, std::ostringstream &out)
{
    // Color
    out << "|c";
    switch(item->Quality)
    {
        case ITEM_QUALITY_POOR:     out << "ff9d9d9d"; break;  //GREY
        case ITEM_QUALITY_NORMAL:   out << "ffffffff"; break;  //WHITE
        case ITEM_QUALITY_UNCOMMON: out << "ff1eff00"; break;  //GREEN
        case ITEM_QUALITY_RARE:     out << "ff0070dd"; break;  //BLUE
        case ITEM_QUALITY_EPIC:     out << "ffa335ee"; break;  //PURPLE
        case ITEM_QUALITY_LEGENDARY:out << "ffff8000"; break;  //ORANGE
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
}

// Builds a hlink for an item, includes everything
// |color|Hitem:item_id:perm_ench_id:gem1:gem2:gem3:0:0:0:0:reporter_level|h[name]|h|r
void PlayerbotAI::MakeItemLink(const Item *item, std::ostringstream &out, bool IncludeQuantity /*= true*/)
{
    const ItemPrototype *proto = item->GetProto();
    // Color
    out << "|c";
    switch(proto->Quality)
    {
        case ITEM_QUALITY_POOR:     out << "ff9d9d9d"; break;  //GREY
        case ITEM_QUALITY_NORMAL:   out << "ffffffff"; break;  //WHITE
        case ITEM_QUALITY_UNCOMMON: out << "ff1eff00"; break;  //GREEN
        case ITEM_QUALITY_RARE:     out << "ff0070dd"; break;  //BLUE
        case ITEM_QUALITY_EPIC:     out << "ffa335ee"; break;  //PURPLE
        case ITEM_QUALITY_LEGENDARY:out << "ffff8000"; break;  //ORANGE
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
    for(uint32 slot = SOCK_ENCHANTMENT_SLOT; slot < SOCK_ENCHANTMENT_SLOT+MAX_GEM_SOCKETS; ++slot)
    {
        uint32 eId = item->GetEnchantmentId(EnchantmentSlot(slot));
        if (!eId) continue;

        SpellItemEnchantmentEntry const* entry = sSpellItemEnchantmentStore.LookupEntry(eId);
        if (!entry) continue;

        switch(slot-SOCK_ENCHANTMENT_SLOT)
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

void PlayerbotAI::extractSpellId(const std::string& text, uint32 &spellId) const
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

void PlayerbotAI::extractSpellIdList(const std::string& text, BotSpellList& m_spellsToLearn) const
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

void PlayerbotAI::extractTalentIds(const std::string &text, std::list<talentPair> &talentIds) const
{
    // Link format:
    // |color|Htalent:talent_id:rank|h[name]|h|r
    // |cff4e96f7|Htalent:1396:4|h[Unleashed Fury]|h|r

    uint8 pos = 0;
    while(true)
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
            talentIds.push_back(std::pair<uint32 ,uint32>(id, rank));
    }
}

void PlayerbotAI::extractGOinfo(const std::string& text, BotLootTarget& m_lootTargets) const
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

        if (guid)
            m_lootTargets.push_back(lootCurrent);
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
        Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
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

    std::list<GameObject*> tempTargetGOList;
    float radius = 20.0f;

    for (BotLootEntry::iterator itr = m_collectObjects.begin(); itr != m_collectObjects.end(); ++itr)
    {
        uint32 entry = *(itr);
        GameObjectInfo const * gInfo = ObjectMgr::GetGameObjectInfo(entry);
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
        MaNGOS::GameObjectEntryInPosRangeCheck go_check(*m_bot, entry, m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), radius);
        MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectEntryInPosRangeCheck> checker(tempTargetGOList, go_check);
        Cell::VisitGridObjects(m_bot, checker, radius);

        // no objects found, continue to next entry
        if (tempTargetGOList.empty())
            continue;

        // add any objects found to our lootTargets
        for (std::list<GameObject*>::iterator iter = tempTargetGOList.begin(); iter != tempTargetGOList.end(); ++iter)
        {
            GameObject* go = (*iter);
            if (go->isSpawned())
                m_lootTargets.push_back(go->GetObjectGuid());
        }
    }
}

void PlayerbotAI::findNearbyCreature()
{
    std::list<Creature*> creatureList;
    float radius = INTERACTION_DISTANCE;

    CellPair pair(MaNGOS::ComputeCellPair(m_bot->GetPositionX(), m_bot->GetPositionY()));
    Cell cell(pair);

    MaNGOS::AnyUnitInObjectRangeCheck go_check(m_bot, radius);
    MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> go_search(creatureList, go_check);
    TypeContainerVisitor<MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck>, GridTypeMapContainer> go_visit(go_search);

    // Get Creatures
    cell.Visit(pair, go_visit, *(m_bot->GetMap()), *(m_bot), radius);

    // if (!creatureList.empty())
    //    TellMaster("Found %i Creatures.", creatureList.size());

    for (std::list<Creature*>::iterator iter = creatureList.begin(); iter != creatureList.end(); iter++)
    {
        Creature* currCreature = *iter;

        for (std::list<enum NPCFlags>::iterator itr = m_findNPC.begin(); itr != m_findNPC.end(); itr = m_findNPC.erase(itr))
        {
            uint32 npcflags = currCreature->GetUInt32Value(UNIT_NPC_FLAGS);

            if (!(*itr & npcflags))
                continue;

            if ((*itr == UNIT_NPC_FLAG_TRAINER_CLASS) && !currCreature->CanTrainAndResetTalentsOf(m_bot))
                continue;

            WorldObject *wo = m_bot->GetMap()->GetWorldObject(currCreature->GetObjectGuid());

            if (m_bot->GetDistance(wo) > CONTACT_DISTANCE + wo->GetObjectBoundingRadius())
            {
                float x, y, z;
                wo->GetContactPoint(m_bot, x, y, z, 1.0f);
                m_bot->GetMotionMaster()->MovePoint(wo->GetMapId(), x, y, z);
                // give time to move to point before trying again
                SetIgnoreUpdateTime(1);
            }

            if (m_bot->GetDistance(wo) < INTERACTION_DISTANCE)
            {

                // DEBUG_LOG("%s is interacting with (%s)",m_bot->GetName(),currCreature->GetCreatureInfo()->Name);
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
                        case GOSSIP_OPTION_INNKEEPER:
                        case GOSSIP_OPTION_TRAINER:
                        case GOSSIP_OPTION_QUESTGIVER:
                        case GOSSIP_OPTION_VENDOR:
                        case GOSSIP_OPTION_UNLEARNTALENTS:
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
                                        // sell items
                                        case SELL_ITEMS:
                                        {
                                            // TellMaster("Selling items");
                                            Sell(ait->second);
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
                    m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }
            }
            m_bot->GetMotionMaster()->Clear();
            m_bot->GetMotionMaster()->MoveIdle();
        }
    }
}

// use item on self
void PlayerbotAI::UseItem(Item *item)
{
    UseItem(item, TARGET_FLAG_SELF, ObjectGuid());
}

// use item on equipped item
void PlayerbotAI::UseItem(Item *item, uint8 targetInventorySlot)
{
    if (targetInventorySlot >= EQUIPMENT_SLOT_END)
        return;

    Item* const targetItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetInventorySlot);
    if (!targetItem)
        return;

    UseItem(item, TARGET_FLAG_ITEM, targetItem->GetObjectGuid());
}

// use item on unit
void PlayerbotAI::UseItem(Item *item, Unit *target)
{
    if (!target)
        return;

    UseItem(item, TARGET_FLAG_UNIT, target->GetObjectGuid());
}

// generic item use method
void PlayerbotAI::UseItem(Item *item, uint32 targetFlag, ObjectGuid targetGUID)
{
    if (!item)
        return;

    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 cast_count = 1;
    ObjectGuid item_guid = item->GetObjectGuid();
    uint32 glyphIndex = 0;
    uint8 unk_flags = 0;

    if(uint32 questid = item->GetProto()->StartQuest)
    {
        std::ostringstream report;

        Quest const* qInfo = sObjectMgr.GetQuestTemplate(questid);
        if (qInfo)
        {
            m_bot->GetMotionMaster()->Clear(true);
            WorldPacket* const packet = new WorldPacket(CMSG_QUESTGIVER_ACCEPT_QUEST, 8+4+4);
            *packet << item_guid;
            *packet << questid;
            *packet << uint32(0);
            m_bot->GetSession()->QueuePacket(packet); // queue the packet to get around race condition
            report << "Got quest " << qInfo->GetTitle();
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

    WorldPacket *packet = new WorldPacket(CMSG_USE_ITEM, 28);
    *packet << bagIndex << slot << cast_count << spellId << item_guid
            << glyphIndex << unk_flags << targetFlag;

    if (targetFlag & (TARGET_FLAG_UNIT | TARGET_FLAG_ITEM | TARGET_FLAG_OBJECT))
        *packet << targetGUID.WriteAsPacked();

    m_bot->GetSession()->QueuePacket(packet);

    SpellEntry const * spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
    {
        TellMaster("Can't find spell entry for spell %u on item %u", spellId, item->GetEntry());
        return;
    }

    SpellCastTimesEntry const * castingTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);
    if (!castingTimeEntry)
    {
        TellMaster("Can't find casting time entry for spell %u with index %u", spellId, spellInfo->CastingTimeIndex);
        return;
    }

    uint8 duration, castTime;
    castTime = (uint8) ((float) castingTimeEntry->CastTime / 1000.0f);

    if (item->GetProto()->Class == ITEM_CLASS_CONSUMABLE && item->GetProto()->SubClass == ITEM_SUBCLASS_FOOD)
    {
        duration = (uint8) ((float) GetSpellDuration(spellInfo) / 1000.0f);
        SetIgnoreUpdateTime(castTime + duration);
    }
    else
        SetIgnoreUpdateTime(castTime);
}

// submits packet to use an item
void PlayerbotAI::EquipItem(Item* src_Item)
{
    uint8 src_bagIndex = src_Item->GetBagSlot();
    uint8 src_slot = src_Item->GetSlot();

    DEBUG_LOG("PlayerbotAI::EquipItem: %s in srcbag = %u, srcslot = %u",src_Item->GetProto()->Name1, src_bagIndex, src_slot);

    uint16 dest;
    InventoryResult msg = m_bot->CanEquipItem(NULL_SLOT, dest, src_Item, !src_Item->IsBag());
    if( msg != EQUIP_ERR_OK )
    {
        m_bot->SendEquipError( msg, src_Item, NULL );
        return;
    }

    uint16 src = src_Item->GetPos();
    if(dest == src)                                         // prevent equip in same slot, only at cheat
        return;

    Item *dest_Item = m_bot->GetItemByPos( dest );
    if( !dest_Item )                                         // empty slot, simple case
    {
        m_bot->RemoveItem(src_bagIndex, src_slot, true);
        m_bot->EquipItem(dest, src_Item, true);
        m_bot->AutoUnequipOffhandIfNeed();
    }
    else                                                    // have currently equipped item, not simple case
    {
        uint8 dest_bagIndex = dest_Item->GetBagSlot();
        uint8 dest_slot = dest_Item->GetSlot();

        msg = m_bot->CanUnequipItem( dest, false );
        if( msg != EQUIP_ERR_OK )
        {
            m_bot->SendEquipError( msg, dest_Item, NULL );
            return;
        }

        // check dest->src move possibility
        ItemPosCountVec sSrc;
        uint16 eSrc = 0;
        if( m_bot->IsInventoryPos( src ) )
        {
            msg = m_bot->CanStoreItem( src_bagIndex, src_slot, sSrc, dest_Item, true );
            if( msg != EQUIP_ERR_OK )
                msg = m_bot->CanStoreItem( src_bagIndex, NULL_SLOT, sSrc, dest_Item, true );
            if( msg != EQUIP_ERR_OK )
                msg = m_bot->CanStoreItem( NULL_BAG, NULL_SLOT, sSrc, dest_Item, true );
        }

        if( msg != EQUIP_ERR_OK )
        {
            m_bot->SendEquipError( msg, dest_Item, src_Item );
            return;
        }

        // now do moves, remove...
        m_bot->RemoveItem(dest_bagIndex, dest_slot, false);
        m_bot->RemoveItem(src_bagIndex, src_slot, false);

        // add to dest
        m_bot->EquipItem(dest, src_Item, true);

        // add to src
        if( m_bot->IsInventoryPos( src ) )
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
    DEBUG_LOG ("[PlayerbotAI]: TradeItem - slot=%d, hasTrader=%d, itemInTrade=%d, itemTradeable=%d",
                  slot,
                  (m_bot->GetTrader() ? 1 : 0),
                  (item.IsInTrade() ? 1 : 0),
                  (item.CanBeTraded() ? 1 : 0)
                  );

    if (!m_bot->GetTrader() || item.IsInTrade() || (!item.CanBeTraded() && slot != TRADE_SLOT_NONTRADED))
        return false;

    int8 tradeSlot = -1;

    TradeData* pTrade = m_bot->GetTradeData();
    if ((slot >= 0 && slot < TRADE_SLOT_COUNT) && pTrade->GetItem(TradeSlots(slot)) == NULL)
        tradeSlot = slot;
    else
        for (uint8 i = 0; i < TRADE_SLOT_TRADED_COUNT && tradeSlot == -1; i++)
        {
            if (pTrade->GetItem(TradeSlots(i)) == NULL)
            {
                tradeSlot = i;
                // reserve trade slot to allow multiple items to be traded
                pTrade->SetItem(TradeSlots(i), const_cast<Item*>(&item));
            }
        }

    if (tradeSlot == -1) return false;

    WorldPacket* const packet = new WorldPacket(CMSG_SET_TRADE_ITEM, 3);
    *packet << (uint8) tradeSlot << (uint8) item.GetBagSlot()
            << (uint8) item.GetSlot();
    m_bot->GetSession()->QueuePacket(packet);
    return true;
}

// submits packet to trade copper (trade window must be open)
bool PlayerbotAI::TradeCopper(uint32 copper)
{
    if (copper > 0)
    {
        WorldPacket* const packet = new WorldPacket(CMSG_SET_TRADE_GOLD, 4);
        *packet << copper;
        m_bot->GetSession()->QueuePacket(packet);
        return true;
    }
    return false;
}

bool PlayerbotAI::DoTeleport(WorldObject &obj)
{
    m_ignoreAIUpdatesUntilTime = time(0) + 6;
    PlayerbotChatHandler ch(GetMaster());
    if (!ch.teleport(*m_bot))
    {
        ch.sysmessage(".. could not be teleported ..");
        // DEBUG_LOG ("[PlayerbotAI]: DoTeleport - %s failed to teleport", m_bot->GetName() );
        return false;
    }
    return true;
}

void PlayerbotAI::HandleTeleportAck()
{
    m_ignoreAIUpdatesUntilTime = time(0) + 6;
    m_bot->GetMotionMaster()->Clear(true);
    if (m_bot->IsBeingTeleportedNear())
    {
        WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
        p.appendPackGUID(m_bot->GetObjectGuid());
        p << (uint32) 0; // supposed to be flags? not used currently
        p << (uint32) time(0); // time - not currently used
        m_bot->GetSession()->HandleMoveTeleportAckOpcode(p);
    }
    else if (m_bot->IsBeingTeleportedFar())
        m_bot->GetSession()->HandleMoveWorldportAckOpcode();
}

// Localization support
void PlayerbotAI::ItemLocalization(std::string& itemName, const uint32 itemID) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    ItemLocale const *pItemInfo = sObjectMgr.GetItemLocale(itemID);
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

    QuestLocale const *pQuestInfo = sObjectMgr.GetQuestLocale(questID);
    if (pQuestInfo)
        if (pQuestInfo->Title.size() > loc && !pQuestInfo->Title[loc].empty())
        {
            const std::string title = pQuestInfo->Title[loc];
            if (Utf8FitTo(title, wnamepart))
                questTitle = title.c_str();
        }
}

// Helper function for automatically selling poor quality items to the vendor
void PlayerbotAI::_doSellItem(Item* const item, std::ostringstream &report, std::ostringstream &canSell, uint32 &TotalCost, uint32 &TotalSold)
{
    if (!item)
        return;

    if (item->CanBeTraded() && item->GetProto()->Quality == ITEM_QUALITY_POOR)
    {
        uint32 cost = item->GetCount() * item->GetProto()->SellPrice;
        m_bot->ModifyMoney(cost);
        m_bot->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);
        m_bot->AddItemToBuyBackSlot(item);

        ++TotalSold;
        TotalCost += cost;

        report << "Sold ";
        MakeItemLink(item, report, true);
        report << " for ";

        uint32 gold = uint32(cost / 10000);
        cost -= (gold * 10000);
        uint32 silver = uint32(cost / 100);
        cost -= (silver * 100);

        if (gold > 0)
            report << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver > 0)
            report << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        report << cost << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t\n";
    }
    else if (item->GetProto()->SellPrice > 0)
        MakeItemLink(item, canSell, true);
}

bool PlayerbotAI::Withdraw(const uint32 itemid)
{
    Item* pItem = FindItemInBank(itemid);
    if(pItem)
    {
        std::ostringstream report;

        ItemPosCountVec dest;
        InventoryResult msg = m_bot->CanStoreItem( NULL_BAG, NULL_SLOT, dest, pItem, false );
        if( msg != EQUIP_ERR_OK )
        {
            m_bot->SendEquipError( msg, pItem, NULL );
            return true;
        }

        m_bot->RemoveItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->StoreItem( dest, pItem, true );

        report << "Withdrawn ";
        MakeItemLink(pItem, report, true);

        TellMaster(report.str());
    }

    return true; // item either withdrawn or not in bot bank
}

bool PlayerbotAI::Deposit(const uint32 itemid)
{
    Item* pItem = FindItem(itemid);
    if(pItem)
    {
        std::ostringstream report;

        ItemPosCountVec dest;
        InventoryResult msg = m_bot->CanBankItem( NULL_BAG, NULL_SLOT, dest, pItem, false );
        if( msg != EQUIP_ERR_OK )
        {
            m_bot->SendEquipError( msg, pItem, NULL );
            return true;
        }

        m_bot->RemoveItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->BankItem( dest, pItem, true );

        report << "Deposited ";
        MakeItemLink(pItem, report, true);

        TellMaster(report.str());
    }

    return true; // item either deposited or not in bot inventory
}

void PlayerbotAI::BankBalance()
{
    DEBUG_LOG("PlayerbotAI: BankBalance");

    std::ostringstream report;

    report << "In my bank\n ";
    report << "My item slots: ";

    for(uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_END; ++slot)
    {
        Item* const item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if(item)
            MakeItemLink(item, report, true);
    }
    TellMaster(report.str());

    // and each of my bank bags
    for(uint8 bag = BANK_SLOT_BAG_START; bag < BANK_SLOT_BAG_END; ++bag)
    {
        std::ostringstream goods;
        const Bag* const pBag = static_cast<Bag *>(m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
        if (pBag)
        {
            goods << "\nMy ";
            const ItemPrototype* const pBagProto = pBag->GetProto();
            std::string bagName = pBagProto->Name1;
            ItemLocalization(bagName, pBagProto->ItemId);
            goods << bagName << " slot: ";

            for(uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const item = m_bot->GetItemByPos(bag, slot);
                if(item)
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
        WorldPacket* const packet = new WorldPacket( MSG_TALENT_WIPE_CONFIRM, 8+4);    //you do not have any talent
        *packet << uint64(0);
        *packet << uint32(0);
        m_bot->GetSession()->QueuePacket(packet);
        return false;
    }

    m_bot->SendTalentsInfoData(false);
    trainer->CastSpell(m_bot, 14867, true);                  //spell: "Untalent Visual Effect"
    return true;
}

void PlayerbotAI::InspectUpdate()
{
    WorldPacket packet(SMSG_INSPECT_RESULTS, 50);
    packet << m_bot->GetPackGUID();
    m_bot->BuildPlayerTalentsInfoData(&packet);
    m_bot->BuildEnchantmentsInfoData(&packet);
    GetMaster()->GetSession()->SendPacket(&packet);
}

bool PlayerbotAI::Repair(const uint32 itemid, Creature* rCreature)
{
    Item* rItem = FindItem(itemid); // if item equipped or in bags
    uint8 IsInGuild = (m_bot->GetGuildId() != 0) ? uint8(1) : uint8(0);
    ObjectGuid itemGuid = (rItem) ? rItem->GetObjectGuid() : ObjectGuid();

    WorldPacket* const packet = new WorldPacket(CMSG_REPAIR_ITEM, 8+8+1);
    *packet << rCreature->GetObjectGuid();  // repair npc guid
    *packet << itemGuid; // if item specified then repair this, else repair all
    *packet << IsInGuild;  // guildbank yes=1 no=0
    m_bot->GetSession()->QueuePacket(packet);  // queue the packet to get around race condition

    return true;
}

bool PlayerbotAI::RemoveAuction(const uint32 auctionid)
{
    DEBUG_LOG("PlayerbotAI: RemoveAuction");

    QueryResult *result = CharacterDatabase.PQuery(
    "SELECT houseid,itemguid,item_template,itemowner,buyoutprice,time,buyguid,lastbid,startbid,deposit FROM auction WHERE id = '%u'",auctionid);

    AuctionEntry *auction;

    if(result)
    {
        Field *fields = result->Fetch();

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
        auction->auctionHouseEntry = NULL;                  // init later

        // check if sold item exists for guid
        // and item_template in fact (GetAItem will fail if problematic in result check in AuctionHouseMgr::LoadAuctionItems)
        Item* pItem = sAuctionMgr.GetAItem(auction->itemGuidLow);
        if (!pItem)
        {
            auction->DeleteFromDB();
            sLog.outError("Auction %u has not a existing item : %u, deleted", auction->Id, auction->itemGuidLow);
            delete auction;
            delete result;
            return true;
        }

        auction->auctionHouseEntry = sAuctionHouseStore.LookupEntry(houseid);

        // Attempt send item back to owner
        std::ostringstream msgAuctionCanceledOwner;
        msgAuctionCanceledOwner << auction->itemTemplate << ":0:" << AUCTION_CANCELED << ":0:0";

        // item will deleted or added to received mail list
        MailDraft(msgAuctionCanceledOwner.str(), "")    // TODO: fix body
            .AddItem(pItem)
            .SendMailTo(MailReceiver(ObjectGuid(HIGHGUID_PLAYER, auction->owner)), auction, MAIL_CHECK_MASK_COPIED);

        if(sAuctionMgr.RemoveAItem(auction->itemGuidLow))
            m_bot->GetSession()->SendAuctionCommandResult(auction, AUCTION_REMOVED, AUCTION_OK);

        auction->DeleteFromDB();

        delete auction;
        delete result;
    }

    return true; // remove auction item from list m_auction;
}

bool PlayerbotAI::ListAuctions()
{
    DEBUG_LOG("PlayerbotAI: ListAuctions");

    std::ostringstream report;

    QueryResult *result = CharacterDatabase.PQuery(
    "SELECT id,itemguid,item_template,time,buyguid,lastbid FROM auction WHERE itemowner = '%u'",m_bot->GetObjectGuid().GetCounter());
    if(result)
    {
        report << "My active auctions are: \n";
        do
        {
            Field *fields = result->Fetch();

            uint32 Id = fields[0].GetUInt32();
            uint32 itemGuidLow = fields[1].GetUInt32();
            uint32 itemTemplate = fields[2].GetUInt32();
            time_t expireTime = fields[3].GetUInt32();
            uint32 bidder = fields[4].GetUInt32();
            uint32 bid = fields[5].GetUInt32();

            // current time
            time_t currtime = time(NULL);
            time_t remtime = expireTime - currtime;

            tm* aTm = gmtime(&remtime);

            if(expireTime > currtime)
            {
                Item* aItem = sAuctionMgr.GetAItem(itemGuidLow);
                if(aItem)
                {
                    // Name
                    uint32 count = aItem->GetCount();
                    std::string name = aItem->GetProto()->Name1;
                    ItemLocalization(name, itemTemplate);
                    report << "\n|cffffffff|Htitle:" << Id << "|h[" << name;
                    if(count > 1)
                        report << "|cff00ff00x" << count << "|cffffffff" << "]|h|r";
                    else
                        report << "]|h|r";
                }

                if(bidder)
                {
                    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, bidder);
                    std::string bidder_name;
                    if(sObjectMgr.GetPlayerNameByGUID(guid, bidder_name))
                    report << " " << bidder_name << ": ";

                    uint32 gold = uint32(bid / 10000);
                    bid -= (gold * 10000);
                    uint32 silver = uint32(bid / 100);
                    bid -= (silver * 100);

                    if (gold > 0)
                        report << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                    if (silver > 0)
                        report << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                    report << bid << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";
                }
                if(aItem)
                    report << " ends: " << aTm->tm_hour << "|cff0070dd|hH|h|r " << aTm->tm_min << "|cff0070dd|hmin|h|r";
            }
        } while (result->NextRow());

        delete result;
        TellMaster(report.str().c_str());
    }

    return true; // auction either finished or does not exit
}

bool PlayerbotAI::AddAuction(const uint32 itemid, Creature* aCreature)
{
    DEBUG_LOG("PlayerbotAI: AddAuction");

    Item* aItem = FindItem(itemid);
    if(aItem)
    {
        std::ostringstream out;
        srand(time(NULL));
        uint32 duration[3] = { 720, 1440 ,2880 };  // 720 = 12hrs, 1440 = 24hrs, 2880 = 48hrs
        uint32 etime = duration[rand() % 3];

        uint32 min = urand(aItem->GetProto()->SellPrice * aItem->GetCount(),aItem->GetProto()->BuyPrice * aItem->GetCount()) * (aItem->GetProto()->Quality + 1);
        uint32 max = urand(aItem->GetProto()->SellPrice * aItem->GetCount(),aItem->GetProto()->BuyPrice * aItem->GetCount()) * (aItem->GetProto()->Quality + 1);

        out << "Auctioning ";
        MakeItemLink(aItem, out, true);
        out << " with " << aCreature->GetCreatureInfo()->Name;
        TellMaster(out.str().c_str());

        WorldPacket* const packet = new WorldPacket(CMSG_AUCTION_SELL_ITEM, 8+4+8+4+4+4+4 );
        *packet << aCreature->GetObjectGuid();     // auctioneer guid
        *packet << uint32(1);                      // const 1
        *packet << aItem->GetObjectGuid();         // item guid
        *packet << aItem->GetCount();      // stacksize
        *packet << uint32((min < max) ? min : max);  // starting bid
        *packet << uint32((max > min) ? max : min);  // buyout
        *packet << uint32(etime);  // auction duration

        m_bot->GetSession()->QueuePacket(packet);  // queue the packet to get around race condition
    }

    return true; // item either sold or not in bot inventory
}

bool PlayerbotAI::Sell(const uint32 itemid)
{
    Item* pItem = FindItem(itemid);
    if(pItem)
    {
        std::ostringstream report;

        uint32 cost = pItem->GetCount() * pItem->GetProto()->SellPrice;
        m_bot->ModifyMoney(cost);
        m_bot->MoveItemFromInventory(pItem->GetBagSlot(), pItem->GetSlot(), true);
        m_bot->AddItemToBuyBackSlot(pItem);

        report << "Sold ";
        MakeItemLink(pItem, report, true);
        report << " for ";

        uint32 gold = uint32(cost / 10000);
        cost -= (gold * 10000);
        uint32 silver = uint32(cost / 100);
        cost -= (silver * 100);

        if (gold > 0)
            report << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver > 0)
            report << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        report << cost << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";

        TellMaster(report.str());
    }

    return true; // item either sold or not in bot inventory
}

void PlayerbotAI::SellGarbage(bool verbose)
{
    uint32 TotalCost = 0;
    uint32 TotalSold = 0;
    std::ostringstream report, goods;

    goods << "Items that are not trash and can be sold: \n";
    goods << "In my main backpack:";
    // list out items in main backpack
    for(uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if(item)
            _doSellItem(item, report, goods, TotalCost, TotalSold);
    }
    if(verbose)
        TellMaster(goods.str());

    // and each of our other packs
    for(uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        std::ostringstream goods;
        const Bag* const pBag = static_cast<Bag *>(m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
        if (pBag)
        {
            goods << "\nIn my ";
            const ItemPrototype* const pBagProto = pBag->GetProto();
            std::string bagName = pBagProto->Name1;
            ItemLocalization(bagName, pBagProto->ItemId);
            goods << bagName << ":";

            for(uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const item = m_bot->GetItemByPos(bag, slot);
                if(item)
                    _doSellItem(item, report, goods, TotalCost, TotalSold);
            }
            if(verbose)
                TellMaster(goods.str());
        }
    }

    if (TotalSold > 0)
    {
        report << "Sold total " << TotalSold << " item(s) for ";
        uint32 gold = uint32(TotalCost / 10000);
        TotalCost -= (gold * 10000);
        uint32 silver = uint32(TotalCost / 100);
        TotalCost -= (silver * 100);

        if (gold > 0)
            report << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver > 0)
            report << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        report << TotalCost << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";

        TellMaster(report.str());
    }
    return;
}

void PlayerbotAI::GetTaxi(ObjectGuid guid, BotTaxiNode& nodes)
{
    DEBUG_LOG("[PlayerbotAI]: GetTaxi - %s node[0] %d node[1] %d", m_bot->GetName(), nodes[0], nodes[1]);

    Creature *unit = m_bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_FLIGHTMASTER);
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
    // ignore any messages from Addons
    if (text.empty() ||
        text.find("X-Perl") != std::wstring::npos ||
        text.find("HealBot") != std::wstring::npos ||
        text.find("LOOT_OPENED") != std::wstring::npos ||
        text.find("CTRA") != std::wstring::npos ||
        text.find("GathX") == 0) // Gatherer
        return;

    // if message is not from a player in the masters account auto reply and ignore
    if (!canObeyCommandFrom(fromPlayer))
    {
        std::string msg = "I can't talk to you. Please speak to my master ";
        msg += GetMaster()->GetName();
        SendWhisper(msg, fromPlayer);
        m_bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
    }

    // if in the middle of a trade, and player asks for an item/money
    else if (m_bot->GetTrader() && m_bot->GetTrader()->GetObjectGuid() == fromPlayer.GetObjectGuid())
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

    // if we are turning in a quest

    else if (text == "reset")
    {
        SetState(BOTSTATE_NORMAL);
        MovementReset();
        SetQuestNeedItems();
        UpdateAttackerInfo();
        m_lootTargets.clear();
        m_lootCurrent = ObjectGuid();
        m_targetCombat = 0;
    }
    else if (text == "report")
        SendQuestItemList(*GetMaster());
    else if (text == "orders")
        SendOrders(*GetMaster());
    else if (text == "follow" || text == "come")
        SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
    else if (text == "stay" || text == "stop")
        SetMovementOrder(MOVEMENT_STAY);
    else if (text == "attack")
    {
        ObjectGuid attackOnGuid = fromPlayer.GetSelectionGuid();
        if (attackOnGuid)
        {
            if (Unit* thingToAttack = ObjectAccessor::GetUnit(*m_bot, attackOnGuid))
                if (!m_bot->IsFriendlyTo(thingToAttack) && m_bot->IsWithinLOSInMap(thingToAttack))
                    GetCombatTarget(thingToAttack);
        }
        else
        {
            TellMaster("No target is selected.");
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }

    // handle cast command
    else if ((text.size() > 2 && text.substr(0, 2) == "c ") || (text.size() > 5 && text.substr(0, 5) == "cast "))
    {
        // sLog.outErrorDb("Selected link : %s", text.c_str());

        std::string spellStr = text.substr(text.find(" ") + 1);
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
        if (spellId != 0 && castOnGuid && m_bot->HasSpell(spellId))
        {
            m_spellIdCommand = spellId;
            m_targetGuidCommand = castOnGuid;
        }

    }

    // Handle selling items
    // sell [Item Link][Item Link] .. -- Sells bot(s) items from inventory
    else if (text.size() > 5 && text.substr(0, 5) == "sell ")
    {
        enum NPCFlags VENDOR_MASK = (enum NPCFlags) (UNIT_NPC_FLAG_VENDOR
			 | UNIT_NPC_FLAG_VENDOR_AMMO
			 | UNIT_NPC_FLAG_VENDOR_FOOD
			 | UNIT_NPC_FLAG_VENDOR_POISON
			 | UNIT_NPC_FLAG_VENDOR_REAGENT);

        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
            m_tasks.push_back(std::pair<enum TaskFlags,uint32>(SELL_ITEMS, *it));
        m_findNPC.push_back(VENDOR_MASK);
    }

    // Handle repair items
    // repair  all                      -- repair all bot(s) items
    // repair [Item Link][Item Link] .. -- repair select bot(s) items
    else if (text.size() >= 6 && text.substr(0, 6) == "repair")
    {
        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 6 && text.substr(0, 7) == "repair ")
            part = text.substr(7);  // Truncate 'repair ' part

        if (part.find(" ") > 0)
            subcommand = part.substr(0, part.find(" "));

        std::list<uint32> itemIds;
        extractItemIds(part, itemIds);
        for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
        {
            m_tasks.push_back(std::pair<enum TaskFlags,uint32>(REPAIR_ITEMS, *it));
            m_findNPC.push_back(UNIT_NPC_FLAG_REPAIR);
        }
        if(itemIds.empty() && subcommand == "all")
        {
            m_tasks.push_back(std::pair<enum TaskFlags,uint32>(REPAIR_ITEMS, 0));
            m_findNPC.push_back(UNIT_NPC_FLAG_REPAIR);
        }
    }

    // Handle auctions:
    // auction                                        -- Lists bot(s) active auctions.
    // auction add [Item Link][Item Link] ..          -- Create bot(s) active auction.
    // auction remove [Auction Link][Auction Link] .. -- Cancel bot(s) active auction. ([Auction Link] from auction)
    else if (text.size() >= 7 && text.substr(0, 7) == "auction")
    {
        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 7 && text.substr(0, 8) == "auction ")
            part = text.substr(8);  // Truncate 'auction ' part

        if (part.find(" ") > 0)
        {
            subcommand = part.substr(0, part.find(" "));
            if (part.size() > subcommand.size())
                part = part.substr(subcommand.size() + 1);
        }
        else
            subcommand = part;

        if (subcommand == "add" || subcommand == "remove")
        {
            if(subcommand == "add")
            {
                std::list<uint32> itemIds;
                extractItemIds(part, itemIds);
                for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
                    m_tasks.push_back(std::pair<enum TaskFlags,uint32>(ADD_AUCTION, *it));
                m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);
            }

            if(subcommand == "remove")
            {
                std::list<uint32> auctionIds;
                extractAuctionIds(part, auctionIds);
                for (std::list<uint32>::iterator it = auctionIds.begin(); it != auctionIds.end(); ++it)
                    m_tasks.push_back(std::pair<enum TaskFlags,uint32>(REMOVE_AUCTION, *it));
                m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);
            }
        }
        else // list all bot auctions
            m_findNPC.push_back(UNIT_NPC_FLAG_AUCTIONEER);
    }

    // Handle bank:
    // bank                                        -- Lists bot(s) bank balance.
    // bank deposit [Item Link][Item Link] ..      -- Deposit item(s) in bank.
    // bank withdraw [Item Link][Item Link] ..     -- Withdraw item(s) from bank. ([Item Link] from bank)
    else if (text.size() >= 4 && text.substr(0, 4) == "bank")
    {
        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 4 && text.substr(0, 5) == "bank ")
            part = text.substr(5);  // Truncate 'bank ' part

        if (part.find(" ") > 0)
        {
            subcommand = part.substr(0, part.find(" "));
            if (part.size() > subcommand.size())
                part = part.substr(subcommand.size() + 1);
        }
        else
            subcommand = part;

        if (subcommand == "deposit" || subcommand == "withdraw")
        {
            if(subcommand == "deposit")
            {
                std::list<uint32> itemIds;
                extractItemIds(part, itemIds);
                for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
                    m_tasks.push_back(std::pair<enum TaskFlags,uint32>(BANK_DEPOSIT, *it));
                m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);
            }

            if(subcommand == "withdraw")
            {
                std::list<uint32> itemIds;
                extractItemIds(part, itemIds);
                for (std::list<uint32>::iterator it = itemIds.begin(); it != itemIds.end(); ++it)
                    m_tasks.push_back(std::pair<enum TaskFlags,uint32>(BANK_WITHDRAW, *it));
                m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);
            }
        }
        else // list all bot balance
            m_findNPC.push_back(UNIT_NPC_FLAG_BANKER);
    }

    // Handle talents & glyphs:
    // talent                           -- Lists bot(s) active talents [TALENT LINK] & glyphs [GLYPH LINK], unspent points & cost to reset
    // talent learn [TALENT LINK] ..    -- Learn selected talent from bot client 'inspect' dialog -> 'talent' tab or from talent command (shift click icon/link)
    // talent reset                     -- Resets all talents
    else if (text.size() >= 6 && text.substr(0, 6) == "talent")
    {
        std::ostringstream out;
        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 6 && text.substr(0, 7) == "talent ")
            part = text.substr(7);  // Truncate 'talent ' part

        if (part.find(" ") > 0)
        {
            subcommand = part.substr(0, part.find(" "));
            if (part.size() > subcommand.size())
                part = part.substr(subcommand.size() + 1);
        }
        else
            subcommand = part;

        if (subcommand == "learn" || subcommand == "reset")
        {
            if(subcommand == "learn")
            {
                std::list<talentPair>talents;
                extractTalentIds(part, talents);

                for(std::list<talentPair>::iterator itr = talents.begin(); itr != talents.end(); ++itr)
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
            else if(subcommand == "reset")
            {
                m_tasks.push_back(std::pair<enum TaskFlags,uint32>(RESET_TALENTS, 0));
                m_findNPC.push_back(UNIT_NPC_FLAG_TRAINER_CLASS);
            }
        }
        else
        {
            uint32 gold = uint32(m_bot->resetTalentsCost()/ 10000);

            if (gold > 0)
                out << "Cost to reset all Talents is " << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";

            m_bot->MakeTalentGlyphLink(out);
            SendWhisper(out.str(), fromPlayer);
        }
    }

    // use items
    else if ((text.size() > 2 && text.substr(0, 2) == "u ") || (text.size() > 4 && text.substr(0, 4) == "use "))
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
            UseItem(*it);
    }

    // equip items
    else if ((text.size() > 2 && text.substr(0, 2) == "e ") || (text.size() > 6 && text.substr(0, 6) == "equip "))
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
            EquipItem(*it);
        InspectUpdate();
        SendNotEquipList(*m_bot);
    }

    // find project: 20:50 02/12/10 rev.4 item in world and wait until ordered to follow
    else if ((text.size() > 2 && text.substr(0, 2) == "f ") || (text.size() > 5 && text.substr(0, 5) == "find "))
    {
        extractGOinfo(text, m_lootTargets);

        m_lootCurrent = m_lootTargets.front();
        m_lootTargets.pop_front();

        GameObject *go = m_bot->GetMap()->GetGameObject(m_lootCurrent);
        if (!go)
        {
            m_lootTargets.clear();
            m_lootCurrent = ObjectGuid();
            return;
        }

        SetMovementOrder(MOVEMENT_STAY);
        m_bot->GetMotionMaster()->MovePoint(go->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
        m_lootTargets.clear();
        m_lootCurrent = ObjectGuid();
    }

    // get project: 20:50 02/12/10 rev.4 compact edition, handles multiple linked gameobject & improves visuals
    else if ((text.size() > 2 && text.substr(0, 2) == "g ") || (text.size() > 4 && text.substr(0, 4) == "get "))
    {
        extractGOinfo(text, m_lootTargets);
        SetState(BOTSTATE_LOOTING);
    }
    else if (text == "g" || text == "get") // get a selected lootable corpse
    {
        ObjectGuid getOnGuid = fromPlayer.GetSelectionGuid();
        if (getOnGuid)
        {
            Creature *c = m_bot->GetMap()->GetCreature(getOnGuid);
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
                TellMaster("Target is not lootable by me.");
        }
        else
        {
            TellMaster("No target is selected.");
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
        return;
    }

    // Handle all collection related commands here
    else if (text.size() >= 7 && text.substr(0, 7) == "collect")
    {
        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 7 && text.substr(0, 8) == "collect ")
            part = text.substr(8);  // Truncate 'collect ' part

        while (true)
        {
            if (part.find(" ") > 0)
            {
                subcommand = part.substr(0, part.find(" "));
                if (part.size() > subcommand.size())
                    part = part.substr(subcommand.size() + 1);
            }
            else
                subcommand = part;

            if (subcommand == "combat")
                SetCollectFlag(COLLECT_FLAG_COMBAT);
            else if (subcommand == "loot")
                SetCollectFlag(COLLECT_FLAG_LOOT);
            else if (subcommand == "quest")
                SetCollectFlag(COLLECT_FLAG_QUEST);
            else if (subcommand == "profession" || subcommand == "skill")
                SetCollectFlag(COLLECT_FLAG_PROFESSION);
            else if (subcommand == "skin" && m_bot->HasSkill(SKILL_SKINNING))
                SetCollectFlag(COLLECT_FLAG_SKIN);
            else if (subcommand == "objects" || subcommand == "nearby")
            {
                SetCollectFlag(COLLECT_FLAG_NEAROBJECT);
                if (!HasCollectFlag(COLLECT_FLAG_NEAROBJECT))
                    m_collectObjects.clear();
            }
            else if (subcommand == "none" || subcommand == "nothing")
            {
                m_collectionFlags = 0;
                m_collectObjects.clear();
            }
            else
            {
                std::string collout = "";
                if (m_bot->HasSkill(SKILL_SKINNING))
                    collout += ", skin";
                // TODO: perhaps change the command syntax, this way may be lacking in ease of use
                TellMaster("Collect <what>?: none, combat, loot, quest, profession, objects" + collout);
                break;
            }
            if (part == subcommand)
                break;
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
                collset += "  ";    // padding for substr
            collset += "nearby objects (";
            if (!m_collectObjects.empty())
            {
                std::string strobjects = "";
                for (BotLootEntry::iterator itr = m_collectObjects.begin(); itr != m_collectObjects.end(); ++itr)
                {
                    uint32 objectentry = *(itr);
                    GameObjectInfo const * gInfo = ObjectMgr::GetGameObjectInfo(objectentry);
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
            TellMaster("I'm collecting " + collset.substr(2));
        else
            TellMaster("I'm collecting nothing.");
    }

    else if (text == "quests")
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

                std::string questTitle  = pQuest->GetTitle();
                m_bot->GetPlayerbotAI()->QuestLocalization(questTitle, questId);

                if (m_bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE) {
                    hasCompleteQuests = true;
                    comout << " |cFFFFFF00|Hquest:" << questId << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                }
                else
                {
                    Item* qitem = FindItem(pQuest->GetSrcItemId());
                    if(qitem)
                        incomout << " use " << "|cffffffff|Hitem:" << qitem->GetProto()->ItemId << ":0:0:0:0:0:0:0" << "|h[" << qitem->GetProto()->Name1 << "]|h|r" << " on ";
                    hasIncompleteQuests = true;
                    incomout << " |cFFFFFF00|Hquest:" << questId << ':' << pQuest->GetQuestLevel() << "|h[" <<  questTitle << "]|h|r";
                }
            }
        }
        if (hasCompleteQuests)
            SendWhisper(comout.str(), fromPlayer);
        if (hasIncompleteQuests)
            SendWhisper(incomout.str(), fromPlayer);
        if (!hasCompleteQuests && !hasIncompleteQuests)
            SendWhisper("I have no quests!", fromPlayer);
    }

    // drop a quest
    else if (text.size() > 5 && text.substr(0, 5) == "drop ")
    {
        ObjectGuid oldSelectionGUID = ObjectGuid();
        if (fromPlayer.GetSelectionGuid() != m_bot->GetObjectGuid())
        {
            oldSelectionGUID = m_bot->GetObjectGuid();
            fromPlayer.SetSelectionGuid(oldSelectionGUID);
        }
        PlayerbotChatHandler ch(GetMaster());
        int8 linkStart = text.find("|");
        if (linkStart < 0 || !ch.dropQuest((char *) text.substr(linkStart).c_str()))
            ch.sysmessage("ERROR: could not drop quest");
        if (oldSelectionGUID)
            fromPlayer.SetSelectionGuid(oldSelectionGUID);
    }

    // Handle all pet related commands here
    else if (text.size() > 4 && text.substr(0, 4) == "pet ")
    {
        Pet * pet = m_bot->GetPet();
        if (!pet)
        {
            SendWhisper("I have no pet.", fromPlayer);
            return;
        }

        std::string part = text.substr(4); // Truncate `pet` part
        std::string subcommand = part.substr(0, part.find(" "));
        std::string argument;
        bool argumentFound = false;

        if (part.find(" ") != std::string::npos)
        {
            argument = part.substr(part.find(" ") + 1);
            if (argument.length() > 0)
                argumentFound = true;
        }

        if (subcommand == "react" && argumentFound)
        {
            if (argument == "a" || argument == "aggressive")
                pet->GetCharmInfo()->SetReactState(REACT_AGGRESSIVE);
            else if (argument == "d" || argument == "defensive")
                pet->GetCharmInfo()->SetReactState(REACT_DEFENSIVE);
            else if (argument == "p" || argument == "passive")
                pet->GetCharmInfo()->SetReactState(REACT_PASSIVE);
        }
        else if (subcommand == "state" && !argumentFound)
        {
            std::string state;
            switch (pet->GetCharmInfo()->GetReactState())
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
        else if (subcommand == "cast" && argumentFound)
        {
            uint32 spellId = (uint32) atol(argument.c_str());

            if (spellId == 0)
            {
                spellId = getPetSpellId(argument.c_str());
                if (spellId == 0)
                    extractSpellId(argument, spellId);
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
        else if (subcommand == "toggle" && argumentFound)
        {
            uint32 spellId = (uint32) atol(argument.c_str());

            if (spellId == 0)
            {
                spellId = getPetSpellId(argument.c_str());
                if (spellId == 0)
                    extractSpellId(argument, spellId);
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
        else if (subcommand == "spells" && !argumentFound)
        {
            int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

            std::ostringstream posOut;
            std::ostringstream negOut;

            for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
            {
                const uint32 spellId = itr->first;

                if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
                    continue;

                const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
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

    else if (text == "spells")
    {

        int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

        std::ostringstream posOut;
        std::ostringstream negOut;

        typedef std::map<std::string, uint32> spellMap;

        spellMap posSpells, negSpells;
        std::string spellName;

        uint32 ignoredSpells[] = {1843, 5019, 2479, 6603, 3365, 8386, 21651, 21652, 6233, 6246, 6247,
                                  61437, 22810, 22027, 45927, 7266, 7267, 6477, 6478, 7355, 68398};
        uint32 ignoredSpellsCount = sizeof(ignoredSpells) / sizeof(uint32);

        for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
        {
            const uint32 spellId = itr->first;

            if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
                continue;

            const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
            if (!pSpellInfo)
                continue;

            spellName = pSpellInfo->SpellName[loc];

            SkillLineAbilityMapBounds const bounds = sSpellMgr.GetSkillLineAbilityMapBounds(spellId);

            bool isProfessionOrRidingSpell = false;
            for (SkillLineAbilityMap::const_iterator skillIter = bounds.first; skillIter != bounds.second; ++skillIter)
            {
                if (IsProfessionOrRidingSkill(skillIter->second->skillId) && skillIter->first == spellId) {
                    isProfessionOrRidingSpell = true;
                    break;
                }
            }
            if (isProfessionOrRidingSpell)
                continue;

            bool isIgnoredSpell = false;
            for (uint8 i = 0; i < ignoredSpellsCount; ++i)
            {
                if (spellId == ignoredSpells[i]) {
                    isIgnoredSpell = true;
                    break;
                }
            }
            if (isIgnoredSpell)
                continue;

            if (IsPositiveSpell(spellId)) {
                if (posSpells.find(spellName) == posSpells.end())
                    posSpells[spellName] = spellId;
                else
                if (posSpells[spellName] < spellId)
                    posSpells[spellName] = spellId;
            }
            else
            {
                if (negSpells.find(spellName) == negSpells.end())
                    negSpells[spellName] = spellId;
                else
                if (negSpells[spellName] < spellId)
                    negSpells[spellName] = spellId;
            }
        }

        for (spellMap::const_iterator iter = posSpells.begin(); iter != posSpells.end(); ++iter)
        {
            posOut << " |cffffffff|Hspell:" << iter->second << "|h["
                   << iter->first << "]|h|r";
        }

        for (spellMap::const_iterator iter = negSpells.begin(); iter != negSpells.end(); ++iter)
        {
            negOut << " |cffffffff|Hspell:" << iter->second << "|h["
                   << iter->first << "]|h|r";
        }

        ChatHandler ch(&fromPlayer);
        SendWhisper("here's my non-attack spells:", fromPlayer);
        ch.SendSysMessage(posOut.str().c_str());
        SendWhisper("and here's my attack spells:", fromPlayer);
        ch.SendSysMessage(negOut.str().c_str());
    }

    // survey project: 20:50 02/12/10 rev.4 compact edition
    else if (text == "survey")
    {
        uint32 count = 0;
        std::ostringstream detectout;
        QueryResult *result;
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
                Field *fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                uint32 entry = fields[1].GetUInt32();

                GameObject *go = m_bot->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, entry, guid));
                if (!go)
                    continue;

                if (!go->isSpawned())
                    continue;

                detectout << "|cFFFFFF00|Hfound:" << guid << ":" << entry  << ":" <<  "|h[" << go->GetGOInfo()->name << "]|h|r";
                ++count;
            } while (result->NextRow());

            delete result;
        }
        SendWhisper(detectout.str().c_str(), fromPlayer);
    }

    // Handle class & professions training:
    // skill                           -- Lists bot(s) Primary profession skills.
    // skill train                     -- List available class or profession (Primary or Secondary) skills & spells, from selected trainer.
    // skill learn [HLINK][HLINK] ..   -- Learn selected skill and spells, from selected trainer ([HLINK] from skill train).
    // skill unlearn [HLINK][HLINK] .. -- Unlearn selected primary profession skill(s) and all associated spells ([HLINK] from skill)
    else if (text.size() >= 5 && text.substr(0, 5) == "skill")
    {
        uint32 rank[8] = {0, 75, 150, 225, 300, 375, 450, 525};

        std::ostringstream msg;

        std::string part = "";
        std::string subcommand = "";

        if (text.size() > 5 && text.substr(0, 6) == "skill ")
            part = text.substr(6);  // Truncate 'skill ' part

        if (part.find(" ") > 0)
        {
            subcommand = part.substr(0, part.find(" "));
            if (part.size() > subcommand.size())
                part = part.substr(subcommand.size() + 1);
        }
        else
            subcommand = part;

        if (subcommand == "train" || subcommand == "learn")
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

            Creature *creature =  m_bot->GetMap()->GetCreature(fromPlayer.GetSelectionGuid());
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
            if (!cSpells && !tSpells)
            {
                SendWhisper("No spells can be learnt from this trainer", fromPlayer);
                return;
            }

            // reputation discount
            float fDiscountMod =  m_bot->GetReputationPriceDiscount(creature);

            // Handle: Learning class or profession (primary or secondary) skill & spell(s) for selected trainer, skill learn [HLINK][HLINK][HLINK].. ([HLINK] from skill train)
            if (subcommand == "learn")
            {
                msg << "I have learnt the following spells:\r";
                uint32 totalSpellLearnt = 0;
                m_spellsToLearn.clear();
                extractSpellIdList(part, m_spellsToLearn);
                for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); advance(it, 1))
                {
                    uint32 spellId = *it;

                    if (!spellId)
                        break;

                    TrainerSpell const* trainer_spell = cSpells->Find(spellId);
                    if (!trainer_spell)
                        trainer_spell = tSpells->Find(spellId);

                    if (!trainer_spell || !trainer_spell->learnedSpell)
                        continue;

                    // apply reputation discount
                    uint32 cost = uint32(floor(trainer_spell->spellCost * fDiscountMod));
                    // check money requirement
                    if (m_bot->GetMoney() < cost)
                        continue;

                    m_bot->ModifyMoney(-int32(cost));
                    // learn explicitly or cast explicitly
                    if (trainer_spell->IsCastable())
                        m_bot->CastSpell(m_bot, trainer_spell->spell, true);
                    else
                        m_bot->learnSpell(spellId, false);
                    ++totalSpellLearnt;
                    totalCost += cost;
                    const SpellEntry *const pSpellInfo =  sSpellStore.LookupEntry(spellId);
                    if (!pSpellInfo)
                        continue;

                    MakeSpellLink(pSpellInfo, msg, &fromPlayer);
                    uint32 gold = uint32(cost / 10000);
                    cost -= (gold * 10000);
                    uint32 silver = uint32(cost / 100);
                    cost -= (silver * 100);
                    msg << " ";
                    if (gold > 0)
                        msg << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                    if (silver > 0)
                        msg << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                    msg << cost <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t\r";
                }
                uint32 gold = uint32(totalCost / 10000);
                totalCost -= (gold * 10000);
                uint32 silver = uint32(totalCost / 100);
                totalCost -= (silver * 100);
                msg << "Total of " << totalSpellLearnt << " spell";
                if (totalSpellLearnt != 1) msg << "s";
                msg << " learnt, ";
                if (gold > 0)
                    msg << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                if (silver > 0)
                    msg << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                msg << totalCost <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t spent.";
            }
            // Handle: List class or profession skill & spells for selected trainer, skill train
            else
            if (subcommand == "train")
            {
                msg << "The spells I can learn and their cost:\r";

                TrainerSpellData const* trainer_spells = cSpells;
                if (!trainer_spells)
                    trainer_spells = tSpells;

                for (TrainerSpellMap::const_iterator itr =  trainer_spells->spellList.begin(); itr !=  trainer_spells->spellList.end(); ++itr)
                {
                    TrainerSpell const* tSpell = &itr->second;

                    if (!tSpell)
                        break;

                    if (!tSpell->learnedSpell && !m_bot->IsSpellFitByClassAndRace(tSpell->learnedSpell))
                        continue;

                    if  (sSpellMgr.IsPrimaryProfessionFirstRankSpell(tSpell->learnedSpell) && m_bot->HasSpell(tSpell->learnedSpell))
                        continue;

                    TrainerSpellState state =  m_bot->GetTrainerSpellState(tSpell);
                    if (state != TRAINER_SPELL_GREEN)
                        continue;

                    uint32 spellId = tSpell->spell;
                    const SpellEntry *const pSpellInfo =  sSpellStore.LookupEntry(spellId);
                    if (!pSpellInfo)
                        continue;
                    uint32 cost = uint32(floor(tSpell->spellCost *  fDiscountMod));
                    totalCost += cost;

                    uint32 gold = uint32(cost / 10000);
                    cost -= (gold * 10000);
                    uint32 silver = uint32(cost / 100);
                    cost -= (silver * 100);
                    MakeSpellLink(pSpellInfo, msg, &fromPlayer);
                    msg << " ";
                    if (gold > 0)
                        msg << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                    if (silver > 0)
                        msg << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                    msg << cost <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t\r";
                }
                int32 moneyDiff = m_bot->GetMoney() - totalCost;
                if (moneyDiff >= 0)
                {
                    // calculate how much money bot has
                    uint32 gold = uint32(moneyDiff / 10000);
                    moneyDiff -= (gold * 10000);
                    uint32 silver = uint32(moneyDiff / 100);
                    moneyDiff -= (silver * 100);
                    msg << " ";
                    if (gold > 0)
                        msg << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                    if (silver > 0)
                        msg << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                    msg << moneyDiff <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t left.";
                }
                else
                {
                    moneyDiff *= -1;
                    uint32 gold = uint32(moneyDiff / 10000);
                    moneyDiff -= (gold * 10000);
                    uint32 silver = uint32(moneyDiff / 100);
                    moneyDiff -= (silver * 100);
                    msg << "I need ";
                    if (gold > 0)
                        msg << " " << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
                    if (silver > 0)
                        msg << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
                    msg << moneyDiff <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t more to learn all the  spells!";
                }
            }
        }
        // Handle: Unlearning selected primary profession skill(s) and all associated spells, skill unlearn [HLINK][HLINK].. ([HLINK] from skill)
        else
        if (subcommand == "unlearn")
        {
            m_spellsToLearn.clear();
            extractSpellIdList(part, m_spellsToLearn);
        }
        // Handle: Lists bot(s) primary profession skills, skill.
        else
        {
            m_spellsToLearn.clear();
            m_bot->skill(m_spellsToLearn);
            msg << "My Primary Professions are: ";
        }

        for (std::list<uint32>::iterator it = m_spellsToLearn.begin(); it != m_spellsToLearn.end(); ++it)
        {
            if (sSpellMgr.IsPrimaryProfessionSpell(*it) && subcommand != "learn")
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
            else
            if (IsPrimaryProfessionSkill(*it))
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);
                    if (!skillLine)
                        continue;

                    // has skill
                    if (skillLine->skillId == *it && skillLine->learnOnGetSkill == 0)
                    {
                        SpellEntry const* spellInfo = sSpellStore.LookupEntry(skillLine->spellId);
                        if (!spellInfo)
                            continue;

                        if (m_bot->GetSkillValue(*it) <= rank[sSpellMgr.GetSpellRank(skillLine->spellId)] && m_bot->HasSpell(skillLine->spellId))
                        {
                            // DEBUG_LOG ("[PlayerbotAI]: HandleCommand - skill (%u)(%u)(%u):",skillLine->spellId, rank[sSpellMgr.GetSpellRank(skillLine->spellId)], m_bot->GetSkillValue(*it));
                            MakeSpellLink(spellInfo, msg, &fromPlayer);
                            break;
                        }
                    }
                }
        }
        SendWhisper(msg.str(), fromPlayer);
        m_spellsToLearn.clear();
        m_bot->GetPlayerbotAI()->GetClassAI();
    }

    // stats project: 11:30 15/12/10 rev.2 display bot statistics
    else if (text == "stats")
    {
        std::ostringstream out;

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
            const Bag* const pBag = (Bag *) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
            if (pBag)
            {
                ItemPrototype const* pBagProto = pBag->GetProto();
                if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                    totalfree =  totalfree + pBag->GetFreeSlots();
            }

        }

        // estimate how much item damage the bot has
        uint32 copper = EstRepairAll();
        uint32 gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        uint32 silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|cffffffff[|h|cff00ffff" << m_bot->GetName() << "|h|cffffffff] has |cff00ff00";
        out << totalfree << " |h|cffffffff bag slots,|h" << " |cff00ff00";
        if (gold > 0)
            out << "|r|cff00ff00" << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver > 0)
            out << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        out << copper <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";

        // calculate how much money bot has
        copper = m_bot->GetMoney();
        gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|h|cffffffff item damage & has " << "|r|cff00ff00";
        if (gold > 0)
            out << gold <<  " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver > 0)
            out << silver <<  " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        out << copper <<  " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";
        ChatHandler ch(&fromPlayer);
        ch.SendSysMessage(out.str().c_str());
    }
    else
    {
        // if this looks like an item link, reward item it completed quest and talking to NPC
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        if (!itemIds.empty()) {
            uint32 itemId = itemIds.front();
            bool wasRewarded = false;
            ObjectGuid questRewarderGUID = m_bot->GetSelectionGuid();
            Object* const pNpc = (WorldObject *) m_bot->GetObjectByTypeMask(questRewarderGUID, TYPEMASK_CREATURE_OR_GAMEOBJECT);
            if (!pNpc)
                return;

            QuestMenu& questMenu = m_bot->PlayerTalkClass->GetQuestMenu();
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
                        ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        if (itemId == pRewardItem->ItemId)
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, pNpc, false);

                            std::string questTitle  = pQuest->GetTitle();
                            m_bot->GetPlayerbotAI()->QuestLocalization(questTitle, questID);
                            std::string itemName = pRewardItem->Name1;
                            m_bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);

                            std::ostringstream out;
                            out << "|cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r rewarded";
                            SendWhisper(out.str(), fromPlayer);
                            wasRewarded = true;
                        }
                    }
            }
        }
        else
        {
            std::string msg = "What? follow, stay, (c)ast <spellname>, spells, (e)quip <itemlink>, (u)se <itemlink>, drop <questlink>, report, quests, stats, collect";
            SendWhisper(msg, fromPlayer);
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }
}
