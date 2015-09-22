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
SDName: Boss_Maiden_of_Virtue
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    SAY_AGGRO               = -1532018,
    SAY_SLAY1               = -1532019,
    SAY_SLAY2               = -1532020,
    SAY_SLAY3               = -1532021,
    SAY_REPENTANCE1         = -1532022,
    SAY_REPENTANCE2         = -1532023,
    SAY_DEATH               = -1532024,

    SPELL_REPENTANCE        = 29511,
    SPELL_HOLYFIRE          = 29522,
    SPELL_HOLYWRATH         = 32445,
    SPELL_HOLYGROUND        = 29512
};

struct boss_maiden_of_virtueAI : public ScriptedAI
{
    boss_maiden_of_virtueAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiRepentanceTimer;
    uint32 m_uiHolyfireTimer;
    uint32 m_uiHolywrathTimer;
    uint32 m_uiHolygroundTimer;

    void Reset() override
    {
        m_uiRepentanceTimer    = urand(25000, 40000);
        m_uiHolyfireTimer      = urand(8000, 25000);
        m_uiHolywrathTimer     = urand(15000, 25000);
        m_uiHolygroundTimer    = 3000;
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 5))                                // 50% chance to say something out of 3 texts
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAIDEN, DONE);
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

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHolygroundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HOLYGROUND, CAST_TRIGGERED) == CAST_OK)
                m_uiHolygroundTimer = 3000;
        }
        else
            m_uiHolygroundTimer -= uiDiff;

        if (m_uiRepentanceTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_REPENTANCE) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_REPENTANCE1 : SAY_REPENTANCE2, m_creature);
                m_uiRepentanceTimer = urand(25000, 35000);
            }
        }
        else
            m_uiRepentanceTimer -= uiDiff;

        if (m_uiHolyfireTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HOLYFIRE, SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_HOLYFIRE) == CAST_OK)
                    m_uiHolyfireTimer = urand(8000, 23000);
            }
        }
        else
            m_uiHolyfireTimer -= uiDiff;

        if (m_uiHolywrathTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_HOLYWRATH);

            m_uiHolywrathTimer = urand(20000, 25000);
        }
        else
            m_uiHolywrathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_maiden_of_virtue(Creature* pCreature)
{
    return new boss_maiden_of_virtueAI(pCreature);
}

void AddSC_boss_maiden_of_virtue()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_maiden_of_virtue";
    pNewScript->GetAI = &GetAI_boss_maiden_of_virtue;
    pNewScript->RegisterSelf();
}
