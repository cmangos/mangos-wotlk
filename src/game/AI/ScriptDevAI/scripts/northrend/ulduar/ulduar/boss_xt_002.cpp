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
SDName: boss_xt_002
SD%Complete: 80%
SDComment: Timers may need adjustments; Achievements and hard mode abilities NYI.
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_AGGRO                           = 33508,
    SAY_SLAY_1                          = 33509,
    SAY_SLAY_2                          = 33510,
    SAY_BERSERK                         = 33512,
    SAY_DEATH                           = 33511,
    SAY_HEART_OPEN                      = 33446,
    SAY_HEART_CLOSE                     = 33448,
    SAY_TANCTRUM                        = 33453,

    EMOTE_HEART                         = 33447,
    EMOTE_REPAIR                        = 33856,
    EMOTE_KILL_HEART                    = 34023,
    EMOTE_EARTH_QUAKE                   = 33454,

    // spells
    SPELL_TYMPANIC_TANTRUM              = 62776,
    SPELL_SEARING_LIGHT                 = 63018,
    SPELL_SEARING_LIGHT_H               = 65121,
    SPELL_GRAVITY_BOMB                  = 63024,
    SPELL_GRAVITY_BOMB_H                = 64234,
    SPELL_BERSERK                       = 26662,
    SPELL_SUBMERGED                     = 37751,            // cast before a heart phase. not used because it's unk purpose
    SPELL_STAND                         = 37752,            // cast after a heart phase. not used because it's unk purpose
    SPELL_CUSTOM_COOLDOWN               = 64404,

    // hard mode spells
    SPELL_HEARTBREAK                    = 65737,
    SPELL_HEARTBREAK_H                  = 64193,
    SPELL_VOIDZONE                      = 64203,
    SPELL_VOIDZONE_H                    = 64235,
    SPELL_LIFE_SPARK                    = 64210,

    // heart of XT002 spells
    SPELL_HEART_RIDE_VEHICLE            = 63852,            // ride seat 0 - procs on damage (probably spell 17683)
    SPELL_FULL_HEAL                     = 17683,
    SPELL_RIDE_VEHICLE                  = 63313,            // ride seat 1
    SPELL_LIGHTNING_TETHER              = 64799,            // dummy
    SPELL_HEART_OVERLOAD                = 62789,            // triggers missing spell 62791
    SPELL_EXPOSED_HEART                 = 63849,            // procs on damage; triggers missing spell 62791
    SPELL_ENERGY_ORB                    = 62790,            // targets 33337, in order to start spawning robots
    SPELL_ENERGY_ORB_DUMMY              = 62826,            // dummy effect which spawns robots - not used due to core targeting issues

    // robot summoning spells, used by the XT toy pile
    SPELL_RECHARGE_ROBOT_1              = 62828,            // summons 33343
    SPELL_RECHARGE_ROBOT_2              = 62835,            // summons 33346
    SPELL_RECHARGE_ROBOT_3              = 62831,            // summons 33344

    // summoned spells
    SPELL_CONSUMPTION                   = 64209,            // cast by the void zone
    SPELL_CONSUMPTION_PERIODIC          = 64208,
    SPELL_ARCANE_POWER_STATE            = 49411,            // cast by the life spark
    SPELL_STATIC_CHARGED                = 64227,            // cast by the life spark (needs to be confirmed)
    SPELL_STATIC_CHARGED_H              = 64236,
    SPELL_SCRAP_REPAIR                  = 62832,            // cast on scrapbot in range to heal XT002; sends event 21606
    SPELL_RIDE_VEHICLE_SCRAPBOT         = 47020,            // cast by scrapbot on XT002 heal
    SPELL_SHOCK                         = 64230,

    // NPC ids
    NPC_SCRAPBOT                        = 33343,
    NPC_BOOMBOT                         = 33346,
    NPC_PUMMELLER                       = 33344,
    NPC_VOIDZONE                        = 34001,
    NPC_LIFE_SPARK                      = 34004,

    MAX_SCRAPBOTS                       = 5,
};

/*######
## boss_xt_002
######*/

enum XT002Actions
{
    XT002_TIRED,
    XT002_MOUNT_HEART,
    XT002_HEALTH_CHECK,
    XT002_ACTIONS_MAX,
};

struct boss_xt_002AI : public BossAI
{
    boss_xt_002AI(Creature* creature) : BossAI(creature, XT002_ACTIONS_MAX),
    m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
    m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        SetDataType(TYPE_XT002);
        AddOnAggroText(SAY_AGGRO);
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddOnDeathText(SAY_DEATH);
        AddTimerlessCombatAction(XT002_HEALTH_CHECK, true);
        AddCustomAction(XT002_MOUNT_HEART, 1s, [&]()
        {
            Creature* heart = m_instance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR);
            if (!heart)
                return;
                // safeguard in case the Heart isn't respawned
            if (!heart->IsAlive())
                heart->Respawn();

            heart->AI()->EnterEvadeMode();
            m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
            heart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            heart->CastSpell(m_creature, SPELL_HEART_RIDE_VEHICLE, TRIGGERED_OLD_TRIGGERED);
        });
        AddCustomAction(XT002_TIRED, true, [&]()
        {
            // inform the heart about the phase switch
            if (Creature* heart = m_instance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR))
                SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, heart);

            DoBroadcastText(EMOTE_HEART, m_creature);

            m_creature->CastSpell(nullptr, SPELL_SUBMERGED, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
        });
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    uint8 m_heartStage;

    void Aggro(Unit* who) override
    {
        BossAI::Aggro(who);
        if (m_instance)
            m_instance->SetData(TYPE_XT002_HARD, NOT_STARTED);
    }

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();
        if (Creature* heart = m_instance->GetSingleCreatureFromStorage(NPC_HEART_DECONSTRUCTOR))
            heart->ForcedDespawn();
    }

    void JustRespawned() override
    {
        StopTargeting(false);
        SetCombatScriptStatus(false);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_heartStage = 1;
        ResetTimer(XT002_MOUNT_HEART, 1s);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // enable hard mode
        if (eventType == AI_EVENT_CUSTOM_B && invoker->GetEntry() == NPC_HEART_DECONSTRUCTOR)
        {
            // reset to normal phase and don't allow the boss to get back to heart phases
            DoResetToNormalPhase();
            m_heartStage = 5;
            DisableCombatAction(XT002_HEALTH_CHECK);

            if (m_instance)
                m_instance->SetData(TYPE_XT002_HARD, DONE);

            DoBroadcastText(EMOTE_KILL_HEART, m_creature);
            DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_HEARTBREAK : SPELL_HEARTBREAK_H, CAST_TRIGGERED);

            // no spell used for this action
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->SetSpellList(m_creature->GetSpellList().Id + 1);
        }
    }

    // wrapper to reset to normal phase
    void DoResetToNormalPhase()
    {
        DoBroadcastText(SAY_HEART_CLOSE, m_creature);
        m_creature->CastSpell(nullptr, SPELL_STAND, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
        m_creature->CastSpell(nullptr, SPELL_CUSTOM_COOLDOWN, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
        StopTargeting(false);
        SetCombatScriptStatus(false);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case XT002_HEALTH_CHECK:
            {
                if (m_creature->GetHealthPercent() < float(100 - 25 * m_heartStage))
                {
                    DoBroadcastText(SAY_HEART_OPEN, m_creature);

                    ++m_heartStage;
                    ResetTimer(XT002_TIRED, 5s);
                    SetCombatScriptStatus(true);
                    DoStopAttack();
                    m_creature->SetTarget(nullptr);
                    StopTargeting(true);
                }
                return;
            }
        }
    }
};

/*######
## boss_heart_deconstructor
######*/

enum HeartActions
{
    HEART_ACTIONS_MAX,
};

struct boss_heart_deconstructorAI : public CombatAI
{
    boss_heart_deconstructorAI(Creature* creature) : CombatAI(creature, HEART_ACTIONS_MAX),
    m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData()))
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
    }

    instance_ulduar* m_instance;

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void JustDied(Unit* /*killer*/) override
    {
        // notify XT that hard mode is enabled
        CombatAI::JustDied(nullptr);
        if (!m_instance)
            return;
        if (Creature* pDeconstructor = m_instance->GetSingleCreatureFromStorage(NPC_XT002))
            SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pDeconstructor);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // start XT phase switch and start recharging robots
        if (eventType != AI_EVENT_CUSTOM_A || invoker->GetEntry() != NPC_XT002)
            return;
        // remove flags and previous vehicle aura before applying the new one
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        invoker->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
        m_creature->CastSpell(nullptr, SPELL_FULL_HEAL, TRIGGERED_OLD_TRIGGERED);
        DoCastSpellIfCan(invoker, SPELL_RIDE_VEHICLE, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_HEART_OVERLOAD, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_EXPOSED_HEART, CAST_TRIGGERED);
    }
};

/*######
## npc_scrapbot
######*/

struct npc_scrapbotAI : public ScriptedAI
{
    npc_scrapbotAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool m_bIsHealed;

    void Reset() override
    {
        m_bIsHealed = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_bIsHealed && who->GetEntry() == NPC_XT002 && who->IsAlive() && who->IsWithinDistInMap(m_creature, 10.0f))
        {
            DoCastSpellIfCan(who, SPELL_RIDE_VEHICLE_SCRAPBOT, CAST_TRIGGERED);
            who->CastSpell(m_creature, SPELL_SCRAP_REPAIR, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(EMOTE_REPAIR, who);
            m_creature->ForcedDespawn(4000);
            m_bIsHealed = true;
        }
    }
};

/*######
## npc_xt_toy_pile
######*/

struct npc_xt_toy_pileAI : public ScriptedAI
{
    npc_xt_toy_pileAI(Creature* creature) : ScriptedAI(creature)
    {
        SetMeleeEnabled(false);
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }
};

struct ExposedHeart : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(caster->GetInstanceData());
        if (!instance)
            return;
        Creature* xt = instance->GetSingleCreatureFromStorage(NPC_XT002);
        if (!xt || !xt->AI())
            return;
        auto* xt_ai = static_cast<boss_xt_002AI*>(xt->AI());
        xt_ai->DoResetToNormalPhase();
        if (instance->GetData(TYPE_XT002_HARD) == DONE)
            return;        
        caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        xt->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
        caster->CastSpell(xt, SPELL_HEART_RIDE_VEHICLE, TRIGGERED_OLD_TRIGGERED);
        int32 returnHealth(xt->GetHealth() - int32(caster->GetMaxHealth() - caster->GetHealth()));
        xt->SetHealth(std::max(returnHealth, 1));
        caster->RemoveAllAurasOnDeath();
    }
};

struct HeartOverload : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster || !caster->IsAlive())
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(caster->GetInstanceData());
        if (!instance)
            return;
        GuidVector toyPiles;
        instance->GetToyPileGuids(toyPiles);
        if (toyPiles.empty())
            return;
        Creature* toyPile;
#ifndef PRENERF_3_1_0
        for (int i = 0; i < toyPiles.size(); i++)
        {
#endif
            std::shuffle(toyPiles.begin(), toyPiles.end(), *GetRandomGenerator());
            toyPile = caster->GetMap()->GetCreature(toyPiles.at(0));
            if (!toyPile)
                return;
#ifndef PRENERF_3_1_0
            if (caster->GetDistance(toyPile) > 60.f)
                break;
        }
#endif
        caster->CastSpell(toyPile, SPELL_ENERGY_ORB, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(nullptr, SPELL_LIGHTNING_TETHER, TRIGGERED_OLD_TRIGGERED);
        data.spellInfo = nullptr;
    }
};

struct EnergyOrbDummy : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE)
            return;
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;
        if (target->GetEntry() == NPC_XT_TOY_PILE)
        {
            for (uint8 i = 0; i < MAX_SCRAPBOTS; ++i)
                target->CastSpell(nullptr, SPELL_RECHARGE_ROBOT_1, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, roll_chance_i(80) ? SPELL_RECHARGE_ROBOT_2 : SPELL_RECHARGE_ROBOT_3, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct RechargeRobot : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(caster->GetInstanceData());
        if (!instance)
            return;
        Creature* xt = instance->GetSingleCreatureFromStorage(NPC_XT002);
        if (!xt)
            return;
        summon->GetMotionMaster()->MoveChase(xt, 0.f, 0.f, false, true);
        xt->AddSummonForOnDeathDespawn(summon->GetObjectGuid());
    }
};

struct SearingLight : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(target->GetInstanceData());
        if (!instance)
            return;
        if (instance->GetData(TYPE_XT002_HARD) != DONE)
            return;
        target->CastSpell(nullptr, SPELL_LIFE_SPARK, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
    }
};

struct GravityBomb : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        instance_ulduar* instance = dynamic_cast<instance_ulduar*>(target->GetInstanceData());
        if (!instance)
            return;
        if (instance->GetData(TYPE_XT002_HARD) != DONE)
            return;
        bool isRegularMode = target->GetMap()->IsRegularDifficulty();
        target->CastSpell(nullptr, isRegularMode ? SPELL_VOIDZONE : SPELL_VOIDZONE_H, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
    }
};

struct LifeSparkXT : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        bool isRegularMode = spell->GetCaster()->GetMap()->IsRegularDifficulty();
        auto value = spell->CalculateSpellEffectValue(EFFECT_INDEX_1, summon);
        summon->CastSpell(nullptr, isRegularMode ? SPELL_STATIC_CHARGED : SPELL_STATIC_CHARGED_H, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(nullptr, SPELL_ARCANE_POWER_STATE, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(nullptr, SPELL_SHOCK, TRIGGERED_OLD_TRIGGERED);
    }
};

struct VoidZoneXT : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (summon->AI())
        {
            summon->AI()->SetCombatMovement(false);
            summon->AI()->SetMeleeEnabled(false);
            summon->AI()->SetAIImmobilizedState(true);
        }
        auto value = spell->CalculateSpellEffectValue(EFFECT_INDEX_1, summon);
        summon->CastCustomSpell(nullptr, SPELL_CONSUMPTION, &value, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ConsumptionXT : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        int32 bp = aura->GetBasePoints();
        data.basePoints[0] = bp;
    }
};

void AddSC_boss_xt_002()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_xt_002";
    pNewScript->GetAI = &GetNewAIInstance<boss_xt_002AI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_heart_deconstructor";
    pNewScript->GetAI = &GetNewAIInstance<boss_heart_deconstructorAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scrapbot";
    pNewScript->GetAI = &GetNewAIInstance<npc_scrapbotAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_xt_toy_pile";
    pNewScript->GetAI = &GetNewAIInstance<npc_xt_toy_pileAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ExposedHeart>("spell_exposed_heart");
    RegisterSpellScript<HeartOverload>("spell_heart_overload");
    RegisterSpellScript<EnergyOrbDummy>("spell_energy_orb_dummy");
    RegisterSpellScript<RechargeRobot>("spell_recharge_robot");
    RegisterSpellScript<SearingLight>("spell_searing_light");
    RegisterSpellScript<GravityBomb>("spell_gravity_bomb");
    RegisterSpellScript<LifeSparkXT>("spell_life_spark_xt");
    RegisterSpellScript<VoidZoneXT>("spell_void_zone_xt");
    RegisterSpellScript<ConsumptionXT>("spell_consumption_xt");
}
