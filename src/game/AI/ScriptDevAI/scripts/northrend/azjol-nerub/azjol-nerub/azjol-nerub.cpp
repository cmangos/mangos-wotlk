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
SDName: Instance_Azjol-Nerub
SD%Complete: 90
SDComment: Supports Krik'thir watcher event, Hadronox gauntlet event and Anub'arak submerged phase mobs
SDCategory: Azjol-Nerub
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "azjol-nerub.h"

instance_azjol_nerub::instance_azjol_nerub(Map* pMap) : ScriptedInstance(pMap),
    m_uiWatcherTimer(0),
    m_uiPeriodicAuraTimer(0),
    m_uiPeriodicAuraStage(0),
    m_uiSpiderSummonTimer(0),
    m_bWatchHimDie(true),
    m_bHadronoxDenied(true)
{
    Initialize();
}

void instance_azjol_nerub::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_azjol_nerub::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOOR_KRIKTHIR:
            if (m_auiEncounter[TYPE_KRIKTHIR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DOOR_ANUBARAK_1:
        case GO_DOOR_ANUBARAK_2:
        case GO_DOOR_ANUBARAK_3:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_azjol_nerub::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KRIKTHIR:
        case NPC_GASHRA:
        case NPC_NARJIL:
        case NPC_SILTHIK:
        case NPC_HADRONOX:
        case NPC_ANUBARAK:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_WORLD_TRIGGER:
            // sort triggers based on position
            if (pCreature->GetPositionZ() < 150.0f)
                m_darterSummonTarget = pCreature->GetObjectGuid();
            else if (pCreature->GetPositionZ() < 230.0f)
                m_anubSummonTarget = pCreature->GetObjectGuid();
            else if (pCreature->GetPositionZ() < 245.0f && pCreature->GetPositionY() < 360.0f)
                m_guardianSummonTarget = pCreature->GetObjectGuid();
            else if (pCreature->GetPositionZ() > 270.0f && pCreature->GetPositionZ() < 285.0f && pCreature->GetPositionY() < 410.0f)
                m_venomancerSummonTarget = pCreature->GetObjectGuid();
            break;
        case NPC_ANUBAR_CRUSHER:
            m_lCrusherGuids.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER_LARGE:
            if (pCreature->IsTemporarySummon())
                return;

            m_lSpiderTriggersGuids.push_back(pCreature->GetObjectGuid());

            // sort the triggers
            if (pCreature->GetPositionZ() > 750.0f)
            {
                if (pCreature->GetPositionX() < 500.0f)
                    m_rightTargetTriggerGuid = pCreature->GetObjectGuid();
                else
                    m_leftTargetTriggerGuid = pCreature->GetObjectGuid();
            }
            break;
    }
}

void instance_azjol_nerub::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following have passive behavior movement
        case NPC_WORLD_TRIGGER_LARGE:
        case NPC_WORLD_TRIGGER:
        case NPC_IMPALE_TARGET:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
        // following creatures have WP movement
        case NPC_ANUBAR_CHAMPION_1:
        case NPC_ANUBAR_CRYPT_FIEND_1:
        case NPC_ANUBAR_NECROMANCER_1:
        case NPC_ANUBAR_GUARDIAN:
        case NPC_ANUBAR_VENOMANCER:
            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MoveWaypoint();
            break;
        // following creature has levitate and WP movement
        case NPC_ANUBAR_DARTER:
            pCreature->SetLevitate(true);
            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MoveWaypoint();
            break;
        // following creatures have WP movement with variable path
        case NPC_ANUBAR_CHAMPION_2:
        case NPC_ANUBAR_CRYPT_FIEND_2:
        case NPC_ANUBAR_NECROMANCER_2:
            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MoveWaypoint(pCreature->GetPositionX() < 500.0f ? 0 : 1);
            break;
        // following creatures attack the player directly
        case NPC_ANUBAR_CHAMPION_3:
        case NPC_ANUBAR_CRYPT_FIEND_3:
        case NPC_ANUBAR_NECROMANCER_3:
            pCreature->CastSpell(pCreature, SPELL_CHECK_RESET, TRIGGERED_OLD_TRIGGERED);

            if (Player* pTarget = instance->GetPlayer(m_playerGuid))
            {
                if (pTarget->IsAlive())
                    pCreature->AI()->AttackStart(pTarget);
            }
            else if (Player* pPlayer = GetPlayerInMap(true))
                pCreature->AI()->AttackStart(pPlayer);
            else if (Creature* pHadronox = GetSingleCreatureFromStorage(NPC_HADRONOX))
                pCreature->AI()->AttackStart(pHadronox);
            break;
        // following creatures have jump movement
        case NPC_ANUBAR_ASSASSIN:
            if (Creature* pTrigger = instance->GetCreature(m_anubSummonTarget))
            {
                float fX, fY, fZ;
                pCreature->GetRandomPoint(pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 15.0f, fX, fY, fZ);
                pCreature->GetMotionMaster()->MoveJump(fX, fY, fZ, 3 * pCreature->GetSpeed(MOVE_RUN), 10.0f);
            }
            break;
    }
}

void instance_azjol_nerub::OnCreatureDeath(Creature* pCreature)
{
    uint32 uiEntry = pCreature->GetEntry();
    if (uiEntry == NPC_GASHRA || uiEntry == NPC_NARJIL || uiEntry == NPC_SILTHIK)
    {
        if (m_auiEncounter[TYPE_KRIKTHIR] == NOT_STARTED)
            m_uiWatcherTimer = 5000;

        // Set achiev criteriat to false if one of the watchers dies
        m_bWatchHimDie = false;
    }
    else if (uiEntry == NPC_ANUBAR_CRUSHER)
    {
        m_lCrusherGuids.remove(pCreature->GetObjectGuid());

        // make boss passive to ignore mobs and speed up the tunnel movement
        if (m_lCrusherGuids.empty())
        {
            if (Creature* pHadronox = GetSingleCreatureFromStorage(NPC_HADRONOX))
            {
                pHadronox->AI()->EnterEvadeMode();
                pHadronox->AI()->SetReactState(REACT_PASSIVE);

                pHadronox->SetWalk(false);
                pHadronox->GetMotionMaster()->Clear(false, true);
                pHadronox->GetMotionMaster()->MoveWaypoint();
            }
        }
    }
}

void instance_azjol_nerub::OnCreatureEnterCombat(Creature* pCreature)
{
    uint32 uiEntry = pCreature->GetEntry();

    if (uiEntry == NPC_GASHRA || uiEntry == NPC_NARJIL || uiEntry == NPC_SILTHIK)
    {
        // Creature enter combat is not equal to having a victim yet.
        if (!m_playerGuid && pCreature->GetVictim())
            m_playerGuid = pCreature->GetVictim()->GetBeneficiaryPlayer()->GetObjectGuid();
    }
    else if (uiEntry == NPC_ANUBAR_CRUSHER)
    {
        // there is only one Crusher which is spawned in DB; the script reacts only to that one
        if (pCreature->IsTemporarySummon())
            return;

        // Only for the first try
        if (GetData(TYPE_HADRONOX) == IN_PROGRESS)
            return;

        DoScriptText(SAY_CRUSHER_AGGRO, pCreature);

        if (pCreature->GetVictim())
            m_playerGuid = pCreature->GetVictim()->GetBeneficiaryPlayer()->GetObjectGuid();

        // spawn and store additional triggers for player attack
        for (uint8 i = 0; i < MAX_GAUNTLET_SPELLS; ++i)
            if (Creature* pTrigger = pCreature->SummonCreature(triggerSummonData[i].entry, triggerSummonData[i].x, triggerSummonData[i].y, triggerSummonData[i].z, triggerSummonData[i].o, TEMPSPAWN_DEAD_DESPAWN, 0))
                m_playerTriggerGuid[i] = pTrigger->GetObjectGuid();

        // Spawn 6 extra spawns; movement in DB
        for (uint8 i = 0; i < MAX_GAUNTLET_SPAWNS; ++i)
        {
            if (Creature* pMob = pCreature->SummonCreature(gauntletSummonData[i].entry, gauntletSummonData[i].x, gauntletSummonData[i].y, gauntletSummonData[i].z, gauntletSummonData[i].o, TEMPSPAWN_DEAD_DESPAWN, 0))
            {
                pMob->SetWalk(false);
                pMob->GetMotionMaster()->MoveWaypoint(gauntletSummonData[i].pathId);
            }
        }

        // start gauntlet; spider moves up the tunnel
        if (Creature* pHadronox = GetSingleCreatureFromStorage(NPC_HADRONOX))
            DoScriptText(EMOTE_MOVE_TUNNEL, pHadronox);

        SetData(TYPE_HADRONOX, IN_PROGRESS);

        m_uiSpiderSummonTimer = 20000;
    }
}

void instance_azjol_nerub::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_GASHRA:
        case NPC_NARJIL:
        case NPC_SILTHIK:
            m_playerGuid.Clear();
            break;
        case NPC_ANUBAR_GUARDIAN:
        case NPC_ANUBAR_VENOMANCER:
        case NPC_ANUBAR_ASSASSIN:
        case NPC_ANUBAR_DARTER:
            if (Creature* pAnub = GetSingleCreatureFromStorage(NPC_ANUBARAK))
                pAnub->AI()->EnterEvadeMode();
            break;
    }
}

void instance_azjol_nerub::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_KRIKTHIR:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_DOOR_KRIKTHIR);

                // start gauntlet event
                m_uiPeriodicAuraTimer = 1000;
                m_uiPeriodicAuraStage = 0;
            }
            break;
        case TYPE_HADRONOX:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE || uiData == SPECIAL)
                ResetHadronoxTriggers();
            break;
        case TYPE_ANUBARAK:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_DOOR_ANUBARAK_1);
            DoUseDoorOrButton(GO_DOOR_ANUBARAK_2);
            DoUseDoorOrButton(GO_DOOR_ANUBARAK_3);
            if (uiData == IN_PROGRESS)
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_ANUB_ID);
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_azjol_nerub::Load(const char* chrIn)
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

uint32 instance_azjol_nerub::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

bool instance_azjol_nerub::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1*/ /* = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRITERIA_WATCH_DIE:
            return m_bWatchHimDie;
        case ACHIEV_CRITERIA_DENIED:
            return m_bHadronoxDenied;

        default:
            return false;
    }
}

// Method that will send watchers to engange in combat for Krikthir
void instance_azjol_nerub::DoSendWatcherOrKrikthir()
{
    Creature* pAttacker = nullptr;
    Creature* pKrikthir = GetSingleCreatureFromStorage(NPC_KRIKTHIR);

    if (!pKrikthir)
        return;

    for (unsigned int aWatcher : aWatchers)
    {
        if (Creature* pTemp = GetSingleCreatureFromStorage(aWatcher))
        {
            if (pTemp->IsAlive())
            {
                if (pAttacker && urand(0, 1))
                    continue;
                pAttacker = pTemp;
            }
        }
    }

    if (pAttacker)
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SEND_GROUP_1, pKrikthir); break;
            case 1: DoScriptText(SAY_SEND_GROUP_2, pKrikthir); break;
            case 2: DoScriptText(SAY_SEND_GROUP_3, pKrikthir); break;
        }
    }
    else
        pAttacker = pKrikthir;

    if (Player* pTarget = instance->GetPlayer(m_playerGuid))
    {
        if (pTarget->IsAlive())
            pAttacker->AI()->AttackStart(pTarget);
    }
}

// Method to reset the web triggers that spawn the gauntlet mobs
void instance_azjol_nerub::ResetHadronoxTriggers()
{
    // Drop the summon auras from the triggers
    for (const auto& guid : m_lSpiderTriggersGuids)
    {
        if (Creature* pTrigger = instance->GetCreature(guid))
        {
            pTrigger->RemoveAurasDueToSpell(SPELL_SUMMON_CHAMPION_S);
            pTrigger->RemoveAurasDueToSpell(SPELL_SUMMON_NECROMANCER_S);
            pTrigger->RemoveAurasDueToSpell(SPELL_SUMMON_CRYPT_FIEND_S);
        }
    }

    // despawn temp triggers
    for (uint8 i = 0; i < MAX_GAUNTLET_SPELLS; ++i)
        if (Creature* pTrigger = instance->GetCreature(m_playerTriggerGuid[i]))
            pTrigger->ForcedDespawn();

    // stop the timer
    m_uiSpiderSummonTimer = 0;
}

void instance_azjol_nerub::Update(uint32 uiDiff)
{
    // timer to engange watchers for Krikthir
    if (m_uiWatcherTimer)
    {
        if (m_uiWatcherTimer <= uiDiff)
        {
            DoSendWatcherOrKrikthir();
            m_uiWatcherTimer = 0;
        }
        else
            m_uiWatcherTimer -= uiDiff;
    }

    // apply period auras on the triggers
    if (m_uiPeriodicAuraTimer)
    {
        if (m_uiPeriodicAuraTimer <= uiDiff)
        {
            for (const auto& guid : m_lSpiderTriggersGuids)
                if (Creature* pTrigger = instance->GetCreature(guid))
                    pTrigger->CastSpell(pTrigger, aSpiderSummonSpells[m_uiPeriodicAuraStage], TRIGGERED_OLD_TRIGGERED);

            ++m_uiPeriodicAuraStage;
            m_uiPeriodicAuraTimer = m_uiPeriodicAuraStage == 2 ? 0 : 5000;
        }
        else
            m_uiPeriodicAuraTimer -= uiDiff;
    }

    // spider summon timer
    if (m_uiSpiderSummonTimer)
    {
        if (m_uiSpiderSummonTimer <= uiDiff)
        {
            for (uint8 i = 0; i < MAX_GAUNTLET_SPELLS; ++i)
                if (Creature* pTrigger = instance->GetCreature(m_playerTriggerGuid[i]))
                    pTrigger->CastSpell(pTrigger, aPlayerSummonSpells[i], TRIGGERED_OLD_TRIGGERED);

            m_uiSpiderSummonTimer = 60000;
        }
        else
            m_uiSpiderSummonTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_azjol_nerub(Map* pMap)
{
    return new instance_azjol_nerub(pMap);
}

void AddSC_instance_azjol_nerub()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_azjol-nerub";
    pNewScript->GetInstanceData = &GetInstanceData_instance_azjol_nerub;
    pNewScript->RegisterSelf();
}
