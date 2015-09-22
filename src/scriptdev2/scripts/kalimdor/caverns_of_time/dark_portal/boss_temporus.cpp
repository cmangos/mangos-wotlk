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
SDName: Boss_Temporus
SD%Complete: 90
SDComment: Small adjustments; Timers
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "precompiled.h"
#include "dark_portal.h"

enum
{
    SAY_AGGRO               = -1269001,
    SAY_BANISH              = -1269002,
    SAY_SLAY1               = -1269003,
    SAY_SLAY2               = -1269004,
    SAY_DEATH               = -1269005,

    SPELL_HASTE             = 31458,
    SPELL_MORTAL_WOUND      = 31464,
    SPELL_WING_BUFFET       = 31475,
    SPELL_WING_BUFFET_H     = 38593,
    SPELL_REFLECT           = 38592
};

struct boss_temporusAI : public ScriptedAI
{
    boss_temporusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiHasteTimer;
    uint32 m_uiSpellReflectionTimer;
    uint32 m_uiMortalWoundTimer;
    uint32 m_uiWingBuffetTimer;

    void Reset() override
    {
        m_uiHasteTimer           = urand(15000, 23000);
        m_uiSpellReflectionTimer = 30000;
        m_uiMortalWoundTimer     = 8000;
        m_uiWingBuffetTimer      = urand(25000, 35000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
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

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Attack Haste
        if (m_uiHasteTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_HASTE) == CAST_OK)
                m_uiHasteTimer = urand(20000, 25000);
        }
        else
            m_uiHasteTimer -= uiDiff;

        // MortalWound_Timer
        if (m_uiMortalWoundTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_WOUND) == CAST_OK)
                m_uiMortalWoundTimer = urand(10000, 20000);
        }
        else
            m_uiMortalWoundTimer -= uiDiff;

        // Wing ruffet
        if (m_uiWingBuffetTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WING_BUFFET : SPELL_WING_BUFFET_H) == CAST_OK)
                m_uiWingBuffetTimer = urand(20000, 30000);
        }
        else
            m_uiWingBuffetTimer -= uiDiff;

        // Spell reflection
        if (!m_bIsRegularMode)
        {
            if (m_uiSpellReflectionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_REFLECT) == CAST_OK)
                    m_uiSpellReflectionTimer = urand(25000, 35000);
            }
            else
                m_uiSpellReflectionTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_temporus(Creature* pCreature)
{
    return new boss_temporusAI(pCreature);
}

void AddSC_boss_temporus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_temporus";
    pNewScript->GetAI = &GetAI_boss_temporus;
    pNewScript->RegisterSelf();
}
