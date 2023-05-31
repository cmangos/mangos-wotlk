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

// TODO: Implement WOLTK only scaling auras

enum
{
    SPELL_PET_RESISTANCE = 37386,
    SPELL_INCREASED_ATTACK_POWER = 38297,
};

struct HunterPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        Unit* target = data.target;
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                {
                    float coeff = 1.f;
                    if (target->HasSpell(62762))
                        coeff = 1.4f;
                    else if (target->HasSpell(62758))
                        coeff = 1.2f;
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f * coeff;
                    if (owner->HasAura(SPELL_INCREASED_ATTACK_POWER))
                        value += 52;
                }
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    float coeff = 1.f;
                    if (target->HasSpell(62762))
                        coeff = 1.3f;
                    else if (target->HasSpell(62758))
                        coeff = 1.15f;
                    value = owner->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.25f * coeff;
                    if (owner->HasAura(SPELL_INCREASED_ATTACK_POWER))
                        value += 70;
                }
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    float coeff = 1.f;
                    if (target->HasSpell(62762))
                        coeff = 1.3f;
                    else if (target->HasSpell(62758))
                        coeff = 1.15f;
                    value = owner->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.125f * coeff;
                }
                break;
        }
        return value;
    }
};

struct HunterPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct HunterPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // armor
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = owner->GetArmor() * 0.35f;
                    if (owner->HasAura(SPELL_INCREASED_ATTACK_POWER))
                        value += 490;
                }
                break;
        }
        return value;
    }
};

struct HunterPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // hit chance
                break;
            case EFFECT_INDEX_1: // spell hit chance
                break;
            case EFFECT_INDEX_2: // expertise
                break;
        }
        return value;
    }
};

struct HunterSnakeTrapScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct WarlockPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 fire = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FIRE);
                        int32 shadow = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW);
                        int32 maximum = (fire > shadow) ? fire : shadow;
                        if (maximum < 0)
                            maximum = 0;
                        value = maximum * 0.57f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 fire = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FIRE);
                        int32 shadow = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW);
                        int32 maximum = (fire > shadow) ? fire : shadow;
                        if (maximum < 0)
                            maximum = 0;
                        value = maximum * 0.15f;
                    }
                }
                break;
        }
        return value;
    }
};

struct WarlockPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
        }
        return value;
    }
};

struct WarlockPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
        }
        return value;
    }
};

struct WarlockPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_1: // mana regen
                if (Unit* owner = data.caster->GetOwner())
                    if (owner->IsPlayer())
                        value = static_cast<Player*>(owner)->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER) * 0.5f * 5.f;
                break;
            default: break;
        }
        return value;
    }
};

struct WarlockPetScaling5 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // hit chance
                break;
            case EFFECT_INDEX_1: // spell hit chance
                break;
            case EFFECT_INDEX_2: // expertise
                break;
        }
        return value;
    }
};

struct MagePetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // attack power
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 frost = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FROST)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FROST);
                        if (frost < 0)
                            frost = 0;
                        value = frost * 0.4f;
                    }
                }
                break;
        }
        return value;
    }
};

struct MagePetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
        }
        return value;
    }
};

struct MagePetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct MagePetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            default: break;
        }
        return value;
    }
};

struct PriestPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 shadow = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW);
                        if (shadow < 0)
                            shadow = 0;
                        value = shadow * 0.65f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                break;
        }
        return value;
    }
};

struct PriestPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
        }
        return value;
    }
};

struct PriestPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct PriestPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            default: break;
        }
        return value;
    }
};

struct ElementalPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // dummy - unk
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 nature = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE);
                        if (nature < 0)
                            nature = 0;
                        value = nature * 0.57f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 nature = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE);
                        if (nature < 0)
                            nature = 0;
                        value = nature * 0.65f;
                    }
                }
                break;
        }
        return value;
    }
};

struct ElementalPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
        }
        return value;
    }
};

struct ElementalPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct ElementalPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            default: break;
        }
        return value;
    }
};

struct DruidPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.15f; // unconfirmed value
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 nature = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE);
                        if (nature < 0)
                            nature = 0;
                        value = nature * 0.5f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                break;
        }
        return value;
    }
};

struct DruidPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
        }
        return value;
    }
};

struct DruidPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct DruidPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            default: break;
        }
        return value;
    }
};

struct FeralSpiritPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 nature = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE);
                        if (nature < 0)
                            nature = 0;
                        value = nature * 0.5f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 nature = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE);
                        if (nature < 0)
                            nature = 0;
                        value = nature * 0.65f;
                    }
                }
                break;
        }
        return value;
    }
};

struct FeralSpiritPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                break;
        }
        return value;
    }
};

struct FeralSpiritPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                break;
            case EFFECT_INDEX_2: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
        }
        return value;
    }
};

struct FeralSpiritPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // hit chance
                break;
            case EFFECT_INDEX_1: // spell hit chance
                break;
            case EFFECT_INDEX_2: // damage percent done
                break;
        }
        return value;
    }
};

// when tweaking these also need to adjust serverside aura types
struct EarthElementalPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct EarthElementalPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct EarthElementalPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct EarthElementalPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct InfernalPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // attack power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 fire = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FIRE);
                        int32 shadow = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW);
                        int32 maximum = (fire > shadow) ? fire : shadow;
                        if (maximum < 0)
                            maximum = 0;
                        value = maximum * 0.57f;
                    }
                }
                break;
            case EFFECT_INDEX_2: // spell power
                if (Unit* owner = data.caster->GetOwner())
                {
                    if (owner->IsPlayer())
                    {
                        int32 fire = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FIRE);
                        int32 shadow = int32(owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW)) - owner->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW);
                        int32 maximum = (fire > shadow) ? fire : shadow;
                        if (maximum < 0)
                            maximum = 0;
                        value = maximum * 0.15f;
                    }
                }
                break;
        }
        return value;
    }
};

struct InfernalPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // intelect
                if (Unit* owner = data.caster->GetOwner())
                    value = float(owner->GetStat(STAT_STAMINA)) * 0.3f;
                break;
            case EFFECT_INDEX_1: // armor
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetArmor() * 0.35f;
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_FIRE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
        }
        return value;
    }
};

struct InfernalPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_FROST) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_1: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_ARCANE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_2: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_NATURE) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
        }
        return value;
    }
};

struct InfernalPetScaling4 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // resistance
                if (Unit* owner = data.caster->GetOwner())
                {
                    value = (owner->GetResistance(SPELL_SCHOOL_SHADOW) * 0.4f);
                    if (owner->HasAura(SPELL_PET_RESISTANCE))
                        value += 130;
                }
                break;
            case EFFECT_INDEX_1: // mana regen
                if (Unit* owner = data.caster->GetOwner())
                    if (owner->IsPlayer())
                        value = static_cast<Player*>(owner)->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER) * 0.5f * 5.f;
                break;
            default: break;
        }
        return value;
    }
};

struct DeathKnightPetScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // stamina
            {
                float coeff = 0.3f;
                if (Unit* owner = data.caster->GetOwner())
                {
                    for (uint32 auraId : {49572, 49571, 48965})
                    {
                        if (Aura* aura = owner->GetAura(auraId, EFFECT_INDEX_0))
                        {
                            coeff = coeff * (100 + aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1)) / 100;
                            break;
                        }
                    }
                    if (Aura* aura = owner->GetAura(58686, EFFECT_INDEX_0)) // Glyph of the Ghoul
                        coeff += float(aura->GetAmount()) / 100;
                    value = float(owner->GetStat(STAT_STAMINA)) * coeff;
                }
                break;
            }
            case EFFECT_INDEX_1: // strength
            {
                float coeff = 0.7f;
                if (Unit* owner = data.caster->GetOwner())
                {
                    for (uint32 auraId : {49572, 49571, 48965})
                    {
                        if (Aura* aura = owner->GetAura(auraId, EFFECT_INDEX_0))
                        {
                            coeff = coeff * (100 + aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1)) / 100;
                            break;
                        }
                    }
                    if (Aura* aura = owner->GetAura(58686, EFFECT_INDEX_0)) // Glyph of the Ghoul
                        coeff += float(aura->GetAmount()) / 100;
                    value = float(owner->GetStat(STAT_STRENGTH)) * coeff;
                }
                break;
            }
            case EFFECT_INDEX_2: // spell damage
                if (Unit* owner = data.caster->GetOwner())
                    value = owner->GetTotalAttackPowerValue(BASE_ATTACK) * 0.125f;
                break;
        }
        return value;
    }
};

struct DeathKnightPetScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // damage percent done
                break;
            case EFFECT_INDEX_1: // melee slow
                if (Unit* owner = data.caster->GetOwner())
                {
                    // For others recalculate it from:
                    float hasteMelee = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    hasteMelee += (1 - owner->m_modAttackSpeedPct[BASE_ATTACK]) * 100;
                    value += int32(hasteMelee);
                }
                break;
            case EFFECT_INDEX_2: // mechanic immunity
                break;
        }
        return value;
    }
};

struct DeathKnightPetScaling3 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // hit chance
                if (Player* owner = dynamic_cast<Player*>(data.caster->GetOwner()))
                {
                    // For others recalculate it from:
                    float hitMelee = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    hitMelee += owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
                    // Increase hit melee from meele hit ratings
                    hitMelee += owner->GetRatingBonusValue(CR_HIT_MELEE);
                    value += int32(hitMelee);
                }
                break;
            case EFFECT_INDEX_1: // spell hit chance
                if (Player* owner = dynamic_cast<Player*>(data.caster->GetOwner()))
                {
                    // For others recalculate it from:
                    float hitSpell = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_SPELL_HIT_CHANCE
                    hitSpell += owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE);
                    // Increase hit spell from spell hit ratings
                    hitSpell += owner->GetRatingBonusValue(CR_HIT_SPELL);

                    value += int32(hitSpell);
                }
                break;
            case EFFECT_INDEX_2: // mechanic immunity
                break;
        }
        return value;
    }
};

// when tweaking these also need to adjust serverside aura types
struct DeathKnightRuneWeaponScaling1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // unk
                break;
            case EFFECT_INDEX_1: // unk
                break;
            case EFFECT_INDEX_2: // unk
                break;
        }
        return value;
    }
};

struct DeathKnightRuneWeaponScaling2 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // damage done
                break;
            case EFFECT_INDEX_1: // melee slow
                if (Unit* owner = data.caster->GetOwner())
                {
                    // For others recalculate it from:
                    float hasteMelee = 0.0f;
                    // Increase hit from SPELL_AURA_MOD_HIT_CHANCE
                    hasteMelee += (1 - owner->m_modAttackSpeedPct[BASE_ATTACK]) * 100;
                    value += int32(hasteMelee);
                }
                break;
            case EFFECT_INDEX_2: // damage percent done
                break;
        }
        return value;
    }
};

struct DeathKnightArmyOfTheDead1 : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // Unk
                break;
            case EFFECT_INDEX_1: // Unk
                break;
            case EFFECT_INDEX_2: // Unk
                break;
        }
        return value;
    }
};

struct DeathKnightBloodParasitePetScaling : public AuraScript
{
    int32 OnAuraValueCalculate(AuraCalcData& data, int32 value) const override
    {
        switch (data.effIdx)
        {
            case EFFECT_INDEX_0: // Unk
                break;
            case EFFECT_INDEX_1: // Unk
                break;
            case EFFECT_INDEX_2: // Unk
                break;
        }
        return value;
    }
};

void LoadScalingScripts()
{
    RegisterSpellScript<HunterPetScaling1>("spell_hunter_pet_scaling_1");
    RegisterSpellScript<HunterPetScaling2>("spell_hunter_pet_scaling_2");
    RegisterSpellScript<HunterPetScaling3>("spell_hunter_pet_scaling_3");
    RegisterSpellScript<HunterPetScaling4>("spell_hunter_pet_scaling_4");

    RegisterSpellScript<HunterSnakeTrapScaling1>("spell_hunter_snake_trap_scaling_1");

    RegisterSpellScript<WarlockPetScaling1>("spell_warlock_pet_scaling_1");
    RegisterSpellScript<WarlockPetScaling2>("spell_warlock_pet_scaling_2");
    RegisterSpellScript<WarlockPetScaling3>("spell_warlock_pet_scaling_3");
    RegisterSpellScript<WarlockPetScaling4>("spell_warlock_pet_scaling_4");
    RegisterSpellScript<WarlockPetScaling5>("spell_warlock_pet_scaling_5");

    RegisterSpellScript<MagePetScaling1>("spell_mage_pet_scaling_1");
    RegisterSpellScript<MagePetScaling2>("spell_mage_pet_scaling_2");
    RegisterSpellScript<MagePetScaling3>("spell_mage_pet_scaling_3");
    RegisterSpellScript<MagePetScaling4>("spell_mage_pet_scaling_4");

    RegisterSpellScript<PriestPetScaling1>("spell_priest_pet_scaling_1");
    RegisterSpellScript<PriestPetScaling2>("spell_priest_pet_scaling_2");
    RegisterSpellScript<PriestPetScaling3>("spell_priest_pet_scaling_3");
    RegisterSpellScript<PriestPetScaling4>("spell_priest_pet_scaling_4");

    RegisterSpellScript<ElementalPetScaling1>("spell_elemental_pet_scaling_1");
    RegisterSpellScript<ElementalPetScaling2>("spell_elemental_pet_scaling_2");
    RegisterSpellScript<ElementalPetScaling3>("spell_elemental_pet_scaling_3");
    RegisterSpellScript<ElementalPetScaling4>("spell_elemental_pet_scaling_4");

    RegisterSpellScript<DruidPetScaling1>("spell_druid_pet_scaling_1");
    RegisterSpellScript<DruidPetScaling2>("spell_druid_pet_scaling_2");
    RegisterSpellScript<DruidPetScaling3>("spell_druid_pet_scaling_3");
    RegisterSpellScript<DruidPetScaling4>("spell_druid_pet_scaling_4");

    RegisterSpellScript<FeralSpiritPetScaling1>("spell_feral_spirit_pet_scaling_1");
    RegisterSpellScript<FeralSpiritPetScaling2>("spell_feral_spirit_pet_scaling_2");
    RegisterSpellScript<FeralSpiritPetScaling3>("spell_feral_spirit_pet_scaling_3");
    RegisterSpellScript<FeralSpiritPetScaling4>("spell_feral_spirit_pet_scaling_4");

    RegisterSpellScript<EarthElementalPetScaling1>("spell_earth_elemental_pet_scaling_1");
    RegisterSpellScript<EarthElementalPetScaling2>("spell_earth_elemental_pet_scaling_2");
    RegisterSpellScript<EarthElementalPetScaling3>("spell_earth_elemental_pet_scaling_3");
    RegisterSpellScript<EarthElementalPetScaling4>("spell_earth_elemental_pet_scaling_4");

    RegisterSpellScript<InfernalPetScaling1>("spell_infernal_pet_scaling_1");
    RegisterSpellScript<InfernalPetScaling2>("spell_infernal_pet_scaling_2");
    RegisterSpellScript<InfernalPetScaling3>("spell_infernal_pet_scaling_3");
    RegisterSpellScript<InfernalPetScaling4>("spell_infernal_pet_scaling_4");

    RegisterSpellScript<DeathKnightPetScaling1>("spell_death_knight_pet_scaling_1");
    RegisterSpellScript<DeathKnightPetScaling2>("spell_death_knight_pet_scaling_2");
    RegisterSpellScript<DeathKnightPetScaling3>("spell_death_knight_pet_scaling_3");

    RegisterSpellScript<DeathKnightRuneWeaponScaling1>("spell_death_knight_rune_weapon_scaling_1");
    RegisterSpellScript<DeathKnightRuneWeaponScaling2>("spell_death_knight_rune_weapon_scaling_2");

    RegisterSpellScript<DeathKnightArmyOfTheDead1>("spell_death_knight_army_of_the_dead_1");

    RegisterSpellScript<DeathKnightBloodParasitePetScaling>("spell_death_knight_blood_parasite_pet_scaling");
}