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
SDName: Boss_Svala
SD%Complete: 80%
SDComment: The way spells for intro work could use more research.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "utgarde_pinnacle.h"
#include "Spells/SpellAuras.h"
#include "Spells/Scripts/SpellScript.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,

    NPC_CHANNELER               = 27281,        // partially scripted in EAI
    NPC_SCOURGE_HULK            = 26555,        // used to check the achiev

    SPELL_RITUAL_OF_SWORD       = 48276,        // teleports the boss
    SPELL_RITUAL_STRIKE         = 48331,
    SPELL_RITUAL_DISARM         = 54159,
    SPELL_RITUAL_SELECTION      = 54148,        // triggers 48267
    SPELL_CALL_FLAMES           = 48258,        // sends event 17841 - this makes npc 27273 cast 48246
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,

    SPELL_SUMMON_CHANNELER_1    = 48271,
    SPELL_SUMMON_CHANNELER_2    = 48274,
    SPELL_SUMMON_CHANNELER_3    = 48275,

    // spells used by channelers
    SPELL_PARALIZE              = 48278,        // should apply effect 48267 on target
    SPELL_SHADOWS_IN_THE_DARK   = 59407,
    // SPELL_SIMPLE_TELEPORT    = 12980,

    SPELL_BALL_OF_FLAME         = 48246,
};

enum SvalaActions
{
    SVALA_ACTION_SINISTER_STRIKE,
    SVALA_ACTION_CALL_FLAMES,
    SVALA_ACTION_RITUAL_STRIKE,
    SVALA_ACTION_RITUAL_OF_SWORD,
    SVALA_ACTION_MAX,
};

/*######
## boss_svala
######*/

struct boss_svalaAI : public CombatAI
{
    boss_svalaAI(Creature* creature) : CombatAI(creature, SVALA_ACTION_MAX), m_instance(static_cast<instance_pinnacle*>(creature->GetInstanceData()))
    {
        AddCombatAction(SVALA_ACTION_SINISTER_STRIKE, 7000u);
        AddCombatAction(SVALA_ACTION_CALL_FLAMES, 10000u);
        AddCombatAction(SVALA_ACTION_RITUAL_STRIKE, true);
        AddTimerlessCombatAction(SVALA_ACTION_RITUAL_OF_SWORD, true);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    instance_pinnacle* m_instance;
    bool m_isRegularMode;

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_SVALA, FAIL);

        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->SetLevitate(false);
        DoScriptText(SAY_AGGRO, m_creature);

        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Note: creature is partially scripted in EAI
        if (pSummoned->GetEntry() == NPC_CHANNELER)
        {
            if (!m_isRegularMode)
                pSummoned->CastSpell(pSummoned, SPELL_SHADOWS_IN_THE_DARK, TRIGGERED_OLD_TRIGGERED);

            // cast paralize; the spell will automatically pick the target with aura 48267
            pSummoned->CastSpell(pSummoned, SPELL_PARALIZE, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        CombatAI::KilledUnit(victim);

        // set achiev to true if boss kills a hulk
        if (victim->GetEntry() == NPC_SCOURGE_HULK && m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_INCREDIBLE_HULK, true);

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
            m_instance->SetData(TYPE_SVALA, DONE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case SVALA_ACTION_SINISTER_STRIKE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), m_isRegularMode ? SPELL_SINISTER_STRIKE : SPELL_SINISTER_STRIKE_H) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            case SVALA_ACTION_CALL_FLAMES:
                if (DoCastSpellIfCan(m_creature, SPELL_CALL_FLAMES) == CAST_OK)
                    ResetCombatAction(action, urand(15000, 20000));
                break;
            case SVALA_ACTION_RITUAL_STRIKE:
                if (DoCastSpellIfCan(m_creature, SPELL_RITUAL_STRIKE, CAST_TRIGGERED) == CAST_OK)
                    DisableCombatAction(action);
                break;
            case SVALA_ACTION_RITUAL_OF_SWORD:
                if (m_creature->GetHealthPercent() < 50.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_RITUAL_SELECTION) == CAST_OK)
                    {
                        // summon channelers
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_1, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_2, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_CHANNELER_3, CAST_TRIGGERED);

                        // disarm and teleport the boss
                        DoCastSpellIfCan(m_creature, SPELL_RITUAL_DISARM, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_RITUAL_OF_SWORD, CAST_TRIGGERED);

                        switch (urand(0, 3))
                        {
                            case 0: DoScriptText(SAY_SACRIFICE_1, m_creature); break;
                            case 1: DoScriptText(SAY_SACRIFICE_2, m_creature); break;
                            case 2: DoScriptText(SAY_SACRIFICE_3, m_creature); break;
                            case 3: DoScriptText(SAY_SACRIFICE_4, m_creature); break;
                        }

                        SetActionReadyStatus(action, false);
                        ResetCombatAction(SVALA_ACTION_RITUAL_STRIKE, 1000);
                    }
                }
                break;
        }
    }
};

bool ProcessEventId_event_spell_call_flames(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    if (instance_pinnacle* pInstance = static_cast<instance_pinnacle*>(static_cast<Creature*>(pSource)->GetInstanceData()))
    {
        if (Creature* pBrazier = pInstance->instance->GetCreature(pInstance->GetRandomBrazier()))
            pBrazier->CastSpell(pBrazier, SPELL_BALL_OF_FLAME, TRIGGERED_OLD_TRIGGERED);

        return true;
    }
    return false;
}

/*######
## spell_ritual_of_the_sword - 54148
######*/

struct spell_ritual_of_the_sword : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* target = spell->GetUnitTarget();
        Unit* caster = spell->GetCaster();
        if (!target || !caster)
            return;

        // teleport target to the given location and stun it using spell
        caster->CastSpell(target, 48267, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_svala_transforming - 54205
######*/

struct spell_svala_transforming : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsCreature())
            return;

        // remove aura based on effect value
        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(effIdx);
        target->RemoveAurasDueToSpell(spellId);
    }
};

void AddSC_boss_svala()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_svala";
    pNewScript->GetAI = &GetNewAIInstance<boss_svalaAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_call_flames";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_call_flames;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_ritual_of_the_sword>("spell_ritual_of_the_sword");
    RegisterSpellScript<spell_svala_transforming>("spell_svala_transforming");
}
