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
#include "BattleGroundDS.h"
#include "Tools/Language.h"
#include "Globals/ObjectMgr.h"
#include "WorldPacket.h"

BattleGroundDS::BattleGroundDS()
{
    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    // we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

void BattleGroundDS::Update(uint32 diff)
{
    BattleGround::Update(diff);

    // push people from the tubes
    if (m_uiFlushTimer)
    {
        if (m_uiFlushTimer <= diff)
        {
            // cast flush spell
            for (GuidList::const_iterator itr = m_lGateTriggersGuids.begin(); itr != m_lGateTriggersGuids.end(); ++itr)
            {
                if (Creature* trigger = GetBgMap()->GetCreature(*itr))
                    trigger->CastSpell(trigger, BG_DS_SPELL_FLUSH, TRIGGERED_OLD_TRIGGERED);
            }

            // knockback players manually due to missing triggered spell 61698
            for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            {
                Player* plr = sObjectMgr.GetPlayer(itr->first);
                if (!plr)
                    continue;

                if (plr->GetPositionZ() < 11.0f)
                    continue;

                // knockback player depending on location
                if (plr->IsWithinDist2d(1249.43f, 764.601f, 40))
                    plr->KnockBackWithAngle(0, 55, 7);

                if (plr->IsWithinDist2d(1333.51f, 818.031f, 40))
                    plr->KnockBackWithAngle(3.10f, 55, 7);
            }

            m_uiFlushTimer = 0;
        }
        else
            m_uiFlushTimer -= diff;
    }

    // Waterfall
    if (m_uiWaterfallTimer < diff)
    {
        switch (m_uiWaterfallStage)
        {
            case 0:
                // prepare waterfall
                if (GameObject* pAnim = GetBgMap()->GetGameObject(m_waterfallAnimGuid))
                    pAnim->UseDoorOrButton();
                ++m_uiWaterfallStage;
                m_uiWaterfallTimer = 5000;
                break;
            case 1:
                // set collision
                if (GameObject* pCollision = GetBgMap()->GetGameObject(m_waterfallCollisionGuid))
                    pCollision->UseDoorOrButton();
                ++m_uiWaterfallStage;
                m_uiWaterfallSpellTimer = 1500;
                m_uiWaterfallTimer = 25000;
                break;
            case 2:
                // reset waterfall
                if (GameObject* pCollision = GetBgMap()->GetGameObject(m_waterfallCollisionGuid))
                    pCollision->ResetDoorOrButton();
                if (GameObject* pAnim = GetBgMap()->GetGameObject(m_waterfallAnimGuid))
                    pAnim->ResetDoorOrButton();
                m_uiWaterfallStage = 0;
                m_uiWaterfallSpellTimer = 0;
                m_uiWaterfallTimer = 30000;
                break;
        }
    }
    else
        m_uiWaterfallTimer -= diff;

    // cast water spout spell on timer
    if (m_uiWaterfallSpellTimer)
    {
        if (m_uiWaterfallSpellTimer <= diff)
        {
            if (Creature* trigger = GetBgMap()->GetCreature(m_waterfallTriggerGuid))
                trigger->CastSpell(trigger, BG_DS_SPELL_WATER_SPOUT, TRIGGERED_OLD_TRIGGERED);

            m_uiWaterfallSpellTimer = 1500;
        }
        else
            m_uiWaterfallSpellTimer -= diff;
    }
}

void BattleGroundDS::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);

    m_uiFlushTimer = 5000;
}

void BattleGroundDS::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundDSScore* sc = new BattleGroundDSScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
}

void BattleGroundDS::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

void BattleGroundDS::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog.outError("BattleGroundDS: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player,killer);

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

bool BattleGroundDS::HandlePlayerUnderMap(Player* player)
{
    player->TeleportTo(GetMapId(), 1299.046f, 784.825f, 9.338f, player->GetOrientation(), false);
    return true;
}

bool BattleGroundDS::HandleAreaTrigger(Player* player, uint32 triggerId)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return false;

    switch (triggerId)
    {
        case 5326:
            // player under the map check?
            player->TeleportTo(GetMapId(), 1299.046f, 784.825f, 9.338f, player->GetOrientation(), false);
            break;
        case 5347:
        case 5348:
            // safety check
            player->RemoveAurasDueToSpell(BG_DS_SPELL_DEMONIC_CIRCLE);
            break;
        default:
            return false;
    }
    return true;
}

void BattleGroundDS::HandleCreatureCreate(Creature* creature)
{
    if (creature->GetEntry() == BG_DS_CREATURE_WATER_SPOUT)
    {
        if (creature->GetPositionZ() < 10.0f)
            m_waterfallTriggerGuid = creature->GetObjectGuid();
        else
            m_lGateTriggersGuids.push_back(creature->GetObjectGuid());
    }
}

void BattleGroundDS::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_DS_GO_WATERFALL_COLLISION:
            m_waterfallCollisionGuid = go->GetObjectGuid();
            break;
        case BG_DS_GO_WATERFALL_ANIM:
            m_waterfallAnimGuid = go->GetObjectGuid();
            break;
    }
}

void BattleGroundDS::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    FillInitialWorldState(data, count, 0xe10, GetAlivePlayersCountByTeam(HORDE));
    FillInitialWorldState(data, count, 0xe1a, 1);
}

void BattleGroundDS::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    m_uiFlushTimer          = 0;
    m_uiWaterfallTimer      = urand(20000, 40000);
    m_uiWaterfallSpellTimer = 0;
    m_uiWaterfallStage      = 0;
}
