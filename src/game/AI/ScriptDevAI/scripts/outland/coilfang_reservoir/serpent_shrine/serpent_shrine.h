/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SERPENT_SHRINE_H
#define DEF_SERPENT_SHRINE_H

#include "Chat/Chat.h"

enum
{
    MAX_ENCOUNTER                   = 7,
    MAX_SPELLBINDERS                = 3,

    TYPE_HYDROSS_EVENT              = 0,
    TYPE_KARATHRESS_EVENT           = 1,
    TYPE_LADYVASHJ_EVENT            = 2,
    TYPE_LEOTHERAS_EVENT            = 3,
    TYPE_MOROGRIM_EVENT             = 4,
    TYPE_THELURKER_EVENT            = 5,
    TYPE_COMBAT_MAX                 = 6,
    TYPE_LEOTHERAS_EVENT_DEMONS     = 6,

    DATA_WATERSTATE_EVENT           = 1,                    // DO NOT CHANGE! Used by Acid. - used to check the mobs for the water event.

    NPC_KARATHRESS                  = 21214,
    NPC_CARIBDIS                    = 21964,
    NPC_SHARKKIS                    = 21966,
    NPC_TIDALVESS                   = 21965,
    NPC_LEOTHERAS                   = 21215,
    NPC_LADYVASHJ                   = 21212,
    NPC_GREYHEART_SPELLBINDER       = 21806,
    NPC_HYDROSS_BEAM_HELPER         = 21933,
    NPC_SHIELD_GENERATOR            = 19870,
    NPC_TINY_TRIGGER                = 21987,
    NPC_WORLD_TRIGGER               = 21252,
    NPC_SEER_OLUM                   = 22820,
    NPC_LURKER_BELOW                = 21217,

    // waterstate event related
    NPC_COILFANG_PRIESTESS          = 21220,
    NPC_COILFANG_SHATTERER          = 21301,
    NPC_VASHJIR_HONOR_GUARD         = 21218,
    NPC_GREYHEART_TECHNICIAN        = 21263,
    NPC_COILFANG_FRENZY_CORPSE      = 21689,

    GO_SHIELD_GENERATOR_1           = 185051,
    GO_SHIELD_GENERATOR_2           = 185052,
    GO_SHIELD_GENERATOR_3           = 185053,
    GO_SHIELD_GENERATOR_4           = 185054,

    // Objects and doors no longer used since 2.4.0
    GO_CONSOLE_HYDROSS              = 185117,
    GO_CONSOLE_LURKER               = 185118,
    GO_CONSOLE_LEOTHERAS            = 185115,
    GO_CONSOLE_KARATHRESS           = 185114,
    GO_CONSOLE_MOROGRIM             = 185116,
    GO_CONSOLE_VASHJ                = 184568,
    GO_BRIDGE_PART_1                = 184203,
    GO_BRIDGE_PART_2                = 184204,
    GO_BRIDGE_PART_3                = 184205,

    GO_STRANGE_POOL                 = 184956,

    SPELL_LEOTHERAS_BANISH          = 37546,

    SPELL_LURKER_SPAWN_TRIGGER      = 54587,
};

class instance_serpentshrine_cavern : public ScriptedInstance
{
    public:
        instance_serpentshrine_cavern(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnPlayerResurrect(Player* player) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void SetData64(uint32 uiType, uint64 uiGuid) override;

        bool CheckConditionCriteriaMeet(Player const* pPlayer, uint32 uiInstanceConditionId, WorldObject const* pConditionSource, uint32 conditionSourceType) const override;

        void GetBeamHelpersGUIDList(GuidList& lList) const { lList = m_lBeamHelpersGUIDList; }
        void GetTinyTriggerGUIDList(GuidList& lList) const { lList = m_tinyTriggerGUIDList; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void SpawnFishCorpses();
        void EngageBridgeConsole(GameObject* _console = nullptr);
        void EngageBossConsole(uint32 uiType, GameObject* console);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiSpellBinderCount;

        GuidList m_lFishCorpsesGUIDList;
        GuidList m_lSpellBindersGUIDList;
        GuidList m_lBeamHelpersGUIDList;
        GuidList m_tinyTriggerGUIDList;
        GuidSet m_sPlatformMobsGUIDSet;
        GuidSet m_sPlatformMobsAliveGUIDSet;
        GuidVector m_vashjBridgeParts;
};

#endif
