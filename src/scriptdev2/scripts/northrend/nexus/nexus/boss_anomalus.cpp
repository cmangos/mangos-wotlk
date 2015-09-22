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
SDName: Boss_Anomalus
SD%Complete: 90%
SDComment: Small adjustments required
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                          = -1576006,
    SAY_RIFT                           = -1576007,
    SAY_SHIELD                         = -1576008,
    SAY_KILL                           = -1576009,
    SAY_DEATH                          = -1576010,
    EMOTE_OPEN_RIFT                    = -1576021,
    EMOTE_SHIELD                       = -1576022,

    // Anomalus
    SPELL_CREATE_RIFT                  = 47743,
    SPELL_CHARGE_RIFT                  = 47747,
    SPELL_RIFT_SHIELD                  = 47748,

    SPELL_SPARK                        = 47751,
    SPELL_SPARK_H                      = 57062,

    // Chaotic Rift
    SPELL_RIFT_AURA                    = 47687,
    SPELL_RIFT_SUMMON_AURA             = 47732,

    // Charged Chaotic Rift
    SPELL_CHARGED_RIFT_AURA            = 47733,
    SPELL_CHARGED_RIFT_SUMMON_AURA     = 47742,

    NPC_CHAOTIC_RIFT                   = 26918,
    NPC_CRAZED_MANA_WRAITH             = 26746
};

/*######
## boss_anomalus
######*/

struct boss_anomalusAI : public ScriptedAI
{
    boss_anomalusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_nexus*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_nexus* m_pInstance;
    bool m_bIsRegularMode;

    bool   m_bChaoticRift;
    uint32 m_uiSparkTimer;
    uint32 m_uiCreateRiftTimer;
    uint8 m_uiChaoticRiftCount;

    void Reset() override
    {
        m_bChaoticRift = false;
        m_uiSparkTimer = 5000;
        m_uiCreateRiftTimer = 25000;
        m_uiChaoticRiftCount = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANOMALUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANOMALUS, DONE);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHAOTIC_RIFT)
        {
            ++m_uiChaoticRiftCount;

            DoScriptText(SAY_RIFT, m_creature);

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHAOTIC_RIFT)
        {
            --m_uiChaoticRiftCount;

            // If players kill the Chaotic Rifts then mark the achievement as false
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_CHAOS_THEORY, false);

            if (!m_uiChaoticRiftCount)
            {
                if (m_creature->HasAura(SPELL_RIFT_SHIELD))
                    m_creature->RemoveAurasDueToSpell(SPELL_RIFT_SHIELD);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_creature->HasAura(SPELL_RIFT_SHIELD))
            return;

        // Create additional Chaotic Rift at 50% HP
        if (!m_bChaoticRift && m_creature->GetHealthPercent() < 50.0f)
        {
            // create a rift then set shield up and finally charge rift
            if (DoCastSpellIfCan(m_creature, SPELL_CREATE_RIFT, CAST_TRIGGERED) == CAST_OK)
            {
                // emotes are in this order
                DoScriptText(EMOTE_SHIELD, m_creature);
                DoScriptText(SAY_SHIELD, m_creature);
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);

                DoCastSpellIfCan(m_creature, SPELL_RIFT_SHIELD, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_CHARGE_RIFT, CAST_TRIGGERED);
                m_bChaoticRift = true;
            }
            return;
        }

        if (m_uiCreateRiftTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CREATE_RIFT) == CAST_OK)
            {
                DoScriptText(SAY_RIFT, m_creature);
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                m_uiCreateRiftTimer = 25000;
            }
        }
        else
            m_uiCreateRiftTimer -= uiDiff;

        if (m_uiSparkTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SPARK : SPELL_SPARK_H);

            m_uiSparkTimer = 5000;
        }
        else
            m_uiSparkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anomalus(Creature* pCreature)
{
    return new boss_anomalusAI(pCreature);
}

struct mob_chaotic_riftAI : public Scripted_NoMovementAI
{
    mob_chaotic_riftAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiChargedRemoveTimer;

    void Reset() override
    {
        m_uiChargedRemoveTimer = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        // Auras are applied on aggro because there are many npcs with this entry in the instance
        DoCastSpellIfCan(m_creature, SPELL_RIFT_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RIFT_SUMMON_AURA, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CRAZED_MANA_WRAITH)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // When hit with Charge Rift cast the Charged Rift spells
        if (pSpell->Id == SPELL_CHARGE_RIFT)
        {
            DoCastSpellIfCan(m_creature, SPELL_CHARGED_RIFT_AURA, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_CHARGED_RIFT_SUMMON_AURA, CAST_TRIGGERED);
            m_uiChargedRemoveTimer = 45000;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChargedRemoveTimer)
        {
            // The charged spells need to be removed by casting the normal ones in case the npc isn't killed
            if (m_uiChargedRemoveTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_RIFT_AURA, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_RIFT_SUMMON_AURA, CAST_TRIGGERED);
                m_uiChargedRemoveTimer = 0;
            }
            else
                m_uiChargedRemoveTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_chaotic_rift(Creature* pCreature)
{
    return new mob_chaotic_riftAI(pCreature);
}

void AddSC_boss_anomalus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_anomalus";
    pNewScript->GetAI = &GetAI_boss_anomalus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_chaotic_rift";
    pNewScript->GetAI = &GetAI_mob_chaotic_rift;
    pNewScript->RegisterSelf();
}
