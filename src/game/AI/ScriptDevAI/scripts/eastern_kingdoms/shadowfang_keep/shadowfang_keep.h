/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SHADOWFANG_H
#define DEF_SHADOWFANG_H

enum
{
    MAX_ENCOUNTER           = 7,

    TYPE_FREE_NPC           = 1,
    TYPE_RETHILGORE         = 2,
    TYPE_FENRUS             = 3,
    TYPE_NANDOS             = 4,
    TYPE_INTRO              = 5,
    TYPE_VOIDWALKER         = 6,
    TYPE_APOTHECARY         = 7,

    SAY_BOSS_DIE_AD         = -1033007,
    SAY_BOSS_DIE_AS         = -1033008,
    YELL_PACK_DEAD          = -1033026,
    SAY_ARUGAL_INTRO_1      = -1033009,
    SAY_ARUGAL_INTRO_2      = -1033010,
    SAY_ARUGAL_INTRO_3      = -1033011,
    SAY_ARUGAL_INTRO_4      = -1033012,

    YELL_FENRUS             = -1033013,
    YELL_AGGRO              = -1033017,
    YELL_KILLED_PLAYER      = -1033018,
    YELL_WORGEN_CURSE       = -1033019,

    SPELL_SPAWN             = 7741,
    SPELL_ARUGAL_TELEPORT   = 7136,
    SPELL_FIRE              = 6422,

    NPC_ASH                 = 3850,
    NPC_ADA                 = 3849,
    GOSSIP_ASH              = 798,
    GOSSIP_ADA              = 799,

    NPC_ARUGAL              = 10000,                        // "Arugal" says intro text
    NPC_ARCHMAGE_ARUGAL     = 4275,                         // "Archmage Arugal" does Fenrus event
    NPC_FENRUS              = 4274,                         // used to summon Arugal in Fenrus event
    NPC_VINCENT             = 4444,                         // Vincent should be "dead" is Arugal is done the intro already

    NPC_HUMMEL              = 36296,                        // Love is in the Air event
    NPC_FRYE                = 36272,
    NPC_BAXTER              = 36565,
    NPC_CROWN_APOTHECARY    = 36885,
    NPC_VALENTINE_BOSS_MGR  = 36643,                        // controller npc for the apothecary event
    NPC_APOTHECARY_GENERATOR = 36212,                       // the npc which summons the crazed apothecary
    NPC_CRAZED_APOTHECARY   = 36568,                        // casts spell 68957 on range check
    NPC_VALENTINE_VIAL_BUNNY = 36530,

    // Wolf Master Nandos intro event
    NPC_MASTER_NANDOS       = 3927,
    NPC_LUPINE_HORROR       = 3863,
    NPC_WOLFGUARD_WORG      = 5058,
    NPC_BLEAK_WORG          = 3861,
    NPC_SLAVERING_WORG      = 3862,

    GO_COURTYARD_DOOR       = 18895,                        // door to open when talking to NPC's
    GO_SORCERER_DOOR        = 18972,                        // door to open when Fenrus the Devourer
    GO_ARUGAL_DOOR          = 18971,                        // door to open when Wolf Master Nandos
    GO_ARUGAL_FOCUS         = 18973,                        // this generates the lightning visual in the Fenrus event

    GO_APOTHECARE_VIALS     = 190678,                       // move position for Baxter
    GO_CHEMISTRY_SET        = 200335,                       // move position for Frye

    MAX_APOTHECARY          = 3,
};

struct Waypoint
{
    float fX, fY, fZ;
};

enum {
    VINCENT_DEATH = 1,
    ARUGAL_VISIBLE,
    ARUGAL_TELEPORT_IN,
    ARUGAL_TURN_TO_VINCENT,
    ARUGAL_EMOTE_POINT,
    ARUGAL_EMOTE_EXCLAMATION,
    ARUGAL_EMOTE_LAUGH,
    ARUGAL_TELEPORT_OUT,
    ARUGAL_INTRO_DONE,
    ARCHMAGE_FIRE,
    ARCHMAGE_LIGHTNING,
    ARCHMAGE_INVIS,
    ARCHMAGE_VOIDWALKERS,
};

static const Waypoint nandosMovement = {-170.6f, 2182.45f, 151.91f};

class instance_shadowfang_keep : public ScriptedInstance, public DialogueHelper
{
    public:
        instance_shadowfang_keep(Map* map);

        void Initialize() override;

        void OnPlayerEnter(Player* player) override;

        void OnCreatureCreate(Creature* creature) override;
        void OnCreatureDeath(Creature* creature) override;
        void OnObjectCreate(GameObject* go) override;
        void DoSpeech();
        void JustDidDialogueStep(int32 entry) override;

        void OnCreatureEvade(Creature* creature) override;
        void OnCreatureRespawn(Creature* creature) override;

        void SetData(uint32 type, uint32 data) override;
        uint32 GetData(uint32 type) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

        void GetCrownApothecaryGuids(GuidList& lApothecaryList) const { lApothecaryList = m_lCrownApothecaryGuids; }

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint8 m_uiApothecaryDead;
        GuidList m_lNandosWolvesGuids;
        GuidList m_lCrownApothecaryGuids;

        uint32 m_uiApothecaryResetTimer;
};

#endif
