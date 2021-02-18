/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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

/* ScriptData
SDName: world_map_ebon_hold
SD%Complete: 100
SDComment: Handles the Light of Dawn quest script
SDCategory: Ebon Hold
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "world_map_ebon_hold.h"

world_map_ebon_hold::world_map_ebon_hold(Map* pMap) : ScriptedInstance(pMap),
    m_uiGothikYellTimer(0),
    m_uiBattleEncounter(0),
    m_uiBattleProgressTimer(0),
    m_uiTrapUsageTimer(0),
    m_uiDefendersDead(0),
    m_uiAttackersDead(0),
    m_uiAbominationsDead(0)
{
    Initialize();
}

void world_map_ebon_hold::Initialize() {}

void world_map_ebon_hold::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_HIGHLORD_DARION_MOGRAINE:
        case NPC_KOLTIRA_DEATHWEAVER:
        case NPC_ORBAZ_BLOODBANE:
        case NPC_THASSARIAN:

        case NPC_HIGHLORD_TIRION_FORDRING:
        case NPC_KORFAX_CHAMPION_OF_THE_LIGHT:
        case NPC_LORD_MAXWELL_TYROSUS:
        case NPC_LEONID_BARTHALOMEW_THE_REVERED:
        case NPC_DUKE_NICHOLAS_ZVERENHOFF:
        case NPC_COMMANDER_ELIGOR_DAWNBRINGER:
        case NPC_RIMBLAT_EARTHSHATTER:
        case NPC_RAYNE:

        case NPC_THE_LICH_KING:
        case NPC_HIGHLORD_ALEXANDROS_MOGRAINE:
        case NPC_DARION_MOGRAINE:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;

        case NPC_FLESH_BEHEMOTH:
            m_lFleshBehemothGuids.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_RAMPAGING_ABOMINATION:
        case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
        case NPC_VOLATILE_GHOUL:
            if (pCreature->IsTemporarySummon())
                m_lUndeadArmyGuids.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_DEFENDER_OF_THE_LIGHT:
            m_lLightDefendersGuids.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void world_map_ebon_hold::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_RAMPAGING_ABOMINATION:
        case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
        case NPC_VOLATILE_GHOUL:
            if (pCreature->IsTemporarySummon())
                pCreature->SetWalk(false);
            break;
    }
}

void world_map_ebon_hold::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // summon an abomination if a behemoth or abomination dies
        case NPC_FLESH_BEHEMOTH:
        {
            if (GetData(TYPE_BATTLE) != BATTLE_STATE_IN_PROGRESS && GetData(TYPE_BATTLE) != BATTLE_STATE_LOST)
                return;

            ++m_uiAttackersDead;
            DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE - m_uiAttackersDead);

            if (GetData(TYPE_BATTLE) == BATTLE_STATE_IN_PROGRESS)
            {
                if (Creature* pBehemoth = pCreature->SummonCreature(NPC_FLESH_BEHEMOTH, aAbominationLocations[0].m_fX, aAbominationLocations[0].m_fY, aAbominationLocations[0].m_fZ, aAbominationLocations[0].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0))
                {
                    pBehemoth->SetWalk(false);
                    pBehemoth->GetMotionMaster()->MovePoint(0, 2281.6511f, -5282.5884f, 82.54389f);
                }
            }

            if (!pCreature->IsTemporarySummon())
                pCreature->SetCorpseDelay(10);
            break;
        }
        case NPC_RAMPAGING_ABOMINATION:
        {
            if (GetData(TYPE_BATTLE) != BATTLE_STATE_IN_PROGRESS && GetData(TYPE_BATTLE) != BATTLE_STATE_LOST)
                return;

            ++m_uiAttackersDead;
            DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE - m_uiAttackersDead);

            ++m_uiAbominationsDead;
            if (m_uiAbominationsDead % MAX_ABOMINATIONS == 0 && GetData(TYPE_BATTLE) == BATTLE_STATE_IN_PROGRESS)
            {
                for (uint8 i = 0; i < MAX_ABOMINATIONS; ++i)
                {
                    if (Creature* pAbomination = pCreature->SummonCreature(aAbominationLocations[i].m_uiEntry, aAbominationLocations[i].m_fX, aAbominationLocations[i].m_fY, aAbominationLocations[i].m_fZ, aAbominationLocations[i].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0))
                    {
                        pAbomination->SetWalk(false);
                        pAbomination->GetMotionMaster()->MovePoint(0, 2281.6511f, -5282.5884f, 82.54389f);
                    }
                }
                m_uiAbominationsDead = 0;
            }
            break;
        }
        case NPC_VOLATILE_GHOUL:
        case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
        {
            if (GetData(TYPE_BATTLE) != BATTLE_STATE_IN_PROGRESS && GetData(TYPE_BATTLE) != BATTLE_STATE_LOST)
                return;

            ++m_uiAttackersDead;
            DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE - m_uiAttackersDead);

            if (m_uiAttackersDead % MAX_UNDEAD_WARRIORS == 0 && GetData(TYPE_BATTLE) == BATTLE_STATE_IN_PROGRESS)
                DoSpawnUndeadWave();
            break;
        }
        case NPC_DEFENDER_OF_THE_LIGHT:
            ++m_uiDefendersDead;
            DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT - m_uiDefendersDead);
            // no break;
        case NPC_KORFAX_CHAMPION_OF_THE_LIGHT:
        case NPC_LORD_MAXWELL_TYROSUS:
        case NPC_COMMANDER_ELIGOR_DAWNBRINGER:
        case NPC_LEONID_BARTHALOMEW_THE_REVERED:
        case NPC_DUKE_NICHOLAS_ZVERENHOFF:
        case NPC_RIMBLAT_EARTHSHATTER:
        case NPC_RAYNE:
            pCreature->SummonCreature(pCreature->GetEntry(), pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), pCreature->GetOrientation(), TEMPSPAWN_CORPSE_DESPAWN, 0);
            break;
    }
}

void world_map_ebon_hold::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_LIGHT_OF_DAWN:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_HOLY_LIGHTNING_1:
        case GO_HOLY_LIGHTNING_2:
            m_lLightTrapsGuids.push_back(pGo->GetObjectGuid());
            break;
    }
}

void world_map_ebon_hold::SetData(uint32 uiType, uint32 uiData)
{
    if (uiType == TYPE_BATTLE)
    {
        m_uiBattleEncounter = uiData;

        switch (uiData)
        {
            case BATTLE_STATE_NOT_STARTED:
                // no action during this state
                break;
            case BATTLE_STATE_PREPARE:
                // display timer
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_TIME, MAX_BATTLE_INTRO_TIMER);

                // update world states to also show the army
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE);

                m_uiBattleProgressTimer = TIMER_BATTLE_PREPARE;
                break;
            case BATTLE_STATE_IN_PROGRESS:
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 0);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_BEGIN, 1);

                // force resapwn creatures just in case
                DoResetBattleScene();

                m_uiBattleProgressTimer = TIMER_BATTLE_PROGRESS;
                m_uiAttackersDead = 0;
                m_uiDefendersDead = 0;
                break;
            case BATTLE_STATE_LOST:
                // enable the holy traps
                DoEnableHolyTraps();
                m_uiTrapUsageTimer = 1000;
                break;
            case BATTLE_STATE_DIALOGUE:
                // despawn light defenders; new ones will spawn for the dialogue
                for (const auto& guid : m_lLightDefendersGuids)
                    if (Creature* pTemp = instance->GetCreature(guid))
                        pTemp->ForcedDespawn();

                // kill undead army if any left
                DoDefeatUndeadArmy();
                break;
            case BATTLE_STATE_WAIT_QUEST:
                // event is complete; wait for players to complete quest
                m_uiBattleProgressTimer = TIMER_BATTLE_QUEST_WAIT;
                break;
            case BATTLE_STATE_RESET:
                // update world states to default
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE);

                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 0);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_BEGIN, 0);

                // set timer to reset the scene
                m_uiBattleProgressTimer = TIMER_BATTLE_RESET_DELAY;
                break;
        }
    }
}

uint32 world_map_ebon_hold::GetData(uint32 uiType) const
{
    if (uiType == TYPE_BATTLE)
        return m_uiBattleEncounter;

    return 0;
}

// update world states for Light of Dawn event
void world_map_ebon_hold::DoUpdateBattleWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for (const auto& lPlayer : lPlayers)
        if (Player* pPlayer = lPlayer.getSource())
            if (pPlayer->HasAura(SPELL_CHAPTER_IV) || pPlayer->IsGameMaster())
                pPlayer->SendUpdateWorldState(uiStateId, uiStateData);
}

// update zone weather for Light of Dawn event
void world_map_ebon_hold::DoUpdateBattleWeatherData(uint32 weatherId, uint32 weatherGrade)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for (const auto& lPlayer : lPlayers)
        if (Player* pPlayer = lPlayer.getSource())
            if (pPlayer->HasAura(SPELL_CHAPTER_IV) || pPlayer->IsGameMaster())
                pPlayer->SendWeatherUpdate(weatherId, weatherGrade);
}

// update zone light override for Light of Dawn event
void world_map_ebon_hold::DoUpdateBattleZoneLightData(uint32 lightId, uint32 fadeInTime)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for (const auto& lPlayer : lPlayers)
        if (Player* pPlayer = lPlayer.getSource())
            if (pPlayer->HasAura(SPELL_CHAPTER_IV) || pPlayer->IsGameMaster())
                pPlayer->SendOverrideLightUpdate(lightId, fadeInTime);
}

// Wrapper to despawn all creatures involved in the Light of Dawn event
void world_map_ebon_hold::DoClearBattleScene()
{
    // despawn light champions
    for (auto& entry : aLightChampions)
        if (Creature* pTemp = GetSingleCreatureFromStorage(entry))
            pTemp->ForcedDespawn();

    // despawn scourge champions
    for (auto& entry : aScourgeChampions)
        if (Creature* pTemp = GetSingleCreatureFromStorage(entry))
            pTemp->ForcedDespawn();

    // despawn light defenders
    for (const auto& guid : m_lLightDefendersGuids)
        if (Creature* pTemp = instance->GetCreature(guid))
            pTemp->ForcedDespawn();

    // despawn Tirion
    if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
        pTirion->ForcedDespawn();
}

// Wrapper to respawn all creatures involved in the Light of Dawn event
void world_map_ebon_hold::DoResetBattleScene()
{
    // respawn scourge champions
    for (auto& entry : aScourgeChampions)
        if (Creature* pTemp = GetSingleCreatureFromStorage(entry))
            pTemp->Respawn();

    // respawn flesh behemoths
    for (const auto& guid : m_lFleshBehemothGuids)
        if (Creature* pTemp = instance->GetCreature(guid))
            pTemp->Respawn();
}

// Wrapper to kill all undead before the final dialogue
void world_map_ebon_hold::DoDefeatUndeadArmy()
{
    // suicide all army units when the battle is finished
    for (const auto& guid : m_lUndeadArmyGuids)
        if (Creature* pTemp = instance->GetCreature(guid))
            if (pTemp->IsAlive())
                pTemp->Suicide();

    for (const auto& guid : m_lFleshBehemothGuids)
        if (Creature* pTemp = instance->GetCreature(guid))
            if (pTemp->IsAlive())
                pTemp->Suicide();
}

// Temporarely spawn the holy traps at the end of the Light of Dawn event
void world_map_ebon_hold::DoEnableHolyTraps()
{
    for (const auto& guid : m_lLightTrapsGuids)
        DoRespawnGameObject(guid, 25);
}

// Spawn new wave of undead warriors
void world_map_ebon_hold::DoSpawnUndeadWave()
{
    GameObject* pSource = GetSingleGameObjectFromStorage(GO_LIGHT_OF_DAWN);
    if (!pSource)
    {
        script_error_log("Ebon Hold: ERROR cannot find gameobject with entry: %u", GO_LIGHT_OF_DAWN);
        return;
    }

    float fX, fY, fZ;
    for (uint8 i = 0; i < MAX_UNDEAD_WARRIORS; ++i)
    {
        uint32 entry = roll_chance_i(20) ? NPC_WARRIOR_OF_THE_FROZEN_WASTES : NPC_VOLATILE_GHOUL;

        pSource->GetRandomPoint(pSource->GetPositionX(), pSource->GetPositionY(), pSource->GetPositionZ(), 50.0f, fX, fY, fZ);
        pSource->SummonCreature(entry, fX, fY, fZ, 0, TEMPSPAWN_CORPSE_DESPAWN, 0);
    }

    // first warriors respawn will call the abominations as well
    if (m_uiAttackersDead < MAX_UNDEAD_WARRIORS * 2)
    {
        for (uint8 i = 0; i < MAX_ABOMINATIONS; ++i)
        {
            if (Creature* pAbomination = pSource->SummonCreature(aAbominationLocations[i].m_uiEntry, aAbominationLocations[i].m_fX, aAbominationLocations[i].m_fY, aAbominationLocations[i].m_fZ, aAbominationLocations[i].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0))
            {
                pAbomination->SetWalk(false);
                pAbomination->GetMotionMaster()->MovePoint(0, pSource->GetPositionX(), pSource->GetPositionY(), pSource->GetPositionZ());
            }
        }
    }
}

void world_map_ebon_hold::Update(uint32 uiDiff)
{
    if (m_uiGothikYellTimer)
    {
        if (m_uiGothikYellTimer <= uiDiff)
            m_uiGothikYellTimer = 0;
        else
            m_uiGothikYellTimer -= uiDiff;
    }

    // Light of Dawn battle preparation
    if (GetData(TYPE_BATTLE) == BATTLE_STATE_PREPARE)
    {
        if (m_uiBattleProgressTimer)
        {
            if (m_uiBattleProgressTimer <= uiDiff)
            {
                Creature* pDarion = GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE);
                if (!pDarion)
                {
                    script_error_log("Ebon Hold: ERROR cannot find creature with entry: %u", NPC_HIGHLORD_DARION_MOGRAINE);

                    SetData(TYPE_BATTLE, BATTLE_STATE_RESET);
                    m_uiBattleProgressTimer = 0;
                    return;
                }

                m_uiBattleProgressTimer = 0;
                pDarion->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pDarion, pDarion);

                SetData(TYPE_BATTLE, BATTLE_STATE_IN_PROGRESS);
            }
            else
            {
                m_uiBattleProgressTimer -= uiDiff;

                if (m_uiBattleProgressTimer / IN_MILLISECONDS % 60 == 0)
                    DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_TIME, m_uiBattleProgressTimer / (MINUTE * IN_MILLISECONDS));
            }
        }
    }
    // Light of Dawn battle execution
    else if (GetData(TYPE_BATTLE) == BATTLE_STATE_IN_PROGRESS)
    {
        if (m_uiBattleProgressTimer)
        {
            if (m_uiBattleProgressTimer <= uiDiff || m_uiDefendersDead >= 100)
            {
                Creature* pDarion = GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE);
                if (!pDarion)
                {
                    script_error_log("Ebon Hold: ERROR cannot find creature with entry: %u", NPC_HIGHLORD_DARION_MOGRAINE);

                    SetData(TYPE_BATTLE, BATTLE_STATE_RESET);
                    m_uiBattleProgressTimer = 0;
                    return;
                }

                // inform Darion about the battle lost
                m_uiBattleProgressTimer = 0;
                pDarion->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, pDarion, pDarion);

                SetData(TYPE_BATTLE, BATTLE_STATE_LOST);
            }
            else
                m_uiBattleProgressTimer -= uiDiff;
        }
    }
    // Kill the undead army using the light traps
    else if (GetData(TYPE_BATTLE) == BATTLE_STATE_LOST)
    {
        if (m_uiTrapUsageTimer < uiDiff)
        {
            if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
                for (const auto& guid : m_lLightTrapsGuids)
                    if (GameObject* pTrap = instance->GetGameObject(guid))
                        pTrap->Use(pTirion);

            DoSpawnUndeadWave();
            m_uiTrapUsageTimer = urand(3000, 4000);
        }
        else
            m_uiTrapUsageTimer -= uiDiff;
    }
    // Wait for players to complete quest; then clear the scene
    else if (GetData(TYPE_BATTLE) == BATTLE_STATE_WAIT_QUEST)
    {
        if (m_uiBattleProgressTimer)
        {
            if (m_uiBattleProgressTimer <= uiDiff)
            {
                DoClearBattleScene();
                m_uiBattleProgressTimer = 0;
                SetData(TYPE_BATTLE, BATTLE_STATE_RESET);
            }
            else
                m_uiBattleProgressTimer -= uiDiff;
        }
    }
    // Light of Dawn reset
    else if (GetData(TYPE_BATTLE) == BATTLE_STATE_RESET)
    {
        if (m_uiBattleProgressTimer)
        {
            if (m_uiBattleProgressTimer <= uiDiff)
            {
                DoResetBattleScene();
                m_uiBattleProgressTimer = 0;
                SetData(TYPE_BATTLE, BATTLE_STATE_NOT_STARTED);
            }
            else
                m_uiBattleProgressTimer -= uiDiff;
        }
    }
}

bool world_map_ebon_hold::CanAndToggleGothikYell()
{
    if (m_uiGothikYellTimer)
        return false;

    m_uiGothikYellTimer = 2000;
    return true;
}

InstanceData* GetInstance_world_map_ebon_hold(Map* pMap)
{
    return new world_map_ebon_hold(pMap);
}

void AddSC_world_map_ebon_hold()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "world_map_ebon_hold";
    pNewScript->GetInstanceData = &GetInstance_world_map_ebon_hold;
    pNewScript->RegisterSelf();
}
