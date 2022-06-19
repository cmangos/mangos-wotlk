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
SDName: boss_hummel
SD%Complete: 100
SDComment: The sub-bosses are handled in eventAI
SDCategory: Shadowfang Keep
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "shadowfang_keep.h"

enum
{
    SAY_INTRO_1                     = -1033020,
    SAY_INTRO_2                     = -1033021,
    SAY_INTRO_3                     = -1033022,
    SAY_CALL_BAXTER                 = -1033023,
    SAY_CALL_FRYE                   = -1033024,
    SAY_HUMMEL_DEATH                = -1033025,

    SPELL_ALLURING_PERFUME          = 68589,
    SPELL_ALLURING_PERFUME_SPRAY    = 68607,
    SPELL_CHAIN_REACTION            = 68821,
    SPELL_SUMMON_TABLE              = 69218,
    SPELL_PERMANENT_FEIGN_DEATH     = 29266,
    SPELL_SUMMON_VALENTINE_ADD      = 68610,        // summons the crazy apothecary

    QUEST_BEEN_SERVED               = 14488,

    FACTION_HOSTILE                 = 14,
};

enum HummelActions
{
    HUMMEL_ACTION_PERFUME_SPRAY,
    HUMMEL_ACTION_CHAIN_REACTION,
    HUMMEL_ACTION_SUMMON_ADD,
    HUMMEL_ACTION_MAX,
};

/*######
## boss_apothecary_hummel
######*/

struct boss_apothecary_hummelAI : public CombatAI
{
    boss_apothecary_hummelAI(Creature* creature) : CombatAI(creature, HUMMEL_ACTION_MAX), m_instance(static_cast<instance_shadowfang_keep*>(creature->GetInstanceData()))
    {
        AddCombatAction(HUMMEL_ACTION_PERFUME_SPRAY, 4000u);
        AddCombatAction(HUMMEL_ACTION_CHAIN_REACTION, 15000u);
        AddCombatAction(HUMMEL_ACTION_SUMMON_ADD, 15000u);

        DoCastSpellIfCan(m_creature, SPELL_ALLURING_PERFUME, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);

        m_bIsFakingDeath = false;
    }

    instance_shadowfang_keep* m_instance;

    bool m_bIsFakingDeath;

    void JustRespawned() override
    {
        DoCastSpellIfCan(m_creature, SPELL_ALLURING_PERFUME, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& uiDamage, DamageEffectType /*damagetype*/, SpellEntry const* /*spellInfo*/) override
    {
        if (m_bIsFakingDeath)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;

            DoScriptText(SAY_HUMMEL_DEATH, m_creature);

            DoCastSpellIfCan(m_creature, SPELL_PERMANENT_FEIGN_DEATH, CAST_TRIGGERED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);

            SetCombatScriptStatus(true);
            m_creature->SetTarget(nullptr);
            m_creature->SetHealth(1);

            DisableCombatAction(HUMMEL_ACTION_PERFUME_SPRAY);
            DisableCombatAction(HUMMEL_ACTION_CHAIN_REACTION);
            DisableCombatAction(HUMMEL_ACTION_SUMMON_ADD);

            m_bIsFakingDeath = true;

            if (m_instance)
                m_instance->SetData(TYPE_APOTHECARY, SPECIAL);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case HUMMEL_ACTION_PERFUME_SPRAY:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ALLURING_PERFUME_SPRAY) == CAST_OK)
                    ResetCombatAction(action, 5000);
                break;
            case HUMMEL_ACTION_CHAIN_REACTION:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_REACTION) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_TABLE, CAST_TRIGGERED);
                    ResetCombatAction(action, 25000);
                }
                break;
            case HUMMEL_ACTION_SUMMON_ADD:
                if (m_instance)
                {
                    if (Creature* pGenerator = m_instance->GetSingleCreatureFromStorage(NPC_APOTHECARY_GENERATOR))
                        pGenerator->CastSpell(pGenerator, SPELL_SUMMON_VALENTINE_ADD, TRIGGERED_OLD_TRIGGERED);

                    ResetCombatAction(action, 15000);
                }
                break;
        }
    }
};

bool QuestRewarded_boss_apothecary_hummel(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BEEN_SERVED)
        if (instance_shadowfang_keep* pInstance = static_cast<instance_shadowfang_keep*>(pCreature->GetInstanceData()))
            if (Creature* pValentineMgr = pInstance->GetSingleCreatureFromStorage(NPC_VALENTINE_BOSS_MGR))
                pCreature->AI()->SendAIEvent(AI_EVENT_START_EVENT, pPlayer, pValentineMgr);

    return true;
}

static const DialogueEntry aIntroDialogue[] =
{
    {QUEST_BEEN_SERVED, 0,          1000},
    {SAY_INTRO_1,       NPC_HUMMEL, 4000},
    {SAY_INTRO_2,       NPC_HUMMEL, 4000},
    {SAY_INTRO_3,       NPC_HUMMEL, 3000},
    {NPC_HUMMEL,        0,          8000},
    {NPC_BAXTER,        0,          8000},
    {NPC_FRYE,          0,          0},
    {0, 0, 0},
};

struct npc_valentine_boss_managerAI : public ScriptedAI, private DialogueHelper
{
    npc_valentine_boss_managerAI(Creature* pCreature) : ScriptedAI(pCreature),
        DialogueHelper(aIntroDialogue)
    {
        m_instance = static_cast<instance_shadowfang_keep*>(pCreature->GetInstanceData());
        InitializeDialogueHelper(m_instance);
        Reset();
    }

    instance_shadowfang_keep* m_instance;

    ObjectGuid m_eventStarterGuid;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 /*uiMiscValue*/) override
    {
        if (eventType == AI_EVENT_START_EVENT)
        {
            m_eventStarterGuid = pInvoker->GetObjectGuid();
            DoStartValentineEvent();
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        if (!m_instance)
            return;

        switch (iEntry)
        {
            case NPC_HUMMEL:
            {
                if (Creature* pHummel = m_instance->GetSingleCreatureFromStorage(NPC_HUMMEL))
                {
                    pHummel->SetImmuneToPlayer(false);
                    pHummel->SetImmuneToNPC(false);

                    if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_eventStarterGuid))
                        pHummel->AI()->AttackStart(pPlayer);
                }

                m_instance->SetData(TYPE_APOTHECARY, IN_PROGRESS);
                break;
            }
            case NPC_BAXTER:
            {
                Creature* pHummel = m_instance->GetSingleCreatureFromStorage(NPC_HUMMEL);
                if (!pHummel)
                    return;

                if (Creature* pBaxter = m_instance->GetSingleCreatureFromStorage(NPC_BAXTER))
                {
                    DoScriptText(SAY_CALL_BAXTER, pHummel);

                    pBaxter->SetImmuneToPlayer(false);
                    pBaxter->SetImmuneToNPC(false);

                    if (pHummel->GetVictim())
                        pBaxter->AI()->AttackStart(pHummel->GetVictim());
                }
                break;
            }
            case NPC_FRYE:
            {
                Creature* pHummel = m_instance->GetSingleCreatureFromStorage(NPC_HUMMEL);
                if (!pHummel)
                    return;

                if (Creature* pFrye = m_instance->GetSingleCreatureFromStorage(NPC_FRYE))
                {
                    DoScriptText(SAY_CALL_FRYE, pHummel);

                    pFrye->SetImmuneToPlayer(false);
                    pFrye->SetImmuneToNPC(false);

                    if (pHummel->GetVictim())
                        pFrye->AI()->AttackStart(pHummel->GetVictim());
                }
                break;
            }
        }
    }

    // Wrapper to get the event started
    void DoStartValentineEvent()
    {
        if (!m_instance)
            return;

        StartNextDialogueText(QUEST_BEEN_SERVED);

        if (Creature* pHummel = m_instance->GetSingleCreatureFromStorage(NPC_HUMMEL))
        {
            pHummel->SetImmuneToPlayer(true);
            pHummel->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN);
        }

        // Move Baxter to position
        if (Creature* pBaxter = m_instance->GetSingleCreatureFromStorage(NPC_BAXTER))
        {
            pBaxter->SetImmuneToPlayer(true);
            pBaxter->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN);
            pBaxter->SetWalk(false);
            pBaxter->GetMotionMaster()->MoveWaypoint();
        }

        // Move Frye to position
        if (Creature* pFrye = m_instance->GetSingleCreatureFromStorage(NPC_FRYE))
        {
            pFrye->SetImmuneToPlayer(true);
            pFrye->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN);
            pFrye->SetWalk(false);
            pFrye->GetMotionMaster()->MoveWaypoint();
        }

        // despawn the other Apothecaries
        GuidList lApothecaryList;
        m_instance->GetCrownApothecaryGuids(lApothecaryList);

        for (const auto& guid : lApothecaryList)
            if (Creature* pApothecary = m_creature->GetMap()->GetCreature(guid))
                pApothecary->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);
    }
};

/*######
## spell_lingering_fumes_targetting - 68965
######*/

struct spell_lingering_fumes_targetting : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        caster->CastSpell(target, 68799, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(target, 68644, TRIGGERED_OLD_TRIGGERED);
    }

    void OnAfterHit(Spell* spell) const override
    {
        if (urand(0, 1))
            return;

        Unit* caster = spell->GetCaster();
        if (!caster)
            return;

        if (Creature* bunny = GetClosestCreatureWithEntry(caster, NPC_VALENTINE_VIAL_BUNNY, 100.0f))
            caster->GetMotionMaster()->MovePoint(0, bunny->GetPosition());
    }
};

/*######
## spell_valentine_boss_validate_area - 68644
######*/

struct spell_valentine_boss_validate_area : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster || target->HasAura(69039))
            return;

        // target triggers spell Valentine Boss 3 Throw Vial A or Valentine Boss 3 Throw Vial B
        caster->CastSpell(caster, 69039, TRIGGERED_OLD_TRIGGERED);
        caster->CastSpell(target, urand(0, 1) ? 68841 : 68799, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_valentine_trigger_vial_a - 68966
######*/

struct spell_valentine_trigger_vial_a : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        caster->CastSpell(target, 68798, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_valentine_trigger_vial_b - 69038
######*/

struct spell_valentine_trigger_vial_b : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return;

        Unit* caster = spell->GetCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        caster->CastSpell(target, 68614, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_alluring_perfume_spill_aura - 68798
######*/

struct spell_alluring_perfume_spill_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!target || !caster)
            return;

        caster->CastSpell(target, 68927, TRIGGERED_OLD_TRIGGERED);
    }
};

/*######
## spell_irresistible_cologne_spill_aura - 68614
######*/

struct spell_irresistible_cologne_spill_aura : public AuraScript
{
    void OnPeriodicDummy(Aura* aura) const override
    {
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!target || !caster)
            return;

        caster->CastSpell(target, 68934, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_hummel()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_apothecary_hummel";
    pNewScript->GetAI = &GetNewAIInstance<boss_apothecary_hummelAI>;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_boss_apothecary_hummel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_valentine_boss_manager";
    pNewScript->GetAI = &GetNewAIInstance<npc_valentine_boss_managerAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_lingering_fumes_targetting>("spell_lingering_fumes_targetting");
    RegisterSpellScript<spell_valentine_boss_validate_area>("spell_valentine_boss_validate_area");
    RegisterSpellScript<spell_valentine_trigger_vial_a>("spell_valentine_trigger_vial_a");
    RegisterSpellScript<spell_valentine_trigger_vial_b>("spell_valentine_trigger_vial_b");
    RegisterSpellScript<spell_alluring_perfume_spill_aura>("spell_alluring_perfume_spill_aura");
    RegisterSpellScript<spell_irresistible_cologne_spill_aura>("spell_irresistible_cologne_spill_aura");
}
