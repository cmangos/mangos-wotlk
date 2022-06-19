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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"

enum
{
    // Svala intro dialogue
    SAY_INTRO_1                 = -1575000,
    SAY_INTRO_2_ARTHAS          = -1575001,
    SAY_INTRO_3                 = -1575002,
    SAY_INTRO_4_ARTHAS          = -1575003,
    SAY_INTRO_5                 = -1575004,

    SPELL_TRANSFORMING          = 54205,        // should also remove aura 54140 (script effect)
    SPELL_TRANSFORMING_FLOATING = 54140,        // followed by 54142
    SPELL_TRANSFORMING_CHANNEL  = 54142,
};

static const DialogueEntry aPinnacleDialogue[] =
{
    // Intro dialogue
    {NPC_ARTHAS_IMAGE,              0,                      1000},
    {SAY_INTRO_1,                   NPC_SVALA,              9000},
    {SAY_INTRO_2_ARTHAS,            NPC_ARTHAS_IMAGE,       10000},
    {SPELL_TRANSFORMING_CHANNEL,    0,                      1000},
    {NPC_SVALA,                     0,                      6000},
    {SPELL_TRANSFORMING_FLOATING,   0,                      4000},

    // Spawn undead Svala
    {SPELL_TRANSFORMING,            0,                      2000},
    {SAY_INTRO_3,                   NPC_SVALA_SORROWGRAVE,  11000},
    {SAY_INTRO_4_ARTHAS,            NPC_ARTHAS_IMAGE,       10000},
    {SAY_INTRO_5,                   NPC_SVALA_SORROWGRAVE,  13000},
    {NPC_SVALA_SORROWGRAVE,         0,                  0},
    {0, 0, 0},
};

instance_pinnacle::instance_pinnacle(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aPinnacleDialogue),
    m_uiGortokOrbTimer(0),
    m_uiGortokOrbPhase(0),
    m_uiGauntletCheckTimer(0),
    m_uiSkadiResetTimer(0)
{
    Initialize();
}

void instance_pinnacle::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    for (bool& i : m_abAchievCriteria)
        i = false;

    InitializeDialogueHelper(this);
}

void instance_pinnacle::OnPlayerEnter(Player* pPlayer)
{
    // summon undead Svala if intro is complete and the creature doesn't exist already
    if (GetSingleCreatureFromStorage(NPC_SVALA_SORROWGRAVE))
        return;

    if (GetData(TYPE_SVALA) == SPECIAL)
    {
        if (Creature* pSvala = pPlayer->SummonCreature(NPC_SVALA_SORROWGRAVE, aSvalaSpawnPos[0], aSvalaSpawnPos[1], aSvalaSpawnPos[2], aSvalaSpawnPos[3], TEMPSPAWN_DEAD_DESPAWN, 0))
        {
            pSvala->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
            pSvala->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        }

        // despawn the original Svala, in case it's still here
        if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA))
            pSvala->ForcedDespawn();
    }
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
        case NPC_ARTHAS_IMAGE:
        case NPC_SVALA:
        case NPC_SVALA_SORROWGRAVE:
        case NPC_WORLD_TRIGGER_LARGE:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
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
        case NPC_FLAME_BRAZIER:
            m_vbrazierVector.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_pinnacle::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_YMIRJAR_HARPOONER:
        case NPC_YMIRJAR_WARRIOR:
        case NPC_YMIRJAR_WITCH_DOCTOR:
            if (!pCreature->IsTemporarySummon())
                break;

            // creatures spawned for the gauntlet event have waypoint movement after 5 secs delay
            if (pCreature->GetSpawnerGuid().GetEntry() == NPC_WORLD_TRIGGER)
            {
                pCreature->SetWalk(false);
                pCreature->GetMotionMaster()->MoveWaypoint(0, 0, 5000);
            }
            break;
        case NPC_FLAME_BREATH_TRIGGER:
        case NPC_RITUAL_TARGET:
        case NPC_SPIRIT_FOUNT:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
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
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_pinnacle::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_SVALA:
            if (uiData == IN_PROGRESS || uiData == FAIL)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_INCREDIBLE_HULK, false);
            else if (uiData == SPECIAL)
                StartNextDialogueText(NPC_ARTHAS_IMAGE);
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_GORTOK:
            if (uiData == IN_PROGRESS)
            {
                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                {
                    pOrb->SetLevitate(true);
                    pOrb->CastSpell(pOrb, SPELL_ORB_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    pOrb->GetMotionMaster()->MovePoint(0, aOrbPositions[0][0], aOrbPositions[0][1], aOrbPositions[0][2]);

                    m_uiGortokOrbTimer = 2000;
                }
            }
            else if (uiData == FAIL)
            {
                if (Creature* pOrb = instance->GetCreature(m_gortokEventTriggerGuid))
                {
                    if (!pOrb->IsAlive())
                        pOrb->Respawn();
                    else
                        pOrb->RemoveAllAuras();

                    // For some reasone the Orb doesn't evade automatically
                    pOrb->GetMotionMaster()->MoveTargetedHome();
                }

                for (unsigned int aGortokMiniBosse : aGortokMiniBosses)
                {
                    // Reset each miniboss
                    if (Creature* pTemp = GetSingleCreatureFromStorage(aGortokMiniBosse))
                    {
                        if (!pTemp->IsAlive())
                            pTemp->Respawn();

                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
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
                    m_uiGauntletCheckTimer = 0;
                    break;
                case SPECIAL:
                    // Prepare achievements
                    SetSpecialAchievementCriteria(TYPE_ACHIEV_LOVE_SKADI, true);
                    DoStartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEV_START_SKADI_ID);

                    m_uiGauntletCheckTimer = 7000;
                    m_auiEncounter[uiType] = uiData;
                    return;
                case FAIL:
                {
                    // Handle event reset
                    if (Creature* pGrauf = GetSingleCreatureFromStorage(NPC_GRAUF))
                        pGrauf->ForcedDespawn();
                    if (Creature* pSkadi = GetSingleCreatureFromStorage(NPC_SKADI))
                        pSkadi->ForcedDespawn();

                    m_uiGauntletCheckTimer = 0;
                    m_uiSkadiResetTimer = 30000;

                    // remove periodic gauntlet spell on all players
                    Map::PlayerList const& lPlayers = instance->GetPlayers();
                    for (const auto& lPlayer : lPlayers)
                    {
                        if (Player* pPlayer = lPlayer.getSource())
                            pPlayer->RemoveAurasDueToSpell(SPELL_GAUNTLET_PERIODIC);
                    }
                }
                // no break;
                case NOT_STARTED:
                    // Despawn all summons
                    for (GuidList::const_iterator itr = m_lskadiGauntletMobsList.begin(); itr != m_lskadiGauntletMobsList.end(); ++itr)
                    {
                        if (Creature* pYmirjar = instance->GetCreature(*itr))
                            pYmirjar->ForcedDespawn();
                    }
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

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
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

void instance_pinnacle::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case NPC_ARTHAS_IMAGE:
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA))
                pSvala->SummonCreature(NPC_ARTHAS_IMAGE, aArthasSpawnPos[0], aArthasSpawnPos[1], aArthasSpawnPos[2], aArthasSpawnPos[3], TEMPSPAWN_TIMED_DESPAWN, 55000);
            break;
        case SPELL_TRANSFORMING_CHANNEL:
            if (Creature* pArthas = GetSingleCreatureFromStorage(NPC_ARTHAS_IMAGE))
                pArthas->CastSpell(pArthas, SPELL_TRANSFORMING_CHANNEL, TRIGGERED_NONE);
            break;
        case NPC_SVALA:
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA))
            {
                float fX, fZ, fY;
                pSvala->GetRespawnCoord(fX, fY, fZ);

                pSvala->SetLevitate(true);
                pSvala->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                pSvala->GetMotionMaster()->MovePoint(0, fX, fY, fZ + 5.0f);
            }
            break;
        case SPELL_TRANSFORMING_FLOATING:
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA))
                pSvala->CastSpell(pSvala, SPELL_TRANSFORMING_FLOATING, TRIGGERED_NONE);
            break;
        case SPELL_TRANSFORMING:
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA))
            {
                pSvala->CastSpell(pSvala, SPELL_TRANSFORMING, TRIGGERED_NONE);
                pSvala->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                pSvala->ForcedDespawn(2000);

                if (Creature* pBoss = pSvala->SummonCreature(NPC_SVALA_SORROWGRAVE, aSvalaSpawnPos[0], aSvalaSpawnPos[1], aSvalaSpawnPos[2], aSvalaSpawnPos[3], TEMPSPAWN_DEAD_DESPAWN, 0))
                    pBoss->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
            }
            break;
        case SAY_INTRO_5:
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA_SORROWGRAVE))
                pSvala->SetFacingTo(1.8325f);
            break;
        case NPC_SVALA_SORROWGRAVE:
            // Note: removal of the flight animation is handled by boss script
            if (Creature* pSvala = GetSingleCreatureFromStorage(NPC_SVALA_SORROWGRAVE))
            {
                // Remove immunity flag and attack random player
                pSvala->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);

                if (Player* pPlayer = GetPlayerInMap(true, false))
                    pSvala->AI()->AttackStart(pPlayer);
            }
            break;
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
        case NPC_GORTOK:
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
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
        case NPC_GORTOK:
            SetData(TYPE_GORTOK, DONE);
            break;
    }
}

void instance_pinnacle::Update(uint32 const uiDiff)
{
    DialogueUpdate(uiDiff);

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
                    pOrb->CastSpell(pOrb, uiSpellId, TRIGGERED_NONE);

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

    if (m_uiGauntletCheckTimer)
    {
        if (m_uiGauntletCheckTimer <= uiDiff)
        {
            if (Creature* pTrigger = GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER_LARGE))
                pTrigger->CastSpell(pTrigger, SPELL_GAUNTLET_RESET_CHECK, TRIGGERED_NONE);

            m_uiGauntletCheckTimer = 7000;
        }
        else
            m_uiGauntletCheckTimer -= uiDiff;
    }

    // Reset Skadi encounter if fail
    if (m_uiSkadiResetTimer)
    {
        if (m_uiSkadiResetTimer <= uiDiff)
        {
            if (Creature* pGrauf = GetSingleCreatureFromStorage(NPC_GRAUF))
                pGrauf->Respawn();

            if (Creature* pSkadi = GetSingleCreatureFromStorage(NPC_SKADI))
                pSkadi->Respawn();

            m_uiSkadiResetTimer = 0;
        }
        else
            m_uiSkadiResetTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

bool AreaTrigger_at_svala_intro(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (pPlayer->IsGameMaster())
        return false;

    if (instance_pinnacle* pInstance = (instance_pinnacle*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SVALA) == NOT_STARTED)
            pInstance->SetData(TYPE_SVALA, SPECIAL);
    }

    return false;
}

void AddSC_instance_pinnacle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_pinnacle";
    pNewScript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_svala_intro";
    pNewScript->pAreaTrigger = &AreaTrigger_at_svala_intro;
    pNewScript->RegisterSelf();
}
