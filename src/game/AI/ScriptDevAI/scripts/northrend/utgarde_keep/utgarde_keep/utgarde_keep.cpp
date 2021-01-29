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
SDName: Instance_Utgarde_Keep
SD%Complete: 20%
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_keep.h"

enum
{
    SAY_SKA_DEATH               = -1574012,
    SAY_SKA_DEATH_REAL          = -1574013,

    SAY_DAL_DEATH               = -1574017,
    SAY_DAL_DEATH_REAL          = -1574018,

    SPELL_SUMMON_DAL_GHOST      = 48612,
    SPELL_SUMMON_SKA_GHOST      = 48613,
};

instance_utgarde_keep::instance_utgarde_keep(Map* pMap) : ScriptedInstance(pMap),
    m_bKelesethAchievFailed(false)
{
    Initialize();
}

void instance_utgarde_keep::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_utgarde_keep::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KELESETH:
        case NPC_SKARVALD:
        case NPC_DALRONN:
        case NPC_INGVAR:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_utgarde_keep::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FROST_TOMB:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
        case NPC_DALRONN_GHOST:
            if (Creature* pSkarvald = GetSingleCreatureFromStorage(NPC_SKARVALD))
            {
                if (pSkarvald->IsAlive() && pSkarvald->GetVictim())
                    pCreature->AI()->AttackStart(pSkarvald->GetVictim());
            }
            pCreature->SetForceAttackingCapability(true);
            break;
        case NPC_SKARVALD_GHOST:
            if (Creature* pDarlonn = GetSingleCreatureFromStorage(NPC_DALRONN))
            {
                if (pDarlonn->IsAlive() && pDarlonn->GetVictim())
                    pCreature->AI()->AttackStart(pDarlonn->GetVictim());
            }
            pCreature->SetForceAttackingCapability(true);
            break;
    }
}

void instance_utgarde_keep::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_BELLOW_1:
            if (m_auiEncounter[TYPE_BELLOW_1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_BELLOW_2:
            if (m_auiEncounter[TYPE_BELLOW_2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_BELLOW_3:
            if (m_auiEncounter[TYPE_BELLOW_3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_1:
            if (m_auiEncounter[TYPE_BELLOW_1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_2:
            if (m_auiEncounter[TYPE_BELLOW_2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FORGEFIRE_3:
            if (m_auiEncounter[TYPE_BELLOW_3] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORTCULLIS_EXIT_1:
        case GO_PORTCULLIS_EXIT_2:
            if (m_auiEncounter[TYPE_INGVAR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PORTCULLIS_COMBAT:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_utgarde_keep::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FROST_TOMB:
            m_bKelesethAchievFailed = true;
            break;
        case NPC_SKARVALD:
            if (Creature* pDalronn = GetSingleCreatureFromStorage(NPC_DALRONN))
            {
                if (pDalronn->IsAlive())
                {
                    pCreature->CastSpell(pCreature, SPELL_SUMMON_SKA_GHOST, TRIGGERED_OLD_TRIGGERED);
                    pCreature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    DoScriptText(SAY_SKA_DEATH, pCreature);
                }
                else
                {
                    pDalronn->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    DoScriptText(SAY_SKA_DEATH_REAL, pCreature);
                }
            }
            break;
        case NPC_DALRONN:
            if (Creature* pSkarvald = GetSingleCreatureFromStorage(NPC_SKARVALD))
            {
                if (pSkarvald->IsAlive())
                {
                    pCreature->CastSpell(pCreature, SPELL_SUMMON_DAL_GHOST, TRIGGERED_OLD_TRIGGERED);
                    pCreature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    DoScriptText(SAY_DAL_DEATH, pCreature);
                }
                else
                {
                    pSkarvald->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    DoScriptText(SAY_DAL_DEATH_REAL, pCreature);
                }
            }
            break;
    }
}

void instance_utgarde_keep::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_KELESETH:
            if (uiData == IN_PROGRESS)
                m_bKelesethAchievFailed = false;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SKARVALD_DALRONN:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_INGVAR:
            if (m_auiEncounter[uiType] == uiData)
                return;
            DoUseDoorOrButton(GO_PORTCULLIS_COMBAT);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_PORTCULLIS_EXIT_1);
                DoUseDoorOrButton(GO_PORTCULLIS_EXIT_2);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_1:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_2:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_BELLOW_3:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
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

uint32 instance_utgarde_keep::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_utgarde_keep::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

bool instance_utgarde_keep::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    if (uiCriteriaId == ACHIEV_CRIT_ON_THE_ROCKS)
        return !m_bKelesethAchievFailed;

    return false;
}

InstanceData* GetInstanceData_instance_utgarde_keep(Map* pMap)
{
    return new instance_utgarde_keep(pMap);
}

void AddSC_instance_utgarde_keep()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_utgarde_keep";
    pNewScript->GetInstanceData = GetInstanceData_instance_utgarde_keep;
    pNewScript->RegisterSelf();
}
