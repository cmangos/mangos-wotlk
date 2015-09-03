/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BLACK_TEMPLE_H
#define DEF_BLACK_TEMPLE_H

enum
{
    MAX_ENCOUNTER                   = 9,

    TYPE_NAJENTUS                   = 0,
    TYPE_SUPREMUS                   = 1,
    TYPE_SHADE                      = 2,
    TYPE_GOREFIEND                  = 3,
    TYPE_BLOODBOIL                  = 4,
    TYPE_RELIQUIARY                 = 5,
    TYPE_SHAHRAZ                    = 6,
    TYPE_COUNCIL                    = 7,
    TYPE_ILLIDAN                    = 8,

    // NPC_WARLORD_NAJENTUS         = 22887,
    // NPC_SUPREMUS                 = 22898,
    NPC_SHADE_OF_AKAMA              = 22841,
    NPC_AKAMA_SHADE                 = 22990,
    NPC_RELIQUARY_OF_SOULS          = 22856,
    NPC_ILLIDARI_COUNCIL            = 23426,
    NPC_COUNCIL_VOICE               = 23499,
    NPC_LADY_MALANDE                = 22951,
    NPC_ZEREVOR                     = 22950,
    NPC_GATHIOS                     = 22949,
    NPC_VERAS                       = 22952,
    NPC_AKAMA                       = 23089,
    NPC_MAIEV_SHADOWSONG            = 23197,
    NPC_ILLIDAN_STORMRAGE           = 22917,

    NPC_ASH_CHANNELER               = 23421,
    NPC_CREATURE_GENERATOR          = 23210,
    NPC_ILLIDAN_DOOR_TRIGGER        = 23412,
    NPC_GLAIVE_TARGET               = 23448,
    NPC_SPIRIT_OF_OLUM              = 23411,
    NPC_SPIRIT_OF_UDALO             = 23410,

    GO_NAJENTUS_GATE                = 185483,
    GO_SUPREMUS_DOORS               = 185882,
    GO_SHADE_OF_AKAMA               = 185478,
    GO_GOREFIEND_DOOR               = 186153,
    GO_GURTOGG_DOOR                 = 185892,
    GO_PRE_SHAHRAZ_DOOR             = 185479,
    GO_POST_SHAHRAZ_DOOR            = 185482,
    GO_PRE_COUNCIL_DOOR             = 185481,
    GO_COUNCIL_DOOR                 = 186152,
    GO_ILLIDAN_GATE                 = 185905,
    GO_ILLIDAN_DOOR_R               = 186261,
    GO_ILLIDAN_DOOR_L               = 186262,
};

class instance_black_temple : public ScriptedInstance
{
    public:
        instance_black_temple(Map* pMap);

        void Initialize() override;

        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        void GetChannelersGuidList(GuidList& lList) { lList = m_lChannelersGuidList; }
        void GetGeneratorGuidVector(GuidVector& vVector) { vVector = m_vCreatureGeneratorGuidVector; }
        void GetGlaiveTargetGuidVector(GuidVector& vVector) { vVector = m_vGlaiveTargetGuidVector; }

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:
        void DoOpenPreMotherDoor();
        void DoSpawnAkamaIfCan();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        GuidList m_lChannelersGuidList;
        GuidVector m_vCreatureGeneratorGuidVector;
        GuidVector m_vGlaiveTargetGuidVector;
};

#endif
