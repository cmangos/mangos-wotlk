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
SDName: boss_rotface
SD%Complete: 70%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SAY_AGGRO                   = -1631071,
    SAY_SLIME_SPRAY             = -1631072,
    SAY_OOZE_EXPLODE            = -1631073,
    SAY_SLIME_FLOW_1            = -1631074,
    SAY_SLIME_FLOW_2            = -1631075,
    SAY_SLAY_1                  = -1631076,
    SAY_SLAY_2                  = -1631077,
    SAY_BERSERK                 = -1631078,
    SAY_DEATH                   = -1631079,
    SAY_ROTFACE_DEATH           = -1631080,
};

enum
{
    // Mutated Infection
    SPELL_MUTATED_INFECTION_1   = 70090, // periodic trigger auras
    SPELL_MUTATED_INFECTION_2   = 70003,
    SPELL_MUTATED_INFECTION_3   = 70004,
    SPELL_MUTATED_INFECTION_4   = 70005,
    SPELL_MUTATED_INFECTION_5   = 70006,

    // Slime Spray
// SPELL_SLIME_SPRAY_SUMMON    = 70882, // precast of Slime Spray dmg spell
    SPELL_SLIME_SPRAY           = 69508,

    // Ooze Flood
    SPELL_OOZE_FLOOD_PERIODIC   = 70069, // periodically trigger ooze flood
    SPELL_OOZE_FLOOD_REMOVE     = 70079,

    // Little Ooze
    SPELL_STICKY_OOZE           = 69774,
    SPELL_STICKY_AURA           = 69776, // aura on dummy Sticky Ooze NPC
    SPELL_WEAK_RADIATING_OOZE   = 69750,
    SPELL_LITTLE_OOZE_COMBINE   = 69537, // periodic check
// SPELL_MERGE                 = 69889,

    // Big Ooze
    SPELL_UNSTABLE_OOZE         = 69558, // stacking buff
    SPELL_RADIATING_OOZE        = 69760,
    SPELL_BIG_OOZE_COMBINE      = 69552, // periodic check
    SPELL_BIG_OOZE_BUFF_COMB    = 69611, // periodic check
    SPELL_UNSTABLE_EXPLOSION    = 69839,

    MAX_MUTATE_INFACTION_STEPS  = 5,
};

static const uint32 uiMutatedInfections[MAX_MUTATE_INFACTION_STEPS] =
{
    SPELL_MUTATED_INFECTION_1,
    SPELL_MUTATED_INFECTION_2,
    SPELL_MUTATED_INFECTION_3,
    SPELL_MUTATED_INFECTION_4,
    SPELL_MUTATED_INFECTION_5
};

struct boss_rotfaceAI : public ScriptedAI
{
    boss_rotfaceAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiSlimeSprayTimer;
    uint32 m_uiSlimeFlowTimer;
    uint32 m_uiMutatedInfectionTimer;
    uint32 m_uiInfectionsRate;

    void Reset() override
    {
        m_uiSlimeSprayTimer             = urand(17000, 23000);
        m_uiSlimeFlowTimer              = 20000;
        m_uiMutatedInfectionTimer       = 60000;
        m_uiInfectionsRate              = 1;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROTFACE, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_MUTATED_INFECTION_1, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_PERIODIC, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROTFACE, FAIL);

        DoCastSpellIfCan(m_creature, SPELL_OOZE_FLOOD_REMOVE, CAST_TRIGGERED);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature, pVictim);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROTFACE, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Slime Spray
        if (m_uiSlimeSprayTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SLIME_SPRAY) == CAST_OK)
            {
                DoScriptText(SAY_SLIME_SPRAY, m_creature);
                m_uiSlimeSprayTimer = urand(17000, 23000);
            }
        }
        else
            m_uiSlimeSprayTimer -= uiDiff;

        // Mutated Infection - faster with time
        // implemented this instead of phases
        if (m_uiInfectionsRate < MAX_MUTATE_INFACTION_STEPS)
        {
            if (m_uiMutatedInfectionTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, uiMutatedInfections[m_uiInfectionsRate], CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->RemoveAurasDueToSpell(uiMutatedInfections[m_uiInfectionsRate - 1]);
                    // every next 15 seconds faster
                    m_uiMutatedInfectionTimer = 60000 - m_uiInfectionsRate * 15000;
                    ++m_uiInfectionsRate;
                }
            }
            else
                m_uiMutatedInfectionTimer -= uiDiff;
        }

        // Slime Flow
        if (m_uiSlimeFlowTimer <= uiDiff)
        {
            if (Creature* pProfessor = m_pInstance->GetSingleCreatureFromStorage(NPC_PROFESSOR_PUTRICIDE))
                DoScriptText(urand(0, 1) ? SAY_SLIME_FLOW_1 : SAY_SLIME_FLOW_2, pProfessor);

            m_uiSlimeFlowTimer = 20000;
        }
        else
            m_uiSlimeFlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rotface(Creature* pCreature)
{
    return new boss_rotfaceAI(pCreature);
}

struct mob_little_oozeAI : public ScriptedAI
{
    mob_little_oozeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    uint32 m_uiStickyOozeTimer;

    void Reset() override
    {
        m_uiStickyOozeTimer = 5000;
    }

    void EnterEvadeMode() override
    {
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->AddThreat(pWho, 500000.0f); // not sure about the threat amount but should be very high
        DoCastSpellIfCan(m_creature, SPELL_WEAK_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_LITTLE_OOZE_COMBINE, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStickyOozeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STICKY_OOZE) == CAST_OK)
                m_uiStickyOozeTimer = urand(10000, 15000);
        }
        else
            m_uiStickyOozeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_little_ooze(Creature* pCreature)
{
    return new mob_little_oozeAI(pCreature);
}

struct mob_big_oozeAI : public ScriptedAI
{
    mob_big_oozeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    uint32 m_uiStickyOozeTimer;
    uint32 m_uiUnstableExplosionCheckTimer;

    void Reset() override
    {
        m_uiStickyOozeTimer             = 5000;
        m_uiUnstableExplosionCheckTimer = 1000;
    }

    void EnterEvadeMode() override
    {
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->AddThreat(pWho, 500000.0f);
        DoCastSpellIfCan(m_creature, SPELL_RADIATING_OOZE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_COMBINE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BIG_OOZE_BUFF_COMB, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Unstable Ooze
        if (m_uiUnstableExplosionCheckTimer)
        {
            if (m_uiUnstableExplosionCheckTimer <= uiDiff)
            {
                m_uiUnstableExplosionCheckTimer = 1000;

                SpellAuraHolder* holder = m_creature->GetSpellAuraHolder(SPELL_UNSTABLE_OOZE);
                if (holder && holder->GetStackAmount() >= 5)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_EXPLOSION) == CAST_OK)
                    {
                        if (m_pInstance)
                        {
                            if (Creature* pRotface = m_pInstance->GetSingleCreatureFromStorage(NPC_ROTFACE))
                                DoScriptText(SAY_OOZE_EXPLODE, pRotface);
                        }
                    }
                }
            }
            else
                m_uiUnstableExplosionCheckTimer -= uiDiff;
        }

        // Sticky Ooze
        if (m_uiStickyOozeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STICKY_OOZE) == CAST_OK)
                m_uiStickyOozeTimer = urand(10000, 15000);
        }
        else
            m_uiStickyOozeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_big_ooze(Creature* pCreature)
{
    return new mob_big_oozeAI(pCreature);
}

void AddSC_boss_rotface()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_rotface";
    pNewscript->GetAI = &GetAI_boss_rotface;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_little_ooze";
    pNewscript->GetAI = &GetAI_mob_little_ooze;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_big_ooze";
    pNewscript->GetAI = &GetAI_mob_big_ooze;
    pNewscript->RegisterSelf();
}
