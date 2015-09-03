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
SDComment: Intro movement is a little choppy because of the lack of waypoint movement support. Air phase movement requires improvement. Timers need adjustment.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"
#include "escort_ai.h"

enum
{
    EMOTE_AWAKEN                = -1532125,
    SAY_AGGRO                   = -1532126,
    SAY_AIR_PHASE               = -1532127,
    SAY_LAND_PHASE_1            = -1532128,
    SAY_LAND_PHASE_2            = -1532129,
    EMOTE_DEEP_BREATH           = -1532130,

    // ground phase spells
    SPELL_BELLOWING_ROAR        = 39427,
    SPELL_CHARRED_EARTH         = 30129,                    // Also 30209 (Target Charred Earth) triggers this
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

    bool m_bCombatStarted;

    void Reset() override
    {
        m_uiPhase                   = PHASE_GROUND;
        m_uiFlightPhase             = 1;
        m_bCombatStarted            = false;

        m_uiBellowingRoarTimer      = urand(20000, 30000);
        m_uiCharredEarthTimer       = urand(10000, 15000);
        m_uiSmolderingBreathTimer   = urand(9000, 13000);
        m_uiTailSweepTimer          = urand(12000, 15000);
        m_uiCleavetimer             = urand(4000, 8000);

        SetCombatMovement(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void DoResetAirTimers()
    {
        m_uiPhaseResetTimer         = urand(20000, 40000);
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

        // reset boss on evade
        m_creature->ForcedDespawn();
    }

    void WaypointReached(uint32 uiPointId) override
    {
        // Set in combat after the intro is done
        if (uiPointId == 31)
        {
            SetEscortPaused(true);
            m_creature->SetLevitate(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

            m_bCombatStarted = true;
            m_creature->SetInCombatWithZone();
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
                    m_creature->SetLevitate(false);
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);

                    m_uiPhase = PHASE_GROUND;
                    SetCombatMovement(true);
                    DoStartMovement(m_creature->getVictim());
                    break;
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (m_creature->getVictim())
            pSummoned->AI()->AttackStart(m_creature->getVictim());
    }

    // Wrapper to handle movement to the closest trigger
    void DoMoveToClosestTrigger(bool bGround)
    {
        if (!m_pInstance)
            return;

        Unit* pChosenTrigger = NULL;
        GuidList lTriggersList;
        float fX, fY, fZ;

        // get the list of wanted triggers
        m_pInstance->GetNightbaneTriggers(lTriggersList, bGround);

        // calculate the closest trigger from the list
        for (GuidList::const_iterator itr = lTriggersList.begin(); itr != lTriggersList.end(); ++itr)
        {
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!pChosenTrigger || m_creature->GetDistanceOrder(pTrigger, pChosenTrigger, false))
                    pChosenTrigger = pTrigger;
            }
        }

        // Move to trigger position
        if (pChosenTrigger)
        {
            pChosenTrigger->GetPosition(fX, fY, fZ);
            m_creature->GetMotionMaster()->MovePoint(bGround ? POINT_ID_GROUND : POINT_ID_AIR, fX, fY, fZ);
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
                    if (DoCastSpellIfCan(m_creature, SPELL_BELLOWING_ROAR) == CAST_OK)
                        m_uiBellowingRoarTimer = urand(20000, 30000);
                }
                else
                    m_uiBellowingRoarTimer -= uiDiff;

                if (m_uiSmolderingBreathTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SMOLDERING_BREATH) == CAST_OK)
                        m_uiSmolderingBreathTimer = urand(14000, 20000);
                }
                else
                    m_uiSmolderingBreathTimer -= uiDiff;

                if (m_uiCharredEarthTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_CHARRED_EARTH) == CAST_OK)
                            m_uiCharredEarthTimer = urand(25000, 35000);
                    }
                }
                else
                    m_uiCharredEarthTimer -= uiDiff;

                if (m_uiTailSweepTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SWEEP) == CAST_OK)
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
                    m_creature->SetLevitate(true);
                    DoMoveToClosestTrigger(false);

                    DoScriptText(SAY_AIR_PHASE, m_creature);
                    m_uiPhase = PHASE_TRANSITION;
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
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_RAIN_OF_BONES) == CAST_OK)
                            {
                                DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                                m_uiRainBonesTimer = 0;
                            }
                        }
                    }
                    else
                        m_uiRainBonesTimer -= uiDiff;
                }

                if (m_uiDistractingAshTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DISTRACTING_ASH) == CAST_OK)
                            m_uiDistractingAshTimer = urand(7000, 13000);
                    }
                }
                else
                    m_uiDistractingAshTimer -= uiDiff;

                if (m_uiSmokingBlastTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_SMOKING_BLAST) == CAST_OK)
                            m_uiSmokingBlastTimer = urand(1000, 3000);
                    }
                }
                else
                    m_uiSmokingBlastTimer -= uiDiff;

                if (m_uiFireballBarrageTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_FIREBALL_BARRAGE, SELECT_FLAG_NOT_IN_MELEE_RANGE))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_FIREBALL_BARRAGE) == CAST_OK)
                            m_uiFireballBarrageTimer = urand(3000, 6000);
                    }
                }
                else
                    m_uiFireballBarrageTimer -= uiDiff;

                if (m_uiPhaseResetTimer < uiDiff)
                {
                    // ToDo: more circle movement should be done here!
                    DoScriptText(urand(0, 1) ? SAY_LAND_PHASE_1 : SAY_LAND_PHASE_2, m_creature);
                    DoMoveToClosestTrigger(true);

                    m_uiPhase = PHASE_TRANSITION;
                    m_uiPhaseResetTimer = 20000;
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

CreatureAI* GetAI_boss_nightbane(Creature* pCreature)
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
            if (Creature* pNightbane = pInstance->GetSingleCreatureFromStorage(NPC_NIGHTBANE))
            {
                DoScriptText(EMOTE_AWAKEN, ((Player*)pSource));
                pInstance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);

                // Sort of a hack, it is unclear how this really work but the values appear to be valid (see Onyxia, too)
                pNightbane->SetStandState(UNIT_STAND_STATE_STAND);
                pNightbane->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                pNightbane->SetLevitate(true);

                // Switch to waypoint movement
                if (boss_nightbaneAI* pNightbaneAI = dynamic_cast<boss_nightbaneAI*>(pNightbane->AI()))
                    pNightbaneAI->Start(true);
            }
        }
    }

    return true;
}

void AddSC_boss_nightbane()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_nightbane";
    pNewScript->GetAI = &GetAI_boss_nightbane;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_summon_nightbane";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_summon_nightbane;
    pNewScript->RegisterSelf();
}
