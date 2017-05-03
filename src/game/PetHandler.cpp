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
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "AI/BaseAI/CreatureAI.h"
#include "Util.h"
#include "Pet.h"
#include "SpellAuras.h"

void WorldSession::HandlePetAction(WorldPacket& recv_data)
{
    ObjectGuid petGuid;
    uint32 data;
    ObjectGuid targetGuid;
    recv_data >> petGuid;
    recv_data >> data;
    recv_data >> targetGuid;

    uint32 spellid = UNIT_ACTION_BUTTON_ACTION(data);
    uint8 flag = UNIT_ACTION_BUTTON_TYPE(data);             // delete = 0x07 CastSpell = C1

    DETAIL_LOG("HandlePetAction: %s flag is %u, spellid is %u, target %s.", petGuid.GetString().c_str(), uint32(flag), spellid, targetGuid.GetString().c_str());

    // used also for charmed creature/player
    Unit* petUnit = _player->GetMap()->GetUnit(petGuid);
    if (!petUnit)
    {
        sLog.outError("HandlePetAction: %s not exist.", petGuid.GetString().c_str());
        return;
    }

    if (_player->GetObjectGuid() != petUnit->GetCharmerOrOwnerGuid())
    {
        sLog.outError("HandlePetAction: %s isn't controlled by %s.", petGuid.GetString().c_str(), _player->GetGuidStr().c_str());
        return;
    }

    if (!petUnit->isAlive())
        return;

    CharmInfo* charmInfo = petUnit->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", petUnit->GetGUIDLow(), petUnit->GetTypeId());
        return;
    }

    Pet* pet = nullptr;
    Creature* creature = nullptr;

    if (petUnit->GetTypeId() == TYPEID_UNIT)
    {
        creature = static_cast<Creature*>(petUnit);

        if (creature->IsPet())
        {
            pet = static_cast<Pet*>(petUnit);

            if (pet->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)
                return;
        }
    }

    if (!pet)
    {
        if (petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
        {
            // possess case
            if (flag != uint8(ACT_COMMAND))
            {
                sLog.outError("PetHAndler: unknown PET flag Action %i and spellid %i. For possessed %s", uint32(flag), spellid, petUnit->GetGuidStr().c_str());
                return;
            }

            switch (spellid)
            {
                case COMMAND_STAY:
                case COMMAND_FOLLOW:
                    charmInfo->SetCommandState(CommandStates(spellid));
                    break;
                case COMMAND_ATTACK:
                {
                    Unit* targetUnit = targetGuid ? _player->GetMap()->GetUnit(targetGuid) : nullptr;

                    if (targetUnit && targetUnit != petUnit && targetUnit->isTargetableForAttack())
                    {
                        // This is true if pet has no target or has target but targets differs.
                        if (petUnit->getVictim() != targetUnit)
                            petUnit->Attack(targetUnit, true);
                    }
                    break;
                }
                case COMMAND_DISMISS:
                    _player->Uncharm();
                    break;
                default:
                    sLog.outError("PetHandler: Not allowed action %i and spellid %i. Pet %s owner is %s", uint32(flag), spellid, petUnit->GetGuidStr().c_str(), _player->GetGuidStr().c_str());
                    break;
            }
        }

        if (!petUnit->GetCharmerGuid())
            return;
    }

    switch (flag)
    {
        case ACT_COMMAND:                                   // 0x07
            switch (spellid)
            {
                case COMMAND_STAY:                          // flat=1792  // STAY
                {
                    if (!petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
                    {
                        petUnit->StopMoving();
                        petUnit->GetMotionMaster()->Clear();
                    }
                    petUnit->AttackStop(true, true);
                    charmInfo->SetCommandState(COMMAND_STAY);
                    break;
                }
                case COMMAND_FOLLOW:                        // spellid=1792  // FOLLOW
                {
                    if (!petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
                    {
                        petUnit->StopMoving();
                        petUnit->GetMotionMaster()->Clear();
                        charmInfo->SetIsRetreating(true);
                    }
                    petUnit->AttackStop(true, true);
                    charmInfo->SetCommandState(COMMAND_FOLLOW);
                    break;
                }
                case COMMAND_ATTACK:                        // spellid=1792  // ATTACK
                {
                    charmInfo->SetIsRetreating();
                    charmInfo->SetSpellOpener();

                    Unit* targetUnit = targetGuid ? _player->GetMap()->GetUnit(targetGuid) : nullptr;

                    if (targetUnit && targetUnit != petUnit && targetUnit->isTargetableForAttack() && targetUnit->isInAccessablePlaceFor((Creature*)petUnit))
                    {
                        // This is true if pet has no target or has target but targets differs.
                        if (petUnit->getVictim() != targetUnit)
                        {
                            petUnit->AttackStop();
                            if (!petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
                            {
                                petUnit->GetMotionMaster()->Clear();

                                petUnit->AI()->AttackStart(targetUnit);

                                if (pet)
                                {
                                    // 10% chance to play special warlock pet attack talk, else growl
                                    if (pet->getPetType() == SUMMON_PET && roll_chance_i(10))
                                        pet->SendPetTalk((uint32)PET_TALK_ATTACK);

                                    pet->SendPetAIReaction();
                                }
                            }
                            else
                                petUnit->Attack(targetUnit, true);
                        }
                    }
                    break;
                }
                case COMMAND_DISMISS:                       // dismiss permanent pet, remove temporary pet, uncharm unit
                {
                    if (pet)
                    {
                        // No action for Hunter pets, Hunters must use their Dismiss Pet spell
                        if (pet->getPetType() != HUNTER_PET)
                            petUnit->SetDeathState(CORPSE);
                    }
                    else
                    {
                        // dismissing a summoned pet is like killing them (this prevents returning a soulshard...)
                        if (creature && creature->IsTemporarySummon())
                            petUnit->SetDeathState(CORPSE);
                        else
                            _player->Uncharm();
                    }

                    charmInfo->SetStayPosition();
                    break;
                }
                default:
                    sLog.outError("WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
            }
            break;
        case ACT_REACTION:                                  // 0x6
            switch (spellid)
            {
                case REACT_PASSIVE:                         // passive
                {
                    petUnit->AttackStop(true, true);
                    charmInfo->SetSpellOpener();
                }
                case REACT_DEFENSIVE:                       // recovery
                case REACT_AGGRESSIVE:                      // activete
                {
                    charmInfo->SetReactState(ReactStates(spellid));
                    break;
                }
            }
            break;
        case ACT_DISABLED:                                  // 0x81    spell (disabled), ignore
        case ACT_PASSIVE:                                   // 0x01
        case ACT_ENABLED:                                   // 0xC1    spell
        {
            charmInfo->SetIsRetreating();
            charmInfo->SetSpellOpener();

            Unit* unit_target = targetGuid ? _player->GetMap()->GetUnit(targetGuid) : nullptr;

            // do not cast unknown spells
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellid);
            if (!spellInfo)
            {
                sLog.outError("WORLD: unknown PET spell id %i", spellid);
                return;
            }

            if (petUnit->GetCharmInfo() && petUnit->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
                return;

            for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                if (spellInfo->EffectImplicitTargetA[i] == TARGET_ALL_ENEMY_IN_AREA
                    || spellInfo->EffectImplicitTargetA[i] == TARGET_ALL_ENEMY_IN_AREA_INSTANT
                    || spellInfo->EffectImplicitTargetA[i] == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                    return;
            }

            // do not cast not learned spells
            if (!petUnit->HasSpell(spellid) || IsPassiveSpell(spellInfo))
                return;

            _player->SetInCombatState(true, unit_target);

            petUnit->clearUnitState(UNIT_STAT_MOVING);

            Spell* spell = new Spell(petUnit, spellInfo, false);

            SpellCastResult result = spell->CheckPetCast(unit_target);

            const SpellRangeEntry* sRange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);

            if (unit_target && !(petUnit->IsWithinDistInMap(unit_target, sRange->maxRange) && petUnit->IsWithinLOSInMap(unit_target))
                && !(GetPlayer()->IsFriendlyTo(unit_target) || petUnit->HasAuraType(SPELL_AURA_MOD_POSSESS)))
            {
                charmInfo->SetSpellOpener(spellid, sRange->minRange, sRange->maxRange);
                spell->finish(false);
                delete spell;

                petUnit->AttackStop();

                if (!petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
                {
                    petUnit->GetMotionMaster()->Clear();

                    petUnit->AI()->AttackStart(unit_target);
                    // 10% chance to play special warlock pet attack talk, else growl
                    if (pet && pet->getPetType() == SUMMON_PET && pet != unit_target && roll_chance_i(10))
                        petUnit->SendPetTalk((uint32)PET_TALK_ATTACK);

                    petUnit->SendPetAIReaction();
                }
                else
                    petUnit->Attack(unit_target, true);

                return;
            }

            // auto turn to target unless possessed
            if (result == SPELL_FAILED_UNIT_NOT_INFRONT && !petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
            {
                if (unit_target)
                {
                    petUnit->SetInFront(unit_target);
                    if (unit_target->GetTypeId() == TYPEID_PLAYER)
                        petUnit->SendCreateUpdateToPlayer((Player*)unit_target);
                }
                else if (Unit* unit_target2 = spell->m_targets.getUnitTarget())
                {
                    petUnit->SetInFront(unit_target2);
                    if (unit_target2->GetTypeId() == TYPEID_PLAYER)
                        petUnit->SendCreateUpdateToPlayer((Player*)unit_target2);
                }
                if (Unit* powner = petUnit->GetCharmerOrOwner())
                    if (powner->GetTypeId() == TYPEID_PLAYER)
                        petUnit->SendCreateUpdateToPlayer((Player*)powner);
                result = SPELL_CAST_OK;
            }

            if (result == SPELL_CAST_OK)
            {
                if (creature)
                    creature->AddCreatureSpellCooldown(spellid);

                unit_target = spell->m_targets.getUnitTarget();

                charmInfo->SetSpellOpener();
                spell->SpellStart(&(spell->m_targets));
            }
            else
            {
                if (petUnit->hasUnitState(UNIT_STAT_CONTROLLED))
                    Spell::SendCastResult(GetPlayer(), spellInfo, 0, result);
                else
                {
                    Unit* owner = petUnit->GetCharmerOrOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                        Spell::SendCastResult((Player*)owner, spellInfo, 0, result, true);
                }

                if (creature && !creature->HasSpellCooldown(spellid))
                    GetPlayer()->SendClearCooldown(spellid, petUnit);

                charmInfo->SetSpellOpener();
                spell->finish(false);
                delete spell;
            }
            break;
        }
        default:
            sLog.outError("WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
    }
}

void WorldSession::HandlePetStopAttack(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_PET_STOP_ATTACK");

    ObjectGuid petGuid;
    recv_data >> petGuid;

    Unit* pet = _player->GetMap()->GetUnit(petGuid);    // pet or controlled creature/player
    if (!pet)
    {
        sLog.outError("%s doesn't exist.", petGuid.GetString().c_str());
        return;
    }

    if (_player->GetObjectGuid() != pet->GetCharmerOrOwnerGuid())
    {
        sLog.outError("HandlePetStopAttack: %s isn't charm/pet of %s.", petGuid.GetString().c_str(), _player->GetGuidStr().c_str());
        return;
    }

    if (!pet->isAlive())
        return;

    pet->AttackStop();
}

void WorldSession::HandlePetNameQueryOpcode(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetNameQuery. CMSG_PET_NAME_QUERY");

    uint32 petnumber;
    ObjectGuid petguid;

    recv_data >> petnumber;
    recv_data >> petguid;

    SendPetNameQuery(petguid, petnumber);
}

void WorldSession::SendPetNameQuery(ObjectGuid petguid, uint32 petnumber) const
{
    Creature* pet = _player->GetMap()->GetAnyTypeCreature(petguid);
    if (!pet || !pet->GetCharmInfo() || pet->GetCharmInfo()->GetPetNumber() != petnumber)
    {
        WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4 + 1 + 4 + 1));
        data << uint32(petnumber);
        data << uint8(0);
        data << uint32(0);
        data << uint8(0);
        _player->GetSession()->SendPacket(data);
        return;
    }

    char const* name = pet->GetName();

    // creature pets have localization like other creatures
    if (!pet->GetOwnerGuid().IsPlayer())
    {
        int loc_idx = GetSessionDbLocaleIndex();
        sObjectMgr.GetCreatureLocaleStrings(pet->GetEntry(), loc_idx, &name);
    }

    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4 + 4 + strlen(name) + 1));
    data << uint32(petnumber);
    data << name;
    data << uint32(pet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP));

    if (pet->IsPet() && ((Pet*)pet)->GetDeclinedNames())
    {
        data << uint8(1);
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << ((Pet*)pet)->GetDeclinedNames()->name[i];
    }
    else
        data << uint8(0);

    _player->GetSession()->SendPacket(data);
}

void WorldSession::HandlePetSetAction(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetSetAction. CMSG_PET_SET_ACTION");

    ObjectGuid petGuid;
    uint8  count;

    recv_data >> petGuid;

    Unit* petUnit = _player->GetMap()->GetUnit(petGuid);

    if (!petUnit || (petUnit != _player->GetPet() && petUnit != _player->GetCharm()))
    {
        sLog.outError("HandlePetSetAction: Unknown pet or pet owner.");
        return;
    }

    Creature* petCreature = petUnit->GetTypeId() == TYPEID_UNIT ? static_cast<Creature*>(petUnit) : nullptr;
    Pet* pet = (petCreature && petCreature->IsPet()) ? static_cast<Pet*>(petUnit) : nullptr;

    // pet can have action bar disabled
    if (pet && (pet->GetModeFlags() & PET_MODE_DISABLE_ACTIONS))
        return;

    CharmInfo* charmInfo = petUnit->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetSetAction: %s is considered pet-like but doesn't have a charminfo!", petUnit->GetObjectGuid().GetString().c_str());
        return;
    }

    count = (recv_data.size() == 24) ? 2 : 1;

    uint32 position[2];
    uint32 data[2];
    bool move_command = false;

    for (uint8 i = 0; i < count; ++i)
    {
        recv_data >> position[i];
        recv_data >> data[i];

        uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

        // ignore invalid position
        if (position[i] >= MAX_UNIT_ACTION_BAR_INDEX)
            return;

        // in the normal case, command and reaction buttons can only be moved, not removed
        // at moving count ==2, at removing count == 1
        // ignore attempt to remove command|reaction buttons (not possible at normal case)
        if (act_state == ACT_COMMAND || act_state == ACT_REACTION)
        {
            if (count == 1)
                return;

            move_command = true;
        }
    }

    // check swap (at command->spell swap client remove spell first in another packet, so check only command move correctness)
    if (move_command)
    {
        uint8 act_state_0 = UNIT_ACTION_BUTTON_TYPE(data[0]);
        if (act_state_0 == ACT_COMMAND || act_state_0 == ACT_REACTION)
        {
            uint32 spell_id_0 = UNIT_ACTION_BUTTON_ACTION(data[0]);
            UnitActionBarEntry const* actionEntry_1 = charmInfo->GetActionBarEntry(position[1]);
            if (!actionEntry_1 || spell_id_0 != actionEntry_1->GetAction() ||
                    act_state_0 != actionEntry_1->GetType())
                return;
        }

        uint8 act_state_1 = UNIT_ACTION_BUTTON_TYPE(data[1]);
        if (act_state_1 == ACT_COMMAND || act_state_1 == ACT_REACTION)
        {
            uint32 spell_id_1 = UNIT_ACTION_BUTTON_ACTION(data[1]);
            UnitActionBarEntry const* actionEntry_0 = charmInfo->GetActionBarEntry(position[0]);
            if (!actionEntry_0 || spell_id_1 != actionEntry_0->GetAction() ||
                    act_state_1 != actionEntry_0->GetType())
                return;
        }
    }

    for (uint8 i = 0; i < count; ++i)
    {
        uint32 spell_id = UNIT_ACTION_BUTTON_ACTION(data[i]);
        uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

        DETAIL_LOG("Player %s has changed pet spell action. Position: %u, Spell: %u, State: 0x%X", _player->GetName(), position[i], spell_id, uint32(act_state));

        // if it's act for spell (en/disable/cast) and there is a spell given (0 = remove spell) which pet doesn't know, don't add
        if (!((act_state == ACT_ENABLED || act_state == ACT_DISABLED || act_state == ACT_PASSIVE) && spell_id && !petUnit->HasSpell(spell_id)))
        {
            // sign for autocast
            if (act_state == ACT_ENABLED && spell_id)
            {
                if (petUnit->isCharmed())
                    charmInfo->ToggleCreatureAutocast(spell_id, true);
                else if (pet)
                    pet->ToggleAutocast(spell_id, true);
            }
            // sign for no/turn off autocast
            else if (act_state == ACT_DISABLED && spell_id)
            {
                if (petUnit->isCharmed())
                    charmInfo->ToggleCreatureAutocast(spell_id, false);
                else if (pet)
                    pet->ToggleAutocast(spell_id, false);
            }

            charmInfo->SetActionBar(position[i], spell_id, ActiveStates(act_state));
        }
    }
}

void WorldSession::HandlePetRename(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetRename. CMSG_PET_RENAME");

    ObjectGuid petGuid;
    uint8 isdeclined;

    std::string name;
    DeclinedName declinedname;

    recv_data >> petGuid;
    recv_data >> name;
    recv_data >> isdeclined;

    Pet* pet = _player->GetMap()->GetPet(petGuid);
    // check it!
    if (!pet || pet->getPetType() != HUNTER_PET ||
            !pet->HasByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED) ||
            pet->GetOwnerGuid() != _player->GetObjectGuid() || !pet->GetCharmInfo())
        return;

    PetNameInvalidReason res = ObjectMgr::CheckPetName(name);
    if (res != PET_NAME_SUCCESS)
    {
        SendPetNameInvalid(res, name, nullptr);
        return;
    }

    if (sObjectMgr.IsReservedName(name))
    {
        SendPetNameInvalid(PET_NAME_RESERVED, name, nullptr);
        return;
    }

    pet->SetName(name);

    if (_player->GetGroup())
        _player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_NAME);

    pet->RemoveByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);

    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
        {
            recv_data >> declinedname.name[i];
        }

        std::wstring wname;
        Utf8toWStr(name, wname);
        if (!ObjectMgr::CheckDeclinedNames(GetMainPartOfName(wname, 0), declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabase.BeginTransaction();
    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            CharacterDatabase.escape_string(declinedname.name[i]);
        CharacterDatabase.PExecute("DELETE FROM character_pet_declinedname WHERE owner = '%u' AND id = '%u'", _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
        CharacterDatabase.PExecute("INSERT INTO character_pet_declinedname (id, owner, genitive, dative, accusative, instrumental, prepositional) VALUES ('%u','%u','%s','%s','%s','%s','%s')",
                                   pet->GetCharmInfo()->GetPetNumber(), _player->GetGUIDLow(), declinedname.name[0].c_str(), declinedname.name[1].c_str(), declinedname.name[2].c_str(), declinedname.name[3].c_str(), declinedname.name[4].c_str());
    }

    CharacterDatabase.escape_string(name);
    CharacterDatabase.PExecute("UPDATE character_pet SET name = '%s', renamed = '1' WHERE owner = '%u' AND id = '%u'", name.c_str(), _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
    CharacterDatabase.CommitTransaction();

    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(nullptr)));
}

void WorldSession::HandlePetAbandon(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;                                      // pet guid

    DETAIL_LOG("HandlePetAbandon. CMSG_PET_ABANDON pet guid is %s", guid.GetString().c_str());

    if (!_player->IsInWorld())
        return;

    // pet/charmed
    if (Unit* petUnit = _player->GetMap()->GetUnit(guid))
    {
        if (petUnit->GetOwnerGuid() != _player->GetObjectGuid() || !petUnit->GetCharmInfo())
            return;

        Creature* petCreature = petUnit->GetTypeId() == TYPEID_UNIT ? static_cast<Creature*>(petUnit) : nullptr;
        Pet* pet = (petCreature && petCreature->IsPet()) ? static_cast<Pet*>(petUnit) : nullptr;

        if (pet)
        {
            // Permanently abandon pet
            if (pet->getPetType() == HUNTER_PET)
                pet->Unsummon(PET_SAVE_AS_DELETED, _player);
            // Simply dismiss
            else
                petUnit->SetDeathState(CORPSE);
        }
        else if (petUnit->GetObjectGuid() == _player->GetCharmGuid())
        {
            _player->Uncharm();
        }
    }
}

void WorldSession::HandlePetUnlearnOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("CMSG_PET_UNLEARN");

    ObjectGuid guid;
    recvPacket >> guid;                 // Pet guid

    Pet* pet = _player->GetPet();

    if (!pet || guid != pet->GetObjectGuid())
    {
        sLog.outError("HandlePetUnlearnOpcode. %s isn't pet of %s .", guid.GetString().c_str(), _player->GetGuidStr().c_str());
        return;
    }

    if (pet->getPetType() != HUNTER_PET || pet->m_usedTalentCount == 0)
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetUnlearnOpcode: %s is considered pet-like but doesn't have a charminfo!", pet->GetGuidStr().c_str());
        return;
    }
    pet->resetTalents();
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("CMSG_PET_SPELL_AUTOCAST");

    ObjectGuid guid;
    uint32 spellid;
    uint8  state;                                           // 1 for on, 0 for off
    recvPacket >> guid >> spellid >> state;

    Unit* petUnit = _player->GetMap()->GetUnit(guid);
    if (!petUnit || (guid != _player->GetPetGuid() && guid != _player->GetCharmGuid()))
    {
        sLog.outError("HandlePetSpellAutocastOpcode. %s isn't pet of %s .", guid.GetString().c_str(), _player->GetGuidStr().c_str());
        return;
    }

    Creature* petCreature = petUnit->GetTypeId() == TYPEID_UNIT ? static_cast<Creature*>(petUnit) : nullptr;
    Pet* pet = (petCreature && petCreature->IsPet()) ? static_cast<Pet*>(petUnit) : nullptr;

    // do not add not learned spells/ passive spells
    if (!petUnit->HasSpell(spellid) || !IsAutocastable(spellid))
        return;

    CharmInfo* charmInfo = petUnit->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetSpellAutocastOpcod: %s is considered pet-like but doesn't have a charminfo!", guid.GetString().c_str());
        return;
    }

    if (petUnit->isCharmed())
        // state can be used as boolean
        petUnit->GetCharmInfo()->ToggleCreatureAutocast(spellid, !!state);
    else if (pet)
        pet->ToggleAutocast(spellid, !!state);

    charmInfo->SetSpellAutocast(spellid, !!state);
}

void WorldSession::HandlePetCastSpellOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("WORLD: CMSG_PET_CAST_SPELL");

    ObjectGuid guid;
    uint32 spellid;
    uint8  cast_count;
    uint8  unk_flags;                                       // flags (if 0x02 - some additional data are received)

    recvPacket >> guid >> cast_count >> spellid >> unk_flags;

    DEBUG_LOG("WORLD: CMSG_PET_CAST_SPELL, %s, cast_count: %u, spellid %u, unk_flags %u", guid.GetString().c_str(), cast_count, spellid, unk_flags);

    Unit* petUnit = _player->GetMap()->GetUnit(guid);
    if (!petUnit || (guid != _player->GetPetGuid() && guid != _player->GetCharmGuid()))
    {
        sLog.outError("HandlePetSpellAutocastOpcode. %s isn't pet of %s .", guid.GetString().c_str(), _player->GetGuidStr().c_str());
        return;
    }

    Creature* petCreature = petUnit->GetTypeId() == TYPEID_UNIT ? static_cast<Creature*>(petUnit) : nullptr;
    Pet* pet = (petCreature && petCreature->IsPet()) ? static_cast<Pet*>(petUnit) : nullptr;

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellid);
    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown PET spell id %i", spellid);
        return;
    }

    if (petUnit->GetCharmInfo() && petUnit->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
        return;

    Aura* triggeredByAura = petUnit->GetTriggeredByClientAura(spellid);

    // do not cast not learned spells
    if ((!triggeredByAura && !petUnit->HasSpell(spellid)) || IsPassiveSpell(spellInfo))
        return;

    SpellCastTargets targets;

    recvPacket >> targets.ReadForCaster(petUnit);

    petUnit->clearUnitState(UNIT_STAT_MOVING);

    Spell* spell = new Spell(petUnit, spellInfo, triggeredByAura ? true : false, petUnit->GetObjectGuid(), triggeredByAura ? triggeredByAura->GetSpellProto() : nullptr);
    spell->m_cast_count = cast_count;                       // probably pending spell cast
    spell->m_targets = targets;

    SpellCastResult result = triggeredByAura ? SPELL_CAST_OK : spell->CheckPetCast(nullptr);
    if (result == SPELL_CAST_OK)
    {
        if (pet)
            pet->AddCreatureSpellCooldown(spellid);
        spell->SpellStart(&(spell->m_targets), triggeredByAura);
    }
    else
    {
        Unit* owner = petUnit->GetCharmerOrOwner();
        if (owner && owner->GetTypeId() == TYPEID_PLAYER && !triggeredByAura)
            Spell::SendCastResult((Player*)owner, spellInfo, 0, result, true);

        if (pet && !pet->HasSpellCooldown(spellid) && !triggeredByAura)
            GetPlayer()->SendClearCooldown(spellid, pet);

        spell->finish(false);
        delete spell;
    }
}

void WorldSession::SendPetNameInvalid(uint32 error, const std::string& name, DeclinedName* declinedName) const
{
    WorldPacket data(SMSG_PET_NAME_INVALID, 4 + name.size() + 1 + 1);
    data << uint32(error);
    data << name;
    if (declinedName)
    {
        data << uint8(1);
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << declinedName->name[i];
    }
    else
        data << uint8(0);
    SendPacket(data);
}

void WorldSession::HandlePetLearnTalent(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_PET_LEARN_TALENT");

    ObjectGuid guid;
    uint32 talent_id, requested_rank;
    recv_data >> guid >> talent_id >> requested_rank;

    _player->LearnPetTalent(guid, talent_id, requested_rank);
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandleLearnPreviewTalentsPet(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LEARN_PREVIEW_TALENTS_PET");

    ObjectGuid guid;
    recv_data >> guid;

    uint32 talentsCount;
    recv_data >> talentsCount;

    uint32 talentId, talentRank;

    for (uint32 i = 0; i < talentsCount; ++i)
    {
        recv_data >> talentId >> talentRank;

        _player->LearnPetTalent(guid, talentId, talentRank);
    }

    _player->SendTalentsInfoData(true);
}
