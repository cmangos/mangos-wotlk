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
SD%Complete: 100%
SDComment:
SDCategory: The Forge of Souls
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "forge_of_souls.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

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

enum KelesethActions
{
    BRONJAHM_ACTION_SHADOW_BOLT,
    BRONJAHM_ACTION_MAGIC_BANE,
    BRONJAHM_ACTION_FEAR,
    BRONJAHM_ACTION_TELEPORT,
    BRONJAHM_ACTION_CORRUPT_SOUL,
    BRONJAHM_ACTION_MAX
};

struct boss_bronjahmAI : public CombatAI
{
    boss_bronjahmAI(Creature* creature) : CombatAI(creature, BRONJAHM_ACTION_MAX), m_instance(static_cast<instance_forge_of_souls*>(creature->GetInstanceData()))
    {
        AddCombatAction(BRONJAHM_ACTION_MAGIC_BANE, 8000u, 12000u);
        AddCombatAction(BRONJAHM_ACTION_CORRUPT_SOUL, 20000u, 30000u);
        AddCombatAction(BRONJAHM_ACTION_SHADOW_BOLT, 2000u);
        AddCombatAction(BRONJAHM_ACTION_FEAR, true);
        AddTimerlessCombatAction(BRONJAHM_ACTION_TELEPORT, true);
        AddMainSpell(SPELL_SHADOW_BOLT);
        SetRangedMode(true, 20.f, TYPE_PROXIMITY);
    }

    instance_forge_of_souls* m_instance;

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_AGGRO_1 : SAY_AGGRO_2, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_BRONJAHM, IN_PROGRESS);

        // Remove OOC visual soulstorm effect (added in creature_template_addon
        m_creature->InterruptNonMeleeSpells(false);
        m_creature->RemoveAurasDueToSpell(SPELL_SOULSTORM_VISUAL_OOC);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_BRONJAHM, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BRONJAHM, FAIL);

        SetCombatMovement(true);
    }

    void JustRespawned() override
    {
        SetCombatMovement(true);
    }

    void SpellHitTarget(Unit* pTarget, SpellEntry const* pSpellEntry) override
    {
        if (pTarget == m_creature && pSpellEntry->Id == SPELL_TELEPORT)
        {
            // Say Text and cast Soulstorm
            DoScriptText(SAY_SOULSTORM, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_SOULSTORM_VISUAL, CAST_TRIGGERED | CAST_INTERRUPT_PREVIOUS);
            DoCastSpellIfCan(m_creature, SPELL_SOULSTORM, CAST_INTERRUPT_PREVIOUS);

            // enable phase 2 spells
            SetCombatMovement(false);
            ResetCombatAction(BRONJAHM_ACTION_FEAR, urand(1000, 2000));
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BRONJAHM_ACTION_MAGIC_BANE:
                if (DoCastSpellIfCan(m_creature, SPELL_MAGICS_BANE) == CAST_OK)
                    ResetCombatAction(action, urand(7000, 15000));
                break;
            case BRONJAHM_ACTION_CORRUPT_SOUL:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_CORRUPT_SOUL) == CAST_OK)
                    {
                        DoScriptText(SAY_CORRUPT_SOUL, m_creature);
                        ResetCombatAction(action, urand(20000, 30000));
                    }
                }
                break;
            case BRONJAHM_ACTION_SHADOW_BOLT:
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_BOLT) == CAST_OK)
                            ResetCombatAction(action, urand(1000, 2000));
                    }
                }
                else
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                        ResetCombatAction(action, urand(1000, 2000));
                }
                break;
            case BRONJAHM_ACTION_FEAR:
                if (DoCastSpellIfCan(m_creature, SPELL_FEAR) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case BRONJAHM_ACTION_TELEPORT:
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT) == CAST_OK)
                    {
                        // prepare phase 2; disable phase 1 spells
                        SetActionReadyStatus(action, false);

                        DisableCombatAction(BRONJAHM_ACTION_MAGIC_BANE);
                        DisableCombatAction(BRONJAHM_ACTION_CORRUPT_SOUL);
                    }
                }
                break;
        }
    }
};

struct npc_corrupted_soul_fragmentAI : public ScriptedAI
{
    npc_corrupted_soul_fragmentAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_instance = static_cast<instance_forge_of_souls*>(pCreature->GetInstanceData());
        DoCastSpellIfCan(m_creature, SPELL_BANISH_VISUAL);
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    instance_forge_of_souls* m_instance;

    void Reset() override { }

    void JustRespawned() override
    {
        if (m_instance)
            if (Creature* pBronjahm = m_instance->GetSingleCreatureFromStorage(NPC_BRONJAHM))
                m_creature->GetMotionMaster()->MoveChase(pBronjahm, 0.f, 0.f, false, true, false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_instance)
            m_instance->SetGuid(DATA_SOULFRAGMENT_REMOVE, m_creature->GetObjectGuid());
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_BRONJAHM && m_creature->IsWithinDistInMap(pWho, CONTACT_DISTANCE * 3))
        {
            DoCastSpellIfCan(pWho, SPELL_CONSUME_SOUL_TRIGGER, CAST_TRIGGERED);

            // Inform the instance about a used soul fragment
            if (m_instance)
                m_instance->SetGuid(DATA_SOULFRAGMENT_REMOVE, m_creature->GetObjectGuid());

            m_creature->ForcedDespawn();
        }
    }
};

/*######
## spell_consume_soul - 68861
######*/

struct spell_consume_soul : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // trigger spell 68858; difficulty handled in core
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_corrupt_soul_aura - 68839
######*/

struct spell_corrupt_soul_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply)
        {
            // Knockdown Stun
            target->CastSpell(target, 68848, TRIGGERED_OLD_TRIGGERED);
            // Draw Corrupted Soul
            target->CastSpell(target, 68846, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

/*######
## spell_soulstorm_visual_aura - 68870, 69008
######*/

struct spell_soulstorm_visual_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        uint32 auraList[8] = { 68886, 68896, 68897, 68898, 68904, 68905, 68906, 68907 };
        target->CastSpell(target, auraList[urand(0, 7)], TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_bronjahm()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_bronjahm";
    pNewScript->GetAI = &GetNewAIInstance<boss_bronjahmAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_corrupted_soul_fragment";
    pNewScript->GetAI = &GetNewAIInstance<npc_corrupted_soul_fragmentAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_consume_soul>("spell_consume_soul");
    RegisterSpellScript<spell_corrupt_soul_aura>("spell_corrupt_soul_aura");
    RegisterSpellScript<spell_soulstorm_visual_aura>("spell_soulstorm_visual_aura");
}
