#ifndef _PLAYERBOTAI_H
#define _PLAYERBOTAI_H

#include "Common.h"
#include "../QuestDef.h"
#include "../GameEventMgr.h"

class WorldPacket;
class WorldObject;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;
class PlayerbotMgr;

#define BOTLOOT_DISTANCE 25.0f

enum RankOneSpellId
{
    // Mage spells and talents
    CONJURE_FOOD_1 = 587,
    CONJURE_MANA_GEM_1 = 759,
    CONJURE_REFRESHMENT_1 = 42955,
    CONJURE_WATER_1 = 5504,
    AMPLIFY_MAGIC_1 = 1008,
    ARCANE_BLAST_1 = 30451,
    ARCANE_BRILLIANCE_1 = 23028,
    ARCANE_EXPLOSION_1 = 1449,
    ARCANE_INTELLECT_1 = 1459,
    ARCANE_MISSILES_1 = 5143,
    BLINK_1 = 1953,
    BLIZZARD_1 = 10,
    CONE_OF_COLD_1 = 120,
    COUNTERSPELL_1 = 2139,
    DALARAN_BRILLIANCE_1 = 61316,
    DALARAN_INTELLECT_1 = 61024,
    DAMPEN_MAGIC_1 = 604,
    EVOCATION_1 = 12051,
    FIREBALL_1 = 133,
    FIRE_BLAST_1 = 2136,
    FIRE_WARD_1 = 543,
    FLAMESTRIKE_1 = 2120,
    FROST_ARMOR_1 = 168,
    FROSTBOLT_1 = 116,
    FROSTFIRE_BOLT_1 = 44614,
    FROST_NOVA_1 = 122,
    FROST_WARD_1 = 6143,
    ICE_ARMOR_1 = 7302,
    ICE_BLOCK_1 = 45438,
    ICE_LANCE_1 = 30455,
    INVISIBILITY_1 = 66,
    MAGE_ARMOR_1 = 6117,
    MANA_SHIELD_1 = 1463,
    MIRROR_IMAGE_1 = 55342,
    MOLTEN_ARMOR_1 = 30482,
    REMOVE_CURSE_1 = 475,
    RITUAL_OF_REFRESHMENT_1 = 43987,
    SCORCH_1 = 2948,
    SLOW_FALL_1 = 130,
    SPELLSTEAL_1 = 30449,
    ARCANE_BARRAGE_1 = 44425,
    ARCANE_POWER_1 = 12042,
    BLAST_WAVE_1 = 11113,
    COLD_SNAP_1 = 11958,
    COMBUSTION_1 = 11129,
    DEEP_FREEZE_1 = 44572,
    DRAGONS_BREATH_1 = 31661,
    FOCUS_MAGIC_1 = 54646,
    ICE_BARRIER_1 = 11426,
    ICY_VEINS_1 = 12472,
    LIVING_BOMB_1 = 44457,
    PRESENCE_OF_MIND_1 = 12043,
    PYROBLAST_1 = 11366,
    SLOW_1 = 31589,
    SUMMON_WATER_ELEMENTAL_1 = 31687,

    // Warlock
    CREATE_FIRESTONE_1 = 6366,
    CREATE_SOULSTONE_1 = 693,
    CREATE_SPELLSTONE_1 = 2362,
    BANISH_1 = 710,
    CHALLENGING_HOWL_1 = 59671,
    CORRUPTION_1 = 172,
    CREATE_HEALTHSTONE_1 = 6201,
    CURSE_OF_AGONY_1 = 980,
    CURSE_OF_DOOM_1 = 603,
    CURSE_OF_THE_ELEMENTS_1 = 1490,
    CURSE_OF_TONGUES_1 = 1714,
    CURSE_OF_WEAKNESS_1 = 702,
    DEATH_COIL_1 = 6789,
    DEMON_ARMOR_1 = 706,
    DEMON_CHARGE_1 = 54785,
    DEMONIC_CIRCLE_SUMMON_1 = 48018,
    DEMONIC_CIRCLE_TELEPORT_1 = 48020,
    DEMONIC_IMMOLATE_1 = 75445,
    DEMON_SKIN_1 = 687,
    DETECT_INVISIBILITY_1 = 132,
    DRAIN_LIFE_1 = 689,
    DRAIN_MANA_1 = 5138,
    DRAIN_SOUL_1 = 1120,
    ENSLAVE_DEMON_1 = 1098,
    EYE_OF_KILROGG_1 = 126,
    FEAR_1 = 5782,
    FEL_ARMOR_1 = 28176,
    HEALTH_FUNNEL_1 = 755,
    HELLFIRE_1 = 1949,
    HOWL_OF_TERROR_1 = 5484,
    IMMOLATE_1 = 348,
    IMMOLATION_AURA_1 = 50589,
    INCINERATE_1 = 29722,
    INFERNO_1 = 1122,
    LIFE_TAP_1 = 1454,
    RAIN_OF_FIRE_1 = 5740,
    RITUAL_OF_DOOM_1 = 18540,
    RITUAL_OF_SOULS_1 = 29893,
    RITUAL_OF_SUMMONING_1 = 698,
    SEARING_PAIN_1 = 5676,
    SEED_OF_CORRUPTION_1 = 27243,
    SENSE_DEMONS_1 = 5500,
    SHADOW_BOLT_1 = 686,
    SHADOW_CLEAVE_1 = 50581,
    SHADOWFLAME_1 = 47897,
    SHADOW_WARD_1 = 6229,
    SOUL_FIRE_1 = 6353,
    SOULSHATTER_1 = 29858,
    SUMMON_FELHUNTER_1 = 691,
    SUMMON_IMP_1 = 688,
    SUMMON_SUCCUBUS_1 = 712,
    SUMMON_VOIDWALKER_1 = 697,
    UNENDING_BREATH_1 = 5697,
    CHAOS_BOLT_1 = 50796,
    CONFLAGRATE_1 = 17962,
    CURSE_OF_EXHAUSTION_1 = 18223,
    DARK_PACT_1 = 18220,
    DEMONIC_EMPOWERMENT_1 = 47193,
    FEL_DOMINATION_1 = 18708,
    HAUNT_1 = 48181,
    METAMORPHOSIS_1 = 59672,
    SHADOWBURN_1 = 17877,
    SHADOWFURY_1 = 30283,
    SOUL_LINK_1 = 19028,
    SUMMON_FELGUARD_1 = 30146,
    UNSTABLE_AFFLICTION_1 = 30108,

    //Paladin
    AVENGING_WRATH_1 = 31884,
    BLESSING_OF_KINGS_1 = 20217,
    BLESSING_OF_MIGHT_1 = 19740,
    BLESSING_OF_WISDOM_1 = 19742,
    CLEANSE_1 = 4987,
    CONCENTRATION_AURA_1 = 19746,
    CONSECRATION_1 = 26573,
    CRUSADER_AURA_1 = 32223,
    DEVOTION_AURA_1 = 465,
    DIVINE_INTERVENTION_1 = 19752,
    DIVINE_PLEA_1 = 54428,
    DIVINE_PROTECTION_1 = 498,
    DIVINE_SHIELD_1 = 642,
    EXORCISM_1 = 879,
    FIRE_RESISTANCE_AURA_1 = 19891,
    FLASH_OF_LIGHT_1 = 19750,
    FROST_RESISTANCE_AURA_1 = 19888,
    GREATER_BLESSING_OF_KINGS_1 = 25898,
    GREATER_BLESSING_OF_MIGHT_1 = 25782,
    GREATER_BLESSING_OF_SANCTUARY_1 = 25899,
    GREATER_BLESSING_OF_WISDOM_1 = 25894,
    HAMMER_OF_JUSTICE_1 = 853,
    HAMMER_OF_WRATH_1 = 24275,
    HAND_OF_FREEDOM_1 = 1044,
    HAND_OF_PROTECTION_1 = 1022,
    HAND_OF_RECKONING_1 = 62124,
    HAND_OF_SACRIFICE_1 = 6940,
    HAND_OF_SALVATION_1 = 1038,
    HOLY_LIGHT_1 = 635,
    HOLY_WRATH_1 = 2812,
    JUDGEMENT_OF_JUSTICE_1 = 53407,
    JUDGEMENT_OF_LIGHT_1 = 20271,
    JUDGEMENT_OF_WISDOM_1 = 53408,
    LAY_ON_HANDS_1 = 633,
    PURIFY_1 = 1152,
    REDEMPTION_1 = 7328,
    RETRIBUTION_AURA_1 = 7294,
    RIGHTEOUS_DEFENSE_1 = 31789,
    RIGHTEOUS_FURY_1 = 25780,
    SACRED_SHIELD_1 = 53601,
    SEAL_OF_CORRUPTION = 53736,
    SEAL_OF_JUSTICE_1 = 20164,
    SEAL_OF_LIGHT_1 = 20165,
    SEAL_OF_RIGHTEOUSNESS_1 = 20154,
    SEAL_OF_VENGEANCE = 31801,
    SEAL_OF_WISDOM_1 = 20166,
    SENSE_UNDEAD_1 = 5502,
    SHADOW_RESISTANCE_AURA_1 = 19876,
    SHIELD_OF_RIGHTEOUSNESS_1 = 53600,
    TURN_EVIL_1 = 10326,
    AURA_MASTERY_1 = 31821,
    AVENGERS_SHIELD_1 = 31935,
    BEACON_OF_LIGHT_1 = 53563,
    BLESSING_OF_SANCTUARY_1 = 20911,
    CRUSADER_STRIKE_1 = 35395,
    DIVINE_FAVOR_1 = 20216,
    DIVINE_ILLUMINATION_1 = 31842,
    DIVINE_SACRIFICE_1 = 64205,
    DIVINE_STORM_1 = 53385,
    HAMMER_OF_THE_RIGHTEOUS_1 = 53595,
    HOLY_SHIELD_1 = 20925,
    HOLY_SHOCK_1 = 20473,
    REPENTANCE_1 = 20066,
    SEAL_OF_COMMAND_1 = 20375,

    //Priest
    ABOLISH_DISEASE_1 = 552,
    BINDING_HEAL_1 = 32546,
    BLESSED_HEALING_1 = 70772,
    CURE_DISEASE_1 = 528,
    DEVOURING_PLAGUE_1 = 2944,
    DISPEL_MAGIC_1 = 527,
    DIVINE_HYMN_1 = 64843,
    DIVINE_SPIRIT_1 = 14752,
    FADE_1 = 586,
    FEAR_WARD_1 = 6346,
    FLASH_HEAL_1 = 2061,
    GREATER_HEAL_1 = 2060,
    HEAL_1 = 2054,
    HOLY_FIRE_1 = 14914,
    HOLY_NOVA_1 = 15237,
    HYMN_OF_HOPE_1 = 64901,
    INNER_FIRE_1 = 588,
    LESSER_HEAL_1 = 2050,
    LEVITATE_1 = 1706,
    MANA_BURN_1 = 8129,
    MASS_DISPEL_1 = 32375,
    MIND_BLAST_1 = 8092,
    MIND_CONTROL_1 = 605,
    MIND_SEAR_1 = 48045,
    MIND_SOOTHE_1 = 453,
    MIND_VISION_1 = 2096,
    POWER_WORD_FORTITUDE_1 = 1243,
    POWER_WORD_SHIELD_1 = 17,
    PRAYER_OF_FORTITUDE_1 = 21562,
    PRAYER_OF_HEALING_1 = 596,
    PRAYER_OF_MENDING_1 = 33076,
    PRAYER_OF_SHADOW_PROTECTION_1 = 27683,
    PRAYER_OF_SPIRIT_1 = 27681,
    PSYCHIC_SCREAM_1 = 8122,
    RENEW_1 = 139,
    RESURRECTION_1 = 2006,
    SHACKLE_UNDEAD_1 = 9484,
    SHADOWFIEND_1 = 34433,
    SHADOW_PROTECTION_1 = 976,
    SHADOW_WORD_DEATH_1 = 32379,
    SHADOW_WORD_PAIN_1 = 589,
    SMITE_1 = 585,
    CIRCLE_OF_HEALING_1 = 34861,
    DESPERATE_PRAYER_1 = 19236,
    DISPERSION_1 = 47585,
    GUARDIAN_SPIRIT_1 = 47788,
    INNER_FOCUS_1 = 14751,
    LIGHTWELL_1 = 724,
    MIND_FLAY_1 = 15407,
    PAIN_SUPPRESSION_1 = 33206,
    PENANCE_1 = 47540,
    POWER_INFUSION_1 = 10060,
    PSYCHIC_HORROR_1 = 64044,
    SHADOWFORM_1 = 15473,
    SILENCE_1 = 15487,
    VAMPIRIC_EMBRACE_1 = 15286,
    VAMPIRIC_TOUCH_1 = 34914,

    //Druid
    ABOLISH_POISON_1 = 2893,
    AQUATIC_FORM_1 = 1066,
    BARKSKIN_1 = 22812,
    BASH_1 = 5211,
    BEAR_FORM_1 = 5487,
    CAT_FORM_1 = 768,
    CHALLENGING_ROAR_1 = 5209,
    CLAW_1 = 1082,
    COWER_1 = 8998,
    CURE_POISON_1 = 8946,
    CYCLONE_1 = 33786,
    DASH_1 = 1850,
    DEMORALIZING_ROAR_1 = 99,
    DIRE_BEAR_FORM_1 = 9634,
    ENRAGE_1 = 5229,
    ENTANGLING_ROOTS_1 = 339,
    FAERIE_FIRE_1 = 770,
    FAERIE_FIRE_FERAL_1 = 16857,
    FERAL_CHARGE_BEAR_1 = 16979,
    FERAL_CHARGE_CAT_1 = 49376,
    FEROCIOUS_BITE_1 = 22568,
    FLIGHT_FORM_1 = 33943,
    FRENZIED_REGENERATION_1 = 22842,
    GIFT_OF_THE_WILD_1 = 21849,
    GROWL_1 = 6795,
    HEALING_TOUCH_1 = 5185,
    HIBERNATE_1 = 2637,
    HURRICANE_1 = 16914,
    INNERVATE_1 = 29166,
    LACERATE_1 = 33745,
    LIFEBLOOM_1 = 33763,
    MAIM_1 = 22570,
    MANGLE_BEAR_1 = 33878,
    MANGLE_CAT_1 = 33876,
    MARK_OF_THE_WILD_1 = 1126,
    MAUL_1 = 6807,
    MOONFIRE_1 = 8921,
    NATURES_GRASP_1 = 16689,
    NOURISH_1 = 50464,
    POUNCE_1 = 9005,
    PROWL_1 = 5215,
    RAKE_1 = 1822,
    RAVAGE_1 = 6785,
    REBIRTH_1 = 20484,
    REGROWTH_1 = 8936,
    REJUVENATION_1 = 774,
    REMOVE_CURSE_1 = 2782,
    REVIVE_1 = 50769,
    RIP_1 = 1079,
    SAVAGE_ROAR_1 = 52610,
    SHRED_1 = 5221,
    SOOTHE_ANIMAL_1 = 2908,
    STARFIRE_1 = 2912,
    SWIFT_FLIGHT_FORM_1 = 40120,
    SWIPE_BEAR_1 = 779,
    SWIPE_CAT_1 = 62078,
    THORNS_1 = 467,
    TIGERS_FURY_1 = 5217,
    TRANQUILITY_1 = 740,
    TRAVEL_FORM_1 = 783,
    WRATH_1 = 5176,
    BERSERK_1 = 50334,
    FERAL_CHARGE_1 = 49377,
    FORCE_OF_NATURE_1 = 33831,
    INSECT_SWARM_1 = 5570,
    MANGLE_1 = 33917,
    MOONKIN_FORM_1 = 24858,
    NATURES_SWIFTNESS_1 = 17116,
    STARFALL_1 = 48505,
    SURVIVAL_INSTINCTS_1 = 61336,
    SWIFTMEND_1 = 18562,
    TREE_OF_LIFE_1 = 33891,
    TYPHOON_1 = 50516,
    WILD_GROWTH_1 = 48438,

};

class MANGOS_DLL_SPEC PlayerbotAI
{
    public:
        enum ScenarioType
        {
            SCENARIO_PVEEASY,
            SCENARIO_PVEHARD,
            SCENARIO_DUEL,
            SCENARIO_PVPEASY,
            SCENARIO_PVPHARD
        };

        enum CombatStyle {
            COMBAT_MELEE                = 0x01,         // class melee attacker
            COMBAT_RANGED               = 0x02          // class is ranged attacker
        };

        // masters orders that should be obeyed by the AI during the updteAI routine
        // the master will auto set the target of the bot
        enum CombatOrderType
        {
            ORDERS_NONE                 = 0x00,         // no special orders given
            ORDERS_TANK                 = 0x01,         // bind attackers by gaining threat
            ORDERS_ASSIST               = 0x02,         // assist someone (dps type)
            ORDERS_HEAL                 = 0x04,         // concentrate on healing (no attacks, only self defense)
            ORDERS_PROTECT              = 0x10,         // combinable state: check if protectee is attacked
            ORDERS_PRIMARY              = 0x0F,
            ORDERS_SECONDARY            = 0xF0,
            ORDERS_RESET                = 0xFF
        };

        enum CombatTargetType
        {
            TARGET_NORMAL       = 0x00,
            TARGET_THREATEN     = 0x01
        };

        enum BotState
        {
            BOTSTATE_NORMAL,        // normal AI routines are processed
            BOTSTATE_COMBAT,        // bot is in combat
            BOTSTATE_DEAD,          // we are dead and wait for becoming ghost
            BOTSTATE_DEADRELEASED,  // we released as ghost and wait to revive
            BOTSTATE_LOOTING        // looting mode, used just after combat
        };

        enum MovementOrderType
        {
            MOVEMENT_NONE		= 0x00,
            MOVEMENT_FOLLOW		= 0x01,
            MOVEMENT_STAY		= 0x02
        };

        typedef std::map<uint32, uint32> BotNeedItem;
        typedef std::list<uint64> BotLootCreature;

        // attacker query used in PlayerbotAI::FindAttacker()
        enum ATTACKERINFOTYPE
        {
            AIT_NONE            = 0x00,
            AIT_LOWESTTHREAT    = 0x01,
            AIT_HIGHESTTHREAT   = 0x02,
            AIT_VICTIMSELF      = 0x04,
            AIT_VICTIMNOTSELF   = 0x08	// !!! must use victim param in FindAttackers
        };
        struct AttackerInfo
        {
            Unit*    attacker;        // reference to the attacker
            Unit*    victim;          // combatant's current victim
            float    threat;          // own threat on this combatant
            float    threat2;         // highest threat not caused by bot
            uint32  count;            // number of units attacking
            uint32  source;           // 1=bot, 2=master, 3=group
        };
        typedef std::map<uint64,AttackerInfo> AttackerInfoList;

    public:
        PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot);
        virtual ~PlayerbotAI();

        // This is called from Unit.cpp and is called every second (I think)
        void UpdateAI(const uint32 p_time);

        // This is called from ChatHandler.cpp when there is an incoming message to the bot
        // from a whisper or from the party channel
        void HandleCommand(const std::string& text, Player& fromPlayer);

        // This is called by WorldSession.cpp
        // It provides a view of packets normally sent to the client.
        // Since there is no client at the other end, the packets are dropped of course.
        // For a list of opcodes that can be caught see Opcodes.cpp (SMSG_* opcodes only)
        void HandleBotOutgoingPacket(const WorldPacket& packet);

        // This is called by WorldSession.cpp
        // when it detects that a bot is being teleported. It acknowledges to the server to complete the
        // teleportation
        void HandleTeleportAck();

        // Returns what kind of situation we are in so the ai can react accordingly
        ScenarioType GetScenarioType() {return m_ScenarioType;}

        PlayerbotClassAI* GetClassAI() {return m_classAI;}
        PlayerbotMgr* const GetManager() {return m_mgr;}

        // finds spell ID for matching substring args
        // in priority of full text match, spells not taking reagents, and highest rank
        uint32 getSpellId(const char* args, bool master = false) const;
        uint32 getPetSpellId(const char* args) const;
        uint32 getMaxKnownRankSpellId(uint32 spellId);

        // extracts item ids from links
        void extractItemIds(const std::string& text, std::list<uint32>& itemIds) const;

        // extract spellid from links
        bool extractSpellId(const std::string& text, uint32 &spellId) const;

        // extracts currency from a string as #g#s#c and returns the total in copper
        uint32 extractMoney(const std::string& text) const;

        // extracts gameobject info from link
        bool extractGOinfo(const std::string& text, uint32 &guid,  uint32 &entry, int &mapid, float &x, float &y, float &z) const;

        // finds items in bots equipment and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
        // finds items in bots inventory and adds them to foundItemList, removes found items from itemIdSearchList
        void findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;

        // currently bots only obey commands from the master
        bool canObeyCommandFrom(const Player& player) const;

        // get current casting spell (will return NULL if no spell!)
        Spell* GetCurrentSpell() const;

        bool HasAura(uint32 spellId, const Unit& player) const;
        bool HasAura(const char* spellName, const Unit& player) const;
        bool HasAura(const char* spellName) const;

        bool HasPick();

        uint8 GetHealthPercent(const Unit& target) const;
        uint8 GetHealthPercent() const;
        uint8 GetBaseManaPercent(const Unit& target) const;
        uint8 GetBaseManaPercent() const;
        uint8 GetManaPercent(const Unit& target) const;
        uint8 GetManaPercent() const;
        uint8 GetRageAmount(const Unit& target) const;
        uint8 GetRageAmount() const;
        uint8 GetEnergyAmount(const Unit& target) const;
        uint8 GetEnergyAmount() const;
        uint8 GetRunicPower(const Unit& target) const;
        uint8 GetRunicPower() const;

        Item* FindFood() const;
        Item* FindDrink() const;
        Item* FindBandage() const;
        Item* FindPoison() const;
        Item* FindMount(uint32 matchingRidingSkill) const;
        Item* FindItem(uint32 ItemId);

        // ******* Actions ****************************************
        // Your handlers can call these actions to make the bot do things.
        void TellMaster(const std::string& text) const;
        void TellMaster( const char *fmt, ... ) const;
        void SendWhisper(const std::string& text, Player& player) const;
        bool CastSpell(const char* args);
        bool CastSpell(uint32 spellId);
        bool CastSpell(uint32 spellId, Unit& target);
        bool CastPetSpell(uint32 spellId, Unit* target = NULL);
        void UseItem(Item& item);
        void EquipItem(Item& item);
        //void Stay();
        //bool Follow(Player& player);
        void SendNotEquipList(Player& player);
        void Feast();
        void InterruptCurrentCastingSpell();
        void GetCombatTarget( Unit* forcedTarged = 0 );
        Unit *GetCurrentTarget() { return m_targetCombat; };
        void DoNextCombatManeuver();
        void DoCombatMovement();
        void SetIgnoreUpdateTime(uint8 t) {m_ignoreAIUpdatesUntilTime=time(0) + t; };

        Player *GetPlayerBot() const {return m_bot;}
        Player *GetPlayer() const {return m_bot;}
        Player *GetMaster() const;

        BotState GetState() { return m_botState; };
        void SetState( BotState state );
        void SetQuestNeedItems();
        void SendQuestItemList( Player& player );
        void SendOrders( Player& player );
        bool FollowCheckTeleport( WorldObject &obj );
        void DoLoot();

        uint32 EstRepairAll();
        uint32 EstRepair(uint16 pos);

        void AcceptQuest( Quest const *qInfo, Player *pGiver );
        void TurnInQuests( WorldObject *questgiver );

        bool IsInCombat();
        void UpdateAttackerInfo();
        Unit* FindAttacker( ATTACKERINFOTYPE ait=AIT_NONE, Unit *victim=0 );
        uint32 GetAttackerCount() { return m_attackerInfo.size(); };
        void SetCombatOrderByStr( std::string str, Unit *target=0 );
        void SetCombatOrder( CombatOrderType co, Unit *target=0 );
        CombatOrderType GetCombatOrder() { return this->m_combatOrder; }
        void SetMovementOrder( MovementOrderType mo, Unit *followTarget=0 );
        MovementOrderType GetMovementOrder() { return this->m_movementOrder; }
        void MovementReset();
        void MovementUpdate();
        void MovementClear();
        bool IsMoving();

        void SetInFront( const Unit* obj );

        void ItemLocalization(std::string& itemName, const uint32 itemID) const;
        void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    private:
        // ****** Closed Actions ********************************
        // These actions may only be called at special times.
        // Trade methods are only applicable when the trade window is open
        // and are only called from within HandleCommand.
        bool TradeItem(const Item& item, int8 slot=-1);
        bool TradeCopper(uint32 copper);

        // it is safe to keep these back reference pointers because m_bot
        // owns the "this" object and m_master owns m_bot. The owner always cleans up.
        PlayerbotMgr* const m_mgr;
        Player* const m_bot;
        PlayerbotClassAI* m_classAI;

        // ignores AI updates until time specified
        // no need to waste CPU cycles during casting etc
        time_t m_ignoreAIUpdatesUntilTime;

        CombatStyle m_combatStyle;
        CombatOrderType m_combatOrder;
        MovementOrderType m_movementOrder;

        ScenarioType m_ScenarioType;

        // defines the state of behaviour of the bot
        BotState m_botState;

        // list of items needed to fullfill quests
        BotNeedItem m_needItemList;

        // list of creatures we recently attacked and want to loot
        BotLootCreature m_lootCreature;  // list of creatures
        uint64 m_lootCurrent;            // current remains of interest

        time_t m_TimeDoneEating;
        time_t m_TimeDoneDrinking;
        uint32 m_CurrentlyCastingSpellId;
        //bool m_IsFollowingMaster;

        // if master commands bot to do something, store here until updateAI
        // can do it
        uint32 m_spellIdCommand;
        uint64 m_targetGuidCommand;

        AttackerInfoList m_attackerInfo;

        bool m_targetChanged;
        CombatTargetType m_targetType;

        Unit *m_targetCombat;	// current combat target
        Unit *m_targetAssist;	// get new target by checking attacker list of assisted player
        Unit *m_targetProtect;	// check

        Unit *m_followTarget;	// whom to follow in non combat situation?
};

#endif
