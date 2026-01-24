/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BLOOD_FURNACE_H
#define DEF_BLOOD_FURNACE_H

enum
{
    MAX_ENCOUNTER                   = 3,
    FIRST_ORC_WAVE                  = 0,
    SECOND_ORC_WAVE                 = 1,
    THIRD_ORC_WAVE                  = 2,
    FOURTH_ORC_WAVE                 = 3,
    MAX_ORC_WAVES                   = 4,

    TYPE_THE_MAKER_EVENT            = 0,
    TYPE_BROGGOK_EVENT              = 1,
    TYPE_KELIDAN_EVENT              = 2,

    // NPC_THE_MAKER                = 17381,
    NPC_BROGGOK                     = 17380,
    NPC_KELIDAN_THE_BREAKER         = 17377,
    NPC_NASCENT_FEL_ORC             = 17398,                // Used in the Broggok event
    NPC_FEL_ORC_NEOPHYTE            = 17429,
    NPC_MAGTHERIDON                 = 21174,
    NPC_SHADOWMOON_CHANNELER        = 17653,

    GO_DOOR_FINAL_EXIT              = 181766,
    GO_DOOR_MAKER_FRONT             = 181811,
    GO_DOOR_MAKER_REAR              = 181812,
    GO_DOOR_BROGGOK_FRONT           = 181822,
    GO_DOOR_BROGGOK_REAR            = 181819,
    GO_DOOR_KELIDAN_EXIT            = 181823,

    // GO_PRISON_CELL_MAKER1        = 181813,               // The maker cell front right
    // GO_PRISON_CELL_MAKER2        = 181814,               // The maker cell back right
    // GO_PRISON_CELL_MAKER3        = 181816,               // The maker cell front left
    // GO_PRISON_CELL_MAKER4        = 181815,               // The maker cell back left

    GO_PRISON_CELL_BROGGOK_1        = 181817,               // Broggok cell back left   (NE)
    GO_PRISON_CELL_BROGGOK_2        = 181818,               // Broggok cell back right  (SE)
    GO_PRISON_CELL_BROGGOK_3        = 181820,               // Broggok cell front left  (NW)
    GO_PRISON_CELL_BROGGOK_4        = 181821,               // Broggok cell front right (SW)

    GO_CRACK_1                      = 181921,
    GO_CRACK_2                      = 181922,
    GO_CRACK_3                      = 181923,
    GO_CRACK_4                      = 181924,
    GO_CRACK_5                      = 181925,
    GO_CRACK_6                      = 181926,
    GO_CRACK_7                      = 181927,

    GO_PRISON_CELL_DOOR_LEVER       = 181982,

    SPELL_COMBAT_TRIGGER            = 26837,

    NPC_IN_COMBAT_TRIGGER           = 16006,

    SAY_BROGGOK_INTRO               = 15115,

    // Prison Cell SpawnGroups
    SPAWN_GROUP_PRISON_CELL_1       = 5420044,              // Back Left
    SPAWN_GROUP_PRISON_CELL_2       = 5420045,              // Back Right
    SPAWN_GROUP_PRISON_CELL_3       = 5420042,              // Front Left
    SPAWN_GROUP_PRISON_CELL_4       = 5420043,              // Front Right    
};

const std::string FIRST_BROGGOK_CELL_STRING = "BF_PRISON_CELL_GROUP_01";
const std::string SECOND_BROGGOK_CELL_STRING = "BF_PRISON_CELL_GROUP_02";
const std::string THIRD_BROGGOK_CELL_STRING = "BF_PRISON_CELL_GROUP_03";
const std::string FOURTH_BROGGOK_CELL_STRING = "BF_PRISON_CELL_GROUP_04";
const std::string FIFTH_BROGGOK_CELL_STRING = "BF_PRISON_CELL_GROUP_05";

// Random Magtheridon taunt
static const int32 aRandomTaunt[] = {17339, 17340, 17341, 17342, 17343, 17344};

struct BroggokEventInfo
{
    BroggokEventInfo() : m_bIsCellOpened(false) {}

    ObjectGuid m_cellGuid;
    bool m_bIsCellOpened;
};

class instance_blood_furnace : public ScriptedInstance
{
    public:
        instance_blood_furnace(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* creature) override;
        void OnObjectCreate(GameObject* go) override;

        void OnCreatureRespawn(Creature* creature) override;

        void OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* pCreature) override;

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        void Update(const uint32 diff) override;

        void Load(const char* chrIn) override;
        const char* Save() const override { return m_strInstData.c_str(); }

        void GetKelidanAddList(GuidList& lList) { lList = m_lChannelersGuids; m_lChannelersGuids.clear(); }

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void DoNextBroggokEventPhase();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        BroggokEventInfo m_aBroggokEvent[MAX_ORC_WAVES];

        uint32 m_uiBroggokEventTimer;                       // Timer for opening the event cages; 90s on normal and heroic
        uint32 m_uiBroggokEventPhase;
        uint32 m_uiRandYellTimer;                           // Random yell for Magtheridon
        uint32 m_crackTimer;
        uint32 m_uiBroggokEventDelay;
        bool m_uiBroggokEventDelaySpawn;
        std::vector<uint32> m_broggokEventGuids;

        GameObject* m_lLeverGO;

        GuidList m_cracks;
        GuidList m_luiNascentOrcGuids;
        GuidList m_lChannelersGuids;
};

#endif
