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
#include "Server/DBCStores.h"
#include "WorldPacket.h"
#include "Server/WorldSession.h"
#include "Globals/ObjectMgr.h"
#include "Spells/SpellMgr.h"
#include "Log.h"
#include "Server/Opcodes.h"
#include "Spells/Spell.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Entities/Totem.h"
#include "Spells/SpellAuras.h"
#include "Loot/LootMgr.h"
#include "Maps/TransportSystem.h"

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
    uint8 bagIndex, slot;
    uint8 cast_flags;                                       // flags (if 0x02 - some additional data are received)
    uint8 cast_count;                                       // next cast if exists (single or not)
    ObjectGuid itemGuid;
    uint32 glyphIndex;                                      // something to do with glyphs?
    uint32 spellid;                                         // casted spell id

    recvPacket >> bagIndex >> slot >> cast_count >> spellid >> itemGuid >> glyphIndex >> cast_flags;

    // TODO: add targets.read() check
    Player* pUser = _player;

    // ignore for remote control state
    if (!pUser->IsSelfMover())
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        return;
    }

    // reject fake data
    if (glyphIndex >= MAX_GLYPH_SLOT_INDEX)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
        return;
    }

    Item* pItem = pUser->GetItemByPos(bagIndex, slot);
    if (!pItem)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
        return;
    }

    if (pItem->GetObjectGuid() != itemGuid)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
        return;
    }

    DETAIL_LOG("WORLD: CMSG_USE_ITEM packet, bagIndex: %u, slot: %u, cast_count: %u, spellid: %u, Item: %u, glyphIndex: %u, unk_flags: %u, data length = %u", bagIndex, slot, cast_count, spellid, pItem->GetEntry(), glyphIndex, cast_flags, (uint32)recvPacket.size());

    ItemPrototype const* proto = pItem->GetProto();
    if (!proto)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, nullptr);
        return;
    }

    // some item classes can be used only in equipped state
    if (proto->InventoryType != INVTYPE_NON_EQUIP && !pItem->IsEquipped())
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, nullptr);
        return;
    }

    InventoryResult msg = pUser->CanUseItem(pItem);
    if (msg != EQUIP_ERR_OK)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(msg, pItem, nullptr);
        return;
    }

    // not allow use item from trade (cheat way only)
    if (pItem->IsInTrade())
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, nullptr);
        return;
    }

    // only allow conjured consumable, bandage, poisons (all should have the 2^21 item flag set in DB)
    if (proto->Class == ITEM_CLASS_CONSUMABLE &&
            !(proto->Flags & ITEM_FLAG_IGNORE_DEFAULT_ARENA_RESTRICTIONS) &&
            pUser->InArena())
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at not read packet tail
        pUser->SendEquipError(EQUIP_ERR_NOT_DURING_ARENA_MATCH, pItem, nullptr);
        return;
    }

    if (pUser->IsInCombat())
    {
        for (const auto& Spell : proto->Spells)
        {
            if (SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(Spell.SpellId))
            {
                if (IsNonCombatSpell(spellInfo))
                {
                    recvPacket.rpos(recvPacket.wpos());     // prevent spam at not read packet tail
                    pUser->SendEquipError(EQUIP_ERR_NOT_IN_COMBAT, pItem, nullptr);
                    return;
                }
            }
        }

        // Prevent potion drink if another potion in processing (client have potions disabled in like case)
        if (pItem->IsPotion() && pUser->GetLastPotionId())
        {
            recvPacket.rpos(recvPacket.wpos());             // prevent spam at not read packet tail
            pUser->SendEquipError(EQUIP_ERR_OBJECT_IS_BUSY, pItem, nullptr);
            return;
        }
    }

    // check also  BIND_WHEN_PICKED_UP and BIND_QUEST_ITEM for .additem or .additemset case by GM (not binded at adding to inventory)
    if (pItem->GetProto()->Bonding == BIND_WHEN_USE || pItem->GetProto()->Bonding == BIND_WHEN_PICKED_UP || pItem->GetProto()->Bonding == BIND_QUEST_ITEM)
    {
        if (!pItem->IsSoulBound())
        {
            pItem->SetState(ITEM_CHANGED, pUser);
            pItem->SetBinding(true);
        }
    }

    SpellCastTargets targets;

    recvPacket >> targets.ReadForCaster(pUser);

    // some spell cast packet including more data (for projectiles)
    HandleClientCastFlags(recvPacket, cast_flags, targets);

    targets.Update(pUser);

    if (!pItem->IsTargetValidForItemUse(targets.getUnitTarget()))
    {
        // free gray item after use fail
        pUser->SendEquipError(EQUIP_ERR_NONE, pItem, nullptr);

        // send spell error
        if (SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellid))
        {
            // for implicit area/coord target spells
            if (IsPointEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_0])) ||
                    IsAreaEffectTarget(SpellTarget(spellInfo->EffectImplicitTargetA[EFFECT_INDEX_0])))
                Spell::SendCastResult(_player, spellInfo, cast_count, SPELL_FAILED_NO_VALID_TARGETS);
            // for explicit target spells
            else
                Spell::SendCastResult(_player, spellInfo, cast_count, SPELL_FAILED_BAD_TARGETS);
        }
        return;
    }

    _player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ITEM_USE);

    // Note: If script stop casting it must send appropriate data to client to prevent stuck item in gray state.
    if (!sScriptDevAIMgr.OnItemUse(pUser, pItem, targets))
    {
        // no script or script not process request by self
        pUser->CastItemUseSpell(pItem, targets, cast_count, glyphIndex, spellid);
    }
}

#define OPEN_CHEST 11437
#define OPEN_SAFE 11535
#define OPEN_CAGE 11792
#define OPEN_BOOTY_CHEST 5107
#define OPEN_STRONGBOX 8517

void WorldSession::HandleOpenItemOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("WORLD: CMSG_OPEN_ITEM packet, data length = " SIZEFMTD, recvPacket.size());

    uint8 bagIndex, slot;

    recvPacket >> bagIndex >> slot;

    DETAIL_LOG("bagIndex: %u, slot: %u", bagIndex, slot);

    Player* pUser = _player;

    // ignore for remote control state
    if (!pUser->IsSelfMover())
        return;

    Item* pItem = pUser->GetItemByPos(bagIndex, slot);
    if (!pItem)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
        return;
    }

    ItemPrototype const* proto = pItem->GetProto();
    if (!proto)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, nullptr);
        return;
    }

    // locked item
    uint32 lockId = proto->LockID;
    if (lockId && !pItem->HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_UNLOCKED))
    {
        LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

        if (!lockInfo)
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, pItem, nullptr);
            sLog.outError("WORLD::OpenItem: item [guid = %u] has an unknown lockId: %u!", pItem->GetGUIDLow(), lockId);
            return;
        }

        // required picklocking
        if (lockInfo->Skill[1] || lockInfo->Skill[0])
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, pItem, nullptr);
            return;
        }
    }

    if (pItem->HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_WRAPPED))// wrapped?
    {
        QueryResult* result = CharacterDatabase.PQuery("SELECT entry, flags FROM character_gifts WHERE item_guid = '%u'", pItem->GetGUIDLow());
        if (result)
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            uint32 flags = fields[1].GetUInt32();

            pItem->SetGuidValue(ITEM_FIELD_GIFTCREATOR, ObjectGuid());
            pItem->SetEntry(entry);
            pItem->SetUInt32Value(ITEM_FIELD_FLAGS, flags);
            pItem->SetState(ITEM_CHANGED, pUser);
            delete result;
        }
        else
        {
            sLog.outError("Wrapped item %u don't have record in character_gifts table and will deleted", pItem->GetGUIDLow());
            pUser->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
            return;
        }

        static SqlStatementID delGifts ;

        SqlStatement stmt = CharacterDatabase.CreateStatement(delGifts, "DELETE FROM character_gifts WHERE item_guid = ?");
        stmt.PExecute(pItem->GetGUIDLow());
    }
    else
    {
        Loot*& loot = pItem->m_loot;
        if (!loot)
            loot = new Loot(pUser, pItem, LOOT_PICKPOCKETING);

        loot->ShowContentTo(pUser);
    }
}

void WorldSession::HandleGameObjectUseOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;

    recv_data >> guid;

    DEBUG_LOG("WORLD: Received opcode CMSG_GAMEOBJ_USE guid: %s", guid.GetString().c_str());

    if (_player->IsBeingTeleported())
        return;

    GameObject* obj = _player->GetMap()->GetGameObject(guid);
    if (!obj)
        return;

    if (!obj->IsAtInteractDistance(_player))
        return;

    // Additional check preventing exploits (ie loot despawned chests)
    if (!obj->IsSpawned())
    {
        sLog.outError("HandleGameObjectUseOpcode: CMSG_GAMEOBJ_USE for despawned GameObject (Entry %u), didn't expect this to happen.", obj->GetEntry());
        return;
    }

    // ignore for remote control state
    if (!_player->IsSelfMover())
    {
        // check player on vehicle
        if (!_player->GetTransportInfo() || !_player->GetTransportInfo()->IsOnVehicle() || !obj->GetGOInfo()->IsUsableMounted())
            return;
    }

    // Never expect this opcode for some type GO's
    if (obj->GetGoType() == GAMEOBJECT_TYPE_GENERIC)
    {
        sLog.outError("HandleGameObjectUseOpcode: CMSG_GAMEOBJ_USE for not allowed GameObject type %u (Entry %u), didn't expect this to happen.", obj->GetGoType(), obj->GetEntry());
        return;
    }

    if (obj->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED)) // we should not allow use of a locked GO
        return;

    if (obj->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE))
        return;

    // Never expect this opcode for non intractable GO's
    if (obj->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT))
    {
        sLog.outError("HandleGameObjectUseOpcode: CMSG_GAMEOBJ_USE for GameObject (Entry %u) with non intractable flag (Flags %u), didn't expect this to happen.", obj->GetEntry(), obj->GetUInt32Value(GAMEOBJECT_FLAGS));
        return;
    }

    // client checks this but needs recheck
    if (obj->GetGOInfo()->CannotBeUsedUnderImmunity() && _player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE))
        return;

    obj->Use(_player);
}

void WorldSession::HandleGameobjectReportUse(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    recvPacket >> guid;

    DEBUG_LOG("WORLD: Received opcode CMSG_GAMEOBJ_REPORT_USE guid: %s", guid.GetString().c_str());

    // ignore for remote control state
    if (!_player->IsSelfMover())
        return;

    GameObject* go = GetPlayer()->GetMap()->GetGameObject(guid);
    if (!go)
        return;

    if (!go->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
        return;

    _player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT, go->GetEntry());
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    uint8  cast_count, cast_flags;
    recvPacket >> cast_count;
    recvPacket >> spellId;
    recvPacket >> cast_flags;                               // flags (if 0x02 - some additional data are received)

    // ignore for remote control state (for player case)
    Unit* mover = _player->GetMover();
    if (mover != _player && mover->GetTypeId() == TYPEID_PLAYER)
    {
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at ignore packet
        return;
    }

    DEBUG_LOG("WORLD: CMSG_CAST_SPELL, spellId - %u, cast_count: %u, unk_flags %u, data length = " SIZEFMTD,
              spellId, cast_count, cast_flags, recvPacket.size());

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown spell id %u", spellId);
        recvPacket.rpos(recvPacket.wpos());                 // prevent spam at ignore packet
        return;
    }

    Aura* triggeredByAura = mover->GetTriggeredByClientAura(spellId);

    Unit* caster = mover;
    if (mover->GetTypeId() == TYPEID_PLAYER)
    {
        // not have spell in spellbook or spell passive and not casted by client

        if ((!((Player*)mover)->HasActiveSpell(spellId) && !triggeredByAura) || IsPassiveSpell(spellInfo))
        {
            sLog.outError("World: %s casts spell %u which he shouldn't have", mover->GetGuidStr().c_str(), spellId);
            // cheater? kick? ban?
            recvPacket.rpos(recvPacket.wpos());             // prevent spam at ignore packet
            return;
        }
    }
    else
    {
        // If the vehicle creature does not have the spell but it allows the passenger to cast own spells
        // change caster to player and let him cast
        if (!_player->GetTransportInfo() || _player->GetTransportInfo()->GetTransport() != caster || Spell::CheckVehicle(_player, *spellInfo) != SPELL_CAST_OK)
        {
            bool isPassive = IsPassiveSpell(spellInfo);
            // not have spell in spellbook or spell passive and not casted by client
            if (!mover->HasSpell(spellId) || isPassive)
            {
                if (!_player->HasSpell(spellId) || isPassive)
                {
                    // cheater? kick? ban?
                    recvPacket.rpos(recvPacket.wpos());             // prevent spam at ignore packet
                    return;
                }
            }
            else
                caster = _player;
        }
    }

    // client provided targets
    SpellCastTargets targets;

#ifdef BUILD_PLAYERBOT
    recvPacket >> targets.ReadForCaster(caster);
#else
    recvPacket >> targets.ReadForCaster(_player);
#endif

    // some spell cast packet including more data (for projectiles)
    HandleClientCastFlags(recvPacket, cast_flags, targets);

    // auto-selection buff level base at target level (in spellInfo)
    if (Unit* target = targets.getUnitTarget())
    {
        // if rank not found then function return nullptr but in explicit cast case original spell can be casted and later failed with appropriate error message
        if (SpellEntry const* actualSpellInfo = sSpellMgr.SelectAuraRankForLevel(spellInfo, target->GetLevel()))
            spellInfo = actualSpellInfo;
    }

    if (HasMissingTargetFromClient(spellInfo))
        targets.setUnitTarget(caster->GetTarget());

    if (_player->HasQueuedSpell())
        return;

    bool handled = false;
    Spell* spell = new Spell(caster, spellInfo, triggeredByAura ? TRIGGERED_OLD_TRIGGERED : TRIGGERED_NONE, caster->GetObjectGuid(), triggeredByAura ? triggeredByAura->GetSpellProto() : nullptr);
    spell->m_cast_count = cast_count;                       // set count of casts
    spell->m_clientCast = true;
    if (!triggeredByAura && (caster->HasGCD(spellInfo) || !caster->IsSpellReady(*spellInfo)))
    {
        if (caster->HasGCDOrCooldownWithinMargin(*spellInfo))
        {
            handled = true;
            _player->SetQueuedSpell(spell);
            GetMessager().AddMessage([guid = caster->GetObjectGuid(), isPlayer = caster != mover, targets = targets](WorldSession* session) mutable
            {
                if (session->GetPlayer()) // in case of logout
                {
                    // in case of mind control end
                    if ((isPlayer && session->GetPlayer()->GetObjectGuid() == guid) || (!isPlayer && session->GetPlayer()->GetMover()->GetObjectGuid() == guid))
                        session->GetPlayer()->CastQueuedSpell(targets);
                    else
                        session->GetPlayer()->ClearQueuedSpell();
                }
            });
        }
    }

    if (!handled)
        spell->SpellStart(&targets, triggeredByAura);
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;

    recvPacket.read_skip<uint8>();                          // counter, increments with every CANCEL packet, don't use for now
    recvPacket >> spellId;

    // ignore for remote control state (for player case)
    Unit* mover = _player->GetMover();
    if (mover != _player && mover->GetTypeId() == TYPEID_PLAYER)
        return;

    if (!_player->IsClientControlled(_player))
        return;

    if (mover->IsNonMeleeSpellCasted(false))
        mover->InterruptNonMeleeSpells(false, spellId);
}

void WorldSession::HandleCancelAuraOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    recvPacket >> spellId;

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return;

    if (spellInfo->HasAttribute(SPELL_ATTR_CANT_CANCEL))
        return;

    if (IsPassiveSpell(spellInfo))
        return;

    SpellAuraHolder* holder = _player->GetSpellAuraHolder(spellId);

    if (!holder)
        return;

    if (!holder->IsPositive())
    {
        // ignore for remote control state
        if (!_player->IsSelfMover())
        {
            // except own aura spells
            bool allow = false;
            for (unsigned int k : spellInfo->EffectApplyAuraName)
            {
                if (k == SPELL_AURA_MOD_POSSESS ||
                    k == SPELL_AURA_MOD_POSSESS_PET)
                {
                    allow = true;
                    break;
                }
            }

            // this also include case when aura not found
            if (!allow)
                return;
        }
        else
            return;
    }

    // channeled spell case (it currently casted then)
    if (IsChanneledSpell(spellInfo))
    {
        if (Spell* curSpell = _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (curSpell->m_spellInfo->Id == spellId)
                _player->InterruptSpell(CURRENT_CHANNELED_SPELL);
        return;
    }

    // not own area auras can't be cancelled (note: maybe need to check for aura on holder and not general on spell)
    if (holder && holder->GetCasterGuid() != _player->GetObjectGuid() && HasAreaAuraEffect(holder->GetSpellProto()))
        return;

    // non channeled case
    _player->RemoveAurasDueToSpellByCancel(spellId);
}

void WorldSession::HandlePetCancelAuraOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    uint32 spellId;

    recvPacket >> guid;
    recvPacket >> spellId;

    // ignore for remote control state
    if (!_player->IsSelfMover())
        return;

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown PET spell id %u", spellId);
        return;
    }

    Creature* pet = GetPlayer()->GetMap()->GetAnyTypeCreature(guid);

    if (!pet)
    {
        sLog.outError("HandlePetCancelAuraOpcode - %s not exist.", guid.GetString().c_str());
        return;
    }

    if (guid != GetPlayer()->GetPetGuid() && !GetPlayer()->HasCharm(guid))
    {
        sLog.outError("HandlePetCancelAura. %s isn't pet of %s", guid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        return;
    }

    if (!pet->IsAlive())
    {
        pet->SendPetActionFeedback(FEEDBACK_PET_DEAD);
        return;
    }

    pet->RemoveAurasDueToSpell(spellId);

    // TODO: check if its correctly handled in aura remove
    //pet->AddCreatureSpellCooldown(spellId);
}

void WorldSession::HandleCancelGrowthAuraOpcode(WorldPacket& /*recvPacket*/)
{
    // nothing do
}

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& /*recvPacket*/)
{
    // cancel and prepare for deleting
    // do not send SMSG_CANCEL_AUTO_REPEAT! client will send this Opcode again (loop)
    _player->GetMover()->InterruptSpell(CURRENT_AUTOREPEAT_SPELL, true, false);
}

void WorldSession::HandleCancelChanneling(WorldPacket& recv_data)
{
    recv_data.read_skip<uint32>();                          // spellid, not used

    // ignore for remote control state (for player case)
    Unit* mover = _player->GetMover();
    if (mover != _player && mover->GetTypeId() == TYPEID_PLAYER)
        return;

    mover->InterruptSpell(CURRENT_CHANNELED_SPELL);
}

void WorldSession::HandleTotemDestroyed(WorldPacket& recvPacket)
{
    uint8 slotId;

    recvPacket >> slotId;

    // ignore for remote control state
    if (!_player->IsSelfMover())
        return;

    if (int(slotId) >= MAX_TOTEM_SLOT)
        return;

    if (Totem* totem = GetPlayer()->GetTotem(TotemSlot(slotId)))
        totem->UnSummon();
}

void WorldSession::HandleSelfResOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: CMSG_SELF_RES");                  // empty opcode

    if (_player->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return;

    if (_player->GetUInt32Value(PLAYER_SELF_RES_SPELL))
    {
        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(_player->GetUInt32Value(PLAYER_SELF_RES_SPELL));
        if (spellInfo)
            _player->CastSpell(_player, spellInfo, TRIGGERED_NONE);

        _player->SetUInt32Value(PLAYER_SELF_RES_SPELL, 0);
    }
}

void WorldSession::HandleSpellClick(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;

    // client prevent click and set different icon at combat state; however combat state is allowed for dungeons
    if (_player->IsInCombat() && !_player->GetMap()->IsDungeon())
        return;

    Creature* unit = _player->GetMap()->GetAnyTypeCreature(guid);
    if (!unit || unit->IsInCombat())                        // client prevent click and set different icon at combat state
        return;

    SpellClickInfoMapBounds clickPair = sObjectMgr.GetSpellClickInfoMapBounds(unit->GetEntry());
    for (SpellClickInfoMap::const_iterator itr = clickPair.first; itr != clickPair.second; ++itr)
    {
        if (itr->second.IsFitToRequirements(_player, unit))
        {
            if (sScriptDevAIMgr.OnNpcSpellClick(_player, unit, itr->second.spellId))
                return;

            Unit* caster = (itr->second.castFlags & 0x1) ? (Unit*)_player : (Unit*)unit;
            Unit* target = (itr->second.castFlags & 0x2) ? (Unit*)_player : (Unit*)unit;

            if (itr->second.spellId)
                caster->CastSpell(target, itr->second.spellId, TRIGGERED_OLD_TRIGGERED);
            else
                sLog.outError("WorldSession::HandleSpellClick: npc_spell_click with entry %u has 0 in spell_id. Not handled custom case?", unit->GetEntry());
        }
    }
}

void WorldSession::HandleGetMirrorimageData(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: CMSG_GET_MIRRORIMAGE_DATA");

    ObjectGuid guid;
    recv_data >> guid;

    Creature* pCreature = _player->GetMap()->GetAnyTypeCreature(guid);

    if (!pCreature)
        return;

    Unit::AuraList const& images = pCreature->GetAurasByType(SPELL_AURA_MIRROR_IMAGE);

    if (images.empty())
        return;

    Unit* pCaster = images.front()->GetCaster();

    WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);

    data << guid;
    data << (uint32)pCreature->GetDisplayId();

    data << (uint8)pCreature->getRace();
    data << (uint8)pCreature->getGender();
    data << (uint8)pCreature->getClass();

    if (pCaster && pCaster->GetTypeId() == TYPEID_PLAYER)
    {
        Player* pPlayer = (Player*)pCaster;

        // skin, face, hair, haircolor
        data << (uint8)pPlayer->GetByteValue(PLAYER_BYTES, 0);
        data << (uint8)pPlayer->GetByteValue(PLAYER_BYTES, 1);
        data << (uint8)pPlayer->GetByteValue(PLAYER_BYTES, 2);
        data << (uint8)pPlayer->GetByteValue(PLAYER_BYTES, 3);

        // facial hair
        data << (uint8)pPlayer->GetByteValue(PLAYER_BYTES_2, 0);

        // guild id
        data << (uint32)pPlayer->GetGuildId();

        if (pPlayer->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
            data << (uint32)0;
        else
            data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD);

        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);

        if (pPlayer->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK))
            data << (uint32)0;
        else
            data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);

        data << (uint32)pPlayer->GetItemDisplayIdInSlot(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_TABARD);
    }
    else
    {
        // pCaster may have been nullptr (usually not expected, but may happen at disconnect, etc)
        // OR
        // pCaster is not player, data is taken from CreatureDisplayInfoExtraEntry by model already
        data << (uint8)0;
        data << (uint8)0;
        data << (uint8)0;
        data << (uint8)0;

        data << (uint8)0;

        data << (uint32)0;

        for (int i = 0; i < 11; ++i)
            data << (uint32)0;
    }

    SendPacket(data);
}

void WorldSession::HandleUpdateMissileTrajectory(WorldPacket& recv_data)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: CMSG_UPDATE_MISSILE_TRAJECTORY");

    ObjectGuid guid;
    uint32 spellId;
    float elevation, speed;
    float srcX, srcY, srcZ;
    float dstX, dstY, dstZ;
    uint8 moveFlag;

    recv_data >> guid;

    recv_data >> spellId;
    recv_data >> elevation;
    recv_data >> speed;
    recv_data >> srcX >> srcY >> srcZ;
    recv_data >> dstX >> dstY >> dstZ;
    recv_data >> moveFlag;

    Unit* unit = ObjectAccessor::GetUnit(*GetPlayer(), guid);
    if (!unit)
        return;

    Spell* spell = unit ? unit->GetCurrentSpell(CURRENT_GENERIC_SPELL) : nullptr;
    if (!spell || spell->m_spellInfo->Id != spellId || !(spell->m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WorldSession::HandleUpdateMissileTrajectory spell %u ajusted coords: %f/%f %f/%f %f/%f %f/%f %f/%f %f/%f speed %f/%f elevation %f/%f",
        spellId,
        spell->m_targets.m_srcPos.x, srcX, spell->m_targets.m_srcPos.y, srcY, spell->m_targets.m_srcPos.z, srcZ,
        spell->m_targets.m_destPos.x, dstX, spell->m_targets.m_destPos.y, dstY, spell->m_targets.m_destPos.z, dstZ,
        spell->m_targets.getSpeed(), speed, spell->m_targets.getElevation(), elevation);

    spell->m_targets.setSource(srcX, srcY, srcZ);
    spell->m_targets.setDestination(dstX, dstY, dstZ);
    spell->m_targets.setElevation(elevation);
    spell->m_targets.setSpeed(speed);

    if (moveFlag)
    {
        uint32 opcode;
        recv_data >> opcode;
        if (opcode != MSG_MOVE_STOP)
            return; // hacking attempt
        recv_data.SetOpcode(Opcodes(opcode));
        HandleMovementOpcodes(recv_data);
    }
}

void WorldSession::HandleOnMissileTrajectoryCollision(WorldPacket& recvPacket)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: CMSG_ON_MISSILE_TRAJECTORY_COLLISION");

    ObjectGuid casterGuid;
    uint32 spellId;
    uint8 castCount;
    float x, y, z;    // Position of missile hit

    recvPacket >> casterGuid;
    recvPacket >> spellId;
    recvPacket >> castCount;
    recvPacket >> x;
    recvPacket >> y;
    recvPacket >> z;

    Unit* caster = ObjectAccessor::GetUnit(*_player, casterGuid);
    if (!caster)
        return;

    Spell* spell = caster->FindCurrentSpellBySpellId(spellId);
    if (!spell || !(spell->m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        return;

    spell->m_targets.setDestination(x, y, z);

    // we changed dest, recalculate flight time
    spell->RecalculateDelayMomentForDest();

    WorldPacket data(SMSG_NOTIFY_MISSILE_TRAJECTORY_COLLISION, 21);
    data << uint64(casterGuid);
    data << uint8(castCount);
    data << float(x);
    data << float(y);
    data << float(z);
    caster->SendMessageToSet(data, true);
}

void WorldSession::HandleClientCastFlags(WorldPacket& recv_data, uint8 castFlags, SpellCastTargets& targets)
{
    // some spell cast packet including more data (for projectiles?)
    if (castFlags & 0x02)
    {
        // not sure about these two
        float elevation, speed;
        recv_data >> elevation;
        recv_data >> speed;

        targets.setElevation(elevation);
        targets.setSpeed(speed);

        uint8 hasMovementData;
        recv_data >> hasMovementData;
        if (hasMovementData)
        {
            uint32 opcode;
            recv_data >> opcode;
            if (opcode != MSG_MOVE_STOP)
                return; // hacking attempt
            recv_data.SetOpcode(Opcodes(opcode));
            HandleMovementOpcodes(recv_data);
        }
    }
}
