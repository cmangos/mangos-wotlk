/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_GRUULS_LAIR_H
#define DEF_GRUULS_LAIR_H

enum
{
    MAX_ENCOUNTER                   = 2,
    MAX_COUNCIL                     = 5,

    // Encounter Status
    TYPE_MAULGAR_EVENT              = 0,
    TYPE_GRUUL_EVENT                = 1,

    GO_PORT_GRONN_1                 = 183817,               // 184468 not in use
    GO_PORT_GRONN_2                 = 184662,

    // NPC GUIDs
    NPC_MAULGAR                     = 18831,
    // NPC_BLINDEYE                  = 18836,
    // NPC_KIGGLER                   = 18835,
    // NPC_KROSH                     = 18832,
    // NPC_OLM                       = 18834,
};

class instance_gruuls_lair : public ScriptedInstance
{
    public:
        instance_gruuls_lair(Map* pMap);

        void Initialize() override;
        bool IsEncounterInProgress() const override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strSaveData.c_str(); }
        void Load(const char* chrIn) override;

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strSaveData;

        uint8 m_uiCouncilMembersDied;
};

#endif
