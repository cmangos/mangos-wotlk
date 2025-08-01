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

#include "Common.h"
#include "Server/WorldPacket.h"
#include "Entities/Player.h"
#include "Server/Opcodes.h"
#include "Chat/Chat.h"
#include "Log/Log.h"
#include "Entities/Unit.h"
#include "Entities/GossipDef.h"
#include "Tools/Language.h"
#include "BattleGround/BattleGroundMgr.h"
#include <fstream>
#include "Maps/MapManager.h"
#include "Globals/ObjectMgr.h"
#include "Entities/ObjectGuid.h"
#include "Spells/SpellMgr.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Maps/InstanceData.h"
#include "Models/M2Stores.h"
#include "Entities/Transports.h"
#include "World/World.h"

bool ChatHandler::HandleDebugSendSpellFailCommand(char* args)
{
    if (!*args)
        return false;

    uint32 failnum;
    if (!ExtractUInt32(&args, failnum) || failnum > 255)
        return false;

    uint32 failarg1;
    if (!ExtractOptUInt32(&args, failarg1, 0))
        return false;

    uint32 failarg2;
    if (!ExtractOptUInt32(&args, failarg2, 0))
        return false;

    WorldPacket data(SMSG_CAST_RESULT, 5);
    data << uint8(0);
    data << uint32(133);
    data << uint8(failnum);
    if (failarg1 || failarg2)
        data << uint32(failarg1);
    if (failarg2)
        data << uint32(failarg2);

    m_session->SendPacket(data);

    return true;
}

bool ChatHandler::HandleDebugSendPoiCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return true;
    }

    uint32 icon;
    if (!ExtractUInt32(&args, icon))
        return false;

    uint32 flags;
    if (!ExtractUInt32(&args, flags))
        return false;

    DETAIL_LOG("Command : POI, NPC = %u, icon = %u flags = %u", target->GetGUIDLow(), icon, flags);
    pPlayer->GetPlayerMenu()->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(icon), flags, 30, "Test POI");
    return true;
}

bool ChatHandler::HandleDebugSendEquipErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendEquipError(InventoryResult(msg), nullptr, nullptr);
    return true;
}

bool ChatHandler::HandleDebugSendSellErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendSellError(SellResult(msg), nullptr, ObjectGuid(), 0);
    return true;
}

bool ChatHandler::HandleDebugSendBuyErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendBuyError(BuyResult(msg), nullptr, 0, 0);
    return true;
}

bool ChatHandler::HandleDebugSendOpcodeCommand(char* /*args*/)
{
    Unit* unit = getSelectedUnit();
    if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
        unit = m_session->GetPlayer();

    std::ifstream stream("opcode.txt");
    if (!stream.is_open())
        return false;

    uint32 opcode = 0;
    if (!(stream >> opcode))
    {
        stream.close();
        return false;
    }

    WorldPacket data(Opcodes(opcode), 0);

    std::string type;
    while (stream >> type)
    {
        if (type.empty())
            break;

        if (type == "uint8")
        {
            uint16 value;
            stream >> value;
            data << uint8(value);
        }
        else if (type == "uint16")
        {
            uint16 value;
            stream >> value;
            data << value;
        }
        else if (type == "uint32")
        {
            uint32 value;
            stream >> value;
            data << value;
        }
        else if (type == "uint64")
        {
            uint64 value;
            stream >> value;
            data << value;
        }
        else if (type == "float")
        {
            float value;
            stream >> value;
            data << value;
        }
        else if (type == "string")
        {
            std::string value;
            stream >> value;
            data << value;
        }
        else if (type == "pguid")
        {
            data << unit->GetPackGUID();
        }
        else
        {
            DEBUG_LOG("Sending opcode: unknown type '%s'", type.c_str());
            break;
        }
    }
    stream.close();

    DEBUG_LOG("Sending opcode %u, %s", data.GetOpcode(), data.GetOpcodeName());

    data.hexlike();
    ((Player*)unit)->GetSession()->SendPacket(data);

    PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName());

    return true;
}

bool ChatHandler::HandleDebugUpdateWorldStateCommand(char* args)
{
    uint32 world;
    if (!ExtractUInt32(&args, world))
        return false;

    uint32 state;
    if (!ExtractUInt32(&args, state))
        return false;

    m_session->GetPlayer()->SendUpdateWorldState(world, state);
    return true;
}

bool ChatHandler::HandleDebugPlayCinematicCommand(char* args)
{
    // USAGE: .debug play cinematic #cinematicid
    // #cinematicid - ID decimal number from CinemaicSequences.dbc (1st column)
    uint32 dwId;
    if (!ExtractUInt32(&args, dwId))
        return false;

    if (!sCinematicSequencesStore.LookupEntry(dwId))
    {
        PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, dwId);
        SetSentErrorMessage(true);
        return false;
    }

    // Dump camera locations
    if (CinematicSequencesEntry const* cineSeq = sCinematicSequencesStore.LookupEntry(dwId))
    {
        std::unordered_map<uint32, FlyByCameraCollection>::const_iterator itr = sFlyByCameraStore.find(cineSeq->cinematicCamera);
        if (itr != sFlyByCameraStore.end())
        {
            PSendSysMessage("Waypoints for sequence %u, camera %u", dwId, cineSeq->cinematicCamera);
            uint32 count = 1;
            for (FlyByCamera cam : itr->second)
            {
                PSendSysMessage("%02u - %7ums [%f, %f, %f] Facing %f (%f degrees)", count, cam.timeStamp, cam.locations.x, cam.locations.y, cam.locations.z, cam.locations.w, cam.locations.w * (180 / M_PI));
                count++;
            }
            PSendSysMessage("%u waypoints dumped", uint32(itr->second.size()));
        }
    }

    m_session->GetPlayer()->SendCinematicStart(dwId);
    return true;
}

bool ChatHandler::HandleDebugPlayMovieCommand(char* args)
{
    // USAGE: .debug play movie #movieid
    // #movieid - ID decimal number from Movie.dbc (1st column)
    uint32 dwId;
    if (!ExtractUInt32(&args, dwId))
        return false;

    if (!sMovieStore.LookupEntry(dwId))
    {
        PSendSysMessage(LANG_MOVIE_NOT_EXIST, dwId);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->SendMovieStart(dwId);
    return true;
}

// Play sound
bool ChatHandler::HandleDebugPlaySoundCommand(char* args)
{
    // USAGE: .debug playsound #soundid
    // #soundid - ID decimal number from SoundEntries.dbc (1st column)
    uint32 dwSoundId;
    if (!ExtractUInt32(&args, dwSoundId))
        return false;

    if (!sSoundEntriesStore.LookupEntry(dwSoundId))
    {
        PSendSysMessage(LANG_SOUND_NOT_EXIST, dwSoundId);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->GetSelectionGuid())
        unit->PlayDistanceSound(dwSoundId, PlayPacketParameters(PlayPacketSettings::TARGET, m_session->GetPlayer()));
    else
        unit->PlayDirectSound(dwSoundId, PlayPacketParameters(PlayPacketSettings::TARGET, m_session->GetPlayer()));

    PSendSysMessage(LANG_YOU_HEAR_SOUND, dwSoundId);
    return true;
}

// Play Music
bool ChatHandler::HandleDebugPlayMusicCommand(char* args)
{
    // USAGE: .debug playmusic #musicid
    // #musicid - ID decimal number from SoundEntries.dbc (1st column)
    uint32 dwMusicId;
    if (!ExtractUInt32(&args, dwMusicId))
        return false;

    if (!sSoundEntriesStore.LookupEntry(dwMusicId))
    {
        PSendSysMessage(LANG_SOUND_NOT_EXIST, dwMusicId);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->PlayMusic(dwMusicId, PlayPacketParameters(PlayPacketSettings::TARGET, dynamic_cast<Player*>(getSelectedUnit())));

    PSendSysMessage(LANG_YOU_HEAR_SOUND, dwMusicId);
    return true;
}

// Play pet dismiss sound
bool ChatHandler::HandleDebugPetDismissSound(char* args)
{
    uint32 petDismissSound;
    if (!ExtractUInt32(&args, petDismissSound))
        return false;

    m_session->GetPlayer()->SendPetDismiss(petDismissSound);
    PSendSysMessage(LANG_YOU_HEAR_SOUND, petDismissSound);
    return true;
}

bool ChatHandler::HandleDebugAreaTriggersCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    if (!player->isDebuggingAreaTriggers())
    {
        PSendSysMessage(LANG_DEBUG_AREATRIGGER_ON);
        player->SetDebuggingAreaTriggers(true);
    }
    else
    {
        PSendSysMessage(LANG_DEBUG_AREATRIGGER_OFF);
        player->SetDebuggingAreaTriggers(false);
    }
    return true;
}

// Send notification in channel
bool ChatHandler::HandleDebugSendChannelNotifyCommand(char* args)
{
    const char* name = "test";

    uint32 code;
    if (!ExtractUInt32(&args, code) || code > 255)
        return false;

    WorldPacket data(SMSG_CHANNEL_NOTIFY, (1 + 10));
    data << uint8(code);                                    // notify type
    data << name;                                           // channel name
    data << uint32(0);
    data << uint32(0);
    m_session->SendPacket(data);
    return true;
}

// Send notification in chat
bool ChatHandler::HandleDebugSendChatMsgCommand(char* args)
{
    const char* msg = "testtest";

    uint32 type;
    if (!ExtractUInt32(&args, type) || type > 255)
        return false;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, ChatMsg(type), msg, LANG_UNIVERSAL, CHAT_TAG_NONE, m_session->GetPlayer()->GetObjectGuid(), m_session->GetPlayerName());
    m_session->SendPacket(data);
    return true;
}

bool ChatHandler::HandleDebugSendQuestFailedMsgCommand(char* args)
{
    uint32 questId;
    if (!ExtractUInt32(&args, questId))
        return false;

    uint32 reason;
    if (!ExtractUInt32(&args, reason))
        return false;

    m_session->GetPlayer()->SendQuestFailed(questId, InventoryResult(reason));
    return true;
}

bool ChatHandler::HandleDebugSendQuestPartyMsgCommand(char* args)
{
    uint32 msg;
    if (!ExtractUInt32(&args, msg))
        return false;

    m_session->GetPlayer()->SendPushToPartyResponse(m_session->GetPlayer(), msg);
    return true;
}

bool ChatHandler::HandleDebugGetLootRecipientCommand(char* /*args*/)
{
    Creature* target = getSelectedCreature();
    if (!target)
        return false;

    if (!target->HasLootRecipient())
        SendSysMessage("loot recipient: no loot recipient");
    else if (Player* recipient = target->GetLootRecipient())
        PSendSysMessage("loot recipient: %s with raw data %s from group %u",
                        recipient->GetGuidStr().c_str(),
                        target->GetLootRecipientGuid().GetString().c_str(),
                        target->GetLootGroupRecipientId());
    else
        SendSysMessage("loot recipient: offline ");

    return true;
}

bool ChatHandler::HandleDebugSendQuestInvalidMsgCommand(char* args)
{
    uint32 msg = std::stoul(args);
    m_session->GetPlayer()->SendCanTakeQuestResponse(msg);
    return true;
}

bool ChatHandler::HandleDebugListUpdateFieldsCommand(char* /*args*/)
{
    Unit* pTarget = getSelectedUnit();

    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    ShowAllUpdateFieldsHelper(pTarget);

    return true;
}

bool ChatHandler::HandleDebugGetItemStateCommand(char* args)
{
    if (!*args)
        return false;

    ItemUpdateState state = ITEM_UNCHANGED;
    bool list_queue = false, check_all = false;

    std::string state_str;

    if (strncmp(args, "unchanged", strlen(args)) == 0)
    {
        state = ITEM_UNCHANGED;
        state_str = "unchanged";
    }
    else if (strncmp(args, "changed", strlen(args)) == 0)
    {
        state = ITEM_CHANGED;
        state_str = "changed";
    }
    else if (strncmp(args, "new", strlen(args)) == 0)
    {
        state = ITEM_NEW;
        state_str = "new";
    }
    else if (strncmp(args, "removed", strlen(args)) == 0)
    {
        state = ITEM_REMOVED;
        state_str = "removed";
    }
    else if (strncmp(args, "queue", strlen(args)) == 0)
        list_queue = true;
    else if (strncmp(args, "all", strlen(args)) == 0)
        check_all = true;
    else
        return false;

    Player* player = getSelectedPlayer();
    if (!player) player = m_session->GetPlayer();

    if (!list_queue && !check_all)
    {
        state_str = "The player has the following " + state_str + " items: ";
        SendSysMessage(state_str.c_str());
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
        {
            if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;
            if (!item->IsBag())
            {
                if (item->GetState() == state)
                    PSendSysMessage("%s bag: 255 slot: %u owner: %s",
                                    item->GetGuidStr().c_str(),  uint32(item->GetSlot()), item->GetOwnerGuid().GetString().c_str());
            }
            else
            {
                Bag* bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item2 = bag->GetItemByPos(j);
                    if (item2 && item2->GetState() == state)
                        PSendSysMessage("%s bag: %u slot: %u owner: %s",
                                        item2->GetGuidStr().c_str(), uint32(item2->GetBagSlot()), uint32(item2->GetSlot()),
                                        item2->GetOwnerGuid().GetString().c_str());
                }
            }
        }
    }

    if (list_queue)
    {
        std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
        for (auto item : updateQueue)
        {
            if (!item) continue;

            Bag* container = item->GetContainer();
            uint8 bag_slot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

            std::string st;
            switch (item->GetState())
            {
                case ITEM_UNCHANGED: st = "unchanged"; break;
                case ITEM_CHANGED: st = "changed"; break;
                case ITEM_NEW: st = "new"; break;
                case ITEM_REMOVED: st = "removed"; break;
            }

            PSendSysMessage("%s bag: %u slot: %u - state: %s",
                            item->GetGuidStr().c_str(), uint32(bag_slot), uint32(item->GetSlot()), st.c_str());
        }
        if (updateQueue.empty())
            PSendSysMessage("updatequeue empty");
    }

    if (check_all)
    {
        bool error = false;
        std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
        {
            if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;

            if (item->GetSlot() != i)
            {
                PSendSysMessage("%s at slot %u has an incorrect slot value: %d",
                                item->GetGuidStr().c_str(), uint32(i), uint32(item->GetSlot()));
                error = true; continue;
            }

            if (item->GetOwnerGuid() != player->GetObjectGuid())
            {
                PSendSysMessage("%s at slot %u owner (%s) and inventory owner (%s) don't match!",
                                item->GetGuidStr().c_str(), uint32(item->GetSlot()),
                                item->GetOwnerGuid().GetString().c_str(), player->GetGuidStr().c_str());
                error = true; continue;
            }

            if (Bag* container = item->GetContainer())
            {
                PSendSysMessage("%s at slot %u has a container %s from slot %u but shouldnt!",
                                item->GetGuidStr().c_str(), uint32(item->GetSlot()),
                                container->GetGuidStr().c_str(), uint32(container->GetSlot()));
                error = true; continue;
            }

            if (item->IsInUpdateQueue())
            {
                uint16 qp = item->GetQueuePos();
                if (qp > updateQueue.size())
                {
                    PSendSysMessage("%s at slot %u has a queuepos (%d) larger than the update queue size! ",
                                    item->GetGuidStr().c_str(), uint32(item->GetSlot()), uint32(qp));
                    error = true; continue;
                }

                if (updateQueue[qp] == nullptr)
                {
                    PSendSysMessage("%s at slot %u has a queuepos (%d) that points to NULL in the queue!",
                                    item->GetGuidStr().c_str(), uint32(item->GetSlot()), uint32(qp));
                    error = true; continue;
                }

                if (updateQueue[qp] != item)
                {
                    PSendSysMessage("%s at slot %u has a queuepos (%d) that points to %s in the queue (bag %u, slot %u)",
                                    item->GetGuidStr().c_str(), uint32(item->GetSlot()), uint32(qp),
                                    updateQueue[qp]->GetGuidStr().c_str(), uint32(updateQueue[qp]->GetBagSlot()), uint32(updateQueue[qp]->GetSlot()));
                    error = true; continue;
                }
            }
            else if (item->GetState() != ITEM_UNCHANGED)
            {
                PSendSysMessage("%s at slot %u is not in queue but should be (state: %d)!",
                                item->GetGuidStr().c_str(), uint32(item->GetSlot()), item->GetState());
                error = true; continue;
            }

            if (item->IsBag())
            {
                Bag* bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item2 = bag->GetItemByPos(j);
                    if (!item2) continue;

                    if (item2->GetSlot() != j)
                    {
                        PSendSysMessage("%s in bag %u at slot %u has an incorrect slot value: %u",
                                        item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(j), uint32(item2->GetSlot()));
                        error = true; continue;
                    }

                    if (item2->GetOwnerGuid() != player->GetObjectGuid())
                    {
                        PSendSysMessage("%s in bag %u at slot %u owner (%s) and inventory owner (%s) don't match!",
                                        item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()),
                                        item2->GetOwnerGuid().GetString().c_str(), player->GetGuidStr().c_str());
                        error = true; continue;
                    }

                    Bag* container = item2->GetContainer();
                    if (!container)
                    {
                        PSendSysMessage("%s in bag %u at slot %u has no container!",
                                        item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()));
                        error = true; continue;
                    }

                    if (container != bag)
                    {
                        PSendSysMessage("%s in bag %u at slot %u has a different container %s from slot %u!",
                                        item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()),
                                        container->GetGuidStr().c_str(), uint32(container->GetSlot()));
                        error = true; continue;
                    }

                    if (item2->IsInUpdateQueue())
                    {
                        uint16 qp = item2->GetQueuePos();
                        if (qp > updateQueue.size())
                        {
                            PSendSysMessage("%s in bag %u at slot %u has a queuepos (%d) larger than the update queue size! ",
                                            item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()), uint32(qp));
                            error = true; continue;
                        }

                        if (updateQueue[qp] == nullptr)
                        {
                            PSendSysMessage("%s in bag %u at slot %u has a queuepos (%d) that points to NULL in the queue!",
                                            item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()), uint32(qp));
                            error = true; continue;
                        }

                        if (updateQueue[qp] != item2)
                        {
                            PSendSysMessage("%s in bag %u at slot %u has a queuepos (%d) that points to %s in the queue (bag %u slot %u)",
                                            item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()), uint32(qp),
                                            updateQueue[qp]->GetGuidStr().c_str(), uint32(updateQueue[qp]->GetBagSlot()), uint32(updateQueue[qp]->GetSlot()));
                            error = true;
                        }
                    }
                    else if (item2->GetState() != ITEM_UNCHANGED)
                    {
                        PSendSysMessage("%s in bag %u at slot %u is not in queue but should be (state: %d)!",
                                        item2->GetGuidStr().c_str(), uint32(bag->GetSlot()), uint32(item2->GetSlot()), item2->GetState());
                        error = true;
                    }
                }
            }
        }

        for (size_t i = 0; i < updateQueue.size(); ++i)
        {
            Item* item = updateQueue[i];
            if (!item) continue;

            if (item->GetOwnerGuid() != player->GetObjectGuid())
            {
                PSendSysMessage("queue(" SIZEFMTD "): %s has the owner (%s) and inventory owner (%s) don't match!",
                                i, item->GetGuidStr().c_str(),
                                item->GetOwnerGuid().GetString().c_str(), player->GetGuidStr().c_str());
                error = true; continue;
            }

            if (item->GetQueuePos() != i)
            {
                PSendSysMessage("queue(" SIZEFMTD "): %s has queuepos doesn't match it's position in the queue!",
                                i, item->GetGuidStr().c_str());
                error = true; continue;
            }

            if (item->GetState() == ITEM_REMOVED) continue;
            Item* test = player->GetItemByPos(item->GetBagSlot(), item->GetSlot());

            if (test == nullptr)
            {
                PSendSysMessage("queue(" SIZEFMTD "): %s has incorrect (bag %u slot %u) values, the player doesn't have an item at that position!",
                                i, item->GetGuidStr().c_str(), uint32(item->GetBagSlot()), uint32(item->GetSlot()));
                error = true; continue;
            }

            if (test != item)
            {
                PSendSysMessage("queue(" SIZEFMTD "): %s has incorrect (bag %u slot %u) values, the %s is there instead!",
                                i, item->GetGuidStr().c_str(), uint32(item->GetBagSlot()), uint32(item->GetSlot()),
                                test->GetGuidStr().c_str());
                error = true;
            }
        }
        if (!error)
            SendSysMessage("All OK!");
    }

    return true;
}

bool ChatHandler::HandleDebugBattlegroundCommand(char* /*args*/)
{
    sBattleGroundMgr.GetMessager().AddMessage([](BattleGroundMgr* mgr)
    {
        mgr->ToggleTesting();
    });
    return true;
}

bool ChatHandler::HandleDebugArenaCommand(char* /*args*/)
{
    sBattleGroundMgr.GetMessager().AddMessage([](BattleGroundMgr* mgr)
    {
        mgr->ToggleArenaTesting();
    });
    return true;
}

bool ChatHandler::HandleDebugSpellCheckCommand(char* /*args*/)
{
    sLog.outString("Check expected in code spell properties base at table 'spell_check' content...");
    sSpellMgr.CheckUsedSpells("spell_check");
    return true;
}

bool ChatHandler::HandleDebugSendLargePacketCommand(char* /*args*/)
{
    const char* stuffingString = "This is a dummy string to push the packet's size beyond 128000 bytes. ";
    std::ostringstream ss;
    while (ss.str().size() < 128000)
        ss << stuffingString;
    SendSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleDebugSendSetPhaseShiftCommand(char* args)
{
    if (!*args)
        return false;

    uint32 PhaseShift = atoi(args);
    m_session->SendSetPhaseShift(PhaseShift);
    return true;
}

// show animation
bool ChatHandler::HandleDebugAnimCommand(char* args)
{
    uint32 emote_id;
    if (!ExtractUInt32(&args, emote_id))
        return false;

    m_session->GetPlayer()->HandleEmoteCommand(emote_id);
    return true;
}

bool ChatHandler::HandleDebugSetAuraStateCommand(char* args)
{
    int32 state;
    if (!ExtractInt32(&args, state))
        return false;

    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!state)
    {
        // reset all states
        for (int i = 1; i <= 32; ++i)
            unit->ModifyAuraState(AuraState(i), false);
        return true;
    }

    unit->ModifyAuraState(AuraState(abs(state)), state > 0);
    return true;
}

bool ChatHandler::HandleSetValueHelper(Object* target, uint32 field, char* typeStr, char* valStr)
{
    ObjectGuid guid = target->GetObjectGuid();

    // not allow access to nonexistent or critical for work field
    if (field >= target->GetValuesCount() || field <= OBJECT_FIELD_ENTRY)
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), uint32(target->GetValuesCount()));
        return false;
    }

    uint32 base;                                            // 0 -> float
    if (!typeStr)
        base = 10;
    else if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        base = 10;
    else if (strncmp(typeStr, "hex", strlen(typeStr)) == 0)
        base = 16;
    else if (strncmp(typeStr, "bit", strlen(typeStr)) == 0)
        base = 2;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        base = 0;
    else
        return false;

    if (base)
    {
        uint32 iValue;
        if (!ExtractUInt32Base(&valStr, iValue, base))
            return false;

        DEBUG_LOG(GetMangosString(LANG_SET_UINT), guid.GetString().c_str(), field, iValue);
        target->SetUInt32Value(field, iValue);
        PSendSysMessage(LANG_SET_UINT_FIELD, guid.GetString().c_str(), field, iValue);
    }
    else
    {
        float fValue;
        if (!ExtractFloat(&valStr, fValue))
            return false;

        DEBUG_LOG(GetMangosString(LANG_SET_FLOAT), guid.GetString().c_str(), field, fValue);
        target->SetFloatValue(field, fValue);
        PSendSysMessage(LANG_SET_FLOAT_FIELD, guid.GetString().c_str(), field, fValue);
    }

    return true;
}

bool ChatHandler::HandleDebugSetItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractOptNotLastArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    Item* item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandleSetValueHelper(item, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugSetValueByIndexCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractOptNotLastArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    return HandleSetValueHelper(target, field, typeStr, valStr);
}

bool ChatHandler::HandleGetValueHelper(Object* target, uint32 field, char* typeStr)
{
    ObjectGuid guid = target->GetObjectGuid();

    if (field >= target->GetValuesCount())
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), target->GetValuesCount());
        return false;
    }

    uint32 base;                                            // 0 -> float
    if (!typeStr)
        base = 10;
    else if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        base = 10;
    else if (strncmp(typeStr, "hex", strlen(typeStr)) == 0)
        base = 16;
    else if (strncmp(typeStr, "bit", strlen(typeStr)) == 0)
        base = 2;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        base = 0;
    else
        return false;

    if (base)
    {
        uint32 iValue = target->GetUInt32Value(field);

        switch (base)
        {
            case 2:
            {
                // starting 0 if need as required bitstring format
                std::string res;
                res.reserve(1 + 32 + 1);
                res = iValue & (1 << (32 - 1)) ? "0" : " ";
                for (int i = 32; i > 0; --i)
                    res += iValue & (1 << (i - 1)) ? "1" : "0";
                DEBUG_LOG(GetMangosString(LANG_GET_BITSTR), guid.GetString().c_str(), field, res.c_str());
                PSendSysMessage(LANG_GET_BITSTR_FIELD, guid.GetString().c_str(), field, res.c_str());
                break;
            }
            case 16:
                DEBUG_LOG(GetMangosString(LANG_GET_HEX), guid.GetString().c_str(), field, iValue);
                PSendSysMessage(LANG_GET_HEX_FIELD, guid.GetString().c_str(), field, iValue);
                break;
            case 10:
            default:
                DEBUG_LOG(GetMangosString(LANG_GET_UINT), guid.GetString().c_str(), field, iValue);
                PSendSysMessage(LANG_GET_UINT_FIELD, guid.GetString().c_str(), field, iValue);
        }
    }
    else
    {
        float fValue = target->GetFloatValue(field);
        DEBUG_LOG(GetMangosString(LANG_GET_FLOAT), guid.GetString().c_str(), field, fValue);
        PSendSysMessage(LANG_GET_FLOAT_FIELD, guid.GetString().c_str(), field, fValue);
    }

    return true;
}

bool ChatHandler::HandleDebugGetItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    Item* item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandleGetValueHelper(item, field, typeStr);
}

bool ChatHandler::HandleDebugGetValueByIndexCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    return HandleGetValueHelper(target, field, typeStr);
}

bool ChatHandler::HandlerDebugModValueHelper(Object* target, uint32 field, char* typeStr, char* valStr)
{
    ObjectGuid guid = target->GetObjectGuid();

    // not allow access to nonexistent or critical for work field
    if (field >= target->GetValuesCount() || field <= OBJECT_FIELD_ENTRY)
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), uint32(target->GetValuesCount()));
        return false;
    }

    uint32 type;                                            // 0 -> float 1 -> int add 2-> bit or 3 -> bit and  4 -> bit and not
    if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        type = 1;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        type = 0;
    else if (strncmp(typeStr, "|=", strlen("|=") + 1) == 0) // exactly copy
        type = 2;
    else if (strncmp(typeStr, "&=", strlen("&=") + 1) == 0) // exactly copy
        type = 3;
    else if (strncmp(typeStr, "&=~", strlen("&=~") + 1) == 0) // exactly copy
        type = 4;
    else
        return false;

    if (type)
    {
        uint32 iValue;
        if (!ExtractUInt32Base(&valStr, iValue, type == 1 ? 10 : 16))
            return false;

        uint32 value = target->GetUInt32Value(field);

        switch (type)
        {
            default:
            case 1:                                         // int +
                value = uint32(int32(value) + int32(iValue));
                DEBUG_LOG(GetMangosString(LANG_CHANGE_INT32), guid.GetString().c_str(), field, iValue, value, value);
                PSendSysMessage(LANG_CHANGE_INT32_FIELD, guid.GetString().c_str(), field, iValue, value, value);
                break;
            case 2:                                         // |= bit or
                value |= iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
            case 3:                                         // &= bit and
                value &= iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
            case 4:                                         // &=~ bit and not
                value &= ~iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
        }

        target->SetUInt32Value(field, value);
    }
    else
    {
        float fValue;
        if (!ExtractFloat(&valStr, fValue))
            return false;

        float value = target->GetFloatValue(field);

        value += fValue;

        DEBUG_LOG(GetMangosString(LANG_CHANGE_FLOAT), guid.GetString().c_str(), field, fValue, value);
        PSendSysMessage(LANG_CHANGE_FLOAT_FIELD, guid.GetString().c_str(), field, fValue, value);

        target->SetFloatValue(field, value);
    }

    return true;
}

bool ChatHandler::HandleDebugSetValueByNameCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    char* fieldName = ExtractQuotedOrLiteralArg(&args);
    if (!fieldName)
        return false;

    if (UpdateFieldData const* pField = UpdateFields::GetUpdateFieldDataByName(fieldName))
    {
        if ((pField->objectTypeMask & target->GetTypeMask()) == 0)
        {
            SendSysMessage("Target does have that field.");
            return true;
        }

        switch (pField->valueType)
        {
            case UF_TYPE_INT:
            {
                uint32 value;
                if (!ExtractUInt32(&args, value))
                    return false;

                target->SetUInt32Value(pField->offset, value);
                PSendSysMessage("Field %s of %s set to %u.", pField->name, target->GetName(), value);
                break;
            }
            case UF_TYPE_TWO_SHORT:
            {
                uint32 value1;
                if (!ExtractUInt32(&args, value1))
                    return false;

                uint32 value2;
                if (!ExtractUInt32(&args, value2))
                    return false;

                target->SetUInt16Value(pField->offset, 0, value1);
                target->SetUInt16Value(pField->offset, 1, value2);
                PSendSysMessage("Field %s of %s set to %u/%u.", pField->name, target->GetName(), value1, value2);
                break;
            }
            case UF_TYPE_FLOAT:
            {
                float value;
                if (!ExtractFloat(&args, value))
                    return false;

                target->SetFloatValue(pField->offset, value);
                PSendSysMessage("Field %s of %s set to %g.", pField->name, target->GetName(), value);
                break;
            }
            case UF_TYPE_BYTES:
            case UF_TYPE_BYTES2:
            {
                uint32 value1;
                if (!ExtractUInt32(&args, value1))
                    return false;

                uint32 value2;
                if (!ExtractUInt32(&args, value2))
                    return false;

                uint32 value3;
                if (!ExtractUInt32(&args, value3))
                    return false;

                uint32 value4;
                if (!ExtractUInt32(&args, value4))
                    return false;

                target->SetByteValue(pField->offset, 0, value1);
                target->SetByteValue(pField->offset, 1, value2);
                target->SetByteValue(pField->offset, 2, value3);
                target->SetByteValue(pField->offset, 3, value4);
                PSendSysMessage("Field %s of %s set to %u/%u/%u/%u.", pField->name, target->GetName(), value1, value2, value3, value4);
                break;
            }
            default:
            {
                SendSysMessage("Unsupported field type.");
                break;
            }
        }
       
    }
    else
        SendSysMessage("Wrong field name.");

    return true;
}

bool ChatHandler::HandleDebugGetValueByNameCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    char* fieldName = ExtractQuotedOrLiteralArg(&args);
    if (!fieldName)
        return false;

    if (UpdateFieldData const* pField = UpdateFields::GetUpdateFieldDataByName(fieldName))
    {
        if ((pField->objectTypeMask & target->GetTypeMask()) == 0)
        {
            SendSysMessage("Target does have that field.");
            return true;
        }
        PSendSysMessage("Update field info for %s", target->GetGuidStr().c_str());
        ShowUpdateFieldHelper(target, pField->offset);
    }
    else
        SendSysMessage("Wrong field name.");

    return true;
}

void ChatHandler::ShowAllUpdateFieldsHelper(Object const* target)
{
    PSendSysMessage("Update field info for %s", target->GetGuidStr().c_str());

    for (uint16 index = 0; index < target->GetValuesCount(); index++)
    {
        if (!target->GetUInt32Value(index))
            continue;

        ShowUpdateFieldHelper(target, index);
    }
}

void ChatHandler::ShowUpdateFieldHelper(Object const* target, uint16 index)
{
    if (UpdateFieldData const* pField = UpdateFields::GetUpdateFieldDataByTypeMaskAndOffset(target->GetTypeMask(), index))
    {
        std::string fieldName = pField->name;
        if (index > pField->offset)
            fieldName += "+" + std::to_string(index - pField->offset);

        switch (pField->valueType)
        {
            case UF_TYPE_INT:
                PSendSysMessage("%s: %u", fieldName.c_str(), target->GetUInt32Value(index));
                break;
            case UF_TYPE_TWO_SHORT:
                PSendSysMessage("%s: %u/%u", fieldName.c_str(), target->GetUInt16Value(index, 0), target->GetUInt16Value(index, 1));
                break;
            case UF_TYPE_FLOAT:
                PSendSysMessage("%s: %g", fieldName.c_str(), target->GetFloatValue(index));
                break;
            case UF_TYPE_GUID:
                if (index == pField->offset)
                    PSendSysMessage("%s: %s", fieldName.c_str(), target->GetGuidValue(index).GetString().c_str());
                break;
            case UF_TYPE_BYTES:
            case UF_TYPE_BYTES2:
                PSendSysMessage("%s: %u/%u/%u/%u", fieldName.c_str(), target->GetByteValue(index, 0), target->GetByteValue(index, 1), target->GetByteValue(index, 2), target->GetByteValue(index, 3));
                break;
            default:
                SendSysMessage("Unsupported field type.");
                break;
        }
    }
}

bool ChatHandler::HandleDebugModItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    Item* item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandlerDebugModValueHelper(item, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugModValueCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    return HandlerDebugModValueHelper(target, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugSpellCoefsCommand(char* args)
{
    uint32 spellid = ExtractSpellIdFromLink(&args);
    if (!spellid)
        return false;

    SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellid);
    if (!spellEntry)
        return false;

    float direct_calc = CalculateDefaultCoefficient(spellEntry, SPELL_DIRECT_DAMAGE);
    float dot_calc = CalculateDefaultCoefficient(spellEntry, DOT);

    bool isDirectHeal = false;
    for (int i = 0; i < 3; ++i)
    {
        // Heals (Also count Mana Shield and Absorb effects as heals)
        if (spellEntry->Effect[i] == SPELL_EFFECT_HEAL || spellEntry->Effect[i] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                (spellEntry->Effect[i] == SPELL_EFFECT_APPLY_AURA && (spellEntry->EffectApplyAuraName[i] == SPELL_AURA_SCHOOL_ABSORB || spellEntry->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_HEAL)))
        {
            isDirectHeal = true;
            break;
        }
    }

    bool isDotHeal = false;
    for (int i = 0; i < 3; ++i)
    {
        // Periodic Heals
        if (spellEntry->Effect[i] == SPELL_EFFECT_APPLY_AURA && spellEntry->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_HEAL)
        {
            isDotHeal = true;
            break;
        }
    }

    char const* directHealStr = GetMangosString(LANG_DIRECT_HEAL);
    char const* directDamageStr = GetMangosString(LANG_DIRECT_DAMAGE);
    char const* dotHealStr = GetMangosString(LANG_DOT_HEAL);
    char const* dotDamageStr = GetMangosString(LANG_DOT_DAMAGE);

    PSendSysMessage(LANG_SPELLCOEFS, spellid, isDirectHeal ? directHealStr : directDamageStr,
                    direct_calc, direct_calc * SCALE_SPELLPOWER_HEALING, spellEntry->effectBonusCoefficient[0], 0.0f);
    PSendSysMessage(LANG_SPELLCOEFS, spellid, isDotHeal ? dotHealStr : dotDamageStr,
                    dot_calc, dot_calc * SCALE_SPELLPOWER_HEALING, spellEntry->effectBonusCoefficient[0], 0.0f);

    return true;
}

bool ChatHandler::HandleDebugSpellModsCommand(char* args)
{
    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr)
        return false;

    Opcodes opcode;
    if (strncmp(typeStr, "flat", strlen(typeStr)) == 0)
        opcode = SMSG_SET_FLAT_SPELL_MODIFIER;
    else if (strncmp(typeStr, "pct", strlen(typeStr)) == 0)
        opcode = SMSG_SET_PCT_SPELL_MODIFIER;
    else
        return false;

    uint32 effidx;
    if (!ExtractUInt32(&args, effidx) || effidx >= 64 + 32)
        return false;

    uint32 spellmodop;
    if (!ExtractUInt32(&args, spellmodop) || spellmodop >= MAX_SPELLMOD)
        return false;

    int32 value;
    if (!ExtractInt32(&args, value))
        return false;

    Player* chr = getSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(chr))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_SPELLMODS, opcode == SMSG_SET_FLAT_SPELL_MODIFIER ? "flat" : "pct",
                    spellmodop, value, effidx, GetNameLink(chr).c_str());
    if (needReportToTarget(chr))
        ChatHandler(chr).PSendSysMessage(LANG_YOURS_SPELLMODS_CHANGED, GetNameLink().c_str(),
                                         opcode == SMSG_SET_FLAT_SPELL_MODIFIER ? "flat" : "pct", spellmodop, value, effidx);

    WorldPacket data(opcode, (1 + 1 + 2 + 2));
    data << uint8(effidx);
    data << uint8(spellmodop);
    data << int32(value);
    chr->GetSession()->SendPacket(data);

    return true;
}

bool ChatHandler::HandleDebugTaxiCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    player->ToggleTaxiDebug();
    PSendSysMessage(LANG_COMMAND_TAXI_DEBUG, (player->IsTaxiDebug() ? GetMangosString(LANG_ON) : GetMangosString(LANG_OFF)));
    return true;
}

bool ChatHandler::HandleShowTemporarySpawnList(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();

    std::map<uint32, uint32> temp_creature = pPlayer->GetMap()->GetTempCreatures();

    SendSysMessage("Current temporary creatures in player map.");

    for (std::map<uint32, uint32>::iterator it = temp_creature.begin(); it != temp_creature.end(); ++it)
        PSendSysMessage("Entry: %u, Count: %u ", (*it).first, (*it).second);

    std::map<uint32, uint32> temp_pet = pPlayer->GetMap()->GetTempPets();
    SendSysMessage("Current temporary pets in player map.");

    for (std::map<uint32, uint32>::iterator it = temp_pet.begin(); it != temp_pet.end(); ++it)
        PSendSysMessage("Entry: %u, Count: %u ", (*it).first, (*it).second);

    return true;
}

bool ChatHandler::HandleGridsLoadedCount(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    if (!player)
        return false;

    PSendSysMessage("There are currently %u loaded grids.", player->GetMap()->GetLoadedGridsCount());
    return true;
}

bool ChatHandler::HandleDebugWaypoint(char* args)
{
    Creature* target = getSelectedCreature();
    if (!target)
        return false;

    uint32 pathId;
    if (!ExtractUInt32(&args, pathId) || pathId >= 256)
    {
        PSendSysMessage("Current target path ID: %d", target->GetMotionMaster()->GetPathId());
        return true;
    }

    target->GetMotionMaster()->MoveWaypoint(pathId, 2);

    return true;
}

bool ChatHandler::HandleDebugByteFields(char* args)
{
    Creature* target = getSelectedCreature();
    if (!target)
        return false;

    int32 fieldNum;
    if (!ExtractInt32(&args, fieldNum))
        return false;

    uint32 byte;
    if (!ExtractUInt32(&args, byte))
        return false;

    uint32 value;
    if (!ExtractUInt32(&args, value))
        return false;

    switch (fieldNum)
    {
        case 0:
            target->SetByteFlag(UNIT_FIELD_BYTES_0, byte, value);
            break;
        case 1:
            target->SetByteFlag(UNIT_FIELD_BYTES_1, byte, value);
            break;
        case 2:
            target->SetByteFlag(UNIT_FIELD_BYTES_2, byte, value);
            break;
        case -10:
            target->RemoveByteFlag(UNIT_FIELD_BYTES_0, byte, value);
            break;
        case -1:
            target->RemoveByteFlag(UNIT_FIELD_BYTES_1, byte, value);
            break;
        case -2:
            target->RemoveByteFlag(UNIT_FIELD_BYTES_2, byte, value);
            break;
        default:
            break;
    }

    return true;
}

bool ChatHandler::HandleDebugSpellVisual(char* args)
{
    Unit* target = getSelectedUnit();
    if (!target)
        return false;

    uint32 spellVisualID;
    if (!ExtractUInt32(&args, spellVisualID))
        return false;

    target->PlaySpellVisual(spellVisualID);
    return true;
}

bool ChatHandler::HandleDebugMoveflags(char* /*args*/)
{
    Unit* target = getSelectedUnit();
    if (!target)
        return false;

    PSendSysMessage("Moveflags on target %u", target->m_movementInfo.GetMovementFlags());
    return true;
}

bool ChatHandler::HandleSD2HelpCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    if (InstanceData* data = player->GetMap()->GetInstanceData())
        data->ShowChatCommands(this);
    else
        PSendSysMessage("Map script does not support chat commands.");
    return true;
}

bool ChatHandler::HandleSD2ScriptCommand(char* args)
{
    Player* player = m_session->GetPlayer();
    if (InstanceData* data = player->GetMap()->GetInstanceData())
        data->ExecuteChatCommand(this, args);
    else
        PSendSysMessage("Map script does not support chat commands.");
    return true;
}

bool ChatHandler::HandleDebugSendWorldState(char* args)
{
    Player* player = m_session->GetPlayer();
    uint32 worldState;
    if (!ExtractUInt32(&args, worldState))
        return false;

    uint32 value;
    if (!ExtractUInt32(&args, value))
        return false;

    player->SendUpdateWorldState(worldState, value);
    return true;
}

bool ChatHandler::HandleDebugHaveAtClientCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return false;
    }

    if (player->HasAtClient(target))
        PSendSysMessage("Target %s is at your client.", target->GetName());
    else
        PSendSysMessage("Target %s is not at your client.", target->GetName());

    return true;
}

bool ChatHandler::HandleDebugIsVisibleCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return false;
    }

    Camera& camera = player->GetCamera();
    if (target->isVisibleForInState(player, camera.GetBody(), player->HasAtClient(target)))
        PSendSysMessage("Target %s should be visible at client.", target->GetName());
    else
        PSendSysMessage("Target %s should not be visible at client.", target->GetName());

    return true;
}

bool ChatHandler::HandleDebugOverflowCommand(char* /*args*/)
{
    std::string name("\360\222\214\245\360\222\221\243\360\222\221\251\360\223\213\215\360\223\213\210\360\223\211\241\360\222\214\245\360\222\221\243\360\222\221\251\360\223\213\215\360\223\213\210\360\223\211\241");
    // Overflow: \xd808\xdf25\xd809\xdc63\xd809\xdc69\xd80c\xdecd\xd80c\xdec8\xd80c\xde61\000\xdf25\xd809\xdc63

    normalizePlayerName(name);
    return true;
}

bool ChatHandler::HandleDebugChatFreezeCommand(char* /*args*/)
{
    std::string message("| |01");

    Player* player = m_session->GetPlayer();
    player->Whisper(message, LANG_UNIVERSAL, player->GetObjectGuid());

    return true;
}

bool ChatHandler::HandleDebugObjectFlags(char* args)
{
    char* debugCmd = ExtractLiteralArg(&args);
    std::string debugCmdStr;
    CMDebugCommandTableStruct const* foundCommand = nullptr;

    if (debugCmd)
    {
        debugCmdStr = debugCmd;
        uint32 bestMaches = 0;
        for (std::vector < CMDebugCommandTableStruct>::const_iterator itr = CMDebugCommandTable.begin(); itr < CMDebugCommandTable.end(); ++itr)
        {
            CMDebugCommandTableStruct const* cmd = &*itr;
            for (uint32 i = 0; i < debugCmdStr.length(); ++i)
            {
                if (i < cmd->command.length() && cmd->command[i] == debugCmdStr[i])
                {
                    if (bestMaches < i + 1)
                    {
                        bestMaches = i + 1;
                        foundCommand = cmd;
                    }
                }
                else
                    break;
            }
            if (bestMaches == debugCmdStr.length())
                break;
        }
    }

    if (!debugCmd || !foundCommand)
    {
        if (debugCmd && !foundCommand)
            PSendSysMessage("%s is not a valid command", debugCmdStr.c_str());

        PSendSysMessage("Commands available...");

        for (auto c : CMDebugCommandTable)
            PSendSysMessage("%s > %s", c.command.c_str(), c.description.c_str());

        PSendSysMessage("for all command if you dont specify on/off the flag will be toggled");
        PSendSysMessage("ex: .debug debugobject intPoins on");
        return false;
    }

    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return false;
    }

    if (foundCommand->flag != CMDEBUGFLAG_NONE)
    {
        bool onOff = false;
        if (!ExtractOnOff(&args, onOff))
            onOff = !target->HaveDebugFlag(foundCommand->flag);

        if (onOff)
        {
            PSendSysMessage("%s enabled for %s.", foundCommand->command.c_str(), target->GetGuidStr().c_str());
            target->SetDebugFlag(foundCommand->flag);
        }
        else
        {
            PSendSysMessage("%s disabled for %s.", foundCommand->command.c_str(), target->GetGuidStr().c_str());
            target->ClearDebugFlag(foundCommand->flag);
        }
    }
    else
    {
        if (foundCommand->command == CMDebugCommandTable[0].command)
        {
            // clear all
            target->ClearDebugFlag(CMDebugFlags(~CMDEBUGFLAG_NONE));
        }
        else if (foundCommand->command == CMDebugCommandTable[1].command)
        {
            // set all
            target->SetDebugFlag(CMDebugFlags(~CMDEBUGFLAG_NONE));
        }
    }

    return true;
}

bool ChatHandler::HandleDebugOutPacketHistory(char* /*args*/)
{
    Player* player = getSelectedPlayer();
    if (player == nullptr)
        player = m_session->GetPlayer();

    auto history = player->GetSession()->GetOutOpcodeHistory();
    std::string output = "Opcodes (reverse order):\n";
    for (auto itr = history.rbegin(); itr != history.rend(); ++itr)
    {
        output += LookupOpcodeName(*itr);
        output += "\n";
    }

    SendSysMessage(output.data());
    return true;
}

bool ChatHandler::HandleDebugIncPacketHistory(char* /*args*/)
{
    Player* player = getSelectedPlayer();
    if (player == nullptr)
        player = m_session->GetPlayer();

    auto history = player->GetSession()->GetIncOpcodeHistory();
    std::string output = "Opcodes (reverse order):\n";
    for (auto itr = history.rbegin(); itr != history.rend(); ++itr)
    {
        output += LookupOpcodeName(*itr);
        output += "\n";
    }

    SendSysMessage(output.data());
    return true;
}

bool ChatHandler::HandleDebugTransports(char* /*args*/)
{
    Player* player = GetSession()->GetPlayer();
    if (!player->IsInWorld())
        return true;

    auto& transports = player->GetMap()->GetTransports();
    SendSysMessage("Transports:");
    for (auto transport : transports)
        PSendSysMessage("Name %s Entry %u Position %s", transport->GetName(), transport->GetEntry(), transport->GetPosition().to_string().data());
    return true;
}

bool ChatHandler::HandleDebugSpawnsList(char* /*args*/)
{
    Player* player = GetSession()->GetPlayer();
    if (!player->IsInWorld())
        return true;

    PSendSysMessage("%s", player->GetMap()->GetSpawnManager().GetRespawnList().c_str());
    return true;
}

bool ChatHandler::HandleDebugRespawnDynguid(char* /*args*/)
{
    Creature* target = getSelectedCreature();
    if (!target)
    {
        Player* player = GetSession()->GetPlayer();
        player->GetMap()->GetSpawnManager().RespawnAll();
        return true;
    }

    if (uint32 dbguid = target->GetDbGuid())
        target->GetMap()->GetSpawnManager().RespawnCreature(dbguid);
    return true;
}

bool ChatHandler::HandleDebugPacketLog(char* args)
{
    uint32 value;
    if (!ExtractUInt32(&args, value))
        return false;

    GetSession()->SetPacketLogging(value == 1);
    return true;
}

bool ChatHandler::HandleDebugDbscript(char* args)
{
    Unit* target = getSelectedUnit();
    uint32 scriptType;
    if (!ExtractUInt32(&args, scriptType) || scriptType > SCRIPT_TYPE_INTERNAL)
        return false;

    uint32 chosenId;
    if (!ExtractUInt32(&args, chosenId))
        return false;

    Player* player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    player->GetMap()->ScriptsStart(ScriptMapType(scriptType), chosenId, player, target);
    return true;
}

bool ChatHandler::HandleDebugDbscriptTargeted(char* args)
{
    Unit* source = getSelectedUnit();
    uint32 scriptType;
    if (!ExtractUInt32(&args, scriptType) || scriptType > SCRIPT_TYPE_INTERNAL)
        return false;

    uint32 chosenId;
    if (!ExtractUInt32(&args, chosenId))
        return false;

    uint32 dbGuidTarget;
    if (!ExtractUInt32(&args, dbGuidTarget))
        return false;

    Player* player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    Creature* target = player->GetMap()->GetCreature(dbGuidTarget);
    if (!source || !target)
        return false;

    player->GetMap()->ScriptsStart(ScriptMapType(scriptType), chosenId, source, target);
    return true;
}

bool ChatHandler::HandleDebugDbscriptSourced(char* args)
{
    Unit* target = getSelectedUnit();
    uint32 scriptType;
    if (!ExtractUInt32(&args, scriptType) || scriptType > SCRIPT_TYPE_INTERNAL)
        return false;

    uint32 chosenId;
    if (!ExtractUInt32(&args, chosenId))
        return false;

    uint32 dbGuidSource;
    if (!ExtractUInt32(&args, dbGuidSource))
        return false;

    Player* player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    Creature* source = player->GetMap()->GetCreature(dbGuidSource);
    if (!source || !target)
        return false;

    player->GetMap()->ScriptsStart(ScriptMapType(scriptType), chosenId, source, target);
    return true;
}

bool ChatHandler::HandleDebugDbscriptGuided(char* args)
{
    uint32 scriptType;
    if (!ExtractUInt32(&args, scriptType) || scriptType > SCRIPT_TYPE_INTERNAL)
        return false;

    uint32 chosenId;
    if (!ExtractUInt32(&args, chosenId))
        return false;

    uint32 dbGuidSource;
    if (!ExtractUInt32(&args, dbGuidSource))
        return false;

    uint32 dbGuidTarget;
    if (!ExtractUInt32(&args, dbGuidTarget))
        return false;

    Player* player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    Creature* source = player->GetMap()->GetCreature(dbGuidSource);
    Creature* target = player->GetMap()->GetCreature(dbGuidTarget);
    if (!source || !target)
        return false;

    player->GetMap()->ScriptsStart(ScriptMapType(scriptType), chosenId, source, target);
    return true;
}

bool ChatHandler::HandleDebugLfgCommand(char* /*args*/)
{
    sWorld.GetLFGQueue().ToggleTesting();
    return true;
}