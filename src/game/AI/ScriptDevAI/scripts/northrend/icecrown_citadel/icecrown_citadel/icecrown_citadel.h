/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_CITADEL_H
#define DEF_ICECROWN_CITADEL_H

#include "Chat/Chat.h"

enum
{
    MAX_ENCOUNTER                   = 16,

    TYPE_MARROWGAR                  = 0,
    TYPE_LADY_DEATHWHISPER          = 1,
    TYPE_GUNSHIP_BATTLE             = 2,
    TYPE_DEATHBRINGER_SAURFANG      = 3,
    TYPE_FESTERGUT                  = 4,
    TYPE_ROTFACE                    = 5,
    TYPE_PROFESSOR_PUTRICIDE        = 6,
    TYPE_BLOOD_PRINCE_COUNCIL       = 7,
    TYPE_QUEEN_LANATHEL             = 8,
    TYPE_VALITHRIA                  = 9,
    TYPE_SINDRAGOSA                 = 10,
    TYPE_LICH_KING                  = 11,
    TYPE_BLOOD_WING_ENTRANCE        = 12,
    TYPE_FROST_WING_ENTRANCE        = 13,
    TYPE_PLAGUE_WING_ENTRANCE       = 14,
    TYPE_SPIRE_FROSTWYRM            = 15,

    // main boss entries
    NPC_LORD_MARROWGAR              = 36612,
    NPC_LADY_DEATHWHISPER           = 36855,
    NPC_DEATHBRINGER_SAURFANG       = 37813,
    NPC_FESTERGUT                   = 36626,
    NPC_ROTFACE                     = 36627,
    NPC_PROFESSOR_PUTRICIDE         = 36678,
    NPC_TALDARAM                    = 37973,
    NPC_VALANAR                     = 37970,
    NPC_KELESETH                    = 37972,
    NPC_QUEEN_LANATHEL              = 37955,
    NPC_VALITHRIA                   = 36789,
    NPC_SINDRAGOSA                  = 36853,
    NPC_LICH_KING                   = 36597,

    // boss-related and other NPCs
    NPC_COLDFLAME                   = 36672,

    NPC_DEATHWHISPER_SPAWN_STALKER  = 37947,        // Lady Deathwhisper stalkers
    NPC_DEATHWHISPER_CONTROLLER     = 37948,

    NPC_CULT_FANATIC                = 37890,        // Deathwhisper cultists
    NPC_CULT_ADHERENT               = 37949,
    NPC_DEFORMED_FANATIC            = 38135,
    NPC_EMPOWERED_ADHERENT          = 38136,
    NPC_REANIMATED_FANATIC          = 38009,
    NPC_REANIMATED_ADHERENT         = 38010,

    NPC_SPIRE_FROSTWYRM             = 37230,        // Frostwyrm on the Rampart

    NPC_GUNSHIP_SAURFANG            = 36939,        // Gunship npcs
    NPC_GUNSHIP_MURADIN             = 36948,
    NPC_SKYBREAKER                  = 37540,        // actual ships that will take the damage
    NPC_ORGRIMS_HAMMER              = 37215,
    NPC_GUNSHIP_HULL                = 37547,
    NPC_TELEPORT_PORTAL             = 37227,        // spawned on enemy ship as a source to teleport soldiers
    NPC_TELEPORT_EXIT               = 37488,        // spawned on player ship as a target to teleport soldiers

    NPC_SKYBREAKER_SORCERER         = 37116,        // Gunship soldiers
    NPC_SKYBREAKER_RIFLEMAN         = 36969,
    NPC_SKYBREAKER_MORTAR_SOLDIER   = 36978,
    NPC_SKYBREAKER_MARINE           = 36950,
    NPC_SKYBREAKER_SERGEANT         = 36961,
    NPC_KORKRON_BATTLE_MAGE         = 37117,
    NPC_KORKRON_AXETHROWER          = 36968,
    NPC_KORKRON_ROCKETEER           = 36982,
    NPC_KORKRON_REAVER              = 36957,
    NPC_KORKRON_SERGEANT            = 36960,

    NPC_OVERLORD_SAURFANG           = 37187,        // Saurfang intro / outro
    NPC_KORKRON_REAVER_RISE         = 37920,
    NPC_MURADIN_BRONZEBEARD         = 37200,
    NPC_SKYBREAKER_MARINE_RISE      = 37830,
    // Note: additional to these on the Alliance side there are also the following used: 37879, 37188, 37880

    NPC_FROST_FREEZE_TRAP           = 37744,

    NPC_BLOOD_ORB_CONTROL           = 38008,
    NPC_PUTRICIDES_TRAP             = 38879,        // Handles trap event before Putricide
    NPC_GAS_STALKER                 = 36659,        // Handles the gas in Festergut room
    NPC_MAD_SCIENTIST_STALKER       = 37824,        // Putricide stalkers
    NPC_LANATHEL_INTRO              = 38004,

    NPC_VALITHRIA_QUEST             = 38589,
    NPC_VALITHRIA_COMBAT_TRIGGER    = 38752,

    NPC_TIRION_FORDRING             = 38995,        // Lich King event creatures
    NPC_TENERAS_MENETHIL            = 38579,

    NPC_FROSTMOURNE_TRIGGER         = 38584,
    NPC_FROSTMOURNE_HOLDER          = 27880,
    NPC_STINKY                      = 37025,
    NPC_PRECIOUS                    = 37217,
    NPC_PUDDLE_STALKER              = 37013,        // related to Festergut and Rotface
    NPC_RIMEFANG                    = 37533,
    NPC_SPINESTALKER                = 37534,
    NPC_OOZE_TENTACLE_STALKER       = 38308,        // accessories to Putricide
    NPC_SLIMY_TENTACLE_STALKER      = 38309,

    // Blood wing entrance creatures
    NPC_DARFALLEN_NOBLE             = 37663,
    NPC_DARKFALLEN_ARCHMAGE         = 37664,
    NPC_DARKFALLEN_BLOOD_KNIGHT     = 37595,
    NPC_DARKFALLEN_ADVISOR          = 37571,
    NPC_EMPOWERING_ORB_STALKER      = 36934,

    // achievement types
    MAX_SPECIAL_ACHIEV_CRITS        = 4,

    TYPE_ACHIEV_BONED               = 0,
    TYPE_ACHIEV_MADE_A_MESS         = 1,
    TYPE_ACHIEV_DANCES_OOZES        = 2,
    TYPE_ACHIEV_NAUSEA              = 3,

    // GameObjects entries
    GO_ICEWALL_1                    = 201911,
    GO_ICEWALL_2                    = 201910,
    GO_MARROWGAR_DOOR               = 201857,       // Marrowgar combat door

    GO_ORATORY_DOOR                 = 201563,
    GO_DEATHWHISPER_ELEVATOR        = 202220,

    // Gunships
    GO_THE_SKYBREAKER_A             = 201580,       // alliance raid, alliance ship; map 672
    GO_ORGRIMS_HAMMER_A             = 201581,       // alliance raid, horde ship; map 673

    GO_THE_SKYBREAKER_H             = 201811,       // horde raid, alliance ship; map 672
    GO_ORGRIMS_HAMMER_H             = 201812,       // horde raid, horde ship; map 673

    GO_ZEPPELIN_HORDE               = 201834,       // ship used during the Saurfang event; map 718

    GO_SAURFANG_DOOR                = 201825,
    GO_HORDE_TELEPORTER             = 201880,       // dummy objects; spawned on the Deathbringers rise
    GO_ALLIANCE_TELEPORTER          = 201858,

    GO_GREEN_PLAGUE                 = 201370,       // Rotface combat door
    GO_ORANGE_PLAGUE                = 201371,       // Festergut combat door
    GO_SCIENTIST_DOOR               = 201372,       // Putricide combat door
    GO_SCIENTIST_DOOR_COLLISION     = 201612,       // Putricide pathway doors
    GO_SCIENTIST_DOOR_ORANGE        = 201613,
    GO_SCIENTIST_DOOR_GREEN         = 201614,
    GO_GREEN_VALVE                  = 201615,       // Valves used to release the Gas / Oozes in order to open the pathway to Putricide - triggers event 23426
    GO_ORANGE_VALVE                 = 201616,       // triggers event 23438
    GO_ORANGE_TUBE                  = 201617,
    GO_GREEN_TUBE                   = 201618,
    GO_DRINK_ME                     = 201584,       // Putricide table; used to start the encounter

    // GO_BLOODWING_DOOR            = 201920,       // Blood wing entrance door - not used
    GO_EMPOWERING_BLOOD_ORB         = 201741,
    GO_CRIMSON_HALL_DOOR            = 201376,       // Council combat door
    GO_COUNCIL_DOOR_1               = 201377,
    GO_COUNCIL_DOOR_2               = 201378,
    GO_BLOODPRINCE_DOOR             = 201746,       // Lanathel combat door
    GO_ICECROWN_GRATE               = 201755,       // Lanathel trap door

    // GO_FROSTWING_DOOR             = 201919,       // not used
    GO_GREEN_DRAGON_ENTRANCE        = 201375,       // Valithria combat door
    GO_GREEN_DRAGON_EXIT            = 201374,
    GO_VALITHRIA_DOOR_1             = 201381,       // Valithria event doors
    GO_VALITHRIA_DOOR_2             = 201382,
    GO_VALITHRIA_DOOR_3             = 201383,
    GO_VALITHRIA_DOOR_4             = 201380,
    GO_SINDRAGOSA_SHORTCUT_ENTRANCE = 201369,       // Shortcut doors are opened only after the trash before Sindragosa is cleared
    GO_SINDRAGOSA_SHORTCUT_EXIT     = 201379,
    GO_SINDRAGOSA_ENTRANCE          = 201373,

    GO_ICESHARD_1                   = 202142,
    GO_ICESHARD_2                   = 202141,
    GO_ICESHARD_3                   = 202143,
    GO_ICESHARD_4                   = 202144,
    GO_FROSTY_WIND                  = 202188,
    GO_FROSTY_EDGE                  = 202189,
    GO_SNOW_EDGE                    = 202190,
    GO_ARTHAS_PLATFORM              = 202161,
    GO_ARTHAS_PRECIPICE             = 202078,

    GO_PLAGUE_SIGIL                 = 202182,       // Possible used after each wing is cleared
    GO_FROSTWING_SIGIL              = 202181,
    GO_BLOODWING_SIGIL              = 202183,

    // Loot chests
    GO_SAURFANG_CACHE               = 202239,
    GO_SAURFANG_CACHE_25            = 202240,
    GO_SAURFANG_CACHE_10_H          = 202238,
    GO_SAURFANG_CACHE_25_H          = 202241,

    GO_GUNSHIP_ARMORY_A             = 201872,
    GO_GUNSHIP_ARMORY_A_25          = 201873,
    GO_GUNSHIP_ARMORY_A_10H         = 201874,
    GO_GUNSHIP_ARMORY_A_25H         = 201875,

    GO_GUNSHIP_ARMORY_H             = 202177,
    GO_GUNSHIP_ARMORY_H_25          = 202178,
    GO_GUNSHIP_ARMORY_H_10H         = 202179,
    GO_GUNSHIP_ARMORY_H_25H         = 202180,

    GO_DREAMWALKER_CACHE            = 201959,
    GO_DREAMWALKER_CACHE_25         = 202339,
    GO_DREAMWALKER_CACHE_10_H       = 202338,
    GO_DREAMWALKER_CACHE_25_H       = 202340,

    // transporters
    GO_TRANSPORTER_FROZEN_THRONE    = 202223,       // used only for display
    GO_TRANSPORTER_UPPER_SPIRE      = 202235,
    GO_TRANSPORTER_LIGHTS_HAMMER    = 202242,
    GO_TRANSPORTER_RAMPART_SKULLS   = 202243,
    GO_TRANSPORTER_DEATHBRINGER     = 202244,
    GO_TRANSPORTER_ORATORY_DAMNED   = 202245,
    GO_TRANSPORTER_SINDRAGOSA       = 202246,

    // Area triggers
    AT_SINDRAGOSA_PLATFORM          = 5604,
    AT_LIGHTS_HAMMER_INTRO_1        = 5611,
    AT_LIGHTS_HAMMER_INTRO_2        = 5612,
    AT_RAMPART_ALLIANCE             = 5628,
    AT_RAMPART_HORDE                = 5630,
    AT_PUTRICIDES_TRAP              = 5647,
    AT_DEATHWHISPER_INTRO           = 5709,
    AT_FROZEN_THRONE_TELE           = 5718,
    AT_MARROWGAR_INTRO              = 5732,

    // Spells
    SPELL_SOUL_FEAST                = 71203,            // kill credit for 38121
    SPELL_FULL_HOUSE_ACHIEV_CHECK   = 72827,
    SPELL_SIPHON_ESSENCE            = 70299,

    // Achievement criterias
    ACHIEV_CRIT_BONED_10N                  = 12775,     // Lord Marrowgar, achievs 4534, 4610
    ACHIEV_CRIT_BONED_25N                  = 12962,
    ACHIEV_CRIT_BONED_10H                  = 13393,
    ACHIEV_CRIT_BONED_25H                  = 13394,

    ACHIEV_CRIT_HOUSE_10N                  = 12776,     // Lady Deathwhisper, achievs 4535, 4611
    ACHIEV_CRIT_HOUSE_25N                  = 12997,
    ACHIEV_CRIT_HOUSE_10H                  = 12995,
    ACHIEV_CRIT_HOUSE_25H                  = 12998,

    ACHIEV_CRIT_IM_ON_A_BOAT_10N           = 12777,     // Gunship Battle, achievs 4536, 4612
    ACHIEV_CRIT_IM_ON_A_BOAT_25N           = 13080,
    ACHIEV_CRIT_IM_ON_A_BOAT_10H           = 13079,
    ACHIEV_CRIT_IM_ON_A_BOAT_25H           = 13081,

    ACHIEV_CRIT_MADE_A_MESS_10N            = 12778,     // Deathbringer Saurfang, achievs 4537, 4613
    ACHIEV_CRIT_MADE_A_MESS_25N            = 13036,
    ACHIEV_CRIT_MADE_A_MESS_10H            = 13035,
    ACHIEV_CRIT_MADE_A_MESS_25H            = 13037,

    ACHIEV_CRIT_FLU_SHOT_SHORTAGE_10N      = 12977,     // Festergut, achievs 4615, 4577
    ACHIEV_CRIT_FLU_SHOT_SHORTAGE_25N      = 12982,
    ACHIEV_CRIT_FLU_SHOT_SHORTAGE_10H      = 12986,
    ACHIEV_CRIT_FLU_SHOT_SHORTAGE_25H      = 12967,

    ACHIEV_CRIT_DANCES_WITH_OOZES_10N      = 12984,     // Rotface, achievs 4538, 4614
    ACHIEV_CRIT_DANCES_WITH_OOZES_25N      = 12966,
    ACHIEV_CRIT_DANCES_WITH_OOZES_10H      = 12985,
    ACHIEV_CRIT_DANCES_WITH_OOZES_25H      = 12983,

    ACHIEV_CRIT_NAUSEA_10N                 = 12987,     // Professor Putricide, achievs 4578, 4616
    ACHIEV_CRIT_NAUSEA_25N                 = 12968,
    ACHIEV_CRIT_NAUSEA_10H                 = 12988,
    ACHIEV_CRIT_NAUSEA_25H                 = 12981,

    ACHIEV_CRIT_ORB_WHISPERER_10N          = 13033,     // Blood Prince Council, achievs 4582, 4617
    ACHIEV_CRIT_ORB_WHISPERER_25N          = 12969,
    ACHIEV_CRIT_ORB_WHISPERER_10H          = 13034,
    ACHIEV_CRIT_ORB_WHISPERER_25H          = 13032,

    ACHIEV_CRIT_ONCE_BITTEN_TWICE_SHY_10N  = 12780,     // Blood-Queen Lana'thel, achievs 4539, 4618
    ACHIEV_CRIT_ONCE_BITTEN_TWICE_SHY_25N  = 13012,
    ACHIEV_CRIT_ONCE_BITTEN_TWICE_SHY_10V  = 13011,
    ACHIEV_CRIT_ONCE_BITTEN_TWICE_SHY_25V  = 13013,

    ACHIEV_CRIT_PORTAL_JOCKEY_10N          = 12978,    // Valithria, achievs 4579, 4619
    ACHIEV_CRIT_PORTAL_JOCKEY_25N          = 12971,
    ACHIEV_CRIT_PORTAL_JOCKEY_10H          = 12979,
    ACHIEV_CRIT_PORTAL_JOCKEY_25H          = 12980,

    ACHIEV_CRIT_ALL_YOU_CAN_EAT_10N        = 12822,    // Sindragosa, achievs 4580, 4620
    ACHIEV_CRIT_ALL_YOU_CAN_EAT_25N        = 12972,
    ACHIEV_CRIT_ALL_YOU_CAN_EAT_10V        = 12996,
    ACHIEV_CRIT_ALL_YOU_CAN_EAT_25V        = 12989,

    ACHIEV_CRIT_WAITING_A_LONG_TIME_10N    = 13246,    // Lich King, achievs 4601, 4621
    ACHIEV_CRIT_WAITING_A_LONG_TIME_25N    = 13244,
    ACHIEV_CRIT_WAITING_A_LONG_TIME_10H    = 13247,
    ACHIEV_CRIT_WAITING_A_LONG_TIME_25H    = 13245,

    ACHIEV_CRIT_NECK_DEEP_VILE_10N         = 12823,    // Lich King, achievs 4581, 4622
    ACHIEV_CRIT_NECK_DEEP_VILE_25N         = 13243,
    ACHIEV_CRIT_NECK_DEEP_VILE_10H         = 13163,
    ACHIEV_CRIT_NECK_DEEP_VILE_25H         = 13164,
};

struct sEventNpcSingleSpawnLocations
{
    uint32 uiEntry;
    float fSpawnX, fSpawnY, fSpawnZ, fSpawnO;
};

struct sEventNpcDualSpawnLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fSpawnX, fSpawnY, fSpawnZ, fSpawnO;
};

const sEventNpcDualSpawnLocations aSaurfangLocations[5] =
{
    {NPC_OVERLORD_SAURFANG,     NPC_MURADIN_BRONZEBEARD,      -555.958f, 2211.4f,  539.369f, 6.26573f},
    {NPC_KORKRON_REAVER_RISE,   NPC_SKYBREAKER_MARINE_RISE,   -560.399f, 2209.3f,  539.368f, 6.23082f},
    {NPC_KORKRON_REAVER_RISE,   NPC_SKYBREAKER_MARINE_RISE,   -557.936f, 2214.46f, 539.368f, 6.26573f},
    {NPC_KORKRON_REAVER_RISE,   NPC_SKYBREAKER_MARINE_RISE,   -557.958f, 2207.16f, 539.368f, 6.26573f},
    {NPC_KORKRON_REAVER_RISE,   NPC_SKYBREAKER_MARINE_RISE,   -560.451f, 2212.86f, 539.368f, 6.17846f},
};

class instance_icecrown_citadel : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_icecrown_citadel(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* pCreature) override;
        void OnObjectSpawn(GameObject* pGo) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        uint32 GetPlayerTeam() const { return m_uiTeam; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* strIn) override;

        void DoHandleCitadelAreaTrigger(uint32 uiTriggerId, Player* pPlayer);

        // Difficulty wrappers
        bool IsHeroicDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }
        bool Is25ManDifficulty() const { return instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC; }

        void GetDeathwhisperStalkersList(GuidList& lList) const { lList = m_lDeathwhisperStalkersGuids; }
        void GetRotfaceStalkersList(GuidList& lList) const { lList = m_lRotfaceUpperStalkersGuids; }
        ObjectGuid GetScientistStalkerGuid(bool bLeft) { return bLeft ? m_leftScientistStalkerGuid : m_rightScientistStalkerGuid; }

        // Open Putricide door in a few seconds
        void DoPreparePutricideDoor() { m_uiPutricideValveTimer = 15000; }

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget = nullptr, uint32 uiMiscvalue1 = 0) const override;

        bool CheckConditionCriteriaMeet(Player const* source, uint32 instance_condition_id, WorldObject const* conditionSource, uint32 conditionSourceType) const override;

        void Update(const uint32 diff) override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void JustDidDialogueStep(int32 iEntry) override;

        void ProcessEventNpcs(Player* pPlayer);

        std::string m_strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];

        uint32 m_uiTeam;                                    // Team of first entered player, used on the Gunship event
        uint32 m_uiPutricideValveTimer;

        bool m_bHasMarrowgarIntroYelled;
        bool m_bHasDeathwhisperIntroYelled;
        bool m_bHasRimefangLanded;
        bool m_bHasSpinestalkerLanded;

        ObjectGuid m_leftScientistStalkerGuid;
        ObjectGuid m_rightScientistStalkerGuid;

        GuidList m_lDeathwhisperStalkersGuids;
        GuidList m_lDeathwhisperCultistsGuids;
        GuidList m_lRotfaceUpperStalkersGuids;
        GuidList m_lFactionTeleporterGuids[PVP_TEAM_COUNT];
        GuidSet m_sDarkfallenCreaturesLowerGuids;
        GuidSet m_sDarkfallenCreaturesLeftGuids;
        GuidSet m_sDarkfallenCreaturesRightGuids;
};

#endif
