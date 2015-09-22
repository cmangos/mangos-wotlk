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
SDName: Boss_Murmur
SD%Complete: 75
SDComment: Sonic Boom and Murmur's Touch require additional research and core support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "precompiled.h"
#include "shadow_labyrinth.h"

enum
{
    EMOTE_SONIC_BOOM            = -1555036,

    // Intro spells - used on npcs
    SPELL_SUPPRESSION_BLAST     = 33332,
    SPELL_MURMURS_WRATH         = 33331,
    SPELL_MURMURS_WRATH_2       = 33329,

    SPELL_MAGNETIC_PULL         = 33689,
    SPELL_SONIC_BOOM            = 33923,        // dummy spell - triggers 33666
    SPELL_SONIC_BOOM_H          = 38796,        // dummy spell - triggers 38795
    SPELL_MURMURS_TOUCH         = 33711,        // on expire silences the party members using shockwave - 33686 - also related to spell 33760
    SPELL_MURMURS_TOUCH_H       = 38794,
    SPELL_RESONANCE             = 33657,

    SPELL_SONIC_SHOCK           = 38797,        // Heroic Spell
    SPELL_THUNDERING_STORM      = 39365,        // Heroic Spell
};

struct boss_murmurAI : public Scripted_NoMovementAI
{
    boss_murmurAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSonicBoomTimer;
    uint32 m_uiMurmursTouchTimer;
    uint32 m_uiResonanceTimer;
    uint32 m_uiMagneticPullTimer;
    uint32 m_uiSonicShockTimer;
    uint32 m_uiThunderingStormTimer;

    void Reset() override
    {
        m_uiSonicBoomTimer          = urand(21000, 35000);
        m_uiMurmursTouchTimer       = urand(9000, 18000);
        m_uiResonanceTimer          = urand(1000, 7000);
        m_uiMagneticPullTimer       = urand(15000, 25000);
        m_uiSonicShockTimer         = urand(5000, 15000);
        m_uiThunderingStormTimer    = urand(10000, 50000);

        // Boss has only 0.4 of max health
        m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.4));
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // SonicBoom_Timer
        if (m_uiSonicBoomTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SONIC_BOOM : SPELL_SONIC_BOOM_H) == CAST_OK)
            {
                DoScriptText(EMOTE_SONIC_BOOM, m_creature);
                m_uiSonicBoomTimer = urand(31000, 38000);
            }
        }
        else
            m_uiSonicBoomTimer -= uiDiff;

        // MurmursTouch_Timer
        if (m_uiMurmursTouchTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MURMURS_TOUCH : SPELL_MURMURS_TOUCH_H) == CAST_OK)
                m_uiMurmursTouchTimer = m_bIsRegularMode ? urand(21000, 21000) : urand(29000, 40000);
        }
        else
            m_uiMurmursTouchTimer -= uiDiff;

        // Resonance_Timer - cast if no target is in range
        if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
        {
            if (m_uiResonanceTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_RESONANCE) == CAST_OK)
                    m_uiResonanceTimer = urand(5000, 12000);
            }
            else
                m_uiResonanceTimer -= uiDiff;
        }

        // MagneticPull_Timer
        if (m_uiMagneticPullTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MAGNETIC_PULL, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MAGNETIC_PULL) == CAST_OK)
                    m_uiMagneticPullTimer = urand(21000, 30000);
            }
        }
        else
            m_uiMagneticPullTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiSonicShockTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SONIC_SHOCK, SELECT_FLAG_IN_MELEE_RANGE))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SONIC_SHOCK) == CAST_OK)
                        m_uiSonicShockTimer = urand(3000, 10000);
                }
            }
            else
                m_uiSonicShockTimer -= uiDiff;

            if (m_uiThunderingStormTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STORM) == CAST_OK)
                    m_uiThunderingStormTimer = urand(5000, 6000);
            }
            else
                m_uiThunderingStormTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_murmur(Creature* pCreature)
{
    return new boss_murmurAI(pCreature);
}

void AddSC_boss_murmur()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_murmur";
    pNewScript->GetAI = &GetAI_boss_murmur;
    pNewScript->RegisterSelf();
}
