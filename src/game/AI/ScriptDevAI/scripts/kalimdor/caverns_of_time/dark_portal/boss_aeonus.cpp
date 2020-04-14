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
SDName: Boss_Aeonus
SD%Complete: 90
SDComment: Small adjustments; Timers
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "dark_portal.h"

enum
{
    SAY_AGGRO               = -1269013,
    SAY_BANISH              = -1269014,
    SAY_SLAY1               = -1269015,
    SAY_SLAY2               = -1269016,
    SAY_DEATH               = -1269017,
    EMOTE_GENERIC_FRENZY    = -1000002,

    SPELL_THRASH            = 8876,
    SPELL_DOUBLE_ATTACK     = 19818,

    SPELL_CLEAVE            = 40504,
    SPELL_TIME_STOP         = 31422,
    SPELL_ENRAGE            = 37605,
    SPELL_SAND_BREATH       = 31473,
    SPELL_SAND_BREATH_H     = 39049
};

struct boss_aeonusAI : public ScriptedAI
{
    boss_aeonusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSandBreathTimer;
    uint32 m_uiTimeStopTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiCleaveTimer;

    void Reset() override
    {
        m_uiSandBreathTimer = urand(15000, 30000);
        m_uiTimeStopTimer   = urand(10000, 15000);
        m_uiFrenzyTimer     = urand(30000, 45000);
        m_uiCleaveTimer     = urand(5000, 9000);

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_THRASH : SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // Despawn Time Keeper
        if (pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_TIME_KEEPER)
        {
            if (m_creature->IsWithinDistInMap(pWho, 20.0f))
            {
                if (DoCastSpellIfCan(pWho, SPELL_BANISH_HELPER) == CAST_OK)
                    DoScriptText(SAY_BANISH, m_creature);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Sand Breath
        if (m_uiSandBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SAND_BREATH : SPELL_SAND_BREATH_H) == CAST_OK)
                m_uiSandBreathTimer = urand(15000, 25000);
        }
        else
            m_uiSandBreathTimer -= uiDiff;

        // Time Stop
        if (m_uiTimeStopTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TIME_STOP) == CAST_OK)
                m_uiTimeStopTimer = urand(20000, 35000);
        }
        else
            m_uiTimeStopTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(7000, 12000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Frenzy
        if (m_uiFrenzyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
            {
                DoScriptText(EMOTE_GENERIC_FRENZY, m_creature);
                m_uiFrenzyTimer = urand(20000, 35000);
            }
        }
        else
            m_uiFrenzyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_aeonus(Creature* pCreature)
{
    return new boss_aeonusAI(pCreature);
}

void AddSC_boss_aeonus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_aeonus";
    pNewScript->GetAI = &GetAI_boss_aeonus;
    pNewScript->RegisterSelf();
}
