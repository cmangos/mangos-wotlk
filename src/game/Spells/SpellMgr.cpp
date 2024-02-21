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

#include "Spells/SpellMgr.h"
#include "Globals/ObjectMgr.h"
#include "Spells/SpellAuraDefines.h"
#include "Spells/SpellTargets.h"
#include "Spells/SpellEffectDefines.h"
#include "Util/ProgressBar.h"
#include "Server/DBCStores.h"
#include "Server/SQLStorages.h"
#include "Chat/Chat.h"
#include "Spells/Spell.h"
#include "Entities/Unit.h"
#include "World/World.h"
#include "Maps/Map.h"

bool IsPrimaryProfessionSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    if (!pSkill)
        return false;

    if (pSkill->categoryId != SKILL_CATEGORY_PROFESSION)
        return false;

    return true;
}

SpellMgr::SpellMgr()
{
}

SpellMgr::~SpellMgr()
{
}

SpellMgr& SpellMgr::Instance()
{
    static SpellMgr spellMgr;
    return spellMgr;
}

int32 GetSpellDuration(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const* du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[0] == -1) ? -1 : abs(du->Duration[0]);
}

int32 GetSpellMaxDuration(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const* du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[2] == -1) ? -1 : abs(du->Duration[2]);
}

int32 CalculateSpellDuration(SpellEntry const* spellInfo, Unit const* caster, Unit const* target, AuraScript* auraScript)
{
    int32 duration = GetSpellDuration(spellInfo);

    if (duration != -1 && caster && !spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
    {
        int32 maxduration = GetSpellMaxDuration(spellInfo);

        if (duration != maxduration && caster->IsPlayer())
            duration += int32((maxduration - duration) * caster->GetComboPoints() / 5);

        if (auraScript)
            duration = auraScript->OnDurationCalculate(caster, target, duration);

        if (Player* modOwner = caster->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_DURATION, duration);

            if (spellInfo->HasAttribute(SPELL_ATTR_EX5_SPELL_HASTE_AFFECTS_PERIODIC) || caster->HasAffectedAura(SPELL_AURA_PERIODIC_HASTE, spellInfo))
                duration = int32(duration * caster->GetFloatValue(UNIT_MOD_CAST_SPEED));

            if (duration < 0)
                duration = 0;
        }
    }
    else if (auraScript)
        duration = auraScript->OnDurationCalculate(caster, target, duration);

    return duration;
}

uint32 GetSpellCastTime(SpellEntry const* spellInfo, WorldObject* caster, Spell* spell, bool consume)
{
    if (spell)
    {
        // Workaround for custom cast time
        switch (spellInfo->Id)
        {
            case 3366:  // Opening - seems to have a settable timer per usage
                if (Item* item = spell->GetCastItem())
                {
                    switch (item->GetEntry())
                    {
                        case 31088: // Tainted Core - instant opening
                            return 0;
                        default:
                            break;
                    }
                }
                break;
            case 46546: // Ritual of Summoning
                return 0;
            default:
                break;
        }

        // some triggered spells have data only usable for client
        if (spell->IsTriggeredSpellWithRedundantCastTime())
            return 0;

        // spell targeted to non-trading trade slot item instant at trade success apply
        if (caster->IsPlayer())
            if (TradeData* my_trade = static_cast<Player*>(caster)->GetTradeData())
                if (Item* nonTrade = my_trade->GetTraderData()->GetItem(TRADE_SLOT_NONTRADED))
                    if (nonTrade == spell->m_targets.getItemTarget())
                        return 0;
    }

    SpellCastTimesEntry const* spellCastTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);

    // not all spells have cast time index and this is all is passive abilities
    if (!spellCastTimeEntry)
        return 0;

    int32 spellRank = caster && caster->IsUnit() ? static_cast<Unit*>(caster)->GetSpellRank(spellInfo) : 0;
    int32 castTime = spellCastTimeEntry->CastTime + spellCastTimeEntry->CastTimePerLevel * (spellRank / 5 - spellInfo->baseLevel);
    castTime = std::max(castTime, spellCastTimeEntry->MinCastTime);

    // Hunter Ranged spells need cast time + 0.5s to reflect tooltips, excluding Auto Shot
    if (spellInfo->HasAttribute(SPELL_ATTR_USES_RANGED_SLOT) && (!spell || !spell->IsAutoRepeat()))
        castTime += 500;

    if (spell)
    {
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CASTING_TIME, castTime, consume);

        if (caster->IsUnit())
        {
            Unit* unitCaster = static_cast<Unit*>(caster);
            if (!spellInfo->HasAttribute(SPELL_ATTR_IS_ABILITY) && !spellInfo->HasAttribute(SPELL_ATTR_IS_TRADESKILL) && !spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
                castTime = int32(castTime * unitCaster->GetFloatValue(UNIT_MOD_CAST_SPEED));
            else if (spell->IsRangedSpell() && !spell->IsAutoRepeat())
                castTime = int32(castTime * unitCaster->m_modAttackSpeedPct[RANGED_ATTACK]);
        }
    }

    return (castTime > 0) ? uint32(castTime) : 0;
}

uint32 GetSpellCastTimeForBonus(SpellEntry const* spellProto, DamageEffectType damagetype)
{
    uint32 CastingTime = (!IsChanneledSpell(spellProto)) ? GetSpellCastTime(spellProto, nullptr) : GetSpellDuration(spellProto);

    if (CastingTime > 7000) CastingTime = 7000;
    if (CastingTime < 1500) CastingTime = 1500;

    if (damagetype == DOT && !IsChanneledSpell(spellProto))
        CastingTime = 3500;

    int32 overTime    = 0;
    uint8 effects     = 0;
    bool DirectDamage = false;
    bool AreaEffect   = false;

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (IsAreaEffectTarget(SpellTarget(spellProto->EffectImplicitTargetA[i])) || IsAreaEffectTarget(SpellTarget(spellProto->EffectImplicitTargetB[i])))
            AreaEffect = true;

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        switch (spellProto->Effect[i])
        {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_POWER_DRAIN:
            case SPELL_EFFECT_HEALTH_LEECH:
            case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_HEAL:
                DirectDamage = true;
                break;
            case SPELL_EFFECT_APPLY_AURA:
                switch (spellProto->EffectApplyAuraName[i])
                {
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_LEECH:
                        if (GetSpellDuration(spellProto))
                            overTime = GetSpellDuration(spellProto);
                        break;
                    // Penalty for additional effects
                    case SPELL_AURA_DUMMY:
                        ++effects;
                        break;
                    case SPELL_AURA_MOD_DECREASE_SPEED:
                        ++effects;
                        break;
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_STUN:
                    case SPELL_AURA_MOD_ROOT:
                        // -10% per effect
                        effects += 2;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    // Combined Spells with Both Over Time and Direct Damage
    if (overTime > 0 && CastingTime > 0 && DirectDamage)
    {
        // mainly for DoTs which are 3500 here otherwise
        uint32 OriginalCastTime = GetSpellCastTime(spellProto, nullptr);
        if (OriginalCastTime > 7000) OriginalCastTime = 7000;
        if (OriginalCastTime < 1500) OriginalCastTime = 1500;
        // Portion to Over Time
        float PtOT = (overTime / 15000.0f) / ((overTime / 15000.0f) + (OriginalCastTime / 3500.0f));

        if (damagetype == DOT)
            CastingTime = uint32(CastingTime * PtOT);
        else if (PtOT < 1.0f)
            CastingTime  = uint32(CastingTime * (1 - PtOT));
        else
            CastingTime = 0;
    }

    // Area Effect Spells receive only half of bonus
    if (AreaEffect)
        CastingTime /= 2;

    // 50% for damage and healing spells for leech spells from damage bonus and 0% from healing
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        if (spellProto->Effect[j] == SPELL_EFFECT_HEALTH_LEECH ||
                (spellProto->Effect[j] == SPELL_EFFECT_APPLY_AURA && spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
        {
            CastingTime /= 2;
            break;
        }
    }

    // -5% of total per any additional effect (multiplicative)
    for (int i = 0; i < effects; ++i)
        CastingTime *= 0.95f;

    return CastingTime;
}

uint16 GetSpellAuraMaxTicks(SpellEntry const* spellInfo)
{
    int32 DotDuration = GetSpellDuration(spellInfo);
    if (DotDuration == 0)
        return 1;

    // 200% limit
    if (DotDuration > 30000)
        DotDuration = 30000;

    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        if (spellInfo->Effect[j] == SPELL_EFFECT_APPLY_AURA && (
                    spellInfo->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_DAMAGE ||
                    spellInfo->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_HEAL ||
                    spellInfo->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
        {
            if (spellInfo->EffectAmplitude[j] != 0)
                return DotDuration / spellInfo->EffectAmplitude[j];
            break;
        }
    }

    return 6;
}

uint16 GetSpellAuraMaxTicks(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
    {
        sLog.outError("GetSpellAuraMaxTicks: Spell %u not exist!", spellId);
        return 1;
    }

    return GetSpellAuraMaxTicks(spellInfo);
}

float CalculateDefaultCoefficient(SpellEntry const* spellProto, DamageEffectType const damagetype)
{
    // Damage over Time spells bonus calculation
    float DotFactor = 1.0f;
    if (damagetype == DOT)
    {
        if (!IsChanneledSpell(spellProto))
            DotFactor = GetSpellDuration(spellProto) / 15000.0f;

        if (uint16 DotTicks = GetSpellAuraMaxTicks(spellProto))
            DotFactor /= DotTicks;
    }

    // Distribute Damage over multiple effects, reduce by AoE
    float coeff = GetSpellCastTimeForBonus(spellProto, damagetype) / 3500.0f;

    return coeff * DotFactor;
}

WeaponAttackType GetWeaponAttackType(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return BASE_ATTACK;

    switch (spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            if (spellInfo->HasAttribute(SPELL_ATTR_EX3_REQUIRES_OFFHAND_WEAPON))
                return OFF_ATTACK;
            return BASE_ATTACK;
        case SPELL_DAMAGE_CLASS_RANGED:
            return RANGED_ATTACK;
        default:
        {
            // Wands
            if (spellInfo->HasAttribute(SPELL_ATTR_EX3_NORMAL_RANGED_ATTACK))
                return RANGED_ATTACK;
            if (spellInfo->HasAttribute(SPELL_ATTR_EX3_REQUIRES_OFFHAND_WEAPON))
                return OFF_ATTACK;
            return BASE_ATTACK;
        }
    }
}

bool IsPassiveSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;
    return IsPassiveSpell(spellInfo);
}

bool IsPassiveSpell(SpellEntry const* spellInfo)
{
    return spellInfo->HasAttribute(SPELL_ATTR_PASSIVE);
}

SpellSpecific GetSpellSpecific(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return SPELL_NORMAL;

    if (SpellSpecific food = sSpellMgr.GetSpellFoodSpecific(spellInfo))
        return food;

    switch (spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // family flags 18(Molten), 25(Frost/Ice), 28(Mage)
            if (spellInfo->SpellFamilyFlags & uint64(0x12040000))
                return SPELL_MAGE_ARMOR;

            // Arcane Power (using id instead of mask):
            if (spellInfo->Id == 12042)
                return SPELL_BUFF_CASTER_POWER;

            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            if (spellInfo->SpellFamilyFlags & uint64(0x00008000010000))
                return SPELL_SHOUT_BUFF;

            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // only warlock curses have this
            if (spellInfo->Dispel == DISPEL_CURSE)
                return SPELL_CURSE;

            // Warlock (Demon Armor | Demon Skin | Fel Armor)
            if (spellInfo->IsFitToFamilyMask(uint64(0x2000002000000000), 0x00000010))
                return SPELL_WARLOCK_ARMOR;

            // Corruption and Seed of Corruption
            if (spellInfo->IsFitToFamilyMask(uint64(0x1000000002)))
                return SPELL_CORRUPTION_DEBUFF;

            // Unstable Affliction | Immolate
            if (spellInfo->IsFitToFamilyMask(uint64(0x0000010000000004)))
                return SPELL_UA_IMMOLATE;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Power Infusion:
            if (spellInfo->Id == 10060)
                return SPELL_BUFF_CASTER_POWER;

            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // only hunter stings have this
            if (spellInfo->Dispel == DISPEL_POISON)
                return SPELL_STING;

            // only hunter aspects have this
            if (spellInfo->IsFitToFamilyMask(uint64(0x0044000000380000), 0x00001010))
                return SPELL_ASPECT;

            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (IsSealSpell(spellInfo))
                return SPELL_SEAL;

            if (spellInfo->IsFitToFamilyMask(uint64(0x0000000011010002)))
                return SPELL_BLESSING;

            if (spellInfo->IsFitToFamilyMask(uint64(0x0000000000002190)))
                return SPELL_HAND;

            // skip Heart of the Crusader that have also same spell family mask
            if (spellInfo->IsFitToFamilyMask(uint64(0x00000800180400)) && !spellInfo->IsFitToFamilyMask(uint64(0x20000000), uint64(0x40)))
                return SPELL_JUDGEMENT;

            // only paladin auras have this (for palaldin class family)
            if (spellInfo->IsFitToFamilyMask(uint64(0x0000000000000000), 0x00000020))
                return SPELL_AURA;

            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            if (IsElementalShield(spellInfo))
                return SPELL_ELEMENTAL_SHIELD;

            break;
        }

        case SPELLFAMILY_POTION:
            return sSpellMgr.GetSpellElixirSpecific(spellInfo->Id);

        case SPELLFAMILY_DEATHKNIGHT:
            if (spellInfo->Category == 47)
                return SPELL_PRESENCE;
            break;
    }

    // Tracking spells (exclude Well Fed, some other always allowed cases)
    if (spellInfo->HasAttribute(SPELL_ATTR_EX6_ALLOW_WHILE_RIDING_VEHICLE) && (IsSpellHaveAura(spellInfo, SPELL_AURA_TRACK_CREATURES) ||
        IsSpellHaveAura(spellInfo, SPELL_AURA_TRACK_STEALTHED) ||
        IsSpellHaveAura(spellInfo, SPELL_AURA_TRACK_RESOURCES)))
        return SPELL_TRACKER;

    // elixirs can have different families, but potion most ofc.
    if (SpellSpecific sp = sSpellMgr.GetSpellElixirSpecific(spellInfo->Id))
        return sp;

    return SPELL_NORMAL;
}

bool IsExplicitPositiveTarget(uint32 targetA)
{
    // positive targets that in target selection code expect target in m_targets, so not that auto-select target by spell data by m_caster and etc
    switch (targetA)
    {
        case TARGET_UNIT_FRIEND:
        case TARGET_UNIT_PARTY:
        case TARGET_UNIT_FRIEND_CHAIN_HEAL:
        case TARGET_UNIT_RAID:
        case TARGET_UNIT_RAID_AND_CLASS:
            return true;
        default:
            break;
    }
    return false;
}

bool IsExplicitNegativeTarget(uint32 targetA)
{
    // non-positive targets that in target selection code expect target in m_targets, so not that auto-select target by spell data by m_caster and etc
    switch (targetA)
    {
        case TARGET_UNIT_ENEMY:
        case TARGET_LOCATION_CASTER_TARGET_POSITION:
            return true;
        default:
            break;
    }
    return false;
}

SpellCastResult GetErrorAtShapeshiftedCast(SpellEntry const* spellInfo, uint32 form)
{
    // talents that learn spells can have stance requirements that need ignore
    // (this requirement only for client-side stance show in talent description)
    if (GetTalentSpellCost(spellInfo->Id) > 0 &&
            (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[EFFECT_INDEX_2] == SPELL_EFFECT_LEARN_SPELL))
        return SPELL_CAST_OK;

    uint32 stanceMask = (form ? 1 << (form - 1) : 0);

    if (stanceMask & spellInfo->StancesNot[0])                 // can explicitly not be casted in this stance
        return SPELL_FAILED_NOT_SHAPESHIFT;

    if (stanceMask & spellInfo->Stances[0])                    // can explicitly be casted in this stance
        return SPELL_CAST_OK;

    bool actAsShifted = false;
    if (form > 0)
    {
        SpellShapeshiftFormEntry const* shapeInfo = sSpellShapeshiftFormStore.LookupEntry(form);
        if (!shapeInfo)
        {
            sLog.outError("GetErrorAtShapeshiftedCast: unknown shapeshift %u", form);
            return SPELL_CAST_OK;
        }
        actAsShifted = !(shapeInfo->flags1 & 1);            // shapeshift acts as normal form for spells
    }

    if (actAsShifted)
    {
        if (spellInfo->HasAttribute(SPELL_ATTR_NOT_SHAPESHIFT)) // not while shapeshifted
            return SPELL_FAILED_NOT_SHAPESHIFT;
        if (spellInfo->Stances[0] != 0)                   // needs other shapeshift
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }
    else
    {
        // needs shapeshift
        if (!spellInfo->HasAttribute(SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED) && spellInfo->Stances[0] != 0)
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }

    return SPELL_CAST_OK;
}

void SpellMgr::LoadSpellTargetPositions()
{
    mSpellTargetPositions.clear();                          // need for reload case

    uint32 count = 0;

    //                                                0   1           2                  3                  4                  5
    auto queryResult = WorldDatabase.Query("SELECT id, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM spell_target_position");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u spell target destination coordinates", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 Spell_ID = fields[0].GetUInt32();

        SpellTargetPosition st;

        st.target_mapId       = fields[1].GetUInt32();
        st.target_X           = fields[2].GetFloat();
        st.target_Y           = fields[3].GetFloat();
        st.target_Z           = fields[4].GetFloat();
        st.target_Orientation = fields[5].GetFloat();

        MapEntry const* mapEntry = sMapStore.LookupEntry(st.target_mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Spell (ID:%u) target map (ID: %u) does not exist in `Map.dbc`.", Spell_ID, st.target_mapId);
            continue;
        }

        if (st.target_X == 0 && st.target_Y == 0 && st.target_Z == 0)
        {
            sLog.outErrorDb("Spell (ID:%u) target coordinates not provided.", Spell_ID);
            continue;
        }

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(Spell_ID);
        if (!spellInfo)
        {
            sLog.outErrorDb("Spell (ID:%u) listed in `spell_target_position` does not exist.", Spell_ID);
            continue;
        }

        bool found = false;
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (spellInfo->EffectImplicitTargetA[i] == TARGET_LOCATION_DATABASE || spellInfo->EffectImplicitTargetB[i] == TARGET_LOCATION_DATABASE)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            sLog.outErrorDb("Spell (Id: %u) listed in `spell_target_position` does not have target TARGET_LOCATION_DATABASE (17).", Spell_ID);
            continue;
        }

        mSpellTargetPositions[Spell_ID] = st;
        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u spell target destination coordinates", count);
    sLog.outString();
}

template <typename EntryType, typename WorkerType, typename StorageType>
struct SpellRankHelper
{
    SpellRankHelper(SpellMgr& _mgr, StorageType& _storage): mgr(_mgr), worker(_storage), customRank(0) {}
    void RecordRank(EntryType& entry, uint32 spell_id)
    {
        const SpellEntry* spell = sSpellTemplate.LookupEntry<SpellEntry>(spell_id);
        if (!spell)
        {
            sLog.outErrorDb("Spell %u listed in `%s` does not exist", spell_id, worker.TableName());
            return;
        }

        uint32 first_id = mgr.GetFirstSpellInChain(spell_id);

        // most spell ranks expected same data
        if (first_id)
        {
            firstRankSpells.insert(first_id);

            if (first_id != spell_id)
            {
                if (!worker.IsValidCustomRank(entry, spell_id, first_id))
                    return;
                // for later check that first rank also added
                firstRankSpellsWithCustomRanks.insert(first_id);
                ++customRank;
            }
        }

        worker.AddEntry(entry, spell);
    }
    void FillHigherRanks()
    {
        // check that first rank added for custom ranks
        for (std::set<uint32>::const_iterator itr = firstRankSpellsWithCustomRanks.begin(); itr != firstRankSpellsWithCustomRanks.end(); ++itr)
            if (!worker.HasEntry(*itr))
                sLog.outErrorDb("Spell %u must be listed in `%s` as first rank for listed custom ranks of spell but not found!", *itr, worker.TableName());

        // fill absent non first ranks data base at first rank data
        for (std::set<uint32>::const_iterator itr = firstRankSpells.begin(); itr != firstRankSpells.end(); ++itr)
        {
            if (worker.SetStateToEntry(*itr))
                mgr.doForHighRanks(*itr, worker);
        }
    }
    std::set<uint32> firstRankSpells;
    std::set<uint32> firstRankSpellsWithCustomRanks;

    SpellMgr& mgr;
    WorkerType worker;
    uint32 customRank;
};

struct DoSpellProcEvent
{
    DoSpellProcEvent(SpellProcEventMap& _spe_map) : spe_map(_spe_map), customProc(0), count(0) {}
    void operator()(uint32 spell_id)
    {
        SpellProcEventEntry const& spe = state->second;
        // add ranks only for not filled data (some ranks have ppm data different for ranks for example)
        SpellProcEventMap::const_iterator spellItr = spe_map.find(spell_id);
        if (spellItr == spe_map.end())
            spe_map[spell_id] = spe;
        // if custom rank data added then it must be same except ppm
        else
        {
            SpellProcEventEntry const& r_spe = spellItr->second;
            if (spe.schoolMask != r_spe.schoolMask)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different schoolMask from first rank in chain", spell_id);

            if (spe.spellFamilyName != r_spe.spellFamilyName)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different spellFamilyName from first rank in chain", spell_id);

            for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                if (spe.spellFamilyMask[i] != r_spe.spellFamilyMask[i])
                {
                    sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different spellFamilyMask/spellFamilyMask2 from first rank in chain", spell_id);
                    break;
                }
            }

            if (spe.procFlags != r_spe.procFlags)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different procFlags from first rank in chain", spell_id);

            if (spe.procEx != r_spe.procEx)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different procEx from first rank in chain", spell_id);

            // only ppm allowed has been different from first rank

            if (spe.customChance != r_spe.customChance)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different customChance from first rank in chain", spell_id);

            if (spe.cooldown != r_spe.cooldown)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` as custom rank have different cooldown from first rank in chain", spell_id);
        }
    }

    static const char* TableName() { return "spell_proc_event"; }
    bool IsValidCustomRank(SpellProcEventEntry const& spe, uint32 entry, uint32 first_id) const
    {
        // let have independent data in table for spells with ppm rates (exist rank dependent ppm rate spells)
        if (!spe.ppmRate)
        {
            sLog.outErrorDb("Spell %u listed in `spell_proc_event` is not first rank (%u) in chain", entry, first_id);
            // prevent loading since it won't have an effect anyway
            return false;
        }
        return true;
    }
    void AddEntry(SpellProcEventEntry const& spe, SpellEntry const* spell)
    {
        spe_map[spell->Id] = spe;

        bool isCustom = false;

        if (spe.procFlags == 0)
        {
            if (spell->procFlags == 0)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` probably not triggered spell (no proc flags)", spell->Id);
        }
        else
        {
            if (spell->procFlags == spe.procFlags)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` has exactly same proc flags as in spell.dbc, field value redundant", spell->Id);
            else
                isCustom = true;
        }

        if (spe.customChance == 0)
        {
            /* enable for re-check cases, 0 chance ok for some cases because in some cases it set by another spell/talent spellmod)
            if (spell->procChance==0 && !spe.ppmRate)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` probably not triggered spell (no chance or ppm)", spell->Id);
            */
        }
        else
        {
            if (spell->procChance == spe.customChance)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` has exactly same custom chance as in spell.dbc, field value redundant", spell->Id);
            else
                isCustom = true;
        }

        // totally redundant record
        if (!spe.schoolMask && !spe.procFlags &&
                !spe.procEx && !spe.ppmRate && !spe.customChance && !spe.cooldown)
        {
            bool empty = spe.spellFamilyName == 0;
            for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                if (spe.spellFamilyMask[i])
                {
                    empty = false;
                    ClassFamilyMask const& mask = spell->GetEffectSpellClassMask(SpellEffectIndex(i));
                    if (mask == spe.spellFamilyMask[i])
                        sLog.outErrorDb("Spell %u listed in `spell_proc_event` has same class mask as in Spell.dbc (EffectIndex %u) and doesn't have any other data", spell->Id, i);
                }
            }
            if (empty)
                sLog.outErrorDb("Spell %u listed in `spell_proc_event` doesn't have any useful data", spell->Id);
        }

        if (isCustom)
            ++customProc;
        else
            ++count;
    }

    bool HasEntry(uint32 spellId) const { return spe_map.find(spellId) != spe_map.end(); }
    bool SetStateToEntry(uint32 spellId) { return (state = spe_map.find(spellId)) != spe_map.end(); }
    SpellProcEventMap& spe_map;
    SpellProcEventMap::const_iterator state;

    uint32 customProc;
    uint32 count;
};

void SpellMgr::LoadSpellProcEvents()
{
    mSpellProcEventMap.clear();                             // need for reload case

    //                                             0      1           2                3                  4                  5                  6                  7                  8                  9                  10                 11                 12         13      14       15            16
    auto queryResult = WorldDatabase.Query("SELECT entry, SchoolMask, SpellFamilyName, SpellFamilyMaskA0, SpellFamilyMaskA1, SpellFamilyMaskA2, SpellFamilyMaskB0, SpellFamilyMaskB1, SpellFamilyMaskB2, SpellFamilyMaskC0, SpellFamilyMaskC1, SpellFamilyMaskC2, procFlags, procEx, ppmRate, CustomChance, Cooldown FROM spell_proc_event");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString();
        sLog.outString(">> No spell proc event conditions loaded");
        return;
    }

    SpellRankHelper<SpellProcEventEntry, DoSpellProcEvent, SpellProcEventMap> rankHelper(*this, mSpellProcEventMap);

    BarGoLink bar(queryResult->GetRowCount());
    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 entry = fields[0].GetUInt32();

        SpellProcEventEntry spe;

        spe.schoolMask      = fields[1].GetUInt32();
        spe.spellFamilyName = fields[2].GetUInt32();

        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            spe.spellFamilyMask[i] = ClassFamilyMask(
                                         (uint64)fields[i + 3].GetUInt32() | ((uint64)fields[i + 6].GetUInt32() << 32),
                                         fields[i + 9].GetUInt32());
        }
        spe.procFlags       = fields[12].GetUInt32();
        spe.procEx          = fields[13].GetUInt32();
        spe.ppmRate         = fields[14].GetFloat();
        spe.customChance    = fields[15].GetFloat();
        spe.cooldown        = fields[16].GetUInt32();

        rankHelper.RecordRank(spe, entry);
    }
    while (queryResult->NextRow());

    rankHelper.FillHigherRanks();

    sLog.outString(">> Loaded %u extra spell proc event conditions +%u custom proc (inc. +%u custom ranks)",  rankHelper.worker.count, rankHelper.worker.customProc, rankHelper.customRank);
    sLog.outString();
}

struct DoSpellProcItemEnchant
{
    DoSpellProcItemEnchant(SpellProcItemEnchantMap& _procMap, float _ppm) : procMap(_procMap), ppm(_ppm) {}
    void operator()(uint32 spell_id) { procMap[spell_id] = ppm; }

    SpellProcItemEnchantMap& procMap;
    float ppm;
};

void SpellMgr::LoadSpellProcItemEnchant()
{
    mSpellProcItemEnchantMap.clear();                       // need for reload case

    uint32 count = 0;

    //                                             0      1
    auto queryResult = WorldDatabase.Query("SELECT entry, ppmRate FROM spell_proc_item_enchant");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u proc item enchant definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 entry = fields[0].GetUInt32();
        float ppmRate = fields[1].GetFloat();

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(entry);

        if (!spellInfo)
        {
            sLog.outErrorDb("Spell %u listed in `spell_proc_item_enchant` does not exist", entry);
            continue;
        }

        uint32 first_id = GetFirstSpellInChain(entry);

        if (first_id != entry)
        {
            sLog.outErrorDb("Spell %u listed in `spell_proc_item_enchant` is not first rank (%u) in chain", entry, first_id);
            // prevent loading since it won't have an effect anyway
            continue;
        }

        mSpellProcItemEnchantMap[entry] = ppmRate;

        // also add to high ranks
        DoSpellProcItemEnchant worker(mSpellProcItemEnchantMap, ppmRate);
        doForHighRanks(entry, worker);

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u proc item enchant definitions", count);
    sLog.outString();
}

bool IsCastEndProcModifierAura(SpellEntry const* spellInfo, SpellEffectIndex effecIdx, SpellEntry const* procSpell)
{
    // modifier auras that can proc on cast end
    switch (AuraType(spellInfo->EffectApplyAuraName[effecIdx]))
    {
        case SPELL_AURA_ADD_FLAT_MODIFIER:
        case SPELL_AURA_ADD_PCT_MODIFIER:
        {
            switch (spellInfo->EffectMiscValue[effecIdx])
            {
                case SPELLMOD_RANGE:
                case SPELLMOD_RADIUS:
                case SPELLMOD_NOT_LOSE_CASTING_TIME:
                case SPELLMOD_CASTING_TIME:
                case SPELLMOD_COOLDOWN:
                case SPELLMOD_COST:
                case SPELLMOD_GLOBAL_COOLDOWN:
                    return true;
                default:
                    break;
            }
        }
        case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
        {
            for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                if (IsEffectHandledImmediatelySpellLaunch(procSpell, SpellEffectIndex(i)))
                    return true;

            return false;
        }
        default:
            return false;
    }
}

bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellEntry const* spellInfo, uint32 procFlags, uint32 procExtra)
{
    // No extra req need
    uint32 procEvent_procEx = PROC_EX_NONE;

    // check prockFlags for condition
    if ((procFlags & EventProcFlag) == 0)
        return false;

    // Always trigger for this
    if (EventProcFlag & (PROC_FLAG_HEARTBEAT | PROC_FLAG_KILL | PROC_FLAG_ON_TRAP_ACTIVATION | PROC_FLAG_DEATH))
        return true;

    if (procFlags & PROC_FLAG_DEAL_HARMFUL_PERIODIC && EventProcFlag & PROC_FLAG_DEAL_HARMFUL_PERIODIC)
    {
        if (procExtra & PROC_EX_INTERNAL_HOT)
        {
            if (EventProcFlag == PROC_FLAG_DEAL_HARMFUL_PERIODIC)
            {
                // no aura with only PROC_FLAG_DONE_PERIODIC and spellFamilyName == 0 can proc from a HOT.
                if (!spellInfo->SpellFamilyName)
                    return false;
            }
            // Aura must have positive procflags for a HOT to proc
            else if (!(EventProcFlag & (PROC_FLAG_DEAL_HELPFUL_SPELL | PROC_FLAG_DEAL_HELPFUL_ABILITY)))
                return false;
        }
        // Aura must have negative or neutral(PROC_FLAG_DONE_PERIODIC only) procflags for a DOT to proc
        else if (EventProcFlag != PROC_FLAG_DEAL_HARMFUL_PERIODIC)
            if (!(EventProcFlag & (PROC_FLAG_DEAL_HARMFUL_SPELL | PROC_FLAG_DEAL_HARMFUL_ABILITY)))
                return false;
    }

    if (procFlags & PROC_FLAG_TAKE_HARMFUL_PERIODIC && EventProcFlag & PROC_FLAG_TAKE_HARMFUL_PERIODIC)
    {
        if (procExtra & PROC_EX_INTERNAL_HOT)
        {
            // No aura that only has PROC_FLAG_TAKEN_PERIODIC can proc from a HOT.
            if (EventProcFlag == PROC_FLAG_TAKE_HARMFUL_PERIODIC)
                return false;
            // Aura must have positive procflags for a HOT to proc
            if (!(EventProcFlag & (PROC_FLAG_TAKE_HELPFUL_SPELL | PROC_FLAG_TAKE_HELPFUL_ABILITY)))
                return false;
        }
        // Aura must have negative or neutral(PROC_FLAG_TAKEN_PERIODIC only) procflags for a DOT to proc
        else if (EventProcFlag != PROC_FLAG_TAKE_HARMFUL_PERIODIC)
            if (!(EventProcFlag & (PROC_FLAG_TAKE_HARMFUL_SPELL | PROC_FLAG_TAKE_HARMFUL_ABILITY)))
                return false;
    }

    if (spellProcEvent)     // Exist event data
    {
        // Store extra req
        procEvent_procEx = spellProcEvent->procEx;

        // For melee triggers
        if (spellInfo == nullptr)
        {
            // Check (if set) for school (melee attack have Normal school)
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
                return false;
        }
        else // For spells need check school/spell family/family mask
        {
            // Check (if set) for school
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & spellInfo->SchoolMask) == 0)
                return false;

            // Check (if set) for spellFamilyName
            if (spellProcEvent->spellFamilyName && (spellProcEvent->spellFamilyName != spellInfo->SpellFamilyName))
                return false;
        }
    }

    // Check for extra req (if none) and hit/crit
    if (procEvent_procEx == PROC_EX_NONE)
    {
        // Don't allow proc from periodic heal if no extra requirement is defined
        if (EventProcFlag & (PROC_FLAG_DEAL_HARMFUL_PERIODIC | PROC_FLAG_TAKE_HARMFUL_PERIODIC) && (procExtra & PROC_EX_PERIODIC_POSITIVE))
            return false;

        // No extra req, so can trigger for (damage/healing present) and cast end/hit/crit
        if (procExtra & (PROC_EX_CAST_END | PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT))
            return true;
    }
    else // all spells hits here only if resist/reflect/immune/evade
    {
        // Exist req for PROC_EX_EX_TRIGGER_ON_NO_DAMAGE
        if (procEvent_procEx & PROC_EX_EX_TRIGGER_ON_NO_DAMAGE)
            return true;
        // Check Extra Requirement like (hit/crit/miss/resist/parry/dodge/block/immune/reflect/absorb and other)
        if (procEvent_procEx & procExtra)
            return true;
    }
    return false;
}

void SpellMgr::LoadSpellElixirs()
{
    mSpellElixirs.clear();                                  // need for reload case

    uint32 count = 0;

    //                                             0      1
    auto queryResult = WorldDatabase.Query("SELECT entry, mask FROM spell_elixir");
    if (!queryResult)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString(">> Loaded %u spell elixir definitions", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 entry = fields[0].GetUInt32();
        uint8 mask = fields[1].GetUInt8();

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(entry);

        if (!spellInfo)
        {
            sLog.outErrorDb("Spell %u listed in `spell_elixir` does not exist", entry);
            continue;
        }

        mSpellElixirs[entry] = mask;

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u spell elixir definitions", count);
    sLog.outString();
}

struct DoSpellThreat
{
    DoSpellThreat(SpellThreatMap& _threatMap) : threatMap(_threatMap), count(0) {}
    void operator()(uint32 spell_id)
    {
        SpellThreatEntry const& ste = state->second;
        // add ranks only for not filled data (spells adding flat threat are usually different for ranks)
        SpellThreatMap::const_iterator spellItr = threatMap.find(spell_id);
        if (spellItr == threatMap.end())
            threatMap[spell_id] = ste;

        // just assert that entry is not redundant
        else
        {
            SpellThreatEntry const& r_ste = spellItr->second;
            if (ste.threat == r_ste.threat && ste.multiplier == r_ste.multiplier && ste.ap_bonus == r_ste.ap_bonus)
                sLog.outErrorDb("Spell %u listed in `spell_threat` as custom rank has same data as Rank 1, so redundant", spell_id);
        }
    }
    const char* TableName() const { return "spell_threat"; }
    bool IsValidCustomRank(SpellThreatEntry const& ste, uint32 entry, uint32 first_id) const
    {
        if (!ste.threat)
        {
            sLog.outErrorDb("Spell %u listed in `spell_threat` is not first rank (%u) in chain and has no threat", entry, first_id);
            // prevent loading unexpected data
            return false;
        }
        return true;
    }
    void AddEntry(SpellThreatEntry const& ste, SpellEntry const* spell)
    {
        threatMap[spell->Id] = ste;

        // flat threat bonus and attack power bonus currently only work properly when all
        // effects have same targets, otherwise, we'd need to separate it by effect index
        if (ste.threat || ste.ap_bonus != 0.f)
        {
            const uint32* targetA = spell->EffectImplicitTargetA;

            uint32 target = 0;
            bool failed = false;
            for (int i = 0; i < MAX_EFFECT_INDEX; i++)
            {
                if (targetA[i] != 0)
                {
                    if (target == 0)
                        target = targetA[i];
                    else if (targetA[i] != target)
                        failed = true;
                }
            }
            if (failed)
                sLog.outErrorDb("Spell %u listed in `spell_threat` has effects with different targets, threat may be assigned incorrectly", spell->Id);
        }
        ++count;
    }
    bool HasEntry(uint32 spellId) const { return threatMap.count(spellId) > 0; }
    bool SetStateToEntry(uint32 spellId) { return (state = threatMap.find(spellId)) != threatMap.end(); }

    SpellThreatMap& threatMap;
    SpellThreatMap::const_iterator state;
    uint32 count;
};

void SpellMgr::LoadSpellThreats()
{
    mSpellThreatMap.clear();                                // need for reload case

    //                                             0      1       2           3
    auto queryResult = WorldDatabase.Query("SELECT entry, Threat, multiplier, ap_bonus FROM spell_threat");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> No spell threat entries loaded.");
        sLog.outString();
        return;
    }

    SpellRankHelper<SpellThreatEntry, DoSpellThreat, SpellThreatMap> rankHelper(*this, mSpellThreatMap);

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 entry = fields[0].GetUInt32();

        SpellThreatEntry ste;
        ste.threat = fields[1].GetUInt16();
        ste.multiplier = fields[2].GetFloat();
        ste.ap_bonus = fields[3].GetFloat();

        rankHelper.RecordRank(ste, entry);
    }
    while (queryResult->NextRow());

    rankHelper.FillHigherRanks();

    sLog.outString(">> Loaded %u spell threat entries", rankHelper.worker.count);
    sLog.outString();
}

bool SpellMgr::IsSpellCanAffectSpell(SpellEntry const* spellInfo_1, SpellEntry const* spellInfo_2) const
{
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        ClassFamilyMask mask = spellInfo_1->GetEffectSpellClassMask(SpellEffectIndex(i));
        if (spellInfo_2->IsFitToFamilyMask(mask))
            return true;
    }
    return false;
}

bool SpellMgr::IsProfessionOrRidingSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[EFFECT_INDEX_1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[EFFECT_INDEX_1];

    return IsProfessionOrRidingSkill(skill);
}

bool SpellMgr::IsProfessionSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[EFFECT_INDEX_1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[EFFECT_INDEX_1];

    return IsProfessionSkill(skill);
}

bool SpellMgr::IsPrimaryProfessionSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[EFFECT_INDEX_1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[EFFECT_INDEX_1];

    return IsPrimaryProfessionSkill(skill);
}

uint32 SpellMgr::GetProfessionSpellMinLevel(uint32 spellId) const
{
    const uint32 s2l[8][3] =
    {
        // 0 - gather 1 - non-gather 2 - fish
        /*0*/ { 0,   5,  5 },
        /*1*/ { 0,   5,  5 },
        /*2*/ { 0,  10, 10 },
        /*3*/ { 10, 20, 10 },
        /*4*/ { 25, 35, 10 },
        /*5*/ { 40, 50, 10 },
        /*6*/ { 55, 65, 10 },
        /*7*/ { 75, 75, 10 },
    };

    uint32 rank = GetSpellRank(spellId);
    if (rank >= 8)
        return 0;

    SkillLineAbilityMapBounds bounds = GetSkillLineAbilityMapBoundsBySpellId(spellId);
    if (bounds.first == bounds.second)
        return 0;

    switch (bounds.first->second->skillId)
    {
        case SKILL_FISHING:
            return s2l[rank][2];
        case SKILL_HERBALISM:
        case SKILL_MINING:
        case SKILL_SKINNING:
            return s2l[rank][0];
        default:
            return s2l[rank][1];
    }
}

bool SpellMgr::IsPrimaryProfessionFirstRankSpell(uint32 spellId) const
{
    return IsPrimaryProfessionSpell(spellId) && GetSpellRank(spellId) == 1;
}

bool SpellMgr::IsSkillBonusSpell(uint32 spellId) const
{
    SkillLineAbilityMapBounds bounds = GetSkillLineAbilityMapBoundsBySpellId(spellId);

    for (SkillLineAbilityMap::const_iterator _spell_idx = bounds.first; _spell_idx != bounds.second; ++_spell_idx)
    {
        SkillLineAbilityEntry const* pAbility = _spell_idx->second;
        if (!pAbility || pAbility->learnOnGetSkill != ABILITY_LEARNED_ON_GET_PROFESSION_SKILL)
            continue;

        if (pAbility->req_skill_value > 0)
            return true;
    }

    return false;
}

SpellEntry const* SpellMgr::SelectAuraRankForLevel(SpellEntry const* spellInfo, uint32 level) const
{
    // fast case
    if (level + 10 >= spellInfo->spellLevel)
        return spellInfo;

    // ignore selection for passive spells
    if (IsPassiveSpell(spellInfo))
        return spellInfo;

    bool needRankSelection = false;
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        // for simple aura in check apply to any non caster based targets, in rank search mode to any explicit targets
        if (((spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA &&
                (IsExplicitPositiveTarget(spellInfo->EffectImplicitTargetA[i]) ||
                 IsAreaEffectPositiveTarget(SpellTarget(spellInfo->EffectImplicitTargetA[i])))) ||
                spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY ||
                spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_RAID) &&
                IsPositiveEffect(spellInfo, SpellEffectIndex(i)))
        {
            needRankSelection = true;
            break;
        }
    }

    // not required (rank check more slow so check it here)
    if (!needRankSelection || GetSpellRank(spellInfo->Id) == 0)
        return spellInfo;

    // Check for lower rank of same spell if config is set to auto downrank
    if (sWorld.getConfig(CONFIG_BOOL_AUTO_DOWNRANK))
    {
        for (uint32 nextSpellId = spellInfo->Id; nextSpellId != 0; nextSpellId = GetPrevSpellInChain(nextSpellId))
        {
            SpellEntry const* nextSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(nextSpellId);
            if (!nextSpellInfo)
                break;

            // if found appropriate level
            if (level + 10 >= nextSpellInfo->spellLevel)
                return nextSpellInfo;

            // one rank less then
        }
    }

    // Recipient is too low level
    return nullptr;
}

typedef std::unordered_map<uint32, uint32> AbilitySpellPrevMap;

static void LoadSpellChains_AbilityHelper(SpellChainMap& chainMap, AbilitySpellPrevMap const& prevRanks, uint32 spell_id, uint32 prev_id, uint32 deep = 30)
{
    // spell already listed in chains store
    SpellChainMap::const_iterator chain_itr = chainMap.find(spell_id);
    if (chain_itr != chainMap.end())
    {
        MANGOS_ASSERT(chain_itr->second.prev == prev_id && "LoadSpellChains_AbilityHelper: Conflicting data in talents or spell abilities dbc");
        return;
    }

    // prev rank listed in main chain table (can fill correct data directly)
    SpellChainMap::const_iterator prev_chain_itr = chainMap.find(prev_id);
    if (prev_chain_itr != chainMap.end())
    {
        SpellChainNode node;
        node.prev  = prev_id;
        node.first = prev_chain_itr->second.first;
        node.rank  = prev_chain_itr->second.rank + 1;
        node.req   = 0;
        chainMap[spell_id] = node;
        return;
    }

    // prev spell not listed in prev ranks store, so it first rank
    AbilitySpellPrevMap::const_iterator prev_itr = prevRanks.find(prev_id);
    if (prev_itr == prevRanks.end())
    {
        SpellChainNode prev_node;
        prev_node.prev  = 0;
        prev_node.first = prev_id;
        prev_node.rank  = 1;
        prev_node.req   = 0;
        chainMap[prev_id] = prev_node;

        SpellChainNode node;
        node.prev  = prev_id;
        node.first = prev_id;
        node.rank  = 2;
        node.req   = 0;
        chainMap[spell_id] = node;
        return;
    }

    if (deep == 0)
    {
        MANGOS_ASSERT(false && "LoadSpellChains_AbilityHelper: Infinity cycle in spell ability data");
        return;
    }

    // prev rank listed, so process it first
    LoadSpellChains_AbilityHelper(chainMap, prevRanks, prev_id, prev_itr->second, deep - 1);

    // prev rank must be listed now
    prev_chain_itr = chainMap.find(prev_id);
    if (prev_chain_itr == chainMap.end())
        return;

    SpellChainNode node;
    node.prev  = prev_id;
    node.first = prev_chain_itr->second.first;
    node.rank  = prev_chain_itr->second.rank + 1;
    node.req   = 0;
    chainMap[spell_id] = node;
}

void SpellMgr::LoadSpellChains()
{
    mSpellChains.clear();                                   // need for reload case
    mSpellChainsNext.clear();                               // need for reload case

    // load known data for talents
    for (unsigned int i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        // not add ranks for 1 ranks talents (if exist non ranks spells then it will included in table data)
        if (!talentInfo->RankID[1])
            continue;

        for (int j = 0; j < MAX_TALENT_RANK; ++j)
        {
            uint32 spell_id = talentInfo->RankID[j];
            if (!spell_id)
                continue;

            if (!sSpellTemplate.LookupEntry<SpellEntry>(spell_id))
            {
                // sLog.outErrorDb("Talent %u not exist as spell",spell_id);
                continue;
            }

            SpellChainNode node;
            node.prev  = (j > 0) ? talentInfo->RankID[j - 1] : 0;
            node.first = talentInfo->RankID[0];
            node.rank  = j + 1;
            node.req   = 0;

            mSpellChains[spell_id] = node;
        }
    }

    // load known data from spell abilities
    {
        // we can calculate ranks only after full data generation
        AbilitySpellPrevMap prevRanks;
        for (SkillLineAbilityMap::const_iterator ab_itr = mSkillLineAbilityMapBySpellId.begin(); ab_itr != mSkillLineAbilityMapBySpellId.end(); ++ab_itr)
        {
            uint32 spell_id = ab_itr->first;

            // skip GM/test/internal spells.begin Its not have ranks anyway
            if (ab_itr->second->skillId == SKILL_INTERNAL)
                continue;

            // some forward spells not exist and can be ignored (some outdated data)
            SpellEntry const* spell_entry = sSpellTemplate.LookupEntry<SpellEntry>(spell_id);
            if (!spell_entry)                               // no cases
                continue;

            // ignore spell without forwards (non ranked or missing info in skill abilities)
            uint32 forward_id = ab_itr->second->forward_spellid;
            if (!forward_id)
                continue;

            // some forward spells not exist and can be ignored (some outdated data)
            SpellEntry const* forward_entry = sSpellTemplate.LookupEntry<SpellEntry>(forward_id);
            if (!forward_entry)
                continue;

            // some forward spells still exist but excluded from real use as ranks and not listed in skill abilities now
            SkillLineAbilityMapBounds bounds = mSkillLineAbilityMapBySpellId.equal_range(forward_id);
            if (bounds.first == bounds.second)
                continue;

            // spell already listed in chains store
            SpellChainMap::const_iterator chain_itr = mSpellChains.find(forward_id);
            if (chain_itr != mSpellChains.end())
            {
                MANGOS_ASSERT(chain_itr->second.prev == spell_id && "Conflicting data in talents or spell abilities dbc");
                continue;
            }

            // spell already listed in prev ranks store
            AbilitySpellPrevMap::const_iterator prev_itr = prevRanks.find(forward_id);
            if (prev_itr != prevRanks.end())
            {
                MANGOS_ASSERT(prev_itr->second == spell_id && "Conflicting data in talents or spell abilities dbc");
                continue;
            }

            // prev rank listed in main chain table (can fill correct data directly)
            SpellChainMap::const_iterator prev_chain_itr = mSpellChains.find(spell_id);
            if (prev_chain_itr != mSpellChains.end())
            {
                SpellChainNode node;
                node.prev  = spell_id;
                node.first = prev_chain_itr->second.first;
                node.rank  = prev_chain_itr->second.rank + 1;
                node.req   = 0;

                mSpellChains[forward_id] = node;
                continue;
            }

            // need temporary store for later rank calculation
            prevRanks[forward_id] = spell_id;
        }

        while (!prevRanks.empty())
        {
            uint32 spell_id = prevRanks.begin()->first;
            uint32 prev_id  = prevRanks.begin()->second;
            prevRanks.erase(prevRanks.begin());

            LoadSpellChains_AbilityHelper(mSpellChains, prevRanks, spell_id, prev_id);
        }
    }

    // load custom case
    auto queryResult = WorldDatabase.Query("SELECT spell_id, prev_spell, first_spell, `rank`, req_spell FROM spell_chain");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Loaded 0 spell chain records");
        sLog.outErrorDb("`spell_chains` table is empty!");
        sLog.outString();
        return;
    }

    uint32 dbc_count = mSpellChains.size();
    uint32 new_count = 0;
    uint32 req_count = 0;

    BarGoLink bar(queryResult->GetRowCount());
    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();

        uint32 spell_id = fields[0].GetUInt32();

        SpellChainNode node;
        node.prev  = fields[1].GetUInt32();
        node.first = fields[2].GetUInt32();
        node.rank  = fields[3].GetUInt8();
        node.req   = fields[4].GetUInt32();

        if (!sSpellTemplate.LookupEntry<SpellEntry>(spell_id))
        {
            sLog.outErrorDb("Spell %u listed in `spell_chain` does not exist", spell_id);
            continue;
        }

        SpellChainMap::iterator chain_itr = mSpellChains.find(spell_id);
        if (chain_itr != mSpellChains.end())
        {
            if (chain_itr->second.rank != node.rank)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` expected rank %u by DBC data.",
                                spell_id, node.prev, node.first, node.rank, node.req, chain_itr->second.rank);
                continue;
            }

            if (chain_itr->second.prev != node.prev)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` expected prev %u by DBC data.",
                                spell_id, node.prev, node.first, node.rank, node.req, chain_itr->second.prev);
                continue;
            }

            if (chain_itr->second.first != node.first)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` expected first %u by DBC data.",
                                spell_id, node.prev, node.first, node.rank, node.req, chain_itr->second.first);
                continue;
            }

            // update req field by table data
            if (node.req)
            {
                chain_itr->second.req = node.req;
                ++req_count;
                continue;
            }

            // in other case redundant
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) already added (talent or spell ability with forward) and non need in `spell_chain`",
                            spell_id, node.prev, node.first, node.rank, node.req);
            continue;
        }

        if (node.prev != 0 && !sSpellTemplate.LookupEntry<SpellEntry>(node.prev))
        {
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has nonexistent previous rank spell.",
                            spell_id, node.prev, node.first, node.rank, node.req);
            continue;
        }

        if (!sSpellTemplate.LookupEntry<SpellEntry>(node.first))
        {
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has not existing first rank spell.",
                            spell_id, node.prev, node.first, node.rank, node.req);
            continue;
        }

        // check basic spell chain data integrity (note: rank can be equal 0 or 1 for first/single spell)
        if ((spell_id == node.first) != (node.rank <= 1) ||
                (spell_id == node.first) != (node.prev == 0) ||
                (node.rank <= 1) != (node.prev == 0))
        {
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has not compatible chain data.",
                            spell_id, node.prev, node.first, node.rank, node.req);
            continue;
        }

        if (node.req != 0 && !sSpellTemplate.LookupEntry<SpellEntry>(node.req))
        {
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has not existing required spell.",
                            spell_id, node.prev, node.first, node.rank, node.req);
            continue;
        }

        // talents not required data in spell chain for work, but must be checked if present for integrity
        if (TalentSpellPos const* pos = GetTalentSpellPos(spell_id))
        {
            if (node.rank != pos->rank + 1)
            {
                sLog.outErrorDb("Talent %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has wrong rank.",
                                spell_id, node.prev, node.first, node.rank, node.req);
                continue;
            }

            if (TalentEntry const* talentEntry = sTalentStore.LookupEntry(pos->talent_id))
            {
                if (node.first != talentEntry->RankID[0])
                {
                    sLog.outErrorDb("Talent %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has wrong first rank spell.",
                                    spell_id, node.prev, node.first, node.rank, node.req);
                    continue;
                }

                if (node.rank > 1 && node.prev != talentEntry->RankID[node.rank - 1 - 1])
                {
                    sLog.outErrorDb("Talent %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has wrong prev rank spell.",
                                    spell_id, node.prev, node.first, node.rank, node.req);
                    continue;
                }

                /*if(node.req!=talentEntry->DependsOnSpell)
                {
                    sLog.outErrorDb("Talent %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has wrong required spell.",
                        spell_id,node.prev,node.first,node.rank,node.req);
                    continue;
                }*/
            }
        }

        // removed ranks often still listed as forward in skill abilities but not listed as spell in it
        if (node.prev)
        {
            bool skip = false;
            // some forward spells still exist but excluded from real use as ranks and not listed in skill abilities now
            SkillLineAbilityMapBounds bounds = mSkillLineAbilityMapBySpellId.equal_range(spell_id);
            if (bounds.first == bounds.second)
            {
                SkillLineAbilityMapBounds prev_bounds = mSkillLineAbilityMapBySpellId.equal_range(node.prev);
                for (SkillLineAbilityMap::const_iterator ab_itr = prev_bounds.first; ab_itr != prev_bounds.second; ++ab_itr)
                {
                    // spell listed as forward and not listed as ability
                    // this is marker for removed ranks
                    if (ab_itr->second->forward_spellid == spell_id)
                    {
                        sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` is removed rank by DBC data.",
                                        spell_id, node.prev, node.first, node.rank, node.req);
                        skip = true;
                        break;
                    }
                }
            }

            if (skip)
                continue;
        }

        mSpellChains[spell_id] = node;

        ++new_count;
    }
    while (queryResult->NextRow());

    // additional integrity checks
    for (SpellChainMap::const_iterator i = mSpellChains.begin(); i != mSpellChains.end(); ++i)
    {
        if (i->second.prev)
        {
            SpellChainMap::const_iterator i_prev = mSpellChains.find(i->second.prev);
            if (i_prev == mSpellChains.end())
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has not found previous rank spell in table.",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req);
            }
            else if (i_prev->second.first != i->second.first)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has different first spell in chain compared to previous rank spell (prev: %u, first: %u, rank: %d, req: %u).",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req,
                                i_prev->second.prev, i_prev->second.first, i_prev->second.rank, i_prev->second.req);
            }
            else if (i_prev->second.rank + 1 != i->second.rank)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has different rank compared to previous rank spell (prev: %u, first: %u, rank: %d, req: %u).",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req,
                                i_prev->second.prev, i_prev->second.first, i_prev->second.rank, i_prev->second.req);
            }
        }

        if (i->second.req)
        {
            SpellChainMap::const_iterator i_req = mSpellChains.find(i->second.req);
            if (i_req == mSpellChains.end())
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has not found required rank spell in table.",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req);
            }
            else if (i_req->second.first == i->second.first)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has required rank spell from same spell chain (prev: %u, first: %u, rank: %d, req: %u).",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req,
                                i_req->second.prev, i_req->second.first, i_req->second.rank, i_req->second.req);
            }
            else if (i_req->second.req)
            {
                sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has required rank spell with required spell (prev: %u, first: %u, rank: %d, req: %u).",
                                i->first, i->second.prev, i->second.first, i->second.rank, i->second.req,
                                i_req->second.prev, i_req->second.first, i_req->second.rank, i_req->second.req);
            }
        }
    }

    // fill next rank cache
    for (SpellChainMap::const_iterator i = mSpellChains.begin(); i != mSpellChains.end(); ++i)
    {
        uint32 spell_id = i->first;
        SpellChainNode const& node = i->second;

        if (node.prev)
            mSpellChainsNext.insert(SpellChainMapNext::value_type(node.prev, spell_id));

        if (node.req)
            mSpellChainsNext.insert(SpellChainMapNext::value_type(node.req, spell_id));
    }

    // check single rank redundant cases (single rank talents/spell abilities not added by default so this can be only custom cases)
    for (SpellChainMap::const_iterator i = mSpellChains.begin(); i != mSpellChains.end(); ++i)
    {
        // skip non-first ranks, and spells with additional reqs
        if (i->second.rank > 1 || i->second.req)
            continue;

        if (mSpellChainsNext.find(i->first) == mSpellChainsNext.end())
        {
            sLog.outErrorDb("Spell %u (prev: %u, first: %u, rank: %d, req: %u) listed in `spell_chain` has single rank data, so redundant.",
                            i->first, i->second.prev, i->second.first, i->second.rank, i->second.req);
        }
    }

    sLog.outString(">> Loaded %u spell chain records (%u from DBC data with %u req field updates, and %u loaded from table)", dbc_count + new_count, dbc_count, req_count, new_count);
    sLog.outString();
}

void SpellMgr::LoadSpellLearnSkills()
{
    mSpellLearnSkills.clear();                              // need for reload case

    // search auto-learned skills and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    BarGoLink bar(sSpellTemplate.GetMaxEntry());
    for (uint32 spell = 0; spell < sSpellTemplate.GetMaxEntry(); ++spell)
    {
        bar.step();
        SpellEntry const* entry = sSpellTemplate.LookupEntry<SpellEntry>(spell);

        if (!entry)
            continue;

        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_SKILL)
            {
                SpellLearnSkillNode dbc_node;
                dbc_node.skill    = entry->EffectMiscValue[i];
                dbc_node.step     = entry->CalculateSimpleValue(SpellEffectIndex(i));
                dbc_node.effect   = SpellEffects(entry->Effect[i]);

                if (dbc_node.skill && dbc_node.step)
                {
                    mSpellLearnSkills[spell] = dbc_node;
                    ++dbc_count;
                    break;
                }
            }
        }
    }

    sLog.outString(">> Loaded %u Spell Learn Skills from DBC", dbc_count);
    sLog.outString();
}

void SpellMgr::LoadSpellLearnSpells()
{
    mSpellLearnSpells.clear();                              // need for reload case

    //                                             0      1        2
    auto queryResult = WorldDatabase.Query("SELECT entry, SpellID, Active FROM spell_learn_spell");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded 0 spell learn spells");
        sLog.outErrorDb("`spell_learn_spell` table is empty!");
        return;
    }

    uint32 count = 0;

    BarGoLink bar(queryResult->GetRowCount());
    do
    {
        bar.step();
        Field* fields = queryResult->Fetch();

        uint32 spell_id    = fields[0].GetUInt32();

        SpellLearnSpellNode node;
        node.spell      = fields[1].GetUInt32();
        node.active     = fields[2].GetBool();
        node.autoLearned = false;

        if (!sSpellTemplate.LookupEntry<SpellEntry>(spell_id))
        {
            sLog.outErrorDb("Spell %u listed in `spell_learn_spell` does not exist", spell_id);
            continue;
        }

        if (!sSpellTemplate.LookupEntry<SpellEntry>(node.spell))
        {
            sLog.outErrorDb("Spell %u listed in `spell_learn_spell` learning nonexistent spell %u", spell_id, node.spell);
            continue;
        }

        if (GetTalentSpellCost(node.spell))
        {
            sLog.outErrorDb("Spell %u listed in `spell_learn_spell` attempt learning talent spell %u, skipped", spell_id, node.spell);
            continue;
        }

        mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell_id, node));

        ++count;
    }
    while (queryResult->NextRow());

    // search auto-learned spells and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < sSpellTemplate.GetMaxEntry(); ++spell)
    {
        SpellEntry const* entry = sSpellTemplate.LookupEntry<SpellEntry>(spell);

        if (!entry)
            continue;

        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_LEARN_SPELL)
            {
                SpellLearnSpellNode dbc_node;
                dbc_node.spell       = entry->EffectTriggerSpell[i];
                dbc_node.active      = true;                // all dbc based learned spells is active (show in spell book or hide by client itself)

                // ignore learning nonexistent spells (broken/outdated/or generic learning spell 483
                if (!sSpellTemplate.LookupEntry<SpellEntry>(dbc_node.spell))
                    continue;

                // talent or passive spells or skill-step spells auto-casted and not need dependent learning,
                // pet teaching spells don't must be dependent learning (casted)
                // other required explicit dependent learning
                dbc_node.autoLearned = entry->EffectImplicitTargetA[i] == TARGET_UNIT_CASTER_PET || GetTalentSpellCost(spell) > 0 || IsPassiveSpell(entry) || IsSpellHaveEffect(entry, SPELL_EFFECT_SKILL_STEP);

                SpellLearnSpellMapBounds db_node_bounds = GetSpellLearnSpellMapBounds(spell);

                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_bounds.first; itr != db_node_bounds.second; ++itr)
                {
                    if (itr->second.spell == dbc_node.spell)
                    {
                        sLog.outErrorDb("Spell %u auto-learn spell %u in spell.dbc then the record in `spell_learn_spell` is redundant, please fix DB.",
                                        spell, dbc_node.spell);
                        found = true;
                        break;
                    }
                }

                if (!found)                                 // add new spell-spell pair if not found
                {
                    mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell, dbc_node));
                    ++dbc_count;
                }
            }
        }
    }

    sLog.outString(">> Loaded %u spell learn spells + %u found in DBC", count, dbc_count);
    sLog.outString();
}

void SpellMgr::LoadSpellScriptTarget()
{
    sSpellScriptTargetStorage.Load();

    // Check content
    for (SQLMultiStorage::SQLSIterator<SpellTargetEntry> itr = sSpellScriptTargetStorage.getDataBegin<SpellTargetEntry>(); itr < sSpellScriptTargetStorage.getDataEnd<SpellTargetEntry>(); ++itr)
    {
        SpellEntry const* spellProto = sSpellTemplate.LookupEntry<SpellEntry>(itr->spellId);
        if (!spellProto)
        {
            sLog.outErrorDb("Table `spell_script_target`: spellId %u listed for TargetEntry %u does not exist.", itr->spellId, itr->targetEntry);
            sSpellScriptTargetStorage.EraseEntry(itr->spellId);
            continue;
        }

        bool targetfound = false;
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (spellProto->EffectImplicitTargetA[i] == TARGET_UNIT_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_UNIT_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_LOCATION_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_LOCATION_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_GAMEOBJECT_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_GAMEOBJECT_SCRIPT_NEAR_CASTER ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DEST_LOC ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DEST_LOC ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_SRC_LOC ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_SRC_LOC ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_UNITS_SCRIPT_IN_CONE_60 ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_UNITS_SCRIPT_IN_CONE_60 ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_UNITS_SCRIPT_IN_CONE_110 ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_UNITS_SCRIPT_IN_CONE_110 ||
                    spellProto->EffectImplicitTargetA[i] == TARGET_ENUM_UNITS_ENEMY_IN_CONE_54 ||
                    spellProto->EffectImplicitTargetB[i] == TARGET_ENUM_UNITS_ENEMY_IN_CONE_54)
            {
                targetfound = true;
                break;
            }
        }
        if (!targetfound)
        {
            sLog.outErrorDb("Table `spell_script_target`: spellId %u listed for TargetEntry %u does not have any implicit target TARGET_UNIT_SCRIPT_NEAR_CASTER(38) or TARGET_LOCATION_SCRIPT_NEAR_CASTER (46) or TARGET_GAMEOBJECT_SCRIPT_NEAR_CASTER (40).", itr->spellId, itr->targetEntry);
            sSpellScriptTargetStorage.EraseEntry(itr->spellId);
            continue;
        }

        if (itr->type >= MAX_SPELL_TARGET_TYPE)
        {
            sLog.outErrorDb("Table `spell_script_target`: target type %u for TargetEntry %u is incorrect.", itr->type, itr->targetEntry);
            sSpellScriptTargetStorage.EraseEntry(itr->spellId);
            continue;
        }

        // Checks by target type
        switch (itr->type)
        {
            case SPELL_TARGET_TYPE_GAMEOBJECT:
            {
                if (!itr->targetEntry)
                    break;

                if (!sGOStorage.LookupEntry<GameObjectInfo>(itr->targetEntry))
                {
                    sLog.outErrorDb("Table `spell_script_target`: gameobject template entry %u does not exist.", itr->targetEntry);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                }
                break;
            }
            case SPELL_TARGET_TYPE_GAMEOBJECT_GUID:
            {
                if (!itr->targetEntry)
                    break;

                if (!sObjectMgr.GetGOData(itr->targetEntry))
                {
                    sLog.outErrorDb("Table `spell_script_target`: gameobject guid %u does not exist.", itr->targetEntry);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                }
                break;
            }
            case SPELL_TARGET_TYPE_CREATURE_GUID:
            {
                if (!sObjectMgr.GetCreatureData(itr->targetEntry))
                {
                    sLog.outErrorDb("Table `spell_script_target`: creature guid %u does not exist.", itr->targetEntry);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                }
                break;
            }
            case SPELL_TARGET_TYPE_STRING_ID:
            {
                if (!sScriptMgr.ExistsStringId(itr->targetEntry))
                {
                    sLog.outErrorDb("Table `spell_script_target`: stringId %u does not exist.", itr->targetEntry);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                }
                break;
            }
            default:
                if (!itr->targetEntry)
                {
                    sLog.outErrorDb("Table `spell_script_target`: target entry == 0 for not GO target type (%u).", itr->type);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                    continue;
                }
                if (const CreatureInfo* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(itr->targetEntry))
                {
                    if (itr->spellId == 30427 && !cInfo->SkinningLootId)
                    {
                        sLog.outErrorDb("Table `spell_script_target` has creature %u as a target of spellid 30427, but this creature has no SkinningLootId. Gas extraction will not work!", cInfo->Entry);
                        sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                    }
                }
                else
                {
                    sLog.outErrorDb("Table `spell_script_target`: creature template entry %u does not exist.", itr->targetEntry);
                    sSpellScriptTargetStorage.EraseEntry(itr->spellId);
                }
                break;
        }
    }

    // Check all spells
    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
    {
        for (uint32 i = 1; i < sSpellTemplate.GetMaxEntry(); ++i)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(i);
            if (!spellInfo)
                continue;

            for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
            {
                if (spellInfo->Effect[j] && (spellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_SCRIPT_NEAR_CASTER ||
                                             (spellInfo->EffectImplicitTargetA[j] != TARGET_UNIT_CASTER && spellInfo->EffectImplicitTargetB[j] == TARGET_UNIT_SCRIPT_NEAR_CASTER)))
                {
                    SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(i);
                    if (bounds.first == bounds.second)
                    {
                        sLog.outErrorDb("Spell (ID: %u) has effect EffectImplicitTargetA/EffectImplicitTargetB = %u (TARGET_UNIT_SCRIPT_NEAR_CASTER), but does not have record in `spell_script_target`", spellInfo->Id, TARGET_UNIT_SCRIPT_NEAR_CASTER);
                        break;                              // effects of spell
                    }
                }
            }
        }
    }

    sLog.outString(">> Loaded %u spell_script_target definitions", sSpellScriptTargetStorage.GetRecordCount());
    sLog.outString();
}

void SpellMgr::LoadSpellPetAuras()
{
    mSpellPetAuraMap.clear();                               // need for reload case

    uint32 count = 0;

    //                                             0      1         2    3
    auto queryResult = WorldDatabase.Query("SELECT spell, effectId, pet, aura FROM spell_pet_auras");
    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u spell pet auras", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 spell = fields[0].GetUInt32();
        SpellEffectIndex eff = SpellEffectIndex(fields[1].GetUInt32());
        uint32 pet = fields[2].GetUInt32();
        uint32 aura = fields[3].GetUInt32();

        if (eff >= MAX_EFFECT_INDEX)
        {
            sLog.outErrorDb("Spell %u listed in `spell_pet_auras` with wrong spell effect index (%u)", spell, eff);
            continue;
        }

        SpellPetAuraMap::iterator itr = mSpellPetAuraMap.find((spell << 8) + eff);
        if (itr != mSpellPetAuraMap.end())
        {
            itr->second.AddAura(pet, aura);
        }
        else
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spell);
            if (!spellInfo)
            {
                sLog.outErrorDb("Spell %u listed in `spell_pet_auras` does not exist", spell);
                continue;
            }

            if (spellInfo->Effect[eff] != SPELL_EFFECT_DUMMY &&
                    (spellInfo->Effect[eff] != SPELL_EFFECT_APPLY_AURA ||
                     spellInfo->EffectApplyAuraName[eff] != SPELL_AURA_DUMMY))
            {
                sLog.outError("Spell %u listed in `spell_pet_auras` does not have dummy aura or dummy effect", spell);
                continue;
            }

            SpellEntry const* spellInfo2 = sSpellTemplate.LookupEntry<SpellEntry>(aura);
            if (!spellInfo2)
            {
                sLog.outErrorDb("Aura %u listed in `spell_pet_auras` does not exist", aura);
                continue;
            }

            PetAura pa(pet, aura, spellInfo->EffectImplicitTargetA[eff] == TARGET_UNIT_CASTER_PET, spellInfo->CalculateSimpleValue(eff));
            mSpellPetAuraMap[(spell << 8) + eff] = pa;
        }

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u spell pet auras", count);
    sLog.outString();
}

void SpellMgr::LoadPetLevelupSpellMap()
{
    uint32 count = 0;
    uint32 family_count = 0;

    for (uint32 i = 0; i < sCreatureFamilyStore.GetNumRows(); ++i)
    {
        CreatureFamilyEntry const* creatureFamily = sCreatureFamilyStore.LookupEntry(i);
        if (!creatureFamily)                                // not exist
            continue;

        for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
        {
            SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
            if (!skillLine)
                continue;

            if (skillLine->skillId != creatureFamily->skillLine[0] &&
                    (!creatureFamily->skillLine[1] || skillLine->skillId != creatureFamily->skillLine[1]))
                continue;

            if (skillLine->learnOnGetSkill != ABILITY_LEARNED_ON_GET_RACE_OR_CLASS_SKILL)
                continue;

            SpellEntry const* spell = sSpellTemplate.LookupEntry<SpellEntry>(skillLine->spellId);
            if (!spell)                                     // not exist
                continue;

            PetLevelupSpellSet& spellSet = mPetLevelupSpellMap[creatureFamily->ID];
            if (spellSet.empty())
                ++family_count;

            spellSet.insert(PetLevelupSpellSet::value_type(spell->spellLevel, spell->Id));
            ++count;
        }
    }

    sLog.outString(">> Loaded %u pet levelup and default spells for %u families", count, family_count);
    sLog.outString();
}

bool SpellMgr::LoadPetDefaultSpells_helper(CreatureInfo const* cInfo, PetDefaultSpellsEntry& petDefSpells)
{
    // skip empty list;
    bool have_spell = false;
    for (unsigned int j : petDefSpells.spellid)
    {
        if (j)
        {
            have_spell = true;
            break;
        }
    }
    if (!have_spell)
        return false;

    // remove duplicates with levelupSpells if any
    if (PetLevelupSpellSet const* levelupSpells = cInfo->Family ? GetPetLevelupSpellList(cInfo->Family) : nullptr)
    {
        for (unsigned int& j : petDefSpells.spellid)
        {
            if (!j)
                continue;

            for (const auto& levelupSpell : *levelupSpells)
            {
                if (levelupSpell.second == j)
                {
                    j = 0;
                    break;
                }
            }
        }
    }

    // skip empty list;
    have_spell = false;
    for (unsigned int j : petDefSpells.spellid)
    {
        if (j)
        {
            have_spell = true;
            break;
        }
    }

    return have_spell;
}

void SpellMgr::LoadPetDefaultSpells()
{
    MANGOS_ASSERT(MAX_CREATURE_SPELL_DATA_SLOT <= CREATURE_MAX_SPELLS);

    mPetDefaultSpellsMap.clear();

    uint32 countCreature = 0;
    uint32 countData = 0;

    for (uint32 i = 0; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i);
        if (!cInfo)
            continue;

        if (!cInfo->PetSpellDataId)
            continue;

        // for creature with PetSpellDataId get default pet spells from dbc
        CreatureSpellDataEntry const* spellDataEntry = sCreatureSpellDataStore.LookupEntry(cInfo->PetSpellDataId);
        if (!spellDataEntry)
            continue;

        int32 petSpellsId = -(int32)cInfo->PetSpellDataId;
        PetDefaultSpellsEntry petDefSpells;
        for (int j = 0; j < MAX_CREATURE_SPELL_DATA_SLOT; ++j)
            petDefSpells.spellid.push_back(spellDataEntry->spellId[j]);

        if (LoadPetDefaultSpells_helper(cInfo, petDefSpells))
        {
            mPetDefaultSpellsMap[petSpellsId] = petDefSpells;
            ++countData;
        }
    }

    // different summon spells
    for (uint32 i = 0; i < sSpellTemplate.GetMaxEntry(); ++i)
    {
        SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(i);
        if (!spellEntry)
            continue;

        for (int k = 0; k < MAX_EFFECT_INDEX; ++k)
        {
            if (spellEntry->Effect[k] == SPELL_EFFECT_SUMMON || spellEntry->Effect[k] == SPELL_EFFECT_SUMMON_PET)
            {
                uint32 creature_id = spellEntry->EffectMiscValue[k];
                CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(creature_id);
                if (!cInfo)
                    continue;

                // already loaded
                if (cInfo->PetSpellDataId)
                    continue;

                // for creature without PetSpellDataId get default pet spells from creature_template
                int32 petSpellsId = cInfo->Entry;
                if (mPetDefaultSpellsMap.find(cInfo->Entry) != mPetDefaultSpellsMap.end())
                    continue;

                PetDefaultSpellsEntry petDefSpells;
                if (CreatureSpellList* spellList = sObjectMgr.GetCreatureSpellList(cInfo->SpellList ? cInfo->SpellList : cInfo->Entry * 100))
                    for (auto& data : spellList->Spells)
                        petDefSpells.spellid.push_back(data.second.SpellId);

                if (LoadPetDefaultSpells_helper(cInfo, petDefSpells))
                {
                    mPetDefaultSpellsMap[petSpellsId] = petDefSpells;
                    ++countCreature;
                }
            }
        }
    }

    sLog.outString(">> Loaded addition spells for %u pet spell data entries and %u summonable creature templates", countData, countCreature);
    sLog.outString();
}

/// Some checks for spells, to prevent adding deprecated/broken spells for trainers, spell book, etc
bool SpellMgr::IsSpellValid(SpellEntry const* spellInfo, Player* pl, bool msg)
{
    // not exist
    if (!spellInfo)
        return false;

    bool need_check_reagents = false;

    // check effects
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        switch (spellInfo->Effect[i])
        {
            case SPELL_EFFECT_NONE:
                continue;

            // craft spell for crafting nonexistent item (break client recipes list show)
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_CREATE_ITEM_2:
            {
                if (spellInfo->EffectItemType[i] == 0)
                {
                    // skip auto-loot crafting spells, its not need explicit item info (but have special fake items sometime)
                    if (!IsLootCraftingSpell(spellInfo))
                    {
                        if (msg)
                        {
                            if (pl)
                                ChatHandler(pl).PSendSysMessage("Craft spell %u not have create item entry.", spellInfo->Id);
                            else
                                sLog.outErrorDb("Craft spell %u not have create item entry.", spellInfo->Id);
                        }
                        return false;
                    }
                }
                // also possible IsLootCraftingSpell case but fake item must exist anyway
                else if (!ObjectMgr::GetItemPrototype(spellInfo->EffectItemType[i]))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Craft spell %u create item (Entry: %u) but item does not exist in item_template.", spellInfo->Id, spellInfo->EffectItemType[i]);
                        else
                            sLog.outErrorDb("Craft spell %u create item (Entry: %u) but item does not exist in item_template.", spellInfo->Id, spellInfo->EffectItemType[i]);
                    }
                    return false;
                }

                need_check_reagents = true;
                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                SpellEntry const* spellInfo2 = sSpellTemplate.LookupEntry<SpellEntry>(spellInfo->EffectTriggerSpell[i]);
                if (!IsSpellValid(spellInfo2, pl, msg))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Spell %u learn to broken spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                        else
                            sLog.outErrorDb("Spell %u learn to invalid spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                    }
                    return false;
                }
                break;
            }
        }
    }

    if (need_check_reagents)
    {
        for (int j : spellInfo->Reagent)
        {
            if (j > 0 && !ObjectMgr::GetItemPrototype(j))
            {
                if (msg)
                {
                    if (pl)
                        ChatHandler(pl).PSendSysMessage("Craft spell %u requires reagent item (Entry: %u) but item does not exist in item_template.", spellInfo->Id, j);
                    else
                        sLog.outErrorDb("Craft spell %u requires reagent item (Entry: %u) but item does not exist in item_template.", spellInfo->Id, j);
                }
                return false;
            }
        }
    }

    return true;
}

void SpellMgr::LoadSpellAreas()
{
    mSpellAreaMap.clear();                                  // need for reload case
    mSpellAreaForAuraMap.clear();

    uint32 count = 0;

    //                                             0      1     2            3                   4          5             6           7         8       9
    auto queryResult = WorldDatabase.Query("SELECT spell, area, quest_start, quest_start_active, quest_end, condition_id, aura_spell, racemask, gender, autocast FROM spell_area");

    if (!queryResult)
    {
        BarGoLink bar(1);
        bar.step();
        sLog.outString(">> Loaded %u spell area requirements", count);
        sLog.outString();
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 spell = fields[0].GetUInt32();
        SpellArea spellArea;
        spellArea.spellId             = spell;
        spellArea.areaId              = fields[1].GetUInt32();
        spellArea.questStart          = fields[2].GetUInt32();
        spellArea.questStartCanActive = fields[3].GetBool();
        spellArea.questEnd            = fields[4].GetUInt32();
        spellArea.conditionId         = fields[5].GetUInt16();
        spellArea.auraSpell           = fields[6].GetInt32();
        spellArea.raceMask            = fields[7].GetUInt32();
        spellArea.gender              = Gender(fields[8].GetUInt8());
        spellArea.autocast            = fields[9].GetBool();

        if (!sSpellTemplate.LookupEntry<SpellEntry>(spell))
        {
            sLog.outErrorDb("Spell %u listed in `spell_area` does not exist", spell);
            continue;
        }

        {
            bool ok = true;
            SpellAreaMapBounds sa_bounds = GetSpellAreaMapBounds(spellArea.spellId);
            for (SpellAreaMap::const_iterator itr = sa_bounds.first; itr != sa_bounds.second; ++itr)
            {
                if (spellArea.spellId != itr->second.spellId)
                    continue;
                if (spellArea.areaId != itr->second.areaId)
                    continue;
                if (spellArea.questStart != itr->second.questStart)
                    continue;
                if (spellArea.auraSpell != itr->second.auraSpell)
                    continue;
                if ((spellArea.raceMask & itr->second.raceMask) == 0)
                    continue;
                if (spellArea.gender != itr->second.gender)
                    continue;

                // duplicate by requirements
                ok = false;
                break;
            }

            if (!ok)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` already listed with similar requirements.", spell);
                continue;
            }
        }

        if (spellArea.areaId && !GetAreaEntryByAreaID(spellArea.areaId))
        {
            sLog.outErrorDb("Spell %u listed in `spell_area` have wrong area (%u) requirement", spell, spellArea.areaId);
            continue;
        }

        if (spellArea.conditionId && !sConditionStorage.LookupEntry<ConditionEntry>(spellArea.conditionId))
        {
            sLog.outErrorDb("Spell %u listed in `spell_area` have wrong conditionId (%u) requirement", spell, spellArea.conditionId);
            continue;
        }
        if (!spellArea.conditionId)
        {
            if (spellArea.questStart && !sObjectMgr.GetQuestTemplate(spellArea.questStart))
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have wrong start quest (%u) requirement", spell, spellArea.questStart);
                continue;
            }

            if (spellArea.questEnd)
            {
                if (!sObjectMgr.GetQuestTemplate(spellArea.questEnd))
                {
                    sLog.outErrorDb("Spell %u listed in `spell_area` have wrong end quest (%u) requirement", spell, spellArea.questEnd);
                    continue;
                }

                if (spellArea.questEnd == spellArea.questStart && !spellArea.questStartCanActive)
                {
                    sLog.outErrorDb("Spell %u listed in `spell_area` have quest (%u) requirement for start and end in same time", spell, spellArea.questEnd);
                    continue;
                }
            }

            if (spellArea.raceMask && (spellArea.raceMask & RACEMASK_ALL_PLAYABLE) == 0)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have wrong race mask (%u) requirement", spell, spellArea.raceMask);
                continue;
            }

            if (spellArea.gender != GENDER_NONE && spellArea.gender != GENDER_FEMALE && spellArea.gender != GENDER_MALE)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have wrong gender (%u) requirement", spell, spellArea.gender);
                continue;
            }
        }

        if (spellArea.auraSpell)
        {
            SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(abs(spellArea.auraSpell));
            if (!spellInfo)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have wrong aura spell (%u) requirement", spell, abs(spellArea.auraSpell));
                continue;
            }

            bool validSpellEffect = false;
            for (uint32 i = EFFECT_INDEX_0; i < MAX_EFFECT_INDEX; ++i)
            {
                switch (spellInfo->EffectApplyAuraName[i])
                {
                    case SPELL_AURA_DUMMY:
                    case SPELL_AURA_PHASE:
                    case SPELL_AURA_GHOST:
                        validSpellEffect = true;
                        break;
                }
            }

            if (!validSpellEffect)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have aura spell requirement (%u) without dummy/phase/ghost aura in spell effects", spell, abs(spellArea.auraSpell));
                continue;
            }

            if (uint32(abs(spellArea.auraSpell)) == spellArea.spellId)
            {
                sLog.outErrorDb("Spell %u listed in `spell_area` have aura spell (%u) requirement for itself", spell, abs(spellArea.auraSpell));
                continue;
            }

            // not allow autocast chains by auraSpell field (but allow use as alternative if not present)
            if (spellArea.autocast && spellArea.auraSpell > 0)
            {
                bool chain = false;
                SpellAreaForAuraMapBounds saBound = GetSpellAreaForAuraMapBounds(spellArea.spellId);
                for (SpellAreaForAuraMap::const_iterator itr = saBound.first; itr != saBound.second; ++itr)
                {
                    if (itr->second->autocast && itr->second->auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    sLog.outErrorDb("Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }

                SpellAreaMapBounds saBound2 = GetSpellAreaMapBounds(spellArea.auraSpell);
                for (SpellAreaMap::const_iterator itr2 = saBound2.first; itr2 != saBound2.second; ++itr2)
                {
                    if (itr2->second.autocast && itr2->second.auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    sLog.outErrorDb("Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }
            }
        }

        SpellArea const* sa = &mSpellAreaMap.insert(SpellAreaMap::value_type(spell, spellArea))->second;

        // for search by current zone/subzone at zone/subzone change
        if (spellArea.areaId)
            mSpellAreaForAreaMap.insert(SpellAreaForAreaMap::value_type(spellArea.areaId, sa));

        // for search at aura apply
        if (spellArea.auraSpell)
            mSpellAreaForAuraMap.insert(SpellAreaForAuraMap::value_type(abs(spellArea.auraSpell), sa));

        ++count;
    }
    while (queryResult->NextRow());

    sLog.outString(">> Loaded %u spell area requirements", count);
    sLog.outString();
}

SpellCastResult SpellMgr::GetSpellAllowedInLocationError(SpellEntry const* spellInfo, uint32 map_id, uint32 zone_id, uint32 area_id, Player const* player) const
{
    // normal case
    int32 areaGroupId = spellInfo->AreaGroupId;
    if (areaGroupId > 0)
    {
        bool found = false;
        AreaGroupEntry const* groupEntry = sAreaGroupStore.LookupEntry<AreaGroupEntry>(areaGroupId);
        while (groupEntry)
        {
            for (unsigned int i : groupEntry->AreaId)
                if (i == zone_id || i == area_id)
                    found = true;
            if (found || !groupEntry->nextGroup)
                break;
            // Try search in next group
            groupEntry = sAreaGroupStore.LookupEntry<AreaGroupEntry>(groupEntry->nextGroup);
        }

        if (!found)
            return SPELL_FAILED_INCORRECT_AREA;
    }

    // continent limitation (virtual continent), ignore for GM
    if (spellInfo->HasAttribute(SPELL_ATTR_EX4_ONLY_FLYING_AREAS) && !(player && player->IsGameMaster()))
    {
        uint32 v_map = GetVirtualMapForMapAndZone(map_id, zone_id);
        MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
        if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
            return SPELL_FAILED_INCORRECT_AREA;
    }

    // raid instance limitation
    if (spellInfo->HasAttribute(SPELL_ATTR_EX6_NOT_IN_RAID_INSTANCES))
    {
        MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
        if (!mapEntry || mapEntry->IsRaid())
            return SPELL_FAILED_NOT_IN_RAID_INSTANCE;
    }

    // DB base check (if non empty then must fit at least single for allow)
    SpellAreaMapBounds saBounds = GetSpellAreaMapBounds(spellInfo->Id);
    if (saBounds.first != saBounds.second)
    {
        for (SpellAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
        {
            if (itr->second.IsFitToRequirements(player, zone_id, area_id))
                return SPELL_CAST_OK;
        }
        return SPELL_FAILED_INCORRECT_AREA;
    }

    // bg spell checks

    // do not allow spells to be cast in arenas
    // - with SPELL_ATTR_EX4_NOT_USABLE_IN_ARENA flag
    // - with greater than 10 min CD
    if (spellInfo->HasAttribute(SPELL_ATTR_EX4_NOT_IN_ARENA) ||
            (GetSpellRecoveryTime(spellInfo) > 10 * MINUTE * IN_MILLISECONDS && !spellInfo->HasAttribute(SPELL_ATTR_EX4_IGNORE_DEFAULT_ARENA_RESTRICTIONS)))
        if (player && player->InArena())
            return SPELL_FAILED_NOT_IN_ARENA;

    // Spell casted only on battleground
    if (spellInfo->HasAttribute(SPELL_ATTR_EX3_ONLY_BATTLEGROUNDS))
        if (!player || !player->InBattleGround())
            return SPELL_FAILED_ONLY_BATTLEGROUNDS;

    switch (spellInfo->Id)
    {
        // a trinket in alterac valley allows to teleport to the boss
        case 22564:                                         // recall
        case 22563:                                         // recall
        {
            if (!player)
                return SPELL_FAILED_REQUIRES_AREA;
            BattleGround* bg = player->GetBattleGround();
            return map_id == 30 && bg
                   && bg->GetStatus() != STATUS_WAIT_JOIN ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
        case 2584:                                          // Waiting to Resurrect
        case 42792:                                         // Recently Dropped Flag
        case 44521:                                         // Preparation
        case 22011:                                         // Spirit Heal Channel
        case 22012:                                         // Spirit Heal
        case 24171:                                         // Resurrection Impact Visual
        case 44535:                                         // Spirit Heal (mana)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_INCORRECT_AREA;
            return mapEntry->IsBattleGround() || zone_id == 4197 ? SPELL_CAST_OK : SPELL_FAILED_ONLY_BATTLEGROUNDS;
        }
        case 32724:                                         // Gold Team (Alliance)
        case 32725:                                         // Green Team (Alliance)
        case 35774:                                         // Gold Team (Horde)
        case 35775:                                         // Green Team (Horde)
        {
            return player && player->InArena() ? SPELL_CAST_OK : SPELL_FAILED_ONLY_IN_ARENA;
        }
        case 32727:                                         // Arena Preparation
        {
            if (!player)
                return SPELL_FAILED_REQUIRES_AREA;
            if (!player->InArena())
                return SPELL_FAILED_REQUIRES_AREA;

            BattleGround* bg = player->GetBattleGround();
            return bg && bg->GetStatus() == STATUS_WAIT_JOIN ? SPELL_CAST_OK : SPELL_FAILED_ONLY_IN_ARENA;
        }
        case 74410:                                         // Arena - Dampening
            return player && player->InArena() ? SPELL_CAST_OK : SPELL_FAILED_ONLY_IN_ARENA;
        case 74411:                                         // Battleground - Dampening
        {
            if (!player)
                return SPELL_FAILED_ONLY_BATTLEGROUNDS;

            BattleGround* bg = player->GetBattleGround();
            return bg && !bg->IsArena() ? SPELL_CAST_OK : SPELL_FAILED_ONLY_BATTLEGROUNDS;
        }
    }

    return SPELL_CAST_OK;
}

void SpellMgr::LoadSkillLineAbilityMaps()
{
    mSkillLineAbilityMapBySpellId.clear();
    mSkillLineAbilityMapBySkillId.clear();

    const uint32 rows = sSkillLineAbilityStore.GetNumRows();
    uint32 count = 0;

    BarGoLink bar(rows);
    for (uint32 row = 0; row < rows; ++row)
    {
        bar.step();
        if (SkillLineAbilityEntry const* entry = sSkillLineAbilityStore.LookupEntry(row))
        {
            mSkillLineAbilityMapBySpellId.insert(SkillLineAbilityMap::value_type(entry->spellId, entry));
            mSkillLineAbilityMapBySkillId.insert(SkillLineAbilityMap::value_type(entry->skillId, entry));
            ++count;
        }
    }

    sLog.outString(">> Loaded %u SkillLineAbility MultiMaps Data", count);
    sLog.outString();
}

void SpellMgr::LoadSkillRaceClassInfoMap()
{
    mSkillRaceClassInfoMap.clear();

    BarGoLink bar(sSkillRaceClassInfoStore.GetNumRows());
    uint32 count = 0;

    for (uint32 i = 0; i < sSkillRaceClassInfoStore.GetNumRows(); ++i)
    {
        bar.step();
        SkillRaceClassInfoEntry const* skillRCInfo = sSkillRaceClassInfoStore.LookupEntry(i);
        if (!skillRCInfo)
            continue;

        // not all skills really listed in ability skills list
        if (!sSkillLineStore.LookupEntry(skillRCInfo->skillId))
            continue;

        mSkillRaceClassInfoMap.insert(SkillRaceClassInfoMap::value_type(skillRCInfo->skillId, skillRCInfo));

        ++count;
    }

    sLog.outString(">> Loaded %u SkillRaceClassInfo MultiMap Data", count);
    sLog.outString();
}

void SpellMgr::CheckUsedSpells(char const* table) const
{
    uint32 countSpells = 0;
    uint32 countMasks = 0;

    //                                                 0       1               2                3                4         5           6             7          8          9         10   11
    auto queryResult = WorldDatabase.PQuery("SELECT spellid,SpellFamilyName,SpellFamilyMaskA,SpellFamilyMaskB,SpellIcon,SpellVisual,SpellCategory,EffectType,EffectAura,EffectIdx,Name,Code FROM %s", table);

    if (!queryResult)
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        sLog.outErrorDb("`%s` table is empty!", table);
        return;
    }

    BarGoLink bar(queryResult->GetRowCount());

    do
    {
        Field* fields = queryResult->Fetch();

        bar.step();

        uint32 spell       = fields[0].GetUInt32();
        int32  family      = fields[1].GetInt32();
        uint64 familyMaskA = fields[2].GetUInt64();
        uint32 familyMaskB = fields[3].GetUInt32();
        int32  spellIcon   = fields[4].GetInt32();
        int32  spellVisual = fields[5].GetInt32();
        int32  category    = fields[6].GetInt32();
        int32  effectType  = fields[7].GetInt32();
        int32  auraType    = fields[8].GetInt32();
        int32  effectIdx   = fields[9].GetInt32();
        std::string name   = fields[10].GetCppString();
        std::string code   = fields[11].GetCppString();

        // checks of correctness requirements itself

        if (family < -1 || family > SPELLFAMILY_PET)
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellFamily value(%u), skipped.", table, spell, family);
            continue;
        }

        // TODO: spellIcon check need dbc loading
        if (spellIcon < -1)
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellIcon value(%u), skipped.", table, spell, spellIcon);
            continue;
        }

        // TODO: spellVisual check need dbc loading
        if (spellVisual < -1)
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellVisual value(%u), skipped.", table, spell, spellVisual);
            continue;
        }

        // TODO: for spellCategory better check need dbc loading
        if (category < -1 || (category >= 0 && sSpellCategoryStore.LookupEntry(category)))
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellCategory value(%u), skipped.", table, spell, category);
            continue;
        }

        if (effectType < -1 || effectType >= MAX_SPELL_EFFECTS)
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellEffect type value(%u), skipped.", table, spell, effectType);
            continue;
        }

        if (auraType < -1 || auraType >= TOTAL_AURAS)
        {
            sLog.outError("Table '%s' for spell %u have wrong SpellAura type value(%u), skipped.", table, spell, auraType);
            continue;
        }

        if (effectIdx < -1 || effectIdx >= 3)
        {
            sLog.outError("Table '%s' for spell %u have wrong EffectIdx value(%u), skipped.", table, spell, effectIdx);
            continue;
        }

        // now checks of requirements

        if (spell)
        {
            ++countSpells;

            SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spell);
            if (!spellEntry)
            {
                sLog.outError("Spell %u '%s' not exist but used in %s.", spell, name.c_str(), code.c_str());
                continue;
            }

            if (family >= 0 && spellEntry->SpellFamilyName != uint32(family))
            {
                sLog.outError("Spell %u '%s' family(%u) <> %u but used in %s.", spell, name.c_str(), spellEntry->SpellFamilyName, family, code.c_str());
                continue;
            }

            if (familyMaskA != uint64(0xFFFFFFFFFFFFFFFF) || familyMaskB != 0xFFFFFFFF)
            {
                if (familyMaskA == uint64(0x0000000000000000) && familyMaskB == 0x00000000)
                {
                    if (spellEntry->SpellFamilyFlags)
                    {
                        sLog.outError("Spell %u '%s' not fit to (" UI64FMTD ", %u) but used in %s.",
                                      spell, name.c_str(), familyMaskA, familyMaskB, code.c_str());
                        continue;
                    }
                }
                else
                {
                    if (!spellEntry->IsFitToFamilyMask(familyMaskA, familyMaskB))
                    {
                        sLog.outError("Spell %u '%s' not fit to (" UI64FMTD ", %u) but used in %s.", spell, name.c_str(), familyMaskA, familyMaskB, code.c_str());
                        continue;
                    }
                }
            }

            if (spellIcon >= 0 && spellEntry->SpellIconID != uint32(spellIcon))
            {
                sLog.outError("Spell %u '%s' icon(%u) <> %u but used in %s.", spell, name.c_str(), spellEntry->SpellIconID, spellIcon, code.c_str());
                continue;
            }

            if (spellVisual >= 0 && spellEntry->SpellVisual[0] != uint32(spellVisual))
            {
                sLog.outError("Spell %u '%s' visual(%u) <> %u but used in %s.", spell, name.c_str(), spellEntry->SpellVisual[0], spellVisual, code.c_str());
                continue;
            }

            if (category >= 0 && spellEntry->Category != uint32(category))
            {
                sLog.outError("Spell %u '%s' category(%u) <> %u but used in %s.", spell, name.c_str(), spellEntry->Category, category, code.c_str());
                continue;
            }

            if (effectIdx >= EFFECT_INDEX_0)
            {
                if (effectType >= 0 && spellEntry->Effect[effectIdx] != uint32(effectType))
                {
                    sLog.outError("Spell %u '%s' effect%d <> %u but used in %s.", spell, name.c_str(), effectIdx + 1, effectType, code.c_str());
                    continue;
                }

                if (auraType >= 0 && spellEntry->EffectApplyAuraName[effectIdx] != uint32(auraType))
                {
                    sLog.outError("Spell %u '%s' aura%d <> %u but used in %s.", spell, name.c_str(), effectIdx + 1, auraType, code.c_str());
                }
            }
            else
            {
                if (effectType >= 0 && !IsSpellHaveEffect(spellEntry, SpellEffects(effectType)))
                {
                    sLog.outError("Spell %u '%s' not have effect %u but used in %s.", spell, name.c_str(), effectType, code.c_str());
                    continue;
                }

                if (auraType >= 0 && !IsSpellHaveAura(spellEntry, AuraType(auraType)))
                {
                    sLog.outError("Spell %u '%s' not have aura %u but used in %s.", spell, name.c_str(), auraType, code.c_str());
                }
            }
        }
        else
        {
            ++countMasks;

            bool found = false;
            for (uint32 spellId = 1; spellId < sSpellTemplate.GetMaxEntry(); ++spellId)
            {
                SpellEntry const* spellEntry = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
                if (!spellEntry)
                    continue;

                if (family >= 0 && spellEntry->SpellFamilyName != uint32(family))
                    continue;

                if (familyMaskA != uint64(0xFFFFFFFFFFFFFFFF) || familyMaskB != 0xFFFFFFFF)
                {
                    if (familyMaskA == uint64(0x0000000000000000) && familyMaskB == 0x00000000)
                    {
                        if (spellEntry->SpellFamilyFlags)
                            continue;
                    }
                    else
                    {
                        if (!spellEntry->IsFitToFamilyMask(familyMaskA, familyMaskB))
                            continue;
                    }
                }

                if (spellIcon >= 0 && spellEntry->SpellIconID != uint32(spellIcon))
                    continue;

                if (spellVisual >= 0 && spellEntry->SpellVisual[0] != uint32(spellVisual))
                    continue;

                if (category >= 0 && spellEntry->Category != uint32(category))
                    continue;

                if (effectIdx >= 0)
                {
                    if (effectType >= 0 && spellEntry->Effect[effectIdx] != uint32(effectType))
                        continue;

                    if (auraType >= 0 && spellEntry->EffectApplyAuraName[effectIdx] != uint32(auraType))
                        continue;
                }
                else
                {
                    if (effectType >= 0 && !IsSpellHaveEffect(spellEntry, SpellEffects(effectType)))
                        continue;

                    if (auraType >= 0 && !IsSpellHaveAura(spellEntry, AuraType(auraType)))
                        continue;
                }

                found = true;
                break;
            }

            if (!found)
            {
                if (effectIdx >= 0)
                    sLog.outError("Spells '%s' not found for family %i (" UI64FMTD ", %u) icon(%i) visual(%i) category(%i) effect%d(%i) aura%d(%i) but used in %s",
                                  name.c_str(), family, familyMaskA, familyMaskB, spellIcon, spellVisual, category, effectIdx + 1, effectType, effectIdx + 1, auraType, code.c_str());
                else
                    sLog.outError("Spells '%s' not found for family %i (" UI64FMTD ", %u) icon(%i) visual(%i) category(%i) effect(%i) aura(%i) but used in %s",
                                  name.c_str(), family, familyMaskA, familyMaskB, spellIcon, spellVisual, category, effectType, auraType, code.c_str());
            }
        }
    }
    while (queryResult->NextRow());

    sLog.outString();
    sLog.outString(">> Checked %u spells and %u spell masks", countSpells, countMasks);
}

DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellEntry const* spellproto, bool triggered)
{
    // Explicit Diminishing Groups
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            // some generic arena related spells have by some strange reason MECHANIC_TURN
            if (spellproto->Mechanic == MECHANIC_TURN)
                return DIMINISHING_NONE;
            break;
        case SPELLFAMILY_MAGE:
            // Dragon's Breath
            if (spellproto->SpellIconID == 1548)
                return DIMINISHING_DISORIENT;
            break;
        case SPELLFAMILY_ROGUE:
        {
            // Blind
            if (spellproto->IsFitToFamilyMask(uint64(0x00001000000)))
                return DIMINISHING_FEAR_CHARM_BLIND;
            // Cheap Shot
            if (spellproto->IsFitToFamilyMask(uint64(0x00000000400)))
                return DIMINISHING_CHEAPSHOT_POUNCE;
                // Crippling poison - Limit to 10 seconds in PvP (No SpellFamilyFlags)
            if (spellproto->SpellIconID == 163)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Freezing Trap & Freezing Arrow & Wyvern Sting
            if (spellproto->SpellIconID == 180 || spellproto->SpellIconID == 1721)
                return DIMINISHING_DISORIENT;
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Curses/etc
            if (spellproto->IsFitToFamilyMask(uint64(0x00080000000)))
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Judgement of Justice - Limit to 10 seconds in PvP
            if (spellproto->IsFitToFamilyMask(uint64(0x00000100000)))
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Cyclone
            if (spellproto->IsFitToFamilyMask(uint64(0x02000000000)))
                return DIMINISHING_CYCLONE;
            // Pounce
            if (spellproto->IsFitToFamilyMask(uint64(0x00000020000)))
                return DIMINISHING_CHEAPSHOT_POUNCE;
            // Faerie Fire
            if (spellproto->IsFitToFamilyMask(uint64(0x00000000400)))
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Hamstring - limit duration to 10s in PvP
            if (spellproto->IsFitToFamilyMask(uint64(0x00000000002)))
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Shackle Undead
            if (spellproto->SpellIconID == 27)
                return DIMINISHING_DISORIENT;
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Hungering Cold (no flags)
            if (spellproto->SpellIconID == 2797)
                return DIMINISHING_DISORIENT;
            break;
        }
        case SPELLFAMILY_UNK1:
            return DIMINISHING_NONE;
        default:
            break;
    }

    // Get by mechanic
    uint32 mechanic = GetAllSpellMechanicMask(spellproto);
    if (!mechanic)
        return DIMINISHING_NONE;

    if (mechanic & ((1 << (MECHANIC_STUN - 1)) | (1 << (MECHANIC_SHACKLE - 1))))
        return triggered ? DIMINISHING_TRIGGER_STUN : DIMINISHING_CONTROL_STUN;
    if (mechanic & ((1 << (MECHANIC_SLEEP - 1)) | (1 << (MECHANIC_FREEZE - 1))))
        return DIMINISHING_FREEZE_SLEEP;
    if (mechanic & ((1 << (MECHANIC_KNOCKOUT - 1)) | (1 << (MECHANIC_POLYMORPH - 1)) | (1 << (MECHANIC_SAPPED - 1))))
        return DIMINISHING_DISORIENT;
    if (mechanic & (1 << (MECHANIC_ROOT - 1)))
        return triggered ? DIMINISHING_TRIGGER_ROOT : DIMINISHING_CONTROL_ROOT;
    if (mechanic & ((1 << (MECHANIC_FEAR - 1)) | (1 << (MECHANIC_CHARM - 1)) | (1 << (MECHANIC_TURN - 1))))
        return DIMINISHING_FEAR_CHARM_BLIND;
    if (mechanic & ((1 << (MECHANIC_SILENCE - 1)) | (1 << (MECHANIC_INTERRUPT - 1))))
        return DIMINISHING_SILENCE;
    if (mechanic & (1 << (MECHANIC_DISARM - 1)))
        return DIMINISHING_DISARM;
    if (mechanic & (1 << (MECHANIC_BANISH - 1)))
        return DIMINISHING_BANISH;
    if (mechanic & (1 << (MECHANIC_HORROR - 1)))
        return DIMINISHING_HORROR;

    return DIMINISHING_NONE;
}

int32 GetDiminishingReturnsLimitDuration(DiminishingGroup group, SpellEntry const* spellproto)
{
    if (!IsDiminishingReturnsGroupDurationLimited(group))
        return 0;

    // Explicit diminishing duration
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_PALADIN:
        {
            // Repentance - limit to 6 seconds in PvP
            if (spellproto->SpellFamilyFlags & uint64(0x00000000004))
                return 6000;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Faerie Fire - limit to 40 seconds in PvP (3.1)
            if (spellproto->SpellFamilyFlags & uint64(0x00000000400))
                return 40000;
            break;
        }
        default:
            break;
    }

    return 10000;
}

bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_CONTROL_STUN:
        case DIMINISHING_TRIGGER_STUN:
        case DIMINISHING_CONTROL_ROOT:
        case DIMINISHING_TRIGGER_ROOT:
        case DIMINISHING_FEAR_CHARM_BLIND:
        case DIMINISHING_DISORIENT:
        case DIMINISHING_CHEAPSHOT_POUNCE:
        case DIMINISHING_FREEZE_SLEEP:
        case DIMINISHING_CYCLONE:
        case DIMINISHING_BANISH:
        case DIMINISHING_LIMITONLY:
            return true;
        default:
            return false;
    }
}

bool IsDiminishingReturnsGroupDurationDiminished(DiminishingGroup group, bool pvp)
{
//     switch (group)
//     {
//         default:
//             return true;
//     }
    return false;
}

DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_CYCLONE:
        case DIMINISHING_TRIGGER_STUN:
        case DIMINISHING_CONTROL_STUN:
            return DRTYPE_ALL;
        case DIMINISHING_CONTROL_ROOT:
        case DIMINISHING_TRIGGER_ROOT:
        case DIMINISHING_FEAR_CHARM_BLIND:
        case DIMINISHING_DISORIENT:
        case DIMINISHING_SILENCE:
        case DIMINISHING_DISARM:
        case DIMINISHING_HORROR:
        case DIMINISHING_FREEZE_SLEEP:
        case DIMINISHING_BANISH:
        case DIMINISHING_CHEAPSHOT_POUNCE:
            return DRTYPE_PLAYER;
        default:
            break;
    }

    return DRTYPE_NONE;
}

bool IsSubjectToDiminishingLevels(DiminishingGroup group, bool pvp)
{
    if ((GetDiminishingReturnsGroupType(group) == DRTYPE_PLAYER && pvp) || GetDiminishingReturnsGroupType(group) == DRTYPE_ALL)
        return true;
    return false;
}

bool IsCreatureDRSpell(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        case 36924: // Harbinger Skyriss - Mind Rend
        case 31408: // Lesser Doomguard - War Stomp
        case 31480: return true; // Kazrogal War Stomp - confirmed via video
        default: return false;
    }
}

bool SpellArea::IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const
{
    if (conditionId)
    {
        if (!player || !sObjectMgr.IsConditionSatisfied(conditionId, player, player->GetMap(), nullptr, CONDITION_FROM_SPELL_AREA))
            return false;
    }
    else                                                    // This block will be removed
    {
        if (gender != GENDER_NONE)
        {
            // not in expected gender
            if (!player || gender != player->getGender())
                return false;
        }

        if (raceMask)
        {
            // not in expected race
            if (!player || !(raceMask & player->getRaceMask()))
                return false;
        }

        if (questStart)
        {
            // not in expected required quest state
            if (!player || ((!questStartCanActive || !player->IsActiveQuest(questStart)) && !player->GetQuestRewardStatus(questStart)))
                return false;
        }

        if (questEnd)
        {
            // not in expected forbidden quest state
            if (!player || player->GetQuestRewardStatus(questEnd))
                return false;
        }
    }

    if (areaId)
    {
        // not in expected zone
        if (newZone != areaId && newArea != areaId)
            return false;
    }

    if (auraSpell)
    {
        // not have expected aura
        if (!player)
            return false;
        if (auraSpell > 0)
            // have expected aura
            return player->HasAura(auraSpell);
            // not have expected aura
        return !player->HasAura(-auraSpell);
    }

    return true;
}

void SpellArea::ApplyOrRemoveSpellIfCan(Player* player, uint32 newZone, uint32 newArea, bool onlyApply) const
{
    MANGOS_ASSERT(player);

    if (IsFitToRequirements(player, newZone, newArea))
    {
        if (autocast && !player->HasAura(spellId))
            player->CastSpell(player, spellId, TRIGGERED_OLD_TRIGGERED);
    }
    else if (!onlyApply && player->HasAura(spellId))
        player->RemoveAurasDueToSpell(spellId);
}

SpellEntry const* GetSpellEntryByDifficulty(uint32 id, Difficulty difficulty, bool isRaid)
{
    SpellDifficultyEntry const* spellDiff = sSpellDifficultyStore.LookupEntry(id);

    if (!spellDiff)
        return nullptr;

    for (Difficulty diff = difficulty; diff >= REGULAR_DIFFICULTY; diff = GetPrevDifficulty(diff, isRaid))
    {
        if (spellDiff->spellId[diff])
            return sSpellTemplate.LookupEntry<SpellEntry>(spellDiff->spellId[diff]);
    }

    return nullptr;
}
