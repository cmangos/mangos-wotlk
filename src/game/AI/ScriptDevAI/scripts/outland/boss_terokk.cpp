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
SDName: boss_terokk
SD%Complete: 100
SDComment:
SDCategory: Terokkar Forest
EndScriptData */

#include <utility>
#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "AI/ScriptDevAI/scripts/outland/world_outland.h"

enum
{
    // Terokk
    SAY_SPAWN           = -1001278,
    SAY_CHOSEN_ONE      = -1001279,
    SAY_DIVINE_SHIELD   = -1001280,
    SAY_ENRAGE          = -1001281,
    // Skyguard Ace
    SAY_SPAWN_ACE       = -1001282,
    SAY_FLAMES          = -1001283,
    SAY_TEROKK_DOWN     = -1001284,

    NPC_TEROKK              = 21838,
    NPC_ACE                 = 23377,
    NPC_INVISIBLE_STALKER   = 15214,

    // Intro
    SPELL_RED_BEAM      = 24240,
    SPELL_SHADOWFORM_TEROKK = 41408,

    // Combat
    SPELL_SHADOW_BOLT_VOLLEY        = 40721,
    SPELL_CLEAVE                    = 15284,
    SPELL_DIVINE_SHIELD             = 40733,
    SPELL_ENRAGE                    = 28747,
    SPELL_CHOSEN_ONE                = 40726,
    SPELL_WILL_OF_THE_ARAKKOA_GOD   = 40722,

    // Bombardment
    SPELL_SKYGUARD_FLARE        = 40655, // Summons flare at random location
    SPELL_ANCIENT_FLAMES        = 40657, // removes divine shield
    SPELL_SKYGUARD_FLARE_VISUAL = 40656, // placed on 23277 to see the location where flames will fall

    PATH_ID_START = 0,
    PATH_ID_CYCLE = 1,
    PATH_ID_END   = 2,

    POINT_FINAL_START = 2,
    POINT_FINAL_END   = 4,

    COUNT_ACE = 3,

    MOUNT_OFFSET    = 21155,
    MOUNT_COUNT     = 4,
};

enum TerokkActions
{
    TEROKK_COMBAT_ACTION_SPAWN_ACE,
    TEROKK_COMBAT_ACTION_PHASE_2,
    TEROKK_COMBAT_ACTION_DIVINE_SHIELD,
    TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY,
    TEROKK_COMBAT_ACTION_CHOSEN_ONE,
    TEROKK_COMBAT_ACTION_CLEAVE,
    TEROKK_COMBAT_ACTION_MAX,
    TEROKK_ACTION_SPAWN = TEROKK_COMBAT_ACTION_MAX,
    TEROKK_ACTION_SAY,
    TEROKK_ACTION_ATTACK,
    TEROKK_ACTION_ACE_CAST,
};

struct boss_terokkAI : public CombatAI
{
    boss_terokkAI(Creature* creature) : CombatAI(creature, TEROKK_COMBAT_ACTION_MAX)
    {
        AddCustomAction(TEROKK_ACTION_SPAWN, 0u, [&] { m_creature->CastSpell(nullptr, SPELL_RED_BEAM, TRIGGERED_OLD_TRIGGERED); });
        AddCustomAction(TEROKK_ACTION_SAY, 2000u, [&]
        {
            m_creature->CastSpell(nullptr, SPELL_SHADOWFORM_TEROKK, TRIGGERED_OLD_TRIGGERED);
            DoScriptText(SAY_SPAWN, m_creature);
        });
        AddCustomAction(TEROKK_ACTION_ATTACK, 9000u, [&]
        {
            m_creature->SetImmuneToPlayer(false);
            if (Unit* spawner = m_creature->GetSpawner()) AttackStart(spawner);
        });
        AddCustomAction(TEROKK_ACTION_ACE_CAST, true, [&]
        {
            Creature* target = GetClosestCreatureWithEntry(m_creature, NPC_SKYGUARD_TARGET, 70.f);
            if (Creature* ace = m_creature->GetMap()->GetCreature(m_aces[1]))
                ace->AI()->DoCastSpellIfCan(target, SPELL_ANCIENT_FLAMES);
        });
        AddTimerlessCombatAction(TEROKK_COMBAT_ACTION_SPAWN_ACE, true);
        AddTimerlessCombatAction(TEROKK_COMBAT_ACTION_PHASE_2, true);
        AddCombatAction(TEROKK_COMBAT_ACTION_DIVINE_SHIELD, GetInitialActionTimer(TEROKK_COMBAT_ACTION_DIVINE_SHIELD));
        AddCombatAction(TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY, GetInitialActionTimer(TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY));
        AddCombatAction(TEROKK_COMBAT_ACTION_CHOSEN_ONE, GetInitialActionTimer(TEROKK_COMBAT_ACTION_CHOSEN_ONE));
        AddCombatAction(TEROKK_COMBAT_ACTION_CLEAVE, GetInitialActionTimer(TEROKK_COMBAT_ACTION_CLEAVE));

        m_creature->GetCombatManager().SetLeashingDisable(true);
    }

    bool m_phase;
    GuidVector m_aces;

    uint32 GetInitialActionTimer(TerokkActions id)
    {
        switch (id)
        {
            case TEROKK_COMBAT_ACTION_DIVINE_SHIELD: return 0;
            case TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY: return 4000;
            case TEROKK_COMBAT_ACTION_CHOSEN_ONE: return 30000;
            case TEROKK_COMBAT_ACTION_CLEAVE: return urand(6000, 9000);
            default: return 0;
        }
    }

    uint32 GetSubsequentActionTimer(TerokkActions id) const
    {
        switch (id)
        {
            case TEROKK_COMBAT_ACTION_DIVINE_SHIELD: return 22000;
            case TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY: return urand(4000, 15000);
            case TEROKK_COMBAT_ACTION_CHOSEN_ONE: return 30000;
            case TEROKK_COMBAT_ACTION_CLEAVE: return urand(7000, 15000);
            default: return 0;
        }
    }

    void Reset() override
    {
        CombatAI::Reset();

        m_phase = false;

        DespawnGuids(m_aces);

        if (InstanceData* instance = m_creature->GetInstanceData())
            instance->SetData(TYPE_TEROKK, 0);
    }

    void JustDied(Unit* killer) override
    {
        if (InstanceData* instance = m_creature->GetInstanceData())
            instance->SetData(TYPE_TEROKK, 0);

        if (m_aces.empty())
            return;

        if (Creature* spawn = m_creature->GetMap()->GetCreature(m_aces[0]))
        {
            DoScriptText(SAY_TEROKK_DOWN, spawn, killer);
            spawn->GetMotionMaster()->Clear(false, true);
            spawn->GetMotionMaster()->MoveWaypoint(PATH_ID_END);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (m_creature->HasAura(SPELL_DIVINE_SHIELD))
            {
                m_creature->CastSpell(nullptr, SPELL_ENRAGE, TRIGGERED_NONE);
                DoScriptText(SAY_ENRAGE, m_creature);
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->AI()->SetReactState(REACT_PASSIVE);
        static_cast<CreatureAI*>(summoned->AI())->SetDeathPrevention(true);
        summoned->SetCanEnterCombat(false);
        switch (m_aces.size())
        {
            case 0:
                DoScriptText(SAY_SPAWN_ACE, summoned);
                summoned->GetMotionMaster()->Clear(false, true);
                summoned->GetMotionMaster()->MoveWaypoint();
                break;
            case 1:
                if (Creature* spawn = m_creature->GetMap()->GetCreature(m_aces[0]))
                    summoned->GetMotionMaster()->MoveFollow(spawn, 1.f, M_PI_F + M_PI_F / 4, true);
                break;
            case 2:
                if (Creature* spawn = m_creature->GetMap()->GetCreature(m_aces[0]))
                    summoned->GetMotionMaster()->MoveFollow(spawn, 1.f, M_PI_F - M_PI_F / 4, true);
                break;
        }
        summoned->Mount(MOUNT_OFFSET + urand(0, MOUNT_COUNT - 1));
        m_aces.push_back(summoned->GetObjectGuid());
    }

    void SummonedMovementInform(Creature* summoned, uint32 motionType, uint32 data) override
    {
        if (motionType == WAYPOINT_MOTION_TYPE)
        {
            switch (summoned->GetMotionMaster()->GetPathId())
            {
                case PATH_ID_START:
                    if (data == POINT_FINAL_START)
                    {
                        summoned->GetMotionMaster()->Clear(false, true);
                        summoned->GetMotionMaster()->MoveWaypoint(PATH_ID_CYCLE);
                    }
                    break;
                case PATH_ID_END:
                    if (data == POINT_FINAL_END)
                        DespawnGuids(m_aces);
                    break;
                default: break;
            }                
        }
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case TEROKK_COMBAT_ACTION_SPAWN_ACE:
                if (m_creature->GetHealthPercent() > 50.f)
                    return;
                m_creature->SummonCreature(NPC_ACE, -3335.223f, 3431.947f, 426.3864f, 0.f, TEMPSPAWN_MANUAL_DESPAWN, 0, true, true);
                m_creature->SummonCreature(NPC_ACE, -3335.223f, 3431.947f, 426.3864f, 0.f, TEMPSPAWN_MANUAL_DESPAWN, 0, true, true);
                m_creature->SummonCreature(NPC_ACE, -3335.223f, 3431.947f, 426.3864f, 0.f, TEMPSPAWN_MANUAL_DESPAWN, 0, true, true);
                SetActionReadyStatus(action, false);
                return; // not exclusive action
            case TEROKK_COMBAT_ACTION_PHASE_2:
                if (m_creature->GetHealthPercent() > 25.f)
                    break;
                m_phase = true;
                DoScriptText(SAY_DIVINE_SHIELD, m_creature);
                if (Creature* ace = m_creature->GetMap()->GetCreature(m_aces[0]))
                    DoScriptText(SAY_FLAMES, ace);
                SetActionReadyStatus(action, false);
                return; // not exclusive action
            case TEROKK_COMBAT_ACTION_DIVINE_SHIELD:
                if (!m_phase)
                    return;
                m_creature->RemoveAurasDueToSpell(SPELL_ENRAGE);
                if (DoCastSpellIfCan(nullptr, SPELL_DIVINE_SHIELD) == CAST_OK)
                {
                    Creature* stalker = GetClosestCreatureWithEntry(m_creature, NPC_INVISIBLE_STALKER, 70.f);
                    if (stalker)
                        if (Creature* ace = m_creature->GetMap()->GetCreature(m_aces[0]))
                            ace->AI()->DoCastSpellIfCan(stalker, SPELL_SKYGUARD_FLARE);
                    ResetTimer(TEROKK_ACTION_ACE_CAST, 15000);
                    ResetCombatAction(action, GetSubsequentActionTimer(TerokkActions(action)));
                }
                return;
            case TEROKK_COMBAT_ACTION_SHADOW_BOLT_VOLLEY:
                if (DoCastSpellIfCan(nullptr, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(TerokkActions(action)));
                return;
            case TEROKK_COMBAT_ACTION_CHOSEN_ONE:
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(target, SPELL_CHOSEN_ONE) == CAST_OK)
                    {
                        m_creature->CastSpell(nullptr, SPELL_WILL_OF_THE_ARAKKOA_GOD, TRIGGERED_NONE);
                        DoScriptText(SAY_CHOSEN_ONE, m_creature, target);
                        ResetCombatAction(action, GetSubsequentActionTimer(TerokkActions(action)));
                        return;
                    }
                }
                return;
            case TEROKK_COMBAT_ACTION_CLEAVE:
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                    ResetCombatAction(action, GetSubsequentActionTimer(TerokkActions(action)));
                return;
        }
    }
};

bool ProcessEventId_event_summon_terokk(uint32 /*eventId*/, Object* source, Object* /*pTarget*/, bool /*isStart*/)
{
    Player* player = (Player*)source;
    if (player->GetMap()->SpawnedCountForEntry(NPC_TEROKK) == 0)
        player->SummonCreature(NPC_TEROKK, -3788.856f, 3507.526f, 286.8846f, 3.159046f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 600000, true, false, 0, 0, 0, true);
    return true;
}

struct TerokkAncientFlames : public AuraScript
{
    // substitute for missing serverside 40720
    void OnPeriodicTickEnd(Aura* aura) const override
    {
        Unit* target = aura->GetTarget();
        target->RemoveAurasDueToSpell(SPELL_DIVINE_SHIELD);
        if (target->GetEntry() == NPC_TEROKK)
            target->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, (Creature*)target);
    }
};

void AddSC_boss_terokk()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "event_summon_terokk";
    pNewScript->pProcessEventId = &ProcessEventId_event_summon_terokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_terokk";
    pNewScript->GetAI = &GetNewAIInstance<boss_terokkAI>;
    pNewScript->RegisterSelf();

    RegisterAuraScript<TerokkAncientFlames>("spell_terokk_ancient_flames");
}