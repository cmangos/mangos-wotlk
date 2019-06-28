/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_HYJALAI_H
#define SC_HYJALAI_H

#include "hyjal.h"

enum Misc
{
    MAX_SPELL               = 3,

    ITEM_TEAR_OF_GODDESS    = 24494,

    SAY_CALL_FOR_HELP_EMOTE = -1000007,
};

enum HyjalSpells
{
    SPELL_MASS_TELEPORT     = 16807,

    // Spells for Jaina
    SPELL_BRILLIANCE_AURA   = 31260,
    SPELL_BLIZZARD          = 31266,
    SPELL_PYROBLAST         = 31263,
    SPELL_SUMMON_ELEMENTALS = 31264,

    // Thrall spells
    SPELL_CHAIN_LIGHTNING   = 31330,
    SPELL_FERAL_SPIRIT      = 31331,

    // Tyrande spells
    SPELL_STARFALL          = 20687,
    SPELL_TRUESHOT_AURA     = 31519,
};

enum TargetType                                             // Used in the spell cast system for the AI
{
    TARGETTYPE_SELF     = 0,
    TARGETTYPE_RANDOM   = 1,
    TARGETTYPE_VICTIM   = 2,
};

enum YellType
{
    ATTACKED     = 0,                                       // Used when attacked and below 90% HP
    INCOMING     = 1,                                       // Used to warn the raid that another wave phase is coming
    RALLY        = 2,                                       // Used to rally the raid randomly while a wave event is in progress
    RETREAT      = 3,                                       // Used when the scourge mob count reaches 30 (which fails the event)
    WIN          = 4,                                       // Used when both bosses are done and a player asks the NPC to teleport away
    DEATH        = 5,                                       // Used on death
};

struct hyjalAI : public ScriptedAI
{
        hyjalAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            memset(m_aSpells, 0, sizeof(m_aSpells));
            m_pInstance = (instance_mount_hyjal*)pCreature->GetInstanceData();
            Reset();
        }

        // Generically used to reset our variables. Do *not* call in EnterEvadeMode as this may make problems if the raid is still in combat
        void Reset() override;

        // Send creature back to spawn location and evade.
        void EnterEvadeMode() override;

        // Called when creature reached home location after evade.
        void JustReachedHome() override;

        // Used to reset cooldowns for our spells and to inform the raid that we're under attack
        void Aggro(Unit* who) override;

        // Called to summon waves, check for boss deaths and to cast our spells.
        void UpdateAI(const uint32 diff) override;

        void JustRespawned() override;

        void JustDied(Unit* /*killer*/) override;

        void ReceiveAIEvent(AIEventType /*eventType*/, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override;

        // Instance response handlers
        void EventStarted();
        void Retreat();
        void Win();

        // Searches for the appropriate yell and sound and uses it to inform the raid of various things
        void DoTalk(YellType yellType);

        instance_mount_hyjal* m_pInstance;

        struct sSpells
        {
            uint32 m_uiSpellId;
            uint32 m_uiCooldown;
            TargetType m_pType;
        } m_aSpells[MAX_SPELL];

        uint32 m_uiSpellTimer[MAX_SPELL];
        uint32 m_uiAttackedYellTimer;
        uint32 m_uiRallyYellTimer = 0;

        bool m_calledForHelp;
};

#endif
