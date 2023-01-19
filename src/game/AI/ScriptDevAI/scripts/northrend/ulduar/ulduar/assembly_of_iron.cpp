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
    SAY_BRUNDIR_AGGRO                   = -1603056,
    SAY_BRUNDIR_WHIRL                   = -1603057,
    SAY_BRUNDIR_DEATH_1                 = -1603058,
    SAY_BRUNDIR_DEATH_2                 = -1603059,
    SAY_BRUNDIR_SLAY_1                  = -1603060,
    SAY_BRUNDIR_SLAY_2                  = -1603061,
    SAY_BRUNDIR_BERSERK                 = -1603062,
    SAY_BRUNDIR_FLY                     = -1603063,

    SAY_MOLGEIM_AGGRO                   = -1603064,
    SAY_MOLGEIM_DEATH_1                 = -1603065,
    SAY_MOLGEIM_DEATH_2                 = -1603066,
    SAY_MOLGEIM_DEATH_RUNE              = -1603067,
    SAY_MOLGEIM_SURGE                   = -1603068,
    SAY_MOLGEIM_SLAY_1                  = -1603069,
    SAY_MOLGEIM_SLAY_2                  = -1603070,
    SAY_MOLGEIM_BERSERK                 = -1603071,

    SAY_STEEL_AGGRO                     = -1603072,
    SAY_STEEL_DEATH_1                   = -1603073,
    SAY_STEEL_DEATH_2                   = -1603074,
    SAY_STEEL_SLAY_1                    = -1603075,
    SAY_STEEL_SLAY_2                    = -1603076,
    SAY_STEEL_OVERWHELM                 = -1603077,
    SAY_STEEL_BERSERK                   = -1603078,

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
};

enum BrundirActions
{
    BRUNDIR_PREFIGHT_CHANNEL,
    BRUNDIR_ACTIONS_MAX,
};

struct boss_brundirAI : public BossAI
{
    boss_brundirAI(Creature* creature) : BossAI(creature, BRUNDIR_ACTIONS_MAX),
        m_instance(dynamic_cast<instance_ulduar*>(creature->GetInstanceData())),
        m_isRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        //SetDataType(TYPE_ASSEMBLY);
        AddOnAggroText(SAY_BRUNDIR_AGGRO);
        AddOnKillText(SAY_BRUNDIR_SLAY_1, SAY_BRUNDIR_SLAY_2);
        AddOnDeathText(SAY_BRUNDIR_DEATH_1, SAY_BRUNDIR_DEATH_2);
        AddCastOnDeath({ObjectGuid(), SPELL_SUPERCHARGE, TRIGGERED_OLD_TRIGGERED});
        AddCustomAction(BRUNDIR_PREFIGHT_CHANNEL, 5s, [&]()
        {
            m_creature->CastSpell(nullptr, SPELL_LIGHTNING_CHANNEL_PREFIGHT, TRIGGERED_OLD_TRIGGERED);
        });
    }

    instance_ulduar* m_instance;
    bool m_isRegularMode;

    ObjectGuid m_followTargetGuid;

    void Reset() override
    {
        m_creature->SetLevitate(false);
    }

    void Aggro(Unit* /*who*/) override
    {
        BossAI::Aggro();
        m_creature->InterruptNonMeleeSpells(false);
        DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED);
    }

    void JustReachedHome() override
    {
        ResetTimer(BRUNDIR_PREFIGHT_CHANNEL, 5s);
        if (m_instance)
            m_instance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_OVERLOAD_VISUAL)
        {
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

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != POINT_MOTION_TYPE || !pointId)
            return;

        switch (pointId)
        {
            // After lift up follow a target and set the target change timer
            case POINT_ID_LIFT_OFF:
                // TODO: the boss should follow without changing his Z position - missing core feature
                // Current implementation with move point is wrong
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                {
                    DoMoveToTarget(target);
                    m_followTargetGuid = target->GetObjectGuid();
                }
                break;
            // After reached the land remove all the auras and resume basic combat
            case POINT_ID_LAND:
                m_creature->SetLevitate(false);
                SetCombatMovement(true);
                if (m_creature->GetVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                break;
        }
    }

    // Wrapper for target movement
    void DoMoveToTarget(Unit* target)
    {
        if (target)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), m_creature->GetPositionZ());
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
        AddOnAggroText(SAY_MOLGEIM_AGGRO);
        AddOnKillText(SAY_MOLGEIM_SLAY_1, SAY_MOLGEIM_SLAY_2);
        AddOnDeathText(SAY_MOLGEIM_DEATH_1, SAY_MOLGEIM_DEATH_2);
        AddCastOnDeath({ObjectGuid(), SPELL_SUPERCHARGE, TRIGGERED_OLD_TRIGGERED});
        AddCustomAction(MOLGEIM_PRE_FIGHT_VISUAL, 5s, [&]()
        {
            m_creature->CastSpell(nullptr, SPELL_RUNE_OF_POWER_PREFIGHT, TRIGGERED_OLD_TRIGGERED);
        });
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
        if (summoned->GetEntry() == NPC_RUNE_OF_SUMMONING)
            summoned->CastSpell(summoned, SPELL_RUNE_OF_SUMMONING_AURA, true, nullptr, nullptr, m_creature->GetObjectGuid());
        else if (summoned->GetEntry() == NPC_RUNE_OF_POWER)
            summoned->CastSpell(summoned, SPELL_RUNE_OF_POWER_AURA, TRIGGERED_OLD_TRIGGERED);
        else if (summoned->GetEntry() == NPC_LIGHTNING_ELEMENTAL)
        {
            summoned->CastSpell(summoned, m_isRegularMode ? SPELL_LIGHTNING_ELEMENTAL_PASSIVE : SPELL_LIGHTNING_ELEMENTAL_PASSIVE_H, TRIGGERED_OLD_TRIGGERED);

            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                summoned->AI()->AttackStart(target);
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
        AddOnAggroText(SAY_STEEL_AGGRO);
        AddOnKillText(SAY_STEEL_SLAY_1, SAY_STEEL_SLAY_2);
        AddOnDeathText(SAY_STEEL_DEATH_1, SAY_STEEL_DEATH_2);
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

    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (!caster->HasAura(SPELL_SUPERCHARGE))
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

struct LightningWhirlHeroic : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(3, 6));
    }

    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (!caster->HasAura(SPELL_SUPERCHARGE))
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

struct SuperChargeIronCouncil : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;
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
            }
            return;
        }
        else
        {
            caster->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsAlive())
            return;
        if (target->GetEntry() == NPC_BRUNDIR && target->GetAuraCount(SPELL_SUPERCHARGE) == 2)
            target->CastSpell(nullptr, SPELL_STORMSHIELD, TRIGGERED_OLD_TRIGGERED);
        target->SetHealthPercent(100.f);
        if (target->AI())
            target->AI()->SpellListChanged();
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
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (apply)
        {
            caster->CastSpell(nullptr, SPELL_TENDRILS_VISUAL, TRIGGERED_OLD_TRIGGERED);
            DoBroadcastText(SAY_BRUNDIR_FLY, caster);
            caster->SetLevitate(true);
            caster->GetMotionMaster()->MovePoint(POINT_ID_LIFT_OFF, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ() + 15.0f);
        }
        else
        {
            float groundZ = caster->GetMap()->GetHeight(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), false);
            float fZ = caster->GetTerrain()->GetWaterOrGroundLevel(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), groundZ);
            caster->GetMotionMaster()->MovePoint(POINT_ID_LAND, caster->GetPositionX(), caster->GetPositionY(), fZ);
            caster->RemoveAurasDueToSpell(SPELL_TENDRILS_VISUAL);
        }
    }

    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        if (aura->GetAuraTicks() <= 25)
        {
            if (aura->GetAuraTicks() % 5)
            {
                if (Unit* target = caster->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
                {
                    if (boss_brundirAI* brundirAI = dynamic_cast<boss_brundirAI*>(caster->AI()))
                    {
                        brundirAI->DoMoveToTarget(target);
                        brundirAI->m_followTargetGuid = target->GetObjectGuid();
                    }
                }
            }
            return;
        }
        caster->RemoveAurasDueToSpell(aura->GetId());
    }
};

struct LightningTendrilsVisual : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;
        caster->RemoveAurasDueToSpell(aura->GetId());
        if (boss_brundirAI* brundirAI = dynamic_cast<boss_brundirAI*>(caster->AI()))
            if (Unit* target = caster->GetMap()->GetUnit(brundirAI->m_followTargetGuid))
                brundirAI->DoMoveToTarget(target);
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
    void OnInit(Spell* spell) const override
    {
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_FURTHEST);
    }

    SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster)
            return SPELL_FAILED_CASTER_DEAD;
        if (caster->GetAuraCount(SPELL_SUPERCHARGE) < 2)
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
}
