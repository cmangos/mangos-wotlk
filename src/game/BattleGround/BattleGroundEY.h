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

#ifndef __BATTLEGROUNDEY_H
#define __BATTLEGROUNDEY_H

#include "Tools/Language.h"

class BattleGround;

enum EYUpdateTimers
{
    EY_FLAG_RESPAWN_TIME                                = 10 * IN_MILLISECONDS,
    EY_RESOURCES_UPDATE_TIME                            = 2 * IN_MILLISECONDS,
    EY_FEL_REAVER_FLAG_UPDATE_TIME                      = 1 * IN_MILLISECONDS,
};

enum EYWorldStates
{
    WORLD_STATE_EY_RESOURCES_ALLIANCE                   = 2749,
    WORLD_STATE_EY_RESOURCES_HORDE                      = 2750,
    WORLD_STATE_EY_TOWER_COUNT_ALLIANCE                 = 2752,
    WORLD_STATE_EY_TOWER_COUNT_HORDE                    = 2753,

    WORLD_STATE_EY_BLOOD_ELF_TOWER_ALLIANCE             = 2723,
    WORLD_STATE_EY_BLOOD_ELF_TOWER_HORDE                = 2724,
    WORLD_STATE_EY_BLOOD_ELF_TOWER_NEUTRAL              = 2722,
    //WORLD_STATE_EY_BLOOD_ELF_TOWER_ALLIANCE_CONFLICT  = 2735, // unused on retail
    //WORLD_STATE_EY_BLOOD_ELF_TOWER_HORDE_CONFLICT     = 2736, // unused on retail

    WORLD_STATE_EY_FEL_REAVER_RUINS_ALLIANCE            = 2726,
    WORLD_STATE_EY_FEL_REAVER_RUINS_HORDE               = 2727,
    WORLD_STATE_EY_FEL_REAVER_RUINS_NEUTRAL             = 2725,
    //WORLD_STATE_EY_FEL_REAVER_RUINS_ALLIANCE_CONFLICT = 2739, // unused on retail
    //WORLD_STATE_EY_FEL_REAVER_RUINS_HORDE_CONFLICT    = 2740, // unused on retail

    WORLD_STATE_EY_MAGE_TOWER_ALLIANCE                  = 2730,
    WORLD_STATE_EY_MAGE_TOWER_HORDE                     = 2729,
    WORLD_STATE_EY_MAGE_TOWER_NEUTRAL                   = 2728,
    //WORLD_STATE_EY_MAGE_TOWER_ALLIANCE_CONFLICT       = 2741, // unused on retail
    //WORLD_STATE_EY_MAGE_TOWER_HORDE_CONFLICT          = 2742, // unused on retail

    WORLD_STATE_EY_DRAENEI_RUINS_ALLIANCE               = 2732,
    WORLD_STATE_EY_DRAENEI_RUINS_HORDE                  = 2733,
    WORLD_STATE_EY_DRAENEI_RUINS_NEUTRAL                = 2731,
    //WORLD_STATE_EY_DRAENEI_RUINS_ALLIANCE_CONFLICT    = 2738, // unused on retail
    //WORLD_STATE_EY_DRAENEI_RUINS_HORDE_CONFLICT       = 2737, // unused on retail

    WORLD_STATE_EY_NETHERSTORM_FLAG_READY               = 2757,
    WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_ALLIANCE      = 2769,
    WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_HORDE         = 2770,

    WORLD_STATE_EY_CAPTURE_POINT_SLIDER_DISPLAY         = 2718
};

enum EYCapturePoints
{
    GO_CAPTURE_POINT_BLOOD_ELF_TOWER            = 184080,
    GO_CAPTURE_POINT_FEL_REAVER_RUINS           = 184081,
    GO_CAPTURE_POINT_MAGE_TOWER                 = 184082,
    GO_CAPTURE_POINT_DRAENEI_RUINS              = 184083
};

enum EYGameObjects
{
    GO_EY_NETHERSTORM_FLAG                      = 184141,
    GO_EY_NETHERSTORM_FLAG_DROP                 = 184142,                   // temp summoned gameobject when flag is dropped
    GO_EY_NETHERSTORM_FLAG_VISUAL               = 184493,                   // visual flag; spawned at any of the bases when team scores

    GO_EY_VISUAL_BANNER_HORDE                   = 184380,                   // visual gameobjects; the usage of these objects isn't clear
    GO_EY_VISUAL_BANNER_ALLIANCE                = 184381,                   // main capture point objects use art kit for alliance / horde flags
    GO_EY_VISUAL_BANNER_NEUTRAL                 = 184382,
};

enum EYEvents
{
    //EVENT_BLOOD_ELF_TOWER_WIN_ALLIANCE        = 12965,
    //EVENT_BLOOD_ELF_TOWER_WIN_HORDE           = 12964,
    EVENT_BLOOD_ELF_TOWER_PROGRESS_ALLIANCE     = 12905,
    EVENT_BLOOD_ELF_TOWER_PROGRESS_HORDE        = 12904,
    EVENT_BLOOD_ELF_TOWER_NEUTRAL_ALLIANCE      = 12957,
    EVENT_BLOOD_ELF_TOWER_NEUTRAL_HORDE         = 12956,

    //EVENT_FEL_REAVER_RUINS_WIN_ALLIANCE       = 12969,
    //EVENT_FEL_REAVER_RUINS_WIN_HORDE          = 12968,
    EVENT_FEL_REAVER_RUINS_PROGRESS_ALLIANCE    = 12911,
    EVENT_FEL_REAVER_RUINS_PROGRESS_HORDE       = 12910,
    EVENT_FEL_REAVER_RUINS_NEUTRAL_ALLIANCE     = 12960,
    EVENT_FEL_REAVER_RUINS_NEUTRAL_HORDE        = 12961,

    //EVENT_MAGE_TOWER_WIN_ALLIANCE             = 12971,
    //EVENT_MAGE_TOWER_WIN_HORDE                = 12970,
    EVENT_MAGE_TOWER_PROGRESS_ALLIANCE          = 12909,
    EVENT_MAGE_TOWER_PROGRESS_HORDE             = 12908,
    EVENT_MAGE_TOWER_NEUTRAL_ALLIANCE           = 12962,
    EVENT_MAGE_TOWER_NEUTRAL_HORDE              = 12963,

    //EVENT_DRAENEI_RUINS_WIN_ALLIANCE          = 12967,
    //EVENT_DRAENEI_RUINS_WIN_HORDE             = 12966,
    EVENT_DRAENEI_RUINS_PROGRESS_ALLIANCE       = 12907,
    EVENT_DRAENEI_RUINS_PROGRESS_HORDE          = 12906,
    EVENT_DRAENEI_RUINS_NEUTRAL_ALLIANCE        = 12958,
    EVENT_DRAENEI_RUINS_NEUTRAL_HORDE           = 12959,

    // The following event ids are used for the flag handling
    EVENT_NETHERSTORM_FLAG_PICKUP               = 13000,                    // not used; called when player clicks on the flag drop
    EVENT_NETHERSTORM_FLAG_SPELL                = 13042,                    // called when player casts flag aura - source player, target flag go
};

enum EYSounds
{
    EY_SOUND_FLAG_PICKED_UP_ALLIANCE    = 8212,
    EY_SOUND_FLAG_CAPTURED_HORDE        = 8213,
    EY_SOUND_FLAG_PICKED_UP_HORDE       = 8174,
    EY_SOUND_FLAG_CAPTURED_ALLIANCE     = 8173,
    EY_SOUND_FLAG_RESET                 = 8192
};

enum EYSpells
{
    EY_SPELL_NETHERSTORM_FLAG           = 34976,                            // cast on player click on GO 184141 or 184142; sends event 13042
    EY_SPELL_PLAYER_DROPPED_FLAG        = 34991,                            // summon gameobject 184142
};

enum EYPointsTrigger
{
    AREATRIGGER_BLOOD_ELF_TOWER_POINT   = 4476, // also 4512
    AREATRIGGER_FEL_REAVER_RUINS_POINT  = 4514, // also 4515
    AREATRIGGER_MAGE_TOWER_POINT        = 4516, // also 4517
    AREATRIGGER_DRAENEI_RUINS_POINT     = 4518, // also 4519

    AREATRIGGER_BLOOD_ELF_TOWER_BUFF    = 4568,
    AREATRIGGER_FEL_REAVER_RUINS_BUFF   = 4569,
    //AREATRIGGER_FEL_REAVER_RUINS_BUFF_2 = 5866,
    AREATRIGGER_MAGE_TOWER_BUFF         = 4570,
    AREATRIGGER_DRAENEI_RUINS_BUFF      = 4571

    //AREATRIGGER_EY_HORDE_START          = 4530,
    //AREATRIGGER_EY_ALLIANCE_START       = 4531
};

enum EYGaveyards
{
    GRAVEYARD_EY_MAIN_ALLIANCE      = 1103,
    GRAVEYARD_EY_MAIN_HORDE         = 1104,
    GRAVEYARD_FEL_REAVER_RUINS      = 1105,
    GRAVEYARD_BLOOD_ELF_TOWER       = 1106,
    GRAVEYARD_DRAENEI_RUINS         = 1107,
    GRAVEYARD_MAGE_TOWER            = 1108,

    EY_ZONE_ID_MAIN                 = 3820,
};

// node-events work like this: event1:nodeid, event2:state (0alliance,1horde,2neutral)
// all other event2 are just node Ids; see the next enum
enum EYScriptEvents
{
    EY_EVENT_CAPTURE_FLAG = 4,            // event1=4, event2=nodeid or 4 for the default center spawn
    EY_EVENT2_FLAG_CENTER = 4,            // maximum node is 3 so 4 for center is ok
};

enum EYNodes
{
    // TODO: Re-change order after we drop battleground_event and associated tables
    NODE_BLOOD_ELF_TOWER            = 1,
    NODE_FEL_REAVER_RUINS           = 0,
    NODE_MAGE_TOWER                 = 3,
    NODE_DRAENEI_RUINS              = 2,

    EY_MAX_NODES                    = 4,
};

enum EYAchievCriteria
{
    EY_TIMED_ACHIEV_FLURRY          = 13180,

    EY_ACHIEV_CRIT_DOMINATION       = 1239,
    EY_ACHIEV_CRIT_STORM_GLORY      = 3693,
};

enum EYHonorIntervals
{
    EY_NORMAL_HONOR_INTERVAL        = 260,
    EY_WEEKEND_HONOR_INTERVAL       = 160
};

enum EYScore
{
    EY_WARNING_NEAR_VICTORY_SCORE       = 1400,
    EY_MAX_TEAM_SCORE                   = 1600
};

enum EYFlagState
{
    EY_FLAG_STATE_ON_BASE               = 0,
    EY_FLAG_STATE_WAIT_RESPAWN          = 1,
    EY_FLAG_STATE_ON_ALLIANCE_PLAYER    = 2,
    EY_FLAG_STATE_ON_HORDE_PLAYER       = 3,
    EY_FLAG_STATE_ON_GROUND             = 4
};

static const uint8 eyTickPoints[EY_MAX_NODES] = {1, 2, 5, 10};
static const uint32 eyFlagPoints[EY_MAX_NODES] = {75, 85, 100, 500};

static const uint32 eyTriggers[EY_MAX_NODES] = {AREATRIGGER_FEL_REAVER_RUINS_BUFF, AREATRIGGER_BLOOD_ELF_TOWER_BUFF, AREATRIGGER_DRAENEI_RUINS_BUFF, AREATRIGGER_MAGE_TOWER_BUFF};

struct EyeSummonData
{
    uint32 id;
    float x, y, z, o;
};

// Data used to summon spirit healers
// Note: the orientation has to be confirmed
static const EyeSummonData eyeGraveyardData[] =
{
    {GRAVEYARD_FEL_REAVER_RUINS, 2013.06f, 1677.24f, 1182.13f, 3.14159f},
    {GRAVEYARD_BLOOD_ELF_TOWER,  2012.4f,  1455.41f, 1172.2f,  3.14159f},
    {GRAVEYARD_DRAENEI_RUINS,    2351.78f, 1455.4f,  1185.33f, 3.14159f},
    {GRAVEYARD_MAGE_TOWER,       2355.3f,  1683.71f, 1173.15f, 3.14159f},
};

// Data used to summon honorable defenders
static const EyeSummonData eyeTowerdData[] =
{
    {NODE_FEL_REAVER_RUINS, 2024.6f,  1742.82f, 1195.16f, 0},
    {NODE_BLOOD_ELF_TOWER,  2050.49f, 1372.24f, 1194.56f, 0},
    {NODE_DRAENEI_RUINS,    2301.01f, 1386.93f, 1197.18f, 0},
    {NODE_MAGE_TOWER,       2282.12f, 1760.01f, 1189.71f, 0},
};

struct EYTowerEvent
{
    uint32  eventEntry;
    Team    team;
    uint32  message;
    uint32  worldState;
};

static const EYTowerEvent eyTowerEvents[EY_MAX_NODES][4] =
{
    {
        {EVENT_FEL_REAVER_RUINS_PROGRESS_ALLIANCE,  ALLIANCE,   LANG_BG_EY_HAS_TAKEN_A_B_TOWER, WORLD_STATE_EY_FEL_REAVER_RUINS_ALLIANCE},
        {EVENT_FEL_REAVER_RUINS_PROGRESS_HORDE,     HORDE,      LANG_BG_EY_HAS_TAKEN_H_F_RUINS, WORLD_STATE_EY_FEL_REAVER_RUINS_HORDE},
        {EVENT_FEL_REAVER_RUINS_NEUTRAL_HORDE,      TEAM_NONE,  LANG_BG_EY_HAS_LOST_A_F_RUINS,  WORLD_STATE_EY_FEL_REAVER_RUINS_NEUTRAL},
        {EVENT_FEL_REAVER_RUINS_NEUTRAL_ALLIANCE,   TEAM_NONE,  LANG_BG_EY_HAS_LOST_H_F_RUINS,  WORLD_STATE_EY_FEL_REAVER_RUINS_NEUTRAL},
    },
    {
        {EVENT_BLOOD_ELF_TOWER_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_BG_EY_HAS_TAKEN_A_B_TOWER, WORLD_STATE_EY_BLOOD_ELF_TOWER_ALLIANCE},
        {EVENT_BLOOD_ELF_TOWER_PROGRESS_HORDE,      HORDE,      LANG_BG_EY_HAS_TAKEN_H_B_TOWER, WORLD_STATE_EY_BLOOD_ELF_TOWER_HORDE},
        {EVENT_BLOOD_ELF_TOWER_NEUTRAL_HORDE,       TEAM_NONE,  LANG_BG_EY_HAS_LOST_A_B_TOWER,  WORLD_STATE_EY_BLOOD_ELF_TOWER_NEUTRAL},
        {EVENT_BLOOD_ELF_TOWER_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_BG_EY_HAS_LOST_H_B_TOWER,  WORLD_STATE_EY_BLOOD_ELF_TOWER_NEUTRAL},
    },
    {
        {EVENT_DRAENEI_RUINS_PROGRESS_ALLIANCE,     ALLIANCE,   LANG_BG_EY_HAS_TAKEN_A_D_RUINS, WORLD_STATE_EY_DRAENEI_RUINS_ALLIANCE},
        {EVENT_DRAENEI_RUINS_PROGRESS_HORDE,        HORDE,      LANG_BG_EY_HAS_TAKEN_H_D_RUINS, WORLD_STATE_EY_DRAENEI_RUINS_HORDE},
        {EVENT_DRAENEI_RUINS_NEUTRAL_HORDE,         TEAM_NONE,  LANG_BG_EY_HAS_LOST_A_D_RUINS,  WORLD_STATE_EY_DRAENEI_RUINS_NEUTRAL},
        {EVENT_DRAENEI_RUINS_NEUTRAL_ALLIANCE,      TEAM_NONE,  LANG_BG_EY_HAS_LOST_H_D_RUINS,  WORLD_STATE_EY_DRAENEI_RUINS_NEUTRAL},
    },
    {
        {EVENT_MAGE_TOWER_PROGRESS_ALLIANCE,        ALLIANCE,   LANG_BG_EY_HAS_TAKEN_A_M_TOWER, WORLD_STATE_EY_MAGE_TOWER_ALLIANCE},
        {EVENT_MAGE_TOWER_PROGRESS_HORDE,           HORDE,      LANG_BG_EY_HAS_TAKEN_H_M_TOWER, WORLD_STATE_EY_MAGE_TOWER_HORDE},
        {EVENT_MAGE_TOWER_NEUTRAL_HORDE,            TEAM_NONE,  LANG_BG_EY_HAS_LOST_A_M_TOWER,  WORLD_STATE_EY_MAGE_TOWER_NEUTRAL},
        {EVENT_MAGE_TOWER_NEUTRAL_ALLIANCE,         TEAM_NONE,  LANG_BG_EY_HAS_LOST_H_M_TOWER,  WORLD_STATE_EY_MAGE_TOWER_NEUTRAL},
    },
};

static const uint32 eyTowers[EY_MAX_NODES] = {GO_CAPTURE_POINT_FEL_REAVER_RUINS, GO_CAPTURE_POINT_BLOOD_ELF_TOWER, GO_CAPTURE_POINT_DRAENEI_RUINS, GO_CAPTURE_POINT_MAGE_TOWER};

class BattleGroundEYScore : public BattleGroundScore
{
    public:
        BattleGroundEYScore() : flagCaptures(0) {};
        virtual ~BattleGroundEYScore() {};

        uint32 flagCaptures;
};

class BattleGroundEY : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundEY();
        void Reset() override;
        void Update(uint32 diff) override;

        // Main battleground functions
        void AddPlayer(Player* player) override;
        void RemovePlayer(Player* player, ObjectGuid guid) override;
        void StartingEventOpenDoors() override;
        void EndBattleGround(Team winner) override;

        // General functions
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;
        Team GetPrematureWinner() override;

        // Battleground event handlers
        bool HandleEvent(uint32 eventId, Object* source, Object* target) override;
        bool HandleAreaTrigger(Player* source, uint32 trigger) override;
        void HandleGameObjectCreate(GameObject* go) override;
        void HandleKillPlayer(Player* player, Player* killer) override;
        void HandlePlayerClickedOnFlag(Player* player, GameObject* go) override;
        void HandlePlayerDroppedFlag(Player* source) override;

        // Flag handler
        ObjectGuid const& GetFlagCarrierGuid() const { return m_flagCarrier; }

        // Achievements
        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscvalue1) override;

    private:
        // Battleground flag functions
        void SetFlagCarrier(ObjectGuid guid) { m_flagCarrier = guid; }
        void ClearFlagCarrier() { m_flagCarrier.Clear(); }

        bool IsFlagPickedUp() const { return !m_flagCarrier.IsEmpty(); }
        uint8 GetFlagState() const { return m_flagState; }

        void RespawnFlagAtCenter(bool wasCaptured);
        void RespawnDroppedFlag();

        // process capture events
        void ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState, uint32 message);
        void ProcessPlayerFlagScoreEvent(Player* source, EYNodes node);
        bool AreAllNodesControlledByTeam(Team team);

        // Process score and resources
        void CheckVictory(Team team);
        void UpdateResources();
        void AddPoints(Team team, uint32 points);
        void SetFlagState(EYFlagState state);
        void SetTowerOwner(EYNodes node, Team team);

        EYFlagState m_flagState;
        ObjectGuid m_flagCarrier;
        ObjectGuid m_droppedFlagGuid;
        uint32 m_mainFlagDbGuid;

        Team m_towerOwner[EY_MAX_NODES];
        ObjectGuid m_towers[EY_MAX_NODES];
        ObjectGuid m_spiritHealers[EY_MAX_NODES];
        ObjectGuid m_honorableDefender[EY_MAX_NODES];

        uint32 m_honorTicks;
        uint32 m_honorScoreTicks[PVP_TEAM_COUNT];

        uint32 m_flagRespawnTimer;
        uint32 m_resourceUpdateTimer;
        uint32 m_felReaverFlagTimer;
};
#endif
