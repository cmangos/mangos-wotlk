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

#include "Common.h"
#include "Globals/SharedDefines.h"
#include "Entities/Player.h"
#include "BattleGroundMgr.h"
#include "BattleGroundAV.h"
#include "BattleGroundAB.h"
#include "BattleGroundEY.h"
#include "BattleGroundWS.h"
#include "BattleGroundNA.h"
#include "BattleGroundBE.h"
#include "BattleGroundRL.h"
#include "BattleGroundSA.h"
#include "BattleGroundDS.h"
#include "BattleGroundRV.h"
#include "BattleGroundIC.h"
#include "Maps/MapManager.h"
#include "Maps/Map.h"
#include "Globals/ObjectMgr.h"
#include "Util/ProgressBar.h"
#include "Chat/Chat.h"
#include "Arena/ArenaTeam.h"
#include "World/World.h"
#include "Server/WorldPacket.h"
#include "GameEvents/GameEventMgr.h"
#include "Tools/Formulas.h"
#include "Mails/Mail.h"
#include "Loot/LootMgr.h"

#include "Policies/Singleton.h"

INSTANTIATE_SINGLETON_1(BattleGroundMgr);

/*********************************************************/
/***            BATTLEGROUND MANAGER                   ***/
/*********************************************************/

BattleGroundMgr::BattleGroundMgr() : m_arenaTesting(false), m_testing(false)
{
    for (uint8 i = BATTLEGROUND_TYPE_NONE; i < MAX_BATTLEGROUND_TYPE_ID; ++i)
        m_battleGrounds[i].clear();
}

BattleGroundMgr::~BattleGroundMgr()
{
    DeleteAllBattleGrounds();
}

/**
  Method that deletes all battlegrounds
*/
void BattleGroundMgr::DeleteAllBattleGrounds()
{
    // will also delete template bgs:
    for (uint8 i = BATTLEGROUND_TYPE_NONE; i < MAX_BATTLEGROUND_TYPE_ID; ++i)
        m_battleGrounds[i].clear();
}

/**
  Update method

  @param    diff
*/
void BattleGroundMgr::Update(uint32 /*diff*/)
{
    m_messager.Execute(this);
}

/**
  Send battleground status update

  @param    packet
  @param    battleground
  @param    queue slot
  @param    status id
  @param    time1
  @param    time2
  @param    arena type
  @param    arena team
*/
void BattleGroundMgr::BuildBattleGroundStatusPacket(WorldPacket& data, bool bgExists, uint32 bgTypeId, uint32 bgClientInstanceId, bool isRated, uint32 mapId, uint8 queueSlot, uint8 statusId, uint32 time1, uint32 time2, ArenaType arenaType, Team arenaTeam, uint8 minLevel, uint8 maxLevel)
{
    // we can be in 2 queues in same time...

    if (statusId == 0 || !bgExists)
    {
        data.Initialize(SMSG_BATTLEFIELD_STATUS, 4 + 8);
        data << uint32(queueSlot);                         // queue id (0...1)
        data << uint64(0);
        return;
    }

    data.Initialize(SMSG_BATTLEFIELD_STATUS, (4 + 8 + 1 + 1 + 4 + 1 + 4 + 4 + 4));
    data << uint32(queueSlot);                             // queue id (0...1) - player can be in 2 queues in time
    // uint64 in client
    data << uint64(uint64(arenaType) | (uint64(0x0D) << 8) | (uint64(bgTypeId) << 16) | (uint64(0x1F90) << 48));
    data << uint8(minLevel);
    data << uint8(maxLevel);
    data << uint32(bgClientInstanceId);
    // alliance/horde for BG and skirmish/rated for Arenas
    // following displays the minimap-icon 0 = faction icon 1 = arenaicon
    data << uint8(isRated);
    data << uint32(statusId);                              // status

    switch (statusId)
    {
        case STATUS_WAIT_QUEUE:                            // status_in_queue
            data << uint32(time1);                         // average wait time, milliseconds
            data << uint32(time2);                         // time in queue, updated every minute!, milliseconds
            break;
        case STATUS_WAIT_JOIN:                              // status_invite
            data << uint32(mapId);                         // map id
            data << uint64(0);                             // 3.3.5, unknown
            data << uint32(time1);                         // time to remove from queue, milliseconds
            break;
        case STATUS_IN_PROGRESS:                            // status_in_progress
            data << uint32(mapId);                         // map id
            data << uint64(0);                             // 3.3.5, unknown
            data << uint32(time1);                         // time to bg auto leave, 0 at bg start, 120000 after bg end, milliseconds
            data << uint32(time2);                         // time from bg start, milliseconds
            data << uint8(arenaTeam == ALLIANCE ? 1 : 0);  // arenaTeam (0 for horde, 1 for alliance)
            break;
        default:
            sLog.outError("Unknown BG status!");
            break;
    }
}

/**
  Send battleground log update

  @param    packet
  @param    battleground
*/
void BattleGroundMgr::BuildPvpLogDataPacket(WorldPacket& data, BattleGround* bg)
{
    uint8 type = (bg->IsArena() ? 1 : 0);
    // last check on 3.0.3
    data.Initialize(MSG_PVP_LOG_DATA, (1 + 1 + 4 + 40 * bg->GetPlayerScoresSize()));
    data << uint8(type);                                   // type (battleground=0/arena=1)

    if (type)                                               // arena
    {
        // it seems this must be according to BG_WINNER_A/H and _NOT_ BG_TEAM_A/H
        for (int8 i = 1; i >= 0; --i)
        {
            uint32 pointsLost = bg->m_arenaTeamRatingChanges[i] < 0 ? abs(bg->m_arenaTeamRatingChanges[i]) : 0;
            uint32 pointsGained = bg->m_arenaTeamRatingChanges[i] > 0 ? bg->m_arenaTeamRatingChanges[i] : 0;

            data << uint32(pointsLost);                    // Rating Lost
            data << uint32(pointsGained);                  // Rating gained
            data << uint32(0);                             // Matchmaking Value
            DEBUG_LOG("rating change: %d", bg->m_arenaTeamRatingChanges[i]);
        }

        for (int8 i = 1; i >= 0; --i)
        {
            uint32 at_id = bg->m_arenaTeamIds[i];
            ArenaTeam* at = sObjectMgr.GetArenaTeamById(at_id);
            if (at)
                data << at->GetName();
            else
                data << (uint8)0;
        }
    }

    if (bg->GetStatus() != STATUS_WAIT_LEAVE)
        data << uint8(0);                                  // bg not ended
    else
    {
        data << uint8(1);                                  // bg ended
        data << uint8(bg->GetWinner());                    // who won
    }

    data << (int32)(bg->GetPlayerScoresSize());

    for (BattleGround::BattleGroundScoreMap::const_iterator itr = bg->GetPlayerScoresBegin(); itr != bg->GetPlayerScoresEnd(); ++itr)
    {
        const BattleGroundScore* score = itr->second;

        data << ObjectGuid(itr->first);
        data << (int32)score->killingBlows;

        if (type == 0)
        {
            data << (int32)score->honorableKills;
            data << (int32)score->deaths;
            data << (int32)(score->bonusHonor);
        }
        else
        {
            Team team = bg->GetPlayerTeam(itr->first);
            if (team != ALLIANCE && team != HORDE)
                if (Player* player = sObjectMgr.GetPlayer(itr->first))
                    team = bg->IsArena() ? Team(itr->second->Team) : player->GetTeam();

            data << uint8(team == ALLIANCE ? 1 : 0); // green or yellow
        }
        data << (int32)score->damageDone;            // damage done
        data << (int32)score->healingDone;           // healing done

        switch (bg->GetTypeId())                            // battleground specific things
        {
            case BATTLEGROUND_AV:
                data << (uint32)0x00000005;                // count of next fields
                data << (uint32)((BattleGroundAVScore*)score)->graveyardsAssaulted;  // GraveyardsAssaulted
                data << (uint32)((BattleGroundAVScore*)score)->graveyardsDefended;   // GraveyardsDefended
                data << (uint32)((BattleGroundAVScore*)score)->towersAssaulted;      // TowersAssaulted
                data << (uint32)((BattleGroundAVScore*)score)->towersDefended;       // TowersDefended
                data << (uint32)((BattleGroundAVScore*)score)->secondaryObjectives;  // SecondaryObjectives - free some of the Lieutnants
                break;
            case BATTLEGROUND_WS:
                data << (uint32)0x00000002;                // count of next fields
                data << (uint32)((BattleGroundWGScore*)score)->flagCaptures;         // flag captures
                data << (uint32)((BattleGroundWGScore*)score)->flagReturns;          // flag returns
                break;
            case BATTLEGROUND_AB:
                data << (uint32)0x00000002;                // count of next fields
                data << (uint32)((BattleGroundABScore*)score)->basesAssaulted;       // bases asssulted
                data << (uint32)((BattleGroundABScore*)score)->basesDefended;        // bases defended
                break;
            case BATTLEGROUND_EY:
                data << (uint32)0x00000001;                // count of next fields
                data << (uint32)((BattleGroundEYScore*)score)->flagCaptures;         // flag captures
                break;
            case BATTLEGROUND_SA:                          // wotlk
                data << (uint32)0x00000002;                // count of next fields
                data << (uint32)((BattleGroundSAScore*)score)->demolishersDestroyed; // demolishers destroyed
                data << (uint32)((BattleGroundSAScore*)score)->gatesDestroyed;       // gates destroyed
                break;
            case BATTLEGROUND_IC:                           // wotlk
                data << uint32(0x00000002);                 // count of next fields
                data << uint32(((BattleGroundICScore*)score)->basesAssaulted);       // bases asssulted
                data << uint32(((BattleGroundICScore*)score)->basesDefended);        // bases defended
                break;
            case BATTLEGROUND_NA:
            case BATTLEGROUND_BE:
            case BATTLEGROUND_AA:
            case BATTLEGROUND_RL:
            case BATTLEGROUND_DS:                           // wotlk
            case BATTLEGROUND_RV:                           // wotlk
            case BATTLEGROUND_RB:                           // wotlk
                data << (int32)0;                          // 0
                break;
            default:
                DEBUG_LOG("Unhandled MSG_PVP_LOG_DATA for BG id %u", bg->GetTypeId());
                data << (int32)0;
                break;
        }
    }
}

/**
  Send battleground joined packet

  @param    packet
  @param    result
*/
void BattleGroundMgr::BuildGroupJoinedBattlegroundPacket(WorldPacket& data, BattleGroundGroupJoinStatus result)
{
    data.Initialize(SMSG_GROUP_JOINED_BATTLEGROUND, 4);
    data << int32(result);
    switch (result)
    {
        case BG_GROUP_JOIN_STATUS_JOIN_TIMED_OUT:
        case BG_GROUP_JOIN_STATUS_JOIN_FAILED:
            data << uint64(0); // player guid
            break;
    }
}

/**
  Send battleground world state packet

  @param    packet
  @param    field
  @param    value
*/
void BattleGroundMgr::BuildUpdateWorldStatePacket(WorldPacket& data, uint32 field, uint32 value)
{
    data.Initialize(SMSG_UPDATE_WORLD_STATE, 4 + 4);
    data << uint32(field);
    data << uint32(value);
}

/**
  Send battleground sound packet

  @param    packet
  @param    sound id
*/
void BattleGroundMgr::BuildPlaySoundPacket(WorldPacket& data, uint32 soundId)
{
    data.Initialize(SMSG_PLAY_SOUND, 4);
    data << uint32(soundId);
}

/**
  Send player leave from battleground packet

  @param    packet
  @param    object guid
*/
void BattleGroundMgr::BuildPlayerLeftBattleGroundPacket(WorldPacket& data, ObjectGuid guid)
{
    data.Initialize(SMSG_BATTLEGROUND_PLAYER_LEFT, 8);
    data << ObjectGuid(guid);
}

/**
  Send player join from battleground packet

  @param    packet
  @param    player
*/
void BattleGroundMgr::BuildPlayerJoinedBattleGroundPacket(WorldPacket& data, Player* player)
{
    data.Initialize(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
    data << player->GetObjectGuid();
}

/**
  Function that returns battleground from client instance id and battleground type

  @param    instance id
  @param    battleground type id
*/
BattleGround* BattleGroundMgr::GetBattleGroundThroughClientInstance(uint32 instanceId, BattleGroundTypeId bgTypeId)
{
    // cause at HandleBattleGroundJoinOpcode the clients sends the instanceid he gets from
    // SMSG_BATTLEFIELD_LIST we need to find the battleground with this clientinstance-id
    BattleGround* bg = GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return nullptr;

    if (bg->IsArena())
        return GetBattleGround(instanceId, bgTypeId);

    for (auto& itr : m_battleGrounds[bgTypeId])
    {
        if (itr.second->GetClientInstanceId() == instanceId)
            return itr.second.get();
    }

    return nullptr;
}

/**
  Function that returns battleground from instance id and battleground type

  @param    instance id
  @param    battleground type id
*/
BattleGround* BattleGroundMgr::GetBattleGround(uint32 instanceId, BattleGroundTypeId bgTypeId)
{
    // search if needed
    BattleGroundSet::iterator itr;
    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
    {
        for (uint8 i = BATTLEGROUND_AV; i < MAX_BATTLEGROUND_TYPE_ID; ++i)
        {
            itr = m_battleGrounds[i].find(instanceId);
            if (itr != m_battleGrounds[i].end())
                return itr->second.get();
        }
        return nullptr;
    }

    itr = m_battleGrounds[bgTypeId].find(instanceId);
    return ((itr != m_battleGrounds[bgTypeId].end()) ? itr->second.get() : nullptr);
}

/**
  Function that returns battleground from template id

  @param    battleground type id
*/
BattleGround* BattleGroundMgr::GetBattleGroundTemplate(BattleGroundTypeId bgTypeId) const
{
    // map is sorted and we can be sure that lowest instance id has only BG template
    return m_battleGrounds[bgTypeId].empty() ? nullptr : m_battleGrounds[bgTypeId].begin()->second.get();
}



/**
  Function that creates a new battleground that is actually used

  @param    battleground type id
  @param    bracket entry
  @param    arena type
  @param    isRated
*/
BattleGround* BattleGroundMgr::CreateNewBattleGround(BattleGroundTypeId bgTypeId, PvPDifficultyEntry const* bracketEntry, ArenaType arenaType, bool isRated, uint32 instanceId, uint32 clientInstanceId)
{
    // get the template BG
    BattleGround* bgTemplate = GetBattleGroundTemplate(bgTypeId);
    if (!bgTemplate)
    {
        sLog.outError("BattleGround: CreateNewBattleGround - bg template not found for %u", bgTypeId);
        return nullptr;
    }

    bool isRandom = false;
    BattleGroundTypeId bgRandomTypeId = BattleGroundTypeId(0);

    if (bgTypeId == BATTLEGROUND_RB)
    {
        BattleGroundTypeId battlegrounds[] = { BATTLEGROUND_AV, BATTLEGROUND_WS, BATTLEGROUND_AB, BATTLEGROUND_EY, BATTLEGROUND_SA, BATTLEGROUND_IC};
        bgTypeId = battlegrounds[urand(0, countof(battlegrounds) - 1)];

        bgRandomTypeId = bgTypeId;
        bgTemplate = GetBattleGroundTemplate(bgTypeId);

        if (!bgTemplate)
        {
            sLog.outError("BattleGround: CreateNewBattleGround - bg template not found for %u", bgTypeId);
            return nullptr;
        }
        isRandom = true;
    }

    BattleGround* bg;
    // create a copy of the BG template
    switch (bgTypeId)
    {
        case BATTLEGROUND_AV:
            bg = new BattleGroundAV(*(BattleGroundAV*)bgTemplate);
            break;
        case BATTLEGROUND_WS:
            bg = new BattleGroundWS(*(BattleGroundWS*)bgTemplate);
            break;
        case BATTLEGROUND_AB:
            bg = new BattleGroundAB(*(BattleGroundAB*)bgTemplate);
            break;
        case BATTLEGROUND_NA:
            bg = new BattleGroundNA(*(BattleGroundNA*)bgTemplate);
            break;
        case BATTLEGROUND_BE:
            bg = new BattleGroundBE(*(BattleGroundBE*)bgTemplate);
            break;
        case BATTLEGROUND_EY:
            bg = new BattleGroundEY(*(BattleGroundEY*)bgTemplate);
            break;
        case BATTLEGROUND_RL:
            bg = new BattleGroundRL(*(BattleGroundRL*)bgTemplate);
            break;
        case BATTLEGROUND_SA:
            bg = new BattleGroundSA(*(BattleGroundSA*)bgTemplate);
            break;
        case BATTLEGROUND_DS:
            bg = new BattleGroundDS(*(BattleGroundDS*)bgTemplate);
            break;
        case BATTLEGROUND_RV:
            bg = new BattleGroundRV(*(BattleGroundRV*)bgTemplate);
            break;
        case BATTLEGROUND_IC:
            bg = new BattleGroundIC(*(BattleGroundIC*)bgTemplate);
            break;
        case BATTLEGROUND_AA:
        case BATTLEGROUND_RB:
            break;
        default:
            // error, but it is handled few lines above
            return nullptr;
    }

    // must occur before CreateBgMap - used to detect difficulty of BG
    bg->SetBracket(bracketEntry);

    bgTypeId = isRandom ? BATTLEGROUND_RB : bgTypeId;

    // will also set m_bgMap, instanceid
    sMapMgr.CreateBgMap(bg->GetMapId(), instanceId, bg);

    bg->SetClientInstanceId(clientInstanceId);

    // reset the new bg (set status to status_wait_queue from status_none)
    bg->Reset();

    // start the joining of the bg
    bg->SetStatus(STATUS_WAIT_JOIN);
    bg->SetArenaType(arenaType);
    bg->SetRated(isRated);
    bg->SetRandom(isRandom);
    bg->SetTypeId(bgTypeId);
    bg->SetRandomTypeId(bgRandomTypeId);

    return bg;
}

/**
  Function that creates battleground templates

  @param    battleground type id
  @param    isArena
  @param    min players per team
  @param    max players per team
  @param    level min
  @param    level max
  @param    battleground name
  @param    map id
  @param    team 1 start location X
  @param    team 1 start location Y
  @param    team 1 start location Z
  @param    team 1 start location O
  @param    team 2 start location X
  @param    team 2 start location Y
  @param    team 2 start location Z
  @param    team 2 start location O
  @param    start max distance
*/
uint32 BattleGroundMgr::CreateBattleGround(BattleGroundTypeId bgTypeId, bool IsArena, uint32 minPlayersPerTeam, uint32 maxPlayersPerTeam, uint32 levelMin, uint32 levelMax, char const* battleGroundName, uint32 mapId, float team1StartLocX, float team1StartLocY, float team1StartLocZ, float team1StartLocO, float team2StartLocX, float team2StartLocY, float team2StartLocZ, float team2StartLocO, float startMaxDist, uint32 playerSkinReflootId)
{
    // Create the BG
    BattleGround* bg;
    switch (bgTypeId)
    {
        case BATTLEGROUND_AV: bg = new BattleGroundAV; break;
        case BATTLEGROUND_WS: bg = new BattleGroundWS; break;
        case BATTLEGROUND_AB: bg = new BattleGroundAB; break;
        case BATTLEGROUND_NA: bg = new BattleGroundNA; break;
        case BATTLEGROUND_BE: bg = new BattleGroundBE; break;
        case BATTLEGROUND_EY: bg = new BattleGroundEY; break;
        case BATTLEGROUND_RL: bg = new BattleGroundRL; break;
        case BATTLEGROUND_SA: bg = new BattleGroundSA; break;
        case BATTLEGROUND_DS: bg = new BattleGroundDS; break;
        case BATTLEGROUND_RV: bg = new BattleGroundRV; break;
        case BATTLEGROUND_IC: bg = new BattleGroundIC; break;
        case BATTLEGROUND_AA: bg = new BattleGround;   break;
        case BATTLEGROUND_RB: bg = new BattleGround;   break;
        default:              bg = new BattleGround;   break;                           // placeholder for non implemented BG
    }

    bg->SetMapId(mapId);
    bg->SetTypeId(bgTypeId);
    bg->SetArenaorBGType(IsArena);
    bg->SetMinPlayersPerTeam(minPlayersPerTeam);
    bg->SetMaxPlayersPerTeam(maxPlayersPerTeam);
    bg->SetMinPlayers(minPlayersPerTeam * 2);
    bg->SetMaxPlayers(maxPlayersPerTeam * 2);
    bg->SetName(battleGroundName);
    bg->SetTeamStartLoc(ALLIANCE, team1StartLocX, team1StartLocY, team1StartLocZ, team1StartLocO);
    bg->SetTeamStartLoc(HORDE,    team2StartLocX, team2StartLocY, team2StartLocZ, team2StartLocO);
    bg->SetStartMaxDist(startMaxDist);
    bg->SetLevelRange(levelMin, levelMax);
    bg->SetPlayerSkinRefLootId(playerSkinReflootId);

    // add bg to update list
    AddBattleGround(bg->GetInstanceId(), bg->GetTypeId(), bg);

    // return some not-null value, bgTypeId is good enough for me
    return bgTypeId;
}

void BattleGroundMgr::AddBattleGround(uint32 instanceId, BattleGroundTypeId bgTypeId, BattleGround* bg)
{
    MaNGOS::unique_trackable_ptr<BattleGround>& ptr = m_battleGrounds[bgTypeId][instanceId];
    ptr.reset(bg);
    bg->SetWeakPtr(ptr);
}

/**
  Method that loads battleground data from DB
*/
void BattleGroundMgr::CreateInitialBattleGrounds()
{
    uint32 count = 0;

    //                                             0   1                 2                 3                4             5             6
    auto queryResult = WorldDatabase.Query("SELECT id, MinPlayersPerTeam,MaxPlayersPerTeam,AllianceStartLoc,HordeStartLoc,StartMaxDist, PlayerSkinReflootId FROM battleground_template");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 battlegrounds. DB table `battleground_template` is empty.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();
        bar.step();

        uint32 resultedBgTypeId = fields[0].GetUInt32();

        // can be overwrite by values from DB
        BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(resultedBgTypeId);
        if (!bl)
        {
            sLog.outError("Battleground ID %u not found in BattlemasterList.dbc. Battleground not created.", resultedBgTypeId);
            continue;
        }

        BattleGroundTypeId bgTypeId = BattleGroundTypeId(resultedBgTypeId);

        bool isArena = (bl->type == TYPE_ARENA);
        uint32 minPlayersPerTeam = fields[1].GetUInt32();
        uint32 maxPlayersPerTeam = fields[2].GetUInt32();

        // check values from DB
        if (maxPlayersPerTeam == 0)
        {
            sLog.outErrorDb("Table `battleground_template` for id %u doesn't allow any player per team settings. BG not created.", bgTypeId);
            continue;
        }

        if (minPlayersPerTeam > maxPlayersPerTeam)
        {
            minPlayersPerTeam = maxPlayersPerTeam;
            sLog.outErrorDb("Table `battleground_template` for id %u has min players > max players per team settings. Min players will use same value as max players.", bgTypeId);
        }

        float allianceStartLoc[4];
        float hordeStartLoc[4];

        uint32 start1 = fields[3].GetUInt32();

        WorldSafeLocsEntry const* start = sWorldSafeLocsStore.LookupEntry<WorldSafeLocsEntry>(start1);
        if (start)
        {
            allianceStartLoc[0] = start->x;
            allianceStartLoc[1] = start->y;
            allianceStartLoc[2] = start->z;
            allianceStartLoc[3] = start->o;
        }
        else if (bgTypeId == BATTLEGROUND_AA || bgTypeId == BATTLEGROUND_RB)
        {
            allianceStartLoc[0] = 0;
            allianceStartLoc[1] = 0;
            allianceStartLoc[2] = 0;
            allianceStartLoc[3] = 0;
        }
        else
        {
            sLog.outErrorDb("Table `battleground_template` for id %u have nonexistent WorldSafeLocs.dbc id %u in field `AllianceStartLoc`. BG not created.", bgTypeId, start1);
            continue;
        }

        uint32 start2 = fields[4].GetUInt32();

        start = sWorldSafeLocsStore.LookupEntry<WorldSafeLocsEntry>(start2);
        if (start)
        {
            hordeStartLoc[0] = start->x;
            hordeStartLoc[1] = start->y;
            hordeStartLoc[2] = start->z;
            hordeStartLoc[3] = start->o;
        }
        else if (bgTypeId == BATTLEGROUND_AA || bgTypeId == BATTLEGROUND_RB)
        {
            hordeStartLoc[0] = 0;
            hordeStartLoc[1] = 0;
            hordeStartLoc[2] = 0;
            hordeStartLoc[3] = 0;
        }
        else
        {
            sLog.outErrorDb("Table `battleground_template` for id %u have nonexistent WorldSafeLocs.dbc id %u in field `HordeStartLoc`. BG not created.", bgTypeId, start2);
            continue;
        }

        float startMaxDist = fields[5].GetFloat();

        uint32 playerSkinReflootId = fields[6].GetUInt32();
        if (playerSkinReflootId && !sLootMgr.ExistsRefLootTemplate(playerSkinReflootId))
        {
            playerSkinReflootId = 0;
            sLog.outErrorDb("Table `battleground_template` for id %u associated with nonexistent refloot id %u. Setting to 0.", bgTypeId, playerSkinReflootId);
        }

        if (playerSkinReflootId)
            m_usedRefloot.insert(playerSkinReflootId);

        // sLog.outDetail("Creating battleground %s, %u-%u", bl->name[sWorld.GetDBClang()], MinLvl, MaxLvl);
        if (!CreateBattleGround(bgTypeId, isArena, minPlayersPerTeam, maxPlayersPerTeam, bl->minLevel, bl->maxLevel, bl->name[sWorld.GetDefaultDbcLocale()], bl->mapid[0], allianceStartLoc[0], allianceStartLoc[1], allianceStartLoc[2], allianceStartLoc[3], hordeStartLoc[0], hordeStartLoc[1], hordeStartLoc[2], hordeStartLoc[3], startMaxDist, playerSkinReflootId))
            continue;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u battlegrounds", count);
    sLog.outString();
}

/**
  Method that distributes the arena points distribution
*/
void BattleGroundMgr::DistributeArenaPoints() const
{
    // used to distribute arena points based on last week's stats
    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_START);

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_ONLINE_START);

    // temporary structure for storing maximum points to add values for all players
    std::map<uint32, uint32> PlayerPoints;

    // at first update all points for all team members
    for (ObjectMgr::ArenaTeamMap::iterator team_itr = sObjectMgr.GetArenaTeamMapBegin(); team_itr != sObjectMgr.GetArenaTeamMapEnd(); ++team_itr)
    {
        if (ArenaTeam* at = team_itr->second)
        {
            at->UpdateArenaPointsHelper(PlayerPoints);
        }
    }

    // cycle that gives points to all players
    for (auto& PlayerPoint : PlayerPoints)
    {
        // update to database
        CharacterDatabase.PExecute("UPDATE characters SET arenaPoints = arenaPoints + '%u' WHERE guid = '%u'", PlayerPoint.second, PlayerPoint.first);
        // add points if player is online
        if (Player* pl = sObjectMgr.GetPlayer(ObjectGuid(HIGHGUID_PLAYER, PlayerPoint.first)))
            pl->ModifyArenaPoints(PlayerPoint.second);
    }

    PlayerPoints.clear();

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_ONLINE_END);

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_TEAM_START);
    for (ObjectMgr::ArenaTeamMap::iterator titr = sObjectMgr.GetArenaTeamMapBegin(); titr != sObjectMgr.GetArenaTeamMapEnd(); ++titr)
    {
        if (ArenaTeam* at = titr->second)
        {
            at->FinishWeek();                              // set played this week etc values to 0 in memory, too
            at->SaveToDB();                                // save changes
            at->NotifyStatsChanged();                      // notify the players of the changes
        }
    }

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_TEAM_END);

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_POINTS_END);
}

/**
  Method checks players and arena winners

  @param    arena team
  @param    player ranks map
  @param    rank
*/
void CheckPlayersAndAddToWinners(ArenaTeam* team, std::map<ObjectGuid, uint32>& playerRanks, uint32 rank)
{
    for (ArenaTeamMember& member : team->GetMembers())
    {
        // Has personal rating within 100 of team rating and played at least 20% of games of given team
        if (std::abs(int32(member.personal_rating) - int32(team->GetStats().rating)) <= 100 && member.games_season > (team->GetStats().games_season * 20 / 100))
        {
            auto itr = playerRanks.find(member.guid);
            if (itr != playerRanks.end())
            {
                if (itr->second > rank)
                    itr->second = rank;
            }
            else
                playerRanks.emplace(member.guid, rank);
        }
    }
}

/**
  Method that sets that title values

  @param    first title
  @param    second title
  @param    title id
*/
void SetTitleValues(uint32& first, uint32& second, uint32 titleId)
{
    switch (titleId)
    {
        case 42: first |= 0x20000000; break;
        case 43: first |= 0x40000000; break;
        case 44: first |= 0x80000000; break;
        case 45: second |= 0x00000001; break;
        case 62: second |= 0x00000020; break;
        case 71: second |= 0x00000100; break;
        default: break; // cant award gladiator
    }
}

/**
  Method that computes arena season rewards

  @param    season id
*/
void BattleGroundMgr::RewardArenaSeason(uint32 seasonId)
{
    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_REWARDS_START, seasonId);

    std::vector<ArenaTeam*> sortedTeams[3];
    for (ObjectMgr::ArenaTeamMap::iterator titr = sObjectMgr.GetArenaTeamMapBegin(); titr != sObjectMgr.GetArenaTeamMapEnd(); ++titr)
        if (ArenaTeam* at = titr->second)
        {
            // team needs minimum of 10 games played during season to be part of ladders for rewards
            // this number is unproven - in later expansions it may be 50 games min
            if (at->GetStats().games_season >= 10)
            {
                switch (at->GetType())
                {
                    case ARENA_TYPE_2v2:
                        sortedTeams[0].push_back(at);
                        break;
                    case ARENA_TYPE_3v3:
                        sortedTeams[1].push_back(at);
                        break;
                    case ARENA_TYPE_5v5:
                        sortedTeams[2].push_back(at);
                        break;
                    default: break;
                }
            }
        }

    // first sort teams based on rating
    for (uint32 i = 0; i < 3; ++i)
        std::sort(sortedTeams[i].begin(), sortedTeams[i].end(), [](ArenaTeam* first, ArenaTeam* second)
    {
        return first->GetStats().rating > second->GetStats().rating;
    });

    uint32 gladiatorCount[3], duelistCount[3], rivalCount[3], challengerCount[3];
    for (uint32 i = 0; i < 3; ++i)
    {
        gladiatorCount[i]       = std::max(uint32(sortedTeams[i].size()) * 5 / 1000, uint32(1));
        duelistCount[i]         = std::max(uint32(sortedTeams[i].size()) * 3 / 100, uint32(1));
        rivalCount[i]           = std::max(uint32(sortedTeams[i].size()) * 10 / 100, uint32(1));
        challengerCount[i]      = std::max(uint32(sortedTeams[i].size()) * 35 / 100, uint32(1));
    }

    std::map<ObjectGuid, uint32> playerRanks;
    for (uint32 i = 0; i < 3; ++i)
    {
        for (uint32 k = 0; k < sortedTeams[i].size(); ++k)
        {
            ArenaTeam* team = sortedTeams[i][k];
            if (k < 1)
                CheckPlayersAndAddToWinners(team, playerRanks, 0);
            else if (k < gladiatorCount[i])
                CheckPlayersAndAddToWinners(team, playerRanks, 1);
            else if (k < duelistCount[i])
                CheckPlayersAndAddToWinners(team, playerRanks, 2);
            else if (k < rivalCount[i])
                CheckPlayersAndAddToWinners(team, playerRanks, 3);
            else if (k < challengerCount[i])
                CheckPlayersAndAddToWinners(team, playerRanks, 4);
        }
    }

    uint32 mountId;
    uint32 titles[5];
    titles[1] = 42; // Gladiator
    titles[2] = 43; // Duelist
    titles[3] = 44; // Rival
    titles[4] = 45; // Challenger
    switch (seasonId)
    {
        case 1:
        default: mountId = 30609; titles[0] = 42; break;
        case 2: mountId = 34092; titles[0] = 62; break;
        case 3: mountId = 37676; titles[0] = 71; break;
        case 4: mountId = 43516; titles[0] = 80; break;
        case 5: mountId = 46708; titles[0] = 157; break;
        case 6: mountId = 46171; titles[0] = 167; break;
        case 7: mountId = 47840; titles[0] = 169; break;
        case 8: mountId = 50435; titles[0] = 177; break;
    }
    CharTitlesEntry const* titleEntries[5]; // optimization
    for (uint32 i = 0; i < 5; ++i)
        titleEntries[i] = sCharTitlesStore.LookupEntry(titles[i]);

    // Remove titles from online players
    // Only Rank 1 titles are permanent
    sObjectAccessor.ExecuteOnAllPlayers([&](Player* player)
    {
        player->SetTitle(titleEntries[1], true);
        player->SetTitle(titleEntries[2], true);
        player->SetTitle(titleEntries[3], true);
        player->SetTitle(titleEntries[4], true);
        player->SaveTitles();
        player->ModifyHonorPoints(player->GetArenaPoints() * 4);
        player->SetArenaPoints(0);
    });

    /* TODO: Fix removal of old titles in wotlk due to 6x uint32 values
    // Remove Gladiator, Duelist and Rival from every offline player
    CharacterDatabase.PExecute("UPDATE characters a SET knownTitles="
        "CONCAT(CAST(TRIM(SUBSTRING_INDEX(knownTitles, ' ', 1))  AS UNSIGNED) &~0xE0000000, ' ', SUBSTR(knownTitles, LOCATE(' ', knownTitles)))"
        "WHERE(CAST(TRIM(SUBSTRING_INDEX(knownTitles, ' ', 1))  AS UNSIGNED) & 0xE0000000) != 0");

    // Remove Challenger from every offline player
    CharacterDatabase.PExecute("UPDATE characters a SET knownTitles ="
        "CONCAT(SUBSTRING_INDEX(knownTitles, ' ', 1), ' ', CAST(TRIM(SUBSTR(knownTitles, LOCATE(' ', knownTitles)))  AS UNSIGNED) &~0x00000001)"
        "WHERE(CAST(TRIM(SUBSTR(knownTitles, LOCATE(' ', knownTitles)))  AS UNSIGNED) & 0x00000001) != 0");
    */

    CharacterDatabase.PExecute("UPDATE characters SET totalHonorPoints=4*arenaPoints,arenaPoints=0");

    for (auto& data : playerRanks)
    {
        Player* player = sObjectMgr.GetPlayer(data.first);
        if (data.second <= 1) // gladiator reward
        {
            MailDraft draft;
            draft.SetSubjectAndBody("Season Rewards", "On behalf of the Steamwheedle Fighting Circuit, we congratulate you for your successes in this arena season. In recognition of your skill and savagery, we hereby bestow upon you this Nether Drake. May it serve you well.");
            Item* item = Item::CreateItem(mountId, 1, nullptr);
            item->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted
            draft.AddItem(item);
            MailSender sender;
            draft.SendMailTo(MailReceiver(player, data.first), sender);
        }
        if (player)
        {
            player->SetTitle(titleEntries[data.second]);
            player->SaveTitles();
        }
        else
        {
            auto queryResult = CharacterDatabase.PQuery("SELECT knownTitles FROM characters WHERE guid = '%u'", data.first.GetCounter());
            if (queryResult)
            {
                uint32 titleValueCount = 2;
                uint32 titleValues[2];
                std::string titlesData = queryResult->Fetch()[0].GetCppString();
                Tokens tokens = StrSplit(titlesData, " ");
                if (tokens.size() != titleValueCount)
                    return;

                Tokens::iterator iter;
                uint32 index;
                for (iter = tokens.begin(), index = 0; index < titleValueCount; ++iter, ++index)
                    titleValues[index] = std::stoul((*iter).c_str());

                SetTitleValues(titleValues[0], titleValues[1], titles[data.second]);

                std::string newTitleData = std::to_string(titleValues[0]) + " " + std::to_string(titleValues[1]) + " ";
                CharacterDatabase.PExecute("UPDATE characters SET knownTitles='%s' WHERE guid = '%u'", newTitleData.data(), data.first.GetCounter());
            }
        }
    }

    ResetAllArenaData();

    sWorld.SendWorldTextToAboveSecurity(SEC_GAMEMASTER, LANG_DIST_ARENA_REWARDS_END);
}

/**
  Method that resets all arena data
*/
void BattleGroundMgr::ResetAllArenaData()
{
    for (ObjectMgr::ArenaTeamMap::iterator titr = sObjectMgr.GetArenaTeamMapBegin(); titr != sObjectMgr.GetArenaTeamMapEnd(); ++titr)
    {
        if (ArenaTeam* at = titr->second)
        {
            at->FinishSeason();                            // set all values back to default
            at->SaveToDB();                                // save changes
            at->NotifyStatsChanged();                      // notify the players of the changes
        }
    }
}

/**
  Method that sends player to battleground

  @param    player
  @param    instance id
  @param    battleground type id
*/
void BattleGroundMgr::SendToBattleGround(Player* player, uint32 instanceId, BattleGroundTypeId bgTypeId)
{
    BattleGround* bg = GetBattleGround(instanceId, bgTypeId);
    if (bg)
    {
        uint32 mapid = bg->GetMapId();
        float x, y, z, O;
        Team team = player->GetBGTeam();
        if (team == 0)
            team = player->GetTeam();
        bg->GetTeamStartLoc(team, x, y, z, O);

        DETAIL_LOG("BATTLEGROUND: Sending %s to map %u, X %f, Y %f, Z %f, O %f", player->GetName(), mapid, x, y, z, O);
        player->TeleportTo(mapid, x, y, z, O);
    }
    else
    {
        sLog.outError("player %u trying to port to nonexistent bg instance %u", player->GetGUIDLow(), instanceId);
    }
}

/**
  Function that returns the arena type

  @param    battleground type id
*/
bool BattleGroundMgr::IsArenaType(BattleGroundTypeId bgTypeId)
{
    switch (bgTypeId)
    {
        case BATTLEGROUND_NA:
        case BATTLEGROUND_BE:
        case BATTLEGROUND_RL:
        case BATTLEGROUND_DS:
        case BATTLEGROUND_RV:
        case BATTLEGROUND_AA:
            return true;
        default:
            return false;
    }
}

/**
  Function that returns the queue type for battleground type id

  @param    battleground type id
  @param    arena type
*/
BattleGroundQueueTypeId BattleGroundMgr::BgQueueTypeId(BattleGroundTypeId bgTypeId, ArenaType arenaType)
{
    switch (bgTypeId)
    {
        case BATTLEGROUND_WS:
            return BATTLEGROUND_QUEUE_WS;
        case BATTLEGROUND_AB:
            return BATTLEGROUND_QUEUE_AB;
        case BATTLEGROUND_AV:
            return BATTLEGROUND_QUEUE_AV;
        case BATTLEGROUND_EY:
            return BATTLEGROUND_QUEUE_EY;
        case BATTLEGROUND_SA:
            return BATTLEGROUND_QUEUE_SA;
        case BATTLEGROUND_IC:
            return BATTLEGROUND_QUEUE_IC;
        case BATTLEGROUND_RB:
            return BATTLEGROUND_QUEUE_RB;
        case BATTLEGROUND_AA:
        case BATTLEGROUND_NA:
        case BATTLEGROUND_RL:
        case BATTLEGROUND_BE:
        case BATTLEGROUND_DS:
        case BATTLEGROUND_RV:
            switch (arenaType)
            {
                case ARENA_TYPE_2v2:
                    return BATTLEGROUND_QUEUE_2v2;
                case ARENA_TYPE_3v3:
                    return BATTLEGROUND_QUEUE_3v3;
                case ARENA_TYPE_5v5:
                    return BATTLEGROUND_QUEUE_5v5;
                default:
                    return BATTLEGROUND_QUEUE_NONE;
            }
        default:
            return BATTLEGROUND_QUEUE_NONE;
    }
}

/**
  Function that returns the battleground type id from battleground queue id

  @param    battleground queue id
*/
BattleGroundTypeId BattleGroundMgr::BgTemplateId(BattleGroundQueueTypeId bgQueueTypeId)
{
    switch (bgQueueTypeId)
    {
        case BATTLEGROUND_QUEUE_WS:
            return BATTLEGROUND_WS;
        case BATTLEGROUND_QUEUE_AB:
            return BATTLEGROUND_AB;
        case BATTLEGROUND_QUEUE_AV:
            return BATTLEGROUND_AV;
        case BATTLEGROUND_QUEUE_EY:
            return BATTLEGROUND_EY;
        case BATTLEGROUND_QUEUE_SA:
            return BATTLEGROUND_SA;
        case BATTLEGROUND_QUEUE_IC:
            return BATTLEGROUND_IC;
        case BATTLEGROUND_QUEUE_RB:
            return BATTLEGROUND_RB;
        case BATTLEGROUND_QUEUE_2v2:
        case BATTLEGROUND_QUEUE_3v3:
        case BATTLEGROUND_QUEUE_5v5:
            return BATTLEGROUND_AA;
        default:
            return BattleGroundTypeId(0);                   // used for unknown template (it exist and do nothing)
    }
}

/**
  Function that returns the arena type from battleground queue id

  @param    battleground queue id
*/
ArenaType BattleGroundMgr::BgArenaType(BattleGroundQueueTypeId bgQueueTypeId)
{
    switch (bgQueueTypeId)
    {
        case BATTLEGROUND_QUEUE_2v2:
            return ARENA_TYPE_2v2;
        case BATTLEGROUND_QUEUE_3v3:
            return ARENA_TYPE_3v3;
        case BATTLEGROUND_QUEUE_5v5:
            return ARENA_TYPE_5v5;
        default:
            return ARENA_TYPE_NONE;
    }
}

/**
  Method that toggles battleground test mode on / off
*/
void BattleGroundMgr::ToggleTesting()
{
    m_testing = !m_testing;
    if (m_testing)
        sWorld.SendWorldText(LANG_DEBUG_BG_ON);
    else
        sWorld.SendWorldText(LANG_DEBUG_BG_OFF);
    sWorld.GetBGQueue().GetMessager().AddMessage([testing = m_testing](BattleGroundQueue* queue)
    {
        queue->SetTesting(testing);
    });
}

/**
  Method that toggles arena test mode on / off
*/
void BattleGroundMgr::ToggleArenaTesting()
{
    m_arenaTesting = !m_arenaTesting;
    if (m_arenaTesting)
        sWorld.SendWorldText(LANG_DEBUG_ARENA_ON);
    else
        sWorld.SendWorldText(LANG_DEBUG_ARENA_OFF);
    sWorld.GetBGQueue().GetMessager().AddMessage([arenaTesting = m_arenaTesting](BattleGroundQueue* queue)
    {
        queue->SetArenaTesting(arenaTesting);
    });
}

/**
  Function that returns max arena rating difference
*/
uint32 BattleGroundMgr::GetMaxRatingDifference() const
{
    // this is for stupid people who can't use brain and set max rating difference to 0
    uint32 diff = sWorld.getConfig(CONFIG_UINT32_ARENA_MAX_RATING_DIFFERENCE);
    if (diff == 0)
        diff = 5000;
    return diff;
}

/**
  Function that returns the arena rating discard timer
*/
uint32 BattleGroundMgr::GetRatingDiscardTimer() const
{
    return sWorld.getConfig(CONFIG_UINT32_ARENA_RATING_DISCARD_TIMER);
}

/**
  Function that returns the premature finish time
*/
uint32 BattleGroundMgr::GetPrematureFinishTime() const
{
    return sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMATURE_FINISH_TIMER);
}

/**
  Method that loads battlemaster entries from DB
*/
void BattleGroundMgr::LoadBattleMastersEntry(bool reload)
{
    std::shared_ptr<BattleMastersMap> newBattleMastersMap = std::make_shared<BattleMastersMap>();

    auto queryResult = WorldDatabase.Query("SELECT entry,bg_template FROM battlemaster_entry");

    uint32 count = 0;

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded 0 battlemaster entries - table is empty!");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        ++count;
        bar.step();

        Field* fields = queryResult->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint32 bgTypeId  = fields[1].GetUInt32();
        if (!sBattlemasterListStore.LookupEntry(bgTypeId))
        {
            sLog.outErrorDb("Table `battlemaster_entry` contain entry %u for nonexistent battleground type %u, ignored.", entry, bgTypeId);
            continue;
        }

        (*newBattleMastersMap)[entry] = BattleGroundTypeId(bgTypeId);
    }
    while (queryResult->NextRow());

    m_battleMastersMap = newBattleMastersMap;

    if (reload)
    {
        sMapMgr.DoForAllMaps([battleMasters = newBattleMastersMap](Map* map)
        {
            map->GetMessager().AddMessage([battleMasters](Map* map)
            {
                map->GetMapDataContainer().SetBattleMastersMap(battleMasters);
            });
        });
    }

    sLog.outString(">> Loaded %u battlemaster entries", count);
    sLog.outString();
}

/**
  Function that returns the holiday id from battleground type id

  @param    battleground type id
*/
HolidayIds BattleGroundMgr::BgTypeToWeekendHolidayId(BattleGroundTypeId bgTypeId)
{
    switch (bgTypeId)
    {
        case BATTLEGROUND_AV: return HOLIDAY_CALL_TO_ARMS_AV;
        case BATTLEGROUND_EY: return HOLIDAY_CALL_TO_ARMS_EY;
        case BATTLEGROUND_WS: return HOLIDAY_CALL_TO_ARMS_WS;
        case BATTLEGROUND_SA: return HOLIDAY_CALL_TO_ARMS_SA;
        case BATTLEGROUND_AB: return HOLIDAY_CALL_TO_ARMS_AB;
        default: return HOLIDAY_NONE;
    }
}

/**
  Function that returns the battleground type id from holiday id

  @param    holiday id
*/
BattleGroundTypeId BattleGroundMgr::WeekendHolidayIdToBgType(HolidayIds holiday)
{
    switch (holiday)
    {
        case HOLIDAY_CALL_TO_ARMS_AV: return BATTLEGROUND_AV;
        case HOLIDAY_CALL_TO_ARMS_EY: return BATTLEGROUND_EY;
        case HOLIDAY_CALL_TO_ARMS_WS: return BATTLEGROUND_WS;
        case HOLIDAY_CALL_TO_ARMS_SA: return BATTLEGROUND_SA;
        case HOLIDAY_CALL_TO_ARMS_AB: return BATTLEGROUND_AB;
        default: return BATTLEGROUND_TYPE_NONE;
    }
}

/**
  Function that checks if it's an active battleground holiday weekend

  @param    battleground type id
*/
bool BattleGroundMgr::IsBgWeekend(BattleGroundTypeId bgTypeId)
{
    return sGameEventMgr.IsActiveHoliday(BgTypeToWeekendHolidayId(bgTypeId));
}

/**
  Method that loads battleground events used in battleground scripts
*/
void BattleGroundMgr::LoadBattleEventIndexes(bool reload)
{
    BattleGroundEventIdx events;
    events.event1 = BG_EVENT_NONE;
    events.event2 = BG_EVENT_NONE;
    std::shared_ptr<GameObjectBattleEventIndexesMap> newGameObjectIndexes = std::make_shared<GameObjectBattleEventIndexesMap>();
    (*newGameObjectIndexes)[static_cast<uint32>(-1)] = events;
    std::shared_ptr<CreatureBattleEventIndexesMap> newCreatureIndexes = std::make_shared<CreatureBattleEventIndexesMap>();
    (*newCreatureIndexes)[static_cast<uint32>(-1)] = events;

    uint32 count = 0;

    auto queryResult =
        //                           0         1           2                3                4              5           6
        WorldDatabase.Query("SELECT data.typ, data.guid1, data.ev1 AS ev1, data.ev2 AS ev2, data.map AS m, data.guid2, description.map, "
                            //                              7                  8                   9
                            "description.event1, description.event2, description.description "
                            "FROM "
                            "(SELECT '1' AS typ, a.guid AS guid1, a.event1 AS ev1, a.event2 AS ev2, b.map AS map, b.guid AS guid2 "
                            "FROM gameobject_battleground AS a "
                            "LEFT OUTER JOIN gameobject AS b ON a.guid = b.guid "
                            "UNION "
                            "SELECT '2' AS typ, a.guid AS guid1, a.event1 AS ev1, a.event2 AS ev2, b.map AS map, b.guid AS guid2 "
                            "FROM creature_battleground AS a "
                            "LEFT OUTER JOIN creature AS b ON a.guid = b.guid "
                            ") data "
                            "RIGHT OUTER JOIN battleground_events AS description ON data.map = description.map "
                            "AND data.ev1 = description.event1 AND data.ev2 = description.event2 "
                            // full outer join doesn't work in mysql :-/ so just UNION-select the same again and add a left outer join
                            "UNION "
                            "SELECT data.typ, data.guid1, data.ev1, data.ev2, data.map, data.guid2, description.map, "
                            "description.event1, description.event2, description.description "
                            "FROM "
                            "(SELECT '1' AS typ, a.guid AS guid1, a.event1 AS ev1, a.event2 AS ev2, b.map AS map, b.guid AS guid2 "
                            "FROM gameobject_battleground AS a "
                            "LEFT OUTER JOIN gameobject AS b ON a.guid = b.guid "
                            "UNION "
                            "SELECT '2' AS typ, a.guid AS guid1, a.event1 AS ev1, a.event2 AS ev2, b.map AS map, b.guid AS guid2 "
                            "FROM creature_battleground AS a "
                            "LEFT OUTER JOIN creature AS b ON a.guid = b.guid "
                            ") data "
                            "LEFT OUTER JOIN battleground_events AS description ON data.map = description.map "
                            "AND data.ev1 = description.event1 AND data.ev2 = description.event2 "
                            "ORDER BY m, ev1, ev2");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outErrorDb(">> Loaded 0 battleground eventindexes.");
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();
        if (fields[2].GetUInt8() == BG_EVENT_NONE || fields[3].GetUInt8() == BG_EVENT_NONE)
            continue;                                       // we don't need to add those to the eventmap

        bool gameobject         = (fields[0].GetUInt8() == 1);
        uint32 dbTableGuidLow   = fields[1].GetUInt32();
        events.event1           = fields[2].GetUInt8();
        events.event2           = fields[3].GetUInt8();
        uint32 map              = fields[4].GetUInt32();

        uint32 desc_map = fields[6].GetUInt32();
        uint8 desc_event1 = fields[7].GetUInt8();
        uint8 desc_event2 = fields[8].GetUInt8();
        const char* description = fields[9].GetString();

        // checking for nullptr - through right outer join this will mean following:
        if (fields[5].GetUInt32() != dbTableGuidLow)
        {
            sLog.outErrorDb("BattleGroundEvent: %s with nonexistent guid %u for event: map:%u, event1:%u, event2:%u (\"%s\")",
                            (gameobject) ? "gameobject" : "creature", dbTableGuidLow, map, events.event1, events.event2, description);
            continue;
        }

        // checking for nullptr - through full outer join this can mean 2 things:
        if (desc_map != map)
        {
            // there is an event missing
            if (dbTableGuidLow == 0)
            {
                sLog.outErrorDb("BattleGroundEvent: missing db-data for map:%u, event1:%u, event2:%u (\"%s\")", desc_map, desc_event1, desc_event2, description);
                continue;
            }
            // we have an event which shouldn't exist
            sLog.outErrorDb("BattleGroundEvent: %s with guid %u is registered, for a nonexistent event: map:%u, event1:%u, event2:%u",
                    (gameobject) ? "gameobject" : "creature", dbTableGuidLow, map, events.event1, events.event2);
            continue;
        }

        if (gameobject)
            (*newGameObjectIndexes)[dbTableGuidLow] = events;
        else
            (*newCreatureIndexes)[dbTableGuidLow] = events;

        ++count;
    }
    while (queryResult->NextRow());

    m_gameObjectBattleEventIndexMap = newGameObjectIndexes;
    m_creatureBattleEventIndexMap = newCreatureIndexes;

    if (reload)
    {
        sMapMgr.DoForAllMaps([gameobjects = newGameObjectIndexes, creatures = newCreatureIndexes](Map* map)
        {
            map->GetMessager().AddMessage([gameobjects, creatures](Map* map)
            {
                map->GetMapDataContainer().SetGameObjectEventIndexes(gameobjects);
                map->GetMapDataContainer().SetCreatureEventIndexes(creatures);
            });
        });
    }

    sLog.outString(">> Loaded %u battleground eventindexes", count);
    sLog.outString();
}
