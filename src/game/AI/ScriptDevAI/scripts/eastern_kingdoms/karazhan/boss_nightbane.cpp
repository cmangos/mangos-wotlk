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

#include "AI/ScriptDevAI/include/precompiled.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

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
};

struct boss_nightbaneAI : public npc_escortAI
{
    boss_nightbaneAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_karazhan*)pCreature->GetInstanceData();
        Reset();
    }

    instance_karazhan* m_pInstance;

    uint8 m_uiPhase;
    uint8 m_uiFlightPhase;
    uint32 m_uiPhaseResetTimer;

    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiCharredEarthTimer;
    uint32 m_uiSmolderingBreathTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleavetimer;

    uint32 m_uiDistractingAshTimer;
    uint32 m_uiRainBonesTimer;
    uint32 m_uiSmokingBlastTimer;
    uint32 m_uiFireballBarrageTimer;

    GuidVector m_skeletons;

    bool m_bCombatStarted;

    void Reset() override
    {
        m_uiPhase                   = PHASE_GROUND;
        m_uiFlightPhase             = 1;
        m_bCombatStarted            = false;

        m_uiBellowingRoarTimer      = urand(30000, 45000);
        m_uiCharredEarthTimer       = urand(10000, 15000);
        m_uiSmolderingBreathTimer   = urand(9000, 13000);
        m_uiTailSweepTimer          = urand(12000, 15000);
        m_uiCleavetimer             = urand(4000, 8000);

        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
        m_creature->SetCanFly(false);
        m_creature->SetHover(false);
        m_creature->SetLevitate(true);
        SetDeathPrevention(false);

        m_skeletons.clear();
    }

    void DoResetAirTimers()
    {
        m_uiPhaseResetTimer         = urand(45000, 55000);
        m_uiRainBonesTimer          = 3000;
        m_uiDistractingAshTimer     = urand(10000, 12000);
        m_uiSmokingBlastTimer       = urand(10000, 12000);
        m_uiFireballBarrageTimer    = 10000;
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NIGHTBANE, DONE);
    }

    void EnterEvadeMode() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NIGHTBANE, FAIL);

        for (ObjectGuid& guid : m_skeletons)
            if (Creature* skeleton = m_creature->GetMap()->GetCreature(guid))
                skeleton->ForcedDespawn();

        // reset boss on evade
        m_creature->ForcedDespawn();
    }

    void CorpseRemoved(uint32& respawnDelay) override
    {
        npc_escortAI::CorpseRemoved(respawnDelay);
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_NIGHTBANE) == FAIL)
                respawnDelay = 30; // respawn after 30 seconds on failure
        }
    }

    void JustRespawned() override
    {
        npc_escortAI::JustRespawned();
        if (GameObject* pUrn = m_pInstance->GetSingleGameObjectFromStorage(GO_BLACKENED_URN))
            pUrn->ResetDoorOrButton();
    }

    void JustSummoned(Creature* summoned) override
    {
        m_skeletons.push_back(summoned->GetObjectGuid());

        if (m_creature->getVictim())
            summoned->AI()->AttackStart(m_creature->getVictim());
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        m_skeletons.erase(std::remove(m_skeletons.begin(), m_skeletons.end(), summoned->GetObjectGuid()), m_skeletons.end());
    }

    void WaypointReached(uint32 uiPointId) override
    {
        // Set in combat after the intro is done
        if (uiPointId == 31)
        {
            SetEscortPaused(true);
            m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
            m_creature->SetCanFly(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
            m_creature->SetLevitate(false);
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);

            m_bCombatStarted = true;
            m_creature->SetInCombatWithZone();
            AttackClosestEnemy();
        }
    }

    void PhaseTransitionTimersReset()
    {
        if (m_uiPhase == PHASE_GROUND)
        {
            m_uiBellowingRoarTimer = 10000;
            m_uiSmolderingBreathTimer = 5000;
        }
        else
        {
            // TODO:
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId) override
    {
        // avoid overlapping of escort and combat movement
        if (!m_bCombatStarted)
            npc_escortAI::MovementInform(uiMotionType, uiPointId);
        else
        {
            if (uiMotionType != POINT_MOTION_TYPE)
                return;

            switch (uiPointId)
            {
                case POINT_ID_AIR:
                    m_uiPhase = PHASE_AIR;
                    break;
                case POINT_ID_GROUND:
                    // TODO: remove this once MMAPs are more reliable in the area
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    m_creature->SetCanFly(false);
                    m_creature->SetHover(false);
                    m_uiPhase = PHASE_GROUND;
                    SetCombatMovement(true);
                    SetDeathPrevention(false);
                    DoResetThreat();
                    AttackClosestEnemy();
                    break;
            }
            PhaseTransitionTimersReset();
        }
    }

    // Wrapper to handle movement to the closest trigger
    void DoMoveToClosestTrigger(bool bGround)
    {
        if (!m_pInstance)
            return;

        Unit* pChosenTrigger = nullptr;
        GuidList lTriggersList;
        float fX, fY, fZ;

        // get the list of wanted triggers
        m_pInstance->GetNightbaneTriggers(lTriggersList, bGround);

        // calculate the closest trigger from the list
        for (GuidList::const_iterator itr = lTriggersList.begin(); itr != lTriggersList.end(); ++itr)
        {
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!pChosenTrigger || m_creature->GetDistanceOrder(pTrigger, pChosenTrigger, false, DIST_CALC_NONE))
                    pChosenTrigger = pTrigger;
            }
        }

        // Move to trigger position
        if (pChosenTrigger)
        {
            pChosenTrigger->GetPosition(fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePointTOL(bGround ? POINT_ID_GROUND : POINT_ID_AIR, fX, fY, fZ, !bGround);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
            case PHASE_GROUND:

                if (m_uiBellowingRoarTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_BELLOWING_ROAR) == CAST_OK)
                        m_uiBellowingRoarTimer = urand(30000, 45000);
                }
                else
                    m_uiBellowingRoarTimer -= uiDiff;

                if (m_uiSmolderingBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SMOLDERING_BREATH) == CAST_OK)
                        m_uiSmolderingBreathTimer = urand(14000, 20000);
                }
                else
                    m_uiSmolderingBreathTimer -= uiDiff;

                if (m_uiCharredEarthTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_CHARRED_EARTH_TARGETING) == CAST_OK) // shouldnt be sent to client
                        m_uiCharredEarthTimer = urand(25000, 35000);
                }
                else
                    m_uiCharredEarthTimer -= uiDiff;

                if (m_uiTailSweepTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_TAIL_SWEEP) == CAST_OK)
                        m_uiTailSweepTimer = urand(14000, 20000);
                }
                else
                    m_uiTailSweepTimer -= uiDiff;

                if (m_uiCleavetimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleavetimer = urand(6000, 12000);
                }
                else
                    m_uiCleavetimer -= uiDiff;

                if (m_creature->GetHealthPercent() < 100 - 25 * m_uiFlightPhase)
                {
                    // Start air phase movement (handled by creature_movement_template)
                    SetCombatMovement(false);
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    m_creature->SetCanFly(true);
                    m_creature->SetHover(true);
                    DoMoveToClosestTrigger(false);

                    DoScriptText(SAY_AIR_PHASE, m_creature);
                    m_uiPhase = PHASE_TRANSITION;
                    SetDeathPrevention(true);
                    DoResetAirTimers();
                    ++m_uiFlightPhase;
                }

                DoMeleeAttackIfReady();

                break;
            case PHASE_AIR:

                if (m_uiRainBonesTimer)
                {
                    if (m_uiRainBonesTimer <= uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                        {
                            if (m_creature->CastSpell(pTarget, SPELL_RAIN_OF_BONES, TRIGGERED_NONE) == SPELL_CAST_OK)
                            {
                                DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                                m_uiRainBonesTimer = 0;
                            }
                        }
                    }
                    else
                        m_uiRainBonesTimer -= uiDiff;
                }

                if (m_uiSmokingBlastTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SMOKING_BLAST) == CAST_OK) // should hit highest aggro target
                            m_uiSmokingBlastTimer = urand(1000, 3000);
                    }
                }
                else
                    m_uiSmokingBlastTimer -= uiDiff;

                if (m_uiDistractingAshTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DISTRACTING_ASH) == CAST_OK) // should be used in both phases
                            m_uiDistractingAshTimer = urand(7000, 13000);
                    }
                }
                else
                    m_uiDistractingAshTimer -= uiDiff;

                if (m_uiFireballBarrageTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_FARTHEST_AWAY, 0, SPELL_FIREBALL_BARRAGE, SELECT_FLAG_PLAYER))
                        if (pTarget->IsWithinDist(m_creature, 60.f) || m_creature->CastSpell(m_creature->getVictim(), SPELL_FIREBALL_BARRAGE, TRIGGERED_NONE) == SPELL_CAST_OK)
                            m_uiFireballBarrageTimer = urand(3000, 6000);  // if farthest target is 40+ yd away
                }
                else
                    m_uiFireballBarrageTimer -= uiDiff;

                if (m_uiPhaseResetTimer < uiDiff)
                {
                    // ToDo: more circle movement should be done here!
                    DoScriptText(urand(0, 1) ? SAY_LAND_PHASE_1 : SAY_LAND_PHASE_2, m_creature);
                    DoMoveToClosestTrigger(true);

                    m_uiPhase = PHASE_TRANSITION;
                    m_uiPhaseResetTimer = urand(45000, 55000);
                }
                else
                    m_uiPhaseResetTimer -= uiDiff;

                break;
            case PHASE_TRANSITION:
                // nothing here
                break;
        }
    }
};

UnitAI* GetAI_boss_nightbane(Creature* pCreature)
{
    return new boss_nightbaneAI(pCreature);
}

bool ProcessEventId_event_spell_summon_nightbane(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        ScriptedInstance* pInstance = (ScriptedInstance*)((Player*)pSource)->GetInstanceData();
        if (!pInstance)
            return false;

        if (pInstance->GetData(TYPE_NIGHTBANE) == NOT_STARTED || pInstance->GetData(TYPE_NIGHTBANE) == FAIL)
        {
            Creature* nightbane = pInstance->GetSingleCreatureFromStorage(NPC_NIGHTBANE);
            if (nightbane && nightbane->isAlive())
            {
                DoScriptText(EMOTE_AWAKEN, ((Player*)pSource));
                pInstance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);

                // Sort of a hack, it is unclear how this really work but the values appear to be valid (see Onyxia, too)
                nightbane->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                nightbane->SetStandState(UNIT_STAND_STATE_STAND);
                nightbane->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                nightbane->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                nightbane->SetLevitate(true);

                // Switch to waypoint movement
                if (boss_nightbaneAI* nightbaneAI = dynamic_cast<boss_nightbaneAI*>(nightbane->AI()))
                    nightbaneAI->Start(true);
            }
        }
    }

    return true;
}

void AddSC_boss_nightbane()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_nightbane";
    pNewScript->GetAI = &GetAI_boss_nightbane;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_nightbane";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_nightbane;
    pNewScript->RegisterSelf();
}
