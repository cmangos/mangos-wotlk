/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_OLD_HILLSBRAD_H
#define DEF_OLD_HILLSBRAD_H

#include "World/WorldStateDefines.h"

enum
{
    MAX_ENCOUNTER                   = 7,
    MAX_BARRELS                     = 5,
    MAX_WIPE_COUNTER                = 20,

    TYPE_BARREL_DIVERSION           = 0,                    // barrel event
    TYPE_DRAKE                      = 1,                    // first boss
    TYPE_SKARLOC                    = 2,                    // prison to keep - boss
    TYPE_ESCORT_BARN                = 3,                    // keep to barn
    TYPE_ESCORT_INN                 = 4,                    // barn to inn
    TYPE_EPOCH                      = 5,                    // inn event
    TYPE_THRALL_EVENT               = 6,                    // global event

    // event npcs
    NPC_THRALL                      = 17876,
    NPC_TARETHA                     = 18887,
    NPC_EROZION                     = 18723,
    NPC_LODGE_QUEST_TRIGGER         = 20155,
    NPC_ARMORER                     = 18764,
    NPC_ORC_PRISONER                = 18598,

    NPC_TARREN_MILL_GUARDSMAN       = 18092,
    NPC_TARREN_MILL_PROTECTOR       = 18093,
    NPC_TARREN_MILL_LOOKOUT         = 18094,
    NPC_YOUNG_BLANCHY               = 18651,

    NPC_BARN_GUARDSMAN              = 23175,
    NPC_BARN_PROTECTOR              = 23179,
    NPC_BARN_LOOKOUT                = 23177,

    // bosses
    NPC_DRAKE                       = 17848,
    NPC_SKARLOC                     = 17862,
    NPC_EPOCH                       = 18096,

    GO_ROARING_FLAME                = 182592,
    GO_PRISON_DOOR                  = 184393,

    QUEST_ENTRY_HILLSBRAD           = 10282,
    QUEST_ENTRY_DIVERSION           = 10283,
    QUEST_ENTRY_ESCAPE              = 10284,
    QUEST_ENTRY_RETURN              = 10285,

    // RP scripts
    NPC_BARTOLO                     = 20365,
    NPC_BEGGAR                      = 20432,
};

static const float afInstanceLoc[][4] =
{
    {2104.51f, 91.96f, 53.14f, 0},                  // right orcs outside loc
    {2192.58f, 238.44f, 52.44f, 0},                 // left orcs outside loc
};

static const float aDrakeSummonLoc[4] = {2170.7588f, 146.05963f, 87.63939f, 4.19093f};

class instance_old_hillsbrad : public ScriptedInstance
{
    public:
        instance_old_hillsbrad(Map* pMap);
        ~instance_old_hillsbrad() {}

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        uint32 GetThrallEventCount() const { return m_uiThrallEventCount; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

        TimePoint m_beggarScriptTime;

    protected:
        void HandleThrallRelocation();
        void UpdateLodgeQuestCredit();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiBarrelCount;
        uint32 m_uiThrallEventCount;
        uint32 m_uiThrallResetTimer;

        GuidList m_lRoaringFlamesList;
        GuidList m_lLeftPrisonersList;
        GuidList m_lRightPrisonersList;
};

#endif
