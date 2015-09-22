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
SDName: boss_eregos
SD%Complete: 90
SDComment: Small adjustments may be required.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

enum
{
    SAY_AGGRO                       = -1578011,
    SAY_ARCANE_SHIELD               = -1578012,
    SAY_FIRE_SHIELD                 = -1578013,
    SAY_NATURE_SHIELD               = -1578014,
    SAY_FRENZY                      = -1578015,
    SAY_KILL_1                      = -1578016,
    SAY_KILL_2                      = -1578017,
    SAY_KILL_3                      = -1578018,
    SAY_DEATH                       = -1578019,
    EMOTE_ASTRAL_PLANE              = -1578024,

    SPELL_ARCANE_BARRAGE            = 50804,
    SPELL_ARCANE_BARRAGE_H          = 59381,
    SPELL_ARCANE_VOLLEY             = 51153,
    SPELL_ARCANE_VOLLEY_H           = 59382,
    SPELL_ENRAGED_ASSAULT           = 51170,
    SPELL_PLANAR_ANOMALIES          = 57959,
    SPELL_SUMMON_LEY_WHELP          = 51175,
    SPELL_PLANAR_SHIFT              = 51162,

    SPELL_PLANAR_ANOMALY_AGGRO      = 57971,
    SPELL_PLANAR_BLAST              = 57976,

    NPC_PLANAR_ANOMALY              = 30879,
    NPC_GREATER_LEY_WHELP           = 28276,
};

/*######
## boss_eregos
######*/

struct boss_eregosAI : public ScriptedAI
{
    boss_eregosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneBarrageTimer;
    uint32 m_uiArcaneVolleyTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiSummonWhelpsTimer;
    float m_fHpPercent;

    uint8 m_uiAnomalyTargetIndex;
    GuidVector m_vAnomalyTargets;

    void Reset() override
    {
        m_uiArcaneBarrageTimer  = 0;
        m_uiArcaneVolleyTimer   = 20000;
        m_uiEnrageTimer         = 35000;
        m_uiSummonWhelpsTimer   = urand(15000, 20000);
        m_fHpPercent            = 60.0f;
        m_uiAnomalyTargetIndex  = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_creature->Attack(pWho, false))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, FAIL);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_PLANAR_ANOMALY)
        {
            pSummoned->CastSpell(pSummoned, SPELL_PLANAR_ANOMALY_AGGRO, true);

            // If this happens then something is really wrong
            if (m_vAnomalyTargets.empty())
                return;

            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_vAnomalyTargets[m_uiAnomalyTargetIndex]))
                pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0, 0);

            if (m_uiAnomalyTargetIndex < m_vAnomalyTargets.size() - 1)
                ++m_uiAnomalyTargetIndex;
        }
        else if (pSummoned->GetEntry() == NPC_GREATER_LEY_WHELP)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasAura(SPELL_PLANAR_SHIFT))
            return;

        if (m_creature->GetHealthPercent() < m_fHpPercent)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PLANAR_SHIFT) == CAST_OK)
            {
                // Get all the vehicle entries which are in combat with the boss
                m_vAnomalyTargets.clear();
                m_uiAnomalyTargetIndex = 0;

                ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
                for (ThreatList::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                {
                    if (Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
                    {
                        if (pTarget->GetEntry() == NPC_RUBY_DRAKE || pTarget->GetEntry() == NPC_AMBER_DRAKE || pTarget->GetEntry() == NPC_EMERALD_DRAKE)
                            m_vAnomalyTargets.push_back(pTarget->GetObjectGuid());
                    }
                }

                // This will summon an anomaly for each player (vehicle)
                DoCastSpellIfCan(m_creature, SPELL_PLANAR_ANOMALIES, CAST_TRIGGERED);

                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_ARCANE_SHIELD, m_creature); break;
                    case 1: DoScriptText(SAY_FIRE_SHIELD, m_creature); break;
                    case 2: DoScriptText(SAY_NATURE_SHIELD, m_creature); break;
                }
                DoScriptText(EMOTE_ASTRAL_PLANE, m_creature);

                // set next phase to 20%
                m_fHpPercent -= 40;
            }
        }

        if (m_uiArcaneBarrageTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE : SPELL_ARCANE_BARRAGE_H) == CAST_OK)
                    m_uiArcaneBarrageTimer = urand(2000, 3000);
            }
        }
        else
            m_uiArcaneBarrageTimer -= uiDiff;

        if (m_uiSummonWhelpsTimer < uiDiff)
        {
            // ToDo: the number of whelps summoned may be different based on difficulty. Needs research!
            for (uint8 i = 0; i < 4; ++i)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_LEY_WHELP, CAST_TRIGGERED) == CAST_OK)
                    m_uiSummonWhelpsTimer = 20000;
            }
        }
        else
            m_uiSummonWhelpsTimer -= uiDiff;

        if (m_uiArcaneVolleyTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H) == CAST_OK)
                m_uiArcaneVolleyTimer = urand(10000, 15000);
        }
        else
            m_uiArcaneVolleyTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGED_ASSAULT) == CAST_OK)
            {
                DoScriptText(SAY_FRENZY, m_creature);
                m_uiEnrageTimer = urand(40000, 50000);
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
    return new boss_eregosAI(pCreature);
}

/*######
## npc_planar_anomaly
######*/

struct npc_planar_anomalyAI : public ScriptedAI
{
    npc_planar_anomalyAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiPlanarBlastTimer;
    bool m_bHasBlastCasted;

    void Reset() override
    {
        m_uiPlanarBlastTimer = 15000;
        m_bHasBlastCasted = false;
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bHasBlastCasted)
            return;

        // Check for the players mounted on the vehicles
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_PLANAR_BLAST) == CAST_OK)
                {
                    m_bHasBlastCasted = true;
                    m_creature->ForcedDespawn(1000);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bHasBlastCasted)
            return;

        if (m_uiPlanarBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PLANAR_BLAST) == CAST_OK)
            {
                m_bHasBlastCasted = true;
                m_creature->ForcedDespawn(1000);
            }
        }
        else
            m_uiPlanarBlastTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_planar_anomaly(Creature* pCreature)
{
    return new npc_planar_anomalyAI(pCreature);
}

void AddSC_boss_eregos()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_eregos";
    pNewScript->GetAI = &GetAI_boss_eregos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_planar_anomaly";
    pNewScript->GetAI = &GetAI_npc_planar_anomaly;
    pNewScript->RegisterSelf();
}
