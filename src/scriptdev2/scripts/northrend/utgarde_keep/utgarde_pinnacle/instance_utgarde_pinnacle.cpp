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
SDName: instance_pinnacle
SD%Complete: 75%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

instance_pinnacle::instance_pinnacle(Map* pMap) : ScriptedInstance(pMap),
    m_uiGortokOrbTimer(0),
    m_uiGortokOrbPhase(0)
{
    Initialize();
}

void instance_pinnacle::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for (uint8 i = 0; i < MAX_SPECIAL_ACHIEV_CRITS; ++i)
        m_abAchievCriteria[i] = false;
}

void instance_pinnacle::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FURBOLG:
        case NPC_WORGEN:
        case NPC_JORMUNGAR:
        case NPC_RHINO:
        case NPC_BJORN:
        case NPC_HALDOR:
        case NPC_RANULF:
        case NPC_TORGYN:
        case NPC_SKADI:
        case NPC_GRAUF:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_WORLD_TRIGGER:
            if (pCreature->GetPositionX() < 250.0f)
                m_gortokEventTriggerGuid = pCreature->GetObjectGuid();
            else if (pCreature->GetPositionX() > 400.0f && pCreature->GetPositionX() < 500.0f)
                m_skadiMobsTriggerGuid = pCreature->GetObjectGuid();
            break;
        case NPC_YMIRJAR_HARPOONER:
        case NPC_YMIRJAR_WARRIOR:
        case NPC_YMIRJAR_WITCH_DOCTOR:
            m_lskadiGauntletMobsList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_pinnacle::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOOR_SKADI:
            if (m_auiEncounter[TYPE_SKADI] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_YMIRON:
            if (m_auiEncounter[TYPE_YMIRON] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_pinnacle::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SVALA:
            if (uiData == IN_PROGRESS || uiData == FAIL)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_INCREDIBLE_HULK, false);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GORTOK:
            if (uiData == IN_PROGRESS)
            {
                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                {
                    pOrb->SetLevitate(true);
                    pOrb->CastSpell(pOrb, SPELL_ORB_VISUAL, true);
                    pOrb->GetMotionMaster()->MovePoint(0, aOrbPositions[0][0], aOrbPositions[0][1], aOrbPositions[0][2]);

                    m_uiGortokOrbTimer = 2000;
                }
            }
            else if (uiData == FAIL)
            {
                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                {
                    if (!pOrb->isAlive())
                        pOrb->Respawn();
                    else
                        pOrb->RemoveAllAuras();

                    // For some reasone the Orb doesn't evade automatically
                    pOrb->GetMotionMaster()->MoveTargetedHome();
                }

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    // Reset each miniboss
                    if (Creature* pTemp = GetSingleCreatureFromStorage(aGortokMiniBosses[i]))
                    {
                        if (!pTemp->isAlive())
                            pTemp->Respawn();

                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                m_uiGortokOrbPhase = 0;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SKADI:
            // Don't process the event twice
            if (m_auiEncounter[uiType] == uiData)
                return;
            switch (uiData)
            {
                case DONE:
                    DoUseDoorOrButton(GO_DOOR_SKADI);
                    break;
                case SPECIAL:
                    // Prepare achievements
                    SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, true);
                    DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_SKADI_ID);

                    m_auiEncounter[uiType] = uiData;
                    return;
                case FAIL:
                    // Handle Grauf evade - if event is in phase 1
                    if (Creature* pGrauf = GetSingleCreatureFromStorage(NPC_GRAUF))
                        pGrauf->AI()->EnterEvadeMode();

                    // no break;
                case NOT_STARTED:
                    // Despawn all summons
                    for (GuidList::const_iterator itr = m_lskadiGauntletMobsList.begin(); itr != m_lskadiGauntletMobsList.end(); ++itr)
                    {
                        if (Creature* pYmirjar = instance->GetCreature(*itr))
                            pYmirjar->ForcedDespawn();
                    }

                    // Reset position
                    if (Creature* pGrauf = GetSingleCreatureFromStorage(NPC_GRAUF))
                        pGrauf->GetMotionMaster()->MoveTargetedHome();

                    // no break;
                case IN_PROGRESS:

                    // Remove the summon aura on phase 2 or fail
                    if (Creature* pTrigger = instance->GetCreature(m_skadiMobsTriggerGuid))
                        pTrigger->RemoveAllAuras();
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_YMIRON:
            if (uiData == DONE)
                DoUseDoorOrButton(GO_DOOR_YMIRON);
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_KINGS_BANE, true);
            else if (uiData == SPECIAL)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_KINGS_BANE, false);
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            script_error_log("Instance Pinnacle: SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    // Saving also SPECIAL for this instance
    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_pinnacle::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_pinnacle::Load(const char* chrIn)
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

void instance_pinnacle::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_pinnacle::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_INCREDIBLE_HULK:
            return m_abAchievCriteria[TYPE_ACHIEV_INCREDIBLE_HULK];
        case ACHIEV_CRIT_GIRL_LOVES_SKADI:
            return m_abAchievCriteria[TYPE_ACHIEV_LOVE_SKADI];
        case ACHIEV_CRIT_KINGS_BANE:
            return m_abAchievCriteria[TYPE_ACHIEV_KINGS_BANE];

        default:
            return false;
    }
}

void instance_pinnacle::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FURBOLG:
        case NPC_WORGEN:
        case NPC_JORMUNGAR:
        case NPC_RHINO:
            SetData(TYPE_GORTOK, FAIL);
            break;
        case NPC_YMIRJAR_WARRIOR:
        case NPC_YMIRJAR_WITCH_DOCTOR:
        case NPC_YMIRJAR_HARPOONER:
            // Handle Skadi gauntlet reset. Used instead of using spell 49308
            SetData(TYPE_SKADI, FAIL);
            break;
    }
}

void instance_pinnacle::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_FURBOLG:
        case NPC_WORGEN:
        case NPC_JORMUNGAR:
        case NPC_RHINO:
            m_uiGortokOrbTimer = 3000;
            break;
    }
}

void instance_pinnacle::Update(uint32 const uiDiff)
{
    if (m_uiGortokOrbTimer)
    {
        if (m_uiGortokOrbTimer <= uiDiff)
        {
            if (!m_uiGortokOrbPhase)
            {
                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                    pOrb->GetMotionMaster()->MovePoint(0, aOrbPositions[1][0], aOrbPositions[1][1], aOrbPositions[1][2]);

                m_uiGortokOrbTimer = 18000;
            }
            // Awaken Gortok if this is the last phase
            else
            {
                uint8 uiMaxOrbPhase = instance->IsRegularDifficulty() ? 3 : 5;
                uint32 uiSpellId = m_uiGortokOrbPhase == uiMaxOrbPhase ? SPELL_AWAKEN_GORTOK : SPELL_AWAKEN_SUBBOSS;

                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                {
                    pOrb->CastSpell(pOrb, uiSpellId, false);

                    if (m_uiGortokOrbPhase == uiMaxOrbPhase)
                        pOrb->ForcedDespawn(10000);
                }

                m_uiGortokOrbTimer = 0;
            }
            ++m_uiGortokOrbPhase;
        }
        else
            m_uiGortokOrbTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

void AddSC_instance_pinnacle()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_pinnacle";
    pNewScript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    pNewScript->RegisterSelf();
}
