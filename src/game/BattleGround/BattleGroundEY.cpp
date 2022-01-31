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

#include "Entities/Object.h"
#include "Entities/Player.h"
#include "BattleGround.h"
#include "BattleGroundEY.h"
#include "Entities/Creature.h"
#include "Globals/ObjectMgr.h"
#include "BattleGroundMgr.h"
#include "Tools/Language.h"
#include "WorldPacket.h"
#include "Util.h"
#include "Maps/MapManager.h"
#include "AI/ScriptDevAI/include/sc_grid_searchers.h"

BattleGroundEY::BattleGroundEY(): m_flagState(), m_towersAlliance(0), m_towersHorde(0), m_honorTicks(0), m_flagRespawnTimer(0), m_resourceUpdateTimer(0), m_felReaverFlagTimer(0)
{
    // set battleground start message id
    m_startMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_startMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_EY_START_ONE_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_EY_START_HALF_MINUTE;
    m_startMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_EY_HAS_BEGUN;
}

void BattleGroundEY::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // resource counter
    if (m_resourceUpdateTimer < diff)
    {
        UpdateResources();
        m_resourceUpdateTimer = EY_RESOURCES_UPDATE_TIME;
    }
    else
        m_resourceUpdateTimer -= diff;

    // flag respawn
    if (m_flagState == EY_FLAG_STATE_WAIT_RESPAWN || m_flagState == EY_FLAG_STATE_ON_GROUND)
    {
        if (m_flagRespawnTimer < diff)
        {
            m_flagRespawnTimer = 0;

            if (m_flagState == EY_FLAG_STATE_WAIT_RESPAWN)
                RespawnFlagAtCenter(true);
            else
                RespawnDroppedFlag();
        }
        else
            m_flagRespawnTimer -= diff;
    }

    // workaround for Fel Reaver Ruins flag capture needed on 3.3.5 only
    // the original areatrigger (4514) is covered by a bigger one (4515) and is not triggered on client side
    if (IsFlagPickedUp())
    {
        if (m_felReaverFlagTimer < diff)
        {
            Player* flagCarrier = sObjectMgr.GetPlayer(GetFlagCarrierGuid());
            if (flagCarrier)
            {
                if (m_towerOwner[NODE_FEL_REAVER_RUINS] == flagCarrier->GetTeam())
                {
                    // coords and range taken from DBC of areatrigger (4514)
                    if (flagCarrier->IsWithinDist3d(2044.0f, 1729.729f, 1190.03f, 3.0f))
                        ProcessPlayerFlagScoreEvent(flagCarrier, NODE_FEL_REAVER_RUINS);
                }
            }
            m_felReaverFlagTimer = EY_FEL_REAVER_FLAG_UPDATE_TIME;
        }
        else
            m_felReaverFlagTimer -= diff;
    }
}

void BattleGroundEY::StartingEventOpenDoors()
{
    // eye-doors have a despawn animation
    OpenDoorEvent(BG_EVENT_DOOR);

    // Players that join battleground after start are not eligible to get achievement.
    StartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG, EY_TIMED_ACHIEV_FLURRY);

    // setup graveyards
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(GRAVEYARD_EY_MAIN_ALLIANCE, EY_ZONE_ID_MAIN, ALLIANCE);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(GRAVEYARD_EY_MAIN_HORDE, EY_ZONE_ID_MAIN, HORDE);
}

void BattleGroundEY::AddPoints(Team team, uint32 points)
{
    PvpTeamIndex team_index = GetTeamIndexByTeamId(team);
    m_teamScores[team_index] += points;
    m_honorScoreTicks[team_index] += points;

    if (m_honorScoreTicks[team_index] >= m_honorTicks)
    {
        RewardHonorToTeam(GetBonusHonorFromKill(1), team);
        m_honorScoreTicks[team_index] -= m_honorTicks;
    }
}

// Update resources by team based on the owned towers
void BattleGroundEY::UpdateResources()
{
    if (m_towersAlliance > 0)
    {
        AddPoints(ALLIANCE, eyTickPoints[m_towersAlliance - 1]);
        UpdateTeamScore(ALLIANCE);
    }

    if (m_towersHorde > 0)
    {
        AddPoints(HORDE, eyTickPoints[m_towersHorde - 1]);
        UpdateTeamScore(HORDE);
    }
}

// Update team score
void BattleGroundEY::UpdateTeamScore(Team team)
{
    uint32 score = m_teamScores[GetTeamIndexByTeamId(team)];

    if (score >= EY_MAX_TEAM_SCORE)
    {
        score = EY_MAX_TEAM_SCORE;
        EndBattleGround(team);
    }

    // Update world states
    UpdateWorldState(team == ALLIANCE ? WORLD_STATE_EY_RESOURCES_ALLIANCE : WORLD_STATE_EY_RESOURCES_HORDE, score);
}

// Handle battleground end
void BattleGroundEY::EndBattleGround(Team winner)
{
    // win reward
    RewardHonorToTeam(GetBonusHonorFromKill(1), winner);

    // complete map reward
    RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    // disable capture points
    for (auto m_tower : m_towers)
        if (GameObject* go = GetBgMap()->GetGameObject(m_tower))
            go->SetLootState(GO_JUST_DEACTIVATED);

    BattleGround::EndBattleGround(winner);
}

void BattleGroundEY::AddPlayer(Player* player)
{
    BattleGround::AddPlayer(player);

    // create score and add it to map
    BattleGroundEYScore* score = new BattleGroundEYScore;

    m_playerScores[player->GetObjectGuid()] = score;
}

void BattleGroundEY::RemovePlayer(Player* player, ObjectGuid guid)
{
    // sometimes flag aura not removed :(
    if (IsFlagPickedUp())
    {
        if (m_flagCarrier == guid)
        {
            if (player)
                HandlePlayerDroppedFlag(player);
            else
            {
                sLog.outError("BattleGroundEY: Removing offline player who unexpectendly carries the flag!");

                ClearFlagCarrier();
                RespawnFlagAtCenter(false);
            }
        }
    }
}

void BattleGroundEY::HandleGameObjectCreate(GameObject* go)
{
    // set initial data and activate capture points
    switch (go->GetEntry())
    {
        case GO_CAPTURE_POINT_BLOOD_ELF_TOWER:
            m_towers[NODE_BLOOD_ELF_TOWER] = go->GetObjectGuid();
            go->SetCapturePointSlider(CAPTURE_SLIDER_MIDDLE, false);
            break;
        case GO_CAPTURE_POINT_FEL_REAVER_RUINS:
            m_towers[NODE_FEL_REAVER_RUINS] = go->GetObjectGuid();
            go->SetCapturePointSlider(CAPTURE_SLIDER_MIDDLE, false);
            break;
        case GO_CAPTURE_POINT_MAGE_TOWER:
            m_towers[NODE_MAGE_TOWER] = go->GetObjectGuid();
            go->SetCapturePointSlider(CAPTURE_SLIDER_MIDDLE, false);
            break;
        case GO_CAPTURE_POINT_DRAENEI_RUINS:
            m_towers[NODE_DRAENEI_RUINS] = go->GetObjectGuid();
            go->SetCapturePointSlider(CAPTURE_SLIDER_MIDDLE, false);
            break;
        case GO_EY_NETHERSTORM_FLAG_DROP:
            m_droppedFlagGuid = go->GetObjectGuid();
            break;
        case GO_EY_NETHERSTORM_FLAG:
            m_mainFlagDbGuid = go->GetDbGuid();
            break;
    }
}

// process the capture events
bool BattleGroundEY::HandleEvent(uint32 eventId, GameObject* go, Unit* invoker)
{
    // event called when player picks up a dropped flag
    if (eventId == EVENT_NETHERSTORM_FLAG_PICKUP && invoker->GetTypeId() == TYPEID_PLAYER)
    {
        DEBUG_LOG("BattleGroundEY: Handle flag pickup event id %u", eventId);

        HandlePlayerClickedOnFlag((Player*)invoker, go);
        return true;
    }

    // events called from the capture points
    for (uint8 i = 0; i < EY_MAX_NODES; ++i)
    {
        if (eyTowers[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (eyTowerEvents[i][j].eventEntry == eventId)
                {
                    ProcessCaptureEvent(go, i, eyTowerEvents[i][j].team, eyTowerEvents[i][j].worldState, eyTowerEvents[i][j].message);

                    // no need to iterate other events or towers
                    return false;
                }
            }
            // no need to iterate other towers
            return false;
        }
    }

    return false;
}

// Method that handles the capture point capture events
void BattleGroundEY::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState, uint32 message)
{
    DEBUG_LOG("BattleGroundEY: Process capture point event from gameobject entry %u, captured by team %u", go->GetEntry(), team);

    if (team == ALLIANCE)
    {
        // update counter
        ++m_towersAlliance;
        UpdateWorldState(WORLD_STATE_EY_TOWER_COUNT_ALLIANCE, m_towersAlliance);

        SendMessageToAll(message, CHAT_MSG_BG_SYSTEM_ALLIANCE);

        // link graveyard
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(eyeGraveyardData[towerId].id, EY_ZONE_ID_MAIN, ALLIANCE);

        // spawn spirit healer and defender
        if (Creature* healer = go->SummonCreature(BG_NPC_SPIRIT_GUIDE_ALLIANCE, eyeGraveyardData[towerId].x, eyeGraveyardData[towerId].y, eyeGraveyardData[towerId].z, eyeGraveyardData[towerId].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_spiritHealers[towerId] = healer->GetObjectGuid();
        if (Creature* defender = go->SummonCreature(BG_NPC_HON_DEFENDER_TRIGGER_A, eyeTowerdData[towerId].x, eyeTowerdData[towerId].y, eyeTowerdData[towerId].z, eyeTowerdData[towerId].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_honorableDefender[towerId] = defender->GetObjectGuid();
    }
    else if (team == HORDE)
    {
        // update counter
        ++m_towersHorde;
        UpdateWorldState(WORLD_STATE_EY_TOWER_COUNT_HORDE, m_towersHorde);

        SendMessageToAll(message, CHAT_MSG_BG_SYSTEM_HORDE);

        // link graveyard
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(eyeGraveyardData[towerId].id, EY_ZONE_ID_MAIN, HORDE);

        // spawn spirit healer and defender
        if (Creature* healer = go->SummonCreature(BG_NPC_SPIRIT_GUIDE_HORDE, eyeGraveyardData[towerId].x, eyeGraveyardData[towerId].y, eyeGraveyardData[towerId].z, eyeGraveyardData[towerId].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_spiritHealers[towerId] = healer->GetObjectGuid();
        if (Creature* defender = go->SummonCreature(BG_NPC_HON_DEFENDER_TRIGGER_H, eyeTowerdData[towerId].x, eyeTowerdData[towerId].y, eyeTowerdData[towerId].z, eyeTowerdData[towerId].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            m_honorableDefender[towerId] = defender->GetObjectGuid();
    }
    else
    {
        if (m_towerOwner[towerId] == ALLIANCE)
        {
            // update counter
            --m_towersAlliance;
            UpdateWorldState(WORLD_STATE_EY_TOWER_COUNT_ALLIANCE, m_towersAlliance);

            SendMessageToAll(message, CHAT_MSG_BG_SYSTEM_ALLIANCE);
        }
        else
        {
            // update counter
            --m_towersHorde;
            UpdateWorldState(WORLD_STATE_EY_TOWER_COUNT_HORDE, m_towersHorde);

            SendMessageToAll(message, CHAT_MSG_BG_SYSTEM_HORDE);
        }

        // unlink graveyard
        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(eyeGraveyardData[towerId].id, EY_ZONE_ID_MAIN, TEAM_INVALID);

        // despawn spirit healer and defender
        if (Creature* healer = GetBgMap()->GetCreature(m_spiritHealers[towerId]))
            healer->ForcedDespawn();
        if (Creature* defender = GetBgMap()->GetCreature(m_honorableDefender[towerId]))
            defender->ForcedDespawn();
    }

    // update tower state
    UpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    UpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // update capture point owner
    Team oldTeam = m_towerOwner[towerId];
    m_towerOwner[towerId] = team;

    if (oldTeam == ALLIANCE || oldTeam == HORDE) // only on going to grey
    {
        // teleport players off of GY
        Creature* spiritHealer = GetClosestCreatureWithEntry(go, oldTeam == ALLIANCE ? BG_NPC_SPIRIT_GUIDE_ALLIANCE : BG_NPC_SPIRIT_GUIDE_HORDE, 100.f);

        if (spiritHealer)
            spiritHealer->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, spiritHealer, spiritHealer);
    }

    // replace the visual flags
    switch (team)
    {
        case ALLIANCE:  SpawnEvent(towerId, TEAM_INDEX_ALLIANCE, true); break;
        case HORDE:     SpawnEvent(towerId, TEAM_INDEX_HORDE,    true); break;
        case TEAM_NONE: SpawnEvent(towerId, TEAM_INDEX_NEUTRAL,  true); break;
        default: break;
    }
}

// Function that handles area triggers; used to check for flag carriers
bool BattleGroundEY::HandleAreaTrigger(Player* source, uint32 trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return false;

    if (!source->IsAlive())                                 // hack code, must be removed later
        return false;

    switch (trigger)
    {
        case AREATRIGGER_BLOOD_ELF_TOWER_POINT:
            if (m_towerOwner[NODE_BLOOD_ELF_TOWER] == source->GetTeam())
                ProcessPlayerFlagScoreEvent(source, NODE_BLOOD_ELF_TOWER);
            break;
        case AREATRIGGER_FEL_REAVER_RUINS_POINT:
            if (m_towerOwner[NODE_FEL_REAVER_RUINS] == source->GetTeam())
                ProcessPlayerFlagScoreEvent(source, NODE_FEL_REAVER_RUINS);
            break;
        case AREATRIGGER_MAGE_TOWER_POINT:
            if (m_towerOwner[NODE_MAGE_TOWER] == source->GetTeam())
                ProcessPlayerFlagScoreEvent(source, NODE_MAGE_TOWER);
            break;
        case AREATRIGGER_DRAENEI_RUINS_POINT:
            if (m_towerOwner[NODE_DRAENEI_RUINS] == source->GetTeam())
                ProcessPlayerFlagScoreEvent(source, NODE_DRAENEI_RUINS);
            break;
        default:
            return false;
    }
    return true;
}

void BattleGroundEY::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    m_teamScores[TEAM_INDEX_ALLIANCE] = 0;
    m_teamScores[TEAM_INDEX_HORDE] = 0;

    m_towersAlliance = 0;
    m_towersHorde = 0;

    m_honorTicks = BattleGroundMgr::IsBgWeekend(GetTypeId()) ? EY_WEEKEND_HONOR_INTERVAL : EY_NORMAL_HONOR_INTERVAL;
    m_honorScoreTicks[TEAM_INDEX_ALLIANCE] = 0;
    m_honorScoreTicks[TEAM_INDEX_HORDE] = 0;

    m_flagState = EY_FLAG_STATE_ON_BASE;
    m_flagCarrier.Clear();
    m_droppedFlagGuid.Clear();

    m_flagRespawnTimer = 0;
    m_resourceUpdateTimer = 0;
    m_felReaverFlagTimer = 0;

    m_towerWorldState[NODE_BLOOD_ELF_TOWER] = WORLD_STATE_EY_BLOOD_ELF_TOWER_NEUTRAL;
    m_towerWorldState[NODE_FEL_REAVER_RUINS] = WORLD_STATE_EY_FEL_REAVER_RUINS_NEUTRAL;
    m_towerWorldState[NODE_MAGE_TOWER] = WORLD_STATE_EY_MAGE_TOWER_NEUTRAL;
    m_towerWorldState[NODE_DRAENEI_RUINS] = WORLD_STATE_EY_DRAENEI_RUINS_NEUTRAL;

    for (uint8 i = 0; i < EY_MAX_NODES; ++i)
    {
        m_towerOwner[i] = TEAM_NONE;
        m_activeEvents[i] = TEAM_INDEX_NEUTRAL;

        GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(eyeGraveyardData[i].id, EY_ZONE_ID_MAIN, TEAM_INVALID);
    }

    // setup graveyards
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(GRAVEYARD_EY_MAIN_ALLIANCE, EY_ZONE_ID_MAIN, ALLIANCE);
    GetBgMap()->GetGraveyardManager().SetGraveYardLinkTeam(GRAVEYARD_EY_MAIN_HORDE, EY_ZONE_ID_MAIN, HORDE);

    // the flag in the middle is spawned at beginning
    m_activeEvents[EY_EVENT_CAPTURE_FLAG] = EY_EVENT2_FLAG_CENTER;
}

// Method that resets main flag at the center
void BattleGroundEY::RespawnFlagAtCenter(bool wasCaptured)
{
    DEBUG_LOG("BattleGroundEY: Respawn flag at the center of the battleground.");

    m_flagState = EY_FLAG_STATE_ON_BASE;

    // will despawn captured flags at the node and spawn in center
    if (wasCaptured)
        SpawnEvent(EY_EVENT_CAPTURE_FLAG, EY_EVENT2_FLAG_CENTER, true);
    // respawn because of flag drop has to be handled separately
    else
        ChangeBgObjectSpawnState(m_mainFlagDbGuid, RESPAWN_IMMEDIATELY);

    PlaySoundToAll(EY_SOUND_FLAG_RESET);
    SendMessageToAll(LANG_BG_EY_RESETED_FLAG, CHAT_MSG_BG_SYSTEM_NEUTRAL);

    UpdateWorldState(WORLD_STATE_EY_NETHERSTORM_FLAG_READY, WORLD_STATE_ADD);
}

// Method that respawns dropped flag; called if nobody picks the dropped flag after 10 seconds
void BattleGroundEY::RespawnDroppedFlag()
{
    RespawnFlagAtCenter(false);

    if (GameObject* flag = GetBgMap()->GetGameObject(m_droppedFlagGuid))
        flag->SetLootState(GO_JUST_DEACTIVATED);

    m_droppedFlagGuid.Clear();
}

void BattleGroundEY::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);

    // handle flag drop if necessary
    HandlePlayerDroppedFlag(player);
}

// Handle flag drop
void BattleGroundEY::HandlePlayerDroppedFlag(Player* source)
{
    if (!IsFlagPickedUp())
        return;

    if (GetFlagCarrierGuid() != source->GetObjectGuid())
        return;

    DEBUG_LOG("BattleGroundEY: Team %u has dropped the flag.", source->GetTeam());

    ClearFlagCarrier();
    source->RemoveAurasDueToSpell(EY_SPELL_NETHERSTORM_FLAG);

    // do not cast auras or send messages after match has ended
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    m_flagState = EY_FLAG_STATE_ON_GROUND;
    m_flagRespawnTimer = EY_FLAG_RESPAWN_TIME;

    source->CastSpell(source, SPELL_RECENTLY_DROPPED_FLAG, TRIGGERED_OLD_TRIGGERED);
    source->CastSpell(source, EY_SPELL_PLAYER_DROPPED_FLAG, TRIGGERED_OLD_TRIGGERED);

    Team playerTeam = source->GetTeam();

    UpdateWorldState(playerTeam == ALLIANCE ? WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_ALLIANCE : WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_HORDE, WORLD_STATE_ADD);
    SendMessageToAll(LANG_BG_EY_DROPPED_FLAG, playerTeam == ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, nullptr);
}

// Method that handles when a player clicks on the flag
void BattleGroundEY::HandlePlayerClickedOnFlag(Player* source, GameObject* go)
{
    if (GetStatus() != STATUS_IN_PROGRESS || IsFlagPickedUp() || !source->IsWithinDistInMap(go, 10))
        return;

    DEBUG_LOG("BattleGroundEY: Team %u has taken the flag, gameobject entry %u.", source->GetTeam(), go->GetEntry());

    // remove world state for base flag
    if (m_flagState == EY_FLAG_STATE_ON_BASE)
        UpdateWorldState(WORLD_STATE_EY_NETHERSTORM_FLAG_READY, WORLD_STATE_REMOVE);

    // update for the team that picked up the flag
    if (source->GetTeam() == ALLIANCE)
    {
        PlaySoundToAll(EY_SOUND_FLAG_PICKED_UP_ALLIANCE);

        m_flagState = EY_FLAG_STATE_ON_ALLIANCE_PLAYER;
        UpdateWorldState(WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_ALLIANCE, 2);
    }
    else if (source->GetTeam() == HORDE)
    {
        PlaySoundToAll(EY_SOUND_FLAG_PICKED_UP_HORDE);

        m_flagState = EY_FLAG_STATE_ON_HORDE_PLAYER;
        UpdateWorldState(WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_HORDE, 2);
    }

    // Note: flag despawn and spell cast are handled in GameObject code
    // Set flag carrier and set right auras 
    SetFlagCarrier(source->GetObjectGuid());
    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_PVP_ACTIVE_CANCELS);

    // send message
    if (source->GetTeam() == ALLIANCE)
        PSendMessageToAll(LANG_BG_EY_HAS_TAKEN_FLAG, CHAT_MSG_BG_SYSTEM_ALLIANCE, nullptr, source->GetName());
    else
        PSendMessageToAll(LANG_BG_EY_HAS_TAKEN_FLAG, CHAT_MSG_BG_SYSTEM_HORDE, nullptr, source->GetName());
}

// Method that handles the player score when flag is captured at one of controlled nodes
void BattleGroundEY::ProcessPlayerFlagScoreEvent(Player* source, EYNodes node)
{
    if (GetStatus() != STATUS_IN_PROGRESS || GetFlagCarrierGuid() != source->GetObjectGuid())
        return;

    DEBUG_LOG("BattleGroundEY: Team %u has successfully delivered the flag to the node id %u", source->GetTeam(), node);

    ClearFlagCarrier();

    m_flagState = EY_FLAG_STATE_WAIT_RESPAWN;
    m_flagRespawnTimer = EY_FLAG_RESPAWN_TIME;

    source->RemoveAurasDueToSpell(EY_SPELL_NETHERSTORM_FLAG);
    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_PVP_ACTIVE_CANCELS);

    // process score actions by team
    if (source->GetTeam() == ALLIANCE)
    {
        PlaySoundToAll(EY_SOUND_FLAG_CAPTURED_ALLIANCE);

        if (m_towersAlliance > 0)
            AddPoints(ALLIANCE, eyFlagPoints[m_towersAlliance - 1]);

        SendMessageToAll(LANG_BG_EY_CAPTURED_FLAG_A, CHAT_MSG_BG_SYSTEM_ALLIANCE, source);
    }
    else
    {
        PlaySoundToAll(EY_SOUND_FLAG_CAPTURED_HORDE);

        if (m_towersHorde > 0)
            AddPoints(HORDE, eyFlagPoints[m_towersHorde - 1]);

        SendMessageToAll(LANG_BG_EY_CAPTURED_FLAG_H, CHAT_MSG_BG_SYSTEM_HORDE, source);
    }

    SpawnEvent(EY_EVENT_CAPTURE_FLAG, node, true);

    UpdatePlayerScore(source, SCORE_FLAG_CAPTURES, 1);

    // update achievement
    source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, 183);
}

void BattleGroundEY::UpdatePlayerScore(Player* source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_playerScores.find(source->GetObjectGuid());
    if (itr == m_playerScores.end())                        // player not found
        return;

    switch (type)
    {
        case SCORE_FLAG_CAPTURES:                           // flags captured
            ((BattleGroundEYScore*)itr->second)->flagCaptures += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(source, type, value);
            break;
    }
}

void BattleGroundEY::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    // counter states
    FillInitialWorldState(data, count, WORLD_STATE_EY_TOWER_COUNT_ALLIANCE, m_towersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_EY_TOWER_COUNT_HORDE, m_towersHorde);

    FillInitialWorldState(data, count, WORLD_STATE_EY_RESOURCES_ALLIANCE, m_teamScores[TEAM_INDEX_ALLIANCE]);
    FillInitialWorldState(data, count, WORLD_STATE_EY_RESOURCES_HORDE, m_teamScores[TEAM_INDEX_HORDE]);

    // tower world states
    FillInitialWorldState(data, count, WORLD_STATE_EY_BLOOD_ELF_TOWER_ALLIANCE, m_towerOwner[NODE_BLOOD_ELF_TOWER] == ALLIANCE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_BLOOD_ELF_TOWER_HORDE, m_towerOwner[NODE_BLOOD_ELF_TOWER] == HORDE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_BLOOD_ELF_TOWER_NEUTRAL, m_towerOwner[NODE_BLOOD_ELF_TOWER] == TEAM_NONE);

    FillInitialWorldState(data, count, WORLD_STATE_EY_FEL_REAVER_RUINS_ALLIANCE, m_towerOwner[NODE_FEL_REAVER_RUINS] == ALLIANCE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_FEL_REAVER_RUINS_HORDE, m_towerOwner[NODE_FEL_REAVER_RUINS] == HORDE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_FEL_REAVER_RUINS_NEUTRAL, m_towerOwner[NODE_FEL_REAVER_RUINS] == TEAM_NONE);

    FillInitialWorldState(data, count, WORLD_STATE_EY_MAGE_TOWER_ALLIANCE, m_towerOwner[NODE_MAGE_TOWER] == ALLIANCE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_MAGE_TOWER_HORDE, m_towerOwner[NODE_MAGE_TOWER] == HORDE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_MAGE_TOWER_NEUTRAL, m_towerOwner[NODE_MAGE_TOWER] == TEAM_NONE);

    FillInitialWorldState(data, count, WORLD_STATE_EY_DRAENEI_RUINS_ALLIANCE, m_towerOwner[NODE_DRAENEI_RUINS] == ALLIANCE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_DRAENEI_RUINS_HORDE, m_towerOwner[NODE_DRAENEI_RUINS] == HORDE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_DRAENEI_RUINS_NEUTRAL, m_towerOwner[NODE_DRAENEI_RUINS] == TEAM_NONE);

    // flag states
    FillInitialWorldState(data, count, WORLD_STATE_EY_NETHERSTORM_FLAG_READY, m_flagState == EY_FLAG_STATE_ON_BASE);
    FillInitialWorldState(data, count, WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_ALLIANCE, m_flagState == EY_FLAG_STATE_ON_ALLIANCE_PLAYER ? 2 : 1);
    FillInitialWorldState(data, count, WORLD_STATE_EY_NETHERSTORM_FLAG_STATE_HORDE, m_flagState == EY_FLAG_STATE_ON_HORDE_PLAYER ? 2 : 1);

    // capture point states
    // if you leave the bg while being in capture point radius - and later join same type of bg the slider would still be displayed because the client caches it
    FillInitialWorldState(data, count, WORLD_STATE_EY_CAPTURE_POINT_SLIDER_DISPLAY, WORLD_STATE_REMOVE);
}

bool BattleGroundEY::AreAllNodesControlledByTeam(Team team)
{
    for (auto i : m_towerOwner)
        if (i != team)
            return false;

    return true;
}

bool BattleGroundEY::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1)
{
    switch (criteria_id)
    {
        case EY_ACHIEV_CRIT_DOMINATION:
        case EY_ACHIEV_CRIT_STORM_GLORY:
            return AreAllNodesControlledByTeam(source->GetTeam());
    }

    return false;
}

Team BattleGroundEY::GetPrematureWinner()
{
    int32 hordeScore = m_teamScores[TEAM_INDEX_HORDE];
    int32 allianceScore = m_teamScores[TEAM_INDEX_ALLIANCE];

    if (hordeScore > allianceScore)
        return HORDE;
    if (allianceScore > hordeScore)
        return ALLIANCE;

    // If the values are equal, fall back to number of players on each team
    return BattleGround::GetPrematureWinner();
}
