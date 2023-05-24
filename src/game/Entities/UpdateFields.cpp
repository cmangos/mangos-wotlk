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

#include "UpdateFields.h"
#include "Log.h"
#include "ObjectGuid.h"
#include <array>
#include <vector>

// Auto generated file
// Patch: 3.3.5
// Build: 12340

static std::array<UpdateFieldData, 381> const g_updateFieldsData =
{{
    // enum EObjectFields
    { TYPEMASK_OBJECT       , "OBJECT_FIELD_GUID"                               , 0x0  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_OBJECT       , "OBJECT_FIELD_TYPE"                               , 0x2  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_OBJECT       , "OBJECT_FIELD_ENTRY"                              , 0x3  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_OBJECT       , "OBJECT_FIELD_SCALE_X"                            , 0x4  , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_OBJECT       , "OBJECT_FIELD_PADDING"                            , 0x5  , 1  , UF_TYPE_INT      , UF_FLAG_NONE },
    { TYPEMASK_OBJECT       , "OBJECT_END"                                      , 0x6  , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EContainerFields
    { TYPEMASK_CONTAINER    , "CONTAINER_FIELD_NUM_SLOTS"                       , 0x40 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_CONTAINER    , "CONTAINER_ALIGN_PAD"                             , 0x41 , 1  , UF_TYPE_BYTES    , UF_FLAG_NONE },
    { TYPEMASK_CONTAINER    , "CONTAINER_FIELD_SLOT_1"                          , 0x42 , 72 , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_CONTAINER    , "CONTAINER_END"                                   , 0x8A , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EItemFields
    { TYPEMASK_ITEM         , "ITEM_FIELD_OWNER"                                , 0x6  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_CONTAINED"                            , 0x8  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_CREATOR"                              , 0xA  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_GIFTCREATOR"                          , 0xC  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_STACK_COUNT"                          , 0xE  , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY + UF_FLAG_UNK2 },
    { TYPEMASK_ITEM         , "ITEM_FIELD_DURATION"                             , 0xF  , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY + UF_FLAG_UNK2 },
    { TYPEMASK_ITEM         , "ITEM_FIELD_SPELL_CHARGES"                        , 0x10 , 5  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY + UF_FLAG_UNK2 },
    { TYPEMASK_ITEM         , "ITEM_FIELD_FLAGS"                                , 0x15 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_1_1"                      , 0x16 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_1_3"                      , 0x18 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_2_1"                      , 0x19 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_2_3"                      , 0x1B , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_3_1"                      , 0x1C , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_3_3"                      , 0x1E , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_4_1"                      , 0x1F , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_4_3"                      , 0x21 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_5_1"                      , 0x22 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_5_3"                      , 0x24 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_6_1"                      , 0x25 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_6_3"                      , 0x27 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_7_1"                      , 0x28 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_7_3"                      , 0x2A , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_8_1"                      , 0x2B , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_8_3"                      , 0x2D , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_9_1"                      , 0x2E , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_9_3"                      , 0x30 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_10_1"                     , 0x31 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_10_3"                     , 0x33 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_11_1"                     , 0x34 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_11_3"                     , 0x36 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_12_1"                     , 0x37 , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_ENCHANTMENT_12_3"                     , 0x39 , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_PROPERTY_SEED"                        , 0x3A , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_RANDOM_PROPERTIES_ID"                 , 0x3B , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_DURABILITY"                           , 0x3C , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY + UF_FLAG_UNK2 },
    { TYPEMASK_ITEM         , "ITEM_FIELD_MAXDURABILITY"                        , 0x3D , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY + UF_FLAG_UNK2 },
    { TYPEMASK_ITEM         , "ITEM_FIELD_CREATE_PLAYED_TIME"                   , 0x3E , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_ITEM         , "ITEM_FIELD_PAD"                                  , 0x3F , 1  , UF_TYPE_INT      , UF_FLAG_NONE },
    { TYPEMASK_ITEM         , "ITEM_END"                                        , 0x40 , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EUnitFields
    { TYPEMASK_UNIT         , "UNIT_FIELD_CHARM"                                , 0x6  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_SUMMON"                               , 0x8  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_CRITTER"                              , 0xA  , 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_UNIT         , "UNIT_FIELD_CHARMEDBY"                            , 0xC  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_SUMMONEDBY"                           , 0xE  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_CREATEDBY"                            , 0x10 , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_TARGET"                               , 0x12 , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_CHANNEL_OBJECT"                       , 0x14 , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_CHANNEL_SPELL"                              , 0x16 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BYTES_0"                              , 0x17 , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_HEALTH"                               , 0x18 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER1"                               , 0x19 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER2"                               , 0x1A , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER3"                               , 0x1B , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER4"                               , 0x1C , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER5"                               , 0x1D , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER6"                               , 0x1E , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER7"                               , 0x1F , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXHEALTH"                            , 0x20 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER1"                            , 0x21 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER2"                            , 0x22 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER3"                            , 0x23 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER4"                            , 0x24 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER5"                            , 0x25 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER6"                            , 0x26 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXPOWER7"                            , 0x27 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER"            , 0x28 , 7  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER", 0x2F , 7  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_LEVEL"                                , 0x36 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_FACTIONTEMPLATE"                      , 0x37 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_VIRTUAL_ITEM_SLOT_ID"                       , 0x38 , 3  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_FLAGS"                                , 0x3B , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_FLAGS_2"                              , 0x3C , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_AURASTATE"                            , 0x3D , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BASEATTACKTIME"                       , 0x3E , 2  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RANGEDATTACKTIME"                     , 0x40 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BOUNDINGRADIUS"                       , 0x41 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_COMBATREACH"                          , 0x42 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_DISPLAYID"                            , 0x43 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NATIVEDISPLAYID"                      , 0x44 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MOUNTDISPLAYID"                       , 0x45 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MINDAMAGE"                            , 0x46 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY + UF_FLAG_SPECIAL_INFO },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXDAMAGE"                            , 0x47 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY + UF_FLAG_SPECIAL_INFO },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MINOFFHANDDAMAGE"                     , 0x48 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY + UF_FLAG_SPECIAL_INFO },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXOFFHANDDAMAGE"                     , 0x49 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY + UF_FLAG_SPECIAL_INFO },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BYTES_1"                              , 0x4A , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_PETNUMBER"                            , 0x4B , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_PET_NAME_TIMESTAMP"                   , 0x4C , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_PETEXPERIENCE"                        , 0x4D , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_PETNEXTLEVELEXP"                      , 0x4E , 1  , UF_TYPE_INT      , UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_DYNAMIC_FLAGS"                              , 0x4F , 1  , UF_TYPE_INT      , UF_FLAG_DYNAMIC },
    { TYPEMASK_UNIT         , "UNIT_MOD_CAST_SPEED"                             , 0x50 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_CREATED_BY_SPELL"                           , 0x51 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_NPC_FLAGS"                                  , 0x52 , 1  , UF_TYPE_INT      , UF_FLAG_DYNAMIC },
    { TYPEMASK_UNIT         , "UNIT_NPC_EMOTESTATE"                             , 0x53 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_STAT0"                                , 0x54 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_STAT1"                                , 0x55 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_STAT2"                                , 0x56 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_STAT3"                                , 0x57 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_STAT4"                                , 0x58 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POSSTAT0"                             , 0x59 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POSSTAT1"                             , 0x5A , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POSSTAT2"                             , 0x5B , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POSSTAT3"                             , 0x5C , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POSSTAT4"                             , 0x5D , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NEGSTAT0"                             , 0x5E , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NEGSTAT1"                             , 0x5F , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NEGSTAT2"                             , 0x60 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NEGSTAT3"                             , 0x61 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_NEGSTAT4"                             , 0x62 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RESISTANCES"                          , 0x63 , 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY + UF_FLAG_SPECIAL_INFO },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE"           , 0x6A , 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE"           , 0x71 , 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BASE_MANA"                            , 0x78 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BASE_HEALTH"                          , 0x79 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_BYTES_2"                              , 0x7A , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_ATTACK_POWER"                         , 0x7B , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_ATTACK_POWER_MODS"                    , 0x7C , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_ATTACK_POWER_MULTIPLIER"              , 0x7D , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RANGED_ATTACK_POWER"                  , 0x7E , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RANGED_ATTACK_POWER_MODS"             , 0x7F , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER"       , 0x80 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MINRANGEDDAMAGE"                      , 0x81 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXRANGEDDAMAGE"                      , 0x82 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER_COST_MODIFIER"                  , 0x83 , 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_POWER_COST_MULTIPLIER"                , 0x8A , 7  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_MAXHEALTHMODIFIER"                    , 0x91 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE + UF_FLAG_OWNER_ONLY },
    { TYPEMASK_UNIT         , "UNIT_FIELD_HOVERHEIGHT"                          , 0x92 , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_UNIT         , "UNIT_FIELD_PADDING"                              , 0x93 , 1  , UF_TYPE_INT      , UF_FLAG_NONE },
    { TYPEMASK_UNIT         , "UNIT_END"                                        , 0x94 , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EPlayerFields
    { TYPEMASK_PLAYER       , "PLAYER_DUEL_ARBITER"                             , 0x94 , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_FLAGS"                                    , 0x96 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_GUILDID"                                  , 0x97 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_GUILDRANK"                                , 0x98 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_BYTES"                                    , 0x99 , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_BYTES_2"                                  , 0x9A , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_BYTES_3"                                  , 0x9B , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_DUEL_TEAM"                                , 0x9C , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_GUILD_TIMESTAMP"                          , 0x9D , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_1_1"                            , 0x9E , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_1_2"                            , 0x9F , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_1_3"                            , 0xA0 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_1_4"                            , 0xA2 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_2_1"                            , 0xA3 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_2_2"                            , 0xA4 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_2_3"                            , 0xA5 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_2_5"                            , 0xA7 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_3_1"                            , 0xA8 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_3_2"                            , 0xA9 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_3_3"                            , 0xAA , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_3_5"                            , 0xAC , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_4_1"                            , 0xAD , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_4_2"                            , 0xAE , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_4_3"                            , 0xAF , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_4_5"                            , 0xB1 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_5_1"                            , 0xB2 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_5_2"                            , 0xB3 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_5_3"                            , 0xB4 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_5_5"                            , 0xB6 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_6_1"                            , 0xB7 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_6_2"                            , 0xB8 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_6_3"                            , 0xB9 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_6_5"                            , 0xBB , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_7_1"                            , 0xBC , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_7_2"                            , 0xBD , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_7_3"                            , 0xBE , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_7_5"                            , 0xC0 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_8_1"                            , 0xC1 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_8_2"                            , 0xC2 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_8_3"                            , 0xC3 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_8_5"                            , 0xC5 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_9_1"                            , 0xC6 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_9_2"                            , 0xC7 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_9_3"                            , 0xC8 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_9_5"                            , 0xCA , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_10_1"                           , 0xCB , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_10_2"                           , 0xCC , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_10_3"                           , 0xCD , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_10_5"                           , 0xCF , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_11_1"                           , 0xD0 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_11_2"                           , 0xD1 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_11_3"                           , 0xD2 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_11_5"                           , 0xD4 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_12_1"                           , 0xD5 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_12_2"                           , 0xD6 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_12_3"                           , 0xD7 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_12_5"                           , 0xD9 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_13_1"                           , 0xDA , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_13_2"                           , 0xDB , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_13_3"                           , 0xDC , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_13_5"                           , 0xDE , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_14_1"                           , 0xDF , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_14_2"                           , 0xE0 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_14_3"                           , 0xE1 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_14_5"                           , 0xE3 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_15_1"                           , 0xE4 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_15_2"                           , 0xE5 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_15_3"                           , 0xE6 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_15_5"                           , 0xE8 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_16_1"                           , 0xE9 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_16_2"                           , 0xEA , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_16_3"                           , 0xEB , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_16_5"                           , 0xED , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_17_1"                           , 0xEE , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_17_2"                           , 0xEF , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_17_3"                           , 0xF0 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_17_5"                           , 0xF2 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_18_1"                           , 0xF3 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_18_2"                           , 0xF4 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_18_3"                           , 0xF5 , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_18_5"                           , 0xF7 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_19_1"                           , 0xF8 , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_19_2"                           , 0xF9 , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_19_3"                           , 0xFA , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_19_5"                           , 0xFC , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_20_1"                           , 0xFD , 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_20_2"                           , 0xFE , 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_20_3"                           , 0xFF , 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_20_5"                           , 0x101, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_21_1"                           , 0x102, 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_21_2"                           , 0x103, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_21_3"                           , 0x104, 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_21_5"                           , 0x106, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_22_1"                           , 0x107, 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_22_2"                           , 0x108, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_22_3"                           , 0x109, 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_22_5"                           , 0x10B, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_23_1"                           , 0x10C, 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_23_2"                           , 0x10D, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_23_3"                           , 0x10E, 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_23_5"                           , 0x110, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_24_1"                           , 0x111, 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_24_2"                           , 0x112, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_24_3"                           , 0x113, 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_24_5"                           , 0x115, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_25_1"                           , 0x116, 1  , UF_TYPE_INT      , UF_FLAG_GROUP_ONLY },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_25_2"                           , 0x117, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_25_3"                           , 0x118, 2  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_QUEST_LOG_25_5"                           , 0x11A, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_1_ENTRYID"                   , 0x11B, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_1_ENCHANTMENT"               , 0x11C, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_2_ENTRYID"                   , 0x11D, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_2_ENCHANTMENT"               , 0x11E, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_3_ENTRYID"                   , 0x11F, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_3_ENCHANTMENT"               , 0x120, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_4_ENTRYID"                   , 0x121, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_4_ENCHANTMENT"               , 0x122, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_5_ENTRYID"                   , 0x123, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_5_ENCHANTMENT"               , 0x124, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_6_ENTRYID"                   , 0x125, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_6_ENCHANTMENT"               , 0x126, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_7_ENTRYID"                   , 0x127, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_7_ENCHANTMENT"               , 0x128, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_8_ENTRYID"                   , 0x129, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_8_ENCHANTMENT"               , 0x12A, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_9_ENTRYID"                   , 0x12B, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_9_ENCHANTMENT"               , 0x12C, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_10_ENTRYID"                  , 0x12D, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_10_ENCHANTMENT"              , 0x12E, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_11_ENTRYID"                  , 0x12F, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_11_ENCHANTMENT"              , 0x130, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_12_ENTRYID"                  , 0x131, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_12_ENCHANTMENT"              , 0x132, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_13_ENTRYID"                  , 0x133, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_13_ENCHANTMENT"              , 0x134, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_14_ENTRYID"                  , 0x135, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_14_ENCHANTMENT"              , 0x136, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_15_ENTRYID"                  , 0x137, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_15_ENCHANTMENT"              , 0x138, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_16_ENTRYID"                  , 0x139, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_16_ENCHANTMENT"              , 0x13A, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_17_ENTRYID"                  , 0x13B, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_17_ENCHANTMENT"              , 0x13C, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_18_ENTRYID"                  , 0x13D, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_18_ENCHANTMENT"              , 0x13E, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_19_ENTRYID"                  , 0x13F, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_VISIBLE_ITEM_19_ENCHANTMENT"              , 0x140, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_CHOSEN_TITLE"                             , 0x141, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_FAKE_INEBRIATION"                         , 0x142, 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_PAD_0"                              , 0x143, 1  , UF_TYPE_INT      , UF_FLAG_NONE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_INV_SLOT_HEAD"                      , 0x144, 46 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_PACK_SLOT_1"                        , 0x172, 32 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BANK_SLOT_1"                        , 0x192, 56 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BANKBAG_SLOT_1"                     , 0x1CA, 14 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_VENDORBUYBACK_SLOT_1"               , 0x1D8, 24 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_KEYRING_SLOT_1"                     , 0x1F0, 64 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_CURRENCYTOKEN_SLOT_1"               , 0x230, 64 , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FARSIGHT"                                 , 0x270, 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER__FIELD_KNOWN_TITLES"                      , 0x272, 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER__FIELD_KNOWN_TITLES1"                     , 0x274, 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER__FIELD_KNOWN_TITLES2"                     , 0x276, 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_KNOWN_CURRENCIES"                   , 0x278, 2  , UF_TYPE_GUID     , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_XP"                                       , 0x27A, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_NEXT_LEVEL_XP"                            , 0x27B, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_SKILL_INFO_1_1"                           , 0x27C, 384, UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_CHARACTER_POINTS1"                        , 0x3FC, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_CHARACTER_POINTS2"                        , 0x3FD, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_TRACK_CREATURES"                          , 0x3FE, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_TRACK_RESOURCES"                          , 0x3FF, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_BLOCK_PERCENTAGE"                         , 0x400, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_DODGE_PERCENTAGE"                         , 0x401, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_PARRY_PERCENTAGE"                         , 0x402, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_EXPERTISE"                                , 0x403, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_OFFHAND_EXPERTISE"                        , 0x404, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_CRIT_PERCENTAGE"                          , 0x405, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_RANGED_CRIT_PERCENTAGE"                   , 0x406, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_OFFHAND_CRIT_PERCENTAGE"                  , 0x407, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_SPELL_CRIT_PERCENTAGE1"                   , 0x408, 7  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_SHIELD_BLOCK"                             , 0x40F, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE"             , 0x410, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_EXPLORED_ZONES_1"                         , 0x411, 128, UF_TYPE_BYTES    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_REST_STATE_EXPERIENCE"                    , 0x491, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_COINAGE"                            , 0x492, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_DAMAGE_DONE_POS"                , 0x493, 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_DAMAGE_DONE_NEG"                , 0x49A, 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_DAMAGE_DONE_PCT"                , 0x4A1, 7  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_HEALING_DONE_POS"               , 0x4A8, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_HEALING_PCT"                    , 0x4A9, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_HEALING_DONE_PCT"               , 0x4AA, 1  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_TARGET_RESISTANCE"              , 0x4AB, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE"     , 0x4AC, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BYTES"                              , 0x4AD, 1  , UF_TYPE_BYTES    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_AMMO_ID"                                  , 0x4AE, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_SELF_RES_SPELL"                           , 0x4AF, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_PVP_MEDALS"                         , 0x4B0, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BUYBACK_PRICE_1"                    , 0x4B1, 12 , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BUYBACK_TIMESTAMP_1"                , 0x4BD, 12 , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_KILLS"                              , 0x4C9, 1  , UF_TYPE_TWO_SHORT, UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_TODAY_CONTRIBUTION"                 , 0x4CA, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_YESTERDAY_CONTRIBUTION"             , 0x4CB, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_LIFETIME_HONORABLE_KILLS"           , 0x4CC, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_BYTES2"                             , 0x4CD, 1  , UF_TYPE_BYTES2   , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_WATCHED_FACTION_INDEX"              , 0x4CE, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_COMBAT_RATING_1"                    , 0x4CF, 25 , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_ARENA_TEAM_INFO_1_1"                , 0x4E8, 21 , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_HONOR_CURRENCY"                     , 0x4FD, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_ARENA_CURRENCY"                     , 0x4FE, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_MAX_LEVEL"                          , 0x4FF, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_DAILY_QUESTS_1"                     , 0x500, 25 , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_RUNE_REGEN_1"                             , 0x519, 4  , UF_TYPE_FLOAT    , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_NO_REAGENT_COST_1"                        , 0x51D, 3  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_GLYPH_SLOTS_1"                      , 0x520, 6  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_FIELD_GLYPHS_1"                           , 0x526, 6  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_GLYPHS_ENABLED"                           , 0x52C, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_PET_SPELL_POWER"                          , 0x52D, 1  , UF_TYPE_INT      , UF_FLAG_PRIVATE },
    { TYPEMASK_PLAYER       , "PLAYER_END"                                      , 0x52E, 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EGameObjectFields
    { TYPEMASK_GAMEOBJECT   , "OBJECT_FIELD_CREATED_BY"                         , 0x6  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_DISPLAYID"                            , 0x8  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_FLAGS"                                , 0x9  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_PARENTROTATION"                       , 0xA  , 4  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_DYNAMIC"                              , 0xE  , 1  , UF_TYPE_TWO_SHORT, UF_FLAG_DYNAMIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_FACTION"                              , 0xF  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_LEVEL"                                , 0x10 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_BYTES_1"                              , 0x11 , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_GAMEOBJECT   , "GAMEOBJECT_END"                                  , 0x12 , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum EDynamicObjectFields
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_CASTER"                            , 0x6  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_BYTES"                             , 0x8  , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_SPELLID"                           , 0x9  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_RADIUS"                            , 0xA  , 1  , UF_TYPE_FLOAT    , UF_FLAG_PUBLIC },
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_CASTTIME"                          , 0xB  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_DYNAMICOBJECT, "DYNAMICOBJECT_END"                               , 0xC  , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
    // enum ECorpseFields
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_OWNER"                              , 0x6  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_PARTY"                              , 0x8  , 2  , UF_TYPE_GUID     , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_DISPLAY_ID"                         , 0xA  , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_ITEM"                               , 0xB  , 19 , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_BYTES_1"                            , 0x1E , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_BYTES_2"                            , 0x1F , 1  , UF_TYPE_BYTES    , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_GUILD"                              , 0x20 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_FLAGS"                              , 0x21 , 1  , UF_TYPE_INT      , UF_FLAG_PUBLIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_DYNAMIC_FLAGS"                      , 0x22 , 1  , UF_TYPE_INT      , UF_FLAG_DYNAMIC },
    { TYPEMASK_CORPSE       , "CORPSE_FIELD_PAD"                                , 0x23 , 1  , UF_TYPE_INT      , UF_FLAG_NONE },
    { TYPEMASK_CORPSE       , "CORPSE_END"                                      , 0x24 , 0  , UF_TYPE_NONE     , UF_FLAG_NONE },
}};

template<std::size_t SIZE>
static std::array<uint16, SIZE> SetupUpdateFieldFlagsArray(uint8 objectTypeMask)
{
    std::array<uint16, SIZE> flagsArray;
    for (auto const& itr : g_updateFieldsData)
    {
        if ((itr.objectTypeMask & objectTypeMask) == 0)
            continue;

        for (uint16 i = itr.offset; i < itr.offset + itr.size; i++)
        {
            flagsArray.at(i) = itr.flags;
        }
    }
    return flagsArray;
}

static std::array<uint16, CONTAINER_END> const g_containerUpdateFieldFlags = SetupUpdateFieldFlagsArray<CONTAINER_END>(TYPEMASK_OBJECT | TYPEMASK_ITEM | TYPEMASK_CONTAINER);
static std::array<uint16, PLAYER_END> const g_playerUpdateFieldFlags = SetupUpdateFieldFlagsArray<PLAYER_END>(TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER);
static std::array<uint16, GAMEOBJECT_END> const g_gameObjectUpdateFieldFlags = SetupUpdateFieldFlagsArray<GAMEOBJECT_END>(TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT);
static std::array<uint16, DYNAMICOBJECT_END> const g_dynamicObjectUpdateFieldFlags = SetupUpdateFieldFlagsArray<DYNAMICOBJECT_END>(TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT);
static std::array<uint16, CORPSE_END> const g_corpseUpdateFieldFlags = SetupUpdateFieldFlagsArray<CORPSE_END>(TYPEMASK_OBJECT | TYPEMASK_CORPSE);

uint16 const* UpdateFields::GetUpdateFieldFlagsArray(uint8 objectTypeId)
{
    switch (objectTypeId)
    {
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
        {
            return g_containerUpdateFieldFlags.data();
        }
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
        {
            return g_playerUpdateFieldFlags.data();
        }
        case TYPEID_GAMEOBJECT:
        {
            return g_gameObjectUpdateFieldFlags.data();
        }
        case TYPEID_DYNAMICOBJECT:
        {
            return g_dynamicObjectUpdateFieldFlags.data();
        }
        case TYPEID_CORPSE:
        {
            return g_corpseUpdateFieldFlags.data();
        }
    }
    sLog.outError("Unhandled object type id (%hhu) in GetUpdateFieldFlagsArray!", objectTypeId);
    return 0;
}

UpdateFieldData const* UpdateFields::GetUpdateFieldDataByName(char const* name)
{
    for (const auto& itr : g_updateFieldsData)
    {
        if (strcmp(itr.name, name) == 0)
            return &itr;
    }
    return nullptr;
}

UpdateFieldData const* UpdateFields::GetUpdateFieldDataByTypeMaskAndOffset(uint8 objectTypeMask, uint16 offset)
{
    for (auto const& itr : g_updateFieldsData)
    {
        if ((itr.objectTypeMask & objectTypeMask) == 0)
            continue;

        if (offset == itr.offset || (offset > itr.offset && offset < (itr.offset + itr.size)))
            return &itr;
    }
    return nullptr;
}
