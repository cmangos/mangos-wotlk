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
SDName: Boss_Warlord_Kalithres
SD%Complete: 95
SDComment: Timers may need some fine adjustments
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/TimerAI.h"
#include "steam_vault.h"

enum
{
    SAY_INTRO                   = -1545016,
    SAY_REGEN                   = -1545017,
    SAY_AGGRO1                  = -1545018,
    SAY_AGGRO2                  = -1545019,
    SAY_AGGRO3                  = -1545020,
    SAY_SLAY1                   = -1545021,
    SAY_SLAY2                   = -1545022,
    SAY_DEATH                   = -1545023,

    SPELL_SPELL_REFLECTION      = 31534,
    SPELL_IMPALE                = 39061,
    SPELL_WARLORDS_RAGE         = 37081,        // triggers 36453
    SPELL_WARLORDS_RAGE_NAGA    = 31543,        // triggers 37076
    SPELL_HEAD_CRACK            = 16172,

    POINT_MOVE_DISTILLER = 1
};

enum WarlordKalithreshActions // order based on priority
{
    WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE,
    WARLORD_KALITHRESH_ACTION_REFLECTION,
    WARLORD_KALITHRESH_ACTION_IMPALE,
    WARLORD_KALITHRESH_ACTION_HEAD_CRACK,
    WARLORD_KALITHRESH_ACTION_MAX,
};

struct boss_warlord_kalithreshAI : public ScriptedAI
{
    boss_warlord_kalithreshAI(Creature* creature) : ScriptedAI(creature, WARLORD_KALITHRESH_ACTION_MAX)
    {
        m_instance = (instance_steam_vault*)creature->GetInstanceData();
        m_bHasTaunted = false;

        AddCombatAction(WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE, 0u);
        AddCombatAction(WARLORD_KALITHRESH_ACTION_REFLECTION, 0u);
        AddCombatAction(WARLORD_KALITHRESH_ACTION_IMPALE, 0u);
        AddCombatAction(WARLORD_KALITHRESH_ACTION_HEAD_CRACK, 0u);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float /*z*/)
            {
                return x < -95.7f && y > -439.6f;
            });
        Reset();
    }

    instance_steam_vault* m_instance;
    ObjectGuid m_distillerGuid;

    bool m_bHasTaunted;

    void Reset() override
    {
        for (uint32 i = 0; i < WARLORD_KALITHRESH_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE, GetInitialActionTimer(WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE));
        ResetTimer(WARLORD_KALITHRESH_ACTION_REFLECTION, GetInitialActionTimer(WARLORD_KALITHRESH_ACTION_REFLECTION));
        ResetTimer(WARLORD_KALITHRESH_ACTION_IMPALE, GetInitialActionTimer(WARLORD_KALITHRESH_ACTION_IMPALE));
        ResetTimer(WARLORD_KALITHRESH_ACTION_HEAD_CRACK, GetInitialActionTimer(WARLORD_KALITHRESH_ACTION_HEAD_CRACK));

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
    }

    uint32 GetInitialActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE: return urand(15000, 20000);
            case WARLORD_KALITHRESH_ACTION_REFLECTION: return urand(15000, 20000);
            case WARLORD_KALITHRESH_ACTION_IMPALE: return urand(7000, 14000);
            case WARLORD_KALITHRESH_ACTION_HEAD_CRACK: return urand(10000, 15000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(const uint32 action) const
    {
        switch (action)
        {
            case WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE: return urand(35000, 45000);
            case WARLORD_KALITHRESH_ACTION_REFLECTION: return urand(24000, 35000);
            case WARLORD_KALITHRESH_ACTION_IMPALE: return urand(7500, 12500);
            case WARLORD_KALITHRESH_ACTION_HEAD_CRACK: return urand(45000, 58000);
            default: return 0; // never occurs but for compiler
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, FAIL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_WARLORD_KALITHRESH, DONE);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasTaunted && m_creature->IsWithinDistInMap(pWho, 45.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        if (uiPointId == POINT_MOVE_DISTILLER)
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->GetVictim());
            DoCastSpellIfCan(m_creature, SPELL_WARLORDS_RAGE);

            // Make Distiller cast on arrival
            if (Creature* Distiller = m_creature->GetMap()->GetCreature(m_distillerGuid))
            {
                Distiller->CastSpell(Distiller, SPELL_WARLORDS_RAGE_NAGA, TRIGGERED_OLD_TRIGGERED);
                Distiller->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            }
        }
    }

    void ExecuteActions()
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < WARLORD_KALITHRESH_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case WARLORD_KALITHRESH_ACTION_WARLORDS_RAGE:
                    {
                        DoScriptText(SAY_REGEN, m_creature);
                        SetCombatScriptStatus(true);
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);
                        // Move to closest distiller
                        if (Creature* Distiller = GetClosestCreatureWithEntry(m_creature, NPC_NAGA_DISTILLER, 100.0f))
                        {
                            float fX, fY, fZ;
                            Distiller->GetContactPoint(m_creature, fX, fY, fZ, INTERACTION_DISTANCE);
                            m_creature->SetWalk(false, true); // Prevent him from slowing down while meleehit/casted upon while starting to move
                            m_creature->GetMotionMaster()->MovePoint(POINT_MOVE_DISTILLER, fX, fY, fZ);
                            m_distillerGuid = Distiller->GetObjectGuid();

                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case WARLORD_KALITHRESH_ACTION_REFLECTION:
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SPELL_REFLECTION) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        break;
                    }
                    case WARLORD_KALITHRESH_ACTION_IMPALE:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_IMPALE) == CAST_OK)
                            {
                                ResetTimer(i, GetSubsequentActionTimer(i));
                                SetActionReadyStatus(i, false);
                                return;
                            }
                        }
                        break;
                    }
                    case WARLORD_KALITHRESH_ACTION_HEAD_CRACK:
                    {
                        if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEAD_CRACK) == CAST_OK)
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

struct mob_naga_distillerAI : public Scripted_NoMovementAI
{
    mob_naga_distillerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

UnitAI* GetAI_boss_warlord_kalithresh(Creature* pCreature)
{
    return new boss_warlord_kalithreshAI(pCreature);
}

UnitAI* GetAI_mob_naga_distiller(Creature* pCreature)
{
    return new mob_naga_distillerAI(pCreature);
}

void AddSC_boss_warlord_kalithresh()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_warlord_kalithresh";
    pNewScript->GetAI = &GetAI_boss_warlord_kalithresh;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_naga_distiller";
    pNewScript->GetAI = &GetAI_mob_naga_distiller;
    pNewScript->RegisterSelf();
}
