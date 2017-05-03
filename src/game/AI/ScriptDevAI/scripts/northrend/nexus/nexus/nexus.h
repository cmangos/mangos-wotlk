/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_NEXUS_H
#define DEF_NEXUS_H

enum
{
    MAX_ENCOUNTER                  = 4,
    MAX_SPECIAL_ACHIEV_CRITS       = 2,

    TYPE_TELESTRA                  = 0,
    TYPE_ANOMALUS                  = 1,
    TYPE_ORMOROK                   = 2,
    TYPE_KERISTRASZA               = 3,
    TYPE_INTENSE_COLD_FAILED       = 4,

    TYPE_ACHIEV_CHAOS_THEORY       = 0,
    TYPE_ACHIEV_SPLIT_PERSONALITY  = 1,

    NPC_TELESTRA                   = 26731,
    NPC_ANOMALUS                   = 26763,
    NPC_ORMOROK                    = 26794,
    NPC_KERISTRASZA                = 26723,

    GO_CONTAINMENT_SPHERE_TELESTRA = 188526,
    GO_CONTAINMENT_SPHERE_ANOMALUS = 188527,
    GO_CONTAINMENT_SPHERE_ORMOROK  = 188528,

    SPELL_FROZEN_PRISON             = 47854,

    ACHIEV_CRIT_CHAOS_THEORY        = 7316,                 // Anomalus, achiev 2037
    ACHIEV_CRIT_INTENSE_COLD        = 7315,                 // Keristrasza, achiev 2036
    ACHIEV_CRIT_SPLIT_PERSONALITY   = 7577,                 // Telestra, achiev 2150
};

class instance_nexus : public ScriptedInstance
{
    public:
        instance_nexus(Map* pMap);

        void Initialize() override;

        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        uint32 GetData(uint32 uiType) const override;
        void SetData(uint32 uiType, uint32 uiData) override;

        void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);
        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        const char* Save() const override { return m_strInstData.c_str(); }

        void Load(const char* chrIn) override;

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        bool m_abAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];

        std::set<uint32> m_sIntenseColdFailPlayers;
};

#endif
