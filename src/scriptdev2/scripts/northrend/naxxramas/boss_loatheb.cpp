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
SDName: Boss_Loatheb
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    EMOTE_AURA_BLOCKING     = -1533143,
    EMOTE_AURA_WANE         = -1533144,
    EMOTE_AURA_FADING       = -1533145,

    SPELL_DEATHBLOOM        = 29865,
    SPELL_DEATHBLOOM_H      = 55053,
    SPELL_INEVITABLE_DOOM   = 29204,
    SPELL_INEVITABLE_DOOM_H = 55052,
    SPELL_NECROTIC_AURA     = 55593,
    SPELL_SUMMON_SPORE      = 29234,
    SPELL_BERSERK           = 26662,

    NPC_SPORE               = 16286
};

struct boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_naxxramas* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDeathbloomTimer;
    uint32 m_uiNecroticAuraTimer;
    uint32 m_uiInevitableDoomTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiBerserkTimer;
    uint8  m_uiNecroticAuraCount;                           // Used for emotes, 5min check

    void Reset() override
    {
        m_uiDeathbloomTimer = 5000;
        m_uiNecroticAuraTimer = 12000;
        m_uiInevitableDoomTimer = MINUTE * 2 * IN_MILLISECONDS;
        m_uiSummonTimer = urand(10000, 15000);              // first seen in vid after approx 12s
        m_uiBerserkTimer = MINUTE * 12 * IN_MILLISECONDS;   // only in heroic, after 12min
        m_uiNecroticAuraCount = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOATHEB, NOT_STARTED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() != NPC_SPORE)
            return;

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AddThreat(pTarget);
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SPORE && m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SPORE_LOSER, false);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserk (only heroic)
        if (!m_bIsRegularMode)
        {
            if (m_uiBerserkTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                m_uiBerserkTimer = 300000;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        // Inevitable Doom
        if (m_uiInevitableDoomTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_INEVITABLE_DOOM : SPELL_INEVITABLE_DOOM_H);
            m_uiInevitableDoomTimer = (m_uiNecroticAuraCount <= 40) ? 30000 : 15000;
        }
        else
            m_uiInevitableDoomTimer -= uiDiff;

        // Necrotic Aura
        if (m_uiNecroticAuraTimer < uiDiff)
        {
            switch (m_uiNecroticAuraCount % 3)
            {
                case 0:
                    DoCastSpellIfCan(m_creature, SPELL_NECROTIC_AURA);
                    DoScriptText(EMOTE_AURA_BLOCKING, m_creature);
                    m_uiNecroticAuraTimer = 14000;
                    break;
                case 1:
                    DoScriptText(EMOTE_AURA_WANE, m_creature);
                    m_uiNecroticAuraTimer = 3000;
                    break;
                case 2:
                    DoScriptText(EMOTE_AURA_FADING, m_creature);
                    m_uiNecroticAuraTimer = 3000;
                    break;
            }
            ++m_uiNecroticAuraCount;
        }
        else
            m_uiNecroticAuraTimer -= uiDiff;

        // Summon
        if (m_uiSummonTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPORE);
            m_uiSummonTimer = m_bIsRegularMode ? 36000 : 18000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        // Deathbloom
        if (m_uiDeathbloomTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_DEATHBLOOM : SPELL_DEATHBLOOM_H);
            m_uiDeathbloomTimer = 30000;
        }
        else
            m_uiDeathbloomTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_loatheb(Creature* pCreature)
{
    return new boss_loathebAI(pCreature);
}

void AddSC_boss_loatheb()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_loatheb";
    pNewScript->GetAI = &GetAI_boss_loatheb;
    pNewScript->RegisterSelf();
}
