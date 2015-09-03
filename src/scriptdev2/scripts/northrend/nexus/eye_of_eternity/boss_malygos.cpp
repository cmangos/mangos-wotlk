/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_malygos
SD%Complete: 80
SDComment: Timers need adjustments; Vortex event NYI; Npc movement in Phase 2 NYI.
SDCategory: Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"
#include "TemporarySummon.h"

enum
{
    SAY_INTRO_1             = -1616000,
    SAY_INTRO_2             = -1616001,
    SAY_INTRO_3             = -1616002,
    SAY_INTRO_4             = -1616003,
    SAY_INTRO_5             = -1616004,
    SAY_AGGRO               = -1616005,
    SAY_VORTEX              = -1616006,
    SAY_SPARK_BUFF          = -1616007,
    SAY_SLAY_1_A            = -1616008,
    SAY_SLAY_1_B            = -1616009,
    SAY_SLAY_1_C            = -1616010,
    SAY_END_PHASE_1         = -1616011,
    SAY_START_PHASE_2       = -1616012,
    SAY_DEEP_BREATH         = -1616013,
    SAY_SHELL               = -1616014,
    SAY_SLAY_2_A            = -1616015,
    SAY_SLAY_2_B            = -1616016,
    SAY_SLAY_2_C            = -1616017,
    SAY_END_PHASE_2         = -1616018,
    SAY_INTRO_PHASE_3       = -1616019,
    SAY_START_PHASE_3       = -1616020,
    SAY_SLAY_3_A            = -1616021,
    SAY_SLAY_3_B            = -1616022,
    SAY_SLAY_3_C            = -1616023,
    SAY_SURGE               = -1616024,
    SAY_SPELL_1             = -1616025,
    SAY_SPELL_2             = -1616026,
    SAY_SPELL_3             = -1616027,
    SAY_DEATH               = -1616028,

    SAY_EMOTE_SPARK         = -1616033,
    SAY_EMOTE_BREATH        = -1616034,

    // phase 1 spells
    SPELL_BERSERK                   = 26662,
    SPELL_ARCANE_BREATH             = 56272,
    SPELL_ARCANE_BREATH_H           = 60072,
    SPELL_SUMMON_SPARK              = 56140,
    SPELL_VORTEX                    = 56105,

    // phase 2 spells
    SPELL_ARCANE_STORM              = 57459,            // related to spell 61693
    SPELL_ARCANE_STORM_H            = 61694,
    SPELL_SUMMON_ARCANE_BOMB        = 56429,            // summons 30282
    SPELL_ARCANE_BOMB               = 56430,            // triggers 56432 and 56431 on target hit
    SPELL_SURGE_OF_POWER_PULSE      = 56505,            // deep breath spell
    // SPELL_ARCANE_PULSE           = 57432,            // purpose unk

    // transition spells
    SPELL_DESTROY_PLATFORM_PRE      = 58842,
    SPELL_DESTROY_PLATFORM_BOOM     = 59084,
    SPELL_DESTROY_PLATFORM_EVENT    = 59099,
    SPELL_SUMMON_RED_DRAGON         = 58846,

    // phase 3 spells
    SPELL_STATIC_FIELD_SUMMON       = 57430,            // cast on 1 or 3 targets based on difficulty
    SPELL_SURGE_OF_POWER            = 57407,            // related to 60936 and 60939

    // power spark
    SPELL_POWER_SPARK_MALYGOS       = 56152,
    SPELL_POWER_SPARK_PLAYERS       = 55852,
    SPELL_POWER_SPARK_VISUAL        = 55845,

    // vortex - thse spells require additional research
    // related auras: 55853, 55883, 56263, 56264, 56265, 56266, 59666, 61071, 61072, 61073, 61074, 61075
    SPELL_VORTEX_SPAWN              = 59670,
    SPELL_VORTEX_VISUAL             = 55873,
    SPELL_VORTEX_CHANNEL            = 56237,

    // arcane overload - handled in core
    // SPELL_ARCANE_OVERLOAD        = 56432,
    // SPELL_ARCANE_BOMB_KNOCKBACK  = 56431,

    // static field
    SPELL_STATIC_FIELD              = 57428,

    // vehicle related
    SPELL_SUMMON_DISC               = 56378,            // summons npc 30234 for players
    SPELL_RIDE_RED_DRAGON           = 56072,
    SPELL_FLIGHT                    = 60534,            // ToDo: check if id is correct!

    // summoned npcs
    NPC_VORTEX                      = 30090,
    NPC_POWER_SPARK                 = 30084,

    NPC_NEXUS_LORD                  = 30245,
    NPC_HOVER_DISK                  = 30248,
    NPC_SCION_OF_ETERNITY           = 30249,
    NPC_ARCANE_OVERLOAD             = 30282,

    NPC_STATIC_FIELD                = 30592,

    // phases
    PHASE_FLOOR                     = 1,
    PHASE_TRANSITION_1              = 2,
    PHASE_DISCS                     = 3,
    PHASE_TRANSITION_2              = 4,
    PHASE_DRAGONS                   = 5,

    POINT_ID_COMBAT                 = 1,
};

static const DialogueEntry aIntroDialogue[] =
{
    // Intro dialogue
    {SAY_INTRO_1,   NPC_MALYGOS,    11000},
    {SAY_INTRO_2,   NPC_MALYGOS,    13000},
    {SAY_INTRO_3,   NPC_MALYGOS,    14000},
    {SAY_INTRO_4,   NPC_MALYGOS,    12000},
    {SAY_INTRO_5,   NPC_MALYGOS,    0},

    // Phase transitions
    {SAY_END_PHASE_1,             NPC_MALYGOS,  25000},
    {PHASE_DISCS,                 0,            0},
    {SAY_END_PHASE_2,             NPC_MALYGOS,  13000},
    {SPELL_DESTROY_PLATFORM_BOOM, 0,            2000},
    {SPELL_SUMMON_RED_DRAGON,     0,            5000},
    {SAY_INTRO_PHASE_3,           NPC_MALYGOS,  0},
    {0, 0, 0},
};

static const float aCenterMovePos[3] = {754.395f, 1301.270f, 266.253f};
static const float aAlextraszaSpawnPos[4] = {700.354f, 1310.718f, 298.13f, 6.02f};
static const float aAlextraszaMovePos[3] = {726.754f, 1307.259f, 282.679f};

/*######
## boss_malygos
######*/

struct boss_malygosAI : public ScriptedAI, private DialogueHelper
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = (instance_eye_of_eternity*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);

        m_uiMaxStaticFieldTargets = m_bIsRegularMode ? 1 : 3;
        m_uiMaxNexusLords = m_bIsRegularMode ? 2 : 4;
        m_uiMaxScions = m_bIsRegularMode ? 4 : 8;

        m_bHasDoneIntro = false;
        Reset();
    }

    instance_eye_of_eternity* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bHasDoneIntro;

    uint8 m_uiPhase;
    uint8 m_uiMaxNexusLords;
    uint8 m_uiMaxScions;
    uint8 m_uiAddsDeadCount;
    uint8 m_uiMaxStaticFieldTargets;

    uint32 m_uiBerserkTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiArcaneBreathTimer;
    uint32 m_uiPowerSparkTimer;

    uint32 m_uiArcanePulseTimer;
    uint32 m_uiOverloadTimer;
    uint32 m_uiArcaneStormTimer;

    uint32 m_uiStaticFieldTimer;
    uint32 m_uiSurgeOfPowerTimer;

    void Reset() override
    {
        m_uiPhase               = PHASE_FLOOR;

        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;
        m_uiVortexTimer         = 60000;
        m_uiArcaneBreathTimer   = 15000;
        m_uiPowerSparkTimer     = 30000;

        m_uiArcanePulseTimer    = 60000;
        m_uiOverloadTimer       = 1000;
        m_uiArcaneStormTimer    = 15000;
        m_uiAddsDeadCount       = 0;

        m_uiStaticFieldTimer    = 15000;
        m_uiSurgeOfPowerTimer   = 30000;

        // reset flags
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        SetCombatMovement(false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 110.0f))
        {
            StartNextDialogueText(SAY_INTRO_1);
            m_bHasDoneIntro = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        uint8 uiTextId = 0;
        switch (m_uiPhase)
        {
            case PHASE_FLOOR:   uiTextId = urand(0, 2); break;
            case PHASE_DISCS:   uiTextId = urand(3, 5); break;
            case PHASE_DRAGONS: uiTextId = urand(6, 8); break;
        }

        switch (uiTextId)
        {
            case 0: DoScriptText(SAY_SLAY_1_A, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_1_B, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_1_C, m_creature); break;

            case 3: DoScriptText(SAY_SLAY_2_A, m_creature); break;
            case 4: DoScriptText(SAY_SLAY_2_B, m_creature); break;
            case 5: DoScriptText(SAY_SLAY_2_C, m_creature); break;

            case 6: DoScriptText(SAY_SLAY_3_A, m_creature); break;
            case 7: DoScriptText(SAY_SLAY_3_B, m_creature); break;
            case 8: DoScriptText(SAY_SLAY_3_C, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->SummonCreature(NPC_ALEXSTRASZA, aAlextraszaSpawnPos[0], aAlextraszaSpawnPos[1], aAlextraszaSpawnPos[2], aAlextraszaSpawnPos[3], TEMPSUMMON_TIMED_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, FAIL);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_COMBAT)
        {
            m_creature->SetLevitate(false);
            SetCombatMovement(true);
            DoStartMovement(m_creature->getVictim());
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ALEXSTRASZA:
                pSummoned->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                pSummoned->GetMotionMaster()->MovePoint(0, aAlextraszaMovePos[0], aAlextraszaMovePos[1], aAlextraszaMovePos[2]);
                break;
            case NPC_POWER_SPARK:
                pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                break;
            case NPC_ARCANE_OVERLOAD:
                DoCastSpellIfCan(pSummoned, SPELL_ARCANE_BOMB, CAST_TRIGGERED);
                break;
            case NPC_STATIC_FIELD:
                pSummoned->CastSpell(pSummoned, SPELL_STATIC_FIELD, false);
                break;
            case NPC_NEXUS_LORD:
            case NPC_SCION_OF_ETERNITY:
                if (Creature* pDisk = GetClosestCreatureWithEntry(pSummoned, NPC_HOVER_DISK, 10.0f))
                    pSummoned->CastSpell(pDisk, SPELL_RIDE_VEHICLE_HARDCODED, true);
                pSummoned->SetInCombatWithZone();
                break;
            case NPC_HOVER_DISK:
                pSummoned->CastSpell(pSummoned, SPELL_FLIGHT, true);
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_NEXUS_LORD || pSummoned->GetEntry() == NPC_SCION_OF_ETERNITY)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMON_DISC, true);
            ++m_uiAddsDeadCount;

            // When all adds are killed start phase 3
            if (m_uiAddsDeadCount == m_uiMaxScions + m_uiMaxNexusLords)
            {
                StartNextDialogueText(SAY_END_PHASE_2);
                m_uiPhase = PHASE_TRANSITION_2;

                // Start platform animation - not sure if this is cast by the right npc
                if (m_pInstance)
                {
                    if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER))
                        pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_PRE, false);
                }
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // Handle yell on Power Spark hit
        if (pSpell->Id == SPELL_POWER_SPARK_MALYGOS && pCaster->GetEntry() == NPC_POWER_SPARK && m_uiPhase == PHASE_FLOOR)
            DoScriptText(SAY_SPARK_BUFF, m_creature);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case PHASE_DISCS:
                // ToDo: start some movement over the platform
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_uiPhase = PHASE_DISCS;
                DoSpawnAdds();
                break;
            case SPELL_DESTROY_PLATFORM_BOOM:
                if (m_pInstance)
                {
                    if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER))
                        pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_BOOM, false);
                }
                break;
            case SPELL_SUMMON_RED_DRAGON:
                if (m_pInstance)
                {
                    // Destroy the platform
                    if (GameObject* pPlatform = m_pInstance->GetSingleGameObjectFromStorage(GO_PLATFORM))
                        pPlatform->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK_11);
                }

                DoCastSpellIfCan(m_creature, SPELL_SUMMON_RED_DRAGON);
                break;
            case SAY_INTRO_PHASE_3:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_uiPhase = PHASE_DRAGONS;
                break;
        }
    }

    // Wrapper to spawn the adds in phase 2
    void DoSpawnAdds()
    {
        float fX, fY, fZ;
        for (uint8 i = 0; i < m_uiMaxNexusLords; ++i)
        {
            m_creature->GetRandomPoint(aCenterMovePos[0], aCenterMovePos[1], aCenterMovePos[2], 50.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_HOVER_DISK, fX, fY, fZ + 30.0f, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_NEXUS_LORD, fX, fY, fZ + 30.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }

        for (uint8 i = 0; i < m_uiMaxScions; ++i)
        {
            m_creature->GetRandomPoint(aCenterMovePos[0], aCenterMovePos[1], aCenterMovePos[2], 50.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_HOVER_DISK, fX, fY, fZ + 30.0f, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_SCION_OF_ETERNITY, fX, fY, fZ + 30.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_FLOOR:

                /* ToDo: Enable this when the spells are properly supported in core
                if (m_uiVortexTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_VORTEX) == CAST_OK)
                    {
                        DoScriptText(SAY_VORTEX, m_creature);
                        m_uiVortexTimer = 60000;
                    }
                }
                else
                    m_uiVortexTimer -= uiDiff;
                */

                if (m_uiArcaneBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H) == CAST_OK)
                        m_uiArcaneBreathTimer = urand(13000, 16000);
                }
                else
                    m_uiArcaneBreathTimer -= uiDiff;

                if (m_uiPowerSparkTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPARK) == CAST_OK)
                    {
                        DoScriptText(SAY_EMOTE_SPARK, m_creature);
                        m_uiPowerSparkTimer = 30000;
                    }
                }
                else
                    m_uiPowerSparkTimer -= uiDiff;

                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    SetCombatMovement(false);
                    m_creature->SetLevitate(true);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                    // Move idle first, so we can avoid evading, because of the waypoint movement
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->GetMotionMaster()->MovePoint(0, aCenterMovePos[0], aCenterMovePos[1], aCenterMovePos[2] + 30.0f);

                    StartNextDialogueText(SAY_END_PHASE_1);
                    m_uiPhase = PHASE_TRANSITION_1;
                }

                DoMeleeAttackIfReady();

                break;
            case PHASE_DISCS:

                if (m_uiOverloadTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ARCANE_BOMB) == CAST_OK)
                    {
                        if (!urand(0, 3))
                            DoScriptText(SAY_SHELL, m_creature);

                        m_uiOverloadTimer = urand(16000, 19000);
                    }
                }
                else
                    m_uiOverloadTimer -= uiDiff;

                // Note: the boss should move in certain points before he does the breath ability
                if (m_uiArcanePulseTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_POWER_PULSE) == CAST_OK)
                    {
                        DoScriptText(SAY_DEEP_BREATH, m_creature);
                        DoScriptText(SAY_EMOTE_BREATH, m_creature);
                        m_uiArcanePulseTimer = 60000;
                    }
                }
                else
                    m_uiArcanePulseTimer -= uiDiff;

                if (m_uiArcaneStormTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H) == CAST_OK)
                        m_uiArcaneStormTimer = urand(15000, 17000);
                }
                else
                    m_uiArcaneStormTimer -= uiDiff;

                break;
            case PHASE_DRAGONS:

                if (m_uiStaticFieldTimer < uiDiff)
                {
                    // Cast Static Field spell on a number of targets, based on difficulty
                    for (uint8 i = 0; i < m_uiMaxStaticFieldTargets; ++i)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_STATIC_FIELD_SUMMON, CAST_TRIGGERED) == CAST_OK)
                            {
                                switch (urand(0, 2))
                                {
                                    case 0: DoScriptText(SAY_SPELL_1, m_creature); break;
                                    case 1: DoScriptText(SAY_SPELL_2, m_creature); break;
                                    case 2: DoScriptText(SAY_SPELL_3, m_creature); break;
                                }
                                m_uiStaticFieldTimer = urand(10000, 17000);
                            }
                        }
                    }
                }
                else
                    m_uiStaticFieldTimer -= uiDiff;

                if (m_uiSurgeOfPowerTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SURGE_OF_POWER) == CAST_OK)
                        {
                            if (!urand(0, 3))
                                DoScriptText(SAY_SURGE, m_creature);

                            m_uiSurgeOfPowerTimer = urand(5000, 15000);
                        }
                    }
                }
                else
                    m_uiSurgeOfPowerTimer -= uiDiff;

                break;
            case PHASE_TRANSITION_1:
            case PHASE_TRANSITION_2:
                // Nothing here - wait for transition to finish
                break;
        }
    }
};

CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}

/*######
## npc_power_spark
######*/

struct npc_power_sparkAI : public ScriptedAI
{
    npc_power_sparkAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_VISUAL);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_MALYGOS && m_creature->CanReachWithMeleeAttack(pWho))
        {
            DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_MALYGOS, CAST_TRIGGERED);
            m_creature->ForcedDespawn();
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_PLAYERS, CAST_TRIGGERED);
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_power_spark(Creature* pCreature)
{
    return new npc_power_sparkAI(pCreature);
}

/*######
## npc_wyrmrest_skytalon
######*/

struct npc_wyrmrest_skytalonAI : public ScriptedAI
{
    npc_wyrmrest_skytalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        m_bHasMounted = false;
        Reset();
    }

    bool m_bHasMounted;

    void Reset() override { }

    // TODO: Temporary workaround - please remove when the boarding wrappers are implemented in core
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        if (pCaster->GetTypeId() != TYPEID_PLAYER)
            return;

        if (pSpell->Id == 56071)
            DoCastSpellIfCan(m_creature, SPELL_FLIGHT, CAST_TRIGGERED);
    }

    // TODO: Enable the wrappers below, when they will be properly supported by the core
    /*
    void PassengerBoarded(Unit* pPassenger, uint8 uiSeat) override
    {
        if (pPassenger->GetTypeId() != TYPEID_PLAYER)
            return;

        // Set vehicle auras
        DoCastSpellIfCan(m_creature, SPELL_FLIGHT, CAST_TRIGGERED);
    }
    */

    void UpdateAI(const uint32 /*uiDiff*/) override
    {
        if (!m_bHasMounted)
        {
            if (m_creature->IsTemporarySummon())
            {
                TemporarySummon* pTemporary = (TemporarySummon*)m_creature;

                // Force player to mount
                if (Player* pSummoner = m_creature->GetMap()->GetPlayer(pTemporary->GetSummonerGuid()))
                    pSummoner->CastSpell(m_creature, SPELL_RIDE_RED_DRAGON, true);
            }

            m_bHasMounted = true;
        }
    }
};

CreatureAI* GetAI_npc_wyrmrest_skytalon(Creature* pCreature)
{
    return new npc_wyrmrest_skytalonAI(pCreature);
}

/*######
## event_go_focusing_iris
######*/

bool ProcessEventId_event_go_focusing_iris(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_eye_of_eternity* pInstance = (instance_eye_of_eternity*)((Creature*)pSource)->GetInstanceData())
    {
        if (pSource->GetTypeId() != TYPEID_PLAYER)
            return false;

        if (pInstance->GetData(TYPE_MALYGOS) == IN_PROGRESS || pInstance->GetData(TYPE_MALYGOS) == DONE)
            return false;

        Creature* pMalygos = pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS);
        Creature* pTrigger = pInstance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER);
        if (!pMalygos || !pTrigger)
            return false;

        // Enter combat area - Move to ground point first, then start chasing target
        float fX, fY, fZ;
        pTrigger->GetNearPoint(pTrigger, fX, fY, fZ, 0, 30.0f, pTrigger->GetAngle(pMalygos));
        pMalygos->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, fX, fY, fZ);
        pMalygos->AI()->AttackStart((Player*)pSource);

        return true;
    }
    return false;
}

void AddSC_boss_malygos()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_malygos";
    pNewScript->GetAI = &GetAI_boss_malygos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_power_spark";
    pNewScript->GetAI = &GetAI_npc_power_spark;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wyrmrest_skytalon";
    pNewScript->GetAI = &GetAI_npc_wyrmrest_skytalon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_focusing_iris";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_focusing_iris;
    pNewScript->RegisterSelf();
}
