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

class BattleGround;

#define BG_SA_EVENT_START_BATTLE_1      23748       // Ally / Horde likely
#define BG_SA_EVENT_START_BATTLE_2      21702

enum
{
    BG_SA_MAX_GATES                             = 6,

    // world state values
    BG_SA_STATE_VALUE_GATE_INTACT               = 1,
    BG_SA_STATE_VALUE_GATE_DAMAGED              = 2,
    BG_SA_STATE_VALUE_GATE_DESTROYED            = 3,

    // timers
    BG_SA_TIMER_ROUND_LENGTH                    = 10 * MINUTE * IN_MILLISECONDS,
    //BG_SA_TIMER_UPDATE_TIME                     = 1000,
    BG_SA_TIMER_BOAT_START                      = 60000,

    // world states
    BG_SA_STATE_TIMER_MINUTES                   = 3559,
    BG_SA_STATE_TIMER_SEC_SECOND_DIGIT          = 3560,             // tens
    BG_SA_STATE_TIMER_SEC_FIRST_DIGIT           = 3561,             // units
    BG_SA_STATE_ENABLE_TIMER                    = 3564,
    BG_SA_STATE_BONUS_TIMER                     = 3571,

    BG_SA_STATE_ATTACKER_ALLIANCE               = 4352,
    BG_SA_STATE_ATTACKER_HORDE                  = 4353,

    BG_SA_STATE_PURPLE_GATE                     = 3614,
    BG_SA_STATE_RED_GATE                        = 3617,
    BG_SA_STATE_BLUE_GATE                       = 3620,
    BG_SA_STATE_GREEN_GATE                      = 3623,
    BG_SA_STATE_YELLOW_GATE                     = 3638,
    BG_SA_STATE_ANCIENT_GATE                    = 3849,

    BG_SA_STATE_GY_LEFT_ALLIANCE                = 3635,
    BG_SA_STATE_GY_RIGHT_ALLIANCE               = 3636,
    BG_SA_STATE_GY_CENTER_ALLIANCE              = 3637,

    BG_SA_STATE_GY_LEFT_HORDE                   = 3633,
    BG_SA_STATE_GY_RIGHT_HORDE                  = 3632,
    BG_SA_STATE_GY_CENTER_HORDE                 = 3634,

    BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE     = 3627,
    BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE      = 3626,

    BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE        = 3628,
    BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE         = 3629,

    BG_SA_STATE_DEFENSE_TOKEN_HORDE             = 3631,
    BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE          = 3630,

    // creatures
    BG_SA_VEHICLE_DEMOLISHER                    = 28781,
    BG_SA_VEHICLE_CANNON                        = 27894,

    // gameobjects
    // Main gates
    BG_SA_GO_GATE_GREEN_EMERALD                 = 190722,
    BG_SA_GO_GATE_MAUVE_AMETHYST                = 190723,
    BG_SA_GO_GATE_BLUE_SAPHIRE                  = 190724,
    BG_SA_GO_GATE_RED_SUN                       = 190726,
    BG_SA_GO_GATE_YELLOW_MOON                   = 190727,
    BG_SA_GO_GATE_ANCIENT_SHRINE                = 192549,

    // Relics - main objective of the battleground (has same faction as the attackers)
    BG_SA_GO_TITAN_RELIC_HORDE                   = 194082,
    BG_SA_GO_TITAN_RELIC_ALLIANCE                = 194083,

    // Sigils - used for decoration purpose
    BG_SA_GO_SIGIL_YELLOW_MOON                  = 192685,
    BG_SA_GO_SIGIL_GREEN_MOON                   = 192687,
    BG_SA_GO_SIGIL_BLUE_MOON                    = 192689,
    BG_SA_GO_SIGIL_RED_MOON                     = 192690,
    BG_SA_GO_SIGIL_PURPLE_MOON                  = 192691,

    // various objects (have same faction as the defenders)
    BG_SA_GO_DEFENDERS_PORTAL_ALLIANCE          = 191575,
    BG_SA_GO_DEFENDERS_PORTAL_HORDE             = 190763,
    BG_SA_GO_SEAFORIUM_BOMB_ALLIANCE            = 190753,
    BG_SA_GO_SEAFORIUM_BOMB_HORDE               = 194086,

    // graveyard flags
    BG_SA_GO_GY_FLAG_ALLIANCE_1                 = 191306,
    BG_SA_GO_GY_FLAG_ALLIANCE_2                 = 191308,
    BG_SA_GO_GY_FLAG_ALLIANCE_3                 = 191310,
    BG_SA_GO_GY_FLAG_HORDE_1                    = 191305,
    BG_SA_GO_GY_FLAG_HORDE_2                    = 191307,
    BG_SA_GO_GY_FLAG_HORDE_3                    = 191309,

    // transport ships
    BG_SA_GO_TRANSPORT_SHIP_HORDE_1             = 193183,
    BG_SA_GO_TRANSPORT_SHIP_HORDE_2             = 193184,
    BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_1          = 193182,
    BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_2          = 193185,

    // spells
    BG_SA_SPELL_TELEPORT_DEFENDER               = 52364,
    BG_SA_SPELL_TELEPORT_ATTACKERS              = 60178,
    BG_SA_SPELL_END_OF_ROUND                    = 52459,
    BG_SA_SPELL_REMOVE_SEAFORIUM                = 59077,
    BG_SA_SPELL_ALLIANCE_CONTROL_PHASE_SHIFT    = 60027,
    BG_SA_SPELL_HORDE_CONTROL_PHASE_SHIFT       = 60028,
};

static const uint32 strandGates[BG_SA_MAX_GATES] = { BG_SA_STATE_PURPLE_GATE, BG_SA_STATE_RED_GATE, BG_SA_STATE_BLUE_GATE, BG_SA_STATE_GREEN_GATE, BG_SA_STATE_YELLOW_GATE, BG_SA_STATE_ANCIENT_GATE };

static const float strandTeleportLoc[3][4] =
{
    {2682.936f, -830.368f, 19.0f, 2.895f},      // left side boat
    {2577.003f, 980.261f, 19.0f, 0.807f},       // right side boat
    {1209.7f, -65.16f, 70.1f, 0.0f}             // defender position (not used; player teleported by spell)
};

class BattleGroundSAScore : public BattleGroundScore
{
    public:
        BattleGroundSAScore() {};
        virtual ~BattleGroundSAScore() {};
};

class BattleGroundSA : public BattleGround
{
        friend class BattleGroundMgr;

    public:
        BattleGroundSA();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;
        virtual void StartingEventOpenDoors() override;
        virtual void FillInitialWorldStates(WorldPacket &d, uint32& count) override;
        virtual void Reset() override;

        void HandleCreatureCreate(Creature* creature) override;
        void HandleGameObjectCreate(GameObject* go) override;

        /* Scorekeeping */
        void UpdatePlayerScore(Player* source, uint32 type, uint32 value) override;

        void Update(uint32 diff) override;

    private:
        void UpdateTimerWorldState();

        PvpTeamIndex m_attackingTeamIdx;

        uint32 m_gateStateValue[BG_SA_MAX_GATES];
        uint32 m_battleRoundTimer;
        uint32 m_boatStartTimer;

        GuidList m_transportShipGuids[PVP_TEAM_COUNT];
};
#endif
