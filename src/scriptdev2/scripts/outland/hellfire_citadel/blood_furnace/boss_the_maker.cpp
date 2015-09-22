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
SDName: Boss_The_Maker
SD%Complete: 80
SDComment: Mind control no support
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "precompiled.h"
#include "blood_furnace.h"

enum
{
    SAY_AGGRO_1                 = -1542009,
    SAY_AGGRO_2                 = -1542010,
    SAY_AGGRO_3                 = -1542011,
    SAY_KILL_1                  = -1542012,
    SAY_KILL_2                  = -1542013,
    SAY_DIE                     = -1542014,

    SPELL_ACID_SPRAY            = 38153,                    // heroic 38973 ??? 38153
    SPELL_EXPLODING_BREAKER     = 30925,
    SPELL_EXPLODING_BREAKER_H   = 40059,
    SPELL_KNOCKDOWN             = 20276,
    SPELL_DOMINATION            = 30923
};

struct boss_the_makerAI : public ScriptedAI
{
    boss_the_makerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiAcidSprayTimer;
    uint32 m_uiExplodingBreakerTimer;
    uint32 m_uiDominationTimer;
    uint32 m_uiKnockdownTimer;

    void Reset() override
    {
        m_uiAcidSprayTimer          = 15000;
        m_uiExplodingBreakerTimer   = 6000;
        m_uiDominationTimer         = 20000;
        m_uiKnockdownTimer          = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_MAKER_EVENT, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_MAKER_EVENT, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DIE, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_MAKER_EVENT, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiAcidSprayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ACID_SPRAY) == CAST_OK)
                m_uiAcidSprayTimer = urand(15000, 23000);
        }
        else
            m_uiAcidSprayTimer -= uiDiff;

        if (m_uiExplodingBreakerTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_EXPLODING_BREAKER : SPELL_EXPLODING_BREAKER_H) == CAST_OK)
                    m_uiExplodingBreakerTimer = urand(4000, 12000);
            }
        }
        else
            m_uiExplodingBreakerTimer -= uiDiff;

        if (m_uiDominationTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DOMINATION) == CAST_OK)
                    m_uiDominationTimer = urand(15000, 25000);
            }
        }
        else
            m_uiDominationTimer -= uiDiff;

        if (m_uiKnockdownTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKDOWN) == CAST_OK)
                m_uiKnockdownTimer = urand(4000, 12000);
        }
        else
            m_uiKnockdownTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_the_makerAI(Creature* pCreature)
{
    return new boss_the_makerAI(pCreature);
}

void AddSC_boss_the_maker()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_the_maker";
    pNewScript->GetAI = &GetAI_boss_the_makerAI;
    pNewScript->RegisterSelf();
}
