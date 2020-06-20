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
SDName: mana_tombs
SD%Complete: 95
SDComment: Quest 10218
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "Entities/TemporarySpawn.h"

/*#####
## npc_shaheen
#####*/

enum
{
    SAY_ESCORT_START            = -1557015,
    SAY_START                   = -1557016,
    SAY_FIRST_STOP              = -1557017,
    SAY_FIRST_STOP_COMPLETE     = -1557018,
    SAY_COLLECTOR_SEARCH        = -1557019,
    SAY_COLLECTOR_FOUND         = -1557020,
    SAY_SECOND_STOP             = -1557021,
    SAY_THIRD_STOP              = -1557022,
    SAY_REST                    = -1557023,
    SAY_READY_GO                = -1557024,
    SAY_BREAK_OVER              = -1557025,
    SAY_XIRAXIS_SPAWN           = -1557026,
    SAY_FINAL_STOP_1            = -1557027,
    SAY_FINAL_STOP_2            = -1557028,
    SAY_FINAL_STOP_3            = -1557029,
    SAY_XIRAXIS_DEAD            = -1557030,
    SAY_EVENT_COMPLETE          = -1557031,

    EMOTE_TELEPORT              = -1557032,
    SAY_SPAWN                   = -1557033,
    EMOTE_PANEL                 = -1557034,
    SAY_ESCORT_READY            = -1557035,

    SPELL_ETHEREAL_TELEPORT     = 34427,

    NPC_ETHEREAL_SPELLBINDER    = 18312,
    NPC_ETHEREAL_THEURGIST      = 18315,
    NPC_ETHEREAL_SORCERER       = 18313,
    NPC_ETHEREAL_CRYPT_RAIDER   = 18311,
    NPC_NEXUS_TERROR            = 19307,
    NPC_ENGINEER_SHAEEN         = 19671,
    NPC_SHADOW_LORD_XIRAXIS     = 19666,

    QUEST_ID_HARD_WORK_PAYS_OFF = 10218
};

static const DialogueEntry aIntroDialogue[] =
{
    {SPELL_ETHEREAL_TELEPORT,   0,                       1000},
    {EMOTE_TELEPORT,            NPC_ENGINEER_SHAEEN,     3000},
    {NPC_ETHEREAL_SPELLBINDER,  0,                       5000},
    {EMOTE_PANEL,               NPC_ENGINEER_SHAEEN,     3000},
    {SAY_ESCORT_READY,          NPC_ENGINEER_SHAEEN,     0},
    {SAY_ESCORT_START,          NPC_ENGINEER_SHAEEN,     5000},
    {SAY_START,                 NPC_ENGINEER_SHAEEN,     0},
    {SAY_COLLECTOR_SEARCH,      NPC_ENGINEER_SHAEEN,     6000},
    {SAY_COLLECTOR_FOUND,       NPC_ENGINEER_SHAEEN,     0},
    {NPC_ETHEREAL_THEURGIST,    0,                       12000},
    {SAY_REST,                  NPC_ENGINEER_SHAEEN,     20000},
    {SAY_READY_GO,              NPC_ENGINEER_SHAEEN,     0},
    {SAY_FINAL_STOP_1,          NPC_SHADOW_LORD_XIRAXIS, 6000},
    {SAY_FINAL_STOP_2,          NPC_ENGINEER_SHAEEN,     4000},
    {SAY_FINAL_STOP_3,          NPC_SHADOW_LORD_XIRAXIS, 3000},
    {NPC_SHADOW_LORD_XIRAXIS,   0,                       0},
    {NPC_ENGINEER_SHAEEN,       0,                       3000},
    {SAY_XIRAXIS_DEAD,          NPC_ENGINEER_SHAEEN,     7000},
    {SAY_EVENT_COMPLETE,        NPC_ENGINEER_SHAEEN,     0},
    {0, 0, 0},
};

struct npc_shaheenAI : public npc_escortAI, private DialogueHelper
{
    npc_shaheenAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        StartNextDialogueText(SPELL_ETHEREAL_TELEPORT);
        Reset();
    }

    ObjectGuid m_xiraxisGuid;
    uint32 m_uiSummonCount;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
            StartNextDialogueText(SAY_ESCORT_START);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_SHADOW_LORD_XIRAXIS:
                m_xiraxisGuid = pSummoned->GetObjectGuid();
                DoScriptText(SAY_XIRAXIS_SPAWN, pSummoned);
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(1, -67.49f, -74.55f, -0.86f);
                break;
            default:
                pSummoned->AI()->AttackStart(m_creature);
            // no break;
            case NPC_NEXUS_TERROR:
                ++m_uiSummonCount;
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (pSummoned->GetEntry() == NPC_SHADOW_LORD_XIRAXIS && uiMotionType == POINT_MOTION_TYPE && uiPointId == 1)
            StartNextDialogueText(SAY_FINAL_STOP_1);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_SHADOW_LORD_XIRAXIS:
                StartNextDialogueText(NPC_ENGINEER_SHAEEN);
                m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                break;
            default:
                --m_uiSummonCount;
                if (!m_uiSummonCount)
                {
                    SetEscortPaused(false);
                    m_creature->HandleEmote(EMOTE_STATE_NONE);
                }
                break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 8:
                SetEscortPaused(true);
                DoScriptText(SAY_FIRST_STOP, m_creature);
                // summon first wave
                m_creature->SummonCreature(NPC_ETHEREAL_THEURGIST, -375.86f, -129.16f, -0.95f, 4.82f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_THEURGIST, -370.67f, -199.79f, -0.95f, 1.52f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_SPELLBINDER, -371.12f, -129.07f, -0.95f, 4.82f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_SPELLBINDER, -375.18f, -199.58f, -0.95f, 1.52f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 9:
                DoScriptText(SAY_FIRST_STOP_COMPLETE, m_creature);
                break;
            case 14:
                StartNextDialogueText(SAY_COLLECTOR_SEARCH);
                break;
            case 18:
                SetEscortPaused(true);
                DoScriptText(SAY_SECOND_STOP, m_creature);
                // summon second wave
                m_creature->SummonCreature(NPC_ETHEREAL_SORCERER, -278.17f, -195.50f, 0.68f, 1.22f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_SORCERER, -234.51f, -197.03f, -0.95f, 1.92f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_CRYPT_RAIDER, -282.28f, -194.44f, 0.44f, 1.22f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ETHEREAL_CRYPT_RAIDER, -231.15f, -194.53f, -0.95f, 1.52f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 25:
                // summon third wave
                m_creature->SummonCreature(NPC_NEXUS_TERROR, -37.30f, -222.44f, -0.33f, 3.01f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 240000);
                m_creature->SummonCreature(NPC_NEXUS_TERROR, -15.76f, -225.36f,  0.79f, 2.93f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 240000);
                break;
            case 27:
                if (m_uiSummonCount)
                    SetEscortPaused(true);
                DoScriptText(SAY_THIRD_STOP, m_creature);
                break;
            case 30:
                StartNextDialogueText(NPC_ETHEREAL_THEURGIST);
                m_creature->HandleEmote(EMOTE_STATE_WORK);
                break;
            case 31:
                DoScriptText(SAY_BREAK_OVER, m_creature);
                break;
            case 41:
                SetEscortPaused(true);
                m_creature->SummonCreature(NPC_SHADOW_LORD_XIRAXIS, -47.10f, -0.49f, -0.95f, 3.45f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 42:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_HARD_WORK_PAYS_OFF, m_creature);
                break;
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SPELL_ETHEREAL_TELEPORT:
                DoCastSpellIfCan(m_creature, SPELL_ETHEREAL_TELEPORT);
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                break;
            case NPC_ETHEREAL_SPELLBINDER:
                if (m_creature->IsTemporarySummon())
                {
                    if (Player* pSummoner = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                        DoScriptText(SAY_SPAWN, m_creature, pSummoner);
                }
                break;
            case SAY_ESCORT_READY:
                m_creature->HandleEmote(EMOTE_STATE_NONE);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                break;
            case SAY_REST:
                m_creature->HandleEmote(EMOTE_STATE_NONE);
                break;
            case NPC_SHADOW_LORD_XIRAXIS:
                if (Creature* pXiraxis = m_creature->GetMap()->GetCreature(m_xiraxisGuid))
                {
                    pXiraxis->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    pXiraxis->AI()->AttackStart(m_creature);
                }
                break;
            case SAY_EVENT_COMPLETE:
                SetRun();
                SetEscortPaused(false);
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
            case NPC_ENGINEER_SHAEEN:        return m_creature;
            case NPC_SHADOW_LORD_XIRAXIS:    return m_creature->GetMap()->GetCreature(m_xiraxisGuid);
            default:
                return nullptr;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // ToDo: add combat spells

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_shaheen(Creature* pCreature)
{
    return new npc_shaheenAI(pCreature);
}

bool QuestAccept_npc_shaheen(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_HARD_WORK_PAYS_OFF)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

void AddSC_mana_tombs()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_shaheen";
    pNewScript->GetAI = &GetAI_npc_shaheen;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_shaheen;
    pNewScript->RegisterSelf();
}
