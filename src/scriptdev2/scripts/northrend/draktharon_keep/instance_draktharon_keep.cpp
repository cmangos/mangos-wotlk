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
SDName: instance_draktharon_keep
SD%Complete: 50%
SDComment:
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

instance_draktharon_keep::instance_draktharon_keep(Map* pMap) : ScriptedInstance(pMap),
    m_uiDreadAddsKilled(0),
    m_bNovosAddGrounded(false),
    m_bTrollgoreConsume(true)
{
    Initialize();
}

void instance_draktharon_keep::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_draktharon_keep::OnCreatureEnterCombat(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_KING_DRED)
        SetData(TYPE_KING_DRED, IN_PROGRESS);
}

void instance_draktharon_keep::OnCreatureEvade(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_KING_DRED)
        SetData(TYPE_KING_DRED, FAIL);
}

void instance_draktharon_keep::OnCreatureDeath(Creature* pCreature)
{
    if ((pCreature->GetEntry() == NPC_DRAKKARI_GUTRIPPER || pCreature->GetEntry() == NPC_DRAKKARI_SCYTHECLAW) && m_auiEncounter[TYPE_KING_DRED] == IN_PROGRESS)
        ++m_uiDreadAddsKilled;

    if (pCreature->GetEntry() == NPC_KING_DRED)
        SetData(TYPE_KING_DRED, DONE);
}

void instance_draktharon_keep::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_NOVOS:
            m_mNpcEntryGuidStore[NPC_NOVOS] = pCreature->GetObjectGuid();
            break;
        case NPC_CRYSTAL_CHANNEL_TARGET:
            m_lNovosDummyGuids.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER:
            if (pCreature->GetPositionZ() > 30.0f)
                m_vTriggerGuids.push_back(pCreature->GetObjectGuid());
            else
                m_trollgoreCornerTriggerGuid = pCreature->GetObjectGuid();
            break;
    }
}

void instance_draktharon_keep::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CRYSTAL_SW: m_aNovosCrystalInfo[0].m_crystalGuid = pGo->GetObjectGuid(); break;
        case GO_CRYSTAL_NW: m_aNovosCrystalInfo[1].m_crystalGuid = pGo->GetObjectGuid(); break;
        case GO_CRYSTAL_SE: m_aNovosCrystalInfo[2].m_crystalGuid = pGo->GetObjectGuid(); break;
        case GO_CRYSTAL_NE: m_aNovosCrystalInfo[3].m_crystalGuid = pGo->GetObjectGuid(); break;
    }
}

void instance_draktharon_keep::DoSortNovosDummies()
{
    // Sorting once is good enough
    if (m_lNovosDummyGuids.empty())
        return;

    Creature* pNovos = GetSingleCreatureFromStorage(NPC_NOVOS);
    if (!pNovos)
        return;

    // First sort the Dummies to the Crystals
    for (uint8 i = 0; i < MAX_CRYSTALS; ++i)
    {
        GameObject* pCrystal = instance->GetGameObject(m_aNovosCrystalInfo[i].m_crystalGuid);
        if (!pCrystal)
            continue;

        for (GuidList::iterator itr = m_lNovosDummyGuids.begin(); itr != m_lNovosDummyGuids.end();)
        {
            Creature* pDummy = instance->GetCreature(*itr);
            if (!pDummy)
            {
                m_lNovosDummyGuids.erase(itr++);
                continue;
            }

            // Check if dummy fits to crystal
            if (pCrystal->IsWithinDistInMap(pDummy, INTERACTION_DISTANCE, false))
            {
                m_aNovosCrystalInfo[i].m_channelGuid = pDummy->GetObjectGuid();
                m_lNovosDummyGuids.erase(itr);
                break;
            }

            ++itr;
        }
    }

    // Find the crystal channel target (above Novos)
    float fNovosX, fNovosY, fNovosZ;
    pNovos->GetRespawnCoord(fNovosX, fNovosY, fNovosZ);
    for (GuidList::iterator itr = m_lNovosDummyGuids.begin(); itr != m_lNovosDummyGuids.end();)
    {
        Creature* pDummy = instance->GetCreature(*itr);
        if (!pDummy)
        {
            m_lNovosDummyGuids.erase(itr++);
            continue;
        }

        // As the wanted dummy is exactly above Novos, check small range, and only 2d
        if (pDummy->IsWithinDist2d(fNovosX, fNovosY, 5.0f))
        {
            m_novosChannelGuid = pDummy->GetObjectGuid();
            m_lNovosDummyGuids.erase(itr);
            break;
        }

        ++itr;
    }

    // Summon positions (at end of stairs)
    for (GuidList::iterator itr = m_lNovosDummyGuids.begin(); itr != m_lNovosDummyGuids.end();)
    {
        Creature* pDummy = instance->GetCreature(*itr);
        if (!pDummy)
        {
            m_lNovosDummyGuids.erase(itr++);
            continue;
        }

        // The wanted dummies are quite above Novos
        if (pDummy->GetPositionZ() > fNovosZ + 20.0f)
        {
            m_vSummonDummyGuids.push_back(pDummy->GetObjectGuid());
            m_lNovosDummyGuids.erase(itr++);
        }
        else
            ++itr;
    }

    // Clear remaining (unused) dummies
    m_lNovosDummyGuids.clear();
}

Creature* instance_draktharon_keep::GetNextCrystalTarget(Creature* pCrystalHandler, uint8& uiIndex)
{
    Creature* pTarget = NULL;
    uiIndex = 0;

    for (uint8 i = 0; i < MAX_CRYSTALS; ++i)
    {
        Creature* pDummy = instance->GetCreature(m_aNovosCrystalInfo[i].m_channelGuid);
        // Return the nearest 'unused' crystal dummy
        // unused means, that the crystal was not already used, and the dummy-npc doesn't have the aura that will trigger the use on remove
        if (pDummy && !m_aNovosCrystalInfo[i].m_bWasUsed && (!pTarget || pCrystalHandler->GetDistanceOrder(pDummy, pTarget)) && !pDummy->HasAura(aCrystalHandlerDeathSpells[i]))
        {
            pTarget = pDummy;
            uiIndex = i;
        }
    }

    return pTarget;
}

void instance_draktharon_keep::DoHandleCrystal(uint8 uiIndex)
{
    m_aNovosCrystalInfo[uiIndex].m_bWasUsed = true;

    DoUseDoorOrButton(m_aNovosCrystalInfo[uiIndex].m_crystalGuid);

    if (Creature* pDummy = instance->GetCreature(m_aNovosCrystalInfo[uiIndex].m_channelGuid))
        pDummy->InterruptNonMeleeSpells(false);
}

Creature* instance_draktharon_keep::GetSummonDummy()
{
    if (m_vSummonDummyGuids.empty())
        return NULL;

    return instance->GetCreature(m_vSummonDummyGuids[urand(0, m_vSummonDummyGuids.size() - 1)]);
}

bool instance_draktharon_keep::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_BETTER_OFF_DREAD: return m_uiDreadAddsKilled >= 6;
        case ACHIEV_CRIT_OH_NOVOS:         return !m_bNovosAddGrounded;
        case ACHIEV_CRIT_CONSUME_JUNCTION: return m_bTrollgoreConsume;
        default:
            return false;
    }
}

void instance_draktharon_keep::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_TROLLGORE:
            if (uiData == IN_PROGRESS)
                m_bTrollgoreConsume = true;
            if (uiData == SPECIAL)
                m_bTrollgoreConsume = false;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_NOVOS:
            if (uiData == IN_PROGRESS)
            {
                // Sort the dummies
                DoSortNovosDummies();

                // Cast some visual spells
                Creature* pTarget = instance->GetCreature(m_novosChannelGuid);
                for (uint8 i = 0; i < MAX_CRYSTALS; ++i)
                {
                    Creature* pCaster = instance->GetCreature(m_aNovosCrystalInfo[i].m_channelGuid);
                    if (pCaster && pTarget)
                        pCaster->CastSpell(pTarget, SPELL_BEAM_CHANNEL, false);

                    m_aNovosCrystalInfo[i].m_bWasUsed = false;
                }

                // Achievement related
                m_bNovosAddGrounded = false;
            }
            else if (uiData == SPECIAL)
            {
                // Achievement related
                m_bNovosAddGrounded = true;
            }
            else if (uiData == FAIL)
            {
                // Interrupt casted spells
                for (uint8 i = 0; i < MAX_CRYSTALS; ++i)
                {
                    Creature* pDummy = instance->GetCreature(m_aNovosCrystalInfo[i].m_channelGuid);
                    if (pDummy)
                        pDummy->InterruptNonMeleeSpells(false);
                    // And reset used crystals
                    if (m_aNovosCrystalInfo[i].m_bWasUsed)
                        DoUseDoorOrButton(m_aNovosCrystalInfo[i].m_crystalGuid);
                }
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_KING_DRED:
            if (uiData == IN_PROGRESS)
                m_uiDreadAddsKilled = 0;
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_THARONJA:
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

void instance_draktharon_keep::Load(const char* chrIn)
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

uint32 instance_draktharon_keep::GetData(uint32 uiType) const
{
    switch (uiType)
    {
        case TYPE_TROLLGORE: return m_auiEncounter[uiType];
        case TYPE_NOVOS:     return m_auiEncounter[uiType];
        case TYPE_KING_DRED: return m_auiEncounter[uiType];
        case TYPE_THARONJA:  return m_auiEncounter[uiType];
        default:
            return 0;
    }
}

InstanceData* GetInstanceData_instance_draktharon_keep(Map* pMap)
{
    return new instance_draktharon_keep(pMap);
}

void AddSC_instance_draktharon_keep()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_draktharon_keep";
    pNewScript->GetInstanceData = &GetInstanceData_instance_draktharon_keep;
    pNewScript->RegisterSelf();
}
