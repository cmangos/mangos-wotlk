/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ONYXIA_H
#define DEF_ONYXIA_H

enum
{
    TYPE_ONYXIA                 = 0,

    // Special data fields for Onyxia
    DATA_LIFTOFF                = 4,
    DATA_PLAYER_TOASTED         = 5,

    NPC_ONYXIA_WHELP            = 11262,
    NPC_ONYXIA_TRIGGER          = 12758,

    // Achievement Related
    TIME_LIMIT_MANY_WHELPS      = 10,                       // 10s timeframe to kill 50 whelps after liftoff
    ACHIEV_CRIT_REQ_MANY_WHELPS = 50,

    ACHIEV_CRIT_MANY_WHELPS_N   = 12565,                    // Achievements 4403, 4406
    ACHIEV_CRIT_MANY_WHELPS_H   = 12568,
    ACHIEV_CRIT_NO_BREATH_N     = 12566,                    // Acheivements 4404, 4407
    ACHIEV_CRIT_NO_BREATH_H     = 12569,

    ACHIEV_START_ONYXIA_ID      = 6601,
};

class instance_onyxias_lair : public ScriptedInstance
{
    public:
        instance_onyxias_lair(Map* pMap);
        ~instance_onyxias_lair() {}

        void Initialize() override;

        bool IsEncounterInProgress() const override;

        void OnCreatureCreate(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

    protected:
        uint32 m_uiEncounter;
        uint32 m_uiAchievWhelpsCount;

        time_t m_tPhaseTwoStart;
};

#endif
