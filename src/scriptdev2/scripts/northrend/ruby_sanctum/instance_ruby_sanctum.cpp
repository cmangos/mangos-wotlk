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
SDName: instance_ruby_sanctum
SD%Complete: 30%
SDComment: Basic instance script
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

instance_ruby_sanctum::instance_ruby_sanctum(Map* pMap) : ScriptedInstance(pMap),
    m_uiHalionSummonTimer(0),
    m_uiHalionSummonStage(0),
    m_uiHalionResetTimer(0)
{
    Initialize();
}

void instance_ruby_sanctum::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_ruby_sanctum::IsEncounterInProgress() const
{
    for (uint8 i = 1; i < MAX_ENCOUNTER ; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_ruby_sanctum::OnPlayerEnter(Player* /*pPlayer*/)
{
    // Return if Halion already dead, or Zarithrian alive
    if (m_auiEncounter[TYPE_ZARITHRIAN] != DONE || m_auiEncounter[TYPE_HALION] == DONE)
        return;

    // Return if already summoned
    if (GetSingleCreatureFromStorage(NPC_HALION_REAL, true))
        return;

    if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
        pSummoner->SummonCreature(NPC_HALION_REAL, pSummoner->GetPositionX(), pSummoner->GetPositionY(), pSummoner->GetPositionZ(), 3.159f, TEMPSUMMON_DEAD_DESPAWN, 0);
}

void instance_ruby_sanctum::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_XERESTRASZA:
            // Special case for Xerestrasza: she only needs to have questgiver flag if Baltharus is killed
            if (m_auiEncounter[TYPE_BALTHARUS] != DONE)
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ZARITHRIAN:
            if (m_auiEncounter[TYPE_SAVIANA] == DONE && m_auiEncounter[TYPE_BALTHARUS] == DONE)
                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            // no break;
        case NPC_BALTHARUS:
        case NPC_HALION_REAL:
        case NPC_HALION_TWILIGHT:
        case NPC_HALION_CONTROLLER:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ZARITHRIAN_SPAWN_STALKER:
            m_lSpawnStalkersGuidList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_ruby_sanctum::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_FLAME_WALLS:
            if (m_auiEncounter[TYPE_SAVIANA] == DONE && m_auiEncounter[TYPE_BALTHARUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FIRE_FIELD:
            if (m_auiEncounter[TYPE_BALTHARUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_FLAME_RING:
            break;
        case GO_BURNING_TREE_1:
        case GO_BURNING_TREE_2:
        case GO_BURNING_TREE_3:
        case GO_BURNING_TREE_4:
            if (m_auiEncounter[TYPE_ZARITHRIAN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_TWILIGHT_PORTAL_ENTER_1:
        case GO_TWILIGHT_PORTAL_ENTER_2:
        case GO_TWILIGHT_PORTAL_LEAVE:
            break;
        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

// Wrapper to unlock the flame wall in from of Zarithrian
void instance_ruby_sanctum::DoHandleZarithrianDoor()
{
    if (m_auiEncounter[TYPE_SAVIANA] == DONE && m_auiEncounter[TYPE_BALTHARUS] == DONE)
    {
        DoUseDoorOrButton(GO_FLAME_WALLS);

        // Also remove not_selectable unit flag
        if (Creature* pZarithrian = GetSingleCreatureFromStorage(NPC_ZARITHRIAN))
            pZarithrian->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
}

void instance_ruby_sanctum::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SAVIANA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoHandleZarithrianDoor();
            break;
        case TYPE_BALTHARUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoHandleZarithrianDoor();
                DoUseDoorOrButton(GO_FIRE_FIELD);

                // Start outro event by DB script
                if (Creature* pXerestrasza = GetSingleCreatureFromStorage(NPC_XERESTRASZA))
                    pXerestrasza->GetMotionMaster()->MoveWaypoint();
            }
            break;
        case TYPE_ZARITHRIAN:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_FLAME_WALLS);
            if (uiData == DONE)
            {
                // Start Halion summoning process
                if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                {
                    pSummoner->CastSpell(pSummoner, SPELL_FIRE_PILLAR, false);
                    m_uiHalionSummonTimer = 5000;
                }
            }
            break;
        case TYPE_HALION:
            // Don't set the same data twice
            if (m_auiEncounter[uiType] == uiData)
                return;
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_FLAME_RING);

            // encounter unit frame
            if (uiData == DONE || uiData == FAIL)
            {
                // remove encounter frames
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_HALION_REAL))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pDragon->GetObjectGuid());
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pDragon->GetObjectGuid());
            }

            // cleanup
            switch (uiData)
            {
                case FAIL:
                    // Despawn the boss
                    if (Creature* pHalion = GetSingleCreatureFromStorage(NPC_HALION_REAL))
                        pHalion->ForcedDespawn();
                    if (Creature* pHalion = GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                        pHalion->ForcedDespawn();
                    // Note: rest of the cleanup is handled by creature_linking

                    m_uiHalionResetTimer = 30000;
                    // no break;
                case DONE:
                    // clear debuffs
                    if (Creature* pController = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                        pController->CastSpell(pController, SPELL_CLEAR_DEBUFFS, true);

                    // Despawn the portals
                    if (GameObject* pPortal = GetSingleGameObjectFromStorage(GO_TWILIGHT_PORTAL_ENTER_1))
                        pPortal->SetLootState(GO_JUST_DEACTIVATED);

                    // ToDo: despawn the other portals as well, and disable world state
                    break;
            }
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_ruby_sanctum::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_ruby_sanctum::Update(uint32 uiDiff)
{
    if (m_uiHalionSummonTimer)
    {
        if (m_uiHalionSummonTimer <= uiDiff)
        {
            switch (m_uiHalionSummonStage)
            {
                case 0:
                    // Burn the first line of trees
                    DoUseDoorOrButton(GO_BURNING_TREE_1);
                    DoUseDoorOrButton(GO_BURNING_TREE_2);
                    m_uiHalionSummonTimer = 5000;
                    break;
                case 1:
                    // Burn the second line of trees
                    DoUseDoorOrButton(GO_BURNING_TREE_3);
                    DoUseDoorOrButton(GO_BURNING_TREE_4);
                    m_uiHalionSummonTimer = 4000;
                    break;
                case 2:
                    // Cast Fiery explosion
                    if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                        pSummoner->CastSpell(pSummoner, SPELL_FIERY_EXPLOSION, true);
                    m_uiHalionSummonTimer = 2000;
                case 3:
                    // Spawn Halion
                    if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                    {
                        if (Creature* pHalion = pSummoner->SummonCreature(NPC_HALION_REAL, pSummoner->GetPositionX(), pSummoner->GetPositionY(), pSummoner->GetPositionZ(), 3.159f, TEMPSUMMON_DEAD_DESPAWN, 0))
                            DoScriptText(SAY_HALION_SPAWN, pHalion);
                    }
                    m_uiHalionSummonTimer = 0;
                    break;
            }
            ++m_uiHalionSummonStage;
        }
        else
            m_uiHalionSummonTimer -= uiDiff;
    }

    // Resummon Halion if the encounter resets
    if (m_uiHalionResetTimer)
    {
        if (m_uiHalionResetTimer <= uiDiff)
        {
            if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_HALION_CONTROLLER))
                pSummoner->SummonCreature(NPC_HALION_REAL, pSummoner->GetPositionX(), pSummoner->GetPositionY(), pSummoner->GetPositionZ(), 3.159f, TEMPSUMMON_DEAD_DESPAWN, 0);

            if (Creature* pHalion = GetSingleCreatureFromStorage(NPC_HALION_TWILIGHT))
                pHalion->Respawn();

            m_uiHalionResetTimer = 0;
        }
        else
            m_uiHalionResetTimer -= uiDiff;
    }
}

void instance_ruby_sanctum::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_ruby_sanctum(Map* pMap)
{
    return new instance_ruby_sanctum(pMap);
}

void AddSC_instance_ruby_sanctum()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_ruby_sanctum";
    pNewScript->GetInstanceData = &GetInstanceData_instance_ruby_sanctum;
    pNewScript->RegisterSelf();
}
