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
SDName: Instance_Magisters_Terrace
SD%Complete: 80
SDComment:
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "magisters_terrace.h"

/*
0  - Selin Fireheart
1  - Vexallus
2  - Priestess Delrissa
3  - Kael'thas Sunstrider
*/

instance_magisters_terrace::instance_magisters_terrace(Map* pMap) : ScriptedInstance(pMap), m_kalecgosOrbHandled(false)
{
    Initialize();
}

void instance_magisters_terrace::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_magisters_terrace::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SELIN_FIREHEART:
        case NPC_DELRISSA:
        case NPC_KALECGOS_DRAGON:
        case NPC_KAELTHAS:
        case NPC_SCRYERS_BUNNY:
        // insert Delrissa adds here, for better handling
        case NPC_KAGANI:
        case NPC_ELLRYS:
        case NPC_ERAMAS:
        case NPC_YAZZAI:
        case NPC_SALARIS:
        case NPC_GARAXXAS:
        case NPC_APOKO:
        case NPC_ZELFAN:
        case NPC_SLIVER:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_FEL_CRYSTAL:
            m_lFelCrystalGuid.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_magisters_terrace::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_VEXALLUS_DOOR:
            if (m_auiEncounter[TYPE_VEXALLUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SELIN_DOOR:
            if (m_auiEncounter[TYPE_SELIN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DELRISSA_DOOR:
            if (m_auiEncounter[TYPE_DELRISSA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SELIN_ENCOUNTER_DOOR:
        case GO_KAEL_DOOR:
        case GO_ESCAPE_QUEL_DANAS:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_magisters_terrace::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SELIN:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SELIN_DOOR);
            if (uiData == FAIL)
            {
                // Reset crystals - respawn and kill is handled by creature linking
                for (GuidList::const_iterator itr = m_lFelCrystalGuid.begin(); itr != m_lFelCrystalGuid.end(); ++itr)
                {
                    if (Creature* pTemp = instance->GetCreature(*itr))
                    {
                        if (!pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
                            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

                        if (pTemp->IsAlive())
                        {
                            if (pTemp->IsInCombat())
                            {
                                pTemp->ForcedDespawn();
                                pTemp->Respawn();
                            }
                            else
                                pTemp->CastSpell(nullptr, SPELL_FEL_CRYSTAL_VISUAL, TRIGGERED_NONE);
                        }
                        else
                            pTemp->Respawn();
                    }
                }
            }
            if (uiData == IN_PROGRESS)
            {
                // Stop channeling when the fight starts
                for (GuidList::const_iterator itr = m_lFelCrystalGuid.begin(); itr != m_lFelCrystalGuid.end(); ++itr)
                {
                    if (Creature* pTemp = instance->GetCreature(*itr))
                        pTemp->InterruptNonMeleeSpells(false);
                }
            }
            DoUseDoorOrButton(GO_SELIN_ENCOUNTER_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_VEXALLUS:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_VEXALLUS_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_DELRISSA:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_DELRISSA_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_KAELTHAS:
            DoUseDoorOrButton(GO_KAEL_DOOR);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_ESCAPE_QUEL_DANAS, GO_FLAG_NO_INTERACT, false);
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_magisters_terrace::StartCrystalVisual()
{
    for (GuidList::const_iterator itr = m_lFelCrystalGuid.begin(); itr != m_lFelCrystalGuid.end(); ++itr)
    {
        if (Creature* pTemp = instance->GetCreature(*itr))
            if (pTemp->IsAlive())
                pTemp->CastSpell(nullptr, SPELL_FEL_CRYSTAL_VISUAL, TRIGGERED_NONE);
    }
}

void instance_magisters_terrace::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_magisters_terrace::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void AddSC_instance_magisters_terrace()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_magisters_terrace";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_magisters_terrace>;
    pNewScript->RegisterSelf();
}
