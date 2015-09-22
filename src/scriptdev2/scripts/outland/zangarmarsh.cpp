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
SDName: Zangarmarsh
SD%Complete: 100
SDComment: Quest support: 9729, 9752, 9785, 10009.
SDCategory: Zangarmarsh
EndScriptData */

/* ContentData
npc_cooshcoosh
npc_kayra_longmane
event_stormcrow
npc_fhwoor
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_cooshcoosh
######*/

enum
{
    SPELL_LIGHTNING_BOLT    = 9532,
};

struct npc_cooshcooshAI : public ScriptedAI
{
    npc_cooshcooshAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormFaction;
    uint32 m_uiLightningBolt_Timer;

    void Reset() override
    {
        m_uiLightningBolt_Timer = 2000;

        if (m_creature->getFaction() != m_uiNormFaction)
            m_creature->setFaction(m_uiNormFaction);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiLightningBolt_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LIGHTNING_BOLT);
            m_uiLightningBolt_Timer = 5000;
        }
        else m_uiLightningBolt_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_cooshcoosh(Creature* pCreature)
{
    return new npc_cooshcooshAI(pCreature);
}

/*#####
## npc_kayra_longmane
#####*/

enum
{
    SAY_START           = -1000343,
    SAY_AMBUSH1         = -1000344,
    SAY_PROGRESS        = -1000345,
    SAY_AMBUSH2         = -1000346,
    SAY_END             = -1000347,

    QUEST_ESCAPE_FROM   = 9752,
    NPC_SLAVEBINDER     = 18042
};

struct npc_kayra_longmaneAI : public npc_escortAI
{
    npc_kayra_longmaneAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 i) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 4:
                DoScriptText(SAY_AMBUSH1, m_creature, pPlayer);
                DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                break;
            case 5:
                DoScriptText(SAY_PROGRESS, m_creature, pPlayer);
                SetRun();
                break;
            case 16:
                DoScriptText(SAY_AMBUSH2, m_creature, pPlayer);
                DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);
                break;
            case 17:
                DoScriptText(SAY_END, m_creature, pPlayer);
                break;
            case 25:
                pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM, m_creature);
                break;
        }
    }

    void Reset() override { }
};

bool QuestAccept_npc_kayra_longmane(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);

        if (npc_kayra_longmaneAI* pEscortAI = dynamic_cast<npc_kayra_longmaneAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_kayra_longmane(Creature* pCreature)
{
    return new npc_kayra_longmaneAI(pCreature);
}

/*######
## event_stormcrow
######*/

enum
{
    QUEST_AS_THE_CROW_FLIES = 9718,
    EVENT_ID_STORMCROW      = 11225,
};

bool ProcessEventId_event_taxi_stormcrow(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (uiEventId == EVENT_ID_STORMCROW && !bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)pSource)->SetDisplayId(((Player*)pSource)->GetNativeDisplayId());
        ((Player*)pSource)->AreaExploredOrEventHappens(QUEST_AS_THE_CROW_FLIES);
        return true;
    }
    return false;
}

/*#####
## npc_fhwoor
#####*/

enum
{
    SAY_ESCORT_START            = -1000995,
    SAY_PREPARE                 = -1000996,
    SAY_CAMP_ENTER              = -1000997,
    SAY_AMBUSH                  = -1000998,
    SAY_AMBUSH_CLEARED          = -1000999,
    SAY_ESCORT_COMPLETE         = -1001000,

    SPELL_STOMP                 = 31277,
    SPELL_THUNDERSHOCK          = 31964,

    NPC_ENCHANTRESS             = 18088,
    NPC_SLAVEDRIVER             = 18089,
    NPC_SSSLITH                 = 18154,

    GO_ARK_OF_SSSLITH           = 182082,

    QUEST_ID_FHWOOR_SMASH       = 9729,
};

struct npc_fhwoorAI : public npc_escortAI
{
    npc_fhwoorAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiStompTimer;
    uint32 m_uiShockTimer;

    bool m_bIsAmbush;

    void Reset() override
    {
        m_uiStompTimer = urand(3000, 7000);
        m_uiShockTimer = urand(7000, 11000);
        m_bIsAmbush = false;
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_ESCORT_START, m_creature, pInvoker);
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            Start(true, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue), true);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // move summoned towards the creature
        if (m_bIsAmbush)
        {
            float fX, fY, fZ;
            m_creature->GetContactPoint(pSummoned, fX, fY, fZ);
            pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // resume escort
        if (pSummoned->GetEntry() == NPC_SSSLITH)
            SetEscortPaused(false);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 24:
                DoScriptText(SAY_PREPARE, m_creature);
                break;
            case 25:
                DoScriptText(SAY_CAMP_ENTER, m_creature);
                SetRun(false);
                break;
            case 46:
                // despawn the Ark
                if (GameObject* pArk = GetClosestGameObjectWithEntry(m_creature, GO_ARK_OF_SSSLITH, 10.0f))
                    pArk->SetLootState(GO_JUST_DEACTIVATED);
                // spawn npcs
                m_creature->SummonCreature(NPC_ENCHANTRESS, 526.12f, 8136.96f, 21.64f, 0.57f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 524.09f, 8138.67f, 21.49f, 0.58f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 526.93f, 8133.88f, 21.56f, 0.58f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 70:
                DoScriptText(SAY_AMBUSH, m_creature);
                // spawn npcs
                m_bIsAmbush = true;
                m_creature->SummonCreature(NPC_SSSLITH, 162.91f, 8192.08f, 22.55f, 5.98f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ENCHANTRESS, 162.34f, 8193.99f, 22.85f, 5.98f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 163.07f, 8187.04f, 22.71f, 0.10f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                SetEscortPaused(true);
                break;
            case 71:
                DoScriptText(SAY_AMBUSH_CLEARED, m_creature);
                SetRun();
                break;
            case 92:
                SetRun(false);
                break;
            case 93:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ID_FHWOOR_SMASH, m_creature);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STOMP) == CAST_OK)
                m_uiStompTimer = urand(9000, 15000);
        }
        else
            m_uiStompTimer -= uiDiff;

        if (m_uiShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_THUNDERSHOCK) == CAST_OK)
                m_uiShockTimer = urand(15000, 20000);
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_fhwoor(Creature* pCreature)
{
    return new npc_fhwoorAI(pCreature);
}

bool QuestAccept_npc_fhwoor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_FHWOOR_SMASH)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

void AddSC_zangarmarsh()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_cooshcoosh";
    pNewScript->GetAI = &GetAI_npc_cooshcoosh;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kayra_longmane";
    pNewScript->GetAI = &GetAI_npc_kayra_longmane;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_kayra_longmane;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_taxi_stormcrow";
    pNewScript->pProcessEventId = &ProcessEventId_event_taxi_stormcrow;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fhwoor";
    pNewScript->GetAI = &GetAI_npc_fhwoor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_fhwoor;
    pNewScript->RegisterSelf();
}
