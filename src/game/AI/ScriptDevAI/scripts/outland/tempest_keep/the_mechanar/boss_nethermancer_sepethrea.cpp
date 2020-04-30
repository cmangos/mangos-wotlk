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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 95
SDComment: May need some small adjustments
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "mechanar.h"
#include "Entities/TemporarySpawn.h"

enum
{
    SAY_AGGRO                       = -1554013,
    SAY_SUMMON                      = -1554014,
    SAY_DRAGONS_BREATH_1            = -1554015,
    SAY_DRAGONS_BREATH_2            = -1554016,
    SAY_SLAY1                       = -1554017,
    SAY_SLAY2                       = -1554018,
    SAY_DEATH                       = -1554019,

    SPELL_SUMMON_RAGING_FLAMES      = 35275,
    SPELL_SUMMON_RAGING_FLAMES_H    = 39084,
    SPELL_FROST_ATTACK              = 45196, // serverside - triggers 45195
    SPELL_ARCANE_BLAST              = 35314,
    SPELL_DRAGONS_BREATH            = 35250,

    NPC_RAGING_FLAMES               = 20481,
};

struct boss_nethermancer_sepethreaAI : public ScriptedAI
{
    boss_nethermancer_sepethreaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit* unit, float x, float y, float z)->bool
        {
            return x < 266.0f;
        });
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiDragonsBreathTimer;

    void Reset() override
    {
        m_uiArcaneBlastTimer    = urand(14000, 25000);
        m_uiDragonsBreathTimer  = urand(20000, 26000);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->SetInCombatWithZone();
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(nullptr, SPELL_FROST_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SUMMON_RAGING_FLAMES : SPELL_SUMMON_RAGING_FLAMES_H);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SEPETHREA, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Arcane Blast
        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ARCANE_BLAST) == CAST_OK)
            {
                m_uiArcaneBlastTimer = urand(15000, 30000);
                m_creature->getThreatManager().modifyThreatPercent(m_creature->GetVictim(), -50.0f);
            }
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // Dragons Breath
        if (m_uiDragonsBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_DRAGONS_BREATH) == CAST_OK)
            {
                if (urand(0, 1))
                    DoScriptText(urand(0, 1) ? SAY_DRAGONS_BREATH_1 : SAY_DRAGONS_BREATH_2, m_creature);

                m_uiDragonsBreathTimer = urand(20000, 35000);
            }
        }
        else
            m_uiDragonsBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_nethermancer_sepethrea(Creature* pCreature)
{
    return new boss_nethermancer_sepethreaAI(pCreature);
}

enum
{
    SPELL_RAGING_FLAMES = 35281,
    SPELL_INFERNO = 35268,
    SPELL_INFERNO_H = 39346,
};

struct npc_raging_flamesAI : public ScriptedAI
{
    npc_raging_flamesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if (m_creature->IsTemporarySummon())
            m_summonerGuid = m_creature->GetSpawnerGuid();

        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiInfernoTimer;

    ObjectGuid m_summonerGuid;

    void Reset() override
    {
        m_uiInfernoTimer = urand(15700, 30000);
        DoCastSpellIfCan(nullptr, SPELL_RAGING_FLAMES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        FixateRandomTarget();
    }

    void FixateRandomTarget()
    {
        DoResetThreat();

        if (Creature* pSummoner = m_creature->GetMap()->GetCreature(m_summonerGuid))
            if (Unit* pNewTarget = pSummoner->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER))
                m_creature->AddThreat(pNewTarget, 10000000.0f);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Inferno
        if (m_uiInfernoTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_INFERNO : SPELL_INFERNO_H) == CAST_OK)
            {
                m_uiInfernoTimer = urand(15700, 28900);
                FixateRandomTarget();
            }
        }
        else
            m_uiInfernoTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_raging_flames(Creature* pCreature)
{
    return new npc_raging_flamesAI(pCreature);
}

void AddSC_boss_nethermancer_sepethrea()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nethermancer_sepethrea";
    pNewScript->GetAI = &GetAI_boss_nethermancer_sepethrea;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_raging_flames";
    pNewScript->GetAI = &GetAI_npc_raging_flames;
    pNewScript->RegisterSelf();
}
