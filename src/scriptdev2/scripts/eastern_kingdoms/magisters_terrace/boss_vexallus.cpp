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
SD%Complete: 90
SDComment: Timers.
SDCategory: Magister's Terrace
EndScriptData */

#include "precompiled.h"
#include "magisters_terrace.h"
#include "TemporarySummon.h"

enum
{
    SAY_AGGRO                       = -1585007,
    SAY_ENERGY                      = -1585008,
    SAY_OVERLOAD                    = -1585009,
    SAY_KILL                        = -1585010,
    EMOTE_DISCHARGE_ENERGY          = -1585011,

    // is this text for real?
    //#define SAY_DEATH             "What...happen...ed."

    // Pure energy spell info
    SPELL_ENERGY_BOLT               = 46156,
    SPELL_ENERGY_FEEDBACK           = 44335,
    SPELL_ENERGY_PASSIVE            = 44326,

    // Vexallus spell info
    SPELL_CHAIN_LIGHTNING           = 44318,
    SPELL_CHAIN_LIGHTNING_H         = 46380,                // heroic spell
    SPELL_OVERLOAD                  = 44353,
    SPELL_ARCANE_SHOCK              = 44319,
    SPELL_ARCANE_SHOCK_H            = 46381,                // heroic spell

    SPELL_SUMMON_PURE_ENERGY        = 44322,                // mod scale -10
    SPELL_SUMMON_PURE_ENERGY1_H     = 46154,                // mod scale -5
    SPELL_SUMMON_PURE_ENERGY2_H     = 46159,                // mod scale -5

    // Creatures
    NPC_PURE_ENERGY                 = 24745,
};

struct boss_vexallusAI : public ScriptedAI
{
    boss_vexallusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiChainLightningTimer;
    uint32 m_uiArcaneShockTimer;
    uint32 m_uiOverloadTimer;
    uint32 m_uiIntervalHealthAmount;
    bool m_bEnraged;

    void Reset() override
    {
        m_uiChainLightningTimer  = 8000;
        m_uiArcaneShockTimer     = 5000;
        m_uiOverloadTimer        = 1200;
        m_uiIntervalHealthAmount = 1;
        m_bEnraged               = false;
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

        pSummoned->CastSpell(pSummoned, SPELL_ENERGY_PASSIVE, true, NULL, NULL, m_creature->GetObjectGuid());
        pSummoned->CastSpell(pSummoned, SPELL_ENERGY_BOLT, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bEnraged)
        {
            // Enrage at 20% hp
            if (m_creature->GetHealthPercent() < 20.0f)
            {
                m_bEnraged = true;
                return;
            }

            // used for check, when Vexallus cast adds 85%, 70%, 55%, 40%, 25%
            if (m_creature->GetHealthPercent() <= float(100.0f - 15.0f * m_uiIntervalHealthAmount))
            {
                DoScriptText(SAY_ENERGY, m_creature);
                DoScriptText(EMOTE_DISCHARGE_ENERGY, m_creature);
                ++m_uiIntervalHealthAmount;

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
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                        m_uiChainLightningTimer = 8000;
                }
            }
            else
                m_uiChainLightningTimer -= uiDiff;

            if (m_uiArcaneShockTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H) == CAST_OK)
                        m_uiArcaneShockTimer = 8000;
                }
            }
            else
                m_uiArcaneShockTimer -= uiDiff;
        }
        else
        {
            if (m_uiOverloadTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_OVERLOAD) == CAST_OK)
                    m_uiOverloadTimer = 2000;
            }
            else
                m_uiOverloadTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vexallus(Creature* pCreature)
{
    return new boss_vexallusAI(pCreature);
};

struct mob_pure_energyAI : public ScriptedAI
{
    mob_pure_energyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() override { }

    void JustDied(Unit* pKiller) override
    {
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

            if (pTemporary->GetSummonerGuid().IsCreature())
            {
                Creature* pVex = m_creature->GetMap()->GetCreature(pTemporary->GetSummonerGuid());

                if (!pVex || !pVex->isAlive())
                    return;

                if (Player* pPlayer = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself())
                    pPlayer->CastSpell(pPlayer, SPELL_ENERGY_FEEDBACK, true, NULL, NULL, pVex->GetObjectGuid());
            }
        }
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {}
    void AttackStart(Unit* /*pWho*/) override {}
};

CreatureAI* GetAI_mob_pure_energy(Creature* pCreature)
{
    return new mob_pure_energyAI(pCreature);
};

void AddSC_boss_vexallus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_vexallus";
    pNewScript->GetAI = &GetAI_boss_vexallus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_pure_energy";
    pNewScript->GetAI = &GetAI_mob_pure_energy;
    pNewScript->RegisterSelf();
}
