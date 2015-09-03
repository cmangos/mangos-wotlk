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
SDName: boss_general_vezax
SD%Complete: 100%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                           = -1603096,
    SAY_SLAY_1                          = -1603097,
    SAY_SLAY_2                          = -1603098,
    SAY_SURGE                           = -1603099,
    SAY_DEATH                           = -1603100,
    SAY_ENRAGE                          = -1603101,
    SAY_HARD_MODE                       = -1603102,

    EMOTE_VAPOR                         = -1603103,
    EMOTE_SURGE                         = -1603104,
    EMOTE_ANIMUS                        = -1603105,

    // normal spells
    SPELL_AURA_OF_DESPAIR               = 62692,
    SPELL_SHADOW_CRASH                  = 62660,
    SPELL_SHADOW_CRASH_DAMAGE           = 62659,            // used for achiev check
    SPELL_MARK_OF_FACELESS              = 63276,            // triggers 63278
    SPELL_SEARING_FLAMES                = 62661,
    SPELL_SURGE_OF_DARKNESS             = 62662,
    SPELL_SUMMON_VAPORS                 = 63081,            // cast by Vezax bunny
    SPELL_BERSERK                       = 26662,

    // hard mode spells
    SPELL_SARONITE_BARRIER              = 63364,            // also sends event 9735
    SPELL_SUMMON_ANIMUS                 = 63145,            // the animus should spam 63420 on target - to be done in acid
    SPELL_ANIMUS_FORMATION              = 63319,            // visual aura on Saronite summoner bunny

    // other spells
    SPELL_SARONITE_VAPORS               = 63323,            // cast by vapor on death
    SPELL_CORRUPTED_RAGE                = 68415,            // Unused
    SPELL_CORRUPTED_WISDOM              = 64646,            // Unused

    MAX_HARD_MODE_VAPORS                = 6,

    NPC_SARONITE_VAPOR                  = 33488,
};

/*######
## boss_general_vezax
######*/

struct boss_general_vezaxAI : public ScriptedAI
{
    boss_general_vezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEnrageTimer;
    uint32 m_uiCrashTimer;
    uint32 m_uiMarkTimer;
    uint32 m_uiFlamesTimer;
    uint32 m_uiSurgeTimer;
    uint32 m_uiSaroniteVaporTimer;
    uint32 m_uiHardModeTimer;
    uint8 m_uiHardModeStage;

    uint8 m_uiVaporsGathered;

    GuidList m_lVaporsGuids;

    void Reset() override
    {
        m_uiEnrageTimer          = 10 * MINUTE * IN_MILLISECONDS;
        m_uiFlamesTimer          = 8000;
        m_uiSaroniteVaporTimer   = 30000;
        m_uiSurgeTimer           = 60000;
        m_uiMarkTimer            = 20000;
        m_uiCrashTimer           = 15000;
        m_uiHardModeTimer        = 0;
        m_uiHardModeStage        = 0;
        m_uiVaporsGathered       = 0;

        m_lVaporsGuids.clear();
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);
            m_pInstance->SetData(TYPE_VEZAX_HARD, NOT_STARTED);
        }

        DoCastSpellIfCan(m_creature, SPELL_AURA_OF_DESPAIR);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SARONITE_VAPOR)
        {
            m_lVaporsGuids.push_back(pSummoned->GetObjectGuid());

            // if vapors have reached the max number for hard mode then summon animus
            if (m_lVaporsGuids.size() == MAX_HARD_MODE_VAPORS)
                DoPrepareAnimusIfCan();
        }
        else if (pSummoned->GetEntry() == NPC_SARONITE_ANIMUS)
            pSummoned->SetInCombatWithZone();
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (pSender->GetEntry() == NPC_SARONITE_VAPOR)
        {
            // decrease the number of vapors when they die
            if (eventType == AI_EVENT_CUSTOM_A)
                m_lVaporsGuids.remove(pSender->GetObjectGuid());
            else if (eventType == AI_EVENT_CUSTOM_B)
            {
                ++m_uiVaporsGathered;

                // Cast the saronite formation aura when all vapors arive
                if (m_uiVaporsGathered == MAX_HARD_MODE_VAPORS)
                {
                    // visual on the summoning bunny
                    if (m_pInstance)
                    {
                        if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_pInstance->GetVezaxBunnyGuid(true)))
                            pBunny->CastSpell(pBunny, SPELL_ANIMUS_FORMATION, true);
                    }

                    // Despawn the vapors
                    for (GuidList::const_iterator itr = m_lVaporsGuids.begin(); itr != m_lVaporsGuids.end(); ++itr)
                    {
                        if (Creature* pVapor = m_creature->GetMap()->GetCreature(*itr))
                            pVapor->ForcedDespawn();
                    }

                    DoScriptText(EMOTE_ANIMUS, m_creature);
                    m_uiHardModeTimer = 4000;
                }
            }
        }
        // remove saronite barrier when animus dies
        else if (pSender->GetEntry() == NPC_SARONITE_ANIMUS && eventType == AI_EVENT_CUSTOM_C)
            m_creature->RemoveAurasDueToSpell(SPELL_SARONITE_BARRIER);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pTarget->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        // Check achiev criterias
        if (pSpell->Id == SPELL_SHADOW_CRASH_DAMAGE)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SHADOWDODGER, false);
    }

    // Merge vapors
    void DoPrepareAnimusIfCan()
    {
        if (!m_pInstance)
            return;

        Creature* pBunny = m_creature->GetMap()->GetCreature(m_pInstance->GetVezaxBunnyGuid(true));
        if (!pBunny)
            return;

        // Gather the vapors to the spawn point
        for (GuidList::const_iterator itr = m_lVaporsGuids.begin(); itr != m_lVaporsGuids.end(); ++itr)
        {
            if (Creature* pVapor = m_creature->GetMap()->GetCreature(*itr))
            {
                pVapor->SetWalk(false);
                pVapor->GetMotionMaster()->MovePoint(1, pBunny->GetPositionX(), pBunny->GetPositionY(), pBunny->GetPositionZ());
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHardModeTimer)
        {
            if (m_uiHardModeTimer <= uiDiff)
            {
                switch (m_uiHardModeStage)
                {
                    case 0:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SARONITE_BARRIER, CAST_TRIGGERED | CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                        {
                            DoScriptText(SAY_HARD_MODE, m_creature);
                            m_uiHardModeTimer = 2000;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (m_pInstance)
                        {
                            if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_pInstance->GetVezaxBunnyGuid(true)))
                            {
                                pBunny->RemoveAurasDueToSpell(SPELL_ANIMUS_FORMATION);
                                pBunny->CastSpell(pBunny, SPELL_SUMMON_ANIMUS, true, NULL, NULL, m_creature->GetObjectGuid());
                            }
                        }
                        m_uiHardModeTimer = 0;
                        break;
                    }
                }
                ++m_uiHardModeStage;
            }
            else
                m_uiHardModeTimer -= uiDiff;
        }

        // summon saronite vapors before the hard mode
        if (m_pInstance && m_pInstance->GetData(TYPE_VEZAX_HARD) == NOT_STARTED)
        {
            if (m_uiSaroniteVaporTimer < uiDiff)
            {
                // get a bunny that will summon the vapors
                if (m_pInstance)
                {
                    if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_pInstance->GetVezaxBunnyGuid(false)))
                    {
                        pBunny->CastSpell(pBunny, SPELL_SUMMON_VAPORS, true, NULL, NULL, m_creature->GetObjectGuid());
                        DoScriptText(EMOTE_VAPOR, m_creature);

                        m_uiSaroniteVaporTimer = 30000;
                    }
                }
            }
            else
                m_uiSaroniteVaporTimer -= uiDiff;
        }

        // Searing flames only while animus is not around
        if (m_pInstance && (m_pInstance->GetData(TYPE_VEZAX_HARD) != IN_PROGRESS || !m_bIsRegularMode))
        {
            if (m_uiFlamesTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SEARING_FLAMES) == CAST_OK)
                    m_uiFlamesTimer = urand(9000, 16000);
            }
            else
                m_uiFlamesTimer -= uiDiff;
        }

        if (m_uiSurgeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_DARKNESS) == CAST_OK)
            {
                DoScriptText(SAY_SURGE, m_creature);
                DoScriptText(EMOTE_SURGE, m_creature);
                m_uiSurgeTimer = 62000;
            }
        }
        else
            m_uiSurgeTimer -= uiDiff;

        if (m_uiMarkTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_FACELESS) == CAST_OK)
                    m_uiMarkTimer = urand(25000, 30000);
            }
        }
        else
            m_uiMarkTimer -= uiDiff;

        if (m_uiCrashTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_CRASH) == CAST_OK)
                    m_uiCrashTimer = 15000;
            }
        }
        else
            m_uiCrashTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_ENRAGE, m_creature);
                m_uiEnrageTimer = 30000;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_general_vezax(Creature* pCreature)
{
    return new boss_general_vezaxAI(pCreature);
}

/*######
## npc_saronite_vapor
######*/

struct npc_saronite_vaporAI : public Scripted_NoMovementAI
{
    npc_saronite_vaporAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    void Reset() override { }

    void JustDied(Unit* /*pKiller*/) override
    {
        // inform Vezax of death
        if (m_pInstance)
        {
            if (Creature* pVezax = m_pInstance->GetSingleCreatureFromStorage(NPC_VEZAX))
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pVezax);
        }

        DoCastSpellIfCan(m_creature, SPELL_SARONITE_VAPORS, CAST_TRIGGERED);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId || !m_pInstance)
            return;

        // inform vezax of point reached
        if (Creature* pVezax = m_pInstance->GetSingleCreatureFromStorage(NPC_VEZAX))
            SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pVezax);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
};

CreatureAI* GetAI_npc_saronite_vapor(Creature* pCreature)
{
    return new npc_saronite_vaporAI(pCreature);
}

bool ProcessEventId_event_spell_saronite_barrier(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (pSource->GetTypeId() == TYPEID_UNIT && ((Creature*)pSource)->GetEntry() == NPC_VEZAX)
    {
        if (instance_ulduar* pInstance = (instance_ulduar*)((Creature*)pSource)->GetInstanceData())
        {
            // Start hard mode for Vezax and summon the Animus
            pInstance->SetData(TYPE_VEZAX_HARD, IN_PROGRESS);
            return true;
        }
    }
    return false;
}

void AddSC_boss_general_vezax()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_general_vezax";
    pNewScript->GetAI = &GetAI_boss_general_vezax;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_saronite_vapor";
    pNewScript->GetAI = &GetAI_npc_saronite_vapor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_saronite_barrier";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_saronite_barrier;
    pNewScript->RegisterSelf();
}
