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

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
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
    THADDIUS_POLARITY_SHIFT,
    THADDIUS_CHAIN_LIGHTNING,
    THADDIUS_BALL_LIGHTNING,
    THADDIUS_BERSERK,
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
        AddCombatAction(THADDIUS_POLARITY_SHIFT, 15s);
        AddCombatAction(THADDIUS_CHAIN_LIGHTNING, 8s);
        AddCombatAction(THADDIUS_BALL_LIGHTNING, true);
        AddCombatAction(THADDIUS_BERSERK, 6min);
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

    void Aggro(Unit* /*pWho*/) override
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
            Creature* pFeugen  = m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            Creature* pStalagg = m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);
            if (pFeugen)
            {
                pFeugen->ForcedDespawn();
                pFeugen->Respawn();
            }
            if (pStalagg)
            {
                pStalagg->ForcedDespawn();
                pStalagg->Respawn();
            }
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        BossAI::JustDied();

        if (m_instance)
        {
            // Force Despawn of Adds
            Creature* pFeugen  = m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            Creature* pStalagg = m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);

            if (pFeugen)
                pFeugen->ForcedDespawn();
            if (pStalagg)
                pStalagg->ForcedDespawn();
        }
        DoCastSpellIfCan(nullptr, SPELL_CLEAR_CHARGES);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case THADDIUS_POLARITY_SHIFT: return 30s;
            case THADDIUS_CHAIN_LIGHTNING: return 15s;
            case THADDIUS_BALL_LIGHTNING: return 1s;
            case THADDIUS_BERSERK: return 10min;
            default: return 0s;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (!m_instance)
        {
            DisableCombatAction(action);
            return;
        }
        switch (action)
        {
            case THADDIUS_CHAIN_LIGHTNING:
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget && DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                    break;
                return;
            }
            case THADDIUS_POLARITY_SHIFT:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_POLARITY_SHIFT, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoBroadcastText(SAY_ELECT, m_creature);
                    DoBroadcastText(EMOTE_POLARITY_SHIFT, m_creature);
                    break;
                }
                return;
            }
            case THADDIUS_BALL_LIGHTNING:
            {
                if (!m_creature->SelectAttackingTarget(ATTACKING_TARGET_NEAREST_BY, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_IN_MELEE_RANGE | SELECT_FLAG_NOT_IMMUNE))
                {
                    DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BALL_LIGHTNING);
                }
                break;
            }
            case THADDIUS_BERSERK:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BESERK) == CAST_OK)
                    break;
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

bool EffectDummyNPC_spell_thaddius_encounter(Unit* /*pCaster*/, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    switch (uiSpellId)
    {
        case SPELL_SHOCK_OVERLOAD:
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                // Only do something to Thaddius, and on the first hit.
                if (pCreatureTarget->GetEntry() != NPC_THADDIUS || !pCreatureTarget->HasAura(SPELL_THADIUS_SPAWN))
                    return true;
                // remove Stun and then Cast
                pCreatureTarget->RemoveAurasDueToSpell(SPELL_THADIUS_SPAWN);
                pCreatureTarget->CastSpell(pCreatureTarget, SPELL_THADIUS_LIGHTNING_VISUAL, TRIGGERED_NONE);
            }
            return true;
        case SPELL_THADIUS_LIGHTNING_VISUAL:
            if (uiEffIndex == EFFECT_INDEX_0 && pCreatureTarget->GetEntry() == NPC_THADDIUS)
            {
                pCreatureTarget->AI()->SetCombatScriptStatus(false);
                pCreatureTarget->SetInCombatWithZone(false);
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

struct npc_tesla_coilAI : public Scripted_NoMovementAI
{
    npc_tesla_coilAI(Creature* creature) : Scripted_NoMovementAI(creature),
    m_instance(dynamic_cast<instance_naxxramas*>(creature->GetInstanceData()))
    {
        SetRootSelf(true);
        Reset();
    }

    instance_naxxramas* m_instance;
    bool m_bToFeugen = false;

    void Reset() override {
        m_creature->SetImmuneToPlayer(true);
        EstablishTarget();
        if (!m_instance || m_instance->GetData(TYPE_THADDIUS) == DONE)
            return;
        DoCastSpellIfCan(m_creature, m_bToFeugen ? SPELL_FEUGEN_TESLA_PASSIVE : SPELL_STALAGG_TESLA_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void EstablishTarget()
    {
        if (m_creature->GetPositionX() > 3500.f)
            m_bToFeugen = true;
    }

    void MoveInLineOfSight(Unit* /*pWho*/) override {}

    void Aggro(Unit* /*pWho*/) override
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
    THADDIUS_ADD_POWER_SURGE,
    THADDIUS_ADD_STATIC_FIELD,
    THADDIUS_ADD_MAGNETIC_PULL,
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
        AddCustomAction(THADDIUS_ADD_REVIVE, true, [&](){
            if(!GetOtherAdd())
                return;
            if (auto* otherAI = dynamic_cast<boss_thaddiusAddsAI*>(GetOtherAdd()->AI()))
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
    // For Stalagg returns pFeugen, for Feugen returns pStalagg
    {
        switch (m_creature->GetEntry())
        {
            case NPC_FEUGEN:  return m_instance->GetSingleCreatureFromStorage(NPC_STALAGG);
            case NPC_STALAGG: return m_instance->GetSingleCreatureFromStorage(NPC_FEUGEN);
            default:
                return nullptr;
        }
    }

    void Aggro(Unit* pWho) override
    {
        BossAI::Aggro(pWho);

        if (Creature* pOtherAdd = GetOtherAdd())
        {
            if (!pOtherAdd->IsInCombat())
                pOtherAdd->AI()->AttackStart(pWho);
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

        if (Creature* pOther = GetOtherAdd())
        {
            if (boss_thaddiusAddsAI* pOtherAI = dynamic_cast<boss_thaddiusAddsAI*>(pOther->AI()))
            {
                if (pOtherAI->IsCountingDead())
                {
                    pOther->ForcedDespawn();
                    pOther->Respawn();
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
        SetRootSelf(true);
        AddCustomAction(THADDIUS_ADD_HOLD, 1s + 500ms, [&](){
            SetRootSelf(false);
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
        ResetTimer(THADDIUS_ADD_REVIVE, 5s);
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
        AddCombatAction(THADDIUS_ADD_POWER_SURGE, RandomTimer(10s, 15s));
        Reset();
    }

    void Reset() override
    {
        boss_thaddiusAddsAI::Reset();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoBroadcastText(SAY_STAL_DEATH, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case THADDIUS_ADD_POWER_SURGE:
            {
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_POWERSURGE : SPELL_POWERSURGE_H) == CAST_OK)
                    ResetCombatAction(action, RandomTimer(10s, 15s));
                return;
            }
            default:
                DisableCombatAction(action);
        }
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
        AddCombatAction(THADDIUS_ADD_STATIC_FIELD, 10s, 15s);
        AddCombatAction(THADDIUS_ADD_MAGNETIC_PULL, 20s);
        Reset();
    }

    void Reset() override
    {
        boss_thaddiusAddsAI::Reset();
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoBroadcastText(SAY_FEUG_DEATH, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case THADDIUS_ADD_STATIC_FIELD:
            {
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_STATIC_FIELD : SPELL_STATIC_FIELD_H) == CAST_OK)
                    ResetCombatAction(action, RandomTimer(10s, 15s));
                return;
            }
            case THADDIUS_ADD_MAGNETIC_PULL:
            {
                if (DoCastSpellIfCan(GetOtherAdd(), SPELL_MAGNETIC_PULL_B, TRIGGERED_OLD_TRIGGERED))
                    ResetCombatAction(action, 20s);
                return;
            }
            default:
                DisableCombatAction(action);
        }
    }
};

struct MagneticPull : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo /*missInfo*/) const override
    {
        Unit* pFeugen;
        Unit* pStalagg;
        pFeugen = spell->GetCaster();
        pStalagg = spell->GetUnitTarget();

        if (!pFeugen || !pStalagg)
            return;
        
        if (pFeugen->GetEntry() != NPC_FEUGEN || pStalagg->GetEntry() != NPC_STALAGG)
            return;

        auto* pFeugenTank = pFeugen->getThreatManager().getCurrentVictim();
        auto* pStalaggTank = pStalagg->getThreatManager().getCurrentVictim();

        if (!pFeugenTank || !pStalaggTank)
            return;
        
        float feugenThreat, stalaggThreat;

        feugenThreat = pFeugenTank->getThreat();
        stalaggThreat = pStalaggTank->getThreat();

        pFeugenTank->addThreatPercent(-100);
        pStalaggTank->addThreatPercent(-100);
        pFeugen->AddThreat(pStalaggTank->getTarget(), stalaggThreat);
        pStalagg->AddThreat(pFeugenTank->getTarget(), feugenThreat);

        pStalagg->CastSpell(pFeugenTank->getTarget(), SPELL_MAGNETIC_PULL_EFFECT, TRIGGERED_OLD_TRIGGERED);
        pFeugen->CastSpell(pStalaggTank->getTarget(), SPELL_MAGNETIC_PULL_EFFECT, TRIGGERED_OLD_TRIGGERED);
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
    pNewScript->pEffectDummyNPC = &EffectDummyNPC_spell_thaddius_encounter;
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
    pNewScript->pEffectDummyNPC = &EffectDummyNPC_spell_thaddius_encounter;
    pNewScript->RegisterSelf();

    RegisterSpellScript<MagneticPull>("spell_magnetic_pull");
    RegisterSpellScript<PolarityShift>("spell_thaddius_polarity_shift");
    RegisterSpellScript<ThaddiusChargeDamage>("spell_thaddius_charge_damage");
    RegisterSpellScript<ThaddiusCharge>("spell_thaddius_charge_buff");
    RegisterSpellScript<TriggerTeslas>("spell_trigger_teslas");
    RegisterSpellScript<ThaddiusTeslaEffect>("spell_thaddius_tesla_effect");
    RegisterSpellScript<ThaddiusTeslaChain>("spell_thaddius_tesla_chain");
}
