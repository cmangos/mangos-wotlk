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
SDName: Boss_Harbinger_Skyriss
SD%Complete: 95
SDComment: Timers will need adjustments.
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "arcatraz.h"

enum
{
    SAY_KILL_1                  = -1552002,
    SAY_KILL_2                  = -1552003,
    SAY_MIND_1                  = -1552004,
    SAY_MIND_2                  = -1552005,
    SAY_FEAR_1                  = -1552006,
    SAY_FEAR_2                  = -1552007,
    SAY_IMAGE                   = -1552008,
    SAY_DEATH                   = -1552009,

    SPELL_FEAR                  = 39415,
    SPELL_MIND_REND             = 36924,
    SPELL_MIND_REND_H           = 39017,
    SPELL_DOMINATION            = 37162,
    SPELL_DOMINATION_H          = 39019,
    SPELL_MANA_BURN_H           = 39020,
    SPELL_66_ILLUSION           = 36931,                    // Summons 21466
    SPELL_33_ILLUSION           = 36932,                    // Summons 21467

    SPELL_BIRTH                 = 26262,                    // Cast by illusions on spawn in EAI
    SPELL_BLINK_VISUAL          = 36937,
};

struct boss_harbinger_skyrissAI : public ScriptedAI
{
    boss_harbinger_skyrissAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiSplitPhase;
    uint32 m_uiMindRendTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiDominationTimer;
    uint32 m_uiManaBurnTimer;

    void Reset() override
    {
        m_uiSplitPhase      = 1;
        m_uiMindRendTimer   = 3000;
        m_uiFearTimer       = 15000;
        m_uiDominationTimer = 30000;
        m_uiManaBurnTimer   = 25000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HARBINGERSKYRISS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HARBINGERSKYRISS, FAIL);
    }

    void KilledUnit(Unit* pVictim) override
    {
        // won't yell killing pet/other unit
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->GetVictim())
            pSummoned->AI()->AttackStart(m_creature->GetVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Check if creature is below 66% or 33%; Also don't allow it to split the third time
        if (m_creature->GetHealthPercent() < 100 - 33 * m_uiSplitPhase && m_creature->GetHealthPercent() > 5.0f)
        {
            DoCastSpellIfCan(m_creature, m_uiSplitPhase == 1 ? SPELL_66_ILLUSION : SPELL_33_ILLUSION, CAST_INTERRUPT_PREVIOUS);
            DoScriptText(SAY_IMAGE, m_creature);
            ++m_uiSplitPhase;
        }

        if (m_uiMindRendTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (!pTarget)
                pTarget = m_creature->GetVictim();

            if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_MIND_REND : SPELL_MIND_REND_H) == CAST_OK)
                m_uiMindRendTimer = 8000;
        }
        else
            m_uiMindRendTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (!pTarget)
                pTarget = m_creature->GetVictim();

            if (DoCastSpellIfCan(pTarget, SPELL_FEAR) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_FEAR_1 : SAY_FEAR_2, m_creature);
                m_uiFearTimer = 25000;
            }
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiDominationTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_DOMINATION : SPELL_DOMINATION_H) == CAST_OK)
                {
                    DoScriptText(urand(0, 1) ? SAY_MIND_1 : SAY_MIND_2, m_creature);
                    m_uiDominationTimer = urand(16000, 32000);
                }
            }
        }
        else
            m_uiDominationTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiManaBurnTimer < uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA);
                if (!pTarget)
                    pTarget = m_creature->GetVictim();

                if (DoCastSpellIfCan(pTarget, SPELL_MANA_BURN_H) == CAST_OK)
                    m_uiManaBurnTimer = urand(16000, 32000);
            }
            else
                m_uiManaBurnTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_harbinger_skyriss(Creature* pCreature)
{
    return new boss_harbinger_skyrissAI(pCreature);
}

void AddSC_boss_harbinger_skyriss()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_harbinger_skyriss";
    pNewScript->GetAI = &GetAI_boss_harbinger_skyriss;
    pNewScript->RegisterSelf();
}
