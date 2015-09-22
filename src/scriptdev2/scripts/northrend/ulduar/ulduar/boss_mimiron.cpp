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
SDName: boss_mimiron
SD%Complete: 95%
SDComment: Laser Barrage rotation require additional research.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_INTRO                               = -1603176,
    SAY_HARD_MODE                           = -1603177,
    SAY_BERSERK                             = -1603178,

    SAY_TANK_ACTIVE                         = -1603179,
    SAY_TANK_SLAY_1                         = -1603180,
    SAY_TANK_SLAY_2                         = -1603181,
    SAY_TANK_DEATH                          = -1603182,

    SAY_TORSO_ACTIVE                        = -1603183,
    SAY_TORSO_SLAY_1                        = -1603184,
    SAY_TORSO_SLAY_2                        = -1603185,
    SAY_TORSO_DEATH                         = -1603186,

    SAY_HEAD_ACTIVE                         = -1603187,
    SAY_HEAD_SLAY_1                         = -1603188,
    SAY_HEAD_SLAY_2                         = -1603189,
    SAY_HEAD_DEATH                          = -1603190,

    SAY_ROBOT_ACTIVE                        = -1603191,
    SAY_ROBOT_SLAY_1                        = -1603192,
    SAY_ROBOT_SLAY_2                        = -1603193,
    SAY_ROBOT_DEATH                         = -1603194,

    EMOTE_PLASMA_BLAST                      = -1603196,

    SAY_SELF_DESTRUCT                       = -1603248,
    SAY_SELF_DESTRUCT_END                   = -1603260,
    SAY_DESTRUCT_10_MIN                     = -1603249,
    SAY_DESTRUCT_9_MIN                      = -1603250,
    SAY_DESTRUCT_8_MIN                      = -1603251,
    SAY_DESTRUCT_7_MIN                      = -1603252,
    SAY_DESTRUCT_6_MIN                      = -1603253,
    SAY_DESTRUCT_5_MIN                      = -1603254,
    SAY_DESTRUCT_4_MIN                      = -1603255,
    SAY_DESTRUCT_3_MIN                      = -1603256,
    SAY_DESTRUCT_2_MIN                      = -1603257,
    SAY_DESTRUCT_1_MIN                      = -1603258,
    SAY_DESTRUCT_0_MIN                      = -1603259,

    // mimiron spells
    SPELL_WELD                              = 63339,
    SPELL_TELEPORT_VISUAL                   = 41232,
    SPELL_TELEPORT_VISUAL_2                 = 64446,
    SPELL_RIDE_VEHICLE_MIMIRON_0            = 52391,                    // seat 0
    SPELL_RIDE_VEHICLE_MIMIRON_1            = 63313,                    // seat 1
    SPELL_RIDE_VEHICLE_MIMIRON_2            = 63314,                    // seat 2
    SPELL_RIDE_VEHICLE_MIMIRON_3            = 63315,                    // seat 3
    SPELL_RIDE_VEHICLE_MIMIRON_4            = 63316,                    // seat 4
    SPELL_RIDE_VEHICLE_MIMIRON_5            = 63344,                    // seat 5
    SPELL_RIDE_VEHICLE_MIMIRON_6            = 63345,                    // seat 6
    SPELL_JET_PACK_VISUAL                   = 63307,                    // fly animation
    SPELL_JET_PACK                          = 63341,                    // seat switch spell
    SPELL_SLEEP_VISUAL                      = 64393,
    SPELL_SLEEP_WAKE                        = 64394,
    SPELL_NOT_FRIENDLY_FIRE                 = 65040,                    // achiev check spell
    SPELL_BERSERK                           = 26662,

    // generic spells
    SPELL_FREEZE_ANIM                       = 16245,
    SPELL_SELF_REPAIR                       = 64383,                    // self repair for the robot phase
    SPELL_HALF_HEAL                         = 64188,                    // heal to prepare the robot phase
    SPELL_CLEAR_DEBUFFS                     = 34098,
    SPELL_RIDE_VEHICLE_ROBOT_1              = 64387,                    // seat 7
    SPELL_RIDE_VEHICLE_ROBOT_2              = 64388,                    // seat 3
    SPELL_VEHICLE_DAMAGED                   = 63415,
    SPELL_DESPAWN_ASSAULT_BOTS              = 64463,

    // Leviathan spells
    SPELL_PROXIMITY_MINES                   = 63027,                    // triggers 65347
    SPELL_NAPALM_SHELL                      = 64539,                    // triggers 63667 which casts 63666 or 65026
    SPELL_PLASMA_BLAST                      = 62997,                    // cast by the turret
    SPELL_PLASMA_BLAST_H                    = 64529,
    SPELL_SHOCK_BLAST                       = 63631,
    SPELL_FREEZE_ANIM_DEFEATED              = 63354,                    // visual aura after defeat
    SPELL_FLAME_SUPPRESSANT                 = 64570,                    // hard mode spells
    SPELL_EMERGENCY_MODE_LEVIATHAN          = 65101,

    // VX001 spells
    SPELL_RAPID_BURST_SUMMON                = 64840,
    SPELL_RAPID_BURST_EFFECT                = 64841,
    SPELL_RAPID_BURST_AURA                  = 63382,                    // triggers alternative the left and right Rapid Burst or Hand Pulse
    // SPELL_RAPID_BURST_LEFT               = 63387,                    // used in the VX phase; each spell has a different robot animation
    // SPELL_RAPID_BURST_RIGHT              = 64019,
    // SPELL_RAPID_BURST_LEFT_H             = 64531,
    // SPELL_RAPID_BURST_RIGHT_H            = 64532,
    // SPELL_LASER_BARRAGE                  = 63274,
    SPELL_SPINNING_UP                       = 63414,                    // triggers 63274 and 66490;
    SPELL_ROCKET_STRIKE                     = 64402,                    // targets npc 34050; triggers 63681 and 63036 from target; will spawn npc 34047
    SPELL_HEAT_WAVE                         = 63679,
    SPELL_HEAT_WAVE_H                       = 64534,
    SPELL_HAND_PULSE_LEFT                   = 64348,                    // spells used only in the final phase
    SPELL_HAND_PULSE_RIGHT                  = 64352,                    // each as a different visual for robot hand animation
    SPELL_HAND_PULSE_LEFT_H                 = 64536,
    SPELL_HAND_PULSE_RIGHT_H                = 64537,
    SPELL_TORSO_DISABLED                    = 64120,                    // visual aura on defeat
    SPELL_FLAME_SUPPRESSANT_CLOSE           = 65192,                    // hard mode spell
    SPELL_FROST_BOMB_SUMMON                 = 64623,                    // hard mode spell; triggers 64627 in order to summon npc 34149

    // Aerial unit spells
    SPELL_PLASMA_BALL_FLY                   = 63689,                    // used during the air phase
    SPELL_PLASMA_BALL_FLY_H                 = 64535,
    SPELL_PLASMA_BALL                       = 65647,                    // used during the final phase
    SPELL_PLASMA_BALL_H                     = 65648,
    SPELL_SUMMON_ASSAULT_BOT_TRIGGER        = 64425,                    // triggers 64427 and 64426; used to summon npc 34057
    SPELL_SUMMON_ASSAULT_BOT_VISUAL         = 64426,
    SPELL_SUMMON_ASSAULT_BOT                = 64427,
    SPELL_SUMMON_SCRAP_BOT_TRIGGER          = 63820,                    // triggers 63819 and 64398; used to summon npc 33855
    SPELL_SUMMON_SCRAP_BOT_VISUAL           = 64398,
    SPELL_SUMMON_SCRAP_BOT                  = 63819,
    SPELL_BOMB_BOT_SUMMON                   = 63811,                    // summon npc 33836
    SPELL_MAGNETIC_CORE_PULL                = 64436,
    SPELL_MAGNETIC_CORE_VISUAL              = 64438,
    SPELL_SUMMON_FIRE_BOT_TRIGGER           = 64620,                    // triggers 64621, 64622; hard mode spell; used to summon npc 34147
    SPELL_SUMMON_FIRE_BOT_VISUAL            = 64621,
    SPELL_SUMMON_FIRE_BOT                   = 64622,

    // proximity mine
    SPELL_PROXIMITY_MINE                    = 65345,
    SPELL_EXPLOSION                         = 66351,
    SPELL_EXPLOSION_H                       = 63009,

    // bots spells
    // SPELL_BOMB_BOT                       = 63767,
    SPELL_ROCKET_STRIKE_DAMAGE              = 64064,

    // hard mode spells
    SPELL_SELF_DESTRUCTION                  = 64613,
    SPELL_SELF_DESTRUCTION_DAMAGE           = 64610,
    SPELL_EMERGENCY_MODE                    = 64582,

    // fire spells
    SPELL_SUMMON_FLAMES_INITIAL             = 64563,                    // cast by npc 21252
    SPELL_FLAMES                            = 64561,                    // cast by npcs 34363 and 34121
    SPELL_SUMMON_FLAMES_SPREAD              = 64562,                    // cast by npc 34363

    // frost bomb spells
    SPELL_EXPLOSION_FROST                   = 64626,
    SPELL_FROST_BOMB_VISUAL                 = 64624,
    SPELL_CLEAR_FIRES                       = 65354,

    // summoned
    NPC_PROXIMITY_MINE                      = 34362,                    // has aura 65345
    NPC_ROCKET_VISUAL                       = 34050,                    // mounted on vehicle 33651
    NPC_ROCKET_STRIKE                       = 34047,                    // has aura 64064
    NPC_BURST_TARGET                        = 34211,                    // casts 64841 on VX001 which triggers 63382
    NPC_ASSALT_BOT                          = 34057,                    // handled in eventAI
    NPC_BOMB_BOT                            = 33836,                    // has aura 63767; handled in eventAI
    NPC_MAGNETIC_CORE                       = 34068,                    // has auras 64438 and 64436
    NPC_JUNK_BOT                            = 33855,

    // hard mode summoned
    // NPC_FLAME_INITIAL                    = 34363,
    // NPC_FLAME_SPREAD                     = 34121,
    NPC_FROST_BOMB                          = 34149,
    // NPC_EMERGENCY_FIRE_BOT               = 34147,                    // handled in eventAI

    // other
    POINT_ID_PARK                           = 1,
    POINT_ID_CENTER                         = 2,
    SEAT_ID_TURRET                          = 4,

    // phases
    PHASE_INTRO                             = 0,
    PHASE_LEVIATHAN                         = 1,
    PHASE_VX001                             = 2,
    PHASE_AERIAL_UNIT                       = 3,
    PHASE_FULL_ROBOT                        = 4,
    PHASE_TRANSITION                        = 5,
    PHASE_DAMAGED                           = 6,
};

static const DialogueEntry aMimironDialogue[] =
{
    {NPC_MIMIRON,               0,              3000},              // encounter start, normal
    {SAY_TANK_ACTIVE,           NPC_MIMIRON,    6000},
    {PHASE_LEVIATHAN,           0,              3000},
    {NPC_LEVIATHAN_MK,          0,              0},

    {SAY_SELF_DESTRUCT,         NPC_COMPUTER,   3000},              // encounter start, hard mode
    {SAY_DESTRUCT_10_MIN,       NPC_COMPUTER,   3000},
    {SAY_HARD_MODE,             NPC_MIMIRON,    5000},
    {NPC_LEVIATHAN_MK,          0,              0},

    {NPC_LEVIATHAN_MK_TURRET,   0,              1000},              // Leviathan defeated, first transition
    {SAY_TANK_DEATH,            NPC_MIMIRON,    5000},
    {GO_MIMIRON_ELEVATOR,       0,              15000},
    {NPC_VX001,                 0,              8000},
    {SPELL_JET_PACK_VISUAL,     0,              1000},
    {SPELL_JET_PACK,            0,              3000},
    {SAY_TORSO_ACTIVE,          NPC_MIMIRON,    3000},
    {PHASE_VX001,               0,              3000},
    {SEAT_ID_TURRET,            0,              0},

    {SPELL_TORSO_DISABLED,      0,              5000},              // VX001 defeated, second transition
    {NPC_ROCKET_STRIKE,         0,              1000},
    {SAY_TORSO_DEATH,           NPC_MIMIRON,    5000},
    {NPC_AERIAL_UNIT,           0,              9000},
    {PHASE_TRANSITION,          0,              1000},
    {PHASE_AERIAL_UNIT,         0,              6000},
    {SAY_HEAD_ACTIVE,           NPC_MIMIRON,    3000},
    {NPC_MAGNETIC_CORE,         0,              0},

    {NPC_JUNK_BOT,              0,              4000},              // Aerial Unit defeated, last transition
    {SAY_HEAD_DEATH,            NPC_MIMIRON,    2000},
    {NPC_COMPUTER,              0,              4000},
    {SPELL_HALF_HEAL,           0,              4000},
    {NPC_BOMB_BOT,              0,              3000},
    {NPC_BURST_TARGET,          0,              4000},
    {SAY_ROBOT_ACTIVE,          NPC_MIMIRON,    3000},
    {NPC_ROCKET_VISUAL,         0,              4000},
    {NPC_PROXIMITY_MINE,        0,              0},

    {SPELL_SLEEP_VISUAL,        0,              7000},              // Robot defeated, epilogue
    {SPELL_SLEEP_WAKE,          0,              3000},
    {SAY_ROBOT_DEATH,           NPC_MIMIRON,    10000},
    {SPELL_TELEPORT_VISUAL,     0,              0},
    {0, 0, 0},
};

// a random list of seats which can be used by Mimiron in idle mode
static const uint32 aRandomAnimationSpells[] = {SPELL_RIDE_VEHICLE_MIMIRON_0, SPELL_RIDE_VEHICLE_MIMIRON_1, SPELL_RIDE_VEHICLE_MIMIRON_2, SPELL_RIDE_VEHICLE_MIMIRON_4};

// teleporters
static const uint32 aMimironTeleporters[] = {GO_MIMIRON_TEL1, GO_MIMIRON_TEL2, GO_MIMIRON_TEL3, GO_MIMIRON_TEL4, GO_MIMIRON_TEL5, GO_MIMIRON_TEL6, GO_MIMIRON_TEL7, GO_MIMIRON_TEL8, GO_MIMIRON_TEL9};

// spawn or move positions
static const float afTankEvadePos[4] = {2792.07f, 2596.32f, 364.3136f, 3.5f};
static const float afRobotSpawnPos[4] = {2744.431f, 2569.385f, 364.3968f, 3.141f};
static const float afRocketSpawnPos[4] = {2746.262f, 2567.085f, 369.2921f, 3.14f};
static const float afAerialSpawnPos[4] = {2744.365f, 2569.303f, 392.2355f, 3.15f};
static const float afAerialMovePos[3] = {2743.32f, 2569.285f, 378.2812f};
static const float afTankMovePos[3] = {2763.82f, 2568.87f, 364.3136f};
static const float afCenterMovePos[3] = {2744.61f, 2569.38f, 364.3136f};

/*######
## boss_mimiron
######*/

struct boss_mimironAI : public ScriptedAI, private DialogueHelper
{
    boss_mimironAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aMimironDialogue)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        InitializeDialogueHelper(m_pInstance);
        m_bHasDoneIntro = false;
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiDestructStage;
    uint32 m_uiDestructTimer;

    uint32 m_uiWeldTimer;
    uint32 m_uiAnimationTimer;
    uint32 m_uiCurrentSeatAura;

    uint32 m_uiWakeUpTimer;
    uint32 m_uiFlamesTimer;
    uint32 m_uiBerserkTimer;

    bool m_bHasDoneIntro;

    void Reset() override
    {
        m_uiPhase           = PHASE_INTRO;
        m_uiDestructStage   = 0;
        m_uiDestructTimer   = 0;
        m_uiWakeUpTimer     = 0;
        m_uiWeldTimer       = 1000;
        m_uiAnimationTimer  = 10000;
        m_uiFlamesTimer     = 0;
        m_uiBerserkTimer    = 0;
        m_uiCurrentSeatAura = SPELL_RIDE_VEHICLE_MIMIRON_0;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void AttackStart(Unit* /*pWho*/) override { }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasDoneIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 70.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasDoneIntro = true;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage) override
    {
        // start encounter on first damage
        if (m_uiPhase == PHASE_INTRO && uiDamage)
        {
            m_uiPhase = PHASE_LEVIATHAN;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);

            StartNextDialogueText(NPC_MIMIRON);
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_pInstance)
            return;

        switch (iEntry)
        {
                // Encounter intro (normal and hard mode)
            case NPC_MIMIRON:
            case NPC_LEVIATHAN_MK_TURRET:
                // jump on the top of the robot for intro / phase end text
                m_creature->RemoveAurasDueToSpell(SPELL_WELD);
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                {
                    pLeviathan->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                    DoCastSpellIfCan(pLeviathan, SPELL_RIDE_VEHICLE_MIMIRON_5, CAST_TRIGGERED);
                }
                break;
            case PHASE_LEVIATHAN:
            case SAY_HARD_MODE:
                // mount inside the robot
                m_creature->RemoveAurasDueToSpell(SPELL_WELD);
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                {
                    pLeviathan->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                    DoCastSpellIfCan(pLeviathan, SPELL_RIDE_VEHICLE_MIMIRON_6, CAST_TRIGGERED);

                    // hard mode aura
                    if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
                    {
                        pLeviathan->CastSpell(pLeviathan, SPELL_EMERGENCY_MODE, true);
                        pLeviathan->CastSpell(pLeviathan, SPELL_EMERGENCY_MODE_LEVIATHAN, true);
                    }

                    m_uiBerserkTimer = 15 * MINUTE * IN_MILLISECONDS;
                }
                break;
            case NPC_LEVIATHAN_MK:
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                {
                    pLeviathan->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                    pLeviathan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pLeviathan->SetInCombatWithZone();
                }
                // Note: maybe the flags are handled by the vehicle seats. Set them manually for the moment.
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                break;

                // Start phase 2 transition
            case GO_MIMIRON_ELEVATOR:
                m_pInstance->DoUseDoorOrButton(GO_MIMIRON_ELEVATOR);
                break;
            case NPC_VX001:
                if (GameObject* pElevator = m_pInstance->GetSingleGameObjectFromStorage(GO_MIMIRON_ELEVATOR))
                    pElevator->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                m_creature->SummonCreature(NPC_VX001, afRobotSpawnPos[0], afRobotSpawnPos[1], afRobotSpawnPos[2], afRobotSpawnPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
                break;
            case SPELL_JET_PACK_VISUAL:
                DoCastSpellIfCan(m_creature, SPELL_JET_PACK_VISUAL);
                break;
            case SPELL_JET_PACK:
                // fly from the Leviathan to VX001
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                    pLeviathan->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                    DoCastSpellIfCan(pVx001, SPELL_RIDE_VEHICLE_MIMIRON_0, CAST_TRIGGERED);
                break;
            case SAY_TORSO_ACTIVE:
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK_VISUAL);
                break;
            case PHASE_VX001:
                // mount inside the robot
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                {
                    pVx001->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                    DoCastSpellIfCan(pVx001, SPELL_RIDE_VEHICLE_MIMIRON_1, CAST_TRIGGERED);

                    // hard mode aura
                    if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
                        pVx001->CastSpell(pVx001, SPELL_EMERGENCY_MODE, true);
                }
                break;
            case SEAT_ID_TURRET:
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                {
                    pVx001->SetHealth(pVx001->GetMaxHealth());
                    pVx001->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                    pVx001->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pVx001->SetInCombatWithZone();
                }
                break;

                // Start phase 3 transition
            case NPC_ROCKET_STRIKE:
                // mount on the top of the robot for phase end text
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                {
                    pVx001->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                    DoCastSpellIfCan(pVx001, SPELL_RIDE_VEHICLE_MIMIRON_4, CAST_TRIGGERED);
                }
                break;
            case NPC_AERIAL_UNIT:
                m_creature->SummonCreature(NPC_AERIAL_UNIT, afAerialSpawnPos[0], afAerialSpawnPos[1], afAerialSpawnPos[2], afAerialSpawnPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
                break;
            case PHASE_TRANSITION:
                DoCastSpellIfCan(m_creature, SPELL_JET_PACK_VISUAL);
                break;
            case PHASE_AERIAL_UNIT:
                // mount inside the flying robot
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                    pVx001->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT))
                {
                    DoCastSpellIfCan(pAerial, SPELL_RIDE_VEHICLE_MIMIRON_0, CAST_TRIGGERED);

                    // hard mode aura
                    if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
                        pAerial->CastSpell(pAerial, SPELL_EMERGENCY_MODE, true);
                }
                break;
            case SAY_HEAD_ACTIVE:
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK_VISUAL);
                break;
            case NPC_MAGNETIC_CORE:
                if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT))
                {
                    pAerial->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pAerial->SetInCombatWithZone();
                }
                break;

                // Start phase 4 transition
            case NPC_COMPUTER:
                // get the tank into combat position
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                {
                    pLeviathan->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM_DEFEATED);
                    pLeviathan->GetMotionMaster()->MovePoint(POINT_ID_CENTER, afTankMovePos[0], afTankMovePos[1], afTankMovePos[2]);
                }
                break;
            case SPELL_HALF_HEAL:
            {
                // mount the torso on top of the tank
                Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK);
                Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001);
                if (!pLeviathan || !pVx001)
                    return;

                pVx001->RemoveAurasDueToSpell(SPELL_TORSO_DISABLED);
                pVx001->CastSpell(pLeviathan, SPELL_RIDE_VEHICLE_ROBOT_1, true);
                break;
            }
            case NPC_BOMB_BOT:
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                    pLeviathan->GetMotionMaster()->MovePoint(POINT_ID_CENTER, afCenterMovePos[0], afCenterMovePos[1], afCenterMovePos[2]);
                break;
            case NPC_BURST_TARGET:
            {
                // mount the head on top of the torso
                Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT);
                Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001);
                if (!pAerial || !pVx001)
                    return;

                pAerial->CastSpell(pVx001, SPELL_RIDE_VEHICLE_ROBOT_2, true);
                break;
            }
            case NPC_ROCKET_VISUAL:
                // switch from the head to inside the torso
                if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT))
                    pAerial->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                    DoCastSpellIfCan(pVx001, SPELL_RIDE_VEHICLE_MIMIRON_1, CAST_TRIGGERED);
                break;
            case NPC_PROXIMITY_MINE:
                // set the whole robot in combat and inform about phase 4
                if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT))
                {
                    SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pAerial);
                    pAerial->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001))
                {
                    SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pVx001);
                    pVx001->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pVx001->SetInCombatWithZone();
                }
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                {
                    SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, pLeviathan);
                    pLeviathan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                break;

                // Start encounter epilogue
            case SPELL_SLEEP_VISUAL:
                DoCastSpellIfCan(m_creature, SPELL_SLEEP_VISUAL);
                if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
                {
                    if (Creature* pComputer = m_pInstance->GetSingleCreatureFromStorage(NPC_COMPUTER))
                    {
                        DoScriptText(SAY_SELF_DESTRUCT_END, pComputer);
                        m_uiFlamesTimer = 0;
                        m_uiDestructTimer = 0;
                    }
                }
                break;
            case SPELL_SLEEP_WAKE:
                if (DoCastSpellIfCan(m_creature, SPELL_SLEEP_WAKE) == CAST_OK)
                    m_creature->RemoveAurasDueToSpell(SPELL_SLEEP_VISUAL);
                break;
            case SPELL_TELEPORT_VISUAL:
                if (DoCastSpellIfCan(m_creature, SPELL_TELEPORT_VISUAL) == CAST_OK)
                    m_creature->ForcedDespawn(2000);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_AERIAL_UNIT)
            pSummoned->GetMotionMaster()->MovePoint(1, afAerialMovePos[0], afAerialMovePos[1], afAerialMovePos[2]);
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        switch (eventType)
        {
                // Red button pressed
            case AI_EVENT_CUSTOM_A:
                StartNextDialogueText(SAY_SELF_DESTRUCT);
                m_uiPhase = PHASE_LEVIATHAN;

                if (m_pInstance)
                    m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
                m_uiDestructTimer = MINUTE * IN_MILLISECONDS;
                m_uiFlamesTimer = 7000;
                break;
                // Leviathan phase finished
            case AI_EVENT_CUSTOM_B:
                StartNextDialogueText(NPC_LEVIATHAN_MK_TURRET);
                break;
                // VX001 phase finished
            case AI_EVENT_CUSTOM_C:
                StartNextDialogueText(SPELL_TORSO_DISABLED);
                break;
                // Aerial unit phase finished
            case AI_EVENT_CUSTOM_D:
                StartNextDialogueText(NPC_COMPUTER);
                break;
                // Robot piece destroyed
            case AI_EVENT_CUSTOM_E:
                if (!m_uiWakeUpTimer)
                    m_uiWakeUpTimer = 10000;
                break;
        }
    }

    // function to switch to another seat on the Leviathan
    void DoFlyToNextRandomSeat()
    {
        if (!m_pInstance)
            return;

        Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK);
        if (!pLeviathan)
            return;

        m_creature->RemoveAurasDueToSpell(SPELL_WELD);
        pLeviathan->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());

        uint32 uiNextAuraId = aRandomAnimationSpells[urand(0, countof(aRandomAnimationSpells) - 1)];

        while (uiNextAuraId == m_uiCurrentSeatAura)
            uiNextAuraId = aRandomAnimationSpells[urand(0, countof(aRandomAnimationSpells) - 1)];

        m_uiCurrentSeatAura = uiNextAuraId;
        DoCastSpellIfCan(pLeviathan, m_uiCurrentSeatAura, CAST_TRIGGERED);
    }

    // function to trigger the flames explosion for hard mode
    void DoSpawnFlamesInitial()
    {
        if (!m_pInstance)
            return;

        Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK);
        Creature* pTrigger = m_pInstance->GetSingleCreatureFromStorage(NPC_WORLD_TRIGGER_FLAMES);
        if (!pLeviathan || !pTrigger)
            return;

        for (uint8 i = 0; i < 3; ++i)
        {
            // Select targets based on Leviathan threat list; if the Leviathan is not in combat select them using instance
            Unit* pTarget;
            if (pLeviathan->getVictim())
                pTarget = pLeviathan->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SUMMON_FLAMES_INITIAL, SELECT_FLAG_PLAYER);
            else
                pTarget = m_pInstance->GetPlayerInMap(true, false);

            if (pTarget)
                pTrigger->CastSpell(pTarget, SPELL_SUMMON_FLAMES_INITIAL, true);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
        {
            script_error_log("Instance Ulduar: ERROR Failed to load instance data for this instace.");
            return;
        }

        DialogueUpdate(uiDiff);

        if (m_uiPhase == PHASE_INTRO)
        {
            // in idle mode Mimiron jumps around the Leviathan
            if (m_uiAnimationTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_JET_PACK) == CAST_OK)
                {
                    DoFlyToNextRandomSeat();
                    m_uiWeldTimer = 2000;
                    m_uiAnimationTimer = urand(12000, 15000);
                }
            }
            else
                m_uiAnimationTimer -= uiDiff;

            if (m_uiWeldTimer)
            {
                if (m_uiWeldTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_WELD) == CAST_OK)
                        m_uiWeldTimer = 0;
                }
                else
                    m_uiWeldTimer -= uiDiff;
            }
        }

        if (m_uiWakeUpTimer)
        {
            // check if all robot pieces are damaged
            if (m_uiWakeUpTimer <= uiDiff)
            {
                Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK);
                Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001);
                Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT);
                if (!pAerial || !pVx001 || !pLeviathan)
                    return;

                // if all robot pieces are damaged finish the encounter
                if (pLeviathan->HasAura(SPELL_FREEZE_ANIM) && pVx001->getStandState() == UNIT_STAND_STATE_DEAD && pAerial->getStandState() == UNIT_STAND_STATE_DEAD)
                {
                    // eject from the robot and start dialogue
                    pVx001->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, m_creature->GetObjectGuid());
                    StartNextDialogueText(SPELL_SLEEP_VISUAL);

                    // kill the robot parts
                    m_creature->DealDamage(pLeviathan, pLeviathan->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    m_creature->DealDamage(pVx001, pVx001->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    m_creature->DealDamage(pAerial, pAerial->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                }
                m_uiWakeUpTimer = 0;
            }
            else
                m_uiWakeUpTimer -= uiDiff;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK))
                    pLeviathan->CastSpell(pLeviathan, SPELL_BERSERK, true);
                if (Creature* pVx001 = m_pInstance->GetSingleCreatureFromStorage(NPC_VX001, true))
                    pVx001->CastSpell(pVx001, SPELL_BERSERK, true);
                if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT, true))
                    pAerial->CastSpell(pAerial, SPELL_BERSERK, true);

                DoScriptText(SAY_BERSERK, m_creature);
                m_uiBerserkTimer = 0;
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiFlamesTimer)
        {
            if (m_uiFlamesTimer <= uiDiff)
            {
                DoSpawnFlamesInitial();
                m_uiFlamesTimer = urand(25000, 30000);
            }
            else
                m_uiFlamesTimer -= uiDiff;
        }

        if (m_uiDestructTimer)
        {
            // handle the platform destruction for hard mode
            if (m_uiDestructTimer <= uiDiff)
            {
                Creature* pComputer = m_pInstance->GetSingleCreatureFromStorage(NPC_COMPUTER);
                if (!pComputer)
                    return;

                ++m_uiDestructStage;
                m_uiDestructTimer = MINUTE * IN_MILLISECONDS;

                switch (m_uiDestructStage)
                {
                    case 1: DoScriptText(SAY_DESTRUCT_9_MIN, pComputer); break;
                    case 2: DoScriptText(SAY_DESTRUCT_8_MIN, pComputer); break;
                    case 3: DoScriptText(SAY_DESTRUCT_7_MIN, pComputer); break;
                    case 4: DoScriptText(SAY_DESTRUCT_6_MIN, pComputer); break;
                    case 5: DoScriptText(SAY_DESTRUCT_5_MIN, pComputer); break;
                    case 6: DoScriptText(SAY_DESTRUCT_4_MIN, pComputer); break;
                    case 7: DoScriptText(SAY_DESTRUCT_3_MIN, pComputer); break;
                    case 8: DoScriptText(SAY_DESTRUCT_2_MIN, pComputer); break;
                    case 9: DoScriptText(SAY_DESTRUCT_1_MIN, pComputer); break;
                    case 10:
                        DoScriptText(SAY_DESTRUCT_0_MIN, pComputer);
                        pComputer->CastSpell(pComputer, SPELL_SELF_DESTRUCTION, true);
                        pComputer->CastSpell(pComputer, SPELL_SELF_DESTRUCTION_DAMAGE, true);
                        m_uiDestructTimer = 0;
                        break;
                }
            }
            else
                m_uiDestructTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

/*######
## boss_leviathan_mk2
######*/

struct boss_leviathan_mk2AI : public ScriptedAI
{
    boss_leviathan_mk2AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_uiPhase = PHASE_INTRO;
        m_uiMountTimer = 1000;
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;

    uint32 m_uiMountTimer;

    uint32 m_uiMinesTimer;
    uint32 m_uiNapalmTimer;
    uint32 m_uiPlasmaBlastTimer;
    uint32 m_uiShockBlastTimer;
    uint32 m_uiFlameSuppressTimer;

    void Reset() override
    {
        m_uiMinesTimer          = 1000;
        m_uiNapalmTimer         = 20000;
        m_uiPlasmaBlastTimer    = 10000;
        m_uiShockBlastTimer     = 30000;
        m_uiFlameSuppressTimer  = 0;

        SetCombatMovement(true);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_uiPhase = PHASE_LEVIATHAN;

        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
            m_uiFlameSuppressTimer = 50000;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, DONE);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (pDoneBy->GetEntry() == NPC_MIMIRON && m_uiPhase == PHASE_DAMAGED)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_uiPhase == PHASE_LEVIATHAN)
            {
                // unmount and destroy the turret
                if (m_pInstance)
                {
                    if (Creature* pTurret = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK_TURRET))
                    {
                        m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE, pTurret->GetObjectGuid());
                        m_creature->DealDamage(pTurret, pTurret->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    }
                }

                // move to parking position
                SetCombatMovement(false);
                m_uiPhase = PHASE_TRANSITION;
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_PARK, afTankEvadePos[0], afTankEvadePos[1], afTankEvadePos[2]);
            }
            else if (m_uiPhase == PHASE_FULL_ROBOT)
            {
                // start self repair
                if (DoCastSpellIfCan(m_creature, SPELL_SELF_REPAIR, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_VEHICLE_DAMAGED, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM, CAST_TRIGGERED);

                    // inform Mimiron about the damaged state
                    if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                        SendAIEvent(AI_EVENT_CUSTOM_E, m_creature, pMimiron);

                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_uiPhase = PHASE_DAMAGED;
                }
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // self repair succesfull; resume fight
        if (pSpell->Id == SPELL_SELF_REPAIR)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->Clear();
            DoStartMovement(m_creature->getVictim());
            m_uiPhase = PHASE_FULL_ROBOT;
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MIMIRON, FAIL);

            // respawn the turret if necessary
            if (Creature* pTurret = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK_TURRET))
            {
                if (!pTurret->isAlive())
                    pTurret->Respawn();
            }
        }

        DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        // reset all the vehicle accessories
        m_uiMountTimer = 1000;
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
        {
            if (m_uiPhase == PHASE_FULL_ROBOT)
                DoScriptText(urand(0, 1) ? SAY_ROBOT_SLAY_1 : SAY_ROBOT_SLAY_2, pMimiron);
            else
                DoScriptText(urand(0, 1) ? SAY_TANK_SLAY_1 : SAY_TANK_SLAY_2, pMimiron);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        if (uiPointId == POINT_ID_PARK)
        {
            // start transition phase
            if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, pMimiron);

            // park the Leviathan
            DoCastSpellIfCan(m_creature, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_HALF_HEAL, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM_DEFEATED, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetFacingTo(afTankEvadePos[3]);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // switch to full robot abilities
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            SetCombatMovement(true);
            DoStartMovement(m_creature->getVictim());
            m_uiPhase = PHASE_FULL_ROBOT;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_PROXIMITY_MINE)
            pSummoned->CastSpell(pSummoned, SPELL_PROXIMITY_MINE, true);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_pInstance)
        {
            script_error_log("Instance Ulduar: ERROR Failed to load instance data for this instace.");
            return;
        }

        // Mount Mimiron and the Turret manually
        if (m_uiMountTimer)
        {
            if (m_uiMountTimer <= uiDiff)
            {
                m_creature->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

                if (Creature* pTurret = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK_TURRET))
                {
                    int32 iSeat = (int32)SEAT_ID_TURRET;
                    pTurret->CastCustomSpell(m_creature, SPELL_RIDE_VEHICLE_HARDCODED, &iSeat, NULL, NULL, true);
                }

                if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                    pMimiron->CastSpell(m_creature, SPELL_RIDE_VEHICLE_MIMIRON_0, true);

                m_uiMountTimer = 0;
            }
            else
                m_uiMountTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // no combat during transition or when damaged
        if (m_uiPhase == PHASE_TRANSITION || m_uiPhase == PHASE_DAMAGED)
            return;

        // Leviathan phase spells
        if (m_uiPhase == PHASE_LEVIATHAN)
        {
            if (m_uiPlasmaBlastTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (Creature* pTurret = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK_TURRET))
                        pTurret->CastSpell(pTarget, m_bIsRegularMode ? SPELL_PLASMA_BLAST : SPELL_PLASMA_BLAST_H, false);

                    DoScriptText(EMOTE_PLASMA_BLAST, m_creature);
                    m_uiPlasmaBlastTimer = 30000;
                }
            }
            else
                m_uiPlasmaBlastTimer -= uiDiff;

            if (m_uiNapalmTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_NAPALM_SHELL) == CAST_OK)
                    m_uiNapalmTimer = 7000;
            }
            else
                m_uiNapalmTimer -= uiDiff;

            if (m_uiFlameSuppressTimer)
            {
                if (m_uiFlameSuppressTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_SUPPRESSANT) == CAST_OK)
                        m_uiFlameSuppressTimer = 60000;
                }
                else
                    m_uiFlameSuppressTimer -= uiDiff;
            }
        }

        if (m_uiMinesTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PROXIMITY_MINES) == CAST_OK)
                m_uiMinesTimer = 35000;
        }
        else
            m_uiMinesTimer -= uiDiff;

        if (m_uiShockBlastTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SHOCK_BLAST) == CAST_OK)
                m_uiShockBlastTimer = 34000;
        }
        else
            m_uiShockBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_leviathan_mk2(Creature* pCreature)
{
    return new boss_leviathan_mk2AI(pCreature);
}

/*######
## boss_vx001
######*/

struct boss_vx001AI : public ScriptedAI
{
    boss_vx001AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        DoCastSpellIfCan(m_creature, SPELL_FREEZE_ANIM, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_uiPhase = PHASE_INTRO;
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;

    uint32 m_uiRocketStrikeTimer;
    uint32 m_uiRapidBurstTimer;
    uint32 m_uLaserBarrageTimer;
    uint32 m_uiHandPulseTimer;
    uint32 m_uiBurstEndTimer;
    uint32 m_uiLaserEndTimer;
    uint32 m_uiFlameSuppressTimer;
    uint32 m_uiFrostBombTimer;

    void Reset() override
    {
        m_uiBurstEndTimer       = 0;
        m_uiLaserEndTimer       = 0;
        m_uiRapidBurstTimer     = 1000;
        m_uiHandPulseTimer      = 1000;
        m_uiRocketStrikeTimer   = 20000;
        m_uLaserBarrageTimer    = urand(30000, 60000);
        m_uiFlameSuppressTimer  = 0;
        m_uiFrostBombTimer      = 0;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_uiPhase == PHASE_INTRO)
        {
            m_uiPhase = PHASE_VX001;
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HEAT_WAVE : SPELL_HEAT_WAVE_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

            // Set levitate for animation purpose
            m_creature->SetLevitate(true);
        }

        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
        {
            m_uiFrostBombTimer     = 1000;
            m_uiFlameSuppressTimer = 5000;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (pDoneBy->GetEntry() == NPC_MIMIRON && m_uiPhase == PHASE_DAMAGED)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_uiPhase == PHASE_VX001)
            {
                // shut down the VX001
                if (DoCastSpellIfCan(m_creature, SPELL_TORSO_DISABLED, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    Reset();
                    m_uiPhase = PHASE_TRANSITION;

                    // start transition phase
                    if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                        SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, pMimiron);

                    DoCastSpellIfCan(m_creature, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_HALF_HEAL, CAST_TRIGGERED);

                    // custom evade in order to properly handle the animations
                    // Note: we won't remove all auras because of the hard mode; Debuffs should be removed by the spell above
                    // m_creature->RemoveAllAurasOnEvade();
                    m_creature->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_HEAT_WAVE : SPELL_HEAT_WAVE_H);
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop();
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }
            else if (m_uiPhase == PHASE_FULL_ROBOT)
            {
                // start self repair
                if (DoCastSpellIfCan(m_creature, SPELL_SELF_REPAIR, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    // inform Mimiron about the damaged state
                    if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                        SendAIEvent(AI_EVENT_CUSTOM_E, m_creature, pMimiron);

                    m_uiPhase = PHASE_DAMAGED;
                    m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                }
            }
        }
    }

    void EnterEvadeMode() override {}

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell) override
    {
        // self repair succesfull; resume fight
        if (pSpell->Id == SPELL_SELF_REPAIR)
        {
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_uiPhase = PHASE_FULL_ROBOT;
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
        {
            if (m_uiPhase == PHASE_FULL_ROBOT)
                DoScriptText(urand(0, 1) ? SAY_ROBOT_SLAY_1 : SAY_ROBOT_SLAY_2, pMimiron);
            else
                DoScriptText(urand(0, 1) ? SAY_TORSO_SLAY_1 : SAY_TORSO_SLAY_2, pMimiron);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // switch to full robot abilities
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_uiPhase = PHASE_FULL_ROBOT;

            // Set levitate for animation purpose
            m_creature->SetLevitate(true);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_BURST_TARGET:
                pSummoned->CastSpell(m_creature, SPELL_RAPID_BURST_EFFECT, true);
                m_uiBurstEndTimer = 3000;

                // Remove the target focus but allow the boss to face the burst target
                m_creature->SetTargetGuid(ObjectGuid());
                m_creature->SetFacingToObject(pSummoned);
                break;
            case NPC_FROST_BOMB:
                pSummoned->CastSpell(pSummoned, SPELL_FROST_BOMB_VISUAL, true);
                break;
        }
    }

    // Custom threat management
    bool SelectCustomHostileTarget()
    {
        Unit* pTarget = NULL;
        Unit* pOldTarget = m_creature->getVictim();

        if (!m_creature->getThreatManager().isThreatListEmpty())
            pTarget = m_creature->getThreatManager().getHostileTarget();

        if (pTarget)
        {
            if (pOldTarget != pTarget && !m_uiBurstEndTimer && !m_uiLaserEndTimer)
                AttackStart(pTarget);

            // Set victim to old target (if not while Burst or Laser)
            if (pOldTarget && pOldTarget->isAlive() && !m_uiBurstEndTimer && !m_uiLaserEndTimer)
            {
                m_creature->SetTargetGuid(pOldTarget->GetObjectGuid());
                m_creature->SetInFront(pOldTarget);
            }

            return true;
        }

        // Will call EnterEvadeMode if fit
        return m_creature->SelectHostileTarget() && m_creature->getVictim();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!SelectCustomHostileTarget())
            return;

        // no combat during transition or when damaged
        if (m_uiPhase == PHASE_TRANSITION || m_uiPhase == PHASE_DAMAGED)
            return;

        // count the burst or laser expire timer for target reset
        if (m_uiBurstEndTimer)
        {
            if (m_uiBurstEndTimer <= uiDiff)
                m_uiBurstEndTimer = 0;
            else
                m_uiBurstEndTimer -= uiDiff;
        }

        if (m_uiLaserEndTimer)
        {
            if (m_uiLaserEndTimer <= uiDiff)
                m_uiLaserEndTimer = 0;
            else
                m_uiLaserEndTimer -= uiDiff;

            // no other abilities during Laser
            return;
        }

        if (m_uiPhase == PHASE_VX001)
        {
            if (m_uiRapidBurstTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RAPID_BURST_SUMMON, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_RAPID_BURST_SUMMON) == CAST_OK)
                        m_uiRapidBurstTimer = 4000;
                }
            }
            else
                m_uiRapidBurstTimer -= uiDiff;

            if (m_uiFlameSuppressTimer)
            {
                if (m_uiFlameSuppressTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FLAME_SUPPRESSANT_CLOSE) == CAST_OK)
                        m_uiFlameSuppressTimer = 10000;
                }
                else
                    m_uiFlameSuppressTimer -= uiDiff;
            }
        }
        else if (m_uiPhase == PHASE_FULL_ROBOT)
        {
            if (m_uiHandPulseTimer < uiDiff)
            {
                CanCastResult uiResult;
                if (urand(0, 1))
                    uiResult = DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HAND_PULSE_LEFT : SPELL_HAND_PULSE_LEFT_H);
                else
                    uiResult = DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HAND_PULSE_RIGHT : SPELL_HAND_PULSE_RIGHT_H);

                if (uiResult == CAST_OK)
                    m_uiHandPulseTimer = urand(1000, 2000);
            }
            else
                m_uiHandPulseTimer -= uiDiff;
        }

        if (m_uiRocketStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ROCKET_STRIKE) == CAST_OK)
                m_uiRocketStrikeTimer = 20000;
        }
        else
            m_uiRocketStrikeTimer -= uiDiff;

        if (m_uLaserBarrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINNING_UP) == CAST_OK)
            {
                m_uiLaserEndTimer = 14000;
                m_uLaserBarrageTimer = 40000;
            }
        }
        else
            m_uLaserBarrageTimer -= uiDiff;

        if (m_uiFrostBombTimer)
        {
            if (m_uiFrostBombTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FROST_BOMB_SUMMON) == CAST_OK)
                    m_uiFrostBombTimer = 30000;
            }
            else
                m_uiFrostBombTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_vx001(Creature* pCreature)
{
    return new boss_vx001AI(pCreature);
}

/*######
## boss_aerial_unit
######*/

struct boss_aerial_unitAI : public ScriptedAI
{
    boss_aerial_unitAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetLevitate(true);
        m_uiPhase = PHASE_TRANSITION;
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;

    uint32 m_uiCombatMoveTimer;
    uint32 m_uiPlasmaBallTimer;
    uint32 m_uiBombBotTimer;
    uint32 m_uiAssaultBotTimer;
    uint32 m_uiScrapBotTimer;
    uint32 m_uiFireBotTimer;
    uint32 m_uiMagneticTimer;

    void Reset() override
    {
        m_uiCombatMoveTimer = 2000;
        m_uiPlasmaBallTimer = 2000;
        m_uiAssaultBotTimer = 5000;
        m_uiBombBotTimer    = 15000;
        m_uiScrapBotTimer   = 10000;
        m_uiMagneticTimer   = 0;
        m_uiFireBotTimer    = 0;

        SetCombatMovement(false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_uiPhase = PHASE_AERIAL_UNIT;
        m_creature->SetWalk(false);

        // init hard mode spells
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_FIRE_BOT_TRIGGER);
            m_uiFireBotTimer = 45000;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) override
    {
        if (pDoneBy->GetEntry() == NPC_MIMIRON && m_uiPhase == PHASE_DAMAGED)
            return;

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            if (m_uiPhase == PHASE_AERIAL_UNIT)
            {
                // start transition phase
                if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                    SendAIEvent(AI_EVENT_CUSTOM_D, m_creature, pMimiron);

                // shut down the aerial unit and prepare for the final phase
                DoCastSpellIfCan(m_creature, SPELL_CLEAR_DEBUFFS, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_HALF_HEAL, CAST_TRIGGERED);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(0, afAerialMovePos[0], afAerialMovePos[1], afAerialMovePos[2]);
                m_uiPhase = PHASE_TRANSITION;
            }
            else if (m_uiPhase == PHASE_FULL_ROBOT)
            {
                // start self repair
                if (DoCastSpellIfCan(m_creature, SPELL_SELF_REPAIR, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                {
                    // inform Mimiron about the damaged state
                    if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
                        SendAIEvent(AI_EVENT_CUSTOM_E, m_creature, pMimiron);

                    m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                    m_uiPhase = PHASE_DAMAGED;
                }
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell) override
    {
        // self repair succesfull; resume fight
        if (pSpell->Id == SPELL_SELF_REPAIR)
        {
            m_uiPhase = PHASE_FULL_ROBOT;
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        }
        else if (pSpell->Id == SPELL_MAGNETIC_CORE_PULL && pCaster->GetEntry() == NPC_MAGNETIC_CORE)
        {
            DoCastSpellIfCan(m_creature, SPELL_MAGNETIC_CORE_VISUAL, CAST_INTERRUPT_PREVIOUS);
            m_uiMagneticTimer = 20000;

            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(0, pCaster->GetPositionX(), pCaster->GetPositionY(), pCaster->GetPositionZ());
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, FAIL);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        if (Creature* pMimiron = m_pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
        {
            if (m_uiPhase == PHASE_FULL_ROBOT)
                DoScriptText(urand(0, 1) ? SAY_ROBOT_SLAY_1 : SAY_ROBOT_SLAY_2, pMimiron);
            else
                DoScriptText(urand(0, 1) ? SAY_HEAD_SLAY_1 : SAY_HEAD_SLAY_2, pMimiron);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 /*uiMiscValue*/) override
    {
        // switch to full robot abilities
        if (eventType == AI_EVENT_CUSTOM_A)
            m_uiPhase = PHASE_FULL_ROBOT;
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == DONE)
            pSummoned->CastSpell(pSummoned, SPELL_EMERGENCY_MODE, true);

        pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMagneticTimer)
        {
            if (m_uiMagneticTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), afAerialMovePos[2]);

                m_creature->RemoveAurasDueToSpell(SPELL_MAGNETIC_CORE_VISUAL);
                m_uiMagneticTimer = 0;
            }
            else
                m_uiMagneticTimer -= uiDiff;

            // no other abilities during the magnetic pull
            return;
        }

        // no combat during transition or when damaged
        if (m_uiPhase == PHASE_TRANSITION || m_uiPhase == PHASE_DAMAGED)
            return;

        // aerial phase spells
        if (m_uiPhase == PHASE_AERIAL_UNIT)
        {
            // move to a closer point to target
            if (m_uiCombatMoveTimer < uiDiff)
            {
                if (m_creature->GetDistance(m_creature->getVictim()) > 30.0f)
                {
                    float fX, fY, fZ;
                    m_creature->getVictim()->GetContactPoint(m_creature, fX, fY, fZ, 3 * ATTACK_DISTANCE);

                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(0, fX, fY, m_creature->GetPositionZ());
                }
                m_uiCombatMoveTimer = 2000;
            }
            else
                m_uiCombatMoveTimer -= uiDiff;

            if (m_uiPlasmaBallTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PLASMA_BALL_FLY : SPELL_PLASMA_BALL_FLY_H) == CAST_OK)
                    m_uiPlasmaBallTimer = urand(2000, 3000);
            }
            else
                m_uiPlasmaBallTimer -= uiDiff;

            if (m_uiAssaultBotTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_ASSAULT_BOT_TRIGGER) == CAST_OK)
                    m_uiAssaultBotTimer = 30000;
            }
            else
                m_uiAssaultBotTimer -= uiDiff;

            if (m_uiBombBotTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BOMB_BOT_SUMMON) == CAST_OK)
                    m_uiBombBotTimer = 15000;
            }
            else
                m_uiBombBotTimer -= uiDiff;

            if (m_uiScrapBotTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCRAP_BOT_TRIGGER) == CAST_OK)
                    m_uiScrapBotTimer = 10000;
            }
            else
                m_uiScrapBotTimer -= uiDiff;

            if (m_uiFireBotTimer)
            {
                if (m_uiFireBotTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_FIRE_BOT_TRIGGER) == CAST_OK)
                        m_uiFireBotTimer = 45000;
                }
                else
                    m_uiFireBotTimer -= uiDiff;
            }
        }
        // full robot abilities
        else if (m_uiPhase == PHASE_FULL_ROBOT)
        {
            if (m_uiPlasmaBallTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PLASMA_BALL : SPELL_PLASMA_BALL_H) == CAST_OK)
                    m_uiPlasmaBallTimer = 2000;
            }
            else
                m_uiPlasmaBallTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_aerial_unit(Creature* pCreature)
{
    return new boss_aerial_unitAI(pCreature);
}

/*######
## npc_proximity_mine
######*/

struct npc_proximity_mineAI : public Scripted_NoMovementAI
{
    npc_proximity_mineAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiExplodeTimer;

    void Reset() override
    {
        m_uiExplodeTimer = 35000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiExplodeTimer)
        {
            if (m_uiExplodeTimer <= uiDiff)
            {
                // just despawn if already exploded
                if (!m_creature->HasAura(SPELL_PROXIMITY_MINE))
                    m_creature->ForcedDespawn();
                else
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_EXPLOSION_H) == CAST_OK)
                    {
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_creature->RemoveAurasDueToSpell(SPELL_PROXIMITY_MINE);
                        m_creature->ForcedDespawn(2000);
                        m_uiExplodeTimer = 0;
                    }
                }
            }
            else
                m_uiExplodeTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_proximity_mine(Creature* pCreature)
{
    return new npc_proximity_mineAI(pCreature);
}

/*######
## npc_bot_trigger
######*/

struct npc_bot_triggerAI : public Scripted_NoMovementAI
{
    npc_bot_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 m_uiSummonTimer;
    uint32 m_uiSummonSpell;

    void Reset() override
    {
        m_uiSummonTimer = 0;
        m_uiSummonSpell = 0;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void ReceiveAIEvent(AIEventType eventType, Creature* /*pSender*/, Unit* /*pInvoker*/, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_uiSummonTimer = 6000;
            m_uiSummonSpell = uiMiscValue;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    if (Creature* pAerial = m_pInstance->GetSingleCreatureFromStorage(NPC_AERIAL_UNIT))
                    {
                        if (DoCastSpellIfCan(m_creature, m_uiSummonSpell, CAST_TRIGGERED, pAerial->GetObjectGuid()) == CAST_OK)
                            m_uiSummonTimer = 0;
                    }
                }

                // search for a nearby teleporter and disable the visual
                for (uint8 i = 0; i < countof(aMimironTeleporters); ++i)
                {
                    if (GameObject* pTeleporter = GetClosestGameObjectWithEntry(m_creature, aMimironTeleporters[i], 2.0f))
                    {
                        pTeleporter->ResetDoorOrButton();
                        break;
                    }
                }
            }
            else
                m_uiSummonTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_bot_trigger(Creature* pCreature)
{
    return new npc_bot_triggerAI(pCreature);
}

bool EffectDummyCreature_npc_bot_trigger(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget, ObjectGuid /*originalCasterGuid*/)
{
    // always check spellid and effectindex
    if ((uiSpellId == SPELL_SUMMON_ASSAULT_BOT_TRIGGER || uiSpellId == SPELL_SUMMON_SCRAP_BOT_TRIGGER || uiSpellId == SPELL_SUMMON_FIRE_BOT_TRIGGER) && uiEffIndex == EFFECT_INDEX_0)
    {
        uint32 uiVisualSpell = 0;
        uint32 uiSummonSpell = 0;

        switch (uiSpellId)
        {
            case SPELL_SUMMON_SCRAP_BOT_TRIGGER:
                uiVisualSpell = SPELL_SUMMON_ASSAULT_BOT_VISUAL;
                uiSummonSpell = SPELL_SUMMON_ASSAULT_BOT;
                break;
            case SPELL_SUMMON_ASSAULT_BOT_TRIGGER:
                uiVisualSpell = SPELL_SUMMON_SCRAP_BOT_VISUAL;
                uiSummonSpell = SPELL_SUMMON_SCRAP_BOT;
                break;
            case SPELL_SUMMON_FIRE_BOT_TRIGGER:
                uiVisualSpell = SPELL_SUMMON_FIRE_BOT_VISUAL;
                uiSummonSpell = SPELL_SUMMON_FIRE_BOT;
                break;
        }

        pCreatureTarget->CastSpell(pCreatureTarget, uiVisualSpell, true);
        pCreatureTarget->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pCaster, pCreatureTarget, uiSummonSpell);

        // search for a nearby teleporter and enable the visual
        for (uint8 i = 0; i < countof(aMimironTeleporters); ++i)
        {
            if (GameObject* pTeleporter = GetClosestGameObjectWithEntry(pCreatureTarget, aMimironTeleporters[i], 2.0f))
            {
                pTeleporter->UseDoorOrButton();
                break;
            }
        }

        // always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## npc_mimiron_flames
######*/

struct npc_mimiron_flamesAI : public Scripted_NoMovementAI
{
    npc_mimiron_flamesAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;
    uint32 m_uiSpreadTimer;

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_FLAMES, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_uiSpreadTimer = 4000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    // function to select the closest player to spread the fire
    Unit* SelectClosestSpreadTarget()
    {
        if (!m_pInstance)
            return NULL;

        Creature* pLeviathan = m_pInstance->GetSingleCreatureFromStorage(NPC_LEVIATHAN_MK);
        if (!pLeviathan)
            return NULL;

        std::list<Unit*> lTargets;
        ThreatList const& threatList = pLeviathan->getThreatManager().getThreatList();

        for (ThreatList::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
            {
                if (pTarget->GetTypeId() == TYPEID_PLAYER)
                    lTargets.push_back(pTarget);
            }
        }

        // return the closest target to the caster
        if (!lTargets.empty())
        {
            lTargets.sort(ObjectDistanceOrder(m_creature));
            return lTargets.front();
        }

        return NULL;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSpreadTimer < uiDiff)
        {
            if (Unit* pTarget = SelectClosestSpreadTarget())
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SUMMON_FLAMES_SPREAD) == CAST_OK)
                    m_uiSpreadTimer = 4000;
            }
        }
        else
            m_uiSpreadTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mimiron_flames(Creature* pCreature)
{
    return new npc_mimiron_flamesAI(pCreature);
}

/*######
## npc_frost_bomb
######*/

// TODO Move this 'script' to EventAI when combat can be proper prevented from core-side
struct npc_frost_bombAI : public Scripted_NoMovementAI
{
    npc_frost_bombAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    uint32 m_uiExplosionTimer;
    uint32 m_uiFireClearTimer;

    void Reset() override
    {
        m_uiExplosionTimer = 10000;
        m_uiFireClearTimer = 12000;
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiExplosionTimer)
        {
            if (m_uiExplosionTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_EXPLOSION_FROST) == CAST_OK)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_FROST_BOMB_VISUAL);
                    m_uiExplosionTimer = 0;
                }
            }
            else
                m_uiExplosionTimer -= uiDiff;
        }

        if (m_uiFireClearTimer)
        {
            if (m_uiFireClearTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_CLEAR_FIRES) == CAST_OK)
                {
                    m_creature->ForcedDespawn(2000);
                    m_uiFireClearTimer = 0;
                }
            }
            else
                m_uiFireClearTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_frost_bomb(Creature* pCreature)
{
    return new npc_frost_bombAI(pCreature);
}

/*######
## npc_rocket_strike
######*/

struct npc_rocket_strikeAI : public Scripted_NoMovementAI
{
    npc_rocket_strikeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ROCKET_STRIKE_DAMAGE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        m_creature->ForcedDespawn(7000);
    }

    void KilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetEntry() == NPC_ASSALT_BOT)
            DoCastSpellIfCan(m_creature, SPELL_NOT_FRIENDLY_FIRE, CAST_TRIGGERED);
    }

    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_rocket_strike(Creature* pCreature)
{
    return new npc_rocket_strikeAI(pCreature);
}

/*######
## boss_leviathan_mk2_turret
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct boss_leviathan_mk2_turretAI : public Scripted_NoMovementAI
{
    boss_leviathan_mk2_turretAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_boss_leviathan_mk2_turret(Creature* pCreature)
{
    return new boss_leviathan_mk2_turretAI(pCreature);
}

/*######
## npc_computer
######*/

// TODO Remove this 'script' when combat can be proper prevented from core-side
struct npc_computerAI : public Scripted_NoMovementAI
{
    npc_computerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }
    void UpdateAI(const uint32 /*uiDiff*/) override { }
};

CreatureAI* GetAI_npc_computer(Creature* pCreature)
{
    return new npc_computerAI(pCreature);
}

/*######
## go_big_red_button
######*/

bool GOUse_go_big_red_button(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_MIMIRON) == IN_PROGRESS || pInstance->GetData(TYPE_MIMIRON) == DONE)
        return true;

    // Inform Mimiron about the button being pressed
    if (Creature* pMimiron = pInstance->GetSingleCreatureFromStorage(NPC_MIMIRON))
        pMimiron->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pMimiron);

    // Set instance data and allow Mimiron script to continue the event
    pInstance->SetData(TYPE_MIMIRON_HARD, DONE);
    return false;
}

void AddSC_boss_mimiron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mimiron";
    pNewScript->GetAI = GetAI_boss_mimiron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_leviathan_mk2";
    pNewScript->GetAI = GetAI_boss_leviathan_mk2;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_vx001";
    pNewScript->GetAI = GetAI_boss_vx001;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_aerial_unit";
    pNewScript->GetAI = GetAI_boss_aerial_unit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_proximity_mine";
    pNewScript->GetAI = GetAI_npc_proximity_mine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bot_trigger";
    pNewScript->GetAI = GetAI_npc_bot_trigger;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_bot_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mimiron_flames";
    pNewScript->GetAI = GetAI_npc_mimiron_flames;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frost_bomb";
    pNewScript->GetAI = GetAI_npc_frost_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rocket_strike";
    pNewScript->GetAI = GetAI_npc_rocket_strike;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_leviathan_mk2_turret";
    pNewScript->GetAI = GetAI_boss_leviathan_mk2_turret;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_computer";
    pNewScript->GetAI = GetAI_npc_computer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_big_red_button";
    pNewScript->pGOUse = &GOUse_go_big_red_button;
    pNewScript->RegisterSelf();
}
