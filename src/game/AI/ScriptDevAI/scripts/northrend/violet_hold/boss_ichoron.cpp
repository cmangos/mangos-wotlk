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
SDName: boss_ichoron
SD%Complete: 100
SDComment:
SDCategory: Violet Hold
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "violet_hold.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO           = -1608019,
    SAY_SHATTERING      = -1608020,
    SAY_SHIELD          = -1608021,
    SAY_SLAY_1          = -1608022,
    SAY_SLAY_2          = -1608023,
    SAY_SLAY_3          = -1608024,
    SAY_ENRAGE          = -1608025,
    SAY_DEATH           = -1608026,
    EMOTE_BUBBLE        = -1608028,

    SPELL_THREAT_PROC           = 61733,
    SPELL_DRAINED               = 59820,
    SPELL_BURST                 = 54379,
    SPELL_SPLATTER              = 54259,
    SPELL_SHRINK                = 54297,
    SPELL_FRENZY                = 54312,
    SPELL_FRENZY_H              = 59522,
    SPELL_PROTECTIVE_BUBBLE     = 54306,
    SPELL_WATER_BLAST           = 54237,
    SPELL_WATER_BLAST_H         = 59520,
    SPELL_WATER_BOLT_VOLLEY     = 54241,
    SPELL_WATER_BOLT_VOLLEY_H   = 59521,

    SPELL_WATER_GLOBULE         = 54260,                // cast by boss on the ichoron globule
    SPELL_WATER_GLOBULE_SPAWN_1 = 54258,
    SPELL_WATER_GLOBULE_SPAWN_2 = 54264,
    SPELL_WATER_GLOBULE_SPAWN_3 = 54265,
    SPELL_WATER_GLOBULE_SPAWN_4 = 54266,
    SPELL_WATER_GLOBULE_SPAWN_5 = 54267,

    // Ichor globule spells
    SPELL_MERGE                 = 54269,                // used by globules
    SPELL_WATER_GLOBULE_TRANS   = 54268,                // triggered by 54260
    SPELL_SPLASH                = 59516,
};

enum IchoronActions
{
    ICHORON_WATER_BLAST,
    ICHORON_WATER_BOLT_VOLLEY,
    ICHORON_FRENZY,
    ICHORON_ACTION_MAX,
};

static const uint32 aWaterGlobuleSpells[5] = {SPELL_WATER_GLOBULE_SPAWN_1, SPELL_WATER_GLOBULE_SPAWN_2, SPELL_WATER_GLOBULE_SPAWN_3, SPELL_WATER_GLOBULE_SPAWN_4, SPELL_WATER_GLOBULE_SPAWN_5};

struct boss_ichoronAI : public CombatAI
{
    boss_ichoronAI(Creature* creature) : CombatAI(creature, ICHORON_ACTION_MAX), m_instance(static_cast<instance_violet_hold*>(creature->GetInstanceData()))
    {
        DoCastSpellIfCan(m_creature, SPELL_THREAT_PROC);

        m_isRegularMode = creature->GetMap()->IsRegularDifficulty();

        AddCombatAction(ICHORON_WATER_BLAST, 10000u);
        AddCombatAction(ICHORON_WATER_BOLT_VOLLEY, 10000u, 12000u);
        AddTimerlessCombatAction(ICHORON_FRENZY, true);
    }

    instance_violet_hold* m_instance;
    bool m_isRegularMode;

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        DoCastSpellIfCan(m_creature, SPELL_PROTECTIVE_BUBBLE);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        // merge with Ichoron and despawn
        if (who->GetEntry() == NPC_ICHOR_GLOBULE && who->IsWithinDistInMap(m_creature, CONTACT_DISTANCE))
        {
            who->CastSpell(who, SPELL_MERGE, TRIGGERED_OLD_TRIGGERED);
            static_cast<Creature*>(who)->ForcedDespawn();
        }

        CombatAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();

        m_creature->ForcedDespawn();
    }

    void KilledUnit(Unit* who) override
    {
        CombatAI::KilledUnit(who);

        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        // allow events only from the following entries
        switch (sender->GetEntry())
        {
            case NPC_ICHORON:
            case NPC_ICHOR_GLOBULE:
            case NPC_SWIRLING:
                break;
            default:
                return;
        }

        // handle water globules
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            DoScriptText(EMOTE_BUBBLE, m_creature);
            DoScriptText(SAY_SHATTERING, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_BURST, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_SPLATTER, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_DRAINED, CAST_TRIGGERED);

            // reduce health and set unselectable
            m_creature->SetHealthPercent(m_creature->GetHealthPercent() - 30.0f);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            m_creature->SetFeignDeath(true);

            DisableCombatAction(ICHORON_WATER_BLAST);
            DisableCombatAction(ICHORON_WATER_BOLT_VOLLEY);
        }
        // a merg has occurred; end split
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->SetHealthPercent(m_creature->GetHealthPercent() + 3.0f);

            // remove one stack from the shrink aura
            if (Aura* shrinkAura = m_creature->GetAura(SPELL_SHRINK, EFFECT_INDEX_0))
                shrinkAura->GetHolder()->ModStackAmount(-1, m_creature);

            EndSplitPhase();

            // fail achievement
            if (m_instance)
                m_instance->SetData(TYPE_ICHORON, SPECIAL);
        }
        // end split at the end of the Drained aura
        else if (eventType == AI_EVENT_CUSTOM_C)
            EndSplitPhase();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ICHOR_GLOBULE)
        {
            DoCastSpellIfCan(summoned, SPELL_WATER_GLOBULE, CAST_TRIGGERED);

            summoned->AI()->SetCombatMovement(false);
        }
    }

    void SummonedCreatureJustDied(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ICHOR_GLOBULE)
            summoned->CastSpell(summoned, SPELL_SPLASH, TRIGGERED_OLD_TRIGGERED);
    }

    // method to end the split phase
    void EndSplitPhase()
    {
        if (m_creature->HasAura(SPELL_DRAINED))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_DRAINED);
            DoScriptText(SAY_SHIELD, m_creature);
        }

        m_creature->SetFeignDeath(false);

        // apply shield if HP is high enough
        if (m_creature->GetHealthPercent() > 35.0f)
            DoCastSpellIfCan(m_creature, SPELL_PROTECTIVE_BUBBLE, CAST_AURA_NOT_PRESENT);

        // reset timers
        ResetCombatAction(ICHORON_WATER_BLAST, urand(8000, 14000));
        ResetCombatAction(ICHORON_WATER_BOLT_VOLLEY, urand(7000, 12000));

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case ICHORON_WATER_BLAST:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, m_isRegularMode ? SPELL_WATER_BLAST : SPELL_WATER_BLAST_H) == CAST_OK)
                        ResetCombatAction(action, urand(8000, 14000));
                }
                break;
            case ICHORON_WATER_BOLT_VOLLEY:
                if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_WATER_BOLT_VOLLEY : SPELL_WATER_BOLT_VOLLEY_H) == CAST_OK)
                    ResetCombatAction(action, urand(7000, 12000));
                break;
            case ICHORON_FRENZY:
                if (m_creature->GetHealthPercent() < 25.0f)
                {
                    if (DoCastSpellIfCan(m_creature, m_isRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        SetActionReadyStatus(action, false);
                    }
                }
                break;
        }
    }
};

/*######
## spell_protective_bubble_aura - 54306
######*/

struct spell_protective_bubble_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply && aura->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, target);
    }
};

/*######
## spell_drained_aura - 59820
######*/

struct spell_drained_aura : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply)
            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, target, target);
    }
};

/*######
## spell_splatter_aura - 54259
######*/

struct spell_splatter_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        target->CastSpell(target, aWaterGlobuleSpells[urand(0, 4)], TRIGGERED_OLD_TRIGGERED);
        target->CastSpell(target, 54297, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_water_globule_missile - 54260
######*/

struct spell_water_globule_missile : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        target->CastSpell(target, 54268, TRIGGERED_OLD_TRIGGERED);

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        static_cast<Creature*>(target)->SetWalk(false);
        target->GetMotionMaster()->MoveFollow(caster, 0, 0);
    }
};

/*######
## spell_water_globule_merge - 54269
######*/

struct spell_water_globule_merge : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || !caster->IsCreature())
            return;

        // inform boss to resume combat
        caster->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, caster, target);
    }
};

void AddSC_boss_ichoron()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_ichoron";
    pNewScript->GetAI = &GetNewAIInstance<boss_ichoronAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_protective_bubble_aura>("spell_protective_bubble_aura");
    RegisterSpellScript<spell_splatter_aura>("spell_splatter_aura");
    RegisterSpellScript<spell_drained_aura>("spell_drained_aura");
    RegisterSpellScript<spell_water_globule_missile>("spell_water_globule_missile");
    RegisterSpellScript<spell_water_globule_merge>("spell_water_globule_merge");
}
