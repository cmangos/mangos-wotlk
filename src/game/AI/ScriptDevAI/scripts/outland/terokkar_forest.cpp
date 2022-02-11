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
SDName: Terokkar_Forest
SD%Complete: 80
SDComment: Quest support: 9889, 10009, 10051, 10052, 10873, 10879, 10887, 10896, 10898, 10922, 10988, 11085, 11093, 11096.
SDCategory: Terokkar Forest
EndScriptData */

/* ContentData
mob_unkor_the_ruthless
mob_netherweb_victim
npc_akuno
npc_hungry_nether_ray
npc_letoll
npc_isla_starmane
npc_skywing
npc_cenarion_sparrowhawk
npc_skyguard_prisoner
EndContentData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/ScriptDevAI/base/pet_ai.h"

/*######
## mob_unkor_the_ruthless
######*/

enum
{
    SAY_SUBMIT                  = -1000194,

    FACTION_FRIENDLY            = 35,

    SPELL_PULVERIZE             = 2676,
    SPELL_QUID9889              = 32174,
};

struct mob_unkor_the_ruthlessAI : public ScriptedAI
{
    mob_unkor_the_ruthlessAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bCanDoQuest;
    uint32 m_uiUnfriendlyTimer;
    uint32 m_uiPulverizeTimer;
    uint32 m_uiFriendlyTimer;

    void Reset() override
    {
        m_bCanDoQuest       = false;
        m_uiUnfriendlyTimer = 0;
        m_uiFriendlyTimer   = 0;
        m_uiPulverizeTimer  = 3000;
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void DoNice()
    {
        DoScriptText(SAY_SUBMIT, m_creature);
        m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_REACH_HOME);
        m_creature->SetStandState(UNIT_STAND_STATE_SIT);
        m_creature->RemoveAllAuras();
        m_creature->CombatStop(true);
        m_uiUnfriendlyTimer = 60000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Reset npc on timer
        if (m_uiUnfriendlyTimer)
        {
            if (m_uiUnfriendlyTimer <= uiDiff)
                EnterEvadeMode();
            else
                m_uiUnfriendlyTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Do quest kill credit at 30%
        if (!m_bCanDoQuest && m_creature->GetHealthPercent() < 30.0f)
        {
            DoCastSpellIfCan(m_creature, SPELL_QUID9889, CAST_TRIGGERED);
            m_uiFriendlyTimer = 1000;
            m_bCanDoQuest = true;
        }

        // Set faction right after the spell is casted, in order to avoid any issues
        if (m_uiFriendlyTimer)
        {
            if (m_uiFriendlyTimer <= uiDiff)
            {
                DoNice();
                m_uiFriendlyTimer = 0;
            }
            else
                m_uiFriendlyTimer -= uiDiff;
        }

        if (m_uiPulverizeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_PULVERIZE);
            m_uiPulverizeTimer = 9000;
        }
        else
            m_uiPulverizeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_mob_unkor_the_ruthless(Creature* pCreature)
{
    return new mob_unkor_the_ruthlessAI(pCreature);
}

/*######
## mob_netherweb_victim
######*/

enum
{
    NPC_FREED_WARRIOR       = 22459,
    QUEST_TAKEN_IN_NIGHT    = 10873
                              // SPELL_FREE_WEBBED       = 38950
};

const uint32 netherwebVictims[6] =
{
    18470, 16805, 21242, 18452, 22482, 21285
};

struct mob_netherweb_victimAI : public ScriptedAI
{
    mob_netherweb_victimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    void Reset() override { }
    void MoveInLineOfSight(Unit* /*pWho*/) override { }

    void JustDied(Unit* pKiller) override
    {
        if (Player* pPlayer = pKiller->GetBeneficiaryPlayer())
        {
            if (pPlayer->GetQuestStatus(QUEST_TAKEN_IN_NIGHT) == QUEST_STATUS_INCOMPLETE)
            {
                if (!urand(0, 3))
                {
                    m_creature->SummonCreature(NPC_FREED_WARRIOR, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 60000);
                    pPlayer->KilledMonsterCredit(NPC_FREED_WARRIOR, m_creature->GetObjectGuid());
                }
                else
                    m_creature->SummonCreature(netherwebVictims[urand(0, 5)], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
            }
        }
    }
};

UnitAI* GetAI_mob_netherweb_victim(Creature* pCreature)
{
    return new mob_netherweb_victimAI(pCreature);
}

/*#####
## npc_akuno
#####*/

enum
{
    SAY_AKU_START           = -1000477,
    SAY_AKU_AMBUSH_A        = -1000478,
    SAY_AKU_AMBUSH_B        = -1000479,
    SAY_AKU_AMBUSH_B_REPLY  = -1000480,
    SAY_AKU_COMPLETE        = -1000481,

    SPELL_CHAIN_LIGHTNING   = 39945,

    QUEST_ESCAPING_TOMB     = 10887,
    NPC_CABAL_SKIRMISHER    = 21661
};

static float m_afAmbushB1[] = { -2895.525879f, 5336.431641f, -11.800f};
static float m_afAmbushB2[] = { -2890.604980f, 5331.938965f, -11.282f};

struct npc_akunoAI : public npc_escortAI
{
    npc_akunoAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiChainLightningTimer;

    void Reset() override
    {
        m_uiChainLightningTimer = 1000;
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 6:
                DoScriptText(SAY_AKU_AMBUSH_A, m_creature);
                m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                break;
            case 15:
                DoScriptText(SAY_AKU_AMBUSH_B, m_creature);

                if (Creature* pTemp = m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB1[0], m_afAmbushB1[1], m_afAmbushB1[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000))
                    DoScriptText(SAY_AKU_AMBUSH_B_REPLY, pTemp);

                m_creature->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB2[0], m_afAmbushB2[1], m_afAmbushB2[2], 0.0f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 25000);
                break;
            case 16:
                SetRun();
                break;
            case 19:
                DoScriptText(SAY_AKU_COMPLETE, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ESCAPING_TOMB, m_creature);

                break;
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_akuno(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_TOMB)
    {
        if (npc_akunoAI* pEscortAI = dynamic_cast<npc_akunoAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);

            DoScriptText(SAY_AKU_START, pCreature);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

UnitAI* GetAI_npc_akuno(Creature* pCreature)
{
    return new npc_akunoAI(pCreature);
}

/*######
## npc_hungry_nether_ray
######*/

enum
{
    EMOTE_FEED                  = -1000628,
    NPC_BLACK_WARP_CHASER       = 23219,
    SPELL_FEED_CREDIT           = 41427,                    // credit for quest 11093
};

struct npc_hungry_nether_rayAI : public ScriptedPetAI
{
    npc_hungry_nether_rayAI(Creature* pCreature) : ScriptedPetAI(pCreature) { Reset(); }

    uint32 m_uiFeedTimer;
    uint8 m_uiFeedCounter;
    bool m_bFeeding;

    void Reset() override
    {
        m_uiFeedTimer = 0;
        m_uiFeedCounter = 0;
        m_bFeeding = false;
        SetReactState(REACT_PASSIVE);
    }

    void OwnerKilledUnit(Unit* pVictim) override
    {
        if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetEntry() == NPC_BLACK_WARP_CHASER)
        {
            if (m_creature->IsWithinDistInMap(pVictim, 30.0f))
            {
                m_creature->CastSpell(m_creature, SPELL_FEED_CREDIT, TRIGGERED_OLD_TRIGGERED);
                m_bFeeding = true;
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MovePoint(1, pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());
            }
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE || !uiPointId)
            return;

        m_uiFeedTimer = 3000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_bFeeding)
        {
            if (m_uiFeedTimer)
            {
                if (m_uiFeedTimer <= uiDiff)
                {
                    m_uiFeedCounter++;

                    if (m_uiFeedCounter == 3)
                        DoScriptText(EMOTE_FEED, m_creature);

                    if (m_uiFeedCounter == 5)
                    {
                        m_uiFeedCounter = 0;
                        m_uiFeedTimer = 0;
                        m_bFeeding = false;
                    }
                    else
                    {
                        m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
                        m_uiFeedTimer = 2000;
                    }
                }
                else
                    m_uiFeedTimer -= uiDiff;
            }
        }
        else
            ScriptedPetAI::UpdateAI(uiDiff);
    }
};

UnitAI* GetAI_npc_hungry_nether_ray(Creature* pCreature)
{
    return new npc_hungry_nether_rayAI(pCreature);
}

/*######
## npc_letoll
######*/

enum
{
    SAY_LE_START                    = -1000511,
    SAY_LE_KEEP_SAFE                = -1000512,
    SAY_LE_NORTH                    = -1000513,
    SAY_LE_ARRIVE                   = -1000514,
    SAY_LE_BURIED                   = -1000515,
    SAY_LE_ALMOST                   = -1000516,
    SAY_LE_DRUM                     = -1000517,
    SAY_LE_DRUM_REPLY               = -1000518,
    SAY_LE_DISCOVERY                = -1000519,
    SAY_LE_DISCOVERY_REPLY          = -1000520,
    SAY_LE_NO_LEAVE                 = -1000521,
    SAY_LE_NO_LEAVE_REPLY1          = -1000522,
    SAY_LE_NO_LEAVE_REPLY2          = -1000523,
    SAY_LE_NO_LEAVE_REPLY3          = -1000524,
    SAY_LE_NO_LEAVE_REPLY4          = -1000525,
    SAY_LE_SHUT                     = -1000526,
    SAY_LE_REPLY_HEAR               = -1000527,
    SAY_LE_IN_YOUR_FACE             = -1000528,
    SAY_LE_HELP_HIM                 = -1000529,
    EMOTE_LE_PICK_UP                = -1000530,
    SAY_LE_THANKS                   = -1000531,

    QUEST_DIGGING_BONES             = 10922,

    NPC_RESEARCHER                  = 22464,
    NPC_BONE_SIFTER                 = 22466,

    MAX_RESEARCHER                  = 4
};

// Some details still missing from here, and will also have issues if followers evade for any reason.
struct npc_letollAI : public npc_escortAI
{
    npc_letollAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;
        Reset();
    }

    CreatureList m_lResearchersList;

    uint32 m_uiEventTimer;
    uint32 m_uiEventCount;

    void Reset() override {}

    // will make them follow, but will only work until they enter combat with any unit
    void SetFormation()
    {
        uint32 uiCount = 0;

        for (auto& itr : m_lResearchersList)
        {
            float fAngle = uiCount < MAX_RESEARCHER ? M_PI / MAX_RESEARCHER - (uiCount * 2 * M_PI / MAX_RESEARCHER) : 0.0f;

            if (itr->IsAlive() && !itr->IsInCombat())
                itr->GetMotionMaster()->MoveFollow(m_creature, 2.5f, fAngle);

            ++uiCount;
        }
    }

    Creature* GetAvailableResearcher(uint8 uiListNum)
    {
        if (!m_lResearchersList.empty())
        {
            uint8 uiNum = 1;

            for (auto& itr : m_lResearchersList)
            {
                if (uiListNum && uiListNum != uiNum)
                {
                    ++uiNum;
                    continue;
                }

                if (itr->IsAlive() && itr->IsWithinDistInMap(m_creature, 20.0f))
                    return itr;
            }
        }

        return nullptr;
    }

    void JustStartedEscort() override
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;

        m_lResearchersList.clear();

        GetCreatureListWithEntryInGrid(m_lResearchersList, m_creature, NPC_RESEARCHER, 25.0f);

        if (!m_lResearchersList.empty())
            SetFormation();
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_LE_KEEP_SAFE, m_creature, pPlayer);
                break;
            case 2:
                DoScriptText(SAY_LE_NORTH, m_creature);
                break;
            case 11:
                DoScriptText(SAY_LE_ARRIVE, m_creature);
                break;
            case 13:
                DoScriptText(SAY_LE_BURIED, m_creature);
                SetEscortPaused(true);
                break;
            case 14:
                SetRun();
                break;
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->IsInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
            DoScriptText(SAY_LE_HELP_HIM, m_creature);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (pPlayer && pPlayer->IsAlive())
            pSummoned->AI()->AttackStart(pPlayer);
        else
            pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiEventTimer < uiDiff)
                {
                    m_uiEventTimer = 7000;

                    switch (m_uiEventCount)
                    {
                        case 0:
                            DoScriptText(SAY_LE_ALMOST, m_creature);
                            break;
                        case 1:
                            DoScriptText(SAY_LE_DRUM, m_creature);
                            break;
                        case 2:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
                            break;
                        case 3:
                            DoScriptText(SAY_LE_DISCOVERY, m_creature);
                            break;
                        case 4:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
                            break;
                        case 5:
                            DoScriptText(SAY_LE_NO_LEAVE, m_creature);
                            break;
                        case 6:
                            if (Creature* pResearcher = GetAvailableResearcher(1))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
                            break;
                        case 7:
                            if (Creature* pResearcher = GetAvailableResearcher(2))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
                            break;
                        case 8:
                            if (Creature* pResearcher = GetAvailableResearcher(3))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
                            break;
                        case 9:
                            if (Creature* pResearcher = GetAvailableResearcher(4))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
                            break;
                        case 10:
                            DoScriptText(SAY_LE_SHUT, m_creature);
                            break;
                        case 11:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
                            break;
                        case 12:
                            DoScriptText(SAY_LE_IN_YOUR_FACE, m_creature);
                            m_creature->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000);
                            break;
                        case 13:
                            DoScriptText(EMOTE_LE_PICK_UP, m_creature);

                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                DoScriptText(SAY_LE_THANKS, m_creature, pPlayer);
                                pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_DIGGING_BONES, m_creature);
                            }

                            SetEscortPaused(false);
                            break;
                    }

                    ++m_uiEventCount;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_letoll(Creature* pCreature)
{
    return new npc_letollAI(pCreature);
}

bool QuestAccept_npc_letoll(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DIGGING_BONES)
    {
        if (npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_LE_START, pCreature);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

            pEscortAI->Start(false, pPlayer, pQuest, true);
        }
    }

    return true;
}

/*#####
## npc_isla_starmane
##
##  TODO: Verify SpellIDs, Research Timers, Finish Text?
#####*/

enum
{
    QUEST_ESCAPE_FROM_FIREWING_POINT_A  = 10051,
    QUEST_ESCAPE_FROM_FIREWING_POINT_H  = 10052,

    SAY_ISLA_PERIODIC_1                 = -1000629,
    SAY_ISLA_PERIODIC_2                 = -1000630,
    SAY_ISLA_PERIODIC_3                 = -1000631,
    SAY_ISLA_START                      = -1000632,
    SAY_ISLA_WAITING                    = -1000633,
    SAY_ISLA_LEAVE_BUILDING             = -1000634,

    GO_CAGE                             = 182794,

    SPELL_ENTANGLING_ROOTS              = 33844,            // these spell IDs seem to deal not enough dmg, but are linked
    SPELL_MOONFIRE                      = 15798,
    SPELL_WRATH                         = 9739,
    SPELL_TRAVELFORM                    = 32447             // guesswork
};

struct npc_isla_starmaneAI : public npc_escortAI
{
    npc_isla_starmaneAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiPeriodicTalkTimer;
    uint32 m_uiEntanglingRootsTimer;
    uint32 m_uiMoonfireTimer;
    uint32 m_uiWrathTimer;

    void Reset() override
    {
        m_uiPeriodicTalkTimer = urand(20000, 40000);
        m_uiEntanglingRootsTimer = 100;
        m_uiMoonfireTimer = 1600;
        m_uiWrathTimer = 2000;

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void JustStartedEscort() override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        DoScriptText(SAY_ISLA_START, m_creature);
        if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, 2 * INTERACTION_DISTANCE))
            pCage->Use(m_creature);
    }

    void WaypointStart(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 7:  DoScriptText(SAY_ISLA_LEAVE_BUILDING, m_creature); break;
            case 68: DoCastSpellIfCan(m_creature, SPELL_TRAVELFORM);    break;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 6:
                DoScriptText(SAY_ISLA_WAITING, m_creature);
                break;
            case 61:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(pPlayer->GetTeam() == ALLIANCE ? QUEST_ESCAPE_FROM_FIREWING_POINT_A : QUEST_ESCAPE_FROM_FIREWING_POINT_H, m_creature);
                break;
            case 67:
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (m_uiPeriodicTalkTimer < uiDiff)
            {
                m_uiPeriodicTalkTimer = urand(30000, 60000);
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_ISLA_PERIODIC_1, m_creature); break;
                    case 1: DoScriptText(SAY_ISLA_PERIODIC_2, m_creature); break;
                    case 2: DoScriptText(SAY_ISLA_PERIODIC_3, m_creature); break;
                }
            }
            else
                m_uiPeriodicTalkTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiEntanglingRootsTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_ENTANGLING_ROOTS) == CAST_OK)
                m_uiEntanglingRootsTimer = urand(8000, 16000);
        }
        else
            m_uiEntanglingRootsTimer -= uiDiff;

        if (m_uiMoonfireTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_MOONFIRE) == CAST_OK)
                m_uiMoonfireTimer = urand(6000, 12000);
        }
        else
            m_uiMoonfireTimer -= uiDiff;

        if (m_uiWrathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_WRATH) == CAST_OK)
                m_uiWrathTimer = 2000;
        }
        else
            m_uiWrathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_isla_starmane(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_FIREWING_POINT_A || pQuest->GetQuestId() == QUEST_ESCAPE_FROM_FIREWING_POINT_H)
    {
        if (npc_isla_starmaneAI* pEscortAI = dynamic_cast<npc_isla_starmaneAI*>(pCreature->AI()))
        {
            pCreature->SetFactionTemporary(pPlayer->GetTeam() == ALLIANCE ? FACTION_ESCORT_A_NEUTRAL_ACTIVE : FACTION_ESCORT_H_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

UnitAI* GetAI_npc_isla_starmane(Creature* pCreature)
{
    return new npc_isla_starmaneAI(pCreature);
}

/*######
## npc_skywing
######*/

enum
{
    SAY_SKYWING_START            = -1000797,
    SAY_SKYWING_TREE_DOWN        = -1000798,
    SAY_SKYWING_TREE_UP          = -1000799,
    SAY_SKYWING_JUMP             = -1000800,
    SAY_SKYWING_SUMMON           = -1000801,
    SAY_SKYWING_END              = -1000802,

    SPELL_FEATHERY_CYCLONE_BURST = 39166,           // triggered many times by server side spell - 39167 (channeled for 5 sec)
    SPELL_RILAK_THE_REDEEMED     = 39179,

    NPC_LUANGA_THE_IMPRISONER    = 18533,

    QUEST_SKYWING                = 10898
};

static const float aLuangaSpawnCoords[3] = { -3507.203f, 4084.619f, 92.947f};

struct npc_skywingAI : public npc_escortAI
{
    npc_skywingAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiCycloneTimer;
    uint8 m_uiCycloneCounter;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiCycloneTimer = 0;
            m_uiCycloneCounter = 0;
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 6:
                DoScriptText(SAY_SKYWING_TREE_DOWN, m_creature);
                break;
            case 36:
                DoScriptText(SAY_SKYWING_TREE_UP, m_creature);
                break;
            case 60:
                DoScriptText(SAY_SKYWING_JUMP, m_creature);
                m_creature->SetLevitate(true);
                break;
            case 61:
                m_creature->SetLevitate(false);
                break;
            case 80:
                DoScriptText(SAY_SKYWING_SUMMON, m_creature);
                m_creature->SummonCreature(NPC_LUANGA_THE_IMPRISONER, aLuangaSpawnCoords[0], aLuangaSpawnCoords[1], aLuangaSpawnCoords[2], 0, TEMPSPAWN_TIMED_OOC_DESPAWN, 30000);
                break;
            case 81:
                // Start transformation
                m_uiCycloneTimer = 100;
                break;
            case 82:
                DoScriptText(SAY_SKYWING_END, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_SKYWING, m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (m_uiCycloneTimer)
        {
            if (m_uiCycloneTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FEATHERY_CYCLONE_BURST) == CAST_OK)
                {
                    ++m_uiCycloneCounter;

                    if (m_uiCycloneCounter == 30)
                        DoCastSpellIfCan(m_creature, SPELL_RILAK_THE_REDEEMED, CAST_TRIGGERED);

                    // Only cast this spell 50 times
                    if (m_uiCycloneCounter == 50)
                        m_uiCycloneTimer = 0;
                    else
                        m_uiCycloneTimer = 100;
                }
            }
            else
                m_uiCycloneTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

bool QuestAccept_npc_skywing(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SKYWING)
    {
        if (npc_skywingAI* pEscortAI = dynamic_cast<npc_skywingAI*>(pCreature->AI()))
        {
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
            DoScriptText(SAY_SKYWING_START, pCreature);

            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

UnitAI* GetAI_npc_skywing(Creature* pCreature)
{
    return new npc_skywingAI(pCreature);
}

/*######
## npc_cenarion_sparrowhawk
######*/

enum
{
    EMOTE_FOLLOW                = -1000963,
    EMOTE_SURVEY                = -1000964,
    EMOTE_LOCATE                = -1000965,

    NPC_SKETTIS_RAVEN_STONE     = 22986,
    GO_RAVEN_STONE              = 185541,
};

struct npc_cenarion_sparrowhawkAI : public ScriptedAI
{
    npc_cenarion_sparrowhawkAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiSurveyTimer;
    bool m_bFirstTimer;

    ObjectGuid m_currentStone;

    void Reset() override
    {
        m_uiSurveyTimer = 3000;
        m_bFirstTimer   = true;
        DoScriptText(EMOTE_FOLLOW, m_creature);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        // despawn the trigger and spawn the nearby stone
        if (Creature* pStoneTrigger = m_creature->GetMap()->GetCreature(m_currentStone))
            pStoneTrigger->ForcedDespawn();

        if (GameObject* pStone = GetClosestGameObjectWithEntry(m_creature, GO_RAVEN_STONE, 5.0f))
        {
            pStone->SetRespawnTime(pStone->GetRespawnDelay());
            pStone->Refresh();
        }
        DoScriptText(EMOTE_LOCATE, m_creature);

        // check if we still have other stones in range
        m_uiSurveyTimer = 5000;
    }

    void DoFindNearbyStones()
    {
        float fX, fY, fZ;
        if (Creature* pStoneTrigger = GetClosestCreatureWithEntry(m_creature, NPC_SKETTIS_RAVEN_STONE, 80.0f))
        {
            m_currentStone = pStoneTrigger->GetObjectGuid();
            pStoneTrigger->GetContactPoint(m_creature, fX, fY, fZ);

            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
        }
        else
            m_creature->ForcedDespawn(10000);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiSurveyTimer)
        {
            if (m_uiSurveyTimer <= uiDiff)
            {
                if (m_bFirstTimer)
                {
                    DoScriptText(EMOTE_SURVEY, m_creature);
                    m_bFirstTimer = false;
                }

                DoFindNearbyStones();
                m_uiSurveyTimer = 0;
            }
            else
                m_uiSurveyTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_cenarion_sparrowhawk(Creature* pCreature)
{
    return new npc_cenarion_sparrowhawkAI(pCreature);
}

/*#####
## npc_skyguard_prisoner
#####*/

enum
{
    SAY_ESCORT_START            = -1001006,
    SAY_AMBUSH_END              = -1001007,
    SAY_ESCORT_COMPLETE         = -1001008,
    SAY_AMBUSH_1                = -1001009,
    SAY_AMBUSH_2                = -1001010,
    SAY_AMBUSH_3                = -1001011,
    SAY_AMBUSH_4                = -1001012,

    NPC_WING_GUARD              = 21644,
    GO_PRISONER_CAGE            = 185952,

    QUEST_ID_ESCAPE_SKETTIS     = 11085,

    SPELL_PORT_LOCATION_1		= 41136,
    SPELL_PORT_LOCATION_2		= 41138,
    SPELL_PORT_LOCATION_3		= 41141,
    SPELL_CAGE_SUMMON			= 41147
};

struct npc_skyguard_prisonerAI : public npc_escortAI
{
    npc_skyguard_prisonerAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() override { }

    void JustRespawned() override
    {
        m_creature->SetActiveObjectState(true);
        switch (urand(0, 2))
        {
            case 0: DoCastSpellIfCan(m_creature, SPELL_PORT_LOCATION_1); break;
            case 1: DoCastSpellIfCan(m_creature, SPELL_PORT_LOCATION_2); break;
            case 2: DoCastSpellIfCan(m_creature, SPELL_PORT_LOCATION_3); break;
        }
        DoCastSpellIfCan(m_creature, SPELL_CAGE_SUMMON);
        m_creature->SetActiveObjectState(false);
        m_creature->SetImmuneToNPC(true); // hack, cage should break los and prevent any interaction with npcs outside the cage befor the escort is started
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*pSender*/, Unit* pInvoker, uint32 uiMiscValue) override
    {
        if (eventType == AI_EVENT_START_ESCORT && pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_ACTIVE, TEMPFACTION_RESTORE_RESPAWN);
            m_creature->SetImmuneToNPC(false); // hack, cage should break los and prevent any interaction with npcs outside the cage befor the escort is started

            Start(false, (Player*)pInvoker, GetQuestTemplateStore(uiMiscValue));

            SetEscortPaused(true);

            if (m_creature->GetPositionZ() < 310.0f)        // -3720.35, 3789.91, 302.888
                SetCurrentWaypoint(20);
            else if (m_creature->GetPositionZ() < 320.0f)   // -3669.57, 3386.74, 312.955
                SetCurrentWaypoint(34);
            else if (m_creature->GetPositionZ() < 350.0f)   // -4106.64, 3029.76, 344.877
                SetCurrentWaypoint(1);

            SetEscortPaused(false);

            // open cage
            if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_PRISONER_CAGE, 10.0f))
                pCage->Use(m_creature);
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_WING_GUARD)
        {
            pSummoned->AI()->AttackStart(m_creature);

            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_AMBUSH_1, pSummoned); break;
                case 1: DoScriptText(SAY_AMBUSH_2, pSummoned); break;
                case 2: DoScriptText(SAY_AMBUSH_3, pSummoned); break;
                case 3: DoScriptText(SAY_AMBUSH_4, pSummoned); break;
            }
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
            case 20:
            case 34:
                DoScriptText(SAY_ESCORT_START, m_creature);
                break;

            case 13:
                m_creature->SummonCreature(NPC_WING_GUARD, -4179.043f, 3081.007f, 328.28f, 4.51f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_WING_GUARD, -4181.610f, 3081.289f, 328.32f, 4.52f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 27:
                m_creature->SummonCreature(NPC_WING_GUARD, -3653.75f, 3750.8f, 302.101f, 2.11185f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_WING_GUARD, -3649.91f, 3754.08f, 303.007f, 2.3911f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;
            case 38:
                m_creature->SummonCreature(NPC_WING_GUARD, -3680.32f, 3318.81f, 311.501f, 1.55334f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                m_creature->SummonCreature(NPC_WING_GUARD, -3677.91f, 3317.93f, 311.573f, 1.48353f, TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 60000);
                break;

            case 14:
            case 28:
            case 39:
                DoScriptText(SAY_AMBUSH_END, m_creature);
                break;

            case 18:
            case 32:
            case 45:
                DoScriptText(SAY_ESCORT_COMPLETE, m_creature);
                SetRun();

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RewardPlayerAndGroupAtEventExplored(QUEST_ID_ESCAPE_SKETTIS, m_creature);

                break;

            case 19:
            case 33:
            case 46:
                m_creature->ForcedDespawn();
                break;
        }
    }
};

UnitAI* GetAI_npc_skyguard_prisoner(Creature* pCreature)
{
    return new npc_skyguard_prisonerAI(pCreature);
}

bool QuestAccept_npc_skyguard_prisoner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_ESCAPE_SKETTIS)
    {
        pCreature->AI()->SendAIEvent(AI_EVENT_START_ESCORT, pPlayer, pCreature, pQuest->GetQuestId());
        return true;
    }

    return false;
}

enum
{
    SPELL_TERRIFYING_SCREECH    = 38021,
    SPELL_FEATHER_BURST         = 39068,

    SAY_DEFENDER_GRASHNA        = -1001241,

    NPC_DEFENDER_GRASHNA        = 22373,

    QUEST_SKETTIS_OFFENSIVE     = 10879,
};

void AttackPlayerWithQuest(Creature* creature)
{
    PlayerList playerList;
    GetPlayerListWithEntryInWorld(playerList, creature, 50.0f);
    for (auto& player : playerList)
    {
        if (player->IsActiveQuest(QUEST_SKETTIS_OFFENSIVE))
        {
            creature->AI()->AttackStart(player);
            return;
        }
    }
}

struct npc_avatar_of_terokkAI : public ScriptedAI
{
    npc_avatar_of_terokkAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAbilityTimer;

    void Reset() override
    {
        m_uiAbilityTimer = 4000;
    }

    void JustRespawned() override
    {
        AttackPlayerWithQuest(m_creature);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (Creature* grashna = GetClosestCreatureWithEntry(m_creature, NPC_DEFENDER_GRASHNA, 100.0f))
        {
            DoScriptText(SAY_DEFENDER_GRASHNA, grashna);

            PlayerList playerList;
            GetPlayerListWithEntryInWorld(playerList, m_creature, 50.0f);
            for (auto& player : playerList)
                if (player->IsActiveQuest(QUEST_SKETTIS_OFFENSIVE))
                    player->RewardPlayerAndGroupAtEventExplored(QUEST_SKETTIS_OFFENSIVE, m_creature);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiAbilityTimer <= uiDiff)
        {
            m_uiAbilityTimer = 12000;
            m_creature->CastSpell(m_creature->GetVictim(), SPELL_FEATHER_BURST, TRIGGERED_NONE);
            return;
        }
        m_uiAbilityTimer -= uiDiff;
        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_avatar_of_terokk(Creature* pCreature)
{
    return new npc_avatar_of_terokkAI(pCreature);
}

struct npc_minion_of_terokkAI : public ScriptedAI
{
    npc_minion_of_terokkAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiAbilityTimer;

    void Reset() override
    {
        m_uiAbilityTimer = 4000;
    }

    void JustRespawned() override
    {
        AttackPlayerWithQuest(m_creature);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiAbilityTimer <= uiDiff)
        {
            m_uiAbilityTimer = 15000;
            m_creature->CastSpell(m_creature->GetVictim(), SPELL_TERRIFYING_SCREECH, TRIGGERED_NONE);
            return;
        }
        m_uiAbilityTimer -= uiDiff;
        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_minion_of_terokk(Creature* pCreature)
{
    return new npc_minion_of_terokkAI(pCreature);
}

/*######
## go_monstrous_kaliri_egg
######*/

struct go_monstrous_kaliri_egg : public GameObjectAI
{
    go_monstrous_kaliri_egg(GameObject* go) : GameObjectAI(go) {}

    void OnLootStateChange() override
    {
        if (m_go->GetLootState() == GO_ACTIVATED)
            m_go->SetForcedDespawn();
    }
};

GameObjectAI* GetAI_go_monstrous_kaliri_egg(GameObject* go)
{
    return new go_monstrous_kaliri_egg(go);
}

struct ShadowyDisguise : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        Unit* target = aura->GetTarget();
        if (!apply)
            target->RemoveAurasDueToSpell(target->getGender() == GENDER_MALE ? 38080 : 38081);
    }
};

void AddSC_terokkar_forest()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "mob_unkor_the_ruthless";
    pNewScript->GetAI = &GetAI_mob_unkor_the_ruthless;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_netherweb_victim";
    pNewScript->GetAI = &GetAI_mob_netherweb_victim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_akuno";
    pNewScript->GetAI = &GetAI_npc_akuno;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_akuno;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hungry_nether_ray";
    pNewScript->GetAI = &GetAI_npc_hungry_nether_ray;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_letoll";
    pNewScript->GetAI = &GetAI_npc_letoll;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_letoll;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_isla_starmane";
    pNewScript->GetAI = &GetAI_npc_isla_starmane;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_isla_starmane;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_skywing";
    pNewScript->GetAI = &GetAI_npc_skywing;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_skywing;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cenarion_sparrowhawk";
    pNewScript->GetAI = &GetAI_npc_cenarion_sparrowhawk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_skyguard_prisoner";
    pNewScript->GetAI = &GetAI_npc_skyguard_prisoner;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_skyguard_prisoner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_avatar_of_terokk";
    pNewScript->GetAI = &GetAI_npc_avatar_of_terokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_minion_of_terokk";
    pNewScript->GetAI = &GetAI_npc_minion_of_terokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_monstrous_kaliri_egg";
    pNewScript->GetGameObjectAI = &GetAI_go_monstrous_kaliri_egg;
    pNewScript->RegisterSelf();

    RegisterSpellScript<ShadowyDisguise>("spell_shadowy_disguise");
}
