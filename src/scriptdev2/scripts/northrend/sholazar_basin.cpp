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
SDName: Sholazar_Basin
SD%Complete: 100
SDComment: Quest support: 12570, 12580, 12644, 12688
SDCategory: Sholazar Basin
EndScriptData */

/* ContentData
npc_helice
npc_injured_rainspeaker
npc_mosswalker_victim
npc_tipsy_mcmanus
npc_wants_fruit_credit
go_quest_still_at_it_credit
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_helice
######*/

enum
{
    QUEST_ENGINEERING_DISASTER          = 12688,

    SAY_HELICE_ACCEPT                   = -1000657,
    SAY_HELICE_EXPLOSIVES_1             = -1000658,
    SAY_HELICE_EXPLODE_1                = -1000659,
    SAY_HELICE_MOVE_ON                  = -1000660,
    SAY_HELICE_EXPLOSIVES_2             = -1000661,
    SAY_HELICE_EXPLODE_2                = -1000662,
    SAY_HELICE_COMPLETE                 = -1000663,

    SPELL_DETONATE_EXPLOSIVES_1         = 52369,            // first "barrel"
    SPELL_DETONATE_EXPLOSIVES_2         = 52371,            // second "barrel"
};

struct npc_heliceAI : public npc_escortAI
{
    npc_heliceAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiExplodeTimer = 5000;
        m_uiExplodePhase = 0;
        m_bFirstBarrel = true;
        Reset();
    }

    uint32 m_uiExplodeTimer;
    uint32 m_uiExplodePhase;
    bool m_bFirstBarrel;

    void Reset() override
    {
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 2:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_EXPLOSIVES_1, m_creature, pPlayer);
                    SetEscortPaused(true);
                }
                break;
            }
            case 13:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_EXPLOSIVES_2, m_creature, pPlayer);
                    SetEscortPaused(true);
                }
                break;
            }
            case 22:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_HELICE_COMPLETE, m_creature, pPlayer);
                    pPlayer->GroupEventHappens(QUEST_ENGINEERING_DISASTER, m_creature);
                }
                break;
            }
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiExplodeTimer < uiDiff)
                {
                    if (m_bFirstBarrel)
                    {
                        switch (m_uiExplodePhase)
                        {
                            case 0:
                                DoCastSpellIfCan(m_creature, SPELL_DETONATE_EXPLOSIVES_1);

                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_EXPLODE_1, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 1:
                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_MOVE_ON, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 2:
                                SetEscortPaused(false);
                                m_uiExplodePhase = 0;
                                m_uiExplodeTimer = 5000;
                                m_bFirstBarrel = false;
                                break;
                        }
                    }
                    else
                    {
                        switch (m_uiExplodePhase)
                        {
                            case 0:
                                DoCastSpellIfCan(m_creature, SPELL_DETONATE_EXPLOSIVES_2);

                                if (Player* pPlayer = GetPlayerForEscort())
                                    DoScriptText(SAY_HELICE_EXPLODE_2, m_creature, pPlayer);

                                m_uiExplodeTimer = 2500;
                                ++m_uiExplodePhase;
                                break;
                            case 1:
                                SetEscortPaused(false);
                                m_uiExplodePhase = 0;
                                m_uiExplodeTimer = 5000;
                                m_bFirstBarrel = true;
                                break;
                        }
                    }
                }
                else
                    m_uiExplodeTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_helice(Creature* pCreature)
{
    return new npc_heliceAI(pCreature);
}

bool QuestAccept_npc_helice(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ENGINEERING_DISASTER)
    {
        DoScriptText(SAY_HELICE_ACCEPT, pCreature, pPlayer);

        if (npc_heliceAI* pEscortAI = dynamic_cast<npc_heliceAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
    }

    return false;
}

/*######
## npc_injured_rainspeaker
######*/

enum
{
    QUEST_FORTUNATE_MISUNDERSTAND       = 12570,

    GOSSIP_ITEM_READY                   = -3000103,

    SAY_ACCEPT                          = -1000605,
    SAY_START                           = -1000606,
    SAY_END_1                           = -1000607,
    SAY_END_2                           = -1000608,
    SAY_TRACKER                         = -1000609,         // not used in escort (aggro text for trackers? something for vekjik?)

    NPC_FRENZYHEART_TRACKER             = 28077,

    SPELL_ORACLE_ESCORT_START           = 51341,            // unknown purpose
    SPELL_FEIGN_DEATH                   = 51329,
    SPELL_ORACLE_INTRO                  = 51448,
};

struct npc_injured_rainspeakerAI : public npc_escortAI
{
    npc_injured_rainspeakerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void JustStartedEscort() override
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            DoScriptText(SAY_START, m_creature, pPlayer);
            DoCastSpellIfCan(m_creature, SPELL_ORACLE_ESCORT_START);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 22:
            {
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END_1, m_creature, pPlayer);
                    DoCastSpellIfCan(m_creature, SPELL_ORACLE_INTRO);
                }
                break;
            }
            case 23:
            {
                DoScriptText(SAY_END_2, m_creature);

                // location behind
                float fAngle = m_creature->GetOrientation();
                fAngle += M_PI_F;

                float fX, fY, fZ;
                m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0.0f, 15.0f, fAngle);

                m_creature->SummonCreature(NPC_FRENZYHEART_TRACKER, fX, fY, fZ, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000);
                break;
            }
        }
    }

    void UpdateEscortAI(const uint32 /*uiDiff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_injured_rainspeaker(Creature* pCreature)
{
    return new npc_injured_rainspeakerAI(pCreature);
}

bool QuestAccept_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FORTUNATE_MISUNDERSTAND)
    {
        pCreature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
        DoScriptText(SAY_ACCEPT, pCreature, pPlayer);

        // Workaround, GossipHello/GossipSelect doesn't work well when object already has gossip from database
        if (npc_injured_rainspeakerAI* pEscortAI = dynamic_cast<npc_injured_rainspeakerAI*>(pCreature->AI()))
        {
            pEscortAI->Start(true, pPlayer, pQuest);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
    }

    return false;
}

/*
bool GossipHello_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_FORTUNATE_MISUNDERSTAND) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        return true;
    }

    return false;
}

bool GossipSelect_npc_injured_rainspeaker(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_injured_rainspeakerAI* pEscortAI = dynamic_cast<npc_injured_rainspeakerAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer);
    }

    return false;
}
*/

/*######
## npc_mosswalker_victim
######*/

enum
{
    QUEST_MOSSWALKER_SAVIOR         = 12580,
    SPELL_DEAD_SOLDIER              = 45801,                // not clear what this does, but looks like all have it
    SPELL_MOSSWALKER_QUEST_CREDIT   = 52157,

    GOSSIP_ITEM_PULSE               = -3000104,
    TEXT_ID_INJURED                 = 13318,

    EMOTE_PAIN                      = -1000610,

    SAY_RESCUE_1                    = -1000611,
    SAY_RESCUE_2                    = -1000612,
    SAY_RESCUE_3                    = -1000613,
    SAY_RESCUE_4                    = -1000614,

    SAY_DIE_1                       = -1000615,
    SAY_DIE_2                       = -1000616,
    SAY_DIE_3                       = -1000617,
    SAY_DIE_4                       = -1000618,
    SAY_DIE_5                       = -1000619,
    SAY_DIE_6                       = -1000620,
};

bool GossipHello_npc_mosswalker_victim(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_MOSSWALKER_SAVIOR) == QUEST_STATUS_INCOMPLETE)
    {
        // doesn't appear they always emote
        if (urand(0, 3) == 0)
            DoScriptText(EMOTE_PAIN, pCreature);

        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PULSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INJURED, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_mosswalker_victim(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        // just to prevent double credit
        if (pCreature->GetLootRecipient())
            return true;
        else
            pCreature->SetLootRecipient(pPlayer);

        if (urand(0, 2))                                    // die
        {
            switch (urand(0, 5))
            {
                case 0: DoScriptText(SAY_DIE_1, pCreature, pPlayer); break;
                case 1: DoScriptText(SAY_DIE_2, pCreature, pPlayer); break;
                case 2: DoScriptText(SAY_DIE_3, pCreature, pPlayer); break;
                case 3: DoScriptText(SAY_DIE_4, pCreature, pPlayer); break;
                case 4: DoScriptText(SAY_DIE_5, pCreature, pPlayer); break;
                case 5: DoScriptText(SAY_DIE_6, pCreature, pPlayer); break;
            }
        }
        else                                                // survive
        {
            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_RESCUE_1, pCreature, pPlayer); break;
                case 1: DoScriptText(SAY_RESCUE_2, pCreature, pPlayer); break;
                case 2: DoScriptText(SAY_RESCUE_3, pCreature, pPlayer); break;
                case 3: DoScriptText(SAY_RESCUE_4, pCreature, pPlayer); break;
            }

            pCreature->CastSpell(pPlayer, SPELL_MOSSWALKER_QUEST_CREDIT, true);
        }

        // more details may apply, instead of just despawn
        pCreature->ForcedDespawn(5000);
    }
    return true;
}

/*######
## npc_tipsy_mcmanus
######*/

enum
{
    SAY_DISTILLATION_START          = -1001125,
    SAY_ADD_ORANGE                  = -1001126,
    SAY_ADD_BANANAS                 = -1001127,
    SAY_ADD_PAPAYA                  = -1001128,
    SAY_LIGHT_BRAZIER               = -1001129,
    SAY_OPEN_VALVE                  = -1001130,
    SAY_ACTION_COMPLETE_1           = -1001131,
    SAY_ACTION_COMPLETE_2           = -1001132,
    SAY_ACTION_COMPLETE_3           = -1001133,
    SAY_ACTION_COMPLETE_4           = -1001134,
    SAY_DISTILLATION_FAIL           = -1001135,
    SAY_DISTILLATION_COMPLETE       = -1001136,

    GOSSIP_ITEM_TIPSY_MCMANUS       = -3000114,
    TEXT_ID_READY                   = 13288,
    QUEST_ID_STILL_AT_IT            = 12644,

    SPELL_TOSS_ORANGE               = 51931,
    SPELL_TOSS_BANANA               = 51932,
    SPELL_TOSS_PAPAYA               = 51933,

    NPC_WANTS_ORANGE_TRIGGER        = 28535,
    NPC_WANTS_PAPAYA_TRIGGER        = 28536,
    NPC_WANTS_BANANA_TRIGGER        = 28537,
    // NPC_STEAMING_VALVE_TRIGGER   = 28539,
    // NPC_WANTS_FIRE_TRIGGER       = 28540,
    NPC_TIPSY_MCMANUS               = 28566,

    GO_THUNDERBREW_JUNGLE_PUNCH     = 190643,
    GO_PRESSURE_VALVE               = 190635,
    GO_BRAZIER                      = 190636,
};

struct StillAtItData
{
    int32 iText;
    uint32 uiOwnerEntry;
};

static const StillAtItData aStillAtItFruits[3] =
{
    {SAY_ADD_ORANGE,     NPC_WANTS_ORANGE_TRIGGER},
    {SAY_ADD_BANANAS,    NPC_WANTS_BANANA_TRIGGER},
    {SAY_ADD_PAPAYA,     NPC_WANTS_PAPAYA_TRIGGER},
};

static const StillAtItData aStillAtItMachines[2] =
{
    {SAY_LIGHT_BRAZIER,  GO_BRAZIER},
    {SAY_OPEN_VALVE,     GO_PRESSURE_VALVE},
};

struct npc_tipsy_mcmanusAI : public ScriptedAI
{
    npc_tipsy_mcmanusAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiTaskIndex;
    uint32 m_uiTaskOwnerEntry;
    uint32 m_uiTaskTimer;
    uint32 m_uiActionTimer;

    void Reset() override
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        m_uiTaskIndex       = 0;
        m_uiTaskOwnerEntry  = 0;
        m_uiTaskTimer       = 0;
        m_uiActionTimer     = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (pInvoker->GetTypeId() != TYPEID_PLAYER)
            return;

        // start event
        if (eventType == AI_EVENT_START_EVENT)
        {
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            DoScriptText(SAY_DISTILLATION_START, m_creature);
            m_uiTaskTimer = 5000;
        }
        // check fruit tasks
        else if (eventType == AI_EVENT_CUSTOM_A)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (aStillAtItFruits[i].uiOwnerEntry == uiMiscValue)
                    DoCheckDistillationTask(uiMiscValue);
            }
        }
        // check machine tasks
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                if (aStillAtItMachines[i].uiOwnerEntry == uiMiscValue)
                    DoCheckDistillationTask(uiMiscValue);
            }
        }
    }

    // wrapper to complete a distillation task
    void DoCheckDistillationTask(uint32 uiOwnerEntry)
    {
        // check if the given entry matches the expected one
        if (uiOwnerEntry == m_uiTaskOwnerEntry)
        {
            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_ACTION_COMPLETE_1, m_creature); break;
                case 1: DoScriptText(SAY_ACTION_COMPLETE_2, m_creature); break;
                case 2: DoScriptText(SAY_ACTION_COMPLETE_3, m_creature); break;
                case 3: DoScriptText(SAY_ACTION_COMPLETE_4, m_creature); break;
            }

            m_uiTaskTimer = 6000;
            m_uiActionTimer = 0;
        }
        // reset if failed
        else
        {
            DoScriptText(SAY_DISTILLATION_FAIL, m_creature);
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTaskTimer)
        {
            if (m_uiTaskTimer <= uiDiff)
            {
                switch (m_uiTaskIndex)
                {
                        // fruit tasks
                    case 0:
                    case 1:
                    case 3:
                    case 6:
                    case 7:
                    {
                        uint8 uiIndex = urand(0, 2);
                        DoScriptText(aStillAtItFruits[uiIndex].iText, m_creature);
                        m_uiTaskOwnerEntry = aStillAtItFruits[uiIndex].uiOwnerEntry;

                        m_uiTaskTimer = 0;
                        m_uiActionTimer = 5000;
                        break;
                    }
                    // valve or fire task
                    case 2:
                    case 4:
                    case 5:
                    case 8:
                    {
                        uint8 uiIndex = urand(0, 1);
                        DoScriptText(aStillAtItMachines[uiIndex].iText, m_creature);
                        m_uiTaskOwnerEntry = aStillAtItMachines[uiIndex].uiOwnerEntry;

                        m_uiTaskTimer = 0;
                        m_uiActionTimer = 5000;
                        break;
                    }
                    // complete event
                    case 9:
                        DoScriptText(SAY_DISTILLATION_COMPLETE, m_creature);
                        if (GameObject* pPunch = GetClosestGameObjectWithEntry(m_creature, GO_THUNDERBREW_JUNGLE_PUNCH, 10.0f))
                        {
                            pPunch->SetRespawnTime(30);
                            pPunch->Refresh();
                        }
                        m_uiTaskTimer = 20000;
                        break;
                    case 10:
                        EnterEvadeMode();
                        m_uiTaskTimer = 0;
                        break;
                }
                ++m_uiTaskIndex;
            }
            else
                m_uiTaskTimer -= uiDiff;
        }

        // timer delay to allow player to complete the task
        if (m_uiActionTimer)
        {
            if (m_uiActionTimer <= uiDiff)
            {
                DoScriptText(SAY_DISTILLATION_FAIL, m_creature);
                EnterEvadeMode();
                m_uiActionTimer = 0;
            }
            else
                m_uiActionTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tipsy_mcmanus(Creature* pCreature)
{
    return new npc_tipsy_mcmanusAI(pCreature);
}

bool GossipHello_npc_tipsy_mcmanus(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_ID_STILL_AT_IT) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TIPSY_MCMANUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_READY, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_tipsy_mcmanus(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction != GOSSIP_ACTION_INFO_DEF + 1)
        return false;

    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pCreature);
    return true;
}

/*######
## npc_wants_fruit_credit
######*/

bool EffectDummyCreature_npc_wants_fruit_credit(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    if ((uiSpellId == SPELL_TOSS_ORANGE || uiSpellId == SPELL_TOSS_BANANA || uiSpellId == SPELL_TOSS_PAPAYA) && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && ((Player*)pCaster)->GetQuestStatus(QUEST_ID_STILL_AT_IT) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* pTipsyMcmanus = GetClosestCreatureWithEntry(pCaster, NPC_TIPSY_MCMANUS, 2 * INTERACTION_DISTANCE))
            {
                pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pTipsyMcmanus, pCreatureTarget->GetEntry());
                return true;
            }
        }
    }
    return false;
}

/*######
## go_quest_still_at_it_credit
######*/

bool GOUse_go_quest_still_at_it_credit(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_ID_STILL_AT_IT) != QUEST_STATUS_INCOMPLETE)
        return true;

    if (Creature* pTipsyMcmanus = GetClosestCreatureWithEntry(pPlayer, NPC_TIPSY_MCMANUS, 2 * INTERACTION_DISTANCE))
        pTipsyMcmanus->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pPlayer, pTipsyMcmanus, pGo->GetEntry());

    return false;
}

void AddSC_sholazar_basin()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_helice";
    pNewScript->GetAI = &GetAI_npc_helice;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_helice;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_rainspeaker";
    pNewScript->GetAI = &GetAI_npc_injured_rainspeaker;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_injured_rainspeaker;
    // pNewScript->pGossipHello = &GossipHello_npc_injured_rainspeaker;
    // pNewScript->pGossipSelect = &GossipSelect_npc_injured_rainspeaker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mosswalker_victim";
    pNewScript->pGossipHello = &GossipHello_npc_mosswalker_victim;
    pNewScript->pGossipSelect = &GossipSelect_npc_mosswalker_victim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tipsy_mcmanus";
    pNewScript->GetAI = &GetAI_npc_tipsy_mcmanus;
    pNewScript->pGossipHello = &GossipHello_npc_tipsy_mcmanus;
    pNewScript->pGossipSelect = &GossipSelect_npc_tipsy_mcmanus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wants_fruit_credit";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_wants_fruit_credit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_quest_still_at_it_credit";
    pNewScript->pGOUse = &GOUse_go_quest_still_at_it_credit;
    pNewScript->RegisterSelf();
}
