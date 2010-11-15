#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "../ItemPrototype.h"
#include "../World.h"
#include "../SpellMgr.h"
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
#include "../WorldPacket.h"
#include "../Spell.h"
#include "../Unit.h"
#include "../SpellAuras.h"
#include "../SharedDefines.h"
#include "Log.h"
#include "../GossipDef.h"

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
    bool revive(Player& botPlayer) { return HandleReviveCommand((char*) botPlayer.GetName()); }
    bool teleport(Player& botPlayer) { return HandleNamegoCommand((char*) botPlayer.GetName()); }
    void sysmessage(const char *str) { SendSysMessage(str); }
    bool dropQuest(char *str) { return HandleQuestRemoveCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot) :
    m_mgr(mgr), m_bot(bot), m_ignoreAIUpdatesUntilTime(0),
    m_combatOrder(ORDERS_NONE), m_ScenarioType(SCENARIO_PVEEASY),
    m_TimeDoneEating(0), m_TimeDoneDrinking(0),
    m_CurrentlyCastingSpellId(0), m_spellIdCommand(0),
    m_targetGuidCommand(0), m_classAI(0)
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

    // start following master (will also teleport bot to master)
    SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());

    // get class specific ai
    switch (m_bot->getClass())
    {
        case CLASS_PRIEST:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(GetMaster(), m_bot, this);
            break;
        case CLASS_MAGE:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(GetMaster(), m_bot, this);
            break;
        case CLASS_WARLOCK:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(GetMaster(), m_bot, this);
            break;
        case CLASS_WARRIOR:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(GetMaster(), m_bot, this);
            break;
        case CLASS_SHAMAN:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(GetMaster(), m_bot, this);
            break;
        case CLASS_PALADIN:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(GetMaster(), m_bot, this);
            break;
        case CLASS_ROGUE:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(GetMaster(), m_bot, this);
            break;
        case CLASS_DRUID:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(GetMaster(), m_bot, this);
            break;
        case CLASS_HUNTER:
            m_combatStyle = COMBAT_RANGED;
            m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(GetMaster(), m_bot, this);
            break;
        case CLASS_DEATH_KNIGHT:
            m_combatStyle = COMBAT_MELEE;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(GetMaster(), m_bot, this);
            break;
    }
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
        sLog.outDebug("Playerbot spell init: %s is %u", pSpellInfo->SpellName[0], spellId);

        // Add spell to spellrange map
        Spell *spell = new Spell(m_bot, pSpellInfo, false);
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange, IsPositiveSpell(spellId));
        m_bot->ApplySpellMod(spellId, SPELLMOD_RANGE, range, spell);
        m_spellRangeMap.insert(std::pair<uint32,float>(spellId, range));
        delete spell;
    }
    return (next == 0) ? spellId : next;
}

/*
 * Send a list of equipment that is in bot's inventor that is currently unequipped.
 * This is called when the master is inspecting the bot.
 */

void PlayerbotAI::SendNotEquipList(Player& player)
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
        uint8 equipSlot = uint8(dest);
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
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 equipSlot;
                uint8 msg = m_bot->CanEquipItem(NULL_SLOT, equipSlot, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;
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

void PlayerbotAI::SendQuestItemList(Player& player)
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

void PlayerbotAI::SendOrders(Player& player)
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
            uint64 flagGuid;
            p >> flagGuid;
            uint64 playerGuid;
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

        case SMSG_INVENTORY_CHANGE_FAILURE:
        {
            TellMaster("I can't use that.");
            return;
        }
        case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            uint8 castCount;
            uint32 spellId;

            uint64 casterGuid = p.readPackGUID();
            if (casterGuid != m_bot->GetGUID())
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
            uint64 guid = p.readPackGUID();
            if (guid != GetMaster()->GetGUID())
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
            uint64 guid = p.readPackGUID();
            if (guid != m_bot->GetGUID())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            //m_bot->SetSpeed(MOVE_RUN, GetMaster()->GetSpeed(MOVE_FLIGHT) +0.1f, true);
            return;
        }

        // handle dismount flying acknowledgement
        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            uint64 guid = p.readPackGUID();
            if (guid != m_bot->GetGUID())
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
                if (m_bot->GetGroup()->IsMember(GetMaster()->GetGUID()))
                {
                    p.resize(8);
                    p << GetMaster()->GetGUID();
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

                // list out items in main backpack
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem && pItem->CanBeTraded())
                    {
                        const ItemPrototype* const pItemProto = pItem->GetProto();

                        std::string itemName = pItemProto->Name1;
                        ItemLocalization(itemName, pItemProto->ItemId);

                        out << " |cffffffff|Hitem:" << pItemProto->ItemId
                            << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        if (pItem->GetCount() > 1)
                            out << "x" << pItem->GetCount() << ' ';
                    }
                }
                // list out items in other removable backpacks
                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem && pItem->CanBeTraded())
                            {
                                const ItemPrototype* const pItemProto = pItem->GetProto();

                                std::string itemName = pItemProto->Name1;
                                ItemLocalization(itemName, pItemProto->ItemId);

                                // item link format: http://www.wowwiki.com/ItemString
                                // itemId, enchantId, jewelId1, jewelId2, jewelId3, jewelId4, suffixId, uniqueId
                                out << " |cffffffff|Hitem:" << pItemProto->ItemId
                                    << ":0:0:0:0:0:0:0" << "|h[" << itemName
                                    << "]|h|r";
                                if (pItem->GetCount() > 1)
                                    out << "x" << pItem->GetCount() << ' ';
                            }
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
                        << "|r|cffffd333c|r" << " and the following items:";
                SendWhisper(whisper.str().c_str(), *(m_bot->GetTrader()));
                ChatHandler ch(m_bot->GetTrader());
                ch.SendSysMessage(out.str().c_str());
            }
            return;
        }

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);
            uint64 castItemGuid = p.readPackGUID();
            uint64 casterGuid = p.readPackGUID();
            if (casterGuid != m_bot->GetGUID())
                return;

            uint32 spellId;
            p >> spellId;
            uint16 castFlags;
            p >> castFlags;
            uint32 msTime;
            p >> msTime;
            uint8 numHit;
            p >> numHit;

            if (m_CurrentlyCastingSpellId == spellId)
            {
                Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
                if (!pSpell)
                    return;

                if (pSpell->IsChannelActive() || pSpell->IsAutoRepeat())
                    m_ignoreAIUpdatesUntilTime = time(0) + (GetSpellDuration(pSpell->m_spellInfo) / 1000) + 1;
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

                WorldPacket* const packet = new WorldPacket(CMSG_RESURRECT_RESPONSE, 8+1);
                *packet << guid;
                *packet << uint8(1);                        // accept
                m_bot->GetSession()->QueuePacket(packet);   // queue the packet to get around race condition

                // set back to normal
                SetState(BOTSTATE_NORMAL);
                SetIgnoreUpdateTime(0);
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

//typedef std::pair<uint32, uint8> spellEffectPair;
//typedef std::multimap<spellEffectPair, Aura*> AuraMap;

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
            UseItem(*pItem);
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
            UseItem(*pItem);
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
        SetQuestNeedItems();
        m_lootCreature.clear();
        m_lootCurrent = 0;
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
    m_lootCreature.push_back(m_targetCombat->GetGUID());

    // set movement generators for combat movement
    MovementClear();
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
    m_lootCreature.clear();
    m_lootCurrent = 0;

    // run through accepted quests, get quest infoand data
    for (QuestStatusMap::iterator iter = m_bot->getQuestStatusMap().begin(); iter != m_bot->getQuestStatusMap().end(); ++iter)
    {
        const Quest *qInfo = sObjectMgr.GetQuestTemplate(iter->first);
        if (!qInfo)
            continue;

        QuestStatusData *qData = &iter->second;
        // only check quest if it is incomplete
        if (qData->m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        // check for items we not have enough of
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
        {
            if (!qInfo->ReqItemCount[i] || (qInfo->ReqItemCount[i] - qData->m_itemcount[i]) <= 0)
                continue;
            m_needItemList[qInfo->ReqItemId[i]] = (qInfo->ReqItemCount[i] - qData->m_itemcount[i]);
        }
    }
}

void PlayerbotAI::SetState(BotState state)
{
    //sLog.outDebug( "[PlayerbotAI]: %s switch state %d to %d", m_bot->GetName(), m_botState, state );
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
        Bag* pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pBag && pBag->GetProto()->BagFamily == BAG_FAMILY_MASK_NONE)
            space += pBag->GetFreeSlots();
    }
    return space;
}

void PlayerbotAI::DoLoot()
{
    if (!m_lootCurrent && m_lootCreature.empty())
    {
        //sLog.outDebug( "[PlayerbotAI]: %s reset loot list / go back to idle", m_bot->GetName() );
        m_botState = BOTSTATE_NORMAL;
        SetQuestNeedItems();
        return;
    }

    if (!m_lootCurrent)
    {
        m_lootCurrent = m_lootCreature.front();
        m_lootCreature.pop_front();
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        // check if we got a creature and if it is still a corpse, otherwise bot runs to spawn point
        if (!c || c->getDeathState() != CORPSE || GetMaster()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        m_bot->GetMotionMaster()->MovePoint(c->GetMapId(), c->GetPositionX(), c->GetPositionY(), c->GetPositionZ());
        //sLog.outDebug( "[PlayerbotAI]: %s is going to loot '%s' deathState=%d", m_bot->GetName(), c->GetName(), c->getDeathState() );
    }
    else
    {
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        if (!c || c->getDeathState() != CORPSE || GetMaster()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        if (m_bot->IsWithinDistInMap(c, INTERACTION_DISTANCE))
        {
            // check for needed items
            m_bot->SendLoot(m_lootCurrent, LOOT_CORPSE);
            Loot *loot = &c->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
            //sLog.outDebug( "[PlayerbotAI]: %s looting: '%s' got %d items", m_bot->GetName(), c->GetName(), loot->GetMaxSlotInLootFor( m_bot ) );
            for (uint32 l = 0; l < lootNum; l++)
            {
                QuestItem *qitem = 0, *ffaitem = 0, *conditem = 0;
                LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
                if (!item)
                    continue;

                if (!qitem && item->is_blocked)
                {
                    m_bot->SendLootRelease(m_bot->GetLootGUID());
                    continue;
                }

                if (m_needItemList[item->itemid] > 0)
                {
                    //sLog.outDebug( "[PlayerbotAI]: %s looting: needed item '%s'", m_bot->GetName(), sObjectMgr.GetItemLocale(item->itemid)->Name );
                    ItemPosCountVec dest;
                    if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
                    {
                        Item * newitem = m_bot->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);

                        if (qitem)
                        {
                            qitem->is_looted = true;
                            if (item->freeforall || loot->GetPlayerQuestItems().size() == 1)
                                m_bot->SendNotifyLootItemRemoved(l);
                            else
                                loot->NotifyQuestItemRemoved(qitem->index);
                        }
                        else
                        {
                            if (ffaitem)
                            {
                                ffaitem->is_looted = true;
                                m_bot->SendNotifyLootItemRemoved(l);
                            }
                            else
                            {
                                if (conditem)
                                    conditem->is_looted = true;
                                loot->NotifyItemRemoved(l);
                            }
                        }
                        if (!item->freeforall)
                            item->is_looted = true;
                        --loot->unlootedCount;
                        m_bot->SendNewItem(newitem, uint32(item->count), false, false, true);
                        m_bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item->itemid, item->count);
                    }
                }
            }
            // release loot
            // if( uint64 lguid = m_bot->GetLootGUID() && m_bot->GetSession() )
            m_bot->GetSession()->DoLootRelease(m_lootCurrent);
            //else if( !m_bot->GetSession() )
            //    sLog.outDebug( "[PlayerbotAI]: %s has no session. Cannot release loot!", m_bot->GetName() );

            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear();
            m_bot->GetMotionMaster()->MoveIdle();
            SetQuestNeedItems();
            //sLog.outDebug( "[PlayerbotAI]: %s looted target 0x%08X", m_bot->GetName(), m_lootCurrent );
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
        m_bot->SetDivider(0);
        return;
    }

    if (!m_bot->CanTakeQuest(qInfo, false))
    {
        // can't take quest
        m_bot->SetDivider(0);
        return;
    }

    if (m_bot->GetDivider() != 0)
    {
        // send msg to quest giving player
        pGiver->SendPushToPartyResponse(m_bot, QUEST_PARTY_MSG_ACCEPT_QUEST);
        m_bot->SetDivider(0);
    }

    if (m_bot->CanAddQuest(qInfo, false))
    {
        m_bot->AddQuest(qInfo, pGiver);

        if (m_bot->CanCompleteQuest(quest))
            m_bot->CompleteQuest(quest);

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
        m_bot->PrepareQuestMenu(giverGUID.GetRawValue());
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
    bool inCombat = false;
    inCombat |= m_bot->isInCombat();
    inCombat |= GetMaster()->isInCombat();
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            inCombat |= ref->getSource()->isInCombat();
            ref = ref->next();
        }
    }
    return inCombat;
}

void PlayerbotAI::UpdateAttackerInfo()
{
    // clear old list
    m_attackerInfo.clear();

    // check own attackers
    HostileReference *ref = m_bot->getHostileRefManager().getFirst();
    while (ref)
    {
        ThreatManager *target = ref->getSource();
        uint64 guid = target->getOwner()->GetGUID();
        m_attackerInfo[guid].attacker = target->getOwner();
        m_attackerInfo[guid].victim = target->getOwner()->getVictim();
        m_attackerInfo[guid].threat = target->getThreat(m_bot);
        m_attackerInfo[guid].count = 1;
        m_attackerInfo[guid].source = 1;
        ref = ref->next();
    }

    // check master's attackers
    ref = GetMaster()->getHostileRefManager().getFirst();
    while (ref)
    {
        ThreatManager *target = ref->getSource();
        uint64 guid = target->getOwner()->GetGUID();
        if (m_attackerInfo.find(guid) == m_attackerInfo.end())
        {
            m_attackerInfo[guid].attacker = target->getOwner();
            m_attackerInfo[guid].victim = target->getOwner()->getVictim();
            m_attackerInfo[guid].count = 0;
            m_attackerInfo[guid].source = 2;
        }
        m_attackerInfo[guid].threat = target->getThreat(m_bot);
        m_attackerInfo[guid].count++;
        ref = ref->next();
    }

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
            ref = gref->getSource()->getHostileRefManager().getFirst();
            while (ref)
            {
                ThreatManager *target = ref->getSource();
                uint64 guid = target->getOwner()->GetGUID();
                if (m_attackerInfo.find(guid) == m_attackerInfo.end())
                {
                    m_attackerInfo[guid].attacker = target->getOwner();
                    m_attackerInfo[guid].victim = target->getOwner()->getVictim();
                    m_attackerInfo[guid].count = 0;
                    m_attackerInfo[guid].source = 3;
                }
                m_attackerInfo[guid].threat = target->getThreat(m_bot);
                m_attackerInfo[guid].count++;
                ref = ref->next();
            }
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

Unit *PlayerbotAI::FindAttacker(ATTACKERINFOTYPE ait, Unit *victim)
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
        if (!m_followTarget) return;

        // target player is teleporting...
        if (m_followTarget->GetTypeId() == TYPEID_PLAYER && ((Player*) m_followTarget)->IsBeingTeleported())
            return;

        // check if bot needs to teleport to reach target...
        if (!m_bot->isInCombat())
        {
            if (m_followTarget->GetTypeId() == TYPEID_PLAYER && ((Player*) m_followTarget)->GetCorpse())
            {
                if (!FollowCheckTeleport(*((Player*) m_followTarget)->GetCorpse())) return;
            }
            else if (!FollowCheckTeleport(*m_followTarget)) return;
        }

        if (m_bot->isAlive())
        {
            float angle = rand_float(0, M_PI_F);
            float dist = rand_float(m_mgr->m_confFollowDistance[0], m_mgr->m_confFollowDistance[1]);
            m_bot->GetMotionMaster()->MoveFollow(m_followTarget, dist, angle);
        }
    }
}

void PlayerbotAI::MovementUpdate()
{
    // send heartbeats to world
    // m_bot->SendHeartBeat(false);

    // call set position (updates states, exploration, etc.)
    m_bot->SetPosition(m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), m_bot->GetOrientation(), false);
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
    m_bot->m_movementInfo.ChangePosition(x,y,z,ori);

    m_bot->SendHeartBeat(false);
}

// some possible things to use in AI
//GetRandomContactPoint
//GetPower, GetMaxPower
// HasSpellCooldown
// IsAffectedBySpellmod
// isMoving
// hasUnitState(FLAG) FLAG like: UNIT_STAT_ROOT, UNIT_STAT_CONFUSED, UNIT_STAT_STUNNED
// hasAuraType

void PlayerbotAI::UpdateAI(const uint32 p_time)
{
    if (m_bot->IsBeingTeleported() || m_bot->GetTrader())
        return;

    time_t currentTime = time(0);
    if (currentTime < m_ignoreAIUpdatesUntilTime)
        return;

    // default updates occur every two seconds
    m_ignoreAIUpdatesUntilTime = time(0) + 2;

    // send heartbeat
    MovementUpdate();

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            //sLog.outDebug( "[PlayerbotAI]: %s died and is not in correct state...", m_bot->GetName() );
            // clear loot list on death
            m_lootCreature.clear();
            m_lootCurrent = 0;
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
                //sLog.outDebug( "[PlayerbotAI]: %s already has a corpse...", m_bot->GetName() );
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
                //sLog.outDebug( "[PlayerbotAI]: %s has no corpse!", m_bot->GetName() );
                return;
             // teleport ghost from graveyard to corpse
            //sLog.outDebug( "[PlayerbotAI]: Teleport %s to corpse...", m_bot->GetName() );
            FollowCheckTeleport(*corpse);
            // check if we are allowed to resurrect now
            if (corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(0))
            {
                m_ignoreAIUpdatesUntilTime = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP);
                //sLog.outDebug( "[PlayerbotAI]: %s has to wait for %d seconds to revive...", m_bot->GetName(), m_ignoreAIUpdatesUntilTime-time(0) );
                return;
            }
            // resurrect now
            //sLog.outDebug( "[PlayerbotAI]: Reviving %s to corpse...", m_bot->GetName() );
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
        // if we are casting a spell then interrupt it
        // make sure any actions that cast a spell set a proper m_ignoreAIUpdatesUntilTime!
        Spell* const pSpell = GetCurrentSpell();
        if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()))
            InterruptCurrentCastingSpell();

        // direct cast command from master
        else if (m_spellIdCommand != 0)
        {
            Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, m_targetGuidCommand);
            if (pTarget != NULL)
                CastSpell(m_spellIdCommand, *pTarget);
            m_spellIdCommand = 0;
            m_targetGuidCommand = 0;
        }

        // handle combat (either self/master/group in combat, or combat state and valid target)
        else if (IsInCombat() || (m_botState == BOTSTATE_COMBAT && m_targetCombat))
            DoNextCombatManeuver();

        // bot was in combat recently - loot now
        else if (m_botState == BOTSTATE_COMBAT)
        {
            SetState(BOTSTATE_LOOTING);
            m_attackerInfo.clear();
            m_ignoreAIUpdatesUntilTime = time(0);
        }
        else if (m_botState == BOTSTATE_LOOTING)
        {
            DoLoot();
            m_ignoreAIUpdatesUntilTime = time(0);
        }
/*
        // are we sitting, if so feast if possible
        else if (m_bot->getStandState() == UNIT_STAND_STATE_SIT)
        Feast();
 */
        // if commanded to follow master and not already following master then follow master
        else if (!m_bot->isInCombat() && !IsMoving())
            MovementReset();

        // do class specific non combat actions
        else if (GetClassAI() && !m_bot->IsMounted())
            (GetClassAI())->DoNonCombatActions();
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
    size_t temp_len = vsnprintf(temp_buf, 1024, fmt, ap);
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
        TellMaster("missing spell entry in CastSpell for spellid %u.", spellId);
        return false;
    }

    // set target
    ObjectGuid targetGUID = m_bot->GetSelectionGuid();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);

    if (!pTarget)
        pTarget = m_bot;

    // Check spell range
    std::map<uint32, float>::iterator it = m_spellRangeMap.find(spellId);
    if (it != m_spellRangeMap.end() && (int)it->second != 0)
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
        sLog.outDebug("Bot movement reset for casting %s (%u)", pSpellInfo->SpellName[0], spellId);
        MovementClear();
    }

    // actually cast spell
    m_bot->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    m_CurrentlyCastingSpellId = spellId;
    m_ignoreAIUpdatesUntilTime = time(0) + (int32) ((float) pSpell->GetCastTime() / 1000.0f) + 1;

    // if this caused the caster to move (blink) update the position
    // I think this is normally done on the client
    // this should be done on spell success
    /*
       if (name == "Blink") {
       float x,y,z;
       m_bot->GetPosition(x,y,z);
       m_bot->GetNearPoint(m_bot, x, y, z, 1, 5, 0);
       m_bot->Relocate(x,y,z);
       m_bot->SendHeartBeat(true);

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
        {
            pet->SetInFront(pTarget);
            MovementUpdate();
        }
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
            if ((*it).second->GetCasterGUID() == m_bot->GetGUID() && GetSpellSpecific((*it).second->GetId()) == SpellSpecific(spec))
                return false;
    }
    return true;
}

Item* PlayerbotAI::FindItem(uint32 ItemId)
{
    // list out items in main backpack
    //INVENTORY_SLOT_ITEM_START = 23
    //INVENTORY_SLOT_ITEM_END = 39

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        // sLog.outDebug("[%s's]backpack slot = %u",m_bot->GetName(),slot); // 23 to 38 = 16
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
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);   // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                sLog.outDebug("[%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);  // 1 to bagsize = ?
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

bool PlayerbotAI::PickPocket(Unit* pTarget)
{
    bool looted = false;

    ObjectGuid markGuid = pTarget->GetObjectGuid();
    Creature *c = m_bot->GetMap()->GetCreature(markGuid);
    m_bot->SendLoot(markGuid, LOOT_PICKPOCKETING);
    Loot *loot = &c->loot;
    uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);

    if (m_mgr->m_confDebugWhisper)
    {
        std::ostringstream out;

        // calculate how much money bot loots
        uint32 copper = loot->gold;
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

    if (loot->gold)
    {
        m_bot->ModifyMoney( loot->gold );
        m_bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, loot->gold);
        loot->gold = 0;
        loot->NotifyMoneyRemoved();
    }

    for (uint32 l = 0; l < lootNum; l++)
    {
        QuestItem *qitem = 0, *ffaitem = 0, *conditem = 0;
        LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
        if (!item)
            continue;

        ItemPosCountVec dest;
        if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
        {
            Item* pItem = m_bot->StoreNewItem (dest, item->itemid, true, item->randomPropertyId);
            m_bot->SendNewItem(pItem, uint32(item->count), false, false, true);
            --loot->unlootedCount;
            looted = true;
        }
    }
    // release loot
    if (looted)
        m_bot->GetSession()->DoLootRelease(markGuid);

    return false; // ensures that the rogue only pick pockets target once
}

bool PlayerbotAI::HasPick()
{
    if (m_bot->HasItemTotemCategory(TC_MINING_PICK))
        return true;

    std::ostringstream out;
    out << "|cffffffffI do not have a pick!";
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
        if(pSpellInfo->Reagent[i] <= 0)
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

bool PlayerbotAI::extractSpellId(const std::string& text, uint32 &spellId) const
{

    //   Link format
    //   |cffffffff|Hspell:" << spellId << ":" << "|h[" << pSpellInfo->SpellName[loc] << "]|h|r";
    //   cast |cff71d5ff|Hspell:686|h[Shadow Bolt]|h|r";
    //   012345678901234567890123456
    //        base = 16 >|  +7 >|

    uint8 pos = 0;

    int i = text.find("Hspell:", pos);
    if (i == -1)
        return false;

    // DEBUG_LOG("extractSpellId first pos %u i %u",pos,i);
    pos = i + 7;     // start of window in text 16 + 7 = 23
    int endPos = text.find('|', pos);
    if (endPos == -1)
        return false;

    // DEBUG_LOG("extractSpellId second endpos : %u pos : %u",endPos,pos);
    std::string idC = text.substr(pos, endPos - pos);     // 26 - 23
    spellId = atol(idC.c_str());
    pos = endPos;     // end
    return true;
}

bool PlayerbotAI::extractGOinfo(const std::string& text, uint32 &guid, uint32 &entry, int &mapid, float &x, float &y, float &z) const
{

    //    Link format
    //    |cFFFFFF00|Hfound:" << guid << ':'  << entry << ':' << x << ':' << y << ':' << z  << ':' << mapid << ':' <<  "|h[" << gInfo->name << "]|h|r";
    //    |cFFFFFF00|Hfound:5093:1731:-9295:-270:81.874:0:|h[Copper Vein]|h|r

    uint8 pos = 0;

    // extract GO guid
    int i = text.find("Hfound:", pos);     // base H = 11
    if (i == -1)     // break if error
        return false;

    pos = i + 7;     //start of window in text 11 + 7 = 18
    int endPos = text.find(':', pos);     // end of window in text 22
    if (endPos == -1)     //break if error
        return false;
    std::string guidC = text.substr(pos, endPos - pos);     // get string within window i.e guid 22 - 18 =  4
    guid = atol(guidC.c_str());     // convert ascii to long int

    // extract GO entry
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string entryC = text.substr(pos, endPos - pos);     // get string within window i.e entry
    entry = atol(entryC.c_str());     // convert ascii to float

    // extract GO x
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string xC = text.substr(pos, endPos - pos);     // get string within window i.e x
    x = atof(xC.c_str());     // convert ascii to float

    // extract GO y
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string yC = text.substr(pos, endPos - pos);     // get string within window i.e y
    y = atof(yC.c_str());     // convert ascii to float

    // extract GO z
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string zC = text.substr(pos, endPos - pos);     // get string within window i.e z
    z = atof(zC.c_str());     // convert ascii to float

    //extract GO mapid
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string mapidC = text.substr(pos, endPos - pos);     // get string within window i.e mapid
    mapid = atoi(mapidC.c_str());     // convert ascii to int
    pos = endPos;     // end
    return true;
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

                foundItemList.push_back(pItem);
                itemIdSearchList.erase(it);
                break;
            }
        }
    }
}

// submits packet to use an item
void PlayerbotAI::UseItem(Item& item, uint8 targetSlot)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();
    uint8 cast_count = 1;
    uint32 spellid;
    uint64 item_guid = item.GetGUID();
    uint32 glyphIndex = 0; // ??
    uint8 unk_flags = 0;  // not 0x02
    uint32 targetFlag;

    WorldPacket* packet;

    // create target data
    // note other targets are possible but not supported at the moment
    // see SpellCastTargets::read in Spell.cpp to see other options
    // for setting target

    if (targetSlot < EQUIPMENT_SLOT_END)
    {
        targetFlag = TARGET_FLAG_ITEM;
        Item* const targetItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetSlot);
        PackedGuid targetGUID = targetItem->GetObjectGuid().WriteAsPacked();
        spellid = item.GetProto()->Spells[0].SpellId;
        packet = new WorldPacket(CMSG_USE_ITEM, 1 + 1 + 1 + 4 + 8 + 4 + 1 + 8);
        *packet << bagIndex << slot << cast_count << spellid << item_guid
                << glyphIndex << unk_flags << targetFlag << targetGUID;
    }
    else
    {
        targetFlag = TARGET_FLAG_SELF;
        spellid = 0;
        packet = new WorldPacket(CMSG_USE_ITEM, 1 + 1 + 1 + 4 + 8 + 4 + 1);
        *packet << bagIndex << slot << cast_count << spellid << item_guid
                << glyphIndex << unk_flags << targetFlag;
    }

    m_bot->GetSession()->QueuePacket(packet); // queue the packet to get around race condition

    // certain items cause player to sit (food,drink)
    // tell bot to stop following if this is the case
    // (doesn't work since we queued the packet!)
    // maybe its not needed???
    //if (! m_bot->IsStandState())
    //    m_bot->GetMotionMaster()->Clear();
}

// submits packet to use an item
void PlayerbotAI::EquipItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();

    WorldPacket* const packet = new WorldPacket(CMSG_AUTOEQUIP_ITEM, 2);
    *packet << bagIndex << slot;
    m_bot->GetSession()->QueuePacket(packet);
}

// submits packet to trade an item (trade window must already be open)
// default slot is -1 which means trade slots 0 to 5. if slot is set
// to TRADE_SLOT_NONTRADED (which is slot 6) item will be shown in the
// 'Will not be traded' slot.
bool PlayerbotAI::TradeItem(const Item& item, int8 slot)
{
    sLog.outDebug("[PlayerbotAI::TradeItem]: slot=%d, hasTrader=%d, itemInTrade=%d, itemTradeable=%d",
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

/*void PlayerbotAI::Stay()
   {
    m_IsFollowingMaster = false;
    m_bot->GetMotionMaster()->Clear(true);
    m_bot->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
   }*/

/*bool PlayerbotAI::Follow(Player& player)
   {
    if (GetMaster()->IsBeingTeleported())
        return false;

    m_IsFollowingMaster = true;

    if (!m_bot->IsStandState())
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (!m_bot->isInCombat())
    {
        // follow player or his corpse if dead (stops bot from running to graveyard if player repops...)
        if( player.GetCorpse() )
        {
            if( !FollowCheckTeleport( *player.GetCorpse() ) )
                return false;
        }
        else
        {
            if( !FollowCheckTeleport( player ) )
                return false;
        }
    }

    if (m_bot->isAlive())
    {
        float angle = rand_float(0, M_PI);
        float dist = rand_float(0.5f, 1.0f);
        m_bot->GetMotionMaster()->Clear(true);
        m_bot->GetMotionMaster()->MoveFollow(&player, dist, angle);
        return true;
    }
    return false;
   }*/

bool PlayerbotAI::FollowCheckTeleport(WorldObject &obj)
{
    // if bot has strayed too far from the master, teleport bot

    if (!m_bot->IsWithinDistInMap(&obj, 50, true) && GetMaster()->isAlive() && !GetMaster()->IsTaxiFlying())
    {
        m_ignoreAIUpdatesUntilTime = time(0) + 6;
        PlayerbotChatHandler ch(GetMaster());
        if (!ch.teleport(*m_bot))
        {
            ch.sysmessage(".. could not be teleported ..");
            //sLog.outDebug( "[PlayerbotAI]: %s failed to teleport", m_bot->GetName() );
            return false;
        }
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
        p.appendPackGUID(m_bot->GetGUID());
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
    else if (m_bot->GetTrader() && m_bot->GetTrader()->GetGUID() == fromPlayer.GetGUID())
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
        m_lootCreature.clear();
        m_lootCurrent = 0;
        m_targetCombat = 0;
        // do we want to reset all states on this command?
//		m_combatOrder = ORDERS_NONE;
//		m_targetCombat = 0;
//		m_targetAssisst = 0;
//		m_targetProtect = 0;
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
        if (!attackOnGuid.IsEmpty())
        {
            Unit* thingToAttack = ObjectAccessor::GetUnit(*m_bot, attackOnGuid);
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
    else if (text.size() > 2 && text.substr(0, 2) == "c " || text.size() > 5 && text.substr(0, 5) == "cast ")
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
            m_bot->RemoveAurasByCasterSpell(spellId, m_bot->GetGUID());
            return;
        }

        ObjectGuid castOnGuid = fromPlayer.GetSelectionGuid();
        if (spellId != 0 && !castOnGuid.IsEmpty() && m_bot->HasSpell(spellId))
        {
            m_spellIdCommand = spellId;
            m_targetGuidCommand = castOnGuid.GetRawValue();
        }

    }

    // use items
    else if (text.size() > 2 && text.substr(0, 2) == "u " || text.size() > 4 && text.substr(0, 4) == "use ")
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
            UseItem(**it);
    }

    // equip items
    else if (text.size() > 2 && text.substr(0, 2) == "e " || text.size() > 6 && text.substr(0, 6) == "equip ")
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
            EquipItem(**it);
    }

    // find item in world
    else if (text.size() > 2 && text.substr(0, 2) == "f " || text.size() > 5 && text.substr(0, 5) == "find ")
    {
        uint32 guid;
        float x, y, z;
        uint32 entry;
        int mapid;
        if (extractGOinfo(text, guid, entry, mapid, x, y, z))
        {    // sLog.outDebug("find: guid : %u entry : %u x : (%f) y : (%f) z : (%f) mapid : %d",guid, entry, x, y, z, mapid);
            m_bot->UpdateGroundPositionZ(x, y, z);
            SetMovementOrder(MOVEMENT_STAY);
            m_bot->GetMotionMaster()->MovePoint(mapid, x, y, z);
        }
        else
            SendWhisper("I have no info on that object", fromPlayer);
    }

    // get project: 18:50 03/05/10 rev.3 allows bots to retrieve all lootable & quest items from gameobjects
    else if (text.size() > 2 && text.substr(0, 2) == "g " || text.size() > 4 && text.substr(0, 4) == "get ")
    {
        uint32 guid;
        float x, y, z;
        uint32 entry;
        int mapid;
        bool looted = false;
        if (extractGOinfo(text, guid, entry, mapid, x, y, z))
        {

            //sLog.outDebug("find: guid : %u entry : %u x : (%f) y : (%f) z : (%f) mapid : %d",guid, entry, x, y, z, mapid);
            ObjectGuid lootCurrent = ObjectGuid(HIGHGUID_GAMEOBJECT, entry, guid);
            GameObject *go = m_bot->GetMap()->GetGameObject(lootCurrent);
            if (!go)
            {
                lootCurrent = 0;
                return;
            }

            if (!go->isSpawned())
                return;

            m_bot->UpdateGroundPositionZ(x, y, z);
            m_bot->GetMotionMaster()->MovePoint(mapid, x, y, z);
            m_bot->SetPosition(x, y, z, m_bot->GetOrientation());
            m_bot->SendLoot(lootCurrent, LOOT_CORPSE);
            Loot *loot = &go->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
            // sLog.outDebug( "[PlayerbotAI]: GetGOType %u - %s looting: '%s' got %d items", go->GetGoType(), m_bot->GetName(), go->GetGOInfo()->name, loot->GetMaxSlotInLootFor( m_bot ));
            if (lootNum == 0) // Handle opening gameobjects that contain no items
            {
                uint32 lockId = go->GetGOInfo()->GetLockId();
                LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
                if (lockInfo)
                    for (int i = 0; i < 8; ++i)
                    {
                        uint32 skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                        if (skillId > 0)
                        {
                            if (m_bot->HasSkill(skillId)) // Has skill
                            {
                                uint32 reqSkillValue = lockInfo->Skill[i];
                                uint32 SkillValue = m_bot->GetPureSkillValue(skillId);
                                if (SkillValue >= reqSkillValue)
                                {
                                    // sLog.outDebug("[PlayerbotAI]i: skillId : %u SkillValue : %u reqSkillValue : %u",skillId,SkillValue,reqSkillValue);
                                    m_bot->UpdateGatherSkill(skillId, SkillValue, reqSkillValue);
                                    looted = true;
                                }
                            }
                            break;
                        }
                    }
            }
            for (uint32 l = 0; l < lootNum; l++)
            {
                // sLog.outDebug("[PlayerbotAI]: lootNum : %u",lootNum);
                QuestItem *qitem = 0, *ffaitem = 0, *conditem = 0;
                LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
                if (!item)
                    continue;

                if (!qitem && item->is_blocked)
                {
                    m_bot->SendLootRelease(lootCurrent);
                    continue;
                }

                if (m_needItemList[item->itemid] > 0)
                {
                    ItemPosCountVec dest;
                    if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
                    {
                        Item * newitem = m_bot->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);

                        if (qitem)
                        {
                            qitem->is_looted = true;
                            if (item->freeforall || loot->GetPlayerQuestItems().size() == 1)
                                m_bot->SendNotifyLootItemRemoved(l);
                            else
                                loot->NotifyQuestItemRemoved(qitem->index);
                        }
                        else
                        {
                            if (ffaitem)
                            {
                                ffaitem->is_looted = true;
                                m_bot->SendNotifyLootItemRemoved(l);
                            }
                            else
                            {
                                if (conditem)
                                    conditem->is_looted = true;
                                loot->NotifyItemRemoved(l);
                            }
                        }
                        if (!item->freeforall)
                            item->is_looted = true;
                        --loot->unlootedCount;
                        m_bot->SendNewItem(newitem, uint32(item->count), false, false, true);
                        m_bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item->itemid, item->count);
                        looted = true;
                    }
                    continue;
                }

                uint32 lockId = go->GetGOInfo()->GetLockId();
                LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
                if (lockInfo)
                {
                    uint32 skillId = 0;
                    uint32 reqSkillValue = 0;
                    for (int i = 0; i < 8; ++i)
                    {
                        skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                        if (skillId > 0)
                        {
                            reqSkillValue = lockInfo->Skill[i];
                            break;
                        }
                    }

                    if (m_bot->HasSkill(skillId) || skillId == SKILL_NONE) // Has skill or skill not required
                    {
                        if ((skillId == SKILL_MINING) && !HasPick())
                            continue;

                        ItemPosCountVec dest;
                        if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
                        {
                            Item* pItem = m_bot->StoreNewItem (dest, item->itemid, true, item->randomPropertyId);
                            uint32 SkillValue = m_bot->GetPureSkillValue(skillId);
                            if (SkillValue >= reqSkillValue)
                            {
                                m_bot->SendNewItem(pItem, uint32(item->count), false, false, true);
                                m_bot->UpdateGatherSkill(skillId, SkillValue, reqSkillValue);
                                --loot->unlootedCount;
                                looted = true;
                            }
                        }
                    }
                }
            }
            // release loot
            if (looted)
                m_bot->GetSession()->DoLootRelease(lootCurrent);
            else
                m_bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
            // sLog.outDebug( "[PlayerbotAI]: %s looted target 0x%08X", m_bot->GetName(), lootCurrent );
            SetQuestNeedItems();
        }
        else
            SendWhisper("I have no info on that object", fromPlayer);
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
        if (linkStart < 0 || !ch.dropQuest((char*) text.substr(linkStart).c_str()))
            ch.sysmessage("ERROR: could not drop quest");
        if (!oldSelectionGUID.IsEmpty())
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
                    pet->RemoveAurasByCasterSpell(spellId, pet->GetGUID());
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
                            pet->RemoveAurasByCasterSpell(spellId, pet->GetGUID());
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

    // survey project: 18:30 29/04/10 rev.3 filter out event triggered objects & now updates list
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
                float x = fields[2].GetFloat();
                float y = fields[3].GetFloat();
                float z = fields[4].GetFloat();
                int mapid = fields[5].GetUInt16();

                GameObject *go = m_bot->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, entry, guid));
                if (!go)
                    continue;

                if (!go->isSpawned())
                    continue;

                detectout << "|cFFFFFF00|Hfound:" << guid << ":" << entry << ":" << x << ":" << y << ":" << z  << ":" << mapid  << ":" <<  "|h[" << go->GetGOInfo()->name << "]|h|r";
                ++count;
            } while (result->NextRow());

            delete result;
        }
        SendWhisper(detectout.str().c_str(), fromPlayer);
    }

    // stats project: 10:00 19/04/10 rev.1 display bot statistics
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
            const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
            if (pBag)
            {
                ItemPrototype const* pBagProto = pBag->GetProto();
                if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                    totalfree =  totalfree + pBag->GetFreeSlots();
            }

        }

        // calculate how much money bot has
        uint32 copper = m_bot->GetMoney();
        uint32 gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        uint32 silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|cffffffff[|h|cff00ffff" << m_bot->GetName() << "|h|cffffffff]" << " has |r|cff00ff00" << gold
            << "|r|cfffffc00g|r|cff00ff00" << silver
            << "|r|cffcdcdcds|r|cff00ff00" << copper
            << "|r|cffffd333c" << "|h|cffffffff bag slots |h|cff00ff00" << totalfree;

        // estimate how much item damage the bot has
        copper = EstRepairAll();
        gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|h|cffffffff & item damage cost " << "|r|cff00ff00" << gold
            << "|r|cfffffc00g|r|cff00ff00" << silver
            << "|r|cffcdcdcds|r|cff00ff00" << copper
            << "|r|cffffd333c";
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
            Object* const pNpc = (WorldObject*) m_bot->GetObjectByTypeMask(questRewarderGUID, TYPEMASK_CREATURE_OR_GAMEOBJECT);
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
            std::string msg = "What? follow, stay, (c)ast <spellname>, spells, (e)quip <itemlink>, (u)se <itemlink>, drop <questlink>, report, quests, stats";
            SendWhisper(msg, fromPlayer);
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }
}
