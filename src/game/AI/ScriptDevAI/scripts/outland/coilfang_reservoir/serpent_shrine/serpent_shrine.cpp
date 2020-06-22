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
SDName: Instance_Serpent_Shrine
SD%Complete: 90
SDComment:
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"

/* Serpentshrine cavern encounters:
0 - Hydross The Unstable event
1 - Leotheras The Blind Event
2 - The Lurker Below Event
3 - Fathom-Lord Karathress Event
4 - Morogrim Tidewalker Event
5 - Lady Vashj Event
*/

instance_serpentshrine_cavern::instance_serpentshrine_cavern(Map* pMap) : ScriptedInstance(pMap),
    m_uiSpellBinderCount(0)
{
    Initialize();
}

void instance_serpentshrine_cavern::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_serpentshrine_cavern::IsEncounterInProgress() const
{
    for (uint32 i = 0; i < TYPE_COMBAT_MAX; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_serpentshrine_cavern::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KARATHRESS:
        case NPC_LADYVASHJ:
        case NPC_SHARKKIS:
        case NPC_TIDALVESS:
        case NPC_CARIBDIS:
        case NPC_LEOTHERAS:
        case NPC_WORLD_TRIGGER:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_SEER_OLUM:
            pCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            break;
        case NPC_GREYHEART_SPELLBINDER:
            m_lSpellBindersGUIDList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_HYDROSS_BEAM_HELPER:
            m_lBeamHelpersGUIDList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_TINY_TRIGGER:
            m_tinyTriggerGUIDList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_COILFANG_PRIESTESS:
        case NPC_COILFANG_SHATTERER:
        case NPC_VASHJIR_HONOR_GUARD:
        case NPC_GREYHEART_TECHNICIAN:
            // Filter only the mobs spawned on the platforms
            if (pCreature->GetPositionZ() > 0)
                m_sPlatformMobsGUIDSet.insert(pCreature->GetObjectGuid());
            break;
        case NPC_COILFANG_FRENZY_CORPSE:
            pCreature->ForcedDespawn();
            m_lFishCorpsesGUIDList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_serpentshrine_cavern::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CONSOLE_HYDROSS:
        case GO_CONSOLE_LURKER:
        case GO_CONSOLE_LEOTHERAS:
        case GO_CONSOLE_KARATHRESS:
        case GO_CONSOLE_MOROGRIM:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            EngageBossConsole(100, pGo);
            break;
        case GO_SHIELD_GENERATOR_1:
        case GO_SHIELD_GENERATOR_2:
        case GO_SHIELD_GENERATOR_3:
        case GO_SHIELD_GENERATOR_4:
        case GO_STRANGE_POOL:
        case GO_BRIDGE_PART_1:
        case GO_BRIDGE_PART_2:
        case GO_BRIDGE_PART_3:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_CONSOLE_VASHJ:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            EngageBridgeConsole(pGo);
            break;
    }
}

void instance_serpentshrine_cavern::OnPlayerResurrect(Player * player)
{
    if (m_auiEncounter[TYPE_LEOTHERAS_EVENT] == IN_PROGRESS)
    {
        if (Creature* leotheras = GetSingleCreatureFromStorage(NPC_LEOTHERAS))
            leotheras->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, player, leotheras);
    }
}

void instance_serpentshrine_cavern::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_THELURKER_EVENT:
            switch (uiData)
            {
                case IN_PROGRESS:
                    if (Player* player = GetPlayerInMap())
                        player->CastSpell(player, SPELL_LURKER_SPAWN_TRIGGER, TRIGGERED_OLD_TRIGGERED); // TODO: wotlk spell should be casted by special someone
                    break;
                case DONE:
                    if (Creature* pWorldTrigger = GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER))
                        pWorldTrigger->ForcedDespawn();
                    break;
                case FAIL:
                    if (GameObject* go = GetSingleGameObjectFromStorage(GO_STRANGE_POOL))
                    {
                        go->SetRespawnTime(2);
                        go->Refresh();
                    }
                    break;
            }
        case TYPE_LEOTHERAS_EVENT:
            if (uiData == FAIL)
            {
                for (GuidList::const_iterator itr = m_lSpellBindersGUIDList.begin(); itr != m_lSpellBindersGUIDList.end(); ++itr)
                {
                    if (Creature* pSpellBinder = instance->GetCreature(*itr))
                        pSpellBinder->Respawn();
                }

                m_uiSpellBinderCount = 0;
            }
            // no break;
        case TYPE_HYDROSS_EVENT:
        case TYPE_KARATHRESS_EVENT:
        case TYPE_MOROGRIM_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                EngageBossConsole(uiType, 0);
            if (uiData == SPECIAL)
                EngageBridgeConsole();
            break;
        case TYPE_LADYVASHJ_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == FAIL)
            {
                // interrupt the shield
                for (GuidList::const_iterator itr = m_tinyTriggerGUIDList.begin(); itr != m_tinyTriggerGUIDList.end(); ++itr)
                {
                    if (Creature* pGenerator = instance->GetCreature(*itr))
                        pGenerator->InterruptNonMeleeSpells(false);
                }

                // reset generators
                DoToggleGameObjectFlags(GO_SHIELD_GENERATOR_1, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_SHIELD_GENERATOR_2, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_SHIELD_GENERATOR_3, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_SHIELD_GENERATOR_4, GO_FLAG_NO_INTERACT, false);

                if (Creature* vashj = GetSingleCreatureFromStorage(NPC_LADYVASHJ))
                {
                    vashj->SetRespawnDelay(30, true);
                    vashj->ForcedDespawn();
                }
            }
            break;
        case TYPE_LEOTHERAS_EVENT_DEMONS:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_serpentshrine_cavern::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[TYPE_HYDROSS_EVENT] >> m_auiEncounter[TYPE_KARATHRESS_EVENT] >> m_auiEncounter[TYPE_LADYVASHJ_EVENT] >> m_auiEncounter[TYPE_LEOTHERAS_EVENT]
               >> m_auiEncounter[TYPE_MOROGRIM_EVENT] >> m_auiEncounter[TYPE_THELURKER_EVENT];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    if (m_auiEncounter[TYPE_THELURKER_EVENT] >= DONE)
        SpawnFishCorpses();

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_serpentshrine_cavern::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_serpentshrine_cavern::SetData64(uint32 uiData, uint64 uiGuid)
{
    // Note: this is handled in Acid. The purpose is check which npc from the platform set is alive
    // The function is triggered by eventAI on generic timer
    if (uiData == DATA_WATERSTATE_EVENT)
    {
        if (m_sPlatformMobsGUIDSet.find(ObjectGuid(uiGuid)) != m_sPlatformMobsGUIDSet.end())
            m_sPlatformMobsAliveGUIDSet.insert(ObjectGuid(uiGuid));
    }
}

bool instance_serpentshrine_cavern::CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const
{
    switch (uiInstanceConditionId)
    {
        case INSTANCE_CONDITION_ID_LURKER:
            return GetData(TYPE_THELURKER_EVENT) < DONE;
        case INSTANCE_CONDITION_ID_SCALDING_WATER:
            return m_sPlatformMobsAliveGUIDSet.empty();
    }

    script_error_log("instance_serpentshrine_cavern::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
                     uiInstanceConditionId, pPlayer ? pPlayer->GetGuidStr().c_str() : "NULL", pConditionSource ? pConditionSource->GetGuidStr().c_str() : "NULL", conditionSourceType);
    return false;
}

void instance_serpentshrine_cavern::OnCreatureEnterCombat(Creature* pCreature)
{
    // Interrupt spell casting on aggro
    if (pCreature->GetEntry() == NPC_GREYHEART_SPELLBINDER)
        pCreature->InterruptNonMeleeSpells(false);
}

void instance_serpentshrine_cavern::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_GREYHEART_SPELLBINDER:
            ++m_uiSpellBinderCount;

            if (m_uiSpellBinderCount == MAX_SPELLBINDERS)
                if (Creature* pLeotheras = GetSingleCreatureFromStorage(NPC_LEOTHERAS))
                    pLeotheras->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pLeotheras, pLeotheras);
            break;
        case NPC_COILFANG_PRIESTESS:
        case NPC_COILFANG_SHATTERER:
        case NPC_VASHJIR_HONOR_GUARD:
        case NPC_GREYHEART_TECHNICIAN:
            if (m_sPlatformMobsGUIDSet.find(pCreature->GetObjectGuid()) != m_sPlatformMobsGUIDSet.end())
                m_sPlatformMobsAliveGUIDSet.erase(pCreature->GetObjectGuid());
            if (m_sPlatformMobsAliveGUIDSet.empty())
                SpawnFishCorpses();
            break;
    }
}

void instance_serpentshrine_cavern::SpawnFishCorpses()
{
    for (GuidList::const_iterator itr = m_lFishCorpsesGUIDList.begin(); itr != m_lFishCorpsesGUIDList.end(); ++itr)
    {
        if (Creature* pFish = instance->GetCreature(*itr))
            pFish->Respawn();
    }
}

void instance_serpentshrine_cavern::EngageBridgeConsole(GameObject* _console)
{
    if(m_auiEncounter[TYPE_HYDROSS_EVENT] == SPECIAL && m_auiEncounter[TYPE_THELURKER_EVENT] == SPECIAL && m_auiEncounter[TYPE_LEOTHERAS_EVENT] == SPECIAL
        && m_auiEncounter[TYPE_KARATHRESS_EVENT] == SPECIAL && m_auiEncounter[TYPE_MOROGRIM_EVENT] == SPECIAL)
        if(GameObject* console = _console ? _console : instance->GetGameObject(m_goEntryGuidStore[GO_CONSOLE_VASHJ]))
            console->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
}

uint32 EncounterTypeFromGOEntry(uint32 goEntry)
{
    switch (goEntry)
    {
        case GO_CONSOLE_HYDROSS:
            return TYPE_HYDROSS_EVENT;
        case GO_CONSOLE_LURKER:
            return TYPE_THELURKER_EVENT;
        case GO_CONSOLE_LEOTHERAS:
            return TYPE_LEOTHERAS_EVENT;
        case GO_CONSOLE_KARATHRESS:
            return TYPE_KARATHRESS_EVENT;
        case GO_CONSOLE_MOROGRIM:
            return TYPE_MOROGRIM_EVENT;
        default:
            return 0;
    }
}

void instance_serpentshrine_cavern::EngageBossConsole(uint32 uiType, GameObject* console)
{
    if (uiType == 100)
        uiType = EncounterTypeFromGOEntry(console->GetEntry());
    else if (console == nullptr) // 0 is taken
    {
        uint32 goEntry;
        switch (uiType)
        {
            case TYPE_HYDROSS_EVENT:
                goEntry = GO_CONSOLE_HYDROSS;
                break;
            case TYPE_THELURKER_EVENT:
                goEntry = GO_CONSOLE_LURKER;
                break;
            case TYPE_LEOTHERAS_EVENT:
                goEntry = GO_CONSOLE_LEOTHERAS;
                break;
            case TYPE_KARATHRESS_EVENT:
                goEntry = GO_CONSOLE_KARATHRESS;
                break;
            case TYPE_MOROGRIM_EVENT:
                goEntry = GO_CONSOLE_MOROGRIM;
                break;
        }
        console = instance->GetGameObject(m_goEntryGuidStore[goEntry]);
    }
    if (m_auiEncounter[uiType] == DONE)
        console->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    else if(m_auiEncounter[uiType] == SPECIAL)
        console->SetGoState(GO_STATE_ACTIVE);
}

bool GOUse_go_ssc_boss_consoles(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return true;

    if (pGo->GetEntry() == GO_CONSOLE_HYDROSS)
        pInstance->SetData(TYPE_HYDROSS_EVENT, SPECIAL);
    else if (pGo->GetEntry() == GO_CONSOLE_LURKER)
        pInstance->SetData(TYPE_THELURKER_EVENT, SPECIAL);
    else if (pGo->GetEntry() == GO_CONSOLE_LEOTHERAS)
        pInstance->SetData(TYPE_LEOTHERAS_EVENT, SPECIAL);
    else if (pGo->GetEntry() == GO_CONSOLE_KARATHRESS)
        pInstance->SetData(TYPE_KARATHRESS_EVENT, SPECIAL);
    else if (pGo->GetEntry() == GO_CONSOLE_MOROGRIM)
        pInstance->SetData(TYPE_MOROGRIM_EVENT, SPECIAL);

    return false;
}

struct npc_serpentshrine_parasiteAI : public ScriptedAI
{
    npc_serpentshrine_parasiteAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void DamageDeal(Unit* /*pDoneTo*/, uint32& /*damage*/, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        m_creature->ForcedDespawn(1000);
    }

    void JustRespawned() override
    {
        m_creature->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
            m_creature->FixateTarget(pTarget);
    }
};

void instance_serpentshrine_cavern::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands: spawnlurker");
}

void instance_serpentshrine_cavern::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "spawnlurker")
        SetData(TYPE_THELURKER_EVENT, IN_PROGRESS);
}

void AddSC_instance_serpentshrine_cavern()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_serpentshrine_parasite";
    pNewScript->GetAI = &GetNewAIInstance<npc_serpentshrine_parasiteAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_ssc_boss_consoles";
    pNewScript->pGOUse = &GOUse_go_ssc_boss_consoles;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "instance_serpent_shrine";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_serpentshrine_cavern>;
    pNewScript->RegisterSelf();
}
