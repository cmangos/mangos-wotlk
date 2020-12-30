/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_EYE_ETERNITY_H
#define DEF_EYE_ETERNITY_H

enum
{
    TYPE_MALYGOS                    = 0,

    NPC_MALYGOS                     = 28859,
    NPC_ALEXSTRASZA                 = 32295,
    NPC_ALEXSTRASZA_INVIS           = 31253,            // trigger used during the encounter
    NPC_LARGE_TRIGGER               = 22517,
    // NPC_ALEXSTRASZAS_GIFT        = 32448,

    NPC_NEXUS_LORD                  = 30245,            // spawned by DB as vehicle passenger
    NPC_SCION_OF_ETERNITY           = 30249,            // spawned by DB as vehicle passenger
    NPC_HOVER_DISK_LORD             = 30234,            // this disk can be used by players
    NPC_HOVER_DISK_SCION            = 30248,
    NPC_ARCANE_OVERLOAD             = 30282,            // summoned by spell 56429

    NPC_SURGE_OF_POWER              = 30334,            // involved in the surge of power pulse event

    GO_EXIT_PORTAL                  = 193908,
    GO_PLATFORM                     = 193070,
    GO_FOCUSING_IRIS                = 193958,
    GO_FOCUSING_IRIS_H              = 193960,

    GO_HEART_OF_MAGIC               = 194158,
    GO_HEART_OF_MAGIC_H             = 194159,
    GO_ALEXSTRASZAS_GIFT            = 193905,
    GO_ALEXSTRASZAS_GIFT_H          = 193967,

    ACHIEV_START_MALYGOS_ID         = 20387,
};

class instance_eye_of_eternity : public ScriptedInstance
{
    public:
        instance_eye_of_eternity(Map* pMap);
        ~instance_eye_of_eternity() {}

        void Initialize() override;

        bool IsEncounterInProgress() const override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;

        void Update(const uint32 diff) override;

    protected:

        uint32 m_uiEncounter;

        uint32 m_uiMalygosResetTimer;
        uint32 m_uiMalygosCompleteTimer;

        GuidList m_lDiskRidersGuids;
        GuidList m_lSecondPhaseCreaturesGuids;
};

#endif
