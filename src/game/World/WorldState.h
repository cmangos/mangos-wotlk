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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef WORLD_STATE_H
#define WORLD_STATE_H

#include "Policies/Singleton.h"
#include "Globals/ObjectMgr.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Globals/SharedDefines.h"
#include <atomic>
#include <string>

enum ZoneIds
{
    ZONEID_HELLFIRE_PENINSULA   = 3483,
    ZONEID_HELLFIRE_RAMPARTS    = 3562,
    ZONEID_HELLFIRE_CITADEL     = 3563,
    ZONEID_BLOOD_FURNACE        = 3713,
    ZONEID_SHATTERED_HALLS      = 3714,
    ZONEID_MAGTHERIDON_LAIR     = 3836,

    ZONEID_SHATTRATH    = 3703,
    ZONEID_BOTANICA     = 3847,
    ZONEID_ARCATRAZ     = 3848,
    ZONEID_MECHANAR     = 3849,
};

enum AreaIds
{
    AREAID_SKYGUARD_OUTPOST     = 3964,
    AREAID_SHARTUUL_TRANSPORTER = 4008,
    AREAID_DEATHS_DOOR          = 3831,
    AREAID_THERAMORE_ISLE       = 513,
};

enum SpellId
{
    SPELL_TROLLBANES_COMMAND    = 39911,
    SPELL_NAZGRELS_FAVOR        = 39913,

    SPELL_ADAL_SONG_OF_BATTLE   = 39953,
};

enum GoId
{
    OBJECT_MAGTHERIDONS_HEAD = 184640
};

enum Conditions
{
    ORGRIMMAR_UNDERCITY = 164871,
    GROMGOL_ORGRIMMAR   = 175080,
    GROMGOL_UNDERCITY   = 176495,
};

enum Events
{
    CUSTOM_EVENT_YSONDRE_DIED,
    CUSTOM_EVENT_LETHON_DIED,
    CUSTOM_EVENT_EMERISS_DIED,
    CUSTOM_EVENT_TAERAR_DIED,
    CUSTOM_EVENT_ADALS_SONG_OF_BATTLE,
};

enum SaveIds
{
    SAVE_ID_EMERALD_DRAGONS,
    SAVE_ID_AHN_QIRAJ,
    SAVE_ID_QUEL_DANAS,
    SAVE_ID_EXPANSION_RELEASE,
};

enum GameEvents
{
    GAME_EVENT_BEFORE_THE_STORM = 100,
    GAME_EVENT_QUEL_DANAS_PHASE_1 = 101,
    // next 10 are reserved for quel danas - 110
    GAME_EVENT_ECHOES_OF_DOOM = 111,
};

// To be used
struct AhnQirajData
{
    std::string GetData() { return ""; }
};

struct QuelDanasData
{
    std::string GetData() { return ""; }
};

// Intended for implementing server wide scripts, note: all behaviour must be safeguarded towards multithreading
class WorldState
{
    public:
        WorldState();
        virtual ~WorldState();

        void Load();
        void Save(SaveIds saveId);

        // Called when a gameobject is created or removed
        void HandleGameObjectUse(GameObject* go, Unit* user);
        void HandleGameObjectRevertState(GameObject* go);

        void HandlePlayerEnterZone(Player* player, uint32 zoneId);
        void HandlePlayerLeaveZone(Player* player, uint32 zoneId);

        void HandlePlayerEnterArea(Player* player, uint32 areaId);
        void HandlePlayerLeaveArea(Player* player, uint32 areaId);

        bool IsConditionFulfilled(uint32 conditionId, uint32 state) const;
        void HandleConditionStateChange(uint32 conditionId, uint32 state);

        void HandleExternalEvent(uint32 eventId);
        void ExecuteOnAreaPlayers(uint32 areaId, std::function<void(Player*)> executor);

        void Update(const uint32 diff);

        // tbc section
        void BuffMagtheridonTeam(Team team);
        void DispelMagtheridonTeam(Team team);

        void BuffAdalsSongOfBattle();
        void DispelAdalsSongOfBattle();

        // Release events
        uint8 GetExpansion() const { return m_expansion; }
        bool SetExpansion(uint8 expansion);
    private:
        std::map<uint32, GuidVector> m_areaPlayers;
        std::map<uint32, std::atomic<uint32>> m_transportStates; // atomic to avoid having to lock

        std::mutex m_mutex; // all World State operations are thread unsafe
        uint32 m_saveTimer;

        // vanilla section
        bool IsDragonSpawned(uint32 entry);
        void RespawnEmeraldDragons();

        uint8 m_emeraldDragonsState;
        uint32 m_emeraldDragonsTimer;
        std::vector<uint32> m_emeraldDragonsChosenPositions;
        AhnQirajData m_aqData;

        // tbc section
        bool m_isMagtheridonHeadSpawnedHorde;
        bool m_isMagtheridonHeadSpawnedAlliance;
        ObjectGuid m_guidMagtheridonHeadHorde;
        ObjectGuid m_guidMagtheridonHeadAlliance;
        GuidVector m_magtheridonHeadPlayers;

        GuidVector m_adalSongOfBattlePlayers;
        uint32 m_adalSongOfBattleTimer;

        QuelDanasData m_quelDanasData;

        // Release Events
        void StartExpansionEvent();

        std::atomic<uint8> m_expansion;
};

#define sWorldState MaNGOS::Singleton<WorldState>::Instance()

#endif

