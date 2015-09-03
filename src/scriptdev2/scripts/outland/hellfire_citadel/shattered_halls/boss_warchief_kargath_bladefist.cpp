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
SDName: Boss_Warchief_Kargath_Bladefist
SD%Complete: 90
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_warchief_kargath_bladefist
EndContentData */

#include "precompiled.h"
#include "shattered_halls.h"

enum
{
    SAY_AGGRO1                      = -1540042,
    SAY_AGGRO2                      = -1540043,
    SAY_AGGRO3                      = -1540044,
    SAY_SLAY1                       = -1540045,
    SAY_SLAY2                       = -1540046,
    SAY_DEATH                       = -1540047,
    SAY_EVADE                       = -1540048,

    SPELL_BLADE_DANCE               = 30739,
    SPELL_CHARGE_H                  = 25821,

    TARGET_NUM                      = 5,

    NPC_SHATTERED_ASSASSIN          = 17695,
    NPC_HEARTHEN_GUARD              = 17621,
    NPC_SHARPSHOOTER_GUARD          = 17622,
    NPC_REAVER_GUARD                = 17623,
};

float AssassEntrance[3] = {275.136f, -84.29f, 2.3f};        // y -8
float AssassExit[3] = {184.233f, -84.29f, 2.3f};            // y -8
float AddsEntrance[3] = {306.036f, -84.29f, 1.93f};

struct boss_warchief_kargath_bladefistAI : public ScriptedAI
{
    boss_warchief_kargath_bladefistAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    GuidVector m_vAddGuids;
    GuidVector m_vAssassinGuids;

    uint32 m_uiChargeTimer;
    uint32 m_uiBladeDanceTimer;
    uint32 m_uiSummonAssistantTimer;
    uint32 m_uiWaitTimer;

    uint32 m_uiAssassinsTimer;

    uint32 m_uiSummoned;
    bool m_bInBlade;

    uint32 m_uiTargetNum;

    void Reset() override
    {
        m_creature->SetSpeedRate(MOVE_RUN, 2.0f);

        m_uiSummoned = 2;
        m_bInBlade = false;
        m_uiWaitTimer = 0;

        m_uiChargeTimer = 0;
        m_uiBladeDanceTimer = 45000;
        m_uiSummonAssistantTimer = 30000;
        m_uiAssassinsTimer = 5000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_HEARTHEN_GUARD:
            case NPC_SHARPSHOOTER_GUARD:
            case NPC_REAVER_GUARD:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);

                m_vAddGuids.push_back(pSummoned->GetObjectGuid());
                break;
            case NPC_SHATTERED_ASSASSIN:
                m_vAssassinGuids.push_back(pSummoned->GetObjectGuid());
                break;
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        DoDespawnAdds();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, DONE);
    }

    void JustReachedHome() override
    {
        DoDespawnAdds();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BLADEFIST, FAIL);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (m_bInBlade)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            if (uiPointId != 1)
                return;

            if (m_uiTargetNum > 0) // to prevent loops
            {
                m_uiWaitTimer = 1;
                DoCastSpellIfCan(m_creature, SPELL_BLADE_DANCE, CAST_TRIGGERED);
                --m_uiTargetNum;
            }
        }
    }

    // Note: this should be done by creature linkin in core
    void DoDespawnAdds()
    {
        for (GuidVector::const_iterator itr = m_vAddGuids.begin(); itr != m_vAddGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_vAddGuids.clear();

        for (GuidVector::const_iterator itr = m_vAssassinGuids.begin(); itr != m_vAssassinGuids.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }

        m_vAssassinGuids.clear();
    }

    void SpawnAssassin()
    {
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] + 8, AssassEntrance[2], 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] - 8, AssassEntrance[2], 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] + 8, AssassExit[2], 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 24000);
        m_creature->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] - 8, AssassExit[2], 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 24000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Check if out of range
        if (EnterEvadeIfOutOfCombatArea(uiDiff))
        {
            DoScriptText(SAY_EVADE, m_creature);
            return;
        }

        if (m_uiAssassinsTimer)
        {
            if (m_uiAssassinsTimer <= uiDiff)
            {
                SpawnAssassin();
                m_uiAssassinsTimer = 0;
            }
            else
                m_uiAssassinsTimer -= uiDiff;
        }

        if (m_bInBlade)
        {
            if (m_uiWaitTimer)
            {
                if (m_uiWaitTimer <= uiDiff)
                {
                    if (m_uiTargetNum == 0)
                    {
                        // stop bladedance
                        m_bInBlade = false;
                        m_creature->SetSpeedRate(MOVE_RUN, 2.0f);
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                        m_uiWaitTimer = 0;
                        if (!m_bIsRegularMode)
                            m_uiChargeTimer = 5000;
                    }
                    else
                    {
                        // move in bladedance
                        float x, y, randx, randy;
                        randx = (rand() % 40);
                        randy = (rand() % 40);
                        x = 210 + randx ;
                        y = -60 - randy ;
                        m_creature->GetMotionMaster()->MovePoint(1, x, y, m_creature->GetPositionZ());
                        m_uiWaitTimer = 0;
                    }
                }
                else
                    m_uiWaitTimer -= uiDiff;
            }
        }
        else                                                // !m_bInBlade
        {
            if (m_uiBladeDanceTimer < uiDiff)
            {
                m_uiTargetNum = TARGET_NUM;
                m_uiWaitTimer = 1;
                m_bInBlade = true;
                m_uiBladeDanceTimer = 30000;
                m_creature->SetSpeedRate(MOVE_RUN, 4.0f);
                return;
            }
            else
                m_uiBladeDanceTimer -= uiDiff;

            if (m_uiChargeTimer)
            {
                if (m_uiChargeTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(pTarget, SPELL_CHARGE_H);

                    m_uiChargeTimer = 0;
                }
                else
                    m_uiChargeTimer -= uiDiff;
            }

            if (m_uiSummonAssistantTimer < uiDiff)
            {
                for (uint32 i = 0; i < m_uiSummoned; ++i)
                {
                    switch (urand(0, 2))
                    {
                        case 0: m_creature->SummonCreature(NPC_HEARTHEN_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 20000); break;
                        case 1: m_creature->SummonCreature(NPC_SHARPSHOOTER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 20000); break;
                        case 2: m_creature->SummonCreature(NPC_REAVER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 20000); break;
                    }
                }

                if (!urand(0, 4))
                    ++m_uiSummoned;

                m_uiSummonAssistantTimer = urand(25000, 35000);
            }
            else
                m_uiSummonAssistantTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_warchief_kargath_bladefist(Creature* pCreature)
{
    return new boss_warchief_kargath_bladefistAI(pCreature);
}

void AddSC_boss_warchief_kargath_bladefist()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_warchief_kargath_bladefist";
    pNewScript->GetAI = &GetAI_boss_warchief_kargath_bladefist;
    pNewScript->RegisterSelf();
}
