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
    NPC_AKAMA_SHADE                 = 23191,
    NPC_TERON_GOREFIEND             = 22871,
    NPC_RELIQUARY_OF_SOULS          = 22856,
    NPC_RELIQUARY_COMBAT_TRIGGER    = 23417,
    NPC_ILLIDARI_COUNCIL            = 23426,
    NPC_COUNCIL_VOICE               = 23499,
    NPC_LADY_MALANDE                = 22951,
    NPC_ZEREVOR                     = 22950,
    NPC_GATHIOS                     = 22949,
    NPC_VERAS                       = 22952,
    NPC_AKAMA                       = 23089,
    NPC_MAIEV_SHADOWSONG            = 23197,
    NPC_ILLIDAN_STORMRAGE           = 22917,
    NPC_SKY_STALKER                 = 23030,
    NPC_WIND_REAVER                 = 23330,

    NPC_ASH_CHANNELER               = 23421,
    NPC_CREATURE_GENERATOR          = 23210,
    NPC_ILLIDAN_DOOR_TRIGGER        = 23412,
    NPC_GLAIVE_TARGET               = 23448,
    NPC_SPIRIT_OF_OLUM              = 23411,
    NPC_SPIRIT_OF_UDALO             = 23410,
    NPC_BLACK_TEMPLE_TRIGGER        = 22984,
    NPC_SHADOWY_CONSTRUCT           = 23111,
    NPC_VENGEFUL_SPIRIT             = 23109,
    NPC_WORLD_TRIGGER               = 22515,

    // Ashtongue
    NPC_ASHTONGUE_BATTLELORD        = 22844,
    NPC_ASHTONGUE_MYSTIC            = 22845,
    NPC_ASHTONGUE_STORMCALLER       = 22846,
    NPC_ASHTONGUE_PRIMALIST         = 22847,
    NPC_STORM_FURY                  = 22848,
    NPC_ASHTONGUE_FERAL_SPIRIT      = 22849,
    NPC_ASHTONGUE_STALKER           = 23374,

    // RoS Spirits
    NPC_ANGERED_SOUL_FRAGMENT       = 23398,
    NPC_SUFFERING_SOUL_FRAGMENT     = 23399,
    NPC_HUNGERING_SOUL_FRAGMENT     = 23401,

    // RoS Adds
    NPC_ENSLAVED_SOUL               = 23469,

    // Illidan spawns
    NPC_ILLIDARI_ELITE              = 23226,
    NPC_FLAME_CRASH                 = 23336,
    NPC_PARASITIC_SHADOWFIEND       = 23498,
    NPC_BLADE_OF_AZZINOTH           = 22996,
    NPC_FLAME_OF_AZZINOTH           = 22997,
    NPC_ILLIDAN_TARGET              = 23070,
    NPC_DEMON_FIRE                  = 23069,
    NPC_BLAZE                       = 23259,
    NPC_SHADOW_DEMON                = 23375,
    NPC_CAGE_TRAP_DISTURB_TRIGGER   = 23304,

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

    GO_IMPALING_SPINE               = 185584,

    EMOTE_OPEN_NAJENTUS_DOOR        = -1564137,
    EMOTE_OPEN_MOTHER_DOOR          = -1564139,

    FACTION_ASHTONGUE_FRIENDLY      = 1820,
};

class instance_black_temple : public ScriptedInstance
{
    public:
        instance_black_temple(Map* map);

        void Initialize() override;

        bool IsEncounterInProgress() const override;

        void OnPlayerEnter(Player* player) override;
        void OnPlayerResurrect(Player* player) override;
        void OnCreatureCreate(Creature* creature) override;
        void OnCreatureRespawn(Creature* creature) override;
        void OnObjectCreate(GameObject* go) override;
        void OnCreatureEvade(Creature* creature) override;
        void OnCreatureDeath(Creature* creature) override;

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        GuidVector const& GetChannelersGuidList() const { return m_channelersGuidVector; }
        GuidVector const& GetGeneratorGuidVector() const { return m_creatureGeneratorGuidVector; }
        GuidVector& GetShadowyConstructGuidVector() { return m_shadowyConstructs; } // alterable
        void GetGlaiveTargetGuidVector(GuidVector& vector) const { vector = m_vGlaiveTargetGuidVector; }
        GuidVector& GetIllidanSpawns() { return m_illidanSpawns; }
        GuidVector& GetIllidanTriggersLower() { return m_illidanTriggersLower; }
        GuidVector& GetIllidanTriggersUpper() { return m_illidanTriggersUpper; }
        GuidVector& GetEnslavedSouls() { return m_enslavedSouls; }

        void DespawnImpalingSpines();
        void RespawnChannelers();

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void DoTeleportAkamaIfCan();
    private:
        void DoOpenPreMotherDoor();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        GuidVector m_channelersGuidVector;
        GuidVector m_creatureGeneratorGuidVector;
        GuidVector m_vGlaiveTargetGuidVector;
        GuidVector m_ashtongue;
        GuidVector m_impalingSpines;
        GuidVector m_shadowyConstructs;
        GuidVector m_soulFragments;
        GuidVector m_enslavedSouls;
        bool m_akamaIllidanSequence;

        GuidVector m_illidanSpawns;
        GuidVector m_illidanTriggersLower;
        GuidVector m_illidanTriggersUpper;
};

#endif
