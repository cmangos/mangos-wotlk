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
SDName: Boss_Maiden_of_Grief
SD%Complete: 60%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

enum
{
    SAY_AGGRO                   = -1599005,
    SAY_SLAY_1                  = -1599006,
    SAY_SLAY_2                  = -1599007,
    SAY_SLAY_3                  = -1599008,
    SAY_SLAY_4                  = -1599009,
    SAY_STUN                    = -1599010,
    SAY_DEATH                   = -1599011,

    SPELL_STORM_OF_GRIEF        = 50752,
    SPELL_STORM_OF_GRIEF_H      = 59772,

    SPELL_SHOCK_OF_SORROW       = 50760,
    SPELL_SHOCK_OF_SORROW_H     = 59726,

    SPELL_PILLAR_OF_WOE         = 50761,
    SPELL_PILLAR_OF_WOE_H       = 59727,

    SPELL_PARTING_SORROW        = 59723
};

/*######
## boss_maiden_of_grief
######*/

struct boss_maiden_of_griefAI : public ScriptedAI
{
    boss_maiden_of_griefAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_halls_of_stone*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_halls_of_stone* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStormTimer;
    uint32 m_uiShockTimer;
    uint32 m_uiPillarTimer;
    uint32 m_uiPartingSorrowTimer;

    void Reset() override
    {
        m_uiStormTimer = 5000;
        m_uiShockTimer = 10000;
        m_uiPillarTimer = 15000;
        m_uiPartingSorrowTimer = 12000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPartingSorrowTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PARTING_SORROW, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_PARTING_SORROW) == CAST_OK)
                    m_uiPartingSorrowTimer = 12000 + rand() % 5000;
            }
        }
        else
            m_uiPartingSorrowTimer -= uiDiff;

        if (m_uiStormTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_STORM_OF_GRIEF : SPELL_STORM_OF_GRIEF_H) == CAST_OK)
                m_uiStormTimer = 20000;
        }
        else
            m_uiStormTimer -= uiDiff;

        if (m_uiPillarTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_bIsRegularMode ? SPELL_PILLAR_OF_WOE_H : SPELL_PILLAR_OF_WOE, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_PILLAR_OF_WOE : SPELL_PILLAR_OF_WOE_H) == CAST_OK)
                    m_uiPillarTimer = 10000;
            }
        }
        else
            m_uiPillarTimer -= uiDiff;

        if (m_uiShockTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHOCK_OF_SORROW : SPELL_SHOCK_OF_SORROW_H) == CAST_OK)
            {
                DoScriptText(SAY_STUN, m_creature);
                m_uiShockTimer = 35000;
            }
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_maiden_of_grief(Creature* pCreature)
{
    return new boss_maiden_of_griefAI(pCreature);
}

void AddSC_boss_maiden_of_grief()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_maiden_of_grief";
    pNewScript->GetAI = &GetAI_boss_maiden_of_grief;
    pNewScript->RegisterSelf();
}
