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
SDName: Boss_Felblood_Kaelthas
SD%Complete: 90
SDComment: Minor adjustments required; Timers.
SDCategory: Magisters' Terrace
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "magisters_terrace.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_INTRO_1                 = -1585023,
    SAY_INTRO_2                 = -1585030,
    SAY_PHOENIX                 = -1585024,
    SAY_FLAMESTRIKE             = -1585025,
    SAY_GRAVITY_LAPSE           = -1585026,
    SAY_TIRED                   = -1585027,
    SAY_RECAST_GRAVITY          = -1585028,
    SAY_DEATH                   = -1585029,
    SAY_PYROBLAST               = -1550044,                 // reuse of text from TK

    // Phase 1 spells
    SPELL_FIREBALL              = 44189,                    // Deals 2700-3300 damage at current target
    SPELL_FIREBALL_H            = 46164,                    //       4950-6050
    SPELL_PHOENIX               = 44194,                    // Summons a phoenix
    SPELL_FLAME_STRIKE          = 44192,                    // Summons the trigger + animation (projectile)
    SPELL_FLAME_STRIKE_H        = 46162,
    SPELL_SHOCK_BARRIER         = 46165,                    // Heroic only; 10k damage shield, followed by Pyroblast
    SPELL_PYROBLAST             = 36819,                    // Heroic only; 45-55k fire damage

    // Phase 2 spells
    SPELL_GRAVITY_LAPSE         = 44224,                    // Cast at the beginning of every Gravity Lapse
    SPELL_GRAVITY_LAPSE_VISUAL  = 44251,                    // Channeled; blue beam animation to every enemy in range - when removed the Gravity Lapse auras are removed from players
    SPELL_TELEPORT_CENTER       = 44218,                    // Teleport the boss in the center. Requires DB entry in spell_target_position.
    SPELL_GRAVITY_LAPSE_FLY     = 44227,                    // Hastens flyspeed and allows flying for 1 minute. Requires aura stacking exception for 44226.
    SPELL_GRAVITY_LAPSE_DOT_H   = 44226,                    // Knocks up in the air and applies a 300 DPS DoT.
    SPELL_GRAVITY_LAPSE_DOT_N   = 49887,
    SPELL_ARCANE_SPHERE_SUMMON  = 44265,                    // Summons 1 arcane sphere
    SPELL_POWER_FEEDBACK        = 44233,                    // Stuns him, making him take 50% more damage for 10 seconds. Cast after Gravity Lapse
    SPELL_POWER_FEEDBACK_H      = 47109,
    SPELL_CLEAR_FLIGHT          = 44232,

    // Outro spells
    SPELL_EMOTE_TALK_EXCLAMATION = 48348,
    SPELL_EMOTE_POINT           = 48349,
    SPELL_EMOTE_ROAR            = 48350,
    SPELL_SUICIDE               = 3617,

    // Summoned spells
    SPELL_ARCANE_SPHERE_PASSIVE = 44263,                    // Passive auras on Arcane Spheres
    SPELL_FLAME_STRIKE_DUMMY    = 44191,                    // Flamestrike indicator before the damage

    // Summoned creatures
    NPC_FLAME_STRIKE_TRIGGER    = 24666,
    NPC_PHOENIX                 = 24674,
    NPC_PHOENIX_EGG             = 24675,
    NPC_ARCANE_SPHERE           = 24708,

    MAX_ARCANE_SPHERES          = 3,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_INTRO_1,         NPC_KAELTHAS, 16000},
    {EMOTE_ONESHOT_LAUGH, 0,            2000},
    {EMOTE_STATE_TALK,    0,            2000},
    {SAY_INTRO_2,         NPC_KAELTHAS, 16000},
    {NPC_PHOENIX,         0,            0},
    {SAY_DEATH,           NPC_KAELTHAS, 4000},
    {EMOTE_ONESHOT_POINT, 0,            5000},
    {EMOTE_ONESHOT_ROAR,  0,            3000},
    {NPC_PHOENIX_EGG,     0,            0},
    {0, 0, 0},
};

/*######
## boss_felblood_kaelthas
######*/

enum FelbloodKaelthasActions
{
    KAEL_ACTION_ENERGY_FEEDBACK,
    KAEL_ACTION_GRAVITY_LAPSE,
    KAEL_ACTION_PYROBLAST,
    KAEL_ACTION_PHASE_TRANSITION,
    KAEL_ACTION_SHOCK_BARRIER,
    KAEL_ACTION_FLAMESTRIKE,
    KAEL_ACTION_PHOENIX,
    KAEL_ACTION_FIREBALL,
    KAEL_ACTION_MAX,
    KAEL_INTRO,
    KAEL_GRAVITY_LAPSE_SCRIPT,
    KAEL_OUTRO,
};

struct boss_felblood_kaelthasAI : public CombatAI
{
    boss_felblood_kaelthasAI(Creature* creature) : CombatAI(creature, KAEL_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())), m_isRegularMode(creature->GetMap()->IsRegularDifficulty()), m_introStarted(false)
    {
        AddTimerlessCombatAction(KAEL_ACTION_ENERGY_FEEDBACK, false);
        AddTimerlessCombatAction(KAEL_ACTION_GRAVITY_LAPSE, false);
        AddTimerlessCombatAction(KAEL_ACTION_PHASE_TRANSITION, true);
        AddCombatAction(KAEL_ACTION_PYROBLAST, true);
        if (!m_isRegularMode)
            AddCombatAction(KAEL_ACTION_SHOCK_BARRIER, 60000u);
        else
            AddCombatAction(KAEL_ACTION_SHOCK_BARRIER, true);
        AddCombatAction(KAEL_ACTION_FLAMESTRIKE, 25000u);
        AddCombatAction(KAEL_ACTION_PHOENIX, 10000u);
        AddCombatAction(KAEL_ACTION_FIREBALL, 0u);
        AddCustomAction(KAEL_INTRO, true, [&]()
        {
            HandleIntro();
        });
        AddCustomAction(KAEL_GRAVITY_LAPSE_SCRIPT, true, [&]()
        {
            HandleGravityLapse();
        });
        AddCustomAction(KAEL_OUTRO, true, [&]()
        {
            HandleOutro();
        });
        SetDeathPrevention(true);
        SetRangedMode(true, 20.f, TYPE_PROXIMITY);
        AddMainSpell(m_isRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H);
        if (m_instance)
        {
            m_creature->GetCombatManager().SetLeashingCheck([](Unit* unit, float /*x*/, float /*y*/, float /*z*/)
            {
                return static_cast<ScriptedInstance*>(unit->GetInstanceData())->GetPlayerInMap(true, false) == nullptr;
            });
        }
        Reset();
    }

    ScriptedInstance* m_instance;
    bool m_isRegularMode;

    uint32 m_gravityLapseStage;

    bool m_isFirstPhase;
    bool m_firstGravityLapse;
    bool m_introStarted;

    uint32 m_introStage;
    uint32 m_outroStage;

    GuidVector m_spawns;

    void Reset() override
    {
        CombatAI::Reset();

        if (!m_introStarted)
            SetReactState(REACT_PASSIVE);
        else
            SetReactState(REACT_AGGRESSIVE);
        m_gravityLapseStage   = 0;

        m_firstGravityLapse    = true;
        m_isFirstPhase         = true;

        m_introStage = 0;
        m_outroStage = 0;

        m_attackDistance = 20.0f;

        SetCombatMovement(true);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(true);

        DespawnGuids(m_spawns);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, IN_PROGRESS);
    }

    void JustPreventedDeath(Unit* /*attacker*/) override
    {
        m_creature->HandleEmote(EMOTE_STATE_TALK);
        DoFakeDeath();
        HandleOutro();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_introStarted && who->GetTypeId() == TYPEID_PLAYER && !static_cast<Player*>(who)->IsGameMaster() &&
            m_creature->IsWithinDistInMap(who, 55.0) && m_creature->IsWithinLOSInMap(who))
        {
            m_introStarted = true;
            HandleIntro();
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && bool(miscValue) && !m_outroStage) // Gravity Lapse end
            SetActionReadyStatus(KAEL_ACTION_ENERGY_FEEDBACK, true);
    }

    void EnterEvadeMode() override
    {
        DespawnGuids(m_spawns);

        if (m_instance)
            m_instance->SetData(TYPE_KAELTHAS, FAIL);

        m_creature->SetRespawnDelay(30, true);
        m_creature->ForcedDespawn();
    }

    void HandleIntro()
    {
        uint32 timer = 0;
        switch (m_introStage)
        {
            case 0:
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                timer = 5500;
                break;
            case 1:
                m_creature->HandleEmote(EMOTE_STATE_TALK);
                DoScriptText(SAY_INTRO_1, m_creature);
                timer = 15000;
                break;
            case 2:
                m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH_NOSHEATHE);
                timer = 5000;
                break;
            case 3:
                DoScriptText(SAY_INTRO_2, m_creature);
                timer = 15500;
                break;
            case 4:
                m_creature->HandleEmote(0);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                SetReactState(REACT_AGGRESSIVE);
                break;
        }
        ++m_introStage;
        if (timer)
            ResetTimer(KAEL_INTRO, timer);
    }

    void HandleOutro()
    {
        uint32 timer = 0;
        switch (m_outroStage)
        {
            case 0:
                DespawnGuids(m_spawns);
                SetCombatScriptStatus(true);
                m_creature->SetTarget(nullptr);
                SetMeleeEnabled(false);
                DisableTimer(KAEL_GRAVITY_LAPSE_SCRIPT);
                DoScriptText(SAY_DEATH, m_creature);
                m_creature->SetFacingTo(m_creature->GetRespawnPosition().o);
                timer = 1200;
                break;
            case 1:
                m_creature->CastSpell(nullptr, SPELL_EMOTE_TALK_EXCLAMATION, TRIGGERED_NONE);
                timer = 2500;
                break;
            case 2:
                m_creature->CastSpell(nullptr, SPELL_EMOTE_POINT, TRIGGERED_NONE);
                timer = 2500;
                break;
            case 3:
                m_creature->CastSpell(nullptr, SPELL_EMOTE_ROAR, TRIGGERED_NONE);
                timer = 2500;
                break;
            case 4:
                m_creature->CastSpell(nullptr, SPELL_EMOTE_ROAR, TRIGGERED_NONE);
                timer = 1500;
                break;
            case 5:
                SetCombatScriptStatus(false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->CastSpell(nullptr, SPELL_SUICIDE, TRIGGERED_NONE);
                break;
        }
        ++m_outroStage;
        if (timer)
            ResetTimer(KAEL_OUTRO, timer);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_FLAME_STRIKE_TRIGGER)
        {
            summoned->AI()->SetCombatMovement(false);
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->CastSpell(nullptr, SPELL_FLAME_STRIKE_DUMMY, TRIGGERED_NONE);
        }
        else if (summoned->GetEntry() != NPC_ARCANE_SPHERE)
        {
            // Attack or follow target
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                summoned->AI()->AttackStart(target);
        }
        m_spawns.push_back(summoned->GetObjectGuid());
    }

    void HandleGravityLapse()
    {
        uint32 timer = 0;
        switch (m_gravityLapseStage)
        {
            case 0:
                m_creature->SetFacingTo(m_creature->GetRespawnPosition().o);
                for (uint8 i = 0; i < MAX_ARCANE_SPHERES; ++i)
                    DoCastSpellIfCan(nullptr, SPELL_ARCANE_SPHERE_SUMMON);
                timer = 1500;
                break;
            case 1:
                DoCastSpellIfCan(nullptr, SPELL_GRAVITY_LAPSE_VISUAL);
                SetCombatScriptStatus(false);
                SetMeleeEnabled(true);
                break;
        }
        ++m_gravityLapseStage;
        if (timer)
            ResetTimer(KAEL_GRAVITY_LAPSE_SCRIPT, timer);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case KAEL_ACTION_ENERGY_FEEDBACK:
            {
                DoScriptText(SAY_TIRED, m_creature);
                DoCastSpellIfCan(nullptr, m_isRegularMode ? SPELL_POWER_FEEDBACK : SPELL_POWER_FEEDBACK_H);
                SetActionReadyStatus(action, false);
                return;
            }
            case KAEL_ACTION_GRAVITY_LAPSE:
            {
                // Cast Gravity Lapse on Players
                if (DoCastSpellIfCan(nullptr, SPELL_GRAVITY_LAPSE) == CAST_OK)
                {
                    if (m_firstGravityLapse)
                    {
                        DoScriptText(SAY_GRAVITY_LAPSE, m_creature);
                        m_firstGravityLapse = false;
                    }
                    else
                        DoScriptText(SAY_RECAST_GRAVITY, m_creature);

                    ResetTimer(KAEL_GRAVITY_LAPSE_SCRIPT, 4500);
                    m_gravityLapseStage = 0;
                    SetCombatScriptStatus(true);
                    m_creature->SetTarget(nullptr);
                    m_creature->SetFacingTo(m_creature->GetRespawnPosition().o);
                }
                return;
            }
            case KAEL_ACTION_PHASE_TRANSITION:
            {
                // Below 50%
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_TELEPORT_CENTER) == CAST_OK)
                    {
                        SetCombatMovement(false);
                        SetMeleeEnabled(false);

                        m_isFirstPhase = false;
                        SetActionReadyStatus(action, false);
                        DisableCombatAction(KAEL_ACTION_PYROBLAST);
                        DisableCombatAction(KAEL_ACTION_SHOCK_BARRIER);
                        DisableCombatAction(KAEL_ACTION_FLAMESTRIKE);
                        DisableCombatAction(KAEL_ACTION_PHOENIX);
                        DisableCombatAction(KAEL_ACTION_FIREBALL);
                        SetActionReadyStatus(KAEL_ACTION_GRAVITY_LAPSE, true);
                    }
                }
                return;
            }
            case KAEL_ACTION_PYROBLAST:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_PYROBLAST) == CAST_OK)
                {
                    DoScriptText(SAY_PYROBLAST, m_creature);
                    DisableCombatAction(action);
                }
                return;
            }
            case KAEL_ACTION_SHOCK_BARRIER:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SHOCK_BARRIER) == CAST_OK)
                {
                    ResetCombatAction(KAEL_ACTION_PYROBLAST, 2000);
                    ResetCombatAction(action, 60000);
                }
                return;
            }
            case KAEL_ACTION_FLAMESTRIKE:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_FLAME_STRIKE, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, m_isRegularMode ? SPELL_FLAME_STRIKE : SPELL_FLAME_STRIKE_H) == CAST_OK)
                    {
                        DoScriptText(SAY_FLAMESTRIKE, m_creature);
                        ResetCombatAction(action, urand(15000, 25000));
                    }
                }
                return;
            }
            case KAEL_ACTION_PHOENIX:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_PHOENIX) == CAST_OK)
                {
                    DoScriptText(SAY_PHOENIX, m_creature);
                    ResetCombatAction(action, 45000);
                }
                return;
            }
            case KAEL_ACTION_FIREBALL:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H) == CAST_OK)
                    ResetCombatAction(action, GetCurrentRangedMode() ? urand(2000, 3000) : urand(4000, 6000));
                return;
            }
        }
    }
};

/*######
## mob_arcane_sphere
######*/

struct mob_arcane_sphereAI : public ScriptedAI
{
    mob_arcane_sphereAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        Reset();
    }

    ScriptedInstance* m_instance;

    uint32 m_uiDespawnTimer;
    uint32 m_uiChangeTargetTimer;

    void Reset() override
    {
        m_uiDespawnTimer      = 30000;
        m_uiChangeTargetTimer = urand(1000, 2000);

        DoCastSpellIfCan(nullptr, SPELL_ARCANE_SPHERE_PASSIVE);
    }

    void UpdateAI(const uint32 diff) override
    {
        // Should despawn when aura 44251 expires
        if (m_uiDespawnTimer < diff)
        {
            m_creature->Suicide();
            m_uiDespawnTimer = 0;
        }
        else
            m_uiDespawnTimer -= diff;

        if (m_uiChangeTargetTimer < diff)
        {
            if (!m_instance)
                return;

            // Follow the target - do not attack
            if (Creature* kael = m_instance->GetSingleCreatureFromStorage(NPC_KAELTHAS))
            {
                if (Unit* target = kael->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    m_creature->AddThreat(target, 1000000.f);
                    AttackStart(target);
                }
            }

            m_uiChangeTargetTimer = urand(5000, 15000);
        }
        else
            m_uiChangeTargetTimer -= diff;
    }
};

struct spell_gravity_lapse_mgt : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        static const uint32 aGravityLapseSpells[] = { 44219, 44220, 44221, 44222, 44223 };
        spell->GetCaster()->CastSpell(unitTarget, aGravityLapseSpells[spell->GetScriptValue()], TRIGGERED_OLD_TRIGGERED);
        unitTarget->CastSpell(nullptr, SPELL_GRAVITY_LAPSE_FLY, TRIGGERED_OLD_TRIGGERED);
        if (unitTarget->GetMap()->IsRegularDifficulty())
            unitTarget->CastSpell(nullptr, SPELL_GRAVITY_LAPSE_DOT_N, TRIGGERED_OLD_TRIGGERED);
        else
            unitTarget->CastSpell(nullptr, SPELL_GRAVITY_LAPSE_DOT_H, TRIGGERED_OLD_TRIGGERED);
        spell->SetScriptValue(spell->GetScriptValue() + 1);
    }
};

struct spell_clear_flight_mgt : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget)
            return;

        unitTarget->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
        unitTarget->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT_H);
        unitTarget->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT_N);
    }
};

void AddSC_boss_felblood_kaelthas()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_felblood_kaelthas";
    pNewScript->GetAI = &GetNewAIInstance<boss_felblood_kaelthasAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_arcane_sphere";
    pNewScript->GetAI = &GetNewAIInstance<mob_arcane_sphereAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_gravity_lapse_mgt>("spell_gravity_lapse_mgt");
    RegisterSpellScript<spell_clear_flight_mgt>("spell_clear_flight_mgt");
}
