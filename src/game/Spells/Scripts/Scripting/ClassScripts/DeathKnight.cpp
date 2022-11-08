/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
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

#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"

struct ScourgeStrike : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        uint32 count = 0;
        Unit* target = spell->GetUnitTarget();
        Unit::SpellAuraHolderMap const& auras = target->GetSpellAuraHolderMap();
        for (const auto& aura : auras)
        {
            if (aura.second->GetSpellProto()->Dispel == DISPEL_DISEASE &&
                aura.second->GetCasterGuid() == spell->GetCaster()->GetObjectGuid())
                ++count;
        }

        if (count)
        {
            int32 bp = count * spell->CalculateSpellEffectValue(EFFECT_INDEX_2, target) * spell->GetTotalTargetDamage() / 100;
            if (bp)
                spell->GetCaster()->CastCustomSpell(target, 70890, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct RaiseDeadDk : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (eff == EFFECT_INDEX_1)
        {
            Unit* caster = spell->GetCaster();
            if (target->IsTrivialForTarget(caster))
                return false;

            if (target->GetDeathState() != CORPSE)
                return false;

            if (target->GetCreatureType() != CREATURE_TYPE_HUMANOID)
                return false;
        }

        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_2) // reagent consuming version
            if (spell->GetTargetList().size() > 1) // more than caster is hit
                return;

        Unit* caster = spell->GetCaster();
        uint32 spellId = caster->HasAura(52143) ? 52150 : 46585; // Master of Ghouls talent
        if (effIdx == EFFECT_INDEX_2) // corpse dust version
        {
            if (caster->CastSpell(nullptr, 48289, TRIGGERED_IGNORE_GCD) == SPELL_CAST_OK)
            {
                Position pos = Pet::GetPetSpawnPosition(caster);
                caster->CastSpell(pos.x, pos.y, pos.z, spellId, TRIGGERED_OLD_TRIGGERED);
            }
            else
                caster->RemoveSpellCooldown(*spell->m_spellInfo, true);
        }
        else if (effIdx == EFFECT_INDEX_1) // corpse version
            caster->CastSpell(spell->GetUnitTarget(), spellId, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DeathCoilDK : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        uint32 damage = spell->GetDamage();
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (caster->CanAssistSpell(target, spell->m_spellInfo))
        {
            if (!target || target->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                return;

            int32 bp = int32(damage * 1.5f);
            caster->CastCustomSpell(target, 47633, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
        else
        {
            int32 bp = damage;
            caster->CastCustomSpell(target, 47632, &bp, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct UnholyBlightDK : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        uint32 damagePercent = aura->GetModifier()->m_amount;
        if (Aura* glyphAura = aura->GetTarget()->GetAura(63332, EFFECT_INDEX_0))
            damagePercent += (damagePercent * glyphAura->GetModifier()->m_amount / 100);
        int32 damage = int32(procData.damage * damagePercent / 100);
        // unholy blight DOT
        if (procData.attacker)
            procData.attacker->CastCustomSpell(procData.target, 50536, &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_HIDE_CAST_IN_COMBAT_LOG);
        return SPELL_AURA_PROC_OK;
    }
};

struct DeathRuneDK : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (!procData.spell || !aura->GetTarget()->IsPlayer())
            return false;

        if (aura->GetTarget()->getClass() != CLASS_DEATH_KNIGHT)
            return false;

        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        uint8 oldRunesState = procData.spell->GetOldRuneState();
        uint8 newRunesState = procData.spell->GetNewRuneState();
        if (oldRunesState == newRunesState)
            return SPELL_AURA_PROC_OK;

        Player* player = static_cast<Player*>(aura->GetTarget());

        for (uint8 i = 0; i < MAX_RUNES; ++i)
        {
            uint8 mask = (1 << i);
            if (mask & oldRunesState && (!(mask & newRunesState))) // runes that were used by spell as cost
            {
                player->AddRuneByAuraEffect(i, RUNE_DEATH, aura);
            }
        }

        aura->ForcePeriodicity(aura->GetModifier()->periodictime);
        return SPELL_AURA_PROC_OK;
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target->IsPlayer())
            return;

        // timer expired - remove death runes
        static_cast<Player*>(target)->RemoveRunesByAura(aura);
    }
};

struct Bloodworm : public SpellScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 50453, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(nullptr, 52001, TRIGGERED_OLD_TRIGGERED);
        summon->AI()->SetReactState(REACT_DEFENSIVE);
    }
};

struct HealthLeechPassive : public AuraScript
{
    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        int32 heal = procData.damage * 150 / 100; // 1.5x
        aura->GetTarget()->CastCustomSpell(aura->GetTarget()->GetSpawner(), 50454, &heal, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
        return SPELL_AURA_PROC_OK;
    }
};

struct AntiMagicZone : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_HIDE_BODY);
        summon->CastSpell(nullptr, 40031, TRIGGERED_OLD_TRIGGERED);
    }
};

struct CorpseExplosionDK : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex eff) const override
    {
        if (eff == EFFECT_INDEX_0) // can be cast on a ghoul
        {
            if (target->IsPlayerControlled() && target->GetOwnerGuid() == spell->GetCaster()->GetObjectGuid() &&
                target->GetEntry() == 26125)
                return true;
        }
        
        if (target->GetDeathState() == CORPSE &&
            (target->GetCreatureTypeMask() & CREATURE_TYPEMASK_MECHANICAL_OR_ELEMENTAL) == 0)
            return true;

        return false;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (spell->GetScriptValue() == 1)
            return;

        spell->SetScriptValue(1); // mark as used
        Unit* target = spell->GetUnitTarget();
        if (effIdx == EFFECT_INDEX_0) // ghoul
        {
            target->InterruptNonMeleeSpells(false);
            target->CastSpell(nullptr, 47496, TRIGGERED_NORMAL_COMBAT_CAST);
        }
        else
        {
            int32 damage = spell->CalculateSpellEffectValue(EFFECT_INDEX_0, target);
            spell->GetCaster()->CastCustomSpell(spell->GetUnitTarget(), spell->CalculateSpellEffectValue(EFFECT_INDEX_1, spell->GetUnitTarget()),
                &damage, nullptr, nullptr, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL);
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), 51270, TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct ExplodeGhoulCorpseExplosion : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            int32 percentage = spell->CalculateSpellEffectValue(EFFECT_INDEX_2, spell->GetUnitTarget());
            spell->SetDamage(spell->GetCaster()->GetMaxHealth() * percentage / 100);
		}
        else
        {
            spell->SetDamage(spell->GetCaster()->GetMaxHealth());
        }
    }

    void OnSuccessfulFinish(Spell* spell) const override
    {
        // Suicide
        if (spell->GetCaster()->IsAlive())
            spell->GetCaster()->CastSpell(nullptr, 7, TRIGGERED_OLD_TRIGGERED);
        // Set corpse look
        if (spell->GetCaster()->IsCreature())
            static_cast<Creature*>(spell->GetCaster())->ForcedDespawn(500);
    }          
};

struct DeathKnightDisease : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        if (caster->GetOverrideScript(7282)) // Crypt Fever and Ebon Plaguebringer
        {
            Aura* chosen = nullptr; // need highest id
            for (Aura* aura : caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS))
                if (aura->GetModifier()->m_miscvalue == 7282)
                    if (!chosen || chosen->GetId() < aura->GetId())
                        chosen = aura;

            if (chosen)
            {
                uint32 spellId = 0;
                switch (chosen->GetId())
                {
                    // Ebon Plague
                    case 51161: spellId = 51735; break;
                    case 51160: spellId = 51734; break;
                    case 51099: spellId = 51726; break;
                        // Crypt Fever
                    case 49632: spellId = 50510; break;
                    case 49631: spellId = 50509; break;
                    case 49032: spellId = 50508; break;
                    default: break;
                }

                if (spellId)
                    caster->CastSpell(aura->GetTarget(), spellId, TRIGGERED_OLD_TRIGGERED);
            }
        }
    }
};

struct CryptFeverServerside : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.effIdx == EFFECT_INDEX_1)
        {
            if (data.caster)
            {
                Aura* aura = data.target->GetAura(51735, EFFECT_INDEX_1);
                if (!aura)
                    aura = data.target->GetAura(51734, EFFECT_INDEX_1);
                if (!aura)
                    aura = data.target->GetAura(51726, EFFECT_INDEX_1);
                if (aura)
                    value = aura->GetAmount();
            }                
        }
        return value;
    }
};

struct ArmyOfTheDead : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        if (data.effIdx == EFFECT_INDEX_1)
            value = data.target->GetParryChance() + data.target->GetDodgeChance();
        return value;
    }
};

struct ArmyOfTheDeadGhoul : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->CastSpell(nullptr, 7398, TRIGGERED_NORMAL_COMBAT_CAST);
        summon->CastSpell(nullptr, 51996, TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(nullptr, 67561, TRIGGERED_OLD_TRIGGERED); // unk scaling aura
        summon->CastSpell(nullptr, 61697, TRIGGERED_OLD_TRIGGERED);
        summon->AI()->SetFollowMovement(false);
    }
};

struct SuddenDoom : public AuraScript
{
    bool OnCheckProc(Aura* aura, ProcExecutionData& /*data*/) const override
    {
        if (aura->GetTarget()->IsPlayer())
            return true;

        return false;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        // Death Coil
        uint32 highestDeathCoil = static_cast<Player*>(aura->GetTarget())->LookupHighestLearnedRank(47541);
        if (highestDeathCoil && procData.attacker)
            procData.attacker->CastSpell(procData.victim, highestDeathCoil, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_COSTS);
        return SPELL_AURA_PROC_OK;
    }
};

struct WillOfTheNecropolis : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, uint32& /*reflectedSpellId*/, int32& /*reflectDamage*/, bool& /*preventedDeath*/) const override
    {
        currentAbsorb = 0;
        if (aura->GetTarget()->GetHealth() - remainingDamage < aura->GetTarget()->GetMaxHealth() * 35 / 100)
            currentAbsorb = aura->GetAmount() * remainingDamage / 100;
    }
};

enum DancingRuneWeaponData
{
    SPELL_COPY_WEAPON                   = 63416,
    SPELL_DANCING_RUNE_WEAPON_VISUAL    = 53160,
    SPELL_RUNE_WEAPON_MARK              = 50474,
    SPELL_FAKE_AGGRO_RADIUS_8YD         = 49812, // dummy periodic aura
    SPELL_AGGRO_RADIUS_8YD              = 49813,
    SPELL_RUNE_WEAPON_SCALING_01        = 51905,
    SPELL_RUNE_WEAPON_SCALING_02        = 51906,
    SPELL_DEATH_KNIGHT_PET_SCALING_03   = 61697,

    NPC_DANCING_RUNE_WEAPON             = 27893,
};

struct DancingRuneWeapon : public SpellScript, public AuraScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        Unit* caster = spell->GetCaster();
        summon->CastSpell(caster, SPELL_COPY_WEAPON, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_DANCING_RUNE_WEAPON_VISUAL, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_RUNE_WEAPON_MARK, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_FAKE_AGGRO_RADIUS_8YD, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_RUNE_WEAPON_SCALING_01, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_RUNE_WEAPON_SCALING_02, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_DEATH_KNIGHT_PET_SCALING_03, TRIGGERED_NONE);
        summon->AI()->SetMoveChaseParams(0.f, M_PI_F, false);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_1)
            if (Pet* guardian = aura->GetTarget()->FindGuardianWithEntry(NPC_DANCING_RUNE_WEAPON))
                guardian->ForcedDespawn();
    }

    bool OnCheckProc(Aura* aura, ProcExecutionData& data) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
            return data.spellInfo && data.spellInfo->IsFitToFamily(SPELLFAMILY_DEATHKNIGHT, 0x2002000001402013, 0x1);
        return true;
    }

    SpellAuraProcResult OnProc(Aura* aura, ProcExecutionData& procData) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1)
        {
            Player* player = dynamic_cast<Player*>(aura->GetTarget());
            Unit* runeWeapon = player->FindGuardianWithEntry(NPC_DANCING_RUNE_WEAPON);
            if (runeWeapon && runeWeapon->GetVictim())
                runeWeapon->CastSpell(runeWeapon->GetVictim(), procData.spellInfo, TRIGGERED_IGNORE_COSTS | TRIGGERED_NORMAL_COMBAT_CAST);
        }
        return SPELL_AURA_PROC_OK;
    }
};

struct FakeAggroRadius8YD : public AuraScript
{
    void OnAuraInit(Aura* aura) const override
    {
        aura->ForcePeriodicity(1000);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_AGGRO_RADIUS_8YD, TRIGGERED_OLD_TRIGGERED);
    }
};

struct AggroRadius8YD : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster->IsInCombat())
            return;

        if (caster->CanAttack(target))
        {
            if (caster->IsVisibleForOrDetect(target, target, true))
            {
                if (Unit* spawner = caster->GetSpawner())
                {
                    if (spawner->getAttackers().find(target) != spawner->getAttackers().end())
                    {
                        if (caster->GetVictim())
                            caster->AddThreat(target);
                        else
                            caster->AI()->AttackStart(target);
                    }
                }
            }
        }
    }
};

enum SummonGargoyleData
{
    SPELL_RISEN_GHOUL_SPAWN_IN = 47448,
    SPELL_DEATH_KNIGHT_PET_SCALING_01 = 54566,
    SPELL_DEATH_KNIGHT_PET_SCALING_02 = 51996,
    SPELL_AVOIDANCE_PASSIVE = 62137,
    SPELL_TAUNT_GARGOYLE = 37486,

    NPC_GARGOYLE_DK = 27829,

    POINT_ABOVE_TARGET = 1,
};

struct SummonGargoyle : public SpellScript, public AuraScript
{
    void OnSummon(Spell* spell, Creature* summon) const override
    {
        summon->CastSpell(nullptr, SPELL_RISEN_GHOUL_SPAWN_IN, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_DEATH_KNIGHT_PET_SCALING_01, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_DEATH_KNIGHT_PET_SCALING_02, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_DEATH_KNIGHT_PET_SCALING_03, TRIGGERED_NONE);
        summon->CastSpell(nullptr, SPELL_AVOIDANCE_PASSIVE, TRIGGERED_NONE);
        // TODO: Figure out cast speed scaling

        Unit* target = spell->m_targets.getUnitTarget();
        Position pos = target ? target->GetPosition() : spell->GetCaster()->GetPosition();
        pos.z += 15.f;
        summon->SetLevitate(true);
        summon->AI()->SetFollowMovement(false);
        summon->AI()->SetCombatMovement(false);
        summon->AI()->SetCombatScriptStatus(true);
        summon->AI()->SetMeleeEnabled(false);
        summon->GetMotionMaster()->MovePoint(POINT_ABOVE_TARGET, pos, FORCED_MOVEMENT_RUN);
        if (target)
            summon->AddThreat(target, 0.f);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        Pet* gargoyle = caster->FindGuardianWithEntry(NPC_GARGOYLE_DK);
        aura->GetTarget()->CastSpell(gargoyle, SPELL_TAUNT_GARGOYLE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct GargoyleDeathKnightAI : public CombatAI
{
    GargoyleDeathKnightAI(Creature* creature) : CombatAI(creature, 0)
    {
        if (creature->GetCreatureInfo()->SpellList)
            creature->SetSpellList(creature->GetCreatureInfo()->SpellList);
        SetRangedMode(true, 40.f, TYPE_FULL_CASTER);
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == POINT_MOTION_TYPE && data == POINT_ABOVE_TARGET)
        {
            SetCombatScriptStatus(false);
            m_creature->SetHover(true);
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        }
    }
};

void LoadDeathKnightScripts()
{
    RegisterSpellScript<ScourgeStrike>("spell_scourge_strike");
    RegisterSpellScript<RaiseDeadDk>("spell_dk_raise_dead");
    RegisterSpellScript<DeathCoilDK>("spell_dk_death_coil");
    RegisterSpellScript<UnholyBlightDK>("spell_dk_unholy_blight");
    RegisterSpellScript<DeathRuneDK>("spell_death_rune_dk");
    RegisterSpellScript<Bloodworm>("spell_bloodworm");
    RegisterSpellScript<HealthLeechPassive>("spell_health_leech_passive");
    RegisterSpellScript<AntiMagicZone>("spell_anti_magic_zone");
    RegisterSpellScript<CorpseExplosionDK>("spell_dk_corpse_explosion");
    RegisterSpellScript<ExplodeGhoulCorpseExplosion>("spell_explode_ghoul_corpse_explosion");
    RegisterSpellScript<DeathKnightDisease>("spell_death_knight_disease");
    RegisterSpellScript<CryptFeverServerside>("spell_crypt_fever_serverside");
    RegisterSpellScript<ArmyOfTheDead>("spell_army_of_the_dead");
    RegisterSpellScript<ArmyOfTheDeadGhoul>("spell_army_of_the_dead_ghoul");
    RegisterSpellScript<SuddenDoom>("spell_sudden_doom");
    RegisterSpellScript<WillOfTheNecropolis>("spell_will_of_the_necropolis");
    RegisterSpellScript<DancingRuneWeapon>("spell_dancing_rune_weapon");
    RegisterSpellScript<FakeAggroRadius8YD>("spell_fake_aggro_radius_8yd");
    RegisterSpellScript<AggroRadius8YD>("spell_aggro_radius_8yd");
    RegisterSpellScript<SummonGargoyle>("spell_summon_gargoyle");

    Script* pNewScript = new Script;
    pNewScript->Name = "npc_gargoyle_dk";
    pNewScript->GetAI = &GetNewAIInstance<GargoyleDeathKnightAI>;
    pNewScript->RegisterSelf();
}