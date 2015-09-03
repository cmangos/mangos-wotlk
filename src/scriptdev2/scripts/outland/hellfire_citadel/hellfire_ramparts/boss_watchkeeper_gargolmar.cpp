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
SDName: Boss_Watchkeeper_Gargolmar
SD%Complete: 80
SDComment: Missing adds to heal him. Surge should be used on pTarget furthest away, not random.
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_TAUNT                   = -1543000,
    SAY_HEAL                    = -1543001,
    SAY_SURGE                   = -1543002,
    SAY_AGGRO_1                 = -1543003,
    SAY_AGGRO_2                 = -1543004,
    SAY_AGGRO_3                 = -1543005,
    SAY_KILL_1                  = -1543006,
    SAY_KILL_2                  = -1543007,
    SAY_DIE                     = -1543008,

    SPELL_MORTAL_WOUND          = 30641,
    SPELL_MORTAL_WOUND_H        = 36814,
    SPELL_SURGE                 = 34645,
    SPELL_RETALIATION           = 22857,
    SPELL_OVERPOWER             = 32154,
};

struct boss_watchkeeper_gargolmarAI : public ScriptedAI
{
    boss_watchkeeper_gargolmarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiSurgeTimer;
    uint32 m_uiMortalWoundTimer;
    uint32 m_uiRetaliationTimer;
    uint32 m_uiOverpowerTimer;

    bool m_bHasTaunted;
    bool m_bYelledForHeal;

    void Reset() override
    {
        m_uiSurgeTimer = urand(2400, 6100);
        m_uiMortalWoundTimer = urand(3500, 14400);
        m_uiRetaliationTimer = 0;
        m_uiOverpowerTimer = urand(3600, 14800);

        m_bHasTaunted = false;
        m_bYelledForHeal = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && m_creature->IsWithinDistInMap(pWho, 60.0f))
        {
            DoScriptText(SAY_TAUNT, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DIE, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMortalWoundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MORTAL_WOUND : SPELL_MORTAL_WOUND_H) == CAST_OK)
                m_uiMortalWoundTimer = urand(6100, 12200);
        }
        else
            m_uiMortalWoundTimer -= uiDiff;

        if (m_uiSurgeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SURGE) == CAST_OK)
                {
                    DoScriptText(SAY_SURGE, m_creature);
                    m_uiSurgeTimer = urand(12100, 21700);
                }
            }
        }
        else
            m_uiSurgeTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 20.0f)
        {
            if (m_uiRetaliationTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_RETALIATION) == CAST_OK)
                    m_uiRetaliationTimer = 30000;
            }
            else
                m_uiRetaliationTimer -= uiDiff;
        }

        if (m_uiOverpowerTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_OVERPOWER) == CAST_OK)
                m_uiOverpowerTimer = urand(18100, 33700);
        }
        else
            m_uiOverpowerTimer -= uiDiff;

        if (!m_bYelledForHeal)
        {
            if (m_creature->GetHealthPercent() < 40.0f)
            {
                DoScriptText(SAY_HEAL, m_creature);
                m_bYelledForHeal = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_watchkeeper_gargolmarAI(Creature* pCreature)
{
    return new boss_watchkeeper_gargolmarAI(pCreature);
}

void AddSC_boss_watchkeeper_gargolmar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_watchkeeper_gargolmar";
    pNewScript->GetAI = &GetAI_boss_watchkeeper_gargolmarAI;
    pNewScript->RegisterSelf();
}
