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
SDName: Boss_Vexallus
SD%Complete: 95
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "magisters_terrace.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/TimerAI.h"

enum
{
    SAY_AGGRO                       = -1585007,
    SAY_ENERGY                      = -1585008,
    SAY_OVERLOAD                    = -1585009,
    SAY_KILL                        = -1585010,
    EMOTE_DISCHARGE_ENERGY          = -1585011,
    EMOTE_OVERLOAD                  = -1585031,

    // Pure energy spell info
    SPELL_ENERGY_BOLT_PERIODIC      = 46156,
    SPELL_ENERGY_FEEDBACK_CHANNELED = 44328, // Channel
    SPELL_ENERGY_FEEDBACK_DEBUFF    = 44335, // The actual debuff
    //SPELL_ENERGY_FEEDBACK_VISUAL    = 44339, // Visual

    SPELL_ENERGY_PASSIVE            = 44326,
    SPELL_ENERGY_BOLT               = 44342,

    // Vexallus spell info
    SPELL_CHAIN_LIGHTNING           = 44318,
    SPELL_CHAIN_LIGHTNING_H         = 46380,                // heroic spell
    SPELL_OVERLOAD                  = 44353,
    SPELL_ARCANE_SHOCK              = 44319,
    SPELL_ARCANE_SHOCK_H            = 46381,                // heroic spell

    SPELL_SUMMON_PURE_ENERGY        = 44322,                // mod scale -10
    SPELL_SUMMON_PURE_ENERGY1_H     = 46154,                // mod scale -5
    SPELL_SUMMON_PURE_ENERGY2_H     = 46159,                // mod scale -5

    SPELL_CLEAR_ENERGY_FEEDBACK     = 47108,

    // Creatures
    NPC_PURE_ENERGY                 = 24745,
};

enum VexallusActions
{
    VEXALLUS_ACTION_CHAIN_LIGHTNING,
    VEXALLUS_ACTION_SHOCK,
    VEXALLUS_ACTION_OVERLOAD,
    VEXALLUS_ACTION_MAX,
};

struct boss_vexallusAI : public ScriptedAI, public CombatActions
{
    boss_vexallusAI(Creature* pCreature) : ScriptedAI(pCreature), CombatActions(VEXALLUS_ACTION_MAX)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        AddCombatAction(VEXALLUS_ACTION_CHAIN_LIGHTNING, 0u);
        AddCombatAction(VEXALLUS_ACTION_SHOCK, 0u);
        AddCombatAction(VEXALLUS_ACTION_OVERLOAD, 0u);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    //bool m_bEnraged; Is there an enrage mechanic or not?
    bool m_bOverloading;

    uint32 m_uiChainLightningTimer;
    uint32 m_uiArcaneShockTimer;
    uint32 m_uiOverloadTimer;

    float m_uiIntervalHealthAmount;

    void Reset() override
    {
        SetCombatMovement(true);

        for (uint32 i = 0; i < VEXALLUS_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(VEXALLUS_ACTION_CHAIN_LIGHTNING, GetInitialActionTimer(VEXALLUS_ACTION_CHAIN_LIGHTNING));
        ResetTimer(VEXALLUS_ACTION_SHOCK, GetInitialActionTimer(VEXALLUS_ACTION_SHOCK));
        ResetTimer(VEXALLUS_ACTION_OVERLOAD, GetInitialActionTimer(VEXALLUS_ACTION_OVERLOAD));

        DisableCombatAction(VEXALLUS_ACTION_OVERLOAD);

        m_uiIntervalHealthAmount = 85;
        m_bOverloading           = false;
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case VEXALLUS_ACTION_CHAIN_LIGHTNING: return urand(8000, 16000);
            case VEXALLUS_ACTION_SHOCK: return urand(25000, 30000);
            case VEXALLUS_ACTION_OVERLOAD: return 0;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case VEXALLUS_ACTION_CHAIN_LIGHTNING: return urand(14000, 24000);
            case VEXALLUS_ACTION_SHOCK: return urand(12000, 16000);
            case VEXALLUS_ACTION_OVERLOAD: return 2000;
            default: return 0; // never occurs but for compiler
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < VEXALLUS_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case VEXALLUS_ACTION_CHAIN_LIGHTNING:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CHAIN_LIGHTNING, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case VEXALLUS_ACTION_SHOCK:
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ARCANE_SHOCK, SELECT_FLAG_IN_LOS | SELECT_FLAG_PLAYER))
                            DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H);

                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                    case VEXALLUS_ACTION_OVERLOAD:
                    {
                        DoCastSpellIfCan(m_creature, SPELL_OVERLOAD);
                        ResetTimer(i, GetSubsequentActionTimer(i));
                        SetActionReadyStatus(i, false);
                        continue;
                    }
                }
            }
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEXALLUS, FAIL);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->CastSpell(m_creature, SPELL_CLEAR_ENERGY_FEEDBACK, TRIGGERED_NONE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEXALLUS, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEXALLUS, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0.0f);

        pSummoned->CastSpell(pSummoned, SPELL_ENERGY_BOLT_PERIODIC, TRIGGERED_OLD_TRIGGERED/*, nullptr, nullptr, m_creature->GetObjectGuid()*/);
        pSummoned->CastSpell(pSummoned, SPELL_ENERGY_PASSIVE, TRIGGERED_OLD_TRIGGERED/*, nullptr, nullptr, m_creature->GetObjectGuid()*/);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        UpdateTimers(uiDiff, m_creature->isInCombat());
        ExecuteActions();

        if (!m_bOverloading)
        {
            if (m_creature->GetHealthPercent() > 20.0f) // overload at 20%
            {
                // used for check, when Vexallus cast adds 85%, 70%, 55%, 40%, 25%
                if (m_creature->GetHealthPercent() <= m_uiIntervalHealthAmount)
                {
                    DoScriptText(SAY_ENERGY, m_creature);
                    DoScriptText(EMOTE_DISCHARGE_ENERGY, m_creature);
                    m_uiIntervalHealthAmount -= 15.0f;

                    if (m_bIsRegularMode)
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_PURE_ENERGY);
                    else
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_PURE_ENERGY1_H, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_PURE_ENERGY2_H, CAST_TRIGGERED);
                    }
                }

                if (m_uiChainLightningTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                            m_uiChainLightningTimer = 8000;
                    }
                }
                else
                    m_uiChainLightningTimer -= uiDiff;

                if (m_uiArcaneShockTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H) == CAST_OK)
                            m_uiArcaneShockTimer = 8000;
                    }
                }
                else
                    m_uiArcaneShockTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }
            else // overload at 20%
            {
                DisableCombatAction(VEXALLUS_ACTION_CHAIN_LIGHTNING);
                DisableCombatAction(VEXALLUS_ACTION_SHOCK);
                ResetTimer(VEXALLUS_ACTION_OVERLOAD, GetSubsequentActionTimer(VEXALLUS_ACTION_OVERLOAD));

                DoScriptText(SAY_OVERLOAD, m_creature);
                DoScriptText(EMOTE_OVERLOAD, m_creature);

                SetCombatMovement(false);
                m_bOverloading = true;
            }
        }
    }
};

UnitAI* GetAI_boss_vexallus(Creature* pCreature)
{
    return new boss_vexallusAI(pCreature);
};

struct mob_pure_energyAI : public ScriptedAI
{
    mob_pure_energyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() override { }

    void JustDied(Unit* pKiller) override
    {
        m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_BOLT_PERIODIC);
        m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_PASSIVE);

        if (m_creature->IsTemporarySummon())
        {
            if (m_creature->GetSpawnerGuid().IsCreature())
            {
                Creature* pVex = m_creature->GetMap()->GetCreature(m_creature->GetSpawnerGuid());

                if (!pVex || !pVex->isAlive())
                    return;

                if (Player* pPlayer = pKiller->GetBeneficiaryPlayer())
                {
                    m_creature->CastSpell(pPlayer, SPELL_ENERGY_FEEDBACK_CHANNELED, TRIGGERED_NONE);
                    pPlayer->CastSpell(pPlayer, SPELL_ENERGY_FEEDBACK_DEBUFF, TRIGGERED_NONE);
                }
            }
        }
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}
};

UnitAI* GetAI_mob_pure_energy(Creature* pCreature)
{
    return new mob_pure_energyAI(pCreature);
};

void AddSC_boss_vexallus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_vexallus";
    pNewScript->GetAI = &GetAI_boss_vexallus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_pure_energy";
    pNewScript->GetAI = &GetAI_mob_pure_energy;
    pNewScript->RegisterSelf();
}
