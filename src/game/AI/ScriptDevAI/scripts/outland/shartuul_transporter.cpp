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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/outland/world_outland.h"
#include "Spells/Scripts/SpellScript.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum Possession
{
    SPELL_POSSESSION_TRANSFER_P1 = 40503,
    SPELL_POSSESSION_TRANSFER_P2 = 41962,

    SPELL_CHARM_P2 = 40382,
    SPELL_CHARM_P3 = 40523,
};

enum
{
    SPELL_FEL_FLAMES = 40561,
};

enum PunisherActions
{
    PUNISHER_POSSESSION_TRANSFER,
    PUNISHER_FEL_FLAMES,
    PUNISHER_ACTION_MAX,
    PUNISHER_ATTACK_DELAY,
};

struct npc_doomguard_punisherAI : public CombatAI
{
    npc_doomguard_punisherAI(Creature* creature) : CombatAI(creature, PUNISHER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        AddTimerlessCombatAction(PUNISHER_POSSESSION_TRANSFER, false);
        AddCombatAction(PUNISHER_FEL_FLAMES, 30000u);
        AddCustomAction(PUNISHER_ATTACK_DELAY, true, [&]() { HandleAttackDelay(); });
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
    }

    ScriptedInstance* m_instance;
    ObjectGuid m_demonGuid;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/)
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_demonGuid = invoker->GetObjectGuid();
            ResetTimer(PUNISHER_ATTACK_DELAY, 5000);
        }
    }

    void HandleAttackDelay()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
        m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
        if (Creature* demon = m_creature->GetMap()->GetCreature(m_demonGuid))
            AttackStart(demon);
        else if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void JustPreventedDeath(Unit* killer) override
    {
        SetActionReadyStatus(PUNISHER_POSSESSION_TRANSFER, true);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case PUNISHER_POSSESSION_TRANSFER:
                if (DoCastSpellIfCan(nullptr, SPELL_POSSESSION_TRANSFER_P1) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            case PUNISHER_FEL_FLAMES:
                if (DoCastSpellIfCan(nullptr, SPELL_FEL_FLAMES) == CAST_OK)
                    ResetCombatAction(action, 60000);
                break;
        }
    }
};

enum ShivanActions
{
    SHIVAN_POSSESSION_TRANSFER,
    SHIVAN_ACTION_MAX,
    SHIVAN_ATTACK_DELAY,
    SHIVAN_OOC_ANIM,
};

struct npc_shivan_assassinAI : public CombatAI
{
    npc_shivan_assassinAI(Creature* creature) : CombatAI(creature, SHIVAN_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        AddCombatAction(SHIVAN_POSSESSION_TRANSFER, true);
        AddCustomAction(SHIVAN_ATTACK_DELAY, true, [&]() { HandleAttackDelay(); });
        AddCustomAction(SHIVAN_OOC_ANIM, 1000u, [&]() { HandleOocAnim(); });
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
    }

    ScriptedInstance* m_instance;

    ObjectGuid m_demonGuid;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/)
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_demonGuid = invoker->GetObjectGuid();
            ResetTimer(SHIVAN_ATTACK_DELAY, 5000);
            DisableTimer(SHIVAN_OOC_ANIM);
        }
    }

    void HandleAttackDelay()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
        m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
        if (Creature* demon = m_creature->GetMap()->GetCreature(m_demonGuid))
            AttackStart(demon);
        else if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void HandleOocAnim()
    {
        if (m_instance)
            if (Creature* dummy = m_instance->GetSingleCreatureFromStorage(NPC_WARP_GATE_SHIELD_SHARTUUL))
                m_creature->CastSpell(dummy, SPELL_LEGION_RING_SHIELD_LIGHTNING, TRIGGERED_NONE);

        ResetTimer(SHIVAN_OOC_ANIM, 8000);
    }

    void JustPreventedDeath(Unit* killer) override
    {
        SetActionReadyStatus(SHIVAN_POSSESSION_TRANSFER, true);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHIVAN_POSSESSION_TRANSFER:
                if (DoCastSpellIfCan(nullptr, SPELL_POSSESSION_TRANSFER_P2) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
        }
    }
};

enum
{
    SPELL_FEL_FIREBALL = 40633,
    SPELL_DISRUPTION_RAY = 41550,
    SPELL_DARK_GLARE = 41936,
    SPELL_TONGUE_LASH = 42002,
};

enum EyeActions
{
    EYE_DARK_GLARE,
    EYE_DISRUPTION_RAY,
    EYE_TONGUE_LASH,
    EYE_FEL_FIREBALL,
    EYE_ACTION_MAX,
    EYE_ATTACK_DELAY,
};

struct npc_eye_of_shartuulAI : public CombatAI
{
    npc_eye_of_shartuulAI(Creature* creature) : CombatAI(creature, EYE_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        AddCombatAction(EYE_DARK_GLARE, 60000u);
        AddCombatAction(EYE_DISRUPTION_RAY, 10000u);
        AddCombatAction(EYE_TONGUE_LASH, 5000u);
        AddCombatAction(EYE_FEL_FIREBALL, 15000u);
        AddCustomAction(EYE_ATTACK_DELAY, true, [&]() { HandleAttackDelay(); });
    }

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
    }

    ScriptedInstance* m_instance;

    ObjectGuid m_demonGuid;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/)
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_demonGuid = invoker->GetObjectGuid();
            ResetTimer(EYE_ATTACK_DELAY, 5000);
        }
    }

    void HandleAttackDelay()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
        if (Creature* demon = m_creature->GetMap()->GetCreature(m_demonGuid))
            AttackStart(demon);
        else if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void JustPreventedDeath(Unit* killer) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_EYE_OF_SHARTUUL_DEATH);
        m_creature->Suicide();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case EYE_DARK_GLARE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DARK_GLARE) == CAST_OK)
                {
                    DoScriptText(SAY_EYE_GLARE, m_creature);
                    ResetCombatAction(action, 60000);
                }
                break;
            case EYE_DISRUPTION_RAY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DISRUPTION_RAY) == CAST_OK)
                    ResetCombatAction(action, 30000);
                break;
            case EYE_TONGUE_LASH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_TONGUE_LASH) == CAST_OK)
                    ResetCombatAction(action, 5000);
                break;
            case EYE_FEL_FIREBALL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FEL_FIREBALL) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
        }
    }
};

enum
{
    SPELL_RAMPAGING_CHARGE  = 41939,
    SPELL_GROWTH            = 41953,
    SPELL_LACERATING_BITE   = 41940,
};

enum DreadmawActions
{
    DREADMAW_RAMPAGING_CHARGE,
    DREADMAW_GROWTH,
    DREADMAW_ACTION_MAX,
    DREADMAW_ATTACK_DELAY,
};

struct npc_dreadmawAI : public CombatAI
{
    npc_dreadmawAI(Creature* creature) : CombatAI(creature, DREADMAW_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        AddCombatAction(DREADMAW_RAMPAGING_CHARGE, 60000u);
        AddCombatAction(DREADMAW_GROWTH, 30000u);
        AddCustomAction(DREADMAW_ATTACK_DELAY, true, [&]() { HandleAttackDelay(); });
    }

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
    }

    ScriptedInstance* m_instance;

    ObjectGuid m_demonGuid;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/)
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_demonGuid = invoker->GetObjectGuid();
            ResetTimer(DREADMAW_ATTACK_DELAY, 5000);
        }
    }

    void HandleAttackDelay()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
        if (Creature* demon = m_creature->GetMap()->GetCreature(m_demonGuid))
            AttackStart(demon);
        else if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void JustPreventedDeath(Unit* killer) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_DREADMAW_DEATH);
        m_creature->Suicide();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo) override
    {
        if (spellInfo->Id == SPELL_LACERATING_BITE)
            DoScriptText(SAY_DREADMAW_LASH, m_creature, target);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case DREADMAW_RAMPAGING_CHARGE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RAMPAGING_CHARGE) == CAST_OK)
                    ResetCombatAction(action, 60000);
                break;
            case DREADMAW_GROWTH:
                if (DoCastSpellIfCan(nullptr, SPELL_GROWTH) == CAST_OK)
                    ResetCombatAction(action, 60000);
                break;
        }
    }
};

enum
{
    SPELL_SHADOW_BOLT = 41069,
    SPELL_SHADOW_RESONANCE = 41961,
    SPELL_MAGNETIC_PULL = 41959,
    SPELL_TELEPORT = 41993,
    SPELL_IMMOLATE = 41958,
    SPELL_INCINERATE = 41960,
};

enum ShartuulActions
{
    SHARTUUL_TELEPORT,
    SHARTUUL_INCINERATE,
    SHARTUUL_IMMOLATE,
    SHARTUUL_SHADOW_RESONANCE,
    SHARTUUL_SHADOW_BOLT,
    SHARTUUL_ACTION_MAX,
    SHARTUUL_HANDLE_FIGHT_START,
};

struct npc_shartuulAI : public CombatAI
{
    npc_shartuulAI(Creature* creature) : CombatAI(creature, SHARTUUL_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_fightSequenceStage(0)
    {
        m_creature->GetCombatManager().SetLeashingDisable(true);
        // AddCombatAction(SHARTUUL_TELEPORT, 60000u);
        AddCombatAction(SHARTUUL_INCINERATE, 60000u);
        AddCombatAction(SHARTUUL_IMMOLATE, 30000u);
        AddCombatAction(SHARTUUL_SHADOW_RESONANCE, 15000u);
        AddCombatAction(SHARTUUL_SHADOW_BOLT, 3000u);
        AddCustomAction(SHARTUUL_HANDLE_FIGHT_START, true, [&]() { HandleFightSequence(); });
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        SetRangedMode(true, 30.f, TYPE_FULL_CASTER);
    }

    void Reset() override
    {
        CombatAI::Reset();
        SetDeathPrevention(true);
    }

    ScriptedInstance* m_instance;

    uint32 m_fightSequenceStage;

    void JustPreventedDeath(Unit* killer) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_SHARTUUL_DEATH);

        DoScriptText(SAY_SHARTUUL_DEATH, m_creature, killer->GetCharmer());
        m_creature->Suicide();
    }

    void HandleFightSequence()
    {
        uint32 timer = 0;
        switch (m_fightSequenceStage)
        {
            case 0:
                if (Unit* spawner = m_creature->GetSpawner())
                    m_creature->SetFacingToObject(spawner->GetCharm());
                timer = 2000;
                break;
            case 1:
                DoScriptText(SAY_SHARTUUL_AGGRO, m_creature, m_creature->GetSpawner());
                timer = 10000;
                break;
            case 2:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
                m_creature->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
                Unit* spawner = m_creature->GetSpawner();
                Unit* charm = nullptr;
                if (spawner)
                    charm = spawner->GetCharm();
                if (charm)
                    AttackStart(charm);
                else if (m_instance)
                    m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
                break;
        }
        ++m_fightSequenceStage;
        if (timer)
            ResetTimer(SHARTUUL_HANDLE_FIGHT_START, timer);
    }

    void MovementInform(uint32 motionType, uint32 data) override
    {
        if (motionType == POINT_MOTION_TYPE)
            if (data == POINT_SHARTUUL_FIGHT)
                HandleFightSequence();
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SHARTUUL, EVENT_FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SHARTUUL_INCINERATE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_INCINERATE) == CAST_OK)
                    ResetCombatAction(action, 60000);
                break;
            case SHARTUUL_IMMOLATE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_IMMOLATE) == CAST_OK)
                    ResetCombatAction(action, 30000u);
                break;
            case SHARTUUL_SHADOW_RESONANCE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_RESONANCE) == CAST_OK)
                    ResetCombatAction(action, 15000u);
                break;
            case SHARTUUL_SHADOW_BOLT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                    ResetCombatAction(action, 3000);
                break;
        }
    }
};

struct PossessionTransfer : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_2)
            return;
        if (!apply)
        {
            Unit* caster = aura->GetTarget();
            Unit* target = caster->GetChannelObject();
            if (!target)
                return;
            Unit* charmer = target->GetCharmer();
            if (!charmer || !caster || caster == target)
                return;
            uint32 removalSpell;
            uint32 newSpell;
            uint32 eventId;
            if (aura->GetSpellProto()->Id == SPELL_POSSESSION_TRANSFER_P1)
            {
                removalSpell = SPELL_LEGION_RING_CHARM_NORTH_01;
                newSpell = SPELL_CHARM_P2;
                eventId = EVENT_DOOMGUARD_PUNISHER_DEATH;
            }
            else
            {
                removalSpell = SPELL_CHARM_P2;
                newSpell = SPELL_CHARM_P3;
                eventId = EVENT_SHIVAN_ASSASSIN_DEATH;
            }

            charmer->RemoveAurasDueToSpell(removalSpell);
            target->RemoveAurasDueToSpell(removalSpell);
            if (target->IsCreature())
                static_cast<Creature*>(target)->ForcedDespawn();
            charmer->CastSpell(nullptr, newSpell, TRIGGERED_OLD_TRIGGERED);
            caster->CombatStop();
            if (target->GetInstanceData())
                target->GetInstanceData()->SetData(TYPE_SHARTUUL, eventId);
        }
    }
};

bool ProcessEventId_CharmDegrader(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (pSource->IsPlayer())
    {
        Player* player = static_cast<Player*>(pSource);
        if (Unit* overseer = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(NPC_OVERSEER_SHARTUUL))
            DoScriptText(SAY_SECOND_DEMON, overseer, player);
    }
    return true;
}

bool ProcessEventId_CharmShivan(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (pSource->IsPlayer())
    {
        Player* player = static_cast<Player*>(pSource);
        if (Unit* overseer = static_cast<ScriptedInstance*>(player->GetInstanceData())->GetSingleCreatureFromStorage(NPC_OVERSEER_SHARTUUL))
            DoScriptText(SAY_THIRD_DEMON, overseer, player);
    }
    return true;
}

struct TouchOfMadness : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_MADNESS_RIFT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct MadnessRift : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        target->CastSpell(nullptr, SPELL_LEGION_RING_EYE_STALK_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        static_cast<Creature*>(target)->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
    }
};

struct EredarPreGateBeam : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->ForcePeriodicity(3000);
    }

    void OnPeriodicTickEnd(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_ARCANE_EXPLOSION, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ThrowAxe : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (!spell->GetUnitTarget() || effIdx != EFFECT_INDEX_1)
            return;
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), spell->GetDamage(), TRIGGERED_OLD_TRIGGERED);
    }
};

struct SuperJump : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, spell->m_spellInfo->EffectTriggerSpell[EFFECT_INDEX_2], TRIGGERED_OLD_TRIGGERED);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1 && spell->GetUnitTarget())
            spell->GetUnitTarget()->CastSpell(spell->GetCaster(), spell->GetDamage(), TRIGGERED_OLD_TRIGGERED);
    }
};

struct CleansingFlame : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->RemoveAurasDueToSpell(SPELL_LACERATING_BITE);
    }
};

struct RampagingCharge : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_LACERATING_BITE, TRIGGERED_OLD_TRIGGERED);
    }
};

enum
{
    ITEM_CHARGED_CRYSTAL_FOCUS = 32578,
    SPELL_CRYSTAL_CHARGE_VACUUM = 40660,

    SPELL_CHAOS_STRIKE          = 40741,
    SPELL_CHAOS_STRIKE_TRIGGER  = 41964,
    SPELL_CHAOS_FORM            = 42006,
    SPELL_FLAME_BUFFET          = 41596,
    SPELL_CORRUPTED_LIGHT       = 41965,

    SPELL_ABSORB_LIFE = 40501,

    SPELL_FLYING_ATTACK_AURA = 40557,
    SPELL_KNOCKBACK_IMMUNE_FLY = 40795,
    SPELL_BOMBING_RUN   = 41076,
};

struct CheckCrystalFocusSpell : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (Unit* master = spell->GetCaster()->GetMaster())
            if (master->IsPlayer())
                if (static_cast<Player*>(master)->HasItemCount(ITEM_CHARGED_CRYSTAL_FOCUS, spell->m_spellInfo->Id == SPELL_CHAOS_STRIKE ? 2 : 1))
                    return SPELL_CAST_OK;
        return SPELL_FAILED_REAGENTS;
    }
};

struct AbsorbLife : public CheckCrystalFocusSpell
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_CRYSTAL_CHARGE_VACUUM, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(nullptr, SPELL_ABSORB_LIFE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ShartuulDiveBomb : public CheckCrystalFocusSpell
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_CRYSTAL_CHARGE_VACUUM, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(nullptr, SPELL_FLYING_ATTACK_AURA, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FlyingAttackAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_KNOCKBACK_IMMUNE_FLY, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_KNOCKBACK_IMMUNE_FLY);
    }
};

struct ShartuulFireballBarrage : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->GetUnitTarget())
            return;

        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_BOMBING_RUN, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ChaosStrike : public CheckCrystalFocusSpell
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_CRYSTAL_CHARGE_VACUUM, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(nullptr, SPELL_CRYSTAL_CHARGE_VACUUM, TRIGGERED_OLD_TRIGGERED);

        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_CHAOS_STRIKE_TRIGGER, TRIGGERED_OLD_TRIGGERED);
    }
};

enum
{
    SPELL_SIPHON_LIFE = 41597,
    SPELL_DARK_MENDING = 41938,
};

struct ChaosStrikeDamage : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (effIdx != EFFECT_INDEX_0 || !target)
            return;

        uint32 damage = spell->GetDamage();
        if (target->isFrozen())
            damage *= 3;
        if (Aura* aura = target->GetAura(SPELL_FLAME_BUFFET, EFFECT_INDEX_1))
            damage += aura->GetAmount();
    }

    void OnAfterHit(Spell* spell) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        
        if (target->HasAura(SPELL_SIPHON_LIFE))
            spell->GetCaster()->CastSpell(nullptr, SPELL_CORRUPTED_LIGHT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ChaosStrikeTransform : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;

        spell->GetCaster()->CastSpell(nullptr, SPELL_CHAOS_FORM, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DeathBlast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1 && spell->GetUnitTarget())
        {
            if (Aura* aura = spell->GetUnitTarget()->GetAura(SPELL_SIPHON_LIFE, EFFECT_INDEX_0))
            {
                int32 amount = aura->GetAmount() * 5;
                spell->GetCaster()->CastCustomSpell(nullptr, SPELL_DARK_MENDING, &amount, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

struct Iceblast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->isFrozen())
            return;

        spell->SetDamage(spell->GetDamage() * 2);
        target->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT, spell->GetCaster()->GetObjectGuid());
    }
};

enum
{
    SPELL_ASPECT_OF_THE_FLAME   = 41593,
    SPELL_ASPECT_OF_THE_ICE     = 41594,
    SPELL_ASPECT_OF_THE_SHADOW  = 41595,
};

struct ShivanShapeshiftForm : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (apply && target->IsCreature())
        {
            Creature* demon = static_cast<Creature*>(target);
            CharmInfo* charmInfo = demon->GetCharmInfo();
            if (!charmInfo)
                return;
            uint32 spellSet;
            switch (aura->GetId())
            {
                default:
                case SPELL_ASPECT_OF_THE_FLAME:     spellSet = 1; break;
                case SPELL_ASPECT_OF_THE_ICE:       spellSet = 2; break;
                case SPELL_ASPECT_OF_THE_SHADOW:    spellSet = 0; break;
            }
            demon->SetSpellList(spellSet);
            charmInfo->InitPossessCreateSpells();
            if (Player* player = dynamic_cast<Player*>(demon->GetCharmer()))
                player->PossessSpellInitialize();
        }
    }
};

struct BuildPortableFelCannon : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            Unit* target = aura->GetTarget();
            target->CastSpell(nullptr, SPELL_LEGION_RING_FEL_CANNON_TRANSFORM, TRIGGERED_OLD_TRIGGERED);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            static_cast<Creature*>(target)->SetFactionTemporary(SHARTUUL_FACTION_HOSTILE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_REACH_HOME);
        }
    }
};

enum
{
    SPELL_SUMMON_STUN_ZONE_GRAPHIC_NS = 40776,
    SPELL_SUMMON_STUN_ZONE_GRAPHIC_WE = 40783,

    SPELL_STUN_ROPE_ATTUNEMENT      = 40777,
    SPELL_STUN_ROPE                 = 40778,
    SPELL_STUN_ZONE                 = 40775,
};

struct StunField : public ScriptedAI
{
    StunField(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }

    void JustRespawned() override
    {
        m_creature->CastSpell(nullptr, SPELL_SUMMON_STUN_ZONE_GRAPHIC_NS, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_SUMMON_STUN_ZONE_GRAPHIC_WE, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        m_creature->CastSpell(summoned, SPELL_STUN_ROPE_ATTUNEMENT, TRIGGERED_OLD_TRIGGERED);
    }
};

struct StunRopeAttunement : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(spell->GetCaster(), SPELL_STUN_ROPE, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->AI()->DoCastSpellIfCan(nullptr, SPELL_STUN_ZONE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }
};

void AddSC_shartuul_transporter()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_doomguard_punisher";
    pNewScript->GetAI = &GetNewAIInstance<npc_doomguard_punisherAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shivan_assassin";
    pNewScript->GetAI = &GetNewAIInstance<npc_shivan_assassinAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eye_of_shartuul";
    pNewScript->GetAI = &GetNewAIInstance<npc_eye_of_shartuulAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dreadmaw";
    pNewScript->GetAI = &GetNewAIInstance<npc_dreadmawAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shartuul";
    pNewScript->GetAI = &GetNewAIInstance<npc_shartuulAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_doomguard_punisher";
    pNewScript->pProcessEventId = ProcessEventId_CharmDegrader;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_shivan_assassin";
    pNewScript->pProcessEventId = ProcessEventId_CharmShivan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stun_field";
    pNewScript->GetAI = &GetNewAIInstance<StunField>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PossessionTransfer>("spell_possession_transfer");
    RegisterSpellScript<TouchOfMadness>("spell_touch_of_madness");
    RegisterSpellScript<MadnessRift>("spell_madness_rift");
    RegisterSpellScript<EredarPreGateBeam>("spell_eredar_pre_gate_beam");
    RegisterSpellScript<ThrowAxe>("spell_throw_axe");
    RegisterSpellScript<SuperJump>("spell_super_jump");
    RegisterSpellScript<CleansingFlame>("spell_cleansing_flame");
    RegisterSpellScript<RampagingCharge>("spell_rampaging_charge");
    RegisterSpellScript<DeathBlast>("spell_death_blast");
    RegisterSpellScript<Iceblast>("spell_iceblast");
    RegisterSpellScript<AbsorbLife>("spell_absorb_life");
    RegisterSpellScript<ShartuulDiveBomb>("spell_shartuul_dive_bomb");
    RegisterSpellScript<ChaosStrike>("spell_chaos_strike");
    RegisterSpellScript<FlyingAttackAura>("spell_flying_attack_aura");
    RegisterSpellScript<ShartuulFireballBarrage>("spell_fireball_barrage");
    RegisterSpellScript<ChaosStrikeTransform>("spell_chaos_strike_transform");
    RegisterSpellScript<ShivanShapeshiftForm>("spell_shivan_shapeshift_form");
    RegisterSpellScript<BuildPortableFelCannon>("spell_build_portable_fel_cannon");
    RegisterSpellScript<StunRopeAttunement>("spell_stun_rope_attunement");
}