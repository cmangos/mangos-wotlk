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
SDName: Magisters_Terrace
SD%Complete: 100
SDComment: Quest support: 11490(post-event)
SDCategory: Magisters Terrace
EndScriptData */

/* ContentData
npc_kalecgos
EndContentData */

#include "precompiled.h"
#include "magisters_terrace.h"

/*######
## npc_kalecgos
######*/

enum
{
    SPELL_TRANSFORM_TO_KAEL     = 44670,
    SPELL_ORB_KILL_CREDIT       = 46307,
    NPC_KALECGOS                = 24848,                    // human form entry

    MAP_ID_MAGISTER             = 585,
};

static const float afKaelLandPoint[4] = {200.36f, -270.77f, -8.73f, 0.01f};

// This is friendly keal that appear after used Orb.
// If we assume DB handle summon, summon appear somewhere outside the platform where Orb is
struct npc_kalecgosAI : public ScriptedAI
{
    npc_kalecgosAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiTransformTimer;

    void Reset() override
    {
        // Check the map id because the same creature entry is involved in other scripted event in other instance
        if (m_creature->GetMapId() != MAP_ID_MAGISTER)
            return;

        m_uiTransformTimer = 0;

        // Move the dragon to landing point
        m_creature->GetMotionMaster()->MovePoint(1, afKaelLandPoint[0], afKaelLandPoint[1], afKaelLandPoint[2]);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId)
        {
            m_creature->SetLevitate(false);
            m_creature->SetFacingTo(afKaelLandPoint[3]);
            m_uiTransformTimer = MINUTE * IN_MILLISECONDS;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiTransformTimer)
        {
            if (m_uiTransformTimer <= uiDiff)
            {
                // Transform and update entry, now ready for quest/read gossip
                if (DoCastSpellIfCan(m_creature, SPELL_TRANSFORM_TO_KAEL) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_ORB_KILL_CREDIT, CAST_TRIGGERED);
                    m_creature->UpdateEntry(NPC_KALECGOS);

                    m_uiTransformTimer = 0;
                }
            }
            else
                m_uiTransformTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_kalecgos(Creature* pCreature)
{
    return new npc_kalecgosAI(pCreature);
}

bool ProcessEventId_event_go_scrying_orb(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_magisters_terrace* pInstance = (instance_magisters_terrace*)((Player*)pSource)->GetInstanceData())
        {
            // Check if the Dragon is already spawned and don't allow it to spawn it multiple times
            if (pInstance->GetSingleCreatureFromStorage(NPC_KALECGOS_DRAGON, true))
                return true;
        }
    }
    return false;
}

void AddSC_magisters_terrace()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_kalecgos";
    pNewScript->GetAI = &GetAI_npc_kalecgos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_go_scrying_orb";
    pNewScript->pProcessEventId = &ProcessEventId_event_go_scrying_orb;
    pNewScript->RegisterSelf();
}
