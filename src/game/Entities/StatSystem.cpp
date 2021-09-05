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

#include "Entities/Unit.h"
#include "Entities/Player.h"
#include "Entities/Pet.h"
#include "Entities/Creature.h"
#include "Globals/SharedDefines.h"
#include "Spells/SpellAuras.h"

/*#######################################
########                         ########
########   PLAYERS STAT SYSTEM   ########
########                         ########
#######################################*/

bool Player::UpdateStats(Stats stat)
{
    if (stat > STAT_SPIRIT)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);

    int32 oldValue = GetStat(stat);
    SetStat(stat, int32(value));

    if (oldValue != value)
        if (stat == STAT_STAMINA || stat == STAT_INTELLECT)
            if (Pet* pet = GetPet())
                pet->UpdateScalingAuras();

    switch (stat)
    {
        case STAT_STRENGTH:
            UpdateShieldBlockValue();
            break;
        case STAT_AGILITY:
            UpdateArmor();
            UpdateAllCritPercentages();
            UpdateDodgePercentage();
            break;
        case STAT_STAMINA:   UpdateMaxHealth(); break;
        case STAT_INTELLECT:
            UpdateMaxPower(POWER_MANA);
            UpdateAllSpellCritChances();
            UpdateArmor();                                  // SPELL_AURA_MOD_RESISTANCE_OF_INTELLECT_PERCENT, only armor currently
            break;

        case STAT_SPIRIT:
            break;

        default:
            break;
    }
    // Need update (exist AP from stat auras)
    UpdateAttackPowerAndDamage();
    UpdateAttackPowerAndDamage(true);

    UpdateSpellHealingBonus();
    UpdateSpellDamageBonus();
    UpdateManaRegen();

    // Update ratings in exist SPELL_AURA_MOD_RATING_FROM_STAT and only depends from stat
    uint32 mask = 0;
    AuraList const& modRatingFromStat = GetAurasByType(SPELL_AURA_MOD_RATING_FROM_STAT);
    for (auto i : modRatingFromStat)
        if (Stats(i->GetMiscBValue()) == stat)
            mask |= i->GetMiscValue();
    if (mask)
    {
        for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
            if (mask & (1 << rating))
                ApplyRatingMod(CombatRating(rating), 0, true);
    }
    return true;
}

void Player::ApplySpellPowerBonus(int32 amount, bool apply)
{
    m_baseSpellPower += apply ? amount : -amount;

    // For speed just update for client
    ApplyModUInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, amount, apply);
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, amount, apply);
}

void Player::UpdateSpellHealingBonus()
{
    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get healing bonus for all schools
    SetStatInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_ALL));
}

void Player::UpdateSpellDamageBonus()
{
    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get damage bonus for all schools
    for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        SetStatInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, SpellBaseDamageBonusDone(SpellSchoolMask(1 << i)));

    if (Pet* pet = GetPet())
        pet->UpdateScalingAuras();
}

bool Player::UpdateAllStats()
{
    for (int i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        float value = GetTotalStatValue(Stats(i));
        SetStat(Stats(i), (int32)value);
    }

    UpdateArmor();
    // calls UpdateAttackPowerAndDamage() in UpdateArmor for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    UpdateAttackPowerAndDamage(true);
    UpdateMaxHealth();

    for (int i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    UpdateAllRatings();
    UpdateAllCritPercentages();
    UpdateAllSpellCritChances();
    UpdateDefenseBonusesMod();
    UpdateShieldBlockValue();
    UpdateArmorPenetration();
    UpdateSpellHealingBonus();
    UpdateSpellDamageBonus();
    UpdateManaRegen();
    UpdateExpertise(BASE_ATTACK);
    UpdateExpertise(OFF_ATTACK);
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Player::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        int32 value = GetTotalResistanceValue(SpellSchools(school));
        int32 oldValue = GetResistance(SpellSchools(school));
        SetResistance(SpellSchools(school), value);

        if (value != oldValue)
            if (Pet* pet = GetPet())
                pet->UpdateScalingAuras();
    }
    else
        UpdateArmor();
}

void Player::UpdateArmor()
{
    float dynamic = (GetStat(STAT_AGILITY) * 2.0f);

    // Add dynamic flat mods
    for (auto& i : GetAurasByType(SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT))
    {
        if (Modifier* mod = i->GetModifier())
        {
            if (mod->m_miscvalue & SPELL_SCHOOL_MASK_NORMAL)
                dynamic += (GetStat(Stats(i->GetMiscBValue())) * (mod->m_amount * 0.01f));
        }
    }

    m_auraModifiersGroup[UNIT_MOD_ARMOR][TOTAL_VALUE] += dynamic;
    int32 value = GetTotalResistanceValue(SPELL_SCHOOL_NORMAL);
    int32 oldValue = GetArmor();
    SetArmor(value);
    m_auraModifiersGroup[UNIT_MOD_ARMOR][TOTAL_VALUE] -= dynamic;

    if (value != oldValue)
        if (Pet* pet = GetPet())
            pet->UpdateScalingAuras();
    
    UpdateAttackPowerAndDamage();                           // armor dependent auras update for SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
}

float Player::GetHealthBonusFromStamina() const
{
    float stamina = GetStat(STAT_STAMINA);

    float baseStam = stamina < 20 ? stamina : 20;
    float moreStam = stamina - baseStam;

    return baseStam + (moreStam * 10.0f);
}

float Player::GetManaBonusFromIntellect() const
{
    float intellect = GetStat(STAT_INTELLECT);

    float baseInt = intellect < 20 ? intellect : 20;
    float moreInt = intellect - baseInt;

    return baseInt + (moreInt * 15.0f);
}

void Player::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + GetHealthBonusFromStamina();
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxHealth((uint32)value);
}

void Player::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    uint32 create_power = GetCreatePowers(power);

    // ignore classes without mana
    float bonusPower = (power == POWER_MANA && create_power > 0) ? GetManaBonusFromIntellect() : 0;

    float value = GetModifierValue(unitMod, BASE_VALUE) + create_power;
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) +  bonusPower;
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxPower(power, uint32(value));
}

void Player::ApplyFeralAPBonus(int32 amount, bool apply)
{
    m_baseFeralAP += apply ? amount : -amount;
    UpdateAttackPowerAndDamage();
}

void Player::UpdateAttackPowerAndDamage(bool ranged)
{
    float val2 = 0.0f;
    float level = float(GetLevel());

    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mod = UNIT_FIELD_ATTACK_POWER_MODS;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mod = UNIT_FIELD_RANGED_ATTACK_POWER_MODS;
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;

        switch (getClass())
        {
            case CLASS_HUNTER: val2 = level * 2.0f + GetStat(STAT_AGILITY) - 10.0f;    break;
            case CLASS_ROGUE:  val2 = level        + GetStat(STAT_AGILITY) - 10.0f;    break;
            case CLASS_WARRIOR: val2 = level        + GetStat(STAT_AGILITY) - 10.0f;    break;
            case CLASS_DRUID:
                switch (GetShapeshiftForm())
                {
                    case FORM_CAT:
                    case FORM_BEAR:
                    case FORM_DIREBEAR:
                        val2 = 0.0f; break;
                    default:
                        val2 = GetStat(STAT_AGILITY) - 10.0f; break;
                }
                break;
            default: val2 = GetStat(STAT_AGILITY) - 10.0f; break;
        }
    }
    else
    {
        switch (getClass())
        {
            case CLASS_WARRIOR:      val2 = level * 3.0f + GetStat(STAT_STRENGTH) * 2.0f                    - 20.0f; break;
            case CLASS_PALADIN:      val2 = level * 3.0f + GetStat(STAT_STRENGTH) * 2.0f                    - 20.0f; break;
            case CLASS_DEATH_KNIGHT: val2 = level * 3.0f + GetStat(STAT_STRENGTH) * 2.0f                  - 20.0f; break;
            case CLASS_ROGUE:        val2 = level * 2.0f + GetStat(STAT_STRENGTH) + GetStat(STAT_AGILITY) - 20.0f; break;
            case CLASS_HUNTER:       val2 = level * 2.0f + GetStat(STAT_STRENGTH) + GetStat(STAT_AGILITY) - 20.0f; break;
            case CLASS_SHAMAN:       val2 = level * 2.0f + GetStat(STAT_STRENGTH) + GetStat(STAT_AGILITY) - 20.0f; break;
            case CLASS_DRUID:
            {
                val2 = level * 3.0f + GetStat(STAT_STRENGTH) * 2.0f - 20.0f;
                if (ShapeshiftForm form = GetShapeshiftForm())
                    if (SpellShapeshiftFormEntry const* entry = sSpellShapeshiftFormStore.LookupEntry(form))
                        if (entry->flags1 & SHAPESHIFT_FLAG_AGILITY_ATTACK_BONUS)
                            val2 += GetStat(STAT_AGILITY);
                break;
            }
            case CLASS_MAGE:    val2 =              GetStat(STAT_STRENGTH)                         - 10.0f; break;
            case CLASS_PRIEST:  val2 =              GetStat(STAT_STRENGTH)                         - 10.0f; break;
            case CLASS_WARLOCK: val2 =              GetStat(STAT_STRENGTH)                         - 10.0f; break;
        }
    }

    SetModifierValue(unitMod, BASE_VALUE, val2);

    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);

    // add dynamic flat mods
    if (ranged)
    {
        if ((getClassMask() & CLASSMASK_WAND_USERS) == 0)
        {
            AuraList const& mRAPbyStat = GetAurasByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT);
            for (auto i : mRAPbyStat)
                attPowerMod += int32(GetStat(Stats(i->GetModifier()->m_miscvalue)) * i->GetModifier()->m_amount / 100.0f);
        }
    }
    else
    {
        AuraList const& mAPbyStat = GetAurasByType(SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT);
        for (auto i : mAPbyStat)
            attPowerMod += int32(GetStat(Stats(i->GetModifier()->m_miscvalue)) * i->GetModifier()->m_amount / 100.0f);

        AuraList const& mAPbyArmor = GetAurasByType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR);
        for (auto iter : mAPbyArmor)
            // always: ((*i)->GetModifier()->m_miscvalue == 1 == SPELL_SCHOOL_MASK_NORMAL)
            attPowerMod += int32(GetArmor() / iter->GetModifier()->m_amount);
    }

    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    SetInt32Value(index, (uint32)base_attPower);            // UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(index_mod, (uint32)attPowerMod);          // UNIT_FIELD_(RANGED)_ATTACK_POWER_MODS field
    SetFloatValue(index_mult, attPowerMultiplier);          // UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field

    // automatically update weapon damage after attack power modification
    if (ranged)
    {
        UpdateDamagePhysical(RANGED_ATTACK);

        if (Pet* pet = GetPet()) // update pet's AP
            pet->UpdateScalingAuras();
    }
    else
    {
        UpdateDamagePhysical(BASE_ATTACK);
        if (CanDualWield() && hasOffhandWeaponForAttack())          // allow update offhand damage only if player knows DualWield Spec and has equipped offhand weapon
            UpdateDamagePhysical(OFF_ATTACK);
    }
}

void Player::UpdateShieldBlockValue()
{
    SetUInt32Value(PLAYER_SHIELD_BLOCK, GetShieldBlockValue());
}

void Player::SetEnchantmentModifier(uint32 value, WeaponAttackType attType, bool apply)
{
    if (apply)
        m_enchantmentFlatMod[attType] += value;
    else
        m_enchantmentFlatMod[attType] -= value;
}

uint32 Player::GetEnchantmentModifier(WeaponAttackType attType)
{
    return m_enchantmentFlatMod[attType];
}

void Player::CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, float& min_damage, float& max_damage, uint8 index)
{
    UnitMods unitMod;

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
    }

    float att_speed = GetAPMultiplier(attType, normalized);

    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + GetTotalAttackPowerValue(attType) / 14.0f * att_speed;
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = GetModifierValue(unitMod, TOTAL_PCT);

    float weapon_mindamage = GetBaseWeaponDamage(attType, MINDAMAGE, index);
    float weapon_maxdamage = GetBaseWeaponDamage(attType, MAXDAMAGE, index);

    if (IsNoWeaponShapeShift())                             // check if player is in shapeshift which doesnt use weapon
    {
        uint32 lvl = GetLevel();
        if (lvl > 60)
            lvl = 60;

        weapon_mindamage = lvl * 0.85f * att_speed;
        weapon_maxdamage = lvl * 1.25f * att_speed;
    }
    else if (!CanUseEquippedWeapon(attType))                // check if player not in form but still can't use weapon (broken/etc)
    {
        weapon_mindamage = BASE_MINDAMAGE;
        weapon_maxdamage = BASE_MAXDAMAGE;
    }
    else
    {
        total_value += GetEnchantmentModifier(attType);
        if (attType == RANGED_ATTACK)                      // add ammo DPS to ranged damage
        {
            weapon_mindamage += GetAmmoDPS() * att_speed;
            weapon_maxdamage += GetAmmoDPS() * att_speed;
        }

        if (index != 0)
        {
            base_value = 0.0f;
            total_value = 0.0f;
        }
    }

    min_damage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    max_damage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;
}

void Player::UpdateDamagePhysical(WeaponAttackType attType)
{
    float mindamage;
    float maxdamage;

    CalculateMinMaxDamage(attType, false, mindamage, maxdamage);

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            SetStatFloatValue(UNIT_FIELD_MINDAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAXDAMAGE, maxdamage);
            break;
        case OFF_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, maxdamage);
            break;
        case RANGED_ATTACK:
            SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, mindamage);
            SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, maxdamage);
            break;
    }
}

void Player::UpdateDefenseBonusesMod()
{
    UpdateBlockPercentage();
    UpdateParryPercentage();
    UpdateDodgePercentage();
}

void Player::UpdateBlockPercentage()
{
    float value = 0.0f;
    float real = 0.0f;
    if (CanBlock())
    {
        // Base value
        value = 5.0f;
        // Increase from SPELL_AURA_MOD_BLOCK_PERCENT aura
        value += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
        // Increase from rating
        value += GetRatingBonusValue(CR_BLOCK);
        real = value;
        // Set UI display value: modify value from defense skill against same level target
        value += (int32(GetDefenseSkillValue()) - int32(GetSkillMaxForLevel())) * 0.04f;
    }
    m_modBlockChance = real;
    SetStatFloatValue(PLAYER_BLOCK_PERCENTAGE, std::max(0.0f, std::min(value, 100.0f)));
}

void Player::UpdateCritPercentage(WeaponAttackType attType)
{
    BaseModGroup modGroup;
    uint16 index;
    CombatRating cr;

    switch (attType)
    {
        case OFF_ATTACK:
            modGroup = OFFHAND_CRIT_PERCENTAGE;
            index = PLAYER_OFFHAND_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
        case RANGED_ATTACK:
            modGroup = RANGED_CRIT_PERCENTAGE;
            index = PLAYER_RANGED_CRIT_PERCENTAGE;
            cr = CR_CRIT_RANGED;
            break;
        case BASE_ATTACK:
        default:
            modGroup = CRIT_PERCENTAGE;
            index = PLAYER_CRIT_PERCENTAGE;
            cr = CR_CRIT_MELEE;
            break;
    }

    float value = GetTotalPercentageModValue(modGroup) + GetRatingBonusValue(cr);
    m_modCritChance[attType] = value;
    // Modify crit from weapon skill and maximized defense skill of same level victim difference
    value += (int32(GetWeaponSkillValue(attType)) - int32(GetSkillMaxForLevel())) * 0.04f;
    SetStatFloatValue(index, std::max(0.0f, std::min(value, 100.0f)));
}

void Player::UpdateAllCritPercentages()
{
    float value = GetMeleeCritFromAgility();

    SetBaseModValue(CRIT_PERCENTAGE, PCT_MOD, value);
    SetBaseModValue(OFFHAND_CRIT_PERCENTAGE, PCT_MOD, value);
    SetBaseModValue(RANGED_CRIT_PERCENTAGE, PCT_MOD, value);

    UpdateCritPercentage(BASE_ATTACK);
    UpdateCritPercentage(OFF_ATTACK);
    UpdateCritPercentage(RANGED_ATTACK);
}

void Player::UpdateParryPercentage()
{
    float value = 0.0f;
    float real = 0.0f;
    if (CanParry())
    {
        // Base parry
        value  = 5.0f;
        // Parry from SPELL_AURA_MOD_PARRY_PERCENT aura
        value += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT);
        // Parry from rating
        value += GetRatingBonusValue(CR_PARRY);
        real = value;
        // Set value for diminishing when in combat
        m_modDodgeChanceDiminishing = GetRatingBonusValue(CR_PARRY);
        // Set UI display value: modify value from defense skill against same level target
        value += (int32(GetDefenseSkillValue()) - int32(GetSkillMaxForLevel())) * 0.04f;
    }
    // Set current dodge chance
    m_modParryChance = real;
    SetStatFloatValue(PLAYER_PARRY_PERCENTAGE, std::max(0.0f, std::min(value, 100.0f)));
}

// Base static dodge values in percentages (%)
static const float PLAYER_BASE_DODGE[MAX_CLASSES] =
{
    0.0000f, // [0]  <Unused>
    3.6640f, // [1]  Warrior
    3.4943f, // [2]  Paladin
    -4.0873f, // [3]  Hunter
    2.0957f, // [4]  Rogue
    3.4178f, // [5]  Priest
    3.6640f, // [6]  DK
    2.1080f, // [7]  Shaman
    3.6587f, // [8]  Mage
    2.4211f, // [9]  Warlock
    0.0000f, // [10] <Unused>
    5.6097f, // [11] Druid
};

void Player::UpdateDodgePercentage()
{
    // Base dodge
    float value = (getClass() < MAX_CLASSES) ? PLAYER_BASE_DODGE[getClass()] : 0.0f;
    // Dodge from agility
    value += GetDodgeFromAgility(GetStat(STAT_AGILITY));
    // Dodge from SPELL_AURA_MOD_DODGE_PERCENT aura
    value += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
    // Dodge from rating
    value += GetRatingBonusValue(CR_DODGE);
    // Set current dodge chance
    m_modDodgeChance = value;
    // Set value for diminishing when in combat
    m_modDodgeChanceDiminishing = GetDodgeFromAgility((GetStat(STAT_AGILITY) - (GetCreateStat(STAT_AGILITY) * m_auraModifiersGroup[UNIT_MOD_STAT_START + STAT_AGILITY][BASE_PCT])));
    m_modDodgeChanceDiminishing += GetRatingBonusValue(CR_DODGE);
    // Set UI display value: modify value from defense skill against same level target
    value += (int32(GetDefenseSkillValue()) - int32(GetSkillMaxForLevel())) * 0.04f;
    SetStatFloatValue(PLAYER_DODGE_PERCENTAGE, std::max(0.0f, std::min(value, 100.0f)));
}

void Player::UpdateSpellCritChance(uint32 school)
{
    float crit = 0.0f;
    // Base spell crit and spell crit from Intellect
    crit += GetSpellCritFromIntellect();
    // Increase crit from SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    crit += GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    // Increase crit from SPELL_AURA_MOD_ALL_CRIT_CHANCE
    crit += GetTotalAuraModifier(SPELL_AURA_MOD_ALL_CRIT_CHANCE);
    // Increase crit by school from SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    crit += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, (1 << school));
    // Increase crit from spell crit ratings
    crit += GetRatingBonusValue(CR_CRIT_SPELL);
    // Set current crit chance
    m_modSpellCritChance[school] = crit;
    // Set UI display value:
    SetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + school, std::max(0.0f, std::min(crit, 100.0f)));
}

void Player::UpdateMeleeHitChances()
{
    m_modMeleeHitChance = GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    m_modMeleeHitChance +=  GetRatingBonusValue(CR_HIT_MELEE);
}

void Player::UpdateRangedHitChances()
{
    m_modRangedHitChance = GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE);
    m_modRangedHitChance += GetRatingBonusValue(CR_HIT_RANGED);
}

void Player::UpdateSpellHitChances()
{
    m_modSpellHitChance = GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE);
    m_modSpellHitChance += GetRatingBonusValue(CR_HIT_SPELL);
}

void Player::UpdateAllSpellCritChances()
{
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateSpellCritChance(i);
}

void Player::UpdateExpertise(WeaponAttackType attack)
{
    if (attack == RANGED_ATTACK)
        return;

    int32 expertise = int32(GetRatingBonusValue(CR_EXPERTISE));

    Item* weapon = GetWeaponForAttack(attack);

    AuraList const& expAuras = GetAurasByType(SPELL_AURA_MOD_EXPERTISE);
    for (auto expAura : expAuras)
    {
        // item neutral spell
        if (expAura->GetSpellProto()->EquippedItemClass == -1)
            expertise += expAura->GetModifier()->m_amount;
        // item dependent spell
        else if (weapon && weapon->IsFitToSpellRequirements(expAura->GetSpellProto()))
            expertise += expAura->GetModifier()->m_amount;
    }

    if (expertise < 0)
        expertise = 0;

    switch (attack)
    {
        case BASE_ATTACK: SetUInt32Value(PLAYER_EXPERTISE, expertise);         break;
        case OFF_ATTACK:  SetUInt32Value(PLAYER_OFFHAND_EXPERTISE, expertise); break;
        default: break;
    }
}

void Player::UpdateArmorPenetration()
{
    m_armorPenetrationPct = GetRatingBonusValue(CR_ARMOR_PENETRATION);

    AuraList const& armorAuras = GetAurasByType(SPELL_AURA_MOD_TARGET_ARMOR_PCT);
    for (auto armorAura : armorAuras)
    {
        // affects all weapons
        if (armorAura->GetSpellProto()->EquippedItemClass == -1)
        {
            m_armorPenetrationPct += armorAura->GetModifier()->m_amount;
            continue;
        }

        // dependent on weapon class
        for (uint8 i = 0; i < MAX_ATTACK; ++i)
        {
            Item* weapon = GetWeaponForAttack(WeaponAttackType(i));
            if (weapon && weapon->IsFitToSpellRequirements(armorAura->GetSpellProto()))
            {
                m_armorPenetrationPct += armorAura->GetModifier()->m_amount;
                break;
            }
        }
    }
}

void Player::ApplyManaRegenBonus(int32 amount, bool apply)
{
    m_baseManaRegen += apply ? amount : -amount;
    UpdateManaRegen();
}

void Player::UpdateManaRegen()
{
    // need to award mana based on previous rate - Patch 2.2
    if (GetHealth() > 0) // on death we must never do this
        RegenerateAll();

    float Intellect = GetStat(STAT_INTELLECT);
    // Mana regen from spirit and intellect
    float power_regen = sqrt(Intellect) * OCTRegenMPPerSpirit();
    // Apply PCT bonus from SPELL_AURA_MOD_POWER_REGEN_PERCENT aura on spirit base regen
    power_regen *= GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_POWER_REGEN_PERCENT, POWER_MANA);

    // Mana regen from SPELL_AURA_MOD_POWER_REGEN aura
    float power_regen_mp5 = (GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) + m_baseManaRegen) / 5.0f;

    // Get bonus from SPELL_AURA_MOD_MANA_REGEN_FROM_STAT aura
    AuraList const& regenAura = GetAurasByType(SPELL_AURA_MOD_MANA_REGEN_FROM_STAT);
    for (auto i : regenAura)
    {
        Modifier* mod = i->GetModifier();
        power_regen_mp5 += GetStat(Stats(mod->m_miscvalue)) * mod->m_amount / 500.0f;
    }

    // Set regen rate in cast state apply only on spirit based regen
    int32 modManaRegenInterrupt = GetTotalAuraModifier(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
    if (modManaRegenInterrupt > 100)
        modManaRegenInterrupt = 100;
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, power_regen_mp5 + power_regen * modManaRegenInterrupt / 100.0f);

    float value = power_regen_mp5 + power_regen;
    float oldValue = GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER);
    SetStatFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER, value);

    if (value != oldValue)
        if (Pet* pet = GetPet())
            pet->UpdateScalingAuras();
}

void Player::UpdateEnergyRegen()
{
    // need to award mana based on previous rate - Patch 2.2
    if (GetHealth() > 0) // on death we must never do this
        RegenerateAll();

    m_energyRegenRate = GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_POWER_REGEN_PERCENT, POWER_ENERGY);
}

void Player::_ApplyAllStatBonuses()
{
    SetCanModifyStats(false);

    _ApplyAllAuraMods();
    _ApplyAllItemMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

void Player::_RemoveAllStatBonuses()
{
    SetCanModifyStats(false);

    _RemoveAllItemMods();
    _RemoveAllAuraMods();

    SetCanModifyStats(true);

    UpdateAllStats();
}

/*#######################################
########                         ########
########    MOBS STAT SYSTEM     ########
########                         ########
#######################################*/

bool Creature::UpdateStats(Stats /*stat*/)
{
    return true;
}

bool Creature::UpdateAllStats()
{
    UpdateMaxHealth();
    UpdateAttackPowerAndDamage();
    UpdateAttackPowerAndDamage(true);

    for (int i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Creature::UpdateResistances(uint32 school)
{
    if (school > SPELL_SCHOOL_NORMAL)
    {
        int32 value = GetTotalResistanceValue(SpellSchools(school));
        SetResistance(SpellSchools(school), value);
    }
    else
        UpdateArmor();
}

void Creature::UpdateArmor()
{
    int32 value = GetTotalResistanceValue(SPELL_SCHOOL_NORMAL);
    SetArmor(value);
}

void Creature::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE);
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxHealth((uint32)value);
}

void Creature::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreatePowers(power);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE);
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxPower(power, uint32(value));
}

void Creature::UpdateAttackPowerAndDamage(bool ranged)
{
    UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

    uint16 index = UNIT_FIELD_ATTACK_POWER;
    uint16 index_mod = UNIT_FIELD_ATTACK_POWER_MODS;
    uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

    if (ranged)
    {
        index = UNIT_FIELD_RANGED_ATTACK_POWER;
        index_mod = UNIT_FIELD_RANGED_ATTACK_POWER_MODS;
        index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;
    }

    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    SetInt32Value(index, (uint32)base_attPower);            // UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(index_mod, (uint32)attPowerMod);          // UNIT_FIELD_(RANGED)_ATTACK_POWER_MODS field
    SetFloatValue(index_mult, attPowerMultiplier);          // UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field

    if (ranged)
    {
        UpdateDamagePhysical(RANGED_ATTACK);
        return;
    }

    // automatically update weapon damage after attack power modification
    UpdateDamagePhysical(BASE_ATTACK);
    UpdateDamagePhysical(OFF_ATTACK);
}

void Creature::UpdateDamagePhysical(WeaponAttackType attType)
{
    UnitMods unitMod;
    UnitMods attPower;

    switch (attType)
    {
        case BASE_ATTACK:
        default:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            attPower = UNIT_MOD_ATTACK_POWER;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            attPower = UNIT_MOD_ATTACK_POWER;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            attPower = UNIT_MOD_ATTACK_POWER_RANGED;
            break;
    }

    float att_pwr = (GetModifierValue(attPower, BASE_VALUE) * GetModifierValue(attPower, BASE_PCT) + GetModifierValue(attPower, TOTAL_VALUE)) * GetModifierValue(attPower, TOTAL_PCT);
    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + (att_pwr * GetAPMultiplier(attType, false) / 14.0f);
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = GetModifierValue(unitMod, TOTAL_PCT);

    float weapon_mindamage = GetBaseWeaponDamage(attType, MINDAMAGE);
    float weapon_maxdamage = GetBaseWeaponDamage(attType, MAXDAMAGE);

    float mindamage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    float maxdamage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;

    // Disarm for creatures
    if (hasWeapon(attType) && !hasWeaponForAttack(attType))
    {
        mindamage *= 0.5f;
        maxdamage *= 0.5f;
    }

    uint16 fieldmin, fieldmax;

    switch (attType)
    {
        case RANGED_ATTACK:
            fieldmin = UNIT_FIELD_MINRANGEDDAMAGE;
            fieldmax = UNIT_FIELD_MAXRANGEDDAMAGE;
            break;
        case BASE_ATTACK:
            fieldmin = UNIT_FIELD_MINDAMAGE;
            fieldmax = UNIT_FIELD_MAXDAMAGE;
            break;
        case OFF_ATTACK:
            fieldmin = UNIT_FIELD_MINOFFHANDDAMAGE;
            fieldmax = UNIT_FIELD_MAXOFFHANDDAMAGE;
            break;
        default:
            return;
    }

    SetStatFloatValue(fieldmin, mindamage);
    SetStatFloatValue(fieldmax, maxdamage);
}

/*#######################################
########                         ########
########    PETS STAT SYSTEM     ########
########                         ########
#######################################*/

bool Pet::UpdateStats(Stats stat)
{
    if (stat > STAT_SPIRIT)
        return false;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = GetTotalStatValue(stat);
    
    float oldValue = GetStat(stat);
    SetStat(stat, int32(value));

    switch (stat)
    {
        case STAT_STRENGTH:         UpdateAttackPowerAndDamage();        break;
        case STAT_AGILITY:          UpdateArmor();                       break;
        case STAT_STAMINA:          UpdateMaxHealth();                   break;
        case STAT_INTELLECT:        UpdateMaxPower(POWER_MANA);          break;
        case STAT_SPIRIT:
        default:
            break;
    }

    if (m_inStatsUpdate)
        return true;

    m_inStatsUpdate = true;
    if (oldValue != value)
        if (GetSpellAuraHolder(35696))
            if (stat == STAT_STAMINA || stat == STAT_INTELLECT)
                if (Unit* owner = GetOwner())
                    if (Aura* aura = owner->GetAura(35696, EFFECT_INDEX_0))
                        aura->UpdateAuraScaling();

    m_inStatsUpdate = false;
    return true;
}

bool Pet::UpdateAllStats()
{
    for (int i = STAT_STRENGTH; i < MAX_STATS; ++i)
        UpdateStats(Stats(i));

    for (int i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i));

    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        UpdateResistances(i);

    return true;
}

void Pet::UpdateResistances(uint32 school)
{
    // This override contains current hardcoded implementation for pet scaling (spells since 2.x):
    // 34903 - Hunter Pet Scaling 02
    // 34904 - Hunter Pet Scaling 03
    // 34956 - Warlock Pet Scaling 02
    // 34957 - Warlock Pet Scaling 03
    // 34958 - Warlock Pet Scaling 04

    if (school > SPELL_SCHOOL_NORMAL)
        return Creature::UpdateResistances(school);
    else
        UpdateArmor();
}

void Pet::UpdateArmor()
{
    float amount = (GetStat(STAT_AGILITY) * 2.0f);

    // This override contains current hardcoded implementation for pet scaling (spells since 2.x):
    // 34903 - Hunter Pet Scaling 02
    // 34956 - Warlock Pet Scaling 02

    m_auraModifiersGroup[UNIT_MOD_ARMOR][TOTAL_VALUE] += amount;
    Creature::UpdateArmor();
    m_auraModifiersGroup[UNIT_MOD_ARMOR][TOTAL_VALUE] -= amount;
}

void Pet::UpdateMaxHealth()
{
    UnitMods unitMod = UNIT_MOD_HEALTH;
    float stamina = GetStat(STAT_STAMINA) - GetCreateStat(STAT_STAMINA);

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreateHealth();
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + std::max((stamina - 20) * 10 + 20, 0.f);
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxHealth((uint32)value);
}

void Pet::UpdateMaxPower(Powers power)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float addValue = (power == POWER_MANA) ? GetStat(STAT_INTELLECT) - GetCreateStat(STAT_INTELLECT) : 0.0f;

    float value = GetModifierValue(unitMod, BASE_VALUE) + GetCreatePowers(power);
    value *= GetModifierValue(unitMod, BASE_PCT);
    value += GetModifierValue(unitMod, TOTAL_VALUE) + std::max((addValue - 20) * 15 + 20, 0.f);
    value *= GetModifierValue(unitMod, TOTAL_PCT);

    SetMaxPower(power, uint32(value));
}

void Pet::UpdateAttackPowerAndDamage(bool ranged)
{
    if (ranged)
        return;

    float val = 0.0f;
    UnitMods unitMod = UNIT_MOD_ATTACK_POWER;

    val = 2 * GetStat(STAT_STRENGTH) - 20.0f;

    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, val);

    // in BASE_VALUE of UNIT_MOD_ATTACK_POWER for creatures we store data of meleeattackpower field in DB
    float base_attPower  = GetModifierValue(unitMod, BASE_VALUE) * GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = GetModifierValue(unitMod, TOTAL_VALUE);
    float attPowerMultiplier = GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    // UNIT_FIELD_(RANGED)_ATTACK_POWER field
    SetInt32Value(UNIT_FIELD_ATTACK_POWER, (int32)base_attPower);
    // UNIT_FIELD_(RANGED)_ATTACK_POWER_MODS field
    SetInt32Value(UNIT_FIELD_ATTACK_POWER_MODS, (int32)attPowerMod);
    // UNIT_FIELD_(RANGED)_ATTACK_POWER_MULTIPLIER field
    SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, attPowerMultiplier);

    // automatically update weapon damage after attack power modification
    UpdateDamagePhysical(BASE_ATTACK);
    UpdateDamagePhysical(OFF_ATTACK);
}

void Pet::UpdateDamagePhysical(WeaponAttackType attType)
{
    if (attType > OFF_ATTACK)
        return;

    UnitMods unitMod = UNIT_MOD_DAMAGE_MAINHAND;

    float att_speed = float(GetAttackTime(attType)) / 1000.0f;

    float base_value  = GetModifierValue(unitMod, BASE_VALUE) + GetTotalAttackPowerValue(attType) / 14.0f * att_speed;
    float base_pct    = GetModifierValue(unitMod, BASE_PCT);
    float total_value = GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct   = GetModifierValue(unitMod, TOTAL_PCT);

    float weapon_mindamage = GetBaseWeaponDamage(attType, MINDAMAGE);
    float weapon_maxdamage = GetBaseWeaponDamage(attType, MAXDAMAGE);

    float mindamage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    float maxdamage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;

    //  Pet's base damage changes depending on happiness
    if (getPetType() == HUNTER_PET && attType == BASE_ATTACK)
    {
        switch (GetHappinessState())
        {
            case HAPPY:
                // 125% of normal damage
                mindamage = mindamage * 1.25f;
                maxdamage = maxdamage * 1.25f;
                break;
            case CONTENT:
                // 100% of normal damage, nothing to modify
                break;
            case UNHAPPY:
                // 75% of normal damage
                mindamage = mindamage * 0.75f;
                maxdamage = maxdamage * 0.75f;
                break;
        }
    }

    // Disarm for creatures
    if (hasWeapon(attType) && !hasWeaponForAttack(attType))
    {
        mindamage *= 0.5f;
        maxdamage *= 0.5f;
    }

    uint16 fieldmin, fieldmax;

    switch (attType)
    {
        case RANGED_ATTACK:
            fieldmin = UNIT_FIELD_MINRANGEDDAMAGE;
            fieldmax = UNIT_FIELD_MAXRANGEDDAMAGE;
            break;
        case BASE_ATTACK:
            fieldmin = UNIT_FIELD_MINDAMAGE;
            fieldmax = UNIT_FIELD_MAXDAMAGE;
            break;
        case OFF_ATTACK:
            fieldmin = UNIT_FIELD_MINOFFHANDDAMAGE;
            fieldmax = UNIT_FIELD_MAXOFFHANDDAMAGE;
            break;
        default:
            return;
    }

    SetStatFloatValue(fieldmin, mindamage);
    SetStatFloatValue(fieldmax, maxdamage);
}
