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
SDName: boss_kiljaeden
SD%Complete: 90
SDComment: Sinister Reflection needs AI support.
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_EMERGE                  = -1580069,
    SAY_SLAY_1                  = 25549,
    SAY_SLAY_2                  = 25550,
    SAY_REFLECTION_1            = -1580072,
    SAY_REFLECTION_2            = -1580073,
    SAY_DARKNESS_1              = -1580074,
    SAY_DARKNESS_2              = -1580075,
    SAY_DARKNESS_3              = -1580076,
    SAY_PHASE_3                 = -1580077,
    SAY_PHASE_4                 = -1580078,
    SAY_PHASE_5                 = -1580079,
    SAY_KALECGOS_INTRO          = -1580080,
    SAY_KALECGOS_AWAKE_1        = -1580081,
    SAY_ANVEENA_IMPRISONED      = -1580082,
    SAY_KALECGOS_AWAKE_2        = -1580083,
    SAY_ANVEENA_LOST            = -1580084,
    SAY_KALECGOS_AWAKE_4        = -1580085,
    SAY_ANVEENA_AWAKE           = -1580086,
    SAY_KALECGOS_AWAKE_5        = -1580087,
    SAY_ANVEENA_SACRIFICE       = -1580088,
    SAY_KALECGOS_GOODBYE        = -1580089,
    SAY_KALECGOS_ENCOURAGE      = -1580090,
    SAY_KALECGOS_ORB_1          = -1580091,
    SAY_KALECGOS_ORB_2          = -1580092,
    SAY_KALECGOS_ORB_3          = -1580093,
    SAY_KALECGOS_ORB_4          = -1580094,

    SAY_GENERIC_BERSERK         = -1000004,

    // outro
    SAY_OUTRO_1                 = -1580095,         // Velen
    SAY_OUTRO_2                 = -1580096,
    SAY_OUTRO_3                 = -1580097,
    SAY_OUTRO_4                 = -1580098,
    SAY_OUTRO_5                 = -1580099,         // Liadrin
    SAY_OUTRO_6                 = -1580100,         // Velen
    SAY_OUTRO_7                 = -1580101,         // Liadrin
    SAY_OUTRO_8                 = -1580102,         // Velen
    SAY_OUTRO_9                 = -1580103,
    SAY_OUTRO_10                = -1580104,         // Liadrin
    SAY_OUTRO_11                = -1580105,         // Velen
    SAY_OUTRO_12                = -1580106,

    // generic spells
    SPELL_BIRTH                 = 26586,            // Kiljaeden spawn animation
    SPELL_KNOCKBACK             = 47113,
    SPELL_SWAT                  = 46548,
    SPELL_ENRAGE                = 26662,

    // transition spells
    SPELL_DESTROY_DRAKES        = 46707,
    SPELL_SINISTER_REFLECTION   = 45892,
    SPELL_SINISTER_REFLECTION_SUMMON = 45891,
    SPELL_SHADOW_SPIKE          = 46680,
    SPELL_WING_SWEEP_1          = 46625,
    SPELL_WING_SWEEP_2          = 46626,
    SPELL_WING_SWEEP_3          = 46627,

    // phase 1
    SPELL_SOUL_FLAY             = 45442,
    SPELL_LEGION_LIGHTING       = 45664,
    SPELL_FIRE_BLOOM            = 45641,

    // phase 2
    SPELL_FLAME_DART_MAIN       = 45737,
    SPELL_FLAME_DART_1          = 45740,
    SPELL_FLAME_DART_2          = 45741,
    SPELL_DARKNESS_OF_SOULS     = 46605,

    // phase 3
    SPELL_ARMAGEDDON            = 45921,        // used from 50% hp - summons 25735 on target location

    // Npc spells
    SPELL_SHADOW_BOLT_AURA      = 45679,        // periodic aura on shield orbs
    // SPELL_RING_BLUE_FLAME       = 45825,        // cast by the orb targets when activated
    SPELL_ANVEENA_PRISON        = 46367,
    SPELL_SACRIFICE_ANVEENA     = 46474,
    SPELL_ARCANE_BOLT           = 45670,        // used by Kalec
    SPELL_SINISTER_REFL_CLASS   = 45893,        // increase the size of the clones
    SPELL_SINISTER_REFL_CLONE   = 45785,        // clone the player
    SPELL_VENGEANCE_BLUE_FLIGHT = 45839,        // possess the dragon
    SPELL_POSSESS_DRAKE_IMMUNE  = 45838,        // immunity while the player possesses the dragon
    SPELL_SHIELD_OF_THE_BLUE_2  = 47314,

    // Npcs
    NPC_SHIELD_ORB              = 25502,
    // NPC_SINISTER_REFLECTION     = 25708,
    NPC_ARMAGEDDON              = 25735,        // npc handled by eventAI
    // NPC_BLUE_ORB_TARGET         = 25640,        // dummy npc near gameobjects 187869, 188114, 188115, 188116

    // phases
    PHASE_INFERNO               = 1,
    PHASE_DARKNESS              = 2,
    PHASE_ARMAGEDDON            = 3,
    PHASE_SACRIFICE             = 4,
    PHASE_TRANSITION            = 5,

    // dummy members, used in the phase switch event
    EVENT_SWITCH_PHASE_2        = 6,
    EVENT_SWITCH_PHASE_3        = 7,
    EVENT_SWITCH_PHASE_4        = 8,
    EVENT_DRAGON_ORB            = 9,
    EVENT_LAST_PHASE_AGGRO      = 10,

    // outro
    SPELL_TELEPORT_VISUAL       = 12980,
    SPELL_KALEC_TELEPORT        = 46473,            // teleports and transforms Kalec in human form
    SPELL_ARCANE_PORTAL         = 42047,
    SPELL_CALL_ENTROPIUS        = 46818,
    SPELL_ENTROPIUS_BODY        = 46819,
    SPELL_BLAZE_TO_LIGHT        = 46821,
    SPELL_SUNWELL_IGNITION      = 46822,
    SPELL_PORTAL_FROM_SHATTRATH = 46801,            // Riftwalker after reached point 1, summon NPC_RIFTWALKER and cast on him this spell.

    NPC_INERT_PORTAL            = 26254,
    NPC_CORE_ENTROPIUS          = 26262,
    NPC_SOLDIER                 = 26259,            // summoned in 2 waves before Velen. Should move into 2 circle formations
    NPC_RIFTWALKER              = 26289,
    NPC_SHATTRATH_PORTAL        = 26251,

    POINT_SUMMON_SOLDIERS_LEFT  = 1,
    POINT_START_LEFT_MOVEMENT   = 2,
    POINT_SUMMON_SOLDIERS_RIGHT = 3,
    POINT_START_RIGHT_MOVEMENT  = 4,
    POINT_START_RIGHT_KNEEL     = 5,
    POINT_START_CORE_MOVEMENT   = 6,
    POINT_MOVE_LIADRIN          = 7,
    POINT_EVENT_EXIT            = 8,
    POINT_GROUPS_STOP_KNEEL     = 9,
    POINT_GROUPS_MOVE           = 10,
    POINT_VELEN_DESPAWN         = 11,
    POINT_GROUPS_DESPAWN        = 12,

    SOUND_1                     = 10824,
    SOUND_2                     = 9843,
};

// Encounter phase dialogue
static const DialogueEntry aPhaseDialogue[] =
{
    {PHASE_DARKNESS,            0,              2000},
    {SPELL_WING_SWEEP_1,        0,              1000},
    {EVENT_SWITCH_PHASE_2,      0,              17000},
    {SAY_KALECGOS_AWAKE_1,      NPC_KALECGOS,   6000},
    {SAY_ANVEENA_IMPRISONED,    NPC_ANVEENA,    5000},
    {SAY_PHASE_3,               NPC_KILJAEDEN,  6000},
    {SAY_KALECGOS_ORB_1,        NPC_KALECGOS,   0},         // phase 2 transition end
    {PHASE_ARMAGEDDON,          0,              2000},
    {SPELL_WING_SWEEP_2,        0,              1000},
    {EVENT_SWITCH_PHASE_3,      0,              15000},
    {SAY_KALECGOS_AWAKE_2,      NPC_KALECGOS,   6500},
    {SAY_ANVEENA_LOST,          NPC_ANVEENA,    7000},
    {SAY_PHASE_4,               NPC_KILJAEDEN,  6000},
    {EVENT_DRAGON_ORB,          0,              0},         // phase 3 transition end
    {PHASE_SACRIFICE,           0,              2000},
    {SPELL_WING_SWEEP_2,        0,              1000},
    {EVENT_SWITCH_PHASE_4,      0,              6000},
    {SAY_KALECGOS_AWAKE_4,      NPC_KALECGOS,   10000},
    {SAY_ANVEENA_AWAKE,         NPC_ANVEENA,    2000},
    {SAY_KALECGOS_AWAKE_5,      NPC_KALECGOS,   7500},
    {SAY_ANVEENA_SACRIFICE,     NPC_ANVEENA,    5000},
    {SAY_PHASE_5,               NPC_KILJAEDEN,  1000},
    {SPELL_SACRIFICE_ANVEENA,   0,              6000},
    {EVENT_LAST_PHASE_AGGRO,    0,              5500},
    {SAY_KALECGOS_ORB_4,        NPC_KALECGOS,   5000},
    {SAY_KALECGOS_ENCOURAGE,    NPC_KALECGOS,   0},         // phase 4 transition end
    {0, 0, 0},
};

// Epilogue dialogue
static const DialogueEntry aOutroDialogue[] =
{
    {NPC_KALECGOS,          0,              35000},
    {SAY_KALECGOS_GOODBYE,  NPC_KALECGOS,   15000},
    {NPC_RIFTWALKER,        0,              5500},
    {SPELL_PORTAL_FROM_SHATTRATH, 0,        500},
    {NPC_INERT_PORTAL,      0,              10000},
    {SOUND_1,               0,              2000},
    {SOUND_2,               0,              1500},
    {SPELL_ARCANE_PORTAL,   0,              15000},
    {POINT_SUMMON_SOLDIERS_LEFT, 0,         5000},
    {POINT_START_LEFT_MOVEMENT, 0,          6000},
    {POINT_SUMMON_SOLDIERS_RIGHT, 0,        5000},
    {POINT_START_RIGHT_MOVEMENT, 0,         11000},
    {POINT_START_RIGHT_KNEEL, 0,            1000},
    {NPC_VELEN,             0,              5500},
    {SAY_OUTRO_1,           NPC_VELEN,      500},
    {NPC_ENTROPIUS,         0,              24000},
    {SAY_OUTRO_2,           NPC_VELEN,      15000},
    {SAY_OUTRO_3,           NPC_VELEN,      13000},
    {SPELL_CALL_ENTROPIUS,  0,              10000},
    {SAY_OUTRO_4,           NPC_VELEN,      20000},
    {POINT_MOVE_LIADRIN,    0,              5000},
    {SAY_OUTRO_5,           NPC_LIADRIN,    10000},
    {SAY_OUTRO_6,           NPC_VELEN,      15000},
    {SAY_OUTRO_7,           NPC_LIADRIN,    3000},
    {SAY_OUTRO_8,           NPC_VELEN,      4000},
    {SPELL_BLAZE_TO_LIGHT,  0,              8000},
    {POINT_START_CORE_MOVEMENT, 0,          5000},
    {SAY_OUTRO_9,           NPC_VELEN,      14000},
    {SAY_OUTRO_10,          NPC_LIADRIN,    20000},
    {SAY_OUTRO_11,          NPC_VELEN,      8000},
    {SAY_OUTRO_12,          NPC_VELEN,      4500},
    {POINT_EVENT_EXIT,      0,              500},
    {POINT_GROUPS_STOP_KNEEL, 0,            2000},
    {POINT_GROUPS_MOVE,     0,              2000},
    {POINT_VELEN_DESPAWN,   0,              3000},
    {POINT_GROUPS_DESPAWN,  0,              0},
    {0, 0, 0},
};

/*######
## npc_kiljaeden_controller
######*/

struct npc_kiljaeden_controllerAI : public ScriptedAI, private DialogueHelper
{
    npc_kiljaeden_controllerAI(Creature* creature) : ScriptedAI(creature),
        DialogueHelper(aOutroDialogue), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        InitializeDialogueHelper(m_instance);
        Reset();
    }

    instance_sunwell_plateau* m_instance;

    ObjectGuid m_EntropiusGuid;
    ObjectGuid m_PortalGuid;
    ObjectGuid m_firstRiftwalkerGuid;
    ObjectGuid m_secondRiftwalkerGuid;

    void Reset() override
    {
        SetCombatMovement(false);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
            return;

        switch (iEntry)
        {
            case NPC_KALECGOS:
                if (Creature* kalec = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS))
                {
                    kalec->GetMotionMaster()->Clear();
                    kalec->GetMotionMaster()->MoveIdle();
                    kalec->CastSpell(nullptr, SPELL_KALEC_TELEPORT, TRIGGERED_OLD_TRIGGERED);
                    kalec->SetHover(false);
                    kalec->SetLevitate(false);
                }
                break;
            case NPC_RIFTWALKER:
                if (Creature* kalec = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS))
                    kalec->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (Creature* riftwalker = WorldObject::SpawnCreature(GUID_PREFIX + 365, m_creature->GetMap()))
                {
                    riftwalker->SetWalk(false);
                    riftwalker->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    riftwalker->GetMotionMaster()->MoveWaypoint();
                    m_firstRiftwalkerGuid = riftwalker->GetObjectGuid();
                }
                if (Creature* riftwalker = WorldObject::SpawnCreature(GUID_PREFIX + 366, m_creature->GetMap()))
                {
                    riftwalker->SetWalk(false);
                    riftwalker->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    riftwalker->GetMotionMaster()->MoveWaypoint();
                    m_secondRiftwalkerGuid = riftwalker->GetObjectGuid();
                }
                WorldObject::SpawnCreature(GUID_PREFIX + 367, m_creature->GetMap());
                WorldObject::SpawnCreature(GUID_PREFIX + 368, m_creature->GetMap());
                break;
            case SPELL_PORTAL_FROM_SHATTRATH:
                if (Creature* riftwalker = m_creature->GetMap()->GetCreature(GUID_PREFIX + 365))
                    if (Creature* dummy = m_creature->GetMap()->GetCreature(GUID_PREFIX + 367))
                        riftwalker->CastSpell(dummy, SPELL_PORTAL_FROM_SHATTRATH, TRIGGERED_NONE);
                if (Creature* riftwalker = m_creature->GetMap()->GetCreature(GUID_PREFIX + 366))
                    if (Creature* dummy = m_creature->GetMap()->GetCreature(GUID_PREFIX + 368))
                        riftwalker->CastSpell(dummy, SPELL_PORTAL_FROM_SHATTRATH, TRIGGERED_NONE);
                break;
            case NPC_INERT_PORTAL:
                WorldObject::SpawnCreature(GUID_PREFIX + 369, m_creature->GetMap());
                break;
            case SOUND_1:
                if (Creature* portalDummy = m_creature->GetMap()->GetCreature(GUID_PREFIX + 367))
                    portalDummy->ForcedDespawn();
                if (Creature* portalDummy = m_creature->GetMap()->GetCreature(GUID_PREFIX + 368))
                    portalDummy->ForcedDespawn();
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 369))
                    portal->PlayDirectSound(SOUND_1);
                break;
            case SOUND_2:
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 369))
                    portal->PlayDirectSound(SOUND_2);
                break;
            case SPELL_ARCANE_PORTAL:
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 369))
                {
                    portal->PlayDirectSound(SOUND_1);
                    portal->CastSpell(nullptr, SPELL_ARCANE_PORTAL, TRIGGERED_NONE);
                }
                break;
            case POINT_SUMMON_SOLDIERS_LEFT:
                for (uint32 i = 370u; i <= 379; ++i)
                    if (Creature* guard = WorldObject::SpawnCreature(GUID_PREFIX + i, m_creature->GetMap()))
                        guard->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                break;
            case POINT_START_LEFT_MOVEMENT:
            {
                float angle = 0.f;
                if (Creature* leader = m_creature->GetMap()->GetCreature(GUID_PREFIX + 370))
                {
                    for (uint32 i = 371u; i <= 379; ++i, angle += (M_PI_F * 2) / 9)
                        if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                            guard->GetMotionMaster()->MoveFollow(leader, 5.f, angle);
                    leader->GetMotionMaster()->MovePoint(1, Position(1718.26f, 608.50726f, 28.050209f, 1.431169986724853515f));
                }
                break;
            }
            case POINT_SUMMON_SOLDIERS_RIGHT:
                // left kneel right spawn
                for (uint32 i = 370u; i <= 379; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->SetStandState(UNIT_STAND_STATE_KNEEL);

                for (uint32 i = 380u; i <= 389; ++i)
                    if (Creature* guard = WorldObject::SpawnCreature(GUID_PREFIX + i, m_creature->GetMap()))
                        guard->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                break;
            case POINT_START_RIGHT_MOVEMENT:
            {
                float angle = 0.f;
                if (Creature* leader = m_creature->GetMap()->GetCreature(GUID_PREFIX + 380))
                {
                    for (uint32 i = 381u; i <= 389; ++i, angle += (M_PI_F * 2) / 9)
                        if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                            guard->GetMotionMaster()->MoveFollow(leader, 5.f, angle);
                    leader->GetMotionMaster()->MovePoint(1, Position(1680.1309f, 649.1477f, 28.050209f, 0.331612557172775268f));
                }
                break;
            }
            case POINT_START_RIGHT_KNEEL:
                for (uint32 i = 380u; i <= 389; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case NPC_VELEN:
                if (Creature* velen = WorldObject::SpawnCreature(GUID_PREFIX + 390, m_creature->GetMap()))
                {
                    velen->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                    velen->GetMotionMaster()->MoveWaypoint(1);
                }
                if (Creature* liadrin = WorldObject::SpawnCreature(GUID_PREFIX + 391, m_creature->GetMap()))
                    liadrin->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_ENTROPIUS:
                if (Creature* entropius = WorldObject::SpawnCreature(GUID_PREFIX + 392, m_creature->GetMap()))
                {
                    entropius->SetHover(true);
                    entropius->SetLevitate(true);
                    entropius->CastSpell(nullptr, SPELL_ENTROPIUS_BODY, TRIGGERED_OLD_TRIGGERED);
                    m_EntropiusGuid = entropius->GetObjectGuid();
                }
                break;
            case SPELL_CALL_ENTROPIUS:
                if (Creature* entropius = m_creature->GetMap()->GetCreature(m_EntropiusGuid))
                {
                    if (Creature* velen = m_instance->GetSingleCreatureFromStorage(NPC_VELEN))
                    {
                        velen->CastSpell(nullptr, SPELL_CALL_ENTROPIUS, TRIGGERED_NONE);
                    }
                    entropius->SetWalk(false);
                    entropius->GetMotionMaster()->MovePath(1);
                }
                break;
            case POINT_MOVE_LIADRIN:
                if (Creature* liadrin = m_instance->GetSingleCreatureFromStorage(NPC_LIADRIN))
                    liadrin->GetMotionMaster()->MovePoint(0, 1711.7444f, 636.9298f, 27.313908f);
                break;
            case SAY_OUTRO_5:
                if (Creature* liadrin = m_instance->GetSingleCreatureFromStorage(NPC_LIADRIN))
                    if (Creature* entropius = m_creature->GetMap()->GetCreature(m_EntropiusGuid))
                        liadrin->SetFacingToObject(entropius);
                break;
            case SPELL_BLAZE_TO_LIGHT:
                if (Creature* entropius = m_creature->GetMap()->GetCreature(m_EntropiusGuid))
                {
                    entropius->CastSpell(nullptr, SPELL_BLAZE_TO_LIGHT, TRIGGERED_OLD_TRIGGERED);
                    entropius->RemoveAurasDueToSpell(SPELL_ENTROPIUS_BODY);
                }
                break;
            case POINT_START_CORE_MOVEMENT:
                if (Creature* entropius = m_creature->GetMap()->GetCreature(m_EntropiusGuid))
                {
                    entropius->SetWalk(true);
                    entropius->GetMotionMaster()->MovePath(2);
                }
                break;
            case SAY_OUTRO_11:
                if (Creature* liadrin = m_instance->GetSingleCreatureFromStorage(NPC_LIADRIN))
                    liadrin->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case POINT_EVENT_EXIT:
                // Set point id = 1 for the despawn event
                if (Creature* velen = m_instance->GetSingleCreatureFromStorage(NPC_VELEN))
                    velen->GetMotionMaster()->MovePoint(1, 1718.651f, 645.6887f, 28.05021f);
                break;
            case POINT_GROUPS_STOP_KNEEL:
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 365))
                    portal->ForcedDespawn();
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 366))
                    portal->ForcedDespawn();

                for (uint32 i = 370u; i <= 379; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->SetStandState(UNIT_STAND_STATE_STAND);

                for (uint32 i = 380u; i <= 389; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case POINT_GROUPS_MOVE:
                if (Creature* leader = m_creature->GetMap()->GetCreature(GUID_PREFIX + 370))
                    leader->GetMotionMaster()->MovePoint(1, Position(1723.8624f, 642.50574f, 28.050207f, 0.f));
                if (Creature* leader = m_creature->GetMap()->GetCreature(GUID_PREFIX + 380))
                    leader->GetMotionMaster()->MovePoint(1, Position(1712.3164f, 654.0204f, 28.050207f, 0.f));
                break;
            case POINT_VELEN_DESPAWN:
                if (Creature* velen = m_instance->GetSingleCreatureFromStorage(NPC_VELEN))
                    velen->ForcedDespawn();
                if (Creature* portal = m_creature->GetMap()->GetCreature(GUID_PREFIX + 369))
                    portal->ForcedDespawn(30000);
                break;
            case POINT_GROUPS_DESPAWN:
                for (uint32 i = 370u; i <= 379; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->ForcedDespawn();

                for (uint32 i = 380u; i <= 389; ++i)
                    if (Creature* guard = m_creature->GetMap()->GetCreature(GUID_PREFIX + i))
                        guard->ForcedDespawn();
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        // Start outro dialogue when Kil'jaeden is killed
        if (summoned->GetEntry() == NPC_KILJAEDEN)
            StartNextDialogueText(NPC_KALECGOS);
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        if (pointId == 1)
        {
            if (summoned->GetEntry() == NPC_VELEN)
            {
                // Cast teleport and despawn Velen, the portal and Kalec; Liadrin will despawn on timer
                summoned->CastSpell(nullptr, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
                summoned->ForcedDespawn(1000);

                // Note: portal should despawn only after all the soldiers have reached this point and "teleported" outside

            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);
    }
};

/*######
## boss_kiljaeden
######*/

enum KiljaedenActions
{
    KILJAEDEN_ENRAGE,
    KILJAEDEN_85,
    KILJAEDEN_55,
    KILJAEDEN_25,
    KILJAEDEN_ARMAGEDDON,
    KILJAEDEN_DARKNESS_OF_SOULS,
    KILJAEDEN_FLAME_DART,
    KILJAEDEN_FIRE_BLOOM,
    KILJAEDEN_LEGION_LIGHTNING,
    KILJAEDEN_SHIELD_ORB,
    KILJAEDEN_SOUL_FLAY,
    KILJAEDEN_ACTION_MAX,
    KILJAEDEN_ATTACK_DELAY,
    KILJAEDEN_KALEC_YELL_DELAY,
};

struct boss_kiljaedenAI : public CombatAI, private DialogueHelper
{
    boss_kiljaedenAI(Creature* creature) : CombatAI(creature, KILJAEDEN_ACTION_MAX),
        DialogueHelper(aPhaseDialogue), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData())), m_uiMaxShieldOrbs(3), m_freeShieldOrbs(5)
    {
        InitializeDialogueHelper(m_instance);
        AddCombatAction(KILJAEDEN_ENRAGE, uint32(15 * MINUTE * IN_MILLISECONDS));
        AddTimerlessCombatAction(KILJAEDEN_85, true);
        AddTimerlessCombatAction(KILJAEDEN_55, true);
        AddTimerlessCombatAction(KILJAEDEN_25, true);
        AddCombatAction(KILJAEDEN_ARMAGEDDON, true);
        AddCombatAction(KILJAEDEN_DARKNESS_OF_SOULS, true);
        AddCombatAction(KILJAEDEN_FLAME_DART, true);
        AddCombatAction(KILJAEDEN_LEGION_LIGHTNING, true);
        AddCombatAction(KILJAEDEN_FIRE_BLOOM, true);
        AddCombatAction(KILJAEDEN_SHIELD_ORB, true);
        AddCombatAction(KILJAEDEN_SOUL_FLAY, true);
        AddCustomAction(KILJAEDEN_ATTACK_DELAY, 11000u, [&]() { HandleAttackDelay(); });
        AddCustomAction(KILJAEDEN_KALEC_YELL_DELAY, true, [&]() { HandleKalecYell(); });
        AddOnKillText(SAY_SLAY_1, SAY_SLAY_2);
        AddMainSpell(SPELL_SOUL_FLAY);
        SetRangedMode(true, 100.f, TYPE_FULL_CASTER);
        SetAIImmobilizedState(true);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float /*z*/)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr;
            });
        }
    }

    instance_sunwell_plateau* m_instance;

    uint8 m_uiPhase;
    uint8 m_uiMaxShieldOrbs;
    uint8 m_uiShieldOrbCount;

    GuidVector m_freeShieldOrbs;
    std::vector<uint32> m_randomOrbs = { GO_ORB_BLUE_FLIGHT_1, GO_ORB_BLUE_FLIGHT_2, GO_ORB_BLUE_FLIGHT_3, GO_ORB_BLUE_FLIGHT_4 };

    void Reset() override
    {        
        CombatAI::Reset();
        SetCombatMovement(false);

        DoCastSpellIfCan(nullptr, SPELL_KNOCKBACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        DoCastSpellIfCan(nullptr, SPELL_SWAT, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_uiPhase = PHASE_INFERNO;
        m_uiMaxShieldOrbs = 1;
        m_uiShieldOrbCount = 0;

        SetReactState(REACT_PASSIVE);
        SetMeleeEnabled(false);

        DespawnGuids(m_freeShieldOrbs);
        m_freeShieldOrbs.resize(5);
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_BIRTH);
        m_creature->SetInCombatWithZone();
    }

    void HandleAttackDelay()
    {
        SetReactState(REACT_AGGRESSIVE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoScriptText(SAY_EMERGE, m_creature);
        SetMeleeEnabled(true);
        AttackClosestEnemy();
        ResetCombatAction(KILJAEDEN_SOUL_FLAY, 1000);
        ResetCombatAction(KILJAEDEN_SHIELD_ORB, 10000);
        ResetCombatAction(KILJAEDEN_FIRE_BLOOM, urand(6000, 10000));
        ResetCombatAction(KILJAEDEN_LEGION_LIGHTNING, urand(2000, 5000));
    }

    void EnterEvadeMode() override
    {
        DespawnGuids(m_freeShieldOrbs);

        if (m_instance)
            m_instance->SetData(TYPE_KILJAEDEN, FAIL);

        DoCastSpellIfCan(nullptr, SPELL_DESTROY_DRAKES, CAST_TRIGGERED);

        // Despawn on wipe
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KILJAEDEN, DONE);

        DespawnGuids(m_freeShieldOrbs);
    }

    uint32 GetFirstFreeShieldOrbIndex()
    {
        uint32 i = 0;
        for (; i < m_freeShieldOrbs.size(); ++i)
            if (m_freeShieldOrbs[i].IsEmpty())
                break;
        return i;
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_KALECGOS)
        {
            ResetTimer(KILJAEDEN_KALEC_YELL_DELAY, 35000);
            summoned->CastSpell(nullptr, SPELL_ARCANE_BOLT, TRIGGERED_OLD_TRIGGERED);
            summoned->GetMotionMaster()->MovePath(1, PATH_NO_PATH, FORCED_MOVEMENT_NONE, true, 0.f, true);
        }
        else if (summoned->GetEntry() == NPC_SHIELD_ORB)
        {
            summoned->CastSpell(nullptr, SPELL_SHADOW_BOLT_AURA, TRIGGERED_OLD_TRIGGERED);
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->AI()->SetCombatMovement(false);
            summoned->SetInCombatWithZone();

            // Start the movement of the shadow orb - use db paths

            uint32 i = GetFirstFreeShieldOrbIndex();

            // Move to new position
            summoned->GetMotionMaster()->Clear(false, true);
            summoned->GetMotionMaster()->MovePath(i + 1, PATH_NO_PATH, FORCED_MOVEMENT_NONE, true, 0.f, true);
            m_freeShieldOrbs[i] = summoned->GetObjectGuid();
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SHIELD_ORB)
        {
            --m_uiShieldOrbCount;
            for (uint32 i = 0; i < m_freeShieldOrbs.size(); ++i)
                if (m_freeShieldOrbs[i] == summoned->GetObjectGuid())
                    m_freeShieldOrbs[i] = ObjectGuid();
        }
    }

    void GetAIInformation(ChatHandler& reader) override
    {
        reader.PSendSysMessage("Kil'jaeden is currently in phase %u", uint32(m_uiPhase));
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
            return;

        switch (iEntry)
        {
            case PHASE_DARKNESS:
            case PHASE_ARMAGEDDON:
            case PHASE_SACRIFICE:
                if (DoCastSpellIfCan(nullptr, SPELL_SINISTER_REFLECTION) == CAST_OK)
                {
                    DoScriptText(irand(0, 1) ? SAY_REFLECTION_1 : SAY_REFLECTION_2, m_creature);

                    if (iEntry == PHASE_DARKNESS)
                        DoCastSpellIfCan(nullptr, SPELL_WING_SWEEP_2);
                    else
                        DoCastSpellIfCan(nullptr, SPELL_WING_SWEEP_1);

                    // In the 2nd and 3rd transition kill all drakes
                    if (iEntry == PHASE_ARMAGEDDON || iEntry == PHASE_SACRIFICE)
                        DoCastSpellIfCan(nullptr, SPELL_DESTROY_DRAKES, CAST_TRIGGERED);

                    m_uiPhase = PHASE_TRANSITION;
                }
                break;
            case SPELL_WING_SWEEP_1:
                DoCastSpellIfCan(nullptr, SPELL_WING_SWEEP_1);
                break;
            case SPELL_WING_SWEEP_2:
                DoCastSpellIfCan(nullptr, SPELL_WING_SWEEP_2);
                break;
            case EVENT_SWITCH_PHASE_2:
            case EVENT_SWITCH_PHASE_3:
            case EVENT_SWITCH_PHASE_4:
                DoCastSpellIfCan(nullptr, SPELL_SHADOW_SPIKE);
                break;
            case EVENT_DRAGON_ORB:
                // Activate blue orbs
                if (Creature* kalec = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS))
                    DoScriptText(irand(0, 1) ? SAY_KALECGOS_ORB_2 : SAY_KALECGOS_ORB_3, kalec);
                DoActivateDragonOrb();
                break;
            case SAY_KALECGOS_ORB_1:
                DoActivateDragonOrb();
                break;
            case SAY_KALECGOS_ORB_4:
                DoActivateDragonOrb();
                DoActivateDragonOrb();
                DoActivateDragonOrb();
                DoActivateDragonOrb();
                break;
            case SAY_PHASE_3:
                // Set next phase and increase the max shield orbs
                m_uiPhase = PHASE_DARKNESS;
                SetCombatScriptStatus(false);
                SetMeleeEnabled(true);
                SetEquipmentSlots(false, 35510);
                ResetCombatAction(KILJAEDEN_DARKNESS_OF_SOULS, 45000);
                ResetCombatAction(KILJAEDEN_FIRE_BLOOM, urand(20000, 22000));
                ResetCombatAction(KILJAEDEN_SHIELD_ORB, 10000);
                ResetCombatAction(KILJAEDEN_LEGION_LIGHTNING, urand(10000, 14000));
                ResetCombatAction(KILJAEDEN_FLAME_DART, 20000);
                ++m_uiMaxShieldOrbs;
                break;
            case SAY_PHASE_4:
                // Set next phase and increase the max shield orbs
                m_uiPhase = PHASE_ARMAGEDDON;
                ++m_uiMaxShieldOrbs;
                SetCombatScriptStatus(false);
                SetMeleeEnabled(true);
                SetEquipmentSlots(false, 35510);
                ResetCombatAction(KILJAEDEN_DARKNESS_OF_SOULS, 45000);
                ResetCombatAction(KILJAEDEN_ARMAGEDDON, 21000);
                ResetCombatAction(KILJAEDEN_FIRE_BLOOM, urand(20000, 22000));
                ResetCombatAction(KILJAEDEN_SHIELD_ORB, 10000);
                ResetCombatAction(KILJAEDEN_LEGION_LIGHTNING, urand(10000, 14000));
                ResetCombatAction(KILJAEDEN_FLAME_DART, 20000);
                break;
            case SAY_PHASE_5:
                DoCastSpellIfCan(nullptr, SPELL_WING_SWEEP_3);
                break;
            case SPELL_SACRIFICE_ANVEENA:
                // Set next phase and sacrifice Anveena
                if (Creature* anveena = m_instance->GetSingleCreatureFromStorage(NPC_ANVEENA))
                {
                    anveena->RemoveAurasDueToSpell(SPELL_ANVEENA_PRISON);
                    anveena->CastSpell(nullptr, SPELL_SACRIFICE_ANVEENA, TRIGGERED_OLD_TRIGGERED);
                    anveena->ForcedDespawn(3000);
                }
                break;
            case EVENT_LAST_PHASE_AGGRO:
                m_uiPhase = PHASE_SACRIFICE;
                SetCombatScriptStatus(false);
                SetMeleeEnabled(true);
                ResetCombatAction(KILJAEDEN_DARKNESS_OF_SOULS, 17000);
                ResetCombatAction(KILJAEDEN_FIRE_BLOOM, urand(6000, 22000));
                DisableCombatAction(KILJAEDEN_SHIELD_ORB);
                ResetCombatAction(KILJAEDEN_LEGION_LIGHTNING, urand(10000, 14000));
                ResetCombatAction(KILJAEDEN_FLAME_DART, urand(15000, 20000));
                ResetCombatAction(KILJAEDEN_ARMAGEDDON, 6000);
                break;
        }
    }

    // Hack for simulating spell list
    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            ResetCombatAction(KILJAEDEN_FLAME_DART, urand(6000, 15000));
            ResetCombatAction(KILJAEDEN_LEGION_LIGHTNING, urand(5000, 10000));
            ResetCombatAction(KILJAEDEN_FIRE_BLOOM, urand(6000, 15000));
        }
    }

    // Wrapper to activate dragon orbs
    void DoActivateDragonOrb()
    {
        if (!m_instance)
            return;

        std::shuffle(m_randomOrbs.begin(), m_randomOrbs.end(), *GetRandomGenerator());

        uint32 selectedEntry = 0;
        for (uint32 entry : m_randomOrbs)
        {
            // Set the visual around the Orb
            if (GameObject* go = m_instance->GetSingleGameObjectFromStorage(entry))
            {
                if (Creature* target = GetClosestCreatureWithEntry(go, NPC_BLUE_ORB_TARGET, 5.0f))
                {
                    if (!target->GetDynObject(SPELL_RING_BLUE_FLAME))
                    {
                        selectedEntry = entry;
                        target->CastSpell(nullptr, SPELL_RING_BLUE_FLAME, TRIGGERED_NONE);
                        break;
                    }
                }
            }
        }

        if (selectedEntry) // Make the orb usable
            m_instance->DoToggleGameObjectFlags(selectedEntry, GO_FLAG_NO_INTERACT, false);
    }

    void HandleKalecYell()
    {
        if (Creature* kalec = m_instance->GetSingleCreatureFromStorage(NPC_KALECGOS))
            DoScriptText(SAY_KALECGOS_INTRO, kalec);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KILJAEDEN_ENRAGE:
                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case KILJAEDEN_85:
                // Go to next phase and start transition dialogue
                if (m_uiPhase == PHASE_INFERNO && m_creature->GetHealthPercent() < 85.0f)
                {
                    SetActionReadyStatus(action, false);
                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    StartNextDialogueText(PHASE_DARKNESS);
                }
                break;
            case KILJAEDEN_55:
                if (m_uiPhase == PHASE_DARKNESS && m_creature->GetHealthPercent() < 55.0f)
                {
                    SetActionReadyStatus(action, false);
                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    StartNextDialogueText(PHASE_ARMAGEDDON);
                }
                break;
            case KILJAEDEN_25:
                if (m_uiPhase == PHASE_ARMAGEDDON && m_creature->GetHealthPercent() < 25.0f)
                {
                    SetActionReadyStatus(action, false);
                    SetCombatScriptStatus(true);
                    SetMeleeEnabled(false);
                    m_creature->RemoveAurasDueToSpell(SPELL_ARMAGEDDON);
                    StartNextDialogueText(PHASE_SACRIFICE);
                }
                break;
            case KILJAEDEN_ARMAGEDDON:
                if (DoCastSpellIfCan(nullptr, SPELL_ARMAGEDDON) == CAST_OK)
                    ResetCombatAction(action, m_uiPhase == PHASE_SACRIFICE ? 20000 : 25000);
                break;
            case KILJAEDEN_DARKNESS_OF_SOULS:
                if (DoCastSpellIfCan(nullptr, SPELL_DARKNESS_OF_SOULS) == CAST_OK)
                    ResetCombatAction(action, m_uiPhase == PHASE_SACRIFICE ? 25000 : 45000);
                break;
            case KILJAEDEN_FLAME_DART:
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_DART_MAIN) == CAST_OK)
                    ResetCombatAction(action, urand(16000, 26000));
                break;
            case KILJAEDEN_LEGION_LIGHTNING:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_LEGION_LIGHTING) == CAST_OK)
                        ResetCombatAction(action, urand(14000, 18000));
                break;
            case KILJAEDEN_FIRE_BLOOM:
                if (DoCastSpellIfCan(nullptr, SPELL_FIRE_BLOOM) == CAST_OK)
                    ResetCombatAction(action, m_uiPhase == PHASE_SACRIFICE ? urand(20000, 28000) : urand(20000, 23000));
                break;
            case KILJAEDEN_SHIELD_ORB:
                if (!CanExecuteCombatAction())
                    return;

                // Only spawn a Shadow orb when necessary
                while (m_uiShieldOrbCount < m_uiMaxShieldOrbs)
                {
                    uint32 i = GetFirstFreeShieldOrbIndex();

                    static Position positions[] =
                    {
                        {1721.7328f, 616.782f,      45.059704f, 4.106034278869628906f},
                        {1694.8438f, 651.71295f,    28.45205f,  5.20198822021484375f},
                        {1682.4557f, 609.28815f,    28.315763f, 0.031090540811419487f},
                        {0.f,        0.f,           0.f,        0.f} // avoiding a crash here in future
                    };

                    m_creature->SummonCreature(NPC_SHIELD_ORB, positions[i].x, positions[i].y, positions[i].z, positions[i].o, TEMPSPAWN_CORPSE_DESPAWN, 0);
                    ++m_uiShieldOrbCount;
                    ResetCombatAction(action, 45000);
                }
                break;
            case KILJAEDEN_SOUL_FLAY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SOUL_FLAY) == CAST_OK)
                    ResetCombatAction(action, urand(0, 1) ? 3000 : 4000);
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        DialogueUpdate(diff);
        CombatAI::UpdateAI(diff);
    }
};

/*######
## npc_shield_orb
######*/

struct npc_shield_orbAI : public ScriptedAI
{
    npc_shield_orbAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    instance_sunwell_plateau* m_instance;

    void Reset() override { }
};

/*######
## npc_power_blue_flight
######*/

struct npc_power_blue_flightAI : public ScriptedAI
{
    npc_power_blue_flightAI(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        m_bHasPossessed = false;
        m_creature->SetCorpseDelay(1);
        Reset();
    }

    bool m_bHasPossessed;

    void Reset() override { }

    void UpdateAI(const uint32 /*diff*/) override
    {
        if (!m_bHasPossessed)
        {
            if (m_creature->IsTemporarySummon())
            {
                if (Player* player = m_creature->GetMap()->GetPlayer(m_creature->GetSpawnerGuid()))
                {
                    player->getHostileRefManager().deleteReferences();
                    player->CastSpell(nullptr, SPELL_POSSESS_DRAKE_IMMUNE, TRIGGERED_OLD_TRIGGERED);
                    player->CastSpell(nullptr, SPELL_VENGEANCE_BLUE_FLIGHT, TRIGGERED_OLD_TRIGGERED);
                }
            }

            m_bHasPossessed = true;
        }
    }
};

enum SinisterReflectionSpells
{
    // generic
    SPELL_DUAL_WEILD_PASSIVE = 42459,
    // hunter
    SPELL_WING_CLIP = 40652, // 47168 root used as prenerf
    // shaman
    SPELL_EARTH_SHOCK = 47071,
    // warrior
    SPELL_WHIRLWIND = 17207,
    // rogue
    SPELL_HEMORRHAGE = 45897,
    // druid
    SPELL_MOONFIRE = 47072,
    // warlock
    SPELL_SHADOW_BOLT = 47076,
    SPELL_CURSE_OF_AGONY = 46190,
    // priest
    SPELL_HOLY_SMITE = 47077,
    SPELL_RENEW = 47079,
    // paladin
    SPELL_HOLY_SHOCK = 38921,
    SPELL_HAMMER_OF_JUSTICE = 37369,
    // mage
    SPELL_FIREBALL = 47074,
};

enum SinisterReflectionActions
{
    SINISTER_WING_CLIP,
    SINISTER_EARTH_SHOCK,
    SINISTER_WHIRLWIND,
    SINISTER_HEMORRHAGE,
    SINISTER_MOONFIRE,
    SINISTER_SHADOW_BOLT,
    SINISTER_CURSE_OF_AGONY,
    SINISTER_HOLY_SMITE,
    SINISTER_RENEW,
    SINISTER_HOLY_SHOCK,
    SINISTER_HAMMER_OF_JUSTICE,
    SINISTER_FIREBALL,
    SINISTER_ACTION_MAX,
    SINISTER_ATTACK_DELAY,
};

struct npc_sinister_reflectionAI : public CombatAI
{
    npc_sinister_reflectionAI(Creature* creature) : CombatAI(creature, SINISTER_ACTION_MAX)
    {
        AddCustomAction(SINISTER_ATTACK_DELAY, 7000u, [&]() { HandleAttackDelay(); });
        SetReactState(REACT_DEFENSIVE);
    }

    uint8 m_class;

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_class = invoker->getClass();
            switch (m_class)
            {
                case CLASS_WARRIOR:
                    m_creature->SetPowerType(POWER_RAGE);
                    AddCombatAction(SINISTER_WHIRLWIND, urand(10000, 15000));
                    break;
                case CLASS_PALADIN:
                    AddCombatAction(SINISTER_HOLY_SHOCK, urand(10000, 20000));
                    AddCombatAction(SINISTER_HAMMER_OF_JUSTICE, urand(15000, 25000));
                    break;
                case CLASS_HUNTER:
                    AddCombatAction(SINISTER_WING_CLIP, 10000u);
                    break;
                case CLASS_ROGUE:
                    AddCombatAction(SINISTER_HEMORRHAGE, urand(10000, 15000));
                    break;
                case CLASS_PRIEST:
                    AddCombatAction(SINISTER_HOLY_SMITE, urand(5000, 10000));
                    AddCombatAction(SINISTER_RENEW, 10000u);
                    break;
                case CLASS_SHAMAN:
                    AddCombatAction(SINISTER_EARTH_SHOCK, urand(10000, 15000));
                    break;
                case CLASS_MAGE:
                    AddCombatAction(SINISTER_FIREBALL, urand(2000, 5000));
                    break;
                case CLASS_WARLOCK:
                    AddCombatAction(SINISTER_SHADOW_BOLT, urand(10000, 15000));
                    AddCombatAction(SINISTER_CURSE_OF_AGONY, 15000u);
                    break;
                case CLASS_DRUID:
                    AddCombatAction(SINISTER_MOONFIRE, 10000u);
                    break;
            }
            if (m_class == CLASS_WARRIOR || m_class == CLASS_PALADIN || m_class == CLASS_HUNTER || m_class == CLASS_ROGUE || m_class == CLASS_SHAMAN)
                if (m_creature->hasOffhandWeaponForAttack())
                    DoCastSpellIfCan(nullptr, SPELL_DUAL_WEILD_PASSIVE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        }
    }

    void HandleAttackDelay()
    {
        SetReactState(REACT_AGGRESSIVE);
        m_creature->SetInCombatWithZone();
        AttackStart(m_creature->GetSpawner());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SINISTER_WING_CLIP:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_WING_CLIP) == CAST_OK)
                    ResetCombatAction(action, 10000u);
                break;
            case SINISTER_EARTH_SHOCK:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_EARTH_SHOCK) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_WHIRLWIND:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_WHIRLWIND) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_HEMORRHAGE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HEMORRHAGE) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_MOONFIRE: // random
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MOONFIRE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
                    if (DoCastSpellIfCan(target, SPELL_MOONFIRE) == CAST_OK)
                        ResetCombatAction(action, 10000u);
                break;
            case SINISTER_SHADOW_BOLT:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SHADOW_BOLT) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_CURSE_OF_AGONY: // random
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_CURSE_OF_AGONY, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
                    if (DoCastSpellIfCan(target, SPELL_CURSE_OF_AGONY) == CAST_OK)
                        ResetCombatAction(action, 10000u);
                break;
            case SINISTER_HOLY_SMITE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HOLY_SMITE) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_RENEW:
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 50.f, true))
                {
                    if (DoCastSpellIfCan(target, SPELL_RENEW, CAST_AURA_NOT_PRESENT) == CAST_OK)
                    {
                        ResetCombatAction(action, 10000u);
                        return;
                    }
                }
                ResetCombatAction(action, urand(1000, 5000));
                break;
            case SINISTER_HOLY_SHOCK:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_HOLY_SHOCK) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_HAMMER_OF_JUSTICE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_HAMMER_OF_JUSTICE, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
                    if (DoCastSpellIfCan(target, SPELL_HAMMER_OF_JUSTICE) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 15000));
                break;
            case SINISTER_FIREBALL:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL) == CAST_OK)
                    ResetCombatAction(action, urand(3000, 6000));
                break;
        }
    }
};

struct npc_core_of_entropiusAI : public CombatAI
{
    npc_core_of_entropiusAI(Creature* creature) : CombatAI(creature, 0), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData())) {}

    instance_sunwell_plateau* m_instance;

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType == PATH_MOTION_TYPE && pointId == 2)
        {
            if (m_creature->GetMotionMaster()->GetPathId() == 1)
            {
                // Interrupt Velen's casting when entropius has reached the ground
                if (Creature* velen = m_instance->GetSingleCreatureFromStorage(NPC_VELEN))
                    velen->InterruptNonMeleeSpells(false);
            }
            else
            {
                // When the purified Muru reaches the ground the sunwell ignites and Muru despawns
                DoCastSpellIfCan(nullptr, SPELL_BLAZE_TO_LIGHT);
                DoCastSpellIfCan(nullptr, SPELL_SUNWELL_IGNITION);
            }
        }
    }
};

struct DarknessOfSouls : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_0 && !apply && aura->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            Unit* target = aura->GetTarget();
            uint32 spellId = aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);
            target->CastSpell(target, spellId, TRIGGERED_OLD_TRIGGERED);
            target->AddGCD(*sSpellTemplate.LookupEntry<SpellEntry>(spellId));
            if (target->AI())
                target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);

            switch (urand(0, 2))
            {
                case 0: DoScriptText(SAY_DARKNESS_1, target); break;
                case 1: DoScriptText(SAY_DARKNESS_2, target); break;
                case 2: DoScriptText(SAY_DARKNESS_3, target); break;
            }
        }
    }
};

struct SinisterReflection : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        // Summon 4 clones of the same player
        for (uint8 i = 0; i < 4; ++i)
            unitTarget->CastSpell(nullptr, SPELL_SINISTER_REFLECTION_SUMMON, TRIGGERED_OLD_TRIGGERED);

        unitTarget->CastSpell(nullptr, SPELL_SINISTER_REFL_CLONE, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SinisterReflectionClone : public SpellScript, public AuraScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->GetEntry() != NPC_SINISTER_REFLECTION)
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || effIdx == EFFECT_INDEX_0)
            return;

        target->CastSpell(spell->GetCaster(), spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            if (Unit* caster = aura->GetCaster())
                aura->GetTarget()->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, caster, aura->GetTarget());
    }
};

struct CopyOffhandWeapon : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!caster->IsCreature() || !target || !target->IsPlayer())
            return;

        // also includes shield
        if (Item* item = static_cast<Player*>(target)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
        {
            static_cast<Creature*>(caster)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, item->GetEntry());

            // Unclear what this spell should do
            target->CastSpell(caster, spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct Armageddon : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        if (aura->GetTarget()->IsCreature())
        {
            bool lastPhase = true;
            if (auto ai = dynamic_cast<boss_kiljaedenAI*>(aura->GetTarget()->AI()))
                if (ai->m_uiPhase != PHASE_SACRIFICE)
                    lastPhase = false;
            if (urand(0, lastPhase ? 1 : 2) == 0)
            {
                data.caster = aura->GetTarget();
                data.target = aura->GetTarget()->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, data.spellInfo, SELECT_FLAG_PLAYER);
            }
            else
                data.spellInfo = nullptr;
        }
    }
};

struct OpenPortalFromShattrath : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (apply)
            target->GetMotionMaster()->MovePath();
        else if (target->IsCreature())
            static_cast<Creature*>(target)->ForcedDespawn();
    }
};

struct ShadowSpike : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        // should target random players each tick
        data.caster = aura->GetTarget();
        data.target = aura->GetTarget()->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER);
    }
};

struct ArmageddonAnimation : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z += 72.f;
    }
};

struct PowerOfTheBlueFlight : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetUnitTarget()->CastSpell(nullptr, 45836, TRIGGERED_OLD_TRIGGERED);
        WorldObject* castingObject = spell->GetCastingObject();
        if (Creature* target = GetClosestCreatureWithEntry(castingObject, NPC_BLUE_ORB_TARGET, 5.0f))
            target->RemoveDynObject(SPELL_RING_BLUE_FLAME);
        if (castingObject->IsGameObject())
            static_cast<GameObject*>(castingObject)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    }
};

struct VengeanceOfTheBlueFlight : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_0)
        {
            if (Unit* caster = aura->GetCaster())
            {
                caster->RemoveAurasDueToSpell(SPELL_VENGEANCE_BLUE_FLIGHT);
                caster->RemoveAurasDueToSpell(SPELL_POSSESS_DRAKE_IMMUNE);
            }
            if (aura->GetTarget()->IsCreature())
                static_cast<Creature*>(aura->GetTarget())->ForcedDespawn();
        }
    }
};

struct BreathHaste : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, 49725, TRIGGERED_OLD_TRIGGERED);
    }
};

struct ShieldOfTheBlue : public SpellScript, public AuraScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, SPELL_SHIELD_OF_THE_BLUE_2, TRIGGERED_OLD_TRIGGERED);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
        {
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FLAME_DART_MAIN);
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FLAME_DART_1);
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FLAME_DART_2);
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FIRE_BLOOM);
        }
    }
};

struct FlameDart : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_1)
            spell->GetUnitTarget()->CastSpell(nullptr, 45746, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FlameDartMain : public FlameDart
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, SPELL_FLAME_DART_1, TRIGGERED_OLD_TRIGGERED);
        spell->GetCaster()->CastSpell(nullptr, SPELL_FLAME_DART_2, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_kiljaeden()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_kiljaeden";
    pNewScript->GetAI = &GetNewAIInstance<boss_kiljaedenAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_kiljaeden_controller";
    pNewScript->GetAI = &GetNewAIInstance<npc_kiljaeden_controllerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shield_orb";
    pNewScript->GetAI = &GetNewAIInstance<npc_shield_orbAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_power_blue_flight";
    pNewScript->GetAI = &GetNewAIInstance<npc_power_blue_flightAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sinister_reflection";
    pNewScript->GetAI = &GetNewAIInstance<npc_sinister_reflectionAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_core_of_entropius";
    pNewScript->GetAI = &GetNewAIInstance<npc_core_of_entropiusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<DarknessOfSouls>("spell_darkness_of_souls");
    RegisterSpellScript<SinisterReflection>("spell_sinister_reflection");
    RegisterSpellScript<SinisterReflectionClone>("spell_sinister_reflection_clone");
    RegisterSpellScript<CopyOffhandWeapon>("spell_copy_off_hand_weapon");
    RegisterSpellScript<Armageddon>("spell_armageddon");
    RegisterSpellScript<OpenPortalFromShattrath>("spell_open_portal_from_shattrath");
    RegisterSpellScript<ShadowSpike>("spell_shadow_spike");
    RegisterSpellScript<ArmageddonAnimation>("spell_armageddon_animation");
    RegisterSpellScript<PowerOfTheBlueFlight>("spell_power_of_the_blue_flight");
    RegisterSpellScript<VengeanceOfTheBlueFlight>("spell_vengeance_of_the_blue_flight");
    RegisterSpellScript<BreathHaste>("spell_breath_haste");
    RegisterSpellScript<ShieldOfTheBlue>("spell_shield_of_the_blue");
    RegisterSpellScript<FlameDart>("spell_flame_dart");
    RegisterSpellScript<FlameDartMain>("spell_flame_dart_main");
}
