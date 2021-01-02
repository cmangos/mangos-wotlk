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
SDName: Boss Ionar
SD%Complete: 80%
SDComment: Timer check
SDCategory: Halls of Lightning
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                               = -1602011,
    SAY_SLAY_1                              = -1602012,
    SAY_SLAY_2                              = -1602013,
    SAY_SLAY_3                              = -1602014,
    SAY_DEATH                               = -1602015,
    SAY_SPLIT_1                             = -1602016,
    SAY_SPLIT_2                             = -1602017,

    SPELL_BALL_LIGHTNING_N                  = 52780,
    SPELL_BALL_LIGHTNING_H                  = 59800,
    SPELL_STATIC_OVERLOAD_N                 = 52658,
    SPELL_STATIC_OVERLOAD_H                 = 59795,

    SPELL_DISPERSE                          = 52770,
    SPELL_SUMMON_SPARK                      = 52746,
    SPELL_SPARK_DESPAWN                     = 52776,

    // Spark of Ionar
    SPELL_SPARK_VISUAL_TRIGGER_N            = 52667,
    SPELL_SPARK_VISUAL_TRIGGER_H            = 59833,

    NPC_SPARK_OF_IONAR                      = 28926,

    MAX_SPARKS                              = 5,
    POINT_CALLBACK                          = 0
};

/*######
## Boss Ionar
######*/

struct boss_ionarAI : public ScriptedAI
{
    boss_ionarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_halls_of_lightning*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_halls_of_lightning* m_pInstance;

    GuidList m_lSparkGUIDList;

    bool m_bIsRegularMode;

    bool m_bIsDesperseCasting;
    bool m_bIsSplitPhase;
    uint32 m_uiSplitTimer;
    uint32 m_uiSparkAtHomeCount;

    uint32 m_uiStaticOverloadTimer;
    uint32 m_uiBallLightningTimer;

    uint32 m_uiHealthAmountModifier;

    void Reset() override
    {
        m_bIsSplitPhase = true;
        m_bIsDesperseCasting = false;
        m_uiSplitTimer = 25000;
        m_uiSparkAtHomeCount = 0;

        m_uiStaticOverloadTimer = urand(5000, 6000);
        m_uiBallLightningTimer = urand(10000, 11000);

        m_uiHealthAmountModifier = 1;

        if (m_creature->GetVisibility() == VISIBILITY_OFF)
            m_creature->SetVisibility(VISIBILITY_ON);
    }

    void AttackedBy(Unit* pAttacker) override
    {
        if (m_creature->GetVictim())
            return;

        if (m_creature->GetVisibility() == VISIBILITY_OFF)
            return;

        AttackStart(pAttacker);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, FAIL);

        DespawnSpark();
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (m_creature->GetVisibility() != VISIBILITY_OFF)
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnSpark();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, DONE);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void DespawnSpark()
    {
        for (GuidList::const_iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->IsAlive())
                    pTemp->ForcedDespawn();
            }
        }

        m_lSparkGUIDList.clear();
    }

    // make sparks come back
    void CallBackSparks()
    {
        for (GuidList::const_iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
        {
            if (Creature* pSpark = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pSpark->IsAlive())
                {
                    // Required to prevent combat movement, elsewise they might switch movement on aggro-change
                    if (ScriptedAI* pSparkAI = dynamic_cast<ScriptedAI*>(pSpark->AI()))
                        pSparkAI->SetCombatMovement(false);

                    pSpark->SetWalk(false);
                    pSpark->GetMotionMaster()->MovePoint(POINT_CALLBACK, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
                }
            }
        }
    }

    void RegisterSparkAtHome()
    {
        ++m_uiSparkAtHomeCount;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPARK_OF_IONAR)
        {
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SPARK_VISUAL_TRIGGER_N : SPELL_SPARK_VISUAL_TRIGGER_H, TRIGGERED_OLD_TRIGGERED);

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);

            m_lSparkGUIDList.push_back(pSummoned->GetObjectGuid());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Splitted
        if (m_creature->GetVisibility() == VISIBILITY_OFF)
        {
            if (m_uiSplitTimer < uiDiff)
            {
                m_uiSplitTimer = 2500;

                // Return sparks to where Ionar splitted
                if (m_bIsSplitPhase)
                {
                    CallBackSparks();
                    m_bIsSplitPhase = false;
                }
                // Lightning effect and restore Ionar
                else if (m_uiSparkAtHomeCount == MAX_SPARKS)
                {
                    m_creature->SetVisibility(VISIBILITY_ON);
                    DoCastSpellIfCan(m_creature, SPELL_SPARK_DESPAWN);

                    m_uiSparkAtHomeCount = 0;
                    m_uiSplitTimer = 25000;
                    m_bIsSplitPhase = true;
                    m_bIsDesperseCasting = false;

                    if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        if (m_creature->GetVictim())
                            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                    }
                }
            }
            else
                m_uiSplitTimer -= uiDiff;

            return;
        }

        if (m_uiStaticOverloadTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_STATIC_OVERLOAD_N : SPELL_STATIC_OVERLOAD_H) == CAST_OK)
                    m_uiStaticOverloadTimer = urand(5000, 6000);
            }
        }
        else
            m_uiStaticOverloadTimer -= uiDiff;

        if (m_uiBallLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BALL_LIGHTNING_N : SPELL_BALL_LIGHTNING_H) == CAST_OK)
                    m_uiBallLightningTimer = urand(10000, 11000);
            }
        }
        else
            m_uiBallLightningTimer -= uiDiff;

        // Health check
        if (m_creature->GetHealthPercent() < float(100 - 20 * m_uiHealthAmountModifier))
        {
            ++m_uiHealthAmountModifier;

            if (!m_bIsDesperseCasting && DoCastSpellIfCan(m_creature, SPELL_DISPERSE, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_SPLIT_1 : SAY_SPLIT_2, m_creature);
                m_bIsDesperseCasting = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

bool EffectDummyCreature_boss_ionar(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_DISPERSE && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() != NPC_IONAR)
            return true;

        for (uint8 i = 0; i < MAX_SPARKS; ++i)
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_SUMMON_SPARK, TRIGGERED_OLD_TRIGGERED);

        pCreatureTarget->AttackStop();
        pCreatureTarget->SetVisibility(VISIBILITY_OFF);

        if (pCreatureTarget->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
            pCreatureTarget->GetMotionMaster()->MovementExpired();

        // always return true when we are handling this spell and effect
        return true;
    }
    if (uiSpellId == SPELL_SPARK_DESPAWN && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCreatureTarget->GetEntry() != NPC_IONAR)
            return true;

        if (boss_ionarAI* pIonarAI = dynamic_cast<boss_ionarAI*>(pCreatureTarget->AI()))
            pIonarAI->DespawnSpark();

        return true;
    }
    return false;
}

/*######
## mob_spark_of_ionar
######*/

struct mob_spark_of_ionarAI : public ScriptedAI
{
    mob_spark_of_ionarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_halls_of_lightning*>(pCreature->GetInstanceData());
        Reset();
    }

    instance_halls_of_lightning* m_pInstance;

    void Reset() override { }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        if (uiPointId == POINT_CALLBACK)
        {
            if (Creature* pIonar = m_pInstance->GetSingleCreatureFromStorage(NPC_IONAR))
            {
                if (!pIonar->IsAlive())
                {
                    m_creature->ForcedDespawn();
                    return;
                }

                if (boss_ionarAI* pIonarAI = dynamic_cast<boss_ionarAI*>(pIonar->AI()))
                    pIonarAI->RegisterSparkAtHome();
            }
            else
                m_creature->ForcedDespawn();
        }
    }
};

void AddSC_boss_ionar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ionar";
    pNewScript->GetAI = &GetNewAIInstance<boss_ionarAI>;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_boss_ionar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_spark_of_ionar";
    pNewScript->GetAI = &GetNewAIInstance<mob_spark_of_ionarAI>;
    pNewScript->RegisterSelf();
}
