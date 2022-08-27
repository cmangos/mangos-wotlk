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
SDName: Boss_Sapphiron
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "AI/ScriptDevAI/base/BossAI.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "naxxramas.h"

enum
{
    EMOTE_BREATH                = 36542,
    EMOTE_GENERIC_ENRAGED       = 7798,
    EMOTE_SAPPHIRON_FLY         = 32801,
    EMOTE_SAPPHIRON_LAND        = 32802,

    // All phases spells
    SPELL_FROST_AURA            = 71387,            // Periodically triggers 28531
    SPELL_FROST_AURA_H          = 55799,
    SPELL_BERSERK               = 26662,

    // Ground phase spells
    SPELL_CLEAVE                = 19983,
    SPELL_TAIL_SWEEP            = 15847,
    SPELL_TAIL_SWEEP_H          = 55696,
    SPELL_LIFE_DRAIN            = 28542,
    SPELL_LIFE_DRAIN_H          = 55665,
    SPELL_SUMMON_BLIZZARD_INIT  = 28560,
    SPELL_SUMMON_BLIZZARD       = 28561,

    // Air phase spells
    SPELL_DRAGON_HOVER          = 18430,
    SPELL_ICEBOLT_INIT          = 28526,            // Triggers spell 28522 (Icebolt)
    SPELL_ICEBOLT               = 28522,
    SPELL_ICEBOLT_IMMUNITY      = 31800,
    SPELL_ICEBLOCK_SUMMON       = 28535,
    SPELL_FROST_BREATH_DUMMY    = 30101,
    SPELL_FROST_BREATH          = 28524,            // Triggers spells 29318 (Frost Breath) and 30132 (Despawn Ice Block)
    SPELL_DESPAWN_ICEBLOCK_GO   = 28523,
    SPELL_SUMMON_WING_BUFFET    = 29329,
    SPELL_DESPAWN_WING_BUFFET   = 29330,            // Triggers spell 29336 (Despawn Buffet)
    //SPELL_WING_BUFFET           = 29328,
    SPELL_DESPAWN_BUFFET_EFFECT = 29336,
    SPELL_CHILL                 = 28547,
    SPELL_CHILL_H               = 55699,
    SPELL_ACHIEVEMENT_CHECK     = 60539,                    // unused

    NPC_BLIZZARD                = 16474,
    NPC_WING_BUFFET             = 17025,

    SPELLSET_10N                = 1598901,
    SPELLSET_AIR_10N            = 1598902,
    SPELLSET_NULL_10N           = 1598903,
    SPELLSET_25N                = 2999101,
    SPELLSET_AIR_25N            = 2999102,
    SPELLSET_NULL_25N           = 2999103,
};

static const float aLiftOffPosition[3] = {3522.386f, -5236.784f, 137.709f};

enum SapphironPhases
{
    PHASE_GROUND        = 1,
    PHASE_LIFT_OFF      = 2,
};

enum SapphironActions
{
    SAPPHIRON_AIR_PHASE,
    SAPPHIRON_LANDING_PHASE,
    SAPPHIRON_GROUND_PHASE,
    SAPPHIRON_BERSERK,
    SAPPHIRON_ACTION_MAX,
};

std::unordered_set<ObjectGuid> localIceBlocks;

struct boss_sapphironAI : public BossAI
{
    boss_sapphironAI(Creature* creature) : BossAI(creature, SAPPHIRON_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_phase(PHASE_GROUND), m_iceboltCount(0), m_blizzardCount(0)
    {
        SetDataType(TYPE_SAPPHIRON);
        AddCombatAction(SAPPHIRON_BERSERK, 15min);
        AddCombatAction(SAPPHIRON_AIR_PHASE, true);
        AddCombatAction(SAPPHIRON_LANDING_PHASE, true);
        AddCombatAction(SAPPHIRON_GROUND_PHASE, true);
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    SapphironPhases m_phase;
    uint32 m_iceboltCount;
    uint32 m_blizzardCount;

    void Reset() override
    {
        BossAI::Reset();

        m_iceboltCount = 0;
        m_blizzardCount = 0;
        m_phase = PHASE_GROUND;

        SetCombatMovement(true);
        SetDeathPrevention(false);
        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);
        m_creature->SetHover(false);
        m_creature->SetSpellList(m_isRegularMode ? SPELLSET_10N : SPELLSET_25N);
    }

    std::chrono::milliseconds GetSubsequentActionTimer(uint32 action)
    {
        switch (action)
        {
            case SAPPHIRON_BERSERK: return 5min;
            case SAPPHIRON_AIR_PHASE: return 46s;
            default: return 0s;
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_FROST_AURA : SPELL_FROST_AURA_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        ResetCombatAction(SAPPHIRON_AIR_PHASE, GetSubsequentActionTimer(SAPPHIRON_AIR_PHASE));
    }

    void MovementInform(uint32 type, uint32 /*pointId*/) override
    {
        if (type == POINT_MOTION_TYPE && m_phase == PHASE_LIFT_OFF)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_WING_BUFFET);

            m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
            DoBroadcastText(EMOTE_SAPPHIRON_FLY, m_creature);
            m_creature->SetHover(true);
            m_creature->CastSpell(nullptr, SPELL_DRAGON_HOVER, TRIGGERED_OLD_TRIGGERED);
            SetCombatScriptStatus(false);
            m_creature->SetSpellList(m_isRegularMode ? SPELLSET_AIR_10N : SPELLSET_AIR_25N);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch(action)
        {
            case SAPPHIRON_BERSERK:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoBroadcastText(EMOTE_GENERIC_ENRAGED, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case SAPPHIRON_AIR_PHASE:
            {
                if (m_creature->GetHealthPercent() > 10.f)
                {
                    m_phase = PHASE_LIFT_OFF;
                    SetCombatScriptStatus(true);
                    SetDeathPrevention(true);
                    m_creature->InterruptNonMeleeSpells(false);
                    SetCombatMovement(false);
                    SetMeleeEnabled(false);
                    m_creature->AttackStop(false, true);
                    m_creature->SetTarget(nullptr);
                    m_creature->GetMotionMaster()->MovePoint(1, aLiftOffPosition[0], aLiftOffPosition[1], aLiftOffPosition[2], FORCED_MOVEMENT_RUN);
                }
                DisableCombatAction(SAPPHIRON_AIR_PHASE);
                return;
            }
            case SAPPHIRON_LANDING_PHASE:
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                DoBroadcastText(EMOTE_SAPPHIRON_LAND, m_creature);
                if (Creature* buffetDummy = GetClosestCreatureWithEntry(m_creature, NPC_WING_BUFFET, 10.f))
                    buffetDummy->ForcedDespawn(); // Despawn Dummy manually due to different Air-Phase durations between 10 and 25 versions
                ResetCombatAction(SAPPHIRON_GROUND_PHASE, 2s);
                DisableCombatAction(SAPPHIRON_LANDING_PHASE);
                return;
            }
            case SAPPHIRON_GROUND_PHASE:
            {
                m_phase = PHASE_GROUND;
                m_creature->SetSpellList(m_isRegularMode ? SPELLSET_10N : SPELLSET_25N);
                SetDeathPrevention(false);
                SetCombatMovement(true);
                SetMeleeEnabled(true);
                m_creature->RemoveAurasDueToSpell(SPELL_DRAGON_HOVER);
                m_creature->SetHover(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());

                ResetCombatAction(SAPPHIRON_AIR_PHASE, GetSubsequentActionTimer(SAPPHIRON_AIR_PHASE));
                DisableCombatAction(SAPPHIRON_GROUND_PHASE);
                return;
            }
        }
        ResetCombatAction(action, GetSubsequentActionTimer(action));
    }
};

bool GOUse_go_sapphiron_birth(Player* /*player*/, GameObject* go)
{
    ScriptedInstance* instance = (ScriptedInstance*)go->GetInstanceData();

    if (!instance)
        return true;

    if (instance->GetData(TYPE_SAPPHIRON) != NOT_STARTED)
        return true;

    // If already summoned return (safety check)
    if (instance->GetSingleCreatureFromStorage(NPC_SAPPHIRON, true))
        return true;

    // Set data to special and allow the Go animation to proceed
    instance->SetData(TYPE_SAPPHIRON, SPECIAL);
    return false;
}

struct IceBolt : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /* effIdx */) const override
    {
        Creature* caster = static_cast<Creature*>(spell->GetCaster());
        if (!caster)
            return;
        boss_sapphironAI* boss_ai = dynamic_cast<boss_sapphironAI*>(caster->AI());
        if (!boss_ai)
            return;

        if (Unit* target = caster->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ICEBOLT, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
            caster->CastSpell(target, SPELL_ICEBOLT, TRIGGERED_NONE);

        if (boss_ai->m_iceboltCount >= (boss_ai->m_isRegularMode ? 2 : 3))
        {
            caster->CastSpell(nullptr, SPELL_FROST_BREATH, TRIGGERED_IGNORE_COOLDOWNS | TRIGGERED_IGNORE_GCD);
            caster->CastSpell(nullptr, SPELL_FROST_BREATH_DUMMY, TRIGGERED_IGNORE_COOLDOWNS | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
            DoBroadcastText(EMOTE_BREATH, caster);
            caster->SetSpellList(boss_ai->m_isRegularMode ? SPELLSET_NULL_10N : SPELLSET_NULL_25N);
            boss_ai->ResetCombatAction(SAPPHIRON_LANDING_PHASE, 10s);
        }

        boss_ai->m_iceboltCount += 1;
    }
};

struct FrostBreath : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;

        if (boss_sapphironAI* ai = dynamic_cast<boss_sapphironAI*>(caster->AI()))
        {
            ai->m_iceboltCount = 0;
        }
    }
};

struct PeriodicIceBolt : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        Unit* target =  aura->GetTarget();
        if (!target)
            return;

        if (target->IsAlive() && !target->HasAura(SPELL_ICEBOLT_IMMUNITY))
        {
            target->CastSpell(nullptr, SPELL_ICEBOLT_IMMUNITY, TRIGGERED_OLD_TRIGGERED);     // Icebolt which causes immunity to frost dmg
            data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SPELL_ICEBLOCK_SUMMON); // Summon Ice Block
        }
    }
};

struct DespawnIceBlock : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /* effIdx */) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        if (unitTarget->IsPlayer())
        {
            unitTarget->RemoveAurasDueToSpell(SPELL_ICEBOLT_IMMUNITY);                          // Icebolt immunity spell
            unitTarget->RemoveAurasDueToSpell(SPELL_ICEBOLT);                                   // Icebolt stun/damage spell
            unitTarget->CastSpell(nullptr, SPELL_DESPAWN_ICEBLOCK_GO, TRIGGERED_OLD_TRIGGERED); // Despawn Ice Block (targets Ice Block GOs)
        }
    }
};

struct SapphironBlizzardInit : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster || !spell->GetUnitTarget())
            return;

        CreatureList blizzards;
        GetCreatureListWithEntryInGrid(blizzards, caster, NPC_BLIZZARD, 100.f);
        uint8 maximumBlizzards = caster->GetMap()->IsRegularDifficulty() ? 2 : 6;
        if (blizzards.size() >= maximumBlizzards || !caster->AI())
            return;
        spell->GetUnitTarget()->CastSpell(nullptr, SPELL_SUMMON_BLIZZARD, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SapphironAchievementCheck : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex) const override
    {
        Creature* caster = dynamic_cast<Creature*>(spell->GetCaster());
        if (!caster)
            return;
        instance_naxxramas* instance = dynamic_cast<instance_naxxramas*>(caster->GetInstanceData());
        if (!instance)
            return;

        const auto& targets = spell->GetTargetList();
        Map* map = caster->GetMap();
        bool failed = false;
        if (!map)
            return;

        for (auto& target : targets)
        {
            Player* player = map->GetPlayer(target.targetGUID);
            if (!player)
                continue;
            if (player->GetResistance(SPELL_SCHOOL_FROST) > 100)
            {
                failed = true;
                break;
            }
        }
        if (failed)
            instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_HUNDRED_CLUB, false);
    }
};

void AddSC_boss_sapphiron()
{
    Script* newScript = new Script;
    newScript->Name = "boss_sapphiron";
    newScript->GetAI = &GetNewAIInstance<boss_sapphironAI>;
    newScript->RegisterSelf();

    newScript = new Script;
    newScript->Name = "go_sapphiron_birth";
    newScript->pGOUse = &GOUse_go_sapphiron_birth;
    newScript->RegisterSelf();

    RegisterSpellScript<PeriodicIceBolt>("spell_sapphiron_icebolt_aura");
    RegisterSpellScript<IceBolt>("spell_sapphiron_icebolt");
    RegisterSpellScript<FrostBreath>("spell_sapphiron_frost_breath");
    RegisterSpellScript<DespawnIceBlock>("spell_sapphiron_iceblock");
    RegisterSpellScript<SapphironBlizzardInit>("spell_sapphiron_blizzard_init");
    RegisterSpellScript<SapphironAchievementCheck>("spell_sapphiron_achievement_check");
}