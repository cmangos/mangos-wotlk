/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_HYJALAI_H
#define SC_HYJALAI_H

#include "hyjal.h"

enum eBaseArea
{
    BASE_ALLY       = 0,
    BASE_HORDE      = 1
};

enum eMisc
{
    MAX_SPELL               = 3,
    MAX_WAVES               = 9,
    MAX_WAVE_MOB            = 18,

    ITEM_TEAR_OF_GODDESS    = 24494
};

enum eSpell
{
    SPELL_MASS_TELEPORT     = 16807,

    // Spells for Jaina
    SPELL_BRILLIANCE_AURA   = 31260,
    SPELL_BLIZZARD          = 31266,
    SPELL_PYROBLAST         = 31263,
    SPELL_SUMMON_ELEMENTALS = 31264,

    // Thrall spells
    SPELL_CHAIN_LIGHTNING   = 31330,
    SPELL_FERAL_SPIRIT      = 31331
};

enum TargetType                                             // Used in the spell cast system for the AI
{
    TARGETTYPE_SELF     = 0,
    TARGETTYPE_RANDOM   = 1,
    TARGETTYPE_VICTIM   = 2,
};

enum YellType
{
    ATTACKED     = 0,                                       // Used when attacked and set in combat
    BEGIN        = 1,                                       // Used when the event is begun
    INCOMING     = 2,                                       // Used to warn the raid that another wave phase is coming
    RALLY        = 3,                                       // Used to rally the raid and warn that the next wave has been summoned
    FAILURE      = 4,                                       // Used when raid has failed (unsure where to place)
    SUCCESS      = 5,                                       // Used when the raid has sucessfully defeated a wave phase
    DEATH        = 6,                                       // Used on death
};

struct hyjalAI : public ScriptedAI
{
        hyjalAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            memset(m_aSpells, 0, sizeof(m_aSpells));
            m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
            Reset();
        }

        // Generically used to reset our variables. Do *not* call in EnterEvadeMode as this may make problems if the raid is still in combat
        void Reset() override;

        // Send creature back to spawn location and evade.
        void EnterEvadeMode() override;

        // Called when creature reached home location after evade.
        void JustReachedHome() override;

        // Used to reset cooldowns for our spells and to inform the raid that we're under attack
        void Aggro(Unit* pWho) override;

        // Called to summon waves, check for boss deaths and to cast our spells.
        void UpdateAI(const uint32 uiDiff) override;

        // Called on death, informs the raid that they have failed.
        void JustDied(Unit* /*pKiller*/) override;

        void JustRespawned() override;

        // "Teleport" all friendly creatures away from the base.
        void Retreat();

        // Summons a creature for that wave in that base
        void SpawnCreatureForWave(uint32 uiMobEntry);

        void JustSummoned(Creature*) override;

        void SummonedCreatureJustDied(Creature* pSummoned) override;

        // Summons the next wave, calls SummonCreature
        void SummonNextWave();

        // Begins the event by gossip click
        void StartEvent();

        // Searches for the appropriate yell and sound and uses it to inform the raid of various things
        void DoTalk(YellType pYellType);

        // Used to filter who to despawn after mass teleport
        void SpellHitTarget(Unit*, const SpellEntry*) override;

        bool IsEventInProgress() const;

    public:
        ScriptedInstance* m_pInstance;

        ObjectGuid m_aBossGuid[2];

        uint32 m_uiNextWaveTimer;
        uint32 m_uiWaveCount;
        uint32 m_uiWaveMoveTimer;
        uint32 m_uiEnemyCount;
        uint32 m_uiRetreatTimer;
        uint32 m_uiBase;

        bool m_bIsEventInProgress;
        bool m_bIsFirstBossDead;
        bool m_bIsSecondBossDead;
        bool m_bIsSummoningWaves;
        bool m_bIsRetreating;
        bool m_bDebugMode;

        struct sSpells
        {
            uint32 m_uiSpellId;
            uint32 m_uiCooldown;
            TargetType m_pType;
        } m_aSpells[MAX_SPELL];

    private:
        uint32 m_uiSpellTimer[MAX_SPELL];
        GuidList lWaveMobGUIDList;
};

#endif
