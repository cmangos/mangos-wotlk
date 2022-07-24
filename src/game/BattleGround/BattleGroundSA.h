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

#ifndef __BATTLEGROUNDSA_H
#define __BATTLEGROUNDSA_H

#include "Common.h"
#include "BattleGround.h"
#include "Tools/Language.h"

class BattleGround;

enum SAGlobalVariables
{
    BG_SA_MAX_GATES                             = 6,
    BG_SA_MAX_SIGILS                            = 5,
    BG_SA_MAX_GRAVEYARDS                        = 3,                // max capturable graveyards
    BG_SA_MAX_DEFENSE_LINES                     = 4,                // there are 3 defense lines and 1 relic that count for the bonus honor

    BG_SA_ZONE_ID_STRAND                        = 4384,
    // BG_SA_ZONE_ID_COURTYARD_ANCIENTS         = 4609,

    // world state values
    BG_SA_STATE_VALUE_GATE_INTACT               = 1,
    BG_SA_STATE_VALUE_GATE_DAMAGED              = 2,
    BG_SA_STATE_VALUE_GATE_DESTROYED            = 3,

    // timers
    BG_SA_TIMER_ROUND_LENGTH                    = 10 * MINUTE * IN_MILLISECONDS,
    //BG_SA_TIMER_UPDATE_TIME                     = 1000,
    BG_SA_TIMER_BOAT_START                      = 60000,
};

enum SAWorldStates
{
    // world states
    BG_SA_STATE_TIMER_MINUTES                   = 3559,
    BG_SA_STATE_TIMER_SEC_SECOND_DIGIT          = 3560,             // tens
    BG_SA_STATE_TIMER_SEC_FIRST_DIGIT           = 3561,             // units
    BG_SA_STATE_ENABLE_TIMER                    = 3564,
    BG_SA_STATE_BONUS_TIMER                     = 3571,

    BG_SA_STATE_ATTACKER_ALLIANCE               = 4352,
    BG_SA_STATE_ATTACKER_HORDE                  = 4353,

    // gates
    BG_SA_STATE_PURPLE_GATE                     = 3614,
    BG_SA_STATE_RED_GATE                        = 3617,
    BG_SA_STATE_BLUE_GATE                       = 3620,
    BG_SA_STATE_GREEN_GATE                      = 3623,
    BG_SA_STATE_YELLOW_GATE                     = 3638,
    BG_SA_STATE_ANCIENT_GATE                    = 3849,

    // graveyard owner
    BG_SA_STATE_GY_LEFT_ALLIANCE                = 3635,                 // west graveyard state
    BG_SA_STATE_GY_RIGHT_ALLIANCE               = 3636,                 // east graveyard state
    BG_SA_STATE_GY_CENTER_ALLIANCE              = 3637,                 // south graveyard state

    BG_SA_STATE_GY_LEFT_HORDE                   = 3633,                 // west graveyard state
    BG_SA_STATE_GY_RIGHT_HORDE                  = 3632,                 // east graveyard state
    BG_SA_STATE_GY_CENTER_HORDE                 = 3634,                 // south graveyard state

    // attacker / defender map tokens
    BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE     = 3627,
    BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE      = 3626,

    BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE        = 3628,
    BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE         = 3629,

    BG_SA_STATE_DEFENSE_TOKEN_HORDE             = 3631,
    BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE          = 3630,
};

enum SACreatures
{
    // creatures
    BG_SA_VEHICLE_DEMOLISHER                    = 28781,
    BG_SA_VEHICLE_CANNON                        = 27894,

    BG_SA_NPC_KANRETHAD                         = 29,
    BG_SA_NPC_INVISIBLE_STALKER                 = 15214,                // related to the start locations of the teams; possible target for teleport?
    BG_SA_NPC_WORLD_TRIGGER                     = 22515,                // emote handler
    // BG_SA_NPC_WORLD_TRIGGER_LARGE            = 23472,                // gate teleport target

    // workshop owners - used when the attacker takes over the east / west graveyard
    BG_SA_NPC_RIGGER_SPARKLIGHT                 = 29260,
    BG_SA_NPC_GORGRIL_RIGSPARK                  = 29262,
};

enum SAObjects
{
    // gameobjects
    // Main gates
    BG_SA_GO_GATE_GREEN_EMERALD                 = 190722,
    BG_SA_GO_GATE_PURPLE_AMETHYST               = 190723,
    BG_SA_GO_GATE_BLUE_SAPHIRE                  = 190724,
    BG_SA_GO_GATE_RED_SUN                       = 190726,
    BG_SA_GO_GATE_YELLOW_MOON                   = 190727,
    BG_SA_GO_GATE_ANCIENT_SHRINE                = 192549,

    // Relics - main objective of the battleground (has same faction as the attackers)
    BG_SA_GO_TITAN_RELIC_HORDE                  = 194082,               // phased for attacker
    BG_SA_GO_TITAN_RELIC_ALLIANCE               = 194083,

    // Sigils - used for decoration purpose
    BG_SA_GO_SIGIL_YELLOW_MOON                  = 192685,
    BG_SA_GO_SIGIL_GREEN_MOON                   = 192687,
    BG_SA_GO_SIGIL_BLUE_MOON                    = 192689,
    BG_SA_GO_SIGIL_RED_MOON                     = 192690,
    BG_SA_GO_SIGIL_PURPLE_MOON                  = 192691,

    // various objects (have same faction as the defenders)
    BG_SA_GO_DEFENDERS_PORTAL_ALLIANCE          = 191575,               // phased for defender
    BG_SA_GO_DEFENDERS_PORTAL_HORDE             = 190763,

    BG_SA_GO_SEAFORIUM_BOMB_ALLIANCE            = 190753,               // phased for attacker
    BG_SA_GO_SEAFORIUM_BOMB_HORDE               = 194086,
    BG_SA_GO_SEAFORIUM_BOMB                     = 190752,               // actual bomb - trap triggering spell 52408
};

enum SABanners
{
    // graveyard flags - type buttons
    BG_SA_GO_GY_FLAG_ALLIANCE_EAST              = 191306,
    BG_SA_GO_GY_FLAG_ALLIANCE_WEST              = 191308,
    BG_SA_GO_GY_FLAG_ALLIANCE_SOUTH             = 191310,

    BG_SA_GO_GY_FLAG_HORDE_EAST                 = 191305,
    BG_SA_GO_GY_FLAG_HORDE_WEST                 = 191307,
    BG_SA_GO_GY_FLAG_HORDE_SOUTH                = 191309,
};

enum SAShips
{
    // transport ships - phased for attacker
    BG_SA_GO_TRANSPORT_SHIP_HORDE_1             = 193184,
    BG_SA_GO_TRANSPORT_SHIP_HORDE_2             = 193183,
    BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_1          = 193182,
    BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_2          = 193185,
};

enum SAGraveyards
{
    // graveyard links
    BG_SA_GRAVEYARD_ID_EAST                     = 1346,                 // controllable graveyards
    BG_SA_GRAVEYARD_ID_WEST                     = 1347,
    BG_SA_GRAVEYARD_ID_SOUTH                    = 1348,
    BG_SA_GRAVEYARD_ID_SHRINE                   = 1349,                 // defender graveyard
    BG_SA_GRAVEYARD_ID_BEACH                    = 1350,                 // attacker graveyards
    BG_SA_GRAVEYARD_ID_SHIP                     = 1351,
};

enum SASpells
{
    // spells
    BG_SA_SPELL_TELEPORT_DEFENDER               = 52364,                // teleport defender team to the south graveyard platform
    // BG_SA_SPELL_TELEPORT_ATTACKERS           = 60178,                // unk or not used
    BG_SA_SPELL_END_OF_ROUND                    = 52459,                // stun the players at the end of the round; after 5 seconds, teleport and reset for round 2
    BG_SA_SPELL_REMOVE_SEAFORIUM                = 59077,
    BG_SA_SPELL_SEAFORIUM_BLAST                 = 52408,
    BG_SA_SPELL_CARRY_SEAFORIUM                 = 52418,

    // spells that will split teleport attacker team to each boat
    BG_SA_SPELL_SPLIT_TELEPORT_H_BOAT1          = 52365,                // teleport to boat 193184
    BG_SA_SPELL_SPLIT_TELEPORT_H_BOAT2          = 52528,                // teleport to boat 193183
    BG_SA_SPELL_SPLIT_TELEPORT_A_BOAT1          = 53464,
    BG_SA_SPELL_SPLIT_TELEPORT_A_BOAT2          = 53465,

    // phasing auras
    BG_SA_SPELL_ALLIANCE_CONTROL_PHASE_SHIFT    = 60027,                // phase 65 - alliance is defender
    BG_SA_SPELL_HORDE_CONTROL_PHASE_SHIFT       = 60028,                // phase 129 - horde is defender
};

enum SAAchievCriteria
{
    // achiev criterias
    BG_SA_CRIT_NOT_A_SCRATCH_ALLY               = 7626,                 // achiev id 1762; criteria type 1
    BG_SA_CRIT_NOT_A_SCRATCH_HORDE              = 7634,                 // achiev id 2192; criteria type 1
    BG_SA_CRIT_DROP_IT                          = 6446,                 // achiev id 1764; criteria type 70
    BG_SA_CRIT_DROP_IT_NOW                      = 7629,                 // achiev id 2190; criteria type 70; resets every battleground start if not completed
    // BG_SA_CRIT_ANCIENT_PROTECTOR             = 7630,                 // achiev id 1766; criteria type 70
    // BG_SA_CRIT_COURTYARD_PROTECTOR           = 7631,                 // achiev id 2191; criteria type 70; resets every battleground start if not completed
    BG_SA_CRIT_DEFENSE_ANCIENTS_ALLY            = 7636,                 // achiev id 1757; criteria type 28; spell id 52459 (end of round)
    BG_SA_CRIT_DEFENSE_ANCIENTS_HORDE           = 7740,                 // achiev id 2200; criteria type 28; spell id 52459 (end of round)
    BG_SA_CRIT_ARTILLERY_VETERAN                = 7625,                 // achiev id 1763; criteria type 0; creature id 28781
    BG_SA_CRIT_ARTILLERY_EXPERT                 = 7628,                 // achiev id 2189; criteria type 0; creature id 28781; resets every battleground start if not completed
    // BG_SA_CRIT_DAPPER_SAPPER                 = 7632,                 // achiev id 1761; criteria type 28; spell id 60937
    // BG_SA_CRIT_EXPLOSIVES_EXPERT             = 7635,                 // achiev id 1761; criteria type 28; spell id 60937; resets every battleground start if not completed

    BG_SA_ACHIEV_START_ID_STORM_BEACH_ALLY      = 23748,                // achiev id 1310; criteria tyep 28; spell id 65246
    BG_SA_ACHIEV_START_ID_STORM_BEACH_HORDE     = 21702,

    // missing achiev criteria spells
    BG_SA_SPELL_ACHIEV_STORM_BEACH              = 65246,                // achiev id 1310 | serverside spell
    BG_SA_SPELL_ACHIEV_SEAFORIUM_DAMAGE         = 60937,                // achiev ids 1761, 2193 | serverside spell
};

enum SAEvents
{
    // events
    BG_SA_EVENT_ID_RELIC                        = 20572,                // event used to end the round
    BG_SA_EVENT_ID_SHIP_PAUSE_1                 = 22095,                // events used to pause the ships
    BG_SA_EVENT_ID_SHIP_PAUSE_2                 = 18829,
    BG_SA_EVENT_SHRINE_DOOR_DESTROY             = 19837,
};

enum SASounds
{
    // sounds
    BG_SA_SOUND_GRAVEYARD_TAKEN_HORDE           = 8174,
    BG_SA_SOUND_GRAVEYARD_TAKEN_ALLIANCE        = 8212,
    BG_SA_SOUND_DEFEAT_HORDE                    = 15905,
    BG_SA_SOUND_VICTORY_HORDE                   = 15906,
    BG_SA_SOUND_VICTORY_ALLIANCE                = 15907,
    BG_SA_SOUND_DEFEAT_ALLIANCE                 = 15908,
    BG_SA_SOUND_WALL_DESTROYED_ALLIANCE         = 15909,
    BG_SA_SOUND_WALL_DESTROYED_HORDE            = 15910,
    BG_SA_SOUND_WALL_ATTACKED_HORDE             = 15911,
    BG_SA_SOUND_WALL_ATTACKED_ALLIANCE          = 15912,
};

enum SAFactions
{
    // factions
    BG_SA_FACTION_ID_ALLIANCE                   = 1732,
    BG_SA_FACTION_ID_HORDE                      = 1735,
};

enum SABattleStages
{
    // battle stages
    BG_SA_STAGE_ROUND_1                         = 0,
    BG_SA_STAGE_RESET                           = 1,
    BG_SA_STAGE_SECOND_ROUND_1                  = 2,
    BG_SA_STAGE_SECOND_ROUND_2                  = 3,
    BG_SA_STAGE_SECOND_ROUND_3                  = 4,
    BG_SA_STAGE_ROUND_2                         = 5,
};

enum SAConditions
{
    // conditions
    BG_SA_COND_DEFENDER_ALLIANCE                = 0,
    BG_SA_COND_DEFENDER_HORDE                   = 1,
};

static const uint32 strandGates[BG_SA_MAX_GATES] = { BG_SA_STATE_GREEN_GATE, BG_SA_STATE_PURPLE_GATE, BG_SA_STATE_BLUE_GATE, BG_SA_STATE_RED_GATE, BG_SA_STATE_YELLOW_GATE, BG_SA_STATE_ANCIENT_GATE };

static const uint32 strandSigils[BG_SA_MAX_SIGILS] = { BG_SA_GO_SIGIL_GREEN_MOON, BG_SA_GO_SIGIL_PURPLE_MOON, BG_SA_GO_SIGIL_BLUE_MOON, BG_SA_GO_SIGIL_RED_MOON, BG_SA_GO_SIGIL_YELLOW_MOON };

// *** Battleground factions *** //
const uint32 sotaTeamFactions[PVP_TEAM_COUNT] = { BG_SA_FACTION_ID_ALLIANCE, BG_SA_FACTION_ID_HORDE };

struct StrandSummonData
{
    uint32 entry;
    float x, y, z, o;
};

// *** Battleground spawn data *** //
static const StrandSummonData sotaEastSpawns[] =
{
    {BG_SA_NPC_RIGGER_SPARKLIGHT, 1348.644f, -298.7865f, 31.0801f, 1.7104f},
    {BG_SA_VEHICLE_DEMOLISHER, 1370.013f, -313.8679f, 34.8538f, 1.9547f},
    {BG_SA_VEHICLE_DEMOLISHER, 1361.173f, -290.2466f, 30.8998f, 1.8684f}
};

static const StrandSummonData sotaWestSpawns[] =
{
    {BG_SA_NPC_GORGRIL_RIGSPARK, 1358.191f, 195.5278f, 31.0181f, 4.1713f},
    {BG_SA_VEHICLE_DEMOLISHER, 1353.148f, 223.7602f, 35.2643f, 4.4029f},
    {BG_SA_VEHICLE_DEMOLISHER, 1343.434f, 196.6284f, 30.9863f, 4.2935f}
};

struct StrandGoData
{
    uint32 goEntry, worldState, eventDamaged, eventDestroyed, eventRebuild, messageDamaged, messagedDestroyed, index;
};

// *** Battleground object data *** //
static const StrandGoData sotaObjectData[] =
{
    {BG_SA_GO_GATE_GREEN_EMERALD,   BG_SA_STATE_GREEN_GATE,     19041,  19046,  21630,  LANG_BG_SA_GATE_GREEN_ATTACK,   LANG_BG_SA_GATE_GREEN_DESTROY,  0},
    {BG_SA_GO_GATE_PURPLE_AMETHYST, BG_SA_STATE_PURPLE_GATE,    19043,  19048,  21630,  LANG_BG_SA_GATE_PURPLE_ATTACK,  LANG_BG_SA_GATE_PURPLE_DESTROY, 1},
    {BG_SA_GO_GATE_BLUE_SAPHIRE,    BG_SA_STATE_BLUE_GATE,      19040,  19045,  21630,  LANG_BG_SA_GATE_BLUE_ATTACK,    LANG_BG_SA_GATE_BLUE_DESTROY,   2},
    {BG_SA_GO_GATE_RED_SUN,         BG_SA_STATE_RED_GATE,       19042,  19047,  21630,  LANG_BG_SA_GATE_RED_ATTACK,     LANG_BG_SA_GATE_RED_DESTROY,    3},
    {BG_SA_GO_GATE_YELLOW_MOON,     BG_SA_STATE_YELLOW_GATE,    19044,  19049,  21630,  LANG_BG_SA_GATE_YELLOW_ATTACK,  LANG_BG_SA_GATE_YELLOW_DESTROY, 4},
    {BG_SA_GO_GATE_ANCIENT_SHRINE,  BG_SA_STATE_ANCIENT_GATE,   19836,  19837,  21630,  LANG_BG_SA_CHAMBER_ATTACK,      LANG_BG_SA_CHAMBER_BREACH,      5},
};

struct StrandGraveyardData
{
    uint32 graveyardId, worldStateAlliance, worldStateHorde, goEntryAlliance, goEntryHorde, textCaptureAlliance, textCaptureHorde;
};

// *** Battleground graveyard data *** //
static const StrandGraveyardData sotaGraveyardData[] =
{
    {BG_SA_GRAVEYARD_ID_EAST,   BG_SA_STATE_GY_RIGHT_ALLIANCE,  BG_SA_STATE_GY_RIGHT_HORDE,   BG_SA_GO_GY_FLAG_ALLIANCE_EAST,   BG_SA_GO_GY_FLAG_HORDE_EAST,   LANG_BG_SA_GRAVE_EAST_ALLIANCE,  LANG_BG_SA_GRAVE_EAST_HORDE},
    {BG_SA_GRAVEYARD_ID_WEST,   BG_SA_STATE_GY_LEFT_ALLIANCE,   BG_SA_STATE_GY_LEFT_HORDE,    BG_SA_GO_GY_FLAG_ALLIANCE_WEST,   BG_SA_GO_GY_FLAG_HORDE_WEST,   LANG_BG_SA_GRAVE_WEST_ALLIANCE,  LANG_BG_SA_GRAVE_WEST_HORDE},
    {BG_SA_GRAVEYARD_ID_SOUTH,  BG_SA_STATE_GY_CENTER_ALLIANCE, BG_SA_STATE_GY_CENTER_HORDE,  BG_SA_GO_GY_FLAG_ALLIANCE_SOUTH,  BG_SA_GO_GY_FLAG_HORDE_SOUTH,  LANG_BG_SA_GRAVE_SOUTH_ALLIANCE, LANG_BG_SA_GRAVE_SOUTH_HORDE},
};

struct StrandGraveyardSpawnData
{
    uint32 graveyardId;
    float x, y, z, o;
};

static const StrandGraveyardSpawnData strandGraveyardData[] =
{
    {BG_SA_GRAVEYARD_ID_EAST,   1400.12f, -293.852f, 32.07f,  1.888f},
    {BG_SA_GRAVEYARD_ID_WEST,   1388.8f,   203.355f, 32.153f, 4.488f},
    {BG_SA_GRAVEYARD_ID_SOUTH,  1122.28f,    4.416f, 68.936f, 6.063f},
};

struct StrandGraveyardNode
{
    // banner entries and timers
    uint32 oldEntry;
    uint32 newEntry;
    uint32 changeTimer;

    // owner and spirit healer
    PvpTeamIndex graveyardOwner;
    ObjectGuid spiritHealerGuid;
};

class BattleGroundSAScore : public BattleGroundScore
{
    public:
        BattleGroundSAScore(): demolishersDestroyed(0), gatesDestroyed(0) {};
        virtual ~BattleGroundSAScore() {};

        uint32 GetAttr1() const { return demolishersDestroyed; }
        uint32 GetAttr2() const { return gatesDestroyed; }

        uint32 demolishersDestroyed;
        uint32 gatesDestroyed;
};

class BattleGroundSA : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundSA();
        void Reset() override;
        void Update(uint32 diff) override;

        // Main battleground functions
        void AddPlayer(Player* player) override;
        void StartingEventOpenDoors() override;
        void EndBattleGround(Team winner) override;

        // General functions
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

        // Battleground event handlers
        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;
        void HandlePlayerClickedOnFlag(Player* player, GameObject* go) override;
        bool HandleEvent(uint32 eventId, Object* source, Object* target) override;
        void HandleKillUnit(Creature* unit, Player* killer) override;
        void HandleGameObjectDamaged(Player* player, GameObject* object, uint32 spellId) override;

        // Conditions
        bool IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType) override;

        // Achievements
        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1) override;

        void AlterTeleportLocation(Player* player, ObjectGuid& transportGuid, float& x, float& y, float& z, float& ori) override;

    private:
        // Helper functions
        void UpdateTimerWorldState();
        void ProcessBattlegroundWinner();
        void SendBattlegroundWarning(int32 messageId);
        void ChangeBannerState(uint8 nodeId);
        void AwardBonusHonor();

        // Battleground setup functions
        void SetupBattleground(bool initialSetup);
        void EnableDemolishers();
        void SetupGraveyards();

        void TeleportPlayerToStartArea(Player* player);

        PvpTeamIndex GetAttacker() { return m_defendingTeamIdx == TEAM_INDEX_ALLIANCE ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE; }
        void SetDefender(PvpTeamIndex teamIndex);
        PvpTeamIndex m_defendingTeamIdx;

        StrandGraveyardNode m_strandGraveyard[BG_SA_MAX_GRAVEYARDS];

        bool m_initialSetup;
        bool m_noScratchAchiev;                     // no demolisher is destroyed
        bool m_defenseAncients;                     // no wall is destroyed
        bool m_defenseLineCaptured[BG_SA_MAX_DEFENSE_LINES];

        uint32 m_gateStateValue[BG_SA_MAX_GATES];
        uint32 m_winTime[PVP_TEAM_COUNT];
        uint32 m_battleRoundTimer;
        uint32 m_boatStartTimer;
        uint8 m_scoreCount[PVP_TEAM_COUNT];
        uint8 m_battleStage;

        // single spawn guid storages can be normal guids because they get overwritten on respawn
        ObjectGuid m_battlegroundMasterGuid;
        uint32 m_defenderTeleportStalkerGuid;
        ObjectGuid m_riggerGuid;
        ObjectGuid m_gorgrilGuid;
        ObjectGuid m_relicGuid[PVP_TEAM_COUNT];

        // guid sets need to be dbguid centric because dbguid does not change across respawns for static spawns
        std::set<uint32> m_cannonsGuids;
        std::set<uint32> m_demolishersGuids;
        GuidList m_tempDemolishersGuids; // these use normal guids because they are tempspawns
        std::set<uint32> m_gatesGuids;
        std::set<uint32> m_attackerTeleportStalkersGuids;
        std::set<uint32> m_triggerGuids;
        std::set<uint32> m_transportShipGuids[PVP_TEAM_COUNT];
};
#endif
