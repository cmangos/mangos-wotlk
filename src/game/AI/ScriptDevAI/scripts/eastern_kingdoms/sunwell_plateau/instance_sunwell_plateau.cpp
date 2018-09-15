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
SDName: Instance_Sunwell_Plateau
SD%Complete: 70%
SDComment:
SDCategory: Sunwell_Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "sunwell_plateau.h"

/* Sunwell Plateau:
0 - Kalecgos and Sathrovarr
1 - Brutallus
2 - Felmyst
3 - Eredar Twins (Alythess and Sacrolash)
4 - M'uru
5 - Kil'Jaeden
*/

static const DialogueEntry aFelmystOutroDialogue[] =
{
    {NPC_KALECGOS_MADRIGOSA, 0,                        10000},
    {SAY_KALECGOS_OUTRO,     NPC_KALECGOS_MADRIGOSA,   5000},
    {NPC_FELMYST,            0,                        5000},
    {SPELL_OPEN_BACK_DOOR,   0,                        9000},
    {NPC_BRUTALLUS,          0,                        0},
    {0, 0, 0},
};

instance_sunwell_plateau::instance_sunwell_plateau(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aFelmystOutroDialogue),
    m_uiDeceiversKilled(0),
    m_uiSpectralRealmTimer(5000),
    m_uiKalecRespawnTimer(0),
    m_uiMuruBerserkTimer(0),
    m_uiKiljaedenYellTimer(90000)
{
    Initialize();
}

void instance_sunwell_plateau::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
}

bool instance_sunwell_plateau::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_sunwell_plateau::OnPlayerEnter(Player* pPlayer)
{
    // Spawn Felmyst if not already dead and Brutallus is complete
    if (m_auiEncounter[TYPE_BRUTALLUS] == DONE && m_auiEncounter[TYPE_FELMYST] != DONE)
    {
        // Summon Felmyst in reload case if not already summoned
        if (!GetSingleCreatureFromStorage(NPC_FELMYST, true))
            pPlayer->SummonCreature(NPC_FELMYST, aMadrigosaLoc[0].m_fX, aMadrigosaLoc[0].m_fY, aMadrigosaLoc[0].m_fZ, aMadrigosaLoc[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0, true);
    }

    // Spawn M'uru after the Eredar Twins
    if (m_auiEncounter[TYPE_EREDAR_TWINS] == DONE && m_auiEncounter[TYPE_MURU] != DONE)
    {
        if (!GetSingleCreatureFromStorage(NPC_MURU, true))
            pPlayer->SummonCreature(NPC_MURU, afMuruSpawnLoc[0], afMuruSpawnLoc[1], afMuruSpawnLoc[2], afMuruSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
    }
}

void instance_sunwell_plateau::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_KALECGOS_DRAGON:
        case NPC_KALECGOS_HUMAN:
        case NPC_SATHROVARR:
        case NPC_FLIGHT_TRIGGER_LEFT:
        case NPC_FLIGHT_TRIGGER_RIGHT:
        case NPC_MADRIGOSA:
        case NPC_BRUTALLUS:
        case NPC_FELMYST:
        case NPC_KALECGOS_MADRIGOSA:
        case NPC_ALYTHESS:
        case NPC_SACROLASH:
        case NPC_MURU:
        case NPC_ENTROPIUS:
        case NPC_KILJAEDEN_CONTROLLER:
        case NPC_KILJAEDEN:
        case NPC_KALECGOS:
        case NPC_ANVEENA:
        case NPC_VELEN:
        case NPC_LIADRIN:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_DECEIVER:
            m_lDeceiversGuidList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER:
            // sort triggers for flightpath
            if (pCreature->GetPositionZ() < 51.0f)
                m_lAllFlightTriggersList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_WORLD_TRIGGER_LARGE:
            if (pCreature->GetPositionY() < 523.0f)
                m_lBackdoorTriggersList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_BERSERKER:
        case NPC_FURY_MAGE:
        case NPC_DARK_FIEND:
        case NPC_VOID_SENTINEL:
            m_lMuruTrashGuidList.push_back(pCreature->GetObjectGuid());
    }
}

void instance_sunwell_plateau::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_DECEIVER)
    {
        ++m_uiDeceiversKilled;
        // Spawn Kiljaeden when all deceivers are killed
        if (m_uiDeceiversKilled == MAX_DECEIVERS)
        {
            if (Creature* pController = GetSingleCreatureFromStorage(NPC_KILJAEDEN_CONTROLLER))
            {
                if (Creature* pKiljaeden = pController->SummonCreature(NPC_KILJAEDEN, pController->GetPositionX(), pController->GetPositionY(), pController->GetPositionZ(), pController->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0))
                    pKiljaeden->SetInCombatWithZone();

                pController->RemoveAurasDueToSpell(SPELL_ANVEENA_DRAIN);
            }
        }
    }
}

void instance_sunwell_plateau::OnCreatureEvade(Creature* pCreature)
{
    // Reset encounter if raid wipes at deceivers
    if (pCreature->GetEntry() == NPC_DECEIVER)
        SetData(TYPE_KILJAEDEN, FAIL);
}

void instance_sunwell_plateau::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_FORCEFIELD:
        case GO_BOSS_COLLISION_1:
        case GO_BOSS_COLLISION_2:
        case GO_ICE_BARRIER:
            break;
        case GO_FIRE_BARRIER:
            if (m_auiEncounter[TYPE_KALECGOS] == DONE && m_auiEncounter[TYPE_BRUTALLUS] == DONE && m_auiEncounter[TYPE_FELMYST] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_MURU_ENTER_GATE:
            break;
        case GO_MURU_EXIT_GATE:
            if (m_auiEncounter[TYPE_MURU] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ORB_BLUE_FLIGHT_1:
        case GO_ORB_BLUE_FLIGHT_2:
        case GO_ORB_BLUE_FLIGHT_3:
        case GO_ORB_BLUE_FLIGHT_4:
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_sunwell_plateau::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_KALECGOS:
            m_auiEncounter[uiType] = uiData;
            // combat doors
            DoUseDoorOrButton(GO_FORCEFIELD);
            DoUseDoorOrButton(GO_BOSS_COLLISION_1);
            DoUseDoorOrButton(GO_BOSS_COLLISION_2);
            if (uiData == FAIL)
            {
                m_uiKalecRespawnTimer = 20000;

                if (Creature* pKalecDragon = GetSingleCreatureFromStorage(NPC_KALECGOS_DRAGON))
                    pKalecDragon->ForcedDespawn();
                if (Creature* pKalecHuman = GetSingleCreatureFromStorage(NPC_KALECGOS_HUMAN))
                    pKalecHuman->ForcedDespawn();
                if (Creature* pSathrovarr = GetSingleCreatureFromStorage(NPC_SATHROVARR))
                    pSathrovarr->AI()->EnterEvadeMode();
            }
            break;
        case TYPE_BRUTALLUS:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FELMYST:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                StartNextDialogueText(NPC_KALECGOS_MADRIGOSA);
            else if (uiData == IN_PROGRESS)
                DoSortFlightTriggers();
            break;
        case TYPE_EREDAR_TWINS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                if (Player* pPlayer = GetPlayerInMap())
                    pPlayer->SummonCreature(NPC_MURU, afMuruSpawnLoc[0], afMuruSpawnLoc[1], afMuruSpawnLoc[2], afMuruSpawnLoc[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
            }
            break;
        case TYPE_MURU:
            m_auiEncounter[uiType] = uiData;
            // combat door
            DoUseDoorOrButton(GO_MURU_ENTER_GATE);
            if (uiData == DONE)
                DoUseDoorOrButton(GO_MURU_EXIT_GATE);
            else if (uiData == IN_PROGRESS)
                m_uiMuruBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
            if (uiData == FAIL || uiData == DONE)
            {
                // clear all the trash mobs
                for (GuidList::const_iterator itr = m_lMuruTrashGuidList.begin(); itr != m_lMuruTrashGuidList.end(); ++itr)
                {
                    if (Creature* pTrash = instance->GetCreature(*itr))
                        pTrash->ForcedDespawn();
                }

                m_lMuruTrashGuidList.clear();
            }
            break;
        case TYPE_KILJAEDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == FAIL)
            {
                m_uiDeceiversKilled = 0;

                // Reset Orbs
                DoToggleGameObjectFlags(GO_ORB_BLUE_FLIGHT_1, GO_FLAG_NO_INTERACT, true);
                DoToggleGameObjectFlags(GO_ORB_BLUE_FLIGHT_2, GO_FLAG_NO_INTERACT, true);
                DoToggleGameObjectFlags(GO_ORB_BLUE_FLIGHT_3, GO_FLAG_NO_INTERACT, true);
                DoToggleGameObjectFlags(GO_ORB_BLUE_FLIGHT_4, GO_FLAG_NO_INTERACT, true);

                // Respawn deceivers
                for (GuidList::const_iterator itr = m_lDeceiversGuidList.begin(); itr != m_lDeceiversGuidList.end(); ++itr)
                {
                    if (Creature* pDeceiver = instance->GetCreature(*itr))
                    {
                        if (!pDeceiver->isAlive())
                            pDeceiver->Respawn();
                    }
                }
            }
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

uint32 instance_sunwell_plateau::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_sunwell_plateau::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    if (m_uiKalecRespawnTimer)
    {
        if (m_uiKalecRespawnTimer <= uiDiff)
        {
            if (Creature* pKalecDragon = GetSingleCreatureFromStorage(NPC_KALECGOS_DRAGON))
                pKalecDragon->Respawn();
            if (Creature* pKalecHuman = GetSingleCreatureFromStorage(NPC_KALECGOS_HUMAN))
                pKalecHuman->Respawn();
            m_uiKalecRespawnTimer = 0;
        }
        else
            m_uiKalecRespawnTimer -= uiDiff;
    }

    // Muru berserk timer; needs to be done here because it involves two distinct creatures
    if (m_auiEncounter[TYPE_MURU] == IN_PROGRESS)
    {
        if (m_uiMuruBerserkTimer < uiDiff)
        {
            if (Creature* pEntrpius = GetSingleCreatureFromStorage(NPC_ENTROPIUS, true))
                pEntrpius->CastSpell(pEntrpius, SPELL_MURU_BERSERK, TRIGGERED_OLD_TRIGGERED);
            else if (Creature* pMuru = GetSingleCreatureFromStorage(NPC_MURU))
                pMuru->CastSpell(pMuru, SPELL_MURU_BERSERK, TRIGGERED_OLD_TRIGGERED);

            m_uiMuruBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
        }
        else
            m_uiMuruBerserkTimer -= uiDiff;
    }

    if (m_auiEncounter[TYPE_KILJAEDEN] == NOT_STARTED || m_auiEncounter[TYPE_KILJAEDEN] == FAIL)
    {
        if (m_uiKiljaedenYellTimer < uiDiff)
        {
            switch (urand(0, 4))
            {
                case 0: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_1, NPC_KILJAEDEN_CONTROLLER); break;
                case 1: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_2, NPC_KILJAEDEN_CONTROLLER); break;
                case 2: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_3, NPC_KILJAEDEN_CONTROLLER); break;
                case 3: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_4, NPC_KILJAEDEN_CONTROLLER); break;
                case 4: DoOrSimulateScriptTextForThisInstance(SAY_ORDER_5, NPC_KILJAEDEN_CONTROLLER); break;
            }
            m_uiKiljaedenYellTimer = 90000;
        }
        else
            m_uiKiljaedenYellTimer -= uiDiff;
    }
}

void instance_sunwell_plateau::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >>
               m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

static bool sortByPositionX(Creature* pFirst, Creature* pSecond)
{
    return pFirst && pSecond && pFirst->GetPositionX() > pSecond->GetPositionX();
}

void instance_sunwell_plateau::DoSortFlightTriggers()
{
    if (m_lAllFlightTriggersList.empty())
    {
        script_error_log("Instance Sunwell Plateau: ERROR Failed to load flight triggers for creature id %u.", NPC_FELMYST);
        return;
    }

    std::list<Creature*> lTriggers;                         // Valid pointers, only used locally
    for (GuidList::const_iterator itr = m_lAllFlightTriggersList.begin(); itr != m_lAllFlightTriggersList.end(); ++itr)
    {
        if (Creature* pTrigger = instance->GetCreature(*itr))
            lTriggers.push_back(pTrigger);
    }

    if (lTriggers.empty())
        return;

    // sort the flight triggers; first by position X, then group them by Y (left and right)
    lTriggers.sort(sortByPositionX);
    for (auto& lTrigger : lTriggers)
    {
        if (lTrigger->GetPositionY() < 600.0f)
            m_vRightFlightTriggersVect.push_back(lTrigger->GetObjectGuid());
        else
            m_vLeftFlightTriggersVect.push_back(lTrigger->GetObjectGuid());
    }
}

ObjectGuid instance_sunwell_plateau::SelectFelmystFlightTrigger(bool bLeftSide, uint8 uiIndex)
{
    // Return the flight trigger from the selected index
    GuidVector& vTemp = bLeftSide ? m_vLeftFlightTriggersVect : m_vRightFlightTriggersVect;

    if (uiIndex >= vTemp.size())
        return ObjectGuid();

    return vTemp[uiIndex];
}

void instance_sunwell_plateau::DoEjectSpectralPlayers()
{
    for (auto m_spectralRealmPlayer : m_spectralRealmPlayers)
    {
        if (Player* pPlayer = instance->GetPlayer(m_spectralRealmPlayer))
        {
            if (!pPlayer->HasAura(SPELL_SPECTRAL_REALM_AURA))
                continue;

            pPlayer->CastSpell(pPlayer, SPELL_TELEPORT_NORMAL_REALM, TRIGGERED_OLD_TRIGGERED);
            pPlayer->CastSpell(pPlayer, SPELL_SPECTRAL_EXHAUSTION, TRIGGERED_OLD_TRIGGERED);
            pPlayer->RemoveAurasDueToSpell(SPELL_SPECTRAL_REALM_AURA);
        }
    }
}

void instance_sunwell_plateau::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case NPC_KALECGOS_MADRIGOSA:
            if (Creature* pTrigger = GetSingleCreatureFromStorage(NPC_FLIGHT_TRIGGER_LEFT))
            {
                if (Creature* pKalec = pTrigger->SummonCreature(NPC_KALECGOS_MADRIGOSA, aKalecLoc[0].m_fX, aKalecLoc[0].m_fY, aKalecLoc[0].m_fZ, aKalecLoc[0].m_fO, TEMPSPAWN_CORPSE_DESPAWN, 0))
                {
                    pKalec->SetWalk(false);
                    pKalec->SetLevitate(true);
                    pKalec->GetMotionMaster()->MovePoint(0, aKalecLoc[1].m_fX, aKalecLoc[1].m_fY, aKalecLoc[1].m_fZ, false);
                }
            }
            break;
        case NPC_FELMYST:
            if (Creature* pKalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
                pKalec->GetMotionMaster()->MovePoint(0, aKalecLoc[2].m_fX, aKalecLoc[2].m_fY, aKalecLoc[2].m_fZ, false);
            break;
        case SPELL_OPEN_BACK_DOOR:
            if (Creature* pKalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
            {
                // ToDo: update this when the AoE spell targeting will support many explicit target. Kalec should target all creatures from the list
                if (Creature* pTrigger = instance->GetCreature(m_lBackdoorTriggersList.front()))
                    pKalec->CastSpell(pTrigger, SPELL_OPEN_BACK_DOOR, TRIGGERED_OLD_TRIGGERED);
            }
            break;
        case NPC_BRUTALLUS:
            if (Creature* pKalec = GetSingleCreatureFromStorage(NPC_KALECGOS_MADRIGOSA))
            {
                pKalec->ForcedDespawn(10000);
                pKalec->GetMotionMaster()->MovePoint(0, aKalecLoc[3].m_fX, aKalecLoc[3].m_fY, aKalecLoc[3].m_fZ, false);
            }
            break;
    }
}

InstanceData* GetInstanceData_instance_sunwell_plateau(Map* pMap)
{
    return new instance_sunwell_plateau(pMap);
}

bool AreaTrigger_at_sunwell_plateau(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_TWINS)
    {
        if (pPlayer->isGameMaster() || pPlayer->isDead())
            return false;

        instance_sunwell_plateau* pInstance = (instance_sunwell_plateau*)pPlayer->GetInstanceData();

        if (pInstance && pInstance->GetData(TYPE_EREDAR_TWINS) == NOT_STARTED)
            pInstance->SetData(TYPE_EREDAR_TWINS, SPECIAL);
    }

    return false;
}

void AddSC_instance_sunwell_plateau()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_sunwell_plateau";
    pNewScript->GetInstanceData = &GetInstanceData_instance_sunwell_plateau;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_sunwell_plateau";
    pNewScript->pAreaTrigger = &AreaTrigger_at_sunwell_plateau;
    pNewScript->RegisterSelf();
}
