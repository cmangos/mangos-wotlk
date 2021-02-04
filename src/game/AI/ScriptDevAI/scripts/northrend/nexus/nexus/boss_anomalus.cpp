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
SDName: Boss_Anomalus
SD%Complete: 100
SDComment:
SDCategory: Nexus
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "nexus.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_AGGRO                          = -1576006,
    SAY_RIFT                           = -1576007,
    SAY_SHIELD                         = -1576008,
    SAY_KILL                           = -1576009,
    SAY_DEATH                          = -1576010,
    EMOTE_OPEN_RIFT                    = -1576021,
    EMOTE_SHIELD                       = -1576022,

    // Anomalus
    SPELL_CREATE_RIFT                  = 47743,                 // spawn creature 26918
    SPELL_CHARGE_RIFT                  = 47747,
    SPELL_RIFT_SHIELD                  = 47748,

    SPELL_SPARK                        = 47751,
    SPELL_SPARK_H                      = 57062,

    NPC_CHAOTIC_RIFT                   = 26918
};

enum AnomalusActions
{
    ANOMALUS_ACTION_CHAOTIC_RIFT,
    ANOMALUS_ACTION_SPARK,
    ANOMALUS_ACTION_CREATE_RIFT,
    ANOMALUS_ACTION_RESUME_COMBAT,
    ANOMALUS_ACTION_MAX,
};

/*######
## boss_anomalus
######*/

struct boss_anomalusAI : public CombatAI
{
    boss_anomalusAI(Creature* creature) : CombatAI(creature, ANOMALUS_ACTION_MAX), m_instance(static_cast<instance_nexus*>(creature->GetInstanceData()))
    {
        AddCombatAction(ANOMALUS_ACTION_SPARK, 10000u);
        AddCombatAction(ANOMALUS_ACTION_CREATE_RIFT, 15000u);
        AddCombatAction(ANOMALUS_ACTION_RESUME_COMBAT, true);
        AddTimerlessCombatAction(ANOMALUS_ACTION_CHAOTIC_RIFT, true);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    instance_nexus* m_instance;
    bool m_isRegularMode;

    uint8 m_uiChaoticRiftCount;

    void Reset() override
    {
        CombatAI::Reset();

        m_uiChaoticRiftCount = 0;
    }

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ANOMALUS, IN_PROGRESS);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_ANOMALUS, DONE);
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHAOTIC_RIFT)
        {
            ++m_uiChaoticRiftCount;

            DoScriptText(SAY_RIFT, m_creature);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_CHAOTIC_RIFT)
        {
            --m_uiChaoticRiftCount;

            // If players kill the Chaotic Rifts then mark the achievement as false
            if (m_instance)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_CHAOS_THEORY, false);

            if (!m_uiChaoticRiftCount)
            {
                if (m_creature->HasAura(SPELL_RIFT_SHIELD))
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_RIFT_SHIELD);
                    m_creature->InterruptNonMeleeSpells(false);
                    DoResumeCombat();
                }
            }
        }
    }

    // Method to resume combat
    void DoResumeCombat()
    {
        ResetCombatAction(ANOMALUS_ACTION_SPARK, 10000);
        ResetCombatAction(ANOMALUS_ACTION_CREATE_RIFT, 15000);

        // inform remaining rifts to resume normal auras
        SendAIEventAround(AI_EVENT_CUSTOM_EVENTAI_A, m_creature, 0, 40.0f);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ANOMALUS_ACTION_CHAOTIC_RIFT:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    // create a rift then set shield up and finally charge rift
                    if (DoCastSpellIfCan(m_creature, SPELL_CREATE_RIFT, CAST_TRIGGERED) == CAST_OK)
                    {
                        // emotes are in this order
                        DoScriptText(SAY_SHIELD, m_creature);
                        DoScriptText(EMOTE_SHIELD, m_creature);
                        DoScriptText(EMOTE_OPEN_RIFT, m_creature);

                        DoCastSpellIfCan(m_creature, SPELL_RIFT_SHIELD, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_CHARGE_RIFT, CAST_TRIGGERED);

                        SetActionReadyStatus(action, false);
                        ResetCombatAction(ANOMALUS_ACTION_RESUME_COMBAT, 45000);
                        DisableCombatAction(ANOMALUS_ACTION_SPARK);
                        DisableCombatAction(ANOMALUS_ACTION_CREATE_RIFT);
                    }
                }
                break;
            case ANOMALUS_ACTION_SPARK:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_SPARK : SPELL_SPARK_H) == CAST_OK)
                        ResetCombatAction(action, urand(8000, 10000));
                }
                break;
            case ANOMALUS_ACTION_CREATE_RIFT:
                if (DoCastSpellIfCan(m_creature, SPELL_CREATE_RIFT) == CAST_OK)
                {
                    DoScriptText(SAY_RIFT, m_creature);
                    DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                    ResetCombatAction(action, 25000);
                }
                break;
            case ANOMALUS_ACTION_RESUME_COMBAT:
                DoResumeCombat();
                DisableCombatAction(action);
                break;
        }
    }
};

/*######
## spell_charge_rifts - 47747
######*/

struct spell_charge_rifts : public SpellScript
{
    void OnRadiusCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, bool /*targetB*/, float& radius) const override
    {
        radius = 50.0f;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        // remove standard damage and summon auras
        target->RemoveAurasDueToSpell(47687);
        target->RemoveAurasDueToSpell(47732);

        // cast charged damage and summon auras
        target->CastSpell(target, 47733, TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 47742, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_anomalus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_anomalus";
    pNewScript->GetAI = &GetNewAIInstance<boss_anomalusAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_charge_rifts>("spell_charge_rifts");
}
