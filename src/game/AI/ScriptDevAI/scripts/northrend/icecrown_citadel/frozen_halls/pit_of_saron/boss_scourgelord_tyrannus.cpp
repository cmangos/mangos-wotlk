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
SDName: boss_scourgelord_tyrannus
SD%Complete: 80
SDComment: Overlord's Brand logic not impelemneted
SDCategory: Pit of Saron
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "pit_of_saron.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_AGGRO                           = -1658053,
    SAY_SLAY_1                          = -1658054,
    SAY_SLAY_2                          = -1658055,
    SAY_DEATH                           = -1658056,
    SAY_MARK                            = -1658057,
    SAY_SMASH                           = -1658058,

    EMOTE_RIMEFANG_ICEBOLT              = -1658059,
    EMOTE_SMASH                         = -1658060,

    // Tyrannus spells
    SPELL_FORCEFUL_SMASH                = 69155,
    SPELL_OVERLORDS_BRAND               = 69172,                // triggers 69189 and 69190 from target
    SPELL_UNHOLY_POWER                  = 69167,
    SPELL_MARK_OF_RIMEFANG              = 69275,

    // Rimefang spells
    SPELL_HOARFROST                     = 69246,
    SPELL_ICY_BLAST                     = 69232,
    SPELL_KILLING_ICE                   = 72531,

    // Icy blast
    SPELL_ICY_BLAST_AURA                = 69238,
    NPC_ICY_BLAST                       = 36731,                // summoned by missing spell 69234
};

static const float afRimefangExitPos[3] = {1248.29f, 145.924f, 733.914f};

/*######
## boss_tyrannus
######*/

enum TyrannusActions
{
    TYRANNUS_FORCEFUL_SMASH,
    TYRANNUS_OVERLORDS_BRAND,
    TYRANNUS_UNHOLY_POWER,
    TYRANNUS_MARK_RIMEFANG,
    TYRANNUS_ACTION_MAX,
};

struct boss_tyrannusAI : public CombatAI
{
    boss_tyrannusAI(Creature* creature) : CombatAI(creature, TYRANNUS_ACTION_MAX), m_instance(static_cast<instance_pit_of_saron*>(creature->GetInstanceData()))
    {
        AddCombatAction(TYRANNUS_FORCEFUL_SMASH, 15000u);
        AddCombatAction(TYRANNUS_OVERLORDS_BRAND, 9000u);
        AddCombatAction(TYRANNUS_UNHOLY_POWER, 15000u, 20000u);
        AddCombatAction(TYRANNUS_MARK_RIMEFANG, 20000u, 25000u);
    }

    instance_pit_of_saron* m_instance;

    void Aggro(Unit* who) override
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_TYRANNUS, IN_PROGRESS);

            // Set Rimefang in combat - ToDo: research if it has some wp movement during combat
            if (Creature* pRimefang = m_instance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
                pRimefang->AI()->AttackStart(who);
        }
    }

    void KilledUnit(Unit* victim)
    {
        CombatAI::KilledUnit(victim);

        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* killer) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
        {
            m_instance->SetData(TYPE_TYRANNUS, DONE);

            if (Creature* pCreature = m_instance->GetSingleCreatureFromStorage(m_instance->GetPlayerTeam() == HORDE ? NPC_SYLVANAS_PART1 : NPC_JAINA_PART1))
                pCreature->ForcedDespawn();

            // Move Rimefang out of the area
            if (Creature* pRimefang = m_instance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
            {
                pRimefang->AI()->EnterEvadeMode();
                pRimefang->SetWalk(false);
                pRimefang->ForcedDespawn(15000);
                pRimefang->GetMotionMaster()->Clear(false, true);
                pRimefang->GetMotionMaster()->MovePoint(0, afRimefangExitPos[0], afRimefangExitPos[1], afRimefangExitPos[2]);
            }
        }
    }

    void JustReachedHome() override
    {
        if (m_instance)
            m_instance->SetData(TYPE_TYRANNUS, FAIL);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TYRANNUS_FORCEFUL_SMASH:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FORCEFUL_SMASH) == CAST_OK)
                    ResetCombatAction(action, urand(40000, 50000));
                break;
            case TYRANNUS_OVERLORDS_BRAND:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
                    if (DoCastSpellIfCan(target, SPELL_OVERLORDS_BRAND) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 13000));
                }
                break;
            case TYRANNUS_UNHOLY_POWER:
                if (DoCastSpellIfCan(m_creature, SPELL_UNHOLY_POWER) == CAST_OK)
                {
                    DoScriptText(SAY_SMASH, m_creature);
                    DoScriptText(EMOTE_SMASH, m_creature);
                    ResetCombatAction(action, urand(45000, 50000));
                }
                break;
            case TYRANNUS_MARK_RIMEFANG:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_MARK_OF_RIMEFANG) == CAST_OK)
                    {
                        DoScriptText(SAY_MARK, m_creature);
                        if (m_instance)
                        {
                            if (Creature* rimefang = m_instance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
                                SendAIEvent(AI_EVENT_CUSTOM_A, target, rimefang);
                        }
                        ResetCombatAction(action, urand(20000, 25000));
                    }
                }
                break;
        }
    }
};

/*######
## boss_rimefang_pos
######*/

enum RimefangPoSActions
{
    RIMEFANG_POS_ICY_BLAST,
    RIMEFANG_POS_HOARFROST,
    RIMEFANG_POS_ACTION_MAX,
};

struct boss_rimefang_posAI : public CombatAI
{
    boss_rimefang_posAI(Creature* creature) : CombatAI(creature, RIMEFANG_POS_ACTION_MAX), m_instance(static_cast<instance_pit_of_saron*>(creature->GetInstanceData()))
    {
        AddCombatAction(RIMEFANG_POS_ICY_BLAST, 5000u);
        AddCombatAction(RIMEFANG_POS_HOARFROST, true);

        SetCombatMovement(false);
    }

    instance_pit_of_saron* m_instance;

    ObjectGuid m_hoarfrostTarget;

    void AttackStart(Unit* who) override
    {
        // Don't attack unless Tyrannus is in combat or Ambush is completed
        if (m_instance && (m_instance->GetData(TYPE_AMBUSH) != DONE || m_instance->GetData(TYPE_TYRANNUS) != IN_PROGRESS))
            return;

        CombatAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!m_instance || !who->IsPlayer())
            return;

        if (who->IsInCombat())
            return;

        // Check if ambush is done
        if (m_instance->GetData(TYPE_AMBUSH) != DONE)
            return;

        // Start the intro when possible
        if (m_instance->GetData(TYPE_TYRANNUS) == NOT_STARTED && m_creature->IsWithinDistInMap(who, 85.0f) && m_creature->IsWithinLOSInMap(who))
            m_instance->SetData(TYPE_TYRANNUS, SPECIAL);

        // Check for out of range players - ToDo: confirm the distance
        if (m_instance->GetData(TYPE_TYRANNUS) == IN_PROGRESS && !m_creature->IsWithinDistInMap(who, DEFAULT_VISIBILITY_INSTANCE))
            DoCastSpellIfCan(who, SPELL_KILLING_ICE);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_ICY_BLAST)
        {
            summoned->AI()->SetReactState(REACT_PASSIVE);
            summoned->SetCanEnterCombat(false);
            summoned->CastSpell(summoned, SPELL_ICY_BLAST_AURA, TRIGGERED_OLD_TRIGGERED);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && sender->GetEntry() == NPC_TYRANNUS)
        {
            m_hoarfrostTarget = invoker->GetObjectGuid();
            ResetCombatAction(RIMEFANG_POS_HOARFROST, 1000);
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case RIMEFANG_POS_ICY_BLAST:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(target, SPELL_ICY_BLAST) == CAST_OK)
                        ResetCombatAction(action, 8000);
                }
                break;
            case RIMEFANG_POS_HOARFROST:
                if (Unit* target = m_creature->GetMap()->GetUnit(m_hoarfrostTarget))
                {
                    if (DoCastSpellIfCan(target, SPELL_HOARFROST) == CAST_OK)
                    {
                        DoScriptText(EMOTE_RIMEFANG_ICEBOLT, m_creature, target);
                        DisableCombatAction(action);
                    }
                }
                break;
        }
    }
};

/*######
## spell_icy_blast - 69232
######*/

struct spell_icy_blast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return;

        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        caster->SummonCreature(NPC_ICY_BLAST, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSPAWN_CORPSE_TIMED_DESPAWN, 30000);
    }
};

void AddSC_boss_tyrannus()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_tyrannus";
    pNewScript->GetAI = &GetNewAIInstance<boss_tyrannusAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_rimefang_pos";
    pNewScript->GetAI = &GetNewAIInstance<boss_rimefang_posAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_icy_blast>("spell_icy_blast");
}
