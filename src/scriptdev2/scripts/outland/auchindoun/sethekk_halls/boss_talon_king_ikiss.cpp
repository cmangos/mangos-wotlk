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
SDName: Boss_Talon_King_Ikiss
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "precompiled.h"
#include "sethekk_halls.h"

enum
{
    SAY_INTRO                   = -1556007,
    SAY_AGGRO_1                 = -1556008,
    SAY_AGGRO_2                 = -1556009,
    SAY_AGGRO_3                 = -1556010,
    SAY_SLAY_1                  = -1556011,
    SAY_SLAY_2                  = -1556012,
    SAY_DEATH                   = -1556013,
    EMOTE_ARCANE_EXP            = -1556015,

    SPELL_BLINK                 = 38194,
    SPELL_MANA_SHIELD           = 38151,
    SPELL_ARCANE_BUBBLE         = 9438,
    SPELL_SLOW_H                = 35032,

    SPELL_POLYMORPH             = 38245,
    SPELL_POLYMORPH_H           = 43309,

    SPELL_ARCANE_VOLLEY         = 35059,
    SPELL_ARCANE_VOLLEY_H       = 40424,

    SPELL_ARCANE_EXPLOSION      = 38197,
    SPELL_ARCANE_EXPLOSION_H    = 40425,
};

struct boss_talon_king_ikissAI : public ScriptedAI
{
    boss_talon_king_ikissAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIntro = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneVolleyTimer;
    uint32 m_uiSheepTimer;
    uint32 m_uiSlowTimer;
    uint8 m_uiBlinkPhase;
    float m_fHealthCheck;

    bool m_bManaShield;
    bool m_bBlink;
    bool m_bIntro;

    void Reset() override
    {
        m_uiArcaneVolleyTimer = urand(5000, 12000);
        m_uiSheepTimer = 8000;
        m_uiSlowTimer = urand(9000, 13000);
        m_uiBlinkPhase = 0;
        m_fHealthCheck = 80.0f;

        m_bBlink = false;
        m_bManaShield = false;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_creature->getVictim() && pWho->isTargetableForAttack() && (m_creature->IsHostileTo(pWho)) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_bIntro && m_creature->IsWithinDistInMap(pWho, 100.0f))
            {
                m_bIntro = true;
                DoScriptText(SAY_INTRO, m_creature);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO_1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO_2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IKISS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IKISS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IKISS, FAIL);
    }

    void KilledUnit(Unit* /*pVctim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (EnterEvadeIfOutOfCombatArea(uiDiff))
            return;

        if (m_bBlink)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION_H);
            DoCastSpellIfCan(m_creature, SPELL_ARCANE_BUBBLE, CAST_TRIGGERED);
            DoResetThreat();
            m_bBlink = false;
        }

        if (m_uiArcaneVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                m_uiArcaneVolleyTimer = urand(8000, 12000);
        }
        else
            m_uiArcaneVolleyTimer -= uiDiff;

        if (m_uiSheepTimer < uiDiff)
        {
            // second top aggro target in normal, random target in heroic
            if (Unit* pTarget = m_bIsRegularMode ? m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1) : m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_POLYMORPH : SPELL_POLYMORPH_H);
            m_uiSheepTimer = urand(15000, 17500);
        }
        else
            m_uiSheepTimer -= uiDiff;

        if (!m_bManaShield && m_creature->GetHealthPercent() < 15.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MANA_SHIELD) == CAST_OK)
                m_bManaShield = true;
        }

        if (!m_bIsRegularMode)
        {
            if (m_uiSlowTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SLOW_H) == CAST_OK)
                    m_uiSlowTimer = urand(15000, 24000);
            }
            else
                m_uiSlowTimer -= uiDiff;
        }

        if (m_creature->GetHealthPercent() < m_fHealthCheck)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLINK, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                m_bBlink = true;
                DoScriptText(EMOTE_ARCANE_EXP, m_creature);

                // There is no relationship between the health percentages
                switch (m_uiBlinkPhase)
                {
                    case 0: m_fHealthCheck = 50.0f; break;
                    case 1: m_fHealthCheck = 25.0f; break;
                    case 2: m_fHealthCheck = 0.0f; break;
                }

                ++m_uiBlinkPhase;
            }
        }

        if (!m_bBlink)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_talon_king_ikiss(Creature* pCreature)
{
    return new boss_talon_king_ikissAI(pCreature);
}

void AddSC_boss_talon_king_ikiss()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_talon_king_ikiss";
    pNewScript->GetAI = &GetAI_boss_talon_king_ikiss;
    pNewScript->RegisterSelf();
}
