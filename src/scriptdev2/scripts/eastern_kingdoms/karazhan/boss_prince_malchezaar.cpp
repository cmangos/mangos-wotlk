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
SDName: Boss_Prince_Malchezzar
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    SAY_AGGRO                   = -1532091,
    SAY_AXE_TOSS1               = -1532092,
    SAY_AXE_TOSS2               = -1532093,
    SAY_SPECIAL1                = -1532094,
    SAY_SPECIAL2                = -1532095,
    SAY_SPECIAL3                = -1532096,
    SAY_SLAY1                   = -1532097,
    SAY_SLAY2                   = -1532098,
    SAY_SLAY3                   = -1532099,
    SAY_SUMMON1                 = -1532100,
    SAY_SUMMON2                 = -1532101,
    SAY_DEATH                   = -1532102,

    // Enfeeble is supposed to reduce hp to 1 and then heal player back to full when it ends
    SPELL_ENFEEBLE              = 30843,                    // Enfeeble during phases 1 and 2
    SPELL_ENFEEBLE_EFFECT       = 41624,                    // purpose unk - cast during the transition from phase 2 to 3
    SPELL_SHADOW_NOVA           = 30852,                    // Shadownova used during all phases
    SPELL_SW_PAIN_PHASE1        = 30854,                    // Shadow word pain during phase 1
    SPELL_SW_PAIN_PHASE3        = 30898,                    // Shadow word pain during phase 3
    SPELL_SUNDER_ARMOR          = 30901,                    // Sunder armor during phase 2
    SPELL_THRASH_AURA           = 12787,                    // Passive proc chance for thrash
    SPELL_SUMMON_AXES           = 30891,                    // Summon 17650
    SPELL_EQUIP_AXES            = 30857,                    // Visual for axe equiping - transition to phase 2
    SPELL_AMPLIFY_DAMAGE        = 39095,                    // Amplifiy during phase 3 3
    // SPELL_CLEAVE              = 30131,                   // spell not confirmed
    // SPELL_INFERNAL_RELAY      = 30834,                   // purpose unk
    SPELL_INFERNAL_RELAY_SUMMON = 30835,                    // triggers 30836, which summons an infernal

    SPELL_HELLFIRE              = 30859,                    // Infernal damage aura

    NPC_NETHERSPITE_INFERNAL    = 17646,                    // The netherspite infernal creature
    NPC_MALCHEZARS_AXE          = 17650,                    // Malchezar's axes summoned during phase 3

    EQUIP_ID_AXE                = 23996,                    // Axes info

    ATTACK_TIMER_DEFAULT        = 2000,                     // note: for TBC it was 2400
    ATTACK_TIMER_AXES           = 1333,                     // note: for TBC it was 1600

    MAX_ENFEEBLE_TARGETS        = 5,
};

struct boss_malchezaarAI : public ScriptedAI
{
    boss_malchezaarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 m_uiEnfeebleIndex;
    uint32 m_uiEnfeebleTimer;
    uint32 m_uiEnfeebleResetTimer;
    uint32 m_uiShadowNovaTimer;
    uint32 m_uiSWPainTimer;
    uint32 m_uiSunderArmorTimer;
    uint32 m_uiAmplifyDamageTimer;
    uint32 m_uiInfernalTimer;

    ObjectGuid m_aEnfeebleTargetGuid[MAX_ENFEEBLE_TARGETS];
    uint32 m_auiEnfeebleHealth[MAX_ENFEEBLE_TARGETS];

    uint8 m_uiPhase;

    void Reset() override
    {
        for (uint8 i = 0; i < MAX_ENFEEBLE_TARGETS; ++i)
        {
            m_aEnfeebleTargetGuid[i].Clear();
            m_auiEnfeebleHealth[i] = 0;
        }

        m_uiEnfeebleIndex           = 0;
        m_uiEnfeebleTimer           = 30000;
        m_uiEnfeebleResetTimer      = 0;
        m_uiShadowNovaTimer         = 35500;
        m_uiSWPainTimer             = 20000;
        m_uiAmplifyDamageTimer      = 5000;
        m_uiInfernalTimer           = 40000;
        m_uiSunderArmorTimer        = urand(5000, 10000);

        m_uiPhase                   = 1;

        // Reset equipment and attack
        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
        m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_DEFAULT);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Remove the summoned axe - which is considered a guardian
        m_creature->RemoveGuardians();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALCHEZZAR, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALCHEZZAR, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALCHEZZAR, FAIL);

        // Remove the summoned axe - which is considered a guardian
        m_creature->RemoveGuardians();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_NETHERSPITE_INFERNAL)
            pSummoned->CastSpell(pSummoned, SPELL_HELLFIRE, false);
        else if (pSummoned->GetEntry() == NPC_MALCHEZARS_AXE)
            pSummoned->SetInCombatWithZone();
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        // Target selection is already handled properly in core (doesn't affect tank)
        if (pSpellEntry->Id == SPELL_ENFEEBLE && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            // Workaround to handle health set to 1
            m_aEnfeebleTargetGuid[m_uiEnfeebleIndex] = pTarget->GetObjectGuid();
            m_auiEnfeebleHealth[m_uiEnfeebleIndex] = pTarget->GetHealth();
            pTarget->SetHealth(1);
            ++m_uiEnfeebleIndex;
        }
    }

    // Wrapper to reset health of the Enfeebled targets
    void DoHandleEnfeebleHealthReset()
    {
        for (int i = 0; i < m_uiEnfeebleIndex; ++i)
        {
            Player* pTarget = m_creature->GetMap()->GetPlayer(m_aEnfeebleTargetGuid[i]);

            if (pTarget && pTarget->isAlive())
                pTarget->SetHealth(m_auiEnfeebleHealth[i]);

            m_aEnfeebleTargetGuid[i].Clear();
            m_auiEnfeebleHealth[i] = 0;
        }

        m_uiEnfeebleIndex = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Phase 1 - over 60% HP
        if (m_uiPhase == 1)
        {
            // transition to phase 2
            if (m_creature->GetHealthPercent() < 60.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EQUIP_AXES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THRASH_AURA, CAST_TRIGGERED);
                    DoScriptText(SAY_AXE_TOSS1, m_creature);

                    SetEquipmentSlots(false, EQUIP_ID_AXE, EQUIP_ID_AXE, EQUIP_NO_CHANGE);
                    m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_AXES);
                    m_uiPhase = 2;
                }
            }
        }
        // Phase 2 - over 30% HP
        else if (m_uiPhase == 2)
        {
            // transition to phase 3
            if (m_creature->GetHealthPercent() < 30.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_AXES) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_ENFEEBLE_EFFECT, CAST_TRIGGERED);
                    DoScriptText(SAY_SPECIAL3, m_creature);

                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                    m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_DEFAULT);

                    // Reset Enfeebled targets if necessary
                    DoHandleEnfeebleHealthReset();
                    m_uiEnfeebleResetTimer = 0;

                    m_creature->RemoveAurasDueToSpell(SPELL_THRASH_AURA);
                    m_uiShadowNovaTimer = m_uiEnfeebleTimer + 5000;
                    m_uiInfernalTimer = 15000;
                    m_uiPhase = 3;

                    return;
                }
            }

            if (m_uiSunderArmorTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR) == CAST_OK)
                    m_uiSunderArmorTimer = urand(10000, 18000);
            }
            else
                m_uiSunderArmorTimer -= uiDiff;
        }
        // Phase 3
        else
        {
            if (m_uiAmplifyDamageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_AMPLIFY_DAMAGE) == CAST_OK)
                    m_uiAmplifyDamageTimer = urand(20000, 30000);
            }
            else
                m_uiAmplifyDamageTimer -= uiDiff;
        }

        // Summon an infernal on timer
        if (m_uiInfernalTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INFERNAL_RELAY_SUMMON) == CAST_OK)
            {
                DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);
                m_uiInfernalTimer =  m_uiPhase == 3 ? 17000 : 45000;
            }
        }
        else
            m_uiInfernalTimer -= uiDiff;

        // Cast shadow nova - on timer during phase 3, or after Enfeeble during phases 1 and 2
        if (m_uiShadowNovaTimer)
        {
            if (m_uiShadowNovaTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_NOVA) == CAST_OK)
                    m_uiShadowNovaTimer = m_uiPhase == 3 ? 30000 : 0;
            }
            else
                m_uiShadowNovaTimer -= uiDiff;
        }

        // Cast SW pain during phase 1 and 3
        if (m_uiPhase != 2)
        {
            if (m_uiSWPainTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_uiPhase == 1 ? m_creature->getVictim() : m_creature, m_uiPhase == 1 ? SPELL_SW_PAIN_PHASE1 : SPELL_SW_PAIN_PHASE3) == CAST_OK)
                    m_uiSWPainTimer = 20000;
            }
            else
                m_uiSWPainTimer -= uiDiff;
        }

        // Cast Enfeeble during phase 1 and 2
        if (m_uiPhase != 3)
        {
            if (m_uiEnfeebleTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENFEEBLE) == CAST_OK)
                {
                    m_uiEnfeebleTimer       = 30000;
                    m_uiShadowNovaTimer     = 5000;
                    m_uiEnfeebleResetTimer  = 9000;
                }
            }
            else
                m_uiEnfeebleTimer -= uiDiff;
        }

        if (m_uiEnfeebleResetTimer)
        {
            if (m_uiEnfeebleResetTimer <= uiDiff)
            {
                DoHandleEnfeebleHealthReset();
                m_uiEnfeebleResetTimer = 0;
            }
            else
                m_uiEnfeebleResetTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_malchezaar(Creature* pCreature)
{
    return new boss_malchezaarAI(pCreature);
}

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_infernal_targetAI : public Scripted_NoMovementAI
{
    npc_infernal_targetAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_infernal_target(Creature* pCreature)
{
    return new npc_infernal_targetAI(pCreature);
}

void AddSC_boss_prince_malchezaar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_malchezaar";
    pNewScript->GetAI = &GetAI_boss_malchezaar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_infernal_target";
    pNewScript->GetAI = &GetAI_npc_infernal_target;
    pNewScript->RegisterSelf();
}
