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
SDName: boss_the_lurker_below
SD%Complete: 100
SDComment:
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "serpent_shrine.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    EMOTE_DEEP_BREATH               = -1548056,

    SPELL_WHIRL                     = 37660,
    SPELL_GEYSER                    = 37478,
    SPELL_SPOUT                     = 37431,                // trigger spells 37429, 37430
    SPELL_SPOUT_LEFT                = 37429,
    SPELL_SPOUT_RIGHT               = 37430,
    SPELL_SPOUT_DAMAGE              = 37433,
    SPELL_WATERBOLT                 = 37138,
    SPELL_SUBMERGE                  = 28819,
    SPELL_CLEAR_ALL_DEBUFFS         = 34098,

    NPC_COILFANG_AMBUSHER           = 21865,
    NPC_COILFANG_GUARDIAN           = 21873,

    MAX_SUBMERGE_ADDS               = 9,
};

enum Phases
{
    PHASE_EMERGING              = 0,
    PHASE_NORMAL                = 1,
    PHASE_SPOUT                 = 2,
    PHASE_SUBMERGED             = 3,           
};

struct AddsLocations
{
    uint32 uiEntry;
    float fX, fY, fZ;
    uint32 uiPathId;
};

static const AddsLocations aLurkerLoc[MAX_SUBMERGE_ADDS] =
{
    {NPC_COILFANG_AMBUSHER, 99.992676f, -275.775f, -21.852718f, 0},
    {NPC_COILFANG_AMBUSHER, 158.81189f, -316.7783f, -20.568338f, 1},
    {NPC_COILFANG_AMBUSHER, 82.55382f, -550.19965f, -20.765057f, 2},
    {NPC_COILFANG_AMBUSHER, 17.01065f, -557.4272f, -21.487282f, 3},
    {NPC_COILFANG_AMBUSHER, -14.230728f, -549.0762f, -21.056725f, 4},
    {NPC_COILFANG_AMBUSHER, -1.9712651f, -540.2956f, -21.792835f, 5},
    {NPC_COILFANG_GUARDIAN, 106.962f, -468.5177f, -21.63681f, 0},
    {NPC_COILFANG_GUARDIAN, 65.17899f, -340.6617f, -21.5217f, 1,},
    {NPC_COILFANG_GUARDIAN, -49.6826f, -414.9719f, -20.54604f, 2},
};

enum LurkerActions
{
    LURKER_SUBMERGE_PHASE,
    LURKER_SPOUT,
    LURKER_WHIRL,
    LURKER_GEYSER,
    LURKER_WATERBOLT,
    LURKER_ACTION_MAX,
    LURKER_INITIAL_AGGRO,
    LURKER_SUBMERGE_ACTIONS,
    LURKER_SPOUT_END,
};

struct boss_the_lurker_belowAI : public CombatAI
{
    boss_the_lurker_belowAI(Creature* pCreature) : CombatAI(pCreature, LURKER_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        AddCombatAction(LURKER_SUBMERGE_PHASE, 90000u);
        AddCombatAction(LURKER_SPOUT, 42000u);
        AddCombatAction(LURKER_WHIRL, 18000u);
        AddCombatAction(LURKER_GEYSER, 50000u);
        AddCombatAction(LURKER_WATERBOLT, 2000u);
        AddCustomAction(LURKER_INITIAL_AGGRO, 2000u, [&]() { HandleInitialAggro(); });
        AddCustomAction(LURKER_SUBMERGE_ACTIONS, true, [&]() { HandleSubmergeActions(); });
        AddCustomAction(LURKER_SPOUT_END, true, [&]() { HandleSpoutEnd(); });
        m_creature->SetSwim(true);
        SetReactState(REACT_PASSIVE);
    }

    ScriptedInstance* m_instance;

    uint32 m_submergePhases;

    int32 m_rangeCheckState;

    void Reset() override
    {
        CombatAI::Reset();
        m_rangeCheckState   = -1;

        m_submergePhases = 0;

        m_creature->SetImmobilizedState(true);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(false);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_THELURKER_EVENT, FAIL);

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*victim*/) override
    {
        if (m_instance)
            m_instance->SetData(TYPE_THELURKER_EVENT, DONE);
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 pointId) override
    {
        if (motionType != WAYPOINT_MOTION_TYPE || pointId != 6)
            return;

        summoned->GetMotionMaster()->Clear();

        if (summoned->GetEntry() == NPC_COILFANG_AMBUSHER)
        {
            summoned->AI()->SetCombatMovement(false);
            summoned->SetImmobilizedState(true);
        }

        summoned->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        summoned->SetInCombatWithZone();
    }

    // Wrapper to summon adds in phase 2
    void DoSummonCoilfangNaga()
    {
        for (auto& i : aLurkerLoc)
            m_creature->SummonCreature(i.uiEntry, i.fX, i.fY, i.fZ, 0.0f, TEMPSPAWN_DEAD_DESPAWN, 0, false, true, i.uiPathId);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
    }

    void HandleInitialAggro()
    {
        SetReactState(REACT_AGGRESSIVE);
        SetMeleeEnabled(true);
        m_creature->SetInCombatWithZone();
        AttackClosestEnemy();
    }

    void HandleSubmergeActions()
    {
        uint32 timer = 0;
        switch (m_submergePhases)
        {
            case 0:
            {
                m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                timer = 2000;
                break;
            }
            case 1:
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                timer = 1000;
                break;
            }
            case 2:
            {
                ResetCombatAction(LURKER_WHIRL, 0);
                ResetCombatAction(LURKER_SUBMERGE_PHASE, 2 * MINUTE * IN_MILLISECONDS);
                ResetCombatAction(LURKER_SPOUT, 2000);
                ResetCombatAction(LURKER_GEYSER, 50000);
                ResetCombatAction(LURKER_WATERBOLT, 2000);
                SetCombatScriptStatus(false);
                break;
            }
        }
        ++m_submergePhases;
        if (timer)
            ResetTimer(LURKER_SUBMERGE_ACTIONS, timer);
    }

    void HandleSpoutEnd()
    {
        // Remove rotation auras
        m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_LEFT);
        m_creature->RemoveAurasDueToSpell(SPELL_SPOUT_RIGHT);
        SetCombatScriptStatus(false);
        m_meleeEnabled = true;
        if (m_creature->GetVictim())
        {
            m_creature->MeleeAttackStart(m_creature->GetVictim());
            m_creature->SetTarget(m_creature->GetVictim());
        }

        ResetCombatAction(LURKER_WHIRL, 2000);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LURKER_SUBMERGE_PHASE:
            {
                m_creature->CastSpell(nullptr, SPELL_CLEAR_ALL_DEBUFFS, TRIGGERED_NONE);
                if (DoCastSpellIfCan(nullptr, SPELL_SUBMERGE) == CAST_OK)
                {
                    DoSummonCoilfangNaga();
                    SetCombatScriptStatus(true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE); // UNIT_STAND_STATE_CUSTOM
                    m_creature->SetStandState(UNIT_STAND_STATE_CUSTOM); // TODO investigate Submerge visual spell to see if it shouldnt do this instead in aura
                    m_submergePhases = 0;
                    ResetTimer(LURKER_SUBMERGE_ACTIONS, MINUTE * IN_MILLISECONDS);
                    DisableCombatAction(LURKER_WHIRL);
                    DisableCombatAction(LURKER_SUBMERGE_PHASE);
                    DisableCombatAction(LURKER_SPOUT);
                    DisableCombatAction(LURKER_GEYSER);
                    DisableCombatAction(LURKER_WATERBOLT);
                    break;
                }
                break;
            }
            case LURKER_SPOUT:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SPOUT) == CAST_OK)
                {
                    DoScriptText(EMOTE_DEEP_BREATH, m_creature);

                    // Remove the target focus but allow the boss to face the current victim
                    SetCombatScriptStatus(true);
                    m_meleeEnabled = false;
                    m_creature->MeleeAttackStop(m_creature->GetVictim());
                    m_creature->SetTarget(nullptr);

                    ResetTimer(LURKER_SPOUT_END, 16000);
                    ResetCombatAction(action, 45000);
                }
                break;
            }
            case LURKER_WHIRL:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_WHIRL) == CAST_OK)
                    ResetCombatAction(action, 18000);
                break;
            }
            case LURKER_GEYSER:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_GEYSER, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(target, SPELL_GEYSER) == CAST_OK)
                        ResetCombatAction(action, urand(50000, 60000));
                break;
            }
            case LURKER_WATERBOLT:
            {
                uint32 timer = 500;
                // If victim exists we have a target in melee range
                if (m_creature->GetVictim() && m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                    m_rangeCheckState = -1;
                // Spam Waterbolt spell when not tanked
                else
                {
                    ++m_rangeCheckState;
                    if (m_rangeCheckState > 1)
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_WATERBOLT, SELECT_FLAG_PLAYER))
                            if (DoCastSpellIfCan(target, SPELL_WATERBOLT) == CAST_OK)
                                timer = 2500;
                }
                ResetCombatAction(action, timer);
                break;
            }
        }
    }
};

// Cast the spell that should summon the Lurker-Below
bool GOUse_go_strange_pool(Player* /*player*/, GameObject* go)
{
    // There is some chance to fish The Lurker Below, sources are from 20s to 10minutes, average 5min => 20 tries, hence 5%
    if (urand(0, 99) < 10)
    {
        if (ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData())
        {
            if (pInstance->GetData(TYPE_THELURKER_EVENT) == NOT_STARTED || pInstance->GetData(TYPE_THELURKER_EVENT) == FAIL)
            {
                pInstance->SetData(TYPE_THELURKER_EVENT, IN_PROGRESS);          
                go->SetRespawnTime(7 * DAY); // dont respawn until reset
                return true;
            }
        }
    }
    return false;
}

struct Spout : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (!spell->GetUnitTarget())
            return;

        spell->GetUnitTarget()->CastSpell(nullptr, urand(0, 1) ? SPELL_SPOUT_LEFT : SPELL_SPOUT_RIGHT, TRIGGERED_OLD_TRIGGERED);
        return;
    }
};

struct SpoutTurning : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        Unit* target = aura->GetTarget();
        float newAngle = target->GetOrientation();

        if (aura->GetId() == 37429)
            newAngle += 2 * M_PI_F / 72;
        else
            newAngle -= 2 * M_PI_F / 72;

        newAngle = MapManager::NormalizeOrientation(newAngle);

        target->SetFacingTo(newAngle);
        target->SetOrientation(newAngle);

        data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(SPELL_SPOUT_DAMAGE);
        data.caster = target;
        data.target = nullptr;
    }
};

void AddSC_boss_the_lurker_below()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_the_lurker_below";
    pNewScript->GetAI = &GetNewAIInstance<boss_the_lurker_belowAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_strange_pool";
    pNewScript->pGOUse = &GOUse_go_strange_pool;
    pNewScript->RegisterSelf();

    RegisterSpellScript<Spout>("spell_lurker_spout_initial");
    RegisterSpellScript<SpoutTurning>("spell_lurker_spout_turning");
}
