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
SDName: Boss_Sladran
SD%Complete: 75%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    SAY_AGGRO                 = -1604000,
    SAY_SUMMON_SNAKE          = -1604001,
    SAY_SUMMON_CONSTRICTOR    = -1604002,
    SAY_SLAY_1                = -1604003,
    SAY_SLAY_2                = -1604004,
    SAY_SLAY_3                = -1604005,
    SAY_DEATH                 = -1604006,
    EMOTE_NOVA                = -1604007,

    // Slad'Ran spells
    SPELL_POISON_NOVA         = 55081,
    SPELL_POISON_NOVA_H       = 59842,
    SPELL_POWERFUL_BITE       = 48287,
    SPELL_POWERFUL_BITE_H     = 59840,
    SPELL_VENOM_BOLT          = 54970,
    SPELL_VENOM_BOLT_H        = 59839,

    // Summon spells
    SPELL_SUMMON_VIPER        = 55060,
    SPELL_SUMMON_CONSTRICTOR  = 54969,

    // Constrictor spells
    SPELL_GRIP_OF_SLADRAN     = 55093,
    SPELL_GRIP_OF_SLADRAN_H   = 61474,

    // Snake Wrap spells - mechanics unk
    SPELL_SNAKE_WRAP          = 55099,
    SPELL_SNAKE_WRAP_H        = 61475,
    SPELL_SNAKE_WRAP_SUMMON   = 55126,
    SPELL_SNAKE_WRAP_SUMMON_H = 61476,
    SPELL_SNAKE_WRAP_EFFECT   = 55128,
    SPELL_SNAKE_WRAP_SNAKES   = 55127,              // kills all snakes

    NPC_SLADRAN_CONSTRICTOR   = 29713,
    NPC_SLADRAN_VIPER         = 29680,
    NPC_SNAKE_WRAP            = 29742,
};

/*######
## boss_sladran
######*/

struct boss_sladranAI : public ScriptedAI
{
    boss_sladranAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiPoisonNovaTimer;
    uint32 m_uiPowerfulBiteTimer;
    uint32 m_uiVenomBoltTimer;

    void Reset() override
    {
        m_uiSummonTimer       = m_bIsRegularMode ? 5000 : 3000;
        m_uiPoisonNovaTimer   = 22000;
        m_uiPowerfulBiteTimer = 10000;
        m_uiVenomBoltTimer    = 15000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, IN_PROGRESS);
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

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() != NPC_SLADRAN_CONSTRICTOR && pSummoned->GetEntry() != NPC_SLADRAN_VIPER)
            return;

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPoisonNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_POISON_NOVA : SPELL_POISON_NOVA_H) == CAST_OK)
            {
                DoScriptText(EMOTE_NOVA, m_creature);
                m_uiPoisonNovaTimer = 22000;
            }
        }
        else
            m_uiPoisonNovaTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            if (!m_pInstance)
                return;

            if (Creature* pSummonTarget = m_creature->GetMap()->GetCreature(m_pInstance->SelectRandomSladranTargetGuid()))
            {
                if (urand(0, 3))
                {
                    // we don't want to get spammed
                    if (!urand(0, 4))
                        DoScriptText(SAY_SUMMON_CONSTRICTOR, m_creature);

                    pSummonTarget->CastSpell(pSummonTarget, SPELL_SUMMON_CONSTRICTOR, false, NULL, NULL, m_creature->GetObjectGuid());
                }
                else
                {
                    // we don't want to get spammed
                    if (!urand(0, 4))
                        DoScriptText(SAY_SUMMON_SNAKE, m_creature);

                    pSummonTarget->CastSpell(pSummonTarget, SPELL_SUMMON_VIPER, false, NULL, NULL, m_creature->GetObjectGuid());
                }
            }

            m_uiSummonTimer = m_bIsRegularMode ? 5000 : 3000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        if (m_uiPowerfulBiteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POWERFUL_BITE : SPELL_POWERFUL_BITE_H) == CAST_OK)
                m_uiPowerfulBiteTimer = 10000;
        }
        else
            m_uiPowerfulBiteTimer -= uiDiff;

        if (m_uiVenomBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_VENOM_BOLT : SPELL_VENOM_BOLT_H) == CAST_OK)
                    m_uiVenomBoltTimer = 15000;
            }
        }
        else
            m_uiVenomBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sladran(Creature* pCreature)
{
    return new boss_sladranAI(pCreature);
}

void AddSC_boss_sladran()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_sladran";
    pNewScript->GetAI = &GetAI_boss_sladran;
    pNewScript->RegisterSelf();
}
