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
SDName: Instance_Dark_Portal
SD%Complete: 75
SDComment: Quest support: 9836, 10297. Some visuals for the event are missing;
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "dark_portal.h"

instance_dark_portal::instance_dark_portal(Map* pMap) : ScriptedInstance(pMap),
    m_uiWorldState(0),
    m_uiWorldStateRiftCount(0),
    m_uiWorldStateShieldCount(100),
    m_uiSummonedCrystalCount(0),
    m_bHasIntroYelled(false),
    m_uiMedivhYellCount(1),
    m_uiNextPortalTimer(0),
    m_uiSummonCrystalTimer(0),
    m_uiSummonBeamStalkerTimer(0),
    m_uiCurrentRiftId(0),
    m_uiPostEventTimer(0),
    m_uiPostEventStep(0)
{
    Initialize();
}

void instance_dark_portal::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_dark_portal::DoResetEvent()
{
    UpdateWorldState(false);

    m_uiWorldStateShieldCount = 100;
    m_uiWorldStateRiftCount   = 0;
    m_uiSummonedCrystalCount = 0;

    m_uiCurrentRiftId    = 0;
    m_uiNextPortalTimer  = 0;
    m_uiMedivhYellCount  = 1;
    m_uiSummonCrystalTimer = 0;
    m_uiSummonBeamStalkerTimer = 0;
}

void instance_dark_portal::UpdateWorldState(bool bEnable)
{
    m_uiWorldState = bEnable ? 1 : 0;

    DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_IS_ACTIVE,     m_uiWorldState);
    DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_SHIELD_STATE, m_uiWorldStateShieldCount);
    DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_RIFT_STATE,   m_uiWorldStateRiftCount);
}

void instance_dark_portal::OnPlayerEnter(Player* /*pPlayer*/)
{
    UpdateWorldState(m_auiEncounter[TYPE_MEDIVH] == IN_PROGRESS);
}

void instance_dark_portal::DoHandleAreaTrigger(uint32 uiTriggerId)
{
    if (uiTriggerId == AREATRIGGER_ENTER)
    {
        // Yell at instance entrance
        if (!m_bHasIntroYelled)
        {
            if (Creature* pSaat = GetSingleCreatureFromStorage(NPC_SAAT))
                DoScriptText(SAY_SAAT_WELCOME, pSaat);
            m_bHasIntroYelled = true;
        }
    }
    else if (uiTriggerId == AREATRIGGER_MEDIVH)
    {
        // Start Dark Portal event
        if (GetData(TYPE_MEDIVH) == NOT_STARTED || GetData(TYPE_MEDIVH) == FAIL)
            SetData(TYPE_MEDIVH, IN_PROGRESS);
    }
}

void instance_dark_portal::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MEDIVH:
        case NPC_SAAT:
        case NPC_DARK_PORTAL_DUMMY:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_dark_portal::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DARK_PORTAL:
            m_goEntryGuidStore[GO_DARK_PORTAL] = pGo->GetObjectGuid();
            break;
    }
}

void instance_dark_portal::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_MEDIVH:
        {
            if (uiData == IN_PROGRESS)
            {
                if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
                {
                    if (pMedivh->IsAlive())
                    {
                        DoScriptText(SAY_MEDIVH_ENTER, pMedivh);

                        if (Creature* pSaat = GetSingleCreatureFromStorage(NPC_SAAT)) // Sa'at is not a questgiver while event is in progress
                            pSaat->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    }
                    // If Medivh is not available the do not store the uiData;
                    else
                        return;
                }

                UpdateWorldState();
                m_uiNextPortalTimer = 3000;
                m_uiSummonCrystalTimer = 1000;
                m_uiSummonBeamStalkerTimer = 1000;
            }
            if (uiData == DONE)
            {
                StartPostEvent();

                if (Creature* pSaat = GetSingleCreatureFromStorage(NPC_SAAT))
                    pSaat->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            }
            if (uiData == FAIL)
            {
                DoResetEvent();

                if (Creature* pSaat = GetSingleCreatureFromStorage(NPC_SAAT))
                    pSaat->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        }
        case TYPE_SHIELD:
            if (uiData == SPECIAL)
            {
                --m_uiWorldStateShieldCount;
                DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_SHIELD_STATE, m_uiWorldStateShieldCount);

                // Yell at 75%, 50% and 25% shield
                if (m_uiWorldStateShieldCount < 100 - 25 * m_uiMedivhYellCount)
                {
                    if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
                    {
                        DoScriptText(uiMedivhWeakYell[m_uiMedivhYellCount - 1], pMedivh);
                        ++m_uiMedivhYellCount;
                    }

                    if (Creature* pDarkPortalDummy = GetSingleCreatureFromStorage(NPC_DARK_PORTAL_DUMMY))
                        pDarkPortalDummy->CastSpell(pDarkPortalDummy, SPELL_CRYSTAL_SHATTER, TRIGGERED_OLD_TRIGGERED);
                }

                // Kill the npc when the shield is broken
                if (!m_uiWorldStateShieldCount)
                {
                    if (Creature* medivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
                    {
                        if (medivh->IsAlive())
                            medivh->Suicide();
                    }
                }
            }
            m_auiEncounter[uiType] = uiData;
            return;
        case TYPE_TIME_RIFT:
        {
            // Set the delay to the next time rift from the point the rift despawns
            if (uiData == DONE)
                m_uiNextPortalTimer = IsBossTimeRift() ? 125000 : 3000;
            // Set the delay to the next time rift from the point the rift summons it's guardian
            // ToDo: research if these timers are correct
            else if (uiData == SPECIAL)
                m_uiNextPortalTimer = IsBossTimeRift() ? 0 : m_uiWorldStateRiftCount > 12 ? 90000 : 2 * MINUTE * IN_MILLISECONDS;

            m_auiEncounter[uiType] = uiData;
            return;
        }
        case TYPE_CHRONO_LORD:
        case TYPE_TEMPORUS:
            if (m_auiEncounter[uiType] != DONE)             // Keep the DONE-information stored
                m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_AEONUS:
            if (uiData == DONE)
            {
                UpdateWorldState(false);
                SetData(TYPE_MEDIVH, DONE); // Start Post Event
            }
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
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

uint32 instance_dark_portal::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_dark_portal::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_dark_portal::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHRONO_LORD_DEJA:
        case NPC_CHRONO_LORD:
            SetData(TYPE_CHRONO_LORD, IN_PROGRESS);
            break;
        case NPC_TEMPORUS:
        case NPC_TIMEREAVER:
            SetData(TYPE_TEMPORUS, IN_PROGRESS);
            break;
        case NPC_AEONUS:
            SetData(TYPE_AEONUS, IN_PROGRESS);
        // no break
        case NPC_ASSASSIN:
        case NPC_ASSASSIN_2:
        case NPC_WHELP:
        case NPC_CHRONOMANCER:
        case NPC_CHRONOMANCER_2:
        case NPC_EXECUTIONER:
        case NPC_EXECUTIONER_2:
        case NPC_VANQUISHER:
        case NPC_VANQUISHER_2:
            pCreature->InterruptNonMeleeSpells(false);
            break;
    }
}

void instance_dark_portal::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHRONO_LORD_DEJA:
        case NPC_CHRONO_LORD:
            SetData(TYPE_CHRONO_LORD, FAIL);
            break;
        case NPC_TEMPORUS:
        case NPC_TIMEREAVER:
            SetData(TYPE_TEMPORUS, FAIL);
            break;
        case NPC_AEONUS:
            SetData(TYPE_AEONUS, FAIL);
        // no break;
        // Allow these guys to go and finish off Medivh
        case NPC_ASSASSIN:
        case NPC_ASSASSIN_2:
        case NPC_WHELP:
        case NPC_CHRONOMANCER:
        case NPC_CHRONOMANCER_2:
        case NPC_EXECUTIONER:
        case NPC_EXECUTIONER_2:
        case NPC_VANQUISHER:
        case NPC_VANQUISHER_2:
            if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
            {
                float fX, fY, fZ;
                pMedivh->GetNearPoint(pMedivh, fX, fY, fZ, 0, 20.0f, pMedivh->GetAngle(pCreature));
                pCreature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
            }
            break;
    }
}

void instance_dark_portal::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHRONO_LORD_DEJA:
        case NPC_CHRONO_LORD:
            SetData(TYPE_CHRONO_LORD, DONE);
            break;
        case NPC_TEMPORUS:
        case NPC_TIMEREAVER:
            SetData(TYPE_TEMPORUS, DONE);
            break;
        case NPC_AEONUS:
            SetData(TYPE_AEONUS, DONE);
            break;
    }
}

void instance_dark_portal::DoSpawnNextPortal()
{
    if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
    {
        // Randomize portal locations
        uint8 uiTmp = urand(0, 2);

        if (uiTmp >= m_uiCurrentRiftId)
            ++uiTmp;

        debug_log("SD2: instance_dark_portal: SetRiftId %u, old was id %u.", uiTmp, m_uiCurrentRiftId);

        m_uiCurrentRiftId = uiTmp;

        // Summon next portal
        pMedivh->SummonCreature(NPC_TIME_RIFT, afPortalLocation[uiTmp].fX, afPortalLocation[uiTmp].fY, afPortalLocation[uiTmp].fZ, afPortalLocation[uiTmp].fOrient, TEMPSPAWN_CORPSE_DESPAWN, 0);
    }
}

void instance_dark_portal::StartPostEvent()
{
    if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        pMedivh->InterruptNonMeleeSpells(false);
    m_uiPostEventStep = POST_EVENT_1_MEDIVH_YELL;
    m_uiPostEventTimer = 10000;

    // Quests are completed instantly on Aeonus death, but can't be turned in until Medivh gets Quest NPCFlag
    Map::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
            {
                if (pPlayer->GetQuestStatus(QUEST_OPENING_PORTAL) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->AreaExploredOrEventHappens(QUEST_OPENING_PORTAL);

                if (pPlayer->GetQuestStatus(QUEST_MASTER_TOUCH) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->AreaExploredOrEventHappens(QUEST_MASTER_TOUCH);
            }
        }
    }
}

void instance_dark_portal::ExecutePostEventStep()
{
    switch (m_uiPostEventStep)
    {
    case POST_EVENT_1_MEDIVH_YELL:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            DoScriptText(SAY_MEDIVH_WIN, pMedivh);
            pMedivh->SetFacingTo(6.265732f);
            pMedivh->HandleEmoteState(378);
        }
        DoRespawnGameObject(GO_DARK_PORTAL, WEEK);
        m_uiPostEventTimer = 16700;
        m_uiPostEventStep++;
        break;
    }
    case POST_EVENT_2_MEDIVH_NPCFLAGS:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            pMedivh->SetFacingTo(3.001966f);
            pMedivh->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            pMedivh->HandleEmoteState(0);
        }
        m_uiPostEventTimer = 800;
        m_uiPostEventStep++;
        break;
    }
    case POST_EVENT_3_SUMMON_ORCS_1:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.848f, 7132.722f, 34.67148f, 6.126106f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 1);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.195f, 7129.969f, 34.67148f, 6.143559f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 2);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.48f, 7127.273f, 34.67148f, 6.161012f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 3);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.801f, 7124.51f, 34.67148f, 6.178465f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 4);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.082f, 7121.888f, 34.67148f, 6.195919f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 5);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.386f, 7119.379f, 34.67148f, 6.213372f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 6);
        }
        m_uiPostEventTimer = 2000;
        m_uiPostEventStep++;
        break;
    }
    case POST_EVENT_4_SUMMON_ORCS_2:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.572f, 7132.635f, 34.67148f, 6.126106f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 7);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.985f, 7129.968f, 34.67148f, 6.143559f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 8);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.409f, 7127.203f, 34.67148f, 6.161012f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 9);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.883f, 7124.376f, 34.67148f, 6.178465f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 10);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.225f, 7121.99f, 34.67147f, 6.195919f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 11);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.599f, 7119.243f, 34.67148f, 6.213372f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 12);
        }
        m_uiPostEventTimer = 2400;
        m_uiPostEventStep++;
        break;
    }
    case POST_EVENT_5_SUMMON_ORCS_3:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.509f, 7132.678f, 34.67148f, 6.126106f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 13);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.954f, 7129.987f, 34.67148f, 6.143559f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 14);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.31f, 7127.19f, 34.67148f, 6.161012f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 15);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.646f, 7124.358f, 34.67148f, 6.178465f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 16);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.013f, 7121.874f, 34.67148f, 6.195919f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 17);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.501f, 7119.221f, 34.67148f, 6.213372f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 18);
        }
        m_uiPostEventTimer = 2400;
        m_uiPostEventStep++;
        break;
    }
    case POST_EVENT_6_SUMMON_ORCS_4:
    {
        if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
        {
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.459f, 7132.609f, 34.67148f, 6.149603f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 19);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2086.878f, 7129.967f, 34.67148f, 6.097349f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 20);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.149f, 7127.205f, 34.67148f, 6.089332f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 21);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.488f, 7124.414f, 34.67148f, 6.143568f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 22);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2087.892f, 7121.918f, 34.67148f, 6.139006f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 23);
            pMedivh->SummonCreature(NPC_COUNCIL_ENFORCER, -2088.388f, 7119.204f, 34.67148f, 6.178112f, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, 24);
        }
        m_uiPostEventTimer = 0;
        m_uiPostEventStep = 0;
        break;
    }
    }
}

void instance_dark_portal::Update(uint32 uiDiff)
{
    if (m_uiPostEventTimer && GetData(TYPE_MEDIVH) == DONE)
    {
        if (m_uiPostEventTimer <= uiDiff)
        {
            ExecutePostEventStep();
        }
        else
            m_uiPostEventTimer -= uiDiff;
    }

    if (GetData(TYPE_MEDIVH) != IN_PROGRESS)
        return;

    if (m_uiNextPortalTimer)
    {
        if (m_uiNextPortalTimer <= uiDiff)
        {
            DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_RIFT_STATE, ++m_uiWorldStateRiftCount);

            DoSpawnNextPortal();
            m_uiNextPortalTimer = 0;
        }
        else
            m_uiNextPortalTimer -= uiDiff;
    }

    if (m_uiSummonCrystalTimer)
    {
        if (m_uiSummonCrystalTimer <= uiDiff)
        {
            if (m_uiSummonedCrystalCount < MAX_CRYSTALS)
            {
                if (Creature* pDarkPortalDummy = GetSingleCreatureFromStorage(NPC_DARK_PORTAL_DUMMY))
                {
                    pDarkPortalDummy->CastSpell(pDarkPortalDummy, SPELL_PORTAL_CRYSTAL, TRIGGERED_OLD_TRIGGERED);
                    m_uiSummonedCrystalCount++;
                    m_uiSummonCrystalTimer = 5000;
                }
            }
            else
                m_uiSummonCrystalTimer = 0;
        }
        else
            m_uiSummonCrystalTimer -= uiDiff;
    }

    if (m_uiSummonBeamStalkerTimer)
    {
        if (m_uiSummonBeamStalkerTimer <= uiDiff)
        {
            if (Creature* pDarkPortalDummy = GetSingleCreatureFromStorage(NPC_DARK_PORTAL_DUMMY))
            {
                pDarkPortalDummy->CastSpell(pDarkPortalDummy, SPELL_PORTAL_BEAM, TRIGGERED_OLD_TRIGGERED);
                m_uiSummonBeamStalkerTimer = urand(1000, 2000);
            }
        }
        else
            m_uiSummonBeamStalkerTimer -= uiDiff;
    }
}

bool instance_dark_portal::CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const
{
    switch (uiInstanceConditionId)
    {
        case INSTANCE_CONDITION_ID_SAAT_BEACON: // Event not in progress and Medivh is spawned and alive
        {
            if (GetData(TYPE_MEDIVH) == IN_PROGRESS)
                return false;

            if (Creature* pMedivh = GetSingleCreatureFromStorage(NPC_MEDIVH))
            {
                if (pMedivh->IsAlive())
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
    }

    script_error_log("instance_dark_portal::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
        uiInstanceConditionId, pPlayer ? pPlayer->GetGuidStr().c_str() : "nullptr", pConditionSource ? pConditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
    return false;
}


InstanceData* GetInstanceData_instance_dark_portal(Map* pMap)
{
    return new instance_dark_portal(pMap);
}

bool AreaTrigger_at_dark_portal(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_MEDIVH || pAt->id == AREATRIGGER_ENTER)
    {
        if (pPlayer->IsGameMaster() || pPlayer->IsDead())
            return false;

        if (instance_dark_portal* pInstance = (instance_dark_portal*)pPlayer->GetInstanceData())
            pInstance->DoHandleAreaTrigger(pAt->id);
    }

    return false;
}

void instance_dark_portal::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands: setrift");
}

void instance_dark_portal::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "setrift")
    {
        uint32 riftId;
        handler->ExtractUInt32(&args, riftId);

        if (riftId > 18 || riftId < 1)
        {
            handler->PSendSysMessage("Could not spawn rift %u because it doesn't exist. Please specify a value 1-18", riftId);
            return;
        }

        m_uiWorldStateRiftCount = riftId;
        DoUpdateWorldState(WORLD_STATE_OPENING_THE_DARK_PORTAL_RIFT_STATE, riftId);
        DoSpawnNextPortal();
    }
}

void AddSC_instance_dark_portal()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_dark_portal";
    pNewScript->GetInstanceData = &GetInstanceData_instance_dark_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_dark_portal";
    pNewScript->pAreaTrigger = &AreaTrigger_at_dark_portal;
    pNewScript->RegisterSelf();
}
