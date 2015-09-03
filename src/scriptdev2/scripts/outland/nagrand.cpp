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
SDName: Nagrand
SD%Complete: 90
SDComment: Quest support: 9868, 9879, 9918, 10085, 10646, 11090.
SDCategory: Nagrand
EndScriptData */

/* ContentData
mob_lump
npc_nagrand_captive
npc_creditmarker_visit_with_ancestors
npc_rethhedron
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## mob_lump
######*/

enum
{
    SAY_LUMP_AGGRO_1            = -1000190,
    SAY_LUMP_AGGRO_2            = -1000191,
    SAY_LUMP_DEFEAT             = -1000192,

    SPELL_VISUAL_SLEEP          = 16093,
    SPELL_SPEAR_THROW           = 32248,

    FACTION_FRIENDLY            = 35
};

struct mob_lumpAI : public ScriptedAI
{
    mob_lumpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bReset = false;
        Reset();
    }

    uint32 m_uiResetTimer;
    uint32 m_uiSpearThrowTimer;
    bool m_bReset;

    void Reset() override
    {
        m_uiResetTimer = MINUTE * IN_MILLISECONDS;
        m_uiSpearThrowTimer = 2000;
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        if (m_creature->GetHealth() < uiDamage || (m_creature->GetHealth() - uiDamage) * 100 / m_creature->GetMaxHealth() < 30)
        {
            uiDamage = 0;                               // Take 0 damage

            m_creature->RemoveAllAuras();
            m_creature->DeleteThreatList();
            m_creature->CombatStop(true);

            // should get unit_flags UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_PASSIVE at faction change, but unclear why/for what reason, skipped (no flags expected as default)
            m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_REACH_HOME);

            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
            DoScriptText(SAY_LUMP_DEFEAT, m_creature);

            m_bReset = true;
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (m_creature->HasAura(SPELL_VISUAL_SLEEP, EFFECT_INDEX_0))
            m_creature->RemoveAurasDueToSpell(SPELL_VISUAL_SLEEP);

        if (!m_creature->IsStandState())
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(urand(0, 1) ? SAY_LUMP_AGGRO_1 : SAY_LUMP_AGGRO_2, m_creature, pWho);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Check if we waiting for a reset
        if (m_bReset)
        {
            if (m_uiResetTimer < uiDiff)
            {
                EnterEvadeMode();
                m_bReset = false;
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // SpearThrow Timer
        if (m_uiSpearThrowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPEAR_THROW);
            m_uiSpearThrowTimer = 20000;
        }
        else
            m_uiSpearThrowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lump(Creature* pCreature)
{
    return new mob_lumpAI(pCreature);
}

/*######
## npc_nagrand_captive
#####*/

enum
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_NO_ESCAPE           = -1000483,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_MORE_REPLY          = -1000485,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SAY_KUR_START               = -1001001,
    SAY_KUR_AMBUSH_1            = -1001002,
    SAY_KUR_AMBUSH_2            = -1001003,
    SAY_KUR_COMPLETE_1          = -1001004,
    SAY_KUR_COMPLETE_2          = -1001005,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,
    QUEST_TOTEM_KARDASH_A       = 9879,

    NPC_KURENAI_CAPTIVE         = 18209,
    NPC_MAGHAR_CAPTIVE          = 18210,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202
};

static float m_afAmbushA[] = { -1568.805786f, 8533.873047f, 1.958f};
static float m_afAmbushB[] = { -1491.554321f, 8506.483398f, 1.248f};

struct npc_nagrand_captiveAI : public npc_escortAI
{
    npc_nagrand_captiveAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiChainLightningTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiFrostShockTimer;

    void Reset() override
    {
        m_uiChainLightningTimer = 1000;
        m_uiHealTimer = 0;
        m_uiFrostShockTimer = 6000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_EARTHBIND_TOTEM);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
            {
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->SetFactionTemporary(FACTION_ESCORT_H_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);

                DoScriptText(SAY_MAG_START, m_creature);

                m_creature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushA[0] + 2.5f, m_afAmbushA[1] - 2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushA[0] - 2.5f, m_afAmbushA[1] + 2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushA[0], m_afAmbushA[1], m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
            }
            else if (m_creature->GetEntry() == NPC_KURENAI_CAPTIVE)
            {
                m_creature->SetFactionTemporary(FACTION_ESCORT_A_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);

                DoScriptText(SAY_KUR_START, m_creature);

                m_creature->SummonCreature(NPC_MURK_RAIDER, -1509.606f, 8484.284f, -3.841f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_MURK_PUTRIFIER, -1532.475f, 8454.706f, -4.102f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_MURK_BRUTE, -1525.484f, 8475.383f, -2.482f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
            }

            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 7:
                if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
                    DoScriptText(SAY_MAG_MORE, m_creature);
                else if (m_creature->GetEntry() == NPC_KURENAI_CAPTIVE)
                    DoScriptText(urand(0, 1) ? SAY_KUR_AMBUSH_1 : SAY_KUR_AMBUSH_2, m_creature);

                if (Creature* pTemp = m_creature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0], m_afAmbushB[1], m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000))
                    DoScriptText(SAY_MAG_MORE_REPLY, pTemp);

                m_creature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0] - 2.5f, m_afAmbushB[1] - 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);

                m_creature->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0] + 2.5f, m_afAmbushB[1] + 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                m_creature->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0] + 2.5f, m_afAmbushB[1] - 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 25000);
                break;
            case 16:
                if (m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE)
                    DoScriptText(SAY_MAG_COMPLETE, m_creature);
                else if (m_creature->GetEntry() == NPC_KURENAI_CAPTIVE)
                    DoScriptText(urand(0, 1) ? SAY_KUR_COMPLETE_1 : SAY_KUR_COMPLETE_2, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(m_creature->GetEntry() == NPC_MAGHAR_CAPTIVE ? QUEST_TOTEM_KARDASH_H : QUEST_TOTEM_KARDASH_A, m_creature);

                SetRun();
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_MURK_BRUTE)
            DoScriptText(SAY_MAG_NO_ESCAPE, pSummoned);

        if (pSummoned->IsTotem())
            return;

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
    }

    void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CHAIN_LIGHTNING)
        {
            if (urand(0, 9))
                return;

            DoScriptText(SAY_MAG_LIGHTNING, m_creature);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING) == CAST_OK)
                m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 30.0f)
        {
            if (m_uiHealTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HEALING_WAVE) == CAST_OK)
                    m_uiHealTimer = 5000;
            }
            else
                m_uiHealTimer -= uiDiff;
        }

        if (m_uiFrostShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_SHOCK) == CAST_OK)
                m_uiFrostShockTimer = urand(7500, 15000);
        }
        else
            m_uiFrostShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_nagrand_captive(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_H || pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_A)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

CreatureAI* GetAI_npc_nagrand_captive(Creature* pCreature)
{
    return new npc_nagrand_captiveAI(pCreature);
}

/*######
## npc_creditmarker_visist_with_ancestors (Quest 10085)
######*/

enum
{
    QUEST_VISIT_WITH_ANCESTORS  = 10085
};

struct npc_creditmarker_visit_with_ancestorsAI : public ScriptedAI
{
    npc_creditmarker_visit_with_ancestorsAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 30.0f))
        {
            if (((Player*)pWho)->GetQuestStatus(QUEST_VISIT_WITH_ANCESTORS) == QUEST_STATUS_INCOMPLETE)
            {
                uint32 creditMarkerId = m_creature->GetEntry();
                if ((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                {
                    // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                    if (!((Player*)pWho)->GetReqKillOrCastCurrentCount(QUEST_VISIT_WITH_ANCESTORS, creditMarkerId))
                        ((Player*)pWho)->KilledMonsterCredit(creditMarkerId, m_creature->GetObjectGuid());
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_creditmarker_visit_with_ancestors(Creature* pCreature)
{
    return new npc_creditmarker_visit_with_ancestorsAI(pCreature);
}

/*######
## npc_rethhedron
######*/

enum
{
    SAY_LOW_HP                      = -1000966,
    SAY_EVENT_END                   = -1000967,

    SPELL_CRIPPLE                   = 41281,
    SPELL_SHADOW_BOLT               = 41280,
    SPELL_ABYSSAL_TOSS              = 41283,                // summon npc 23416 at target position
    SPELL_ABYSSAL_IMPACT            = 41284,
    // SPELL_GROUND_AIR_PULSE       = 41270,                // spell purpose unk
    // SPELL_AGGRO_CHECK            = 41285,                // spell purpose unk
    // SPELL_AGGRO_BURST            = 41286,                // spell purpose unk

    SPELL_COSMETIC_LEGION_RING      = 41339,
    SPELL_QUEST_COMPLETE            = 41340,

    NPC_SPELLBINDER                 = 22342,
    NPC_RETHHEDRONS_TARGET          = 23416,

    POINT_ID_PORTAL_FRONT           = 0,
    POINT_ID_PORTAL                 = 1,
};

static const float afRethhedronPos[2][3] =
{
    { -1502.39f, 9772.33f, 200.421f},
    { -1557.93f, 9834.34f, 200.949f}
};

struct npc_rethhedronAI : public ScriptedAI
{
    npc_rethhedronAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiCrippleTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiAbyssalTossTimer;
    uint32 m_uiDelayTimer;

    bool m_bLowHpYell;
    bool m_bEventFinished;

    void Reset() override
    {
        m_uiCrippleTimer     = urand(5000, 9000);
        m_uiShadowBoltTimer  = urand(1000, 3000);
        m_uiAbyssalTossTimer = 0;
        m_uiDelayTimer       = 0;

        m_bLowHpYell        = false;
        m_bEventFinished    = false;
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 30.0f);
        }
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage) override
    {
        // go to epilog at 10% health
        if (!m_bEventFinished && m_creature->GetHealthPercent() < 10.0f)
        {
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_PORTAL_FRONT, afRethhedronPos[0][0], afRethhedronPos[0][1], afRethhedronPos[0][2]);
            m_bEventFinished = true;
        }

        // npc is not allowed to die
        if (m_creature->GetHealth() < uiDamage)
            uiDamage = 0;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_PORTAL_FRONT)
        {
            DoScriptText(SAY_EVENT_END, m_creature);
            m_creature->GetMotionMaster()->MoveIdle();
            m_uiDelayTimer = 2000;
        }
        else if (uiPointId == POINT_ID_PORTAL)
        {
            DoCastSpellIfCan(m_creature, SPELL_COSMETIC_LEGION_RING, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_QUEST_COMPLETE, CAST_TRIGGERED);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->ForcedDespawn(2000);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_RETHHEDRONS_TARGET)
            pSummoned->CastSpell(pSummoned, SPELL_ABYSSAL_IMPACT, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiDelayTimer)
        {
            if (m_uiDelayTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_PORTAL, afRethhedronPos[1][0], afRethhedronPos[1][1], afRethhedronPos[1][2]);
                m_uiDelayTimer = 0;
            }
            else
                m_uiDelayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bEventFinished)
            return;

        if (m_uiCrippleTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRIPPLE) == CAST_OK)
                m_uiCrippleTimer = urand(20000, 30000);
        }
        else
            m_uiCrippleTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                m_uiShadowBoltTimer = urand(3000, 5000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiAbyssalTossTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ABYSSAL_TOSS) == CAST_OK)
                m_uiAbyssalTossTimer = urand(500, 2000);
        }
        else
            m_uiAbyssalTossTimer -= uiDiff;

        if (!m_bLowHpYell && m_creature->GetHealthPercent() < 40.0f)
        {
            DoScriptText(SAY_LOW_HP, m_creature);
            m_bLowHpYell = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_rethhedron(Creature* pCreature)
{
    return new npc_rethhedronAI(pCreature);
}

void AddSC_nagrand()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_lump";
    pNewScript->GetAI = &GetAI_mob_lump;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_nagrand_captive";
    pNewScript->GetAI = &GetAI_npc_nagrand_captive;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_nagrand_captive;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_creditmarker_visit_with_ancestors";
    pNewScript->GetAI = &GetAI_npc_creditmarker_visit_with_ancestors;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rethhedron";
    pNewScript->GetAI = &GetAI_npc_rethhedron;
    pNewScript->RegisterSelf();
}
