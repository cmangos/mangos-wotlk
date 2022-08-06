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
SD%Complete: 90
SDComment: Spells need additional support
SDCategory: Eye of Eternity
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "eye_of_eternity.h"
#include "Entities/TemporarySpawn.h"

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
    SAY_START_PHASE_2       = -1616012,                 // not used
    SAY_DEEP_BREATH         = -1616013,
    SAY_SHELL               = -1616014,
    SAY_SLAY_2_A            = -1616015,
    SAY_SLAY_2_B            = -1616016,
    SAY_SLAY_2_C            = -1616017,
    SAY_END_PHASE_2         = -1616018,
    SAY_INTRO_PHASE_3       = -1616019,
    SAY_START_PHASE_3       = -1616020,                 // not used
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

    // SPELL_BEAM_PORTAL            = 56046,            // visual, before encounter; handled by DB script

    // phase 1 spells
    SPELL_BERSERK                   = 60670,
    SPELL_ARCANE_BREATH             = 56272,
    SPELL_ARCANE_BREATH_H           = 60072,
    SPELL_SUMMON_SPARK              = 56140,            // triggers 56142 which summons 30084
    SPELL_VORTEX                    = 56105,            // forces all players into vortex; triggers vehicle control aura on players
    SPELL_VORTEX_STUN               = 56237,            // visual aura on the boss
    SPELL_ARCANE_STORM_MASTER       = 57473,            // depending on the encounter phase can trigger 61693 | 61694 in P2 or 57459 in P3
    SPELL_ARCANE_STORM              = 61693,
    SPELL_ARCANE_STORM_H            = 61694,

    // phase 2 spells
    SPELL_SUMMON_ARCANE_BOMB        = 56429,            // summons 30282
    SPELL_SURGE_OF_POWER_PULSE      = 56505,            // deep breath spell
    // SPELL_ARCANE_PULSE           = 57432,            // purpose unk

    // transition spells
    SPELL_DESTROY_PLATFORM_PRE      = 58842,
    SPELL_DESTROY_PLATFORM_BOOM     = 59084,
    SPELL_DESTROY_PLATFORM_EVENT    = 59099,
    SPELL_SUMMON_RED_DRAGON         = 58846,

    SPELL_CLEAR_ALL_DEBUFFS         = 34098,            // not clear exactly which debuffs have to be removed
    SPELL_IMMUNE_CURSES             = 64515,

    // phase 3 spells
    SPELL_ARCANE_STORM_VEHICLE      = 57459,            // targets vehicles
    SPELL_STATIC_FIELD_SUMMON       = 57430,            // cast on 1 or 3 targets based on difficulty
    SPELL_SURGE_OF_POWER            = 57407,
    SPELL_SURGE_OF_POWER_H          = 60936,
    SPELL_SURGE_OF_POWER_WARNING    = 60939,            // dummy effect which sends a warning to the targets

    // power spark
    SPELL_POWER_SPARK_MALYGOS       = 56152,
    SPELL_POWER_SPARK_PLAYERS       = 55852,
    SPELL_POWER_SPARK_VISUAL        = 55845,

    // vortex spells
    // SPELL_VORTEX_SPAWN           = 59670,            // not used; probably unrelated
    SPELL_VORTEX_VISUAL             = 55873,            // cast by creature 22517
    SPELL_VORTEX_CONTROL_1          = 55853,            // cast by player on creature 30090; each spell controlls a specific seat id
    SPELL_VORTEX_CONTROL_2          = 56263,
    SPELL_VORTEX_CONTROL_3          = 56264,
    SPELL_VORTEX_CONTROL_4          = 56265,
    SPELL_VORTEX_CONTROL_5          = 56266,
    SPELL_VORTEX_CONTROL_1_H        = 61071,
    SPELL_VORTEX_CONTROL_2_H        = 61072,
    SPELL_VORTEX_CONTROL_3_H        = 61073,
    SPELL_VORTEX_CONTROL_4_H        = 61074,
    SPELL_VORTEX_CONTROL_5_H        = 61075,
    SPELL_VORTEX_TELEPORT           = 73040,            // teleport player at the end of vortex
    // SPELL_VORTEX_PERIODIC        = 59666,            // not used; probably unrelated
    // SPELL_VORTEX_AURA            = 55883,            // cast by creature 30090; handled in c_t_a

    // arcane overload - handled in spell script
    // SPELL_ARCANE_OVERLOAD        = 56432,
    // SPELL_ARCANE_BOMB_KNOCKBACK  = 56431,

    // arcane bomb
    SPELL_ARCANE_BOMB               = 56430,            // triggers 56432 and 56431 on target hit

    // static field
    SPELL_STATIC_FIELD              = 57428,

    // vehicle related
    // SPELL_SUMMON_DISC            = 56378,            // not used
    // SPELL_RIDE_RED_DRAGON        = 56072,            // handled by EAI

    // npcs
    // NPC_MALYGOS_PORTAL           = 30118,            // used for intro and for power sparks
    NPC_VORTEX                      = 30090,            // vehicle with 5 seats
    NPC_POWER_SPARK                 = 30084,
    NPC_STATIC_FIELD                = 30592,            // summoned by spell 57430

    // phases
    PHASE_FLOOR                     = 1,
    PHASE_TRANSITION                = 2,
    PHASE_DISCS                     = 3,
    PHASE_DRAGONS                   = 4,
    PHASE_VORTEX                    = 8,                // sub-phase, part of phase 1
    PHASE_SURGE                     = 9,                // sub-phase, part of phase 2

    // move point ids
    POINT_ID_COMBAT                 = 1,
    POINT_ID_VORTEX                 = 2,
    POINT_ID_TRANSITION_1           = 3,
    POINT_ID_FLIGHT                 = 4,
    POINT_ID_SURGE                  = 5,
    POINT_ID_TRANSITION_2           = 6,
    POINT_ID_PHASE_3                = 7,

    // light overrid id
    LIGHT_ID_DEFAULT                = 1773,
    LIGHT_ID_OBSCURE_SPACE          = 1822,
    LIGHT_ID_CHANGE_DIMENSIONS      = 1823,
    LIGHT_ID_ARCANE_RUNES           = 1824,
    LIGHT_ID_OBSCURE_ARCANE_RUNES   = 1825,

    AREA_ID_EYE_OF_ETERNITY         = 4500,
};

static const DialogueEntry aIntroDialogue[] =
{
    // Phase transitions
    {SAY_END_PHASE_1,             NPC_MALYGOS,  3000},
    {LIGHT_ID_ARCANE_RUNES,       0,            22000},
    {PHASE_DISCS,                 0,            0},

    {SAY_END_PHASE_2,             NPC_MALYGOS,  1000},
    {LIGHT_ID_CHANGE_DIMENSIONS,  0,            6000},
    {LIGHT_ID_OBSCURE_SPACE,      0,            4000},
    {SPELL_DESTROY_PLATFORM_PRE,  0,            3000},
    {SPELL_DESTROY_PLATFORM_BOOM, 0,            2000},
    {SPELL_SUMMON_RED_DRAGON,     0,            5000},
    {SAY_INTRO_PHASE_3,           NPC_MALYGOS,  11000},
    {LIGHT_ID_OBSCURE_ARCANE_RUNES, 0,          0},

    {0, 0, 0},
};

static const float aCenterMovePos[3] = {754.395f, 1301.270f, 266.253f};                 // platform center location
static const float aVortexMovePos[3] = { 756.70105f, 1303.9907f, 286.1703f };           // phase 1 vortex center location
static const float aHoverMovePos[3] = { 828.996f, 1298.84f, 300.0f };                   // phase 2 transition location
static const float aSurgeMovePos[3] = { 746.10187f, 1305.9847f, 316.653f };             // phase 2 surge of power location
static const float aTransitionPos[3] = { 754.544f, 1301.71f, 320.0f };                  // phase 3 transition location
static const float aDragonsMovePos[3] = { 752.20557f, 1304.3005f, 220.06091f };         // phase 3 static location

static const uint32 vortexSpellsNormal[] = { SPELL_VORTEX_CONTROL_1, SPELL_VORTEX_CONTROL_2, SPELL_VORTEX_CONTROL_3, SPELL_VORTEX_CONTROL_4, SPELL_VORTEX_CONTROL_5 };
static const uint32 vortexSpellsHeroic[] = { SPELL_VORTEX_CONTROL_1_H, SPELL_VORTEX_CONTROL_2_H, SPELL_VORTEX_CONTROL_3_H, SPELL_VORTEX_CONTROL_4_H, SPELL_VORTEX_CONTROL_5_H };

struct HoverDiskSummonData
{
    float x, y, z;
    uint8 pathId;
};

static const HoverDiskSummonData lordDiskSummonData[] =
{
    {753.9635f, 1319.003f, 285.052f, 0},
    {773.4768f, 1301.474f, 266.582f, 1},
    {778.6023f, 1301.635f, 285.671f, 2},            // heroic spawns
    {730.3984f, 1301.644f, 285.091f, 3}
};

static const HoverDiskSummonData scionDiskSummonData[] =
{
    {753.960f, 1272.719f, 282.3402f, 0},
    {724.828f, 1301.750f, 282.3402f, 2},
    {754.020f, 1330.910f, 282.3402f, 3},
    {784.416f, 1301.570f, 282.3402f, 4},
    {734.432f, 1280.950f, 282.3402f, 5},            // heroic spawns
    {733.814f, 1323.340f, 282.3402f, 6},
    {774.969f, 1322.170f, 282.3402f, 7},
    {773.226f, 1279.437f, 282.3402f, 8},
};

/*######
## boss_malygos
######*/

struct boss_malygosAI : public ScriptedAI, private DialogueHelper
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_pInstance = static_cast<instance_eye_of_eternity*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        InitializeDialogueHelper(m_pInstance);

        m_uiMaxStaticFieldTargets = m_bIsRegularMode ? 1 : 3;
        m_uiMaxStormTargets = m_bIsRegularMode ? 5 : 15;
        m_uiMaxNexusLords = m_bIsRegularMode ? 2 : 4;
        m_uiMaxScions = m_bIsRegularMode ? 4 : 8;

        Reset();
    }

    instance_eye_of_eternity* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiIntroStage;

    uint8 m_uiPhase;
    uint8 m_uiMaxNexusLords;
    uint8 m_uiMaxScions;

    uint8 m_uiMaxStormTargets;
    uint8 m_uiMaxStaticFieldTargets;

    uint32 m_uiBerserkTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiArcaneBreathTimer;
    uint32 m_uiPowerSparkTimer;
    uint32 m_uiArcaneStormTimer;
    uint32 m_uiVortexEndTimer;

    uint32 m_uiArcanePulseTimer;
    uint32 m_uiOverloadTimer;
    uint32 m_uiArcanePulseEndTimer;

    uint32 m_uiStaticFieldTimer;
    uint32 m_uiSurgeOfPowerTimer;

    void Reset() override
    {
        m_uiIntroStage          = 0;
        m_uiPhase               = PHASE_FLOOR;

        m_uiBerserkTimer        = 10 * MINUTE * IN_MILLISECONDS;
        m_uiVortexTimer         = 35000;
        m_uiArcaneBreathTimer   = 15000;
        m_uiPowerSparkTimer     = 30000;
        m_uiArcaneStormTimer    = 10000;
        m_uiVortexEndTimer      = 10000;

        m_uiArcanePulseTimer    = 60000;
        m_uiOverloadTimer       = 1000;
        m_uiArcanePulseEndTimer = 8000;

        m_uiStaticFieldTimer    = 25000;
        m_uiSurgeOfPowerTimer   = 10000;

        // reset flags
        m_creature->SetImmuneToPlayer(true);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

        SetCombatMovement(false);

        // reset lights
        m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_DEFAULT, 0);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
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
        m_creature->SetLevitate(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, DONE);
    }

    void JustReachedHome() override
    {
        m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_DEFAULT, 0);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, FAIL);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        // remove flight anim and start moving
        if (uiMoveType == POINT_MOTION_TYPE)
        {
            switch (uiPointId)
            {
                case POINT_ID_COMBAT:
                    m_creature->SetLevitate(false);
                    SetCombatMovement(true);
                    DoStartMovement(m_creature->GetVictim());
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                    // after vortex phase inform the Power sparks and set phase
                    if (m_uiPhase != PHASE_FLOOR)
                    {
                        SendAIEventAround(AI_EVENT_CUSTOM_D, m_creature, 0, 50.0f);
                        m_uiPowerSparkTimer = 10000;
                        m_uiPhase = PHASE_FLOOR;
                    }
                    break;
                case POINT_ID_VORTEX:
                    if (m_pInstance)
                    {
                        if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER))
                            pTrigger->CastSpell(pTrigger, SPELL_VORTEX_VISUAL, TRIGGERED_NONE);
                    }
                    if (DoCastSpellIfCan(m_creature, SPELL_VORTEX) == CAST_OK)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_VORTEX_STUN, CAST_TRIGGERED);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_uiPhase = PHASE_VORTEX;
                        m_uiVortexEndTimer = 10000;
                    }
                    break;
                case POINT_ID_TRANSITION_1:
                case POINT_ID_TRANSITION_2:
                    // no action here
                    break;
                case POINT_ID_FLIGHT:
                    // start wp movement during phase 2; use path id 1
                    m_creature->GetMotionMaster()->MoveWaypoint(1);
                    break;
                case POINT_ID_SURGE:
                    if (DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_POWER_PULSE) == CAST_OK)
                    {
                        DoScriptText(SAY_DEEP_BREATH, m_creature);
                        DoScriptText(SAY_EMOTE_BREATH, m_creature);
                        m_uiArcanePulseEndTimer = 8000;
                        m_uiPhase = PHASE_SURGE;
                    }
                    break;
            }
        }
        // intro yells; handled once per boss circle; ToDo: maybe this is more dyanmic than this
        else if (uiMoveType == WAYPOINT_MOTION_TYPE && uiPointId == 1 && m_uiIntroStage < 5 && !m_creature->IsInCombat())
        {
            switch (m_uiIntroStage)
            {
                case 0: DoScriptText(SAY_INTRO_1, m_creature); break;
                case 1: DoScriptText(SAY_INTRO_2, m_creature); break;
                case 2: DoScriptText(SAY_INTRO_3, m_creature); break;
                case 3: DoScriptText(SAY_INTRO_4, m_creature); break;
                case 4: DoScriptText(SAY_INTRO_5, m_creature); break;
            }

            ++m_uiIntroStage;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_ARCANE_OVERLOAD:
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);
                DoCastSpellIfCan(pSummoned, SPELL_ARCANE_BOMB, CAST_TRIGGERED);
                break;
            case NPC_STATIC_FIELD:
                pSummoned->AI()->SetReactState(REACT_PASSIVE);
                pSummoned->SetCanEnterCombat(false);
                pSummoned->CastSpell(pSummoned, SPELL_STATIC_FIELD, TRIGGERED_NONE);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        // start third phase
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_TRANSITION_2, aTransitionPos[0], aTransitionPos[1], aTransitionPos[2]);

            StartNextDialogueText(SAY_END_PHASE_2);
            m_uiPhase = PHASE_TRANSITION;
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // arcane storm spell
            if (m_uiPhase == PHASE_FLOOR)
                DoCastSpellIfCan(pInvoker, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H, CAST_TRIGGERED);
            else if (m_uiPhase == PHASE_DRAGONS)
                DoCastSpellIfCan(pInvoker, SPELL_ARCANE_STORM_VEHICLE, CAST_TRIGGERED);
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
            case LIGHT_ID_ARCANE_RUNES:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_ARCANE_RUNES, 5);
                break;
            case PHASE_DISCS:
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_FLIGHT, aHoverMovePos[0], aHoverMovePos[1], aHoverMovePos[2]);
                m_uiPhase = PHASE_DISCS;
                DoSpawnAdds();
                break;
            case LIGHT_ID_CHANGE_DIMENSIONS:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_CHANGE_DIMENSIONS, 5);
                break;
            case LIGHT_ID_OBSCURE_SPACE:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_OBSCURE_SPACE, 5);
                break;
            case SPELL_DESTROY_PLATFORM_PRE:
                DoCastSpellIfCan(m_creature, SPELL_DESTROY_PLATFORM_PRE);
                break;
            case SPELL_DESTROY_PLATFORM_BOOM:
                if (m_pInstance)
                {
                    if (Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_ALEXSTRASZA_INVIS))
                    {
                        pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_EVENT, TRIGGERED_NONE);
                        pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_BOOM, TRIGGERED_OLD_TRIGGERED);
                    }
                }
                break;
            case SPELL_SUMMON_RED_DRAGON:
                if (m_pInstance)
                {
                    // Destroy the platform; this can be also handled by event 20158
                    if (GameObject* pPlatform = m_pInstance->GetSingleGameObjectFromStorage(GO_PLATFORM))
                    {
                        pPlatform->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                        pPlatform->SetGoState(GO_STATE_ACTIVE);
                        m_creature->GetMap()->ChangeGOPathfinding(194232, 8546, false);
                    }
                }

                DoCastSpellIfCan(m_creature, SPELL_SUMMON_RED_DRAGON);
                break;
            case SAY_INTRO_PHASE_3:
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_3, aDragonsMovePos[0], aDragonsMovePos[1], aDragonsMovePos[2]);
                break;
            case LIGHT_ID_OBSCURE_ARCANE_RUNES:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_OBSCURE_ARCANE_RUNES, 5);
                DoCastSpellIfCan(m_creature, SPELL_CLEAR_ALL_DEBUFFS, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_IMMUNE_CURSES, CAST_TRIGGERED);
                m_uiArcaneStormTimer = 20000;
                m_uiPhase = PHASE_DRAGONS;
                break;
        }
    }

    // Wrapper to spawn the adds in phase 2
    void DoSpawnAdds()
    {
        // spawn the nexus lord disks; 2 for normal | 4 for heroic
        for (uint8 i = 0; i < m_uiMaxNexusLords; ++i)
            m_creature->SummonCreature(NPC_HOVER_DISK_LORD, lordDiskSummonData[i].x, lordDiskSummonData[i].y, lordDiskSummonData[i].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, lordDiskSummonData[i].pathId);

        // spawn the scion hover disks; 4 for normal | 8 for heroic
        for (uint8 i = 0; i < m_uiMaxScions; ++i)
            m_creature->SummonCreature(NPC_HOVER_DISK_SCION, scionDiskSummonData[i].x, scionDiskSummonData[i].y, scionDiskSummonData[i].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, scionDiskSummonData[i].pathId);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
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

                if (m_uiVortexTimer < uiDiff)
                {
                    // start flying and prepare vortex sub-phase
                    SetCombatMovement(false);
                    m_creature->SetLevitate(true);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_VORTEX, aVortexMovePos[0], aVortexMovePos[1], aVortexMovePos[2]);

                    SendAIEventAround(AI_EVENT_CUSTOM_C, m_creature, 0, 50.0f);
                    m_uiPhase = PHASE_TRANSITION;

                    DoScriptText(SAY_VORTEX, m_creature);
                    m_uiVortexTimer = 45000;
                }
                else
                    m_uiVortexTimer -= uiDiff;

                if (m_uiArcaneBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H) == CAST_OK)
                        m_uiArcaneBreathTimer = urand(20000, 25000);
                }
                else
                    m_uiArcaneBreathTimer -= uiDiff;

                if (m_uiPowerSparkTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SPARK) == CAST_OK)
                        m_uiPowerSparkTimer = 30000;
                }
                else
                    m_uiPowerSparkTimer -= uiDiff;

                if (m_uiArcaneStormTimer < uiDiff)
                {
                    std::vector<Unit*> suitableTargets;
                    ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

                    for (auto itr : threatList)
                    {
                        if (Unit* target = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
                            suitableTargets.push_back(target);
                    }

                    // shuffle the targets
                    std::shuffle(suitableTargets.begin(), suitableTargets.end(), *GetRandomGenerator());
                    uint8 maxTargets = suitableTargets.size() >= m_uiMaxStormTargets ? m_uiMaxStormTargets : suitableTargets.size();

                    // cast spell
                    for (uint8 i = 0; i < maxTargets; ++i)
                        DoCastSpellIfCan(suitableTargets[i], SPELL_ARCANE_STORM_MASTER, CAST_TRIGGERED);

                    m_uiArcaneStormTimer = 10000;
                }
                else
                    m_uiArcaneStormTimer -= uiDiff;

                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    SetCombatMovement(false);
                    m_creature->SetLevitate(true);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_TRANSITION_1, aVortexMovePos[0], aVortexMovePos[1], aVortexMovePos[2]);

                    StartNextDialogueText(SAY_END_PHASE_1);
                    m_uiPhase = PHASE_TRANSITION;
                }

                DoMeleeAttackIfReady();

                break;
            case PHASE_VORTEX:

                // end Vortex and lower the boss on the ground
                if (m_uiVortexEndTimer < uiDiff)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_VORTEX_STUN);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, aCenterMovePos[0], aCenterMovePos[1], aCenterMovePos[2]);
                    m_uiPhase = PHASE_TRANSITION;
                    m_uiVortexEndTimer = 10000;
                }
                else
                    m_uiVortexEndTimer -= uiDiff;

                break;
            case PHASE_DISCS:

                if (m_uiOverloadTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_ARCANE_BOMB) == CAST_OK)
                    {
                        m_creature->GetMotionMaster()->PauseWaypoints(1000);
                        m_uiOverloadTimer = urand(16000, 19000);
                    }
                }
                else
                    m_uiOverloadTimer -= uiDiff;

                // move to center position before deep breath
                if (m_uiArcanePulseTimer < uiDiff)
                {
                    // pause WP movement; don't clear the movegen
                    m_creature->GetMotionMaster()->PauseWaypoints(15000);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_SURGE, aSurgeMovePos[0], aSurgeMovePos[1], aSurgeMovePos[2]);
                    m_uiPhase = PHASE_TRANSITION;
                    m_uiArcanePulseTimer = 55000;
                }
                else
                    m_uiArcanePulseTimer -= uiDiff;

                break;
            case PHASE_SURGE:

                // end of power surge; resume WP movement
                if (m_uiArcanePulseEndTimer < uiDiff)
                {
                    m_creature->GetMotionMaster()->UnpauseWaypoints();
                    m_uiPhase = PHASE_DISCS;
                    m_uiArcanePulseEndTimer = 8000;
                }
                else
                    m_uiArcanePulseEndTimer -= uiDiff;

                break;
            case PHASE_DRAGONS:

                // Arcane storm targets
                if (m_uiArcaneStormTimer < uiDiff)
                {
                    std::vector<Unit*> suitableTargets;
                    ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

                    for (auto itr : threatList)
                    {
                        if (Unit* target = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
                            suitableTargets.push_back(target);
                    }

                    // shuffle the targets
                    std::shuffle(suitableTargets.begin(), suitableTargets.end(), *GetRandomGenerator());
                    uint8 maxTargets = suitableTargets.size() >= m_uiMaxStormTargets ? m_uiMaxStormTargets : suitableTargets.size();

                    // cast spell
                    for (uint8 i = 0; i < maxTargets; ++i)
                        DoCastSpellIfCan(suitableTargets[i], SPELL_ARCANE_STORM_MASTER, CAST_TRIGGERED);

                    m_uiArcaneStormTimer = urand(3000, 5000);
                }
                else
                    m_uiArcaneStormTimer -= uiDiff;

                // Cast Static Field spell on a number of targets, based on difficulty
                if (m_uiStaticFieldTimer < uiDiff)
                {
                    std::vector<Unit*> suitableTargets;
                    ThreatList const& threatList = m_creature->getThreatManager().getThreatList();

                    for (auto itr : threatList)
                    {
                        if (Unit* target = m_creature->GetMap()->GetUnit(itr->getUnitGuid()))
                            suitableTargets.push_back(target);
                    }

                    // shuffle the targets
                    std::shuffle(suitableTargets.begin(), suitableTargets.end(), *GetRandomGenerator());
                    uint8 maxTargets = suitableTargets.size() >= m_uiMaxStaticFieldTargets ? m_uiMaxStaticFieldTargets : suitableTargets.size();

                    // cast spell
                    for (uint8 i = 0; i < maxTargets; ++i)
                        DoCastSpellIfCan(suitableTargets[i], SPELL_STATIC_FIELD_SUMMON, CAST_TRIGGERED);

                    m_uiStaticFieldTimer = 15000;
                }
                else
                    m_uiStaticFieldTimer -= uiDiff;

                if (m_uiSurgeOfPowerTimer < uiDiff)
                {
                    if (m_bIsRegularMode)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCastSpellIfCan(pTarget, SPELL_SURGE_OF_POWER);
                    }
                    else
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_POWER_H) == CAST_OK)
                            DoCastSpellIfCan(m_creature, SPELL_SURGE_OF_POWER_WARNING, CAST_TRIGGERED);
                    }

                    if (!urand(0, 3))
                        DoScriptText(SAY_SURGE, m_creature);

                    m_uiArcaneStormTimer += 6000;
                    m_uiSurgeOfPowerTimer = 15000;
                }
                else
                    m_uiSurgeOfPowerTimer -= uiDiff;

                break;
            case PHASE_TRANSITION:
                // Nothing here - wait for transition to finish
                break;
        }
    }
};

/*######
## npc_power_spark
######*/

struct npc_power_sparkAI : public ScriptedAI
{
    npc_power_sparkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_eye_of_eternity*>(pCreature->GetInstanceData());

        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);

        DoScriptText(SAY_EMOTE_SPARK, m_creature);
        Reset();
    }

    instance_eye_of_eternity* m_pInstance;

    void Reset() override
    {
        if (m_pInstance)
        {
            if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                m_creature->GetMotionMaster()->MoveChase(pMalygos, 0.f, 0.f, false, true, false);
        }

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

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // start / stop following during vortex
        if (eventType == AI_EVENT_CUSTOM_C)
            m_creature->GetMotionMaster()->MoveIdle();
        else if (eventType == AI_EVENT_CUSTOM_D)
            m_creature->GetMotionMaster()->MoveChase(pSender, 0.f, 0.f, false, true, false);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_PLAYERS, CAST_TRIGGERED);
    }
};

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

        // interrupt spells and allow attack
        pMalygos->InterruptNonMeleeSpells(false);
        pMalygos->SetImmuneToPlayer(false);

        // Enter combat area - Move to ground point first, then start chasing target
        float fX, fY, fZ;
        pTrigger->GetNearPoint(pTrigger, fX, fY, fZ, 0, 30.0f, pTrigger->GetAngle(pMalygos));
        pMalygos->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, fX, fY, fZ);
        pMalygos->SetInCombatWithZone();

        return true;
    }
    return false;
}

/*######
## spell_ride_red_dragon_buddy - 56072
######*/

struct spell_ride_red_dragon_buddy : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster || !target || !target->IsPlayer())
            return;

        uint32 uiSpell = spell->m_spellInfo->CalculateSimpleValue(effIdx);

        // player cast 56071 on caster
        target->CastSpell(caster, uiSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_arcane_bomb - 56430
######*/

struct spell_arcane_bomb : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(target, 56431, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 56432, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_arcane_storm - 57473
######*/

struct spell_arcane_storm : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // inform the boss to cast the proper spell on target depending on the encounter phase
        caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, target, caster);
    }
};

/*######
## spell_vortex - 56105
######*/

struct spell_vortex : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        uint32 targetCount = spell->GetTargetList().size();
        uint8 index = uint8((targetCount - 1) * 0.2);

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsPlayer())
            return;

        // cast the corresponding vortex control spell, based on the target index
        target->CastSpell(nullptr, target->GetMap()->IsRegularDifficulty() ? vortexSpellsNormal[index] : vortexSpellsHeroic[index], TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_vortex_aura - 55853, 56263, 56264, 56265, 56266, 61071, 61072, 61073, 61074, 61075
######*/

struct spell_vortex_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!target || !caster || !caster->IsPlayer())
            return;

        // teleport vehicle passenger outside of the vortex
        // ToDo: enable this when script hook works properly; spell has to be cast after vehicle unboard is completed
        // if (!apply)
        //     target->CastSpell(caster, 73040, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_align_disk_aggro - 61210
######*/

struct spell_align_disk_aggro : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsVehicle() || !caster || !caster->GetVictim())
            return;

        // no reaction while the disk is still flying
        if (target->GetPositionZ() > 267.0f)
            return;

        // start chasing players
        if (target->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
            target->GetMotionMaster()->MoveChase(caster->GetVictim(), 0.f, 0.f, false, false, false);
    }
};

void AddSC_boss_malygos()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_malygos";
    pNewScript->GetAI = &GetNewAIInstance<boss_malygosAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_power_spark";
    pNewScript->GetAI = &GetNewAIInstance<npc_power_sparkAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_focusing_iris";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_focusing_iris;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_ride_red_dragon_buddy>("spell_ride_red_dragon_buddy");
    RegisterSpellScript<spell_arcane_bomb>("spell_arcane_bomb");
    RegisterSpellScript<spell_arcane_storm>("spell_arcane_storm");
    RegisterSpellScript<spell_vortex>("spell_vortex");
    RegisterSpellScript<spell_vortex_aura>("spell_vortex_aura");
    RegisterSpellScript<spell_align_disk_aggro>("spell_align_disk_aggro");
}
