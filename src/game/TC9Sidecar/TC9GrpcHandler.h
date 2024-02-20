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

#ifndef _TC9_GRPC_HANDLER_H
#define _TC9_GRPC_HANDLER_H

#include "Common.h"
#include "libsidecar.h"

class ToCloud9GrpcHandler
{
public:
    ToCloud9GrpcHandler() {};
    ~ToCloud9GrpcHandler() {};

    // Items
    static GetPlayerItemsByGuidsResponse          GetPlayerItemsByGuids(uint64 player, uint64* items, int items_len);
    static RemoveItemsWithGuidsFromPlayerResponse RemoveItemsWithGuidsFromPlayer(uint64 player, uint64* items, int itemsLen, uint64 assignToPlayer);
    static PlayerItemErrorCode                    AddExistingItemToPlayer(AddExistingItemToPlayerRequest*);

    // Money
    static GetMoneyForPlayerResponse    GetMoneyForPlayer(uint64 player);
    static ModifyMoneyForPlayerResponse ModifyMoneyForPlayer(uint64 player, int32 value);

    // Interactions
    static CanPlayerInteractWithGOAndTypeResponse   CanPlayerInteractWithGOAndType(uint64 player, uint64 go, uint8 goType);
    static CanPlayerInteractWithNPCAndFlagsResponse CanPlayerInteractWithNPCAndFlags(uint64 player, uint64 npc, uint32 unitFlags);
};

#endif // _TC9_GRPC_HANDLER_H
