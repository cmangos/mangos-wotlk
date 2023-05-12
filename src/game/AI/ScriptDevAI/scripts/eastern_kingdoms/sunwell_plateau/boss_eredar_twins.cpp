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
SDName: boss_eredar_twins
SD%Complete: 75
SDComment: A few spells are not working proper yet; Shadow image script needs improvement; Add invulnerability in phase transition
SDCategory: Sunwell Plateau
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "sunwell_plateau.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include <sstream>

enum
{
    SAY_INTRO_1                             = -1580044,
    SAY_INTRO_2                             = -1580045,

    SAY_SACROLASH_SHADOW_NOVA               = -1580052,
    SAY_SACROLASH_EMPOWER                   = -1580053,
    SAY_SACROLASH_KILL_1                    = 25519,
    SAY_SACROLASH_KILL_2                    = 25520,
    SAY_SACROLASH_DEAD                      = -1580056,
    SAY_SACROLASH_BERSERK                   = -1580057,

    SAY_ALYTHESS_CANFLAGRATION              = -1580058,
    SAY_ALYTHESS_EMPOWER                    = -1580059,
    SAY_ALYTHESS_KILL_1                     = 25514,
    SAY_ALYTHESS_KILL_2                     = 25515,
    SAY_ALYTHESS_DEAD                       = -1580062,
    SAY_ALYTHESS_BERSERK                    = -1580063,

    SAY_EMOTE_SHADOW_NOVA                   = -1580117,
    SAY_EMOTE_CONFLAGRATION                 = -1580118,

    // Shared spells
    SPELL_TWINS_ENRAGE                      = 46587,
    SPELL_EMPOWER                           = 45366,        // Cast on self when the twin sister dies
    SPELL_DARK_FLAME                        = 45345,

    // Sacrolash spells
    SPELL_DARK_TOUCHED                      = 45347,        // Player debuff; removed by shadow damage
    SPELL_SHADOW_BLADES                     = 45248,        // 10 secs
    SPELL_SHADOW_NOVA                       = 45329,        // 30-35 secs
    SPELL_CONFOUNDING_BLOW                  = 45256,        // Daze; 25 secs
    SPELL_SHADOW_NOVA_UNK                   = 45332,        // Unknown
    SPELL_DUAL_WEILD                        = 42459,
    SPELL_SHADOWFORM                        = 45455,

    // Shadow Image spells
    // NPC_SHADOW_IMAGE                        = 25214,
    SPELL_SUMMON_SHADOW_IMAGES              = 45258,
    SPELL_SHADOWFURY                        = 45270,
    SPELL_DARK_STRIKE                       = 45271,
    SPELL_SHADOWSTEP                        = 45273,
    SPELL_IMAGE_VISUAL                      = 45263,

    // Alythess spells
    SPELL_PYROGENICS                        = 45230,        // Self buff; 15secs
    SPELL_FLAME_TOUCHED                     = 45348,        // Player debuff; removed by shadow damage
    SPELL_CONFLAGRATION                     = 45342,        // 30-35 secs
    SPELL_CONFLAGRATION_TICK                = 46768,
    SPELL_BLAZE                             = 45235,        // On main target every 3 secs; should trigger 45236 which leaves a fire on the ground
    SPELL_BLAZE_SUMMON                      = 45236,
    SPELL_FLAME_SEAR                        = 46771,        // A few random targets debuff
    SPELL_CONFLAGRATION_UNK                 = 45333,        // Unknown
    SPELL_FIREFORM                          = 45457,

    SPELL_DARK_FLAME_AURA_ALYTHESS          = 47300,        // Aura buff that make flame touched spell proc - for Alythess
    SPELL_DARK_FLAME_AURA_SCAROLASH         = 45343,        // Aura buff that make dark touched spell proc  - for Scarolash

    SPELL_BURN                              = 45246,

    SPELL_INSTAKILL_TWINS                   = 29878,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_INTRO_1, NPC_SACROLASH, 1000},
    {SAY_INTRO_2, NPC_ALYTHESS,  1500},
    {0, 0, 0},
};

/*######
## boss_alythess
######*/

enum AlythessActions
{
    ALYTHESS_ENRAGE,
    ALYTHESS_PYROGENICS,
    ALYTHESS_CONFLAGRATION,
    ALYTHESS_BLAZE,
    ALYTHESS_FLAME_SEAR,
    ALYTHESS_ACTION_MAX,
    ALYTHESS_DELAY,
};

struct boss_alythessAI : public CombatAI
{
    boss_alythessAI(Creature* creature) : CombatAI(creature, ALYTHESS_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_introDialogue(aIntroDialogue)
    {
        SetRangedMode(true, 200.f, TYPE_FULL_CASTER);
        m_introDialogue.InitializeDialogueHelper(m_instance);
        SetDeathPrevention(true);
        AddCombatAction(ALYTHESS_ENRAGE, uint32(6 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(ALYTHESS_PYROGENICS, 20000, 27000);
        AddCombatAction(ALYTHESS_CONFLAGRATION, 15000u);
        AddCombatAction(ALYTHESS_BLAZE, 1000u);
        AddCombatAction(ALYTHESS_FLAME_SEAR, 10000u);
        AddCustomAction(ALYTHESS_DELAY, true, [&]() { HandleEmpowerDelay(); });
        AddOnKillText(SAY_ALYTHESS_KILL_1, SAY_ALYTHESS_KILL_2);
    }

    ScriptedInstance* m_instance;
    DialogueHelper m_introDialogue;
    TimePoint m_aggro;

    void Reset() override
    {
        CombatAI::Reset();

        SetDeathPrevention(true);
        m_creature->SetNoLoot(false);
        SetCombatScriptStatus(false);
        SetMeleeEnabled(false);

        DoCastSpellIfCan(nullptr, SPELL_FIREFORM, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);

        m_creature->SetCorpseDelay(120);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_EREDAR_TWINS) != FAIL)
                m_instance->SetData(TYPE_EREDAR_TWINS, FAIL);
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_EREDAR_TWINS) != IN_PROGRESS)
                m_instance->SetData(TYPE_EREDAR_TWINS, IN_PROGRESS);
        }

        DoCastSpellIfCan(nullptr, SPELL_DARK_FLAME_AURA_SCAROLASH, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_DARK_FLAME_AURA_ALYTHESS, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);

        m_aggro = m_creature->GetMap()->GetCurrentClockTime();
    }

    void JustPreventedDeath(Unit* /*killer*/) override
    {
        if (Creature* sacrolash = m_instance->GetSingleCreatureFromStorage(NPC_SACROLASH))
        {
            if (!sacrolash->IsAlive())
            {
                // should never happen
                SetDeathPrevention(false);
            }
            else
            {
                // Remove loot flag and cast empower
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                m_creature->SetNoLoot(true);
                m_creature->CastSpell(nullptr, SPELL_INSTAKILL_TWINS, TRIGGERED_OLD_TRIGGERED);
                sacrolash->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, sacrolash);
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance && m_creature->HasAura(SPELL_EMPOWER))
            m_instance->SetData(TYPE_EREDAR_TWINS, DONE);
        DoScriptText(SAY_ALYTHESS_DEAD, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(SAY_ALYTHESS_EMPOWER, m_creature);
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->CastSpell(nullptr, SPELL_DARK_FLAME_AURA_SCAROLASH, TRIGGERED_NONE);
            m_creature->CastSpell(nullptr, SPELL_EMPOWER, TRIGGERED_NONE);
            SetDeathPrevention(false);
            SetCombatMovement(false);
            SetCombatScriptStatus(true);
            m_creature->SetTarget(nullptr);
            ResetTimer(ALYTHESS_DELAY, 1500);
            m_creature->SetCorpseDelay(3600);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
            m_introDialogue.StartNextDialogueText(SAY_INTRO_1);
    }

    void HandleEmpowerDelay()
    {
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        if (m_creature->GetVictim())
            AttackStart(m_creature->GetVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ALYTHESS_ENRAGE:
                if (DoCastSpellIfCan(m_creature, SPELL_TWINS_ENRAGE) == CAST_OK)
                {
                    DoScriptText(SAY_ALYTHESS_BERSERK, m_creature);
                    ResetCombatAction(action, 6 * MINUTE * IN_MILLISECONDS);
                    sLog.outCustomLog("Sacrolash enrage at %lu and aggro at %lu", m_creature->GetMap()->GetCurrentClockTime().time_since_epoch().count(), m_aggro.time_since_epoch().count());
                }
                break;
            case ALYTHESS_PYROGENICS:
                if (DoCastSpellIfCan(m_creature, SPELL_PYROGENICS) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 27000));
                break;
            case ALYTHESS_CONFLAGRATION:
            {
                Unit* source = m_creature;

                if (Creature* sacrolash = m_instance->GetSingleCreatureFromStorage(NPC_SACROLASH))
                    if (sacrolash->IsAlive())
                        source = sacrolash;

                // If sister is dead cast shadownova instead of conflagration
                bool switchSpell = m_creature->HasAura(SPELL_EMPOWER);
                uint32 spellId = switchSpell ? SPELL_SHADOW_NOVA : SPELL_CONFLAGRATION;
                SelectAttackingTargetParams params;
                if (m_creature->GetVictim())
                    params.skip.guid = m_creature->GetVictim()->GetObjectGuid();
                Unit* target = source->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, urand(2, 7), spellId, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_CUSTOM, params);
                if (!target)
                    target = source->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, spellId, SELECT_FLAG_PLAYER);


                if (DoCastSpellIfCan(target, spellId) == CAST_OK)
                {
                    DoScriptText(switchSpell ? SAY_EMOTE_SHADOW_NOVA : SAY_EMOTE_CONFLAGRATION, m_creature, target);
                    if (!switchSpell)
                        DoScriptText(SAY_ALYTHESS_CANFLAGRATION, m_creature);
                    ResetCombatAction(action, 35000);
                }
                break;
            }
            case ALYTHESS_BLAZE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_BLAZE) == CAST_OK)
                    ResetCombatAction(action, 3000);
                break;
            case ALYTHESS_FLAME_SEAR:
                if (DoCastSpellIfCan(nullptr, SPELL_FLAME_SEAR) == CAST_OK)
                    ResetCombatAction(action, urand(11000, 15000));
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        m_introDialogue.DialogueUpdate(diff);
        CombatAI::UpdateAI(diff);
    }
};

/*######
## boss_sacrolash
######*/

enum SacrolashActions
{
    SACROLASH_ENRAGE,
    SACROLASH_SHADOW_NOVA,
    SACROLASH_CONFOUNDING_BLOW,
    SACROLASH_SHADOW_BLADES,
    SACROLASH_SUMMON_SHADOW_IMAGE,
    SACROLASH_ACTION_MAX,
    SACROLASH_DELAY,
};

struct boss_sacrolashAI : public CombatAI
{
    boss_sacrolashAI(Creature* creature) : CombatAI(creature, SACROLASH_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        SetDeathPrevention(true);
        AddCombatAction(SACROLASH_ENRAGE, uint32(6 * MINUTE * IN_MILLISECONDS));
        AddCombatAction(SACROLASH_SHADOW_NOVA, 35000u);
        AddCombatAction(SACROLASH_CONFOUNDING_BLOW, 25000, 27000);
        AddCombatAction(SACROLASH_SHADOW_BLADES, 10000, 11000);
        AddCombatAction(SACROLASH_SUMMON_SHADOW_IMAGE, 10000u);
        AddCustomAction(ALYTHESS_DELAY, true, [&]() { HandleEmpowerDelay(); });
        AddOnKillText(SAY_SACROLASH_KILL_1, SAY_SACROLASH_KILL_2);
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float y, float) -> bool
        {
            return (x < 1800 && y < 580) || (x > 1832 && y > 670);
        });
    }

    ScriptedInstance* m_instance;
    TimePoint m_aggro;

    void Reset() override
    {
        CombatAI::Reset();

        SetDeathPrevention(true);
        m_creature->SetNoLoot(false);
        SetCombatScriptStatus(false);

        DoCastSpellIfCan(nullptr, SPELL_DUAL_WEILD, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_SHADOWFORM, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);

        m_creature->SetCorpseDelay(120);
    }

    void EnterEvadeMode() override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_EREDAR_TWINS) != FAIL)
                m_instance->SetData(TYPE_EREDAR_TWINS, FAIL);
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
        {
            if (m_instance->GetData(TYPE_EREDAR_TWINS) != IN_PROGRESS)
                m_instance->SetData(TYPE_EREDAR_TWINS, IN_PROGRESS);
        }

        DoCastSpellIfCan(nullptr, SPELL_DARK_FLAME_AURA_SCAROLASH, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);
        DoCastSpellIfCan(nullptr, SPELL_DARK_FLAME_AURA_ALYTHESS, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED);

        m_aggro = m_creature->GetMap()->GetCurrentClockTime();
    }

    void JustPreventedDeath(Unit* /*killer*/) override
    {
        if (Creature* alythess = m_instance->GetSingleCreatureFromStorage(NPC_ALYTHESS))
        {
            if (!alythess->IsAlive())
            {
                // should never happen
                SetDeathPrevention(false);
            }
            else
            {
                // Remove loot flag and cast empower
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                m_creature->SetNoLoot(true);
                m_creature->CastSpell(nullptr, SPELL_INSTAKILL_TWINS, TRIGGERED_OLD_TRIGGERED);
                alythess->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, alythess);
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_instance && m_creature->HasAura(SPELL_EMPOWER))
            m_instance->SetData(TYPE_EREDAR_TWINS, DONE);
        DoScriptText(SAY_SACROLASH_DEAD, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(SAY_SACROLASH_EMPOWER, m_creature);
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->CastSpell(nullptr, SPELL_DARK_FLAME_AURA_ALYTHESS, TRIGGERED_NONE);
            m_creature->CastSpell(nullptr, SPELL_EMPOWER, TRIGGERED_NONE);
            SetDeathPrevention(false);
            SetCombatMovement(false);
            SetCombatScriptStatus(true);
            SetMeleeEnabled(false);
            m_creature->SetTarget(nullptr);
            ResetTimer(SACROLASH_DELAY, 1500);
            m_creature->SetCorpseDelay(3600);
        }
    }

    void HandleEmpowerDelay()
    {
        SetCombatScriptStatus(false);
        SetCombatMovement(true);
        SetMeleeEnabled(true);
        if (m_creature->GetVictim())
            AttackStart(m_creature->GetVictim());
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SACROLASH_ENRAGE:
                if (DoCastSpellIfCan(m_creature, SPELL_TWINS_ENRAGE) == CAST_OK)
                {
                    DoScriptText(SAY_SACROLASH_BERSERK, m_creature);
                    ResetCombatAction(action, 6 * MINUTE * IN_MILLISECONDS);
                    sLog.outCustomLog("Sacrolash enrage at %lu and aggro at %lu", m_creature->GetMap()->GetCurrentClockTime().time_since_epoch().count(), m_aggro.time_since_epoch().count());
                }
                break;
            case SACROLASH_SHADOW_NOVA:
            {
                Unit* source = m_creature;

                if (Creature* alythess = m_instance->GetSingleCreatureFromStorage(NPC_ALYTHESS))
                    if (alythess->IsAlive())
                        source = alythess;

                // If sister is dead cast conflagration instead of shadownova
                bool switchSpell = m_creature->HasAura(SPELL_EMPOWER);
                uint32 spellId = switchSpell ? SPELL_CONFLAGRATION : SPELL_SHADOW_NOVA;
                SelectAttackingTargetParams params;
                if (m_creature->GetVictim())
                    params.skip.guid = m_creature->GetVictim()->GetObjectGuid();
                Unit* target = source->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, urand(1, 7), spellId, SELECT_FLAG_PLAYER | SELECT_FLAG_SKIP_CUSTOM, params);
                if (!target)
                    target = source->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, spellId, SELECT_FLAG_PLAYER);

                if (DoCastSpellIfCan(target, spellId) == CAST_OK)
                {
                    DoScriptText(switchSpell ? SAY_EMOTE_CONFLAGRATION : SAY_EMOTE_SHADOW_NOVA, m_creature, target);
                    if (!switchSpell)
                        DoScriptText(SAY_SACROLASH_SHADOW_NOVA, m_creature);
                    ResetCombatAction(action, 35000);
                }
                break;
            }
            case SACROLASH_CONFOUNDING_BLOW:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CONFOUNDING_BLOW) == CAST_OK)
                    ResetCombatAction(action, urand(25000, 27000));
                break;
            case SACROLASH_SHADOW_BLADES:
                if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_BLADES) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 11000));
                break;
            case SACROLASH_SUMMON_SHADOW_IMAGE:
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_SHADOW_IMAGES) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 20000));
                break;
        }
    }

    // Return a random target which it's not in range of 10 yards of boss
    Unit* GetRandomTargetAtDist(float fDist) const
    {
        std::vector<Unit*> m_vRangeTargets;

        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (auto iter : tList)
        {
            if (Unit* threatTarget = m_creature->GetMap()->GetUnit(iter->getUnitGuid()))
            {
                if (!threatTarget->IsPlayer())
                    continue;

                if (!threatTarget->IsWithinDistInMap(m_creature, fDist))
                    m_vRangeTargets.push_back(threatTarget);
            }
        }

        if (!m_vRangeTargets.empty())
            return m_vRangeTargets[urand(0, m_vRangeTargets.size() - 1)];
        return m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, nullptr, SELECT_FLAG_PLAYER);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SHADOW_IMAGE)
            summoned->CastSpell(nullptr, SPELL_IMAGE_VISUAL, TRIGGERED_NONE);
    }
};

/*######
## npc_shadow_image
######*/

struct npc_shadow_imageAI : public ScriptedAI
{
    npc_shadow_imageAI(Creature* creature) : ScriptedAI(creature), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())) { Reset(); }

    uint32 m_uiChosenAbility;
    uint32 m_uiSuicideTimer;
    uint32 m_uiAbilityTimer;
    uint8 m_uiDarkStrikes;

    ScriptedInstance* m_instance;

    void Reset() override
    {
        // Choose only one spell for attack
        m_uiChosenAbility = urand(0, 5) ? SPELL_DARK_STRIKE : SPELL_SHADOWFURY;
        m_uiAbilityTimer = 500;
        m_uiDarkStrikes = 0;
        m_uiSuicideTimer = 0;
    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        m_creature->SetCorpseDelay(5);
        AttackRandomTarget(false);
        m_creature->ForcedDespawn(15000);
    }

    void AttackRandomTarget(bool shadowstep)
    {
        DoResetThreat();
        if (Creature* sacrolash = m_instance->GetSingleCreatureFromStorage(NPC_SACROLASH))
        {
            if (Unit* target = static_cast<boss_sacrolashAI*>(sacrolash->AI())->GetRandomTargetAtDist(10.0f)) // Attack random range target
            {
                m_creature->AddThreat(target, 1000000.f);
                m_creature->AI()->AttackStart(target);
                if (shadowstep)
                    DoCastSpellIfCan(target, SPELL_SHADOWSTEP);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Suicide on timer; this is needed because of the cast time
        if (m_uiSuicideTimer)
        {
            if (m_uiSuicideTimer <= uiDiff)
            {
                // confirmed suicide like this
                m_creature->Suicide();
            }
            else
                m_uiSuicideTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Do chosen ability
        switch (m_uiChosenAbility)
        {
            case SPELL_SHADOWFURY:
                if (m_uiAbilityTimer)
                {
                    if (m_uiAbilityTimer <= uiDiff)
                    {
                        if (m_creature->IsWithinDistInMap(m_creature->GetVictim(), INTERACTION_DISTANCE))
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_SHADOWFURY) == CAST_OK)
                                m_uiSuicideTimer = 3000, m_uiAbilityTimer = 0;
                        }
                    }
                    else
                        m_uiAbilityTimer -= uiDiff;
                }
                break;
            case SPELL_DARK_STRIKE:
                if (m_uiAbilityTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DARK_STRIKE) == CAST_OK)
                    {
                        ++m_uiDarkStrikes;
                        if (urand(1, 100) <= 10)
                            AttackRandomTarget(true);
                        // kill itself after 2 strikes
                        if (m_uiDarkStrikes == 2)
                            m_uiSuicideTimer = 3000;
                        m_uiAbilityTimer = 1000;
                    }
                }
                else
                    m_uiAbilityTimer -= uiDiff;
                break;
        }
    }
};

struct BlazeTwins : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        if (!target)
            return;

        target->CastSpell(nullptr, SPELL_BLAZE_SUMMON, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DarkFlameShadowAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
    }

    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (!data.spellInfo || !data.source || data.source->HasAura(SPELL_DARK_FLAME))
            return false;
        std::set<uint32> spellIds = { SPELL_CONFOUNDING_BLOW, SPELL_SHADOW_BLADES, SPELL_SHADOW_NOVA, SPELL_SHADOWFURY, SPELL_DARK_STRIKE };
        if (spellIds.find(data.spellInfo->Id) == spellIds.end())
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        procData.cooldown = 1;
        if (procData.source->HasAura(SPELL_FLAME_TOUCHED))
        {
            procData.source->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
            procData.triggerTarget = nullptr;
            procData.triggeredSpellId = SPELL_DARK_FLAME;
        }
        else
            procData.triggeredSpellId = SPELL_DARK_TOUCHED;
        return SPELL_AURA_PROC_OK;
    }
};

struct DarkFlameFireAura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
    }

    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
        if (!data.spellInfo || !data.source || data.source->HasAura(SPELL_DARK_FLAME))
            return false;
        std::set<uint32> spellIds = { SPELL_FLAME_SEAR, SPELL_CONFLAGRATION, SPELL_CONFLAGRATION_TICK, SPELL_BLAZE, SPELL_BURN };
        if (spellIds.find(data.spellInfo->Id) == spellIds.end())
            return false;
        return true;
    }

    SpellAuraProcResult OnProc(Aura* /*aura*/, ProcExecutionData& procData) const override
    {
        procData.cooldown = 1;
        if (procData.source->HasAura(SPELL_DARK_TOUCHED))
        {
            procData.source->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
            procData.triggerTarget = nullptr;
            procData.triggeredSpellId = SPELL_DARK_FLAME;
        }
        else
            procData.triggeredSpellId = SPELL_FLAME_TOUCHED;
        return SPELL_AURA_PROC_OK;
    }
};

struct FlameSear : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(urand(3, 5));
    }
};

void AddSC_boss_eredar_twins()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_alythess";
    pNewScript->GetAI = &GetNewAIInstance<boss_alythessAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_sacrolash";
    pNewScript->GetAI = &GetNewAIInstance<boss_sacrolashAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shadow_image";
    pNewScript->GetAI = &GetNewAIInstance<npc_shadow_imageAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<BlazeTwins>("spell_blaze_twins");
    RegisterSpellScript<DarkFlameShadowAura>("spell_dark_flame_shadow_aura");
    RegisterSpellScript<DarkFlameFireAura>("spell_dark_flame_fire_aura");
    RegisterSpellScript<FlameSear>("spell_flame_sear");
}
