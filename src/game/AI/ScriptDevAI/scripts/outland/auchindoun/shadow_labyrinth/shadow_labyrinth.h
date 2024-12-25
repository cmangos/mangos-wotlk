/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SHADOW_LABYRINTH_H
#define DEF_SHADOW_LABYRINTH_H

enum
{
    MAX_ENCOUNTER           = 4,

    TYPE_HELLMAW            = 1,
    // TYPE_OVERSEER           = 2,                            // obsolete id used by acid
    TYPE_INCITER            = 3,
    TYPE_VORPIL             = 4,
    TYPE_MURMUR             = 5,

    DATA_CABAL_RITUALIST    = 1,                            // DO NOT CHANGE! Used by Acid. - used to check the Cabal Ritualists alive

    NPC_HELLMAW             = 18731,
    NPC_VORPIL              = 18732,
    NPC_CABAL_RITUALIST     = 18794,
    NPC_BLACKHEART_THE_INCITER = 18667,
    NPC_CONTAINMENT_BEAM    = 21159,

    NPC_CABAL_SPELLBINDER   = 18639,
    NPC_CABAL_SUMMONER      = 18634,
    NPC_MURMUR              = 18708,

    GO_REFECTORY_DOOR       = 183296,                       // door opened when blackheart the inciter dies
    GO_SCREAMING_HALL_DOOR  = 183295,                       // door opened when grandmaster vorpil dies and player comes in range

    SPELL_BANISH            = 30231,                        // spell is handled in creature_template_addon;
    SPELL_SHAPE_OF_BEAST    = 33949,

    // Blackheart the Inciter
    SL_BLACKHEAT_GROUP_01   = 5550030,
    SL_BLACKHEAT_GROUP_02   = 5550031,
    SL_BLACKHEAT_GROUP_03   = 5550032,
    SL_BLACKHEAT_GROUP_04   = 5550033,
    SL_BLACKHEAT_GROUP_05   = 5550034,
    SL_BLACKHEAT_GROUP_06   = 5550035,
    SL_BLACKHEAT_GROUP_07   = 5550036,
    SL_BLACKHEAT_GROUP_08   = 5550037,
    SL_BLACKHEAT_GROUP_09   = 5550038,
    SL_BLACKHEAT_GROUP_10   = 5550039,
    SL_BLACKHEAT_GROUP_11   = 5550040,
    SL_BLACKHEAT_GROUP_12   = 5550041,
    SL_BLACKHEAT_GROUP_13   = 5550042,


    SL_SPAWN_GROUP_043      = 5550064,                      // SpawnGroup that stops respawning of first runner
    SL_SPAWN_GROUP_044      = 5550065,                      // SpawnGroup that stops respawning of 2nd runner
    SL_SPAWN_GROUP_045      = 5550066,                      // SpawnGroup that stops respawning of third runner
    SL_SPAWN_GROUP_046      = 5550067,                      // SpawnGroup that stops respawning of third runner
};

const std::string MURMURS_WRATH_TARGETS_01 = "SL_MURMUR_WRATH_TARGET_01";
const std::string MURMURS_WRATH_TARGETS_02 = "SL_MURMUR_WRATH_TARGET_02";
const std::string MURMURS_WRATH_TARGETS_03 = "SL_MURMUR_WRATH_TARGET_03";

class instance_shadow_labyrinth : public ScriptedInstance
{
    public:
        instance_shadow_labyrinth(Map* pMap);

        void Initialize() override;

        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* creature) override;

        void OnCreatureGroupDespawn(CreatureGroup* pGroup, Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void SetData64(uint32 uiType, uint64 uiGuid) override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        bool IsHellmawUnbanished() const { return m_sRitualistsAliveGUIDSet.empty(); }

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        GuidSet m_sRitualistsAliveGUIDSet;
};

#endif
