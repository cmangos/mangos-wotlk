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
#include "BattleGround.h"
#include "BattleGroundIC.h"
#include "Tools/Language.h"

BattleGroundIC::BattleGroundIC()
{

    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_IC_START_TWO_MINUTES;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_IC_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_IC_START_HALF_MINUTE;
    // ToDo: use the text LANG_BG_IC_START_15_SECONDS
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_IC_BEGIN;
}

void BattleGroundIC::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
        m_reinforcements[i] = BG_IC_MAX_REINFORCEMENTS;

    // setup the owner and state for all objectives
    for (uint8 i = 0; i < BG_IC_MAX_OBJECTIVES; ++i)
    {
        if (i == BG_IC_OBJECTIVE_KEEP_ALLY)
        {
            m_objectiveOwner[i] = TEAM_INDEX_ALLIANCE;
            m_objectiveState[i] = isleObjectData[i].stateAlly;
        }
        else if (i == BG_IC_OBJECTIVE_KEEP_HORDE)
        {
            m_objectiveOwner[i] = TEAM_INDEX_HORDE;
            m_objectiveState[i] = isleObjectData[i].stateHorde;
        }
        else
        {
            m_objectiveOwner[i] = TEAM_INDEX_NEUTRAL;
            m_objectiveState[i] = isleObjectData[i].stateNeutral;
        }
    }

    // setup the state for the keep walls
    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        m_gatesAllianceState[i] = isleAllianceWallsData[i].stateClosed;

    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        m_gatesHordeState[i] = isleHordeWallsData[i].stateClosed;
}

void BattleGroundIC::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundICScore* sc = new BattleGroundICScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;
}

void BattleGroundIC::StartingEventOpenDoors()
{
    // ToDo: open all gates and enable portals
}

void BattleGroundIC::UpdatePlayerScore(Player* source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(source->GetObjectGuid());

    if (itr == m_PlayerScores.end())                        // player not found...
        return;

    switch (type)
    {
        case SCORE_BASES_ASSAULTED:
            ((BattleGroundICScore*)itr->second)->BasesAssaulted += value;
            break;
        case SCORE_BASES_DEFENDED:
            ((BattleGroundICScore*)itr->second)->BasesDefended += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(source, type, value);
            break;
    }
}

void BattleGroundIC::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // show the reinforcements
    FillInitialWorldState(data, count, BG_IC_STATE_ALLY_REINFORCE_SHOW, 1);
    FillInitialWorldState(data, count, BG_IC_STATE_HORDE_REINFORCE_SHOW, 1);
    FillInitialWorldState(data, count, BG_IC_STATE_ALLY_REINFORCE_COUNT, m_reinforcements[TEAM_INDEX_ALLIANCE]);
    FillInitialWorldState(data, count, BG_IC_STATE_HORDE_REINFORCE_COUNT, m_reinforcements[TEAM_INDEX_ALLIANCE]);

    // show the capturable bases
    for (uint8 i = 0; i < BG_IC_MAX_OBJECTIVES; ++i)
        FillInitialWorldState(data, count, m_objectiveState[i], 1);

    // show the walls
    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        FillInitialWorldState(data, count, m_gatesAllianceState[i], 1);

    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        FillInitialWorldState(data, count, m_gatesHordeState[i], 1);
}

// process the gate events
bool BattleGroundIC::HandleEvent(uint32 eventId, GameObject* go, Unit* invoker)
{
    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
    {
        if (eventId == isleAllianceWallsData[i].eventId)
        {
            m_gatesAllianceState[i] = isleAllianceWallsData[i].stateOpened;
            UpdateWorldState(m_gatesAllianceState[i], 1);

            // todo: send language message
            // todo: open inner gates and spawn the boss

            return true;
        }

        if (eventId == isleHordeWallsData[i].eventId)
        {
            m_gatesHordeState[i] = isleAllianceWallsData[i].stateOpened;
            UpdateWorldState(m_gatesHordeState[i], 1);

            // todo: send language message
            // todo: open inner gates and spawn the boss

            return true;
        }
    }

    return false;
}

// Called when a player clicks a capturable banner
void BattleGroundIC::EventPlayerClickedOnFlag(Player* player, GameObject* go)
{
    // ToDo: handle objective capture
}

void BattleGroundIC::HandleKillUnit(Creature* creature, Player* killer)
{
    DEBUG_LOG("BattleGroundIC: Handle unit kill for craeture entry %u.", creature->GetEntry());

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // ToDo: add extra validations

    // end battle if boss was killed
    switch (creature->GetEntry())
    {
        case BG_IC_NPC_COMMANDER_WYRMBANE:
            EndBattleGround(HORDE);
            break;
        case BG_IC_NPC_OVERLORD_AGMAR:
            EndBattleGround(ALLIANCE);
            break;
    }
}

void BattleGroundIC::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);

    // remove one resource for each player killed
    PvpTeamIndex killedPlayerIdx = GetTeamIndexByTeamId(player->GetTeam());
    --m_reinforcements[killedPlayerIdx];

    // if reached 0, the other team wins
    if (!m_reinforcements[killedPlayerIdx])
        EndBattleGround(killer->GetTeam());
}

void BattleGroundIC::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case BG_IC_VEHICLE_KEEP_CANNON:
            creature->SetFactionTemporary(creature->GetPositionX() < 500.0f ? BG_IC_FACTION_ID_ALLIANCE : BG_IC_FACTION_ID_HORDE, TEMPFACTION_NONE);
            break;
        case BG_IC_VEHICLE_DEMOLISHER:
            creature->SetFactionTemporary(iocTeamFactions[m_objectiveOwner[BG_IC_OBJECTIVE_WORKSHOP]], TEMPFACTION_NONE);
            break;
        case BG_IC_VEHICLE_CATAPULT:
            creature->SetFactionTemporary(iocTeamFactions[m_objectiveOwner[BG_IC_OBJECTIVE_DOCKS]], TEMPFACTION_NONE);
            break;
    }
}

void BattleGroundIC::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_IC_GO_GUNSHIP_A:
            m_gunshipGuid[TEAM_INDEX_ALLIANCE] = go->GetObjectGuid();
            break;
        case BG_IC_GO_GUNSHIP_H:
            m_gunshipGuid[TEAM_INDEX_HORDE] = go->GetObjectGuid();
            break;
        case BG_IC_GO_SEAFORIUM_BOMBS:
            m_bombsGuids.push_back(go->GetObjectGuid());
            break;
    }
}

void BattleGroundIC::EndBattleGround(Team winner)
{
    // win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    // complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);
    RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);

    BattleGround::EndBattleGround(winner);
}

void BattleGroundIC::Update(uint32 diff)
{
    BattleGround::Update(diff);
}
