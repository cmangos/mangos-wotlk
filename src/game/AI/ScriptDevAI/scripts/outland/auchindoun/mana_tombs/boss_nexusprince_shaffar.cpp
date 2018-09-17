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
SDName: Boss_NexusPrince_Shaffar
SD%Complete: 100
SDComment:
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

/* ContentData
boss_nexusprince_shaffar
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"

enum
{
    SAY_INTRO                       = -1557000,
    SAY_AGGRO_1                     = -1557001,
    SAY_AGGRO_2                     = -1557002,
    SAY_AGGRO_3                     = -1557003,
    SAY_SLAY_1                      = -1557004,
    SAY_SLAY_2                      = -1557005,
    SAY_SUMMON                      = -1557006,
    SAY_DEAD                        = -1557007,

    SPELL_BLINK                     = 34605,
    SPELL_FROSTBOLT                 = 32364,
    SPELL_FIREBALL                  = 32363,
    SPELL_FROSTNOVA                 = 32365,

    SPELL_ETHEREAL_BEACON           = 32371,                // Summons 18431
    // SPELL_ETHEREAL_BEACON_VISUAL  = 32368,               // included in creature_template_addon

    POINT_MOVE_DISTANCE = 1,
};

enum ShaffarActions // order based on priority
{
    SHAFFAR_ACTION_BEACON,
    SHAFFAR_ACTION_FROSTNOVA,
    SHAFFAR_ACTION_BLINK,
    SHAFFAR_ACTION_PRIMARY_SPELL,
    SHAFFAR_ACTION_MAX
};

struct boss_nexusprince_shaffarAI : public ScriptedAI
{
    boss_nexusprince_shaffarAI(Creature* creature) : ScriptedAI(creature)
    {
        m_hasTaunted = false;
        m_instance = (ScriptedInstance*)creature->GetInstanceData();
        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 m_actionTimers[SHAFFAR_ACTION_MAX];

    bool m_actionReadyStatus[SHAFFAR_ACTION_MAX];
    bool m_hasTaunted;

    void Reset() override
    {
        m_actionTimers[SHAFFAR_ACTION_BEACON] = GetInitialActionTimer(SHAFFAR_ACTION_BEACON);
        m_actionTimers[SHAFFAR_ACTION_FROSTNOVA] = GetInitialActionTimer(SHAFFAR_ACTION_FROSTNOVA);
        m_actionTimers[SHAFFAR_ACTION_BLINK] = GetInitialActionTimer(SHAFFAR_ACTION_BLINK);
        m_actionReadyStatus[SHAFFAR_ACTION_PRIMARY_SPELL] = true;

        for (uint32 i = 0; i < SHAFFAR_ACTION_MAX; ++i)
            m_actionReadyStatus[i] = false;

        m_attackDistance = 20.f;

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
    }

    uint32 GetInitialActionTimer(ShaffarActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return urand(16000,20000);
                case SHAFFAR_ACTION_FROSTNOVA: return urand(16000, 25000);
                case SHAFFAR_ACTION_BLINK: return urand(45000,60000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(3000, 5000);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return 15000;
                case SHAFFAR_ACTION_FROSTNOVA: return urand(10000, 25000);
                case SHAFFAR_ACTION_BLINK: return urand(20000, 25000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(1200, 2400);
                default: return 0;
            }
        }
    }

    uint32 GetSubsequentActionTimer(ShaffarActions id)
    {
        if (m_isRegularMode)
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return urand(16000, 20000);
                case SHAFFAR_ACTION_FROSTNOVA: return urand(16000, 25000);
                case SHAFFAR_ACTION_BLINK: return urand(25000, 35000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(3000, 5000);
                default: return 0;
            }
        }
        else
        {
            switch (id)
            {
                case SHAFFAR_ACTION_BEACON: return 15000;
                case SHAFFAR_ACTION_FROSTNOVA: return urand(12000, 20000);
                case SHAFFAR_ACTION_BLINK: return urand(20000, 25000);
                case SHAFFAR_ACTION_PRIMARY_SPELL: return urand(1200, 2400);
                default: return 0;
            }
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_hasTaunted && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 100.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_hasTaunted = true;
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
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEAD, m_creature);
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < SHAFFAR_ACTION_MAX; ++i)
        {
            if (m_actionReadyStatus[i])
            {
                switch (i)
                {
                    case SHAFFAR_ACTION_BEACON:
                        if (DoCastSpellIfCan(nullptr, SPELL_ETHEREAL_BEACON) == CAST_OK)
                        {
                            if (!urand(0, 3))
                                DoScriptText(SAY_SUMMON, m_creature);

                            m_actionTimers[i] = GetSubsequentActionTimer(ShaffarActions(i));
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    case SHAFFAR_ACTION_FROSTNOVA:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_FROSTNOVA, SELECT_FLAG_PLAYER | SELECT_FLAG_USE_EFFECT_RADIUS))
                        {
                            if (DoCastSpellIfCan(nullptr, SPELL_FROSTNOVA) == CAST_OK)
                            {
                                m_actionTimers[i] = GetSubsequentActionTimer(ShaffarActions(i));
                                m_actionReadyStatus[i] = false;
                                return;
                            }
                        }
                        break;
                    case SHAFFAR_ACTION_BLINK:
                        if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()) && DoCastSpellIfCan(nullptr, SPELL_BLINK) == CAST_OK)
                        {
                            m_actionTimers[i] = GetSubsequentActionTimer(ShaffarActions(i));
                            m_actionReadyStatus[i] = false;
                            return;
                        }
                        break;
                    case SHAFFAR_ACTION_PRIMARY_SPELL:
                    {
                        uint32 spell = urand(0,1) ? SPELL_FIREBALL : SPELL_FROSTBOLT;
                        if (DoCastSpellIfCan(m_creature->getVictim(), spell) == CAST_OK)
                        {
                            m_actionTimers[i] = GetSubsequentActionTimer(ShaffarActions(i));
                            m_actionReadyStatus[i] = false;
                        }
                        return;
                    }
                    break;
                }
            }
        }
    }

    void JustStoppedMovementOfTarget(SpellEntry const* spell, Unit* victim) override
    {
        switch (spell->Id)
        {
            case SPELL_FROSTNOVA:
                if (m_creature->getVictim() != victim) // frostnova hit others, resist case
                    break;
                DistanceYourself();
                break;
        }
    }

    void DistanceYourself()
    {
        if (Unit* victim = m_creature->getVictim()) // make sure target didnt die
        {
            float x, y, z;
            SetCombatScriptStatus(true);
            SetCombatMovement(false);
            SetMeleeEnabled(false);
            m_creature->getVictim()->GetNearPoint(m_creature, x, y, z, m_creature->GetObjectBoundingRadius(), DISTANCING_CONSTANT + m_creature->GetCombinedCombatReach(victim), victim->GetAngle(m_creature));
            m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_DISTANCE, x, y, z);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_MOVE_DISTANCE)
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->getVictim());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        for (uint32 i = 0; i < SHAFFAR_ACTION_MAX; ++i)
        {
            if (!m_actionReadyStatus[i])
            {
                if (m_actionTimers[i] <= uiDiff)
                {
                    m_actionTimers[i] = 0;
                    m_actionReadyStatus[i] = true;
                }
                else
                    m_actionTimers[i] -= uiDiff;
            }
        }

        ExecuteActions();
        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_nexusprince_shaffar(Creature* pCreature)
{
    return new boss_nexusprince_shaffarAI(pCreature);
}

void AddSC_boss_nexusprince_shaffar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nexusprince_shaffar";
    pNewScript->GetAI = &GetAI_boss_nexusprince_shaffar;
    pNewScript->RegisterSelf();
}
