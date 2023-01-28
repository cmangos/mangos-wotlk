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
SDName: Boss_Thaddius
SD%Complete: 85
SDComment: Magnetic Pull, Tesla-Chains
SDCategory: Naxxramas
EndScriptData */

/* ContentData
boss_thaddius
npc_tesla_coil
boss_stalagg
boss_feugen
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "naxxramas.h"

enum
{
    // Stalagg
    SAY_STAL_AGGRO                  = 13083,
    SAY_STAL_SLAY                   = 13085,
    SAY_STAL_DEATH                  = 12195,

    // Feugen
    SAY_FEUG_AGGRO                  = 13023,
    SAY_FEUG_SLAY                   = 13025,
    SAY_FEUG_DEATH                  = 12195,

    // Tesla Coils
    EMOTE_LOSING_LINK               = 12156,
    EMOTE_TESLA_OVERLOAD            = 12178,

    // Thaddius
    SAY_AGGRO_1                     = 13086,
    SAY_AGGRO_2                     = 13087,
    SAY_AGGRO_3                     = 13088,
    SAY_SLAY                        = 13096,
    SAY_ELECT                       = 13090,
    SAY_DEATH                       = 13089,
    EMOTE_POLARITY_SHIFT            = 32324,

    // Thaddius Spells
    SPELL_THADIUS_SPAWN             = 28160,
    SPELL_THADIUS_LIGHTNING_VISUAL  = 28136,
    SPELL_BALL_LIGHTNING            = 28299,
    SPELL_CHAIN_LIGHTNING           = 28167,
    SPELL_CHAIN_LIGHTNING_H         = 54531,
    SPELL_POLARITY_SHIFT            = 28089,
    SPELL_POSITIVE_CHARGE           = 28059,
    SPELL_NEGATIVE_CHARGE           = 28084,
    SPELL_POSITIVE_CHARGE_BUFF      = 29659,
    SPELL_NEGATIVE_CHARGE_BUFF      = 29660,
    SPELL_POSITIVE_CHARGE_DAMAGE    = 28062,
    SPELL_NEGATIVE_CHARGE_DAMAGE    = 28085,
    SPELL_BESERK                    = 27680,
    SPELL_CLEAR_CHARGES             = 63133,                // TODO NYI, cast on death, most likely to remove remaining buffs

    // Stalagg & Feugen Spells
    SPELL_MAGNETIC_PULL_A           = 28338,
    SPELL_MAGNETIC_PULL_B           = 54517,                // used by Feugen (wotlk)
    SPELL_MAGNETIC_PULL_EFFECT      = 30010,
    SPELL_STATIC_FIELD              = 28135,
    SPELL_STATIC_FIELD_H            = 54528,
    SPELL_POWERSURGE_H              = 28134,
    SPELL_POWERSURGE                = 54529,

    // Tesla Spells
    SPELL_FEUGEN_CHAIN              = 28111,
    SPELL_STALAGG_CHAIN             = 28096,
    SPELL_FEUGEN_TESLA_PASSIVE      = 28109,
    SPELL_STALAGG_TESLA_PASSIVE     = 28097,
    SPELL_FEUGEN_TESLA_EFFECT       = 28110,
    SPELL_STALAGG_TESLA_EFFECT      = 28098,
    SPELL_SHOCK_OVERLOAD            = 28159,
    SPELL_SHOCK                     = 28099,
    SPELL_TRIGGER_TESLAS            = 28359,
};

/************
** boss_thaddius
************/

enum ThaddiusActions
{
    THADDIUS_BALL_LIGHTNING,
    THADDIUS_ACTIONS_MAX,
};

struct boss_thaddiusAI : public BossAI
{
    boss_thaddiusAI(Creature* creature) : BossAI(creature, THADDIUS_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_THADDIUS);
        AddOnKillText(SAY_SLAY);
        AddOnDeathText(SAY_DEATH);
        AddOnAggroText(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3);
        AddCombatAction(THADDIUS_BALL_LIGHTNING, 1s);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    void Reset() override
    {
        SetCombatScriptStatus(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
        DoCastSpellIfCan(m_creature, SPELL_THADIUS_SPAWN);
        BossAI::Reset();
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        // Make Attackable
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
    }

    void JustReachedHome() override
    {
        if (m_instance)
        {
            m_instance->SetData(TYPE_THADDIUS, FAIL);

            // Respawn Adds:
            Creature* feugen  = m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            Creature* stalagg = m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);
            if (feugen)
            {
                feugen->SetRespawnDelay(0, true);
                feugen->ForcedDespawn();
            }
            if (stalagg)
            {
                stalagg->SetRespawnDelay(0, true);
                stalagg->ForcedDespawn();
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        BossAI::JustDied();

        if (m_instance)
        {
            // Force Despawn of Adds
            Creature* feugen  = m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            Creature* stalagg = m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);

            if (feugen)
                feugen->ForcedDespawn();
            if (stalagg)
                stalagg->ForcedDespawn();
        }
        DoCastSpellIfCan(nullptr, SPELL_CLEAR_CHARGES);
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_POLARITY_SHIFT)
        {
            DoBroadcastText(SAY_ELECT, m_creature);
            DoBroadcastText(EMOTE_POLARITY_SHIFT, m_creature);
        }
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case THADDIUS_BALL_LIGHTNING: return 1s;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case THADDIUS_BALL_LIGHTNING: // TODO: Move to spell lists once UnitCondition is added
            {
                if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE | SELECT_FLAG_NOT_IMMUNE))
                {
                    DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BALL_LIGHTNING);
                }
                break;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

struct ShockThaddius : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            Unit* target = spell->GetUnitTarget();
            // Only do something to Thaddius, and on the first hit.
            if (target->GetEntry() != NPC_THADDIUS || !target->HasAura(SPELL_THADIUS_SPAWN))
                return;
            // remove Stun and then Cast
            target->RemoveAurasDueToSpell(SPELL_THADIUS_SPAWN);
            target->CastSpell(nullptr, SPELL_THADIUS_LIGHTNING_VISUAL, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct ThaddiusLightningVisual : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (effIdx == EFFECT_INDEX_0 && target->IsCreature())
        {
            target->AI()->SetCombatScriptStatus(false);
            static_cast<Creature*>(target)->SetInCombatWithZone(false);
        }
    }
};

bool EffectDummyNPC_spell_thaddius_encounter(Unit* /*pCaster*/, uint32 spellId, SpellEffectIndex uiEffIndex, Creature* creatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    switch (spellId)
    {
        case SPELL_SHOCK_OVERLOAD:
            if (uiEffIndex == EFFECT_INDEX_0)
            {

            }
            return true;
        case SPELL_THADIUS_LIGHTNING_VISUAL:
            if (uiEffIndex == EFFECT_INDEX_0 && creatureTarget->GetEntry() == NPC_THADDIUS)
            {

            }
            return true;
    }
    return false;
}

/************
** npc_tesla_coil
************/

enum TeslaCoilActions
{
    TESLA_COIL_SETUP_CHAIN,
};

struct npc_tesla_coilAI : public ScriptedAI
{
    npc_tesla_coilAI(Creature* creature) : ScriptedAI(creature),
        m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData()))
    {
        SetAIImmobilizedState(true);
        SetCombatMovement(false);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_toFeugen = false;

    void Reset() override
    {
        m_creature->SetImmuneToPlayer(true);
        EstablishTarget();
        if (!m_instance || m_instance->GetData(TYPE_THADDIUS) == DONE)
            return;
        DoCastSpellIfCan(m_creature, m_toFeugen ? SPELL_FEUGEN_TESLA_PASSIVE : SPELL_STALAGG_TESLA_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void EstablishTarget()
    {
        if (m_creature->GetPositionX() > 3500.f)
            m_toFeugen = true;
    }

    void MoveInLineOfSight(Unit* /*who*/) override {}

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(EMOTE_LOSING_LINK, m_creature);
    }

    // Overwrite this function here to
    // * Keep Chain spells casted when evading after useless EnterCombat caused by 'buffing' the add
    // * To not remove the Passive spells when evading after ie killed a player
    void EnterEvadeMode() override
    {
        m_creature->CombatStop();
    }
};

/************
** boss_thaddiusAddsAI - Superclass for Feugen & Stalagg
************/

enum ThaddiusAddActions
{
    THADDIUS_ADD_ACTIONS_MAX,
    THADDIUS_ADD_REVIVE,
    THADDIUS_ADD_SHOCK_OVERLOAD,
    THADDIUS_ADD_HOLD,
};

struct boss_thaddiusAddsAI : public BossAI
{
    boss_thaddiusAddsAI(Creature* creature) : BossAI(creature, THADDIUS_ADD_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_THADDIUS);
        AddCustomAction(THADDIUS_ADD_REVIVE, true, [&]()
        {
            if (!GetOtherAdd())
                return;
            if (boss_thaddiusAddsAI* otherAI = dynamic_cast<boss_thaddiusAddsAI*>(GetOtherAdd()->AI()))
            {
                if (otherAI->IsCountingDead())
                {
                    otherAI->DisableTimer(THADDIUS_ADD_REVIVE);
                    AddCustomAction(THADDIUS_ADD_SHOCK_OVERLOAD, 14s, [&](){
                        DoCastSpellIfCan(m_creature, SPELL_TRIGGER_TESLAS, TRIGGERED_OLD_TRIGGERED);
                        DisableTimer(THADDIUS_ADD_SHOCK_OVERLOAD);
                    });
                    DisableTimer(THADDIUS_ADD_REVIVE);
                    return;
                }
                Revive();
                m_isFakingDeath = false;
                DisableTimer(THADDIUS_ADD_REVIVE);
            }
        });
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_isRegularMode;

    bool m_isFakingDeath;
    bool m_areBothDead;

    void Reset() override
    {
        m_isFakingDeath = false;
        m_areBothDead = false;

        // We might Reset while faking death, so undo this
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        SetDeathPrevention(true);
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        BossAI::Reset();
    }

    Creature* GetOtherAdd() const
    // For Stalagg returns feugen, for Feugen returns stalagg
    {
        switch (m_creature->GetEntry())
        {
            case NPC_FEUGEN:  return m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);
            case NPC_STALAGG: return m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            default:
                return nullptr;
        }
    }

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);

        if (Creature* pOtherAdd = GetOtherAdd())
        {
            if (!pOtherAdd->IsInCombat())
                pOtherAdd->AI()->AttackStart(who);
        }
    }

    void JustRespawned() override
    {
        Reset();                                            // Needed to reset the flags properly
        JustReachedHome();
    }

    void JustReachedHome() override
    {
        if (!m_instance)
            return;

        if (Creature* other = GetOtherAdd())
        {
            if (boss_thaddiusAddsAI* pOtherAI = dynamic_cast<boss_thaddiusAddsAI*>(other->AI()))
            {
                if (pOtherAI->IsCountingDead())
                {
                    other->ForcedDespawn();
                    other->Respawn();
                }
            }
        }

        if (Creature* tesla = GetClosestCreatureWithEntry(m_creature, NPC_TESLA_COIL, 50.f))
        {
            tesla->ForcedDespawn();
            tesla->Respawn();
        }

        m_instance->SetData(TYPE_THADDIUS, FAIL);
    }

    void Revive()
    {
        SetCombatScriptStatus(false);
        DoResetThreat();
        PauseCombatMovement();
        Reset();
    }

    bool IsCountingDead() const
    {
        return m_isFakingDeath || m_creature->IsDead();
    }

    void PauseCombatMovement()
    {
        SetAIImmobilizedState(true);
        AddCustomAction(THADDIUS_ADD_HOLD, 1s + 500ms, [&](){
            SetAIImmobilizedState(false);
            m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
            DisableTimer(THADDIUS_ADD_HOLD);
        });
        ResetIfNotStarted(THADDIUS_ADD_HOLD, 1s + 500ms);
    }

    void JustPreventedDeath(Unit* attacker) override
    {
        if (!m_instance)
            return;
        m_isFakingDeath = true;

        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(1);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->ClearAllReactives();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        SetCombatScriptStatus(true);

        JustDied(attacker);                                  // Texts
        ResetTimer(THADDIUS_ADD_REVIVE, 10s);
    }
};

/************
** boss_stalagg
************/

struct boss_stalaggAI : public boss_thaddiusAddsAI
{
    boss_stalaggAI(Creature* creature) : boss_thaddiusAddsAI(creature)
    {
        AddOnKillText(SAY_STAL_SLAY);
        AddOnAggroText(SAY_STAL_AGGRO);
        Reset();
    }

    void Reset() override
    {
        boss_thaddiusAddsAI::Reset();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_STAL_DEATH, m_creature);
    }
};

/************
** boss_feugen
************/

struct boss_feugenAI : public boss_thaddiusAddsAI
{
    boss_feugenAI(Creature* creature) : boss_thaddiusAddsAI(creature)
    {
        AddOnKillText(SAY_FEUG_SLAY);
        AddOnAggroText(SAY_FEUG_AGGRO);
        Reset();
    }

    void Reset() override
    {
        boss_thaddiusAddsAI::Reset();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_FEUG_DEATH, m_creature);
    }
};

struct MagneticPullThaddius : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* feugen;
        Unit* stalagg;
        feugen = spell->GetCaster();
        stalagg = spell->GetUnitTarget();

        if (!feugen || !stalagg)
            return;
        
        if (feugen->GetEntry() != NPC_FEUGEN || stalagg->GetEntry() != NPC_STALAGG)
            return;

        auto* feugenTank = feugen->getThreatManager().getCurrentVictim();
        auto* stalaggTank = stalagg->getThreatManager().getCurrentVictim();

        if (!feugenTank || !stalaggTank)
            return;
        
        float feugenThreat, stalaggThreat;

        feugenThreat = feugenTank->getThreat();
        stalaggThreat = stalaggTank->getThreat();

        feugenTank->addThreatPercent(-100);
        stalaggTank->addThreatPercent(-100);
        feugen->AddThreat(stalaggTank->getTarget(), stalaggThreat);
        stalagg->AddThreat(feugenTank->getTarget(), feugenThreat);

        stalagg->CastSpell(feugenTank->getTarget(), SPELL_MAGNETIC_PULL_EFFECT, TRIGGERED_OLD_TRIGGERED);
        feugen->CastSpell(stalaggTank->getTarget(), SPELL_MAGNETIC_PULL_EFFECT, TRIGGERED_OLD_TRIGGERED);
    }
};

/****************
** Polarity Shift
****************/

struct PolarityShift : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx ) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            if (Unit* unitTarget = spell->GetUnitTarget())
            {
                unitTarget->RemoveAurasDueToSpell(SPELL_POSITIVE_CHARGE);
                unitTarget->RemoveAurasDueToSpell(SPELL_NEGATIVE_CHARGE);

                uint64 scriptValue = spell->GetScriptValue();

                // 28059 : Positive Charge, 28084 : Negative Charge
                switch (scriptValue)
                {
                    case 0: // first target random
                        scriptValue = urand(0, 1) ? SPELL_POSITIVE_CHARGE : SPELL_NEGATIVE_CHARGE;
                        spell->SetScriptValue(scriptValue);
                        unitTarget->CastSpell(unitTarget, scriptValue, TRIGGERED_INSTANT_CAST);
                        break;
                    case SPELL_POSITIVE_CHARGE: // second target the other
                        spell->SetScriptValue(1);
                        unitTarget->CastSpell(unitTarget, SPELL_NEGATIVE_CHARGE, TRIGGERED_INSTANT_CAST);
                        break;
                    case SPELL_NEGATIVE_CHARGE:
                        spell->SetScriptValue(1);
                        unitTarget->CastSpell(unitTarget, SPELL_POSITIVE_CHARGE, TRIGGERED_INSTANT_CAST);
                        break;
                    default: // third and later random
                        unitTarget->CastSpell(unitTarget, urand(0, 1) ? SPELL_POSITIVE_CHARGE : SPELL_NEGATIVE_CHARGE, TRIGGERED_INSTANT_CAST);
                        break;
                }
            }
        }
    }
};

struct ThaddiusChargeDamage : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            switch (spell->m_spellInfo->Id)
            {
                case SPELL_POSITIVE_CHARGE_DAMAGE:                // Positive Charge
                    if (target->HasAura(SPELL_POSITIVE_CHARGE))   // Only deal damage to targets without Positive Charge
                        return false;
                    break;
                case SPELL_NEGATIVE_CHARGE_DAMAGE:                // Negative Charge
                    if (target->HasAura(SPELL_NEGATIVE_CHARGE))   // Only deal damage to targets without Negative Charge
                        return false;
                    break;
                default:
                    break;
            }
        }
        if (auto* instance = dynamic_cast<instance_naxxramas*>(target->GetInstanceData()))
            instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SHOCKING, false);
        return true;
    }
};

struct ThaddiusCharge : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            Unit* target = aura->GetTarget();
            if (!target)
                return;

            // On Polarity Shift, remove the previous damage buffs
            uint32 buffAuraIds[2] = { SPELL_POSITIVE_CHARGE_BUFF , SPELL_NEGATIVE_CHARGE_BUFF };
            for (auto buffAura: buffAuraIds)
                target->RemoveAurasDueToSpell(buffAura);
        }
    }

    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (Unit* target = aura->GetTarget())
        {
            uint32 buffAuraId = aura->GetId() == SPELL_POSITIVE_CHARGE ? SPELL_POSITIVE_CHARGE_BUFF : SPELL_NEGATIVE_CHARGE_BUFF;
            float range = 13.f; // Static value from DBC files. As the value is the same for both spells we can hardcode it instead of accessing is through sSpellRadiusStore

            if (!aura->GetCaster()->IsAlive())
            {
                target->RemoveAurasDueToSpell(buffAuraId);
                return;
            }

            uint32 curCount = 0;
            PlayerList playerList;
            GetPlayerListWithEntryInWorld(playerList, target, range);
            for (Player* player : playerList)
                if (target != player && player->HasAura(aura->GetId()))
                    ++curCount;

            // Remove previous buffs in case we have less targets of the same charge near use than in previous tick
            target->RemoveAurasDueToSpell(buffAuraId);
                for (uint32 i = 0; i < curCount; i++)
                    target->CastSpell(target, buffAuraId, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct TriggerTeslas : SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (target->GetTypeId() == TYPEID_UNIT && target->GetEntry() == NPC_TESLA_COIL)
            return true;
        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx ) const override
    {
        if (Unit* target = spell->GetUnitTarget())
        {
            DoBroadcastText(EMOTE_TESLA_OVERLOAD, target);
            target->RemoveAllAuras();
            target->CastSpell(target, SPELL_SHOCK_OVERLOAD, TRIGGERED_NONE);
        }
        return;
    }
};

struct ThaddiusTeslaEffect : SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex effIdx) const override
    {
        if (!target)
            return false;
        if (target->GetTypeId() == TYPEID_UNIT)
        {
            if (spell->m_spellInfo->Id == SPELL_FEUGEN_TESLA_EFFECT && target->GetEntry() == NPC_FEUGEN)
                return true;
            if (spell->m_spellInfo->Id == SPELL_STALAGG_TESLA_EFFECT && target->GetEntry() == NPC_STALAGG)
                return true;
        }
        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx ) const override
    {
        if (Unit* target = spell->GetUnitTarget())
        {
            if (Unit* caster = spell->GetCaster())
            {
                if (target->GetTypeId() == TYPEID_UNIT && target->IsAlive())
                {
                    uint32 chainSpellId = spell->m_spellInfo->Id == SPELL_STALAGG_TESLA_EFFECT ? SPELL_STALAGG_CHAIN : SPELL_FEUGEN_CHAIN;

                    if (!target->HasAura(chainSpellId) && caster->IsWithinDistInMap(target, 60.0f))
                    {
                        if (!caster->IsImmuneToPlayer())
                            caster->SetImmuneToPlayer(true);
                        caster->CastSpell(target, chainSpellId, TRIGGERED_OLD_TRIGGERED);
                        caster->CombatStop(true);
                    }
                    else if (!caster->IsWithinDistInMap(target, 60.0f))
                    {
                        target->RemoveAurasDueToSpell(chainSpellId);
                        caster->SetImmuneToPlayer(false);
                        static_cast<Creature*>(caster)->SetInCombatWithZone(false);
                        
                        if (Unit* teslaTarget = (static_cast<Creature*>(caster))->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            caster->CastSpell(teslaTarget, SPELL_SHOCK, TRIGGERED_NONE);
                    }
                    else
                        caster->CombatStop(true);
                }
            }
        }
    }
};

struct ThaddiusTeslaChain : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        return;
    }
};

void AddSC_boss_thaddius()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_thaddius";
    pNewScript->GetAI = &GetNewAIInstance<boss_thaddiusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_stalagg";
    pNewScript->GetAI = &GetNewAIInstance<boss_stalaggAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_feugen";
    pNewScript->GetAI = &GetNewAIInstance<boss_feugenAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tesla_coil";
    pNewScript->GetAI = &GetNewAIInstance<npc_tesla_coilAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ShockThaddius>("spell_shock_thaddius");
    RegisterSpellScript<ThaddiusLightningVisual>("spell_thaddius_lightning_visual");
    RegisterSpellScript<MagneticPullThaddius>("spell_magnetic_pull_thaddius");
    RegisterSpellScript<PolarityShift>("spell_thaddius_polarity_shift");
    RegisterSpellScript<ThaddiusChargeDamage>("spell_thaddius_charge_damage");
    RegisterSpellScript<ThaddiusCharge>("spell_thaddius_charge_buff");
    RegisterSpellScript<TriggerTeslas>("spell_trigger_teslas");
    RegisterSpellScript<ThaddiusTeslaEffect>("spell_thaddius_tesla_effect");
    RegisterSpellScript<ThaddiusTeslaChain>("spell_thaddius_tesla_chain");
}
