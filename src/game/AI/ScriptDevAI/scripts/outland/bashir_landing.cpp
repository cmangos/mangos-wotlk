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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/scripts/outland/world_outland.h"
#include "AI/ScriptDevAI/base/CombatAI.h"
#include "Spells/Scripts/SpellScript.h"

// Bashir Landing event scripts
// also connected with world_map_scripts.cpp and WorldState

enum
{
    PATH_ID_BIG_PATH = 1,
    PATH_ID_DISMOUNT = 2, // for tech and lieutenant
    PATH_ID_OUTRO    = 3,

    POINT_DISMOUNT = 101,
    POINT_FINAL = 3,
    POINT_MOUNT = 1,
    POINT_DESPAWN = 102,

    SPELL_ETHEREAL_RING_SIGNAL_FLARE_BURST = 41293,
};

struct npc_skyguard_aether_techAI : public ScriptedAI
{
    npc_skyguard_aether_techAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    void Reset() override
    {

    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            DoCastSpellIfCan(nullptr, SPELL_ETHEREAL_RING_SIGNAL_FLARE_BURST);
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_OUTRO);
        }
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == WAYPOINT_MOTION_TYPE)
        {
            switch (m_creature->GetMotionMaster()->GetPathId())
            {
                case PATH_ID_BIG_PATH:
                {
                    if (data == POINT_DISMOUNT)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->SetHover(false);
                        m_creature->SetLevitate(false);
                        m_creature->Unmount();
                        m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_DISMOUNT);
                    }
                    break;
                }
                case PATH_ID_OUTRO:
                {
                    if (data == POINT_MOUNT)
                    {
                        m_creature->Mount(MOUNT_NETHER_RAY_DISPLAY_ID);
                        m_creature->SetHover(true);
                        m_creature->SetLevitate(true);
                    }
                    else if (data == POINT_DESPAWN)
                    {
                        m_creature->GetMap()->GetInstanceData()->SetData(TYPE_BASHIR, 3);
                        m_creature->ForcedDespawn();
                    }
                    break;
                }
                default:
                {
                    if (data == POINT_FINAL)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->HandleEmote(EMOTE_STATE_USESTANDING_NOSHEATHE);
                        m_creature->GetMap()->GetInstanceData()->SetData(TYPE_BASHIR, 1);
                    }
                    break;
                }
            }
        }
    }
};

enum
{
    SPELL_WHIRLWIND = 38618,

    NPC_SKYGUARD_LIEUTENANT = 23430,

    SAY_AGGRO_1 = -1015055,
    SAY_AGGRO_2 = -1015056,
    SAY_AGGRO_3 = -1015057,
    SAY_AGGRO_4 = -1015058,
    SAY_AGGRO_5 = -1015059,
    SAY_AGGRO_6 = -1015060,
    SAY_AGGRO_7 = -1015061,
};

enum RangerActions
{
    RANGER_COMBAT_ACTION_WHIRLWIND,
    RANGER_COMBAT_ACTION_MAX,
};

struct npc_skyguard_rangerAI : public ScriptedAI
{
    npc_skyguard_rangerAI(Creature* creature) : ScriptedAI(creature, RANGER_COMBAT_ACTION_MAX), m_spawnId(PATH_ID_DISMOUNT) // implicit default for lieutenant
    {
        AddCombatAction(RANGER_COMBAT_ACTION_WHIRLWIND, 0u);
        Reset();
    }

    uint32 m_spawnId;

    void Reset() override
    {
        for (uint32 i = 0; i < RANGER_COMBAT_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(RANGER_COMBAT_ACTION_WHIRLWIND, GetInitialActionTimer(RANGER_COMBAT_ACTION_WHIRLWIND));
    }

    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case RANGER_COMBAT_ACTION_WHIRLWIND: return urand(15000, 30000);
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case RANGER_COMBAT_ACTION_WHIRLWIND: return urand(15000, 30000);
            default: return 0; // never occurs but for compiler
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            m_spawnId = miscValue; // map passes which spawn this is
    }

    void MovementInform(uint32 movementType, uint32 data) override
    {
        if (movementType == WAYPOINT_MOTION_TYPE)
        {
            switch (m_creature->GetMotionMaster()->GetPathId())
            {
                case PATH_ID_BIG_PATH:
                {
                    if (data == POINT_DISMOUNT)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->SetHover(false);
                        m_creature->SetLevitate(false);
                        m_creature->Unmount();
                        m_creature->GetMotionMaster()->MoveWaypoint(m_spawnId);
                        if (m_creature->GetEntry() == NPC_SKYGUARD_LIEUTENANT)
                            m_creature->GetMap()->GetInstanceData()->SetData(TYPE_BASHIR, 0);
                    }
                    break;
                }
                default:
                {
                    if (data == POINT_FINAL)
                    {
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                    }
                    break;
                }
            }
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < RANGER_COMBAT_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case RANGER_COMBAT_ACTION_WHIRLWIND:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_WHIRLWIND) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        UpdateTimers(diff, m_creature->IsInCombat());
        ExecuteActions();

        DoMeleeAttackIfReady();
    }
};

enum BashirVendors
{
    PATH_ID_INITIAL = 1,
    PATH_ID_DISMOUNTED = 2,

    PATH_INITIAL_LAST_POINT = 14,
    PATH_DISMOUNTED_CAST_POINT = 4,
    PATH_DISMOUNTED_LAST_POINT = 5,

    SAY_ASSISTANT       = -1015047,
    SAY_ADEPT           = -1015049,
    SAY_MASTER          = -1015053,

    SPELL_ETHEREAL_RING_VISUAL = 41457,
};

std::map<uint32, float> vendorOrientations =
{
    {23243, 0.1919862},
    {23244, 1.466077},
    {23245, 1.32645},
};

std::map<uint32, int32> vendorText =
{
    {23243, SAY_ASSISTANT },
    {23244, SAY_ADEPT },
    {23245, SAY_MASTER },
};

struct npc_aether_tech_vendorAI : public ScriptedAI
{
    npc_aether_tech_vendorAI(Creature* creature) : ScriptedAI(creature, 0)
    {
        SetReactState(REACT_PASSIVE);
        AddCustomAction(0, true, [&] {m_creature->GetMap()->GetInstanceData()->SetData(TYPE_BASHIR, 2); });
        Reset();
    }

    void Reset() override
    {

    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != WAYPOINT_MOTION_TYPE)
            return;

        if (m_creature->GetMotionMaster()->GetPathId() == PATH_ID_INITIAL)
        {
            if (pointId == PATH_INITIAL_LAST_POINT)
            {
                m_creature->SetLevitate(false);
                m_creature->SetCanFly(false);
                m_creature->Unmount();
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveWaypoint(PATH_ID_DISMOUNTED);
                m_creature->SetBaseRunSpeed(8.f/7.f); // sniffed value
            }
        }
        else
        {
            if (pointId == PATH_DISMOUNTED_CAST_POINT)
                DoCastSpellIfCan(nullptr, SPELL_ETHEREAL_RING_VISUAL);
            else if (pointId == PATH_DISMOUNTED_LAST_POINT)
            {
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                DoScriptText(vendorText[m_creature->GetEntry()], m_creature);
                m_creature->HandleEmote(EMOTE_STATE_USESTANDING);
                ResetTimer(0, 19000);
            }
        }
    }
};

// Bashir bosses

enum BashirFleshFiend
{
    SPELL_THRASH            = 3417,
    SPELL_EAT_FRIEND        = 40991,
    SPELL_INFECTIOUS_POISON = 29939,
    SPELL_GAPING_MAW        = 29935,

    NPC_SLAVERING_SLAVE = 23246, // corpse eating target

    POINT_EAT_FRIEND = 1,
};

enum FleshFiendActions
{
    FIEND_ACTION_EAT_FRIEND,
    FIEND_ACTION_INFECTIOUS_POISON,
    FIEND_ACTION_GAPING_MAW,
    FIEND_ACTION_MAX,
    FIEND_ACTION_EAT_FRIEND_END,
};

struct npc_bashir_flesh_fiendAI : public ScriptedAI
{
    npc_bashir_flesh_fiendAI(Creature* creature) : ScriptedAI(creature, FIEND_ACTION_MAX)
    {
        AddCombatAction(FIEND_ACTION_EAT_FRIEND, 0u);
        AddCombatAction(FIEND_ACTION_INFECTIOUS_POISON, 0u);
        AddCombatAction(FIEND_ACTION_GAPING_MAW, 0u);
        AddCustomAction(FIEND_ACTION_EAT_FRIEND_END, 0u, [&]
        {
            SetCombatScriptStatus(false);
            SetCombatMovement(true, true);
        });
        Reset();
    }

    ObjectGuid m_slaveringSlave;

    void Reset() override
    {
        for (uint32 i = 0; i < FIEND_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        ResetTimer(FIEND_ACTION_EAT_FRIEND, GetInitialActionTimer(FIEND_ACTION_EAT_FRIEND));
        ResetTimer(FIEND_ACTION_INFECTIOUS_POISON, GetInitialActionTimer(FIEND_ACTION_INFECTIOUS_POISON));
        ResetTimer(FIEND_ACTION_GAPING_MAW, GetInitialActionTimer(FIEND_ACTION_GAPING_MAW));

        DoCastSpellIfCan(nullptr, SPELL_THRASH, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    // TODO: review timers
    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case FIEND_ACTION_EAT_FRIEND: return 2000;
            case FIEND_ACTION_INFECTIOUS_POISON: return 15000;
            case FIEND_ACTION_GAPING_MAW: return 20000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case FIEND_ACTION_EAT_FRIEND: return 2000;
            case FIEND_ACTION_INFECTIOUS_POISON: return 15000;
            case FIEND_ACTION_GAPING_MAW: return 20000;
            default: return 0; // never occurs but for compiler
        }
    }

    void MovementInform(uint32 motionType, uint32 data) override
    {
        if (motionType == POINT_MOTION_TYPE && data == POINT_EAT_FRIEND)
        {
            ResetTimer(FIEND_ACTION_EAT_FRIEND_END, 1000);
            if (Creature* slave = m_creature->GetMap()->GetCreature(m_slaveringSlave))
                DoCastSpellIfCan(slave, SPELL_EAT_FRIEND);
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < FIEND_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case FIEND_ACTION_EAT_FRIEND:
                    {
                        // TODO:
                        Creature* slaveringSlave = GetClosestCreatureWithEntry(m_creature, NPC_SLAVERING_SLAVE, 10.f, false, true);
                        if (slaveringSlave) // found corpse of ally
                        {
                            SetCombatScriptStatus(true);
                            SetCombatMovement(false);
                            float x, y, z;
                            m_creature->GetVictim()->GetNearPoint(m_creature, x, y, z, m_creature->GetObjectBoundingRadius(), m_creature->GetCombinedCombatReach(slaveringSlave), slaveringSlave->GetAngle(m_creature));
                            m_creature->GetMotionMaster()->MovePoint(POINT_EAT_FRIEND, x, y, z);
                            m_slaveringSlave = slaveringSlave->GetObjectGuid();
                        }
                        else
                        {
                            ResetTimer(i, 2000);
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                    case FIEND_ACTION_INFECTIOUS_POISON:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_INFECTIOUS_POISON) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                    case FIEND_ACTION_GAPING_MAW:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_GAPING_MAW) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                            return;
                        }
                        continue;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        ExecuteActions();
        DoMeleeAttackIfReady();
    }
};

enum DisruptorTower
{
    SPELL_CANNON_VISUAL       = 40858,
    SPELL_CANNON_DEATH_VISUAL_START = 40952,
    SPELL_CANNON_DEATH_VISUAL_END   = 40868,

    SPELL_BOLT_BUNNY          = 40750,
    SPELL_BOLT_BURST          = 40801, // cast by bunny on tower
    SPELL_THE_BOLT            = 40758,
    SPELL_DISRUPTOR_EXPLOSION = 40799, // cast by bunny
};

struct npc_disruptor_towerAI : public ScriptedAI
{
    npc_disruptor_towerAI(Creature* creature) : ScriptedAI(creature, 0)
    {
        SetCombatMovement(false);
        SetMeleeEnabled(false);
        SetReactState(REACT_PASSIVE);
        AddCustomAction(0, 0u, [&]
        {
            DoCastSpellIfCan(nullptr, SPELL_BOLT_BUNNY);
            ResetTimer(0, urand(6000, 12000));
        });
        Reset();
    }

    void Reset() override
    {

    }

    void JustRespawned() override
    {
        ScriptedAI::JustRespawned();
        DoCastSpellIfCan(nullptr, SPELL_ETHEREAL_TELEPORT);
        DoCastSpellIfCan(nullptr, SPELL_CANNON_VISUAL);
        ResetTimer(0, 10000);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->CastSpell(m_creature, SPELL_BOLT_BURST, TRIGGERED_NONE);
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->CastSpell(nullptr, SPELL_CANNON_DEATH_VISUAL_START, TRIGGERED_OLD_TRIGGERED);
    }
};

enum GrandCollector
{
    SPELL_BOSS_3_SPAWN_IN = 41569,
    SPELL_BOSS_3_SUMMON_LIEUTENANT = 41275,

    SPELL_RESONANT_FEEDBACK = 40942,
    SPELL_MIRROR_IMAGE_1    = 40943,
    SPELL_MIRROR_IMAGE_2    = 40944,
    SPELL_ARCANE_VOLLEY     = 40937, // also used by mirror image

    NPC_SKYGUARD_AETHER_TECH = 23241,

    SAY_COLLECTOR_SPAWN  = -1015050,
    SAY_COLLECTOR_ATTACK = -1015051,
};

enum GrandCollectorActions
{
    COLLECTOR_ACTION_MIRROR_IMAGE,
    COLLECTOR_ACTION_RESONANT_FEEDBACK,
    COLLECTOR_ACTION_ARCANE_VOLLEY,
    COLLECTOR_COMBAT_ACTION_MAX,
    COLLECTOR_ACTION_HANDLE_EVENT,
};

struct npc_grand_collectorAI : public ScriptedAI
{
    npc_grand_collectorAI(Creature* creature) : ScriptedAI(creature, COLLECTOR_COMBAT_ACTION_MAX), m_introEventId(0),
        m_instance(static_cast<ScriptedInstance*>(m_creature->GetMap()->GetInstanceData()))
    {
        SetReactState(REACT_PASSIVE);
        AddCombatAction(COLLECTOR_ACTION_MIRROR_IMAGE, 0u);
        AddCombatAction(COLLECTOR_ACTION_RESONANT_FEEDBACK, 0u);
        AddCombatAction(COLLECTOR_ACTION_ARCANE_VOLLEY, 0u);
        AddCustomAction(COLLECTOR_ACTION_HANDLE_EVENT, 0u, [&] { HandleEvent(); });
        Reset();
    }

    uint32 m_introEventId;
    ScriptedInstance* m_instance;

    void Reset() override
    {
        for (uint32 i = 0; i < COLLECTOR_COMBAT_ACTION_MAX; ++i)
            SetActionReadyStatus(i, false);

        SetActionReadyStatus(COLLECTOR_ACTION_MIRROR_IMAGE, true);

        ResetTimer(COLLECTOR_ACTION_RESONANT_FEEDBACK, GetInitialActionTimer(COLLECTOR_ACTION_RESONANT_FEEDBACK));
        ResetTimer(COLLECTOR_ACTION_ARCANE_VOLLEY, GetInitialActionTimer(COLLECTOR_ACTION_ARCANE_VOLLEY));
    }

    // TODO: review timers
    uint32 GetInitialActionTimer(uint32 id)
    {
        switch (id)
        {
            case COLLECTOR_ACTION_RESONANT_FEEDBACK: return 15000;
            case COLLECTOR_ACTION_ARCANE_VOLLEY: return 20000;
            default: return 0; // never occurs but for compiler
        }
    }

    uint32 GetSubsequentActionTimer(uint32 id)
    {
        switch (id)
        {
            case COLLECTOR_ACTION_RESONANT_FEEDBACK: return 15000;
            case COLLECTOR_ACTION_ARCANE_VOLLEY: return 20000;
            default: return 0; // never occurs but for compiler
        }
    }

    void HandleEvent()
    {
        switch (m_introEventId)
        {
            case 0:
            case 1:
            case 2:
                DoCastSpellIfCan(nullptr, SPELL_BOSS_3_SUMMON_LIEUTENANT);
                break;
            case 3:
            {
                Creature* tech = m_instance->GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH);
                SetReactState(REACT_AGGRESSIVE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                DoScriptText(SAY_COLLECTOR_ATTACK, m_creature);
                AttackStart(tech);
                break;
            }
        }
        ++m_introEventId;
        if (m_introEventId < 4)
            ResetTimer(COLLECTOR_ACTION_HANDLE_EVENT, 32000);
    }

    void JustRespawned() override
    {
        DoScriptText(SAY_COLLECTOR_SPAWN, m_creature);
    }

    void JustSummoned(Creature* summoned) override
    {
        if (Creature* tech = m_instance->GetSingleCreatureFromStorage(NPC_SKYGUARD_AETHER_TECH))
        {
            summoned->CastSpell(nullptr, SPELL_ETHEREAL_TELEPORT, TRIGGERED_NONE);
            summoned->AI()->AttackStart(tech);
        }
    }

    void ExecuteActions() override
    {
        if (!CanExecuteCombatAction())
            return;

        for (uint32 i = 0; i < COLLECTOR_COMBAT_ACTION_MAX; ++i)
        {
            if (GetActionReadyStatus(i))
            {
                switch (i)
                {
                    case COLLECTOR_ACTION_MIRROR_IMAGE:
                    {
                        if (m_creature->GetHealthPercent() <= 50.f)
                        {
                            DoCastSpellIfCan(nullptr, SPELL_MIRROR_IMAGE_1);
                            DoCastSpellIfCan(nullptr, SPELL_MIRROR_IMAGE_2);
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                    case COLLECTOR_ACTION_RESONANT_FEEDBACK:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_RESONANT_FEEDBACK) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                    case COLLECTOR_ACTION_ARCANE_VOLLEY:
                    {
                        if (DoCastSpellIfCan(nullptr, SPELL_ARCANE_VOLLEY) == CAST_OK)
                        {
                            ResetTimer(i, GetSubsequentActionTimer(i));
                            SetActionReadyStatus(i, false);
                        }
                        continue;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        UpdateTimers(diff, m_creature->IsInCombat());
        if (!m_creature->SelectHostileTarget())
            return;

        ExecuteActions();
        DoMeleeAttackIfReady();
    }
};

struct EtherealRingSignalFlare : public SpellScript
{
    void OnDestTarget(Spell* spell) const override
    {
        spell->m_targets.m_destPos.z = 342.9485f; // confirmed with sniffs
    }
};

void AddSC_bashir_landing()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_skyguard_aether_tech";
    pNewScript->GetAI = &GetNewAIInstance<npc_skyguard_aether_techAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_skyguard_ranger";
    pNewScript->GetAI = &GetNewAIInstance<npc_skyguard_rangerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_skyguard_lieutenant";
    pNewScript->GetAI = &GetNewAIInstance<npc_skyguard_rangerAI>; // same AI handles them until any difference is found
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_aether_tech_vendor";
    pNewScript->GetAI = &GetNewAIInstance<npc_aether_tech_vendorAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bashir_flesh_fiend";
    pNewScript->GetAI = &GetNewAIInstance<npc_bashir_flesh_fiendAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_disruptor_tower";
    pNewScript->GetAI = &GetNewAIInstance<npc_disruptor_towerAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_grand_collector";
    pNewScript->GetAI = &GetNewAIInstance<npc_grand_collectorAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<EtherealRingSignalFlare>("spell_ethereal_ring_signal_flare");
}