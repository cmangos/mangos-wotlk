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

#include "world_eastern_kingdoms.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/world/world_map_scripts.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "World/WorldState.h"
#include "World/WorldStateDefines.h"
#include "GameEvents/GameEventMgr.h"
#include "AI/ScriptDevAI/scripts/world/brewfest.h"
#include "AI/ScriptDevAI/scripts/world/scourge_invasion.h"

/* *********************************************************
 *                  EASTERN KINGDOMS
 */
struct world_map_eastern_kingdoms : public ScriptedMap, public TimerManager
{
    world_map_eastern_kingdoms(Map* pMap) : ScriptedMap(pMap), m_brewfestEvent(this), m_shadeData({ AREAID_GOLDSHIRE, AREAID_KHARANOS, AREAID_BRILL })
    {
        AddCustomAction(EVENT_SPAWN, true, [&]
        {
            if (_spawn.size() < 16)
            {
                switch (urand(0, 4))
                {
                    case 0: DoSpawn(NPC_INVADING_FELGUARD); break;
                    case 1: DoSpawn(NPC_INVADING_VOIDWALKER); break;
                    case 2: DoSpawn(NPC_INVADING_INFERNAL); break;
                    case 3: DoSpawn(NPC_INVADING_FEL_STALKER); break;
                    case 4: DoSpawn(NPC_INVADING_ANGUISHER); break;
                }
            }
            ResetTimer(EVENT_SPAWN, urand(1000, 5000));
        });
        AddCustomAction(EVENT_SPAWN_BOSS, true, [&]
        {
            if (_spawnBoss.size() < 1)
            {
                urand(0, 1) ? ResetTimer(EVENT_SPAWN_DREADKNIGHT, urand(5000, 10000)) : ResetTimer(EVENT_SPAWN_FELGUARD_LIEUTENANT, urand(5000, 10000));
            }
            ResetTimer(EVENT_SPAWN_BOSS, urand(20000, 30000));
        });
        AddCustomAction(EVENT_SPAWN_DREADKNIGHT, true, [&]
        {
            DoSpawn(NPC_DREADKNIGHT);
            DoSpawn(NPC_INVADING_INFERNAL);
            DoSpawn(NPC_INVADING_INFERNAL);
        });
        AddCustomAction(EVENT_SPAWN_FELGUARD_LIEUTENANT, true, [&]
        {
            DoSpawn(NPC_FELGUARD_LIEUTENANT);
            DoSpawn(NPC_INVADING_FEL_STALKER);
            DoSpawn(NPC_INVADING_ANGUISHER);
        });
        AddCustomAction(EVENT_REINFORCEMENTS_NEEDED, true, [&]
        {
            if (_spawnSupport.size() < 1)
            {
                if (_spawnProtector.size() < 4 || _spawnGuardian.size() < 4)
                {
                    ResetTimer(EVENT_SPAWN_REINFORCEMENTS, 1000);
                    ResetTimer(EVENT_REINFORCEMENTS_LEADER_SPEAK, urand(45000, 90000));
                }
            }
            ResetTimer(EVENT_REINFORCEMENTS_NEEDED, urand(5000, 10000));
        });
        AddCustomAction(EVENT_SPAWN_REINFORCEMENTS, true, [&]
        {
            switch (urand(0, 2))
            {
                case 0: DoSpawn(NPC_LORD_MARSHAL_RAYNOR); break;
                case 1: DoSpawn(NPC_JUSTINIUS_THE_HARBINGER); break;
                case 2: DoSpawn(NPC_MELGROMM_HIGHMOUNTAIN); break;
            }
        });
        AddCustomAction(EVENT_REINFORCEMENTS_LEADER_SPEAK, true, [&]
        {
            if (_spawnSupport.size() > 0)
            {
                uint32 randomScriptTextId = 0;

                for (std::set<ObjectGuid>::iterator it = _spawnSupport.begin(); it != _spawnSupport.end(); ++it)
                {
                    if (Creature* add = instance->GetCreature(*it))
                    {
                        switch (add->GetEntry())
                        {
                            case NPC_LORD_MARSHAL_RAYNOR:
                                switch (urand(0, 4))
                                {
                                    case 0: randomScriptTextId = LMR_RANDOM_TEXT_1; break;
                                    case 1: randomScriptTextId = LMR_RANDOM_TEXT_2; break;
                                    case 2: randomScriptTextId = LMR_RANDOM_TEXT_3; break;
                                    case 3: randomScriptTextId = LMR_RANDOM_TEXT_4; break;
                                    case 4: randomScriptTextId = LMR_RANDOM_TEXT_5; break;
                                }
                                break;
                            case NPC_JUSTINIUS_THE_HARBINGER:
                                switch (urand(0, 3))
                                {
                                    case 0: randomScriptTextId = JTH_RANDOM_TEXT_1; break;
                                    case 1: randomScriptTextId = JTH_RANDOM_TEXT_2; break;
                                    case 2: randomScriptTextId = JTH_RANDOM_TEXT_3; break;
                                }
                                break;
                            case NPC_MELGROMM_HIGHMOUNTAIN:
                                switch (urand(0, 5))
                                {
                                    case 0: randomScriptTextId = MH_RANDOM_TEXT_1; break;
                                    case 1: randomScriptTextId = MH_RANDOM_TEXT_2; break;
                                    case 2: randomScriptTextId = MH_RANDOM_TEXT_3; break;
                                    case 3: randomScriptTextId = MH_RANDOM_TEXT_4; break;
                                    case 4: randomScriptTextId = MH_RANDOM_TEXT_5; break;
                                    case 5: randomScriptTextId = MH_RANDOM_TEXT_6; break;
                                }
                                break;
                        }
                        DoScriptText(randomScriptTextId, add);
                    }
                }
            }
            ResetTimer(EVENT_REINFORCEMENTS_LEADER_SPEAK, urand(60000, 90000));
        });
        Initialize();
    }

    std::set<ObjectGuid> _spawn;
    std::set<ObjectGuid> _spawnBoss;
    std::set<ObjectGuid> _spawnSupport;
    std::set<ObjectGuid> _spawnProtector;
    std::set<ObjectGuid> _spawnGuardian;

    Position const spawnPortalPos = { -11900.3f, -3207.62f, -14.7534f, 0.146405f };
    Position const spawnPortalPos1 = { -11901.25f, -3202.272f, -14.7534f, 0.146405f };
    Position const spawnBossPortalPos = { -11891.500f, -3207.010f, -14.798f, 0.146405f };
    Position const spawnReinforcementPos = { -11815.1f, -3190.39f, -30.7338f, 3.32447f };

    // Shade of the Horseman village attack event
    ShadeOfTheHorsemanData m_shadeData;
    // Brewfest events
    BrewfestEvent m_brewfestEvent;

    void Initialize() override
    {
        m_shadeData.Reset();
    }

    bool CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const override
    {
        if (instanceConditionId >= INSTANCE_CONDITION_ID_FIRE_BRIGADE_PRACTICE_GOLDSHIRE && instanceConditionId <= INSTANCE_CONDITION_ID_LET_THE_FIRES_COME_HORDE)
            return m_shadeData.IsConditionFulfilled(instanceConditionId, player->GetAreaId());

        script_error_log("world_map_eastern_kingdoms::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
            instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
        return false;
    }

    void OnCreatureCreate(Creature* pCreature) override
    {
        switch (pCreature->GetEntry())
        {
            case NPC_NEZRAZ:
            case NPC_HINDENBURG:
            case NPC_ZAPETTA:
            case NPC_MEEFI_FARTHROTTLE:
            case NPC_SQUIBBY_OVERSPECK:
            case NPC_JONATHAN:
            case NPC_WRYNN:
            case NPC_BOLVAR:
            case NPC_PRESTOR:
            case NPC_WINDSOR:
            case NPC_HIGHLORD_KRUUL:
            case NPC_AGENT_PROUDWELL:
            case NPC_FALSTAD_WILDHAMMER:
            case NPC_SHORT_JOHN_MITHRIL:
            case NPC_BELBI_QUIKSWITCH:
            case NPC_ITA_THUNDERBREW:
            case NPC_MAEVE_BARLEYBREW:
            case NPC_GORDOK_BREW_BARKER:
            case NPC_IPFELKOFER_IRONKEG:
            case NPC_MEKKATORQUE:
            case NPC_DARK_IRON_HERALD:
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
            case NPC_DREADKNIGHT:
            case NPC_FELGUARD_LIEUTENANT:
                _spawnBoss.insert(pCreature->GetObjectGuid());
                break;
            case NPC_LORD_MARSHAL_RAYNOR:
            case NPC_JUSTINIUS_THE_HARBINGER:
            case NPC_MELGROMM_HIGHMOUNTAIN:
                _spawnSupport.insert(pCreature->GetObjectGuid());
                break;
            case NPC_INVADING_INFERNAL:
            case NPC_INVADING_FELGUARD:
            case NPC_INVADING_FEL_STALKER:
            case NPC_INVADING_VOIDWALKER:
            case NPC_INVADING_ANGUISHER:
                _spawn.insert(pCreature->GetObjectGuid());
                break;
            case NPC_ARGENT_GUARDIAN:
                _spawnGuardian.insert(pCreature->GetObjectGuid());
                break;
            case NPC_ARGENT_PROTECTOR:
                _spawnProtector.insert(pCreature->GetObjectGuid());
                break;
            case NPC_NECROPOLIS_HEALTH:
                m_npcEntryGuidCollection[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
                break;
        }
    }

    void OnCreatureDeath(Creature* pCreature) override
    {
        switch (pCreature->GetEntry())
        {
            case NPC_DREADKNIGHT:
            case NPC_FELGUARD_LIEUTENANT:
                _spawnBoss.erase(pCreature->GetObjectGuid());
                break;
            case NPC_LORD_MARSHAL_RAYNOR:
            case NPC_JUSTINIUS_THE_HARBINGER:
            case NPC_MELGROMM_HIGHMOUNTAIN:
                _spawnSupport.erase(pCreature->GetObjectGuid());
                break;
            case NPC_ARGENT_GUARDIAN:
                _spawnGuardian.erase(pCreature->GetObjectGuid());
                break;
            case NPC_ARGENT_PROTECTOR:
                _spawnProtector.erase(pCreature->GetObjectGuid());
                break;
            case NPC_NECROPOLIS_HEALTH:
                m_npcEntryGuidCollection.erase(pCreature->GetObjectGuid());
                break;
            default:
                _spawn.erase(pCreature->GetObjectGuid());
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo) override
    {
        switch (pGo->GetEntry())
        {
            case GO_SUMMON_CIRCLE:
                m_goEntryGuidCollection[pGo->GetEntry()].push_back(pGo->GetObjectGuid());
                break;
        }
    }

    void DespawnAdds()
    {
        for (std::set<ObjectGuid>::iterator it = _spawn.begin(); it != _spawn.end(); ++it)
            if (Creature* add = instance->GetCreature(*it))
                add->AddObjectToRemoveList();
        _spawn.clear();

        for (std::set<ObjectGuid>::iterator it = _spawnSupport.begin(); it != _spawnSupport.end(); ++it)
            if (Creature* add = instance->GetCreature(*it))
                add->AddObjectToRemoveList();
        _spawnSupport.clear();

        for (std::set<ObjectGuid>::iterator it = _spawnBoss.begin(); it != _spawnBoss.end(); ++it)
            if (Creature* add = instance->GetCreature(*it))
                add->AddObjectToRemoveList();
        _spawnBoss.clear();
    }

    void DoSpawn(uint32 spawnId)
    {
        Position spawnPos;
        uint32 spawnScriptTextId = 0;
        uint32 formationCreatureEntry = 0;
        uint32 despawnTimer = 0;
        TempSpawnType spawnType = TEMPSPAWN_DEAD_DESPAWN;

        switch (spawnId)
        {
            case NPC_DREADKNIGHT:
            case NPC_FELGUARD_LIEUTENANT:
                spawnPos = spawnBossPortalPos;
                break;
            case NPC_LORD_MARSHAL_RAYNOR:
                spawnPos = spawnReinforcementPos;
                despawnTimer = 3 * MINUTE * IN_MILLISECONDS;
                spawnType = TEMPSPAWN_TIMED_OR_DEAD_DESPAWN;
                formationCreatureEntry = NPC_STORMWIND_MARSHAL;
                spawnScriptTextId = LMR_SPAWN;
                break;
            case NPC_JUSTINIUS_THE_HARBINGER:
                spawnPos = spawnReinforcementPos;
                despawnTimer = 3 * MINUTE * IN_MILLISECONDS;
                spawnType = TEMPSPAWN_TIMED_OR_DEAD_DESPAWN;
                formationCreatureEntry = NPC_AZUREMYST_VINDICATOR;
                spawnScriptTextId = JTH_SPAWN;
                break;
            case NPC_MELGROMM_HIGHMOUNTAIN:
                spawnPos = spawnReinforcementPos;
                despawnTimer = 3 * MINUTE * IN_MILLISECONDS;
                spawnType = TEMPSPAWN_TIMED_OR_DEAD_DESPAWN;
                formationCreatureEntry = NPC_THUNDER_BLUFF_HUNTSMAN;
                spawnScriptTextId = MH_SPAWN;
                break;
            default:
                spawnPos = urand(0, 1) ? spawnPortalPos : spawnPortalPos1;
                break;
        }

        if (Creature* pProudwell = GetSingleCreatureFromStorage(NPC_AGENT_PROUDWELL, true))
        {
            if (Creature* summon = pProudwell->SummonCreature(spawnId, spawnPos.x, spawnPos.y, spawnPos.z, spawnPos.o, spawnType, despawnTimer))
            {
                if (spawnScriptTextId)
                    DoScriptText(spawnScriptTextId, summon);

                if (formationCreatureEntry)
                {
                    if (Creature* add = summon->SummonCreature(formationCreatureEntry, summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), 0.0f, spawnType, despawnTimer))
                        add->GetMotionMaster()->MoveFollow(summon, 5.f, 220.f * float(M_PI) / 180.0f, true);

                    if (Creature* add = summon->SummonCreature(formationCreatureEntry, summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), 0.0f, spawnType, despawnTimer))
                        add->GetMotionMaster()->MoveFollow(summon, 5.f, 140.f * float(M_PI) / 180.0f, true);
                }
            }
        }
    }

    void OnEventHappened(uint16 event_id, bool activate, bool resume) override
    {
        if (event_id == GAME_EVENT_BEFORE_THE_STORM)
        {
            DespawnAdds();

            if (activate && !resume)
            {
                ResetTimer(EVENT_SPAWN, 1000);
                ResetTimer(EVENT_SPAWN_BOSS, 30000);
                ResetTimer(EVENT_REINFORCEMENTS_NEEDED, 30000);
            }
            else
            {
                ResetAllTimers();
            }
        }
        else if (event_id == GAME_EVENT_GURUBASHI_ARENA && activate)
        {
            if (Creature* creature = GetSingleCreatureFromStorage(NPC_SHORT_JOHN_MITHRIL))
                creature->GetMotionMaster()->MoveWaypoint();
        }
        else if (event_id == GAME_EVENT_BREWFEST_DARK_IRON_ATTACK && activate)
            m_brewfestEvent.StartDarkIronAttackEvent();
        else if (event_id == GAME_EVENT_BREWFEST_KEG_TAPPING && activate)
            m_brewfestEvent.StartKegTappingEvent();
    }

    void Update(uint32 diff) override
    {
        UpdateTimers(diff);

        if (sGameEventMgr.IsActiveHoliday(HOLIDAY_BREWFEST))
            m_brewfestEvent.Update(diff);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type >= TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE && type <= TYPE_SHADE_OF_THE_HORSEMAN_MAX)
            return m_shadeData.HandleGetData(type);
        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type >= TYPE_SHADE_OF_THE_HORSEMAN_ATTACK_PHASE && type <= TYPE_SHADE_OF_THE_HORSEMAN_MAX)
            return m_shadeData.HandleSetData(type, data);
    }
};

struct go_infernaling_summoner_portal_hound : public GameObjectAI
{
    go_infernaling_summoner_portal_hound(GameObject* go) : GameObjectAI(go)
    {
        m_uiSummonTimer = 15000;
    }

    uint32 m_uiSummonTimer;

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_go->IsSpawned())
            return;

        if (m_uiSummonTimer <= uiDiff)
        {
            if (Creature* pProudwell = ((ScriptedInstance*)m_go->GetMap()->GetInstanceData())->GetSingleCreatureFromStorage(NPC_AGENT_PROUDWELL))
            {
                pProudwell->CastSpell(pProudwell, SPELL_SUMMON_INFERNALING_PORTAL_HOUND, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_go->GetObjectGuid());
            }
            m_uiSummonTimer = urand(60000, 120000);
        }
        else
            m_uiSummonTimer -= uiDiff;
    }
};

GameObjectAI* GetAI_go_infernaling_summoner_portal_hound(GameObject* go)
{
    return new go_infernaling_summoner_portal_hound(go);
}

// does not work currently - need to handle spell 33710 somehow when it hits/activates either object 183358 or 183357?
bool GOUse_go_infernaling_summoner_portal_hound(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    if (Creature* pProudwell = pInstance->GetSingleCreatureFromStorage(NPC_AGENT_PROUDWELL))
    {
        pProudwell->CastSpell(pProudwell, SPELL_SUMMON_INFERNALING_PORTAL_HOUND, TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, pGo->GetObjectGuid());
    }

    return false;
}

struct npc_agent_proudwell : public ScriptedAI
{
    npc_agent_proudwell(Creature* pCreature) : ScriptedAI(pCreature) { }

    void Reset() override {}

    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        uint32 despawnScriptTextId = 0;
        switch (pSummoned->GetEntry())
        {
            case NPC_LORD_MARSHAL_RAYNOR: despawnScriptTextId = LMR_DESPAWN; break;
            case NPC_JUSTINIUS_THE_HARBINGER: despawnScriptTextId = JTH_DESPAWN; break;
            case NPC_MELGROMM_HIGHMOUNTAIN: despawnScriptTextId = MH_DESPAWN; break;
        }

        world_map_eastern_kingdoms* pInstance = (world_map_eastern_kingdoms*)pSummoned->GetInstanceData();

        if (pInstance)
            pInstance->_spawnSupport.erase(pSummoned->GetObjectGuid());

        if (despawnScriptTextId && pSummoned->IsAlive())
            DoScriptText(despawnScriptTextId, pSummoned);
    }
};

UnitAI* GetAI_npc_agent_proudwell(Creature* pCreature)
{
    return new npc_agent_proudwell(pCreature);
}

InstanceData* GetInstanceData_world_map_eastern_kingdoms(Map* pMap)
{
    return new world_map_eastern_kingdoms(pMap);
}

void AddSC_world_eastern_kingdoms()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "world_map_eastern_kingdoms";
    pNewScript->GetInstanceData = &GetInstanceData_world_map_eastern_kingdoms;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_infernaling_summoner_portal_hound";
    pNewScript->GetGameObjectAI = &GetAI_go_infernaling_summoner_portal_hound;
    pNewScript->pGOUse = &GOUse_go_infernaling_summoner_portal_hound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_agent_proudwell";
    pNewScript->GetAI = &GetAI_npc_agent_proudwell;
    pNewScript->RegisterSelf();
}
