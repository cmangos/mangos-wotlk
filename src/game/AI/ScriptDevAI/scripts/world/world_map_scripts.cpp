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
SDName: world_map_scripts
SD%Complete: 100
SDComment: Quest support: 4740, 8868, 11538
SDCategory: World Map Scripts
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "world_map_scripts.h"
#include "World/WorldState.h"

/* *********************************************************
 *                  EASTERN KINGDOMS
 */
struct world_map_eastern_kingdoms : public ScriptedMap
{
    world_map_eastern_kingdoms(Map* pMap) : ScriptedMap(pMap) {}

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_NEZRAZ:
            case NPC_HINDENBURG:
            case NPC_ZAPETTA:
            case NPC_SQUIBBY_OVERSPECK:
            case NPC_JONATHAN:
            case NPC_WRYNN:
            case NPC_BOLVAR:
            case NPC_PRESTOR:
            case NPC_WINDSOR:
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
        }
    }

    void SetData(uint32 /*uiType*/, uint32 /*uiData*/) {}
};

InstanceData* GetInstanceData_world_map_eastern_kingdoms(Map* pMap)
{
    return new world_map_eastern_kingdoms(pMap);
}

struct GhostOPlasmEvent
{
    ObjectGuid guid;
    uint32 despawnTimer;
    uint8 phaseCounter;
    std::vector<ObjectGuid> summonedMagrami;
};

/* *********************************************************
 *                     KALIMDOR
 */
struct world_map_kalimdor : public ScriptedMap
{
    world_map_kalimdor(Map* pMap) : ScriptedMap(pMap) { Initialize(); }

    uint8 m_uiMurkdeepAdds_KilledAddCount;
    std::vector<GhostOPlasmEvent> m_vGOEvents;
    uint32 m_uiOmenResetTimer;
    uint32 m_uiOmenMoonlightTimer;
    uint8 m_uiRocketsCounter;
    uint32 m_encounter[MAX_ENCOUNTER];
    bool b_isOmenSpellCreditDone;
    std::array<std::vector<ObjectGuid>, MAX_ELEMENTS> m_aElementalRiftGUIDs;

    void Initialize()
    {
        m_uiMurkdeepAdds_KilledAddCount = 0;
        m_vGOEvents.clear();
        m_uiOmenResetTimer = 0;
        m_uiOmenMoonlightTimer = 0;
        m_uiRocketsCounter = 0;
        b_isOmenSpellCreditDone = false;
        for (auto& riftList : m_aElementalRiftGUIDs)
            riftList.clear();
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_FREZZA:
            case NPC_SNURK_BUCKSQUICK:
            case NPC_MURKDEEP:
            case NPC_OMEN:
            case NPC_AVALANCHION:
            case NPC_PRINCESS_TEMPESTRIA:
            case NPC_THE_WINDREAVER:
            case NPC_BARON_CHARR:
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_GREYMIST_COASTRUNNNER:
                if (pCreature->IsTemporarySummon())         // Only count the ones summoned for Murkdeep quest
                {
                    ++m_uiMurkdeepAdds_KilledAddCount;

                    // If all 3 coastrunners are killed, summon 2 warriors
                    if (m_uiMurkdeepAdds_KilledAddCount == 3)
                    {
                        float fX, fY, fZ;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            pCreature->GetRandomPoint(aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][0], aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][1], aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][2], 5.0f, fX, fY, fZ);

                            if (Creature* pTemp = pCreature->SummonCreature(NPC_GREYMIST_WARRIOR, fX, fY, fZ, aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][3], TEMPSPAWN_DEAD_DESPAWN, 0))
                            {
                                pTemp->SetWalk(false);
                                pTemp->GetRandomPoint(aSpawnLocations[POS_IDX_MURKDEEP_MOVE][0], aSpawnLocations[POS_IDX_MURKDEEP_MOVE][1], aSpawnLocations[POS_IDX_MURKDEEP_MOVE][2], 5.0f, fX, fY, fZ);
                                pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                            }
                        }

                        m_uiMurkdeepAdds_KilledAddCount = 0;
                    }
                }
                break;
            case NPC_GREYMIST_WARRIOR:
                if (pCreature->IsTemporarySummon())         // Only count the ones summoned for Murkdeep quest
                {
                    ++m_uiMurkdeepAdds_KilledAddCount;

                    // After the 2 warriors are killed, Murkdeep spawns, along with a hunter
                    if (m_uiMurkdeepAdds_KilledAddCount == 2)
                    {
                        float fX, fY, fZ;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            pCreature->GetRandomPoint(aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][0], aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][1], aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][2], 5.0f, fX, fY, fZ);

                            if (Creature* pTemp = pCreature->SummonCreature(!i ? NPC_MURKDEEP : NPC_GREYMIST_HUNTER, fX, fY, fZ, aSpawnLocations[POS_IDX_MURKDEEP_SPAWN][3], TEMPSPAWN_DEAD_DESPAWN, 0))
                            {
                                pTemp->SetWalk(false);
                                pTemp->GetRandomPoint(aSpawnLocations[POS_IDX_MURKDEEP_MOVE][0], aSpawnLocations[POS_IDX_MURKDEEP_MOVE][1], aSpawnLocations[POS_IDX_MURKDEEP_MOVE][2], 5.0f, fX, fY, fZ);
                                pTemp->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
                            }
                        }

                        m_uiMurkdeepAdds_KilledAddCount = 0;
                    }
                }
                break;
            case NPC_OMEN:
                SetData(TYPE_OMEN, DONE);
                break;
            case NPC_THE_WINDREAVER:
                DoDespawnElementalRifts(ELEMENTAL_AIR);
                break;
            case NPC_PRINCESS_TEMPESTRIA:
                DoDespawnElementalRifts(ELEMENTAL_WATER);
                break;
            case NPC_BARON_CHARR:
                DoDespawnElementalRifts(ELEMENTAL_FIRE);
                break;
            case NPC_AVALANCHION:
                DoDespawnElementalRifts(ELEMENTAL_EARTH);
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo) override
    {
        switch (pGo->GetEntry())
        {
            case GO_GHOST_MAGNET:
                m_vGOEvents.push_back({ pGo->GetObjectGuid(), 0, 0 }); // insert new event with 0 timer
                pGo->SetActiveObjectState(true);
                break;
            case GO_ROCKET_CLUSTER:
                m_goEntryGuidStore[GO_ROCKET_CLUSTER] = pGo->GetObjectGuid();
                break;
            case GO_EARTH_RIFT:
                m_aElementalRiftGUIDs[ELEMENTAL_EARTH].push_back(pGo->GetObjectGuid());
                break;
            case GO_WATER_RIFT:
                m_aElementalRiftGUIDs[ELEMENTAL_WATER].push_back(pGo->GetObjectGuid());
                break;
            case GO_FIRE_RIFT:
                m_aElementalRiftGUIDs[ELEMENTAL_FIRE].push_back(pGo->GetObjectGuid());
                break;
            case GO_AIR_RIFT:
                m_aElementalRiftGUIDs[ELEMENTAL_AIR].push_back(pGo->GetObjectGuid());
                break;
        }
    }

    void DoDespawnElementalRifts(uint8 index)
    {
        // Despawn all GO rifts for a given element type, erase the GUIDs for the GOs
        for (auto guid : m_aElementalRiftGUIDs[index])
        {
            if (GameObject* pRift = instance->GetGameObject(guid))
                pRift->SetLootState(GO_JUST_DEACTIVATED);
        }
        m_aElementalRiftGUIDs[index].clear();
    }

    // Custom function used for quest 6134
    bool GhostOPlasmEventStep(GhostOPlasmEvent& eventData)
    {
        if (eventData.despawnTimer > 3 * MINUTE * IN_MILLISECONDS)
        {
            for (auto guid : eventData.summonedMagrami)
                if (Creature* pMagrami = instance->GetCreature(guid))
                    if (pMagrami->isAlive()) // dont despawn corpses with loot
                        pMagrami->ForcedDespawn();

            // remove gameobject from map
            if (GameObject* pGo = instance->GetGameObject(eventData.guid))
            {
                pGo->SetActiveObjectState(false);
                pGo->SetLootState(GO_JUST_DEACTIVATED);
            }

            return false;
        }

        if (GameObject* pGo = instance->GetGameObject(eventData.guid))
        {
            if (eventData.despawnTimer / 15000 >= eventData.phaseCounter)
            {
                float fX, fY, fZ;
                pGo->GetPosition(fX, fY, fZ); // do some urand radius shenanigans to spawn it further and make it walk to go using doing X and Y yourself and using function in MAP to get proper Z
                uint32 uiRandom = urand(0, 35);
                float xR = fX + uiRandom, yR = fY + (40 - uiRandom), zR = fZ;
                instance->GetHeightInRange(pGo->GetPhaseMask(), xR, yR, zR);

                if (Creature* pCreature = pGo->SummonCreature(NPC_MAGRAMI_SPECTRE, xR, yR, zR, 0, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 3 * MINUTE * IN_MILLISECONDS))
                {
                    // add more timed logic here
                    instance->GetReachableRandomPointOnGround(pGo->GetPhaseMask(), fX, fY, fZ, 10.0f); // get position to which spectre will walk
                    eventData.phaseCounter++;
                    eventData.summonedMagrami.push_back(pCreature->GetObjectGuid());
                    pCreature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                }
            }

            return true;
        }
        else
            return false;
    }

    void Update(uint32 diff)
    {
        if (!m_vGOEvents.empty())
        {
            for (auto iter = m_vGOEvents.begin(); iter != m_vGOEvents.end();)
            {
                iter->despawnTimer += diff;
                if (!GhostOPlasmEventStep((*iter)))
                    iter = m_vGOEvents.erase(iter);
                else
                    ++iter;
            }
        }

        if (GetData(TYPE_OMEN) == DONE)
        {
            // Timer before Omen event reset (OOC)
            if (m_uiOmenResetTimer < diff)
            {
                if (Creature* pOmen = GetSingleCreatureFromStorage(NPC_OMEN))
                {
                    // Return is Omen is in fight
                    if (pOmen->isInCombat())
                        return;
                    else
                        pOmen->ForcedDespawn();
                }
                m_encounter[TYPE_OMEN] = NOT_STARTED;
                m_uiOmenResetTimer = 0;
                m_uiRocketsCounter = 0;
                m_uiOmenMoonlightTimer = 0;
                b_isOmenSpellCreditDone = false;
            }
            else
                m_uiOmenResetTimer -= diff;

            // Spell summoning GO traps for buff and quest credit for Omen
            if (!b_isOmenSpellCreditDone)
            {
                if (m_uiOmenMoonlightTimer < diff)
                {
                    if (Creature* pOmen = GetSingleCreatureFromStorage(NPC_OMEN))
                    {
                        pOmen->CastSpell(pOmen, SPELL_OMEN_MOONLIGHT, TRIGGERED_OLD_TRIGGERED);
                        b_isOmenSpellCreditDone = true;
                    }
                    m_uiOmenMoonlightTimer = 0;
                }
                else
                    m_uiOmenMoonlightTimer -= diff;
            }
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        if (uiType == TYPE_OMEN)
        {
            switch (uiData)
            {
                case NOT_STARTED:
                    // Count another rocket cluster launched
                    m_uiRocketsCounter++;
                    if (m_uiRocketsCounter < MAX_ROCKETS)
                    {
                        // 25% chance of spawning Minions of Omen (guessed), only if not already spawned (encounter is set to FAIL in that case)
                        if (GetData(TYPE_OMEN) == NOT_STARTED && urand(0, 1) < 1)
                            SetData(TYPE_OMEN, SPECIAL); // This will notify the GO to summon Omen's minions
                    }
                    // Set the event in motion and notify the GO to summon Omen
                    else if (GetData(TYPE_OMEN) != IN_PROGRESS && GetData(TYPE_OMEN) != DONE)   // Check that Omen is not already spawned and event is reset
                        SetData(TYPE_OMEN, IN_PROGRESS);

                    return; // Don't store NOT_STARTED data unless explicitly told so: we use it to count rockets
                case SPECIAL:
                    if (GameObject* pRocketCluster = GetSingleGameObjectFromStorage(GO_ROCKET_CLUSTER))
                    {
                        for (uint8 i = POS_IDX_MINION_OMEN_START ; i <= POS_IDX_MINION_OMEN_STOP ; i++)
                            pRocketCluster->SummonCreature(NPC_MINION_OMEN, aSpawnLocations[i][0], aSpawnLocations[i][1], aSpawnLocations[i][2], aSpawnLocations[i][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 15 * MINUTE * IN_MILLISECONDS);
                    }
                    break;
                case IN_PROGRESS:
                    if (GameObject* pRocketCluster = GetSingleGameObjectFromStorage(GO_ROCKET_CLUSTER))
                    {
                        if (Creature* pOmen = pRocketCluster->SummonCreature(NPC_OMEN, aSpawnLocations[POS_IDX_OMEN_SPAWN][0], aSpawnLocations[POS_IDX_OMEN_SPAWN][1], aSpawnLocations[POS_IDX_OMEN_SPAWN][2], aSpawnLocations[POS_IDX_OMEN_SPAWN][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 15 * MINUTE * IN_MILLISECONDS))
                        {
                            // Moving him to the lake bank
                            pOmen->SetWalk(true);
                            pOmen->GetMotionMaster()->MovePoint(1, aSpawnLocations[POS_IDX_OMEN_MOVE][0], aSpawnLocations[POS_IDX_OMEN_MOVE][1], aSpawnLocations[POS_IDX_OMEN_MOVE][2]);
                            m_uiOmenResetTimer = 15 * MINUTE * IN_MILLISECONDS; // Reset after 15 minutes if not engaged or defeated
                        }
                    }
                    break;
                case DONE:
                    m_uiOmenMoonlightTimer = 5 * IN_MILLISECONDS;            // Timer before casting the end quest spell
                    m_uiOmenResetTimer = 5 * MINUTE * IN_MILLISECONDS;        // Prevent another summoning of Omen for 5 minutes (based on spell duration)
                    break;
            }
        }
        m_encounter[uiType] = uiData;
    }

    uint32 GetData(uint32 uiType) const override { return m_encounter[uiType]; }
};

InstanceData* GetInstanceData_world_map_kalimdor(Map* pMap)
{
    return new world_map_kalimdor(pMap);
}

/* *********************************************************
 *                     OUTLAND
 */
struct world_map_outland : public ScriptedMap
{
    world_map_outland(Map* pMap) : ScriptedMap(pMap) { Initialize(); }

    uint8 m_uiEmissaryOfHate_KilledAddCount;

    void Initialize()
    {
        m_uiEmissaryOfHate_KilledAddCount = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_EMISSARY_OF_HATE:
                m_npcEntryGuidStore[NPC_EMISSARY_OF_HATE] = pCreature->GetObjectGuid();
                break;
            case NPC_VIMGOL_VISUAL_BUNNY:
                m_npcEntryGuidCollection[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
                break;
            case NPC_SOCRETHAR:
                m_npcEntryGuidStore[NPC_SOCRETHAR] = pCreature->GetObjectGuid();
                break;
        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_IRESPEAKER:
            case NPC_UNLEASHED_HELLION:
                if (!GetSingleCreatureFromStorage(NPC_EMISSARY_OF_HATE, true))
                {
                    ++m_uiEmissaryOfHate_KilledAddCount;
                    if (m_uiEmissaryOfHate_KilledAddCount == 6)
                    {
                        pCreature->SummonCreature(NPC_EMISSARY_OF_HATE, aSpawnLocations[POS_IDX_EMISSARY_SPAWN][0], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][1], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][2], aSpawnLocations[POS_IDX_EMISSARY_SPAWN][3], TEMPSPAWN_DEAD_DESPAWN, 0);
                        m_uiEmissaryOfHate_KilledAddCount = 0;
                    }
                }
                break;
        }
    }

    void OnObjectCreate(GameObject* go) override
    {
        switch (go->GetEntry())
        {
            case GO_ROCKET_FIRE:
            case GO_ROCKET_SMOKE:
                m_goEntryGuidCollection[go->GetEntry()].push_back(go->GetObjectGuid());
                std::sort(m_goEntryGuidCollection[go->GetEntry()].begin(), m_goEntryGuidCollection[go->GetEntry()].end());
                break;
        }
    }

    void SetData(uint32 /*uiType*/, uint32 /*uiData*/) override {}
};

InstanceData* GetInstanceData_world_map_outland(Map* pMap)
{
    return new world_map_outland(pMap);
}

/* *********************************************************
 *                     NORTHREND
 */
struct world_map_northrend : public ScriptedMap
{
    world_map_northrend(Map* pMap) : ScriptedMap(pMap) {}

    void SetData(uint32 /*uiType*/, uint32 /*uiData*/) {}
};

InstanceData* GetInstanceData_world_map_northrend(Map* pMap)
{
    return new world_map_northrend(pMap);
}

enum
{
    EVENT_UC_FROM_GROMGOL_ARRIVAL = 15312,
    EVENT_GROMGOL_FROM_UC_ARRIVAL = 15314,
    EVENT_OG_FROM_UC_ARRIVAL = 15318,
    EVENT_UC_FROM_OG_ARRIVAL = 15320,
    EVENT_OG_FROM_GROMGOL_ARRIVAL = 15322,
    EVENT_GROMGOL_FROM_OG_ARRIVAL = 15324,

    SOUND_ZEPPELIN_HORN = 11804,
};

bool ProcessEventTransports(uint32 uiEventId, Object* pSource, Object* pTarget, bool bIsStart)
{
    sWorldState.HandleConditionStateChange(pSource->GetEntry(), uiEventId);

    WorldObject* transport = (WorldObject*)pSource;
    uint32 entry = 0;
    switch (uiEventId)
    {
        case EVENT_UC_FROM_GROMGOL_ARRIVAL: // UC arrival from gromgol
            entry = NPC_HINDENBURG;
            break;
        case EVENT_GROMGOL_FROM_UC_ARRIVAL: // gromgol arrival from UC
            entry = NPC_SQUIBBY_OVERSPECK;
            break;
        case EVENT_OG_FROM_UC_ARRIVAL:      // OG arrival from UC
            entry = NPC_FREZZA;
            break;
        case EVENT_UC_FROM_OG_ARRIVAL:      // UC arrival from OG
            entry = NPC_ZAPETTA;
            break;
        case EVENT_OG_FROM_GROMGOL_ARRIVAL: // OG arrival from gromgol
            entry = NPC_SNURK_BUCKSQUICK;
            break;
        case EVENT_GROMGOL_FROM_OG_ARRIVAL: // gromgol arrival from OG
            entry = NPC_NEZRAZ;
            break;
    }
    if (entry)
        if (Creature* zeppelinMaster = ((ScriptedInstance*)transport->GetMap()->GetInstanceData())->GetSingleCreatureFromStorage(entry))
            zeppelinMaster->PlayDistanceSound(SOUND_ZEPPELIN_HORN);

    return true;
}

void AddSC_world_map_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "world_map_eastern_kingdoms";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_eastern_kingdoms;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "world_map_kalimdor";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_kalimdor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "world_map_outland";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_outland;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "world_map_northrend";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_northrend;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "event_transports";
    pNewScript->pProcessEventId = &ProcessEventTransports;
    pNewScript->RegisterSelf();
}
