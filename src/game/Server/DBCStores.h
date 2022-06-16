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

#ifndef MANGOS_DBCSTORES_H
#define MANGOS_DBCSTORES_H

#include "Common.h"
#include "Database/DBCStore.h"
#include "Server/DBCStructure.h"

#include <list>
#include <vector>

bool IsAcceptableClientBuild(uint32 build);
std::string AcceptableClientBuildsListStr();

typedef std::list<uint32> SimpleFactionsList;

SimpleFactionsList const* GetFactionTeamList(uint32 faction);
char const* GetPetName(uint32 petfamily, uint32 dbclang);
uint32 GetTalentSpellCost(uint32 spellId);
uint32 GetTalentSpellCost(TalentSpellPos const* pos);
TalentSpellPos const* GetTalentSpellPos(uint32 spellId);

int32 GetAreaFlagByAreaID(uint32 area_id);                  // -1 if not found
uint32 GetAreaFlagByMapId(uint32 mapid);

std::vector<WMOAreaTableEntry const*>& GetWMOAreaTableEntriesByTripple(int32 rootid, int32 adtid, int32 groupid);

AreaTableEntry const* GetAreaEntryByAreaID(uint32 area_id);
AreaTableEntry const* GetAreaEntryByAreaFlagAndMap(uint32 area_flag, uint32 map_id);

uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId);

enum ContentLevels
{
    CONTENT_1_60 = 0,
    CONTENT_61_70,
    CONTENT_71_80
};
ContentLevels GetContentLevelsForMapAndZone(uint32 mapid, uint32 zoneId);

ChatChannelsEntry const* GetChatChannelsEntryFor(const std::string& name, uint32 channel_id = 0);

bool IsTotemCategoryCompatiableWith(uint32 itemTotemCategoryId, uint32 requiredTotemCategoryId);

CharacterFacialHairStylesEntry const* GetCharFacialHairEntry(uint8 race, uint8 gender, uint8 facialHairId);
CharSectionsEntry const* GetCharSectionEntry(uint8 race, CharSectionType genType, uint8 gender, uint8 type, uint8 color);

bool MapCoordinateVsZoneCheck(float x, float y, uint32 mapid, uint32 zone);
bool Zone2MapCoordinates(float& x, float& y, uint32 zone);
bool Map2ZoneCoordinates(float& x, float& y, uint32 zone);

typedef std::map<uint32/*pair32(map,diff)*/, MapDifficultyEntry const*> MapDifficultyMap;
MapDifficultyEntry const* GetMapDifficultyData(uint32 mapId, Difficulty difficulty);

// natural order for difficulties up-down iteration
// difficulties for dungeons/battleground ordered in normal way
// and if more high version not exist must be used lesser version
// for raid order different:
// 10 man normal version must be used instead nonexistent 10 man heroic version
// 25 man normal version must be used instead nonexistent 25 man heroic version
inline Difficulty GetPrevDifficulty(Difficulty diff, bool isRaid)
{
    switch (diff)
    {
        default:
        case RAID_DIFFICULTY_10MAN_NORMAL:                  // == DUNGEON_DIFFICULTY_NORMAL == REGULAR_DIFFICULTY
            return REGULAR_DIFFICULTY;                      // return itself, caller code must properly check and not call for this case
        case RAID_DIFFICULTY_25MAN_NORMAL:                  // == DUNGEON_DIFFICULTY_HEROIC
            return RAID_DIFFICULTY_10MAN_NORMAL;
        case RAID_DIFFICULTY_10MAN_HEROIC:
            return isRaid ? RAID_DIFFICULTY_10MAN_NORMAL : DUNGEON_DIFFICULTY_HEROIC;
        case RAID_DIFFICULTY_25MAN_HEROIC:
            return isRaid ? RAID_DIFFICULTY_25MAN_NORMAL : RAID_DIFFICULTY_10MAN_HEROIC;
    }
}

uint32 const* /*[3]*/ GetTalentTabPages(uint32 cls);

bool IsPointInAreaTriggerZone(AreaTriggerEntry const* atEntry, uint32 mapid, float x, float y, float z, float delta = 0.0f);

PvPDifficultyEntry const* GetBattlegroundBracketByLevel(uint32 mapid, uint32 level);
PvPDifficultyEntry const* GetBattlegroundBracketById(uint32 mapid, BattleGroundBracketId id);

uint32 GetCreatureModelRace(uint32 modelId);
float GetModelMidpoint(uint32 modelId);

uint32 GetDefaultMapLight(uint32 mapId);

extern DBCStorage <AchievementEntry>             sAchievementStore;
extern DBCStorage <AchievementCriteriaEntry>     sAchievementCriteriaStore;
extern DBCStorage <AreaTableEntry>               sAreaStore;// recommend access using functions
extern DBCStorage <AreaTriggerEntry>             sAreaTriggerStore;
extern DBCStorage <AuctionHouseEntry>            sAuctionHouseStore;
extern DBCStorage <BankBagSlotPricesEntry>       sBankBagSlotPricesStore;
extern DBCStorage <BarberShopStyleEntry>         sBarberShopStyleStore;
extern DBCStorage <BattlemasterListEntry>        sBattlemasterListStore;
// extern DBCStorage <ChatChannelsEntry>           sChatChannelsStore; -- accessed using function, no usable index
extern DBCStorage <CharStartOutfitEntry>         sCharStartOutfitStore;
extern DBCStorage <CharTitlesEntry>              sCharTitlesStore;
extern DBCStorage <ChatChannelsEntry>            sChatChannelsStore;
extern DBCStorage <CharacterFacialHairStylesEntry>  sCharacterFacialHairStylesStore;
extern DBCStorage <CharSectionsEntry>            sCharSectionsStore;
extern DBCStorage <ChrClassesEntry>              sChrClassesStore;
extern DBCStorage <ChrRacesEntry>                sChrRacesStore;
extern DBCStorage <CinematicCameraEntry>         sCinematicCameraStore;
extern DBCStorage <CinematicSequencesEntry>      sCinematicSequencesStore;
extern DBCStorage <CreatureDisplayInfoEntry>     sCreatureDisplayInfoStore;
extern DBCStorage <CreatureDisplayInfoExtraEntry>sCreatureDisplayInfoExtraStore;
extern DBCStorage <CreatureModelDataEntry>       sCreatureModelDataStore;
extern DBCStorage <CreatureFamilyEntry>          sCreatureFamilyStore;
extern DBCStorage <CreatureSpellDataEntry>       sCreatureSpellDataStore;
extern DBCStorage <CreatureTypeEntry>            sCreatureTypeStore;
extern DBCStorage <CurrencyTypesEntry>           sCurrencyTypesStore;
extern DBCStorage <DestructibleModelDataEntry>   sDestructibleModelDataStore;
extern DBCStorage <DurabilityCostsEntry>         sDurabilityCostsStore;
extern DBCStorage <DurabilityQualityEntry>       sDurabilityQualityStore;
extern DBCStorage <EmotesEntry>                  sEmotesStore;
extern DBCStorage <EmotesTextEntry>              sEmotesTextStore;
extern DBCStorage <FactionEntry>                 sFactionStore;
extern DBCStorage <FactionTemplateEntry>         sFactionTemplateStore;
extern DBCStorage <GameObjectArtKitEntry>   sGameObjectArtKitStore;
extern DBCStorage <GameObjectDisplayInfoEntry>   sGameObjectDisplayInfoStore;
extern DBCStorage <GemPropertiesEntry>           sGemPropertiesStore;
extern DBCStorage <GlyphPropertiesEntry>         sGlyphPropertiesStore;
extern DBCStorage <GlyphSlotEntry>               sGlyphSlotStore;

extern DBCStorage <GMSurveyAnswersEntry>         sGMSurveyAnswersStore;
extern DBCStorage <GMSurveyCurrentSurveyEntry>   sGMSurveyCurrentSurveyStore;
extern DBCStorage <GMSurveyQuestionsEntry>       sGMSurveyQuestionsStore;
extern DBCStorage <GMSurveyEntry>                sGMSurveySurveysStore;
extern DBCStorage <GMTicketCategoryEntry>        sGMTicketCategoryStore;

extern DBCStorage <GtBarberShopCostBaseEntry>    sGtBarberShopCostBaseStore;

extern DBCStorage <GtCombatRatingsEntry>         sGtCombatRatingsStore;
extern DBCStorage <GtChanceToMeleeCritBaseEntry> sGtChanceToMeleeCritBaseStore;
extern DBCStorage <GtChanceToMeleeCritEntry>     sGtChanceToMeleeCritStore;
extern DBCStorage <GtChanceToSpellCritBaseEntry> sGtChanceToSpellCritBaseStore;
extern DBCStorage <GtChanceToSpellCritEntry>     sGtChanceToSpellCritStore;
extern DBCStorage <GtOCTClassCombatRatingScalarEntry> sGtOCTClassCombatRatingScalarStore;
extern DBCStorage <GtOCTRegenHPEntry>            sGtOCTRegenHPStore;
extern DBCStorage <GtNPCManaCostScalerEntry>     sGtNPCManaCostScalerStore;
// extern DBCStorage <GtOCTRegenMPEntry>            sGtOCTRegenMPStore; -- not used currently
extern DBCStorage <GtRegenHPPerSptEntry>         sGtRegenHPPerSptStore;
extern DBCStorage <GtRegenMPPerSptEntry>         sGtRegenMPPerSptStore;
extern DBCStorage <HolidaysEntry>                sHolidaysStore;
extern DBCStorage <ItemEntry>                    sItemStore;
extern DBCStorage <ItemBagFamilyEntry>           sItemBagFamilyStore;
extern DBCStorage <ItemClassEntry>               sItemClassStore;
// extern DBCStorage <ItemDisplayInfoEntry>      sItemDisplayInfoStore; -- not used currently
extern DBCStorage <ItemExtendedCostEntry>        sItemExtendedCostStore;
extern DBCStorage <ItemLimitCategoryEntry>       sItemLimitCategoryStore;
extern DBCStorage <ItemRandomPropertiesEntry>    sItemRandomPropertiesStore;
extern DBCStorage <ItemRandomSuffixEntry>        sItemRandomSuffixStore;
extern DBCStorage <ItemSetEntry>                 sItemSetStore;
extern DBCStorage <LFGDungeonEntry>              sLFGDungeonStore;
extern DBCStorage <LfgDungeonExpansionEntry>     sLFGDungeonExpansionStore;
extern DBCStorage <LiquidTypeEntry>              sLiquidTypeStore;
extern DBCStorage <LockEntry>                    sLockStore;
extern DBCStorage <MailTemplateEntry>            sMailTemplateStore;
extern DBCStorage <MapEntry>                     sMapStore;
extern DBCStorage <MapDifficultyEntry>           sMapDifficultyStore; // only used for text lookup
extern MapDifficultyMap                          sMapDifficultyMap;
extern DBCStorage <MovieEntry>                   sMovieStore;
extern DBCStorage <OverrideSpellDataEntry>       sOverrideSpellDataStore;
extern DBCStorage <QuestFactionRewardEntry>      sQuestFactionRewardStore;
extern DBCStorage <QuestSortEntry>               sQuestSortStore;
extern DBCStorage <QuestXPLevel>                 sQuestXPLevelStore;
extern DBCStorage <PowerDisplayEntry>            sPowerDisplayStore;
// extern DBCStorage <PvPDifficultyEntry>           sPvPDifficultyStore; -- use GetBattlegroundSlotByLevel for access
extern DBCStorage <RandomPropertiesPointsEntry>  sRandomPropertiesPointsStore;
extern DBCStorage <ScalingStatDistributionEntry> sScalingStatDistributionStore;
extern DBCStorage <ScalingStatValuesEntry>       sScalingStatValuesStore;
extern DBCStorage <SkillLineEntry>               sSkillLineStore;
extern DBCStorage <SkillLineAbilityEntry>        sSkillLineAbilityStore;
extern DBCStorage <SkillRaceClassInfoEntry>      sSkillRaceClassInfoStore;
extern DBCStorage <SkillTiersEntry>              sSkillTiersStore;
extern DBCStorage <SoundEntriesEntry>            sSoundEntriesStore;
extern DBCStorage <SpellCastTimesEntry>          sSpellCastTimesStore;
extern DBCStorage <SpellCategoryEntry>           sSpellCategoryStore;
extern DBCStorage <SpellDifficultyEntry>         sSpellDifficultyStore;
extern DBCStorage <SpellDurationEntry>           sSpellDurationStore;
extern DBCStorage <SpellFocusObjectEntry>        sSpellFocusObjectStore;
extern DBCStorage <SpellItemEnchantmentEntry>    sSpellItemEnchantmentStore;
extern DBCStorage <SpellItemEnchantmentConditionEntry> sSpellItemEnchantmentConditionStore;
extern ItemSpellCategoryStore                    sItemSpellCategoryStore;
extern PetFamilySpellsStore                      sPetFamilySpellsStore;
extern DBCStorage <SpellRadiusEntry>             sSpellRadiusStore;
extern DBCStorage <SpellRangeEntry>              sSpellRangeStore;
extern DBCStorage <SpellRuneCostEntry>           sSpellRuneCostStore;
extern DBCStorage <SpellShapeshiftFormEntry>     sSpellShapeshiftFormStore;
extern DBCStorage <SpellVisualEntry>             sSpellVisualStore;
extern DBCStorage <StableSlotPricesEntry>        sStableSlotPricesStore;
extern DBCStorage <SummonPropertiesEntry>        sSummonPropertiesStore;
extern DBCStorage <TalentEntry>                  sTalentStore;
extern DBCStorage <TalentTabEntry>               sTalentTabStore;
extern DBCStorage <TaxiNodesEntry>               sTaxiNodesStore;
extern DBCStorage <TaxiPathEntry>                sTaxiPathStore;
extern TaxiMask                                  sTaxiNodesMask;
extern TaxiMask                                  sOldContinentsNodesMask;
extern TaxiPathSetBySource                       sTaxiPathSetBySource;
extern TaxiPathNodesByPath                       sTaxiPathNodesByPath;
extern DBCStorage <TeamContributionPoints>       sTeamContributionPoints;
extern DBCStorage <TotemCategoryEntry>           sTotemCategoryStore;
extern DBCStorage <TransportAnimationEntry>      sTransportAnimationStore;
extern DBCStorage <TransportRotationEntry>       sTransportRotationStore;
extern DBCStorage <VehicleEntry>                 sVehicleStore;
extern DBCStorage <VehicleSeatEntry>             sVehicleSeatStore;
extern DBCStorage <WMOAreaTableEntry>            sWMOAreaTableStore;
// extern DBCStorage <WorldMapAreaEntry>           sWorldMapAreaStore; -- use Zone2MapCoordinates and Map2ZoneCoordinates
extern DBCStorage <WorldMapOverlayEntry>         sWorldMapOverlayStore;

void LoadDBCStores(const std::string& dataPath);

// script support functions
DBCStorage <SoundEntriesEntry>          const* GetSoundEntriesStore();
DBCStorage <SpellRangeEntry>            const* GetSpellRangeStore();
DBCStorage <FactionEntry>               const* GetFactionStore();
DBCStorage <ItemEntry>                  const* GetItemDisplayStore();
DBCStorage <CreatureDisplayInfoEntry>   const* GetCreatureDisplayStore();
DBCStorage <EmotesEntry>                const* GetEmotesStore();
DBCStorage <EmotesTextEntry>            const* GetEmotesTextStore();

LFGDungeonEntry const* GetLFGDungeon(uint32 mapId, Difficulty difficulty);

#endif
