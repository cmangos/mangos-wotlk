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
SDName: Instance_Naxxramas
SD%Complete: 90%
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

static const DialogueEntry aNaxxDialogue[] =
{
    {NPC_KELTHUZAD,         0,                  10000},
    {SAY_SAPP_DIALOG1,      NPC_KELTHUZAD,      8000},
    {SAY_SAPP_DIALOG2_LICH, NPC_THE_LICHKING,   14000},
    {SAY_SAPP_DIALOG3,      NPC_KELTHUZAD,      10000},
    {SAY_SAPP_DIALOG4_LICH, NPC_THE_LICHKING,   12000},
    {SAY_SAPP_DIALOG5,      NPC_KELTHUZAD,      0},
    {NPC_THANE,             0,                  10000},
    {SAY_KORT_TAUNT1,       NPC_THANE,          5000},
    {SAY_ZELI_TAUNT1,       NPC_ZELIEK,         6000},
    {SAY_BLAU_TAUNT1,       NPC_BLAUMEUX,       6000},
    {SAY_RIVE_TAUNT1,       NPC_RIVENDARE,      6000},
    {SAY_BLAU_TAUNT2,       NPC_BLAUMEUX,       6000},
    {SAY_ZELI_TAUNT2,       NPC_ZELIEK,         5000},
    {SAY_KORT_TAUNT2,       NPC_THANE,          7000},
    {SAY_RIVE_TAUNT2,       NPC_RIVENDARE,      0},
    {0, 0, 0}
};

instance_naxxramas::instance_naxxramas(Map* pMap) : ScriptedInstance(pMap),
    m_fChamberCenterX(0.0f),
    m_fChamberCenterY(0.0f),
    m_fChamberCenterZ(0.0f),
    m_uiSapphSpawnTimer(0),
    m_uiTauntTimer(0),
    m_uiHorsemenAchievTimer(0),
    m_uiHorseMenKilled(0),
    m_uiLivingPoisonTimer(5000),
    m_uiScreamsTimer(2 * MINUTE * IN_MILLISECONDS),
    m_despawnKTTriggerTimer(0),
    m_dialogueHelper(aNaxxDialogue)
{
    Initialize();
}

void instance_naxxramas::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for (bool& i : m_abAchievCriteria)
        i = false;

    m_dialogueHelper.InitializeDialogueHelper(this, true);
}

void instance_naxxramas::OnPlayerEnter(Player* pPlayer)
{
    // Function only used to summon Sapphiron in case of server reload
    if (GetData(TYPE_SAPPHIRON) != SPECIAL)
        return;

    // Check if already summoned
    if (GetSingleCreatureFromStorage(NPC_SAPPHIRON, true))
        return;

    pPlayer->SummonCreature(NPC_SAPPHIRON, aSapphPositions[0], aSapphPositions[1], aSapphPositions[2], aSapphPositions[3], TEMPSPAWN_DEAD_DESPAWN, 0);
}

void instance_naxxramas::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ANUB_REKHAN:
        case NPC_FAERLINA:
        case NPC_THADDIUS:
        case NPC_STALAGG:
        case NPC_FEUGEN:
        case NPC_ZELIEK:
        case NPC_THANE:
        case NPC_BLAUMEUX:
        case NPC_RIVENDARE:
        case NPC_GOTHIK:
        case NPC_SAPPHIRON:
        case NPC_KELTHUZAD:
        case NPC_THE_LICHKING:
        case NPC_GLUTH:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;

        case NPC_SUB_BOSS_TRIGGER:  m_lGothTriggerList.push_back(pCreature->GetObjectGuid()); break;
        case NPC_TESLA_COIL:        m_lThadTeslaCoilList.push_back(pCreature->GetObjectGuid()); break;
        case NPC_ROTTING_MAGGOT:
        case NPC_DISEASED_MAGGOT:
        case NPC_EYE_STALK:
            m_sHeiganBackroomAdds.push_back(pCreature->GetObjectGuid()); break;
        case NPC_GROBBULUS_CLOUD:
            m_lGrobbulusClouds.push_back(pCreature->GetObjectGuid()); break;
        case NPC_CORPSE_SCARAB:
            m_lCorpseScarabs.push_back(pCreature->GetObjectGuid()); break;
        case NPC_OLDWORLD_TRIGGER:
        {
            if (pCreature->GetPositionX() > 3250 && pCreature->GetPositionX() < 3322 && pCreature->GetPositionY() > -3190 && pCreature->GetPositionY() < -3115)
                m_gluthTriggerVector.push_back(pCreature->GetObjectGuid());
            break;
        }
        case NPC_ZOMBIE_CHOW_N:
        case NPC_ZOMBIE_CHOW_H:
        {
            if (Creature* gluth = GetSingleCreatureFromStorage(NPC_GLUTH))
            {
                if (!gluth->IsAlive())
                    break;
                pCreature->getThreatManager().addThreat(gluth, 2000.f);
                pCreature->GetMotionMaster()->MoveChase(gluth);
            }
            break;
        }
    }
}

void instance_naxxramas::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        // Arachnid Quarter
        case GO_ARAC_ANUB_DOOR:
            break;
        case GO_ARAC_ANUB_GATE:
            if (m_auiEncounter[TYPE_ANUB_REKHAN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ARAC_FAER_WEB:
            break;
        case GO_ARAC_FAER_DOOR:
            if (m_auiEncounter[TYPE_FAERLINA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ARAC_MAEX_INNER_DOOR:
            break;
        case GO_ARAC_MAEX_OUTER_DOOR:
            if (m_auiEncounter[TYPE_FAERLINA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;

        // Plague Quarter
        case GO_PLAG_NOTH_ENTRY_DOOR:
            break;
        case GO_PLAG_NOTH_EXIT_DOOR:
            if (m_auiEncounter[TYPE_NOTH] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PLAG_HEIG_ENTRY_DOOR:
            if (m_auiEncounter[TYPE_NOTH] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PLAG_HEIG_EXIT_DOOR:
            if (m_auiEncounter[TYPE_HEIGAN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PLAG_LOAT_DOOR:
            break;

        // Military Quarter
        case GO_MILI_GOTH_ENTRY_GATE:
            break;
        case GO_MILI_GOTH_EXIT_GATE:
            if (m_auiEncounter[TYPE_GOTHIK] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_MILI_GOTH_COMBAT_GATE:
            break;
        case GO_MILI_HORSEMEN_DOOR:
            if (m_auiEncounter[TYPE_GOTHIK] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CHEST_HORSEMEN_NORM:
        case GO_CHEST_HORSEMEN_HERO:
            break;

        // Construct Quarter
        case GO_CONS_PATH_EXIT_DOOR:
            if (m_auiEncounter[TYPE_PATCHWERK] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CONS_GLUT_EXIT_DOOR:
            if (m_auiEncounter[TYPE_GLUTH] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CONS_THAD_DOOR:
            if (m_auiEncounter[TYPE_GLUTH] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CONS_NOX_TESLA_FEUGEN:
            if (m_auiEncounter[TYPE_THADDIUS] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_CONS_NOX_TESLA_STALAGG:
            if (m_auiEncounter[TYPE_THADDIUS] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;

        // Frostwyrm Lair
        case GO_KELTHUZAD_WATERFALL_DOOR:
            if (m_auiEncounter[TYPE_SAPPHIRON] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_KELTHUZAD_EXIT_DOOR:
            break;
        case GO_KELTHUZAD_TRIGGER:
            if (m_auiEncounter[TYPE_KELTHUZAD] != NOT_STARTED) // Only spawn the visual trigger for Kel'Thuzad when encounter is not started
                pGo->SetLootState(GO_JUST_DEACTIVATED);
            break;

        // Eyes
        case GO_ARAC_EYE_RAMP:
        case GO_ARAC_EYE_BOSS:
            if (m_auiEncounter[TYPE_MAEXXNA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_PLAG_EYE_RAMP:
        case GO_PLAG_EYE_BOSS:
            if (m_auiEncounter[TYPE_LOATHEB] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_MILI_EYE_RAMP:
        case GO_MILI_EYE_BOSS:
            if (m_auiEncounter[TYPE_FOUR_HORSEMEN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CONS_EYE_RAMP:
        case GO_CONS_EYE_BOSS:
            if (m_auiEncounter[TYPE_THADDIUS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;

        // Portals
        case GO_ARAC_PORTAL:
            if (m_auiEncounter[TYPE_MAEXXNA] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_PLAG_PORTAL:
            if (m_auiEncounter[TYPE_LOATHEB] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_MILI_PORTAL:
            if (m_auiEncounter[TYPE_FOUR_HORSEMEN] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONS_PORTAL:
            if (m_auiEncounter[TYPE_THADDIUS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;

        default:
            // Heigan Traps - many different entries which are only required for sorting
            if (pGo->GetGoType() == GAMEOBJECT_TYPE_TRAP)
            {
                uint32 uiGoEntry = pGo->GetEntry();

                if ((uiGoEntry >= 181517 && uiGoEntry <= 181524) || uiGoEntry == 181678)
                    m_alHeiganTrapGuids[0].push_back(pGo->GetObjectGuid());
                else if ((uiGoEntry >= 181510 && uiGoEntry <= 181516) || (uiGoEntry >= 181525 && uiGoEntry <= 181531) || uiGoEntry == 181533 || uiGoEntry == 181676)
                    m_alHeiganTrapGuids[1].push_back(pGo->GetObjectGuid());
                else if ((uiGoEntry >= 181534 && uiGoEntry <= 181544) || uiGoEntry == 181532 || uiGoEntry == 181677)
                    m_alHeiganTrapGuids[2].push_back(pGo->GetObjectGuid());
                else if ((uiGoEntry >= 181545 && uiGoEntry <= 181552) || uiGoEntry == 181695)
                    m_alHeiganTrapGuids[3].push_back(pGo->GetObjectGuid());
            }

            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_naxxramas::OnPlayerDeath(Player* /*pPlayer*/)
{
    if (IsEncounterInProgress())
        SetData(TYPE_UNDYING_FAILED, DONE);

    if (GetData(TYPE_HEIGAN) == IN_PROGRESS)
        SetSpecialAchievementCriteria(TYPE_ACHIEV_SAFETY_DANCE, false);
}

void instance_naxxramas::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_MR_BIGGLESWORTH && m_auiEncounter[TYPE_KELTHUZAD] != DONE)
        DoOrSimulateScriptTextForThisInstance(SAY_KELTHUZAD_CAT_DIED, NPC_KELTHUZAD);

    switch (pCreature->GetEntry())
    {
        case NPC_EYE_STALK:
        case NPC_DISEASED_MAGGOT:
        case NPC_ROTTING_MAGGOT:
            m_sHeiganBackroomAdds.remove(pCreature->GetObjectGuid()); break;
        case NPC_GROBBULUS_CLOUD:
            m_lGrobbulusClouds.remove(pCreature->GetObjectGuid()); break;
        case NPC_CORPSE_SCARAB:
            m_lCorpseScarabs.remove(pCreature->GetObjectGuid()); break;
        default: break;
    }
}

bool instance_naxxramas::IsEncounterInProgress() const
{
    for (uint8 i = 0; i <= TYPE_KELTHUZAD; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }

    // Some Encounters use SPECIAL while in progress
    return m_auiEncounter[TYPE_GOTHIK] == SPECIAL;
}

void instance_naxxramas::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ANUB_REKHAN:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ARAC_ANUB_DOOR);
            if (!m_lCorpseScarabs.empty())
                for (auto& creatureGuid : m_lCorpseScarabs)
                    if (Creature* add = instance->GetCreature(creatureGuid))
                        add->ForcedDespawn();
            m_lCorpseScarabs.clear();
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ARAC_ANUB_GATE);
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_MAEXXNA_ID);
            }
            break;
        case TYPE_FAERLINA:
            DoUseDoorOrButton(GO_ARAC_FAER_WEB);
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_YOU_OUT, true);
            else if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ARAC_FAER_DOOR);
                DoUseDoorOrButton(GO_ARAC_MAEX_OUTER_DOOR);
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MAEXXNA:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ARAC_MAEX_INNER_DOOR, uiData);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ARAC_EYE_RAMP);
                DoUseDoorOrButton(GO_ARAC_EYE_BOSS);
                DoRespawnGameObject(GO_ARAC_PORTAL, 30 * MINUTE);
                DoToggleGameObjectFlags(GO_ARAC_PORTAL, GO_FLAG_NO_INTERACT, false);
                m_uiTauntTimer = 5000;
            }
            break;
        case TYPE_NOTH:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_PLAG_NOTH_ENTRY_DOOR);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_PLAG_NOTH_EXIT_DOOR);
                DoUseDoorOrButton(GO_PLAG_HEIG_ENTRY_DOOR);
            }
            break;
        case TYPE_HEIGAN:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_PLAG_HEIG_ENTRY_DOOR);
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SAFETY_DANCE, true);
            else if (uiData == DONE)
                DoUseDoorOrButton(GO_PLAG_HEIG_EXIT_DOOR);
            break;
        case TYPE_LOATHEB:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_PLAG_LOAT_DOOR);
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SPORE_LOSER, true);
            else if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_PLAG_EYE_RAMP);
                DoUseDoorOrButton(GO_PLAG_EYE_BOSS);
                DoRespawnGameObject(GO_PLAG_PORTAL, 30 * MINUTE);
                DoToggleGameObjectFlags(GO_PLAG_PORTAL, GO_FLAG_NO_INTERACT, false);
                m_uiTauntTimer = 5000;
                if (!m_sHeiganBackroomAdds.empty())
                    for (auto& creatureGuid : m_sHeiganBackroomAdds)
                        if (Creature* add = instance->GetCreature(creatureGuid))
                            add->ForcedDespawn();
                m_sHeiganBackroomAdds.clear();
            }
            break;
        case TYPE_RAZUVIOUS:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GOTHIK:
            switch (uiData)
            {
                case IN_PROGRESS:
                    DoUseDoorOrButton(GO_MILI_GOTH_ENTRY_GATE);
                    DoUseDoorOrButton(GO_MILI_GOTH_COMBAT_GATE);
                    break;
                case SPECIAL:
                    DoUseDoorOrButton(GO_MILI_GOTH_COMBAT_GATE);
                    break;
                case FAIL:
                    if (m_auiEncounter[uiType] == IN_PROGRESS)
                        DoUseDoorOrButton(GO_MILI_GOTH_COMBAT_GATE);

                    DoUseDoorOrButton(GO_MILI_GOTH_ENTRY_GATE);
                    break;
                case DONE:
                    DoUseDoorOrButton(GO_MILI_GOTH_ENTRY_GATE);
                    DoUseDoorOrButton(GO_MILI_GOTH_EXIT_GATE);
                    DoUseDoorOrButton(GO_MILI_HORSEMEN_DOOR);

                    m_dialogueHelper.StartNextDialogueText(NPC_THANE);
                    break;
            }
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_FOUR_HORSEMEN:
            // Skip if already set
            if (m_auiEncounter[uiType] == uiData)
                return;

            if (uiData == SPECIAL)
            {
                // Start the achiev countdown
                if (!m_uiHorseMenKilled)
                    m_uiHorsemenAchievTimer = 15000;

                ++m_uiHorseMenKilled;

                if (m_uiHorseMenKilled == 4)
                    SetData(TYPE_FOUR_HORSEMEN, DONE);

                // Don't store special data
                return;
            }
            if (uiData == FAIL)
                m_uiHorseMenKilled = 0;
            else if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_MILI_EYE_RAMP);
                DoUseDoorOrButton(GO_MILI_EYE_BOSS);
                DoRespawnGameObject(GO_MILI_PORTAL, 30 * MINUTE);
                DoToggleGameObjectFlags(GO_MILI_PORTAL, GO_FLAG_NO_INTERACT, false);
                DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_CHEST_HORSEMEN_NORM : GO_CHEST_HORSEMEN_HERO, 30 * MINUTE);
                m_uiTauntTimer = 5000;
            }
            DoUseDoorOrButton(GO_MILI_HORSEMEN_DOOR);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_PATCHWERK:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_PATCHWERK_ID);
            else if (uiData == DONE)
                DoUseDoorOrButton(GO_CONS_PATH_EXIT_DOOR);
            break;
        case TYPE_GROBBULUS:
            m_auiEncounter[uiType] = uiData;
            for (const auto& cloud : m_lGrobbulusClouds)
            {
                if (Creature* cloud_creature = instance->GetCreature(cloud))
                {
                    cloud_creature->ForcedDespawn();
                }
            }
            break;
        case TYPE_GLUTH:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_CONS_GLUT_EXIT_DOOR);
                DoUseDoorOrButton(GO_CONS_THAD_DOOR);
            }
            break;
        case TYPE_THADDIUS:
            // Only process real changes here
            if (m_auiEncounter[uiType] == uiData)
                return;

            m_auiEncounter[uiType] = uiData;
            if (uiData != SPECIAL)
                DoUseDoorOrButton(GO_CONS_THAD_DOOR, uiData);
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SHOCKING, true);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_CONS_EYE_RAMP);
                DoUseDoorOrButton(GO_CONS_EYE_BOSS);
                DoRespawnGameObject(GO_CONS_PORTAL, 30 * MINUTE);
                DoToggleGameObjectFlags(GO_CONS_PORTAL, GO_FLAG_NO_INTERACT, false);
                m_uiTauntTimer = 5000;
            }
            break;
        case TYPE_SAPPHIRON:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_HUNDRED_CLUB, true);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_KELTHUZAD_WATERFALL_DOOR);
                m_dialogueHelper.StartNextDialogueText(NPC_KELTHUZAD);
            }
            // Start Sapph summoning process
            if (uiData == SPECIAL)
                m_uiSapphSpawnTimer = 22000;
            break;
        case TYPE_KELTHUZAD:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_KELTHUZAD_EXIT_DOOR);
            if (uiData == IN_PROGRESS)
            {
                SetSpecialAchievementCriteria(TYPE_ACHIEV_GET_ENOUGH, false);
                DoUseDoorOrButton(GO_KELTHUZAD_TRIGGER);
                m_despawnKTTriggerTimer = 5 * IN_MILLISECONDS;
            } else if (uiData == FAIL)
            {
                if (GameObject* trigger = GetSingleGameObjectFromStorage(GO_KELTHUZAD_TRIGGER))
                    trigger->Respawn();
            }
            break;
        case TYPE_UNDYING_FAILED:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE || (uiData == SPECIAL && uiType == TYPE_SAPPHIRON))
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
                   << m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14] << " " << m_auiEncounter[15];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_naxxramas::Load(const char* chrIn)
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
               >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
               >> m_auiEncounter[12] >> m_auiEncounter[13] >> m_auiEncounter[14] >> m_auiEncounter[15];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_naxxramas::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_naxxramas::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_naxxramas::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_SAFETY_DANCE_N:
        case ACHIEV_CRIT_SAFETY_DANCE_H:
            return m_abAchievCriteria[TYPE_ACHIEV_SAFETY_DANCE];
        case ACHIEV_CRIT_KNOCK_YOU_OUT_N:
        case ACHIEV_CRIT_KNOCK_YOU_OUT_H:
            return m_abAchievCriteria[TYPE_ACHIEV_KNOCK_YOU_OUT];
        case ACHIEV_CRIT_HUNDRED_CLUB_N:
        case ACHIEV_CRIT_HUNDRED_CLUB_H:
            return m_abAchievCriteria[TYPE_ACHIEV_HUNDRED_CLUB];
        case ACHIEV_CRIT_SHOCKING_N:
        case ACHIEV_CRIT_SHOCKING_H:
            return m_abAchievCriteria[TYPE_ACHIEV_SHOCKING];
        case ACHIEV_CRIT_SPORE_LOSER_N:
        case ACHIEV_CRIT_SPORE_LOSER_H:
            return m_abAchievCriteria[TYPE_ACHIEV_SPORE_LOSER];
        case ACHIEV_CRIT_GET_ENOUGH_N:
        case ACHIEV_CRIT_GET_ENOUGH_H:
            return m_abAchievCriteria[TYPE_ACHIEV_GET_ENOUGH];
        case ACHIEV_CRIT_TOGETHER_N:
        case ACHIEV_CRIT_TOGETHER_H:
            return m_uiHorsemenAchievTimer > 0;
        // 'The Immortal'(25m) or 'Undying'(10m) - (achievs 2186, 2187)
        case ACHIEV_CRIT_IMMORTAL_KEL:
        case ACHIEV_CRIT_IMMOORTAL_LOA:
        case ACHIEV_CRIT_IMMOORTAL_THAD:
        case ACHIEV_CRIT_IMMOORTAL_MAEX:
        case ACHIEV_CRIT_IMMOORTAL_HORSE:
        case ACHIEV_CRIT_UNDYING_KEL:
        case ACHIEV_CRIT_UNDYING_HORSE:
        case ACHIEV_CRIT_UNDYING_MAEX:
        case ACHIEV_CRIT_UNDYING_LOA:
        case ACHIEV_CRIT_UNDYING_THAD:
        {
            // First, check if all bosses are killed (except the last encounter)
            uint8 uiEncounterDone = 0;
            for (uint8 i = 0; i < TYPE_KELTHUZAD; ++i)
                if (m_auiEncounter[i] == DONE)
                    ++uiEncounterDone;

            return uiEncounterDone >= 14 && GetData(TYPE_UNDYING_FAILED) != DONE;
        }
        default:
            return false;
    }
}

void instance_naxxramas::Update(uint32 uiDiff)
{
    // Handle the continuous spawning of Living Poison blobs in Patchwerk corridor
    if (m_uiLivingPoisonTimer)
    {
        if (m_uiLivingPoisonTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
            {
                // Spawn 3 living poisons every 5 secs and make them cross the corridor and then despawn, for ever and ever
                for (uint8 i = 0; i < 3; i++)
                    if (Creature* pPoison = pPlayer->SummonCreature(NPC_LIVING_POISON, aLivingPoisonPositions[i].m_fX, aLivingPoisonPositions[i].m_fY, aLivingPoisonPositions[i].m_fZ, aLivingPoisonPositions[i].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0))
                    {
                        pPoison->GetMotionMaster()->MovePoint(0, aLivingPoisonPositions[i + 3].m_fX, aLivingPoisonPositions[i + 3].m_fY, aLivingPoisonPositions[i + 3].m_fZ);
                        pPoison->ForcedDespawn(15000);
                    }
            }
            m_uiLivingPoisonTimer = 5000;
        }
        else
            m_uiLivingPoisonTimer -= uiDiff;
    }

    if (m_despawnKTTriggerTimer)
    {
        if (m_despawnKTTriggerTimer < uiDiff)
        {
            if (GameObject* trigger = GetSingleGameObjectFromStorage(GO_KELTHUZAD_TRIGGER))
            {
                trigger->ResetDoorOrButton();
                trigger->SetLootState(GO_JUST_DEACTIVATED);
                trigger->SetForcedDespawn();
            }
            m_despawnKTTriggerTimer = 0;
        }
        else
            m_despawnKTTriggerTimer -= uiDiff;
    }

    if (m_uiScreamsTimer && m_auiEncounter[TYPE_THADDIUS] != DONE)
    {
        if (m_uiScreamsTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
                pPlayer->GetMap()->PlayDirectSoundToMap(SOUND_SCREAM1 + urand(0, 3));
            m_uiScreamsTimer = (2 * MINUTE + urand(0, 30)) * IN_MILLISECONDS;
        }
        else
            m_uiScreamsTimer -= uiDiff;
    }

    if (m_uiTauntTimer)
    {
        if (m_uiTauntTimer <= uiDiff)
        {
            DoTaunt();
            m_uiTauntTimer = 0;
        }
        else
            m_uiTauntTimer -= uiDiff;
    }

    if (m_uiHorsemenAchievTimer)
    {
        if (m_uiHorsemenAchievTimer <= uiDiff)
            m_uiHorsemenAchievTimer = 0;
        else
            m_uiHorsemenAchievTimer -= uiDiff;
    }

    if (m_uiSapphSpawnTimer)
    {
        if (m_uiSapphSpawnTimer <= uiDiff)
        {
            if (Player* pPlayer = GetPlayerInMap())
                pPlayer->SummonCreature(NPC_SAPPHIRON, aSapphPositions[0], aSapphPositions[1], aSapphPositions[2], aSapphPositions[3], TEMPSPAWN_DEAD_DESPAWN, 0);

            m_uiSapphSpawnTimer = 0;
        }
        else
            m_uiSapphSpawnTimer -= uiDiff;
    }

    m_dialogueHelper.DialogueUpdate(uiDiff);
}

const GuidVector instance_naxxramas::GetGluthTriggers()
{
    return m_gluthTriggerVector;
}

void instance_naxxramas::SetGothTriggers()
{
    Creature* pGoth = GetSingleCreatureFromStorage(NPC_GOTHIK);

    if (!pGoth)
        return;

    for (GuidList::const_iterator itr = m_lGothTriggerList.begin(); itr != m_lGothTriggerList.end(); ++itr)
    {
        if (Creature* pTrigger = instance->GetCreature(*itr))
        {
            GothTrigger pGt;
            pGt.bIsAnchorHigh = (pTrigger->GetPositionZ() >= (pGoth->GetPositionZ() - 5.0f));
            pGt.bIsRightSide = IsInRightSideGothArea(pTrigger);

            m_mGothTriggerMap[pTrigger->GetObjectGuid()] = pGt;
        }
    }
}

Creature* instance_naxxramas::GetClosestAnchorForGoth(Creature* pSource, bool bRightSide)
{
    std::list<Creature* > lList;

    for (auto& itr : m_mGothTriggerMap)
    {
        if (!itr.second.bIsAnchorHigh)
            continue;

        if (itr.second.bIsRightSide != bRightSide)
            continue;

        if (Creature* pCreature = instance->GetCreature(itr.first))
            lList.push_back(pCreature);
    }

    if (!lList.empty())
    {
        lList.sort(ObjectDistanceOrder(pSource));
        return lList.front();
    }

    return nullptr;
}

void instance_naxxramas::GetGothSummonPointCreatures(CreatureList& lList, bool bRightSide)
{
    for (auto& itr : m_mGothTriggerMap)
    {
        if (itr.second.bIsAnchorHigh)
            continue;

        if (itr.second.bIsRightSide != bRightSide)
            continue;

        if (Creature* pCreature = instance->GetCreature(itr.first))
            lList.push_back(pCreature);
    }
}

// Right is right side from gothik (eastern)
bool instance_naxxramas::IsInRightSideGothArea(Unit* pUnit)
{
    if (GameObject* pCombatGate = GetSingleGameObjectFromStorage(GO_MILI_GOTH_COMBAT_GATE))
        return (pCombatGate->GetPositionY() >= pUnit->GetPositionY());

    script_error_log("left/right side check, Gothik combat area failed.");
    return true;
}

void instance_naxxramas::DoTriggerHeiganTraps(Creature* pHeigan, uint32 uiAreaIndex)
{
    if (uiAreaIndex >= MAX_HEIGAN_TRAP_AREAS)
        return;

    for (GuidList::const_iterator itr = m_alHeiganTrapGuids[uiAreaIndex].begin(); itr != m_alHeiganTrapGuids[uiAreaIndex].end(); ++itr)
    {
        if (GameObject* pTrap = instance->GetGameObject(*itr))
            pTrap->Use(pHeigan);
    }
}

void instance_naxxramas::SetChamberCenterCoords(float fX, float fY, float fZ)
{
    m_fChamberCenterX = fX;
    m_fChamberCenterY = fY;
    m_fChamberCenterZ = fZ;
}

void instance_naxxramas::DoTaunt()
{
    if (m_auiEncounter[TYPE_KELTHUZAD] != DONE)
    {
        uint8 uiWingsCleared = 0;

        if (m_auiEncounter[TYPE_MAEXXNA] == DONE)
            ++uiWingsCleared;

        if (m_auiEncounter[TYPE_LOATHEB] == DONE)
            ++uiWingsCleared;

        if (m_auiEncounter[TYPE_FOUR_HORSEMEN] == DONE)
            ++uiWingsCleared;

        if (m_auiEncounter[TYPE_THADDIUS] == DONE)
            ++uiWingsCleared;

        switch (uiWingsCleared)
        {
            case 1: DoOrSimulateScriptTextForThisInstance(SAY_KELTHUZAD_TAUNT1, NPC_KELTHUZAD); break;
            case 2: DoOrSimulateScriptTextForThisInstance(SAY_KELTHUZAD_TAUNT2, NPC_KELTHUZAD); break;
            case 3: DoOrSimulateScriptTextForThisInstance(SAY_KELTHUZAD_TAUNT3, NPC_KELTHUZAD); break;
            case 4: DoOrSimulateScriptTextForThisInstance(SAY_KELTHUZAD_TAUNT4, NPC_KELTHUZAD); break;
        }
    }
}

InstanceData* GetInstanceData_instance_naxxramas(Map* pMap)
{
    return new instance_naxxramas(pMap);
}

struct Location3DPoint
{
    float x, y, z;
};

static const Location3DPoint gargoyleResetCoords = {2963.f, -3476.f, 297.6f};

enum
{
    SAY_GARGOYLE_NOISE      = -1533160, // %s emits a strange noise.

    SPELL_STONEFORM         = 29154,
    SPELL_STEALTH_DETECTION = 18950,
    SPELL_STONESKIN         = 28995,
    SPELL_ACID_VOLLEY       = 29325,
    SPELL_ACID_VOLLEY_25    = 54714,
};

struct npc_stoneskin_gargoyleAI : public ScriptedAI
{
    npc_stoneskin_gargoyleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
        {
            return x > gargoyleResetCoords.x && y > gargoyleResetCoords.y && z > gargoyleResetCoords.z;
        });
        Reset();
    }

    uint32 acidVolleyTimer;
    bool canCastVolley;

    void Reset() override
    {
        acidVolleyTimer = 4000;
        canCastVolley = false;
        TryStoneForm();

        DoCastSpellIfCan(m_creature, SPELL_STEALTH_DETECTION, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void TryStoneForm()
    {
        if (m_creature->GetDefaultMovementType() == IDLE_MOTION_TYPE)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STONEFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT) == CAST_OK)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->SetImmuneToPlayer(true);
            }
        }
    }

    void JustReachedHome() override
    {
        TryStoneForm();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
        {
            if (pWho->GetTypeId() == TYPEID_PLAYER
                && !m_creature->IsInCombat()
                && m_creature->IsWithinDistInMap(pWho, 17.0f)
                && !pWho->HasAuraType(SPELL_AURA_FEIGN_DEATH)
                && m_creature->IsWithinLOSInMap(pWho))
            {
                AttackStart(pWho);
            }
        }
        else
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit*) override
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_SPAWNING))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            m_creature->SetImmuneToPlayer(false);
        }

        // All Stoneskin Gargoyles cast Acid Volley but the first one encountered
        float respawnX, respawnY, respawnZ;
        m_creature->GetRespawnCoord(respawnX, respawnY, respawnZ);
        if (m_creature->GetDefaultMovementType() == IDLE_MOTION_TYPE || respawnZ < gargoyleResetCoords.z)
            canCastVolley = true;
    }

    void UpdateAI(uint32 const diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Stoneskin at 30% HP
        if (m_creature->GetHealthPercent() < 30.0f && !m_creature->HasAura(SPELL_STONESKIN))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STONESKIN) == CAST_OK)
                DoScriptText(SAY_GARGOYLE_NOISE, m_creature);
        }

        // Acid Volley
        if (canCastVolley)
        {
            if (acidVolleyTimer < diff)
            {
                if (DoCastSpellIfCan(m_creature, m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? SPELL_ACID_VOLLEY : SPELL_ACID_VOLLEY_25) == CAST_OK)
                    acidVolleyTimer = 8000;
            }
            else
                acidVolleyTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

/*###################
#   npc_living_poison
###################*/

struct npc_living_poisonAI : public ScriptedAI
{
    npc_living_poisonAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
        SetMeleeEnabled(false);
    }

    // Any time a player comes close to the Living Poison, it will explode and kill itself while doing heavy AoE damage to the player
    void MoveInLineOfSight(Unit* who) override
    {
        if (!who->IsPlayer() || m_creature->GetDistance2d(who->GetPositionX(), who->GetPositionY(), DIST_CALC_BOUNDING_RADIUS) > 4.0f)
            return;

        DoCastSpellIfCan(m_creature, SPELL_EXPLODE, CAST_TRIGGERED);
    }

    void AttackStart(Unit* /*who*/) override {}
};


bool AreaTrigger_at_naxxramas(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pAt->id == AREATRIGGER_KELTHUZAD)
    {
        if (pPlayer->IsGameMaster() || !pPlayer->IsAlive())
            return false;

        instance_naxxramas* pInstance = (instance_naxxramas*)pPlayer->GetInstanceData();

        if (!pInstance)
            return false;

        pInstance->SetChamberCenterCoords(pAt->x, pAt->y, pAt->z);

        if (pInstance->GetData(TYPE_KELTHUZAD) == NOT_STARTED)
        {
            if (Creature* pKelthuzad = pInstance->GetSingleCreatureFromStorage(NPC_KELTHUZAD))
            {
                if (pKelthuzad->IsAlive())
                {
                    pInstance->SetData(TYPE_KELTHUZAD, IN_PROGRESS);
                    pKelthuzad->SetInCombatWithZone();
                }
            }
        }
    }

    if (pAt->id == AREATRIGGER_THADDIUS_DOOR)
    {
        if (instance_naxxramas* pInstance = (instance_naxxramas*)pPlayer->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_THADDIUS) == NOT_STARTED)
            {
                if (Creature* pThaddius = pInstance->GetSingleCreatureFromStorage(NPC_THADDIUS))
                {
                    pInstance->SetData(TYPE_THADDIUS, SPECIAL);
                    DoScriptText(SAY_THADDIUS_GREET, pThaddius);
                }
            }
        }
    }

    return false;
}

void AddSC_instance_naxxramas()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_naxxramas";
    pNewScript->GetInstanceData = &GetInstanceData_instance_naxxramas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stoneskin_gargoyle";
    pNewScript->GetAI = &GetNewAIInstance<npc_stoneskin_gargoyleAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_poison";
    pNewScript->GetAI = &GetNewAIInstance<npc_living_poisonAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_naxxramas";
    pNewScript->pAreaTrigger = &AreaTrigger_at_naxxramas;
    pNewScript->RegisterSelf();
}
