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

/*
SD2 file for global quests scripts
*/

#include "AI/ScriptDevAI/include/sc_common.h"

enum
{
    NPC_AKAMA                               = 22990,
    NPC_VAGATH                              = 23152,
    NPC_ASHTONGUE_DEATHSWORN                = 21701,
    NPC_SHADOWLORD                          = 22988,
    NPC_MAIEV                               = 22989,
    
    RELAY_SCRIPT_AKAMA                      = 10135,

    QUEST_DISTRACTION_FOR_AKAMA             = 10985,
    QUEST_VIALS_OF_ETERNITY                 = 10445,
    QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON    = 10888,
    QUEST_CUDGEL_OF_KARDESH                 = 10901,

    TITLE_CHAMPION_OF_THE_NAARU             = 53,
    TITLE_HAND_OF_ADAL                      = 64,
};

bool QuestRewarded_tbc_attunement_titles(Player* player, Creature* /*creature*/, Quest const* quest)
{
    switch (quest->GetQuestId())
    {
        case QUEST_CUDGEL_OF_KARDESH:
            if (player->GetQuestStatus(QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_CHAMPION_OF_THE_NAARU);
            return true;
        case QUEST_DISTRACTION_FOR_AKAMA:
            if (player->GetQuestStatus(QUEST_VIALS_OF_ETERNITY) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_HAND_OF_ADAL);
            return true;
        case QUEST_VIALS_OF_ETERNITY:
            if (player->GetQuestStatus(QUEST_DISTRACTION_FOR_AKAMA) == QUEST_STATUS_COMPLETE)
                player->SetTitle(TITLE_HAND_OF_ADAL);
            return true;
    }

    return false;
}

float DeathswornSpawnCoords[8][3] = {
    { -3562.689f, 696.251f, -8.158f },
    { -3559.341f, 699.388f, -8.877f },
    { -3567.493f, 699.383f, -8.488f },
    { -3556.366f, 703.439f, -9.633f },
    { -3569.277f, 702.603f, -8.984f },
    { -3554.079f, 707.530f, -10.408f },
    { -3571.522f, 706.726f, -9.5219f },
    { -3562.928f, 704.436f, -9.5568f } };

float ShadowlordSpawnCoords[6][3] = {
    { -3612.274f, 400.077f, 32.444f },
    { -3591.716f, 392.962f, 32.895f },
    { -3601.192f, 403.606f, 32.621f },
    { -3513.504f, 410.263f, 30.440f },
    { -3515.933f, 376.395f, 33.786f },
    { -3508.507f, 392.346f, 32.418f } };

float* DeathswornPackLeader = DeathswornSpawnCoords[0];
float* DeathswornLeft1 = DeathswornSpawnCoords[1];
float* DeathswornRight1 = DeathswornSpawnCoords[2];
float* DeathswornLeft2 = DeathswornSpawnCoords[3];
float* DeathswornRight2 = DeathswornSpawnCoords[4];
float* DeathswornLeft3 = DeathswornSpawnCoords[5];
float* DeathswornRight3 = DeathswornSpawnCoords[6];
float* DeathswornCenter = DeathswornSpawnCoords[7];

float* ShadowlordLeader1 = ShadowlordSpawnCoords[2];
float* ShadowlordLeft1 = ShadowlordSpawnCoords[0];
float* ShadowlordRight1 = ShadowlordSpawnCoords[1];
float* ShadowlordLeader2 = ShadowlordSpawnCoords[5];
float* ShadowlordLeft2 = ShadowlordSpawnCoords[4];
float* ShadowlordRight2 = ShadowlordSpawnCoords[3];

struct npc_xiri : public ScriptedAI
{
    npc_xiri(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    GuidList m_lSummonedDeathsworn;
    ObjectGuid m_leaderGUID;

    bool m_bAkamaInteractionPerformed;
    bool m_bVagathDead;

    uint32 m_uiAkamaNextWaypoint;

    void Reset() override
    {
        m_lSummonedDeathsworn.clear();
        m_leaderGUID.Clear();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        /* A:
           From xiri = introduce Akama and friends
           From Akama = Akama running RP dialog
           B = RP dialog over
           C = Shadowlords join the party
         */
        if (eventType == AI_EVENT_CUSTOM_EVENTAI_A)
        {
            if (sender->GetEntry() == m_creature->GetEntry())
            {
                m_bAkamaInteractionPerformed = false;
                m_bVagathDead = false;

                // Follow the leader crap
                if (Creature *leader = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornPackLeader[0], DeathswornPackLeader[1], DeathswornPackLeader[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                {
                    m_leaderGUID = leader->GetObjectGuid();

                    if (Creature *l1 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornLeft1[0], DeathswornLeft1[1], DeathswornLeft1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                    {
                        l1->GetMotionMaster()->MoveFollow(leader, l1->GetDistance(leader), M_PI_F / 2 + l1->GetAngle(leader), true);

                        if (Creature *l2 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornLeft2[0], DeathswornLeft2[1], DeathswornLeft2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                        {
                            l2->GetMotionMaster()->MoveFollow(l1, l2->GetDistance(l1), M_PI_F / 2 + l2->GetAngle(l1), true);

                            if (Creature *l3 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornLeft3[0], DeathswornLeft3[1], DeathswornLeft3[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                                l3->GetMotionMaster()->MoveFollow(l2, l3->GetDistance(l2), M_PI_F / 2 + l3->GetAngle(l2), true);
                        }
                    }
                    if (Creature *r1 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornRight1[0], DeathswornRight1[1], DeathswornRight1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                    {
                        r1->GetMotionMaster()->MoveFollow(leader, r1->GetDistance(leader), M_PI_F / 2 + r1->GetAngle(leader), true);

                        if (Creature *r2 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornRight2[0], DeathswornRight2[1], DeathswornRight2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                        {
                            r2->GetMotionMaster()->MoveFollow(r1, r2->GetDistance(r1), M_PI_F / 2 + r2->GetAngle(r1), true);

                            if (Creature *r3 = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornRight3[0], DeathswornRight3[1], DeathswornRight3[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                                r3->GetMotionMaster()->MoveFollow(r2, r3->GetDistance(r2), M_PI_F / 2 + r3->GetAngle(r2), true);
                        }
                    }

                    if (Creature *c = m_creature->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, DeathswornCenter[0], DeathswornCenter[1], DeathswornCenter[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                        c->GetMotionMaster()->MoveFollow(leader, c->GetDistance(leader), M_PI_F / 2 + c->GetAngle(leader), true);

                    //leader->GetMotionMaster()->MoveWaypoint(0, 2);
                }
            }
            else if (sender->GetEntry() == NPC_AKAMA && !m_bAkamaInteractionPerformed && m_bVagathDead)
            {
                Creature* akama = GetClosestCreatureWithEntry(m_creature, NPC_AKAMA, 300.0f);

                if (akama && !akama->IsInCombat() && !akama->GetCombatManager().IsInEvadeMode())
                {
                    akama->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                    akama->AI()->SetReactState(REACT_PASSIVE);
                }

                if (m_leaderGUID)
                    if (Creature* deathswornLeader = m_creature->GetMap()->GetCreature(m_leaderGUID))
                    {
                        deathswornLeader->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                    }

                RunAkamaRPEvent();
            }
        }
        else if (eventType == AI_EVENT_CUSTOM_EVENTAI_B)
        {
            Creature* akama = GetClosestCreatureWithEntry(m_creature, NPC_AKAMA, 300.0f);

            if (akama)
            {
                akama->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                akama->AI()->SetReactState(REACT_AGGRESSIVE);
            }

            if (m_leaderGUID)
                if (Creature* deathswornLeader = m_creature->GetMap()->GetCreature(m_leaderGUID))
                {
                    deathswornLeader->clearUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                }
        }
        else if (eventType == AI_EVENT_CUSTOM_EVENTAI_C)
        {
            if (sender->GetEntry() == m_creature->GetEntry())
            {
                if (Creature *leader = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordLeader1[0], ShadowlordLeader1[1], ShadowlordLeader1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                {
                    leader->SetWalk(true);
                    leader->GetMotionMaster()->MoveWaypoint(0, 2);

                    if (Creature *left = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordLeft1[0], ShadowlordLeft1[1], ShadowlordLeft1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                    {
                        left->GetMotionMaster()->MoveFollow(leader, left->GetDistance(leader), M_PI_F / 2 + left->GetAngle(leader), true);

                        if (Creature *right = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordRight1[0], ShadowlordRight1[1], ShadowlordRight1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                            right->GetMotionMaster()->MoveFollow(leader, right->GetDistance(leader), M_PI_F / 2 + right->GetAngle(leader), true);
                    }
                }

                if (Creature *leader = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordLeader2[0], ShadowlordLeader2[1], ShadowlordLeader2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                {
                    leader->SetWalk(true);
                    leader->GetMotionMaster()->MoveWaypoint(1, 2);

                    if (Creature *left = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordLeft2[0], ShadowlordLeft2[1], ShadowlordLeft2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                    {
                        left->GetMotionMaster()->MoveFollow(leader, left->GetDistance(leader), M_PI_F / 2 + left->GetAngle(leader), true);

                        if (Creature *right = m_creature->SummonCreature(NPC_SHADOWLORD, ShadowlordRight2[0], ShadowlordRight2[1], ShadowlordRight2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 120000, true, true))
                            right->GetMotionMaster()->MoveFollow(leader, right->GetDistance(leader), M_PI_F / 2 + right->GetAngle(leader), true);
                    }
                }
            }
        }
        else if (eventType == AI_EVENT_JUST_DIED)
        {
            if (sender->GetEntry() == NPC_VAGATH)
            {
                Creature* akama = GetClosestCreatureWithEntry(m_creature, NPC_AKAMA, 300.0f);

                if (akama && !akama->IsInCombat() && !akama->GetCombatManager().IsInEvadeMode())
                {
                    akama->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);

                    if (m_leaderGUID)
                        if (Creature* deathswornLeader = m_creature->GetMap()->GetCreature(m_leaderGUID))
                        {
                            deathswornLeader->addUnitState(UNIT_STAT_WAYPOINT_PAUSED);
                        }

                    RunAkamaRPEvent();
                }
                else if (Creature* maiev = GetClosestCreatureWithEntry(m_creature, NPC_MAIEV, 300.0f))
                    maiev->ForcedDespawn(2000); // despawn maiev if akama not present to do RP

                m_bVagathDead = true;
            }
        }
    }

    void RunAkamaRPEvent()
    {
        m_creature->GetMap()->ScriptsStart(SCRIPT_TYPE_RELAY, RELAY_SCRIPT_AKAMA, m_creature, m_creature);

        m_bAkamaInteractionPerformed = true;
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 /*uiMotionType*/, uint32 uiPointId) override
    {
        if (/*pSummoned->GetObjectGuid() == m_leaderGUID*/pSummoned->GetEntry() == NPC_ASHTONGUE_DEATHSWORN && uiPointId == 5)
        {
            for (GuidList::const_iterator itr = m_lSummonedDeathsworn.begin(); itr != m_lSummonedDeathsworn.end(); ++itr)
                if (Creature* pDeathsworm = m_creature->GetMap()->GetCreature(*itr))
                    pDeathsworm->ForcedDespawn();

            m_lSummonedDeathsworn.clear();
            m_leaderGUID.Clear();
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_ASHTONGUE_DEATHSWORN)
            m_lSummonedDeathsworn.push_back(pSummoned->GetObjectGuid());
    }

    void UpdateAI(const uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);
    }
};

UnitAI* GetAI_npc_xiri(Creature* pCreature)
{
    return new npc_xiri(pCreature);
}

void AddSC_quests_scripts()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_quest_attunement";
    pNewScript->GetAI = &GetAI_npc_xiri;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_tbc_attunement_titles;
    pNewScript->RegisterSelf();
}