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
SDName: boss_ahune
SD%Complete: 75
SDComment: Submerged phase visual spells NYI; they require additional research.
SDCategory: Slave Pens
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Entities/TemporarySpawn.h"
#include "LFG/LFGDefines.h"

enum
{
    // general spells
    SPELL_AHUNES_SHIELD         = 45954,
    SPELL_SPANKY_HANDS          = 46146,            // procs on melee hit; should proc 46430 but currently is not used because of the invalid proc flag
    SPELL_SYNCH_HEALTH          = 46430,
    SPELL_SUICIDE               = 45254,
    SPELL_AHUNE_LOOT            = 45941,
    SPELL_AHUNE_LOOT_H          = 46623,
    SPELL_ISDEAD_CHECK          = 61976,            // purpose unk
    SPELL_AHUNE_DIES_ACHIEV     = 62043,

    // ground phase spells
    SPELL_SUMMON_HAILSTONE      = 45951,
    SPELL_SUMMON_COLDWAVE       = 45952,
    SPELL_SUMMON_FROSTWIND      = 45953,
    SPELL_COLD_SNAP             = 46145,

    // submerged phase spells
    SPELL_BIRTH                 = 37745,            // spawn animation - not confirmed
    SPELL_SUBMERGE              = 37550,            // submerge animation - not confirmed
    SPELL_STAY_SUBMERGED        = 46981,            // triggers 37751; this should keep the boss submerged
    SPELL_AHUNE_SELF_STUN       = 46416,
    SPELL_ICE_BOMBARD           = 46397,            // cast on phase 2 end; related to the fire opening visuals
    SPELL_CLOSE_OPENING_VISUAL  = 46236,            // same as above
    SPELL_STAND                 = 37752,            // purpose unk

    // frozen core spells
    SPELL_ICE_SPEAR_AURA        = 46371,
    SPELL_FROZEN_CORE_HIT       = 46810,            // procs on melee hit; should summon npc 26239 for a 2 seconds
    SPELL_GHOST_DISGUISE        = 46786,            // triggered by spell 46809

    // ice spear spells
    SPELL_SUMMON_ICE_SPEAR_GO   = 46369,
    SPELL_ICE_SPEAR_DELAY       = 46878,
    SPELL_ICE_SPEAR_KNOCKBACK   = 46360,
    SPELL_ICE_SPEAR_VISUAL      = 75498,

    // npcs and GOs
    NPC_FROZEN_CORE             = 25865,
    NPC_BUNNY_1                 = 25964,
    NPC_BUNNY_2                 = 25965,
    NPC_BUNNY_3                 = 25966,
    NPC_GHOST_OF_AHUNE          = 26239,
    NPC_AHUNITE_HAILSTONE       = 25755,
    NPC_AHUNITE_COLDWAVE        = 25756,
    NPC_AHUNITE_FROSTWIND       = 25757,
    NPC_ICE_SPEAR_BUNNY         = 25985,
    GO_ICE_SPEAR                = 188077,

    PHASE_GROUND                = 1,
    PHASE_SUBMERGED             = 2,
};

enum AhuneActions
{
    AHUNE_SUBMERGE,
    AHUNE_COLD_SLAP,
    AHUNE_HAILSTONE,
    AHUNE_COLDWAVE,
    AHUNE_FROSTWIND,
    AHUNE_ACTION_MAX,
    AHUNE_EMERGE,
    AHUNE_ATTACK,
};

/*######
## boss_ahune
######*/

struct boss_ahuneAI : public CombatAI
{
    boss_ahuneAI(Creature* creature) : CombatAI(creature, AHUNE_ACTION_MAX)
    {
        AddCustomAction(AHUNE_ATTACK, 3000u, [&]() { HandleAttackDelay(); });
        AddCustomAction(AHUNE_EMERGE, true, [&]() { HandleEmerge(); });
        AddCombatAction(AHUNE_SUBMERGE, 90000u);
        AddCombatAction(AHUNE_COLD_SLAP, 2000u);
        AddCombatAction(AHUNE_HAILSTONE, 1000u);
        AddCombatAction(AHUNE_COLDWAVE, 5000, 10000);
        AddCombatAction(AHUNE_FROSTWIND, true);

        SetRootSelf(true);
    }

    uint32 m_uiPhaseChangeCount;

    ObjectGuid m_frozenCoreGuid;

    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(false);
        m_uiPhaseChangeCount = 0;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoCastSpellIfCan(nullptr, SPELL_BIRTH);
        DoCastSpellIfCan(m_creature, SPELL_AHUNES_SHIELD, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_SPANKY_HANDS, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        SpawnBunnies();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_AHUNE_DIES_ACHIEV, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_creature->GetMap()->IsRegularDifficulty() ? SPELL_AHUNE_LOOT : SPELL_AHUNE_LOOT_H, CAST_TRIGGERED);
        DespawnGuids(m_spawns);
        m_creature->GetMap()->AwardLFGRewards(SEASONAL_AHUNE);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
        DespawnGuids(m_spawns);
    }

    void JustReachedHome() override
    {
        // Cleanup on evade is done by creature_linking
        m_creature->ForcedDespawn();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& /*damage*/, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        // it's not clear whether this should work like this or should be handled by the proc aura
        if (Creature* pCore = m_creature->GetMap()->GetCreature(m_frozenCoreGuid))
            DoCastSpellIfCan(pCore, SPELL_SYNCH_HEALTH, CAST_TRIGGERED);
    }

    void SpellHit(Unit* /*pSource*/, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_SUBMERGE)
        {
            // Note: the following spell breaks the visual. Needs to be fixed!
            // DoCastSpellIfCan(m_creature, SPELL_AHUNE_SELF_STUN, CAST_TRIGGERED);

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            if (Creature* pCore = m_creature->GetMap()->GetCreature(m_frozenCoreGuid))
                pCore->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_AHUNITE_HAILSTONE:
            case NPC_AHUNITE_COLDWAVE:
            case NPC_AHUNITE_FROSTWIND:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    summoned->AI()->AttackStart(pTarget);
                break;
            case NPC_FROZEN_CORE:
                m_frozenCoreGuid = summoned->GetObjectGuid();
                break;
        }

        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // When the core dies, commit suicide
        if (pSummoned->GetEntry() == NPC_FROZEN_CORE)
            DoCastSpellIfCan(m_creature, SPELL_SUICIDE, CAST_TRIGGERED);
    }

    void HandleAttackDelay()
    {
        SetReactState(REACT_AGGRESSIVE);
        SetMeleeEnabled(true);
        AttackClosestEnemy();
    }

    void SpawnBunnies()
    {
        m_creature->SummonCreature(NPC_BUNNY_1, -91.0969f, -232.6422f, 24.65563f, 0.00f, TEMPSPAWN_TIMED_OOC_DESPAWN, 85000);
        m_creature->SummonCreature(NPC_BUNNY_2, -91.0969f, -232.6422f, 24.65563f, 0.00f, TEMPSPAWN_TIMED_OOC_DESPAWN, 85000);
        m_creature->SummonCreature(NPC_BUNNY_3, -91.0969f, -232.6422f, 24.65563f, 0.00f, TEMPSPAWN_TIMED_OOC_DESPAWN, 85000);
    }

    void HandleEmerge()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
        m_creature->RemoveAurasDueToSpell(SPELL_AHUNE_SELF_STUN);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        DoCastSpellIfCan(m_creature, SPELL_BIRTH);
        SpawnBunnies();

        if (Creature* pCore = m_creature->GetMap()->GetCreature(m_frozenCoreGuid))
            pCore->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PLAYER);

        ResetCombatAction(AHUNE_FROSTWIND, urand(20000, 25000));
        ResetCombatAction(AHUNE_HAILSTONE, 1000);
        ResetCombatAction(AHUNE_COLDWAVE, urand(5000, 10000));
        ResetCombatAction(AHUNE_COLD_SLAP, 1000);
        ResetCombatAction(AHUNE_SUBMERGE, 90000);

        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);

        ++m_uiPhaseChangeCount;
    }

    void ExecuteAction(uint32 action)
    {
        switch (action)
        {
            case AHUNE_SUBMERGE:
                if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE) == CAST_OK)
                {
                    DisableCombatAction(AHUNE_SUBMERGE);
                    DisableCombatAction(AHUNE_COLD_SLAP);
                    DisableCombatAction(AHUNE_HAILSTONE);
                    DisableCombatAction(AHUNE_FROSTWIND);
                    DisableCombatAction(AHUNE_COLDWAVE);
                    ResetTimer(AHUNE_EMERGE, 40000);

                    SetMeleeEnabled(false);
                    SetCombatScriptStatus(true);
                }
                break;
            case AHUNE_COLD_SLAP:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, SPELL_COLD_SNAP, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_COLD_SNAP) == CAST_OK)
                        ResetCombatAction(action, 3000);
                break;
            case AHUNE_HAILSTONE:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_HAILSTONE) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case AHUNE_COLDWAVE:
            {
                bool ok = false;
                for (uint8 i = 0; i < 2; ++i)
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_COLDWAVE) == CAST_OK)
                        ok = true;

                if (ok)
                    ResetCombatAction(action, urand(5000, 10000));
                break;
            }
            case AHUNE_FROSTWIND:
            {
                bool ok = false;
                for (uint8 i = 0; i < m_uiPhaseChangeCount; ++i)
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_FROSTWIND) == CAST_OK)
                        ok = true;

                if (ok)
                    ResetCombatAction(action, urand(5000, 10000));
                break;
            }
        }
    }
};

/*######
## npc_frozen_core
######*/

struct npc_frozen_coreAI : public Scripted_NoMovementAI
{
    npc_frozen_coreAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    ObjectGuid m_ahuheGuid;

    void Reset() override
    {
        if (m_creature->IsTemporarySummon())
            m_ahuheGuid = m_creature->GetSpawnerGuid();

        DoCastSpellIfCan(m_creature, SPELL_FROZEN_CORE_HIT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(m_creature, SPELL_ICE_SPEAR_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& /*damage*/, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        // it's not clear whether this should work like this or should be handled by the proc aura
        if (Creature* pAhune = m_creature->GetMap()->GetCreature(m_ahuheGuid))
            DoCastSpellIfCan(pAhune, SPELL_SYNCH_HEALTH, CAST_TRIGGERED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ICE_SPEAR_BUNNY)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ICE_SPEAR_VISUAL, TRIGGERED_OLD_TRIGGERED);
            pSummoned->CastSpell(pSummoned, SPELL_SUMMON_ICE_SPEAR_GO, TRIGGERED_OLD_TRIGGERED);
            pSummoned->CastSpell(pSummoned, SPELL_ICE_SPEAR_DELAY, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

/*######
## npc_ice_spear_bunny
######*/

struct npc_ice_spear_bunnyAI : public Scripted_NoMovementAI
{
    npc_ice_spear_bunnyAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    ObjectGuid m_iceSpearGuid;

    uint8 m_uiEventCount;

    void Reset() override
    {
        m_uiEventCount = 0;
    }

    void JustSummoned(GameObject* pGo) override
    {
        if (pGo->GetEntry() == GO_ICE_SPEAR)
            m_iceSpearGuid = pGo->GetObjectGuid();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            ++m_uiEventCount;

            // Knockback at 4 aura stacks (2 seconds)
            if (m_uiEventCount == 4)
            {
                DoCastSpellIfCan(m_creature, SPELL_ICE_SPEAR_KNOCKBACK);

                if (GameObject* pSpear = m_creature->GetMap()->GetGameObject(m_iceSpearGuid))
                    pSpear->Use(m_creature);
            }
            // Cleanup at 10 aura stacks (5 seconds)
            else if (m_uiEventCount == 10)
            {
                if (GameObject* pSpear = m_creature->GetMap()->GetGameObject(m_iceSpearGuid))
                    pSpear->SetLootState(GO_JUST_DEACTIVATED);

                m_creature->ForcedDespawn();
            }
        }
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

struct SummonIceSpearKnockbackDelayer : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0)
            if (aura->GetTarget()->GetEntry() == NPC_ICE_SPEAR_BUNNY)
                aura->GetTarget()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, aura->GetCaster(), aura->GetTarget());
    }
};

void AddSC_boss_ahune()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ahune";
    pNewScript->GetAI = &GetNewAIInstance<boss_ahuneAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frozen_core";
    pNewScript->GetAI = &GetNewAIInstance<npc_frozen_coreAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ice_spear_bunny";
    pNewScript->GetAI = &GetNewAIInstance<npc_ice_spear_bunnyAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SummonIceSpearKnockbackDelayer>("spell_summon_ice_spear_knockback_delayer");
}
