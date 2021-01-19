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
SDName: instance_forge_of_souls
SD%Complete: 90%
SDComment:
SDCategory: The Forge of Souls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "forge_of_souls.h"

instance_forge_of_souls::instance_forge_of_souls(Map* pMap) : ScriptedInstance(pMap),
    m_bCriteriaPhantomBlastFailed(false),
    m_uiTeam(0)
{
    Initialize();
}

void instance_forge_of_souls::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_forge_of_souls::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_BRONJAHM:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;

        case NPC_CORRUPTED_SOUL_FRAGMENT:
            m_luiSoulFragmentAliveGUIDs.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_forge_of_souls::OnPlayerEnter(Player* pPlayer)
{
    if (!m_uiTeam)                                          // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();
        ProcessEventNpcs(pPlayer);
    }
}

void instance_forge_of_souls::ProcessEventNpcs(Player* pPlayer)
{
    if (!pPlayer)
        return;

    if (GetData(TYPE_BRONJAHM) != DONE || GetData(TYPE_DEVOURER_OF_SOULS) != DONE)
    {
        // Spawn Begin Mobs
        for (const auto& aEventBeginLocation : aEventBeginLocations)
        {
            if (Creature* pSummon = pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocation.uiEntryHorde : aEventBeginLocation.uiEntryAlliance,
                aEventBeginLocation.fSpawnX, aEventBeginLocation.fSpawnY, aEventBeginLocation.fSpawnZ, aEventBeginLocation.fSpawnO, TEMPSPAWN_DEAD_DESPAWN, 24 * HOUR * IN_MILLISECONDS))
                m_lEventMobGUIDs.push_back(pSummon->GetObjectGuid());
        }
    }
    else
    {
        // Despawn entrance mobs
        for (const auto& guid : m_lEventMobGUIDs)
            if (Creature* pSummoned = instance->GetCreature(guid))
                pSummoned->ForcedDespawn();

        // spawn exit mobs; movement and texts handled by DB script
        for (const auto& aEventEndLocation : aEventEndLocations)
        {
            if (Creature* pCreature = pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventEndLocation.uiEntryHorde : aEventEndLocation.uiEntryAlliance,
                aEventEndLocation.fSpawnX, aEventEndLocation.fSpawnY, aEventEndLocation.fSpawnZ, aEventEndLocation.fOrient, TEMPSPAWN_DEAD_DESPAWN, 24 * HOUR * IN_MILLISECONDS))
            {
                pCreature->SetWalk(false);
                pCreature->GetMotionMaster()->MoveWaypoint(aEventEndLocation.pathId);
            }
        }
    }
}

bool instance_forge_of_souls::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_SOUL_POWER:
            return m_luiSoulFragmentAliveGUIDs.size() >= 4;
        case ACHIEV_CRIT_PHANTOM_BLAST:
            return !m_bCriteriaPhantomBlastFailed;
        default:
            return false;
    }
}

void instance_forge_of_souls::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_BRONJAHM:
            m_auiEncounter[0] = uiData;

            // Despawn remaining adds and clear list
            for (const auto& guid : m_luiSoulFragmentAliveGUIDs)
                if (Creature* pFragment = instance->GetCreature(guid))
                    pFragment->ForcedDespawn();

            m_luiSoulFragmentAliveGUIDs.clear();
            break;
        case TYPE_DEVOURER_OF_SOULS:
            m_auiEncounter[1] = uiData;
            if (uiData == DONE)
                ProcessEventNpcs(GetPlayerInMap());
            break;
        case TYPE_ACHIEV_PHANTOM_BLAST:
            m_bCriteriaPhantomBlastFailed = (uiData == FAIL);
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_forge_of_souls::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_forge_of_souls::GetData(uint32 uiType) const
{
    switch (uiType)
    {
        case TYPE_BRONJAHM:
            return m_auiEncounter[0];
        case TYPE_DEVOURER_OF_SOULS:
            return m_auiEncounter[1];
        default:
            return 0;
    }
}

void instance_forge_of_souls::SetData64(uint32 uiType, uint64 uiData)
{
    if (uiType == DATA_SOULFRAGMENT_REMOVE)
        m_luiSoulFragmentAliveGUIDs.remove(ObjectGuid(uiData));
}

InstanceData* GetInstanceData_instance_forge_of_souls(Map* pMap)
{
    return new instance_forge_of_souls(pMap);
}

void AddSC_instance_forge_of_souls()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_forge_of_souls";
    pNewScript->GetInstanceData = &GetInstanceData_instance_forge_of_souls;
    pNewScript->RegisterSelf();
}
