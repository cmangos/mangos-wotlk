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
SDName: Boss_Nazan_And_Vazruden
SD%Complete: 95
SDComment: Bellowing Roar Timer (heroic) needs some love
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hellfire_ramparts.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_INTRO               = -1543017,
    SAY_AGGRO1              = -1543018,
    SAY_AGGRO2              = -1543019,
    SAY_AGGRO3              = -1543020,
    SAY_TAUNT               = -1543021,
    SAY_KILL1               = -1543022,
    SAY_KILL2               = -1543023,
    SAY_DEATH               = -1543024,
    EMOTE_DESCEND           = -1543025,

    // vazruden
    SPELL_REVENGE = 19130,
    SPELL_REVENGE_H = 40392,
    SPELL_VAZRUDENS_MARK = 30689,               // Unused - seemingly non-tank targeted
    SPELL_DEFENSIVE_STATE = 5301,

    // nazan
    SPELL_FACE_HIGHEST_THREAT = 30700,          // normal+heroic flight
    SPELL_SUMMON_VAZRUDEN = 30717,              // normal+heroic DoSplit();

    SPELL_FIREBALL_HIGHEST_THREAT = 30691,      // cast 2-3 times afer 30700, normal flight
    SPELL_FIREBALL_HIGHEST_THREAT_H = 32491,    // cast 2-3 times afer 30700 heroic flight

    SPELL_FIREBALL_FLIGHT = 33793,              // normal flight inbetween 30691
    SPELL_FIREBALL_FLIGHT_H = 33794,            // heroic flight inbetween 32491

    SPELL_FIREBALL_GROUND = 34653,              // normal ground
    SPELL_FIREBALL_GROUND_H = 36920,            // heroic ground

    SPELL_CONE_OF_FIRE = 30926,                 // normal ground
    SPELL_CONE_OF_FIRE_H = 36921,               // heroic ground

    SPELL_BELLOW_ROAR_H = 39427,                // heroic ground

    // misc
    POINT_ID_CENTER = 100,
    POINT_ID_FLYING = 101,
    POINT_ID_FLIGHT_MOVE = 102,
    POINT_ID_LANDING = 103,
    POINT_ID_LANDED = 104,

    NPC_NAZAN = 17536,
};

const Position landingPosition = { -1410.52f, 1740.585f, 89.05998f, 100.f}; // moves here to drop off nazan
const Position flyingPositions[] =
{
    { -1423.491f, 1710.401f, 103.7056f, 100.f},
    {-1375.356f, 1744.604f, 99.14612f, 100.f},
    {-1444.99f, 1752.96f, 98.20342f, 100.f},
}; // moves here to land and fight
const Position landingPhasePos = { -1429.172f, 1749.129f, 84.3476f, 100.f };
const Position landedPhasePos = { -1429.172f, 1749.129f, 81.26282f, 100.f };

enum NazanActions
{
    NAZAN_LAND,
    NAZAN_FLIGHT_MOVE,
    NAZAN_FIREBALL_FLIGHT,
    NAZAN_FIREBALL_GROUND,
    NAZAN_CONE_OF_FIRE,
    NAZAN_BELLOWING_ROAR,
    NAZAN_ACTION_MAX,
    NAZAN_ATTACK_DELAY,
};

// This is the flying mob ("mounted" on dragon) spawned initially
// This npc will morph into the "unmounted" dragon (nazan) after vazruden is summoned and continue flying
struct boss_vazruden_heraldAI : public CombatAI
{
    boss_vazruden_heraldAI(Creature* creature) : CombatAI(creature, NAZAN_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_inRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(NAZAN_LAND, true);
        AddCombatAction(NAZAN_FLIGHT_MOVE, true);
        AddCombatAction(NAZAN_FIREBALL_FLIGHT, true);
        AddCombatAction(NAZAN_FIREBALL_GROUND, true);
        AddCombatAction(NAZAN_CONE_OF_FIRE, true);
        if (!m_inRegularMode)
            AddCombatAction(NAZAN_BELLOWING_ROAR, true);
        AddCustomAction(NAZAN_ATTACK_DELAY, true, [&](){ HandleAttackDelay(); });
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float, float)
        {
            return x >= -1336.0f;
        });
        SetReactState(REACT_DEFENSIVE);
        creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_inRegularMode;
    uint32 m_lastFlightPoint;

    bool m_bIsEventInProgress;

    ObjectGuid m_lastSeenPlayerGuid;
    ObjectGuid m_vazrudenGuid;

    void Reset() override
    {
        CombatAI::Reset();
        if (m_creature->GetEntry() != NPC_VAZRUDEN_HERALD)
            m_creature->UpdateEntry(NPC_VAZRUDEN_HERALD);

        m_bIsEventInProgress = false;
        m_lastSeenPlayerGuid.Clear();
        m_vazrudenGuid.Clear();

        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
        m_creature->SetLevitate(true);
        m_creature->SetHover(true);
        m_creature->SetCanFly(true);
        m_creature->SetImmobilizedState(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoMoveToCenter();
    }

    void AttackStart(Unit* pWho) override
    {
        if (m_instance && m_instance->GetData(TYPE_NAZAN) != IN_PROGRESS)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
        if (m_vazrudenGuid)
        {
            if (Creature* vazruden = m_creature->GetMap()->GetCreature(m_vazrudenGuid))
            {
                if (vazruden->IsInCombat())
                {
                    DoScriptText(SAY_TAUNT, vazruden);
                    vazruden->AI()->EnterEvadeMode();
                }
            }
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (!m_instance)
            return;

        if (motionType == POINT_MOTION_TYPE)
        {
            switch (pointId)
            {
                case POINT_ID_CENTER:
                    DoSplit();
                    break;
                case POINT_ID_LANDING:
                {
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_LANDED, landingPhasePos.x, landingPhasePos.y, landingPhasePos.z);
                    break;
                }
                case POINT_ID_LANDED:
                {
                    m_instance->SetData(TYPE_NAZAN, IN_PROGRESS);

                    // Landing
                    // undo flying
                    m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                    m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    m_creature->SetLevitate(false);
                    m_creature->SetHover(false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                    m_creature->SetCanFly(false);
                    HandleAttackDelay();
                    break;
                }
                case POINT_ID_FLYING:
                    HandlePhaseChange(false);
                // [[fallthrough]]
                case POINT_ID_FLIGHT_MOVE:
                    SetCombatScriptStatus(false);
                    m_creature->SetImmobilizedState(true);
                    m_creature->CastSpell(nullptr, SPELL_FACE_HIGHEST_THREAT, TRIGGERED_OLD_TRIGGERED);
                    break;
            }
        }
    }

    void DoMoveToCenter()
    {
        DoScriptText(SAY_INTRO, m_creature);
        SetCombatMovement(false);
        SetCombatScriptStatus(true);
        SetMeleeEnabled(false);
        m_creature->SetInCombatWithZone();
        // catmulrom in sniff
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, landingPosition.x, landingPosition.y, landingPosition.z);
    }

    void DoSplit()
    {
        m_creature->UpdateEntry(NPC_NAZAN);

        DoCastSpellIfCan(nullptr, SPELL_SUMMON_VAZRUDEN);

        m_lastFlightPoint = 0;
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_FLYING, flyingPositions[0].x, flyingPositions[0].y, flyingPositions[0].z);
    }

    void DoMoveToCombat()
    {
        if (!GetActionReadyStatus(NAZAN_LAND) || !m_instance || m_instance->GetData(TYPE_NAZAN) == NOT_STARTED)
            return;

        m_creature->SetImmobilizedState(false);
        SetActionReadyStatus(NAZAN_LAND, false);

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_LANDING, landingPhasePos.x, landingPhasePos.y, landingPhasePos.z);
        SetCombatScriptStatus(true);
        DoScriptText(EMOTE_DESCEND, m_creature);
    }

    void HandleAttackDelay()
    {
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);
        m_creature->SetInCombatWithZone();
        HandlePhaseChange(true);
        AttackClosestEnemy();
    }

    void HandlePhaseChange(bool phase)
    {
        if (!phase)
        {
            // flying
            ResetCombatAction(NAZAN_FIREBALL_FLIGHT, 0);
            ResetCombatAction(NAZAN_FLIGHT_MOVE, urand(25000, 35000));
        }
        else
        {
            // landing
            DisableCombatAction(NAZAN_FIREBALL_FLIGHT);
            DisableCombatAction(NAZAN_FLIGHT_MOVE);
            ResetCombatAction(NAZAN_FIREBALL_GROUND, urand(5200, 16500));
            ResetCombatAction(NAZAN_CONE_OF_FIRE, urand(8100, 19700));
            if (!m_inRegularMode)
                ResetCombatAction(NAZAN_BELLOWING_ROAR, 2000);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_VAZRUDEN)
            return;

        summoned->SetInCombatWithZone();

        m_vazrudenGuid = summoned->GetObjectGuid();

        if (m_instance)
            m_instance->SetData(TYPE_VAZRUDEN, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NAZAN, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_NAZAN, FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case NAZAN_LAND:
            {
                if (!CanExecuteCombatAction() || m_creature->GetHealthPercent() > 20.0f)
                    return;

                DoMoveToCombat();
                break;
            }
            case NAZAN_FLIGHT_MOVE:
            {
                if (!CanExecuteCombatAction())
                    return;

                m_lastFlightPoint = (m_lastFlightPoint + urand(0, 1)) % 3;
                m_creature->SetImmobilizedState(false);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_FLIGHT_MOVE, flyingPositions[m_lastFlightPoint].x, flyingPositions[m_lastFlightPoint].y, flyingPositions[m_lastFlightPoint].z);
                SetCombatScriptStatus(true);
                ResetCombatAction(action, urand(25000, 35000));
                break;
            }
            case NAZAN_FIREBALL_FLIGHT:
            {
                if (Creature* vazruden = m_creature->GetMap()->GetCreature(m_vazrudenGuid))
                {
                    if (Unit* target = vazruden->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(target, m_inRegularMode ? SPELL_FIREBALL_FLIGHT : SPELL_FIREBALL_FLIGHT_H) == CAST_OK)
                            ResetCombatAction(action, urand(2100, 7300));
                    }
                }
                break;
            }
            case NAZAN_FIREBALL_GROUND:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, m_inRegularMode ? SPELL_FIREBALL_GROUND : SPELL_FIREBALL_GROUND_H) == CAST_OK)
                        ResetCombatAction(action, urand(7300, 13200));
                }
                break;
            }
            case NAZAN_CONE_OF_FIRE:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_inRegularMode ? SPELL_CONE_OF_FIRE : SPELL_CONE_OF_FIRE_H) == CAST_OK)
                    ResetCombatAction(action, urand(7300, 13200));
                break;
            }
            case NAZAN_BELLOWING_ROAR:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BELLOW_ROAR_H) == CAST_OK)
                    ResetCombatAction(action, urand(40000, 50000));
                break;
            }
        }
    }
};

enum VazrudenActions
{
    VAZRUDEN_PHASE_2,
    VAZRUDEN_REVENGE,
    VAZRUDEN_ACTION_MAX,
};

// This is the summoned boss ("dismounted") that starts attacking the players
struct boss_vazrudenAI : public CombatAI
{
    boss_vazrudenAI(Creature* creature) : CombatAI(creature, VAZRUDEN_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_inRegularMode(creature->GetMap()->IsRegularDifficulty())
    {
        AddTimerlessCombatAction(VAZRUDEN_PHASE_2, true);
        AddCombatAction(VAZRUDEN_REVENGE, 5500, 8400);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float, float)
        {
            return x >= -1336.0f;
        });

        DoCastSpellIfCan(nullptr, SPELL_DEFENSIVE_STATE, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    ScriptedInstance* m_instance;
    bool m_inRegularMode;

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
    }

    void EnterEvadeMode() override
    {
        DoScriptText(SAY_TAUNT, m_creature);
        CombatAI::EnterEvadeMode();
        if (Creature* nazan = m_instance->GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
            if (nazan->IsInCombat())
                nazan->AI()->EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_VAZRUDEN, DONE);

        if (GetActionReadyStatus(VAZRUDEN_PHASE_2))
            MakeNazanLand();
    }

    void MakeNazanLand()
    {
        if (m_instance)
            if (Creature* nazan = m_instance->GetSingleCreatureFromStorage(NPC_VAZRUDEN_HERALD))
                if (boss_vazruden_heraldAI* nazanAI = static_cast<boss_vazruden_heraldAI*>(nazan->AI()))
                    nazanAI->DoMoveToCombat();

        SetActionReadyStatus(VAZRUDEN_PHASE_2, false);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_VAZRUDEN, FAIL);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case VAZRUDEN_PHASE_2:
            {
                if (m_creature->GetHealthPercent() > 40.f)
                    return;

                MakeNazanLand();
                break;
            }
            case VAZRUDEN_REVENGE:
            {
                if (!m_creature->HasAuraState(AURA_STATE_DEFENSE))
                    break;

                if (DoCastSpellIfCan(m_creature->GetVictim(), m_inRegularMode ? SPELL_REVENGE : SPELL_REVENGE_H) == CAST_OK)
                    ResetCombatAction(action, urand(11400, 14300));
                break;
            }
        }
    }
};

struct VazrudenLiquidFire : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->CastSpell(nullptr, target->GetMap()->IsRegularDifficulty() ? 23971 : 30928, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FaceHighestThreat : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (HostileReference* ref = caster->getThreatManager().getThreatList().front())
            caster->SetFacingTo(caster->GetAngle(ref->getTarget()));
    }
};

void AddSC_boss_nazan_and_vazruden()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_vazruden";
    pNewScript->GetAI = &GetNewAIInstance<boss_vazrudenAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_vazruden_herald";
    pNewScript->GetAI = &GetNewAIInstance<boss_vazruden_heraldAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<VazrudenLiquidFire>("spell_vazruden_liquid_fire_script");
    RegisterSpellScript<FaceHighestThreat>("spell_face_highest_threat");
}
