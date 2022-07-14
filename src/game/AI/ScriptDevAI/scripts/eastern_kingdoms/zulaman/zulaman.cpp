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
SDName: Instance_Zulaman
SD%Complete: 50
SDComment: Support for Quests and Mini-Events still TODO
SDCategory: Zul'Aman
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "zulaman.h"
#include "MotionGenerators/WaypointManager.h"

instance_zulaman::instance_zulaman(Map* map) : ScriptedInstance(map),
    m_startCheck(false),
    m_uiEventTimer(MINUTE * IN_MILLISECONDS),
    m_uiBearEventPhase(0),
    m_isBearPhaseInProgress(false),
    m_spiritFadeTimer(0)
{
    Initialize();
}

void instance_zulaman::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    memset(&m_auiRandVendor, 0, sizeof(m_auiRandVendor));
    instance->GetVariableManager().SetVariableData(WORLD_STATE_ZUL_AMAN_EVENT_RUN_IS_ACTIVE, true, 0, 0);
    instance->GetVariableManager().SetVariableData(WORLD_STATE_ZUL_AMAN_TIME_COUNTER, true, 0, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_MALACRASS, 0);
}

bool instance_zulaman::IsEncounterInProgress() const
{
    // Skip Time-Event and Time-Event timer
    for (uint8 i = 1; i < TYPE_RUN_EVENT_TIME - 1; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_zulaman::OnPlayerEnter(Player* /*player*/)
{
    if (m_startCheck)
        return;

    m_startCheck = true;

    if (GetKilledPreBosses() == 4 && GetData(TYPE_MALACRASS) != DONE)
        SpawnMalacrass();
}

void instance_zulaman::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_AKILZON:
        case NPC_HALAZZI:
        case NPC_NALORAKK:
        case NPC_JANALAI:
        case NPC_MALACRASS:
        case NPC_ZULJIN:
        case NPC_HARRISON:
        case NPC_BEAR_SPIRIT:
        case NPC_EAGLE_SPIRIT:
        case NPC_LYNX_SPIRIT:
        case NPC_DRAGONHAWK_SPIRIT:
        // Insert Malacrass companions here for better handling
        case NPC_ALYSON:
        case NPC_THURG:
        case NPC_SLITHER:
        case NPC_RADAAN:
        case NPC_GAZAKROTH:
        case NPC_FENSTALKER:
        case NPC_DARKHEART:
        case NPC_KORAGG:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;        
        // Akil'zon gauntlet 
        case NPC_TEMPEST:
            if (creature->GetPositionZ() > 50.0f) // excludes Tempest in Malacrass trash
                sAkilzonTrashGuidSet.insert(creature->GetObjectGuid());
            break;
        case NPC_LOOKOUT:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
        case NPC_PROTECTOR:
        case NPC_WIND_WALKER:
            if (creature->GetPositionZ() > 26.0f) // excludes Wind Walker in first patrol
                sAkilzonTrashGuidSet.insert(creature->GetObjectGuid());
            break;

        case NPC_TANZAR:      m_aEventNpcInfo[INDEX_NALORAKK].npGuid = creature->GetObjectGuid(); break;
        case NPC_KRAZ:        m_aEventNpcInfo[INDEX_JANALAI].npGuid =  creature->GetObjectGuid(); break;
        case NPC_ASHLI:       m_aEventNpcInfo[INDEX_HALAZZI].npGuid =  creature->GetObjectGuid(); break;
        case NPC_HARKOR:      m_aEventNpcInfo[INDEX_AKILZON].npGuid =  creature->GetObjectGuid(); break;

        case NPC_MEDICINE_MAN:
        case NPC_TRIBES_MAN:
        case NPC_WARBRINGER:
        case NPC_AXETHROWER:
            if (creature->GetPositionZ() > 10.0f && creature->GetPositionZ() < 15.0f)
            {
                m_nalorakkEvent[0].nalorakkTrashSet.insert(creature->GetObjectGuid());
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            else if (creature->GetPositionZ() > 25.0f && creature->GetPositionZ() < 30.0f)
            {
                m_nalorakkEvent[1].nalorakkTrashSet.insert(creature->GetObjectGuid());
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            else if (creature->GetPositionZ() > 40.0f && creature->GetPositionZ() < 41.0f)
            {
                m_nalorakkEvent[2].nalorakkTrashSet.insert(creature->GetObjectGuid());
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            else if (creature->GetPositionZ() > 41.0f)
            {
                m_nalorakkEvent[3].nalorakkTrashSet.insert(creature->GetObjectGuid());
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            break;
        case NPC_WORLD_TRIGGER:
            if (creature->GetOrientation() > 2.7f)
                sHutTriggerGuidSet.insert(creature->GetObjectGuid());
            else
                sDrumTriggerGuidSet.insert(creature->GetObjectGuid());
            break;
        case NPC_DRAGONHAWK_EGG:
            creature->SetCorpseDelay(5);
            break;
        case NPC_AMANISHI_GUARDIAN:
            if (creature->GetPositionZ() > 43.f) // only 2 involved with opening RP event in total
            {
                // choose which one will speak and attack Harrison
                if (creature->GetPositionX() > 130.0f)
                    m_GongGuardianAttackerGuid = creature->GetObjectGuid();
                sGongGuardianGuidSet.insert(creature->GetObjectGuid());
                creature->SetImmuneToPlayer(true);
            }
            break;
    }
}

void instance_zulaman::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_MEDICINE_MAN:
        case NPC_TRIBES_MAN:
        case NPC_WARBRINGER:
        case NPC_AXETHROWER:
            if (m_uiBearEventPhase > 3) // prevent crashes during testing
                return;
            if (m_nalorakkEvent[m_uiBearEventPhase].nalorakkTrashSet.find(creature->GetObjectGuid()) != m_nalorakkEvent[m_uiBearEventPhase].nalorakkTrashSet.end())
            {
                ++m_nalorakkEvent[m_uiBearEventPhase].trashKilledCount;
                if (m_nalorakkEvent[m_uiBearEventPhase].trashKilledCount == m_nalorakkEvent[m_uiBearEventPhase].nalorakkTrashSet.size())
                {
                    if (Creature* nalorakk = GetSingleCreatureFromStorage(NPC_NALORAKK))
                    {
                        ++m_uiBearEventPhase;
                        if (m_uiBearEventPhase == MAX_BEAR_WAVES)
                            nalorakk->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                        else
                        {
                            DoScriptText(SAY_RUN_BACK, nalorakk);
                            nalorakk->SetWalk(false);
                            nalorakk->GetMotionMaster()->MovePoint(1, aBearEventInfo[m_uiBearEventPhase].x, aBearEventInfo[m_uiBearEventPhase].y, aBearEventInfo[m_uiBearEventPhase].z);
                        }
                    }
                }
            }
            break;
    }
}

void instance_zulaman::OnCreatureEvade(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_MEDICINE_MAN:
        case NPC_TRIBES_MAN:
        case NPC_WARBRINGER:
        case NPC_AXETHROWER:
            if (m_uiBearEventPhase > 3) // prevent crashes during testing
                return;
            for (auto itr : m_nalorakkEvent[m_uiBearEventPhase].nalorakkTrashSet)
            {
                Creature* temp = instance->GetCreature(itr);

                if (!temp)
                    break;

                temp->ForcedDespawn();
                temp->Respawn();

                temp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            }
            m_nalorakkEvent[m_uiBearEventPhase].trashKilledCount = 0;
            m_isBearPhaseInProgress = false;
            break;
        case NPC_TEMPEST:
        case NPC_PROTECTOR:
        case NPC_WIND_WALKER:
            if (sAkilzonTrashGuidSet.find(creature->GetObjectGuid()) != sAkilzonTrashGuidSet.end())
            {
                SetData(TYPE_AKILZON_GAUNTLET, NOT_STARTED);
                for (auto itr : sAkilzonTrashGuidSet)
                {
                    Creature* pTemp = instance->GetCreature(itr);

                    if (!pTemp)
                        break;

                    if (!pTemp->IsAlive())
                        pTemp->Respawn();
                }
            }
            break;
        case NPC_REINFORCEMENT:
            creature->ForcedDespawn(10000);
            break;
    }
}

void instance_zulaman::OnCreatureRespawn(Creature* creature)
{
	switch (creature->GetEntry())
	{
		case NPC_HATCHLING:
			if (Creature * janalai = GetSingleCreatureFromStorage(NPC_JANALAI))
				janalai->AI()->JustSummoned(creature);
			break;
        case NPC_WORLD_TRIGGER_NOT_IMMUNE_PC:
            creature->SetCanEnterCombat(false);
            creature->AI()->SetReactState(REACT_PASSIVE);
            creature->AI()->SetCombatMovement(false);
            break;
        case NPC_REINFORCEMENT:
            creature->SetInCombatWithZone();
            break;
	}
}

void instance_zulaman::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_STRANGE_GONG:
            break;
        case GO_MASSIVE_GATE:
            // The gate needs to be opened even if the event is still in progress
            if (m_auiEncounter[TYPE_EVENT_RUN] == DONE || m_auiEncounter[TYPE_EVENT_RUN] == FAIL || m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_WIND_DOOR:
            break;
        case GO_LYNX_TEMPLE_ENTRANCE:
            break;
        case GO_LYNX_TEMPLE_EXIT:
            if (m_auiEncounter[TYPE_HALAZZI] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_HEXLORD_ENTRANCE:
            if (GetKilledPreBosses() == 4)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_WOODEN_DOOR:
            if (m_auiEncounter[TYPE_MALACRASS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case GO_FIRE_DOOR:
            break;
        case GO_HARKORS_CAGE:
            break;
        case GO_DWARF_LOOT_BOX:
            break;
        case GO_DWARF_HAMMER:
            break;
        case GO_HARKORS_SATCHEL:
            break;
        case GO_TANZARS_CAGE:
            break;
        case GO_TANZARS_TRUNK:
            break;
        case GO_KRAZS_CAGE:
            break;
        case GO_KRAZS_CHEST:
            break;
        case GO_KRAZS_PACKAGE:
            break;
        case GO_ASHLIS_CAGE:
            break;
        case GO_ASHLIS_BAG:
            break;
        case GO_HARKORS_BREW_KEG:
            break;
        case GO_AMANI_DRUM:
            break;
        case GO_ALTAR_TORCH_EAGLE_GOD:
            if (m_auiEncounter[TYPE_AKILZON] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALTAR_TORCH_DRAGONHAWK_GOD:
            if (m_auiEncounter[TYPE_JANALAI] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALTAR_TORCH_LYNX_GOD:
            if (m_auiEncounter[TYPE_HALAZZI] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALTAR_TORCH_BEAR_GOD:
            if (m_auiEncounter[TYPE_NALORAKK] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_zulaman::SetData(uint32 type, uint32 data)
{
    debug_log("SD2: Instance Zulaman: SetData received for type %u with data %u", type, data);

    switch (type)
    {
        case TYPE_EVENT_RUN:
            if (data == SPECIAL)
            {
                m_auiEncounter[TYPE_EVENT_RUN] = data;
                if (GameObject * gong = GetSingleGameObjectFromStorage(GO_STRANGE_GONG))
                    gong->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                if (Creature * harrisonJones = GetSingleCreatureFromStorage(NPC_HARRISON))
                    harrisonJones->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, harrisonJones, harrisonJones);
                return;
            }
            if (data == IN_PROGRESS)
            {
                DoTimeRunSay(RUN_START);
                DoUseDoorOrButton(GO_MASSIVE_GATE);
                if (m_auiEncounter[TYPE_RUN_EVENT_TIME])
                    SetData(TYPE_RUN_EVENT_TIME, m_auiEncounter[TYPE_RUN_EVENT_TIME]);
                else
                    SetData(TYPE_RUN_EVENT_TIME, 20);   // 20 Minutes as default time

                static_cast<DungeonMap*>(instance)->PermBindAllPlayers();
            }
            if (data == FAIL)
            {
                DoTimeRunSay(RUN_FAIL);
                // Kill remaining Event NPCs
                for (auto& i : m_aEventNpcInfo)
                {
                    // Not yet rescued, so too late
                    if (!i.uiSavePosition)
                    {
                        if (Creature* creature = instance->GetCreature(i.npGuid))
                            creature->ForcedDespawn();
                    }
                }
            }
            if (data == DONE)
                DoTimeRunSay(RUN_DONE);
            m_auiEncounter[type] = data;
            instance->GetVariableManager().SetVariable(WORLD_STATE_ZUL_AMAN_EVENT_RUN_IS_ACTIVE, data == IN_PROGRESS);
            break;
        case TYPE_AKILZON:
            if (data != IN_PROGRESS) // start is done with delay in boss script
                DoUseOpenableObject(GO_WIND_DOOR, true);
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_ALTAR_TORCH_EAGLE_GOD);
                if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
                {
                    m_auiEncounter[TYPE_RUN_EVENT_TIME] += 10; // Add 10 minutes
                    SetData(TYPE_RUN_EVENT_TIME, m_auiEncounter[TYPE_RUN_EVENT_TIME]);
                    DoChestEvent(INDEX_AKILZON);
                }
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_NALORAKK:
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_ALTAR_TORCH_BEAR_GOD);
                if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
                {
                    m_auiEncounter[TYPE_RUN_EVENT_TIME] += 15; // Add 15 minutes
                    SetData(TYPE_RUN_EVENT_TIME, m_auiEncounter[TYPE_RUN_EVENT_TIME]);
                    DoChestEvent(INDEX_NALORAKK);
                }
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_JANALAI:
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_ALTAR_TORCH_DRAGONHAWK_GOD);
                if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
                    DoChestEvent(INDEX_JANALAI);
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_HALAZZI:
            DoUseDoorOrButton(GO_LYNX_TEMPLE_ENTRANCE);
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_ALTAR_TORCH_LYNX_GOD);
                DoUseDoorOrButton(GO_LYNX_TEMPLE_EXIT);
                if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
                    DoChestEvent(INDEX_HALAZZI);
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_MALACRASS:
            DoUseDoorOrButton(GO_HEXLORD_ENTRANCE);
            if (data == DONE)
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_WOODEN_DOOR))
                    pDoor->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            m_auiEncounter[type] = data;
            if (data == FAIL || data == DONE)
                if (Creature* malacrass = GetSingleCreatureFromStorage(NPC_MALACRASS))
                    malacrass->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, malacrass, malacrass);
            break;
        case TYPE_ZULJIN:
            m_auiEncounter[type] = data;
            if (data != IN_PROGRESS) // start is done with delay in boss script
                DoUseOpenableObject(GO_FIRE_DOOR, true);
            if (data == DONE)
            {
                if (Creature* pTanzar = instance->GetCreature(m_aEventNpcInfo[INDEX_NALORAKK].npGuid))
                {
                    if (pTanzar->IsAlive())
                    {
                        pTanzar->HandleEmoteState(EMOTE_ONESHOT_NONE);
                        pTanzar->NearTeleportTo(129.8052f, 807.7782f, 33.37591f, 4.7f);
                        pTanzar->GetMotionMaster()->MoveWaypoint(1, 3, 1000);
                    }
                }
                if (Creature* pHarkor = instance->GetCreature(m_aEventNpcInfo[INDEX_AKILZON].npGuid))
                {
                    if (pHarkor->IsAlive())
                    {
                        pHarkor->NearTeleportTo(130.8155f, 809.079f, 33.37591f, 4.7f);
                        pHarkor->GetMotionMaster()->MoveWaypoint(1, 3, 1000);
                    }
                }
                if (Creature* pAshli = instance->GetCreature(m_aEventNpcInfo[INDEX_HALAZZI].npGuid))
                {
                    if (pAshli->IsAlive())
                    {
                        pAshli->NearTeleportTo(137.0035f, 814.2776f, 33.37591f, 4.7f);
                        pAshli->GetMotionMaster()->MoveWaypoint(1, 3, 1000);
                    }
                }
            }
            
            break;
        case TYPE_TANZAR:
        case TYPE_KRAZ:
        case TYPE_ASHLI:
        case TYPE_HARKOR:
            m_auiEncounter[type] = data;
            break;
        case TYPE_RUN_EVENT_TIME:
            m_auiEncounter[type] = data;
            instance->GetVariableManager().SetVariable(WORLD_STATE_ZUL_AMAN_TIME_COUNTER, data);
            break;
        case TYPE_AKILZON_GAUNTLET:
            m_auiEncounter[type] = data;
            return;
        case TYPE_RAND_VENDOR_1:
            m_auiRandVendor[0] = data;
            break;
        case TYPE_RAND_VENDOR_2:
            m_auiRandVendor[1] = data;
            break;
        default:
            script_error_log("Instance Zulaman: ERROR SetData = %u for type %u does not exist/not implemented.", type, data);
            return;
    }

    if (data == DONE && GetKilledPreBosses() == 4 && (type == TYPE_AKILZON || type == TYPE_NALORAKK || type == TYPE_JANALAI || type == TYPE_HALAZZI))
    {
        DoUseDoorOrButton(GO_HEXLORD_ENTRANCE);
        if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
            SetData(TYPE_EVENT_RUN, DONE);
        SpawnMalacrass();
    }

    if (data == DONE || type == TYPE_RUN_EVENT_TIME || type == TYPE_EVENT_RUN)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_zulaman::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11];

    // Skip m_auiEncounter[7], to start the time event properly if needed
    for (uint8 i = 0; i < MAX_ENCOUNTER - 1; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    // Restart TYPE_EVENT_RUN if was already started
    if (m_auiEncounter[TYPE_RUN_EVENT_TIME] != 0 && m_auiEncounter[TYPE_EVENT_RUN] != DONE && m_auiEncounter[TYPE_EVENT_RUN] != FAIL)
        SetData(TYPE_EVENT_RUN, IN_PROGRESS);
    if (GetData(TYPE_AKILZON) == DONE)
        SetData(TYPE_AKILZON_GAUNTLET, DONE);

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_zulaman::GetData(uint32 type) const
{
    switch (type)
    {
        case TYPE_EVENT_RUN:
        case TYPE_AKILZON:
        case TYPE_NALORAKK:
        case TYPE_JANALAI:
        case TYPE_HALAZZI:
        case TYPE_ZULJIN:
        case TYPE_MALACRASS:
        case TYPE_RUN_EVENT_TIME:
        case TYPE_TANZAR:
        case TYPE_KRAZ:
        case TYPE_ASHLI:
        case TYPE_HARKOR:
            return m_auiEncounter[type];
        case TYPE_RAND_VENDOR_1: return m_auiRandVendor[0];
        case TYPE_RAND_VENDOR_2: return m_auiRandVendor[1];
        default:
            return 0;
    }
}

void instance_zulaman::SendNextBearWave(Unit* target)
{
    Creature* mainGuy = nullptr;
    std::vector<Creature*> followers;
    for (auto itr : m_nalorakkEvent[m_uiBearEventPhase].nalorakkTrashSet)
    {
        Creature* temp = instance->GetCreature(itr);
        if (temp && temp->IsAlive())
        {
            temp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
            if (sWaypointMgr.GetDefaultPath(0, temp->GetGUIDLow(), nullptr)) // mob in each wave has WPs
            {
                mainGuy = temp;
                temp->SetWalk(false);
                temp->GetMotionMaster()->MoveWaypoint();
            }
            else
                followers.push_back(temp);
        }
    }

    if (mainGuy)
        for (Creature* follower : followers)
            follower->GetMotionMaster()->MoveFollow(mainGuy, follower->GetDistance(mainGuy, true, DIST_CALC_COMBAT_REACH), follower->GetAngle(mainGuy), true);
    m_nalorakkEvent[m_uiBearEventPhase].trashKilledCount = 0;
    m_isBearPhaseInProgress = true;
}

bool instance_zulaman::CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instanceConditionId)
    {
        case INSTANCE_CONDITION_ID_NORMAL_MODE:             // Not rescued
        case INSTANCE_CONDITION_ID_HARD_MODE:               // Rescued as first
        case INSTANCE_CONDITION_ID_HARD_MODE_2:             // Rescued as first
        case INSTANCE_CONDITION_ID_HARD_MODE_3:             // Rescued as second
        case INSTANCE_CONDITION_ID_HARD_MODE_4:             // Rescued as third
        {
            if (!conditionSource)
                break;

            int32 index = -1;
            switch (conditionSource->GetEntry())
            {
                case NPC_TANZAR:
                case GO_TANZARS_TRUNK:
                    index = INDEX_NALORAKK;
                    break;
                case NPC_KRAZ:
                case GO_KRAZS_PACKAGE:
                    index = INDEX_JANALAI;
                    break;
                case NPC_ASHLI:
                case GO_ASHLIS_BAG:
                    index = INDEX_HALAZZI;
                    break;
                case NPC_HARKOR:
                case GO_HARKORS_SATCHEL:
                    index = INDEX_AKILZON;
                    break;
            }
            if (index < 0)
                break;

            return m_aEventNpcInfo[index].uiSavePosition == instanceConditionId;
        }
    }

    script_error_log("instance_zulaman::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
                     instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
    return false;
}

uint8 instance_zulaman::GetKilledPreBosses()
{
    return (m_auiEncounter[TYPE_AKILZON] == DONE ? 1 : 0) + (m_auiEncounter[TYPE_NALORAKK] == DONE ? 1 : 0) + (m_auiEncounter[TYPE_JANALAI] == DONE ? 1 : 0) + (m_auiEncounter[TYPE_HALAZZI] == DONE ? 1 : 0);
}

void instance_zulaman::DoTimeRunSay(RunEventSteps data)
{
    switch (data)
    {
        case RUN_START:     DoOrSimulateScriptTextForThisInstance(SAY_INST_BEGIN, NPC_MALACRASS); break;
        case RUN_FAIL:      DoOrSimulateScriptTextForThisInstance(urand(0, 1) ? SAY_INST_SACRIF1 : SAY_INST_SACRIF2, NPC_MALACRASS); break;
        case RUN_DONE:      DoOrSimulateScriptTextForThisInstance(SAY_INST_COMPLETE, NPC_MALACRASS); break;
        case RUN_PROGRESS:
            // This function is on progress called before the data is set to the array
            switch (GetKilledPreBosses() + 1)
            {
                case 1:     DoOrSimulateScriptTextForThisInstance(SAY_INST_PROGRESS_1, NPC_MALACRASS); break;
                case 2:     DoOrSimulateScriptTextForThisInstance(SAY_INST_PROGRESS_2, NPC_MALACRASS); break;
                case 3:     DoOrSimulateScriptTextForThisInstance(SAY_INST_PROGRESS_3, NPC_MALACRASS); break;
            }
            break;
        case RUN_FAIL_SOON:
            switch (GetKilledPreBosses())
            {
                case 0:     DoOrSimulateScriptTextForThisInstance(SAY_INST_WARN_1, NPC_MALACRASS); break;
                case 1:     DoOrSimulateScriptTextForThisInstance(SAY_INST_WARN_2, NPC_MALACRASS); break;
                case 2:     DoOrSimulateScriptTextForThisInstance(SAY_INST_WARN_3, NPC_MALACRASS); break;
                case 3:     DoOrSimulateScriptTextForThisInstance(SAY_INST_WARN_4, NPC_MALACRASS); break;
            }
            break;
    }
}

void instance_zulaman::DoChestEvent(BossToChestIndex uiIndex)
{
    // Store Order of this kill
    m_aEventNpcInfo[uiIndex].uiSavePosition = GetKilledPreBosses() + 1;

    // Do Yell
    DoTimeRunSay(RUN_PROGRESS);

    if (Creature* creature = instance->GetCreature(m_aEventNpcInfo[uiIndex].npGuid))
    {
        creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, creature, creature); // yell for help

        switch (creature->GetEntry())
        {
            case NPC_TANZAR:
                break;
            case NPC_KRAZ:
                if (GameObject* pPackage = GetSingleGameObjectFromStorage(GO_KRAZS_PACKAGE))
                {
                    if (Player* player = GetPlayerInMap(false, false))
                        pPackage->GenerateLootFor(player);
                    pPackage->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                }
                break;
            case NPC_ASHLI:
                if (GameObject* pBag = GetSingleGameObjectFromStorage(GO_ASHLIS_BAG))
                {
                    if (Player* player = GetPlayerInMap(false, false))
                        pBag->GenerateLootFor(player);
                    pBag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                }
                break;
            case NPC_HARKOR:
                if (GameObject* pSatchel = GetSingleGameObjectFromStorage(GO_HARKORS_SATCHEL))
                {
                    if (Player* player = GetPlayerInMap(false, false))
                        pSatchel->GenerateLootFor(player);
                    pSatchel->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                }
                break;
        }
    }

    // related NPC:     m_aEventNpcInfo[uiIndex].npGuid
    // related Chest:   m_aEventNpcInfo[uiIndex]        // Not yet stored, because likely unneeded
}

void instance_zulaman::SpawnMalacrass()
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_CUSTOM_SPAWN_MALACRASS, 1);
}

void instance_zulaman::StartSpiritTimer()
{
    m_spiritFadeTimer = 30000;
}

void instance_zulaman::Update(uint32 diff)
{
    if (m_auiEncounter[TYPE_EVENT_RUN] == IN_PROGRESS)
    {
        if (m_uiEventTimer <= diff)
        {
            if (m_auiEncounter[TYPE_RUN_EVENT_TIME] == 5)   // TODO, verify 5min for warning texts
                DoTimeRunSay(RUN_FAIL_SOON);

            if (m_auiEncounter[TYPE_RUN_EVENT_TIME] == 0)
            {
                debug_log("SD2: Instance Zulaman: event time reach end, event failed.");
                SetData(TYPE_EVENT_RUN, FAIL);
                return;
            }

            --m_auiEncounter[TYPE_RUN_EVENT_TIME];
            SetData(TYPE_RUN_EVENT_TIME, m_auiEncounter[TYPE_RUN_EVENT_TIME]);
            debug_log("SD2: Instance Zulaman: minute decrease to %u.", m_auiEncounter[TYPE_RUN_EVENT_TIME]);

            m_uiEventTimer = MINUTE * IN_MILLISECONDS;
        }
        else
            m_uiEventTimer -= diff;
    }

    if (m_spiritFadeTimer)
    {
        if (m_spiritFadeTimer <= diff)
        {
            static const uint32 spirits[] = { NPC_BEAR_SPIRIT , NPC_EAGLE_SPIRIT, NPC_LYNX_SPIRIT, NPC_DRAGONHAWK_SPIRIT };
            for (uint32 spiritId : spirits)
                if (Creature* spirit = GetSingleCreatureFromStorage(spiritId))
                    spirit->CastSpell(nullptr, SPELL_RETURN_TO_SPIRIT_REALM, TRIGGERED_NONE);
            m_spiritFadeTimer = 0;
        }
        else m_spiritFadeTimer -= diff;
    }
}

void instance_zulaman::ChangeWeather(bool rain)
{
    WorldPacket data(SMSG_WEATHER, 4 + 4 + 1);
    data << uint32(rain ? 4 : 1);
    data << float(rain ? 0.75f : 0);
    data << uint8(0);

    for (auto& ref : instance->GetPlayers())
    {
        Player* player = ref.getSource();
        player->GetSession()->SendPacket(data);
    }
}

void instance_zulaman::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n spawnmalacrass, opengongdoor");
}

void instance_zulaman::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "spawnmalacrass")
    {
        DoUseDoorOrButton(GO_HEXLORD_ENTRANCE);
        SpawnMalacrass();
    }
    else if (val == "opengongdoor")
    {
        SetData(TYPE_EVENT_RUN, SPECIAL);
    }
}

InstanceData* GetInstanceData_instance_zulaman(Map* map)
{
    return new instance_zulaman(map);
}

void AddSC_instance_zulaman()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_zulaman";
    pNewScript->GetInstanceData = &GetInstanceData_instance_zulaman;
    pNewScript->RegisterSelf();
}
