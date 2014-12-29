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
#include "WorldPacket.h"
#include "Log.h"
#include "Corpse.h"
#include "GameObject.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "WorldSession.h"
#include "LootMgr.h"
#include "Object.h"
#include "Group.h"
#include "World.h"
#include "Util.h"
#include "DBCStores.h"
#include "ObjectMgr.h"

void WorldSession::HandleAutostoreLootItemOpcode(WorldPacket& recv_data)
{
    uint8 itemSlot;
    recv_data >> itemSlot;

    DEBUG_LOG("WORLD: CMSG_AUTOSTORE_LOOT_ITEM > requesting item in slot %u", uint32(itemSlot));

    Loot* loot = sLootMgr.GetLoot(_player);

    if (!loot)
    {
        sLog.outError("HandleAutostoreLootItemOpcode> Cannot retrieve loot for player %s", _player->GetGuidStr().c_str());
        return;
    }

    ObjectGuid const& lguid = loot->GetLootGuid();

    LootItem* lootItem = loot->GetLootItemInSlot(itemSlot);

    if (!lootItem)
    {
        _player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    // item may be blocked by roll system or already looted or another cheating possibility
    if (lootItem->isBlocked || lootItem->GetSlotTypeForSharedLoot(_player, loot) == MAX_LOOT_SLOT_TYPE)
    {
        sLog.outError("HandleAutostoreLootItemOpcode> %s have no right to loot itemId(%u)", _player->GetGuidStr().c_str(), lootItem->itemId);
        return;
    }

    InventoryResult result = loot->SendItem(_player, lootItem);

    if (result == EQUIP_ERR_OK && lguid.IsItem())
    {
        if (Item* item = _player->GetItemByGuid(lguid))
            item->SetLootState(ITEM_LOOT_CHANGED);
    }
}

void WorldSession::HandleLootMoneyOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_LOG("WORLD: CMSG_LOOT_MONEY");

    Loot* pLoot = sLootMgr.GetLoot(_player);

    if (!pLoot)
    {
        sLog.outError("HandleLootMoneyOpcode> Cannot retrieve loot for player %s", _player->GetGuidStr().c_str());
        return;
    }

    pLoot->SendGold(_player);
}

void WorldSession::HandleLootOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_LOOT");

    ObjectGuid lguid;
    recv_data >> lguid;

    // Check possible cheat
    if (!_player->isAlive())
        return;

    if (Loot* loot = sLootMgr.GetLoot(_player, lguid))
        loot->ShowContentTo(_player);
}

void WorldSession::HandleLootReleaseOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_LOOT_RELEASE");

    ObjectGuid lguid;
    recv_data >> lguid;

    if (Loot* loot = sLootMgr.GetLoot(_player, lguid))
        loot->Release(_player);

}

void WorldSession::DoLootRelease(ObjectGuid lguid)
{
    Player*  player = GetPlayer();
    Loot*    loot;

    player->SetLootGuid(ObjectGuid());
    player->SendLootRelease(lguid);

    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);

    if (!player->IsInWorld())
        return;

    switch (lguid.GetHigh())
    {
        case HIGHGUID_GAMEOBJECT:
        {
            GameObject* go = GetPlayer()->GetMap()->GetGameObject(lguid);

            // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
            if (!go || ((go->GetOwnerGuid() != _player->GetObjectGuid() && go->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !go->IsWithinDistInMap(_player, INTERACTION_DISTANCE)))
                return;

            loot = &go->loot;

            if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR)
            {
                // locked doors are opened with spelleffect openlock, prevent remove its as looted
                go->UseDoorOrButton();
            }
            else if (loot->isLooted() || go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
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

                loot->clear();
            }
            else
                // not fully looted object
                go->SetLootState(GO_ACTIVATED);
            break;
        }
        case HIGHGUID_CORPSE:                               // ONLY remove insignia at BG
        {
            Corpse* corpse = _player->GetMap()->GetCorpse(lguid);
            if (!corpse || !corpse->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
                return;

            loot = &corpse->loot;

            if (loot->isLooted())
            {
                loot->clear();
                corpse->RemoveFlag(CORPSE_FIELD_DYNAMIC_FLAGS, CORPSE_DYNFLAG_LOOTABLE);
            }
            break;
        }
        case HIGHGUID_ITEM:
        {
            Item* pItem = player->GetItemByGuid(lguid);
            if (!pItem)
                return;

            switch (pItem->loot->lootType)
            {
                // temporary loot in stacking items, clear loot state, no auto loot move
                case LOOT_MILLING:
                case LOOT_PROSPECTING:
                {
                    uint32 count = pItem->GetCount();

                    // >=5 checked in spell code, but will work for cheating cases also with removing from another stacks.
                    if (count > 5)
                        count = 5;

                    // reset loot for allow repeat looting if stack > 5
                    pItem->loot->clear();
                    pItem->SetLootState(ITEM_LOOT_REMOVED);

                    player->DestroyItemCount(pItem, count, true);
                    break;
                }
                // temporary loot, auto loot move
                case LOOT_DISENCHANTING:
                {
                    if (!pItem->loot->isLooted())
                        player->AutoStoreLoot(*pItem->loot); // can be lost if no space
                    pItem->loot->clear();
                    pItem->SetLootState(ITEM_LOOT_REMOVED);
                    player->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
                    break;
                }
                // normal persistence loot
                default:
                {
                    // must be destroyed only if no loot
                    if (pItem->loot->isLooted())
                    {
                        pItem->SetLootState(ITEM_LOOT_REMOVED);
                        player->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
                    }
                    break;
                }
            }
            return;                                         // item can be looted only single player
        }
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:
        {
            Creature* pCreature = GetPlayer()->GetMap()->GetCreature(lguid);

            bool ok_loot = pCreature && pCreature->isAlive() == (player->getClass() == CLASS_ROGUE && pCreature->lootForPickPocketed);
            if (!ok_loot || !pCreature->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
                return;

            loot = &pCreature->loot;

            // update next looter
            if (Group* group = pCreature->GetGroupLootRecipient())
                if (group->GetMasterLooterGuid() == player->GetObjectGuid())
                    group->UpdateLooterGuid(pCreature);

            if (loot->isLooted() && !pCreature->isAlive())
            {
                // for example skinning after normal loot
                //pCreature->PrepareBodyLootState(pCreature);
                pCreature->AllLootRemovedFromCorpse();
            }
            break;
        }
        default:
        {
            sLog.outError("%s is unsupported for looting.", lguid.GetString().c_str());
            return;
        }
    }

    // Player is not looking at loot list, he doesn't need to see updates on the loot list
    loot->RemoveLooter(player->GetObjectGuid());
}

void WorldSession::HandleLootMasterGiveOpcode(WorldPacket& recv_data)
{
    uint8      itemSlot;        // slot sent in LOOT_RESPONSE
    ObjectGuid lootguid;        // the guid of the loot object owner
    ObjectGuid targetGuid;      // the item receiver guid

    recv_data >> lootguid >> itemSlot >> targetGuid;

    Player* target = ObjectAccessor::FindPlayer(targetGuid);
    if (!target)
    {
        sLog.outError("WorldSession::HandleLootMasterGiveOpcode> Cannot retrieve target %s", targetGuid.GetString().c_str());
        return;
    }

    DEBUG_LOG("WorldSession::HandleLootMasterGiveOpcode> Giver = %s, Target = %s.", _player->GetGuidStr().c_str(), targetGuid.GetString().c_str());

    Loot* pLoot = sLootMgr.GetLoot(_player, lootguid);

    if (!pLoot)
    {
        sLog.outError("WorldSession::HandleLootMasterGiveOpcode> Cannot retrieve loot for player %s", _player->GetGuidStr().c_str());
        return;
    }
    
    if (_player->GetObjectGuid() != pLoot->GetMasterLootGuid())
    {
        sLog.outError("WorldSession::HandleLootMasterGiveOpcode> player %s is not the loot master!", _player->GetGuidStr().c_str());
        return;
    }

    LootItem* lootItem = pLoot->GetLootItemInSlot(itemSlot);

    if (!lootItem)
    {
        _player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    // item may be already looted or another cheating possibility
    if (lootItem->GetSlotTypeForSharedLoot(_player, pLoot) == MAX_LOOT_SLOT_TYPE)
    {
        sLog.outError("HandleAutostoreLootItemOpcode> %s have no right to loot itemId(%u)", _player->GetGuidStr().c_str(), lootItem->itemId);
        return;
    }

    InventoryResult result = pLoot->SendItem(_player, lootItem);

    if (result != EQUIP_ERR_OK)
    {
        // send duplicate of error massage to master looter
        if (LootItem* lootItem = pLoot->GetLootItemInSlot(itemSlot))
            _player->SendEquipError(result, NULL, NULL, lootItem->itemId);
        else
            _player->SendEquipError(result, NULL, NULL);
        return;
    }
}

void WorldSession::HandleLootMethodOpcode(WorldPacket& recv_data)
{
    uint32 lootMethod;
    ObjectGuid lootMaster;
    uint32 lootThreshold;
    recv_data >> lootMethod >> lootMaster >> lootThreshold;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    /** error handling **/
    if (!group->IsLeader(GetPlayer()->GetObjectGuid()))
        return;
    /********************/

    // everything is fine, do it
    group->SetLootMethod((LootMethod)lootMethod);
    group->SetMasterLooterGuid(lootMaster);
    group->SetLootThreshold((ItemQualities)lootThreshold);
    group->SendUpdate();
}

void WorldSession::HandleLootRoll(WorldPacket& recv_data)
{
    ObjectGuid lootedTarget;
    uint32 itemSlot;
    uint8  rollType;
    recv_data >> lootedTarget;                              // guid of the item rolled
    recv_data >> itemSlot;
    recv_data >> rollType;

    sLog.outDebug("WORLD RECIEVE CMSG_LOOT_ROLL, From:%s, rollType:%u", lootedTarget.GetString().c_str(), uint32(rollType));

    Group* group = _player->GetGroup();
    if (!group)
        return;

    if (rollType >= ROLL_NOT_EMITED_YET)
        return;

    sLootMgr.PlayerVote(GetPlayer(), lootedTarget, itemSlot, RollVote(rollType));
}

