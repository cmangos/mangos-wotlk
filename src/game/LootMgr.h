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

#ifndef MANGOS_LOOTMGR_H
#define MANGOS_LOOTMGR_H

#include "ItemEnchantmentMgr.h"
#include "ByteBuffer.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"

#include "Utilities/LinkedReference/RefManager.h"

#include <map>
#include <vector>
#include "Bag.h"

#define LOOT_ROLL_TIMEOUT  (1*MINUTE*IN_MILLISECONDS)

class Player;
class Group;
class LootStore;
class WorldObject;
class LootTemplate;
class Loot;
struct LootView;
struct LootItem;
struct ItemPrototype;


#define MAX_NR_LOOT_ITEMS 16
// note: the client cannot show more than 16 items total
#define MAX_NR_QUEST_ITEMS 32
// unrelated to the number of quest items shown, just for reserve

enum PermissionTypes
{
    ALL_PERMISSION    = 0,
    GROUP_PERMISSION  = 1,
    MASTER_PERMISSION = 2,
    OWNER_PERMISSION  = 3,                                  // for single player only loots
    NONE_PERMISSION   = 4
};

enum LootType
{
    LOOT_NONE                   = 0,                        // for internal use only
    LOOT_CORPSE                 = 1,
    LOOT_PICKPOCKETING          = 2,
    LOOT_FISHING                = 3,
    LOOT_DISENCHANTING          = 4,
    // ignored always by client
    LOOT_SKINNING               = 6,
    LOOT_PROSPECTING            = 7,
    LOOT_MILLING                = 8,

    LOOT_FISHINGHOLE            = 20,                       // unsupported by client, sending LOOT_FISHING instead
    LOOT_FISHING_FAIL           = 21,                       // unsupported by client, sending LOOT_FISHING instead
    LOOT_INSIGNIA               = 22                        // unsupported by client, sending LOOT_CORPSE instead
};

enum LootSlotType
{
    LOOT_SLOT_NORMAL  = 0,                                  // can be looted
    LOOT_SLOT_VIEW    = 1,                                  // can be only view (ignore any loot attempts)
    LOOT_SLOT_MASTER  = 2,                                  // can be looted only master (error message)
    LOOT_SLOT_REQS    = 3,                                  // can't be looted (error message about missing reqs)
    LOOT_SLOT_OWNER   = 4,                                  // ignore binding confirmation and etc, for single player looting
    MAX_LOOT_SLOT_TYPE                                      // custom, use for mark skipped from show items
};

enum LootMethod
{
    FREE_FOR_ALL = 0,
    ROUND_ROBIN = 1,
    MASTER_LOOT = 2,
    GROUP_LOOT = 3,
    NEED_BEFORE_GREED = 4,

    NOT_GROUP_TYPE_LOOT = 5                                   // internal use only
};

enum RollVote
{
    ROLL_PASS = 0,
    ROLL_NEED = 1,
    ROLL_GREED = 2,
    ROLL_DISENCHANT = 3,
    ROLL_NOT_EMITED_YET = 4,                             // send to client
    ROLL_NOT_VALID = 5                                   // not send to client
};

// set what votes allowed
enum RollVoteMask
{
    ROLL_VOTE_MASK_PASS = 0x01,
    ROLL_VOTE_MASK_NEED = 0x02,
    ROLL_VOTE_MASK_GREED = 0x04,
    ROLL_VOTE_MASK_DISENCHANT = 0x08,

    ROLL_VOTE_MASK_ALL = 0x0F,
};

enum CreatureLootStatus
{
    CREATURE_LOOT_STATUS_NONE           = 0,
    CREATURE_LOOT_STATUS_PICKPOCKETED   = 1,
    CREATURE_LOOT_STATUS_LOOTED         = 2,
    CREATURE_LOOT_STATUS_SKINNED        = 3
};

struct PlayerRollVote
{
    PlayerRollVote() : vote(ROLL_NOT_VALID), number(0) {}
    RollVote vote;
    uint8    number;
};

class GroupLootRoll
{
public:
    typedef UNORDERED_MAP<ObjectGuid, PlayerRollVote> RollVoteMap;

    GroupLootRoll() : m_rollVoteMap(ROLL_VOTE_MASK_ALL), m_isStarted(false), m_lootItem(NULL), m_loot(NULL), m_itemProto(NULL) {}
    ~GroupLootRoll();

    void Start(Loot& loot, uint32 itemSlot);
    bool PlayerVote(Player* playerGuid, RollVote vote);
    bool UpdateRoll();

private:
    void SendStartRoll();
    void SendAllPassed();
    void SendRoll(ObjectGuid const& targetGuid, uint32 rollNumber, uint32 rollType);
    void SendLootRollWon(ObjectGuid const& targetGuid, uint32 rollNumber, RollVote rollType);
    void Finish(RollVoteMap::const_iterator& winnerItr);
    bool AllPlayerVoted(RollVoteMap::const_iterator& winnerItr);
    RollVoteMap           m_rollVoteMap;
    bool                  m_isStarted;
    LootItem*             m_lootItem;
    Loot*                 m_loot;
    uint32                m_itemSlot;
    RollVoteMask          m_voteMask;
    ItemPrototype const*  m_itemProto;
    uint32                m_voteCount;
    time_t                m_endTime;
};
typedef UNORDERED_MAP<uint32, GroupLootRoll> GroupLootRollMap;

struct LootStoreItem
{
    uint32  itemid;                                         // id of the item
    float   chance;                                         // always positive, chance to drop for both quest and non-quest items, chance to be used for refs
    int32   mincountOrRef;                                  // mincount for drop items (positive) or minus referenced TemplateleId (negative)
    uint8   group       : 7;
    bool    needs_quest : 1;                                // quest drop (negative ChanceOrQuestChance in DB)
    uint8   maxcount    : 8;                                // max drop count for the item (mincountOrRef positive) or Ref multiplicator (mincountOrRef negative)
    uint16  conditionId : 16;                               // additional loot condition Id

    // Constructor, converting ChanceOrQuestChance -> (chance, needs_quest)
    // displayid is filled in IsValid() which must be called after
    LootStoreItem(uint32 _itemid, float _chanceOrQuestChance, int8 _group, uint16 _conditionId, int32 _mincountOrRef, uint8 _maxcount)
        : itemid(_itemid), chance(fabs(_chanceOrQuestChance)), mincountOrRef(_mincountOrRef),
          group(_group), needs_quest(_chanceOrQuestChance < 0), maxcount(_maxcount), conditionId(_conditionId)
    {}

    bool Roll(bool rate) const;                             // Checks if the entry takes it's chance (at loot generation)
    bool IsValid(LootStore const& store, uint32 entry) const;
    // Checks correctness of values
};

struct LootItem
{
    uint32  itemid;
    uint32  randomSuffix;
    int32   randomPropertyId;
    uint16  conditionId       : 16;                         // allow compiler pack structure
    uint8   count             : 8;
    bool    is_looted         : 1;
    bool    is_blocked        : 1;
    bool    freeforall        : 1;                          // free for all
    bool    is_underthreshold : 1;
    bool    is_counted        : 1;
    bool    needs_quest       : 1;                          // quest drop

    // Constructor, copies most fields from LootStoreItem, generates random count and random suffixes/properties
    // Should be called for non-reference LootStoreItem entries only (mincountOrRef > 0)
    explicit LootItem(LootStoreItem const& li);

    LootItem(uint32 itemid_, uint32 count_, uint32 randomSuffix_ = 0, int32 randomPropertyId_ = 0);

    // Basic checks for player/item compatibility - if false no chance to see the item in the loot
    bool AllowedForPlayer(Player const* player, WorldObject const* lootTarget) const;
    LootSlotType GetSlotTypeForSharedLoot(LootView const& lv, bool condition_ok = false) const;
};

typedef std::vector<LootItem> LootItemList;

struct QuestItem
{
    uint8   index;                                          // position in quest_items;
    bool    is_looted;

    QuestItem()
        : index(0), is_looted(false) {}

    QuestItem(uint8 _index, bool _islooted = false)
        : index(_index), is_looted(_islooted) {}
};


typedef std::vector<QuestItem> QuestItemList;
typedef std::map<uint32, QuestItemList*> QuestItemMap;
typedef std::vector<LootStoreItem> LootStoreItemList;
typedef UNORDERED_MAP<uint32, LootTemplate*> LootTemplateMap;

typedef std::set<uint32> LootIdSet;

class LootStore
{
    public:
        explicit LootStore(char const* name, char const* entryName, bool ratesAllowed)
            : m_name(name), m_entryName(entryName), m_ratesAllowed(ratesAllowed) {}
        virtual ~LootStore() { Clear(); }

        void Verify() const;

        void LoadAndCollectLootIds(LootIdSet& ids_set);
        void CheckLootRefs(LootIdSet* ref_set = nullptr) const;// check existence reference and remove it from ref_set
        void ReportUnusedIds(LootIdSet const& ids_set) const;
        void ReportNotExistedId(uint32 id) const;

        bool HaveLootFor(uint32 loot_id) const { return m_LootTemplates.find(loot_id) != m_LootTemplates.end(); }
        bool HaveQuestLootFor(uint32 loot_id) const;
        bool HaveQuestLootForPlayer(uint32 loot_id, Player* player) const;

        LootTemplate const* GetLootFor(uint32 loot_id) const;

        char const* GetName() const { return m_name; }
        char const* GetEntryName() const { return m_entryName; }
        bool IsRatesAllowed() const { return m_ratesAllowed; }
    protected:
        void LoadLootTable();
        void Clear();
    private:
        LootTemplateMap m_LootTemplates;
        char const* m_name;
        char const* m_entryName;
        bool m_ratesAllowed;
};

class LootTemplate
{
        class  LootGroup;                                   // A set of loot definitions for items (refs are not allowed inside)
        typedef std::vector<LootGroup> LootGroups;

    public:
        // Adds an entry to the group (at loading stage)
        void AddEntry(LootStoreItem& item);
        // Rolls for every item in the template and adds the rolled items the the loot
        void Process(Loot& loot, LootStore const& store, bool rate, uint8 GroupId = 0) const;

        // True if template includes at least 1 quest drop entry
        bool HasQuestDrop(LootTemplateMap const& store, uint8 GroupId = 0) const;
        // True if template includes at least 1 quest drop for an active quest of the player
        bool HasQuestDropForPlayer(LootTemplateMap const& store, Player const* player, uint8 GroupId = 0) const;

        // Checks integrity of the template
        void Verify(LootStore const& store, uint32 Id) const;
        void CheckLootRefs(LootIdSet* ref_set) const;
    private:
        LootStoreItemList Entries;                          // not grouped only
        LootGroups        Groups;                           // groups have own (optimised) processing, grouped entries go there
};

//=====================================================

class LootValidatorRef :  public Reference<Loot, LootValidatorRef>
{
    public:
        LootValidatorRef() {}
        void targetObjectDestroyLink() override {}
        void sourceObjectDestroyLink() override {}
};

//=====================================================

class LootValidatorRefManager : public RefManager<Loot, LootValidatorRef>
{
    public:
        typedef LinkedListHead::Iterator< LootValidatorRef > iterator;

        LootValidatorRef* getFirst() { return (LootValidatorRef*)RefManager<Loot, LootValidatorRef>::getFirst(); }
        LootValidatorRef* getLast() { return (LootValidatorRef*)RefManager<Loot, LootValidatorRef>::getLast(); }

        iterator begin() { return iterator(getFirst()); }
        iterator end() { return iterator(nullptr); }
        iterator rbegin() { return iterator(getLast()); }
        iterator rend() { return iterator(nullptr); }
};

//=====================================================

ByteBuffer& operator<<(ByteBuffer& b, LootItem const& li);
ByteBuffer& operator<<(ByteBuffer& b, LootView const& lv);

class Loot
{
public:
    friend ByteBuffer& operator<<(ByteBuffer& b, LootView const& lv);

    QuestItemMap const& GetPlayerQuestItems() const { return m_playerQuestItems; }
    QuestItemMap const& GetPlayerFFAItems() const { return m_playerFFAItems; }
    QuestItemMap const& GetPlayerNonQuestNonFFAConditionalItems() const { return m_playerNonQuestNonFFAConditionalItems; }

    LootItemList     items;
    uint32           gold;
    uint8            unlootedCount;
    LootType         lootType;                      // required for achievement system
    LootMethod       lootMethod;                    // used to know what kind of check must be done at loot time
    ItemQualities    threshold;                     // group threshold for items
    ObjectGuid       masterOwnerGuid;               // master loot player or round robin owner
    ObjectGuid       currentLooterGuid;             // current player for under threshold items (Round Robin)
    GuidSet          ownerSet;                      // set of all player who have right to the loot
    GroupLootRollMap roll;                          // used if an item is under rolling
    uint32           maxEnchantSkill;               // used to know group right to use disenchant option
    bool             isReleased;                    // used to release loot for round robin item
    bool             haveItemOverThreshold;         // if at least one item in the loot is over threshold
    bool             isChecked;                     // true if at least one player received the loot content

    Loot() : gold(0), unlootedCount(0), lootType(LOOT_NONE), m_lootTarget(NULL),
        lootMethod(NOT_GROUP_TYPE_LOOT), maxEnchantSkill(0), isReleased(false), threshold(ITEM_QUALITY_UNCOMMON),
        haveItemOverThreshold(false), isChecked(false)
    {
    }

    Loot(Player* player, Creature* creature, LootType type);
    Loot(Player* player, GameObject* gameObject, LootType type);
    Loot(Player* player, Corpse* corpse, LootType type);
    Loot(Player* player, Item* item, LootType type);
    Loot(Unit* unit, Item* item);

    ~Loot() { clear(); }

    // if loot becomes invalid this reference is used to inform the listener
    void addLootValidatorRef(LootValidatorRef* pLootValidatorRef)
    {
        m_LootValidatorRefManager.insertFirst(pLootValidatorRef);
    }

    // void clear();
    void clear()
    {
        for (QuestItemMap::const_iterator itr = m_playerQuestItems.begin(); itr != m_playerQuestItems.end(); ++itr)
            delete itr->second;
        m_playerQuestItems.clear();

        for (QuestItemMap::const_iterator itr = m_playerFFAItems.begin(); itr != m_playerFFAItems.end(); ++itr)
            delete itr->second;
        m_playerFFAItems.clear();

        for (QuestItemMap::const_iterator itr = m_playerNonQuestNonFFAConditionalItems.begin(); itr != m_playerNonQuestNonFFAConditionalItems.end(); ++itr)
            delete itr->second;
        m_playerNonQuestNonFFAConditionalItems.clear();

        m_playersLooting.clear();
        items.clear();
        m_questItems.clear();
        gold = 0;
        unlootedCount = 0;
        m_LootValidatorRefManager.clearReferences();
        ownerSet.clear();
        masterOwnerGuid.Clear();
        lootType = LOOT_NONE;
        lootMethod = NOT_GROUP_TYPE_LOOT;
        roll.clear();
        maxEnchantSkill = 0;
        isReleased = false;
        haveItemOverThreshold = false;
        isChecked = false;
    }

    bool CanLoot(ObjectGuid const& playerGuid, bool onlyRightCheck = false);
    bool IsLootedFor(ObjectGuid const& playerGuid) const;
    bool IsLootedForAll() const;

    void NotifyItemRemoved(uint8 lootIndex);
    void NotifyQuestItemRemoved(uint8 questIndex);
    void NotifyMoneyRemoved();
    void AddLooter(ObjectGuid guid) { m_playersLooting.insert(guid); }
    void RemoveLooter(ObjectGuid guid) { m_playersLooting.erase(guid); }
    void SendReleaseFor(ObjectGuid const& guid);
    void SendReleaseFor(Player* plr);
    void SendReleaseForAll();

    void generateMoneyLoot(uint32 minAmount, uint32 maxAmount);
    bool FillLoot(uint32 loot_id, LootStore const& store, Player* loot_owner, bool personal, bool noEmptyError = false);

    // Inserts the item into the loot (called by LootTemplate processors)
    void AddItem(LootStoreItem const& item);

    LootItem* LootItemInSlot(uint32 lootslot, Player* player, QuestItem** qitem = NULL, QuestItem** ffaitem = NULL, QuestItem** conditem = NULL);
    uint32 GetMaxSlotInLootFor(Player* player) const;

    WorldObject const* GetLootTarget() const { return m_lootTarget; }
    ObjectGuid const& GetLootGuid() const { return m_guidTarget; }
    //WorldObject* GetLootTarget() { return m_lootTarget; };

    void ShowContentTo(Player* plr);
    InventoryResult SendItem(ObjectGuid const& targetGuid, uint32 itemSlot);
    InventoryResult SendItem(Player* target, uint32 itemSlot);
    void Update();
    void SendAllowedLooter();
    void GroupCheck();
    void Release(Player* player);
    void ForceLootAnimationCLientUpdate();
    bool AutoStore(Player* player, bool broadcast = false, uint32 bag = NULL_BAG, uint32 slot = NULL_SLOT);

private:
    void SetGroupLootRight(Player* player);
    void FillNotNormalLootFor(Player* player);
    QuestItemList* FillFFALoot(Player* player);
    QuestItemList* FillQuestLoot(Player* player);
    QuestItemList* FillNonQuestNonFFAConditionalLoot(Player* player);
    LootItemList m_questItems;

    GuidSet m_playersLooting;

    QuestItemMap m_playerQuestItems;
    QuestItemMap m_playerFFAItems;
    QuestItemMap m_playerNonQuestNonFFAConditionalItems;

    // All rolls are registered here. They need to know, when the loot is not valid anymore
    LootValidatorRefManager m_LootValidatorRefManager;

    // What is looted
    WorldObject* m_lootTarget;
    Item*        m_itemTarget;
    ObjectGuid   m_guidTarget;
};

struct LootView
{
    Loot const& loot;
    Player* viewer;
    LootView(Loot const& _loot, Player* _viewer)
        : loot(_loot), viewer(_viewer) {}
};

extern LootStore LootTemplates_Creature;
extern LootStore LootTemplates_Fishing;
extern LootStore LootTemplates_Gameobject;
extern LootStore LootTemplates_Item;
extern LootStore LootTemplates_Mail;
extern LootStore LootTemplates_Milling;
extern LootStore LootTemplates_Pickpocketing;
extern LootStore LootTemplates_Skinning;
extern LootStore LootTemplates_Disenchant;
extern LootStore LootTemplates_Prospecting;
extern LootStore LootTemplates_Spell;

void LoadLootTemplates_Creature();
void LoadLootTemplates_Fishing();
void LoadLootTemplates_Gameobject();
void LoadLootTemplates_Item();
void LoadLootTemplates_Mail();
void LoadLootTemplates_Milling();
void LoadLootTemplates_Pickpocketing();
void LoadLootTemplates_Skinning();
void LoadLootTemplates_Disenchant();
void LoadLootTemplates_Prospecting();

void LoadLootTemplates_Spell();
void LoadLootTemplates_Reference();

inline void LoadLootTables()
{
    LoadLootTemplates_Creature();
    LoadLootTemplates_Fishing();
    LoadLootTemplates_Gameobject();
    LoadLootTemplates_Item();
    LoadLootTemplates_Mail();
    LoadLootTemplates_Milling();
    LoadLootTemplates_Pickpocketing();
    LoadLootTemplates_Skinning();
    LoadLootTemplates_Disenchant();
    LoadLootTemplates_Prospecting();
    LoadLootTemplates_Spell();

    LoadLootTemplates_Reference();
}

class LootMgr
{
public:
    void PlayerVote(Player* player, ObjectGuid const& lootTargetGuid, uint32 itemSlot, RollVote vote);
    Loot* GetLoot(Player* player, ObjectGuid const& targetGuid = ObjectGuid());
    bool IsAllowedToLoot(Player* player, Creature* creature);

    void update(uint32 diff);

private:
    void StartRoll(Loot& loot, uint32 itemSlot);
};

#define sLootMgr MaNGOS::Singleton<LootMgr>::Instance()
#endif
