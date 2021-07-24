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
SDName: Boss_Pandemonius
SD%Complete: 80
SDComment: Not known how void blast is done (amount of rapid cast seems to be related to players in party).
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    SAY_AGGRO_1                     = -1557008,
    SAY_AGGRO_2                     = -1557009,
    SAY_AGGRO_3                     = -1557010,
    SAY_KILL_1                      = -1557011,
    SAY_KILL_2                      = -1557012,
    SAY_DEATH                       = -1557013,
    EMOTE_DARK_SHELL                = -1557014,

    SPELL_VOID_BLAST                = 32325,
    SPELL_VOID_BLAST_H              = 38760,
    SPELL_DARK_SHELL                = 32358,
    SPELL_DARK_SHELL_H              = 38759,

    MAX_VOID_BLASTS                 = 5,
};

struct boss_pandemoniusAI : public ScriptedAI
{
    boss_pandemoniusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiVoidBlastTimer;
    uint32 m_uiDarkShellTimer;
    uint8 m_uiVoidBlastCounter;

    void Reset() override
    {
        m_uiVoidBlastTimer   = urand(15000, 20000);
        m_uiDarkShellTimer   = urand(13000, 15000);
        m_uiVoidBlastCounter = 0;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
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

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiVoidBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, m_bIsRegularMode ? SPELL_VOID_BLAST : SPELL_VOID_BLAST_H))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_VOID_BLAST : SPELL_VOID_BLAST_H);

            // reset timer and counter when counter has reached the max limit
            if (m_uiVoidBlastCounter == MAX_VOID_BLASTS)
            {
                m_uiVoidBlastTimer = urand(25000, 30000);
                m_uiVoidBlastCounter = 0;
            }
            // cast the void blasts in a row until we reach the max limit
            else
            {
                m_uiVoidBlastTimer = 500;
                ++m_uiVoidBlastCounter;
            }
        }
        else
            m_uiVoidBlastTimer -= uiDiff;

        // use the darkshell only when the boss isn't casting the void blasts
        if (!m_uiVoidBlastCounter)
        {
            if (m_uiDarkShellTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_DARK_SHELL : SPELL_DARK_SHELL_H) == CAST_OK)
                {
                    DoScriptText(EMOTE_DARK_SHELL, m_creature);
                    m_uiDarkShellTimer = urand(18000, 30000);
                }
            }
            else
                m_uiDarkShellTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_pandemonius(Creature* pCreature)
{
    return new boss_pandemoniusAI(pCreature);
}

void AddSC_boss_pandemonius()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_pandemonius";
    pNewScript->GetAI = &GetAI_boss_pandemonius;
    pNewScript->RegisterSelf();
}
