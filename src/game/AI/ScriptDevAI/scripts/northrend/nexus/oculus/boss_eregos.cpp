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

#include "AI/ScriptDevAI/include/sc_common.h"
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
        m_pInstance = static_cast<instance_oculus*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        // ToDo: check if different for heroic
        m_uiMaxWhelps = 4;
        Reset();
    }

    instance_oculus* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneBarrageTimer;
    uint32 m_uiArcaneVolleyTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiSummonWhelpsTimer;
    uint8 m_uiMaxWhelps;
    float m_fHpPercent;

    void Reset() override
    {
        m_uiArcaneBarrageTimer  = 0;
        m_uiArcaneVolleyTimer   = 20000;
        m_uiEnrageTimer         = 35000;
        m_uiSummonWhelpsTimer   = urand(15000, 20000);
        m_fHpPercent            = 60.0f;

        m_attackDistance = 20.0f;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, IN_PROGRESS);
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
        if (pSummoned->GetEntry() == NPC_GREATER_LEY_WHELP)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_creature->HasAura(SPELL_PLANAR_SHIFT))
            return;

        if (m_creature->GetHealthPercent() < m_fHpPercent)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PLANAR_SHIFT) == CAST_OK)
            {
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
            for (uint8 i = 0; i < m_uiMaxWhelps; ++i)
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_LEY_WHELP, CAST_TRIGGERED);

            m_uiSummonWhelpsTimer = 20000;
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

/*######
## npc_planar_anomaly
######*/

struct npc_planar_anomalyAI : public ScriptedAI
{
    npc_planar_anomalyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);
        Reset();
    }

    uint32 m_uiPlanarBlastTimer;
    bool m_bHasBlastCasted;

    ObjectGuid m_spawnerGuid;

    void Reset() override
    {
        // fix visual - hack
        m_creature->SetDisplayId(11686);

        // visual spell
        DoCastSpellIfCan(m_creature, SPELL_PLANAR_ANOMALY_AGGRO);

        m_uiPlanarBlastTimer = 15000;
        m_bHasBlastCasted = false;

        // start chasing the summoner (player / vehicle)
        if (Unit* pSpawner = m_creature->GetSpawner())
        {
            m_creature->GetMotionMaster()->MoveChase(pSpawner);
            m_spawnerGuid = pSpawner->GetObjectGuid();
        }
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bHasBlastCasted)
            return;

        // Check if the spawner Guid is in range
        if (pWho->GetObjectGuid() == m_spawnerGuid && m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PLANAR_BLAST) == CAST_OK)
            {
                m_bHasBlastCasted = true;
                m_creature->ForcedDespawn(1000);
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

void AddSC_boss_eregos()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_eregos";
    pNewScript->GetAI = &GetNewAIInstance<boss_eregosAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_planar_anomaly";
    pNewScript->GetAI = &GetNewAIInstance<npc_planar_anomalyAI>;
    pNewScript->RegisterSelf();
}
