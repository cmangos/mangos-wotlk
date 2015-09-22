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
SDName: boss_ichoron
SD%Complete: 50
SDComment: Water Globule event NYI
SDCategory: Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SAY_AGGRO           = -1608019,
    SAY_SHATTERING      = -1608020,
    SAY_SHIELD          = -1608021,
    SAY_SLAY_1          = -1608022,
    SAY_SLAY_2          = -1608023,
    SAY_SLAY_3          = -1608024,
    SAY_ENRAGE          = -1608025,
    SAY_DEATH           = -1608026,
    EMOTE_BUBBLE        = -1608028,

    SPELL_SPLASH                = 59516,
    SPELL_DRAINED               = 59820,
    SPELL_FRENZY                = 54312,
    SPELL_FRENZY_H              = 59522,
    SPELL_PROTECTIVE_BUBBLE     = 54306,
    SPELL_WATER_BLAST           = 54237,
    SPELL_WATER_BLAST_H         = 59520,
    SPELL_WATER_BOLT_VOLLEY     = 54241,
    SPELL_WATER_BOLT_VOLLEY_H   = 59521,
    SPELL_WATER_GLOBULE         = 54260,

    SPELL_WATER_GLOBULE_SPAWN_1 = 54258,
    SPELL_WATER_GLOBULE_SPAWN_2 = 54264,
    SPELL_WATER_GLOBULE_SPAWN_3 = 54265,
    SPELL_WATER_GLOBULE_SPAWN_4 = 54266,
    SPELL_WATER_GLOBULE_SPAWN_5 = 54267,

    SPELL_MERGE                 = 54269,                // used by globules
    SPELL_WATER_GLOBULE_TRANS   = 54268,
};

static const uint32 aWaterGlobuleSpells[5] = {SPELL_WATER_GLOBULE_SPAWN_1, SPELL_WATER_GLOBULE_SPAWN_2, SPELL_WATER_GLOBULE_SPAWN_3, SPELL_WATER_GLOBULE_SPAWN_4, SPELL_WATER_GLOBULE_SPAWN_5};

struct boss_ichoronAI : public ScriptedAI
{
    boss_ichoronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_violet_hold*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        Reset();
    }

    instance_violet_hold* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWaterBoltVolleyTimer;
    uint32 m_uiWaterBlastTimer;
    bool m_bIsFrenzy;

    void Reset() override
    {
        m_uiWaterBoltVolleyTimer = urand(10000, 12000);
        m_uiWaterBlastTimer      = 10000;
        m_bIsFrenzy              = false;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_PROTECTIVE_BUBBLE);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pWho) override
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWaterBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_WATER_BLAST : SPELL_WATER_BLAST_H) == CAST_OK)
                    m_uiWaterBlastTimer = urand(8000, 14000);
            }
        }
        else
            m_uiWaterBlastTimer -= uiDiff;

        if (m_uiWaterBoltVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WATER_BOLT_VOLLEY : SPELL_WATER_BOLT_VOLLEY_H) == CAST_OK)
                m_uiWaterBoltVolleyTimer = urand(7000, 12000);
        }
        else
            m_uiWaterBoltVolleyTimer -= uiDiff;

        if (!m_bIsFrenzy && m_creature->GetHealthPercent() < 25.0f)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H) == CAST_OK)
            {
                DoScriptText(SAY_ENRAGE, m_creature);
                m_bIsFrenzy = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ichoron(Creature* pCreature)
{
    return new boss_ichoronAI(pCreature);
}

void AddSC_boss_ichoron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ichoron";
    pNewScript->GetAI = &GetAI_boss_ichoron;
    pNewScript->RegisterSelf();
}
