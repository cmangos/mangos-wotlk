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

#include "TC9GrpcHandler.h"
#include "Globals/ObjectAccessor.h"
#include "Entities/Item.h"
#include "Entities/Bag.h"
#include "Entities/Player.h"

GetPlayerItemsByGuidsResponse ToCloud9GrpcHandler::GetPlayerItemsByGuids(uint64 playerGuid, uint64* items, int itemsLen)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        GetPlayerItemsByGuidsResponse resp;
        resp.errorCode = PlayerItemErrorCodePlayerNotFound;
        return resp;
    }

    int itemsFound = 0;
    std::unique_ptr<Item* []> foundItems(new Item * [itemsLen]);
    for (int i = 0; i < itemsLen; i++)
    {
        foundItems[i] = player->GetItemByGuid(ObjectGuid(items[i]));
        if (foundItems[i])
            itemsFound++;
    }

    // Don't forget to delete on "that" side.
    PlayerItem* itemsResult = (PlayerItem*)malloc(sizeof(PlayerItem) * itemsFound);
    int itemsResultsItr = 0;
    for (int i = 0; i < itemsLen; i++)
    {
        if (!foundItems[i])
            continue;

        Item* pItem = foundItems[i];

        PlayerItem item;
        item.guid = pItem->GetObjectGuid().GetRawValue();
        item.entry = pItem->GetEntry();
        item.owner = playerGuid;
        item.bagSlot = pItem->GetBagSlot();
        item.slot = pItem->GetSlot();
        item.isTradable = pItem->CanBeTraded(true);
        item.count = pItem->GetCount();
        item.flags = pItem->GetUInt32Value(ITEM_FIELD_FLAGS);
        item.durability = pItem->GetUInt32Value(ITEM_FIELD_DURABILITY);
        item.randomPropertyID = pItem->GetItemRandomPropertyId();

        // Don't forget to delete on "that" side.
        char *text = (char*)malloc(sizeof(char) * (pItem->GetText().length() + 1));
        strcpy(text, pItem->GetText().c_str());
        item.text = text;

        itemsResult[itemsResultsItr] = item;

        itemsResultsItr++;
    }

    GetPlayerItemsByGuidsResponse resp;
    resp.errorCode = PlayerItemErrorCodeNoError;
    resp.items = itemsResult;
    resp.itemsSize = itemsFound;
    return resp;
}

RemoveItemsWithGuidsFromPlayerResponse ToCloud9GrpcHandler::RemoveItemsWithGuidsFromPlayer(uint64 playerGuid, uint64* items, int itemsLen, uint64 assignToPlayerGuid)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        RemoveItemsWithGuidsFromPlayerResponse resp;
        resp.errorCode = PlayerItemErrorCodePlayerNotFound;
        return resp;
    }


    int itemsFound = 0;
    std::unique_ptr<uint64[]> deletedItems(new uint64 [itemsLen]);
    for (int i = 0; i < itemsLen; i++)
    {
        Item *item = player->GetItemByGuid(ObjectGuid(items[i]));
        if (!item)
        {
            deletedItems[i] = 0;
            continue;
        }

        itemsFound++;
        deletedItems[i] = item->GetObjectGuid().GetRawValue();

        player->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);

        item->DeleteFromInventoryDB();
        item->SetOwnerGuid(ObjectGuid(assignToPlayerGuid));
        item->SetState(ITEM_CHANGED);
        item->SaveToDB();

        delete item;
    }

    if (itemsFound > 0)
    {
        player->SaveInventoryAndGoldToDB();
    }

    // Don't forget to delete on "that" side.
    uint64_t* itemsResult = (uint64_t*)malloc(sizeof(uint64_t) * itemsFound);
    int itemsResultsItr = 0;
    for (int i = 0; i < itemsLen; i++)
    {
        if (deletedItems[i] == 0)
            continue;

        itemsResult[itemsResultsItr] = deletedItems[i];
        itemsResultsItr++;
    }

    RemoveItemsWithGuidsFromPlayerResponse resp;
    resp.errorCode = PlayerItemErrorCodeNoError;
    resp.updatedItems = itemsResult;
    resp.updatedItemsSize = itemsResultsItr;
    return resp;
}

PlayerItemErrorCode ToCloud9GrpcHandler::AddExistingItemToPlayer(AddExistingItemToPlayerRequest* request)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(request->playerGuid));
    if (!player)
        return PlayerItemErrorCodePlayerNotFound;

    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(request->itemEntry);
    if (!proto)
        return PlayerItemErrorUnknownTemplate;

    Item* item = NewItemOrBag(proto);
    if (!item->Create(ObjectGuid(request->itemGuid).GetCounter(), request->itemEntry, player))
    {
        delete item;
        return PlayerItemErrorFailedToCreateItem;
    }

    item->SetUInt32Value(ITEM_FIELD_FLAGS, request->itemFlags);
    item->SetUInt32Value(ITEM_FIELD_DURABILITY, request->itemDurability);
    item->SetItemRandomProperties(request->itemRandomPropertyID);
    item->SetCount(request->itemCount);

    // TODO: Add text.

    ItemPosCountVec dest;
    uint8 msg = player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false);
    if (msg != EQUIP_ERR_OK)
    {
        delete item;
        return PlayerItemErrorNoInventorySpace;
    }

    player->MoveItemToInventory(dest, item, true);
    player->SaveInventoryAndGoldToDB();

    return PlayerItemErrorCodeNoError;
}

GetMoneyForPlayerResponse ToCloud9GrpcHandler::GetMoneyForPlayer(uint64 playerGuid)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        GetMoneyForPlayerResponse resp;
        resp.errorCode = PlayerMoneyErrorCodePlayerNotFound;
        return resp;
    }

    GetMoneyForPlayerResponse resp;
    resp.errorCode = PlayerMoneyErrorCodeNoError;
    resp.money = player->GetMoney();
    return resp;
}

ModifyMoneyForPlayerResponse ToCloud9GrpcHandler::ModifyMoneyForPlayer(uint64 playerGuid, int32 value)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        ModifyMoneyForPlayerResponse resp;
        resp.errorCode = PlayerMoneyErrorCodePlayerNotFound;
        return resp;
    }

    // TODO: add overflow check.
    player->ModifyMoney(value);

    ModifyMoneyForPlayerResponse resp;
    resp.errorCode = PlayerMoneyErrorCodeNoError;
    resp.newMoneyValue = player->GetMoney();
    return resp;
}

CanPlayerInteractWithGOAndTypeResponse ToCloud9GrpcHandler::CanPlayerInteractWithGOAndType(uint64 playerGuid, uint64 go, uint8 goType)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        CanPlayerInteractWithGOAndTypeResponse resp;
        resp.errorCode = PlayerInteractionErrorCodeCodePlayerNotFound;
        return resp;
    }

    CanPlayerInteractWithGOAndTypeResponse resp;
    resp.errorCode = PlayerInteractionErrorCodeNoError;
    resp.canInteract = player->GetGameObjectIfCanInteractWith(ObjectGuid(go), (GameobjectTypes)goType) != nullptr;
    return resp;
}

CanPlayerInteractWithNPCAndFlagsResponse ToCloud9GrpcHandler::CanPlayerInteractWithNPCAndFlags(uint64 playerGuid, uint64 npc, uint32 unitFlags)
{
    Player *player = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
    if (!player)
    {
        CanPlayerInteractWithNPCAndFlagsResponse resp;
        resp.errorCode = PlayerInteractionErrorCodeCodePlayerNotFound;
        return resp;
    }

    CanPlayerInteractWithNPCAndFlagsResponse resp;
    resp.errorCode = PlayerInteractionErrorCodeNoError;
    resp.canInteract = player->GetNPCIfCanInteractWith(ObjectGuid(npc), (NPCFlags)unitFlags) != nullptr;
    return resp;
}
