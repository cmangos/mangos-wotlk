/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_OBSIDIAN_SANCTUM_H
#define DEF_OBSIDIAN_SANCTUM_H

enum
{
    MAX_ENCOUNTER                           = 1,

    TYPE_SARTHARION_EVENT                   = 1,
    // internal used types for achievement
    TYPE_ALIVE_DRAGONS                      = 2,
    TYPE_VOLCANO_BLOW_FAILED                = 3,

    MAX_TWILIGHT_DRAGONS                    = 3,

    TYPE_PORTAL_TENEBRON                    = 0,
    TYPE_PORTAL_SHADRON                     = 1,
    TYPE_PORTAL_VESPERON                    = 2,

    NPC_SARTHARION                          = 28860,
    NPC_TENEBRON                            = 30452,
    NPC_SHADRON                             = 30451,
    NPC_VESPERON                            = 30449,

    // Vesperon related npcs
    NPC_DISCIPLE_OF_VESPERON                = 30858,        // Disciple of Vesperon
    NPC_ACOLYTE_OF_VESPERON                 = 31219,        // Acolyte of Vesperon - summoned during Sartharion event
    NPC_VESPERON_CONTROLLER                 = 30878,        // casts 58853 on all players
    NPC_VESPERON_CONTROLLER_DEBUFF_CLEAR    = 32694,        // cast 61190 and 61608 on players

    // Shadron related npcs
    NPC_DISCIPLE_OF_SHADRON                 = 30688,        // Disciple of Shadron
    NPC_ACOLYTE_OF_SHADRON                  = 31218,        // Acolyte of Shadron - summoned during Sartharion event
    NPC_SHADRON_PORTAL                      = 30741,
    NPC_SHADRON_PORTAL_VISUAL               = 30650,

    // Tenebron related npcs
    NPC_TWILIGHT_EGG                        = 30882,        // Twilight Egg - summoned during Tenebron event
    NPC_SARTHARION_TWILIGHT_EGG             = 31204,        // Twilight Egg - summoned during Sartharion event
    NPC_TENEBRON_EGG_CONTROLLER             = 31138,        // not clear how to use this; used only to eject players to normal realm

    NPC_TWILIGHT_WHELP                      = 30890,
    NPC_SHARTHARION_TWILIGHT_WHELP          = 31214,

    NPC_FIRE_CYCLONE                        = 30648,

    GO_TWILIGHT_PORTAL                      = 193988,

    // Achievement related
    ACHIEV_CRIT_VOLCANO_BLOW_N              = 7326,         // achievs 2047, 2048 (Go When the Volcano Blows) -- This is individual achievement!
    ACHIEV_CRIT_VOLCANO_BLOW_H              = 7327,
    ACHIEV_DRAGONS_ALIVE_1_N                = 7328,         // achievs 2049, 2052 (Twilight Assist)
    ACHIEV_DRAGONS_ALIVE_1_H                = 7331,
    ACHIEV_DRAGONS_ALIVE_2_N                = 7329,         // achievs 2050, 2053 (Twilight Duo)
    ACHIEV_DRAGONS_ALIVE_2_H                = 7332,
    ACHIEV_DRAGONS_ALIVE_3_N                = 7330,         // achievs 2051, 2054 (The Twilight Zone)
    ACHIEV_DRAGONS_ALIVE_3_H                = 7333,
};

class instance_obsidian_sanctum : public ScriptedInstance
{
    public:
        instance_obsidian_sanctum(Map* map);

        void Initialize() override;

        void OnCreatureCreate(Creature* creature) override;

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        ObjectGuid SelectRandomFireCycloneGuid();

        bool IsActivePortal()
        {
            for (bool i : m_portalActive)
            {
                if (i)
                    return true;
            }

            return false;
        }

        void SetPortalStatus(uint8 type, bool status) { m_portalActive[type] = status; }
        bool GetPortaStatus(uint8 type) { return m_portalActive[type]; }

        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscValue1 /* = 0*/) const override;
        bool CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const override;

    private:
        uint32 m_encounter[MAX_ENCOUNTER];
        bool m_portalActive[MAX_TWILIGHT_DRAGONS];

        uint8 m_aliveDragons;

        std::set<uint32> m_volcanoBlowFailPlayers;

        GuidList m_fireCycloneGuidList;
};

#endif
