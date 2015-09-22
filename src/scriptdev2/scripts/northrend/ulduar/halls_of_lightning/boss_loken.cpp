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
SDName: Boss Loken
SD%Complete: 80%
SDComment: Missing intro.
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                           = -1602018,
    SAY_INTRO_1                         = -1602019,
    SAY_INTRO_2                         = -1602020,
    SAY_SLAY_1                          = -1602021,
    SAY_SLAY_2                          = -1602022,
    SAY_SLAY_3                          = -1602023,
    SAY_DEATH                           = -1602024,
    SAY_NOVA_1                          = -1602025,
    SAY_NOVA_2                          = -1602026,
    SAY_NOVA_3                          = -1602027,
    SAY_75HEALTH                        = -1602028,
    SAY_50HEALTH                        = -1602029,
    SAY_25HEALTH                        = -1602030,
    EMOTE_NOVA                          = -1602031,

    SPELL_ARC_LIGHTNING                 = 52921,
    SPELL_LIGHTNING_NOVA                = 52960,
    SPELL_LIGHTNING_NOVA_H              = 59835,

    SPELL_PULSING_SHOCKWAVE             = 52961,
    SPELL_PULSING_SHOCKWAVE_H           = 59836,
    SPELL_PULSING_SHOCKWAVE_AURA        = 59414
};

/*######
## Boss Loken
######*/

struct boss_lokenAI : public ScriptedAI
{
    boss_lokenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;

    uint32 m_uiArcLightningTimer;
    uint32 m_uiLightningNovaTimer;

    uint32 m_uiHealthAmountModifier;

    void Reset() override
    {
        m_uiArcLightningTimer = 15000;
        m_uiLightningNovaTimer = 20000;

        m_uiHealthAmountModifier = 1;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOKEN, IN_PROGRESS);

        // Cast Pulsing Shockwave at aggro - ToDo: enable this when the core will properly support this spell
        // DoCastSpellIfCan(m_creature, SPELL_PULSING_SHOCKWAVE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        // DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_PULSING_SHOCKWAVE : SPELL_PULSING_SHOCKWAVE_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOKEN, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOKEN, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ARC_LIGHTNING) == CAST_OK)
                    m_uiArcLightningTimer = urand(15000, 16000);
            }
        }
        else
            m_uiArcLightningTimer -= uiDiff;

        if (m_uiLightningNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_NOVA : SPELL_LIGHTNING_NOVA_H) == CAST_OK)
            {
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_NOVA_1, m_creature); break;
                    case 1: DoScriptText(SAY_NOVA_2, m_creature); break;
                    case 2: DoScriptText(SAY_NOVA_3, m_creature); break;
                }
                m_uiLightningNovaTimer = urand(20000, 21000);
            }
        }
        else
            m_uiLightningNovaTimer -= uiDiff;

        // Health check
        if (m_creature->GetHealthPercent() < float(100 - 25 * m_uiHealthAmountModifier))
        {
            switch (m_uiHealthAmountModifier)
            {
                case 1: DoScriptText(SAY_75HEALTH, m_creature); break;
                case 2: DoScriptText(SAY_50HEALTH, m_creature); break;
                case 3: DoScriptText(SAY_25HEALTH, m_creature); break;
            }

            ++m_uiHealthAmountModifier;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_loken(Creature* pCreature)
{
    return new boss_lokenAI(pCreature);
}

void AddSC_boss_loken()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_loken";
    pNewScript->GetAI = &GetAI_boss_loken;
    pNewScript->RegisterSelf();
}
