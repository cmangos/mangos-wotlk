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
SDName: Boss_Darkweaver_Syth
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_SUMMON                  = -1556000,
    SAY_AGGRO_1                 = -1556001,
    SAY_AGGRO_2                 = -1556002,
    SAY_AGGRO_3                 = -1556003,
    SAY_SLAY_1                  = -1556004,
    SAY_SLAY_2                  = -1556005,
    SAY_DEATH                   = -1556006,

    SPELL_FROST_SHOCK           = 12548,
    SPELL_FROST_SHOCK_H         = 21401,
    SPELL_FLAME_SHOCK           = 15039,
    SPELL_FLAME_SHOCK_H         = 15616,
    SPELL_SHADOW_SHOCK          = 33620,
    SPELL_SHADOW_SHOCK_H        = 38136,
    SPELL_ARCANE_SHOCK          = 33534,
    SPELL_ARCANE_SHOCK_H        = 38135,

    SPELL_CHAIN_LIGHTNING       = 15659,
    SPELL_CHAIN_LIGHTNING_H     = 15305,

    SPELL_SUMMON_SYTH_FIRE      = 33537,                    // Spawns 19203
    SPELL_SUMMON_SYTH_ARCANE    = 33538,                    // Spawns 19205
    SPELL_SUMMON_SYTH_FROST     = 33539,                    // Spawns 19204
    SPELL_SUMMON_SYTH_SHADOW    = 33540,                    // Spawns 19206

    // Npc entries
    NPC_FIRE_ELEMENTAL          = 19203,
    NPC_FROST_ELEMENTAL         = 19204,
    NPC_ARCANE_ELEMENTAL        = 19205,
    NPC_SHADOW_ELEMENTAL        = 19206,
};

struct boss_darkweaver_sythAI : public ScriptedAI
{
    boss_darkweaver_sythAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiFlameshockTimer;
    uint32 m_uiArcaneshockTimer;
    uint32 m_uiFrostshockTimer;
    uint32 m_uiShadowshockTimer;
    uint32 m_uiChainlightningTimer;

    float m_fHpCheck;

    void Reset() override
    {
        m_uiFlameshockTimer     = 18000;
        m_uiArcaneshockTimer    = 19000;
        m_uiFrostshockTimer     = 18000;
        m_uiShadowshockTimer    = 17000;
        m_uiChainlightningTimer = urand(6000, 9000);

        m_fHpCheck              = 90.0f;
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

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            return;

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_FIRE_ELEMENTAL:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                break;
            case NPC_FROST_ELEMENTAL:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                break;
            case NPC_ARCANE_ELEMENTAL:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                break;
            case NPC_SHADOW_ELEMENTAL:
                pSummoned->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
                break;
        }

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    // Wrapper to handle the elementals summon
    void SythSummoning()
    {
        DoScriptText(SAY_SUMMON, m_creature);

        if (m_creature->IsNonMeleeSpellCasted(false))
            m_creature->InterruptNonMeleeSpells(false);

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SYTH_ARCANE, CAST_TRIGGERED); // front
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SYTH_FIRE,   CAST_TRIGGERED); // back
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SYTH_FROST,  CAST_TRIGGERED); // left
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_SYTH_SHADOW, CAST_TRIGGERED); // right
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Summon elementals at 90%, 50% and 10% health
        if (m_creature->GetHealthPercent() < m_fHpCheck)
        {
            SythSummoning();
            m_fHpCheck -= 40.0f;
        }

        if (m_uiFlameshockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FLAME_SHOCK : SPELL_FLAME_SHOCK_H) == CAST_OK)
                    m_uiFlameshockTimer = m_bIsRegularMode ? urand(13000, 28000) : urand(11000, 20000);
            }
        }
        else
            m_uiFlameshockTimer -= uiDiff;

        if (m_uiArcaneshockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H) == CAST_OK)
                    m_uiArcaneshockTimer = m_bIsRegularMode ? urand(13000, 28000) : urand(11000, 20000);
            }
        }
        else
            m_uiArcaneshockTimer -= uiDiff;

        if (m_uiFrostshockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FROST_SHOCK : SPELL_FROST_SHOCK_H) == CAST_OK)
                    m_uiFrostshockTimer = m_bIsRegularMode ? urand(13000, 28000) : urand(11000, 20000);
            }
        }
        else
            m_uiFrostshockTimer -= uiDiff;

        if (m_uiShadowshockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHADOW_SHOCK : SPELL_SHADOW_SHOCK_H) == CAST_OK)
                    m_uiShadowshockTimer = m_bIsRegularMode ? urand(13000, 28000) : urand(11000, 20000);
            }
        }
        else
            m_uiShadowshockTimer -= uiDiff;

        if (m_uiChainlightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                    m_uiChainlightningTimer = m_bIsRegularMode ? urand(14000, 26000) : urand(13000, 19000);
            }
        }
        else
            m_uiChainlightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_darkweaver_syth(Creature* pCreature)
{
    return new boss_darkweaver_sythAI(pCreature);
}

void AddSC_boss_darkweaver_syth()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_darkweaver_syth";
    pNewScript->GetAI = &GetAI_boss_darkweaver_syth;
    pNewScript->RegisterSelf();
}
