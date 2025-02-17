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
 SDName: Shattered Halls Trash
 SD%Complete:
 SDComment:
 SDCategory: Hellfire Citadel, Shattered Halls
 EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "shattered_halls.h"
#include "AI/ScriptDevAI/base/CombatAI.h"

enum
{
    SAY_START = 14134,
    SAY_FINISH = 14135,
    // Aggro Textes for Centurion
    SAY_AGGRO1 = 16697,
    SAY_AGGRO2 = 16698,
    SAY_AGGRO3 = 16699,
    SAY_AGGRO4 = 16700,
    SAY_AGGRO5 = 16701,
    SAY_AGGRO6 = 16702,
    SAY_AGGRO7 = 16703,

    SPELL_DOUBLE_ATTACK = 19818,

    NPC_SHATTERED_HAND_GLADIATOR = 17464,
    NPC_SHATTERED_HAND_CENTURION = 17465,

    MAX_GLADIATORS = 3,
};

enum GladiatorActions
{
    GLADIATOR_STOP_EVENT,
    GLADIATOR_EVENT_RP,
    GLADIATOR_ACTION_MAX,
};

struct npc_shattered_hand_gladiator : public CombatAI
{
    npc_shattered_hand_gladiator(Creature* creature) : CombatAI(creature, GLADIATOR_ACTION_MAX)
    {
        AddTimerlessCombatAction(GLADIATOR_STOP_EVENT, true);
        AddCustomAction(GLADIATOR_EVENT_RP, true, [&]() { DoDelayedReset(); });
    }

    ObjectGuid m_sparringPartner;

    void Reset() override
    {
        CombatAI::Reset();
        m_eventStarted = false;

        DoCastSpellIfCan(nullptr, SPELL_DOUBLE_ATTACK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
    }

    bool m_eventStarted;

    void DoDelayedReset()
    {
        EnterEvadeMode();
        DisableTimer(GLADIATOR_EVENT_RP);
    }

    void HandleEventStart(Creature* partner)
    {
        m_creature->SetFactionTemporary(1693, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
        AttackStart(partner);
        m_sparringPartner = partner->GetObjectGuid();

        m_eventStarted = true;
        SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, partner);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* invoker, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A) // Attacked by Player
        {
            if (Creature* partner = m_creature->GetMap()->GetCreature(m_sparringPartner))
            {
                m_creature->getThreatManager().modifyThreatPercent(partner, -101);
                m_creature->ClearTemporaryFaction();
                m_eventStarted = false;
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_B) // End Duel
        {
            m_creature->AttackStop();
            m_creature->ClearTemporaryFaction();
            m_creature->HandleEmote(EMOTE_ONESHOT_BOW);
            ResetTimer(GLADIATOR_EVENT_RP, 1000);
            m_eventStarted = false;
        }
        else if (eventType == AI_EVENT_CUSTOM_C) // Start Duel
        {
            m_creature->SetFactionTemporary(1692, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_RESTORE_COMBAT_STOP);
            AttackStart(invoker);
            m_sparringPartner = invoker->GetObjectGuid();
            m_eventStarted = true;
        }
    }
    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case GLADIATOR_STOP_EVENT:
            {
                if (m_creature->GetHealthPercent() <= 40.f)
                {
                    if (Creature* centurion = GetClosestCreatureWithEntry(m_creature, NPC_SHATTERED_HAND_CENTURION, 15.f, true, false, true))
                        SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, centurion);
                }
                break;
            }
        }
    }
    void UpdateAI(const uint32 diff) override
    {
        CombatAI::UpdateAI(diff);
        if (m_creature->IsInCombat() && m_eventStarted)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (Creature* partner = m_creature->GetMap()->GetCreature(m_sparringPartner))
                {
                    SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, partner);
                    m_creature->getThreatManager().modifyThreatPercent(partner, -101);
                    m_creature->ClearTemporaryFaction();
                    m_eventStarted = false;
                }
            }
        }
    }
};

enum CenturionActions
{
    CENTURION_EVENT_RP,
    CENTURION_ACTION_MAX,
    CENTURION_START_EVENT,
};

struct npc_shattered_hand_centurion : public CombatAI
{
    npc_shattered_hand_centurion(Creature* creature) : CombatAI(creature, CENTURION_ACTION_MAX),
        m_instance(static_cast<ScriptedInstance*>(creature->GetInstanceData())),
        m_eventStarted(false)
    {
        AddCustomAction(CENTURION_START_EVENT, 1000, 4000, [&]() { HandleEventStart(); });
        AddCustomAction(CENTURION_EVENT_RP, 2000, 10000, [&]() { HandleRP(); });
    }

    void Reset() override
    {
        CombatAI::Reset();
        m_eventStarted = false;
        ResetTimer(CENTURION_START_EVENT, urand(10000, 25000));
    }

    ScriptedInstance* m_instance;
    bool m_eventStarted;

    void Aggro(Unit* /*who*/) override
    {
        switch (urand(0, 6))
        {
            case 0: DoBroadcastText(SAY_AGGRO1, m_creature); break;
            case 1: DoBroadcastText(SAY_AGGRO2, m_creature); break;
            case 2: DoBroadcastText(SAY_AGGRO3, m_creature); break;
            case 3: DoBroadcastText(SAY_AGGRO4, m_creature); break;
            case 4: DoBroadcastText(SAY_AGGRO5, m_creature); break;
            case 5: DoBroadcastText(SAY_AGGRO6, m_creature); break;
            case 6: DoBroadcastText(SAY_AGGRO7, m_creature); break;
        }
    }

    void HandleEventStart()
    {
        if (m_creature->IsInCombat())
            return;

        if (m_eventStarted)
            return;

        // Get all 4 Gladiators in Range 
        std::list<Creature*> m_gladiator;
        GuidVector m_gladiatorGuid;
        GetCreatureListWithEntryInGrid(m_gladiator, m_creature, NPC_SHATTERED_HAND_GLADIATOR, 10.0f);
        for (Creature* gladiator : m_gladiator)
            m_gladiatorGuid.push_back(gladiator->GetObjectGuid());

        // There should always be 4 Gladiators up handled via spawn_group flag respawn together
        if (m_gladiatorGuid.size() != 4)
            return;

        // 2 of them get faction 1693
        // the other 2 get faction 1692
        Creature* first = m_creature->GetMap()->GetCreature(m_gladiatorGuid[0]);  // A  
        Creature* second = m_creature->GetMap()->GetCreature(m_gladiatorGuid[1]); // B
        Creature* third = m_creature->GetMap()->GetCreature(m_gladiatorGuid[2]); // A
        Creature* fourth = m_creature->GetMap()->GetCreature(m_gladiatorGuid[3]); // B

        if (first->IsAlive() && second->IsAlive())
            if (npc_shattered_hand_gladiator* firstGladiator = dynamic_cast<npc_shattered_hand_gladiator*>(first->AI()))
                firstGladiator->HandleEventStart(second);

        if (third->IsAlive() && fourth->IsAlive())
            if (npc_shattered_hand_gladiator* thirdGladiator = dynamic_cast<npc_shattered_hand_gladiator*>(third->AI()))
                thirdGladiator->HandleEventStart(fourth);

        DoBroadcastText(SAY_START, m_creature);
        DisableTimer(CENTURION_START_EVENT);
        m_eventStarted = true;
    }


    void HandleRP()
    {
        switch (urand(0, 3))
        {
            case 0: m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH); break;
            case 1: m_creature->HandleEmote(EMOTE_ONESHOT_FLEX); break;
            case 2: m_creature->HandleEmote(EMOTE_ONESHOT_ROAR); break;
            case 3: m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION); break;
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            // Only finish event when Centurion is not in Combat
            if (m_creature->IsInCombat())
                return;

            if (m_eventStarted)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                DoBroadcastText(SAY_FINISH, m_creature);

                CreatureList gladiatorList;
                GetCreatureListWithEntryInGrid(gladiatorList, m_creature, NPC_SHATTERED_HAND_GLADIATOR, 20.0f);
                for (Creature* gladiator : gladiatorList)
                {
                    if (gladiator->IsAlive())
                        SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, gladiator);
                }                
                ResetTimer(CENTURION_START_EVENT, urand(10000, 25000));
                m_eventStarted = false;
            }
        }
    }
};

/*######
 ## npc_shattered_hand_legionnaire 16700
 ######*/

enum ShatteredHandLegionnairActions
{
    LEGIONNAIRE_PUMMEL,
    LEGIONNAIRE_AURA_OF_DISCIPLIN,
    LEGIONNAIRE_ACTION_MAX,
    LEGIONNAIRE_CALL_FOR_REINFORCEMENTS,
    LEGIONNAIRE_REINF_CD,
};

enum ShatteredHandLegionnair
{
    SPELL_AURA_OF_DISCIPLINE        = 30472,
    SPELL_PUMMEL                    = 15615,
    SPELL_ENRAGE                    = 30485,
    EMOTE_ENRAGE                    = 1151,
    MOB_FEL_ORC                     = 17083,   
};

static float FelOrcSpawnCoords[][4] =               // Coords needed for spawns 
{
    { 0.0f, 0.0f, 0.0f, 0.0f},                      // Legionnaire 001 spawn coords
    { 79.9949f, 111.5607f, -13.1384f, 4.6949f},     // Legionnaire 002 right side felorc spawn
    { 61.1264f, 110.8250f, -13.1384f, 6.1784f },    // Legionnaire 002 left side felorc spawn
    { 88.4735f, 187.3315f, -13.1929f, 3.144f },     // Legionnaire 003 spawn
    { 78.6885f, 218.2196f, -13.2166f, 4.013f },     // Legionnaire 004 spawn
    { 83.5307f, 250.5344f, -13.1131f, 3.060f },     // Legionnaire 005 spawn
    { 69.8188f, 239.513f, -13.193643f, 0.0 }         // Legionnaire 006 waypoint
};

static const int32 aRandomAggro[] = { 16700, 16703, 16698, 16701, 16702, 16697, 16699 };

static const int32 aRandomReinf[] = { 16356, 16357, 16358, 16359, 16360, 16361, 16362 };
static const int32 aRandomReinfSleeping[] = { 16363, 16364, 16365, 16366, 16367 };

struct npc_shattered_hand_legionnaire : public CombatAI
{
    npc_shattered_hand_legionnaire(Creature* creature) : CombatAI(creature, LEGIONNAIRE_ACTION_MAX), m_instance(static_cast<instance_shattered_halls*>(creature->GetInstanceData())),
    m_reinfCD(false)
    {
        AddCombatAction(LEGIONNAIRE_PUMMEL, 10000, 15000);
        AddCombatAction(LEGIONNAIRE_AURA_OF_DISCIPLIN, 0, 5000);
        AddCustomAction(LEGIONNAIRE_CALL_FOR_REINFORCEMENTS, true, [&]() { CallForReinforcements(); });
        AddCustomAction(LEGIONNAIRE_REINF_CD, true, [&]() { DoReinfCD(); });
        if (m_creature->HasStringId(FIRST_LEGIONNAIRE_STRING))
            if (m_creature->IsAlive())
                m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_001, 1);
            else
                m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_001, 0);
        else if (m_creature->HasStringId(SECOND_LEGIONNAIRE_STRING))
            legionnaireGuid = urand(1, 2);
        else if (m_creature->HasStringId(THIRD_LEGIONNAIRE_STRING))
            legionnaireGuid = 3;
        else if (m_creature->HasStringId(FOURTH_LEGIONNAIRE_STRING))
            legionnaireGuid = 4;
        else if (m_creature->HasStringId(FIFTH_LEGIONNAIRE_STRING))
            legionnaireGuid = 5;
        else if (m_creature->HasStringId(SIX_LEGIONNAIRE_STRING))
            legionnaireGuid = 6;
    }

    uint32 legionnaireGuid;
    instance_shattered_halls* m_instance;
    bool m_reinfCD;

    void Aggro(Unit* /*who*/) override
    {
        if (urand(0, 4) > 2)
            DoBroadcastText(aRandomAggro[urand(0, 6)], m_creature);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (m_creature->HasStringId(FIRST_LEGIONNAIRE_STRING))
        {
            // When Legionnaire 001 is dead change worldstate to false, heathen/savage group around him cant respawn anymore
            m_creature->GetMap()->GetVariableManager().SetVariable(WORLD_STATE_LEGIONNAIRE_001, 0);
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_JUST_DIED)
            ResetTimer(LEGIONNAIRE_CALL_FOR_REINFORCEMENTS, 0u);
    }

    void SummonedCreatureJustDied(Creature* /*summoned*/) override
    {
        // There can always be just one reinforcement up, when summoned dies legionnaire can spawn a new one after ~5-15 seconds cooldown
        if (m_reinfCD)
            ResetTimer(LEGIONNAIRE_REINF_CD, urand(5000, 15000));
    }

    // also reset timer when summoned despawns 
    void SummonedCreatureDespawn(Creature* /*summoned*/) override
    {
        if (m_reinfCD)
            ResetTimer(LEGIONNAIRE_REINF_CD, urand(5000, 15000));
    }

    void SummonedMovementInform(Creature* summoned, uint32 /*motionType*/, uint32 pointId) override
    {
        // When last waypoint reached, search for players.
        if (pointId == 100)
        {
            summoned->GetMotionMaster()->MoveIdle();
            summoned->SetInCombatWithZone();            
        }
    }

    void CallForReinforcements()
    {
        // reinforcement can get spawned even if legionnaire is outfight and has a cooldown between 10 and 15 seconds, but only one can be up
        if (!m_reinfCD)
        {
            if (m_creature->HasStringId(SECOND_LEGIONNAIRE_STRING) || m_creature->HasStringId(THIRD_LEGIONNAIRE_STRING) || m_creature->HasStringId(FOURTH_LEGIONNAIRE_STRING) || m_creature->HasStringId(FIFTH_LEGIONNAIRE_STRING))
            {
                if (Creature* felorc = m_creature->SummonCreature(MOB_FEL_ORC, FelOrcSpawnCoords[legionnaireGuid][0], FelOrcSpawnCoords[legionnaireGuid][1], FelOrcSpawnCoords[legionnaireGuid][2], FelOrcSpawnCoords[legionnaireGuid][3], TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, urand(20000, 25000), true, true))
                {
                    felorc->GetMotionMaster()->MoveWaypoint(legionnaireGuid, 0, 0, 0, FORCED_MOVEMENT_RUN);
                    felorc->SetCanCallForAssistance(false);
                    felorc->SetCanCheckForHelp(false);
                    DoBroadcastText(aRandomReinf[urand(0, 6)], m_creature);
                }
            }
            m_reinfCD = true;
        }

        // Legionnaire 006
        if (m_creature->HasStringId(SIX_LEGIONNAIRE_STRING))
        {
            // there are 4 sleeping npcs around him, if one of his group members dies he will call for one of the sleeping creatures to get up and join the fight
            // this doesnt have a cd, if all 4 npcs with sleeping aura are up, nothing more happens
            auto m_sleepingReinf = m_creature->GetMap()->GetCreatures(SLEEPING_REINF_STRING);
            Creature* closest = nullptr;
            for (Creature* creature : *m_sleepingReinf)
            {
                // Only call alive creatures
                // Only call creature that isnt in combat already
                // Only call creature that still has sleeping aura
                if (creature->IsAlive() && !creature->IsInCombat() && !creature->HasAura(AURA_SLEEPING))
                    continue;

                if (!closest)
                    closest = creature;
                else if (m_creature->GetDistance(creature, true, DIST_CALC_NONE) < m_creature->GetDistance(closest, true, DIST_CALC_NONE))
                    closest = creature;
            }
            if (closest)
            {
                DoBroadcastText(aRandomReinfSleeping[urand(0, 4)], m_creature);
                closest->RemoveAurasDueToSpell(AURA_SLEEPING);
                SendAIEvent(AI_EVENT_CUSTOM_EVENTAI_C, m_creature, closest);
            }
        }
        else if (m_creature->HasStringId(SEVENTH_LEGIONNAIRE_STRING))
        {
            // For the legionnaire 07, if one of his group members dies he will inform the nearest npc staying at the dummys behind him
            // all 3 npc at dummys have StringID assigned
            auto m_dummyReinf = m_creature->GetMap()->GetCreatures(DUMMY_REINF_STRING_1);
            Creature* closest = nullptr;
            for (Creature* creature : *m_dummyReinf)
            {
                // Only call alive creatures
                // Only call creature that isnt in combat already
                if (creature->IsAlive() && !creature->IsInCombat())
                    continue;

                if (!closest)
                    closest = creature;
                else if (m_creature->GetDistance(creature, true, DIST_CALC_NONE) < m_creature->GetDistance(closest, true, DIST_CALC_NONE))
                    closest = creature;
            }
            if (closest)
            {
                Unit* target = m_creature->GetVictim();
                if (target)
                {
                    closest->AI()->AttackStart(target);
                    DoBroadcastText(aRandomReinf[urand(0, 6)], m_creature);
                }
            }
        }
        else if (m_creature->HasStringId(EIGTH_LEGIONNAIRE_STRING))
        {
            // For the legionnaire 08, if one of his group members dies he will inform the nearest npc staying at the dummys behind him
            // all 3 npc at dummys have StringID assigned
            auto m_dummyReinf = m_creature->GetMap()->GetCreatures(DUMMY_REINF_STRING_2);
            Creature* closest = nullptr;
            for (Creature* creature : *m_dummyReinf)
            {
                // Only call alive creatures
                // Only call creature that isnt in combat already
                if (creature->IsAlive() && !creature->IsInCombat())
                    continue;

                if (!closest)
                    closest = creature;
                else if (m_creature->GetDistance(creature, true, DIST_CALC_NONE) < m_creature->GetDistance(closest, true, DIST_CALC_NONE))
                    closest = creature;
            }
            if (closest)
            {
                Unit* target = m_creature->GetVictim();
                if (target)
                {
                    closest->AI()->AttackStart(target);
                    DoBroadcastText(aRandomReinf[urand(0, 6)], m_creature);
                }
            }
        }

        // Buff can only get casted when legionnaire is infight and doesnt already have the buff
        if (!m_creature->IsInCombat() || !m_creature->GetVictim())
            return;

        if (!m_creature->HasAura(SPELL_ENRAGE))
        {
            m_creature->CastSpell(nullptr, SPELL_ENRAGE, TRIGGERED_NONE);
            DoBroadcastText(EMOTE_ENRAGE, m_creature);
        }
    }

    void DoReinfCD()
    {
        m_reinfCD = false;
    }

    void ExecuteAction(uint32 action) override
    {
        switch (action)
        {
            case LEGIONNAIRE_PUMMEL:
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, (SELECT_FLAG_PLAYER | SELECT_FLAG_CASTING)))
                    if (DoCastSpellIfCan(target, SPELL_PUMMEL) == CAST_OK)
                        ResetCombatAction(action, urand(10000, 15000));
                return;
            }
            case LEGIONNAIRE_AURA_OF_DISCIPLIN:
            {
                if (DoCastSpellIfCan(nullptr, SPELL_AURA_OF_DISCIPLINE) == CAST_OK)
                    ResetCombatAction(action, 240000);
                return;
            }
        }
    }
};

// 37437 ClearAllDebuffs
struct ClearAllDebuffs : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        auto holderMap = target->GetSpellAuraHolderMap();
        for (auto holderPair : holderMap)
        {
            if (!holderPair.second->IsPositive() && !holderPair.second->IsPassive())
            {
                target->RemoveAurasDueToSpell(holderPair.second->GetId());
            }
        }
        return;
    }
};

void AddSC_shattered_halls()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_centurion";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_centurion>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_gladiator";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_gladiator>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_legionnaire";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_legionnaire>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ClearAllDebuffs>("spell_clear_all_debuffs");
}
