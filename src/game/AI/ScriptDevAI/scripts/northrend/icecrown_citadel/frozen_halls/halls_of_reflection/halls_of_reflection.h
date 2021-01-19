/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_HOR_H
#define DEF_ICECROWN_HOR_H

enum
{
    MAX_ENCOUNTER                   = 6,

    TYPE_FROSTMOURNE_INTRO          = 0,
    TYPE_FALRIC                     = 1,
    TYPE_MARWYN                     = 2,
    TYPE_LICH_KING                  = 3,
    TYPE_FROSTWORN_GENERAL          = 4,
    TYPE_QUEL_DELAR                 = 5,

    NPC_FALRIC                      = 38112,
    NPC_MARWYN                      = 38113,
    NPC_LICH_KING                   = 36954,

    NPC_JAINA_PART1                 = 37221,
    NPC_JAINA_PART2                 = 36955,
    NPC_SYLVANAS_PART1              = 37223,
    NPC_SYLVANAS_PART2              = 37554,
    NPC_KILARA                      = 37583,
    NPC_ELANDRA                     = 37774,
    NPC_LORALEN                     = 37779,
    NPC_KORELN                      = 37582,

    // intro related npcs
    NPC_UTHER                       = 37225,
    NPC_LICH_KING_INTRO             = 37226,
    NPC_FROSTMOURNE_ALTAR_BUNNY     = 37704,                    // dummy trigger for Quel'Delar
    NPC_QUEL_DELAR                  = 37158,
    NPC_DUNGEON_TAP_CONTROLLER      = 37071,                    // casts 69843 on all players to maintain dungeon control

    // Frostworn general related
    NPC_DUNGEON_TRAP_STALKER        = 36736,
    NPC_FROSTSWORN_GENERAL          = 36723,                    // miniboss between Marwyn and Lich King
    NPC_SPIRITUAL_REFLECTION_1      = 37068,
    NPC_SPIRITUAL_REFLECTION_2      = 37107,

    // spirit event creatures
    NPC_PHANTOM_MAGE                = 38172,
    NPC_SPECTRAL_FOOTMAN            = 38173,
    NPC_GHOSTLY_PRIEST              = 38175,
    NPC_TORTURED_RIFLEMAN           = 38176,
    NPC_SHADOWY_MERCENARY           = 38177,

    // escape event creatures
    // NPC_RAGING_GHOUL             = 36940,
    // NPC_RISEN_WHITCH_DOCTOR      = 36941,
    NPC_LUMBERING_ABOMINATION       = 37069,
    // NPC_ICE_WALL_TARGET          = 37014,                    // dummy ice wall target

    // objects
    GO_ICECROWN_DOOR_ENTRANCE       = 201976,                   // entrance door; used in combat during the spirit event
    GO_IMPENETRABLE_DOOR            = 197341,                   // door after the spirit event
    GO_ICECROWN_DOOR_LK_ENTRANCE    = 197342,                   // door before the Lich King
    GO_ICECROWN_DOOR_LK_EXIT        = 197343,                   // door after the Lich King
    GO_FROSTMOURNE_ALTAR            = 202236,
    GO_FROSTMOURNE                  = 202302,

    GO_ICE_WALL                     = 201385,                   // summoned during the Lich King escape
    // GO_CAVE_IN                   = 201596,                   // door after the final encounter
    // GO_PORTAL_DALARAN            = 202079,
    GO_TRANSPORT_SKYBREAKER         = 201598,                   // alliance transport; map 712
    GO_TRANSPORT_OGRIMS_HAMMER      = 201599,                   // horde transport; map 713
    GO_GUNSHIP_STAIRS_A             = 201709,
    GO_GUNSHIP_STAIRS_H             = 202211,

    GO_CAPTAIN_CHEST_HORDE          = 202212,
    GO_CAPTAIN_CHEST_HORDE_H        = 202337,
    GO_CAPTAIN_CHEST_ALLIANCE       = 201710,
    GO_CAPTAIN_CHEST_ALLIANCE_H     = 202336,

    // spells
    SPELL_ICE_PRISON                = 69708,
    SPELL_DARK_BINDING              = 70194,
    SPELL_QUELDELAR_COMPULSION      = 70013,

    // world states
    WORLD_STATE_SPIRIT_WAVES        = 4884,
    WORLD_STATE_SPIRIT_WAVES_COUNT  = 4882,

    // area triggers
    AREATRIGGER_FROSTMOURNE_ALTAR   = 5697,
    AREATRIGGER_FROSTWORN_GENERAL   = 5740,
    AREATRIGGER_LICH_KING_ROOM      = 5605,
    AREATRIGGER_QUELDELAR_START     = 5660,

    // achievs
    ACHIEV_START_NOT_RETREATING_ID  = 22615,                    // Lich King escape timed achievs 4526
};

struct EventNpcLocations
{
    uint32 uiEntryHorde, uiEntryAlliance;
    float fX, fY, fZ, fO;
};

const EventNpcLocations aEventBeginLocations[2] =
{
    {NPC_SYLVANAS_PART1, NPC_JAINA_PART1,   5236.659f, 1929.894f, 707.7781f, 0.87f},
    {NPC_LORALEN,        NPC_KORELN,        5232.680f, 1931.460f, 707.7781f, 0.83f},
};

const EventNpcLocations aEventKingLocations[2] =
{
    {NPC_LICH_KING,      NPC_LICH_KING,     5552.930f, 2261.475f, 733.0110f, 3.89f},
    {NPC_SYLVANAS_PART2, NPC_JAINA_PART2,   5549.290f, 2257.353f, 733.0943f, 0.89f},
};

static const float afGeneralSpawnLoc[4] = { 5415.538f, 2117.842f, 707.778f, 3.944f };
static const float afUtherSpawnLoc[4] = {5301.767f, 1990.667f, 707.695f, 3.909f };

class instance_halls_of_reflection : public ScriptedInstance, private DialogueHelper
{
    public:
        instance_halls_of_reflection(Map* pMap);
        ~instance_halls_of_reflection() {}

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature) override;
        void OnCreatureDespawn(Creature* pCreature) override;
        void OnCreatureEnterCombat(Creature* pCreature) override;

        void OnPlayerEnter(Player* pPlayer) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        uint32 GetPlayerTeam() const { return m_uiTeam; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void GetDungeonTrapsGUIDList(GuidList& lList) const { lList = m_lDungeonTrapsGuids; }
        void GetReflectionsGUIDList(GuidList& lList) const { lList = m_lSpiritReflectionsGuids; }

        void Update(uint32 uiDiff);

        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    protected:
        void JustDidDialogueStep(int32 iEntry) override;
        void DoCleanupFrostmourneEvent();
        void DoSendNextSpiritWave();
        void DoSetupEscapeEvent(Player* pPlayer);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiTeam;                                    // Team of first entered player, used to set if Jaina or Silvana to spawn
        uint32 m_uiEventTimer;
        uint32 m_uiActivateTimer;
        uint32 m_uiEscapeResetTimer;
        uint32 m_uiShipUpdateTimer;
        uint32 m_uiSummonDelayTimer;
        uint8 m_uiEventStage;

        GuidList m_lRisenSpiritsGuids;
        GuidList m_lActiveSpiritsGuids;
        GuidList m_lDungeonTrapsGuids;
        GuidList m_lSpiritReflectionsGuids;
        GuidList m_lGunshipStairsAllyGuids;
        GuidList m_lGunshipStairsHordeGuids;
        GuidList m_lIceWallGuids;
};

#endif
