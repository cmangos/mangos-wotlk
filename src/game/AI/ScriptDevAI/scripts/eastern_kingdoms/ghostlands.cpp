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

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_ranger_lilatha
######*/

enum
{
    SAY_START                   = 15894,
    SAY_PROGRESS1               = 12187,
    SAY_PROGRESS2               = 12188,
    SAY_PROGRESS3               = 12189,
    SAY_END1                    = 12191,
    SAY_END2                    = 12190,
    CAPTAIN_ANSWER              = 12193,

    WAYPOINT_PATH_ID            = 16295,
    QUEST_CATACOMBS             = 9212,
    GO_CAGE                     = 181152,
    NPC_CAPTAIN_HELIOS          = 16220,
    NPC_MUMMIFIED_HEADHUNTER    = 16342,
    NPC_SHADOWPINE_ORACLE       = 16343,
};

struct npc_ranger_lilathaAI : public npc_escortAI
{
    npc_ranger_lilathaAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_goCageGuid;
    ObjectGuid m_heliosGuid;

    void JustRespawned() override
    {
        npc_escortAI::JustRespawned();
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

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
            case 1:
                if (GameObject* pGoTemp = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, 10.0f))
                {
                    m_goCageGuid = pGoTemp->GetObjectGuid();
                    pGoTemp->SetGoState(GO_STATE_ACTIVE);
                }
                break;
            case 2:
                if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_goCageGuid))
                    pGo->SetGoState(GO_STATE_READY);
                break;
            case 6:
                DoBroadcastText(SAY_PROGRESS1, m_creature, pPlayer);
                break;
            case 12:
                DoBroadcastText(SAY_PROGRESS2, m_creature, pPlayer);
                break;
            case 19:
                DoBroadcastText(SAY_PROGRESS3, m_creature, pPlayer);
                if (Creature* pSum1 = m_creature->SummonCreature(NPC_MUMMIFIED_HEADHUNTER, 7625.81103515625f, -7531.75439453125f, 152.361358642578125f, 1.117010712623596191f, TEMPSPAWN_DEAD_DESPAWN, 0))
                    pSum1->AI()->AttackStart(m_creature);
                if (Creature* pSum2 = m_creature->SummonCreature(NPC_SHADOWPINE_ORACLE, 7622.45849609375f, -7529.16943359375f, 152.9869384765625f, 0.907571196556091308f, TEMPSPAWN_DEAD_DESPAWN, 0))
                    pSum2->AI()->AttackStart(pPlayer);
                break;
            case 20:
                SetRun();
                break;
            case 26:
                SetRun(false);
                break;
            case 31:
                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_CATACOMBS, m_creature);
                break;
            case 33:
                DoBroadcastText(SAY_END1, m_creature, pPlayer);
                break;
            break;
            case 34:
            {
                DoBroadcastText(SAY_END2, m_creature, pPlayer);
                m_creature->ForcedDespawn(10000u);
                m_creature->SetRespawnDelay(1, true);
                if (Creature* pHelios = m_creature->GetMap()->GetCreature(m_heliosGuid))
                    DoBroadcastText(CAPTAIN_ANSWER, pHelios, m_creature);
            }
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

bool QuestAccept_npc_ranger_lilatha(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CATACOMBS)
    {
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);
        pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_IMMUNE_TO_PLAYER | TEMPFACTION_TOGGLE_IMMUNE_TO_NPC);
        DoBroadcastText(SAY_START, pCreature, pPlayer);

        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        if (npc_ranger_lilathaAI* pEscortAI = dynamic_cast<npc_ranger_lilathaAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest, false, false, WAYPOINT_PATH_ID);
    }
    return true;
}

void AddSC_ghostlands()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_ranger_lilatha";
    pNewScript->GetAI = &GetNewAIInstance<npc_ranger_lilathaAI>;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_ranger_lilatha;
    pNewScript->RegisterSelf();
}
