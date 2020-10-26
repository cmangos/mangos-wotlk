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
#include "BattleGroundIC.h"
#include "Tools/Language.h"
#include "Globals/ObjectMgr.h"
#include "Entities/Transports.h"

BattleGroundIC::BattleGroundIC() : m_hordeInnerGateGuid(ObjectGuid()), m_allianceInnerGate1Guid(ObjectGuid()), m_allianceInnerGate2Guid(ObjectGuid()), m_closeDoorTimer(0)
{
    // set battleground start message ids
    m_startMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_IC_START_TWO_MINUTES;
    m_startMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_IC_START_ONE_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_IC_START_HALF_MINUTE;
    // ToDo: use the text LANG_BG_IC_START_15_SECONDS
    m_startMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_IC_BEGIN;
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
        switch (i)
        {
            case BG_IC_OBJECTIVE_KEEP_ALLY:
                m_isleNode[i].nodeOwner  = TEAM_INDEX_ALLIANCE;
                m_isleNode[i].nodeConquerer = TEAM_INDEX_ALLIANCE;
                break;
            case BG_IC_OBJECTIVE_KEEP_HORDE:
                m_isleNode[i].nodeOwner = TEAM_INDEX_HORDE;
                m_isleNode[i].nodeConquerer = TEAM_INDEX_HORDE;
                break;
            default:
                m_isleNode[i].nodeOwner = TEAM_INDEX_NEUTRAL;
                m_isleNode[i].nodeConquerer = TEAM_INDEX_NEUTRAL;
                break;
        }

        m_isleNode[i].nodeWorldState = iocDefaultStates[i];
        m_isleNode[i].nodeChangeTimer = 0;
        m_isleNode[i].bannerChangeTimer = 0;
    }

    // setup the state for the keep walls
    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        m_gatesAllianceState[i] = isleAllianceWallsData[i].stateClosed;

    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        m_gatesHordeState[i] = isleHordeWallsData[i].stateClosed;

    for (uint8 i = 0; i < BG_IC_MAX_RESOURCE_NODES; ++i)
        m_resourceTickTimer[i] = 0;

    m_closeDoorTimer = 0;
    m_isKeepInvaded[TEAM_INDEX_ALLIANCE] = false;
    m_isKeepInvaded[TEAM_INDEX_HORDE] = false;

    // setup master graveyards
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_ALLIANCE, BG_IC_ZONE_ID_ISLE, ALLIANCE);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_HORDE, BG_IC_ZONE_ID_ISLE, HORDE);

    // setup initial graveyards
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_KEEP_ALLY, BG_IC_ZONE_ID_ISLE, ALLIANCE);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_KEEP_HORDE, BG_IC_ZONE_ID_ISLE, HORDE);

    // make sure that the middle graveyards are disabled to begin with
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_DOCKS, BG_IC_ZONE_ID_ISLE, TEAM_INVALID);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_HANGAR, BG_IC_ZONE_ID_ISLE, TEAM_INVALID);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(BG_IC_GRAVEYARD_ID_WORKSHOP, BG_IC_ZONE_ID_ISLE, TEAM_INVALID);
}

void BattleGroundIC::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundICScore* sc = new BattleGroundICScore;

    m_playerScores[plr->GetObjectGuid()] = sc;

    // spawn starting area honorable defenders alliance
    if (m_isleNode[BG_IC_OBJECTIVE_KEEP_ALLY].honorableDefenderGuid.IsEmpty() && plr->GetTeam() == ALLIANCE)
    {
        if (Creature* trigger = plr->SummonCreature(BG_NPC_HON_DEFENDER_TRIGGER_A, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_ALLY].x, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_ALLY].y, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_ALLY].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_isleNode[BG_IC_OBJECTIVE_KEEP_ALLY].honorableDefenderGuid = trigger->GetObjectGuid();

        for (const auto& i : iocHonorTriggerAllySpawns)
        {
            if (Creature* trigger = plr->SummonCreature(i.entryAlly, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                m_isleNode[BG_IC_OBJECTIVE_KEEP_ALLY].creatureGuids.push_back(trigger->GetObjectGuid());
        }
    }

    // spawn starting area honorable defenders horde
    if (m_isleNode[BG_IC_OBJECTIVE_KEEP_HORDE].honorableDefenderGuid.IsEmpty() && plr->GetTeam() == HORDE)
    {
        if (Creature* trigger = plr->SummonCreature(BG_NPC_HON_DEFENDER_TRIGGER_H, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_HORDE].x, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_HORDE].y, isleObjectiveData[BG_IC_OBJECTIVE_KEEP_HORDE].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_isleNode[BG_IC_OBJECTIVE_KEEP_HORDE].honorableDefenderGuid = trigger->GetObjectGuid();

        for (const auto& i : iocHonorTriggerHordeSpawns)
        {
            if (Creature* trigger = plr->SummonCreature(i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                m_isleNode[BG_IC_OBJECTIVE_KEEP_HORDE].creatureGuids.push_back(trigger->GetObjectGuid());
        }
    }

    // summon the initial spirit healers
    if (m_isleNode[BG_IC_OBJECTIVE_KEEP_ALLY].spiritHealerGuid.IsEmpty() && plr->GetTeam() == ALLIANCE)
    {
        if (Creature* healer = plr->SummonCreature(BG_NPC_SPIRIT_GUIDE_ALLIANCE, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_ALLY].x, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_ALLY].y, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_ALLY].z, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_ALLY].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_isleNode[BG_IC_OBJECTIVE_KEEP_ALLY].spiritHealerGuid = healer->GetObjectGuid();
    }

    if (m_isleNode[BG_IC_OBJECTIVE_KEEP_HORDE].spiritHealerGuid.IsEmpty() && plr->GetTeam() == HORDE)
    {
        if (Creature* healer = plr->SummonCreature(BG_NPC_SPIRIT_GUIDE_HORDE, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_HORDE].x, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_HORDE].y, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_HORDE].z, isleGraveyardData[BG_IC_OBJECTIVE_KEEP_HORDE].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_isleNode[BG_IC_OBJECTIVE_KEEP_HORDE].spiritHealerGuid = healer->GetObjectGuid();
    }

    // apply buff auras
    if (m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner != TEAM_INDEX_NEUTRAL)
        if (plr->GetTeam() == GetTeamIdByTeamIndex(m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner) && m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeConquerer == m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner)
            plr->CastSpell(plr, BG_IC_SPELL_REFINERY, TRIGGERED_OLD_TRIGGERED);
    if (m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner != TEAM_INDEX_NEUTRAL)
        if (plr->GetTeam() == GetTeamIdByTeamIndex(m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner) && m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeConquerer == m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner)
            plr->CastSpell(plr, BG_IC_SPELL_QUARRY, TRIGGERED_OLD_TRIGGERED);
}

void BattleGroundIC::StartingEventOpenDoors()
{
    // open alliance gates
    for (const auto& i : m_keepGatesGuid[TEAM_INDEX_ALLIANCE])
        if (GameObject* pGate = GetBgMap()->GetGameObject(i))
            pGate->UseDoorOrButton();

    // open horde gates
    for (const auto& i : m_keepGatesGuid[TEAM_INDEX_HORDE])
        if (GameObject* pGate = GetBgMap()->GetGameObject(i))
            pGate->UseDoorOrButton();

    // open tower gates
    for (const auto& guid : m_towerGatesGuids)
        if (GameObject* pGate = GetBgMap()->GetGameObject(guid))
            pGate->UseDoorOrButton();

    // enable all teleporters
    for (const auto& guid : m_teleporterGuids)
        if (GameObject* pTele = GetBgMap()->GetGameObject(guid))
            pTele->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

    // enable all animations
    for (const auto& guid : m_teleporterAnimGuids)
        if (GameObject* pTele = GetBgMap()->GetGameObject(guid))
            pTele->UseDoorOrButton();

    m_closeDoorTimer = BG_IC_CLOSE_DOORS_TIME;
}

void BattleGroundIC::UpdatePlayerScore(Player* source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_playerScores.find(source->GetObjectGuid());

    if (itr == m_playerScores.end())                        // player not found...
        return;

    switch (type)
    {
        case SCORE_BASES_ASSAULTED:
            ((BattleGroundICScore*)itr->second)->basesAssaulted += value;
            break;
        case SCORE_BASES_DEFENDED:
            ((BattleGroundICScore*)itr->second)->basesDefended += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(source, type, value);
            break;
    }
}

void BattleGroundIC::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // show the reinforcements
    FillInitialWorldState(data, count, BG_IC_STATE_ALLY_REINFORCE_SHOW, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, BG_IC_STATE_HORDE_REINFORCE_SHOW, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, BG_IC_STATE_ALLY_REINFORCE_COUNT, m_reinforcements[TEAM_INDEX_ALLIANCE]);
    FillInitialWorldState(data, count, BG_IC_STATE_HORDE_REINFORCE_COUNT, m_reinforcements[TEAM_INDEX_HORDE]);

    // show the capturable bases
    for (uint8 i = 0; i < BG_IC_MAX_OBJECTIVES; ++i)
        FillInitialWorldState(data, count, m_isleNode[i].nodeWorldState, WORLD_STATE_ADD);

    // show the walls
    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        FillInitialWorldState(data, count, m_gatesAllianceState[i], WORLD_STATE_ADD);

    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
        FillInitialWorldState(data, count, m_gatesHordeState[i], WORLD_STATE_ADD);
}

// process the gate events
bool BattleGroundIC::HandleEvent(uint32 eventId, GameObject* go, Unit* invoker)
{
    DEBUG_LOG("BattleGroundIC: Handle event for gameobject entry %u.", go->GetEntry());

    for (uint8 i = 0; i < BG_IC_MAX_KEEP_GATES; ++i)
    {
        if (eventId == isleAllianceWallsData[i].eventId)
        {
            UpdateWorldState(m_gatesAllianceState[i], WORLD_STATE_REMOVE);
            m_gatesAllianceState[i] = isleAllianceWallsData[i].stateOpened;
            UpdateWorldState(m_gatesAllianceState[i], WORLD_STATE_ADD);

            if (GameObject* pGate = GetBgMap()->GetGameObject(m_keepGatesGuid[TEAM_INDEX_ALLIANCE][i]))
                pGate->UseDoorOrButton(0, true);

            SendMessage2ToAll(isleAllianceWallsData[i].message, CHAT_MSG_BG_SYSTEM_HORDE, nullptr);

            if (!m_isKeepInvaded[TEAM_INDEX_ALLIANCE])
            {
                // open the inner keep gates
                if (GameObject* pGate = GetBgMap()->GetGameObject(m_allianceInnerGate1Guid))
                    pGate->UseDoorOrButton();
                if (GameObject* pGate = GetBgMap()->GetGameObject(m_allianceInnerGate2Guid))
                    pGate->UseDoorOrButton();

                // spawn the boss
                for (const auto& i : iocAllianceKeepSpawns)
                    go->SummonCreature(i.entry, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0);

                // allow the graveyard to be captured
                if (GameObject* pFlag = GetSingleGameObjectFromStorage(BG_IC_GO_BANNER_ALLIANCE_KEEP_A))
                    pFlag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }

            m_isKeepInvaded[TEAM_INDEX_ALLIANCE] = true;

            // allow script to continue using DB scripts
            return false;
        }

        if (eventId == isleHordeWallsData[i].eventId)
        {
            UpdateWorldState(m_gatesHordeState[i], WORLD_STATE_REMOVE);
            m_gatesHordeState[i] = isleHordeWallsData[i].stateOpened;
            UpdateWorldState(m_gatesHordeState[i], WORLD_STATE_ADD);

            if (GameObject* pGate = GetBgMap()->GetGameObject(m_keepGatesGuid[TEAM_INDEX_HORDE][i]))
                pGate->UseDoorOrButton(0, true);

            SendMessage2ToAll(isleHordeWallsData[i].message, CHAT_MSG_BG_SYSTEM_ALLIANCE, nullptr);

            if (!m_isKeepInvaded[TEAM_INDEX_HORDE])
            {
                // open the inner keep gates
                if (GameObject* pGate = GetBgMap()->GetGameObject(m_hordeInnerGateGuid))
                    pGate->UseDoorOrButton();

                // spawn the boss
                for (const auto& i : iocHordeKeepSpawns)
                    go->SummonCreature(i.entry, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0);

                // allow the graveyard to be captured
                if (GameObject* pFlag = GetSingleGameObjectFromStorage(BG_IC_GO_BANNER_HORDE_KEEP_H))
                    pFlag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }

            m_isKeepInvaded[TEAM_INDEX_HORDE] = true;

            // allow script to continue using DB scripts
            return false;
        }
    }

    return false;
}

// Called when a player clicks a capturable banner
void BattleGroundIC::HandlePlayerClickedOnFlag(Player* player, GameObject* go)
{
    DEBUG_LOG("BattleGroundIC: Handle flag clicked for gameobject entry %u.", go->GetEntry());

    PvpTeamIndex newOwnerIdx = GetTeamIndexByTeamId(player->GetTeam());
    uint8 nodeId            = 0;
    uint32 newWorldState    = 0;
    uint32 soundId          = 0;
    uint32 nextFlagEntry    = 0;
    int32 textEntry         = 0;

    // *** Check if status changed from neutral to contested ***
    for (const auto& i : isleGameObjectNeutralData)
    {
        if (go->GetEntry() == i.objectEntry)
        {
            UpdatePlayerScore(player, SCORE_BASES_ASSAULTED, 1);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, 245);

            nodeId = i.objectiveId;
            newWorldState = newOwnerIdx == TEAM_INDEX_ALLIANCE ? i.nextWorldStateAlly : i.nextWorldStateHorde;

            textEntry = LANG_BG_IC_NODE_ASSAULTED;
            soundId = newOwnerIdx == TEAM_INDEX_ALLIANCE ? BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE : BG_IC_SOUND_NODE_ASSAULTED_HORDE;

            nextFlagEntry = newOwnerIdx == TEAM_INDEX_ALLIANCE ? i.nextObjectAlly : i.nextObjectHorde;
            break;
        }
    }

    if (!nodeId)
    {
        // *** Check if status changed from owned to contested ***
        for (const auto& i : isleGameObjectOwnedData)
        {
            if (go->GetEntry() == i.objectEntry)
            {
                UpdatePlayerScore(player, SCORE_BASES_ASSAULTED, 1);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, 245);

                nodeId = i.objectiveId;
                newWorldState = i.nextState;

                textEntry = LANG_BG_IC_NODE_ASSAULTED;
                soundId = newOwnerIdx == TEAM_INDEX_ALLIANCE ? BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE : BG_IC_SOUND_NODE_ASSAULTED_HORDE;

                nextFlagEntry = i.nextObject;

                // reset the objective
                DoResetObjective(IsleObjective(nodeId));
                break;
            }
        }
    }

    if (!nodeId)
    {
        // *** Check if status changed from contested to owned / contested ***
        for (const auto& i : isleGameObjectContestedData)
        {
            if (go->GetEntry() == i.objectEntry)
            {
                nodeId = i.objectiveId;

                UpdatePlayerScore(player, m_isleNode[nodeId].nodeConquerer == newOwnerIdx ? SCORE_BASES_DEFENDED : SCORE_BASES_ASSAULTED, 1);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, m_isleNode[nodeId].nodeConquerer == newOwnerIdx ? 246 : 245);

                newWorldState = m_isleNode[nodeId].nodeConquerer == newOwnerIdx ? i.nextStateDefend : i.nextStateAssault;

                textEntry = m_isleNode[nodeId].nodeConquerer == newOwnerIdx ? LANG_BG_IC_NODE_DEFENDED : LANG_BG_IC_NODE_ASSAULTED;
                soundId = newOwnerIdx == TEAM_INDEX_ALLIANCE ? BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE : BG_IC_SOUND_NODE_ASSAULTED_HORDE;

                nextFlagEntry = m_isleNode[nodeId].nodeConquerer == newOwnerIdx ? i.nextObjectDefend : i.nextObjectDefend;

                // re-apply benefits
                if (m_isleNode[nodeId].nodeConquerer == newOwnerIdx)
                    DoApplyObjectiveBenefits(IsleObjective(nodeId), go);
                break;
            }
        }
    }

    // only process the event if needed
    if (nodeId)
    {
        m_isleNode[nodeId].nodeOwner = newOwnerIdx;

        // update world states
        UpdateWorldState(m_isleNode[nodeId].nodeWorldState, WORLD_STATE_REMOVE);
        m_isleNode[nodeId].nodeWorldState = newWorldState;
        UpdateWorldState(m_isleNode[nodeId].nodeWorldState, WORLD_STATE_ADD);

        // start timer
        m_isleNode[nodeId].nodeChangeTimer = BG_IC_FLAG_CAPTURING_TIME;

        // send the zone message and sound
        ChatMsg chatSystem = newOwnerIdx == TEAM_INDEX_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE;
        uint32 factionStrig = newOwnerIdx == TEAM_INDEX_ALLIANCE ? LANG_BG_ALLY : LANG_BG_HORDE;

        SendMessage2ToAll(textEntry, chatSystem, player, isleObjectiveData[nodeId].message, factionStrig);
        PlaySoundToAll(soundId);

        // change banners
        m_isleNode[nodeId].bannerChangeTimer = 3000;
        m_isleNode[nodeId].currentBannerEntry = nextFlagEntry;
        m_isleNode[nodeId].oldBannerEntry = go->GetEntry();
    }
}

void BattleGroundIC::HandleKillUnit(Creature* creature, Player* killer)
{
    DEBUG_LOG("BattleGroundIC: Handle unit kill for creature entry %u.", creature->GetEntry());

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // end battle if boss was killed
    switch (creature->GetEntry())
    {
        case BG_IC_NPC_COMMANDER_WYRMBANE:
            RewardHonorToTeam(GetBonusHonorFromKill(BG_IC_BONUS_HONOR_BOSS_KILL), HORDE);
            EndBattleGround(HORDE);
            break;
        case BG_IC_NPC_OVERLORD_AGMAR:
            RewardHonorToTeam(GetBonusHonorFromKill(BG_IC_BONUS_HONOR_BOSS_KILL), ALLIANCE);
            EndBattleGround(ALLIANCE);
            break;
        case BG_IC_VEHICLE_DEMOLISHER:
        case BG_IC_VEHICLE_CATAPULT:
        case BG_IC_VEHICLE_GLAIVE_THROWER_A:
        case BG_IC_VEHICLE_GLAIVE_THROWER_H:
            creature->CastSpell(killer, BG_IC_SPELL_ACHIEV_DESTROYED_VEHICLE, TRIGGERED_OLD_TRIGGERED);
            break;
        case BG_IC_VEHICLE_SIEGE_ENGINE_A:
        case BG_IC_VEHICLE_SIEGE_ENGINE_H:
        {
            // kill credit
            creature->CastSpell(killer, BG_IC_SPELL_ACHIEV_DESTROYED_VEHICLE, TRIGGERED_OLD_TRIGGERED);

            PvpTeamIndex vehicleTeamIdx = creature->GetEntry() == BG_IC_VEHICLE_SIEGE_ENGINE_A ? TEAM_INDEX_ALLIANCE : TEAM_INDEX_HORDE;

            // summon a new siege engine
            if (m_isleNode[BG_IC_OBJECTIVE_WORKSHOP].nodeOwner == vehicleTeamIdx)
            {
                if (Creature* siegeEngine = creature->SummonCreature(creature->GetEntry(), iocWorkshopSpawns[0].x, iocWorkshopSpawns[0].y, iocWorkshopSpawns[0].z, iocWorkshopSpawns[0].o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[BG_IC_OBJECTIVE_WORKSHOP].creatureGuids.push_back(siegeEngine->GetObjectGuid());

                if (Creature* mechanic = GetBgMap()->GetCreature(m_workshopMechanicGuids[vehicleTeamIdx]))
                    creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_B, mechanic, mechanic);

            }
            break;
        }
    }
}

void BattleGroundIC::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);

    // remove one resource for each player killed
    PvpTeamIndex killedPlayerIdx = GetTeamIndexByTeamId(player->GetTeam());
    DoUpdateReinforcements(killedPlayerIdx, -1);

    // if reached 0, the other team wins
    if (!m_reinforcements[killedPlayerIdx])
        EndBattleGround(killer->GetTeam());
}

// Function that updates reinforcements score
void BattleGroundIC::DoUpdateReinforcements(PvpTeamIndex teamIdx, int32 value)
{
    m_reinforcements[teamIdx] = m_reinforcements[teamIdx] + value;
    UpdateWorldState(teamIdx == TEAM_INDEX_ALLIANCE ? BG_IC_STATE_ALLY_REINFORCE_COUNT : BG_IC_STATE_HORDE_REINFORCE_COUNT, m_reinforcements[teamIdx]);
}

void BattleGroundIC::HandleGameObjectDamaged(Player* player, GameObject* object, uint32 spellId)
{
    switch (spellId)
    {
        case BG_IC_SPELL_SEAFORIUM_BLAST:       player->CastSpell(player, BG_IC_SPELL_BOMB_CREDIT, TRIGGERED_OLD_TRIGGERED);        break;
        case BG_IC_SPELL_HUGE_SEAFORIUM_BLAST:  player->CastSpell(player, BG_IC_SPELL_HUGE_BOMB_CREDIT, TRIGGERED_OLD_TRIGGERED);   break;
    }
}

void BattleGroundIC::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case BG_IC_VEHICLE_KEEP_CANNON:
            creature->SetFactionTemporary(creature->GetPositionX() < 500.0f ? BG_IC_FACTION_ID_ALLIANCE : BG_IC_FACTION_ID_HORDE, TEMPFACTION_NONE);
            break;
        case BG_IC_VEHICLE_DEMOLISHER:
        {
            PvpTeamIndex ownerTeam = m_isleNode[BG_IC_OBJECTIVE_WORKSHOP].nodeOwner;
            creature->SetFactionTemporary(iocTeamFactions[ownerTeam], TEMPFACTION_NONE);
            break;
        }
        case BG_IC_VEHICLE_CATAPULT:
        {
            PvpTeamIndex ownerTeam = m_isleNode[BG_IC_OBJECTIVE_DOCKS].nodeOwner;
            creature->SetFactionTemporary(iocTeamFactions[ownerTeam], TEMPFACTION_NONE);
            break;
        }
        // store the mechanics for the respawn event
        case BG_IC_NPC_GNOMISH_MECHANIC:
            m_workshopMechanicGuids[TEAM_INDEX_ALLIANCE] = creature->GetObjectGuid();
            break;
        case BG_IC_NPC_GOBLIN_MECHANIC:
            m_workshopMechanicGuids[TEAM_INDEX_HORDE] = creature->GetObjectGuid();
            break;
        case BG_IC_NPC_BOAT_FIRE:
            m_boatFiresGuids.push_back(creature->GetObjectGuid());
            break;
    }
}

void BattleGroundIC::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_IC_GO_SEAFORIUM_BOMBS_A:
            m_seaforiumBombsGuids[TEAM_INDEX_ALLIANCE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_SEAFORIUM_BOMBS_H:
            m_seaforiumBombsGuids[TEAM_INDEX_HORDE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_PORTCULLIS_GATE_A:
            // sort each portculis gate, to use it later in script
            if (go->GetPositionX() > 400.0f)
                m_keepGatesGuid[TEAM_INDEX_ALLIANCE][BG_IC_GATE_FRONT] = go->GetObjectGuid();
            else
            {
                if (go->GetPositionY() > -800.0f)
                    m_keepGatesGuid[TEAM_INDEX_ALLIANCE][BG_IC_GATE_WEST] = go->GetObjectGuid();
                else
                    m_keepGatesGuid[TEAM_INDEX_ALLIANCE][BG_IC_GATE_EAST] = go->GetObjectGuid();
            }
            break;
        case BG_IC_GO_PORTCULLIS_GATE_H:
            // sort each portculis gate, to use it later in script
            if (go->GetPositionX() < 1200.0f)
                m_keepGatesGuid[TEAM_INDEX_HORDE][BG_IC_GATE_FRONT] = go->GetObjectGuid();
            else
            {
                if (go->GetPositionY() > -700.0f)
                    m_keepGatesGuid[TEAM_INDEX_HORDE][BG_IC_GATE_WEST] = go->GetObjectGuid();
                else
                    m_keepGatesGuid[TEAM_INDEX_HORDE][BG_IC_GATE_EAST] = go->GetObjectGuid();
            }
            break;
        case BG_IC_GO_PORTCULLIS_TOWER_A:
        case BG_IC_GO_PORTCULLIS_TOWER_H:
            m_towerGatesGuids.push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_TELEPORTER_OUTSIDE_H:
        case BG_IC_GO_TELEPORTER_INSIDE_H:
        case BG_IC_GO_TELEPORTER_OUTSIDE_A:
        case BG_IC_GO_TELEPORTER_INSIDE_A:
            m_teleporterGuids.push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_TELEPORTER_EFFECTS_H:
        case BG_IC_GO_TELEPORTER_EFFECTS_A:
            m_teleporterAnimGuids.push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_PORTCULLIS_KEEP_H:
            m_hordeInnerGateGuid = go->GetObjectGuid();
            break;
        case BG_IC_GO_PORTCULLIS_KEEP_A1:
            m_allianceInnerGate1Guid = go->GetObjectGuid();
            break;
        case BG_IC_GO_PORTCULLIS_KEEP_A2:
            m_allianceInnerGate2Guid = go->GetObjectGuid();
            break;
        case BG_IC_GO_GUNSHIP_GROUND_PORTAL_A:
            m_hangarPortalsGuids[TEAM_INDEX_ALLIANCE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_GUNSHIP_GROUND_PORTAL_H:
            m_hangarPortalsGuids[TEAM_INDEX_HORDE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_A:
            m_hangarAnimGuids[TEAM_INDEX_ALLIANCE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_GUNSHIP_PORTAL_EFFECTS_H:
            m_hangarAnimGuids[TEAM_INDEX_HORDE].push_back(go->GetObjectGuid());
            break;
        case BG_IC_GO_BANNER_ALLIANCE_KEEP_A:
        case BG_IC_GO_BANNER_ALLIANCE_KEEP_A_GREY:
        case BG_IC_GO_BANNER_ALLIANCE_KEEP_H:
        case BG_IC_GO_BANNER_ALLIANCE_KEEP_H_GREY:
        case BG_IC_GO_BANNER_HORDE_KEEP_A:
        case BG_IC_GO_BANNER_HORDE_KEEP_A_GREY:
        case BG_IC_GO_BANNER_HORDE_KEEP_H:
        case BG_IC_GO_BANNER_HORDE_KEEP_H_GREY:
        case BG_IC_GO_BANNER_WORKSHOP_A:
        case BG_IC_GO_BANNER_WORKSHOP_A_GREY:
        case BG_IC_GO_BANNER_WORKSHOP_H:
        case BG_IC_GO_BANNER_WORKSHOP_H_GREY:
        case BG_IC_GO_BANNER_DOCKS_A:
        case BG_IC_GO_BANNER_DOCKS_A_GREY:
        case BG_IC_GO_BANNER_DOCKS_H:
        case BG_IC_GO_BANNER_DOCKS_H_GREY:
        case BG_IC_GO_BANNER_HANGAR_A:
        case BG_IC_GO_BANNER_HANGAR_A_GREY:
        case BG_IC_GO_BANNER_HANGAR_H:
        case BG_IC_GO_BANNER_HANGAR_H_GREY:
        case BG_IC_GO_BANNER_REFINERY_A:
        case BG_IC_GO_BANNER_REFINERY_A_GREY:
        case BG_IC_GO_BANNER_REFINERY_H:
        case BG_IC_GO_BANNER_REFINERY_H_GREY:
        case BG_IC_GO_BANNER_QUARRY_A:
        case BG_IC_GO_BANNER_QUARRY_A_GREY:
        case BG_IC_GO_BANNER_QUARRY_H:
        case BG_IC_GO_BANNER_QUARRY_H_GREY:
        case BG_IC_GO_BANNER_DOCKS:
        case BG_IC_GO_BANNER_HANGAR:
        case BG_IC_GO_BANNER_QUARRY:
        case BG_IC_GO_BANNER_WORKSHOP:
        case BG_IC_GO_BANNER_REFINERY:
        case BG_IC_GO_GUNSHIP_A:
        case BG_IC_GO_GUNSHIP_H:
            m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
            break;
    }
}

void BattleGroundIC::EndBattleGround(Team winner)
{
    // win reward
    RewardHonorToTeam(GetBonusHonorFromKill(1), winner);

    // complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(2), HORDE);
    RewardHonorToTeam(GetBonusHonorFromKill(2), ALLIANCE);

    BattleGround::EndBattleGround(winner);
}

bool BattleGroundIC::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1)
{
    switch (criteria_id)
    {
        case BG_IC_CRIT_RESOURCE_GLUT_A:
            return m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner == TEAM_INDEX_ALLIANCE && m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner == TEAM_INDEX_ALLIANCE;
        case BG_IC_CRIT_RESOURCE_GLUT_H:
            return m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner == TEAM_INDEX_HORDE && m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner == TEAM_INDEX_HORDE;
        case BG_IC_CRIT_MINE_A_1:
        case BG_IC_CRIT_MINE_A_2:
            return m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner == TEAM_INDEX_ALLIANCE && m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner == TEAM_INDEX_ALLIANCE &&
                m_isleNode[BG_IC_OBJECTIVE_DOCKS].nodeOwner == TEAM_INDEX_ALLIANCE && m_isleNode[BG_IC_OBJECTIVE_WORKSHOP].nodeOwner == TEAM_INDEX_ALLIANCE &&
                m_isleNode[BG_IC_OBJECTIVE_HANGAR].nodeOwner == TEAM_INDEX_ALLIANCE;
        case BG_IC_CRIT_MINE_H_1:
        case BG_IC_CRIT_MINE_H_2:
            return m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner == TEAM_INDEX_HORDE && m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner == TEAM_INDEX_HORDE &&
                m_isleNode[BG_IC_OBJECTIVE_DOCKS].nodeOwner == TEAM_INDEX_HORDE && m_isleNode[BG_IC_OBJECTIVE_WORKSHOP].nodeOwner == TEAM_INDEX_HORDE &&
                m_isleNode[BG_IC_OBJECTIVE_HANGAR].nodeOwner == TEAM_INDEX_HORDE;
        case BG_IC_CRIT_MOVED_DOWN_VEHICLE:
        case BG_IC_CRIT_MOVED_DOWN_PLAYER:
            return source->IsBoarded() && source->GetTransportInfo()->GetTransport()->GetEntry() == BG_IC_VEHICLE_KEEP_CANNON;
        case BG_IC_CRIT_GLAIVE_GRAVE:
            return source->IsBoarded() && (source->GetTransportInfo()->GetTransport()->GetEntry() == BG_IC_VEHICLE_GLAIVE_THROWER_A ||
                source->GetTransportInfo()->GetTransport()->GetEntry() == BG_IC_VEHICLE_GLAIVE_THROWER_H);
    }

    return false;
}

// Method that will swap the objective banners
void BattleGroundIC::DoChangeBannerState(IsleObjective nodeId)
{
    if (GameObject* oldBanner = GetSingleGameObjectFromStorage(m_isleNode[nodeId].oldBannerEntry))
        ChangeBgObjectSpawnState(oldBanner->GetObjectGuid(), RESPAWN_ONE_DAY);

    if (GameObject* newBanner = GetSingleGameObjectFromStorage(m_isleNode[nodeId].currentBannerEntry))
        ChangeBgObjectSpawnState(newBanner->GetObjectGuid(), RESPAWN_IMMEDIATELY);
}

// Function that handles the completion of objective
void BattleGroundIC::DoCaptureObjective(IsleObjective nodeId)
{
    // Loop through the list of objects in the Conquer data list
    for (const auto& i : isleGameObjectConquerData)
    {
        if (m_isleNode[nodeId].currentBannerEntry == i.objectEntry)
        {
            PvpTeamIndex ownerIdx = m_isleNode[nodeId].nodeOwner;
            m_isleNode[nodeId].nodeConquerer = ownerIdx;

            // update world states
            UpdateWorldState(m_isleNode[nodeId].nodeWorldState, WORLD_STATE_REMOVE);
            m_isleNode[nodeId].nodeWorldState = i.nextState;
            UpdateWorldState(m_isleNode[nodeId].nodeWorldState, WORLD_STATE_ADD);

            ChatMsg chatSystem = ownerIdx == TEAM_INDEX_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE;
            uint32 factionStrig = ownerIdx == TEAM_INDEX_ALLIANCE ? LANG_BG_ALLY : LANG_BG_HORDE;

            // send zone message; the AB string is the same for IC
            SendMessage2ToAll(LANG_BG_AB_NODE_TAKEN, chatSystem, nullptr, factionStrig, isleObjectiveData[nodeId].message);

            // play sound is
            uint32 soundId = ownerIdx == TEAM_INDEX_ALLIANCE ? BG_IC_SOUND_NODE_CAPTURED_ALLIANCE : BG_IC_SOUND_NODE_CAPTURED_HORDE;
            PlaySoundToAll(soundId);

            // change banners
            m_isleNode[nodeId].bannerChangeTimer = 3000;
            m_isleNode[nodeId].oldBannerEntry = m_isleNode[nodeId].currentBannerEntry;
            m_isleNode[nodeId].currentBannerEntry = i.nextObject;

            // apply benefits
            if (GameObject* originalFlag = GetSingleGameObjectFromStorage(m_isleNode[nodeId].oldBannerEntry))
                DoApplyObjectiveBenefits(nodeId, originalFlag);

            return;
        }
    }
}

// Function that applies the objective benefits
void BattleGroundIC::DoApplyObjectiveBenefits(IsleObjective nodeId, GameObject* objRef)
{
    PvpTeamIndex ownerIdx = m_isleNode[nodeId].nodeOwner;

    // if spell is provided, apply spell
    uint32 spellId = isleObjectiveData[nodeId].spellEntry;
    if (spellId)
        DoApplyTeamBuff(ownerIdx, spellId, true);

    // if graveyard is provided, link the graveyard
    uint32 graveyardId = isleObjectiveData[nodeId].graveyardId;
    if (graveyardId)
    {
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(graveyardId, BG_IC_ZONE_ID_ISLE, GetTeamIdByTeamIndex(ownerIdx));

        // summon spirit healer if possible
        if (Creature* healer = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? BG_NPC_SPIRIT_GUIDE_ALLIANCE : BG_NPC_SPIRIT_GUIDE_HORDE, isleGraveyardData[nodeId].x, isleGraveyardData[nodeId].y, isleGraveyardData[nodeId].z, isleGraveyardData[nodeId].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_isleNode[nodeId].spiritHealerGuid = healer->GetObjectGuid();
    }

    // spawn the honor defender trigger
    if (Creature* trigger = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? BG_NPC_HON_DEFENDER_TRIGGER_A : BG_NPC_HON_DEFENDER_TRIGGER_H, isleObjectiveData[nodeId].x, isleObjectiveData[nodeId].y, isleObjectiveData[nodeId].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0))
        m_isleNode[nodeId].honorableDefenderGuid = trigger->GetObjectGuid();

    // spawn the vehicles / enable the gunship
    switch (nodeId)
    {
        case BG_IC_OBJECTIVE_WORKSHOP:
        {
            // summon the vehicles and the mechanic
            for (const auto& i : iocWorkshopSpawns)
            {
                if (Creature* creature = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[nodeId].creatureGuids.push_back(creature->GetObjectGuid());
            }

            // respawn the workshop bombs and give them the right faction
            for (const auto& guid : m_seaforiumBombsGuids[ownerIdx])
                ChangeBgObjectSpawnState(guid, RESPAWN_IMMEDIATELY);

            break;
        }
        case BG_IC_OBJECTIVE_DOCKS:
        {
            // summon the docks vehicles
            for (const auto& i : iocDocksSpawns)
            {
                if (Creature* creature = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[nodeId].creatureGuids.push_back(creature->GetObjectGuid());
            }

            // spawn boats for visual reference
            for (const auto& i : iocBoatsSpawns)
            {
                if (Creature* creature = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0, true, true))
                    m_isleNode[nodeId].specialCreatureGuid = creature->GetObjectGuid();
            }
            break;
        }
        case BG_IC_OBJECTIVE_HANGAR:
        {
            // respawn portals
            for (const auto& guid : m_hangarPortalsGuids[ownerIdx])
                ChangeBgObjectSpawnState(guid, RESPAWN_IMMEDIATELY);

            // respawn and enable the animations
            for (const auto& guid : m_hangarAnimGuids[ownerIdx])
            {
                ChangeBgObjectSpawnState(guid, RESPAWN_IMMEDIATELY);

                if (GameObject* pAnim = GetBgMap()->GetGameObject(guid))
                    pAnim->UseDoorOrButton();
            }

            // start the gunship
            if (GenericTransport* gunship = GetBgMap()->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(iocGunships[ownerIdx]))))
                gunship->SetGoState(GO_STATE_ACTIVE);

            // spawn the captain and start wp movement for script purpose
            for (const auto& i : iocHangarSpawns)
            {
                if (Creature* creature = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    creature->GetMotionMaster()->MoveWaypoint();
            }
            break;
        }
        case BG_IC_OBJECTIVE_REFINERY:
        {
            // summon the refinery npcs
            for (const auto& i : iocRefinerySpawns)
            {
                if (Creature* creature = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[nodeId].creatureGuids.push_back(creature->GetObjectGuid());
            }

            m_resourceTickTimer[BG_IC_RESOURCE_REFINERY] = BG_IC_RESOURCE_TICK_TIMER;
            break;
        }
        case BG_IC_OBJECTIVE_QUARY:
        {
            m_resourceTickTimer[BG_IC_RESOURCE_QUARY] = BG_IC_RESOURCE_TICK_TIMER;
            break;
        }
        case BG_IC_OBJECTIVE_KEEP_ALLY:
        {
            // summon extra honor triggers
            for (const auto& i : iocHonorTriggerAllySpawns)
            {
                if (Creature* trigger = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[nodeId].creatureGuids.push_back(trigger->GetObjectGuid());
            }
            break;
        }
        case BG_IC_OBJECTIVE_KEEP_HORDE:
        {
            // summon extra honor triggers
            for (const auto& i : iocHonorTriggerHordeSpawns)
            {
                if (Creature* trigger = objRef->SummonCreature(ownerIdx == TEAM_INDEX_ALLIANCE ? i.entryAlly : i.entryHorde, i.x, i.y, i.z, i.o, TEMPSPAWN_DEAD_DESPAWN, 0))
                    m_isleNode[nodeId].creatureGuids.push_back(trigger->GetObjectGuid());
            }
            break;
        }

        default:
            break;
    }
}

// Function that handles the reset of objective
void BattleGroundIC::DoResetObjective(IsleObjective nodeId)
{
    PvpTeamIndex ownerIdx = m_isleNode[nodeId].nodeOwner;

    // despawn the main honor trigger
    if (Creature* trigger = GetBgMap()->GetCreature(m_isleNode[nodeId].honorableDefenderGuid))
        trigger->ForcedDespawn();

    // reset graveyard
    uint32 graveyardId = isleObjectiveData[nodeId].graveyardId;
    if (graveyardId)
    {
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(graveyardId, BG_IC_ZONE_ID_ISLE, TEAM_INVALID);

        // despawn healer
        if (Creature* healer = GetBgMap()->GetCreature(m_isleNode[nodeId].spiritHealerGuid))
        {
            healer->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, healer, healer);
            healer->ForcedDespawn();
        }
    }

    // remove spell aura buff
    uint32 spellId = isleObjectiveData[nodeId].spellEntry;
    if (spellId)
        DoApplyTeamBuff(ownerIdx, spellId, false);

    switch (nodeId)
    {
        case BG_IC_OBJECTIVE_WORKSHOP:
        {
            // despwn bombs
            for (const auto& guid : m_seaforiumBombsGuids[ownerIdx])
                ChangeBgObjectSpawnState(guid, RESPAWN_ONE_DAY);

            // despwn the vehicles if not already in use
            for (const auto& guid : m_isleNode[nodeId].creatureGuids)
            {
                if (Creature* creature = GetBgMap()->GetCreature(guid))
                {
                    if (!creature->IsVehicle() || (creature->IsVehicle() && !creature->GetVehicleInfo()->GetPassenger(0)))
                        creature->ForcedDespawn();
                }
            }

            m_isleNode[nodeId].creatureGuids.clear();
            break;
        }
        case BG_IC_OBJECTIVE_DOCKS:
        {
            // despwn the vehicles if not already in use
            for (const auto& guid : m_isleNode[nodeId].creatureGuids)
            {
                if (Creature* vehicle = GetBgMap()->GetCreature(guid))
                {
                    if (!vehicle->GetVehicleInfo()->GetPassenger(0))
                        vehicle->ForcedDespawn();
                }
            }

            // boat fire animations and despawn
            for (const auto& guid : m_boatFiresGuids)
            {
                if (Creature* fire = GetBgMap()->GetCreature(guid))
                {
                    fire->CastSpell(fire, BG_IC_SPELL_BOAT_FIRE, TRIGGERED_OLD_TRIGGERED);
                    fire->ForcedDespawn(10000);
                }
            }

            // despawn boat and let it sink
            if (Creature* boat = GetBgMap()->GetCreature(m_isleNode[nodeId].specialCreatureGuid))
            {
                boat->SetLevitate(false);
                boat->ForcedDespawn(10000);
            }

            m_boatFiresGuids.clear();
            m_isleNode[nodeId].creatureGuids.clear();
            break;
        }
        case BG_IC_OBJECTIVE_HANGAR:
        {
            // reset the animations and depswn the portals
            for (const auto& guid : m_hangarPortalsGuids[ownerIdx])
                ChangeBgObjectSpawnState(guid, RESPAWN_ONE_DAY);

            for (const auto& guid : m_hangarAnimGuids[ownerIdx])
            {
                ChangeBgObjectSpawnState(guid, RESPAWN_ONE_DAY);

                if (GameObject* anim = GetBgMap()->GetGameObject(guid))
                    anim->ResetDoorOrButton();
            }

            // stop the gunship
            if (GenericTransport* gunship = GetBgMap()->GetTransport(ObjectGuid(HIGHGUID_MO_TRANSPORT, uint32(iocGunships[ownerIdx]))))
                gunship->SetGoState(GO_STATE_READY);

            break;
        }
        case BG_IC_OBJECTIVE_REFINERY:
        {
            for (const auto& guid : m_isleNode[nodeId].creatureGuids)
                if (Creature* creature = GetBgMap()->GetCreature(guid))
                    creature->ForcedDespawn();

            m_resourceTickTimer[BG_IC_RESOURCE_REFINERY] = 0;
            break;
        }
        case BG_IC_OBJECTIVE_QUARY:
        {
            m_resourceTickTimer[BG_IC_RESOURCE_QUARY] = 0;
            break;
        }
        case BG_IC_OBJECTIVE_KEEP_ALLY:
        case BG_IC_OBJECTIVE_KEEP_HORDE:
        {
            // despawn honor triggers inside the keep
            for (const auto& guid : m_isleNode[nodeId].creatureGuids)
                if (Creature* trigger = GetBgMap()->GetCreature(guid))
                    trigger->ForcedDespawn();

            break;
        }
        default:
            break;
    }
}

// Function that applies and removes buff aura from vehicles and players
void BattleGroundIC::DoApplyTeamBuff(PvpTeamIndex teamIdx, uint32 spellEntry, bool apply)
{
    Team teamId = GetTeamIdByTeamIndex(teamIdx);

    // Note: on vehicles the auras is applied by the forced trigger spell
    if (apply)
        CastSpellOnTeam(spellEntry, teamId);
    else
    {
        for (BattleGroundPlayerMap::const_iterator itr = m_players.begin(); itr != m_players.end(); ++itr)
        {
            if (Player* player = sObjectMgr.GetPlayer(itr->first))
            {
                if (player->GetTeam() == teamId)
                    player->RemoveAurasDueToSpell(spellEntry);
            }
        }
    }
}

void BattleGroundIC::Update(uint32 diff)
{
    BattleGround::Update(diff);

    // close keep gates
    if (m_closeDoorTimer)
    {
        if (m_closeDoorTimer <= diff)
        {
            //  reset alliance and horde gates
            for (const auto& i : m_keepGatesGuid[TEAM_INDEX_ALLIANCE])
                if (GameObject* pGate = GetBgMap()->GetGameObject(i))
                    pGate->ResetDoorOrButton();

            for (const auto& i : m_keepGatesGuid[TEAM_INDEX_HORDE])
                if (GameObject* pGate = GetBgMap()->GetGameObject(i))
                    pGate->ResetDoorOrButton();

            m_closeDoorTimer = 0;
        }
        else
            m_closeDoorTimer -= diff;
    }

    // objective timers
    for (uint8 i = 0; i < BG_IC_MAX_OBJECTIVES; ++i)
    {
        // timer to change the node owner
        if (m_isleNode[i].nodeChangeTimer)
        {
            if (m_isleNode[i].nodeChangeTimer <= diff)
            {
                DoCaptureObjective(IsleObjective(i));
                m_isleNode[i].nodeChangeTimer = 0;
            }
            else
                m_isleNode[i].nodeChangeTimer -= diff;
        }

        // timer to change the node banner
        if (m_isleNode[i].bannerChangeTimer)
        {
            if (m_isleNode[i].bannerChangeTimer <= diff)
            {
                DoChangeBannerState(IsleObjective(i));
                m_isleNode[i].bannerChangeTimer = 0;
            }
            else
                m_isleNode[i].bannerChangeTimer -= diff;
        }
    }

    // resource timers
    for (uint8 i = 0; i < BG_IC_MAX_RESOURCE_NODES; ++i)
    {
        if (m_resourceTickTimer[i])
        {
            if (m_resourceTickTimer[i] <= diff)
            {
                switch (i)
                {
                    case BG_IC_RESOURCE_REFINERY:
                    {
                        PvpTeamIndex index = m_isleNode[BG_IC_OBJECTIVE_REFINERY].nodeOwner;

                        DoUpdateReinforcements(index, 1);
                        RewardHonorToTeam(GetBonusHonorFromKill(1), GetTeamIdByTeamIndex(index));
                        break;
                    }
                    case BG_IC_RESOURCE_QUARY:
                    {
                        PvpTeamIndex index = m_isleNode[BG_IC_OBJECTIVE_QUARY].nodeOwner;

                        DoUpdateReinforcements(index, 1);
                        RewardHonorToTeam(GetBonusHonorFromKill(1), GetTeamIdByTeamIndex(index));
                        break;
                    }
                }
                m_resourceTickTimer[i] = BG_IC_RESOURCE_TICK_TIMER;
            }
            else
                m_resourceTickTimer[i] -= diff;
        }
    }
}
