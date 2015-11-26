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
SDName: instance_halls_of_reflection
SD%Complete: 20
SDComment: Basic support
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"

enum
{
    SPELL_SPIRIT_ACTIVATE_VISUAL            = 72130,            // cast when activate spirit
};

instance_halls_of_reflection::instance_halls_of_reflection(Map* pMap) : ScriptedInstance(pMap),
    m_uiTeam(TEAM_NONE),
    m_uiEventTimer(0),
    m_uiActivateTimer(0),
    m_uiEventStage(0)
{
    Initialize();
}

void instance_halls_of_reflection::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_halls_of_reflection::OnPlayerEnter(Player* pPlayer)
{
    if (!m_uiTeam)                                          // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();

        if (GetData(TYPE_FROSTMOURNE_INTRO) == DONE)
            return;

        // Spawn intro npcs and make the start the movement
        for (uint8 i = 0; i < countof(aEventBeginLocations); ++i)
        {
            if (Creature* pCreature = pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocations[i].uiEntryHorde : aEventBeginLocations[i].uiEntryAlliance,
                aEventBeginLocations[i].fX, aEventBeginLocations[i].fY, aEventBeginLocations[i].fZ, aEventBeginLocations[i].fO, TEMPSUMMON_DEAD_DESPAWN, 0, true))
            {
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                pCreature->GetMotionMaster()->MoveWaypoint();
            }
        }
    }

    // ToDo: spawn Uther for Quel'delar quest
}

void instance_halls_of_reflection::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_JAINA_PART1:
        case NPC_JAINA_PART2:
        case NPC_SYLVANAS_PART1:
        case NPC_SYLVANAS_PART2:
        case NPC_LICH_KING:
            break;
        case NPC_FALRIC:
            // Start event after intro
            if (m_auiEncounter[TYPE_FALRIC] != DONE)
                SetData(TYPE_FALRIC, SPECIAL);
            break;
        case NPC_MARWYN:
            // Start event after event wipe, but Falric is done
            if (m_auiEncounter[TYPE_MARWYN] != DONE && m_auiEncounter[TYPE_FALRIC] == DONE)
                SetData(TYPE_MARWYN, SPECIAL);
            break;
        case NPC_PHANTOM_MAGE:
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            m_lRisenSpiritsGuids.push_back(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }
    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_halls_of_reflection::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_IMPENETRABLE_DOOR:
            if (m_auiEncounter[TYPE_MARWYN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FROSTMOURNE:
            // Note: Frostmourne shouldn't be loaded if the intro is done
            if (m_auiEncounter[TYPE_FROSTMOURNE_INTRO] == DONE)
                pGo->Delete();
            break;
        case GO_FROSTMOURNE_ALTAR:
        case GO_ICECROWN_DOOR_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_ENTRANCE:
        case GO_ICECROWN_DOOR_LK_EXIT:
        case GO_CAVE_IN:

        case GO_CAPTAIN_CHEST_HORDE:
        case GO_CAPTAIN_CHEST_HORDE_H:
        case GO_CAPTAIN_CHEST_ALLIANCE:
        case GO_CAPTAIN_CHEST_ALLIANCE_H:
            break;
        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_halls_of_reflection::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_FROSTMOURNE_INTRO:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FALRIC:
            if (uiData == DONE)
                m_uiEventTimer = 60000;
            else if (uiData == FAIL)
                DoCleanupFrostmourneEvent();
            else if (uiData == SPECIAL)
            {
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

                m_uiEventTimer = 40000;
                m_uiEventStage = 0;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MARWYN:
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_IMPENETRABLE_DOOR);
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);
                DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 0);
            }
            else if (uiData == FAIL)
                DoCleanupFrostmourneEvent();
            else if (uiData == SPECIAL)
            {
                DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

                m_uiEventTimer = 60000;
                m_uiEventStage = 5;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_LICH_KING:
            if (uiData == DONE)
            {
                uint32 uiChestEntry = m_uiTeam == ALLIANCE ? (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_ALLIANCE : GO_CAPTAIN_CHEST_ALLIANCE_H) :
                        (instance->IsRegularDifficulty() ? GO_CAPTAIN_CHEST_HORDE : GO_CAPTAIN_CHEST_HORDE_H);
                DoToggleGameObjectFlags(uiChestEntry, GO_FLAG_NO_INTERACT, false);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE || uiData == FAIL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_halls_of_reflection::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2]  >> m_auiEncounter[3];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_halls_of_reflection::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_halls_of_reflection::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, DONE); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, DONE); break;

        case NPC_PHANTOM_MAGE:
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            m_lActiveSpiritsGuids.remove(pCreature->GetObjectGuid());
            if (m_lActiveSpiritsGuids.empty())
                m_uiEventTimer = 1000;
            break;
    }
}
void instance_halls_of_reflection::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, FAIL); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, FAIL); break;

        case NPC_PHANTOM_MAGE:
            if (pCreature->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                break;
        case NPC_SPECTRAL_FOOTMAN:
        case NPC_GHOSTLY_PRIEST:
        case NPC_TORTURED_RIFLEMAN:
        case NPC_SHADOWY_MERCENARY:
            if (m_uiEventStage <= 5)
                SetData(TYPE_FALRIC, FAIL);
            else
                SetData(TYPE_MARWYN, FAIL);
            break;
    }
}

void instance_halls_of_reflection::OnCreatureDespawn(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_LICH_KING_INTRO)
        SetData(TYPE_FROSTMOURNE_INTRO, DONE);
}

void instance_halls_of_reflection::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FALRIC: SetData(TYPE_FALRIC, IN_PROGRESS); break;
        case NPC_MARWYN: SetData(TYPE_MARWYN, IN_PROGRESS); break;
    }
}

// Function to send the next spirit wave
void instance_halls_of_reflection::DoSendNextSpiritWave()
{
    uint8 m_uiMaxMobs = 0;
    DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES_COUNT, m_uiEventStage);

    switch (m_uiEventStage)
    {
        case 1:
        case 6:
            // First wave in the series
            m_uiMaxMobs = 3;
            DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 1);
            break;
        case 2:
        case 7:
            // Second wave in the series
            m_uiMaxMobs = 4;
            break;
        case 5:
            // Falric (wave 5)
            if (Creature* pFalric = GetSingleCreatureFromStorage(NPC_FALRIC))
            {
                pFalric->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                if (Player* pPlayer = GetPlayerInMap(true, false))
                    pFalric->AI()->AttackStart(pPlayer);

                m_uiEventTimer = 0;
            }
            return;
        case 10:
            // Marwyn (wave 10)
            if (Creature* pMarwyn = GetSingleCreatureFromStorage(NPC_MARWYN))
            {
                pMarwyn->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                if (Player* pPlayer = GetPlayerInMap(true, false))
                    pMarwyn->AI()->AttackStart(pPlayer);

                m_uiEventTimer = 0;
            }
            return;
        default:
            // Third and fourth wave of the series
            m_uiMaxMobs = 5;
            break;
    }

    // Activate random spirits
    for (uint8 i = 0; i < m_uiMaxMobs; ++i)
    {
        if (m_lRisenSpiritsGuids.empty())
            return;

        GuidList::iterator iter = m_lRisenSpiritsGuids.begin();
        advance(iter, urand(0, m_lRisenSpiritsGuids.size() - 1));

        if (Creature* pCreature = instance->GetCreature(*iter))
        {
            if (!pCreature->isAlive())
            {
                script_error_log("instance_halls_of_reflection: Error: couldn't find alive creature %u", pCreature->GetEntry());
                return;
            }

            pCreature->CastSpell(pCreature, SPELL_SPIRIT_ACTIVATE_VISUAL, false);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            m_lActiveSpiritsGuids.push_back(*iter);
            m_lRisenSpiritsGuids.remove(*iter);
        }
        else
            script_error_log("instance_halls_of_reflection: Error: couldn't find creature %u", pCreature->GetEntry());
    }

    m_uiActivateTimer = 3000;
}

// Function to cleanup the first two encounters events
void instance_halls_of_reflection::DoCleanupFrostmourneEvent()
{
    // Cleanup boses
    if (GetData(TYPE_FALRIC) != DONE)
    {
        if (Creature* pFalric = GetSingleCreatureFromStorage(NPC_FALRIC))
            pFalric->ForcedDespawn();
    }
    if (Creature* pMarwyn = GetSingleCreatureFromStorage(NPC_MARWYN))
        pMarwyn->ForcedDespawn();

    // Cleanup trash
    for (GuidList::const_iterator itr = m_lRisenSpiritsGuids.begin(); itr != m_lRisenSpiritsGuids.end(); ++itr)
    {
        if (Creature* pCreature = instance->GetCreature(*itr))
            pCreature->ForcedDespawn();
    }
    for (GuidList::const_iterator itr = m_lActiveSpiritsGuids.begin(); itr != m_lActiveSpiritsGuids.end(); ++itr)
    {
        if (Creature* pCreature = instance->GetCreature(*itr))
            pCreature->ForcedDespawn();
    }

    // Remove world state and open door
    DoUpdateWorldState(WORLD_STATE_SPIRIT_WAVES, 0);
    DoUseDoorOrButton(GO_ICECROWN_DOOR_ENTRANCE);

    m_lRisenSpiritsGuids.clear();
    m_lActiveSpiritsGuids.clear();

    m_uiEventTimer = 0;
}

void instance_halls_of_reflection::Update(uint32 uiDiff)
{
    // Main spirits event timer
    if (m_uiEventTimer)
    {
        if (m_uiEventTimer <= uiDiff)
        {
            ++m_uiEventStage;
            m_uiEventTimer = 60000;
            DoSendNextSpiritWave();
        }
        else
            m_uiEventTimer -= uiDiff;
    }

    // Activate spirits after a few seconds only
    if (m_uiActivateTimer)
    {
        if (m_uiActivateTimer <= uiDiff)
        {
            Player* pPlayer = GetPlayerInMap(true, false);
            if (!pPlayer)
            {
                script_error_log("instance_halls_of_reflection: Error: couldn't find any player alive in instance");
                return;
            }

            for (GuidList::const_iterator itr = m_lActiveSpiritsGuids.begin(); itr != m_lActiveSpiritsGuids.end(); ++itr)
            {
                if (Creature* pCreature = instance->GetCreature(*itr))
                {
                    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                    pCreature->AI()->AttackStart(pPlayer);
                }
            }
            m_uiActivateTimer = 0;
        }
        else
            m_uiActivateTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_halls_of_reflection(Map* pMap)
{
    return new instance_halls_of_reflection(pMap);
}

void AddSC_instance_halls_of_reflection()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_reflection";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_reflection;
    pNewScript->RegisterSelf();
}
