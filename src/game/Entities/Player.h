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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Common.h"
#include "Entities/ItemPrototype.h"
#include "Entities/Unit.h"
#include "Entities/Item.h"

#include "Database/DatabaseEnv.h"
#include "Quests/QuestDef.h"
#include "Groups/Group.h"
#include "Entities/Bag.h"
#include "Entities/Taxi.h"
#include "Server/WorldSession.h"
#include "Entities/Pet.h"
#include "Maps/MapReference.h"
#include "Util/Util.h"                                           // for Tokens typedef
#include "Achievements/AchievementMgr.h"
#include "Reputation/ReputationMgr.h"
#include "BattleGround/BattleGround.h"
#include "Globals/SharedDefines.h"
#include "Chat/Chat.h"
#include "Server/SQLStorages.h"
#include "Loot/LootMgr.h"
#include "Cinematics/CinematicMgr.h"
#include "LFG/LFG.h"

#include <functional>
#include <vector>

struct Mail;
class Channel;
class DynamicObject;
class Creature;
class PlayerMenu;
class Transport;
class UpdateMask;
class SpellCastTargets;
class PlayerSocial;
class DungeonPersistentState;
class Spell;
class Item;
struct FactionTemplateEntry;

#ifdef BUILD_PLAYERBOT
#include "PlayerBot/Base/PlayerbotMgr.h"
#include "PlayerBot/Base/PlayerbotAI.h"
#endif

struct AreaTrigger;

typedef std::deque<Mail*> PlayerMails;

#define PLAYER_MAX_SKILLS           127
#define PLAYER_MAX_DAILY_QUESTS     25
#define PLAYER_EXPLORED_ZONES_SIZE  128

// TODO: Maybe this can be implemented in configuration file.
#define PLAYER_NEW_INSTANCE_LIMIT_PER_HOUR 5

enum EnvironmentFlags
{
    ENVIRONMENT_FLAG_NONE           = 0x00,
    ENVIRONMENT_FLAG_IN_WATER       = 0x01,                     // Swimming or standing in water
    ENVIRONMENT_FLAG_IN_MAGMA       = 0x02,                     // Swimming or standing in magma
    ENVIRONMENT_FLAG_IN_SLIME       = 0x04,                     // Swimming or standing in slime
    ENVIRONMENT_FLAG_HIGH_SEA       = 0x08,                     // Anywhere inside deep water area
    ENVIRONMENT_FLAG_UNDERWATER     = 0x10,                     // Swimming fully submerged in any liquid
    ENVIRONMENT_FLAG_HIGH_LIQUID    = 0x20,                     // In any liquid deep enough to be able to swim
    ENVIRONMENT_FLAG_LIQUID         = 0x40,                     // Anywhere indide area with any liquid

    ENVIRONMENT_MASK_LIQUID_HAZARD  = (ENVIRONMENT_FLAG_IN_MAGMA | ENVIRONMENT_FLAG_IN_SLIME),
    ENVIRONMENT_MASK_IN_LIQUID      = (ENVIRONMENT_FLAG_IN_WATER | ENVIRONMENT_MASK_LIQUID_HAZARD),
    ENVIRONMENT_MASK_LIQUID_FLAGS   = (ENVIRONMENT_FLAG_UNDERWATER | ENVIRONMENT_MASK_IN_LIQUID | ENVIRONMENT_FLAG_HIGH_SEA | ENVIRONMENT_FLAG_LIQUID | ENVIRONMENT_FLAG_HIGH_LIQUID),
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY    = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER          = 2,
    ERR_BANKSLOT_OK                 = 3
};

enum PlayerSpellState
{
    PLAYERSPELL_UNCHANGED       = 0,
    PLAYERSPELL_CHANGED         = 1,
    PLAYERSPELL_NEW             = 2,
    PLAYERSPELL_REMOVED         = 3
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active            : 1;                             // show in spellbook
    bool dependent         : 1;                             // learned as result another spell learn, skill grow, quest reward, etc
    bool disabled          : 1;                             // first rank has been learned in result talent learn but currently talent unlearned, save max learned ranks
};

struct PlayerTalent
{
    TalentEntry const* talentEntry;
    uint32 currentRank;
    PlayerSpellState state;
};

typedef std::unordered_map<uint32, PlayerSpell> PlayerSpellMap;
typedef std::unordered_map<uint32, PlayerTalent> PlayerTalentMap;

struct SpellCooldown
{
    time_t end;
    uint16 itemid;
};

typedef std::map<uint32, SpellCooldown> SpellCooldowns;

enum TrainerSpellState
{
    TRAINER_SPELL_GREEN         = 0,
    TRAINER_SPELL_RED           = 1,
    TRAINER_SPELL_GRAY          = 2,
    TRAINER_SPELL_GREEN_DISABLED = 10                       // custom value, not send to client: formally green but learn not allowed
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED      = 0,
    ACTIONBUTTON_CHANGED        = 1,
    ACTIONBUTTON_NEW            = 2,
    ACTIONBUTTON_DELETED        = 3
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL         = 0x00,
    ACTION_BUTTON_C             = 0x01,                     // click?
    ACTION_BUTTON_EQSET         = 0x20,
    ACTION_BUTTON_MACRO         = 0x40,
    ACTION_BUTTON_CMACRO        = ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM          = 0x80
};

#define ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAX_ACTION_BUTTON_ACTION_VALUE (0x00FFFFFF+1)

struct ActionButton
{
    ActionButton() : packedData(0), uState(ACTIONBUTTON_NEW) {}

    uint32 packedData;
    ActionButtonUpdateState uState;

    // helpers
    ActionButtonType GetType() const { return ActionButtonType(ACTION_BUTTON_TYPE(packedData)); }
    uint32 GetAction() const { return ACTION_BUTTON_ACTION(packedData); }
    void SetActionAndType(uint32 action, ActionButtonType type)
    {
        uint32 newData = action | (uint32(type) << 24);
        if (newData != packedData || uState == ACTIONBUTTON_DELETED)
        {
            packedData = newData;
            if (uState != ACTIONBUTTON_NEW)
                uState = ACTIONBUTTON_CHANGED;
        }
    }
};

// some action button indexes used in code or clarify structure
enum ActionButtonIndex
{
    ACTION_BUTTON_SHAMAN_TOTEMS_BAR = 132,
};

#define  MAX_ACTION_BUTTONS     144                         // checked in 3.2.0

typedef std::map<uint8, ActionButton> ActionButtonList;

enum GlyphUpdateState
{
    GLYPH_UNCHANGED             = 0,
    GLYPH_CHANGED               = 1,
    GLYPH_NEW                   = 2,
    GLYPH_DELETED               = 3
};

struct Glyph
{
    uint32 id;
    GlyphUpdateState uState;

    Glyph() : id(0), uState(GLYPH_UNCHANGED) { }

    uint32 GetId() const { return id; }

    void SetId(uint32 newId)
    {
        if (newId == id)
            return;

        if (id == 0 && uState == GLYPH_UNCHANGED)           // not exist yet in db and already saved
        {
            uState = GLYPH_NEW;
        }
        else if (newId == 0)
        {
            if (uState == GLYPH_NEW)                        // delete before add new -> no change
                uState = GLYPH_UNCHANGED;
            else                                            // delete existing data
                uState = GLYPH_DELETED;
        }
        else if (uState != GLYPH_NEW)                       // if not new data, change current data
        {
            uState = GLYPH_CHANGED;
        }

        id = newId;
    }
};

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount) {}

    uint32 item_id;
    uint32 item_amount;
};

typedef std::list<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerClassLevelInfo
{
    PlayerClassLevelInfo() : basehealth(0), basemana(0) {}
    uint16 basehealth;
    uint16 basemana;
};

struct PlayerClassInfo
{
    PlayerClassInfo() : levelInfo(nullptr) { }

    PlayerClassLevelInfo* levelInfo;                        //[level-1] 0..MaxPlayerLevel-1
};

struct PlayerLevelInfo
{
    PlayerLevelInfo() { for (unsigned char& stat : stats) stat = 0; }

    uint8 stats[MAX_STATS];
};

typedef std::list<uint32> PlayerCreateInfoSpells;

struct PlayerCreateInfoAction
{
    PlayerCreateInfoAction() : button(0), type(0), action(0) {}
    PlayerCreateInfoAction(uint8 _button, uint32 _action, uint8 _type) : button(_button), type(_type), action(_action) {}

    uint8 button;
    uint8 type;
    uint32 action;
};

typedef std::list<PlayerCreateInfoAction> PlayerCreateInfoActions;

struct PlayerCreateInfoSkill
{
    uint16 SkillId;
    uint16 Step;
};

typedef std::list<PlayerCreateInfoSkill> PlayerCreateInfoSkills;

struct PlayerInfo
{
    // existence checked by displayId != 0             // existence checked by displayId != 0
    PlayerInfo() : mapId(0), areaId(0), positionX(0.f), positionY(0.f), positionZ(0.f), orientation(0.f), displayId_m(0), displayId_f(0), levelInfo(nullptr)
    {
    }

    uint32 mapId;
    uint32 areaId;
    float positionX;
    float positionY;
    float positionZ;
    float orientation;
    uint16 displayId_m;
    uint16 displayId_f;
    PlayerCreateInfoItems item;
    PlayerCreateInfoSpells spell;
    PlayerCreateInfoSkills skill;
    PlayerCreateInfoActions action;

    PlayerLevelInfo* levelInfo;                             //[level-1] 0..MaxPlayerLevel-1
};

struct PvPInfo
{
    PvPInfo() : inPvPCombat(false), inPvPEnforcedArea(false), inPvPCapturePoint(false), isPvPFlagCarrier(false), timerPvPRemaining(0), timerPvPContestedRemaining(0) {}

    bool inPvPCombat;
    bool inPvPEnforcedArea;
    bool inPvPCapturePoint;
    bool isPvPFlagCarrier;
    uint32 timerPvPRemaining;
    uint32 timerPvPContestedRemaining;
};

struct DuelInfo
{
    DuelInfo() : initiator(nullptr), opponent(nullptr), startTimer(0), startTime(0), outOfBound(0) {}

    Player* initiator;
    Player* opponent;
    time_t startTimer;
    time_t startTime;
    time_t outOfBound;
};

struct Areas
{
    uint32 areaID;
    uint32 areaFlag;
    float x1;
    float x2;
    float y1;
    float y2;
};

#define MAX_RUNES               6
#define RUNE_COOLDOWN           (2*5*IN_MILLISECONDS)       // msec

enum RuneType
{
    RUNE_BLOOD                  = 0,
    RUNE_UNHOLY                 = 1,
    RUNE_FROST                  = 2,
    RUNE_DEATH                  = 3,
    NUM_RUNE_TYPES              = 4
};

struct RuneInfo
{
    uint8  BaseRune;
    uint8  CurrentRune;
    uint16 Cooldown;                                        // msec
    std::unordered_set<Aura const*> ConvertAuras;
};

struct Runes
{
    RuneInfo runes[MAX_RUNES];
    uint8 runeState;                                        // mask of available runes

    void SetRuneState(uint8 index, bool set = true)
    {
        if (set)
            runeState |= (1 << index);                      // usable
        else
            runeState &= ~(1 << index);                     // on cooldown
    }
};

struct EnchantDuration
{
    EnchantDuration() : item(nullptr), slot(MAX_ENCHANTMENT_SLOT), leftduration(0) {};
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot), leftduration(_leftduration) { MANGOS_ASSERT(item); };

    Item* item;
    EnchantmentSlot slot;
    uint32 leftduration;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

enum RaidGroupError
{
    ERR_RAID_GROUP_NONE                 = 0,
    ERR_RAID_GROUP_LOWLEVEL             = 1,
    ERR_RAID_GROUP_ONLY                 = 2,
    ERR_RAID_GROUP_FULL                 = 3,
    ERR_RAID_GROUP_REQUIREMENTS_UNMATCH = 4
};

enum DrunkenState
{
    DRUNKEN_SOBER               = 0,
    DRUNKEN_TIPSY               = 1,
    DRUNKEN_DRUNK               = 2,
    DRUNKEN_SMASHED             = 3
};

#define MAX_DRUNKEN             4

enum PlayerFlags
{
    PLAYER_FLAGS_NONE                   = 0x00000000,
    PLAYER_FLAGS_GROUP_LEADER           = 0x00000001,
    PLAYER_FLAGS_AFK                    = 0x00000002,
    PLAYER_FLAGS_DND                    = 0x00000004,
    PLAYER_FLAGS_GM                     = 0x00000008,
    PLAYER_FLAGS_GHOST                  = 0x00000010,
    PLAYER_FLAGS_RESTING                = 0x00000020,
    PLAYER_FLAGS_UNK7                   = 0x00000040,
    PLAYER_FLAGS_UNK8                   = 0x00000080,       // pre-3.0.3 PLAYER_FLAGS_FFA_PVP flag for FFA PVP state
    PLAYER_FLAGS_CONTESTED_PVP          = 0x00000100,       // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_PVP_DESIRED            = 0x00000200,       // Stores player's permanent PvP flag preference
    PLAYER_FLAGS_HIDE_HELM              = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK             = 0x00000800,
    PLAYER_FLAGS_PARTIAL_PLAY_TIME      = 0x00001000,       // played long time
    PLAYER_FLAGS_NO_PLAY_TIME           = 0x00002000,       // played too long time
    PLAYER_FLAGS_IS_OUT_OF_BOUNDS       = 0x00004000,       // Lua_IsOutOfBounds
    PLAYER_FLAGS_DEVELOPER              = 0x00008000,       // <Dev> chat tag, name prefix
    PLAYER_FLAGS_ENABLE_LOW_LEVEL_RAID  = 0x00010000,       // triggers lua event EVENT_ENABLE_LOW_LEVEL_RAID
    PLAYER_FLAGS_TAXI_BENCHMARK         = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER              = 0x00040000,       // 3.0.2, pvp timer active (after you disable pvp manually or leave pvp zones)
    PLAYER_FLAGS_COMMENTATOR            = 0x00080000,       // first appeared in TBC
    PLAYER_FLAGS_UNK21                  = 0x00100000,
    PLAYER_FLAGS_UNK22                  = 0x00200000,
    PLAYER_FLAGS_COMMENTATOR_UBER       = 0x00400000,       // something like COMMENTATOR_CAN_USE_INSTANCE_COMMAND
    PLAYER_ALLOW_ONLY_ABILITY           = 0x00800000,       // EVENT_SPELL_UPDATE_USABLE and EVENT_UPDATE_SHAPESHIFT_USABLE, disabled all abilitys on tab except autoattack
    PLAYER_FLAGS_UNK25                  = 0x01000000,       // EVENT_SPELL_UPDATE_USABLE and EVENT_UPDATE_SHAPESHIFT_USABLE, disabled all melee ability on tab include autoattack
    PLAYER_FLAGS_XP_USER_DISABLED       = 0x02000000,
};

#define KNOWN_TITLES_SIZE       3
#define MAX_TITLE_INDEX         (KNOWN_TITLES_SIZE*64)      // 3 uint64 fields

// used in (PLAYER_FIELD_BYTES, 0) byte values
enum PlayerFieldByteFlags
{
    PLAYER_FIELD_BYTE_TRACK_STEALTHED   = 0x02,
    PLAYER_FIELD_BYTE_RELEASE_TIMER     = 0x08,             // Display time till auto release spirit
    PLAYER_FIELD_BYTE_NO_RELEASE_WINDOW = 0x10              // Display no "release spirit" window at all
};

// used in byte (PLAYER_FIELD_BYTES2,3) values
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTE2_NONE              = 0x00,
    PLAYER_FIELD_BYTE2_DETECT_AMORE_0    = 0x02,            // SPELL_AURA_DETECT_AMORE, not used as value and maybe not relcted to, but used in code as base for mask apply
    PLAYER_FIELD_BYTE2_DETECT_AMORE_1    = 0x04,            // SPELL_AURA_DETECT_AMORE value 1
    PLAYER_FIELD_BYTE2_DETECT_AMORE_2    = 0x08,            // SPELL_AURA_DETECT_AMORE value 2
    PLAYER_FIELD_BYTE2_DETECT_AMORE_3    = 0x10,            // SPELL_AURA_DETECT_AMORE value 3
    PLAYER_FIELD_BYTE2_STEALTH           = 0x20,
    PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW = 0x40
};

enum PlayerFieldBytesOffsets
{
    PLAYER_FIELD_BYTES_OFFSET_FLAGS                 = 0,
    PLAYER_FIELD_BYTES_OFFSET_RAF_GRANTABLE_LEVEL   = 1,
    PLAYER_FIELD_BYTES_OFFSET_ACTION_BAR_TOGGLES    = 2,
    PLAYER_FIELD_BYTES_OFFSET_LIFETIME_MAX_PVP_RANK = 3
};

class MirrorTimer
{
    public:
        enum Type
        {
            FATIGUE         = 0,
            BREATH          = 1,
            FEIGNDEATH      = 2,

            NUM_CLIENT_TIMERS,

            ENVIRONMENTAL   = NUM_CLIENT_TIMERS,

            NUM_TIMERS
        };

        enum Status
        {
            UNCHANGED       = 0,
            FULL_UPDATE     = 1,
            STATUS_UPDATE   = 2,
        };

        MirrorTimer(Type type) : m_type(type), m_scale(-1), m_spellId(0), m_status(UNCHANGED), m_active(false), m_frozen(false) {}

        inline bool     IsActive() const { return m_active; }
        inline bool     IsRegenerating() const { return (m_scale > 0); }
        inline bool     IsFrozen() const { return (m_frozen && !IsRegenerating()); }

        inline Type     GetType() const { return m_type; }
        inline uint32   GetRemaining() const { return (m_tracker.GetInterval() - m_tracker.GetCurrent()); }
        inline uint32   GetDuration() const { return m_tracker.GetInterval(); }
        inline int32    GetScale() const { return m_scale; }
        inline uint32   GetSpellId() const { return m_spellId; }

        inline Status   FetchStatus();

        inline void Stop();

        inline void Start(uint32 interval, uint32 spellId = 0);
        inline void Start(uint32 current, uint32 max, uint32 spellId);

        inline void SetRemaining(uint32 duration);
        inline void SetDuration(uint32 duration);

        inline void SetFrozen(bool state);

        inline void SetScale(int32 scale);

        bool Update(uint32 diff);

    private:
        Type m_type;
        int32 m_scale;
        uint32 m_spellId;

        ShortIntervalTimer m_tracker;
        ShortIntervalTimer m_pulse;

        Status m_status;

        bool m_active;
        bool m_frozen;
};

// 2^n values
enum PlayerExtraFlags
{
    // gm abilities
    PLAYER_EXTRA_GM_ON              = 0x0001,
    PLAYER_EXTRA_GM_ACCEPT_TICKETS  = 0x0002,
    PLAYER_EXTRA_ACCEPT_WHISPERS    = 0x0004,
    PLAYER_EXTRA_TAXICHEAT          = 0x0008,
    PLAYER_EXTRA_GM_INVISIBLE       = 0x0010,
    PLAYER_EXTRA_GM_CHAT            = 0x0020,               // Show GM badge in chat messages
    PLAYER_EXTRA_AUCTION_NEUTRAL    = 0x0040,
    PLAYER_EXTRA_AUCTION_ENEMY      = 0x0080,               // overwrite PLAYER_EXTRA_AUCTION_NEUTRAL

    // other states
    PLAYER_EXTRA_PVP_DEATH          = 0x0100,                // store PvP death status until corpse creating.
    PLAYER_EXTRA_WHISP_RESTRICTION  = 0x0200,
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE               = 0x00,
    AT_LOGIN_RENAME             = 0x01,
    AT_LOGIN_RESET_SPELLS       = 0x02,
    AT_LOGIN_RESET_TALENTS      = 0x04,
    AT_LOGIN_CUSTOMIZE          = 0x08,
    AT_LOGIN_RESET_PET_TALENTS  = 0x10,
    AT_LOGIN_FIRST              = 0x20,
    AT_LOGIN_RESET_TAXINODES    = 0x40,
    AT_LOGIN_ADD_BG_DESERTER    = 0x80
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET             = 0,
    QUEST_STATE_OFFSET          = 1,
    QUEST_COUNTS_OFFSET         = 2,                        // 2 and 3
    QUEST_TIME_OFFSET           = 4
};

#define MAX_QUEST_OFFSET 5

enum QuestSlotStateMask
{
    QUEST_STATE_NONE            = 0x0000,
    QUEST_STATE_COMPLETE        = 0x0001,
    QUEST_STATE_FAIL            = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED             = 0,
    SKILL_CHANGED               = 1,
    SKILL_NEW                   = 2,
    SKILL_DELETED               = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState)
    {
    }
    uint8 pos;
    SkillUpdateState uState;
};

typedef std::unordered_map<uint32, SkillStatusData> SkillStatusMap;

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START           = 0,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END             = 150,
    PLAYER_SLOTS_COUNT          = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

#define INVENTORY_SLOT_BAG_0    255

enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_START    = 19,
    INVENTORY_SLOT_BAG_END      = 23
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START   = 23,
    INVENTORY_SLOT_ITEM_END     = 39
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START        = 39,
    BANK_SLOT_ITEM_END          = 67
};

enum BankBagSlots                                           // 7 slots
{
    BANK_SLOT_BAG_START         = 67,
    BANK_SLOT_BAG_END           = 74
};

enum BuyBackSlots                                           // 12 slots
{
    // stored in m_buybackitems
    BUYBACK_SLOT_START          = 74,
    BUYBACK_SLOT_END            = 86
};

enum KeyRingSlots                                           // 32 slots
{
    KEYRING_SLOT_START          = 86,
    KEYRING_SLOT_END            = 118
};

enum CurrencyTokenSlots                                     // 32 slots
{
    CURRENCYTOKEN_SLOT_START    = 118,
    CURRENCYTOKEN_SLOT_END      = 150
};

enum EquipmentSetUpdateState
{
    EQUIPMENT_SET_UNCHANGED     = 0,
    EQUIPMENT_SET_CHANGED       = 1,
    EQUIPMENT_SET_NEW           = 2,
    EQUIPMENT_SET_DELETED       = 3
};

struct EquipmentSet
{
    EquipmentSet() : Guid(0), IgnoreMask(0), state(EQUIPMENT_SET_NEW)
    {
        for (unsigned int& Item : Items)
            Item = 0;
    }

    uint64 Guid;
    std::string Name;
    std::string IconName;
    uint32 IgnoreMask;
    uint32 Items[EQUIPMENT_SLOT_END];
    EquipmentSetUpdateState state;
};

#define MAX_EQUIPMENT_SET_INDEX 10                          // client limit

typedef std::map<uint32, EquipmentSet> EquipmentSets;

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint32 _count) : pos(_pos), count(_count) {}
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint32 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

enum TradeSlots
{
    TRADE_SLOT_COUNT            = 7,
    TRADE_SLOT_TRADED_COUNT     = 6,
    TRADE_SLOT_NONTRADED        = 6
};

enum TransferAbortReason
{
    TRANSFER_ABORT_NONE                         = 0x00,
    TRANSFER_ABORT_ERROR                        = 0x01,
    TRANSFER_ABORT_MAX_PLAYERS                  = 0x02,     // Transfer Aborted: instance is full
    TRANSFER_ABORT_NOT_FOUND                    = 0x03,     // Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES           = 0x04,     // You have entered too many instances recently.
    TRANSFER_ABORT_ZONE_IN_COMBAT               = 0x06,     // Unable to zone in while an encounter is in progress.
    TRANSFER_ABORT_INSUF_EXPAN_LVL              = 0x07,     // You must have <TBC,WotLK> expansion installed to access this area.
    TRANSFER_ABORT_DIFFICULTY                   = 0x08,     // <Normal,Heroic,Epic> difficulty mode is not available for %s.
    TRANSFER_ABORT_UNIQUE_MESSAGE               = 0x09,     // Until you've escaped TLK's grasp, you cannot leave this place!
    TRANSFER_ABORT_TOO_MANY_REALM_INSTANCES     = 0x0A,     // Additional instances cannot be launched, please try again later.
    TRANSFER_ABORT_NEED_GROUP                   = 0x0B,     // 3.1
    TRANSFER_ABORT_NOT_FOUND2                   = 0x0C,     // 3.1
    TRANSFER_ABORT_NOT_FOUND3                   = 0x0D,     // 3.1
    TRANSFER_ABORT_NOT_FOUND4                   = 0x0E,     // 3.2
    TRANSFER_ABORT_REALM_ONLY                   = 0x0F,     // All players on party must be from the same realm.
    TRANSFER_ABORT_MAP_NOT_ALLOWED              = 0x10,     // Map can't be entered at this time.
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS     = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN       = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON  = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME           = 4,                    // Welcome to %s. This raid instance is scheduled to reset in %s.
    RAID_INSTANCE_EXPIRED           = 5
};

// PLAYER_FIELD_ARENA_TEAM_INFO_1_1 offsets
enum ArenaTeamInfoType
{
    ARENA_TEAM_ID               = 0,
    ARENA_TEAM_TYPE             = 1,                        // new in 3.2 - team type?
    ARENA_TEAM_MEMBER           = 2,                        // 0 - captain, 1 - member
    ARENA_TEAM_GAMES_WEEK       = 3,
    ARENA_TEAM_GAMES_SEASON     = 4,
    ARENA_TEAM_WINS_SEASON      = 5,
    ARENA_TEAM_PERSONAL_RATING  = 6,
    ARENA_TEAM_END              = 7
};

enum RestType
{
    REST_TYPE_NO                = 0,
    REST_TYPE_IN_TAVERN         = 1,
    REST_TYPE_IN_CITY           = 2,
    REST_TYPE_FACTION_AREA      = 4,
};

enum DuelCompleteType
{
    DUEL_INTERRUPTED            = 0,
    DUEL_WON                    = 1,
    DUEL_FLED                   = 2
};

enum TeleportToOptions
{
    TELE_TO_GM_MODE             = 0x01,
    TELE_TO_NOT_LEAVE_TRANSPORT = 0x02,
    TELE_TO_NOT_LEAVE_COMBAT    = 0x04,
    TELE_TO_NOT_UNSUMMON_PET    = 0x08,
    TELE_TO_SPELL               = 0x10,
};

/// Type of environmental damages
enum EnviromentalDamage
{
    DAMAGE_EXHAUSTED            = 0,
    DAMAGE_DROWNING             = 1,
    DAMAGE_FALL                 = 2,
    DAMAGE_LAVA                 = 3,
    DAMAGE_SLIME                = 4,
    DAMAGE_FIRE                 = 5,
    DAMAGE_FALL_TO_VOID         = 6                         // custom case for fall without durability loss
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL           = 0,
    PLAYED_TIME_LEVEL           = 1
};

#define MAX_PLAYED_TIME_INDEX   2

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM,
    PLAYER_LOGIN_QUERY_LOADGROUP,
    PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES,
    PLAYER_LOGIN_QUERY_LOADAURAS,
    PLAYER_LOGIN_QUERY_LOADSPELLS,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADDAILYQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADREPUTATION,
    PLAYER_LOGIN_QUERY_LOADINVENTORY,
    PLAYER_LOGIN_QUERY_LOADITEMLOOT,
    PLAYER_LOGIN_QUERY_LOADACTIONS,
    PLAYER_LOGIN_QUERY_LOADSOCIALLIST,
    PLAYER_LOGIN_QUERY_LOADHOMEBIND,
    PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS,
    PLAYER_LOGIN_QUERY_LOADDECLINEDNAMES,
    PLAYER_LOGIN_QUERY_LOADGUILD,
    PLAYER_LOGIN_QUERY_LOADARENAINFO,
    PLAYER_LOGIN_QUERY_LOADACHIEVEMENTS,
    PLAYER_LOGIN_QUERY_LOADCRITERIAPROGRESS,
    PLAYER_LOGIN_QUERY_LOADEQUIPMENTSETS,
    PLAYER_LOGIN_QUERY_LOADBGDATA,
    PLAYER_LOGIN_QUERY_LOADACCOUNTDATA,
    PLAYER_LOGIN_QUERY_LOADSKILLS,
    PLAYER_LOGIN_QUERY_LOADGLYPHS,
    PLAYER_LOGIN_QUERY_LOADMAILS,
    PLAYER_LOGIN_QUERY_LOADMAILEDITEMS,
    PLAYER_LOGIN_QUERY_LOADTALENTS,
    PLAYER_LOGIN_QUERY_LOADWEEKLYQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADMONTHLYQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADRANDOMBATTLEGROUND,

    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER         = 0x01,
    DELAYED_RESURRECT_PLAYER    = 0x02,
    DELAYED_SPELL_CAST_DESERTER = 0x04,
    DELAYED_BG_MOUNT_RESTORE    = 0x08,                     ///< Flag to restore mount state after teleport from BG
    DELAYED_BG_TAXI_RESTORE     = 0x10,                     ///< Flag to restore taxi state after teleport from BG
    DELAYED_END
};

enum ReputationSource
{
    REPUTATION_SOURCE_KILL,
    REPUTATION_SOURCE_QUEST,
    REPUTATION_SOURCE_SPELL
};

// Player summoning auto-decline time (in secs)
#define MAX_PLAYER_SUMMON_DELAY (2*MINUTE)
#define MAX_MONEY_AMOUNT        (0x7FFFFFFF-1)

enum BindExtensionState
{
    EXTEND_STATE_EXPIRED  =   0,
    EXTEND_STATE_NORMAL   =   1,
    EXTEND_STATE_EXTENDED =   2,
    EXTEND_STATE_KEEP     = 255   // special state: keep current save type
};

struct InstancePlayerBind
{
    DungeonPersistentState* state;
    bool perm;
    /* permanent PlayerInstanceBinds are created in Raid/Heroic instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    /* extend state listing:
    EXPIRED  - doesn't affect anything unless manually re-extended by player
    NORMAL   - standard state
    EXTENDED - won't be promoted to EXPIRED at next reset period, will instead be promoted to NORMAL */
    BindExtensionState extendState;
    InstancePlayerBind() : state(nullptr), perm(false), extendState(EXTEND_STATE_NORMAL) {}
};

enum ReferAFriendError
{
    ERR_REFER_A_FRIEND_NONE                          = 0x00,
    ERR_REFER_A_FRIEND_NOT_REFERRED_BY               = 0x01,
    ERR_REFER_A_FRIEND_TARGET_TOO_HIGH               = 0x02,
    ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS = 0x03,
    ERR_REFER_A_FRIEND_TOO_FAR                       = 0x04,
    ERR_REFER_A_FRIEND_DIFFERENT_FACTION             = 0x05,
    ERR_REFER_A_FRIEND_NOT_NOW                       = 0x06,
    ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I             = 0x07,
    ERR_REFER_A_FRIEND_NO_TARGET                     = 0x08,
    ERR_REFER_A_FRIEND_NOT_IN_GROUP                  = 0x09,
    ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I            = 0x0A,
    ERR_REFER_A_FRIEND_SUMMON_COOLDOWN               = 0x0B,
    ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL               = 0x0C,
    ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S              = 0x0D
};

enum PlayerRestState
{
    REST_STATE_RESTED           = 0x01,
    REST_STATE_NORMAL           = 0x02,
    REST_STATE_RAF_LINKED       = 0x04                      // Exact use unknown
};

enum SetPlayerDifficultyResult
{
    RESULT_SET_DIFFICULTY           = 0, // payload bool isHeroic
    RESULT_COOLDOWN                 = 1, // payload time
    RESULT_WORLDSTATE               = 2, // failed state - maybe instance state like gunship moving?
    RESULT_ENCOUNTER_IN_PROGRESS    = 3,
    RESULT_PLAYER_IN_COMBAT         = 4,
    RESULT_PLAYER_BUSY              = 5, // unk what it could be
    RESULT_START                    = 6, // starts loading screen
    RESULT_ALREADY_IN_PROGRESS      = 7, // we do it in place, originally its likely delayed to map update
    RESULT_FAILED_CONDITION         = 8, // likely missing areatrigger transition condition
    RESULT_COMPLETE                 = 9, // finishes and sticks changes on client
};

class PlayerTaxi
{
    public:
        PlayerTaxi();
        ~PlayerTaxi() {}
        // Nodes
        void InitTaxiNodesForLevel(uint32 race, uint32 chrClass, uint32 level);
        void LoadTaxiMask(const char* data);

        bool IsTaximaskNodeKnown(uint32 nodeidx) const
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            return (m_taximask[field] & submask) == submask;
        }
        bool SetTaximaskNode(uint32 nodeidx)
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            if ((m_taximask[field] & submask) != submask)
            {
                m_taximask[field] |= submask;
                return true;
            }
            return false;
        }
        void AppendTaximaskTo(ByteBuffer& data, bool all);

        friend std::ostringstream& operator<<(std::ostringstream& ss, PlayerTaxi const& taxi);

    private:
        TaxiMask m_taximask;
};

std::ostringstream& operator<< (std::ostringstream& ss, PlayerTaxi const& taxi);

/// Holder for BattleGround data
struct BGData
{
    BGData() : bgInstanceID(0), bgTypeID(BATTLEGROUND_TYPE_NONE), bgAfkReportedCount(0), bgAfkReportedTimer(0),
        bgTeam(TEAM_NONE), mountSpell(0), m_needSave(false) {}

    uint32 bgInstanceID;                                    ///< This variable is set to bg->m_InstanceID, saved
    ///  when player is teleported to BG - (it is battleground's GUID)
    BattleGroundTypeId bgTypeID;

    std::set<uint32>   bgAfkReporter;
    uint8              bgAfkReportedCount;
    time_t             bgAfkReportedTimer;

    Team bgTeam;                                            ///< What side the player will be added to, saved

    uint32 mountSpell;                                      ///< Mount used before join to bg, saved

    WorldLocation joinPos;                                  ///< From where player entered BG, saved

    bool m_needSave;                                        ///< true, if saved to DB fields modified after prev. save (marked as "saved" above)
};

struct TradeStatusInfo
{
    TradeStatusInfo() : Status(TRADE_STATUS_BUSY), TraderGuid(), Result(EQUIP_ERR_OK),
        IsTargetResult(false), ItemLimitCategoryId(0), Slot(0) { }

    TradeStatus Status;
    ObjectGuid TraderGuid;
    InventoryResult Result;
    bool IsTargetResult;
    uint32 ItemLimitCategoryId;
    uint8 Slot;
};

class TradeData
{
    public:                                                 // constructors
        TradeData(Player* player, Player* trader) :
            m_player(player),  m_trader(trader), m_accepted(false), m_acceptProccess(false),
            m_money(0), m_spell(0) {}

        Player* GetTrader() const { return m_trader; }
        TradeData* GetTraderData() const;

        Item* GetItem(TradeSlots slot) const;
        bool HasItem(ObjectGuid item_guid) const;

        uint32 GetSpell() const { return m_spell; }
        Item*  GetSpellCastItem() const;
        bool HasSpellCastItem() const { return !m_spellCastItem.IsEmpty(); }

        uint32 GetMoney() const { return m_money; }

        bool IsAccepted() const { return m_accepted; }
        bool IsInAcceptProcess() const { return m_acceptProccess; }

        void SetItem(TradeSlots slot, Item* item);
        void SetSpell(uint32 spell_id, Item* castItem = nullptr);
        void SetMoney(uint32 money);

        void SetAccepted(bool state, bool crosssend = false);

        // must be called only from accept handler helper functions
        void SetInAcceptProcess(bool state) { m_acceptProccess = state; }

    private:                                                // internal functions

        void Update(bool for_trader = true) const;

        Player*    m_player;                                // Player who own of this TradeData
        Player*    m_trader;                                // Player who trade with m_player

        bool       m_accepted;                              // m_player press accept for trade list
        bool       m_acceptProccess;                        // one from player/trader press accept and this processed

        uint32     m_money;                                 // m_player place money to trade

        uint32     m_spell;                                 // m_player apply spell to non-traded slot item
        ObjectGuid m_spellCastItem;                         // applied spell casted by item use

        ObjectGuid m_items[TRADE_SLOT_COUNT];               // traded itmes from m_player side including non-traded slot
};

class Player : public Unit
{
        friend class WorldSession;
        friend class CinematicMgr;

        friend void Item::AddToUpdateQueueOf(Player* player);
        friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
        explicit Player(WorldSession* session);
        ~Player();

        void CleanupsBeforeDelete() override;

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation, uint32 options = 0, AreaTrigger const* at = nullptr, GenericTransport* transport = nullptr);

        bool TeleportTo(WorldLocation const& loc, uint32 options = 0)
        {
            return TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation, options);
        }

        bool TeleportToBGEntryPoint();

        void SetSummonPoint(uint32 mapid, float x, float y, float z, ObjectGuid summoner)
        {
            m_summon_expire = time(nullptr) + MAX_PLAYER_SUMMON_DELAY;
            m_summon_mapid = mapid;
            m_summon_x = x;
            m_summon_y = y;
            m_summon_z = z;
            m_summoner = summoner;
        }
        void SummonIfPossible(bool agree, ObjectGuid guid);

        bool Create(uint32 guidlow, const std::string& name, uint8 race, uint8 class_, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair, uint8 outfitId);

        void Update(const uint32 diff) override;
        void Heartbeat() override;

        static bool BuildEnumData(QueryResult* result,  WorldPacket& p_data);

        void SendInitialPacketsBeforeAddToMap();
        void SendInitialPacketsAfterAddToMap();
        void SendInstanceResetWarning(uint32 mapid, Difficulty difficulty, uint32 time) const;

        Creature* GetNPCIfCanInteractWith(ObjectGuid guid, uint32 npcflagmask);
        GameObject* GetGameObjectIfCanInteractWith(ObjectGuid guid, uint32 gameobject_type = MAX_GAMEOBJECT_TYPE);
        bool CanSeeSpecialInfoOf(Unit const* target) const;

        ReputationRank GetReactionTo(Unit const* unit) const override;
        ReputationRank GetReactionTo(Corpse const* corpse) const override;
        bool IsInGroup(Unit const* other, bool party = false, bool ignoreCharms = false) const override;

        bool Mount(uint32 displayid, const Aura* aura = nullptr) override;
        bool Unmount(const Aura* aura = nullptr) override;

        void ToggleAFK();
        void ToggleDND();
        bool isAFK() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK); }
        bool isDND() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND); }
        ChatTagFlags GetChatTag() const;
        std::string autoReplyMsg;

        uint32 GetBarberShopCost(uint8 newhairstyle, uint8 newhaircolor, uint8 newfacialhair, uint32 newskintone) const;

        PlayerSocial* GetSocial() { return m_social; }
        const PlayerSocial* GetSocial() const { return m_social; }

        bool isAcceptTickets() const;
        void SetAcceptTicket(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_ACCEPT_TICKETS; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_ACCEPT_TICKETS; }
        bool isAcceptWhispers() const { return (m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS) != 0; }
        void SetAcceptWhispers(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS; else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS; }
        bool IsGameMaster() const { return m_ExtraFlags & PLAYER_EXTRA_GM_ON; }
        void SetGameMaster(bool on);
        bool isGMChat() const;
        void SetGMChat(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_CHAT; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_CHAT; }
        bool isTaxiCheater() const { return (m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT) != 0; }
        void SetTaxiCheater(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT; else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT; }
        bool isGMVisible() const { return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE); }
        void SetGMVisible(bool on);
        void SetPvPDeath(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH; else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH; }
        bool isDebuggingAreaTriggers() { return m_isDebuggingAreaTriggers; }
        void SetDebuggingAreaTriggers(bool on) { m_isDebuggingAreaTriggers = on; }
        bool isAllowedWhisperFrom(ObjectGuid guid);
        bool isEnabledWhisperRestriction() const { return m_ExtraFlags & PLAYER_EXTRA_WHISP_RESTRICTION; }
        void SetWhisperRestriction(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_WHISP_RESTRICTION; else m_ExtraFlags &= ~PLAYER_EXTRA_WHISP_RESTRICTION; }

        // 0 = own auction, -1 = enemy auction, 1 = goblin auction
        int GetAuctionAccessMode() const { return m_ExtraFlags & PLAYER_EXTRA_AUCTION_ENEMY ? -1 : (m_ExtraFlags & PLAYER_EXTRA_AUCTION_NEUTRAL ? 1 : 0); }
        void SetAuctionAccessMode(int state)
        {
            m_ExtraFlags &= ~(PLAYER_EXTRA_AUCTION_ENEMY | PLAYER_EXTRA_AUCTION_NEUTRAL);

            if (state < 0)
                m_ExtraFlags |= PLAYER_EXTRA_AUCTION_ENEMY;
            else if (state > 0)
                m_ExtraFlags |= PLAYER_EXTRA_AUCTION_NEUTRAL;
        }

        void GiveXP(uint32 xp, Creature* victim, float groupRate = 1.f);
        void GiveLevel(uint32 level);

        void InitStatsForLevel(bool reapplyMods = false);
        uint32 GetMaxAttainableLevel() const { return GetUInt32Value(PLAYER_FIELD_MAX_LEVEL); }
        void OnExpansionChange();

        // Played Time Stuff
        time_t m_logintime;
        time_t m_Last_tick;

        uint32 m_Played_time[MAX_PLAYED_TIME_INDEX];
        uint32 GetTotalPlayedTime() { return m_Played_time[PLAYED_TIME_TOTAL]; }
        uint32 GetLevelPlayedTime() { return m_Played_time[PLAYED_TIME_LEVEL]; }

        Player* GetSpellModOwner() const override { return const_cast<Player*>(this); }

        void SetDeathState(DeathState s) override;          // overwrite Unit::SetDeathState

        float GetRestBonus() const { return m_rest_bonus; }
        void SetRestBonus(float rest_bonus_new);

        bool IsRafResting() const;
        bool IsAtRecruitAFriendDistance(WorldObject const* other) const;
        bool GetsRecruitAFriendBonus();
        uint32 GetGrantableLevels() const { return m_grantableLevels; }
        void SetGrantableLevels(uint32 levels) { m_grantableLevels = levels; }

        /**
        * \brief: compute rest bonus
        * \param: time_t timePassed > time from last check
        * \param: bool offline      > is the player was offline?
        * \param: bool inRestPlace  > if it was offline, is the player was in city/tavern/inn?
        * \returns: float
        **/
        float ComputeRest(time_t timePassed, bool offline = false, bool inRestPlace = false) const;

        /**
        * \brief: player is interacting with something.
        * \param: ObjectGuid interactObj > object that interact with this player
        **/
        void DoInteraction();
        void DoLoot();
        RestType GetRestType() const { return m_restType; }
        void SetRestType(RestType n_r_type, uint32 areaTriggerId = 0);

        time_t GetTimeInnEnter() const { return time_inn_enter; }
        void UpdateInnerTime(time_t time) { time_inn_enter = time; }

        void RemovePet(PetSaveMode mode);
        uint32 GetPhaseMaskForSpawn() const;                // used for proper set phase for DB at GM-mode creature/GO spawn

        void Say(const std::string& text, const uint32 language) const;
        void Yell(const std::string& text, const uint32 language) const;
        void TextEmote(const std::string& text) const;
        void Whisper(const std::string& text, const uint32 language, ObjectGuid receiver);

        /*********************************************************/
        /***                    TAXI SYSTEM                    ***/
        /*********************************************************/

        // Legacy taxi system
        PlayerTaxi m_taxi;

        void InitTaxiNodesForLevel() { m_taxi.InitTaxiNodesForLevel(getRace(), getClass(), GetLevel()); }

        bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, Creature* npc = nullptr, uint32 spellid = 0);
        bool ActivateTaxiPathTo(uint32 path_id, uint32 spellid = 0);

        // New taxi system
        void TaxiFlightResume(bool forceRenewMoveGen = false);
        bool TaxiFlightInterrupt(bool cancel = true);

        bool IsTaxiDebug() const { return m_taxiTracker.m_debug; }
        void ToggleTaxiDebug() { m_taxiTracker.m_debug = !m_taxiTracker.m_debug; }

        Taxi::Map const& GetTaxiPathSpline() const;
        int32 GetTaxiPathSplineOffset() const;

        void OnTaxiFlightStart(const TaxiPathEntry* path);
        void OnTaxiFlightEnd(const TaxiPathEntry* path);
        void OnTaxiFlightEject(bool clear = true);
        bool OnTaxiFlightUpdate(const size_t waypointIndex, const bool movement);
        void OnTaxiFlightSplineStart(const TaxiPathNodeEntry* node);
        void OnTaxiFlightSplineEnd();
        bool OnTaxiFlightSplineUpdate();
        void OnTaxiFlightRouteStart(uint32 pathID, bool initial);
        void OnTaxiFlightRouteEnd(uint32 pathID, bool final);
        void OnTaxiFlightRouteProgress(const TaxiPathNodeEntry* node, const TaxiPathNodeEntry* next = nullptr);

        /*********************************************************/
        /***                    STORAGE SYSTEM                 ***/
        /*********************************************************/

        void SetVirtualItemSlot(uint8 i, Item* item);
        void SetSheath(SheathState sheathed) override;      // overwrite Unit version
        bool ViableEquipSlots(ItemPrototype const* proto, uint8* viable_slots) const;
        uint8 FindEquipSlot(ItemPrototype const* proto, uint32 slot, bool swap) const;
        uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item* skipItem = nullptr) const;
        uint32 GetItemCountWithLimitCategory(uint32 limitCategory, Item* skipItem = nullptr) const;
        Item* GetItemByGuid(ObjectGuid guid) const;
        Item* GetItemByEntry(uint32 item) const;            // only for special cases
        Item* GetItemByLimitedCategory(uint32 limitedCategory) const;
        Item* GetItemByPos(uint16 pos) const;
        Item* GetItemByPos(uint8 bag, uint8 slot) const;
        uint32 GetItemDisplayIdInSlot(uint8 bag, uint8 slot) const;
        Item* GetWeaponForAttack(WeaponAttackType attackType) const { return GetWeaponForAttack(attackType, false, false); }
        Item* GetWeaponForAttack(WeaponAttackType attackType, bool nonbroken, bool useable) const;
        Item* GetShield(bool useable = false) const;
        static uint32 GetAttackBySlot(uint8 slot);          // MAX_ATTACK if not weapon slot
        std::vector<Item*>& GetItemUpdateQueue() { return m_itemUpdateQueue; }
        static bool IsInventoryPos(uint16 pos) { return IsInventoryPos(pos >> 8, pos & 255); }
        static bool IsInventoryPos(uint8 bag, uint8 slot);
        static bool IsEquipmentPos(uint16 pos) { return IsEquipmentPos(pos >> 8, pos & 255); }
        static bool IsEquipmentPos(uint8 bag, uint8 slot);
        static bool IsBagPos(uint16 pos);
        static bool IsBankPos(uint16 pos) { return IsBankPos(pos >> 8, pos & 255); }
        static bool IsBankPos(uint8 bag, uint8 slot);
        bool IsValidPos(uint16 pos, bool explicit_pos) const { return IsValidPos(pos >> 8, pos & 255, explicit_pos); }
        bool IsValidPos(uint8 bag, uint8 slot, bool explicit_pos) const;
        uint8 GetBankBagSlotCount() const { return GetByteValue(PLAYER_BYTES_2, 2); }
        void SetBankBagSlotCount(uint8 count) { SetByteValue(PLAYER_BYTES_2, 2, count); }
        bool HasItemCount(uint32 item, uint32 count, bool inBankAlso = false) const;
        bool HasItemFitToSpellReqirements(SpellEntry const* spellInfo, Item const* ignoreItem = nullptr, uint32* error = nullptr) const;
        bool CanNoReagentCast(SpellEntry const* spellInfo) const;
        bool HasItemOrGemWithIdEquipped(uint32 item, uint32 count, uint8 except_slot = NULL_SLOT) const;
        bool HasItemOrGemWithLimitCategoryEquipped(uint32 limitCategory, uint32 count, uint8 except_slot = NULL_SLOT) const;
        InventoryResult CanTakeMoreSimilarItems(Item* pItem, uint32* itemLimitCategory = nullptr) const { return _CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem, itemLimitCategory); }
        InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count, uint32* itemLimitCategory = nullptr) const { return _CanTakeMoreSimilarItems(entry, count, nullptr, itemLimitCategory); }
        InventoryResult CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = nullptr) const
        {
            return _CanStoreItem(bag, slot, dest, item, count, nullptr, false, no_space_count);
        }
        InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const
        {
            if (!pItem)
                return EQUIP_ERR_ITEM_NOT_FOUND;
            uint32 count = pItem->GetCount();
            return _CanStoreItem(bag, slot, dest, pItem->GetEntry(), count, pItem, swap, nullptr);
        }
        InventoryResult CanStoreItems(Item** pItems, int count, uint32* itemLimitCategory) const;
        InventoryResult CanEquipNewItem(uint8 slot, uint16& dest, uint32 item, bool swap) const;
        InventoryResult CanEquipItem(uint8 slot, uint16& dest, Item* pItem, bool swap, bool direct_action = true) const;

        InventoryResult CanEquipUniqueItem(Item* pItem, uint8 eslot = NULL_SLOT, uint32 limit_count = 1) const;
        InventoryResult CanEquipUniqueItem(ItemPrototype const* itemProto, uint8 except_slot = NULL_SLOT, uint32 limit_count = 1) const;
        InventoryResult CanUnequipItems(uint32 item, uint32 count) const;
        InventoryResult CanUnequipItem(uint16 pos, bool swap) const;
        InventoryResult CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanUseItem(Item* pItem, bool direct_action = true) const;
        bool HasItemTotemCategory(uint32 TotemCategory) const;
        InventoryResult CanUseItem(ItemPrototype const* pProto) const;
        InventoryResult CanUseAmmo(uint32 item) const;
        Item* StoreNewItem(ItemPosCountVec const& dest, uint32 item, bool update, int32 randomPropertyId = 0);
        Item* StoreItem(ItemPosCountVec const& dest, Item* pItem, bool update);
        Item* EquipNewItem(uint16 pos, uint32 item, bool update);
        Item* EquipItem(uint16 pos, Item* pItem, bool update);
        void AutoUnequipOffhandIfNeed(uint8 bag = NULL_BAG);
        bool StoreNewItemInBestSlots(uint32 titem_id, uint32 titem_amount);
        Item* StoreNewItemInInventorySlot(uint32 itemEntry, uint32 amount);

        bool hasWeapon(WeaponAttackType type) const override { return GetWeaponForAttack(type, false, false); }
        bool hasWeaponForAttack(WeaponAttackType type) const override { return (Unit::hasWeaponForAttack(type) && GetWeaponForAttack(type, true, true)); }

        Item* ConvertItem(Item* item, uint32 newItemId);

        InventoryResult _CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item* pItem, uint32* no_space_count = nullptr, uint32* itemLimitCategory = nullptr) const;
        InventoryResult _CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item* pItem = nullptr, bool swap = false, uint32* no_space_count = nullptr) const;

        void ApplyEquipCooldown(Item* pItem);
        void SetAmmo(uint32 item);
        void RemoveAmmo();
        std::pair<float, float> GetAmmoDPS() const { return { m_ammoDPSMin, m_ammoDPSMax}; }
        bool CheckAmmoCompatibility(const ItemPrototype* ammo_proto) const;
        void QuickEquipItem(uint16 pos, Item* pItem);
        void VisualizeItem(uint8 slot, Item* pItem);
        void SetVisibleItemSlot(uint8 slot, Item* pItem);
        Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update)
        {
            return StoreItem(dest, pItem, update);
        }
        void RemoveItem(uint8 bag, uint8 slot, bool update);// see ApplyItemOnStoreSpell notes
        void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
        // in trade, auction, guild bank, mail....
        void MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
        // in trade, guild bank, mail....
        void RemoveItemDependentAurasAndCasts(Item* pItem);
        void DestroyItem(uint8 bag, uint8 slot, bool update);
        void DestroyItemCount(uint32 item, uint32 count, bool update, bool unequip_check = false, bool inBankAlso = false);
        void DestroyItemCount(Item* pItem, uint32& count, bool update);
        void DestroyConjuredItems(bool update);
        void DestroyZoneLimitedItem(bool update, uint32 new_zone);
        void SplitItem(uint16 src, uint16 dst, uint32 count);
        void SwapItem(uint16 src, uint16 dst);
        void AddItemToBuyBackSlot(Item* pItem, uint32 money);
        Item* GetItemFromBuyBackSlot(uint32 slot);
        void RemoveItemFromBuyBackSlot(uint32 slot, bool del);

        void TakeExtendedCost(uint32 extendedCostId, uint32 count);

        uint32 GetMaxKeyringSize() const { return KEYRING_SLOT_END - KEYRING_SLOT_START; }
        void SendEquipError(InventoryResult msg, Item* pItem, Item* pItem2 = nullptr, uint32 itemid = 0) const;
        void SendBuyError(BuyResult msg, Creature* pCreature, uint32 item, uint32 param) const;
        void SendSellError(SellResult msg, Creature* pCreature, ObjectGuid itemGuid, uint32 param) const;
        void AddWeaponProficiency(uint32 newflag) { m_WeaponProficiency |= newflag; }
        void AddArmorProficiency(uint32 newflag) { m_ArmorProficiency |= newflag; }
        uint32 GetWeaponProficiency() const { return m_WeaponProficiency; }
        uint32 GetArmorProficiency() const { return m_ArmorProficiency; }
        bool IsTwoHandUsed() const
        {
            Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            return mainItem && mainItem->GetProto()->InventoryType == INVTYPE_2HWEAPON && !CanTitanGrip();
        }
        bool HasTwoHandWeaponInOneHand() const
        {
            Item* offItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            return offItem && ((mainItem && mainItem->GetProto()->InventoryType == INVTYPE_2HWEAPON) || offItem->GetProto()->InventoryType == INVTYPE_2HWEAPON);
        }
        void SendNewItem(Item* item, uint32 count, bool received, bool created, bool broadcast = false, bool showInChat = true);
        bool BuyItemFromVendorSlot(ObjectGuid vendorGuid, uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot);

        float GetReputationPriceDiscount(Creature const* creature) const;
        float GetReputationPriceDiscount(FactionTemplateEntry const* factionTemplate) const;

        Player* GetTrader() const { return m_trade ? m_trade->GetTrader() : nullptr; }
        TradeData* GetTradeData() const { return m_trade; }
        void TradeCancel(bool sendback);

        void UpdateEnchantTime(uint32 time);
        void UpdateItemDuration(uint32 time, bool realtimeonly = false);
        void AddEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurations(Item* item);
        void RemoveAllEnchantments(EnchantmentSlot slot, bool arena = false);
        void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
        void ApplyEnchantment(Item* item, EnchantmentSlot slot, bool apply, bool apply_dur = true, bool ignore_condition = false);
        void ApplyEnchantment(Item* item, bool apply);
        void SendEnchantmentDurations();
        void BuildEnchantmentsInfoData(WorldPacket& data) const;
        void AddItemDurations(Item* item);
        void RemoveItemDurations(Item* item);
        void SendItemDurations();
        void LoadCorpse();
        void LoadPet();

        uint32 m_stableSlots;

        uint32 GetEquipGearScore(bool withBags = true, bool withBank = false);
        void ResetCachedGearScore() { m_cachedGS = 0; }
        typedef std::vector < uint32/*item level*/ > GearScoreVec;

        /*********************************************************/
        /***                    GOSSIP SYSTEM                  ***/
        /*********************************************************/

        void PrepareGossipMenu(WorldObject* pSource, uint32 menuId = 0, bool forceQuests = false);
        void SendPreparedGossip(WorldObject* pSource);
        void OnGossipSelect(WorldObject* pSource, uint32 gossipListId, uint32 menuId);

        uint32 GetGossipTextId(uint32 menuId, WorldObject* pSource);
        uint32 GetGossipTextId(WorldObject* pSource) const;
        uint32 GetDefaultGossipMenuForSource(WorldObject* pSource) const;

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

        // Return player level when QuestLevel is dynamic (-1)
        uint32 GetQuestLevelForPlayer(Quest const* pQuest) const { return pQuest && (pQuest->GetQuestLevel() > 0) ? (uint32)pQuest->GetQuestLevel() : GetLevel(); }

        void PrepareQuestMenu(ObjectGuid guid) const;
        void SendPreparedQuest(ObjectGuid guid) const;
        bool IsActiveQuest(uint32 quest_id) const;          // can be taken or taken

        // Quest is taken and not yet rewarded
        // if completed_or_not = 0 (or any other value except 1 or 2) - returns true, if quest is taken and doesn't depend if quest is completed or not
        // if completed_or_not = 1 - returns true, if quest is taken but not completed
        // if completed_or_not = 2 - returns true, if quest is taken and already completed
        bool IsCurrentQuest(uint32 quest_id, uint8 completed_or_not = 0) const; // taken and not yet rewarded

        Quest const* GetNextQuest(ObjectGuid guid, Quest const* pQuest) const;
        bool CanSeeStartQuest(Quest const* pQuest) const;
        bool CanTakeQuest(Quest const* pQuest, bool msg) const;
        bool CanAddQuest(Quest const* pQuest, bool msg) const;
        bool CanCompleteQuest(uint32 quest_id) const;
        bool CanCompleteRepeatableQuest(Quest const* pQuest) const;
        bool CanRewardQuest(Quest const* pQuest, bool msg) const;
        bool CanRewardQuest(Quest const* pQuest, uint32 reward, bool msg) const;
        void AddQuest(Quest const* pQuest, Object* questGiver);
        void CompleteQuest(uint32 quest_id);
        void IncompleteQuest(uint32 quest_id);
        void RewardQuest(Quest const* pQuest, uint32 reward, Object* questGiver, bool announce = true);
        bool IsQuestExplored(uint32 quest_id) const;

        void FailQuest(uint32 questId);
        void FailQuest(Quest const* quest);
        void FailQuestForGroup(uint32 questId);
        void FailQuestsOnDeath();
        bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestCondition(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLevel(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLog(bool msg) const;
        bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestBreadcrumbQuest(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestDependentBreadcrumbQuests(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestRace(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestReputation(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestStatus(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestTimed(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestNextChain(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestPrevChain(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestDay(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestWeek(Quest const* qInfo) const;
        bool SatisfyQuestMonth(Quest const* qInfo) const;
        bool CanGiveQuestSourceItemIfNeed(Quest const* pQuest, ItemPosCountVec* dest = nullptr) const;
        void GiveQuestSourceItemIfNeed(Quest const* pQuest);
        bool TakeQuestSourceItem(uint32 quest_id, bool msg);
        bool GetQuestRewardStatus(uint32 quest_id) const;
        QuestStatus GetQuestStatus(uint32 quest_id) const;
        void SetQuestStatus(uint32 quest_id, QuestStatus status);

        void SetDailyQuestStatus(uint32 quest_id);
        void SetWeeklyQuestStatus(uint32 quest_id);
        void SetMonthlyQuestStatus(uint32 quest_id);
        void ResetDailyQuestStatus();
        void ResetWeeklyQuestStatus();
        void ResetMonthlyQuestStatus();

        uint16 FindQuestSlot(uint32 quest_id) const;
        uint32 GetQuestSlotQuestId(uint16 slot) const { return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET); }
        void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0)
        {
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET, quest_id);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET + 1, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SetQuestSlotCounter(uint16 slot, uint8 counter, uint16 count)
        {
            uint64 val = GetUInt64Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET);
            val &= ~((uint64)0xFFFF << (counter * 16));
            val |= ((uint64)count << (counter * 16));
            SetUInt64Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET, val);
        }
        void SetQuestSlotState(uint16 slot, uint32 state) { SetFlag(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state); }
        void RemoveQuestSlotState(uint16 slot, uint32 state) { RemoveFlag(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state); }
        void SetQuestSlotTimer(uint16 slot, uint32 timer) { SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer); }
        void SwapQuestSlot(uint16 slot1, uint16 slot2)
        {
            for (int i = 0; i < MAX_QUEST_OFFSET; ++i)
            {
                uint32 temp1 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot1 + i);
                uint32 temp2 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot2 + i);

                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot1 + i, temp2);
                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot2 + i, temp1);
            }
        }
        uint32 GetReqKillOrCastCurrentCount(uint32 quest_id, int32 entry) const;
        void AreaExploredOrEventHappens(uint32 questId);
        void ItemAddedQuestCheck(uint32 entry, uint32 count);
        void ItemRemovedQuestCheck(uint32 entry, uint32 count);
        void KilledMonster(CreatureInfo const* cInfo, Creature const* creature);
        void KilledMonsterCredit(uint32 entry, ObjectGuid guid = ObjectGuid());
        void KilledPlayerCredit(uint16 count = 1);
        void KilledPlayerCreditForQuest(uint16 count, Quest const* quest);
        void CastedCreatureOrGO(uint32 entry, ObjectGuid guid, uint32 spell_id, bool original_caster = true);
        void TalkedToCreature(uint32 entry, ObjectGuid guid);
        void MoneyChanged(uint32 count);
        void ReputationChanged(FactionEntry const* factionEntry);
        bool HasQuestForItem(uint32 itemid) const;
        bool HasQuestForGO(int32 GOId) const;
        void UpdateForQuestWorldObjects();
        bool CanShareQuest(uint32 quest_id) const;

        void SendQuestCompleteEvent(uint32 quest_id) const;
        void SendQuestReward(Quest const* pQuest, uint32 XP, uint32 honor) const;
        void SendQuestFailed(uint32 quest_id, InventoryResult reason = EQUIP_ERR_OK) const;
        void SendQuestTimerFailed(uint32 quest_id) const;
        void SendCanTakeQuestResponse(uint32 msg) const;
        void SendQuestConfirmAccept(Quest const* pQuest, Player* pReceiver) const;
        void SendPushToPartyResponse(Player* pPlayer, uint32 msg) const;
        void SendQuestUpdateAddItem(Quest const* pQuest, uint32 item_idx, uint32 current, uint32 count);
        void SendQuestUpdateAddCreatureOrGo(Quest const* pQuest, ObjectGuid guid, uint32 creatureOrGO_idx, uint32 count);
        void SendQuestUpdateAddPlayer(Quest const* quest, uint32 count);
        void SendQuestGiverStatusMultiple() const;

        ObjectGuid GetDividerGuid() const { return m_dividerGuid; }
        void SetDividerGuid(ObjectGuid guid) { m_dividerGuid = guid; }
        void ClearDividerGuid() { m_dividerGuid.Clear(); }

        uint32 GetInGameTime() const { return m_ingametime; }

        void SetInGameTime(uint32 time) { m_ingametime = time; }

        void AddTimedQuest(uint32 quest_id) { m_timedquests.insert(quest_id); }
        void RemoveTimedQuest(uint32 quest_id) { m_timedquests.erase(quest_id); }

#ifdef BUILD_PLAYERBOT
        PlayerTalentMap GetTalents(uint8 spec) { return m_talents[spec]; }
        void chompAndTrim(std::string& str);
        bool getNextQuestId(const std::string& pString, unsigned int& pStartPos, unsigned int& pId);
        void skill(std::list<uint32>& m_spellsToLearn);
        void MakeTalentGlyphLink(std::ostringstream& out);
        bool requiredQuests(const char* pQuestIdString);
        PlayerMails::reverse_iterator GetMailRBegin() { return m_mail.rbegin();}
        PlayerMails::reverse_iterator GetMailREnd() { return m_mail.rend();}
        void UpdateMail();
        uint32 GetSpec();
#endif

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

        bool LoadFromDB(ObjectGuid guid, SqlQueryHolder* holder);

        static uint32 GetZoneIdFromDB(ObjectGuid guid);
        static uint32 GetLevelFromDB(ObjectGuid guid);
        static bool   LoadPositionFromDB(ObjectGuid guid, uint32& mapid, float& x, float& y, float& z, float& o, bool& in_flight);

        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/

        void SaveToDB();
        void SaveInventoryAndGoldToDB();                    // fast save function for item/money cheating preventing
        void SaveGoldToDB() const;
        static void SetUInt32ValueInArray(Tokens& tokens, uint16 index, uint32 value);
        static void Customize(ObjectGuid guid, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair);
        static void SavePositionInDB(ObjectGuid guid, uint32 mapid, float x, float y, float z, float o, uint32 zone);

        static void DeleteFromDB(ObjectGuid playerguid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
        static void DeleteOldCharacters();
        static void DeleteOldCharacters(uint32 keepDays);

        bool m_mailsUpdated;

        void SendPetTameFailure(PetTameFailureReason reason) const;

        void SetBindPoint(ObjectGuid guid) const;
        void SendTalentWipeConfirm(ObjectGuid guid) const;
        void RewardRage(uint32 damage, uint32 weaponSpeedHitFactor, bool attacker);
        void SendPetSkillWipeConfirm() const;
        void RegenerateAll(uint32 diff = REGEN_TIME_FULL);
        void Regenerate(Powers power, uint32 diff);
        void RegenerateHealth(uint32 diff);

        uint32 GetMoney() const { return GetUInt32Value(PLAYER_FIELD_COINAGE); }
        void ModifyMoney(int32 d)
        {
            if (d < 0)
                SetMoney(GetMoney() > uint32(-d) ? GetMoney() + d : 0);
            else
                SetMoney(GetMoney() < uint32(MAX_MONEY_AMOUNT - d) ? GetMoney() + d : MAX_MONEY_AMOUNT);

            // "At Gold Limit"
            if (GetMoney() >= MAX_MONEY_AMOUNT)
                SendEquipError(EQUIP_ERR_TOO_MUCH_GOLD, nullptr, nullptr);
        }
        void SetMoney(uint32 value)
        {
            SetUInt32Value(PLAYER_FIELD_COINAGE, value);
            MoneyChanged(value);
            UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED);
        }

        QuestStatusMap& getQuestStatusMap() { return mQuestStatus; };
        const QuestStatusMap& getQuestStatusMap() const { return mQuestStatus; };

        ObjectGuid const& GetSelectionGuid() const override { return m_curSelectionGuid; }
        void SetSelectionGuid(ObjectGuid guid) override { m_curSelectionGuid = guid; SetTargetGuid(guid); }
        void ClearSelectionGuid();

        void SendComboPoints() const;

        void SendMailResult(uint32 mailId, MailResponseType mailAction, MailResponseResult mailError, uint32 equipError = 0, uint32 item_guid = 0, uint32 item_count = 0) const;
        void SendNewMail() const;
        void UpdateNextMailTimeAndUnreads();
        void AddNewMailDeliverTime(time_t deliver_time);

        void RemoveMail(uint32 id);

        void AddMail(Mail* mail) { m_mail.push_front(mail);}// for call from WorldSession::SendMailTo
        size_t GetMailSize() const { return m_mail.size(); }
        Mail* GetMail(uint32 id);

        void SendItemRetrievalMail(uint32 itemEntry, uint32 count); // Item retrieval mails sent by The Postmaster (34337), used in multiple places.

        PlayerMails::iterator GetMailBegin() { return m_mail.begin();}
        PlayerMails::iterator GetMailEnd() { return m_mail.end();}

        /*********************************************************/
        /*** MAILED ITEMS SYSTEM ***/
        /*********************************************************/

        uint8 unReadMails;
        time_t m_nextMailDelivereTime;

        typedef std::unordered_map<uint32, Item*> ItemMap;

        ItemMap mMitems;                                    // template defined in objectmgr.cpp

        Item* GetMItem(uint32 id)
        {
            ItemMap::const_iterator itr = mMitems.find(id);
            return itr != mMitems.end() ? itr->second : nullptr;
        }

        void AddMItem(Item* it)
        {
            MANGOS_ASSERT(it);
            // ASSERT deleted, because items can be added before loading
            mMitems[it->GetGUIDLow()] = it;
        }

        bool RemoveMItem(uint32 id)
        {
            return mMitems.erase(id) ? true : false;
        }

        void PetSpellInitialize() const;
        void SendPetGUIDs() const;
        void PossessSpellInitialize() const;
        void VehicleSpellInitialize() const;
        void CharmSpellInitialize() const;
        void RemovePetActionBar() const;
        Unit* GetFirstControlled() const;
        std::pair<float, float> RequestFollowData(ObjectGuid guid);
        void RelinquishFollowData(ObjectGuid guid);

        bool HasSpell(uint32 spell) const override;
        bool HasActiveSpell(uint32 spell) const;            // show in spellbook
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell, uint32 reqLevel) const;
        bool IsSpellFitByClassAndRace(uint32 spell_id, uint32* pReqlevel = nullptr) const;
        bool IsNeedCastPassiveLikeSpellAtLearn(SpellEntry const* spellInfo) const;
        bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex index, bool castOnSelf) const override;

        void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask) const;
        void SendInitialSpells() const;
        void SendUnlearnSpells() const;
        void SendSupercededSpell(uint32 oldSpell, uint32 newSpell) const;
        void SendRemovedSpell(uint32 spellId) const;
        bool addSpell(uint32 spell_id, bool active, bool learning, bool dependent, bool disabled);
        void learnSpell(uint32 spell_id, bool dependent, bool talent = false);
        void removeSpell(uint32 spell_id, bool disabled = false, bool learn_low_rank = true, bool sendUpdate = true);
        void resetSpells();
        void learnDefaultSpells();
        void learnQuestRewardedSpells();
        void learnQuestRewardedSpells(Quest const* quest);
        void learnSpellHighRank(uint32 spellid);

        uint32 GetFreeTalentPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS1); }
        void SetFreeTalentPoints(uint32 points) { SetUInt32Value(PLAYER_CHARACTER_POINTS1, points); }
        void UpdateFreeTalentPoints(bool resetIfNeed = true);
        bool resetTalents(bool no_cost = false, bool all_specs = false);
        uint32 resetTalentsCost() const;
        void InitTalentForLevel();
        void BuildPlayerTalentsInfoData(WorldPacket& data);
        void BuildPetTalentsInfoData(WorldPacket& data) const;
        void SendTalentsInfoData(bool pet);
        void LearnTalent(uint32 talentId, uint32 talentRank);
        void LearnPetTalent(ObjectGuid petGuid, uint32 talentId, uint32 talentRank);

        uint32 CalculateTalentsPoints() const;

        // Dual Spec
        uint8 GetActiveSpec() const { return m_activeSpec; }
        void SetActiveSpec(uint8 spec) { m_activeSpec = spec; }
        uint8 GetSpecsCount() const { return m_specsCount; }
        void SetSpecsCount(uint8 count) { m_specsCount = count; }
        void ActivateSpec(uint8 specNum);
        void UpdateSpecCount(uint8 count);

        void InitGlyphsForLevel();
        void SetGlyphSlot(uint8 slot, uint32 slottype) { SetUInt32Value(PLAYER_FIELD_GLYPH_SLOTS_1 + slot, slottype); }
        uint32 GetGlyphSlot(uint8 slot) const { return GetUInt32Value(PLAYER_FIELD_GLYPH_SLOTS_1 + slot); }
        void SetGlyph(uint8 slot, uint32 glyph) { m_glyphs[m_activeSpec][slot].SetId(glyph); }
        uint32 GetGlyph(uint8 slot) const { return m_glyphs[m_activeSpec][slot].GetId(); }
        void ApplyGlyph(uint8 slot, bool apply);
        void ApplyGlyphs(bool apply);

        uint32 GetFreePrimaryProfessionPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS2); }
        void SetFreePrimaryProfessions(uint16 profs) { SetUInt32Value(PLAYER_CHARACTER_POINTS2, profs); }
        void InitPrimaryProfessions();

        PlayerSpellMap const& GetSpellMap() const { return m_spells; }
        PlayerSpellMap&       GetSpellMap()       { return m_spells; }

        PlayerTalent const* GetKnownTalentById(int32 talentId) const;
        SpellEntry const* GetKnownTalentRankById(int32 talentId) const;
        Aura* GetKnownTalentRankAuraById(int32 talentId, SpellEffectIndex effIdx);

        void AddSpellMod(SpellModifier* mod, bool apply);
        void SendAllSpellMods(SpellModType modType);
        bool IsAffectedBySpellmod(SpellEntry const* spellInfo, SpellModifier* mod, std::set<SpellModifierPair>* consumedMods);
        template <class T> void ApplySpellMod(uint32 spellId, SpellModOp op, T& basevalue, bool finalUse = true);
        SpellModifier* GetSpellMod(SpellModOp op, uint32 spellId) const;
        void RemoveSpellMods(std::set<SpellModifierPair>& usedAuraCharges);
        void ResetSpellModsDueToCanceledSpell(std::set<SpellModifierPair>& usedAuraCharges);
        void SetSpellClass(uint8 playerClass);
        SpellFamily GetSpellClass() const { return m_spellClassName; } // client function equivalent - says what player can cast

        void SetResurrectRequestData(ObjectGuid guid, uint32 mapId, float X, float Y, float Z, uint32 health, uint32 mana, bool ghoul)
        {
            m_resurrectGuid = guid;
            m_resurrectMap = mapId;
            m_resurrectX = X;
            m_resurrectY = Y;
            m_resurrectZ = Z;
            m_resurrectHealth = health;
            m_resurrectMana = mana;
            m_resurrectToGhoul = ghoul;
        }
        static void QueueOrAddResurrectRequest(Corpse* corpseTarget, Unit* caster, Player* player, SpellEntry const* spellInfo, uint32 damage, SpellEffectIndex effIdx, bool ghoul);
        void AddResurrectRequest(ObjectGuid casterGuid, SpellEntry const* spellInfo, Position position, uint32 mapId, uint32 health, uint32 mana, bool isSpiritHealer, const char* sentName, bool ghoul);
        void SendResurrectRequest(SpellEntry const* spellInfo, bool isSpiritHealer, const char* sentName);
        void ClearResurrectRequestData() { SetResurrectRequestData(ObjectGuid(), 0, 0.0f, 0.0f, 0.0f, 0, 0, false); }
        bool isRessurectRequestedBy(ObjectGuid guid) const { return m_resurrectGuid == guid; }
        bool isRessurectRequested() const { return !m_resurrectGuid.IsEmpty(); }
        void ResurrectUsingRequestDataInit(); // Initializes motion and schedules rest / executes it
        void ResurrectUsingRequestDataFinish(); // Finalizes resurrection

        uint32 getCinematic() const { return m_cinematic; }
        void setCinematic(uint32 cine) { m_cinematic = cine; }

        static bool IsActionButtonDataValid(uint8 button, uint32 action, uint8 type, Player* player, bool msg = true);
        ActionButton* addActionButton(uint8 spec, uint8 button, uint32 action, uint8 type);
        void removeActionButton(uint8 spec, uint8 button);
        void SendInitialActionButtons() const;
        void SendLockActionButtons() const;
        ActionButton const* GetActionButton(uint8 button);

        PvPInfo pvpInfo;
        void UpdatePvP(bool state, bool overriding = false);
        void UpdatePvPContested(bool state, bool overriding = false);

        // forced update needed for on-resurrection event
        void UpdateZone(uint32 newZone, uint32 newArea, bool force = false);
        void UpdateArea(uint32 newArea);
        uint32 GetCachedZoneId() const { return m_zoneUpdateId; }

        void UpdateZoneDependentAuras();
        void UpdateAreaDependentAuras();                    // subzones
        void UpdateZoneDependentPets();

        void UpdateAfkReport(time_t currTime);
        void UpdatePvPFlagTimer(uint32 diff);
        void UpdatePvPContestedFlagTimer(uint32 diff);

        /** todo: -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.. **/
        DuelInfo* duel;
        bool IsInDuelWith(Player const* player) const { return duel && duel->opponent == player && duel->startTime != 0; }
        void UpdateDuelFlag(time_t currTime);
        void CheckDuelDistance(time_t currTime);
        void DuelComplete(DuelCompleteType type);
        void SendDuelCountdown(uint32 counter) const;

        void UninviteFromGroup();
        static void RemoveFromGroup(Group* group, ObjectGuid guid);
        void RemoveFromGroup() { RemoveFromGroup(GetGroup(), GetObjectGuid()); }
        void SendUpdateToOutOfRangeGroupMembers();
        void SetAllowLowLevelRaid(bool allow) { ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_ENABLE_LOW_LEVEL_RAID, allow); }
        bool GetAllowLowLevelRaid() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_ENABLE_LOW_LEVEL_RAID); }

        void SetInGuild(uint32 GuildId) { SetUInt32Value(PLAYER_GUILDID, GuildId); }
        void SetRank(uint32 rankId) { SetUInt32Value(PLAYER_GUILDRANK, rankId); }
        void SetGuildIdInvited(uint32 GuildId) { m_GuildIdInvited = GuildId; }
        uint32 GetGuildId() const { return GetUInt32Value(PLAYER_GUILDID);  }
        static uint32 GetGuildIdFromDB(ObjectGuid guid);
        uint32 GetRank() const { return GetUInt32Value(PLAYER_GUILDRANK); }
        static uint32 GetRankFromDB(ObjectGuid guid);
        int GetGuildIdInvited() const { return m_GuildIdInvited; }
        static void RemovePetitionsAndSigns(ObjectGuid guid, uint32 type);

        // Arena Team
        void SetInArenaTeam(uint32 ArenaTeamId, uint8 slot, ArenaType type)
        {
            SetArenaTeamInfoField(slot, ARENA_TEAM_ID, ArenaTeamId);
            SetArenaTeamInfoField(slot, ARENA_TEAM_TYPE, type);
        }
        void SetArenaTeamInfoField(uint8 slot, ArenaTeamInfoType type, uint32 value)
        {
            SetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + type, value);
        }
        uint32 GetArenaTeamId(uint8 slot) const { return GetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + ARENA_TEAM_ID); }
        uint32 GetArenaPersonalRating(uint8 slot) const { return GetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + ARENA_TEAM_PERSONAL_RATING); }
        static uint32 GetArenaTeamIdFromDB(ObjectGuid guid, ArenaType type);
        void SetArenaTeamIdInvited(uint32 ArenaTeamId) { m_ArenaTeamIdInvited = ArenaTeamId; }
        uint32 GetArenaTeamIdInvited() const { return m_ArenaTeamIdInvited; }
        static void LeaveAllArenaTeams(ObjectGuid guid);

        Difficulty GetDifficulty(bool isRaid) const;
        Difficulty GetDungeonDifficulty() const { return m_dungeonDifficulty; }
        Difficulty GetRaidDifficulty() const { return m_raidDifficulty; }
        void SetDungeonDifficulty(Difficulty dungeon_difficulty) { m_dungeonDifficulty = dungeon_difficulty; }
        void SetRaidDifficulty(Difficulty raid_difficulty) { m_raidDifficulty = raid_difficulty; }

        bool CanEnterNewInstance(uint32 instanceId);
        void AddNewInstanceId(uint32 instanceId);
        void UpdateNewInstanceIdTimers(TimePoint const& now);

        void UpdateClientAuras();
        void SendPetBar();
        void StartCinematic();
        void StopCinematic();
        bool UpdateSkill(uint16 id, uint16 diff);
        bool UpdateSkillPro(uint16 id, int32 Chance, uint16 diff);

        bool UpdateCraftSkill(uint32 spellid);
        bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1);
        bool UpdateFishingSkill();

        float GetHealthBonusFromStamina() const;
        float GetManaBonusFromIntellect() const;

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void ApplyFeralAPBonus(int32 amount, bool apply);
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateShieldBlockValue();
        void UpdateDamagePhysical(WeaponAttackType attType) override;
        void ApplySpellPowerBonus(int32 amount, bool apply);
        void UpdateSpellHealingBonus();
        void UpdateSpellDamageBonus();
        void ApplyRatingMod(CombatRating cr, int32 value, bool apply);
        void UpdateRating(CombatRating cr);
        void UpdateAllRatings();

        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, float& min_damage, float& max_damage, uint8 index = 0);

        void UpdateDefenseBonusesMod();
        float GetMeleeCritFromAgility() const;
        float GetDodgeFromAgility(float amount) const;
        float GetSpellCritFromIntellect() const;
        float GetRatingMultiplier(CombatRating cr) const;
        float GetRatingBonusValue(CombatRating cr) const;
        uint32 GetBaseSpellPowerBonus() const { return m_baseSpellPower; }

        void UpdateBlockPercentage();
        void UpdateCritPercentage(WeaponAttackType attType);
        void UpdateAllCritPercentages();
        void UpdateParryPercentage();
        void UpdateDodgePercentage();
        void UpdateMeleeHitChances();
        void UpdateRangedHitChances();
        void UpdateSpellHitChances();

        void UpdateAllSpellCritChances();
        void UpdateSpellCritChance(uint32 school);
        void UpdateExpertise(WeaponAttackType attType);
        void UpdateArmorPenetration();
        void ApplyManaRegenBonus(int32 amount, bool apply);
        void UpdateManaRegen();
        void UpdateEnergyRegen();

        ObjectGuid const& GetLootGuid() const { return m_lootGuid; }
        void SetLootGuid(ObjectGuid const& guid) { m_lootGuid = guid; }

        void RemovedInsignia(Player* looterPlr);

        WorldSession* GetSession() const { return m_session; }
        void SetSession(WorldSession* s) { m_session = s; }

        void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const override;
        void DestroyForPlayer(Player* target, bool anim = false) const override;
        void SendLogXPGain(uint32 GivenXP, Unit* victim, uint32 RestXP, bool recruitAFriend, float groupRate) const;

        uint8 LastSwingErrorMsg() const { return m_swingErrorMsg; }
        void SwingErrorMsg(uint8 val) { m_swingErrorMsg = val; }

        // notifiers
        void SendAttackSwingCantAttack() const;
        void SendAttackSwingCancelAttack() const;
        void SendAttackSwingDeadTarget() const;
        void SendAttackSwingNotInRange() const;
        void SendAttackSwingBadFacingAttack() const;
        void SendAutoRepeatCancel(Unit* target) const;
        void SendFeignDeathResisted() const;
        void SendExplorationExperience(uint32 Area, uint32 Experience) const;

        void SendDungeonDifficulty(bool IsInGroup) const;
        void SendRaidDifficulty(bool IsInGroup, uint32 difficulty) const;
        void ResetInstances(InstanceResetMethod method, bool isRaid);
        void SendResetInstanceSuccess(uint32 MapId) const;
        void SendResetInstanceFailed(uint32 reason, uint32 MapId) const;
        void SendResetFailedNotify(uint32 mapid) const;

        bool SetPosition(float x, float y, float z, float orientation, bool teleport = false);
        void UpdateTerainEnvironmentFlags(Map* m, float x, float y, float z);

        void SendMessageToSet(WorldPacket const& data, bool self) const override;// overwrite Object::SendMessageToSet
        void SendMessageToSetInRange(WorldPacket const& data, float dist, bool self) const override;
        // overwrite Object::SendMessageToSetInRange
        void SendMessageToSetInRange(WorldPacket const& data, float dist, bool self, bool own_team_only) const;
        void SendMessageToAllWhoSeeMe(WorldPacket const& data, bool self) const override;

        Corpse* GetCorpse() const;
        void SpawnCorpseBones();
        Corpse* CreateCorpse();
        void KillPlayer();
        uint32 GetResurrectionSpellId() const;
        void ResurrectPlayer(float restore_percent, bool applySickness = false);
        void BuildPlayerRepop();
        void RepopAtGraveyard();
        std::pair<bool, AreaTrigger const*> CheckAndRevivePlayerOnDungeonEnter(MapEntry const* targetMapEntry, uint32 targetMapId);

        void DurabilityLossAll(double percent, bool inventory);
        void DurabilityLoss(Item* item, double percent);
        void DurabilityPointsLossAll(int32 points, bool inventory);
        void DurabilityPointsLoss(Item* item, int32 points);
        void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
        uint32 DurabilityRepairAll(bool cost, float discountMod, bool guildBank);
        uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod, bool guildBank);

        void JoinedChannel(Channel* c);
        void LeftChannel(Channel* c);
        void CleanupChannels();
        void UpdateLocalChannels(uint32 newZone);
        void LeaveLFGChannel();

        void UpdateDefense();
        void UpdateWeaponSkill(WeaponAttackType attType);
        void UpdateCombatSkills(Unit* pVictim, WeaponAttackType attType, bool defence);
        uint16 GetWeaponSkillIdForAttack(WeaponAttackType attType) const;

        SkillRaceClassInfoEntry const* GetSkillInfo(uint16 id, std::function<bool (SkillRaceClassInfoEntry const&)> filterfunc = nullptr) const;
        bool HasSkill(uint16 id) const;
        void SetSkill(SkillStatusMap::iterator itr, uint16 value, uint16 max, uint16 step = 0);
        void SetSkill(uint16 id, uint16 value, uint16 max, uint16 step = 0);
        uint16 GetSkill(uint16 id, bool bonusPerm, bool bonusTemp, bool max = false) const;
        inline uint16 GetSkillValue(uint16 id) const { return GetSkill(id, true, true); }           // skill value + perm. bonus + temp bonus
        inline uint16 GetSkillValueBase(uint16 id) const { return GetSkill(id, true, false); }      // skill value + perm. bonus
        inline uint16 GetSkillValuePure(uint16 id) const { return GetSkill(id, false, false); }     // skill value
        inline uint16 GetSkillMax(uint16 id) const { return GetSkill(id, true, true, true); }       // skill max + perm. bonus + temp bonus
        inline uint16 GetSkillMaxPure(uint16 id) const { return GetSkill(id, false, false, true); } // skill max
        void SetSkillStep(uint16 id, uint16 step);
        uint16 GetSkillStep(uint16 id) const;
        bool ModifySkillBonus(uint16 id, int16 diff, bool permanent = false);
        int16 GetSkillBonus(uint16 id, bool permanent = false) const;
        inline int16 GetSkillBonusPermanent(uint16 id) const { return GetSkillBonus(id, true); }    // skill perm. bonus
        inline int16 GetSkillBonusTemporary(uint16 id) const { return GetSkillBonus(id); }          // skill temp bonus
        void UpdateSkillsForLevel(bool maximize = false);
        void UpdateSkillTrainedSpells(uint16 id, uint16 currVal);                                   // learns/unlearns spells dependent on a skill
        void UpdateSpellTrainedSkills(uint32 spellId, bool apply);                                  // learns/unlearns skills dependent on a spell
        void LearnDefaultSkills();

        virtual uint32 GetSpellRank(SpellEntry const* spellInfo) override;

        bool IsLaunched() const { return m_launched; }
        void SetLaunched(bool apply) { m_launched = apply; }

        WorldLocation& GetTeleportDest() { return m_teleport_dest; }
        bool IsBeingTeleported() const { return m_semaphoreTeleport_Near || m_semaphoreTeleport_Far; }
        bool IsBeingTeleportedNear() const { return m_semaphoreTeleport_Near; }
        bool IsBeingTeleportedFar() const { return m_semaphoreTeleport_Far; }
        void SetSemaphoreTeleportNear(bool semphsetting);
        void SetSemaphoreTeleportFar(bool semphsetting);
        void ProcessDelayedOperations();
        void SetDelayedZoneUpdate(bool state, uint32 newZone) { m_needsZoneUpdate = state; m_newZone = newZone; }

        void CheckAreaExploreAndOutdoor();

        static Team TeamForRace(uint8 race);
        Team GetTeam() const { return m_team; }
        static uint32 getFactionForRace(uint8 race);
        void setFactionForRace(uint8 race);

        void InitDisplayIds();

        bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
        void RewardSinglePlayerAtKill(Unit* pVictim);
        void RewardPlayerAndGroupAtEventCredit(uint32 creature_id, WorldObject* pRewardSource);
        void RewardPlayerAndGroupAtCast(WorldObject* pRewardSource, uint32 spellid = 0);
        void RewardPlayerAndGroupAtEventExplored(uint32 questId, WorldObject const* pEventObject);
        bool isHonorOrXPTarget(Unit* pVictim) const;

        template<typename T>
        bool CheckForGroup(T functor) const
        {
            if (Group const* group = GetGroup())
            {
                for (GroupReference const* ref = group->GetFirstMember(); ref != nullptr; ref = ref->next())
                {
                    Player const* member = ref->getSource();
                    if (member && functor(member))
                        return true;
                }
            }
            else
            {
                if (functor(this))
                    return true;
            }
            return false;
        }

        ReputationMgr&       GetReputationMgr()       { return m_reputationMgr; }
        ReputationMgr const& GetReputationMgr() const { return m_reputationMgr; }
        ReputationRank GetReputationRank(uint32 faction_id) const;
        void RewardReputation(Creature* victim, float rate);
        void RewardReputation(Quest const* pQuest);
        int32 CalculateReputationGain(ReputationSource source, int32 rep, int32 maxRep, int32 faction, uint32 creatureOrQuestLevel = 0, bool noAuraBonus = false) const;

        /*********************************************************/
        /***                  PVP SYSTEM                       ***/
        /*********************************************************/
        void UpdateArenaFields();
        void UpdateHonorFields();
        bool RewardHonor(Unit* uVictim, uint32 groupsize, float honor = -1);
        uint32 GetHonorPoints() const { return GetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY); }
        uint32 GetArenaPoints() const { return GetUInt32Value(PLAYER_FIELD_ARENA_CURRENCY); }
        void SetHonorPoints(uint32 value);
        void SetArenaPoints(uint32 value);
        void ModifyHonorPoints(int32 value);
        void ModifyArenaPoints(int32 value);

        uint8 GetHighestPvPRankIndex() const;
        uint32 GetMaxPersonalArenaRatingRequirement(uint32 minarenaslot) const;

        // End of PvP System

        void SetDrunkValue(uint8 newDrunkValue, uint32 itemId = 0);
        uint8 GetDrunkValue() const { return GetByteValue(PLAYER_BYTES_3, 1); }
        static DrunkenState GetDrunkenstateByValue(uint8 value);

        uint32 GetDeathTimer() const { return m_deathTimer; }
        void ResetDeathTimer();
        uint32 GetCorpseReclaimDelay(bool pvp) const;
        void UpdateCorpseReclaimDelay();
        void SendCorpseReclaimDelay(bool load = false) const;

        uint32 GetShieldBlockValue() const override;        // overwrite Unit version (virtual)
        bool CanTitanGrip() const { return m_canTitanGrip; }
        void SetCanTitanGrip(bool value) { m_canTitanGrip = value; }
        bool CanTameExoticPets() const { return IsGameMaster() || HasAuraType(SPELL_AURA_ALLOW_TAME_PET_TYPE); }

        void SetRegularAttackTime();
        void SetBaseModValue(BaseModGroup modGroup, BaseModType modType, float value) { m_auraBaseMod[modGroup][modType] = value; }
        void HandleBaseModValue(BaseModGroup modGroup, BaseModType modType, float amount, bool apply);
        float GetBaseModValue(BaseModGroup modGroup, BaseModType modType) const;
        float GetTotalBaseModValue(BaseModGroup modGroup) const;
        float GetTotalPercentageModValue(BaseModGroup modGroup) const { return m_auraBaseMod[modGroup][FLAT_MOD] + m_auraBaseMod[modGroup][PCT_MOD]; }
        void _ApplyAllStatBonuses();
        void _RemoveAllStatBonuses();
        void SetEnchantmentModifier(uint32 value, WeaponAttackType attType, bool apply);
        uint32 GetEnchantmentModifier(WeaponAttackType attType);
        float GetArmorPenetrationPct() const { return m_armorPenetrationPct; }
        int32 GetSpellPenetrationItemMod() const { return m_spellPenetrationItemMod; }

        void _ApplyWeaponDependentAuraMods(Item* item, WeaponAttackType attackType, bool apply);
        void _ApplyWeaponDependentAuraCritMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);
        void _ApplyWeaponDependentAuraDamageMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);

        void _ApplyItemMods(Item* item, uint8 slot, bool apply);
        void _RemoveAllItemMods();
        void _ApplyAllItemMods();
        void _ApplyAllLevelScaleItemMods(bool apply);
        void _ApplyItemBonuses(ItemPrototype const* proto, uint8 slot, bool apply, bool only_level_scale = false);
        void _ApplyAmmoBonuses();
        bool EnchantmentFitsRequirements(uint32 enchantmentcondition, int8 slot) const;
        void ToggleMetaGemsActive(uint8 exceptslot, bool apply);
        void CorrectMetaGemEnchants(uint8 exceptslot, bool apply);
        void InitDataForForm(bool reapplyMods = false);

        void ApplyItemEquipSpell(Item* item, bool apply, bool form_change = false);
        void ApplyEquipSpell(SpellEntry const* spellInfo, Item* item, bool apply, bool form_change = false);
        void UpdateEquipSpellsAtFormChange();
        void CastItemCombatSpell(Unit* Target, WeaponAttackType attType, bool spellProc = false);
        void CastItemUseSpell(Item* item, SpellCastTargets& targets, uint8 cast_count, uint32 glyphIndex, uint32 spellId);

        void ApplyItemOnStoreSpell(Item* item, bool apply);
        void DestroyItemWithOnStoreSpell(Item* item, uint32 spellId);

        void SendEquipmentSetList();
        void SetEquipmentSet(uint32 index, EquipmentSet eqset);
        void DeleteEquipmentSet(uint64 setGuid);

        void SendInitWorldStates(uint32 zoneid, uint32 areaid) const;
        void SendUpdateWorldState(uint32 Field, uint32 Value) const;
        void SendDirectMessage(WorldPacket const& data) const;
        void FillBGWeekendWorldStates(WorldPacket& data, uint32& count) const;

        void SendAurasForTarget(Unit* target) const;

        PlayerMenu* GetPlayerMenu() const { return m_playerMenu.get(); }

        ItemSetEffect* GetItemSetEffect(uint32 setId);
        ItemSetEffect* AddItemSetEffect(uint32 setId);
        void RemoveItemSetEffect(uint32 setId);

        /*********************************************************/
        /***               BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        bool InBattleGround()       const                { return m_bgData.bgInstanceID != 0; }
        bool InArena()              const;
        uint32 GetBattleGroundId()  const                { return m_bgData.bgInstanceID; }
        BattleGroundTypeId GetBattleGroundTypeId() const { return m_bgData.bgTypeID; }
        BattleGround* GetBattleGround() const;

        bool InBattleGroundQueue() const
        {
            for (auto i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId != BATTLEGROUND_QUEUE_NONE)
                    return true;
            return false;
        }

        BattleGroundQueueTypeId GetBattleGroundQueueTypeId(uint32 index) const { return m_bgBattleGroundQueueID[index].bgQueueTypeId; }
        uint32 GetBattleGroundQueueIndex(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (int i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == bgQueueTypeId)
                    return i;
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool IsInvitedForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (auto i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    return i.invitedToInstance != 0;
            return false;
        }
        bool InBattleGroundQueueForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            return GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        void SetBattleGroundId(uint32 val, BattleGroundTypeId bgTypeId)
        {
            m_bgData.bgInstanceID = val;
            m_bgData.bgTypeID = bgTypeId;
            m_bgData.m_needSave = true;
        }
        uint32 AddBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (int i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == BATTLEGROUND_QUEUE_NONE || m_bgBattleGroundQueueID[i].bgQueueTypeId == val)
                {
                    m_bgBattleGroundQueueID[i].bgQueueTypeId = val;
                    m_bgBattleGroundQueueID[i].invitedToInstance = 0;
                    return i;
                }
            }
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool HasFreeBattleGroundQueueId() const
        {
            for (auto i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == BATTLEGROUND_QUEUE_NONE)
                    return true;
            return false;
        }

        void RemoveBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (auto& i : m_bgBattleGroundQueueID)
            {
                if (i.bgQueueTypeId == val)
                {
                    i.bgQueueTypeId = BATTLEGROUND_QUEUE_NONE;
                    i.invitedToInstance = 0;
                    return;
                }
            }
        }

        void SetInviteForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId, uint32 instanceId)
        {
            for (auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    i.invitedToInstance = instanceId;
        }

        bool IsInvitedForBattleGroundInstance(uint32 instanceId) const
        {
            for (auto i : m_bgBattleGroundQueueID)
                if (i.invitedToInstance == instanceId)
                    return true;
            return false;
        }

        WorldLocation const& GetBattleGroundEntryPoint() const { return m_bgData.joinPos; }
        void SetBattleGroundEntryPoint();

        void SetBGTeam(Team team) { m_bgData.bgTeam = team; m_bgData.m_needSave = true; }
        Team GetBGTeam() const { return m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam(); }

        void LeaveBattleground(bool teleportToEntryPoint = true);
        bool CanJoinToBattleground() const;
        bool CanReportAfkDueToLimit();
        void ReportedAfkBy(Player* reporter);
        void ClearAfkReports() { m_bgData.bgAfkReporter.clear(); }

        bool GetBGAccessByLevel(BattleGroundTypeId bgTypeId) const;
        bool CanUseBattleGroundObject() const;
        bool isTotalImmune() const;

        // set and get random battleground winner status
        bool HasWonRandomBattleground() { return m_hasWonRandomBattleground; }
        void SetRandomBattlegroundWinner(bool isWinner);

        // returns true if the player is in active state for capture point capturing
        bool CanUseCapturePoint() const;

        /*********************************************************/
        /***                    REST SYSTEM                    ***/
        /*********************************************************/

        bool isRested() const { return GetRestTime() >= 10 * IN_MILLISECONDS; }
        uint32 GetXPRestBonus(uint32 xp);
        uint32 GetRestTime() const { return m_restTime; }
        void SetRestTime(uint32 v) { m_restTime = v; }

        /*********************************************************/
        /***              ENVIROMENTAL SYSTEM                  ***/
        /*********************************************************/

        bool IsUnderwater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_UNDERWATER); }
        bool IsInWater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_WATER); }
        inline bool IsInMagma() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_MAGMA); }
        inline bool IsInSlime() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_SLIME); }
        inline bool IsInHighSea() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_SEA); }
        inline bool IsInHighLiquid() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_LIQUID); }

        inline uint32 GetWaterBreathingInterval() const;
        void SetWaterBreathingIntervalMultiplier(float multiplier);

        void SendMirrorTimers(bool forced = false);

        uint32 EnvironmentalDamage(EnviromentalDamage type, uint32 damage);

        void SendWeatherUpdate(uint32 weatherId, uint32 weatherGrade) const;
        void SendOverrideLightUpdate(uint32 lightId, uint32 fadeInTime) const;

        /*********************************************************/
        /***               FLOOD FILTER SYSTEM                 ***/
        /*********************************************************/

        void UpdateSpeakTime();
        bool CanSpeak() const;

        /*********************************************************/
        /***                 VARIOUS SYSTEMS                   ***/
        /*********************************************************/
        bool HasMovementFlag(MovementFlags f) const;        // for script access to m_movementInfo.HasMovementFlag
        void UpdateFallInformationIfNeed(MovementInfo const& minfo, uint16 opcode);
        void SetFallInformation(uint32 time, float z)
        {
            m_lastFallTime = time;
            m_lastFallZ = z;
        }
        void HandleFall(MovementInfo const& movementInfo);

        bool isMovingOrTurning() const { return m_movementInfo.HasMovementFlag(movementOrTurningFlagsMask); }

        bool CanSwim() const override { return true; }
        bool CanFly() const override { return m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING); }
        bool CanWalk() const override { return true; }
        bool IsFreeFlying() const { return HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED) || HasAuraType(SPELL_AURA_FLY); }
        bool IsSwimming() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING); }
        bool CanStartFlyInArea(uint32 mapid, uint32 zone, uint32 area, bool ignoreColdWeather) const;

        void UpdateClientControl(Unit const* target, bool enabled, bool forced = false) const;

        void SetMover(Unit* target) { m_mover = target ? target : this; }
        Unit* GetMover() const { return m_mover; }
        bool IsSelfMover() const { return m_mover == this; }// normal case for player not controlling other unit

        ObjectGuid const& GetFarSightGuid() const { return GetGuidValue(PLAYER_FARSIGHT); }

        uint32 GetSaveTimer() const { return m_nextSave; }
        void   SetSaveTimer(uint32 timer) { m_nextSave = timer; }

        // Recall position
        uint32 m_recallMap;
        float  m_recallX;
        float  m_recallY;
        float  m_recallZ;
        float  m_recallO;
        void   SaveRecallPosition();

        void SetHomebindToLocation(WorldLocation const& loc, uint32 area_id);
        void GetHomebindLocation(float& x, float& y, float& z, uint32& mapId) { x = m_homebindX; y = m_homebindY; z = m_homebindZ; mapId = m_homebindMapId; }
        void RelocateToHomebind() { SetLocationMapId(m_homebindMapId); Relocate(m_homebindX, m_homebindY, m_homebindZ); }
        bool TeleportToHomebind(uint32 options = 0) { return TeleportTo(m_homebindMapId, m_homebindX, m_homebindY, m_homebindZ, GetOrientation(), options); }

        Object* GetObjectByTypeMask(ObjectGuid guid, TypeMask typemask);

        // currently visible objects at player client
        bool HasAtClient(WorldObject const* u) { return u == this || m_clientGUIDs.find(u->GetObjectGuid()) != m_clientGUIDs.end(); }
        void AddAtClient(WorldObject* target);
        void RemoveAtClient(WorldObject* target);
        GuidSet& GetClientGuids() { return m_clientGUIDs; }

        bool IsVisibleInGridForPlayer(Player* pl) const override;
        bool IsVisibleGloballyFor(Player* u) const;

        void UpdateVisibilityOf(WorldObject const* viewPoint, WorldObject* target);

        template<class T>
        void UpdateVisibilityOf(WorldObject const* viewPoint, T* target, UpdateData& data, WorldObjectSet& visibleNow);

        void BeforeVisibilityDestroy(Creature* creature);

        void ResetMap() override;

        // Stealth detection system
        void HandleStealthedUnitsDetection();

        Camera& GetCamera() { return m_camera; }

        virtual void SetPhaseMask(uint32 newPhaseMask, bool update) override;// overwrite Unit::SetPhaseMask

        uint8 m_forced_speed_changes[MAX_MOVE_TYPE];

        bool HasAtLoginFlag(AtLoginFlags f) const { return (m_atLoginFlags & f) != 0; }
        void SetAtLoginFlag(AtLoginFlags f) { m_atLoginFlags |= f; }
        void RemoveAtLoginFlag(AtLoginFlags f, bool in_db_also = false);
        static bool ValidateAppearance(uint8 race, uint8 class_, uint8 gender, uint8 hairID, uint8 hairColor, uint8 faceID, uint8 facialHair, uint8 skinColor, bool create = false);

        // Temporarily removed pet cache
        uint32 GetTemporaryUnsummonedPetNumber() const { return m_temporaryUnsummonedPetNumber; }
        void SetTemporaryUnsummonedPetNumber(uint32 petnumber) { m_temporaryUnsummonedPetNumber = petnumber; }
        void UnsummonPetTemporaryIfAny();
        void UnsummonPetIfAny();
        void ResummonPetTemporaryUnSummonedIfAny();
        bool IsPetNeedBeTemporaryUnsummoned(Pet* pet) const;
        uint32 GetBGPetSpell() const { return m_BGPetSpell; }
        void SetBGPetSpell(uint32 petSpell) { m_BGPetSpell = petSpell; }
        void AddControllable(Unit* controlled);
        void RemoveControllable(Unit* controlled);
        GuidSet const& GetControlled() { return m_controlled; }

        void SendCinematicStart(uint32 CinematicSequenceId);
        void SendMovieStart(uint32 MovieId) const;

        /*********************************************************/
        /***                 INSTANCE SYSTEM                   ***/
        /*********************************************************/

        typedef std::unordered_map < uint32 /*mapId*/, InstancePlayerBind > BoundInstancesMap;

        void UpdateHomebindTime(uint32 time);

        uint32 m_HomebindTimer;
        bool m_InstanceValid;
        // permanent binds and solo binds by difficulty
        BoundInstancesMap m_boundInstances[MAX_DIFFICULTY];
        InstancePlayerBind* GetBoundInstance(uint32 mapid, Difficulty difficulty, bool withExpired = false);
        BoundInstancesMap& GetBoundInstances(Difficulty difficulty) { return m_boundInstances[difficulty]; }
        void UnbindInstance(uint32 mapid, Difficulty difficulty, bool unload = false);
        void UnbindInstance(BoundInstancesMap::iterator& itr, Difficulty difficulty, bool unload = false);
        InstancePlayerBind* BindToInstance(DungeonPersistentState* state, bool permanent, BindExtensionState extendState = EXTEND_STATE_NORMAL, bool load = false);
        void BindToInstance();
        void SetPendingBind(uint32 mapId, uint32 instanceId, uint32 bindTimer);
        bool HasPendingBind() const { return m_pendingBindId > 0; }
        void SendRaidInfo();
        void SendSavedInstances();
        static void ConvertInstancesToGroup(Player* player, Group* group = nullptr, ObjectGuid player_guid = ObjectGuid());
        DungeonPersistentState* GetBoundInstanceSaveForSelfOrGroup(uint32 mapid);

        AreaLockStatus GetAreaTriggerLockStatus(AreaTrigger const* at, Difficulty difficulty, uint32& miscRequirement, bool forceAllChecks = false);
        void SendTransferAbortedByLockStatus(MapEntry const* mapEntry, AreaLockStatus lockStatus, uint32 miscRequirement = 0) const;

        /*********************************************************/
        /***                   GROUP SYSTEM                    ***/
        /*********************************************************/

        Group* GetGroupInvite() const { return m_groupInvite; }
        void SetGroupInvite(Group* group) { m_groupInvite = group; }
        Group* GetGroup() { return m_group.getTarget(); }
        const Group* GetGroup() const { return (const Group*)m_group.getTarget(); }
        GroupReference& GetGroupRef() { return m_group; }
        void SetGroup(Group* group, int8 subgroup = -1);
        uint8 GetSubGroup() const { return m_group.getSubGroup(); }
        uint32 GetGroupUpdateFlag() const { return m_groupUpdateMask; }
        void SetGroupUpdateFlag(uint32 flag) { m_groupUpdateMask |= flag; }
        Player* GetNextRaidMemberWithLowestLifePercentage(float radius, AuraType noAuraType);
        PartyResult CanUninviteFromGroup() const;
        void UpdateGroupLeaderFlag(const bool remove = false);
        // BattleGround Group System
        void SetBattleRaid(Group* group, int8 subgroup = -1);
        void RemoveFromBattleGroundRaid();
        Group* GetOriginalGroup() const { return m_originalGroup.getTarget(); }
        GroupReference& GetOriginalGroupRef() { return m_originalGroup; }
        uint8 GetOriginalSubGroup() const { return m_originalGroup.getSubGroup(); }
        void SetOriginalGroup(Group* group, int8 subgroup = -1);

        GridReference<Player>& GetGridRef() { return m_gridRef; }
        MapReference& GetMapRef() { return m_mapRef; }

        DeclinedName const* GetDeclinedNames() const { return m_declinedname; }

        // Rune functions, need check  getClass() == CLASS_DEATH_KNIGHT before access
        uint8 GetRunesState() const { return m_runes->runeState; }
        RuneType GetBaseRune(uint8 index) const { return RuneType(m_runes->runes[index].BaseRune); }
        RuneType GetCurrentRune(uint8 index) const { return RuneType(m_runes->runes[index].CurrentRune); }
        uint16 GetRuneCooldown(uint8 index) const { return m_runes->runes[index].Cooldown; }
        uint32 GetRuneBaseCooldown(uint8 index);
        bool IsBaseRuneSlotsOnCooldown(RuneType runeType) const;
        void SetBaseRune(uint8 index, RuneType baseRune) { m_runes->runes[index].BaseRune = baseRune; }
        void SetCurrentRune(uint8 index, RuneType currentRune) { m_runes->runes[index].CurrentRune = currentRune; }
        void SetRuneCooldown(uint8 index, uint16 cooldown) { m_runes->runes[index].Cooldown = cooldown; m_runes->SetRuneState(index, (cooldown == 0) ? true : false); }
        void ConvertRune(uint8 index, RuneType newType);
        bool ActivateRunes(RuneType type, uint32 count);
        void ResyncRunes() const;
        void AddRunePower(uint8 index) const;
        void InitRunes();
        void SetRuneConvertAura(uint8 index, Aura const* aura);
        void RemoveRuneConvertAura(uint8 index, Aura const* aura);
        void AddRuneByAuraEffect(uint8 index, RuneType newType, Aura const* aura);
        void RemoveRunesByAura(Aura const* aura);
        void RestoreBaseRune(uint8 index);

        AchievementMgr const& GetAchievementMgr() const { return m_achievementMgr; }
        AchievementMgr& GetAchievementMgr() { return m_achievementMgr; }
        void UpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscvalue1 = 0, uint32 miscvalue2 = 0, Unit* unit = nullptr, uint32 time = 0);
        void StartTimedAchievementCriteria(AchievementCriteriaTypes type, uint32 timedRequirementId, time_t startTime = 0);

        bool HasTitle(uint32 bitIndex) const;
        bool HasTitle(CharTitlesEntry const* title) const { return HasTitle(title->bit_index); }
        void SetTitle(uint32 titleId, bool lost = false);
        void SetTitle(CharTitlesEntry const* title, bool lost = false, bool send = true);

        virtual UnitAI* AI() override { if (m_charmInfo) return m_charmInfo->GetAI(); return nullptr; }
        virtual CombatData* GetCombatData() override { if (m_charmInfo && m_charmInfo->GetCombatData()) return m_charmInfo->GetCombatData(); return m_combatData; }
        virtual CombatData const* GetCombatData() const override { if (m_charmInfo && m_charmInfo->GetCombatData()) return m_charmInfo->GetCombatData(); return m_combatData; }

        bool canSeeSpellClickOn(Creature const* c) const;

        void SendMessageToPlayer(std::string const& message) const; // debugging purposes

#ifdef BUILD_PLAYERBOT
        // A Player can either have a playerbotMgr (to manage its bots), or have playerbotAI (if it is a bot), or
        // neither. Code that enables bots must create the playerbotMgr and set it using SetPlayerbotMgr.
        void SetPlayerbotAI(PlayerbotAI* ai) { assert(!m_playerbotAI && !m_playerbotMgr); m_playerbotAI = ai; }
        PlayerbotAI* GetPlayerbotAI() { return m_playerbotAI; }
        void SetPlayerbotMgr(PlayerbotMgr* mgr) { assert(!m_playerbotAI && !m_playerbotMgr); m_playerbotMgr = mgr; }
        PlayerbotMgr* GetPlayerbotMgr() { return m_playerbotMgr; }
        void SetBotDeathTimer() { m_deathTimer = 0; }
        bool IsInDuel() const { return duel && duel->startTime != 0; }
#endif

        // function used for raise ally spell
        bool IsGhouled() const { return m_isGhouled; }
        void SetGhouled(bool enable) { m_isGhouled = enable; }

        void SendLootError(ObjectGuid guid, LootError error) const;

        // cooldown system
        virtual void AddGCD(SpellEntry const& spellEntry, uint32 forcedDuration = 0, bool updateClient = false) override;
        virtual void AddCooldown(SpellEntry const& spellEntry, ItemPrototype const* itemProto = nullptr, bool permanent = false, uint32 forcedDuration = 0, bool ignoreCat = false) override;
        virtual void RemoveSpellCooldown(SpellEntry const& spellEntry, bool updateClient = true) override;
        virtual void RemoveSpellCategoryCooldown(uint32 category, bool updateClient = true) override;
        virtual void RemoveAllCooldowns(bool sendOnly = false);
        virtual void LockOutSpells(SpellSchoolMask schoolMask, uint32 duration) override;
        void ModifyCooldown(uint32 spellId, int32 cooldownModMs);
        void RemoveSpellLockout(SpellSchoolMask spellSchoolMask, std::set<uint32>* spellAlreadySent = nullptr);
        void SendClearCooldown(uint32 spell_id, Unit* target) const;
        void RemoveArenaSpellCooldowns();
        void _LoadSpellCooldowns(QueryResult* result);
        void _SaveSpellCooldowns();
        void SetLastPotionId(uint32 itemId) { m_lastPotionId = itemId; }
        void SetCooldownEventOnLeaveCombatSpellId(uint32 spellId) { m_triggerCoooldownOnLeaveCombatSpellId = spellId; }
        uint32 GetLastPotionId() const { return m_lastPotionId; }
        void UpdatePotionCooldown(Spell* spell = nullptr);

        template <typename F>
        void RemoveSomeCooldown(F check)
        {
            auto spellCDItr = m_cooldownMap.begin();
            while (spellCDItr != m_cooldownMap.end())
            {
                SpellEntry const* entry = sSpellTemplate.LookupEntry<SpellEntry>(spellCDItr->first);
                if (entry && check(*entry))
                {
                    SendClearCooldown(spellCDItr->first, this);
                    spellCDItr = m_cooldownMap.erase(spellCDItr);
                }
                else
                    ++spellCDItr;
            }
        }

        void UpdateEverything();

        // Public Save system functions
        void SaveItemToInventory(Item* item); // optimization for gift wrapping
        void SaveTitles(); // optimization for arena rewards

        void SetQueuedSpell(Spell* spell);
        bool HasQueuedSpell();
        void ClearQueuedSpell();
        void CastQueuedSpell(SpellCastTargets& targets);

        void BanPlayer(std::string const& reason);

        uint32 m_teleportSpellIdDiagnostics;

        Spell* GetSpellModSpell() { return m_modsSpell; }
        void SetSpellModSpell(Spell* spell);

        float GetAverageItemLevel() const;

        LFGData& GetLfgData() { return m_lfgData; }

        uint32 LookupHighestLearnedRank(uint32 spellId);

        bool IsMirrorTimerActive(MirrorTimer::Type timer) const;
    protected:
        /*********************************************************/
        /***               BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        /*
        this is an array of BG queues (BgTypeIDs) in which is player
        */
        struct BgBattleGroundQueueID_Rec
        {
            BattleGroundQueueTypeId bgQueueTypeId;
            uint32 invitedToInstance;
        };

        BgBattleGroundQueueID_Rec m_bgBattleGroundQueueID[PLAYER_MAX_BATTLEGROUND_QUEUES];
        BGData                    m_bgData;

        bool m_hasWonRandomBattleground;

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

        // We allow only one timed quest active at the same time. Below can then be simple value instead of set.
        typedef std::set<uint32> QuestSet;
        QuestSet m_timedquests;
        QuestSet m_weeklyquests;
        QuestSet m_monthlyquests;

        ObjectGuid m_dividerGuid;
        uint32 m_ingametime;

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

        void _LoadActions(QueryResult* result);
        void _LoadAuras(QueryResult* result, uint32 timediff);
        void _LoadBoundInstances(QueryResult* result);
        void _LoadInventory(QueryResult* result, uint32 timediff);
        void _LoadItemLoot(QueryResult* result);
        void _LoadMails(QueryResult* result);
        void _LoadMailedItems(QueryResult* result);
        void _LoadQuestStatus(QueryResult* result);
        void _LoadDailyQuestStatus(QueryResult* result);
        void _LoadWeeklyQuestStatus(QueryResult* result);
        void _LoadMonthlyQuestStatus(QueryResult* result);
        void _LoadRandomBattlegroundStatus(QueryResult* result);
        void _LoadGroup(QueryResult* result);
        void _LoadSkills(QueryResult* result);
        void _LoadSpells(QueryResult* result);
        void _LoadTalents(QueryResult* result);
        bool _LoadHomeBind(QueryResult* result);
        void _LoadDeclinedNames(QueryResult* result);
        void _LoadArenaTeamInfo(QueryResult* result);
        void _LoadEquipmentSets(QueryResult* result);
        void _LoadBGData(QueryResult* result);
        void _LoadGlyphs(QueryResult* result);
        void _LoadIntoDataField(const char* data, uint32 startOffset, uint32 count);
        void _LoadCreatedInstanceTimers();
        void _SaveNewInstanceIdTimer();

        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/

        void _SaveActions();
        void _SaveAuras();
        void _SaveInventory();
        void _SaveMail();
        void _SaveQuestStatus();
        void _SaveDailyQuestStatus();
        void _SaveWeeklyQuestStatus();
        void _SaveMonthlyQuestStatus();
        void _SaveSkills();
        void _SaveSpells();
        void _SaveEquipmentSets();
        void _SaveBGData();
        void _SaveGlyphs();
        void _SaveTalents();
        void _SaveStats();

        /*********************************************************/
        /***              ENVIRONMENTAL SYSTEM                 ***/
        /*********************************************************/
        void HandleSobering();

        void SetEnvironmentFlags(EnvironmentFlags flags, bool apply);

        void SendMirrorTimerStart(uint32 type, uint32 remaining, uint32 duration, int32 scale, bool paused = false, uint32 spellId = 0);
        void SendMirrorTimerStop(uint32 type);
        void SendMirrorTimerPause(uint32 type, bool state);

        void FreezeMirrorTimers(bool state);
        void UpdateMirrorTimers(uint32 diff, bool send = true);

        inline bool CheckMirrorTimerActivation(MirrorTimer::Type timer) const;
        inline bool CheckMirrorTimerDeactivation(MirrorTimer::Type timer) const;

        inline void OnMirrorTimerExpirationPulse(MirrorTimer::Type timer);

        inline uint32 GetMirrorTimerMaxDuration(MirrorTimer::Type timer) const;
        inline SpellAuraHolder const* GetMirrorTimerBuff(MirrorTimer::Type timer) const;

        /*********************************************************/
        /***                  HONOR SYSTEM                     ***/
        /*********************************************************/
        time_t m_lastHonorUpdateTime;

        void outDebugStatsValues() const;
        ObjectGuid m_lootGuid;

        Team m_team;
        uint32 m_nextSave;
        time_t m_speakTime;
        uint32 m_speakCount;
        Difficulty m_dungeonDifficulty;
        Difficulty m_raidDifficulty;

        uint32 m_atLoginFlags;

        Item* m_items[PLAYER_SLOTS_COUNT];
        uint32 m_currentBuybackSlot;

        std::vector<Item*> m_itemUpdateQueue;
        bool m_itemUpdateQueueBlocked;

        uint32 m_ExtraFlags;
        ObjectGuid m_curSelectionGuid;

        QuestStatusMap mQuestStatus;

        SkillStatusMap mSkillStatus;

        uint32 m_GuildIdInvited;
        uint32 m_ArenaTeamIdInvited;

        PlayerMails m_mail;
        PlayerSpellMap m_spells;
        PlayerTalentMap m_talents[MAX_TALENT_SPEC_COUNT];
        uint32 m_lastPotionId;                              // last used health/mana potion in combat, that block next potion use
        uint32 m_triggerCoooldownOnLeaveCombatSpellId;

        uint8 m_activeSpec;
        uint8 m_specsCount;

        ActionButtonList m_actionButtons[MAX_TALENT_SPEC_COUNT];

        Glyph m_glyphs[MAX_TALENT_SPEC_COUNT][MAX_GLYPH_SLOT_INDEX];

        float m_auraBaseMod[BASEMOD_END][MOD_END];
        int16 m_baseRatingValue[MAX_COMBAT_RATING];
        uint16 m_baseSpellPower;
        uint16 m_baseFeralAP;
        uint16 m_baseManaRegen;
        float m_armorPenetrationPct;
        int32 m_spellPenetrationItemMod;

        uint32 m_enchantmentFlatMod[MAX_ATTACK]; // TODO: Stat system - incorporate generically, exposes a required hidden weapon stat that does not apply when unarmed

        SpellModList m_spellMods[MAX_SPELLMOD];
        SpellFamily m_spellClassName; // s_spellClassSet
        EnchantDurationList m_enchantDuration;
        ItemDurationList m_itemDuration;

        ObjectGuid m_resurrectGuid;
        uint32 m_resurrectMap;
        float m_resurrectX, m_resurrectY, m_resurrectZ;
        uint32 m_resurrectHealth, m_resurrectMana;
        bool m_resurrectToGhoul;

        WorldSession* m_session;

        typedef std::list<Channel*> JoinedChannelsList;
        JoinedChannelsList m_channels;

        uint32 m_cinematic;

        TradeData* m_trade;

        bool   m_DailyQuestChanged;
        bool   m_WeeklyQuestChanged;
        bool   m_MonthlyQuestChanged;

        uint32 m_drunkTimer;

        uint32 m_zoneUpdateId;
        uint32 m_zoneUpdateTimer;
        uint32 m_areaUpdateId;
        uint32 m_positionStatusUpdateTimer;

        uint32 m_deathTimer;
        time_t m_deathExpireTime;

        uint32 m_restTime;

        uint32 m_WeaponProficiency;
        uint32 m_ArmorProficiency;
        bool m_canTitanGrip;
        uint8 m_swingErrorMsg;
        float m_ammoDPSMin;
        float m_ammoDPSMax;

        //////////////////// Rest System/////////////////////
        time_t time_inn_enter;
        uint32 inn_trigger_id;
        float m_rest_bonus;
        RestType m_restType;
        //////////////////// Rest System/////////////////////

        uint32 m_resetTalentsCost;
        time_t m_resetTalentsTime;
        uint32 m_usedTalentCount;
        uint32 m_questRewardTalentCount;

        // Social
        PlayerSocial* m_social;

        // Groups
        GroupReference m_group;
        GroupReference m_originalGroup;
        Group* m_groupInvite;
        uint32 m_groupUpdateMask;

        // Player summoning
        time_t m_summon_expire;
        uint32 m_summon_mapid;
        float  m_summon_x;
        float  m_summon_y;
        float  m_summon_z;
        ObjectGuid m_summoner;

        DeclinedName* m_declinedname;
        Runes* m_runes;
        EquipmentSets m_EquipmentSets;

        bool m_isDebuggingAreaTriggers;
    private:
        void _HandleDeadlyPoison(Unit* Target, WeaponAttackType attType, SpellEntry const* spellInfo);
        // internal common parts for CanStore/StoreItem functions
        InventoryResult _CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool swap, Item* pSrcItem) const;
        InventoryResult _CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, bool non_specialized, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        InventoryResult _CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);

        void UpdateKnownCurrencies(uint32 itemId, bool apply);

        void AdjustQuestReqItemCount(Quest const* pQuest, QuestStatusData& questStatusData);

        void SetCanDelayTeleport(bool setting) { m_bCanDelayTeleport = setting; }
        bool IsHasDelayedTeleport() const
        {
            // we should not execute delayed teleports for now dead players but has been alive at teleport
            // because we don't want player's ghost teleported from graveyard
            return m_bHasDelayedTeleport && (IsAlive() || !m_bHasBeenAliveAtDelayedTeleport || GetDeathState() == JUST_DIED);
        }

        bool SetDelayedTeleportFlagIfCan()
        {
            m_bHasDelayedTeleport = m_bCanDelayTeleport;
            m_bHasBeenAliveAtDelayedTeleport = IsAlive();
            return m_bHasDelayedTeleport;
        }

        void ScheduleDelayedOperation(uint32 operation)
        {
            if (operation < DELAYED_END)
                m_DelayedOperations |= operation;
        }

        Taxi::Tracker m_taxiTracker;

        void _fillGearScoreData(Item* item, GearScoreVec* gearScore, uint32& twoHandScore);

        CinematicMgrUPtr m_cinematicMgr;

        Unit* m_mover;
        Camera m_camera;

        GridReference<Player> m_gridRef;
        MapReference m_mapRef;
        std::unique_ptr<PlayerMenu> m_playerMenu;

#ifdef BUILD_PLAYERBOT
        PlayerbotAI* m_playerbotAI;
        PlayerbotMgr* m_playerbotMgr;
#endif

        // Homebind coordinates
        uint32 m_homebindMapId;
        uint16 m_homebindAreaId;
        float m_homebindX;
        float m_homebindY;
        float m_homebindZ;

        uint32 m_lastFallTime;
        float  m_lastFallZ;

        LiquidTypeEntry const* m_lastLiquid;

        uint8 m_environmentFlags = ENVIRONMENT_FLAG_NONE;
        float m_environmentBreathingMultiplier = 1.0f;

        MirrorTimer m_mirrorTimers[MirrorTimer::NUM_TIMERS] = { MirrorTimer::FATIGUE, MirrorTimer::BREATH, MirrorTimer::FEIGNDEATH, MirrorTimer::ENVIRONMENTAL };

        // Current teleport data
        WorldLocation m_teleport_dest;
        uint32 m_teleport_options;
        ObjectGuid m_teleportTransport;
        bool m_semaphoreTeleport_Near;
        bool m_semaphoreTeleport_Far;

        uint32 m_DelayedOperations;
        bool m_bCanDelayTeleport;
        bool m_bHasDelayedTeleport;
        bool m_bHasBeenAliveAtDelayedTeleport;

        bool m_needsZoneUpdate;
        uint32 m_newZone;

        uint32 m_DetectInvTimer;

        // Temporary removed pet cache
        uint32 m_temporaryUnsummonedPetNumber;
        uint32 m_BGPetSpell;

        AchievementMgr m_achievementMgr;
        ReputationMgr  m_reputationMgr;

        uint32 m_cachedGS;

        bool m_isGhouled;

        float m_energyRegenRate;

        bool m_launched;

        std::unique_ptr<Spell> m_queuedSpell;

        Spell* m_modsSpell;
        std::set<SpellModifierPair>* m_consumedMods;

        GuidSet m_clientGUIDs;

        // Recruit-A-Friend
        uint8 m_grantableLevels;

        std::unordered_map<uint32, TimePoint> m_enteredInstances;
        uint32 m_createdInstanceClearTimer;

        uint32 m_pendingBindMapId;
        uint32 m_pendingBindId;
        uint32 m_pendingBindTimer;

        LFGData m_lfgData;

        GuidSet m_controlled;
        std::map<uint32, ObjectGuid> m_followAngles;

        uint8 m_fishingSteps;

        std::map<uint32, ItemSetEffect> m_itemSetEffects;

        std::set<uint32> m_serversideDailyQuests;
};

void AddItemsSetItem(Player* player, Item* item);
void RemoveItemsSetItem(Player* player, ItemPrototype const* proto);

// "the bodies of template functions must be made available in a header file"
template <class T> void Player::ApplySpellMod(uint32 spellId, SpellModOp op, T& basevalue, bool finalUse)
{
    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo || spellInfo->SpellFamilyName != GetSpellClass() || spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS)) return; // client condition
    int32 totalpct = 100;
    int32 totalflat = 0;
    std::vector<SpellModifier*> consumedFiniteMods;
    for (SpellModifier* mod : m_spellMods[op])
    {
        if (mod->op == SPELLMOD_CASTING_TIME || mod->op == SPELLMOD_COST)
            if (T((basevalue + totalflat) * std::max(0, totalpct) / 100) <= 0)
                break;

        if (!IsAffectedBySpellmod(spellInfo, mod, m_consumedMods))
            continue;
        if (mod->type == SPELLMOD_FLAT)
            totalflat += mod->value;
        else if (mod->type == SPELLMOD_PCT)
        {
            // special case (skip >10sec spell casts for instant cast setting)
            if (mod->op == SPELLMOD_CASTING_TIME && basevalue >= T(10 * IN_MILLISECONDS) && mod->value <= -100)
                continue;

            totalpct += mod->value;
        }

        if (mod->isFinite && finalUse)
        {
            bool consume = true;
            if (!m_consumedMods) // If empty pointer then immediately consume
            {
                consumedFiniteMods.push_back(mod); // need to delay in order to not corrupt the list
                consume = false;
            }
            else if (m_consumedMods->find(SpellModifierPair(mod->spellId, mod->modId)) != m_consumedMods->end()) // if already consumed, dont consume again
                consume = false;

            if (consume)
            {
                --mod->charges;
                m_consumedMods->insert(SpellModifierPair(mod->spellId, mod->modId));
                if (!mod->charges)
                    mod->charges = -1;
            }
        }
    }

    for (SpellModifier* mod : consumedFiniteMods)
        RemoveAuraCharge(mod->spellId);

    if (totalpct < 0)
        totalpct = 0;

    if (totalflat != 0 || totalpct != 100)
        basevalue = T((basevalue + totalflat) * std::max(0, totalpct) / 100);
}


#endif
