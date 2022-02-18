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
SDName: Boss_Zuljin
SD%Complete: 90
SDComment: Timers should be improved.
SDCategory: Zul'Aman
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "zulaman.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    // SAY_INTRO                       = -1568056,
    SAY_AGGRO                       = -1568057,
    SAY_BEAR_TRANSFORM              = -1568058,
    SAY_EAGLE_TRANSFORM             = -1568059,
    SAY_LYNX_TRANSFORM              = -1568060,
    SAY_DRAGONHAWK_TRANSFORM        = -1568061,
    SAY_FIRE_BREATH                 = -1568062,
    SAY_BERSERK                     = -1568053,
    SAY_KILL1                       = -1568064,
    SAY_KILL2                       = -1568065,
    SAY_DEATH                       = -1568066,

    EMOTE_BEAR_SPIRIT               = -1568082,
    EMOTE_EAGLE_SPIRIT              = -1568083,
    EMOTE_LYNX_SPIRIT               = -1568084,
    EMOTE_DRAGONHAWK_SPIRIT         = -1568085,
    EMOTE_FADE_AWAY                 = -1568121,

    // Troll Form
    SPELL_WHIRLWIND                 = 17207,
    SPELL_GRIEVOUS_THROW            = 43093,                // removes debuff after full healed

    // Bear Form
    SPELL_CREEPING_PARALYSIS        = 43095,                // should cast on the whole raid
    SPELL_OVERPOWER                 = 43456,                // use after melee attack dodged

    // Eagle Form
    SPELL_ENERGY_STORM              = 43983,                // enemy area aura, trigger 42577 on vortexes which cast 43137 on targets
    SPELL_SUMMON_CYCLONE            = 43112,                // summon four feather vortex
    NPC_FEATHER_VORTEX              = 24136,                // ToDo: script via ACID
    SPELL_CYCLONE_VISUAL            = 43119,                // trigger 43147 visual
    SPELL_CYCLONE_PASSIVE           = 43120,                // trigger 43121 (4y aoe) every second
    SPELL_CYCLONE                   = 43121,
    SPELL_BALL_OF_ENERGY            = 43457,
    SPELL_DREAM_FOG                 = 24780,

    // Lynx Form
    SPELL_CLAW_RAGE                 = 42583,                // Charges a random target and applies dummy effect 43149 on it
    SPELL_CLAW_RAGE_TRIGGER         = 43149,
    SPELL_LYNX_RUSH                 = 43152,                // Charges 9 targets in a row - Dummy effect should apply 43153
    SPELL_LYNX_RUSH_CHARGE          = 43153,

    // Dragonhawk Form
    SPELL_FLAME_WHIRL               = 43213,                // trigger two spells
    SPELL_FLAME_BREATH              = 43215,
    SPELL_SUMMON_PILLAR             = 43216,                // summon 24187
    NPC_COLUMN_OF_FIRE              = 24187,
    SPELL_PILLAR_TRIGGER            = 43218,                // trigger 43217
    SPELL_BIRTH                     = 40031,

    // Cosmetic
    SPELL_SPIRIT_DRAINED            = 42520,
    SPELL_SPIRIT_DRAIN              = 42542,

    // Transforms
    SPELL_SHAPE_OF_THE_BEAR         = 42594,
    SPELL_SHAPE_OF_THE_EAGLE        = 42606,
    SPELL_SHAPE_OF_THE_LYNX         = 42607,
    SPELL_SHAPE_OF_THE_DRAGONHAWK   = 42608,

    SPELL_BERSERK                   = 45078,                // Berserk timer or existance is unk

    SPELL_COSMETIC_INCINERATE_BLUE  = 42567,

    MAX_VORTEXES                    = 4,
    MAX_LYNX_RUSH                   = 10,
    POINT_ID_CENTER                 = 0,

    PHASE_TROLL                     = 0,
    PHASE_BEAR                      = 1,
    PHASE_EAGLE                     = 2,
    PHASE_LYNX                      = 3,
    PHASE_DRAGONHAWK                = 4,
};

struct BossPhase
{
    uint32 spiritSpellId;
    int32 yellId, emoteId;
    uint32 spiritId;
    uint8 phase;
};

static const BossPhase aZuljinPhases[] =
{
    {},
    {SPELL_SHAPE_OF_THE_BEAR,       SAY_BEAR_TRANSFORM,       EMOTE_BEAR_SPIRIT,       NPC_BEAR_SPIRIT,       PHASE_BEAR},
    {SPELL_SHAPE_OF_THE_EAGLE,      SAY_EAGLE_TRANSFORM,      EMOTE_EAGLE_SPIRIT,      NPC_EAGLE_SPIRIT,      PHASE_EAGLE},
    {SPELL_SHAPE_OF_THE_LYNX,       SAY_LYNX_TRANSFORM,       EMOTE_LYNX_SPIRIT,       NPC_LYNX_SPIRIT,       PHASE_LYNX},
    {SPELL_SHAPE_OF_THE_DRAGONHAWK, SAY_DRAGONHAWK_TRANSFORM, EMOTE_DRAGONHAWK_SPIRIT, NPC_DRAGONHAWK_SPIRIT, PHASE_DRAGONHAWK}
};

// coords for going for changing form
static const float fZuljinMoveLoc[3] = { 120.172f, 706.444f, 45.11137f};

enum ZuljinActions
{
    ZULJIN_ACTION_LYNX_RUSH_RE_CHARGE,
    ZULJIN_ACTION_PHASE_TRANSITION,
    ZULJIN_ACTION_WHIRLWIND,
    ZULJIN_ACTION_GRIEVOUS_THROW,
    ZULJIN_ACTION_PARALYSIS,
    ZULJIN_ACTION_OVERPOWER,
    ZULJIN_ACTION_CLAW_RAGE,
    ZULJIN_ACTION_LYNX_RUSH,
    ZULJIN_ACTION_FLAME_WHIRL,
    ZULJIN_ACTION_SUMMON_PILLAR,
    ZULJIN_ACTION_FLAME_BREATH,
    ZULJIN_ACTION_MAX,
    ZULJIN_SPIRIT_ACQUIRED,
    ZULJIN_FIRE_WALL_DELAY,
};

struct boss_zuljinAI : public CombatAI
{
    boss_zuljinAI(Creature* creature) : CombatAI(creature, ZULJIN_ACTION_MAX), m_instance(static_cast<instance_zulaman*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(ZULJIN_ACTION_LYNX_RUSH_RE_CHARGE, false);
        AddTimerlessCombatAction(ZULJIN_ACTION_PHASE_TRANSITION, true);
        AddCombatAction(ZULJIN_ACTION_WHIRLWIND, 7000u);
        AddCombatAction(ZULJIN_ACTION_GRIEVOUS_THROW, 8000u);
        AddCombatAction(ZULJIN_ACTION_PARALYSIS, true);
        AddCombatAction(ZULJIN_ACTION_OVERPOWER, true);
        AddCombatAction(ZULJIN_ACTION_CLAW_RAGE, true);
        AddCombatAction(ZULJIN_ACTION_LYNX_RUSH, true);
        AddCombatAction(ZULJIN_ACTION_FLAME_WHIRL, true);
        AddCombatAction(ZULJIN_ACTION_SUMMON_PILLAR, true);
        AddCombatAction(ZULJIN_ACTION_FLAME_BREATH, true);
        AddCustomAction(ZULJIN_SPIRIT_ACQUIRED, true, [&]()
        {
            SpiritAcquired();
        });
        AddCustomAction(ZULJIN_FIRE_WALL_DELAY, true, [&]()
        {
            if (m_creature->IsInCombat() && !m_creature->GetCombatManager().IsEvadingHome())
                m_instance->DoUseDoorOrButton(GO_FIRE_DOOR);
        });
        m_creature->SetWalk(true);
        m_creature->GetCombatManager().SetLeashingCheck([&](Unit*, float x, float y, float z)
            {
                return y > 734.0f;
            });
        AddOnKillText(SAY_KILL1, SAY_KILL2);
    }

    instance_zulaman* m_instance;

    uint8 m_phase;
    uint8 m_healthCheck;

    uint8 m_lynxRushCount;

    GuidVector m_summons;

    void Reset() override
    {
        CombatAI::Reset();
        m_healthCheck         = 80;
        m_phase               = PHASE_TROLL;

        m_lynxRushCount       = 0;

        SetEquipmentSlots(true);

        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ZULJIN, IN_PROGRESS);

        ResetTimer(ZULJIN_FIRE_WALL_DELAY, 5000);
    }

    void EnterEvadeMode() override
    {
        m_creature->SetStunned(false);

        if (m_instance)
            m_instance->SetData(TYPE_ZULJIN, FAIL);

        // Despawn all feather vortexes
        DoDespawnVortexes();

        // Reset all spirits
        for (const auto& aZuljinPhase : aZuljinPhases)
            if (Creature* spirit = m_instance->GetSingleCreatureFromStorage(aZuljinPhase.spiritId))
                spirit->RemoveAurasDueToSpell(SPELL_SPIRIT_DRAINED);

        m_creature->SetHealthPercent(100.f); // TODO: Remove this hack once evade issues are resolved

        CombatAI::EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        DoCastSpellIfCan(nullptr, SPELL_COSMETIC_INCINERATE_BLUE, CAST_TRIGGERED);

        if (!m_instance)
            return;

        m_instance->SetData(TYPE_ZULJIN, DONE);
    }

    // Function to handle the Feather Vortexes despawn on phase change
    void DoDespawnVortexes()
    {
        DespawnGuids(m_summons);
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_FEATHER_VORTEX:
                m_summons.push_back(summoned->GetObjectGuid());
                summoned->CastSpell(nullptr, SPELL_DREAM_FOG, TRIGGERED_OLD_TRIGGERED);
                break;
            case NPC_COLUMN_OF_FIRE:
                summoned->AI()->SetCombatMovement(false);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                summoned->CastSpell(nullptr, SPELL_PILLAR_TRIGGER, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(nullptr, SPELL_BIRTH, TRIGGERED_OLD_TRIGGERED);
                break;
        }
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType == POINT_MOTION_TYPE && pointId == POINT_ID_CENTER)
        {
            // increment phase
            if (m_phase == PHASE_TROLL)
                m_phase = PHASE_BEAR;
            else
                ++m_phase;

            // drain the spirit
            if (Creature* spirit = m_instance->GetSingleCreatureFromStorage(aZuljinPhases[m_phase].spiritId))
            {
                float angle = m_creature->GetAngle(spirit);
                m_creature->SetOrientation(angle);
                m_creature->SetFacingTo(angle);
                spirit->CastSpell(m_creature, SPELL_SPIRIT_DRAIN, TRIGGERED_NONE);
            }
        }
        else if (motionType == EFFECT_MOTION_TYPE && pointId == SPELL_LYNX_RUSH_CHARGE)
            SetActionReadyStatus(ZULJIN_ACTION_LYNX_RUSH_RE_CHARGE, true);
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell) override
    {
        if (spell->Id == SPELL_SPIRIT_DRAIN)
        {
            DoScriptText(aZuljinPhases[m_phase].emoteId, m_creature);
            ResetTimer(ZULJIN_SPIRIT_ACQUIRED, 3000);
        }
        else if (spell->Id == SPELL_SHAPE_OF_THE_BEAR || spell->Id == SPELL_SHAPE_OF_THE_EAGLE || spell->Id == SPELL_SHAPE_OF_THE_LYNX || spell->Id == SPELL_SHAPE_OF_THE_DRAGONHAWK)
        {
            // in eagle phase we don't move
            if (m_phase != PHASE_EAGLE)
            {
                SetCombatMovement(true);
                SetMeleeEnabled(true);
                if (m_creature->GetVictim())
                {
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                }
            }
            // In Eagle phase we just cast Energy storm and summon 4 Feather cyclones; Boss doesn't move in this phase
            else
            {
                DoCastSpellIfCan(nullptr, SPELL_ENERGY_STORM, CAST_TRIGGERED);

                // summon 4 vortexes
                DoCastSpellIfCan(nullptr, SPELL_SUMMON_CYCLONE, CAST_TRIGGERED);
                m_creature->SetStunned(true);
            }
            HandlePhaseTransition();
            DoResetThreat();
            SetCombatScriptStatus(false);
        }
    }

    void SpiritAcquired()
    {
        DoCastSpellIfCan(m_creature, aZuljinPhases[m_phase].spiritSpellId);
    }

    void DamageTaken(Unit* dealer, uint32& damage, DamageEffectType damagetype, SpellEntry const* spellInfo) override
    {
        CombatAI::DamageTaken(dealer, damage, damagetype, spellInfo);
        if (m_creature->IsStunned() && ((m_creature->GetHealth() - damage) * 100.0f) / m_creature->GetMaxHealth() < 40.f)
            ExecutePhaseTransition();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            SetCombatScriptStatus(bool(miscValue));
    }

    void ExecutePhaseTransition()
    {
        // Despawn vortexes and remvoe the energy storm after eagle phase is complete
        if (m_phase == PHASE_EAGLE)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_STORM);
            DoDespawnVortexes();
            m_creature->SetStunned(false);
        }

        m_healthCheck -= 20;
        SetCombatScriptStatus(true);
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        m_creature->SetTarget(nullptr);
        m_creature->GetMotionMaster()->MovePoint(POINT_ID_CENTER, fZuljinMoveLoc[0], fZuljinMoveLoc[1], fZuljinMoveLoc[2], FORCED_MOVEMENT_RUN);
        DoScriptText(aZuljinPhases[m_phase + 1].yellId, m_creature);

        // don't do this after troll phase
        if (m_phase != PHASE_TROLL)
        {
            if (m_creature->HasAura(aZuljinPhases[m_phase].spiritSpellId))
                m_creature->RemoveAurasDueToSpell(aZuljinPhases[m_phase].spiritSpellId);

            // drain spirit
            if (Creature* spirit = m_instance->GetSingleCreatureFromStorage(aZuljinPhases[m_phase].spiritId))
            {
                spirit->InterruptNonMeleeSpells(false);
                spirit->CastSpell(nullptr, SPELL_SPIRIT_DRAINED, TRIGGERED_NONE);
            }
        }
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellEntry) override
    {
        if (spellEntry->Id == SPELL_COSMETIC_INCINERATE_BLUE && target->GetTypeId() == TYPEID_UNIT)
        {
            DoScriptText(EMOTE_FADE_AWAY, target);
            target->RemoveAurasDueToSpell(SPELL_SPIRIT_DRAINED);
            m_instance->StartSpiritTimer();
        }
    }

    uint32 GetInitialTimerForAction(uint32 action)
    {
        switch (action)
        {
            case ZULJIN_ACTION_PARALYSIS: return 0;
            case ZULJIN_ACTION_OVERPOWER: return 5000;
            case ZULJIN_ACTION_CLAW_RAGE: return 5000;
            case ZULJIN_ACTION_LYNX_RUSH: return 15000;
            case ZULJIN_ACTION_FLAME_WHIRL: return 7000;
            case ZULJIN_ACTION_SUMMON_PILLAR: return 7000;
            case ZULJIN_ACTION_FLAME_BREATH: return 15000;
            default: return 0;
        }
    }

    void HandlePhaseTransition()
    {
        switch (m_phase)
        {
            case PHASE_TROLL:
                break;
            case PHASE_BEAR:
                SetEquipmentSlots(false, 0, 0, 0);
                DisableCombatAction(ZULJIN_ACTION_WHIRLWIND);
                DisableCombatAction(ZULJIN_ACTION_GRIEVOUS_THROW);
                ResetCombatAction(ZULJIN_ACTION_PARALYSIS, GetInitialTimerForAction(ZULJIN_ACTION_PARALYSIS));
                ResetCombatAction(ZULJIN_ACTION_OVERPOWER, GetInitialTimerForAction(ZULJIN_ACTION_OVERPOWER));
                break;
            case PHASE_EAGLE:
                DisableCombatAction(ZULJIN_ACTION_PARALYSIS);
                DisableCombatAction(ZULJIN_ACTION_OVERPOWER);
                break;
            case PHASE_LYNX:
                ResetCombatAction(ZULJIN_ACTION_CLAW_RAGE, GetInitialTimerForAction(ZULJIN_ACTION_CLAW_RAGE));
                ResetCombatAction(ZULJIN_ACTION_LYNX_RUSH, GetInitialTimerForAction(ZULJIN_ACTION_LYNX_RUSH));
                break;
            case PHASE_DRAGONHAWK:
                DisableCombatAction(ZULJIN_ACTION_CLAW_RAGE);
                DisableCombatAction(ZULJIN_ACTION_LYNX_RUSH);
                ResetCombatAction(ZULJIN_ACTION_FLAME_WHIRL, GetInitialTimerForAction(ZULJIN_ACTION_FLAME_WHIRL));
                ResetCombatAction(ZULJIN_ACTION_SUMMON_PILLAR, GetInitialTimerForAction(ZULJIN_ACTION_SUMMON_PILLAR));
                ResetCombatAction(ZULJIN_ACTION_FLAME_BREATH, GetInitialTimerForAction(ZULJIN_ACTION_FLAME_BREATH));
                break;
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ZULJIN_ACTION_LYNX_RUSH_RE_CHARGE:
            {
                ++m_lynxRushCount;
                if (m_lynxRushCount != MAX_LYNX_RUSH)
                {
                    SpellCastResult result = m_creature->CastSpell(nullptr, SPELL_LYNX_RUSH, TRIGGERED_IGNORE_COOLDOWNS);
                    if (result != SPELL_CAST_OK)
                        m_creature->RemoveAurasDueToSpell(SPELL_LYNX_RUSH);
                }
                else
                    m_creature->RemoveAurasDueToSpell(SPELL_LYNX_RUSH);
                SetActionReadyStatus(ZULJIN_ACTION_LYNX_RUSH_RE_CHARGE, false);
                return;
            }
            case ZULJIN_ACTION_PHASE_TRANSITION:
            {
                if (m_creature->HasAura(SPELL_LYNX_RUSH) || m_creature->HasAura(SPELL_CLAW_RAGE_TRIGGER)) // do not allow phase transition during lynx rush/claw rage
                    return;

                if (m_creature->GetHealthPercent() < m_healthCheck)
                    ExecutePhaseTransition();
                return;
            }
            case ZULJIN_ACTION_WHIRLWIND:
                if (DoCastSpellIfCan(nullptr, SPELL_WHIRLWIND) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                return;
            case ZULJIN_ACTION_GRIEVOUS_THROW:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_AURA))
                    if (DoCastSpellIfCan(target, SPELL_GRIEVOUS_THROW) == CAST_OK)
                        ResetCombatAction(action, 10000);
                return;
            case ZULJIN_ACTION_PARALYSIS:
                if (DoCastSpellIfCan(nullptr, SPELL_CREEPING_PARALYSIS) == CAST_OK)
                    ResetCombatAction(action, 27000);
                return;
            case ZULJIN_ACTION_OVERPOWER:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_OVERPOWER) == CAST_OK)
                    ResetCombatAction(action, urand(12000, 16000));
                return;
            case ZULJIN_ACTION_CLAW_RAGE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_CLAW_RAGE, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_CLAW_RAGE) == CAST_OK)
                        ResetCombatAction(action, urand(15000, 20000));
                return;
            case ZULJIN_ACTION_LYNX_RUSH:
                if (DoCastSpellIfCan(nullptr, SPELL_LYNX_RUSH) == CAST_OK)
                {
                    m_lynxRushCount = 0;
                    ResetCombatAction(action, urand(20000, 25000));
                }
                return;
            case ZULJIN_ACTION_FLAME_WHIRL:
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_WHIRL) == CAST_OK)
                    ResetCombatAction(action, 15000);
                return;
            case ZULJIN_ACTION_SUMMON_PILLAR:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_SUMMON_PILLAR, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_SUMMON_PILLAR) == CAST_OK)
                        ResetCombatAction(action, 10000);
                return;
            case ZULJIN_ACTION_FLAME_BREATH:
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_BREATH) == CAST_OK)
                    ResetCombatAction(action, 15000);
                return;
        }
    }
};

/*######
## npc_feather_vortex
######*/

struct npc_feather_vortexAI : public ScriptedAI
{
    npc_feather_vortexAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetMeleeEnabled(false);
        AddCustomAction(1, 1500u, [&]()
        {
            StartAttacking();
        });
        Reset();
    }

    ScriptedInstance* m_instance;
    SelectAttackingTargetParams m_params;

    void Reset() override { }

    void JustRespawned() override
    {
        m_creature->SetInCombatWithZone();
    }

    void PickNewTarget()
    {
        if (Creature* zuljin = m_instance->GetSingleCreatureFromStorage(NPC_ZULJIN))
        {
            // Change target on player hit
            if (m_creature->GetVictim())
                m_params.skip.guid = m_creature->GetVictim()->GetObjectGuid();
            if (Unit* target = zuljin->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_CUSTOM))
            {
                DoResetThreat();
                m_creature->AddThreat(target, 1000000.f);
                AttackStart(target);
            }
        }
    }

    void StartAttacking()
    {
        m_creature->CastSpell(nullptr, SPELL_CYCLONE_VISUAL, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_CYCLONE_PASSIVE, TRIGGERED_OLD_TRIGGERED);
        m_creature->CastSpell(nullptr, SPELL_BALL_OF_ENERGY, TRIGGERED_OLD_TRIGGERED);
        m_creature->SetInCombatWithZone();
        // Attack random target
        if (Unit * target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
        {
            m_creature->AddThreat(target, 1000000.f);
            AttackStart(target);
        }
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spellEntry) override
    {
        if (spellEntry->Id == SPELL_CYCLONE && target == m_creature->GetVictim())
            PickNewTarget();
    }
};

void AddSC_boss_zuljin()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_zuljin";
    pNewScript->GetAI = &GetNewAIInstance<boss_zuljinAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_feather_vortex";
    pNewScript->GetAI = &GetNewAIInstance<npc_feather_vortexAI>;
    pNewScript->RegisterSelf();
}
