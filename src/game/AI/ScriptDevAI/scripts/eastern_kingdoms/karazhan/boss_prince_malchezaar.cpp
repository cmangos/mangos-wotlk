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
SDName: Boss_Prince_Malchezzar
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "karazhan.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1532091,
    SAY_AXE_TOSS1               = -1532092,
    SAY_AXE_TOSS2               = -1532093,
    SAY_SPECIAL1                = -1532094,
    SAY_SPECIAL2                = -1532095,
    SAY_SPECIAL3                = -1532096,
    SAY_SLAY1                   = -1532097,
    SAY_SLAY2                   = -1532098,
    SAY_SLAY3                   = -1532099,
    SAY_SUMMON1                 = -1532100,
    SAY_SUMMON2                 = -1532101,
    SAY_DEATH                   = -1532102,

    // Enfeeble is supposed to reduce hp to 1 and then heal player back to full when it ends
    SPELL_ENFEEBLE              = 30843,                    // Enfeeble during phases 1 and 2
    SPELL_ENFEEBLE_EFFECT       = 41624,                    // purpose unk - cast during the transition from phase 2 to 3
    SPELL_SHADOW_NOVA           = 30852,                    // Shadownova used during all phases
    SPELL_SW_PAIN_PHASE1        = 30854,                    // Shadow word pain during phase 1
    SPELL_SW_PAIN_PHASE3        = 30898,                    // Shadow word pain during phase 3
    SPELL_SUNDER_ARMOR          = 30901,                    // Sunder armor during phase 2
    SPELL_THRASH                = 3391,                     // Passive proc chance for thrash
    SPELL_SUMMON_AXES           = 30891,                    // Summon 17650
    SPELL_EQUIP_AXES            = 30857,                    // Visual for axe equiping - transition to phase 2
    SPELL_AMPLIFY_DAMAGE        = 39095,                    // Amplifiy during phase 3 3
    // SPELL_CLEAVE              = 30131,                   // spell not confirmed
    SPELL_INFERNAL_RELAY        = 30834,                    // purpose unk
    SPELL_INFERNAL_RELAY_SUMMON = 30835,                    // triggers 30836, which summons an infernal

    NPC_NETHERSPITE_INFERNAL    = 17646,                    // The netherspite infernal creature
    NPC_MALCHEZARS_AXE          = 17650,                    // Malchezar's axes summoned during phase 3
    //NPC_INFERNAL_RELAY          = 17645,

    EQUIP_ID_AXE                = 23996,                    // Axes info

    ATTACK_TIMER_DEFAULT        = 2000,                     // note: for TBC it was 2400
    ATTACK_TIMER_AXES           = 1333,                     // note: for TBC it was 1600

    MAX_ENFEEBLE_TARGETS        = 5,
};

enum MalchezaarActions
{
    MALCHEZAAR_PHASE_2,
    MALCHEZAAR_PHASE_3,
    MALCHEZAAR_SUNDER_ARMOR,
    MALCHEZAAR_THRASH,
    MALCHEZAAR_AMPLIFY_DAMAGE,
    MALCHEZAAR_INFERNAL,
    MALCHEZAAR_SHADOW_NOVA,
    MALCHEZAAR_SHADOW_WORD_PAIN,
    MALCHEZAAR_ENFEEBLE,
    MALCHEZAAR_ACTION_MAX,
};

struct boss_malchezaarAI : public CombatAI
{
    boss_malchezaarAI(Creature* creature) : CombatAI(creature, MALCHEZAAR_ACTION_MAX), m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData()))
    {
        AddTimerlessCombatAction(MALCHEZAAR_PHASE_2, true);
        AddTimerlessCombatAction(MALCHEZAAR_PHASE_3, true);
        AddCombatAction(MALCHEZAAR_SUNDER_ARMOR, true);
        AddCombatAction(MALCHEZAAR_THRASH, true);
        AddCombatAction(MALCHEZAAR_AMPLIFY_DAMAGE, true);
        AddCombatAction(MALCHEZAAR_INFERNAL, 40000u);
        AddCombatAction(MALCHEZAAR_SHADOW_NOVA, 35500u);
        AddCombatAction(MALCHEZAAR_SHADOW_WORD_PAIN, 20000u);
        AddCombatAction(MALCHEZAAR_ENFEEBLE, 30000u);
        AddOnKillText(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3);
    }

    ScriptedInstance* m_instance;

    ObjectGuid m_uiRelayGuidClose;
    ObjectGuid m_uiRelayGuidFar;

    std::map<uint64, uint32> m_enfleebleMap;

    void Reset() override
    {
        CombatAI::Reset();

        // Reset equipment and attack
        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
        m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_DEFAULT);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        // Remove the summoned axe - which is considered a guardian
        m_creature->RemoveGuardians();

        if (m_instance)
            m_instance->SetData(TYPE_MALCHEZZAR, DONE);
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        /*CreatureList creatureList;
        m_creature->GetMap()->ForceLoadGrid(-10833.1, -2151.58);
        m_creature->GetMap()->ForceLoadGrid(-10893.51, -2081.342);
        GetCreatureListWithEntryInGrid(creatureList,m_creature, NPC_INFERNAL_RELAY,400.0f);*/
        float z = 0;
        if (instance_karazhan* kara = dynamic_cast<instance_karazhan*>(m_instance))
        {
            for (auto& relayGuid : kara->m_vInfernalRelays)
            {
                if (z == 0)
                {
                    Creature* relay = m_creature->GetMap()->GetCreature(relayGuid);
                    m_uiRelayGuidClose = relayGuid;
                    m_uiRelayGuidFar = relayGuid;
                    z = relay->GetPositionZ();
                }
                else
                {
                    Creature* relay = m_creature->GetMap()->GetCreature(relayGuid);
                    if (relay->GetPositionZ() < z)
                    {
                        m_uiRelayGuidClose = relayGuid;
                    }
                    else
                    {
                        m_uiRelayGuidFar = relayGuid;
                    }
                }
            }
        }
        if (m_instance)
            m_instance->SetData(TYPE_MALCHEZZAR, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_MALCHEZZAR, FAIL);

        // Remove the summoned axe - which is considered a guardian
        m_creature->RemoveGuardians();
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_MALCHEZARS_AXE)
        {
            summoned->SetForceAttackingCapability(true); // has to be able to attack even if not selectable or attackable
            summoned->SetInCombatWithZone();
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case MALCHEZAAR_PHASE_2:
            {
                // transition to phase 2
                if (m_creature->GetHealthPercent() < 60.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_EQUIP_AXES, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    {
                        DoScriptText(SAY_AXE_TOSS1, m_creature);

                        SetEquipmentSlots(false, EQUIP_ID_AXE, EQUIP_ID_AXE, EQUIP_NO_CHANGE);
                        m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_AXES);
                        ResetCombatAction(MALCHEZAAR_THRASH, urand(6000, 8000));
                        DisableCombatAction(MALCHEZAAR_SHADOW_WORD_PAIN);
                        ResetCombatAction(MALCHEZAAR_SUNDER_ARMOR, urand(5000, 10000));
                        SetActionReadyStatus(action, false);
                        m_creature->SetCanParry(true);
                    }
                }
                return;
            }
            case MALCHEZAAR_PHASE_3:
            {
                // transition to phase 3
                if (m_creature->GetHealthPercent() < 30.0f)
                {
                    if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_AXES) == CAST_OK)
                    {
                        DoCastSpellIfCan(nullptr, SPELL_ENFEEBLE_EFFECT, CAST_TRIGGERED);
                        DoScriptText(SAY_SPECIAL3, m_creature);

                        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                        m_creature->SetAttackTime(BASE_ATTACK, ATTACK_TIMER_DEFAULT);

                        DisableCombatAction(MALCHEZAAR_THRASH);
                        DisableCombatAction(MALCHEZAAR_ENFEEBLE);
                        DisableCombatAction(MALCHEZAAR_SUNDER_ARMOR);
                        ResetCombatAction(MALCHEZAAR_SHADOW_NOVA, 10000);
                        ResetCombatAction(MALCHEZAAR_AMPLIFY_DAMAGE, 5000);
                        ResetCombatAction(MALCHEZAAR_INFERNAL, 15000);
                        ResetCombatAction(MALCHEZAAR_SHADOW_WORD_PAIN, 5000);
                        m_creature->SetCanParry(false);
                        SetActionReadyStatus(action, false);
                    }
                }
                return;
            }
            case MALCHEZAAR_SUNDER_ARMOR:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SUNDER_ARMOR) == CAST_OK)
                    ResetCombatAction(action, urand(10000, 18000));
                break;
            }
            case MALCHEZAAR_AMPLIFY_DAMAGE:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_AMPLIFY_DAMAGE) == CAST_OK)
                    ResetCombatAction(action, urand(20000, 30000));
                break;
            }
            case MALCHEZAAR_INFERNAL:
            {
                if (Creature* relayClose = m_creature->GetMap()->GetCreature(m_uiRelayGuidClose))
                {
                    if (Creature* relayFar = m_creature->GetMap()->GetCreature(m_uiRelayGuidFar))
                    {
                        relayFar->CastSpell(relayClose, SPELL_INFERNAL_RELAY, TRIGGERED_NONE);
                        DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, m_creature);
                        ResetCombatAction(action, GetActionReadyStatus(MALCHEZAAR_PHASE_3) ? 45000 : 17000);
                    }
                }
                break;
            }
            case MALCHEZAAR_SHADOW_NOVA:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_NOVA) == CAST_OK)
                {
                    if (GetActionReadyStatus(MALCHEZAAR_PHASE_3))
                        ResetCombatAction(action, 30000);
                    else
                        DisableCombatAction(action);
                }
                break;
            }
            case MALCHEZAAR_SHADOW_WORD_PAIN:
            {
                if (DoCastSpellIfCan(GetActionReadyStatus(MALCHEZAAR_PHASE_2) ? m_creature->GetVictim() : nullptr, GetActionReadyStatus(MALCHEZAAR_PHASE_2) ? SPELL_SW_PAIN_PHASE1 : SPELL_SW_PAIN_PHASE3) == CAST_OK)
                    ResetCombatAction(action, 20000);
                break;
            }
            case MALCHEZAAR_ENFEEBLE:
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENFEEBLE) == CAST_OK)
                {
                    ResetCombatAction(action, 30000);
                    ResetCombatAction(MALCHEZAAR_SHADOW_NOVA, 5000);
                }
                break;
            }
            case MALCHEZAAR_THRASH:
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_THRASH) == CAST_OK)
                    ResetCombatAction(action, urand(6000, 8000));
                break;
            }
        }
    }
};

struct Enfeeble : public SpellScript, public AuraScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(5);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        if (spell->GetCaster()->GetVictim() == target)
            return false;

        return true;
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        if (apply)
        {
            if (Unit* caster = aura->GetCaster())
            {
                if (boss_malchezaarAI* ai = dynamic_cast<boss_malchezaarAI*>(caster->AI()))
                {
                    ai->m_enfleebleMap[aura->GetTarget()->GetObjectGuid().GetRawValue()] = aura->GetTarget()->GetHealth();
                    aura->GetTarget()->SetHealth(1);
                }
            }
        }
        else
        {

            if (aura->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                return;

            if (Unit* caster = aura->GetCaster())
                if (boss_malchezaarAI* ai = dynamic_cast<boss_malchezaarAI*>(caster->AI()))
                    aura->GetTarget()->SetHealth(ai->m_enfleebleMap[aura->GetTarget()->GetObjectGuid().GetRawValue()]);
        }
    }
};

struct EnfeebleRemoval : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        target->RemoveAurasDueToSpell(SPELL_ENFEEBLE);
    }
};

void AddSC_boss_prince_malchezaar()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_malchezaar";
    pNewScript->GetAI = &GetNewAIInstance<boss_malchezaarAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<Enfeeble>("spell_enfeeble");
    RegisterSpellScript<EnfeebleRemoval>("spell_enfeeble_removal");
}