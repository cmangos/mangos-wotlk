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
SDName: Boss_Nightbane
SD%Complete: 80
SDComment: Boss apparently only takes assist threat in P2
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "MotionGenerators/WaypointManager.h"
#include <G3D/Vector3.h>

enum
{
    EMOTE_AWAKEN                = -1532125,
    SAY_AGGRO                   = -1532126,
    SAY_AIR_PHASE               = -1532127,
    SAY_LAND_PHASE_1            = -1532128,
    SAY_LAND_PHASE_2            = -1532129,
    EMOTE_DEEP_BREATH           = -1532130,

    // ground phase spells
    SPELL_BELLOWING_ROAR        = 36922,
    SPELL_CHARRED_EARTH         = 30129,                    // Also 30209 (Target Charred Earth) triggers this
    SPELL_CHARRED_EARTH_TARGETING = 30209,
    SPELL_SMOLDERING_BREATH     = 30210,
    SPELL_TAIL_SWEEP            = 25653,
    SPELL_CLEAVE                = 30131,
    SPELL_DISTRACTING_ASH_GROUND= 30280, // prenerf 2.1.0

    // air phase spells
    SPELL_DISTRACTING_ASH       = 30130,
    SPELL_RAIN_OF_BONES         = 37098,                    // should trigger 30170
    SPELL_SMOKING_BLAST         = 37057,
    SPELL_FIREBALL_BARRAGE      = 30282,

    PHASE_GROUND                = 1,
    PHASE_AIR                   = 2,
    PHASE_TRANSITION            = 3,

    // These points are a placeholder for the air phase movement. The dragon should do some circles around the area before landing again
    POINT_ID_AIR                = 1,
    POINT_ID_GROUND             = 2,

    POINT_INTRO_END             = 11,
    POINT_LANDING_END           = 6,
};

enum NightbaneActions
{
    NIGHTBANE_PHASE_RESET,
    NIGHTBANE_PHASE_2,
    NIGHTBANE_BELLOWING_ROAR,
    NIGHTBANE_CHARRED_EARTH,
    NIGHTBANE_SMOLDERING_BREATH,
    NIGHTBANE_TAIL_SWEEP,
    NIGHTBANE_CLEAVE,
    NIGHTBANE_DISTRACTING_ASH,
    NIGHTBANE_RAIN_OF_BONES,
    NIGHTBANE_SMOKING_BLAST,
    NIGHTBANE_FIREBALL_BARRAGE,
    NIGHTBANE_ACTION_MAX,
    NIGHTBANE_ATTACK_DELAY,
};

struct boss_nightbaneAI : public CombatAI
{
    boss_nightbaneAI(Creature* creature) : CombatAI(creature, NIGHTBANE_ACTION_MAX), m_instance(static_cast<instance_karazhan*>(creature->GetInstanceData()))
    {
        AddCombatAction(NIGHTBANE_PHASE_RESET, true);
        AddTimerlessCombatAction(NIGHTBANE_PHASE_2, true);
        AddCombatAction(NIGHTBANE_BELLOWING_ROAR, 55000, 60000);
        AddCombatAction(NIGHTBANE_CHARRED_EARTH, 10000, 15000);
        AddCombatAction(NIGHTBANE_SMOLDERING_BREATH, 9000, 13000);
        AddCombatAction(NIGHTBANE_TAIL_SWEEP, 12000, 15000);
        AddCombatAction(NIGHTBANE_CLEAVE, 4000, 8000);
        AddCombatAction(NIGHTBANE_DISTRACTING_ASH, 16000, 22000);
        AddCombatAction(NIGHTBANE_RAIN_OF_BONES, true);
        AddCombatAction(NIGHTBANE_SMOKING_BLAST, true);
        AddCombatAction(NIGHTBANE_FIREBALL_BARRAGE, true);
        AddCustomAction(NIGHTBANE_ATTACK_DELAY, true, [&]() { HandleAttackDelay(); });
    }

    instance_karazhan* m_instance;

    uint8 m_phase;
    uint8 m_uiFlightPhase;

    GuidVector m_skeletons;

    bool m_bCombatStarted;

    void Reset() override
    {
        CombatAI::Reset();
        m_phase                   = PHASE_GROUND;
        m_uiFlightPhase             = 1;
        m_bCombatStarted            = false;

        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
        m_creature->SetCanFly(false);
        m_creature->SetHover(false);
        m_creature->SetLevitate(true);
        SetDeathPrevention(false);
        m_creature->SetSupportThreatOnly(false);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);

        m_skeletons.clear();
    }

    void StartIntro()
    {
        m_creature->GetMotionMaster()->MovePath(0, PATH_FROM_EXTERNAL, FORCED_MOVEMENT_RUN);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NIGHTBANE, DONE);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NIGHTBANE, FAIL);

        for (ObjectGuid& guid : m_skeletons)
            if (Creature* skeleton = m_creature->GetMap()->GetCreature(guid))
                skeleton->ForcedDespawn();

        // reset boss on evade
        m_creature->ForcedDespawn();
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_NIGHTBANE) == FAIL)
                respawnDelay = 30; // respawn after 30 seconds on failure
        }
    }

    void JustRespawned() override
    {
        CombatAI::JustRespawned();
        if (GameObject* urn = m_instance->GetSingleGameObjectFromStorage(GO_BLACKENED_URN))
            urn->ResetDoorOrButton();
    }

    void JustSummoned(Creature* summoned) override
    {
        m_skeletons.push_back(summoned->GetObjectGuid());

        if (m_creature->GetVictim())
            summoned->AI()->AttackStart(m_creature->GetVictim());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        m_skeletons.erase(std::remove(m_skeletons.begin(), m_skeletons.end(), summoned->GetObjectGuid()), m_skeletons.end());
        if (m_skeletons.empty() && m_phase == PHASE_AIR && !m_creature->HasAura(SPELL_RAIN_OF_BONES))
            ResetCombatAction(NIGHTBANE_PHASE_RESET, 1000);
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType == WAYPOINT_MOTION_TYPE)
        {
            if (m_bCombatStarted) // combat movement
            {
                if (pointId == POINT_LANDING_END)
                {
                    m_creature->SetIgnoreMMAP(false);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    m_creature->SetCanFly(false);
                    m_creature->SetHover(false);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_GROUND, -11162.23f, -1900.329f, 91.47265f); // noted as falling in sniff
                }
            }
            else // intro movement
            {
                // Set in combat after the intro is done
                if (pointId == POINT_INTRO_END)
                {
                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                    m_creature->SetCanFly(false);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->SetLevitate(false);
                    m_creature->SetHover(false);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);

                    m_bCombatStarted = true;
                    ResetTimer(NIGHTBANE_ATTACK_DELAY, 2000);
                }
            }
        }
        // avoid overlapping of escort and combat movement
        else if (motionType == POINT_MOTION_TYPE)
        {
            switch (pointId)
            {
                case POINT_ID_AIR:
                    m_phase = PHASE_AIR;
                    SetCombatScriptStatus(false);
                    HandlePhaseTransition();
                    break;
                case POINT_ID_GROUND:
                    // TODO: remove this once MMAPs are more reliable in the area
                    m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                    m_phase = PHASE_GROUND;
                    SetCombatMovement(true);
                    SetDeathPrevention(false);
                    SetMeleeEnabled(true);
                    DoResetThreat();
                    m_creature->SetSupportThreatOnly(false);
                    ResetTimer(NIGHTBANE_ATTACK_DELAY, 2000);
                    break;
            }
        }
    }

    void HandleAttackDelay()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->SetInCombatWithZone();
        SetCombatScriptStatus(false);
        AttackClosestEnemy();
        HandlePhaseTransition();
    }

    // Wrapper to handle movement to the closest trigger
    void DoMoveToClosestTrigger(bool ground)
    {
        if (!m_instance)
            return;

        Unit* chosenTrigger = nullptr;
        GuidList lTriggersList;
        float fX, fY, fZ;

        // get the list of wanted triggers
        m_instance->GetNightbaneTriggers(lTriggersList, ground);

        // calculate the closest trigger from the list
        for (GuidList::const_iterator itr = lTriggersList.begin(); itr != lTriggersList.end(); ++itr)
        {
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!chosenTrigger || (ground ? !m_creature->GetDistanceOrder(pTrigger, chosenTrigger, false, DIST_CALC_NONE) : m_creature->GetDistanceOrder(pTrigger, chosenTrigger, false, DIST_CALC_NONE)))
                    chosenTrigger = pTrigger;
            }
        }

        // Move to trigger position
        if (chosenTrigger)
        {
            chosenTrigger->GetPosition(fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePointTOL(ground ? POINT_ID_GROUND : POINT_ID_AIR, fX, fY, fZ, !ground, FORCED_MOVEMENT_WALK);
        }
    }

    void HandlePhaseTransition()
    {
        switch (m_phase)
        {
            case PHASE_GROUND:
            {
                ResetCombatAction(NIGHTBANE_BELLOWING_ROAR, 10000);
                ResetCombatAction(NIGHTBANE_CHARRED_EARTH, 10000);
                ResetCombatAction(NIGHTBANE_SMOLDERING_BREATH, 5000);
                ResetCombatAction(NIGHTBANE_TAIL_SWEEP, 10000);
                ResetCombatAction(NIGHTBANE_CLEAVE, 10000);
                DisableCombatAction(NIGHTBANE_PHASE_RESET);
                DisableCombatAction(NIGHTBANE_RAIN_OF_BONES);
                DisableCombatAction(NIGHTBANE_SMOKING_BLAST);
                DisableCombatAction(NIGHTBANE_FIREBALL_BARRAGE);
                break;
            }
            case PHASE_AIR:
            {
                ResetCombatAction(NIGHTBANE_PHASE_RESET, urand(45000, 55000));
                ResetCombatAction(NIGHTBANE_DISTRACTING_ASH, urand(10000, 12000));
                ResetCombatAction(NIGHTBANE_RAIN_OF_BONES, 3000);
                ResetCombatAction(NIGHTBANE_SMOKING_BLAST, urand(10000, 12000));
                ResetCombatAction(NIGHTBANE_FIREBALL_BARRAGE, 10000);
                DisableCombatAction(NIGHTBANE_BELLOWING_ROAR);
                DisableCombatAction(NIGHTBANE_CHARRED_EARTH);
                DisableCombatAction(NIGHTBANE_SMOLDERING_BREATH);
                DisableCombatAction(NIGHTBANE_TAIL_SWEEP);
                DisableCombatAction(NIGHTBANE_CLEAVE);
                break;
            }
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NIGHTBANE_PHASE_RESET:
            {
                DoScriptText(urand(0, 1) ? SAY_LAND_PHASE_1 : SAY_LAND_PHASE_2, m_creature);
                m_creature->GetMotionMaster()->MovePath(1, PATH_FROM_ENTRY, FORCED_MOVEMENT_WALK);
                m_phase = PHASE_TRANSITION;
                SetCombatScriptStatus(true);
                DisableCombatAction(action);
                break;
            }
            case NIGHTBANE_PHASE_2:
            {
                if (m_creature->GetHealthPercent() < 100 - 25 * m_uiFlightPhase)
                {
                    // Start air phase movement (handled by creature_movement_template)
                    SetCombatMovement(false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    m_creature->SetCanFly(true);
                    m_creature->SetHover(true);
                    DoMoveToClosestTrigger(false);

                    DoScriptText(SAY_AIR_PHASE, m_creature);
                    m_phase = PHASE_TRANSITION;
                    SetDeathPrevention(true);
                    SetMeleeEnabled(false);
                    SetCombatScriptStatus(true);
                    DoResetThreat();
                    m_creature->SetSupportThreatOnly(true);
                    ++m_uiFlightPhase;
                    SetActionReadyStatus(action, true);
                }
                break;
            }
            case NIGHTBANE_BELLOWING_ROAR:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_BELLOWING_ROAR) == CAST_OK)
#ifdef PRENERF_2_0_3
                    ResetCombatAction(action, urand(30000, 45000));
#else
                    ResetCombatAction(action, urand(38000, 48000));
#endif
                break;
            }
            case NIGHTBANE_CHARRED_EARTH:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_CHARRED_EARTH_TARGETING) == CAST_OK) // shouldnt be sent to client
                    ResetCombatAction(action, urand(25000, 35000));
                break;
            }
            case NIGHTBANE_SMOLDERING_BREATH:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SMOLDERING_BREATH) == CAST_OK)
                    ResetCombatAction(action, urand(14000, 20000));
                break;
            }
            case NIGHTBANE_TAIL_SWEEP:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_TAIL_SWEEP) == CAST_OK)
                    ResetCombatAction(action, urand(14000, 20000));
                break;
            }
            case NIGHTBANE_CLEAVE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, urand(6000, 12000));
                break;
            }
            case NIGHTBANE_DISTRACTING_ASH:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, m_phase == PHASE_GROUND ? SPELL_DISTRACTING_ASH_GROUND : SPELL_DISTRACTING_ASH) == CAST_OK) // should be used in both phases
                        ResetCombatAction(action, urand(7000, 13000));
                break;
            }
            case NIGHTBANE_RAIN_OF_BONES:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (m_creature->CastSpell(target, SPELL_RAIN_OF_BONES, TRIGGERED_NONE) == SPELL_CAST_OK)
                    {
                        DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                        DisableCombatAction(action);
                    }
                }
                break;
            }
            case NIGHTBANE_SMOKING_BLAST:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SMOKING_BLAST) == CAST_OK) // should hit highest aggro target
                        ResetCombatAction(action, urand(1000, 3000));
                break;
            }
            case NIGHTBANE_FIREBALL_BARRAGE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_FARTHEST_AWAY, 0, SPELL_FIREBALL_BARRAGE, SELECT_FLAG_PLAYER))
                    if (m_creature->GetDistance(target, false, DIST_CALC_COMBAT_REACH) < 60.f || (m_creature->CastSpell(m_creature->GetVictim(), SPELL_FIREBALL_BARRAGE, TRIGGERED_NONE) == SPELL_CAST_OK))
                        ResetCombatAction(action, urand(3000, 6000)); // if farthest target is 40+ yd away
                break;
            }
        }
    }
};

bool ProcessEventId_event_spell_summon_nightbane(uint32 /*eventId*/, Object* source, Object* /*target*/, bool bIsStart)
{
    if (bIsStart && source->GetTypeId() == TYPEID_PLAYER)
    {
        ScriptedInstance* instance = static_cast<ScriptedInstance*>(static_cast<Player*>(source)->GetInstanceData());
        if (!instance)
            return false;

        if (instance->GetData(TYPE_NIGHTBANE) == NOT_STARTED || instance->GetData(TYPE_NIGHTBANE) == FAIL)
        {
            Creature* nightbane = instance->GetSingleCreatureFromStorage(NPC_NIGHTBANE);
            if (nightbane && nightbane->IsAlive())
            {
                DoScriptText(EMOTE_AWAKEN, ((Player*)source));
                instance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);

                // Sort of a hack, it is unclear how this really work but the values appear to be valid (see Onyxia, too)
                nightbane->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                nightbane->SetStandState(UNIT_STAND_STATE_STAND);
                nightbane->SetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                nightbane->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                nightbane->SetLevitate(true);
                nightbane->SetHover(true);

                // Switch to waypoint movement
                if (boss_nightbaneAI* nightbaneAI = dynamic_cast<boss_nightbaneAI*>(nightbane->AI()))
                    nightbaneAI->StartIntro();
            }
        }
    }

    return true;
}

void AddSC_boss_nightbane()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nightbane";
    pNewScript->GetAI = &GetNewAIInstance<boss_nightbaneAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_nightbane";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_nightbane;
    pNewScript->RegisterSelf();
}
