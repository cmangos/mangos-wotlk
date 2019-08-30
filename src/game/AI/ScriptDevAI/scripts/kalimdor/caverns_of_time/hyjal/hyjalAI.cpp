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

#include "AI/ScriptDevAI/include/precompiled.h"
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

void hyjalAI::Reset()
{
    m_uiAttackedYellTimer = 0;
    m_calledForHelp = m_creature->GetEntry() == NPC_JAINA ? false : true; // only jaina calls for help

    // Set base area based on creature entry
    switch (m_creature->GetEntry())
    {
        case NPC_JAINA:
            DoCastSpellIfCan(m_creature, SPELL_BRILLIANCE_AURA, CAST_TRIGGERED);
            break;
        case NPC_THRALL:
            break;
    }
}

void hyjalAI::EnterEvadeMode()
{
    m_creature->RemoveAllAurasOnEvade();
    m_creature->CombatStop(true);

    if (m_creature->isAlive())
        m_creature->GetMotionMaster()->MoveTargetedHome();

    m_creature->SetLootRecipient(nullptr);
}

void hyjalAI::JustReachedHome()
{
    if (m_creature->GetEntry() == NPC_JAINA)
        DoCastSpellIfCan(m_creature, SPELL_BRILLIANCE_AURA, CAST_TRIGGERED);
}

void hyjalAI::Aggro(Unit* /*who*/)
{
    for (uint8 i = 0; i < MAX_SPELL; ++i)
        if (m_aSpells[i].m_uiCooldown)
            m_uiSpellTimer[i] = m_aSpells[i].m_uiCooldown;
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
    if (m_pInstance)
        m_pInstance->SetData(TYPE_WIN, DONE);
}

void hyjalAI::EventStarted()
{
    m_uiRallyYellTimer = urand(60000, 240000);
}

void hyjalAI::ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 miscValue)
{
    switch (eventType)
    {
        case AI_EVENT_CUSTOM_B:
        {
            DoTalk(WIN);
            if (miscValue == 1)
                m_creature->GetMotionMaster()->MoveWaypoint(miscValue);
            break;
        }
        case AI_EVENT_CUSTOM_C: // Event failed due to exceeding max number of wave mobs
            Retreat();
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
    }
}

void hyjalAI::Retreat()
{
    DoTalk(RETREAT);
}

void hyjalAI::UpdateAI(const uint32 uiDiff)
{
    if (m_uiRallyYellTimer)
    {
        if (m_uiRallyYellTimer <= uiDiff)
        {
            DoTalk(RALLY);
            m_uiRallyYellTimer = urand(60000, 240000);

            // Increase Attacked yell timer to prevent them from occuring too quickly after each other
            if (m_uiAttackedYellTimer)
                m_uiAttackedYellTimer += 20000;
        }
        else
            m_uiRallyYellTimer -= uiDiff;
    }

    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        return;

    for (uint8 i = 0; i < MAX_SPELL; ++i)
    {
        if (m_aSpells[i].m_uiSpellId)
        {
            if (m_uiSpellTimer[i] < uiDiff)
            {
                if (m_creature->IsNonMeleeSpellCasted(false))
                    m_creature->InterruptNonMeleeSpells(false);

                Unit* pTarget = nullptr;

                switch (m_aSpells[i].m_pType)
                {
                    case TARGETTYPE_SELF:   pTarget = m_creature; break;
                    case TARGETTYPE_RANDOM: pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0); break;
                    case TARGETTYPE_VICTIM: pTarget = m_creature->getVictim(); break;
                }

                if (pTarget)
                {
                    DoCastSpellIfCan(pTarget, m_aSpells[i].m_uiSpellId);
                    m_uiSpellTimer[i] = m_aSpells[i].m_uiCooldown;
                }
            }
            else
                m_uiSpellTimer[i] -= uiDiff;
        }
    }

    if (m_uiAttackedYellTimer)
    {
        if (m_uiAttackedYellTimer <= uiDiff)
        {
            DoTalk(ATTACKED);
            m_uiAttackedYellTimer = urand(60000, 180000);

            // Increase Rally yell timer to prevent them from occuring too quickly after each other
            if (m_uiRallyYellTimer)
                m_uiRallyYellTimer += 20000;
        }
        else
            m_uiAttackedYellTimer -= uiDiff;
    }
    else if (m_creature->GetHealthPercent() <= 90.f)
    {
        DoTalk(ATTACKED);
        m_uiAttackedYellTimer = urand(60000, 180000);

        // Increase Rally yell timer to prevent them from occuring too quickly after each other
        if (m_uiRallyYellTimer)
            m_uiRallyYellTimer += 20000;
    }
    else if (m_creature->GetHealthPercent() <= 20.f && !m_calledForHelp)
    {
        m_calledForHelp = true;
        DoScriptText(SAY_CALL_FOR_HELP_EMOTE, m_creature);
        DoCallForHelp(30.f);
    }

    DoMeleeAttackIfReady();
}

void hyjalAI::JustRespawned()
{
    Reset();
}

void hyjalAI::JustDied(Unit* /*killer*/)
{
    DoTalk(DEATH);
}
