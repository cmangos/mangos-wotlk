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
SDName: Boss_Kazzak
SD%Complete: 90
SDComment: Some timers need to be confirmed.
SDCategory: Bosses
EndScriptData
*/

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_INTRO                 = -1000147,
    SAY_AGGRO1                = -1000148,
    SAY_AGGRO2                = -1000149,
    SAY_SURPREME1             = -1000150,
    SAY_SURPREME2             = -1000151,
    SAY_KILL1                 = -1000152,
    SAY_KILL2                 = -1000153,
    SAY_KILL3                 = -1000154,
    SAY_DEATH                 = -1000155,
    EMOTE_FRENZY              = -1000002,
    SAY_RAND1                 = -1000157,
    SAY_RAND2                 = -1000158,

    SPELL_SHADOW_VOLLEY       = 21341,
    SPELL_BERSERK             = 21340,
    SPELL_CLEAVE              = 20691,
    SPELL_THUNDERCLAP         = 26554,
    SPELL_VOIDBOLT            = 21066,
    SPELL_MARK_OF_KAZZAK      = 21056,                  // triggers 21058 when target gets to 0 mana
    SPELL_CAPTURESOUL         = 21053,                  // procs 21054 on kill
    SPELL_TWISTED_REFLECTION   = 21063
};

enum KazzakActions // order based on priority
{
    KAZZAK_ACTION_SUPREME,
    KAZZAK_ACTION_VOIDBOLT,
    KAZZAK_ACTION_TWISTED_REFLECTION,
    KAZZAK_ACTION_MARK_OF_KAZZAK,
    KAZZAK_ACTION_SHADOW_BOLT_VOLLEY,
    KAZZAK_ACTION_THUNDERCLAP,
    KAZZAK_ACTION_CLEAVE,

    KAZZAK_ACTION_MAX,
};


struct boss_kazzakAI : public ScriptedAI
{
    boss_kazzakAI(Creature* creature) : ScriptedAI(creature, KAZZAK_ACTION_MAX)
    {
        AddCombatAction(KAZZAK_ACTION_SUPREME, 0u);
        AddCombatAction(KAZZAK_ACTION_VOIDBOLT, 0u);
        AddCombatAction(KAZZAK_ACTION_TWISTED_REFLECTION, 0u);
        AddCombatAction(KAZZAK_ACTION_MARK_OF_KAZZAK, 0u);
        AddCombatAction(KAZZAK_ACTION_SHADOW_BOLT_VOLLEY, 0u);
        AddCombatAction(KAZZAK_ACTION_THUNDERCLAP, 0u);
        AddCombatAction(KAZZAK_ACTION_CLEAVE, 0u);
        Reset();
    }


    void Reset() override
    {
        for (uint32 i = 0; i < KAZZAK_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(KAZZAK_ACTION_SUPREME, GetInitialActionTimer(KAZZAK_ACTION_SUPREME));
        ResetTimer(KAZZAK_ACTION_VOIDBOLT, GetInitialActionTimer(KAZZAK_ACTION_VOIDBOLT));
        ResetTimer(KAZZAK_ACTION_TWISTED_REFLECTION, GetInitialActionTimer(KAZZAK_ACTION_TWISTED_REFLECTION));
        ResetTimer(KAZZAK_ACTION_MARK_OF_KAZZAK, GetInitialActionTimer(KAZZAK_ACTION_MARK_OF_KAZZAK));
        ResetTimer(KAZZAK_ACTION_SHADOW_BOLT_VOLLEY, GetInitialActionTimer(KAZZAK_ACTION_SHADOW_BOLT_VOLLEY));
        ResetTimer(KAZZAK_ACTION_THUNDERCLAP, GetInitialActionTimer(KAZZAK_ACTION_THUNDERCLAP));
        ResetTimer(KAZZAK_ACTION_CLEAVE, GetInitialActionTimer(KAZZAK_ACTION_CLEAVE));

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case KAZZAK_ACTION_SUPREME: return 3 * MINUTE * IN_MILLISECONDS;
            case KAZZAK_ACTION_VOIDBOLT: return 30000;
            case KAZZAK_ACTION_TWISTED_REFLECTION: return 33000;
            case KAZZAK_ACTION_MARK_OF_KAZZAK: return 25000;
            case KAZZAK_ACTION_SHADOW_BOLT_VOLLEY: return urand(3000, 12000);
            case KAZZAK_ACTION_THUNDERCLAP: return urand(16000, 20000);
            case KAZZAK_ACTION_CLEAVE: return 7000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case KAZZAK_ACTION_SUPREME: return 0;
            case KAZZAK_ACTION_VOIDBOLT: return urand(15000, 28000);
            case KAZZAK_ACTION_TWISTED_REFLECTION: return 15000;
            case KAZZAK_ACTION_MARK_OF_KAZZAK: return  20000;
            case KAZZAK_ACTION_SHADOW_BOLT_VOLLEY: return urand(5000, 30000);
            case KAZZAK_ACTION_THUNDERCLAP: return urand(10000, 14000);
            case KAZZAK_ACTION_CLEAVE: return urand(8000, 12000);
            default: return 0; // never occurs but for compiler
        }
    }

    void JustRespawned() override
    {
        DoScriptText(SAY_INTRO, m_creature);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_CAPTURESOUL, CAST_TRIGGERED);
        DoScriptText(urand(0, 1) ? SAY_AGGRO1 : SAY_AGGRO2, m_creature);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 3))
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

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < KAZZAK_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case KAZZAK_ACTION_SUPREME:
                    {
                        // Enrage - cast shadowbolt volley every second
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                        {
                            DoScriptText(urand(0, 1) ? SAY_SURPREME1 : SAY_SURPREME2, m_creature);
                            DisableCombatAction(KAZZAK_ACTION_SUPREME);
                            return;
                        }
                        break;
                    }
                    case KAZZAK_ACTION_VOIDBOLT:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_VOIDBOLT) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case KAZZAK_ACTION_TWISTED_REFLECTION:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_TWISTED_REFLECTION, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_TWISTED_REFLECTION) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        break;
                    }
                    case KAZZAK_ACTION_MARK_OF_KAZZAK:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MARK_OF_KAZZAK, SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_KAZZAK) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                            break;
                        }
                    }
                    case KAZZAK_ACTION_SHADOW_BOLT_VOLLEY:
                    {
                        // Cast shadowbolt volley on timer before Berserk
                        if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_VOLLEY, SELECT_FLAG_USE_EFFECT_RADIUS) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case KAZZAK_ACTION_THUNDERCLAP:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_THUNDERCLAP, SELECT_FLAG_USE_EFFECT_RADIUS) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case KAZZAK_ACTION_CLEAVE:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();
        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_kazzak(Creature* pCreature)
{
    return new boss_kazzakAI(pCreature);
}

void AddSC_boss_kazzakAI()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kazzak";
    pNewScript->GetAI = &GetAI_boss_kazzak;
    pNewScript->RegisterSelf();
}
