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
SDName: HyjalAI
SD%Complete: 90
SDComment:
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjalAI.h"

struct HyjalYells
{
    uint32   uiCreatureEntry;
    YellType m_pYellType;                                   // Used to determine the type of yell (attack, rally, etc)
    int32    m_iTextId;                                     // The text id to be yelled
};

static const HyjalYells aHyjalYell[] =
{
    {NPC_JAINA,  ATTACKED, -1534000},
    {NPC_JAINA,  ATTACKED, -1534001},
    //{NPC_JAINA,  INCOMING, -1534002}, // No official source confirming this text exists or was ever used
    {NPC_JAINA,  RALLY,    -1534003},
    {NPC_JAINA,  RALLY,    -1534004},
    {NPC_JAINA,  RALLY,    -1534005},
    {NPC_JAINA,  RETREAT,  -1534006},
    {NPC_JAINA,  WIN,      -1534007},
    {NPC_JAINA,  DEATH,    -1534008},

    {NPC_THRALL, ATTACKED, -1534009},
    {NPC_THRALL, ATTACKED, -1534010},
    //{NPC_THRALL, INCOMING, -1534011}, // No official source confirming this text exists or was ever used
    {NPC_THRALL, RALLY,    -1534012},
    {NPC_THRALL, RALLY,    -1534013},
    {NPC_THRALL, RALLY,    -1534014},
    {NPC_THRALL, RETREAT,  -1534015},
    {NPC_THRALL, WIN,      -1534016},
    //{NPC_THRALL, DEATH,    -1534017} // Only sound used
};

hyjalAI::hyjalAI(Creature* pCreature, uint32 maxActions) : CombatAI(pCreature, maxActions), m_instance(static_cast<instance_mount_hyjal*>(pCreature->GetInstanceData()))
{
    AddTimerlessCombatAction(HYJAL_AI_90, true);
    AddCombatAction(HYJAL_AI_ATTACKED_TIMER, false);
    AddCustomAction(HYJAL_AI_RALLY_TIMER, true, [&]()
    {
        DoTalk(RALLY);
        ResetTimer(HYJAL_AI_RALLY_TIMER, urand(60000, 240000));
    });
}

void hyjalAI::Reset()
{
    CombatAI::Reset();

    // Set base area based on creature entry
    switch (m_creature->GetEntry())
    {
        case NPC_JAINA:
            DoCastSpellIfCan(m_creature, SPELL_BRILLIANCE_AURA, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            break;
        case NPC_THRALL:
            break;
    }
}

void hyjalAI::EnterEvadeMode()
{
    m_creature->RemoveAllAurasOnEvade();
    m_creature->CombatStop(true);

    if (m_creature->IsAlive())
        m_creature->GetMotionMaster()->MoveTargetedHome();

    m_creature->SetLootRecipient(nullptr);
}

void hyjalAI::JustReachedHome()
{
    if (m_creature->GetEntry() == NPC_JAINA)
        DoCastSpellIfCan(m_creature, SPELL_BRILLIANCE_AURA, CAST_TRIGGERED);
}

void hyjalAI::JustRespawned()
{
    m_started = false;
    CombatAI::JustRespawned();
}

void hyjalAI::DoTalk(YellType pYellType)
{
    HyjalYells const* pYell = nullptr;

    bool bGetNext = false;

    for (const auto& i : aHyjalYell)
    {
        if (i.uiCreatureEntry == m_creature->GetEntry() && i.m_pYellType == pYellType)
        {
            if (pYellType == ATTACKED)
            {
                if (!bGetNext && urand(0, 1))
                {
                    bGetNext = true;
                    continue;
                }
            }
            else if (pYellType == RALLY)
            {
                if (!bGetNext && urand(0, 2))
                {
                    bGetNext = true;
                    continue;
                }
            }

            pYell = &i;
            break;
        }
    }

    if (pYell)
    {
        if (pYell->uiCreatureEntry == NPC_THRALL && pYell->m_pYellType == DEATH) // Thrall doesn't have a text on Death, only sound
            DoPlaySoundToSet(m_creature, 11034);
        else
            DoScriptText(pYell->m_iTextId, m_creature);
    }
}

void hyjalAI::Win()
{
    // This will despawn the won base
    if (m_instance)
        m_instance->SetData(TYPE_WIN, DONE);
}

void hyjalAI::EventStarted()
{
    m_started = true;
    ResetTimer(HYJAL_AI_RALLY_TIMER, urand(60000, 240000));
    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
}

void hyjalAI::ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 miscValue)
{
    switch (eventType)
    {
        case AI_EVENT_CUSTOM_B:
        {
            m_creature->SetCanEnterCombat(false);
            SetReactState(REACT_PASSIVE);
            DoTalk(WIN);
            if (miscValue == 1)
                m_creature->GetMotionMaster()->MoveWaypoint(miscValue);
            break;
        }
        case AI_EVENT_CUSTOM_C: // Event failed due to exceeding max number of wave mobs
            Retreat();
            break;
        case AI_EVENT_CUSTOM_D: // Boss was killed - stop yelling
            m_started = false;
            DisableTimer(HYJAL_AI_RALLY_TIMER);
            break;
        case AI_EVENT_CUSTOM_EVENTAI_B:
        {
            if (m_creature->GetEntry() != NPC_JAINA)
                return;

            if (sender != m_creature) // Only retreat if AI Event is sent by Jaina herself
                return;

            Win();
            break;
        }
        default: break;
    }
}

void hyjalAI::ExecuteAction(uint32 action)
{
    switch (action)
    {
        case HYJAL_AI_90:
            if (m_creature->GetHealthPercent() <= 90.f)
            {
                DoTalk(ATTACKED);
                ResetCombatAction(HYJAL_AI_ATTACKED_TIMER, urand(60000, 180000));
                SetActionReadyStatus(action, false);
            }
            break;
        case HYJAL_AI_ATTACKED_TIMER:
            DoTalk(ATTACKED);
            ResetCombatAction(action, urand(60000, 180000));
            break;
    }
}

void hyjalAI::Retreat()
{
    DoTalk(RETREAT);
}

void hyjalAI::JustDied(Unit* /*killer*/)
{
    DoTalk(DEATH);
}
