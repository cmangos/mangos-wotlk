/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_DARKPORTAL_H
#define DEF_DARKPORTAL_H

enum
{
    MAX_ENCOUNTER           = 6,

    TYPE_MEDIVH             = 0,
    TYPE_SHIELD             = 1,
    TYPE_TIME_RIFT          = 2,
    TYPE_CHRONO_LORD        = 3,
    TYPE_TEMPORUS           = 4,
    TYPE_AEONUS             = 5,

    WORLD_STATE_ID          = 2541,
    WORLD_STATE_SHIELD      = 2540,
    WORLD_STATE_RIFT        = 2784,

    QUEST_OPENING_PORTAL    = 10297,
    QUEST_MASTER_TOUCH      = 9836,

    // event controlers
    NPC_TIME_RIFT           = 17838,
    NPC_MEDIVH              = 15608,

    // main bosses
    NPC_CHRONO_LORD_DEJA    = 17879,
    NPC_TEMPORUS            = 17880,
    NPC_AEONUS              = 17881,

    // boss replacements for heroic
    NPC_CHRONO_LORD         = 21697,
    NPC_TIMEREAVER          = 21698,

    // portal guardians
    NPC_RIFT_KEEPER         = 21104,
    NPC_RIFT_LORD           = 17839,

    // portal summons
    NPC_ASSASSIN            = 17835,
    NPC_WHELP               = 21818,
    NPC_CHRONOMANCER        = 17892,
    NPC_EXECUTIONER         = 18994,
    NPC_VANQUISHER          = 18995,

    // additional npcs
    NPC_COUNCIL_ENFORCER    = 17023,
    NPC_TIME_KEEPER         = 17918,
    NPC_SAAT                = 20201,
    NPC_DARK_PORTAL_DUMMY   = 18625,            // cast spell 32564 on coordinates
    NPC_DARK_PORTAL_BEAM    = 18555,            // purple beams which travel from Medivh to the Dark Portal

    // event spells
    SPELL_RIFT_CHANNEL      = 31387,            // Aura channeled by the Time Rifts on the Rift Keepers

    SPELL_BANISH_HELPER     = 31550,            // used by the main bosses to banish the time keeprs
    SPELL_CORRUPT_AEONUS    = 37853,            // used by Aeonus to corrupt Medivh

    // cosmetic spells
    SPELL_PORTAL_CRYSTAL    = 32564,            // summons 18553 - Dark Portal Crystal stalker
    SPELL_BANISH_GREEN      = 32567,

    // yells during the event
    SAY_SAAT_WELCOME        = -1269019,

    SAY_MEDIVH_INTRO        = -1269021,
    SAY_MEDIVH_ENTER        = -1269020,
    SAY_MEDIVH_WIN          = -1269026,
    SAY_MEDIVH_WEAK75       = -1269022,
    SAY_MEDIVH_WEAK50       = -1269023,
    SAY_MEDIVH_WEAK25       = -1269024,
    SAY_ORCS_ENTER          = -1269027,
    SAY_ORCS_ANSWER         = -1269028,

    AREATRIGGER_MEDIVH      = 4288,
    AREATRIGGER_ENTER       = 4485,
};

struct PortalData
{
    float fX, fY, fZ, fOrient;
};

static const PortalData afPortalLocation[] =
{
    { -2030.832f, 7024.944f, 23.07182f, 3.141593f},
    { -1961.734f, 7029.528f, 21.8114f,  2.129302f},
    { -1887.695f, 7106.557f, 22.0495f,  4.956735f},
    { -1930.911f, 7183.597f, 23.00764f, 3.595378f}
};

// Dark Crystal summon location
static const float fDarkPortalCrystalLoc[3] = { -2024.31f, 7127.75f, 22.65419f};

static const int32 uiMedivhWeakYell[3] = {SAY_MEDIVH_WEAK75, SAY_MEDIVH_WEAK50, SAY_MEDIVH_WEAK25};

class instance_dark_portal : public ScriptedInstance
{
    public:
        instance_dark_portal(Map* pMap);

        void Initialize() override;

        void OnPlayerEnter(Player* pPlayer) override;
        void OnCreatureCreate(Creature* pCreature) override;

        void OnCreatureEnterCombat(Creature* pCreature) override;
        void OnCreatureEvade(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void DoHandleAreaTrigger(uint32 uiTriggerId);

        uint32 GetCurrentRiftId() { return m_uiWorldStateRiftCount; }

        void Update(uint32 uiDiff) override;

    private:
        bool IsBossTimeRift() { return m_uiWorldStateRiftCount == 6 || m_uiWorldStateRiftCount == 12; }
        void UpdateWorldState(bool bEnable = true);
        void DoSpawnNextPortal();
        void DoResetEvent();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        uint32 m_uiWorldState;
        uint32 m_uiWorldStateRiftCount;
        uint32 m_uiWorldStateShieldCount;

        bool m_bHasIntroYelled;
        uint32 m_uiMedivhYellCount;

        uint32 m_uiNextPortalTimer;
        uint8 m_uiCurrentRiftId;
};

#endif
