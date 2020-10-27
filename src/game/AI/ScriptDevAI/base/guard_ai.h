/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_GUARDAI_H
#define SC_GUARDAI_H

enum
{
    GENERIC_CREATURE_COOLDOWN       = 5000,

    SAY_GUARD_SIL_AGGRO1            = -1000198,
    SAY_GUARD_SIL_AGGRO2            = -1000199,
    SAY_GUARD_SIL_AGGRO3            = -1000200,

    NPC_CENARION_INFANTRY           = 15184
};

enum eShattrathGuard
{
    SPELL_BANISHED_SHATTRATH_A      = 36642,
    SPELL_BANISHED_SHATTRATH_S      = 36671,
    SPELL_BANISH_TELEPORT           = 36643,
    SPELL_EXILE                     = 39533
};

enum eHallowsEndGuard
{
    SPELL_START_FIRE_TARGET_TEST_GUARD  = 43711,
    SPELL_FIRE                          = 42074,

    TEXT_ID_AZUREMYST_PEACEKEEPER       = 12119,
    TEXT_ID_RAZOR_HILL_GRUNT            = 60004,
    TEXT_ID_DEATHGUARD_FLORENCE         = 60005,
    TEXT_ID_SILVERMOON_GUARDIAN         = 60006,
    TEXT_ID_IRONFORGE_MOUNTAINEER       = 60007,
    TEXT_ID_STORMWIND_GUARD             = 60008,
    SAY_GUARD_RELIGHT_FIRE              = -1015137,
    
    NPC_HEADLESS_HORSEMAN_FIRE          = 23537,
    NPC_DEATHGUARD_FLORENCE             = 24491,
    NPC_RAZOR_HILL_GRUNT                = 5953,
    NPC_IRONFORGE_MOUNTAINEER           = 727,
    NPC_STORMWIND_GUARD                 = 1423,
    NPC_SILVERMOON_GUARDIAN             = 16221,
    NPC_AZUREMYST_PEACEKEEPER           = 18038,
};

struct guardAI : public ScriptedAI
{
    public:
        explicit guardAI(Creature* creature);
        ~guardAI() {}

        uint32 m_globalCooldown;                          // This variable acts like the global cooldown that players have (1.5 seconds)
        uint32 m_buffTimer;                               // This variable keeps track of buffs

        void Reset() override;

        void Aggro(Unit* who) override;

        void JustDied(Unit* /*killer*/) override;

        void UpdateAI(const uint32 diff) override;

        // Commonly used for guards in main cities
        void DoReplyToTextEmote(uint32 textEmote);
};

struct guardAI_orgrimmar : public guardAI
{
    guardAI_orgrimmar(Creature* creature) : guardAI(creature) {}

    void ReceiveEmote(Player* player, uint32 textEmote) override;
};

#endif
