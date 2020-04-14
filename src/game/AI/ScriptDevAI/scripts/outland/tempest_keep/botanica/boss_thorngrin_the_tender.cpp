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
SDName: Boss_Thorngrin_The_Tender
SD%Complete: 99.9
SDComment:
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    SAY_DEATH       = -1553013,
    SAY_PLAYER_KILL = -1553014,
    SAY_SACRIFICE   = -1553015,
    SAY_20_HP       = -1553016,
    SAY_50_HP       = -1553017,
    SAY_HELLFIRE_1  = -1553018,
    SAY_HELLFIRE_2  = -1553019,
    SAY_AGGRO       = -1553020,

    SPELL_HELLFIRE      = 34659,
    SPELL_HELLFIRE_H    = 39131,
    SPELL_SACRIFICE     = 34661,
    SPELL_ENRAGE        = 34670,
};

struct boss_thorngrinAI : ScriptedAI
{
    boss_thorngrinAI(Creature* creature) : ScriptedAI(creature)
    {
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_isRegularMode;
    uint32 m_sacrificeTimer;
    uint32 m_hellfireTimer;
    uint32 m_enrageTimer;
    bool m_below50;
    bool m_below20;

    void Reset() override
    {
        m_hellfireTimer = urand(4500, 12500);
        m_sacrificeTimer = urand(7000, 12000);
        m_enrageTimer = urand(15000, 30000);
        m_below50 = m_below20 = false;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_PLAYER_KILL, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Hellfire
        if (m_hellfireTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_HELLFIRE : SPELL_HELLFIRE_H) == CAST_OK)
                m_hellfireTimer = urand(16000, 24000);
        }
        else
            m_hellfireTimer -= diff;

        // Sacrifice
        if (m_sacrificeTimer < diff)
        {
            m_sacrificeTimer = 0;
            if (!m_creature->IsNonMeleeSpellCasted(false))
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SACRIFICE, SELECT_FLAG_PLAYER | SELECT_FLAG_IN_LOS))
                    if (DoCastSpellIfCan(target, SPELL_SACRIFICE) == CAST_OK)
                        m_sacrificeTimer = urand(22000, 34000);
        }
        else
            m_sacrificeTimer -= diff;

        // Enrage
        if (m_enrageTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                m_enrageTimer = urand(22000, 40000);
        }
        else
            m_enrageTimer -= diff;

        if (!m_below50 && m_creature->GetHealthPercent() <= 50.f)
        {
            DoScriptText(SAY_50_HP, m_creature);
            m_below50 = true;
        }

        if (!m_below20 && m_creature->GetHealthPercent() <= 20.f)
        {
            DoScriptText(SAY_20_HP, m_creature);
            m_below20 = true;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_thorngrin(Creature* pCreature)
{
    return new boss_thorngrinAI(pCreature);
}

void AddSC_boss_thorngrin()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_thorngrin";
    pNewScript->GetAI = &GetAI_boss_thorngrin;
    pNewScript->RegisterSelf();
}