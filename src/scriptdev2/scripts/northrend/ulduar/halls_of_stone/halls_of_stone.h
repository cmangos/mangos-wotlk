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

    // NPC_BRANN            = 28070,

    NPC_KADDRAK             = 30898,
    NPC_ABEDNEUM            = 30899,
    NPC_MARNAK              = 30897,
    NPC_TRIBUNAL_OF_AGES    = 28234,
    NPC_WORLDTRIGGER        = 22515,
    NPC_DARK_MATTER         = 28235,                        // used by the Tribunal event
    NPC_LIGHTNING_STALKER   = 28130,                        // used by the Tribunal event as spawn point for the dwarfs
    NPC_IRON_SLUDGE         = 28165,                        // checked in the Sjonnir achiev
    NPC_SJONNIR             = 27978,

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

    SPELL_DARK_MATTER_START = 51001,                        // Channeled spells used by the Tribunal event

    MAX_FACES               = 3,
    FACE_MARNAK             = 0,
    FACE_ABEDNEUM           = 1,
    FACE_KADDRAK            = 2,

    MAX_ACHIEV_SLUDGES      = 5,

    ACHIEV_START_MAIDEN_ID  = 20383,

    ACHIEV_CRIT_BRANN       = 7590,                         // Brann, achiev 2154
    ACHIEV_CRIT_ABUSE_OOZE  = 7593,                         // Snonnir, achiev 2155
};

struct Face
{
    Face() : m_bIsActive(false), m_uiTimer(1000) {}

    ObjectGuid m_leftEyeGuid;
    ObjectGuid m_rightEyeGuid;
    ObjectGuid m_goFaceGuid;
    ObjectGuid m_speakerGuid;
    bool m_bIsActive;
    uint32 m_uiTimer;
};

class instance_halls_of_stone : public ScriptedInstance
{
    public:
        instance_halls_of_stone(Map* pMap);
        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(uint32 uiDiff) override;

        bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/) const override;

        void ActivateFace(uint8 uiFace, bool bAfterEvent);
        void DoFaceSpeak(uint8 uiFace, int32 iTextId);
        void SetBrannSpankin(bool bIsMet) { m_bIsBrannSpankin = bIsMet; }

        ObjectGuid GetProtectorStalkerGuid() { return m_protectorStalkerGuid; }
        ObjectGuid GeStormcallerStalkerGuid() { return m_stormcallerStalkerGuid; }
        ObjectGuid GetCustodianStalkerGuid() { return m_custodianStalkerGuid; }

    private:
        void SortFaces();
        void ProcessFace(uint8 uiFace);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        Face m_aFaces[MAX_FACES];
        std::string m_strInstData;

        uint8 m_uiIronSludgeKilled;
        bool m_bIsBrannSpankin;

        ObjectGuid m_protectorStalkerGuid;
        ObjectGuid m_stormcallerStalkerGuid;
        ObjectGuid m_custodianStalkerGuid;

        GuidList m_lKaddrakGUIDs;
        GuidList m_lAbedneumGUIDs;
        GuidList m_lMarnakGUIDs;
        GuidList m_lTribunalGUIDs;
        GuidList m_lWorldtriggerGUIDs;
};

#endif
