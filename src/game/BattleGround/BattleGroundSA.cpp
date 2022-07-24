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
#include "Entities/Vehicle.h"
#include "BattleGround.h"
#include "BattleGroundSA.h"
#include "Tools/Language.h"
#include "Globals/ObjectMgr.h"

BattleGroundSA::BattleGroundSA(): m_defendingTeamIdx(TEAM_INDEX_NEUTRAL), m_battleRoundTimer(0), m_boatStartTimer(0), m_battleStage(BG_SA_STAGE_ROUND_1), m_initialSetup(false)
{
    // set battleground start message ids
    m_startMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SA_START_TWO_MINUTES;
    m_startMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_FOURTH] = 0;
}

void BattleGroundSA::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    auto& variableManager = GetBgMap()->GetVariableManager();
    variableManager.SetVariableData(BG_SA_STATE_ATTACKER_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_ATTACKER_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_DEFENSE_TOKEN_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_TIMER_MINUTES, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_TIMER_SEC_SECOND_DIGIT, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_TIMER_SEC_FIRST_DIGIT, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_ENABLE_TIMER, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_BONUS_TIMER, true, 0, 0);

    variableManager.SetVariableData(BG_SA_STATE_PURPLE_GATE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_RED_GATE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_BLUE_GATE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_GREEN_GATE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_YELLOW_GATE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_ANCIENT_GATE, true, 0, 0);

    variableManager.SetVariableData(BG_SA_STATE_GY_LEFT_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_GY_RIGHT_ALLIANCE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_GY_CENTER_ALLIANCE, true, 0, 0);

    variableManager.SetVariableData(BG_SA_STATE_GY_LEFT_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_GY_RIGHT_HORDE, true, 0, 0);
    variableManager.SetVariableData(BG_SA_STATE_GY_CENTER_HORDE, true, 0, 0);

    SetDefender(TEAM_INDEX_HORDE);

    m_boatStartTimer = BG_SA_TIMER_BOAT_START;
    m_battleRoundTimer = BG_SA_TIMER_ROUND_LENGTH;

    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        m_scoreCount[i] = 0;
        m_winTime[i] = 0;
    }
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
                    pShip->SetGoState(GO_STATE_ACTIVE);
            }

            // ToDo: stop the boats when they reach the destination. Should be handled by event id 22095 and 18829
            m_boatStartTimer = 0;
        }
        else
            m_boatStartTimer -= diff;
    }

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // banner change timers; swap the flags between teams
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
    {
        if (m_strandGraveyard[i].changeTimer)
        {
            if (m_strandGraveyard[i].changeTimer <= diff)
            {
                ChangeBannerState(i);
                m_strandGraveyard[i].changeTimer = 0;
            }
            else
                m_strandGraveyard[i].changeTimer -= diff;
        }
    }

    // battle timer
    if (m_battleRoundTimer)
    {
        if (m_battleRoundTimer < diff)
        {
            switch (m_battleStage)
            {
                case BG_SA_STAGE_ROUND_1:
                    // eject all vehicle passengers and despawn vehicles immediately
                    for (uint32 dbGuid : m_demolishersGuids)
                    {
                        if (Creature* demolisher = GetBgMap()->GetCreature(dbGuid))
                        {
                            demolisher->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                            demolisher->ForcedDespawn();
                        }
                    }
                    for (ObjectGuid guid : m_tempDemolishersGuids)
                    {
                        if (Creature* demolisher = GetBgMap()->GetCreature(guid))
                        {
                            demolisher->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                            demolisher->ForcedDespawn();
                        }
                    }
                    for (uint32 dbGuid : m_cannonsGuids)
                    {
                        if (Creature* cannon = GetBgMap()->GetCreature(dbGuid))
                        {
                            cannon->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                            cannon->ForcedDespawn();
                        }
                    }
                    // send warning
                    SendBattlegroundWarning(LANG_BG_SA_ROUND_FINISHED);
                    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ENABLE_TIMER, WORLD_STATE_REMOVE);

                    // cast end of round spell; spell will also force resurrect dead players
                    CastSpellOnTeam(BG_SA_SPELL_END_OF_ROUND, ALLIANCE);
                    CastSpellOnTeam(BG_SA_SPELL_END_OF_ROUND, HORDE);

                    // award bonus honor
                    AwardBonusHonor();

                    m_battleRoundTimer = 5000;
                    break;
                case BG_SA_STAGE_RESET:
                {
                    // invert the defender
                    SetDefender(m_defendingTeamIdx == TEAM_INDEX_ALLIANCE ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE);

                    // reset
                    SetupBattleground(false);

                    // setup player spells
                    for (auto& m_player : m_players)
                    {
                        if (!m_player.first)
                            continue;

                        Player* player = sObjectMgr.GetPlayer(m_player.first);
                        if (player)
                        {
                            if (GetAttacker() == TEAM_INDEX_ALLIANCE)
                                player->CastSpell(nullptr, BG_SA_SPELL_HORDE_CONTROL_PHASE_SHIFT, TRIGGERED_OLD_TRIGGERED);
                            else
                                player->CastSpell(nullptr, BG_SA_SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, TRIGGERED_OLD_TRIGGERED);

                            // teleport to the right spot and set phase
                            TeleportPlayerToStartArea(player);
                        }
                    }

                    m_battleRoundTimer = 2000;
                    m_boatStartTimer = 2000;
                    break;
                }
                case BG_SA_STAGE_SECOND_ROUND_1:
                    SendMessageToAll(LANG_BG_SA_ROUND_START_ONE_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);

                    // cast preparation again before the 2nd round
                    CastSpellOnTeam(SPELL_PREPARATION, ALLIANCE);
                    CastSpellOnTeam(SPELL_PREPARATION, HORDE);

                    // make sure that the cannons have the right faction set; for some reason this isn't always correctly set
                    for (uint32 dbGuid : m_cannonsGuids)
                    {
                        if (Creature* cannon = GetBgMap()->GetCreature(dbGuid))
                            cannon->SetFactionTemporary(sotaTeamFactions[m_defendingTeamIdx], TEMPFACTION_NONE);
                    }

                    m_battleRoundTimer = 30000;
                    break;
                case BG_SA_STAGE_SECOND_ROUND_2:
                    SendMessageToAll(LANG_BG_SA_ROUND_START_HALF_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    m_battleRoundTimer = 30000;
                    break;
                case BG_SA_STAGE_SECOND_ROUND_3:

                    // remove preparation aura
                    for (auto& m_player : m_players)
                        if (Player* player = sObjectMgr.GetPlayer(m_player.first))
                            player->RemoveAurasDueToSpell(SPELL_PREPARATION);

                    SendBattlegroundWarning(LANG_BG_SA_BEGIN);
                    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ENABLE_TIMER, WORLD_STATE_ADD);
                    EnableDemolishers();
                    StartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, GetAttacker() == TEAM_INDEX_ALLIANCE ? BG_SA_ACHIEV_START_ID_STORM_BEACH_ALLY : BG_SA_ACHIEV_START_ID_STORM_BEACH_HORDE);
                    m_battleRoundTimer = BG_SA_TIMER_ROUND_LENGTH;
                    break;
                case BG_SA_STAGE_ROUND_2:
                    ProcessBattlegroundWinner();
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

void BattleGroundSA::AddPlayer(Player* player)
{
    BattleGround::AddPlayer(player);
    // create score and add it to map, default values are set in constructor
    BattleGroundSAScore* score = new BattleGroundSAScore;

    m_playerScores[player->GetObjectGuid()] = score;

    // TeleportPlayerToStartArea(player);

    // setup the battleground
    if (!m_initialSetup)
    {
        SetupBattleground(true);
        m_initialSetup = true;
    }
}

// function to teleport player to the starting area
void BattleGroundSA::TeleportPlayerToStartArea(Player* player)
{
    // Teleport player to the correct location
    if (GetTeamIndexByTeamId(player->GetTeam()) == GetAttacker())
    {
        // randomly teleport each player to the corresponding boat
        if (player->GetTeam() == ALLIANCE)
            player->CastSpell(player, urand(0, 1) ? BG_SA_SPELL_SPLIT_TELEPORT_A_BOAT1 : BG_SA_SPELL_SPLIT_TELEPORT_A_BOAT2, TRIGGERED_OLD_TRIGGERED);
        else
            player->CastSpell(player, urand(0, 1) ? BG_SA_SPELL_SPLIT_TELEPORT_H_BOAT1 : BG_SA_SPELL_SPLIT_TELEPORT_H_BOAT2, TRIGGERED_OLD_TRIGGERED);
    }
    else
        player->CastSpell(player, BG_SA_SPELL_TELEPORT_DEFENDER, TRIGGERED_OLD_TRIGGERED);
}

void BattleGroundSA::SetDefender(PvpTeamIndex teamIndex)
{
    PvpTeamIndex attacker = GetOtherTeamIndex(teamIndex);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ATTACKER_ALLIANCE, attacker == TEAM_INDEX_ALLIANCE);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ATTACKER_HORDE, attacker == TEAM_INDEX_HORDE);

    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_DEFENSE_TOKEN_HORDE, attacker == TEAM_INDEX_ALLIANCE);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_DEFENSE_TOKEN_ALLIANCE, attacker == TEAM_INDEX_HORDE);

    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_RIGHT_ATTACK_TOKEN_ALLIANCE, attacker == TEAM_INDEX_ALLIANCE);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_LEFT_ATTACK_TOKEN_ALLIANCE, attacker == TEAM_INDEX_ALLIANCE);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_RIGHT_ATTACK_TOKEN_HORDE, attacker == TEAM_INDEX_HORDE);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_LEFT_ATTACK_TOKEN_HORDE, attacker == TEAM_INDEX_HORDE);
    m_defendingTeamIdx = teamIndex;
}

void BattleGroundSA::StartingEventOpenDoors()
{
    EnableDemolishers();
    SetupGraveyards();

    SendBattlegroundWarning(LANG_BG_SA_BEGIN);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ENABLE_TIMER, WORLD_STATE_ADD);
    StartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, GetAttacker() == TEAM_INDEX_ALLIANCE ? BG_SA_ACHIEV_START_ID_STORM_BEACH_ALLY : BG_SA_ACHIEV_START_ID_STORM_BEACH_HORDE);
}

// function to allow demolishers to be used by players
void BattleGroundSA::EnableDemolishers()
{
    for (uint32 dbGuid : m_demolishersGuids)
    {
        if (Creature* demolisher = GetBgMap()->GetCreature(dbGuid))
            demolisher->SetFactionTemporary(sotaTeamFactions[GetAttacker()], TEMPFACTION_TOGGLE_NOT_SELECTABLE);
    }

    m_noScratchAchiev = true;
    m_defenseAncients = true;
}

void BattleGroundSA::SetupGraveyards()
{
    Creature* master = GetBgMap()->GetCreature(m_battlegroundMasterGuid);
    if (!master)
        return;

    uint32 defenderHealerEntry = GetAttacker() == TEAM_INDEX_ALLIANCE ? BG_NPC_SPIRIT_GUIDE_HORDE : BG_NPC_SPIRIT_GUIDE_ALLIANCE;

    // summon the spirit healer
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
        if (Creature* healer = master->SummonCreature(defenderHealerEntry, strandGraveyardData[i].x, strandGraveyardData[i].y, strandGraveyardData[i].z, strandGraveyardData[i].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_strandGraveyard[i].spiritHealerGuid = healer->GetObjectGuid();
};

void BattleGroundSA::UpdatePlayerScore(Player* source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_playerScores.find(source->GetObjectGuid());
    if (itr == m_playerScores.end())
        return;

    switch (type)
    {
        case SCORE_DEMOLISHERS_DESTROYED:
            static_cast<BattleGroundSAScore*>(itr->second)->demolishersDestroyed += value;
            break;
        case SCORE_GATES_DESTROYED:
            static_cast<BattleGroundSAScore*>(itr->second)->gatesDestroyed += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(source, type, value);
            break;
    }
}

void BattleGroundSA::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case BG_SA_VEHICLE_CANNON:
            m_cannonsGuids.insert(creature->GetDbGuid());
            break;
        case BG_SA_VEHICLE_DEMOLISHER:
            if (creature->IsTemporarySummon())
            {
                creature->SetFactionTemporary(sotaTeamFactions[GetAttacker()], TEMPFACTION_TOGGLE_NOT_SELECTABLE);
                m_tempDemolishersGuids.push_back(creature->GetObjectGuid());
            }
            else
                m_demolishersGuids.insert(creature->GetDbGuid());
            break;
        case BG_SA_NPC_KANRETHAD:
            m_battlegroundMasterGuid = creature->GetObjectGuid();
            break;
        case BG_SA_NPC_INVISIBLE_STALKER:
            if (creature->GetPositionZ() > 80.0f)
                m_defenderTeleportStalkerGuid = creature->GetDbGuid();
            else
                m_attackerTeleportStalkersGuids.insert(creature->GetDbGuid());
            break;
        case BG_SA_NPC_WORLD_TRIGGER:
            m_triggerGuids.insert(creature->GetDbGuid());
            break;
        case BG_SA_NPC_RIGGER_SPARKLIGHT:
            m_riggerGuid = creature->GetObjectGuid();
            break;
        case BG_SA_NPC_GORGRIL_RIGSPARK:
            m_gorgrilGuid = creature->GetObjectGuid();
            break;
    }
}

void BattleGroundSA::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_1:
        case BG_SA_GO_TRANSPORT_SHIP_HORDE_2:
                m_transportShipGuids[TEAM_INDEX_HORDE].insert(go->GetDbGuid());
            break;
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_1:
        case BG_SA_GO_TRANSPORT_SHIP_ALLIANCE_2:
                m_transportShipGuids[TEAM_INDEX_ALLIANCE].insert(go->GetDbGuid());
            break;
        case BG_SA_GO_GATE_GREEN_EMERALD:
        case BG_SA_GO_GATE_PURPLE_AMETHYST:
        case BG_SA_GO_GATE_BLUE_SAPHIRE:
        case BG_SA_GO_GATE_RED_SUN:
        case BG_SA_GO_GATE_YELLOW_MOON:
        case BG_SA_GO_GATE_ANCIENT_SHRINE:
            m_gatesGuids.insert(go->GetDbGuid());
            break;
        case BG_SA_GO_TITAN_RELIC_ALLIANCE:
            m_relicGuid[TEAM_INDEX_ALLIANCE] = go->GetObjectGuid();
            break;
        case BG_SA_GO_TITAN_RELIC_HORDE:
            m_relicGuid[TEAM_INDEX_HORDE] = go->GetObjectGuid();
            break;
        case BG_SA_GO_GY_FLAG_ALLIANCE_EAST:
        case BG_SA_GO_GY_FLAG_ALLIANCE_WEST:
        case BG_SA_GO_GY_FLAG_ALLIANCE_SOUTH:
        case BG_SA_GO_GY_FLAG_HORDE_EAST:
        case BG_SA_GO_GY_FLAG_HORDE_WEST:
        case BG_SA_GO_GY_FLAG_HORDE_SOUTH:
        case BG_SA_GO_SIGIL_YELLOW_MOON:
        case BG_SA_GO_SIGIL_GREEN_MOON:
        case BG_SA_GO_SIGIL_BLUE_MOON:
        case BG_SA_GO_SIGIL_RED_MOON:
        case BG_SA_GO_SIGIL_PURPLE_MOON:
            m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
            break;
    }
}

// Function used to update the timer
void BattleGroundSA::UpdateTimerWorldState()
{
    // Calculate time
    uint32 secondsLeft = m_battleRoundTimer;

    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_TIMER_MINUTES, secondsLeft / 60000);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_TIMER_SEC_FIRST_DIGIT, ((secondsLeft % 60000) % 10000) / 1000);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_TIMER_SEC_SECOND_DIGIT, (secondsLeft % 60000) / 10000);
    GetBgMap()->GetVariableManager().SetVariable(BG_SA_STATE_ENABLE_TIMER, WORLD_STATE_ADD);
}

// process the gate and relic events
bool BattleGroundSA::HandleEvent(uint32 eventId, Object* source, Object* target)
{
    if (!source->IsGameObject())
        return false;

    GameObject* go = static_cast<GameObject*>(source);

    // handle ships stop event
    if (eventId == BG_SA_EVENT_ID_SHIP_PAUSE_1 || eventId == BG_SA_EVENT_ID_SHIP_PAUSE_2)
    {
        DEBUG_LOG("BattleGroundSA: Ship with entry %u has reached the docks.", go->GetEntry());

        // ToDo: implement ship stop logic once transports can handle this properly
        return false;
    }

    // handle round end by relic click
    if (eventId == BG_SA_EVENT_ID_RELIC)
    {
        DEBUG_LOG("BattleGroundSA: Relic with entry %u has been activated", go->GetEntry());

        // update score count and keep track of the time
        if (go->GetEntry() == BG_SA_GO_TITAN_RELIC_ALLIANCE)
        {
            ++m_scoreCount[TEAM_INDEX_ALLIANCE];
            m_winTime[TEAM_INDEX_ALLIANCE] = m_battleRoundTimer;
            CastSpellOnTeam(BG_SA_SPELL_ACHIEV_STORM_BEACH, ALLIANCE);
        }
        else if (go->GetEntry() == BG_SA_GO_TITAN_RELIC_HORDE)
        {
            ++m_scoreCount[TEAM_INDEX_HORDE];
            m_winTime[TEAM_INDEX_HORDE] = m_battleRoundTimer;
            CastSpellOnTeam(BG_SA_SPELL_ACHIEV_STORM_BEACH, HORDE);
        }

        // process winner
        if (m_battleStage == BG_SA_STAGE_ROUND_2)
            ProcessBattlegroundWinner();
        // prepare second round
        else if (m_battleStage == BG_SA_STAGE_ROUND_1)
            m_battleRoundTimer = 500;

        m_defenseLineCaptured[3] = true;
        return false;
    }

    // special event for the ancient shrine door
    if (go->GetEntry() == BG_SA_GO_GATE_ANCIENT_SHRINE && eventId == BG_SA_EVENT_SHRINE_DOOR_DESTROY)
    {
        DEBUG_LOG("BattleGroundSA: Ancient Shrine id %u has been activated by event %u", go->GetEntry(), eventId);

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
                DEBUG_LOG("BattleGroundSA: Gate with entry %u has been damaged", go->GetEntry());

                m_gateStateValue[i.index] = BG_SA_STATE_VALUE_GATE_DAMAGED;
                GetBgMap()->GetVariableManager().SetVariable(i.worldState, m_gateStateValue[i.index]);
                SendBattlegroundWarning(i.messageDamaged);
                PlaySoundToAll(GetAttacker() == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
            }
            else if (eventId == i.eventDestroyed)
            {
                DEBUG_LOG("BattleGroundSA: Gate with entry %u has been destroyed", go->GetEntry());

                m_gateStateValue[i.index] = BG_SA_STATE_VALUE_GATE_DESTROYED;
                GetBgMap()->GetVariableManager().SetVariable(i.worldState, m_gateStateValue[i.index]);
                SendBattlegroundWarning(i.messagedDestroyed);
                PlaySoundToAll(GetAttacker() == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);

                if (target && target->IsPlayer())
                    UpdatePlayerScore(static_cast<Player*>(target), SCORE_GATES_DESTROYED, 1);

                // fail the achievement
                m_defenseAncients = false;

                // despawn sigil
                if (i.index < BG_SA_MAX_SIGILS)
                    if (GameObject* sigil = GetSingleGameObjectFromStorage(strandSigils[i.index]))
                        ChangeBgObjectSpawnState(sigil->GetObjectGuid(), RESPAWN_ONE_DAY);

                // count honor multiplier, this depends on the defense lines that are destroyed
                // the pair of gates, count as one
                switch (go->GetEntry())
                {
                    case BG_SA_GO_GATE_YELLOW_MOON:
                        m_defenseLineCaptured[2] = true;
                        break;
                    case BG_SA_GO_GATE_PURPLE_AMETHYST:
                    case BG_SA_GO_GATE_RED_SUN:
                        if (!m_defenseLineCaptured[1])
                            m_defenseLineCaptured[1] = true;
                        break;
                    case BG_SA_GO_GATE_GREEN_EMERALD:
                    case BG_SA_GO_GATE_BLUE_SAPHIRE:
                        if (!m_defenseLineCaptured[0])
                            m_defenseLineCaptured[0] = true;
                        break;
                }
            }
            else if (eventId == i.eventRebuild)
            {
                m_gateStateValue[i.index] = BG_SA_STATE_VALUE_GATE_INTACT;
                go->SetFaction(sotaTeamFactions[m_defendingTeamIdx]);
                GetBgMap()->GetVariableManager().SetVariable(i.worldState, m_gateStateValue[i.index]);
            }

            break;
        }
    }

    return false;
}

// Called when a player clicks a graveyard banner
void BattleGroundSA::HandlePlayerClickedOnFlag(Player* player, GameObject* go)
{
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
    {
        if (go->GetEntry() == sotaGraveyardData[i].goEntryAlliance || go->GetEntry() == sotaGraveyardData[i].goEntryHorde)
        {
            DEBUG_LOG("BattleGroundSA: Graveyard banner with id %u was clicked.", go->GetEntry());

            m_strandGraveyard[i].graveyardOwner = GetAttacker();

            uint32 newBannerEntry = GetAttacker() == TEAM_INDEX_ALLIANCE ? sotaGraveyardData[i].goEntryAlliance : sotaGraveyardData[i].goEntryHorde;
            GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(sotaGraveyardData[i].graveyardId, BG_SA_ZONE_ID_STRAND, GetTeamIdByTeamIndex(GetAttacker()));

            // spawn demolishers
            if (sotaGraveyardData[i].graveyardId == BG_SA_GRAVEYARD_ID_EAST)
            {
                for (uint8 i = 0; i < 3; ++i)
                    go->SummonCreature(sotaEastSpawns[i].entry, sotaEastSpawns[i].x, sotaEastSpawns[i].y, sotaEastSpawns[i].z, sotaEastSpawns[i].o, TEMPSPAWN_TIMED_OR_CORPSE_DESPAWN, 10 * MINUTE * IN_MILLISECONDS);
            }
            else if (sotaGraveyardData[i].graveyardId == BG_SA_GRAVEYARD_ID_WEST)
            {
                for (uint8 i = 0; i < 3; ++i)
                    go->SummonCreature(sotaWestSpawns[i].entry, sotaWestSpawns[i].x, sotaWestSpawns[i].y, sotaWestSpawns[i].z, sotaWestSpawns[i].o, TEMPSPAWN_TIMED_OR_CORPSE_DESPAWN, 10 * MINUTE * IN_MILLISECONDS);
            }

            // set the data for the banner change
            m_strandGraveyard[i].changeTimer = 3000;
            m_strandGraveyard[i].oldEntry = go->GetEntry();
            m_strandGraveyard[i].newEntry = newBannerEntry;

            // send text and update world states
            if (go->GetEntry() == sotaGraveyardData[i].goEntryHorde)
            {
                GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateHorde, WORLD_STATE_REMOVE);
                GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateAlliance, WORLD_STATE_ADD);

                SendBattlegroundWarning(sotaGraveyardData[i].textCaptureAlliance);
                PlaySoundToAll(BG_SA_SOUND_GRAVEYARD_TAKEN_ALLIANCE);
            }
            else if (go->GetEntry() == sotaGraveyardData[i].goEntryAlliance)
            {
                GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateAlliance, WORLD_STATE_REMOVE);
                GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateHorde, WORLD_STATE_ADD);

                SendBattlegroundWarning(sotaGraveyardData[i].textCaptureHorde);
                PlaySoundToAll(BG_SA_SOUND_GRAVEYARD_TAKEN_HORDE);
            }

            // update spirit healer; despawn old healer and summon new one
            if (Creature* pHealer = GetBgMap()->GetCreature(m_strandGraveyard[i].spiritHealerGuid))
                pHealer->ForcedDespawn();

            uint32 healerEntry = go->GetEntry() == sotaGraveyardData[i].goEntryAlliance ? BG_NPC_SPIRIT_GUIDE_HORDE : BG_NPC_SPIRIT_GUIDE_ALLIANCE;
            if (Creature* healer = go->SummonCreature(healerEntry, strandGraveyardData[i].x, strandGraveyardData[i].y, strandGraveyardData[i].z, strandGraveyardData[i].o, TEMPSPAWN_DEAD_DESPAWN, 0))
                m_strandGraveyard[i].spiritHealerGuid = healer->GetObjectGuid();

            break;
        }
    }
}

void BattleGroundSA::HandleKillUnit(Creature* unit, Player* killer)
{
    if (!unit)
        return;

    if (unit->GetEntry() == BG_SA_VEHICLE_DEMOLISHER)
    {
        UpdatePlayerScore(killer, SCORE_DEMOLISHERS_DESTROYED, 1);
        m_noScratchAchiev = false;
    }
}

void BattleGroundSA::EndBattleGround(Team winner)
{
    // win reward
    RewardHonorToTeam(GetBonusHonorFromKill(1), winner);

    // complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(2), HORDE);
    RewardHonorToTeam(GetBonusHonorFromKill(2), ALLIANCE);

    BattleGround::EndBattleGround(winner);
}

// function that will process the battleground winner
void BattleGroundSA::ProcessBattlegroundWinner()
{
    Team winner = TEAM_NONE;

    // cast preparation again at the end of the rounds; required for achievement check
    CastSpellOnTeam(SPELL_PREPARATION, ALLIANCE);
    CastSpellOnTeam(SPELL_PREPARATION, HORDE);

    // in case of a tie verify the time counters
    if (m_scoreCount[TEAM_INDEX_ALLIANCE] == m_scoreCount[TEAM_INDEX_HORDE] && m_scoreCount[TEAM_INDEX_ALLIANCE] == 1)
    {
        // the higher the win time = the faster the team captured the relic
        if (m_winTime[TEAM_INDEX_ALLIANCE] > m_winTime[TEAM_INDEX_HORDE])
            winner = ALLIANCE;
        else if (m_winTime[TEAM_INDEX_ALLIANCE] < m_winTime[TEAM_INDEX_HORDE])
            winner = HORDE;
    }
    else if (m_scoreCount[TEAM_INDEX_ALLIANCE] > m_scoreCount[TEAM_INDEX_HORDE])
        winner = ALLIANCE;
    else if (m_scoreCount[TEAM_INDEX_ALLIANCE] < m_scoreCount[TEAM_INDEX_HORDE])
        winner = HORDE;

    DEBUG_LOG("BattleGroundSA: Team %u won the match", winner);

    // send game end sounds
    if (winner == ALLIANCE)
    {
        PlaySoundToTeam(BG_SA_SOUND_VICTORY_ALLIANCE, ALLIANCE);
        PlaySoundToTeam(BG_SA_SOUND_DEFEAT_HORDE, HORDE);
    }
    else if (winner == HORDE)
    {
        PlaySoundToTeam(BG_SA_SOUND_DEFEAT_ALLIANCE, ALLIANCE);
        PlaySoundToTeam(BG_SA_SOUND_VICTORY_HORDE, HORDE);
    }

    AwardBonusHonor();
    EndBattleGround(winner);
}

// Process and award bonus honor to both teams
void BattleGroundSA::AwardBonusHonor()
{
    // Each defense line counts for the honor multiplier
    Team defender = GetTeamIdByTeamIndex(m_defendingTeamIdx);
    Team attacker = GetTeamIdByTeamIndex(GetAttacker());

    uint8 attackerMultiplier = 0;

    for (uint8 i = 0; i < BG_SA_MAX_DEFENSE_LINES; ++i)
    {
        if (m_defenseLineCaptured[i])
            ++attackerMultiplier;
    }

    uint8 defenderMultiplier = (BG_SA_MAX_DEFENSE_LINES - attackerMultiplier) * 0.5;

    RewardHonorToTeam(GetBonusHonorFromKill(defenderMultiplier), defender);
    RewardHonorToTeam(GetBonusHonorFromKill(attackerMultiplier), attacker);
}

// Method that will change the banner between teams
void BattleGroundSA::ChangeBannerState(uint8 nodeId)
{
    if (GameObject* oldBanner = GetSingleGameObjectFromStorage(m_strandGraveyard[nodeId].oldEntry))
        oldBanner->SetLootState(GO_JUST_DEACTIVATED);

    if (GameObject* newBanner = GetSingleGameObjectFromStorage(m_strandGraveyard[nodeId].newEntry))
    {
        newBanner->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        newBanner->SetRespawnTime(13 * MINUTE);
        newBanner->Refresh();
    }
}

// Function to setup battleground
void BattleGroundSA::SetupBattleground(bool initialSetup)
{
    DEBUG_LOG("BattleGroundSA: Setup battleground for stage: %u", m_battleStage);

    // reset honor multiplier
    for (uint8 i = 0; i < BG_SA_MAX_DEFENSE_LINES; ++i)
        m_defenseLineCaptured[i] = false;

    // set initial gate state values
    for (uint32& i : m_gateStateValue)
        i = BG_SA_STATE_VALUE_GATE_INTACT;

    for (uint8 i = 0; i < BG_SA_MAX_GATES; ++i)
    {
        // special case for final gate: color depends on the defender (blue for alliance; red for horde)
        if (i == BG_SA_MAX_GATES - 1 && GetAttacker() == TEAM_INDEX_ALLIANCE)
            GetBgMap()->GetVariableManager().SetVariable(strandGates[i], m_gateStateValue[i] + 3);
        else
            GetBgMap()->GetVariableManager().SetVariable(strandGates[i], m_gateStateValue[i]);
    }

    // reset cannons
    for (uint32 dbGuid : m_cannonsGuids)
    {
        if (Creature* cannon = GetBgMap()->GetCreature(dbGuid))
        {
            cannon->SetFactionTemporary(sotaTeamFactions[m_defendingTeamIdx], TEMPFACTION_NONE);

            if (!cannon->IsAlive())
                cannon->Respawn();
        }
    }

    // despawn goblins
    if (Creature* goblin = GetBgMap()->GetCreature(m_riggerGuid))
        goblin->ForcedDespawn();
    if (Creature* goblin = GetBgMap()->GetCreature(m_gorgrilGuid))
        goblin->ForcedDespawn();

    // reset demolishers; clean temp faction to re-enable the unit flag
    for (uint32 dbGuid : m_demolishersGuids)
    {
        if (Creature* demolisher = GetBgMap()->GetCreature(dbGuid))
        {
            if (!demolisher->IsAlive())
                demolisher->Respawn();

            demolisher->ClearTemporaryFaction();
        }
    }

    // reset graveyards
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
    {
        // despawn the spirit healers
        if (Creature* healer = GetBgMap()->GetCreature(m_strandGraveyard[i].spiritHealerGuid))
            healer->ForcedDespawn();

        uint32 attackerBannerEntry = GetAttacker() == TEAM_INDEX_ALLIANCE ? sotaGraveyardData[i].goEntryAlliance : sotaGraveyardData[i].goEntryHorde;
        uint32 defenderBannerEntry = m_defendingTeamIdx == TEAM_INDEX_ALLIANCE ? sotaGraveyardData[i].goEntryAlliance : sotaGraveyardData[i].goEntryHorde;

        // despawn the attacker banners
        if (GameObject* banner = GetSingleGameObjectFromStorage(attackerBannerEntry))
            ChangeBgObjectSpawnState(banner->GetObjectGuid(), RESPAWN_ONE_DAY);

        // respawn graveyard banners and remove the no interact flag
        if (GameObject* banner = GetSingleGameObjectFromStorage(defenderBannerEntry))
        {
            ChangeBgObjectSpawnState(banner->GetObjectGuid(), RESPAWN_IMMEDIATELY);
            banner->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        }

        // set capturable graveyard links and states
        m_strandGraveyard[i].graveyardOwner = m_defendingTeamIdx;
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(sotaGraveyardData[i].graveyardId, BG_SA_ZONE_ID_STRAND, GetTeamIdByTeamIndex(m_defendingTeamIdx));

        m_strandGraveyard[i].changeTimer = 0;
    }

    // fill graveyard states
    for (uint8 i = 0; i < BG_SA_MAX_GRAVEYARDS; ++i)
    {
        GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateAlliance, m_strandGraveyard[i].graveyardOwner == TEAM_INDEX_ALLIANCE);
        GetBgMap()->GetVariableManager().SetVariable(sotaGraveyardData[i].worldStateHorde, m_strandGraveyard[i].graveyardOwner == TEAM_INDEX_HORDE);
    }

    // set static graveyards
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_SA_GRAVEYARD_ID_SHRINE, BG_SA_ZONE_ID_STRAND, GetTeamIdByTeamIndex(m_defendingTeamIdx));
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_SA_GRAVEYARD_ID_BEACH, BG_SA_ZONE_ID_STRAND, GetTeamIdByTeamIndex(GetAttacker()));
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_SA_GRAVEYARD_ID_SHIP, BG_SA_ZONE_ID_STRAND, GetTeamIdByTeamIndex(GetAttacker()));

    // sigil and gates don't have to be initialized
    if (initialSetup)
        return;

    // reset gates
    if (Creature* master = GetBgMap()->GetCreature(m_battlegroundMasterGuid))
    {
        for (uint32 dbGuid : m_gatesGuids)
        {
            if (GameObject* gate = GetBgMap()->GetGameObject(dbGuid))
                gate->RebuildGameObject(master);
        }
    }

    // reset sigils
    for (uint8 i = 0; i < BG_SA_MAX_SIGILS; ++i)
        if (GameObject* sigil = GetSingleGameObjectFromStorage(strandSigils[i]))
            ChangeBgObjectSpawnState(sigil->GetObjectGuid(), RESPAWN_IMMEDIATELY);
}

// Function to handle the warnings
void BattleGroundSA::SendBattlegroundWarning(int32 messageId)
{
    // The warnings are sent by a random trigger; it's difficult to figure out the exact logic here
    auto itr = m_triggerGuids.begin();
    std::advance(itr, urand(0, m_triggerGuids.size() - 1));
    Creature* creature = GetBgMap()->GetCreature(*itr); // doing inefficient objectguid resolution

    if (creature)
        SendMessageToAll(messageId, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, creature->GetObjectGuid());
}

void BattleGroundSA::HandleGameObjectDamaged(Player* player, GameObject* object, uint32 spellId)
{
    if (spellId == BG_SA_SPELL_SEAFORIUM_BLAST)
        player->CastSpell(player, BG_SA_SPELL_ACHIEV_SEAFORIUM_DAMAGE, TRIGGERED_OLD_TRIGGERED);
}

bool BattleGroundSA::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1)
{
    switch (criteria_id)
    {
        case BG_SA_CRIT_NOT_A_SCRATCH_ALLY:
        case BG_SA_CRIT_NOT_A_SCRATCH_HORDE:
            return m_noScratchAchiev;
        case BG_SA_CRIT_DEFENSE_ANCIENTS_ALLY:
        case BG_SA_CRIT_DEFENSE_ANCIENTS_HORDE:
            return m_defenseAncients && GetTeamIndexByTeamId(source->GetTeam()) == m_defendingTeamIdx;
        case BG_SA_CRIT_ARTILLERY_EXPERT:
        case BG_SA_CRIT_ARTILLERY_VETERAN:
            return source->IsBoarded() && source->GetTransportInfo()->GetTransport()->GetEntry() == BG_SA_VEHICLE_CANNON;
        case BG_SA_CRIT_DROP_IT:
        case BG_SA_CRIT_DROP_IT_NOW:
            // check aura in script because the DB condition uses a different function which returns wrong result when player is killed
            return target->HasAura(BG_SA_SPELL_CARRY_SEAFORIUM);
    }

    return false;
}

void BattleGroundSA::AlterTeleportLocation(Player* player, ObjectGuid& transportGuid, float& x, float& y, float& z, float& ori)
{
    SpellCastResult result;
    if (GetAttacker() == TEAM_INDEX_ALLIANCE)
        result = player->CastSpell(nullptr, BG_SA_SPELL_HORDE_CONTROL_PHASE_SHIFT, TRIGGERED_OLD_TRIGGERED);
    else
        result = player->CastSpell(nullptr, BG_SA_SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, TRIGGERED_OLD_TRIGGERED);

    if (GetTeamIndexByTeamId(player->GetTeam()) == GetAttacker())
    {
        if (urand(0, 1))
        {
            if (GameObject* gameobject = GetBgMap()->GetGameObject(player->GetTeam() == ALLIANCE ? 6070149 : 6070150))
                transportGuid = gameobject->GetObjectGuid();
            x = -6.f;
            y = -3.f;
            z = 8.8f;
            ori = 3.8f;
        }
        else
        {
            if (GameObject* gameobject = GetBgMap()->GetGameObject(player->GetTeam() == ALLIANCE ? 6070151 : 6070148))
                transportGuid = gameobject->GetObjectGuid();
            x = 0.f;
            y = 5.f;
            z = 9.6f;
            ori = 3.14f;
        }
    }
    else
    {
        x = 1226.43f;
        y = -71.9616f;
        z = 70.0842f;
        ori = 0.f;
    }
}

// Check condition for SotA phasing aura
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
