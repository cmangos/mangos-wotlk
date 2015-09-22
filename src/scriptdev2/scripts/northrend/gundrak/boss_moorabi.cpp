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
SDName: Boss_Moorabi
SD%Complete: 20%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    SAY_AGGRO                   = -1604011,
    SAY_QUAKE                   = -1604012,
    SAY_TRANSFORM               = -1604013,
    SAY_SLAY_1                  = -1604014,
    SAY_SLAY_2                  = -1604015,
    SAY_SLAY_3                  = -1604016,
    SAY_DEATH                   = -1604017,
    EMOTE_TRANSFORM             = -1604018,
    EMOTE_TRANSFORMED           = -1604029,

    // Troll form
    SPELL_DETERMINED_STAB   = 55104,
    SPELL_MOJO_FRENZY       = 55163,
    SPELL_GROUND_TREMOR     = 55142,
    SPELL_NUMBING_SHOUT     = 55106,
    SPELL_TRANSFORMATION    = 55098,

    // Mammoth
    SPELL_DETERMINED_GORE   = 55102,
    SPELL_DETERMINED_GORE_H = 59444,
    SPELL_QUAKE             = 55101,
    SPELL_NUMBING_ROAR      = 55100,
};

/*######
## boss_moorabi
######*/

struct boss_moorabiAI : public ScriptedAI
{
    boss_moorabiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStabTimer;                                   // used for stab and gore
    uint32 m_uiQuakeTimer;                                  // used for quake and ground tremor
    uint32 m_uiRoarTimer;                                   // both roars on it
    uint32 m_uiTransformationTimer;
    uint32 m_uiPreviousTimer;

    bool m_bMammothPhase;

    void Reset() override
    {
        m_bMammothPhase = false;

        m_uiStabTimer           = 8000;
        m_uiQuakeTimer          = 1000;
        m_uiRoarTimer           = 7000;
        m_uiTransformationTimer = 10000;
        m_uiPreviousTimer       = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_MOJO_FRENZY);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOORABI, IN_PROGRESS);
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
            m_pInstance->SetData(TYPE_MOORABI, DONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasAura(SPELL_TRANSFORMATION) && !m_bMammothPhase)
        {
            DoScriptText(EMOTE_TRANSFORMED, m_creature);
            m_bMammothPhase = true;

            // Set the achievement to failed
            if (m_pInstance)
                m_pInstance->SetLessRabiAchievementCriteria(false);
        }

        if (m_uiRoarTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bMammothPhase ? SPELL_NUMBING_ROAR : SPELL_NUMBING_SHOUT);
            m_uiRoarTimer = 20000;
        }
        else
            m_uiRoarTimer -= uiDiff;

        if (m_uiQuakeTimer < uiDiff)
        {
            DoScriptText(SAY_QUAKE, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), m_bMammothPhase ? SPELL_QUAKE : SPELL_GROUND_TREMOR);
            m_uiQuakeTimer = m_bMammothPhase ? 13000 : 18000;
        }
        else
            m_uiQuakeTimer -= uiDiff;

        if (m_uiStabTimer < uiDiff)
        {
            if (m_bMammothPhase)
                DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_DETERMINED_GORE : SPELL_DETERMINED_GORE_H);
            else
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DETERMINED_STAB);

            m_uiStabTimer = 7000;
        }
        else
            m_uiStabTimer -= uiDiff;

        // check only in troll phase
        if (!m_bMammothPhase)
        {
            if (m_uiTransformationTimer < uiDiff)
            {
                DoScriptText(SAY_TRANSFORM, m_creature);
                DoScriptText(EMOTE_TRANSFORM, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_TRANSFORMATION);
                m_uiPreviousTimer *= 0.8;
                m_uiTransformationTimer = m_uiPreviousTimer;
            }
            else
                m_uiTransformationTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_moorabi(Creature* pCreature)
{
    return new boss_moorabiAI(pCreature);
}

void AddSC_boss_moorabi()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_moorabi";
    pNewScript->GetAI = &GetAI_boss_moorabi;
    pNewScript->RegisterSelf();
}
