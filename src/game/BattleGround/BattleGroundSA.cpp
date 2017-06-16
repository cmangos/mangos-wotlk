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

#include "Entities/Player.h"
#include "Entities/GameObject.h"
#include "BattleGround.h"
#include "BattleGroundSA.h"
#include "Tools/Language.h"

BattleGroundSA::BattleGroundSA()
{
    // TODO FIX ME!
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}

void BattleGroundSA::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() == STATUS_WAIT_JOIN)
    {
        if (m_boatStartTimer)
        {
            if (m_boatStartTimer <= diff)
            {
                // Set the boats in motion
                for (GuidList::const_iterator itr = m_transportShipGuids[m_attackingTeamIdx].begin(); itr != m_transportShipGuids[m_attackingTeamIdx].end(); ++itr)
                {
                    if (GameObject* pShip = GetBgMap()->GetGameObject(*itr))
                    {
                        pShip->SetRespawnTime(30);
                        pShip->Refresh();
                        pShip->SetUInt32Value(GAMEOBJECT_LEVEL, 0);
                        pShip->SetGoState(GO_STATE_READY);
                    }
                }

                // ToDo: stop the boats when they reach the destination. Should be handled by event id 22095 and 18829
                m_boatStartTimer = 0;
            }
            else
                m_boatStartTimer -= diff;
        }
    }
    else if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // battle timer
        if (m_battleRoundTimer)
        {
            if (m_battleRoundTimer < diff)
            {
                // ToDo:
                m_battleRoundTimer = 0;
            }
            else
            {
                // update timer - if BattlegroundMgr update timer interval needs to be lowered replace this line with the commented-out ones below
                UpdateTimerWorldState();

                /*if (m_zoneUpdateTimer < diff)
                {
                    // update timer
                    UpdateTimerWorldState();
                    m_zoneUpdateTimer = BG_SA_TIMER_UPDATE_TIME;
                }
                else
                    m_zoneUpdateTimer -= diff;*/

                m_battleRoundTimer -= diff;
            }
        }
    }
}

void BattleGroundSA::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundSAScore* sc = new BattleGroundSAScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    // add phase aura
    plr->CastSpell(plr, m_attackingTeamIdx == TEAM_INDEX_ALLIANCE ? BG_SA_SPELL_HORDE_CONTROL_PHASE_SHIFT : BG_SA_SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, TRIGGERED_OLD_TRIGGERED);

    // Teleport player to the correct location
    if (GetTeamIndexByTeamId(plr->GetTeam()) == m_attackingTeamIdx)
    {
        // TODO: make this location more dyanmic, depending on the transport position
        plr->CastSpell(plr, BG_SA_SPELL_TELEPORT_ATTACKERS, TRIGGERED_OLD_TRIGGERED);

        // Note: the following code is temporary until spell effect 60178 is implemented
        uint8 randLoc = (urand(0, 1));
        plr->TeleportTo(plr->GetMapId(), strandTeleportLoc[randLoc][0], strandTeleportLoc[randLoc][1], strandTeleportLoc[randLoc][2], strandTeleportLoc[randLoc][3]);
    }
    else
        plr->CastSpell(plr, BG_SA_SPELL_TELEPORT_DEFENDER, TRIGGERED_OLD_TRIGGERED);
}

void BattleGroundSA::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);

    UpdateWorldState(BG_SA_STATE_ENABLE_TIMER, 1);
}

void BattleGroundSA::UpdatePlayerScore(Player* source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(source->GetObjectGuid());
    if (itr == m_PlayerScores.end())
        return;

    BattleGround::UpdatePlayerScore(source, type, value);
}

void BattleGroundSA::HandleCreatureCreate(Creature* creature)
{
    // ToDo handle vehicles
}

void BattleGroundSA::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_1:
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_2:
            if (m_attackingTeamIdx == TEAM_INDEX_HORDE)
                m_transportShipGuids[TEAM_INDEX_HORDE].push_back(go->GetObjectGuid());
            break;
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_1:
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_2:
            if (m_attackingTeamIdx == TEAM_INDEX_ALLIANCE)
                m_transportShipGuids[TEAM_INDEX_ALLIANCE].push_back(go->GetObjectGuid());
            break;
    }
}

// Function used to update the timer
void BattleGroundSA::UpdateTimerWorldState()
{
    // Calculate time
    uint32 secondsLeft = m_battleRoundTimer;

    UpdateWorldState(BG_SA_STATE_TIMER_SEC_FIRST_DIGIT, ((secondsLeft % 60000) % 10000) / 1000);
    UpdateWorldState(BG_SA_STATE_TIMER_SEC_SECOND_DIGIT, (secondsLeft % 60000) / 10000);
    UpdateWorldState(BG_SA_STATE_TIMER_MINUTES, secondsLeft / 60000);
}

void BattleGroundSA::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    // fill attacker & defender states
    FillInitialWorldState(data, count, BG_SA_STATE_ATTACKER_ALLIANCE, m_attackingTeamIdx == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_ATTACKER_HORDE, m_attackingTeamIdx == TEAM_INDEX_HORDE);

    FillInitialWorldState(data, count, BG_SA_STATE_DEFENSE_TOKEN_HORDE, m_attackingTeamIdx == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE, m_attackingTeamIdx == TEAM_INDEX_HORDE);

    FillInitialWorldState(data, count, BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE, m_attackingTeamIdx == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE, m_attackingTeamIdx == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE, m_attackingTeamIdx == TEAM_INDEX_HORDE);
    FillInitialWorldState(data, count, BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE, m_attackingTeamIdx == TEAM_INDEX_HORDE);

    // fill gates states
    for (uint8 i = 0; i < BG_SA_MAX_GATES; ++i)
    {
        // special case for final gate: color depends on the defender (blue for alliance; red for horde)
        if (i == BG_SA_MAX_GATES - 1 && m_attackingTeamIdx == TEAM_INDEX_ALLIANCE)
            FillInitialWorldState(data, count, strandGates[i], m_gateStateValue[i] + 3);
        else
            FillInitialWorldState(data, count, strandGates[i], m_gateStateValue[i]);
    }

    // fill timer states - will be updated later in the script
    FillInitialWorldState(data, count, BG_SA_STATE_ENABLE_TIMER, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_MINUTES, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_SEC_SECOND_DIGIT, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_SEC_FIRST_DIGIT, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_BONUS_TIMER, 0);

    // fill graveyard states
    // ToDo:
}

void BattleGroundSA::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    m_attackingTeamIdx = urand(0, 1) ? TEAM_INDEX_ALLIANCE : TEAM_INDEX_HORDE;

    m_boatStartTimer = BG_SA_TIMER_BOAT_START;
    m_battleRoundTimer = BG_SA_TIMER_ROUND_LENGTH;

    // set initial gate state values
    for (uint8 i = 0; i < BG_SA_MAX_GATES; ++i)
        m_gateStateValue[i] = BG_SA_STATE_VALUE_GATE_INTACT;
}
