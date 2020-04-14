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
SDName: Boss_Doomlord_Kazzak
SD%Complete: 90
SDComment: Timers
SDCategory: Hellfire Peninsula
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    SAY_INTRO                       = -1000147,
    SAY_AGGRO1                      = -1000148,
    SAY_AGGRO2                      = -1000149,
    SAY_SURPREME1                   = -1000150,
    SAY_SURPREME2                   = -1000151,
    SAY_KILL1                       = -1000152,
    SAY_KILL2                       = -1000153,
    SAY_KILL3                       = -1000154,
    SAY_DEATH                       = -1000155,
    EMOTE_GENERIC_FRENZY            = -1000002,
    SAY_RAND1                       = -1000157,
    SAY_RAND2                       = -1000158,

    SPELL_SHADOW_VOLLEY             = 32963,
    SPELL_CLEAVE                    = 16044,
    SPELL_THUNDERCLAP               = 36706,
    SPELL_VOID_BOLT                 = 21066,
    SPELL_MARK_OF_KAZZAK            = 32960,
    SPELL_FRENZY                    = 32964,        // triggers 32963
    SPELL_CAPTURE_SOUL              = 48473,        // procs 32966 on player kill
    SPELL_TWISTED_REFLECTION        = 21063,
    SPELL_BERSERK                   = 32965,        // triggers 32963
};

struct boss_doomlordkazzakAI : public ScriptedAI
{
    boss_doomlordkazzakAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiShadowVolleyTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiThunderClapTimer;
    uint32 m_uiVoidBoltTimer;
    uint32 m_uiMarkOfKazzakTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiGreatEnrageTimer;
    uint32 m_uiTwistedReflectionTimer;

    void Reset() override
    {
        m_uiShadowVolleyTimer       = urand(6000, 10000);
        m_uiCleaveTimer             = 7000;
        m_uiThunderClapTimer        = urand(14000, 18000);
        m_uiVoidBoltTimer           = 30000;
        m_uiMarkOfKazzakTimer       = 25000;
        m_uiEnrageTimer             = 60000;
        m_uiGreatEnrageTimer        = 3 * MINUTE * IN_MILLISECONDS;
        m_uiTwistedReflectionTimer  = 33000;                // Timer may be incorrect
    }

    void JustRespawned() override
    {
        DoScriptText(SAY_INTRO, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_AGGRO1 : SAY_AGGRO2, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_CAPTURE_SOUL, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* pVictim) override
    {
        // When Kazzak kills a player (not pets/totems), he regens some health
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL1, m_creature); break;
            case 1: DoScriptText(SAY_KILL2, m_creature); break;
            case 2: DoScriptText(SAY_KILL3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Cleave_Timer
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(8000, 12000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // ThunderClap_Timer
        if (m_uiThunderClapTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderClapTimer = urand(10000, 14000);
        }
        else
            m_uiThunderClapTimer -= uiDiff;

        // VoidBolt_Timer
        if (m_uiVoidBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_VOID_BOLT) == CAST_OK)
                m_uiVoidBoltTimer = urand(15000, 18000);
        }
        else
            m_uiVoidBoltTimer -= uiDiff;

        // MarkOfKazzak_Timer
        if (m_uiMarkOfKazzakTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MARK_OF_KAZZAK, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_KAZZAK) == CAST_OK)
                    m_uiMarkOfKazzakTimer = 20000;
            }
        }
        else
            m_uiMarkOfKazzakTimer -= uiDiff;

        // Enrage_Timer
        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(EMOTE_GENERIC_FRENZY, m_creature);
                m_uiEnrageTimer = 60000;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        // Great_Enrage_Timer
        if (m_uiGreatEnrageTimer)
        {
            if (m_uiGreatEnrageTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                    m_uiGreatEnrageTimer = 0;
            }
            else
                m_uiGreatEnrageTimer -= uiDiff;
        }

        // Twisted Reflection
        if (m_uiTwistedReflectionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_TWISTED_REFLECTION, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_TWISTED_REFLECTION) == CAST_OK)
                    m_uiTwistedReflectionTimer = 15000;
            }
        }
        else
            m_uiTwistedReflectionTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_doomlordkazzak(Creature* pCreature)
{
    return new boss_doomlordkazzakAI(pCreature);
}

void AddSC_boss_doomlordkazzak()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_doomlord_kazzak";
    pNewScript->GetAI = &GetAI_boss_doomlordkazzak;
    pNewScript->RegisterSelf();
}
