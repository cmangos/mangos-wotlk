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
#include "Globals/ObjectMgr.h"

BattleGroundSA::BattleGroundSA(): m_defendingTeamIdx(TEAM_INDEX_NEUTRAL), m_battleRoundTimer(0), m_boatStartTimer(0), m_battleStage(BG_SA_STAGE_ROUND_1)
{
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SA_START_TWO_MINUTES;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SA_BEGIN;
}

void BattleGroundSA::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (m_boatStartTimer)
    {
        if (m_boatStartTimer <= diff)
        {
            // Set the boats in motion
            for (const auto& guid : m_transportShipGuids[GetAttacker()])
            {
                if (GameObject* pShip = GetBgMap()->GetGameObject(guid))
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

    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // battle timer
        if (m_battleRoundTimer)
        {
            if (m_battleRoundTimer < diff)
            {
                switch (m_battleStage)
                {
                    case BG_SA_STAGE_ROUND_1:
                        // cast end of round spell
                        CastSpellOnTeam(BG_SA_SPELL_END_OF_ROUND, ALLIANCE);
                        CastSpellOnTeam(BG_SA_SPELL_END_OF_ROUND, HORDE);
                        m_battleRoundTimer = 5000;
                        break;
                    case BG_SA_STAGE_RESET:
                    {
                        // invert the defender
                        m_defendingTeamIdx = m_defendingTeamIdx == TEAM_INDEX_ALLIANCE ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE;

                        // setup player spells
                        for (auto& m_player : m_Players)
                        {
                            if (!m_player.first)
                                continue;

                            Player* player = sObjectMgr.GetPlayer(m_player.first);
                            if (player)
                            {
                                // remove auras
                                player->RemoveAurasDueToSpell(BG_SA_SPELL_END_OF_ROUND);

                                // teleport to the right spot and set phase
                                TeleportPlayerToStartArea(player);
                            }
                        }

                        // reset
                        SetupBattleground();
                        m_battleRoundTimer = 2000;
                        m_boatStartTimer = 2000;
                        break;
                    }
                    case BG_SA_STAGE_SECOND_ROUND_1:
                        SendMessageToAll(LANG_BG_SA_ROUND_START_ONE_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                        m_battleRoundTimer = 30000;
                        break;
                    case BG_SA_STAGE_SECOND_ROUND_2:
                        SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_SECOND], CHAT_MSG_BG_SYSTEM_NEUTRAL);
                        m_battleRoundTimer = BG_SA_TIMER_ROUND_LENGTH;
                        break;
                    case BG_SA_STAGE_ROUND_2:
                        // ToDo: compute winner
                        //EndBattleGround(xxx);
                        m_battleRoundTimer = 0;
                        break;
                }

                ++m_battleStage;
            }
            else
            {
                // update timer - if BattlegroundMgr update timer interval needs to be lowered replace this line with the commented-out ones below
                if (m_battleStage == BG_SA_STAGE_ROUND_1 || m_battleStage == BG_SA_STAGE_ROUND_2)
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

void BattleGroundSA::AddPlayer(Player* player)
{
    BattleGround::AddPlayer(player);
    // create score and add it to map, default values are set in constructor
    BattleGroundSAScore* score = new BattleGroundSAScore;

    m_PlayerScores[player->GetObjectGuid()] = score;

    TeleportPlayerToStartArea(player);
}

// function to teleport player to the starting area
void BattleGroundSA::TeleportPlayerToStartArea(Player* player)
{
    // Teleport player to the correct location
    if (GetTeamIndexByTeamId(player->GetTeam()) == GetAttacker())
    {
        // TODO: make this location more dyanmic, depending on the transport position
        player->CastSpell(player, BG_SA_SPELL_TELEPORT_ATTACKERS, TRIGGERED_OLD_TRIGGERED);

        // Note: the following code is temporary until spell effect 60178 is implemented
        uint8 randLoc = (urand(0, 1));
        player->TeleportTo(player->GetMapId(), strandTeleportLoc[randLoc][0], strandTeleportLoc[randLoc][1], strandTeleportLoc[randLoc][2], strandTeleportLoc[randLoc][3]);
    }
    else
    {
        // use stalker as a target
        if (Creature* stalker = GetBgMap()->GetCreature(m_defenderTeleportStalkerGuid))
            player->CastSpell(stalker, BG_SA_SPELL_TELEPORT_DEFENDER, TRIGGERED_OLD_TRIGGERED);
    }
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
    switch (creature->GetEntry())
    {
        case BG_SA_VEHICLE_CANNON:
            m_cannonsGuids.push_back(creature->GetObjectGuid());
            break;
        case BG_SA_VEHICLE_DEMOLISHER:
            break;
        case BG_SA_NPC_KANRETHAD:
            m_battlegroundMasterGuid = creature->GetObjectGuid();
            break;
        case BG_SA_NPC_INVISIBLE_STALKER:
            if (creature->GetPositionZ() > 80.0f)
                m_defenderTeleportStalkerGuid = creature->GetObjectGuid();
            else
                m_attackerTeleportStalkersGuids.push_back(creature->GetObjectGuid());
            break;
    }
}

void BattleGroundSA::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_1:
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_2:
                m_transportShipGuids[TEAM_INDEX_HORDE].push_back(go->GetObjectGuid());
            break;
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_1:
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_2:
                m_transportShipGuids[TEAM_INDEX_ALLIANCE].push_back(go->GetObjectGuid());
            break;
        case BG_SA_GO_GATE_GREEN_EMERALD:
        case BG_SA_GO_GATE_PURPLE_AMETHYST:
        case BG_SA_GO_GATE_BLUE_SAPHIRE:
        case BG_SA_GO_GATE_RED_SUN:
        case BG_SA_GO_GATE_YELLOW_MOON:
        case BG_SA_GO_GATE_ANCIENT_SHRINE:
            m_gatesGuids.push_back(go->GetObjectGuid());
            break;
        case BG_SA_GO_TITAN_RELIC_ALLIANCE:
            m_relicGuid[TEAM_INDEX_ALLIANCE] = go->GetObjectGuid();
            return;
        case BG_SA_GO_TITAN_RELIC_HORDE:
            m_relicGuid[TEAM_INDEX_HORDE] = go->GetObjectGuid();
            return;
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
    UpdateWorldState(BG_SA_STATE_ENABLE_TIMER, 1);
}

void BattleGroundSA::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // fill attacker & defender states
    FillInitialWorldState(data, count, BG_SA_STATE_ATTACKER_ALLIANCE, GetAttacker() == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_ATTACKER_HORDE, GetAttacker() == TEAM_INDEX_HORDE);

    FillInitialWorldState(data, count, BG_SA_STATE_DEFENSE_TOKEN_HORDE, GetAttacker() == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE, GetAttacker() == TEAM_INDEX_HORDE);

    FillInitialWorldState(data, count, BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE, GetAttacker() == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE, GetAttacker() == TEAM_INDEX_ALLIANCE);
    FillInitialWorldState(data, count, BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE, GetAttacker() == TEAM_INDEX_HORDE);
    FillInitialWorldState(data, count, BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE, GetAttacker() == TEAM_INDEX_HORDE);

    // fill gates states
    for (uint8 i = 0; i < BG_SA_MAX_GATES; ++i)
    {
        // special case for final gate: color depends on the defender (blue for alliance; red for horde)
        if (i == BG_SA_MAX_GATES - 1 && GetAttacker() == TEAM_INDEX_ALLIANCE)
            FillInitialWorldState(data, count, strandGates[i], m_gateStateValue[i] + 3);
        else
            FillInitialWorldState(data, count, strandGates[i], m_gateStateValue[i]);
    }

    // fill graveyard states
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
    {
        FillInitialWorldState(data, count, strandGraveyardAlly[i], m_defendingTeamIdx == m_graveyardOwner[i]);
        FillInitialWorldState(data, count, strandGraveyardHorde[i], m_defendingTeamIdx == m_graveyardOwner[i]);
    }

    // fill timer states - will be updated later in the script
    FillInitialWorldState(data, count, BG_SA_STATE_ENABLE_TIMER, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_MINUTES, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_SEC_SECOND_DIGIT, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_TIMER_SEC_FIRST_DIGIT, 0);
    FillInitialWorldState(data, count, BG_SA_STATE_BONUS_TIMER, 0);
}

// process the gate events
bool BattleGroundSA::HandleEvent(uint32 eventId, GameObject* go)
{
    // special event for the ancient shrine door
    if (go->GetEntry() == BG_SA_GO_GATE_ANCIENT_SHRINE && eventId == BG_SA_EVENT_SHRINE_DOOR_DESTROY)
    {
        // Remove the no interact flag when door is destroyed
        if (GameObject* relic = go->GetMap()->GetGameObject(m_relicGuid[GetAttacker()]))
            relic->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    }

    // handle world state updates and warnings
    for (const auto& i : sotaObjectData)
    {
        if (go->GetEntry() == i.goEntry)
        {
            if (eventId == i.eventDamaged)
            {
                UpdateWorldState(i.worldState, BG_SA_STATE_VALUE_GATE_DAMAGED);
                // SendMessage2ToAll(i.messageDamaged, CHAT_MSG_BG_SYSTEM_ALLIANCE, nullptr);
            }
            else if (eventId == i.eventDestroyed)
            {
                UpdateWorldState(i.worldState, BG_SA_STATE_VALUE_GATE_DESTROYED);
                // SendMessage2ToAll(i.messageDestroyed, CHAT_MSG_BG_SYSTEM_ALLIANCE, nullptr);

                // ToDo: despawn sigil
            }
            else if (eventId == i.eventRebuild)
            {
                go->SetFaction(sotaTeamFactions[m_defendingTeamIdx]);
                UpdateWorldState(i.worldState, BG_SA_STATE_VALUE_GATE_INTACT);
            }
        }
    }

    return false;
}

void BattleGroundSA::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    m_defendingTeamIdx = PvpTeamIndex(urand(0, 1));

    m_boatStartTimer = BG_SA_TIMER_BOAT_START;
    m_battleRoundTimer = BG_SA_TIMER_ROUND_LENGTH;

    // setup the battleground
    SetupBattleground();
}

void BattleGroundSA::EndBattleGround(Team winner)
{
    // win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    BattleGround::EndBattleGround(winner);
}

// Function to setup battleground
void BattleGroundSA::SetupBattleground()
{
    // set initial gate state values
    for (unsigned int& i : m_gateStateValue)
        i = BG_SA_STATE_VALUE_GATE_INTACT;

    // reset cannons
    for (const auto& guid : m_cannonsGuids)
    {
        if (Creature* cannon = GetBgMap()->GetCreature(guid))
        {
            cannon->setFaction(sotaTeamFactions[m_defendingTeamIdx]);
            cannon->Respawn();
        }
    }

    // reset Gates
    if (Creature* master = GetBgMap()->GetCreature(m_battlegroundMasterGuid))
    {
        for (const auto& guid : m_gatesGuids)
        {
            if (GameObject* gate = GetBgMap()->GetGameObject(guid))
                gate->RebuildGameObject(master);
        }
    }

    // set graveyards
    for (PvpTeamIndex& team : m_graveyardOwner)
        team = m_defendingTeamIdx;
}

// Check condition for ZM flag NPCs
bool BattleGroundSA::IsConditionFulfilled(Player const* source, uint32 conditionId, WorldObject const* conditionSource, uint32 conditionSourceType)
{
    switch (conditionId)
    {
        case BG_SA_COND_DEFENDER_ALLIANCE:
            return m_defendingTeamIdx == TEAM_INDEX_ALLIANCE;
        case BG_SA_COND_DEFENDER_HORDE:
            return m_defendingTeamIdx == TEAM_INDEX_HORDE;
    }

    return false;
}
