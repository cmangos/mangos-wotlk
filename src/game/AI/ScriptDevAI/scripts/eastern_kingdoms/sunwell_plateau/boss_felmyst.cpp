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
SDName: boss_felmyst
SD%Complete: 90%
SDComment: Intro movement NYI; Event cleanup (despawn & resummon) NYI; Breath phase spells could use some improvements.
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "Entities/TemporarySpawn.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_INTRO           = -1580036,
    SAY_KILL_1          = -1580037,
    SAY_KILL_2          = -1580038,
    SAY_DEATH           = -1580042,
    SAY_TAKEOFF         = -1580040,
    SAY_BREATH          = -1580039,
    SAY_BERSERK         = -1580041,
    EMOTE_DEEP_BREATH   = -1580107,

    SPELL_FELBLAZE_VISUAL       = 45068,        // Visual transform aura
    SPELL_NOXIOUS_FUMES         = 47002,
    SPELL_SOUL_SEVER            = 45918,        // kills all charmed targets at wipe - script effect for 45917
    SPELL_SOUL_SEVER_INSTAKILL  = 45917,
    SPELL_BERSERK               = 26662,

    // ground phase
    SPELL_CLEAVE                = 19983,
    SPELL_CORROSION             = 45866,
    SPELL_GAS_NOVA              = 45855,
    SPELL_ENCAPSULATE           = 45665,
    SPELL_ENCAPSULATE_CHANNEL   = 45661,

    // flight phase
    SPELL_SUMMON_VAPOR          = 45391,
    SPELL_VAPOR_SPAWN_TRIGGER   = 45388,
    SPELL_SPEED_BURST           = 45495,
    // SPELL_FOG_CORRUPTION        = 45582,

    SPELL_TRIGGER_TOP_STRAFE    = 45586,
    SPELL_TRIGGER_MIDDLE_STRAFE = 45622,
    SPELL_TRIGGER_BOTTOM_STRAFE = 45623,

    SPELL_STRAFE_TOP            = 45585,
    SPELL_STRAFE_MIDDLE         = 45633,
    SPELL_STRAFE_BOTTOM         = 45635,

    // demonic vapor spells
    SPELL_DEMONIC_VAPOR_PER     = 45411,
    SPELL_DEMONIC_VAPOR         = 45399,
    // SPELL_SUMMON_BLAZING_DEAD = 45400,
    
    SPELL_FOG_OF_CORRUPTION_CHARM = 45717,
    SPELL_FOG_OF_CORRUPTION_BUFF = 45726,

    SPELL_DUMMY_NUKE            = 21912,

    // npcs
    // NPC_UNYELDING_DEAD       = 25268,        // spawned during flight phase
    // NPC_DEMONIC_VAPOR           = 25265,        // npc which follows the player
    // NPC_DEMONIC_VAPOR_TRAIL     = 25267,

    // phases
    PHASE_GROUND                = 1,
    PHASE_AIR                   = 2,
    PHASE_TRANSITION            = 3,

    // subphases for air phase
    SUBPHASE_VAPOR              = 4,
    SUBPHASE_BREATH_SIDE        = 5,
    SUBPHASE_BREATH_PREPARE     = 6,
    SUBPHASE_BREATH_MOVE        = 7,

    POINT_AGGRO                 = 8,
};

/*######
## boss_felmyst
######*/

enum FelmystActions
{
    FELMYST_BERSERK,
    FELMYST_PHASE_CHANGE,
    FELMYST_ENCAPSULATE,
    FELMYST_CORROSION,
    FELMYST_CLEAVE,
    FELMYST_GAS_NOVA,
    FELMYST_ACTION_MAX,
    FELMYST_WAYPOINT_DELAY,
    FELMYST_DEMONIC_VAPOR,
    FELMYST_LIFTOFF_DELAY,
    FELMYST_BREATH_DELAY,
    FELMYST_PREBREATH_DELAY,
    FELMYST_TRANSITION_DELAY,
    FELMYST_SIDE_DELAY,
    FELMYST_DUMMY_NUKE,
};

struct boss_felmystAI : public CombatAI
{
    boss_felmystAI(Creature* creature) : CombatAI(creature, FELMYST_ACTION_MAX), m_instance(static_cast<instance_sunwell_plateau*>(creature->GetInstanceData()))
    {
        AddCombatAction(FELMYST_BERSERK, uint32(10 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(FELMYST_PHASE_CHANGE, 60000u);
        AddCombatAction(FELMYST_ENCAPSULATE, 20000, 30000);
        AddCombatAction(FELMYST_CORROSION, 13000u, 16000u);
        AddCombatAction(FELMYST_CLEAVE, 5000, 14000);
        AddCombatAction(FELMYST_GAS_NOVA, 18000, 20000);
        AddCustomAction(FELMYST_WAYPOINT_DELAY, true, [&]() { HandleFelmystSpawnDelay(); });
        AddCustomAction(FELMYST_DEMONIC_VAPOR, true, [&]() { HandleDemonicVapor(); });
        AddCustomAction(FELMYST_LIFTOFF_DELAY, true, [&]() { HandleLiftoff(); });
        AddCustomAction(FELMYST_BREATH_DELAY, true, [&]() { HandleBreath(); });
        AddCustomAction(FELMYST_PREBREATH_DELAY, true, [&]() { HandlePreBreath(); });
        AddCustomAction(FELMYST_TRANSITION_DELAY, true, [&]() { HandleTransitionDelay(); });
        AddCustomAction(FELMYST_SIDE_DELAY, true, [&]() { HandleSideMove(); });
        AddCustomAction(FELMYST_DUMMY_NUKE, true, [&]() { HandleDummyNuke(); });
        AddOnKillText(SAY_KILL_1, SAY_KILL_2);
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float z)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr;
            });
        }
        if (creature->IsTemporarySummon())
            static_cast<TemporarySpawn*>(m_creature)->SetSummonProperties(TEMPSPAWN_MANUAL_DESPAWN, 0);
    }

    instance_sunwell_plateau* m_instance;

    uint8 m_uiPhase;

    uint32 m_animationSpawnStage;

    // Air Phase timers
    uint8 m_uiSubPhase;
    bool m_bIsLeftSide;

    uint8 m_uiDemonicVaporCount;
    uint8 m_uiCorruptionCount;
    uint8 m_uiCorruptionIndex;

    void Reset() override
    {
        CombatAI::Reset();
        m_uiPhase               = PHASE_GROUND;

        // Air phase
        m_uiSubPhase            = SUBPHASE_VAPOR;
        m_uiDemonicVaporCount   = 0;
        m_uiCorruptionCount     = 0;

        DoCastSpellIfCan(nullptr, SPELL_FELBLAZE_VISUAL, CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_SOUL_SEVER, CAST_TRIGGERED);

        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetCombatScriptStatus(false);

        SetDeathPrevention(false);

        SetReactState(REACT_DEFENSIVE);
    }

    float GetFlightOrientation(bool isLeftSide) { return isLeftSide ? 4.675057 : 1.58825; }

    void JustRespawned() override
    {
        if (Creature* madrigosa = m_instance->GetSingleCreatureFromStorage(NPC_MADRIGOSA))
            madrigosa->ForcedDespawn();
        CombatAI::JustRespawned();
        if (m_instance && m_instance->GetData(TYPE_FELMYST) != FAIL)
        {
            m_animationSpawnStage = 0;
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            ResetTimer(FELMYST_WAYPOINT_DELAY, 5000);
        }
        else
        {
            m_creature->SetImmobilizedState(false);
            m_creature->SetHover(true);
            m_creature->GetMotionMaster()->MovePath(1, PATH_FROM_ENTRY, FORCED_MOVEMENT_RUN, true, 0.f, true);
        }
    }

    void HandleFelmystSpawnDelay()
    {
        uint32 timer = 0;
        switch (m_animationSpawnStage)
        {
            case 0:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                timer = 2500;
                break;
            case 1:
                m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                DoScriptText(SAY_INTRO, m_creature);
                timer = 1000;
                break;
            case 2:
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                timer = 7500;
                break;
            case 3:
                m_creature->GetMotionMaster()->MovePoint(PHASE_GROUND, 1489.438f, 651.615f, 44.65125f, FORCED_MOVEMENT_RUN);
                break;
            case 4:
                SetCombatScriptStatus(false);
                SetCombatMovement(true);
                SetMeleeEnabled(true);
                DoCastSpellIfCan(nullptr, SPELL_NOXIOUS_FUMES);
                AttackClosestEnemy();
                break;
        }
        ++m_animationSpawnStage;
        if (timer)
            ResetTimer(FELMYST_WAYPOINT_DELAY, timer);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_FELMYST, FAIL);
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (playerRef.getSource()->HasAura(SPELL_FOG_OF_CORRUPTION_CHARM))
                playerRef.getSource()->Suicide();

        m_creature->SetLevitate(true);
        m_creature->SetHover(true);
        m_creature->SetRespawnDelay(30, true);
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* who) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_FELMYST, IN_PROGRESS);

        if (!who) // should never happen but just a safeguard
            m_creature->GetMotionMaster()->MovePoint(POINT_AGGRO, 1483.703f, 623.2387f, 28.17801f, FORCED_MOVEMENT_RUN, false);
        else
        {
            Position pos;
            float targetDist = who->GetCombinedCombatReach(m_creature, false);
            who->GetNearPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius(), targetDist, who->GetAngle(m_creature));
            m_creature->GetMotionMaster()->MovePoint(POINT_AGGRO, pos, FORCED_MOVEMENT_RUN, 0.f, false);
        }
        SetCombatScriptStatus(true);
        m_creature->SetTarget(nullptr);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_FELMYST, DONE);
    }

    void CorpseRemoved(uint32& time) override
    {
        CombatAI::CorpseRemoved(time);
        if (m_instance && m_instance->GetData(TYPE_FELMYST) == DONE)
            if (m_creature->IsTemporarySummon())
                static_cast<TemporarySpawn*>(m_creature)->UnSummon();
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case PHASE_GROUND:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MovePath(1, PATH_FROM_ENTRY, FORCED_MOVEMENT_RUN, true, 0.f, true);
                break;
            case SUBPHASE_VAPOR:
                // prepare to move to flight trigger
                ResetTimer(FELMYST_DEMONIC_VAPOR, 5000);
                m_uiSubPhase = SUBPHASE_VAPOR;
                break;
            case SUBPHASE_BREATH_SIDE:
                ResetTimer(FELMYST_PREBREATH_DELAY, 5000);
                ResetTimer(FELMYST_DUMMY_NUKE, 1500);
                break;
            case SUBPHASE_BREATH_PREPARE:
                // move across the arena
                if (!m_instance)
                    return;

                // After the third breath land and resume phase 1
                if (m_uiCorruptionCount == 3)
                {
                    static Position landingPositions[] =
                    {
                        {1469.93f, 557.009f, 22.6317f, 0.f},
                        {1476.77f, 665.094f, 20.6423f, 0.f}
                    };
                    m_uiPhase = PHASE_TRANSITION; // TODO: figure out where to land
                    m_creature->GetMotionMaster()->MovePoint(PHASE_TRANSITION, landingPositions[m_bIsLeftSide], FORCED_MOVEMENT_RUN, 0.f, false);
                    return;
                }

                ++m_uiCorruptionCount;
                ResetTimer(FELMYST_BREATH_DELAY, 4000);
                DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                break;
            case SUBPHASE_BREATH_MOVE:
                if (!m_instance)
                    return;

                // remove speed aura
                m_creature->RemoveAurasDueToSpell(SPELL_SPEED_BURST);

                // switch sides
                m_bIsLeftSide = !m_bIsLeftSide;
                HandleSideMove();
                break;
            case PHASE_TRANSITION:
                HandleTransition();
                break;
            case POINT_AGGRO:
                m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                m_creature->SetLevitate(false);
                m_creature->SetHover(false);
                m_creature->GetMotionMaster()->MoveIdle();
                ResetTimer(FELMYST_WAYPOINT_DELAY, 3000);
                m_animationSpawnStage = 4; // should already be 3 but sanity assignment
                break;
        }
    }

    void HandleTransition()
    {
        // switch back to ground combat from flight transition
        m_uiPhase = PHASE_GROUND;
        SetDeathPrevention(false);
        m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
        m_creature->SetLevitate(false);
        m_creature->SetHover(false);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
        ResetTimer(FELMYST_TRANSITION_DELAY, 2500);
    }

    void HandleTransitionDelay()
    {
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        SetCombatScriptStatus(false);
        DoStartMovement(m_creature->GetVictim()); // TODO: Check if its delayed
        ResetCombatAction(FELMYST_PHASE_CHANGE, 60000);
        ResetCombatAction(FELMYST_ENCAPSULATE, urand(20000, 30000));
        ResetCombatAction(FELMYST_CORROSION, urand(13000, 16000));
        ResetCombatAction(FELMYST_CLEAVE, urand(2000, 5000));
        ResetCombatAction(FELMYST_GAS_NOVA, urand(16000, 20000));
    }

    void HandleDemonicVapor()
    {
        // After the second Demonic Vapor trial, start the breath phase
        if (m_uiDemonicVaporCount == 2)
        {
            if (!m_instance)
                return;

            m_creature->SetImmobilizedState(false);
            m_creature->SetHover(true);

            m_uiCorruptionCount = 0;
            m_bIsLeftSide = urand(0, 1) != 0;
            HandleSideMove();
        }
        else
        {
            if (m_creature->HasHoverAura())
            {
                m_creature->SetImmobilizedState(true);
                m_creature->SetHover(false);
                m_uiSubPhase = SUBPHASE_VAPOR;
            }
            DoCastSpellIfCan(nullptr, SPELL_SUMMON_VAPOR);
            ++m_uiDemonicVaporCount;
            ResetTimer(FELMYST_DEMONIC_VAPOR, 11000);
        }
    }

    void HandleLiftoff()
    {
        m_creature->GetMotionMaster()->MovePoint(SUBPHASE_VAPOR, Position(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 20.f, m_creature->GetOrientation()), FORCED_MOVEMENT_RUN, 0.f, false);
    }

    void HandleSideMove()
    {
        if (Creature* trigger = m_instance->GetSingleCreatureFromStorage(m_bIsLeftSide ? NPC_FLIGHT_TRIGGER_LEFT : NPC_FLIGHT_TRIGGER_RIGHT))
            m_creature->GetMotionMaster()->MovePoint(SUBPHASE_BREATH_SIDE, Position(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), GetFlightOrientation(m_bIsLeftSide)), FORCED_MOVEMENT_RUN, 0.f, false);
    }

    void HandlePreBreath()
    {
        m_uiSubPhase = SUBPHASE_BREATH_PREPARE;
        // Fly to trigger on the same side - choose a random index for the trigger
        m_uiCorruptionIndex = urand(0, 2);
        if (Creature* trigger = m_creature->GetMap()->GetCreature(m_instance->SelectFelmystFlightTrigger(m_bIsLeftSide, m_uiCorruptionIndex)))
            m_creature->GetMotionMaster()->MovePoint(SUBPHASE_BREATH_PREPARE, Position(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), GetFlightOrientation(m_bIsLeftSide)), FORCED_MOVEMENT_RUN, 0.f, false);
    }

    void HandleBreath()
    {
        if (m_instance)
            m_instance->DropFog();
        // Fly to the other side, casting the breath. Keep the same trigger index
        if (Creature* trigger = m_creature->GetMap()->GetCreature(m_instance->SelectFelmystFlightTrigger(!m_bIsLeftSide, m_uiCorruptionIndex)))
        {
            DoCastSpellIfCan(nullptr, SPELL_SPEED_BURST, CAST_TRIGGERED);
            static const uint32 strafeSpells[] = { SPELL_TRIGGER_TOP_STRAFE , SPELL_TRIGGER_MIDDLE_STRAFE, SPELL_TRIGGER_BOTTOM_STRAFE };
            DoCastSpellIfCan(nullptr, strafeSpells[m_uiCorruptionIndex], CAST_TRIGGERED);
            // the 50.f speed is confirmed even tho they use speed burst
            m_creature->GetMotionMaster()->MovePoint(SUBPHASE_BREATH_MOVE, Position(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), GetFlightOrientation(!m_bIsLeftSide)), FORCED_MOVEMENT_RUN, 50.f, false);
        }
    }

    void HandleDummyNuke()
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DUMMY_NUKE, SELECT_FLAG_PLAYER))
            DoCastSpellIfCan(target, SPELL_DUMMY_NUKE);
    }

    void OnSpellCooldownAdded(SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ENCAPSULATE_CHANNEL)
            ResetCombatAction(FELMYST_ENCAPSULATE, urand(30000, 45000));
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case FELMYST_BERSERK:
                if (DoCastSpellIfCan(nullptr, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DisableCombatAction(action);
                }
                break;
            case FELMYST_PHASE_CHANGE:
            {
                if (!CanExecuteCombatAction() || !m_creature->IsSpellReady(SPELL_ENCAPSULATE_CHANNEL))
                    return;

                m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);

                SetCombatMovement(false);
                m_creature->SetLevitate(true);
                m_creature->SetHover(true);
                DoScriptText(SAY_TAKEOFF, m_creature);
                SetMeleeEnabled(false);
                m_creature->SetTarget(nullptr);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_MISC_FLAGS, UNIT_BYTE1_FLAG_FLY_ANIM);
                m_creature->SendForcedObjectUpdate();
                ResetTimer(FELMYST_LIFTOFF_DELAY, 5000);
                SetDeathPrevention(true);
                SetCombatScriptStatus(true);

                m_uiPhase = PHASE_TRANSITION;
                m_uiSubPhase = SUBPHASE_VAPOR;
                m_uiDemonicVaporCount = 0;
                break;
            }
            case FELMYST_ENCAPSULATE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_ENCAPSULATE_CHANNEL, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IMMUNE))
                    DoCastSpellIfCan(target, SPELL_ENCAPSULATE_CHANNEL);
                break;
            case FELMYST_CORROSION:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CORROSION) == CAST_OK)
                {
                    DoScriptText(SAY_BREATH, m_creature);
                    ResetCombatAction(action, urand(32000, 40000));
                }
                break;
            case FELMYST_CLEAVE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 29000));
                break;
            case FELMYST_GAS_NOVA:
                if (DoCastSpellIfCan(nullptr, SPELL_GAS_NOVA) == CAST_OK)
                    ResetCombatAction(action, urand(22000, 27000));
                break;
        }
    }
};

/*######
## npc_demonic_vapor
######*/

struct npc_demonic_vaporAI : public CombatAI
{
    npc_demonic_vaporAI(Creature* creature) : CombatAI(creature, 0)
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetDeathPrevention(true);
        AddCustomAction(1, 1500u, [&]()
        {
            SetReactState(REACT_AGGRESSIVE);
            m_creature->SetInCombatWithZone();
            SetCombatMovement(true);
            Unit* attackTarget = nullptr;
            if (Unit* target = m_creature->GetSpawner())
                attackTarget = target;
            else
                attackTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
            if (attackTarget)
            {
                m_creature->AddThreat(attackTarget, 1000000.f);
                AttackStart(attackTarget);
            }
        });
    }

    void JustRespawned()
    {
        CombatAI::JustRespawned();
        m_creature->CastSpell(nullptr, SPELL_DEMONIC_VAPOR_PER, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_VAPOR_SPAWN_TRIGGER, TRIGGERED_OLD_TRIGGERED);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_DEMONIC_VAPOR_TRAIL)
        {
            summoned->AI()->SetCombatMovement(false);
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->CastSpell(nullptr, SPELL_DEMONIC_VAPOR, TRIGGERED_OLD_TRIGGERED);
            summoned->ForcedDespawn(20000);
        }
    }
};

struct SoulSever : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || !unitTarget->HasAura(SPELL_FOG_OF_CORRUPTION_CHARM))
            return;

        // kill all charmed targets
        unitTarget->CastSpell(nullptr, SPELL_SOUL_SEVER_INSTAKILL, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FogOfCorruption : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || spell->GetCaster()->GetTypeId() != TYPEID_PLAYER)
            return;

        unitTarget->CastSpell(spell->GetCaster(), spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
    }
};

struct FogOfCorruptionCharm : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_FOG_OF_CORRUPTION_BUFF, TRIGGERED_OLD_TRIGGERED);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FOG_OF_CORRUPTION_BUFF);
    }
};

struct TriggerStrafe : public SpellScript, public AuraScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetFilteringScheme(EFFECT_INDEX_0, false, SCHEME_CLOSEST);
        spell->SetFilteringScheme(EFFECT_INDEX_0, true, SCHEME_CLOSEST);
        if (spell->m_spellInfo->Id == SPELL_STRAFE_TOP)
            spell->SetMaxAffectedTargets(3);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster() == target || target->HasAura(SPELL_FOG_CORRUPTION))
            return false;
        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
        {
            std::map<uint32, uint32> strafeSpells =
            {
                {SPELL_STRAFE_TOP, SPELL_STRAFE_TOP},
                {SPELL_TRIGGER_TOP_STRAFE, SPELL_STRAFE_TOP},
                {SPELL_TRIGGER_MIDDLE_STRAFE, SPELL_STRAFE_MIDDLE},
                {SPELL_TRIGGER_BOTTOM_STRAFE, SPELL_STRAFE_BOTTOM},
                {SPELL_STRAFE_MIDDLE, SPELL_STRAFE_MIDDLE},
                {SPELL_STRAFE_BOTTOM, SPELL_STRAFE_BOTTOM},
            };
            Unit* target = aura->GetTarget();
            target->CastSpell(nullptr, SPELL_FOG_CORRUPTION, TRIGGERED_OLD_TRIGGERED);
            target->CastSpell(nullptr, strafeSpells[aura->GetSpellProto()->Id], TRIGGERED_OLD_TRIGGERED);
        }
    }
};

struct Encapsulate : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_1)
            return;

        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_ENCAPSULATE, TRIGGERED_NONE);
        else
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_ENCAPSULATE);
    }
};

void AddSC_boss_felmyst()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_felmyst";
    pNewScript->GetAI = &GetNewAIInstance<boss_felmystAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_demonic_vapor";
    pNewScript->GetAI = &GetNewAIInstance<npc_demonic_vaporAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SoulSever>("spell_soul_sever");
    RegisterSpellScript<FogOfCorruption>("spell_fog_of_corruption");
    RegisterSpellScript<FogOfCorruptionCharm>("spell_fog_of_corruption_charm");
    RegisterSpellScript<TriggerStrafe>("spell_trigger_strafe");
    RegisterSpellScript<Encapsulate>("spell_encapsulate");
}
