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
SDName: Westfall
SD%Complete: 90
SDComment: Quest support: 155, 1651
SDCategory: Westfall
EndScriptData */

/* ContentData
npc_daphne_stilwell
npc_defias_traitor
EndContentData */

#include "AI/ScriptDevAI/include/precompiled.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## npc_daphne_stilwell
######*/

enum
{
    SAY_DS_START        = -1000293,
    SAY_DS_DOWN_1       = -1000294,
    SAY_DS_DOWN_2       = -1000295,
    SAY_DS_DOWN_3       = -1000296,
    SAY_DS_PROLOGUE     = -1000297,

    SPELL_SHOOT         = 6660,
    QUEST_TOME_VALOR    = 1651,
    NPC_DEFIAS_RAIDER   = 6180,
    EQUIP_ID_RIFLE      = 2511,

    DAPHNE_SHOOT_CD     = 2000
};

enum Wave
{
    FIRST,
    SECOND,
    THIRD
};

float RaiderCoords[15][3] = 
{
    {-11428.520, 1612.757, 72.241}, // Spawn1
    {-11422.998, 1616.106, 74.153}, // Spawn2
    {-11430.354, 1618.334, 72.632}, // Spawn3
    {-11423.307, 1621.033, 74.224}, // Spawn4
    {-11427.141, 1623.220, 73.168}, // Spawn5
    
    {-11453.118, 1554.380, 53.100}, // WP1a
    {-11449.692, 1554.672, 53.598}, // WP2a
    {-11454.533, 1558.679, 52.497}, // WP3a
    {-11449.488, 1557.817, 53.443}, // WP4a
    {-11452.123, 1559.800, 52.890}, // WP5a
                                 
    {-11475.067, 1534.259, 50.199}, // WP1b
    {-11470.306, 1533.835, 50.267}, // WP2b
    {-11471.954, 1539.599, 50.273}, // WP3b
    {-11465.560, 1534.399, 50.649}, // WP4b
    {-11467.391, 1537.989, 50.726}  // WP5b
};

struct npc_daphne_stilwellAI : public npc_escortAI
{
    npc_daphne_stilwellAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiWPHolder = 0;
        Reset();
    }

    uint32 m_uiWPHolder;
    uint32 m_uiShootTimer;

    GuidList m_lSummonedRaidersGUIDs;

    void Reset() override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (m_uiWPHolder >= 5)
                m_creature->SetSheath(SHEATH_STATE_RANGED);

            switch (m_uiWPHolder)
            {
                case 7: DoScriptText(SAY_DS_DOWN_1, m_creature); break;
                case 8: DoScriptText(SAY_DS_DOWN_2, m_creature); break;
                case 9: 
                    if (m_lSummonedRaidersGUIDs.size() == 0)
                        DoScriptText(SAY_DS_DOWN_3, m_creature); 
                    break;
            }
        }
        else
            m_uiWPHolder = 0;

        m_uiShootTimer = 0;
    }

    void WaypointReached(uint32 uiPointId) override
    {
        m_uiWPHolder = uiPointId;

        switch (uiPointId)
        {
            case 4:
                m_creature->HandleEmote(EMOTE_STATE_USESTANDING_NOSHEATHE);
                break;
            case 5:
                SetEquipmentSlots(false, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE, EQUIP_ID_RIFLE);
                m_creature->SetSheath(SHEATH_STATE_RANGED);
                m_creature->HandleEmote(EMOTE_STATE_STAND);
                break;
            case 7:
                DoSendWave(Wave::FIRST);
                break;
            case 8:
                DoSendWave(Wave::SECOND);
                break;
            case 9:
                DoSendWave(Wave::THIRD);
                SetEscortPaused(true);
                break;
            case 10:
                SetRun(false);
                break;
            case 11:
                DoScriptText(SAY_DS_PROLOGUE, m_creature);
                break;
            case 13:
                SetEquipmentSlots(true);
                m_creature->SetSheath(SHEATH_STATE_UNARMED);
                m_creature->HandleEmote(EMOTE_STATE_USESTANDING);
                break;
            case 14:
                m_creature->HandleEmote(EMOTE_STATE_STAND);
                break;
            case 17:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_TOME_VALOR, m_creature);
                break;
            case 18:
                DoEndEscort();
                break;
        }
    }

    void DoSendWave(uint32 wave)
    {
        uint8 uiFirstWPOffset = 5;

        switch (wave)
        {
            case Wave::FIRST:
                for (int counter = 0; counter < 3; counter++)
                    if (Creature* pAdd = m_creature->SummonCreature(NPC_DEFIAS_RAIDER, RaiderCoords[counter][0], RaiderCoords[counter][1], RaiderCoords[counter][2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000, false, true))
                        pAdd->GetMotionMaster()->MovePoint(counter, RaiderCoords[uiFirstWPOffset + counter][0], RaiderCoords[uiFirstWPOffset + counter][1], RaiderCoords[uiFirstWPOffset + counter][2]);
                break;
            case Wave::SECOND:
                for (int counter = 0; counter < 4; counter++)
                    if (Creature* pAdd = m_creature->SummonCreature(NPC_DEFIAS_RAIDER, RaiderCoords[counter][0], RaiderCoords[counter][1], RaiderCoords[counter][2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000, false, true))
                        pAdd->GetMotionMaster()->MovePoint(counter, RaiderCoords[uiFirstWPOffset + counter][0], RaiderCoords[uiFirstWPOffset + counter][1], RaiderCoords[uiFirstWPOffset + counter][2]);
                break;
            case Wave::THIRD:
                for (int counter = 0; counter < 5; counter++)
                    if (Creature* pAdd = m_creature->SummonCreature(NPC_DEFIAS_RAIDER, RaiderCoords[counter][0], RaiderCoords[counter][1], RaiderCoords[counter][2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000, false, true))
                        pAdd->GetMotionMaster()->MovePoint(counter, RaiderCoords[uiFirstWPOffset + counter][0], RaiderCoords[uiFirstWPOffset + counter][1], RaiderCoords[uiFirstWPOffset + counter][2]);
                break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiData) override
    {
        uint8 uiSecondWPOffset = 10;

        if (pSummoned->GetEntry() == NPC_DEFIAS_RAIDER && uiMotionType == POINT_MOTION_TYPE) // sanity check
        {
            switch (uiData)
            {
                case 0:
                    pSummoned->GetMotionMaster()->MovePoint(5, RaiderCoords[uiSecondWPOffset][0], RaiderCoords[uiSecondWPOffset][1], RaiderCoords[uiSecondWPOffset][2]);
                    break;
                case 1:
                    pSummoned->GetMotionMaster()->MovePoint(5, RaiderCoords[uiSecondWPOffset+1][0], RaiderCoords[uiSecondWPOffset+1][1], RaiderCoords[uiSecondWPOffset+1][2]);
                    break;
                case 2:
                    pSummoned->GetMotionMaster()->MovePoint(5, RaiderCoords[uiSecondWPOffset+2][0], RaiderCoords[uiSecondWPOffset+2][1], RaiderCoords[uiSecondWPOffset+2][2]);
                    break;
                case 3:
                    pSummoned->GetMotionMaster()->MovePoint(5, RaiderCoords[uiSecondWPOffset+3][0], RaiderCoords[uiSecondWPOffset+3][1], RaiderCoords[uiSecondWPOffset+3][2]);
                    break;
                case 4:
                    pSummoned->GetMotionMaster()->MovePoint(5, RaiderCoords[uiSecondWPOffset+4][0], RaiderCoords[uiSecondWPOffset+4][1], RaiderCoords[uiSecondWPOffset+4][2]);
                    break;
                default:
                    pSummoned->GetMotionMaster()->MoveIdle();
                    break;
            }
        }
    }

    void Aggro(Unit* /*who*/) override
    {
        SetCombatMovement(false);
    }

    void JustReachedHome() override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) && m_uiWPHolder >= 5)
            m_creature->SetSheath(SHEATH_STATE_RANGED);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_DEFIAS_RAIDER)
        {
            m_lSummonedRaidersGUIDs.push_back(pSummoned->GetObjectGuid());
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned) override 
    {
        m_lSummonedRaidersGUIDs.remove(pSummoned->GetObjectGuid());

        if (m_uiWPHolder >= 9 && m_lSummonedRaidersGUIDs.size() == 0)
            SetEscortPaused(false);
    }

    void SummonedCreatureDespawn(Creature* pSummoned) override // just in case this happens somehow
    {
        if (pSummoned->isAlive())
            m_lSummonedRaidersGUIDs.remove(pSummoned->GetObjectGuid());
    }

    void JustDied(Unit* pKiller) override
    {
        m_lSummonedRaidersGUIDs.clear();

        npc_escortAI::JustDied(pKiller);
    }

    void DoEndEscort()
    {
        m_creature->ForcedDespawn();
        m_creature->Respawn();
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShootTimer < uiDiff)
        {
            m_uiShootTimer = DAPHNE_SHOOT_CD;

            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT) != CanCastResult::CAST_OK && !IsCombatMovement())
            {
                SetCombatMovement(true);
                DoStartMovement(m_creature->getVictim());
            }
        }
        else
            m_uiShootTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_daphne_stilwell(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOME_VALOR)
    {
        DoScriptText(SAY_DS_START, pCreature);

        if (npc_daphne_stilwellAI* pEscortAI = dynamic_cast<npc_daphne_stilwellAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer, pQuest);
    }

    return true;
}

CreatureAI* GetAI_npc_daphne_stilwell(Creature* pCreature)
{
    return new npc_daphne_stilwellAI(pCreature);
}

/*######
## npc_defias_traitor
######*/

enum
{
    SAY_START                = -1000101,
    SAY_PROGRESS             = -1000102,
    SAY_END                  = -1000103,
    SAY_AGGRO_1              = -1000104,
    SAY_AGGRO_2              = -1000105,

    QUEST_DEFIAS_BROTHERHOOD = 155
};

struct npc_defias_traitorAI : public npc_escortAI
{
    npc_defias_traitorAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 35:
                SetRun(false);
                break;
            case 36:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_PROGRESS, m_creature, pPlayer);
                break;
            case 44:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END, m_creature, pPlayer);
                    pPlayer->GroupEventHappens(QUEST_DEFIAS_BROTHERHOOD, m_creature);
                }
                break;
        }
    }

    void Aggro(Unit* pWho) override
    {
        DoScriptText(urand(0, 1) ? SAY_AGGRO_1 : SAY_AGGRO_2, m_creature, pWho);
    }

    void Reset() override { }
};

bool QuestAccept_npc_defias_traitor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DEFIAS_BROTHERHOOD)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);

        if (npc_defias_traitorAI* pEscortAI = dynamic_cast<npc_defias_traitorAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer, pQuest);
    }

    return true;
}

CreatureAI* GetAI_npc_defias_traitor(Creature* pCreature)
{
    return new npc_defias_traitorAI(pCreature);
}

void AddSC_westfall()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_daphne_stilwell";
    pNewScript->GetAI = &GetAI_npc_daphne_stilwell;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_daphne_stilwell;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_defias_traitor";
    pNewScript->GetAI = &GetAI_npc_defias_traitor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_defias_traitor;
    pNewScript->RegisterSelf();
}
