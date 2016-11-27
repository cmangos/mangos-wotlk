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

#ifndef MANGOS_DBCSFRM_H
#define MANGOS_DBCSFRM_H

const char Achievementfmt[] = "niixssssssssssssssssxxxxxxxxxxxxxxxxxxiixixxxxxxxxxxxxxxxxxxii";
const char AchievementCriteriafmt[] = "niiiiiiiissssssssssssssssxixiii";
const char AreaTableEntryfmt[] = "iiinixxxxxissssssssssssssssxiiiiixxx";
const char AreaGroupEntryfmt[] = "niiiiiii";
const char AreaTriggerEntryfmt[] = "niffffffff";
const char AuctionHouseEntryfmt[] = "niiixxxxxxxxxxxxxxxxx";
const char BankBagSlotPricesEntryfmt[] = "ni";
const char BarberShopStyleEntryfmt[] = "nixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxiii";
const char BattlemasterListEntryfmt[] = "niiiiiiiiixssssssssssssssssxiiii";
const char CharStartOutfitEntryfmt[] = "diiiiiiiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char CharTitlesEntryfmt[] = "nxssssssssssssssssxxxxxxxxxxxxxxxxxxi";
const char ChatChannelsEntryfmt[] = "iixssssssssssssssssxxxxxxxxxxxxxxxxxx";
// ChatChannelsEntryfmt, index not used (more compact store)
const char ChrClassesEntryfmt[] = "nxixssssssssssssssssxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxixii";
const char ChrRacesEntryfmt[] = "nxixiixixxxxixssssssssssssssssxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxi";
const char CinematicSequencesEntryfmt[] = "nxxxxxxxxx";
const char CreatureDisplayInfofmt[] = "nixifxxxxxxxxxxx";
const char CreatureDisplayInfoExtrafmt[] = "nixxxxxxxxxxxxxxxxxxx";
const char CreatureModelDatafmt[] = "nixxfxxxxxxxxxxffxxxxxxxxxxx";
const char CreatureFamilyfmt[] = "nfifiiiiixssssssssssssssssxx";
const char CreatureSpellDatafmt[] = "niiiixxxx";
const char CreatureTypefmt[] = "nxxxxxxxxxxxxxxxxxx";
const char CurrencyTypesfmt[] = "xnxi";
const char DestructibleModelDataFmt[] = "nxxixxxixxxixxxxxxx";
const char DungeonEncounterfmt[] = "niiiissssssssssssssssxx";
const char DurabilityCostsfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char DurabilityQualityfmt[] = "nf";
const char EmotesEntryfmt[] = "nxxiiix";
const char EmotesTextEntryfmt[] = "nxixxxxxxxxxxxxxxxx";
const char FactionEntryfmt[] = "niiiiiiiiiiiiiiiiiiffixssssssssssssssssxxxxxxxxxxxxxxxxxx";
const char FactionTemplateEntryfmt[] = "niiiiiiiiiiiii";
const char GameObjectDisplayInfofmt[] = "nsxxxxxxxxxxffffffx";
const char GemPropertiesEntryfmt[] = "nixxi";
const char GlyphPropertiesfmt[] = "niii";
const char GlyphSlotfmt[] = "nii";
const char GtBarberShopCostBasefmt[] = "f";
const char GtCombatRatingsfmt[] = "f";
const char GtChanceToMeleeCritBasefmt[] = "f";
const char GtChanceToMeleeCritfmt[] = "f";
const char GtChanceToSpellCritBasefmt[] = "f";
const char GtChanceToSpellCritfmt[] = "f";
const char GtOCTClassCombatRatingScalarfmt[] = "df";
const char GtOCTRegenHPfmt[] = "f";
const char GtNPCManaCostScalerfmt[] = "f";
// const char GtOCTRegenMPfmt[]="f";
const char GtRegenHPPerSptfmt[] = "f";
const char GtRegenMPPerSptfmt[] = "f";
const char Holidaysfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char Itemfmt[] = "niiiiiii";
const char ItemBagFamilyfmt[] = "nxxxxxxxxxxxxxxxxx";
const char ItemClassfmt[] = "nxxssssssssssssssssx";
// const char ItemDisplayTemplateEntryfmt[]="nxxxxxxxxxxixxxxxxxxxxx";
// const char ItemCondExtCostsEntryfmt[]="xiii";
const char ItemExtendedCostEntryfmt[] = "niiiiiiiiiiiiiix";
const char ItemLimitCategoryEntryfmt[] = "nxxxxxxxxxxxxxxxxxii";
const char ItemRandomPropertiesfmt[] = "nxiiiiissssssssssssssssx";
const char ItemRandomSuffixfmt[] = "nssssssssssssssssxxiiiiiiiiii";
const char ItemSetEntryfmt[] = "dssssssssssssssssxxxxxxxxxxxxxxxxxxiiiiiiiiiiiiiiiiii";
const char LiquidTypefmt[] = "nxxixixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char LockEntryfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiixxxxxxxx";
const char MailTemplateEntryfmt[] = "nxxxxxxxxxxxxxxxxxssssssssssssssssx";
const char MapEntryfmt[] = "nxixxssssssssssssssssxixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxixiffxixx";
const char MapDifficultyEntryfmt[] = "diixxxxxxxxxxxxxxxxxiix";
const char MovieEntryfmt[] = "nxx";
const char OverrideSpellDatafmt[] = "niiiiiiiiiix";
const char QuestFactionRewardfmt[] = "niiiiiiiiii";
const char QuestSortEntryfmt[] = "nxxxxxxxxxxxxxxxxx";
const char QuestXPLevelfmt[] = "niiiiiiiiii";
const char PowerDisplayfmt[] = "nixxxx";
const char PvPDifficultyfmt[] = "diiiii";
const char RandomPropertiesPointsfmt[] = "niiiiiiiiiiiiiii";
const char ScalingStatDistributionfmt[] = "niiiiiiiiiiiiiiiiiiiii";
const char ScalingStatValuesfmt[] = "iniiiiiiiiiiiiiiiiixiiii";
const char SkillLinefmt[] = "nixssssssssssssssssxxxxxxxxxxxxxxxxxxixxxxxxxxxxxxxxxxxi";
const char SkillLineAbilityfmt[] = "niiiixxiiiiixx";
const char SkillRaceClassInfofmt[] = "diiiiixx";
const char SoundEntriesfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char SpellCastTimefmt[] = "nixx";
const char SpellDurationfmt[] = "niii";
const char SpellDifficultyfmt[] = "niiii";
const char SpellFocusObjectfmt[] = "nxxxxxxxxxxxxxxxxx";
const char SpellItemEnchantmentfmt[] = "nxiiiiiixxxiiissssssssssssssssxiiiixxx";
const char SpellItemEnchantmentConditionfmt[] = "nbbbbbxxxxxbbbbbbbbbbiiiiiXXXXX";
const char SpellRadiusfmt[] = "nfxx";
const char SpellRangefmt[] = "nffffxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char SpellRuneCostfmt[] = "niiii";
const char SpellShapeshiftFormfmt[] = "nxxxxxxxxxxxxxxxxxxiixiiixxiiiiiiii";
const char StableSlotPricesfmt[] = "ni";
const char SummonPropertiesfmt[] = "niiiii";
const char TalentEntryfmt[] = "niiiiiiiixxxxixxixxxxxx";
const char TalentTabEntryfmt[] = "nxxxxxxxxxxxxxxxxxxxiiix";
const char TaxiNodesEntryfmt[] = "nifffssssssssssssssssxii";
const char TaxiPathEntryfmt[] = "niii";
const char TaxiPathNodeEntryfmt[] = "diiifffiiii";
const char TeamContributionPointsfmt[] = "df";
const char TotemCategoryEntryfmt[] = "nxxxxxxxxxxxxxxxxxii";
const char VehicleEntryfmt[] = "niffffiiiiiiiifffffffffffffffssssfifiixx";
const char VehicleSeatEntryfmt[] = "niiffffffffffiiiiiifffffffiiifffiiiiiiiffiiiiixxxxxxxxxxxx";
const char WMOAreaTableEntryfmt[] = "niiixxxxxiixxxxxxxxxxxxxxxxx";
const char WorldMapAreaEntryfmt[] = "xinxffffixx";
const char WorldMapOverlayEntryfmt[] = "nxiiiixxxxxxxxxxx";
const char WorldSafeLocsEntryfmt[] = "nifffxxxxxxxxxxxxxxxxx";

#endif
