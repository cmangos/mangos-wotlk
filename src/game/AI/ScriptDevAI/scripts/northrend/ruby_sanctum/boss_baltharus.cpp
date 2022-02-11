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
SDName: boss_baltharus
SD%Complete: 100
SDComment: Intro channeled spell implemented in DB script
SDCategory: Ruby Sanctum
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ruby_sanctum.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/SpellAuras.h"

enum
{
    // Xerestrasza intro and outro texts
    SAY_HELP                    = -1724000,
    SAY_INTRO                   = -1724001,

    // Baltharus texts
    SAY_AGGRO                   = -1724010,
    SAY_SLAY_1                  = -1724011,
    SAY_SLAY_2                  = -1724012,
    SAY_DEATH                   = -1724013,
    SAY_SPLIT                   = -1724014,

    // SPELL_BARRIER_CHANNEL    = 76221,            // channeled on the tree; handled in DB script
    SPELL_BLADE_TEMPEST         = 75125,
    SPELL_CLEAVE                = 40504,
    SPELL_ENERVATING_BRAND      = 74502,            // triggers 74505
    SPELL_REPELLING_WAVE        = 74509,
    SPELL_SIPHONED_MIGHT        = 74507,            // spell triggered on boss by 74505
    SPELL_SUMMON_CLONE          = 74511,            // summons 39899
    SPELL_SIMPLE_TELEPORT       = 64195,

    NPC_BALTHARUS_CLONE         = 39899,
};

static const DialogueEntry aIntroDialogue[] =
{
    {SAY_HELP,  NPC_XERESTRASZA,  7000},
    {SAY_INTRO, NPC_BALTHARUS,    0},
    {0, 0, 0},
};

enum BaltharusActions
{
    BALTHARUS_REPELLING_WAVE,
    BALTHARUS_ENERVATING_BRAND,
    BALTHARUS_SUMMON_CLONE,
    BALTHARUS_BLADE_TEMPEST,
    BALTHARUS_CLEAVE,
    BALTHARUS_ACTION_MAX,
};

/*######
## boss_baltharus
######*/

struct boss_baltharusAI : public CombatAI
{
    boss_baltharusAI(Creature* creature) : CombatAI(creature, BALTHARUS_ACTION_MAX), m_instance(static_cast<instance_ruby_sanctum*>(creature->GetInstanceData())),
        m_introDialogue(aIntroDialogue)
    {
        AddCombatAction(BALTHARUS_ENERVATING_BRAND, 12000u);
        AddCombatAction(BALTHARUS_BLADE_TEMPEST, 15000u);
        AddCombatAction(BALTHARUS_CLEAVE, 7000u, 10000u);
        AddCombatAction(BALTHARUS_SUMMON_CLONE, true);

        AddTimerlessCombatAction(BALTHARUS_REPELLING_WAVE, true);

        m_introDialogue.InitializeDialogueHelper(m_instance);

        // Health check percent depends on difficulty
        if (m_instance)
            m_fHealthPercentCheck = m_instance->Is25ManDifficulty() ? 33.3f : 50;
        else
            script_error_log("Instance Ruby Sanctum: ERROR Failed to load instance data for this instace.");

        m_bHasDoneIntro = false;
    }

    instance_ruby_sanctum* m_instance;
    DialogueHelper m_introDialogue;

    bool m_bHasDoneIntro;

    uint8 m_uiPhase;
    float m_fHealthPercentCheck;

    void Reset() override
    {
        CombatAI::Reset();

        m_uiPhase = 1;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_BALTHARUS, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!m_bHasDoneIntro && pWho->IsPlayer())
        {
            m_introDialogue.StartNextDialogueText(SAY_HELP);
            m_bHasDoneIntro = true;
        }

        CombatAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_BALTHARUS, DONE);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_BALTHARUS, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_BALTHARUS_CLONE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SIMPLE_TELEPORT, TRIGGERED_OLD_TRIGGERED);
            pSummoned->SetInCombatWithZone();
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BALTHARUS_REPELLING_WAVE:
                if (m_creature->GetHealthPercent() < 100 - m_fHealthPercentCheck * m_uiPhase)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_REPELLING_WAVE) == CAST_OK)
                    {
                        ResetCombatAction(BALTHARUS_SUMMON_CLONE, 2000);
                        ++m_uiPhase;
                    }
                }
                break;
            case BALTHARUS_ENERVATING_BRAND:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_ENERVATING_BRAND) == CAST_OK)
                        ResetCombatAction(action, 25000);
                }
                break;
            case BALTHARUS_BLADE_TEMPEST:
                if (DoCastSpellIfCan(m_creature, SPELL_BLADE_TEMPEST) == CAST_OK)
                    ResetCombatAction(action, 25000);
                break;
            case BALTHARUS_CLEAVE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            case BALTHARUS_SUMMON_CLONE:
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CLONE) == CAST_OK)
                {
                    DoScriptText(SAY_SPLIT, m_creature);
                    DisableCombatAction(action);
                }
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
## spell_enervating_brand_aura - 74502
######*/

struct spell_enervating_brand_aura : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& /*data*/) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!target || !caster)
            return;

        target->CastSpell(caster, 74507, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_baltharus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_baltharus";
    pNewScript->GetAI = &GetNewAIInstance<boss_baltharusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_enervating_brand_aura>("spell_enervating_brand_aura");
}
