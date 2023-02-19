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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "Maps/MapManager.h"

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
        m_uiNormFaction = pCreature->GetFaction();
        Reset();
    }

    uint32 m_uiNormFaction;
    uint32 m_uiLightningBolt_Timer;

    void Reset() override
    {
        m_uiLightningBolt_Timer = 2000;

        if (m_creature->GetFaction() != m_uiNormFaction)
            m_creature->setFaction(m_uiNormFaction);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiLightningBolt_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_LIGHTNING_BOLT);
            m_uiLightningBolt_Timer = 5000;
        }
        else m_uiLightningBolt_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_cooshcoosh(Creature* pCreature)
{
    return new npc_cooshcooshAI(pCreature);
}

/*#####
## npc_kayra_longmane
#####*/

enum
{
    SAY_START               = -1001243,
    SAY_AMBUSH1             = -1001244,
    SAY_PROGRESS1           = -1001245,
    SAY_AMBUSH2             = -1001246,
    SAY_PROGRESS2           = -1001247,
    SAY_END                 = -1001248,
    SAY_SLAVEBINDER_AMBUSH2 = -1001249,

    QUEST_ESCAPE_FROM       = 9752,
    NPC_SLAVEBINDER         = 18042
};

struct npc_kayra_longmaneAI : public npc_escortAI
{
    npc_kayra_longmaneAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void JustRespawned() override
    {
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

    void WaypointReached(uint32 i) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 1:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case 5:
                DoScriptText(SAY_AMBUSH1, m_creature, pPlayer);
                m_creature->SummonCreature(NPC_SLAVEBINDER, -916.4861f, 5355.635f, 18.25233f, 5.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                m_creature->SummonCreature(NPC_SLAVEBINDER, -918.9288f, 5358.43f, 18.05894f, 5.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                break;
            case 6:
                DoScriptText(SAY_PROGRESS1, m_creature, pPlayer);
                SetRun();
                break;
            case 17:
                DoScriptText(SAY_AMBUSH2, m_creature, pPlayer);
                break;
            case 18:
            {
                Creature* spawn = m_creature->SummonCreature(NPC_SLAVEBINDER, -668.2899f, 5382.913f, 22.32479f, 5.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                DoScriptText(SAY_SLAVEBINDER_AMBUSH2, spawn, pPlayer);
                m_creature->SummonCreature(NPC_SLAVEBINDER, -669.2795f, 5386.802f, 23.01249f, 5.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                break;
            }
            case 19:
                DoScriptText(SAY_PROGRESS2, m_creature, pPlayer);
                break;
            case 27:
                DoScriptText(SAY_END, m_creature, pPlayer);
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ESCAPE_FROM, m_creature);
                m_creature->ForcedDespawn(10000);
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
        pCreature->SetFactionTemporary(FACTION_ESCORT_N_FRIEND_ACTIVE, TEMPFACTION_RESTORE_RESPAWN); // sniffed faction

        if (npc_kayra_longmaneAI* pEscortAI = dynamic_cast<npc_kayra_longmaneAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

UnitAI* GetAI_npc_kayra_longmane(Creature* pCreature)
{
    return new npc_kayra_longmaneAI(pCreature);
}

/*######
## event_stormcrow
######*/

enum
{
    EVENT_ID_STORMCROW      = 11225,
    RELAY_SCRIPT_CROW_FLIES = 10205,
    MODEL_ID_CROW           = 17447,
    MODEL_ID_INVISIBLE      = 17657,
    NPC_YSIEL_WINDSINGER    = 17841,
};

bool ProcessEventId_event_taxi_stormcrow(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (uiEventId == EVENT_ID_STORMCROW && !bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)pSource)->SetDisplayId(((Player*)pSource)->GetNativeDisplayId());
        return true;
    }
    return false;
}

struct StormcrowAmulet : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (apply)
        {
            caster->SetDisplayId(MODEL_ID_INVISIBLE);

            if (ScriptedMap* scriptedMap = (ScriptedMap*)caster->GetInstanceData())
            {
                if (Creature* ysiel = scriptedMap->GetSingleCreatureFromStorage(NPC_YSIEL_WINDSINGER))
                {
                    caster->GetMap()->ScriptsStart(SCRIPT_TYPE_RELAY, RELAY_SCRIPT_CROW_FLIES, ysiel, caster);
                }
            }
        }
        else
        {
            caster->SetDisplayId(MODEL_ID_CROW);
        }
    }
};

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

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
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
            case 25:
                DoScriptText(SAY_PREPARE, m_creature);
                break;
            case 26:
                DoScriptText(SAY_CAMP_ENTER, m_creature);
                SetRun(false);
                break;
            case 47:
                // despawn the Ark
                if (GameObject* pArk = GetClosestGameObjectWithEntry(m_creature, GO_ARK_OF_SSSLITH, 10.0f))
                    pArk->SetLootState(GO_JUST_DEACTIVATED);
                // spawn npcs
                m_creature->SummonCreature(NPC_ENCHANTRESS, 526.12f, 8136.96f, 21.64f, 0.57f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 524.09f, 8138.67f, 21.49f, 0.58f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 526.93f, 8133.88f, 21.56f, 0.58f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 71:
                DoScriptText(SAY_AMBUSH, m_creature);
                // spawn npcs
                m_bIsAmbush = true;
                m_creature->SummonCreature(NPC_SSSLITH, 162.91f, 8192.08f, 22.55f, 5.98f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_ENCHANTRESS, 162.34f, 8193.99f, 22.85f, 5.98f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_SLAVEDRIVER, 163.07f, 8187.04f, 22.71f, 0.10f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                SetEscortPaused(true);
                break;
            case 72:
                DoScriptText(SAY_AMBUSH_CLEARED, m_creature);
                SetRun();
                break;
            case 93:
                SetRun(false);
                break;
            case 94:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_FHWOOR_SMASH, m_creature);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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

UnitAI* GetAI_npc_fhwoor(Creature* pCreature)
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

/*######
## npc_frostbite
######*/

enum
{
    SPELL_FROSTBITE_ROTATE = 34748,
    SPELL_PERIODIC_TRIGGER_DUMMY = 30023,
    SPELL_FROST_RING_FRONT = 34740,
    SPELL_FROST_RING_BEHIND = 34746,
    SPELL_FREEZING_CIRCLE = 34779
};

struct npc_frostbiteAI : public ScriptedAI
{
    npc_frostbiteAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
        tick = 0;
    }

    uint8 tick;

    void Reset() override
    {
        m_creature->CastSpell(m_creature, SPELL_FROSTBITE_ROTATE, TRIGGERED_NONE);
    }
    
    void SpellHit(Unit* caster, const SpellEntry* spell) override
    {
        if (caster != m_creature)
            return;

        if (spell->Id != SPELL_PERIODIC_TRIGGER_DUMMY)
            return;

        switch (tick)
        {
            case 0:
            case 1:
            case 2:
            {
                float newAngle = m_creature->GetOrientation();

                newAngle += M_PI_F / 3;

                newAngle = MapManager::NormalizeOrientation(newAngle);

                m_creature->SetFacingTo(newAngle);
                m_creature->SetOrientation(newAngle);

                m_creature->CastSpell(m_creature, SPELL_FROST_RING_FRONT, TRIGGERED_NONE);
                m_creature->CastSpell(m_creature, SPELL_FROST_RING_BEHIND, TRIGGERED_NONE);

                tick++;
                break;
            }
            case 3:
                m_creature->CastSpell(m_creature, SPELL_FREEZING_CIRCLE, TRIGGERED_NONE);
                tick++;
                break;
            default:
                m_creature->ForcedDespawn();
                break;
        }
    }

    void UpdateAI(const uint32 /*diff*/) override
    {
    }
};

UnitAI* GetAI_npc_frostbite(Creature* creature)
{
    return new npc_frostbiteAI(creature);
}

void AddSC_zangarmarsh()
{
    Script* pNewScript = new Script;
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

    RegisterSpellScript<StormcrowAmulet>("spell_stormcrow_amulet");

    pNewScript = new Script;
    pNewScript->Name = "npc_fhwoor";
    pNewScript->GetAI = &GetAI_npc_fhwoor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_fhwoor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frostbite";
    pNewScript->GetAI = &GetAI_npc_frostbite;
    pNewScript->RegisterSelf();
}
