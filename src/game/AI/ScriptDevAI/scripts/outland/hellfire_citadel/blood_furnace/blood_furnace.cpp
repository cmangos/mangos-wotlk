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
SDName: Instance_Blood_Furnace
SD%Complete: 75
SDComment:
SDCategory: Blood Furnace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "blood_furnace.h"

instance_blood_furnace::instance_blood_furnace(Map* map) : ScriptedInstance(map),
    m_uiBroggokEventTimer(90 * IN_MILLISECONDS),
    m_uiBroggokEventPhase(0),
    m_uiRandYellTimer(90000),
    m_crackTimer(30000),
    m_uiBroggokEventDelay(0),
    m_uiBroggokEventDelaySpawn(false)
{
    Initialize();
}

void instance_blood_furnace::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_blood_furnace::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_BROGGOK:
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        case NPC_KELIDAN_THE_BREAKER:
        case NPC_MAGTHERIDON:
        case NPC_IN_COMBAT_TRIGGER:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_SHADOWMOON_CHANNELER:
            m_lChannelersGuids.push_back(creature->GetObjectGuid());
            break;
    }
}

void instance_blood_furnace::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_NASCENT_FEL_ORC:
        case NPC_FEL_ORC_NEOPHYTE:
            if (creature->HasStringId(FIRST_BROGGOK_CELL_STRING) || creature->HasStringId(SECOND_BROGGOK_CELL_STRING) || creature->HasStringId(THIRD_BROGGOK_CELL_STRING) || creature->HasStringId(FOURTH_BROGGOK_CELL_STRING))
            {
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE); // ones in cages
                creature->setFaction(14); // sniffed value
                m_broggokEventGuids.push_back(creature->GetDbGuid());
            }
            else if (creature->HasStringId(FIFTH_BROGGOK_CELL_STRING))
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_NPC);
            break;
    }
}
           
void instance_blood_furnace::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_DOOR_MAKER_FRONT:                           // the maker front door
            break;
        case GO_DOOR_MAKER_REAR:                            // the maker rear door
            if (m_auiEncounter[TYPE_THE_MAKER_EVENT] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_BROGGOK_FRONT:                         // broggok front door
            break;
        case GO_DOOR_BROGGOK_REAR:                          // broggok rear door
            if (m_auiEncounter[TYPE_BROGGOK_EVENT] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_KELIDAN_EXIT:                          // kelidan exit door
            if (m_auiEncounter[TYPE_KELIDAN_EVENT] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_FINAL_EXIT:                            // final exit door
            if (m_auiEncounter[TYPE_KELIDAN_EVENT] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;

        case GO_PRISON_CELL_BROGGOK_1: m_aBroggokEvent[0].m_cellGuid = go->GetObjectGuid(); return;
        case GO_PRISON_CELL_BROGGOK_2: m_aBroggokEvent[1].m_cellGuid = go->GetObjectGuid(); return;
        case GO_PRISON_CELL_BROGGOK_3: m_aBroggokEvent[2].m_cellGuid = go->GetObjectGuid(); return;
        case GO_PRISON_CELL_BROGGOK_4: m_aBroggokEvent[3].m_cellGuid = go->GetObjectGuid(); return;

        case GO_PRISON_CELL_DOOR_LEVER:
            m_lLeverGO = go;
          
            if (m_auiEncounter[TYPE_BROGGOK_EVENT] != DONE && m_auiEncounter[TYPE_BROGGOK_EVENT] != IN_PROGRESS)
            {
                if (m_lLeverGO)
                {
                    m_lLeverGO->SetRespawnTime(time(nullptr));
                    m_lLeverGO->Respawn();
                }
            }
            break;
        case GO_CRACK_1:
        case GO_CRACK_2:
        case GO_CRACK_3:
        case GO_CRACK_4:
        case GO_CRACK_5:
        case GO_CRACK_6:
        case GO_CRACK_7:
            m_cracks.push_back(go->GetObjectGuid());
            return;
        default:
            return;
    }
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_blood_furnace::OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* /*pCreature*/)
{
    switch (pGroup->GetGroupId())
    {
        // Only release next group if not already released from timer 
        case SPAWN_GROUP_PRISON_CELL_1:
            if (m_uiBroggokEventPhase == SECOND_ORC_WAVE)
                DoNextBroggokEventPhase();
            break;
        case SPAWN_GROUP_PRISON_CELL_2:
            if (m_uiBroggokEventPhase == THIRD_ORC_WAVE)
                DoNextBroggokEventPhase();
            break;
        case SPAWN_GROUP_PRISON_CELL_3:
            if (m_uiBroggokEventPhase == FOURTH_ORC_WAVE)
                DoNextBroggokEventPhase();
            break;
        case SPAWN_GROUP_PRISON_CELL_4:
            if (m_uiBroggokEventPhase == MAX_ORC_WAVES)
                DoNextBroggokEventPhase();
            break;
    }
}

void instance_blood_furnace::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_THE_MAKER_EVENT:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_DOOR_MAKER_FRONT);
            if (data == FAIL)
                DoUseDoorOrButton(GO_DOOR_MAKER_FRONT);
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_DOOR_MAKER_FRONT);
                DoUseDoorOrButton(GO_DOOR_MAKER_REAR);
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_BROGGOK_EVENT:
            if (m_auiEncounter[type] == data)
                return;

            // Combat door; the exit door is opened in event
            DoUseDoorOrButton(GO_DOOR_BROGGOK_FRONT);
            if (data == IN_PROGRESS)
            {
                if (Creature* broggok = GetSingleCreatureFromStorage(NPC_BROGGOK))
                {
                    broggok->CastSpell(nullptr, SPELL_COMBAT_TRIGGER, TRIGGERED_OLD_TRIGGERED); // cast doesnt show in sniff
                    DoBroadcastText(SAY_BROGGOK_INTRO, broggok);
                }

                if (m_uiBroggokEventPhase <= MAX_ORC_WAVES)
                {
                    m_uiBroggokEventPhase = 0;
    
                    // open first cage
                    DoNextBroggokEventPhase();

                    // Confirmed on Sniffs that lever gets uninteractable while even is in progress
                    m_lLeverGO->ApplyModFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT, true);
                }
            }
            else if (data == FAIL)
            {
                // On wipe we reset only the orcs; if the party wipes at the boss itself then the orcs don't reset
                if (m_uiBroggokEventPhase <= MAX_ORC_WAVES)
                {          
                    // Respawn all Groups
                    RespawnDbGuids(m_broggokEventGuids, 0);
                    m_broggokEventGuids.clear();

                    // Close all Cells
                    for (auto& i : m_aBroggokEvent)
                    {
                        if (!i.m_bIsCellOpened)
                            continue;

                        // Close Door
                        DoUseDoorOrButton(i.m_cellGuid);
                        i.m_bIsCellOpened = false;
                    }

                    if (m_lLeverGO)
                    {
                        m_lLeverGO->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                        m_lLeverGO->SetRespawnTime(time(nullptr));
                        m_lLeverGO->Respawn();
                    }
                }
            }
            if (data == FAIL || data == DONE)
                if (Creature* trigger = GetSingleCreatureFromStorage(NPC_IN_COMBAT_TRIGGER))
                    trigger->ForcedDespawn();
            m_auiEncounter[type] = data;
            break;
        case TYPE_KELIDAN_EVENT:
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_DOOR_KELIDAN_EXIT);
                DoUseDoorOrButton(GO_DOOR_FINAL_EXIT);
            }
            m_auiEncounter[type] = data;
            break;
        default:
            script_error_log("Instance Blood Furnace SetData with Type %u Data %u, but this is not implemented.", type, data);
            return;
    }

    if (data == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_blood_furnace::DoNextBroggokEventPhase()
{
    // Open door to the final boss now and move boss to the center of the room
    if (m_uiBroggokEventPhase >= MAX_ORC_WAVES)
    {
        DoUseDoorOrButton(GO_DOOR_BROGGOK_REAR);

        if (Creature* pBroggok = GetSingleCreatureFromStorage(NPC_BROGGOK))
        {
            pBroggok->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            pBroggok->SetWalk(false);
            pBroggok->GetMotionMaster()->Clear(false, true);
            pBroggok->GetMotionMaster()->MoveWaypoint();
        }
    }
    else
    {
        // Open cage door
        if (!m_aBroggokEvent[m_uiBroggokEventPhase].m_bIsCellOpened)
            DoUseDoorOrButton(m_aBroggokEvent[m_uiBroggokEventPhase].m_cellGuid);

        m_aBroggokEvent[m_uiBroggokEventPhase].m_bIsCellOpened = true;

        // First Wave spawns instantly
        if (m_uiBroggokEventPhase == FIRST_ORC_WAVE)
        {
            auto pOrc = instance->GetCreatures(FIRST_BROGGOK_CELL_STRING);
            for (Creature* creature : *pOrc)
            {
                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                creature->SetInCombatWithZone();
            }
            ++m_uiBroggokEventPhase;
        }
        else
        {
            // All other waves have a 2 second delay after cell openes
            m_uiBroggokEventDelay = 2000;
            m_uiBroggokEventDelaySpawn = true;
        }

        // Always reset Timer if a cell already got opened because the previous group died
        m_uiBroggokEventTimer = 90000;
    }
}

void instance_blood_furnace::Update(uint32 uiDiff)
{
    if (m_auiEncounter[TYPE_BROGGOK_EVENT] == IN_PROGRESS)
    {
        // 2nd, 3rd and 4th Orc wave will attack 2 seconds after cell opened
        if (m_uiBroggokEventDelaySpawn)
        {
            if (m_uiBroggokEventDelay < uiDiff)
            {
                if (m_uiBroggokEventPhase == SECOND_ORC_WAVE)
                {
                    auto pOrc = instance->GetCreatures(SECOND_BROGGOK_CELL_STRING);
                    for (Creature* creature : *pOrc)
                    {
                        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        creature->SetInCombatWithZone();
                    }
                    ++m_uiBroggokEventPhase;
                    m_uiBroggokEventDelaySpawn = false;
                    m_uiBroggokEventDelay = 2000;
                }
                else if (m_uiBroggokEventPhase == THIRD_ORC_WAVE)
                {
                    auto pOrc = instance->GetCreatures(THIRD_BROGGOK_CELL_STRING);
                    for (Creature* creature : *pOrc)
                    {
                        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        creature->SetInCombatWithZone();
                    }
                    ++m_uiBroggokEventPhase;
                    m_uiBroggokEventDelaySpawn = false;
                    m_uiBroggokEventDelay = 2000;
                }
                else if (m_uiBroggokEventPhase == FOURTH_ORC_WAVE)
                {
                    auto pOrc = instance->GetCreatures(FOURTH_BROGGOK_CELL_STRING);
                    for (Creature* creature : *pOrc)
                    {
                        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                        creature->SetInCombatWithZone();
                    }
                    ++m_uiBroggokEventPhase;
                    m_uiBroggokEventDelaySpawn = false;
                    m_uiBroggokEventDelay = 2000;
                }
            }
            else
                m_uiBroggokEventDelay -= uiDiff;
        }

        // Broggok Event: For the last wave we don't check the timer; the boss is released only when all mobs die
        if (m_uiBroggokEventPhase <= MAX_ORC_WAVES)
        {
            // on wotlk ptr normal and heroic both used 90 seconds timer 
            if (m_uiBroggokEventTimer < uiDiff)            
                DoNextBroggokEventPhase();
            else
                m_uiBroggokEventTimer -= uiDiff;
            
        }
    }


    if (m_uiRandYellTimer < uiDiff)
    {
        if (Creature* pMagtheridon = GetSingleCreatureFromStorage(NPC_MAGTHERIDON))
        {
            DoBroadcastText(aRandomTaunt[urand(0, 5)], pMagtheridon);
            m_uiRandYellTimer = 90000;
        }
    }
    else
        m_uiRandYellTimer -= uiDiff;

    if (m_crackTimer <= uiDiff)
    {
        m_crackTimer = 30000;
        for (ObjectGuid guid : m_cracks)
        {
            if (GameObject* crack = instance->GetGameObject(guid))
            {
                if (crack->GetGoState() != GO_STATE_READY)
                    if (urand(0, 5))
                        crack->SetGoState(GO_STATE_READY);

                if (crack->GetGoState() == GO_STATE_READY)
                    if (urand(0, 2))
                        crack->SendGameObjectCustomAnim(crack->GetObjectGuid(), 2);
            }
        }
    }
    else
        m_crackTimer -= uiDiff;
}

uint32 instance_blood_furnace::GetData(uint32 type) const
{
    if (type < MAX_ENCOUNTER)
        return m_auiEncounter[type];

    return 0;
}

void instance_blood_furnace::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

    for (uint32& i : m_auiEncounter)
        if (i == IN_PROGRESS || i == FAIL)
            i = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_blood_furnace::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands: diagnostics, resetgauntlet");
}

void instance_blood_furnace::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "diagnostics")
    {
        handler->PSendSysMessage("Broggok event stage: %u Broggok instance data: %u", m_uiBroggokEventPhase, GetData(TYPE_BROGGOK_EVENT));
    }
    else if (val == "resetgauntlet")
    {
        if (GetData(TYPE_BROGGOK_EVENT) == IN_PROGRESS)
            SetData(TYPE_BROGGOK_EVENT, FAIL);
    }
}

bool GOUse_go_prison_cell_lever(Player* /*player*/, GameObject* go)
{
    ScriptedInstance* pInstance = static_cast<ScriptedInstance*>(go->GetInstanceData());

    if (!pInstance)
        return false;

    // Set broggok event in progress
    if (pInstance->GetData(TYPE_BROGGOK_EVENT) != DONE && pInstance->GetData(TYPE_BROGGOK_EVENT) != IN_PROGRESS)
        pInstance->SetData(TYPE_BROGGOK_EVENT, IN_PROGRESS);

    return false;
}

void AddSC_instance_blood_furnace()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_blood_furnace";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_blood_furnace>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_prison_cell_lever";
    pNewScript->pGOUse = &GOUse_go_prison_cell_lever;
    pNewScript->RegisterSelf();
}
