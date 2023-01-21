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
SDName: assembly_of_iron
SD%Complete: 90%
SDComment: Lightning Tendrils target following could use some love from the core side
SDCategory: Ulduar
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ulduar.h"
#include "AI/ScriptDevAI/base/BossAI.h"

enum
{
    SAY_BRUNDIR_AGGRO                   = 34314,
    SAY_BRUNDIR_WHIRL                   = 33962,
    SAY_BRUNDIR_DEATH_1                 = 34318,
    SAY_BRUNDIR_DEATH_2                 = 34319,
    SAY_BRUNDIR_SLAY_1                  = 34315,
    SAY_BRUNDIR_SLAY_2                  = 34316,
    SAY_BRUNDIR_BERSERK                 = 34320,
    SAY_BRUNDIR_FLY                     = 34317,

    SAY_MOLGEIM_AGGRO                   = 34328,
    SAY_MOLGEIM_DEATH_1                 = 34333,
    SAY_MOLGEIM_DEATH_2                 = 34334,
    SAY_MOLGEIM_DEATH_RUNE              = 34331,
    SAY_MOLGEIM_SURGE                   = 34332,
    SAY_MOLGEIM_SLAY_1                  = 34329,
    SAY_MOLGEIM_SLAY_2                  = 34330,
    SAY_MOLGEIM_BERSERK                 = 34320,

    SAY_STEEL_AGGRO                     = 34321,
    SAY_STEEL_DEATH_1                   = 34325,
    SAY_STEEL_DEATH_2                   = 34326,
    SAY_STEEL_SLAY_1                    = 34322,
    SAY_STEEL_SLAY_2                    = 34323,
    SAY_STEEL_OVERWHELM                 = 34324,
    SAY_STEEL_BERSERK                   = 34320,

    // Common spells
    SPELL_BERSERK                       = 62535,        // triggers 47008 after 15 min
    SPELL_SUPERCHARGE                   = 61920,
    SPELL_LIGHTNING_CHANNEL_PREFIGHT    = 61942,        // cast by Brundir on Steelbreaker
    SPELL_RUNE_OF_POWER_PREFIGHT        = 61975,        // cast by Molgeim on Stellbreaker
    SPELL_COUNCIL_KILL_CREDIT           = 65195,        // currently missing from DBC

    // Steelbreaker
    SPELL_HIGH_VOLTAGE                  = 61890,        // phase 1 spells
    SPELL_HIGH_VOLTAGE_H                = 63498,        // probably related to 61892 - couldn't find any info regarding this one
    SPELL_FUSION_PUNCH                  = 61903,
    SPELL_FUSION_PUNCH_H                = 63493,
    SPELL_STATIC_DISRUPTION             = 61911,        // phase 2 spells
    SPELL_STATIC_DISRUPTION_H           = 63495,        // should be triggered by 64641
    SPELL_OVERWHELMING_POWER            = 61888,        // phase 3 spells
    SPELL_OVERWHELMING_POWER_H          = 64637,
    SPELL_ELECTRICAL_CHARGE             = 61900,        // triggers 61901 when target dies

    // Runemaster Molgeim
    SPELL_SHIELD                        = 62274,        // phase 1 spells
    SPELL_SHIELD_H                      = 63489,
    SPELL_RUNE_OF_POWER                 = 61973,
    SPELL_RUNE_OF_DEATH                 = 62269,        // phase 2 spells
    SPELL_RUNE_OF_DEATH_H               = 63490,
    SPELL_RUNE_OF_SUMMONING             = 62273,        // phase 3 spells

    // Stormcaller Brundir
    SPELL_CHAIN_LIGHTNING               = 61879,        // phase 1 spells
    SPELL_CHAIN_LIGHTNING_H             = 63479,
    SPELL_OVERLOAD                      = 61869,
    SPELL_LIGHTNING_WHIRL               = 61915,        // phase 2 spells
    SPELL_LIGHTNING_WHIRL_H             = 63483,
    SPELL_LIGHTNING_WHIRL_DAMAGE        = 61916,        // used to check achiev criterias
    SPELL_LIGHTNING_WHIRL_DAMAGE_H      = 63482,
    SPELL_STORMSHIELD                   = 64187,        // phase 3 spells
    SPELL_LIGHTNING_TENDRILS            = 61887,
    SPELL_LIGHTNING_TENDRILS_H          = 63486,
    SPELL_TENDRILS_VISUAL               = 61883,

    // Summoned spells
    SPELL_OVERLOAD_AURA                 = 61877,
    SPELL_RUNE_OF_POWER_AURA            = 61974,
    SPELL_RUNE_OF_SUMMONING_AURA        = 62019,        // triggers 62020 which summons 32958
    SPELL_LIGHTNING_ELEMENTAL_PASSIVE   = 62052,
    SPELL_LIGHTNING_ELEMENTAL_PASSIVE_H = 63492,

    // summoned npcs
    NPC_OVERLOAD_VISUAL                 = 32866,
    NPC_RUNE_OF_POWER                   = 33705,
    NPC_RUNE_OF_SUMMONING               = 33051,
    NPC_LIGHTNING_ELEMENTAL             = 32958,

    PHASE_NO_CHARGE                     = 0,
    PHASE_CHARGE_ONE                    = 1,
    PHASE_CHARGE_TWO                    = 2,

    POINT_ID_LIFT_OFF                   = 1,
    POINT_ID_LAND                       = 2,
    POINT_ID_PRECHANNEL                 = 3,
};

enum BrundirActions
{
    BRUNDIR_PREFIGHT_CHANNEL,
    BRUNDIR_CLOSE_DOOR,
    BRUNDIR_ACTIONS_MAX,
};

struct boss_brundirAI : public BossAI
{
    boss_brundirAI(Creature* creature) : BossAI(creature, BRUNDIR_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_BRUNDIR_SLAY_1, SAY_BRUNDIR_SLAY_2);
        AddOnDeathText(SAY_BRUNDIR_DEATH_1, SAY_BRUNDIR_DEATH_2);
        AddCastOnDeath({ObjectGuid(), SPELL_SUPERCHARGE, TRIGGERED_OLD_TRIGGERED});
        AddCombatAction(BRUNDIR_CLOSE_DOOR, 3s);
        AddCustomAction(BRUNDIR_PREFIGHT_CHANNEL, 5s, [&]()
        {
            if (m_creature->IsInCombat())
                return;
            float o = urand(0, 5) * M_PI_F / 3.0f;
            Creature* steel = m_instance->GetSingleCreatureFromStorage(NPC_STEELBREAKER);
            if (!steel)
                return;
            m_creature->InterruptNonMeleeSpells(true);
            Position dest;
            steel->GetFirstCollisionPosition(dest, 10.f, o);
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_PRECHANNEL, dest, FORCED_MOVEMENT_RUN);
        }, TIMER_COMBAT_OOC);
        m_creature->SetNoLoot(true);
        m_creature->SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 13.f); // Should be 10.f but that results him rising only 10 units, when he should rise by 13 (probably some collision height calculations)
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_followTargetGuid;

    void Reset() override
    {
        BossAI::Reset();
        m_creature->SetHover(false);
        m_creature->SetStunned(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        m_creature->InterruptNonMeleeSpells(false);
        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        ResetTimer(BRUNDIR_PREFIGHT_CHANNEL, 5s);
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void JustDied(Unit* who) override
    {
        BossAI::JustDied(who);
        if (m_creature->GetHoverOffset() > 0)
            m_creature->GetMotionMaster()->MoveFall();
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType != POINT_MOTION_TYPE || data != POINT_ID_PRECHANNEL)
            return;
        m_creature->CastSpell(nullptr, SPELL_LIGHTNING_CHANNEL_PREFIGHT, TRIGGERED_OLD_TRIGGERED);
        if (!m_creature->IsInCombat())
            ResetTimer(BRUNDIR_PREFIGHT_CHANNEL, 10s);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->AI())
            summoned->AI()->SetRootSelf(true);
        m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
        summoned->SetInCombatWithZone(false);
        if (summoned->GetEntry() == NPC_OVERLOAD_VISUAL)
        {
            if (summoned->AI())
                summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->CastSpell(summoned, SPELL_OVERLOAD_AURA, TRIGGERED_OLD_TRIGGERED);
            // Visual npc- shouldn't move and should despawn in 6 sec
            summoned->GetMotionMaster()->MoveIdle();
            summoned->ForcedDespawn(6000);
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell) override
    {
        if (target->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_instance)
            return;

        // Check achiev criterias
        switch (spell->Id)
        {
            case SPELL_CHAIN_LIGHTNING:
            case SPELL_CHAIN_LIGHTNING_H:
            case SPELL_LIGHTNING_WHIRL_DAMAGE:
            case SPELL_LIGHTNING_WHIRL_DAMAGE_H:
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_STUNNED, false);
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        if (action == BRUNDIR_CLOSE_DOOR)
        {
            if (m_creature->IsAlive() && m_creature->IsInCombat())
                if (GameObject* door = m_instance->GetSingleGameObjectFromStorage(GO_IRON_ENTRANCE_DOOR))
                    door->SetGoState(GO_STATE_READY);
            DisableCombatAction(action);
        }
    }
};

enum MolgeimActions
{
    MOLGEIM_PRE_FIGHT_VISUAL,
    MOLGEIM_ACTIONS_MAX,
};

struct boss_molgeimAI : public BossAI
{
    boss_molgeimAI(Creature* creature) : BossAI(creature, MOLGEIM_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar *>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_MOLGEIM_SLAY_1, SAY_MOLGEIM_SLAY_2);
        AddOnDeathText(SAY_MOLGEIM_DEATH_1, SAY_MOLGEIM_DEATH_2);
        AddCastOnDeath({ObjectGuid(), SPELL_SUPERCHARGE, TRIGGERED_OLD_TRIGGERED});
        AddCustomAction(MOLGEIM_PRE_FIGHT_VISUAL, 5s, [&]()
        {
            Creature* steel = m_instance->GetSingleCreatureFromStorage(NPC_STEELBREAKER);
            if (!steel || m_creature->IsInCombat())
                return;
            m_creature->CastSpell(steel, SPELL_RUNE_OF_POWER_PREFIGHT, TRIGGERED_OLD_TRIGGERED);
        });
        m_creature->SetNoLoot(true);
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        m_creature->InterruptNonMeleeSpells(false);
        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        ResetTimer(MOLGEIM_PRE_FIGHT_VISUAL, 5s);
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->AI())
        {
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->AI()->SetRootSelf(true);
        }
        m_creature->AddSummonForOnDeathDespawn(summoned->GetObjectGuid());
        if (summoned->GetEntry() == NPC_RUNE_OF_SUMMONING)
        {
            summoned->CastSpell(summoned, SPELL_RUNE_OF_SUMMONING_AURA, true, nullptr, nullptr, m_creature->GetObjectGuid());
            summoned->ForcedDespawn(20000);
        }
        else if (summoned->GetEntry() == NPC_LIGHTNING_ELEMENTAL)
        {
            summoned->CastSpell(summoned, m_isRegularMode ? SPELL_LIGHTNING_ELEMENTAL_PASSIVE : SPELL_LIGHTNING_ELEMENTAL_PASSIVE_H, TRIGGERED_OLD_TRIGGERED);

            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                summoned->AI()->SetReactState(REACT_AGGRESSIVE);
                summoned->AI()->AttackStart(target);
            }
        }
    }
};

enum STEELBREAKER_ACTIONS
{
    STEELBREAKER_ACTION_MAX,
};

struct boss_steelbreakerAI : public BossAI
{
    boss_steelbreakerAI(Creature* creature) : BossAI(creature, STEELBREAKER_ACTION_MAX),
        m_instance(dynamic_cast<instance_ulduar *>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddOnKillText(SAY_STEEL_SLAY_1, SAY_STEEL_SLAY_2);
        AddOnDeathText(SAY_STEEL_DEATH_1, SAY_STEEL_DEATH_2);
        AddCastOnDeath({ObjectGuid(), SPELL_SUPERCHARGE, TRIGGERED_OLD_TRIGGERED});
        m_creature->SetNoLoot(true);
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, FAIL);
    }
};

struct LightningWhirl : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(2, 3));
    }
};

struct LightningWhirlHeroic : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(3, 6));
    }
};

struct SuperChargeIronCouncil : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) >= 2)
        {
            if (instance_ulduar* instance = dynamic_cast<instance_ulduar*>(caster->GetInstanceData()))
            {
                switch (caster->GetEntry())
                {
                    case NPC_BRUNDIR: instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_BRUNDIR, true); break;
                    case NPC_MOLGEIM: instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MOLGEIM, true); break;
                    case NPC_STEELBREAKER: instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_STEELBREAKER, true); break;
                }
                caster->CastSpell(nullptr, SPELL_COUNCIL_KILL_CREDIT, TRIGGERED_OLD_TRIGGERED);
                instance->SetData(TYPE_ASSEMBLY, DONE);
                static_cast<Creature*>(caster)->SetNoLoot(false);
            }
            return;
        }
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (eff != EFFECT_INDEX_2)
            return true;
        if (!target || !target->IsAlive())
            return false;
        if (target->GetEntry() == NPC_BRUNDIR && target->GetAuraCount(SPELL_SUPERCHARGE) == 1)
            target->CastSpell(nullptr, SPELL_STORMSHIELD, TRIGGERED_OLD_TRIGGERED);
        if (target->GetEntry() == NPC_STEELBREAKER && target->GetAuraCount(SPELL_SUPERCHARGE) == 1)
        {
            target->CastSpell(nullptr, SPELL_ELECTRICAL_CHARGE, TRIGGERED_OLD_TRIGGERED);
        }
        target->SetHealthPercent(100.f);
        if (target->AI())
            target->AI()->SpellListChanged();
        return true;
    }
};

struct LightningTendrils : public SpellScript, public AuraScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 2)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (apply)
        {
            caster->CastSpell(nullptr, SPELL_TENDRILS_VISUAL, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(SAY_BRUNDIR_FLY, caster);
            caster->SetHover(true);
            caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            if (caster->AI())
            {
                caster->AI()->SetReactState(REACT_PASSIVE);
                caster->AI()->SetCombatScriptStatus(true);
                caster->AttackStop();
                caster->SetTarget(nullptr);
                caster->GetMotionMaster()->Clear();
                caster->GetMotionMaster()->MovePoint(POINT_ID_LIFT_OFF, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ() + caster->GetHoverOffset());
            }
        }
        else
        {
            caster->RemoveAurasDueToSpell(SPELL_TENDRILS_VISUAL);
            caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            if (caster->AI())
            {
                caster->AI()->SetReactState(REACT_AGGRESSIVE);
                caster->AI()->SetCombatMovement(true);
                caster->AI()->SetCombatScriptStatus(false);
            }
            if (caster->GetVictim())
                caster->GetMotionMaster()->MoveChase(caster->GetVictim());//caster->SetStunned(false);
        }
    }

    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (aura->GetAuraTicks() <= 29)
        {
            if (!(aura->GetAuraTicks() % 5))
            {
                if (Unit* target = caster->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                {
                    caster->GetMotionMaster()->Clear();
                    caster->GetMotionMaster()->MoveChase(target);
                }
            }
            if (aura->GetAuraTicks() == 25)
            {
                float gZ = caster->GetMap()->GetHeight(caster->GetPhaseMask(), caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                caster->GetMotionMaster()->Clear();
                caster->GetMotionMaster()->MovePointTOL(POINT_ID_LAND, caster->GetPositionX(), caster->GetPositionY(), gZ, false);
                caster->SetHover(false);
            }
            return;
        }
        caster->RemoveAurasDueToSpell(aura->GetId());
    }
};

struct RuneOfDeathCouncil : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 1)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        DoBroadcastText(SAY_MOLGEIM_DEATH_RUNE, caster);
    }
};

struct RuneOfSummoningCouncil : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 2)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        DoBroadcastText(SAY_MOLGEIM_SURGE, caster);
    }
};

struct OverwhelmingPower : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 2)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        DoBroadcastText(SAY_STEEL_OVERWHELM, caster);
    }
};

struct StaticDisruption : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;
        Unit* caster = spell->GetCaster();
        if (!caster)
            return;
        auto targets = spell->GetTargetList();
        if (targets.empty())
            return;
        bool isRegularDifficulty = caster->GetMap()->IsRegularDifficulty();
        UnitList unitList;
        for (auto& target : targets)
        {
            if (Unit* utarget = caster->GetMap()->GetUnit(target.targetGUID))
                unitList.push_back(utarget);
        }
        unitList.sort(TargetDistanceOrderFarAway(caster));
        unitList.resize(std::min(uint32(unitList.size()), uint32(isRegularDifficulty ? 2 : 3)));
        std::vector targetVector(unitList.begin(), unitList.end());
        std::shuffle(targetVector.begin(), targetVector.end(), *GetRandomGenerator());
        if (Unit* target = targetVector.front())
        {
            caster->CastSpell(target, isRegularDifficulty ? SPELL_STATIC_DISRUPTION : SPELL_STATIC_DISRUPTION_H, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG | TRIGGERED_IGNORE_CASTER_AURA_STATE);
        }
    }

    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 1)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }
};

struct BerserkCouncil : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        switch (caster->GetEntry())
        {
            case NPC_BRUNDIR: DoBroadcastText(SAY_BRUNDIR_BERSERK, caster); break;
            case NPC_MOLGEIM: DoBroadcastText(SAY_MOLGEIM_BERSERK, caster); break;
            case NPC_STEELBREAKER: DoBroadcastText(SAY_STEEL_BERSERK, caster); break;
        }
    }
};

struct LightningWhirlTrigger : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 1)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx)
    {
        Unit* caster = spell->GetCaster();
        if (!caster || effIdx != EFFECT_INDEX_0)
            return;
        DoBroadcastText(SAY_BRUNDIR_WHIRL, caster);
    }
};

struct RuneOfPowerCouncil : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        if (summon->AI())
        {
            summon->AI()->SetReactState(REACT_PASSIVE);
            summon->AI()->SetRootSelf(true);
        }
        summon->SetInCombatWithZone(false);
        summon->CastSpell(summon, SPELL_RUNE_OF_POWER_AURA, TRIGGERED_OLD_TRIGGERED);
        summon->ForcedDespawn(60000);
    }
};

struct ElectricalChargeCouncil : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            return;
        if (aura->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;
        Unit* target = aura->GetTarget();
        if (!target)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        target->CastSpell(aura->GetCaster(), aura->GetBasePoints(), TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_assembly_of_iron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_brundir";
    pNewScript->GetAI = &GetNewAIInstance<boss_brundirAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_molgeim";
    pNewScript->GetAI = &GetNewAIInstance<boss_molgeimAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_steelbreaker";
    pNewScript->GetAI = &GetNewAIInstance<boss_steelbreakerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<LightningWhirl>("spell_lightning_whirl");
    RegisterSpellScript<LightningWhirlHeroic>("spell_lightning_whirl_heroic");
    RegisterSpellScript<SuperChargeIronCouncil>("spell_supercharge_iron_council");
    RegisterSpellScript<BerserkCouncil>("spell_berserk_iron_council");
    RegisterSpellScript<StaticDisruption>("spell_static_disruption");
    RegisterSpellScript<OverwhelmingPower>("spell_overwhelming_power");
    RegisterSpellScript<RuneOfSummoningCouncil>("spell_rune_of_summoning_iron_council");
    RegisterSpellScript<RuneOfDeathCouncil>("spell_rune_of_death_iron_council");
    RegisterSpellScript<LightningTendrils>("spell_lightning_tendrils");
    RegisterSpellScript<LightningWhirlTrigger>("spell_lightning_whirl_trigger");
    RegisterSpellScript<RuneOfPowerCouncil>("spell_rune_of_power_council");
    RegisterSpellScript<ElectricalChargeCouncil>("spell_electrical_charge_council");
}
