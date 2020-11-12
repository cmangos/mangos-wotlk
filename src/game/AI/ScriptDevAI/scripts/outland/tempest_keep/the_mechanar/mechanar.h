/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_MECHANAR_H
#define DEF_MECHANAR_H

enum
{
    MAX_ENCOUNTER           = 6,
    MAX_BRIDGE_LOCATIONS    = 7,
    MAX_BRIDGE_TRASH        = 4,

    TYPE_GYRO_KILL          = 0,
    TYPE_IRON_HAND          = 1,
    TYPE_CAPACITUS          = 2,
    TYPE_SEPETHREA          = 3,
    TYPE_BRIDGEEVENT        = 4,
    TYPE_PATHALEON          = 5,

    NPC_GYRO_KILL           = 19218,
    NPC_IRON_HAND           = 19710,
    NPC_LORD_CAPACITUS      = 19219,
    // NPC_SEPETHREA         = 19221,
    NPC_PATHALEON           = 19220,

    // bridge event related
    NPC_ASTROMAGE           = 19168,
    NPC_PHYSICIAN           = 20990,
    NPC_CENTURION           = 19510,
    NPC_ENGINEER            = 20988,
    NPC_NETHERBINDER        = 20059,
    NPC_FORGE_DESTROYER     = 19735,

    GO_MOARG_DOOR_1         = 184632,
    GO_MOARG_DOOR_2         = 184322,
    // GO_FACTORY_ELEVATOR  = 183788,
    GO_NETHERMANCER_DOOR    = 184449,
    GO_CACHE_OF_THE_LEGION  = 184465,
    GO_CACHE_OF_THE_LEGION_H= 184849,

    SPELL_ETHEREAL_TELEPORT = 34427,

    SAY_PATHALEON_INTRO     = -1554028,
};

struct SpawnLocation
{
    uint32 m_uiSpawnEntry;
    float m_fX, m_fY, m_fZ, m_fO;
};

static const SpawnLocation aBridgeEventLocs[MAX_BRIDGE_LOCATIONS][4] =
{
    {
        {NPC_ASTROMAGE,     243.9323f, -24.53621f, 26.3284f, 0},
        {NPC_ASTROMAGE,     240.5847f, -21.25438f, 26.3284f, 0},
        {NPC_PHYSICIAN,     238.4178f, -25.92982f, 26.3284f, 0},
        {NPC_CENTURION,     237.1122f, -19.14261f, 26.3284f, 0},
    },
    {
        {NPC_FORGE_DESTROYER, 199.945f, -22.85885f, 24.95783f, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {NPC_ENGINEER,      179.8642f, -25.84609f, 24.8745f, 0},
        {NPC_ENGINEER,      181.9983f, -17.56084f, 24.8745f, 0},
        {NPC_PHYSICIAN,     183.4078f, -22.46612f, 24.8745f, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {NPC_ENGINEER,      141.0496f, 37.86048f, 24.87399f, 4.65f},
        {NPC_ASTROMAGE,     137.6626f, 34.89631f, 24.8742f,  4.65f},
        {NPC_PHYSICIAN,     135.3587f, 38.03816f, 24.87417f, 4.65f},
        {0, 0, 0, 0, 0},
    },
    {
        {NPC_FORGE_DESTROYER, 137.8275f, 53.18128f, 24.95783f, 4.65f},
        {0, 0, 0, 0, 0},
    },
    {
        {NPC_PHYSICIAN,     134.3062f, 109.1506f, 26.45663f, 4.65f},
        {NPC_ASTROMAGE,     135.3307f, 99.96439f, 26.45663f, 4.65f},
        {NPC_NETHERBINDER,  141.3976f, 102.7863f, 26.45663f, 4.65f},
        {NPC_ENGINEER,      140.8281f, 112.0363f, 26.45663f, 4.65f},
    },
    {
        {NPC_PATHALEON,     139.5425f, 149.3192f, 25.65904f, 4.63f},
        {0, 0, 0, 0, 0},
    },
};

class instance_mechanar : public ScriptedInstance
{
    public:
        instance_mechanar(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

    private:
        void DoSpawnBridgeWave();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiBridgeEventTimer;
        uint8 m_uiBridgeEventPhase;
        uint32 m_uiPathaleonEngageTimer;

        GuidSet m_sBridgeTrashGuidSet;
};

#endif
