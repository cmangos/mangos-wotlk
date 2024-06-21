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
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "sethekk_halls.h"

enum
{
    SAY_INTRO                   = 17769,
    SAY_AGGRO_1                 = 17765,
    SAY_AGGRO_2                 = 17767,
    SAY_AGGRO_3                 = 17768,
    SAY_SLAY_1                  = 17763,
    SAY_SLAY_2                  = 17764,
    SAY_DEATH                   = 17762,
    EMOTE_ARCANE_EXP            = 19738,

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
    TALON_KING_IKISS_MANA_SHIELD,
    TALON_KING_IKISS_BLINK_START,
    TALON_KING_IKISS_ACTION_SLOW,
    TALON_KING_IKISS_ACTION_POLYMORPH,
    TALON_KING_IKISS_ACTION_ARCANE_VOLLEY,
    TALON_KING_IKISS_ACTION_MAX,
    TALON_KING_IKISS_ARCANE_EXPLOSION_EMOTE,
    TALON_KING_IKISS_ARCANE_EXPLOSION_CAST,
    TALON_KING_IKISS_ARCANE_EXPLOSION_ATTACK_START,
};

struct boss_talon_king_ikissAI : public CombatAI
{
    boss_talon_king_ikissAI(Creature* creature) : CombatAI(creature, TALON_KING_IKISS_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()),
            m_Intro(false), m_uiBlinkPhase(0), m_HealthPercent(80.f)
    {
        AddTimerlessCombatAction(TALON_KING_IKISS_MANA_SHIELD, true);
        AddTimerlessCombatAction(TALON_KING_IKISS_BLINK_START, true);
        AddCombatAction(TALON_KING_IKISS_ACTION_SLOW, 9000, 13000);
        AddCombatAction(TALON_KING_IKISS_ACTION_POLYMORPH, 6000, 10000);
        AddCombatAction(TALON_KING_IKISS_ACTION_ARCANE_VOLLEY, 5000, 12000);
        AddCustomAction(TALON_KING_IKISS_ARCANE_EXPLOSION_EMOTE, true, [&]() { HandleExplosionEmote(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(TALON_KING_IKISS_ARCANE_EXPLOSION_CAST, true, [&]() { HandleArcaneExplosionCast(); }, TIMER_COMBAT_COMBAT);
        AddCustomAction(TALON_KING_IKISS_ARCANE_EXPLOSION_ATTACK_START, true, [&]() { HandleArcaneExplosionAttackStart(); }, TIMER_COMBAT_COMBAT);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    bool m_Intro;
    uint8 m_uiBlinkPhase;
    float m_HealthPercent;

    void Reset() override
    {
        CombatAI::Reset();

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);

        m_uiBlinkPhase = 0;
        m_HealthPercent = 80.0f;
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
                DoBroadcastText(SAY_INTRO, m_creature);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoBroadcastText(SAY_AGGRO_1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO_2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO_3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_IKISS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_IKISS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_IKISS, FAIL);
    }

    void HandleArcaneExplosionCast()
    {
        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION_H);
        DoCastSpellIfCan(nullptr, SPELL_ARCANE_BUBBLE, CAST_TRIGGERED);
    }

    void HandleExplosionEmote()
    {
        DoBroadcastText(EMOTE_ARCANE_EXP, m_creature);
    }

    void HandleArcaneExplosionAttackStart()
    {
        DoResetThreat();
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TALON_KING_IKISS_MANA_SHIELD:
                if (m_creature->GetHealthPercent() < 15.0f)
                    if (DoCastSpellIfCan(nullptr, SPELL_MANA_SHIELD) == CAST_OK)
                        DisableCombatAction(action);
                break;
            case TALON_KING_IKISS_BLINK_START:
                if (m_creature->GetHealthPercent() < m_HealthPercent)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BLINK) == CAST_OK)
                    {
                        SetMeleeEnabled(false);
                        SetCombatMovement(false);
                        SetCombatScriptStatus(true);
                        ResetTimer(TALON_KING_IKISS_ARCANE_EXPLOSION_EMOTE, 1500);
                        ResetTimer(TALON_KING_IKISS_ARCANE_EXPLOSION_CAST, 2500);
                        ResetTimer(TALON_KING_IKISS_ARCANE_EXPLOSION_ATTACK_START, 8500);
                        

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
                break;
            case TALON_KING_IKISS_ACTION_SLOW:
            {
                if (!m_isRegularMode)
                    if (DoCastSpellIfCan(nullptr, SPELL_SLOW_H) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case TALON_KING_IKISS_ACTION_POLYMORPH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA | SELECT_FLAG_SKIP_TANK))
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_POLYMORPH : SPELL_POLYMORPH_H) == CAST_OK)
                        ResetCombatAction(action, GetSubsequentActionTimer(action));
                break;
            }
            case TALON_KING_IKISS_ACTION_ARCANE_VOLLEY:
            {
                if (DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(action));
            }
            break;
        }
    }
};

void AddSC_boss_talon_king_ikiss()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_talon_king_ikiss";
    pNewScript->GetAI = &GetNewAIInstance<boss_talon_king_ikissAI>;
    pNewScript->RegisterSelf();
}
