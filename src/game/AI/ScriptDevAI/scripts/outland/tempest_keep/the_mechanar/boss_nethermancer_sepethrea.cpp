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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 95
SDComment: May need some small adjustments
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "mechanar.h"

enum
{
    SAY_AGGRO                       = -1554013,
    SAY_SUMMON                      = -1554014,
    SAY_DRAGONS_BREATH_1            = -1554015,
    SAY_DRAGONS_BREATH_2            = -1554016,
    SAY_SLAY1                       = -1554017,
    SAY_SLAY2                       = -1554018,
    SAY_DEATH                       = -1554019,

    SPELL_SUMMON_RAGING_FLAMES      = 35275,
    SPELL_SUMMON_RAGING_FLAMES_H    = 39084,
    SPELL_FROST_ATTACK              = 45195,
    SPELL_ARCANE_BLAST              = 35314,
    SPELL_DRAGONS_BREATH            = 35250,

    NPC_RAGING_FLAMES               = 20481,
};

struct boss_nethermancer_sepethreaAI : public ScriptedAI
{
    boss_nethermancer_sepethreaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFrostAttackTimer;
    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiDragonsBreathTimer;

    void Reset() override
    {
        m_uiFrostAttackTimer    = urand(8000, 17000);
        m_uiArcaneBlastTimer    = urand(14000, 25000);
        m_uiDragonsBreathTimer  = urand(20000, 26000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_RAGING_FLAMES : SPELL_SUMMON_RAGING_FLAMES_H);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_RAGING_FLAMES)
        {
            pSummoned->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
            pSummoned->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);

            // ToDo: need to fixate target and make them walk!
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->GetMotionMaster()->MoveChase(pTarget);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frost Attack
        if (m_uiFrostAttackTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_ATTACK) == CAST_OK)
                m_uiFrostAttackTimer = urand(5000, 17000);
        }
        else
            m_uiFrostAttackTimer -= uiDiff;

        // Arcane Blast
        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_BLAST) == CAST_OK)
                m_uiArcaneBlastTimer = urand(15000, 30000);
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // Dragons Breath
        if (m_uiDragonsBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DRAGONS_BREATH) == CAST_OK)
            {
                if (urand(0, 1))
                    DoScriptText(urand(0, 1) ? SAY_DRAGONS_BREATH_1 : SAY_DRAGONS_BREATH_2, m_creature);

                m_uiDragonsBreathTimer = urand(20000, 35000);
            }
        }
        else
            m_uiDragonsBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nethermancer_sepethrea(Creature* pCreature)
{
    return new boss_nethermancer_sepethreaAI(pCreature);
}

void AddSC_boss_nethermancer_sepethrea()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_nethermancer_sepethrea";
    pNewScript->GetAI = &GetAI_boss_nethermancer_sepethrea;
    pNewScript->RegisterSelf();
}
