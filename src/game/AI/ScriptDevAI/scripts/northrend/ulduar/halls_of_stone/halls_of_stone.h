/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_HALLS_OF_STONE_H
#define DEF_HALLS_OF_STONE_H

enum
{
    MAX_ENCOUNTER           = 4,

    TYPE_TRIBUNAL           = 0,
    TYPE_MAIDEN             = 1,
    TYPE_KRYSTALLUS         = 2,
    TYPE_SJONNIR            = 3,

    NPC_BRANN               = 28070,

    NPC_KADDRAK             = 30898,
    NPC_ABEDNEUM            = 30899,
    NPC_MARNAK              = 30897,

    NPC_TRIBUNAL_OF_AGES    = 28234,
    NPC_WORLDTRIGGER        = 22515,
    NPC_DARK_MATTER         = 28235,                        // used by the Tribunal event
    NPC_LIGHTNING_STALKER   = 28130,                        // used by the Tribunal event as spawn point for the dwarfs
    NPC_DARK_MATTER_TARGET  = 28237,
    NPC_SEARING_GAZE_TARGET = 28265,

    NPC_RUNE_PROTECTOR      = 27983,                        // mobs summoned during Tribunal event
    NPC_RUNE_STORMCALLER    = 27984,
    NPC_GOLEM_CUSTODIAN     = 27985,

    // Sjonnir mobs
    NPC_SJONNIR             = 27978,
    NPC_IRON_TROGG          = 27979,
    NPC_IRON_DWARF          = 27982,
    NPC_MALFORMED_OOZE      = 27981,
    NPC_EARTHEN_DWARF       = 27980,
    NPC_IRON_SLUDGE         = 28165,                        // checked in the Sjonnir achiev

    NPC_MAIDEN_GRIEF        = 27975,
    NPC_KRYSTALLUS          = 27977,

    GO_DOOR_MAIDEN          = 191292,
    GO_DOOR_TRIBUNAL        = 191294,                       // possibly closed during event?
    GO_DOOR_TO_TRIBUNAL     = 191295,
    GO_DOOR_SJONNIR         = 191296,

    GO_TRIBUNAL_CHEST       = 190586,
    GO_TRIBUNAL_CHEST_H     = 193996,

    GO_TRIBUNAL_HEAD_RIGHT  = 191670,                       // marnak
    GO_TRIBUNAL_HEAD_CENTER = 191669,                       // abedneum
    GO_TRIBUNAL_HEAD_LEFT   = 191671,                       // kaddrak

    GO_TRIBUNAL_CONSOLE     = 193907,
    GO_TRIBUNAL_FLOOR       = 191527,

    GO_SJONNIR_CONSOLE      = 193906,
    // GO_LEFT_PIPE         = 192163,                       // pipes used for animation purpose only
    // GO_RIGHT_PIPE        = 192164,

    SPELL_DARK_MATTER_START = 51001,                        // Channeled spells used by the Tribunal event

    // SPELL_TAUNT_BRANN    = 51774,                        // cast by the summoned mobs during the Tribunal event; handled in EAI
    SPELL_BRANN_STEALTH     = 58506,

    MAX_FACES               = 3,
    FACE_MARNAK             = 0,
    FACE_ABEDNEUM           = 1,
    FACE_KADDRAK            = 2,

    MAX_ACHIEV_SLUDGES      = 5,

    ACHIEV_START_MAIDEN_ID  = 20383,

    ACHIEV_CRIT_BRANN       = 7590,                         // Brann, achiev 2154
    ACHIEV_CRIT_ABUSE_OOZE  = 7593,                         // Snonnir, achiev 2155
};

static const float fBrannDoorLocation[4] = { 1199.685f, 667.15497f, 196.32364f, 3.12413f };

struct Face
{
    Face() : m_bIsActive(false), m_uiTimer(1000), m_uiSummonTimer(10000) {}

    ObjectGuid m_leftEyeGuid;
    ObjectGuid m_rightEyeGuid;
    ObjectGuid m_goFaceGuid;
    ObjectGuid m_speakerGuid;
    ObjectGuid m_summonerGuid;

    bool m_bIsActive;

    uint32 m_uiTimer;
    uint32 m_uiSummonTimer;
};

class instance_halls_of_stone : public ScriptedInstance
{
    public:
        instance_halls_of_stone(Map* pMap);
        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;
        void OnCreatureRespawn(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        void ShowChatCommands(ChatHandler* handler) override;
        void ExecuteChatCommand(ChatHandler* handler, char* args) override;

        void ActivateFace(uint8 uiFace, bool bAfterEvent);
        void ResetFace(uint8 uiFace);
        void SetFaceTimer(uint8 uiFace, uint32 uiTimer);
        void SetBrannSpankin(bool bIsMet) { m_bIsBrannSpankin = bIsMet; }
        ObjectGuid GetBrannSummonedGuid() { return m_brannSummonedGuid; }

    private:
        void SortFaces();
        void ProcessFaceSpell(uint8 uiFace);
        void ProcessFaceSummon(uint8 uiFace);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        Face m_aFaces[MAX_FACES];
        std::string m_strInstData;

        uint8 m_uiIronSludgeKilled;
        bool m_bIsBrannSpankin;

        uint32 m_uiLightningChannelTimer;

        ObjectGuid m_protectorStalkerGuid;
        ObjectGuid m_stormcallerStalkerGuid;
        ObjectGuid m_custodianStalkerGuid;
        ObjectGuid m_tribunalCasterGuid;
        ObjectGuid m_brannSummonedGuid;

        GuidList m_lKaddrakGUIDs;
        GuidList m_lAbedneumGUIDs;
        GuidList m_lMarnakGUIDs;
        GuidList m_lTribunalGUIDs;
        GuidList m_lWorldtriggerGUIDs;
        GuidList m_lRuneDwarfGUIDs;
};

#endif
