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
SDName: Halls_of_Stone
SD%Complete: 100%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_stone.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_KILL_1                          = -1599012,
    SAY_KILL_2                          = -1599013,
    SAY_KILL_3                          = -1599014,

    SAY_LOW_HEALTH                      = -1599015,
    SAY_DEATH                           = -1599016,

    SAY_PLAYER_DEATH_1                  = -1599017,
    SAY_PLAYER_DEATH_2                  = -1599018,
    SAY_PLAYER_DEATH_3                  = -1599019,

    // Tribunal event
    SAY_ESCORT_START                    = -1599020,

    SAY_EVENT_INTRO_2                   = -1599026,
    SAY_EVENT_INTRO_3_ABED              = -1599027,

    SAY_EVENT_A_1                       = -1599028,
    SAY_EVENT_A_2_KADD                  = -1599029,
    SAY_EVENT_A_3                       = -1599030,

    SAY_EVENT_B_1                       = -1599031,
    SAY_EVENT_B_2_MARN                  = -1599032,
    SAY_EVENT_B_3                       = -1599033,

    SAY_EVENT_C_1                       = -1599034,
    SAY_EVENT_C_2_ABED                  = -1599035,
    SAY_EVENT_C_3                       = -1599036,

    SAY_EVENT_D_1                       = -1599037,
    SAY_EVENT_D_2_ABED                  = -1599038,
    SAY_EVENT_D_3                       = -1599039,
    SAY_EVENT_D_4_ABED                  = -1599040,

    // ending dialogue
    SAY_EVENT_END_01                    = -1599041,
    SAY_EVENT_END_02                    = -1599042,
    SAY_EVENT_END_03_ABED               = -1599043,
    SAY_EVENT_END_04                    = -1599044,
    SAY_EVENT_END_05_ABED               = -1599045,
    SAY_EVENT_END_06                    = -1599046,
    SAY_EVENT_END_07_ABED               = -1599047,
    SAY_EVENT_END_08                    = -1599048,
    SAY_EVENT_END_09_KADD               = -1599049,
    SAY_EVENT_END_10                    = -1599050,
    SAY_EVENT_END_11_KADD               = -1599051,
    SAY_EVENT_END_12                    = -1599052,
    SAY_EVENT_END_13_KADD               = -1599053,
    SAY_EVENT_END_14                    = -1599054,
    SAY_EVENT_END_15_MARN               = -1599055,
    SAY_EVENT_END_16                    = -1599056,
    SAY_EVENT_END_17_MARN               = -1599057,
    SAY_EVENT_END_18                    = -1599058,
    SAY_EVENT_END_19_MARN               = -1599059,
    SAY_EVENT_END_20                    = -1599060,
    SAY_EVENT_END_21_ABED               = -1599061,

    SAY_ENTRANCE_MEET                   = -1599064,

    // gossip options
    GOSSIP_ITEM_ID_START                = -3599000,
    GOSSIP_ITEM_ID_PROGRESS             = -3599001,
    GOSSIP_ITEM_ID_END_TRIBUNAL         = -3599002,
    GOSSIP_ITEM_ID_START_SJONNIR        = -3599003,

    // gossip texts
    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101,
    TEXT_ID_END_TRIBUNAL                = 14176,
    TEXT_ID_START_SJONNIR               = 13883,

    // spells used for the event
    SPELL_BRANN_HEALTH_CHECK            = 51810,

    EVENT_ID_BRANN_ACHIEV_FAIL          = 20645,

    // path ids for the creature
    PHASE_GETTING_THERE                 = 0,
    PHASE_TRIBUNAL                      = 1,
    PHASE_ENDING                        = 2,
    PHASE_DESPAWN                       = 3,
    PHASE_SJONNIR                       = 4,
};

static const DialogueEntry aTribunalDialogue[] =
{
    // Intro
    {SAY_EVENT_INTRO_2,         NPC_BRANN,      6000},
    {SAY_EVENT_INTRO_3_ABED,    NPC_ABEDNEUM,   10000},

    // Combat event
    {SAY_EVENT_A_1,             NPC_BRANN,      6000},
    {SAY_EVENT_A_2_KADD,        NPC_KADDRAK,    11000},
    {SAY_EVENT_A_3,             NPC_BRANN,      60000},

    {SAY_EVENT_B_1,             NPC_BRANN,      3000},
    {SAY_EVENT_B_2_MARN,        NPC_MARNAK,     7000},
    {SAY_EVENT_B_3,             NPC_BRANN,      90000},

    {SAY_EVENT_C_1,             NPC_BRANN,      5000},
    {SAY_EVENT_C_2_ABED,        NPC_ABEDNEUM,   7000},
    {SAY_EVENT_C_3,             NPC_BRANN,      90000},

    // Event complete
    {SAY_EVENT_D_1,             NPC_BRANN,      6000},
    {SAY_EVENT_D_2_ABED,        NPC_ABEDNEUM,   6000},
    {SAY_EVENT_D_3,             NPC_BRANN,      7000},
    {SAY_EVENT_D_4_ABED,        NPC_ABEDNEUM,   4000},
    {PHASE_ENDING,              0,              0},

    // Ending dialogue
    {SAY_EVENT_END_01,          NPC_BRANN,      6000},
    {SAY_EVENT_END_02,          NPC_BRANN,      5000},
    {SAY_EVENT_END_03_ABED,     NPC_ABEDNEUM,   8000},
    {SAY_EVENT_END_04,          NPC_BRANN,      12000},
    {SAY_EVENT_END_05_ABED,     NPC_ABEDNEUM,   11000},
    {SAY_EVENT_END_06,          NPC_BRANN,      5000},
    {SAY_EVENT_END_07_ABED,     NPC_ABEDNEUM,   22000},
    {SAY_EVENT_END_08,          NPC_BRANN,      8000},
    {SAY_EVENT_END_09_KADD,     NPC_KADDRAK,    18000},
    {SAY_EVENT_END_10,          NPC_BRANN,      6000},
    {SAY_EVENT_END_11_KADD,     NPC_KADDRAK,    20000},
    {SAY_EVENT_END_12,          NPC_BRANN,      2000},
    {SAY_EVENT_END_13_KADD,     NPC_KADDRAK,    20000},
    {SAY_EVENT_END_14,          NPC_BRANN,      11000},
    {SAY_EVENT_END_15_MARN,     NPC_MARNAK,     7000},
    {SAY_EVENT_END_16,          NPC_BRANN,      5000},
    {SAY_EVENT_END_17_MARN,     NPC_MARNAK,     25000},
    {SAY_EVENT_END_18,          NPC_BRANN,      24000},
    {SAY_EVENT_END_19_MARN,     NPC_MARNAK,     2000},
    {SAY_EVENT_END_20,          NPC_BRANN,      9000},
    {SAY_EVENT_END_21_ABED,     NPC_ABEDNEUM,   8000},

    {SAY_ENTRANCE_MEET,         NPC_BRANN,      5000},
    {PHASE_DESPAWN,             0,              0},
    {0, 0, 0},
};

static const float fBrannResetLocation[4] = { 939.6467f, 375.48926f, 207.42229f, 3.92699f };

/*######
## npc_brann_hos
######*/

struct npc_brann_hosAI : public ScriptedAI, private DialogueHelper
{
    npc_brann_hosAI(Creature* creature) : ScriptedAI(creature),
        DialogueHelper(aTribunalDialogue)
    {
        m_instance = static_cast<instance_halls_of_stone*>(creature->GetInstanceData());
        InitializeDialogueHelper(m_instance);
        m_uiPhase = PHASE_GETTING_THERE;
        Reset();
    }

    instance_halls_of_stone* m_instance;

    uint8 m_uiPhase;

    bool m_bIsBattle;
    bool m_bIsLowHP;

    void Reset() override
    {
        m_bIsLowHP = false;
        m_bIsBattle = false;
    }

    void KilledUnit(Unit* /*pVictim*/) override                          // TODO - possible better as SummonedJustDied
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (!m_instance)
            return;

        m_instance->SetData(TYPE_TRIBUNAL, FAIL);

        if (m_uiPhase == PHASE_TRIBUNAL)
            m_instance->SetData(TYPE_TRIBUNAL, IN_PROGRESS);

        m_creature->SetRespawnDelay(30);
        m_creature->ForcedDespawn();
    }

    // Respawn Handling: Relocate and Set Escort to WP 14
    void JustRespawned() override
    {
        if (m_instance && m_instance->GetData(TYPE_TRIBUNAL) == IN_PROGRESS)
        {
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

            // Relocate to start position
            m_creature->GetMap()->CreatureRelocation(m_creature, fBrannResetLocation[0], fBrannResetLocation[1], fBrannResetLocation[2], fBrannResetLocation[3]);
        }
    }

    // Method to move Brann to the Tribunal event
    void PrepareTribunalEvent()
    {
        if (m_instance)
            m_instance->SetData(TYPE_TRIBUNAL, IN_PROGRESS);

        DoScriptText(SAY_ESCORT_START, m_creature);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

        m_creature->SetWalk(false);
        m_creature->RemoveAurasDueToSpell(SPELL_BRANN_STEALTH);
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveWaypoint(m_uiPhase);
    }

    // Method to begin the Tribunal event
    void StartTribunalEvent()
    {
        if (!m_instance || m_instance->GetData(TYPE_TRIBUNAL) != IN_PROGRESS)
            return;

        // Set the achiev in progress
        if (DoCastSpellIfCan(m_creature, SPELL_BRANN_HEALTH_CHECK, CAST_TRIGGERED) == CAST_OK)
            m_instance->SetBrannSpankin(true);

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveWaypoint(m_uiPhase);
    }

    // Method that marks the event as complete and starts the ending dialogue
    void CompleteTribunalEvent()
    {
        // Mark the event as complete; allow the epilogue to continue
        if (m_instance)
        {
            m_instance->SetData(TYPE_TRIBUNAL, SPECIAL);
            m_instance->ActivateFace(FACE_ABEDNEUM, true);
        }

        m_uiPhase = PHASE_ENDING;
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveWaypoint(m_uiPhase);
    }

    // Method to fully end the Tribunal event
    void EndTribunalEvent()
    {
        if (m_instance)
        {
            m_instance->ResetFace(FACE_ABEDNEUM);
            m_instance->SetData(TYPE_TRIBUNAL, DONE);
        }

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_uiPhase = PHASE_DESPAWN;
    }

    // Method to force end event; used as a shortcut
    void ForceEndTribunalEvent()
    {
        StartNextDialogueText(SAY_ENTRANCE_MEET);
    }

    // Method to start Sjonnir event
    void StartSjonnirEvent()
    {
        m_uiPhase = PHASE_SJONNIR;

        m_creature->SetWalk(false);
        m_creature->RemoveAurasDueToSpell(SPELL_BRANN_STEALTH);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveWaypoint(m_uiPhase);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case SAY_EVENT_A_2_KADD:
                if (m_instance)
                    m_instance->ActivateFace(FACE_KADDRAK, false);
                break;
            case SAY_EVENT_A_3:
                if (m_instance)
                    m_instance->SetFaceTimer(FACE_KADDRAK, 1000);
                break;
            case SAY_EVENT_B_2_MARN:
                if (m_instance)
                    m_instance->ActivateFace(FACE_MARNAK, false);
                break;
            case SAY_EVENT_B_3:
                if (m_instance)
                    m_instance->SetFaceTimer(FACE_MARNAK, 1000);
                break;
            case SAY_EVENT_C_2_ABED:
                if (m_instance)
                    m_instance->ActivateFace(FACE_ABEDNEUM, false);
                break;
            case SAY_EVENT_C_3:
                if (m_instance)
                    m_instance->SetFaceTimer(FACE_ABEDNEUM, 1000);
                break;
            case SAY_EVENT_D_1:
                m_creature->SetImmuneToNPC(true);
                break;
            case PHASE_ENDING:
                CompleteTribunalEvent();
                break;
            case SAY_EVENT_END_09_KADD:
                if (m_instance)
                {
                    m_instance->ActivateFace(FACE_KADDRAK, true);
                    m_instance->ResetFace(FACE_ABEDNEUM);
                }
                break;
            case SAY_EVENT_END_15_MARN:
                if (m_instance)
                {
                    m_instance->ActivateFace(FACE_MARNAK, true);
                    m_instance->ResetFace(FACE_KADDRAK);
                }
                break;
            case SAY_EVENT_END_20:
                if (m_instance)
                {
                    m_instance->DoUseDoorOrButton(GO_TRIBUNAL_FLOOR);
                    m_instance->ResetFace(FACE_MARNAK);
                }
                break;
            case SAY_EVENT_END_21_ABED:
                if (m_instance)
                    m_instance->ActivateFace(FACE_ABEDNEUM, true);
                break;
            case SAY_ENTRANCE_MEET:
                EndTribunalEvent();
                break;
            case PHASE_DESPAWN:
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveWaypoint(m_uiPhase);
                break;

            default:
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        m_uiPhase = uiMiscValue;

        switch (eventType)
        {
            case AI_EVENT_CUSTOM_A:
                PrepareTribunalEvent();
                break;
            case AI_EVENT_CUSTOM_B:
                StartTribunalEvent();
                break;
            case AI_EVENT_CUSTOM_C:
                ForceEndTribunalEvent();
                break;
            case AI_EVENT_CUSTOM_D:
                StartSjonnirEvent();
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != WAYPOINT_MOTION_TYPE || !m_instance)
            return;

        if (uiPointId == 1)
        {
            switch (m_uiPhase)
            {
                case PHASE_TRIBUNAL:                // Start combat event
                    StartNextDialogueText(SAY_EVENT_INTRO_2);
                    if (m_instance)
                        m_instance->DoUseDoorOrButton(GO_TRIBUNAL_CONSOLE);
                    break;
                case PHASE_ENDING:                  // Start ending event
                    StartNextDialogueText(SAY_EVENT_END_01);
                    if (m_instance)
                        m_instance->DoUseDoorOrButton(GO_TRIBUNAL_FLOOR);
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case PHASE_DESPAWN:                 // Despawn and prepare for Sjonnir
                    m_creature->SummonCreature(NPC_BRANN, fBrannDoorLocation[0], fBrannDoorLocation[1], fBrannDoorLocation[2], fBrannDoorLocation[3], TEMPSPAWN_DEAD_DESPAWN, 0, true);
                    m_creature->SetRespawnDelay(24 * HOUR);
                    m_creature->ForcedDespawn();
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        // HP check
        if (!m_bIsLowHP && m_creature->GetHealthPercent() < 30)
        {
            DoScriptText(SAY_LOW_HEALTH, m_creature);
            m_bIsLowHP = true;
        }
        else if (m_bIsLowHP && m_creature->GetHealthPercent() > 30)
            m_bIsLowHP = false;
    }
};

bool GossipHello_npc_brann_hos(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_halls_of_stone* pInstance = (instance_halls_of_stone*)(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_TRIBUNAL) == NOT_STARTED || pInstance->GetData(TYPE_TRIBUNAL) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START, pCreature->GetObjectGuid());
        }
        else if (pInstance->GetData(TYPE_TRIBUNAL) == IN_PROGRESS)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_PROGRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetObjectGuid());
        }
        else if (pInstance->GetData(TYPE_TRIBUNAL) == SPECIAL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_END_TRIBUNAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_END_TRIBUNAL, pCreature->GetObjectGuid());
        }
        else if (pInstance->GetData(TYPE_TRIBUNAL) == DONE && (pInstance->GetData(TYPE_SJONNIR) == NOT_STARTED || pInstance->GetData(TYPE_SJONNIR) == FAIL))
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_SJONNIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_SJONNIR, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_brann_hos(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pCreature, PHASE_GETTING_THERE);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pPlayer, pCreature, PHASE_TRIBUNAL);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, pPlayer, pCreature, PHASE_DESPAWN);
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pCreature->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, pPlayer, pCreature, PHASE_SJONNIR);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

enum
{
    SPELL_SUMMON_DARK_MATTER_TARGET     = 51003,
    SPELL_DARK_MATTER                   = 51012,
    SPELL_DARK_MATTER_H                 = 59868,

    // searing gaze handled in EAI
    // SPELL_SEARING_GAZE               = 51136,
    // SPELL_SEARING_GAZE_H             = 59867,
};

/*######
## npc_dark_matter
######*/

struct npc_dark_matterAI : public ScriptedAI
{
    npc_dark_matterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetLevitate(true);
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiResetTimer;

    void Reset() override
    {
        m_uiSummonTimer = 0;
        m_uiResetTimer = 0;
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_DARK_MATTER_START)
            m_uiSummonTimer = 5000;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // Cast the Dark Matter spell and reset to home position
        if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_DARK_MATTER : SPELL_DARK_MATTER_H) == CAST_OK)
            m_uiResetTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_DARK_MATTER_TARGET) == CAST_OK)
                    m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (m_uiResetTimer)
        {
            // Reset creature to start point
            if (m_uiResetTimer <= uiDiff)
            {
                EnterEvadeMode();
                m_creature->SetWalk(false);

                float fX, fY, fZ;
                m_creature->GetRespawnCoord(fX, fY, fZ);
                m_creature->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

                m_uiResetTimer = 0;
            }
            else
                m_uiResetTimer -= uiDiff;
        }
    }
};

/*######
## spell_shatter - 50810, 61546
######*/

struct spell_shatter : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer() || !target->HasAura(50812))
            return;

        target->RemoveAurasDueToSpell(50812);
        target->CastSpell(target, spell->m_spellInfo->Id == 50810 ? 50811 : 61547, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_petrifying_grip_aura - 50836
######*/

struct spell_petrifying_grip_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target || !target->IsPlayer() || !apply)
            return;

        if (aura->GetStackAmount() == 5)
            target->CastSpell(target, 50812, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_carve_stone_aura - 50563
######*/

struct spell_carve_stone_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (apply)
            target->CastSpell(target, 50549, TRIGGERED_OLD_TRIGGERED);
        else
            target->RemoveAurasDueToSpell(50549);
    }
};

/*######
## spell_taunt_brann - 51774
######*/

struct spell_taunt_brann : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        // trigger 51775 on caster
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(caster, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## event_spell_brann_achievement_fail
######*/

bool ProcessEventId_event_spell_brann_achievement_fail(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (uiEventId == EVENT_ID_BRANN_ACHIEV_FAIL && pSource->IsCreature())
    {
        Creature* brann = static_cast<Creature*>(pSource);
        instance_halls_of_stone* instance = static_cast<instance_halls_of_stone*>(brann->GetInstanceData());
        if (!instance)
            return true;

        instance->SetBrannSpankin(false);
        return true;
    }

    return false;
}

void AddSC_halls_of_stone()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_brann_hos";
    pNewScript->GetAI = &GetNewAIInstance<npc_brann_hosAI>;
    pNewScript->pGossipHello = &GossipHello_npc_brann_hos;
    pNewScript->pGossipSelect = &GossipSelect_npc_brann_hos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_matter";
    pNewScript->GetAI = &GetNewAIInstance<npc_dark_matterAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_brann_achievement_fail";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_brann_achievement_fail;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_shatter>("spell_shatter");
    RegisterSpellScript<spell_petrifying_grip_aura>("spell_petrifying_grip_aura");
    RegisterSpellScript<spell_carve_stone_aura>("spell_carve_stone_aura");
    RegisterSpellScript<spell_taunt_brann>("spell_taunt_brann");
}
