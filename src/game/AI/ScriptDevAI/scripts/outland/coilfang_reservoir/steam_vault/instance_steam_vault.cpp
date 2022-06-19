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
SDName: Instance_Steam_Vault
SD%Complete: 80
SDComment: Instance script and access panel GO
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "steam_vault.h"

/* Steam Vaults encounters:
1 - Hydromancer Thespia Event
2 - Mekgineer Steamrigger Event
3 - Warlord Kalithresh Event
*/

bool GOUse_go_main_chambers_access_panel(Player* /*pPlayer*/, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return true;

    if (pGo->GetEntry() == GO_ACCESS_PANEL_HYDRO && pInstance->GetData(TYPE_HYDROMANCER_THESPIA) != SPECIAL)
        pInstance->SetData(TYPE_HYDROMANCER_THESPIA, SPECIAL);
    else if (pGo->GetEntry() == GO_ACCESS_PANEL_MEK && pInstance->GetData(TYPE_MEKGINEER_STEAMRIGGER) != SPECIAL)
        pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, SPECIAL);

    return false;
}

instance_steam_vault::instance_steam_vault(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_steam_vault::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_steam_vault::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_STEAMRIGGER:
        case NPC_KALITHRESH:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_NAGA_DISTILLER:
            m_lNagaDistillerGuidList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_steam_vault::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_MAIN_CHAMBERS_DOOR:
            if (m_auiEncounter[TYPE_HYDROMANCER_THESPIA] == SPECIAL && m_auiEncounter[TYPE_MEKGINEER_STEAMRIGGER] == SPECIAL)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ACCESS_PANEL_HYDRO:
            if (m_auiEncounter[TYPE_HYDROMANCER_THESPIA] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_ACCESS_PANEL_MEK:
            if (m_auiEncounter[TYPE_MEKGINEER_STEAMRIGGER] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_steam_vault::OnCreatureDeath(Creature* pCreature)
{
    // Break the Warlord spell on the Distiller death
    if (pCreature->GetEntry() == NPC_NAGA_DISTILLER)
    {
        if (Creature* pWarlord = GetSingleCreatureFromStorage(NPC_KALITHRESH))
            pWarlord->InterruptNonMeleeSpells(false);
    }
}

void instance_steam_vault::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_HYDROMANCER_THESPIA:
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_ACCESS_PANEL_HYDRO, GO_FLAG_NO_INTERACT, false);
            if (uiData == SPECIAL)
            {
                if (GetData(TYPE_MEKGINEER_STEAMRIGGER) == SPECIAL)
                    DoUseDoorOrButton(GO_MAIN_CHAMBERS_DOOR);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MEKGINEER_STEAMRIGGER:
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_ACCESS_PANEL_MEK, GO_FLAG_NO_INTERACT, false);
            if (uiData == SPECIAL)
            {
                if (GetData(TYPE_HYDROMANCER_THESPIA) == SPECIAL)
                    DoUseDoorOrButton(GO_MAIN_CHAMBERS_DOOR);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_WARLORD_KALITHRESH:
            DoUseDoorOrButton(GO_MAIN_CHAMBERS_DOOR);
            if (uiData == FAIL)
            {
                // Reset Distiller flags - respawn is handled by DB
                for (GuidList::const_iterator itr = m_lNagaDistillerGuidList.begin(); itr != m_lNagaDistillerGuidList.end(); ++itr)
                {
                    if (Creature* pDistiller = instance->GetCreature(*itr))
                    {
                        if (!pDistiller->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
                            pDistiller->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                    }
                }
            }
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_steam_vault::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_steam_vault::Load(const char* chrIn)
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
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_steam_vault(Map* pMap)
{
    return new instance_steam_vault(pMap);
}

void AddSC_instance_steam_vault()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "go_main_chambers_access_panel";
    pNewScript->pGOUse = &GOUse_go_main_chambers_access_panel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "instance_steam_vault";
    pNewScript->GetInstanceData = &GetInstanceData_instance_steam_vault;
    pNewScript->RegisterSelf();
}
