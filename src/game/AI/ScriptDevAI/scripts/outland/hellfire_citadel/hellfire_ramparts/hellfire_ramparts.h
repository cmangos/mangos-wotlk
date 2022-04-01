/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_RAMPARTS_H
#define DEF_RAMPARTS_H

enum
{
    MAX_ENCOUNTER               = 2,

    TYPE_VAZRUDEN               = 1,
    TYPE_NAZAN                  = 2,                        // Do not change, used in ACID (SetData(SPECIAL) on death of 17517

    NPC_HELLFIRE_SENTRY         = 17517,
    NPC_VAZRUDEN_HERALD         = 17307,
    NPC_VAZRUDEN                = 17537,

    GO_FEL_IRON_CHEST           = 185168,
    GO_FEL_IRON_CHEST_H         = 185169,
};

class instance_ramparts : public ScriptedInstance
{
    public:
        instance_ramparts(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureRespawn(Creature* creature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        // No need to save and load this instance (only one encounter needs special handling, no doors used)

    private:
        void DoFailVazruden();

        uint32 m_auiEncounter[MAX_ENCOUNTER];

        uint32 m_uiSentryCounter;
        CreatureGroup* m_sentryGroup;
};

#endif
