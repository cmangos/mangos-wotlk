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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
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

enum TalonKingIkissActions // order based on priority
{
    TALON_KING_IKISS_ACTION_SLOW,
    TALON_KING_IKISS_ACTION_POLYMORPH,
    TALON_KING_IKISS_ACTION_ARCANE_VOLLEY,
    TALON_KING_IKISS_ACTION_MAX,
    TALON_KING_IKISS_BLINK
};

struct boss_talon_king_ikissAI : public ScriptedAI
{
    boss_talon_king_ikissAI(Creature* creature) : ScriptedAI(creature, TALON_KING_IKISS_ACTION_MAX)
    {
        m_instance = (ScriptedInstance*)creature->GetInstanceData();
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        m_Intro = false;

        AddCombatAction(TALON_KING_IKISS_ACTION_SLOW, 0u);
        AddCombatAction(TALON_KING_IKISS_ACTION_POLYMORPH, 0u);
        AddCombatAction(TALON_KING_IKISS_ACTION_ARCANE_VOLLEY, 0u);
        AddCustomAction(TALON_KING_IKISS_BLINK, true, [&]() { HandleBlink(); });
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    bool m_ManaShield;
    bool m_Intro;
    uint8 m_uiBlinkPhase;
    float m_HealthPercent;

    void Reset() override
    {
        for (uint32 i = 0; i < TALON_KING_IKISS_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(TALON_KING_IKISS_ACTION_SLOW, GetInitialActionTimer(TALON_KING_IKISS_ACTION_SLOW));
        ResetTimer(TALON_KING_IKISS_ACTION_POLYMORPH, GetInitialActionTimer(TALON_KING_IKISS_ACTION_POLYMORPH));
        ResetTimer(TALON_KING_IKISS_ACTION_ARCANE_VOLLEY, GetInitialActionTimer(TALON_KING_IKISS_ACTION_ARCANE_VOLLEY));

        SetCombatMovement(true);
        SetCombatScriptStatus(false);

        m_uiBlinkPhase = 0;
        m_HealthPercent = 80.0f;

        m_ManaShield = false;
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case TALON_KING_IKISS_ACTION_SLOW: return urand(9000, 13000);
            case TALON_KING_IKISS_ACTION_POLYMORPH: return urand(6000, 10000);
            case TALON_KING_IKISS_ACTION_ARCANE_VOLLEY: return urand(5000, 12000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case TALON_KING_IKISS_ACTION_SLOW: return urand(15000, 20000);
            case TALON_KING_IKISS_ACTION_POLYMORPH: return urand(10000, 15000);
            case TALON_KING_IKISS_ACTION_ARCANE_VOLLEY: return urand(8000, 12000);
            default: return 0; // never occurs but for compiler
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_creature->GetVictim() && m_creature->CanAttackOnSight(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_Intro && m_creature->IsWithinDistInMap(pWho, 100.0f))
            {
                m_Intro = true;
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

        if (m_instance)
            m_instance->SetData(TYPE_IKISS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_IKISS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_IKISS, FAIL);
    }

    void KilledUnit(Unit* /*pVctim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void HandleBlink()
    {
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION_H);
        DoCastSpellIfCan(m_creature, SPELL_ARCANE_BUBBLE, CAST_TRIGGERED);
        DoResetThreat();
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < TALON_KING_IKISS_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case TALON_KING_IKISS_ACTION_SLOW:
                    {
                        if (!m_isRegularMode)
                            if (DoCastSpellIfCan(m_creature, SPELL_SLOW_H) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        break;
                    }
                    case TALON_KING_IKISS_ACTION_POLYMORPH:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER| SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK))
                        {
                            DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_POLYMORPH : SPELL_POLYMORPH_H);
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case TALON_KING_IKISS_ACTION_ARCANE_VOLLEY:
                    {
                        if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                    }
                    break;
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_ManaShield && m_creature->GetHealthPercent() < 15.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_MANA_SHIELD) == CAST_OK)
                m_ManaShield = true;
        }

        if (m_creature->GetHealthPercent() < m_HealthPercent)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLINK, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
            {
                SetMeleeEnabled(false);
                ResetTimer(TALON_KING_IKISS_BLINK, 1000);
                DoScriptText(EMOTE_ARCANE_EXP, m_creature);

                // There is no relationship between the health percentages
                switch (m_uiBlinkPhase)
                {
                    case 0: m_HealthPercent = 50.0f; break;
                    case 1: m_HealthPercent = 25.0f; break;
                    case 2: m_HealthPercent = 0.0f; break;
                }

                ++m_uiBlinkPhase;
            }
        }

        if (m_creature->HasAura(SPELL_ARCANE_BUBBLE))
            return;

        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_talon_king_ikiss(Creature* pCreature)
{
    return new boss_talon_king_ikissAI(pCreature);
}

void AddSC_boss_talon_king_ikiss()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_talon_king_ikiss";
    pNewScript->GetAI = &GetAI_boss_talon_king_ikiss;
    pNewScript->RegisterSelf();
}
