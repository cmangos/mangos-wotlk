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
SD%Complete: 0
SDComment:
SDCategory: Ebon Hold
EndScriptData */

#include "precompiled.h"
#include "world_map_ebon_hold.h"

world_map_ebon_hold::world_map_ebon_hold(Map* pMap) : ScriptedInstance(pMap),
    m_uiGothikYellTimer(0),
    m_uiBattleEncounter(0)
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
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;

            // Behemots and abominations are spawned by default on the map so they need to be handled here
        case NPC_FLESH_BEHEMOTH:
        case NPC_RAMPAGING_ABOMINATION:
            m_lArmyGuids.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void world_map_ebon_hold::OnCreatureDeath(Creature* pCreature)
{
    if (GetData(TYPE_BATTLE) != IN_PROGRESS)
        return;

    switch (pCreature->GetEntry())
    {
            // resummon the behemots or abominations if they die
        case NPC_FLESH_BEHEMOTH:
        case NPC_RAMPAGING_ABOMINATION:
            m_lArmyGuids.remove(pCreature->GetObjectGuid());// if remove respawning on reset won't work! (are there any spawned by default?) ?? - unclear related to ResetBattle()
            if (Creature* pTemp = pCreature->SummonCreature(pCreature->GetEntry(), pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), pCreature->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                // the new summoned mob should attack
                Creature* pDarion = GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE);
                if (pDarion && pDarion->getVictim())
                    pTemp->AI()->AttackStart(pDarion->getVictim());
            }
            pCreature->ForcedDespawn(1000);
            break;
    }
}

void world_map_ebon_hold::OnCreatureEvade(Creature* pCreature)
{
    if (GetData(TYPE_BATTLE) != IN_PROGRESS)
        return;

    switch (pCreature->GetEntry())
    {
            // don't let the scourge evade while the battle is running
        case NPC_FLESH_BEHEMOTH:
        case NPC_RAMPAGING_ABOMINATION:
        case NPC_VOLATILE_GHOUL:
        case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
            if (Creature* pDarion = GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE))
            {
                if (!pDarion->isInCombat())
                    return;

                if (Unit* pTarget = pDarion->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pCreature->AI()->AttackStart(pTarget);
            }
        case NPC_KORFAX_CHAMPION_OF_THE_LIGHT:
        case NPC_LORD_MAXWELL_TYROSUS:
        case NPC_COMMANDER_ELIGOR_DAWNBRINGER:
        case NPC_LEONID_BARTHALOMEW_THE_REVERED:
        case NPC_DUKE_NICHOLAS_ZVERENHOFF:
        case NPC_RIMBLAT_EARTHSHATTER:
        case NPC_RAYNE:
        case NPC_DEFENDER_OF_THE_LIGHT:
            if (Creature* pDarion = GetSingleCreatureFromStorage(NPC_HIGHLORD_DARION_MOGRAINE))
                pCreature->AI()->AttackStart(pDarion);
            break;
    }
}

void world_map_ebon_hold::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_LIGHT_OF_DAWN:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
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
        switch (uiData)
        {
            case NOT_STARTED:
                // update world states to default
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE);

                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 0);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_BEGIN, 0);

                DoResetBattle();
                break;
            case SPECIAL:
                // display timer
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_TIME, MAX_BATTLE_INTRO_TIMER);

                // update world states to also show the army
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SHOW, 1);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT);
                DoUpdateBattleWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE);
                break;
            case IN_PROGRESS:
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_TIMER_SHOW, 0);
                DoUpdateBattleWorldState(WORLD_STATE_BATTLE_BEGIN, 1);
                break;
        }

        m_uiBattleEncounter = uiData;
    }
}

uint32 world_map_ebon_hold::GetData(uint32 uiType) const
{
    if (uiType == TYPE_BATTLE)
        return m_uiBattleEncounter;

    return 0;
}

void world_map_ebon_hold::DoUpdateBattleWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
    {
        if (Player* pPlayer = itr->getSource())
        {
            // we need to manually check the phase mask because the value from DBC is not used yet
            if (pPlayer->HasAura(SPELL_CHAPTER_IV) || pPlayer->isGameMaster())
                pPlayer->SendUpdateWorldState(uiStateId, uiStateData);
        }
    }
}

void world_map_ebon_hold::DoResetBattle()
{
    // reset all npcs to the original state
    if (Creature* pKoltira = GetSingleCreatureFromStorage(NPC_KOLTIRA_DEATHWEAVER))
        pKoltira->Respawn();
    if (Creature* pThassarian = GetSingleCreatureFromStorage(NPC_THASSARIAN))
        pThassarian->Respawn();
    if (Creature* pOrbaz = GetSingleCreatureFromStorage(NPC_ORBAZ_BLOODBANE))
        pOrbaz->Respawn();

    // respawn all abominations
    for (GuidList::const_iterator itr = m_lArmyGuids.begin(); itr != m_lArmyGuids.end(); ++itr)
    {
        if (Creature* pTemp = instance->GetCreature(*itr))
            pTemp->Respawn();
    }

    // despawn the argent dawn
    for (uint8 i = 0; i < MAX_LIGHT_CHAMPIONS; i++)
    {
        if (Creature* pTemp = GetSingleCreatureFromStorage(aLightArmySpawnLoc[i].m_uiEntry))
            pTemp->ForcedDespawn();
    }

    if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_HIGHLORD_TIRION_FORDRING))
        pTirion->ForcedDespawn();
}

void world_map_ebon_hold::DoMoveArmy()
{
    // move all the army to the chapel
    float fX, fY, fZ;
    for (GuidList::const_iterator itr = m_lArmyGuids.begin(); itr != m_lArmyGuids.end(); ++itr)
    {
        if (Creature* pTemp = instance->GetCreature(*itr))
        {
            pTemp->SetWalk(false);
            pTemp->GetRandomPoint(aEventLocations[1].m_fX, aEventLocations[1].m_fY, aEventLocations[1].m_fZ, 30.0f, fX, fY, fZ);
            pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
        }
    }
}

void world_map_ebon_hold::DoDespawnArmy()
{
    // despawn all army units when the battle is finished
    for (GuidList::const_iterator itr = m_lArmyGuids.begin(); itr != m_lArmyGuids.end(); ++itr)
    {
        if (Creature* pTemp = instance->GetCreature(*itr))
        {
            if (pTemp->isAlive())
                pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
}

void world_map_ebon_hold::DoEnableHolyTraps()
{
    for (GuidList::const_iterator itr = m_lLightTrapsGuids.begin(); itr != m_lLightTrapsGuids.end(); ++itr)
        DoRespawnGameObject(*itr, 25);
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
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "world_map_ebon_hold";
    pNewScript->GetInstanceData = &GetInstance_world_map_ebon_hold;
    pNewScript->RegisterSelf();
}
