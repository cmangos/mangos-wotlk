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
SDName: Instance_Arcatraz
SD%Complete: 80
SDComment: Mainly Harbringer Skyriss event
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "arcatraz.h"

/* Arcatraz encounters:
1 - Zereketh the Unbound event
2 - Dalliah the Doomsayer event
3 - Wrath-Scryer Soccothrates event
4 - Harbinger Skyriss event, 5 sub-events
*/

enum
{
    SAY_SOCCOTHRATES_AGGRO          = -1552039,
    SAY_SOCCOTHRATES_DEATH          = -1552043,

    YELL_MELLICHAR_INTRO1           = -1552023,
    YELL_MELLICHAR_INTRO2           = -1552024,
    YELL_MELLICHAR_RELEASE1         = -1552025,
    YELL_MELLICHAR_RELEASE2         = -1552026,
    YELL_MELLICHAR_RELEASE3         = -1552027,
    YELL_MELLICHAR_RELEASE4         = -1552028,
    YELL_MELLICHAR_RELEASE5         = -1552029,
    YELL_MELLICAR_WELCOME           = -1552030,
    SAY_SKYRISS_INTRO               = -1552000,
    SAY_SKYRISS_AGGRO               = -1552001,
    SAY_MILLHOUSE_COMPLETE          = -1552022,

    // Spells used by Mellichar during the dialogue
    SPELL_TARGET_BETA               = 36854,
    SPELL_TARGET_ALPHA              = 36856,
    SPELL_TARGET_DELTA              = 36857,
    SPELL_TARGET_GAMMA              = 36858,
    SPELL_SIMPLE_TELEPORT           = 12980, // TODO: Other mobs need to use it too
    SPELL_MIND_REND                 = 36859,
    SPELL_QUIET_SUICIDE             = 3617,

    QUEST_TRIAL_OF_THE_NAARU_TENACITY = 10886,
};

static const DialogueEntry aArcatrazDialogue[] =
{
    // Soccothares taunts
    {TYPE_DALLIAH,            0,             5000},
    {SAY_SOCCOTHRATES_AGGRO,  NPC_SOCCOTHRATES, 0},
    {TYPE_SOCCOTHRATES,       0,             5000},
    {SAY_SOCCOTHRATES_DEATH,  NPC_SOCCOTHRATES, 0},
    // Skyriss event
    {YELL_MELLICHAR_INTRO1,   NPC_MELLICHAR, 22000},
    {YELL_MELLICHAR_INTRO2,   NPC_MELLICHAR, 7000},
    {SPELL_TARGET_ALPHA,      0,             7000},
    {YELL_MELLICHAR_RELEASE1, NPC_MELLICHAR, 0},
    {YELL_MELLICHAR_RELEASE2, NPC_MELLICHAR, 7000},
    {SPELL_TARGET_BETA,       0,             7000},
    {TYPE_WARDEN_2,           0,             0},
    {YELL_MELLICHAR_RELEASE3, NPC_MELLICHAR, 7000},
    {SPELL_TARGET_DELTA,      0,             7000},
    {TYPE_WARDEN_3,           0,             0},
    {YELL_MELLICHAR_RELEASE4, NPC_MELLICHAR, 7000},
    {SPELL_TARGET_GAMMA,      0,             7000},
    {TYPE_WARDEN_4,           0,             0},
    {YELL_MELLICHAR_RELEASE5, NPC_MELLICHAR, 8000},
    {TYPE_WARDEN_5,           0,             5000},
    {SAY_SKYRISS_INTRO,       NPC_SKYRISS,   25000},
    {YELL_MELLICAR_WELCOME,   NPC_MELLICHAR, 3000},
    {SAY_SKYRISS_AGGRO,       NPC_SKYRISS,   0},
    {0, 0, 0},
};

instance_arcatraz::instance_arcatraz(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aArcatrazDialogue),
    m_uiResetDelayTimer(0),
    m_uiEntranceEventTimer(0),
    m_uiKilledWarders(0),
    m_uiKilledDefenders(0)
{
    Initialize();
}

void instance_arcatraz::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
}

void instance_arcatraz::OnPlayerEnter(Player* /*pPlayer*/)
{
    // Check encounter states
    if (GetData(TYPE_ENTRANCE) == DONE || GetData(TYPE_ENTRANCE) == IN_PROGRESS)
        return;

    SetData(TYPE_ENTRANCE, IN_PROGRESS);
    m_uiEntranceEventTimer = 1000;
}

void instance_arcatraz::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CORE_SECURITY_FIELD_ALPHA:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CORE_SECURITY_FIELD_BETA:
            if (m_auiEncounter[1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SEAL_SPHERE:
        case GO_POD_ALPHA:
        case GO_POD_BETA:
        case GO_POD_DELTA:
        case GO_POD_GAMMA:
        case GO_POD_OMEGA:
            break;
        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_arcatraz::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SKYRISS:
        case NPC_MILLHOUSE:
            m_lSkyrissEventMobsGuidList.push_back(pCreature->GetObjectGuid());
        // no break here because we want them in both lists
        case NPC_PRISON_APHPA_POD:
        case NPC_PRISON_BETA_POD:
        case NPC_PRISON_DELTA_POD:
        case NPC_PRISON_GAMMA_POD:
        case NPC_PRISON_BOSS_POD:
        case NPC_MELLICHAR:
        case NPC_DALLIAH:
        case NPC_SOCCOTHRATES:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_WRATH_SCRYER_FELFIRE:
            m_npcEntryGuidCollection[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
            break;
        case NPC_BLAZING_TRICKSTER:
        case NPC_PHASE_HUNTER:
        case NPC_AKKIRIS:
        case NPC_SULFURON:
        case NPC_TW_DRAKONAAR:
        case NPC_BL_DRAKONAAR:
            m_lSkyrissEventMobsGuidList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_arcatraz::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ENTRANCE:
        case TYPE_ZEREKETH:
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_DALLIAH:
            if (uiData == IN_PROGRESS)
            {
                // Soccothares taunts after Dalliah gets aggro
                if (GetData(TYPE_SOCCOTHRATES) != DONE)
                    StartNextDialogueText(TYPE_DALLIAH);
            }
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_CORE_SECURITY_FIELD_BETA);

                // Soccothares taunts after Dalliah dies
                if (GetData(TYPE_SOCCOTHRATES) != DONE)
                    StartNextDialogueText(TYPE_SOCCOTHRATES);
            }
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_SOCCOTHRATES:
            if (uiData != IN_PROGRESS)
            {
                GuidVector felfireVector; // at aggro felfire mobs always teleport to respawn location
                GetCreatureGuidVectorFromStorage(NPC_WRATH_SCRYER_FELFIRE, felfireVector);
                for (ObjectGuid& guid : felfireVector)
                {
                    if (Creature* creature = instance->GetCreature(guid))
                    {
                        creature->RemoveAllDynObjects();
                        creature->CombatStop();
                    }
                }
            }
            if (uiData == DONE)
                DoUseDoorOrButton(GO_CORE_SECURITY_FIELD_ALPHA);
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_HARBINGERSKYRISS:
            if (uiData == FAIL)
            {
                SetData(TYPE_WARDEN_1, NOT_STARTED);
                SetData(TYPE_WARDEN_2, NOT_STARTED);
                SetData(TYPE_WARDEN_3, NOT_STARTED);
                SetData(TYPE_WARDEN_4, NOT_STARTED);
                SetData(TYPE_WARDEN_5, NOT_STARTED);

                // Reset event in 1 min
                if (Creature* pMellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR))
                    pMellichar->ForcedDespawn();
                m_uiResetDelayTimer = 60000;

                // Despawn all the summons manually
                for (GuidList::const_iterator itr = m_lSkyrissEventMobsGuidList.begin(); itr != m_lSkyrissEventMobsGuidList.end(); ++itr)
                {
                    if (Creature* pTemp = instance->GetCreature(*itr))
                        pTemp->ForcedDespawn();
                }

                // Reset these objects, because they doesn't reset automatically
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_POD_BETA))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_POD_OMEGA))
                    pGo->ResetDoorOrButton();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_SEAL_SPHERE))
                    pGo->ResetDoorOrButton();
            }
            if (uiData == IN_PROGRESS)
            {
                StartNextDialogueText(YELL_MELLICHAR_INTRO1);
                DoUseDoorOrButton(GO_SEAL_SPHERE);
            }
            if (uiData == DONE)
            {
                if (Creature* pMillhouse = GetSingleCreatureFromStorage(NPC_MILLHOUSE))
                {
                    DoScriptText(SAY_MILLHOUSE_COMPLETE, pMillhouse);
                    if (!instance->IsRegularDifficulty())
                    {
                        Map::PlayerList const& PlayerList = instance->GetPlayers();

                        for (const auto& itr : PlayerList)
                        {
                            Player* pPlayer = itr.getSource();
                            if (pPlayer && pPlayer->GetQuestStatus(QUEST_TRIAL_OF_THE_NAARU_TENACITY) == QUEST_STATUS_INCOMPLETE)
                                pPlayer->AreaExploredOrEventHappens(QUEST_TRIAL_OF_THE_NAARU_TENACITY);
                        }
                    }
                }
            }
            m_auiEncounter[3] = uiData;
            break;

        case TYPE_WARDEN_1:
            if (uiData == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_ALPHA);
            if (uiData == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE2);
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_WARDEN_2:
            if (uiData == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_BETA);
            if (uiData == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE3);
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_WARDEN_3:
            if (uiData == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_DELTA);
            if (uiData == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE4);
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_WARDEN_4:
            if (uiData == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_GAMMA);
            if (uiData == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE5);
            m_auiEncounter[uiType] = uiData;
            break;

        case TYPE_WARDEN_5:
            if (uiData == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_OMEGA);
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_arcatraz::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_arcatraz::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_arcatraz::JustDidDialogueStep(int32 iEntry)
{
    Creature* pMellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR);
    if (!pMellichar)
        return;

    switch (iEntry)
    {
        case SPELL_TARGET_ALPHA:
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_ALPHA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_APHPA_POD))
                pMellichar->SetFacingToObject(pTarget);
            SetData(TYPE_WARDEN_1, IN_PROGRESS);
            break;
        case YELL_MELLICHAR_RELEASE1:
            pMellichar->SummonCreature(urand(0, 1) ? NPC_BLAZING_TRICKSTER : NPC_PHASE_HUNTER, aSummonPosition[0].m_fX, aSummonPosition[0].m_fY, aSummonPosition[0].m_fZ, aSummonPosition[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case YELL_MELLICHAR_RELEASE2:
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_BETA_POD))
                pMellichar->SetFacingToObject(pTarget);
            break;
        case SPELL_TARGET_BETA:
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_BETA, TRIGGERED_NONE);
            SetData(TYPE_WARDEN_2, IN_PROGRESS);
            break;
        case TYPE_WARDEN_2:
            pMellichar->SummonCreature(NPC_MILLHOUSE, aSummonPosition[1].m_fX, aSummonPosition[1].m_fY, aSummonPosition[1].m_fZ, aSummonPosition[1].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case SPELL_TARGET_DELTA:
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_DELTA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_DELTA_POD))
                pMellichar->SetFacingToObject(pTarget);
            SetData(TYPE_WARDEN_3, IN_PROGRESS);
            break;
        case TYPE_WARDEN_3:
            pMellichar->SummonCreature(urand(0, 1) ? NPC_AKKIRIS : NPC_SULFURON, aSummonPosition[2].m_fX, aSummonPosition[2].m_fY, aSummonPosition[2].m_fZ, aSummonPosition[2].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_OMEGA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_BOSS_POD))
                pMellichar->SetFacingToObject(pTarget);
            break;
        case YELL_MELLICHAR_RELEASE4:
            pMellichar->InterruptNonMeleeSpells(false);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_GAMMA_POD))
                pMellichar->SetFacingToObject(pTarget);
            break;
        case SPELL_TARGET_GAMMA:
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_GAMMA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_GAMMA_POD))
                pMellichar->SetFacingToObject(pTarget);
            SetData(TYPE_WARDEN_4, IN_PROGRESS);
            break;
        case TYPE_WARDEN_4:
            pMellichar->SummonCreature(urand(0, 1) ? NPC_TW_DRAKONAAR : NPC_BL_DRAKONAAR, aSummonPosition[3].m_fX, aSummonPosition[3].m_fY, aSummonPosition[3].m_fZ, aSummonPosition[3].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            pMellichar->CastSpell(pMellichar, SPELL_TARGET_OMEGA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_BOSS_POD))
                pMellichar->SetFacingToObject(pTarget);
            break;
        case YELL_MELLICHAR_RELEASE5:
            pMellichar->InterruptNonMeleeSpells(false);
            SetData(TYPE_WARDEN_5, IN_PROGRESS);
            break;
        case TYPE_WARDEN_5:
            if (Creature* pSkyriss = pMellichar->SummonCreature(NPC_SKYRISS, aSummonPosition[4].m_fX, aSummonPosition[4].m_fY, aSummonPosition[4].m_fZ, aSummonPosition[4].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0))
                pSkyriss->CastSpell(pSkyriss, SPELL_SIMPLE_TELEPORT, TRIGGERED_NONE);
            break;
        case YELL_MELLICAR_WELCOME:
            if (Creature* pSkyriss = GetSingleCreatureFromStorage(NPC_SKYRISS))
                pSkyriss->CastSpell(pSkyriss, SPELL_MIND_REND, TRIGGERED_NONE);
            break;
        case SAY_SKYRISS_AGGRO:
            // Kill Mellichar and start combat
            if (Creature* pSkyriss = GetSingleCreatureFromStorage(NPC_SKYRISS))
            {
                pSkyriss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                pSkyriss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                pMellichar->CastSpell(pMellichar, SPELL_QUIET_SUICIDE, TRIGGERED_NONE);
            }
            DoUseDoorOrButton(GO_SEAL_SPHERE);
            break;
    }
}

void instance_arcatraz::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ARCATRAZ_WARDER:
            ++m_uiKilledWarders;
            break;
        case NPC_ARCATRAZ_DEFENDER:
            ++m_uiKilledDefenders;
            break;
        case NPC_PROTEAN_NIGHTMARE:
        case NPC_PROTEAN_HORROR:
            // intro Protean spawn corpses should despawn after about 5 seconds
            pCreature->ForcedDespawn(5000);
            break;
    }

    // Stop the intro spawns when the wardens are killed
    if (m_uiKilledDefenders == MAX_DEFENDERS && m_uiKilledWarders == MAX_WARDERS)
    {
        SetData(TYPE_ENTRANCE, DONE);
        m_uiEntranceEventTimer = 0;
    }
}

void instance_arcatraz::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    if (m_uiResetDelayTimer)
    {
        if (m_uiResetDelayTimer <= uiDiff)
        {
            if (Creature* pMellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR))
                pMellichar->Respawn();
            m_uiResetDelayTimer = 0;
        }
        else
            m_uiResetDelayTimer -= uiDiff;
    }

    if (m_uiEntranceEventTimer)
    {
        if (m_uiEntranceEventTimer <= uiDiff)
        {
            Player* pPlayer = GetPlayerInMap();
            if (!pPlayer)
                return;

            uint32 uiEntry = urand(0, 10) ? NPC_PROTEAN_HORROR : NPC_PROTEAN_NIGHTMARE;

            // Protean Horrors stop spawning once 4 warders are killed
            // Protean Nightmares stop spawning once 3 defenders are killed
            if ((uiEntry == NPC_PROTEAN_HORROR && m_uiKilledWarders == MAX_WARDERS) || (uiEntry == NPC_PROTEAN_NIGHTMARE && m_uiKilledDefenders == MAX_DEFENDERS))
            {
                m_uiEntranceEventTimer = urand(0, 10) ? urand(2000, 3500) : urand(5000, 7000);
                return;
            }

            // Summon and move the intro creatures into combat positions
            if (Creature* pTemp = pPlayer->SummonCreature(uiEntry, aEntranceSpawnLoc[0], aEntranceSpawnLoc[1], aEntranceSpawnLoc[2], aEntranceSpawnLoc[3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000))
            {
                pTemp->GetMotionMaster()->MoveWaypoint(1);
            }
            m_uiEntranceEventTimer = urand(0, 10) ? urand(2000, 3500) : urand(5000, 7000);
        }
        else
            m_uiEntranceEventTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_arcatraz(Map* pMap)
{
    return new instance_arcatraz(pMap);
}

void AddSC_instance_arcatraz()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_arcatraz";
    pNewScript->GetInstanceData = &GetInstanceData_instance_arcatraz;
    pNewScript->RegisterSelf();
}
