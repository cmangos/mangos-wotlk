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
SDName: boss_bronjahm
SD%Complete: 90%
SDComment: Small unknown behaviour for his Shadow Bold use in phase 1; Soulstorm needs additional handling in core
SDCategory: The Forge of Souls
EndScriptData */

#include "precompiled.h"
#include "forge_of_souls.h"

enum
{
    SAY_AGGRO_1                 = -1632000,                 // Without sound, really correct?
    SAY_AGGRO_2                 = -1632001,
    SAY_SLAY_1                  = -1632002,
    SAY_SLAY_2                  = -1632003,
    SAY_DEATH                   = -1632004,
    SAY_SOULSTORM               = -1632005,
    SAY_CORRUPT_SOUL            = -1632006,

    // Heroic spells are selected by spell difficulty dbc
    SPELL_SOULSTORM_VISUAL_OOC  = 69008,
    SPELL_MAGICS_BANE           = 68793,
    SPELL_SHADOW_BOLT           = 70043,
    SPELL_CORRUPT_SOUL          = 68839,
    SPELL_BANISH_VISUAL         = 68862,
    SPELL_CONSUME_SOUL_TRIGGER  = 68861,
    SPELL_TELEPORT              = 68988,
    SPELL_SOULSTORM_VISUAL      = 68870,                    // Cast before Soulstorm, should trigger some visual spells
    SPELL_SOULSTORM             = 68872,
    SPELL_FEAR                  = 68950,
};

struct boss_bronjahmAI : public ScriptedAI
{
    boss_bronjahmAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_forge_of_souls*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_forge_of_souls* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;

    uint32 m_uiMagicsBaneTimer;
    uint32 m_uiCorruptSoulTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiShadowboltTimer;

    void Reset() override
    {
        m_uiPhase = 0;
        m_uiMagicsBaneTimer = urand(8000, 12000);
        m_uiCorruptSoulTimer = urand(20000, 30000);
        m_uiFearTimer = 1000;
        m_uiShadowboltTimer = 5000;
        SetCombatMovement(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_AGGRO_1 : SAY_AGGRO_2, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BRONJAHM, IN_PROGRESS);

        // Remove OOC visual soulstorm effect (added in creature_template_addon
        m_creature->RemoveAurasDueToSpell(SPELL_SOULSTORM_VISUAL_OOC);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BRONJAHM, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BRONJAHM, FAIL);
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget == m_creature && pSpellEntry->Id == SPELL_TELEPORT)
        {
            // Say Text and cast Soulstorm
            DoScriptText(SAY_SOULSTORM, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_SOULSTORM_VISUAL, CAST_TRIGGERED | CAST_INTERRUPT_PREVIOUS);
            DoCastSpellIfCan(m_creature, SPELL_SOULSTORM, CAST_INTERRUPT_PREVIOUS);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == 0)                                 // Phase 1
        {
            // Switching Phase, Soulstorm is cast in SpellHitTarget
            if (m_creature->GetHealthPercent() < 30.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                    m_uiPhase = 1;
            }

            // Corrupt Soul
            if (m_uiCorruptSoulTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CORRUPT_SOUL) == CAST_OK)
                    {
                        DoScriptText(SAY_CORRUPT_SOUL, m_creature);
                        m_uiCorruptSoulTimer = urand(20000, 30000);
                    }
                }
            }
            else
                m_uiCorruptSoulTimer -= uiDiff;

            // Magic's Bane
            if (m_uiMagicsBaneTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_MAGICS_BANE) == CAST_OK)
                    m_uiMagicsBaneTimer = urand(7000, 15000);
            }
            else
                m_uiMagicsBaneTimer -= uiDiff;

            // Used to prevent Shadowbolt-Casting on Aggro for a few seconds
            if (m_uiShadowboltTimer <= uiDiff)
                m_uiShadowboltTimer = 0;
            else
                m_uiShadowboltTimer -= uiDiff;

            // Use ShadowBolt as default attack if victim is not in range
            // TODO - not entirely clear how this works in case the tank is out of shadow-bolt range
            if (!m_uiShadowboltTimer && !m_creature->CanReachWithMeleeAttack(m_creature->getVictim()) && m_creature->GetCombatDistance(m_creature->getVictim(), false) < 20.0f)
            {
                if (IsCombatMovement())
                {
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->StopMoving();
                }
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            }
            else
            {
                if (!IsCombatMovement())
                {
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_uiShadowboltTimer = 2000;             // Give some time to chase
                }

                DoMeleeAttackIfReady();
            }
        }
        else                                                // Soulstorm Phase
        {
            if (m_uiFearTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FEAR) == CAST_OK)
                    m_uiFearTimer = urand(10000, 15000);
            }
            else
                m_uiFearTimer -= uiDiff;

            // Default attack
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_SHADOW_BOLT);
        }
    }
};

CreatureAI* GetAI_boss_bronjahm(Creature* pCreature)
{
    return new boss_bronjahmAI(pCreature);
}

struct npc_corrupted_soul_fragmentAI : public ScriptedAI
{
    npc_corrupted_soul_fragmentAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        DoCastSpellIfCan(m_creature, SPELL_BANISH_VISUAL);
    }

    void Reset() override
    {
        SetCombatMovement(true);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (instance_forge_of_souls* pInstance = (instance_forge_of_souls*)m_creature->GetInstanceData())
            pInstance->SetGuid(DATA_SOULFRAGMENT_REMOVE, m_creature->GetObjectGuid());
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_BRONJAHM)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                DoCastSpellIfCan(pWho, SPELL_CONSUME_SOUL_TRIGGER, CAST_TRIGGERED);

                // Inform the instance about a used soul fragment
                if (instance_forge_of_souls* pInstance = (instance_forge_of_souls*)m_creature->GetInstanceData())
                    pInstance->SetGuid(DATA_SOULFRAGMENT_REMOVE, m_creature->GetObjectGuid());

                m_creature->ForcedDespawn();
                return;
            }
            if (IsCombatMovement())
            {
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MoveFollow(pWho, 0.0f, 0.0f);
            }
        }
    }
};

CreatureAI* GetAI_npc_corrupted_soul_fragment(Creature* pCreature)
{
    return new npc_corrupted_soul_fragmentAI(pCreature);
}

void AddSC_boss_bronjahm()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_bronjahm";
    pNewScript->GetAI = &GetAI_boss_bronjahm;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_corrupted_soul_fragment";
    pNewScript->GetAI = &GetAI_npc_corrupted_soul_fragment;
    pNewScript->RegisterSelf();
}
