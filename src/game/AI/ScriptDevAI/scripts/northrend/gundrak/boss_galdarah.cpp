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
SDName: Boss_Galdarah
SD%Complete: 80%
SDComment: achievements need to be implemented, channeling before engage is missing
SDCategory: Gundrak
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "gundrak.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                  = 32534,
    SAY_TRANSFORM_1            = 32535,
    SAY_TRANSFORM_2            = 32536,
    SAY_SUMMON_1               = 32537,
    SAY_SUMMON_2               = 32538,
    SAY_SUMMON_3               = 32539,
    SAY_SLAY_1                 = 32540,
    SAY_SLAY_2                 = 32541,
    SAY_SLAY_3                 = 32542,
    SAY_DEATH                  = 32543,

    EMOTE_IMPALED              = 30718,

    NPC_RHINO_SPIRIT           = 29791,
    SPELL_STAMPEDE_RHINO       = 55220,
    SPELL_STAMPEDE_RHINO_H     = 59823,
    SPELL_STAMPEDE_EFFECT      = 55219,                 // spell cast on boss; possibly used to lock a target
    SPELL_STAMPEDE_PROC        = 55221,                 // proc creature despawn

    // troll form spells
    SPELL_STAMPEDE             = 55218,
    SPELL_WHIRLING_SLASH       = 55250,
    SPELL_WHIRLING_SLASH_H     = 59824,
    SPELL_RHINO_TRANSFORM      = 55297,
    SPELL_PUNCTURE             = 55276,
    SPELL_PUNCTURE_H           = 59826,

    // rhino form spells
    SPELL_TROLL_TRANSFORM      = 55299,
    SPELL_ENRAGE               = 55285,
    SPELL_ENRAGE_H             = 59828,
    SPELL_IMPALING_CHARGE      = 54956,
    SPELL_IMPALING_CHARGE_H    = 59827,
    SPELL_STOMP                = 55292,
    SPELL_STOMP_H              = 59829,
};

/*######
## boss_galdarah
######*/

enum GaldarahActions
{
    GALDARAH_STAMPEDE,
    GALDARAH_SPECIAL,
    GALDARAH_PUNCTURE,
    GALDARAH_STOMP,
    GALDARAH_ENRAGE,
    GALDARAH_PHASE_CHECK,
    GALDARAH_PHASE_CHANGE,
    GALDARAH_ACTIONS_MAX,
};

struct boss_galdarahAI : public BossAI
{
    boss_galdarahAI(Creature* creature) : BossAI(creature, GALDARAH_ACTIONS_MAX),
    instance(dynamic_cast<instance_gundrak*>(creature->GetInstanceData())),
    isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_GALDARAH);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3);
        AddOnDeathText(SAY_DEATH);
        AddCombatAction(GALDARAH_STAMPEDE, 10s);
        AddCombatAction(GALDARAH_SPECIAL, 12s);
        AddCombatAction(GALDARAH_PUNCTURE, 25s);
        AddCombatAction(GALDARAH_PHASE_CHECK, 1s);
        AddCombatAction(GALDARAH_PHASE_CHANGE, true);
        AddCombatAction(GALDARAH_STOMP, true);
        AddCombatAction(GALDARAH_ENRAGE, true);
    }

    instance_gundrak* instance;
    bool isRegularMode;
    bool isTrollPhase;

    uint8 abilityCount;

    void Reset() override
    {
        isTrollPhase         = true;
        abilityCount         = 0;
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_RHINO_SPIRIT)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, isRegularMode ? SPELL_STAMPEDE_RHINO : SPELL_STAMPEDE_RHINO_H, SELECT_FLAG_PLAYER))
            {
                summoned->CastSpell(m_creature, SPELL_STAMPEDE_EFFECT, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_STAMPEDE_PROC, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(pTarget, isRegularMode ? SPELL_STAMPEDE_RHINO : SPELL_STAMPEDE_RHINO_H, TRIGGERED_NONE, nullptr, nullptr, m_creature->GetObjectGuid());

                // Store the player guid in order to count it for the achievement
                if (instance)
                    instance->SetData(TYPE_ACHIEV_SHARE_LOVE, pTarget->GetGUIDLow());
            }
        }
    }

    void DoPhaseSwitch()
    {
        if (!isTrollPhase)
            m_creature->RemoveAurasDueToSpell(SPELL_RHINO_TRANSFORM);

        isTrollPhase = !isTrollPhase;

        if (isTrollPhase)
        {
            DoCastSpellIfCan(nullptr, SPELL_TROLL_TRANSFORM);
            DisableCombatAction(GALDARAH_ENRAGE);
            DisableCombatAction(GALDARAH_STOMP);
            ResetCombatAction(GALDARAH_STAMPEDE, 15s);
            ResetCombatAction(GALDARAH_PUNCTURE, 25s);
        }
        else
        {
            DoBroadcastText(urand(0, 1) ? SAY_TRANSFORM_1 : SAY_TRANSFORM_2, m_creature);
            DoCastSpellIfCan(nullptr, SPELL_RHINO_TRANSFORM);
            DisableCombatAction(GALDARAH_STAMPEDE);
            DisableCombatAction(GALDARAH_PUNCTURE);
            ResetCombatAction(GALDARAH_ENRAGE, 4s);
            ResetCombatAction(GALDARAH_STOMP, 1s);
        }

        abilityCount        = 0;
        ResetCombatAction(GALDARAH_PHASE_CHECK, 1s);
        ResetCombatAction(GALDARAH_SPECIAL, 12s);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GALDARAH_PUNCTURE:
            {
                DoCastSpellIfCan(m_creature->GetVictim(), isRegularMode ? SPELL_PUNCTURE : SPELL_PUNCTURE_H);
                ResetCombatAction(action, 25s);
                return;
            }
            case GALDARAH_STAMPEDE:
            {
                switch (urand(0, 2))
                {
                    case 0: DoBroadcastText(SAY_SUMMON_1, m_creature); break;
                    case 1: DoBroadcastText(SAY_SUMMON_2, m_creature); break;
                    case 2: DoBroadcastText(SAY_SUMMON_3, m_creature); break;
                }

                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_STAMPEDE);
                ResetCombatAction(action, 15s);
                return;
            }
            case GALDARAH_ENRAGE:
            {
                DoCastSpellIfCan(nullptr, isRegularMode ? SPELL_ENRAGE : SPELL_ENRAGE_H);
                ResetCombatAction(action, 15s);
                return;
            }
            case GALDARAH_STOMP:
            {
                DoCastSpellIfCan(nullptr, isRegularMode ? SPELL_STOMP : SPELL_STOMP_H);
                ResetCombatAction(action, 10s);
                return;
            }
            case GALDARAH_SPECIAL:
            {
                if (isTrollPhase)
                {
                    if (DoCastSpellIfCan(nullptr, isRegularMode ? SPELL_WHIRLING_SLASH : SPELL_WHIRLING_SLASH_H) == CAST_OK)
                        ResetCombatAction(action, 12s);

                    ++abilityCount;
                }
                else
                {
                    Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
                    if (!pTarget)
                        pTarget = m_creature->GetVictim();

                    if (DoCastSpellIfCan(pTarget, isRegularMode ? SPELL_IMPALING_CHARGE : SPELL_IMPALING_CHARGE_H) == CAST_OK)
                    {
                        DoBroadcastText(EMOTE_IMPALED, m_creature, pTarget);
                        ResetCombatAction(action, 12s);

                        ++abilityCount;
                    }
                }
                return;
            }
            case GALDARAH_PHASE_CHECK:
            {
                if (abilityCount < 2)
                {
                    ResetCombatAction(action, 1s);
                    return;
                }
                ResetCombatAction(GALDARAH_PHASE_CHANGE, 7s);
                DisableCombatAction(action);
                return;
            }
            case GALDARAH_PHASE_CHANGE:
            {
                DoPhaseSwitch();
                DisableCombatAction(action);
                return;
            }
        }
    }
};

void AddSC_boss_galdarah()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_galdarah";
    pNewScript->GetAI = &GetNewAIInstance<boss_galdarahAI>;
    pNewScript->RegisterSelf();
}
