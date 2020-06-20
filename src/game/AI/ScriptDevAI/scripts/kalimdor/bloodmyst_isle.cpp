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
SDName: Bloodmyst_Isle
SD%Complete: 100
SDComment: Quest support: 9670, 9759
SDCategory: Bloodmyst Isle
EndScriptData */

/* ContentData
mob_webbed_creature
npc_demolitionist_legoso
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"

/*######
## mob_webbed_creature
######*/

enum
{
    NPC_EXPEDITION_RESEARCHER       = 17681,
};

// possible creatures to be spawned (too many to be added to enum)
const uint32 possibleSpawns[31] = {17322, 17661, 17496, 17522, 17340, 17352, 17333, 17524, 17654, 17348, 17339, 17345, 17353, 17336, 17550, 17330, 17701, 17321, 17325, 17320, 17683, 17342, 17715, 17334, 17341, 17338, 17337, 17346, 17344, 17327};

struct mob_webbed_creatureAI : public Scripted_NoMovementAI
{
    mob_webbed_creatureAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { Reset(); }

    void Reset() override { }
    void AttackStart(Unit* /*pWho*/) override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* pKiller) override
    {
        uint32 uiSpawnCreatureEntry = 0;

        switch (urand(0, 2))
        {
            case 0:
                uiSpawnCreatureEntry = NPC_EXPEDITION_RESEARCHER;
                if (pKiller->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)pKiller)->KilledMonsterCredit(uiSpawnCreatureEntry, m_creature->GetObjectGuid());
                break;
            case 1:
            case 2:
                uiSpawnCreatureEntry = possibleSpawns[urand(0, 30)];
                break;
        }

        if (uiSpawnCreatureEntry)
            if (Creature* spawnedCreature = m_creature->SummonCreature(uiSpawnCreatureEntry, 0.0f, 0.0f, 0.0f, m_creature->GetOrientation(), TEMPSPAWN_TIMED_OOC_DESPAWN, 25000))
                spawnedCreature->GetMotionMaster()->MoveRandomAroundPoint(spawnedCreature->GetPositionX(), spawnedCreature->GetPositionY(), spawnedCreature->GetPositionZ(), 5.f); // Needed because one or more of the spawned creatures has MovementType 2 by default, causing them to go flying to the first default waypoint
    }
};

UnitAI* GetAI_mob_webbed_creature(Creature* pCreature)
{
    return new mob_webbed_creatureAI(pCreature);
}

/*######
## npc_demolitionist_legoso
######*/

enum
{
    SAY_ESCORT_START            = -1000185,
    SAY_ESCORT_LEGOSO_1         = -1000186,
    SAY_ESCORT_LEGOSO_2         = -1001198,
    SAY_ESCORT_LEGOSO_3         = -1001199,
    SAY_ESCORT_LEGOSO_4         = -1001200,
    SAY_ESCORT_COUNT_3          = -1001201,
    SAY_ESCORT_COUNT_2          = -1001202,
    SAY_ESCORT_COUNT_1          = -1001203,
    SAY_ESCORT_CONTINUE         = -1001204,
    SAY_ESCORT_LEGOSO_5         = -1001205,
    SAY_ESCORT_LEGOSO_6         = -1001206,
    SAY_ESCORT_LEGOSO_7         = -1001207,
    SAY_ESCORT_LEGOSO_8         = -1001208,
    SAY_ESCORT_LEGOSO_9         = -1001209,
    SAY_ESCORT_LEGOSO_10        = -1001210,
    SAY_LEGOSO_AGGRO            = -1001211,
    SAY_ESCORT_COMPLETE_1       = -1001212,
    SAY_ESCORT_COMPLETE_2       = -1001213,

    NPC_LEGOSO                  = 17982,
    NPC_SIRONAS                 = 17678,

    GO_EXPLOSIVES_1             = 182088,
    GO_EXPLOSIVES_2             = 182091,
    GO_COIL_FIRE_L              = 182090,
    GO_COIL_FIRE_S              = 182089,

    QUEST_ENDING_THEIR_WORLD    = 9759,

    POINT_ID_EXPLOSIVES_1       = 11,
    POINT_ID_EXPLOSIVES_2       = 12,
    POINT_ID_EXPLOSIVES_3       = 13,
    POINT_ID_EXPLOSIVES_4       = 14,
    POINT_ID_EXPLOSIVES_5       = 15,
    POINT_ID_EXPLOSIVES_6       = 16,
    POINT_ID_EXPLOSIVES_7       = 17,
    POINT_ID_EXPLOSIVES_8       = 18,
    POINT_ID_EXPLOSIVES_9       = 19,

    POINT_ID_COUNT_START        = 20,
    POINT_ID_COUNT_FINISHED     = 21,

    POINT_ID_TEXT_1             = 30,
    POINT_ID_TEXT_2             = 31,
    POINT_ID_TEXT_3             = 32,
};

static const DialogueEntry aLegosoDialogue[] =
{
    {POINT_ID_EXPLOSIVES_1,     0,          10000},             // reach explosive site 1
    {POINT_ID_TEXT_1,           0,          18000},
    {SAY_ESCORT_LEGOSO_4,       NPC_LEGOSO, 0},

    {POINT_ID_COUNT_START,      0,          1000},              // countdown for explosion
    {SAY_ESCORT_COUNT_3,        NPC_LEGOSO, 1000},
    {SAY_ESCORT_COUNT_2,        NPC_LEGOSO, 1000},
    {SAY_ESCORT_COUNT_1,        NPC_LEGOSO, 1000},
    {POINT_ID_COUNT_FINISHED,   0,          0},

    {POINT_ID_EXPLOSIVES_2,     0,          1000},              // explode first site
    {POINT_ID_EXPLOSIVES_3,     0,          2000},
    {SAY_ESCORT_CONTINUE,       NPC_LEGOSO, 6000},
    {POINT_ID_EXPLOSIVES_4,     0,          0},

    {SAY_ESCORT_LEGOSO_5,       NPC_LEGOSO, 3000},
    {POINT_ID_TEXT_2,           0,          0},

    {SAY_ESCORT_LEGOSO_7,       NPC_LEGOSO, 5000},
    {SAY_ESCORT_LEGOSO_8,       NPC_LEGOSO, 0},

    {POINT_ID_EXPLOSIVES_5,     0,          10000},             // reach explosive site 2
    {SAY_ESCORT_LEGOSO_9,       NPC_LEGOSO, 17000},
    {POINT_ID_TEXT_3,           0,          4000},
    {POINT_ID_EXPLOSIVES_6,     0,          0},

    {SAY_LEGOSO_AGGRO,          NPC_LEGOSO, 1000},              // combat start
    {POINT_ID_EXPLOSIVES_7,     0,          0},

    {POINT_ID_EXPLOSIVES_8,     0,          5000},              // quest ending event
    {SAY_ESCORT_COMPLETE_1,     NPC_LEGOSO, 2000},
    {QUEST_ENDING_THEIR_WORLD,  0,          7000},
    {SAY_ESCORT_COMPLETE_2,     NPC_LEGOSO, 5000},
    {POINT_ID_EXPLOSIVES_9,     0,          0},
    {0, 0, 0},
};

struct npc_demolitionist_legosoAI : public npc_escortAI, private DialogueHelper
{
    npc_demolitionist_legosoAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aLegosoDialogue)
    {
        m_bFirstExplosives = true;
        Reset();
    }

    bool m_bFirstExplosives;

    GuidList m_lExplosivesGuidsList;

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* pSender, Unit* pInvoker, uint32 uiMiscValue) override
    {
        // start quest
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_bFirstExplosives = true;
            DoScriptText(SAY_ESCORT_START, m_creature, pInvoker);
            m_creature->SetFactionTemporary(FACTION_ESCORT_A_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            Start(true, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));
        }
        // start event epilog
        else if (eventType == AI_EVENT_CUSTOM_EVENTAI_A && pSender->GetEntry() == NPC_SIRONAS)
            StartNextDialogueText(POINT_ID_EXPLOSIVES_8);
    }

    void WaypointStart(uint32 uiWP) override
    {
        switch (uiWP)
        {
            case 24:
                DoScriptText(SAY_ESCORT_LEGOSO_2, m_creature);
                break;
        }
    }

    void WaypointReached(uint32 uiWP) override
    {
        switch (uiWP)
        {
            case 23:
                DoScriptText(SAY_ESCORT_LEGOSO_1, m_creature);
                break;
            case 24:
                // prepare first event
                SetEscortPaused(true);
                m_creature->SetFacingTo(1.334f);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                StartNextDialogueText(POINT_ID_EXPLOSIVES_1);
                break;
            case 26:
                // countdown for first event
                SetEscortPaused(true);
                m_creature->SetFacingTo(1.099f);
                StartNextDialogueText(POINT_ID_COUNT_START);
                break;
            case 34:
                m_creature->SetFacingTo(0.942f);
                StartNextDialogueText(SAY_ESCORT_LEGOSO_5);
                break;
            case 36:
                m_creature->SetFacingTo(0.862f);
                StartNextDialogueText(SAY_ESCORT_LEGOSO_7);
                break;
            case 38:
                // prepare second event
                SetEscortPaused(true);
                m_creature->SetFacingTo(5.961f);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                StartNextDialogueText(POINT_ID_EXPLOSIVES_5);
                break;
            case 39:
                // countdown for second event
                m_creature->SetFacingTo(5.654f);
                StartNextDialogueText(POINT_ID_COUNT_START);
                break;
            case 40:
                SetEscortPaused(true);
                StartNextDialogueText(SAY_LEGOSO_AGGRO);
                break;
        }
    }

    // Function to respawn GOs
    void DoRespawnObjects(uint32 uiEntry, float fRange)
    {
        GameObjectList lBarrelsInRange;
        m_lExplosivesGuidsList.clear();
        GetGameObjectListWithEntryInGrid(lBarrelsInRange, m_creature, uiEntry, fRange);

        if (lBarrelsInRange.empty())
        {
            script_error_log("Bloodmyst Isle: ERROR Failed to find any gameobjects of entry %u in range.", uiEntry);
            return;
        }

        // respawn explosives and store for future use
        for (GameObjectList::const_iterator itr = lBarrelsInRange.begin(); itr != lBarrelsInRange.end(); ++itr)
        {
            (*itr)->SetRespawnTime(5 * MINUTE);
            (*itr)->Refresh();
            m_lExplosivesGuidsList.push_back((*itr)->GetObjectGuid());
        }
    }

    // Function to use the recently spawned GOs
    void DoUseExplosiveTraps()
    {
        for (GuidList::const_iterator itr = m_lExplosivesGuidsList.begin(); itr != m_lExplosivesGuidsList.end(); ++itr)
        {
            if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(*itr))
            {
                pTrap->SendGameObjectCustomAnim(pTrap->GetObjectGuid());
                pTrap->SetLootState(GO_JUST_DEACTIVATED);
            }
        }
    }

    // Function to respawn fire GOs
    void DoRespawnFires(bool bFirstEvent)
    {
        GameObjectList lFiresInRange;
        GetGameObjectListWithEntryInGrid(lFiresInRange, m_creature, GO_COIL_FIRE_L, 110.0f);
        GetGameObjectListWithEntryInGrid(lFiresInRange, m_creature, GO_COIL_FIRE_S, 110.0f);

        if (lFiresInRange.empty())
        {
            script_error_log("Bloodmyst Isle: ERROR Failed to find any gameobjects of entry %u and %u in range.", GO_COIL_FIRE_L, GO_COIL_FIRE_S);
            return;
        }

        for (GameObjectList::const_iterator itr = lFiresInRange.begin(); itr != lFiresInRange.end(); ++itr)
        {
            if ((bFirstEvent && (*itr)->GetPositionZ() < 150.0f) || (!bFirstEvent && (*itr)->GetPositionZ() > 150.0f))
            {
                (*itr)->SetRespawnTime(5 * MINUTE);
                (*itr)->Refresh();
            }
        }
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
            case POINT_ID_TEXT_1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_ESCORT_LEGOSO_3, m_creature, pPlayer);
                break;
            case POINT_ID_TEXT_2:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_ESCORT_LEGOSO_6, m_creature, pPlayer);
                break;
            case POINT_ID_TEXT_3:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_ESCORT_LEGOSO_10, m_creature, pPlayer);
                break;
            case SAY_ESCORT_LEGOSO_4:
                // first event
                SetEscortPaused(false);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->HandleEmote(EMOTE_STATE_NONE);

                DoRespawnObjects(GO_EXPLOSIVES_1, 10.0f);
                break;
            case POINT_ID_COUNT_FINISHED:
                // different actions depending on the event
                if (m_bFirstExplosives)
                {
                    StartNextDialogueText(POINT_ID_EXPLOSIVES_2);
                    m_bFirstExplosives = false;
                }
                break;
            case POINT_ID_EXPLOSIVES_2:
                DoUseExplosiveTraps();
                break;
            case POINT_ID_EXPLOSIVES_3:
                m_creature->HandleEmote(EMOTE_ONESHOT_CHEER_NOSHEATHE);
                break;
            case POINT_ID_EXPLOSIVES_4:
                DoRespawnFires(true);
                SetEscortPaused(false);
                break;
            case POINT_ID_EXPLOSIVES_6:
                // second event
                SetEscortPaused(false);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                DoRespawnObjects(GO_EXPLOSIVES_2, 10.0f);
                break;
            case SAY_LEGOSO_AGGRO:
                // explode traps and start combat event
                DoUseExplosiveTraps();
                if (Creature* pSironas = GetClosestCreatureWithEntry(m_creature, NPC_SIRONAS, 40.0f))
                {
                    m_creature->SetFacingToObject(pSironas);
                    pSironas->InterruptNonMeleeSpells(false);
                    pSironas->RemoveAllAurasOnEvade();
                    // ToDo: find the proper spell to scale up Sironas
                }
                break;
            case POINT_ID_EXPLOSIVES_7:
                DoRespawnFires(false);
                if (Creature* pSironas = GetClosestCreatureWithEntry(m_creature, NPC_SIRONAS, 40.0f))
                {
                    pSironas->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PLAYER);
                    pSironas->AI()->AttackStart(m_creature);
                }
                break;
            case QUEST_ENDING_THEIR_WORLD:
                m_creature->HandleEmote(EMOTE_ONESHOT_CHEER_NOSHEATHE);
                // complete quest
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ENDING_THEIR_WORLD, m_creature);
                break;
            case POINT_ID_EXPLOSIVES_9:
                // event complete
                SetEscortPaused(false);
                break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        if (uiEntry == NPC_LEGOSO)
            return m_creature;

        return nullptr;
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        // Combat check
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // ToDo: research if there are any abilities involved

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_demolitionist_legoso(Creature* pCreature)
{
    return new npc_demolitionist_legosoAI(pCreature);
}

bool QuestAccept_npc_demolitionist_legoso(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ENDING_THEIR_WORLD)
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());

    return true;
}

void AddSC_bloodmyst_isle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_webbed_creature";
    pNewScript->GetAI = &GetAI_mob_webbed_creature;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_demolitionist_legoso";
    pNewScript->GetAI = &GetAI_npc_demolitionist_legoso;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_demolitionist_legoso;
    pNewScript->RegisterSelf();
}
