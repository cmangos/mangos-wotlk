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
SDName: Boss_Trollgore
SD%Complete: 80%
SDComment: Some details related to the summoned creatures need more adjustments
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_AGGRO                       = -1600000,
    SAY_CONSUME                     = -1600001,
    SAY_DEATH                       = -1600002,
    SAY_EXPLODE                     = -1600003,
    SAY_KILL                        = -1600004,

    SPELL_CRUSH                     = 49639,
    SPELL_INFECTED_WOUND            = 49637,
    SPELL_CORPSE_EXPLODE            = 49555,
    SPELL_CORPSE_EXPLODE_H          = 59807,
    SPELL_CONSUME                   = 49380,
    SPELL_CONSUME_H                 = 59803,
    SPELL_CONSUME_BUFF              = 49381,            // used to measure the achiev
    SPELL_CONSUME_BUFF_H            = 59805,

    SPELL_SUMMON_INVADER_1          = 49456,            // summon 27709
    SPELL_SUMMON_INVADER_2          = 49457,            // summon 27753
    // SPELL_SUMMON_INVADER_3        = 49458,            // summon 27754
    SPELL_INVADER_TAUNT             = 49405,            // triggers 49406

    MAX_CONSOME_STACKS              = 10,
};

/*######
## boss_trollgore
######*/

struct boss_trollgoreAI : public ScriptedAI
{
    boss_trollgoreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_draktharon_keep*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_draktharon_keep* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiConsumeStacks;

    uint32 m_uiConsumeTimer;
    uint32 m_uiCrushTimer;
    uint32 m_uiInfectedWoundTimer;
    uint32 m_uiWaveTimer;
    uint32 m_uiCorpseExplodeTimer;

    GuidVector m_vTriggers;

    void Reset() override
    {
        m_uiCorpseExplodeTimer  = 20000;
        m_uiConsumeTimer        = 15000;
        m_uiCrushTimer          = 10000;
        m_uiInfectedWoundTimer  = 5000;
        m_uiWaveTimer           = 0;
        m_uiConsumeStacks       = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_TROLLGORE, IN_PROGRESS);
            m_pInstance->GetTrollgoreOutsideTriggers(m_vTriggers);
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetCharmerOrOwnerPlayerOrPlayerItself())
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TROLLGORE, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TROLLGORE, FAIL);
    }

    void SpellHit(Unit* /*pTarget*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_CONSUME_BUFF || pSpell->Id == SPELL_CONSUME_BUFF_H)
        {
            ++m_uiConsumeStacks;

            // if the boss has 10 stacks then set the achiev to fail
            if (m_uiConsumeStacks == MAX_CONSOME_STACKS)
            {
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_TROLLGORE, SPECIAL);
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // This spell taunts the boss and the boss taunts back
        pSummoned->CastSpell(m_creature, SPELL_INVADER_TAUNT, true);
    }

    // Wrapper to handle the drakkari invaders summon
    void DoSummonDrakkariInvaders()
    {
        if (!m_pInstance)
            return;

        // check if there are there are at least 2 triggers in the vector
        if (m_vTriggers.size() < 2)
            return;

        if (roll_chance_i(30))
        {
            // Summon a troll in the corner and 2 trolls in the air
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_pInstance->GetTrollgoreCornerTrigger()))
                pTrigger->CastSpell(pTrigger, roll_chance_i(20) ? SPELL_SUMMON_INVADER_1 : SPELL_SUMMON_INVADER_2, true, NULL, NULL, m_creature->GetObjectGuid());

            // get two random outside triggers
            uint8 uiMaxTriggers = m_vTriggers.size();
            uint8 uiPos1 = urand(0, uiMaxTriggers  - 1);
            uint8 uiPos2 = (uiPos1 + urand(1, uiMaxTriggers  - 1)) % uiMaxTriggers;

            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_vTriggers[uiPos1]))
                pTrigger->CastSpell(pTrigger, roll_chance_i(30) ? SPELL_SUMMON_INVADER_1 : SPELL_SUMMON_INVADER_2, true, NULL, NULL, m_creature->GetObjectGuid());
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_vTriggers[uiPos2]))
                pTrigger->CastSpell(pTrigger, roll_chance_i(30) ? SPELL_SUMMON_INVADER_1 : SPELL_SUMMON_INVADER_2, true, NULL, NULL, m_creature->GetObjectGuid());
        }
        else
        {
            // Summon 3 trolls in the air
            for (uint8 i = 0; i < m_vTriggers.size(); ++i)
            {
                if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_vTriggers[i]))
                    pTrigger->CastSpell(pTrigger, roll_chance_i(30) ? SPELL_SUMMON_INVADER_1 : SPELL_SUMMON_INVADER_2, true, NULL, NULL, m_creature->GetObjectGuid());
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCrushTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSH) == CAST_OK)
                m_uiCrushTimer = 10000;
        }
        else
            m_uiCrushTimer -= uiDiff;

        if (m_uiInfectedWoundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INFECTED_WOUND) == CAST_OK)
                m_uiInfectedWoundTimer = urand(20000, 30000);
        }
        else
            m_uiInfectedWoundTimer -= uiDiff;

        if (m_uiWaveTimer < uiDiff)
        {
            DoSummonDrakkariInvaders();
            m_uiWaveTimer = 30000;
        }
        else
            m_uiWaveTimer -= uiDiff;

        if (m_uiConsumeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature,  m_bIsRegularMode ? SPELL_CONSUME : SPELL_CONSUME_H) == CAST_OK)
            {
                DoScriptText(SAY_CONSUME, m_creature);
                m_uiConsumeTimer = 15000;
            }
        }
        else
            m_uiConsumeTimer -= uiDiff;

        if (m_uiCorpseExplodeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature,  m_bIsRegularMode ? SPELL_CORPSE_EXPLODE : SPELL_CORPSE_EXPLODE_H) == CAST_OK)
            {
                DoScriptText(SAY_EXPLODE, m_creature);
                m_uiCorpseExplodeTimer = 10000;
            }
        }
        else
            m_uiCorpseExplodeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_trollgore(Creature* pCreature)
{
    return new boss_trollgoreAI(pCreature);
}

void AddSC_boss_trollgore()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_trollgore";
    pNewScript->GetAI = &GetAI_boss_trollgore;
    pNewScript->RegisterSelf();
}
