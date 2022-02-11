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
SDName: Black_Temple
SD%Complete: 0
SDComment: Placeholder
SDCategory: Black Temple
EndScriptData */

/* ContentData
npc_greater_shadowfiend
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "black_temple.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

/*######
## npc_greater_shadowfiend
######*/

#define SPELL_SHADOWFORM    34429

struct npc_greater_shadowfiend : public ScriptedAI
{
    npc_greater_shadowfiend(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() override
    {
        DoCastSpellIfCan(m_creature, SPELL_SHADOWFORM);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        m_creature->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            m_creature->AddThreat(pTarget, 100000.f);
    }
};

enum
{
    SPELL_COMBAT_RAGE   = 41251,
    SPELL_FRENZY        = 41254,
    SPELL_ENRAGE        = 8269,

    NPC_BONECHEWER_BRAWLER      = 23222,
    NPC_BONECHEWER_COMBATANT    = 23239,


    SAY_EMOTE_EXPIRE    = -1564143,
    SAY_EMOTE_RISE      = -1564144,
    SAY_KILLING_FRENZY  = -1564145,
};

enum BonechewerActions
{
    BONECHEWER_STOP_EVENT,
    BONECHEWER_ENRAGE,
    BONECHEWER_FRENZY,
    BONECHEWER_ACTION_MAX,
    BONECHEWER_START_EVENT,
};

struct npc_bonechewer_brawler : public CombatAI
{
    npc_bonechewer_brawler(Creature* creature) : CombatAI(creature, BONECHEWER_ACTION_MAX)
    {
        AddTimerlessCombatAction(BONECHEWER_STOP_EVENT, true);
        AddTimerlessCombatAction(BONECHEWER_ENRAGE, true);
        AddCombatAction(BONECHEWER_FRENZY, 7000, 14000);
    }

    void Reset() override
    {
        CombatAI::Reset();
        m_eventStarted = false;
    }

    bool m_eventStarted;

    void JustReachedHome() override
    {
        CombatAI::JustReachedHome();
        DoScriptText(SAY_EMOTE_RISE, m_creature);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            m_creature->SetFactionTemporary(1693, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
            AttackStart(invoker);
            m_eventStarted = true;
            DisableCombatAction(BONECHEWER_FRENZY);
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->getThreatManager().modifyThreatPercent(invoker, -101);
            m_creature->ClearTemporaryFaction();
            m_eventStarted = false;
            ResetCombatAction(BONECHEWER_FRENZY, urand(7000, 14000));
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BONECHEWER_STOP_EVENT:
            {
                if (m_eventStarted && m_creature->GetHealthPercent() <= 40.f)
                {
                    EnterEvadeMode();
                }
                break;
            }
            case BONECHEWER_ENRAGE:
            {
                if (m_creature->GetHealthPercent() > 30.f)
                    break;

                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            }
            case BONECHEWER_FRENZY:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_FRENZY) == CAST_OK)
                {
                    ResetCombatAction(action, urand(21000, 28000));
                    DoScriptText(SAY_KILLING_FRENZY, m_creature);
                }
                break;
            }
        }
    }
};

struct npc_bonechewer_combatant : public CombatAI
{
    npc_bonechewer_combatant(Creature* creature) : CombatAI(creature, BONECHEWER_ACTION_MAX)
    {
        AddTimerlessCombatAction(BONECHEWER_STOP_EVENT, true);
        AddTimerlessCombatAction(BONECHEWER_ENRAGE, true);
        AddCombatAction(BONECHEWER_FRENZY, 5000, 7000);
        AddCustomAction(BONECHEWER_START_EVENT, 25000u, [&]() { HandleEventStart(); });
    }

    void Reset() override
    {
        CombatAI::Reset();
        m_eventStarted = false;
    }

    bool m_eventStarted;

    void JustReachedHome() override
    {
        CombatAI::JustReachedHome();
        DoScriptText(SAY_EMOTE_EXPIRE, m_creature);
    }

    void HandleEventStart()
    {
        if (m_creature->IsInCombat())
            return;

        if (Creature* brawler = GetClosestCreatureWithEntry(m_creature, NPC_BONECHEWER_BRAWLER, 30.f))
        {
            m_creature->SetFactionTemporary(1692, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
            SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, brawler);
            m_eventStarted = true;
            DisableCombatAction(BONECHEWER_FRENZY);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case BONECHEWER_STOP_EVENT:
            {
                if (m_eventStarted && m_creature->GetHealthPercent() <= 40.f)
                {
                    EnterEvadeMode();
                }
                break;
            }
            case BONECHEWER_ENRAGE:
            {
                if (m_creature->GetHealthPercent() > 30.f)
                    break;

                if (DoCastSpellIfCan(nullptr, SPELL_ENRAGE) == CAST_OK)
                    SetActionReadyStatus(action, false);
                break;
            }
            case BONECHEWER_FRENZY:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_COMBAT_RAGE) == CAST_OK)
                    ResetCombatAction(action, 10000);
                break;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        CombatAI::UpdateAI(diff);
        if (m_creature->IsInCombat() && m_eventStarted)
        {
            if (m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (Creature* brawler = GetClosestCreatureWithEntry(m_creature, NPC_BONECHEWER_BRAWLER, 30.f))
                {
                    SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, brawler);
                    m_creature->getThreatManager().modifyThreatPercent(brawler, -101);
                    m_creature->ClearTemporaryFaction();
                    m_eventStarted = false;
                    ResetCombatAction(BONECHEWER_FRENZY, urand(5000, 7000));
                }
            }
        }
    }
};

enum
{
    SPELL_CHAOTIC_CHARGE = 41033,
};

struct SpellAbsorption : public AuraScript
{
    bool OnCheckProc(Aura* /*aura*/, ProcExecutionData& data) const override
    {
		return data.spell && !data.spell->m_spellInfo->HasAttribute(SPELL_ATTR_ABILITY);
    }

    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.basePoints[0] = data.basePoints[0] * aura->GetTarget()->GetAuraCount(SPELL_CHAOTIC_CHARGE);
        if (!data.basePoints[0])
            data.spellInfo = nullptr;
    }
};

enum
{
    SPELL_FOCUSED = 40085,
};

struct HarpoonersMark : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply)
            aura->GetTarget()->CastSpell(nullptr, SPELL_FOCUSED, TRIGGERED_OLD_TRIGGERED);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_FOCUSED, TRIGGERED_OLD_TRIGGERED);
    }
};

struct AssistBT : public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (target->HasAura(40893) || target->GetHealthPercent() < 20.f)
            return false;
        return true;
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || !unitTarget->IsInCombat())
            return;

        unitTarget->CastSpell(spell->GetCaster(), 40892, TRIGGERED_OLD_TRIGGERED);
    }
};

struct FixateBT : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_2)
            return;

        Unit* unitTarget = spell->GetUnitTarget();
        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        spell->GetCaster()->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, unitTarget, spell->GetCaster());
        unitTarget->CastSpell(spell->GetCaster(), spell->m_spellInfo->CalculateSimpleValue(effIdx), TRIGGERED_OLD_TRIGGERED);
    }
};

struct StormBlink : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (!spell->GetUnitTarget() || effIdx != EFFECT_INDEX_0)
            return;
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), 39582, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_black_temple()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_greater_shadowfiend";
    pNewScript->GetAI = &GetNewAIInstance<npc_greater_shadowfiend>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bonechewer_brawler";
    pNewScript->GetAI = &GetNewAIInstance<npc_bonechewer_brawler>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bonechewer_combatant";
    pNewScript->GetAI = &GetNewAIInstance<npc_bonechewer_combatant>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<SpellAbsorption>("spell_spell_absorption");
    RegisterSpellScript<HarpoonersMark>("spell_harpooners_mark");
    RegisterSpellScript<AssistBT>("spell_assist_bt");
    RegisterSpellScript<FixateBT>("spell_fixate_bt");
    RegisterSpellScript<StormBlink>("spell_storm_blink");
}
