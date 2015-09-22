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
SDName: Ghostlands
SD%Complete: 100
SDComment: Quest support: 9212.
SDCategory: Ghostlands
EndScriptData */

/* ContentData
npc_ranger_lilatha
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_ranger_lilatha
######*/

enum
{
    SAY_START           = -1000140,
    SAY_PROGRESS1       = -1000141,
    SAY_PROGRESS2       = -1000142,
    SAY_PROGRESS3       = -1000143,
    SAY_END1            = -1000144,
    SAY_END2            = -1000145,
    CAPTAIN_ANSWER      = -1000146,

    QUEST_CATACOMBS     = 9212,
    GO_CAGE             = 181152,
    NPC_CAPTAIN_HELIOS  = 16220,
    FACTION_SMOON_E     = 1603,
};

struct npc_ranger_lilathaAI : public npc_escortAI
{
    npc_ranger_lilathaAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_goCageGuid;
    ObjectGuid m_heliosGuid;

    void MoveInLineOfSight(Unit* pUnit) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (!m_heliosGuid && pUnit->GetEntry() == NPC_CAPTAIN_HELIOS)
            {
                if (m_creature->IsWithinDistInMap(pUnit, 30.0f))
                    m_heliosGuid = pUnit->GetObjectGuid();
            }
        }

        npc_escortAI::MoveInLineOfSight(pUnit);
    }

    void WaypointReached(uint32 i) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 0:
                if (GameObject* pGoTemp = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, 10.0f))
                {
                    m_goCageGuid = pGoTemp->GetObjectGuid();
                    pGoTemp->SetGoState(GO_STATE_ACTIVE);
                }

                m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                DoScriptText(SAY_START, m_creature, pPlayer);
                break;
            case 1:
                if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_goCageGuid))
                    pGo->SetGoState(GO_STATE_READY);
                break;
            case 5:
                DoScriptText(SAY_PROGRESS1, m_creature, pPlayer);
                break;
            case 11:
                DoScriptText(SAY_PROGRESS2, m_creature, pPlayer);
                break;
            case 18:
                DoScriptText(SAY_PROGRESS3, m_creature, pPlayer);
                if (Creature* pSum1 = m_creature->SummonCreature(16342, 7627.083984f, -7532.538086f, 152.128616f, 1.082733f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pSum1->AI()->AttackStart(m_creature);
                if (Creature* pSum2 = m_creature->SummonCreature(16343, 7620.432129f, -7532.550293f, 152.454865f, 0.827478f, TEMPSUMMON_DEAD_DESPAWN, 0))
                    pSum2->AI()->AttackStart(pPlayer);
                break;
            case 19:
                SetRun();
                break;
            case 25:
                SetRun(false);
                break;
            case 30:
                pPlayer->GroupEventHappens(QUEST_CATACOMBS, m_creature);
                break;
            case 32:
                DoScriptText(SAY_END1, m_creature, pPlayer);
                break;
            case 33:
                DoScriptText(SAY_END2, m_creature, pPlayer);
                if (Creature* pHelios = m_creature->GetMap()->GetCreature(m_heliosGuid))
                    DoScriptText(CAPTAIN_ANSWER, pHelios, m_creature);
                break;
        }
    }

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_goCageGuid.Clear();
            m_heliosGuid.Clear();
        }
    }
};

CreatureAI* GetAI_npc_ranger_lilathaAI(Creature* pCreature)
{
    return new npc_ranger_lilathaAI(pCreature);
}

bool QuestAccept_npc_ranger_lilatha(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CATACOMBS)
    {
        pCreature->SetFactionTemporary(FACTION_SMOON_E, TEMPFACTION_RESTORE_RESPAWN);

        if (npc_ranger_lilathaAI* pEscortAI = dynamic_cast<npc_ranger_lilathaAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

void AddSC_ghostlands()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_ranger_lilatha";
    pNewScript->GetAI = &GetAI_npc_ranger_lilathaAI;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_ranger_lilatha;
    pNewScript->RegisterSelf();
}
