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
SDName: boss_coren_direbrew
SD%Complete: 75
SDComment: Some parts are not complete - requires additional research. Brewmaidens scripts handled in eventAI.
SDCategory: Blackrock Depths
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "LFG/LFGDefines.h"

enum
{
    SAY_AGGRO                       = -1230034,

    // spells
    SPELL_DIREBREW_DISARM           = 47310,
    SPELL_SUMMON_DIREBREW_MINION    = 47375,
    SPELL_DIREBREW_CHARGE           = 47718,
    SPELL_SUMMON_MOLE_MACHINE       = 47691,            // triggers 47690
    SPELL_SUMMON_MOLE_MACHINE_MINION_SUMMONER = 47690,

    // summoned auras
    SPELL_PORT_TO_COREN             = 52850,

    // other summoned spells - currently not used in script
    // SPELL_CHUCK_MUG               = 50276,
    // SPELL_BARRELED_AURA           = 50278,            // used by Ursula
    // SPELL_HAS_BREW                = 47331,            // triggers 47344 - aura which asks for the second brew on item expire
    // SPELL_SEND_FIRST_MUG          = 47333,            // triggers 47345
    SPELL_SEND_SECOND_MUG           = 47339,             // triggers 47340 - spell triggered by 47344
    // SPELL_BREWMAIDEN_DESPAWN_AURA = 48186,            // purpose unk
    SPELL_DIREBREW_MINION_KNOCKBACK = 50313,

    SPELL_DIREBREWS_DISARM_PRECAST  = 47407,
    SPELL_DIREBREWS_DISARM_GROW     = 47409,

    // npcs
    NPC_ILSA_DIREBREW               = 26764,
    NPC_URSULA_DIREBREW             = 26822,
    NPC_DARK_IRON_ANTAGONIST        = 23795,

    // other
    FACTION_HOSTILE                 = 736,

    QUEST_INSULT_COREN              = 12062,

    MAX_DIREBREW_MINIONS            = 3,
};

enum CorenActions
{
    COREN_SPAWN_ILSA,
    COREN_SPAWN_URSULA,
    COREN_DISARM,
    COREN_DIREBREW_MINION,
    COREN_ACTION_MAX,
    COREN_ATTACK_TIMER,
};

struct boss_coren_direbrewAI : public CombatAI
{
    boss_coren_direbrewAI(Creature* creature) : CombatAI(creature, COREN_ACTION_MAX)
    {
        AddTimerlessCombatAction(COREN_SPAWN_ILSA, true);
        AddTimerlessCombatAction(COREN_SPAWN_URSULA, true);
        AddCombatAction(COREN_DISARM, 10000u);
        AddCombatAction(COREN_DIREBREW_MINION, 15000u);
        AddCustomAction(COREN_ATTACK_TIMER, true, [&]()
        {
            if (Player* player = m_creature->GetMap()->GetPlayer(m_targetPlayer))
            {
                m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_PLAYER);
                m_creature->SetInCombatWithZone();
                AttackStart(player);
                CreatureList staticSpawns;
                GetCreatureListWithEntryInGrid(staticSpawns, m_creature, NPC_DARK_IRON_ANTAGONIST, 50.f);
                for (auto creature : staticSpawns)
                {
                    creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_RESTORE_REACH_HOME | TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_PLAYER);
                    creature->SetInCombatWithZone();
                    creature->AI()->AttackClosestEnemy();
                }
            }
        });
    }

    ObjectGuid m_targetPlayer;
    GuidVector m_guids;

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_ILSA_DIREBREW:
            case NPC_URSULA_DIREBREW:
                summoned->CastSpell(m_creature, SPELL_PORT_TO_COREN, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->AttackStart(m_creature->GetVictim());
                summoned->SetCorpseDelay(5);
                break;
        }
        m_guids.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        if (!m_creature->IsAlive() || !m_creature->IsInCombat())
            return;

        switch (summoned->GetEntry())
        {
            case NPC_ILSA_DIREBREW:
                SetActionReadyStatus(COREN_SPAWN_ILSA, true);
                break;
            case NPC_URSULA_DIREBREW:
                SetActionReadyStatus(COREN_SPAWN_ILSA, true);
                break;
        }
    }

    void JustSummoned(GameObject* go) override
    {
        go->Use(m_creature);
        go->ForcedDespawn(7000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DespawnGuids(m_guids);
        CreatureList staticSpawns;
        GetCreatureListWithEntryInGrid(staticSpawns, m_creature, NPC_DARK_IRON_ANTAGONIST, 50.f);
        for (auto creature : staticSpawns)
            creature->ForcedDespawn();
        m_creature->GetMap()->AwardLFGRewards(SEASONAL_COREN_DIREBREW);
    }

    void EnterEvadeMode() override
    {
        CombatAI::EnterEvadeMode();
        DespawnGuids(m_guids);
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case COREN_SPAWN_ILSA:
                // Spawn Ilsa
                if (m_creature->GetHealthPercent() < 66.0f)
                {
                    m_creature->SummonCreature(NPC_ILSA_DIREBREW, 889.1403f, -130.9002f, -49.660107f, 5.427973747253417968f, TEMPSPAWN_DEAD_DESPAWN, 0);
                    SetActionReadyStatus(action, false);
                }
                break;
            case COREN_SPAWN_URSULA:
                // Spawn Ursula
                if (m_creature->GetHealthPercent() < 33.0f)
                {
                    m_creature->SummonCreature(NPC_URSULA_DIREBREW, 894.956f, -127.81718f, -49.659885f, 5.270894527435302734f, TEMPSPAWN_DEAD_DESPAWN, 0);
                    SetActionReadyStatus(action, false);
                }
                break;
            case COREN_DISARM:
                if (DoCastSpellIfCan(nullptr, SPELL_DIREBREW_DISARM) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
            case COREN_DIREBREW_MINION:
                if (DoCastSpellIfCan(nullptr, SPELL_SUMMON_MOLE_MACHINE) == CAST_OK)
                    ResetCombatAction(action, 15000);
                break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_EVENTAI_A)
        {
            DoScriptText(SAY_AGGRO, m_creature, invoker);

            m_targetPlayer = invoker->GetObjectGuid();
            ResetTimer(COREN_ATTACK_TIMER, 2000);
        }
    }

    // unconfirmed usage - at least during tbc
    //if (m_uiChargeTimer < uiDiff)
    //{
    //    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_DIREBREW_CHARGE, SELECT_FLAG_NOT_IN_MELEE_RANGE | SELECT_FLAG_PLAYER))
    //    {
    //        if (DoCastSpellIfCan(pTarget, SPELL_DIREBREW_CHARGE) == CAST_OK)
    //            m_uiChargeTimer = urand(5000, 10000);
    //    }
    //}
    //else
    //    m_uiChargeTimer -= uiDiff;
};

bool QuestRewarded_npc_coren_direbrew(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_INSULT_COREN)
        creature->AI()->SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_A, player, creature);

    return true;
}

struct RequestSecondMug : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetUnitTarget()->CastSpell(spell->GetCaster(), SPELL_SEND_SECOND_MUG, TRIGGERED_NONE);
    }
};

struct DirebrewDisarm : public SpellScript
{
    void OnCast(Spell* spell) const override
    {
        spell->GetCaster()->CastSpell(nullptr, SPELL_DIREBREWS_DISARM_PRECAST, TRIGGERED_OLD_TRIGGERED);
    }
};

struct DirebrewDisarmPrecast : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply)
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_DIREBREWS_DISARM_GROW);
    }

    void OnPeriodicDummy(Aura* aura) const override
    {
        aura->GetTarget()->CastSpell(nullptr, SPELL_DIREBREWS_DISARM_GROW, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SummonMoleMachineTargetPicker : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        spell->SetMaxAffectedTargets(1);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        spell->GetCaster()->CastSpell(spell->GetUnitTarget(), SPELL_SUMMON_MOLE_MACHINE_MINION_SUMMONER, TRIGGERED_OLD_TRIGGERED);
    }
};

struct SummonDirebrewMinion : public SpellScript
{
    void OnSummon(Spell* /*spell*/, Creature* summon) const override
    {
        summon->CastSpell(nullptr, SPELL_DIREBREW_MINION_KNOCKBACK, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_boss_coren_direbrew()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_coren_direbrew";
    pNewScript->GetAI = &GetNewAIInstance<boss_coren_direbrewAI>;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_coren_direbrew;
    pNewScript->RegisterSelf();

    RegisterSpellScript<RequestSecondMug>("spell_request_second_mug");
    RegisterSpellScript<DirebrewDisarm>("spell_direbrew_disarm");
    RegisterSpellScript<DirebrewDisarmPrecast>("spell_direbrew_disarm_precast");
    RegisterSpellScript<SummonMoleMachineTargetPicker>("spell_summon_mole_machine_target_picker");
    RegisterSpellScript<SummonDirebrewMinion>("spell_summon_direbrew_minion");
}
