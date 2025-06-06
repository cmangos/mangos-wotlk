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

#ifndef MANGOSSERVER_GAMEOBJECT_H
#define MANGOSSERVER_GAMEOBJECT_H

#include "Common.h"
#include "Globals/SharedDefines.h"
#include "Entities/Object.h"
#include "Util/Util.h"
#include "AI/BaseAI/GameObjectAI.h"
#include "Spells/SpellDefines.h"
#include "Entities/GameObjectDefines.h"

#include <array>

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

// from `gameobject_template`
struct GameObjectInfo
{
    uint32  id;
    uint32  type;
    uint32  displayId;
    char*   name;
    char*   IconName;
    char*   castBarCaption;
    char*   unk1;
    uint32  faction;
    uint32  flags;
    uint32  ExtraFlags;
    float   size;
    uint32  questItems[6];
    union                                                   // different GO types have different data field
    {
        //0 GAMEOBJECT_TYPE_DOOR
        struct
        {
            uint32 startOpen;                               //0 used client side to determine GO_ACTIVATED means open/closed
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 noDamageImmune;                          //3 break opening whenever you recieve damage?
            uint32 openTextID;                              //4 can be used to replace castBarCaption?
            uint32 closeTextID;                             //5
            uint32 ignoredByPathing;                        //6
        } door;
        //1 GAMEOBJECT_TYPE_BUTTON
        struct
        {
            uint32 startOpen;                               //0
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 linkedTrapId;                            //3
            uint32 noDamageImmune;                          //4 isBattlegroundObject
            uint32 large;                                   //5
            uint32 openTextID;                              //6 can be used to replace castBarCaption?
            uint32 closeTextID;                             //7
            uint32 losOK;                                   //8
        } button;
        //2 GAMEOBJECT_TYPE_QUESTGIVER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 questList;                               //1
            uint32 pageMaterial;                            //2
            uint32 gossipID;                                //3
            uint32 customAnim;                              //4
            uint32 noDamageImmune;                          //5
            int32 openTextID;                               //6 can be used to replace castBarCaption?
            uint32 losOK;                                   //7
            uint32 allowMounted;                            //8
            uint32 large;                                   //9
        } questgiver;
        //3 GAMEOBJECT_TYPE_CHEST
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 lootId;                                  //1
            uint32 chestRestockTime;                        //2
            uint32 consumable;                              //3
            uint32 minSuccessOpens;                         //4
            uint32 maxSuccessOpens;                         //5
            int32 eventId;                                  //6 lootedEvent
            uint32 linkedTrapId;                            //7
            uint32 questId;                                 //8 not used currently but store quest required for GO activation for player
            uint32 level;                                   //9
            uint32 losOK;                                   //10
            uint32 leaveLoot;                               //11
            uint32 notInCombat;                             //12
            uint32 logLoot;                                 //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 groupLootRules;                          //15
            uint32 floatingTooltip;                         //16
        } chest;
        //4 GAMEOBJECT_TYPE_BINDER - empty
        //5 GAMEOBJECT_TYPE_GENERIC
        struct
        {
            uint32 floatingTooltip;                         //0
            uint32 highlight;                               //1
            uint32 serverOnly;                              //2
            uint32 large;                                   //3
            uint32 floatOnWater;                            //4
            uint32 questID;                                 //5
        } _generic;
        //6 GAMEOBJECT_TYPE_TRAP
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 level;                                   //1
            uint32 diameter;                                //2 radius for trap activation
            uint32 spellId;                                 //3
            uint32 charges;                                 //4 need respawn (if > 0)
            uint32 cooldown;                                //5 time in secs
            int32 autoCloseTime;                            //6 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 startDelay;                              //7
            uint32 serverOnly;                              //8
            uint32 stealthed;                               //9
            uint32 large;                                   //10
            uint32 invisible;                               //11
            uint32 openTextID;                              //12 can be used to replace castBarCaption?
            uint32 closeTextID;                             //13
            uint32 ignoreTotems;                            //14
        } trap;
        //7 GAMEOBJECT_TYPE_CHAIR
        struct
        {
            uint32 slots;                                   //0
            uint32 height;                                  //1
            uint32 onlyCreatorUse;                          //2
            uint32 triggeredEvent;                          //3
        } chair;
        //8 GAMEOBJECT_TYPE_SPELL_FOCUS
        struct
        {
            uint32 focusId;                                 //0
            uint32 dist;                                    //1
            uint32 linkedTrapId;                            //2
            uint32 serverOnly;                              //3
            uint32 questID;                                 //4
            uint32 large;                                   //5
            int32 floatingTooltip;                          //6
        } spellFocus;
        //9 GAMEOBJECT_TYPE_TEXT
        struct
        {
            uint32 pageID;                                  //0
            uint32 language;                                //1
            uint32 pageMaterial;                            //2
            uint32 allowMounted;                            //3
        } text;
        //10 GAMEOBJECT_TYPE_GOOBER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            int32 questId;                                  //1
            uint32 eventId;                                 //2
            uint32 autoCloseTime;                           //3 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 customAnim;                              //4
            uint32 consumable;                              //5
            int32 cooldown;                                 //6
            uint32 pageId;                                  //7
            uint32 language;                                //8
            uint32 pageMaterial;                            //9
            uint32 spellId;                                 //10
            uint32 noDamageImmune;                          //11
            uint32 linkedTrapId;                            //12
            uint32 large;                                   //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 closeTextID;                             //15
            uint32 isPvPObject;                             //16 flags used only in battlegrounds
            uint32 allowMounted;                            //17
            uint32 floatingTooltip;                         //18
            uint32 gossipID;                                //19
            uint32 WorldStateSetsState;                     //20
        } goober;
        //11 GAMEOBJECT_TYPE_TRANSPORT
        struct
        {
            uint32 pause;                                   //0
            uint32 startOpen;                               //1
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 pause1EventID;                           //3
            uint32 pause2EventID;                           //4
        } transport;
        //12 GAMEOBJECT_TYPE_AREADAMAGE
        struct
        {
            uint32 lockId;                                  //0
            uint32 radius;                                  //1
            uint32 damageMin;                               //2
            uint32 damageMax;                               //3
            uint32 damageSchool;                            //4
            uint32 autoCloseTime;                           //5 secs till autoclose = autoCloseTime / IN_MILLISECONDS (previous was 0x10000)
            uint32 openTextID;                              //6
            uint32 closeTextID;                             //7
        } areadamage;
        //13 GAMEOBJECT_TYPE_CAMERA
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 cinematicId;                             //1
            uint32 eventID;                                 //2
            uint32 openTextID;                              //3 can be used to replace castBarCaption?
        } camera;
        //14 GAMEOBJECT_TYPE_MAPOBJECT - empty
        //15 GAMEOBJECT_TYPE_MO_TRANSPORT
        struct
        {
            uint32 taxiPathId;                              //0
            uint32 moveSpeed;                               //1
            uint32 accelRate;                               //2
            uint32 startEventID;                            //3
            uint32 stopEventID;                             //4
            uint32 transportPhysics;                        //5
            uint32 mapID;                                   //6
            uint32 worldState1;                             //7
            uint32 canBeStopped;                            //8
        } moTransport;
        //16 GAMEOBJECT_TYPE_DUELFLAG - empty
        //17 GAMEOBJECT_TYPE_FISHINGNODE - empty
        //18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
        struct
        {
            uint32 reqParticipants;                         //0
            uint32 spellId;                                 //1
            uint32 animSpell;                               //2
            uint32 ritualPersistent;                        //3
            uint32 casterTargetSpell;                       //4
            uint32 casterTargetSpellTargets;                //5
            uint32 castersGrouped;                          //6
            uint32 ritualNoTargetCheck;                     //7
        } summoningRitual;
        //19 GAMEOBJECT_TYPE_MAILBOX - empty
        //20 GAMEOBJECT_TYPE_DONOTUSE - empty
        //21 GAMEOBJECT_TYPE_GUARDPOST
        struct
        {
            uint32 creatureID;                              //0
            uint32 charges;                                 //1
        } guardpost;
        //22 GAMEOBJECT_TYPE_SPELLCASTER
        struct
        {
            uint32 spellId;                                 //0
            int32 charges;                                  //1
            uint32 partyOnly;                               //2
            uint32 allowMounted;                            //3 Is usable while on mount/vehicle. (0/1)
            uint32 large;                                   //4
            uint32 conditionID1;                            //5
        } spellcaster;
        //23 GAMEOBJECT_TYPE_MEETINGSTONE
        struct
        {
            uint32 minLevel;                                //0
            uint32 maxLevel;                                //1
            uint32 areaID;                                  //2
        } meetingstone;
        //24 GAMEOBJECT_TYPE_FLAGSTAND
        struct
        {
            uint32 lockId;                                  //0
            uint32 pickupSpell;                             //1
            uint32 radius;                                  //2
            uint32 returnAura;                              //3
            uint32 returnSpell;                             //4
            uint32 noDamageImmune;                          //5
            uint32 openTextID;                              //6
            uint32 losOK;                                   //7
        } flagstand;
        //25 GAMEOBJECT_TYPE_FISHINGHOLE
        struct
        {
            uint32 radius;                                  //0 how close bobber must land for sending loot
            uint32 lootId;                                  //1
            uint32 minSuccessOpens;                         //2
            uint32 maxSuccessOpens;                         //3
            uint32 lockId;                                  //4 -> Lock.dbc; possibly 1628 for all?
        } fishinghole;
        //26 GAMEOBJECT_TYPE_FLAGDROP
        struct
        {
            uint32 lockId;                                  //0
            uint32 eventID;                                 //1
            uint32 pickupSpell;                             //2
            uint32 noDamageImmune;                          //3
            uint32 openTextID;                              //4
        } flagdrop;
        //27 GAMEOBJECT_TYPE_MINI_GAME
        struct
        {
            uint32 gameType;                                //0
        } miniGame;
        //29 GAMEOBJECT_TYPE_CAPTURE_POINT
        struct
        {
            uint32 radius;                                  //0
            uint32 spell;                                   //1
            uint32 worldState1;                             //2
            uint32 worldState2;                             //3
            uint32 winEventID1;                             //4
            uint32 winEventID2;                             //5
            uint32 contestedEventID1;                       //6
            uint32 contestedEventID2;                       //7
            uint32 progressEventID1;                        //8
            uint32 progressEventID2;                        //9
            uint32 neutralEventID1;                         //10
            uint32 neutralEventID2;                         //11
            uint32 neutralPercent;                          //12
            uint32 worldState3;                             //13
            uint32 minSuperiority;                          //14
            uint32 maxSuperiority;                          //15
            uint32 minTime;                                 //16
            uint32 maxTime;                                 //17
            uint32 large;                                   //18
            uint32 highlight;                               //19
            uint32 startingValue;                           //20
            uint32 unidirectional;                          //21
        } capturePoint;
        //30 GAMEOBJECT_TYPE_AURA_GENERATOR
        struct
        {
            uint32 startOpen;                               //0
            uint32 radius;                                  //1
            uint32 auraID1;                                 //2
            uint32 conditionID1;                            //3
            uint32 auraID2;                                 //4
            uint32 conditionID2;                            //5
            int32 serverOnly;                               //6
        } auraGenerator;
        //31 GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY
        struct
        {
            uint32 mapID;                                   //0
            uint32 difficulty;                              //1
        } dungeonDifficulty;
        //32 GAMEOBJECT_TYPE_BARBER_CHAIR
        struct
        {
            uint32 chairheight;                             //0
            uint32 heightOffset;                            //1
        } barberChair;
        //33 GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING          // Much guesswork
        struct
        {
            uint32 intactNumHits;                           //0
            uint32 creditProxyCreature;                     //1
            uint32 empty1;                                  //2
            uint32 intactEvent;                             //3
            uint32 damagedDisplayId;                        //4
            uint32 damagedNumHits;                          //5
            int32 unk1;                                     //6
            uint32 unk2;                                    //7
            uint32 unk3;                                    //8
            uint32 damagedEvent;                            //9
            uint32 destroyedDisplayId;                      //10
            uint32 unk4;                                    //11
            uint32 unk5;                                    //12
            uint32 unk6;                                    //13
            uint32 destroyedEvent;                          //14
            uint32 empty10;                                 //15
            uint32 debuildingTimeSecs;                      //16 // unk, only few with value 300)
            uint32 empty11;                                 //17
            uint32 destructibleData;                        //18 m_ID of DestructibleModelData.DBC
            uint32 empty12;                                 //19
            uint32 unk7;                                    //20
            uint32 empty13;                                 //21
            uint32 rebuildingEvent;                         //22
            uint32 unk8;                                    //23
        } destructibleBuilding;
        //34 GAMEOBJECT_TYPE_GUILDBANK - empty
        //35 GAMEOBJECT_TYPE_TRAPDOOR
        struct
        {
            uint32 whenToPause;                             // 0
            uint32 startOpen;                               // 1
            uint32 autoClose;                               // 2
        } trapDoor;

        // not use for specific field access (only for output with loop by all filed), also this determinate max union size
        struct
        {
            uint32 data[24];
        } raw;
    };

    union
    {
        //6 GAMEOBJECT_TYPE_TRAP
        struct
        {
            uint32 triggerOn;
        } trapCustom;

        //18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
        struct
        {
            uint32 delay;
        } summoningRitualCustom;

        struct
        {
            uint32 data[1];
        } rawCustom;
    };

    uint32 MinMoneyLoot;
    uint32 MaxMoneyLoot;
    uint32 StringId;
    uint32 ScriptId;

    // helpers
    bool IsDespawnAtAction() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_CHEST:  return chest.consumable != 0;
            case GAMEOBJECT_TYPE_GOOBER: return goober.consumable != 0;
            default: return false;
        }
    }

    bool IsUsableMounted() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_MAILBOX: return true;
            case GAMEOBJECT_TYPE_BARBER_CHAIR: return false;
            case GAMEOBJECT_TYPE_QUESTGIVER: return questgiver.allowMounted != 0;
            case GAMEOBJECT_TYPE_TEXT: return text.allowMounted != 0;
            case GAMEOBJECT_TYPE_GOOBER: return goober.allowMounted != 0;
            case GAMEOBJECT_TYPE_SPELLCASTER: return spellcaster.allowMounted != 0;
            default: return false;
        }
    }

    uint32 GetLockId() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:       return door.lockId;
            case GAMEOBJECT_TYPE_BUTTON:     return button.lockId;
            case GAMEOBJECT_TYPE_QUESTGIVER: return questgiver.lockId;
            case GAMEOBJECT_TYPE_CHEST:      return chest.lockId;
            case GAMEOBJECT_TYPE_TRAP:       return trap.lockId;
            case GAMEOBJECT_TYPE_GOOBER:     return goober.lockId;
            case GAMEOBJECT_TYPE_AREADAMAGE: return areadamage.lockId;
            case GAMEOBJECT_TYPE_CAMERA:     return camera.lockId;
            case GAMEOBJECT_TYPE_FLAGSTAND:  return flagstand.lockId;
            case GAMEOBJECT_TYPE_FISHINGHOLE: return fishinghole.lockId;
            case GAMEOBJECT_TYPE_FLAGDROP:   return flagdrop.lockId;
            default: return 0;
        }
    }

    bool GetDespawnPossibility() const                      // despawn at targeting of cast?
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:       return door.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_BUTTON:     return button.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_GOOBER:     return goober.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_FLAGSTAND:  return flagstand.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_FLAGDROP:   return flagdrop.noDamageImmune != 0;
            default: return true;
        }
    }

    bool CannotBeUsedUnderImmunity() const // Cannot be used/activated/looted by players under immunity effects (example: Divine Shield)
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:       return door.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_BUTTON:     return button.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_QUESTGIVER: return questgiver.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_CHEST:      return true;                           // All chests cannot be opened while immune on 3.3.5a
            case GAMEOBJECT_TYPE_GOOBER:     return goober.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_FLAGSTAND:  return flagstand.noDamageImmune != 0;
            case GAMEOBJECT_TYPE_FLAGDROP:   return flagdrop.noDamageImmune != 0;
            default: return false;
        }
    }

    uint32 GetCharges() const                               // despawn at uses amount
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_TRAP:        return trap.charges;
            case GAMEOBJECT_TYPE_GUARDPOST:   return guardpost.charges;
            case GAMEOBJECT_TYPE_SPELLCASTER: return spellcaster.charges;
            default: return 0;
        }
    }

    uint32 GetCooldown() const                              // not triggering at detection target or use until coolodwn expire
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_TRAP:        return trap.cooldown;
            case GAMEOBJECT_TYPE_GOOBER:      return goober.cooldown;
            default: return 0;
        }
    }

    uint32 GetLinkedGameObjectEntry() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_BUTTON:      return button.linkedTrapId;
            case GAMEOBJECT_TYPE_CHEST:       return chest.linkedTrapId;
            case GAMEOBJECT_TYPE_SPELL_FOCUS: return spellFocus.linkedTrapId;
            case GAMEOBJECT_TYPE_GOOBER:      return goober.linkedTrapId;
            default: return 0;
        }
    }

    uint32 GetAutoCloseTime() const
    {
        uint32 autoCloseTime = 0;
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:          autoCloseTime = door.autoCloseTime; break;
            case GAMEOBJECT_TYPE_BUTTON:        autoCloseTime = button.autoCloseTime; break;
            case GAMEOBJECT_TYPE_TRAP:          autoCloseTime = trap.autoCloseTime; break;
            case GAMEOBJECT_TYPE_GOOBER:        autoCloseTime = goober.autoCloseTime; break;
            case GAMEOBJECT_TYPE_TRANSPORT:     autoCloseTime = transport.autoCloseTime; break;
            case GAMEOBJECT_TYPE_AREADAMAGE:    autoCloseTime = areadamage.autoCloseTime; break;
            default: break;
        }
        return autoCloseTime / IN_MILLISECONDS;             // prior to 3.0.3, conversion was / 0x10000;
    }

    uint32 GetLootId() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_CHEST:       return chest.lootId;
            case GAMEOBJECT_TYPE_FISHINGHOLE: return fishinghole.lootId;
            default: return 0;
        }
    }

    uint32 GetGossipMenuId() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_QUESTGIVER:    return questgiver.gossipID;
            case GAMEOBJECT_TYPE_GOOBER:        return goober.gossipID;
            default: return 0;
        }
    }

    bool IsLargeGameObject() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_BUTTON:            return button.large != 0;
            case GAMEOBJECT_TYPE_QUESTGIVER:        return questgiver.large != 0;
            case GAMEOBJECT_TYPE_GENERIC:           return _generic.large != 0;
            case GAMEOBJECT_TYPE_TRAP:              return trap.large != 0;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:       return spellFocus.large != 0;
            case GAMEOBJECT_TYPE_GOOBER:            return goober.large != 0;
            case GAMEOBJECT_TYPE_TRANSPORT:         return true;
            case GAMEOBJECT_TYPE_SPELLCASTER:       return spellcaster.large != 0;
            case GAMEOBJECT_TYPE_CAPTURE_POINT:     return capturePoint.large != 0;
            default: return false;
        }
    }

    bool IsInfiniteGameObject() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING: return true;
            case GAMEOBJECT_TYPE_TRAPDOOR:              return true;
            default: return false;
        }
    }

    bool IsLargeOrBiggerGameObject() const
    {
        return IsLargeGameObject() || IsInfiniteGameObject();
    }

    bool IsServerOnly() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_GENERIC: return _generic.serverOnly;
            case GAMEOBJECT_TYPE_TRAP: return trap.serverOnly;
            case GAMEOBJECT_TYPE_SPELL_FOCUS: return spellFocus.serverOnly;
            case GAMEOBJECT_TYPE_AURA_GENERATOR: return auraGenerator.serverOnly;
            default: return false;
        }
    }

    HighGuid GetHighGuid() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_TRANSPORT:
            case GAMEOBJECT_TYPE_MO_TRANSPORT:
            case GAMEOBJECT_TYPE_TRAPDOOR:
                return HIGHGUID_MO_TRANSPORT;
            default: return HIGHGUID_GAMEOBJECT;
        }
    }

    bool IsSlowUpdateObject() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_BINDER:
            case GAMEOBJECT_TYPE_GENERIC:
            case GAMEOBJECT_TYPE_MAP_OBJECT:
            case GAMEOBJECT_TYPE_DUEL_ARBITER:
            case GAMEOBJECT_TYPE_MAILBOX:
            case GAMEOBJECT_TYPE_MEETINGSTONE:
            case GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY:
            case GAMEOBJECT_TYPE_GUILD_BANK:
            case GAMEOBJECT_TYPE_BARBER_CHAIR:
                return true;
            default: return false;
        }
    }
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

struct GameObjectLocale
{
    std::vector<std::string> Name;
    std::vector<std::string> CastBarCaption;
};

struct QuaternionData
{
    float x, y, z, w;

    QuaternionData() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }
    QuaternionData(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { }

    bool isUnit() const;
    void toEulerAnglesZYX(float& Z, float& Y, float& X) const;
    static QuaternionData fromEulerAnglesZYX(float Z, float Y, float X);
};

// from `gameobject`
struct GameObjectData
{
    uint32 id;                                              // entry in gamobject_template
    uint16 mapid;
    uint16 phaseMask;
    float posX;
    float posY;
    float posZ;
    float orientation;
    QuaternionData rotation;
    int32 spawntimesecsmin;
    int32 spawntimesecsmax;
    uint32 animprogress;
    int32 goState;
    uint32 StringId;
    uint8 spawnMask;
    uint16 gameEvent;
    uint16 GuidPoolId;
    uint16 EntryPoolId;
    uint16 OriginalZoneId;
    QuaternionData path_rotation;

    uint32 GetRandomRespawnTime() const { return urand(uint32(spawntimesecsmin), uint32(spawntimesecsmax)); }

    // return false if it should be handled by GameEventMgr or PoolMgr
    bool IsNotPartOfPoolOrEvent() const { return (!gameEvent && !GuidPoolId && !EntryPoolId); }
};

struct GameObjectTemplateAddon
{
    std::array<uint32, 4> artKits = {};
};

enum class GameObjectActions : uint32
{
    // Name from client executable    // Comments
    NONE,                             // -NONE-
    ANIMATE_CUSTOM_0,                 // Animate Custom0
    ANIMATE_CUSTOM_1,                 // Animate Custom1
    ANIMATE_CUSTOM_2,                 // Animate Custom2
    ANIMATE_CUSTOM_3,                 // Animate Custom3
    DISTURB,                          // Disturb                          // Triggers trap
    UNLOCK,                           // Unlock                           // Resets GO_FLAG_LOCKED
    LOCK,                             // Lock                             // Sets GO_FLAG_LOCKED
    OPEN,                             // Open                             // Sets GO_STATE_ACTIVE
    OPEN_AND_UNLOCK,                  // Open + Unlock                    // Sets GO_STATE_ACTIVE and resets GO_FLAG_LOCKED
    CLOSE,                            // Close                            // Sets GO_STATE_READY
    TOGGLE_OPEN,                      // Toggle Open
    DESTROY,                          // Destroy                          // Sets GO_STATE_DESTROYED
    REBUILD,                          // Rebuild                          // Resets from GO_STATE_DESTROYED
    CREATION,                         // Creation
    DESPAWN,                          // Despawn
    MAKE_INERT,                       // Make Inert                       // Disables interactions
    MAKE_ACTIVE,                      // Make Active                      // Enables interactions
    CLOSE_AND_LOCK,                   // Close + Lock                     // Sets GO_STATE_READY and sets GO_FLAG_LOCKED
    USE_ART_KIT_0,                    // Use ArtKit0                      // 46904: 121
    USE_ART_KIT_1,                    // Use ArtKit1                      // 36639: 81, 46903: 122
    USE_ART_KIT_2,                    // Use ArtKit2
    USE_ART_KIT_3,                    // Use ArtKit3
    SET_TAP_LIST,                     // Set Tap List
};

// For containers:  [GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY        -> ...
// For bobber:      GO_NOT_READY  ->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED-><deleted>
// For door(closed):[GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY(close) -> ...
// For door(open):  [GO_NOT_READY]->GO_READY (open) ->GO_ACTIVATED (close)->GO_JUST_DEACTIVATED->GO_READY(open)  -> ...
enum LootState
{
    GO_NOT_READY = 0,
    GO_READY,                                               // can be ready but despawned, and then not possible activate until spawn
    GO_ACTIVATED,
    GO_JUST_DEACTIVATED
};

enum CapturePointState
{
    CAPTURE_STATE_NEUTRAL = 0,
    CAPTURE_STATE_PROGRESS_ALLIANCE,
    CAPTURE_STATE_PROGRESS_HORDE,
    CAPTURE_STATE_CONTEST_ALLIANCE,
    CAPTURE_STATE_CONTEST_HORDE,
    CAPTURE_STATE_WIN_ALLIANCE,
    CAPTURE_STATE_WIN_HORDE
};

enum CapturePointSliderValue
{
    CAPTURE_SLIDER_ALLIANCE         = 100,                  // full alliance
    CAPTURE_SLIDER_HORDE            = 0,                    // full horde
    CAPTURE_SLIDER_MIDDLE           = 50                    // middle
};

enum GameobjectExtraFlags
{
    GAMEOBJECT_EXTRA_FLAG_CUSTOM_ANIM_ON_USE = 0x00000001,    // GO that plays custom animation on usage
    GAMEOBJECT_EXTRA_FLAG_DYNGUID            = 0x00000002,    // Temporary - Uses new dynguid system
    GAMEOBJECT_EXTRA_FLAG_ACTIVE             = 0x00001000,    // Always active
};

class Unit;
class GameObjectModel;
struct GameObjectDisplayInfoEntry;
struct TransportAnimation;
class Item;
class GameObjectGroup;

// 5 sec for bobber catch
#define FISHING_BOBBER_READY_TIME 5

#define GO_ANIMPROGRESS_DEFAULT 0xFF

class GameObject : public WorldObject
{
    public:
        explicit GameObject();
        ~GameObject();

        static GameObject* CreateGameObject(uint32 entry);

        void AddToWorld() override;
        void RemoveFromWorld() override;

        virtual bool Create(uint32 dbGuid, uint32 guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang,
                    const QuaternionData & rotation = QuaternionData(), uint8 animprogress = GO_ANIMPROGRESS_DEFAULT, GOState go_state = GO_STATE_READY);

        void Update(const uint32 diff) override;
        void Heartbeat() override;
        GameObjectInfo const* GetGOInfo() const;
        GameObjectTemplateAddon const* GetTemplateAddon() const;

        bool IsTransport() const;
        bool IsMoTransport() const;
        bool IsDynTransport() const;

        bool HasStaticDBSpawnData() const;                  // listed in `gameobject` table and have fixed in DB guid

        // z_rot, y_rot, x_rot - rotation angles around z, y and x axes
        void SetLocalRotationAngles(float z_rot, float y_rot, float x_rot);
        void SetLocalRotation(float qx, float qy, float qz, float qw);
        void SetTransportPathRotation(const QuaternionData& rotation);      // transforms(rotates) transport's path
        int64 GetPackedLocalRotation() const { return m_packedRotation; }
        QuaternionData GetWorldRotation() const;
        QuaternionData const GetLocalRotation() const;

        // overwrite WorldObject function for proper name localization
        const char* GetNameForLocaleIdx(int32 loc_idx) const override;

        void SaveToDB() const;
        void SaveToDB(uint32 mapid, uint8 spawnMask, uint32 phaseMask) const;
        bool LoadFromDB(uint32 dbGuid, Map* map, uint32 newGuid, uint32 forcedEntry, GenericTransport* transport = nullptr);
        void DeleteFromDB() const;

        ObjectGuid const& GetOwnerGuid() const override { return GetGuidValue(OBJECT_FIELD_CREATED_BY); }
        void SetOwnerGuid(ObjectGuid guid) override;
        ObjectGuid const GetSpawnerGuid() const override { return m_spawnerGuid; }
        void SetSpawnerGuid(ObjectGuid guid) { m_spawnerGuid = guid; }

        Unit* GetOwner() const;
        WorldObject* GetSpawner() const;

        void SetSpellId(uint32 id)
        {
            m_spawnedByDefault = false;                     // all summoned object is despawned after delay
            m_spellId = id;
        }
        uint32 GetSpellId() const { return m_spellId;}

        void ForcedDespawn(uint32 timeMSToDespawn = 0);

        time_t GetRespawnTime() const { return m_respawnTime; }
        time_t GetRespawnTimeEx() const
        {
            time_t now = time(nullptr);
            if (m_respawnTime > now)
                return m_respawnTime;
            return now;
        }

        void SetRespawnTime(time_t respawn)
        {
            m_respawnTime = respawn > 0 ? time(nullptr) + respawn : 0;
            m_respawnDelay = respawn > 0 ? uint32(respawn) : 0;
        }
        void Respawn();
        bool IsSpawned() const
        {
            return m_respawnDelay == 0 ||
                   (m_respawnTime > 0 && !m_spawnedByDefault) ||
                   (m_respawnTime == 0 && m_spawnedByDefault);
        }
        bool IsSpawnedByDefault() const { return m_spawnedByDefault; }
        uint32 GetRespawnDelay() const override { return m_respawnDelay; }
        void SetRespawnDelay(uint32 delay, bool once = false) { m_respawnDelay = delay; m_respawnOverriden = true; m_respawnOverrideOnce = once; }
        void SetForcedDespawn() { m_forcedDespawn = true; };
        void SetChestDespawn();
        void Refresh();
        void Delete();

        GameobjectTypes GetGoType() const { return GameobjectTypes(GetByteValue(GAMEOBJECT_BYTES_1, 1)); }
        void SetGoType(GameobjectTypes type) { SetByteValue(GAMEOBJECT_BYTES_1, 1, type); }
        GOState GetGoState() const { return GOState(GetByteValue(GAMEOBJECT_BYTES_1, 0)); }
        virtual void SetGoState(GOState state);
        uint8 GetGoArtKit() const { return GetByteValue(GAMEOBJECT_BYTES_1, 2); }
        void SetGoArtKit(uint8 artkit) { SetByteValue(GAMEOBJECT_BYTES_1, 2, artkit); }
        uint8 GetGoAnimProgress() const { return GetByteValue(GAMEOBJECT_BYTES_1, 3); }
        void SetGoAnimProgress(uint8 animprogress) { SetByteValue(GAMEOBJECT_BYTES_1, 3, animprogress); }
        uint32 GetDisplayId() const { return GetUInt32Value(GAMEOBJECT_DISPLAYID); }
        void SetDisplayId(uint32 modelId);
        void SetPhaseMask(uint32 newPhaseMask, bool update) override;
        uint32 GetFaction() const override { return GetUInt32Value(GAMEOBJECT_FACTION); }
        void SetFaction(uint32 faction) { SetUInt32Value(GAMEOBJECT_FACTION, faction); }
        uint32 GetLevel() const override { return GetUInt32Value(GAMEOBJECT_LEVEL); }

        void Use(Unit* user, SpellEntry const* spellInfo = nullptr);

        LootState GetLootState() const { return m_lootState; }
        void SetLootState(LootState state, Unit* user = nullptr);

        void AddToSkillupList(Player* player);
        bool IsInSkillupList(Player* player) const;
        void ClearSkillupList() { m_SkillupSet.clear(); }
        void ClearAllUsesData()
        {
            ClearSkillupList();
            m_useTimes = 0;
            m_firstUser.Clear();
            m_UniqueUsers.clear();
        }

        void SetActionTarget(ObjectGuid guid) { m_actionTarget = guid; };
        void AddUniqueUse(Player* player);
        void AddUse() { ++m_useTimes; }
        bool IsInUse() const { return m_isInUse; }
        void SetInUse(bool use);

        uint32 GetUseCount() const { return m_useTimes; }
        uint32 GetUniqueUseCount() const { return m_UniqueUsers.size(); }

        void SaveRespawnTime() override;

        // Loot System
        ObjectGuid GetLootRecipientGuid() const { return m_lootRecipientGuid; }
        uint32 GetLootGroupRecipientId() const { return m_lootGroupRecipientId; }
        Player* GetLootRecipient() const;                   // use group cases as prefered
        Group* GetGroupLootRecipient() const;
        bool HasLootRecipient() const { return m_lootGroupRecipientId || !m_lootRecipientGuid.IsEmpty(); }
        bool IsGroupLootRecipient() const { return m_lootGroupRecipientId != 0; }
        void SetLootRecipient(Unit* pUnit);
        Player* GetOriginalLootRecipient() const;           // ignore group changes/etc, not for looting

        bool HasQuest(uint32 quest_id) const override;
        bool HasInvolvedQuest(uint32 quest_id) const override;
        bool ActivateToQuest(Player* pTarget) const;
        void UseDoorOrButton(uint32 time_to_restore = 0, bool alternative = false);
        // 0 = use `gameobject`.`spawntimesecs`
        void ResetDoorOrButton(Unit* user = nullptr);
        void UseOpenableObject(bool open, uint32 withRestoreTime = 0, bool useAlternativeState = false);

        ReputationRank GetReactionTo(Unit const* unit) const override;

        bool IsEnemy(Unit const* unit) const override;
        bool IsFriend(Unit const* unit) const override;

        bool CanAttackSpell(Unit const* target, SpellEntry const* spellInfo = nullptr, bool isAOE = false) const override;
        bool CanAssistSpell(Unit const* target, SpellEntry const* spellInfo = nullptr) const override;

        GameObject* SummonLinkedTrapIfAny() const;
        void TriggerLinkedGameObject(Unit* target) const;
        GameObject* GetLinkedTrap();
        void SetLinkedTrap(GameObject* linkedTrap) { m_linkedTrap = linkedTrap->GetObjectGuid(); }

        // Destructible GO handling
        void DealGameObjectDamage(uint32 damage, uint32 spell, Unit* caster);
        void RebuildGameObject(Unit* caster);
        void SetDestructibleState(GameObjectDestructibleState state, Unit* attackerOrHealer = nullptr, bool setHealth = false);
        GameObjectDestructibleState GetDestructibleState() const;
        uint32 GetHealth() const { return m_useTimes; }
        uint32 GetMaxHealth() const { return m_goInfo->destructibleBuilding.intactNumHits + m_goInfo->destructibleBuilding.damagedNumHits; }

        bool isVisibleForInState(Player const* u, WorldObject const* viewPoint, bool inVisibleList) const override;

        bool IsCollisionEnabled() const;                    // Check if a go should collide. Like if a door is closed

        GameObject* LookupFishingHoleAround(float range) const;

        void SetCapturePointSlider(float value, bool isLocked);
        float GetCapturePointSliderValue() const { return m_captureSlider; }

        float GetInteractionDistance() const;
        float GetCollisionHeight() const override { return 1.f; } // to get away with ground collision

        GridReference<GameObject>& GetGridRef() { return m_gridRef; }

        uint32 GetScriptId() const;
        void AIM_Initialize();
        void OnEventHappened(uint16 eventId, bool activate, bool resume) override { return AI()->OnEventHappened(eventId, activate, resume); }
        GameObjectAI* AI() const { return m_AI.get(); }

        GameObjectModel* m_model;
        void AddModelToMap();
        void RemoveModelFromMap();
        void UpdateModelPosition();

        bool _IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const override;

        bool IsAtInteractDistance(Position const& pos, float radius, bool is3D = true) const;
        bool IsAtInteractDistance(Player const* player, uint32 maxRange = 0) const;

        SpellEntry const* GetSpellForLock(Player const* player) const;

        float GetStationaryX() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionX(); return 0.f; }
        float GetStationaryY() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionY(); return 0.f; }
        float GetStationaryZ() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionZ(); return 0.f; }
        float GetStationaryO() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionO(); return GetOrientation(); }

        std::pair<float, float> GetClosestChairSlotPosition(Unit* user) const;

        SpellCastResult CastSpell(Unit* temporaryCaster, Unit* Victim, uint32 spellId, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);

        SpellCastResult CastSpell(Unit* temporaryCaster, Unit* Victim, uint32 spellId, TriggerCastFlags triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr)
        {
            return CastSpell(temporaryCaster, Victim, spellId, uint32(triggeredFlags), castItem, triggeredByAura, originalCaster, triggeredBy);
        }

        SpellCastResult CastSpell(Unit* temporaryCaster, Unit* Victim, SpellEntry const* spellInfo, uint32 triggeredFlags, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), SpellEntry const* triggeredBy = nullptr);

        void GenerateLootFor(Player* player); // used to tie chest loot to encounter at the moment of its end

        HighGuid GetParentHigh() const override { return HIGHGUID_GAMEOBJECT; }

        void SetCooldown(uint32 cooldown); // seconds

        void SetGameObjectGroup(GameObjectGroup* group);
        void ClearGameObjectGroup();
        GameObjectGroup* GetGameObjectGroup() const { return m_goGroup; }

        void UpdateNextUpdateTime() override;
        uint32 ShouldPerformObjectUpdate(uint32 const diff) override;

    protected:
        uint32      m_spellId;
        time_t      m_respawnTime;                          // (secs) time of next respawn (or despawn if GO have owner()),
        uint32      m_respawnDelay;                         // (secs) if 0 then current GO state no dependent from timer
        bool        m_respawnOverriden;
        bool        m_respawnOverrideOnce;
        bool        m_forcedDespawn;
        LootState   m_lootState;
        bool        m_spawnedByDefault;
        time_t      m_cooldownTime;                         // used as internal reaction delay time store (not state change reaction).
        // For traps/goober this: spell casting cooldown, for doors/buttons: reset time.

        uint32      m_captureTimer;                         // (msecs) timer used for capture points
        float       m_captureSlider;                        // capture point slider value in range of [0..100]
        CapturePointState m_captureState;

        GuidSet m_SkillupSet;                               // players that already have skill-up at GO use

        uint32 m_useTimes;                                  // amount uses/charges triggered - also used for health for DESTRUCTIBLE_BUILDING

        // collected only for GAMEOBJECT_TYPE_SUMMONING_RITUAL
        ObjectGuid m_firstUser;                             // first GO user, in most used cases owner, but in some cases no, for example non-summoned multi-use GAMEOBJECT_TYPE_SUMMONING_RITUAL
        GuidSet m_UniqueUsers;                              // all players who use item, some items activated after specific amount unique uses

        GameObjectInfo const* m_goInfo;
        GameObjectDisplayInfoEntry const* m_displayInfo;
        int64 m_packedRotation;
        QuaternionData m_localRotation;

        Position m_stationaryPosition;

        // Loot System
        ObjectGuid m_lootRecipientGuid;                     // player who will have rights for looting if m_lootGroupRecipient==0 or group disbanded
        uint32 m_lootGroupRecipientId;                      // group who will have rights for looting if set and exist

        // Used for trap type
        time_t m_rearmTimer;                                // timer to rearm the trap once disarmed

        // Used for chest type
        bool m_isInUse;                                     // only one player at time are allowed to open chest
        time_t m_reStockTimer;                              // timer to refill the chest
        TimePoint m_despawnTimer;                           // timer to despawn the chest if something changed in it

        void TriggerSummoningRitual();
        void TriggerDelayedAction();

        uint32 m_delayedActionTimer;                        // used for delayed GO actions

        ObjectGuid m_actionTarget;                          // used for setting target of Summoning rituals

        ObjectGuid m_linkedTrap;

        std::unique_ptr<GameObjectAI> m_AI;

        ObjectGuid m_spawnerGuid;

        GameObjectGroup* m_goGroup;

    private:
        void SwitchDoorOrButton(bool activate, bool alternative = false);
        void TickCapturePoint();
        void UpdateModel();                                 // updates model in case displayId were changed
        void UpdateCollisionState() const;                  // updates state in Map's dynamic collision tree
        void ForceGameObjectHealth(int32 diff, Unit* caster); // should use SetDestructibleState from outside

        GridReference<GameObject> m_gridRef;
};

class ForcedDespawnDelayGameObjectEvent : public BasicEvent
{
    public:
        ForcedDespawnDelayGameObjectEvent(GameObject& owner) : BasicEvent(), m_owner(owner) { }
        bool Execute(uint64 e_time, uint32 p_time) override;

    private:
        GameObject& m_owner;
        bool m_onlyAlive;
};


#endif
