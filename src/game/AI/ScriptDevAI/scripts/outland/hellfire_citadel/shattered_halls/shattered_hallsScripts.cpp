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


void AddSC_shattered_halls()
{
    Script* pNewScript = new Script;
    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_centurion";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_centurion>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shattered_hand_gladiator";
    pNewScript->GetAI = &GetNewAIInstance<npc_shattered_hand_gladiator>;
    pNewScript->RegisterSelf();
}
