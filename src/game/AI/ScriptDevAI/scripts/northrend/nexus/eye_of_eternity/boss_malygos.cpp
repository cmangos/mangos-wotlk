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
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_INTRO_1             = 31759,
    SAY_INTRO_2             = 31760,
    SAY_INTRO_3             = 31761,
    SAY_INTRO_4             = 31762,
    SAY_INTRO_5             = 31763,
    SAY_AGGRO               = 31764,
    SAY_VORTEX              = 31774,
    SAY_SPARK_BUFF          = 31783,
    SAY_SLAY_1_A            = 31767,
    SAY_SLAY_1_B            = 31768,
    SAY_SLAY_1_C            = 31769,
    SAY_END_PHASE_1         = 31772,
    SAY_START_PHASE_2       = 31765,                 // not used
    SAY_DEEP_BREATH         = 31781,
    SAY_SHELL               = 31773,
    SAY_SLAY_2_A            = 31775,
    SAY_SLAY_2_B            = 31776,
    SAY_SLAY_2_C            = 31777,
    SAY_END_PHASE_2         = 31778,
    SAY_INTRO_PHASE_3       = 31779,
    SAY_START_PHASE_3       = 31780,                 // not used
    SAY_SLAY_3_A            = 31784,
    SAY_SLAY_3_B            = 31785,
    SAY_SLAY_3_C            = 31786,
    SAY_SURGE               = 31782,
    SAY_SPELL_1             = 31787,
    SAY_SPELL_2             = 31788,
    SAY_SPELL_3             = 31789,
    SAY_DEATH               = 31790,

    SAY_EMOTE_SPARK         = 32957,
    SAY_EMOTE_BREATH        = 20021,

    // SPELL_BEAM_PORTAL            = 56046,            // visual, before encounter; handled by DB script
    SPELL_RANDOM_PORTAL             = 56047,            // TODO: meant to be cast and move to random portals rather than cycle

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

    SPELL_COOLDOWN_CREATURE_SPECIAL = 61207,

    // phase 2 spells
    SPELL_SUMMON_ARCANE_BOMB        = 56429,            // summons 30282
    SPELL_SURGE_OF_POWER_PULSE      = 56505,            // deep breath spell

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
    // SPELL_ARCANE_PULSE           = 57432,            // purpose unk

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

    // move point ids
    POINT_ID_COMBAT                 = 1,
    POINT_ID_VORTEX                 = 2,
    POINT_ID_TRANSITION_1           = 3,
    POINT_ID_FLIGHT                 = 4,
    POINT_ID_SURGE                  = 5,
    POINT_ID_TRANSITION_2           = 6,
    POINT_ID_PHASE_3                = 7,
    POINT_ID_COMBAT_INTERMEDIATE    = 8,

    // light overrid id
    LIGHT_ID_DEFAULT                = 1773,
    LIGHT_ID_OBSCURE_SPACE          = 1822,
    LIGHT_ID_CHANGE_DIMENSIONS      = 1823,
    LIGHT_ID_ARCANE_RUNES           = 1824,
    LIGHT_ID_OBSCURE_ARCANE_RUNES   = 1825,

    AREA_ID_EYE_OF_ETERNITY         = 4500,

    SPELLLIST_INITIAL_N = 2885900,
    SPELLLIST_INITIAL_H = 3173400,

    SPELLLIST_REPEAT_N = 2885901,
    SPELLLIST_REPEAT_H = 3173401,

    SPELLLIST_DRAGONS_N = 2885902,
    SPELLLIST_DRAGONS_H = 3173402,
};

enum MalygosPhase
{
    // phases
    PHASE_FLOOR             = 1,
    PHASE_DISCS             = 2,
    PHASE_DRAGONS           = 3,

    PHASE_TRANSITION_FLOOR,
    PHASE_TRANSITION_DISKS,
    PHASE_TRANSITION_SURGE,
    PHASE_TRANSITION_VORTEX,
    PHASE_TRANSITION_DRAGONS,
    PHASE_VORTEX,
    PHASE_SURGE,
};

static const DialogueEntry aIntroDialogue[] =
{
    // Phase transitions
    {SAY_END_PHASE_1,             NPC_MALYGOS,  3000},
    {LIGHT_ID_ARCANE_RUNES,       0,            22000},
    {PHASE_DISCS,                 0,            0},

    {SAY_END_PHASE_2,             NPC_MALYGOS,  6000},
    {LIGHT_ID_OBSCURE_SPACE,      0,            4000},
    {SPELL_DESTROY_PLATFORM_PRE,  0,            4000},
    {SPELL_DESTROY_PLATFORM_BOOM, 0,            1500},
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

enum MalygosActions
{
    MALYGOS_BERSERK,
    MALYGOS_PHASE_TRANSITION_DISKS,
    MALYGOS_VORTEX,
    MALYGOS_ARCANE_PULSE,
    MALYGOS_ARCANE_BOMB,
    MALYGOS_ACTION_MAX,
    MALYGOS_VORTEX_DELAY,
    MALYGOS_VORTEX_END,
    MALYGOS_ARCANE_PULSE_END,
};

struct boss_malygosAI : public CombatAI, private DialogueHelper
{
    boss_malygosAI(Creature* creature) : CombatAI(creature, MALYGOS_ACTION_MAX), DialogueHelper(aIntroDialogue), m_instance(static_cast<instance_eye_of_eternity*>(creature->GetInstanceData())), m_bIsRegularMode(creature->GetMap()->IsRegularDifficulty()),
        m_maxStaticFieldTargets(m_bIsRegularMode ? 1 : 3),
        m_maxStormTargets(m_bIsRegularMode ? 5 : 15),
        m_maxNexusLords(m_bIsRegularMode ? 2 : 4),
        m_maxScions(m_bIsRegularMode ? 4 : 8)
    {
        InitializeDialogueHelper(m_instance);
        AddTimerlessCombatAction(MALYGOS_PHASE_TRANSITION_DISKS, true);
        AddCombatAction(MALYGOS_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(MALYGOS_VORTEX, 35000u);
        AddCombatAction(MALYGOS_ARCANE_BOMB, true);

        AddCustomAction(MALYGOS_VORTEX_DELAY, true, [&]() { HandleVortexDelay(); });
        AddCustomAction(MALYGOS_VORTEX_END, true, [&]() { HandleVortexEnd(); });
        AddCustomAction(MALYGOS_ARCANE_PULSE_END, true, [&]() { HandleArcanePulseEnd(); });
    }

    instance_eye_of_eternity* m_instance;
    bool m_bIsRegularMode;

    uint8 m_uiIntroStage;

    MalygosPhase m_uiPhase;
    uint8 m_maxNexusLords;
    uint8 m_maxScions;

    uint8 m_maxStormTargets;
    uint8 m_maxStaticFieldTargets;

    void Reset() override
    {
        CombatAI::Reset();

        m_uiIntroStage          = 0;
        m_uiPhase               = PHASE_FLOOR;

        // reset flags
        m_creature->SetImmuneToPlayer(true);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_REAL_FLY_ANIM);

        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(false);

        // reset lights
        m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_DEFAULT, 0);

        ClearOnKillReactions();
        AddOnKillText(SAY_SLAY_1_A, SAY_SLAY_1_B, SAY_SLAY_1_C);

        m_creature->SetSpellList(m_bIsRegularMode ? SPELLLIST_INITIAL_N : SPELLLIST_INITIAL_H);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoBroadcastText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_MALYGOS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoBroadcastText(SAY_DEATH, m_creature);
        m_creature->SetLevitate(false);
        m_creature->SetHover(false);

        if (m_instance)
            m_instance->SetData(TYPE_MALYGOS, DONE);
    }

    void EnterEvadeMode() override
    {
        m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_DEFAULT, 0);

        if (m_instance)
            m_instance->SetData(TYPE_MALYGOS, FAIL);

        // Destroy the platform; this can be also handled by event 20158
        if (GameObject* pPlatform = m_instance->GetSingleGameObjectFromStorage(GO_PLATFORM))
        {
            pPlatform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
            pPlatform->SetGoState(GO_STATE_READY);
            m_creature->GetMap()->ChangeGOPathfinding(194232, 8546, true);
        }

        m_creature->SetRespawnDelay(60, true);
        m_creature->ForcedDespawn();
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType == FALL_MOTION_TYPE && pointId == EVENT_FALL)
        {
            SetCombatMovement(true);
            SetMeleeEnabled(true);
            DoStartMovement(m_creature->GetVictim());
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
        }

        // remove flight anim and start moving
        if (moveType == POINT_MOTION_TYPE)
        {
            switch (pointId)
            {
                case POINT_ID_COMBAT_INTERMEDIATE:
                    m_creature->SetLevitate(false);
                    m_creature->SetHover(false);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_REAL_FLY_ANIM);
                    m_creature->GetMotionMaster()->MoveFall();
                    break;
                case POINT_ID_COMBAT:
                    // after vortex phase inform the Power sparks and set phase
                    if (m_uiPhase != PHASE_FLOOR)
                    {
                        SetPhase(PHASE_FLOOR);
                    }
                    break;
                case POINT_ID_VORTEX:
                    SetPhase(PHASE_VORTEX);
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
                    SetPhase(PHASE_SURGE);
                    break;
            }
        }
        // intro yells; handled once per boss circle; ToDo: maybe this is more dyanmic than this
        else if (moveType == WAYPOINT_MOTION_TYPE && pointId == 1 && m_uiIntroStage < 5 && !m_creature->IsInCombat())
        {
            switch (m_uiIntroStage)
            {
                case 0: DoBroadcastText(SAY_INTRO_1, m_creature); break;
                case 1: DoBroadcastText(SAY_INTRO_2, m_creature); break;
                case 2: DoBroadcastText(SAY_INTRO_3, m_creature); break;
                case 3: DoBroadcastText(SAY_INTRO_4, m_creature); break;
                case 4: DoBroadcastText(SAY_INTRO_5, m_creature); break;
            }

            ++m_uiIntroStage;
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ARCANE_OVERLOAD:
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCanEnterCombat(false);
                DoCastSpellIfCan(summoned, SPELL_ARCANE_BOMB, CAST_TRIGGERED);
                break;
            case NPC_STATIC_FIELD:
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->SetCanEnterCombat(false);
                summoned->CastSpell(summoned, SPELL_STATIC_FIELD, TRIGGERED_NONE);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        // start third phase
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            SetPhase(PHASE_TRANSITION_DRAGONS);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            // arcane storm spell
            if (m_uiPhase == PHASE_FLOOR)
                DoCastSpellIfCan(invoker, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H, CAST_TRIGGERED);
            else if (m_uiPhase == PHASE_DRAGONS)
                DoCastSpellIfCan(invoker, SPELL_ARCANE_STORM_VEHICLE, CAST_TRIGGERED);
        }
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        // Handle yell on Power Spark hit
        if (spellInfo->Id == SPELL_POWER_SPARK_MALYGOS && caster->GetEntry() == NPC_POWER_SPARK && m_uiPhase == PHASE_FLOOR)
            DoBroadcastText(SAY_SPARK_BUFF, m_creature);
    }

    void JustDidDialogueStep(int32 textEntry) override
    {
        switch (textEntry)
        {
            case LIGHT_ID_ARCANE_RUNES:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_ARCANE_RUNES, 5);
                break;
            case PHASE_DISCS:
                SetPhase(PHASE_DISCS);
                break;
            case SAY_END_PHASE_2:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_CHANGE_DIMENSIONS, 5);
                break;
            case LIGHT_ID_OBSCURE_SPACE:
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_OBSCURE_SPACE, 5);
                break;
            case SPELL_DESTROY_PLATFORM_PRE:
                DoCastSpellIfCan(m_creature, SPELL_DESTROY_PLATFORM_PRE);
                break;
            case SPELL_DESTROY_PLATFORM_BOOM:
                if (m_instance)
                {
                    if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(NPC_ALEXSTRASZA_INVIS))
                    {
                        trigger->CastSpell(nullptr, SPELL_DESTROY_PLATFORM_EVENT, TRIGGERED_NONE);
                        trigger->CastSpell(nullptr, SPELL_DESTROY_PLATFORM_BOOM, TRIGGERED_OLD_TRIGGERED);
                    }
                }
                break;
            case SPELL_SUMMON_RED_DRAGON:
                if (m_instance)
                {
                    // Destroy the platform; this can be also handled by event 20158
                    if (GameObject* pPlatform = m_instance->GetSingleGameObjectFromStorage(GO_PLATFORM))
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
                SetPhase(PHASE_DRAGONS);
                break;
        }
    }

    // Wrapper to spawn the adds in phase 2
    void DoSpawnAdds()
    {
        // spawn the nexus lord disks; 2 for normal | 4 for heroic
        for (uint8 i = 0; i < m_maxNexusLords; ++i)
            m_creature->SummonCreature(NPC_HOVER_DISK_LORD, lordDiskSummonData[i].x, lordDiskSummonData[i].y, lordDiskSummonData[i].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, lordDiskSummonData[i].pathId);

        // spawn the scion hover disks; 4 for normal | 8 for heroic
        for (uint8 i = 0; i < m_maxScions; ++i)
            m_creature->SummonCreature(NPC_HOVER_DISK_SCION, scionDiskSummonData[i].x, scionDiskSummonData[i].y, scionDiskSummonData[i].z, 0, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, scionDiskSummonData[i].pathId);
    }

    void HandleVortexDelay()
    {
        DoCastSpellIfCan(nullptr, SPELL_VORTEX, CAST_TRIGGERED);
    }

    void HandleVortexEnd()
    {
        SetPhase(PHASE_TRANSITION_FLOOR);
    }

    void HandleArcanePulseEnd()
    {
        m_creature->GetMotionMaster()->UnpauseWaypoints();
        SetPhase(PHASE_DISCS);
    }

    void SetPhase(MalygosPhase newPhase)
    {
        switch (newPhase)
        {
            case PHASE_FLOOR:
            {
                if (m_uiPhase == PHASE_TRANSITION_FLOOR)
                    m_creature->CastSpell(nullptr, SPELL_COOLDOWN_CREATURE_SPECIAL, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
                SendAIEventAround(AI_EVENT_CUSTOM_D, m_creature, 0, 50.0f);
                m_creature->SetSpellList(m_bIsRegularMode ? SPELLLIST_REPEAT_N : SPELLLIST_REPEAT_H);
                ResetCombatAction(MALYGOS_VORTEX, 45000);
                SetMeleeEnabled(true);
                break;
            }
            case PHASE_TRANSITION_VORTEX:
            {
                SetCombatMovement(false);
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_REAL_FLY_ANIM);

                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_VORTEX, aVortexMovePos[0], aVortexMovePos[1], aVortexMovePos[2]);

                SendAIEventAround(AI_EVENT_CUSTOM_C, m_creature, 0, 50.0f);

                DoBroadcastText(SAY_VORTEX, m_creature);
                break;
            }
            case PHASE_VORTEX:
            {
                if (m_instance)
                {
                    if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER))
                        trigger->CastSpell(trigger, SPELL_VORTEX_VISUAL, TRIGGERED_NONE);
                }
                DoCastSpellIfCan(nullptr, SPELL_VORTEX_STUN, CAST_TRIGGERED);
                m_creature->GetMotionMaster()->MoveIdle();
                ResetTimer(MALYGOS_VORTEX_END, 15000);
                ResetTimer(MALYGOS_VORTEX_DELAY, 1200);
                SetMeleeEnabled(false);
                break;
            }
            case PHASE_TRANSITION_FLOOR:
            {
                m_creature->RemoveAurasDueToSpell(SPELL_VORTEX_STUN);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, aCenterMovePos[0], aCenterMovePos[1], aCenterMovePos[2]);
                break;
            }
            case PHASE_TRANSITION_DISKS:
            {
                DisableCombatAction(MALYGOS_VORTEX);
                m_creature->SetSpellList(0);
                SetCombatMovement(false);
                SetMeleeEnabled(false);
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_REAL_FLY_ANIM);

                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_TRANSITION_1, m_creature->GetPositionX(), m_creature->GetPositionY(), 300.f);

                StartNextDialogueText(SAY_END_PHASE_1);
                break;
            }
            case PHASE_DISCS:
            {
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_FLIGHT, aHoverMovePos[0], aHoverMovePos[1], aHoverMovePos[2]);
                ClearOnKillReactions();
                AddOnKillText(SAY_SLAY_2_A, SAY_SLAY_2_B, SAY_SLAY_2_C);
                DoSpawnAdds();
                ResetCombatAction(MALYGOS_ARCANE_PULSE, m_uiPhase == PHASE_TRANSITION_DISKS ? 60000 : 55000);
                ResetCombatAction(MALYGOS_ARCANE_BOMB, 1000);
                break;
            }
            case PHASE_TRANSITION_SURGE:
            {
                // pause WP movement; don't clear the movegen
                m_creature->GetMotionMaster()->PauseWaypoints(15000);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_SURGE, aSurgeMovePos[0], aSurgeMovePos[1], aSurgeMovePos[2]);
                break;
            }
            case PHASE_SURGE:
                if (DoCastSpellIfCan(nullptr, SPELL_SURGE_OF_POWER_PULSE) == CAST_OK)
                {
                    DoBroadcastText(SAY_DEEP_BREATH, m_creature);
                    DoBroadcastText(SAY_EMOTE_BREATH, m_creature);
                    ResetTimer(MALYGOS_ARCANE_PULSE_END, 8000);
                }
                break;
            case PHASE_TRANSITION_DRAGONS:
            {
                SetCombatScriptStatus(true);
                DisableCombatAction(MALYGOS_ARCANE_PULSE);
                DisableCombatAction(MALYGOS_ARCANE_BOMB);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_TRANSITION_2, aTransitionPos[0], aTransitionPos[1], aTransitionPos[2]);

                StartNextDialogueText(SAY_END_PHASE_2);
                break;
            }
            case PHASE_DRAGONS:
            {
                SetCombatScriptStatus(false);
                m_creature->GetMap()->SetZoneOverrideLight(AREA_ID_EYE_OF_ETERNITY, 0, LIGHT_ID_OBSCURE_ARCANE_RUNES, 5);
                DoCastSpellIfCan(nullptr, SPELL_CLEAR_ALL_DEBUFFS, CAST_TRIGGERED);
                DoCastSpellIfCan(nullptr, SPELL_IMMUNE_CURSES, CAST_TRIGGERED);
                ClearOnKillReactions();
                AddOnKillText(SAY_SLAY_3_A, SAY_SLAY_3_B, SAY_SLAY_3_C);
                m_creature->SetSpellList(m_bIsRegularMode ? SPELLLIST_DRAGONS_N : SPELLLIST_DRAGONS_H);
                break;
            }
        }
        m_uiPhase = newPhase;        
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MALYGOS_BERSERK:
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    ResetCombatAction(action, 600000);
                break;
            case MALYGOS_PHASE_TRANSITION_DISKS:
                if (m_creature->GetHealthPercent() < 50.0f && m_uiPhase != PHASE_VORTEX)
                {
                    SetPhase(PHASE_TRANSITION_DISKS);
                    DisableCombatAction(action);
                }
                break;
            case MALYGOS_VORTEX:
                SetPhase(PHASE_TRANSITION_VORTEX);
                DisableCombatAction(action);
                break;
            case MALYGOS_ARCANE_BOMB:
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_ARCANE_BOMB) == CAST_OK)
                {
                    m_creature->GetMotionMaster()->PauseWaypoints(1000);
                    ResetCombatAction(action, urand(16000, 19000));
                }
                break;
            case MALYGOS_ARCANE_PULSE:
                SetPhase(PHASE_TRANSITION_SURGE);
                break;
        }
    }

    void OnSpellCast(SpellEntry const* spellInfo, Unit* /*target*/) override
    {
        if (spellInfo->Id == SPELL_SURGE_OF_POWER || spellInfo->Id == SPELL_SURGE_OF_POWER_H)
            if (!urand(0, 3))
                DoBroadcastText(SAY_SURGE, m_creature);
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);
        CombatAI::UpdateAI(diff);
    }
};

/*######
## npc_power_spark
######*/

struct npc_power_sparkAI : public ScriptedAI
{
    npc_power_sparkAI(Creature* creature) : ScriptedAI(creature), m_instance(dynamic_cast<instance_eye_of_eternity*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        m_creature->SetCanEnterCombat(false);

        DoBroadcastText(SAY_EMOTE_SPARK, m_creature);
        Reset();
    }

    instance_eye_of_eternity* m_instance;

    void Reset() override
    {
        if (m_instance)
        {
            if (Creature* malygos = m_instance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                m_creature->GetMotionMaster()->MoveChase(malygos, 0.f, 0.f, false, true, false);
        }

        DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_VISUAL);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetEntry() == NPC_MALYGOS && m_creature->CanReachWithMeleeAttack(who))
        {
            DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_MALYGOS, CAST_TRIGGERED);
            m_creature->ForcedDespawn();
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        // start / stop following during vortex
        if (eventType == AI_EVENT_CUSTOM_C)
            m_creature->GetMotionMaster()->MoveIdle();
        else if (eventType == AI_EVENT_CUSTOM_D)
            m_creature->GetMotionMaster()->MoveChase(sender, 0.f, 0.f, false, true, false);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_SPARK_PLAYERS, CAST_TRIGGERED);
    }
};

/*######
## event_go_focusing_iris
######*/

bool ProcessEventId_event_go_focusing_iris(uint32 /*eventId*/, Object* source, Object* /*target*/, bool /*isStart*/)
{
    if (instance_eye_of_eternity* pInstance = (instance_eye_of_eternity*)((Creature*)source)->GetInstanceData())
    {
        if (source->GetTypeId() != TYPEID_PLAYER)
            return false;

        if (pInstance->GetData(TYPE_MALYGOS) == IN_PROGRESS || pInstance->GetData(TYPE_MALYGOS) == DONE)
            return false;

        Creature* malygos = pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS);
        Creature* trigger = pInstance->GetSingleCreatureFromStorage(NPC_LARGE_TRIGGER);
        if (!malygos || !trigger)
            return false;

        // interrupt spells and allow attack
        malygos->InterruptNonMeleeSpells(false);
        malygos->SetImmuneToPlayer(false);

        // Enter combat area - Move to ground point first, then start chasing target
        float fX, fY, fZ;
        trigger->GetNearPoint(trigger, fX, fY, fZ, 0, 30.0f, trigger->GetAngle(malygos));
        malygos->GetMotionMaster()->MovePoint(POINT_ID_COMBAT_INTERMEDIATE, fX, fY, malygos->GetPositionZ());

        return true;
    }
    return false;
}

// 56072 - Ride Red Dragon Buddy
struct RideRedDragonBuddy : public SpellScript
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

        Position pos = caster->GetPosition();
        pos.z = 275.f; // TODO: each dragon has diff slightly
        caster->GetMotionMaster()->MovePoint(0, pos, FORCED_MOVEMENT_RUN);

        // player cast 56071 on caster
        target->CastSpell(caster, uiSpell, TRIGGERED_OLD_TRIGGERED);
    }
};

// 56430 - Arcane Bomb
struct ArcaneBomb : public SpellScript
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

// 57459, 61693, 61694 - Arcane Storm
struct ArcaneStorm : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        switch (spell->m_spellInfo->Id)
        {
            case 57459: spell->GetCaster()->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? spell->SetMaxAffectedTargets(4) : spell->SetMaxAffectedTargets(10); break;
            case 61693: spell->SetMaxAffectedTargets(5); break;
            case 61694: spell->SetMaxAffectedTargets(15); break;
        }
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_ARCANE_STORM_MASTER, TRIGGERED_OLD_TRIGGERED);
    }
};

// 56105 - Vortex
struct Vortex : public SpellScript
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

// 55853, 56263, 56264, 56265, 56266, 61071, 61072, 61073, 61074, 61075 - Vortex
struct VortexAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (apply || !target || !caster || !caster->IsPlayer())
            return;

        // teleport vehicle passenger outside of the vortex
        if (!apply)
            target->CastSpell(caster, 73040, TRIGGERED_OLD_TRIGGERED);
    }
};

// 61210 - Align Disk Aggro
struct AlignDiskAggro : public SpellScript
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

// 60936 - Surge of Power
struct SurgeOfPowerHeroic : public SpellScript
{
    void OnSuccessfulStart(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, SPELL_SURGE_OF_POWER_WARNING, TRIGGERED_IGNORE_CURRENT_CASTED_SPELL | TRIGGERED_IGNORE_GCD);
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

    RegisterSpellScript<RideRedDragonBuddy>("spell_ride_red_dragon_buddy");
    RegisterSpellScript<ArcaneBomb>("spell_arcane_bomb");
    RegisterSpellScript<ArcaneStorm>("spell_arcane_storm");
    RegisterSpellScript<Vortex>("spell_vortex");
    RegisterSpellScript<VortexAura>("spell_vortex_aura");
    RegisterSpellScript<AlignDiskAggro>("spell_align_disk_aggro");
    RegisterSpellScript<SurgeOfPowerHeroic>("spell_surge_of_power_heroic");
}
