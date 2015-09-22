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
SDName: Instance_Violet_Hold
SD%Complete: 75
SDComment: Prison defense system requires more research
SDCategory: Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

instance_violet_hold::instance_violet_hold(Map* pMap) : ScriptedInstance(pMap),
    m_uiWorldState(0),
    m_uiWorldStateSealCount(100),
    m_uiWorldStatePortalCount(0),

    m_uiPortalId(0),
    m_uiPortalTimer(0),
    m_uiMaxCountPortalLoc(0),

    m_uiSealYellCount(0),
    m_uiEventResetTimer(0),

    m_bIsVoidDance(false),
    m_bIsDefenseless(false),
    m_bIsDehydratation(false)
{
    Initialize();
}

void instance_violet_hold::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_uiMaxCountPortalLoc = countof(afPortalLocation) - 1;
}

void instance_violet_hold::ResetVariables()
{
    m_uiWorldStateSealCount = 100;
    m_uiWorldStatePortalCount = 0;
    m_uiSealYellCount = 0;
}

void instance_violet_hold::ResetAll()
{
    ResetVariables();
    UpdateWorldState(false);
    CallGuards(true);
    SetIntroPortals(false);
    // ToDo: reset the activation crystals when implemented

    for (std::vector<BossSpawn*>::const_iterator itr = m_vRandomBosses.begin(); itr != m_vRandomBosses.end(); ++itr)
    {
        const BossInformation* pData = GetBossInformation((*itr)->uiEntry);
        if (pData && m_auiEncounter[pData->uiType] == DONE)
        {
            // Despawn ghost boss
            if (Creature* pGhostBoss = GetSingleCreatureFromStorage(pData->uiGhostEntry))
                pGhostBoss->ForcedDespawn();

            // Spawn new boss replacement
            if (Creature* pSummoner = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
                pSummoner->SummonCreature(pData->uiGhostEntry, (*itr)->fX, (*itr)->fY, (*itr)->fZ, (*itr)->fO, TEMPSUMMON_DEAD_DESPAWN, 0);

            // Replace Erekem guards
            if (pData->uiType == TYPE_EREKEM)
            {
                // Despawn ghost guards
                for (GuidList::const_iterator itr = m_lArakkoaGuardList.begin(); itr != m_lArakkoaGuardList.end(); ++itr)
                {
                    if (Creature* pGhostGuard = instance->GetCreature(*itr))
                        pGhostGuard->ForcedDespawn();
                }

                m_lArakkoaGuardList.clear();

                // Spawn new guards replacement
                float fX, fY, fZ, fO;
                for (GuidList::const_iterator itr = m_lErekemGuardList.begin(); itr != m_lErekemGuardList.end(); ++itr)
                {
                    if (Creature* pGuard = instance->GetCreature(*itr))
                    {
                        // Don't allow alive original guards while the boss is dead
                        if (!pGuard->isDead())
                            pGuard->ForcedDespawn();

                        // Spawn a ghost guard for each original guard
                        pGuard->GetRespawnCoord(fX, fY, fZ, &fO);
                        pGuard->SummonCreature(NPC_ARAKKOA_GUARD, fX, fY, fZ, fO, TEMPSUMMON_DEAD_DESPAWN, 0);
                    }
                }
            }
        }

        // Close Door if still open
        if (pData && (m_auiEncounter[pData->uiType] == DONE || m_auiEncounter[pData->uiType] == FAIL))
            UpdateCellForBoss(pData->uiEntry, true);
    }
}

void instance_violet_hold::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SINCLARI:
        case NPC_SINCLARI_ALT:
        case NPC_DOOR_SEAL:
        case NPC_EVENT_CONTROLLER:
            break;

        case NPC_EREKEM:
        case NPC_MORAGG:
        case NPC_ICHORON:
        case NPC_XEVOZZ:
        case NPC_LAVANTHOR:
        case NPC_ZURAMAT:
            m_vRandomBossList.push_back(pCreature->GetEntry());
            break;

        case NPC_PORTAL_INTRO:
            m_lIntroPortalList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_HOLD_GUARD:
            m_lGuardsList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_EREKEM_GUARD:
            m_lErekemGuardList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_ARAKKOA_GUARD:
            m_lArakkoaGuardList.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_ICHORON_SUMMON_TARGET:
            m_lIchoronTargetsList.push_back(pCreature->GetObjectGuid());
            return;

        case NPC_ARAKKOA:
        case NPC_VOID_LORD:
        case NPC_ETHERAL:
        case NPC_SWIRLING:
        case NPC_WATCHER:
        case NPC_LAVA_HOUND:
            break;

        default:
            return;
    }
    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_violet_hold::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CELL_LAVANTHOR:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_LAVANTHOR, pGo->GetObjectGuid()));
            return;
        case GO_CELL_MORAGG:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_MORAGG, pGo->GetObjectGuid()));
            return;
        case GO_CELL_ZURAMAT:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_ZURAMAT, pGo->GetObjectGuid()));
            return;
        case GO_CELL_XEVOZZ:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_XEVOZZ, pGo->GetObjectGuid()));
            return;
        case GO_CELL_ICHORON:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_ICHORON, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM_GUARD_L:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;
        case GO_CELL_EREKEM_GUARD_R:
            m_mBossToCellMap.insert(BossToCellMap::value_type(NPC_EREKEM, pGo->GetObjectGuid()));
            return;

        case GO_INTRO_CRYSTAL:
        case GO_PRISON_SEAL_DOOR:
            break;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_violet_hold::UpdateCellForBoss(uint32 uiBossEntry, bool bForceClosing /*= false*/)
{
    BossToCellMap::const_iterator itrCellLower = m_mBossToCellMap.lower_bound(uiBossEntry);
    BossToCellMap::const_iterator itrCellUpper = m_mBossToCellMap.upper_bound(uiBossEntry);

    if (itrCellLower == itrCellUpper)
        return;

    for (BossToCellMap::const_iterator itr = itrCellLower; itr != itrCellUpper; ++itr)
    {
        if (!bForceClosing)
            DoUseDoorOrButton(itr->second);
        else
        {
            GameObject* pGo = instance->GetGameObject(itr->second);
            if (pGo && pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR && pGo->GetGoState() == GO_STATE_ACTIVE)
                pGo->ResetDoorOrButton();
        }
    }
}

void instance_violet_hold::UpdateWorldState(bool bEnable)
{
    m_uiWorldState = bEnable ? 1 : 0;

    DoUpdateWorldState(WORLD_STATE_ID, m_uiWorldState);
    DoUpdateWorldState(WORLD_STATE_SEAL, m_uiWorldStateSealCount);
    DoUpdateWorldState(WORLD_STATE_PORTALS, m_uiWorldStatePortalCount);
}

void instance_violet_hold::OnPlayerEnter(Player* /*pPlayer*/)
{
    UpdateWorldState(m_auiEncounter[TYPE_MAIN] == IN_PROGRESS ? true : false);

    if (m_vRandomBosses.empty())
    {
        SetRandomBosses();
        ResetAll();
    }
}

void instance_violet_hold::SetData(uint32 uiType, uint32 uiData)
{
    debug_log("SD2: instance_violet_hold: SetData got type %u, data %u.", uiType, uiData);

    switch (uiType)
    {
        case TYPE_MAIN:
        {
            if (uiData == m_auiEncounter[uiType])
                return;
            if (m_auiEncounter[uiType] == DONE)
                return;

            switch (uiData)
            {
                case IN_PROGRESS:
                    // ToDo: enable the prison defense system when implemented
                    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR);
                    UpdateWorldState();
                    m_bIsDefenseless = true;
                    m_uiPortalId = urand(0, 2);
                    m_uiPortalTimer = 15000;
                    break;
                case FAIL:
                    if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI))
                        pSinclari->DealDamage(pSinclari, pSinclari->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    if (Creature* pController = GetSingleCreatureFromStorage(NPC_EVENT_CONTROLLER))
                        pController->AI()->EnterEvadeMode();
                    // Reset the event (creature cleanup is handled in creature_linking)
                    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR); // open instance door
                    ResetAll();
                    m_uiEventResetTimer = 20000;            // Timer may not be correct - 20 sec is default reset timer for blizz
                    break;
                case DONE:
                    DoUseDoorOrButton(GO_PRISON_SEAL_DOOR);
                    UpdateWorldState(false);
                    break;
                case SPECIAL:
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        }
        case TYPE_SEAL:
            m_auiEncounter[uiType] = uiData;
            if (uiData == SPECIAL)
            {
                --m_uiWorldStateSealCount;
                DoUpdateWorldState(WORLD_STATE_SEAL, m_uiWorldStateSealCount);

                // Yell at 75%, 50% and 25% shield
                if (m_uiWorldStateSealCount < 100 - 25 * m_uiSealYellCount)
                {
                    if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
                    {
                        // ToDo: I'm not sure if the last yell should be at 25% or at 5%. Needs research
                        ++m_uiSealYellCount;
                        DoScriptText(aSealWeakYell[m_uiSealYellCount - 1], pSinclari);
                    }
                }

                // set achiev to failed
                if (m_bIsDefenseless)
                    m_bIsDefenseless = false;

                if (!m_uiWorldStateSealCount)
                {
                    SetData(TYPE_MAIN, FAIL);
                    SetData(TYPE_SEAL, NOT_STARTED);
                }
            }
            break;
        case TYPE_PORTAL:
        {
            switch (uiData)
            {
                case SPECIAL:                               // timer to next
                    m_uiPortalTimer = 90000;
                    break;
                case DONE:                                  // portal done, set timer to 5 secs
                    m_uiPortalTimer = 3000;
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        }
        case TYPE_LAVANTHOR:
        case TYPE_MORAGG:
        case TYPE_EREKEM:
        case TYPE_ICHORON:
        case TYPE_XEVOZZ:
        case TYPE_ZURAMAT:
            if (uiData == DONE)
                m_uiPortalTimer = 35000;
            if (m_auiEncounter[uiType] != DONE)             // Keep the DONE-information stored
                m_auiEncounter[uiType] = uiData;
            // Handle achievements if necessary
            if (uiData == IN_PROGRESS)
            {
                if (uiType == TYPE_ZURAMAT)
                    m_bIsVoidDance = true;
                else if (uiType == TYPE_ICHORON)
                    m_bIsDehydratation = true;
            }
            if (uiData == SPECIAL && uiType == TYPE_ICHORON)
                m_bIsDehydratation = false;
            if (uiData == FAIL)
                SetData(TYPE_MAIN, FAIL);
            break;
        case TYPE_CYANIGOSA:
            if (uiData == DONE)
                SetData(TYPE_MAIN, DONE);
            if (uiData == FAIL)
                SetData(TYPE_MAIN, FAIL);
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
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_violet_hold::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];
    return 0;
}

void instance_violet_hold::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_violet_hold::SetIntroPortals(bool bDeactivate)
{
    for (GuidList::const_iterator itr = m_lIntroPortalList.begin(); itr != m_lIntroPortalList.end(); ++itr)
    {
        if (Creature* pPortal = instance->GetCreature(*itr))
        {
            if (bDeactivate)
                pPortal->ForcedDespawn();
            else
                pPortal->Respawn();
        }
    }
}

void instance_violet_hold::SpawnPortal()
{
    if (const PortalData* pData = GetPortalData())
    {
        if (Creature* pController = GetSingleCreatureFromStorage(NPC_SINCLARI_ALT))
        {
            uint32 uiPortalEntry = pData->pPortalType == PORTAL_TYPE_NORM ? NPC_PORTAL : NPC_PORTAL_ELITE;

            pController->SummonCreature(uiPortalEntry, pData->fX, pData->fY, pData->fZ, pData->fOrient, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 1800 * IN_MILLISECONDS);
        }
    }
}

void instance_violet_hold::SetPortalId()
{
    if (IsCurrentPortalForTrash())
    {
        // Find another Trash portal position
        uint8 uiTemp = m_uiPortalId + urand(1, m_uiMaxCountPortalLoc - 1);
        // Decrease m_uiMaxCountPortalLoc so that the center position is skipped
        uiTemp %= m_uiMaxCountPortalLoc - 1;

        debug_log("SD2: instance_violet_hold: SetPortalId %u, old was id %u.", uiTemp, m_uiPortalId);

        m_uiPortalId = uiTemp;
    }
    else if (GetCurrentPortalNumber() == 18)
    {
        debug_log("SD2: instance_violet_hold: SetPortalId %u (Cyanigosa), old was id %u.", 0, m_uiPortalId);
        m_uiPortalId = 0;
    }
    else
    {
        debug_log("SD2: instance_violet_hold: SetPortalId %u (is boss), old was id %u.", m_uiMaxCountPortalLoc, m_uiPortalId);
        m_uiPortalId = m_uiMaxCountPortalLoc;
    }
}

BossSpawn* instance_violet_hold::CreateBossSpawnByEntry(uint32 uiEntry)
{
    BossSpawn* pBossSpawn = new BossSpawn;
    pBossSpawn->uiEntry = uiEntry;

    if (Creature* pBoss = GetSingleCreatureFromStorage(uiEntry))
        pBoss->GetRespawnCoord(pBossSpawn->fX, pBossSpawn->fY, pBossSpawn->fZ, &(pBossSpawn->fO));

    return pBossSpawn;
}

void instance_violet_hold::SetRandomBosses()
{
    // Store bosses that are already done
    for (uint8 i = 0; i < MAX_MINIBOSSES; ++i)
    {
        if (m_auiEncounter[aBossInformation[i].uiType] == DONE)
            m_vRandomBosses.push_back(CreateBossSpawnByEntry(aBossInformation[i].uiEntry));
    }

    if (m_vRandomBosses.size() < 2)                         // Get some new random bosses
    {
        std::random_shuffle(m_vRandomBossList.begin(), m_vRandomBossList.end());
        // two required, in case the first is already pushed to m_vRandomBosses
        if (m_vRandomBossList.size() < 2)
            script_error_log("instance_violet_hold, Mini Bosses are not properly spawned");
        else
            m_vRandomBossList.resize(2);

        // Fill up some random bosses
        for (std::vector<uint32>::const_iterator itr = m_vRandomBossList.begin(); itr != m_vRandomBossList.end(); ++itr)
        {
            if (m_vRandomBosses.empty() || m_vRandomBosses[0]->uiEntry != *itr)
                m_vRandomBosses.push_back(CreateBossSpawnByEntry(*itr));
        }
    }

    for (uint8 i = 0; i < m_vRandomBosses.size(); ++i)
        debug_log("SD2: instance_violet_hold random boss %u is entry %u", i, m_vRandomBosses[i]->uiEntry);
}

void instance_violet_hold::CallGuards(bool bRespawn)
{
    for (GuidList::const_iterator itr = m_lGuardsList.begin(); itr != m_lGuardsList.end(); ++itr)
    {
        if (Creature* pGuard = instance->GetCreature(*itr))
        {
            if (bRespawn)
                pGuard->Respawn();
            else if (pGuard->isAlive())
            {
                pGuard->SetWalk(false);
                pGuard->GetMotionMaster()->MovePoint(0, fGuardExitLoc[0], fGuardExitLoc[1], fGuardExitLoc[2]);
                pGuard->ForcedDespawn(6000);
            }
        }
    }
}

void instance_violet_hold::ProcessActivationCrystal(Unit* pUser, bool bIsIntro)
{
    if (Creature* pSummon = pUser->SummonCreature(NPC_DEFENSE_SYSTEM, fDefenseSystemLoc[0], fDefenseSystemLoc[1], fDefenseSystemLoc[2], fDefenseSystemLoc[3], TEMPSUMMON_TIMED_DESPAWN, 10000))
    {
        pSummon->CastSpell(pSummon, SPELL_DEFENSE_SYSTEM_VISUAL, true);

        // TODO: figure out how the rest work
        // NPC's NPC_DEFENSE_DUMMY_TARGET are probably channeling some spell to the defense system
    }

    if (bIsIntro)
        DoUseDoorOrButton(GO_INTRO_CRYSTAL);

    // else, kill (and despawn?) certain trash mobs. Also boss affected, but not killed.
}

bool instance_violet_hold::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
            // ToDo: uncomment these when they are implemented
            // case ACHIEV_CRIT_DEFENSELES:
            //    return m_bIsDefenseless;
            // case ACHIEV_CRIT_DEHYDRATATION:
            //    return m_bIsDehydratation;
        case ACHIEV_CRIT_VOID_DANCE:
            return m_bIsVoidDance;

        default:
            return false;
    }
}

void instance_violet_hold::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, IN_PROGRESS);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, IN_PROGRESS);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, IN_PROGRESS);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, IN_PROGRESS);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, IN_PROGRESS);
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, IN_PROGRESS);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, IN_PROGRESS);
            break;
        case NPC_AZURE_CAPTAIN:
        case NPC_AZURE_RAIDER:
        case NPC_AZURE_SORCEROR:
        case NPC_AZURE_STALKER:
        case NPC_AZURE_INVADER:
        case NPC_MAGE_HUNTER:
        case NPC_AZURE_SPELLBREAKER:
        case NPC_AZURE_BINDER:
        case NPC_AZURE_MAGE_SLAYER:
            // Interrupt door seal casting (if necessary)
            pCreature->InterruptNonMeleeSpells(false);
            break;
    }
}

void instance_violet_hold::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_EREKEM_GUARD:
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, FAIL);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, FAIL);
            break;
        case NPC_AZURE_CAPTAIN:
        case NPC_AZURE_RAIDER:
        case NPC_AZURE_SORCEROR:
        case NPC_AZURE_STALKER:
        case NPC_AZURE_INVADER:
        case NPC_MAGE_HUNTER:
        case NPC_AZURE_SPELLBREAKER:
        case NPC_AZURE_BINDER:
        case NPC_AZURE_MAGE_SLAYER:
            // Allow them to finish off the door seal
            pCreature->SetWalk(false);
            pCreature->GetMotionMaster()->MovePoint(1, fSealAttackLoc[0], fSealAttackLoc[1], fSealAttackLoc[2]);
            break;
    }
}

void instance_violet_hold::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ZURAMAT:
        case NPC_VOID_LORD:
            SetData(TYPE_ZURAMAT, DONE);
            break;
        case NPC_XEVOZZ:
        case NPC_ETHERAL:
            SetData(TYPE_XEVOZZ, DONE);
            break;
        case NPC_LAVANTHOR:
        case NPC_LAVA_HOUND:
            SetData(TYPE_LAVANTHOR, DONE);
            break;
        case NPC_MORAGG:
        case NPC_WATCHER:
            SetData(TYPE_MORAGG, DONE);
            break;
        case NPC_EREKEM:
        case NPC_ARAKKOA:
            SetData(TYPE_EREKEM, DONE);
            break;
        case NPC_ICHORON:
        case NPC_SWIRLING:
            SetData(TYPE_ICHORON, DONE);
            break;
        case NPC_CYANIGOSA:
            SetData(TYPE_CYANIGOSA, DONE);
            break;
        case NPC_VOID_SENTRY:
            if (GetData(TYPE_ZURAMAT) == IN_PROGRESS)
                m_bIsVoidDance = false;
            break;
    }
}

void instance_violet_hold::Update(uint32 uiDiff)
{
    if (m_uiEventResetTimer)
    {
        if (m_uiEventResetTimer <= uiDiff)
        {
            if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI))
                pSinclari->Respawn();

            m_uiEventResetTimer = 0;
        }
        else
            m_uiEventResetTimer -= uiDiff;
    }

    if (m_auiEncounter[TYPE_MAIN] != IN_PROGRESS)
        return;

    if (m_uiPortalTimer)
    {
        if (m_uiPortalTimer <= uiDiff)
        {
            DoUpdateWorldState(WORLD_STATE_PORTALS, ++m_uiWorldStatePortalCount);

            SetPortalId();
            SpawnPortal();

            m_uiPortalTimer = 0;
        }
        else
            m_uiPortalTimer -= uiDiff;
    }
}

BossInformation const* instance_violet_hold::GetBossInformation(uint32 uiEntry/* = 0*/)
{
    uint32 mEntry = uiEntry;
    if (!mEntry)
    {
        if (GetCurrentPortalNumber() == 6 && m_vRandomBosses.size() >= 1)
            mEntry = m_vRandomBosses[0]->uiEntry;
        else if (GetCurrentPortalNumber() == 12 && m_vRandomBosses.size() >= 2)
            mEntry = m_vRandomBosses[1]->uiEntry;
    }

    if (!mEntry)
        return NULL;

    for (uint8 i = 0; i < MAX_MINIBOSSES; ++i)
    {
        if (aBossInformation[i].uiEntry == mEntry)
            return &aBossInformation[i];
    }

    return NULL;
}

instance_violet_hold::~instance_violet_hold()
{
    // Need to free std::vector<sBossSpawn*> m_vRandomBosses;
    for (std::vector<BossSpawn*>::const_iterator itr = m_vRandomBosses.begin(); itr != m_vRandomBosses.end(); ++itr)
    {
        if (*itr)
            delete(*itr);
    }
}

InstanceData* GetInstanceData_instance_violet_hold(Map* pMap)
{
    return new instance_violet_hold(pMap);
}

void AddSC_instance_violet_hold()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_violet_hold";
    pNewScript->GetInstanceData = GetInstanceData_instance_violet_hold;
    pNewScript->RegisterSelf();
}
