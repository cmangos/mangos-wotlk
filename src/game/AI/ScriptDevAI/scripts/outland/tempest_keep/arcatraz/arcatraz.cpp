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
    SAY_SOCCOTHRATES_AGGRO          = 19968,
    SAY_SOCCOTHRATES_DEATH          = 19977,

    YELL_MELLICHAR_INTRO1           = 19103,
    YELL_MELLICHAR_INTRO2           = 19104,
    YELL_MELLICHAR_RELEASE1         = 19106,
    YELL_MELLICHAR_RELEASE2         = 19107,
    YELL_MELLICHAR_RELEASE3         = 19109,
    YELL_MELLICHAR_RELEASE4         = 19110,
    YELL_MELLICHAR_RELEASE5         = 19111,
    YELL_MELLICAR_WELCOME           = 19114,
    SAY_SKYRISS_INTRO               = 19112,
    SAY_SKYRISS_AGGRO               = 19113,
    SAY_MILLHOUSE_COMPLETE          = 19146,

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

instance_arcatraz::instance_arcatraz(Map* map) : ScriptedInstance(map), DialogueHelper(aArcatrazDialogue),
    m_resetDelayTimer(0), m_WardenGroup(false)
{
    Initialize();
}

void instance_arcatraz::Initialize()
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_PROTEAN_HORROR, 1);
    instance->GetVariableManager().SetVariable(WORLD_STATE_PROTEAN_NIGHTMARE, 1);
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    InitializeDialogueHelper(this);
}

void instance_arcatraz::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_CORE_SECURITY_FIELD_ALPHA:
            if (m_auiEncounter[2] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CORE_SECURITY_FIELD_BETA:
            if (m_auiEncounter[1] == DONE)
                go->SetGoState(GO_STATE_ACTIVE);
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
    m_goEntryGuidStore[go->GetEntry()] = go->GetObjectGuid();
}

void instance_arcatraz::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_SKYRISS:
        case NPC_MILLHOUSE:
            m_skyrissEventMobsGuids.push_back(creature->GetObjectGuid());
            [[fallthrough]];
        // no break here because we want them in both lists
        case NPC_PRISON_APHPA_POD:
        case NPC_PRISON_BETA_POD:
        case NPC_PRISON_DELTA_POD:
        case NPC_PRISON_GAMMA_POD:
        case NPC_PRISON_BOSS_POD:
        case NPC_MELLICHAR:
        case NPC_DALLIAH:
        case NPC_SOCCOTHRATES:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_WRATH_SCRYER_FELFIRE:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            break;
        case NPC_BLAZING_TRICKSTER:
        case NPC_PHASE_HUNTER:
        case NPC_AKKIRIS:
        case NPC_SULFURON:
        case NPC_TW_DRAKONAAR:
        case NPC_BL_DRAKONAAR:
            m_skyrissEventMobsGuids.push_back(creature->GetObjectGuid());
            break;
    }
}
void instance_arcatraz::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_PROTEAN_NIGHTMARE:
        case NPC_PROTEAN_HORROR:
            if (creature->HasStringId(STRING_ID_ENTRANCE_GROUP))
            {
                creature->SetNoXP(true);
                creature->SetNoLoot(true);
                creature->SetNoReputation(true);
            }
        break;
    }
}

// Intro Handling
// When both Arcatraz Warder spawn_groups are dead, Protean Horror stop spawning
// When Arcatraz Defender spawn_group is dead, Protean Nightmare stop spawning
void instance_arcatraz::OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* /*pCreature*/)
{
    if (pGroup->GetGroupId() == SPAWN_GROUP_WARDEN_01)
    {
        if (m_WardenGroup)
            instance->GetVariableManager().SetVariable(WORLD_STATE_PROTEAN_HORROR, 0);
        else
            m_WardenGroup = true;
    }

    if (pGroup->GetGroupId() == SPAWN_GROUP_WARDEN_02)
    {
        if (m_WardenGroup)
    
            instance->GetVariableManager().SetVariable(WORLD_STATE_PROTEAN_HORROR, 0);
        else
            m_WardenGroup = true;
    }

    if (pGroup->GetGroupId() == SPAWN_GROUP_DEFENDER)
        instance->GetVariableManager().SetVariable(WORLD_STATE_PROTEAN_NIGHTMARE, 0);
}

void instance_arcatraz::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_ZEREKETH:
            m_auiEncounter[type] = data;
            break;
        case TYPE_DALLIAH:
            if (data == IN_PROGRESS)
            {
                // Soccothares taunts after Dalliah gets aggro
                if (GetData(TYPE_SOCCOTHRATES) != DONE)
                    StartNextDialogueText(TYPE_DALLIAH);
            }
            if (data == DONE)
            {
                DoUseDoorOrButton(GO_CORE_SECURITY_FIELD_BETA);

                // Soccothares taunts after Dalliah dies
                if (GetData(TYPE_SOCCOTHRATES) != DONE)
                    StartNextDialogueText(TYPE_SOCCOTHRATES);
            }
            m_auiEncounter[type] = data;
            break;
        case TYPE_SOCCOTHRATES:
            if (data != IN_PROGRESS)
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
            if (data == DONE)
                DoUseDoorOrButton(GO_CORE_SECURITY_FIELD_ALPHA);
            m_auiEncounter[type] = data;
            break;
        case TYPE_HARBINGERSKYRISS:
            if (data == FAIL)
            {
                SetData(TYPE_WARDEN_1, NOT_STARTED);
                SetData(TYPE_WARDEN_2, NOT_STARTED);
                SetData(TYPE_WARDEN_3, NOT_STARTED);
                SetData(TYPE_WARDEN_4, NOT_STARTED);
                SetData(TYPE_WARDEN_5, NOT_STARTED);

                // Reset event in 1 min
                if (Creature* mellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR))
                    mellichar->ForcedDespawn();
                m_resetDelayTimer = 60000;

                // Despawn all the summons manually
                DespawnGuids(m_skyrissEventMobsGuids);

                // Reset these objects, because they doesn't reset automatically
                if (GameObject* go = GetSingleGameObjectFromStorage(GO_POD_BETA))
                    go->ResetDoorOrButton();
                if (GameObject* go = GetSingleGameObjectFromStorage(GO_POD_OMEGA))
                    go->ResetDoorOrButton();
                if (GameObject* go = GetSingleGameObjectFromStorage(GO_SEAL_SPHERE))
                    go->ResetDoorOrButton();
            }
            if (data == IN_PROGRESS)
            {
                StartNextDialogueText(YELL_MELLICHAR_INTRO1);
                DoUseDoorOrButton(GO_SEAL_SPHERE);
            }
            if (data == DONE)
            {
                if (Creature* millhouse = GetSingleCreatureFromStorage(NPC_MILLHOUSE))
                {
                    DoBroadcastText(SAY_MILLHOUSE_COMPLETE, millhouse);
                    if (!instance->IsRegularDifficulty())
                    {
                        Map::PlayerList const& PlayerList = instance->GetPlayers();

                        for (const auto& itr : PlayerList)
                        {
                            Player* player = itr.getSource();
                            if (player && player->GetQuestStatus(QUEST_TRIAL_OF_THE_NAARU_TENACITY) == QUEST_STATUS_INCOMPLETE)
                                player->AreaExploredOrEventHappens(QUEST_TRIAL_OF_THE_NAARU_TENACITY);
                        }
                    }
                }
            }
            m_auiEncounter[3] = data;
            break;
        case TYPE_WARDEN_1:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_ALPHA);
            if (data == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE2);
            m_auiEncounter[type] = data;
            break;
        case TYPE_WARDEN_2:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_BETA);
            if (data == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE3);
            m_auiEncounter[type] = data;
            break;
        case TYPE_WARDEN_3:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_DELTA);
            if (data == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE4);
            m_auiEncounter[type] = data;
            break;
        case TYPE_WARDEN_4:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_GAMMA);
            if (data == DONE)
                StartNextDialogueText(YELL_MELLICHAR_RELEASE5);
            m_auiEncounter[type] = data;
            break;
        case TYPE_WARDEN_5:
            if (data == IN_PROGRESS)
                DoUseDoorOrButton(GO_POD_OMEGA);
            m_auiEncounter[type] = data;
            break;
    }

    if (data == DONE)
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

uint32 instance_arcatraz::GetData(uint32 type) const
{
    if (type < MAX_ENCOUNTER)
        return m_auiEncounter[type];

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

void instance_arcatraz::JustDidDialogueStep(int32 dialogueEntry)
{
    Creature* mellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR);
    if (!mellichar)
        return;

    switch (dialogueEntry)
    {
        case SPELL_TARGET_ALPHA:
            mellichar->CastSpell(nullptr, SPELL_TARGET_ALPHA, TRIGGERED_NONE);
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_APHPA_POD))
                mellichar->SetFacingToObject(target);
            SetData(TYPE_WARDEN_1, IN_PROGRESS);
            break;
        case YELL_MELLICHAR_RELEASE1:
            mellichar->SummonCreature(urand(0, 1) ? NPC_BLAZING_TRICKSTER : NPC_PHASE_HUNTER, aSummonPosition[0].m_fX, aSummonPosition[0].m_fY, aSummonPosition[0].m_fZ, aSummonPosition[0].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case YELL_MELLICHAR_RELEASE2:
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_BETA_POD))
                mellichar->SetFacingToObject(target);
            break;
        case SPELL_TARGET_BETA:
            mellichar->CastSpell(nullptr, SPELL_TARGET_BETA, TRIGGERED_NONE);
            SetData(TYPE_WARDEN_2, IN_PROGRESS);
            break;
        case TYPE_WARDEN_2:
            mellichar->SummonCreature(NPC_MILLHOUSE, aSummonPosition[1].m_fX, aSummonPosition[1].m_fY, aSummonPosition[1].m_fZ, aSummonPosition[1].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            break;
        case SPELL_TARGET_DELTA:
            mellichar->CastSpell(nullptr, SPELL_TARGET_DELTA, TRIGGERED_NONE);
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_DELTA_POD))
                mellichar->SetFacingToObject(target);
            SetData(TYPE_WARDEN_3, IN_PROGRESS);
            break;
        case TYPE_WARDEN_3:
            mellichar->SummonCreature(urand(0, 1) ? NPC_AKKIRIS : NPC_SULFURON, aSummonPosition[2].m_fX, aSummonPosition[2].m_fY, aSummonPosition[2].m_fZ, aSummonPosition[2].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            mellichar->CastSpell(nullptr, SPELL_TARGET_OMEGA, TRIGGERED_NONE);
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_BOSS_POD))
                mellichar->SetFacingToObject(target);
            break;
        case YELL_MELLICHAR_RELEASE4:
            mellichar->InterruptNonMeleeSpells(false);
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_GAMMA_POD))
                mellichar->SetFacingToObject(target);
            break;
        case SPELL_TARGET_GAMMA:
            mellichar->CastSpell(nullptr, SPELL_TARGET_GAMMA, TRIGGERED_NONE);
            if (Creature* target = GetSingleCreatureFromStorage(NPC_PRISON_GAMMA_POD))
                mellichar->SetFacingToObject(target);
            SetData(TYPE_WARDEN_4, IN_PROGRESS);
            break;
        case TYPE_WARDEN_4:
            mellichar->SummonCreature(urand(0, 1) ? NPC_TW_DRAKONAAR : NPC_BL_DRAKONAAR, aSummonPosition[3].m_fX, aSummonPosition[3].m_fY, aSummonPosition[3].m_fZ, aSummonPosition[3].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0);
            mellichar->CastSpell(nullptr, SPELL_TARGET_OMEGA, TRIGGERED_NONE);
            if (Creature* pTarget = GetSingleCreatureFromStorage(NPC_PRISON_BOSS_POD))
                mellichar->SetFacingToObject(pTarget);
            break;
        case YELL_MELLICHAR_RELEASE5:
            mellichar->InterruptNonMeleeSpells(false);
            SetData(TYPE_WARDEN_5, IN_PROGRESS);
            break;
        case TYPE_WARDEN_5:
            if (Creature* skyriss = mellichar->SummonCreature(NPC_SKYRISS, aSummonPosition[4].m_fX, aSummonPosition[4].m_fY, aSummonPosition[4].m_fZ, aSummonPosition[4].m_fO, TEMPSPAWN_DEAD_DESPAWN, 0))
                skyriss->CastSpell(nullptr, SPELL_SIMPLE_TELEPORT, TRIGGERED_NONE);
            break;
        case YELL_MELLICAR_WELCOME:
            if (Creature* skyriss = GetSingleCreatureFromStorage(NPC_SKYRISS))
                skyriss->CastSpell(nullptr, SPELL_MIND_REND, TRIGGERED_NONE);
            break;
        case SAY_SKYRISS_AGGRO:
            // Kill Mellichar and start combat
            if (Creature* skyriss = GetSingleCreatureFromStorage(NPC_SKYRISS))
            {
                skyriss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                skyriss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                mellichar->CastSpell(nullptr, SPELL_QUIET_SUICIDE, TRIGGERED_NONE);
            }
            DoUseDoorOrButton(GO_SEAL_SPHERE);
            break;
    }
}

void instance_arcatraz::Update(uint32 diff)
{
    DialogueUpdate(diff);

    if (m_resetDelayTimer)
    {
        if (m_resetDelayTimer <= diff)
        {
            if (Creature* mellichar = GetSingleCreatureFromStorage(NPC_MELLICHAR))
                mellichar->Respawn();
            m_resetDelayTimer = 0;
        }
        else
            m_resetDelayTimer -= diff;
    }
}

void AddSC_instance_arcatraz()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_arcatraz";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_arcatraz>;
    pNewScript->RegisterSelf();
}
